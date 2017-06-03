#include <jni.h>
#include <mruby.h>
#include <mruby/string.h>
#include "converter.h"
#include "generator.h"

JNIEXPORT jobject JNICALL Java_info_shibafu528_yukari_exvoice_MRubyObjectHandler_invokeNative(JNIEnv *env, jobject self, jlong jMRubyPointer, jlong jMrbObjectPointer, jstring jMethodName, jobjectArray jArgs) {
    mrb_state *mrb = (mrb_state *) jMRubyPointer;
    jobject jResult = NULL;

    if (mrb != NULL) {
        mrb_value *object = (mrb_value *) jMrbObjectPointer;
        const char *methodName = (*env)->GetStringUTFChars(env, jMethodName, NULL);

        // Create call args array
        jsize argc = 0;
        mrb_value *rArgs = NULL;
        if (jArgs != NULL) {
            argc = (*env)->GetArrayLength(env, jArgs);
            if (argc > 0) {
                rArgs = mrb_calloc(mrb, (size_t) argc, sizeof(mrb_value));
                for (int i = 0; i < argc; i++) {
                    // Convert argument to mrb_value
                    jobject obj = (*env)->GetObjectArrayElement(env, jArgs, i);
                    rArgs[i] = convertJavaToMrbValue(env, mrb, obj);
                    (*env)->DeleteLocalRef(env, obj);
                }
            }
        }

        // Invoke mruby method
        mrb_value rResult = mrb_funcall_argv(mrb, *object, mrb_intern_cstr(mrb, methodName), argc, rArgs);
        if (rArgs != NULL) {
            mrb_free(mrb, rArgs);
        }

        // Handle exception or convert result mrb_value to jobject
        if (mrb_exception_p(rResult)) {
            jclass runtimeExceptionClass = (*env)->FindClass(env, JCLASS_EXVOICE_MRUBY_EXCEPTION);
            mrb_value ins = mrb_inspect(mrb, rResult);
            (*env)->ThrowNew(env, runtimeExceptionClass, mrb_str_to_cstr(mrb, ins));

            (*env)->DeleteLocalRef(env, runtimeExceptionClass);

            mrb->exc = 0;
        } else {
            jResult = convertMrbValueToJava(env, mrb, rResult);
        }

        (*env)->ReleaseStringUTFChars(env, jMethodName, methodName);
    }

    return jResult;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_MRubyObjectHandler_disposeNative(JNIEnv *env, jobject self, jlong jMRubyPointer, jlong jMrbObjectPointer) {
    mrb_state *mrb = (mrb_state *) jMRubyPointer;
    if (mrb != NULL) {
        mrb_value *object = (mrb_value *) jMrbObjectPointer;
        mrb_gc_unregister(mrb, *object);
    }
}
