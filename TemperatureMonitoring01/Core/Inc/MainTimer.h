/*
 * MainTimer.h
 *
 *  Created on: 8 Nov 2022
 *      Author: student
 */

#ifndef INC_MAINTIMER_H_
#define INC_MAINTIMER_H_

#include "TimerTask.h"
enum MaxTask{
	 MAX_TIMER_TASKS = 10
};

class MainTimer {
private:
	TimerTask* m_timerTasks[MAX_TIMER_TASKS];
	int m_timerCount;
public:
	MainTimer() {
		m_timerCount = 0;

	}
	~MainTimer(){}
	void addTimerTask(TimerTask *pTimerTask) {
		if (getTimerTaskIndex(pTimerTask) == -1) {
			m_timerTasks[m_timerCount] = pTimerTask;
			m_timerCount++;
		}

	}

	void deleteTimerTask(TimerTask *pTimerTask) {
		int i = getTimerTaskIndex(pTimerTask);
		if (i != -1) {
			m_timerCount--;
			if (i == m_timerCount) {
				m_timerTasks[i] = { 0 };
			} else {
				m_timerTasks[i] = m_timerTasks[m_timerCount];
				m_timerTasks[m_timerCount] = { 0 };
			}
		}

	}
	void callTimerFunc() {
		for (int i = 0; i < m_timerCount; i++) {
			m_timerTasks[i]->timerFunc();
		}
	}

private:
	int getTimerTaskIndex(TimerTask *pTimerTask) {
		for (int i = 0; i < m_timerCount; i++) {
			if (m_timerTasks[i] == pTimerTask) {
				return i;
			}
		}
		return -1;
	}

};

#endif /* INC_MAINTIMER_H_ */

