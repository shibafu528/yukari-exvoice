#include <jni.h>
#include <mruby.h>
#include <mruby/variable.h>
#include "jni_MRuby.h"
#include "generator.h"

OBJECT_FIELD_READER(getField_ConfigLoader_mRuby, mRuby, "Linfo/shibafu528/yukari/exvoice/MRuby;");

JNIEXPORT void JNICALL Java_info_shibafu528_yukari_exvoice_ConfigLoader_initializeNative(JNIEnv *env, jobject self) {
    // Get mrb_state
    jobject mRubyObject = getField_ConfigLoader_mRuby(env, self);
    mrb_state *mrb = getField_MRuby_mrubyInstancePointer(env, mRubyObject);

    // Create global-ref
    jobject gSelf = (*env)->NewGlobalRef(env, self);

    // Store in ConfigLoader@@jobjectReference
    struct RClass *configloader = mrb_module_get(mrb, "ConfigLoader");
    mrb_mod_cv_set(mrb, configloader, mrb_intern_lit(mrb, "jobjectReference"), mrb_cptr_value(mrb, gSelf));

    // Release references
    (*env)->DeleteLocalRef(env, mRubyObject);
}