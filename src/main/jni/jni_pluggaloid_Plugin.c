#include <jni.h>
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/proc.h>
#include <mruby/error.h>
#include <mruby/mix.h>
#include <stddef.h>
#include <android/log.h>
#include "jni_MRuby.h"
#include "converter.h"
#include "common.h"
#include "generator.h"
#include "mrb_JavaThrowable.h"

OBJECT_FIELD_READER(getField_Plugin_mRuby, mRuby, JSIG_EXVOICE_MRUBY)
OBJECT_FIELD_READER(getField_Plugin_slug, slug, JSIG_STRING)

static inline void call_Plugin_onEvent(JNIEnv *env, jobject self, jstring eventName, jobjectArray args) {
    static jmethodID method_Plugin_onEvent = NULL;
    if (method_Plugin_onEvent == NULL) {
        jclass selfClass = (*env)->GetObjectClass(env, self);
        method_Plugin_onEvent = (*env)->GetMethodID(env, selfClass, "onEvent", "(Ljava/lang/String;[Ljava/lang/Object;)V");

        (*env)->DeleteLocalRef(env, selfClass);
    }
    (*env)->CallVoidMethod(env, self, method_Plugin_onEvent, eventName, args);
}

static inline jobjectArray call_Plugin_filter(JNIEnv *env, jobject self, jstring eventName, jobjectArray args) {
    static jmethodID method_Plugin_filter = NULL;
    if (method_Plugin_filter == NULL) {
        jclass selfClass = (*env)->GetObjectClass(env, self);
        method_Plugin_filter = (*env)->GetMethodID(env, selfClass, "filter", "(Ljava/lang/String;[Ljava/lang/Object;)[Ljava/lang/Object;");

        (*env)->DeleteLocalRef(env, selfClass);
    }
    return (jobjectArray) (*env)->CallObjectMethod(env, self, method_Plugin_filter, eventName, args);
}

static inline void call_Plugin_onSpell(JNIEnv *env, jobject self, jstring callbackKey, jobjectArray models, jobject options) {
    static jmethodID method_Plugin_onSpell = NULL;
    if (method_Plugin_onSpell == NULL) {
        jclass selfClass = (*env)->GetObjectClass(env, self);
        method_Plugin_onSpell = (*env)->GetMethodID(env, selfClass, "onSpell", "(Ljava/lang/String;[Ljava/lang/Object;Ljava/util/Map;)V");

        (*env)->DeleteLocalRef(env, selfClass);
    }
    (*env)->CallVoidMethod(env, self, method_Plugin_onSpell, callbackKey, models, options);
}


static inline mrb_sym convertJstringToSymbol(JNIEnv *env, mrb_state *mrb, jstring str) {
    const char *cstr = (*env)->GetStringUTFChars(env, str, NULL);
    mrb_sym sym = mrb_intern_cstr(mrb, cstr);
    (*env)->ReleaseStringUTFChars(env, str, cstr);

    return sym;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_pluggaloid_Plugin_initializeNative(JNIEnv *env, jobject self) {
    // Get mrb_state
    jobject mRubyObject = getField_Plugin_mRuby(env, self);
    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRubyObject);

    // Get this.slug
    mrb_value rSlug;
    {
        jstring slug = getField_Plugin_slug(env, self);
        rSlug = mrb_symbol_value(convertJstringToSymbol(env, mrb, slug));

        (*env)->DeleteLocalRef(env, slug);
    }

    // Create Plugin
    struct RClass *pluggaloid = mrb_module_get(mrb, "Pluggaloid");
    struct RClass *plugin = mrb_class_get_under(mrb, pluggaloid, "Plugin");
    mrb_funcall_argv(mrb, mrb_obj_value(plugin), mrb_intern_lit(mrb, "create"), 1, &rSlug);

    // Release references
    (*env)->DeleteLocalRef(env, mRubyObject);
}

