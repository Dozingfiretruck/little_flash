#ifndef _LITTLE_FLASH_DEFINE_H_
#define _LITTLE_FLASH_DEFINE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "little_flash_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LF_SW_VERSION                             "0.0.1"

struct little_flash;
typedef struct little_flash little_flash_t;

#ifndef LF_PRINTF
#define LF_PRINTF printf
#endif

#ifndef LF_INFO
#define LF_INFO(...)  LF_PRINTF(__VA_ARGS__)
#endif

#ifndef LF_ERROR
#define LF_ERROR(...)  LF_PRINTF(__VA_ARGS__)
#endif

/* assert for developer. */
#ifdef LF_DEBUG_MODE
#ifndef LF_DEBUG
#define LF_DEBUG(...)  LF_PRINTF(__VA_ARGS__)
#endif
#define LF_ASSERT(EXPR)                                                      \
if (!(EXPR))                                                                   \
{                                                                              \
    LF_PRINTF("(%s) has assert failed at %s.", #EXPR, __FUNCTION__);          \
    while (1);                                                                 \
}
#else
#ifndef LF_DEBUG
#define LF_DEBUG(...)
#endif
#define LF_ASSERT(EXPR)
#endif

#ifndef LF_NULL
#define LF_NULL NULL
#endif

#define LF_ENABLE       1
#define LF_DISABLE      0

/**
 * status register bits
 */
enum {
    LF_STATUS_REGISTER_BUSY = (1 << 0),                  /**< busing */
    LF_STATUS_REGISTER_WEL = (1 << 1),                   /**< write enable latch */
    LF_STATUS_REGISTER_SRP = (1 << 7),                   /**< status register protect */
};

typedef enum {
    LF_NOR_FLASH = 0,
    LF_NAND_FLASH,
}lf_type_t;

typedef enum {
    LF_ERR_OK = 0,
    LF_ERR_NO_FLASH,
    LF_ERR_NOT_INIT,
    LF_ERR_TRANSFER,
    LF_ERR_ERASE,
    LF_ERR_WRITE,
    LF_ERR_READ,
    LF_ERR_BUSY,
    LF_ERR_BAD_ADDRESS,
    LF_ERR_NO_MEM,
}lf_err_t;

#ifndef LF_NAME_LEN
#define LF_NAME_LEN 24
#endif

typedef struct {
    char name[LF_NAME_LEN];                      /**< flash chip name */
    uint8_t manufacturer_id;                     /**< MANUFACTURER ID */
    uint16_t device_id;                          /**< DEVICE ID */
    lf_type_t flash_type;                        /**< flash type */
    uint32_t capacity;                           /**< flash capacity (bytes) */
    uint8_t addr_bytes;                          /**< address bytes 2 3 4*/
    uint8_t erase_cmd;                           /**< erase granularity size block command */
    uint32_t erase_size;                         /**< erase granularity (bytes) */

} little_flash_chipinfo_t;

typedef struct{
    lf_err_t (*transfer)(const little_flash_t *lf,uint8_t *tx_buf, uint32_t tx_len, uint8_t *rx_buf, uint32_t rx_len);
    void* user_data;
} little_flash_spi_t;

struct little_flash{
    little_flash_chipinfo_t chip_info;
    little_flash_spi_t spi;
    void (*wait_us)(uint32_t us);
    void (*wait_ms)(uint32_t ms);
    void* (*malloc)(size_t size);
    void (*free)(void* ptr);
    void* user_data;
};

#define LF_NANDFLASH_PAGE_ZISE                      (2048)     /**< NAND flash page size (bytes) */

#define LF_CMD_WRITE_STATUS_REGISTER                (0x01)

#define LF_CMD_WRITE_DISABLE                        (0x04)

#define LF_CMD_READ_STATUS_REGISTER                 (0x05)

#define LF_CMD_WRITE_ENABLE                         (0x06)

#define LF_CMD_JEDEC_ID                             (0x9F)

#define LF_CMD_ENABLE_RESET                         (0x66)

#define LF_CMD_NORFLASH_RESET                       (0x99)

#define LF_CMD_BLOCK_ERASE                          (0xD8)

#define LF_CMD_NANDFLASH_RESET                      (0xFF)

#define LF_CMD_PROG_DATA                            (0x02)
#define LF_CMD_READ_DATA                            (0x03)

#define LF_NANDFLASH_PAGE_DATA_READ                 (0x13)
#define LF_NANDFLASH_PAGE_PROG_EXEC                 (0x10)


/* NAND flash status registers */
#define LF_NANDFLASH_STATUS_REGISTER1               (0xA0)
#define LF_NANDFLASH_STATUS_REGISTER2               (0xB0)
#define LF_NANDFLASH_STATUS_REGISTER3               (0xC0)
#define LF_NANDFLASH_STATUS_REGISTER4               (0xD0)

#ifdef __cplusplus
}
#endif

#endif /* _LITTLE_FLASH_DEFINE_H_ */










