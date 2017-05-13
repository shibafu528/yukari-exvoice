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

file(GLOB jni_c_sources "src/main/jni/*.c")
file(GLOB jni_cpp_sources "src/main/jni/*.cpp")
add_library(exvoice SHARED ${jni_c_sources} ${jni_cpp_sources})

target_link_libraries(exvoice android log mruby)



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