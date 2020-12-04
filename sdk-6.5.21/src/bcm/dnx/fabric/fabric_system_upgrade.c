/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/**
 * \file fabric_system_upgrade.c $Id$ Fabric System Upgrade Configuration for DNX.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/error.h>
#include <bcm/fabric.h>
#include <bcm/types.h>

#include <shared/utilex/utilex_u64.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/**
 * \brief
 *   Check if the system upgrade is in progress or not.
 * \param [in] unit         - The unit number.
 * \param [out] in_progress - The indication of in progress
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_in_progress_get(
    int unit,
    int *in_progress)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE_STATUS, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_5, INST_SINGLE, (uint32 *) in_progress);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get enabling status of system upgrade.
 * \param [in] unit         - The unit number.
 * \param [out] enable      - The enabling status
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_enable_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) enable);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify fabric system upgrade phase 1 input.
 * \param [in] unit         - The unit number.
 * \param [in] flags        - The flags.
 * \param [in] master_id    - The master id used for system upgrade.
 * \param [in] target       - The target for system upgrade.
 * \param [in] is_set       - Indicates set/get function.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_phase1_verify(
    int unit,
    int flags,
    int *master_id,
    bcm_fabric_system_upgrade_target_t * target,
    int is_set)
{
    uint32 target_value = 0;
    uint32 state_0, state_1, state_2;
    uint32 max_master_id;
    int in_progress;
    SHR_FUNC_INIT_VARS(unit);

    /** NULL checks */
    SHR_NULL_CHECK(master_id, _SHR_E_PARAM, "master_id");
    SHR_NULL_CHECK(target, _SHR_E_PARAM, "target");

    if (is_set)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_in_progress_get(unit, &in_progress));
        if (in_progress)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "The system upgrade is in progress, please call bcm_fabric_system_upgrade_phase3_get() to check the status.\n");
        }

        max_master_id = dnx_data_fabric.system_upgrade.max_master_id_get(unit);
        /** Check master_id */
        if (*master_id > max_master_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "master_id=%d should be less than %d.\n", *master_id, max_master_id + 1);
        }

        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_0.get(unit, &state_0));
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_1.get(unit, &state_1));
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_2.get(unit, &state_2));

        if (BCM_FABRIC_SYSTEM_UPGRADE_FIRST_ITERATION == flags)
        {
            /** Check target */
            SHR_MAX_VERIFY(*target, bcmFabricSystemUpgradeTargetCount - 1, _SHR_E_PARAM,
                           "target=%d should be less than %d.\n", *target, bcmFabricSystemUpgradeTargetCount);
            target_value = dnx_data_fabric.system_upgrade.target_get(unit, *target)->value;
            if (state_0 == target_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Target has been achieved, no need to upgrade.\n");
            }
        }
        else if (BCM_FABRIC_SYSTEM_UPGRADE_REPEAT_ITERATION == flags)
        {
            if (state_0 >= state_1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "No error happened during system upgrade, don't need to repeat the iter.\n");
            }
        }
        else if (0 == flags)
        {
            if (state_0 != state_1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Please make sure BCM_FABRIC_SYSTEM_UPGRADE_FIRST_ITERATION is called and each system upgrade iter has completed.\n");
            }
            if (state_0 == state_2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Target has been achieved, no need to upgrade.\n");
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Flags is invalid, should be 0 or BCM_FABRIC_SYSTEM_UPGRADE_xxx.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Translate target into integer and fraction.
 * \param [in] unit         - The unit number.
 * \param [in] target       - The system upgrade target
 * \param [out] intg        - The integer part of target
 * \param [out] frac        - The fraction part of target
 * \return
 *   See \ref shr_error_e
 */
static void
dnx_fabric_system_upgrade_target_translate(
    int unit,
    uint32 target,
    uint32 *intg,
    uint32 *frac)
{
    uint32 device_core_clock_khz;
    UTILEX_U64 frac_0, frac_x, frac_x2;

    device_core_clock_khz = DNXCMN_CORE_CLOCK_KHZ_GET(unit);
    *intg = target / device_core_clock_khz;
    utilex_u64_clear(&frac_0);
    utilex_u64_clear(&frac_x);
    utilex_u64_multiply_longs(target, (1 << 19), &frac_x);
    utilex_u64_devide_u64_long(&frac_x, device_core_clock_khz, &frac_0);
    if (device_core_clock_khz <= target)
    {
        utilex_u64_multiply_longs(*intg, (1 << 19), &frac_x2);
        utilex_u64_subtract_u64(&frac_0, &frac_x2);
    }
    *frac = frac_0.arr[0];
}
/**
 * \brief
 *   Enable or disable system upgrade.
 * \param [in] unit         - The unit number.
 * \param [in] enable       - The enabling status: 1 - enable, 0 - disable
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));
    /** Enable system upgrade */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (enable != 0));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure fabric system upgrade phase 1.
 * \param [in] unit         - The unit number.
 * \param [in] flags        - The flags.
 * \param [in] master_id    - The master id used for system upgrade.
 * \param [in] target       - The target for system upgrade.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_system_upgrade_phase1_set(
    int unit,
    int flags,
    int master_id,
    bcm_fabric_system_upgrade_target_t target)
{
    uint32 entry_handle_id;
    uint32 current_target;
    uint32 intg, frac;
    uint32 state_1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_system_upgrade_phase1_verify(unit, flags, &master_id, &target, 1));

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));

    if (BCM_FABRIC_SYSTEM_UPGRADE_FIRST_ITERATION == flags)
    {
        /** Configure parameters */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_0, INST_SINGLE,
                                     dnx_data_fabric.system_upgrade.param_0_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_1, INST_SINGLE,
                                     dnx_data_fabric.system_upgrade.param_1_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_2, INST_SINGLE,
                                     dnx_data_fabric.system_upgrade.param_2_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_3, INST_SINGLE,
                                     dnx_data_fabric.system_upgrade.param_3_get(unit));
        dnx_fabric_system_upgrade_target_translate(unit, dnx_data_fabric.system_upgrade.param_4_get(unit), &intg,
                                                   &frac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_4, INST_SINGLE, frac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_5, INST_SINGLE, intg);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_6, INST_SINGLE, master_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_7, INST_SINGLE,
                                     dnx_data_fabric.system_upgrade.param_7_get(unit));

        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_0.get(unit, &current_target));
        current_target += dnx_data_fabric.system_upgrade.param_5_get(unit);
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_1.set(unit, current_target));
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_2.set(unit,
                                                                 dnx_data_fabric.system_upgrade.target_get(unit,
                                                                                                           target)->value));

        dnx_fabric_system_upgrade_target_translate(unit, current_target, &intg, &frac);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_8, INST_SINGLE, frac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_9, INST_SINGLE, intg);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    else if (0 == flags)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_1.get(unit, &state_1));
        state_1 += dnx_data_fabric.system_upgrade.param_5_get(unit);
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_1.set(unit, state_1));
        dnx_fabric_system_upgrade_target_translate(unit, state_1, &intg, &frac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_8, INST_SINGLE, frac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PARAM_9, INST_SINGLE, intg);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_enable_set(unit, 1));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get configured master id for system upgrade.
 * \param [in] unit         - The unit number.
 * \param [out] master_id    - The master id used for system upgrade.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_master_id_get(
    int unit,
    int *master_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PARAM_6, INST_SINGLE, (uint32 *) master_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get phase1 configuration for system upgrade.
 * \param [in] unit         - The unit number.
 * \param [in] flags        - The flags.
 * \param [out] master_id    - The master id used for system upgrade.
 * \param [out] target       - The target for system upgrade.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_system_upgrade_phase1_get(
    int unit,
    int flags,
    int *master_id,
    bcm_fabric_system_upgrade_target_t * target)
{
    int i;
    uint32 state_2;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_system_upgrade_phase1_verify(unit, flags, master_id, target, 0));

    SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_master_id_get(unit, master_id));

    SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_2.get(unit, &state_2));
    if (0 == state_2)
    {
        *target = bcmFabricSystemUpgradeTargetInvalid;
    }
    else
    {
        for (i = 0; i < dnx_data_fabric.system_upgrade.target_info_get(unit)->key_size[0]; ++i)
        {
            if (dnx_data_fabric.system_upgrade.target_get(unit, i)->value == state_2)
            {
                *target = (bcm_fabric_system_upgrade_target_t) i;
                break;
            }
        }

        if (bcmFabricSystemUpgradeTargetCount == i)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Target values mismatch.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Helper function to trigger the system upgrade.
 * \param [in] unit         - The unit number.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_trig(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRIG, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRIG, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    sal_usleep(10000);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the trigger status.
 * \param [in] unit             - The unit number.
 * \param [out] is_triggered    - Is triggered status.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_trig_get(
    int unit,
    int *is_triggered)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRIG, INST_SINGLE, (uint32 *) is_triggered);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Pull the trigger and start the system upgrade.
 * \param [in] unit         - The unit number.
 * \param [in] flags        - The flags.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_system_upgrade_phase2_trig(
    int unit,
    int flags)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Check if system upgrade is enabled */
    SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_enable_get(unit, &enable));

    if (!enable)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "System upgrade is not enabled. Please call bcm_fabric_system_upgrade_phase1_set() first.\n");
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_trig(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check if the system upgrade is done or not.
 * \param [in] unit         - The unit number.
 * \param [out] done        - The done status: 1 - done, 0 - not done
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_done_get(
    int unit,
    int *done)
{
    uint32 entry_handle_id;
    uint32 state_5, state_6;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE_STATUS, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_5, INST_SINGLE, &state_5);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_6, INST_SINGLE, &state_6);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *done = !state_5 && state_6;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Check if the system upgrade is OK.
 * \param [in] unit         - The unit number.
 * \param [out] ok          - The OK status.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_ok_get(
    int unit,
    int *ok)
{
    uint32 entry_handle_id;
    uint32 state_7, state_8;
    uint32 param_8, param_9;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE_STATUS, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_7, INST_SINGLE, &state_7);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_8, INST_SINGLE, &state_8);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PARAM_8, INST_SINGLE, &param_8);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PARAM_9, INST_SINGLE, &param_9);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *ok = (state_7 == param_8) && (state_8 == param_9);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Apply the system upgrade target.
 * \param [in] unit         - The unit number.
 * \return
 *   See \ref shr_error_e
 */
