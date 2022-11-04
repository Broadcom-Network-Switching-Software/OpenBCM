/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:     sal.h
 * Purpose:     SAL Support definitions
 */

#ifndef _SAL_H
#define _SAL_H

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <sal/core/time.h>

/*
 * SAL Initialization
 */

int    sal_appl_init(void) SAL_ATTR_WEAK;

/*
 * Console Initialization
 */

void    sal_readline_init(void) SAL_ATTR_WEAK;
void    sal_readline_term(void) SAL_ATTR_WEAK;

/*
 * SAL Date Routines
 */

int    sal_date_set(sal_time_t *val) SAL_ATTR_WEAK;
int    sal_date_get(sal_time_t *val) SAL_ATTR_WEAK;

/*
 * SAL Booting/Rebooting routines.
 */

void    sal_reboot(void) SAL_ATTR_WEAK;

/*
 * SAL Shell routine - to escape to shell on whatever platform.
 */

void    sal_shell(void) SAL_ATTR_WEAK;

/*
 * SAL Shell Command routine - to execute shell command on whatever platform.
 */

void    sal_shell_cmd(const char *cmd) SAL_ATTR_WEAK;

/* 
 * LED Support, platform specific routine to set led value.
 *  sal_led: sets the state of one or more discrete LEDs; bit 0 is first LED
 *  sal_led_string: sets the state of an alphanumeric display, if any.
 */

extern uint32    sal_led(uint32 led) SAL_ATTR_WEAK;
extern void    sal_led_string(const char *s) SAL_ATTR_WEAK;

/*
 * Watchdog timer, platform specific
 */

extern int    sal_watchdog_arm(uint32 usec) SAL_ATTR_WEAK;

/* 
 * Memory Testing 
 */

extern int    sal_memory_check(uint32 addr) SAL_ATTR_WEAK;

#ifdef BROADCOM_DEBUG
#ifdef INCLUDE_BCM_SAL_PROFILE
extern void sal_dma_alloc_resource_usage_get(unsigned int *alloc_curr,
                                             unsigned int *alloc_max) SAL_ATTR_WEAK;
#endif
#endif

/*
 * Compatibility macros
 */

#ifdef __KERNEL__
#include <linux/kernel.h>
#define strtol simple_strtol 
#define strtoul simple_strtoul 
#endif

/*
 * Some time functions only available on UNIX
 * For now other systems just won't use these.
 */
#if defined(UNIX)
#define sal_mktime mktime
#define sal_localtime localtime
#define sal_strftime strftime
#endif

#endif    /* !_SAL_H */

