/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __BCM_DEBUG_H__
#define __BCM_DEBUG_H__

#include <shared/bsl.h>

#include <bcm/types.h>

#ifndef BCM_HIDE_DISPATCHABLE

/* _bcm_debug_api */
extern void _bcm_debug_api(
    int log_src,
    char *api, 
    int nargs, 
    int ninargs, 
    int arg1, 
    int arg2, 
    int arg3, 
    int rv);

#endif /* BCM_HIDE_DISP  ATCHABLE */

#define BCM_CALL_DISPATCH(ret, f_api, args, n_args, args_by_ref) ret = f_api args

#define BCM_API_SET_CONTEXT(file, line, Func)

#if defined(BROADCOM_DEBUG)
#define BCM_API(_1, _2, _3, _4, _5, _6, _7, _8)  \
    do { \
        if (LOG_CHECK(_1 | BSL_VERBOSE)) \
           _bcm_debug_api(_1,_2,_3,_4,_5,_6,_7,_8); \
    } while (0) 

/*
 * BCM_API_VERBOSE_LOGGING enables capability to log API calls along with their
 * arguments using CINT infrastructure.
 */
#ifdef BCM_API_VERBOSE_LOGGING

#include "src/appl/cint/cint_logger.h"

#undef BCM_CALL_DISPATCH
#define BCM_CALL_DISPATCH(ret, f_api, args, n_args, args_by_ref) \
        CINT_LOGGER_CALL_DISPATCH(ret, f_api, args, n_args, args_by_ref)

#undef BCM_API_SET_CONTEXT
#define BCM_API_SET_CONTEXT(file, line, Func) \
        CINT_LOGGER_SET_CALLER_CONTEXT(file, line, Func)

#endif

#else
#define BCM_API(_1, _2, _3, _4, _5, _6, _7, _8)
#endif

#endif /* __BCM_DEBUG_H__ */
