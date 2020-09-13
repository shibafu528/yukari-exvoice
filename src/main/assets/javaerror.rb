class JavaError < StandardError
  attr_reader :throwable
  
  # @param [JavaThrowable] exc
  def initialize(throwable)
    @throwable = throwable
  end

  def to_s
    @throwable.to_s
  end

  def inspect
    msg = @throwable.to_s
    if msg && msg != ''
      "#{msg} (#{self.class.name})"
    else
      self.class.name
    end
  end
end
