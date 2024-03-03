# rpi5-rp1-spi
Using the SPI on the Raspberry Pi 5 through direct register control on the RP1

Tests on using the PCI->RP1 direct register access to use SPI and other peripherals.
See also [rpi5-rp1-gpio](https://github.com/praktronics/rpi5-rp1-gpio)

The Raspberry Pi 5 is super cool, and the RP1 even cooler. However, it's not really well documented, so there's a lot of poking around in the device-tree and source files to understand registers and approach.  Hopefull (March 2024) the docs will be completed at some point.

This example shows some simple code for how to use the PCI BAR to
1. Map the RP1 peripheral memory to user space
2. Create an SPI device
3. Set the pins for SPI usage
4. Write some data to, and read some data from a slave device using the registers, rather than spi-dev

In this instance, the slave is a Raspberry Pico (which also has some new SPI functions to enable it to work properly as a slave), which can be found in the 'pico' folder.

To build and run:
```
    /rpi5-rp1-spi $ mkdir build
    /rpi5-rp1-spi $ cd build
    /rpi5-rp1-spi/build $ cmake ..
    /rpi5-rp1-spi/build $ cmake --build .
```
    run with sudo or as root
`   /rpi5-rp1-spi/build/bin $ sudo ./rpi5-rp1-spi`

