# Castor/Xcastor in Click
This repository contains **Castor** and its multicast extension **Xcastor** implementations for the [Click Modular Router](http://www.read.cs.ucla.edu/click/).

## Table of Contents
* [Code Naviation](#code-navigation)
* [Install](#install)
  * [Prerequisites](#prerequisites)
  * [General Build Instructions](#general-build-instructions)
  * [Cross Compilation](#cross-compilation)
  * [Extending the Code](#extending-the-code)
* [Run (userlevel)](#run-userlevel)
* [Access Element Handlers](#access-element-handlers)
* [Related Publications](#related-publications)

## Code Navigation
This section gives a rough overview where relevant code for Castor/Xcastor is located.
* `conf/castor/`: Click script files.
  * `castor_settings.click`: common settings, e.g., defining whether to run as userlevel or ns-3 module; Castor-specific parameters; ...
  * `castor_multicast_via_unicast_routing.click`: **Castor** run configuration.
  * `castor_xcast_routing.click`: **Xcastor** run configuration.
  * other `.click`: shared modules which are included by the main files (above).
* `elements/local/`: C++ source code.
  * `castor/`: main code base, code only relevant for Xcastor is prefixed with `castor_xcast*`.
  * `neighbordiscovery/`: reusable simple neighbor discovery protocol.
  * `flooding/`: "stupid" flooding protocol (was used to compare performance with Xcastor).

## Install
### Prerequisites
* **libsodium**. To compile Castor, the [libsodium](https://download.libsodium.org/doc/) crypto library is required.
The only `Element` requiring this library is `elements/local/castor/crypto/crypto.cc`, which performs all relevant crypto operations in Castor.
If libsodium is installed in a non-standard path, you need to include an appropriate linker flag such as `LDFLAGS="-L<lib_dir>"`.
Up-to-date installation instructions for libsodium can be found [here](https://download.libsodium.org/doc/installation/index.html).
* **C++11**. The Castor and Xcastor implementations use some C\++11, e.g., `auto`, and, thus, require a compatible compiler. You might have to enforce `CXXFLAGS="-std=c++11"`.

### General Build Instructions
Click can be built as a regular userlevel or ns-3 module.
After cloning this repository, configure Click with `--enable-local` (include Castor modules in `elements/local/` directory) and the appropriate target. More detailed build instructions for Click can be found in the official INSTALL file.
* userlevel: `--enable-userlevel --disable-linuxmodule`
* ns-3: `--enable-nsclick --disable-userlevelm --disable-linuxmodule`
* can be omitted for Castor/Xcastor (to speed up compliation process): `--disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads --disable-tools`

```bash
git clone <PROJECT>
cd click-castor
# Configure (userlevel)
./configure --enable-local --enable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads --disable-tools
# Configure (ns-3)
./configure --enable-local --enable-nsclick --disable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads --disable-tools
# Build
make
```

### Cross Compilation
To build Click for a different architecture (such as i386 in our mesh nodes) on a x64 machine, you can cross compile using the `--host=i386-linux-gnu` (make sure to include the proper header files, e.g., `chroot` in the target file system):
```bash
./configure --host=i386-linux-gnu --enable-local --enable-userlevel --disable-linuxmodule --disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads --disable-tools
```

### Extending the Code
If new `Element`s are created, i.e., new `EXPORT_ELEMENT` directives are added in any source file in the `elements/` directory, run
```bash
make elemlist all
```

## Run (userlevel)
* Make sure to include the proper `castor_io_userlevel.click` in `castor_settings.click`.
* Castor uses a single network interface for communication with other nodes. By default, this interface is `wlan0`. This can be changed either in the `conf/castor/castor_io_userlevel.click` configuration file (look for variable `EthDev`) or by specifying `EthDev=wlanX` when starting Click.

```bash
cd <CLICK_DIR>
# Run Castor ...
userlevel/click conf/castor/castor_multicast_via_unicast_routing.click
# ... or with non-standard network interface:
userlevel/click EthDev=wlanX conf/castor/castor_multicast_via_unicast_routing.click
# Run Xcastor
userlevel/click conf/castor/castor_xcast_routing.click
```

## Access Element Handlers
Element Handlers can be accessed using a [ControlSocket Element](http://read.cs.ucla.edu/click/elements/controlsocket).
For example, one could generate a Unix socket to communicate with the Element Handlers like this: ControlSocket(unix, /tmp/click_socket);
After connecting with the socket, one can read data from the Element Handlers using a line-based protocol described [here](http://read.cs.ucla.edu/click/elements/controlsocket).
For example, one could read the list of neighbouring nodes by sending the command `READ neighbors.print` to the socket, which would lead to an answer of `200 OK\r\nDATA N\r\nx_1x_2x_n` where N denotes the length of the returned data and x_1 to x_n are the data symbols.

## Related Publications
* E. Kohler, R. Morris, B. Chen, J. Jannotti, and M. F. Kaashoek, “The Click Modular Router,” *ACM Transactions on Computer Systems*, vol. 18, no. 3, pp. 263–297, Aug. 2000.
* W. Galuba, P. Papadimitratos, M. Poturalski, K. Aberer, Z. Despotovic, and W. Kellerer, “Castor: Scalable Secure Routing for Ad Hoc Networks,” in *Proceedings of the IEEE Conference on Computer Communications (INFOCOM)*, 2010, pp. 1–9.
