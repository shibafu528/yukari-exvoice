#include "JNIObject.h"
#include "JNILocalRef.h"
#include "JNIMethodInfo.h"
#include <memory>

extern "C" {
#include "generator.h"
}

using namespace JNI;

template <typename JNIType>
JNILocalRef<jclass> JNIObject<JNIType>::getClass() {
    return JNILocalRef<jclass>(_env, _env->GetObjectClass(_ref));
}

JNILocalRef<jclass> JNIObject<void>::getClass() {
    return JNILocalRef<jclass>(_env, _env->FindClass("java/lang/Void"));
}

void callsample(JNIObject<jobject> &receiver, jstring str) {
    auto mi = JNIMethodInfo<jstring, jstring>(receiver.getEnv(), JCLASS_EXVOICE_MRUBY, "callTopLevelFunc", JSIG_METHOD(JSIG_OBJECT, JSIG_STRING));
    auto jStr = JNILocalRef<jstring>(receiver.getEnv(), str);
    const JNILocalRef<jstring> local = mi.call(receiver, jStr);
}

void callsample2(JNIObject<jobject> &receiver, jstring str) {
    const JNILocalRef<jclass> receiverClazz = receiver.getClass();
    auto mi = JNIMethodInfo<jstring, jstring>(receiverClazz, "callTopLevelFunc", JSIG_METHOD(JSIG_OBJECT, JSIG_STRING));
    auto jStr = JNILocalRef<jstring>(receiver.getEnv(), str);
    const JNILocalRef<jstring> local = mi.call(receiver, jStr);
}

void callsample3(JNIObject<jobject> &receiver, jstring str) {
    const JNILocalRef<jclass> receiverClazz = receiver.getClass();
    auto mi = JNIMethodInfo<void, jstring>(receiverClazz, "callTopLevelFunc", JSIG_METHOD(JSIG_VOID, JSIG_STRING));
    auto jStr = JNILocalRef<jstring>(receiver.getEnv(), str);
    mi.call(receiver, jStr);
}