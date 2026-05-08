#ifndef APP_WECON_HMI_H
#define APP_WECON_HMI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "app_rs485.h"

/* Global register bank for Wecon HMI */
#define AppWeconHMI_BIT_D1                (1u)
#define AppWeconHMI_BIT_V1                (2u)
#define AppWeconHMI_BIT_X1                (3u)
#define AppWeconHMI_BIT_X2                (4u)
#define AppWeconHMI_BIT_V2                (5u)
#define AppWeconHMI_BIT_D2                (6u)
#define AppWeconHMI_BIT_getTime_HMI       (97u)
#define AppWeconHMI_BIT_inputOk       	  (11u)
#define AppWeconHMI_BIT_SET_TIME_RTC      (96u)

/* Screen setting 1 */
#define AppWeconHMI_REG_X1              (120u)
#define AppWeconHMI_REG_V1              (121u)
#define AppWeconHMI_REG_GT1             (122u)

#define AppWeconHMI_REG_X2              (123u)
#define AppWeconHMI_REG_V2              (124u)
#define AppWeconHMI_REG_GT2             (125u)

#define AppWeconHMI_REG_X3              (142u)
#define AppWeconHMI_REG_V3              (143u)
#define AppWeconHMI_REG_GT3             (144u)

#define AppWeconHMI_REG_begin_hour1	    (126u)
#define AppWeconHMI_REG_begin_min1	    (127u)
#define AppWeconHMI_REG_end_hour1	    (140u)
#define AppWeconHMI_REG_end_min1	    (141u)
#define AppWeconHMI_BIT_BlinkYel_ENA1   (30u)

/* Screen setting 2 */
#define AppWeconHMI_REG_begin_hour2	    (203u)
#define AppWeconHMI_REG_begin_min2	    (204u)
#define AppWeconHMI_REG_end_hour2	    (205u)
#define AppWeconHMI_REG_end_min2	    (206u)
#define AppWeconHMI_BIT_BlinkYel_ENA2   (32u)

/* Screen setting 3 */
#define AppWeconHMI_REG_CaoDiem_X1  (75u)
#define AppWeconHMI_REG_CaoDiem_V1  (76u)
#define AppWeconHMI_REG_CaoDiem_GT1 (77u)
						
#define AppWeconHMI_REG_CaoDiem_X2  (78u)
#define AppWeconHMI_REG_CaoDiem_V2  (79u)
#define AppWeconHMI_REG_CaoDiem_GT2 (80u)

#define AppWeconHMI_REG_CaoDiem_X3  (92u)
#define AppWeconHMI_REG_CaoDiem_V3  (93u)
#define AppWeconHMI_REG_CaoDiem_GT3  (94u)
		
#define AppWeconHMI_REG_begin_hour3     (81u)
#define AppWeconHMI_REG_begin_min3	    (82u)
#define AppWeconHMI_REG_end_hour3	    (90u)
#define AppWeconHMI_REG_end_min3	    (91u)
#define AppWeconHMI_BIT_CaoDiem_ENA     (31u)

/* Screen back setting */
#define AppWeconHMI_BIT_Thaco_Blink     (34u)

/* Screen realtime */
#define AppWeconHMI_REG_view_Hours	    (52u)
#define AppWeconHMI_REG_view_Minutes	(53u)
#define AppWeconHMI_REG_view_Seconds	(54u)
#define AppWeconHMI_REG_view_Date	    (55u)
#define AppWeconHMI_REG_view_Month	    (56u)
#define AppWeconHMI_REG_view_Year	    (57u)
	
#define AppWeconHMI_REG_setTime_Hours 	(58u)
#define AppWeconHMI_REG_setTime_Minutes (59u)
#define AppWeconHMI_REG_setTime_Seconds (70u)

#define AppWeconHMI_REG_setDate_Month  (72u)
#define AppWeconHMI_REG_setDate_Date   (71u)
#define AppWeconHMI_REG_setDate_Year   (73u)
#define AppWeconHMI_REG_setDate_WeekDay (74u)

