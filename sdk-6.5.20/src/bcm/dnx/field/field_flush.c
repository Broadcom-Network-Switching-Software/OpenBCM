/** \file field_flush.c
 * 
 *
 * Field flush procedures for DNX.
 *
 * This file contains all procedures to be called by user application (Host API's)
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <sal/core/libc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_flush.h>
#include <bcm/error.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>


#include "../../../soc/dnx/dbal/dbal_internal.h"

#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>

/*
 * }Include files
 */

/*
 * }Include files
 */

/**See H in field_entry.h*/
shr_error_e
dnx_field_flush_entry_t_init(
    int unit,
    dnx_field_flush_entry_t * flush_entry_info_p)
{
    int qual_index;
    int action_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_info_p, _SHR_E_PARAM, "flush_entry_info_p");

    sal_memset(flush_entry_info_p, 0x0, sizeof(*flush_entry_info_p));

    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_index++)
    {
        flush_entry_info_p->key_info.qual_info[qual_index].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }
    for (action_index = 0; action_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_index++)
    {
        flush_entry_info_p->key_info.action_info[action_index].dnx_action = DNX_FIELD_ACTION_INVALID;
    }
    for (action_index = 0; action_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_index++)
    {
        flush_entry_info_p->payload_info.action_info[action_index].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_field_flush_profile_create
 */
static shr_error_e
dnx_field_flush_profile_create_profile_info_parameter_verify(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 parameter)
{
    int field_size;
    int instance;
    uint32 param_max_value;

    SHR_FUNC_INIT_VARS(unit);


    if (table_id == DBAL_TABLE_MDB_20_EXEM_3)
    {
        instance = INST_SINGLE;
    }
    else
    {
        instance = 0;
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, field_id, FALSE, 0, instance, &field_size));

    param_max_value = (2 << field_size) - 1;

    if (parameter > param_max_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! flush_profile_info parameter (=%d) is out of range [0:%d]! \n",
                     parameter, param_max_value);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_field_flush_profile_create
 */
static shr_error_e
dnx_field_flush_profile_create_verify(
    int unit,
    uint32 flags,
    bcm_field_flush_profile_info_t * flush_profile_info,
    uint32 *flush_profile_id)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flush_profile_info, _SHR_E_PARAM, "flush_profile_info");
    SHR_NULL_CHECK(flush_profile_id, _SHR_E_PARAM, "flush_profile_id");

    /*
     * Check supported flags
     */
    if (flags & (~(BCM_FIELD_FLAG_WITH_ID)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Unsupported flag: flags = 0x%08x. Supported flags are: BCM_FIELD_FLAG_WITH_ID = 0x%08x \n",
                     flags, BCM_FIELD_FLAG_WITH_ID);
    }

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }

    /*
     * "WITH_ID":
     *   - check profile ID is in range
     *   - check profile ID is not already in use.
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_FIELD_FLAG_WITH_ID))
    {
        uint32 nof_flush_profiles = dnx_data_field.exem.small_nof_flush_profiles_get(unit);
        uint8 is_allocated;

        if (*flush_profile_id >= nof_flush_profiles)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Error! The requested flush_profile_id (=%d) is out of range [0:%d]! \n",
                         *flush_profile_id, nof_flush_profiles - 1);
        }

        SHR_IF_ERR_EXIT(dnx_algo_field_flush_profile_is_allocated(unit, *flush_profile_id, &is_allocated));

        if (is_allocated == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS,
                         "Error! The requested flush_group_id (=%d) is already allocated! \n", *flush_profile_id);
        }
    }

    /*
     * Check flush_profile_info
     */


    if (dnx_data_mdb.em.age_profile_per_ratio_support_get(unit))
    {
        table_id = DBAL_TABLE_MDB_20_EXEM_3;
    }
    else
    {
        table_id = DBAL_TABLE_MDB_19_EXEM_3;
    }

    SHR_IF_ERR_EXIT(dnx_field_flush_profile_create_profile_info_parameter_verify
                    (unit, table_id, DBAL_FIELD_MDB_ITEM_0, flush_profile_info->high_threshold_value));
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_create_profile_info_parameter_verify
                    (unit, table_id, DBAL_FIELD_MDB_ITEM_1, flush_profile_info->low_threshold_value));
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_create_profile_info_parameter_verify
                    (unit, table_id, DBAL_FIELD_MDB_ITEM_2, flush_profile_info->maximal_value));
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_create_profile_info_parameter_verify
                    (unit, table_id, DBAL_FIELD_MDB_ITEM_3, flush_profile_info->increment_value));
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_create_profile_info_parameter_verify
                    (unit, table_id, DBAL_FIELD_MDB_ITEM_4, flush_profile_info->decrement_value));
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_create_profile_info_parameter_verify
                    (unit, table_id, DBAL_FIELD_MDB_ITEM_5, flush_profile_info->out_value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - allocates a flush profile ID
 */
static shr_error_e
dnx_field_flush_profile_allocate(
    int unit,
    uint32 flags,
    uint32 *flush_profile_id)
{
    uint32 alloc_flags;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_IS_FLAG_SET(flags, BCM_FIELD_FLAG_WITH_ID))
    {
        alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {
        alloc_flags = 0;
    }

    SHR_IF_ERR_EXIT(algo_field_info_sw.flush_profile_id.allocate_single(unit,
                                                                        alloc_flags, NULL, (int *) flush_profile_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Sets a flush profile
 */
static shr_error_e
dnx_field_flush_profile_set(
    int unit,
    uint32 flags,
    bcm_field_flush_profile_info_t * flush_profile_info,
    uint32 flush_profile_id,
    uint32 is_disable)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    uint32 entry_handle_id_21;
    uint32 ratio;
    uint32 instance;
    uint32 nof_supported_age_ratios;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    if (dnx_data_mdb.em.age_profile_per_ratio_support_get(unit))
    {
        table_id = DBAL_TABLE_MDB_20_EXEM_3;
        nof_supported_age_ratios = MDB_STEP_TABLE_NOF_ENTRY_SIZES;
        instance = INST_SINGLE;
    }
    else
    {
        table_id = DBAL_TABLE_MDB_19_EXEM_3;
        nof_supported_age_ratios = 1;
        instance = flush_profile_id;
    }

    /** allocate table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_21_EXEM_3, &entry_handle_id_21));


    for (ratio = 0; ratio < nof_supported_age_ratios; ratio++)
    {
        if (nof_supported_age_ratios == MDB_STEP_TABLE_NOF_ENTRY_SIZES)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0,
                                       (flush_profile_id * nof_supported_age_ratios) + ratio);
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, instance,
                                     flush_profile_info->high_threshold_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, instance,
                                     flush_profile_info->low_threshold_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, instance,
                                     flush_profile_info->maximal_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3, instance,
                                     flush_profile_info->increment_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_4, instance,
                                     flush_profile_info->decrement_value);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_5, instance,
                                     flush_profile_info->out_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_value_field32_set(unit, entry_handle_id_21, DBAL_FIELD_MDB_ITEM_0,
                                     (flush_profile_id * nof_supported_age_ratios) + ratio,
                                     flush_profile_info->init_value);
    }


    dbal_entry_value_field32_set(unit, entry_handle_id_21, DBAL_FIELD_MDB_ITEM_4, flush_profile_id, is_disable);
    dbal_entry_value_field32_set(unit, entry_handle_id_21, DBAL_FIELD_MDB_ITEM_5, flush_profile_id, is_disable);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_21, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_flush_profile_create(
    int unit,
    uint32 flags,
    bcm_field_flush_profile_info_t * flush_profile_info,
    uint32 *flush_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_profile_create_verify(unit, flags, flush_profile_info, flush_profile_id));

    /** Allocate flush-profile ID */
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_allocate(unit, flags, flush_profile_id));

    /** Set flush-profile HW */
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_set(unit, flags, flush_profile_info, *flush_profile_id, FALSE));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_field_flush_profile_get
 */
static shr_error_e
dnx_field_flush_profile_get_verify(
    int unit,
    uint32 flags,
    uint32 flush_profile_id,
    bcm_field_flush_profile_info_t * flush_profile_info)
{
    uint32 nof_flush_profiles;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(flush_profile_info, _SHR_E_PARAM, "flush_profile_info");

    /*
     * Check supported flags
     */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! Unsupported flag: flags = 0x%08x. No flag is supported! \n", flags);
    }

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }

    /*
     *   - check profile ID is in range
     *   - check profile ID is not already in use.
     */
    nof_flush_profiles = dnx_data_field.exem.small_nof_flush_profiles_get(unit);

    if (flush_profile_id >= nof_flush_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! The requested flush_profile_id (=%d) is out of range [0:%d]! \n",
                     flush_profile_id, nof_flush_profiles - 1);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_flush_profile_is_allocated(unit, flush_profile_id, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! The requested flush_group_id (=%d) was not allocated! \n", flush_profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets a flush profile
 */
static shr_error_e
dnx_field_flush_profile_get(
    int unit,
    uint32 flags,
    uint32 flush_profile_id,
    bcm_field_flush_profile_info_t * flush_profile_info)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    uint32 instance;
    uint32 nof_supported_age_ratios;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    if (dnx_data_mdb.em.age_profile_per_ratio_support_get(unit))
    {
        table_id = DBAL_TABLE_MDB_20_EXEM_3;
        nof_supported_age_ratios = MDB_STEP_TABLE_NOF_ENTRY_SIZES;
        instance = INST_SINGLE;
    }
    else
    {
        table_id = DBAL_TABLE_MDB_19_EXEM_3;
        nof_supported_age_ratios = 1;
        instance = flush_profile_id;
    }

    /** allocate table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    if (nof_supported_age_ratios == MDB_STEP_TABLE_NOF_ENTRY_SIZES)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0,
                                   (flush_profile_id * nof_supported_age_ratios));
    }

    /** get all fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, instance,
                                                        &(flush_profile_info->high_threshold_value)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, instance,
                                                        &(flush_profile_info->low_threshold_value)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, instance,
                                                        &(flush_profile_info->maximal_value)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3, instance,
                                                        &(flush_profile_info->increment_value)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_4, instance,
                                                        &(flush_profile_info->decrement_value)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_5, instance,
                                                        &(flush_profile_info->out_value)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_flush_profile_get(
    int unit,
    uint32 flags,
    uint32 flush_profile_id,
    bcm_field_flush_profile_info_t * flush_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_profile_get_verify(unit, flags, flush_profile_id, flush_profile_info));

    /** Get flush-profile HW */
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_get(unit, flags, flush_profile_id, flush_profile_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_field_flush_profile_destroy
 */
static shr_error_e
dnx_field_flush_profile_destroy_verify(
    int unit,
    uint32 flags,
    uint32 flush_profile_id)
{
    uint32 nof_flush_profiles;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check supported flags
     */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! Unsupported flag: flags = 0x%08x. No flag is supported! \n", flags);
    }

    /*
     *   - check profile ID is in range
     *   - check profile ID is not already in use.
     */
    nof_flush_profiles = dnx_data_field.exem.small_nof_flush_profiles_get(unit);

    if (flush_profile_id >= nof_flush_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! The requested flush_profile_id (=%d) is out of range [0:%d]! \n",
                     flush_profile_id, nof_flush_profiles - 1);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_flush_profile_is_allocated(unit, flush_profile_id, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! The requested flush_group_id (=%d) was not allocated! \n", flush_profile_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - deallocates a flush profile ID
 */
static shr_error_e
dnx_field_flush_profile_deallocate(
    int unit,
    uint32 flush_profile_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_field_info_sw.flush_profile_id.free_single(unit, (int) flush_profile_id));

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_flush_profile_destroy(
    int unit,
    uint32 flags,
    uint32 flush_profile_id)
{
    bcm_field_flush_profile_info_t flush_profile_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_profile_destroy_verify(unit, flags, flush_profile_id));

    /** Deallocate flush-profile ID */
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_deallocate(unit, flush_profile_id));

    /** Clear flush-profile HW */
    sal_memset(&flush_profile_info, 0, sizeof(bcm_field_flush_profile_info_t));
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_set(unit, flags, &flush_profile_info, flush_profile_id, TRUE));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_field_flush_profile_attach
 */
static shr_error_e
dnx_field_flush_profile_attach_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    uint32 flush_profile_id)
{
    uint32 nof_flush_profiles;
    uint8 is_allocated;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check supported flags
     */
    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! Unsupported flag: flags = 0x%08x. No flag is supported! \n", flags);
    }

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }

    /*
     * Verify flush_profile_id:
     *  - check it is in range. 
     *  - check it was created.
     */
    nof_flush_profiles = dnx_data_field.exem.small_nof_flush_profiles_get(unit);

    if (flush_profile_id >= nof_flush_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! The requested flush_profile_id (=%d) is out of range [0:%d]! \n",
                     flush_profile_id, nof_flush_profiles - 1);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_flush_profile_is_allocated(unit, flush_profile_id, &is_allocated));

    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error! The requested flush_group_id (=%d) was not allocated! \n", flush_profile_id);
    }

    /** Get FG type */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    /*
     * Verify FG type is EXEM3/4
     */
    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Wrong fg_id (=%d) -it's fg_type = %d is not DNX_FIELD_GROUP_TYPE_EXEM (=%d)! \n", fg_id,
                     fg_type, DNX_FIELD_GROUP_TYPE_EXEM);
    }

    /*
     * Verify That the age parameters fit into the field group's maximum age size.
     */
    {
        dbal_tables_e exem_dbal_table_id;
        dbal_table_mdb_access_info_t access_info = { 0 };
        dnx_field_key_template_t key_template;
        unsigned int payload_size;
        int app_db_id_size;
        dnx_field_app_db_id_t app_db_id;
        mdb_em_entry_encoding_e entry_encoding;
        int nof_age_bits;
        uint32 age_mask_inverted;
        bcm_field_flush_profile_info_t flush_profile_info;

        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &exem_dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                        (unit, exem_dbal_table_id, DBAL_ACCESS_METHOD_MDB, (void *) &access_info));
        SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
        SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));
        SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &app_db_id));
        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, access_info.physical_db_id[0], key_template.key_size_in_bits, payload_size, app_db_id,
                         app_db_id_size, &entry_encoding));
        SHR_IF_ERR_EXIT(mdb_init_em_age_nof_bits_get
                        (unit, access_info.physical_db_id[0], entry_encoding, &nof_age_bits));
        age_mask_inverted = SAL_FROM_BIT(nof_age_bits);
        SHR_IF_ERR_EXIT(dnx_field_flush_profile_get(unit, 0, flush_profile_id, &flush_profile_info));
        if ((flush_profile_info.high_threshold_value & age_mask_inverted) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! high_threshold_value (=%d) doesn not fit within maximum number "
                         "of age bits (=%d) for field group %d.\n",
                         flush_profile_info.high_threshold_value, nof_age_bits, fg_id);
        }
        if ((flush_profile_info.low_threshold_value & age_mask_inverted) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! low_threshold_value (=%d) doesn not fit within maximum number "
                         "of age bits (=%d) for field group %d.\n",
                         flush_profile_info.low_threshold_value, nof_age_bits, fg_id);
        }
        if ((flush_profile_info.increment_value & age_mask_inverted) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! increment_value (=%d) doesn not fit within maximum number "
                         "of age bits (=%d) for field group %d.\n",
                         flush_profile_info.increment_value, nof_age_bits, fg_id);
        }
        if ((flush_profile_info.decrement_value & age_mask_inverted) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! decrement_value (=%d) doesn not fit within maximum number "
                         "of age bits (=%d) for field group %d.\n",
                         flush_profile_info.decrement_value, nof_age_bits, fg_id);
        }
        if ((flush_profile_info.maximal_value & age_mask_inverted) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! maximal_value (=%d) doesn not fit within maximum number "
                         "of age bits (=%d) for field group %d.\n",
                         flush_profile_info.maximal_value, nof_age_bits, fg_id);
        }
        if ((flush_profile_info.out_value & age_mask_inverted) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error! out_value (=%d) doesn not fit within maximum number "
                         "of age bits (=%d) for field group %d.\n", flush_profile_info.out_value, nof_age_bits, fg_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Sets a flush profile mapping (app-db-id to flush-profile-id)
 */
static shr_error_e
dnx_field_flush_profile_map_set(
    int unit,
    uint32 app_db_id,
    uint32 flush_profile_id,
    dbal_physical_tables_e physical_db_id)
{
    dbal_tables_e cfg_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    if (physical_db_id == DBAL_PHYSICAL_TABLE_SEXEM_3)
    {
        cfg_table_id = DBAL_TABLE_MDB_24_EXEM_3;
    }
    else if (physical_db_id == DBAL_PHYSICAL_TABLE_LEXEM)
    {
        cfg_table_id = DBAL_TABLE_MDB_24_EXEM_4;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error! Wrong app_db_id (=%d) -it's physical_db_id = %d is not DBAL_PHYSICAL_TABLE_SEXEM_3 (=%d) or DBAL_PHYSICAL_TABLE_LEXEM (=%d)!\n",
                     app_db_id, physical_db_id, DBAL_PHYSICAL_TABLE_SEXEM_3, DBAL_PHYSICAL_TABLE_LEXEM);
    }

    /** allocate table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, cfg_table_id, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_4, /** instance */ app_db_id,
                                 flush_profile_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Sets flush profile learn mapping (flush-profile-id to app-db-id)
 */
static shr_error_e
dnx_field_flush_profile_learn_map_set(
    int unit,
    dnx_field_group_t fg_id,
    uint32 app_db_id,
    uint32 flush_profile_id,
    dbal_physical_tables_e physical_db_id)
{
    uint32 entry_handle_id;
    uint8 vmv_size, vmv_value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * There are 16 entries and 8 flush_profile_id.
     * Map pair of flush_profile_id - the MSB is don't care - {key (0,8), (1,9), ... (6,14), (7,15)} to same app_db_id.
     */
    /** get VMV value */
    SHR_IF_ERR_EXIT(dnx_field_group_vmv_get(unit, fg_id, &vmv_size, &vmv_value));

    /** allocate table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_SMALL_EXEM_LEARNING_INFO_MAP, &entry_handle_id));

    /** set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ID, flush_profile_id);

    /** set app-db-id */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, app_db_id);

    /** set VMV */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORMAT_VALUE, INST_SINGLE, vmv_value);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Gets flush profile learn mapping (app-db-id)
 */
