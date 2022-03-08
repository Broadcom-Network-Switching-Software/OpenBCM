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
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
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

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
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
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/field/tcam/tcam_handler.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>

#include <bcm_int/dnx/algo/ppmc/algo_ppmc.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.h>
#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
#include <bcm_int/dnx/field/field_kbp.h>
#endif

/*
 * }Include files
 */

#define DNX_FIELD_ENTRY_STATE_TABLE_DELETE_VAL 0
/*
 * Globals
 * {
 */
/*
 * }
 */

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
dnx_field_entry_state_table_t_init(
    int unit,
    dnx_field_entry_state_table_t * entry_add_in_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_add_in_p, _SHR_E_PARAM, "entry_add_in_p");

    sal_memset(entry_add_in_p, 0x0, sizeof(*entry_add_in_p));

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        entry_add_in_p->address_info.qual_info[ii].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
        entry_add_in_p->data_info.qual_info[ii].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    entry_add_in_p->core = _SHR_CORE_ALL;

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
* \param [in] flags            - dnx_field_entry_group_add_flags_e flags.
* \param [in] field_stage      - Field Stage (iPMF1/2...)
* \param [in] dbal_table_id    - DBAL table id that was created for Field group ad fg_create()
* \param [in] entry_info_p     - Information that will be written into HW (QUALS & ACTIONS)
* \param [in] is_dt            - TRUE when TCAM fg is DT
* \param [in,out] access_id    - Access ID that TCAM manager allocated for this entry
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_hw_set(
    int unit,
    uint32 flags,
    dnx_field_stage_e field_stage,
    dbal_tables_e dbal_table_id,
    dnx_field_entry_t * entry_info_p,
    uint8 is_dt,
    uint32 *access_id)
{
    uint32 entry_handle_id;
    uint32 qual_idx;
    uint32 action_idx;
    dbal_fields_e field_id;
    dbal_entry_action_flags_e dbal_flags = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_attribute_set(unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, entry_info_p->priority));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_set
                    (unit, entry_handle_id, DBAL_ENTRY_ATTR_INVALID, !(entry_info_p->valid_bit)));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, entry_info_p->core);

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

    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)
    {
        dbal_flags = (DBAL_COMMIT_UPDATE | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }
    else
    {
        dbal_flags = (DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }

    if (is_dt || (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID) || (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *access_id));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));

    /*
     * The access id is created by TCAM management (for non-DT FGs) and can be only read after the hw_set (entry dbal_commit) Thus, we
     * read it here.
     */
    if ((!is_dt) && (!(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID)) &&
        (!(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, access_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Call DBAL table and set the EXEM entry into HW
* \param [in] unit           - Device ID
* \param [in] flags          - dnx_field_entry_group_add_flags_e flags.
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

    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)
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
*  Call DBAL table and set the external TCAM entry into HW
* \param [in] unit           - Device ID
* \param [in] flags          - dnx_field_entry_group_add_flags_e flags.
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
dnx_field_entry_external_tcam_hw_set(
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
             * (in fact, we only support 16 bit ranges).
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

    /*
     * Set the core
     */
    {
        dbal_core_mode_e core_mode;
        SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, entry_info_p->core);
        }
    }

    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)
    {
        dbal_flags = (DBAL_COMMIT_UPDATE | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }
    else
    {
        dbal_flags = (DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT);
    }

    if ((flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID) || (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, *access_id_p));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, dbal_flags));

    /*
     * The access id is created by the external TCAM device and can be only read after the hw_set
     * (entry dbal_commit) Thus, we read it here.
     */
    if ((!(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID)) && (!(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)))
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
* \param [in] core_id - Core the entry is in
* \param [in] address - Address of the entry to set
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
    int core_id,
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

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

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
                        (unit, stage, entry_key_info_p->qual_info[qual_idx].dnx_qual, NULL, NULL, &qual_size));

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
                        (unit, field_stage, DNX_FIELD_ENTRY_TCAM_DT_ACCESS_ID(access_id), &(entry_info_p->key_info)));

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

    /*
     * Dummy core set, real core_id is resolved from access_id at the access layer 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

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

    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_CORE_ID, (uint32 *) &(entry_info_p->core)));

    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        uint32 invalidate_bit;
        SHR_IF_ERR_EXIT(dbal_entry_attribute_get
                        (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, &(entry_info_p->priority)));
        SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, DBAL_ENTRY_ATTR_INVALID, &invalidate_bit));
        /*
         * Flip received *invalidate* bit to retrieve valid 
         */
        entry_info_p->valid_bit = !invalidate_bit;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
