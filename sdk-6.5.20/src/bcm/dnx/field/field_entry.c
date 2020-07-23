/** \file field_entry.c
 * 
 *
 * Field entry procedures for DNX.
 * This is part of Field Group Database module, help for:
 * Management for pushing entries into TCAM/ MDB / State Table / Action payload of TCAM (DT)
 */
/*
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
/** allow memregs.h include explicitly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>

#include <shared/gport.h>
#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/mem.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/dnxc/drv_dnxc_utils.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> 

#include <bcm_int/dnx/algo/ppmc/algo_ppmc.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.h>

/*
 * }Include files
 */

#define DNX_FIELD_ENTRY_STATE_TABLE_DELETE_VAL 0

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_t_init(
    int unit,
    dnx_field_entry_t * entry_add_in_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_add_in_p, _SHR_E_PARAM, "entry_add_in_p");

    sal_memset(entry_add_in_p, 0x0, sizeof(*entry_add_in_p));

    entry_add_in_p->priority = DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID;

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; ii++)
    {
        entry_add_in_p->payload_info.action_info[ii].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        entry_add_in_p->key_info.qual_info[ii].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    entry_add_in_p->core = _SHR_CORE_ALL;

    entry_add_in_p->valid_bit = TRUE;

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_payload_t_init(
    int unit,
    dnx_field_entry_payload_t * payload_info_p)
{
    unsigned int action_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(payload_info_p, _SHR_E_PARAM, "payload_info_p");

    sal_memset(payload_info_p, 0x0, sizeof(*payload_info_p));

    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_ndx++)
    {
        payload_info_p->action_info[action_ndx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Call DBAL table and set the TCAM entry into HW
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_add_flags_e flags.
* \param [in] field_stage      - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id    - DBAL table id that was created for Field group ad fg_create()
* \param [in] access_id        - Access ID that TCAM manager allocated for this entry
* \param [in] entry_info_p     - Information that will be written into HW (QUALS & ACTIONS)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/ static shr_error_e
dnx_field_entry_tcam_hw_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    uint32 access_id,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 action_idx;
    dbal_fields_e field_id;
    dbal_entry_action_flags_e dbal_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id,
                                              entry_info_p->key_info.qual_info[qual_idx].qual_value,
                                              entry_info_p->key_info.qual_info[qual_idx].qual_mask);

    }

    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action(unit, field_stage,
                                                         entry_info_p->payload_info.action_info[action_idx].dnx_action,
                                                         &field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         entry_info_p->payload_info.action_info[action_idx].action_value);
    }

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        dbal_flags = (DBAL_COMMIT_UPDATE | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }
    else
    {
        dbal_flags = (DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Call DBAL table and set the EXEM entry into HW
* \param [in] unit           - Device ID
* \param [in] flags            - dnx_field_entry_add_flags_e flags.
* \param [in] field_stage    - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id  - DBAL table id that was created for Field group at fg_create()
* \param [in] entry_info_p   - Information that will be written into HW (QUALS & ACTIONS)
* \return
*   shr_error_e - Error Type
* \remark
*   * Unlike dnx_field_entry_tcam_hw_set(), this procedure requires all qualifiers to be given to it.
*     The size of the qualifiers and actions array are terminated by DNX_FIELD_QUAL_TYPE_INVALID
*     and DNX_FIELD_ACTION_INVALID respectively, otherwise all elements are regarded as valid.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_exem_hw_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 action_idx;
    dbal_fields_e field_id;
    dbal_entry_action_flags_e dbal_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id,
                                       entry_info_p->key_info.qual_info[qual_idx].qual_value);
    }

    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action(unit, field_stage,
                                                         entry_info_p->payload_info.action_info[action_idx].dnx_action,
                                                         &field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         entry_info_p->payload_info.action_info[action_idx].action_value);
    }

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        dbal_flags = (DBAL_COMMIT_UPDATE | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }
    else
    {
        dbal_flags = (DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Call DBAL table and set the MDB DT entry into HW
* \param [in] unit           - Device ID
* \param [in] flags          - TBD
* \param [in] field_stage    - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id  - DBAL table id that was created for Field group at fg_create()
* \param [in] entry_info_p   - Information that will be written into HW (QUALS & ACTIONS)
* \return
*   shr_error_e - Error Type
* \remark
*   * Unlike dnx_field_entry_tcam_hw_set(), this procedure requires all qualifiers to be given to it.
*     The size of the qualifiers and actions array are terminated by DNX_FIELD_QUAL_TYPE_INVALID
*     and DNX_FIELD_ACTION_INVALID respectively, otherwise all elements are regarded as valid.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_mdb_dt_hw_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 action_idx;
    dbal_fields_e field_id;
    dbal_entry_action_flags_e dbal_flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id,
                                       entry_info_p->key_info.qual_info[qual_idx].qual_value);
    }

    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action(unit, field_stage,
                                                         entry_info_p->payload_info.action_info[action_idx].dnx_action,
                                                         &field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         entry_info_p->payload_info.action_info[action_idx].action_value);
    }

    dbal_flags = (DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Call DBAL table and set the KBP entry into HW
* \param [in] unit           - Device ID
* \param [in] flags            - dnx_field_entry_add_flags_e flags.
* \param [in] field_stage    - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id  - DBAL table id that was created for Field group at fg_create()
* \param [in] entry_info_p   - Information that will be written into HW (QUALS & ACTIONS)
* \param [in,out] access_id_p   - Access id received from the KBP hw. Input if WITH_ID or UPDATE flags are set.
* \return
*   shr_error_e - Error Type
* \remark
*     The size of the qualifiers and actions array are terminated by DNX_FIELD_QUAL_TYPE_INVALID
*     and DNX_FIELD_ACTION_INVALID respectively, otherwise all elements are regarded as valid.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_kbp_hw_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *access_id_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 action_idx;
    dbal_fields_e field_id;
    uint8 is_ranged;
    dbal_entry_action_flags_e dbal_flags;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, entry_info_p->priority));

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, field_id, &is_ranged));
        if (is_ranged)
        {
            if (utilex_bitstream_have_one(entry_info_p->key_info.qual_info[qual_idx].qual_mask,
                                          (sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_mask) /
                                           sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_mask[0]))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier %s is a ranged qualifier, but also has a mask.\n ",
                             dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual));
            }
            /*
             * Note that at this point we don't support ranges larger then 32 bits
             * (in fact, e only support 16 bit ranges).
             */
            dbal_entry_key_field32_range_set(unit, entry_handle_id, field_id,
                                             entry_info_p->key_info.qual_info[qual_idx].qual_value[0],
                                             entry_info_p->key_info.qual_info[qual_idx].qual_max_value[0]);
        }
        else
        {
            if (utilex_bitstream_have_one(entry_info_p->key_info.qual_info[qual_idx].qual_max_value,
                                          (sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_max_value) /
                                           sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_max_value[0]))))

            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier %s is not a ranged qualifier, but also has a max_value.\n ",
                             dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual));
            }
            dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id,
                                                  entry_info_p->key_info.qual_info[qual_idx].qual_value,
                                                  entry_info_p->key_info.qual_info[qual_idx].qual_mask);
        }

    }

    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action(unit, field_stage,
                                                         entry_info_p->payload_info.action_info[action_idx].dnx_action,
                                                         &field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         entry_info_p->payload_info.action_info[action_idx].action_value);
    }

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        dbal_flags = (DBAL_COMMIT_UPDATE | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }
    else
    {
        dbal_flags = (DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }

    if ((flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID) || (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *access_id_p));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));

    /*
     * The access id is created by the KBP device and can be only read after the hw_set (entry dbal_commit) Thus, we
     * read it here.
     */
    if ((!(flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID)) && (!(flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, access_id_p));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * Refer to field_entry.h file for more information
 */
static shr_error_e
dnx_field_ace_entry_hw_set(
    int unit,
    int result_type_index,
    int is_update,
    dnx_field_ace_key_t ace_entry_key,
    dnx_field_entry_payload_t * payload_info_p)
{
    uint32 entry_handle_id;
    unsigned int action_idx;
    dbal_fields_e field_id;
    int commit_or_update_flag;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, &entry_handle_id));

    /*
     * Write the key.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_CUD, ace_entry_key);
    /*
     * Write the result type index.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type_index);
    /*
     * Write the rest of the payload.
     */
    for (action_idx = 0;
         action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && payload_info_p->action_info[action_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action(unit, DNX_FIELD_STAGE_ACE,
                                                         payload_info_p->action_info[action_idx].dnx_action,
                                                         &field_id));
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         payload_info_p->action_info[action_idx].action_value);
    }
    if (is_update)
    {
        commit_or_update_flag = DBAL_COMMIT_UPDATE;
    }
    else
    {
        commit_or_update_flag = DBAL_COMMIT;
    }
    /*
     * The flag DBAL_COMMIT_OVERRIDE_DEFAULT is set, to set default values 0 to all actions that aren't given,
     * so that their valid bit is set to zero.
     * If polarity is changed, the code will need to change as well.
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_or_update_flag | DBAL_COMMIT_OVERRIDE_DEFAULT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Sets the given data at the given address in the State Table Memory.
*
* \param [in] unit    - Device ID
* \param [in] address - Address of the entry to set (between 0-1023)
* \param [in] data    - Data to set at the given address
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_state_table_hw_set(
    int unit,
    uint32 address,
    uint32 data)
{
    uint32 entry_handle_id;
    int data_size_entry_max = dnx_data_field.state_table.data_size_entry_max_get(unit);
    int data_size = dnx_data_field.state_table.data_size_get(unit);
    uint32 hw_address;
    uint32 hw_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if ((data & (~(SAL_UPTO_BIT(data_size)))) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry data 0x%x (for address 0x%x) is larger than data size (%d).\r\n",
                     data, address, data_size);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_STATE_TABLE, &entry_handle_id));

    if (data_size != data_size_entry_max)
    {
        /*
         * Get the entry and update only a part of it.
         */
        int address_size_entry_max = dnx_data_field.state_table.address_size_entry_max_get(unit);
        int address_size = dnx_data_field.state_table.address_size_get(unit);
        int lsb_address_size = address_size - address_size_entry_max;
        uint32 lsb_address;
        uint32 old_data;
        int new_data_offset;
        uint32 new_data_mask;

        /*
         * Sanity check, as it was checked during init.
         */
        if (data_size > data_size_entry_max || address_size_entry_max > address_size ||
            address_size >= SAL_UINT32_NOF_BITS || data_size_entry_max >= SAL_UINT32_NOF_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported state table DNX DATA values. "
                         "data_size_entry_max %d data_size %d address_size_entry_max %d address_size %d.\r\n",
                         data_size_entry_max, data_size, address_size_entry_max, address_size);
        }

        hw_address = address >> lsb_address_size;
        lsb_address = address & (SAL_UPTO_BIT(lsb_address_size));
        new_data_offset = lsb_address * data_size;
        new_data_mask = (SAL_UPTO_BIT(data_size)) << new_data_offset;

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STATE_TABLE_ADDRESS_HW, hw_address);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_STATE_TABLE_DATA_HW, INST_SINGLE, &old_data));

        hw_data = (old_data & (~new_data_mask)) | (data << new_data_offset);
    }
    else
    {
        hw_address = address;
        hw_data = data;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STATE_TABLE_ADDRESS_HW, hw_address);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STATE_TABLE_DATA_HW, INST_SINGLE, hw_data);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Rebuilds the TCAM DT qualifiers value from the given key.
* \param [in] unit             - Device ID.
* \param [in] stage            - Stage of the FG.
* \param [in] key_value - Key from which the qualifiers values will be extracted.
* \param [out] entry_key_info_p - Contains qualifiers info,
*  where the re-builded qualifiers values are returned.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_entry_tcam_add
*/
static shr_error_e
dnx_field_entry_tcam_dt_key_rebuild(
    int unit,
    dnx_field_stage_e stage,
    uint32 key_value,
    dnx_field_entry_key_t * entry_key_info_p)
{
    int qual_idx;
    int qual_lsb;
    uint32 key[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    qual_lsb = 0;
    key[0] = key_value;

    for (qual_idx = 0;
         qual_idx < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP
         && entry_key_info_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        uint32 qual_size;
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, stage, entry_key_info_p->qual_info[qual_idx].dnx_qual, &qual_size));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key, qual_lsb, qual_size,
                                                       &entry_key_info_p->qual_info[qual_idx].qual_value[0]));

        qual_lsb += qual_size;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* This procedure retrieves the content of a TCAM entry from HW.
