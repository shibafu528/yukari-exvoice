# disable pkg-config
ENV['PKG_CONFIG_LIBDIR'] = ''

MRuby::Lockfile.disable

configure = -> (conf) {
  enable_debug
  
  # default.gembox without commands
  conf.gem :core => 'mruby-metaprog'
  conf.gem :core => 'mruby-io'
  conf.gem :core => 'mruby-pack'
  conf.gem :core => 'mruby-sprintf'
  conf.gem :core => 'mruby-print'
  conf.gem :core => 'mruby-math'
  conf.gem :core => 'mruby-time'
  conf.gem :core => 'mruby-struct'
  conf.gem :core => 'mruby-compar-ext'
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
  conf.gem :core => 'mruby-kernel-ext'
  conf.gem :core => 'mruby-class-ext'
  conf.gem :core => 'mruby-error'
  conf.gem :core => 'mruby-rational'
  conf.gem :core => 'mruby-complex'
  conf.gem :core => 'mruby-compiler'

  # exvoice dependencies
  # libyamlをsubmoduleでcheckoutするようになったあたりから上手くクロスコンパイルできない
  conf.gem :github => 'mrbgems/mruby-yaml', :checksum_hash => '0606652a6e99d902cd3101cf2d757a7c0c37a7fd'
  conf.gem :github => 'shibafu528/mruby-mix', :path => 'mruby-mix'
  conf.gem :github => 'shibafu528/mruby-mix', :path => 'mruby-mix-miquire-fs'
  conf.gem :github => 'shibafu528/mruby-mix', :path => 'mruby-mix-polyfill-gtk'
  conf.gem :github => 'shibafu528/mruby-mix', :path => 'mruby-mix-command-conditions'
  conf.gem :github => 'shibafu528/mruby-mix', :path => 'mruby-mix-twitter-models'
  conf.gem :github => 'matsumoto-r/mruby-sleep'
  conf.gem :github => 'mattn/mruby-json'
  conf.gem :github => 'shibafu528/mruby-thread', :branch => 'patch-android'
  conf.gem :github => 'iij/mruby-dir'
  conf.gem :github => 'iij/mruby-require'
  conf.gem :github => 'ksss/mruby-singleton'

  conf.cc.defines << %w(MRB_INT64 MRB_UTF8_STRING)
  conf.cc.flags << '-std=gnu99'

  # expose libyaml.a (mruby-yaml)
  file conf.libfile("#{conf.build_dir}/lib/libyaml") => conf.libfile("#{conf.build_dir}/lib/libmruby") do |t|
    cp Dir.glob(File.join(conf.build_dir, 'mrbgems/**/libyaml.a')).first, t.name
  end
  task :all => conf.libfile("#{conf.build_dir}/lib/libyaml")
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

MRuby::CrossBuild.new('armv7-linux-androideabi') do |conf|
  toolchain :android, arch: 'armeabi-v7a', platform: 'android-14', toolchain: :gcc
  conf.host_target = 'armv7-linux-androideabi'
  conf.build_target = 'x86_64-pc-linux-gnu'
  instance_exec conf, &configure
end

MRuby::CrossBuild.new('aarch64-linux-androideabi') do |conf|
  toolchain :android, arch: 'arm64-v8a', platform: 'android-21', toolchain: :gcc
  conf.host_target = 'aarch64-linux-androideabi'
  conf.build_target = 'x86_64-pc-linux-gnu'
  instance_exec conf, &configure
end

# MRuby::CrossBuild.new('x86-linux-androideabi') do |conf|
#   toolchain :android, arch: 'x86', platform: 'android-14', toolchain: :gcc
#   conf.host_target = 'x86-linux-androideabi'
#   conf.build_target = 'x86_64-pc-linux-gnu'
#   instance_exec conf, &configure
# end

# MRuby::CrossBuild.new('x86_64-linux-androideabi') do |conf|
#   toolchain :android, arch: 'x86_64', platform: 'android-21', toolchain: :gcc
#   instance_exec conf, &configure
# end
