/**
 * \file switch_wide_data_ext.c
 *
 * WIDE_DATA_EXT - extension for wide (generic) data in lif field
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <include/bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/switch/switch_wide_data_ext.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_kleap_stage_info.h>
#include <soc/dnx/swstate/auto_generated/access/switch_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
/*
 * }
 */
#define SWITCH_WIDE_DATA_EXT_DBAL_FIELD_KEY(unit) \
    (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_data) ? \
             DBAL_FIELD_IN_LIF_DIRECT_ADDITIONAL_DATA : DBAL_FIELD_GLOB_IN_LIF)

#define SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_VAL (0xF)
#define SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_SIZE (4)
#define SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_POSITION (18)
#define SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_FFC_LOOKUP_OFFSET (20)
#define SWITCH_WIDE_DATA_EXT_KEY_BY_GLOBAL_LIF_FFC_LOOKUP_OFFSET (64)

extern shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);

static shr_error_e
dnx_switch_wide_data_extension_key_verify(
    int unit,
    uint32 key)
{
    uint32 compare_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    compare_val = 1 << (SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_POSITION);

    if (key >= compare_val)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "wide_data_key=%d, must be smaller than %d \n", key, compare_val);
    }
exit:
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_switch_wide_data_extension_add_verify(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info)
{
    int nof_bits;
    uint64 compare_value;
    bcm_gport_t gport;
    bcm_switch_wide_data_extension_key_mode_t key_global_mode = bcmSwitchWideDataExtKeyModeByWideData;
    uint32 allowed_flags = BCM_SWITCH_WIDE_DATA_EXTENSION_REPLACE;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_ac_lif))
    {
        allowed_flags |= BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY;
        /** get the global key mode */
        SHR_IF_ERR_EXIT(dnx_switch_wide_data_ext_key_mode_get(unit, &key_global_mode));
    }
    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_eth_rif))
    {
        allowed_flags |= BCM_SWITCH_WIDE_DATA_EXTENSION_INTF_KEY;
    }
    SHR_MASK_VERIFY(flags, allowed_flags, _SHR_E_PARAM, "Incorrect flags\n");
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    /** get the global key mode */
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_ext_key_mode_get(unit, &key_global_mode));

    if (!_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_REPLACE))
    {
        if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY))
        {
            gport = info->gport_key;
            /** if the global mode is wide data - do not give gport key */
            if (key_global_mode == bcmSwitchWideDataExtKeyModeByWideData)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WideDataExtKeyMode=wida_data. Do not give gport key");
            }
            /** verify gport is flow_id or vlan_port, since it should be AC gport */
            if ((!BCM_GPORT_IS_FLOW_LIF(info->gport_key)) && (!BCM_GPORT_IS_VLAN_PORT(info->gport_key)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid type of gport_key (%d)", info->gport_key);
            }
            /** verify the gport is not virtual */
            if (BCM_GPORT_IS_VLAN_PORT(info->gport_key) &&
                dnx_data_flow.general.feature_get(unit, dnx_data_flow_general_is_flow_enabled_for_legacy_applications))
            {
                SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert
                                (unit, info->gport_key, BCM_GPORT_TYPE_FLOW_LIF,
                                 DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &gport));
            }
            if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS(gport) || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "gport_key (%d) is virtual. Must be gport with valid global_lif",
                             info->gport_key);
            }
        }
        else if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_INTF_KEY))
        {
            /** if global mode is not "first", i.e. the key is per AC or RIF, do not set intf_key */
            if (key_global_mode != bcmSwitchWideDataExtKeyModeByFirstLif)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WideDataExtKeyMode!=FirstLif. Do not set intf key");
            }
            if (!BCM_L3_ITF_TYPE_IS_RIF(info->intf_key))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "intf_key (%d) must be L3_ITF RIF type.", info->intf_key);
            }
        }
        else
        {
            if (key_global_mode != bcmSwitchWideDataExtKeyModeByWideData)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WideDataExtKeyMode!=wide_data. Do not set wide_data key");
            }
            /** check the key size */
            SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_verify(unit, info->wide_data_key));
        }
    }

    /** check the result size */
    dbal_tables_field_size_get(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, DBAL_FIELD_RAW_DATA, FALSE, 0, 0, &nof_bits);
    COMPILER_64_ZERO(compare_value);
    COMPILER_64_BITSET(compare_value, nof_bits);
    if (COMPILER_64_GE(info->wide_data_result, compare_value))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "wide_data_result={0x%x,0x%x}, must be smaller than {0x%x,0x%x} \n",
                     COMPILER_64_HI(info->wide_data_result), COMPILER_64_LO(info->wide_data_result),
                     COMPILER_64_HI(compare_value), COMPILER_64_LO(compare_value));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_wide_data_extension_get_verify(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_ac_lif) &&
        dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_eth_rif))
    {
        SHR_MASK_VERIFY(flags, (BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY | BCM_SWITCH_WIDE_DATA_EXTENSION_INTF_KEY),
                        _SHR_E_PARAM, "Incorrect flags\n");
    }
    else if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_ac_lif))
    {
        SHR_MASK_VERIFY(flags, (BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY), _SHR_E_PARAM, "Incorrect flags\n");
    }
    else
    {
        SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Incorrect flags\n");
    }
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    if (flags == 0)
    {
        SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_verify(unit, info->wide_data_key));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_wide_data_extension_delete_verify(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_ac_lif) &&
        dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_eth_rif))
    {
        SHR_MASK_VERIFY(flags, (BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY | BCM_SWITCH_WIDE_DATA_EXTENSION_INTF_KEY),
                        _SHR_E_PARAM, "Incorrect flags\n");
    }
    else if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_ac_lif))
    {
        SHR_MASK_VERIFY(flags, (BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY), _SHR_E_PARAM, "Incorrect flags\n");
    }
    else
    {
        SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Incorrect flags\n");
    }
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    if (flags == 0)
    {
        SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_verify(unit, info->wide_data_key));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_wide_data_extension_key_get(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info,
    uint32 *key)
{
    bcm_gport_t flow_id;
    SHR_FUNC_INIT_VARS(unit);
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY))
    {
        /** get global_lif from gport. key must be flow gport or vlan_port gport, since it is the AC gport */
        SHR_IF_ERR_EXIT(algo_gpm_gport_flow_convert(unit, info->gport_key, BCM_GPORT_TYPE_FLOW_LIF,
                                                    DNX_ALGO_GPM_CONVERSION_FLAG_FLOW_TERMINATOR, &flow_id));
        *key = _SHR_GPORT_FLOW_LIF_VAL_GET_ID(flow_id);
    }
    else if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_INTF_KEY))
    {
        *key = BCM_L3_ITF_VAL_GET(info->intf_key);
    }
    else
    {
        *key = (SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_VAL << SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_POSITION) |
            info->wide_data_key;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: Adding a EXEM entry which extend the in lif wide (generic) data field
 *
 * \param [in] unit -relevant unit.
 * \param [in] flags - flags.
 * \param [in] info -wide data extension info - key and result of the entry;
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark 
 *  None
 */
int
bcm_dnx_switch_wide_data_extension_add(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info)
{
    uint32 entry_handle_id;
    uint32 extension[2] = { COMPILER_64_LO(info->wide_data_result), COMPILER_64_HI(info->wide_data_result) };
    uint32 key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_switch_wide_data_extension_add_verify(unit, flags, info));
    /** get the key to the extension */
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_get(unit, flags, info, &key));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, SWITCH_WIDE_DATA_EXT_DBAL_FIELD_KEY(unit), key);

    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, extension);
    /** Create the entry if it doesn't exist, update it if it does. */
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: get in lif wide (generic) data extension entry result from EXEM HW DB.
 *
 * \param [in] unit -relevant unit.
 * \param [in] flags - flags.
 * \param [inout] info -wide data extension info - key (inout) and result (output) of the entry.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark
 *  None
 */
