/*
 * Snek.c
 *
 *  Created on: 17 sty 2022
 *      Author: ROJEK
 */


#include "Snek.h"
#include "SSD1306_OLED.h"
#include "GFX_BW.h"
#include "string.h"


GameState_t State;
MenuButtons_t Buttons;

static uint16_t Snek_DrawButton (uint8_t* Text, uint8_t Count, uint8_t Select)
{
	uint8_t _tempX = ((SSD1306_LCDWIDTH - 1 - (strlen((char*)Text) * 5) + strlen((char*)Text) - 1)) / 2;

	//draw selected button
	if(Select)
	{
		GFX_DrawFillRectangle(0, Count * 16, 128, 15, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(_tempX, (Count * 16) + 4, (char*)Text, SSD1306_COLOR_BLACK, SSD1306_COLOR_YELLOWBLUE);
	}else // draw unselected button
	{
		GFX_DrawRectangle(0, Count * 16, 128, 15, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(_tempX, (Count * 16) + 4, (char*)Text, SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
	}


	return 0;
}

static void Snek_Init(void)
{


	// switch to menu screen
	State = GAMESTATE_MENUMAIN;
	// set button on first button
	Buttons = BUTTON_START;

}

static void Snek_MenuMain (void)
{
	//draw buttons
	Snek_DrawButton((uint8_t*)"Start", 0, 0);
	Snek_DrawButton((uint8_t*)"High scores", 1, 0);
	Snek_DrawButton((uint8_t*)"Settings", 2, 0);
	Snek_DrawButton((uint8_t*)"About", 3, 0);

	//draw selected button
	switch(Buttons)
	{
	case(BUTTON_START):
		Snek_DrawButton((uint8_t*)"Start", 0, 1);
		break;

	case(BUTTON_SCORES):
		Snek_DrawButton((uint8_t*)"High scores", 1, 1);
		break;

	case(BUTTON_SETTINGS):
		Snek_DrawButton((uint8_t*)"Settings", 2, 1);
		break;

	case(BUTTON_ABOUT):
		Snek_DrawButton((uint8_t*)"About", 3, 1);
		break;
	}


	SSD1306_Display();
}

void Snek(void)
{
	switch(State)
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
