#include "flash.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "usart.h"



HAL_StatusTypeDef flash_write(uint32_t address, uint64_t data)
{
    HAL_StatusTypeDef ret = HAL_OK;

    HAL_FLASH_Unlock();
    ret = HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, address , data);
    HAL_FLASH_Lock();

    return ret;
}

void flash_page_erase(uint32_t page)
{
    HAL_StatusTypeDef test = HAL_FLASH_Unlock();

    test = FLASH_WaitForLastOperation(1000); //1s timeout
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR);
    FLASH_PageErase(FLASH_BANK_1,page);
    test = FLASH_WaitForLastOperation(1000);
    CLEAR_BIT(FLASH->CR, FLASH_CR_PER);

    HAL_FLASH_Lock();
}


//写入一个字符串
void USER_FLASH_Write(uint8_t* string)
{
    uint8_t* ptr = string;
    int length = strlen((char*)string);
    
    union
    {
        uint64_t uint64_value;
        uint8_t bytes[8];
    }ByteConverter;

    //擦除一个扇区
    flash_page_erase(STM32FLASH_PAGE_NUM - 1);
    
    // 循环遍历字符串
    for (int i = 0; i < length; i += 8) 
    {
        // 按照组大小打印字符
        for (int j = 0; j < 8 && *ptr != '\0'; j++) 
        {
            ByteConverter.bytes[j] = *ptr++;
        }
        //写入一个数据double-word (64-bit)
        uint8_t ret = flash_write((uint32_t)(WRITE_START_ADDR + i*8), (uint64_t)ByteConverter.uint64_value);
    }
}

uint8_t *USER_FLASH_Read()
{
    static uint8_t OutStr[100] = {0};
    
    uint8_t temp[10] = {0};
    
    memset(OutStr,0,100);
    for(uint8_t i=0;;i+=8)
    {
        memset(temp,0,10);
        uint8_t v=0;
        for(v=0;v<8;v++)
        {
            uint8_t u_data1 = *((uint8_t*)(WRITE_START_ADDR + i*8 + v));
            if(u_data1 == '\n')
            {
                sprintf((char*)OutStr,"%s%s",(char*)OutStr,(char*)temp);
                return OutStr;
            }
            sprintf((char*)temp,"%s%c",(char*)temp,u_data1);
        }
        sprintf((char*)OutStr,"%s%s",(char*)OutStr,(char*)temp);
    }
}















