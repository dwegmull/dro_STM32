/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "quadrature_decoder_LS7366R.h"

/** @addtogroup CORE
  * @{
  */

/** @defgroup MAIN
* @brief main file
* @{
*/ 

/** @defgroup MAIN_Private_TypesDefinitions
* @{
*/ 
#ifdef INCLUDE_THIRD_PARTY_MODULE 
typedef  void (*pFunc)(void);
#endif
/**
* @}
*/ 

/** @defgroup MAIN_Private_Defines
* @{
*/ 
/**
* @}
*/ 


/** @defgroup MAIN_Private_Macros
* @{
*/ 
/**
* @}
*/ 


/** @defgroup MAIN_Private_Variables
* @{
*/
SPI_HandleTypeDef hspi2;
/**
* @}
*/ 


/** @defgroup MAIN_Private_FunctionPrototypes
* @{
*/ 

static void SystemClock_Config(void);
static void GUIThread(void const * argument);
static void TimerCallback(void const *n);
static void MX_SPI2_Init(void);
static void MX_GPIO_Init(void);
currentState_T currentState;
osTimerId lcd_timer;

uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
/**
* @}
*/ 

/** @defgroup MAIN_Private_Functions
* @{
*/ 


/**
* @brief  Main program
* @param  None
* @retval int
*/
int main(void)
{     
  /* STM32F4xx HAL library initialization:
  - Configure the Flash ART accelerator on ITCM interface
  - Configure the Systick to generate an interrupt each 1 msec
  - Set NVIC Group Priority to 4
  - Global MSP (MCU Support Package) initialization
  */

  HAL_Init();
  
  /* Configure the system clock @ 180 Mhz */
  SystemClock_Config();

  uint8_t i;
  for(i = 0; i < 5; i++)
  {
	 	currentState.axis[i] = 0;
	 	currentState.offset[i] = 0;
	 	currentState.direction[i] = 0;
  }
  for(i = 0; i < 9; i++)
  {
	  currentState.editString[i] = 0;
  }
  k_BspInit(); 
  MX_SPI2_Init();
  MX_GPIO_Init();
  quadDecode_Init();
  /* Create GUI task */
  osThreadDef(GUI_Thread, GUIThread, osPriorityLow, 0, 2048);
  osThreadCreate (osThread(GUI_Thread), NULL); 
  /* Start scheduler */
  osKernelStart ();

  /* We should never get here as control is now taken by the scheduler */
  for( ;; );
}

const char keypadChars[12] = {'7', '8', '9', '4', '5', '6', '1', '2', '3', '-', '0', '.'};
const char* commandKeysLathe[3] = {"M/L", 0, 0};
const char* commandKeysMill[3] = {"M/L", "1/2", 0};
const char* commandKeysEdit[3] = {"BSP", 0, "ENT"};

static void drawDigit(uint16_t x, uint16_t y, char digit)
{
	GUI_DrawRoundedRect(x - 10, y - 10, x + 70, y + 90, 5);
	GUI_SetFont(GUI_FONT_D60X80);
	GUI_DispCharAt(digit, x, y);
}

void drawCommands(void)
{
	uint8_t commandCnt;
	GUI_ClearRect(5, 345, 530, 445);
	for(commandCnt = 0; commandCnt < 3; commandCnt++)
	{
		if(currentState.entryMode == entryMode_active)
		{
			if(commandKeysEdit[commandCnt])
			{
				GUI_DrawRoundedRect(5 + (commandCnt * 180), 345, 170 + (commandCnt * 180), 445, 5);
				GUI_SetFont(GUI_FONT_32B_ASCII);
				GUI_DispStringAt(commandKeysEdit[commandCnt], 10 + (commandCnt * 200), 360);
			}
		}
		else
		{
			if(currentState.currentMachine == currentMachine_lathe)
			{
				if(commandKeysLathe[commandCnt])
				{
					GUI_DrawRoundedRect(5 + (commandCnt * 180), 345, 170 + (commandCnt * 180), 445, 5);
					GUI_SetFont(GUI_FONT_32B_ASCII);
					GUI_DispStringAt(commandKeysLathe[commandCnt], 10 + (commandCnt * 200), 360);
				}
			}
			else
			{
				if(commandKeysMill[commandCnt])
				{
					GUI_DrawRoundedRect(5 + (commandCnt * 180), 345, 170 + (commandCnt * 180), 445, 5);
					GUI_SetFont(GUI_FONT_32B_ASCII);
					GUI_DispStringAt(commandKeysMill[commandCnt], 10 + (commandCnt * 200), 360);
				}
			}
		}
	}
}

