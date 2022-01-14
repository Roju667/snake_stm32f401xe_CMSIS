/*
 * stm32f401xe_RCC.c
 *
 *  Created on: Jan 13, 2022
 *      Author: ROJEK
 */

#include "stm32f401xe_RCC.h"


/*
 * Configure clock
 * 1. Enable HSI/HSE as main source RCC->CR
 * 2. Set regulator scaling PWR->VOS if necessary before PLL is ON
 * 3. Configure Flash and Latency settings
 * 4. Configure HCLK, PCLK1, PCLK2
 * 5. Configure PLL
 * 6. Enable the PLL
 * 7. Select system clock
 * @param[*GPIO] - base address of gpiox peripheral
 * @return - void
 */
uint8_t RCC_InitClock(RCC_ClockInitTypeDef *pClockInit)
{

/* 1. Enable HSI/HSE as main source RCC->CR */

	//enable HSI/HSE
	if (pClockInit->OscillatorType == RCC_OSCILLATORTYPE_HSI)
	{
		// set HSI flag
		RCC->CR |= RCC_CR_HSION;
		// wait until HSI is on
		while (!(RCC->CR & RCC_CR_HSIRDY))
			;
	}
	else if (pClockInit->OscillatorType == RCC_OSCILLATORTYPE_HSE)
	{
		// set HSE flag
		RCC->CR |= RCC_CR_HSEON;
		// wait until HSE is on
		while (!(RCC->CR & RCC_CR_HSERDY))
			;
	}

/* 2. Set regulator scaling PWR->VOS if necessary before PLL is ON */
	PWR->CR &= ~(PWR_CR_VOS);
	PWR->CR |= (pClockInit->VOSScale << PWR_CR_VOS_Pos);

/* 3. Configure Flash and Latency settings */
	// enable data, instruction and prefetch cache
	FLASH->ACR |= (FLASH_ACR_DCEN | FLASH_ACR_ICEN | FLASH_ACR_PRFTEN);
	// to read correct from flash at certain voltage and frequency latency between read has to be set correctly
	// this table can be found in reference manual
	FLASH->ACR &= ~(FLASH_ACR_LATENCY);
	FLASH->ACR |= (pClockInit->FLASHLatency << FLASH_ACR_LATENCY_Pos);

/* 4. Configure HCLK, PCLK1, PCLK2 */
	// configure AHB prescaler
	RCC->CFGR &= ~(RCC_CFGR_HPRE);
	RCC->CFGR |= (pClockInit->HPREPrescaler << RCC_CFGR_HPRE_Pos);

	// configure APB1 prescaler
	RCC->CFGR &= ~(RCC_CFGR_PPRE1);
	RCC->CFGR |= (pClockInit->APB1Prescaler << RCC_CFGR_PPRE1_Pos);

	// configure APB2 prescaler
	RCC->CFGR &= ~(RCC_CFGR_PPRE2);
	RCC->CFGR |= (pClockInit->APB2Prescaler << RCC_CFGR_PPRE2_Pos);

/* 5. Configure PLL */

			// PLL - phase-locked loop - clock generation engine in the MCU

			// if PLL is enable
			if (pClockInit->PLL.PLLState == RCC_PLL_STATE_ENABLE)
			{
				// init PLL source
				RCC->PLLCFGR |= (pClockInit->PLL.PLLSource << RCC_PLLCFGR_PLLSRC_Pos);

				// PLLM - division factor for the main PLL
				// It is recommended to put source frequency in range of 1 - 2Mhz (2Mhz recommended)
				// Frequency after this division operation is input of VCO (voltage controlled oscillator)

				if (pClockInit->PLL.PLLM < 3 || pClockInit->PLL.PLLM > 63)
				{
					return PLL_CONFIG_ERROR;
				}
				RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);
				RCC->PLLCFGR |= (pClockInit->PLL.PLLM << RCC_PLLCFGR_PLLM_Pos);

				// PLLN - multiplication factor for VCO
				// After this operation value should be between 192 and 432 MHz
				// Frequency after this operation is output of VCO

				if (pClockInit->PLL.PLLN < 2 || pClockInit->PLL.PLLM > 432)
				{
					return PLL_CONFIG_ERROR;
				}
				RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN);
				RCC->PLLCFGR |= (pClockInit->PLL.PLLN << RCC_PLLCFGR_PLLN_Pos);

				// PLLP - division factor for main system clock
				// After this operation values should not exceed 84MHz
				// Frequency after this operation is frequency of main system clock

				// 2 bits register so value can not be more than 3
				if (pClockInit->PLL.PLLP > 3)
				{
					return PLL_CONFIG_ERROR;
				}
				RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP);
				RCC->PLLCFGR |= (pClockInit->PLL.PLLP << RCC_PLLCFGR_PLLP_Pos);

				// PLLQ - ) division factor for USB OTG FS, SDIO and random number generator clocks
				// The USB OTG FS requires a 48 MHz clock to work correctly. The SDIO and the
				// random number generator need a frequency lower than or equal to 48 MHz to work
				// correctly

				if (pClockInit->PLL.PLLQ < 2 || pClockInit->PLL.PLLQ > 15)
				{
					return PLL_CONFIG_ERROR;
				}
				RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ);
				RCC->PLLCFGR |= (pClockInit->PLL.PLLQ << RCC_PLLCFGR_PLLQ_Pos);

/* 6. Enable the PLL */
				RCC->CR |= (RCC_PLL_STATE_ENABLE << RCC_CR_PLLON_Pos);

				// wait until PLL is ready
				while (!(RCC->CR & RCC_CR_PLLRDY))
					;
/* 7. Select system clock switch */
				// set PLL as system clock
				RCC->CFGR &= ~(RCC_CFGR_SW);
				RCC->CFGR |= RCC_CFGR_SW_PLL;
				while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

				return 0;
			}
			else
			{
/* 6. Disable the PLL */
				RCC->CR = (RCC_PLL_STATE_DISABLE << RCC_CR_PLLON_Pos);

				// wait until PLL is disables
				while (!(RCC->CR & RCC_CR_PLLRDY));
/* 7. Select system clock switch */
				// set HSI or HE as main system clock
				RCC->CFGR &= ~(RCC_CFGR_SW);
				RCC->CFGR |= (pClockInit->OscillatorType << RCC_CFGR_SW_Pos);

				// wait until HSI/HSE is set as a clock source
				while((RCC->CFGR & (pClockInit->OscillatorType << RCC_CFGR_SW_Pos)) != RCC_CFGR_SWS_PLL);

			}

	return 0;
}