void AppWeconHMI_InitValues(uint16_t regbank[][APP_RS485_REG_PER_GROUP]);
void AppWeconHMI_ResetInitValues(void);
void AppWeconHMI_resetInputRTCVars(uint16_t regbank[][APP_RS485_REG_PER_GROUP]);
void AppWeconHMI_sendRTCtoHMI(uint16_t regbank[][APP_RS485_REG_PER_GROUP], RTC_TimeTypeDef Time, RTC_DateTypeDef Date);
void AppWeconHMI_setTimeToRTC(uint16_t regbank[][APP_RS485_REG_PER_GROUP], RTC_HandleTypeDef source_hrtc);
void AppWeconHMI_SettingScreen1(uint16_t regbank[][APP_RS485_REG_PER_GROUP]);

#ifdef __cplusplus
}
#endif

#endif /* APP_WECON_HMI_H */

	/*
	// RS485_regbank[2][58] = getTime.Seconds;
	// RS485_regbank[2][59] = getTime.Seconds;
	// RS485_regbank[2][70] = getTime.Seconds;
	// RS485_regbank[2][71] = getTime.Seconds;
	// RS485_regbank[2][72] = getTime.Seconds;
	// RS485_regbank[2][73] = getTime.Seconds;
	// RS485_regbank[2][74] = getTime.Seconds;
	// RS485_regbank[2][75] = getTime.Seconds;
	// RS485_regbank[2][76] = getTime.Seconds;
	// RS485_regbank[2][77] = getTime.Seconds;
	// RS485_regbank[2][78] = getTime.Seconds;
	// RS485_regbank[2][79] = getTime.Seconds;
	// RS485_regbank[2][80] = getTime.Seconds;
	// RS485_regbank[2][81] = getTime.Seconds;
	// RS485_regbank[2][82] = getTime.Seconds;
	// RS485_regbank[2][90] = getTime.Seconds;
	// RS485_regbank[2][91] = getTime.Seconds;
	// RS485_regbank[2][92] = getTime.Seconds;
	// RS485_regbank[2][93] = getTime.Seconds;
	// RS485_regbank[2][94] = getTime.Seconds;
	// RS485_regbank[2][120] = getTime.Seconds;
	// RS485_regbank[2][121] = getTime.Seconds;
	// RS485_regbank[2][122] = getTime.Seconds;
	// RS485_regbank[2][123] = getTime.Seconds;
	// RS485_regbank[2][124] = getTime.Seconds;
	// RS485_regbank[2][125] = getTime.Seconds;
	// RS485_regbank[2][126] = getTime.Seconds;
	// RS485_regbank[2][127] = getTime.Seconds;
	// RS485_regbank[2][140] = getTime.Seconds;
	// RS485_regbank[2][141] = getTime.Seconds;
	// RS485_regbank[2][142] = getTime.Seconds;
	// RS485_regbank[2][143] = getTime.Seconds;
	// RS485_regbank[2][144] = getTime.Seconds;
	// RS485_regbank[2][203] = getTime.Seconds;
	// RS485_regbank[2][204] = getTime.Seconds;
	// RS485_regbank[2][205] = getTime.Seconds;
	// RS485_regbank[2][206] = getTime.Seconds;
	
	// if (RS485_regbank[0][94] == 1) RS485_regbank[0][94] = 0; //Setting 1
	// if (RS485_regbank[0][96] == 1) RS485_regbank[0][96] = 0;
	// if (RS485_regbank[0][97] == 1) RS485_regbank[0][97] = 0;
	// if (RS485_regbank[0][98] == 1) RS485_regbank[0][98] = 0;  // Setting_2
	// if (RS485_regbank[0][99] == 1) RS485_regbank[0][99] = 0;  // Thaco_Blink
	// if (RS485_regbank[0][30] == 1) RS485_regbank[0][30] = 0;
	// if (RS485_regbank[0][31] == 1) RS485_regbank[0][31] = 0;
	// if (RS485_regbank[0][32] == 1) RS485_regbank[0][32] = 0; // ChopVang_ENA_2
	// if (RS485_regbank[0][34] == 1) RS485_regbank[0][34] = 0; // Thaco_Blink
	// if (RS485_regbank[0][95] == 1) RS485_regbank[0][95] = 0; // Thaco_Blink
	*/