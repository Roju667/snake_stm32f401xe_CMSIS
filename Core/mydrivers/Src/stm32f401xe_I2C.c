/*
 * stm32f401xe_I2C.c
 *
 *  Created on: 12 sty 2022
 *      Author: pawel
 */

#include "stm32f401xe_I2C.h"

void I2C_ClockEnable(I2C_TypeDef *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if (pI2Cx == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
	else if ()
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	}
}

void I2C_Init(I2C_Handle_t *hI2C)
{
	
	hI2C->pI2Cx->CR1 |= I2C_CR1_PE;
}