void drawAxes(uint8_t axesToRefresh)
{
	if(axesToRefresh & (1 << allAxes))
	{
		GUI_ClearRect(0, 0, 530, 340);
	}
	GUI_SetPenSize(5);
	if((axesToRefresh & (1 << currentState.currentAxis[currentState.currentMachine])) || (axesToRefresh & (1 << allAxes)))
	{
		GUI_SetColor(GUI_BLUE);
		GUI_DrawRoundedRect(5, currentState.currentAxis[currentState.currentMachine] * 115, 530, 100 + (currentState.currentAxis[currentState.currentMachine] * 115), 5);
	}
	GUI_SetColor(GUI_WHITE);
	if(axesToRefresh & (1 << allAxes))
	{
		GUI_SetFont(GUI_FONT_32B_ASCII);
		GUI_DispStringAt("X: ", 10, 10);
	}
	if((axesToRefresh & (1 << ((currentState.currentMachine * 3) + currentAxis_X))) || (axesToRefresh & (1 << allAxes)))
	{
		if((currentState.currentAxis[currentState.currentMachine] == currentAxis_X) && (currentState.entryMode == entryMode_active))
		{
			GUI_SetFont(GUI_FONT_32B_ASCII);
			GUI_DispStringAt("X: ", 10, 10);
			GUI_SetFont(GUI_FONT_D60X80);
			GUI_DispStringAt(currentState.editString, 45, 10);
		}
		else
		{
			GUI_SetFont(GUI_FONT_D60X80);
			GUI_GotoXY(45, 10);
			GUI_DispSDecShift(currentState.axis[currentState.currentMachine * 3] + currentState.offset[currentState.currentMachine * 3], 8, 3);
		}
	}
	if(axesToRefresh & (1 << allAxes))
	{
		GUI_SetFont(GUI_FONT_32B_ASCII);
		GUI_DispStringAt("Y: ", 10, 125);
	}
	if((axesToRefresh & (1 << ((currentState.currentMachine * 3) + currentAxis_Y))) || (axesToRefresh & (1 << allAxes)))
	{
		if((currentState.currentAxis[currentState.currentMachine] == currentAxis_Y) && (currentState.entryMode == entryMode_active))
		{
			GUI_SetFont(GUI_FONT_32B_ASCII);
			GUI_DispStringAt("Y: ", 10, 125);
			GUI_SetFont(GUI_FONT_D60X80);
			GUI_DispStringAt(currentState.editString, 45, 125);
		}
		else
		{
			int32_t multiplier = 1;
			if(currentState.currentMachine == currentMachine_lathe)
			{
				multiplier = 2; // Lathe cross slide sensor measures the radius, but we want to see the diameter.
			}
			GUI_SetFont(GUI_FONT_D60X80);
			GUI_GotoXY(45, 125);
			GUI_DispSDecShift((currentState.axis[(currentState.currentMachine * 3) + 1] + currentState.offset[(currentState.currentMachine * 3) + 1]) * multiplier, 8, 3);
		}
	}
	if((axesToRefresh & (1 << ((currentState.currentMachine * 3) + currentAxis_Z))) || (axesToRefresh & (1 << allAxes)))
	{
		if (currentState.currentMachine == currentMachine_mill)
		{
			if(axesToRefresh & (1 << allAxes))
			{
				GUI_SetFont(GUI_FONT_32B_ASCII);
				GUI_DispStringAt("Z: ", 10, 240);
			}
			if((currentState.currentAxis[currentState.currentMachine] == currentAxis_Z) && (currentState.entryMode == entryMode_active))
			{
				GUI_SetFont(GUI_FONT_32B_ASCII);
				GUI_DispStringAt("Z: ", 10, 240);
				GUI_SetFont(GUI_FONT_D60X80);
				GUI_DispStringAt(currentState.editString, 45, 240);
			}
			else
			{
				GUI_SetFont(GUI_FONT_D60X80);
				GUI_GotoXY(45, 240);
				GUI_DispSDecShift(currentState.axis[(currentState.currentMachine * 3) + 2] + currentState.offset[(currentState.currentMachine * 3) + 2], 8, 3);
			}
		}
		else
		{
			GUI_ClearRect(10, 240, 530, 340);
		}
	}
}

