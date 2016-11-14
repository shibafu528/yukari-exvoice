configure = -> (conf) {
  # default.gembox without commands
  conf.gem :core => 'mruby-sprintf'
  conf.gem :core => 'mruby-print'
  conf.gem :core => 'mruby-math'
  conf.gem :core => 'mruby-time'
  conf.gem :core => 'mruby-struct'
  conf.gem :core => 'mruby-enum-ext'
  conf.gem :core => 'mruby-string-ext'
  conf.gem :core => 'mruby-numeric-ext'
  conf.gem :core => 'mruby-array-ext'
  conf.gem :core => 'mruby-hash-ext'
  conf.gem :core => 'mruby-range-ext'
  conf.gem :core => 'mruby-proc-ext'
  conf.gem :core => 'mruby-symbol-ext'
  conf.gem :core => 'mruby-random'
  conf.gem :core => 'mruby-object-ext'
  conf.gem :core => 'mruby-objectspace'
  conf.gem :core => 'mruby-fiber'
  conf.gem :core => 'mruby-enumerator'
  conf.gem :core => 'mruby-enum-lazy'
  conf.gem :core => 'mruby-toplevel-ext'
  conf.gem :core => 'mruby-compiler'
  conf.gem :core => 'mruby-kernel-ext'

  # exvoice dependencies
  conf.gem :github => 'shibafu528/mruby-pluggaloid'
  conf.gem :github => 'matsumoto-r/mruby-sleep'
  conf.gem :github => 'mattn/mruby-json'
  conf.gem :github => 'shibafu528/mruby-thread', :branch => 'patch-android'
  conf.gem :github => 'iij/mruby-regexp-pcre'
  conf.gem :github => 'iij/mruby-io'
  conf.gem :github => 'iij/mruby-require'

  conf.cc.defines << %w(MRB_INT64 MRB_UTF8_STRING)
}

MRuby::Build.new do |conf|
  if ENV['VisualStudioVersion'] || ENV['VSINSTALLDIR']
    toolchain :visualcpp
  else
    toolchain :gcc
  end

  conf.gem :core => 'mruby-bin-mirb'
  conf.gem :core => 'mruby-bin-mruby'
  conf.gem :core => 'mruby-bin-strip'

  enable_debug
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-armeabi') do |conf|
  toolchain :android, arch: 'armeabi', platform: 'android-14', toolchain: :gcc
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-armeabi-v7a') do |conf|
  toolchain :android, arch: 'armeabi-v7a', platform: 'android-14', toolchain: :gcc
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-arm64-v8a') do |conf|
  toolchain :android, arch: 'arm64-v8a', platform: 'android-21', toolchain: :gcc
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-x86') do |conf|
  toolchain :android, arch: 'x86', platform: 'android-14', toolchain: :gcc
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('android-x86_64') do |conf|
  toolchain :android, arch: 'x86_64', platform: 'android-21', toolchain: :gcc
  instance_exec conf, &configure
end