/**
  ******************************************************************************
  * @file    app_WeconHMI.c
  * @author  Suu Nguyen Van - FPT Company 0971346938
  * @version V1.0
  * @date    14-05-2026
  * @brief   source file for flash application.
  ******************************************************************************
  */

#include "app_WeconHMI.h"
#include "app_flash.h"

#define APP_WECON_HMI_INIT_MAX_COUNT    (2000u)
#define APP_WECON_HMI_FLASH_ADD0        (0x08020080)
#define APP_WECON_HMI_g_input_ok_MAX      (1000u) /* 10s */

/* External configuration ----------------------------------------------------*/
extern RTC_TimeTypeDef setTime;
extern RTC_DateTypeDef setDate;
extern RTC_TimeTypeDef getTime;
extern RTC_DateTypeDef getDate;
extern RTC_HandleTypeDef hrtc;
extern uint16_t g_input_ok;
extern uint16_t g_ResetRTC_count;
extern uint16_t g_realTime_m;
extern uint16_t g_realTime_h;

/* Private variables ---------------------------------------------------------*/
static uint16_t s_init_value_count = 0u;

/* Private functions ---------------------------------------------------------*/

/* Public functions ---------------------------------------------------------*/
void AppWeconHMI_ResetInitValues(void)
{
    s_init_value_count = 0u;
}

