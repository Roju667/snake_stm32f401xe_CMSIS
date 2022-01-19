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
#include "stdlib.h"
#include "stdio.h"
#include "Snek.h"

snek_game_t g_snek_game;

static void snek_ui_drawbutton(uint8_t *p_text, uint8_t count, uint8_t select)
{
	// lcd width - char size - spaces between chars
	uint8_t string_start_pos = ((SSD1306_LCDWIDTH - (strlen((char*) p_text) * 5) - strlen((char*) p_text))) / 2;

	GFX_DrawFillRectangle(0, count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, BLACK);
	//draw selected button
	if (select)
	{
		GFX_DrawFillRectangle(0, count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, YELLOWBLUE);
		GFX_DrawString(string_start_pos, (count * SNEK_UI_BUTTON_OFFSET) + SNEK_UI_CHAR_OFFSET, (char*) p_text, BLACK,
		YELLOWBLUE);
	}
	else // draw unselected button
	{
		GFX_DrawRectangle(0, count * SNEK_UI_BUTTON_OFFSET, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, YELLOWBLUE);
		GFX_DrawString(string_start_pos, (count * SNEK_UI_BUTTON_OFFSET) + SNEK_UI_CHAR_OFFSET, (char*) p_text, YELLOWBLUE,
		BLACK);
	}

	return;
}

static void snek_gamestate_init(snek_game_t *p_snek_game)
{

	// switch to menu screen
	p_snek_game->game_state = GAMESTATE_MENUMAIN;
	// set button on first button
	p_snek_game->menu_buttons = BUTTON_START;
	SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_START);
	// set bit to draw menu for the first time
	SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED);
	SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_ERROR_NODE);

}

static void snek_gamesate_mainmenu(snek_game_t *p_snek_game)
{
	// check if there is a menu update
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED))
	{
		//draw SNEK->Buttons
		snek_ui_drawbutton((uint8_t*) "Start", 0, 0);
		snek_ui_drawbutton((uint8_t*) "High scores", 1, 0);
		snek_ui_drawbutton((uint8_t*) "Settings", 2, 0);
		snek_ui_drawbutton((uint8_t*) "About", 3, 0);

		// clear active buttons bits
		p_snek_game->SR1 &= ~(15U << SNEK_SR1_ACTIVE_BUTTON_START);

		//draw selected button
		//set active window bit
		switch (p_snek_game->menu_buttons)
		{
		case (BUTTON_START):
			snek_ui_drawbutton((uint8_t*) "Start", 0, 1);
			SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_START);
			break;

		case (BUTTON_SCORES):
			snek_ui_drawbutton((uint8_t*) "High scores", 1, 1);
			SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_SCORES);
			break;

		case (BUTTON_SETTINGS):
			snek_ui_drawbutton((uint8_t*) "Settings", 2, 1);
			SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_SETTINGS);
			break;

		case (BUTTON_ABOUT):
			snek_ui_drawbutton((uint8_t*) "About", 3, 1);
			SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_ABOUT);
			break;
		}

		// sent data to OLED
		SSD1306_Display();
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED);
	}

	// check if button down is clicked
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_DOWN))
	{
		// move button cursor down
		p_snek_game->menu_buttons = (p_snek_game->menu_buttons + 1) % 4;
		SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_DOWN);
	}

	// check if button up is clicked
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_UP))
	{
		// move button up
		p_snek_game->menu_buttons = (p_snek_game->menu_buttons + 3) % 4;
		SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED);
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_UP);
	}

	// check if enter is clicked
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_ENTER))
	{
		// change menu screen
		if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_START))
		{
			p_snek_game->game_state = GAMESTATE_GAME;
			SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN);
		}
		else if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_SCORES))
		{
			p_snek_game->game_state = GAMESTATE_MENUSCORES;
		}
		else if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_ABOUT))
		{
			p_snek_game->game_state = GAMESTATE_MENUABOUT;
		}
		else if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_ACTIVE_BUTTON_SETTINGS))
		{
			p_snek_game->game_state = GAMESTATE_MENUSETTINGS;
		}

		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_ENTER);

	}
}

