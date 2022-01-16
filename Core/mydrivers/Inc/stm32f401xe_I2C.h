/*
 * stm32f401xe_I2C.h
 *
 *  Created on: 12 sty 2022
 *      Author: pawel
 */

#ifndef MYDRIVERS_INC_STM32F401XE_I2C_H_
#define MYDRIVERS_INC_STM32F401XE_I2C_H_

#include "stm32f401xe.h"

// Struct to configure I2C peripheral
typedef struct
{
	uint8_t ABP1FrequencyMHz;			// @Frequency of ABP1 [MHz]

	uint8_t Speed;						// @Speed

} I2C_Config_t;

typedef struct
{
	I2C_TypeDef *pI2Cx;		// @GPIOaddress

	I2C_Config_t I2CConfig;	// @Peripheral config

} I2C_Handle_t;


/*
 * @Frequency  The minimum allowed frequency is 2 MHz,
 * the maximum frequency is limited by the maximum APB1 frequency and cannot exceed
 * 50 MHz (peripheral intrinsic maximum limit). Assign here ABP1 frequency.
 */

#define I2C_FREQUENCY_MINIMUM	2U
#define I2C_FREQUENCY_MAXIMUM	50U

/*
 * @Speed  fPCLK1 must be at least 2 MHz to achieve Sm mode I²C frequencies. It must be at least 4
 * MHz to achieve Fm mode I²C frequencies. It must be a multiple of 10MHz to reach the
 * 400 kHz maximum I²C Fm mode clock.
 */
#define I2C_SPEED_SLOW			0U
#define I2C_SPEED_FAST_DUTY0	1U
#define I2C_SPEED_FAST_DUTY1	2U

/*
 * @CCR - those are the times in nanoseconds that are defined by I2C characteristics
 * and are used to calculate value that has to be put in CCR register
 * hot to calculate it properly can be found in RM on CCR register description
 * values below an be found in DS in I2C characteristics as tw(SCLH) and tr(SCL)
 *
 * TR_SCL	- SDA and SCL rise time [ns]
 * TW_SCLH	- SCL clock high time [ns]
 * TF_SCL	- SDA and SCL fall time [ns]
 * TW_SCLL	- SCL clock low time [ns]
 */

// Slow mode
#define I2C_CCR_SM_TR_SCL					1000U
#define I2C_CCR_SM_TW_SCLH					4000U
#define I2C_CCR_SM_TF_SCL					300U
#define I2C_CCR_SM_TW_SCLL					4700U
#define I2C_CCR_SM_COEFF					2U
#define I2C_CCR_SM_THIGH					(I2C_CCR_SM_TR_SCL + I2C_CCR_SM_TW_SCLH)
#define I2C_CCR_SM_TLOW						(I2C_CCR_SM_TF_SCL + I2C_CCR_SM_TW_SCLL)

// Fast mode
#define I2C_CCR_FM_TR_SCL					300U
#define I2C_CCR_FM_TW_SCLH					600U
#define I2C_CCR_FM_TF_SCL					300U
#define I2C_CCR_FM_TW_SCLL					1300U
#define I2C_CCR_FM_COEFF_DUTY0				3U
#define I2C_CCR_FM_COEFF_DUTY1				25U
#define I2C_CCR_FM_THIGH					(I2C_CCR_FM_TR_SCL + I2C_CCR_FM_TW_SCLH)
#define I2C_CCR_FM_TLOW						(I2C_CCR_FM_TF_SCL + I2C_CCR_FM_TW_SCLL)




/*
 * Errors
 */
#define I2C_ERROR_WRONG_FREQUENCY 1U

uint8_t I2C_Init(I2C_Handle_t *phI2C);
uint8_t I2C_Transmit(I2C_Handle_t *phI2C, uint8_t SlaveAddres, uint8_t pMemAddress);
#endif /* MYDRIVERS_INC_STM32F401XE_I2C_H_ */
