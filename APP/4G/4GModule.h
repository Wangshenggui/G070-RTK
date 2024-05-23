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

typedef struct
{
    uint8_t COM : 3;
    uint8_t Freq : 4;
    uint8_t GGA : 1;

    uint8_t GSV : 1;
    uint8_t RMC : 1;
    uint8_t Satellite : 1;
    uint8_t Antenna : 1;
} Mod4G_Structure;
extern Mod4G_Structure Mod4G_Struct;

void modifyString(char* str);
void USART2_IDLE_Handler(void);
void ParseModules4G(const char* string, int n);

typedef void (*ParseCallback)(const char*);
void COM_Callback(const char* result);
void GGA_Callback(const char* result);
void GSV_Callback(const char* result);
void RMC_Callback(const char* result);
void Freq_Callback(const char* result);
void Satellite_Callback(const char* result);
void Antenna_Callback(const char* result);
void Parse4G_RTKConfig(const char* string, int n, ParseCallback callback);


#endif
