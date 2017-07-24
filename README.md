# Lulu P/XP colony simulator written in C
Lulu P/XP colony simulator library that allows one to import (as text file) a P colony or a P swarm (colony of P colonies) and simulate the transfer of simbolic objects.

The library can be included in any standalone C application and includes methods for modifying the contents of the P/XP colony, in order to interface a P/XP colony model with other applications.

A standalone P/XP colony simulation application is included in this package.

# Requirements
* Compilers:
  * GCC - for simulations on a computer
  * AVR-GCC - for building a library that can be included in an AVR microcontroller application
* [Kilolib](https://github.com/acornejo/kilolib) - ONLY for AVR
* [Lulu\_C](https://github.com/andrei91ro/lulu_c) - Python 3 conversion script used to translate Lulu text input files to C source code
* [Make](https://www.gnu.org/software/make/) - for an automated build process

The build process was tested on an up-to-date Linux distribution (ArchLinux). The build process should work on most unixes and on Windows using packages such as MinGW.

# Usage
After installing all components and adjunsting the corresponding paths (using Makefile parameters), simply call `make` in the project folder

If no errors are reported during compilation, there should be two `build` folders:

* build
  * lulu.a
  * simulator
* build\_hex
  * lulu.a
  * lulu\_debug.a

`simulator` can be executed as a PC application and will run the P colony specified in the `LULU_INSTANCE_FILE` Makefile parameter, by default the increment P colony.

The other build results are static libraries that can be linked to another application and allow it to control the execution process and the contents of the P/XP colony.
The `build_hex` folder contains static libraries that can be linked to an AVR micro-controller application, such as a Kilobot controller.

# Configuration

## Debug level

This parameter controls the verbosity of the `Lulu` library and the `simulator` application. The accepted values are

* 0 (DEBUG)
* 1 (INFO)
* 2 (RELEASE) - no messages are printed

For micro-controller applications, the buildsystem will create 2 versions of the library, with/without message printing in order to simplify the build process for the host application.


# Authors
Andrei George Florea, [Cătălin Buiu](http://catalin.buiu.net)

[Department of Automatic Control And Systems Engineering](http://acse.pub.ro),

Politehnica University of Bucharest

Bucharest, Romania.
