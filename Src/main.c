/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "board_io.h"
#include "app_digital_input.h"
#include "app_rs485.h"
#include "app_DGT.h"
#include "app_I2CvsESP32.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	#define ESP32_ADDR 0x12
	#define HMI_ID 10	// ID for HMI RS485 deceive
	#define FLASH_ADD0   0x08020080
	#define FLASH_ADD1   0x08020084
	#define FLASH_ADD2   0x08020088
	#define FLASH_ADD3   0x0802008C
	#define FLASH_ADD4   0x08020090
	#define FLASH_ADD5   0x08020094
	#define FLASH_ADD6   0x08020098
	#define FLASH_ADD7   0x0802009C
	#define FLASH_ADD8   0x080200A0
	#define FLASH_ADD9	 0x080200A4
	
	#define I2C_FRAME_SIZE 22U
	#define I2C_FRAME_START '{'
	#define I2C_FRAME_END   '}'

	#define TIME_2_5S_COUNT   25000u   // 2.5s / 0.1ms
	#define TIME_5S_COUNT     50000u   // 5s / 0.1ms
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart3;

osThreadId MainTaskHandle;
osThreadId ConnectivityTasHandle;
/* USER CODE BEGIN PV */

// I2C variables
uint8_t txData[22] = "";
uint8_t I2C_Count = 150;

// RTC variables
    RTC_TimeTypeDef setTime = {0};
    RTC_DateTypeDef setDate = {0};
    RTC_TimeTypeDef getTime = {0};
    RTC_DateTypeDef getDate = {0};

// DGT variables 
	uint16_t input_ok = 0, ResetRTC_count = 0, HMI_initValue_count = 0;
	uint16_t X1, V1, GT1, D1; 
	uint16_t X2, V2, GT2, D2;
	uint16_t X3, V3, GT3, D3;
	uint16_t Xanh1=5, Vang1=3, GiaiToa1=2, Do1=22; 
	uint16_t Xanh2=5, Vang2=3, GiaiToa2=2, Do2=22;
	uint16_t Xanh3=5, Vang3=3, GiaiToa3=2, Do3=22;
	uint16_t CaoDiem_X1=5, CaoDiem_V1=3, CaoDiem_GT1=2, CaoDiem_D1=22; 
	uint16_t CaoDiem_X2=5, CaoDiem_V2=3, CaoDiem_GT2=2, CaoDiem_D2=22;
	uint16_t CaoDiem_X3=5, CaoDiem_V3=3, CaoDiem_GT3=2, CaoDiem_D3=22;
	volatile uint32_t t_count = 0, delay_count = 0;
	uint32_t Total_Time = 0, CaoDiem_Total_Time, Dummy_1;
	uint8_t BlinkYel_ENA1 = 1, BlinkYel_ENA2 = 0, Thaco_Blink = 0, CaoDiem_ENA;
  uint8_t BlinkYel_Auto = 0;
	uint8_t begin_hour1 = 22, end_hour1 = 5, begin_min1 = 0, end_min1 = 0; 	// Chop vang 1
	uint8_t begin_hour2 = 0 , end_hour2 = 0, begin_min2 = 0, end_min2 = 0;	// chop vang 2
	uint8_t begin_hour3 = 0 , end_hour3 = 0, begin_min3 = 0, end_min3 = 0;	// Cao diem
	uint16_t min_realTime = 0, hour_realTime = 0;
  uint32_t Walk_count;
  uint8_t Light_Status = 0;
  uint8_t Voltage1 = 0, Voltage2 = 0;
  uint8_t Current1 = 0, Current2 = 0;
	
// DAC variables
	float dac_voltage = 1.5f;
	uint32_t dac_value;
	const uint32_t DAC_MAXDIGITALVALUE_12BIT = 0xfff;
	const uint32_t DAC_MAXDIGITALVALUE_8BIT  = 0xff;
	const float Vref = 3.3f;
	
	
	
// input filer variables
static volatile uint8_t tim7_1ms_div = 0u;
uint8_t I1_F, I2_F, I3_F, I4_F, I5_F;

// Flash ROM variables
uint32_t myFlashData[10] = {0}; //24 
	

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM7_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_DAC_Init(void);
static void MX_USART3_UART_Init(void);
void StartMainTask(void const * argument);
void StartConnectivityTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t Read_Flash(uint32_t Data_adr)
	{
		return (*(__IO uint32_t *)Data_adr);
	}
