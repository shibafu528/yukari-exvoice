#ifndef YUKARI_EXVOICE_GENERATOR_H
#define YUKARI_EXVOICE_GENERATOR_H

#include <jni.h>

#define OBJECT_FIELD_READER(funcName, fieldName, typeSignature) \
static inline jobject funcName (JNIEnv *env, jobject self) { \
    static jfieldID fid = NULL; \
    if (fid == NULL) { \
        jclass selfClass = (*env)->GetObjectClass(env, self); \
        fid = (*env)->GetFieldID(env, selfClass, #fieldName, typeSignature); \
        (*env)->DeleteLocalRef(env, selfClass); \
    } \
    return (*env)->GetObjectField(env, self, fid); \
}

#endif //YUKARI_EXVOICE_GENERATOR_H