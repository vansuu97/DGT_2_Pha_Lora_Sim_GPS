#ifndef BOARD_IO_H
#define BOARD_IO_H

#include "main.h"

	#define SL_0		HAL_GPIO_WritePin(GPIOB, SL_Pin,0)			// so lui
	#define SL_1		HAL_GPIO_WritePin(GPIOB, SL_Pin,1)			// so lui

	#define LED1_0		HAL_GPIO_WritePin(GPIOC, LED_1_Pin, 1)
	#define LED2_0		HAL_GPIO_WritePin(GPIOC, LED_2_Pin, 1)
	#define LED3_0		HAL_GPIO_WritePin(GPIOC, LED_3_Pin, 1)
	#define LED4_0		HAL_GPIO_WritePin(GPIOD, LED_4_Pin, 1)
	#define LED1_1		HAL_GPIO_WritePin(GPIOC, LED_1_Pin, 0)
	#define LED2_1		HAL_GPIO_WritePin(GPIOC, LED_2_Pin, 0)
	#define LED3_1		HAL_GPIO_WritePin(GPIOC, LED_3_Pin, 0)
	#define LED4_1		HAL_GPIO_WritePin(GPIOD, LED_4_Pin, 0)
	
	#define X1_0		HAL_GPIO_WritePin(GPIOB, X1_Pin,0)
	#define X1_1		HAL_GPIO_WritePin(GPIOB, X1_Pin,1)
	#define V1_0		HAL_GPIO_WritePin(GPIOB, V1_Pin,0)
	#define V1_1		HAL_GPIO_WritePin(GPIOB, V1_Pin,1)
	#define D1_0		HAL_GPIO_WritePin(GPIOB, D1_Pin,0)
	#define D1_1		HAL_GPIO_WritePin(GPIOB, D1_Pin,1)
	#define Xdb1_0		HAL_GPIO_WritePin(GPIOE, Xdb1_Pin,0)
	#define Xdb1_1		HAL_GPIO_WritePin(GPIOE, Xdb1_Pin,1)
	#define Ddb1_0		HAL_GPIO_WritePin(GPIOE, Ddb1_Pin,0)
	#define Ddb1_1		HAL_GPIO_WritePin(GPIOE, Ddb1_Pin,1)
	
	#define X2_0		HAL_GPIO_WritePin(GPIOE, X2_Pin,0)
	#define X2_1		HAL_GPIO_WritePin(GPIOE, X2_Pin,1)
	#define V2_0		HAL_GPIO_WritePin(GPIOE, V2_Pin,0)
	#define V2_1		HAL_GPIO_WritePin(GPIOE, V2_Pin,1)
	#define D2_0		HAL_GPIO_WritePin(GPIOE, D2_Pin,0)
	#define D2_1		HAL_GPIO_WritePin(GPIOE, D2_Pin,1)
	#define Xdb2_0		HAL_GPIO_WritePin(GPIOE, Xdb2_Pin,0)
	#define Xdb2_1		HAL_GPIO_WritePin(GPIOE, Xdb2_Pin,1)
	#define Ddb2_0		HAL_GPIO_WritePin(GPIOE, Ddb2_Pin,0)
	#define Ddb2_1		HAL_GPIO_WritePin(GPIOE, Ddb2_Pin,1)
	
	#define X3_0		HAL_GPIO_WritePin(GPIOC, X3_Pin,0)
	#define X3_1		HAL_GPIO_WritePin(GPIOC, X3_Pin,1)
	#define V3_0		HAL_GPIO_WritePin(GPIOC, V3_Pin,0)
	#define V3_1		HAL_GPIO_WritePin(GPIOC, V3_Pin,1)
	#define D3_0		HAL_GPIO_WritePin(GPIOC, D3_Pin,0)
	#define D3_1		HAL_GPIO_WritePin(GPIOC, D3_Pin,1)
	#define Xdb3_0		HAL_GPIO_WritePin(GPIOC, Xdb3_Pin,0)
	#define Xdb3_1		HAL_GPIO_WritePin(GPIOC, Xdb3_Pin,1)
	#define Ddb3_0		HAL_GPIO_WritePin(GPIOA, Ddb3_Pin,0)
	#define Ddb3_1		HAL_GPIO_WritePin(GPIOA, Ddb3_Pin,1)
	
	#define ETH_PWR_0	HAL_GPIO_WritePin(GPIOB, ETH_PWR_Pin,0)
	#define ETH_PWR_1	HAL_GPIO_WritePin(GPIOB, ETH_PWR_Pin,1)
	
	#define RS485_DIR_0	HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, 0)
	#define RS485_DIR_1	HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, 1)

#endif