* \brief
* This procedure retrieves the content of the external TCAM entry from HW.
* \param [in] unit                  -  Device ID
* \param [in] field_stage           -  Field Stage (iPMF1/2...)
* \param [in] fg_id                 -  Identifier of the field group. Used for debugging.
* \param [in] dbal_table_id         -  DBAL table id that was created for Field group at fg_create()
* \param [in] entry_access_id       -  Access ID that KBP manager allocated for this entry
* \param [in,out] cached_indication -  Indication whether the entry is cached in SW or installed to HW.
*                                      When NULL is provided, the entry will be retrieved only if installed to HW.
*                                      Otherwise, the parameter will be set to TRUE if the entry is cached or FALSE if
*                                      the entry is installed. All relevant entry information will be retrieved if the
*                                      entry is found (only relevant for diagnostics).
* \param [out] entry_info_p         -  Information extracted from the HW (QUALS & ACTIONS)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_external_tcam_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dbal_tables_e dbal_table_id,
    uint32 entry_access_id,
    uint32 *cached_indication,
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
    uint32 is_entry_cached = FALSE;
    dnx_field_group_cache_mode_e cache_mode = DNX_FIELD_GROUP_CACHE_MODE_CLEAR;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    access_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_ACCESS_ID(entry_access_id);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reading external TCAM entry from field group %d access_id (entry handle) 0x%X. %s%s\r\n",
                 fg_id, access_id, EMPTY, EMPTY);

    /** Get - all results fields of the table_p will be read to handle buffer */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.cache_mode.get(unit, fg_id, &cache_mode));
    /** When caching is enabled, request indication whether the entry is cached or not. */
    if (cache_mode == DNX_FIELD_GROUP_CACHE_MODE_START)
    {
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_CACHED, &is_entry_cached));
    }

    /*
     * Get entry's priority
     */
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request
                    (unit, entry_handle_id, DBAL_ENTRY_ATTR_PRIORITY, &entry_info_p->priority));

    /*
     * Set the core
     */
    {
        dbal_core_mode_e core_mode;
        SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            int core_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_PER_CORE_CORE_ID(entry_access_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            entry_info_p->core = core_id;
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Set cached indication if requested in order to get if the entry is cached or installed */
    if (cached_indication != NULL)
    {
        *cached_indication = is_entry_cached;
    }

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
* \param [in] core_id - Core to read from
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
    int core_id,
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

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);

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
* \param [in] access_id      - entry access ID
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

    /*
     * Dummy core set, real core_id is resolved from access_id at the access layer 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, BCM_CORE_ALL);
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
*  This function allocates an ACE key for an ACE entry. Only allocates ACE pointers, no other keys.
* \param [in] unit               - Device Id
* \param [in] flags              - dnx_field_entry_ace_add_flags_e flags.
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

    if (flags & DNX_FIELD_ENTRY_ACE_ADD_FLAG_WITH_ID)
    {
        alloc_flag = DNX_ALGO_RES_ALLOCATE_WITH_ID;
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
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * TCAM handler provides an iterator to loop on all entries for the field group
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, tcam_handler_id, &entry_iter));
        while (entry_iter != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
        {
            /*
             * Old trick of saving next iterator before deleting the current one
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next(unit, core, tcam_handler_id, entry_iter, &next_iter));

            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, fg_id, entry_iter, NULL));

            entry_iter = next_iter;
        }
    }
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
                        (unit, stage, entry_key_info_p->qual_info[qual_idx].dnx_qual, NULL, NULL, &qual_size));

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

        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, stage, qual_p->dnx_qual, NULL, NULL, &qual_size));

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
*   * dnx_field_entry_external_tcam_add
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
* Verify Function for standard lookup entry add
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_group_add_flags_e flags.
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
dnx_field_entry_standard_lookup_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    uint8 is_allocated;
    unsigned int qual_ndx, action_ndx;
    dnx_field_group_type_e fg_type;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG];
    uint32 unsupported_flags;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /*
     * Verify that the field group exists (its ID is allocated).
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group ID %d isn't allocated.\r\n", fg_id);
    }
    if ((unsupported_flags =
         (flags & ~(DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID | DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }
    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID && flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag UPDATE can't be used together with flag WITH_ID\n");
    }

    /*
     * Verify core
     */
    DNXCMN_CORE_VALIDATE(unit, entry_info_p->core, TRUE);

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
* Verify Function for dnx_field_entry_tcam_add().
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_group_add_flags_e flags.
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
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");
    SHR_NULL_CHECK(entry_handle_p, _SHR_E_PARAM, "entry_handle_p");

    SHR_IF_ERR_EXIT(dnx_field_entry_standard_lookup_add_verify(unit, flags, fg_id, entry_info_p, entry_handle_p));

    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID)
    {
        /** Validate entry handle's params */
        uint32 clean_entry_handle;
        uint8 is_allocated;
        uint32 access_id = DNX_FIELD_ENTRY_TCAM_INTERNAL_ACCESS_ID(*entry_handle_p);

        SHR_IF_ERR_EXIT(dnx_algo_field_entry_tcam_access_id_is_allocated(unit, access_id, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Entry add WITH_ID failed: Given entry handle is already allocated\n");
        }

        /** Validate entry handle has none of the reserved bits set */
        /*
         * Create clean entry
         */
        clean_entry_handle = DNX_FIELD_ENTRY_TCAM_INTERNAL(access_id);
        /*
         * Compare given handle to the clean one
         */
        if (clean_entry_handle != *entry_handle_p)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Entry add WITH_ID failed: Given entry handle has some of its reserved bits turned on\n");
        }
    }
    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)
    {
        dnx_field_entry_t existing_entry_info;
        /** Verify entry exists and const params are not changed */
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get(unit, fg_id, *entry_handle_p, &existing_entry_info));
        /*
         * For single-core devices, core verify check is sufficient 
         */
        if (dnx_data_device.general.nof_cores_get(unit) > 1 && existing_entry_info.core != entry_info_p->core)
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
     * Verify that priority is valid
     * the max_tcam_priority is used internally so it can't be used by user.
     * And the (-1) comes from the fact that we add 1 to our TCAM priorities to avoid
     * checking limits each time.
     */
    if (entry_info_p->priority == DNX_FIELD_TCAM_ENTRY_PRIORITY_INVALID
        || entry_info_p->priority >= dnx_data_field.tcam.max_tcam_priority_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Field group ID %d entry add error: Invalid priority(0x%X): (valid range 0-0x%X).\r\n", fg_id,
                     entry_info_p->priority, dnx_data_field.tcam.max_tcam_priority_get(unit) - 2);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_entry_tcam_dt_add().
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_group_add_flags_e flags.
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
    DNXCMN_CORE_VALIDATE(unit, entry_info_p->core, TRUE);

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
* \param [in] fg_id            - ID of the FG
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
    dnx_field_group_t fg_id,
    dnx_field_entry_key_t * entry_key_info_p,
    uint32 *key)
{
    int qual_idx;
    dnx_field_key_template_t key_template;
    int ii;
    dnx_field_qual_t dnx_qual;
    SHR_FUNC_INIT_VARS(unit);

    *key = 0;
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));

    for (qual_idx = 0;
         qual_idx < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP
         && (dnx_qual = entry_key_info_p->qual_info[qual_idx].dnx_qual) != DNX_FIELD_QUAL_TYPE_INVALID; qual_idx++)
    {
        int qual_val = entry_key_info_p->qual_info[qual_idx].qual_value[0];

        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            if (key_template.key_qual_map[ii].qual_type == DNX_FIELD_QUAL_TYPE_INVALID)
            {
                break;
            }
            if (key_template.key_qual_map[ii].qual_type == dnx_qual)
            {
                dbal_fields_e field_id;
                uint32 hw_val;
                /*
                 * Convert to HW value 
                 */
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, stage, dnx_qual, &field_id));
                SHR_IF_ERR_EXIT(dbal_fields_32_hw_value_get(unit, field_id, qual_val, &hw_val));
                *key |= (hw_val << key_template.key_qual_map[ii].lsb);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Reallocate TCAM cache from old_size to new_size
