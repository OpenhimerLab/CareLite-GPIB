#ifndef __FLASH_H
#define __FLASH_H

#include "main.h"

#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

/* FLASH大小：128K */
#define STM32FLASH_SIZE  0x00040000UL

/* FLASH起始地址 */
#define STM32FLASH_BASE  FLASH_BASE

/* FLASH结束地址 */
#define STM32FLASH_END   (STM32FLASH_BASE | STM32FLASH_SIZE)

#define FLASH_PAGE_SIZE 0x800
/* FLASH页大小：1K */
#define STM32FLASH_PAGE_SIZE FLASH_PAGE_SIZE

/* FLASH总页数 */
#define STM32FLASH_PAGE_NUM  (STM32FLASH_SIZE / STM32FLASH_PAGE_SIZE)


uint8_t erase_flash(uint32_t sector_start,uint32_t sector_end);
uint8_t writedata_to_flash(uint8_t *data,uint32_t len,uint32_t address);

#endif // !__FLASH_H