void drawKeypad(GUI_COLOR color)
{
	GUI_SetColor(color);
	uint8_t keyCnt = 0;
	uint8_t keyX = 0;
	uint8_t keyY = 0;
	GUI_ClearRect(550, 0, 799, 479);
	for(keyY = 0; keyY < 4; keyY++)
	{
		for(keyX = 0; keyX < 3; keyX++)
		{
			drawDigit(550 + (keyX * 85), 10 + (keyY * 115), keypadChars[keyCnt]);
			keyCnt++;
		}
	}
	GUI_SetColor(GUI_WHITE);
}

/**
  * @brief  Start task
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
static void GUIThread(void const * argument)
{   
	/* Initialize Storage Units */
	//k_StorageInit();

	/* Initialize GUI */
	GUI_Init();

	WM_MULTIBUF_Enable(1);
	GUI_SetLayerVisEx (1, 0);
	GUI_SelectLayer(0);

	//GUI_SetBkColor(GUI_WHITE);
	GUI_Clear();

	drawAxes(1 << allAxes);
	drawKeypad(GUI_WHITE);
	drawCommands();

  /* Create Touch screen Timer */
  osTimerDef(TS_Timer, TimerCallback);
  lcd_timer =  osTimerCreate(osTimer(TS_Timer), osTimerPeriodic, (void *)0);

  /* Start the TS Timer */
  osTimerStart(lcd_timer, 30);
  
  /* Show the main menu */
  //k_InitMenu();
    
  /* Gui background Task */
  while(1) {
    GUI_Exec(); /* Do the background work ... Update windows etc.) */
    osDelay(30); /* Nothing left to do for the moment ... Idle processing */
  }
}

/**
  * @brief  Timer callbacsk (40 ms)
  * @param  n: Timer index 
  * @retval None
  */
static void TimerCallback(void const *n)
{  
  k_TouchUpdate();
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 6
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  RCC_OscInitStruct.PLL.PLLR = 6;
  
  
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    while(1);
  }
  /* Enable the OverDrive to reach the 180 Mhz Frequency */  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    while(1);
  }
  
  /* Select PLLSAI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CK48;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CK48CLKSOURCE_PLLSAIP; 
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  

  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    while(1);
  }
}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    while(1);
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, QUAD_A0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, QUAD_A1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOH, QUAD_A2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : QUAD_A0_Pin */
  GPIO_InitStruct.Pin = QUAD_A0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QUAD_A0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : QUAD_A2_Pin */
  GPIO_InitStruct.Pin = QUAD_A2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QUAD_A2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : QUAD_SSN_Pin QUAD_A1_Pin */
  GPIO_InitStruct.Pin = QUAD_SSN_Pin | QUAD_A1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line
  number,ex: printf("Wrong parameters value: file %s on line %d\r\n", 
  file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}

#endif


/**
* @}
*/ 

/**
* @}
*/ 

/**
* @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
