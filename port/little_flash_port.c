
#include "little_flash.h"

static lf_err_t little_flash_spi_transfer(const little_flash_t *lf,uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len){
    lf_err_t result = LF_ERR_OK;
    return result;
}

static void little_flash_wait_us(uint32_t us){
    uint32_t delay = us;
    while(delay--);
}

static void little_flash_wait_ms(uint32_t ms){
    sleep(ms);
}

static void* little_flash_malloc(size_t size){
    return malloc(size);
}

static void little_flash_free(void* ptr){
    free(ptr);
}

lf_err_t little_flash_port_init(little_flash_t *lf){
    lf->spi.transfer = little_flash_spi_transfer;
    lf->wait_us = little_flash_wait_us;
    lf->wait_ms = little_flash_wait_ms;
    lf->malloc = little_flash_malloc;
    lf->free = little_flash_free;
    return LF_ERR_OK;
}
























