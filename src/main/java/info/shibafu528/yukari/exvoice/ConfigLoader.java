package info.shibafu528.yukari.exvoice;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * core/configloader.rb の機能を、{@link android.content.SharedPreferences} を用いて実装したクラス。
 */
public class ConfigLoader {
    private static final String PREF_NAME = "exvoice_settings";

    private MRuby mRuby;
    private SharedPreferences preferences;

    ConfigLoader(MRuby mRuby) {
        this.mRuby = mRuby;
        this.preferences = mRuby.getContext().getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE);
        initializeNative();
    }

    public boolean containsKey(String key) {
        return preferences.getAll().containsKey(key);
    }

    public int getInt(String key) {
        return preferences.getInt(key, 0);
    }

    public long getLong(String key) {
        return preferences.getLong(key, 0);
    }

    public float getFloat(String key) {
        return preferences.getFloat(key, 0);
    }

    public boolean getBoolean(String key) {
        return preferences.getBoolean(key, false);
    }

    public String getString(String key) {
        return preferences.getString(key, "");
    }

    public void putInt(String key, int value) {
        preferences.edit().putInt(key, value).apply();
    }

    public void putLong(String key, long value) {
        preferences.edit().putLong(key, value).apply();
    }

    public void putFloat(String key, float value) {
        preferences.edit().putFloat(key, value).apply();
    }

    public void putBoolean(String key, boolean value) {
        preferences.edit().putBoolean(key, value).apply();
    }

    public void putString(String key, String value) {
        preferences.edit().putString(key, value).apply();
    }

    /**
     * ConfigLoaderモジュールにインスタンスを登録します。
     */
    private native void initializeNative();
}
