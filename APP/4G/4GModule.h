#ifndef _4GMODULE_H
#define _4GMODULE_H

#include "main.h"
#include "usart.h"


extern DMA_HandleTypeDef hdma_usart2_rx;


#define Rx_LENG 200

typedef struct
{
    uint8_t Rx_Buff[Rx_LENG];
    uint8_t Rx_len;
    uint8_t Buff[Rx_LENG];
}USART2_RxStructure;
extern USART2_RxStructure USART2_RxStruct;


void USART2_IDLE_Handler(void);



#endif
