/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: 	spl.h
 * Purpose: 	Interrupt Blocking
 */

#ifndef _SAL_SPL_H
#define _SAL_SPL_H

#include <sal/compiler.h>

extern int	sal_spl_init(void) SAL_ATTR_WEAK;
extern int	sal_spl(int level) SAL_ATTR_WEAK;
extern int	sal_splhi(void) SAL_ATTR_WEAK;

extern int      sal_int_locked(void) SAL_ATTR_WEAK;
extern int      sal_int_context(void) SAL_ATTR_WEAK;
extern int      sal_no_sleep(void) SAL_ATTR_WEAK;

#endif	/* !_SAL_SPL_H */
