/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "functions.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int selection = 0, selectionLastState, gainLastState, relayStatus, stab, stabLastState, fanSpd, fanSpdLastState, tempLastState;
float impedanceLastState;
char gainS[2] = "1", impS[20] = "0.0", relayStatusS[5] = "1", stabS[5] = "OFF", fanSpdS[5] = "0", temp_cS[10] = "0";
volatile uint8_t temp_c;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (selection == 0){
	  gain = countGain(htim);
	  if (gain != gainLastState){
		  sprintf(gainS, "%d", gain);
		  relayStatus = amplify(gain);
		  sprintf(relayStatusS, "%d", relayStatus);
		  print(gainS, impS, relayStatusS, stabS, fanSpdS, selection, temp_cS);
		  gainLastState = gain;
		  }
	  }
	  else if (selection == 1){
		  impedance = countImpedance(htim);
		  sprintf(impS, "%.1f", impedance);
		  if(impedance != impedanceLastState){
			  outImp(impedance);
			  print(gainS, impS, relayStatusS, stabS, fanSpdS, selection, temp_cS);
			  impedanceLastState = impedance;
		  }
	  }
	  else{
		  stab = countStab(htim);
		  if (stab != stabLastState){
			  if (stab == 1){
				  strcpy(stabS, "ON");
				  HAL_GPIO_WritePin(stabilization_GPIO_Port, stabilization_Pin, GPIO_PIN_SET);
			  }
			  else{
				  strcpy(stabS, "OFF");
				  HAL_GPIO_WritePin(stabilization_GPIO_Port, stabilization_Pin, GPIO_PIN_RESET);
			  }
			  print(gainS, impS, relayStatusS, stabS, fanSpdS, selection, temp_cS);
			  stabLastState = stab;
		  }
	  }
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL); // Create interrupt for rotary encoder with timer2
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // Start pwm for fan with timer1
  if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
  {
    /* Starting Error */
    Error_Handler();
  }

  ssd1306_Init(); //Screen initialization

  ssd1306_Fill(White);
  ssd1306_DrawBitmap(0, 0, afry_128x64, 128, 64, Black); // Show AFRY logo for 3 seconds
  ssd1306_UpdateScreen(); // Send picture to screen
  HAL_Delay(3000);
  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Turn knob", Font_11x18, White);
  ssd1306_SetCursor(0, 19);
  ssd1306_WriteString("to start", Font_11x18, White);
  ssd1306_UpdateScreen();
  // Screen now shows "Turn knob to start" and the program wont proceed
  // until user has triggered an interrupt with the knob

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (temp_c != tempLastState && temp_c < 150){
		  fanSpd = fanCtrl(&htim1, temp_c);
		  sprintf(temp_cS, "%u C", temp_c);
		  sprintf(fanSpdS, "%d", fanSpd);
		  print(gainS, impS, relayStatusS, stabS, fanSpdS, selection, temp_cS);
		  tempLastState = temp_c;
	  }
	  else if (temp_c != tempLastState && temp_c >= 150){
		  fanCtrl(&htim1, temp_c);
		  tempWarning(temp_c);
		  tempLastState = temp_c;
	  }


	  selection = pbSelection();
	  if (selection != selectionLastState){
		  print(gainS, impS, relayStatusS, stabS, fanSpdS, selection, temp_cS);
		  selectionLastState = selection;
	  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
