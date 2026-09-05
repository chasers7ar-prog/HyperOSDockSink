# Analysis Results (from provided system launcher APK)

**APK version**: `RELEASE-8.01.02.6236-260818-09031543-R`  
**Package**: `com.miui.home`

## Key libraries

| Library | Size | Role |
|---------|------|------|
| `libapp.so` | ~28MB | Flutter/Dart AOT (main UI logic, GetX controllers) |
| `libapp_launcher.so` | ~15MB | Rust side (Dock window hierarchy, bounds, multi-split) |
| `libhyper_os_flutter.so` | ~17MB | Flutter engine / HyperOS Flutter glue |

## High-value strings found (layout related)

### From `libapp.so` (Flutter side)
```
dockWindowHeight=
dockHeightPx / dockHeightPx:
dockCellHeight= / dockCellHeight:
dockExitHeight=
hotSeatsHeight= / hotseatHeight=
hotSeatTranslateY=
hotseatsTranslationY:
squareHotseatCellHeight:
workspaceCellPaddingTop=
workspaceCellPaddingSide:
workspaceCellPaddingBottom
workspacePaddingTop=
workspaceTopPadding:
mVariableWorkspaceCellPaddingTop:
stableWorkspaceCellPaddingTop:
bottomPadding:
opticalInsetBottom:
folderGridPaddingBottom:
layout_marginBottom
```

### From `libapp_launcher.so` (Rust side)
```
Setting dock bounds: x=
dock bounds: x=
multiple_split_dock_offset
multiple_split_dock_bounds
multiple_split_dock_corner_radius
navigation_bar_height
real_navigation_bar_height
status_bar_height
update_dock_hierarchy
```

### Controllers / classes (Dart side names preserved in strings)
```
Dock_HotseatLayerGetxController
Dock_HotseatLayerGetxControllerPadNew
DockRelayManager
FloatingDockAnimationTracker
LaptopDesktop.dockHideRequest
```

## Implications for hooking

1. **Most layout numbers live in Flutter/Dart AOT** (`libapp.so`).  
   The strings above are mostly log format strings. The actual float/int values are computed inside Dart methods of `Dock_HotseatLayerGetxController` etc.

2. **Rust side** (`libapp_launcher.so`) manages native Dock window bounds and hierarchy (`Setting dock bounds`, `update_dock_hierarchy`).  
   Hooking here may allow changing the native window position/size after Flutter decides the values.

3. **Best attack points** (need Frida confirmation):
   - Dart methods that write `dockWindowHeight` / `dockHeightPx` / `hotSeatsHeight`
   - Native function that calls `Setting dock bounds`
   - Any bridge that passes bottom inset / navigation_bar_height into Flutter

4. Exported symbols related to Insets are mostly **imports** from Android framework (`Insets_bottom`, `WindowInsets_get_insets`…). Directly useful for understanding system insets but not the Dock height itself.

## Recommended next step for user

On device, use Frida against the `com.miui.home` process (or the hyos_spawner child) and:

```js
// Example ideas
Interceptor.attach(Module.findExportByName(null, "..."), ...)

// Or search memory / Dart snapshot for the string "dockWindowHeight="
// then find the function that uses it and log the height value.
```

Once a concrete function address or offset is obtained, it can be turned into a stable `inlineHook` / memory patch in `do_dock_sink()`.