* \param [in] unit             - Device ID
* \param [in] fg_id            - ID of the FG
* \param [in] old_size         - Old cache size
* \param [in] new_size         - New cache size
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_entry_tcam_add
*/
static shr_error_e
dnx_field_entry_tcam_cache_realloc(
    int unit,
    dnx_field_group_t fg_id,
    int old_size,
    int new_size)
{
    dnx_field_group_tcam_cache_entry_t *entry_list = NULL;
    int nof_valid_entries;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.get(unit, fg_id, &nof_valid_entries));
    /*
     * Create new pointer for updated cache list
     */
    SHR_ALLOC_SET_ZERO(entry_list, sizeof(*entry_list) * new_size, "tcam_cache_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    sal_memcpy(entry_list, dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries, nof_valid_entries *
               sizeof(dnx_field_group_tcam_cache_entry_t));

    /*
     * Copy new cache list pointer to global variable
     */
    SHR_FREE(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries);
    dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries = NULL;
    dnx_field_group_tcam_entry_cache[unit][fg_id].cache_size = new_size;
    dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries = entry_list;

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_install(
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

    /** Get the needed information of the Field Group that the entry is being configured for */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /** Set the HW with entry information */
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_set
                    (unit, flags, field_stage, dbal_table_id, entry_info_p, FALSE, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Update entry from cache for specific TCAM Field group by entry id
* \param [in] unit           - Device ID
* \param [in] fg_id          - Entry's FG ID
* \param [out] entry_info_p  - Pointer to dnx_field_entry_t to update info into TCAM cache
* \param [in] entry_handle   - unique entry handle
* \param [out] entry_found_p - Pointer that specifies whether the entry was found on cache
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_cache_update(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 entry_handle,
    int *entry_found_p)
{
    uint32 entry_idx;
    int nof_valid_entries;
    SHR_FUNC_INIT_VARS(unit);
    *entry_found_p = FALSE;

    if (dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.get(unit, fg_id, &nof_valid_entries));
        for (entry_idx = 0; entry_idx < nof_valid_entries; entry_idx++)
        {
            if (dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_handle == entry_handle)
            {
                *entry_found_p = TRUE;
                sal_memcpy(&(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_info),
                           entry_info_p,
                           sizeof(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_info));
                break;
            }
        }
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
    dnx_field_group_cache_mode_e cache_mode = DNX_FIELD_GROUP_CACHE_MODE_CLEAR;
    int cache_size, nof_valid_entries;
    int entry_found;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input */
    SHR_INVOKE_VERIFY_DNXC(dnx_field_entry_tcam_add_verify(unit, flags, fg_id, entry_info_p, entry_handle_p));

    /** Get the needed information of the Field Group that the entry is being configured for */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.cache_mode.get(unit, fg_id, &cache_mode));

    /*
     * First check if entry exist in cache for update procedure
     */
    if (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_cache_update(unit, fg_id, entry_info_p, *entry_handle_p, &entry_found));
        if (entry_found)
        {
            SHR_EXIT();
        }
    }

    if (cache_mode != DNX_FIELD_GROUP_CACHE_MODE_START || (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        /*
         * On cache mode off or update procedure, commit the entry to HW
         */
        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_install(unit, flags, fg_id, entry_info_p, entry_handle_p));
    }
    else
    {
        int access_id = (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID) ? *entry_handle_p : 0;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_access_id_allocate(unit, &access_id));
        *entry_handle_p = access_id;

        cache_size = dnx_field_group_tcam_entry_cache[unit][fg_id].cache_size;
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.get(unit, fg_id, &nof_valid_entries));

        /*
         * First check there is enough memory in cache to add this entry, otherwise reallocate cache to larger memory
         */
        if (nof_valid_entries == cache_size)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_cache_realloc(unit, fg_id, cache_size, cache_size * 2));
        }

        /*
         * Save new entry to cache
         */
        dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[nof_valid_entries].entry_handle = *entry_handle_p;
        sal_memcpy(&(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[nof_valid_entries].entry_info),
                   entry_info_p,
                   sizeof(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[nof_valid_entries].entry_info));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.inc(unit, fg_id, 1));
    }
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

    SHR_FUNC_INIT_VARS(unit);

    /** Verify the input */
    SHR_INVOKE_VERIFY_DNXC(dnx_field_entry_tcam_dt_add_verify(unit, flags, fg_id, entry_info_p, entry_handle_p));

    /** Get the needed information of the Field Group that the entry is being configured for */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /**
     * For DT FGs, entry handle is created from the key (input quals).
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_dt_key_build(unit, field_stage, fg_id, &(entry_info_p->key_info), &key));
    *entry_handle_p = DNX_FIELD_ENTRY_TCAM_DT(key, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_set
                    (unit, flags, field_stage, dbal_table_id, entry_info_p, TRUE, entry_handle_p));

exit:
    SHR_FUNC_EXIT;
}

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
* \brief
* pads the mask for LPM entries.
* \param [in] unit             - Device ID
* \param [in] fg_id            - Field Group ID
* \param [in] entry_info_p     - Entry info to configure into the database.
* \param [in] dbal_table_id    - The DBAL table of the field group
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_kbp_lpm_pad_mask(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    dbal_tables_e dbal_table_id)
{
    int qual_idx;
    dnx_field_stage_e field_stage;
    uint32 qual_size;
    int field_size;
    dbal_fields_e field_id;
    dbal_core_mode_e core_mode;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
    if (core_mode == DBAL_CORE_MODE_SBC)
    {
        instance = INST_SINGLE;
    }
    else
    {
        int legal_core_id;
        int legal_core_id_found = FALSE;
        /** Get any legal core, since all cores are the same. */
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, legal_core_id)
        {
            instance = legal_core_id;
            legal_core_id_found = TRUE;
            break;
        }
        /** Sanity check*/
        if (legal_core_id_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No valid core found.\n");
        }
    }

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage,
                                                    entry_info_p->key_info.qual_info[qual_idx].dnx_qual, NULL, NULL,
                                                    &qual_size));
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, dbal_table_id, field_id, TRUE, 0, instance, &field_size));
        /** Sanity checks*/
        if (qual_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), DBAL field size %d, "
                         "but qualifier size %d is not positive.\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         field_size, qual_size);
        }
        if (field_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), qualifier size %d, "
                         "but DBAL field size %d is not positive.\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         qual_size, field_size);
        }
        if (qual_size > field_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), qualifier size %d, but DBAL field size %d.\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         qual_size, field_size);
        }
        if (field_size > (SAL_UINT8_NOF_BITS * sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_mask)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), qualifier size %d, DBAL field size %d. "
                         "Maximum size is %d\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         qual_size, field_size,
                         (int) (SAL_UINT8_NOF_BITS * sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_mask)));
        }

        if ((field_size > qual_size) &&
            utilex_bitstream_test_bit(entry_info_p->key_info.qual_info[qual_idx].qual_mask, qual_size - 1))
        {
            int first_bit = qual_size;
            int last_bit = field_size - 1;
            SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry_info_p->key_info.qual_info[qual_idx].qual_mask,
                                                           first_bit, last_bit));
        }

    }

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
/**
* \brief
* removes LPM mask padding
* \param [in] unit             - Device ID
* \param [in] fg_id            - Field Group ID
* \param [in] entry_info_p     - Entry info to configure into the database.
* \param [in] dbal_table_id    - The DBAL table of the field group
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_kbp_lpm_unpad_mask(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    dbal_tables_e dbal_table_id)
{
    int qual_idx;
    dnx_field_stage_e field_stage;
    uint32 qual_size;
    int field_size;
    dbal_fields_e field_id;
    dbal_core_mode_e core_mode;
    int instance;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
    if (core_mode == DBAL_CORE_MODE_SBC)
    {
        instance = INST_SINGLE;
    }
    else
    {
        int legal_core_id;
        int legal_core_id_found = FALSE;
        /** Get any legal core, since all cores are the same. */
        DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, legal_core_id)
        {
            instance = legal_core_id;
            legal_core_id_found = TRUE;
            break;
        }
        /** Sanity check*/
        if (legal_core_id_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "No valid core found.\n");
        }
    }

    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (entry_info_p->key_info.qual_info[qual_idx].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       entry_info_p->key_info.qual_info[qual_idx].dnx_qual, &field_id));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage,
                                                    entry_info_p->key_info.qual_info[qual_idx].dnx_qual, NULL, NULL,
                                                    &qual_size));
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, dbal_table_id, field_id, TRUE, 0, instance, &field_size));
        /** Sanity checks*/
        if (qual_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), DBAL field size %d, "
                         "but qualifier size %d is not positive.\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         field_size, qual_size);
        }
        if (field_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), qualifier size %d, "
                         "but DBAL field size %d is not positive.\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         qual_size, field_size);
        }
        if (qual_size > field_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), qualifier size %d, but DBAL field size %d.\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         qual_size, field_size);
        }
        if (field_size > (SAL_UINT8_NOF_BITS * sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_mask)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "fg_id %d qualifier number %d (name %s), qualifier size %d, DBAL field size %d. "
                         "Maximum size is %d\n",
                         fg_id, qual_idx,
                         dnx_field_dnx_qual_text(unit, entry_info_p->key_info.qual_info[qual_idx].dnx_qual),
                         qual_size, field_size,
                         (int) (SAL_UINT8_NOF_BITS * sizeof(entry_info_p->key_info.qual_info[qual_idx].qual_mask)));
        }

        if (field_size > qual_size)
        {
            int first_bit = qual_size;
            int last_bit = field_size - 1;
            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(entry_info_p->key_info.qual_info[qual_idx].qual_mask,
                                                             first_bit, last_bit));
        }

    }

