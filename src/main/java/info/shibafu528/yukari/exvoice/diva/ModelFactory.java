package info.shibafu528.yukari.exvoice.diva;

import info.shibafu528.yukari.exvoice.MRuby;
import info.shibafu528.yukari.exvoice.MRubyObjectHandler;

import java.util.Map;

/**
 * Diva::Modelの新規インスタンスをJavaから作成するためのクラスです。
 *
 * Created by shibafu on 2017/05/31.
 */
public class ModelFactory {

    /**
     * Diva::Modelの新規インスタンスを作成します。
     * @param modelInterface ModelのJava側インターフェース
     * @param modelClassName ModelのRuby側完全クラス名
     * @param values コンストラクタ引数として与える値
     * @param <T> ModelのJava側インターフェース
     * @return 生成したインスタンスに対応するプロキシ
     */
    public static <T extends Model> T newInstance(MRuby mRuby, Class<T> modelInterface, String modelClassName, Map<String, Object> values) {
        long mrbObjectPointer = newInstanceNative(mRuby.getMRubyInstancePointer(), modelClassName, values);
        return MRubyObjectHandler.create(mRuby, mrbObjectPointer, modelInterface);
    }

    private static native long newInstanceNative(long mRubyInstancePointer, String modelClassName, Map<String, Object> values);
}
