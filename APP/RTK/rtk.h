#ifndef _RTK_H
#define _RTK_H

#include "main.h"
#include "usart.h"

#include "4gmodule.h"


extern DMA_HandleTypeDef hdma_usart3_rx;

#ifndef Rx_LENG
#define Rx_LENG 400
#endif

typedef struct
{
    uint8_t Rx_Buff[Rx_LENG];
    uint16_t Rx_len;
    uint8_t Buff[Rx_LENG];
}USART3_RxStructure;
extern USART3_RxStructure USART3_RxStruct;


void processInfo(uint8_t* info1, uint8_t* info2, uint8_t* info3, uint8_t* rmcBuffer, uint8_t* ggaBuffer, uint8_t* gsvBuffer, size_t bufferLength);
void separateString(char* inputString, const char* delimiter, char* info1, char* info2, char* info3);
void USART3_IDLE_Handler(void);

void ConfigRTK(Mod4G_Structure M4G);




#endif