static shr_error_e
dnx_field_flush_profile_learn_map_get(
    int unit,
    uint32 flush_profile_id,
    uint32 *app_db_id,
    uint8 *vmv_value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * There are 16 entries and 8 flush_profile_id.
     * Each pair of flush_profile_id - the MSB is don't care - {key (0,8), (1,9), ... (6,14), (7,15)} is mapped to same app_db_id..
     */

    /** allocate table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_PMF_SMALL_EXEM_LEARNING_INFO_MAP, &entry_handle_id));

    /** set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LEARN_ID, flush_profile_id);

    /** get all fields */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** get app-db-id */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, app_db_id));

    /** get VMV */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id, DBAL_FIELD_FORMAT_VALUE, INST_SINGLE, vmv_value));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_field_flush_profile_attach
 */
static shr_error_e
dnx_field_flush_profile_attach_app_db_id_verify(
    int unit,
    uint32 new_app_db_id,
    uint32 new_flush_profile_id)
{
    uint32 flush_profile_id;
    uint32 nof_flush_profiles;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the APP-ID is not already mapped to a flush profile.
     * Go over on all allocate flush profiles and compare it's APP-ID with the inputed APP-ID 
     */
    nof_flush_profiles = dnx_data_field.exem.small_nof_flush_profiles_get(unit);

    for (flush_profile_id = 0; flush_profile_id < nof_flush_profiles; flush_profile_id++)
    {
        if (flush_profile_id != new_flush_profile_id)
        {
            uint8 is_allocated;

            /** Check the profile is allocated */
            SHR_IF_ERR_EXIT(dnx_algo_field_flush_profile_is_allocated(unit, flush_profile_id, &is_allocated));

            if (is_allocated == TRUE)
            {
                uint32 app_db_id;
                uint8 vmv_value;

                /** get APP-DB-ID */
                SHR_IF_ERR_EXIT(dnx_field_flush_profile_learn_map_get(unit, flush_profile_id, &app_db_id, &vmv_value));

                if (app_db_id == new_app_db_id)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error! flush_profile_id (= %d) and (=%d) has same APP-DB-ID (=%d)! \n",
                                 new_flush_profile_id, flush_profile_id, app_db_id);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_flush_profile_attach(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    uint32 flush_profile_id)
{
    int app_db_id_size;
    dnx_field_app_db_id_t app_db_id;
    dbal_tables_e exem_dbal_table_id;
    dbal_table_mdb_access_info_t access_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_profile_attach_verify(unit, flags, fg_id, flush_profile_id));

    /** Get FG's APP-DB ID */
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &app_db_id));

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_profile_attach_app_db_id_verify(unit, app_db_id, flush_profile_id));

    /**Get physical table ID */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &exem_dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                    (unit, exem_dbal_table_id, DBAL_ACCESS_METHOD_MDB, (void *) &access_info));

    /** Set profile mapping: app-db to profile-id */
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_map_set(unit, app_db_id, flush_profile_id, access_info.physical_db_id[0]));

    /**  Set flush profile learn mapping (flush-profile-id to app-db-id) */
    SHR_IF_ERR_EXIT(dnx_field_flush_profile_learn_map_set
                    (unit, fg_id, app_db_id, flush_profile_id, access_info.physical_db_id[0]));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Stop the EXEM-3/4 age machine. 
 * Wait until the age machine is stopped. In case it takes too much time return an error.
 *
 * \param [in] unit - unit id
 * \param [in] exem_index     - exem index to configure.
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
dnx_field_flush_stop_age_machine(
    int unit,
    int exem_index)
{
    dbal_tables_e table_id;
    dbal_tables_e status_table_id;
    uint32 entry_handle_id;
    uint32 age_machine_status = DBAL_ENUM_FVAL_AGE_SCAN_STATUS_WORKING;
    uint32 nof_status_checks = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);


    if (exem_index == 0)
    {
        table_id = DBAL_TABLE_MDB_22_EXEM_3;
        status_table_id = DBAL_TABLE_MDB_33_EXEM_3;
    }
    else
    {
        table_id = DBAL_TABLE_MDB_22_EXEM_4;
        status_table_id = DBAL_TABLE_MDB_33_EXEM_4;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, status_table_id, entry_handle_id));

    /** Check if the age machine is still working. '0' means idle. */
    while (age_machine_status != DBAL_ENUM_FVAL_AGE_SCAN_STATUS_IDLE)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, &age_machine_status);
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
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error! Age machine did not stop in time! exem_index = %d.", exem_index);
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See header file for description
 */
