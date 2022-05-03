package info.shibafu528.yukari.exvoice;

import android.content.Context;
import android.content.res.AssetManager;
import android.os.Handler;
import android.os.Looper;
import info.shibafu528.yukari.exvoice.pluggaloid.Plugin;

import java.io.File;
import java.io.FileNotFoundException;
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
     * Rubyスクリプトを読み込みます。
     * @param file Rubyスクリプトファイル
     * @throws FileNotFoundException ファイルが存在しない場合にスロー
     * @return 読み込みに成功すればtrue, 何らかの理由で失敗した場合はfalse
     */
    public boolean require(File file) throws FileNotFoundException {
        synchronized (mutex) {
            if (!file.exists()) {
                throw new FileNotFoundException("File not found: " + file.getAbsolutePath());
            }

            return n_require(mrubyInstancePointer, file.getAbsolutePath());
        }
    }

    /**
     * AssetsからRubyスクリプトを読み込みます。
     * @param path Assets内のファイル名
     */
    public void requireAssets(String path) {
        synchronized (mutex) {
            n_requireAssets(mrubyInstancePointer, path);
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
            throw new RuntimeException(e);
        } catch (IllegalAccessException e) {
            throw new RuntimeException(e);
        } catch (InstantiationException e) {
            throw new RuntimeException(e);
        } catch (InvocationTargetException e) {
            Throwable cause = e.getCause();
            if (cause instanceof Error) {
                throw (Error) cause;
            } else if (cause instanceof RuntimeException) {
                throw (RuntimeException) cause;
            } else {
                throw new RuntimeException(cause);
            }
        }
    }

    public void setPrintCallback(PrintCallback printCallback) {
        this.printCallback = printCallback;
    }

    public void printStringCallback(String value) {
        if (printCallback != null) {
            printCallback.print(value);
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
            if (mrubyInstancePointer == 0) {
                return;
            }
            n_runDelayer(mrubyInstancePointer);
        });
    }

    private void delayerReserveCallback(final double delay) {
        delayerHandler.postDelayed(() -> {
            if (mrubyInstancePointer == 0) {
                return;
            }
            n_runDelayer(mrubyInstancePointer);
        }, (long) (delay * 1000) + 100); // TODO: なにやら誤差でうまく動かないこともある
    }

    private native long n_open();
    private native void n_close(long mrb);

    private native void n_loadString(long mrb, String code, boolean echo);
    private native Object n_callTopLevelFunc(long mrb, String name);
    private native void n_callTopLevelProc(long mrb, String name);
    private native void n_runDelayer(long mrb);
    private native boolean n_require(long mrb, String path);
    private native void n_requireAssets(long mrb, String path);

    public interface PrintCallback {
        void print(String value);
    }
}
