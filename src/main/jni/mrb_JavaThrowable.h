#ifndef YUKARI_EXVOICE_MRB_JAVATHROWABLE_H
#define YUKARI_EXVOICE_MRB_JAVATHROWABLE_H

#include <mruby.h>
#include <jni.h>

void exvoice_init_javathrowable(mrb_state *mrb);

mrb_value exvoice_java_error_new(mrb_state *mrb, jthrowable exc);

#endif //YUKARI_EXVOICE_MRB_JAVATHROWABLE_H