/**
  ******************************************************************************
  * @file    k_bsp.c
  * @author  MCD Application Team
  * @brief   This file provides the kernel bsp functions
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
#include "k_bsp.h"
#include "main.h"
#include "quadrature_decoder_LS7366R.h"
/** @addtogroup CORE
  * @{
  */

/** @defgroup KERNEL_BSP
  * @brief Kernel BSP routines
  * @{
  */

/* External variables --------------------------------------------------------*/
extern uint8_t SelLayer;
extern uint8_t  I2C_Address;
TS_StateTypeDef  TS_State = {0};
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define HYST 1
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes LEDs, SDRAM, touch screen, CRC and SRAM.
  * @param  None 
  * @retval None
  */
void k_BspInit(void)
{
  /* Initialize the NOR */
  BSP_QSPI_Init();
  BSP_QSPI_EnableMemoryMappedMode();  
  
  
  /* Initialize the SDRAM */
  BSP_SDRAM_Init();
  
  HAL_Delay(100);
  
  /* Initialize the Touch screen */
  BSP_TS_Init(800, 480);
    
  /* Enable CRC to Unlock GUI */
 __HAL_RCC_CRC_CLK_ENABLE();
  
 /* Enable Back up SRAM */
__HAL_RCC_BKPSRAM_CLK_ENABLE();
  
}

extern currentState_T currentState;

static void processDigit(char digit)
{
	uint8_t numberOfDigits = 0;
	uint8_t stringOffset;
	uint8_t numberOfDots = 0;
	for(stringOffset = 0; stringOffset < 8; stringOffset++)
	{
		if((numberOfDots == 0 ) && (numberOfDigits == 3) && (currentState.editString[stringOffset] != '.'))
		{
			// we can't have more than three digits in a row: 123.123
			return;
		}
		if(currentState.editString[stringOffset] == '.')
		{
			numberOfDots++;
		}
		else
		{
			if(currentState.editString[stringOffset] >= 0x30)
			{
				numberOfDigits++;
			}
		}
	}
	if(numberOfDigits < 6)
	{
		currentState.editString[strlen(currentState.editString)] = digit;
	}
}

static uint8_t hysteresis(uint8_t axis, int32_t newVal)
{
	uint8_t returnVal = 0;
	if(newVal != currentState.axis[axis])
	{
	  if(newVal > currentState.axis[axis])
	  {
		  if(currentState.direction[axis] == 0)
		  {
			  // Change of direction: check the hysteresis
			  if((newVal - HYST) > currentState.axis[axis])
			  {
				  currentState.axis[axis] = newVal;
				  returnVal = 1;
				  currentState.direction[axis] = 1;
			  }
		  }
		  else
		  {
			  // Same direction: always report
			  currentState.axis[axis] = newVal;
			  returnVal = 1;
		  }
	  }
	  else
	  {
		  if(currentState.direction[3] == 1)
		  {
			  // Change of direction: check the hysteresis
			  if((currentState.axis[axis] - HYST) < newVal)
			  {
				  currentState.axis[axis] = newVal;
				  returnVal = 1;
				  currentState.direction[axis] = 0;
			  }
		  }
		  else
		  {
			  // Same direction: always report
			  currentState.axis[axis] = newVal;
			  returnVal = 1;
		  }
	  }
	}
	return returnVal;
}
/**
  * @brief  Read the coordinate of the point touched and assign their
  *         value to the variables u32_TSXCoordinate and u32_TSYCoordinate
  * @param  None
  * @retval None
  */
