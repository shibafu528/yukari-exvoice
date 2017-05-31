#include <jni.h>
#include <mruby.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include "converter.h"

JNIEXPORT jlong JNICALL Java_info_shibafu528_yukari_exvoice_diva_ModelFactory_newInstanceNative(JNIEnv *env, jclass clazz, jlong jMRubyPointer, jstring jModelClassName, jobject valuesMap) {
    mrb_state *mrb = (mrb_state *) jMRubyPointer;
    const char *modelClassName = (*env)->GetStringUTFChars(env, jModelClassName, NULL);

    // valuesMap -> Ruby Hash
    mrb_value rValuesHash = convertJavaToMrbValue(env, mrb, valuesMap);

    // Hash Keys -> Symbol
    mrb_value rKeys = mrb_hash_keys(mrb, rValuesHash);
    for (int i = 0; i < RARRAY_LEN(rKeys); i++) {
        mrb_value rKey = mrb_ary_ref(mrb, rKeys, i);
        mrb_sym rKeySym = mrb_intern_str(mrb, rKey);
        
        mrb_value rValue = mrb_hash_get(mrb, rValuesHash, rKey);
        mrb_hash_delete_key(mrb, rValuesHash, rKey);
        mrb_hash_set(mrb, rValuesHash, mrb_symbol_value(rKeySym), rValue);
    }
    
    // Initialize Model
    mrb_value rModelInstance = mrb_obj_new(mrb, mrb_class_get(mrb, modelClassName), 1, &rValuesHash);

    // Keep from GC
    mrb_gc_register(mrb, rModelInstance);

    (*env)->ReleaseStringUTFChars(env, jModelClassName, modelClassName);

    return (jlong) mrb_ptr(rModelInstance);
}