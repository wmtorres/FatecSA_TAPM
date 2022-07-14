/* 
 * File:  Config.h 
 * Author: Weslley Torres
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __CONFIG_H
#define	__CONFIG_H

#include <xc.h> // include processor files - each processor file is guarded.  

// #pragma config statements should precede project file includes.

//General defines
#define _XTAL_FREQ                 48000000      //clock frequency: 48MHz

#define MACHINE_CYCLE              (4/_XTAL_FREQ)   //In 8-bit PICs (at least, in 16F series ones), one instruction is executed only after 4 machine/clock cycles.
#define YES                        1
#define NO                         0
#define ENABLE                     YES
#define DISABLE                    NO 
#define TIMER0_INIT_VALUE          67
#define t_High                     0x01
#define t_Low                      0x00

//Tasks scheduling defines
#define INDEX_TAREFA_1                 0
#define INDEX_TAREFA_2                 1
#define INDEX_TAREFA_3                 2
#define INDEX_TAREFA_4                 3
#define NUMBER_OF_TASKS                4
#define TIME_TO_EXECUTE_TASK1          100   //time unit: ms
#define TIME_TO_EXECUTE_TASK2          300  //time unit: ms
#define TIME_TO_EXECUTE_TASK3          500  //time unit: ms
#define TIME_TO_EXECUTE_TASK4          800   //time unit: ms
#define TASK_TIMEOUT                   1000  //time unit: ms

typedef unsigned char uint8_t;
typedef signed char sint8_t;

#endif	/* Config_H */

