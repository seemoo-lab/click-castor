# Castor + Xcastor
**Castor** and multicast extension **Xcastor** implementations for the [Click Modular Router](http://www.read.cs.ucla.edu/click/).

## Build Instructions

### Prerequisites
To compile Castor, the [libsodium](https://download.libsodium.org/doc/) crypto library is required.
The only `Element` requiring this library is `elements/local/castor/crypto/crypto.cc`, which performs all relevant crypto operations in Castor.

If libsodium is installed in a non-standard path, you need to include an appropriate linker flag such as `LDFLAGS="-L<lib_dir>"` where `<lib_dir>` is where you installed libsodium.
Up-to-date installation instructions can be found [here](https://download.libsodium.org/doc/installation/index.html).

### Click with ns-3
```bash
git clone <PROJECT>
cd click-castor
./configure --enable-local --enable-nsclick --disable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads --disable-tools CXXFLAGS="-std=c++11"
make
```
Note, that `--enable-local` (include Castor modules in `elements/local/` directory) and `--enable-nsclick` (ns-3 support) are mandatory. User level and linux modules are not required.

If new `Element`s are created, i.e., new `EXPORT_ELEMENT` directives are added in any source file in the `elements/` directory, run
```bash
make elemlist
make
```
to include the new `Element`s in the build process.

### Click on Linux (userlevel)
Basically as above, but disable nsclick and enable user level elements:
```bash
git clone <PROJECT>
cd click-castor
./configure --enable-local --enable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads
make
```

#### Cross compilation for i386
To build Click for a different architecture (such as i386 in our mesh nodes) on your x64 machine, you can cross compile using the `--host=i386-linux-gnu`:
```bash
./configure --host=i386-linux-gnu --enable-local --enable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads
```

**Important**: Make sure to `chroot` in the target file system.

**Important**: Do not try to disable `int64` support (`--disable-int64`) as this will break some of the core Click libraries.
