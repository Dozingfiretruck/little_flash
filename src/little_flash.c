
#include "little_flash.h"
#include "little_flash_table.h"

extern lf_err_t little_flash_port_init(little_flash_t *lf);

static const little_flash_chipinfo_t little_flash_table[] = LITTLE_FLASH_CHIP_TABLE;

lf_err_t little_flash_write_status(const little_flash_t *lf, uint8_t address, uint8_t status){
    lf_err_t result = LF_ERR_OK;
    uint8_t cmd_data[3]={0};
    cmd_data[0]=LF_CMD_WRITE_STATUS_REGISTER;
    if (address){
        cmd_data[1]=address;
        cmd_data[2]=status;
    }else{
        cmd_data[1]=status;
    }
    result |= lf->spi.transfer(lf,cmd_data, address?3:2,LF_NULL,0);
    return result;
}

lf_err_t little_flash_read_status(const little_flash_t *lf, uint8_t address, uint8_t *status){
    lf_err_t result = LF_ERR_OK;
    uint8_t cmd_data[2]={0};
    cmd_data[0]=LF_CMD_READ_STATUS_REGISTER;
    if (address){
        cmd_data[1]=address;
    }
    cmd_data[1]=address;
    result |= lf->spi.transfer(lf,cmd_data, address?2:1,status,1);
    return result;
}

// 待完善
static lf_err_t little_flash_wait_busy(const little_flash_t *lf) {
    lf_err_t result = LF_ERR_OK;
    uint8_t status;
    while (true) {
        result |= little_flash_read_status(lf, LF_NANDFLASH_STATUS_REGISTER3, &status);
        // LF_DEBUG("status 0x%02x",status);
        if (result==LF_ERR_OK && (status&LF_STATUS_REGISTER_BUSY)==0){
            return LF_ERR_OK;
        }
    }
    return result;
}

/* 
    reset device
*/
static lf_err_t little_flash_reset(const little_flash_t *lf){
    lf_err_t result = LF_ERR_OK;
    result |= little_flash_wait_busy(lf);
    // nand flash
    result |= lf->spi.transfer(lf,(uint8_t[]){LF_CMD_NANDFLASH_RESET}, 1,LF_NULL,0);
    lf->wait_us(500);
    result |= little_flash_wait_busy(lf);

    // 以下需要根据型号进行适配
    result |= little_flash_write_status(lf,LF_NANDFLASH_STATUS_REGISTER1,0x00);
    // ECC-E = 1, BUF = 1
    result |= little_flash_write_status(lf,LF_NANDFLASH_STATUS_REGISTER2,(1 << 4) | (1 << 3));

    return result;
}

static lf_err_t little_flash_write_enabled(const little_flash_t *lf, uint8_t enable){
    lf_err_t result = LF_ERR_OK;
    uint8_t status;
    // nand flash
    lf->spi.transfer(lf,enable?(uint8_t[]){LF_CMD_WRITE_ENABLE}:(uint8_t[]){LF_CMD_WRITE_DISABLE}, 1,LF_NULL,0);

    result |= little_flash_wait_busy(lf);

    // nand flash
    little_flash_read_status(lf, LF_NANDFLASH_STATUS_REGISTER3, &status);

    if (enable && (status & LF_STATUS_REGISTER_WEL) == 0) {
        LF_ERROR("Error: Can't enable write status.");
        result = LF_ERR_WRITE;
    } else if (!enable && (status & LF_STATUS_REGISTER_WEL) != 0) {
        LF_ERROR("Error: Can't disable write status.");
        result = LF_ERR_WRITE;
    }

    return result;
}

lf_err_t little_flash_init(void){
    LF_INFO("little_flash_init");
    return LF_ERR_OK;
}

lf_err_t little_flash_device_init(little_flash_t *lf){
    lf_err_t result = LF_ERR_OK;
    little_flash_port_init(lf);
    uint8_t cmd_data[2]={LF_CMD_JEDEC_ID}, recv_data[3]={0};
    result |= lf->spi.transfer(lf,cmd_data, 2, recv_data, sizeof(recv_data));
    if(result) return result;
    for (size_t i = 0; i < sizeof(little_flash_table)/sizeof(little_flash_table[0]); i++){
        if (recv_data[0]==little_flash_table[i].manufacturer_id && (recv_data[1]<<8|recv_data[2])==little_flash_table[i].device_id){
            memcpy(&lf->chip_info.name,&little_flash_table[i],sizeof(little_flash_chipinfo_t));
            LF_DEBUG("fonud flash %s %d",lf->chip_info.name,i);
            result |= little_flash_reset(lf);
            return result;
        }
    }
    LF_DEBUG("NOT fonud flash");
    return LF_ERR_NO_FLASH;
}

lf_err_t little_flash_device_deinit(little_flash_t *lf){



    return LF_ERR_OK;
}

