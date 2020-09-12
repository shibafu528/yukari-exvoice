# encoding: utf-8

# ConfigLoader プラットフォーム依存部分の再定義
module ConfigLoader

  # キーに対応する値が存在するかを調べる。
  # @param [Symbol] key キー
  # @return [Boolean] _key_ が存在していれば _true_ を返す
  def include?(key)
    _jni_containsKey configloader_key(key)
  end

  # キーに対応するオブジェクトを取り出す。
  # @param [Symbol] key キー
  # @param [Object] ifnone _key_ が存在しない場合に返す値
  # @return [Object] _key_ に対応するオブジェクト、それが存在しない場合は nil か _ifnone_ の値
  def at(key, ifnone=nil)
    ckey = configloader_key(key)
    if _jni_containsKey ckey
      JSON::parse _jni_getString(ckey)
    else
      ifnone
    end
  end

  # キーに対してオブジェクトを関連付ける。
  # @param [Symbol] key キー
  # @param [Object] val オブジェクト
  # @return [Object] _val_
  def store(key, val)
    ConfigLoader.validate(key)
    val = ConfigLoader.validate(val)
    ckey = configloader_key(key)
    _jni_putString(ckey, JSON::stringify(val))
  end
end