void Write_Flash(uint32_t adr_Flash, uint32_t adr_Data)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,adr_Flash ,adr_Data);	
	}

static void Flash_GetData(uint32_t start_addr, uint32_t *buffer, uint8_t length){ // Read data to flash memory
    for (uint8_t i = 0; i < length; i++)
    {
        buffer[i] = Read_Flash(start_addr + i * 4);
    }
}
static void Flash_WriteData(uint32_t start_addr, uint32_t *buffer, uint8_t length){ // Write data to flash memory
    HAL_FLASH_Unlock();
	FLASH_Erase_Sector(FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE_3);
	for (uint8_t i = 0; i < length; i++)
    {
        Write_Flash((start_addr + i * 4), buffer[i]);
    }
	HAL_FLASH_Lock();
}
static void LoadSettings_from_Flash(){
		Xanh2			= (uint8_t) (myFlashData[0]      &0x000000ff);
		GiaiToa1		= (uint8_t) (myFlashData[0] >>8  &0x000000ff);
		Vang1			= (uint8_t) (myFlashData[0] >>16 &0x000000ff);
		Xanh1			= (uint8_t) (myFlashData[0] >>24 &0x000000ff);
		Vang3			= (uint8_t) (myFlashData[1]      &0x000000ff);
		Xanh3			= (uint8_t) (myFlashData[1] >>8  &0x000000ff);
		GiaiToa2		= (uint8_t) (myFlashData[1] >>16 &0x000000ff);
		Vang2			= (uint8_t) (myFlashData[1] >>24 &0x000000ff);
		end_hour1		= (uint8_t) (myFlashData[2]      &0x000000ff);
		begin_min1		= (uint8_t) (myFlashData[2] >>8  &0x000000ff);
		begin_hour1 	= (uint8_t) (myFlashData[2] >>16 &0x000000ff);
		GT3 			= (uint8_t) (myFlashData[2] >>24 &0x000000ff);
		Dummy_1			= (uint8_t) (myFlashData[3]      &0x000000ff);
		Dummy_1			= (uint8_t) (myFlashData[3] >>8  &0x000000ff);
		BlinkYel_ENA1	= (uint8_t) (myFlashData[3] >>16 &0x000000ff);
		end_min1		= (uint8_t) (myFlashData[3] >>24 &0x000000ff);
		// Cao diem
		CaoDiem_X2  	= (uint8_t) (myFlashData[4]      &0x000000ff);
		CaoDiem_GT1 	= (uint8_t) (myFlashData[4] >>8  &0x000000ff);
		CaoDiem_V1  	= (uint8_t) (myFlashData[4] >>16 &0x000000ff);
		CaoDiem_X1  	= (uint8_t) (myFlashData[4] >>24 &0x000000ff);
		CaoDiem_V3  	= (uint8_t) (myFlashData[5]      &0x000000ff);
		CaoDiem_X3  	= (uint8_t) (myFlashData[5] >>8  &0x000000ff);
		CaoDiem_GT2 	= (uint8_t) (myFlashData[5] >>16 &0x000000ff);
		CaoDiem_V2  	= (uint8_t) (myFlashData[5] >>24 &0x000000ff);
		end_hour3   	= (uint8_t) (myFlashData[6]      &0x000000ff);
		begin_min3  	= (uint8_t) (myFlashData[6] >>8  &0x000000ff);
		begin_hour3 	= (uint8_t) (myFlashData[6] >>16 &0x000000ff);
		CaoDiem_GT3 	= (uint8_t) (myFlashData[6] >>24 &0x000000ff);
		Dummy_1			= (uint8_t) (myFlashData[7]      &0x000000ff);
		Dummy_1			= (uint8_t) (myFlashData[7] >>8  &0x000000ff);
		CaoDiem_ENA 	= (uint8_t) (myFlashData[7] >>16 &0x000000ff);
		end_min3 		= (uint8_t) (myFlashData[7] >>24 &0x000000ff);
		end_min2		= (uint8_t) (myFlashData[8]      &0x000000ff);
		end_hour2		= (uint8_t) (myFlashData[8] >>8  &0x000000ff);
		begin_min2		= (uint8_t) (myFlashData[8] >>16 &0x000000ff);
		begin_hour2		= (uint8_t) (myFlashData[8] >>24 &0x000000ff);
		Thaco_Blink		= (uint8_t) (myFlashData[9]      &0x000000ff);
		Dummy_1			= (uint8_t) (myFlashData[9] >>8  &0x000000ff);
		Dummy_1			= (uint8_t) (myFlashData[9] >>16 &0x000000ff);
		BlinkYel_ENA2	= (uint8_t) (myFlashData[9] >>24 &0x000000ff);
		
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM7_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_DAC_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	// common
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
	HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
	// RS485
	AppRs485_Config_t rs485_cfg = {
		.huart    = &huart3,
		.dir_port = RS485_DIR_GPIO_Port,
		.dir_pin  = RS485_DIR_Pin,
		.slave_id = HMI_ID,
	};
  // I2c
  AppI2CvsESP32_Config_t i2c_esp32_cfg = {
    .hi2c = &hi2c1,
    .slave_addr_7bit = APP_I2CVSESP32_ADDR_7BIT,
    .timeout_ms = 100U,
};
	// Read from flash
	Flash_GetData(FLASH_ADD0, &myFlashData[0], 10);
	LoadSettings_from_Flash();
	HAL_Delay(10);
	// Ethernet
	ETH_PWR_0;
	// App
	AppDigitalInput_Init();
	AppRs485_Init(&rs485_cfg);
  AppI2CvsESP32_Init(&i2c_esp32_cfg);
	
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of MainTask */
  osThreadDef(MainTask, StartMainTask, osPriorityNormal, 0, 128);
  MainTaskHandle = osThreadCreate(osThread(MainTask), NULL);

  /* definition and creation of ConnectivityTas */
  osThreadDef(ConnectivityTas, StartConnectivityTask, osPriorityIdle, 0, 128);
  ConnectivityTasHandle = osThreadCreate(osThread(ConnectivityTas), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */

  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT2 config
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
uint32_t bkp = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
  if (bkp != 0x12) {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
  sDate.Month = RTC_MONTH_APRIL;
  sDate.Date = 0x14;
  sDate.Year = 0x26;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x12);
  }

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 0;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 8400;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, X2_Pin|V2_Pin|D2_Pin|Xdb2_Pin
                          |Ddb2_Pin|Xdb1_Pin|Ddb1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, X3_Pin|V3_Pin|D3_Pin|Xdb3_Pin
                          |LED_3_Pin|LED_2_Pin|LED_1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Ddb3_GPIO_Port, Ddb3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ETH_PWR_Pin|RS485_DIR_Pin|SL_Pin|X1_Pin
                          |V1_Pin|D1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_4_GPIO_Port, LED_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : X2_Pin V2_Pin D2_Pin Xdb2_Pin
                           Ddb2_Pin Xdb1_Pin Ddb1_Pin */
  GPIO_InitStruct.Pin = X2_Pin|V2_Pin|D2_Pin|Xdb2_Pin
                          |Ddb2_Pin|Xdb1_Pin|Ddb1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : X3_Pin V3_Pin D3_Pin Xdb3_Pin
                           LED_3_Pin LED_2_Pin LED_1_Pin */
  GPIO_InitStruct.Pin = X3_Pin|V3_Pin|D3_Pin|Xdb3_Pin
                          |LED_3_Pin|LED_2_Pin|LED_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : Ddb3_Pin */
  GPIO_InitStruct.Pin = Ddb3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Ddb3_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA3 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PC4 PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : ETH_PWR_Pin RS485_DIR_Pin SL_Pin X1_Pin
                           V1_Pin D1_Pin */
  GPIO_InitStruct.Pin = ETH_PWR_Pin|RS485_DIR_Pin|SL_Pin|X1_Pin
                          |V1_Pin|D1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB11 PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : I1_Pin I4_Pin I3_Pin */
  GPIO_InitStruct.Pin = I1_Pin|I4_Pin|I3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_4_Pin */
  GPIO_InitStruct.Pin = LED_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_4_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : I5_Pin */
  GPIO_InitStruct.Pin = I5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(I5_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PD5 PD6 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : ESP32_IO_Pin */
  GPIO_InitStruct.Pin = ESP32_IO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ESP32_IO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : I2_Pin */
  GPIO_InitStruct.Pin = I2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(I2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	AppRs485_RxCpltCallback(huart);
}

static void I2C_SendFrame(void)
{
    AppI2CvsESP32_Payload_t payload;

    /* Frame 1: Realtime */
    payload.realtime.date   = (uint8_t)getDate.Date;
    payload.realtime.month  = (uint8_t)getDate.Month;
    payload.realtime.year   = (uint8_t)getDate.Year;      /* 2026 -> 26 */

    payload.realtime.hour   = (uint8_t)getTime.Hours;
    payload.realtime.minute = (uint8_t)getTime.Minutes;
    payload.realtime.second = (uint8_t)getTime.Seconds;

    payload.realtime.voltage1 = (uint8_t)Voltage1;
    payload.realtime.voltage2 = (uint8_t)Voltage2;
    payload.realtime.current1 = (uint8_t)Current1;
    payload.realtime.current2 = (uint8_t)Current2;

    payload.realtime.begin_hour1 = (uint8_t)begin_hour1;
    payload.realtime.begin_min1  = (uint8_t)begin_min1;
    payload.realtime.end_hour1   = (uint8_t)end_hour1;
    payload.realtime.end_min1    = (uint8_t)end_min1;

    /* Frame 2: Normal setting + control flags */
    payload.normal.control_flags =
        AppI2CvsESP32_MakeControlFlags((uint8_t)BlinkYel_ENA1,
                                        (uint8_t)BlinkYel_ENA2,
                                        (uint8_t)Thaco_Blink,
                                        (uint8_t)CaoDiem_ENA);

    payload.normal.x1  = (uint8_t)Xanh1;
    payload.normal.v1  = (uint8_t)Vang1;
    payload.normal.gt1 = (uint8_t)GiaiToa1;

    payload.normal.x2  = (uint8_t)Xanh2;
    payload.normal.v2  = (uint8_t)Vang2;
    payload.normal.gt2 = (uint8_t)GiaiToa2;

    payload.normal.x3  = (uint8_t)Xanh3;
    payload.normal.v3  = (uint8_t)Vang3;
    payload.normal.gt3 = (uint8_t)GiaiToa3;

    payload.normal.begin_hour2 = (uint8_t)begin_hour2;
    payload.normal.begin_min2  = (uint8_t)begin_min2;
    payload.normal.end_hour2   = (uint8_t)end_hour2;
    payload.normal.end_min2    = (uint8_t)end_min2;

    /* Frame 3: Peak setting */
    payload.peak.begin_hour3 = (uint8_t)begin_hour3;
    payload.peak.begin_min3  = (uint8_t)begin_min3;
    payload.peak.end_hour3   = (uint8_t)end_hour3;
    payload.peak.end_min3    = (uint8_t)end_min3;

    payload.peak.peak_x1  = (uint8_t)CaoDiem_X1;
    payload.peak.peak_v1  = (uint8_t)CaoDiem_V1;
    payload.peak.peak_gt1 = (uint8_t)CaoDiem_GT1;

    payload.peak.peak_x2  = (uint8_t)CaoDiem_X2;
    payload.peak.peak_v2  = (uint8_t)CaoDiem_V2;
    payload.peak.peak_gt2 = (uint8_t)CaoDiem_GT2;

    payload.peak.peak_x3  = (uint8_t)CaoDiem_X3;
    payload.peak.peak_v3  = (uint8_t)CaoDiem_V3;
    payload.peak.peak_gt3 = (uint8_t)CaoDiem_GT3;

    (void)AppI2CvsESP32_SendAll(&payload);
}

static void RTC_SetDateTime(void){ // SET RTC
    /* Set Time: 14:25:30 */
    setTime.Hours = 14;
    setTime.Minutes = 25;
    setTime.Seconds = 30;
    setTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    setTime.StoreOperation = RTC_STOREOPERATION_RESET;
	HAL_RTC_SetTime(&hrtc, &setTime, RTC_FORMAT_BIN);

    /* Set Date: Monday, 15/04/2026 */
    setDate.WeekDay = RTC_WEEKDAY_MONDAY;
    setDate.Month   = RTC_MONTH_APRIL;
    setDate.Date    = 13;
    setDate.Year    = 26;   // chỉ lưu 2 số cuối: 2026 -> 26
	HAL_RTC_SetDate(&hrtc, &setDate, RTC_FORMAT_BIN);
	
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x12);
}

static void RTC_GetDateTime(void){ // Read RTC
    HAL_RTC_GetTime(&hrtc, &getTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &getDate, RTC_FORMAT_BIN);
}
static uint8_t Check_Time_Event(uint32_t period_count, uint32_t *time_prev){
    uint32_t time_now = delay_count;

    if ((uint32_t)(time_now - *time_prev) >= period_count)
    {
        *time_prev += period_count;
        return 1u;
    }

    return 0u;
}

static uint8_t Check_2_5s_Event(void){
    static uint32_t time_prev = 0u;

    return Check_Time_Event(TIME_2_5S_COUNT, &time_prev);
}

static uint8_t Check_5s_Event(void){
    static uint32_t time_prev = 0u;

    return Check_Time_Event(TIME_5S_COUNT, &time_prev);
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the MainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void const * argument)
{
  /* USER CODE BEGIN 5 */
	static uint8_t blink_state = 0u;
  /* Infinite loop */
  for(;;)
  {
	// Get Digital input status
	I1_F = AppDigitalInput_Check(DIGITAL_INPUT_SWITCH_1);
	I2_F = AppDigitalInput_Check(DIGITAL_INPUT_SWITCH_2);
	I3_F = AppDigitalInput_Check(DIGITAL_INPUT_SWITCH_3);
	I4_F = AppDigitalInput_Check(DIGITAL_INPUT_SWITCH_4);
	I5_F = AppDigitalInput_Check(DIGITAL_INPUT_SWITCH_5);
	
	if (dac_voltage>9.9) dac_voltage = 9.9f;
	dac_value = (uint32_t)((dac_voltage/3.0f) * 4095.0f / Vref); // dac_voltage 0..10V
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dac_value);
	HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dac_value);

  AppDGT2P_Process();

    osDelay(10);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartConnectivityTask */
/**
* @brief Function implementing the ConnectivityTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartConnectivityTask */
void StartConnectivityTask(void const * argument)
{
  /* USER CODE BEGIN StartConnectivityTask */
	uint16_t (*RS485_regbank)[APP_RS485_REG_PER_GROUP] = AppRs485_GetRegBank();

  /* Infinite loop */
  for(;;)
  {
	/////////////RESET WathDog timer//////////////////////////////
	//IWDG->KR = 0xAAAA; // Reload Counter prevents watchdog reset
	//////////////////////////////////////////////////////////////
	HAL_RTC_GetTime(&hrtc, &getTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &getDate, RTC_FORMAT_BIN);
  min_realTime  = (uint16_t)getTime.Minutes;
  hour_realTime = (uint16_t)getTime.Hours;
	
	if (Check_5s_Event()){ // I2C_SendFrame each 5s
		I2C_SendFrame();
    }
	
	//////////////////////////////////////////////////////////////
	AppRs485_Task();
	//////////////////////////////////////////////////////////////
	
	if (HMI_initValue_count<=1200){// Init value to HMI
		HMI_initValue_count++;
		//////
		RS485_regbank[2][120] = Xanh1;		// xanh 1
		RS485_regbank[2][121] = Vang1;		// vang 1
		RS485_regbank[2][122] = GiaiToa1;	// giai toa 1			
		RS485_regbank[2][123] = Xanh2;		// xanh 2 
		RS485_regbank[2][124] = Vang2;		// vang 2
		RS485_regbank[2][125] = GiaiToa2;	// giai toa 2				
		// RS485_regbank[2][142] = Xanh3 ;	// xanh 3
		// RS485_regbank[2][143] = Vang3 ;	// vang 3
		// RS485_regbank[2][144] = GiaiToa3;// giai toa 3
		RS485_regbank[2][126] = begin_hour1;
		RS485_regbank[2][127] = begin_min1;
		RS485_regbank[2][140] = end_hour1;
		RS485_regbank[2][141] = end_min1;
		RS485_regbank[0][30]  = BlinkYel_ENA1;
		Total_Time = X1 + V1 + GT1 + GT2;
		//////
		RS485_regbank[2][75] = CaoDiem_X1 ;	// xanh 1
		RS485_regbank[2][76] = CaoDiem_V1 ;	// vang 1
		RS485_regbank[2][77] = CaoDiem_GT1;	// giai toa 1			
		RS485_regbank[2][78] = CaoDiem_X2 ;	// xanh 2 
		RS485_regbank[2][79] = CaoDiem_V2 ;	// vang 2
		RS485_regbank[2][80] = CaoDiem_GT2;	// giai toa 2			
		// RS485_regbank[2][200] = CaoDiem_X3 ;// xanh 3
		// RS485_regbank[2][201] = CaoDiem_V3 ;// vang 3
		// RS485_regbank[2][202] = CaoDiem_GT3;// giai toa 3
		RS485_regbank[2][81] = begin_hour3 ;
		RS485_regbank[2][82] = begin_min3	;
		RS485_regbank[2][90] = end_hour3	;
		RS485_regbank[2][91] = end_min3	;
		RS485_regbank[0][31] = CaoDiem_ENA ;
		CaoDiem_Total_Time = CaoDiem_X2 + CaoDiem_V2 + CaoDiem_GT1 + CaoDiem_GT2;
		//////
		RS485_regbank[2][203] = begin_hour2;
		RS485_regbank[2][204] = begin_min2;
		RS485_regbank[2][205] = end_hour2;
		RS485_regbank[2][206] = end_min2;
		RS485_regbank[0][32]  = BlinkYel_ENA2;
		//////
		RS485_regbank[0][34] = Thaco_Blink;
	}
	
	input_ok++;
	if (input_ok >=800){// reset thong bao nhap thoi gian ok
		RS485_regbank[0][11]=0;
		input_ok=0;
	}
	
	if (ResetRTC_count <= 60000) ResetRTC_count++;
	if ((RS485_regbank[0][97] == 1) && (ResetRTC_count == 0xFFFF)) ResetRTC_count = 0;
	if ((ResetRTC_count >= 59000) && (ResetRTC_count <= 60000)) {// reset input RTC variables after 1 minute, check HMI background script
		ResetRTC_count = 0xFFFF;
		RS485_regbank[2][58] = 0;
		RS485_regbank[2][59] = 0;
		RS485_regbank[2][70] = 0;
		RS485_regbank[2][71] = 0;
		RS485_regbank[2][72] = 0;
		RS485_regbank[2][73] = 0;
		RS485_regbank[2][74] = 0;
		RS485_regbank[0][97] = 0;
	}
			
// send Real time from RTC to HMI	
	RS485_regbank[2][52] = getTime.Hours;
	RS485_regbank[2][53] = getTime.Minutes;
	RS485_regbank[2][54] = getTime.Seconds;
	RS485_regbank[2][55] = getDate.Date;
	RS485_regbank[2][56] = getDate.Month;
	RS485_regbank[2][57] = getDate.Year;
	
	if (RS485_regbank[0][96] == 1){////// SET time to RTC
		setTime.Hours 	= RS485_regbank[2][58];
		setTime.Minutes = RS485_regbank[2][59];
		setTime.Seconds = RS485_regbank[2][70];
		setTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
		setTime.StoreOperation = RTC_STOREOPERATION_RESET;
		HAL_RTC_SetTime(&hrtc, &setTime, RTC_FORMAT_BIN);
		
		if (RS485_regbank[2][74]>1) setDate.WeekDay = RS485_regbank[2][74]-1;
		if (RS485_regbank[2][74]==1) setDate.WeekDay = RTC_WEEKDAY_SUNDAY;
		
		setDate.Month= RS485_regbank[2][72];
		setDate.Date = RS485_regbank[2][71];
		setDate.Year = RS485_regbank[2][73];   // chỉ lưu 2 số cuối: 2026 -> 26
		HAL_RTC_SetDate(&hrtc, &setDate, RTC_FORMAT_BIN);
		
		HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x12);
		// inform input_ok
		RS485_regbank[0][96] = 0;
		RS485_regbank[0][11] = 1;
		input_ok = 0;
		ResetRTC_count = 0;
	}

	if (RS485_regbank[0][94] == 1){////// SET Data (Setting 1) -- 14 byte -- Original Data
		Xanh1	 = RS485_regbank[2][120];	// xanh 1
		Vang1	 = RS485_regbank[2][121];	// vang 1
		GiaiToa1 = RS485_regbank[2][122];	// giai toa 1
						
		Xanh2	 = RS485_regbank[2][123];	// xanh 2 
		Vang2	 = RS485_regbank[2][124];	// vang 2
		GiaiToa2 = RS485_regbank[2][125];	// giai toa 2
						
//		X3 = RS485_regbank[2][142];	// xanh 3
//		V3 = RS485_regbank[2][143];	// vang 3
//		GT3= RS485_regbank[2][144];	// giai toa 3
		
		begin_hour1		= (uint8_t)RS485_regbank[2][126];
		begin_min1		= (uint8_t)RS485_regbank[2][127];
		end_hour1		= (uint8_t)RS485_regbank[2][140];
		end_min1		= (uint8_t)RS485_regbank[2][141];
		BlinkYel_ENA1	= (uint8_t)RS485_regbank[0][30];
		
		// Read from flash
		Flash_GetData(FLASH_ADD0, &myFlashData[0], 10);
		
		// Write to flash
		myFlashData[0] = RS485_regbank[2][120];							// xanh 1
		myFlashData[0] = (myFlashData[0]) << 8 | RS485_regbank[2][121];	// vang 1
		myFlashData[0] = (myFlashData[0]) << 8 | RS485_regbank[2][122];	// giai toa 1						
		myFlashData[0] = (myFlashData[0]) << 8 | RS485_regbank[2][123];	// xanh 2 
		myFlashData[1] = RS485_regbank[2][124];							// vang 2
		myFlashData[1] = (myFlashData[1]) << 8 | RS485_regbank[2][125];	// giai toa 2
		myFlashData[1] = (myFlashData[1]) << 8 | 0xDD;					// xanh 3
		myFlashData[1] = (myFlashData[1]) << 8 | 0xDD;					// vang 3
		myFlashData[2] = 0xDD;											// giai toa 3
		myFlashData[2] = (myFlashData[2]) << 8 | begin_hour1;			// begin_hour1
		myFlashData[2] = (myFlashData[2]) << 8 | begin_min1;			// begin_min1
		myFlashData[2] = (myFlashData[2]) << 8 | end_hour1;				// end_hour1
		myFlashData[3] = end_min1;										// end_min1
		myFlashData[3] = (myFlashData[3]) << 8 | BlinkYel_ENA1;			// BlinkYel_ENA1
		myFlashData[3] = (myFlashData[3]) << 8 | 0xEE;					// Dummy
		myFlashData[3] = (myFlashData[3]) << 8 | 0xEE;					// Dummy
		
		Flash_WriteData(FLASH_ADD0, &myFlashData[0], 10);
		
		Total_Time = X1 + V1 + GT1 + GT2;
		
		// inform input_ok
		RS485_regbank[0][94] = 0;
		RS485_regbank[0][11] = 1;
		input_ok = 0;
	}

	if (RS485_regbank[0][95] == 1){////// SET Data (Setting_3) -- 14 byte -- dung cho gio cao diem 
		CaoDiem_X1 = RS485_regbank[2][75];	// xanh 1
		CaoDiem_V1 = RS485_regbank[2][76];	// vang 1
		CaoDiem_GT1= RS485_regbank[2][77];	// giai toa 1
						
		CaoDiem_X2 = RS485_regbank[2][78];	// xanh 2 
		CaoDiem_V2 = RS485_regbank[2][79];	// vang 2
		CaoDiem_GT2= RS485_regbank[2][80];	// giai toa 2
						
		// CaoDiem_X3 = RS485_regbank[2][92];	// xanh 3
		// CaoDiem_V3 = RS485_regbank[2][93];	// vang 3
		// CaoDiem_GT3 = RS485_regbank[2][94];// giai toa 3
		
		begin_hour3 = (uint8_t)RS485_regbank[2][81];
		begin_min3	= (uint8_t)RS485_regbank[2][82];
		end_hour3	= (uint8_t)RS485_regbank[2][90];
		end_min3	= (uint8_t)RS485_regbank[2][91];
		CaoDiem_ENA = (uint8_t)RS485_regbank[0][31];
		
		// Read from flash
		Flash_GetData(FLASH_ADD0, &myFlashData[0], 10);
		// Write to flash
		myFlashData[4] = RS485_regbank[2][75];							// xanh 1
		myFlashData[4] = (myFlashData[4]) << 8 | RS485_regbank[2][76];	// vang 1
		myFlashData[4] = (myFlashData[4]) << 8 | RS485_regbank[2][77];	// giai toa 1						
		myFlashData[4] = (myFlashData[4]) << 8 | RS485_regbank[2][78];	// xanh 2 
		myFlashData[5] = RS485_regbank[2][79];							// vang 2
		myFlashData[5] = (myFlashData[5]) << 8 | RS485_regbank[2][80];	// giai toa 2
		myFlashData[5] = (myFlashData[5]) << 8 | 0xDD;					// xanh 3
		myFlashData[5] = (myFlashData[5]) << 8 | 0xDD;					// vang 3
		myFlashData[6] = 0xDD;											// giai toa 3
		myFlashData[6] = (myFlashData[6]) << 8 | begin_hour3;			// begin_hour1
		myFlashData[6] = (myFlashData[6]) << 8 | begin_min3;			// begin_min1
		myFlashData[6] = (myFlashData[6]) << 8 | end_hour3;				// end_hour1
		myFlashData[7] = end_min3;										// end_min1
		myFlashData[7] = (myFlashData[7]) << 8 | CaoDiem_ENA;			// CaoDiem_ENA
		myFlashData[7] = (myFlashData[7]) << 8 | 0xEE;					// Dummy
		myFlashData[7] = (myFlashData[7]) << 8 | 0xEE;					// Dummy
		
		Flash_WriteData(FLASH_ADD0, &myFlashData[0], 10);
		
		CaoDiem_Total_Time = CaoDiem_X2 + CaoDiem_V2 + CaoDiem_GT1 + CaoDiem_GT2;
		
		// inform input_ok
		RS485_regbank[0][95] = 0;
		RS485_regbank[0][11] = 1;
		input_ok = 0;
	}

	if (RS485_regbank[0][98] == 1){////// SET Data (Setting_2) -- 05 byte -- Chop vang 2
		begin_hour2 =	(uint8_t)RS485_regbank[2][203];
		begin_min2 =	(uint8_t)RS485_regbank[2][204];
		end_hour2 =		(uint8_t)RS485_regbank[2][205];
		end_min2 =		(uint8_t)RS485_regbank[2][206];
		BlinkYel_ENA2 = (uint8_t)RS485_regbank[0][32];
		
		// Read from flash
		Flash_GetData(FLASH_ADD0, &myFlashData[0], 10);
		// Write to flash
		myFlashData[8] = begin_hour2;		// begin_hour2
		myFlashData[8] = (myFlashData[8]) << 8 | begin_min2;	// begin_min2
		myFlashData[8] = (myFlashData[8]) << 8 | end_hour2;		// end_hour2
		myFlashData[8] = (myFlashData[8]) << 8 | end_min2;		// end_min2
		myFlashData[9] = BlinkYel_ENA2;	// BlinkYel_ENA2
		myFlashData[9] = (myFlashData[9]) << 8 | 0xEE;			// Dummy
		myFlashData[9] = (myFlashData[9]) << 8 | 0xEE;			// Dummy
		myFlashData[9] = (myFlashData[9]) << 8 | Thaco_Blink;// for Thaco_Blink
		
		Flash_WriteData(FLASH_ADD0, &myFlashData[0], 10);
		
		// inform input_ok
		RS485_regbank[0][98] = 0;
		RS485_regbank[0][11] = 1;
		input_ok = 0;
	}

	if (RS485_regbank[0][99] == 1){////// SET Data (Back Setting) -- 01 byte -- Thaco_Blink
		Thaco_Blink = (uint8_t)RS485_regbank[0][34];
		
		// Read from flash
		Flash_GetData(FLASH_ADD0, &myFlashData[0], 10);
		// Write to flash
		myFlashData[9] = (myFlashData[9] & 0xFFFFFFFE) | (Thaco_Blink & 0x01);		// for Thaco_Blink
		
		Flash_WriteData(FLASH_ADD0, &myFlashData[0], 10);
		
		// inform input_ok
		RS485_regbank[0][99] = 0;
		RS485_regbank[0][11] = 1;
		input_ok = 0;
	}
	
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
	
	
    osDelay(10);
  }
  /* USER CODE END StartConnectivityTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM14 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
if (htim->Instance == TIM7){// 0.1 ms interrupt
    t_count ++;
	// if (t_count > Total_Time) t_count = 0U; // reset this count
	
	delay_count++;// no need to reset this count

	tim7_1ms_div++;
	if (tim7_1ms_div >= 10u){// Read Digital input
        tim7_1ms_div = 0u;
        AppDigitalInput_1msTask();
    }

  Walk_count++;
  if (Walk_count >= 10000u){Walk_count = 0u;}
  }
  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
