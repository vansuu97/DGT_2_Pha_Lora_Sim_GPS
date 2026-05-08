/**
  ******************************************************************************
  * @file    app_rs485.c
  * @brief   RS485 / Modbus-like slave app for HMI communication.
  ******************************************************************************
  */

#include "app_rs485.h"
#include <string.h>

#define APP_RS485_RX_FRAME_LEN      8U
#define APP_RS485_TX_BUF_LEN        256U

#define APP_RS485_DIR_RX()          HAL_GPIO_WritePin(s_cfg.dir_port, s_cfg.dir_pin, GPIO_PIN_RESET)
#define APP_RS485_DIR_TX()          HAL_GPIO_WritePin(s_cfg.dir_port, s_cfg.dir_pin, GPIO_PIN_SET)

static AppRs485_Config_t s_cfg;

static uint16_t s_regbank[APP_RS485_REG_GROUPS][APP_RS485_REG_PER_GROUP];

static uint8_t  s_rx_byte;
static uint8_t  s_rx_state;
static uint8_t  s_rx_cmd;
static uint8_t  s_input_data[APP_RS485_RX_FRAME_LEN];
static uint8_t  s_valid_data[APP_RS485_RX_FRAME_LEN];
static volatile uint8_t s_frame_ready;

static uint8_t  s_tx_buf[APP_RS485_TX_BUF_LEN];

