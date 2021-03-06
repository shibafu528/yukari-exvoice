#include <stddef.h>
#include <jni.h>
#include <mruby.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_MRuby.h"

JavaVM *g_jvm;

jint JNI_OnLoad(JavaVM *jvm, void *reserved) {
    __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "JNI_onLoad, jvm addr: %d", jvm);
    g_jvm = jvm;
    return JNI_VERSION_1_6;
}

JNIEnv* getJNIEnv() {
    JNIEnv *env;
    jint ret;

    ret = (*g_jvm)->GetEnv(g_jvm, (void**)&env, JNI_VERSION_1_6);
    if (ret != JNI_OK) {
        return NULL;
    }
    return env;
}

jobject getContext(mrb_state *mrb) {
    JNIEnv *env = getJNIEnv();
    MRubyInstance *instance = findMRubyInstance(mrb);

    return (*env)->GetObjectField(env, instance->javaInstance, field_MRuby_context);
}

AAssetManager* getAAssetManager(mrb_state *mrb) {
    JNIEnv *env = getJNIEnv();
    MRubyInstance *instance = findMRubyInstance(mrb);

    AAssetManager *manager;
    {
        jobject assetManager = (*env)->GetObjectField(env, instance->javaInstance, field_MRuby_assetManager);
        manager = AAssetManager_fromJava(env, assetManager);

        (*env)->DeleteLocalRef(env, assetManager);
    }
    return manager;
}