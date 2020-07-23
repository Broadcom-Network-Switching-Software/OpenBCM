/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_synce.c
 *
 *  Include:
 *      SyncE DB initialization and deinitialization.
 *      SyncE DB set and get functions.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SYNCEDNX

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_synce_access.h>

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
dnx_algo_synce_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create SW State instance
     */
    SHR_IF_ERR_EXIT(dnx_algo_synce_db.init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * SyncE Set/Get functions
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnx_algo_fabric_synce_enable_set(
    int unit,
    dbal_enum_value_field_synce_index_e synce_index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.master_synce_enabled.set(unit, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.slave_synce_enabled.set(unit, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_algo_fabric_synce_enable_get(
    int unit,
    dbal_enum_value_field_synce_index_e synce_index,
    int *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (synce_index == DBAL_ENUM_FVAL_SYNCE_INDEX_MASTER_SYNCE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.master_synce_enabled.get(unit, enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_synce_db.fabric_synce_status.slave_synce_enabled.get(unit, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

#undef _ERR_MSG_MODULE_NAME
