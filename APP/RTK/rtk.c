#include "rtk.h"

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include <string.h>
#include "main.h"


USART3_RxStructure USART3_RxStruct;


void processInfo(uint8_t* info1, uint8_t* info2, uint8_t* info3, uint8_t* outfo1, uint8_t* outfo2, uint8_t* outfo3, size_t bufferLength)
{
    // Initialize buffers with null characters
    memset(outfo1, 0, bufferLength);
    memset(outfo2, 0, bufferLength);
    memset(outfo3, 0, bufferLength);

    // Process info1
    if (info1[3] == 'R' && info1[4] == 'M' && info1[5] == 'C' && info1[6] == ',')
    {
        memcpy(outfo1, info1, bufferLength - 1);
    }
    else if (info1[3] == 'G' && info1[4] == 'G' && info1[5] == 'A')
    {
        memcpy(outfo2, info1, bufferLength - 1);
    }
    else //if (info1[3] == 'G' && info1[4] == 'S' && info1[5] == 'V')
    {
        memcpy(outfo3, info1, bufferLength - 1);
    }

    // Process info2
    if (info2[3] == 'R' && info2[4] == 'M' && info2[5] == 'C' && info2[6] == ',')
    {
        memcpy(outfo1, info2, bufferLength - 1);
    }
    else if (info2[3] == 'G' && info2[4] == 'G' && info2[5] == 'A')
    {
        memcpy(outfo2, info2, bufferLength - 1);
    }
    else //if (info2[3] == 'G' && info2[4] == 'S' && info2[5] == 'V')
    {
        memcpy(outfo3, info2, bufferLength - 1);
    }

    // Process info3
    if (info3[3] == 'R' && info3[4] == 'M' && info3[5] == 'C' && info3[6] == ',')
    {
        memcpy(outfo1, info3, bufferLength - 1);
    }
    else if (info3[3] == 'G' && info3[4] == 'G' && info3[5] == 'A')
    {
        memcpy(outfo2, info3, bufferLength - 1);
    }
    else //if (info3[3] == 'G' && info3[4] == 'S' && info3[5] == 'V')
    {
        memcpy(outfo3, info3, bufferLength - 1);
    }
}
void separateString(char* inputString, const char* delimiter, char* info1, char* info2, char* info3) {
    char* token = strtok(inputString, delimiter);

    // 使用 strncpy 将每个信息复制到相应的指针指向的缓冲区
    if (token != NULL) {
        strncpy(info1, token, strlen(token));
        info1[strlen(token)] = '\0';
    }
    else
    {
        info1[0] = '\0';
    }

    token = strtok(NULL, delimiter);

    if (token != NULL) {
        strncpy(info2, token, strlen(token));
        info2[strlen(token)] = '\0';
    }
    else
    {
        info2[0] = '\0';
    }

    token = strtok(NULL, delimiter);

    if (token != NULL) {
        strncpy(info3, token, strlen(token));
        info3[strlen(token)] = '\0';
    }
    else
    {
        info3[0] = '\0';
    }
}



//接收中断
void USART3_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(&huart3, UART_FLAG_IDLE))   //判断是否是空闲中断
    {
        UBaseType_t status_value = taskENTER_CRITICAL_FROM_ISR();
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);                     //清除空闲中断标志（否则会一直不断进入中断）
        HAL_UART_DMAStop(&huart3);

        ///////////////////////
        USART3_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);

        //复制到缓冲区
        memcpy(USART3_RxStruct.Buff, USART3_RxStruct.Rx_Buff, USART3_RxStruct.Rx_len);
        
        for(uint8_t i=0;i<USART3_RxStruct.Rx_len-10;i++)
        {
            if(USART3_RxStruct.Buff[i] == 'R')
            {
                if(USART3_RxStruct.Buff[i+1] == 'M' 
                    && USART3_RxStruct.Buff[i+2] == 'C' 
                 && USART3_RxStruct.Buff[i+3] == ',')
                {
                    //释放信号量
                    ReleaseBinarySemaphore(BinarySemaphore.RTK_RxRMCBinarySemHandle);
                    break;
                }
            }
            if(USART3_RxStruct.Buff[i] == 'G')
            {
                if(USART3_RxStruct.Buff[i+1] == 'G' 
                    && USART3_RxStruct.Buff[i+2] == 'A')
                {
                    //释放信号量
                    ReleaseBinarySemaphore(BinarySemaphore.RTK_RxRMCBinarySemHandle);
                    break;
                }
            }
            if(USART3_RxStruct.Buff[i] == 'R')
            {
                if(USART3_RxStruct.Buff[i+1] == 'M' 
                    && USART3_RxStruct.Buff[i+2] == 'C' 
                && USART3_RxStruct.Buff[i+3] == 'H')
                {
                    //释放信号量
                    ReleaseBinarySemaphore(BinarySemaphore.RTK_RxRMCBinarySemHandle);
                    break;
                }
            }
        }


        HAL_UART_Receive_DMA(&huart3, USART3_RxStruct.Rx_Buff, Rx_LENG);           //重启开始DMA传输

        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}









