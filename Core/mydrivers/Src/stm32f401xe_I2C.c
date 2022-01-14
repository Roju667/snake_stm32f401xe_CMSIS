/*
 * stm32f401xe_I2C.c
 *
 *  Created on: 12 sty 2022
 *      Author: pawel
 */

#include "stm32f401xe_I2C.h"
#include "stm32f401xe_GPIO.h"
#include "stm32f401xe_RCC.h"

/*
 * Start clock for GPIO
 *
 * @param[*pI2Cx] - base address of i2c peripheral
 * @return - void
 */
static void I2C_ClockEnable(I2C_TypeDef *pI2Cx)
{
	if (pI2Cx == I2C1)
	{
		RCC_CLOCK_I2C1_ENABLE();
	}
	else if (pI2Cx == I2C2)
	{
		RCC_CLOCK_I2C2_ENABLE();
	}
	else if (pI2Cx == I2C3)
	{
		RCC_CLOCK_I2C3_ENABLE();
	}
}

static void I2C_InitGPIOPins(I2C_TypeDef *pI2Cx, uint8_t AlternatePosition)
{
	GPIO_Handle_t GPIO_SDA, GPIO_SCL;

	if (pI2Cx == I2C1)
	{
		RCC_CLOCK_GPIOB_ENABLE();
		// PB6 SCL
		GPIO_SCL.pGPIOx = GPIOB;
		GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_6;

		// PB7 SDA
		GPIO_SDA.pGPIOx = GPIOB;
		GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_7;

		// Alternate Function
		GPIO_SCL.PinConfig.AF = GPIO_PIN_AF_AF4;
		GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (AlternatePosition == 1)
		{
			// PB8 SCL
			GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_8;
			// PB9 SDA
			GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_9;
		}


	}

	if (pI2Cx == I2C2)
	{
		RCC_CLOCK_GPIOB_ENABLE();

		// PB10 SCL
		GPIO_SCL.pGPIOx = GPIOB;
		GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_10;

		// PB11 SDA
		GPIO_SDA.pGPIOx = GPIOB;
		GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_11;

		// Alternate Function
		GPIO_SCL.PinConfig.AF = GPIO_PIN_AF_AF4;
		GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF4;


		if (AlternatePosition == 1)
		{
			// PB3 SDA
			GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_3;
			GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF9;
		}


	}

	if (pI2Cx == I2C3)
	{
		if (AlternatePosition == 0)
		{
			RCC_CLOCK_GPIOB_ENABLE();
		}

		RCC_CLOCK_GPIOA_ENABLE();

		// PA8 SCL
		GPIO_SCL.pGPIOx = GPIOA;
		GPIO_SCL.PinConfig.PinNumber = GPIO_PIN_8;

		// PB9 SDA
		GPIO_SDA.pGPIOx = GPIOB;
		GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_4;

		// Alternate Function
		GPIO_SCL.PinConfig.AF = GPIO_PIN_AF_AF4;
		GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF4;

		if (AlternatePosition == 1)
		{
			RCC_CLOCK_GPIOC_ENABLE();
			// PC9 SDA
			GPIO_SDA.pGPIOx = GPIOC;
			GPIO_SDA.PinConfig.PinNumber = GPIO_PIN_9;

			// Alternate Function
			GPIO_SDA.PinConfig.AF = GPIO_PIN_AF_AF9;
		}

	}

	// Mode AF
	GPIO_SCL.PinConfig.Mode = GPIO_PIN_MODE_AF;
	GPIO_SDA.PinConfig.Mode = GPIO_PIN_MODE_AF;

	// Output type open drain
	GPIO_SCL.PinConfig.OutputType = GPIO_PIN_OT_OD;
	GPIO_SDA.PinConfig.OutputType = GPIO_PIN_OT_OD;

	// Output speed very high
	GPIO_SCL.PinConfig.OutputSpeed = GPIO_PIN_SPEED_VERYHIGH;
	GPIO_SDA.PinConfig.OutputSpeed = GPIO_PIN_SPEED_VERYHIGH;

	// Pull ups
	GPIO_SCL.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_PULLUP;
	GPIO_SDA.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_PULLUP;

	GPIO_InitPin(&GPIO_SDA);
	GPIO_InitPin(&GPIO_SCL);

}

/*
 * Init I2C peripheral
 *
 * @param[*phI2C] - handler to i2c structure
 * @return - void
 */
uint8_t I2C_Init(I2C_Handle_t *phI2C)
{
	I2C_ClockEnable(phI2C->pI2Cx);

	I2C_InitGPIOPins(phI2C->pI2Cx, 0);

	//Reset I2C
	phI2C->pI2Cx->CR1 |= I2C_CR1_SWRST;
	phI2C->pI2Cx->CR1 &= ~(I2C_CR1_SWRST);

	// check frequency limits
	if (phI2C->I2CConfig.Frequency < I2C_FREQUENCY_MINIMUM || phI2C->I2CConfig.Frequency > I2C_FREQUENCY_MAXIMUM)
	{
		return I2C_ERROR_WRONG_FREQUENCY;
	}

	// set frequency
	phI2C->pI2Cx->CR2 &= ~(I2C_CR2_FREQ);
	phI2C->pI2Cx->CR2 |= (phI2C->I2CConfig.Frequency << I2C_CR2_FREQ_Pos);

	phI2C->pI2Cx->CR1 |= I2C_CR1_PE;

	return 0;
}
