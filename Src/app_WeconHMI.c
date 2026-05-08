#include "app_WeconHMI.h"

#define APP_WECON_HMI_INIT_MAX_COUNT  2000u

static uint16_t s_init_value_count = 0u;

/* Traffic light normal settings */
extern uint16_t X1;
extern uint16_t V1;
extern uint16_t GT1;
extern uint16_t GT2;
extern uint16_t Xanh1;
extern uint16_t Vang1;
extern uint16_t GiaiToa1;
extern uint16_t Xanh2;
extern uint16_t Vang2;
extern uint16_t GiaiToa2;
extern uint8_t begin_hour1;
extern uint8_t begin_min1;
extern uint8_t end_hour1;
extern uint8_t end_min1;
extern uint8_t BlinkYel_ENA1;
extern uint32_t Total_Time;

/* Peak-hour settings */
extern uint16_t CaoDiem_X1;
extern uint16_t CaoDiem_V1;
extern uint16_t CaoDiem_GT1;
extern uint16_t CaoDiem_X2;
extern uint16_t CaoDiem_V2;
extern uint16_t CaoDiem_GT2;
extern uint8_t begin_hour3;
extern uint8_t begin_min3;
extern uint8_t end_hour3;
extern uint8_t end_min3;
extern uint8_t CaoDiem_ENA;
extern uint32_t CaoDiem_Total_Time;

/* Blink-yellow 2 / Thaco settings */
extern uint8_t begin_hour2;
extern uint8_t begin_min2;
extern uint8_t end_hour2;
extern uint8_t end_min2;
extern uint8_t BlinkYel_ENA2;
extern uint8_t Thaco_Blink;

void AppWeconHMI_ResetInitValues(void)
{
    s_init_value_count = 0u;
}

void AppWeconHMI_InitValues(uint16_t regbank[][APP_RS485_REG_PER_GROUP])
{
    if (regbank == 0)
    {
        return;
    }

    if (s_init_value_count > APP_WECON_HMI_INIT_MAX_COUNT)
    {
        return;
    }

    s_init_value_count++;

    regbank[2][AppWeconHMI_REG_X1]      = Xanh1;       /* xanh 1 */
    regbank[2][AppWeconHMI_REG_V1]      = Vang1;       /* vang 1 */
    regbank[2][AppWeconHMI_REG_GT1]     = GiaiToa1;    /* giai toa 1 */
    regbank[2][AppWeconHMI_REG_X2]      = Xanh2;       /* xanh 2 */
    regbank[2][AppWeconHMI_REG_V2]      = Vang2;       /* vang 2 */
    regbank[2][AppWeconHMI_REG_GT2]     = GiaiToa2;    /* giai toa 2 */
    /* regbank[2][AppWeconHMI_REG_X3]   = Xanh3; */
    /* regbank[2][AppWeconHMI_REG_V3]   = Vang3; */
    /* regbank[2][AppWeconHMI_REG_GT3]  = GiaiToa3; */
    regbank[2][AppWeconHMI_REG_begin_hour1]     = begin_hour1;
    regbank[2][AppWeconHMI_REG_begin_min1]      = begin_min1;
    regbank[2][AppWeconHMI_REG_end_hour1]       = end_hour1;
    regbank[2][AppWeconHMI_REG_end_min1]        = end_min1;
    regbank[0][AppWeconHMI_BIT_BlinkYel_ENA1]   = BlinkYel_ENA1;
    Total_Time = X1 + V1 + GT1 + GT2;

    regbank[2][AppWeconHMI_REG_CaoDiem_X1] = CaoDiem_X1;   /* xanh 1 */
    regbank[2][AppWeconHMI_REG_CaoDiem_V1] = CaoDiem_V1;   /* vang 1 */
    regbank[2][AppWeconHMI_REG_CaoDiem_GT1] = CaoDiem_GT1;  /* giai toa 1 */
    regbank[2][AppWeconHMI_REG_CaoDiem_X2] = CaoDiem_X2;   /* xanh 2 */
    regbank[2][AppWeconHMI_REG_CaoDiem_V2] = CaoDiem_V2;   /* vang 2 */
    regbank[2][AppWeconHMI_REG_CaoDiem_GT2] = CaoDiem_GT2;  /* giai toa 2 */
    /* regbank[2][AppWeconHMI_REG_CaoDiem_X3] = CaoDiem_X3; */
    /* regbank[2][AppWeconHMI_REG_CaoDiem_V3] = CaoDiem_V3; */
    /* regbank[2][AppWeconHMI_REG_CaoDiem_GT3] = CaoDiem_GT3; */
    regbank[2][AppWeconHMI_REG_begin_hour3]     = begin_hour3;
    regbank[2][AppWeconHMI_REG_begin_min3]      = begin_min3;
    regbank[2][AppWeconHMI_REG_end_hour3]       = end_hour3;
    regbank[2][AppWeconHMI_REG_end_min3]        = end_min3;
    regbank[0][AppWeconHMI_BIT_CaoDiem_ENA]     = CaoDiem_ENA;
    CaoDiem_Total_Time = CaoDiem_X2 + CaoDiem_V2 + CaoDiem_GT1 + CaoDiem_GT2;

    regbank[2][AppWeconHMI_REG_begin_hour2]     = begin_hour2;
    regbank[2][AppWeconHMI_REG_begin_min2]      = begin_min2;
    regbank[2][AppWeconHMI_REG_end_hour2]       = end_hour2;
    regbank[2][AppWeconHMI_REG_end_min2]        = end_min2;
    regbank[0][AppWeconHMI_BIT_BlinkYel_ENA2]   = BlinkYel_ENA2;

    regbank[0][AppWeconHMI_BIT_Thaco_Blink]      = Thaco_Blink;
}

void AppWeconHMI_reset_input_RTC_vars(uint16_t regbank[][APP_RS485_REG_PER_GROUP]){// reset input RTC variables, check HMI background script
    regbank[2][AppWeconHMI_REG_setTime_Hours] = 0;
	regbank[2][AppWeconHMI_REG_setTime_Minutes] = 0;
	regbank[2][AppWeconHMI_REG_setTime_Seconds] = 0;
	regbank[2][AppWeconHMI_REG_setDate_Date] = 0;
	regbank[2][AppWeconHMI_REG_setDate_Month] = 0;
	regbank[2][AppWeconHMI_REG_setDate_Year] = 0;
	regbank[2][AppWeconHMI_REG_setDate_WeekDay] = 0;
	regbank[0][AppWeconHMI_BIT_getTime_HMI] = 0;// reset bnt get time from HMI to 0, check HMI background script
}