//
// Created by shibafu on 2016/04/03.
//

#ifndef YUKARI_EXVOICE_COMMON_H
#define YUKARI_EXVOICE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <jni.h>
#include <android/asset_manager.h>

extern JavaVM *g_jvm;

JNIEnv* getJNIEnv();
jobject getContext(mrb_state *mrb);
AAssetManager* getAAssetManager(mrb_state *mrb);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif //YUKARI_EXVOICE_COMMON_H
