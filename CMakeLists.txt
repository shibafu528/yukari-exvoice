# build script for Android Studio 2.2+ / Android Plugin for Gradle 2.2+
cmake_minimum_required(VERSION 3.5)

add_definitions("-DMRB_INT64 -DMRB_UTF8_STRING")

include_directories(mruby/include)

# ABI -> mruby build arch
if(ANDROID_ABI STREQUAL "armeabi")
    set(MRUBY_ARCH "armv5-linux-androideabi")
elseif(ANDROID_ABI STREQUAL "armeabi-v7a")
    set(MRUBY_ARCH "armv7-linux-androideabi")
elseif(ANDROID_ABI STREQUAL "arm64-v8a")
    set(MRUBY_ARCH "aarch64-linux-androideabi")
elseif(ANDROID_ABI STREQUAL "x86")
    set(MRUBY_ARCH "x86-linux-androideabi")
elseif(ANDROID_ABI STREQUAL "x86_64")
    set(MRUBY_ARCH "x86_64-linux-androideabi")
endif()

link_directories(mruby/build/${MRUBY_ARCH}/lib)
include_directories(mruby/build/repos/${MRUBY_ARCH}/mruby-mix/mruby-mix/include)

add_library(
        exvoice
        SHARED
        src/main/jni/common.c
        src/main/jni/common.h
        src/main/jni/converter.c
        src/main/jni/converter.h
        src/main/jni/generator.h
        src/main/jni/jni_BuildInfo.c
        src/main/jni/jni_ConfigLoader.c
        src/main/jni/jni_diva_ModelFactory.c
        src/main/jni/jni_MRuby.c
        src/main/jni/jni_MRuby.h
        src/main/jni/jni_MRubyObjectHandler.c
        src/main/jni/jni_miquire_Miquire.c
        src/main/jni/jni_pluggaloid_Plugin.c
        src/main/jni/jni_ProcWrapper.c
        src/main/jni/mrb_Android.c
        src/main/jni/mrb_Android.h
        src/main/jni/mrb_Android_AssetDir.c
        src/main/jni/mrb_Android_AssetDir.h
        src/main/jni/mrb_Android_Log.c
        src/main/jni/mrb_Android_Log.h
        src/main/jni/mrb_ConfigLoader.c
        src/main/jni/mrb_ConfigLoader.h
        src/main/jni/mrb_JavaThrowable.c
        src/main/jni/mrb_JavaThrowable.h
)

target_link_libraries(exvoice android log mruby yaml)



# ------------------------------

# for CLion (OLD)
#project(yukari_exvoice)
#
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#
#include_directories(
#        ${PROJECT_SOURCE_DIR}/mruby/include
#        $ENV{JAVA_HOME}/include
#        $ENV{JAVA_HOME}/include/win32
#        $ENV{ANDROID_NDK_HOME}/platforms/android-24/arch-arm/usr/include
#)
#
#file(GLOB jni_sources "src/main/jni/*.c")
#file(GLOB jni_headers "src/main/jni/*.h")
#
#set(SOURCE_FILES ${jni_sources} ${jni_headers})
#
#add_executable(yukari_exvoice ${SOURCE_FILES})