exit:
    SHR_FUNC_EXIT;
}
#endif

/**
* \brief
* Verify Function for dnx_field_entry_kbp_add().
* \param [in] unit             - Device ID
* \param [in] flags            - dnx_field_entry_group_add_flags_e flags.
* \param [in] fg_id            - Field Group ID
* \param [in] entry_info_p     - Entry info to configure into the database.
* \param [in] entry_handle_p   - The entry handle ID to be filled by the calling function.
* \param [in] dbal_table_id    - The DBAL table of the field group
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_entry_tcam_add
*/
static shr_error_e
dnx_field_entry_external_tcam_add_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p,
    dbal_tables_e dbal_table_id)
{
    dbal_core_mode_e core_mode;
    int device_locked;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");
    SHR_NULL_CHECK(entry_handle_p, _SHR_E_PARAM, "entry_handle_p");

    /** External TCAM entries can only be added after device lock.*/
    SHR_IF_ERR_EXIT(dnx_field_map_is_device_locked(unit, &device_locked));
    if (device_locked == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "External TCAM entries can only be added after device lock. "
                     "Attempted to add an entry to field group %d.\r\n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_entry_standard_lookup_add_verify(unit, flags, fg_id, entry_info_p, entry_handle_p));

    SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));

    if ((core_mode == DBAL_CORE_MODE_DPC) && (entry_info_p->core == BCM_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry with core CORE_ALL, but FG_ID %d has entries_per_core set.\n", fg_id);
    }
    if ((core_mode == DBAL_CORE_MODE_SBC) && (entry_info_p->core != BCM_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry with core that is not CORE_ALL, but FG_ID %d does not have ENTRIES_PER_CORE flag set.\n",
                     fg_id);
    }
    /*
     * Sanity check
     */
    if ((entry_info_p->core != BCM_CORE_ALL) && (entry_info_p->core >= DNX_FIELD_TCAM_CORE_ALL))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal core_id %d\n", DNX_FIELD_TCAM_CORE_ALL);
    }

    if ((flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID) || (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        uint32 access_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_ACCESS_ID(*entry_handle_p);
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            int core_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_PER_CORE_CORE_ID(*entry_handle_p);
            if (DNX_FIELD_ENTRY_TCAM_EXTERNAL_PER_CORE(access_id, core_id) != (*entry_handle_p))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "External TCAM entry_id for fg_id %d (0x%x) is not a legal value, "
                             "and WITH_ID or UPDATE flags set. Illegal entry handle.\r\n", fg_id, *entry_handle_p);
            }
            if (core_id != entry_info_p->core)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "core ID on entry handle %d, core in entry_info is %d, and WITH_ID or UPDATE flags set.\n",
                             core_id, entry_info_p->core);
            }
        }
        else if (core_mode == DBAL_CORE_MODE_SBC)
        {
            if (DNX_FIELD_ENTRY_TCAM_EXTERNAL(access_id) != (*entry_handle_p))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "External TCAM entry_id for fg_id %d (0x%x) is not a legal value, "
                             "and WITH_ID or UPDATE flags set. Some reserved bits are turned on.\r\n",
                             fg_id, *entry_handle_p);
            }
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized core_mode %d for fg_id %d.\n", core_mode, fg_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_external_tcam_add(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_entry_t * entry_info_p,
    uint32 *entry_handle_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage = DNX_FIELD_STAGE_NOF;
    dnx_field_group_cache_mode_e cache_mode = DNX_FIELD_GROUP_CACHE_MODE_CLEAR;
    uint32 access_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /** Verify the input.*/
    SHR_INVOKE_VERIFY_DNXC(dnx_field_entry_external_tcam_add_verify
                           (unit, flags, fg_id, entry_info_p, entry_handle_p, dbal_table_id));

    if ((flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID) || (flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        access_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_ACCESS_ID(*entry_handle_p);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    if (dnx_data_field.external_tcam.type_get(unit) == DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP)
    {
        uint8 is_lpm;
        SHR_IF_ERR_EXIT(dnx_field_kbp_db_is_lpm_get(unit, fg_id, &is_lpm));
        if (is_lpm)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_kbp_lpm_pad_mask(unit, fg_id, entry_info_p, dbal_table_id));
        }
    }
