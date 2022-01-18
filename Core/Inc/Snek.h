/*
 * Snek.h
 *
 *  Created on: 17 sty 2022
 *      Author: ROJEK
 */

#ifndef INC_SNEK_H_
#define INC_SNEK_H_


typedef enum
{
	GAMESTATE_INIT,
	GAMESTATE_MENUMAIN,
	GAMESTATE_MENUSCORES,
	GAMESTATE_MENUABOUT,
	GAMESTATE_MENUSETTINGS,
	GAMESTATE_GAME,
	GAMESTATE_OVER
} game_state_t;

typedef enum
{
	BUTTON_START,
	BUTTON_SCORES,
	BUTTON_SETTINGS,
	BUTTON_ABOUT
} menu_buttons_t;

#define SNEK_SET_BIT(ControlRegister,Bit)		(ControlRegister |= (1U << Bit))
#define SNEK_RESET_BIT(ControlRegister,Bit)		(ControlRegister &= ~(1U << Bit))
#define SNEK_CHECK_BIT(ControlRegister,Bit)		((ControlRegister >> Bit) & 1U)

#define SNEK_CR1_BUTTON_DOWN					0U		// button down clicked
#define SNEK_CR1_BUTTON_UP						1U		// button up clicked
#define SNEK_CR1_BUTTON_LEFT					2U		// button left clicked
#define SNEK_CR1_BUTTON_RIGHT					3U		// button right clicked
#define SNEK_CR1_SNEKMOVE_DOWN					4U		// snek is moving down
#define SNEK_CR1_SNEKMOVE_UP					5U		// snek is moving up
#define SNEK_CR1_SNEKMOVE_LEFT					6U		// snek is moving left
#define SNEK_CR1_SNEKMOVE_RIGHT					7U		// snek is moving right
#define SNEK_CR1_BUTTON_ENTER					8U		// button enter clicked
#define SNEK_CR1_DRAW_OLED						9U		// refresh screen with new values
#define SNEK_CR1_ACTIVE_BUTTON_START			10U		// main menu cursor on button start
#define SNEK_CR1_ACTIVE_BUTTON_SCORES			11U		// main menu cursor on button scores
#define SNEK_CR1_ACTIVE_BUTTON_SETTINGS			12U		// main menu cursor on button settings
#define SNEK_CR1_ACTIVE_BUTTON_ABOUT			13U		// main menu cursor on button about
#define SNEK_CR1_INIT_NEWSCREEN					14U		// information to draw screen once

#define SNEK_BUTTON_DOWN						GPIO_PIN_2
#define SNEK_BUTTON_UP							GPIO_PIN_14
#define SNEK_BUTTON_LEFT						GPIO_PIN_15
#define SNEK_BUTTON_RIGHT						GPIO_PIN_1
#define SNEK_BUTTON_ENTER						GPIO_PIN_13


#define SNEK_UI_BUTTON_WIDTH					128U
#define SNEK_UI_BUTTON_HEIGHT					15U
#define SNEK_UI_BUTTON_OFFSET					16U		// number of pixels from 1 button start to next one
#define SNEK_UI_CHAR_OFFSET						4U		// number of pixels from button border to char

//we take 48 x 128 game screen and divide it to 8x8 suqares that will be food or snake parts
#define SNEK_UI_GAME_HEIGHT						48U
#define SNEK_UI_GAME_WIDTH						128U
#define SNEK_UI_NODE_HEIGHT						8U
#define SNEK_UI_NODE_WIDTH						8U
#define SNEK_UI_NO_NODES						((SNEK_UI_GAME_HEIGHT / SNEK_UI_NODE_HEIGHT) * (SNEK_UI_GAME_WIDTH / SNEK_UI_NODE_WIDTH))

#define SNEK_GAME_HEAD_STARTPOS					8 + (3 * 16)

// linked list to save snek shape and fruity
typedef struct node_t
{

	uint8_t node_taken; // node taken flag

	uint8_t node_x; // y position

	uint8_t node_y; // x position

	uint8_t node_array_pos;  // position in the array x + (y*16)

	uint8_t last_node_pos; // last node position in array

	uint8_t next_node_pos; // next node position in array

} node_t;


typedef struct snek_game_t
{
	// enum state machine
	game_state_t game_state;
	// enum buttons names
	menu_buttons_t menu_buttons;
	// control register 1
	volatile uint16_t CR1;
	// game map that will contain snake shape and fruit
	node_t game_map[SNEK_UI_NO_NODES];
	// snek first node 'address'
	uint8_t head_address;
	// snek last node
	uint8_t tail_address;
	// snek lenght
	uint16_t snek_lenght;

} snek_game_t;


void snek(void);
void snek_button_callback(uint8_t GPIO_Pin);

#endif /* INC_SNEK_H_ */
