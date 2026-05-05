/**
  ******************************************************************************
  * @file app_digital_input.h
  * @author Suu Nguyen Van - FPT Company 0971346938
  * @version V1.0
  * @date 27-04-2026
  ******************************************************************************
  * 
  *
  ******************************************************************************
  */

#ifndef APP_DIGITAL_INPUT_H
#define APP_DIGITAL_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Digital input ID */
typedef enum
{
    DIGITAL_INPUT_SWITCH_1 = 0,
    DIGITAL_INPUT_SWITCH_2,
    DIGITAL_INPUT_SWITCH_3,
    DIGITAL_INPUT_SWITCH_4,
    DIGITAL_INPUT_SWITCH_5,

    DIGITAL_INPUT_MAX
} APP_DIGITAL_INPUT_ID;

/* Public functions */
void AppDigitalInput_Init(void);
void AppDigitalInput_1msTask(void);

/*
 * Used for active-low switch input with pull-up:
 * OFF = 1
 * ON  = 0
 */
uint8_t AppDigitalInput_Check(APP_DIGITAL_INPUT_ID id);
uint8_t AppDigitalInput_TurnedOnEvent(APP_DIGITAL_INPUT_ID id);
uint8_t AppDigitalInput_TurnedOffEvent(APP_DIGITAL_INPUT_ID id);

uint8_t AppDigitalInput_GetRaw(APP_DIGITAL_INPUT_ID id);
uint8_t AppDigitalInput_GetFiltered(APP_DIGITAL_INPUT_ID id);

void AppDigitalInput_SetDebounce(APP_DIGITAL_INPUT_ID id, uint16_t debounce_ms);

#ifdef __cplusplus
}
#endif

#endif /* APP_DIGITAL_INPUT_H */