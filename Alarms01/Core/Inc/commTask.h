/*
 * commTask.h
 *
 *  Created on: Oct 20, 2022
 *      Author: student
 */

#ifndef INC_COMMTASK_H_
#define INC_COMMTASK_H_

typedef void (*HanlderFunc)(void *, char *);
void commTaskInit();
void commTaskprintCommands();
void registerCommand(const char * name, HanlderFunc func, void * obj);

#endif /* INC_COMMTASK_H_ */
