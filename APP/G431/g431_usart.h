#ifndef _G431_USART_H
#define _G431_USART_H

#include "main.h"
#include "usart.h"


extern DMA_HandleTypeDef hdma_usart1_rx;


#ifndef Rx_LENG
#define Rx_LENG 400
#endif

typedef struct
{
    uint8_t Rx_Buff[Rx_LENG];
    uint8_t Rx_len;
    uint8_t Buff[Rx_LENG];
}USART1_RxStructure;
extern USART1_RxStructure USART1_RxStruct;


void USART1_IDLE_Handler(void);




#endif
