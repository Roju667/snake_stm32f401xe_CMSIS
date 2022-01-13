/*
 * main.c
 *
 *  Created on: Jan 12, 2022
 *      Author: ROJEK
 */


#include "stm32f401xe.h"
#include "stm32f401xe_GPIO.h"
#include "stdint.h"

int main()
{



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

	}
}

void EXTI15_10_IRQHandler()
{

	GPIO_ClearPendingEXTIFlag(GPIO_PIN_13);
	GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}
