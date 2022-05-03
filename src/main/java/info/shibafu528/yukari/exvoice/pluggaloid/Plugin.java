package info.shibafu528.yukari.exvoice.pluggaloid;

import android.content.Context;
import android.text.TextUtils;

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

import info.shibafu528.yukari.exvoice.MRuby;
import info.shibafu528.yukari.exvoice.MRubyException;

/**
 * {@code Pluggaloid::Plugin} と対応する機能を提供します。
 *
 * Created by shibafu on 2016/04/10.
 */
public abstract class Plugin {
    static {
        System.loadLibrary("exvoice");
    }

    private final String slug;
    private MRuby mRuby;
    private Map<String, PluggaloidEventListener> listeners = new HashMap<>();
    private Map<String, PluggaloidEventFilter> filters = new HashMap<>();
    private Map<String, PluggaloidSpellListener> spells = new HashMap<>();

    /**
     * プラグインの初期化を行います。サブクラスではここでslugを決定し、{@link MRuby} のみを引数にとる必要があります。
     * @param mRuby MRubyインスタンス
     * @param slug 識別名 (slug)
     */
    public Plugin(MRuby mRuby, String slug) {
        this.mRuby = mRuby;
        this.slug = slug;
        initialize();
    }

    /**
     * {@code Pluggaloid::Plugin.call} を呼び出します。
     * @param mRuby {@link MRuby} のインスタンス
     * @param eventName イベント名
     * @param args 引数
     * @exception MRubyException MRuby上で例外が発生した場合、この例外でラップされます。
     */
    public static native void call(MRuby mRuby, String eventName, Object... args);

    /**
     * {@code Pluggaloid::Plugin.filtering} を呼び出し、引数 args をフィルタリングした結果を返します。
     * @param mRuby {@link MRuby} のインスタンス
     * @param eventName イベント名
     * @param args 引数
     * @return フィルタされた結果の配列
     * @exception FilterException 引数の数が途中で変化した場合に発生します。
     * @exception MRubyException MRuby上で例外が発生した場合、この例外でラップされます。
     */
    public static native Object[] filtering(MRuby mRuby, String eventName, Object... args);

    /**
     * プラグインの識別名(slug)を取得します。
     * @return slug
     */
    public String getSlug() {
        return slug;
    }

    /**
     * イベントが発生した際、ネイティブのコールバック関数から呼び出されます。
     * @param eventName イベント名
     * @param args 引数
     */
    /*package*/ void onEvent(String eventName, Object... args) {
        PluggaloidEventListener l = listeners.get(eventName);
        if (l != null) {
            l.onEvent(args);
        }
    }

    /**
     * イベントフィルタが呼びだされた際、ネイティブのコールバック関数から呼び出されます。
     * @param eventName イベント名
     * @param args 引数
     * @return フィルタされた結果の配列
     */
    /*package*/ Object[] filter(String eventName, Object... args) {
        PluggaloidEventFilter f = filters.get(eventName);
        if (f != null) {
            return f.filter(args);
        } else {
            return args;
        }
    }

    /**
     * Spellが呼び出された際、ネイティブのコールバック関数から呼び出されます。
     * @param callbackKey コールバックリスナーのキー
     * @param models Model引数
     * @param options キーワード引数
     */
    /*package*/ void onSpell(String callbackKey, Object[] models, Map<String, Object> options) {
        PluggaloidSpellListener l = spells.get(callbackKey);
        if (l != null) {
            l.onSpell(models, options);
        }
    }

    /**
     * このプラグインを登録しているMRubyインスタンスを取得します。
     * @return mruby
     */
    protected MRuby getMRuby() {
        return mRuby;
    }

    /**
     * MRubyを所有しているContextを取得します。
     * @return Context
     */
    protected Context getContext() {
        return mRuby.getContext();
    }

