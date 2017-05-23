#ifndef YUKARI_EXVOICE_JNILOCALREF_H
#define YUKARI_EXVOICE_JNILOCALREF_H

#include <jni.h>
#include <type_traits>
#include "JNIObject.h"

namespace JNI {

    /**
     * プリミティブ型用のJNI参照管理処理 (プリミティブなので何もしない)
     */
    template <typename JNIType, typename = void>
    struct JNIRefUtil {
        static JNIType capture(JNIEnv *env, JNIType ref) {
            return ref;
        }
        static void release(JNIEnv *env, JNIType ref) {}
    };

    /**
     * オブジェクト型および派生用のJNI参照管理処理 (LocalRefの管理を行う)
     */
    template <typename JNIType>
    struct JNIRefUtil<JNIType, typename std::enable_if_t<std::is_base_of<jobject, JNIType>::value>::type> {
        static JNIType capture(JNIEnv *env, JNIType ref) {
            return env->NewLocalRef(ref);
        }
        static void release(JNIEnv *env, JNIType ref) {
            env->DeleteLocalRef(ref);
        }
    };

    /**
     * LocalRefのラッパー
     */
    template <typename JNIType>
    class JNILocalRef : public JNIObject<JNIType> {
    public:
        JNILocalRef(JNIEnv *env, JNIType ref)
                : JNIObject<JNIType>(env, JNIRefUtil<JNIType>::capture(env, ref)) {}

        JNILocalRef(const JNILocalRef &obj)
                : JNIObject<JNIType>(obj._env, static_cast<JNIType>(obj._env->NewLocalRef(obj._ref))) {}

        JNILocalRef(JNILocalRef &&obj) noexcept
                : JNIObject<JNIType>(obj._env, obj._ref) {
            obj._env = nullptr;
            obj._ref = nullptr;
        }

        ~JNILocalRef() {
            if (this->_env != nullptr && this->_ref != nullptr) {
                this->_env->DeleteLocalRef(this->_ref);
                this->_env = nullptr;
                this->_ref = nullptr;
            }
        }

        JNILocalRef &operator=(const JNILocalRef &obj) {
            if (this != &obj) {
                this->_env->DeleteLocalRef(this->_ref);

                this->_env = obj._env;
                this->_ref = static_cast<JNIType>(obj._env->NewLocalRef(obj._ref));
            }
            return *this;
        }

        JNILocalRef &operator=(JNILocalRef &&obj) {
            if (this != &obj) {
                this->_env->DeleteLocalRef(this->_ref);

                this->_env = obj._env;
                this->_ref = obj._ref;
                obj._env = nullptr;
                obj._ref = nullptr;
            }
            return *this;
        }
    };

}

#endif // YUKARI_EXVOICE_JNILOCALREF_H