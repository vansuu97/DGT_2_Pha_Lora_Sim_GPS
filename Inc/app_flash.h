#ifndef APP_FLASH_H
#define APP_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdint.h>

#define APP_FLASH_START_ADDR     ((uint32_t)0x08020080U)
#define APP_FLASH_SECTOR         FLASH_SECTOR_5
#define APP_FLASH_WORD_COUNT     12U


HAL_StatusTypeDef AppFlash_ReadWords(uint32_t *buffer, uint32_t word_count);
HAL_StatusTypeDef AppFlash_WriteWords(const uint32_t *buffer, uint32_t word_count);

void AppFlash_DecodeSettings(const uint32_t *raw, DGT_Settings_t *settings);
void AppFlash_EncodeSettings(const DGT_Settings_t *settings, uint32_t *raw);

HAL_StatusTypeDef AppFlash_LoadSettings(DGT_Settings_t *settings);
HAL_StatusTypeDef AppFlash_SaveSettings(const DGT_Settings_t *settings);

#ifdef __cplusplus
}
#endif

#endif /* APP_FLASH_H */
