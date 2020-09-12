# encoding: utf-8

# core/config.rb, core/environment.rb 互換モジュール

module CHIConfig
  NAME = "Yukari for Android"
  ACRO = "yukari4a"

  PIDFILE = "/sdcard/Android/data/shibafu.yukari/files/#{ACRO}.pid"

  CONFROOT = "/sdcard/Android/data/shibafu.yukari/files/"

  TMPDIR = File.join(CONFROOT, 'tmp')

  LOGDIR = File.join(CONFROOT, 'log')

  SETTINGDIR = File.join(CONFROOT, 'settings')

  CACHE = File.join(CONFROOT, 'cache')

  PLUGIN_PATH = File.join(CONFROOT, 'plugin')
end

module Environment
  NAME = CHIConfig::NAME

  ACRO = CHIConfig::ACRO

  PIDFILE = CHIConfig::PIDFILE

  CONFROOT = CHIConfig::CONFROOT

  TMPDIR = CHIConfig::TMPDIR

  LOGDIR = CHIConfig::LOGDIR

  SETTINGDIR = CHIConfig::SETTINGDIR

  CACHE = CHIConfig::CACHE

  PLUGIN_PATH = CHIConfig::PLUGIN_PATH

  VERSION = Version.new(*CHIConfig::VERSION)

end
