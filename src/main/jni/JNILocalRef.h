#ifndef YUKARI_EXVOICE_JNILOCALREF_H
#define YUKARI_EXVOICE_JNILOCALREF_H

#include <jni.h>

namespace JNI {

    /**
     * LocalRefのラッパー
     */
    template<typename JNIType>
    class JNILocalRef {
        friend class JNIGlobalRef;

    private:
        JNIEnv *_env;
        JNIType _ref;

    public:
        JNILocalRef(JNIEnv *env, JNIType ref)
                : _env(env),
                  _ref(env->NewLocalRef(ref)) {}

        JNILocalRef(const JNILocalRef &obj)
                : _env(obj._env),
                  _ref(obj._env->NewLocalRef(obj._ref)) {}

        JNILocalRef(JNILocalRef &&obj) noexcept
                : _env(obj._env),
                  _ref(obj._ref) {
            obj._env = nullptr;
            obj._ref = nullptr;
        }

        ~JNIReference() {
            if (_env != nullptr && _ref != nullptr) {
                _env->DeleteLocalRef(_ref);
                _env = nullptr;
                _ref = nullptr;
            }
        }

        JNILocalRef &operator=(const JNILocalRef &obj) {
            if (this != &obj) {
                _env->DeleteLocalRef(_ref);

                _env = obj._env;
                _ref = obj._env->NewLocalRef(obj._ref);
            }
            return *this;
        }

        operator JNIType() const {
            return _ref;
        }
    };

}

#endif // YUKARI_EXVOICE_JNILOCALREF_H