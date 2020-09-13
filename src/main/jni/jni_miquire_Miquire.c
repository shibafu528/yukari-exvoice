#include <jni.h>
#include <mruby.h>
#include <mruby/mix.h>
#include <mruby/string.h>
#include "generator.h"
#include "converter.h"

JNIEXPORT jobjectArray JNICALL Java_info_shibafu528_yukari_exvoice_miquire_Miquire_n_1loadAll(JNIEnv *env, jclass clazz, jlong pMrb, jboolean failfast) {
    mrb_state *mrb = (mrb_state *) pMrb;
    int arenaIndex = mrb_gc_arena_save(mrb);

    mrb_value result;
    if (failfast) {
        result = mix_miquire_load_all_failfast(mrb);
    } else {
        result = mix_miquire_load_all(mrb);
    }

    if (mrb->exc) {
        jclass exceptionClass = (*env)->FindClass(env, JCLASS_EXVOICE_MRUBY_EXCEPTION);
        mrb_value ins = mrb_inspect(mrb, mrb_obj_value(mrb->exc));
        (*env)->ThrowNew(env, exceptionClass, mrb_str_to_cstr(mrb, ins));

        (*env)->DeleteLocalRef(env, exceptionClass);

        mrb->exc = 0;
        mrb_gc_arena_restore(mrb, arenaIndex);

        return NULL;
    }

    jobject jResult = convertMrbValueToJava(env, mrb, result);
    if (mrb->exc) {
        jclass exceptionClass = (*env)->FindClass(env, JCLASS_EXVOICE_MRUBY_EXCEPTION);
        mrb_value ins = mrb_inspect(mrb, mrb_obj_value(mrb->exc));
        (*env)->ThrowNew(env, exceptionClass, mrb_str_to_cstr(mrb, ins));

        (*env)->DeleteLocalRef(env, exceptionClass);

        mrb->exc = 0;
        mrb_gc_arena_restore(mrb, arenaIndex);

        return NULL;
    }

    mrb_gc_arena_restore(mrb, arenaIndex);

    return jResult;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_miquire_Miquire_n_1appendLoadPath(JNIEnv *env, jclass clazz, jlong pMrb, jstring path) {
    mrb_state *mrb = (mrb_state *) pMrb;
    int arenaIndex = mrb_gc_arena_save(mrb);

    const char *cPath = (*env)->GetStringUTFChars(env, path, NULL);
    mix_miquire_append_loadpath_cstr(mrb, cPath);
    (*env)->ReleaseStringUTFChars(env, path, cPath);

    mrb_gc_arena_restore(mrb, arenaIndex);
}