static mrb_value addEventListener_callback(mrb_state *mrb, mrb_value self) {
    JNIEnv *env = getJNIEnv();
    mrb_value rSlug = mrb_proc_cfunc_env_get(mrb, 0);
    mrb_value rEventName = mrb_proc_cfunc_env_get(mrb, 1);
    MRubyInstance *instance = findMRubyInstance(mrb);
    jstring jSlug = convertMrbValueToJava(env, mrb, rSlug);
    jobject jPlugin = call_MRuby_getPlugin(env, instance->javaInstance, jSlug);

    mrb_value *rArgs;
    mrb_int rArgc;
    mrb_get_args(mrb, "*", &rArgs, &rArgc);

    jclass objectClass = (*env)->FindClass(env, JCLASS_OBJECT);
    jobjectArray jArgs = (*env)->NewObjectArray(env, rArgc, objectClass, NULL);
    for (int i = 0; i < rArgc; i++) {
        (*env)->SetObjectArrayElement(env, jArgs, i, convertMrbValueToJava(env, mrb, rArgs[i]));
    }

    jstring jEventName = convertMrbValueToJava(env, mrb, rEventName);
    call_Plugin_onEvent(env, jPlugin, jEventName, jArgs);

    (*env)->DeleteLocalRef(env, objectClass);
    (*env)->DeleteLocalRef(env, jSlug);
    (*env)->DeleteLocalRef(env, jPlugin);
    (*env)->DeleteLocalRef(env, jArgs);
    (*env)->DeleteLocalRef(env, jEventName);
    return mrb_nil_value();
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_pluggaloid_Plugin_addEventListenerNative(JNIEnv *env, jobject self, jstring eventName) {
    // Get mrb_state
    jobject mRubyObject = getField_Plugin_mRuby(env, self);
    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRubyObject);

    // Get this.slug
    mrb_value rSlug;
    {
        jstring slug = getField_Plugin_slug(env, self);
        rSlug = mrb_symbol_value(convertJstringToSymbol(env, mrb, slug));

        (*env)->DeleteLocalRef(env, slug);
    }

    // String -> Symbol
    mrb_value rEventName = mrb_symbol_value(convertJstringToSymbol(env, mrb, eventName));

    // Find Plugin
    struct RClass *plugin = mrb_class_get_under(mrb, mrb_module_get(mrb, "Pluggaloid"), "Plugin");
    mrb_value rPlugin = mrb_funcall(mrb, mrb_obj_value(plugin), "[]", 1, rSlug);

    // Register event
    mrb_value procEnv[] = { rSlug, rEventName };
    struct RProc *proc = mrb_proc_new_cfunc_with_env(mrb, addEventListener_callback, 2, procEnv);
    mrb_funcall_with_block(mrb, rPlugin, mrb_intern_lit(mrb, "add_event"), 1, &rEventName, mrb_obj_value(proc));

    __android_log_print(ANDROID_LOG_DEBUG, "exvoice-Plugin", "register native event %s%s", mrb_str_to_cstr(mrb, mrb_inspect(mrb, rSlug)), mrb_str_to_cstr(mrb, mrb_inspect(mrb, rEventName)));

    // Release references
    (*env)->DeleteLocalRef(env, mRubyObject);
}

