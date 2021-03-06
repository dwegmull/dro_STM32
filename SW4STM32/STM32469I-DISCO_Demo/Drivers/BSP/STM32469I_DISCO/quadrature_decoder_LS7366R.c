/*
 * quadrature_decoder_LS7366R.c
 *
 *  Created on: Jan 2, 2021
 *      Author: dwegmull
 */
#include "main.h"
#include "quadrature_decoder_LS7366R.h"

extern SPI_HandleTypeDef hspi2;

static void quadDecode_selectChip(uint8_t chipNumber)
{
	HAL_GPIO_WritePin(GPIOG, QUAD_A0_Pin, (chipNumber & 0x01) == 0x01);
	HAL_GPIO_WritePin(GPIOA, QUAD_A1_Pin, (chipNumber & 0x02) == 0x02);
	HAL_GPIO_WritePin(GPIOH, QUAD_A2_Pin, (chipNumber & 0x04) == 0x04);
}

void quadDecode_Init(void)
{
	uint8_t channel;
	uint8_t buffer[4];
	buffer[0] = WRITE_MDR0;
	buffer[1] = QUADRX4 | FREE_RUN | DISABLE_INDX | ASYNCH_INDX | FILTER_1;
	buffer[2] = CLR_CNTR;
	buffer[3] = 0;
	for(channel = 0; channel < 5; channel++)
	{
		quadDecode_selectChip(channel);
		HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_SET);
		HAL_SPI_Transmit(&hspi2, buffer, 2, 100);
		HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_SET);
		HAL_SPI_Transmit(&hspi2, &buffer[2], 2, 100);
		HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_RESET);
	}
}

int32_t quadDecode_getCounter(uint8_t channel)
{
	uint8_t command = READ_CNTR;
	uint8_t temp[4];
	int32_t counterValue = 0;
	quadDecode_selectChip(channel);
	HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_SET);
	HAL_SPI_Transmit(&hspi2, &command, 1, 100);
	HAL_SPI_Receive(&hspi2, temp, 4, 100);
	HAL_GPIO_WritePin(GPIOA, QUAD_SSN_Pin, GPIO_PIN_RESET);
	counterValue = ((int32_t)temp[0]) << 24;
	counterValue += ((int32_t)temp[1]) << 16;
	counterValue += ((int32_t)temp[2]) << 8;
	counterValue += ((int32_t)temp[3]);
	return counterValue;
}

