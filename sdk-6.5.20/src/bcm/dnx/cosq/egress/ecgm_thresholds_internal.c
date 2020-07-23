/** \file ecgm_thresholds_internal.c
* 
* internal ecgm functionality.
*/

/*
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/**
* Included files.
* {
*/

#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>      /* ecgm dnx data */
#include <bcm_int/dnx/stk/stk_sys.h>    /* for dnx_stk_sys_traffic_enable_get */
#include "ecgm_dbal.h"  /* internal module dbal functions */
#include "ecgm_thresholds_internal.h"

/**
* }
*/

/**
 *  \brief - verify modifications to reserved data buffer threshold for SP.
 */
static shr_error_e dnx_ecgm_sp_reserved_db_threshold_modify_verify(
    int unit,
    int core,
    int sp,
    uint32 sp_reserved);

shr_error_e
dnx_ecgm_min_threshold_verify(
    int unit,
    int max,
    int min)
{
    SHR_FUNC_INIT_VARS(unit);
    if (min > max)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "min value (%d) is higher than max value (%d)\n."), min, max));
    }
    SHR_FUNC_EXIT;
}

/** \see .h file */
shr_error_e
dnx_ecgm_pd_threshold_verify(
    int unit,
    int value)
{
    int total_pds = dnx_data_ecgm.core_resources.total_pds_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (value < 0 || value > total_pds)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid PD value %d: must be 0 <= value < %d.", value, total_pds);
    }

exit:
    SHR_FUNC_EXIT;
}

/** \see .h file */
shr_error_e
dnx_ecgm_db_threshold_verify(
    int unit,
    int value)
{
    int total_dbs = dnx_data_ecgm.core_resources.total_dbs_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (value < 0 || value > total_dbs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid DB value %d: must be 0 <= value < %d.", value, total_dbs);
    }

exit:
    SHR_FUNC_EXIT;
}

/** \see .h file */
shr_error_e
dnx_ecgm_alpha_verify(
    int unit,
    int alpha)
{
    uint32 max_alpha[1];
    int min_alpha;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_alpha_max_val_get(unit, max_alpha));
    min_alpha = ((int) max_alpha[0]) * (-1);

    if ((alpha < min_alpha) || (alpha > (int) max_alpha[0]))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid alpha value %d.", alpha);
    }

exit:
    SHR_FUNC_EXIT;
}

/** \see .h file */
shr_error_e
dnx_ecgm_sp_reserved_db_threshold_modify(
    int unit,
    int core_id,
    int sp,
    int reserved_db,
    dnx_ecgm_threshold_sp_action_e action)
{
    uint32 reserved;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                    (unit, core_id, sp, DBAL_FIELD_RESERVED_DB, &reserved));

    switch (action)
    {
        case (DNX_ECGM_THRESHOLD_SP_SET):
            reserved = reserved_db;
            break;
        case (DNX_ECGM_THRESHOLD_SP_DECREASE):
            reserved -= reserved_db;
            break;
        case (DNX_ECGM_THRESHOLD_SP_INCREASE):
            reserved += reserved_db;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid action %d", action);
            break;
    }

    SHR_INVOKE_VERIFY_DNX(dnx_ecgm_sp_reserved_db_threshold_modify_verify(unit, core_id, sp, reserved));

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_set(unit, core_id, sp, DBAL_FIELD_RESERVED_DB, reserved));

exit:
    SHR_FUNC_EXIT;
}

/** \see declaration */
static shr_error_e
dnx_ecgm_sp_reserved_db_threshold_modify_verify(
    int unit,
    int core,
    int sp,
    uint32 sp_reserved)
{
    int total_reserved, max_dbs = dnx_data_ecgm.core_resources.total_dbs_get(unit);
    uint32 other_sp_reserved, mc_db_drop;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_sp_threshold_get
                    (unit, core, ((sp == 0) ? 1 : 0), DBAL_FIELD_RESERVED_DB, &other_sp_reserved));

    total_reserved = (other_sp_reserved + sp_reserved);
    if (total_reserved > max_dbs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid new reserved value - would make the sum of reserved DBs for both SPs = %d. must be: 0 <= sum < %d.",
                     total_reserved, max_dbs);
    }

    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_core_multicast_threshold_get(unit, core, DBAL_FIELD_DB_DROP, &mc_db_drop));
    if (total_reserved > mc_db_drop)
    {
        LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit,
                                             "new reserved value would make the sum of reserved DBs for both SPs = %d, higher than the multicast DB drop value (%d)\n."),
                                  total_reserved, mc_db_drop));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_ecgm_dp_verify(
    int unit,
    int dp)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dp < 0 || dp > DNX_COSQ_NOF_DP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid dp %d. must be between 0-%d.", dp, DNX_COSQ_NOF_DP);
    }
exit:
    SHR_FUNC_EXIT;
}
