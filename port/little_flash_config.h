#ifndef _LITTLE_FLASH_CONFIG_H_
#define _LITTLE_FLASH_CONFIG_H_

#include "luat_debug.h"


#ifdef __cplusplus
extern "C" {
#endif

// define the printf function for little flash
#define LF_PRINTF LUAT_DEBUG_PRINT

#define LF_DEBUG_MODE   /* enable debug mode for little flash */

#define LF_USE_SPI      /* enable SPI for little flash */

// #define LF_USE_QSPI     /* enable QSPI for little flash */

// #define LF_USE_SFDP     /* enable SFDP for little flash */

// #define LF_USE_NOR      /* enable NOR for little flash */

#define LF_USE_NAND     /* enable NAND for little flash */

// #define LF_USE_FMC      /* enable FMC for little flash */

#ifdef __cplusplus
}
#endif

#endif /* _LITTLE_FLASH_CONFIG_H_ */










