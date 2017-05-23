#ifndef YUKARI_EXVOICE_JNIMETHODINFO_H
#define YUKARI_EXVOICE_JNIMETHODINFO_H

#include <memory>
#include <jni.h>
#include "JNIObject.h"
#include "JNILocalRef.h"

namespace JNI {

    template <typename JNIType>
    struct JNITypeConverter {
        static JNILocalRef<JNIType> toNative(JNIEnv *env, JNIType ref) {
            return JNILocalRef<JNIType>(env, ref);
        }
    };

    template <>
    struct JNITypeConverter<void> {
        static void toNative(JNIEnv *env, void *ref) {
            return;
        }
    };

    template <typename JNIReturnType>
    struct JNIMethodCaller {
        template <typename JNIReceiverType, typename ... JNIParamTypes>
        static JNIReturnType call(const JNIObject<JNIReceiverType> &receiver, jmethodID methodId, JNIObject<JNIParamTypes>... params) {
            return static_cast<JNIReturnType>(receiver.getEnv()->CallObjectMethod(receiver, methodId, params...));
        }
    };

    template <>
    struct JNIMethodCaller<void> {
        template <typename JNIReceiverType, typename ... JNIParamTypes>
        static void call(const JNIObject<JNIReceiverType> &receiver, jmethodID methodId, JNIObject<JNIParamTypes>... params) {
            receiver.getEnv()->CallVoidMethod(receiver, methodId, params...);
        }
    };

    template <typename JNIReturnType, typename ... JNIParamTypes>
    class JNIMethodInfo {
    private:
        jmethodID _methodId;

    public:
        JNIMethodInfo(JNIEnv *env, const char* className, const char* methodName, const char* methodSignature) {
            std::unique_ptr<JNILocalRef<jclass>> clazz(new JNILocalRef<jclass>(env, env->FindClass(className)));
            _methodId = env->GetMethodID(*clazz, methodName, methodSignature);
        }

        JNIMethodInfo(const JNILocalRef<jclass> &clazz, const char* methodName, const char* methodSignature) {
            _methodId = clazz.getEnv()->GetMethodID(clazz, methodName, methodSignature);
        }

        template <typename JNIReceiverType>
        JNILocalRef<JNIReturnType> call(const JNIObject<JNIReceiverType> &receiver, JNIObject<JNIParamTypes>... params) {
            return JNITypeConverter<JNIReturnType>::toNative(receiver.getEnv(), JNIMethodCaller<JNIReturnType>::call(receiver, _methodId, params...));
        }

        JNILocalRef<JNIReturnType> call(const JNIObject<jclass> &receiver, JNIObject<JNIParamTypes>... params) {
            return JNILocalRef<JNIReturnType>(receiver.getEnv(), static_cast<JNIReturnType>(receiver.getEnv()->CallStaticObjectMethod(receiver, _methodId, (JNIParamTypes) params...)));
        }
    };

}

#endif // YUKARI_EXVOICE_JNIMETHODINFO_H