shr_error_e
dnx_switch_control_exem_scan_period_set(
    int unit,
    int exem_index,
    uint32 scan_period_us)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    uint32 nof_100_clocks_between_metacycles;
    uint32 machine_auto_enable;
    dnxcmn_time_t time_for_100_clocks;
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }


    if (exem_index == 0)
    {
        table_id = DBAL_TABLE_MDB_22_EXEM_3;
    }
    else
    {
        table_id = DBAL_TABLE_MDB_22_EXEM_4;
    }

    /*
     * Calculate how many clocks are needed to configure age_seconds as the meta cycle.
     * Since each unit in the hardware is 100 clocks the calculation is done on:
     * scan_period [ns] = REG *100 * core_clk_time [ns]
     * ==>
     * REG = (1000 * scan_period [us]) / (100 * core_clk_time [ns]) 
     */
    time_for_100_clocks.time_units = DNXCMN_TIME_UNIT_NSEC;
    /*
     * Implement:
     *   time_for_100_clocks.time = (uint64) (scan_period_us * 10);
     */
    COMPILER_64_SET(work_reg_64, 0, scan_period_us);
    COMPILER_64_UMUL_32(work_reg_64, 10);
    COMPILER_64_COPY(time_for_100_clocks.time, work_reg_64);

    if (scan_period_us > 0)
    {
        machine_auto_enable = 1;
    }
    else
    {
        machine_auto_enable = 0;
    }

    SHR_IF_ERR_EXIT(dnxcmn_time_to_clock_cycles_get(unit, &time_for_100_clocks, &nof_100_clocks_between_metacycles));

    /** Age machine must be stopped before changing the age time */
    SHR_IF_ERR_EXIT(dnx_field_flush_stop_age_machine(unit, exem_index));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                 nof_100_clocks_between_metacycles);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, machine_auto_enable);

    if (dnx_data_mdb.em.scan_bank_participate_in_cuckoo_support_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See header file for description
 */
shr_error_e
dnx_switch_control_exem_scan_period_get(
    int unit,
    int exem_index,
    uint32 *scan_period_us)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    uint32 nof_100_clocks_between_metacycles;
    uint32 machine_auto_enable;
    dnxcmn_time_t time;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }


    if (exem_index == 0)
    {
        table_id = DBAL_TABLE_MDB_22_EXEM_3;
    }
    else
    {
        table_id = DBAL_TABLE_MDB_22_EXEM_4;
    }

    /** Get the number of clocks between two meta cycles */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0,
                                                        INST_SINGLE, &nof_100_clocks_between_metacycles));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1,
                                                        INST_SINGLE, &machine_auto_enable));

    SHR_IF_ERR_EXIT(dnxcmn_clock_cycles_to_time_get
                    (unit, nof_100_clocks_between_metacycles, DNXCMN_TIME_UNIT_NSEC, &time));

     /** Check if the age machine is enabled. In case the age machine is disabled return 0. */
    if (machine_auto_enable)
    {
        uint64 work_reg_64;

         /** Clock to time was used on units of 100 clocks. Need to update the time accordingly */
        COMPILER_64_COPY(work_reg_64, time.time);
        COMPILER_64_UDIV_32(work_reg_64, 10);
        *scan_period_us = COMPILER_64_LO(work_reg_64);
    }
    else
    {
        *scan_period_us = 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See header file for description
*/
shr_error_e
dnx_switch_control_exem_flush_profile_payload_offset_set(
    int unit,
    int exem_index,
    uint32 payload_offset)
{
    uint32 entry_handle_id;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (exem_index == 0)
    {
        field_id = DBAL_FIELD_ELEPHANT_TRAP_LEARN_PAYLOAD_START_BIT;
    }
    else
    {
        field_id = DBAL_FIELD_SLB_LEARN_PAYLOAD_START_BIT;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LBP_GLOBAL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, payload_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* See header file for description
*/
shr_error_e
dnx_switch_control_exem_flush_profile_payload_offset_get(
    int unit,
    int exem_index,
    uint32 *payload_offset)
{
    uint32 entry_handle_id;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (exem_index == 0)
    {
        field_id = DBAL_FIELD_ELEPHANT_TRAP_LEARN_PAYLOAD_START_BIT;
    }
    else
    {
        field_id = DBAL_FIELD_SLB_LEARN_PAYLOAD_START_BIT;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_LBP_GLOBAL, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, field_id, INST_SINGLE, payload_offset));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Write the flush entry to HW.
* \param [in] unit                       - Device ID.
* \param [in] is_large_exem              - If true LEXEM, if false sexem.
* \param [in] entry_id                   - The number of the entry to write to.
* \param [in] valid                      - The entry valid bit to write
* \param [in] hit_bit_value              - What qualify as hit bit
* \param [in] hit_bit_mask               - Whether to qualify the hit bit.
* \param [in] app_db_id_value            - What app_db_id to qualify upon.
* \param [in] app_db_id_mask             - The app_db_id mask.
* \param [in] command                    - What is to be done if the entry hits.
* \param [in] exem_raw_entry             - The full EXEM entry as it appears in the flush machine TCAM to qualify upon.
* \param [in] exem_raw_entry_mask        - The mask of exem_raw_entry.
* \param [in] exem_new_payload_raw       - The new value of the payload to be written.
* \param [in] exem_new_payload_raw_mask  - The bits in exem_new_payload_raw to be written.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_hw_set(
    int unit,
    int is_large_exem,
    dnx_field_flush_entry_id_t entry_id,
    uint8 valid,
    uint32 hit_bit_value,
    uint32 hit_bit_mask,
    uint32 app_db_id_value,
    uint32 app_db_id_mask,
    uint8 command,
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY])
{
    uint32 source_scan_bit;
    uint32 source_val[1];
    uint32 source_mask[1];
    uint32 source_mask_flipped[1];
    uint32 app_db_id_mask_flipped[1];
    uint32 hit_mask_flipped[1];
    uint32 exem_raw_entry_mask_flipped[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
    int nof_bits_source = dnx_data_field.exem_learn_flush_machine.nof_bits_source_get(unit);
    int nof_bits_hit = dnx_data_field.exem_learn_flush_machine.nof_bits_accessed_get(unit);
    int nof_bits_entry;
    int nof_bits_app_db_id_max;
    dbal_tables_e table_id_45;
    dbal_tables_e table_id_46;
    uint32 entry_handle_id_45;
    uint32 entry_handle_id_46;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(exem_raw_entry, _SHR_E_PARAM, "exem_raw_entry");
    SHR_NULL_CHECK(exem_raw_entry_mask, _SHR_E_PARAM, "exem_raw_entry_mask");
    SHR_NULL_CHECK(exem_new_payload_raw, _SHR_E_PARAM, "exem_new_payload_raw");
    SHR_NULL_CHECK(exem_new_payload_raw_mask, _SHR_E_PARAM, "exem_new_payload_raw_mask");


    if (is_large_exem)
    {
        table_id_45 = DBAL_TABLE_MDB_45_EXEM_4;
        table_id_46 = DBAL_TABLE_MDB_46_EXEM_4;
        nof_bits_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_LEXEM)->mdb_item_3_field_size;
        nof_bits_app_db_id_max = dnx_data_field.exem.large_app_db_id_size_get(unit);
    }
    else
    {
        table_id_45 = DBAL_TABLE_MDB_45_EXEM_3;
        table_id_46 = DBAL_TABLE_MDB_46_EXEM_3;
        nof_bits_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_3)->mdb_item_3_field_size;
        nof_bits_app_db_id_max = dnx_data_field.exem.small_app_db_id_size_get(unit);
    }

    source_scan_bit = dnx_data_field.exem_learn_flush_machine.source_bit_for_scan_get(unit);
    source_val[0] = SAL_BIT(source_scan_bit);
    source_mask[0] = SAL_BIT(source_scan_bit);

    source_mask_flipped[0] = source_mask[0];
    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(&(source_mask_flipped[0]), 0, nof_bits_source - 1));
    app_db_id_mask_flipped[0] = app_db_id_mask;
    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(&(app_db_id_mask_flipped[0]), 0, nof_bits_app_db_id_max - 1));
    hit_mask_flipped[0] = hit_bit_mask;
    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(&(hit_mask_flipped[0]), 0, nof_bits_hit - 1));
    sal_memcpy(exem_raw_entry_mask_flipped, exem_raw_entry_mask, sizeof(exem_raw_entry_mask_flipped));
    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(exem_raw_entry_mask_flipped, 0, nof_bits_entry - 1));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id_45, &entry_handle_id_45));
    dbal_entry_key_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_KEY_0, entry_id);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, valid);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_7, INST_SINGLE, exem_raw_entry);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE,
                                     exem_raw_entry_mask_flipped);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_8, INST_SINGLE, source_val[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_4, INST_SINGLE, source_mask_flipped[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_6, INST_SINGLE, app_db_id_value);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE,
                                 app_db_id_mask_flipped[0]);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE, hit_bit_value);
    dbal_entry_value_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, hit_mask_flipped[0]);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id_46, &entry_handle_id_46));
    dbal_entry_key_field32_set(unit, entry_handle_id_46, DBAL_FIELD_MDB_KEY_0, entry_id);
    dbal_entry_value_field32_set(unit, entry_handle_id_46, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, command);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id_46, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE,
                                     exem_new_payload_raw);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id_46, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE,
                                     exem_new_payload_raw_mask);


    if (valid)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_46, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_45, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_45, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id_46, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Write the flush entry to HW.
