#ifndef YUKARI_EXVOICE_JNIOBJECT_H
#define YUKARI_EXVOICE_JNIOBJECT_H

#include <jni.h>

namespace JNI {

    template <typename JNIType>
    class JNILocalRef;

    template <typename JNIType>
    class JNIObject {
    protected:
        JNIEnv *_env;
        JNIType _ref;

    public:
        JNIObject(JNIEnv *env, JNIType ref)
                : _env(env),
                  _ref(ref) {}

        JNIEnv *getEnv() const {
            return _env;
        }

        JNILocalRef<jclass> getClass();

        operator JNIType() const {
            return _ref;
        }
    };

}

#endif // YUKARI_EXVOICE_JNIOBJECT_H