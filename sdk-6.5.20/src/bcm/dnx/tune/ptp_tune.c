/** \file ptp_tune.c
 * Tuning of ptp (1588 protocol)
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

/*
 * }
 * Include files.
 */

/** see .h file */
shr_error_e
dnx_ptp_tune_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
/**exit: */
    SHR_FUNC_EXIT;
}
