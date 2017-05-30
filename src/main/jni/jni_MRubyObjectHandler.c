#include <jni.h>
#include <mruby.h>

JNIEXPORT jobject JNICALL Java_info_shibafu528_yukari_exvoice_MRubyObjectHandler_invokeNative(JNIEnv *env, jobject self, jlong jMRubyPointer, jlong jMrbObjectPointer, jstring jMethodName, jobjectArray jArgs) {
    // TODO: not implemented
    return NULL;
}

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_MRubyObjectHandler_disposeNative(JNIEnv *env, jobject self, jlong jMRubyPointer, jlong jMrbObjectPointer) {
    mrb_state *mrb = (mrb_state *) jMRubyPointer;
    if (mrb != NULL) {
        mrb_value *object = (mrb_value *) jMrbObjectPointer;
        mrb_gc_unregister(mrb, *object);
    }
}
