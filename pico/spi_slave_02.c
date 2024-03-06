#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/watchdog.h"
#include "pico/binary_info.h"

#include "pi_pico_commands.h"
#include "spi_slave.h"

int main()
{
    stdio_init_all();

    spi_init(spi_default, 10000 * 1000);

    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_set_slave(spi_default, true);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, PICO_DEFAULT_SPI_CSN_PIN, GPIO_FUNC_SPI));

    // let's hold here for 2s to allow the serial monitor to open
    sleep_ms(2000);
 
    printf("SPI slave waiting for data\n");

    //unsigned char inbuf[3];
    //uint32_t qdata[8] = {0x11223344, 0x55667788, 0x99AABBCC, 0xEEFFFFEE, 0xABCDEF12, 0x3456789A, 0xBCDE0102, 0x03040506};
    uint8_t qdata[32];
    uint8_t ddata[32];
    for(int cnt = 0; cnt < 32; cnt++)
    {
        qdata[cnt] = cnt+1;
    }

    printf("qdata initialised as follows:\n");
    for(int i = 0; i < 32; i++)
    {
        printf("qdata[%d]: 0x%x ", i, qdata[i]);
    }
    

    unsigned char command = 0;
    //unsigned char command[4];
    int bytesin = 0;
    //int ndex = 0;

    unsigned char dummywritedata = 0xAA;
    //unsigned char dummyout = 0x99;

    watchdog_enable(3000, 1);

    while (true)
    {

        // check if there's something to read
        if (spi_is_readable(spi_default))
        {
            command = spi_slave_read_8_blocking(spi_default);
            switch (command)
            {
            case CMD_NOP:
                printf("NOP command received %d\n", bytesin);
                break;
            case CMD_RESET_ENCODERS:
                printf("Reset encoders command received\n");
                break;
            case CMD_READ_SYSTIME:
                uint32_t systime = time_us_32();
                spi_slave_write_8_n_blocking(spi_default, (uint8_t *)&systime, 4);
                printf("Read systime command received - sent 0x%8X\n", systime);
                break;
            case CMD_RESET_PICO:
                printf("Reset Pico command received. Waking up watchdog and waiting for it to bite.\n");
                watchdog_enable(1, 1);
                watchdog_update();
                while (1);
                break;
            case CMD_READ_ENCODERS:
                //spi_write_blocking(spi_default, (uint8_t *)qdata, 32);
                //spi_write_read_blocking(spi_default, (uint8_t *)qdata, (uint8_t *)ddata, 32);
                //spi_write_read_blocking(spi_default, &dummyout, ddata, 1 );
                //spi_slave_write_8_blocking(spi_default, dummywritedata);
                spi_slave_write_8_n_blocking(spi_default, qdata, 32);
                // for(int i = 0; i < 32; i++)
                // {
                //     spi_slave_write_8_blocking(spi_default, qdata[i]);
                // }
                
                //printf("Read encoders command received. Wrote %d 0x%x\n", dummywritedata, dummywritedata);
                printf("Read encoders command received.\n");
                //dummywritedata++;
                break;
            default:
                printf("Unknown command received: %x\n", command);
                break;
            }

        }

        if(dummywritedata == 0xBB)
            dummywritedata = 0xAA;

        // pet the watchdog
        watchdog_update();
    }

    
}