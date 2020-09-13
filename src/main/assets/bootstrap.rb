# encoding: utf-8
# Yukari; exVoice
# mrubyとPluggaloidを使用したモバイル向けプラグインサブシステム

Android.require_assets 'avoid.rb'
Android.require_assets 'javaerror.rb'

def tick
  begin
    $last_ticked ||= Time.now
    $tick_counter = $tick_counter + 1

    span = Time.now.to_f - $last_ticked.to_f
    Android::Log.d "yukari-exvoice: tick! +#{span} sec, #{Delayer.size} job(s)" if span >= 0.6 || Delayer.size > 0

    Plugin.call :tick
    if $tick_counter >= 120
      Plugin.call :period
      $tick_counter = 0
    end
    Delayer.run until Delayer.empty?

    $last_ticked = Time.now
  rescue Exception => e
    puts e.inspect
    puts e.backtrace.map{|v| "\t" + v}.join("\n")
  end
end

begin
  # Find bundle plugins
  plugins = []
  Android::AssetDir.open('plugin').each do |path|
    plugins << path if path.end_with? '.rb'
  end
  plugins.sort!

  Mix::Miquire.loadpath << plugins.map do |plugin|
    slug = plugin.delete_suffix('.rb').to_sym
    Mix::Miquire::Plugin.new(slug, Mix::Miquire::Spec.new(slug: slug), -> { Android.require_assets plugin })
  end

  # Load bundle plugins
  Mix::Miquire.load_all

  # Initialize global variables
  $tick_counter = 0

  # Call :boot event
  Plugin.call :boot
  puts 'welcome to yukari-exvoice'
rescue Exception => e
  puts e.inspect
  puts e.backtrace.map{|v| "\t" + v}.join("\n")
end
