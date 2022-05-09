/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: 	dpc.h
 * Purpose: 	Deferred Procedure Call module
 */

#ifndef _SAL_DPC_H
#define	_SAL_DPC_H

#include <sal/core/time.h>
#include <sal/core/thread.h>

typedef void (*sal_dpc_fn_t)(void *owner, void *, void *, void *, void *);

extern int sal_dpc_init(void) SAL_ATTR_WEAK;
extern void sal_dpc_term(void) SAL_ATTR_WEAK;
extern int sal_dpc_config(int, int) SAL_ATTR_WEAK;

extern int sal_dpc(sal_dpc_fn_t, void *owner, void *, void *, void *, void *) SAL_ATTR_WEAK;
extern int sal_dpc_time(sal_usecs_t usec,
			sal_dpc_fn_t,
			void *owner, void *, void *, void *, void *) SAL_ATTR_WEAK;

extern void sal_dpc_cancel(void *owner) SAL_ATTR_WEAK;
extern int sal_dpc_enable(void *owner) SAL_ATTR_WEAK;
extern int sal_dpc_disable(void *owner) SAL_ATTR_WEAK;
extern int sal_dpc_disable_and_wait(void *owner) SAL_ATTR_WEAK;
extern void sal_dpc_cancel_and_disable(void *owner) SAL_ATTR_WEAK;

#endif	/* !_SAL_DPC_H */

