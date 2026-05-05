#include "app_I2CvsESP32.h"
#include <stddef.h>
#include <string.h>

/*
 * I2C frame app for STM32 <-> ESP32.
 *
 * This module sends 3 separated packages:
 * 1. REALTIME        : current date/time, voltage/current, blink-yellow-1 time window.
 * 2. NORMAL_SETTING  : normal traffic-light timing values, blink-yellow-2 time window.
 * 3. PEAK_SETTING    : peak-time window and peak traffic-light timing values.
 *
 * Frame format: CMD + DATA + Counter + CRC8.
 */

static const uint8_t CRC8_TABLE[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
    0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
    0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
    0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
    0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
    0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
    0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
    0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
    0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
    0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
    0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
    0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
    0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
    0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
    0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
    0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
    0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
    0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
    0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
    0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
    0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
    0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
    0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
    0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
    0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
    0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
    0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
    0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
    0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
    0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
    0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
    0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

static I2C_HandleTypeDef *s_hi2c = NULL;
static uint16_t s_slave_addr_7bit = APP_I2CVSESP32_ADDR_7BIT;
static uint32_t s_timeout_ms = 100U;

static uint8_t s_tx_realtime_frame[APP_I2CVSESP32_REALTIME_FRAME_SIZE];
static uint8_t s_tx_normal_frame[APP_I2CVSESP32_NORMAL_FRAME_SIZE];
static uint8_t s_tx_peak_frame[APP_I2CVSESP32_PEAK_FRAME_SIZE];

static uint8_t s_frame_counter = 0U;

static uint8_t crc8_cal(const uint8_t *data, size_t length)
{
    uint8_t crc = 0x00U;
    size_t i;

    for (i = 0U; i < length; ++i)
    {
        crc = CRC8_TABLE[crc ^ data[i]];
    }

    return crc;
}

static uint8_t AppI2CvsESP32_NextCounter(void)
{
    s_frame_counter++;

    if (s_frame_counter == 0U)
    {
        s_frame_counter = 1U;
    }

    return s_frame_counter;
}

static HAL_StatusTypeDef AppI2CvsESP32_Transmit(const uint8_t *frame, uint16_t length)
{
    if ((s_hi2c == NULL) || (frame == NULL))
    {
        return HAL_ERROR;
    }

    return HAL_I2C_Master_Transmit(s_hi2c,
                                   (uint16_t)(s_slave_addr_7bit << 1U),
                                   (uint8_t *)frame,
                                   length,
                                   s_timeout_ms);
}

void AppI2CvsESP32_Init(const AppI2CvsESP32_Config_t *config)
{
    if (config == NULL)
    {
        return;
    }

    s_hi2c = config->hi2c;
    s_slave_addr_7bit = config->slave_addr_7bit;
    s_timeout_ms = config->timeout_ms;

    (void)memset(s_tx_realtime_frame, 0, sizeof(s_tx_realtime_frame));
    (void)memset(s_tx_normal_frame, 0, sizeof(s_tx_normal_frame));
    (void)memset(s_tx_peak_frame, 0, sizeof(s_tx_peak_frame));
}

void AppI2CvsESP32_SetCounter(uint8_t counter)
{
    s_frame_counter = counter;
}

uint8_t AppI2CvsESP32_GetCounter(void)
{
    return s_frame_counter;
}

void AppI2CvsESP32_BuildRealtime(uint8_t frame[APP_I2CVSESP32_REALTIME_FRAME_SIZE],
                                 const AppI2CvsESP32_RealtimePayload_t *payload,
                                 uint8_t counter)
{
    if ((frame == NULL) || (payload == NULL))
    {
        return;
    }

    frame[0]  = APP_I2CVSESP32_CMD_REALTIME;
    frame[1]  = payload->date;
    frame[2]  = payload->month;
    frame[3]  = payload->year;
    frame[4]  = payload->hour;
    frame[5]  = payload->minute;
    frame[6]  = payload->second;
    frame[7]  = payload->voltage1;
    frame[8]  = payload->voltage2;
    frame[9]  = payload->current1;
    frame[10] = payload->current2;
    frame[11] = payload->begin_hour1;
    frame[12] = payload->begin_min1;
    frame[13] = payload->end_hour1;
    frame[14] = payload->end_min1;
    frame[APP_I2CVSESP32_REALTIME_COUNTER_INDEX] = counter;
    frame[APP_I2CVSESP32_REALTIME_CRC_INDEX] = crc8_cal(frame, APP_I2CVSESP32_REALTIME_CRC_INDEX);
}

void AppI2CvsESP32_BuildNormalSetting(uint8_t frame[APP_I2CVSESP32_NORMAL_FRAME_SIZE],
                                      const AppI2CvsESP32_NormalPayload_t *payload,
                                      uint8_t counter)
{
    if ((frame == NULL) || (payload == NULL))
    {
        return;
    }

    frame[0]  = APP_I2CVSESP32_CMD_NORMAL_SETTING;
    frame[1]  = payload->x1;
    frame[2]  = payload->v1;
    frame[3]  = payload->gt1;
    frame[4]  = payload->x2;
    frame[5]  = payload->v2;
    frame[6]  = payload->gt2;
    frame[7]  = payload->x3;
    frame[8]  = payload->v3;
    frame[9]  = payload->gt3;
    frame[10] = payload->begin_hour2;
    frame[11] = payload->begin_min2;
    frame[12] = payload->end_hour2;
    frame[13] = payload->end_min2;
    frame[APP_I2CVSESP32_NORMAL_COUNTER_INDEX] = counter;
    frame[APP_I2CVSESP32_NORMAL_CRC_INDEX] = crc8_cal(frame, APP_I2CVSESP32_NORMAL_CRC_INDEX);
}

void AppI2CvsESP32_BuildPeakSetting(uint8_t frame[APP_I2CVSESP32_PEAK_FRAME_SIZE],
                                    const AppI2CvsESP32_PeakPayload_t *payload,
                                    uint8_t counter)
{
    if ((frame == NULL) || (payload == NULL))
    {
        return;
    }

    frame[0]  = APP_I2CVSESP32_CMD_PEAK_SETTING;
    frame[1]  = payload->begin_hour3;
    frame[2]  = payload->begin_min3;
    frame[3]  = payload->end_hour3;
    frame[4]  = payload->end_min3;
    frame[5]  = payload->peak_x1;
    frame[6]  = payload->peak_v1;
    frame[7]  = payload->peak_gt1;
    frame[8]  = payload->peak_x2;
    frame[9]  = payload->peak_v2;
    frame[10] = payload->peak_gt2;
    frame[11] = payload->peak_x3;
    frame[12] = payload->peak_v3;
    frame[13] = payload->peak_gt3;
    frame[APP_I2CVSESP32_PEAK_COUNTER_INDEX] = counter;
    frame[APP_I2CVSESP32_PEAK_CRC_INDEX] = crc8_cal(frame, APP_I2CVSESP32_PEAK_CRC_INDEX);
}

HAL_StatusTypeDef AppI2CvsESP32_SendRealtime(const AppI2CvsESP32_RealtimePayload_t *payload)
{
    if (payload == NULL)
    {
        return HAL_ERROR;
    }

    AppI2CvsESP32_BuildRealtime(s_tx_realtime_frame, payload, AppI2CvsESP32_NextCounter());

    return AppI2CvsESP32_Transmit(s_tx_realtime_frame, APP_I2CVSESP32_REALTIME_FRAME_SIZE);
}

HAL_StatusTypeDef AppI2CvsESP32_SendNormalSetting(const AppI2CvsESP32_NormalPayload_t *payload)
{
    if (payload == NULL)
    {
        return HAL_ERROR;
    }

    AppI2CvsESP32_BuildNormalSetting(s_tx_normal_frame, payload, AppI2CvsESP32_NextCounter());

    return AppI2CvsESP32_Transmit(s_tx_normal_frame, APP_I2CVSESP32_NORMAL_FRAME_SIZE);
}

HAL_StatusTypeDef AppI2CvsESP32_SendPeakSetting(const AppI2CvsESP32_PeakPayload_t *payload)
{
    if (payload == NULL)
    {
        return HAL_ERROR;
    }

    AppI2CvsESP32_BuildPeakSetting(s_tx_peak_frame, payload, AppI2CvsESP32_NextCounter());

    return AppI2CvsESP32_Transmit(s_tx_peak_frame, APP_I2CVSESP32_PEAK_FRAME_SIZE);
}

HAL_StatusTypeDef AppI2CvsESP32_SendAll(const AppI2CvsESP32_Payload_t *payload)
{
    HAL_StatusTypeDef status;
    uint8_t counter;

    if (payload == NULL)
    {
        return HAL_ERROR;
    }

    /* One counter value is shared by all 3 frames in the same send cycle. */
    counter = AppI2CvsESP32_NextCounter();

    AppI2CvsESP32_BuildRealtime(s_tx_realtime_frame, &payload->realtime, counter);
    status = AppI2CvsESP32_Transmit(s_tx_realtime_frame, APP_I2CVSESP32_REALTIME_FRAME_SIZE);
    if (status != HAL_OK)
    {
        return status;
    }

    AppI2CvsESP32_BuildNormalSetting(s_tx_normal_frame, &payload->normal, counter);
    status = AppI2CvsESP32_Transmit(s_tx_normal_frame, APP_I2CVSESP32_NORMAL_FRAME_SIZE);
    if (status != HAL_OK)
    {
        return status;
    }

    AppI2CvsESP32_BuildPeakSetting(s_tx_peak_frame, &payload->peak, counter);
    status = AppI2CvsESP32_Transmit(s_tx_peak_frame, APP_I2CVSESP32_PEAK_FRAME_SIZE);

    return status;
}
