#include <mruby.h>
#include <mruby/string.h>
#include <mruby/compile.h>
#include <android/asset_manager.h>
#include <stdlib.h>
#include "mrb_Android_Log.h"
#include "mrb_Android_AssetDir.h"
#include "common.h"

static mrb_value require_assets(mrb_state *mrb, mrb_value self) {
    mrb_value filename;
    mrb_get_args(mrb, "o", &filename);

    char* cFilename = mrb_str_to_cstr(mrb, filename);
    mrbc_context *mrbContext = mrbc_context_new(mrb);
    mrbc_filename(mrb, mrbContext, cFilename);

    AAssetManager *manager = getAAssetManager(mrb);
    AAsset *asset = AAssetManager_open(manager, cFilename, AASSET_MODE_UNKNOWN);
    if (asset == NULL) {
        return mrb_nil_value();
    }

    size_t length = (size_t) AAsset_getLength(asset);
    char *buffer = calloc(length, sizeof(char));
    if (0 < AAsset_read(asset, buffer, length)) {
        mrb_load_nstring_cxt(mrb, buffer, (int) length, mrbContext);
    }
    free(buffer);

    AAsset_close(asset);
    return filename;
}

void exvoice_init_android(mrb_state *mrb) {
    struct RClass *android = mrb_define_module(mrb, "Android");

    exvoice_init_android_log(mrb, android);
    exvoice_init_android_assetdir(mrb, android);
    mrb_define_module_function(mrb, android, "require_assets", require_assets, MRB_ARGS_REQ(1));
}

