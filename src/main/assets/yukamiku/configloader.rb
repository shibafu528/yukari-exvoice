# encoding: utf-8

# ConfigLoader互換モジュール
module ConfigLoader
  AVAILABLE_TYPES = [Hash, Array, Set, Numeric, String, Symbol, Time, NilClass, TrueClass, FalseClass].freeze

  class << self

    # _obj_ が保存可能な値なら _obj_ を返す。そうでなければ _ArgumentError_ 例外を投げる。
    def validate(obj)
      if AVAILABLE_TYPES.any?{|x| obj.is_a?(x)}
        if obj.is_a? Hash
          result = {}
          obj.each{ |key, value|
            result[self.validate(key)] = self.validate(value) }
          result.freeze
        elsif obj.is_a? Enumerable
          obj.map(&method(:validate)).freeze
        elsif not(obj.freezable?) or obj.frozen?
          obj
        else
          obj.dup.freeze end
      else
        emes = "ConfigLoader recordable class of #{AVAILABLE_TYPES.join(',')} only. but #{obj.class} given."
        error(emes)
        raise ArgumentError.new(emes)
      end
    end

  end

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

  private

  def configloader_key(key)
    "#{self.class.to_s}::#{key}".freeze end
  # memoize :configloader_key
end