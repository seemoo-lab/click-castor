#!/bin/sh

CLICK_DIR=`pwd`
SODIUM_SRC_DIR=${CLICK_DIR}/libsodium
INSTALL_DIR=${SODIUM_SRC_DIR}/libsodium-native

git submodule update --init
if [ ! -d ${INSTALL_DIR} ]; then
	echo "Installing libsodium in ${INSTALL_DIR}"
	cd ${SODIUM_SRC_DIR}
	if [ ! -e configure ]; then
		./autogen.sh
	fi
	./configure --prefix=${INSTALL_DIR} --enable-minimal --enable-shared=no
	make && make check
	make install
fi

echo "Building Click with Castor"
cd ${CLICK_DIR}
make distclean
export CPPFLAGS="-I${INSTALL_DIR}/include"
export CXXFLAGS="-std=c++11"
export LDFLAGS="-L${INSTALL_DIR}/lib"
./configure --enable-castor --enable-nsclick --disable-userlevel --disable-linuxmodule \
            --disable-app --disable-aqm --disable-analysis --disable-test \
            --disable-tcpudp --disable-icmp --disable-threads --disable-tools
make
