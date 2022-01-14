/*
 * main.c
 *
 *  Created on: Jan 12, 2022
 *      Author: ROJEK
 */


#include "stm32f401xe.h"
#include "stm32f401xe_GPIO.h"
#include "stm32f401xe_RCC.h"
#include "stdint.h"

void SysClockInit(void);

int main()
{


	SysClockInit();

	GPIO_Handle_t GPIOx;
	GPIOx.PinConfig.PinNumber = GPIO_PIN_5;
	GPIOx.PinConfig.Mode = GPIO_PIN_MODE_OUTPUT;
	GPIOx.PinConfig.OutputType = GPIO_PIN_OT_PP;
	GPIOx.PinConfig.PullUpPullDown = GPIO_PIN_PUPD_NOPULL;
	GPIOx.pGPIOx = GPIOA;

	GPIO_InitPin(&GPIOx);

	GPIOx.PinConfig.PinNumber = GPIO_PIN_13;
	GPIOx.PinConfig.Mode = GPIO_PIN_MODE_EXTI_FT;
	GPIOx.pGPIOx = GPIOC;

	GPIO_InitPin(&GPIOx);


	while(1)
	{
		for(uint32_t i = 0 ; i < 8400000 ; i++)
		{

		}
		GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}

void EXTI15_10_IRQHandler()
{

	GPIO_ClearPendingEXTIFlag(GPIO_PIN_13);
	GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

// system clock init function
void SysClockInit(void)
{
	RCC_ClockInitTypeDef pClockInit;

	pClockInit.APB1Prescaler = RCC_ABP1_PRESCALER_DIV2;
	pClockInit.APB2Prescaler = RCC_ABP2_PRESCALER_NODIV;
	pClockInit.FLASHLatency = RCC_FLASHLATENCY_2WS;
	pClockInit.HPREPrescaler = RCC_HPRE_PRESCALER_NODIV;
	pClockInit.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	pClockInit.VOSScale = RCC_VOS_SCALE2;
	pClockInit.PLL.PLLSource = RCC_PLL_SOURCE_HSI;
	pClockInit.PLL.PLLState = RCC_PLL_STATE_ENABLE;
	pClockInit.PLL.PLLM = 16;
	pClockInit.PLL.PLLN = 336;
	pClockInit.PLL.PLLP = RCC_PLLP_DIV4;
	pClockInit.PLL.PLLQ = 7;

	RCC_InitClock(&pClockInit);
}
