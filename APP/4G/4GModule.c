#include "4gmodule.h"
#include <string.h>
#include <stdlib.h>


#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

USART2_RxStructure USART2_RxStruct;
Modules4G_Structure Modules4G_Struct;

//判断返回信息标志\
1：OK\
2：+GSTATE:6001\
3：
uint8_t ReturnFlag=0;

extern //判断是内部发送还是通过G431接收的
uint8_t InsideOrOutside;

//4G模块配置信息标志\
0:\
1:开始配置\
2:配置成功\
3:信息错误
uint8_t ConfigurationFlag=0;


void modifyString(char* str) 
{
    size_t len = strlen(str);
    if (len < 11) // 如果字符串长度小于11，无法移除前8个字节和最后3个字节
        return; // 可以添加错误处理或其他逻辑

    // 移除前8个字节
    memmove(str, str + 8, len - 8);
    
    // 移除最后3个字节
    str[len - 3] = '\0';
}

void USART2_IDLE_Handler(void)
{
    if (RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //判断是否是空闲中断
    {
        UBaseType_t status_value = taskENTER_CRITICAL_FROM_ISR();
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //清除空闲中断标志（否则会一直不断进入中断）
        HAL_UART_DMAStop(&huart2);

        ///////////////////////
        USART2_RxStruct.Rx_len = 200 - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

        //复制到缓冲区
        memcpy(USART2_RxStruct.Buff, USART2_RxStruct.Rx_Buff, USART2_RxStruct.Rx_len);
        memset(USART2_RxStruct.Rx_Buff, 0, 200);

        
        //4G模块接收信息
        if(USART2_RxStruct.Buff[0]=='O' && USART2_RxStruct.Buff[1]=='K')
        {
            ReturnFlag=1;
            //释放信号量
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        //配置返回
        else if(USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-9]=='1' 
            && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-7]=='0')
        {
            //从外部接收到
            if(InsideOrOutside==1)
            {
                ReturnFlag=4;
            }
            else
            {
                //配置成功
                ReturnFlag=2;
            }
            
            //释放信号量
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        else if(USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-9]=='0' 
            && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len-7]=='0')
        {
            //配置失败
            ReturnFlag=3;
            //释放信号量
            ReleaseBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
        }
        else if(USART2_RxStruct.Buff[0]=='+')
        {
            ReturnFlag=4;
            //释放信号量
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
        //接收控制指令
        else if(USART2_RxStruct.Buff[0]=='{' \
            && USART2_RxStruct.Buff[1]=='\"' \
        && USART2_RxStruct.Buff[USART2_RxStruct.Rx_len - 1]=='}')
        {
            if(USART2_RxStruct.Buff[2]=='n' && USART2_RxStruct.Buff[3]=='1')
            {
                //释放信号量
                ReleaseBinarySemaphore(BinarySemaphore.Module4GControlBinarySemHandle);
            }
            else if(USART2_RxStruct.Buff[2]=='l' \
                && USART2_RxStruct.Buff[3]=='t' \
             && USART2_RxStruct.Buff[4]=='e')
            {
                modifyString((char*)USART2_RxStruct.Buff);
                //释放信号量
                ReleaseBinarySemaphore(BinarySemaphore.Module4GAccPassConfBinarySemHandle);
            }
        }
        
        
        HAL_GPIO_TogglePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin);
        
        HAL_UART_Receive_DMA(&huart2, USART2_RxStruct.Rx_Buff, 200);           //重启开始DMA传输

        taskEXIT_CRITICAL_FROM_ISR(status_value);
    }
}




static int extractNumber(const char* str) {
    // 找到 ':' 的位置
    const char* colonPos = strchr(str, ':');
    if (colonPos == NULL) {
        // 如果找不到 ':'
        return -1; // 返回 -1 表示错误
    }

    // 提取 ':' 后面的子字符串
    const char* numberStr = colonPos + 1;

    // 将提取的子字符串转换为整数
    int number = atoi(numberStr);

    return number;
}
void ParseModules4G(const char* string, int n)
{
    char result[200];
    unsigned char max_length = 200;
    const char* start = string;

    // 定位到第n个逗号
    for (int i = 0; i < n; i++) {
        start = strchr(start, ',');
        if (!start) {
            // 如果逗号不够n个，将结果设为空字符串
            result[0] = '\0';
            return;
        }

        // 移动到下一个字符
        start++;
    }

    // 计算逗号后字符串的长度
    const char* end = strchr(start, ',');
    size_t length = end ? (size_t)(end - start) : strlen(start);

    // 截取字符串并复制到结果
    if (length < max_length - 1) {
        strncpy(result, start, length);
        // 手动添加 null 结尾
        result[length] = '\0';
    }
    else {
        // 目标长度不足，截断字符串
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