#endif

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_entry_external_tcam_hw_set
                    (unit, flags, field_stage, dbal_table_id, entry_info_p, &access_id));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.cache_mode.get(unit, fg_id, &cache_mode));
    if ((cache_mode == DNX_FIELD_GROUP_CACHE_MODE_START) && !(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        /** Increment the number of cached entries */
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.inc(unit, fg_id, 1));
    }

    if ((!(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_WITH_ID)) && (!(flags & DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE)))
    {
        /*
         * Only after hw_set we can get the external TCAM entry (hw) access_id and encode the entry_id
         * It was allocated automatically after the hw_set
         * */
        if ((access_id & SAL_FROM_BIT(DNX_DATA_FIELD_ENTRY_TCAM_EXTERNAL_ACCESS_ID_SIZE)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "External TCAM access_id larger than %d bits (0x%x).\r\n",
                         DNX_DATA_FIELD_ENTRY_TCAM_EXTERNAL_ACCESS_ID_SIZE, access_id);
        }
        /*
         *   Encode external TCAM entry id per Core ID and Access Id
         */
        {
            dbal_core_mode_e core_mode;
            SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
            if (core_mode == DBAL_CORE_MODE_DPC)
            {
                *entry_handle_p = DNX_FIELD_ENTRY_TCAM_EXTERNAL_PER_CORE(access_id, entry_info_p->core);
            }
            else
            {
                *entry_handle_p = DNX_FIELD_ENTRY_TCAM_EXTERNAL(access_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_state_table_get(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_group_t fg_id,
    int core_id,
    uint32 address,
    dnx_field_entry_key_t * entry_key_info_p)
{
    uint32 data;
    int qual_index;
    dnx_field_qual_t dnx_quals_fg[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    unsigned int nof_quals_fg;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");

    /*
     * Initialization to silence the compiler.
     */
    data = -1;

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_hw_get(unit, core_id, address, &data));

    /*
     * Sanity check that initialziation to silence the compiler wan't needed.
     */
    if (data == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "data not updated.\r\n");
    }

    /*
     * Get the qualifiers on the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals_fg, &nof_quals_fg));
    for (qual_index = 0; qual_index < nof_quals_fg; qual_index++)
    {
        entry_key_info_p->qual_info[qual_index].dnx_qual = dnx_quals_fg[qual_index];
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
    int core_id,
    uint32 address)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deleting is the same as setting the value to 0 in State Table
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_hw_set(unit, core_id, address, DNX_FIELD_ENTRY_STATE_TABLE_DELETE_VAL));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_state_table_add(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_entry_key_t * entry_key_info_p,
    int core_id,
    uint32 *entry_handle_p)
{
    uint32 address;
    uint32 data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_key_info_p, _SHR_E_PARAM, "entry_key_info_p");
    SHR_NULL_CHECK(entry_handle_p, _SHR_E_PARAM, "entry_handle_p");

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_fields_parse(unit, stage, entry_key_info_p, &address, &data));

    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_hw_set(unit, core_id, address, data));

    (*entry_handle_p) = DNX_FIELD_ENTRY_STATE_TABLE(address, core_id);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify Function for dnx_field_entry_exem_add().
* Checks that all qualifiers in the field gorup are given for the entry.
* \param [in] unit           - Device ID
* \param [in] flags          - entry addition flags, currently only supported flag is
*                              DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE
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
    if (flags & (~DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM field group only supports UPDATE flag (0x%x). flags (0x%x) are not supported. "
                     "fg_id %d flags 0x%x.\r\n",
                     DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE,
                     flags & (~DNX_FIELD_ENTRY_GROUP_ADD_FLAG_UPDATE), fg_id, flags);
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
    SHR_INVOKE_VERIFY_DNXC(dnx_field_entry_exem_add_verify(unit, flags, fg_id, entry_info_p));

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
    SHR_INVOKE_VERIFY_DNXC(dnx_field_entry_mdb_dt_add_verify(unit, flags, fg_id, entry_info_p));

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
* \param [in] flags              - dnx_field_entry_ace_add_flags_e flags.
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
    uint8 use_ppmc_range;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(payload_info_p, _SHR_E_PARAM, "payload_info_p");
    SHR_NULL_CHECK(ace_entry_key_p, _SHR_E_PARAM, "ace_entry_key_p");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    SHR_IF_ERR_EXIT(dnx_field_ace_format_uses_ppmc_range(unit, ace_id, &use_ppmc_range));

    /*
     * Verify that the ACE entry has at least one action, since otherwise we could have the same result
     * by not creating the EXEM entry (unless an action has no valid bit, in which case we should anyway add it).
     */
    if (payload_info_p->action_info[0].dnx_action == DNX_FIELD_ACTION_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ACE entry must have at least one action.\r\n");
    }

    if (flags & DNX_FIELD_ENTRY_ACE_ADD_FLAG_UPDATE)
    {
        /*
         * If we update the entry, verify that it is allocated.
         */
        if (use_ppmc_range)
        {
            SHR_IF_ERR_EXIT(dnx_algo_ppmc_mc_replication_id_is_allocated(unit, *ace_entry_key_p, &is_alloc));
            if (is_alloc == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "PPMC ACE entry with key 0x%x not found in allocation manager.",
                             *ace_entry_key_p);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_ace_key_is_allocated(unit, *ace_entry_key_p, &is_alloc));
            if (is_alloc == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ACE entry with key 0x%x not found in allocation manager.",
                             *ace_entry_key_p);
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
        /*
         * Verify that every action without valid bit appears in the entry (unless void action).
         */
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
    SHR_INVOKE_VERIFY_DNXC(dnx_field_ace_entry_add_verify(unit, flags, ace_id, payload_info_p, ace_entry_key_p));

    /*
     * Get the result_type_index.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_result_type_index_get(unit, ace_id, &result_type_index));

    if (flags & DNX_FIELD_ENTRY_ACE_ADD_FLAG_UPDATE)
    {
        is_update = TRUE;
    }
    else
    {
        /*
         * If we create a new entry, allocate entry key, if the ACE format is of type ACE (PMF extension).
         */
        is_update = FALSE;
        if ((flags & DNX_FIELD_ENTRY_ACE_AVOID_ALLOCATION) == 0)
        {
            uint8 use_ppmc_range;
            SHR_IF_ERR_EXIT(dnx_field_ace_format_uses_ppmc_range(unit, ace_id, &use_ppmc_range));
            if (use_ppmc_range)
            {
                uint32 ppmc_flags;
                int ppmc_entry;
                ppmc_flags =
                    (flags & DNX_FIELD_ENTRY_ACE_ADD_FLAG_WITH_ID) ? (BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID) : (0);
                ppmc_entry = *ace_entry_key_p;
                SHR_IF_ERR_EXIT(dnx_algo_ppmc_mc_replication_id_alloc(unit, ppmc_flags, &ppmc_entry));
                *ace_entry_key_p = ppmc_entry;
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_ace_entry_key_allocate(unit, flags, ace_id, ace_entry_key_p));
            }
        }
    }

    /**Set the HW with entry information*/
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_hw_set(unit, result_type_index, is_update, *ace_entry_key_p, payload_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete TCAM Entry From cache
* \param [in] unit           - Device ID
* \param [in] fg_id          - Entry's FG ID
* \param [in] entry_handle   - unique identifier of entry
* \param [out] entry_found_p - Pointer that specifies whether the entry was found on cache
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_cache_delete(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle,
    int *entry_found_p)
{
    uint32 entry_idx;
    int nof_valid_entries;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_found_p, _SHR_E_PARAM, "entry_found");
    *entry_found_p = FALSE;
    if (dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.get(unit, fg_id, &nof_valid_entries));
        for (entry_idx = 0; entry_idx < nof_valid_entries; entry_idx++)
        {
            if (dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_handle == entry_handle)
            {
                *entry_found_p = TRUE;
                break;
            }
        }

        if (*entry_found_p)
        {
            /*
             * If entry was found in cache, move the last entry in cache to its place
             */
            dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_handle =
                DNX_FIELD_KEY_ID_INVALID;
            if (entry_idx != nof_valid_entries - 1)
            {
                sal_memcpy(&(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_info),
                           &(dnx_field_group_tcam_entry_cache[unit][fg_id].
                             tcam_entries[nof_valid_entries - 1].entry_info),
                           sizeof(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_info));
                dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_handle =
                    dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[nof_valid_entries - 1].entry_handle;
                dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[nof_valid_entries - 1].entry_handle =
                    DNX_FIELD_KEY_ID_INVALID;
            }
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.dec(unit, fg_id, 1));

            /*
             * Delete the access ID 
             */
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_access_id_free(unit, entry_handle));
        }
    }
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
    int entry_found;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /*
     * First check whether entry exist on cache
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_cache_delete(unit, fg_id, entry_handle, &entry_found));
    if (entry_found)
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_handle, &entry_fg_id));
    if (entry_fg_id != fg_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Entry delete failed: Given field group ID (%d) is different than the field group to which entry belongs (%d).\n",
                     fg_id, entry_fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &tcam_handler_id));

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_clear(unit, field_stage, dbal_table_id, key_info_p, entry_handle));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_external_tcam_delete(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    uint32 access_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_ACCESS_ID(entry_handle);
    dnx_field_group_cache_mode_e cache_mode = DNX_FIELD_GROUP_CACHE_MODE_CLEAR;
    uint32 entry_handle_id;
    uint32 is_entry_cached = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.cache_mode.get(unit, fg_id, &cache_mode));
    if (cache_mode == DNX_FIELD_GROUP_CACHE_MODE_START)
    {
        /** When caching is enabled, request indication whether the deleted entry is cached or not. */
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, DBAL_ENTRY_ATTR_CACHED, &is_entry_cached));
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    /*
     * Set the core
     */
    {
        dbal_core_mode_e core_mode;
        SHR_IF_ERR_EXIT(dbal_tables_core_mode_get(unit, dbal_table_id, &core_mode));
        if (core_mode == DBAL_CORE_MODE_DPC)
        {
            int core_id = DNX_FIELD_ENTRY_TCAM_EXTERNAL_PER_CORE_CORE_ID(entry_handle);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, (DBAL_COMMIT)));

    if ((cache_mode == DNX_FIELD_GROUP_CACHE_MODE_START) && is_entry_cached)
    {
        /** Update the cached entry counter accordingly. */
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.dec(unit, fg_id, 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
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

    if (DNX_FIELD_ACE_KEY_IS_ACE(unit, ace_entry_key))
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_ace_key_deallocate(unit, ace_entry_key));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_ppmc_mc_replication_id_free(unit, ace_entry_key));
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_tcam_cache_get(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle,
    uint8 *entry_found_p,
    dnx_field_entry_t * entry_info_p)
{
    uint32 entry_idx;
    int nof_valid_entries;
    SHR_FUNC_INIT_VARS(unit);
    *entry_found_p = FALSE;
    if (dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.nof_valid_entries.get(unit, fg_id, &nof_valid_entries));
        for (entry_idx = 0; entry_idx < nof_valid_entries; entry_idx++)
        {
            if (dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_handle == entry_handle)
            {
                *entry_found_p = TRUE;
                sal_memcpy(entry_info_p,
                           &(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_info),
                           sizeof(dnx_field_group_tcam_entry_cache[unit][fg_id].tcam_entries[entry_idx].entry_info));
                break;
            }
        }
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
    uint8 entry_found;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /*
     * First check whether entry exist on cache
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_cache_get(unit, fg_id, entry_handle, &entry_found, entry_info_p));
    if (entry_found)
    {
        SHR_EXIT();
    }

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

    /*
     * Get Actions/Quals values for the given entry
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_quals_actions_type_get(unit, fg_id, entry_info_p));
    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hw_get(unit, field_stage, fg_id, dbal_table_id, entry_handle, entry_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_entry.h*/
shr_error_e
dnx_field_entry_external_tcam_get(
    int unit,
    dnx_field_group_t fg_id,
    uint32 entry_handle,
    uint32 *cached_indication,
    dnx_field_entry_t * entry_info_p)
{
    dbal_tables_e dbal_table_id = DBAL_TABLE_EMPTY;
    dnx_field_stage_e field_stage;
    int device_locked;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info_p, _SHR_E_PARAM, "entry_info_p");

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /** External TCAM entries can only be added after device lock.*/
    SHR_IF_ERR_EXIT(dnx_field_map_is_device_locked(unit, &device_locked));
    if (device_locked == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "External TCAM entries can only be added after device lock. "
                     "Attempted to get an entry to field group %d.\r\n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, entry_info_p));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Get Actions/Quals values for the given entry
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_quals_actions_type_get(unit, fg_id, entry_info_p));
    SHR_IF_ERR_EXIT(dnx_field_entry_external_tcam_hw_get
                    (unit, field_stage, fg_id, dbal_table_id, entry_handle, cached_indication, entry_info_p));

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)
    if (dnx_data_field.external_tcam.type_get(unit) == DNX_FIELD_EXTERNAL_TCAM_TYPE_KBP)
    {
        uint8 is_lpm;
        SHR_IF_ERR_EXIT(dnx_field_kbp_db_is_lpm_get(unit, fg_id, &is_lpm));
        if (is_lpm)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_kbp_lpm_unpad_mask(unit, fg_id, entry_info_p, dbal_table_id));
        }
    }