/*
 * snek map functions are used to clear and update current game state
 */
static void snek_map_clearmap(snek_game_t *p_snek_game)
{
	memset(p_snek_game->game_map, 0, sizeof(node_t) * SNEK_UI_NO_NODES);
}

static void snek_map_createnode(snek_game_t *p_snek_game, uint8_t new_node_number)
{

	if (!(SNEK_CHECK_NODE_LIMITS(new_node_number)))
	{
		SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ERROR_NODE);
		return;
	}

	p_snek_game->game_map[new_node_number].node_taken = 1;

	// if this first node made
	if (p_snek_game->snek_lenght == 0)
	{
		// to first node assign null to last position
		p_snek_game->game_map[new_node_number].last_node_pos = SNEK_NULL_NODE;

		p_snek_game->tail_address = new_node_number;
	}
	else
	{
		// to the next node assign position of last head
		p_snek_game->game_map[new_node_number].last_node_pos = p_snek_game->head_address;
		// to the last head assign node that we are creating
		p_snek_game->game_map[p_snek_game->head_address].next_node_pos = new_node_number;

	}

	// we are creating head to next node is NULL
	p_snek_game->game_map[new_node_number].next_node_pos = SNEK_NULL_NODE;
	// node that we are creating is new head
	p_snek_game->head_address = new_node_number;
	p_snek_game->snek_lenght++;

}

static void snek_map_deletenode(snek_game_t *p_snek_game, uint8_t last_node_number)
{
	uint8_t temp_node_pos;

	if (!(SNEK_CHECK_NODE_LIMITS(last_node_number)))
	{
		SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_ERROR_NODE);
		return;
	}

	// save new tail to temporary buffer
	temp_node_pos = p_snek_game->game_map[p_snek_game->tail_address].next_node_pos;

	// clear node
	p_snek_game->game_map[p_snek_game->tail_address].last_node_pos = 0;
	p_snek_game->game_map[p_snek_game->tail_address].next_node_pos = 0;
	p_snek_game->game_map[p_snek_game->tail_address].node_taken = 0;

	// assign new tail
	p_snek_game->tail_address = temp_node_pos;
	p_snek_game->game_map[temp_node_pos].last_node_pos = SNEK_NULL_NODE;
	p_snek_game->snek_lenght--;

}

static void snek_map_drawnode(snek_game_t *p_snek_game, uint8_t new_node_number)
{
	snek_map_createnode(p_snek_game, new_node_number);
	// draw node square
	uint8_t temp_x_pos = ((new_node_number % 16) * 8);
	uint8_t temp_y_pos = 16 + ((new_node_number / 16) * 8);
	GFX_DrawFillRectangle(temp_x_pos, temp_y_pos, SNEK_UI_NODE_HEIGHT, SNEK_UI_NODE_WIDTH, YELLOWBLUE);
}

static void snek_map_erasenode(snek_game_t *p_snek_game, uint8_t last_node_number)
{
	snek_map_deletenode(p_snek_game, last_node_number);
	// draw node square
	uint8_t temp_x_pos = ((last_node_number % 16) * 8);
	uint8_t temp_y_pos = 16 + ((last_node_number / 16) * 8);
	GFX_DrawFillRectangle(temp_x_pos, temp_y_pos, SNEK_UI_NODE_HEIGHT, SNEK_UI_NODE_WIDTH, BLACK);
}

static void snek_map_drawfruity(snek_game_t *p_snek_game, uint8_t fruity_node)
{
	// draw node square
	uint8_t temp_x_pos = ((fruity_node % 16) * 8);
	uint8_t temp_y_pos = 16 + ((fruity_node / 16) * 8);

	GFX_DrawFillRoundRectangle(temp_x_pos, temp_y_pos, SNEK_UI_NODE_HEIGHT, SNEK_UI_NODE_WIDTH, 1, YELLOWBLUE);
	//GFX_DrawFillRectangle(temp_x_pos, temp_y_pos, SNEK_UI_NODE_HEIGHT, SNEK_UI_NODE_WIDTH, YELLOWBLUE);
}


