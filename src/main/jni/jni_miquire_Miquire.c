#include <jni.h>
#include <mruby.h>
#include <mruby/mix.h>
#include <mruby/array.h>
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

    jclass stringArrayClass = (*env)->FindClass(env, JSIG_ARRAY(JSIG_CLASS("java/lang/String")));
    jclass stringClass = (*env)->FindClass(env, "java/lang/String");
    jobjectArray jResult = (*env)->NewObjectArray(env, 2, stringArrayClass, NULL);

    mrb_value rSuccess = mrb_ary_entry(result, 0);
    jobjectArray jSuccess = (*env)->NewObjectArray(env, RARRAY_LEN(rSuccess), stringClass, NULL);
    for (int i = 0; i < RARRAY_LEN(rSuccess); i++) {
        (*env)->SetObjectArrayElement(env, jSuccess, i, convertMrbValueToJava(env, mrb, mrb_ary_entry(rSuccess, i)));
    }
    (*env)->SetObjectArrayElement(env, jResult, 0, jSuccess);

    mrb_value rFailure = mrb_ary_entry(result, 1);
    jobjectArray jFailure = (*env)->NewObjectArray(env, RARRAY_LEN(rFailure), stringClass, NULL);
    for (int i = 0; i < RARRAY_LEN(rFailure); i++) {
        (*env)->SetObjectArrayElement(env, jFailure, i, convertMrbValueToJava(env, mrb, mrb_ary_entry(rFailure, i)));
    }
    (*env)->SetObjectArrayElement(env, jResult, 1, jFailure);

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