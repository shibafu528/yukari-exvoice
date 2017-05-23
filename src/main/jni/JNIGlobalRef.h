#ifndef YUKARI_EXVOICE_JNIGLOBALREF_H
#define YUKARI_EXVOICE_JNIGLOBALREF_H

#include <jni.h>
#include "JNILocalRef.h"

namespace JNI {

    /**
     * GlobalRefのラッパー
     */
    template<typename JNIType>
    class JNIGlobalRef : public JNIObject<JNIType> {
    public:
        JNIGlobalRef(JNIEnv *env, JNIType ref)
                : JNIObject<JNIType>(env, static_cast<JNIType>(env->NewGlobalRef(ref))) {}

        JNIGlobalRef(const JNIGlobalRef &obj)
                : JNIObject<JNIType>(obj._env, static_cast<JNIType>(obj._env->NewGlobalRef(obj._ref))) {}

        JNIGlobalRef(const JNILocalRef<JNIType> &obj)
                : JNIObject<JNIType>(obj.getEnv(), static_cast<JNIType>(obj.getEnv()->NewGlobalRef(obj))) {}

        JNIGlobalRef(JNIGlobalRef &&obj) noexcept
                : JNIObject<JNIType>(obj._env, obj._ref) {
            obj._env = nullptr;
            obj._ref = nullptr;
        }

        ~JNIGlobalRef() {
            if (this->_env != nullptr && this->_ref != nullptr) {
                this->_env->DeleteGlobalRef(this->_ref);
                this->_env = nullptr;
                this->_ref = nullptr;
            }
        }

        JNIGlobalRef &operator=(const JNIGlobalRef &obj) {
            if (this != &obj) {
                this->_env->DeleteGlobalRef(this->_ref);

                this->_env = obj._env;
                this->_ref = static_cast<JNIType>(obj._env->NewGlobalRef(obj._ref));
            }
            return *this;
        }

        JNIGlobalRef &operator=(JNIGlobalRef &&obj) {
            if (this != &obj) {
                this->_env->DeleteGlobalRef(this->_ref);

                this->_env = obj._env;
                this->_ref = obj._ref;
                obj._env = nullptr;
                obj._ref = nullptr;
            }
            return *this;
        }
    };

}

#endif // YUKARI_EXVOICE_JNIGLOBALREF_H