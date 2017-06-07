#include <jni.h>
#include <mruby.h>
#include <mruby/variable.h>
#include <mruby/class.h>
#include "common.h"
#include "converter.h"

static jmethodID configLoader_containsKey = NULL;
static jmethodID configLoader_getInt = NULL;
static jmethodID configLoader_getLong = NULL;
static jmethodID configLoader_getFloat = NULL;
static jmethodID configLoader_getBoolean = NULL;
static jmethodID configLoader_getString = NULL;
static jmethodID configLoader_putInt = NULL;
static jmethodID configLoader_putLong = NULL;
static jmethodID configLoader_putFloat = NULL;
static jmethodID configLoader_putBoolean = NULL;
static jmethodID configLoader_putString = NULL;

static void prepareJNIIdentifiers() {
    JNIEnv *env = getJNIEnv();

    {
        jclass configLoader = (*env)->FindClass(env, "info/shibafu528/yukari/exvoice/ConfigLoader");

        configLoader_containsKey = (*env)->GetMethodID(env, configLoader, "containsKey", "(Ljava/lang/String;)Z");
        configLoader_getInt = (*env)->GetMethodID(env, configLoader, "getInt", "(Ljava/lang/String;)I");
        configLoader_getLong = (*env)->GetMethodID(env, configLoader, "getLong", "(Ljava/lang/String;)J");
        configLoader_getFloat = (*env)->GetMethodID(env, configLoader, "getFloat", "(Ljava/lang/String;)F");
        configLoader_getBoolean = (*env)->GetMethodID(env, configLoader, "getBoolean", "(Ljava/lang/String;)Z");
        configLoader_getString = (*env)->GetMethodID(env, configLoader, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
        configLoader_putInt = (*env)->GetMethodID(env, configLoader, "putInt", "(Ljava/lang/String;I)V");
        configLoader_putLong = (*env)->GetMethodID(env, configLoader, "putLong", "(Ljava/lang/String;J)V");
        configLoader_putFloat = (*env)->GetMethodID(env, configLoader, "putFloat", "(Ljava/lang/String;F)V");
        configLoader_putBoolean = (*env)->GetMethodID(env, configLoader, "putBoolean", "(Ljava/lang/String;Z)V");
        configLoader_putString = (*env)->GetMethodID(env, configLoader, "putString", "(Ljava/lang/String;Ljava/lang/String;)V");

        (*env)->DeleteLocalRef(env, configLoader);
    }
}

static inline jobject getModuleCV_jobjectReference(mrb_state *mrb) {
    struct RClass *configloader = mrb_module_get(mrb, "ConfigLoader");
    mrb_value mrbVal = mrb_mod_cv_get(mrb, configloader, mrb_intern_lit(mrb, "jobjectReference"));
    jobject jVal = (jobject) mrb_cptr(mrbVal);

    return jVal;
}

static mrb_value _jni_containsKey(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_get_args(mrb, "S", &key);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jboolean jResult = (*env)->CallBooleanMethod(env, jobjectReference, configLoader_containsKey, jKey);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_bool_value(jResult);
}

static mrb_value _jni_getInt(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_get_args(mrb, "S", &key);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jint jResult = (*env)->CallIntMethod(env, jobjectReference, configLoader_getInt, jKey);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_fixnum_value(jResult);
}

static mrb_value _jni_getLong(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_get_args(mrb, "S", &key);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jlong jResult = (*env)->CallLongMethod(env, jobjectReference, configLoader_getLong, jKey);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_fixnum_value(jResult);
}

static mrb_value _jni_getFloat(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_get_args(mrb, "S", &key);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jfloat jResult = (*env)->CallFloatMethod(env, jobjectReference, configLoader_getFloat, jKey);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_float_value(mrb, jResult);
}

static mrb_value _jni_getBoolean(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_get_args(mrb, "S", &key);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jboolean jResult = (*env)->CallBooleanMethod(env, jobjectReference, configLoader_getBoolean, jKey);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_bool_value(jResult);
}

static mrb_value _jni_getString(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_get_args(mrb, "S", &key);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jstring jResult = (*env)->CallObjectMethod(env, jobjectReference, configLoader_getString, jKey);
    mrb_value mResult = convertJavaToMrbValue(env, mrb, jResult);

    (*env)->DeleteLocalRef(env, jKey);
    (*env)->DeleteLocalRef(env, jResult);

    return mResult;
}

static mrb_value _jni_putInt(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_int val;
    mrb_get_args(mrb, "Si", &key, &val);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    (*env)->CallVoidMethod(env, jobjectReference, configLoader_putInt, jKey, val);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_fixnum_value(val);
}

static mrb_value _jni_putLong(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_int val;
    mrb_get_args(mrb, "Si", &key, &val);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    (*env)->CallVoidMethod(env, jobjectReference, configLoader_putLong, jKey, val);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_fixnum_value(val);
}

static mrb_value _jni_putFloat(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_float val;
    mrb_get_args(mrb, "Sf", &key, &val);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    (*env)->CallVoidMethod(env, jobjectReference, configLoader_putFloat, jKey, val);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_float_value(mrb, val);
}

static mrb_value _jni_putBoolean(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_bool val;
    mrb_get_args(mrb, "Sb", &key, &val);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    (*env)->CallVoidMethod(env, jobjectReference, configLoader_putBoolean, jKey, val);

    (*env)->DeleteLocalRef(env, jKey);

    return mrb_bool_value(val);
}

static mrb_value _jni_putString(mrb_state *mrb, mrb_value self) {
    mrb_value key;
    mrb_value val;
    mrb_get_args(mrb, "SS", &key, &val);

    JNIEnv *env = getJNIEnv();
    jobject jobjectReference = getModuleCV_jobjectReference(mrb);
    jstring jKey = convertMrbValueToJava(env, mrb, key);
    jstring jVal = convertMrbValueToJava(env, mrb, val);
    (*env)->CallVoidMethod(env, jobjectReference, configLoader_putString, jKey, jVal);

    (*env)->DeleteLocalRef(env, jKey);
    (*env)->DeleteLocalRef(env, jVal);

    return val;
}

void exvoice_init_configloader(mrb_state *mrb) {
    struct RClass *configloader = mrb_define_module(mrb, "ConfigLoader");

    mrb_define_module_function(mrb, configloader, "_jni_containsKey", _jni_containsKey, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, configloader, "_jni_getInt", _jni_getInt, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, configloader, "_jni_getLong", _jni_getLong, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, configloader, "_jni_getFloat", _jni_getFloat, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, configloader, "_jni_getBoolean", _jni_getBoolean, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, configloader, "_jni_getString", _jni_getString, MRB_ARGS_REQ(1));
    mrb_define_module_function(mrb, configloader, "_jni_putInt", _jni_putInt, MRB_ARGS_REQ(2));
    mrb_define_module_function(mrb, configloader, "_jni_putLong", _jni_putLong, MRB_ARGS_REQ(2));
    mrb_define_module_function(mrb, configloader, "_jni_putFloat", _jni_putFloat, MRB_ARGS_REQ(2));
    mrb_define_module_function(mrb, configloader, "_jni_putBoolean", _jni_putBoolean, MRB_ARGS_REQ(2));
    mrb_define_module_function(mrb, configloader, "_jni_putString", _jni_putString, MRB_ARGS_REQ(2));

    prepareJNIIdentifiers();
}