#include <jni.h>
#include <mruby.h>

JNIEXPORT jlong JNICALL Java_info_shibafu528_yukari_exvoice_diva_ModelFactory_newInstanceNative(JNIEnv *env, jclass clazz, jlong jMRubyPointer, jstring jModelClassName, jobject valuesMap) {
    const char *modelClassName = (*env)->GetStringUTFChars(env, jModelClassName, NULL);

    // TODO: not implemented

    (*env)->ReleaseStringUTFChars(env, jModelClassName, modelClassName);

    return NULL;
}