* \param [in] unit                        - Device ID.
* \param [in] is_large_exem               - If true LEXEM, if false sexem.
* \param [in] entry_id                    - The number of the entry to write to.
* \param [out] valid_p                    - The entry valid bit
* \param [out] hit_bit_value_p            - What qualify as hit bit
* \param [out] hit_bit_mask_p             - Whether to qualify the hit bit.
* \param [out] app_db_id_value_p          - What app_db_id to qualify upon.
* \param [out] app_db_id_mask_p           - The app_db_id mask.
* \param [out] command_p                  - What is to be done if the entry hits.
* \param [out] exem_raw_entry             - The full EXEM entry as it appears in the flush machine TCAM to qualify upon.
* \param [out] exem_raw_entry_mask        - The mask of exem_raw_entry.
* \param [out] exem_new_payload_raw       - The new value of the payload to be written.
* \param [out] exem_new_payload_raw_mask  - The bits in exem_new_payload_raw to be written.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_hw_get(
    int unit,
    int is_large_exem,
    dnx_field_flush_entry_id_t entry_id,
    uint8 *valid_p,
    uint8 *hit_bit_value_p,
    uint8 *hit_bit_mask_p,
    uint32 *app_db_id_value_p,
    uint32 *app_db_id_mask_p,
    uint8 *command_p,
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY])
{
    uint32 app_db_id_mask_flipped[1];
    uint32 hit_mask_flipped[1];
    uint32 exem_raw_entry_mask_flipped[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
    int nof_bits_hit = dnx_data_field.exem_learn_flush_machine.nof_bits_accessed_get(unit);
    int nof_bits_entry;
    int nof_bits_app_db_id_max;
    dbal_tables_e table_id_45;
    dbal_tables_e table_id_46;
    uint32 entry_handle_id_45;
    uint32 entry_handle_id_46;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(valid_p, _SHR_E_PARAM, "valid_p");
    SHR_NULL_CHECK(hit_bit_value_p, _SHR_E_PARAM, "hit_bit_value_p");
    SHR_NULL_CHECK(hit_bit_mask_p, _SHR_E_PARAM, "hit_bit_mask_p");
    SHR_NULL_CHECK(app_db_id_value_p, _SHR_E_PARAM, "app_db_id_value_p");
    SHR_NULL_CHECK(app_db_id_mask_p, _SHR_E_PARAM, "app_db_id_mask_p");
    SHR_NULL_CHECK(command_p, _SHR_E_PARAM, "command_p");
    SHR_NULL_CHECK(exem_raw_entry, _SHR_E_PARAM, "exem_raw_entry");
    SHR_NULL_CHECK(exem_raw_entry_mask, _SHR_E_PARAM, "exem_raw_entry_mask");
    SHR_NULL_CHECK(exem_new_payload_raw, _SHR_E_PARAM, "exem_new_payload_raw");
    SHR_NULL_CHECK(exem_new_payload_raw_mask, _SHR_E_PARAM, "exem_new_payload_raw_mask");

    sal_memset(exem_raw_entry, 0x0, sizeof(exem_raw_entry[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);
    sal_memset(exem_raw_entry_mask, 0x0,
               sizeof(exem_raw_entry_mask[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);
    sal_memset(exem_new_payload_raw, 0x0,
               sizeof(exem_new_payload_raw[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);
    sal_memset(exem_new_payload_raw_mask, 0x0,
               sizeof(exem_new_payload_raw_mask[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);


    if (is_large_exem)
    {
        table_id_45 = DBAL_TABLE_MDB_45_EXEM_4;
        table_id_46 = DBAL_TABLE_MDB_46_EXEM_4;
        nof_bits_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_LEXEM)->mdb_item_3_field_size;
        nof_bits_app_db_id_max = dnx_data_field.exem.large_app_db_id_size_get(unit);
    }
    else
    {
        table_id_45 = DBAL_TABLE_MDB_45_EXEM_3;
        table_id_46 = DBAL_TABLE_MDB_46_EXEM_3;
        nof_bits_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_3)->mdb_item_3_field_size;
        nof_bits_app_db_id_max = dnx_data_field.exem.small_app_db_id_size_get(unit);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id_45, &entry_handle_id_45));
    dbal_entry_key_field32_set(unit, entry_handle_id_45, DBAL_FIELD_MDB_KEY_0, entry_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_45, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id_46, &entry_handle_id_46));
    dbal_entry_key_field32_set(unit, entry_handle_id_46, DBAL_FIELD_MDB_KEY_0, entry_id);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_46, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, valid_p));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_7, INST_SINGLE, exem_raw_entry));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_3, INST_SINGLE, exem_raw_entry_mask_flipped));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_6, INST_SINGLE, app_db_id_value_p));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, &(app_db_id_mask_flipped[0])));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_6, INST_SINGLE, app_db_id_value_p));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, &(app_db_id_mask_flipped[0])));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_5, INST_SINGLE, hit_bit_value_p));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id_45, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, &(hit_mask_flipped[0])));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                    (unit, entry_handle_id_46, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE, command_p));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id_46, DBAL_FIELD_MDB_ITEM_2, INST_SINGLE, exem_new_payload_raw));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id_46, DBAL_FIELD_MDB_ITEM_1, INST_SINGLE, exem_new_payload_raw_mask));

    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(&(app_db_id_mask_flipped[0]), 0, nof_bits_app_db_id_max - 1));
    (*app_db_id_mask_p) = app_db_id_mask_flipped[0];
    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(&(hit_mask_flipped[0]), 0, nof_bits_hit - 1));
    (*hit_bit_mask_p) = hit_mask_flipped[0];
    SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(exem_raw_entry_mask_flipped, 0, nof_bits_entry - 1));
    sal_memcpy(exem_raw_entry_mask, exem_raw_entry_mask_flipped, sizeof(exem_raw_entry_mask_flipped));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Builds the exem entry to qualify upon the the flush entry.