void AppWeconHMI_InitValues(uint16_t regbank[][APP_RS485_REG_PER_GROUP], DGT_Settings_t *s)
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

    regbank[2][AppWeconHMI_REG_X1]      = s->normal.phase1.x;       /* xanh 1 */
    regbank[2][AppWeconHMI_REG_V1]      = s->normal.phase1.v;       /* vang 1 */
    regbank[2][AppWeconHMI_REG_GT1]     = s->normal.phase1.gt;    /* giai toa 1 */
    regbank[2][AppWeconHMI_REG_X2]      = s->normal.phase2.x;       /* xanh 2 */
    regbank[2][AppWeconHMI_REG_V2]      = s->normal.phase2.v;       /* vang 2 */
    regbank[2][AppWeconHMI_REG_GT2]     = s->normal.phase2.gt;    /* giai toa 2 */
    /* regbank[2][AppWeconHMI_REG_X3]   = s->normal.phase3.x; */
    /* regbank[2][AppWeconHMI_REG_V3]   = s->normal.phase3.v; */
    /* regbank[2][AppWeconHMI_REG_GT3]  = s->normal.phase3.gt; */
    regbank[2][AppWeconHMI_REG_begin_hour1]     = s->BlinkYel1_OnTime.start_h;
    regbank[2][AppWeconHMI_REG_begin_min1]      = s->BlinkYel1_OnTime.start_m;
    regbank[2][AppWeconHMI_REG_end_hour1]       = s->BlinkYel1_OnTime.end_h;
    regbank[2][AppWeconHMI_REG_end_min1]        = s->BlinkYel1_OnTime.end_m;
    regbank[0][AppWeconHMI_BIT_BlinkYel_ENA1]   = s->BlinkYel1_OnTime.flag;

    regbank[2][AppWeconHMI_REG_peak1_X1]    = s->peak1.phase1.x;   /* xanh 1 */
    regbank[2][AppWeconHMI_REG_peak1_V1]    = s->peak1.phase1.v;   /* vang 1 */
    regbank[2][AppWeconHMI_REG_peak1_GT1]   = s->peak1.phase1.gt;  /* giai toa 1 */
    regbank[2][AppWeconHMI_REG_peak1_X2]    = s->peak1.phase2.x;   /* xanh 2 */
    regbank[2][AppWeconHMI_REG_peak1_V2]    = s->peak1.phase2.v;   /* vang 2 */
    regbank[2][AppWeconHMI_REG_peak1_GT2]   = s->peak1.phase2.gt;  /* giai toa 2 */
    /* regbank[2][AppWeconHMI_REG_peak1_X3] = s->peak1.phase3.x; */
    /* regbank[2][AppWeconHMI_REG_peak1_V3] = s->peak1.phase3.v; */
    /* regbank[2][AppWeconHMI_REG_peak1_GT3]= s->peak1.phase3.gt; */
    regbank[2][AppWeconHMI_REG_peak1_start_h] = s->peak1_OnTime.start_h;
    regbank[2][AppWeconHMI_REG_peak1_start_m] = s->peak1_OnTime.start_m;
    regbank[2][AppWeconHMI_REG_peak1_end_h]   = s->peak1_OnTime.end_h;
    regbank[2][AppWeconHMI_REG_peak1_end_m]   = s->peak1_OnTime.end_m;
    regbank[0][AppWeconHMI_BIT_peak1_ENA]     = s->peak1_OnTime.flag;
    
    regbank[2][AppWeconHMI_REG_peak2_X1]    = s->peak2.phase1.x;   /* xanh 1 */
    regbank[2][AppWeconHMI_REG_peak2_V1]    = s->peak2.phase1.v;   /* vang 1 */
    regbank[2][AppWeconHMI_REG_peak2_GT1]   = s->peak2.phase1.gt;  /* giai toa 1 */
    regbank[2][AppWeconHMI_REG_peak2_X2]    = s->peak2.phase2.x;   /* xanh 2 */
    regbank[2][AppWeconHMI_REG_peak2_V2]    = s->peak2.phase2.v;   /* vang 2 */
    regbank[2][AppWeconHMI_REG_peak2_GT2]   = s->peak2.phase2.gt;  /* giai toa 2 */
    /* regbank[2][AppWeconHMI_REG_peak2_X3] = s->peak2.phase3.x; */
    /* regbank[2][AppWeconHMI_REG_peak2_V3] = s->peak2.phase3.v; */
    /* regbank[2][AppWeconHMI_REG_peak2_GT3]= s->peak2.phase3.gt; */
    regbank[2][AppWeconHMI_REG_peak2_start_h] = s->peak2_OnTime.start_h;
    regbank[2][AppWeconHMI_REG_peak2_start_m] = s->peak2_OnTime.start_m;
    regbank[2][AppWeconHMI_REG_peak2_end_h]   = s->peak2_OnTime.end_h;
    regbank[2][AppWeconHMI_REG_peak2_end_m]   = s->peak2_OnTime.end_m;
    regbank[0][AppWeconHMI_BIT_peak2_ENA]     = s->peak2_OnTime.flag;

    regbank[2][AppWeconHMI_REG_BlinkYel2_start_h]= s->BlinkYel2_OnTime.start_h;
    regbank[2][AppWeconHMI_REG_BlinkYel2_start_m]= s->BlinkYel2_OnTime.start_m;
    regbank[2][AppWeconHMI_REG_BlinkYel2_end_h]  = s->BlinkYel2_OnTime.end_h;
    regbank[2][AppWeconHMI_REG_BlinkYel2_end_m]  = s->BlinkYel2_OnTime.end_m;
    regbank[0][AppWeconHMI_BIT_BlinkYel2_flag]   = s->BlinkYel2_OnTime.flag;

    regbank[0][AppWeconHMI_BIT_Thaco_Blink_flag] = s->Thaco_Blink_flag;
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
	// inform g_input_ok
	regbank[0][AppWeconHMI_BIT_setTime_SAVE] = 0;
	regbank[0][AppWeconHMI_BIT_inputOk]     = 1;
	g_input_ok = 0;
	g_ResetRTC_count = 0;
}

void AppWeconHMI_NormalSetting(uint16_t regbank[][APP_RS485_REG_PER_GROUP], DGT_Settings_t *s){
// waitttttt
}


