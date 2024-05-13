#ifndef _FLASH_H
#define _FLASH_H

#include "main.h"

/* FLASH��С��STM32G070C8T6:128K */
#define STM32FLASH_SIZE         0x00020000UL
/* FLASH��ʼ��ַ */
#define STM32FLASH_BASE         FLASH_BASE
/* FLASH������ַ */
#define STM32FLASH_END          (STM32FLASH_BASE | STM32FLASH_SIZE)
/* FLASHҳ��С��2K */
#define STM32FLASH_PAGE_SIZE    FLASH_PAGE_SIZE
/* FLASH��ҳ�� */
#define STM32FLASH_PAGE_NUM     (STM32FLASH_SIZE / STM32FLASH_PAGE_SIZE)

//�����һ����������������
#define WRITE_START_ADDR        ((uint32_t)STM32FLASH_END-(uint32_t)STM32FLASH_PAGE_SIZE)
#define WRITE_END_ADDR          ((uint32_t)STM32FLASH_END)


HAL_StatusTypeDef flash_write(uint32_t address, uint64_t data);
void flash_page_erase(uint32_t page);
void FLASH_Program_DoubleWord(uint32_t Address, uint64_t Data);

void USER_FLASH_Write(uint8_t* string);
uint8_t *USER_FLASH_Read(void);



#endif
