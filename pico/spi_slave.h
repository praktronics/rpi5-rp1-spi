#pragma once

unsigned char spi_slave_read_8_blocking(spi_inst_t *spi);
void spi_slave_write_8_blocking(spi_inst_t *spi, unsigned char data);
void spi_slave_write_8_n_blocking(spi_inst_t *spi, unsigned char *data, int len);