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
}GameState_t;

typedef enum
{
	BUTTON_START,
	BUTTON_SCORES,
	BUTTON_SETTINGS,
	BUTTON_ABOUT
}MenuButtons_t;

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

typedef struct Snek
{
	// enum state machine
	GameState_t State;
	// enum buttons names
	MenuButtons_t Buttons;
	// control register 1
	uint16_t CR1;
} Snek_t;


void Snek(void);
void Snek_ButtonCallback(uint8_t GPIO_Pin);

#endif /* INC_SNEK_H_ */
