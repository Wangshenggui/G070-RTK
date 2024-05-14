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

//�ж�ָ������\
1��4Gģ������\
2��4Gģ���ѯ\
3��
uint8_t OrderType=0;
//�ж����ڲ����ͻ���ͨ��G431���յ�
uint8_t InsideOrOutside=0;

//�����ж�
void USART1_IDLE_Handler()
{
    if (RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
    {
        UBaseType_t status_value = taskENTER_CRITICAL_FROM_ISR();
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
        HAL_UART_DMAStop(&huart1);

        ///////////////////////
        USART1_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);

        //���Ƶ�������
        memcpy(USART1_RxStruct.Buff, USART1_RxStruct.Rx_Buff, USART1_RxStruct.Rx_len);

        //G431������Ϣ
        if(USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-2]=='4' 
            && USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-1]=='G' && 
        USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-5]=='#')
        {
            //ָ������Ϊ����4Gģ��
            OrderType=1;
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.G431_Rx4GBinarySemHandle);
        }
        else if(USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-2]=='4' 
            && USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-1]=='G' && 
        USART1_RxStruct.Buff[USART1_RxStruct.Rx_len-5]=='?')
        {
            //���Ϊ����
            InsideOrOutside=1;
            //ָ������Ϊ��ѯ4Gģ��
            OrderType=2;
            //�ͷ��ź���
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
            
            //RTK��λ
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
        //RTKģ��
        else
        {
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.G431_RxRTKBinarySemHandle);
        }
        
        HAL_UART_Receive_DMA(&huart1, USART1_RxStruct.Rx_Buff, 200);           //������ʼDMA����

        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}
