void k_TouchUpdate(void)
{
  static GUI_PID_STATE TS_State = {0, 0, 0, 0};
  __IO TS_StateTypeDef  ts;
  uint16_t xDiff, yDiff;
  uint8_t axesRefreshNeeded = 0;
  BSP_TS_GetState((TS_StateTypeDef *)&ts);

  if((ts.touchX[0] >= LCD_GetXSize()) ||(ts.touchY[0] >= LCD_GetYSize()) ) 
  {
    ts.touchX[0] = 0;
    ts.touchY[0] = 0;
  }

  xDiff = (TS_State.x > ts.touchX[0]) ? (TS_State.x - ts.touchX[0]) : (ts.touchX[0] - TS_State.x);
  yDiff = (TS_State.y > ts.touchY[0]) ? (TS_State.y - ts.touchY[0]) : (ts.touchY[0] - TS_State.y);
  if(currentState.currentMachine == currentMachine_lathe)
  {
	  int32_t tempCount;
	  tempCount = quadDecode_getCounter(3);
	  axesRefreshNeeded += hysteresis(3, tempCount);
	  tempCount = quadDecode_getCounter(4);
	  axesRefreshNeeded += hysteresis(4, tempCount);
  }
  else
  {
	  int32_t tempCount;
	  tempCount = quadDecode_getCounter(0);
	  axesRefreshNeeded += hysteresis(0, tempCount);
	  tempCount = quadDecode_getCounter(1);
	  axesRefreshNeeded += hysteresis(1, tempCount);
	  tempCount = quadDecode_getCounter(2);
	  axesRefreshNeeded += hysteresis(2, tempCount);
  }
  if((TS_State.Pressed != ts.touchDetected ) || (xDiff > 20 ) || (yDiff > 20))
  {
    TS_State.Pressed = ts.touchDetected;
    //TS_State.Layer = SelLayer;
    if(ts.touchDetected) 
    {
    	if(ts.touchX[0] < 550)
    	{
    		if((ts.touchY[0] < 345) && (currentState.entryMode == entryMode_notActive))
    		{
    			// axis selection
    			if(ts.touchY[0] < 125)
    			{
    				// X axis
    				currentState.currentAxis[currentState.currentMachine] = currentAxis_X;
    				axesRefreshNeeded = 1;
    			}
    			else
    			{
    				if(currentState.currentMachine == currentMachine_lathe)
    				{
    					// Lathe: only two axes
        				currentState.currentAxis[currentState.currentMachine] = currentAxis_Y;
        				axesRefreshNeeded = 1;
    				}
    				else
    				{
    					// Mill: check between Y and Z
    					if(ts.touchY[0] < 240)
    					{
            				currentState.currentAxis[currentState.currentMachine] = currentAxis_Y;
            				axesRefreshNeeded = 1;
    					}
    					else
    					{
            				currentState.currentAxis[currentState.currentMachine] = currentAxis_Z;
            				axesRefreshNeeded = 1;
    					}
    				}
    			}
    		}
    		else
    		{
    			// command key
    			if(ts.touchX[0] < 185)
    			{
    				if(currentState.entryMode == entryMode_notActive)
    				{
						// Machine toggle
						currentState.currentMachine = 1 - currentState.currentMachine;
						axesRefreshNeeded = 1;
						drawCommands();
    				}
    				else
    				{
    					// Back space
    					currentState.editString[strlen(currentState.editString) - 1] = 0;
    					axesRefreshNeeded = 1;
    				}
    			}
    			else
    			{
    				if((ts.touchX[0] < 365) && (currentState.entryMode == entryMode_notActive) && (currentState.currentMachine == currentMachine_mill))
    				{
    					// 1/2 function
    					int32_t dv = currentState.axis[currentState.currentAxis[currentState.currentMachine]] + currentState.offset[currentState.currentAxis[currentState.currentMachine]];
    					dv = dv /2;
    					currentState.offset[currentState.currentAxis[currentState.currentMachine]] = dv - currentState.axis[currentState.currentAxis[currentState.currentMachine]];
    					axesRefreshNeeded = 1;
    				}
    				else
    				{
    					if((ts.touchX[0] >= 365) && (currentState.entryMode == entryMode_active))
    					{
    						int8_t stringOffset;
    						int8_t numberDecimals = 0;
    						int32_t value = 0;
    						int32_t multiplier = 1;
    						// Enter key
    						currentState.entryMode = entryMode_notActive;
    						if(strlen(currentState.editString) > 0)
    						{
								stringOffset = strlen(currentState.editString) - 1;
								do
								{
									if(currentState.editString[stringOffset] == '-')
									{
										value = -value;
									}
									else
									{
										if(currentState.editString[stringOffset] != '.')
										{
											// It's a digit.
											value += multiplier * (currentState.editString[stringOffset] - 0x30);
											multiplier *= 10;
											if(numberDecimals >= 0)
											{
												numberDecimals++;
											}
										}
										else
										{
											numberDecimals = -numberDecimals;
										}
									}
									stringOffset--;
								}
								while(stringOffset > -1);
								numberDecimals = -numberDecimals;
								switch(numberDecimals)
								{
									default:
										value *= 1000;
										break;
									case 1:
										value *= 100;
										break;
									case 2:
										value *= 10;
										break;
									case 3:
										break;
								}
    						}
    						if((currentState.currentMachine == currentMachine_lathe) && (currentState.currentAxis[currentState.currentMachine] == currentAxis_Y))
    						{
    							// Lathe Y axis: we enter a diameter, but the data is stored internally as a radius, to match the sensor scale.
    							value = value / 2;
    						}
   							currentState.offset[currentState.currentAxis[currentState.currentMachine] + currentState.currentMachine * 3] = value - currentState.axis[currentState.currentAxis[currentState.currentMachine] + currentState.currentMachine * 3];
    						for(stringOffset = 0; stringOffset < 9; stringOffset++)
							{
							  currentState.editString[stringOffset] = 0;
							}
    					}
    					currentState.entryMode = entryMode_notActive;
						drawKeypad(GUI_WHITE);
    					axesRefreshNeeded = 1;
    					drawCommands();
    				}
    			}
    		}
    	}
    	else
    	{
    		// Key pad
    		if(currentState.entryMode != entryMode_active)
			{
        		currentState.entryMode = entryMode_active;
				drawKeypad(GUI_RED);
    			drawCommands();
			}
    		if(ts.touchY[0] < 115)
    		{
    			//789
    			if(ts.touchX[0] < 620)
    			{
    				processDigit('7');
    			}
    			else
    			{
    				if(ts.touchX[0] < 705)
    				{
        				processDigit('8');
    				}
    				else
    				{
        				processDigit('9');
    				}
    			}
    		}
    		else
    		{
    			if(ts.touchY[0] < 230)
    			{
    				//456
        			if(ts.touchX[0] < 620)
        			{
        				processDigit('4');
        			}
        			else
        			{
        				if(ts.touchX[0] < 705)
        				{
            				processDigit('5');
        				}
        				else
        				{
            				processDigit('6');
        				}
        			}
    			}
    			else
    			{
    				if(ts.touchY[0] < 345)
    				{
    					//123
    	    			if(ts.touchX[0] < 620)
    	    			{
    	    				processDigit('1');
    	    			}
    	    			else
    	    			{
    	    				if(ts.touchX[0] < 705)
    	    				{
    	        				processDigit('2');
    	    				}
    	    				else
    	    				{
    	        				processDigit('3');
    	    				}
    	    			}
    				}
    				else
    				{
    					//-0.
    	    			if(ts.touchX[0] < 620)
    	    			{
    	    				//-
    	    				if(strlen(currentState.editString) == 0)
    	    				{
    	    					// - can only be the leading character
    	    					currentState.editString[0] = '-';
    	    				}
    	    			}
    	    			else
    	    			{
    	    				if(ts.touchX[0] < 705)
    	    				{
    	        				processDigit('0');
    	    				}
    	    				else
    	    				{
    	    					if(0 == strstr(currentState.editString, "."))
    	    					{
    	    						currentState.editString[strlen(currentState.editString)] = '.';
    	    					}
    	    				}
    	    			}
    				}
    			}
    		}
    		axesRefreshNeeded = 1;
    	}
      TS_State.x = ts.touchX[0];
      if(I2C_Address == TS_I2C_ADDRESS)
      {
        if(ts.touchY[0] < 240)
        {
          TS_State.y = ts.touchY[0];
        }
        else
        {
          TS_State.y = (ts.touchY[0] * 480) / 450;
        }
      }
      else
      {
        TS_State.y = ts.touchY[0]; 
      }
//      GUI_TOUCH_StoreStateEx(&TS_State);
    }
    else
    {
//      GUI_TOUCH_StoreStateEx(&TS_State);
      TS_State.x = 0;
      TS_State.y = 0;      
    }
  }
  if(axesRefreshNeeded == 1)
  {
	  drawAxes();
  }
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
