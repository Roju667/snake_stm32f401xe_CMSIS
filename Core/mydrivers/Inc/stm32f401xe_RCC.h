/*
 * stm32f401xe_RCC.h
 *
 *  Created on: Jan 13, 2022
 *      Author: ROJEK
 */

#ifndef MYDRIVERS_INC_STM32F401XE_RCC_H_
#define MYDRIVERS_INC_STM32F401XE_RCC_H_

#include "stm32f401xe.h"

/*
System clock switch
Set and cleared by software to select the system clock source.
Set by hardware to force the HSI selection when leaving the Stop or Standby mode or in
case of failure of the HSE oscillator used directly or indirectly as the system clock.
*/
#define RCC_SW_HSI				0U
#define RCC_SW_HSE				1U
#define RCC_SW_PLL				2U

/*
AHB prescaler
Set and cleared by software to control AHB clock division factor
 */
#define RCC_HPRE_DIV2			0U
#define RCC_HPRE_DIV4			1U
#define RCC_HPRE_DIV8			2U
#define RCC_HPRE_DIV16			3U
#define RCC_HPRE_DIV64			4U
#define RCC_HPRE_DIV128			5U
#define RCC_HPRE_DIV256			6U
#define RCC_HPRE_DIV512			7U

/*
APB Low speed prescaler (APB1)
Set and cleared by software to control APB low-speed clock division factor
 */

#define RCC_ABP1_PRESCALER_DIV2		0U
#define RCC_ABP1_PRESCALER_DIV4		1U
#define RCC_ABP1_PRESCALER_DIV8		2U
#define RCC_ABP1_PRESCALER_DIV16	3U

/*
APB high-speed prescaler (APB2)
Set and cleared by software to control APB high-speed clock division factor
 */

#define RCC_ABP2_PRESCALER_DIV2		0U
#define RCC_ABP2_PRESCALER_DIV4		1U
#define RCC_ABP2_PRESCALER_DIV8		2U
#define RCC_ABP2_PRESCALER_DIV16	3U

#endif /* MYDRIVERS_INC_STM32F401XE_RCC_H_ */