lf_err_t little_flash_deinit(void){
    return LF_ERR_OK;
}

// lf_err_t little_flash_get_info(const little_flash_t *lf, uint32_t *total_size, uint32_t *block_size, uint32_t *page_size, uint32_t *sector_size);

static lf_err_t little_flash_cheak_erase(const little_flash_t *lf){
    lf_err_t result = LF_ERR_OK;
    uint8_t status;
    result |= little_flash_wait_busy(lf);
    result |= little_flash_read_status(lf, LF_NANDFLASH_STATUS_REGISTER3, &status);
    if (result || (status&0x04)){
        return LF_ERR_ERASE;
    }
    return result;
}

static lf_err_t little_flash_cheak_write(const little_flash_t *lf){
    lf_err_t result = LF_ERR_OK;
    uint8_t status;
    result |= little_flash_wait_busy(lf);
    result |= little_flash_read_status(lf, LF_NANDFLASH_STATUS_REGISTER3, &status);
    if (result||(status&0x08)){
        return LF_ERR_WRITE;
    }
    return result;
}

static lf_err_t little_flash_cheak_read(const little_flash_t *lf){
    lf_err_t result = LF_ERR_OK;
    uint8_t status;
    result |= little_flash_wait_busy(lf);
    result |= little_flash_read_status(lf, LF_NANDFLASH_STATUS_REGISTER3, &status);
    // 以下也是要根据不同型号移植的
    uint8_t ecc = (status & 0x30) >> 4;
    if (result==0 && ecc<2){
        return LF_ERR_OK;
    }
    return LF_ERR_READ;
}

lf_err_t little_flash_chip_erase(const little_flash_t *lf){
    lf_err_t result = LF_ERR_OK;
    uint32_t addr = 0;
    uint8_t cmd_data[4];
    result |= little_flash_write_enabled(lf, LF_ENABLE);
    if(result) return result;
    cmd_data[0] = lf->chip_info.erase_cmd;
    while (true){
        uint16_t page_addr = addr/LF_NANDFLASH_PAGE_ZISE;
        cmd_data[2] = page_addr >> 8;
        cmd_data[3] = page_addr;
        result |= lf->spi.transfer(lf,cmd_data, 4,LF_NULL,0);
        if(result) return result;
        lf->wait_ms(2);
        result |= little_flash_cheak_erase(lf);
        if(result) return result;
        addr += lf->chip_info.addr_bytes;
        if (addr>=lf->chip_info.capacity){
            break;
        }
    }
    result |= little_flash_write_enabled(lf, LF_DISABLE);
    return result;
}

lf_err_t little_flash_erase(const little_flash_t *lf, uint32_t addr, uint32_t len){
    lf_err_t result = LF_ERR_OK;
    uint8_t cmd_data[4]={0};
    if (addr + len > lf->chip_info.capacity) {
        LF_ERROR("Error: Flash address is out of bound.");
        return LF_ERR_BAD_ADDRESS;
    }

    if (addr == 0 && len == lf->chip_info.capacity) {
        return little_flash_chip_erase(lf);
    }

    result |= little_flash_write_enabled(lf, LF_ENABLE);
    if(result) return result;
    cmd_data[0] = lf->chip_info.erase_cmd;
    while (len){
        uint32_t page_addr = addr/LF_NANDFLASH_PAGE_ZISE;
        cmd_data[2] = page_addr >> 8;
        cmd_data[3] = page_addr;
        result |= lf->spi.transfer(lf,cmd_data, 4,LF_NULL,0);
        if(result) return result;
        lf->wait_ms(2);
        result |= little_flash_cheak_erase(lf);
        if(result) return result;
        addr += lf->chip_info.erase_size;
        if (len<=lf->chip_info.erase_size){
            break;
        }else{
            len -= lf->chip_info.erase_size;
        }
    }
    result |= little_flash_write_enabled(lf, LF_DISABLE);
    return result;
}


