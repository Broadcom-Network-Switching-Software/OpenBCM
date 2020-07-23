/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ukernel_debug.h
 * Purpose:  
 */

#ifndef _UKERNEL_DEBUG_H
#define _UKERNEL_DEBUG_H

#if defined(SOC_UKERNEL_DEBUG)

#include <soc/defs.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#include <appl/diag/shell.h>

extern cmd_result_t cmd_cmic_ucdebug(int unit, args_t *a);
#else
#define cmd_cmic_ucdebug(unit, a)
#endif
#else
#define cmd_cmic_ucdebug(unit, a)
#endif
#endif /*_UKERNEL_DEBUG_H*/
