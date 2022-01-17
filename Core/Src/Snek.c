/*
 * Snek.c
 *
 *  Created on: 17 sty 2022
 *      Author: ROJEK
 */

#include "stm32f401xe_GPIO.h"
#include "SSD1306_OLED.h"
#include "GFX_BW.h"
#include "string.h"
#include "stdint.h"
#include "Snek.h"

Snek_t SNEK;

static uint16_t Snek_DrawButton(uint8_t *Text, uint8_t Count, uint8_t Select)
{
	uint8_t _tempX = ((SSD1306_LCDWIDTH - (strlen((char*) Text) * 5) - strlen((char*) Text))) / 2;

	GFX_DrawFillRectangle(0, Count * 16, 128, 15, SSD1306_COLOR_BLACK);
	//draw selected button
	if (Select)
	{
		GFX_DrawFillRectangle(0, Count * 16, 128, 15, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(_tempX, (Count * 16) + 4, (char*) Text, SSD1306_COLOR_BLACK, SSD1306_COLOR_YELLOWBLUE);
	}
	else // draw unselected button
	{
		GFX_DrawRectangle(0, Count * 16, 128, 15, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(_tempX, (Count * 16) + 4, (char*) Text, SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
	}

	return 0;
}

static void Snek_Init(void)
{

	// switch to menu screen
	SNEK.State = GAMESTATE_MENUMAIN;
	// set button on first button
	SNEK.Buttons = BUTTON_START;
	SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_START);
	// set bit to draw menu for the first time
	SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);

}

static void Snek_MenuMain(void)
{
	// check if there is a menu update
	if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED))
	{
		//draw SNEK->Buttons
		Snek_DrawButton((uint8_t*) "Start", 0, 0);
		Snek_DrawButton((uint8_t*) "High scores", 1, 0);
		Snek_DrawButton((uint8_t*) "Settings", 2, 0);
		Snek_DrawButton((uint8_t*) "About", 3, 0);

		// clear active buttons bits
		SNEK.CR1 &= ~(15U << SNEK_CR1_ACTIVE_BUTTON_START);

		//draw selected button
		//set active window bit
		switch (SNEK.Buttons)
		{
		case (BUTTON_START):
			Snek_DrawButton((uint8_t*) "Start", 0, 1);
			SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_START);
			break;

		case (BUTTON_SCORES):
			Snek_DrawButton((uint8_t*) "High scores", 1, 1);
			SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_SCORES);
			break;

		case (BUTTON_SETTINGS):
			Snek_DrawButton((uint8_t*) "Settings", 2, 1);
			SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_SETTIGNS);
			break;

		case (BUTTON_ABOUT):
			Snek_DrawButton((uint8_t*) "About", 3, 1);
			SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_ABOUT);
			break;
		}

		// sent data to OLED
		SSD1306_Display();

		// clear bit to draw on OLED
		SNEK_RESET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);
	}

	// check if button down is clicked
	if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_BUTTON_DOWN))
	{
		// move button cursor down
		SNEK.Buttons = (SNEK.Buttons + 1) % 4;
		// set draw
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);
		// reset button clicked
		SNEK_RESET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_DOWN);
	}

	if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_BUTTON_UP))
	{
		// move button up
		SNEK.Buttons = (SNEK.Buttons + 3) % 4;
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_UP);
	}
}

void Snek(void)
{
	switch (SNEK.State)
	{
	case GAMESTATE_INIT:
		Snek_Init();
		break;

	case GAMESTATE_MENUMAIN:
		Snek_MenuMain();
		break;

	case GAMESTATE_MENUSCORES:
		break;

	case GAMESTATE_MENUABOUT:
		break;

	case GAMESTATE_GAME:
		break;

	case GAMESTATE_OVER:
		break;
	}
}

void Snek_ButtonCallback(uint8_t GPIO_Pin)
{
	if (GPIO_Pin == SNEK_BUTTON_DOWN)
	{
		// chcek if snek is no moving down already
		if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_SNEKMOVE_DOWN))
		{
			return;
		}

		// set button down bit
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_DOWN);

	}
}