static void snek_gamestate_game(snek_game_t *p_snek_game)
{
	// draw first screen
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN))
	{
		// init parameters
		p_snek_game->snek_lenght = 0;
		snek_map_clearmap(p_snek_game);
		SSD1306_Clear(BLACK);
		// draw scorebox as a top button
		GFX_DrawRectangle(0, 0, SNEK_UI_BUTTON_WIDTH, SNEK_UI_BUTTON_HEIGHT, YELLOWBLUE);
		GFX_DrawString(5, 0 + SNEK_UI_CHAR_OFFSET, "SNEK", YELLOWBLUE, BLACK);
		GFX_DrawString(60, 0 + SNEK_UI_CHAR_OFFSET, "SCORE: 0", YELLOWBLUE, BLACK);

		// create snake in game map

		snek_map_drawnode(p_snek_game, 40);
		snek_map_drawnode(p_snek_game, 41);
		snek_map_drawnode(p_snek_game, 42);

		// set first movement as right
		SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_RIGHT);

		SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_NO_FRUITY);

		SSD1306_Display();
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN);

	}

	SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED);

	// game tick
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_DRAW_OLED))
	{
		// check if direction change is required

		// if we are moving vertically
		if ((SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_DOWN)) || (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_UP)))
		{
			// ignore up/down buttons and change snek new movement direction
			SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_DOWN);
			SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_UP);

			if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_LEFT))
			{
				// set new direction, reset old direction and button flag
				SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_LEFT);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_CR1_BUTTON_LEFT);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_DOWN);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_UP);
			}
			else if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_RIGHT))
			{
				SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_RIGHT);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_CR1_BUTTON_RIGHT);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_DOWN);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_UP);
			}
		}
		else if ((SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_LEFT)) || (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_RIGHT)))
		{
			// same if we are in horizontal movement
			SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_LEFT);
			SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_RIGHT);

			if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_UP))
			{
				// set new direction, reset old direction and button flag
				SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_UP);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_CR1_BUTTON_UP);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_LEFT);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_RIGHT);
			}
			else if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_DOWN))
			{
				SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_DOWN);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_CR1_BUTTON_DOWN);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_LEFT);
				SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_RIGHT);
			}
		}

		// create fruity if its neccessary
		if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_NO_FRUITY))
		{
			// look for a place for a fruity node
			do
			{
				p_snek_game->fruity_node = rand() % 96;
			} while (p_snek_game->game_map[p_snek_game->fruity_node].node_taken);

			snek_map_drawfruity(p_snek_game, p_snek_game->fruity_node);
			SNEK_RESET_BIT(p_snek_game->SR1, SNEK_SR1_NO_FRUITY);
		}

		// delay
		for (uint32_t i = 0; i < 840000; i++)
		{

		}

		// calculate next node position in array
		uint8_t temp_new_head;
		if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_DOWN))
		{
			// head = same column + new row
			temp_new_head = (p_snek_game->head_address) % 16 + ((((p_snek_game->head_address / 16) + 1) % 6 * 16));
		}
		else if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_UP))
		{
			// head = same column + new row
			temp_new_head = (p_snek_game->head_address) % 16 + ((((p_snek_game->head_address / 16) + 5) % 6 * 16));
		}
		else if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_LEFT))
		{
			//head = new column + same row
			temp_new_head = (p_snek_game->head_address + 15) % 16 + ((p_snek_game->head_address / 16) * 16);
		}
		else if (SNEK_CHECK_BIT(p_snek_game->SR1, SNEK_SR1_SNEKMOVE_RIGHT))
		{
			//head = new column + same row
			temp_new_head = (p_snek_game->head_address + 1) % 16 + ((p_snek_game->head_address / 16) * 16);
		}

		// check if new node is taken -> if yes then collision
		if (p_snek_game->game_map[temp_new_head].node_taken)
		{
			p_snek_game->game_state = GAMESTATE_OVER;
			SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN);
			return;
		}

		// check if next node is fruity node
		if (p_snek_game->fruity_node == temp_new_head)
		{
			SNEK_SET_BIT(p_snek_game->SR1, SNEK_SR1_NO_FRUITY);

		}
		else
		{
			// erase tail
			snek_map_erasenode(p_snek_game, p_snek_game->tail_address);
		}

		snek_map_drawnode(p_snek_game, temp_new_head);

		uint8_t temp_msg[8];
		sprintf((char*) temp_msg, "%d", (p_snek_game->snek_lenght - 3));
		GFX_DrawString(102, 0 + SNEK_UI_CHAR_OFFSET, (char*) temp_msg, YELLOWBLUE, BLACK);

		SSD1306_Display();
	}
}

