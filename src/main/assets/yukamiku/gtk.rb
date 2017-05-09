# encoding: utf-8
# core/plugin/gtk 互換プラグイン

module Plugin::Gtk
  class GtkError < StandardError; end
end

Plugin.create(:gtk) do
  # 互換クラスのインスタンスを保持する
  @pseudo_instances = {
      postbox: Plugin::GUI::Postbox.instance,
      timeline: Plugin::GUI::Timeline.instance
  }

  def widgetof(slug_or_instance)
    if slug_or_instance.is_a? Symbol
      @pseudo_instances[slug_or_instance]
    else
      @pseudo_instances[slug_or_instance.class.to_s.split('::').last.downcase.to_sym]
    end
  end
end