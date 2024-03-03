#include <stddef.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "rp1-regs.h"
#include "rp1-spi.h"
#include "rp1-spi-regs.h"

const uint32_t spi_bases[] = {
    RP1_SPI0_BASE,
    RP1_SPI1_BASE,
    RP1_SPI2_BASE,
    RP1_SPI3_BASE,
    RP1_SPI4_BASE,
    RP1_SPI5_BASE,
    RP1_SPI6_BASE,
    RP1_SPI7_BASE,
    RP1_SPI8_BASE
};

bool rp1_spi_create(rp1_t *rp1, uint8_t spinum, rp1_spi_instance_t **spi)
{

    rp1_spi_instance_t *s = (rp1_spi_instance_t *)calloc(1, sizeof(rp1_spi_instance_t));
    if (s == NULL)
        return false;

    s->regbase = rp1->rp1_peripherial_base + spi_bases[spinum];
    s->txdata = (char *)0x0;
    s->rxdata = (char *)0x0;
    s->txcount = 0x0;

    *spi = s;

    return true;
}


/// @brief Writes 8 bits of data to the SPI bus, blocking until it can write and until the write is complete
/// @param spi SPI instance
/// @param data 8 bits of data to write (unsigned char)
/// @return SPI_OK if successful - note it does not return if busy - it waits until the SPI is not busy
spi_status_t rp1_spi_write_8_blocking(rp1_spi_instance_t *spi, uint8_t data)
{

    // wait until the spi is not busy
    while(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_BUSY)
    {
        ;
    }

    spi->txdata = &data;
    
    // spin until we can write to the fifo
    while(!(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_TF_NOT_FULL))
    {
       ;
    }

    // set the CS pin
    *(volatile uint32_t *)(spi->regbase + DW_SPI_SER) = 1 <<0;

    // put the data into the fifo
    *(volatile uint8_t *)(spi->regbase + DW_SPI_DR) = data;

    // we now need to pull exactly one byte out of the fifo which would
    // have been clocked in when we wrote the data    
    
    while( (!*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_RF_NOT_EMPT) || (*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_BUSY))   // check if there is data to read (check status register for Read Fifo Not Empty)
    {
        ;
    }
    /*uint8_t discard = */*(volatile uint8_t *)(spi->regbase + DW_SPI_DR);
    //printf("write_8 - discarded: %d\n", discard);

    return SPI_OK;
}


/// @brief Reads a number of 8-bit bytes from the SPI bus, blocking until the read is complete
/// @param spi SPI instance
/// @param data buffer to read into
/// @param len number of bytes to read
/// @param timeout timeout in ms - not yet implemented
/// @return 
spi_status_t rp1_spi_read_8_n_blocking(rp1_spi_instance_t *spi, uint8_t *data, uint32_t len, uint32_t timeout)
{
    if (spi->txcount != 0)
        return SPI_BUSY;
    if (len == 0)
        return SPI_INVALID;
   spi->txcount = len;

    // how this works
    // 1. We stuff the TX FIFO with dummy data (zeros, but can be anything you want) until it is full,
    //    or we have stuffed the number of bytes we want to read (we write in order to generate the 
    //    clock pulses to the slave, which sends us data to read)
    // 2. We then set the CS pin to active transmission
    // 3. We then continue to feed the TX FIFO with dummy data until we have sent all the bytes 
    //    we want to send, if we didn't manage them all in step 1
    // 4. As we are feeding the TX FIFO with dummy data, we are clocking out data from the slave
    //    which we read from the RX fifo and discard
    // 5. Once we have sent all the data, we then read the RX FIFO until it is empty, and store the data
    //    in the buffer we were passed
    // 6. The CS pin is turned off by the hardware when the last bit is clocked out

    // pre-stuff the TX buffer with dummy data
    while((*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_TF_NOT_FULL) && (spi->txcount > 0))
    {
        *(volatile uint8_t *)(spi->regbase + DW_SPI_DR) = (uint8_t)0x00;
        spi->txcount--;
    }
    
    // set the CS pin - since we have pre-stuffed data, the clock should start here
    // note the behaviour of te CS pin (active low, or high) is determined by the hardware
    // and the GPIO / PAD settings, but default is active low
    *(volatile uint32_t *)(spi->regbase + DW_SPI_SER) = 1 << 0;  // TODO - fix this to use the correct CS pin
    
    int inbyte = 0;
    // keep loading data into the tx fifo and also see if we have anything to read in the rx fifo
    while((*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_TF_NOT_FULL) && (spi->txcount > 0))
    {
        *(volatile uint8_t *)(spi->regbase + DW_SPI_DR) = (uint8_t)0x00;
        spi->txcount--;
        // check if there is data to read (check status register for Read Fifo Not Empty)
        if(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_RF_NOT_EMPT)
        {
            data[inbyte] = *(volatile uint8_t *)(spi->regbase + DW_SPI_DR);
            inbyte++;
        }
    }

    // read the remaining bytes from the buffer
    while(inbyte < len)
    {
        // check if there is data to read (check status register for Read Fifo Not Empty)
        if(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_RF_NOT_EMPT)
        {
            data[inbyte] = *(volatile uint8_t *)(spi->regbase + DW_SPI_DR);
            inbyte++;
        }
    }

    return SPI_OK;
}

