/**
  ******************************************************************************
  * @file    app_DGT.h
  * @author  Suu Nguyen Van - FPT Company 0971346938
  * @version V1.0
  * @date    05-05-2026
  * @brief   Header for traffic light control application.
  ******************************************************************************
  */

#ifndef APP_DGT_H_
#define APP_DGT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief  Calculate automatic yellow-blink mode from configured time ranges.
 * @retval 1: yellow blink active, 0: normal traffic cycle
 */
uint8_t AppDGT_BlinkYellowAutoCalculate(void);

/**
 * @brief  Run manual traffic-light mode.
 */
void AppDGT2P_Manual(void);

/**
 * @brief  Run yellow-blink mode.
 */
void AppDGT2P_BlinkYellow(void);

/**
 * @brief  Run automatic traffic-light cycle.
 */
void AppDGT2P_Auto(void);

/**
 * @brief  Main DGT application process.
 * @note   Call this function periodically from main loop or RTOS task.
 */
void AppDGT2P_Process(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_DGT_H_ */
