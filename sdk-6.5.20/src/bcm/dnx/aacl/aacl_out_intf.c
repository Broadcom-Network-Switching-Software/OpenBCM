/** \file aacl_out_intf.c
 *
 * Internal DNX AACL OUTPUT Interface APIs connect to DBAL
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_AACL_MNGR

/*
 * Include files.
 * {
 */
#include "aacl_out_intf.h"

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
 * TYPEDEF
 * {
 */
/*
 * Operations performed on LPM table Entries
 */
typedef enum
{
    AACL_ENTRY_ADD,
    AACL_ENTRY_DELETE,
    AACL_ENTRY_PAYLOAD_UPDATE
} aacl_out_entry_action;

/*
 * }
 */
/*
 * FUNCTIONs
 * {
 */

#ifdef DNX_AACL_OPERATIONAL
/* { */
static shr_error_e
dnx_aacl_lpm_prefix_len_to_mask(
    int unit,
    int prefix_length,
    uint32 max_key_size_in_bits,
    uint32 *mask)
{
    uint32 max_key_size_in_uint32 = BITS2WORDS(max_key_size_in_bits);

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(mask, 0x0, DBAL_PHYSICAL_KEY_SIZE_IN_WORDS * sizeof(mask[0]));

    if ((prefix_length >= 0) && (prefix_length <= max_key_size_in_bits))
    {
        int index;
        for (index = 0; index < max_key_size_in_uint32; index++)
        {
            if (prefix_length > index * SAL_UINT32_NOF_BITS)
            {
                int local_prefix_length = prefix_length - index * SAL_UINT32_NOF_BITS;
                mask[max_key_size_in_uint32 - 1 - index] =
                    _shr_ip_mask_create(local_prefix_length >
                                        SAL_UINT32_NOF_BITS ? SAL_UINT32_NOF_BITS : local_prefix_length);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Error: Prefix must be between 0 and %d, prefix given: %d.\n",
                     max_key_size_in_bits, prefix_length);
    }

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dnx_aacl_lpm_entry_action_perform(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size,
    aacl_out_entry_action action)
{
    uint32 entry_handle_id;
    uint32 mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    dbal_tables_e dbal_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Error check for group_id parameter */
    if (group_id >= AACL_MAX_GROUP_ID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal group_id is passed - %d Max group_id are %d\n",
                     group_id, AACL_MAX_GROUP_ID);
    }

    /** AACL_LPM_INDEX1_ENTRY AACL_LPM_INDEX2_ENTRY
     * are the only valid types for lpm entry
     */
    if (entry_type == AACL_LPM_INDEX1_ENTRY)
    {
        dbal_id = aacl_group_to_dbal_info[unit][group_id].index1_dbal_id;
    }
    else if (entry_type == AACL_LPM_INDEX2_ENTRY)
    {
        dbal_id = aacl_group_to_dbal_info[unit][group_id].index2_dbal_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal Entry Type %d \n", entry_type);
    }

    /** Allocate a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_id, &entry_handle_id));

    /** Construct the mask based on the incoming prefix length */
    SHR_IF_ERR_EXIT(dnx_aacl_lpm_prefix_len_to_mask(unit, prefix_length,
                                                    dnx_data_mdb.kaps.key_width_in_bits_get(unit), mask));

    /** Fill the dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                    (unit, entry_handle_id, prefix, mask, DBAL_CORE_ALL, payload_size, payload));

    switch (action)
    {
        case AACL_ENTRY_ADD:
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** Commit the dbal entry */
            break;

        case AACL_ENTRY_DELETE:
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT)); /** clearing the entry */
            break;

        case AACL_ENTRY_PAYLOAD_UPDATE:
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE)); /** updating the entry */
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal Action Type %d \n", action); /** Invalid ACTION */
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_lpm_entry_add(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_aacl_lpm_entry_action_perform(unit, group_id,
                                                      entry_type, prefix, prefix_length,
                                                      payload, payload_size, AACL_ENTRY_ADD));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_lpm_entry_payload_update(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    uint32 payload_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_aacl_lpm_entry_action_perform(unit, group_id, entry_type,
                                                      prefix, prefix_length,
                                                      payload, payload_size, AACL_ENTRY_PAYLOAD_UPDATE));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_lpm_entry_delete(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_aacl_lpm_entry_action_perform(unit, group_id, entry_type,
                                                      prefix, prefix_length, NULL, 0, AACL_ENTRY_DELETE));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_tcam_entry_action_perform(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    int priority,
    uint32 *payload,
    int payload_size,
    uint32 *entry_access_id,
    aacl_out_entry_action action)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Error check for group_id parameter */
    if (group_id >= AACL_MAX_GROUP_ID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal group_id is passed - %d Max group_id are %d\n",
                     group_id, AACL_MAX_GROUP_ID);
    }

    /** AACL_TCAM_COMPRESSED_ENTRY AACL_TCAM_OVERFLOW_ENTRY
     * are the only valid types for tcam entry
     */
    if (entry_type == AACL_TCAM_COMPRESSED_ENTRY)
    {
        dbal_id = aacl_group_to_dbal_info[unit][group_id].compressed_dbal_id;
    }
    else if (entry_type == AACL_TCAM_OVERFLOW_ENTRY)
    {
        dbal_id = aacl_group_to_dbal_info[unit][group_id].overflow_dbal_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal Entry Type %d \n", entry_type);
    }

    /** Allocate a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_id, &entry_handle_id));

    /** Fill the dbal entry */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                    (unit, entry_handle_id, key, mask, DBAL_CORE_ALL, payload_size, payload));

    switch (action)
    {
        case AACL_ENTRY_ADD:
        {
            /** Set the priority for entry */
            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, DBAL_CORE_ALL, dbal_id, priority, entry_access_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *entry_access_id));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT)); /** Commit the dbal entry */
            break;
        }
        case AACL_ENTRY_DELETE:
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *entry_access_id));
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT)); /** clearing the entry */
            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, dbal_id, *entry_access_id));
            break;
        }
        case AACL_ENTRY_PAYLOAD_UPDATE:
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *entry_access_id));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE)); /** updating the entry */
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal Action Type %d \n", action); /** Invalid ACTION */
            break;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_tcam_entry_add(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    int priority,
    uint32 *payload,
    int payload_size,
    uint32 *entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_aacl_tcam_entry_action_perform(unit, group_id,
                                                       entry_type, key, mask, priority, payload, payload_size, entry_id,
                                                       AACL_ENTRY_ADD));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_tcam_entry_delete(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 entry_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_aacl_tcam_entry_action_perform(unit, group_id,
                                                       entry_type, key, mask, 0, NULL, 0, &entry_id,
                                                       AACL_ENTRY_DELETE));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_aacl_tcam_entry_payload_update(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 *payload,
    int payload_size,
    uint32 entry_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_aacl_tcam_entry_action_perform(unit, group_id,
                                                       entry_type, key, mask, 0, payload, payload_size, &entry_id,
                                                       AACL_ENTRY_PAYLOAD_UPDATE));

exit:
    SHR_FUNC_EXIT;
}
/* } */
#endif
