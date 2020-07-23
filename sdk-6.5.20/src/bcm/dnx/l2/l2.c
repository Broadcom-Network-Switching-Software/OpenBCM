/** \file l2.c
 * 
 *
 * L2 procedures for DNX.
 *
 * This file contains functions for managing l2 protocols.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/mdb.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l2/algo_l2.h>
#include <bcm_int/dnx/l2/l2.h>
#include <bcm_int/dnx/l2/l2_learn.h>
#include <bcm_int/dnx/l2/l2_traverse.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/l2_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l2_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
/*
 * }
 */

/*
 * }
 */
/*
 * Function Declaration.
 * {
 */

/*
 * }
 */

/*
 * Defines.
 * {
 */

/** Default MACT dynamic threshold. A value higher than the threshold is handled as static entry. */
#define MACT_DYNAMIC_THRESHOLD 2
/** Age where the refresh event is raised */
#define AGE_FOR_REFRESH 7

/** Stamp used in learn events that exceed the limit */
#define LEARN_EVENT_EXCEED_LIMIT_STAMP_DEFAULT 0xff

/** Empty age mask. Using this mask means updating the age every meta cycle */
#define AGE_CYCLE_NO_MASK 0x0
/*
 * Maximum number of seconds between meta cycles that can be set.
 * Minimum of 300 seconds is assuming that the clock rate is at least 750 Mhz.
 */
#define L2_MAX_AGE_TIME 300

#define L2_LEARN_FORMAT_INDEX_FOR_COUNTING 0
#define L2_LEARN_FORMAT_INDEX_FOR_SELECT_FID_PROFILE 2

 /*
  * }
  */

 /*
  * Internal functions.
  * {
  */
/** Refer to the .h file for more information */
shr_error_e
dnx_vsi_count_get(
    int unit,
    int *nof_vsis)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, DBAL_TABLE_ING_VSI_INFO_DB, nof_vsis));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verifies validity of flags by themselves and vs global mode
 */
static shr_error_e
dnx_l2_learn_flag_verify(
    int unit,
    uint32 limit_flags)
{
    dnx_learn_limit_mode_t learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    /** Check if the set flags are supported */
    if (limit_flags & ~BCM_DNX_L2_LEARN_SUPPORTED_FLAGS)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Flag = 0x%08X is not in the range of supported ones", limit_flags);
    }
    /** Count the flags and check that only one is set */
    if (utilex_bitstream_get_nof_on_bits(&limit_flags, 1) != 1)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "One and only one flag can be used. flags=0x%08X", limit_flags);
    }
    /** Verify that learn mode is suited to the command */
    if ((limit_flags == BCM_L2_LEARN_LIMIT_VLAN) && (learn_limit_mode != LEARN_LIMIT_MODE_VLAN))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Limit per VSI cannot be managed in %s mode", LEARN_LIMIT_MODE_NAME(unit));
    }
    if ((limit_flags == BCM_L2_LEARN_LIMIT_PORT) && (learn_limit_mode != LEARN_LIMIT_MODE_VLAN_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Limit per OUTLIF cannot be managed in %s mode", LEARN_LIMIT_MODE_NAME(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify learn limit parameters for BCM-API: bcm_dnx_l2_learn_limit_set().
 * check the flags support and learn limit number
 * Trying to update the limit number, without setting corresponding flag resides in an error
 */
static shr_error_e
dnx_l2_learn_limit_set_verify(
    int unit,
    bcm_l2_learn_limit_t * limit)
{
    uint32 predef_max_limit;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(limit, _SHR_E_PARAM, "limit");

    SHR_IF_ERR_EXIT(dnx_l2_learn_flag_verify(unit, limit->flags));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_LEARN_LIMIT,
                                                    0, DBAL_PREDEF_VAL_MAX_VALUE, &predef_max_limit));

    /*
     * Check if the set limit value doesn't exceed the maximum number of mact entries
     */
    if (limit->limit < -1 || limit->limit > (int) predef_max_limit)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Limit is out of range");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify learn limit parameters for BCM-API: bcm_dnx_l2_learn_limit_get().
 * check the supported flags
 */
static shr_error_e
dnx_l2_learn_limit_get_verify(
    int unit,
    bcm_l2_learn_limit_t * limit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(limit, _SHR_E_PARAM, "limit");

    /** Only thing to be checked on get is whether flags are legitimate */
    SHR_IF_ERR_EXIT(dnx_l2_learn_flag_verify(unit, limit->flags));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the unknown DA profiles to use the vsi property as the MC unknown DA destination. Setting the vsi value in this
 * property will be done when the vlan is created.
 */
static shr_error_e
dnx_l2_init_unknown_da_profiles(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write to unknown DA table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_FORWARD_UNKNOWN_DA_PROFILE, &entry_handle_id));

    /** Key fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_ACTION_PROFILE,
                                     DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_DA_NOT_FOUND_DESTINATION_PROFILE,
                                     DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_DA_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Result values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ADD_VSI_DEST, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Configure the VMV <-> learn format index translation.
 */
static shr_error_e
dnx_l2_init_vmv_entry_format_index_configs(
    int unit)
{
    uint32 entry_handle_id;
    uint8 vmv_size;
    uint8 vmv_value;
    uint32 vmv_from_wrong_location;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vmv_from_wrong_location =
        dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_vmv_for_limit_in_wrong_location);

    /** MDB_EM_ENTRY_ENCODING_HALF is a good format for all the MACT payloads. */
    SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value
                    (unit, DBAL_PHYSICAL_TABLE_LEM, MDB_EM_ENTRY_ENCODING_HALF, &vmv_size, &vmv_value));

    /** Entry format index to VMV */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARN_FORMAT_INDEX_TO_VMV, &entry_handle_id));
    /** SVL */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, DBAL_DEFINE_MACT_APP_DB_IDX_SVL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_VMV, INST_SINGLE, vmv_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /** PCC */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, DBAL_DEFINE_MACT_APP_DB_IDX_PCC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_VMV, INST_SINGLE, vmv_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** VMV to entry format index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARN_FORMAT_INDEX_FROM_VMV, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_VMV, vmv_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                 DBAL_DEFINE_MACT_APP_DB_IDX_SVL);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    if (vmv_from_wrong_location)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_LEARN_FORMAT_INDEX_FROM_VMV, &entry_handle_id));

        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MDB_VMV, 0, 15);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX, INST_SINGLE,
                                     L2_LEARN_FORMAT_INDEX_FOR_SELECT_FID_PROFILE);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Configure the AppDB ID <-> AppDB index translation. Configure which AppDBs support learn.
 */
