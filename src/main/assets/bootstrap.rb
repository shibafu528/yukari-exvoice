# encoding: utf-8
# Yukari; exVoice
# mrubyとPluggaloidを使用したモバイル向けプラグインサブシステム

Android.require_assets 'avoid.rb'
Android.require_assets 'javaerror.rb'

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

  puts "registered #{plugins.size} bundle plugins."
  puts 'yukari-exvoice bootstrap done!'
rescue Exception => e
  puts e.inspect
  puts e.backtrace.map{|v| "\t" + v}.join("\n")
  raise e
end
