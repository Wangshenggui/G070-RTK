#include "4gmodule.h"
#include <string.h>
#include <stdlib.h>


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

USART2_RxStructure USART2_RxStruct;
Modules4G_Structure Modules4G_Struct;

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


void modifyString(char* str) 
{
    size_t len = strlen(str);
    if (len < 11) // ����ַ�������С��11���޷��Ƴ�ǰ8���ֽں����3���ֽ�
        return; // ������Ӵ�����������߼�

    // �Ƴ�ǰ8���ֽ�
    memmove(str, str + 8, len - 8);
    
    // �Ƴ����3���ֽ�
    str[len - 3] = '\0';
}

void USART2_IDLE_Handler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
    {
        UBaseType_t status_value = taskENTER_CRITICAL_FROM_ISR();
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
        HAL_UART_DMAStop(&huart2);

        ///////////////////////
        USART2_RxStruct.Rx_len = 200 - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

        //���Ƶ�������
        memcpy(USART2_RxStruct.Buff, USART2_RxStruct.Rx_Buff, USART2_RxStruct.Rx_len);
        memset(USART2_RxStruct.Rx_Buff, 0, 200);

        
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
        else if(USART2_RxStruct.Buff[0]=='{' \
            && USART2_RxStruct.Buff[1]=='\"' \
        && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len - 1]=='}')
        {
            if(USART2_RxStruct.Buff[2]=='n' && USART2_RxStruct.Buff[3]=='1')
            {
                //�ͷ��ź���
                ReleaseBinarySemaphore(BinarySemaphore.Module4GControlBinarySemHandle);
            }
            else if(USART2_RxStruct.Buff[2]=='l' \
                && USART2_RxStruct.Buff[3]=='t' \
             && USART2_RxStruct.Buff[4]=='e')
            {
                modifyString((char*)USART2_RxStruct.Buff);
                //�ͷ��ź���
                ReleaseBinarySemaphore(BinarySemaphore.Module4GAccPassConfBinarySemHandle);
            }
        }
        
        
        HAL_GPIO_TogglePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin);
        
        HAL_UART_Receive_DMA(&huart2, USART2_RxStruct.Rx_Buff, 200);           //������ʼDMA����

        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}




static int extractNumber(const char* str) {
    // �ҵ� ':' ��λ��
    const char* colonPos = strchr(str, ':');
    if (colonPos == NULL) {
        // ����Ҳ��� ':'
        return -1; // ���� -1 ��ʾ����
    }

    // ��ȡ ':' ��������ַ���
    const char* numberStr = colonPos + 1;

    // ����ȡ�����ַ���ת��Ϊ����
    int number = atoi(numberStr);

    return number;
}
void ParseModules4G(const char* string, int n)
{
    char result[200];
    unsigned char max_length = 200;
    const char* start = string;

    // ��λ����n������
    for (int i = 0; i < n; i++) {
        start = strchr(start, ',');
        if (!start) {
            // ������Ų���n�����������Ϊ���ַ���
            result[0] = '\0';
            return;
        }

        // �ƶ�����һ���ַ�
        start++;
    }

    // ���㶺�ź��ַ����ĳ���
    const char* end = strchr(start, ',');
    size_t length = end ? (size_t)(end - start) : strlen(start);

    // ��ȡ�ַ��������Ƶ����
    if (length < max_length - 1) {
        strncpy(result, start, length);
        // �ֶ���� null ��β
        result[length] = '\0';
    }
    else {
        // Ŀ�곤�Ȳ��㣬�ض��ַ���
        strncpy(result, start, max_length - 1);
        result[max_length - 1] = '\0';
    }

    if (n >= 0 && n <= 3)
    {
        int CBit = extractNumber(result);
        Modules4G_Struct.ControlByte = \
            (CBit == 1) ? (Modules4G_Struct.ControlByte |= (CBit << n)) : \
            (Modules4G_Struct.ControlByte &= ~(CBit << n));
    }
    else if (n >= 4 && n <= 7)
    {
        int DBit = extractNumber(result);
        Modules4G_Struct.DirByte = \
            (DBit == 1) ? (Modules4G_Struct.DirByte |= (DBit << (n - 4))) : \
            (Modules4G_Struct.DirByte &= ~(DBit << (n - 4)));
    }
    else if (n >= 8 && n <= 11)
    {
        Modules4G_Struct.Speed[n - 8] = extractNumber(result);
    }
}












