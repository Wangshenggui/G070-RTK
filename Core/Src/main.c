/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "dma.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "g431_usart.h"
#include "4gmodule.h"
#include "rtk.h"
#include "flash.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "base64.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//����ģ���ֵ�ź���
BinarySemaphoreStructure BinarySemaphore= {0};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern //4Gģ��������Ϣ��־\
0:\
1:��ʼ����\
2:���óɹ�\
3:��Ϣ����
uint8_t ConfigurationFlag;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
//    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
//    HAL_UART_Receive_DMA(&huart1, USART1_RxStruct.Rx_Buff, 200);

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart2, USART2_RxStruct.Rx_Buff, 200);

    __HAL_UART_ENABLE_IT(&huart3, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart3, USART3_RxStruct.Rx_Buff, Rx_LENG);
    
    
    HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_SET);
    
    
    
    uint8_t str[100];
    uint8_t temp[100];
    uint8_t base64[100];
    uint8_t UpperString[300];
    
    sprintf((char*)str,"%s","Init\r\n");
    HAL_UART_Transmit(&huart1, str, strlen((char*)str),1000);
    
    
    //������д��
    //USER_FLASH_Write((uint8_t*)"$CORS,1,4,9,cedr17622,hm0dcvnt\n");
    
    //��ȡ���ݲ�����
    sprintf((char*)temp,"%s",USER_FLASH_Read());
    ParseCORS((char*)temp,1);
    ParseCORS((char*)temp,2);
    ParseCORS((char*)temp,3);
    ParseCORS((char*)temp,4);
    ParseCORS((char*)temp,5);
    
    //�������ݼ���
    sprintf((char*)temp,"%s:%s",CORS_Struct.Account,CORS_Struct.Password);
    base64_encode((uint8_t*)temp,(char*)base64);
    sprintf((char*)str,"%s\r\n",base64);
    
    //�õ����ݲ����
    sprintf((char*)UpperString,"$CORS,%c,%c,%d,%s",
        CORS_Struct.DNS,CORS_Struct.Port,CORS_Struct.MountPoint,
    str);
    HAL_UART_Transmit(&huart1, UpperString, strlen((char*)UpperString),1000);
    
    HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_SET);
    while(1)
    {
        static uint8_t bbb = 0;
        if(bbb ++ == 18)
        {
            bbb = 0;
            goto START;
        }
        
        HAL_Delay(1000);
        
        switch(ConfigurationFlag)
        {
            case(0):
                HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_RESET);
            
                HAL_UART_Transmit(&huart2, UpperString, strlen((char*)UpperString),1000);
                break;
            case(1)://��ʼ����
                HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_SET);
                HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_RESET);
            
                HAL_UART_Transmit(&huart2, UpperString, strlen((char*)UpperString),1000);
                break;
            case(2)://���óɹ�
                HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_SET);
            
                HAL_UART_Transmit(&huart2, UpperString, strlen((char*)UpperString),1000);
                goto START;
            case(3)://��Ϣ����
                HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_SET);
                HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_SET);
            
                HAL_UART_Transmit(&huart2, UpperString, strlen((char*)UpperString),1000);
                break;
        }
        sprintf((char*)str,">>%d\r\n",ConfigurationFlag);
        HAL_UART_Transmit(&huart1, str, strlen((char*)str),1000);
        ConfigurationFlag = 0;
    }
    
    START:
    HAL_GPIO_WritePin(RTK_LED_GPIO_Port,RTK_LED_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(Module4G_LED_GPIO_Port,Module4G_LED_Pin,GPIO_PIN_SET);
    
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, USART1_RxStruct.Rx_Buff, 200);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
        {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
        {
        }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
