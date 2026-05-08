#include "app_flash.h"
#include <string.h>

#define APP_FLASH_DUMMY_1        0xDDU
#define APP_FLASH_DUMMY_2        0xEEU

static uint8_t AppFlash_GetByte(uint32_t word, uint8_t index)
{
    return (uint8_t)((word >> (index * 8U)) & 0xFFU);
}

static uint32_t AppFlash_Pack4(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0)
{
    return (((uint32_t)b3 << 24U) |
            ((uint32_t)b2 << 16U) |
            ((uint32_t)b1 <<  8U) |
            ((uint32_t)b0));
}

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

void AppFlash_DecodeSettings(const uint32_t *raw, AppFlash_Settings_t *s)
{
    if ((raw == NULL) || (s == NULL))
    {
        return;
    }

    memset(s, 0, sizeof(*s));

    /* Giu dung mapping cu trong LoadSettings_from_Flash() */
    s->Xanh2        = AppFlash_GetByte(raw[0], 0);
    s->GiaiToa1     = AppFlash_GetByte(raw[0], 1);
    s->Vang1        = AppFlash_GetByte(raw[0], 2);
    s->Xanh1        = AppFlash_GetByte(raw[0], 3);

    s->Vang3        = AppFlash_GetByte(raw[1], 0);
    s->Xanh3        = AppFlash_GetByte(raw[1], 1);
    s->GiaiToa2     = AppFlash_GetByte(raw[1], 2);
    s->Vang2        = AppFlash_GetByte(raw[1], 3);

    s->end_hour1    = AppFlash_GetByte(raw[2], 0);
    s->begin_min1   = AppFlash_GetByte(raw[2], 1);
    s->begin_hour1  = AppFlash_GetByte(raw[2], 2);
    s->GT3          = AppFlash_GetByte(raw[2], 3);

    s->BlinkYel_ENA1 = AppFlash_GetByte(raw[3], 2);
    s->end_min1      = AppFlash_GetByte(raw[3], 3);

    s->CaoDiem_X2   = AppFlash_GetByte(raw[4], 0);
    s->CaoDiem_GT1  = AppFlash_GetByte(raw[4], 1);
    s->CaoDiem_V1   = AppFlash_GetByte(raw[4], 2);
    s->CaoDiem_X1   = AppFlash_GetByte(raw[4], 3);

    s->CaoDiem_V3   = AppFlash_GetByte(raw[5], 0);
    s->CaoDiem_X3   = AppFlash_GetByte(raw[5], 1);
    s->CaoDiem_GT2  = AppFlash_GetByte(raw[5], 2);
    s->CaoDiem_V2   = AppFlash_GetByte(raw[5], 3);

    s->end_hour3    = AppFlash_GetByte(raw[6], 0);
    s->begin_min3   = AppFlash_GetByte(raw[6], 1);
    s->begin_hour3  = AppFlash_GetByte(raw[6], 2);
    s->CaoDiem_GT3  = AppFlash_GetByte(raw[6], 3);

    s->CaoDiem_ENA  = AppFlash_GetByte(raw[7], 2);
    s->end_min3     = AppFlash_GetByte(raw[7], 3);

    s->end_min2     = AppFlash_GetByte(raw[8], 0);
    s->end_hour2    = AppFlash_GetByte(raw[8], 1);
    s->begin_min2   = AppFlash_GetByte(raw[8], 2);
    s->begin_hour2  = AppFlash_GetByte(raw[8], 3);

    s->Thaco_Blink   = AppFlash_GetByte(raw[9], 0);
    s->BlinkYel_ENA2 = AppFlash_GetByte(raw[9], 3);
}

void AppFlash_EncodeSettings(const AppFlash_Settings_t *s, uint32_t *raw)
{
    if ((raw == NULL) || (s == NULL))
    {
        return;
    }

    raw[0] = AppFlash_Pack4(s->Xanh1,       s->Vang1,       s->GiaiToa1,    s->Xanh2);
    raw[1] = AppFlash_Pack4(s->Vang2,       s->GiaiToa2,    s->Xanh3,       s->Vang3);
    raw[2] = AppFlash_Pack4(s->GT3,         s->begin_hour1, s->begin_min1,  s->end_hour1);
    raw[3] = AppFlash_Pack4(s->end_min1,    s->BlinkYel_ENA1, APP_FLASH_DUMMY_2, APP_FLASH_DUMMY_2);

    raw[4] = AppFlash_Pack4(s->CaoDiem_X1,  s->CaoDiem_V1,  s->CaoDiem_GT1, s->CaoDiem_X2);
    raw[5] = AppFlash_Pack4(s->CaoDiem_V2,  s->CaoDiem_GT2, s->CaoDiem_X3,  s->CaoDiem_V3);
    raw[6] = AppFlash_Pack4(s->CaoDiem_GT3, s->begin_hour3, s->begin_min3,  s->end_hour3);
    raw[7] = AppFlash_Pack4(s->end_min3,    s->CaoDiem_ENA, APP_FLASH_DUMMY_2, APP_FLASH_DUMMY_2);

    raw[8] = AppFlash_Pack4(s->begin_hour2, s->begin_min2,  s->end_hour2,   s->end_min2);
    raw[9] = AppFlash_Pack4(s->BlinkYel_ENA2, APP_FLASH_DUMMY_2, APP_FLASH_DUMMY_2, s->Thaco_Blink);
}

HAL_StatusTypeDef AppFlash_LoadSettings(AppFlash_Settings_t *settings)
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

HAL_StatusTypeDef AppFlash_SaveSettings(const AppFlash_Settings_t *settings)
{
    uint32_t raw[APP_FLASH_WORD_COUNT] = {0};

    AppFlash_EncodeSettings(settings, raw);
    return AppFlash_WriteWords(raw, APP_FLASH_WORD_COUNT);
}
