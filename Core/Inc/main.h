/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
#define myOSOK  1
typedef struct
{
    bool G431_Rx4GBinarySemHandle;
    bool G431_RxRTKBinarySemHandle;
    bool Module4G_RxBinarySemHandle;
    bool RTK_RxRMCBinarySemHandle;
    bool Module4GControlBinarySemHandle;
}BinarySemaphoreStructure;
extern BinarySemaphoreStructure BinarySemaphore;

//释放信号量
#define ReleaseBinarySemaphore(x)       x=1
//获取信号量
#define AcquireBinarySemaphore(x)       x
//删除信号量
#define DeleteBinarySemaphore(x)        x=0
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Module4G_LED_Pin GPIO_PIN_15
#define Module4G_LED_GPIO_Port GPIOA
#define RTK_LED_Pin GPIO_PIN_1
#define RTK_LED_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
