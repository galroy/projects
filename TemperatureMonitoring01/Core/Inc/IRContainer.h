/*
 * IRContainer.h
 *
 *  Created on: Dec 28, 2022
 *      Author: USER-PC
 */

#ifndef INC_IRCONTAINER_H_
#define INC_IRCONTAINER_H_
#include "IRCommand.h"

enum IRCmd
{
	MAX_IR_COMMANDS = 10
};

enum IRCommands
{
	IR_COMMAND_1 = 2155821255,
	IR_COMMAND_2 = 2155853895,
	IR_COMMAND_3 = 2155837575,
	IR_COMMAND_4 = 2155817175,
	IR_COMMAND_5 = 2155866135,
	IR_COMMAND_6 = 2155813095,
	IR_COMMAND_7 = 2155842165,
	IR_COMMAND_8 = 2155825845,
	IR_COMMAND_9 = 2155858485,
	IR_COMMAND_0 = 2155833495

};

class IRContainer {
private:
	IRCommand* m_pIRCommands[MAX_IR_COMMANDS];
	int m_commandCount;
public:
	IRContainer(){
		m_commandCount = 0;
		memset(m_pIRCommands, 0, sizeof(IRCommand*) * MAX_IR_COMMANDS);
	}
	virtual ~IRContainer(){
		for (int i = 0; i < m_commandCount; i++) {
			if (m_pIRCommands[i] != 0) {
				delete m_pIRCommands[i];
			}
		}
	}

public:
	int getCommandCount(){return m_commandCount;}
		void addCommand(IRCommand *IRCommand) {
			m_pIRCommands[m_commandCount] = IRCommand;
			m_commandCount++;
		}
		bool callCommand(uint32_t commandNum) {

				for (int i = 0; i < m_commandCount; i++) {
					if (m_pIRCommands[i]->getCommandNum()== commandNum) {
						m_pIRCommands[i]->doCommand();
						return true;
					}
				}
				return false;
			}

};

#endif /* INC_IRCONTAINER_H_ */
