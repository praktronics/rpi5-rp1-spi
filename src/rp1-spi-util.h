#pragma once

#include "rp1-spi-regs.h"
#include "rp1-spi.h"

void dump_all_spi_regs(rp1_spi_instance_t *spi, const char *msg);
void dump_sr_msg(rp1_spi_instance_t *spi, const char *msg);
void dump_sr(uint32_t sr);
void dump_risr_msg(rp1_spi_instance_t *spi, const char *msg);
void dump_risr(uint32_t reg_risr);
void dump_ctrlr0_msg(rp1_spi_instance_t *spi, const char *msg);
void dump_ctrlr0(uint32_t reg_ctrlr0);