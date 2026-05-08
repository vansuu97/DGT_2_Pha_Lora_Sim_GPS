#include "app_WeconHMI.h"

#define APP_WECON_HMI_INIT_MAX_COUNT  2000u
#define APP_WECON_HMI_FLASH_ADD0   0x08020080

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

extern RTC_TimeTypeDef setTime;
extern RTC_DateTypeDef setDate;
extern uint16_t input_ok;
extern uint16_t ResetRTC_count;
extern uint32_t myFlashData[10];

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

void AppWeconHMI_resetInputRTCVars(uint16_t regbank[][APP_RS485_REG_PER_GROUP]){// reset input RTC variables, check HMI background script
    regbank[2][AppWeconHMI_REG_setTime_Hours] = 0;
	regbank[2][AppWeconHMI_REG_setTime_Minutes] = 0;
	regbank[2][AppWeconHMI_REG_setTime_Seconds] = 0;
	regbank[2][AppWeconHMI_REG_setDate_Date] = 0;
	regbank[2][AppWeconHMI_REG_setDate_Month] = 0;
	regbank[2][AppWeconHMI_REG_setDate_Year] = 0;
	regbank[2][AppWeconHMI_REG_setDate_WeekDay] = 0;
	regbank[0][AppWeconHMI_BIT_getTime_HMI] = 0;// reset bnt get time from HMI to 0, check HMI background script
}

void AppWeconHMI_sendRTCtoHMI(uint16_t regbank[][APP_RS485_REG_PER_GROUP], RTC_TimeTypeDef sourceTime, RTC_DateTypeDef sourceDate){
    regbank[2][AppWeconHMI_REG_view_Hours]    = sourceTime.Hours;
	regbank[2][AppWeconHMI_REG_view_Minutes]  = sourceTime.Minutes;
	regbank[2][AppWeconHMI_REG_view_Seconds]  = sourceTime.Seconds;
	regbank[2][AppWeconHMI_REG_view_Date]     = sourceDate.Date;
	regbank[2][AppWeconHMI_REG_view_Month]    = sourceDate.Month;
	regbank[2][AppWeconHMI_REG_view_Year]     = sourceDate.Year;
}

void AppWeconHMI_setTimeToRTC(uint16_t regbank[][APP_RS485_REG_PER_GROUP], RTC_HandleTypeDef source_hrtc){
    setTime.Hours 	= regbank[2][AppWeconHMI_REG_setTime_Hours];
	setTime.Minutes = regbank[2][AppWeconHMI_REG_setTime_Minutes];
	setTime.Seconds = regbank[2][AppWeconHMI_REG_setTime_Seconds];
	setTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	setTime.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&source_hrtc, &setTime, RTC_FORMAT_BIN);
	
	if (regbank[2][AppWeconHMI_REG_setDate_WeekDay]>1) setDate.WeekDay = regbank[2][AppWeconHMI_REG_setDate_WeekDay]-1;
	if (regbank[2][AppWeconHMI_REG_setDate_WeekDay]==1) setDate.WeekDay = RTC_WEEKDAY_SUNDAY;
	
	setDate.Month= regbank[2][AppWeconHMI_REG_setDate_Month];
	setDate.Date = regbank[2][AppWeconHMI_REG_setDate_Date];
	setDate.Year = regbank[2][AppWeconHMI_REG_setDate_Year];   // chỉ lưu 2 số cuối: 2026 -> 26
	HAL_RTC_SetDate(&source_hrtc, &setDate, RTC_FORMAT_BIN);
	
	HAL_RTCEx_BKUPWrite(&source_hrtc, RTC_BKP_DR1, 0x12);
	// inform input_ok
	regbank[0][AppWeconHMI_BIT_SET_TIME_RTC] = 0;
	regbank[0][AppWeconHMI_BIT_inputOk]     = 1;
	input_ok = 0;
	ResetRTC_count = 0;
}

void AppWeconHMI_SettingScreen1(uint16_t regbank[][APP_RS485_REG_PER_GROUP]){
//     Xanh1	    = regbank[2][AppWeconHMI_REG_X1];	// xanh 1
// 	Vang1	    = regbank[2][AppWeconHMI_REG_V1];	// vang 1
// 	GiaiToa1  = regbank[2][AppWeconHMI_REG_GT1];	// giai toa 1
					
// 	Xanh2	 = regbank[2][AppWeconHMI_REG_X2];	// xanh 2 
// 	Vang2	 = regbank[2][AppWeconHMI_REG_V2];	// vang 2
// 	GiaiToa2 = regbank[2][AppWeconHMI_REG_GT2];	// giai toa 2
					
// //	X3 = regbank[2][AppWeconHMI_REG_X3];	// xanh 3
// //	V3 = regbank[2][AppWeconHMI_REG_V3];	// vang 3
// //	GT3= regbank[2][AppWeconHMI_REG_GT3];	// giai toa 3
	
// 	begin_hour1		= (uint8_t)regbank[2][AppWeconHMI_REG_begin_hour1];
// 	begin_min1		= (uint8_t)regbank[2][AppWeconHMI_REG_begin_min1];
// 	end_hour1		= (uint8_t)regbank[2][AppWeconHMI_REG_end_hour1];
// 	end_min1		= (uint8_t)regbank[2][AppWeconHMI_REG_end_min1];
// 	BlinkYel_ENA1	= (uint8_t)regbank[0][AppWeconHMI_BIT_BlinkYel_ENA1];
	
// 	// Read from flash
// 	Flash_GetData(APP_WECON_HMI_FLASH_ADD0, &myFlashData[0], 10);
	
// 	// Write to flash
// 	myFlashData[0] = regbank[2][120];							// xanh 1
// 	myFlashData[0] = (myFlashData[0]) << 8 | regbank[2][121];	// vang 1
// 	myFlashData[0] = (myFlashData[0]) << 8 | regbank[2][122];	// giai toa 1						
// 	myFlashData[0] = (myFlashData[0]) << 8 | regbank[2][123];	// xanh 2 
// 	myFlashData[1] = regbank[2][124];							// vang 2
// 	myFlashData[1] = (myFlashData[1]) << 8 | regbank[2][125];	// giai toa 2
// 	myFlashData[1] = (myFlashData[1]) << 8 | 0xDD;					// xanh 3
// 	myFlashData[1] = (myFlashData[1]) << 8 | 0xDD;					// vang 3
// 	myFlashData[2] = 0xDD;											// giai toa 3
// 	myFlashData[2] = (myFlashData[2]) << 8 | begin_hour1;			// begin_hour1
// 	myFlashData[2] = (myFlashData[2]) << 8 | begin_min1;			// begin_min1
// 	myFlashData[2] = (myFlashData[2]) << 8 | end_hour1;				// end_hour1
// 	myFlashData[3] = end_min1;										// end_min1
// 	myFlashData[3] = (myFlashData[3]) << 8 | BlinkYel_ENA1;			// BlinkYel_ENA1
// 	myFlashData[3] = (myFlashData[3]) << 8 | 0xEE;					// Dummy
// 	myFlashData[3] = (myFlashData[3]) << 8 | 0xEE;					// Dummy
	
// 	Flash_WriteData(APP_WECON_HMI_FLASH_ADD0, &myFlashData[0], 10);
	
// 	Total_Time = Xanh1 + Vang1 + GiaiToa1 + GiaiToa2;
	
// 	// inform input_ok
// 	regbank[0][94] = 0;
// 	regbank[0][AppWeconHMI_BIT_inputOk] = 1;
// 	input_ok = 0;
}