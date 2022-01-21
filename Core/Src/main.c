/*
 * main.c
 *
 *  Created on: Jan 12, 2022
 *      Author: ROJEK
 */


#include <stm32f401xe_gpio.h>
#include <stm32f401xe_i2c.h>
#include <stm32f401xe_rcc.h>
#include "stm32f401xe.h"
#include "SSD1306_OLED.h"
#include "GFX_BW.h"
#include "stdint.h"
#include "font_8x5.h"
#include "stddef.h"
#include "stdio.h"
#include "Snek.h"

void SysClockInit(void);
void GPIOConfig(void);
void i2c1_config(i2c_handle_t *p_handle_i2c1);

volatile uint32_t TickS ;

int main()
{
	i2c_handle_t h_i2c1;

	SysTick_Config(84000000);

	// Configure RCC
	SysClockInit();

	RCC_ClockFreqs freqs = {0};
	RCC_GetClockFrequencies(&freqs);
	// Configure GPIO pins
	GPIOConfig();

	// Configure I2C peripherals
	i2c1_config(&h_i2c1);
	SSD1306_Init(&h_i2c1);

	SSD1306_Clear(SSD1306_COLOR_BLACK);
	GFX_SetFont(font_8x5);

	SSD1306_Display();




	while(1)
	{
		snek();
	}
}

// handler for button RIGHT
void EXTI1_IRQHandler(void)
{
	uint8_t _tempGPIOPin;

	if (EXTI->PR & (0b1 << SNEK_BUTTON_RIGHT))
	{
		GPIO_ClearPendingEXTIFlag(SNEK_BUTTON_RIGHT);
		_tempGPIOPin = SNEK_BUTTON_RIGHT;
	}

	snek_button_callback(_tempGPIOPin);
}

// handler for button DOWN
void EXTI2_IRQHandler(void)
{
	uint8_t _tempGPIOPin;

	if (EXTI->PR & (0b1 << SNEK_BUTTON_DOWN))
	{
		GPIO_ClearPendingEXTIFlag(SNEK_BUTTON_DOWN);
		_tempGPIOPin = SNEK_BUTTON_DOWN;
	}

	snek_button_callback(_tempGPIOPin);
}

// handler for buttons up/left/enter
void EXTI15_10_IRQHandler(void)
{
	uint8_t _tempGPIOPin;
	// if button is clicked
	if (EXTI->PR & (0b1 << SNEK_BUTTON_UP))
	{
		GPIO_ClearPendingEXTIFlag(SNEK_BUTTON_UP);
		_tempGPIOPin = SNEK_BUTTON_UP;
	}
	else if (EXTI->PR & (0b1 << SNEK_BUTTON_LEFT))
	{
		GPIO_ClearPendingEXTIFlag(SNEK_BUTTON_LEFT);
		_tempGPIOPin = SNEK_BUTTON_LEFT;
	}

	else if (EXTI->PR & (0b1 << SNEK_BUTTON_ENTER))
	{
		GPIO_ClearPendingEXTIFlag(SNEK_BUTTON_ENTER);
		_tempGPIOPin = SNEK_BUTTON_ENTER;
	}

	snek_button_callback(_tempGPIOPin);
}

void SysTick_Handler()
{
	TickS++;
}

// system clock init function
void SysClockInit(void)
{
	RCC_ClockInitTypeDef pClockInit;

	pClockInit.apb1_prescaler = RCC_ABP_PRESCALER_DIV2;
	pClockInit.apb2_prescaler = RCC_ABP_PRESCALER_NODIV;
	pClockInit.flash_latency = RCC_FLASHLATENCY_2WS;
	pClockInit.hpre_prescaler = RCC_HPRE_PRESCALER_NODIV;
	pClockInit.oscillator_type = RCC_OSCILLATORTYPE_HSI;
	pClockInit.vos_scale = RCC_VOS_SCALE2;

	// init pll
	pClockInit.pll.source = RCC_PLL_SOURCE_HSI;
	pClockInit.pll.state = RCC_PLL_STATE_ENABLE;
	pClockInit.pll.pllm = 16;
	pClockInit.pll.plln = 336;
	pClockInit.pll.pllp = RCC_PLLP_DIV4;
	pClockInit.pll.pllq = 7;

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
	GPIOx.pGPIOx = GPIOB;
	GPIO_InitPin(&GPIOx);

	GPIOx.PinConfig.PinNumber = GPIO_PIN_14;
	GPIO_InitPin(&GPIOx);

	GPIOx.PinConfig.PinNumber = GPIO_PIN_15;
	GPIO_InitPin(&GPIOx);

	GPIOx.PinConfig.PinNumber = GPIO_PIN_1;
	GPIO_InitPin(&GPIOx);

	GPIOx.PinConfig.PinNumber = GPIO_PIN_2;
	GPIO_InitPin(&GPIOx);
	
}

void i2c1_config(i2c_handle_t *p_handle_i2c1)
{
	

	p_handle_i2c1->p_i2cx = I2C1;
	p_handle_i2c1->i2c_config.abp1_freq_mhz = 42;
	p_handle_i2c1->i2c_config.speed = I2C_SPEED_FAST_DUTY0;

	i2c_init(p_handle_i2c1);
}
