class JavaError < StandardError
  attr_reader :throwable
  
  # @param [JavaThrowable] exc
  def initialize(throwable)
    @throwable = throwable
  end

  def to_s
    @throwable.to_s
  end
end
