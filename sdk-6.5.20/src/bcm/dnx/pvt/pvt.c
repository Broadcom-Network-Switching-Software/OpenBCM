/** \file pvt.c
 *
 * Handles the PVT monitors functionalities for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PVT

/*
 * Include files currently used for DNX.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm/error.h>
#include <bcm/debug.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>

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
 * \brief
 *   Initialize PVT monitor module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_pvt_mon_init(
    int unit)
{
    int nof_pvt_mon;
    uint32 entry_handle_id;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_pvt_mon = dnx_data_pvt.general.nof_pvt_monitors_get(unit);

    /*
     * Init
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL, i, 0);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 0);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_VAVSMON_PWRDN, i, 0);
        dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_SEL, i, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Powerdown
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Powerup
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_POWERDOWN, i, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /*
     * Reset
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_PVT_MON_ENABLE, &entry_handle_id));
    for (i = 0; i < nof_pvt_mon; i++)
    {
        dbal_entry_value_field16_set(unit, entry_handle_id, DBAL_FIELD_PVT_MON_CONTROL_RESET, i, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
