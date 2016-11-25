#! /bin/sh

if [ -z "$NDK_API" ]; then
    export NDK_API="24"
    export NDK_API_COMPAT="${NDK_API_COMPAT:-16}"
else
    export NDK_API_COMPAT="${NDK_API_COMPAT:-${NDK_API}}"
fi

if [ -z "$NDK_ROOT" ]; then
    echo "You should probably set NDK_ROOT to the directory containing"
    echo "the Android NDK"
    exit
fi

if [ ! -f ./configure ]; then
	echo "Can't find ./configure. Run from click project root directory."
	exit 1
fi

if [ "x$TARGET_ARCH" = 'x' ] || [ "x$ARCH" = 'x' ] || [ "x$HOST_COMPILER" = 'x' ]; then
    echo "You shouldn't use android-build.sh directly, use android-[arch].sh instead"
    exit 1
fi


export PREFIX="$(pwd)/android/build/click-android-${TARGET_ARCH}"
export TOOLCHAIN_DIR="$(pwd)/android/toolchain/${TARGET_ARCH}"
export PATH="${PATH}:${TOOLCHAIN_DIR}/bin"

export CPPFLAGS="${CPPFLAGS} -fPIC --sysroot=${TOOLCHAIN_DIR}/sysroot"
export CXXFLAGS="${CXXFLAGS} -std=c++11"
export LDFLAGS="${LDFLAGS} -pie"

echo
echo "Building for API [${NDK_API}], retaining compatibility with API [${NDK_API_COMPAT}]"
echo

MAKE_TOOLCHAIN="${NDK_ROOT}/build/tools/make_standalone_toolchain.py"

python $MAKE_TOOLCHAIN --api="$NDK_API_COMPAT" \
    --arch="$ARCH" --install-dir="$TOOLCHAIN_DIR" --force || exit 1

./configure \
    --enable-userlevel \
    --disable-linuxmodule \
    ${CLICK_OPTIONS} \
    --host="${HOST_COMPILER}" \
    --prefix="${PREFIX}"
    CPPFLAGS=--sysroot="${TOOLCHAIN_DIR}/sysroot" || exit 1

if [ "$NDK_API" != "$NDK_API_COMPAT" ]; then
  egrep '^#define ' config.log | sort -u > config-def-compat.log
  echo
  echo "Configuring again for API [${NDK_API}]"
  echo
  python $MAKE_TOOLCHAIN --api="$NDK_API" \
      --arch="$ARCH" --install-dir="$TOOLCHAIN_DIR" --force || exit 1

  ./configure \
    --enable-userlevel \
    --disable-linuxmodule \
    ${CLICK_OPTIONS} \
    --host="${HOST_COMPILER}" \
    --prefix="${PREFIX}" \
    CPPFLAGS=--sysroot="${TOOLCHAIN_DIR}/sysroot" || exit 1

  egrep '^#define ' config.log | sort -u > config-def.log
  if ! cmp config-def.log config-def-compat.log; then
    echo "API [${NDK_API}] is not backwards-compatible with [${NDK_API_COMPAT}]" >&2
    diff -u config-def.log config-def-compat.log >&2
    exit 1
  fi
  rm -f config-def.log config-def-compat.log
fi

make install && \
echo "click has been installed into ${PREFIX}"