void AppWeconHMI_Process(uint16_t regbank[][APP_RS485_REG_PER_GROUP], DGT_Settings_t *s){
    if (g_input_ok >= APP_WECON_HMI_g_input_ok_MAX){// reset thong bao nhap thoi gian ok
      regbank[0][AppWeconHMI_BIT_inputOk]=0;
      g_input_ok=0;
    }
    
    AppWeconHMI_InitValues(regbank, s);
    AppWeconHMI_sendRTCtoHMI(regbank, getTime, getDate);

    if ((regbank[0][AppWeconHMI_BIT_getTime_HMI] == 1) && (g_ResetRTC_count == 0xFFFF)) g_ResetRTC_count = 0;
    if ((g_ResetRTC_count >= 5900) && (g_ResetRTC_count <= 6000)) {// reset input RTC variables after 1 minute, check HMI background script
      g_ResetRTC_count = 0xFFFF;
      AppWeconHMI_resetInputRTCVars(regbank);
    }

    if (regbank[0][AppWeconHMI_BIT_setTime_SAVE] == 1){////// SET time to RTC
      AppWeconHMI_setTimeToRTC(regbank, hrtc);
    }

    if (regbank[0][AppWeconHMI_BIT_NORMAL_SAVE] == 1){////// SET Data (Setting 1) -- 14 byte -- Normal setting
        s->normal.phase1.x             = (uint8_t)regbank[2][AppWeconHMI_REG_X1];	// xanh 1
        s->normal.phase1.v             = (uint8_t)regbank[2][AppWeconHMI_REG_V1];	// vang 1
        s->normal.phase1.gt            = (uint8_t)regbank[2][AppWeconHMI_REG_GT1];	// giai toa 1
        s->normal.phase2.x             = (uint8_t)regbank[2][AppWeconHMI_REG_X2];	// xanh 2
        s->normal.phase2.v             = (uint8_t)regbank[2][AppWeconHMI_REG_V2];	// vang 2
        s->normal.phase2.gt            = (uint8_t)regbank[2][AppWeconHMI_REG_GT2];	// giai toa 2
        // s->normal.phase3.x             = (uint8_t)regbank[2][AppWeconHMI_REG_X3];	// xanh 3
        // s->normal.phase3.v             = (uint8_t)regbank[2][AppWeconHMI_REG_V3];	// vang 3
        // s->normal.phase3.gt            = (uint8_t)regbank[2][AppWeconHMI_REG_GT3];	// giai toa 3
        s->BlinkYel1_OnTime.flag       = (uint8_t)regbank[0][AppWeconHMI_BIT_BlinkYel_ENA1];
        s->BlinkYel1_OnTime.start_h    = (uint8_t)regbank[2][AppWeconHMI_REG_begin_hour1];
        s->BlinkYel1_OnTime.start_m    = (uint8_t)regbank[2][AppWeconHMI_REG_begin_min1];
        s->BlinkYel1_OnTime.end_h      = (uint8_t)regbank[2][AppWeconHMI_REG_end_hour1];
        s->BlinkYel1_OnTime.end_m      = (uint8_t)regbank[2][AppWeconHMI_REG_end_min1];
        // Save current settings to Flash app
        (void)AppFlash_SaveSettings(s);
        // inform g_input_ok
        regbank[0][AppWeconHMI_BIT_NORMAL_SAVE] = 0;
        regbank[0][AppWeconHMI_BIT_inputOk] = 1;
        g_input_ok = 0;
    }

    if (regbank[0][AppWeconHMI_BIT_peak1_SAVE] == 1){////// SET Data (Setting_3) -- 14 byte -- Peak1
      s->peak1.phase1.x = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_X1];	// xanh 1
      s->peak1.phase1.v = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_V1];	// vang 1
      s->peak1.phase1.gt = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_GT1];	// giai toa 1
      s->peak1.phase2.x = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_X2];	// xanh 2
      s->peak1.phase2.v = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_V2];	// vang 2
      s->peak1.phase2.gt = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_GT2];	// giai toa 2
      // s->peak1.phase3.x = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_X3];	// xanh 3
      // s->peak1.phase3.v = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_V3];	// vang 3
      // s->peak1.phase3.gt = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_GT3];	// giai toa 3
      s->peak1_OnTime.flag    = (uint8_t)regbank[0][AppWeconHMI_BIT_peak1_ENA];
      s->peak1_OnTime.start_h = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_start_h];
      s->peak1_OnTime.start_m = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_start_m];
      s->peak1_OnTime.end_h   = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_end_h];
      s->peak1_OnTime.end_m   = (uint8_t)regbank[2][AppWeconHMI_REG_peak1_end_m];

      // Save current settings to Flash app
      (void)AppFlash_SaveSettings(s);
      // inform g_input_ok
      regbank[0][AppWeconHMI_BIT_peak1_SAVE] = 0;
      regbank[0][AppWeconHMI_BIT_inputOk] = 1;
      g_input_ok = 0;
    }

    if (regbank[0][AppWeconHMI_BIT_peak2_SAVE] == 1){////// SET Data (Setting_5) -- 14 byte -- Peak2
      s->peak2.phase1.x = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_X1];	// xanh 1
      s->peak2.phase1.v = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_V1];	// vang 1
      s->peak2.phase1.gt = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_GT1];	// giai toa 1
      s->peak2.phase2.x = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_X2];	// xanh 2
      s->peak2.phase2.v = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_V2];	// vang 2
      s->peak2.phase2.gt = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_GT2];	// giai toa 2
      // s->peak2.phase3.x = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_X3];	// xanh 3
      // s->peak2.phase3.v = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_V3];	// vang 3
      // s->peak2.phase3.gt = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_GT3];	// giai toa 3
      s->peak2_OnTime.flag    = (uint8_t)regbank[0][AppWeconHMI_BIT_peak2_ENA];
      s->peak2_OnTime.start_h = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_start_h];
      s->peak2_OnTime.start_m = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_start_m];
      s->peak2_OnTime.end_h   = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_end_h];
      s->peak2_OnTime.end_m   = (uint8_t)regbank[2][AppWeconHMI_REG_peak2_end_m];

      // Save current settings to Flash app
      (void)AppFlash_SaveSettings(s);
      // inform g_input_ok
      regbank[0][AppWeconHMI_BIT_peak2_SAVE] = 0;
      regbank[0][AppWeconHMI_BIT_inputOk] = 1;
      g_input_ok = 0;
    }

    if (regbank[0][AppWeconHMI_BIT_BlinkYel2_SAVE] == 1){////// SET Data (Setting_2) -- 05 byte -- BlinkYel2
      s->BlinkYel2_OnTime.flag    = (uint8_t)regbank[0][AppWeconHMI_BIT_BlinkYel2_flag];
      s->BlinkYel2_OnTime.start_h = (uint8_t)regbank[2][AppWeconHMI_REG_BlinkYel2_start_h];
      s->BlinkYel2_OnTime.start_m = (uint8_t)regbank[2][AppWeconHMI_REG_BlinkYel2_start_m];
      s->BlinkYel2_OnTime.end_h   = (uint8_t)regbank[2][AppWeconHMI_REG_BlinkYel2_end_h];
      s->BlinkYel2_OnTime.end_m   = (uint8_t)regbank[2][AppWeconHMI_REG_BlinkYel2_end_m];

      // Save current settings to Flash app
      (void)AppFlash_SaveSettings(s);
      
      // inform g_input_ok
      regbank[0][AppWeconHMI_BIT_BlinkYel2_SAVE] = 0;
      regbank[0][AppWeconHMI_BIT_inputOk] = 1;
      g_input_ok = 0;
    }

    if (regbank[0][AppWeconHMI_BIT_Thaco_Blink_flag_SAVE] == 1){////// SET Data (Back Setting) -- 01 byte -- Thaco_Blink
      // Thaco_Blink = (uint8_t)regbank[0][34];
      s->Thaco_Blink_flag = (uint8_t)regbank[0][AppWeconHMI_BIT_Thaco_Blink_flag];

      // Save current settings to Flash app
      (void)AppFlash_SaveSettings(s);

      // inform g_input_ok
      regbank[0][AppWeconHMI_BIT_Thaco_Blink_flag_SAVE] = 0;
      regbank[0][AppWeconHMI_BIT_inputOk] = 1;
      g_input_ok = 0;
    }
}