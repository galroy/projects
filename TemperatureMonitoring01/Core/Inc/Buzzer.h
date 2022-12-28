#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include "main.h"
#include <stdio.h>

enum BuzzerState
{
	BUZZ_ON,
	BUZZ_OFF
};

enum BuzzerTone{
	BUZ_LOW = 202,
	BUZ_HIGH = 390
};

enum temperatureAlarmStat{
	TAS_OFF,
	TAS_OFF_BY_USER,
	TAS_ON
};

class Buzzer
{
private:
	int m_sound[2] = {BUZ_HIGH, BUZ_LOW};
	size_t m_currSound;
	size_t m_soundArrSize;
	BuzzerState m_state;
	TIM_HandleTypeDef* m_htim;
	int m_delay;
	temperatureAlarmStat m_temperatureAlarmStat;
public:
	Buzzer(TIM_HandleTypeDef* htim){
		m_currSound = 0;
		m_delay = 1;
		m_soundArrSize = sizeof(m_sound)/sizeof(m_sound[0]);
		m_state = BUZZ_OFF;
		m_htim = htim;
		m_temperatureAlarmStat = TAS_OFF;
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
	void setTemperatureAlarmStat(temperatureAlarmStat stat){m_temperatureAlarmStat = stat;}
	temperatureAlarmStat getTemperatureAlarmStat(){return m_temperatureAlarmStat;}
private:
	void playSound(int index);


};

#endif /* INC_BUZZER_H_ */
