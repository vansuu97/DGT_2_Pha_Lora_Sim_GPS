/**
  ******************************************************************************
  * @file    app_DGT.c
  * @author  Suu Nguyen Van - FPT Company 0971346938
  * @version V1.0
  * @date    07-05-2026
  * @brief   Traffic light control application for DGT 2-phase mode.
  *
  * @details This file implements the DGT traffic light state machine, including:
  *          - Automatic operation mode.
  *          - Manual operation mode.
  *          - Yellow blink operation mode.
  *          - Peak-time timing selection.
  *          - RS485 register-bank update for output status.
  *
  *          Output status is organized by nested phase structure:
  *          AppDGT2P_OutputStatus_t.phase1 / phase2 / phase3.
  *          phase3 is reserved as a placeholder for future use.
  ******************************************************************************
  */

#include "app_DGT.h"
#include "board_io.h"
#include "app_digital_input.h"
#include "app_rs485.h"
#include "app_WeconHMI.h"

/* External configuration ----------------------------------------------------*/
extern uint16_t g_realTime_h, g_realTime_m;
extern volatile uint32_t g_dgt_count;
extern volatile uint32_t g_delay_count;
extern volatile uint32_t g_Walk_count;
extern uint8_t I1_F, I2_F, I3_F, I4_F, I5_F;
extern uint8_t g_Light_Status;

/* Private variables ---------------------------------------------------------*/
static DGT_Phase_t     s_CurrentPhase = {0};

/* Private constants ---------------------------------------------------------*/
#define APP_DGT_TICK_PER_SECOND       (10000u)
#define APP_DGT_WALK_BLINK_ON_TIME    (4000u)
#define APP_DGT_YELLOW_BLINK_PERIOD   (20000u)
#define APP_DGT_YELLOW_BLINK_ON_TIME  (10000u)

/* Private types -------------------------------------------------------------*/
typedef struct
{
    uint8_t red;   
    uint8_t yellow;
    uint8_t green; 
} AppDGT2P_PhaseOutputStatus_t;

typedef struct
{
    AppDGT2P_PhaseOutputStatus_t phase1;
    AppDGT2P_PhaseOutputStatus_t phase2;
    AppDGT2P_PhaseOutputStatus_t phase3;
} AppDGT2P_OutputStatus_t;
typedef struct
{
    uint16_t x1;
    uint16_t x2;
    uint16_t x3;
    uint16_t v1;
    uint16_t v2;
    uint16_t v3;
    uint16_t gt1;
    uint16_t gt2;
    uint16_t gt3;
} AppDGT_TimingSource_t;

/* Private function prototypes ----------------------------------------------*/
static uint32_t AppDGT_ToTick(uint8_t second);
static uint16_t AppDGT_ToMinute(uint8_t hour, uint8_t minute);
static uint8_t AppDGT_IsTimeInRange(uint16_t now, uint16_t begin, uint16_t end);
static uint8_t AppDGT_IsPeakTimeActive(DGT_OnTime_t source);
static void AppDGT_UpdateTimingByPeakTime(DGT_Settings_t source, DGT_Phase_t *phase);
static void AppDGT2P_UpdateRegBank(const AppDGT2P_OutputStatus_t *status);
static void AppDGT2P_SetDirection1Green(void);
static void AppDGT2P_SetDirection1Yellow(uint8_t walkBlinkOn);
static void AppDGT2P_SetDirection1Clear(void);
static void AppDGT2P_SetDirection2Green(void);
static void AppDGT2P_SetDirection2Yellow(uint8_t walkBlinkOn);
static void AppDGT2P_SetDirection2Clear(void);

/* Private functions ---------------------------------------------------------*/
static uint32_t AppDGT_ToTick(uint8_t second)
{
    return (second * APP_DGT_TICK_PER_SECOND);
}

static uint16_t AppDGT_ToMinute(uint8_t hour, uint8_t minute)
{
    return (uint16_t)(((uint16_t)hour * 60u) + minute);
}

static uint8_t AppDGT_IsTimeInRange(uint16_t now, uint16_t begin, uint16_t end)
{
    if (begin <= end)
    {
        return (uint8_t)((now >= begin) && (now < end));
    }

    /* Time range crosses midnight. Example: 22:00 -> 05:00. */
    return (uint8_t)((now >= begin) || (now < end));
}

static uint8_t AppDGT_IsPeakTimeActive(DGT_OnTime_t source)
{
    uint8_t ret = 0u;
    const uint16_t now = AppDGT_ToMinute((uint8_t)g_realTime_h,
                                         (uint8_t)g_realTime_m);

    if (g_dgt_count > 50000) // 5s
    {
        return 0u;
    }
    
    if (source.flag == 1u){
        ret = AppDGT_IsTimeInRange(now,
                                   AppDGT_ToMinute(source.start_h, source.start_m),
                                   AppDGT_ToMinute(source.end_h, source.end_m));
        }

    return (uint8_t)(ret != 0u);
}