int
bcm_dnx_switch_wide_data_extension_get(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info)
{
    uint32 entry_handle_id;
    uint32 extension[2] = { 0, 0 };
    uint32 key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_switch_wide_data_extension_get_verify(unit, flags, info));
    /** get the key to the extension */
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_get(unit, flags, info, &key));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, SWITCH_WIDE_DATA_EXT_DBAL_FIELD_KEY(unit), key);
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, extension);

    /** get */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    COMPILER_64_SET(info->wide_data_result, extension[1], extension[0]);
exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: delete in lif wide (generic) data extension entry
 *
 * \param [in] unit -relevant unit.
 * \param [in] flags - flags.
 * \param [in] info -wide data extension info - key.
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark
 *  None
 */
int
bcm_dnx_switch_wide_data_extension_delete(
    int unit,
    uint32 flags,
    bcm_switch_wide_data_extension_info_t * info)
{
    uint32 entry_handle_id;
    uint32 key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNXC(dnx_switch_wide_data_extension_delete_verify(unit, flags, info));
    /** get the key to the extension */
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_get(unit, flags, info, &key));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, SWITCH_WIDE_DATA_EXT_DBAL_FIELD_KEY(unit), key);
    /** clear */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * BCM API: API traverse over all valid entries from wide data extension database
 *
 * \param [in] unit -relevant unit.
 * \param [in] cb - callback structure.
 * \param [in] user_data - general user data
 *
 * \return
 *   shr_error_e \n
 *      BCM_E_NONE - Success. \n
 *      BCM_E_XXX - Fails.
 *
 * \remark
 *  None
 */
