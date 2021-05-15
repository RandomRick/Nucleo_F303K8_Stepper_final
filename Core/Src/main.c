/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>		// sprintf
#include <string.h>		// strlen
#include "statemachine.h"
#include "joystick.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim16;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */


char txBuf[TXBUFLEN];
unsigned int Iteration = 0;

char rxBuf[RXBUFLEN];
unsigned int rxBufIndex = 0;
char rxChar; // used for reading into RxBuf one character at a time

unsigned long ms_tick = 0;			// ms tick, used in TIM16 ISR

unsigned long deadZoneTick = 0;		// after joystick is a set time in the central posn, deactivate the stepper coils


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM16_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
  MX_USART2_UART_Init();
  MX_TIM16_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  JoystickInit(10, 0.5);
  StateMachineInit();				// put my state machine into a known state
  HAL_TIM_Base_Start_IT(&htim16);	// start the 1ms timer

  sprintf (txBuf, "Nucleo_F303K8_Stepper\r\n");
  HAL_UART_Transmit_IT (&huart2, (uint8_t *)txBuf, strlen (txBuf));

  HAL_UART_Receive_IT(&huart2, (uint8_t *)&rxChar, 1);	// kick off the reading
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //sprintf (txBuf, "Iteration %04d\r\n", Iteration);
	  Iteration++;
	  //HAL_UART_Transmit_IT (&huart2, (uint8_t*)txBuf, strlen (txBuf));
	  HAL_Delay(995);
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 16000-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Phase_A_Pin|Phase_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_Pin|Phase_D_Pin|Phase_C_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : Phase_A_Pin Phase_B_Pin */
  GPIO_InitStruct.Pin = Phase_A_Pin|Phase_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_Pin Phase_D_Pin Phase_C_Pin */
  GPIO_InitStruct.Pin = LED_Pin|Phase_D_Pin|Phase_C_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
{
/*
	sprintf (TxBuf, "Char = %02d\r\n", rxChar);
	HAL_UART_Transmit(&huart2, (uint8_t *)TxBuf, strlen (TxBuf), 999);
 */


	if (rxBufIndex == 0)
	{
		/* Quick commands that don't get put into rxBuf
		 * "1" - Left + start
		 * "2" - Right + start
		 * SPACE - toggle stop/start
		 */
		switch (rxChar)
		{
		case '1':
			State.Direction = ANTICLOCKWISE;
			State.RunState = RUNNING;
			break;
		case '2':
			State.Direction = CLOCKWISE;
			State.RunState = RUNNING;
			break;
		case ' ':
			if (State.RunState == RUNNING)
				State.RunState = STOPPED;
			else
				State.RunState = RUNNING;
			break;
		default:
			// not a quick command, so put it into the buffer
			*rxBuf = rxChar;
			rxBufIndex++;
			break;
		}
	}
	else
	{
#ifdef REJECTCR
		if (rxChar != '\r')
		{
			// check for message termination character
			if ((rxChar == '>') || (rxChar == '\n'))
#else
				if ((rxChar == '>') || (rxChar == '\n') || (rxChar == '\r'))
#endif
			{
				rxBuf[rxBufIndex] = (char)0;
				ParseUartCommand();
				__NOP(); // for breakpoint
				rxBufIndex=0;
			}
			else
			{
				rxBuf[rxBufIndex] = rxChar;
				// point to next character in buffer
				rxBufIndex++;

				//wrap if necessary
				if (rxBufIndex == RXBUFLEN)
				{
					rxBufIndex=0;
				}
			}
#ifdef REJECTCR
		}
#endif
	}
	// init next read
//	HAL_UART_Receive_IT(&huart2, (uint8_t *)&(rxBuf[rxBufIndex]), 1);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&rxChar, 1);
}


/***********************
 *                     *
 *  Timer 16 (1ms) ISR *
 *                     *
 ***********************/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim16)
	{
		ms_tick++;

		// time for an ADC reading from the Joystick?
		if (ms_tick % 100 == 0)
		{
			JoystickReadingFiltered();
			InterpretADC();
			//sprintf (txBuf, "joy = %f\r\n", joyval);
			//HAL_UART_Transmit(&huart2, (uint8_t *)txBuf, strlen (txBuf), HAL_MAX_DELAY);
		}

		// if joystick has been untouched for 5 seconds, deactivate stepper coils
		if (deadZoneTick == 50) // these ticks are every 100ms as per 9 lines up
		{
			State.RunState = STOPPED;
			//HAL_Delay(5);	// not sure it's needed, but, delays, right?

			// set all Stepper coils to low (no current anywhere)
			HAL_GPIO_WritePin(Phase_A_GPIO_Port, Phase_A_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Phase_B_GPIO_Port, Phase_B_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Phase_C_GPIO_Port, Phase_C_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Phase_D_GPIO_Port, Phase_D_Pin, GPIO_PIN_RESET);
		}

		// If runstate is stopped, do nothing
		if (State.RunState == STOPPED)
		{
			return;
		}

		// not stopped - so do something

		// time to pulse the motor?
		if (ms_tick % State.PulseDelay == 0)
		{
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
			//ms_tick=0;
			SetState(State.CurrentAngle & 0x07);

			// inc / dec the angle
			switch (State.Direction)
			{
			case CLOCKWISE:
				State.CurrentAngle++;
				if (State.CurrentAngle == 64*64)
				{
					State.CurrentAngle = 0;
				}
				break;
			case ANTICLOCKWISE:
				State.CurrentAngle--;
				if (State.CurrentAngle == -1)
				{
					State.CurrentAngle = 64*64-1;
				}
				break;
			default:
				break;
			}
		}
	}
}


/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
