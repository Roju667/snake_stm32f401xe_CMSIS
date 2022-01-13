/*
 * stm32f401xe_RCC.c
 *
 *  Created on: Jan 13, 2022
 *      Author: ROJEK
 */

#include "stm32f401xe_RCC.h"

uint8_t RRC_OscillatorConfig(RCC_OscInitTypeDef *pOscInit)
{

	// system clock switch HSI/HSE
	RCC->CFGR |= pOscInit->OscillatorType << RCC_CFGR_SW_Pos;

	// PLL - phase-locked loop - clock generation engine in the MCU

	// if PLL is enable
	if (pOscInit->PLL.PLLState == RCC_PLL_STATE_ENABLE)
	{
		// init PLL source
		RCC->PLLCFGR = (pOscInit->PLL.PLLSource << RCC_PLLCFGR_PLLSRC_Pos);

		// PLLM - division factor for the main PLL
		// It is recommended to put source frequency in range of 1 - 2Mhz (2Mhz recommended)
		// Frequency after this division operation is input of VCO (voltage controlled oscillator)

		if (pOscInit->PLL.PLLM < 3 || pOscInit->PLL.PLLM > 63)
		{
			return PLL_CONFIG_ERROR;
		}
		RCC->PLLCFGR = (pOscInit->PLL.PLLM << RCC_PLLCFGR_PLLM_Pos);

		// PLLN - multiplication factor for VCO
		// After this operation value should be between 192 and 432 MHz
		// Frequency after this operation is output of VCO

		if (pOscInit->PLL.PLLN < 2 || pOscInit->PLL.PLLM > 432)
		{
			return PLL_CONFIG_ERROR;
		}
		RCC->PLLCFGR = (pOscInit->PLL.PLLN << RCC_PLLCFGR_PLLN_Pos);

		// PLLP - division factor for main system clock
		// After this operation values should not exceed 84MHz
		// Frequency after this operation is frequency of main system clock

		// 2 bits register so value can not be more than 3
		if (pOscInit->PLL.PLLP > 3)
		{
			return PLL_CONFIG_ERROR;
		}
		RCC->PLLCFGR = (pOscInit->PLL.PLLP << RCC_PLLCFGR_PLLP_Pos);

		// PLLQ - ) division factor for USB OTG FS, SDIO and random number generator clocks
		// The USB OTG FS requires a 48 MHz clock to work correctly. The SDIO and the
		// random number generator need a frequency lower than or equal to 48 MHz to work
		// correctly

		if (pOscInit->PLL.PLLN < 2 || pOscInit->PLL.PLLM > 15)
		{
			return PLL_CONFIG_ERROR;
		}
		RCC->PLLCFGR = (pOscInit->PLL.PLLQ << RCC_PLLCFGR_PLLQ_Pos);

		// Enable PLL
		RCC->CR = (RCC_PLL_STATE_ENABLE << RCC_CR_PLLON_Pos);

	}
	else
	{
		// Disable PLL
		RCC->CR = (RCC_PLL_STATE_DISABLE << RCC_CR_PLLON_Pos);
	}

	return 0;
}
