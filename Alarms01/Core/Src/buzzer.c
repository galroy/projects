/*
 * buzzer.c
 *
 *  Created on: Sep 13, 2022
 *      Author: student
 */

#include "buzzer.h"
#include "cmsis_os.h"
#include "main.h"



#define MUSIC_SIZE 49

FREQUENCY music[MUSIC_SIZE] = {Sol, Mi, Mi, Fa, Re, Re, DO, Re, Mi, Fa, Sol, Sol, Sol, // little jonathan
								Sol, Mi, Mi, Fa, Re, Re, DO, Mi, Sol, Sol, DO,
								Re, Re, Re, Re, Re, Mi, Fa,
								Mi, Mi, Mi, Mi, Mi, Fa, Sol,
								Sol, Mi, Mi, Fa, Re, Re, DO, Mi, Sol, Sol, DO};


void buzzerInit(Buzzer *buzzer, TIM_HandleTypeDef* timer, int channel)
{
	buzzer->status = BUZZER_OFF;
	buzzer->nextNote = NONE;
	buzzer->pwmTimer = timer;
	buzzer->channel = channel;
	buzzer->nextNote = 0;
	buzzer->delay = 500;
}



void buzzerOn(Buzzer *buzzer)
{
	buzzer->status = BUZZER_ON;
	HAL_TIM_Base_Start(buzzer->pwmTimer);
	HAL_TIM_PWM_Start(buzzer->pwmTimer, buzzer->channel);
}

void buzzerOff(Buzzer *buzzer)
{
	buzzer->status = BUZZER_OFF;
	HAL_TIM_Base_Stop(buzzer->pwmTimer);
	HAL_TIM_PWM_Stop(buzzer->pwmTimer, buzzer->channel);
	buzzer->nextNote = NONE;
}

void startPlayBzr(void *argo) {
	Buzzer *buzz = (Buzzer*) argo;
	for (;;) {

		while (buzz->status == BUZZER_OFF) {
			osThreadYield();
		}
		int freq = music[buzz->nextNote] / 2;
		__HAL_TIM_SET_COUNTER(buzz->pwmTimer, 0);
		__HAL_TIM_SET_AUTORELOAD(buzz->pwmTimer, freq);
		__HAL_TIM_SET_COMPARE(buzz->pwmTimer, buzz->channel, (freq / 2));
		buzz->nextNote = (buzz->nextNote + 1) % MUSIC_SIZE;
		osDelay(buzz->delay);
	}
}