lf_err_t little_flash_write(const little_flash_t *lf, uint32_t addr, const uint8_t *data, uint32_t len){
    // 250us
    lf_err_t result = LF_ERR_OK;
    uint8_t* cmd_data = (uint8_t*)lf->malloc(4+LF_NANDFLASH_PAGE_ZISE);
    if (!cmd_data){
        LF_ERROR("Error: malloc failed.");
        return LF_ERR_NO_MEM;
    }
    uint32_t base_addr = addr;
    result |= little_flash_write_enabled(lf, LF_ENABLE);

    while (len){
        result |= little_flash_wait_busy(lf);
        uint16_t page_addr = addr/LF_NANDFLASH_PAGE_ZISE;
        uint16_t column_addr = addr%LF_NANDFLASH_PAGE_ZISE;
        cmd_data[0] = LF_CMD_PROG_DATA;
        cmd_data[1] = column_addr >> 8;
        cmd_data[2] = column_addr;
        if (column_addr){
            if ((column_addr+len)<=LF_NANDFLASH_PAGE_ZISE){
                memcpy(&cmd_data[3],&data[addr-base_addr],len);
                lf->spi.transfer(lf,cmd_data, 3+len,LF_NULL,0);
                little_flash_wait_busy(lf);
                cmd_data[0] = LF_NANDFLASH_PAGE_PROG_EXEC;
                cmd_data[1] = 0;
                cmd_data[2] = page_addr >> 8;
                cmd_data[3] = page_addr;
                lf->spi.transfer(lf,cmd_data, 4,LF_NULL,0);
                little_flash_cheak_write(lf);
                break;
            }else{
                memcpy(&cmd_data[3],&data[addr-base_addr],LF_NANDFLASH_PAGE_ZISE-column_addr);
                lf->spi.transfer(lf,cmd_data, 3+LF_NANDFLASH_PAGE_ZISE-column_addr,LF_NULL,0);
                len -= (LF_NANDFLASH_PAGE_ZISE-column_addr);
                addr += (LF_NANDFLASH_PAGE_ZISE-column_addr);
            }
        }else{
            if (len<=LF_NANDFLASH_PAGE_ZISE){
                memcpy(&cmd_data[3],&data[addr-base_addr],len);
                lf->spi.transfer(lf,cmd_data, 3+len,LF_NULL,0);
                little_flash_wait_busy(lf);
                cmd_data[0] = LF_NANDFLASH_PAGE_PROG_EXEC;
                cmd_data[1] = 0;
                cmd_data[2] = page_addr >> 8;
                cmd_data[3] = page_addr;
                lf->spi.transfer(lf,cmd_data, 4,LF_NULL,0);
                little_flash_cheak_write(lf);
                break;
            }else{
                memcpy(&cmd_data[3],&data[addr-base_addr],LF_NANDFLASH_PAGE_ZISE);
                lf->spi.transfer(lf,cmd_data, 3+LF_NANDFLASH_PAGE_ZISE,LF_NULL,0);
                len -= LF_NANDFLASH_PAGE_ZISE;
                addr += LF_NANDFLASH_PAGE_ZISE;
            }
        }

        little_flash_wait_busy(lf);
        cmd_data[0] = LF_NANDFLASH_PAGE_PROG_EXEC;
        cmd_data[1] = 0;
        cmd_data[2] = page_addr >> 8;
        cmd_data[3] = page_addr;
        lf->spi.transfer(lf,cmd_data, 4,LF_NULL,0);
        little_flash_cheak_write(lf);

    }
    little_flash_write_enabled(lf, LF_DISABLE);
    lf->free(cmd_data);
    return LF_ERR_OK;
}



lf_err_t little_flash_read(const little_flash_t *lf, uint32_t addr, uint8_t *data, uint32_t len){
    uint8_t cmd_data[4];
    uint32_t base_addr = addr;
    while (len){
        little_flash_wait_busy(lf);

        uint16_t page_addr = addr/LF_NANDFLASH_PAGE_ZISE;
        uint16_t column_addr = addr%LF_NANDFLASH_PAGE_ZISE;

        cmd_data[0] = LF_NANDFLASH_PAGE_DATA_READ;
        cmd_data[1] = 0;
        cmd_data[2] = page_addr >> 8;
        cmd_data[3] = page_addr;
        lf->spi.transfer(lf,cmd_data, 4,LF_NULL,0);
        if (little_flash_cheak_read(lf)){
            return LF_ERR_READ;
        }
        cmd_data[0] = LF_CMD_READ_DATA;
        cmd_data[1] = column_addr >> 8;
        cmd_data[2] = column_addr;
        cmd_data[3] = 0;
        if (column_addr){
            if ((column_addr+len)<=LF_NANDFLASH_PAGE_ZISE){
                lf->spi.transfer(lf,cmd_data, 4,&data[addr-base_addr],len);
                break;
            }else{
                lf->spi.transfer(lf,cmd_data, 4,&data[addr-base_addr],LF_NANDFLASH_PAGE_ZISE-column_addr);
                len -= (LF_NANDFLASH_PAGE_ZISE-column_addr);
                addr += (LF_NANDFLASH_PAGE_ZISE-column_addr);
            }
        }else{
            if (len<=LF_NANDFLASH_PAGE_ZISE){
                lf->spi.transfer(lf,cmd_data, 4,&data[addr-base_addr],len);
                break;
            }else{
                lf->spi.transfer(lf,cmd_data, 4,&data[addr-base_addr],LF_NANDFLASH_PAGE_ZISE);
                len -= LF_NANDFLASH_PAGE_ZISE;
                addr += LF_NANDFLASH_PAGE_ZISE;
            }
        }
    }
    return LF_ERR_OK;
}




















