/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "g431_usart.h"
#include "4gmodule.h"
#include "rtk.h"
#include <string.h>
#include "nmea.h"
#include "iwdg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern //判断指令类型
uint8_t OrderType;
extern //判断返回信息标志
uint8_t ReturnFlag;
extern //判断是内部发送还是通过G431接收的
uint8_t InsideOrOutside;



extern GPRMC_Structure GPRMCH_Struct;
/* USER CODE END Variables */
osThreadId G431_TaskHandle;
osThreadId Module4G_TaskHandle;
osThreadId RTK_TaskHandle;
osTimerId Timer01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartG431_Task(void const * argument);
void StartModule4G_Task(void const * argument);
void StartRTK_Task(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of Timer01 */
  osTimerDef(Timer01, Callback01);
  Timer01Handle = osTimerCreate(osTimer(Timer01), osTimerOnce, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of G431_Task */
  osThreadDef(G431_Task, StartG431_Task, osPriorityRealtime, 0, 512);
  G431_TaskHandle = osThreadCreate(osThread(G431_Task), NULL);

  /* definition and creation of Module4G_Task */
  osThreadDef(Module4G_Task, StartModule4G_Task, osPriorityBelowNormal, 0, 256);
  Module4G_TaskHandle = osThreadCreate(osThread(Module4G_Task), NULL);

  /* definition and creation of RTK_Task */
  osThreadDef(RTK_Task, StartRTK_Task, osPriorityIdle, 0, 512);
  RTK_TaskHandle = osThreadCreate(osThread(RTK_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
//    //挂起自动上报任务
//    osThreadSuspend(RTK_TaskHandle);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartG431_Task */
/**
  * @brief  Function implementing the G431_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartG431_Task */
void StartG431_Task(void const * argument)
{
  /* USER CODE BEGIN StartG431_Task */
//    hiwdg.Instance = IWDG;
//  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
//  hiwdg.Init.Window = 1000;
//  hiwdg.Init.Reload = 1000;
//  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
//  {
//    Error_Handler();
//  }
    /* Infinite loop */
    for (;;)
    {
//        HAL_IWDG_Refresh(&hiwdg);
        if (myOSOK == AcquireBinarySemaphore(BinarySemaphore.G431_Rx4GBinarySemHandle))
        {
            DeleteBinarySemaphore(BinarySemaphore.G431_Rx4GBinarySemHandle);
            taskENTER_CRITICAL();
            //向4G模块发送指令
            HAL_UART_Transmit_DMA(&huart2, USART1_RxStruct.Buff, USART1_RxStruct.Rx_len-2);
            
            taskEXIT_CRITICAL();
        }
        if (myOSOK == AcquireBinarySemaphore(BinarySemaphore.G431_RxRTKBinarySemHandle))
        {
            DeleteBinarySemaphore(BinarySemaphore.G431_RxRTKBinarySemHandle);
            taskENTER_CRITICAL();
            //向RTK模块发送指令
            HAL_UART_Transmit_DMA(&huart3, USART1_RxStruct.Buff, USART1_RxStruct.Rx_len);
            
            taskEXIT_CRITICAL();
        }
        

        osDelay(1);
    }
  /* USER CODE END StartG431_Task */
}

/* USER CODE BEGIN Header_StartModule4G_Task */
/**
* @brief Function implementing the Module4G_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartModule4G_Task */
void StartModule4G_Task(void const * argument)
{
  /* USER CODE BEGIN StartModule4G_Task */
    /* Infinite loop */
    for (;;)
    {
        if (myOSOK == AcquireBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle))
        {
            DeleteBinarySemaphore(BinarySemaphore.Module4G_RxBinarySemHandle);
            taskENTER_CRITICAL();

            //OK信息
            if (ReturnFlag == 1)
            {
                //配置完成后需要等待一段时间
                osTimerStart(Timer01Handle, 10000);//1000代表回调函数回调周期
            }
            //+GSTATE:6001（确认配置成功）
            else if (ReturnFlag == 2)
            {
                //处理返回信息
                uint8_t buff[10] = { 0xAC,0x4F,0x4B,0x90 };
                HAL_UART_Transmit(&huart1, buff, 4, 100);
            }
            else if (ReturnFlag == 3)
            {
                //处理返回信息
                uint8_t buff[10] = { 0xAC,'E','R','R',0x90 };
                HAL_UART_Transmit(&huart1, buff, 5, 100);
            }
            else if (ReturnFlag == 4)
            {
                //处理返回信息
                HAL_UART_Transmit(&huart1, USART2_RxStruct.Buff, USART2_RxStruct.Rx_len, 1000);
            }

            
            taskEXIT_CRITICAL();
        }
//        if (myOSOK == AcquireBinarySemaphore(BinarySemaphore.Module4GControlBinarySemHandle))
        {
            DeleteBinarySemaphore(BinarySemaphore.Module4GControlBinarySemHandle);
            DeleteBinarySemaphore(BinarySemaphore.RTK_RxRMCBinarySemHandle);
            taskENTER_CRITICAL();
            
            for (int i = 0; i < 12; i++)
            {
                ParseModules4G((char*)USART2_RxStruct.Buff, i);
            }
            
            
            Modules4G_Struct.TxBuff[0] = 0xeb;
            Modules4G_Struct.TxBuff[1] = Modules4G_Struct.ControlByte;
            Modules4G_Struct.TxBuff[2] = Modules4G_Struct.DirByte;
            Modules4G_Struct.TxBuff[3] = Modules4G_Struct.Speed[0];
            Modules4G_Struct.TxBuff[4] = Modules4G_Struct.Speed[1];
            Modules4G_Struct.TxBuff[5] = Modules4G_Struct.Speed[2];
            Modules4G_Struct.TxBuff[6] = Modules4G_Struct.Speed[3];
            Modules4G_Struct.TxBuff[7] = 0x90;
            
            //向控制板（大板）发送控制指令
            HAL_UART_Transmit(&huart1, Modules4G_Struct.TxBuff, 8,1000);
            
            taskEXIT_CRITICAL();
        }
        

        osDelay(500);
    }
  /* USER CODE END StartModule4G_Task */
}

/* USER CODE BEGIN Header_StartRTK_Task */
/**
* @brief Function implementing the RTK_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRTK_Task */
void StartRTK_Task(void const * argument)
{
  /* USER CODE BEGIN StartRTK_Task */
    /* Infinite loop */
    for (;;)
    {
        if (myOSOK == AcquireBinarySemaphore(BinarySemaphore.RTK_RxRMCBinarySemHandle))
        {
            DeleteBinarySemaphore(BinarySemaphore.RTK_RxRMCBinarySemHandle);
            
            HAL_GPIO_TogglePin(RTK_LED_GPIO_Port,RTK_LED_Pin);
            
            char info1[100];
            char info2[100];
            char info3[100];

            char outinfo1[100];
            char outinfo2[100];
            char outinfo3[100];
            
            const char* delimiter = "\r\n";
            
//            HAL_UART_Transmit(&huart1, USART3_RxStruct.Buff, strlen(USART3_RxStruct.Buff),1000);
            separateString((char*)USART3_RxStruct.Buff, delimiter, info1, info2, info3);
            processInfo((uint8_t*)info1, (uint8_t*)info2, (uint8_t*)info3, (uint8_t*)outinfo1, (uint8_t*)outinfo2, (uint8_t*)outinfo3, 100);
            
            for(uint8_t i=0;i<=14;i++)
            {
                ParseGPRMC((char*)outinfo1, i);
                ParseGPGGA((char*)outinfo2, i);
                ParseGPRMCH((char*)outinfo3, i);
            }
                //ParseGPRMC((char*)"$GNRMC,023454.00,A,2623.01151432,N,10636.51528076,E,0.117,202.9,170124,2.0,W,A,C*53",i);
            
            taskENTER_CRITICAL();
            
            copyRMCData();//大板
            
            char str[100];
            #include <string.h>
            #include <stdio.h>
            sprintf(str,"%d-%lf-%lf\r\n",GPRMCH_Struct.Sec,GPRMCH_Struct.Longitude,GPRMCH_Struct.Latitude);
//            HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str),1000);
            
            HAL_UART_Transmit(&huart1, OutGNxxxData, 78,1000);

//            BluetoothCopyRMCData();//蓝牙
//            HAL_UART_Transmit_DMA(&huart1, OutGNRMCData, 27);
            taskEXIT_CRITICAL();
        }
        osDelay(1);
    }
  /* USER CODE END StartRTK_Task */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */
    taskENTER_CRITICAL();
    InsideOrOutside = 0;
    //发送查询命令
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT*GSTATE?\r\n", strlen("AT*GSTATE?\r\n"), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT*GSTATE?\r\n", strlen("AT*GSTATE?\r\n"), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT*GSTATE?\r\n", strlen("AT*GSTATE?\r\n"), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT*GSTATE?\r\n", strlen("AT*GSTATE?\r\n"), 1000);
    HAL_UART_Transmit(&huart2, (uint8_t*)"AT*GSTATE?\r\n", strlen("AT*GSTATE?\r\n"), 1000);
    taskEXIT_CRITICAL();
  /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

