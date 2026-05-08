#ifndef APP_FLASH_H
#define APP_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define APP_FLASH_START_ADDR     ((uint32_t)0x08020080U)
#define APP_FLASH_SECTOR         FLASH_SECTOR_5
#define APP_FLASH_WORD_COUNT     10U

/* Layout setting dang luu trong Flash hien tai: 10 word x 32-bit.
 * Moi gia tri setting chi dung 1 byte, pack theo format cu de khong mat data cu.
 */
typedef struct
{
    uint8_t Xanh1;
    uint8_t Vang1;
    uint8_t GiaiToa1;
    uint8_t Xanh2;
    uint8_t Vang2;
    uint8_t GiaiToa2;
    uint8_t Xanh3;
    uint8_t Vang3;
    uint8_t GT3;

    uint8_t begin_hour1;
    uint8_t begin_min1;
    uint8_t end_hour1;
    uint8_t end_min1;
    uint8_t BlinkYel_ENA1;

    uint8_t CaoDiem_X1;
    uint8_t CaoDiem_V1;
    uint8_t CaoDiem_GT1;
    uint8_t CaoDiem_X2;
    uint8_t CaoDiem_V2;
    uint8_t CaoDiem_GT2;
    uint8_t CaoDiem_X3;
    uint8_t CaoDiem_V3;
    uint8_t CaoDiem_GT3;

    uint8_t begin_hour3;
    uint8_t begin_min3;
    uint8_t end_hour3;
    uint8_t end_min3;
    uint8_t CaoDiem_ENA;

    uint8_t begin_hour2;
    uint8_t begin_min2;
    uint8_t end_hour2;
    uint8_t end_min2;
    uint8_t BlinkYel_ENA2;
    uint8_t Thaco_Blink;
} AppFlash_Settings_t;

HAL_StatusTypeDef AppFlash_ReadWords(uint32_t *buffer, uint32_t word_count);
HAL_StatusTypeDef AppFlash_WriteWords(const uint32_t *buffer, uint32_t word_count);

void AppFlash_DecodeSettings(const uint32_t *raw, AppFlash_Settings_t *settings);
void AppFlash_EncodeSettings(const AppFlash_Settings_t *settings, uint32_t *raw);

HAL_StatusTypeDef AppFlash_LoadSettings(AppFlash_Settings_t *settings);
HAL_StatusTypeDef AppFlash_SaveSettings(const AppFlash_Settings_t *settings);

#ifdef __cplusplus
}
#endif

#endif /* APP_FLASH_H */
