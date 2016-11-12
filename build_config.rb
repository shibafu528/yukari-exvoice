configure = -> (conf) {
  conf.gem :github => 'shibafu528/mruby-pluggaloid'
  conf.gem :github => 'matsumoto-r/mruby-sleep'
  conf.gem :github => 'mattn/mruby-json'
  conf.gem :github => 'shibafu528/mruby-thread', :branch => 'patch-android'
  conf.gem :github => 'iij/mruby-regexp-pcre'
  conf.gem :github => 'iij/mruby-io'
  conf.gem :github => 'iij/mruby-require'

  conf.gembox 'default'

  conf.cc.defines << %w(MRB_INT64 MRB_UTF8_STRING)
}

MRuby::Build.new do |conf|
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  enable_debug
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-armeabi') do |conf|
  toolchain :android, arch: 'armeabi'
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-armeabi-v7a') do |conf|
  toolchain :android, arch: 'armeabi-v7a'
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-arm64-v8a') do |conf|
  toolchain :android, arch: 'arm64', platform: 'android-21'
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-x86') do |conf|
  toolchain :android, arch: 'x86'
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-x86_64') do |conf|
  toolchain :android, arch: 'x86_64', platform: 'android-21'
  instance_exec conf, &configure
end