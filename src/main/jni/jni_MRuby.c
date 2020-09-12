#include <stddef.h>
#include <jni.h>
#include <mruby.h>
#include <mruby/error.h>
#include <mruby/mix.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <android/log.h>
#include "jni_MRuby.h"
#include "common.h"
#include "converter.h"
#include "mrb_Android.h"
#include "mrb_ConfigLoader.h"

#define EXVOICE_INITIALIZERS(mrb) \
    exvoice_init_android(mrb); \
    exvoice_init_configloader(mrb);

#define MRB_INSTANCE_STORE_SIZE 16
static MRubyInstance instances[MRB_INSTANCE_STORE_SIZE] = {};

jfieldID field_MRuby_context = NULL;
jfieldID field_MRuby_assetManager = NULL;
jfieldID field_MRuby_mrubyInstancePointer = NULL;

jmethodID method_MRuby_getPlugin = NULL;
jmethodID method_MRuby_getMutex = NULL;

MRubyInstance *findMRubyInstance(mrb_state *mrb) {
    for (int i = 0; i < MRB_INSTANCE_STORE_SIZE; i++) {
        if (instances[i].mrb == mrb) {
            return &instances[i];
        }
    }
    return NULL;
}

MRubyInstanceManagerResult storeMRubyInstance(mrb_state *mrb, jclass instance) {
    for (int i = 0; i < MRB_INSTANCE_STORE_SIZE; i++) {
        if (instances[i].mrb == NULL) {
            JNIEnv *env = getJNIEnv();
            instances[i].mrb = mrb;
            instances[i].javaInstance = (*env)->NewGlobalRef(env, instance);

            return MRB_INSTANCE_SUCCESS;
        }
    }
    return MRB_INSTANCE_FAIL_ALLOC;
}

MRubyInstanceManagerResult removeMRubyInstance(mrb_state *mrb) {
    for (int i = 0; i < MRB_INSTANCE_STORE_SIZE; i++) {
        if (instances[i].mrb == mrb) {
            JNIEnv *env = getJNIEnv();
            (*env)->DeleteGlobalRef(env, instances[i].javaInstance);

            instances[i].mrb = NULL;
            instances[i].javaInstance = NULL;

            return MRB_INSTANCE_SUCCESS;
        }
    }
    return MRB_INSTANCE_FAIL_NOT_FOUND;
}

static mrb_value mrb_printstr(mrb_state *mrb, mrb_value self) {
    mrb_value argv;
    mrb_get_args(mrb, "o", &argv);

    if (mrb_string_p(argv)) {
        char *string = mrb_str_to_cstr(mrb, argv);
        JNIEnv *env = getJNIEnv();
        MRubyInstance *instance = findMRubyInstance(mrb);
        if (instance != NULL) {
            jstring jstr = (*env)->NewStringUTF(env, string);
            jclass jcls = (*env)->GetObjectClass(env, instance->javaInstance);
            static jmethodID jm_printStringCallback = NULL;
            if (jm_printStringCallback == NULL) {
                jm_printStringCallback = (*env)->GetMethodID(env, jcls, "printStringCallback", "(Ljava/lang/String;)V");
            }
            (*env)->CallVoidMethod(env, instance->javaInstance, jm_printStringCallback, jstr);
            (*env)->DeleteLocalRef(env, jstr);
            (*env)->DeleteLocalRef(env, jcls);
        }
    }

    return argv;
}

static void delayer_remain_handler(mrb_state *mrb) {
    __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "delayer_remain_handler");
    JNIEnv *env = getJNIEnv();
    MRubyInstance *instance = findMRubyInstance(mrb);
    if (instance != NULL) {
        jclass jcls = (*env)->GetObjectClass(env, instance->javaInstance);
        static jmethodID jm_delayerRemainCallback = NULL;
        if (jm_delayerRemainCallback == NULL) {
            jm_delayerRemainCallback = (*env)->GetMethodID(env, jcls, "delayerRemainCallback", "()V");
        }
        (*env)->CallVoidMethod(env, instance->javaInstance, jm_delayerRemainCallback);
        (*env)->DeleteLocalRef(env, jcls);
    }
}

static void delayer_reserve_handler(mrb_state *mrb, mrb_float delay) {
    __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "delayer_reserve_handler");
    JNIEnv *env = getJNIEnv();
    MRubyInstance *instance = findMRubyInstance(mrb);
    if (instance != NULL) {
        jclass jcls = (*env)->GetObjectClass(env, instance->javaInstance);
        static jmethodID jm_delayerReserveCallback = NULL;
        if (jm_delayerReserveCallback == NULL) {
            jm_delayerReserveCallback = (*env)->GetMethodID(env, jcls, "delayerReserveCallback", "(D)V");
        }
        (*env)->CallVoidMethod(env, instance->javaInstance, jm_delayerReserveCallback, (jdouble) delay);
        (*env)->DeleteLocalRef(env, jcls);
    }
}

