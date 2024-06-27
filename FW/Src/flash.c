
#include "flash.h"
#include <string.h>

uint8_t erase_flash(uint32_t sector_start, uint32_t sector_end)
{
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SECTORError = 0;
    uint32_t address, i;
    HAL_FLASH_Unlock(); //�����ڲ�flash
    //FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS; 	//���������в���
    EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
    EraseInitStruct.Sector        = sector_start;  						//������������  ���Ҫ���������������˴�������5
    EraseInitStruct.NbSectors     = sector_end; 							//����������β�������������������������6
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
    {
        return -1;
    }
    //address=add;
    HAL_FLASH_Lock();
    return 0;
}


uint8_t writedata_to_flash(uint8_t *data, uint32_t len, uint32_t address)
{
    HAL_FLASH_Unlock();
    //FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    uint32_t i = 0, temp;

    for(i = 0; i < len; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (uint32_t)address, *data) == HAL_OK)
        {
            address++;
            data++;
        }
        else
        {
            return -1;
        }
    }
    HAL_FLASH_Lock();
    return 0;
}
///////////////////////////////////////////////


