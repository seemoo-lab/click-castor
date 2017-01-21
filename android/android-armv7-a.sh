#!/bin/sh
export TARGET_ARCH=armv7-a
export CFLAGS="-Os -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -marm -march=${TARGET_ARCH}"
NDK_API_COMPAT=21 ARCH=arm HOST_COMPILER=arm-linux-androideabi CLICK_OPTIONS="${CLICK_OPTIONS}" "$(dirname "$0")/android-build.sh"
