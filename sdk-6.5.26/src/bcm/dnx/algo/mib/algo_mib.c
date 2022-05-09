/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/** \file algo_mib.c
 *
 *  Include:
 *      MIB DB initialization and deinitialization.
 *      MIB DB set and get functions.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_mib_access.h>
#include <bcm_int/dnx/algo/mib/algo_mib.h>

/*
 * }
 */
/*
 * Macros
 * {
 */
/*
 * }
 */
/*
 * Module Init / Deinit  
 * { 
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_mib_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * MIB Set/Get functions
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_mib_counter_pbmp_set(
    int unit,
    bcm_pbmp_t counter_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);

    /** set data */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.mib.counter_pbmp.set(unit, counter_pbmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_mib_counter_pbmp_get(
    int unit,
    bcm_pbmp_t * counter_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.mib.counter_pbmp.get(unit, counter_pbmp));
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_mib_counter_pbmp_port_enable(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    bcm_pbmp_t counter_pbmp;
    SHR_FUNC_INIT_VARS(unit);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.mib.counter_pbmp.get(unit, &counter_pbmp));

    if (enable)
    {
        BCM_PBMP_PORT_ADD(counter_pbmp, logical_port);
    }
    else
    {
        BCM_PBMP_PORT_REMOVE(counter_pbmp, logical_port);
    }
    /** set data */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.mib.counter_pbmp.set(unit, counter_pbmp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_mib_interval_set(
    int unit,
    int interval)
{
    SHR_FUNC_INIT_VARS(unit);

    /** set data */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.mib.interval.set(unit, interval));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_mib_interval_get(
    int unit,
    int *interval)
{
    SHR_FUNC_INIT_VARS(unit);

    /** get data */
    SHR_IF_ERR_EXIT(dnx_algo_mib_db.mib.interval.get(unit, interval));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
