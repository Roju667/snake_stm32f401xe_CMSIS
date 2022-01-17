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

void Snek(void);

#endif /* INC_SNEK_H_ */
