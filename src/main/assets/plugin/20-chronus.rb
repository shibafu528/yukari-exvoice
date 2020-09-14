# encoding: utf-8

# periodイベントの供給
Plugin.create(:chronus) do
  def emit_period
    Delayer.new(delay: 1) do
      Plugin.call(:period)
    end
  end

  on_period(&method(:emit_period))

  emit_period
end
