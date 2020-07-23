/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * cmicx_sim_util.h
 *
 * utilities header file. does definitions for things like logging, error logging, etc
 */

#ifndef _CMICX_SIM_UTIL_H_
#define _CMICX_SIM_UTIL_H_

#ifndef __KERNEL__
#ifdef __linux__
#include <stdint.h>
#endif
#endif

#ifndef __KERNEL__
#include <time.h>
#else
#include <linux/time.h>
#endif


/* for bool stuff */
#ifndef __KERNEL__
typedef unsigned char bool;
#endif
#define true  1
#define false 0

#ifdef __linux__
typedef unsigned int addr_t;
#endif

/* typedefs for certain primitive types used everywhere */
typedef unsigned int reg_t;
#ifdef CMICX_STANDALONE_SIM
typedef unsigned char uint8;
typedef unsigned char uint8_t;
typedef unsigned int uint32;
typedef unsigned int uint32_t;
typedef long unsigned int uint64;
#endif

/* pcid function types */
typedef bool pcid_read_func_t(reg_t*);
typedef bool pcid_write_func_t(reg_t);

/* initialize cmicx logging to a certain file */
bool cmicx_logging_init(char *);

/* logging and error functions */
void cmicx_log_level_set(int);
void cmicx_log_level_cutoff_set(int);
void cmicx_log(char *);
void cmicx_error(char *);
void cmicx_log_str(char *);
void cmicx_log_addr(addr_t);
void cmicx_log_reg(reg_t);
void cmicx_log_int(int);
void cmicx_log_unsigned(unsigned int);
void cmicx_log_hex(unsigned int);
void cmicx_log_uint64(uint64_t);
void cmicx_log_byte(unsigned char);
void cmicx_log_long(unsigned long);
void cmicx_log_newline(void);

/* function to log the elapsed time since the last time this function was called. */
void cmicx_log_elapsed_time(void);

/* returns the timespec formatted difference in time between two timespecs */
void cmicx_timespec_diff(const struct timespec * const start_time,
                         const struct timespec * const end_time,
                         struct timespec * const diff_time);

/* random functions (endianness, etc) */
reg_t cmicx_reverse_word_endianness(reg_t val);


#endif   /* _CMICX_SIM_UTIL_H_ */

