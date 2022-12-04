/*
 * logger.h
 *
 *  Created on: Nov 30, 2022
 *      Author: USER-PC
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdarg.h>

#define VERBOSITY_INFO 0x0
#define VERBOSITY_ERROR 0x1
#define VERBOSITY_DEBUG 0x2
#define VERBOSITY VERBOSITY_INFO

#if VERBOSITY >= VERBOSITY_INFO
#define LOG_INFO(FORMAT, ...) printf(FORMAT, ## __VA_ARGS__)
#else
#define LOG_INFO(FORMAT, ...)
#endif



#if VERBOSITY >= VERBOSITY_ERROR
#define LOG_ERROR(FORMAT, ...) printf(FORMAT, ## __VA_ARGS__)
#else
#define LOG_ERROR(FORMAT, ...)
#endif

#if VERBOSITY >= VERBOSITY_DEBUGGER
#define LOG_DEBUG(FORMAT, ...) printf(FORMAT, ## __VA_ARGS__)
#else
#define LOG_DEBUG(FORMAT, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_LOGGER_H_ */


