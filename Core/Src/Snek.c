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

snek_game_t g_snek_game;

static void snek_ui_drawbutton(uint8_t *p_text, uint8_t count, uint8_t select)
{
	// lcd width - char size - spaces between chars
	uint8_t string_start_pos = ((SSD1306_LCDWIDTH - (strlen((char*) p_text) * 5) - strlen((char*) p_text))) / 2;

	GFX_DrawFillRectangle(0, count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_BLACK);
	//draw selected button
	if (select)
	{
		GFX_DrawFillRectangle(0, count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(string_start_pos, (count * SNEK_UI_BUTTON_OFFSET) + SNEK_UI_CHAR_OFFSET, (char*) p_text, SSD1306_COLOR_BLACK, SSD1306_COLOR_YELLOWBLUE);
	}
	else // draw unselected button
	{
		GFX_DrawRectangle(0, count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(string_start_pos, (count * SNEK_UI_BUTTON_OFFSET) + SNEK_UI_CHAR_OFFSET, (char*) p_text, SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
	}

	return;
}

static void snek_gamestate_init(void)
{

	// switch to menu screen
	g_snek_game.game_state = GAMESTATE_MENUMAIN;
	// set button on first button
	g_snek_game.menu_buttons = BUTTON_START;
	SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_START);
	// set bit to draw menu for the first time
	SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_DRAW_OLED);

}

static void snek_gamesate_mainmenu(void)
{
	// check if there is a menu update
	if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_DRAW_OLED))
	{
		//draw SNEK->Buttons
		snek_ui_drawbutton((uint8_t*) "Start", 0, 0);
		snek_ui_drawbutton((uint8_t*) "High scores", 1, 0);
		snek_ui_drawbutton((uint8_t*) "Settings", 2, 0);
		snek_ui_drawbutton((uint8_t*) "About", 3, 0);

		// clear active buttons bits
		g_snek_game.CR1 &= ~(15U << SNEK_CR1_ACTIVE_BUTTON_START);

		//draw selected button
		//set active window bit
		switch (g_snek_game.menu_buttons)
		{
		case (BUTTON_START):
			snek_ui_drawbutton((uint8_t*) "Start", 0, 1);
			SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_START);
			break;

		case (BUTTON_SCORES):
			snek_ui_drawbutton((uint8_t*) "High scores", 1, 1);
			SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_SCORES);
			break;

		case (BUTTON_SETTINGS):
			snek_ui_drawbutton((uint8_t*) "Settings", 2, 1);
			SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_SETTINGS);
			break;

		case (BUTTON_ABOUT):
			snek_ui_drawbutton((uint8_t*) "About", 3, 1);
			SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_ABOUT);
			break;
		}

		// sent data to OLED
		SSD1306_Display();
		SNEK_RESET_BIT(g_snek_game.CR1, SNEK_CR1_DRAW_OLED);
	}

	// check if button down is clicked
	if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_DOWN))
	{
		// move button cursor down
		g_snek_game.menu_buttons = (g_snek_game.menu_buttons + 1) % 4;
		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_DOWN);
	}

	// check if button up is clicked
	if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_UP))
	{
		// move button up
		g_snek_game.menu_buttons = (g_snek_game.menu_buttons + 3) % 4;
		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_UP);
	}

	// check if enter is clicked
	if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_ENTER))
	{
		// change menu screen
		if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_START))
		{
			g_snek_game.game_state = GAMESTATE_GAME;
			SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_INIT_NEWSCREEN);
		}
		else if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_SCORES))
		{
			g_snek_game.game_state = GAMESTATE_MENUSCORES;
		}
		else if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_ABOUT))
		{
			g_snek_game.game_state = GAMESTATE_MENUABOUT;
		}
		else if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_ACTIVE_BUTTON_SETTINGS))
		{
			g_snek_game.game_state = GAMESTATE_MENUSETTINGS;
		}

	}
}

/*
 * snek map functions are used to clear and update current game state
 */
static void snek_map_clearmap(snek_game_t *p_snek_game)
{
	memset(p_snek_game->game_map, 0, sizeof(node_t) * SNEK_UI_NO_NODES);
}

static void snek_map_createnode(snek_game_t *p_snek_game)
{
	// map dimensions 16 x 6
	// if this first node made
	if (p_snek_game->snek_lenght == 0)
	{
		// create head somewhere in the middle
		p_snek_game->game_map[SNEK_GAME_HEAD_STARTPOS]->node_taken = 1;
		p_snek_game->game_map[SNEK_GAME_HEAD_STARTPOS]->last_node_pos = NULL;
		p_snek_game->head_address = SNEK_GAME_HEAD_STARTPOS;
		p_snek_game->snek_lenght++;
		return;
	}

	// calculate next node position in array

	// movements    down (node_pos + 16) % 80
	//				up	 (node_pos + 80) % 81
	//				right (node_pos + 1) % 15
	// 				left (node_pos + 15) % 16

	// check if new node is taken -> if yes then collision


}

static void snek_gamestate_game(void)
{
	// draw first screen
	if(SNEK_CR1_INIT_NEWSCREEN)
	{

	// set snake lenght as 0
		g_snek_game.game_map->snek_lenght = 0;
	// clear snake map
		snek_map_clearmap(&g_snek_game);
	// clear oled screen
		SSD1306_Clear(SSD1306_COLOR_BLACK);
		// draw scorebox as a top button
		GFX_DrawRectangle(0, 0, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, SSD1306_COLOR_YELLOWBLUE);
		GFX_DrawString(5, 0 + SNEK_UI_CHAR_OFFSET, "SNEK", SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
		GFX_DrawString(50, 0 + SNEK_UI_CHAR_OFFSET, "SCORE:", SSD1306_COLOR_YELLOWBLUE, SSD1306_COLOR_BLACK);
		// create snake in game map


		SSD1306_Display();
		SNEK_RESET_BIT(g_snek_game.CR1, SNEK_CR1_INIT_NEWSCREEN);
	}


	// logic tick
	if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_DRAW_OLED))
	{
		SSD1306_Display();
	}
}


void snek(void)
{
	switch (g_snek_game.game_state)
	{
	case GAMESTATE_INIT:
		snek_gamestate_init();
		break;

	case GAMESTATE_MENUMAIN:
		snek_gamesate_mainmenu();
		break;

	case GAMESTATE_MENUSCORES:
		break;

	case GAMESTATE_MENUABOUT:
		break;

	case GAMESTATE_MENUSETTINGS:
		break;

	case GAMESTATE_GAME:
		snek_gamestate_game();
		break;

	case GAMESTATE_OVER:
		break;
	}
}

void snek_button_callback(uint8_t GPIO_Pin)
{
	// check if there is already movement to the side that button is clicked
	// set button clicked bit in CR1
	switch (GPIO_Pin)
	{
	// DOWN
	case SNEK_BUTTON_DOWN:

		if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_SNEKMOVE_DOWN))
		{
			return;
		}
		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_DOWN);
		break;
		// UP
	case SNEK_BUTTON_UP:

		if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_SNEKMOVE_UP))
		{
			return;
		}
		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_UP);
		break;
		// LEFT
	case SNEK_BUTTON_LEFT:

		if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_SNEKMOVE_LEFT))
		{
			return;
		}
		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_LEFT);
		break;
		// RIGHT
	case SNEK_BUTTON_RIGHT:

		if (SNEK_CHECK_BIT(g_snek_game.CR1, SNEK_CR1_SNEKMOVE_RIGHT))
		{
			return;
		}
		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_RIGHT);
		break;
		// ENTER
	case SNEK_BUTTON_ENTER:

		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_ENTER);
		break;
	}

}
