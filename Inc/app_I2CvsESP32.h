/**
 * @file    app_I2CvsESP32.h
 * @brief   Header for STM32F407 I2C communication to ESP32.
 */

#ifndef __APP_I2CVSESP32_H
#define __APP_I2CVSESP32_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

/* ===================== CONFIG ===================== */

#define APP_I2C_ADDR_7BIT               (0x12U)

#define APP_I2C_TIMEOUT_MS              (100U)
#define APP_I2C_DELAY_BETWEEN_FRAME_MS  (5U)

#define APP_I2C_FRAME_COUNT             (4U)
#define APP_I2C_EXTRA_DATA_SIZE         (10U)
#define APP_I2C_MAX_PAYLOAD_PER_FRAME   (15U)

/* counter + frame_id + payload_len + payload + crc */
#define APP_I2C_FRAME_HEADER_SIZE       (3U)
#define APP_I2C_FRAME_CRC_SIZE          (1U)

#define APP_I2C_MAX_FRAME_SIZE \
    (APP_I2C_FRAME_HEADER_SIZE + APP_I2C_MAX_PAYLOAD_PER_FRAME + APP_I2C_FRAME_CRC_SIZE)

/* ===================== PUBLIC FUNCTION ===================== */

void AppI2CvsESP32_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_I2CVSESP32_H */
