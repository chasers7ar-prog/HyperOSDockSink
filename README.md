# HyperOSDockSink

HyperOS 4 Dock 下沉 & 布局调整模块  
基于 **Zygisk Next Hyos Runtime**（针对 `hyos_spawner` / `com.miui.home`）

## 状态

| 项目 | 状态 |
|------|------|
| Zygisk Next Hyos 注入 | ✅ 完成 |
| 配置文件 | ✅ 完成 |
| 符号搜索 / 打印辅助 | ✅ 完成 |
| 真正 Dock 高度 / padding hook | ⏳ 需要动态分析结果 |

## 要求

- **Zygisk Next ≥ 1.5.0**（必须支持 HyperOS Runtime）
- Root（Magisk / KernelSU / APatch）
- HyperOS 4.0（Android 17），小米 17 系列等
- 建议使用 LSPosed IT 构建或最新 Zygisk Next

## 安装

1. 安装并启用 Zygisk Next（关闭 Magisk 内置 Zygisk）
2. 刷入本模块 zip（Actions Artifact 或自己编译）
3. 重启设备
4. （可选）创建配置：
   ```bash
   su
   cp /data/adb/modules/HyperOSDockSink/config.prop.example /data/adb/modules/HyperOSDockSink/config.prop
   # 编辑 sink_dp、enable 等
   ```
5. 强制停止桌面或重启后查看 log：
   ```bash
   adb logcat -s DockSink
   ```

成功标志：
```
Hyos specialized: process=... package=com.miui.home ...
Hyos module registered successfully
=== Starting Dock Sink (sink_dp=24) ===
```

## 构建

### 本地

```bash
cd native
mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-35 \
      -DCMAKE_BUILD_TYPE=Release \
      ..
cmake --build . -j$(nproc)
# 将 libdock_sink.so 复制到模块 lib/arm64-v8a/
```

### GitHub Actions

推送到 `main` 自动编译并上传 `HyperOSDockSink-module` artifact。

## 开发进度与需要你提供的帮助

真正修改 Dock 高度 / bottom padding 需要以下信息（任选其一即可继续）：

1. **Frida 动态结果**（最推荐）  
   在设备上对 `com.miui.home` / hyos 进程跑 Frida，搜索：
   - `dockWindowHeight`
   - `dockWidthPx`
   - `workspaceCellPadding`
   - `layout_marginBottom`
   - `multiple_split_dock_offset`
   - 任何与 Dock 布局计算相关的函数

   把找到的函数地址 / 符号名 / 偏移量发给我。

2. **Ghidra / IDA 静态分析**  
   对 `libapp.so` 或 `libapp_launcher.so` 做反编译，定位 Dock 高度计算函数后把函数签名或偏移告诉我。

3. **已知偏移**  
   如果你或其他模块已经有对应版本（RELEASE-8.01.02.6236）的偏移，直接提供即可。

拿到具体地址/符号后，我会立即补全 `do_dock_sink()` 里的真实 `inlineHook` / 内存修改代码，并让 `sink_dp` 生效。

## 已知相关字符串（来自当前 APK）

```
dockWindowHeight=
dockWidthPx:
workspaceCellPaddingSide
layout_marginBottom
multiple_split_dock_offset
FloatingDockAnimationTracker
DockRelayManager
update_dock_hierarchy
```

## License

GPL-3.0
