package info.shibafu528.yukari.exvoice.miquire;

import info.shibafu528.yukari.exvoice.MRuby;

/**
 * {@code Mix::Miquire} と対応する機能を提供します。
 */
public class Miquire {
    static {
        System.loadLibrary("exvoice");
    }

    /**
     * {@code Mix::Miquire.load_all} を呼び出します。
     * @param mRuby MRubyインスタンス
     * @return 読み込み結果
     */
    public static MiquireResult loadAll(MRuby mRuby) {
        return loadAll(mRuby, false);
    }

    /**
     * {@code Mix::Miquire.load_all} を呼び出します。
     * @param mRuby MRubyインスタンス
     * @param failfast 読み込みに失敗した場合、速やかに {@link info.shibafu528.yukari.exvoice.MRubyException} をスローするかどうか
     * @return 読み込み結果
     */
    public static MiquireResult loadAll(MRuby mRuby, boolean failfast) {
        String[][] result = n_loadAll(mRuby.getMRubyInstancePointer(), failfast);
        return new MiquireResult(result[0], result[1]);
    }

    /**
     * {@code Mix::Miquire.load_path} の末尾にエントリを追加します。
     * @param mRuby MRubyインスタンス
     * @param path ロードパス
     */
    public static void appendLoadPath(MRuby mRuby, String path) {
        n_appendLoadPath(mRuby.getMRubyInstancePointer(), path);
    }

    private static native String[][] n_loadAll(long mrb, boolean failfast);
    private static native void n_appendLoadPath(long mrb, String path);
}
