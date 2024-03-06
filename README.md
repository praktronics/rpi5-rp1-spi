# rpi5-rp1-spi
Using the SPI on the Raspberry Pi 5 through direct register control on the RP1

Tests on using the PCI->RP1 direct register access to use SPI and other peripherals.
See also [rpi5-rp1-gpio](https://github.com/praktronics/rpi5-rp1-gpio)

The Raspberry Pi 5 is super cool, and the RP1 even cooler. However, it's not really well documented, so there's a lot of poking around in the device-tree and source files to understand registers and approach.  Hopefully (March 2024) the docs will be completed at some point.

This example shows some simple code for how to use the PCI BAR to
1. Map the RP1 peripheral memory to user space
2. Create an SPI device
3. Set the pins for SPI usage
4. Write some data to, and read some data from a slave device using the registers, rather than spi-dev

In this instance, the slave is a Raspberry Pico (which also has some new SPI functions to enable it to work properly as a slave), which can be found in the 'pico' folder If you don't want to build the pico code, you can just drop the UF2 supplied onto the pico. Connect a terminal to whatever serial port it shows up as to see the output.

To build and run:
```bash
    /rpi5-rp1-spi $ mkdir build
    /rpi5-rp1-spi $ cd build
    /rpi5-rp1-spi/build $ cmake ..
    /rpi5-rp1-spi/build $ cmake --build .
```
    run with sudo or as root
```bash
    /rpi5-rp1-spi/build/bin $ sudo ./rpi5-rp1-spi
```

There are some utility functions for debugging in the rp1-spi-util.c file. These provide a convenient way to dump all the registers, as well as the details of some of the more frequently set / checked registers.

If you're using the Pico code provided, then the connection between the Pi5 and pico looks like this.  These are direct wire connections, no pullups etc. required. Note these are **GPIO** numbers, physical pin numbers in brackets:
| Signal | Pi header GPIO | Pico GPIO |
| MOSI | GPIO10 (19) | GP16 (21)|
| MISO | GPIO9 (21) | GP19 (25)|
| CLK | GPIO11 (23) | GP18 (24)|
| _CS | GPIO8 (24) | GP17 (22) |
| GND | GND (25) | GND (23 & 28)|

With the limitations of noise and signal integrity on a breadboard setup, I've managed to get this up to ~ 24MHz, but typically run it at 20MHz.
