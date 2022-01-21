/*
 * stm32401xe_tim.c
 *
 *  Created on: Jan 21, 2022
 *      Author: ROJEK
 */

#include "stm32401xe_tim.h"

void TIM_InitTimer(TimerConfig_t *p_handle_timer)
{
	// reset flags
	p_handle_timer->p_timx->SR = 0;

	// set prescaler
	p_handle_timer->p_timx->PSC = p_handle_timer->prescaler;

	// set arr
	p_handle_timer->p_timx->ARR = p_handle_timer->autoreload;

	// start timer
	p_handle_timer->p_timx->CR1 |= TIM_CR1_CEN;
}