#endif

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
    {
        uint8 use_ppmc_range;
        SHR_IF_ERR_EXIT(dnx_field_ace_format_uses_ppmc_range(unit, ace_id, &use_ppmc_range));
        if ((use_ppmc_range == FALSE) && (DNX_FIELD_ACE_KEY_IS_ACE(unit, ace_entry_key)) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "ACE format ID (%d) uses ACE range, but the key (%d) is not in the ACE range.\r\n",
                         ace_id, ace_entry_key);
        }
        if (use_ppmc_range && (DNX_FIELD_ACE_KEY_IS_ACE(unit, ace_entry_key)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "ACE format ID (%d) does not use the ACE range, but the key (%d) does.\r\n",
                         ace_id, ace_entry_key);
        }
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

    /*
     * Dummy core set, real core_id is resolved from access_id at the access layer 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    *entry_hit_core_bmp_p = entry_hit_core_bmp_uint32;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function flushes hit information of a single entry.
* \param [in] unit           - Device ID
* \param [in] fg_id          - field group id.
* \param [in] dbal_table_id  - DBAL table of the field group.
* \param [in] entry_handle   - The entry to flush the hitbit for.
*  entry handle.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_tcam_hit_flush_single(
    int unit,
    dnx_field_group_t fg_id,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle)
{
    uint32 entry_handle_id;
    uint32 entry_hit_core_bmp_uint32;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_CLEAR;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &entry_hit_core_bmp_uint32));

    /*
     * Dummy core set, real core_id is resolved from access_id at the access layer 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

exit:
    DBAL_FUNC_FREE_VARS;
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
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_FIELD_ENTRY_HIT_FLUSH_ALL)
    {
        uint32 tcam_handler_id;
        uint32 entry_iter;
        uint32 next_iter;
        bcm_core_t core_id;
        uint8 fg_is_allocated;
        dnx_field_group_type_e fg_type;
        int nof_fg_ids = dnx_data_field.group.nof_fgs_get(unit);

        for (fg_id = 0; fg_id < nof_fg_ids; fg_id++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &fg_is_allocated));
            if (fg_is_allocated == FALSE)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
            if ((fg_type != DNX_FIELD_GROUP_TYPE_TCAM) && (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM))
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
            DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core_id)
            {
                /*
                 * TCAM handler provides an iterator to loop on all entries for the field group
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core_id, tcam_handler_id, &entry_iter));
                while (entry_iter != DNX_FIELD_ENTRY_ACCESS_ID_INVALID)
                {
                    /*
                     * Old trick of saving next iterator before deleting the current one
                     */
                    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next
                                    (unit, core_id, tcam_handler_id, entry_iter, &next_iter));

                    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_flush_single(unit, fg_id, dbal_table_id, entry_iter));

                    entry_iter = next_iter;
                }
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_entry_fg_id(unit, entry_handle, &fg_id));
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
        SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
        {
            LOG_CLI_EX("\r\n" "The given entry %d (0x%08X) is invalid.\r\n %s%s", entry_handle, entry_handle, EMPTY,
                       EMPTY);
            SHR_EXIT();
        }

        dnx_field_entry_tcam_hit_flush_single(unit, fg_id, dbal_table_id, entry_handle);
    }

exit:
    SHR_FUNC_EXIT;
}