static void snek_gamestate_over(snek_game_t *p_snek_game)
{
	// init game over screen
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN))
	{
		// game over animation
		for (uint8_t count = 0; count < SSD1306_LCDWIDTH; count++)
		{
			GFX_DrawFillRectangle((count % 16) * 8, (count / 16) * 8, SNEK_UI_NODE_WIDTH, SNEK_UI_NODE_HEIGHT, YELLOWBLUE);

			SSD1306_Display();
		}

		uint8_t msg[16];

		sprintf((char*) msg, "Score: %d", (p_snek_game->snek_lenght - 3));

		// draw ui
		snek_ui_drawbutton((uint8_t*) "Game over", 0, 1);
		snek_ui_drawbutton((uint8_t*) msg, 1, 1);
		snek_ui_drawbutton((uint8_t*) "Name:", 2, 1);
		snek_ui_drawbutton((uint8_t*) "", 3, 0);

		// draw ok button
		GFX_DrawRectangle(SNEK_UI_OK_BUTTON_POS_X, SNEK_UI_OK_BUTTON_POS_Y, SNEK_UI_OK_BUTTON_WIDTH, SNEK_UI_OK_BUTTON_HEIGHT, YELLOWBLUE);
		GFX_DrawString(SNEK_UI_OK_BUTTON_POS_X + 4, SNEK_UI_OK_BUTTON_POS_Y + 1, "OK", YELLOWBLUE, BLACK);
		// init text editor values
		p_snek_game->txt_edit.pos = 0;
		p_snek_game->txt_edit.val = 'A';
		memset(p_snek_game->txt_edit.name, ' ', 16);

		SSD1306_Display();
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN);

	}

	// delay
	for (uint32_t i = 0; i < 840000; i++)
	{

	}

	// check if there is a letter to change
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_UP))
	{
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_UP);
		(p_snek_game->txt_edit.val > 'Z') ? p_snek_game->txt_edit.val = 'A' : p_snek_game->txt_edit.val++;
	}
	else if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_DOWN))
	{
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_DOWN);
		(p_snek_game->txt_edit.val < 'A') ? p_snek_game->txt_edit.val = 'Z' : p_snek_game->txt_edit.val--;
	}

	// check if there is position to change
	if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_RIGHT))
	{
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_RIGHT);
		(p_snek_game->txt_edit.pos == SNEK_UI_MAX_NAME_LENGHT) ? p_snek_game->txt_edit.pos = 0 : p_snek_game->txt_edit.pos++;
		p_snek_game->txt_edit.val = p_snek_game->txt_edit.name[p_snek_game->txt_edit.pos];
	}
	else if (SNEK_CHECK_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_LEFT))
	{
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_LEFT);
		(p_snek_game->txt_edit.pos == 0) ? p_snek_game->txt_edit.pos = SNEK_UI_MAX_NAME_LENGHT : p_snek_game->txt_edit.pos--;
		p_snek_game->txt_edit.val = p_snek_game->txt_edit.name[p_snek_game->txt_edit.pos];
	}

	// jump between letters
	if (p_snek_game->txt_edit.pos != SNEK_UI_MAX_NAME_LENGHT)
	{

		GFX_DrawChar((p_snek_game->txt_edit.pos * 6) + SNEK_UI_TXT_EDITOR_OFFSET_LEFT, SSD1306_LCDHEIGHT - SNEK_UI_TXT_EDITOR_OFFSET_BOTTOM,
				p_snek_game->txt_edit.val, YELLOWBLUE,
				BLACK);

		// power off ok button
		GFX_DrawFillRectangle(SNEK_UI_OK_BUTTON_POS_X, SNEK_UI_OK_BUTTON_POS_Y, SNEK_UI_OK_BUTTON_WIDTH, SNEK_UI_OK_BUTTON_HEIGHT, BLACK);
		GFX_DrawRectangle(SNEK_UI_OK_BUTTON_POS_X, SNEK_UI_OK_BUTTON_POS_Y, SNEK_UI_OK_BUTTON_WIDTH, SNEK_UI_OK_BUTTON_HEIGHT, YELLOWBLUE);
		GFX_DrawString(SNEK_UI_OK_BUTTON_POS_X + 4, SNEK_UI_OK_BUTTON_POS_Y + 1, "OK", YELLOWBLUE, BLACK);
	}
	else
	{
		// highlight ok button
		GFX_DrawFillRectangle(SNEK_UI_OK_BUTTON_POS_X, SNEK_UI_OK_BUTTON_POS_Y, SNEK_UI_OK_BUTTON_WIDTH, SNEK_UI_OK_BUTTON_HEIGHT, YELLOWBLUE);
		GFX_DrawRectangle(SNEK_UI_OK_BUTTON_POS_X, SNEK_UI_OK_BUTTON_POS_Y, SNEK_UI_OK_BUTTON_WIDTH, SNEK_UI_OK_BUTTON_HEIGHT, BLACK);
		GFX_DrawString(SNEK_UI_OK_BUTTON_POS_X + 4, SNEK_UI_OK_BUTTON_POS_Y + 1, "OK", BLACK, YELLOWBLUE);
	}

	p_snek_game->txt_edit.name[p_snek_game->txt_edit.pos] = p_snek_game->txt_edit.val;

	SSD1306_Display();

	// check if button enter is clicked and go back to main menu
	if (SNEK_CHECK_BIT(p_snek_game->CR1,SNEK_CR1_BUTTON_ENTER) && (p_snek_game->txt_edit.pos == SNEK_UI_MAX_NAME_LENGHT))
	{
		for (uint32_t i = 0; i < 840000; i++)
		{

		}
		// save data on eeprom
		p_snek_game->game_state = GAMESTATE_SAVE;
		SNEK_RESET_BIT(p_snek_game->CR1, SNEK_CR1_BUTTON_ENTER);
	}
}

