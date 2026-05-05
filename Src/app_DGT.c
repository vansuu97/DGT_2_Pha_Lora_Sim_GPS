/**
  ******************************************************************************
  * @file    app_DGT.c
  * @author  Suu Nguyen Van - FPT Company 0971346938
  * @version V1.0
  * @date    05-05-2026
  * @brief   Traffic light control application.
  ******************************************************************************
  */

#include "app_DGT.h"
#include "board_io.h"
#include "app_digital_input.h"
#include "app_rs485.h"

/* External configuration ----------------------------------------------------*/
extern uint16_t X1, V1, GT1, D1;
extern uint16_t X2, V2, GT2, D2;
extern uint16_t X3, V3, GT3, D3;

extern uint16_t Xanh1, Vang1, GiaiToa1, Do1;
extern uint16_t Xanh2, Vang2, GiaiToa2, Do2;
extern uint16_t Xanh3, Vang3, GiaiToa3, Do3;

extern uint16_t CaoDiem_X1, CaoDiem_V1, CaoDiem_GT1, CaoDiem_D1;
extern uint16_t CaoDiem_X2, CaoDiem_V2, CaoDiem_GT2, CaoDiem_D2;
extern uint16_t CaoDiem_X3, CaoDiem_V3, CaoDiem_GT3, CaoDiem_D3;

extern uint8_t BlinkYel_ENA1, BlinkYel_ENA2, Thaco_Blink, CaoDiem_ENA;

extern uint8_t begin_hour1, begin_min1, end_hour1, end_min1; // Chop vang 1
extern uint8_t begin_hour2, begin_min2, end_hour2, end_min2; // chop vang 2
extern uint8_t begin_hour3, begin_min3, end_hour3, end_min3; // Cao diem

extern uint16_t hour_realTime, min_realTime;
extern volatile uint32_t t_count;
extern volatile uint32_t delay_count;

extern uint8_t I1_F, I2_F, I3_F, I4_F, I5_F;
extern uint8_t BlinkYel_Auto;
extern uint8_t Light_Status;
extern uint32_t Walk_count;

/* Private constants ---------------------------------------------------------*/
#define APP_DGT_TICK_PER_SECOND       (10000u)
#define APP_DGT_WALK_BLINK_ON_TIME    (4000u)
#define APP_DGT_YELLOW_BLINK_PERIOD   (20000u)
#define APP_DGT_YELLOW_BLINK_ON_TIME  (10000u)

#define APP_DGT_REG_D1                (1u)
#define APP_DGT_REG_V1                (2u)
#define APP_DGT_REG_X1                (3u)
#define APP_DGT_REG_X2                (4u)
#define APP_DGT_REG_V2                (5u)
#define APP_DGT_REG_D2                (6u)

