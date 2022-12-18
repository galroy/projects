#include "Buzzer.h"
#include "main.h"
#include "cmsis_os.h"
#include "Manager.h"

extern Manager *m;

extern "C" void Entry_Buzzer(void *argument)
{

  for(;;)
  {
	  if(m->getBuzzer()->getState() == BUZZ_ON){
		  HAL_TIM_PWM_Start_IT(m->getBuzzer()->getHtim(), TIM_CHANNEL_1);
		  m->getBuzzer()->nextSound();
	  }
    osDelay(m->getBuzzer()->getDelay());
  }

}

void Buzzer::playSound(int index)
{
	__HAL_TIM_SET_COUNTER(m_htim, 0);
	__HAL_TIM_SET_AUTORELOAD(m_htim, m_sound[index]);
	__HAL_TIM_SET_COMPARE(m_htim, TIM_CHANNEL_1, (m_sound[index])/2);
}

void Buzzer::nextSound()
{
	m_currSound++;
	if(m_currSound >= m_soundArrSize){
		m_currSound = 0;
	}
	playSound(m_currSound);
}

void Buzzer::off()
{

		HAL_TIM_Base_Stop_IT(m_htim);
		HAL_TIM_PWM_Stop_IT(m_htim, TIM_CHANNEL_1);
		m_state = BUZZ_OFF;
		m_delay = 1;
}