    /**
     * Pluggaloidイベントを受信するリスナを登録します。同じ名前のイベントに対して複数登録することはできません。
     * @param eventName イベント名
     * @param listener リスナ
     */
    protected void addEventListener(String eventName, PluggaloidEventListener listener) {
        if (listeners.get(eventName) == null) {
            listeners.put(eventName, listener);
            addEventListenerNative(eventName);
        }
    }

    /**
     * Pluggaloidイベントフィルタを処理するリスナを登録します。同じ名前のフィルタに対して複数登録することはできません。
     * @param eventName イベント名
     * @param filter フィルタ
     */
    protected void addEventFilter(String eventName, PluggaloidEventFilter filter) {
        if (filters.get(eventName) == null) {
            filters.put(eventName, filter);
            addEventFilterNative(eventName);
        }
    }

    /**
     * Spellを定義し、リスナを紐付けます。同じ名前、同じ制約のSpellに対して複数のリスナを登録することはできません。
     * @param spellName Spell名
     * @param constraints 引数となるModelのslugの配列
     * @param listener リスナ
     */
    protected void defineSpell(String spellName, String[] constraints, PluggaloidSpellListener listener) {
        if (constraints == null || constraints.length == 0) {
            throw new IllegalArgumentException("constraints must have one or more values");
        }

        StringBuilder keyBuilder = new StringBuilder(spellName);
        for (String constraint : constraints) {
            keyBuilder.append('$');
            keyBuilder.append(constraint);
        }

        String key = keyBuilder.toString();
        if (spells.get(key) == null) {
            spells.put(key, listener);
            defineSpellNative(spellName, constraints, key);
        }
    }

    /**
     * プラグインの初期化を行います。また、ここでアノテーションの付いたコールバックを有効にします。
     */
    private void initialize() {
        // Pluggaloidでのプラグイン宣言とコールバックの登録
        initializeNative();

        // イベントの登録
        for (final Method method : getClass().getDeclaredMethods()) {
            if (method.isAnnotationPresent(Event.class)) {
                Event annotation = method.getAnnotation(Event.class);
                String eventName;
                if (TextUtils.isEmpty(annotation.value())) {
                    eventName = method.getName();
                } else {
                    eventName = annotation.value();
                }

                addEventListener(eventName, new AnnotatedEventListener(this, method));
            } else if (method.isAnnotationPresent(Filter.class)) {
                Filter annotation = method.getAnnotation(Filter.class);
                String eventName;
                if (TextUtils.isEmpty(annotation.value())) {
                    eventName = method.getName();
                } else {
                    eventName = annotation.value();
                }

                addEventFilter(eventName, new AnnotatedEventFilter(this, method));
            } else if (method.isAnnotationPresent(Spell.class)) {
                Spell annotation = method.getAnnotation(Spell.class);
                if (TextUtils.isEmpty(annotation.value())) {
                    throw new IllegalArgumentException(String.format(Locale.US, "%s() のSpellアノテーションにSpell名が設定されていません。", method.getName()));
                }

                defineSpell(annotation.value(), annotation.constraints(), new AnnotatedSpellListener(this, method, annotation));
            }
        }
    }

    /**
     * ネイティブに降りて、Pluggaloidプラグインの初期化を行います。
     */
    private native void initializeNative();

    /**
     * ネイティブに降りて、Pluggaloidプラグインにイベントリスナを登録します。
     * @param eventName イベント名
     */
    private native void addEventListenerNative(String eventName);

    /**
     * ネイティブに降りて、Pluggaloidプラグインにイベントフィルタを登録します。
     * @param eventName
     */
    private native void addEventFilterNative(String eventName);

    private native void defineSpellNative(String spellName, String[] constraints, String callbackKey);

    public interface PluggaloidEventListener {
        void onEvent(Object... args);
    }

    public interface PluggaloidEventFilter {
        Object[] filter(Object... args);
    }

    public interface PluggaloidSpellListener {
        void onSpell(Object[] models, Map<String, Object> options);
    }
}
