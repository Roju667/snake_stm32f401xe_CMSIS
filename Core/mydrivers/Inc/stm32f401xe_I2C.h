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
	uint8_t PinNumber;			// @PinNumber

	uint8_t Mode;				// @GPIOModes

	uint8_t OutputType;			// @OutputType

	uint8_t OutputSpeed;		// @GPIOSpeed

	uint8_t PullUpPullDown;		// @GPIOPullUpPullDown

	uint8_t AF;					// @GPIOAlternatefunction

} I2C_Config_t;

typedef struct
{
	I2C_TypeDef *pI2Cx;	// @GPIOaddress

	I2C_Config_t PinConfig;	// @PinConfig

} I2C_Handle_t;

#define I2C_SPEED_SLOW		0
#define I2C_SPEED_FAST		1


#endif /* MYDRIVERS_INC_STM32F401XE_I2C_H_ */
