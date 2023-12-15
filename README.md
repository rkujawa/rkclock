# rkclock
Simple digital clock based on STM32L011K4 MCU. It can be easily implemented on a breadboard
using ST Nucleo L011K4 board.

# Building

You need a working ARM EABI C cross-compiler to build the source. Also, to flash and debug
it is necessary to install OpenOCD. On Fedora all the dependencies can be quickly installed
from the package manager:

```
dnf install arm-none-eabi-binutils-cs arm-none-eabi-gcc-cs arm-none-eabi-newlib openocd make gdb
```

Once dependencies are installed, just run:

```
make
```

# Flashing

The repository contains OpenOCD configuration appropriate for Nucleo L011K4, which should
also work with any ST-Link compatible interface. See `st_nucleo_l011k4.cfg`. 

To flash the MCU, just run:
```
make flash
```

# Debugging

Debugging is possible using GNU `gdb`. Appropriate `.gdbinit` file is provided. 
From the repository main directory, just run:
```
gdb
```

Note that `gdb` won't load the `.gdbinit` file automatically, unless you add it to
`~/.config/gdb/gdbinit`:
```
add-auto-load-safe-path ${MY_REPO_PATH}/rkclock/.gdbinit
```

# Schematics

![schematics](https://raw.githubusercontent.com/rkujawa/rkclock/master/rkclock-sch.png)

![breadboard image](https://raw.githubusercontent.com/rkujawa/rkclock/master/breadboard.jpg)

