/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __TASKS_INFO_H_INCLUDED__
/* { */
#define __TASKS_INFO_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
    #error  "Add your system support for packed attribute."
#endif

/*************
 * INCLUDES  *
 */
/* { */
/*
 * Default size (in bytes) of stack per task.
 */
#define STACK_SIZE           20000
/*
 * stack size for the task running starter Proc
 */
#define STARTER_TASK_STACK_SIZE 200000
/*
 * Number of tasks assigned for this system.
 * May be larger than actually used.
 * See 'List of internal task IDs' below.
 */
#define NUM_TASKS            13
/*
 * Maximal number of letters in a task name.
 */
/*
 * List of internal task IDs.
 * Starting at 0.
 * Task ID must not be larger than NUM_TASKS-1
 */
/*
 * Definitions related to task which collects user input
 * characters for user interface task.
 */
#define USER_PREP_TASK_ID             1
/*
 * Size (in bytes) of stack for ui_prep task.
 */
/*
 * Definitions related to task which collects user input
 * characters from Telnet over port 26 when standard
 * Telnet is active (on port 23).
 */
#define DUMMY_TELNET_TCP_PORT         26
#define DUMMY_TELNET_TASK_ID          2
#define DUMMY_TELNET_TASK_PRIORITY    36
#define DUMMY_TELNET_TASK_NAME        "dDumTel"
/*
 * Definitions related to task which collects user input
 * characters from local console when Telnet is active.
 */
#define DUMMY_CONSOLE_TASK_ID         3
#define DUMMY_CONSOLE_TASK_PRIORITY   38
#define DUMMY_CONSOLE_TASK_NAME       "dDumCon"
/*
 * Definitions related to user interface task.
 */
#define USER_INTERFACE_TASK_ID        4
#define USER_INTERFACE_TASK_PRIORITY  40
#define USER_INTERFACE_TASK_NAME      "dUiTask"
#define USER_INTERFACE_TASK_STACK_SIZE (40*STACK_SIZE)
#define IMPOSED_SHELL_PRIORITY        42
/*
 * Definitions related to background task.
 */
/*
 * Size (in bytes) of stack for background task.
 */
/*
 * Definitions related to snmp task.
 */
#define SNMP_TASK_ID                  6
#define INCLUDE_NET_SNMP              0
/*
 * Definitions related to idle task.
 */
/*
 * Size (in bytes) of stack for idle task.
 */
/*
 * Definition related to root task (initializer which ends up in progStart)
 */
/*
 * This should be a low priority, below 'background' and above 'idle'.
 */
#define ROOT_TASK_PRIORITY            198
/*
 * Definitions related to periodic suspend task.
 */
/*
 * Definitions related to Chip simulator task.
 */
#define CHIP_SIMULATOR_TASK_ID        9
#define CHIP_SIMULATOR_TASK_PRIO      30
#define CHIP_SIMULATOR_TASK_NAME      "dChipSim"
/*
 * Definitions related to DCL RX UNEXPECTED task.
 * (Unsolicited messages to DCL)
 */
/*
 * Size (in bytes) of stack for DCL RX UNEXPECTED task.
 */
/*
 * Set true for DCL transport debug stage.
 * Produces debug printing.
 */


/* Definitions for the syslog task (reads OS originated messages (logMsg)
   from a pipe, and sends them to a syslog server */
/* Same priority as the the OS logTask */
/* change INCLUDE_SYSLOG to 0 to disable syslog task
   This will only disable sending of logMsg() messages to the rsyslog */


/* } */

/*************
 * DEFINES   *
 */
/* { */

/* } */

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

/* } __TASKS_INFO_H_INCLUDED__*/
#endif
