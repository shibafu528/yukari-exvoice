# encoding: utf-8
# 様々なその場しのぎのコード片

# ================

# Stringクラスの不足埋め合わせ
class String
  def ascii_only?
    !(self =~ /[^\x00-\x7f]/)
  end unless method_defined? :ascii_only?
end

# ================

# mruby-printのRubyコードを再定義
# Kernel#putsが期待通りに動作してくれない版が存在するため、そのための処置
module Kernel

  def print(*args)
    i = 0
    len = args.size
    while i < len
      __printstr__ args[i].to_s
      i += 1
    end
  end

  def puts(*args)
    i = 0
    len = args.size
    while i < len
      s = args[i].to_s
      __printstr__ s
      __printstr__ "\n" if (s[-1] != "\n")
      i += 1
    end
    __printstr__ "\n" if len == 0
    nil
  end

  def p(*args)
    i = 0
    len = args.size
    while i < len
      __printstr__ args[i].inspect
      __printstr__ "\n"
      i += 1
    end
    args[0]
  end

  unless Kernel.respond_to?(:sprintf)
    def printf(*args)
      raise NotImplementedError.new('printf not available')
    end
    def sprintf(*args)
      raise NotImplementedError.new('sprintf not available')
    end
  else
    def printf(*args)
      __printstr__(sprintf(*args))
      nil
    end
  end
end