int
bcm_dnx_switch_wide_data_extension_traverse(
    int unit,
    bcm_switch_wide_data_extension_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    int is_end;
    uint32 extension[2] = { 0, 0 };
    bcm_switch_wide_data_extension_info_t info;
    bcm_switch_wide_data_extension_key_mode_t mode;
    uint32 flags = 0, gport_key;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(cb, _SHR_E_PARAM, "cb");

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        /** Receive key and value fields of the entry.
         *  if key is wide data, set it on wide_data_key and remove its prefix,
         *  otherwise, set it on the gport_key with basic flow decoding, since it is unknown if it came from gport/intf.
         */
        SHR_IF_ERR_EXIT(dnx_switch_wide_data_ext_key_mode_get(unit, &mode));
        if (mode == bcmSwitchWideDataExtKeyModeByWideData)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, SWITCH_WIDE_DATA_EXT_DBAL_FIELD_KEY(unit), &info.wide_data_key));
            SHR_BITCLR_RANGE(&info.wide_data_key,
                             SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_POSITION,
                             SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_PREFIX_SIZE);
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, SWITCH_WIDE_DATA_EXT_DBAL_FIELD_KEY(unit), &gport_key));
            BCM_GPORT_FLOW_LIF_ID_SET(info.gport_key, gport_key);
            flags |= BCM_SWITCH_WIDE_DATA_EXTENSION_GPORT_KEY;
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, extension));

        COMPILER_64_SET(info.wide_data_result, extension[1], extension[0]);
        /*
         * Invoke callback function
         */
        SHR_IF_ERR_EXIT((*cb) (unit, flags, &info, user_data));

        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_switch_wide_data_ext_key_mode_verify(
    int unit,
    bcm_switch_wide_data_extension_key_mode_t mode,
    int is_set)
{
    uint32 entry_handle_id;
    int is_end;
    uint8 is_image_standard_1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_set)
    {
        if (mode != bcmSwitchWideDataExtKeyModeByFirstLif && mode != bcmSwitchWideDataExtKeyModeByAcLif &&
            mode != bcmSwitchWideDataExtKeyModeByWideData)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid wide data extension key mode (=%d)", mode);
        }
        if (mode == bcmSwitchWideDataExtKeyModeByFirstLif &&
            !dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_eth_rif))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "wide data extension key mode bcmSwitchWideDataExtKeyModeByFirstLif is not supported");
        }
        if (mode == bcmSwitchWideDataExtKeyModeByAcLif &&
            !dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_global_ac_lif))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "wide data extension key mode bcmSwitchWideDataExtKeyModeByAcLif is not supported");
        }
        if (mode == bcmSwitchWideDataExtKeyModeByWideData &&
            !dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_data))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "wide data extension key mode bcmSwitchWideDataExtKeyModeByWideData is not supported");
        }
        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_image_standard_1));
        if (is_image_standard_1)
        {
            /** check EM_PMF_ADDITIONAL_DATA is empty when setting mode */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            if (is_end == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "unable to set new mode when EM_PMF_ADDITIONAL_DATA table is not empty");
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get the kbr index of EM_PMF lookup
 *
 * \param [in] unit -relevant unit.
 * \param [out] kbr_id - kbr index
 * \return
 *   shr_error_e \n
 * \remark
 *  None
 */
static shr_error_e
switch_wide_data_ext_key_kbr_id_get(
    int unit,
    uint32 *kbr_id)
{
    uint32 nof_stages, stage_idx, kbr_idx;
    SHR_FUNC_INIT_VARS(unit);
    nof_stages = dnx_data_kleap_stage_info.kleap_stage_info.nof_kleap_stages_get(unit);
    /** find FWD12 stage index in kleap db */
    for (stage_idx = 0; stage_idx < nof_stages; stage_idx++)
    {
        if (!sal_memcmp(dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_get(unit, stage_idx)->stage_name,
                        "FWD12", 6))
        {
            break;
        }
    }
    if (stage_idx == nof_stages)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FWD12 stage wasn't found in kleap database");
    }
    /** find the kbr_idx of the phy db belong to wide data extension lookup */
    for (kbr_idx = 0; kbr_idx <
         dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_kbr_get(unit, stage_idx)->nof_kbrs; kbr_idx++)
    {
        if (dnx_data_kleap_stage_info.kleap_stage_info.
            info_per_stage_per_kbr_get(unit, stage_idx)->kbr2physical[kbr_idx] == DBAL_PHYSICAL_TABLE_SEXEM_2)
        {
            break;
        }
    }
    if (kbr_idx == dnx_data_kleap_stage_info.kleap_stage_info.info_per_stage_per_kbr_get(unit, stage_idx)->nof_kbrs)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBR wasn't found in kleap database for FWD12 stage, phy table SEXEM_2");
    }
    *kbr_id = kbr_idx;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get the EM_PMF key lookup base key offset. which is the beginning of container1_96.
 * Used to be able to override the key offset later on.
 *
 * \param [in] unit -relevant unit.
 * \param [out] base_offset - base offset of wide data extension key
 * \return
 *   shr_error_e \n
 * \remark
 *  None
 */