static shr_error_e
dnx_l2_init_appdb_index_configs(
    int unit)
{
    uint32 entry_handle_id;
    uint32 app_db_id_svl;
    uint32 app_db_id_pcc;
    int app_db_size_svl;
    int app_db_size_pcc;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_FWD_MACT, &app_db_id_svl, &app_db_size_svl));
    SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, DBAL_TABLE_PCC_STATE, &app_db_id_pcc, &app_db_size_pcc));

    /** set that learning enabled for logical tables*/
    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, DBAL_TABLE_FWD_MACT, DBAL_TABLE_IND_IS_LEARNING_EN, TRUE));
    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, DBAL_TABLE_PCC_STATE, DBAL_TABLE_IND_IS_LEARNING_EN, TRUE));

    /** Entry AppDB ID to AppDB index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_APP_DB_CONFIGURATION, &entry_handle_id));

    /** SVL  */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEM_APP_DB, app_db_id_svl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MACT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LARGE_EM, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_MACT_APP_DB_INDEX_SVL);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** PCC  */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEM_APP_DB, app_db_id_pcc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MACT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_LARGE_EM, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX, INST_SINGLE,
                                 DBAL_ENUM_FVAL_MACT_APP_DB_INDEX_PCC);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** AppDB Index to AppDB ID */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_APP_DB_INDEX_TO_APP_DB, &entry_handle_id));

    /** SVL*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX,
                               DBAL_ENUM_FVAL_MACT_APP_DB_INDEX_SVL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEM_APP_DB, INST_SINGLE, app_db_id_svl);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** PCC  */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX,
                               DBAL_ENUM_FVAL_MACT_APP_DB_INDEX_PCC);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEM_APP_DB, INST_SINGLE, app_db_id_pcc);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Configure MACT Profile Pointer Location
 */
static shr_error_e
dnx_l2_init_profile_pointer_configs(
    int unit)
{
    uint32 entry_handle_id;
    uint32 size, svl_offset, wa_offset, pcc_offset;
    dnx_learn_limit_mode_t learn_limit_mode;
    uint32 vmv_from_wrong_location;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    vmv_from_wrong_location =
        dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_vmv_for_limit_in_wrong_location);

    /** Entry AppDB ID to AppDB index */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_PROFILE_POINTER_CONFIG, &entry_handle_id));

    learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit);
    /*
     * Size will be always maximum - 17
     * Offset will be figured out from mode and may be different for IVL and SVL
     */
    size = MACT_VSI_TO_PROFILE_IND_SIZE;
    switch (learn_limit_mode)
    {
        case LEARN_LIMIT_MODE_VLAN:
            /*
             * Both point to VSI inside MACT Key which is right after MAC address from LSB
             */
            pcc_offset = svl_offset = MACT_ENTRY_VSI_OFFSET;
            wa_offset = MACT_ENTRY_VSI_OFFSET + dnx_data_l2.general.vsi_offset_shift_get(unit);
            break;
        case LEARN_LIMIT_MODE_VLAN_PORT:
            /*
             * See l2_learn.h for calculation details of MACT_ENTRY_OUTLIF_OFFSET
             */
            pcc_offset = svl_offset = MACT_ENTRY_OUTLIF_OFFSET;
            wa_offset = MACT_ENTRY_OUTLIF_OFFSET + dnx_data_l2.general.lif_offset_shift_get(unit);
            break;
        case LEARN_LIMIT_MODE_TUNNEL:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Tunnel Mode is not supported(yet) for Learn Limit Mode\n");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Learn Limit Mode:%d is not supported\n", learn_limit_mode);
            break;
    }

    /** SVL */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX,
                               DBAL_ENUM_FVAL_MACT_APP_DB_INDEX_SVL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX,
                               L2_LEARN_FORMAT_INDEX_FOR_COUNTING);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POINTER_OFFSET, INST_SINGLE, svl_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POINTER_SIZE, INST_SINGLE, size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** PCC  */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX,
                               DBAL_ENUM_FVAL_MACT_APP_DB_INDEX_PCC);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX,
                               L2_LEARN_FORMAT_INDEX_FOR_COUNTING);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POINTER_OFFSET, INST_SINGLE, pcc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POINTER_SIZE, INST_SINGLE, size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    if (vmv_from_wrong_location)
    {
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_MACT_APP_DB_INDEX, 0, 1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_FORMAT_INDEX,
                                   L2_LEARN_FORMAT_INDEX_FOR_SELECT_FID_PROFILE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POINTER_OFFSET, INST_SINGLE, wa_offset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POINTER_SIZE, INST_SINGLE, size);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize general mact configurations like MACT nof entries limit, threshold for static/dynamic, stamp for exceed limit DSP,
 * etc.
 */
static shr_error_e
dnx_l2_init_general_mact_configs(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));

    /** Result values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DYNAMIC_STRENGTH_THRESHOLD, INST_SINGLE,
                                 MACT_DYNAMIC_THRESHOLD);

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_fid_mgmt_ecc_error))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_ECC, INST_SINGLE, 1);
    }

    /** Enabling the OLP allows the LEM to signal the OLP on valid events */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EVENT_FIFO_ENABLE_OLP, INST_SINGLE, 1);
    /** Always learn all copies */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ALL_COPIES, INST_SINGLE, 1);
    /** Configure the Jericho 1 prefix for Jericho mode */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_PORT_PREFIX, INST_SINGLE, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_VSI_LIMIT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_MACT_DB_LIMIT, INST_SINGLE, 1);

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_appdb_id_for_olp))
    {
        /** Mask all the MACT DB ID bits. Always use 0 as the AppDB ID */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ID_MACT_MASK, INST_SINGLE, 0xf);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ID_BMACT_MASK, INST_SINGLE, 0xf);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ID_PROG_MASK, INST_SINGLE, 0xf);
    }

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Configure the VMV <-> learn format index translation */
    SHR_IF_ERR_EXIT(dnx_l2_init_vmv_entry_format_index_configs(unit));

    /** Configure the APPDB ID <->APPDB index translation */
    SHR_IF_ERR_EXIT(dnx_l2_init_appdb_index_configs(unit));

    SHR_IF_ERR_EXIT(dnx_l2_init_profile_pointer_configs(unit));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Update the DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION with the global limit information
 */