static mrb_value addEventFilter_callback(mrb_state *mrb, mrb_value self) {
    JNIEnv *env = getJNIEnv();
    mrb_value rSlug = mrb_proc_cfunc_env_get(mrb, 0);
    mrb_value rEventName = mrb_proc_cfunc_env_get(mrb, 1);
    MRubyInstance *instance = findMRubyInstance(mrb);
    jstring jSlug = convertMrbValueToJava(env, mrb, rSlug);
    jobject jPlugin = call_MRuby_getPlugin(env, instance->javaInstance, jSlug);

    mrb_value *rArgs;
    mrb_int rArgc;
    mrb_get_args(mrb, "*", &rArgs, &rArgc);

    jclass objectClass = (*env)->FindClass(env, JCLASS_OBJECT);
    jobjectArray jArgs = (*env)->NewObjectArray(env, rArgc, objectClass, NULL);
    for (int i = 0; i < rArgc; i++) {
        (*env)->SetObjectArrayElement(env, jArgs, i, convertMrbValueToJava(env, mrb, rArgs[i]));
    }

    jstring jEventName = convertMrbValueToJava(env, mrb, rEventName);
    jobjectArray jResult = call_Plugin_filter(env, jPlugin, jEventName, jArgs);
    mrb_value rResult = convertJavaToMrbValue(env, mrb, jResult);

    (*env)->DeleteLocalRef(env, objectClass);
    (*env)->DeleteLocalRef(env, jSlug);
    (*env)->DeleteLocalRef(env, jPlugin);
    (*env)->DeleteLocalRef(env, jArgs);
    (*env)->DeleteLocalRef(env, jEventName);
    (*env)->DeleteLocalRef(env, jResult);
    return rResult;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_pluggaloid_Plugin_addEventFilterNative(JNIEnv *env, jobject self, jstring eventName) {
    // Get mrb_state
    jobject mRubyObject = getField_Plugin_mRuby(env, self);
    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRubyObject);

    // Get this.slug
    mrb_value rSlug;
    {
        jstring slug = getField_Plugin_slug(env, self);
        rSlug = mrb_symbol_value(convertJstringToSymbol(env, mrb, slug));

        (*env)->DeleteLocalRef(env, slug);
    }

    // String -> Symbol
    mrb_value rEventName = mrb_symbol_value(convertJstringToSymbol(env, mrb, eventName));

    // Find Plugin
    struct RClass *plugin = mrb_class_get_under(mrb, mrb_module_get(mrb, "Pluggaloid"), "Plugin");
    mrb_value rPlugin = mrb_funcall(mrb, mrb_obj_value(plugin), "[]", 1, rSlug);

    // Register filter
    mrb_value procEnv[] = { rSlug, rEventName };
    struct RProc *proc = mrb_proc_new_cfunc_with_env(mrb, addEventFilter_callback, 2, procEnv);
    mrb_funcall_with_block(mrb, rPlugin, mrb_intern_lit(mrb, "add_event_filter"), 1, &rEventName, mrb_obj_value(proc));

    __android_log_print(ANDROID_LOG_DEBUG, "exvoice-Plugin", "register native filter %s%s", mrb_str_to_cstr(mrb, mrb_inspect(mrb, rSlug)), mrb_str_to_cstr(mrb, mrb_inspect(mrb, rEventName)));

    // Release references
    (*env)->DeleteLocalRef(env, mRubyObject);
}

