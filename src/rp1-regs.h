#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

// pci bar info
// from: https://github.com/G33KatWork/RP1-Reverse-Engineering/blob/master/pcie/hacks.py
#define RP1_BAR1 0x1f00000000
#define RP1_BAR1_LEN 0x400000

// offsets from include/dt-bindings/mfd/rp1.h
// https://github.com/raspberrypi/linux/blob/rpi-6.1.y/include/dt-bindings/mfd/rp1.h
#define RP1_IO_BANK0_BASE 0x0d0000
#define RP1_RIO0_BASE 0x0e0000
#define RP1_PADS_BANK0_BASE 0x0f0000

// the following info is from the RP1 datasheet (draft & incomplete as of 2024-02-18)
// https://datasheets.raspberrypi.com/rp1/rp1-peripherals.pdf
#define RP1_ATOM_XOR_OFFSET 0x1000
#define RP1_ATOM_SET_OFFSET 0x2000
#define RP1_ATOM_CLR_OFFSET 0x3000

#define PADS_BANK0_VOLTAGE_SELECT_OFFSET 0
#define PADS_BANK0_GPIO_OFFSET 0x4

#define RIO_OUT_OFFSET 0x00
#define RIO_OE_OFFSET 0x04
#define RIO_NOSYNC_IN_OFFSET 0x08
#define RIO_SYNC_IN_OFFSET 0x0C
//                           3         2         1
//                          10987654321098765432109876543210
#define CTRL_MASK_FUNCSEL 0b00000000000000000000000000011111
#define PADS_MASK_OUTPUT  0b00000000000000000000000011000000

#define CTRL_FUNCSEL_RIO 0x05



////////////////////////////////////////////////////
// SPI
#define RP1_SPI8_BASE 0x04c000  // not available on gpio
#define RP1_SPI0_BASE 0x050000
#define RP1_SPI1_BASE 0x054000
#define RP1_SPI2_BASE 0x058000
#define RP1_SPI3_BASE 0x05c000
#define RP1_SPI4_BASE 0x060000
#define RP1_SPI5_BASE 0x064000
#define RP1_SPI6_BASE 0x068000  // not available on gpio
#define RP1_SPI7_BASE 0x06c000  // not available on gpio

typedef struct
{
    uint8_t number;
    volatile uint32_t *status;
    volatile uint32_t *ctrl;
    volatile uint32_t *pad;
} gpio_pin_t;

typedef struct {

    volatile void *regbase;
    char *txdata;
    char *rxdata;
    uint8_t txcount;

} rp1_spi_instance_t;

typedef struct
{
    volatile void *rp1_peripherial_base;
    volatile void *gpio_base;
    volatile void *pads_base;
    volatile uint32_t *rio_out;
    volatile uint32_t *rio_output_enable;
    volatile uint32_t *rio_nosync_in;

    gpio_pin_t *pins[27];
    rp1_spi_instance_t *spis[6];

} rp1_t;