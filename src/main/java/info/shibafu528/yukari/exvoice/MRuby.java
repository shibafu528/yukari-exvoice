package info.shibafu528.yukari.exvoice;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import info.shibafu528.yukari.exvoice.pluggaloid.Plugin;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;

/**
 * Created by shibafu on 2016/03/28.
 */
public class MRuby {
    static {
        System.loadLibrary("exvoice");
    }

    private final Object mutex = new Object();
    private long mrubyInstancePointer;
    private Context context;
    private Handler delayerHandler;
    private AssetManager assetManager;
    private PrintCallback printCallback;
    private Map<String, Plugin> plugins = new HashMap<>();

    private ConfigLoader configLoader;

    /**
     * MRubyのVMを初期化し、使用可能な状態にします。  
     * @param context
     */
    public MRuby(Context context) {
        this(context, new Handler(Looper.getMainLooper()));
    }

    /**
     * MRubyのVMを初期化し、使用可能な状態にします。
     * @param context
     * @param delayerHandler Delayerループを実行するHandler
     */
    public MRuby(Context context, Handler delayerHandler) {
        this.mrubyInstancePointer = n_open();
        this.context = context;
        this.delayerHandler = delayerHandler;
        this.assetManager = context.getAssets();

        this.configLoader = new ConfigLoader(this);
    }

    /**
     * MRubyのVMをシャットダウンします。
     */
    public void close() {
        if (mrubyInstancePointer == 0) {
            throw new IllegalStateException("MRuby VM was already closed.");
        }
        n_close(mrubyInstancePointer);
        mrubyInstancePointer = 0;
    }

    /**
     * 引数として渡された文字列をRubyプログラムとしてトップレベルのコンテキストで実行します。
     * @param code Rubyプログラム
     */
    public void loadString(String code) {
        loadString(code, true);
    }

    /**
     * 引数として渡された文字列をRubyプログラムとしてトップレベルのコンテキストで実行します。
     * @param code Rubyプログラム
     * @param echo 入力をLogcat上にエコーします
     */
    public void loadString(String code, boolean echo) {
        synchronized (mutex) {
            n_loadString(mrubyInstancePointer, code, echo);
        }
    }

    /**
     * 指定の名前のメソッドをトップレベルから検索し実行します。
     * @param name メソッド名
     * @return メソッドの返り値
     */
    public Object callTopLevelFunc(String name) {
        synchronized (mutex) {
            return n_callTopLevelFunc(mrubyInstancePointer, name);
        }
    }

    /**
     * 指定の名前のメソッドをトップレベルから検索し実行します。
     * @param name メソッド名
     */
    public void callTopLevelProc(String name) {
        synchronized (mutex) {
            n_callTopLevelProc(mrubyInstancePointer, name);
        }
    }

    /**
     * プラグインを登録し、使用可能な状態にします。
     * @param clazz プラグインクラス
     */
    public void registerPlugin(Class<? extends Plugin> clazz) {
        try {
            Constructor<? extends Plugin> constructor = clazz.getConstructor(MRuby.class);
            Plugin plugin = constructor.newInstance(this);
            plugins.put(plugin.getSlug(), plugin);
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InstantiationException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }
    }

    public void setPrintCallback(PrintCallback printCallback) {
        this.printCallback = printCallback;
    }

    public void printStringCallback(String value) {
        if (printCallback != null) {
            printCallback.print(value);
        } else {
            Log.d("exvoice(Java)", value);
        }
    }

    public long getMRubyInstancePointer() {
        return mrubyInstancePointer;
    }

    public Context getContext() {
        return context;
    }

    public Handler getDelayerHandler() {
        return delayerHandler;
    }

    public Object getMutex() {
        return mutex;
    }

    public Plugin getPlugin(String slug) {
        return plugins.get(slug);
    }

    private void delayerRemainCallback() {
        delayerHandler.post(() -> {
            Log.d("exvoice(Java)", "delayerRemainCallback() -> n_runDelayer()");
            n_runDelayer(mrubyInstancePointer);
        });
    }

    private void delayerReserveCallback(final double delay) {
        delayerHandler.postDelayed(() -> {
            Log.d("exvoice(Java)", String.format(Locale.US, "delayerReserveCallback(%.3f) -> n_runDelayer()", delay));
            n_runDelayer(mrubyInstancePointer);
        }, (long) (delay * 1000) + 100); // TODO: なにやら誤差でうまく動かないこともある
    }

    private native long n_open();
    private native void n_close(long mrb);

    private native void n_loadString(long mrb, String code, boolean echo);
    private native Object n_callTopLevelFunc(long mrb, String name);
    private native void n_callTopLevelProc(long mrb, String name);
    private native void n_runDelayer(long mrb);

    public interface PrintCallback {
        void print(String value);
    }
}
