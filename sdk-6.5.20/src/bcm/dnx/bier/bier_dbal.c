/** \file bier_dbal.c
 * 
 *
 * General MULTICAST functionality for DNX.
 * Dedicated set of MULTICAST APIs are distributed between multicast_*.c files: \n
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <bcm/types.h>
#include <bcm_int/dnx/multicast/multicast.h>

#include "bier_imp.h"
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

/*
 * See .h
 */
shr_error_e
dnx_bier_entry_set(
    int unit,
    uint32 is_ingress,
    int core,
    uint32 bfr_id,
    dnx_bier_mcdb_dbal_field_t * entry_val)
{
    uint32 entry_handle_id;
    uint32 copy_num, nof_copies;
    int ext_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    /*
     * iterate over the number of copies needed for each table. there are devices which need
     * more than 1 copy for egress
     */
    nof_copies = is_ingress ? 1 : dnx_data_bier.params.nof_egr_table_copies_get(unit);

    for (copy_num = 0; copy_num < nof_copies; copy_num++)
    {
        if (is_ingress)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_ING_TABLE, &entry_handle_id));
        }
        else if (core || copy_num)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_1_TABLE, &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_0_TABLE, &entry_handle_id));
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, bfr_id);
        dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
        if (is_ingress)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, entry_val->dest);
        }
        else
        {
            if (ext_mode)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, INST_SINGLE,
                                             entry_val->dest);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_NORMAL_DESTINATION);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, INST_SINGLE,
                                             entry_val->dest);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCM, INST_SINGLE, entry_val->tcm);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, entry_val->cud);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_BITSTR, INST_SINGLE, entry_val->bmp);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, entry_val->valid);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_bier_entry_get(
    int unit,
    uint32 is_ingress,
    int core,
    uint32 bfr_id,
    dnx_bier_mcdb_dbal_field_t * entry_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_ING_TABLE, &entry_handle_id));
    }
    else if (core)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_1_TABLE, &entry_handle_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_0_TABLE, &entry_handle_id));
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, bfr_id);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    if (is_ingress)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &entry_val->dest);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud);
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_BITSTR, INST_SINGLE, entry_val->bmp);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &entry_val->valid);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_val->format));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_TCM, INST_SINGLE, &entry_val->tcm));
        if (entry_val->format == DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION_EXT, INST_SINGLE, &entry_val->dest));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_EGRESS_DESTINATION, INST_SINGLE, &entry_val->dest));
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_CUD, INST_SINGLE, &entry_val->cud));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_BITSTR, INST_SINGLE, entry_val->bmp));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &entry_val->valid));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
bier_dbal_update_predef_values(
    int unit,
    uint32 region_size)
{
    uint32 base_in_banks = 0;
    uint32 bier_start_offset = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Base field is SW cached value, need to be recoveryed
     */
    /*
     * update dbal ingress table with valid min/max offset
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                    (unit, DBAL_TABLE_BIER_ING_TABLE, DBAL_FIELD_BASE, TRUE, 0, 0, DBAL_PREDEF_VAL_MIN_VALUE,
                     base_in_banks));
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                    (unit, DBAL_TABLE_BIER_ING_TABLE, DBAL_FIELD_BASE, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     base_in_banks));

    /*
     * if ingress replication exist in the system, egress offsets needs to be set after ingress part.
     */
    if (!dnx_data_bier.params.egress_only_get(unit))
    {
        bier_start_offset += region_size;
        base_in_banks = bier_start_offset / dnx_data_bier.params.bank_entries_get(unit) /
            dnx_data_bier.params.bfr_entry_size_get(unit);
    }

    /*
     * update dbal egress tables with valid min/max offset
     */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                    (unit, DBAL_TABLE_BIER_EGR_0_TABLE, DBAL_FIELD_BASE, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     base_in_banks));
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                    (unit, DBAL_TABLE_BIER_EGR_0_TABLE, DBAL_FIELD_BASE, TRUE, 0, 0, DBAL_PREDEF_VAL_MIN_VALUE,
                     base_in_banks));
    base_in_banks += region_size / dnx_data_bier.params.bank_entries_get(unit) /
        dnx_data_bier.params.bfr_entry_size_get(unit);
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                    (unit, DBAL_TABLE_BIER_EGR_1_TABLE, DBAL_FIELD_BASE, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                     base_in_banks));
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                    (unit, DBAL_TABLE_BIER_EGR_1_TABLE, DBAL_FIELD_BASE, TRUE, 0, 0, DBAL_PREDEF_VAL_MIN_VALUE,
                     base_in_banks));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/*
 * See .h
 */
shr_error_e
dnx_bier_dbal_init(
    int unit,
    uint32 region_size)
{
    uint32 entry_handle_id;
    uint32 bier_start_offset = 0;
    int ext_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    region_size /= dnx_data_bier.params.egress_only_get(unit) ? 2 : 3;

    /*
     * Base field is SW cached value, need to be recoveryed
     */
    SHR_IF_ERR_EXIT(bier_dbal_update_predef_values(unit, region_size));

    if (sw_state_is_warm_boot(unit))
    {
        SHR_EXIT();
    }

    /*
     * set the offsets to HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_OFFSETS, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_CORE_0_OFFSET, INST_SINGLE,
                                 bier_start_offset);
    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_CORE_1_OFFSET, INST_SINGLE,
                                     bier_start_offset);
    }

    /*
     * if ingress replication exist in the system, egress offsets needs to be set after ingress part.
     */
    if (!dnx_data_bier.params.egress_only_get(unit))
    {
        bier_start_offset += region_size;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_CORE_0_OFFSET, INST_SINGLE,
                                 bier_start_offset);

    /*
     * if more than 1 copy is requested etpp & egress core 0 need to have different offsets
     */
    if (dnx_data_bier.params.nof_egr_table_copies_get(unit) > 1)
    {
        bier_start_offset += region_size;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_CORE_0_OFFSET, INST_SINGLE, bier_start_offset);

    if (dnx_data_device.general.nof_cores_get(unit) > 1)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_CORE_1_OFFSET, INST_SINGLE,
                                     bier_start_offset + region_size);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETPP_CORE_1_OFFSET, INST_SINGLE,
                                     bier_start_offset + region_size);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * set invalid to all entries and tables
     */
    SHR_IF_ERR_EXIT(dnx_multicast_utils_ext_mode_get(unit, &ext_mode));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_ING_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_0_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    if (ext_mode)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_NORMAL_DESTINATION);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_1_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    if (ext_mode)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_EXTENDED_DESTINATION);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_BIER_EGR_0_TABLE_NORMAL_DESTINATION);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_bier_group_invalid_set(
    int unit,
    bcm_multicast_t group,
    int set_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /**DBAL_TABLE_BIER_ING_TABLE*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_ING_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, group, set_size);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BITSTR, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**DBAL_TABLE_BIER_EGR_0_TABLE*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_0_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, group, set_size);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BITSTR, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**DBAL_TABLE_BIER_EGR_1_TABLE*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_BIER_EGR_1_TABLE, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_BFR_ID, group, set_size);
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BASE, DBAL_PREDEF_VAL_MIN_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_BITSTR, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
