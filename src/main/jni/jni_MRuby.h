#ifndef YUKARI_EXVOICE_JNI_MRUBY_H
#define YUKARI_EXVOICE_JNI_MRUBY_H

#include <jni.h>

typedef struct _MRubyInstance {
    mrb_state* mrb;
    jclass javaInstance;
} MRubyInstance;

typedef enum _MRubyInstanceManagerResult {
    MRB_INSTANCE_SUCCESS,
    MRB_INSTANCE_FAIL_ALLOC,
    MRB_INSTANCE_FAIL_NOT_FOUND
} MRubyInstanceManagerResult;

extern jfieldID field_MRuby_context;
extern jfieldID field_MRuby_assetManager;
extern jfieldID field_MRuby_mrubyInstancePointer;

extern jmethodID method_MRuby_getPlugin;
extern jmethodID method_MRuby_getMutex;

static inline mrb_state* getField_MRuby_mrubyInstancePointer(JNIEnv *env, jobject mRubyObject) {
    if (field_MRuby_mrubyInstancePointer == NULL) {
        jclass mRubyClass = (*env)->GetObjectClass(env, mRubyObject);
        field_MRuby_mrubyInstancePointer = (*env)->GetFieldID(env, mRubyClass, "mrubyInstancePointer", "J");

        (*env)->DeleteLocalRef(env, mRubyClass);
    }
    return (mrb_state *) (*env)->GetLongField(env, mRubyObject, field_MRuby_mrubyInstancePointer);
}

static inline jobject call_MRuby_getPlugin(JNIEnv *env, jobject mRubyObject, jstring slug) {
    if (method_MRuby_getPlugin == NULL) {
        jclass mRubyClass = (*env)->GetObjectClass(env, mRubyObject);
        method_MRuby_getPlugin = (*env)->GetMethodID(env, mRubyClass, "getPlugin", "(Ljava/lang/String;)Linfo/shibafu528/yukari/exvoice/Plugin;");

        (*env)->DeleteLocalRef(env, mRubyClass);
    }
    return (*env)->CallObjectMethod(env, mRubyObject, method_MRuby_getPlugin, slug);
}

static inline jobject call_MRuby_getMutex(JNIEnv *env, jobject mRubyObject) {
    if (method_MRuby_getMutex == NULL) {
        jclass mRubyClass = (*env)->GetObjectClass(env, mRubyObject);
        method_MRuby_getMutex = (*env)->GetMethodID(env, mRubyClass, "getMutex", "()Ljava/lang/Object;");

        (*env)->DeleteLocalRef(env, mRubyClass);
    }
    return (*env)->CallObjectMethod(env, mRubyObject, method_MRuby_getMutex);
}

MRubyInstance *findMRubyInstance(mrb_state *mrb);
MRubyInstanceManagerResult storeMRubyInstance(mrb_state *mrb, jclass instance);
MRubyInstanceManagerResult removeMRubyInstance(mrb_state *mrb);

#endif //YUKARI_EXVOICE_JNI_MRUBY_H