static shr_error_e
switch_wide_data_ext_key_ffc_base_offset_get(
    int unit,
    uint32 *base_offset)
{
    uint32 entry_handle_id;
    uint32 ffc_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)] = { 0 };
    int context_profile;
    uint8 is_illegal;
    uint32 context_hw_val;
    uint32 app_db_id;
    dbal_tables_e logical_table;
    uint32 kbr_idx, ffc_idx, ffc_instruction;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(switch_wide_data_ext_key_kbr_id_get(unit, &kbr_idx));
    /** run over all contexts. */
    for (context_profile = 0; context_profile < DBAL_NOF_ENUM_FWD1_CONTEXT_ID_VALUES; context_profile++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_profile, &is_illegal));
        /** use the firs legal context */
        if (is_illegal == FALSE)
        {
            SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                            (unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_profile, &context_hw_val));
            /** alloc DBAL table handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, kbr_idx);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_hw_val);

            /** request for value field */
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, &app_db_id);
            /** Set the ffc id's bitmap*/
            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bmp);
            /** get value */
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
            /** convert phy db id to logical db */
            SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get
                            (unit, DBAL_PHYSICAL_TABLE_SEXEM_2, app_db_id, &logical_table));
            /** verify the table is EM_PMF_ADDITIONAL_DATA */
            if (logical_table != DBAL_TABLE_EM_PMF_ADDITIONAL_DATA)
            {
                continue;
            }
            if (logical_table != DBAL_TABLE_EM_PMF_ADDITIONAL_DATA)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "logical_table=%d, expected %d (DBAL_TABLE_EM_PMF_ADDITIONAL_DATA) context_profile=%d\n",
                             logical_table, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, context_profile);
            }
            SHR_IF_ERR_EXIT(utilex_bitstream_size_of_bitstream_in_bits
                            (ffc_bmp, BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC), &ffc_idx));
            ffc_idx -= 1;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_FFC_INSTRUCTION, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_idx);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_hw_val);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD12_FFC_INSTRUCTION, INST_SINGLE,
                                       &ffc_instruction);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_FWD12_FFC_LITERALLY, DBAL_FIELD_FFC_FIELD_OFFSET, base_offset,
                             &ffc_instruction));
            /** the ucode is configured with EXT_KEY BY DATA, therfore, to get the base offset, need to remove its offset */
            *base_offset = (*base_offset) - SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_FFC_LOOKUP_OFFSET;

            /** once found base offset - break */
            break;
        }

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * set the EM_PMF lookup key offset. This is made by sdk due to resources issues.
 *
 * \param [in] unit -relevant unit.
 * \param [in] mode - wide data extension key lookup mode.
 * \return
 *   shr_error_e \n
 * \remark
 *  None
 */
