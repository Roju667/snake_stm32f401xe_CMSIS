/*
 * main.c
 *
 *  Created on: Jan 12, 2022
 *      Author: ROJEK
 */


#include "stm32f401xe.h"
#include "stm32f401xe_GPIO.h"
#include "stm32f401xe_RCC.h"
#include "stm32f401xe_I2C.h"
#include "SSD1306_OLED.h"
#include "GFX_BW.h"
#include "stdint.h"
#include "font_8x5.h"
#include "stdio.h"
#include "Snek.h"

void SysClockInit(void);
void GPIOConfig(void);
void I2C1Config(I2C_Handle_t *phI2C1);

volatile uint32_t TickS;

int main()
{
	I2C_Handle_t hI2C1;

	SysTick_Config(84000000);

	// Configure RCC
	SysClockInit();

	// Configure GPIO pins
	GPIOConfig();

	// Configure I2C peripherals
	I2C1Config(&hI2C1);
	SSD1306_Init(&hI2C1);

	SSD1306_Clear(SSD1306_COLOR_BLACK);
	GFX_SetFont(font_8x5);

	SSD1306_Display();




	while(1)
	{
		Snek();
	}
}

void EXTI15_10_IRQHandler()
{

	GPIO_ClearPendingEXTIFlag(GPIO_PIN_13);
	GPIO_TogglePin(GPIOA, GPIO_PIN_5);
}

void SysTick_Handler()
{
	TickS++;
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

//configure GPIO used in the project
void GPIOConfig(void)
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
}

void I2C1Config(I2C_Handle_t *phI2C1)
{
	

	phI2C1->pI2Cx = I2C1;
	phI2C1->I2CConfig.ABP1FrequencyMHz = 42;
	phI2C1->I2CConfig.Speed = I2C_SPEED_FAST_DUTY0;

	I2C_Init(phI2C1);
}