static void AppDGT_UpdateTimingByPeakTime(DGT_Settings_t source, DGT_Phase_t *phase)
{
    if (g_dgt_count >= 50000) // 5s
    {
        return;
    }
    if (AppDGT_IsPeakTimeActive(source.peak1_OnTime) != 0u)
    {
        *phase = source.peak1;
        return;
    }
    if (AppDGT_IsPeakTimeActive(source.peak2_OnTime) != 0u)
    {
        *phase = source.peak2;
        return;
    }
    *phase = source.normal;
}

static void AppDGT2P_UpdateRegBank(const AppDGT2P_OutputStatus_t *status)
{
    uint16_t (*regbank)[APP_RS485_REG_PER_GROUP] = AppRs485_GetRegBank();

    regbank[0][AppWeconHMI_BIT_X1] = status->phase1.green;
    regbank[0][AppWeconHMI_BIT_V1] = status->phase1.yellow;
    regbank[0][AppWeconHMI_BIT_D1] = status->phase1.red;
    regbank[0][AppWeconHMI_BIT_X2] = status->phase2.green;
    regbank[0][AppWeconHMI_BIT_V2] = status->phase2.yellow;
    regbank[0][AppWeconHMI_BIT_D2] = status->phase2.red;
}

static void AppDGT2P_SetDirection1Green(void)
{
    const AppDGT2P_OutputStatus_t status = {
        .phase1 = { .red = 0u, .yellow = 0u, .green = 1u },
        .phase2 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase3 = { 0u },
    };

    D1_0; X1_1; V1_0; Ddb1_1;
    D2_1; X2_0; V2_0;
    Xdb1_0; Xdb2_1;

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection1Yellow(uint8_t walkBlinkOn)
{
    const AppDGT2P_OutputStatus_t status = {
        .phase1 = { .red = 0u, .yellow = 1u, .green = 0u },
        .phase2 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase3 = { 0u },
    };

    D1_0; X1_0; V1_1; Ddb1_1;
    D2_1; X2_0; V2_0;

    if (walkBlinkOn != 0u)
    {
        Xdb2_1;
    }
    else
    {
        Xdb2_0;
    }

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection1Clear(void)
{
    const AppDGT2P_OutputStatus_t status = {
        .phase1 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase2 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase3 = { 0u },
    };

    D1_1; X1_0; V1_0; Ddb1_1;
    D2_1; X2_0; V2_0; Ddb2_1;

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection2Green(void)
{
    const AppDGT2P_OutputStatus_t status = {
        .phase1 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase2 = { .red = 0u, .yellow = 0u, .green = 1u },
        .phase3 = { 0u },
    };

    D1_1; X1_0; V1_0;
    D2_0; X2_1; V2_0; Ddb2_1;
    Xdb1_1; Xdb2_0;

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection2Yellow(uint8_t walkBlinkOn)
{
    const AppDGT2P_OutputStatus_t status = {
        .phase1 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase2 = { .red = 0u, .yellow = 1u, .green = 0u },
        .phase3 = { 0u },
    };

    D1_1; X1_0; V1_0;
    D2_0; X2_0; V2_1; Ddb2_1;

    if (walkBlinkOn != 0u)
    {
        Xdb1_1;
    }
    else
    {
        Xdb1_0;
    }

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection2Clear(void)
{
    const AppDGT2P_OutputStatus_t status = {
        .phase1 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase2 = { .red = 1u, .yellow = 0u, .green = 0u },
        .phase3 = { 0u },
    };

    D1_1; X1_0; V1_0; Ddb1_1;
    D2_1; X2_0; V2_0; Ddb2_1;

    AppDGT2P_UpdateRegBank(&status);
}

/* Public functions ----------------------------------------------------------*/
uint8_t AppDGT_BlinkYellowAutoCalculate(DGT_Settings_t s)
{
    uint8_t ret1 = 0u;
    uint8_t ret2 = 0u;
    uint8_t ret3;
    const uint16_t now = AppDGT_ToMinute((uint8_t)g_realTime_h,
                                         (uint8_t)g_realTime_m);

    if (s.BlinkYel1_OnTime.flag != 0u)
    {
        ret1 = AppDGT_IsTimeInRange(now,
                                    AppDGT_ToMinute(s.BlinkYel1_OnTime.start_h, s.BlinkYel1_OnTime.start_m),
                                    AppDGT_ToMinute(s.BlinkYel1_OnTime.end_h, s.BlinkYel1_OnTime.end_m));
    }

    if (s.BlinkYel2_OnTime.flag != 0u)
    {
        ret2 = AppDGT_IsTimeInRange(now,
                                    AppDGT_ToMinute(s.BlinkYel2_OnTime.start_h, s.BlinkYel2_OnTime.start_m),
                                    AppDGT_ToMinute(s.BlinkYel2_OnTime.end_h, s.BlinkYel2_OnTime.end_m));
    }

    ret3 = (uint8_t)(s.Thaco_Blink_flag & 0x01u);

    return (uint8_t)((ret1 | ret2 | ret3) != 0u);
}

void AppDGT2P_Manual(void)
{
    SL_0;

    if (I3_F != 0u)
    {
        g_Light_Status = 10u;
        AppDGT2P_SetDirection1Green();
    }
    else
    {
        g_Light_Status = 11u;
        AppDGT2P_SetDirection2Green();
    }
}

void AppDGT2P_BlinkYellow(void)
{
    const uint8_t blinkOn = (uint8_t)(((g_delay_count % APP_DGT_YELLOW_BLINK_PERIOD)
                                      <= APP_DGT_YELLOW_BLINK_ON_TIME) != 0u);
    AppDGT2P_OutputStatus_t status = { 0u };

    g_Light_Status = 12u;

    D1_0; X1_0; Xdb1_0;
    D2_0; X2_0; Xdb2_0;
    SL_0;

    if (blinkOn != 0u)
    {
        V1_1;
        V2_1;
        status.phase1.yellow = 1u;
        status.phase2.yellow = 1u;
    }
    else
    {
        V1_0;
        V2_0;
    }

    AppDGT2P_UpdateRegBank(&status);
}

void AppDGT2P_Auto(DGT_Settings_t source_Settings, DGT_Phase_t source_currentPhase)
{
    uint32_t phaseEnd;
    uint8_t walkBlinkOn;
    static uint8_t BlinkYel_Auto = 0u;

    BlinkYel_Auto = AppDGT_BlinkYellowAutoCalculate(source_Settings);
    if (BlinkYel_Auto != 0u)
    {
        AppDGT2P_BlinkYellow();
        return;
    }

    SL_1;

    walkBlinkOn = (uint8_t)((g_Walk_count <= APP_DGT_WALK_BLINK_ON_TIME) != 0u);

    Xdb1_0;
    Xdb2_0;
    Ddb1_0;
    Ddb2_0;

    phaseEnd = AppDGT_ToTick(source_currentPhase.phase1.x);
    if (g_dgt_count <= phaseEnd)
    {
        g_Light_Status = 1u;
        AppDGT2P_SetDirection1Green();
        return;
    }

    phaseEnd += AppDGT_ToTick(source_currentPhase.phase1.v);
    if (g_dgt_count <= phaseEnd)
    {
        g_Light_Status = 2u;
        AppDGT2P_SetDirection1Yellow(walkBlinkOn);
        return;
    }

    phaseEnd += AppDGT_ToTick(source_currentPhase.phase1.gt);
    if (g_dgt_count <= phaseEnd)
    {
        g_Light_Status = 3u;
        AppDGT2P_SetDirection1Clear();
        return;
    }

    phaseEnd += AppDGT_ToTick(source_currentPhase.phase2.x);
    if (g_dgt_count <= phaseEnd)
    {
        g_Light_Status = 4u;
        AppDGT2P_SetDirection2Green();
        return;
    }

    phaseEnd += AppDGT_ToTick(source_currentPhase.phase2.v);
    if (g_dgt_count <= phaseEnd)
    {
        g_Light_Status = 5u;
        AppDGT2P_SetDirection2Yellow(walkBlinkOn);
        return;
    }

    phaseEnd += AppDGT_ToTick(source_currentPhase.phase2.gt);
    if (g_dgt_count <= phaseEnd)
    {
        g_Light_Status = 6u;
        AppDGT2P_SetDirection2Clear();
        return;
    }

    g_dgt_count = 0u;
}

void AppDGT2P_Process(DGT_Settings_t *source)
{
    AppDGT_UpdateTimingByPeakTime(*source, &s_CurrentPhase);
    
    #if (AppDGT_test == 1U)
        AppDGT2P_Auto(*source, s_CurrentPhase);
    #else
        if ((I1_F == 1u) && (I2_F == 0u))
        {
            AppDGT2P_Auto(*source, s_CurrentPhase);
        }
        else if ((I1_F == 0u) && (I2_F == 0u))
        {
            AppDGT2P_Manual();
        }
        else if ((I1_F == 0u) && (I2_F == 1u))
        {
            AppDGT2P_BlinkYellow();
        }
        else
        {
            AppDGT2P_BlinkYellow();
        }
    #endif
    
        
    
    
}
