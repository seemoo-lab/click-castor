#!/bin/sh

ARCH=armv8-a

if [ "x$ARCH" = 'x' ] ; then
    echo "You shouldn't use build-castor-android.sh directly, use build-castor-android-[arch].sh instead"
    exit 1
fi

CLICK_DIR=`pwd`
SODIUM_SRC_DIR=${CLICK_DIR}/libsodium
INSTALL_DIR=${SODIUM_SRC_DIR}/libsodium-android-${ARCH}

if [ ! -d ${INSTALL_DIR} ]; then
	echo "Installing libsodium in ${INSTALL_DIR}"
	git submodule update --init
	cd ${SODIUM_SRC_DIR}
	export ANDROID_NDK_HOME=${NDK_ROOT}
	dist-build/android-${ARCH}.sh
fi

echo "Building Click with Castor"
cd ${CLICK_DIR}
export CPPFLAGS="-I${INSTALL_DIR}/include"
export CXXFLAGS="-static"
export LDFLAGS="-L${INSTALL_DIR}/lib"
export CLICK_OPTIONS="--enable-castor \
                      --disable-dynamic-linking \
                      --disable-app --disable-aqm --disable-analysis --disable-test \
                      --disable-tcpudp --disable-icmp --disable-threads"
android/android-${ARCH}.sh
