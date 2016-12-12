#!/bin/sh

CLICK_DIR=`pwd`
SODIUM_SRC_DIR=${CLICK_DIR}/libsodium
INSTALL_DIR=${SODIUM_SRC_DIR}/libsodium-native

if [ ! -d ${INSTALL_DIR} ]; then
	echo "Installing libsodium in ${INSTALL_DIR}"
	git submodule update --init
	cd ${SODIUM_SRC_DIR}
	./configure --prefix=${INSTALL_DIR} --enable-minimal
	make && make check
	make install
fi

echo "Building Click with Castor"
cd ${CLICK_DIR}

export CPPFLAGS="-I${INSTALL_DIR}/include"
export CXXFLAGS="-static"
export LDFLAGS="-L${INSTALL_DIR}/lib"
./configure --enable-castor --enable-nsclick --disable-userlevel --disable-linuxmodule \
            --disable-dynamic-linking \
            --disable-app --disable-aqm --disable-analysis --disable-test \
            --disable-tcpudp --disable-icmp --disable-threads --disable-tools
make
