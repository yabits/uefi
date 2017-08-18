# yabits/uefi
A minoca based UEFI coreboot payload 

[yabits/uefi](https://github.com/yabits/uefi) is an UEFI coreboot payload.
The code is based on the part of [Minoca OS](https://github.com/minoca/os).
Minoca OS has minimal UEFI implementation for some platforms,
like BeagleBone Black, Paspberry Pi and Legacy BIOS.
This project is trying to port the code base to coreboot as a payload.
Our goal is running Linux, \*BSDs and other bootloaders from this firmware.

## Building


Before building, clone
[coreboot](http://review.coreboot.org/p/coreboot)
and build
[Libpayload](https://www.coreboot.org/Libpayload).

```
$ git clone https://github.com/yabits/uefi.git
$ cd uefi
$ make menuconfig
$ make
```

## License

Most of the code comes from Minoca OS, licensed under
the terms of GNU General Public License, version 3.
Some code is from [FILO](http://review.coreboot.org/p/filo.git),
licensed under the terms of GNU Public License, version 2.
See the header of source code files for more details.
