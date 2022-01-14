/*
 * stm32f401xe_I2C.h
 *
 *  Created on: 12 sty 2022
 *      Author: pawel
 */

#ifndef MYDRIVERS_INC_STM32F401XE_I2C_H_
#define MYDRIVERS_INC_STM32F401XE_I2C_H_

#include "stm32f401xe.h"

// Struct to configure pin @PinConfig
typedef struct
{
	uint8_t Frequency;			// @Frequency

	uint8_t Speed;				// @Speed

	uint8_t OutputType;			// @OutputType

	uint8_t OutputSpeed;		// @GPIOSpeed

	uint8_t PullUpPullDown;		// @GPIOPullUpPullDown

	uint8_t AF;					// @GPIOAlternatefunction

} I2C_Config_t;

typedef struct
{
	I2C_TypeDef *pI2Cx;		// @GPIOaddress

	I2C_Config_t I2CConfig;	// @Peripheral config

} I2C_Handle_t;

#define I2C_SPEED_SLOW		0U
#define I2C_SPEED_FAST		1U

/*
 * @Frequency  The minimum allowed frequency is 2 MHz,
 * the maximum frequency is limited by the maximum APB1 frequency and cannot exceed
 * 50 MHz (peripheral intrinsic maximum limit). Assign here ABP1 frequency.
 */

#define I2C_FREQUENCY_MINIMUM	2U
#define I2C_FREQUENCY_MAXIMUM	50U


/*
 * Error
 */
#define I2C_ERROR_WRONG_FREQUENCY 1U

#endif /* MYDRIVERS_INC_STM32F401XE_I2C_H_ */
