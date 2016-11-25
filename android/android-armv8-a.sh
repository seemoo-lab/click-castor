#! /bin/sh
export TARGET_ARCH=armv8-a
export CFLAGS="-Os -march=${TARGET_ARCH}"
NDK_API_COMPAT=21 ARCH=arm64 HOST_COMPILER=aarch64-linux-android CLICK_OPTIONS="${CLICK_OPTIONS}" "$(dirname "$0")/android-build.sh"