static void snek_gamesate_save(snek_game_t *p_snek_game)
{
	// read scores from eeprom
	// check if score is qualifying to top
	// send new scores to eeprom

	SNEK_SET_BIT(p_snek_game->CR1, SNEK_CR1_INIT_NEWSCREEN);
	p_snek_game->game_state = GAMESTATE_MENUMAIN;
}

void snek(void)
{
	switch (g_snek_game.game_state)
	{
	case GAMESTATE_INIT:
		snek_gamestate_init(&g_snek_game);
		break;

	case GAMESTATE_MENUMAIN:
		snek_gamesate_mainmenu(&g_snek_game);
		break;

	case GAMESTATE_MENUSCORES:
		break;

	case GAMESTATE_MENUABOUT:
		break;

	case GAMESTATE_MENUSETTINGS:
		break;

	case GAMESTATE_GAME:
		snek_gamestate_game(&g_snek_game);
		break;

	case GAMESTATE_OVER:
		snek_gamestate_over(&g_snek_game);
		break;

	case GAMESTATE_SAVE:
		snek_gamesate_save(&g_snek_game);
		break;

	default:
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

		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_DOWN);
		break;
		// UP
	case SNEK_BUTTON_UP:

		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_UP);
		break;
		// LEFT
	case SNEK_BUTTON_LEFT:

		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_LEFT);
		break;
		// RIGHT
	case SNEK_BUTTON_RIGHT:

		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_RIGHT);
		break;
		// ENTER
	case SNEK_BUTTON_ENTER:

		SNEK_SET_BIT(g_snek_game.CR1, SNEK_CR1_BUTTON_ENTER);
		break;
	}

}
