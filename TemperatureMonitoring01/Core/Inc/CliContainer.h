/*
 * CliContainer.h
 *
 *  Created on: 7 בנוב׳ 2022
 *      Author: USER-PC
 */

#ifndef INC_CLICONTAINER_H_
#define INC_CLICONTAINER_H_

#define MAX_CLI_COMMANDS 12

#include "CliCommand.h"

class CliContainer {
private:
	CliCommand* m_pCliCommands[MAX_CLI_COMMANDS];
	int m_commandCount;
public:
	CliContainer() {
		m_commandCount = 0;
		memset(m_pCliCommands, 0, sizeof(CliCommand*) * MAX_CLI_COMMANDS);
	}
	~CliContainer() {
		for (int i = 0; i < m_commandCount; i++) {
			if (m_pCliCommands[i] != 0) {
				delete m_pCliCommands[i];
			}
		}
	}
	CliCommand* getCliCommands(int i){return m_pCliCommands[i];}
	int getCommandCount(){return m_commandCount;}
	void addCommand(CliCommand *cliCommand) {
		m_pCliCommands[m_commandCount] = cliCommand;
		m_commandCount++;
	}

	bool callCommand(char *commandName, char *param) {

		for (int i = 0; i < m_commandCount; i++) {
			if (strcmp(m_pCliCommands[i]->getCommandName(), commandName) == 0) {
				m_pCliCommands[i]->doCommand(param);
				return true;
			}
		}
		return false;
	}

};

#endif /* INC_CLICONTAINER_H_ */
