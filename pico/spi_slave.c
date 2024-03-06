#include "hardware/spi.h"
#include "spi_slave.h"

unsigned char spi_slave_read_8_blocking(spi_inst_t *spi)
{

    // spin waiting for the spi to be readable
    while (!spi_is_readable(spi))
        tight_loop_contents();

    // read the data
    return (unsigned char)spi_get_hw(spi)->dr;
}

void spi_slave_write_8_blocking(spi_inst_t *spi, unsigned char data)
{
    // spin waiting for the spi to be writable
    while (!spi_is_writable(spi))
        tight_loop_contents();

    // write the data
    spi_get_hw(spi)->dr = (uint32_t)data;

    // now wait for data to be available to read (the master must
    // clock data in to get our data above out)
    while (!spi_is_readable(spi))
        tight_loop_contents();

    // read the data to clear the RX FIFO
    unsigned char dump = spi_get_hw(spi)->dr;
}

void spi_slave_write_8_n_blocking(spi_inst_t *spi, unsigned char *data, int len)
{

    int bytesin = 0;
    unsigned char dump;

    for (int i = 0; i < len; i++)
    {
        while (!spi_is_writable(spi))
            tight_loop_contents();

        spi_get_hw(spi)->dr = (uint32_t)data[i];

        if (spi_is_readable(spi))
        {
            dump = spi_get_hw(spi)->dr;
            bytesin++;
        }
    }

    // now mop up any remaining data in the rx fifo
    while (bytesin < len)
    {
        if (spi_is_readable(spi))
        {
            dump = spi_get_hw(spi)->dr;
            bytesin++;
        }
    }
}