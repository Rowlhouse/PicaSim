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
  adb logcat -d | grep -E "PicaSim|SDL|libc|DEBUG|AndroidRuntime" | tail -400 > "$LOG_FILE" || true
}

# Presses back a few times once PicaSim has settled, and reports whether it reacted.
# On the start menu back quits, but a first-run dialog may come first, so this only
# reports what happened (the logcat lines are in android-logs.txt) and never fails.
# "input keyevent" sends the key down and up together, like Android's back gesture.
check_back_button() {
  sleep 40
  for n in 1 2 3; do
    adb shell input keyevent KEYCODE_BACK
    sleep 5
    if ! adb shell pidof "$PACKAGE" > /dev/null; then
      echo "Back button: PicaSim exited after back press $n"
      return
    fi
    echo "Back button: PicaSim still running after back press $n"
  done
  echo "Back button: PicaSim's own log lines after the presses:"
  adb logcat -d -s PicaSim | tail -30
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
    check_back_button
    save_logs
    exit 0
  fi
  echo "Waiting for PicaSim... ($((i*5))s)"
done

echo "PicaSim did not start within 60 seconds"
adb logcat -d | grep -E "$PACKAGE|SDL|FATAL|AndroidRuntime" | tail -100
save_logs
exit 1