JNIEXPORT jlong JNICALL Java_info_shibafu528_yukari_exvoice_MRuby_n_1open(JNIEnv *env, jobject self) {
    mrb_state *mrb = mrb_open();
    __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "open addr: %d", mrb);

    // Override mruby-print Kernel.__printstr__
    mrb_define_module_function(mrb, mrb->kernel_module, "__printstr__", mrb_printstr, MRB_ARGS_REQ(1));

    // Override Mix.log
    struct RClass *mix = mrb_module_get(mrb, "Mix");
    mrb_define_module_function(mrb, mix, "log", mrb_printstr, MRB_ARGS_REQ(1));

    // Initialize Objects
    int arenaIndex = mrb_gc_arena_save(mrb);
    EXVOICE_INITIALIZERS(mrb);
    mrb_gc_arena_restore(mrb, arenaIndex);

    // Store instances
    storeMRubyInstance(mrb, self);
    {
        jclass selfClass = (*env)->GetObjectClass(env, self);
        // Get fieldID of MRuby.context
        field_MRuby_context = (*env)->GetFieldID(env, selfClass, "context", "Landroid/content/Context;");
        // Get fieldID of MRuby.assetManager
        field_MRuby_assetManager = (*env)->GetFieldID(env, selfClass, "assetManager", "Landroid/content/res/AssetManager;");
        (*env)->DeleteLocalRef(env, selfClass);
    }

    // Register hooks
    mix_register_remain_handler(mrb, delayer_remain_handler);
    mix_register_reserve_handler(mrb, delayer_reserve_handler);

    return (jlong) mrb;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_MRuby_n_1close(JNIEnv *env, jobject self, jlong mrb) {
    __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "close addr: %d", mrb);

    removeMRubyInstance((mrb_state*) mrb);
    mrb_close((mrb_state*) mrb);
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_MRuby_n_1loadString(JNIEnv *env, jobject self, jlong pMrb, jstring code, jboolean echo) {
    mrb_state *mrb = (mrb_state*) pMrb;
    const char *codeBytes = (*env)->GetStringUTFChars(env, code, NULL);
    if (echo == JNI_TRUE) {
        __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "mrb_load_string\n%s", codeBytes);
    }

    int arenaIndex = mrb_gc_arena_save(mrb);

    mrbc_context *mrbContext = mrbc_context_new(mrb);
    mrbc_filename(mrb, mrbContext, "(eval)");
    mrb_load_string_cxt(mrb, codeBytes, mrbContext);

    mrb_gc_arena_restore(mrb, arenaIndex);

    (*env)->ReleaseStringUTFChars(env, code, codeBytes);
}

JNIEXPORT jobject JNICALL Java_info_shibafu528_yukari_exvoice_MRuby_n_1callTopLevelFunc(JNIEnv *env, jobject self, jlong pMrb, jstring name) {
    mrb_state *mrb = (mrb_state*) pMrb;
    const char *cName = (*env)->GetStringUTFChars(env, name, NULL);
    mrb_value returnValue = mrb_funcall(mrb, mrb_obj_value(mrb->top_self), cName, 0, NULL);
    (*env)->ReleaseStringUTFChars(env, name, cName);
    return convertMrbValueToJava(env, mrb, returnValue);
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_MRuby_n_1callTopLevelProc(JNIEnv *env, jobject self, jlong pMrb, jstring name) {
    mrb_state *mrb = (mrb_state*) pMrb;
    const char *cName = (*env)->GetStringUTFChars(env, name, NULL);

    int arenaIndex = mrb_gc_arena_save(mrb);
    mrb_funcall(mrb, mrb_obj_value(mrb->top_self), cName, 0, NULL);
    mrb_gc_arena_restore(mrb, arenaIndex);

    (*env)->ReleaseStringUTFChars(env, name, cName);
}

static mrb_value mix_run_m(mrb_state *mrb, mrb_value self) {
    mix_run(mrb);
    return mrb_nil_value();
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_MRuby_n_1runDelayer(JNIEnv *env, jobject self, jlong pMrb) {
    mrb_state *mrb = (mrb_state*) pMrb;
    int arenaIndex = mrb_gc_arena_save(mrb);
    mrb_bool state = 0;
    mrb_value result = mrb_protect(mrb, mix_run_m, mrb_cptr_value(mrb, self), &state);
    if (state) {
        mrb_funcall_argv(mrb, mrb_obj_value(mrb->kernel_module), mrb_intern_lit(mrb, "error"), 1, &result);
    }
    mrb_gc_arena_restore(mrb, arenaIndex);
    __android_log_print(ANDROID_LOG_DEBUG, "exvoice", "Delayer.run done");
}