static shr_error_e
dnx_fabric_system_upgrade_apply(
    int unit)
{
    uint32 entry_handle_id;
    uint32 state_7, state_8;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_UPGRADE_STATUS, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_7, INST_SINGLE, &state_7);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_STATE_8, INST_SINGLE, &state_8);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MESH_TOPOLOGY_INIT, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_CONFIG_1, INST_SINGLE, state_7);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_CONFIG_2, INST_SINGLE, state_8);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the system upgrade phase3 status.
 * \param [in] unit         - The unit number.
 * \param [in] flags        - The flags.
 * \param [out] status      - The phase3 status.
 * \return
 *   See \ref shr_error_e
 */
int
bcm_dnx_fabric_system_upgrade_phase3_get(
    int unit,
    int flags,
    bcm_fabric_system_upgrade_status_t * status)
{
    int in_progress = 0;
    int done = 0;
    int ok = 0;
    int is_triggered;
    uint32 state_0, state_1, state_2;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** NULL checks */
    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

    SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_in_progress_get(unit, &in_progress));
    SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_done_get(unit, &done));

    if (in_progress)
    {
        *status = bcmFabricSystemUpgradeStatusInProgress;
    }
    else if (done)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_ok_get(unit, &ok));
        if (ok)
        {
            SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_1.get(unit, &state_1));
            SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_2.get(unit, &state_2));
            if (state_1 == state_2)
            {
                *status = bcmFabricSystemUpgradeStatusComplete;
            }
            else
            {
                *status = bcmFabricSystemUpgradeStatusOk;
            }
            SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_0.set(unit, state_1));
            SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_apply(unit));

            SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_enable_set(unit, 0));
        }
        else
        {
            *status = bcmFabricSystemUpgradeStatusError;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_0.get(unit, &state_0));
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_1.get(unit, &state_1));
        SHR_IF_ERR_EXIT(dnx_fabric_db.system_upgrade.state_2.get(unit, &state_2));
        if (state_0 == state_1)
        {
            if (state_0 == state_2)
            {
                *status = bcmFabricSystemUpgradeStatusComplete;
            }
            else
            {
                *status = bcmFabricSystemUpgradeStatusOk;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_fabric_system_upgrade_trig_get(unit, &is_triggered));
            if (!is_triggered)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "System upgrade is not triggered. Please call bcm_fabric_system_upgrade_phase2_trig() first.\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