static mrb_value defineSpell_callback(mrb_state *mrb, mrb_value self) {
    JNIEnv *env = getJNIEnv();
    mrb_value rSlug = mrb_proc_cfunc_env_get(mrb, 0);
    mrb_value rCallbackKey = mrb_proc_cfunc_env_get(mrb, 1);
    MRubyInstance *instance = findMRubyInstance(mrb);
    jstring jSlug = convertMrbValueToJava(env, mrb, rSlug);
    jobject jPlugin = call_MRuby_getPlugin(env, instance->javaInstance, jSlug);

    // take arguments
    mrb_value *models;
    mrb_int modelLength;
    mrb_value kwrest;
    mrb_kwargs kwargs = { 0, NULL, NULL, 0, &kwrest };
    mrb_get_args(mrb, "*:", &models, &modelLength, &kwargs);

    // Convert models
    jclass objectClass = (*env)->FindClass(env, JCLASS_OBJECT);
    jobjectArray jModels = (*env)->NewObjectArray(env, modelLength, objectClass, NULL);
    mrb_sym toHash = mrb_intern_lit(mrb, "to_hash");
    for (int i = 0; i < modelLength; i++) {
        mrb_value h = mrb_funcall_argv(mrb, models[i], toHash, 0, NULL);
        (*env)->SetObjectArrayElement(env, jModels, i, convertMrbValueToJava(env, mrb, h));
    }

    // Convert kwrest
    jobject jOptions = convertMrbValueToJava(env, mrb, kwrest);

    jstring jCallbackKey = convertMrbValueToJava(env, mrb, rCallbackKey);
    call_Plugin_onSpell(env, jPlugin, jCallbackKey, jModels, jOptions);

    (*env)->DeleteLocalRef(env, objectClass);
    (*env)->DeleteLocalRef(env, jModels);
    (*env)->DeleteLocalRef(env, jOptions);
    (*env)->DeleteLocalRef(env, jCallbackKey);
    (*env)->DeleteLocalRef(env, jSlug);
    (*env)->DeleteLocalRef(env, jPlugin);

    if ((*env)->ExceptionCheck(env)) {
        jthrowable throwable = (*env)->ExceptionOccurred(env);
        (*env)->ExceptionClear(env);

        mrb_value exc = exvoice_java_error_new(mrb, throwable);
        (*env)->DeleteLocalRef(env, throwable);

        mrb_exc_raise(mrb, exc);
    }

    return mrb_nil_value();
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_pluggaloid_Plugin_defineSpellNative(JNIEnv *env, jobject self, jstring spellName, jobjectArray constraints, jstring callbackKey) {
    // Get mrb_state
    jobject mRubyObject = getField_Plugin_mRuby(env, self);
    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRubyObject);

    // Get this.slug
    mrb_value rSlug;
    {
        jstring slug = getField_Plugin_slug(env, self);
        rSlug = mrb_symbol_value(convertJstringToSymbol(env, mrb, slug));
        (*env)->DeleteLocalRef(env, slug);
    }

    // Convert constraints
    jsize constraintsLength = (*env)->GetArrayLength(env, constraints);
    mrb_sym *rConstraints = mrb_calloc(mrb, constraintsLength, sizeof(mrb_sym));
    for (int i = 0; i < constraintsLength; i++) {
        jobject obj = (*env)->GetObjectArrayElement(env, constraints, i);
        rConstraints[i] = convertJstringToSymbol(env, mrb, obj);
        (*env)->DeleteLocalRef(env, obj);
    }

    // Make callback
    mrb_value rCallbackKey;
    {
        const char *cCallbackKey = (*env)->GetStringUTFChars(env, callbackKey, NULL);
        rCallbackKey = mrb_str_new_cstr(mrb, cCallbackKey);
        (*env)->ReleaseStringUTFChars(env, callbackKey, cCallbackKey);
    }
    mrb_value procEnv[] = { rSlug, rCallbackKey };
    struct RProc *proc = mrb_proc_new_cfunc_with_env(mrb, defineSpell_callback, 2, procEnv);

    // Register spell
    const char *cSpellName = (*env)->GetStringUTFChars(env, spellName, NULL);
    mix_define_spell(mrb, cSpellName, constraintsLength, rConstraints, mrb_nil_value(), mrb_obj_value(proc));

    __android_log_print(ANDROID_LOG_DEBUG, "exvoice-Plugin", "register native spell %s@%d", cSpellName, constraintsLength);

    // Release references
    (*env)->ReleaseStringUTFChars(env, spellName, cSpellName);
    (*env)->DeleteLocalRef(env, mRubyObject);
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_pluggaloid_Plugin_call(JNIEnv *env, jclass clazz, jobject mRuby, jstring eventName, jobjectArray args) {
    jobject mutex = call_MRuby_getMutex(env, mRuby);
    (*env)->MonitorEnter(env, mutex);

    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRuby);
    int arenaIndex = mrb_gc_arena_save(mrb);

    struct RClass *plugin = mrb_class_get_under(mrb, mrb_module_get(mrb, "Pluggaloid"), "Plugin");

    // Create call args array
    jsize argc = (*env)->GetArrayLength(env, args);
    mrb_value *rArgs = mrb_calloc(mrb, (size_t) 1 + argc, sizeof(mrb_value));
    rArgs[0] = mrb_symbol_value(convertJstringToSymbol(env, mrb, eventName));
    for (int i = 0; i < argc; i++) {
        // Convert argument to mrb_value
        jobject obj = (*env)->GetObjectArrayElement(env, args, i);
        rArgs[i + 1] = convertJavaToMrbValue(env, mrb, obj);
        (*env)->DeleteLocalRef(env, obj);
    }

    // Call event
    mrb_value result = mrb_funcall_argv(mrb, mrb_obj_value(plugin), mrb_intern_lit(mrb, "call"), 1 + argc, rArgs);
    mrb_free(mrb, rArgs);

    if (mrb_exception_p(result)) {
        jclass runtimeExceptionClass = (*env)->FindClass(env, JCLASS_EXVOICE_MRUBY_EXCEPTION);
        mrb_value ins = mrb_inspect(mrb, result);
        (*env)->ThrowNew(env, runtimeExceptionClass, mrb_str_to_cstr(mrb, ins));

        (*env)->DeleteLocalRef(env, runtimeExceptionClass);

        mrb->exc = 0;
    }

    mrb_gc_arena_restore(mrb, arenaIndex);

    (*env)->MonitorExit(env, mutex);
    (*env)->DeleteLocalRef(env, mutex);
}

