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

volatile Snek_t SNEK;

static void Snek_DrawButton(uint8_t *Text, uint8_t Count, uint8_t Select)
{
	// lcd width - char size - spaces between chars
	uint8_t _tempX = ((SSD1306_LCDWIDTH - (strlen((char*) Text) * 5) - strlen((char*) Text))) / 2;

	GFX_DrawFillRectangle(0, Count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_BLACK);
	//draw selected button
	if (Select)
	{
		GFX_DrawFillRectangle(0, Count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(_tempX, (Count * SNEK_UI_BUTTON_OFFSET) + SNEK_UI_CHAR_OFFSET, (char*) Text, SSD1306_COLOR_BLACK, SSD1306_COLOR_YELLOWBLUE);
	}
	else // draw unselected button
	{
		GFX_DrawRectangle(0, Count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(_tempX, (Count * SNEK_UI_BUTTON_OFFSET) + SNEK_UI_CHAR_OFFSET, (char*) Text, SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
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
			SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_SETTINGS);
			break;

		case (BUTTON_ABOUT):
			Snek_DrawButton((uint8_t*) "About", 3, 1);
			SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_ABOUT);
			break;
		}

		// sent data to OLED
		SSD1306_Display();
		SNEK_RESET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);
	}

	// check if button down is clicked
	if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_BUTTON_DOWN))
	{
		// move button cursor down
		SNEK.Buttons = (SNEK.Buttons + 1) % 4;
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_DOWN);
	}

	// check if button up is clicked
	if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_BUTTON_UP))
	{
		// move button up
		SNEK.Buttons = (SNEK.Buttons + 3) % 4;
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_UP);
	}

	// check if enter is clicked
	if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_BUTTON_ENTER))
	{
		// change menu screen
		if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_START))
		{
			SNEK.State = GAMESTATE_GAME;
			SNEK_SET_BIT(SNEK.CR1 , SNEK_CR1_INIT_NEWSCREEN);
		}
		else if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_SCORES))
		{
			SNEK.State = GAMESTATE_MENUSCORES;
		}
		else if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_ABOUT))
		{
			SNEK.State = GAMESTATE_MENUABOUT;
		}
		else if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_ACTIVE_BUTTON_SETTINGS))
		{
			SNEK.State = GAMESTATE_MENUSETTINGS;
		}

	}
}

static void Snek_Game(void)
{
	// draw first screen
	if(SNEK_CR1_INIT_NEWSCREEN)
	{
		// init map
		uint8_t GameMap [6][16] = {0};
		// clear screen
		SSD1306_Clear(SSD1306_COLOR_BLACK);
		// draw scorebox as a top button
		GFX_DrawRectangle(0, 0, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(5, 0 + SNEK_UI_CHAR_OFFSET, "SNEK", SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
		GFX_DrawString(50, 0 + SNEK_UI_CHAR_OFFSET, "SCORE:", SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
		// draw snek


		SSD1306_Display();
		SNEK_RESET_BIT(SNEK.CR1,SNEK_CR1_INIT_NEWSCREEN);
	}


	// logic tick
	if(SNEK_CHECK_BIT(SNEK.CR1,SNEK_CR1_DRAW_OLED))
	{
		SSD1306_Display();
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

	case GAMESTATE_MENUSETTINGS:
		break;

	case GAMESTATE_GAME:
		Snek_Game();
		break;

	case GAMESTATE_OVER:
		break;
	}
}

void Snek_ButtonCallback(uint8_t GPIO_Pin)
{
	// check if there is already movement to the side that button is clicked
	// set button clicked bit in CR1
	switch (GPIO_Pin)
	{
	// DOWN
	case SNEK_BUTTON_DOWN:

		if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_SNEKMOVE_DOWN))
		{
			return;
		}
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_DOWN);
		break;
		// UP
	case SNEK_BUTTON_UP:

		if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_SNEKMOVE_UP))
		{
			return;
		}
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_UP);
		break;
		// LEFT
	case SNEK_BUTTON_LEFT:

		if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_SNEKMOVE_LEFT))
		{
			return;
		}
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_LEFT);
		break;
		// RIGHT
	case SNEK_BUTTON_RIGHT:

		if (SNEK_CHECK_BIT(SNEK.CR1, SNEK_CR1_SNEKMOVE_RIGHT))
		{
			return;
		}
		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_RIGHT);
		break;
		// ENTER
	case SNEK_BUTTON_ENTER:

		SNEK_SET_BIT(SNEK.CR1, SNEK_CR1_BUTTON_ENTER);
		break;
	}

}
