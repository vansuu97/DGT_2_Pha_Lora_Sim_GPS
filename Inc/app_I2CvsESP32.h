#ifndef APP_I2CVSESP32_H
#define APP_I2CVSESP32_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define APP_I2CVSESP32_ADDR_7BIT              0x12U

#define APP_I2CVSESP32_CMD_REALTIME           0x01U
#define APP_I2CVSESP32_CMD_NORMAL_SETTING     0x02U
#define APP_I2CVSESP32_CMD_PEAK_SETTING       0x03U

/*
 * Frame format: CMD + DATA + Counter + CRC8
 * No start/end bytes are used because I2C already transfers each frame as one transaction.
 * CRC8 is calculated from frame[0] to the byte before CRC8.
 */

/*
 * Frame 1 - REALTIME + BLINK YELLOW 1: 17 bytes
 * [0]  CMD = APP_I2CVSESP32_CMD_REALTIME
 * [1]  Date
 * [2]  Month
 * [3]  Year, last two digits only. Example: 2026 -> 26
 * [4]  Hour
 * [5]  Minute
 * [6]  Second
 * [7]  Voltage1
 * [8]  Voltage2
 * [9]  Current1
 * [10] Current2
 * [11] begin_hour1
 * [12] begin_min1
 * [13] end_hour1
 * [14] end_min1
 * [15] Counter
 * [16] CRC8 checksum of frame[0]..frame[15]
 */
#define APP_I2CVSESP32_REALTIME_FRAME_SIZE        17U
#define APP_I2CVSESP32_REALTIME_COUNTER_INDEX     15U
#define APP_I2CVSESP32_REALTIME_CRC_INDEX         16U

/*
 * Frame 2 - NORMAL SETTING + BLINK YELLOW 2: 16 bytes
 * [0]  CMD = APP_I2CVSESP32_CMD_NORMAL_SETTING
 * [1]  X1
 * [2]  V1
 * [3]  GT1
 * [4]  X2
 * [5]  V2
 * [6]  GT2
 * [7]  X3
 * [8]  V3
 * [9]  GT3
 * [10] begin_hour2
 * [11] begin_min2
 * [12] end_hour2
 * [13] end_min2
 * [14] Counter
 * [15] CRC8 checksum of frame[0]..frame[14]
 */
#define APP_I2CVSESP32_NORMAL_FRAME_SIZE          16U
#define APP_I2CVSESP32_NORMAL_COUNTER_INDEX       14U
#define APP_I2CVSESP32_NORMAL_CRC_INDEX           15U

/*
 * Frame 3 - PEAK SETTING: 16 bytes
 * [0]  CMD = APP_I2CVSESP32_CMD_PEAK_SETTING
 * [1]  begin_hour3
 * [2]  begin_min3
 * [3]  end_hour3
 * [4]  end_min3
 * [5]  CaoDiem_X1
 * [6]  CaoDiem_V1
 * [7]  CaoDiem_GT1
 * [8]  CaoDiem_X2
 * [9]  CaoDiem_V2
 * [10] CaoDiem_GT2
 * [11] CaoDiem_X3
 * [12] CaoDiem_V3
 * [13] CaoDiem_GT3
 * [14] Counter
 * [15] CRC8 checksum of frame[0]..frame[14]
 */
#define APP_I2CVSESP32_PEAK_FRAME_SIZE            16U
#define APP_I2CVSESP32_PEAK_COUNTER_INDEX         14U
#define APP_I2CVSESP32_PEAK_CRC_INDEX             15U

#define APP_I2CVSESP32_MAX_FRAME_SIZE             APP_I2CVSESP32_REALTIME_FRAME_SIZE

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint16_t slave_addr_7bit;
    uint32_t timeout_ms;
} AppI2CvsESP32_Config_t;

typedef struct
{
    uint8_t date;
    uint8_t month;
    uint8_t year;       /* Last two digits only. Example: 2026 -> 26 */

    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t voltage1;
    uint8_t voltage2;
    uint8_t current1;
    uint8_t current2;

    uint8_t begin_hour1;
    uint8_t begin_min1;
    uint8_t end_hour1;
    uint8_t end_min1;
} AppI2CvsESP32_RealtimePayload_t;

typedef struct
{
    uint8_t x1;
    uint8_t v1;
    uint8_t gt1;

    uint8_t x2;
    uint8_t v2;
    uint8_t gt2;

    uint8_t x3;
    uint8_t v3;
    uint8_t gt3;

    uint8_t begin_hour2;
    uint8_t begin_min2;
    uint8_t end_hour2;
    uint8_t end_min2;
} AppI2CvsESP32_NormalPayload_t;

typedef struct
{
    /* begin == end means disabled. */
    uint8_t begin_hour3;
    uint8_t begin_min3;
    uint8_t end_hour3;
    uint8_t end_min3;

    uint8_t peak_x1;
    uint8_t peak_v1;
    uint8_t peak_gt1;

    uint8_t peak_x2;
    uint8_t peak_v2;
    uint8_t peak_gt2;

    uint8_t peak_x3;
    uint8_t peak_v3;
    uint8_t peak_gt3;
} AppI2CvsESP32_PeakPayload_t;

typedef struct
{
    AppI2CvsESP32_RealtimePayload_t realtime;
    AppI2CvsESP32_NormalPayload_t normal;
    AppI2CvsESP32_PeakPayload_t peak;
} AppI2CvsESP32_Payload_t;

void AppI2CvsESP32_Init(const AppI2CvsESP32_Config_t *config);
void AppI2CvsESP32_SetCounter(uint8_t counter);
uint8_t AppI2CvsESP32_GetCounter(void);

HAL_StatusTypeDef AppI2CvsESP32_SendRealtime(const AppI2CvsESP32_RealtimePayload_t *payload);
HAL_StatusTypeDef AppI2CvsESP32_SendNormalSetting(const AppI2CvsESP32_NormalPayload_t *payload);
HAL_StatusTypeDef AppI2CvsESP32_SendPeakSetting(const AppI2CvsESP32_PeakPayload_t *payload);
HAL_StatusTypeDef AppI2CvsESP32_SendAll(const AppI2CvsESP32_Payload_t *payload);

void AppI2CvsESP32_BuildRealtime(uint8_t frame[APP_I2CVSESP32_REALTIME_FRAME_SIZE],
                                 const AppI2CvsESP32_RealtimePayload_t *payload,
                                 uint8_t counter);
void AppI2CvsESP32_BuildNormalSetting(uint8_t frame[APP_I2CVSESP32_NORMAL_FRAME_SIZE],
                                      const AppI2CvsESP32_NormalPayload_t *payload,
                                      uint8_t counter);
void AppI2CvsESP32_BuildPeakSetting(uint8_t frame[APP_I2CVSESP32_PEAK_FRAME_SIZE],
                                    const AppI2CvsESP32_PeakPayload_t *payload,
                                    uint8_t counter);

#ifdef __cplusplus
}
#endif

#endif /* APP_I2CVSESP32_H */
