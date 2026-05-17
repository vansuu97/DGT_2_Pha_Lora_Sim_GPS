/**
  ******************************************************************************
  * @file    app_WeconHMI.h
  * @author  Suu Nguyen Van - FPT Company 0971346938
  * @version V1.0
  * @date    14-05-2026
  * @brief   Header for flash application.
  ******************************************************************************
  */

#ifndef APP_WECON_HMI_H
#define APP_WECON_HMI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "app_rs485.h"

/* Global register bank for Wecon HMI */
#define AppWeconHMI_BIT_X1                (1u)
#define AppWeconHMI_BIT_V1                (2u)
#define AppWeconHMI_BIT_D1                (3u)
#define AppWeconHMI_BIT_X2                (4u)
#define AppWeconHMI_BIT_V2                (5u)
#define AppWeconHMI_BIT_D2                (6u)
#define AppWeconHMI_BIT_X3                (7u)
#define AppWeconHMI_BIT_V3                (8u)
#define AppWeconHMI_BIT_D3                (9u)
#define AppWeconHMI_BIT_inputOk       	  (11u)

/* Screen setting 1 normal */
#define AppWeconHMI_BIT_NORMAL_SAVE     (94u)
#define AppWeconHMI_BIT_BlinkYel_ENA1   (30u)
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

/* Screen setting 2 Blink Yellow 2*/
#define AppWeconHMI_BIT_BlinkYel2_SAVE		(98u)
#define AppWeconHMI_BIT_BlinkYel2_flag   	(32u)
#define AppWeconHMI_REG_BlinkYel2_start_h	(203u)
#define AppWeconHMI_REG_BlinkYel2_start_m	(204u)
#define AppWeconHMI_REG_BlinkYel2_end_h	    (205u)
#define AppWeconHMI_REG_BlinkYel2_end_m	    (206u)

/* Screen setting 3 - peak1 */
#define AppWeconHMI_BIT_peak1_SAVE 		(95u)
#define AppWeconHMI_BIT_peak1_ENA     	(31u)
#define AppWeconHMI_REG_peak1_X1  		(75u)
#define AppWeconHMI_REG_peak1_V1  		(76u)
#define AppWeconHMI_REG_peak1_GT1 		(77u)
#define AppWeconHMI_REG_peak1_X2  		(78u)
#define AppWeconHMI_REG_peak1_V2  		(79u)
#define AppWeconHMI_REG_peak1_GT2 		(80u)
#define AppWeconHMI_REG_peak1_X3  		(92u)
#define AppWeconHMI_REG_peak1_V3  		(93u)
#define AppWeconHMI_REG_peak1_GT3 		(94u)
#define AppWeconHMI_REG_peak1_start_h   (81u)
#define AppWeconHMI_REG_peak1_start_m	(82u)
#define AppWeconHMI_REG_peak1_end_h	    (90u)
#define AppWeconHMI_REG_peak1_end_m	    (91u)

/* Screen setting 3 - peak2 */
#define AppWeconHMI_BIT_peak2_SAVE 		(100u)
#define AppWeconHMI_BIT_peak2_ENA     	(33u)
#define AppWeconHMI_REG_peak2_X1  		(92u)
#define AppWeconHMI_REG_peak2_V1  		(93u)
#define AppWeconHMI_REG_peak2_GT1 		(94u)
#define AppWeconHMI_REG_peak2_X2  		(95u)
#define AppWeconHMI_REG_peak2_V2  		(96u)
#define AppWeconHMI_REG_peak2_GT2 		(97u)
#define AppWeconHMI_REG_peak2_X3  		(98u)
#define AppWeconHMI_REG_peak2_V3  		(99u)
#define AppWeconHMI_REG_peak2_GT3 		(100u)
#define AppWeconHMI_REG_peak2_start_h   (101u)
#define AppWeconHMI_REG_peak2_start_m	(102u)
#define AppWeconHMI_REG_peak2_end_h	    (103u)
#define AppWeconHMI_REG_peak2_end_m	    (104u)


/* Screen back setting */
#define AppWeconHMI_BIT_Thaco_Blink_flag_SAVE 	(99u)
#define AppWeconHMI_BIT_Thaco_Blink_flag 		(34u)

/* Screen realtime */
#define AppWeconHMI_BIT_getTime_HMI     (97u)
#define AppWeconHMI_REG_view_Hours	    (52u)
#define AppWeconHMI_REG_view_Minutes	(53u)
#define AppWeconHMI_REG_view_Seconds	(54u)
#define AppWeconHMI_REG_view_Date	    (55u)
#define AppWeconHMI_REG_view_Month	    (56u)
#define AppWeconHMI_REG_view_Year	    (57u)

#define AppWeconHMI_BIT_setTime_SAVE 	(96u)
#define AppWeconHMI_REG_setTime_Hours 	(58u)
#define AppWeconHMI_REG_setTime_Minutes (59u)
#define AppWeconHMI_REG_setTime_Seconds (70u)
#define AppWeconHMI_REG_setDate_Month  	(72u)
#define AppWeconHMI_REG_setDate_Date   	(71u)
#define AppWeconHMI_REG_setDate_Year   	(73u)
#define AppWeconHMI_REG_setDate_WeekDay (74u)

void AppWeconHMI_InitValues(uint16_t regbank[][APP_RS485_REG_PER_GROUP], DGT_Settings_t *s);
void AppWeconHMI_ResetInitValues(void);
void AppWeconHMI_resetInputRTCVars(uint16_t regbank[][APP_RS485_REG_PER_GROUP]);
void AppWeconHMI_sendRTCtoHMI(uint16_t regbank[][APP_RS485_REG_PER_GROUP], RTC_TimeTypeDef Time, RTC_DateTypeDef Date);
void AppWeconHMI_setTimeToRTC(uint16_t regbank[][APP_RS485_REG_PER_GROUP], RTC_HandleTypeDef source_hrtc);
void AppWeconHMI_NormalSetting(uint16_t regbank[][APP_RS485_REG_PER_GROUP], DGT_Settings_t *s);
void AppWeconHMI_Process(uint16_t regbank[][APP_RS485_REG_PER_GROUP], DGT_Settings_t *s);

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