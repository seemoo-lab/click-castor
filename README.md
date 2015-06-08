# Castor + Xcastor
**Castor** and multicast extension **Xcastor** implementations for the [Click Modular Router](http://www.read.cs.ucla.edu/click/).

## Build Instructions

### Prerequisites
To compile the Castor `Element`s in Click, the **Botan** crypto library is required. On a Debian/Ubuntu system, install via
```bash
sudo apt-get install libbotan1.10-dev
```
The `Element`s currently requiring this library are
- `elements/local/castor/crypto.cc` and
- `elements/local/protswitch/samanagement.cc`

They do so with a `ELEMENT_LIBS(-L/usr/local/lib -lbotan-1.10)` statement.

### Click with ns-3
```bash
git clone <PROJECT>
cd click-castor
./configure --enable-local --enable-nsclick --disable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads
make
```
Note, that `--enable-local` (include Castor modules in `elements/local/` directory) and `--enable-nsclick` (ns-3 support) are mandatory. User level and linux modules are not required.

If new `Element`s are created, i.e., new `EXPORT_ELEMENT` directives are added in any source file in the `elements/` directory, run
```bash
make elemlist
make
```
to include the new `Element`s in the build process.