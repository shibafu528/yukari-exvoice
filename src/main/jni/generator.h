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

#define JSIG_VOID "V"
#define JSIG_INT "I"
#define JSIG_LONG "J"
#define JSIG_SHORT "S"
#define JSIG_FLOAT "F"
#define JSIG_DOUBLE "D"
#define JSIG_BOOL "Z"
#define JSIG_BYTE "B"
#define JSIG_CHAR "C"
#define JSIG_CLASS(fqcn) "L" fqcn ";"
#define JSIG_ARRAY(type) "[" type
#define JSIG_METHOD(returnType, ...) "(" __VA_ARGS__ ")" returnType

#define JCLASS_OBJECT "java/lang/Object"
#define JCLASS_STRING "java/lang/String"
#define JCLASS_LIST "java/util/List"
#define JCLASS_MAP "java/util/Map"
#define JCLASS_SET "java/util/Set"

#define JSIG_OBJECT JSIG_CLASS(JCLASS_OBJECT)
#define JSIG_STRING JSIG_CLASS(JCLASS_STRING)
#define JSIG_LIST JSIG_CLASS(JCLASS_LIST)
#define JSIG_MAP JSIG_CLASS(JCLASS_MAP)
#define JSIG_SET JSIG_CLASS(JCLASS_SET)

#define NS_EXVOICE "info/shibafu528/yukari/exvoice/"
#define JCLASS_EXVOICE_MRUBY NS_EXVOICE "MRuby"
#define JCLASS_EXVOICE_MRUBY_EXCEPTION NS_EXVOICE "MRubyException"
#define JSIG_EXVOICE_MRUBY JSIG_CLASS(JCLASS_EXVOICE_MRUBY)
#define JSIG_EXVOICE_MRUBY_EXCEPTION JSIG_CLASS(JCLASS_EXVOICE_MRUBY_EXCEPTION)

#endif //YUKARI_EXVOICE_GENERATOR_H