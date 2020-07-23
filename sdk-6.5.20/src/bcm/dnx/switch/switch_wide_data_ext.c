/**
 * \file switch_wide_data_ext.c
 *
 * WIDE_DATA_EXT - extension for wide (generic) data in lif field
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

/*
 * Include files.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <include/bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/switch/switch_wide_data_ext.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dbal/dbal.h>

/*
 * }
 */

static shr_error_e
dnx_switch_wide_data_extension_key_verify(
    int unit,
    uint32 key)
{
    int nof_bits;
    uint32 compare_val = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA,
                               DBAL_FIELD_IN_LIF_DIRECT_ADDITIONAL_DATA, TRUE, 0, 0, &nof_bits));
    SHR_BITSET(&compare_val, nof_bits);

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
    SHR_FUNC_INIT_VARS(unit);
    SHR_MASK_VERIFY(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_REPLACE, _SHR_E_PARAM, "Incorrect flags\n");
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    /** check the key size */
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_verify(unit, info->wide_data_key));
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
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Incorrect flags\n");
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_verify(unit, info->wide_data_key));
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
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Incorrect flags\n");
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_switch_wide_data_extension_key_verify(unit, info->wide_data_key));
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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_wide_data_extension_add_verify(unit, flags, info));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_DIRECT_ADDITIONAL_DATA, info->wide_data_key);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, extension);
    /** Create the entry if it doesn't exist, update it if it does. */
    if (_SHR_IS_FLAG_SET(flags, BCM_SWITCH_WIDE_DATA_EXTENSION_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
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
    uint32 extension[2];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_wide_data_extension_get_verify(unit, flags, info));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_DIRECT_ADDITIONAL_DATA, info->wide_data_key);
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
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_switch_wide_data_extension_delete_verify(unit, flags, info));

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, &entry_handle_id));
    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IN_LIF_DIRECT_ADDITIONAL_DATA, info->wide_data_key);
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
    uint32 extension[2];
    bcm_switch_wide_data_extension_info_t info;
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
        /** Receive key and value fields of the entry. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF_DIRECT_ADDITIONAL_DATA, &info.wide_data_key));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, extension));

        COMPILER_64_SET(info.wide_data_result, extension[1], extension[0]);
        /*
         * Invoke callback function
         */
        SHR_IF_ERR_EXIT((*cb) (unit, 0, &info, user_data));

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
