#ifndef YUKARI_EXVOICE_JNIGLOBALREF_H
#define YUKARI_EXVOICE_JNIGLOBALREF_H

#include <jni.h>
#include "JNILocalRef.h"

namespace JNI {

    /**
     * GlobalRefのラッパー
     */
    template<typename JNIType>
    class JNIGlobalRef {
    private:
        JNIEnv *_env;
        JNIType _ref;

    public:
        JNIGlobalRef(JNIEnv *env, JNIType ref)
                : _env(env),
                  _ref(env->NewGlobalRef(ref)) {}

        JNIGlobalRef(const JNIGlobalRef &obj)
                : _env(obj._env),
                  _ref(obj._env->NewGlobalRef(obj._ref)) {}

        JNIGlobalRef(const JNILocalRef &obj)
                : _env(obj._env),
                  _ref(obj._env->NewGlobalRef(obj._ref)) {}

        JNIGlobalRef(JNIGlobalRef &&obj) noexcept
                : _env(obj._env),
                  _ref(obj._ref) {
            obj._env = nullptr;
            obj._ref = nullptr;
        }

        ~JNIReference() {
            if (_env != nullptr && _ref != nullptr) {
                _env->DeleteGlobalRef(_ref);
                _env = nullptr;
                _ref = nullptr;
            }
        }

        JNIGlobalRef &operator=(const JNIGlobalRef &obj) {
            if (this != &obj) {
                _env->DeleteGlobalRef(_ref);

                _env = obj._env;
                _ref = obj._env->NewGlobalRef(obj._ref);
            }
            return *this;
        }

        operator JNIType() const {
            return _ref;
        }
    };

}

#endif // YUKARI_EXVOICE_JNIGLOBALREF_H