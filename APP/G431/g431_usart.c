#include "g431_usart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "flash.h"


USART1_RxStructure USART1_RxStruct;

//判断指令类型\
1：4G模块配置\
2：4G模块查询\
3：
uint8_t OrderType=0;
//判断是内部发送还是通过G431接收的
uint8_t InsideOrOutside=0;

//接收中断
void USART1_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //判断是否是空闲中断
    {
        UBaseType_t status_value = taskENTER_CRITICAL_FROM_ISR();
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //清除空闲中断标志（否则会一直不断进入中断）
        HAL_UART_DMAStop(&huart1);

        ///////////////////////
        USART1_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

        //复制到缓冲区
        memcpy(USART1_RxStruct.Buff, USART1_RxStruct.Rx_Buff, USART1_RxStruct.Rx_len);

        //G431接收信息
        if(USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-2]=='4' 
            && USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-1]=='G' && 
        USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-5]=='#')
        {
            //指令类型为配置4G模块
            OrderType=1;
            //释放信号量
            ReleaseBinarySemaphore(BinarySemaphore.G431_Rx4GBinarySemHandle);
        }
        else if(USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-2]=='4' 
            && USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-1]=='G' && 
        USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-5]=='?')
        {
            //标记为接收
            InsideOrOutside=1;
            //指令类型为查询4G模块
            OrderType=2;
            //释放信号量
            ReleaseBinarySemaphore(BinarySemaphore.G431_Rx4GBinarySemHandle);
        }
        else if(
            USART1_RxStruct.Buff[0]=='$' 
            && USART1_RxStruct.Buff[1]=='C' 
            && USART1_RxStruct.Buff[2]=='O' 
            && USART1_RxStruct.Buff[3]=='R' 
            && USART1_RxStruct.Buff[4]=='S' 
            && USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-1]=='\n' 
        )
        {
            uint8_t temp[200] = {0};
            
            HAL_GPIO_TogglePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin);
            
            sprintf((char*)temp,"%s",USART1_RxStruct.Buff);
            USER_FLASH_Write((uint8_t*)temp);
            
            sprintf((char*)temp,"$CORS,0,4,9,Y2VkcjE3NjIyNjpobTBkY3ZudA==\r\n");
            HAL_UART_Transmit(&huart2, temp, strlen((char*)temp),1000);
            HAL_UART_Transmit(&huart2, temp, strlen((char*)temp),1000);
            HAL_UART_Transmit(&huart2, temp, strlen((char*)temp),1000);
            
            //RTK复位
            //HAL_UART_Transmit(&huart3, (uint8_t*)"RESET\r\n", strlen((char*)"RESET\r\n"),1000);
            
            HAL_NVIC_SystemReset();
        }
        else if(
            USART1_RxStruct.Buff[0]=='$' 
            && USART1_RxStruct.Buff[1]=='S' 
            && USART1_RxStruct.Buff[2]=='L' 
            && USART1_RxStruct.Buff[3]=='A' 
            && USART1_RxStruct.Buff[4]=='V' 
            && USART1_RxStruct.Buff[5]=='E' 
            && USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-1]=='\n' 
        )
        {
            //HAL_UART_Transmit(&huart2, USART1_RxStruct.Buff, USART1_RxStruct.Rx_len,1000);
        }
        //RTK模块
        else
        {
            //释放信号量
            ReleaseBinarySemaphore(BinarySemaphore.G431_RxRTKBinarySemHandle);
        }
        
        HAL_UART_Receive_DMA(&huart1, USART1_RxStruct.Rx_Buff, 200);           //重启开始DMA传输

        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}
















