#include "zygisk_next_api.h"
#include <android/log.h>
#include <string.h>
#include <unistd.h>

#define LOG_TAG "DockSink"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const ZygiskNextAPI *g_api = nullptr;

// TODO: 在这里实现 Dock 下沉逻辑
// 使用 g_api->newSymbolResolver / pltHook / inlineHook
// 定位 libapp.so / libapp_launcher.so / libhyper_os_flutter.so 中的布局参数
static void do_dock_sink() {
    LOGI("Dock sink logic executed (placeholder)");
}

static void on_app_specialized(const ZnHyosAppSpecializeArgs *args) {
    if (args == nullptr) return;

    // 只处理系统桌面
    if (strcmp(args->package_name, "com.miui.home") != 0) {
        return;
    }

    LOGI("Hyos specialized: process=%s package=%s seinfo=%s",
         args->process_name, args->package_name, args->se_info);

    do_dock_sink();
}

static const ZygiskNextHyosModule hyos_module = {
    .target_api_version = ZYGISK_NEXT_HYOS_API_VERSION,
    .onAppSpecialized = on_app_specialized,
};

static void on_module_loaded(void *self_handle, const ZygiskNextAPI *api) {
    g_api = api;

    const ZygiskNextRuntime *runtime = api->getRuntime();
    if (runtime == nullptr ||
        runtime->type != ZN_RUNTIME_HYOS ||
        runtime->api_version < ZYGISK_NEXT_HYOS_API_VERSION) {
        LOGI("Not Hyos runtime, skip");
        return;
    }

    if (runtime->registerModule(&hyos_module) != ZN_SUCCESS) {
        LOGE("Failed to register Hyos module");
        return;
    }

    LOGI("Hyos module registered successfully");
}

extern "C" __attribute__((visibility("default")))
ZygiskNextModule zn_module = {
    .target_api_version = ZYGISK_NEXT_API_VERSION,
    .onModuleLoaded = on_module_loaded,
};
