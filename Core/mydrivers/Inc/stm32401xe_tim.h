/*
 * stm32401xe_tim.h
 *
 *  Created on: Jan 21, 2022
 *      Author: ROJEK
 */
#include "stm32f401xe.h"

typedef struct TimerConfig_t
{

	uint16_t prescaler;

	uint16_t autoreload;

	TIM_TypeDef *p_timx;

}TimerConfig_t;