* \param [in] unit                 - Device ID.
* \param [in] fg_id                - The EXEM field group for which to recreate the entry.
* \param [in] flush_entry_key_info_p - The parameters of the entry's key.
* \param [out] exem_raw_entry      - The full EXEM entry as it appears in the flush machine TCAM to qualify upon.
* \param [out] exem_raw_entry_mask - The mask of exem_raw_entry.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_entry_build_key_mask(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_key_t * flush_entry_key_info_p,
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY])
{
    uint32 exem_entry_key_value[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_key_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_payload_value[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_payload_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_app_db_id_value;
    uint32 exem_entry_vmv_value;
    uint32 exem_entry_vmv_mask;
    int is_lexem;
    int max_bits_in_entry;
    int qual_idx;
    int action_idx;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dnx_field_stage_e field_stage;
    dbal_fields_e dbal_field_id;
    dnx_field_key_template_t key_template;
    int key_size;
    unsigned int payload_size;
    int app_db_id_size;
    int vmv_size;
    dbal_entry_handle_t *entry_handle = NULL;
    dbal_table_field_info_t *dbal_table_field_info;
    int field_index_in_table;
    dbal_fields_e parent_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_key_info_p, _SHR_E_PARAM, "flush_entry_key_info_p");
    SHR_NULL_CHECK(exem_raw_entry, _SHR_E_PARAM, "exem_raw_entry");
    SHR_NULL_CHECK(exem_raw_entry_mask, _SHR_E_PARAM, "exem_raw_entry_mask");

    

    sal_memset(exem_raw_entry, 0x0, sizeof(exem_raw_entry[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);
    sal_memset(exem_raw_entry_mask, 0x0,
               sizeof(exem_raw_entry[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);

    /*
     * Get the information about the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
    if (is_lexem)
    {
        max_bits_in_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_LEXEM)->mdb_item_3_field_size;
    }
    else
    {
        max_bits_in_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_3)->mdb_item_3_field_size;
    }
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    key_size = key_template.key_size_in_bits;
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));

    /*
     * Sanity check.
     */
    if (max_bits_in_entry > DNX_DATA_MAX_FIELD_EXEM_LEARN_FLUSH_MACHINE_LEXEM_ENTRY_MAX_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "max_bits_in_entry (%d) larger than %d.\n",
                     max_bits_in_entry, DNX_DATA_MAX_FIELD_EXEM_LEARN_FLUSH_MACHINE_LEXEM_ENTRY_MAX_SIZE);
    }

    /*
     * Get the app_db_id and vmv.
     */
    {
        uint8 vmv_uint8;
        uint8 vmv_size_uint8;
        dnx_field_app_db_id_t app_db_id_read;
        SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &app_db_id_read));
        exem_entry_app_db_id_value = app_db_id_read;
        SHR_IF_ERR_EXIT(dnx_field_group_vmv_get(unit, fg_id, &vmv_size_uint8, &vmv_uint8));
        exem_entry_vmv_value = vmv_uint8;
        exem_entry_vmv_mask = 0;
        vmv_size = vmv_size_uint8;
    }

    /*
     * Sanity check: Verify that the sizes fit within the entry.
     */
    if ((app_db_id_size > max_bits_in_entry) || (key_size > max_bits_in_entry) ||
        (payload_size > max_bits_in_entry) || (vmv_size > max_bits_in_entry))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size of app_db_id (%d), key (%d), payload (%d) or vmv (%d) "
                     "is above the max entry size (%d).\n",
                     app_db_id_size, key_size, payload_size, vmv_size, max_bits_in_entry);
    }

    /*
     * Create the DBAL entry to get the key and payload of the EXEM entry upon which we qualify.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (flush_entry_key_info_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       flush_entry_key_info_p->qual_info[qual_idx].dnx_qual,
                                                       &dbal_field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, dbal_field_id,
                                       flush_entry_key_info_p->qual_info[qual_idx].qual_value);
        /** Set the mask. */
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal
                        (unit, entry_handle->table_id, dbal_field_id, TRUE, 0, INST_SINGLE, &dbal_table_field_info,
                         &field_index_in_table, &parent_field_id));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (flush_entry_key_info_p->qual_info[qual_idx].qual_mask,
                         dbal_table_field_info->bits_offset_in_buffer, dbal_table_field_info->field_nof_bits,
                         exem_entry_key_mask));

    }
    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && flush_entry_key_info_p->action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, flush_entry_key_info_p->action_info[action_idx].dnx_action,
                         &dbal_field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                                         flush_entry_key_info_p->action_info[action_idx].action_value);
        /** Set mask and valid bit. */
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, dbal_field_id, FALSE,
                                                            entry_handle->cur_res_type, INST_SINGLE,
                                                            &dbal_table_field_info, &field_index_in_table,
                                                            &parent_field_id));
        if (SHR_BITGET(dbal_table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
        {
            uint32 mask_shifted_for_valid_bit[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY] = { 0 };
            SHR_IF_ERR_EXIT(dbal_fields_encode_valid_ind
                            (unit, flush_entry_key_info_p->action_info[action_idx].action_mask,
                             dbal_table_field_info->field_nof_bits, mask_shifted_for_valid_bit));
            mask_shifted_for_valid_bit[0] &= (~1);
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (mask_shifted_for_valid_bit, dbal_table_field_info->bits_offset_in_buffer,
                             dbal_table_field_info->field_nof_bits, exem_entry_payload_mask));
            /** Set valid bit in key and mask. */
            if (flush_entry_key_info_p->action_info[action_idx].valid_bit_mask)
            {
                uint32 valid_bit_value = flush_entry_key_info_p->action_info[action_idx].valid_bit_value;
                uint32 valid_bit_mask = flush_entry_key_info_p->action_info[action_idx].valid_bit_mask;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&valid_bit_value, dbal_table_field_info->bits_offset_in_buffer, 1,
                                 &(entry_handle->phy_entry.payload[0])));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&valid_bit_mask, dbal_table_field_info->bits_offset_in_buffer, 1,
                                 exem_entry_payload_mask));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (flush_entry_key_info_p->action_info[action_idx].action_mask,
                             dbal_table_field_info->bits_offset_in_buffer, dbal_table_field_info->field_nof_bits,
                             exem_entry_payload_mask));
        }
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (&(entry_handle->phy_entry.key[0]), 0, key_size, exem_entry_key_value));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (&(entry_handle->phy_entry.payload[0]), 0, payload_size, exem_entry_payload_value));

    /*
     * Sanity check: Verify that the key, and payload, including masks, do not exceed the size of the key/payload.
     */
    if (utilex_bitstream_have_one_in_range(exem_entry_key_mask, key_size, max_bits_in_entry - 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bits written to key mask outside key size of %d.\n", key_size);
    }
    if (utilex_bitstream_have_one_in_range(exem_entry_payload_mask, payload_size, max_bits_in_entry - 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bits written to key payload mask outside payload size of %d.\n", payload_size);
    }

    /*
     * Build the entry.
     */
    {
        soc_reg_above_64_val_t mdb_entry;
        soc_reg_above_64_val_t mdb_entry_mask;
        soc_mem_t exem_memory;
        dbal_table_mdb_access_info_t dbal_mdb_access_info;
        dbal_physical_tables_e exem_physical_table;
        mdb_em_entry_encoding_e entry_encoding;
        int entry_offset;
        int vmv_aligned_offset;
        int exem_row_size;

        SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                        (unit, dbal_table_id, DBAL_ACCESS_METHOD_MDB, &dbal_mdb_access_info));
        /** sanity check. */
        if (dbal_mdb_access_info.nof_physical_tables != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d does not have one DBAL physical tables. It has %d.\n",
                         fg_id, dbal_mdb_access_info.nof_physical_tables);
        }
        exem_physical_table = dbal_mdb_access_info.physical_db_id[0];

        exem_memory = dnx_data_mdb.em.em_info_get(unit, exem_physical_table)->em_interface;

        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, exem_physical_table, key_size, payload_size, exem_entry_app_db_id_value, app_db_id_size,
                         &entry_encoding));

        
        entry_offset = 0;
        sal_memset(mdb_entry, 0x0, sizeof(mdb_entry));
        SHR_IF_ERR_EXIT(mdb_em_prepare_entry(unit, exem_physical_table, exem_entry_app_db_id_value, exem_memory,
                                             exem_entry_key_value, key_size, exem_entry_payload_value, payload_size,
                                             entry_encoding, entry_offset, exem_raw_entry));

        sal_memset(mdb_entry_mask, 0x0, sizeof(mdb_entry_mask));
        SHR_IF_ERR_EXIT(mdb_em_prepare_entry(unit, exem_physical_table, exem_entry_app_db_id_value, exem_memory,
                                             exem_entry_key_mask, key_size, exem_entry_payload_mask, payload_size,
                                             entry_encoding, entry_offset, exem_raw_entry_mask));
        exem_row_size = dnx_data_mdb.pdbs.pdb_info_get(unit, exem_physical_table)->row_width;
        vmv_aligned_offset = max_bits_in_entry - exem_row_size + (exem_row_size / (1 << entry_encoding)) - vmv_size;
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (&exem_entry_vmv_value, vmv_aligned_offset, vmv_size, exem_raw_entry));
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (&exem_entry_vmv_mask, vmv_aligned_offset, vmv_size, exem_raw_entry_mask));
    }

    /*
     * Only have a key where we have mask.
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_and
                    (exem_raw_entry, exem_raw_entry_mask, DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Builds the payload to be changed, mask included.
* \param [in] unit                       - Device ID.
* \param [in] fg_id                      - The EXEM field group for which to recreate the entry.
* \param [in] flush_entry_payload_info_p - The parameters of the entry's payload.
* \param [out] exem_new_payload_raw      - The new value of the payload to be written.
* \param [out] exem_new_payload_raw_mask - The bits in exem_new_payload_raw to be written.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_payload_build(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_payload_t * flush_entry_payload_info_p,
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY])
{
    uint32 exem_entry_payload_value[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_payload_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    int is_lexem;
    int max_bits_in_entry;
    int payload_position;
    int vmv_position;
    int action_idx;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dnx_field_stage_e field_stage;
    dbal_fields_e dbal_field_id;
    unsigned int payload_size;
    int vmv_size;
    dbal_entry_handle_t *entry_handle = NULL;
    dbal_table_field_info_t *dbal_table_field_info;
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    int max_payload_size_in_hw;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_payload_info_p, _SHR_E_PARAM, "flush_entry_payload_info_p");
    SHR_NULL_CHECK(exem_new_payload_raw, _SHR_E_PARAM, "exem_new_payload_raw");
    SHR_NULL_CHECK(exem_new_payload_raw_mask, _SHR_E_PARAM, "exem_new_payload_raw_mask");

    sal_memset(exem_new_payload_raw, 0x0,
               sizeof(exem_new_payload_raw[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);
    sal_memset(exem_new_payload_raw_mask, 0x0,
               sizeof(exem_new_payload_raw_mask[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);

    /*
     * Get the information about the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
    if (is_lexem)
    {
        max_bits_in_entry = dnx_data_field.exem_learn_flush_machine.lexem_entry_max_size_get(unit);
        max_payload_size_in_hw = dnx_data_field.exem.large_max_result_size_get(unit);
    }
    else
    {
        max_bits_in_entry = dnx_data_field.exem_learn_flush_machine.sexem_entry_max_size_get(unit);
        max_payload_size_in_hw = dnx_data_field.exem.small_max_result_size_get(unit);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));

    /*
     * Get the VMV size. We do not change the VMV value.
     */
    {
        uint8 vmv_uint8;
        uint8 vmv_size_uint8;
        SHR_IF_ERR_EXIT(dnx_field_group_vmv_get(unit, fg_id, &vmv_size_uint8, &vmv_uint8));
        vmv_size = vmv_size_uint8;
    }

    /*
     * Sanity check: Verify that the sum of the payload and VMV fits within the entry.
     */
    if ((payload_size + vmv_size) > max_bits_in_entry)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Sum of the sizes of payload (%d) and vmv (%d) "
                     "is above the entry size (%d).\n", payload_size, vmv_size, max_bits_in_entry);
    }
    if ((payload_size + vmv_size) > max_payload_size_in_hw)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Sum of the sizes of payload (%d) and vmv (%d) "
                     "is above the payload size (%d).\n", payload_size, vmv_size, max_payload_size_in_hw);
    }

    /*
     * Create the DBAL entry to get the new payload of the EXEM entry.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && flush_entry_payload_info_p->action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, flush_entry_payload_info_p->action_info[action_idx].dnx_action,
                         &dbal_field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                                         flush_entry_payload_info_p->action_info[action_idx].action_value);
        /** Set mask and valid bit. */
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, dbal_field_id, FALSE,
                                                            entry_handle->cur_res_type, INST_SINGLE,
                                                            &dbal_table_field_info, &field_index_in_table,
                                                            &parent_field_id));
        if (SHR_BITGET(dbal_table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
        {
            uint32 mask_shifted_for_valid_bit[DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY] = { 0 };
            SHR_IF_ERR_EXIT(dbal_fields_encode_valid_ind
                            (unit, flush_entry_payload_info_p->action_info[action_idx].action_mask,
                             dbal_table_field_info->field_nof_bits, mask_shifted_for_valid_bit));
            mask_shifted_for_valid_bit[0] &= (~1);
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (mask_shifted_for_valid_bit, dbal_table_field_info->bits_offset_in_buffer,
                             dbal_table_field_info->field_nof_bits, exem_entry_payload_mask));
            /** Set valid bit in key and mask. */
            if (flush_entry_payload_info_p->action_info[action_idx].valid_bit_mask)
            {
                uint32 valid_bit_value = flush_entry_payload_info_p->action_info[action_idx].valid_bit_value;
                uint32 valid_bit_mask = flush_entry_payload_info_p->action_info[action_idx].valid_bit_mask;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&valid_bit_value, dbal_table_field_info->bits_offset_in_buffer, 1,
                                 &(entry_handle->phy_entry.payload[0])));
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (&valid_bit_mask, dbal_table_field_info->bits_offset_in_buffer, 1,
                                 exem_entry_payload_mask));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (flush_entry_payload_info_p->action_info[action_idx].action_mask,
                             dbal_table_field_info->bits_offset_in_buffer, dbal_table_field_info->field_nof_bits,
                             exem_entry_payload_mask));
        }
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (&(entry_handle->phy_entry.payload[0]), 0, payload_size, exem_entry_payload_value));

    /*
     * Sanity check: Verify that the payload and mask, do not exceed the size of the key/payload.
     */
    if (utilex_bitstream_have_one_in_range(exem_entry_payload_value, payload_size, max_bits_in_entry - 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bits written to key payload outside payload size of %d.\n", payload_size);
    }
    if (utilex_bitstream_have_one_in_range(exem_entry_payload_mask, payload_size, max_bits_in_entry - 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Bits written to key payload mask outside payload size of %d.\n", payload_size);
    }

    /*
     * position the payload.
     */
    vmv_position = max_payload_size_in_hw - vmv_size;
    payload_position = vmv_position - payload_size;

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (exem_entry_payload_value, payload_position, payload_size, exem_new_payload_raw));

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (exem_entry_payload_mask, payload_position, payload_size, exem_new_payload_raw_mask));

    /*
     * Only have a key where we have mask.
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_and
                    (exem_new_payload_raw, exem_new_payload_raw_mask,
                     DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the flush entry key from the EXEM entry it qualifies upon.
*  Builds the exem entry to qualify upon the the flush entry.
* \param [in] unit                 - Device ID.
* \param [in] fg_id                - The EXEM field group for which to recreate the entry.
* \param [in] exem_raw_entry       - The full EXEM entry as it appears in the flush machine TCAM to qualify upon.
* \param [in] exem_raw_entry_mask  - The mask of exem_raw_entry.
* \param [out] flush_entry_key_info_p - The parameters of the entry's key.
* 
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_entry_analyze_key_mask(
    int unit,
    dnx_field_group_t fg_id,
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    dnx_field_flush_entry_key_t * flush_entry_key_info_p)
{
    uint32 exem_entry_key_value[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_key_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_payload_value[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_payload_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_app_db_id_value;
    int is_lexem;
    int max_bits_in_entry;
    int entry_qual_idx;
    int fg_qual_idx;
    int fg_action_idx;
    int entry_action_idx;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dnx_field_stage_e field_stage;
    dbal_fields_e dbal_field_id;
    dnx_field_key_template_t key_template;
    int key_size;
    unsigned int payload_size;
    int app_db_id_size;
    int vmv_size;
    dbal_entry_handle_t *entry_handle = NULL;
    dbal_table_field_info_t *dbal_table_field_info;
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dnx_field_actions_fg_payload_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_key_info_p, _SHR_E_PARAM, "flush_entry_key_info_p");
    SHR_NULL_CHECK(exem_raw_entry, _SHR_E_PARAM, "exem_raw_entry");
    SHR_NULL_CHECK(exem_raw_entry_mask, _SHR_E_PARAM, "exem_raw_entry_mask");

    /*
     * Get the information about the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
    if (is_lexem)
    {
        max_bits_in_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_LEXEM)->mdb_item_3_field_size;
    }
    else
    {
        max_bits_in_entry = dnx_data_mdb.em.mdb_45_info_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_3)->mdb_item_3_field_size;
    }
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    key_size = key_template.key_size_in_bits;
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));

    /*
     * Sanity check.
     */
    if (max_bits_in_entry > DNX_DATA_MAX_FIELD_EXEM_LEARN_FLUSH_MACHINE_LEXEM_ENTRY_MAX_SIZE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "max_bits_in_entry (%d) larger than %d.\n",
                     max_bits_in_entry, DNX_DATA_MAX_FIELD_EXEM_LEARN_FLUSH_MACHINE_LEXEM_ENTRY_MAX_SIZE);
    }

    /*
     * Get the app_db_id and vmv.
     */
    {
        uint8 vmv_uint8;
        uint8 vmv_size_uint8;
        dnx_field_app_db_id_t app_db_id_read;
        SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &app_db_id_read));
        exem_entry_app_db_id_value = app_db_id_read;
        SHR_IF_ERR_EXIT(dnx_field_group_vmv_get(unit, fg_id, &vmv_size_uint8, &vmv_uint8));
        vmv_size = vmv_size_uint8;
    }

    /*
     * Sanity check: Verify that the sizes fit within the entry.
     */
    if ((app_db_id_size > max_bits_in_entry) || (key_size > max_bits_in_entry) ||
        (payload_size > max_bits_in_entry) || (vmv_size > max_bits_in_entry))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Size of app_db_id (%d), key (%d), payload (%d) or vmv (%d) "
                     "is above the max entry size (%d).\n",
                     app_db_id_size, key_size, payload_size, vmv_size, max_bits_in_entry);
    }

    /*
     * Parse the entry.
     */
    {
        soc_mem_t exem_memory;
        dbal_table_mdb_access_info_t dbal_mdb_access_info;
        dbal_physical_tables_e exem_physical_table;
        mdb_em_entry_encoding_e entry_encoding;
        int entry_offset;
        uint32 entry_size;
        uint32 temp_total_size;
        uint32 zero_buffer;
        soc_field_info_t *field_info;

        SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                        (unit, dbal_table_id, DBAL_ACCESS_METHOD_MDB, &dbal_mdb_access_info));
        /** sanity check. */
        if (dbal_mdb_access_info.nof_physical_tables != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d does not have one DBAL physical tables. It has %d.\n",
                         fg_id, dbal_mdb_access_info.nof_physical_tables);
        }
        exem_physical_table = dbal_mdb_access_info.physical_db_id[0];

        exem_memory = dnx_data_mdb.em.em_info_get(unit, exem_physical_table)->em_interface;

        SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                        (unit, exem_physical_table, key_size, payload_size, exem_entry_app_db_id_value, app_db_id_size,
                         &entry_encoding));

        entry_offset = 0;

        SOC_FIND_FIELD(ENTRYf, SOC_MEM_INFO(unit, exem_memory).fields, SOC_MEM_INFO(unit, exem_memory).nFields,
                       field_info);
        entry_size = field_info->len;

        if (key_size + payload_size > entry_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_size(%d) + payload_size(%d) > entry_size(%d).\n",
                         key_size, payload_size, entry_size);
        }

        temp_total_size = dnx_data_mdb.pdbs.pdb_info_get(unit, exem_physical_table)->row_width / (1 << entry_encoding);

        zero_buffer = dnx_data_mdb.pdbs.pdb_info_get(unit, exem_physical_table)->row_width - temp_total_size;

        if (key_size + payload_size + zero_buffer + vmv_size > entry_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "key_size(%d) + payload_size(%d) + zero_buffer(%d) + vmv_size(%d) > entry_size(%d).\n",
                         key_size, payload_size, zero_buffer, vmv_size, entry_size);
        }

        sal_memset(exem_entry_key_value, 0x0,
                   sizeof(exem_entry_key_value[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);
        sal_memset(exem_entry_key_mask, 0x0,
                   sizeof(exem_entry_key_mask[0]) * DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY);

        SHR_BITCOPY_RANGE(exem_entry_payload_value, 0, exem_raw_entry,
                          entry_offset + entry_size - zero_buffer - vmv_size - payload_size, payload_size);
        SHR_BITCOPY_RANGE(exem_entry_key_value, 0, exem_raw_entry, entry_offset, key_size);
        SHR_BITCOPY_RANGE(exem_entry_payload_mask, 0, exem_raw_entry_mask,
                          entry_offset + entry_size - zero_buffer - vmv_size - payload_size, payload_size);
        SHR_BITCOPY_RANGE(exem_entry_key_mask, 0, exem_raw_entry_mask, entry_offset, key_size);
    }

    /*
     * Create the DBAL entry to get the key and payload of the EXEM entry upon which we qualify.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    entry_qual_idx = 0;
    for (fg_qual_idx = 0;
         (fg_qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[fg_qual_idx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); fg_qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       key_template.key_qual_map[fg_qual_idx].qual_type,
                                                       &dbal_field_id));
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal
                        (unit, entry_handle->table_id, dbal_field_id, TRUE, 0, INST_SINGLE, &dbal_table_field_info,
                         &field_index_in_table, &parent_field_id));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, dbal_table_field_info, dbal_field_id, exem_entry_key_value,
                         flush_entry_key_info_p->qual_info[entry_qual_idx].qual_value));
        SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                        (unit, dbal_table_field_info, dbal_field_id, exem_entry_key_mask,
                         flush_entry_key_info_p->qual_info[entry_qual_idx].qual_mask));
        if (!
            (utilex_bitstream_have_one
             (flush_entry_key_info_p->qual_info[entry_qual_idx].qual_mask,
              sizeof(flush_entry_key_info_p->qual_info[0].qual_mask) /
              sizeof(flush_entry_key_info_p->qual_info[0].qual_mask[0]))))
        {
            continue;
        }
        flush_entry_key_info_p->qual_info[entry_qual_idx].dnx_qual = key_template.key_qual_map[fg_qual_idx].qual_type;
        entry_qual_idx++;
    }
    entry_action_idx = 0;
    for (fg_action_idx = 0;
         fg_action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_payload_info.actions_on_payload_info[fg_action_idx].dnx_action != DNX_FIELD_ACTION_INVALID;
         fg_action_idx++)
    {
        int valid_bit_shift;
        uint32 val_after_valid_ind[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 mask_after_valid_ind[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 valid_bit_value_uint32[1] = { 0 };
        uint32 valid_bit_mask_uint32[1] = { 0 };

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, actions_payload_info.actions_on_payload_info[fg_action_idx].dnx_action,
                         &dbal_field_id));
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, dbal_field_id, FALSE,
                                                            entry_handle->cur_res_type, INST_SINGLE,
                                                            &dbal_table_field_info, &field_index_in_table,
                                                            &parent_field_id));

        if (SHR_BITGET(dbal_table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
        {
            valid_bit_shift = 1;

            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (exem_entry_payload_value, dbal_table_field_info->bits_offset_in_buffer, 1,
                             valid_bit_value_uint32));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (exem_entry_payload_mask, dbal_table_field_info->bits_offset_in_buffer, 1,
                             valid_bit_mask_uint32));

        }
        else
        {
            valid_bit_shift = 0;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (exem_entry_payload_value, dbal_table_field_info->bits_offset_in_buffer + valid_bit_shift,
                         dbal_table_field_info->field_nof_bits - valid_bit_shift, val_after_valid_ind));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (exem_entry_payload_mask, dbal_table_field_info->bits_offset_in_buffer + valid_bit_shift,
                         dbal_table_field_info->field_nof_bits - valid_bit_shift, mask_after_valid_ind));

        if ((valid_bit_mask_uint32[0] == 0)
            &&
            (!(utilex_bitstream_have_one
               (mask_after_valid_ind, sizeof(mask_after_valid_ind) / sizeof(mask_after_valid_ind[0])))))
        {
            continue;
        }

        flush_entry_key_info_p->action_info[entry_action_idx].dnx_action =
            actions_payload_info.actions_on_payload_info[fg_action_idx].dnx_action;

        flush_entry_key_info_p->action_info[entry_action_idx].valid_bit_value = valid_bit_value_uint32[0];
        flush_entry_key_info_p->action_info[entry_action_idx].valid_bit_mask = valid_bit_mask_uint32[0];

        if (SHR_BITGET(dbal_table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_decode
                            (unit, dbal_field_id, parent_field_id, dbal_table_field_info->field_nof_bits,
                             val_after_valid_ind, flush_entry_key_info_p->action_info[entry_action_idx].action_value));
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (val_after_valid_ind, 0, dbal_table_field_info->field_nof_bits - valid_bit_shift,
                             flush_entry_key_info_p->action_info[entry_action_idx].action_value));
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (mask_after_valid_ind, 0, dbal_table_field_info->field_nof_bits - valid_bit_shift,
                         flush_entry_key_info_p->action_info[entry_action_idx].action_mask));
        entry_action_idx++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Builds the payload to be changed, mask included.
* \param [in] unit                       - Device ID.
* \param [in] fg_id                      - The EXEM field group for which to recreate the entry.
* \param [in] exem_new_payload_raw       - The new value of the payload to be written.
* \param [in] exem_new_payload_raw_mask  - The bits in exem_new_payload_raw to be written.
* \param [in,out] flush_entry_payload_info_p - The parameters of the flush entry's payload.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_payload_analyze(
    int unit,
    dnx_field_group_t fg_id,
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY],
    dnx_field_flush_entry_payload_t * flush_entry_payload_info_p)
{
    uint32 exem_entry_payload_value[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_entry_payload_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    int is_lexem;
    int max_bits_in_entry;
    int fg_action_idx;
    int entry_action_idx;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dnx_field_stage_e field_stage;
    dbal_fields_e dbal_field_id;
    unsigned int payload_size;
    int vmv_size;
    dbal_entry_handle_t *entry_handle = NULL;
    dbal_table_field_info_t *dbal_table_field_info;
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    int max_payload_size_in_hw;
    int payload_position;
    int vmv_position;
    dnx_field_actions_fg_payload_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_payload_info_p, _SHR_E_PARAM, "flush_entry_payload_info_p");
    SHR_NULL_CHECK(exem_new_payload_raw, _SHR_E_PARAM, "exem_new_payload_raw");
    SHR_NULL_CHECK(exem_new_payload_raw_mask, _SHR_E_PARAM, "exem_new_payload_raw_mask");

    /*
     * Get the information about the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
    if (is_lexem)
    {
        max_bits_in_entry = dnx_data_field.exem_learn_flush_machine.lexem_entry_max_size_get(unit);
        max_payload_size_in_hw = dnx_data_field.exem.large_max_result_size_get(unit);
    }
    else
    {
        max_bits_in_entry = dnx_data_field.exem_learn_flush_machine.sexem_entry_max_size_get(unit);
        max_payload_size_in_hw = dnx_data_field.exem.small_max_result_size_get(unit);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));

    /*
     * Get the VMV size. We do not change the VMV value.
     */
    {
        uint8 vmv_uint8;
        uint8 vmv_size_uint8;
        SHR_IF_ERR_EXIT(dnx_field_group_vmv_get(unit, fg_id, &vmv_size_uint8, &vmv_uint8));
        vmv_size = vmv_size_uint8;
    }

    /*
     * Sanity check: Verify that the sum of the payload and VMV fits within the entry.
     */
    if ((payload_size + vmv_size) > max_bits_in_entry)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Sum of the sizes of payload (%d) and vmv (%d) "
                     "is above the entry size (%d).\n", payload_size, vmv_size, max_bits_in_entry);
    }
    if ((payload_size + vmv_size) > max_payload_size_in_hw)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Sum of the sizes of payload (%d) and vmv (%d) "
                     "is above the payload size (%d).\n", payload_size, vmv_size, max_payload_size_in_hw);
    }

    /*
     * Collect the payload.
     */
    vmv_position = max_payload_size_in_hw - vmv_size;
    payload_position = vmv_position - payload_size;

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (exem_new_payload_raw, payload_position, payload_size, exem_entry_payload_value));

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (exem_new_payload_raw_mask, payload_position, payload_size, exem_entry_payload_mask));

    /*
     * Create the DBAL entry to get the key and payload of the EXEM entry upon which we qualify.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    entry_action_idx = 0;
    for (fg_action_idx = 0;
         fg_action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_payload_info.actions_on_payload_info[fg_action_idx].dnx_action != DNX_FIELD_ACTION_INVALID;
         fg_action_idx++)
    {
        int valid_bit_shift;
        uint32 val_after_valid_ind[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 mask_after_valid_ind[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 valid_bit_value_uint32[1] = { 0 };
        uint32 valid_bit_mask_uint32[1] = { 0 };

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, actions_payload_info.actions_on_payload_info[fg_action_idx].dnx_action,
                         &dbal_field_id));
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, dbal_field_id, FALSE,
                                                            entry_handle->cur_res_type, INST_SINGLE,
                                                            &dbal_table_field_info, &field_index_in_table,
                                                            &parent_field_id));

        if (SHR_BITGET(dbal_table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
        {
            valid_bit_shift = 1;

            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (exem_entry_payload_value, dbal_table_field_info->bits_offset_in_buffer, 1,
                             valid_bit_value_uint32));
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (exem_entry_payload_mask, dbal_table_field_info->bits_offset_in_buffer, 1,
                             valid_bit_mask_uint32));

        }
        else
        {
            valid_bit_shift = 0;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (exem_entry_payload_value, dbal_table_field_info->bits_offset_in_buffer + valid_bit_shift,
                         dbal_table_field_info->field_nof_bits - valid_bit_shift, val_after_valid_ind));
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (exem_entry_payload_mask, dbal_table_field_info->bits_offset_in_buffer + valid_bit_shift,
                         dbal_table_field_info->field_nof_bits - valid_bit_shift, mask_after_valid_ind));

        if ((valid_bit_mask_uint32[0] == 0)
            &&
            (!(utilex_bitstream_have_one
               (mask_after_valid_ind, sizeof(mask_after_valid_ind) / sizeof(mask_after_valid_ind[0])))))
        {
            continue;
        }

        flush_entry_payload_info_p->action_info[entry_action_idx].dnx_action =
            actions_payload_info.actions_on_payload_info[fg_action_idx].dnx_action;

        flush_entry_payload_info_p->action_info[entry_action_idx].valid_bit_value = valid_bit_value_uint32[0];
        flush_entry_payload_info_p->action_info[entry_action_idx].valid_bit_mask = valid_bit_mask_uint32[0];

        if (SHR_BITGET(dbal_table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_decode
                            (unit, dbal_field_id, parent_field_id, dbal_table_field_info->field_nof_bits,
                             val_after_valid_ind,
                             flush_entry_payload_info_p->action_info[entry_action_idx].action_value));
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                            (val_after_valid_ind, 0, dbal_table_field_info->field_nof_bits - valid_bit_shift,
                             flush_entry_payload_info_p->action_info[entry_action_idx].action_value));
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (mask_after_valid_ind, 0, dbal_table_field_info->field_nof_bits - valid_bit_shift,
                         flush_entry_payload_info_p->action_info[entry_action_idx].action_mask));
        entry_action_idx++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_flush_machine_entry_add().
* \param [in] unit                - Device ID.
* \param [in] flags               - The flags for the entry add.
* \param [in] fg_id               - The EXEM field group upon which we qualify.
* \param [in] entry_id            - The number of the entry in the TCAM.
* \param [in] flush_entry_info_p  - The parameters of the entry to be added.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_entry_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    dnx_field_flush_entry_t * flush_entry_info_p)
{
    uint8 is_allocated;
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e field_stage;
    dnx_field_key_template_t key_template;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    int qual_ndx;
    int qual_ndx_2;
    int action_ndx;
    int action_ndx_2;
    int mask_ndx;
    unsigned int action_size;
    int found;
    int action_without_valid_bit;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_info_p, _SHR_E_PARAM, "flush_entry_info_p");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags are supported for flush entry add. Given flags were 0x%x.\n ", flags);
    }

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }

    if (flush_entry_info_p->payload_info.transplant && flush_entry_info_p->payload_info.delete)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot set both transplant and delete.\n");
    }

    if ((flush_entry_info_p->payload_info.transplant == FALSE) &&
        (flush_entry_info_p->payload_info.action_info[0].dnx_action != DNX_FIELD_ACTION_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Transplant was not set, but actions were provided to be changed.\n ");
    }

    if ((((int) entry_id) < 0) ||
        (entry_id >= dnx_data_field.exem_learn_flush_machine.flush_machine_nof_entries_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "entry_id must be between 0 and %d.\n",
                     dnx_data_field.exem_learn_flush_machine.flush_machine_nof_entries_get(unit) - 1);
    }

    /** Technically this check is already done by dnx_field_group_type_get() and is superfluous.*/
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group %d does not exist.\r\n", fg_id);
    }

    /** Get the field group's info. */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));

    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flush entries are only supported for EXEM field groups. Field group %d isn't exact match.\r\n",
                     fg_id);
    }

    /** Go over the qualifiers in the key. */
    for (qual_ndx = 0;
         qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
         && flush_entry_info_p->key_info.qual_info[qual_ndx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_ndx++)
    {
        /** Verify that each qualifier appears only once */
        for (qual_ndx_2 = 0; qual_ndx_2 < qual_ndx; qual_ndx_2++)
        {
            if (flush_entry_info_p->key_info.qual_info[qual_ndx].dnx_qual ==
                flush_entry_info_p->key_info.qual_info[qual_ndx_2].dnx_qual)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qualifier \"%s\" appears twice in the key, once in place %d and again in %d.\r\n",
                             dnx_field_dnx_qual_text(unit, flush_entry_info_p->key_info.qual_info[qual_ndx].dnx_qual),
                             qual_ndx_2, qual_ndx);
            }
        }
        /** Verify that each qualifier appears in the field group. */
        found = FALSE;
        for (qual_ndx_2 = 0; (qual_ndx_2 < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
             (key_template.key_qual_map[qual_ndx_2].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_ndx_2++)
        {
            if (flush_entry_info_p->key_info.qual_info[qual_ndx].dnx_qual ==
                key_template.key_qual_map[qual_ndx_2].qual_type)
            {
                found = TRUE;
                break;
            }
        }
        if (found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier %d (\"%s\") in key does not appear in field group %d.\r\n",
                         qual_ndx,
                         dnx_field_dnx_qual_text(unit, flush_entry_info_p->key_info.qual_info[qual_ndx].dnx_qual),
                         fg_id);
        }
    }

    /** Go over the actions in the key. */
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && flush_entry_info_p->key_info.action_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        /** Verify that each action appears only once */
        for (action_ndx_2 = 0; action_ndx_2 < action_ndx; action_ndx_2++)
        {
            if (flush_entry_info_p->key_info.action_info[action_ndx].dnx_action ==
                flush_entry_info_p->key_info.action_info[action_ndx_2].dnx_action)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action \"%s\" appears twice in the key, once in place %d and again in %d.\r\n",
                             dnx_field_dnx_action_text(unit,
                                                       flush_entry_info_p->key_info.action_info[action_ndx].dnx_action),
                             action_ndx_2, action_ndx);
            }
        }
        /** Verify that each action appears in the field group. */
        found = FALSE;
        for (action_ndx_2 = 0; (action_ndx_2 < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) &&
             (actions_payload_info.actions_on_payload_info[action_ndx_2].dnx_action != DNX_FIELD_ACTION_INVALID);
             action_ndx_2++)
        {
            if (flush_entry_info_p->key_info.action_info[action_ndx].dnx_action ==
                actions_payload_info.actions_on_payload_info[action_ndx_2].dnx_action)
            {
                found = TRUE;
                action_without_valid_bit =
                    actions_payload_info.actions_on_payload_info[action_ndx_2].dont_use_valid_bit;
                break;
            }
        }
        if (found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in key does not appear in field group %d.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->key_info.action_info[action_ndx].dnx_action),
                         fg_id);
        }
        /** Verify that the valid bit value and mask are one bit only. */
        if ((flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_value & (~1)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in key has valid_bit_value 0x%x, can only accept 0 or 1.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->key_info.action_info[action_ndx].dnx_action),
                         flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_value);
        }
        if ((flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_mask & (~1)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in key has valid_bit_mask 0x%x, can only accept 0 or 1.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->key_info.action_info[action_ndx].dnx_action),
                         flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_mask);
        }
        /** Verify that the valid bit value and mask are not set if there is no valid bit. */
        if (action_without_valid_bit &&
            ((flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_value != 0) ||
             (flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_mask != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in key has no valid bit. "
                         "valid bit_value 0x%x valid_bit_mask 0x%x\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->key_info.action_info[action_ndx].dnx_action),
                         flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_value,
                         flush_entry_info_p->key_info.action_info[action_ndx].valid_bit_mask);
        }
        /** Verify that the action masks are at most 32 bits. */
        for (mask_ndx = 1; mask_ndx < DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY; mask_ndx++)
        {
            if (flush_entry_info_p->key_info.action_info[action_ndx].action_mask[mask_ndx] != 0)
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action %d (\"%s\") in key mask larger than 32 bits.\r\n",
                             action_ndx,
                             dnx_field_dnx_action_text(unit,
                                                       flush_entry_info_p->key_info.
                                                       action_info[action_ndx].dnx_action));
        }
        /** Verify that the action masks are no bigger than the action size (not including valid bit). */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                        (unit, field_stage, flush_entry_info_p->key_info.action_info[action_ndx].dnx_action,
                         &action_size));
        if ((action_size < SAL_UINT32_NOF_BITS)
            && ((SAL_FROM_BIT(action_size) & flush_entry_info_p->key_info.action_info[action_ndx].action_mask[0]) != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in key has more than %d bits in mask 0x%x.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->key_info.action_info[action_ndx].dnx_action),
                         action_size, flush_entry_info_p->key_info.action_info[action_ndx].action_mask[0]);
        }
    }

    /** Go over the actions in the payload. */
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_ndx++)
    {
        /** Verify that each action appears only once */
        for (action_ndx_2 = 0; action_ndx_2 < action_ndx; action_ndx_2++)
        {
            if (flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action ==
                flush_entry_info_p->payload_info.action_info[action_ndx_2].dnx_action)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action \"%s\" appears twice in the payload, once in place %d and again in %d.\r\n",
                             dnx_field_dnx_action_text(unit,
                                                       flush_entry_info_p->payload_info.
                                                       action_info[action_ndx].dnx_action), action_ndx_2, action_ndx);
            }
        }
        /** Verify that each action appears in the field group. */
        found = FALSE;
        for (action_ndx_2 = 0; (action_ndx_2 < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) &&
             (actions_payload_info.actions_on_payload_info[action_ndx_2].dnx_action != DNX_FIELD_ACTION_INVALID);
             action_ndx_2++)
        {
            if (flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action ==
                actions_payload_info.actions_on_payload_info[action_ndx_2].dnx_action)
            {
                found = TRUE;
                action_without_valid_bit =
                    actions_payload_info.actions_on_payload_info[action_ndx_2].dont_use_valid_bit;
                break;
            }
        }
        if (found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in payload does not appear in field group %d.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action),
                         fg_id);
        }
        /** Verify that the valid bit value and mask are one bit only. */
        if ((flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_value & (~1)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in payload has valid_bit_value 0x%x, can only accept 0 or 1.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action),
                         flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_value);
        }
        if ((flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_mask & (~1)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in payload has valid_bit_mask 0x%x, can only accept 0 or 1.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action),
                         flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_mask);
        }
        /** Verify that the valid bit value and mask are not set if there is no valid bit. */
        if (action_without_valid_bit &&
            ((flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_value != 0) ||
             (flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_mask != 0)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in payload has not valid bit. "
                         "valid bit_value 0x%x valid_bit_mask 0x%x\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action),
                         flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_value,
                         flush_entry_info_p->payload_info.action_info[action_ndx].valid_bit_mask);
        }
        /** Verify that the action masks are at most 32 bits. */
        for (mask_ndx = 1; mask_ndx < DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY; mask_ndx++)
        {
            if (flush_entry_info_p->payload_info.action_info[action_ndx].action_mask[mask_ndx] != 0)
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action %d (\"%s\") in payload mask larger than 32 bits.\r\n",
                             action_ndx,
                             dnx_field_dnx_action_text(unit,
                                                       flush_entry_info_p->payload_info.
                                                       action_info[action_ndx].dnx_action));
        }
        /** Verify that the action masks are no bigger than the action size (not including valid bit). */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                        (unit, field_stage, flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action,
                         &action_size));
        if ((action_size < SAL_UINT32_NOF_BITS)
            && ((SAL_FROM_BIT(action_size) & flush_entry_info_p->payload_info.action_info[action_ndx].action_mask[0]) !=
                0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action %d (\"%s\") in payload has more than %d bits in mask 0x%x.\r\n",
                         action_ndx,
                         dnx_field_dnx_action_text(unit,
                                                   flush_entry_info_p->payload_info.action_info[action_ndx].dnx_action),
                         action_size, flush_entry_info_p->payload_info.action_info[action_ndx].action_mask[0]);
        }
    }

    /** Verify that the entry_id is empty.*/
    {
        int is_lexem;
        uint8 valid;
        uint8 hit_bit_value;
        uint8 hit_bit_mask;
        uint32 app_db_id_value;
        uint32 app_db_id_mask;
        uint8 command;
        uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
        uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
        uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
        uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];

        SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
        SHR_IF_ERR_EXIT(dnx_field_flush_machine_hw_get
                        (unit, is_lexem, entry_id, &valid, &hit_bit_value, &hit_bit_mask,
                         &app_db_id_value, &app_db_id_mask, &command, exem_raw_entry, exem_raw_entry_mask,
                         exem_new_payload_raw, exem_new_payload_raw_mask));
        if (valid)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "A flush entry already exists in entry ID %d.\r\n", entry_id);
        }
    }

    /*
     * Verify that the hit bit value and mask are one bit only.
     */
    if ((flush_entry_info_p->key_info.hit_bit_value & (~1)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "hit_bit_value 0x%x, can only accept 0 or 1.\r\n",
                     flush_entry_info_p->key_info.hit_bit_value);
    }
    if ((flush_entry_info_p->key_info.hit_bit_mask & (~1)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "hit_bit_mask 0x%x, can only accept 0 or 1.\r\n",
                     flush_entry_info_p->key_info.hit_bit_mask);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_flush_machine_entry_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    dnx_field_flush_entry_t * flush_entry_info_p)
{
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    dnx_field_app_db_id_t app_db_id;
    int app_db_id_size;
    uint32 app_db_id_mask;
    int is_lexem;
    uint32 command;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input.*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_machine_entry_add_verify(unit, flags, fg_id, entry_id, flush_entry_info_p));

    /** Create an EXEM entry as key.*/
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_entry_build_key_mask
                    (unit, fg_id, &(flush_entry_info_p->key_info), exem_raw_entry, exem_raw_entry_mask));

    /** Form the payload change.*/
    if (flush_entry_info_p->payload_info.transplant)
    {
        SHR_IF_ERR_EXIT(dnx_field_flush_machine_payload_build
                        (unit, fg_id, &(flush_entry_info_p->payload_info), exem_new_payload_raw,
                         exem_new_payload_raw_mask));
    }

    /** Get the app_db_id and its mask.*/
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &app_db_id));
    app_db_id_mask = SAL_UPTO_BIT(app_db_id_size);

    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));

    /*
     * Set the HW command values.
     */
    command = 0;
    if (flush_entry_info_p->payload_info.transplant)
    {
        command |= SAL_BIT(dnx_data_field.exem_learn_flush_machine.command_bit_transplant_get(unit));
    }
    if (flush_entry_info_p->payload_info.delete)
    {
        command |= SAL_BIT(dnx_data_field.exem_learn_flush_machine.command_bit_delete_get(unit));
    }
    if (flush_entry_info_p->payload_info.hit_bit_clear)
    {
        command |= SAL_BIT(dnx_data_field.exem_learn_flush_machine.command_bit_clear_hit_bit_get(unit));
    }

    /** Write to HW.*/
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_hw_set(unit, is_lexem, entry_id, 1,
                                                   flush_entry_info_p->key_info.hit_bit_value,
                                                   flush_entry_info_p->key_info.hit_bit_mask,
                                                   app_db_id, app_db_id_mask, command,
                                                   exem_raw_entry, exem_raw_entry_mask,
                                                   exem_new_payload_raw, exem_new_payload_raw_mask));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_flush_machine_entry_get(), before reading the HW entry.
