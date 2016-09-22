#include <jni.h>
#include <mruby.h>
#include <mruby/value.h>
#include <mruby/array.h>
#include <mruby/hash.h>
#include <mruby/string.h>
#include <mruby/proc.h>
#include <mruby/error.h>
#include <stddef.h>
#include <android/log.h>
#include "MRuby.h"
#include "jni_converter.h"
#include "jni_common.h"

static inline jobject getField_Plugin_mRuby(JNIEnv *env, jobject self) {
    static jfieldID field_Plugin_mRuby = NULL;
    if (field_Plugin_mRuby == NULL) {
        jclass selfClass = (*env)->GetObjectClass(env, self);
        field_Plugin_mRuby = (*env)->GetFieldID(env, selfClass, "mRuby", "Linfo/shibafu528/yukari/exvoice/MRuby;");

        (*env)->DeleteLocalRef(env, selfClass);
    }
    return (*env)->GetObjectField(env, self, field_Plugin_mRuby);
}

static inline jobject getField_Plugin_slug(JNIEnv *env, jobject self) {
    static jfieldID field_Plugin_slug = NULL;
    if (field_Plugin_slug == NULL) {
        jclass selfClass = (*env)->GetObjectClass(env, self);
        field_Plugin_slug = (*env)->GetFieldID(env, selfClass, "slug", "Ljava/lang/String;");

        (*env)->DeleteLocalRef(env, selfClass);
    }
    return (*env)->GetObjectField(env, self, field_Plugin_slug);
}

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

static inline mrb_sym convertJstringToSymbol(JNIEnv *env, mrb_state *mrb, jstring str) {
    const char *cstr = (*env)->GetStringUTFChars(env, str, NULL);
    mrb_sym sym = mrb_intern_cstr(mrb, cstr);
    (*env)->ReleaseStringUTFChars(env, str, cstr);

    return sym;
}

static inline mrb_value convertJstringToString(JNIEnv *env, mrb_state *mrb, jstring str) {
    const char *cstr = (*env)->GetStringUTFChars(env, str, NULL);
    mrb_value rstr = mrb_str_new_cstr(mrb, cstr);
    (*env)->ReleaseStringUTFChars(env, str, cstr);

    return rstr;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_Plugin_initializeNative(JNIEnv *env, jobject self) {
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
    mrb_funcall_argv(mrb, mrb_obj_value(plugin), mrb_intern_cstr(mrb, "create"), 1, &rSlug);

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

    jclass objectClass = (*env)->FindClass(env, "java/lang/Object");
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

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_Plugin_addEventListenerNative(JNIEnv *env, jobject self, jstring eventName) {
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
    mrb_funcall_with_block(mrb, rPlugin, mrb_intern_cstr(mrb, "add_event"), 1, &rEventName, mrb_obj_value(proc));

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

    jclass objectClass = (*env)->FindClass(env, "java/lang/Object");
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

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_Plugin_addEventFilterNative(JNIEnv *env, jobject self, jstring eventName) {
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
    mrb_funcall_with_block(mrb, rPlugin, mrb_intern_cstr(mrb, "add_event_filter"), 1, &rEventName, mrb_obj_value(proc));

    __android_log_print(ANDROID_LOG_DEBUG, "exvoice-Plugin", "register native filter %s%s", mrb_str_to_cstr(mrb, mrb_inspect(mrb, rSlug)), mrb_str_to_cstr(mrb, mrb_inspect(mrb, rEventName)));

    // Release references
    (*env)->DeleteLocalRef(env, mRubyObject);
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_Plugin_call(JNIEnv *env, jclass clazz, jobject mRuby, jstring eventName, jobjectArray args) {
    jobject mutex = call_MRuby_getMutex(env, mRuby);
    (*env)->MonitorEnter(env, mutex);

    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRuby);

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
    mrb_value result = mrb_funcall_argv(mrb, mrb_obj_value(plugin), mrb_intern_cstr(mrb, "call"), 1 + argc, rArgs);
    mrb_free(mrb, rArgs);

    if (mrb_exception_p(result)) {
        jclass runtimeExceptionClass = (*env)->FindClass(env, "info/shibafu528/yukari/exvoice/MRubyException");
        mrb_value ins = mrb_inspect(mrb, result);
        (*env)->ThrowNew(env, runtimeExceptionClass, mrb_str_to_cstr(mrb, ins));

        (*env)->DeleteLocalRef(env, runtimeExceptionClass);

        mrb->exc = 0;
    }

    (*env)->MonitorExit(env, mutex);
    (*env)->DeleteLocalRef(env, mutex);
}

JNIEXPORT jobjectArray JNICALL Java_info_shibafu528_yukari_exvoice_Plugin_filtering(JNIEnv *env, jclass clazz, jobject mRuby, jstring eventName, jobjectArray args) {
    jobject mutex = call_MRuby_getMutex(env, mRuby);
    (*env)->MonitorEnter(env, mutex);

    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRuby);

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
    mrb_value filteringResult = mrb_funcall_argv(mrb, mrb_obj_value(plugin), mrb_intern_cstr(mrb, "filtering"), 1 + argc, rArgs);
    mrb_free(mrb, rArgs);

    if (mrb_exception_p(filteringResult)) {
        struct RClass *filterError = mrb_class_get_under(mrb, mrb_module_get(mrb, "Pluggaloid"), "FilterError");
        if (mrb_exc_ptr(filteringResult)->c == filterError) {
            jclass exceptionClass = (*env)->FindClass(env, "info/shibafu528/yukari/exvoice/FilterException");
            mrb_value ins = mrb_inspect(mrb, filteringResult);
            (*env)->ThrowNew(env, exceptionClass, mrb_str_to_cstr(mrb, ins));

            (*env)->DeleteLocalRef(env, exceptionClass);
        } else {
            jclass runtimeExceptionClass = (*env)->FindClass(env, "info/shibafu528/yukari/exvoice/MRubyException");
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
        jclass objectClass = (*env)->FindClass(env, "java/lang/Object");
        results = (*env)->NewObjectArray(env, resultLength, objectClass, NULL);

        (*env)->DeleteLocalRef(env, objectClass);
    }

    // Convert mrb_value to Java Object
    for (int i = 0; i < resultLength; i++) {
        mrb_value v = mrb_ary_ref(mrb, filteringResult, i);
        (*env)->SetObjectArrayElement(env, results, i, convertMrbValueToJava(env, mrb, v));
    }

    (*env)->MonitorExit(env, mutex);
    (*env)->DeleteLocalRef(env, mutex);

    return results;
}