* \param [in] unit             -  Device ID
* \param [in] field_stage      -  Field Stage (iPMF1/2...)
* \param [in] fg_id            -  Identifier of the field group. Used for debugging.
* \param [in] dbal_table_id    -  DBAL table id that was created for Field group at fg_create()
* \param [in] access_id        -  Access ID that TCAM manager allocated for this entry
* \param [out] entry_info_p    -  Information extracted from the HW (QUALS & ACTIONS)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dbal_tables_e dbal_table_id,
    uint32 access_id,
    dnx_field_entry_t * entry_info_p)
{
    uint32 qual_idx;
    dbal_fields_e field_id;
    uint32 entry_handle_id;
    dnx_field_group_type_e fg_type;
    uint32 qual_value_ndx;
    uint32 nof_qual_values;
    uint32 action_value_ndx;
    uint32 nof_action_values;
    uint32 action_fg_idx;
    uint32 action_entry_idx;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Reading TCAM entry from field group %d access_id (entry handle) 0x%x. %s%s\r\n",
                 fg_id, access_id, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Get - all results fields of the table_p will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    /** Set the key in case we have TCAM DT */
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_dt_key_rebuild
                        (unit, field_stage, DNX_FIELD_ENTRY_ACCESS_ID(access_id), &(entry_info_p->key_info)));

        for (qual_idx = 0;
             (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
             (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                           entry_info_p->key_info.qual_info[qual_idx].dnx_qual,
                                                           &field_id));

            dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id,
                                                  entry_info_p->key_info.qual_info[qual_idx].qual_value,
                                                  entry_info_p->key_info.qual_info[qual_idx].qual_mask);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Parse all result fields.
     * Note we formerly used "table_p->keys_info[qual_idx].field_id" to get the key qualifiers,
     * but that uses the dbal implementation, so we now use the qualifiers found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    /*
     * DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
     */
    nof_qual_values =
        sizeof(entry_info_p->key_info.qual_info[0].qual_value) /
        sizeof(entry_info_p->key_info.qual_info[0].qual_value[0]);
    for (qual_idx = 0;
         entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID
         && qual_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; qual_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s value 0x%s%s%s",
                     dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual), EMPTY, EMPTY,
                     EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                        (unit, entry_handle_id, field_id,
                         entry_info_p->key_info.qual_info[qual_idx].qual_value,
                         entry_info_p->key_info.qual_info[qual_idx].qual_mask));
        for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values; qual_value_ndx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                         entry_info_p->key_info.qual_info[qual_idx].qual_value[nof_qual_values - qual_value_ndx - 1],
                         EMPTY, EMPTY, EMPTY);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, " mask 0x%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
        for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values; qual_value_ndx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                         entry_info_p->key_info.qual_info[qual_idx].qual_mask[nof_qual_values - qual_value_ndx - 1],
                         EMPTY, EMPTY, EMPTY);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

    /*
     * Read the actions, removing the actions not found in the entry.
     * Note that formerly we used "table_p->multi_res_info[0]" to get the result fields (actions),
     * but that uses the dbal implementation, so we now use the actions found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Payload:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    /*
     * DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY
     */
    nof_action_values =
        sizeof(entry_info_p->payload_info.action_info[0].action_value) /
        sizeof(entry_info_p->payload_info.action_info[0].action_value[0]);
    action_entry_idx = 0;
    for (action_fg_idx = 0;
         action_fg_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_fg_idx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_fg_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Action: %s 0x%s%s%s",
                     dnx_field_dnx_action_text(unit, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action),
                     EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action,
                         &field_id));
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                     entry_info_p->payload_info.
                                                     action_info[action_entry_idx].action_value);
        if (rv == _SHR_E_NONE)
        {
            entry_info_p->payload_info.action_info[action_entry_idx].dnx_action =
                entry_info_p->payload_info.action_info[action_fg_idx].dnx_action;
            action_entry_idx++;
            for (action_value_ndx = 0; action_value_ndx < nof_action_values; action_value_ndx++)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                             entry_info_p->payload_info.action_info[action_entry_idx].action_value[nof_action_values -
                                                                                                   action_value_ndx -
                                                                                                   1], EMPTY, EMPTY,
                             EMPTY);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "not found in entry.%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    if (action_entry_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        entry_info_p->payload_info.action_info[action_entry_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
* \brief
* This procedure retrieves the content of the KBP entry from HW.
* \param [in] unit             -  Device ID
* \param [in] field_stage      -  Field Stage (iPMF1/2...)
* \param [in] fg_id            -  Identifier of the field group. Used for debugging.
* \param [in] dbal_table_id    -  DBAL table id that was created for Field group at fg_create()
* \param [in] entry_access_id  -  Access ID that KBP manager allocated for this entry
* \param [out] entry_info_p    -  Information extracted from the HW (QUALS & ACTIONS)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_kbp_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dbal_tables_e dbal_table_id,
    uint32 entry_access_id,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_handle_id;
    uint32 access_id;
    uint32 qual_idx;
    uint32 qual_value_ndx;
    uint32 nof_qual_values;
    uint32 action_value_ndx;
    uint32 nof_action_values;
    uint32 action_fg_idx;
    uint32 action_entry_idx;
    dbal_fields_e field_id;
    shr_error_e rv;
    uint8 is_ranged;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    access_id = DNX_FIELD_ENTRY_ACCESS_ID(entry_access_id);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Reading KBP entry from field group %d access_id (entry handle) 0x%X. %s%s\r\n",
                 fg_id, access_id, EMPTY, EMPTY);

    /** Get - all results fields of the table_p will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    /*
     * Get entry's priority
     */
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request
                    (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, &entry_info_p->priority));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Parse all result fields.
     * Note we formerly used "table_p->keys_info[qual_idx].field_id" to get the key qualifiers,
     * but that uses the dbal implementation, so we now use the qualifiers found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    /*
     * DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
     */
    nof_qual_values =
        sizeof(entry_info_p->key_info.qual_info[0].qual_value) /
        sizeof(entry_info_p->key_info.qual_info[0].qual_value[0]);
    for (qual_idx = 0;
         qual_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));

        SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, field_id, &is_ranged));

        if (is_ranged)
        {
            /** Note that at this point we don't support ranges larger then 32 bits.*/
            dbal_entry_handle_key_field_range_get(unit, entry_handle_id, field_id,
                                                  entry_info_p->key_info.qual_info[qual_idx].qual_value,
                                                  entry_info_p->key_info.qual_info[qual_idx].qual_max_value);
            LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s range 0x%x-0x%x.%s\r\n",
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         entry_info_p->key_info.qual_info[qual_idx].qual_value[0],
                         entry_info_p->key_info.qual_info[qual_idx].qual_max_value[0], EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                            (unit, entry_handle_id, field_id,
                             entry_info_p->key_info.qual_info[qual_idx].qual_value,
                             entry_info_p->key_info.qual_info[qual_idx].qual_mask));
            LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s value 0x%s%s%s",
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual), EMPTY,
                         EMPTY, EMPTY);
            for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values; qual_value_ndx++)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                             entry_info_p->key_info.qual_info[qual_idx].qual_value[nof_qual_values - qual_value_ndx -
                                                                                   1], EMPTY, EMPTY, EMPTY);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, " mask 0x%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
            for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values; qual_value_ndx++)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                             entry_info_p->key_info.qual_info[qual_idx].qual_mask[nof_qual_values - qual_value_ndx - 1],
                             EMPTY, EMPTY, EMPTY);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }

    /*
     * Read the actions, removing the actions not found in the entry.
     * Note that formerly we used "table_p->multi_res_info[0]" to get the result fields (actions),
     * but that uses the dbal implementation, so we now use the actions found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Payload:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    /*
     * DNX_DATA_MAX_FIELD_ENTRY_NOF_ACTION_PARAMS_PER_ENTRY
     */
    nof_action_values =
        sizeof(entry_info_p->payload_info.action_info[0].action_value) /
        sizeof(entry_info_p->payload_info.action_info[0].action_value[0]);
    action_entry_idx = 0;
    for (action_fg_idx = 0;
         action_fg_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_fg_idx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_fg_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Action: %s 0x%s%s%s",
                     dnx_field_dnx_action_text(unit, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action),
                     EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action,
                         &field_id));
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                     entry_info_p->payload_info.
                                                     action_info[action_entry_idx].action_value);
        if (rv == _SHR_E_NONE)
        {
            entry_info_p->payload_info.action_info[action_entry_idx].dnx_action =
                entry_info_p->payload_info.action_info[action_fg_idx].dnx_action;
            action_entry_idx++;
            for (action_value_ndx = 0; action_value_ndx < nof_action_values; action_value_ndx++)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                             entry_info_p->payload_info.action_info[action_entry_idx].action_value[nof_action_values -
                                                                                                   action_value_ndx -
                                                                                                   1], EMPTY, EMPTY,
                             EMPTY);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "not found in entry.%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    if (action_entry_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        entry_info_p->payload_info.action_info[action_entry_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* This procedure retrieves the content of an EXEM entry from HW.
