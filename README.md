# Castor + Xcastor
**Castor** and multicast extension **Xcastor** implementations for the [Click Modular Router](http://www.read.cs.ucla.edu/click/).

## Build Instructions

### Click with ns-3
```bash
$ git clone <PROJECT>
$ cd click-castor
$ ./configure --enable-local --enable-nsclick --disable-userlevel --disable-linuxmodule
$ make
```
Note, that `--enable-local` (include Castor modules in `elements/local/` directory) and `--enable-nsclick` (ns-3 support) are mandatory. User level and linux modules are not required.

If new `Element`s are created, i.e., new `EXPORT_ELEMENT` directives are added in any source file in the `elements/` directory, run
```bash
$ make elemlist
$ make
```
to include the new `Element`s in the build process.