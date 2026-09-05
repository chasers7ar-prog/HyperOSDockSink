# HyperOSDockSink

HyperOS 4 Dock 下沉 & 布局调整模块，基于 **Zygisk Next Hyos Runtime**（针对 `hyos_spawner` / `com.miui.home`）。

## 要求

- Zygisk Next ≥ 1.5.0（必须支持 HyperOS Runtime）
- Root（Magisk / KernelSU / APatch）
- HyperOS 4.0（Android 17），小米 17 系列等

## 构建

### Native 部分

```bash
cd native
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-35 \
      -DCMAKE_BUILD_TYPE=Release \
      ..
cmake --build . -j$(nproc)
```

生成的 `libdock_sink.so` 需要放到模块的 `lib/arm64-v8a/` 目录下。

### GitHub Actions

推送到 `main` 分支会自动触发编译（需要配置 NDK Secrets 或使用仓库自带 workflow）。

## 安装

1. 刷入 Zygisk Next
2. 关闭 Magisk 内置 Zygisk（如果使用 Magisk）
3. 刷入本模块 zip
4. 重启
5. 强制停止 `com.miui.home` 或重启后查看 logcat：

```bash
adb logcat -s DockSink
```

看到 `Hyos specialized: ... package=com.miui.home` 表示注入成功。

## 开发说明

当前仅为骨架，`do_dock_sink()` 中需要自行实现符号定位 + hook（pltHook / inlineHook）来修改 Dock 高度 / bottom padding。

参考：
- [ZygiskNext Hyos Runtime 文档](https://github.com/Dr-TSNG/ZygiskNext/blob/main/docs/hyos_runtime.md)
- MiuiBackGestureHook

## License

GPL-3.0