static shr_error_e
switch_wide_data_ext_key_ffc_offset_set(
    int unit,
    bcm_switch_wide_data_extension_key_mode_t mode)
{
    uint32 entry_handle_id;
    uint32 ffc_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)] = { 0 };
    int context_profile;
    uint8 is_illegal;
    uint32 context_hw_val;
    uint32 app_db_id;
    dbal_tables_e logical_table;
    uint32 kbr_idx, ffc_idx, ffc_instruction;
    int ffc_lookup_offset;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(switch_wide_data_ext_key_kbr_id_get(unit, &kbr_idx));
    /** determine the ffc lookup offset base on the mode */
    SHR_IF_ERR_EXIT(switch_db.wide_data_extension_key_ffc_base_offset.get(unit, &ffc_lookup_offset));
    if (mode == bcmSwitchWideDataExtKeyModeByWideData)
    {
        ffc_lookup_offset += SWITCH_WIDE_DATA_EXT_KEY_BY_DATA_FFC_LOOKUP_OFFSET;
    }
    else
    {
        ffc_lookup_offset += SWITCH_WIDE_DATA_EXT_KEY_BY_GLOBAL_LIF_FFC_LOOKUP_OFFSET;
    }

    /** run over all contexts. */
    for (context_profile = 0; context_profile < DBAL_NOF_ENUM_FWD1_CONTEXT_ID_VALUES; context_profile++)
    {
        logical_table = 0;
        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_profile, &is_illegal));
        if (is_illegal)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_profile, &context_hw_val));
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, kbr_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_hw_val);

        /** request for value field */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, &app_db_id);
        /** Set the ffc id's bitmap*/
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bmp);
        /** get value */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
        /** convert phy db id to logical db */
        SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_2, app_db_id, &logical_table));
        /** verify the table is EM_PMF_ADDITIONAL_DATA */
        if (logical_table != DBAL_TABLE_EM_PMF_ADDITIONAL_DATA)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_size_of_bitstream_in_bits
                        (ffc_bmp, BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC), &ffc_idx));
        ffc_idx -= 1;
        /** get the current instruction and update the offset */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_FFC_INSTRUCTION, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_hw_val);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FWD12_FFC_INSTRUCTION, INST_SINGLE,
                                   &ffc_instruction);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_FWD12_FFC_LITERALLY, DBAL_FIELD_FFC_FIELD_OFFSET,
                         (uint32 *) &ffc_lookup_offset, &ffc_instruction));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
        /** set new instruction to HW */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_FFC_INSTRUCTION, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_hw_val);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_FFC_INSTRUCTION, INST_SINGLE,
                                     ffc_instruction);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file description */
shr_error_e
dnx_switch_wide_data_ext_key_mode_set(
    int unit,
    bcm_switch_wide_data_extension_key_mode_t mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_switch_wide_data_ext_key_mode_verify(unit, mode, TRUE));
    SHR_IF_ERR_EXIT(switch_db.wide_data_extension_key_mode.set(unit, mode));

    /** if ext key by wide data supported, need to set the KEY offset in FFC here and not by ucode, due to resources issues */
    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_data))
    {
        SHR_IF_ERR_EXIT(switch_wide_data_ext_key_ffc_offset_set(unit, mode));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see header file description */
shr_error_e
dnx_switch_wide_data_ext_key_mode_get(
    int unit,
    bcm_switch_wide_data_extension_key_mode_t * mode)
{
    int key_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(switch_db.wide_data_extension_key_mode.get(unit, &key_mode));
    *mode = (bcm_switch_wide_data_extension_key_mode_t) key_mode;
exit:
    SHR_FUNC_EXIT;
}

/** see header file description */
shr_error_e
dnx_switch_wide_data_init(
    int unit)
{
    uint32 base_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** get the base offset of general data in the packet. save it and set it back on the ffc, based on the mode */
    /** get it from the ext lookup key that was set by ucode. */
    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_data))
    {
        SHR_IF_ERR_EXIT(switch_wide_data_ext_key_ffc_base_offset_get(unit, &base_offset));
        SHR_IF_ERR_EXIT(switch_db.wide_data_extension_key_ffc_base_offset.set(unit, (int) base_offset));
    }
    /** set the default wide data extension key mode */
    if (dnx_data_switch.wide_data.feature_get(unit, dnx_data_switch_wide_data_ext_key_by_data))
    {
        SHR_IF_ERR_EXIT(dnx_switch_wide_data_ext_key_mode_set(unit, bcmSwitchWideDataExtKeyModeByWideData));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_switch_wide_data_ext_key_mode_set(unit, bcmSwitchWideDataExtKeyModeByAcLif));
    }
exit:
    SHR_FUNC_EXIT;
}
