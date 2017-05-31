package info.shibafu528.yukari.exvoice;

import com.google.common.base.CaseFormat;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.HashMap;
import java.util.Map;

/**
 * MRuby上のオブジェクトをラップし、呼び出しを中継するためのハンドラです。
 *
 * Created by shibafu on 2017/05/30.
 */
public class MRubyObjectHandler implements InvocationHandler, MRubyPointer {
    private static final Map<String, String> JAVA_TO_RUBY_METHOD_CONVERT_MAP = new HashMap<>();

    static {
        JAVA_TO_RUBY_METHOD_CONVERT_MAP.put("equals", "==");
        JAVA_TO_RUBY_METHOD_CONVERT_MAP.put("toString", "to_s");
        JAVA_TO_RUBY_METHOD_CONVERT_MAP.put("compareTo", "<=>");
    }

    private MRuby mRuby;
    private long mrbObjectPointer;
    private boolean disposed;

    /**
     * MRuby上のオブジェクトへアクセスするためのプロキシを生成します。
     * @param mRuby {@link MRuby} のインスタンス
     * @param mrbObjectPointer オブジェクトのアドレス、これを呼び出す前にMRuby上でGCされないよう必ず保護すること。
     * @param iface プロキシインターフェースのクラス情報
     * @param <T> プロキシインターフェースの型
     * @return 指定のオブジェクトをレシーバとした呼び出しが可能なプロキシのインスタンス
     */
    @SuppressWarnings("unchecked")
    public static <T> T create(MRuby mRuby, long mrbObjectPointer, final Class<T> iface) {
        return (T) Proxy.newProxyInstance(iface.getClassLoader(), new Class<?>[] { iface }, new MRubyObjectHandler(mRuby, mrbObjectPointer));
    }

    private MRubyObjectHandler(MRuby mRuby, long mrbObjectPointer) {
        this.mRuby = mRuby;
        this.mrbObjectPointer = mrbObjectPointer;
    }

    @Override
    public Object invoke(Object proxy, Method method, Object[] args) throws Throwable {
        final String methodName = method.getName();
        if (method.getDeclaringClass() == MRubyPointer.class) {
            return method.invoke(this, args);
        } else {
            return invokeNative(mRuby.getMRubyInstancePointer(), mrbObjectPointer, convertMethodNameToRuby(methodName), args);
        }
    }

    @Override
    public long getPointer() {
        return mrbObjectPointer;
    }

    @Override
    public void dispose() {
        if (!disposed && mRuby != null && mRuby.getMRubyInstancePointer() != 0) {
            disposeNative(mRuby.getMRubyInstancePointer(), mrbObjectPointer);
            mrbObjectPointer = 0;
            disposed = true;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            super.finalize();
        } finally {
            dispose();
        }
    }

    /**
     * Javaの命名規約に則ったメソッド名を、Rubyっぽいものに変換します。
     * @param methodName メソッド名 (Java系)
     * @return メソッド名 (Ruby系)
     */
    private String convertMethodNameToRuby(String methodName) {
        if (JAVA_TO_RUBY_METHOD_CONVERT_MAP.containsKey(methodName)) {
            return JAVA_TO_RUBY_METHOD_CONVERT_MAP.get(methodName);
        } else {
            // is* -> *?
            if (methodName.startsWith("is")) {
                methodName = methodName.substring(2) + "?";
            }
            // get* -> (remove)
            if (methodName.startsWith("get")) {
                methodName = methodName.substring(3);
            }
            // set* -> *=
            if (methodName.startsWith("set")) {
                methodName = methodName.substring(3) + "=";
            }
            // *Unsafe -> *!
            if (methodName.endsWith("Unsafe")) {
                methodName = methodName.substring(0, methodName.length() - 6) + "!";
            }
            // camelCase -> snake_case
            methodName = CaseFormat.LOWER_CAMEL.to(CaseFormat.LOWER_UNDERSCORE, methodName);

            return methodName;
        }
    }

    private native Object invokeNative(long mRubyInstancePointer, long mrbObjectPointer, String methodName, Object[] args);

    private native void disposeNative(long mRubyInstancePointer, long mrbObjectPointer);
}
