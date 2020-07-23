/** \file src/bcm/dnx/sec/sec_init.c
 *
 *
 *  This file contains the implementation on init functions of
 *  the MACSec/IPSec module.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MACSEC

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/sec/sec_init.h>
#include <bcm/types.h>

#ifdef INCLUDE_XFLOW_MACSEC
#include <dnx/dnx_sec.h>
#endif
/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/**
 * see header file
 */
shr_error_e
dnx_sec_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

#ifdef INCLUDE_XFLOW_MACSEC
    SHR_IF_ERR_EXIT(dnx_xflow_macsec_init(unit));
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * see header file
 */
shr_error_e
dnx_sec_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
