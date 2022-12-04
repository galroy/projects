#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"
#include <stdio.h>

enum BuzzerState
{
	BUZZ_ON,
	BUZZ_OFF
};
#define BUZ_LOW 202
#define BUZ_HIGH 390

class Buzzer
{
private:
	int m_sound[2] = {BUZ_HIGH, BUZ_LOW};
	size_t m_currSound;
	size_t m_soundArrSize;
	BuzzerState m_state;
	TIM_HandleTypeDef* m_htim;
	int m_delay;
public:
	Buzzer(TIM_HandleTypeDef* htim){
		m_currSound = 0;
		m_delay = 1;
		m_soundArrSize = sizeof(m_sound)/sizeof(m_sound[0]);
		m_state = BUZZ_OFF;
		m_htim = htim;

	}
	~Buzzer(){}
	void nextSound();

	void off();
	void on(){
		m_state = BUZZ_ON;
		m_delay = 500;
	}
	BuzzerState getState(){return m_state;}
	TIM_HandleTypeDef* getHtim(){return m_htim;}
	int getDelay(){return m_delay;}
private:
	void playSound(int index);


};

#endif /* INC_BUZZER_H_ */
