#!/usr/bin/env bash
# Android smoke test, run by android-emulator-runner once the emulator has booted.
# Installs the debug APK, launches PicaSim and checks that its process starts.
# Logs are saved to android-logs.txt here, while the emulator is still running,
# because the emulator is shut down as soon as this script exits.

set -u

PACKAGE=com.rowlhouse.picasim
ACTIVITY=.PicaSimActivity
APK=apk/app-debug.apk
LOG_FILE=android-logs.txt

save_logs() {
  adb logcat -d | grep -E "PicaSim|SDL|libc|DEBUG|AndroidRuntime" | tail -200 > "$LOG_FILE" || true
}

if ! adb install "$APK"; then
  echo "Failed to install $APK"
  save_logs
  exit 1
fi

adb shell am start -n "$PACKAGE/$ACTIVITY"

# Poll for up to 60 seconds for the process to appear
for i in $(seq 1 12); do
  sleep 5
  if adb shell ps -A | grep -q "$PACKAGE"; then
    echo "PicaSim is running OK"
    save_logs
    exit 0
  fi
  echo "Waiting for PicaSim... ($((i*5))s)"
done

echo "PicaSim did not start within 60 seconds"
adb logcat -d | grep -E "$PACKAGE|SDL|FATAL|AndroidRuntime" | tail -100
save_logs
exit 1
