#include "zygisk_next_api.h"
#include <android/log.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG_TAG "DockSink"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static const ZygiskNextAPI *g_api = nullptr;

struct Config {
    int enable = 1;
    int sink_dp = 24;
    int extra_bottom_pad = 0;
};

static Config g_cfg;

static void load_config() {
    const char *paths[] = {
        "/data/adb/modules/HyperOSDockSink/config.prop",
        "/data/local/tmp/docksink.conf",
        nullptr
    };

    for (int i = 0; paths[i]; ++i) {
        FILE *f = fopen(paths[i], "r");
        if (!f) continue;

        char line[256];
        while (fgets(line, sizeof(line), f)) {
            if (line[0] == '#' || line[0] == '\n') continue;
            char key[64], val[64];
            if (sscanf(line, "%63[^=]=%63s", key, val) == 2) {
                if (strcmp(key, "enable") == 0) g_cfg.enable = atoi(val);
                else if (strcmp(key, "sink_dp") == 0) g_cfg.sink_dp = atoi(val);
                else if (strcmp(key, "extra_bottom_pad") == 0) g_cfg.extra_bottom_pad = atoi(val);
            }
        }
        fclose(f);
        LOGI("Loaded config from %s: enable=%d sink_dp=%d extra_bottom_pad=%d",
             paths[i], g_cfg.enable, g_cfg.sink_dp, g_cfg.extra_bottom_pad);
        return;
    }
    LOGI("No config found, using defaults: sink_dp=%d", g_cfg.sink_dp);
}

static void *find_symbol(const char *lib, const char *name, bool prefix = false) {
    if (!g_api) return nullptr;
    auto *res = g_api->newSymbolResolver(lib, nullptr);
    if (!res) return nullptr;
    size_t size = 0;
    void *addr = g_api->symbolLookup(res, name, prefix, &size);
    if (addr) LOGI("Found %s in %s -> %p (size=%zu)", name, lib, addr, size);
    g_api->freeSymbolResolver(res);
    return addr;
}

static bool dump_cb(const char *name, void *addr, size_t size, void *data) {
    const char *key = (const char *)data;
    if (name && strstr(name, key)) {
        LOGI("  [sym] %s @ %p size=%zu", name, addr, size);
    }
    return true;
}

static void dump_symbols_containing(const char *lib, const char *keyword) {
    if (!g_api) return;
    auto *res = g_api->newSymbolResolver(lib, nullptr);
    if (!res) return;
    LOGI("Dumping symbols in %s containing \"%s\":", lib, keyword);
    g_api->forEachSymbols(res, dump_cb, (void *)keyword);
    g_api->freeSymbolResolver(res);
}

static void do_dock_sink() {
    if (!g_cfg.enable) {
        LOGI("Disabled by config");
        return;
    }

    LOGI("=== Starting Dock Sink (sink_dp=%d) ===", g_cfg.sink_dp);

    // Key libraries identified from APK analysis
    const char *libs[] = {
        "libapp.so",
        "libapp_launcher.so",
        "libhyper_os_flutter.so",
        nullptr
    };

    // High-value keywords from static analysis of the provided APK
    const char *keywords[] = {
        "dock", "Dock", "Hotseat", "hotseat",
        "padding", "Padding", "PaddingTop", "PaddingBottom",
        "height", "Height", "Inset", "inset",
        "bounds", "Bounds",
        nullptr
    };

    for (int i = 0; libs[i]; ++i) {
        for (int k = 0; keywords[k]; ++k) {
            dump_symbols_containing(libs[i], keywords[k]);
        }
    }

    // TODO: After Frida / Ghidra provides concrete function addresses or
    // offsets for the current build (RELEASE-8.01.02.6236), add real hooks:
    //
    // Example (placeholder):
    // void *addr = find_symbol("libapp_launcher.so", "some_real_function");
    // if (addr) {
    //     static void* (*orig)(...) = nullptr;
    //     g_api->inlineHook(addr, (void*)my_hook_func, (void**)&orig);
    // }

    LOGI("Analysis-based symbol dump finished. Waiting for concrete offsets from device Frida/Ghidra.");
}

static void on_app_specialized(const ZnHyosAppSpecializeArgs *args) {
    if (args == nullptr) return;
    if (strcmp(args->package_name, "com.miui.home") != 0) return;

    LOGI("Hyos specialized: process=%s package=%s seinfo=%s",
         args->process_name, args->package_name, args->se_info);

    load_config();
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
