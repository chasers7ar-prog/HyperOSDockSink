#!/system/bin/sh
# KernelSU / Magisk module install script
SKIPUNZIP=0

ui_print "- HyperOSDockSink"
ui_print "- Zygisk Next Hyos Runtime module"

# Ensure lib permission
if [ -f "$MODPATH/lib/arm64-v8a/libdock_sink.so" ]; then
  set_perm "$MODPATH/lib/arm64-v8a/libdock_sink.so" 0 0 0755
  ui_print "- libdock_sink.so OK"
else
  ui_print "! libdock_sink.so missing"
fi

if [ -f "$MODPATH/zn_modules.txt" ]; then
  ui_print "- zn_modules.txt OK"
else
  ui_print "! zn_modules.txt missing"
fi

ui_print "- Done. Reboot required."
