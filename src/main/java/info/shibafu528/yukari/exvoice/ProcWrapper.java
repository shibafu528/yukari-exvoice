package info.shibafu528.yukari.exvoice;

/**
 * MRubyのProcをラップしてJavaから実行できるようにします。
 *
 * Created by shibafu on 2016/05/05.
 */
public class ProcWrapper implements MRubyPointer {

    private MRuby mRuby;
    private long rProcPointer;
    private boolean disposed;

    /*package*/ ProcWrapper(MRuby mRuby, long rProcPointer) {
        this.mRuby = mRuby;
        this.rProcPointer = rProcPointer;
    }

    /**
     * ブロックをMRuby上で評価します。
     * @param args ブロックに渡す引数
     * @return ブロックの返り値
     * @exception MRubyException MRuby上で例外が発生した場合、この例外でラップされます。
     */
    public Object exec(Object... args) {
        return execWithContext(this.mRuby, args);
    }

    /**
     * MRubyのインスタンスを明示的に指定し、ブロックをMRuby上で評価します。
     * @param mRuby {@link MRuby} のインスタンス
     * @param args ブロックに渡す引数
     * @return ブロックの返り値
     * @exception MRubyException MRuby上で例外が発生した場合、この例外でラップされます。
     * @exception IllegalStateException 引数 mRuby が null、またはクローズされている場合にスローされます。
     */
    public Object execWithContext(MRuby mRuby, Object... args) {
        if (mRuby == null || mRuby.getMRubyInstancePointer() == 0) {
            throw new IllegalStateException("無効なMRubyインスタンスが指定されているため、実行できません。");
        }

        synchronized (mRuby.getMutex()) {
            return execNative(mRuby.getMRubyInstancePointer(), args);
        }
    }

    @Override
    public long getPointer() {
        return rProcPointer;
    }

    public void dispose() {
        if (!disposed && mRuby != null && mRuby.getMRubyInstancePointer() != 0) {
            disposeNative(mRuby.getMRubyInstancePointer());
            rProcPointer = 0;
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

    private native void disposeNative(long mRubyInstancePointer);

    private native Object execNative(long mRubyInstancePointer, Object... args);
}