static shr_error_e
dnx_global_learn_limit_hw_set(
    int unit,
    int limit)
{
    uint32 entry_handle_id;
    int check_mact_db_limit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    check_mact_db_limit = (limit == -1) ? 0 : 1;
    /*
     * Write to DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));

    /*
     * Set the MACT_DB_LIMIT check only if the limit value is not the maximum value of entries.
     * Unset, in case the we don't want limit
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHECK_MACT_DB_LIMIT, INST_SINGLE,
                                 check_mact_db_limit);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EVENT_CHECK_MACT_DB_LIMIT, INST_SINGLE,
                                 check_mact_db_limit);

    if (limit == -1)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ENTRIES_LIMIT, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ENTRIES_LIMIT, INST_SINGLE, limit);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get from DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION the global limit information
 */
static shr_error_e
dnx_global_learn_limit_hw_get(
    int unit,
    int *limit)
{
    uint32 entry_handle_id;
    uint32 check_mact_db_limit;
    int max_limit;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get information from DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION Table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));

    /*
     * Get the MACT_DB_LIMIT check and the limit value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CHECK_MACT_DB_LIMIT, INST_SINGLE,
                               &check_mact_db_limit);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MACT_DB_ENTRIES_LIMIT, INST_SINGLE, (uint32 *) limit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION,
                                                    DBAL_FIELD_MACT_DB_ENTRIES_LIMIT, FALSE, 0, INST_SINGLE,
                                                    &max_limit));

    /*
     * If the field CHECK_MACT_DB_LIMIT is unset or the limit is the maximum value, then set the limit to the max value of entries.
     */

    if (check_mact_db_limit == 0 || *limit == max_limit)
    {
        *limit = -1;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get from VSI_LEARNING_PROFILE_INFO the learn limit per VSI
 *
 * \param [in] unit - Relevant unit
 * \param [in] vsi - VSI
 * \param [out] learn_limit_per_vsi - Learn limit per VSI
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_learn_limit_per_vsi_get(
    int unit,
    bcm_vlan_t vsi,
    int *learn_limit_per_vsi)
{
    int vsi_learning_profile;
    dnx_vsi_learning_profile_info_t vsi_learning_profile_info;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the vsi learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &vsi_learning_profile));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, vsi_learning_profile, &ref_count, (void *) &vsi_learning_profile_info));

    *learn_limit_per_vsi = vsi_learning_profile_info.limit;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Initialize the aging registers and tables with its default configuration.
 */
static int
dnx_l2_init_aging(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Write the defaults to the aging configuration table
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, &entry_handle_id));

    /** Configure the delete and age out events */
    /** Key fields */
    /** Only dynamic MACT entries */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
    /** RBD=0 means the refresh was done locally and not due to traffic on different devices in the system. RBD (refreshed by
     *  DSP) */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
    /** Age type is the age state. By default when the age state is 0 the entry is deleted/aged out */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, MACT_DEFAULT_AGE_PROFILE);

    /** Result values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_OUT, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_DELETE, INST_SINGLE, 1);

    /** setting the entry with the default values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Configure the refresh events */
    /** Only dynamic MACT entries raise refresh events */
    /** Refresh events are raised by the device if the MAC entry has its maximum age and it reached the maximum not by a DSP. If
     *  the maximum was reached due to a DSP it means all the devices are already aware of the event. */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE,
                               dnx_data_l2.age_and_flush_machine.max_age_states_get(unit) - 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, MACT_DEFAULT_AGE_PROFILE);

    /** Result values */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_OUT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_DELETE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_REFRESH, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l2_module_init(
    int unit)
{
    uint8 is_init = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        l2_db_context.is_init(unit, &is_init);
        if (!is_init)
        {
            l2_db_context.init(unit);
        }

        /** Configure initial OLP configurations */
        SHR_IF_ERR_EXIT(dnx_l2_learn_init_olp(unit));

        /** Configure initial aging tables/registers */
        SHR_IF_ERR_EXIT(dnx_l2_init_aging(unit));

        /** Configure learn limits */
        SHR_IF_ERR_EXIT(dnx_l2_init_learn_limits(unit));

        /** Configure general MACT configurations */
        SHR_IF_ERR_EXIT(dnx_l2_init_general_mact_configs(unit));

        /** Configure the learn payload build */
        SHR_IF_ERR_EXIT(dnx_l2_init_learn_payload_context(unit));

        /** Configure the learn key context for VSIs */
        SHR_IF_ERR_EXIT(dnx_l2_init_learn_key_context(unit));

        /** Init unknown DA flooding profiles */
        SHR_IF_ERR_EXIT(dnx_l2_init_unknown_da_profiles(unit));
    }

    /** Allow writing to registers and changing SW state for the DMA init time */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

    /** Init OLP's DMA */
    SHR_IF_ERR_EXIT(dnx_l2_olp_dma_init(unit));

    /** Init flush's DMA */
    SHR_IF_ERR_EXIT(dnx_l2_flush_dma_init(unit));

    dnx_l2_learn_mact_fifodma_cpu_learn_thread_info_init(unit);

    /** Return to warmboot normal mode (re-enable verifications) */
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_l2_module_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Allow writing to registers and changing SW state for the DMA init time */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SW_STATE));

    /** Deinit OLP's DMA */
    SHR_IF_ERR_CONT(dnx_l2_olp_dma_deinit(unit));

    /** Deinit flush's DMA */
    SHR_IF_ERR_CONT(dnx_l2_flush_dma_deinit(unit));

    SHR_IF_ERR_CONT(dnx_l2_learn_mact_fifodma_cpu_learn_cleanup(unit));

    /** Return to warmboot normal mode (re-enable verifications) */
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SW_STATE));
    SHR_IF_ERR_CONT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_FUNC_EXIT;
}

 /*
  * }
  */

