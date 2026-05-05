/**
  ******************************************************************************
  * @file    app_rs485.h
  * @brief   RS485 / Modbus-like slave app for HMI communication.
  ******************************************************************************
  */

#ifndef APP_RS485_H
#define APP_RS485_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define APP_RS485_REG_GROUPS        4U
#define APP_RS485_REG_PER_GROUP     255U

#define APP_RS485_DEFAULT_SLAVE_ID  10U

#define APP_RS485_FC_READ_COILS               1U
#define APP_RS485_FC_READ_DISCRETE_INPUTS     2U
#define APP_RS485_FC_READ_HOLDING_REGISTERS   3U
#define APP_RS485_FC_READ_INPUT_REGISTERS     4U
#define APP_RS485_FC_WRITE_SINGLE_COIL        5U
#define APP_RS485_FC_WRITE_SINGLE_REGISTER    6U

typedef struct
{
    UART_HandleTypeDef *huart;
    GPIO_TypeDef       *dir_port;
    uint16_t            dir_pin;
    uint8_t             slave_id;
} AppRs485_Config_t;

void AppRs485_Init(const AppRs485_Config_t *cfg);
void AppRs485_Task(void);
void AppRs485_RxCpltCallback(UART_HandleTypeDef *huart);

uint16_t  AppRs485_GetReg(uint8_t group, uint8_t address);
void      AppRs485_SetReg(uint8_t group, uint8_t address, uint16_t value);
uint16_t (*AppRs485_GetRegBank(void))[APP_RS485_REG_PER_GROUP];

#ifdef __cplusplus
}
#endif

#endif /* APP_RS485_H */
