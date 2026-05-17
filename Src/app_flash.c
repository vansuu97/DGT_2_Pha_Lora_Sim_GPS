#include "app_flash.h"
#include <string.h>

HAL_StatusTypeDef AppFlash_ReadWords(uint32_t *buffer, uint32_t word_count)
{
    if ((buffer == NULL) || (word_count > APP_FLASH_WORD_COUNT))
    {
        return HAL_ERROR;
    }

    for (uint32_t i = 0U; i < word_count; i++)
    {
        buffer[i] = *(__IO uint32_t *)(APP_FLASH_START_ADDR + (i * 4U));
    }

    return HAL_OK;
}

HAL_StatusTypeDef AppFlash_WriteWords(const uint32_t *buffer, uint32_t word_count)
{
    HAL_StatusTypeDef status;
    uint32_t sector_error = 0U;

    if ((buffer == NULL) || (word_count > APP_FLASH_WORD_COUNT))
    {
        return HAL_ERROR;
    }

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK)
    {
        return status;
    }

    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase    = FLASH_TYPEERASE_SECTORS;
    erase.Sector       = APP_FLASH_SECTOR;
    erase.NbSectors    = 1U;
    erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    status = HAL_FLASHEx_Erase(&erase, &sector_error);

    if (status == HAL_OK)
    {
        for (uint32_t i = 0U; i < word_count; i++)
        {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                                       APP_FLASH_START_ADDR + (i * 4U),
                                       buffer[i]);
            if (status != HAL_OK)
            {
                break;
            }
        }
    }

    (void)HAL_FLASH_Lock();
    return status;
}

void AppFlash_DecodeSettings(const uint32_t *raw, DGT_Settings_t *s)
{
    if ((raw == NULL) || (s == NULL))
    {
        return;
    }

    memset(s, 0, sizeof(*s));
    memcpy((uint8_t *)s, (const uint8_t *)raw, sizeof(DGT_Settings_t));
}

void AppFlash_EncodeSettings(const DGT_Settings_t *s, uint32_t *raw)
{
    if ((raw == NULL) || (s == NULL))
    {
        return;
    }

    memset(raw, 0xFF, APP_FLASH_WORD_COUNT * 4);
    memcpy((uint8_t *)raw, (const uint8_t *)s, sizeof(DGT_Settings_t));
}

HAL_StatusTypeDef AppFlash_LoadSettings(DGT_Settings_t *settings)
{
    uint32_t raw[APP_FLASH_WORD_COUNT] = {0};
    HAL_StatusTypeDef status;

    status = AppFlash_ReadWords(raw, APP_FLASH_WORD_COUNT);
    if (status == HAL_OK)
    {
        AppFlash_DecodeSettings(raw, settings);
    }

    return status;
}

HAL_StatusTypeDef AppFlash_SaveSettings(const DGT_Settings_t *settings)
{
    uint32_t raw[APP_FLASH_WORD_COUNT] = {0};

    AppFlash_EncodeSettings(settings, raw);
    return AppFlash_WriteWords(raw, APP_FLASH_WORD_COUNT);
}