/**
 * \brief - The API gets the value of the limit, set to the number of entries in the MACT/per VSI/per port.
 *
 * \param [in] unit - Relevant unit
 * \param [out] limit - Structure, holding the information about the limit to the numer of entries in MACT, per VSI or per port.
 *             * limit -  Maximum number of entries in the MACT, per VSI or per port. In case CHECK_MACT_DB_LIMIT is unset, returns the LEM size.
 *             * flags - BCM_L2_LEARN_LIMIT_SYSTEM - if the flag is set, information of the global limit will be retrieved..
 *                       BCM_L2_LEARN_LIMIT_VLAN - if the flag is set, information for the limit per VSI will be retrieved
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_l2_learn_limit_get(
    int unit,
    bcm_l2_learn_limit_t * limit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_IF_ERR_EXIT(dnx_l2_learn_limit_get_verify(unit, limit));
    /*
     * Check if the Global Limit is enabled and get the limit value
     */
    if (_SHR_IS_FLAG_SET(limit->flags, BCM_L2_LEARN_LIMIT_SYSTEM))
    {
        SHR_IF_ERR_EXIT(dnx_global_learn_limit_hw_get(unit, &limit->limit));
    }

    /*
     * Get the learn limit per VSI in case that BCM_L2_LEARN_LIMIT_VLAN is set
     */
    if (_SHR_IS_FLAG_SET(limit->flags, BCM_L2_LEARN_LIMIT_VLAN))
    {
        SHR_IF_ERR_EXIT(dnx_learn_limit_per_vsi_get(unit, limit->vlan, &limit->limit));
    }

    /*
     * Get the learn limit per VSI in case that BCM_L2_LEARN_LIMIT_VLAN is set
     */
    if (_SHR_IS_FLAG_SET(limit->flags, BCM_L2_LEARN_LIMIT_PORT))
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        /*
         * Fetch OUT_LIF from gport
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, limit->port,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

        SHR_IF_ERR_EXIT(dnx_learn_limit_per_vsi_get(unit, gport_hw_resources.global_out_lif, &limit->limit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear the aging profile's data. Clear only the fields that are updated using other APIs
 *
 * \param [in] unit - unit id
 * \param [in] aging_profile_id - aging profile id to delete
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_aging_profile_hw_delete(
    int unit,
    int aging_profile_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Clear the aging profile data */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, aging_profile_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_REFRESH, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_DELETE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_OUT, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the aging profile info in the HW
 *
 * \param [in] unit - unit id
 * \param [in] aging_profile_id - new aging profile id given to the vsi learning profile.
 * \param [in] is_first_aging_profile - 1 if the reference counter for new_aging_profile_id is 1. 0 otherwise.
 * \param [in] aging_profile_info - data for the new aging profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_aging_profile_hw_set(
    int unit,
    uint32 aging_profile_id,
    uint8 is_first_aging_profile,
    dnx_aging_profile_info_t * aging_profile_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_first_aging_profile)
    {
        /** Clear the aging profile data */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, aging_profile_id);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_REFRESH, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_DELETE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_OUT, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Set new aging profile data*/
        if (aging_profile_info->refresh_age != AGING_PROFILE_INVALID_AGE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, entry_handle_id));
            /** Update refresh command */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE, aging_profile_info->refresh_age);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, aging_profile_id);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_REFRESH, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /** Update delete command */
        if (aging_profile_info->delete_age != AGING_PROFILE_INVALID_AGE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE, aging_profile_info->delete_age);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, aging_profile_id);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_DELETE, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }

        /** Update age out event */
        if (aging_profile_info->ageout_age != AGING_PROFILE_INVALID_AGE)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, DBAL_ENUM_FVAL_MAC_STRENGTH_1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RBD, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_TYPE, aging_profile_info->ageout_age);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_PROFILE, aging_profile_id);

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_OUT, INST_SINGLE, 1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the aging profile info of a given VSI.
 *
 * \param [in] unit - unit ID
 * \param [in] vsi - vsi to update
 * \param [in] is_lif - age configuration per LIF
 * \param [in] cycle_mask - mask which bits of the cycle_period field should be compared to the current age counter.
 * A bit that is set to 1 is compared against the current age of the age machine. 0 means don't care.
 * \param [in] aging_profile_info - info to set in the aging profile
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_aging_profile_update(
    int unit,
    int vsi,
    int cycle_mask,
    int is_lif,
    dnx_aging_profile_info_t * aging_profile_info)
{
    int old_vsi_learning_profile;
    int new_vsi_learning_profile;
    int new_aging_profile;
    dnx_vsi_learning_profile_info_t old_vsi_learning_profile_info;
    dnx_vsi_learning_profile_info_t new_vsi_learning_profile_info;
    dnx_aging_profile_info_t old_aging_profile_info;
    uint8 is_first = 0;
    uint8 is_last;
    int old_learning_ref_count;
    int old_aging_ref_count;
    uint8 first_reference = 0;
    uint8 last_reference;
    int allocate_failed;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify that we are in VLAN mode */
    SHR_IF_ERR_EXIT(dnx_l2_learn_mode_verify(unit, is_lif));

    /** Allocate aging profile. This function increases the ref count.  */
    allocate_failed =
        algo_l2_db.vsi_aging_profile.allocate_single(unit, 0, aging_profile_info, NULL,
                                                     &new_aging_profile, &first_reference);

    /** Get the vsi learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &old_vsi_learning_profile));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, old_vsi_learning_profile, &old_learning_ref_count, (void *) &old_vsi_learning_profile_info));

    /** Get the old aging profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.profile_data_get
                    (unit, old_vsi_learning_profile_info.aging_profile,
                     &old_aging_ref_count, (void *) &old_aging_profile_info));

    /** Update the learning profile to the new data */
    sal_memcpy(&new_vsi_learning_profile_info, &old_vsi_learning_profile_info, sizeof(dnx_vsi_learning_profile_info_t));
    new_vsi_learning_profile_info.aging_profile = new_aging_profile;
    new_vsi_learning_profile_info.cycle_mask = cycle_mask;
    new_vsi_learning_profile_info.cycle_period = 0;

    if (allocate_failed == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.exchange
                        (unit, 0, &new_vsi_learning_profile_info, old_vsi_learning_profile,
                         NULL, &new_vsi_learning_profile, &is_first, &is_last));

        /*
         *  Since allocate is used on the aging profile, its reference count is increased even if it is not the first learning profile
         *  that is pointing to it. In this case reduce the reference count using free_single.
         */

        if (!is_first)
        {
            SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.free_single(unit, new_aging_profile, &last_reference));
        }
        else
        {
            /** Increase the reference count of the old forwarding profile */
            SHR_IF_ERR_EXIT(dnx_algo_l2_event_forwarding_profile_increase_ref
                            (unit, new_vsi_learning_profile_info.event_forwarding_profile));
        }

        /*
         * In case the old vsi learning profile is not pointed by any vsi, the reference count of its aging profile should be reduced.
         */
        if (is_last || (is_first && new_vsi_learning_profile == old_vsi_learning_profile))
        {
            SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.free_single(unit, old_vsi_learning_profile_info.aging_profile,
                                                                     &last_reference));

            /** Reduce the reference count of the old forwarding profile */
            SHR_IF_ERR_EXIT(dnx_algo_l2_event_forwarding_profile_decrease_ref
                            (unit, old_vsi_learning_profile_info.event_forwarding_profile));
        }
    }
    else
    {
        is_first = 1;
        is_last = 0;

        if (old_learning_ref_count == 1 && old_aging_ref_count == 1)
        {
            SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.replace_data
                            (unit, old_vsi_learning_profile_info.aging_profile, aging_profile_info));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "No more aging profiles to complete the aging profile update");
        }
    }

    /** Update the HW with the new aging profile properties */
    SHR_IF_ERR_EXIT(dnx_aging_profile_hw_set
                    (unit, new_aging_profile, (first_reference || (allocate_failed != _SHR_E_NONE)),
                     aging_profile_info));

    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_hw_update(unit, vsi, old_vsi_learning_profile, new_vsi_learning_profile,
                                                       is_first, is_last, &new_vsi_learning_profile_info));

    /** Reset the old aging profile's data */
    if ((old_learning_ref_count == 1) && (old_aging_ref_count == 1)
        && old_vsi_learning_profile_info.aging_profile != new_aging_profile)
    {
        SHR_IF_ERR_EXIT(dnx_aging_profile_hw_delete(unit, old_vsi_learning_profile_info.aging_profile));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_limit_per_vsi_profile_update(
    int unit,
    int vsi,
    int limit)
{
    int old_vsi_learning_profile;
    int new_vsi_learning_profile;
    dnx_vsi_learning_profile_info_t vsi_learning_profile_info;
    uint8 is_first, is_last;
    int ref_count;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the vsi learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &old_vsi_learning_profile));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, old_vsi_learning_profile, &ref_count, (void *) &vsi_learning_profile_info));

    vsi_learning_profile_info.limit = limit;
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.exchange
                    (unit, 0, &vsi_learning_profile_info, old_vsi_learning_profile,
                     NULL, &new_vsi_learning_profile, &is_first, &is_last));

    /** Update the child profiles */
    if (is_first)
    {
        /** Increase the reference count of the child profile */
        SHR_IF_ERR_EXIT(dnx_algo_l2_event_forwarding_profile_increase_ref
                        (unit, vsi_learning_profile_info.event_forwarding_profile));
        SHR_IF_ERR_EXIT(dnx_algo_l2_aging_profile_increase_ref(unit, vsi_learning_profile_info.aging_profile));
    }

    if (is_last || (is_first && old_vsi_learning_profile == new_vsi_learning_profile))
    {
        /** Decrease the reference count of the child profile */
        SHR_IF_ERR_EXIT(dnx_algo_l2_event_forwarding_profile_decrease_ref
                        (unit, vsi_learning_profile_info.event_forwarding_profile));
        SHR_IF_ERR_EXIT(dnx_algo_l2_aging_profile_decrease_ref(unit, vsi_learning_profile_info.aging_profile));
    }

    /** Update the HW with the new limit per VSI profile */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_hw_update(unit, vsi, old_vsi_learning_profile, new_vsi_learning_profile,
                                                       is_first, is_last, &vsi_learning_profile_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See l2.h
 */
shr_error_e
dnx_aging_profile_init(
    int unit,
    dnx_learn_mode_t learn_mode)
{
    dnx_aging_profile_info_t aging_profile_info;
    int aging_profile = DNX_VSI_AGING_PROFILE_DEFAULT_PROFILE_ID;
    uint8 first_reference;
    int alloc_flags;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    alloc_flags = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;

    memset(&aging_profile_info, 0, sizeof(aging_profile_info));
    switch (learn_mode)
    {
        case LEARN_MODE_DISTRIBUTED:
            aging_profile_info.ageout_age = 0;
            aging_profile_info.delete_age = 0;
            aging_profile_info.refresh_age = dnx_data_l2.age_and_flush_machine.max_age_states_get(unit) - 1;
            break;
        case LEARN_MODE_CENTRALIZED:
        case LEARN_MODE_DMA:
            aging_profile_info.ageout_age = 0;
            aging_profile_info.delete_age = AGING_PROFILE_INVALID_AGE;
            aging_profile_info.refresh_age = dnx_data_l2.age_and_flush_machine.max_age_states_get(unit) - 1;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong learn mode %d \n", learn_mode);
    }

    /** Update the HW with the new aging profile properties */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_MACT_AGING_CONFIGURATION, entry_handle_id));
    SHR_IF_ERR_EXIT(dnx_aging_profile_hw_set(unit, DNX_VSI_AGING_PROFILE_DEFAULT_PROFILE_ID, 1, &aging_profile_info));

    /** Update the aging profile templates with the new parameters */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.clear(unit));

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.allocate_single(unit, alloc_flags, &aging_profile_info, NULL,
                                                                 &aging_profile, &first_reference));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the number of meta cycles until an entry with the given vlan is aged out.
 * Possible number of meta cycles are: 0 (no aging), 1, 2, ..., 8, 10, 12, 14, 16, 32, 64, 128
 *
 * \param [in] unit - unit id
 * \param [in] vsi - fid to update its number of aging cycles before age out.
 * \param [in] is_lif - age configuration per LIF.
 * \param [in] nof_meta_cycles_for_ageout - time in units of meta cycles until the vsi's entries are aged out.
 * e.g. if the meta cycle was configured to 10 seconds and nof_meta_cycles_for_ageout=2, the entries will be delete/aged out
 * after 20 seconds.
 *
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_nof_age_cycles_before_ageout_set(
    int unit,
    int vsi,
    int is_lif,
    int nof_meta_cycles_for_ageout)
{
    int age_for_delete;
    int meta_cycles_mask;
    int meta_cycles_mask_nof_bits;
    int bcm_learn_mode;
    dnx_aging_profile_info_t aging_profile_info;

    SHR_FUNC_INIT_VARS(unit);

    memset(&aging_profile_info, 0, sizeof(aging_profile_info));
    SHR_IF_ERR_EXIT(dnx_l2_learn_mode_get(unit, &bcm_learn_mode));

    if (nof_meta_cycles_for_ageout == -1)
    {
        /** For backward compatibility -1 should be returning the default value */
        nof_meta_cycles_for_ageout = 8;
    }

    switch (nof_meta_cycles_for_ageout)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            meta_cycles_mask_nof_bits = AGE_CYCLE_NO_MASK;
            break;
        case 10:
        case 12:
        case 14:
            meta_cycles_mask_nof_bits = 1;
            break;
        case 16:
            meta_cycles_mask_nof_bits = 2;
            break;
        case 32:
            meta_cycles_mask_nof_bits = 3;
            break;
        case 64:
            meta_cycles_mask_nof_bits = 4;
            break;
        case 128:
            meta_cycles_mask_nof_bits = 5;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "nof meta cycles must be one of the following: 0, 1, 2, ..., 8, 10, 12, 14, 16, 32, 64, 128 .\n");
    }

    meta_cycles_mask = (1 << meta_cycles_mask_nof_bits) - 1;

    if (nof_meta_cycles_for_ageout == 0)
    {
        age_for_delete = AGING_PROFILE_INVALID_AGE;
    }
    else
    {
        age_for_delete =
            dnx_data_l2.age_and_flush_machine.max_age_states_get(unit) -
            (nof_meta_cycles_for_ageout >> meta_cycles_mask_nof_bits);
    }

    /** Delete and update the other devices on the deletion are done on the same age state */
    aging_profile_info.ageout_age = age_for_delete;

    /** Only in distributed mode, entries are deleted by HW on age-out */
    if (bcm_learn_mode & BCM_L2_INGRESS_DIST)
    {
        aging_profile_info.delete_age = age_for_delete;
    }
    else
    {
        aging_profile_info.delete_age = AGING_PROFILE_INVALID_AGE;
    }

    /** Refresh is always done on the same age state */
    aging_profile_info.refresh_age = AGE_FOR_REFRESH;

    /** Update the template manager and HW  */
    SHR_IF_ERR_EXIT(dnx_aging_profile_update(unit, vsi, meta_cycles_mask, is_lif, &aging_profile_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Retrieve the number of meta cycles until an entry with the given vlan is aged out.
 * Possible number of meta cycles are: 0 (no aging), 1, 2, ..., 8, 10, 12, 14, 16, 32, 64, 128
 *
 * \param [in] unit - unit id
 * \param [in] vsi - vsi to fetch its number of aging cycles before age out.
 * \param [out] nof_meta_cycles_for_ageout - time in units of meta cycles until the vsi's entries are aged out.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_l2_nof_age_cycles_before_ageout_get(
    int unit,
    int vsi,
    int *nof_meta_cycles_for_ageout)
{
    int nof_age_states_until_age_out;
    int learning_profile_id;
    int learning_profile_ref_count;
    int aging_profile_ref_count;
    dnx_aging_profile_info_t aging_profile_info;
    dnx_vsi_learning_profile_info_t learning_profile_info;
    uint32 mask_factor;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the learning profile of the vsi */
    SHR_IF_ERR_EXIT(dnx_vsi_learning_profile_id_get(unit, vsi, &learning_profile_id));

    /** Get the vsi learning profile info */
    SHR_IF_ERR_EXIT(algo_l2_db.vsi_learning_profile.profile_data_get
                    (unit, learning_profile_id, &learning_profile_ref_count, (void *) &learning_profile_info));

    SHR_IF_ERR_EXIT(algo_l2_db.vsi_aging_profile.profile_data_get
                    (unit, learning_profile_info.aging_profile, &aging_profile_ref_count,
                     (void *) &aging_profile_info));

    if (aging_profile_info.ageout_age != AGING_PROFILE_INVALID_AGE)
    {
        /** Number of age states until age out */
        nof_age_states_until_age_out =
            dnx_data_l2.age_and_flush_machine.max_age_states_get(unit) - aging_profile_info.ageout_age;

        mask_factor = utilex_nof_on_bits_in_long(learning_profile_info.cycle_mask);

        *nof_meta_cycles_for_ageout = nof_age_states_until_age_out << mask_factor;
    }
    else
    {
        *nof_meta_cycles_for_ageout = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_l2_age_timer_get API
 *
 * \param [in] unit - unit id
 * \param [out] age_seconds - number of seconds between meta cycles
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_age_timer_meta_cycle_get_verify(
    int unit,
    int *age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(age_seconds, _SHR_E_PARAM, "age_seconds");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the number of seconds between two meta cycles of the age machine. 0 seconds means no aging.
 *
 * \param [in] unit - unit id
 * \param [out] meta_cycle_time - number of seconds between meta cycles
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_age_timer_meta_cycle_get(
    int unit,
    int *meta_cycle_time)
{
    uint32 nof_100_clocks_between_metacycles;
    uint32 machine_auto_enable;
    uint32 entry_handle_id;
    dnxcmn_time_t time;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_age_timer_meta_cycle_get_verify(unit, meta_cycle_time));

    /** Get the number of clocks between two meta cycles */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_CYCLE_RATE,
                                                        INST_SINGLE, &nof_100_clocks_between_metacycles));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MACHINE_AUTO_EN,
                                                        INST_SINGLE, &machine_auto_enable));

    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get
                    (unit, nof_100_clocks_between_metacycles, DNXCMN_TIME_UNIT_MILISEC, &time));

    /** Check if the age machine is enabled. In case the age machine is disabled return age_seconds=0. */
    if (machine_auto_enable)
    {
        uint64 work_reg_64;

        /** Clock to time was used on units of 100 clocks. Need to update the time accordingly */
        COMPILER_64_COPY(work_reg_64, time.time);
        COMPILER_64_UDIV_32(work_reg_64, 10);
        *meta_cycle_time = COMPILER_64_LO(work_reg_64);
    }
    else
    {
        *meta_cycle_time = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_l2_age_timer_set API
 *
 * \param [in] unit - unit id
 * \param [in] age_seconds - number of seconds between meta cycles
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_age_timer_set_verify(
    int unit,
    int age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);

    if (age_seconds < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Age time can`t be less than 0 seconds\n");
    }

    if (age_seconds > L2_MAX_AGE_TIME)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Maximum age time that can be set is %d seconds\n", L2_MAX_AGE_TIME);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Stop the age machine. Wait until the age machine is stopped. In case it takes too much time return an error.
 *
 * \param [in] unit - unit id
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_stop_age_machine(
    int unit)
{
    uint32 entry_handle_id;
    uint32 age_machine_status = DBAL_ENUM_FVAL_AGE_SCAN_STATUS_WORKING;
    uint32 nof_status_checks = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MACHINE_AUTO_EN, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_AGE_SCAN, entry_handle_id));

    /** Check if the age machine is still working. '0' means idle. */
    while (age_machine_status != DBAL_ENUM_FVAL_AGE_SCAN_STATUS_IDLE)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AGE_SCAN_STATUS, INST_SINGLE, &age_machine_status);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (age_machine_status == DBAL_ENUM_FVAL_AGE_SCAN_STATUS_IDLE)
        {
            /** In case the age machine stopped, set the meta cycle time */
            break;
        }

        nof_status_checks++;

        /** Sleep 1 milisecond */
        sal_usleep(1000);

        if (nof_status_checks > 5)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Age machine did not stop in time.");
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the number of seconds between two meta cycles of the age machine. 0 seconds means no aging. The default number
 *        of meta cycles until deletion is 8. e.g. age_seconds = 10 seconds means the entries will be deleted after 8*10=80
 *        seconds. Using bcm_vlan_control_vlan_set it is possible to change the 8 iterations to a different value for a given
 *        vsi.
 *
 * \param [in] unit - unit id
 * \param [in] meta_cycle_time - number of seconds between meta cycles
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_age_timer_meta_cycle_set(
    int unit,
    int meta_cycle_time)
{
    uint32 nof_100_clocks_between_metacycles;
    uint8 machine_auto_enable = 0;
    dnxcmn_time_t time_for_100_clocks;
    uint32 entry_handle_id;
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_age_timer_set_verify(unit, meta_cycle_time));

    /*
     * Calculate how many clocks are needed to configure age_seconds as the meta cycle.
     * Since each unit in the hardware is 100 clocks the calculation is done on age_seconds / 100.
     */
    time_for_100_clocks.time_units = DNXCMN_TIME_UNIT_MILISEC;
    /*
     * Implement:
     *   time_for_100_clocks.time = (uint64) (meta_cycle_time * 10);
     */
    COMPILER_64_SET(work_reg_64, 0, meta_cycle_time);
    COMPILER_64_UMUL_32(work_reg_64, 10);
    COMPILER_64_COPY(time_for_100_clocks.time, work_reg_64);

    if (meta_cycle_time > 0)
    {
        machine_auto_enable = 1;
    }

    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time_for_100_clocks, &nof_100_clocks_between_metacycles));

    /** Age machine must be stopped before changing the age time */
    SHR_IF_ERR_EXIT(dnx_l2_stop_age_machine(unit));

    /** Update the number of clocks between two meta cycles */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CYCLE_RATE,
                                 INST_SINGLE, nof_100_clocks_between_metacycles);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MACHINE_AUTO_EN, INST_SINGLE, machine_auto_enable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l2_age_scan_set(
    int unit)
{
    uint32 age_machine_status;
    uint32 machine_auto_enable;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get machine_auto_enable in order to understand if the age machine is enabled */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MACHINE_AUTO_EN, INST_SINGLE, &machine_auto_enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** In case that the age machine is enabled return an error */
    if (machine_auto_enable != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Aging is enabled. Manual triggering of the age machine is possible only when automatic aging is disabled.");
    }

    /** Get the status of age machine. It can be disabled but still working */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_AGE_SCAN, entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AGE_SCAN_STATUS, INST_SINGLE, &age_machine_status);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** Check if the age machine is still working. '0' means idle. */
    if (age_machine_status != DBAL_ENUM_FVAL_AGE_SCAN_STATUS_IDLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The age machine is still working!");
    }

    /** Trigger age scan */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_AGE_SCAN, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AGE_SCAN_TRIGGER, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_AGE_SCAN, entry_handle_id));
    /** Get the status of age machine. Exit from the API once the age machine finishes its work */
    do
    {
        /** Wait 1 milisecond */
        sal_usleep(1000);
        /** Get the status of age machine. Once it is 'idle' a new age scan can be triggered. */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_AGE_SCAN_STATUS, INST_SINGLE, &age_machine_status);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    }
    while (age_machine_status != DBAL_ENUM_FVAL_AGE_SCAN_STATUS_IDLE);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l2_learn_limit_to_cpu_set(
    int unit,
    uint32 enable)
{
    uint32 check_limits;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** check_limits '1' means limits are checked, '0' means limits are not forced for CPU add MACT entry */
    if (enable > 0)
    {
        check_limits = 1;
    }
    else
    {
        check_limits = 0;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    /** Enable/disable per FID entry limit when creating learn event */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EVENT_CHECK_VSI_LIMIT, INST_SINGLE,
                                 check_limits);
    /** Enable/disable per MACT entry limit when creating learn event */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_EVENT_CHECK_MACT_DB_LIMIT, INST_SINGLE,
                                 check_limits);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify function for dnx_l2_learn_limit_to_cpu_get
 */
static shr_error_e
dnx_l2_learn_limit_to_cpu_get_verify(
    int unit,
    int *is_enabled)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_enabled, _SHR_E_PARAM, "is_enabled");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_l2_learn_limit_to_cpu_get(
    int unit,
    int *is_enabled)
{
    uint32 mact_db_limit;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_learn_limit_to_cpu_get_verify(unit, is_enabled));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LEARN_EVENT_CHECK_MACT_DB_LIMIT, INST_SINGLE,
                               &mact_db_limit);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (mact_db_limit == 1)
    {
        *is_enabled = 1;
    }
    else
    {
        *is_enabled = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - The API configures a limit to the number of entries in the MACT/per VSI/per port.
 *
 * \param [in] unit - Relevant unit
 * \param [in] limit - Structure, holding the information about the limit to the numer of entries in MACT. \
 *             * limit - the limit value to be set, in case limit = -1 is used - the limit will be set to the maximum MACT entries value. \
 *             * vlan - VSI to update its limit \
 *             * flags - BCM_L2_LEARN_LIMIT_SYSTEM - the global limit value will be updated, if the flag is present. \
 *                       BCM_L2_LEARN_LIMIT_VLAN - a limit per VSI is updated.
 * \return
 *   Error indication based on enum shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e
bcm_dnx_l2_learn_limit_set(
    int unit,
    bcm_l2_learn_limit_t * limit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_IF_ERR_EXIT(dnx_l2_learn_limit_set_verify(unit, limit));
    /*
     * Check if the Global Limit is enabled and update the DBAL_TABLE_MACT_MANAGEMENT_GENERAL_CONFIGURATION accordingly
     */
    if (_SHR_IS_FLAG_SET(limit->flags, BCM_L2_LEARN_LIMIT_SYSTEM))
    {
        SHR_IF_ERR_EXIT(dnx_global_learn_limit_hw_set(unit, limit->limit));
    }

    /** Set limit per VSI */
    if (_SHR_IS_FLAG_SET(limit->flags, BCM_L2_LEARN_LIMIT_VLAN))
    {
        SHR_IF_ERR_EXIT(dnx_limit_per_vsi_profile_update(unit, limit->vlan, limit->limit));
    }
    /*
     * In case of LIF limit, the existing VSI limit table is used.
     * Therefore, the LIF value should be mapped to the VSI value, according to exact mapping that is done by the HW
 */
    /** Set limit per VSI */
    if (_SHR_IS_FLAG_SET(limit->flags, BCM_L2_LEARN_LIMIT_PORT))
    {
        dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
        /*
         * Fetch OUT_LIF from gport
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, limit->port,
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));

        SHR_IF_ERR_EXIT(dnx_limit_per_vsi_profile_update(unit, gport_hw_resources.global_out_lif, limit->limit));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the properties of a VSI to default values
 *
 * \param [in] unit - unit id
 * \param [in] vsi - vsi to update
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_vsi_profile_set_to_default(
    int unit,
    int vsi)
{
    uint32 predef_max_limit;
    dnx_event_forwarding_profile_info_t event_forwarding_profile_info;
    int ref_count;
    uint32 entry_handle_id;
    bcm_l2_learn_limit_t learn_limit;
    dnx_learn_limit_mode_t learn_limit_mode;
    int is_lif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    learn_limit_mode = dnx_data_l2.general.l2_learn_limit_mode_get(unit);
    if (learn_limit_mode == LEARN_LIMIT_MODE_VLAN)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_VSI_LEARNING_PROFILE_INFO, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id, DBAL_FIELD_LEARN_LIMIT,
                                                        0, DBAL_PREDEF_VAL_MAX_VALUE, &predef_max_limit));
        learn_limit.limit = predef_max_limit;
        learn_limit.vlan = vsi;
        learn_limit.flags = BCM_L2_LEARN_LIMIT_VLAN;
        SHR_IF_ERR_EXIT(bcm_dnx_l2_learn_limit_set(unit, &learn_limit));
    }

    /** Update number of aging cycles for the vsi */
    is_lif = (dnx_data_l2.general.l2_learn_limit_mode_get(unit) == LEARN_LIMIT_MODE_VLAN_PORT);
    SHR_IF_ERR_EXIT(dnx_l2_nof_age_cycles_before_ageout_set(unit, vsi, is_lif, DEFAULT_NOF_AGE_CYCLES));

    /** Get the info of forwarding profile 0 (default profile) */
    SHR_IF_ERR_EXIT(algo_l2_db.event_forwarding_profile.profile_data_get
                    (unit, 0, &ref_count, (void *) &event_forwarding_profile_info));

    SHR_IF_ERR_EXIT(dnx_event_forwarding_profile_update(unit, vsi, is_lif, &event_forwarding_profile_info));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear the l2 callbacks, deinit and init the l2 module
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_clear(
    int unit)
{
    int i = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Clear the l2 callbacks if there are such registered */
    if (dnx_l2_cbs[unit].count != 0)
    {
        for (i = 0; i < DNX_L2_CB_MAX; i++)
        {
            if ((dnx_l2_cbs[unit].entry[i].callback != NULL) && (dnx_l2_cbs[unit].entry[i].userdata != NULL))
            {
                dnx_l2_cbs[unit].entry[i].callback = NULL;
                dnx_l2_cbs[unit].entry[i].userdata = NULL;
                dnx_l2_cbs[unit].count--;
            }
            if (dnx_l2_cbs[unit].count == 0)
            {
                break;
            }
        }
    }

    /** Deinit l2 module */
    SHR_IF_ERR_EXIT(dnx_l2_module_deinit(unit));

    /** Init l2 module */
    SHR_IF_ERR_EXIT(dnx_l2_module_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Return forwarding information of L2 gport
 *
 * \param [in] unit - Relevant unit
 * \param [in] gport_id - Gport to get information from
 * \param [out] frwrd_info - Forwarding information returned: Port (physical or FEC) + encap id
 *             * phy_gport - Physical gport or FEC gport
 *             * encap_id - Encapsulation id (OutLif)
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_gport_forward_info_get(
    int unit,
    int gport_id,
    bcm_l2_gport_forward_info_t * frwrd_info)
{
    dnx_algo_gpm_forward_info_t forward_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * First, get forwarding information from logical gport
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, gport_id, BCM_FORWARD_ENCAP_ID_INVALID, &forward_info));
    /*
     * Now convert forward information to port+encap
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_from_forward_information
                    (unit, &frwrd_info->phy_gport, (uint32 *) &frwrd_info->encap_id, &forward_info, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify l2 gport control information for bcm_dnx_l2_gport_control_info_set/get.
 * \param [in] unit - The unit number.
 * \param [in] control - The l2 gport control information structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_l2_gport_control_info_verify(
    int unit,
    bcm_l2_gport_control_info_t * control)
{
    SHR_FUNC_INIT_VARS(unit);

    if (control->entropy_id != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No support for control->entropy_id in the bcm_vlan_control_vlan_set");
    }
    if (control->flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No support for control->flags");
    }

    if (control->aging_cycles != 0)
    {
        if (dnx_data_l2.general.l2_learn_limit_mode_get(unit) != LEARN_LIMIT_MODE_VLAN_PORT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Limit per VLAN_PORT cannot be managed in %s mode", LEARN_LIMIT_MODE_NAME(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get l2 control infomration(aging profile ID, number of aging meta-cycles) to a given gport
 *
 * \param [in] unit - unit ID
 * \param [in] control_info
 *   * flags - flags has bitmap defined using.
 *   * gport - gport to update.
 *   * aging_cycles - number of aging meta-cycles.
 *   * entropy_id - aging profile ID.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_gport_control_info_get(
    int unit,
    bcm_l2_gport_control_info_t * control_info)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_gport_control_info_verify(unit, control_info));
    /*
     * Fetch OUT_LIF from gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, control_info->gport,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));
    /*
     * Get number of aging meta-cycles
     */
    SHR_IF_ERR_EXIT(dnx_l2_nof_age_cycles_before_ageout_get
                    (unit, gport_hw_resources.global_out_lif, &control_info->aging_cycles));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update l2 control infomration(aging profile ID, number of aging meta-cycles) to a given gport
 *
 * \param [in] unit - unit ID
 * \param [in] control_info
 *   * flags - flags has bitmap defined using.
 *   * gport - gport to update.
 *   * aging_cycles - number of aging meta-cycles.
 *   * entropy_id - aging profile ID.
 *
 * \return
 *   int
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_l2_gport_control_info_set(
    int unit,
    bcm_l2_gport_control_info_t * control_info)
{
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l2_gport_control_info_verify(unit, control_info));
    /*
     * Fetch OUT_LIF from gport
     */
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, control_info->gport,
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                     DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT, &gport_hw_resources));
    /** Update number of aging cycles */
    SHR_IF_ERR_EXIT(dnx_l2_nof_age_cycles_before_ageout_set
                    (unit, gport_hw_resources.global_out_lif, 1, control_info->aging_cycles));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
