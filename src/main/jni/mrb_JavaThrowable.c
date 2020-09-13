#include "mrb_JavaThrowable.h"
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/variable.h>
#include "common.h"

static void throwable_free(mrb_state *mrb, void *ptr) {
    JNIEnv *env = getJNIEnv();
    (*env)->DeleteGlobalRef(env, ptr);
}

const static struct mrb_data_type mrb_javathrowable_type = {"JavaThrowable", throwable_free };

static jmethodID throwable_getMessage = NULL;

static void prepareJNIIdentifiers() {
    JNIEnv *env = getJNIEnv();

    {
        jclass throwable = (*env)->FindClass(env, "java/lang/Throwable");

        throwable_getMessage = (*env)->GetMethodID(env, throwable, "getMessage", "()Ljava/lang/String;");

        (*env)->DeleteLocalRef(env, throwable);
    }
}

static mrb_value throwable_initialize(mrb_state *mrb, mrb_value self) {
    mrb_int addr;
    mrb_get_args(mrb, "i", &addr);
    jobject ptr = (jobject) addr;

    JNIEnv *env = getJNIEnv();
    ptr = (*env)->NewGlobalRef(env, ptr);

    DATA_TYPE(self) = &mrb_javathrowable_type;
    DATA_PTR(self) = ptr;

    return self;
}

static mrb_value throwable_to_s(mrb_state *mrb, mrb_value self) {
    JNIEnv *env = getJNIEnv();
    jthrowable exc = DATA_PTR(self);

    mrb_value rMessage = mrb_nil_value();
    jstring jMessage = (*env)->CallObjectMethod(env, exc, throwable_getMessage);
    if (jMessage != NULL) {
        const char *cMessage = (*env)->GetStringUTFChars(env, jMessage, NULL);
        rMessage = mrb_str_new_cstr(mrb, cMessage);
        (*env)->ReleaseStringUTFChars(env, jMessage, cMessage);
        (*env)->DeleteLocalRef(env, jMessage);
    }

    return rMessage;
}

void exvoice_init_javathrowable(mrb_state *mrb) {
    struct RClass *throwable = mrb_define_class(mrb, "JavaThrowable", mrb->object_class);
    MRB_SET_INSTANCE_TT(throwable, MRB_TT_DATA);

    mrb_define_method(mrb, throwable, "initialize", throwable_initialize, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, throwable, "initialize_copy", mrb_notimplement_m, MRB_ARGS_REQ(1));
    mrb_define_method(mrb, throwable, "to_s", throwable_to_s, MRB_ARGS_NONE());

    prepareJNIIdentifiers();
}

mrb_value exvoice_java_error_new(mrb_state *mrb, jthrowable exc) {
    struct RClass *error = mrb_class_get(mrb, "JavaError");
    struct RClass *throwable = mrb_class_get(mrb, "JavaThrowable");

    mrb_value ptrValue = mrb_fixnum_value((mrb_int) exc);
    mrb_value rThrowable = mrb_obj_new(mrb, throwable, 1, &ptrValue);
    mrb_value rError = mrb_obj_new(mrb, error, 1, &rThrowable);

    return rError;
}