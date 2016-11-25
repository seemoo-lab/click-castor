# Castor (v2) in Click
This repository contains **Castor (v2)** implementation for the [Click Modular Router](http://www.read.cs.ucla.edu/click/).

## Table of Contents
* [Code Naviation](#code-navigation)
* [Install](#install)
  * [Prerequisites](#prerequisites)
  * [Build Instructions](#build-instructions)
  * [Cross Compilation](#cross-compilation)
    * [General](#general)
    * [Android](#android)
  * [Extending the Code](#extending-the-code)
* [Run (userlevel)](#run-userlevel)
* [Communicating with Click at Runtime](#communicating-with-click-at-runtime)
* [Related Publications](#related-publications)

## Code Navigation
This section gives a rough overview where relevant code for Castor (v2) is located.
* `conf/castor/`: Click script files.
  * `castor_settings.click`: common settings, e.g., defining whether to run as userlevel or ns-3 module; Castor-specific parameters; ...
  * `castor_unicast_routing.click`: **Castor (v2)** run configuration.
  * other `.click`: shared modules which are included by the main files (above).
* `elements/castor/`: C++ source code.
  * `castor.hh`: packet definitions
  * `attack/`: elements specific to implement certain attacks
  * `crypto/`: all required crypto (includes a wrapper class around `libsodium`)
  * `flooding/`: "stupid" flooding protocol (was used to compare performance with Xcastor).
  * `flow/`: flow generation and reconstruction
  * `neighbors/`: neighbor discovery and neighbor-to-neighbor authentication
  * `ratelimiter/`: prevents neighbors from flooding the network
  * `routing/`: main code base: routing logic
  * `util/`: utility Elements

## Install
### Prerequisites
* **libsodium**. To compile Castor, the [libsodium](https://download.libsodium.org/doc/) crypto library is required.
The only `Element` requiring this library is `elements/castor/crypto/crypto.cc`, which performs all relevant crypto operations in Castor.
If libsodium is installed in a non-standard path, you need to include an appropriate linker flag such as `LDFLAGS="-L<lib_dir>"`.
Up-to-date installation instructions for libsodium can be found [here](https://download.libsodium.org/doc/installation/index.html).

### General Build Instructions
Click can be built as a regular userlevel or ns-3 module.
After cloning this repository, configure Click with `--enable-castor` and the appropriate target. More detailed build instructions for Click can be found in the official INSTALL file.
* userlevel: `--enable-userlevel --disable-linuxmodule`
* ns-3: `--enable-nsclick --disable-userlevel --disable-linuxmodule`
* can be omitted for Castor (to speed up compilation process): `--disable-app --disable-aqm --disable-analysis --disable-test --disable-tcpudp --disable-icmp --disable-threads --disable-tools`

To summarize, `click-castor` can be build with the following commands:
```bash
git clone <PROJECT>
cd click-castor
# Configure (userlevel)
./configure --enable-castor --enable-userlevel --disable-linuxmodule \
            --disable-app --disable-aqm --disable-analysis --disable-test \
            --disable-tcpudp --disable-icmp --disable-threads --disable-tools \
            LDFLAGS="-L/path/to/libsodium"
# Configure (ns-3)
./configure --enable-castor --enable-nsclick --disable-userlevel --disable-linuxmodule \
            --disable-app --disable-aqm --disable-analysis --disable-test \
            --disable-tcpudp --disable-icmp --disable-threads --disable-tools \
            LDFLAGS="-L/path/to/libsodium"
# Build
make
```

### Cross Compilation

#### General
To build Click for a different architecture (such as i386 in our mesh nodes) on a x64 machine, you can cross compile using the `--host=i386-linux-gnu` (make sure to include the proper header files, e.g., `chroot` in the target file system):
```bash
./configure --host=i386-linux-gnu \
            --enable-castor --enable-userlevel --disable-linuxmodule \
            --disable-app --disable-aqm --disable-analysis --disable-test \
            --disable-tcpudp --disable-icmp --disable-threads --disable-tools \
            LDFLAGS="-L/path/to/libsodium"
```

#### Android
**Note:** Android instructions are currently outdated.

To build Click for Android, you must specify the `--enable-android` parameter when running `./configure` and build it with the target `android` afterwards.--- Additionally you need to set the `NDK_ROOT` environment variable, it should contain the path to your Android NDK.
```bash
export NDK_ROOT="/path/to/android/ndk/"
./configure --enable-castor --enable-android --disable-linuxmodule \
            --disable-app --disable-aqm --disable-analysis --disable-test \
            --disable-tcpudp --disable-icmp --disable-threads --disable-tools
make android
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
userlevel/click conf/castor/castor_unicast_routing.click
# ... or with non-standard network interface:
userlevel/click EthDev=wlanX conf/castor/castor_unicast_routing.click
```

## Communicating with Click at Runtime
Element Handlers can be accessed using a [ControlSocket Element](http://read.cs.ucla.edu/click/elements/controlsocket).
To communicate via a Unix socket, one can either
- include `ControlSocket(unix, /tmp/click_socket);` in the `.click` config file, or
- start Click with parameter `-u /tmp/click_socket`.

After connecting with the socket, one can read data from the Element Handlers using a line-based protocol described [here](http://read.cs.ucla.edu/click/elements/controlsocket).
For example, one could read the list of neighbouring nodes by sending the command `READ neighbors.print` to the socket, which would lead to an answer of `200 OK\r\nDATA N\r\nx_1x_2x_n` where `N` denotes the length of the returned data and `x_1` to `x_n` are the data symbols.


## Related Publications
* E. Kohler, R. Morris, B. Chen, J. Jannotti, and M. F. Kaashoek, “**The Click Modular Router**,” *ACM Transactions on Computer Systems*, vol. 18, no. 3, pp. 263–297, Aug. 2000. ([PDF](https://pdos.csail.mit.edu/papers/click:tocs00/paper.pdf), [web](http://read.cs.ucla.edu/click/click))
* W. Galuba, P. Papadimitratos, M. Poturalski, K. Aberer, Z. Despotovic, and W. Kellerer, “**Castor: Scalable Secure Routing for Ad Hoc Networks**,” in *Proceedings of the IEEE Conference on Computer Communications (INFOCOM)*, 2010, pp. 1–9. ([PDF](https://infoscience.epfl.ch/record/148217/files/castor.pdf))
* M. Schmittner and M. Hollick, “**Xcastor: Secure and Scalable Group Communication in Ad Hoc Networks**,” in *IEEE Symposium on a World of Wireless, Mobile and Multimedia Networks (WoWMoM)*, June 2016. ([PDF](https://www.informatik.tu-darmstadt.de/fileadmin/user_upload/Group_SEEMOO/milan_schmittner/xcastor-wowmom16.pdf))