* \param [in] unit                - Device ID.
* \param [in] flags               - The flags for the entry add.
* \param [in] fg_id               - The EXEM field group upon which we qualify.
* \param [in] entry_id            - The number of the entry in the TCAM.
* \param [in] flush_entry_info_p  - The parameters of the entry.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_entry_get_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    dnx_field_flush_entry_t * flush_entry_info_p)
{
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_info_p, _SHR_E_PARAM, "flush_entry_info_p");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags are supported for flush entry add. Given flags were 0x%x.\n ", flags);
    }

    if (dnx_data_field.features.exem_age_flush_scan_get(unit) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM Age/Flush scan is not supported on this device.\n");
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group %d does not exist.\r\n", fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_flush_machine_entry_get(), checking that the entry is of the same field group as
*  the API call.
* \param [in] unit                - Device ID.
* \param [in] fg_id               - The EXEM field group upon which we qualify.
* \param [in] entry_id            - The number of the entry in the TCAM. Used only for error message.
* \param [in] entry_app_db_id_value - The app_db_id of the entry
* \param [in] entry_app_db_id_mask  - The app_db_id mask of the entry.

* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_entry_get_verify_check_fg(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    uint32 entry_app_db_id_value,
    uint32 entry_app_db_id_mask)
{
    dnx_field_app_db_id_t fg_app_db_id;
    int fg_app_db_id_size;
    uint32 fg_app_db_id_mask;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_app_db_id_mask == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Flush entry found without app_db_id in entry_id %d.\r\n", entry_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &fg_app_db_id_size, &fg_app_db_id));
    fg_app_db_id_mask = SAL_UPTO_BIT(fg_app_db_id_size);
    if (fg_app_db_id_mask == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d has no app_db_id.\r\n", fg_id);
    }
    if (fg_app_db_id != entry_app_db_id_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flush entry in entry_id %d does not belong to field group %d.\r\n",
                     entry_id, fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_flush_machine_entry_get(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id,
    dnx_field_flush_entry_t * flush_entry_info_p)
{
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 entry_app_db_id_value;
    uint32 entry_app_db_id_mask;
    int is_lexem;
    uint8 command;
    uint8 entry_valid;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input.*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_machine_entry_get_verify(unit, flags, fg_id, entry_id, flush_entry_info_p));

    /** Init the output structure.*/
    SHR_IF_ERR_EXIT(dnx_field_flush_entry_t_init(unit, flush_entry_info_p));

    /** Read from HW.*/
    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_hw_get
                    (unit, is_lexem, entry_id, &entry_valid, &(flush_entry_info_p->key_info.hit_bit_value),
                     &(flush_entry_info_p->key_info.hit_bit_mask), &entry_app_db_id_value, &entry_app_db_id_mask,
                     &command, exem_raw_entry, exem_raw_entry_mask, exem_new_payload_raw, exem_new_payload_raw_mask));
    if (entry_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No flush entry was found in entry_id %d.\r\n", entry_id);
    }

    /** Verify the entry's field group.*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_machine_entry_get_verify_check_fg
                          (unit, fg_id, entry_id, entry_app_db_id_value, entry_app_db_id_mask));

    /** Parse the key.*/
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_entry_analyze_key_mask
                    (unit, fg_id, exem_raw_entry, exem_raw_entry_mask, &(flush_entry_info_p->key_info)));

    /** Parse the payload.*/
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_payload_analyze
                    (unit, fg_id, exem_new_payload_raw, exem_new_payload_raw_mask,
                     &(flush_entry_info_p->payload_info)));

    /*
     * Get the HW command values.
     */
    if ((command & SAL_BIT(dnx_data_field.exem_learn_flush_machine.command_bit_transplant_get(unit))) != 0)
    {
        flush_entry_info_p->payload_info.transplant = TRUE;
    }
    if ((command & SAL_BIT(dnx_data_field.exem_learn_flush_machine.command_bit_delete_get(unit))) != 0)
    {
        flush_entry_info_p->payload_info.delete = TRUE;
    }
    if ((command & SAL_BIT(dnx_data_field.exem_learn_flush_machine.command_bit_clear_hit_bit_get(unit))) != 0)
    {
        flush_entry_info_p->payload_info.hit_bit_clear = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_flush_machine_entry_delete().
* \param [in] unit                - Device ID.
* \param [in] flags               - The flags for the entry delete.
* \param [in] fg_id               - The EXEM field group upon which the entry qualifies.
* \param [in] entry_id            - The number of the entry in the TCAM.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_flush_machine_entry_delete_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id)
{
    uint8 is_allocated;
    dnx_field_group_type_e fg_type;
    dnx_field_app_db_id_t fg_app_db_id;
    int app_db_id_size;
    uint8 valid;
    uint32 entry_app_db_id_value;
    uint32 entry_app_db_id_mask;

    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags are supported for flush entry delete. Given flags were 0x%x.\n ", flags);
    }

    if ((((int) entry_id) < 0) ||
        (entry_id >= dnx_data_field.exem_learn_flush_machine.flush_machine_nof_entries_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "entry_id must be between 0 and %d.\n",
                     dnx_data_field.exem_learn_flush_machine.flush_machine_nof_entries_get(unit) - 1);
    }

    /** Technically this check is already done by dnx_field_group_type_get() and is superfluous.*/
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group %d does not exist.\r\n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flush entries are only supported for EXEM field groups. Field  group %d isn't exact match.\r\n",
                     fg_id);
    }

    /*
     * Verify that the entry_id has an entry belonging to the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &fg_app_db_id));
    if (app_db_id_size == 0)
    {
        /*
         * If the fg_id has no app_db_id bits we cannot determine what field group it belongs to.
         */
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d has no app_db_id bits.\r\n", fg_id);
    }
    {
        int is_lexem;
        uint8 hit_bit_value;
        uint8 hit_bit_mask;
        uint8 command;
        uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
        uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
        uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];
        uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY];

        SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));
        SHR_IF_ERR_EXIT(dnx_field_flush_machine_hw_get
                        (unit, is_lexem, entry_id, &valid, &hit_bit_value, &hit_bit_mask,
                         &entry_app_db_id_value, &entry_app_db_id_mask, &command, exem_raw_entry, exem_raw_entry_mask,
                         exem_new_payload_raw, exem_new_payload_raw_mask));
    }
    if (valid == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No flush entry was found in entry_id %d.\r\n", entry_id);
    }
    if (entry_app_db_id_mask == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Flush entry found without app_db_id in entry_id %d.\r\n", entry_id);
    }
    if (fg_app_db_id != entry_app_db_id_value)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flush entry in entry_id %d does not belong to field group %d.\r\n", entry_id,
                     fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_flush_machine_entry_delete(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_flush_entry_id_t entry_id)
{
    uint32 exem_raw_entry[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_raw_entry_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_new_payload_raw[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    uint32 exem_new_payload_raw_mask[DNX_FIELD_FLUSH_ENTRY_MAX_NOF_UINT32_IN_EXEM_ENTRY] = { 0 };
    int is_lexem;
    uint32 command;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input.*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_machine_entry_delete_verify(unit, flags, fg_id, entry_id));

    SHR_IF_ERR_EXIT(dnx_field_group_exem_is_lexem(unit, fg_id, &is_lexem));

    /** Give default value to command.*/
    command = 0;

    /** Write to HW default entry.*/
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_hw_set(unit, is_lexem, entry_id, 0, 0, 0, 0, 0, command,
                                                   exem_raw_entry, exem_raw_entry_mask,
                                                   exem_new_payload_raw, exem_new_payload_raw_mask));

exit:
    SHR_FUNC_EXIT;
}
