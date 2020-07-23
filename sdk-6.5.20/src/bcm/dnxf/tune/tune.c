 /** \file tune.c
 * main Tune module file
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
#include "cgm_tune.h"
#include "port_tune.h"

/*
 * }
 * Include files.
 */

/*
 * See .h file
 */
shr_error_e
dnxf_tune_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /** Place your tune-module-code here*/
    SHR_IF_ERR_EXIT(dnxf_cgm_tune_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e dnxf_tune_port_add(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Place your tune-code here*/

    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e dnxf_tune_port_remove(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** nothing to be done */

    SHR_FUNC_EXIT;
}
