#ifndef _4GMODULE_H
#define _4GMODULE_H

#include "main.h"
#include "usart.h"


extern DMA_HandleTypeDef hdma_usart2_rx;



typedef struct
{
    uint8_t Rx_Buff[200];
    uint8_t Rx_len;
    uint8_t Buff[200];
}USART2_RxStructure;
extern USART2_RxStructure USART2_RxStruct;

typedef struct
{
    uint8_t TxBuff[10];

    uint8_t ControlByte : 4;
    uint8_t DirByte : 4;
    uint8_t Speed[4];
}Modules4G_Structure;
extern Modules4G_Structure Modules4G_Struct;

void modifyString(char* str);
void USART2_IDLE_Handler(void);
void ParseModules4G(const char* string, int n);


#endif