spi_status_t rp1_spi_read_32_n(rp1_spi_instance_t *spi, uint32_t *data, uint32_t len, uint32_t timeout)
{
    if (spi->txcount != 0)
        return SPI_BUSY;
    if (len == 0)
        return SPI_INVALID;

    //todo: implement timeout

   
    spi->txcount = len;

    // set the frame size to 32 bits
    *(volatile uint32_t *)(spi->regbase + DW_SPI_CTRLR0) = ((*(volatile uint32_t *)(spi->regbase + DW_SPI_CTRLR0)) | DW_PSSI_CTRLR0_DFS32_MASK | DW_PSSI_CTRLR0_DFS_MASK);

    // pre-stuff the TX buffer with dummy data
    while((*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_TF_NOT_FULL) && (spi->txcount > 0))
    {
        *(volatile uint32_t *)(spi->regbase + DW_SPI_DR) = (uint32_t)0x00;
        spi->txcount--;
    }


    // set the CS pin
    *(volatile uint32_t *)(spi->regbase + DW_SPI_SER) = 1 <<0;
    
    int indw = 0;
    // now load the dummy data into the TX FIFO to start the clock
    while((*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_TF_NOT_FULL) && (spi->txcount > 0))
    {
        *(volatile uint32_t *)(spi->regbase + DW_SPI_DR) = (uint32_t)0x00;
        spi->txcount--;
        // check if there is data to read (check status register for Read Fifo Not Empty)
        if(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_RF_NOT_EMPT)
        {
            data[indw] = *(volatile uint32_t *)(spi->regbase + DW_SPI_DR);
            indw++;
        }
    
    }

    // read the remaining dwords from the buffer
    while(indw < len)
    {
        // check if there is data to read (check status register for Read Fifo Not Empty)
        if(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR) & DW_SPI_SR_RF_NOT_EMPT)
        {
            data[indw] = *(volatile uint32_t *)(spi->regbase + DW_SPI_DR);
            indw++;
        }
    }

    // turn off the CS pin
    *(volatile uint32_t *)(spi->regbase + DW_SPI_SER) = 0x00;

    return SPI_OK;
}

spi_status_t rp1_spi_purge_rx_fifo(rp1_spi_instance_t *spi, int* dwordspurged)
{
    if (spi->txcount != 0)
        return SPI_BUSY;

    int readcount = 0;
    uint32_t temp;

    // read the remaining dwords from the buffer
    while((*(volatile uint32_t *)(spi->regbase + DW_SPI_SR)) & DW_SPI_SR_RF_NOT_EMPT)
    {
        // check if there is data to read (check status register for Read Fifo Not Empty)
        temp = *(volatile uint32_t *)(spi->regbase + DW_SPI_DR);
        readcount++;
    }

    *dwordspurged = readcount;

    //printf("purge: discarded %d\n", temp);

    return SPI_OK;
}