static uint16_t AppRs485_Crc16(const uint8_t *data, uint16_t num)
{
    uint16_t crc = 0xFFFFU;

    for (uint16_t j = 0U; j < num; j++)
    {
        crc = (uint16_t)(crc ^ data[j]);

        for (uint16_t i = 0U; i < 8U; i++)
        {
            if ((crc & 0x0001U) == 1U)
            {
                crc >>= 1U;
                crc ^= 0xA001U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    /* Giữ đúng format CRC của code cũ: byte cao trước, byte thấp sau. */
    crc = (uint16_t)(((crc << 8U) | (crc >> 8U)) & 0xFFFFU);

    return crc;
}

static void AppRs485_Send(const uint8_t *data, uint16_t length)
{
    if ((s_cfg.huart == NULL) || (s_cfg.dir_port == NULL) || (data == NULL) || (length == 0U))
    {
        return;
    }

    APP_RS485_DIR_TX();
    HAL_UART_Transmit(s_cfg.huart, (uint8_t *)data, length, 0xFFU);
    APP_RS485_DIR_RX();
}

static void AppRs485_AddCrcAndSend(uint16_t payload_len)
{
    uint16_t crc;

    if ((payload_len + 2U) > APP_RS485_TX_BUF_LEN)
    {
        return;
    }

    crc = AppRs485_Crc16(s_tx_buf, payload_len);

    s_tx_buf[payload_len]      = (uint8_t)((crc >> 8U) & 0xFFU);
    s_tx_buf[payload_len + 1U]  = (uint8_t)(crc & 0xFFU);

    AppRs485_Send(s_tx_buf, (uint16_t)(payload_len + 2U));
}

static void AppRs485_HandleReadBits(uint8_t function_code)
{
    uint16_t start_address;
    uint16_t num_reg;
    uint8_t  byte_count;
    uint16_t payload_len;
    uint8_t  group;

    start_address = ((uint16_t)s_valid_data[2] << 8U) | s_valid_data[3];
    num_reg       = ((uint16_t)s_valid_data[4] << 8U) | s_valid_data[5];

    if ((function_code == 0U) || (function_code > APP_RS485_REG_GROUPS) || (num_reg == 0U))
    {
        return;
    }

    byte_count  = (uint8_t)(((num_reg - 1U) / 8U) + 1U);
    payload_len = (uint16_t)(3U + byte_count);

    if ((payload_len + 2U) > APP_RS485_TX_BUF_LEN)
    {
        return;
    }

    group = (uint8_t)(function_code - 1U);

    s_tx_buf[0] = s_cfg.slave_id;
    s_tx_buf[1] = function_code;
    s_tx_buf[2] = byte_count;

    memset(&s_tx_buf[3], 0, byte_count);

    for (uint16_t bit = 0U; bit < num_reg; bit++)
    {
        uint16_t reg_index = start_address + bit;
        uint8_t  byte_pos  = (uint8_t)(bit / 8U);
        uint8_t  bit_pos   = (uint8_t)(bit % 8U);

        if (reg_index < APP_RS485_REG_PER_GROUP)
        {
            s_tx_buf[3U + byte_pos] |= (uint8_t)((s_regbank[group][reg_index] & 0x01U) << bit_pos);
        }
    }

    AppRs485_AddCrcAndSend(payload_len);
}

static void AppRs485_HandleReadWords(uint8_t function_code)
{
    uint16_t start_address;
    uint16_t num_reg;
    uint16_t byte_count;
    uint16_t payload_len;
    uint8_t  group;

    start_address = ((uint16_t)s_valid_data[2] << 8U) | s_valid_data[3];
    num_reg       = ((uint16_t)s_valid_data[4] << 8U) | s_valid_data[5];

    if (function_code == APP_RS485_FC_READ_INPUT_REGISTERS)
    {
        /* Giữ logic mới nhất trong main.c anh gửi: FC04 trả thêm 5 word. */
        num_reg = (uint16_t)(num_reg + 5U);
    }

    if ((function_code == 0U) || (function_code > APP_RS485_REG_GROUPS) || (num_reg == 0U))
    {
        return;
    }

    byte_count  = (uint16_t)(num_reg * 2U);
    payload_len = (uint16_t)(3U + byte_count);

    if ((payload_len + 2U) > APP_RS485_TX_BUF_LEN)
    {
        return;
    }

    group = (uint8_t)(function_code - 1U);

    s_tx_buf[0] = s_cfg.slave_id;
    s_tx_buf[1] = function_code;
    s_tx_buf[2] = (uint8_t)byte_count;

    for (uint16_t i = 0U; i < num_reg; i++)
    {
        uint16_t value = 0U;
        uint16_t reg_index = start_address + i;

        if (reg_index < APP_RS485_REG_PER_GROUP)
        {
            value = s_regbank[group][reg_index];
        }

        s_tx_buf[3U + (i * 2U)]      = (uint8_t)((value >> 8U) & 0xFFU);
        s_tx_buf[3U + (i * 2U) + 1U] = (uint8_t)(value & 0xFFU);
    }

    AppRs485_AddCrcAndSend(payload_len);
}

static void AppRs485_HandleWriteSingleCoil(void)
{
    uint16_t address;
    uint16_t value;

    address = ((uint16_t)s_valid_data[2] << 8U) | s_valid_data[3];
    value   = ((uint16_t)s_valid_data[4] << 8U) | s_valid_data[5];

    if (address < APP_RS485_REG_PER_GROUP)
    {
        s_regbank[0][address] = (value == 0xFF00U) ? 1U : 0U;
    }

    memcpy(s_tx_buf, s_valid_data, 6U);
    AppRs485_AddCrcAndSend(6U);
}

static void AppRs485_HandleWriteSingleRegister(void)
{
    uint16_t address;
    uint16_t value;

    address = ((uint16_t)s_valid_data[2] << 8U) | s_valid_data[3];
    value   = ((uint16_t)s_valid_data[4] << 8U) | s_valid_data[5];

    if (address < APP_RS485_REG_PER_GROUP)
    {
        s_regbank[2][address] = value;
    }

    memcpy(s_tx_buf, s_valid_data, 6U);
    AppRs485_AddCrcAndSend(6U);
}

void AppRs485_Init(const AppRs485_Config_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    s_cfg = *cfg;

    if (s_cfg.slave_id == 0U)
    {
        s_cfg.slave_id = s_cfg.slave_id;
    }

    memset(s_regbank, 0, sizeof(s_regbank));
    memset(s_input_data, 0, sizeof(s_input_data));
    memset(s_valid_data, 0, sizeof(s_valid_data));
    memset(s_tx_buf, 0, sizeof(s_tx_buf));

    s_rx_state = 0U;
    s_rx_cmd = 0U;
    s_frame_ready = 0U;

    APP_RS485_DIR_RX();

    __HAL_UART_ENABLE_IT(s_cfg.huart, UART_IT_RXNE);
    HAL_UART_Receive_IT(s_cfg.huart, &s_rx_byte, 1U);
}

void AppRs485_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint16_t receive_crc;
    uint16_t frame_crc;

    if ((s_cfg.huart == NULL) || (huart == NULL) || (huart->Instance != s_cfg.huart->Instance))
    {
        return;
    }

    switch (s_rx_state)
    {
        case 0U:
            if (s_rx_byte == s_cfg.slave_id)
            {
                s_input_data[0] = s_rx_byte;
                s_rx_state = 1U;
            }
            else
            {
                s_rx_state = 0U;
                s_frame_ready = 0U;
            }
            break;

        case 1U:
            s_rx_cmd = s_rx_byte;
            s_input_data[1] = s_rx_byte;
            s_rx_state = 2U;
            break;

        case 2U:
        case 3U:
        case 4U:
        case 5U:
            s_input_data[s_rx_state] = s_rx_byte;
            s_rx_state++;
            break;

        case 6U:
            s_input_data[6] = s_rx_byte;
            s_rx_state = 7U;
            break;

        case 7U:
            s_input_data[7] = s_rx_byte;

            receive_crc = AppRs485_Crc16(s_input_data, 6U);
            frame_crc   = ((uint16_t)s_input_data[6] << 8U) | s_input_data[7];

            if (receive_crc == frame_crc)
            {
                memcpy(s_valid_data, s_input_data, 6U);
                s_frame_ready = 1U;
            }
            else
            {
                memset(s_valid_data, 0, sizeof(s_valid_data));
                s_frame_ready = 0U;
            }

            s_rx_state = 0U;
            break;

        default:
            s_rx_state = 0U;
            s_frame_ready = 0U;
            break;
    }

    HAL_UART_Receive_IT(s_cfg.huart, &s_rx_byte, 1U);
}

void AppRs485_Task(void)
{
    uint8_t cmd;

    if (s_frame_ready == 0U)
    {
        return;
    }

    __disable_irq();
    s_frame_ready = 0U;
    cmd = s_rx_cmd;
    __enable_irq();

    memset(s_tx_buf, 0, sizeof(s_tx_buf));

    switch (cmd)
    {
        case APP_RS485_FC_READ_COILS:
        case APP_RS485_FC_READ_DISCRETE_INPUTS:
            AppRs485_HandleReadBits(cmd);
            break;

        case APP_RS485_FC_READ_HOLDING_REGISTERS:
        case APP_RS485_FC_READ_INPUT_REGISTERS:
            AppRs485_HandleReadWords(cmd);
            break;

        case APP_RS485_FC_WRITE_SINGLE_COIL:
            AppRs485_HandleWriteSingleCoil();
            break;

        case APP_RS485_FC_WRITE_SINGLE_REGISTER:
            AppRs485_HandleWriteSingleRegister();
            break;

        default:
            break;
    }
}

uint16_t AppRs485_GetReg(uint8_t group, uint8_t address)
{
    if ((group >= APP_RS485_REG_GROUPS) || (address >= APP_RS485_REG_PER_GROUP))
    {
        return 0U;
    }

    return s_regbank[group][address];
}

void AppRs485_SetReg(uint8_t group, uint8_t address, uint16_t value)
{
    if ((group >= APP_RS485_REG_GROUPS) || (address >= APP_RS485_REG_PER_GROUP))
    {
        return;
    }

    s_regbank[group][address] = value;
}

uint16_t (*AppRs485_GetRegBank(void))[APP_RS485_REG_PER_GROUP]
{
    return s_regbank;
}
