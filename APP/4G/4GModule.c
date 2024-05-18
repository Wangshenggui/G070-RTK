#include "4gmodule.h"
#include <string.h>


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

USART2_RxStructure USART2_RxStruct;

//�жϷ�����Ϣ��־\
1��OK\
2��+GSTATE:6001\
3��
uint8_t ReturnFlag=0;

extern //�ж����ڲ����ͻ���ͨ��G431���յ�
uint8_t InsideOrOutside;

//4Gģ��������Ϣ��־\
0:\
1:��ʼ����\
2:���óɹ�\
3:��Ϣ����
uint8_t ConfigurationFlag=0;

void USART2_IDLE_Handler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
    {
        UBaseType_t status_value = taskENTER_CRITICAL_FROM_ISR();
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
        HAL_UART_DMAStop(&huart2);

        ///////////////////////
        USART2_RxStruct.Rx_len = Rx_LENG - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

        //���Ƶ�������
        memcpy(USART2_RxStruct.Buff, USART2_RxStruct.Rx_Buff, USART2_RxStruct.Rx_len);

        
        //4Gģ�������Ϣ
        if(USART2_RxStruct.Buff[0]=='O' && USART2_RxStruct.Buff[1]=='K')
        {
            ReturnFlag=1;
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        //���÷���
        else if(USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-9]=='1' 
            && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-7]=='0')
        {
            //���ⲿ���յ�
            if(InsideOrOutside==1)
            {
                ReturnFlag=4;
            }
            else
            {
                //���óɹ�
                ReturnFlag=2;
            }
            
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        else if(USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-9]=='0' 
            && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-7]=='0')
        {
            //����ʧ��
            ReturnFlag=3;
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        else if(USART2_RxStruct.Buff[0]=='+')
        {
            ReturnFlag=4;
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        //$CORS,STAR\r\n
        else if(USART2_RxStruct.Buff[0]=='$' && USART2_RxStruct.Buff[6]=='S' && USART2_RxStruct.Buff[7]=='T')
        {
            ConfigurationFlag = 1;
        }
        //$CORS,OK\r\n
        else if(USART2_RxStruct.Buff[0]=='$' && USART2_RxStruct.Buff[6]=='O' && USART2_RxStruct.Buff[7]=='K')
        {
            ConfigurationFlag = 2;
        }
        //$CORS,ERRORpassword\r\n
        else if(USART2_RxStruct.Buff[0]=='$' && USART2_RxStruct.Buff[6]=='E' && USART2_RxStruct.Buff[7]=='R')
        {
            ConfigurationFlag = 3;
        }
        //���տ���ָ��
        else if(USART2_RxStruct.Buff[0]==0xeb && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len - 1]==0x90)
        {
            //�ͷ��ź���
            ReleaseBinarySemaphore(BinarySemaphore.Module4GControlBinarySemHandle);
        }
        
        
        
        HAL_GPIO_TogglePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin);
        
        HAL_UART_Receive_DMA(&huart2, USART2_RxStruct.Rx_Buff, 200);           //������ʼDMA����

        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}