* \param [in] unit             -  Device ID
* \param [in] field_stage      -  Field Stage (iPMF1/2...)
* \param [in] fg_id            -  Identifier of the field group. Used for debugging.
* \param [in] dbal_table_id    -  DBAL table id that was created for Field group at fg_create()
* \param [in] entry_key_p      -  The Key of the entry we want to read.
* \param [out] entry_info_p    -  Information extracted from the HW (QUALS & ACTIONS),
*                                 although the quals are already known.
* \return
*   shr_error_e - Error Type
* \remark
*   * The output entry_info_p->key_info should contain the key in HW. If all works well, it should be identical to
*     the input entry_key_p.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_exem_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_key_t * entry_key_p,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 qual_value_ndx;
    uint32 nof_qual_values_in;
    uint32 nof_qual_values_out;
    uint32 action_value_ndx;
    uint32 nof_action_values;
    uint32 action_fg_idx;
    uint32 action_entry_idx;
    dbal_fields_e field_id;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Reading EXEM entry from field group %d.%s%s%s\r\n", fg_id, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Write the key that we want to look for. */
    nof_qual_values_in = sizeof(entry_key_p->qual_info[0].qual_value) / sizeof(entry_key_p->qual_info[0].qual_value[0]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Input Key:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (qual_idx = 0;
         qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         entry_key_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s value 0x%s%s%s",
                     dnx_field_dnx_qual_text(unit, entry_key_p->qual_info[qual_idx].dnx_qual), EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_key_p->qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, entry_key_p->qual_info[qual_idx].qual_value);
        for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values_in; qual_value_ndx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                         entry_key_p->qual_info[qual_idx].qual_value[nof_qual_values_in - qual_value_ndx - 1], EMPTY,
                         EMPTY, EMPTY);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    /** Get the entry.*/
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    /*
     * Parse all result fields.
     * Note we formerly used "table_p->keys_info[qual_idx].field_id" to get the key qualifiers,
     * but that uses the dbal implmentation, so we now use the qualifiers found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    nof_qual_values_out =
        sizeof(entry_info_p->key_info.qual_info[0].qual_value) /
        sizeof(entry_info_p->key_info.qual_info[0].qual_value[0]);
    for (qual_idx = 0;
         entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID
         && qual_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; qual_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s value 0x%s%s%s",
                     dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual), EMPTY, EMPTY,
                     EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, field_id, entry_info_p->key_info.qual_info[qual_idx].qual_value));
        for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values_out; qual_value_ndx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                         entry_info_p->key_info.qual_info[qual_idx].qual_value[nof_qual_values_out - qual_value_ndx -
                                                                               1], EMPTY, EMPTY, EMPTY);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

    /*
     * Read the actions, removing the actions not found in the entry.
     * Note that formerly we used "table_p->multi_res_info[0]" to get the result fields (actions),
     * but that uses the dbal implmentation, so we now use the actions found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Payload:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    nof_action_values =
        sizeof(entry_info_p->payload_info.action_info[0].action_value) /
        sizeof(entry_info_p->payload_info.action_info[0].action_value[0]);
    action_entry_idx = 0;
    for (action_fg_idx = 0;
         action_fg_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_fg_idx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_fg_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Action: %s 0x%s%s%s",
                     dnx_field_dnx_action_text(unit, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action),
                     EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action,
                         &field_id));
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                     entry_info_p->payload_info.
                                                     action_info[action_entry_idx].action_value);
        if (rv == _SHR_E_NONE)
        {
            entry_info_p->payload_info.action_info[action_entry_idx].dnx_action =
                entry_info_p->payload_info.action_info[action_fg_idx].dnx_action;
            action_entry_idx++;
            for (action_value_ndx = 0; action_value_ndx < nof_action_values; action_value_ndx++)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                             entry_info_p->payload_info.action_info[action_entry_idx].action_value[nof_action_values -
                                                                                                   action_value_ndx -
                                                                                                   1], EMPTY, EMPTY,
                             EMPTY);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "not found in entry.%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    if (action_entry_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        entry_info_p->payload_info.action_info[action_entry_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* This procedure retrieves the content of an MDB DT entry from HW.
* \param [in] unit             -  Device ID
* \param [in] field_stage      -  Field Stage (iPMF1/2...)
* \param [in] fg_id            -  Identifier of the field group. Used for debugging.
* \param [in] dbal_table_id    -  DBAL table id that was created for Field group at fg_create()
* \param [in] entry_key_p      -  The Key of the entry we want to read.
* \param [out] entry_info_p    -  Information extracted from the HW (QUALS & ACTIONS),
*                                 although the quals are already known.
* \return
*   shr_error_e - Error Type
* \remark
*   * The output entry_info_p->key_info should contain the key in HW. If all works well, it should be identical to
*     the input entry_key_p.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_mdb_dt_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_key_t * entry_key_p,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 qual_value_ndx;
    uint32 nof_qual_values_in;
    uint32 nof_qual_values_out;
    uint32 action_value_ndx;
    uint32 nof_action_values;
    uint32 action_fg_idx;
    uint32 action_entry_idx;
    dbal_fields_e field_id;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Reading MDB DT entry from field group %d.%s%s%s\r\n", fg_id, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /** Write the key that we want to look for. */
    nof_qual_values_in = sizeof(entry_key_p->qual_info[0].qual_value) / sizeof(entry_key_p->qual_info[0].qual_value[0]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Input Key:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    for (qual_idx = 0;
         qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         entry_key_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s value 0x%s%s%s",
                     dnx_field_dnx_qual_text(unit, entry_key_p->qual_info[qual_idx].dnx_qual), EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_key_p->qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, entry_key_p->qual_info[qual_idx].qual_value);
        for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values_in; qual_value_ndx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                         entry_key_p->qual_info[qual_idx].qual_value[nof_qual_values_in - qual_value_ndx - 1], EMPTY,
                         EMPTY, EMPTY);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    /** Get the entry.*/
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    /*
     * Parse all result fields.
     * Note we formerly used "table_p->keys_info[qual_idx].field_id" to get the key qualifiers,
     * but that uses the dbal implmentation, so we now use the qualifiers found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    nof_qual_values_out =
        sizeof(entry_info_p->key_info.qual_info[0].qual_value) /
        sizeof(entry_info_p->key_info.qual_info[0].qual_value[0]);
    for (qual_idx = 0;
         entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID
         && qual_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; qual_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qual: %s value 0x%s%s%s",
                     dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual), EMPTY, EMPTY,
                     EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, field_id, entry_info_p->key_info.qual_info[qual_idx].qual_value));
        for (qual_value_ndx = 0; qual_value_ndx < nof_qual_values_out; qual_value_ndx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                         entry_info_p->key_info.qual_info[qual_idx].qual_value[nof_qual_values_out - qual_value_ndx -
                                                                               1], EMPTY, EMPTY, EMPTY);
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }

    /*
     * Read the actions, removing the actions not found in the entry.
     * Note that formerly we used "table_p->multi_res_info[0]" to get the result fields (actions),
     * but that uses the dbal implmentation, so we now use the actions found in the field group
     * (and not its dbal table).
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Payload:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    nof_action_values =
        sizeof(entry_info_p->payload_info.action_info[0].action_value) /
        sizeof(entry_info_p->payload_info.action_info[0].action_value[0]);
    action_entry_idx = 0;
    for (action_fg_idx = 0;
         action_fg_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && entry_info_p->payload_info.action_info[action_fg_idx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_fg_idx++)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Action: %s 0x%s%s%s",
                     dnx_field_dnx_action_text(unit, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action),
                     EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, entry_info_p->payload_info.action_info[action_fg_idx].dnx_action,
                         &field_id));
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                     entry_info_p->payload_info.
                                                     action_info[action_entry_idx].action_value);
        if (rv == _SHR_E_NONE)
        {
            entry_info_p->payload_info.action_info[action_entry_idx].dnx_action =
                entry_info_p->payload_info.action_info[action_fg_idx].dnx_action;
            action_entry_idx++;
            for (action_value_ndx = 0; action_value_ndx < nof_action_values; action_value_ndx++)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                             entry_info_p->payload_info.action_info[action_entry_idx].action_value[nof_action_values -
                                                                                                   action_value_ndx -
                                                                                                   1], EMPTY, EMPTY,
                             EMPTY);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else if (rv == _SHR_E_NOT_FOUND)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "not found in entry.%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    if (action_entry_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        entry_info_p->payload_info.action_info[action_entry_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Gets the given data from the given address in the State Table Memory.
*
* \param [in] unit    - Device ID
* \param [in] address - Address to read from
* \param [out] data_p - Data to get from the given address
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_state_table_hw_get(
    int unit,
    uint32 address,
    uint32 *data_p)
{
    uint32 entry_handle_id;
    int data_size_entry_max = dnx_data_field.state_table.data_size_entry_max_get(unit);
    int data_size = dnx_data_field.state_table.data_size_get(unit);
    uint32 hw_address;
    uint32 hw_data;
    int address_size_entry_max;
    int address_size;
    int lsb_address_size;
    uint32 lsb_address;
    int new_data_offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Initializations to silence the compiler.
     */
    lsb_address_size = -1;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FIELD_STATE_TABLE, &entry_handle_id));

    if (data_size != data_size_entry_max)
    {
        /*
         * Use the HW part of the address.
         */
        address_size_entry_max = dnx_data_field.state_table.address_size_entry_max_get(unit);
        address_size = dnx_data_field.state_table.address_size_get(unit);
        lsb_address_size = address_size - address_size_entry_max;

        /*
         * Sanity check, as it was checked during init.
         */
        if (data_size > data_size_entry_max || address_size_entry_max > address_size ||
            address_size >= SAL_UINT32_NOF_BITS || data_size_entry_max >= SAL_UINT32_NOF_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported state table DNX DATA values. "
                         "data_size_entry_max %d data_size %d address_size_entry_max %d address_size %d.\r\n",
                         data_size_entry_max, data_size, address_size_entry_max, address_size);
        }

        hw_address = address >> lsb_address_size;
    }
    else
    {
        hw_address = address;
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_STATE_TABLE_ADDRESS_HW, hw_address);

    /** Get the entry.*/
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_STATE_TABLE_DATA_HW, INST_SINGLE, &hw_data));

    if (data_size != data_size_entry_max)
    {
        /*
         * Read the relevant part of the hw_data
         */
        lsb_address = address & (SAL_UPTO_BIT(lsb_address_size));
        new_data_offset = lsb_address * data_size;
        (*data_p) = (hw_data >> new_data_offset) & (SAL_UPTO_BIT(data_size));
    }
    else
    {
        (*data_p) = hw_data;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the action types from ACE sw state
* \param [in] unit             - Device ID
* \param [in] ace_id           - ACE ID
* \param [out] payload_info_p  - Holds the pointer to action types.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_entry_actions_type_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_entry_payload_t * payload_info_p)
{
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int action_idx;
    unsigned int nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_ace_format_dnx_action_arr_get(unit, ace_id, dnx_actions, &nof_actions));

    /*
     * Verify the sizes of the array. Can be remove if we trust the SW state.
     */
    if (nof_actions > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of actions (%d) exceeds maximum (%d).\r\n",
                     nof_actions, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }

    for (action_idx = 0; action_idx < nof_actions; action_idx++)
    {
        payload_info_p->action_info[action_idx].dnx_action = dnx_actions[action_idx];
    }

    if (action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        payload_info_p->action_info[action_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* This procedure retrieves the content of an ACE entry from HW.
* \param [in] unit              -  Device ID
* \param [in] ace_entry_key     -  The Key of the entry we want to read.
* \param [out] ace_id_p         -  Pointer to the ACE format ID associated with the entry.
* \param [out] payload_info_p   -  Information extracted from the HW about the actions in payload.
*                                  If NULL not filled.
* \return
*   shr_error_e - Error Type
* \remark
*   * The output entry_info_p->key_info should contain the key in HW. If all works well, it should be identical to
*     the input entry_key_p.
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_entry_hw_get(
    int unit,
    dnx_field_ace_key_t ace_entry_key,
    dnx_field_ace_id_t * ace_id_p,
    dnx_field_entry_payload_t * payload_info_p)
{
    uint32 entry_handle_id;
    uint32 read_qual_value;
    uint32 result_type_index;
    dbal_fields_e field_id;
    uint32 action_value_ndx;
    uint32 nof_action_values;
    uint32 action_fg_idx;
    uint32 action_entry_idx;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_id_p, _SHR_E_PARAM, "ace_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Reading EXEM entry key 0x%x.%s%s%s\r\n", ace_entry_key, EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, &entry_handle_id));

    /** Write the key that we want to look for. */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_CUD, ace_entry_key);

    /*
     * Get the entry.
     * Note that we get all fields and verify the result type index.
     * We could have gotten the payload directly by providing the result type index.
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Obtain result_type_index.*/
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type_index));
    /** Obtain ACE ID*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_result_type_index_to_ace_id(unit, result_type_index, ace_id_p));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Entry is in ACE format ID: %d. result_type_index %d.%s%s\r\n",
                 *ace_id_p, result_type_index, EMPTY, EMPTY);

    /*
     * Continue reading the entry only if payload_info_p isn't NULL.
     */
    if (payload_info_p != NULL)
    {
        /** Read the actions from the ACE format.*/
        SHR_IF_ERR_EXIT(dnx_field_ace_entry_actions_type_get(unit, *ace_id_p, payload_info_p));

        /** Sanity check: Verify that the key returned is the key we're expecting to read.*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_PP_CUD, &read_qual_value));
        if (read_qual_value != ace_entry_key)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Attempted to read entry with key %u, but read entry with key %u.\n",
                         ace_entry_key, read_qual_value);
        }

        /*
         * Read the payload, removing the actions not found in the entry.
         */
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Payload:%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        nof_action_values =
            sizeof(payload_info_p->action_info[0].action_value) /
            sizeof(payload_info_p->action_info[0].action_value[0]);
        action_entry_idx = 0;
        for (action_fg_idx = 0;
             action_fg_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
             && payload_info_p->action_info[action_fg_idx].dnx_action != DNX_FIELD_ACTION_INVALID; action_fg_idx++)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Action: %s 0x%s%s%s",
                         dnx_field_dnx_action_text(unit, payload_info_p->action_info[action_fg_idx].dnx_action), EMPTY,
                         EMPTY, EMPTY);
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                            (unit, DNX_FIELD_STAGE_ACE, payload_info_p->action_info[action_fg_idx].dnx_action,
                             &field_id));
            rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                         payload_info_p->action_info[action_entry_idx].action_value);
            if (rv == _SHR_E_NONE)
            {
                payload_info_p->action_info[action_entry_idx].dnx_action =
                    payload_info_p->action_info[action_fg_idx].dnx_action;
                action_entry_idx++;
                for (action_value_ndx = 0; action_value_ndx < nof_action_values; action_value_ndx++)
                {
                    LOG_DEBUG_EX(BSL_LOG_MODULE, "%.8x%s%s%s",
                                 payload_info_p->action_info[action_value_ndx].action_value[nof_action_values -
                                                                                            action_value_ndx - 1],
                                 EMPTY, EMPTY, EMPTY);
                }
                LOG_DEBUG_EX(BSL_LOG_MODULE, ".%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
            }
            else if (rv == _SHR_E_NOT_FOUND)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE, "not found in entry.%s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
        }
        if (action_entry_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
        {
            payload_info_p->action_info[action_entry_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear a TCAM entry from HW.
* \param [in] unit           - Device ID
* \param [in] field_stage    - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id  - DBAL table id to clear its entry
* \param [in] key_info_p     - Key information for DT entries
* \param [in] access_id      - entry access_
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_hw_clear(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_key_t * key_info_p,
    uint32 access_id)
{
    uint32 entry_handle_id;
    uint32 qual_idx = 0;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    if (key_info_p != NULL)
    {
        for (qual_idx = 0;
             (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
             (key_info_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                           key_info_p->qual_info[qual_idx].dnx_qual, &field_id));
            dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id,
                                                  key_info_p->qual_info[qual_idx].qual_value,
                                                  key_info_p->qual_info[qual_idx].qual_mask);

        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear an EXEM entry from HW.
* \param [in] unit           - Device ID.
* \param [in] field_stage    - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id  - DBAL table id to clear its entry.
* \param [in] entry_key_p    - The key of the entry to be deleted.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_exem_hw_clear(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_key_t * entry_key_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    for (qual_idx = 0;
         qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         entry_key_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_key_p->qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, entry_key_p->qual_info[qual_idx].qual_value);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear an MDB DT entry from HW.
* \param [in] unit           - Device ID.
* \param [in] field_stage    - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id  - DBAL table id to clear its entry.
* \param [in] entry_key_p    - The key of the entry to be deleted.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_mdb_dt_hw_clear(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_key_t * entry_key_p)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    dbal_fields_e field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    for (qual_idx = 0;
         qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
         && entry_key_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, field_stage, entry_key_p->qual_info[qual_idx].dnx_qual, &field_id));
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id, entry_key_p->qual_info[qual_idx].qual_value);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear an ACE entry from HW.
* \param [in] unit           - Device ID.
* \param [in] ace_entry_key  - The key of the entry to be deleted.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_entry_hw_clear(
    int unit,
    dnx_field_ace_key_t ace_entry_key)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_CUD, ace_entry_key);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Create an ID and finds place in TCAM at the given core to for the new entry
* \param [in] unit                - Device ID
* \param [in] flags               - dnx_field_entry_add_flags_e flags.
* \param [in] core                - Core ID
* \param [in] fg_id               - Field Group Id that the access ID created for
* \param [in] tcam_handler_id     - TCAM handler of the Field group
* \param [in] priority            - Entry priority
* \param [out] entry_access_id_p  -  Unique allocated handle for TCAM entry (see encoding in DNX_FIELD_TCAM_ENTRY)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_access_id_create(
    int unit,
    uint32 flags,
    int core,
    dnx_field_group_t fg_id,
    uint32 tcam_handler_id,
    uint32 priority,
    uint32 *entry_access_id_p)
{
    dnx_field_tcam_entry_t entry;
    int access_id = 0;
    int alloc_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_access_id_p, _SHR_E_PARAM, "entry_access_id_p");

    if (flags & DNX_FIELD_GROUP_ADD_FLAG_WITH_ID)
    {
        alloc_flags |= DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
        access_id = DNX_FIELD_ENTRY_ACCESS_ID(*entry_access_id_p);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_entry_tcam_access_id_allocate(unit, alloc_flags, &access_id));

    /*
     *   Encode internal TCAM entry id per Field Group ID and Access Id
     */
    *entry_access_id_p = DNX_FIELD_TCAM_ENTRY(access_id, core);

    entry.id = *entry_access_id_p;
    entry.priority = priority;

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location_alloc(unit, core, tcam_handler_id, &entry));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_access_id_create(
    int unit,
    int core,
    dbal_tables_e dbal_table_id,
    uint32 priority,
    uint32 *entry_access_id_p)
{
    uint32 tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_access_id_p, _SHR_E_PARAM, "entry_access_id_p");

    if (core == DBAL_CORE_ALL)
    {
        core = DNX_FIELD_TCAM_CORE_ALL;
    }

    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_access_id_create(unit, 0, core, DNX_DATA_MAX_FIELD_GROUP_NOF_FGS,
                                                          tcam_handler_id, priority, entry_access_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function create Id and find a place for the id to be put in HW at the given core.
*  Only used for TCAM field groups.
*
* \param [in] unit                - Device Id
* \param [in] flags               - dnx_field_entry_add_flags_e flags.
* \param [in] core                - Core ID
* \param [in] fg_type             - Field Group type, look at dnx_field_group_type_e
* \param [in] fg_id               - Field Group to create access_id for
* \param [in] priority            - Entry priority, see dnx_field_tcam_entry_t
* \param [out] entry_access_id_p  - Access_id that was allocated for this entry
* \return
*   shr_error_e - Error Type
* \remark
*   * none
* \see
*   * DNX_FIELD_TCAM_ENTRY() to see how entry_access_id_p is encoded
*/
static shr_error_e
dnx_field_entry_pmf_access_id_create(
    int unit,
    uint32 flags,
    int core,
    dnx_field_group_type_e fg_type,
    dnx_field_group_t fg_id,
    uint32 priority,
    uint32 *entry_access_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_access_id_p, _SHR_E_PARAM, "entry_access_id_p");

    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
    {
        uint32 tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_access_id_create
                        (unit, flags, core, fg_id, tcam_handler_id, priority, entry_access_id_p));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "This function does not support FG type %d\r\n", fg_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function allocates an ACE key for an ACE entry. Only allocates ACE pointers, no other keys.
* \param [in] unit               - Device Id
* \param [in] flags              - dnx_field_entry_add_flags_e flags.
* \param [in] ace_id             - ACE ID.
* \param [in,out] ace_key_p      - The key that was allocated. Input if WITH_ID flag is set.
* \return
*   shr_error_e - Error Type
* \remark
*   * none
* \see
*   * dnx_field_ace_entry_add()
*/
static shr_error_e
dnx_field_ace_entry_key_allocate(
    int unit,
    uint32 flags,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_key_t * ace_key_p)
{
    int alloc_flag;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_key_p, _SHR_E_PARAM, "ace_id_p");

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID)
    {
        alloc_flag = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
    }
    else
    {
        alloc_flag = 0;
    }
    SHR_IF_ERR_EXIT(dnx_algo_field_ace_key_allocate(unit, alloc_flag, ace_key_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the action types and qual types from FG sw state
* \param [in] unit             - Device ID
* \param [in] fg_id            - Field Group ID
* \param [out] entry_info_p    - Holds the pointer to action and qualifier types
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_quals_actions_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int qual_idx;
    unsigned int action_idx;
    unsigned int nof_quals;
    unsigned int nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals, &nof_quals));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_action_arr_get(unit, fg_id, dnx_actions, &nof_actions));

    /*
     * Verify the sizes of the arrays. Can be remove if we trust the SW state.
     */
    if (nof_quals > DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of qualifiers (%d) exceeds maximum (%d).\r\n",
                     nof_quals, DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);
    }
    if (nof_actions > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of actions (%d) exceeds maximum (%d).\r\n",
                     nof_actions, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }

    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        entry_info_p->key_info.qual_info[qual_idx].dnx_qual = dnx_quals[qual_idx];
    }

    if (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        entry_info_p->key_info.qual_info[qual_idx].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    for (action_idx = 0; action_idx < nof_actions; action_idx++)
    {
        entry_info_p->payload_info.action_info[action_idx].dnx_action = dnx_actions[action_idx];
    }

    if (action_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        entry_info_p->payload_info.action_info[action_idx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_access_id_destroy(
    int unit,
    dbal_tables_e dbal_table_id,
    uint32 entry_access_id)
{
    uint32 tcam_handler_id;
    uint32 access_id;
    SHR_FUNC_INIT_VARS(unit);

    access_id = DNX_FIELD_ENTRY_ACCESS_ID(entry_access_id);

    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));

    SHR_IF_ERR_EXIT(dnx_algo_field_entry_tcam_access_id_deallocate(unit, access_id));

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location_free(unit, tcam_handler_id, entry_access_id));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_access_id_destroy_all(
    int unit,
    dbal_tables_e dbal_table_id)
{
    uint32 entry_iter;
    uint32 next_iter;
    uint32 tcam_handler_id;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));
    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, tcam_handler_id, &entry_iter));
        while (entry_iter != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {
            /*
             * Old trick of saving next iterator before deleteing the current one
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next(unit, core, tcam_handler_id, entry_iter, &next_iter));

            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, dbal_table_id, entry_iter));

            entry_iter = next_iter;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_delete_all(
    int unit,
    dnx_field_group_t fg_id)
{
    uint32 tcam_handler_id;
    uint32 entry_iter;
    uint32 next_iter;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
    for (core = 0; core < dnx_data_device.general.nof_cores_get(unit); core++)
    {
        /*
         * TCAM handler provides an iterator to loop on all entries for the field group
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, tcam_handler_id, &entry_iter));
        while (entry_iter != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {
            /*
             * Old trick of saving next iterator before deleteing the current one
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next(unit, core, tcam_handler_id, entry_iter, &next_iter));

            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id, entry_iter, NULL));

            entry_iter = next_iter;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_state_table_all_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FIELD_STATE_TABLE));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Parses the given entry key info (qualifiers) to their corresponding State Table Address/Data fields.
*
*  For example, in J2:
*   The address/data fields are part of the State Table key (which is 19b long).
*   The address comes first with 10b length, and the data comes after with 8b length.
*   The qualifiers may compose the address/data fields in any given way, it might be also possible
*   that a certain qualifier may be mutual to both address and data fields.
*   This function ignores the Do-Write bit.
*
* Example of 3 qualifiers (Qual1-3) composing the 18b of the address/data qualifiers in J2:
*  +==========================================================+
*  | Qual1   |         Qual2         |         Qual3          |
*  +----------------------------------------------------------+
*  ^                              ^                           ^
*  0         Address              10            Data          17
*
* \param [in] unit             - Device ID
* \param [in] stage            - Stage of the qualifiers
* \param [in] entry_key_info_p - Entry key info
* \param [out] address_p       - The parsed address value from the qualifiers
* \param [out] data_p          - The parsed data value from the qualifiers
*
* \return
*   shr_error_e - Error Type
* \remark
*   * This function ignores the Do-Write bit.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_state_table_fields_parse(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_entry_key_t * entry_key_info_p,
    uint32 *address_p,
    uint32 *data_p)
{
    int qual_idx;
    uint32 lsb_field_value = 0;
    uint32 msb_field_value = 0;
    int bits_processed = 0;
    int lsb_field_size;
    int lsb_field_left;
    int msb_field_left;
    int state_table_uses_rmw = dnx_data_field.features.state_table_atomic_rmw_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");
    SHR_NULL_CHECK(address_p, _SHR_E_PARAM, "address_p");

    /*
     * For RMW, the argument comes before the address.
     */
    if (state_table_uses_rmw)
    {
        lsb_field_left = dnx_data_field.state_table.data_size_get(unit);
        msb_field_left = dnx_data_field.state_table.address_size_get(unit);
    }
    else
    {
        lsb_field_left = dnx_data_field.state_table.address_size_get(unit);
        msb_field_left = dnx_data_field.state_table.data_size_get(unit);
    }
    lsb_field_size = lsb_field_left;

    for (qual_idx = 0;
         qual_idx < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP
         && entry_key_info_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        uint32 qual_size;
        int qual_val = entry_key_info_p->qual_info[qual_idx].qual_value[0];

        if (lsb_field_left < 0 || msb_field_left < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached negative bits left "
                         "(lsb_field_left %d msb_field_left %d state_table_uses_rmw %d).\r\n",
                         lsb_field_left, msb_field_left, state_table_uses_rmw);
        }

        if (lsb_field_left == 0 && msb_field_left == 0)
        {
            break;
        }

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, stage, entry_key_info_p->qual_info[qual_idx].dnx_qual, &qual_size));

        if (((int) qual_size) <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected qual size %d. Must be at least 1.\r\n", qual_size);
        }

        if (qual_size <= lsb_field_left)
        {
            /*
             * Address parsing
             */
            lsb_field_value |= (qual_val << bits_processed);
            lsb_field_left -= qual_size;
        }
        else if (lsb_field_left > 0)
        {
            /*
             * Mutual qual, address and data parsing
             */
            lsb_field_value |= ((qual_val & SAL_UPTO_BIT(lsb_field_left)) << bits_processed);
            msb_field_value |= (qual_val >> lsb_field_left);

            msb_field_left -= (qual_size - lsb_field_left);
            lsb_field_left = 0;
        }
        else if (qual_size <= msb_field_left)
        {
            /*
             * Data parsing
             */
            msb_field_value |= (qual_val << (bits_processed - lsb_field_size));
            msb_field_left -= qual_size;

        }
        else if (msb_field_left > 0)
        {
            /*
             * Data parsing
             */
            msb_field_value |= ((qual_val & SAL_UPTO_BIT(msb_field_left)) << (bits_processed - lsb_field_size));
            if (qual_size <= msb_field_left)
            {
                msb_field_left -= qual_size;
            }
            else
            {
                msb_field_left = 0;
            }
        }
        bits_processed += qual_size;
    }

    if (state_table_uses_rmw)
    {
        *address_p = msb_field_value;
        if (data_p)
        {
            *data_p = lsb_field_value;
        }
    }
    else
    {
        *address_p = lsb_field_value;
        if (data_p)
        {
            *data_p = msb_field_value;
        }
    }

    if ((*address_p) > dnx_data_field.state_table.address_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "State Table address out of range: %d (max %d).\r\n",
                     (*address_p), dnx_data_field.state_table.address_max_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Combines the given address/data fields of a State Table FG and outputs the State Table key info (qualifiers)
*
*  For example, in J2:
*   The address/data fields are part of the State Table key (which is 19b long).
*   The address comes first with 10b length, and the data comes after with 8b length.
*   The qualifiers may compose the address/data fields in any given way, it might be also possible
*   that a certain qualifier may be mutual to both address and data fields.
*   This function ignores the Do-Write bit.
*
* Example of 3 qualifiers (Qual1-3) composing the 18b of the address/data qualifiers in J2:
*  +==========================================================+
*  | Qual1   |         Qual2         |         Qual3          |
*  +----------------------------------------------------------+
*  ^                              ^                           ^
*  0         Address              10            Data          17
*
* \param [in] unit              - Device ID
* \param [in] stage             - Stage of the qualifiers
* \param [in] address           - The State Table address to combine with the data and output the qualifiers
* \param [in] data              - The State Table data to combine with the address and output the qualifiers
* \param [out] entry_key_info_p - The State Table key info (qualifiers) that are the result of the combination
*
* \return
*   shr_error_e - Error Type
* \remark
*   * This function ignores the Do-Write bit.
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_state_table_fields_combine(
    int unit,
    dnx_field_stage_e stage,
    uint32 address,
    uint32 data,
    dnx_field_entry_key_t * entry_key_info_p)
{
    int lsb_field_left;
    int msb_field_left;
    uint32 lsb_field_value;
    uint32 msb_field_value;
    int bits_processed = 0;
    int state_table_uses_rmw = dnx_data_field.features.state_table_atomic_rmw_get(unit);
    dnx_field_entry_qual_t *qual_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");

    /*
     * For RMW, the argument comes before the address.
     */
    if (state_table_uses_rmw)
    {
        lsb_field_left = dnx_data_field.state_table.data_size_get(unit);
        msb_field_left = dnx_data_field.state_table.address_size_get(unit);
        lsb_field_value = data;
        msb_field_value = address;
    }
    else
    {
        lsb_field_left = dnx_data_field.state_table.address_size_get(unit);
        msb_field_left = dnx_data_field.state_table.data_size_get(unit);
        lsb_field_value = address;
        msb_field_value = data;
    }

    for (qual_p = &(entry_key_info_p->qual_info[0]); qual_p->dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_p++)
    {
        uint32 qual_size;

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, stage, qual_p->dnx_qual, &qual_size));

        if (qual_size >= SAL_UINT32_NOF_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Qualifier \"%s\" has size %d, maximum %d.\r\n",
                         dnx_field_dnx_qual_text(unit, qual_p->dnx_qual), qual_size, SAL_UINT32_NOF_BITS - 1);
        }

        if (lsb_field_left > 0)
        {
            /*
             * Address translation into qualifiers
             */
            qual_p->qual_value[0] = lsb_field_value & SAL_UPTO_BIT(qual_size);
            if (qual_size > lsb_field_left)
            {
                /*
                 * Mutual Qualifier
                 */
                int relevant_qual_size;

                if (qual_size > lsb_field_left + msb_field_left)
                {
                    relevant_qual_size = lsb_field_left + msb_field_left;
                }
                else
                {
                    relevant_qual_size = qual_size;
                }

                qual_p->qual_value[0] |= msb_field_value << lsb_field_left;
                msb_field_left -= (relevant_qual_size - lsb_field_left);
                msb_field_value = msb_field_value >> (relevant_qual_size - lsb_field_left);
                lsb_field_left = 0;
            }
            else
            {
                lsb_field_left -= qual_size;
                lsb_field_value = lsb_field_value >> qual_size;
            }
        }
        else if (msb_field_left > 0)
        {
            /*
             * Data translation into qualifiers
             */
            qual_p->qual_value[0] = msb_field_value & SAL_UPTO_BIT(qual_size);

            if (qual_size > msb_field_left)
            {
                msb_field_left = 0;
                msb_field_value = 0;
            }
            else
            {
                msb_field_left -= qual_size;
                msb_field_value = msb_field_value >> qual_size;
            }
        }

        bits_processed += qual_size;
    }
    if (msb_field_left > 0)
    {
        /*
         * Not enough qualifiers to translate the all of the given data
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Qualifier list given for State Table FG doesn't comply with address/data length: Qualifiers list length less than %d bits\r\n",
                     dnx_data_field.state_table.address_size_get(unit) +
                     dnx_data_field.state_table.data_size_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Valid bit choice verification
* \param [in] unit             - Device ID
* \param [in] fg_id            - Field Group ID
* \param [in] entry_info_p     - Entry info to configure into the database.
* \return
*   shr_error_e - Error Type
* \remark
*   * Because the main purpose of the function is to verify that actions without valid bits that aren't void actions
*     appear in every entry, if we disallow not having a valid bit, this function is disabled
*     by DNX_FIELD_ALLOW_VALID_BIT_CHOICE_FOR_LOOKUP_FG.
* \see
*   * dnx_field_entry_tcam_add
*   * dnx_field_entry_tcam_dt_add
*   * dnx_field_entry_kbp_add
*   * dnx_field_entry_mdb_dt_add
*   * dnx_field_entry_exem_add
*/

static shr_error_e
dnx_field_entry_valid_bit_choice_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
#if (DNX_FIELD_ALLOW_VALID_BIT_CHOICE_FOR_LOOKUP_FG)
    unsigned int action_fg_ndx;
    unsigned int action_entry_ndx;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    int action_found;
    dnx_field_stage_e field_stage;
    int action_is_void;
#endif
    SHR_FUNC_INIT_VARS(unit);

#if (DNX_FIELD_ALLOW_VALID_BIT_CHOICE_FOR_LOOKUP_FG)
    /*
     * Verify that every action without valid bit appears in the entry (unless void action).
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    for (action_fg_ndx = 0;
         action_fg_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_payload_info.actions_on_payload_info[action_fg_ndx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_fg_ndx++)
    {
        if (actions_payload_info.actions_on_payload_info[action_fg_ndx].dont_use_valid_bit == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                            (unit, field_stage,
                             actions_payload_info.actions_on_payload_info[action_fg_ndx].dnx_action, &action_is_void));
            if (action_is_void == FALSE)
            {
                action_found = FALSE;
                for (action_entry_ndx = 0;
                     action_entry_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
                     && entry_info_p->payload_info.action_info[action_entry_ndx].dnx_action != DNX_FIELD_ACTION_INVALID;
                     action_entry_ndx++)
                {
                    if (entry_info_p->payload_info.action_info[action_entry_ndx].dnx_action ==
                        actions_payload_info.actions_on_payload_info[action_fg_ndx].dnx_action)
                    {
                        action_found = TRUE;
                        break;
                    }
                }
                if (action_found == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Action %s in field group %d doen't have a valid bit and isn't a "
                                 "void action, so it must appear in enery entry.\r\n",
                                 dnx_field_dnx_action_text(unit,
                                                           actions_payload_info.actions_on_payload_info[0]
                                                           [action_fg_ndx].dnx_action), fg_id);
                }
            }
        }
    }
#endif
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_entry_tcam_add().
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_add_flags_e flags.
* \param [in] fg_id            - Field Group ID
* \param [in] entry_info_p     - Entry info to configure into the database.
* \param [in] entry_handle_p   - The entry handle ID to be filled by the calling function.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_entry_tcam_add
*/
static shr_error_e
dnx_field_entry_tcam_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    uint8 is_allocated;
    unsigned int qual_ndx, action_ndx;
    dnx_field_group_type_e fg_type;
    int dnx_core;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG];
    uint32 unsupported_flags;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");
    SHR_NULL_CHECK(entry_handle_p, _SHR_E_PARAM, "entry_handle_p");

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /*
     * Verify that the field group exists (its ID is allocated).
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group ID %d isn't allocated.\r\n", fg_id);
    }
    if ((unsupported_flags = (flags & ~(DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID | DNX_FIELD_ENTRY_ADD_FLAG_UPDATE))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }
    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID && flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag UPDATE can't be used together with flag WITH_ID\n");
    }

    /*
     * Verify core
     */
    if ((entry_info_p->core < 0 || entry_info_p->core > dnx_data_device.general.nof_cores_get(unit) - 1)
        && entry_info_p->core != _SHR_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core ID %d is not valid. available range is (0:%d) or _SHR_CORE_ALL \n",
                     entry_info_p->core, dnx_data_device.general.nof_cores_get(unit) - 1);
    }

    dnx_core = (entry_info_p->core == _SHR_CORE_ALL) ? DNX_FIELD_TCAM_CORE_ALL : entry_info_p->core;

    /*
     * Verify that priority is valid
     * the max_tcam_priority is used internally so it can't be used by user.
     * And the (-1) comes from the fact that we add 1 to our TCAM priorities to avoid
     * checking limits each time.
     */
    if (entry_info_p->priority == DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID
        || entry_info_p->priority >= dnx_data_field.tcam.max_tcam_priority_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group ID %d has entry with invalid priority(%d): (valid range 0-%d).\r\n",
                     fg_id, entry_info_p->priority, dnx_data_field.tcam.max_tcam_priority_get(unit) - 2);
    }

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID)
    {
        /** Validate entry handle's params */
        uint32 clean_entry_handle;
        uint32 access_id = DNX_FIELD_ENTRY_ACCESS_ID(*entry_handle_p);
        int handle_core = DNX_FIELD_TCAM_ENTRY_CORE_ID(*entry_handle_p);

        if ((handle_core != dnx_core))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry add WITH_ID failed: Given entry handle has incorrect core ID (%d). Entry belongs to core ID (%d)\n",
                         handle_core, dnx_core);
        }
        if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
        {
            uint8 is_allocated;
            SHR_IF_ERR_EXIT(dnx_algo_field_entry_tcam_access_id_is_allocated(unit, access_id, &is_allocated));
            if (is_allocated)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Entry add WITH_ID failed: Given entry handle is already allocated\n");
            }
        }
        /** Validate entry handle has none of the reserved bits set */
        /*
         * Create clean entry
         */
        clean_entry_handle = DNX_FIELD_ENTRY(DNX_FIELD_TCAM_INTERNAL, access_id, 0, dnx_core);
        /*
         * Compare given handle to the clean one
         */
        if (clean_entry_handle != *entry_handle_p)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry add WITH_ID failed: Given entry handle has some of its reserved bits turned on\n");
        }
    }

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        dnx_field_entry_t existing_entry_info;
        /** Verify entry exists and const params are not changed */
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get(unit, fg_id, *entry_handle_p, &existing_entry_info));
        if (existing_entry_info.core != entry_info_p->core)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Update request failed: Core ID for entry can't be changed, old (%d) new (%d)\n",
                         existing_entry_info.core, entry_info_p->core);
        }
        if (existing_entry_info.priority != entry_info_p->priority)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Update request failed: Priority for entry can't be changed, old (%d) new (%d)\n",
                         existing_entry_info.priority, entry_info_p->priority);
        }
    }
    /*
     * Verify that the qualifiers that appear in the entry also appear in the field group.
     */
    for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
    {
        dnx_quals[qual_ndx] = entry_info_p->key_info.qual_info[qual_ndx].dnx_qual;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_group_qual_arr_subset_verify(unit, fg_id, dnx_quals),
                             "TCAM Table entry qualifiers must be subset of qualifiers in the field group they "
                             "belong to.\r\n%s%s%s", EMPTY, EMPTY, EMPTY);
    /*
     * Verify that the actions that appear in the entry also appear in the field group.
     */
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        dnx_actions[action_ndx] = entry_info_p->payload_info.action_info[action_ndx].dnx_action;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_group_action_arr_subset_verify(unit, fg_id, dnx_actions),
                             "TCAM Table entry actions must be subset of actions in the field group they "
                             "belong to.\r\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_entry_valid_bit_choice_verify(unit, fg_id, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_entry_tcam_dt_add().
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_add_flags_e flags.
* \param [in] fg_id            - Field Group ID
* \param [in] entry_info_p     - Entry info to configure into the database.
* \param [in] entry_handle_p   - The entry handle ID to be filled by the calling function.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_entry_tcam_add
*/
static shr_error_e
dnx_field_entry_tcam_dt_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    uint8 is_allocated;
    unsigned int qual_ndx, action_ndx;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");
    SHR_NULL_CHECK(entry_handle_p, _SHR_E_PARAM, "entry_handle_p");

    /*
     * Verify that the field group exists (its ID is allocated).
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group ID %d isn't allocated.\r\n", fg_id);
    }
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are only supported for TCAM lookup (non DT) entries.\n", flags);
    }

    /*
     * Verify core
     */
    if ((entry_info_p->core < 0 || entry_info_p->core > dnx_data_device.general.nof_cores_get(unit) - 1)
        && entry_info_p->core != _SHR_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core ID %d is not valid. available range is (0:%d) or _SHR_CORE_ALL \n",
                     entry_info_p->core, dnx_data_device.general.nof_cores_get(unit) - 1);
    }

    /*
     * Verify that the qualifiers that appear in the entry also appear in the field group.
     */
    for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
    {
        dnx_quals[qual_ndx] = entry_info_p->key_info.qual_info[qual_ndx].dnx_qual;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_group_qual_arr_subset_verify(unit, fg_id, dnx_quals),
                             "TCAM Table entry qualifiers must be subset of qualifiers in the field group they "
                             "belong to.\r\n%s%s%s", EMPTY, EMPTY, EMPTY);
    /*
     * Verify that the actions that appear in the entry also appear in the field group.
     */
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        dnx_actions[action_ndx] = entry_info_p->payload_info.action_info[action_ndx].dnx_action;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_group_action_arr_subset_verify(unit, fg_id, dnx_actions),
                             "TCAM Table entry actions must be subset of actions in the field group they "
                             "belong to.\r\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_entry_valid_bit_choice_verify(unit, fg_id, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Builds the TCAM DT key value from the given qualifiers
