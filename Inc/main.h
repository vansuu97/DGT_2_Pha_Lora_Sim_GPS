/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
    uint8_t flag; // enable or not
    uint8_t start_h;
    uint8_t start_m;
    uint8_t end_h;
    uint8_t end_m;
} DGT_OnTime_t;
typedef struct
{
    uint8_t x;
    uint8_t v;
    uint8_t gt;
} DGT_PhaseData_t;
typedef struct
{
    DGT_PhaseData_t phase1;
    DGT_PhaseData_t phase2;
    DGT_PhaseData_t phase3;
} DGT_Phase_t;
typedef struct
{
    DGT_Phase_t normal;
    DGT_Phase_t peak1;
    DGT_Phase_t peak2;
    DGT_OnTime_t BlinkYel1_OnTime;
    DGT_OnTime_t BlinkYel2_OnTime;
    DGT_OnTime_t peak1_OnTime;
    DGT_OnTime_t peak2_OnTime;
    uint8_t Thaco_Blink_flag;
} DGT_Settings_t;
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
#define X2_Pin GPIO_PIN_2
#define X2_GPIO_Port GPIOE
#define V2_Pin GPIO_PIN_3
#define V2_GPIO_Port GPIOE
#define D2_Pin GPIO_PIN_4
#define D2_GPIO_Port GPIOE
#define Xdb2_Pin GPIO_PIN_5
#define Xdb2_GPIO_Port GPIOE
#define Ddb2_Pin GPIO_PIN_6
#define Ddb2_GPIO_Port GPIOE
#define X3_Pin GPIO_PIN_13
#define X3_GPIO_Port GPIOC
#define V3_Pin GPIO_PIN_0
#define V3_GPIO_Port GPIOC
#define D3_Pin GPIO_PIN_2
#define D3_GPIO_Port GPIOC
#define Xdb3_Pin GPIO_PIN_3
#define Xdb3_GPIO_Port GPIOC
#define Ddb3_Pin GPIO_PIN_0
#define Ddb3_GPIO_Port GPIOA
#define ETH_PWR_Pin GPIO_PIN_1
#define ETH_PWR_GPIO_Port GPIOB
#define RS485_DIR_Pin GPIO_PIN_15
#define RS485_DIR_GPIO_Port GPIOB
#define I1_Pin GPIO_PIN_14
#define I1_GPIO_Port GPIOD
#define LED_4_Pin GPIO_PIN_15
#define LED_4_GPIO_Port GPIOD
#define LED_3_Pin GPIO_PIN_6
#define LED_3_GPIO_Port GPIOC
#define LED_2_Pin GPIO_PIN_7
#define LED_2_GPIO_Port GPIOC
#define LED_1_Pin GPIO_PIN_8
#define LED_1_GPIO_Port GPIOC
#define I5_Pin GPIO_PIN_15
#define I5_GPIO_Port GPIOA
#define I4_Pin GPIO_PIN_3
#define I4_GPIO_Port GPIOD
#define I3_Pin GPIO_PIN_4
#define I3_GPIO_Port GPIOD
#define ESP32_IO_Pin GPIO_PIN_7
#define ESP32_IO_GPIO_Port GPIOD
#define I2_Pin GPIO_PIN_3
#define I2_GPIO_Port GPIOB
#define SL_Pin GPIO_PIN_4
#define SL_GPIO_Port GPIOB
#define X1_Pin GPIO_PIN_5
#define X1_GPIO_Port GPIOB
#define V1_Pin GPIO_PIN_8
#define V1_GPIO_Port GPIOB
#define D1_Pin GPIO_PIN_9
#define D1_GPIO_Port GPIOB
#define Xdb1_Pin GPIO_PIN_0
#define Xdb1_GPIO_Port GPIOE
#define Ddb1_Pin GPIO_PIN_1
#define Ddb1_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
