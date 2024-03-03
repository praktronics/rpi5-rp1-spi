#include <stdio.h>
#include "rp1-spi-util.h"

//#include "rp1-spi-regs.h"
//#include "rp1-spi.h"

const char *boldred = "\033[1;31m";
const char *boldblue = "\033[1;34m";
const char *boldgreen = "\033[1;32m";
const char *normal = "\033[0m";

void dump_all_spi_regs(rp1_spi_instance_t *spi, const char *msg) {

    printf("\n%sSPI register dump: %s%s\n", boldblue, normal, msg);    

    for(int i=DW_SPI_CTRLR0;i<=DW_SPI_CS_OVERRIDE;i+=4) {
        printf("spi @ %x: %x\n", i, *(uint32_t*)(spi->regbase + i));
    }
}

void dump_sr_msg(rp1_spi_instance_t *spi, const char *msg) {
    printf("\n%sStatus register dump: %s%s\n", boldblue, normal, msg);
    dump_sr(*(volatile uint32_t *)(spi->regbase + DW_SPI_SR));
}

void dump_sr(uint32_t sr) {
        
    printf("\nstatus register: %x\n", sr);
    printf("DW_SPI_SR_BUSY: %s\n", (sr & DW_SPI_SR_BUSY)?"yes":"");
    printf("DW_SPI_SR_TF_NOT_FULL: %s\n", (sr & DW_SPI_SR_TF_NOT_FULL)?"yes":"");
    printf("DW_SPI_SR_TF_EMPT: %s\n", (sr & DW_SPI_SR_TF_EMPT)?"yes":"");
    printf("DW_SPI_SR_RF_NOT_EMPT: %s\n", (sr & DW_SPI_SR_RF_NOT_EMPT)?"yes":"");
    printf("DW_SPI_SR_RF_FULL: %s\n", (sr & DW_SPI_SR_RF_FULL)?"yes":"");
    printf("DW_SPI_SR_TX_ERR: %s\n", (sr & DW_SPI_SR_TX_ERR)?"yes":"");
    printf("DW_SPI_SR_DCOL: %s\n\n", (sr & DW_SPI_SR_DCOL)?"yes":"");
}

void dump_risr_msg(rp1_spi_instance_t *spi, const char *msg) {
    printf("\n%sRISR dump: %s%s\n", boldblue, msg, normal);
    dump_risr(*(volatile uint32_t *)(spi->regbase + DW_SPI_RISR));
}
void dump_risr(uint32_t reg_risr) {

    printf("\nrisr: %x\n", reg_risr);
    printf("DW_SPI_INT_TXEI: %s\n", (reg_risr & DW_SPI_INT_TXEI)?"yes":"");
    printf("DW_SPI_INT_TXOI: %s\n", (reg_risr & DW_SPI_INT_TXOI)?"yes":"");
    printf("DW_SPI_INT_RXUI: %s\n", (reg_risr & DW_SPI_INT_RXUI)?"yes":"");
    printf("DW_SPI_INT_RXOI: %s\n", (reg_risr & DW_SPI_INT_RXOI)?"yes":"");
    printf("DW_SPI_INT_RXFI: %s\n", (reg_risr & DW_SPI_INT_RXFI)?"yes":"");
    printf("DW_SPI_INT_MSTI: %s\n\n", (reg_risr & DW_SPI_INT_MSTI)?"yes":"");
}


void dump_ctrlr0_msg(rp1_spi_instance_t *spi, const char *msg) {
    printf("\n%sCTRLR0 dump: %s%s\n", boldblue, msg, normal);
    dump_ctrlr0(*(volatile uint32_t *)(spi->regbase + DW_SPI_CTRLR0));
}
void dump_ctrlr0(uint32_t reg_ctrlr0) {
    printf("ctrlr0: %x\n", reg_ctrlr0);
    // check frame size 32
    printf("frame size 32: %d\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_DFS32_MASK) >> 16);
    printf("frame size 8: %d\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_DFS_MASK));
    printf("format: %d\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_FRF_MASK) >> 4);
    printf("mode: %d\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_MODE_MASK) >> 6);
    printf("tmod: %d\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_TMOD_MASK) >> 8);
    printf("slave output enable: %s\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_SLV_OE)?"yes":"");
    printf("srl: %s\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_SRL)?"yes":"");
    printf("cfs: %s\n\n", (reg_ctrlr0 & DW_PSSI_CTRLR0_CFS)?"yes":"");

}