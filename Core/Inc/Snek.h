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

#define SNEK_CR1_BUTTON_DOWN					0U
#define SNEK_CR1_BUTTON_UP						1U
#define SNEK_CR1_BUTTON_LEFT					2U
#define SNEK_CR1_BUTTON_RIGHT					3U
#define SNEK_CR1_SNEKMOVE_DOWN					4U
#define SNEK_CR1_SNEKMOVE_UP					5U
#define SNEK_CR1_SNEKMOVE_LEFT					6U
#define SNEK_CR1_SNEKMOVE_RIGHT					7U
#define SNEK_CR1_BUTTON_ENTER					8U
#define SNEK_CR1_DRAW_OLED						9U
#define SNEK_CR1_ACTIVE_BUTTON_START			10U
#define SNEK_CR1_ACTIVE_BUTTON_SCORES			11U
#define SNEK_CR1_ACTIVE_BUTTON_SETTIGNS			12U
#define SNEK_CR1_ACTIVE_BUTTON_ABOUT			13U

#define SNEK_BUTTON_DOWN			GPIO_PIN_2
#define SNEK_BUTTON_UP				GPIO_PIN_14
#define SNEK_BUTTON_LEFT			GPIO_PIN_15
#define SNEK_BUTTON_RIGHT			GPIO_PIN_1
#define SNEK_BUTTON_ENTER			GPIO_PIN_13

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