JNIEXPORT jobjectArray JNICALL Java_info_shibafu528_yukari_exvoice_pluggaloid_Plugin_filtering(JNIEnv *env, jclass clazz, jobject mRuby, jstring eventName, jobjectArray args) {
    jobject mutex = call_MRuby_getMutex(env, mRuby);
    (*env)->MonitorEnter(env, mutex);

    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRuby);
    int arenaIndex = mrb_gc_arena_save(mrb);

    struct RClass *plugin = mrb_class_get_under(mrb, mrb_module_get(mrb, "Pluggaloid"), "Plugin");

    // Create filtering args array
    jsize argc = (*env)->GetArrayLength(env, args);
    mrb_value *rArgs = mrb_calloc(mrb, (size_t) 1 + argc, sizeof(mrb_value));
    rArgs[0] = mrb_symbol_value(convertJstringToSymbol(env, mrb, eventName));
    for (int i = 0; i < argc; i++) {
        // Convert argument to mrb_value
        jobject obj = (*env)->GetObjectArrayElement(env, args, i);
        rArgs[i + 1] = convertJavaToMrbValue(env, mrb, obj);
        (*env)->DeleteLocalRef(env, obj);
    }

    // Call filtering
    mrb_value filteringResult = mrb_funcall_argv(mrb, mrb_obj_value(plugin), mrb_intern_lit(mrb, "filtering"), 1 + argc, rArgs);
    mrb_free(mrb, rArgs);

    if (mrb_exception_p(filteringResult)) {
        struct RClass *filterError = mrb_class_get_under(mrb, mrb_module_get(mrb, "Pluggaloid"), "FilterError");
        if (mrb_exc_ptr(filteringResult)->c == filterError) {
            jclass exceptionClass = (*env)->FindClass(env, NS_EXVOICE "pluggaloid/FilterException");
            mrb_value ins = mrb_inspect(mrb, filteringResult);
            (*env)->ThrowNew(env, exceptionClass, mrb_str_to_cstr(mrb, ins));

            (*env)->DeleteLocalRef(env, exceptionClass);
        } else {
            jclass runtimeExceptionClass = (*env)->FindClass(env, JCLASS_EXVOICE_MRUBY_EXCEPTION);
            mrb_value ins = mrb_inspect(mrb, filteringResult);
            (*env)->ThrowNew(env, runtimeExceptionClass, mrb_str_to_cstr(mrb, ins));

            (*env)->DeleteLocalRef(env, runtimeExceptionClass);
        }

        mrb->exc = 0;

        (*env)->MonitorExit(env, mutex);
        (*env)->DeleteLocalRef(env, mutex);
        return NULL;
    }

    // Create Result Array
    mrb_int resultLength = RARRAY_LEN(filteringResult);
    jobjectArray results;
    {
        jclass objectClass = (*env)->FindClass(env, JCLASS_OBJECT);
        results = (*env)->NewObjectArray(env, resultLength, objectClass, NULL);

        (*env)->DeleteLocalRef(env, objectClass);
    }

    // Convert mrb_value to Java Object
    for (int i = 0; i < resultLength; i++) {
        mrb_value v = mrb_ary_ref(mrb, filteringResult, i);
        (*env)->SetObjectArrayElement(env, results, i, convertMrbValueToJava(env, mrb, v));
    }

    mrb_gc_arena_restore(mrb, arenaIndex);

    (*env)->MonitorExit(env, mutex);
    (*env)->DeleteLocalRef(env, mutex);

    return results;
}