* \param [in] unit             - Device ID
* \param [in] stage            - Stage of the FG
* \param [in] entry_key_info_p - contains qualifiers info
* \param [out] key             - The returned built key from the given qualifiers
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_entry_tcam_add
*/
static shr_error_e
dnx_field_entry_tcam_dt_key_build(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_entry_key_t * entry_key_info_p,
    uint32 *key)
{
    int qual_idx;
    int fill_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    *key = 0;

    for (qual_idx = 0;
         qual_idx < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP
         && entry_key_info_p->qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        uint32 qual_size;
        int qual_val = entry_key_info_p->qual_info[qual_idx].qual_value[0];
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, stage, entry_key_info_p->qual_info[qual_idx].dnx_qual, &qual_size));

        *key |= (qual_val << fill_idx);

        fill_idx += qual_size;
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    dnx_field_group_type_e fg_type = DNX_FIELD_GROUP_TYPE_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input */
    SHR_INVOKE_VERIFY_DNX(dnx_field_entry_tcam_add_verify(unit, flags, fg_id, entry_info_p, entry_handle_p));

    /** Get the needed information of the Field Group that the entry is being configured for */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Convert core here for verify
     */
    if (entry_info_p->core == _SHR_CORE_ALL)
    {
        entry_info_p->core = DNX_FIELD_TCAM_CORE_ALL;
    }

    /** Allocate entry access ID and find location for it */

    /*
     * Skip creating new access ID for update procedure
     */
    if (!(flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE))
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_pmf_access_id_create
                        (unit, flags, entry_info_p->core, fg_type, fg_id, entry_info_p->priority, entry_handle_p));
    }
    /*
     * Set valid bit
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_valid_set
                    (unit, entry_info_p->core, *entry_handle_p, entry_info_p->valid_bit));

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_set
                    (unit, flags, field_stage, dbal_table_id, *entry_handle_p, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_dt_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    uint32 key = 0;
    uint32 tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
    dnx_field_tcam_entry_t entry;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input */
    SHR_INVOKE_VERIFY_DNX(dnx_field_entry_tcam_dt_add_verify(unit, flags, fg_id, entry_info_p, entry_handle_p));

    /** Get the needed information of the Field Group that the entry is being configured for */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    /*
     * Convert core here for verify
     */
    if (entry_info_p->core == _SHR_CORE_ALL)
    {
        entry_info_p->core = DNX_FIELD_TCAM_CORE_ALL;
    }

    /**
     * For DT FGs, entry handle is created from the key (input quals).
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_dt_key_build(unit, field_stage, &(entry_info_p->key_info), &key));
    *entry_handle_p = DNX_FIELD_TCAM_DT_ENTRY(key, fg_id, entry_info_p->core);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
    entry.id = *entry_handle_p;
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_dt_location_alloc(unit, entry_info_p->core, tcam_handler_id, &entry));

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_set
                    (unit, flags, field_stage, dbal_table_id, *entry_handle_p, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_kbp_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    dnx_field_group_type_e fg_type = DNX_FIELD_GROUP_TYPE_INVALID;
    uint32 access_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input.*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_entry_tcam_add_verify(unit, 0, fg_id, entry_info_p, entry_handle_p));

    if ((flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID) || (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE))
    {
        access_id = DNX_FIELD_EXTERNAL_TCAM_ACCESS_ID(*entry_handle_p);
        if (DNX_FIELD_ENTRY(DNX_FIELD_TCAM_EXTERNAL, access_id, fg_id, 0) != (*entry_handle_p))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "External TCAM (KBP) entry_id for fg_id %d (0x%x) is not a legal value, "
                         "and WITH_ID or UPDATE flags set.\r\n", fg_id, *entry_handle_p);
        }

    }

    /**Get the needed information of the Field Group that the entry is being configured for*/
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_kbp_hw_set(unit, flags, field_stage, dbal_table_id, entry_info_p, &access_id));

    if ((!(flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID)) && (!(flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)))
    {
        /*
         * Only after hw_set we can get the  KBP entry (hw) access_id and encode the entry_id
         * It was allocated automatically by the KBP driver after the hw_set
 */
        if ((access_id & SAL_FROM_BIT(DNX_DATA_FIELD_ENTRY_EXTERNAL_TCAM_ACCESS_ID_SIZE)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "External TCAM (KBP) access_id larger than %d bits (0x%x).\r\n",
                         DNX_DATA_FIELD_ENTRY_EXTERNAL_TCAM_ACCESS_ID_SIZE, access_id);
        }
        /*
         *   Encode external TCAM entry id per Field Group ID and Access Id
         */
        *entry_handle_p = DNX_FIELD_ENTRY(DNX_FIELD_TCAM_EXTERNAL, access_id, fg_id, 0);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_state_table_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_entry_key_t * entry_key_info_p)
{
    uint32 address;
    uint32 data_unused;
    uint32 data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");

    /*
     * Initialization to silence the compiler.
     */
    data = -1;

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_fields_parse(unit, stage, entry_key_info_p, &address, &data_unused));

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_hw_get(unit, address, &data));

    /*
     * Sanity check that initialziation to silence the compiler wan't needed.
     */
    if (data == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "data not updated.\r\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_fields_combine(unit, stage, address, data, entry_key_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_state_table_delete(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_entry_key_t * entry_key_info_p)
{
    uint32 address;
    uint32 data_unused;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_fields_parse(unit, stage, entry_key_info_p, &address, &data_unused));

    /*
     * Deleting is the same as setting the value to 0 in State Table
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_hw_set(unit, address, DNX_FIELD_ENTRY_STATE_TABLE_DELETE_VAL));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_state_table_add(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_entry_key_t * entry_key_info_p)
{
    uint32 address;
    uint32 data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_fields_parse(unit, stage, entry_key_info_p, &address, &data));

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_hw_set(unit, address, data));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_entry_exem_add().
* Checks that all qualifiers in the field gorup are given for the entry.
* \param [in] unit           - Device ID
* \param [in] flags          - entry addition flags, currently only supported flag is
*                              DNX_FIELD_ENTRY_ADD_FLAG_UPDATE
* \param [in] fg_id          - The ID of the field group to which we add the entry.
* \param [in] entry_info_p   - Entry info to configure into the database.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_exem_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
    unsigned int qual_ndx, action_ndx;
    uint8 is_allocated;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    /*
     * Verify the flags.
     */
    if (flags & (~DNX_FIELD_ENTRY_ADD_FLAG_UPDATE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM field group only supports UPDATE flag (0x%x). flags (0x%x) are not supported. "
                     "fg_id %d flags 0x%x.\r\n",
                     DNX_FIELD_ENTRY_ADD_FLAG_UPDATE, flags & (~DNX_FIELD_ENTRY_ADD_FLAG_UPDATE), fg_id, flags);
    }

    /*
     * Verify that the field group exists (its ID is allocated).
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group ID %d isn't allocated.\r\n", fg_id);
    }

    /*
     * Verify that the EXEM entry has at least one action, since otherwise we could have the same result
     * by not creating the EXEM entry.
     */
    if (entry_info_p->payload_info.action_info[0].dnx_action == DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Exact match entry must have at least one action.\r\n");
    }

    /*
     * Verify that the same qualifiers appear in the entry as in the field group.
     * That is a required from exact match, as opposed to a TCAM entry.
     */
    for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
    {
        dnx_quals[qual_ndx] = entry_info_p->key_info.qual_info[qual_ndx].dnx_qual;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_group_qual_arr_compare_verify(unit, fg_id, dnx_quals),
                             "EXEM entries must include values for all qualifiers in the field group they "
                             "belong to, since Exact Match doesn't use a mask like TCAM.\r\n%s%s%s",
                             EMPTY, EMPTY, EMPTY);
    /*
     * Verify that the actions that appear in the entry also appear in the field group.
     */
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        dnx_actions[action_ndx] = entry_info_p->payload_info.action_info[action_ndx].dnx_action;
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_group_action_arr_subset_verify(unit, fg_id, dnx_actions),
                             "EXEM Table entry actions must be subset of actions in the field group they "
                             "belong to.\r\n%s%s%s", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_entry_valid_bit_choice_verify(unit, fg_id, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_exem_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    dnx_field_group_type_e fg_type = DNX_FIELD_GROUP_TYPE_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input, specifically that we are given all qualifiers (unlike in a TCAM field group).*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_entry_exem_add_verify(unit, flags, fg_id, entry_info_p));

    /**Get the needed information of the Field Group that the entry is being configured for*/
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_exem_hw_set(unit, flags, field_stage, dbal_table_id, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_entry_mdb_dt_add().
* Checks that all qualifiers in the field gorup are given for the entry.
* \param [in] unit           - Device ID
* \param [in] flags          - TBD
* \param [in] fg_id          - The ID of the field group to which we add the entry.
* \param [in] entry_info_p   - Entry info to configure into the database.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_mdb_dt_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
    unsigned int qual_ndx;
    uint8 is_allocated;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the flags.
     */
    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MDB DT field group entry add supports no flags. flags (0x%x) are not supported. "
                     "fg_id %d.\r\n", flags, fg_id);
    }
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    /*
     * Verify that the field group exists (its ID is allocated).
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group ID %d isn't allocated.\r\n", fg_id);
    }

    /*
     * Verify that the MDB_DT entry has at least one action, since otherwise we could have the same result
     * by not creating the MDB_DT entry.
     */
    if (entry_info_p->payload_info.action_info[0].dnx_action == DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Exact match entry must have at least one action.\r\n");
    }

    /*
     * Verify that the same qualifiers appear in the entry as in the field group.
     * That is a required from exact match, as opposed to a TCAM entry.
     */
    for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
    {
        dnx_quals[qual_ndx] = entry_info_p->key_info.qual_info[qual_ndx].dnx_qual;
    }
    rv = dnx_field_group_qual_arr_compare_verify(unit, fg_id, dnx_quals);
    SHR_IF_ERR_EXIT_WITH_LOG(rv,
                             "Direct Table MDB entries must include values for all qualifiers in the field group they "
                             "belong to, since Direct Table MDB doesn't use a mask like TCAM.\r\n%s%s%s", EMPTY, EMPTY,
                             EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_entry_valid_bit_choice_verify(unit, fg_id, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_mdb_dt_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    dnx_field_group_type_e fg_type = DNX_FIELD_GROUP_TYPE_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input, specifically that we are given all qualifiers (unlike in a TCAM field group).*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_entry_mdb_dt_add_verify(unit, flags, fg_id, entry_info_p));

    /**Get the needed information of the Field Group that the entry is being configured for*/
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_mdb_dt_hw_set(unit, flags, field_stage, dbal_table_id, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_ace_entry_add().
* Checks, if we update an entry, if the entry exists within the ACE format ID.
* \param [in] unit               - Device ID
* \param [in] flags              - dnx_field_entry_add_flags_e flags.
* \param [in] ace_id             - ACE format ID
* \param [in] payload_info_p     - The actions and their value to fill in the payload of the entry.
* \param [in] ace_entry_key_p    - The entry of the ACE key, the value to be written in the CUD_OUTLIF_OR_MCDB_PTR
*                                  action of eMPF for the entry to be matched in the ACE table lookup.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_entry_add_verify(
    int unit,
    uint32 flags,
    dnx_field_ace_id_t ace_id,
    dnx_field_entry_payload_t * payload_info_p,
    dnx_field_ace_key_t * ace_entry_key_p)
{
    dnx_field_ace_id_t ace_id_retrieved;
    uint8 is_alloc;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(payload_info_p, _SHR_E_PARAM, "payload_info_p");
    SHR_NULL_CHECK(ace_entry_key_p, _SHR_E_PARAM, "ace_entry_key_p");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    /*
     * If the ACE format is not of type ACE (that is, PMF extension), we don't know how to allokate a key for it and
     * we demand a "WITH_ID or REPLACE flags."
     */
    if ((DNX_FIELD_ACE_FORMAT_ID_IS_ACE(unit, ace_id) == FALSE) &&
        ((flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE) == 0) && ((flags & DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID) == 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ACE format id %d isn't ACL extension, WITH_ID flag must be set.", ace_id);
    }

    /*
     * Verify that the ACE entry has at least one action, since otherwise we could have the same result
     * by not creating the EXEM entry (unless an action has no valid bit, in which case we should anyway add it).
     */
    if (payload_info_p->action_info[0].dnx_action == DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ACE entry must have at least one action.\r\n");
    }

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        /*
         * If we Update the entry, verify that it is allocated, if it is a PMF entension format.
         */
        if (DNX_FIELD_ACE_FORMAT_ID_IS_ACE(unit, ace_id))
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_ace_key_is_allocated(unit, *ace_entry_key_p, &is_alloc));
            if (is_alloc == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ACE entry with key %u not found in allocation manager.", *ace_entry_key_p);
            }
        }
        /*
         * Verify that the entry is allocated to the ACE ID (and if it exists).
         */
        SHR_IF_ERR_EXIT(dnx_field_ace_entry_hw_get(unit, *ace_entry_key_p, &ace_id_retrieved, NULL));
        if (ace_id_retrieved != ace_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ACE entry with key %u belongs to ACE format ID %d, not %d.",
                         *ace_entry_key_p, ace_id_retrieved, ace_id);
        }
    }

    /*
     * We allow ACE entries actions to not have a valid bit, even if DNX_FIELD_ALLOW_VALID_BIT_CHOICE_FOR_LOOKUP_FG
     * is not set, since the actions won't happen if no entry in the ace format is hit.
     */
    {
        dnx_field_actions_ace_payload_info_t actions_payload_info;
        unsigned int action_fg_ndx;
        int action_is_void;
        int action_found;
        unsigned int action_entry_ndx;
        
        SHR_IF_ERR_EXIT(dnx_field_ace_format_action_template_get(unit, ace_id, &actions_payload_info));
        for (action_fg_ndx = 0;
             action_fg_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
             && actions_payload_info.actions_on_payload_info[action_fg_ndx].dnx_action != DNX_FIELD_ACTION_INVALID;
             action_fg_ndx++)
        {
            if (actions_payload_info.actions_on_payload_info[action_fg_ndx].dont_use_valid_bit == TRUE)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                                (unit, DNX_FIELD_STAGE_ACE,
                                 actions_payload_info.actions_on_payload_info[action_fg_ndx].dnx_action,
                                 &action_is_void));
                if (action_is_void == FALSE)
                {
                    action_found = FALSE;
                    for (action_entry_ndx = 0;
                         action_entry_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
                         && payload_info_p->action_info[action_entry_ndx].dnx_action !=
                         DNX_FIELD_ACTION_INVALID; action_entry_ndx++)
                    {
                        if (payload_info_p->action_info[action_entry_ndx].dnx_action ==
                            actions_payload_info.actions_on_payload_info[action_fg_ndx].dnx_action)
                        {
                            action_found = TRUE;
                            break;
                        }
                    }
                    if (action_found == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Action %s in ace format %d doen't have a valid bit and isn't a "
                                     "void action, so it must appear in enery entry.\r\n",
                                     dnx_field_dnx_action_text(unit,
                                                               actions_payload_info.actions_on_payload_info
                                                               [action_fg_ndx].dnx_action), ace_id);
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_ace_entry_add(
    int unit,
    uint32 flags,
    dnx_field_ace_id_t ace_id,
    dnx_field_entry_payload_t * payload_info_p,
    dnx_field_ace_key_t * ace_entry_key_p)
{
    int result_type_index;
    int is_update;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input.*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_entry_add_verify(unit, flags, ace_id, payload_info_p, ace_entry_key_p));

    /*
     * Get the result_type_index.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_result_type_index_get(unit, ace_id, &result_type_index));

    if (flags & DNX_FIELD_ENTRY_ADD_FLAG_UPDATE)
    {
        is_update = TRUE;
    }
    else
    {
        
        is_update = FALSE;
        if (DNX_FIELD_ACE_FORMAT_ID_IS_ACE(unit, ace_id))
        {
            SHR_IF_ERR_EXIT(dnx_field_ace_entry_key_allocate(unit, flags, ace_id, ace_entry_key_p));
        }
    }

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_hw_set(unit, result_type_index, is_update, *ace_entry_key_p, payload_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_delete(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle,
    dnx_field_entry_key_t * key_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    dnx_field_group_type_e fg_type = DNX_FIELD_GROUP_TYPE_INVALID;
    uint32 tcam_handler_id = DNX_FIELD_TCAM_HANDLER_ID_INVALID;
    dnx_field_group_t entry_fg_id;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_handle, &entry_fg_id));
    if (entry_fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry delete failed: Given field group ID (%d) is different than the field group to which entry belongs (%d).\n",
                     fg_id, entry_fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_clear(unit, field_stage, dbal_table_id, key_info_p, entry_handle));

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, dbal_table_id, entry_handle));
    }
    else        /* DT */
    {
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_location_free(unit, tcam_handler_id, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_kbp_delete(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    uint32 access_id = DNX_FIELD_ENTRY_ACCESS_ID(entry_handle);
    dnx_field_group_t handle_fg_id;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    handle_fg_id = DNX_FIELD_TCAM_EXTERNAL_ENTRY_FG_ID(entry_handle);
    if (handle_fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry delete failed: Given field group ID (%d) is different than the field group to which entry belongs (%d).\n",
                     fg_id, handle_fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_clear(unit, field_stage, dbal_table_id, NULL, access_id));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_exem_delete(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_key_t * entry_key_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    SHR_IF_ERR_EXIT(dnx_field_entry_exem_hw_clear(unit, field_stage, dbal_table_id, entry_key_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_mdb_dt_delete(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_key_t * entry_key_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    SHR_IF_ERR_EXIT(dnx_field_entry_mdb_dt_hw_clear(unit, field_stage, dbal_table_id, entry_key_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_ace_entry_delete(
    int unit,
    dnx_field_ace_key_t ace_entry_key)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_ace_entry_hw_clear(unit, ace_entry_key));

    /** We deallocate ACE pointers only. Note we do not check the ACE format ID, just the ACE key.*/
    if (DNX_FIELD_ACE_KEY_IS_ACE(unit, ace_entry_key))
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_ace_key_deallocate(unit, ace_entry_key));
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_get(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle,
    dnx_field_entry_t * entry_info_p)
{
    uint32 tcam_handler_id;
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    dnx_field_group_t entry_fg_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_handle, &entry_fg_id));
    if (entry_fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry get failed: Given field group ID (%d) is different than the field group to which entry belongs (%d).\n",
                     fg_id, entry_fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));

    entry_info_p->core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_handle);
    /** In case of DT entry, the entry has no meaning and we can't retrieve its priority. */
    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        /*
         * Get entry's priority
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_priority_get
                        (unit, tcam_handler_id, entry_handle, &(entry_info_p->priority)));

        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_valid_get
                        (unit, entry_info_p->core, entry_handle, &(entry_info_p->valid_bit)));
    }

    /*
     * Get Actions/Quals values for the given entry
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_quals_actions_type_get(unit, fg_id, entry_info_p));
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_get(unit, field_stage, fg_id, dbal_table_id, entry_handle, entry_info_p));

    /*
     * Convert internal DNX_CORE_ALL to output BCM _SHR_CORE_ALL
     */
    if (entry_info_p->core == DNX_FIELD_TCAM_CORE_ALL)
    {
        entry_info_p->core = _SHR_CORE_ALL;
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_kbp_get(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle,
    dnx_field_entry_t * entry_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
    dnx_field_group_t handle_fg_id;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    handle_fg_id = DNX_FIELD_TCAM_EXTERNAL_ENTRY_FG_ID(entry_handle);
    if (handle_fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry get failed: Given field group ID (%d) is different than the field group to which entry belongs (%d).\n",
                     fg_id, handle_fg_id);
    }

    /** KBP entries can only be added after device lock.*/
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "External TCAM (KBP) entries can only be added after device lock."
                     "Attempted to add an entry to field group %d.\r\n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, entry_info_p));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Get Actions/Quals values for the given entry
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_quals_actions_type_get(unit, fg_id, entry_info_p));
    SHR_IF_ERR_EXIT(dnx_field_entry_kbp_hw_get(unit, field_stage, fg_id, dbal_table_id, entry_handle, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_exem_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_key_t * entry_key_p,
    dnx_field_entry_t * entry_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Get Actions/Quals values for the given entry
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_quals_actions_type_get(unit, fg_id, entry_info_p));
    rv = dnx_field_entry_exem_hw_get(unit, field_stage, fg_id, dbal_table_id, entry_key_p, entry_info_p);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_mdb_dt_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_key_t * entry_key_p,
    dnx_field_entry_t * entry_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Get Actions/Quals values for the given entry
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_quals_actions_type_get(unit, fg_id, entry_info_p));
    rv = dnx_field_entry_mdb_dt_hw_get(unit, field_stage, fg_id, dbal_table_id, entry_key_p, entry_info_p);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_ace_entry_get(
    int unit,
    dnx_field_ace_key_t ace_entry_key,
    dnx_field_entry_payload_t * payload_info_p,
    dnx_field_ace_id_t * ace_id_p)
{
    dnx_field_ace_id_t ace_id;
    SHR_FUNC_INIT_VARS(unit);

    if (payload_info_p == NULL && ace_id_p == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Function called with both output parameters as NULL pointers.\n");
    }

    /*
     * Get Action values for the given entry
     * Information taken from the ACE format is done inside dnx_field_ace_entry_hw_get() because we read
     * the ACE ID from the HW.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_hw_get(unit, ace_entry_key, &ace_id, payload_info_p));

    /** If ace_id_p isn't NULL, fill it with the ACE_ID that we found.*/
    if (ace_id_p != NULL)
    {
        (*ace_id_p) = ace_id;
    }

    /*
     * Sanity check:
     *  If the ACE format ID is a PMF extension format, verify that the key is an ACE pointer and vice versa.
     */
    if ((DNX_FIELD_ACE_FORMAT_ID_IS_ACE(unit, ace_id)) && (DNX_FIELD_ACE_KEY_IS_ACE(unit, ace_entry_key)) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "ACE format ID (%d) is in the ACE range (that is, a PMF extension), but the key (%d) is not.\r\n",
                     ace_id, ace_entry_key);
    }
    if ((DNX_FIELD_ACE_FORMAT_ID_IS_ACE(unit, ace_id)) == FALSE && (DNX_FIELD_ACE_KEY_IS_ACE(unit, ace_entry_key)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "ACE format ID (%d) is not in ACE range (that is, not a PMF extension), but the key (%d) is.\r\n",
                     ace_id, ace_entry_key);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_hit_get(
    int unit,
    uint32 flags,
    uint32 entry_handle,
    uint8 *entry_hit_core_bmp_p)
{
    dnx_field_entry_t entry_info;
    dnx_field_group_t fg_id;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    uint32 entry_hit_core_bmp_uint32;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_handle, &fg_id));

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
    SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));
    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        LOG_CLI_EX("\r\n" "The given entry %d (0x%08X) is invalid.\r\n %s%s", entry_handle, entry_handle, EMPTY, EMPTY);
        SHR_EXIT();
    }

    *entry_hit_core_bmp_p = 0;

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &entry_hit_core_bmp_uint32));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    *entry_hit_core_bmp_p = entry_hit_core_bmp_uint32;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_hit_per_location_flush(
    int unit,
    dnx_field_tcam_core_e core_id,
    uint8 flush_all,
    uint32 entry_abs_location)
{
    soc_reg_above_64_val_t reg_val_zero;
    int index, index_min, index_max;
    int array_index, array_index_min = 0, array_index_max = 0;
    soc_mem_t mem = INVALIDm;
    uint32 bank_id, bank_sram_id, bank_line_id, sram_line_entry_id;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint8 mem_index, nof_tcam_hit_indication_memories;
    uint32 tcam_indirect_mask;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;

#ifdef ADAPTER_SERVER_MODE
    uint32 mem_entry_data = 0;
    uint32 mem_hit_field_value = 0;
#else
    uint32 mem_addr;
    uint8 at;
    uint32 reg_val = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

    bank_sram_id = bank_id = bank_line_id = sram_line_entry_id = 0;
    max_nof_big_bank_lines = max_nof_small_bank_lines = 0;
    max_nof_entries_in_two_srams = max_nof_srams_per_tcam_bank = 0;

    SOC_REG_ABOVE_64_CLEAR(reg_val_zero);

    if (flush_all == FALSE)
    {
        /** Get the bank_id from entry absolute location. */
        bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(entry_abs_location);

        /** Get max values from DNX data. */
        max_nof_srams_per_tcam_bank = dnx_data_field.tcam.action_width_selector_size_get(unit);
        max_nof_entries_in_two_srams =
            (dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit) * max_nof_srams_per_tcam_bank);

        /** Check if entry is located in TCAM big banks. */
        if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
        {
            mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;

            /** Get max values from DNX data. */
            max_nof_big_bank_lines = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
            /** Calculate the bank_line_id for big banks. */
            bank_line_id = ((entry_abs_location % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
        }
        else
        {
            mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;

            /** Get max values from DNX data. */
            max_nof_small_bank_lines = dnx_data_field.tcam.nof_small_bank_lines_get(unit);
            /** Calculate the bank_line_id for small banks. */
            bank_line_id = ((entry_abs_location % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
        }

        /** Calculate the bank_sram_id and sram_line_entry_id. */
        bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (entry_abs_location % 2));
        sram_line_entry_id =
            (((entry_abs_location % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);
        /** Mask the exact bit, which is corresponding to the entry, which have to be cleared. */
        tcam_indirect_mask = 0x1 << sram_line_entry_id;

        /**
         * In case we have to clear hit information for a specific entry_handle, we have only
         * one memory, which is being chosen above 'mem'.
         */
        nof_tcam_hit_indication_memories = 1;
    }
    else
    {
        /**
         * Set full mask in case there is no specific entry to be cleared,
         * it means that we are going to clear info for all entries.
         */
        tcam_indirect_mask = 0xFF;
        /** Do the clean for both BIG and SMALL bank memories. */
        nof_tcam_hit_indication_memories = 2;
    }

    /** Iterate over all hit indication memories. */
    for (mem_index = 0; mem_index < nof_tcam_hit_indication_memories; mem_index++)
    {
        /**
         * In case there is no specified entry we should do the clean for both
         * tcam hit indication memories.
         */
        if (flush_all)
        {
            if (mem_index == 0)
            {
                mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;
            }
            else
            {
                mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;
            }
        }

        /** Get info about HW table */
        index_min = dnx_drv_soc_mem_index_min(unit, mem);
        index_max = dnx_drv_soc_mem_index_max(unit, mem);

        /** Check if the memory is array. Then get the array min and max indexes. */
        if (dnx_drv_soc_mem_is_array(unit, mem))
        {
            array_index_max = (dnx_drv_soc_mem_numels(unit, mem) - 1) + dnx_drv_soc_mem_first_array_index(unit, mem);
            array_index_min = dnx_drv_soc_mem_first_array_index(unit, mem);
        }

        /** Iterate over memory array indexes. In this case of TCAM this is the SRAMs which we have per bank. */
        for (array_index = array_index_min; array_index <= array_index_max; array_index++)
        {
            uint32 bank_sram_id_internal = array_index;
            if (flush_all == FALSE)
            {
                bank_sram_id_internal = bank_sram_id;
            }

            /** Iterate over memory indexes. In this case of TCAM this is the LINEs per bank. */
            for (index = index_min; index <= index_max; index++)
            {
                uint32 bank_line_id_internal = index;
                if (flush_all == FALSE)
                {
                    bank_line_id_internal = bank_line_id;
                }
#ifdef ADAPTER_SERVER_MODE
                SHR_IF_ERR_EXIT(soc_mem_array_read
                                (unit, mem, bank_sram_id_internal, TCAM_BLOCK(unit, core_id), bank_line_id_internal,
                                 &mem_entry_data));
                soc_mem_field_get(unit, mem, &mem_entry_data, HITf, &mem_hit_field_value);

                mem_entry_data = mem_entry_data & ~tcam_indirect_mask;

                SHR_IF_ERR_EXIT(soc_mem_array_write
                                (unit, mem, bank_sram_id_internal, TCAM_BLOCK(unit, core_id), bank_line_id_internal,
                                 &mem_entry_data));
#else
                /** Get the memory address by array_index, block_index and index. */
                mem_addr =
                    soc_mem_addr_get(unit, mem, bank_sram_id_internal, TCAM_BLOCK(unit, core_id), bank_line_id_internal,
                                     &at);

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMANDr(unit, _SHR_CORE_ALL, 0xe00003));

                reg_val = 0;
                soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
                soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_ADDRESSr(unit, _SHR_CORE_ALL, reg_val));

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_WR_MASKr(unit, _SHR_CORE_ALL, tcam_indirect_mask));

                SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_WR_DATAr(unit, _SHR_CORE_ALL, reg_val_zero));
#endif
                /** Clear the SW state hit indication in case hitbit_volatile is set for current device. */
                if (dnx_data_field.features.hitbit_volatile_get(unit))
                {
                    uint8 hit_bit_sw_state_value;
                    uint32 hit_bit_sw_state_value_uint32;

                    if (mem == TCAM_TCAM_ACTION_HIT_INDICATIONm)
                    {
                        dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.get(unit,
                                                                                                  bank_sram_id_internal,
                                                                                                  core_id,
                                                                                                  bank_line_id,
                                                                                                  &hit_bit_sw_state_value);
                    }
                    else
                    {
                        dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.bank_line_id.get(unit,
                                                                                                        (bank_sram_id_internal
                                                                                                         -
                                                                                                         dnx_data_field.tcam.nof_big_banks_srams_get
                                                                                                         (unit)),
                                                                                                        core_id,
                                                                                                        bank_line_id,
                                                                                                        &hit_bit_sw_state_value);
                    }
                    hit_bit_sw_state_value_uint32 = hit_bit_sw_state_value;
                    hit_bit_sw_state_value_uint32 = hit_bit_sw_state_value_uint32 & ~tcam_indirect_mask;

                    hit_bit_sw_state_value = hit_bit_sw_state_value_uint32;
                    if (mem == TCAM_TCAM_ACTION_HIT_INDICATIONm)
                    {
                        dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.set(unit,
                                                                                                  bank_sram_id_internal,
                                                                                                  core_id,
                                                                                                  bank_line_id,
                                                                                                  hit_bit_sw_state_value);
                    }
                    else
                    {
                        dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.bank_line_id.set(unit,
                                                                                                        (bank_sram_id_internal
                                                                                                         -
                                                                                                         dnx_data_field.tcam.nof_big_banks_srams_get
                                                                                                         (unit)),
                                                                                                        core_id,
                                                                                                        bank_line_id,
                                                                                                        hit_bit_sw_state_value);
                    }
                }

                /**
                 * In case of specific entry, we have to do the clean only for it on the give core_id
                 * instances and then go to the exit.
                 */
                if (flush_all == FALSE)
                {
                    goto exit;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_hit_flush(
    int unit,
    uint32 flags,
    uint32 entry_handle)
{
    dnx_field_entry_t entry_info;
    dnx_field_group_t fg_id;
    uint32 entry_abs_location = 0;
    uint8 nof_cores;
    dnx_field_tcam_core_e core_id;
    dnx_field_tcam_location_t location;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the number of cores from DNX data. */
    nof_cores = dnx_data_device.general.nof_cores_get(unit);

    if (flags & BCM_FIELD_ENTRY_HIT_FLUSH_ALL)
    {
        /** Iterate over nof_cores and clear hit info, on all cores, for the given entry. */
        for (core_id = 0; core_id < nof_cores; core_id++)
        {
          /** Clear hit info for all entries. */
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_per_location_flush(unit, core_id, TRUE, entry_abs_location));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_handle, &fg_id));

        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
        SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
        {
            LOG_CLI_EX("\r\n" "The given entry %d (0x%08X) is invalid.\r\n %s%s", entry_handle, entry_handle, EMPTY,
                       EMPTY);
            SHR_EXIT();
        }

        /** Get the core_id from the given entry_handle. */
        core_id = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_handle);

        if (core_id == DNX_FIELD_TCAM_CORE_ALL)
        {
            /** Iterate over nof_cores and clear hit info, on all cores, for the given entry. */
            for (core_id = 0; core_id < nof_cores; core_id++)
            {
                dnx_field_tcam_handler_entry_location(unit, core_id, entry_handle, &location);
                entry_abs_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);

                /** Clear hit info for the given entry entry. */
                SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_per_location_flush(unit, core_id, FALSE, entry_abs_location));
            }
        }
        else
        {
            dnx_field_tcam_handler_entry_location(unit, core_id, entry_handle, &location);
            entry_abs_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(location.bank_id, location.bank_offset);

            /** Clear hit info for the given entry. */
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_per_location_flush(unit, core_id, FALSE, entry_abs_location));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