/* Private types -------------------------------------------------------------*/
typedef struct
{
    uint8_t d1;
    uint8_t v1;
    uint8_t x1;
    uint8_t x2;
    uint8_t v2;
    uint8_t d2;
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
static uint32_t AppDGT_ToTick(uint32_t second);
static uint16_t AppDGT_ToMinute(uint8_t hour, uint8_t minute);
static uint8_t AppDGT_IsTimeInRange(uint16_t now, uint16_t begin, uint16_t end);
static uint8_t AppDGT_IsPeakTimeActive(void);
static void AppDGT_ApplyTiming(const AppDGT_TimingSource_t *source);
static void AppDGT_UpdateTimingByPeakTime(void);
static void AppDGT2P_UpdateRegBank(const AppDGT2P_OutputStatus_t *status);
static void AppDGT2P_SetDirection1Green(void);
static void AppDGT2P_SetDirection1Yellow(uint8_t walkBlinkOn);
static void AppDGT2P_SetDirection1Clear(void);
static void AppDGT2P_SetDirection2Green(void);
static void AppDGT2P_SetDirection2Yellow(uint8_t walkBlinkOn);
static void AppDGT2P_SetDirection2Clear(void);

/* Private functions ---------------------------------------------------------*/
static uint32_t AppDGT_ToTick(uint32_t second)
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

static uint8_t AppDGT_IsPeakTimeActive(void)
{
    uint8_t ret = 0u;
    const uint16_t now = AppDGT_ToMinute((uint8_t)hour_realTime,
                                         (uint8_t)min_realTime);

    if (CaoDiem_ENA == 1u)
    {
        ret = AppDGT_IsTimeInRange(now,
                                   AppDGT_ToMinute(begin_hour3, begin_min3),
                                   AppDGT_ToMinute(end_hour3, end_min3));
    }

    return (uint8_t)(ret != 0u);
}

static void AppDGT_ApplyTiming(const AppDGT_TimingSource_t *source)
{
    X1 = source->x1;
    X2 = source->x2;
    X3 = source->x3;

    V1 = source->v1;
    V2 = source->v2;
    V3 = source->v3;

    GT1 = source->gt1;
    GT2 = source->gt2;
    GT3 = source->gt3;
}

static void AppDGT_UpdateTimingByPeakTime(void)
{
    const AppDGT_TimingSource_t normalTiming = {
        .x1  = Xanh1,
        .x2  = Xanh2,
        .x3  = Xanh3,
        .v1  = Vang1,
        .v2  = Vang2,
        .v3  = Vang3,
        .gt1 = GiaiToa1,
        .gt2 = GiaiToa2,
        .gt3 = GiaiToa3,
    };

    const AppDGT_TimingSource_t peakTiming = {
        .x1  = CaoDiem_X1,
        .x2  = CaoDiem_X2,
        .x3  = CaoDiem_X3,
        .v1  = CaoDiem_V1,
        .v2  = CaoDiem_V2,
        .v3  = CaoDiem_V3,
        .gt1 = CaoDiem_GT1,
        .gt2 = CaoDiem_GT2,
        .gt3 = CaoDiem_GT3,
    };

    if (AppDGT_IsPeakTimeActive() != 0u)
    {
        AppDGT_ApplyTiming(&peakTiming);
    }
    else
    {
        AppDGT_ApplyTiming(&normalTiming);
    }
}

static void AppDGT2P_UpdateRegBank(const AppDGT2P_OutputStatus_t *status)
{
    uint16_t (*regbank)[APP_RS485_REG_PER_GROUP] = AppRs485_GetRegBank();

    regbank[0][APP_DGT_REG_D1] = status->d1;
    regbank[0][APP_DGT_REG_V1] = status->v1;
    regbank[0][APP_DGT_REG_X1] = status->x1;
    regbank[0][APP_DGT_REG_X2] = status->x2;
    regbank[0][APP_DGT_REG_V2] = status->v2;
    regbank[0][APP_DGT_REG_D2] = status->d2;
}

static void AppDGT2P_SetDirection1Green(void)
{
    const AppDGT2P_OutputStatus_t status = {
        .d1 = 1u, .v1 = 0u, .x1 = 0u,
        .x2 = 0u, .v2 = 0u, .d2 = 1u,
    };

    D1_0; X1_1; V1_0;
    D2_1; X2_0; V2_0;
    Xdb1_0; Xdb2_1;

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection1Yellow(uint8_t walkBlinkOn)
{
    const AppDGT2P_OutputStatus_t status = {
        .d1 = 0u, .v1 = 1u, .x1 = 0u,
        .x2 = 0u, .v2 = 0u, .d2 = 1u,
    };

    D1_0; X1_0; V1_1;
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
        .d1 = 0u, .v1 = 0u, .x1 = 1u,
        .x2 = 0u, .v2 = 0u, .d2 = 1u,
    };

    D1_1; X1_0; V1_0;
    D2_1; X2_0; V2_0;

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection2Green(void)
{
    const AppDGT2P_OutputStatus_t status = {
        .d1 = 0u, .v1 = 0u, .x1 = 1u,
        .x2 = 1u, .v2 = 0u, .d2 = 0u,
    };

    D1_1; X1_0; V1_0;
    D2_0; X2_1; V2_0;
    Xdb1_1; Xdb2_0;

    AppDGT2P_UpdateRegBank(&status);
}

static void AppDGT2P_SetDirection2Yellow(uint8_t walkBlinkOn)
{
    const AppDGT2P_OutputStatus_t status = {
        .d1 = 0u, .v1 = 0u, .x1 = 1u,
        .x2 = 0u, .v2 = 1u, .d2 = 0u,
    };

    D1_1; X1_0; V1_0;
    D2_0; X2_0; V2_1;

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
        .d1 = 0u, .v1 = 0u, .x1 = 1u,
        .x2 = 0u, .v2 = 0u, .d2 = 1u,
    };

    D1_1; X1_0; V1_0;
    D2_1; X2_0; V2_0;

    AppDGT2P_UpdateRegBank(&status);
}

/* Public functions ----------------------------------------------------------*/
uint8_t AppDGT_BlinkYellowAutoCalculate(void)
{
    uint8_t ret1 = 0u;
    uint8_t ret2 = 0u;
    uint8_t ret3;
    const uint16_t now = AppDGT_ToMinute((uint8_t)hour_realTime,
                                         (uint8_t)min_realTime);

    if (BlinkYel_ENA1 != 0u)
    {
        ret1 = AppDGT_IsTimeInRange(now,
                                    AppDGT_ToMinute(begin_hour1, begin_min1),
                                    AppDGT_ToMinute(end_hour1, end_min1));
    }

    if (BlinkYel_ENA2 != 0u)
    {
        ret2 = AppDGT_IsTimeInRange(now,
                                    AppDGT_ToMinute(begin_hour2, begin_min2),
                                    AppDGT_ToMinute(end_hour2, end_min2));
    }

    ret3 = (uint8_t)(Thaco_Blink & 0x01u);

    return (uint8_t)((ret1 | ret2 | ret3) != 0u);
}

void AppDGT2P_Manual(void)
{
    SL_0;

    if (I3_F != 0u)
    {
        Light_Status = 10u;
        AppDGT2P_SetDirection1Green();
    }
    else
    {
        Light_Status = 11u;
        AppDGT2P_SetDirection2Green();
    }
}

void AppDGT2P_BlinkYellow(void)
{
    const uint8_t blinkOn = (uint8_t)(((delay_count % APP_DGT_YELLOW_BLINK_PERIOD)
                                      <= APP_DGT_YELLOW_BLINK_ON_TIME) != 0u);
    AppDGT2P_OutputStatus_t status = { 0u };

    Light_Status = 12u;

    D1_0; X1_0; Xdb1_0;
    D2_0; X2_0; Xdb2_0;
    SL_0;

    if (blinkOn != 0u)
    {
        V1_1;
        V2_1;
        status.v1 = 1u;
        status.v2 = 1u;
    }
    else
    {
        V1_0;
        V2_0;
    }

    AppDGT2P_UpdateRegBank(&status);
}

void AppDGT2P_Auto(void)
{
    uint32_t phaseEnd;
    uint8_t walkBlinkOn;

    BlinkYel_Auto = AppDGT_BlinkYellowAutoCalculate();
    if (BlinkYel_Auto != 0u)
    {
        AppDGT2P_BlinkYellow();
        return;
    }

    SL_1;

    walkBlinkOn = (uint8_t)((Walk_count <= APP_DGT_WALK_BLINK_ON_TIME) != 0u);

    Xdb1_0;
    Xdb2_0;

    phaseEnd = AppDGT_ToTick(X1);
    if (t_count <= phaseEnd)
    {
        Light_Status = 1u;
        AppDGT2P_SetDirection1Green();
        return;
    }

    phaseEnd += AppDGT_ToTick(V1);
    if (t_count <= phaseEnd)
    {
        Light_Status = 2u;
        AppDGT2P_SetDirection1Yellow(walkBlinkOn);
        return;
    }

    phaseEnd += AppDGT_ToTick(GT1);
    if (t_count <= phaseEnd)
    {
        Light_Status = 3u;
        AppDGT2P_SetDirection1Clear();
        return;
    }

    phaseEnd += AppDGT_ToTick(X2);
    if (t_count <= phaseEnd)
    {
        Light_Status = 4u;
        AppDGT2P_SetDirection2Green();
        return;
    }

    phaseEnd += AppDGT_ToTick(V2);
    if (t_count <= phaseEnd)
    {
        Light_Status = 5u;
        AppDGT2P_SetDirection2Yellow(walkBlinkOn);
        return;
    }

    phaseEnd += AppDGT_ToTick(GT2);
    if (t_count <= phaseEnd)
    {
        Light_Status = 6u;
        AppDGT2P_SetDirection2Clear();
        return;
    }

    t_count = 0u;
}

void AppDGT2P_Process(void)
{
    AppDGT_UpdateTimingByPeakTime();
    AppDGT2P_Auto();

    /*
    if ((I1_F == 1u) && (I2_F == 0u))
    {
        AppDGT2P_Auto();
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
    */
}
