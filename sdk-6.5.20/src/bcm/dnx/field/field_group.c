/** \file field_group.c
 * 
 *
 * Field Group procedures for DNX.
 *
 * Database is one of the main structures that hold PMF field group configuration
 * This file will implement all needed procedures for database creation/deletion etc..
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
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_key.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_manager_access.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/types/dnx_algo_field_types.h>
#include <soc/dnx/dbal/dbal_dynamic.h>

/*
 * }Include files
 */

/**
* \brief
*  Verify function for dnx_field_group_sw_state_update().
* \param [in] unit        - Device Id
* \param [in] fg_id       - Field group ID of the Field group that was created and configured
* \param [in] fg_info_p       - Pointer to Field group information to be saved in SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_group_sw_state_update.
*/
static shr_error_e
dnx_field_group_sw_state_update_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    unsigned int nof_actions;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /*
     * If the maximum number of actions on Field group is smaller than the maximum number of actions on all group types,
     * verify that we do not exceed the maximum number of actions on Field group
     */
    if (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        for (nof_actions = 0;
             nof_actions < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG + 1
             && fg_info_p->actions_payload_info.actions_on_payload_info[nof_actions].dnx_action !=
             DNX_FIELD_ACTION_INVALID; nof_actions++);
        if (nof_actions > DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field group cannot accept more than %d actions.\n",
                         DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Update the sw state after creating a field_group.
* \param [in] unit        - Device Id
* \param [in] fg_id       - Field group id which was created and configured
* \param [in] fg_info_p       - Pointer to field group information to be saved in SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_sw_state_update(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    unsigned int action_ndx;
    unsigned int qual_ndx;
    unsigned int context_ndx;
    unsigned int bank_ndx;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_group_sw_state_update_verify(unit, fg_id, fg_info_p));

    /*
     * Convert actions_payload_info to compact form.
     */
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.actions_payload_info.
                        actions_on_payload_info.set(unit, fg_id, action_ndx,
                                                    &(fg_info_p->actions_payload_info.
                                                      actions_on_payload_info[action_ndx])));
    }
    /*
     * The rest of the fields remain unchanged.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.field_stage.set(unit, fg_id, fg_info_p->field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.fg_type.set(unit, fg_id, fg_info_p->fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.set(unit, fg_id, &(fg_info_p->group_name)));

    for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; context_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.context_info_valid.set(unit, fg_id, context_ndx,
                                                                                       fg_info_p->context_info
                                                                                       [context_ndx].context_info_valid));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        key_id.set(unit, fg_id, context_ndx, &fg_info_p->context_info[context_ndx].key_id));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.key_id.
                        bit_range.set(unit, fg_id, context_ndx,
                                      &fg_info_p->context_info[context_ndx].key_id.bit_range));
        for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                            qual_attach_info.set(unit, fg_id, context_ndx, qual_ndx,
                                                 &fg_info_p->context_info[context_ndx].qual_attach_info[qual_ndx]));
        }
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.nof_cascading_refs.set(unit, fg_id, context_ndx,
                                                                                       fg_info_p->context_info
                                                                                       [context_ndx].nof_cascading_refs));
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_template.set(unit, fg_id, &fg_info_p->key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_length_type.set(unit, fg_id, fg_info_p->key_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.action_length_type.set(unit, fg_id, fg_info_p->action_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.dbal_table_id.set(unit, fg_id, fg_info_p->dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.flags.set(unit, fg_id, fg_info_p->flags));

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                    auto_tcam_bank_select.set(unit, fg_id, fg_info_p->tcam_info.bank_allocation_mode));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                    nof_tcam_banks.set(unit, fg_id, fg_info_p->tcam_info.nof_tcam_banks));
    /*
     * Make sure tcam_bank_ids array is allocated
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.tcam_bank_ids.is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.tcam_bank_ids.alloc(unit, fg_id));
    }
    for (bank_ndx = 0; bank_ndx < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                        tcam_bank_ids.set(unit, fg_id, bank_ndx, fg_info_p->tcam_info.tcam_bank_ids[bank_ndx]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_ace_format_sw_state_update().
* \param [in] unit        - Device Id
* \param [in] ace_id       - ACE ID of the ACE format that was created and configured
* \param [in] ace_format_info_p       - Pointer to ACE Format information to be saved in SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * dnx_field_ace_format_sw_state_update.
*/
static shr_error_e
dnx_field_ace_format_sw_state_update_verify(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_info_for_sw_t * ace_format_info_p)
{
    unsigned int nof_actions;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    /*
     * If the maximum number of actions on ACE format is smaller than the maximum number of actions on all group types,
     * verify that we do not exceed the maximum number of actions on ACE format
     */
    if (DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        for (nof_actions = 0;
             nof_actions < DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT + 1
             && ace_format_info_p->actions_payload_info.actions_on_payload_info[nof_actions].dnx_action !=
             DNX_FIELD_ACTION_INVALID; nof_actions++);
        if (nof_actions > DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ACE Format cannot accept more than %d actions.\n",
                         DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Update the sw state after created an ACE Format
* \param [in] unit        - Device Id
* \param [in] ace_id       - ACE ID of the ACE format that was created and configured
* \param [in] ace_format_info_p       - Pointer to ACE format information to be saved in SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * This function receives dnx_field_ace_format_info_for_sw_t and converts is to a packed format of
*     dnx_field_ace_format_sw_info_t
* \see
*   * dnx_field_ace_format_add.
*   * dnx_field_ace_format_delete.
*   * dnx_field_group_sw_state_update
*/
static shr_error_e
dnx_field_ace_format_sw_state_update(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_info_for_sw_t * ace_format_info_p)
{
    unsigned int action_ndx;
    dnx_field_ace_format_sw_info_t ace_format_info_sw;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_format_sw_state_update_verify(unit, ace_id, ace_format_info_p));

    /*
     * Convert the format used by the functions to the format used by the SW states.
     */
    sal_memset(&ace_format_info_sw, 0x0, sizeof(ace_format_info_sw));
    /*
     * Convert actions_payload_info to compact form.
     */
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT; action_ndx++)
    {
        ace_format_info_sw.actions_payload_info.actions_on_payload_info[action_ndx] =
            ace_format_info_p->actions_payload_info.actions_on_payload_info[action_ndx];
    }
    /*
     * The rest of the fields remain unchanged.
     */
    ace_format_info_sw.flags = ace_format_info_p->flags;
    sal_strncpy_s(ace_format_info_sw.name.value, ace_format_info_p->name, sizeof(ace_format_info_sw.name));

    SHR_IF_ERR_EXIT(dnx_field_group_sw.ace_format_info.set(unit, ace_id, &ace_format_info_sw));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Invalidate (init) info of struct dnx_field_attach_context_info_t
* \param [in] unit            - Device Id
* \param [in] context_info_p  - Pointer to struct to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_field_attach_context_info_t_init(
    int unit,
    dnx_field_attach_context_info_t * context_info_p)
{
    int qual_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_info_p, _SHR_E_PARAM, "context_info_p");

    sal_memset(context_info_p, 0x0, sizeof(*context_info_p));

    context_info_p->context_info_valid = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_key_id_t_init(unit, &(context_info_p->key_id)));
    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &(context_info_p->qual_attach_info[qual_idx])));
    }
    context_info_p->nof_cascading_refs = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the struct dnx_field_group_kbp_segment_info_t, used in the SW state of hte master key for the KBP
*  (external TCAM field groups in external stage).
* \param [in] unit            - Device Id
* \param [in] fg_segment_info_p  - Pointer to struct to init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_field_group_kbp_segment_info_t_init(
    int unit,
    dnx_field_group_kbp_segment_info_t * fg_segment_info_p)
{
    int packed_qual_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_segment_info_p, _SHR_E_PARAM, "fg_segment_info_p");

    sal_memset(fg_segment_info_p, 0x0, sizeof(*fg_segment_info_p));

    fg_segment_info_p->fg_id = DNX_FIELD_GROUP_INVALID;

    for (packed_qual_idx = 0; packed_qual_idx < DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT; packed_qual_idx++)
    {
        fg_segment_info_p->qual_idx[packed_qual_idx] = DNX_FIELD_GROUP_EXTERNAL_PACKED_QUAL_INDEX_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear the dnx_field_group_fg_info_for_sw_t, set it to preferred init values
* \param [in] unit       - Device ID
* \param [in] fg_info_p  - Pointer to field group SW state structure that needs to be cleared
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_fg_info_for_sw_t_init(
    int unit,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    dnx_field_context_t context_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    context_idx = 0;

    sal_memset(fg_info_p, 0x0, sizeof(*fg_info_p));

    fg_info_p->field_stage = DNX_FIELD_STAGE_NOF;
    fg_info_p->fg_type = DNX_FIELD_GROUP_TYPE_NOF;
    sal_memset(&(fg_info_p->group_name), 0, sizeof(fg_info_p->group_name));

    SHR_IF_ERR_EXIT(dnx_field_key_template_t_init(unit, &(fg_info_p->key_template)));

    /**Init data that is held per context ID*/
    for (context_idx = 0; context_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; context_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_attach_context_info_t_init(unit, &fg_info_p->context_info[context_idx]));
    }
    /*
     * Mark all 'action info' array as EMPTY.
     */
    fg_info_p->actions_payload_info.actions_on_payload_info[0].dnx_action = DNX_FIELD_ACTION_INVALID;
    /*
     * Init the following fields as they are not used by all field groups.
     */
    fg_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
    fg_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;
    fg_info_p->dbal_table_id = DBAL_TABLE_EMPTY;

    /*
     * It's sufficient to set nof_tcam_bank to zero in order to init tcam_info
     */
    fg_info_p->tcam_info.nof_tcam_banks = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the sw state of a field group.
* \param [in] unit        - Device Id
* \param [in] fg_id       - Field group id which was created and configured
* \param [in] fg_info_p       - Pointer to field group information to be saved in SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_sw_state_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    unsigned int action_ndx;
    unsigned int context_ndx;
    unsigned int bank_ndx;
    unsigned int qual_ndx;
    field_group_name_t group_name;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_fg_info_for_sw_t_init(unit, fg_info_p));

    /*
     * Now copy the data from the compacted structure and add Invalids in case the arrays are full.
     */
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.actions_payload_info.
                        actions_on_payload_info.get(unit, fg_id, action_ndx,
                                                    &(fg_info_p->actions_payload_info.
                                                      actions_on_payload_info[action_ndx])));
    }
    if (DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        fg_info_p->actions_payload_info.actions_on_payload_info[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG].dnx_action =
            DNX_FIELD_ACTION_INVALID;
    }
    /*
     * The rest of the fields remain unchanged.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.field_stage.get(unit, fg_id, &fg_info_p->field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.fg_type.get(unit, fg_id, &fg_info_p->fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_id, &group_name));
    /** Get the name of the field group and return it in the basic FG structure. */
    sal_strncpy_s(fg_info_p->group_name.value, group_name.value, sizeof(fg_info_p->group_name.value));
    for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; context_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.context_info_valid.get(unit, fg_id, context_ndx,
                                                                                       &fg_info_p->context_info
                                                                                       [context_ndx].context_info_valid));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        key_id.get(unit, fg_id, context_ndx, &fg_info_p->context_info[context_ndx].key_id));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.key_id.
                        bit_range.get(unit, fg_id, context_ndx,
                                      &fg_info_p->context_info[context_ndx].key_id.bit_range));
        for (qual_ndx = 0; qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                            qual_attach_info.get(unit, fg_id, context_ndx, qual_ndx,
                                                 &(fg_info_p->context_info[context_ndx].qual_attach_info[qual_ndx])));
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_template.get(unit, fg_id, &fg_info_p->key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_length_type.get(unit, fg_id, &fg_info_p->key_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.action_length_type.get(unit, fg_id, &fg_info_p->action_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.dbal_table_id.get(unit, fg_id, &fg_info_p->dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.flags.get(unit, fg_id, &fg_info_p->flags));

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                    auto_tcam_bank_select.get(unit, fg_id, &fg_info_p->tcam_info.bank_allocation_mode));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                    nof_tcam_banks.get(unit, fg_id, &fg_info_p->tcam_info.nof_tcam_banks));
    for (bank_ndx = 0; bank_ndx < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.
                        tcam_bank_ids.get(unit, fg_id, bank_ndx, &fg_info_p->tcam_info.tcam_bank_ids[bank_ndx]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the sw state of an ACE format.
* \param [in] unit              - Device Id
* \param [in] ace_id            - ACE format ID that we want to retrieve.
* \param [in] ace_format_info_p - Pointer to ACE format information that was saved in SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_sw_state_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_info_for_sw_t * ace_format_info_p)
{
    unsigned int action_ndx;
    dnx_field_ace_format_sw_info_t ace_format_info_sw;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * The structure in SW state has smaller arrays than the one used by the functions (to different group types to use
     * the same functions without fear of error).
     * Read the compacted array.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.ace_format_info.get(unit, ace_id, &ace_format_info_sw));

    /*
     * Now copy the data from the compacted structure and add Invalids in case the arrays are full.
     */
    sal_memset(ace_format_info_p, 0x0, sizeof(*ace_format_info_p));
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT; action_ndx++)
    {
        ace_format_info_p->actions_payload_info.actions_on_payload_info[action_ndx] =
            ace_format_info_sw.actions_payload_info.actions_on_payload_info[action_ndx];
    }
    if (DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        ace_format_info_p->actions_payload_info.
            actions_on_payload_info[DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT].dnx_action =
            DNX_FIELD_ACTION_INVALID;
    }
    /*
     * The rest of the field remain unchanged.
     */
    ace_format_info_p->flags = ace_format_info_sw.flags;
    sal_strncpy_s(ace_format_info_p->name, ace_format_info_sw.name.value, sizeof(ace_format_info_p->name));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Clear the dnx_field_ace_format_info_for_sw_t, set it to preferred init values
* \param [in] unit       - Device ID
* \param [in] ace_format_info_p  - Pointer to ACE format SW state structure that needs to be cleared
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_info_for_sw_t_init(
    int unit,
    dnx_field_ace_format_info_for_sw_t * ace_format_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ace_format_info_p, _SHR_E_PARAM, "ace_format_info_p");

    sal_memset(ace_format_info_p, 0x0, sizeof(*ace_format_info_p));

    /*
     * Mark 'action info' array as EMPTY.
     */
    ace_format_info_p->actions_payload_info.actions_on_payload_info[0].dnx_action = DNX_FIELD_ACTION_INVALID;
exit:
    SHR_FUNC_EXIT;
}

/** See field_group.h */
shr_error_e
dnx_field_group_info_t_init(
    int unit,
    dnx_field_group_info_t * fg_info_in_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fg_info_in_p, _SHR_E_PARAM, "fg_info_in_p");

    sal_memset(fg_info_in_p, 0x0, sizeof(*fg_info_in_p));

    fg_info_in_p->field_stage = DNX_FIELD_STAGE_INVALID;

    fg_info_in_p->fg_type = DNX_FIELD_GROUP_TYPE_INVALID;

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; ii++)
    {
        fg_info_in_p->dnx_actions[ii] = DNX_FIELD_ACTION_INVALID;
        fg_info_in_p->use_valid_bit[ii] = TRUE;
    }
    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        fg_info_in_p->dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    fg_info_in_p->tcam_info.nof_tcam_banks = 0;

exit:
    SHR_FUNC_EXIT;
}

/** See field_group.h */
shr_error_e
dnx_field_ace_format_info_t_init(
    int unit,
    dnx_field_ace_format_info_t * ace_format_info_in_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ace_format_info_in_p, _SHR_E_PARAM, "ace_format_info_in_p");

    sal_memset(ace_format_info_in_p, 0x0, sizeof(*ace_format_info_in_p));

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; ii++)
    {
        ace_format_info_in_p->dnx_actions[ii] = DNX_FIELD_ACTION_INVALID;
        ace_format_info_in_p->use_valid_bit[ii] = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_group.h */
shr_error_e
dnx_field_group_attach_info_t_init(
    int unit,
    dnx_field_group_attach_info_t * attach_info_p)
{
    int ii;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(attach_info_p, _SHR_E_PARAM, "attach_info_p");

    sal_memset(attach_info_p, 0x0, sizeof(*attach_info_p));

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; ii++)
    {
        attach_info_p->dnx_actions[ii] = DNX_FIELD_ACTION_INVALID;
    }

    SHR_IF_ERR_EXIT(dnx_field_action_attach_info_t_init(unit, attach_info_p->action_info));

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        attach_info_p->dnx_quals[ii] = DNX_FIELD_QUAL_TYPE_INVALID;
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &attach_info_p->qual_info[ii]));
    }

    for (ii = 0; ii < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID; ii++)
    {
        attach_info_p->fems_invalidate[ii] = DNX_FIELD_FEM_ID_INVALID;
    }

    attach_info_p->compare_id = DNX_FIELD_GROUP_COMPARE_ID_NONE;

exit:
    SHR_FUNC_EXIT;
}

/** See field_group.h */
shr_error_e
dnx_field_group_sw_state_init(
    int unit)
{
    dnx_field_group_t fg_id = 0;
    int nof_fgs = dnx_data_field.group.nof_fgs_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init and alloc.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.init(unit));

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.alloc(unit));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.ace_format_info.alloc(unit));

    for (fg_id = 0; fg_id < nof_fgs; fg_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.tcam_info.tcam_bank_ids.alloc(unit, fg_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_group.h */
shr_error_e
dnx_field_group_sw_state_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_group.h
 */
shr_error_e
dnx_field_group_context_id_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS])
{
    int context_idx = 0;
    int out_arr_idx = 0;
    uint8 context_valid = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(context_id, _SHR_E_PARAM, "context_id");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /** Copy all the context IDs for Field Group*/
    for (context_idx = 0; context_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS; context_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.context_info_valid.get(unit, fg_id,
                                                                                       context_idx, &context_valid));
        if (context_valid == TRUE)
        {
            context_id[out_arr_idx] = context_idx;
            context_valid = FALSE;
            out_arr_idx++;
        }
    }

    /** Mark the end of valid context ids by an invalid id*/
    if (out_arr_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)
    {
        context_id[out_arr_idx] = DNX_FIELD_CONTEXT_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See procedure header in field_group.h
 */
shr_error_e
dnx_field_group_is_context_id_on_arr(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    unsigned int *found_p)
{
    uint8 context_valid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(found_p, _SHR_E_PARAM, "found_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);
    *found_p = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                    context_info_valid.get(unit, fg_id, context_id, &context_valid));
    if (context_valid)
    {
        *found_p = TRUE;
    }
    else
    {
        *found_p = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_group.h
 */
shr_error_e
dnx_field_group_qualifier_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t qualifier,
    unsigned int *qualifier_bit_lsb_location_p,
    unsigned int *qualifier_bit_width_p)
{
    int qual_ndx, qual_found;
    dnx_field_key_template_t key_template;
    dnx_field_qual_map_in_key_t *key_qual_map_p;

    SHR_FUNC_INIT_VARS(unit);
    qual_ndx = 0;
    qual_found = FALSE;
    SHR_NULL_CHECK(qualifier_bit_lsb_location_p, _SHR_E_PARAM, "qualifier_bit_location_p");
    SHR_NULL_CHECK(qualifier_bit_width_p, _SHR_E_PARAM, "qualifier_bit_width_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /**Init the key template to invalid values*/
    SHR_IF_ERR_EXIT(dnx_field_key_template_t_init(unit, &key_template));

    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));

    /** Scan all qualifiers of the key to find the desired one*/
    for (qual_ndx = 0;
         (qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[qual_ndx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID) && (!qual_found); qual_ndx++)
    {
        key_qual_map_p = &(key_template.key_qual_map[qual_ndx]);
        /** When the qualifier is found will the params*/
        if (DNX_QUAL_IS_SAME(qualifier, key_qual_map_p->qual_type))
        {
            qual_found = TRUE;
            *qualifier_bit_lsb_location_p = key_qual_map_p->lsb;
            *qualifier_bit_width_p = key_qual_map_p->size;
        }
    }
    if (qual_found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "For FG_ID = %d , qual_type =%d not found\n", fg_id, qualifier);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file field_group.h
 */
shr_error_e
dnx_field_group_dbal_table_id_get(
    int unit,
    dnx_field_group_t fg_id,
    dbal_tables_e * dbal_table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dbal_table_id_p, _SHR_E_PARAM, "dbal_table_id_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.dbal_table_id.get(unit, fg_id, dbal_table_id_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_ace_format_result_type_index_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    int *result_type_index_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(result_type_index_p, _SHR_E_PARAM, "result_type_index_p");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    SHR_IF_ERR_EXIT(dbal_tables_hw_value_result_type_get
                    (unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, ace_id, result_type_index_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_ace_format_result_type_index_to_ace_id(
    int unit,
    int result_type_index,
    dnx_field_ace_id_t * ace_id_p)
{
    uint32 hw_value;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_id_p, _SHR_E_PARAM, "ace_id_p");

    /*
     * We get the HW value of the result_type_index which should be the ACE ID.
     * not we could have iterated over all ACE IDs and compare their result type indices.
     */
    SHR_IF_ERR_EXIT(dbal_tables_result_type_hw_value_get
                    (unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, result_type_index, &hw_value));
    (*ace_id_p) = hw_value;

    /** Sanity check.*/
    DNX_FIELD_ACE_ID_VERIFY(unit, *ace_id_p);

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_field_stage_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_stage_e * field_stage_p)
{
    uint8 is_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(field_stage_p, _SHR_E_PARAM, "field_stage_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.field_stage.get(unit, fg_id, field_stage_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_type_e * fg_type_p)
{
    uint8 is_allocated = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_type_p, _SHR_E_PARAM, "fg_type_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.fg_type.get(unit, fg_id, fg_type_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_key_length_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_key_length_type_e * key_length_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_length_type_p, _SHR_E_PARAM, "key_length_type_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_length_type.get(unit, fg_id, key_length_type_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_action_length_type_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_length_type_e * action_length_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(action_length_type_p, _SHR_E_PARAM, "action_length_type_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.action_length_type.get(unit, fg_id, action_length_type_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_tcam_handler_id_get(
    int unit,
    dnx_field_group_t fg_id,
    uint32 *tcam_handler_id_p)
{
    dbal_tables_e dbal_table_id;
    dnx_field_group_type_e fg_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_handler_id_p, _SHR_E_PARAM, "tcam_handler_id_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM || fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, tcam_handler_id_p));
    }
    else
    {
        *tcam_handler_id_p = DNX_FIELD_TCAM_HANDLER_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_action_template_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_actions_fg_payload_info_t * actions_payload_info_p)
{
    unsigned int action_ndx;
    dnx_field_actions_fg_payload_sw_info_t actions_payload_info_sw;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_payload_info_p, _SHR_E_PARAM, "actions_payload_info_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /*
     * The structure in SW state has smaller arrays than the one used by the functions (to different group types to use
     * the same functions without fear of error).
     * Read the compacted array.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.actions_payload_info.get(unit, fg_id, &actions_payload_info_sw));

    /*
     * Now copy the data from the compacted structure and add Invalids in case the arrays are full.
     */
    sal_memset(actions_payload_info_p, 0x0, sizeof(*actions_payload_info_p));
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG; action_ndx++)
    {
        actions_payload_info_p->actions_on_payload_info[action_ndx] =
            actions_payload_info_sw.actions_on_payload_info[action_ndx];
    }
    /** If we did not reach the end of the array, mark the end with an invalid action.*/
    if (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP > DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG)
    {
        actions_payload_info_p->actions_on_payload_info[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG].dnx_action =
            DNX_FIELD_ACTION_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_action_nof_get(
    int unit,
    dnx_field_group_t fg_id,
    unsigned int *nof_actions_p)
{
    unsigned int action_ndx;
    dnx_field_actions_fg_payload_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(nof_actions_p, _SHR_E_PARAM, "nof_actions_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));

    for (action_ndx = 0; (action_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG) &&
         (actions_payload_info.actions_on_payload_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID);
         action_ndx++);

    (*nof_actions_p) = action_ndx;

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_physical_db_vmv_size_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table,
    uint8 *vmv_size_p)
{
    uint8 vmv_value;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vmv_size_p, _SHR_E_PARAM, "vmv_size_p");

    /*
     * Use one type of encoding, assuming all encodings have the same VMV size.
     */
    SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value(unit, dbal_physical_table, MDB_EM_ENTRY_ENCODING_ONE,
                                              vmv_size_p, &vmv_value));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Obtain the parameters of the EXEM PDB used (LEXEM or SEXEM) according to the field stage.
* \param [in] unit                    - Device ID
* \param [in] field_stage             - Field Stage.
* \param [out] exem_info_p            - The paramaeters of the EXEM PDB.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_exem_info_t * exem_info_p)
{
    int lexem_used_by_stage;
    int sexem_used_by_stage;
    int updated;
    uint8 vmv_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(exem_info_p, _SHR_E_PARAM, "exem_info_p");

    /*
     * Initialization to appease the compiler.
     */
    exem_info_p->max_key_size = 0;
    exem_info_p->max_payload_size = 0;
    exem_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
    exem_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;
    exem_info_p->dbal_physical_table_id = DBAL_PHYSICAL_TABLE_NONE;
    exem_info_p->app_db_id_size = 0;
    updated = FALSE;

    lexem_used_by_stage = dnx_data_field.stage.stage_info_get(unit, field_stage)->uses_large_exem;
    sexem_used_by_stage = dnx_data_field.stage.stage_info_get(unit, field_stage)->uses_small_exem;

    if (lexem_used_by_stage && (sexem_used_by_stage == FALSE))
    {
        exem_info_p->dbal_physical_table_id = DBAL_PHYSICAL_TABLE_LEXEM;
        SHR_IF_ERR_EXIT(dnx_field_group_physical_db_vmv_size_get(unit, exem_info_p->dbal_physical_table_id, &vmv_size));
        exem_info_p->max_key_size = dnx_data_field.exem.large_max_key_size_get(unit);
        exem_info_p->max_payload_size = dnx_data_field.exem.large_max_result_size_get(unit) - vmv_size;
        exem_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;
        exem_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM;
        exem_info_p->app_db_id_size = dnx_data_field.exem.large_app_db_id_size_get(unit);
        updated = TRUE;
    }
    else if (sexem_used_by_stage && (lexem_used_by_stage == FALSE))
    {
        exem_info_p->dbal_physical_table_id = DBAL_PHYSICAL_TABLE_SEXEM_3;
        SHR_IF_ERR_EXIT(dnx_field_group_physical_db_vmv_size_get(unit, exem_info_p->dbal_physical_table_id, &vmv_size));
        exem_info_p->max_key_size = dnx_data_field.exem.small_max_key_size_get(unit);
        exem_info_p->max_payload_size = dnx_data_field.exem.small_max_result_size_get(unit) - vmv_size;
        exem_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_HALF;
        exem_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM;
        exem_info_p->app_db_id_size = dnx_data_field.exem.small_app_db_id_size_get(unit);
        updated = TRUE;
    }
    else if (lexem_used_by_stage && sexem_used_by_stage)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage \"%s\"(%d) uses both LEXEM and SEXEM, no decision mechanism "
                     "supported.\r\n", dnx_field_stage_text(unit, field_stage), field_stage);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage does not support EXEM: \"%s\"(%d).\r\n",
                     dnx_field_stage_text(unit, field_stage), field_stage);
    }

    /*
     * Sanity check.
     */
    if (updated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "EXEM information was not filled. Stage %s (%d).\r\n",
                     dnx_field_stage_text(unit, field_stage), field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_action_buffer_shift_get(
    int unit,
    dnx_field_group_t fg_id,
    int *buffer_shift_p)
{
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(buffer_shift_p, _SHR_E_PARAM, "buffer_shift_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    /*
     * Default value/
     */
    (*buffer_shift_p) = 0;

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            /**
             * In JR2_B0 and above, half payload lookups have their results appear on the second half of the payload.
             */
            
            if (dnx_data_field.features.tcam_result_flip_eco_get(unit)
#ifndef ADAPTER_SERVER_MODE
                || dnx_data_field.features.tcam_result_half_payload_on_msb_get(unit)
#endif
                )
            {
                if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
                {
                    dnx_field_action_length_type_e action_length_type;

                    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));
                    if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF)
                    {
                        (*buffer_shift_p) = dnx_data_field.tcam.action_size_half_get(unit);
                    }
                }
                else if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
                {
                    (*buffer_shift_p) = dnx_data_field.tcam.action_size_half_get(unit);
                }
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            if (dnx_data_field.features.exem_vmv_removable_from_payload_get(unit))
            {
                dnx_field_action_length_type_e action_length_type;
                uint8 vmv_size;
                dbal_physical_tables_e dbal_physical_table;

                SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));
                switch (action_length_type)
                {
                    case DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM:
                    {
                        dbal_physical_table = DBAL_PHYSICAL_TABLE_LEXEM;
                        break;
                    }
                    case DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM:
                    {
                        dbal_physical_table = DBAL_PHYSICAL_TABLE_SEXEM_3;
                        break;
                    }
                    default:
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid action_length_type=%d for EXEM.\n", action_length_type);
                        break;
                    }
                }

                SHR_IF_ERR_EXIT(dnx_field_group_physical_db_vmv_size_get(unit, dbal_physical_table, &vmv_size));

                (*buffer_shift_p) = vmv_size;
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            if (dnx_data_field.features.state_table_atomic_rmw_get(unit))
            {
                (*buffer_shift_p) = dnx_data_field.state_table.address_size_entry_max_get(unit) -
                    dnx_data_field.state_table.data_size_entry_max_get(unit);
            }
        }
        default:
        {
            /*
             * Other field group types remain with the default 0.
             */
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_ace_format_action_template_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_actions_ace_payload_info_t * actions_payload_info_p)
{
    unsigned int action_ndx;
    dnx_field_actions_ace_payload_sw_info_t actions_payload_info_sw;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_payload_info_p, _SHR_E_PARAM, "actions_payload_info_p");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    /*
     * The structure in SW state has smaller arrays than the one used by the functions (to different group types to use
     * the same functionswithout fear of error).
     * Read the compacted array.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.ace_format_info.
                    actions_payload_info.get(unit, ace_id, &actions_payload_info_sw));

    /*
     * Now copy the data from the compacted structure and add Invalids in case the arrays are full.
     */
    sal_memset(actions_payload_info_p, 0x0, sizeof(*actions_payload_info_p));
    for (action_ndx = 0; action_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT; action_ndx++)
    {
        actions_payload_info_p->actions_on_payload_info[action_ndx] =
            actions_payload_info_sw.actions_on_payload_info[action_ndx];
    }
    actions_payload_info_p->actions_on_payload_info[DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT].dnx_action =
        DNX_FIELD_ACTION_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_payload_template_min_max_get(
    int unit,
    dnx_field_group_t fg_id,
    int *min_offset_p,
    int *max_offset_p)
{
    int action_lsb;
    int action_msb;
    int total_lsb;
    int total_msb;
    int action_found;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    unsigned int action_ndx;
    unsigned int action_size_w_valid;
    unsigned int action_size_wo_valid;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(min_offset_p, _SHR_E_PARAM, "min_offset_p");
    SHR_NULL_CHECK(max_offset_p, _SHR_E_PARAM, "max_offset_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    action_found = FALSE;

    /** Initialization to appease the compiler. */
    total_lsb = -1;
    total_msb = -1;

    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_payload_info.actions_on_payload_info[action_ndx].dnx_action !=
         DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        /*
         * Calculate the action bit size.
         * Note 'size_wo_valid' is only used for the function interface, and has no use in this function.
         */
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                        (unit, field_stage,
                         &(actions_payload_info.actions_on_payload_info[action_ndx]),
                         &action_size_w_valid, &action_size_wo_valid));
        if (action_size_wo_valid <= 0)
        {
            continue;
        }
        action_lsb = actions_payload_info.actions_on_payload_info[action_ndx].lsb;
        action_msb = action_lsb + action_size_w_valid - 1;
        if (action_msb < action_lsb)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Wrapped around (action_lsb %d action_msb %d fg_id %d action %d).\r\n",
                         action_lsb, action_msb, fg_id, action_ndx);
        }
        if (action_found == FALSE)
        {
            total_lsb = action_lsb;
            total_msb = action_msb;
            action_found = TRUE;
        }
        else
        {
            if (action_lsb < total_lsb)
            {
                total_lsb = action_lsb;
            }
            if (action_msb > total_msb)
            {
                total_msb = action_msb;
            }
        }
    }

    /** Sanity check.*/
    if (total_msb < total_lsb)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Calculated actions on payload msb (%d) to be smaller than the lsb (%d) on field group %d.\r\n",
                     total_msb, total_lsb, fg_id);
    }

    if (action_found)
    {
        (*min_offset_p) = total_lsb;
        (*max_offset_p) = total_msb;
        /** Sanity check.*/
        if (total_lsb < 0 || total_msb < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Found action but range not updated (total_lsb %d total_msb %d fg_id %d).\r\n",
                         total_lsb, total_msb, fg_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_ace_format_payload_template_min_max_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    int *min_offset_p,
    int *max_offset_p)
{
    int action_lsb;
    int action_msb;
    int total_lsb;
    int total_msb;
    int action_found;
    dnx_field_actions_ace_payload_info_t actions_payload_info;
    unsigned int action_ndx;
    unsigned int action_size_w_valid;
    unsigned int action_size_wo_valid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(min_offset_p, _SHR_E_PARAM, "min_offset_p");
    SHR_NULL_CHECK(max_offset_p, _SHR_E_PARAM, "max_offset_p");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    SHR_IF_ERR_EXIT(dnx_field_ace_format_action_template_get(unit, ace_id, &actions_payload_info));

    action_found = FALSE;

    /** Initialization to appease the compiler. */
    total_lsb = -1;
    total_msb = -1;

    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_payload_info.actions_on_payload_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_ndx++)
    {
        /*
         * Calculate the action bit size.
         * Note 'size_wo_valid' is only used for the function interface, and has no use in this function.
         */
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                        (unit, DNX_FIELD_STAGE_ACE, &(actions_payload_info.actions_on_payload_info[action_ndx]),
                         &action_size_w_valid, &action_size_wo_valid));
        if (action_size_wo_valid <= 0)
        {
            continue;
        }
        action_lsb = actions_payload_info.actions_on_payload_info[action_ndx].lsb;
        action_msb = action_lsb + action_size_w_valid - 1;
        if (action_msb < action_lsb)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Wrapped around (action_lsb %d action_msb %d ace_id %d action %d).\r\n",
                         action_lsb, action_msb, ace_id, action_ndx);
        }
        if (action_lsb < 0 || action_msb < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "action_lsb %d action_msb %d. Cannot be negative. ace_id %d action %d.\r\n",
                         action_lsb, action_msb, ace_id, action_ndx);
        }
        if (action_found == FALSE)
        {
            total_lsb = action_lsb;
            total_msb = action_msb;
            action_found = TRUE;
        }
        else
        {
            if (action_lsb < total_lsb)
            {
                total_lsb = action_lsb;
            }
            if (action_msb > total_msb)
            {
                total_msb = action_msb;
            }
        }
    }

    /** Sanity check.*/
    if (total_msb < total_lsb)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Calculated actions on payload msb (%d) to be smaller than the lsb (%d) on ACE format %d.\r\n",
                     total_msb, total_lsb, ace_id);
    }

    if (action_found)
    {
        (*min_offset_p) = total_lsb;
        (*max_offset_p) = total_msb;
        /** Sanity check.*/
        if (total_lsb < 0 || total_msb < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Found action but range not updated (total_lsb %d total_msb %d ace_id %d).\r\n",
                         total_lsb, total_msb, ace_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_actions_payload_size_get(
    int unit,
    dnx_field_group_t fg_id,
    unsigned int *payload_size_p)
{
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(payload_size_p, _SHR_E_PARAM, "payload_size_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, field_stage, actions_payload_info.actions_on_payload_info, payload_size_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_key_template_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_key_template_t * key_template_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.key_template.get(unit, fg_id, key_template_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_flags_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_add_flags_e * flags_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flags_p, _SHR_E_PARAM, "flags_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.flags.get(unit, fg_id, flags_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_context_attach_flags_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_context_attach_flags_e * flags_p)
{
    uint8 is_attached;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flags_p, _SHR_E_PARAM, "flags_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                    context_info_valid.get(unit, fg_id, context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context id %d not configured for Field Group %d \n", context_id, fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.flags.get(unit, fg_id, context_id, flags_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_ace_format_flags_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_add_flags_e * flags_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flags_p, _SHR_E_PARAM, "flags_p");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.ace_format_info.flags.get(unit, ace_id, flags_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_name_get(
    int unit,
    dnx_field_group_t fg_id,
    char name[DBAL_MAX_STRING_LENGTH])
{
    field_group_name_t group_name_sw;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.name.get(unit, fg_id, &group_name_sw));
    sal_strncpy_s(name, group_name_sw.value,
                  MIN((sizeof(name[0]) * DBAL_MAX_STRING_LENGTH), sizeof(group_name_sw.value)));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_exem_is_lexem(
    int unit,
    dnx_field_group_t fg_id,
    int *is_lexem_p)
{
    dnx_field_action_length_type_e action_length_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_lexem_p, _SHR_E_PARAM, "is_lexem_p");

    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));

    if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM)
    {
        (*is_lexem_p) = TRUE;
    }
    else if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM)
    {
        (*is_lexem_p) = FALSE;
    }
    else
    {
        dnx_field_group_type_e fg_type;
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
        if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "fg_id %d is not an EXEM field group. fg_type is %d.\n", fg_id, fg_type);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d is an EXEM field group, but its action length type is %d.\n",
                         fg_id, action_length_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_app_db_id_get(
    int unit,
    dnx_field_group_t fg_id,
    int *app_db_id_size_p,
    dnx_field_app_db_id_t * app_db_id_p)
{
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_db_id_p, _SHR_E_PARAM, "app_db_id_p");
    SHR_NULL_CHECK(app_db_id_size_p, _SHR_E_PARAM, "app_db_id_size_p");

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            uint32 tcam_handler_id;
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
            SHR_IF_ERR_EXIT(dnx_field_tcam_handler_app_db_id_get(unit, tcam_handler_id, app_db_id_p));
            (*app_db_id_size_p) = dnx_data_field.tcam.app_db_id_size_get(unit);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            dbal_tables_e dbal_table_id;
            uint32 dbal_app_db_id;
            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_app_db_id_get(unit, dbal_table_id, &dbal_app_db_id, app_db_id_size_p));
            (*app_db_id_p) = dbal_app_db_id;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "fg_id %d of type %d, does not have an app_db_id.\n", fg_id, fg_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_vmv_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *vmv_size_p,
    uint8 *vmv_value_p)
{
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e fg_stage;
    dnx_field_group_exem_info_t exem_info;
    dnx_field_app_db_id_t app_db_id;
    int app_db_id_size;
    dnx_field_key_template_t key_template;
    dbal_tables_e dbal_table_id;
    int payload_size;
    mdb_em_entry_encoding_e entry_encoding;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(vmv_value_p, _SHR_E_PARAM, "vmv_value_p");
    SHR_NULL_CHECK(vmv_size_p, _SHR_E_PARAM, "vmv_size_p");

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d of type %d, only EXEM field group are supported by this function.\n",
                     fg_id, fg_type);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &fg_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_exem_info(unit, fg_stage, &exem_info));
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &app_db_id));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    /** Note: We assume there is only one result type per field group. */
    SHR_IF_ERR_EXIT(dbal_tables_payload_size_get(unit, dbal_table_id, 0, &payload_size));

    SHR_IF_ERR_EXIT(mdb_em_get_entry_encoding
                    (unit, exem_info.dbal_physical_table_id, key_template.key_size_in_bits, payload_size, app_db_id,
                     app_db_id_size, &entry_encoding));
    SHR_IF_ERR_EXIT(mdb_em_get_vmv_size_value
                    (unit, exem_info.dbal_physical_table_id, entry_encoding, vmv_size_p, vmv_value_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_kbp_support_packing(
    int unit,
    dnx_field_group_t fg_id,
    int *is_support_packing_p)
{
    dnx_field_group_type_e fg_type;
    dnx_field_group_add_flags_e group_add_flags;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_support_packing_p, _SHR_E_PARAM, "is_support_packing_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type != DNX_FIELD_GROUP_TYPE_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Function only support external TCAM field groups. fg_id %d, type %d.\n",
                     fg_id, fg_type);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_flags_get(unit, fg_id, &group_add_flags));
    if ((group_add_flags & DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS) != 0)
    {
        (*is_support_packing_p) = TRUE;
    }
    else
    {
        (*is_support_packing_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_name_to_id(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    dnx_field_group_t * fg_id_p)
{
    int fg_id_ndx;
    int nof_fgs = dnx_data_field.group.nof_fgs_get(unit);
    char iter_fg_id_name[DBAL_MAX_STRING_LENGTH];
    int found = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");

    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }

    for (fg_id_ndx = 0; fg_id_ndx < nof_fgs; fg_id_ndx++)
    {
        uint8 is_alloc;

        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_alloc));
        if (!is_alloc)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_name_get(unit, fg_id_ndx, iter_fg_id_name));
        if (0 == sal_strncmp(name, iter_fg_id_name, DBAL_MAX_STRING_LENGTH))
        {
            found = TRUE;
            break;
        }
    }

    if (found)
    {
        (*fg_id_p) = fg_id_ndx;
    }
    else
    {
        (*fg_id_p) = DNX_FIELD_GROUP_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_ace_format_name_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    char name[DBAL_MAX_STRING_LENGTH])
{
    field_ace_format_name_t ace_format_name_sw;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");
    DNX_FIELD_ACE_ID_VERIFY(unit, ace_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.ace_format_info.name.get(unit, ace_id, &ace_format_name_sw));
    sal_strncpy_s(name, ace_format_name_sw.value,
                  MIN((sizeof(name[0]) * DBAL_MAX_STRING_LENGTH), sizeof(ace_format_name_sw.value)));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_ace_format_name_to_id(
    int unit,
    char name[DBAL_MAX_STRING_LENGTH],
    dnx_field_ace_id_t * ace_id_p)
{
    int ace_id_ndx;
    int first_ace_id = dnx_data_field.ace.ace_id_pmf_alloc_first_get(unit);
    int last_ace_id = dnx_data_field.ace.ace_id_pmf_alloc_last_get(unit);
    char iter_ace_id_name[DBAL_MAX_STRING_LENGTH];
    int found = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name, _SHR_E_PARAM, "name");

    if (name[0] == '\0')
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is an empty string.\n");
    }

    for (ace_id_ndx = first_ace_id; ace_id_ndx <= last_ace_id; ace_id_ndx++)
    {
        uint8 is_alloc;

        SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, ace_id_ndx, &is_alloc));
        if (!is_alloc)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_ace_format_name_get(unit, ace_id_ndx, iter_ace_id_name));
        if (0 == sal_strncmp(name, iter_ace_id_name, DBAL_MAX_STRING_LENGTH))
        {
            found = TRUE;
            break;
        }
    }

    if (found)
    {
        (*ace_id_p) = ace_id_ndx;
    }
    else
    {
        (*ace_id_p) = DNX_FIELD_ACE_FORMAT_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_context_fgs_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t * fg_ids,
    int *fg_count)
{
    dnx_field_attach_context_info_t context_info;
    int fg_id_ndx;
    dnx_field_stage_e fg_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_ids, _SHR_E_PARAM, "fg_ids");
    SHR_NULL_CHECK(fg_count, _SHR_E_PARAM, "fg_count");

    *fg_count = 0;
    for (fg_id_ndx = 0; fg_id_ndx < dnx_data_field.group.nof_fgs_get(unit); fg_id_ndx++)
    {
        uint8 is_alloc;

        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_alloc));
        if (!is_alloc)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_ndx, &fg_stage));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.get(unit, fg_id_ndx, context_id, &context_info));
        if (context_info.context_info_valid && field_stage == fg_stage)
        {
            fg_ids[(*fg_count)++] = fg_id_ndx;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Converts given field group IDs array into their corresponding tcam handler IDs array
 * \param [in] unit   - Device ID
 * \param [in] fg_ids - Array of the field group IDs
 * \param [in] count  - Count of elements in the fg_ids array
 * \param [out] tcam_handler_ids - The corresponding tcam handler IDs of the given field group IDs
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
static shr_error_e
dnx_field_group_fg_ids_to_tcam_handler_ids(
    int unit,
    dnx_field_group_t * fg_ids,
    int count,
    uint32 *tcam_handler_ids)
{
    int ii;
    uint32 tcam_handler_id;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < count; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_ids[ii], &tcam_handler_id));
        tcam_handler_ids[ii] = tcam_handler_id;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Add context ID info to SW state of Field group
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id that context ID being attached to
* \param [in] context_id      - Context ID to add its infot to SW of Field Group
* \param [in] context_info_p  - Pointer type of dnx_field_attach_context_info_t that hold the context ID info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_context_info_set(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_attach_context_info_t * context_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.set(unit, fg_id, context_id, context_info_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See header on field_group.h
*/
shr_error_e
dnx_field_group_context_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_attach_context_info_t * context_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.get(unit, fg_id, context_id, context_info_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_context_key_id_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_attach_context_info_t context_info;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    if (context_info.context_info_valid != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context id %d not configured for Field Group %d \n", context_id, fg_id);
    }

    sal_memcpy(key_id_p, &(context_info.key_id), sizeof(*key_id_p));

    if ((key_id_p->id[0] == DNX_FIELD_KEY_ID_INVALID) &&
        (fg_type != DNX_FIELD_GROUP_TYPE_CONST) && (fg_type != DNX_FIELD_GROUP_TYPE_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No key defined for Context ID %d FG  %d", context_id, fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_context_de_position_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int *group_lsb_p)
{
    dnx_field_attach_context_info_t context_info;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(group_lsb_p, _SHR_E_PARAM, "group_lsb_p");
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Function only supports direct extraction field groups. "
                     "Field group ID is %d, type is %d.\n", fg_id, fg_type);
    }

    if (context_info.context_info_valid != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context id %d not configured for Field Group %d \n", context_id, fg_id);
    }

    if (context_info.key_id.bit_range.bit_range_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d is doesn't have a valid bit range.\n", fg_id);
    }

    *group_lsb_p = context_info.key_id.bit_range.bit_range_offset;

    

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_context_qualifier_attach_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    dnx_field_attach_context_info_t context_info;
    uint32 qual_idx;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));

    if (context_info.context_info_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context id %d not attached for Field Group %d \n", context_id, fg_id);
    }

    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        qual_attach_info.get(unit, fg_id, context_id, qual_idx, &qual_info[qual_idx]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get FES configuration info and payload attach info from HW and algo_field_action SW state.
* \param [in] unit              - Device ID
* \param [in] fg_id             - Field group id that context ID being attached to
* \param [in] context_id        - Context ID
* \param [out] fes_group_info_p - To be loaded with the FES configuration in use by the field group and context.
* \param [out] attach_info      - Array to be loaded with the payload attach info.
* \return
*   shr_error_e - Error Type
* \remark
*   * We do not check that the context is attached to the field group.
* \see
*   * None
*/
static shr_error_e
dnx_field_group_context_fes_info_payload_attach_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_actions_fes_context_group_get_info_t * fes_group_info_p,
    dnx_field_action_attach_info_t attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    dnx_field_stage_e field_stage;
    dnx_field_actions_fes_context_get_info_t actions_context_fes_info;
    unsigned int action_ndx;
    unsigned int action_template_ndx;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    int is_void;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FG_ID_VERIFY(unit, fg_id);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    /** Initialize attach_info. */
    SHR_IF_ERR_EXIT(dnx_field_action_attach_info_t_init(unit, attach_info));

    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_get(unit, field_stage, context_id, &actions_context_fes_info));

    SHR_IF_ERR_EXIT(dnx_field_actions_context_fes_info_to_group_fes_info
                    (unit, fg_id, &actions_context_fes_info, fes_group_info_p));

    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));

    for (action_ndx = 0, action_template_ndx = 0;
         action_template_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         actions_payload_info.actions_on_payload_info[action_template_ndx].dnx_action != DNX_FIELD_ACTION_INVALID;
         action_template_ndx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                        (unit, field_stage,
                         actions_payload_info.actions_on_payload_info[action_template_ndx].dnx_action, &is_void));
        if (is_void == FALSE)
        {
            if (fes_group_info_p->initial_fes_quartets[action_ndx].priority != DNX_FIELD_ACTION_PRIORITY_INVALID)
            {
                attach_info[action_template_ndx].priority = fes_group_info_p->initial_fes_quartets[action_ndx].priority;
                attach_info[action_template_ndx].valid_bit_polarity =
                    fes_group_info_p->initial_fes_quartets[action_ndx].fes2msb_info[0].polarity;
            }
            else
            {
                attach_info[action_template_ndx].priority = BCM_FIELD_ACTION_INVALIDATE;
            }
            action_ndx++;
        }
    }

    /*
     * Sanity check: Verify that the are no more initiali FES quartets.
     */
    for (; action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_ndx++)
    {
        if (fes_group_info_p->initial_fes_quartets[action_ndx].fes_id != DNX_FIELD_EFES_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found more non void actions than number of allocates EFESes "
                         "in field group %d.\r\n", fg_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function will check the validity of parameters for dnx_field_group_add()
* \param [in] unit       - Device ID
*   \param [in] fg_id_p  - Field group id that will be created
*   \param [in] flags    - Currently not used. Future extension.
*   \param [in] fg_info_in_p  - pointer to field group info that will be created
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_add_verify(
    int unit,
    dnx_field_group_t * fg_id_p,
    dnx_field_group_add_flags_e flags,
    dnx_field_group_info_t * fg_info_in_p)
{
    unsigned int qual_ndx;
    unsigned int qual_ndx_2;
    unsigned int action_ndx;
    unsigned int action_ndx_2;
    dnx_field_stage_e stage_on_qual;
    dnx_field_stage_e stage_on_action;
    dnx_field_qual_class_e class_on_qual;
    dnx_field_action_class_e class_on_action;
    int action_is_void;
    unsigned int action_size;
    dnx_field_action_t base_dnx_action;
    int nof_ranged_qual;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fg_info_in_p, _SHR_E_PARAM, "fg_info_in_p");
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");

    if (fg_info_in_p->field_stage != DNX_FIELD_STAGE_IPMF1 &&
        fg_info_in_p->field_stage != DNX_FIELD_STAGE_IPMF2 &&
        fg_info_in_p->field_stage != DNX_FIELD_STAGE_IPMF3 &&
        fg_info_in_p->field_stage != DNX_FIELD_STAGE_EPMF && fg_info_in_p->field_stage != DNX_FIELD_STAGE_EXTERNAL)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, " Field stage %d not supported\n", fg_info_in_p->field_stage);
    }

    if (flags & DNX_FIELD_GROUP_ADD_FLAG_WITH_ID)
    {
        DNX_FIELD_GROUP_ID_VERIFY(*fg_id_p);
    }

    if (((flags & DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS) != 0) &&
        (fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "QUAL_PACKING flag only supported for external TCAM (KBP) field groups. "
                     "FG type %d.\n", fg_info_in_p->fg_type);
    }

    if (fg_info_in_p->field_stage == DNX_FIELD_STAGE_EXTERNAL && fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s supports only field group type KBP.\n",
                     dnx_field_stage_text(unit, DNX_FIELD_STAGE_EXTERNAL));
    }

    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        if (fg_info_in_p->field_stage != DNX_FIELD_STAGE_EXTERNAL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field group type KBP supports only stage %s .\n",
                         dnx_field_stage_text(unit, DNX_FIELD_STAGE_EXTERNAL));
        }
        if (flags & DNX_FIELD_GROUP_ADD_FLAG_WITH_ID)
        {
            DNX_FIELD_GROUP_ID_KBP_VERIFY(*fg_id_p);
        }
    }

    if (fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_CONST &&
        fg_info_in_p->dnx_quals[0] == DNX_FIELD_QUAL_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Key has no qualifiers, yet field group type is %d. "
                     "Only field group type cost (fg_type %d) supports an empty key.\n",
                     fg_info_in_p->fg_type, DNX_FIELD_GROUP_TYPE_CONST);
    }

    /*
     * Verify that no qualifier appears twice.
     */
    for (qual_ndx = 0;
         qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
         && fg_info_in_p->dnx_quals[qual_ndx] != DNX_FIELD_QUAL_TYPE_INVALID; qual_ndx++)
    {
        for (qual_ndx_2 = 0; qual_ndx_2 < qual_ndx; qual_ndx_2++)
        {
            if (fg_info_in_p->dnx_quals[qual_ndx] == fg_info_in_p->dnx_quals[qual_ndx_2])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Qual 0x%x appears twice (in places %d and %d).\n",
                             fg_info_in_p->dnx_quals[qual_ndx], qual_ndx_2, qual_ndx);
            }
        }
    }

    /*
     * Verify that no actions appears twice.
     */
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && fg_info_in_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        for (action_ndx_2 = 0; action_ndx_2 < action_ndx; action_ndx_2++)
        {
            if (fg_info_in_p->dnx_actions[action_ndx] == fg_info_in_p->dnx_actions[action_ndx_2])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Action 0x%x appears twice (in places %d and %d).\n",
                             fg_info_in_p->dnx_actions[action_ndx], action_ndx_2, action_ndx);
            }
        }
    }

    /*
     * Verify the qualifiers correspond to the field stage.
     */
    for (qual_ndx = 0;
         qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
         && fg_info_in_p->dnx_quals[qual_ndx] != DNX_FIELD_QUAL_TYPE_INVALID; qual_ndx++)
    {
        stage_on_qual = DNX_QUAL_STAGE(fg_info_in_p->dnx_quals[qual_ndx]);
        if (fg_info_in_p->field_stage != stage_on_qual)
        {
            class_on_qual = DNX_QUAL_CLASS(fg_info_in_p->dnx_quals[qual_ndx]);
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage on qualifier (at index %d on dnx_quals[]) is %s while stage on \r\n"
                         "==> field group is %s. They should be the same.\r\n"
                         "==> qual class is %s. qual id is %s. Quit.\r\n",
                         qual_ndx, dnx_field_stage_e_get_name(stage_on_qual),
                         dnx_field_stage_e_get_name(fg_info_in_p->field_stage),
                         dnx_field_qual_class_text(class_on_qual), dnx_field_dnx_qual_text(unit,
                                                                                           fg_info_in_p->dnx_quals
                                                                                           [qual_ndx]));
        }
    }

    /*
     * Verify the actions correspond to the field stage.
     */
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && fg_info_in_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        stage_on_action = DNX_ACTION_STAGE(fg_info_in_p->dnx_actions[action_ndx]);
        class_on_action = DNX_ACTION_CLASS(fg_info_in_p->dnx_actions[action_ndx]);
        if (fg_info_in_p->field_stage != stage_on_action)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage on action (at index %d on dnx_actions[]) is %s while stage on \r\n"
                         "==> field group is %s. They should be the same.\r\n"
                         "==> action class is %s. action id is %s. Quit.\r\n",
                         action_ndx, dnx_field_stage_e_get_name(stage_on_action),
                         dnx_field_stage_e_get_name(fg_info_in_p->field_stage),
                         dnx_field_action_class_text(class_on_action),
                         dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]));
        }
    }
    /*
     * Verify that number of and actions does not exceed the maximum for Field group (because the input array is
     * the size of DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP).
     */
    if (action_ndx > DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "There are %d actions, maximum for Field group is %d\n",
                     action_ndx, DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG);
    }

    /*
     * Verify the actions are legal for the stage.
     */
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && fg_info_in_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        int action_is_legal;
        SHR_IF_ERR_EXIT(dnx_field_map_action_is_legal
                        (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx], &action_is_legal));
        if (action_is_legal == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Action number %d (\"%s\") cannot be used on the device.\r\n",
                         action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]));
        }
    }

    /*
     * Verify that all void actions don't have valid bit.
     */
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && fg_info_in_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        if (fg_info_in_p->use_valid_bit[action_ndx])
        {
            if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action number %d (%s) is assigned valid bit, "
                             "but valid bits are not supported with KBP group type.\r\n",
                             action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]));
            }
            if (fg_info_in_p->field_stage == DNX_FIELD_STAGE_EXTERNAL)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action number %d (%s) is assigned valid bit, "
                             "but valid bits are not supported in stage %s.\r\n",
                             action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]),
                             dnx_field_stage_text(unit, DNX_FIELD_STAGE_EXTERNAL));
            }
            if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_CONST)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action number %d (%s) is assigned valid bit, "
                             "but valid bits are not supported with const group type.\r\n",
                             action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]));
            }
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                            (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx], &action_is_void));
            if (action_is_void)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action number %d (%s) is a void action, "
                             "but is assigned valid bit.\r\n",
                             action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]));
            }
        }
        else
        {
            /*
             * Verify that we use valid bit if we ban removing it from lookup based field group types
             */
#if (DNX_FIELD_ALLOW_VALID_BIT_CHOICE_FOR_LOOKUP_FG==0)
            if (fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION &&
                fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_CONST &&
                fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_STATE_TABLE &&
                fg_info_in_p->fg_type != DNX_FIELD_GROUP_TYPE_KBP)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                                (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx],
                                 &action_is_void));
                if (action_is_void == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Action number %d (%s) is not a void action. "
                                 "Field group type %d does not support not using valid bit for non void actions.\r\n",
                                 action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]),
                                 fg_info_in_p->fg_type);
                }
            }
#endif
        }
    }

    /*
     * Verify for field group type const that all actions are of size zero.
     * Note we have already tested that we do not use valid bit above.
     * Also verify that the action is not void.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_CONST)
    {
        for (action_ndx = 0;
             action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
             && fg_info_in_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                            (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx], &action_size));
            if (action_size != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action number %d (%s) has size different from zero (%d). "
                             "Field group type const does not support actions with sizes other than zero.\r\n",
                             action_ndx,
                             dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]), action_size);
            }
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                            (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx], &action_is_void));
            if (action_is_void)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Action number %d (%s) is a void action. "
                             "Field group type const does not support void actions.\r\n",
                             action_ndx, dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]));
            }
        }
    }
    else
    {
        /*
         * Verify for field group other than type const that all actions are not zero sized.
         * Note that zero sized actions are only a problem if they have a valid bit and an even prefix
         * (in which case their value is dependant on the action that comes after them), but we block them anyway.
         * We do allow actions where the predefined action size is zero (and thus have no use for input,
         * such as invalidate next).
         */
        for (action_ndx = 0;
             action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
             && fg_info_in_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
        {
            if (DNX_ACTION_CLASS(fg_info_in_p->dnx_actions[action_ndx]) == DNX_FIELD_ACTION_CLASS_USER)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                                (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx], &action_size));
                if (action_size == 0)
                {
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action
                                    (unit, fg_info_in_p->field_stage, fg_info_in_p->dnx_actions[action_ndx],
                                     &base_dnx_action));
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                                    (unit, fg_info_in_p->field_stage, base_dnx_action, &action_size));
                    if (action_size != 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Action number %d (%s) has size zero, but it is based "
                                     "on an action (%s) with a size different from zero (%d). "
                                     "Field group types other than const do not support user defined actions with "
                                     "a base action that isn't zero (fg_type %d).\r\n",
                                     action_ndx,
                                     dnx_field_dnx_action_text(unit, fg_info_in_p->dnx_actions[action_ndx]),
                                     dnx_field_dnx_action_text(unit, base_dnx_action), action_size,
                                     fg_info_in_p->fg_type);
                    }
                }
            }
        }
    }

    /*
     * Verify that field group type const doesn't have a key.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_CONST &&
        fg_info_in_p->dnx_quals[0] != DNX_FIELD_QUAL_TYPE_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "field group type const cannot have any qualifiers in the key.\n");
    }

    /*
     * Verify that the stage supports direct extraction if the field group type is direct extraction.
     * Note that even without this check, the scenario will fail while trying to acquire field_io.
     */
    if ((fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION) &&
        ((fg_info_in_p->field_stage != DNX_FIELD_STAGE_IPMF2) && (fg_info_in_p->field_stage != DNX_FIELD_STAGE_IPMF3)
         && (fg_info_in_p->field_stage != DNX_FIELD_STAGE_EPMF)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported stage %s for Direct Extraction field group\n",
                     dnx_field_stage_text(unit, fg_info_in_p->field_stage));
    }

    /** Check if Direct Extraction is availble on EPMF stage for current device. */
    if ((fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION) &&
        (fg_info_in_p->field_stage == DNX_FIELD_STAGE_EPMF))
    {
        if (dnx_data_field.features.dir_ext_epmf_get(unit) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Direct Extraction field group in EPMF stage is not supported for current device!\n");
        }

        if (dnx_data_field.stage.stage_info_get(unit, fg_info_in_p->field_stage)->dir_ext_single_key_size <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Direct Extraction EPMF feature is enabled, but the dir_ext_single_key_size value (%d) is invalid!\n",
                         dnx_data_field.stage.stage_info_get(unit, fg_info_in_p->field_stage)->dir_ext_single_key_size);
        }
    }

    /*
     * Verify that an EXEM field group in stages iPMF2 or iPMF3 is supported by MUX.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_EXEM &&
        (fg_info_in_p->field_stage == DNX_FIELD_STAGE_IPMF2 || fg_info_in_p->field_stage == DNX_FIELD_STAGE_IPMF3) &&
        fg_info_in_p->field_stage != dnx_data_field.profile_bits.pmf_sexem3_stage_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Exem type field group in stage \"%s\" (where small EXEM is used), but "
                     "SOC property \"pmf_sexem3_stage\" selected stage \"%s\" for SEXEM.\n",
                     dnx_field_stage_text(unit, fg_info_in_p->field_stage),
                     dnx_field_stage_text(unit, dnx_data_field.profile_bits.pmf_sexem3_stage_get(unit)));
    }

    /*
     * Verify that a MDB DT field group in stages iPMF1/iPMF2 or iPMF3 is supported by MUX.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB &&
        (((fg_info_in_p->field_stage == DNX_FIELD_STAGE_IPMF1 ||
           fg_info_in_p->field_stage == DNX_FIELD_STAGE_IPMF2) &&
          dnx_data_field.profile_bits.pmf_map_stage_get(unit) != DNX_FIELD_STAGE_IPMF1) ||
         (fg_info_in_p->field_stage == DNX_FIELD_STAGE_IPMF3 &&
          dnx_data_field.profile_bits.pmf_map_stage_get(unit) != DNX_FIELD_STAGE_IPMF3)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MDB DT type field group in stage \"%s\", but "
                     "SOC property \"pmf_map_stage\" selected stage \"%s\" for MDB DT.\n",
                     dnx_field_stage_text(unit, fg_info_in_p->field_stage),
                     dnx_field_stage_text(unit, dnx_data_field.profile_bits.pmf_map_stage_get(unit)));
    }

    /*
     * Verify that if a KBP field group is configured, we have a KBP device in SOC property.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP && (dnx_kbp_device_enabled(unit) == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "External (KBP) field groups require a KBP device to be enabled.\n");
    }

    /*
     * Verify that a KBP field group is configured before device lock.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "External TCAM field groups cannot be configured after device lock.\n");
        }
    }

    /*
     * Only a KBP field group can accept a ranged qualifier.
     * For KBP field groups, we still need at least one qualifier that is not a ranged qualifier.
     * We are also limited to four ranged qualifiers per field group, but an error message for that is sent by DBAL.
     */
    nof_ranged_qual = 0;
    for (qual_ndx = 0;
         qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
         && fg_info_in_p->dnx_quals[qual_ndx] != DNX_FIELD_QUAL_TYPE_INVALID; qual_ndx++)
    {

        if (fg_info_in_p->qual_is_ranged[qual_ndx])
        {
            if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP)
            {
                nof_ranged_qual++;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Only external field groups can use ranged qualifiers. Field group type %d. \n"
                             "Qualifier number %d (%s) is ranged.\r\n",
                             fg_info_in_p->fg_type, qual_ndx,
                             dnx_field_dnx_qual_text(unit, fg_info_in_p->dnx_quals[qual_ndx]));
            }
        }
    }
    if ((fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP) && (nof_ranged_qual > 0) && (nof_ranged_qual == qual_ndx))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "External field groups must have at least one qualifier that is not a ranged qualifier. "
                     "Number of qualifiers is %d.\r\n", qual_ndx);
    }

    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_STATE_TABLE)
    {
        /*
         * Note we allow actions for state table  according to state_table_atomic_rmw, even though we can enable
         * them for J2C and Q2A according to state_table_acr_bus.
         */
        if (dnx_data_field.features.state_table_atomic_rmw_get(unit) == FALSE)
        {
            if (fg_info_in_p->dnx_actions[0] != DNX_FIELD_ACTION_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "State table field group does not support actions for this device.\r\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_action_length_to_bit_size(
    int unit,
    dnx_field_action_length_type_e action_length_type,
    unsigned int *block_size)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(block_size, _SHR_E_PARAM, "block_size");

    switch (action_length_type)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
        {
            (*block_size) = dnx_data_field.tcam.action_size_half_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
        {
            (*block_size) = dnx_data_field.tcam.action_size_single_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
        {
            (*block_size) = dnx_data_field.tcam.action_size_double_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM:
        {
            uint8 vmv_size;
            SHR_IF_ERR_EXIT(dnx_field_group_physical_db_vmv_size_get(unit, DBAL_PHYSICAL_TABLE_LEXEM, &vmv_size));
            (*block_size) = dnx_data_field.exem.large_max_result_size_get(unit) - vmv_size;
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM:
        {
            uint8 vmv_size;
            SHR_IF_ERR_EXIT(dnx_field_group_physical_db_vmv_size_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_3, &vmv_size));
            (*block_size) = dnx_data_field.exem.small_max_result_size_get(unit) - vmv_size;
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_ACE:
        {
            (*block_size) = dnx_data_field.ace.payload_size_get(unit) - dnx_data_field.ace.ace_id_size_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_MDB_DT:
        {
            /*
             * MDB DT result size is always 120b (large) msb-aligned regardless of the chosen granularity (payload
             * size)
             */
            (*block_size) = dnx_data_field.map.action_size_large_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_STATE_TABLE:
        {
            (*block_size) = dnx_data_field.state_table.data_size_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_NONE:
        {
            (*block_size) = 0;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid action_length_type=%d \n", action_length_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Fills the pointer dnx_field_info_t thats used as in parameter
 *  for function dnx_field_actions_place_actions().
 *  Specifically, initializes it, then fills the DNX actions and whether or not they use valid bit.
 *  dnx_field_actions_place_actions() then fills the lsb of each action, and the whole is given to
 *  dnx_field_actions_calc_feses() as input.
 * \param [in] unit            -
 *   Device ID
 * \param [in] field_stage     -
 *   Field Stage (iPMF1/2..)
 * \param [in] dnx_actions     -
 *   Action type array to extract the action sizes from.
 *   This is a pointer to a one dimensional array.
 *   The list of actions ends when an action of value 'DNX_FIELD_ACTION_INVALID' is detected, or
 *   DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP elements reached.
 * \param [in] use_valid_bit     -
 *   For each action in dnx_actions, an indication whether or not we use valid bit.
 * \param [out]  actions_info_arr    -
 *   Array of type dnx_field_action_in_group_info_t that is later used by placing actions and
 *   creating the actions payload template.
 *   This function uses a one dimensional array.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_actions_place_actions().
 */
static shr_error_e
dnx_field_group_fill_action_info_arr_dnx_action_type(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    uint8 use_valid_bit[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    int action_indx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_actions, _SHR_E_PARAM, "dnx_actions");
    SHR_NULL_CHECK(actions_info_arr, _SHR_E_PARAM, "actions_info_arr");

    for (action_indx = 0; action_indx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_indx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_info_t_init(unit, &(actions_info_arr[action_indx])));
        actions_info_arr[action_indx].dnx_action = dnx_actions[action_indx];
        if (dnx_actions[action_indx] == DNX_FIELD_ACTION_INVALID)
        {
            break;
        }
        actions_info_arr[action_indx].dont_use_valid_bit = !(use_valid_bit[action_indx]);
    }
exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_calc_actions_bit_size(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *actions_size_in_bits_p)
{
    unsigned int action_ndx;
    unsigned int action_size_w_valid;
    unsigned int action_size_wo_valid;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_info_arr, _SHR_E_PARAM, "actions_info_arr");
    SHR_NULL_CHECK(actions_size_in_bits_p, _SHR_E_PARAM, "actions_size_in_bits_p");

    (*actions_size_in_bits_p) = 0;
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_info_arr[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        /*
         * Calculate the action bit size.
         * Note 'size_wo_valid' is only used for the function interface, and has no use in this function.
         */
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                        (unit, field_stage, &(actions_info_arr[action_ndx]), &action_size_w_valid,
                         &action_size_wo_valid));
        (*actions_size_in_bits_p) += action_size_w_valid;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Given action types on Field group creates action template
* This order of action template will be used to put the action in various databases (TCAM..)
* \param [in] unit                     - Device Id
* \param [in] field_stage              - Field Stage
* \param [in] actions_info_arr         - Actions information to build action payload template from
* \param [in] actions_length_type      - Indicate the block size to build action payload on
* \param [in] actions_size_in_bits     - The number of bits from the block size actually in use by the actions.
* \param [in] dbal_table_id            - The DBAL ID of the dynamic table of the field group.
* \param [out] actions_payload_info_p  -Info regarding the build action payload template look
*                                       Note that the structure is field group specific
*                                       (dnx_field_actions_fg_payload_info_t)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_actions_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_field_action_length_type_e actions_length_type,
    unsigned int actions_size_in_bits,
    dbal_tables_e dbal_table_id,
    dnx_field_actions_fg_payload_info_t * actions_payload_info_p)
{
    unsigned int block_size;
    unsigned int actions_ndx;
    dbal_fields_e field_id;
    int action_offset;
    unsigned int size_w_valid_bit;
    unsigned int size_wo_valid_bit;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_info_arr, _SHR_E_PARAM, "actions_info_arr");
    SHR_NULL_CHECK(actions_payload_info_p, _SHR_E_PARAM, "actions_payload_info_p");

    if (actions_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_DIR_EXT ||
        actions_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_STATE_TABLE)
    {
        action_offset = 0;
    }
    else if (actions_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_NONE &&
             actions_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_KBP)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_action_length_to_bit_size(unit, actions_length_type, &block_size));
    }
    /*
     * Acquire the location of the actions from DBAL.
     * Copy the output to actions_payload_info_p'.
     * Note it can technically be done by copying the pointer as the two arrays have the same scope in the
     * calling function.
     */
    for (actions_ndx = 0; actions_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         actions_info_arr[actions_ndx].dnx_action != DNX_FIELD_ACTION_INVALID; actions_ndx++)
    {
        if (actions_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_DIR_EXT ||
            actions_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_STATE_TABLE)
        {
            /** For direct extraction or state table just add the actions one after the other.*/
            actions_info_arr[actions_ndx].lsb = action_offset;
            SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                            (unit, field_stage, &actions_info_arr[actions_ndx], &size_w_valid_bit, &size_wo_valid_bit));
            action_offset += size_w_valid_bit;

        }
        else if (actions_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_NONE)
        {
            /** Const field groups have no input source and so no meaning for offset.*/
            actions_info_arr[actions_ndx].lsb = 0;
        }
        else
        {
            /** Take the location of the action from DBAL.*/
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                            (unit, field_stage, actions_info_arr[actions_ndx].dnx_action, &field_id));
            SHR_IF_ERR_EXIT(dbal_tables_field_offset_in_hw_get
                            (unit, dbal_table_id, field_id, FALSE, 0, INST_SINGLE, &action_offset));
            /** Align the DBAL relative location to the MSB (as is done in access layer when writing to HW).*/
            if (actions_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_KBP)
            {
                actions_info_arr[actions_ndx].lsb = block_size - actions_size_in_bits + action_offset;
            }
            else
            {
                /** KBP is not aligned to MSB, but taken as is from DBAL*/
                actions_info_arr[actions_ndx].lsb = action_offset;
            }
        }

        /**Write the result to the payload info*/
        actions_payload_info_p->actions_on_payload_info[actions_ndx] = actions_info_arr[actions_ndx];
    }
    /*
     * Even though this should have been done by the initialization, we will still fill the next action with
     * invalid, just to be on the safe side.
     */
    if (actions_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        actions_payload_info_p->actions_on_payload_info[actions_ndx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Given action types on Field group creates action template
* This order of action template will be used to put the action in the ACE table.
* \param [in] unit                     - Device Id
* \param [in] field_stage              - Field Stage
* \param [in] ace_id                   - ACE format ID
* \param [in] actions_info_arr         - Actions information to build action payload template from
* \param [in] actions_length_type      - Indicate the block size to build action payload on
* \param [in] actions_size_in_bits     - The number of bits from the block size actually in use by the actions.
* \param [out] actions_payload_info_p  -Info regarding the build action payload template look
*                                       Note that the structure is ace specific
*                                       (dnx_field_actions_ace_payload_info_t)
* \return
*   shr_error_e - Error Type
* \remark
*   * This function uses any logic, but takes the location of the actions from DBAL.
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_actions_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_ace_id_t ace_id,
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    dnx_field_action_length_type_e actions_length_type,
    unsigned int actions_size_in_bits,
    dnx_field_actions_ace_payload_info_t * actions_payload_info_p)
{
    unsigned int block_size;
    unsigned int actions_ndx;
    dbal_fields_e field_id;
    int action_offset;
    int result_type_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_info_arr, _SHR_E_PARAM, "actions_info_arr");
    SHR_NULL_CHECK(actions_payload_info_p, _SHR_E_PARAM, "actions_payload_info_p");
    SHR_IF_ERR_EXIT(dnx_field_group_action_length_to_bit_size(unit, actions_length_type, &block_size));

    /*
     * Obtain the result type.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_result_type_index_get(unit, ace_id, &result_type_index));

    /*
     * Acquire the location of the actions from DBAL.
     * Copy the output to actions_payload_info_p'.
     * Note it can technically be done by copying the pointer as the two arrays have the same scope in the
     * calling function.
     */
    for (actions_ndx = 0; actions_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         actions_info_arr[actions_ndx].dnx_action != DNX_FIELD_ACTION_INVALID; actions_ndx++)
    {
        /** Take the location of the action from DBAL.*/
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, DNX_FIELD_STAGE_ACE, actions_info_arr[actions_ndx].dnx_action, &field_id));
        SHR_IF_ERR_EXIT(dbal_tables_field_offset_in_hw_get
                        (unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, field_id, FALSE, result_type_index, INST_SINGLE,
                         &action_offset));
        /** Align the DBAL relative location to the MSB (as is done in access layer when writing to HW).*/
        actions_info_arr[actions_ndx].lsb = block_size - actions_size_in_bits + action_offset;
        /**Write the result to the payload info*/
        actions_payload_info_p->actions_on_payload_info[actions_ndx] = actions_info_arr[actions_ndx];
    }
    /*
     * Even though this should have been done by the initialization, we will still fill the next action with
     * invalid, just to be on the safe side.
     */
    if (actions_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        actions_payload_info_p->actions_on_payload_info[actions_ndx].dnx_action = DNX_FIELD_ACTION_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the correct field io for the lookup payload,
*  used to find the correct key select later on.
*  This function is used for the field stage iPMF1.
* \param [in] unit                    - Device ID
* \param [in] key_id_p                - Structure that holds an array of key IDs, the keys being used for the TCAM lookup.
* \param [in] payload_is_double_key   - A boolean to indicate if the payload encompass the result of more
*                                       than one TCAM access.
* \param [out] field_io_p             - The field io being used.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_field_io_ipmf1(
    int unit,
    dnx_field_key_id_t *key_id_p,
    int payload_is_double_key,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");

    *field_io_p = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    /*
     * For each key, check if the payload encompasses two TCAM accesses, and if so if the keys are consecutive.
     */
    switch (key_id_p->id[0])
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_A:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_B)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_B:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_C)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_C:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_D)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_D:
            if (payload_is_double_key)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d does not support double key as first key.\n", key_id_p->id[0]);
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_E:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d Does not support TCAM lookup in iPMF1.\n", key_id_p->id[0]);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid key_id=%d for iPMF1\n", key_id_p->id[0]);
            break;
    }

    /*
     * If we have reached this point and still haven't allocated a field_io, than we had a double key without
     * a legal pair of keys.
     */
    if (*field_io_p == DBAL_NOF_ENUM_FIELD_IO_VALUES)
    {
        if (payload_is_double_key)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Non matching keys for double key TCAM lookup: %d and %d.\n",
                         key_id_p->id[0], key_id_p->id[1]);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "No field_io was found for an unknown reason. This error should not be reached.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the correct field io for the lookup payload,
*  used to find the correct key select later on.
*  This function is used for the field stage iPMF2.
* \param [in] unit            - Device ID
* \param [in] key_id_p        - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
* \param [in] payload_is_double_key   - A boolean to indicate if the payload encompass the result of more
*                                       than one TCAM access.
* \param [out] field_io_p     - The field io being used.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_field_io_ipmf2(
    int unit,
    dnx_field_key_id_t *key_id_p,
    int payload_is_double_key,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");
    *field_io_p = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    /*
     * For each key, check if the payload encompasses two TCAM accesses, and if so if the keys are consecutive.
     */
    switch (key_id_p->id[0])
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_G)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4_5;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_4;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_H)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5_6;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_5;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_I)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6_7;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_6;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
            if (payload_is_double_key)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d does not support double key as first key.\n", key_id_p->id[0]);
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_7;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d Does not support TCAM lookup in iPMF2.\n", key_id_p->id[0]);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid key_id=%d for iPMF2.\n", key_id_p->id[0]);
            break;
    }

    /*
     * If we have reached this point and still haven't allocated a field_io, than we had a double key without
     * a legal pair of keys.
     */
    if (*field_io_p == DBAL_NOF_ENUM_FIELD_IO_VALUES)
    {
        if (payload_is_double_key)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Non matching keys for double key TCAM lookup: %d and %d.\n",
                         key_id_p->id[0], key_id_p->id[1]);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "No field_io was found for an unknown reason. This error should not be reached.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the correct field io for the lookup payload,
*  used to find the correct key select later on.
*  This function is used for the field stage iPMF3.
* \param [in] unit            - Device ID
* \param [in] key_id_p        - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
* \param [in] payload_is_double_key   - A boolean to indicate if the payload encompass the result of more
*                                       than one TCAM access.
* \param [out] field_io_p     - The field io being used.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_field_io_ipmf3(
    int unit,
    dnx_field_key_id_t *key_id_p,
    int payload_is_double_key,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");
    *field_io_p = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    /*
     * For each key, check if the payload encompasses two TCAM accesses, and if so if the keys are consecutive.
     */
    switch (key_id_p->id[0])
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_A:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_B)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_B:
            if (payload_is_double_key)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d does not support double key as first key.\n", key_id_p->id[0]);
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_C:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d Does not support TCAM lookup in iPMF3.\n", key_id_p->id[0]);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid key_id=%d for iPMF3.\n", key_id_p->id[0]);
            break;
    }

    /*
     * If we have reached this point and still haven't allocated a field_io, than we had a double key without
     * a legal pair of keys.
     */
    if (*field_io_p == DBAL_NOF_ENUM_FIELD_IO_VALUES)
    {
        if (payload_is_double_key)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Non matching keys for double key TCAM lookup: %d and %d.\n",
                         key_id_p->id[0], key_id_p->id[1]);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "No field_io was found for an unknown reason. This error should not be reached.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the correct field io for the lookup payload,
*  used to find the correct key select later on.
*  This function is used for the field stage ePMF.
* \param [in] payload_is_double_key   - A boolean to indicate if the payload encompass the result of more
*                                       than one TCAM access.
* \param [in] unit            - Device ID
* \param [in] key_id_p        - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
* \param [out] field_io_p       - The field io being used.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_field_io_epmf(
    int unit,
    dnx_field_key_id_t *key_id_p,
    int payload_is_double_key,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io");

    *field_io_p = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    /*
     * For each key, check if the payload encompasses two TCAM accesses, and if so if the keys are consecutive.
     */
    switch (key_id_p->id[0])
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_B:
            if (payload_is_double_key && key_id_p->id[1] == DBAL_ENUM_FVAL_FIELD_KEY_C)
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1;
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_C:
            if (payload_is_double_key)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d does not support double key as first key.\n", key_id_p->id[0]);
            }
            else
            {
                *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1;
            }
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_A:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id=%d Does not support TCAM lookup in ePMF.\n", key_id_p->id[0]);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid key_id=%d for ePMF.\n", key_id_p->id[0]);
            break;
    }

    /*
     * If we have reached this point and still haven't allocated a field_io, than we had a double key without
     * a legal pair of keys.
     */
    if (*field_io_p == DBAL_NOF_ENUM_FIELD_IO_VALUES)
    {
        if (payload_is_double_key)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Non matching keys for double key TCAM lookup: %d and %d.\n",
                         key_id_p->id[0], key_id_p->id[1]);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "No field_io was found for an unknown reason. This error should not be reached.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the field IO (which identifies where the result of the lookup is located) from the
*  field stage and the key/keys being used.
* \param [in] unit                 - Device ID
* \param [in] field_stage          - Field Stage (IPMF1/2/3/EPMF)
* \param [in] key_id_p             - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
* \param [in] action_length_type   - An enum identifying the size of the TCAM result. Used to support the scenario
*                                    where we use a double key TCAM with payload size of 32 or 64.
* \param [out] field_io_p          - The identifier of where the lookup result returns to, the input to the FESes.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_find_field_io(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_id_t *key_id_p,
    dnx_field_action_length_type_e action_length_type,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    int is_double_key;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Check if we use double key TCAM payload or not, and verify that 'action_length_type' is of a TCAM type.
     */
    if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE)
    {
        is_double_key = TRUE;
    }
    else if (action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF ||
             action_length_type == DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE)
    {
        is_double_key = FALSE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "action_length_type %d is not a TCAM lookup payload.\n", action_length_type);
    }
    /*
     * Note that at this point we can test that the keys are consecutive if we use double keys and simplify * later
     * logic, if we assume that consecutive keys would have consecutive enums and that all double keys * are
     * consecutive.
     */

    /*
     * Decide on the field IO depending of the stage and key.
     */
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_field_io_ipmf1(unit, key_id_p, is_double_key, field_io_p));
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_field_io_ipmf2(unit, key_id_p, is_double_key, field_io_p));
    }
    else if (field_stage == DNX_FIELD_STAGE_IPMF3)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_field_io_ipmf3(unit, key_id_p, is_double_key, field_io_p));
    }
    else if (field_stage == DNX_FIELD_STAGE_EPMF)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_field_io_epmf(unit, key_id_p, is_double_key, field_io_p));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field stage %d not supported for internal TCAM lookup.\n", field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function to make sure the action_length_type selected for EXEM is correct.
* \param [in] unit                - Device ID
* \param [in] field_stage         - Field Stage (IPMF1/2/3/EPMF). Only used for sanity check.
* \param [in] key_id_p            - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
*                                   Only used for sanity check.
* \param [in] action_length_type  - An enum identifying the size of the TCAM result. Used to identify the PDB.
* \return
*   shr_error_e - Error Type
* \remark
*   * This function is a sanity check and can be removed.
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_find_field_io_action_length_type_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_id_t *key_id_p,
    dnx_field_action_length_type_e action_length_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Sanity check: Verify that the key and action length type matches the field stage
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            if (action_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF1 supports only LEXEM. 'action_length_type' is %d.\n",
                             action_length_type);
            }
            if (key_id_p->id[0] != DBAL_ENUM_FVAL_FIELD_KEY_E || key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF1 supports only key E, key ID given (%d,%d).\n",
                             key_id_p->id[0], key_id_p->id[1]);
            }
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            if (action_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF2 supports only SEXEM. 'action_length_type' is %d.\n",
                             action_length_type);
            }
            if (key_id_p->id[0] != DBAL_ENUM_FVAL_FIELD_KEY_J || key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF1 supports only key J, key ID given (%d,%d).\n",
                             key_id_p->id[0], key_id_p->id[1]);
            }
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            if (action_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF3 supports only SEXEM. 'action_length_type' is %d.\n",
                             action_length_type);
            }
            if (key_id_p->id[0] != DBAL_ENUM_FVAL_FIELD_KEY_C || key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF1 supports only key C, key ID given (%d,%d).\n",
                             key_id_p->id[0], key_id_p->id[1]);
            }
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            if (action_length_type != DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in ePMF supports only LEXEM. 'action_length_type' is %d.\n",
                             action_length_type);
            }
            if (key_id_p->id[0] != DBAL_ENUM_FVAL_FIELD_KEY_A || key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "EXEM in iPMF1 supports only key A, key ID given (%d,%d).\n",
                             key_id_p->id[0], key_id_p->id[1]);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported stage for EXEM: %s.\n", dnx_field_stage_text(unit, field_stage));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the field IO (which identifies where the result of the lookup is located) from 'action_length_type'.
* \param [in] unit                - Device ID
* \param [in] field_stage         - Field Stage (IPMF1/2/3/EPMF). Only used for sanity check.
* \param [in] key_id_p            - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
*                                   Only used for sanity check.
* \param [in] action_length_type  - An enum identifying the size of the TCAM result. Used to identify the PDB.
* \param [out] field_io_p         - The identifier of where the lookup result returns to, the input to the FESes.
* \return
*   shr_error_e - Error Type
* \remark
*   * Since field stage determines the action_length_type, and for each stage there is only one possible key
*     that EXEM can use, field_stage and key_id_p parameters are only uses as a sanity check to verify that
*     action_length_type is correct.
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_find_field_io(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_id_t *key_id_p,
    dnx_field_action_length_type_e action_length_type,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");

    /*
     * Sanity check: Verify that the key and action length type matches the field stage.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_field_group_exem_find_field_io_action_length_type_verify
                          (unit, field_stage, key_id_p, action_length_type));

    /*
     * Find the field_io.
     */
    switch (action_length_type)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM:
        {
            *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_SEXEM;
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM:
        {
            *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_LEXEM;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "action_length_type %d is not valid for EXEM.\n", action_length_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the field IO (which identifies where the FESes take their input from) from the keys used.
* \param [in] unit        - Device ID
* \param [in] field_stage - Field Stage. Only iPMF2 and iPMF3 have direct extraction.
* \param [in] key_id_p    - The keys used as data source by the FES in the direct extraction.
*                           The first key will later be used as a reference point for the locations on the qualifiers
*                           on the qualifiers.
*                           If we have more than one key we expect the second key to be the succeeding key to the first,
*                           otherwise we ignore it.
*                           At the moment ignores the key part.
* \param [out] field_io_p - The identifier of where the the FES key selects should take the data from.
* \return
*   shr_error_e - Error Type
* \remark
*   * At the moment this function supports the use of up to two keys.
*     That only limits a single action to not span three keys.
* \see
*   * None
*/
static shr_error_e
dnx_field_group_mdb_dt_find_field_io(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_id_t *key_id_p,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");

    /*
     * Find the keys that can be used for direct extraction in the field stage.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF3:
        {
            *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_DIRECT_0;
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_DIRECT_1;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s (%d) does not support MDB direct table.\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Find the field IO (which identifies where the FESes take their input from) from the keys used.
* \param [in] unit        - Device ID
* \param [in] field_stage - Field Stage. Only iPMF2 and iPMF3 have direct extraction.
* \param [in] key_id_p    - The keys used as data source by the FES in the direct extraction.
*                           The first key will later be used as a reference point for the locations on the qualifiers
*                           on the qualifiers.
*                           If we have more than one key we expect the second key to be the succeeding key to the first,
*                           otherwise we ignore it.
*                           At the moment ignores the key part.
* \param [out] field_io_p - The identifier of where the the FES key selects should take the data from.
* \return
*   shr_error_e - Error Type
* \remark
*   * At the moment this function supports the use of up to two keys.
*     That only limits a single action to not span three keys.
* \see
*   * None
*/
static shr_error_e
dnx_field_group_de_find_field_io(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_key_id_t *key_id_p,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    dbal_enum_value_field_field_key_e first_key = DBAL_NOF_ENUM_FIELD_KEY_VALUES;
    dbal_enum_value_field_field_key_e second_key = DBAL_NOF_ENUM_FIELD_KEY_VALUES;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");

    /*
     * Find the keys that can be used for direct extraction in the field stage.
     */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF2:
        {
            first_key = DBAL_ENUM_FVAL_FIELD_KEY_I;
            second_key = DBAL_ENUM_FVAL_FIELD_KEY_J;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            first_key = DBAL_ENUM_FVAL_FIELD_KEY_A;
            second_key = DBAL_ENUM_FVAL_FIELD_KEY_B;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            first_key = DBAL_ENUM_FVAL_FIELD_KEY_D;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s (%d) does not support direct extraction.\r\n",
                         dnx_field_stage_text(unit, field_stage), field_stage);
        }
    }

    /*
     * Find the field io.
     */
    if (key_id_p->id[0] == first_key)
    {
        if (key_id_p->id[1] == second_key)
        {
            *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0_1;
        }
        else
        {
            *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_0;
        }
    }
    else if (key_id_p->id[0] == second_key)
    {
        *field_io_p = DBAL_ENUM_FVAL_FIELD_IO_DIR_EXT_KEY_1;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid key ID (%d) for direct extraction in stage %s (%d).\r\n",
                     key_id_p->id[0], dnx_field_stage_text(unit, field_stage), field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the payload field_io of a field group.
* \param [in] unit               - Device ID
* \param [in] field_stage        - Field Stage.
* \param [in] fg_type            - Field group type.
* \param [in] key_id_p           - Key or keys used by the field group.
*                                  Not used for field group types that have only one possible key.
* \param [in] action_length_type - Action length type. Indicated the size of the key used.
*                                  Not used for direct extraction.
* \param [out] field_io_p        - to be loaded with the payload field_io.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_find_field_io(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t *key_id_p,
    dnx_field_action_length_type_e action_length_type,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(field_io_p, _SHR_E_PARAM, "field_io_p");

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_find_field_io
                            (unit, field_stage, key_id_p, action_length_type, field_io_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_exem_find_field_io
                            (unit, field_stage, key_id_p, action_length_type, field_io_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_de_find_field_io(unit, field_stage, key_id_p, field_io_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_mdb_dt_find_field_io(unit, field_stage, key_id_p, field_io_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            (*field_io_p) = DBAL_ENUM_FVAL_FIELD_IO_NO_INPUT;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            (*field_io_p) = DBAL_ENUM_FVAL_FIELD_IO_STATE_TABLE;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            /**
             * In case of StateTable and KBP we don't have action resolution.
             */
            (*field_io_p) = DBAL_NOF_ENUM_FIELD_IO_VALUES;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group type %d (%s) is unknown for field_io calculation.\r\n",
                         fg_type, dnx_field_group_type_e_get_name(fg_type));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_field_io_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_io_e * field_io_p)
{
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    dnx_field_key_id_t key_id;
    dnx_field_action_length_type_e action_length_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get(unit, fg_id, context_id, &key_id));
    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));

    SHR_IF_ERR_EXIT(dnx_field_group_find_field_io(unit, field_stage, fg_type, &key_id, action_length_type, field_io_p));

exit:
    SHR_FUNC_EXIT;
}

/** See H file field_group.h*/
shr_error_e
dnx_field_group_fem_invalidated_fems(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fems_invalidate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID])
{
    dnx_field_fem_id_t fem_id_index;
    int nof_fem_ids = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    unsigned int using_fem;
    int invalidated_fem_index;
    dnx_field_fem_program_t fem_program;

    SHR_FUNC_INIT_VARS(unit);

    for (invalidated_fem_index = 0, fem_id_index = 0; fem_id_index < nof_fem_ids; fem_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id_index, fg_id, &fem_program));
        if (fem_program != DNX_FIELD_FEM_PROGRAM_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_field_fem_is_context_using(unit, fem_id_index, context_id, fg_id, &using_fem));
            if (using_fem == FALSE)
            {
                fems_invalidate[invalidated_fem_index] = fem_id_index;
                invalidated_fem_index++;
            }
        }
    }
    /** Fill the rest with invalid FEM ID.*/
    for (; invalidated_fem_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID; invalidated_fem_index++)
    {
        fems_invalidate[invalidated_fem_index] = DNX_FIELD_FEM_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the logical FES configuration and also configure the HW
 * \param [in] unit                    - Device ID
 * \param [in] field_stage             - Field Stage (PMFA/B/E)
 * \param [in] fg_id                   - Field Group id
 * \param [in] context_id              - Context id to configure the FESes for
 * \param [in] field_io                - The location the FESes will read from.
 * \param [in] actions_payload_info_p  - Payload info that was loaded into HW (For example how TCAM entry action info looks like)
 * \param [in] action_info             -
 *   Information per action specific to the attach (e.g. priority).
 *   This is a pointer to an array of information about the actions.
 *   Maximal number of actions accepted here: DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP.
 *   The list of actions ends when an action of value 'DNX_FIELD_ACTION_INVALID' is detected on
 *   the corresponding 'dnx_action' array (See 'dnx_field_group_attach_info_t').
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_group_fes_unconfig()
 *   * dnx_field_group_context_add_fes_update()
 *   * dnx_field_group_fems_context_attach()
 */
static shr_error_e
dnx_field_group_fes_config(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_io_e field_io,
    dnx_field_actions_fg_payload_info_t * actions_payload_info_p,
    dnx_field_action_attach_info_t action_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    /** Structure for FES configuration*/
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_context_t impf2_context_id;
    dnx_field_context_t parent_context_id;
    uint8 is_allocated;
    int template_shift;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_payload_info_p, _SHR_E_PARAM, "actions_payload_info_p");

    /*
     * Initializations to appease the compiler.
     */
    parent_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
    is_allocated = FALSE;

    /*
     * Find any shift the field group may have.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &template_shift));

    SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                    (unit, field_stage, field_io, actions_payload_info_p->actions_on_payload_info, template_shift,
                     action_info, fes_inst_info));
    /** Allocate and configure the FESes.*/
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        /*
         * Add the FESes to each context in iPMF2 that inherits from the iPMF1 context.
         */
        for (impf2_context_id = 0;
             impf2_context_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
             impf2_context_id++)
        {
            /*
             * Note that for now we rely on the fact that the context allocation for iPMF1 and iPMF2 is from the same
             * pool to not check that we have mismatch between 1 to many inheritance and the allocated context IDs,
             * and also we always know that the IPMF1 context will have an IPMF2 counterpart allocated (because the
             * allocation verification will find the context itself).
             */
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &is_allocated));
            if (is_allocated)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &parent_context_id));
            }
            if (is_allocated && parent_context_id == context_id)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                                (unit, DNX_FIELD_STAGE_IPMF2, fg_id, impf2_context_id, FALSE, FALSE, fes_inst_info));
            }
        }
    }
    else
    {
        /*
         * All PMF stages but iPMF1 configure the FESes for their own context ID.
         */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_set(unit, field_stage, fg_id, context_id, FALSE, FALSE, fes_inst_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete from HW the FES configuration for a specific context and field group.
 * \param [in] unit                    - Device ID
 * \param [in] field_stage             - Field Stage (PMFA/B/E)
 * \param [in] fg_id                   - Field Group id
 * \param [in] context_id              - Context id to delete FES configuration for.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_group_fes_config()
 *   * dnx_field_group_context_destroy_fes_update()
 *   * dnx_field_group_fems_context_detach()
 */
static shr_error_e
dnx_field_group_fes_unconfig(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    /** Structure for FES configuration*/
    dnx_field_context_t impf2_context_id;
    dnx_field_context_t parent_context_id;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initializations to appease the compiler.
     */
    parent_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
    is_allocated = FALSE;

    /** Delete the FES configuration.*/
    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        /*
         * Delete the FESes for each context in iPMF2 that inherits from the iPMF1 context.
         */
        for (impf2_context_id = 0;
             impf2_context_id < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_contexts;
             impf2_context_id++)
        {
            /*
             * Note that for now we rely on the fact that the context allocation for iPMF1 and iPMF2 is from the same
             * pool to not check that we have mismatch between 1 to many inheritance and the allocated context IDs,
             * and also we always know that the IPMF1 context will have and IPMF2 counterpart allocated (because the
             * allocation verification will find the context itself).
             */
            SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated
                            (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &is_allocated));
            if (is_allocated)
            {
                SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                (unit, DNX_FIELD_STAGE_IPMF2, impf2_context_id, &parent_context_id));
            }
            if (is_allocated && parent_context_id == context_id)
            {
                SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, DNX_FIELD_STAGE_IPMF2, fg_id, impf2_context_id,
                                                             DNX_FIELD_EFES_ID_INVALID));
            }
        }
    }
    else
    {
        /*
         * All PMF stages but iPMF1 configure the FESes for their own context ID.
         */
        SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach(unit, field_stage, fg_id, context_id, DNX_FIELD_EFES_ID_INVALID));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the logical FES configuration and also configure the HW
 * \param [in] unit                    - Device ID
 * \param [in] ace_id                  - ACE ID
 * \param [in] field_io                - The location the FESes will read from.
 *                                       For ACE it will always be DBAL_ENUM_FVAL_FIELD_IO_ACE.
 * \param [in] actions_payload_info_p  - Payload info that was loaded into HW (For example how entry action info looks like)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_ace_format_fes_config(
    int unit,
    dnx_field_ace_id_t ace_id,
    dbal_enum_value_field_field_io_e field_io,
    dnx_field_actions_ace_payload_info_t * actions_payload_info_p)
{
    int action_index;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_action_attach_info_t attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_payload_info_p, _SHR_E_PARAM, "actions_payload_info_p");

    /** Initialize attach_info. */
    for (action_index = 0; action_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_attach_info_single_t_init(unit, &attach_info[action_index]));
    }

    SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                    (unit, DNX_FIELD_STAGE_ACE, field_io, actions_payload_info_p->actions_on_payload_info,
                     0, attach_info, fes_inst_info));
    /*
     * We do not use dnx_field_actions_attach_info_convert() because we do not have priority.
     */
    /** Allocate and configure the FES's*/
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_ace_set(unit, ace_id, fes_inst_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_context_create_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from)
{
    uint8 is_alloc;
    dnx_field_group_t fg_id_ndx;
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    int is_attached;
    dnx_field_stage_e fg_stage;
    unsigned int context_id_ndx;
    dnx_field_actions_fes_context_group_get_info_t fes_group_info;
    dnx_field_action_attach_info_t attach_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    int is_dir_ext;
    dnx_field_group_type_e fg_type;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_action_length_type_e action_length_type;
    dnx_field_key_id_t key_id;
    dbal_enum_value_field_field_io_e field_io;
    int template_shift;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only iPMF2 needs to update FESes from the context it cascades from.
     */
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * Sanity check: Although it should have been verified, we check that we cascade from a different context_id
         *               than the one we use.
         */
        if (context_id == cascaded_from)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Attempting to allocate an iPMF2 context for default cascading (context id %d).\r\n",
                         context_id);
        }
        /*
         * Go over every field group that is attached to the context we cascade from.
         */
        for (fg_id_ndx = 0; fg_id_ndx < dnx_data_field.group.nof_fgs_get(unit); fg_id_ndx++)
        {
                /** Check if field group is allocated */
            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_alloc));
            if (!is_alloc)
            {
                    /** If field_group is not allocated, move to next FG ID */
                continue;
            }
            else
            {
                    /** Check if the field group is at stage iPMF1.*/
                SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_ndx, &fg_stage));
                if (fg_stage == DNX_FIELD_STAGE_IPMF1)
                {
                        /** Get the list of all contexts that the FG is attached to. */
                    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_ndx, context_id_arr));
                    /*
                     * Find if the context we cascade from is attached to the context we cascade from.
                     */
                    is_attached = FALSE;
                    for (context_id_ndx = 0; context_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
                         context_id_arr[context_id_ndx] != DNX_FIELD_CONTEXT_ID_INVALID; context_id_ndx++)
                    {
                        if (context_id_arr[context_id_ndx] == cascaded_from)
                        {
                            is_attached = TRUE;
                            break;
                        }
                    }
                    if (is_attached)
                    {
                        /*
                         * Attach the FESes to the context.
                         */
                        /*
                         * Collect the action template, attach info, and the payload field_io.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id_ndx, &actions_payload_info));
                        SHR_IF_ERR_EXIT(dnx_field_group_context_fes_info_payload_attach_info_get
                                        (unit, fg_id_ndx, cascaded_from, &fes_group_info, attach_info));
                        SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id_ndx, &actions_payload_info));
                        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id_ndx, &fg_type));
                        SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id_ndx, &action_length_type));
                        SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get(unit, fg_id_ndx, cascaded_from, &key_id));
                        SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                                        (unit, DNX_FIELD_STAGE_IPMF1, fg_type, &key_id, action_length_type, &field_io));
                        
                        if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
                        {
                            is_dir_ext = TRUE;
                        }
                        else
                        {
                            is_dir_ext = FALSE;
                        }
                        /*
                         * Find any shift the field group may have.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id_ndx, &template_shift));
                        /*
                         * Recalculate the FES configuration, as it is not saved in SW state.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                                        (unit, DNX_FIELD_STAGE_IPMF1, field_io,
                                         actions_payload_info.actions_on_payload_info, template_shift,
                                         attach_info, fes_inst_info));

                        /*
                         * Configure the FESes.
                         * Note that the FESes are set for iPMF1 and not iPMF2, as the FESes are part of
                         * an iPMF1 field group.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_actions_fes_set
                                        (unit, DNX_FIELD_STAGE_IPMF1, fg_id_ndx, context_id, is_dir_ext,
                                         FALSE, fes_inst_info));
                    }
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "No stage other than iPMF2 has cascaded contexts and inter stage shared FESes. "
                     "Attempted to update FESes for stage %s context Id %d.\r\n",
                     dnx_field_stage_text(unit, stage), context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_context_destroy_fes_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from)
{
    uint8 is_alloc;
    dnx_field_group_t fg_id_ndx;
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    int is_attached;
    dnx_field_stage_e fg_stage;
    unsigned int context_id_ndx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Note that since we destroy the context, we can simply delete all FESes from the context,
     * But to maintains consistency we still dettach each field group separately.
     */

    /*
     * Only iPMF2 needs to update FESes from the context it cascades from.
     */
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * Sanity check: Although it should have been verified, we check that we cascade from a different context_id
         *               than the one we use.
         */
        if (context_id == cascaded_from)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Attempting to allocate an iPMF2 context for default cascading (context id %d).\r\n",
                         context_id);
        }
        /*
         * Go over every field group that is attached to the context we cascade from.
         */
        for (fg_id_ndx = 0; fg_id_ndx < dnx_data_field.group.nof_fgs_get(unit); fg_id_ndx++)
        {
            /** Check if field group is allocated */
            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_alloc));
            if (!is_alloc)
            {
                /** If field_group is not allocated, move to next FG ID */
                continue;
            }
            else
            {
                /** Check if the field group is at stage iPMF1.*/
                SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_ndx, &fg_stage));
                if (fg_stage == DNX_FIELD_STAGE_IPMF1)
                {
                    /** Get the list of all contexts that the FG is attached to. */
                    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_ndx, context_id_arr));
                    /*
                     * Find if the context we cascade from is attached to the context we cascade from.
                     */
                    is_attached = FALSE;
                    for (context_id_ndx = 0; context_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
                         context_id_arr[context_id_ndx] != DNX_FIELD_CONTEXT_ID_INVALID; context_id_ndx++)
                    {
                        if (context_id_arr[context_id_ndx] == cascaded_from)
                        {
                            is_attached = TRUE;
                            break;
                        }
                    }
                    if (is_attached)
                    {
                        /*
                         * Detach the FESes from the context.
                         * Note that we detach for iPMF1 and not iPMF2, since the actions belong to an iPMF1
                         * field group.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_actions_fes_detach
                                        (unit, DNX_FIELD_STAGE_IPMF1, fg_id_ndx, context_id,
                                         DNX_FIELD_EFES_ID_INVALID));
                    }
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "No stage other than iPMF2 has cascaded contexts and inter stage shared FESes. "
                     "Attempted to update FESes for stage %s context Id %d.\r\n",
                     dnx_field_stage_text(unit, stage), context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Calculate the length type of key and action
 * Action template configuration and HW DB (TCAM) uses these values later on
 * \param [in] unit                   -
 *   Device Id
 * \param [in] key_size_in_bits       -
 *   Key template size in bits
 * \param [in] actions_size_in_bits   -
 *   Size of the payload in bits.
 * \param [out] key_length_type_p     -
 *   Pointer to dnx_field_key_length_type_e (look at enum for more info)
 * \param [out] actions_length_type_p -
 *   Pointer to dnx_field_action_length_type_e (look at enum for more info)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_group_tcam_key_and_action_length_type_calc(
    int unit,
    uint32 key_size_in_bits,
    uint32 actions_size_in_bits,
    dnx_field_key_length_type_e * key_length_type_p,
    dnx_field_action_length_type_e * actions_length_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_length_type_p, _SHR_E_PARAM, "key_length_type_p");
    SHR_NULL_CHECK(actions_length_type_p, _SHR_E_PARAM, "actions_length_type_p");

    if (key_size_in_bits <= dnx_data_field.tcam.key_size_half_get(unit) &&
        actions_size_in_bits <= dnx_data_field.tcam.action_size_half_get(unit))
    {
        *key_length_type_p = DNX_FIELD_KEY_LENGTH_TYPE_HALF;
        *actions_length_type_p = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF;
    }
    else if (key_size_in_bits <= dnx_data_field.tcam.key_size_single_get(unit)
             && actions_size_in_bits <= dnx_data_field.tcam.action_size_single_get(unit))
    {
        *key_length_type_p = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;
        *actions_length_type_p = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE;
        if (actions_size_in_bits <= dnx_data_field.tcam.action_size_half_get(unit))
        {
            /*
             * We override single action length with half to introduce (single_key/half_action) FG combination that
             * can reside with DT FGs on the same bank.
             */
            *actions_length_type_p = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF;
        }
    }
    else if (key_size_in_bits <= dnx_data_field.tcam.key_size_double_get(unit)
             && actions_size_in_bits <= dnx_data_field.tcam.action_size_double_get(unit))
    {
        *key_length_type_p = DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE;
        *actions_length_type_p = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key or actions length bigger than allowed for TCAM, input_key_size=%d, input_actions_size=%d\n",
                     key_size_in_bits, actions_size_in_bits);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Calculate the key length type for Field group type Direct extraction
* \param [in] unit                   - Device Id
* \param [in] key_size_in_bits       - Key template size in bits
* \param [out] key_length_type_p      - Pointer to dnx_field_key_length_type_e (look at enum for more info)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_de_key_length_type_calc(
    int unit,
    uint32 key_size_in_bits,
    dnx_field_key_length_type_e * key_length_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_length_type_p, _SHR_E_PARAM, "key_length_type_p");

    /**
     * For a 'direct extraction' FG type, we allow either a single key or half a key
     * per field group. Double key is not allowed.
     */
    if (key_size_in_bits <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot use zero key size for direct extraction, please use field group type const instead. "
                     "input_key_size=%d.\r\n", key_size_in_bits);
    }
    else if (key_size_in_bits <= dnx_data_field.dir_ext.half_key_size_get(unit))
    {
        *key_length_type_p = DNX_FIELD_KEY_LENGTH_TYPE_HALF;
    }
    else if (key_size_in_bits <= dnx_data_field.dir_ext.single_key_size_get(unit))
    {
        *key_length_type_p = DNX_FIELD_KEY_LENGTH_TYPE_SINGLE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "For a direct extraction field group, this key length is bigger than allowed, input_key_size=%d \r\n",
                     key_size_in_bits);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * Calculate the length type of key and action
 * Action template configuration and HW DB (EXEM) uses these values later on
 * \param [in] unit                   - Device Id
 * \param [in] field_stage            - Field Stage in EXEM different stage can access different EXEM sizes
 * \param [in] key_size_in_bits       - Key template size in bits
 * \param [in] actions_size_in_bits   - Action size in payload.
 * \param [in] exem_info_p            - The paramaeters of the EXEM PDB.
 * \param [out] key_length_type_p     - Pointer to dnx_field_key_length_type_e (look at enum for more info)
 * \param [out] action_length_type_p - Pointer to dnx_field_action_length_type_e (look at enum for more info)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_group_exem_key_and_action_length_type_calc(
    int unit,
    dnx_field_stage_e field_stage,
    uint32 key_size_in_bits,
    uint32 actions_size_in_bits,
    dnx_field_group_exem_info_t * exem_info_p,
    dnx_field_key_length_type_e * key_length_type_p,
    dnx_field_action_length_type_e * action_length_type_p)
{
    mdb_em_entry_encoding_e entry_encoding;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(exem_info_p, _SHR_E_PARAM, "exem_info_p");
    SHR_NULL_CHECK(key_length_type_p, _SHR_E_PARAM, "key_length_type_p");
    SHR_NULL_CHECK(action_length_type_p, _SHR_E_PARAM, "action_length_type_p");

    /*
     * Verify the key size.
     */
    if (key_size_in_bits > exem_info_p->max_key_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key size=%d for exem in stage \"%s\" (%d) is not supported. maximum key size is %d.\r\n",
                     key_size_in_bits, dnx_field_stage_text(unit, field_stage), field_stage, exem_info_p->max_key_size);
    }
    /*
     * Verify the payload size.
     */
    if (actions_size_in_bits > exem_info_p->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Result size=%d for exem in stage \"%s\" (%d) is not supported. maximum payload size is %d.\r\n",
                     actions_size_in_bits, dnx_field_stage_text(unit, field_stage),
                     field_stage, exem_info_p->max_payload_size);
    }

    /*
     * Verify that the key and payload fit inside an entry.
     * Note that we can do this check only for SEXEM, since LEXEM should always fit.
     */
    rv = mdb_em_get_entry_encoding
        (unit, exem_info_p->dbal_physical_table_id, key_size_in_bits,
         actions_size_in_bits, 0, exem_info_p->app_db_id_size, &entry_encoding);
    if (rv == _SHR_E_PARAM || entry_encoding == MDB_EM_ENTRY_ENCODING_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key size=%d Result size=%d for Field Stage \"%s\" (%d) is not supported "
                     "cannot fit inside an entry.\r\n",
                     key_size_in_bits, actions_size_in_bits, dnx_field_stage_text(unit, field_stage), field_stage);
    }
    SHR_IF_ERR_EXIT(rv);

    /*
     * Set the keylength type and action_length_type
     */
    (*key_length_type_p) = exem_info_p->key_length_type;
    (*action_length_type_p) = exem_info_p->action_length_type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Calculate the length type of key and action for ACE table (PPMC).
 * Since the key is allocated (and has no type) and there are no different payload size options, there is only one type.
 * Therefore this function only checks that the size of the payload fits within the maximum range.
 * \param [in] unit                   - Device Id
 * \param [in] actions_size_in_bits   - Total number of bits used by all actions, including valid bits.
 * \param [out] actions_length_type_p  - Pointer to dnx_field_action_length_type_e (look at enum for more info)
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_ace_format_action_length_type_calc(
    int unit,
    unsigned int actions_size_in_bits,
    dnx_field_action_length_type_e * actions_length_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_length_type_p, _SHR_E_PARAM, "actions_length_type_p");
    if (actions_size_in_bits <= (dnx_data_field.ace.payload_size_get(unit) - dnx_data_field.ace.ace_id_size_get(unit)))
    {
        *actions_length_type_p = DNX_FIELD_ACTION_LENGTH_TYPE_ACE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Result size=%d for ACE table is not supported (maximum size is %d).\n",
                     actions_size_in_bits,
                     (dnx_data_field.ace.payload_size_get(unit) - dnx_data_field.ace.ace_id_size_get(unit)));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* /brief
*  Converts Field stage to its TCAM counterpart.
*  Returns error for unsupported Field stage for TCAM.
*
* \param [in] unit         - Device ID
* \param [in] field_stage  - Field stage to convert
* \param [out] tcam_stage  - The converted TCAM stage for the given Field stage
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_stage_to_tcam_convert(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_tcam_stage_e * tcam_stage)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_IPMF1;
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_IPMF2;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_IPMF3;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            *tcam_stage = DNX_FIELD_TCAM_STAGE_EPMF;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field stage %d is not supported for TCAM", field_stage);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will configure TCAM handler for Field group
 * \param [in] unit                 - Device id
 * \param [in] fg_id                - Field Group ID to create handler for
 * \param [in] field_stage          - Field Stage (PMF1/2/3..)
 * \param [in] fg_type              - Field Group Type (Direct table...)
 * \param [in] key_length_type      - look at dnx_field_key_length_type_e
 * \param [in] prefix_size          - how many bits of prefix can be put in the MSB of the key
 * \param [in] actions_length_type  - look at dnx_field_action_length_type_e
 * \param [in] action_size_in_bits   -
 *   Size of payload in bits.
 * \param [in] nof_tcam_banks        - Number of TCAM banks to be preallocated.
 *                                     If 0 TCAM banks will only be allocated when adding an entry.
 * \param [in] bank_allocation_mode  - TCAM bank allocation mode for the handler to be created
 * \param [in] tcam_bank_ids         - If 'bank_allocation_mode' is SELECT and 'nof_tcam_banks' isn't 0, indicates
 *                                     what TCAM banks to allocate.
 * \param [out] tcam_handler_id_p    - ID of the tcam handler that was created
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_group_tcam_handle_create(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_stage_e field_stage,
    dnx_field_group_type_e fg_type,
    dnx_field_key_length_type_e key_length_type,
    uint32 prefix_size,
    dnx_field_action_length_type_e actions_length_type,
    uint32 action_size_in_bits,
    int nof_tcam_banks,
    dnx_field_tcam_bank_allocation_mode_e bank_allocation_mode,
    int tcam_bank_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *tcam_handler_id_p)
{
    dnx_field_tcam_database_t fg_tcam;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tcam_handler_id_p, _SHR_E_PARAM, "tcam_handler_id_p");

    SHR_IF_ERR_EXIT(dnx_field_tcam_database_t_init(unit, &fg_tcam));

    fg_tcam.action_size = actions_length_type;
    fg_tcam.actual_action_size = action_size_in_bits;
    fg_tcam.key_size = key_length_type;
    SHR_IF_ERR_EXIT(dnx_field_group_stage_to_tcam_convert(unit, field_stage, &(fg_tcam.stage)));
    if (fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        fg_tcam.is_direct = TRUE;
    }
    else
    {
        fg_tcam.is_direct = FALSE;
    }

    
    fg_tcam.nof_restricted_tcam_handlers = 0;
    fg_tcam.prefix_value = DNX_FIELD_TCAM_PREFIX_VAL_AUTO;
    fg_tcam.prefix_size = prefix_size;

    fg_tcam.bank_allocation_mode = bank_allocation_mode;
    fg_tcam.nof_banks_to_alloc = nof_tcam_banks;
    sal_memcpy(fg_tcam.banks_to_alloc, tcam_bank_ids, sizeof(fg_tcam.banks_to_alloc));

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_create
                    (unit, &fg_tcam, FIELD_TCAM_ACCESS_PROFILE_ID_AUTO,
                     DNX_FIELD_TCAM_HANDLER_MODE_DEFAULT, &dnx_field_group_context_sharing_tcam_handlers_get,
                     tcam_handler_id_p));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Creates a DBAL table that represent the (key,action) tuple to insert into HW (TCAM..)
* Later the created DBAL table is used by entry add to add entries into HW (TCAM..)
* \param [in] unit                 - Device ID
* \param [in] fg_id                - Field group ID, used for logging.
* \param [in] field_stage          - Field Stage
* \param [in] dnx_quals_arr        - List of the qualifiers that represent how (key) was constructed
* \param [in] qual_is_ranged       - Indicating which qualifiers are expected to receive ranged input.
* \param [in] actions_info_arr     - Array containing the actions on the payload and whether or not they use valid bit.
* \param [in] name                 - String of the name of the field group.
* \param [in] table_type           - which type of HW DB (TCAM,EXEM ..)
* \param [in] access_method        - which type of access method this table is (MDB/TCAM/KBP ..)
* \param [in] alignment_size       - Alignment size is the size that the qualifiers
*                                    from the dnx_quals_arr should be aligned to
* \param [in] packing_quals        - Packing qualifers to be aligned together.
* \param [in] use_hitbit           - Indication if we should enable HIT BIT for current table.
* \param [out] dbal_table_id_p     - DBAL table that was created
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_dbal_table_create(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_quals_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_is_ranged[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    char name[DBAL_MAX_STRING_LENGTH],
    dbal_table_type_e table_type,
    dbal_access_method_e access_method,
    uint32 alignment_size,
    int packing_quals,
    uint8 use_hitbit,
    dbal_tables_e * dbal_table_id_p)
{
    int qual_idx;
    int action_idx;
    int qual_dbal_idx;
    int action_dbal_idx;
    int nof_quals;
    int nof_actions;
    unsigned int size;
    uint32 aligned_offset;
    dbal_table_field_input_info_t dbal_keys_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { {0} };
    dbal_table_field_input_info_t dbal_results_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE] = { {0} };
    uint8 qual_packed[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint8 qual_packed_last[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint32 qual_sizes[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    int packed_qual_size;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_quals_arr, _SHR_E_PARAM, "dnx_quals_arr");
    SHR_NULL_CHECK(actions_info_arr, _SHR_E_PARAM, "actions_info_arr");
    SHR_NULL_CHECK(dbal_table_id_p, _SHR_E_PARAM, "dbal_table_id_p");

    if (packing_quals && (alignment_size != SAL_UINT8_NOF_BITS))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "acking qualifers can only be done with alignment_size of 8, "
                     "alignment_size is %d. \n", alignment_size);
    }

    /*
     * Count the number of qualifiers and actions to send them to DBAL in order from the MSB to the LSB.
     * This ensures the the first qualifier/action is at the LSB of the key/payload.
     */
    for (nof_quals = 0;
         (nof_quals < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
         && (dnx_quals_arr[nof_quals] != DNX_FIELD_QUAL_TYPE_INVALID); nof_quals++);
    for (nof_actions = 0;
         nof_actions < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_info_arr[nof_actions].dnx_action != DNX_FIELD_ACTION_INVALID; nof_actions++);

    /*
     * Find qual sizes
     */
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_quals_arr[qual_idx], &qual_sizes[qual_idx]));
    }

    /*
     * Find qualifers to be packed together.
     */
    if (packing_quals)
    {
        int qual_idx_2;
        for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
        {
            if (qual_packed[qual_idx])
            {
                continue;
            }
            if (qual_sizes[qual_idx] != (BYTES2BITS(BITS2BYTES(qual_sizes[qual_idx]))))
            {
                packed_qual_size = qual_sizes[qual_idx];
                for (qual_idx_2 = qual_idx + 1; qual_idx_2 < nof_quals; qual_idx_2++)
                {
                    if (qual_sizes[qual_idx_2] != (BYTES2BITS(BITS2BYTES(qual_sizes[qual_idx_2]))))
                    {
                        /*
                         * Check if the packing can make the key smaller.
                         */
                        if ((BITS2BYTES(packed_qual_size + qual_sizes[qual_idx_2])) <
                            ((BITS2BYTES(packed_qual_size)) + BITS2BYTES(qual_sizes[qual_idx_2])))
                        {
                            qual_packed[qual_idx_2] = TRUE;
                            qual_packed[qual_idx] = TRUE;
                            packed_qual_size += qual_sizes[qual_idx_2];
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                if (qual_idx_2 > qual_idx + 1)
                {
                    qual_packed_last[qual_idx_2 - 1] = TRUE;
                }
            }
        }
    }

    packed_qual_size = 0;
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        qual_dbal_idx = nof_quals - 1 - qual_idx;
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage,
                                                       dnx_quals_arr[qual_idx],
                                                       &dbal_keys_info[qual_dbal_idx].field_id));
        if (qual_packed[qual_idx] == FALSE)
        {
            packed_qual_size = 0;
        }
        else
        {
            packed_qual_size += qual_sizes[qual_idx];
        }

        dbal_keys_info[qual_dbal_idx].field_nof_bits = qual_sizes[qual_idx];

        /*
         * In some cases (KBP for example) the qualifier has to be aligned to a predefined size.
         * the dbal table should be updated accordingly.
         * In general case the alignment_size will be 0 and will not have any effect on the
         * qual_size. In case alignment_size differs from 0, and the qualifier is of an unaligned size
         * we will add the (alignment_size - qual_size(modulo(alignment_size))) to the qual_size to make it aligned
         */
        if (alignment_size != 0)
        {
            if (packing_quals == FALSE || qual_packed[qual_idx] == FALSE)
            {
                aligned_offset = (qual_sizes[qual_idx] % alignment_size) ?
                    (alignment_size - qual_sizes[qual_idx] % alignment_size) : (0);
                dbal_keys_info[qual_dbal_idx].field_nof_bits += aligned_offset;
            }
            else if (qual_packed_last[qual_idx])
            {
                aligned_offset = (packed_qual_size % alignment_size) ?
                    (alignment_size - packed_qual_size % alignment_size) : (0);
                dbal_keys_info[qual_dbal_idx].field_nof_bits += aligned_offset;
            }
        }
        dbal_keys_info[qual_dbal_idx].is_ranged = qual_is_ranged[qual_idx];
        /*
         * Mark packed fields except the first from DBAL's point of view, which is the last.
         */
        if (qual_packed[qual_idx] && qual_packed_last[qual_idx] == FALSE)
        {
            dbal_keys_info[qual_dbal_idx].is_packed = TRUE;
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "dnx_field_group_dbal_table_create stage %s qual %s class %s size %d\n ",
                     dnx_field_stage_text(unit, field_stage),
                     dnx_field_dnx_qual_text(unit, dnx_quals_arr[qual_idx]),
                     dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_quals_arr[qual_idx])),
                     dbal_keys_info[qual_dbal_idx].field_nof_bits);
        if (qual_packed_last[qual_idx])
        {
            packed_qual_size = 0;
        }
    }
    for (action_idx = 0; action_idx < nof_actions; action_idx++)
    {
        action_dbal_idx = nof_actions - 1 - action_idx;
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, field_stage, actions_info_arr[action_idx].dnx_action,
                         &dbal_results_info[action_dbal_idx].field_id));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                        (unit, field_stage, actions_info_arr[action_idx].dnx_action, &size));
        dbal_results_info[action_dbal_idx].field_nof_bits = size;
        /*
         * All tables that are being created for TCAM need to include valid bit on LSB.
         */
        dbal_results_info[action_dbal_idx].is_valid_indication_needed =
            !(actions_info_arr[action_idx].dont_use_valid_bit);
    }
    SHR_IF_ERR_EXIT_WITH_LOG(dbal_tables_table_create
                             (unit, access_method, table_type, DBAL_CORE_MODE_SBC, nof_quals,
                              dbal_keys_info, nof_actions, dbal_results_info, name, dbal_table_id_p),
                             "failed to create DBAL table for field group %d with name %s.%s\n", fg_id, name, EMPTY);

    if (use_hitbit == TRUE)
    {
        SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, *dbal_table_id_p, DBAL_TABLE_IND_IS_HITBIT_EN, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Add the set of action in the ACE format to be writable as entries into the table.
* Also configures the MDB for the ACE format.
* \param [in] unit                    - Device ID
* \param [in] ace_id                  - The ID of the ACE format. will also serve as the result type to be added and
*                                       the VMV of all the entries of tjhe ACE format.
* \param [in] actions_info_arr        - Array of actions that the ACE format can perform,
*                                       and whether or not they use valid bits.
* \param [in] name                    - String of the name of the ACE format.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_dbal_result_type_add(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    char name[DBAL_MAX_STRING_LENGTH])
{
    int action_idx;
    int action_dbal_idx;
    int nof_actions;
    int nof_result_fields;
    unsigned int size;
    unsigned int nof_bits_in_result_type;
    dbal_table_field_input_info_t results_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE] = { {0} };
    int result_type_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(actions_info_arr, _SHR_E_PARAM, "actions_info_arr");

    nof_bits_in_result_type = dnx_data_field.ace.ace_id_size_get(unit);

    /*
     * Count the number of actions to send them to DBAL in order from the MSB to the LSB.
     * This ensures the the first action is at the LSB of the payload.
     */
    for (nof_actions = 0;
         nof_actions < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && actions_info_arr[nof_actions].dnx_action != DNX_FIELD_ACTION_INVALID; nof_actions++);

    /*
     * Add the result type as the first result field, before the actions.
     */
    results_info[0].field_id = DBAL_FIELD_RESULT_TYPE;
    results_info[0].field_nof_bits = nof_bits_in_result_type;
    results_info[0].is_valid_indication_needed = FALSE;
    nof_result_fields = nof_actions + 1;

    /*
     * Add the rest of the actions.
     */
    for (action_idx = 0; action_idx < nof_actions; action_idx++)
    {
        /*
         * Reverse the order by which we add the actions, and add one to the index to account for the result type.
         * All in all:
         * action_dbal_idx = nof_actions - 1 - action_idx + 1 = nof_actions - action_idx
         */
        action_dbal_idx = nof_actions - action_idx;
        if (action_dbal_idx >= DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions plus result type exceeds the "
                         "maximum result fiesds for DBAL (%d).\n", DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_action
                        (unit, DNX_FIELD_STAGE_ACE, actions_info_arr[action_idx].dnx_action,
                         &results_info[action_dbal_idx].field_id));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size
                        (unit, DNX_FIELD_STAGE_ACE, actions_info_arr[action_idx].dnx_action, &size));
        results_info[action_dbal_idx].field_nof_bits = size;
        /*
         * All tables that are being created need to include valid bit on LSB.
         */
        results_info[action_dbal_idx].is_valid_indication_needed = !(actions_info_arr[action_idx].dont_use_valid_bit);
    }
    SHR_IF_ERR_EXIT(dbal_tables_result_type_add
                    (unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, nof_result_fields, results_info,
                     name, nof_bits_in_result_type, ace_id, &result_type_idx));
    SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, DBAL_PHYSICAL_TABLE_PPMC));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Set the MDB access for the DBAL table according to access_info.
* \param [in] unit                    - Device ID
* \param [in] app_db_id               - The application database identifier for the MDB.
* \param [in] actions_length_type     - An indication whether we use SEXEM or LEXEM.
* \param [in] dbal_table_id           - DBAL table to configure the MDB for.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_dbal_table_access_info_set(
    int unit,
    dnx_field_app_db_id_t app_db_id,
    dnx_field_action_length_type_e actions_length_type,
    dbal_tables_e dbal_table_id)
{
    dbal_table_mdb_access_info_t access_info = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Fill the access info.
     */
    access_info.nof_physical_tables = 1;
    access_info.tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
    access_info.app_id = app_db_id;
    /*
     * Fill the access info according to whether it is SEXEM or LEXEM.
     */
    switch (actions_length_type)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM:
            access_info.nof_physical_tables = 1;
            /** The SEXEM used by the PMF is SEXEM-3. Its PDB is EXEM-3*/
            access_info.physical_db_id[0] = DBAL_PHYSICAL_TABLE_SEXEM_3;
            access_info.app_id_size = dnx_data_field.exem.small_app_db_id_size_get(unit);
            break;
        case DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM:
            access_info.nof_physical_tables = 1;
            /** The PDB of LEXEM is EXEM-4.*/
            access_info.physical_db_id[0] = DBAL_PHYSICAL_TABLE_LEXEM;
            access_info.app_id_size = dnx_data_field.exem.large_app_db_id_size_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid actions_length_type (%d) for EXEM.\n", actions_length_type);
            break;
    }
    /*
     * Call the MDB + DBAL to configure the MDB access layer.
     */
    SHR_IF_ERR_EXIT(mdb_init_logical_table(unit, dbal_table_id, &access_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_qual_arr_compare_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    dnx_field_qual_t dnx_quals_fg[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    unsigned int nof_quals_fg;
    unsigned int nof_quals;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_quals, _SHR_E_PARAM, "dnx_quals");

    /*
     * Get the qualifiers from the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals_fg, &nof_quals_fg));

    /*
     * Count the number of qualifiers in the input array and compare with the field group.
     */
    for (nof_quals = 0;
         nof_quals < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG && dnx_quals[nof_quals] != DNX_FIELD_QUAL_TYPE_INVALID;
         nof_quals++);

    if (nof_quals != nof_quals_fg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of qualifiers (%d) does not match number of qualifiers "
                     "in the field group %d (%d).\r\n", nof_quals, fg_id, nof_quals_fg);
    }

    /*
     *verifying that all the qualifiers present in the field group, and there are no duplicates on dnx_quals.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_qual_arr_subset_verify(unit, fg_id, dnx_quals));
    /*
     * Note that since we verified that array has the same number of qualifiers as the field group, that no qualifier
     * appears twice and that every qualifier appears in the field group, we don't need to check that every qualifier
     * in the field group is represented in the input array.
     */

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_qual_arr_subset_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    dnx_field_qual_t dnx_quals_fg[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    unsigned int nof_quals_fg;
    unsigned int nof_quals;
    unsigned int qual_ndx;
    unsigned int qual_ndx_2;
    unsigned int qual_ndx_fg;
    int qual_found;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_quals, _SHR_E_PARAM, "dnx_quals");

    /*
     * Get the qualifiers from the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals_fg, &nof_quals_fg));

    /*
     * Count the number of qualifiers in the input array and compare with the field group.
     */
    for (nof_quals = 0;
         nof_quals < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG && dnx_quals[nof_quals] != DNX_FIELD_QUAL_TYPE_INVALID;
         nof_quals++);

    /*
     * For each qualifier, verify that it appears in the field group, and that it doesn't appear twice.
     */
    for (qual_ndx = 0; qual_ndx < nof_quals; qual_ndx++)
    {
        /*
         * Verify that the qualifier appears in the field group.
         */
        qual_found = FALSE;
        for (qual_ndx_fg = 0; qual_ndx_fg < nof_quals_fg; qual_ndx_fg++)
        {
            if (dnx_quals[qual_ndx] == dnx_quals_fg[qual_ndx_fg])
            {
                qual_found = TRUE;
                break;
            }
        }
        if (qual_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DNX qual %s (0x%08X) (number %d) does not appear in field group %d.\r\n",
                         dnx_field_dnx_qual_text(unit, dnx_quals[qual_ndx]), dnx_quals[qual_ndx], qual_ndx, fg_id);
        }
        /*
         * Verify that the qualifier doesn't appear twice.
         */
        qual_found = FALSE;
        for (qual_ndx_2 = 0; qual_ndx_2 < qual_ndx; qual_ndx_2++)
        {
            if (dnx_quals[qual_ndx] == dnx_quals[qual_ndx_2])
            {
                qual_found = TRUE;
                break;
            }
        }
        if (qual_found)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DNX qual %s (0x%08X) (number %d) appears twice.\r\n",
                         dnx_field_dnx_qual_text(unit, dnx_quals[qual_ndx]), dnx_quals[qual_ndx], qual_ndx);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_action_arr_subset_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG])
{
    dnx_field_action_t dnx_actions_fg[DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG];
    unsigned int nof_actions_fg;
    unsigned int nof_actions;
    unsigned int action_ndx;
    unsigned int action_ndx_2;
    unsigned int action_ndx_fg;
    int action_found;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_actions, _SHR_E_PARAM, "dnx_actions");

    /*
     * Get the actions from the field group.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_action_arr_get(unit, fg_id, dnx_actions_fg, &nof_actions_fg));

    /*
     * Count the number of qualifiers in the input array and compare with the field group.
     */
    for (nof_actions = 0;
         nof_actions < DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG
         && dnx_actions[nof_actions] != DNX_FIELD_ACTION_INVALID; nof_actions++);

    /*
     * For each action, verify that it appears in the field group, and that it doesn't appear twice.
     */
    for (action_ndx = 0; action_ndx < nof_actions; action_ndx++)
    {
        /*
         * Verify that the action appears in the field group.
         */
        action_found = FALSE;
        for (action_ndx_fg = 0; action_ndx_fg < nof_actions_fg; action_ndx_fg++)
        {
            if (dnx_actions[action_ndx] == dnx_actions_fg[action_ndx_fg])
            {
                action_found = TRUE;
                break;
            }
        }
        if (action_found == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DNX action %s (0x%08X) (number %d) does not appear in field group %d.\r\n",
                         dnx_field_dnx_qual_text(unit, dnx_actions[action_ndx]), dnx_actions[action_ndx], action_ndx,
                         fg_id);
        }
        /*
         * Verify that the action doesn't appear twice.
         */
        action_found = FALSE;
        for (action_ndx_2 = 0; action_ndx_2 < action_ndx; action_ndx_2++)
        {
            if (dnx_actions[action_ndx] == dnx_actions[action_ndx_2])
            {
                action_found = TRUE;
                break;
            }
        }
        if (action_found)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DNX action %s (0x%08X) (number %d) appears twice.\r\n",
                         dnx_field_dnx_qual_text(unit, dnx_actions[action_ndx]), dnx_actions[action_ndx], action_ndx);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Updates the SW state when splitting a zero padding segment.
*  The old segment remains first, but the field sw state information is given to the newer split segment.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP opcode ID
* \param [in] seg_id          - The segment to split.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_acl_split_zero_pad_update_sw_state(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    int seg_id)
{
    uint8 is_valid;
    uint8 nof_fwd_segments;
    dnx_field_group_t fg_id;
    int seg_index;
    dnx_field_group_kbp_segment_info_t segment_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Sanity checks.
     */
    if (seg_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "seg_id %d is negative.\n", seg_id);
    }
    if (seg_id >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "seg_id %d is above maximum %d.\n",
                     seg_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 2);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.is_valid.get(unit, opcode_id, &is_valid));
    if (is_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d is not marked as used by any field group.\n", opcode_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                    nof_fwd_segments.get(unit, opcode_id, &nof_fwd_segments));
    if (seg_id >= nof_fwd_segments)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d splitting zero padding segment %d, but only %d FWD segments.\n",
                     opcode_id, seg_id, nof_fwd_segments);
    }
    if (nof_fwd_segments >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d nof_fwd_segments %d is filling all of the segments on "
                     "master key %d.\n", opcode_id, nof_fwd_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.segment_info.
                    fg_id.get(unit, opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1, &fg_id));
    if (fg_id != DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d splitting zero padding segment %d, "
                     "but last segment is being used.\n", opcode_id, nof_fwd_segments);
    }

    /*
     * Move the segments.
     */
    for (seg_index = DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1; seg_index > seg_id; seg_index--)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                        segment_info.get(unit, opcode_id, seg_index - 1, &segment_info));
        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                        segment_info.set(unit, opcode_id, seg_index, &segment_info));
    }
    /*
     * Delete the info for the new zero padding segment.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_info_t_init(unit, &segment_info));
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                    segment_info.set(unit, opcode_id, seg_id, &segment_info));

    /*
     * Increase the number of FWD segments.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                    nof_fwd_segments.set(unit, opcode_id, nof_fwd_segments + 1));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given FWD segment is an unused zero_padding segment.
* \param [in] unit               - Device ID
* \param [in] ms_key_segment_p   - The FWD segment
* \param [out] is_zero_padding_p - Whether the segment is zero padding.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_acl_segment_is_zero_padding(
    int unit,
    kbp_mngr_key_segment_t * ms_key_segment_p,
    int *is_zero_padding_p)
{
    char zero_padding_string[] = "ZERO_PADDING";
    int zero_padding_string_size = sal_strlen(zero_padding_string);

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ms_key_segment_p, _SHR_E_PARAM, "ms_key_segment_p");

    /** Sanity check. */
    if (zero_padding_string_size >= sizeof(ms_key_segment_p->name))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Zero padding sting (\"%s\" size %d) is too big for segment name (%d).\r\n",
                     zero_padding_string, zero_padding_string_size, (int) sizeof(ms_key_segment_p->name));
    }

    if (sal_memcmp(ms_key_segment_p->name, zero_padding_string, zero_padding_string_size))
    {
        (*is_zero_padding_p) = FALSE;
    }
    else
    {
        (*is_zero_padding_p) = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in the master key in the ACL part of the master key.
*  The verification will be done over all acl segments.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP opcode ID
* \param [in] fg_id           - The qualifier's field group
* \param [in] force_same_qual - Only share the same qualifier type.
* \param [in] nof_packed_quals - The number of qualifiers in the segment.
* \param [in] qual_type       - qualifier type added by user.
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT elements.
* \param [in] qual_info       - attach info as was passed by user
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT elements.
* \param [in] is_overlay      - If we perform sharing using the overlay feature.
* \param [in] already_shared_segments - For each segment on the master key, whether it was already found a sharing
*                                       qualifier on the field group, since we cannot share the same segment twice
*                                       for one DB (field group). Not relevant for overlay sharing.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
*                                    Holds segment_idx if found match
*                                    Holds DNX_KBP_INVALID_SEGMENT_ID if not found
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_acl_segment_exist(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    dnx_field_group_t fg_id,
    int force_same_qual,
    int nof_packed_quals,
    dnx_field_qual_t qual_type[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_info[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int is_overlay,
    uint8 already_shared_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    dnx_field_group_kbp_info_t kbp_kbp_info_info;
    dnx_field_key_template_t key_template;
    dnx_field_qual_attach_info_t qual_attach_info_arr[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 seg_idx;
    uint32 qual_in_key_template_idx;
    int packed_qual_ndx;
    int nof_packed_quals_in_segment;
    dnx_field_qual_t contender_qual_type[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
    dnx_field_qual_attach_info_t contender_attach_info[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
    uint8 is_contender_qual_ranged;
    uint8 is_qual_ranged;
    dbal_fields_e contender_qual_field_id;
    dbal_fields_e qual_field_id;
    dbal_tables_e contender_dbal_table_id;
    dbal_tables_e dbal_table_id;
    dnx_field_group_t contender_fg_id;
    int fully_shareable;
    int partially_shareable;
    int partial_sharing_offset_lsb;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;

    SHR_FUNC_INIT_VARS(unit);

    /** Sanity check. */
    if (nof_packed_quals <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_packed_quals is %d, must be at least 1.\n", nof_packed_quals);
    }

    *found_segment_idx_p = DNX_KBP_INVALID_SEGMENT_ID;
    *msb_offset_bytes_overlay_sharing_p = 0;

    /*
     * Get the kbp information for this opcode. kbp info includes the qualifier type
     and the qualifier attach info hints for all segments in the master key.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.get(unit, opcode_id, &kbp_kbp_info_info));

    /*
     * Get the master key information from KBP, to find which segemnts are overlay segments.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));

    /*
     * Ranged and non-ranged qualifiers cannot be shared at the moment, except with overlay.
     */
    if (is_overlay == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, DNX_FIELD_STAGE_EXTERNAL, qual_type[0], &qual_field_id));
        SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, qual_field_id, &is_qual_ranged));

        if (is_qual_ranged && nof_packed_quals > 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d we have a packed ranged qualifer.\r\n", opcode_id);
        }
    }

    for (seg_idx = 0; seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; seg_idx++)
    {
        /*
         * The same segment cannot be shared twice, except with overlay.
         */
        if ((is_overlay == FALSE) && (already_shared_segments[seg_idx]))
        {
            continue;
        }
        /*
         * Check that the segment isn't an overlay segment already.
         */
        if (ms_key_segments[seg_idx].is_overlay_field)
        {
            continue;
        }
        /*
         * Get the fg_info for a given segment
         */
        contender_fg_id = kbp_kbp_info_info.master_key_info.segment_info[seg_idx].fg_id;
        if (contender_fg_id == DNX_FIELD_GROUP_INVALID)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, contender_fg_id, &key_template));

        SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get(unit, contender_fg_id, opcode_id,
                                                                          qual_attach_info_arr));

        /*
         * Extract the info for each packed qualifer (or for one qualifer if there.
         */
        for (packed_qual_ndx = 0; packed_qual_ndx < DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT; packed_qual_ndx++)
        {
            /*
             * Extract the qualifier index from the given segment info. This is the index of the qualifier_type AND
             * of the attach_info for this qualifier inside the fg_info.
             */
            qual_in_key_template_idx =
                kbp_kbp_info_info.master_key_info.segment_info[seg_idx].qual_idx[packed_qual_ndx];

            if (qual_in_key_template_idx == DNX_FIELD_GROUP_EXTERNAL_PACKED_QUAL_INDEX_INVALID)
            {
                break;
            }

            contender_qual_type[packed_qual_ndx] = key_template.key_qual_map[qual_in_key_template_idx].qual_type;

            /*
             * Qualifier attach_info is pointed by the qual_in_key_template_idx, extracted from the fg_info
             */
            sal_memcpy(&contender_attach_info[packed_qual_ndx], &qual_attach_info_arr[qual_in_key_template_idx],
                       sizeof(contender_attach_info[0]));

        }
        nof_packed_quals_in_segment = packed_qual_ndx;
        if (nof_packed_quals_in_segment <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d  segment %d has field group %d, "
                         "but no qualifers associated with it.\r\n", opcode_id, seg_idx, fg_id);
        }

        /*
         * If we force the qualifers to be the same, verify that they are the same.
         */
        if (force_same_qual)
        {
            int all_quals_the_same = TRUE;
            if (nof_packed_quals != nof_packed_quals_in_segment)
            {
                continue;
            }
            for (packed_qual_ndx = 0; packed_qual_ndx < nof_packed_quals_in_segment; packed_qual_ndx++)
            {
                if (qual_type[packed_qual_ndx] != contender_qual_type[packed_qual_ndx])
                {
                    all_quals_the_same = FALSE;
                    break;
                }
            }
            if (all_quals_the_same == FALSE)
            {
                continue;
            }
        }

        /*
         * Ranged and non-ranged qualifiers cannot be shared at the moment, unless using overlay sharing.
         * ranged qualifiers are not packed.
         */
        if (is_overlay == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, contender_fg_id, &contender_dbal_table_id));
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                            (unit, DNX_FIELD_STAGE_EXTERNAL, contender_qual_type[0], &contender_qual_field_id));
            SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get
                            (unit, contender_dbal_table_id, contender_qual_field_id, &is_contender_qual_ranged));
            if (is_contender_qual_ranged && nof_packed_quals_in_segment > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d we have a packed ranged qualifer.\r\n", opcode_id);
            }
            if ((is_contender_qual_ranged && (is_qual_ranged == FALSE)) ||
                ((is_contender_qual_ranged == FALSE) && is_qual_ranged))
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dnx_field_key_kbp_acl_compare_qualifiers_for_sharing
                        (unit, nof_packed_quals_in_segment, contender_qual_type, contender_attach_info,
                         nof_packed_quals, qual_type, qual_info,
                         &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
        if (fully_shareable)
        {
            /*
             * Full sharing.
             */
            *found_segment_idx_p = seg_idx;
            if (is_overlay)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found ACL match with fg_id %d opcod_id %d qual %s seg_idx %d, using overlay sharing.\n",
                             contender_fg_id, opcode_id, dnx_field_dnx_qual_text(unit, qual_type[0]), seg_idx);
            }
            else
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found ACL match with fg_id %d opcod_id %d qual %s seg_idx %d\n",
                             contender_fg_id, opcode_id, dnx_field_dnx_qual_text(unit, qual_type[0]), seg_idx);
            }
            if (nof_packed_quals > 1)
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "The ACL match with fg_id %d opcod_id %d seg_idx %d is a packed segment "
                             "with %d qualifiers.\n", contender_fg_id, opcode_id, seg_idx, nof_packed_quals);
            }

            break;
        }
        else if (partially_shareable && is_overlay &&
                 (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb) &&
                 (nof_packed_quals == 1))
        {
            uint32 containing_qual_size;
            uint32 contained_qual_size;
            /*
             * Try partial sharing, if we use overlay sharing and the offset from LSB is in full bytes.
             * Do not do partial sharing for packed qualifiers at this point.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type[0], &contained_qual_size));
            *msb_offset_bytes_overlay_sharing_p =
                ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                BITS2BYTES(partial_sharing_offset_lsb);
            *found_segment_idx_p = seg_idx;
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Found ACL match opcod_id %d qual %s seg_idx %d, "
                         "using overlay sharing. Partial sharing with offset from MSB %d\n",
                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type[0]), seg_idx,
                         *msb_offset_bytes_overlay_sharing_p);
            /** Sanity check. */
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, DNX_FIELD_STAGE_EXTERNAL, contender_qual_type[0], &containing_qual_size));
            if (ms_key_segments[seg_idx].nof_bytes != BITS2BYTES(containing_qual_size))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment \"%.*s\" is %d bytes, while qualifier %s is %d bytes.\r\n",
                             DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                             ms_key_segments[seg_idx].nof_bytes, dnx_field_dnx_qual_text(unit, contender_qual_type[0]),
                             BITS2BYTES(containing_qual_size));
            }
            /** Sanity check. */
            if ((*msb_offset_bytes_overlay_sharing_p) < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                             *msb_offset_bytes_overlay_sharing_p);
            }

            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in the master key in the FWD part of the master key.
*  The verification will be done over all fwd segments.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP OPCODE ID
* \param [in] fg_id           - The qualifier's field group
* \param [in] qual_type       - qualifier type added by user
* \param [in] qual_info_p     - attach info as was passed by user
* \param [in] is_overlay      - If we perform sharing using the overlay feature.
* \param [in] already_shared_segments - For each segment on the master key, whether it was already found a sharing
*                                       qualifier on the field group, since we cannot share the same segment twice
*                                       for one DB (field group). Not relevant for overlay sharing.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
*                                    Holds segment_idx if found match
*                                    Holds DNX_KBP_INVALID_SEGMENT_ID if not found
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_fwd_segment_exist(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    int is_overlay,
    uint8 already_shared_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 seg_idx;
    uint32 qual_size;
    uint32 nof_segments;
    uint8 nof_fwd_segments;
    dnx_field_qual_t associated_dnx_qual;
    dnx_field_qual_class_e qual_class;
    dnx_field_qual_attach_info_t associated_attach_info;
    int fully_shareable;
    int partially_shareable;
    int partial_sharing_offset_lsb;
    uint8 is_qual_ranged;
    dbal_fields_e qual_field_id;
    dbal_tables_e dbal_table_id;
    int partial_only_qualifier;
    /** Initializations to appease the compiler. */
    int partial_only_offset_from_lsb = 0;
    int partial_only_size_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    *found_segment_idx_p = DNX_KBP_INVALID_SEGMENT_ID;
    *msb_offset_bytes_overlay_sharing_p = 0;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));
    if (nof_fwd_segments > nof_segments)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d nof_fwd_segments %d is larger than nof_segments %d.\r\n",
                     opcode_id, nof_fwd_segments, nof_segments);
    }
    if (nof_fwd_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has %d fwd segments, maximum is %d.\r\n",
                     opcode_id, nof_fwd_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
    }

    /*
     * Ranged and non-ranged qualifiers cannot be shared at the moment, except using overlay.
     * Even with overlay, we can share non-ranged with ranges, but not vice versa.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, &qual_field_id));
    SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, qual_field_id, &is_qual_ranged));

    if (is_qual_ranged == FALSE)
    {
        for (seg_idx = 0; (seg_idx < nof_fwd_segments) && (*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID);
             seg_idx++)
        {
            if ((is_overlay == FALSE) && (already_shared_segments[seg_idx]))
            {
                continue;
            }
            partial_only_qualifier = FALSE;
            if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV4_SIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV4_SRC);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV4_DIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV4_DST);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV6_SIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV6_SIP);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "IPV6_DIP", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_HEADER_QUAL_IPV6_DIP);
            }
            else if (!sal_strncmp(ms_key_segments[seg_idx].name, "VRF", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_VRF_VALUE);
            }
            else if (!sal_strncmp
                     (ms_key_segments[seg_idx].name, "GLOBAL_IN_LIF", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_0);
            }
            else if (!sal_strncmp
                     (ms_key_segments[seg_idx].name, "GLOBAL_IN_LIF_15_00", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_0);
                partial_only_qualifier = TRUE;
                partial_only_offset_from_lsb = 0;
                partial_only_size_bits = 16;
            }
            else if (!sal_strncmp
                     (ms_key_segments[seg_idx].name, "GLOBAL_IN_LIF_21_16", sizeof(ms_key_segments[seg_idx].name)))
            {
                associated_dnx_qual =
                    DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, DNX_FIELD_STAGE_EXTERNAL, DNX_FIELD_IFWD2_QUAL_GLOB_IN_LIF_0);
                partial_only_qualifier = TRUE;
                partial_only_offset_from_lsb = 16;
                partial_only_size_bits = 6;
            }
            else
            {
                associated_dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
            }

            if (associated_dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID)
            {
                /** Sanity check: Verify the size of the segment.*/
                SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                (unit, DNX_FIELD_STAGE_EXTERNAL, associated_dnx_qual, &qual_size));
                if (partial_only_qualifier)
                {
                    if (qual_size < partial_only_size_bits)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Segment \"%.*s\" is supposed to be a part of a qualfier with size %d bits, "
                                     "but the full qualifier size is %d bits.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     partial_only_size_bits, qual_size);
                    }
                    if (qual_size == partial_only_size_bits)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Segment \"%.*s\" is supposed to be a part of a qualfier with size %d bits, "
                                     "but the full qualifier size is the same as the part.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     partial_only_size_bits);
                    }
                    if (BITS2BYTES(partial_only_size_bits) != ms_key_segments[seg_idx].nof_bytes)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment \"%.*s\" is %d bytes, while it should be %d bytes.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     ms_key_segments[seg_idx].nof_bytes, BITS2BYTES(qual_size));
                    }
                }
                else
                {
                    if (BITS2BYTES(qual_size) != ms_key_segments[seg_idx].nof_bytes)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Segment \"%.*s\" is %d bytes, while qualifier %s is %d bytes.\r\n",
                                     DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, ms_key_segments[seg_idx].name,
                                     ms_key_segments[seg_idx].nof_bytes,
                                     dnx_field_dnx_qual_text(unit, associated_dnx_qual), BITS2BYTES(qual_size));
                    }
                }
                /*
                 * Create the attach info of the associated qualifier.
                 */
                SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &associated_attach_info));
                qual_class = DNX_QUAL_CLASS(associated_dnx_qual);
                switch (qual_class)
                {
                    case DNX_FIELD_QUAL_CLASS_HEADER:
                    {
                        associated_attach_info.input_type = DNX_FIELD_INPUT_TYPE_LAYER_FWD;
                        associated_attach_info.input_arg = 0;
                        associated_attach_info.offset = 0;
                        break;
                    }
                    case DNX_FIELD_QUAL_CLASS_META:
                    {
                        associated_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA;
                        break;
                    }
                        /*
                         * coverity explanation: The possible qual_class values are HEADER and META, we can't have any other types.
                         * This can be seen from the assigned values for associated_dnx_qual. It is possible that the class of above qualifiers
                         * can be change in the future or by mistake someone added wrong class, that's why this default case was added.
                         */
                         /* coverity[dead_error_begin:FALSE]  */
                    default:
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown qual class for sharing %d. DNX qual 0x%x.\n",
                                     qual_class, qual_type);
                        break;
                    }
                }

                /*
                 * Verify that the qualifiers take the same info.
                 */
                SHR_IF_ERR_EXIT(dnx_field_key_kbp_acl_compare_qualifiers_for_sharing
                                (unit, 1, &associated_dnx_qual, &associated_attach_info, 1, &qual_type, qual_info_p,
                                 &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
                if (partial_only_qualifier == FALSE)
                {
                    if (fully_shareable)
                    {
                        /*
                         * full_sharing
                         */
                        *found_segment_idx_p = seg_idx;
                        if (is_overlay)
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                         "using overlay sharing.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                        else
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                    }
                    else if (partially_shareable && is_overlay &&
                             (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb))
                    {
                        uint32 contained_qual_size;
                        /*
                         * Try partial sharing, if we use overlay sharing and the offset from LSB is in full bytes.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                        (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, &contained_qual_size));
                        *msb_offset_bytes_overlay_sharing_p =
                            ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                            BITS2BYTES(partial_sharing_offset_lsb);
                        *found_segment_idx_p = seg_idx;
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                     "using overlay sharing. Partial sharing with offset from MSB %d\n",
                                     opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx,
                                     *msb_offset_bytes_overlay_sharing_p);
                        /** Sanity check. */
                        if ((*msb_offset_bytes_overlay_sharing_p) < 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                                         *msb_offset_bytes_overlay_sharing_p);
                        }
                    }
                }
                else if (partially_shareable && (partial_sharing_offset_lsb >= partial_only_offset_from_lsb))
                {
                    /*
                     * Only a part of the quailfier is present on the FWD segment.
                     */
                    uint32 contained_qual_size;
                    int offset_diff;
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                                    (unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, &contained_qual_size));
                    offset_diff = partial_sharing_offset_lsb - partial_only_offset_from_lsb;
                    /** Try full sharing*/
                    if ((offset_diff == 0) && (contained_qual_size == partial_only_size_bits))
                    {
                        *found_segment_idx_p = seg_idx;
                        if (is_overlay)
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                         "using overlay sharing.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                        else
                        {
                            LOG_DEBUG_EX(BSL_LOG_MODULE,
                                         "Found FWD match with opcode_id %d qual %s seg_idx %d.%s\n",
                                         opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx, EMPTY);
                        }
                    }
                    /** Try partial sharing*/
                    else if ((BYTES2BITS(BITS2BYTES(offset_diff)) == offset_diff) &&
                             (partial_only_offset_from_lsb + partial_only_size_bits >=
                              partial_sharing_offset_lsb + contained_qual_size))
                    {
                        *msb_offset_bytes_overlay_sharing_p =
                            ms_key_segments[seg_idx].nof_bytes - BITS2BYTES(contained_qual_size) -
                            BITS2BYTES(offset_diff);
                        *found_segment_idx_p = seg_idx;
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "Found FWD match with opcode_id %d qual %s seg_idx %d, "
                                     "using overlay sharing. Partial sharing with offset from MSB %d\n",
                                     opcode_id, dnx_field_dnx_qual_text(unit, qual_type), seg_idx,
                                     *msb_offset_bytes_overlay_sharing_p);
                        /** Sanity check. */
                        if ((*msb_offset_bytes_overlay_sharing_p) < 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial sharing for segment too small (offset %d).\n",
                                         *msb_offset_bytes_overlay_sharing_p);
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given qual_type exists in the master key.
*  The verification will be done over all acl and fwd segments.
* \param [in] unit            - Device ID
* \param [in] opcode_id       - The KBP OPCODE ID
* \param [in] fg_id           - The qualifier's field group
* \param [in] force_same_qual - Only share the same qualifier type.
* \param [in] nof_packed_quals - The number of qualifers in qual_type and qual_info.
* \param [in] qual_type       - qualifier type added by user.
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT elements.
* \param [in] qual_info       - attach info as was passed by user
*                               Can be more than 1, depending on nof_packed_quals.
*                               Array of DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT elements.
* \param [in] is_overlay      - If we perform sharing using the overlay feature.
* \param [in] already_shared_segments - For each segment on the master key, whether it was already found a sharing
*                                       qualifier on the field group, since we cannot share the same segment twice
*                                       for one DB (field group). Not relevant for overlay sharing.
* \param [out] found_segment_idx_p - whether the qualifier already exists in the master key.
* \param [out] msb_offset_bytes_overlay_sharing_p - For overlay sharing, extra offset in bytes to be added
*                                                   (used for partial sharing.)
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_segment_exist(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    dnx_field_group_t fg_id,
    int force_same_qual,
    int nof_packed_quals,
    dnx_field_qual_t qual_type[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_info[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int is_overlay,
    uint8 already_shared_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY],
    uint32 *found_segment_idx_p,
    int *msb_offset_bytes_overlay_sharing_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if the qualifier (future segment) exists in the ACL part of the master key .
     */
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_acl_segment_exist
                    (unit, opcode_id, fg_id, force_same_qual, nof_packed_quals, qual_type, qual_info, is_overlay,
                     already_shared_segments, found_segment_idx_p, msb_offset_bytes_overlay_sharing_p));
    /*
     * If not found in the ACL part, Check if the qualifier (future segment) exists in the FWD part of the master key.
     * We do not share packed qualifiers with FWD segments.
     */
    if ((*found_segment_idx_p == DNX_KBP_INVALID_SEGMENT_ID) && (force_same_qual == FALSE) && nof_packed_quals == 1)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_kbp_fwd_segment_exist
                        (unit, opcode_id, fg_id, qual_type[0], &qual_info[0], is_overlay,
                         already_shared_segments, found_segment_idx_p, msb_offset_bytes_overlay_sharing_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file field_group.h
 */
shr_error_e
dnx_field_group_kbp_master_key_init(
    int unit)
{
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
    uint32 nof_segments = DNX_KBP_INVALID_SEGMENT_ID;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_KBP_OPCODE_IN_USE_ITERATOR(opcode_id)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, ms_key_segments));
        if (((int) nof_segments) < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has negative number of segments %d.\r\n", opcode_id, nof_segments);
        }
        if (nof_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d has %d segments, more than the maximum %d.\r\n",
                         opcode_id, nof_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
        }
        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                        nof_fwd_segments.set(unit, opcode_id, nof_segments));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Attach context(opcode) to FG type of KBP
* \param [in] unit            - Device ID
* \param [in] fg_id           - ACL Field group id (lookup_id) to attach to opcode
* \param [in] opcode_id       - the KBP OPCODE
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p - holds the information about this context
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_opcode_attach(
    int unit,
    dnx_field_group_t fg_id,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    int qual_idx;
    int nof_quals;
    uint32 lookup_seg_idx = DNX_KBP_INVALID_SEGMENT_ID;
    uint8 key_seg_idx = DNX_KBP_INVALID_SEGMENT_ID;
    int lookup_seg_idx_without_packing;
    uint32 absolute_key_segs_idx;
    uint32 found_in_ms_key_seg_idx[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    dnx_field_key_template_t key_template;
    kbp_opcode_lookup_info_t lookup_info;
    dnx_field_group_kbp_segment_info_t fg_segment_info;
    kbp_mngr_key_segment_t ms_key_segments_existing[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    kbp_mngr_key_segment_t ms_key_segments_new[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    unsigned int payload_size;
    dbal_tables_e dbal_table_id;
    dnx_field_stage_e field_stage;
    uint32 in_ms_key_utilizes_zero_padding_seg_idx[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    uint8 segments_shared_on_master[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
    uint32 found_in_ms_key_seg_idx_overlay[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    int sharing_iter;
    uint32 master_key_nof_bytes;
    int segment_position_on_master_key_bytes[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 same_field_group_overlay_sharing_is_on_master[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 same_field_group_overlay_sharing_is_not_on_master[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 same_field_group_overlay_sharing_the_shared_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    int overlay_sharing_extra_msb_offset_for_partial[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 absolute_segment_used_by_lookup_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP];
    uint8 qual_packed_lookup_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint8 qual_packed_lookup_segment_last[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    int zero_padding_by_overlay_bytes_diff[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP] = { 0 };
    uint32 qual_sizes_bits[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 qual_sizes_bytes[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    dbal_fields_e qual_dbal_field[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint32 nof_segments_existing;
    
    int unpacked_qual_ind = 0;
    int qual_packing_supported;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    /*
     * Note that we do not have keys updated for KBP field groups.
     */

    /*
     * 1.  Each aligned qualifier will represent a segment
     * 2.  Check if we can re-use(share) the quals. (The attach-info pointers
     *     are stored with the master key)
     * 3.  Result buffer id and offset in the buffer is supplied by user.
     * 4.  KBP: Add new lookup with the List of segments to the Opcode
     *     a.  kbp_mngr_opcode_result_add()
     * 5.  KBP: Update the KBP device with the new Master Key
     *     a. kbp_mngr_opcode_master_key_segments_add()
     */

    SHR_IF_ERR_EXIT(kbp_opcode_lookup_info_t_init(unit, &lookup_info));

    for (key_seg_idx = 0; key_seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; key_seg_idx++)
    {
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &ms_key_segments_existing[key_seg_idx]));
        SHR_IF_ERR_EXIT(kbp_mngr_key_segment_t_init(unit, &ms_key_segments_new[key_seg_idx]));
    }

    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_actions_payload_size_get(unit, fg_id, &payload_size));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    /*
     * Update basic lookup info
     */
    lookup_info.dbal_table_id = dbal_table_id;
    lookup_info.result_index = attach_info_p->payload_id;
    lookup_info.result_size = payload_size;

    /*
     * Convert the payload offset from offset from LSB to offset from MSB
     */
    if (attach_info_p->payload_offset >= dnx_data_field.kbp.max_payload_size_per_opcode_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Payload offset %d exceeds payload buffer range 0-%d\r\n",
                     attach_info_p->payload_offset, dnx_data_field.kbp.max_payload_size_per_opcode_get(unit) - 1);
    }
    if (attach_info_p->payload_offset + lookup_info.result_size >
        dnx_data_field.kbp.max_payload_size_per_opcode_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Payload offset %d plus payload size %d exceeds payload buffer size %d\r\n",
                     attach_info_p->payload_offset, lookup_info.result_size,
                     dnx_data_field.kbp.max_payload_size_per_opcode_get(unit));
    }
    lookup_info.result_offset =
        dnx_data_field.kbp.max_payload_size_per_opcode_get(unit) - attach_info_p->payload_offset -
        lookup_info.result_size;
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fg_id %d opcode_id %d payload size %d%s\n", fg_id, opcode_id, lookup_info.result_size,
                 EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "fg_id %d opcode_id %d offset from lsb %d, from msb %d\n", fg_id, opcode_id,
                 attach_info_p->payload_offset, lookup_info.result_offset);
    /** Sanity check*/
    if (((unsigned int) lookup_info.result_offset >= dnx_data_field.kbp.max_payload_size_per_opcode_get(unit)) ||
        (lookup_info.result_offset % 8) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MSB payload offset calculations reached illogical value %d.\r\n",
                     lookup_info.result_offset);
    }

    /*
     * Get the List of already configured segments in the master key.
     * absolute_key_segs_idx will receive the number of segments already configured in the key.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments_existing, ms_key_segments_existing));
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fg_id %d opcode_id %d absolute_key_segs_idx %d%s\n", fg_id, opcode_id, nof_segments_existing, EMPTY);

    /*
     * Find the position in bytes of each segment we add to the master key.
     */
    for (absolute_key_segs_idx = 0; absolute_key_segs_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY;
         absolute_key_segs_idx++)
    {
        /*
         * Initialize to invalid negative value for sanity check.
         */
        segment_position_on_master_key_bytes[absolute_key_segs_idx] = -1;
    }
    master_key_nof_bytes = 0;
    for (absolute_key_segs_idx = 0; absolute_key_segs_idx < nof_segments_existing; absolute_key_segs_idx++)
    {
        /*
         * Do not count overlay segments.
         */
        if (ms_key_segments_existing[absolute_key_segs_idx].is_overlay_field)
        {
            continue;
        }
        segment_position_on_master_key_bytes[absolute_key_segs_idx] = master_key_nof_bytes;
        master_key_nof_bytes += ms_key_segments_existing[absolute_key_segs_idx].nof_bytes;
    }

    /*
     * Count the number of qualifiers in the field group.
     * We use it to invert the order of the qualifiers (write them to KBP manager from MSB to LSB) so that they match
     * their order in DBAL.
     */
    for (nof_quals = 0; (nof_quals < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[nof_quals].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); nof_quals++);

    if (nof_quals > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Too many segments per field group for fg_id %d. Maximum is %d.\r\n",
                     fg_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.is_valid.set(unit, opcode_id, TRUE));

    /*
     * Initialize arrays.
     */
    for (lookup_seg_idx = 0; lookup_seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP; lookup_seg_idx++)
    {
        found_in_ms_key_seg_idx[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        found_in_ms_key_seg_idx_overlay[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        absolute_segment_used_by_lookup_segment[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
        absolute_segment_used_by_lookup_segment[lookup_seg_idx] = DNX_KBP_INVALID_SEGMENT_ID;
    }

    /*
     * Find qual sizes and dbal fields.
     */
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                        (unit, field_stage, key_template.key_qual_map[qual_idx].qual_type, &qual_sizes_bits[qual_idx]));
        qual_sizes_bytes[qual_idx] = BITS2BYTES(qual_sizes_bits[qual_idx]);
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, DNX_FIELD_STAGE_EXTERNAL, key_template.key_qual_map[qual_idx].qual_type,
                         &qual_dbal_field[qual_idx]));
    }

    /*
     * Mark the packed qualifiers.
     * All DBAL fields other than the last (last qualifer, first DBAL field) in a packed bunch will be marked as packed.
     * We verify that the field group uses the packing flag as a sanity check.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_kbp_support_packing(unit, fg_id, &qual_packing_supported));
    {
        int prev_qual_marked_as_packed = FALSE;
        uint8 is_qual_packed;
        for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
        {
            SHR_IF_ERR_EXIT(dbal_table_field_is_packed_get(unit, dbal_table_id, qual_dbal_field[qual_idx],
                                                           &is_qual_packed));
            if (is_qual_packed && (qual_packing_supported == FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d opcode_id %d qualfier %d dbal field %d is marked "
                             "as packed, but the field group was not created with qual packing flag.\r\n",
                             fg_id, opcode_id, qual_idx, qual_dbal_field[qual_idx]);
            }
            if (is_qual_packed || prev_qual_marked_as_packed)
            {
                qual_packed_lookup_segment[lookup_seg_idx] = TRUE;
            }
            if ((is_qual_packed == FALSE) && prev_qual_marked_as_packed)
            {
                /*
                 * Coverity explanation: Fasle positive, Coverity only checks the first iteration of the loop.
                 */
                 /* coverity[dead_error_line:FALSE]  */
                qual_packed_lookup_segment_last[lookup_seg_idx] = TRUE;
            }
            prev_qual_marked_as_packed = is_qual_packed;
        }
        if (prev_qual_marked_as_packed)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Field group %d opcode_id %d last qualifer %d (0x%x) is marked as packed.\r\n",
                         fg_id, opcode_id, qual_idx, key_template.key_qual_map[nof_quals].qual_type);
        }
    }

    /*
     * Sanity check: verify that no packed qualifer is ranged.
     */
    for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
    {
        uint8 is_qual_ranged;
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, qual_dbal_field[qual_idx],
                                                           &is_qual_ranged));
            if (is_qual_ranged)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Field group %d opcode_id %d qualifier %d (0x%x) is both packed and ranged.\r\n",
                             fg_id, opcode_id, qual_idx, key_template.key_qual_map[nof_quals].qual_type);
            }
        }
    }

    /*
     * Find what segemnts can be shared.
     * Give priority to sharing the same qualifier with itself, to avoid having the same segment name twice on the
     * master key. For that reason we check the sharing twice, once with allowing only to share a qualifier with itself
     * and a second time without that limitation.
     * Technically not needed to give priority to same qualifer sharing since we added the field group ID name to the
     * segment (for the scenario where the qualifier is once a ranged qualifier and at another time isn't).
     */
    for (sharing_iter = 0; sharing_iter < 2; sharing_iter++)
    {
        for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
        {
            uint32 segment_found;
            int qual_idx_2;
            int lookup_seg_idx_2;
            int nof_packed_quals;
            dnx_field_qual_t qual_types[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
            dnx_field_qual_attach_info_t qual_infos[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
            int packed_index;
            int partial_sharing_extra_offset;

            if (found_in_ms_key_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID)
            {
                continue;
            }

            qual_types[0] = key_template.key_qual_map[qual_idx].qual_type;
            sal_memcpy(&qual_infos[0], &attach_info_p->qual_info[qual_idx], sizeof(qual_infos[0]));
            nof_packed_quals = 1;

            /*
             * Get all packed segments, if we have any.
             */
            if (qual_packed_lookup_segment[lookup_seg_idx])
            {
                for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;; qual_idx_2++, lookup_seg_idx_2--)
                {
                    packed_index = qual_idx_2 - qual_idx;
                    if (qual_idx_2 >= nof_quals || qual_packed_lookup_segment[lookup_seg_idx_2] == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "In opcode_id %d reached beyond a bunch of packed qualifers without "
                                     "reaching the last of them.\r\n", opcode_id);
                    }
                    if (packed_index >= DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "In opcode_id %d there is a bunch of packed qualifers beyond maximum %d.\r\n",
                                     opcode_id, DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT);
                    }
                    qual_types[packed_index] = key_template.key_qual_map[qual_idx_2].qual_type;
                    sal_memcpy(&qual_infos[packed_index], &attach_info_p->qual_info[qual_idx_2], sizeof(qual_infos[0]));
                    if (qual_packed_lookup_segment_last[lookup_seg_idx_2])
                    {
                        nof_packed_quals = packed_index + 1;
                        break;
                    }
                }
            }
            if (nof_packed_quals <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d, reached zero sized packed qualifers.\r\n", opcode_id);
            }
            SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_exist(unit,
                                                              opcode_id, fg_id, (sharing_iter == 0),
                                                              nof_packed_quals, qual_types, qual_infos, FALSE,
                                                              segments_shared_on_master, &segment_found,
                                                              &partial_sharing_extra_offset));
            if (segment_found != DNX_KBP_INVALID_SEGMENT_ID)
            {
                /** Sanity check.*/
                if (segment_found >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shareable segment %d found in master key, maximum is %x. "
                                 "Field group %d opcode_id %d share run number %d.\r\n",
                                 segment_found, (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1),
                                 fg_id, opcode_id, sharing_iter);
                }
                segments_shared_on_master[segment_found] = TRUE;
                for (packed_index = 0; packed_index < nof_packed_quals; packed_index++)
                {
                    found_in_ms_key_seg_idx[lookup_seg_idx - packed_index] = segment_found;
                }
            }
            /*
             * Jump over the packed segments.
             */
            qual_idx += nof_packed_quals - 1;
            lookup_seg_idx -= nof_packed_quals - 1;
        }
    }

    /*
     * Find what segemnts can be shared using overlay with already existing segments.
     * That is relevant for sharing with ranged qualifiers.
     */
    for (qual_idx = 0, lookup_seg_idx = nof_quals - 1; qual_idx < nof_quals; qual_idx++, lookup_seg_idx--)
    {
        uint32 segment_found;
        int is_shared_basic = (found_in_ms_key_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID);
        int qual_idx_2;
        int lookup_seg_idx_2;
        int nof_packed_quals;
        dnx_field_qual_t qual_types[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
        dnx_field_qual_attach_info_t qual_infos[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT];
        int packed_index;
        int partial_sharing_extra_offset;

        if (is_shared_basic)
        {
            continue;
        }

        qual_types[0] = key_template.key_qual_map[qual_idx].qual_type;
        sal_memcpy(&qual_infos[0], &attach_info_p->qual_info[qual_idx], sizeof(qual_infos[0]));
        nof_packed_quals = 1;

        /*
         * Get all packed segments, if we have any.
         */
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;; qual_idx_2++, lookup_seg_idx_2--)
            {
                packed_index = qual_idx_2 - qual_idx;
                if (qual_idx_2 >= nof_quals || qual_packed_lookup_segment[lookup_seg_idx_2] == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "In opcode_id %d reached beyond a bunch of packed qualifers without "
                                 "reaching the last of them.\r\n", opcode_id);
                }
                if (packed_index >= DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "In opcode_id %d there is a bunch of packed qualifers beyond maximum %d.\r\n",
                                 opcode_id, DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT);
                }
                qual_types[packed_index] = key_template.key_qual_map[qual_idx_2].qual_type;
                sal_memcpy(&qual_infos[packed_index], &attach_info_p->qual_info[qual_idx_2], sizeof(qual_infos[0]));
                if (qual_packed_lookup_segment_last[lookup_seg_idx_2])
                {
                    nof_packed_quals = packed_index + 1;
                    break;
                }
            }
        }
        if (nof_packed_quals <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d, reached zero sized packed qualifers.\r\n", opcode_id);
        }

        SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_exist(unit,
                                                          opcode_id, fg_id, FALSE,
                                                          nof_packed_quals, qual_types, qual_infos, TRUE,
                                                          NULL, &segment_found, &partial_sharing_extra_offset));
        if (segment_found != DNX_KBP_INVALID_SEGMENT_ID)
        {
            /** Sanity check.*/
            if (segment_found >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Shareable segment %d found in master key, maximum is %x. "
                             "Field group %d opcode_id %d share run overlay.\r\n",
                             segment_found, (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1), fg_id, opcode_id);
            }
            for (packed_index = 0; packed_index < nof_packed_quals; packed_index++)
            {
                found_in_ms_key_seg_idx_overlay[lookup_seg_idx - packed_index] = segment_found;
                overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx - packed_index] =
                    partial_sharing_extra_offset;
            }
        }
        /*
         * Jump over the packed segments.
         */
        qual_idx += nof_packed_quals - 1;
        lookup_seg_idx -= nof_packed_quals - 1;
    }

    /*
     * Find what segemnts can be shared using overlay with the newly added segments.
     * That is relevant for sharing within the same field group and for sharing with ranged qualifiers.
     */
    for (qual_idx = nof_quals - 1, lookup_seg_idx = 0; qual_idx >= 0; qual_idx--, lookup_seg_idx++)
    {
        int fully_shareable;
        int partially_shareable;
        int partial_sharing_offset_lsb;
        int qual_idx_2;
        int lookup_seg_idx_2;

        if (found_in_ms_key_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
            found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
            same_field_group_overlay_sharing_is_on_master[lookup_seg_idx])
        {
            continue;
        }

        /*
         * We do not support same field group overlay sharing for packed qualifiers,
         * as that is only useful for ranged quals.
         */
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            continue;
        }
        for (qual_idx_2 = 0, lookup_seg_idx_2 = nof_quals - 1; qual_idx_2 < nof_quals; qual_idx_2++, lookup_seg_idx_2--)
        {
            if (qual_idx_2 == qual_idx)
            {
                continue;
            }
            /*
             * Coverity explanation: Fasle positive, Coverity only checks the first iteration of the loop.
             */
             /* coverity[dead_error_line:FALSE]  */
            if (found_in_ms_key_seg_idx[lookup_seg_idx_2] != DNX_KBP_INVALID_SEGMENT_ID ||
                found_in_ms_key_seg_idx_overlay[lookup_seg_idx_2] != DNX_KBP_INVALID_SEGMENT_ID)
            {
                continue;
            }
            if (same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx_2])
            {
                continue;
            }
            /*
             * We do not support same field group overlay sharing for packed qualifiers,
             * as that is only useful for ranged quals.
             */
            if (qual_packed_lookup_segment[lookup_seg_idx_2])
            {
                continue;
            }

            SHR_IF_ERR_EXIT(dnx_field_key_kbp_acl_compare_qualifiers_for_sharing
                            (unit, 1, &key_template.key_qual_map[qual_idx_2].qual_type,
                             &(attach_info_p->qual_info[qual_idx_2]), 1, &key_template.key_qual_map[qual_idx].qual_type,
                             &(attach_info_p->qual_info[qual_idx]),
                             &fully_shareable, &partially_shareable, &partial_sharing_offset_lsb));
            if (fully_shareable)
            {
                same_field_group_overlay_sharing_is_on_master[lookup_seg_idx_2] = TRUE;
                same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx] = TRUE;
                same_field_group_overlay_sharing_the_shared_segment[lookup_seg_idx] = lookup_seg_idx_2;
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found same field group match with opcode_id %d quals %s and %s, "
                             "using overlay sharing. field group Id %d.\n",
                             opcode_id, dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx_2].qual_type), fg_id);
                break;
            }
            else if (partially_shareable &&
                     (BYTES2BITS(BITS2BYTES(partial_sharing_offset_lsb)) == partial_sharing_offset_lsb))
            {
                /*
                 * Try partial sharing.
                 */
                same_field_group_overlay_sharing_is_on_master[lookup_seg_idx_2] = TRUE;
                same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx] = TRUE;
                same_field_group_overlay_sharing_the_shared_segment[lookup_seg_idx] = lookup_seg_idx_2;
                overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx] =
                    qual_sizes_bytes[qual_idx_2] - qual_sizes_bytes[qual_idx] - BITS2BYTES(partial_sharing_offset_lsb);
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Found same field group match with opcode_id %d quals %s and %s, "
                             "using overlay sharing. Partial sharing with offset from MSB %d\n", opcode_id,
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx_2].qual_type),
                             overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx]);
                break;
            }
        }
    }

    /*
     * Find zero padding.
     */
    {
        int zero_padding_segment_size[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
        int nof_zero_padding_segments;
        int smallest_zero_padding_size;
        int smallest_zero_padding_idx;
        int biggest_qualifier_size;
        int biggest_qualifier_idx;
        int biggest_qualifier_lookup_seg_idx;
        int is_zero_padding;
        uint8 is_one_ffc_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
        uint8 nof_fwd_segments;
        dbal_enum_value_field_kbp_kbr_idx_e seg_key;
        int seg_lsb_on_key;
        int seg_size_on_one_key;

        /** Find the number of segments used by FWD.*/
        SHR_IF_ERR_EXIT(dnx_field_group_kbp_opcode_nof_fwd_segments(unit, opcode_id, &nof_fwd_segments));

        nof_zero_padding_segments = 0;
        for (key_seg_idx = 0; key_seg_idx < nof_fwd_segments; key_seg_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.segment_info.get(unit, opcode_id, key_seg_idx,
                                                                                         &fg_segment_info));
            if (fg_segment_info.fg_id == DNX_FIELD_GROUP_INVALID)
            {
                /*
                 * We have a segment used by FWD and not being filled already.
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_kbp_acl_segment_is_zero_padding
                                (unit, &ms_key_segments_existing[key_seg_idx], &is_zero_padding));
                if (is_zero_padding)
                {
                    zero_padding_segment_size[key_seg_idx] =
                        BYTES2BITS(ms_key_segments_existing[key_seg_idx].nof_bytes);
                    /** Sanity check.*/
                    if (zero_padding_segment_size[key_seg_idx] <= 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "In opcode %d, we have segemnt %d that is zero padding with "
                                     "size 0 (%d bytes).\r\n",
                                     opcode_id, key_seg_idx, ms_key_segments_existing[key_seg_idx].nof_bytes);
                    }
                    /*
                     * If the zero padded segment straddles multiple keys, only count the LSB.
                     * For now we do not support filling straddling segments at all.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_key_kbp_fwd_segment_unified_position
                                    (unit, opcode_id, key_seg_idx, &seg_key, &seg_lsb_on_key, &seg_size_on_one_key));
                    if (zero_padding_segment_size[key_seg_idx] > seg_size_on_one_key)
                    {
                        LOG_DEBUG_EX(BSL_LOG_MODULE,
                                     "In opcode_id %d, segment %d is a zero padding segment that straddles multiple "
                                     "keys, %d bits in LSB key out of %d bits in segment in total.\n",
                                     opcode_id, key_seg_idx, seg_size_on_one_key,
                                     zero_padding_segment_size[key_seg_idx]);
                        zero_padding_segment_size[key_seg_idx] = 0;
                    }
                    else
                    {
                        nof_zero_padding_segments++;
                    }
                }
            }
        }
        /*
         * Find what qualifers require only one FFC.
         */
        for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_qual_info_uses_up_to_one_ffc
                            (unit, field_stage, key_template.key_qual_map[qual_idx].qual_type,
                             &attach_info_p->qual_info[qual_idx], &is_one_ffc_qual[qual_idx]));
        }
        /*
         * Match segments by size.
         */
        while (nof_zero_padding_segments > 0)
        {
            /** Find the smallest zero padding */
            smallest_zero_padding_size = 0;
            smallest_zero_padding_idx = DNX_KBP_INVALID_SEGMENT_ID;
            for (key_seg_idx = 0; key_seg_idx < nof_fwd_segments; key_seg_idx++)
            {
                if ((zero_padding_segment_size[key_seg_idx] > 0) &&
                    ((zero_padding_segment_size[key_seg_idx] < smallest_zero_padding_size) ||
                     (smallest_zero_padding_idx == DNX_KBP_INVALID_SEGMENT_ID)))
                {
                    smallest_zero_padding_size = zero_padding_segment_size[key_seg_idx];
                    smallest_zero_padding_idx = key_seg_idx;
                }
            }
            /** Sanity check.*/
            if (smallest_zero_padding_size == 0 || smallest_zero_padding_idx == DNX_KBP_INVALID_SEGMENT_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Could not find zero padding, for zero padding %d/%d.\r\n",
                             key_seg_idx, nof_fwd_segments);
            }
            /*
             * Find the biggest fitting non shared one FFC qualifier.
             */
            biggest_qualifier_size = 0;
            biggest_qualifier_idx = DNX_KBP_INVALID_SEGMENT_ID;
            biggest_qualifier_lookup_seg_idx = DNX_KBP_INVALID_SEGMENT_ID;
            for (lookup_seg_idx = 0, qual_idx = nof_quals - 1; qual_idx >= 0; qual_idx--, lookup_seg_idx++)
            {
                int is_shared = (found_in_ms_key_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                                 found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                                 same_field_group_overlay_sharing_is_on_master[lookup_seg_idx] ||
                                 same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx]);
                
                int is_packed = qual_packed_lookup_segment[lookup_seg_idx];
                if (is_shared == FALSE && is_packed == FALSE &&
                    in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] == DNX_KBP_INVALID_SEGMENT_ID &&
                    zero_padding_segment_size[smallest_zero_padding_idx] >= qual_sizes_bits[qual_idx] &&
                    is_one_ffc_qual[qual_idx] &&
                    ((qual_sizes_bits[qual_idx] > biggest_qualifier_size) ||
                     (biggest_qualifier_idx == DNX_KBP_INVALID_SEGMENT_ID)))
                {
                    biggest_qualifier_size = qual_sizes_bits[qual_idx];
                    biggest_qualifier_idx = qual_idx;
                    biggest_qualifier_lookup_seg_idx = lookup_seg_idx;
                }
            }
            /** Check if a suitable qualifier was found.*/
            if (biggest_qualifier_size == 0 || biggest_qualifier_idx == DNX_KBP_INVALID_SEGMENT_ID)
            {
                /** Remove the smallest zero padding from the list.*/
                zero_padding_segment_size[smallest_zero_padding_idx] = 0;
                nof_zero_padding_segments--;
                continue;
            }
            else
            {
                /** Reduce the size of the smallest zero padding, to remove the bytes taken by the filling.*/
                zero_padding_by_overlay_bytes_diff[biggest_qualifier_lookup_seg_idx] =
                    BITS2BYTES(zero_padding_segment_size[smallest_zero_padding_idx]) -
                    BITS2BYTES(biggest_qualifier_size);
                zero_padding_segment_size[smallest_zero_padding_idx] -= BYTES2BITS(BITS2BYTES(biggest_qualifier_size));
                if (zero_padding_segment_size[smallest_zero_padding_idx] == 0)
                {
                    nof_zero_padding_segments--;
                }
                /** Sanity check*/
                if (zero_padding_segment_size[smallest_zero_padding_idx] < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Zero padding segment filled with a larger segment.\r\n");
                }
            }
            /** Mark the padding.*/
            in_ms_key_utilizes_zero_padding_seg_idx[biggest_qualifier_lookup_seg_idx] = smallest_zero_padding_idx;
            LOG_DEBUG_EX(BSL_LOG_MODULE, "In fg_id %d opcode_id %d, sharing qualifier %d with zero padding %d \n",
                         fg_id, opcode_id, qual_idx, smallest_zero_padding_idx);
        }
    }

    /*
     * 1. Update the list of Segments for the Master Key out of the FG qualifiers.
     * 2. Build the list of segments for the lookup update.
     * In case the segment for a given qualifier exists in the master key we will only add its
     * seg_id into the list of the segments for the lookup.
     * We will not add it into the ms_key_segments as it already exists in there.
     */
    absolute_key_segs_idx = nof_segments_existing;
    for (key_seg_idx = 0, lookup_seg_idx = 0, lookup_seg_idx_without_packing = 0, qual_idx = nof_quals - 1;
         qual_idx >= 0; qual_idx--, lookup_seg_idx++, lookup_seg_idx_without_packing++)
    {
        int is_shared_basic;
        int is_shared_overlay;
        int is_zero_padding_filling;
        int nof_quals_packing;
        int qual_idx_2;
        int lookup_seg_idx_2;
        int packed_qual_idx;
        /** Sanity check */
        if (lookup_seg_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "lookup_seg_idx %d exceeds maximum %d, even though number of qualifer "
                         "was already verified.\r\n", lookup_seg_idx, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP - 1);
        }
        is_shared_basic = (found_in_ms_key_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID);
        is_shared_overlay = (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                             same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx]);
        is_zero_padding_filling =
            (in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID);
        /** Sanity check.*/
        if (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID &&
            same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is overlay shared both within field "
                         "group and without.\r\n", qual_idx, fg_id, opcode_id);
        }

        nof_quals_packing = 0;
        if (qual_packed_lookup_segment[lookup_seg_idx])
        {
            /** Sanity check.*/
            if (qual_packed_lookup_segment_last[lookup_seg_idx] == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is not last in packing bunch "
                             "but was not skipped.\r\n", qual_idx, fg_id, opcode_id);
            }
            nof_quals_packing++;
            for (qual_idx_2 = qual_idx - 1, lookup_seg_idx_2 = lookup_seg_idx + 1;
                 qual_idx_2 >= 0; qual_idx_2--, lookup_seg_idx_2++)
            {
                /*
                 * Count the number of packed qualifiers until we reach a nin packed qualifer or one that
                 * begins a new packed bunch.
                 */
                /*
                 * Coverity explanation: Fasle positive, Coverity only checks the first iteration of the loop.
                 */
                 /* coverity[dead_error_line:FALSE]  */
                if (qual_packed_lookup_segment[lookup_seg_idx_2] &&
                    (qual_packed_lookup_segment_last[lookup_seg_idx_2] == FALSE))
                {
                    nof_quals_packing++;
                }
                else
                {
                    break;
                }
            }
        }
        /** Sanity check.*/
        if (nof_quals_packing == 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is a packed qualifer "
                         "only with itself.\r\n", qual_idx, fg_id, opcode_id);
        }
        if (nof_quals_packing > DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Opcode %d Has a packing group of size %d, maximum is %d.\r\n",
                         opcode_id, nof_quals_packing, DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT);
        }

        /*
         * Check if the segment exists. If not - add it to the Master Key.
         * the search for shared segments is done on FWD and ACL segments.
         */
        if (is_shared_basic && is_shared_overlay)
        {
            /** Sanity check*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is both shared (with segment %d) and "
                         "overlay shared (with segment %d/).\r\n", qual_idx, fg_id, opcode_id,
                         found_in_ms_key_seg_idx[lookup_seg_idx], found_in_ms_key_seg_idx_overlay[lookup_seg_idx]);
        }
        else if ((is_shared_basic || is_shared_overlay) && is_zero_padding_filling)
        {
            /** Sanity check*/
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is both shared (with segment %d/%d) and "
                         "filling zero padding with segment %d) \r\n", qual_idx, fg_id, opcode_id,
                         found_in_ms_key_seg_idx[lookup_seg_idx], found_in_ms_key_seg_idx_overlay[lookup_seg_idx],
                         in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx]);
        }
        else if ((is_zero_padding_filling) && (nof_quals_packing > 1))
        {
            
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "qualifier %d of fg %d in opcode %d is packed but still zero "
                         "padding filling.\r\n", qual_idx, fg_id, opcode_id);
        }
        else if (is_shared_basic)
        {
            /*
             * Segment already exists and will be shared - Update the Lookup Info
             */
            lookup_info.nof_segments++;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = found_in_ms_key_seg_idx[lookup_seg_idx];
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Re-use qual %s fg_id %d opcode_id %d key_seg_idx %d \n",
                         ms_key_segments_existing[lookup_info.key_segment_index[lookup_seg_idx_without_packing]].name,
                         fg_id, opcode_id, found_in_ms_key_seg_idx[lookup_seg_idx]);
        }
        else if (is_zero_padding_filling)
        {
            /*
             * An existing zero padding segment will be used.
             * The zero padding segment will be changed later.
             */
            lookup_info.nof_segments++;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] =
                in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx];
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Filling zero padding qual %s fg_id %d opcode_id %d key_seg_idx %d overlay \n",
                         ms_key_segments_existing[lookup_info.key_segment_index[lookup_seg_idx_without_packing]].name,
                         fg_id, opcode_id, found_in_ms_key_seg_idx[lookup_seg_idx]);
        }
        else if (is_shared_overlay)
        {
            /*
             * Segment already exists and will be shared - Update the Lookup Info
             */
            lookup_info.nof_segments++;
            /*
             * The master segment index will be updated later.
             */
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = DNX_KBP_INVALID_SEGMENT_ID;
            if (same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx])
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Re-use qual %s fg_id %d opcode_id %d using same field group overlay.%s\n",
                             ms_key_segments_existing[lookup_info.
                                                      key_segment_index[lookup_seg_idx_without_packing]].name, fg_id,
                             opcode_id, EMPTY);
            }
            else
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Re-use qual %s fg_id %d opcode_id %d key_seg_idx %d using overlay.\n",
                             ms_key_segments_existing[lookup_info.
                                                      key_segment_index[lookup_seg_idx_without_packing]].name, fg_id,
                             opcode_id, found_in_ms_key_seg_idx[lookup_seg_idx]);
            }

            /*
             * The overlay sharing segment will be added in the following loop.
             */
        }
        else
        {
            int segment_size_bits_total;
            /*
             * There is no such segment in the already defined Master Key, create a new Segment.
             */
            if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                             opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            }
            if (nof_quals_packing > 1)
            {
                sal_snprintf(ms_key_segments_new[key_seg_idx].name,
                             sizeof(ms_key_segments_new[key_seg_idx].name),
                             "%s:%d:%d:%d:%d:P%d",
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                             fg_id,
                             attach_info_p->qual_info[qual_idx].input_type,
                             attach_info_p->qual_info[qual_idx].input_arg, attach_info_p->qual_info[qual_idx].offset,
                             nof_quals_packing);
            }
            else
            {
                sal_snprintf(ms_key_segments_new[key_seg_idx].name,
                             sizeof(ms_key_segments_new[key_seg_idx].name),
                             "%s:%d:%d:%d:%d",
                             dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                             fg_id,
                             attach_info_p->qual_info[qual_idx].input_type,
                             attach_info_p->qual_info[qual_idx].input_arg, attach_info_p->qual_info[qual_idx].offset);
            }

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Add qual %s fg_id %d to opcode_id %d key_seg_idx %d \n",
                         ms_key_segments_new[key_seg_idx].name, fg_id, opcode_id, key_seg_idx);

            segment_size_bits_total = qual_sizes_bits[qual_idx];
            for (packed_qual_idx = 1; packed_qual_idx < nof_quals_packing; packed_qual_idx++)
            {
                segment_size_bits_total += qual_sizes_bits[qual_idx - packed_qual_idx];
            }

            /** size has to be byte align need to validate it   */
            ms_key_segments_new[key_seg_idx].nof_bytes = BITS2BYTES(segment_size_bits_total);

            /*
             * Save the information required for future hw configuration of this segment (fg_segment_info)
 */
            SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[0] = qual_idx;
            for (packed_qual_idx = 0; packed_qual_idx < nof_quals_packing; packed_qual_idx++)
            {
                fg_segment_info.qual_idx[packed_qual_idx] = qual_idx - nof_quals_packing + packed_qual_idx + 1;
            }
            /*
             * absolute_key_segs_idx represents the absolute index in the master key, considering all lookups for this
             * opcode. thus, we save the information on index absolute_key_segs_idx.
             */
            SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                            segment_info.set(unit, opcode_id, absolute_key_segs_idx, &fg_segment_info));

            /*
             * Update positon on master key.
             */
            segment_position_on_master_key_bytes[absolute_key_segs_idx] = master_key_nof_bytes;
            master_key_nof_bytes += BITS2BYTES(segment_size_bits_total);
            absolute_segment_used_by_lookup_segment[lookup_seg_idx] = absolute_key_segs_idx;

            /*
             * Update the Lookup Info
 */
            lookup_info.nof_segments++;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = absolute_key_segs_idx;

            /*
             * Increment only if new segment was created
             */
            key_seg_idx++;
            absolute_key_segs_idx++;
        }
        if (nof_quals_packing > 1)
        {
            lookup_seg_idx += (nof_quals_packing - 1);
            qual_idx -= (nof_quals_packing - 1);
        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fg_id %d to opcode_id %d nof added segments %d absolute_key_segs_idx %d.\n",
                 fg_id, opcode_id, key_seg_idx, absolute_key_segs_idx);

    /*
     * Add the overlay segments.
     */
    for (lookup_seg_idx = 0, qual_idx = nof_quals - 1, lookup_seg_idx_without_packing = 0;
         qual_idx >= 0; qual_idx--, lookup_seg_idx++, lookup_seg_idx_without_packing++)
    {
        int is_shared_overlay;
        int nof_quals_packing;
        int qual_idx_2;
        int lookup_seg_idx_2;
        int packed_qual_idx;
        /** Sanity check */
        if (lookup_seg_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "lookup_seg_idx %d exceeds maximum %d, even though number of qualifer "
                         "was already verified.\r\n", lookup_seg_idx, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP - 1);
        }
        is_shared_overlay = (found_in_ms_key_seg_idx_overlay[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID ||
                             same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx]);

        nof_quals_packing = 0;
        for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;
             qual_idx_2 >= 0; qual_idx_2--, lookup_seg_idx_2++)
        {
            if (qual_packed_lookup_segment[lookup_seg_idx_2])
            {
                nof_quals_packing++;
            }
            else
            {
                break;
            }
        }

        /*
         * If overlay shared, add that info to master key.
         */
        if (is_shared_overlay)
        {
            uint32 segment_shared_with;
            /*
             * Sanity check.
             */
            if ((lookup_info.nof_segments < lookup_seg_idx) ||
                (lookup_info.key_segment_index[lookup_seg_idx_without_packing] !=
                 ((typeof(lookup_info.key_segment_index[0])) DNX_KBP_INVALID_SEGMENT_ID)))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error updating lookup for overlay sharing.\r\n");
            }
            /*
             * Sanity check.
             */
            if (overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx] < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error negative offset for overlay partial sharing.\r\n");
            }

            /*
             * There is no such segment in the already defined Master Key, create a new overlay segment.
             */
            if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                             opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
            }

            sal_snprintf(ms_key_segments_new[key_seg_idx].name,
                         sizeof(ms_key_segments_new[key_seg_idx].name),
                         "%s:%d:%d:%d:%d",
                         dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                         fg_id,
                         attach_info_p->qual_info[qual_idx].input_type,
                         attach_info_p->qual_info[qual_idx].input_arg, attach_info_p->qual_info[qual_idx].offset);

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Add qual %s fg_id %d to opcode_id %d key_seg_idx %d \n",
                         ms_key_segments_new[key_seg_idx].name, fg_id, opcode_id, key_seg_idx);

            /** size has to be byte align need to validate it   */
            ms_key_segments_new[key_seg_idx].nof_bytes = qual_sizes_bytes[qual_idx];

            ms_key_segments_new[key_seg_idx].is_overlay_field = TRUE;
            lookup_info.key_segment_index[lookup_seg_idx_without_packing] = absolute_key_segs_idx;

            /*
             * Find the segment to share with.
             */
            if (same_field_group_overlay_sharing_is_not_on_master[lookup_seg_idx])
            {
                segment_shared_with = absolute_segment_used_by_lookup_segment
                    [same_field_group_overlay_sharing_the_shared_segment[lookup_seg_idx]];
            }
            else
            {
                segment_shared_with = found_in_ms_key_seg_idx_overlay[lookup_seg_idx];
            }
            /*
             * Sanity check
             */
            if (segment_shared_with == (typeof(segment_shared_with)) DNX_KBP_INVALID_SEGMENT_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error peforming overlay sharing.\r\n");
            }
            /*
             * Sanity check
             */
            if (segment_position_on_master_key_bytes[segment_shared_with] < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error peforming overlay sharing, "
                             "sharing with a segment with no position.\r\n");
            }

            /*
             * Set the position to share with.
             */
            ms_key_segments_new[key_seg_idx].overlay_offset_bytes =
                segment_position_on_master_key_bytes[segment_shared_with] +
                overlay_sharing_extra_msb_offset_for_partial[lookup_seg_idx];
            /*
             * Save the information required for future hw configuration of this segment (fg_segment_info)
 */
            SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[0] = qual_idx;
            for (packed_qual_idx = 0; packed_qual_idx < nof_quals_packing; packed_qual_idx++)
            {
                fg_segment_info.qual_idx[packed_qual_idx] = qual_idx - nof_quals_packing + packed_qual_idx + 1;
            }
            SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                            segment_info.set(unit, opcode_id, absolute_key_segs_idx, &fg_segment_info));
            /*
             * Increment only if new segment was created
             */
            key_seg_idx++;
            absolute_key_segs_idx++;
        }
        if (nof_quals_packing > 1)
        {
            lookup_seg_idx += (nof_quals_packing - 1);
            qual_idx -= (nof_quals_packing - 1);
        }
    }

    /*
     * Handle zero padding.
     */
    for (lookup_seg_idx = 0, qual_idx = nof_quals - 1, lookup_seg_idx_without_packing = 0;
         qual_idx >= 0; qual_idx--, lookup_seg_idx++, lookup_seg_idx_without_packing++)
    {
        uint32 zero_padding_segment_filled;
        int nof_quals_packing;
        int qual_idx_2;
        int lookup_seg_idx_2;
        char new_segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES];

        /** Sanity check */
        if (zero_padding_by_overlay_bytes_diff[lookup_seg_idx] < 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative byte size difference (%d) between qualifier and zero padding.\r\n",
                         zero_padding_by_overlay_bytes_diff[lookup_seg_idx]);
        }

        zero_padding_segment_filled = in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx];

        /** Sanity check. */
        if ((zero_padding_by_overlay_bytes_diff[lookup_seg_idx] > 0) &&
            (zero_padding_segment_filled == DNX_KBP_INVALID_SEGMENT_ID))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment marked partial sharing of zero "
                         "padding filling, but is not zero padding filling.\r\n");
        }

        nof_quals_packing = 0;
        for (qual_idx_2 = qual_idx, lookup_seg_idx_2 = lookup_seg_idx;
             qual_idx_2 >= 0; qual_idx_2--, lookup_seg_idx_2++)
        {
            if (qual_packed_lookup_segment[lookup_seg_idx_2])
            {
                nof_quals_packing++;
            }
            else
            {
                break;
            }
        }

        if (zero_padding_segment_filled != DNX_KBP_INVALID_SEGMENT_ID)
        {
            if (nof_quals_packing > 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment marked for zero padding filling, but is a packed segment.\r\n");
            }

            if (zero_padding_segment_filled != lookup_info.key_segment_index[lookup_seg_idx_without_packing])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Segment marked for overlay sharing due to not matching size of zero "
                             "padding filling, but it is nt filling the expected segment (%d instead of %d).\r\n",
                             lookup_info.key_segment_index[lookup_seg_idx_without_packing],
                             zero_padding_segment_filled);
            }

            /*
             * Create the new segment.
             */
            sal_snprintf(new_segment_name,
                         sizeof(new_segment_name),
                         "%s:%d:%d:%d:%d",
                         dnx_field_dnx_qual_text(unit, key_template.key_qual_map[qual_idx].qual_type),
                         fg_id,
                         attach_info_p->qual_info[qual_idx].input_type,
                         attach_info_p->qual_info[qual_idx].input_arg, attach_info_p->qual_info[qual_idx].offset);

            /*
             * Change the name of the zero padding segment.
             * If the new qualifier does not cover the full segment, split the zero padding segment into two.
             */
            if (zero_padding_by_overlay_bytes_diff[lookup_seg_idx] == 0)
            {
                kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
                uint32 nof_segments;
                /** Just change the name.*/
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, master_key_segments));
                sal_strncpy_s(master_key_segments[zero_padding_segment_filled].name, new_segment_name,
                              sizeof(master_key_segments[0].name));
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_set(unit, opcode_id, nof_segments, master_key_segments));
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Changed zero padding segment %d to \"%s\" fg_id %d opcode_id %d.\n",
                             zero_padding_segment_filled, new_segment_name, fg_id, opcode_id);
            }
            else
            {
                kbp_mngr_key_segment_t master_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
                uint32 nof_segments;
                kbp_mngr_key_segment_t padding_segment;
                kbp_mngr_key_segment_t new_segment;
                int lookup_seg_index2;
                /** Split into two.*/
                if (absolute_key_segs_idx >= DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY)
                {
                    SHR_ERR_EXIT(_SHR_E_RESOURCE, "Too many segments per master key for opcode %d. Maximum is %d.\r\n",
                                 opcode_id, DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY);
                }
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id, &nof_segments, master_key_segments));
                sal_memcpy(&padding_segment, &master_key_segments[zero_padding_segment_filled],
                           sizeof(padding_segment));
                sal_memcpy(&new_segment, &master_key_segments[zero_padding_segment_filled], sizeof(padding_segment));

                sal_strncpy_s(new_segment.name, new_segment_name, sizeof(master_key_segments[0].name));
                padding_segment.nof_bytes -= zero_padding_by_overlay_bytes_diff[lookup_seg_idx];
                new_segment.nof_bytes = qual_sizes_bytes[qual_idx];

                /** Insert the new segments.*/
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segment_remove
                                (unit, opcode_id, zero_padding_segment_filled));
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                                (unit, opcode_id, zero_padding_segment_filled, 1, &new_segment));
                SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                                (unit, opcode_id, zero_padding_segment_filled, 1, &padding_segment));

                /*
                 * Update the arrays with the new values.
                 */
                for (lookup_seg_index2 = 0; lookup_seg_index2 < lookup_info.nof_segments; lookup_seg_index2++)
                {
                    if (lookup_info.key_segment_index[lookup_seg_index2] > zero_padding_segment_filled)
                    {
                        lookup_info.key_segment_index[lookup_seg_index2]++;
                    }
                }
                /** The new segment is at the LSB of the zero padding (higher index) */
                lookup_info.key_segment_index[lookup_seg_idx_without_packing]++;
                for (lookup_seg_index2 = 0; lookup_seg_index2 < DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP;
                     lookup_seg_index2++)
                {
                    if (in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] != DNX_KBP_INVALID_SEGMENT_ID &&
                        in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx] > zero_padding_segment_filled)
                    {
                        in_ms_key_utilizes_zero_padding_seg_idx[lookup_seg_idx]++;
                    }
                }

                /*
                 * Update the SW state.
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_kbp_acl_split_zero_pad_update_sw_state
                                (unit, opcode_id, zero_padding_segment_filled));

                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Split zero padding segment %d to \"%s\", reamining %d bytes. opcode_id %d.\n",
                             zero_padding_segment_filled, new_segment_name,
                             zero_padding_by_overlay_bytes_diff[lookup_seg_idx], opcode_id);
                /*
                 * Update the zero paddign segment filled.
                 */
                zero_padding_segment_filled++;
            }
            /*
             * Mark the zero padding use in the SW state.
             */
            SHR_IF_ERR_EXIT(dnx_field_group_kbp_segment_info_t_init(unit, &fg_segment_info));
            fg_segment_info.fg_id = fg_id;
            fg_segment_info.qual_idx[unpacked_qual_ind] = qual_idx;
            SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                            segment_info.set(unit, opcode_id, zero_padding_segment_filled, &fg_segment_info));
        }
        if (nof_quals_packing > 1)
        {
            lookup_seg_idx += (nof_quals_packing - 1);
            qual_idx -= (nof_quals_packing - 1);
        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "fg_id %d to opcode_id %d nof added segments %d absolute_key_segs_idx %d including overlay.\n",
                 fg_id, opcode_id, key_seg_idx, absolute_key_segs_idx);

    /*
     * Update the Master Key with the newly constructed segments.
     * Corner-case: Even if no new segments were added for this fg,
     * we call this function as it just sets the sw-state on this stage.
     * Hw-configuration of the kbp will be done after the 'sync' call (see dnx_field_key_kbp_set()).
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_segments_add
                    (unit, opcode_id, DNX_KBP_INVALID_SEGMENT_ID, key_seg_idx, ms_key_segments_new));
    /*
     * Add the new Lookup to the KBP
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_add(unit, opcode_id, &lookup_info));

exit:
    SHR_FUNC_EXIT;

}
/**
* \brief
*  Attach context to FG type of TCAM
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id to attach context to
* \param [in] context_id      - Context id to attach
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p - holds the allocated key id and key part
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_context_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    dnx_field_group_t *fg_ids_p = NULL;
    uint32 *tcam_handler_ids_p = NULL;

    int fgs_count = 0;
    uint32 tcam_handler_id = 0;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    dnx_field_key_attach_info_in_t key_in_info;
    dnx_field_action_length_type_e action_length_type;
    dbal_enum_value_field_field_io_e field_io;
    int app_db_id_size;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(fg_ids_p, sizeof(*fg_ids_p) * dnx_data_field.group.nof_fgs_get(unit),
              "fg_ids_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(tcam_handler_ids_p, sizeof(*tcam_handler_ids_p) * dnx_data_field.group.nof_fgs_get(unit),
              "tcam_handler_ids_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    /** Init get params*/
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &key_in_info.field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_in_info.key_length));
    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
    sal_memcpy(key_in_info.qual_info, attach_info_p->qual_info, sizeof(key_in_info.qual_info));
    /**Get app db id out of tcam manager*/
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_id_size, &key_in_info.app_db_id));
    /*
     * We do not do 'bit-range' allocation for TCAM.
     */
    key_in_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    key_in_info.compare_id = attach_info_p->compare_id;
    SHR_IF_ERR_EXIT(dnx_field_key_attach(unit, context_id, &key_in_info, NULL, &(context_info_p->key_id)));
    /**Find the field IO, the location the FESes will read from.*/
    SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                    (unit, key_in_info.field_stage, DNX_FIELD_GROUP_TYPE_TCAM, &(context_info_p->key_id),
                     action_length_type, &field_io));
    /** Configure FES*/
    SHR_IF_ERR_EXIT(dnx_field_group_fes_config(unit,
                                               key_in_info.field_stage,
                                               fg_id, context_id,
                                               field_io, &actions_payload_info, attach_info_p->action_info));
    /*
     * Collect information on other TCAM handlers which use this same context.
     * Send this information to TCAM manager to verify that this context (PMF program)
     * does not collide, on the same bank, with another field group containing the
     * same context.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get(unit, key_in_info.field_stage, context_id, fg_ids_p, &fgs_count));
    SHR_IF_ERR_EXIT(dnx_field_group_fg_ids_to_tcam_handler_ids(unit, fg_ids_p, fgs_count, tcam_handler_ids_p));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_program_attach(unit, tcam_handler_id, tcam_handler_ids_p, fgs_count));
exit:
    SHR_FREE(fg_ids_p);
    SHR_FREE(tcam_handler_ids_p);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Returns the FG ID for the given TCAM handler ID
*
* \param [in] unit            - Device ID
* \param [in] tcam_handler_id - TCAM handler ID to return its corresponding FG ID
* \param [out] fg_id_p        - FG ID for the given TCAM handler ID
*/
static shr_error_e
dnx_field_group_tcam_handler_to_fg_id(
    int unit,
    uint32 tcam_handler_id,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_group_t fg_id;

    SHR_FUNC_INIT_VARS(unit);

    for (fg_id = 0; fg_id < dnx_data_field.group.nof_fgs_get(unit); fg_id++)
    {
        uint8 is_fg_allocated;
        dnx_field_group_type_e fg_type;

        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
        if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM || fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
        {
            /*
             * Convert FG ID to its corresponding TCAM handler ID and check if it equals the given
             * TCAM handler ID
             */
            dbal_tables_e dbal_table_id;
            uint32 fg_tcam_handler_id;
            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table_id, &fg_tcam_handler_id));
            if (fg_tcam_handler_id == tcam_handler_id)
            {
                *fg_id_p = fg_id;
                break;
            }
        }
    }
    if (fg_id == dnx_data_field.group.nof_fgs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No Corresponding FG ID found for the given TCAM handler ID %d\r\n",
                     tcam_handler_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See H file field_group.h
 */
shr_error_e
dnx_field_group_context_sharing_tcam_handlers_get(
    int unit,
    uint32 tcam_handler_id,
    uint32 tcam_handler_ids[DNX_DATA_MAX_FIELD_TCAM_NOF_TCAM_HANDLERS + 1])
{
    dnx_field_group_t *fg_ids_p = NULL;
    int fgs_count = 0;
    dnx_field_context_t context_ids[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    int context_idx;
    dnx_field_stage_e fg_stage;
    dnx_field_group_t fg_id = 0;
    uint32 nof_handlers = 0;
    uint32 fg_idx;
    uint32 *fg_tcam_handler_ids_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(fg_ids_p, sizeof(*fg_ids_p) * dnx_data_field.group.nof_fgs_get(unit),
              "fg_ids_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(fg_tcam_handler_ids_p, sizeof(*fg_tcam_handler_ids_p) * dnx_data_field.group.nof_fgs_get(unit),
              "fg_tcam_handler_ids_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Convert given TCAM handler ID to its corresponding FG ID */
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_to_fg_id(unit, tcam_handler_id, &fg_id));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &fg_stage));

    /** Loop over all converted FG ID contexts */
    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id, context_ids));
    for (context_idx = 0; context_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
         context_ids[context_idx] != DNX_FIELD_CONTEXT_ID_INVALID; context_idx++)
    {
        /*
         * Convert all context FGs to their corresponding TCAM handler IDs and add them to output param
         */
        SHR_IF_ERR_EXIT(dnx_field_group_context_fgs_get
                        (unit, fg_stage, context_ids[context_idx], fg_ids_p, &fgs_count));
        SHR_IF_ERR_EXIT(dnx_field_group_fg_ids_to_tcam_handler_ids(unit, fg_ids_p, fgs_count, fg_tcam_handler_ids_p));
        for (fg_idx = 0; fg_idx < fgs_count; fg_idx++)
        {
            if (fg_tcam_handler_ids_p[fg_idx] != DNX_FIELD_TCAM_HANDLER_INVALID)
            {
                tcam_handler_ids[nof_handlers++] = fg_tcam_handler_ids_p[fg_idx];
            }
        }
    }
    tcam_handler_ids[nof_handlers] = DNX_FIELD_TCAM_HANDLER_INVALID;

exit:
    SHR_FREE(fg_tcam_handler_ids_p);
    SHR_FREE(fg_ids_p);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Attach context to FG type of State Table
*
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id to attach context to
* \param [in] context_id      - Context id to attach
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p  - holds the allocated key id and key part
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_state_table_context_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    dnx_field_key_attach_info_in_t key_in_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Init get params*/
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &key_in_info.field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_in_info.key_length));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));

    sal_memcpy(key_in_info.qual_info, &attach_info_p->qual_info[0], sizeof(key_in_info.qual_info));

    key_in_info.fg_type = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
    /** Attach the key for the given context */
    SHR_IF_ERR_EXIT(dnx_field_key_attach(unit, context_id, &key_in_info, NULL, &context_info_p->key_id));

    SHR_IF_ERR_EXIT(dnx_field_context_state_table_attach(unit, key_in_info.field_stage, context_id));

    /*
     * Process the payload.
     * Note this can be ne based on state_table_acr_bus, but we do it based on state_table_atomic_rmw.
     */
    if (dnx_data_field.features.state_table_atomic_rmw_get(unit))
    {
        dnx_field_actions_fg_payload_info_t actions_payload_info;
        dnx_field_action_length_type_e action_length_type;
        dbal_enum_value_field_field_io_e field_io;

        SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));
        SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
        /*
         * Find the field IO, the location the FESes will read from.
         */
        SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                        (unit, key_in_info.field_stage, DNX_FIELD_GROUP_TYPE_STATE_TABLE, &(context_info_p->key_id),
                         action_length_type, &field_io));
        /** Configure FES*/
        SHR_IF_ERR_EXIT(dnx_field_group_fes_config
                        (unit, key_in_info.field_stage, fg_id, context_id, field_io, &actions_payload_info,
                         attach_info_p->action_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Attach context to FG type of EXEM
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id to attach context to
* \param [in] context_id      - Context id to attach
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p  - holds the allocated key id and key part
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_context_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    dnx_field_key_attach_info_in_t key_in_info;
    dnx_field_action_length_type_e action_length_type;
    dbal_enum_value_field_field_io_e field_io;
    int app_db_size;

    SHR_FUNC_INIT_VARS(unit);
    /** Init get params*/
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &key_in_info.field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_in_info.key_length));
    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));
    sal_memcpy(&key_in_info.qual_info[0], &attach_info_p->qual_info[0],
               DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG * sizeof(dnx_field_qual_attach_info_t));
    /**Get app db id*/
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_size, &(key_in_info.app_db_id)));
    key_in_info.fg_type = DNX_FIELD_GROUP_TYPE_EXEM;
    /*
     * We do not do 'bit-range' allocation for EXEM.
     */
    SHR_IF_ERR_EXIT(dnx_field_key_attach(unit, context_id, &key_in_info, NULL, &(context_info_p->key_id)));
    /*
     * Find the field IO, the location the FESes will read from.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                    (unit, key_in_info.field_stage, DNX_FIELD_GROUP_TYPE_EXEM, &(context_info_p->key_id),
                     action_length_type, &field_io));
    /** Configure FES*/
    SHR_IF_ERR_EXIT(dnx_field_group_fes_config
                    (unit, key_in_info.field_stage, fg_id, context_id, field_io, &actions_payload_info,
                     attach_info_p->action_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Attach context to FG type of MDB DT
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id to attach context to
* \param [in] context_id      - Context id to attach
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p  - holds the allocated key id and key part
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_mdb_dt_context_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    dnx_field_key_attach_info_in_t key_in_info;
    dnx_field_action_length_type_e action_length_type;
    dbal_enum_value_field_field_io_e field_io;
    int app_db_size;

    SHR_FUNC_INIT_VARS(unit);
    /** Init get params*/
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &key_in_info.field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_in_info.key_length));
    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, fg_id, &action_length_type));
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));
    sal_memcpy(&key_in_info.qual_info[0], &attach_info_p->qual_info[0],
               DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG * sizeof(dnx_field_qual_attach_info_t));
    /**Get app db id*/
    SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_size, &(key_in_info.app_db_id)));
    key_in_info.fg_type = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
    /*
     * We do not do 'bit-range' allocation for MDB_DT.
     */
    SHR_IF_ERR_EXIT(dnx_field_key_attach(unit, context_id, &key_in_info, NULL, &(context_info_p->key_id)));
    /*
     * Find the field IO, the location the FESes will read from.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                    (unit, key_in_info.field_stage, DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB, &(context_info_p->key_id),
                     action_length_type, &field_io));
    /** Configure FES*/
    SHR_IF_ERR_EXIT(dnx_field_group_fes_config
                    (unit, key_in_info.field_stage, fg_id, context_id, field_io, &actions_payload_info,
                     attach_info_p->action_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Load sw state and FESes corresponding to a context which has been attached to
 *   a field group of type of 'New Direct Extraction'. For that type, actions are
 *   extracted, one by one, from the key (regardless of the array of qualifiers
 *   specified for this context). Total number of bits on all actions must be smaller
 *   than the size of the assigned key.
 * \param [in] unit            -
 *   Device ID
 * \param [in] fg_id           -
 *   Field group id to which this context has been attached
 * \param [in] context_id      -
 *   Context id of context which has been attached
 * \param [in] actions_payload_info_p   -
 *   Payload info from the field group's SW state.
 * \param [in] attach_info_p   -
 *   Attach info as was passed by user/caller
 * \param [in] key_length_type   -
 *   Type of key length ('half key', 'single', 'double').
 * \param [in] key_id_p   -
 *   The key (or keys) allocated to the direct extraction.
 *   subparam of key_id_p: bit_range     -
 *   If
 *     it is not valid then this parameter is ignored and allocation
 *     is assumed per 'half key'.
 *   else
 *     type of dnx_field_bit_range_t, a structure containing
 *     information regarding bit-range allocated on specified key.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_group_de_context_entries_load(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_actions_fg_payload_info_t * actions_payload_info_p,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_key_length_type_e key_length_type,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_qual_t *dnx_quals_p;
    dnx_field_action_t *dnx_actions_p;
    dnx_field_action_attach_info_t *action_attach_info_p;
    dnx_field_action_in_group_info_t *action_group_info_p;
    unsigned int quals_sizes[sizeof(attach_info_p->dnx_quals) / sizeof(attach_info_p->dnx_quals[0])];
    unsigned int actions_sizes[sizeof(attach_info_p->dnx_actions) / sizeof(attach_info_p->dnx_actions[0])];
    unsigned int qual_index, action_index;
    unsigned int max_qual_index, max_action_index, max_action_info_index;
    unsigned int nof_quals, nof_actions;
    unsigned int num_qual_bits_used, num_action_bits_used;
    unsigned int total_qual_bits, total_action_bits;
    dnx_field_stage_e field_stage;
    unsigned int key_size_in_bits;
    unsigned int offset_on_current_qual;
    unsigned int qual_bits_in_current_action;
    unsigned int qualifier_bit_lsb_location;
    unsigned int lsb_on_key;
    dbal_enum_value_field_field_io_e field_io;
    uint32 bit_range_offset_within_key;
    unsigned int action_size_without_valid;
    dnx_field_actions_fes_info_t fes_inst_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_action_in_group_info_t actions_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int action_lsb;
    int template_shift;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialization.
     */
    bit_range_offset_within_key = 0;
    field_io = DBAL_NOF_ENUM_FIELD_IO_VALUES;
    /*
     * Verify that if we have a double key, the second key is the key ID that comes right after the key ID of the
     * first key (assuming the enum corresponds to the order of the keys).
     * This is currently redundant since 'double key' is not allowed for direct extraction. However,
     * we leave it in case some changes are introduced in the future.
     */
    if ((key_length_type == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
        && ((key_id_p->id[1] == DBAL_NOF_ENUM_FIELD_KEY_VALUES) || (key_id_p->id[1] != key_id_p->id[0] + 1)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key length type 'double' must receive two consecutive keys. keys received are %d and %d.\r\n",
                     key_id_p->id[0], key_id_p->id[1]);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    /*
     * Maximal number of elements in the arrays.
     */
    max_qual_index = sizeof(attach_info_p->dnx_quals) / sizeof(attach_info_p->dnx_quals[0]);
    max_action_index = sizeof(attach_info_p->dnx_actions) / sizeof(attach_info_p->dnx_actions[0]);
    max_action_info_index = sizeof(attach_info_p->action_info) / sizeof(attach_info_p->action_info[0]);
    /*
     * The arrays of actions and qualifiers.
     */
    dnx_quals_p = attach_info_p->dnx_quals;
    dnx_actions_p = attach_info_p->dnx_actions;
    action_attach_info_p = attach_info_p->action_info;
    action_group_info_p = actions_payload_info_p->actions_on_payload_info;

    key_size_in_bits = key_id_p->bit_range.bit_range_size;
    bit_range_offset_within_key = key_id_p->bit_range.bit_range_offset;

    /*
     * Find the sizes of the actions and qualifiers.
     * Also verify that the qualifiers are consecutive, and find the place on the key where the
     * direct extraction begins (relative to THE FIRST QUALIFIER).
     */
    if (dnx_quals_p[0] != DNX_FIELD_QUAL_TYPE_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_qualifier_info_get
                        (unit, fg_id, dnx_quals_p[0], &qualifier_bit_lsb_location, &quals_sizes[0]));
        total_qual_bits = quals_sizes[0];
        /*
         * Set the place of the first qualifier on the 'key template' (probably always '0')
         */
        lsb_on_key = qualifier_bit_lsb_location;
        for (qual_index = 1; qual_index < max_qual_index && dnx_quals_p[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID;
             qual_index++)
        {

            SHR_IF_ERR_EXIT(dnx_field_group_qualifier_info_get
                            (unit, fg_id, dnx_quals_p[qual_index], &qualifier_bit_lsb_location,
                             &quals_sizes[qual_index]));
            if (qualifier_bit_lsb_location != lsb_on_key + total_qual_bits)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Qualifier 0x%X number %d begins at bit %d on the key, the previous qualifier "
                             "0x%X number %d ended on bit %d on the key. Direct extraction must be performed "
                             "on a continuous block on the key.\r\n",
                             dnx_quals_p[qual_index], qual_index, qualifier_bit_lsb_location,
                             dnx_quals_p[qual_index - 1], qual_index - 1, lsb_on_key + total_qual_bits);
            }
            total_qual_bits += quals_sizes[qual_index];
        }
        nof_quals = qual_index;
    }
    else
    {
        lsb_on_key = 0;
        total_qual_bits = 0;
        nof_quals = 0;
    }
    if (key_id_p->bit_range.bit_range_valid)
    {
        /*
         * For 'bit-range' allocation:
         * At this point, 'total_qual_bits' must be equal to 'key_size_in_bits'.
         * Verify that.
         */
        if (total_qual_bits != key_size_in_bits)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "As calculated on this procedure, the number of bits on all qualifiers\r\n"
                         "is %d, while the input number of bits on key is %d. "
                         "The two must match.\r\n", total_qual_bits, key_size_in_bits);
        }
    }
    total_action_bits = 0;
    for (action_index = 0; action_index < max_action_index && dnx_actions_p[action_index] != DNX_FIELD_ACTION_INVALID;
         action_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_bit_size
                        (unit, field_stage, &(action_group_info_p[action_index]),
                         &actions_sizes[action_index], &action_size_without_valid));
        total_action_bits += actions_sizes[action_index];
    }
    nof_actions = action_index;
    /*
     * Sanity check: Verify that the number of bits used for actions fits inside the key.
     * Note we do not add 'lsb_on_key' to the check, because we may have half key for the second half of the key.
     */
    if (total_action_bits > key_size_in_bits)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The number of bits on all actions (%d) extend the keys size (%d).\r\n",
                     total_action_bits, key_size_in_bits);
    }
    /*
     * Verify that the number of actions does not extend the boundaries of the action info array.
     */
    if (nof_actions > max_action_info_index)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The number of action (%d) exceeds the action info array (%d).\r\n",
                     (int) nof_actions, (int) max_action_info_index);
    }
    /*
     * Verify that we have at least one action.
     */
    if ((int) nof_actions <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There must be at leat one action. nof_actions %d\r\n", (int) nof_actions);
    }

    /*
     * Initialize the FES configuration.
     */
    for (action_index = 0; action_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_action_in_group_info_t_init(unit, &(actions_info[action_index])));
    }
    action_lsb = lsb_on_key + bit_range_offset_within_key;
    /*
     * Now loop over each action and add a FES for it.
     */
    num_qual_bits_used = 0;
    num_action_bits_used = 0;
    offset_on_current_qual = 0;
    qual_index = 0;
    for (action_index = 0; action_index < nof_actions; action_index++)
    {
        qual_bits_in_current_action = 0;

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Handling of action at index (%d) on actions array: (class %s, stage %s, type %s)\r\n",
                     action_index, dnx_field_action_class_text(DNX_ACTION_CLASS(dnx_quals_p[action_index])),
                     dnx_field_stage_e_get_name(DNX_ACTION_STAGE(dnx_quals_p[action_index])),
                     dnx_field_dnx_action_text(unit, dnx_quals_p[action_index]));
        LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Action's size %d bits. %s%s%s\r\n",
                     actions_sizes[action_index], EMPTY, EMPTY, EMPTY);
        /*
         * Loop over the qualifiers to get the input for the action.
         */
        while (qual_bits_in_current_action < actions_sizes[action_index])
        {
            /*
             * Sanity check.
             */
            if (qual_index >= nof_quals)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "The Direct extraction qualifier-action matching process\r\n"
                             "extended its boundaries. qual_index (%d) is larger than nof_quals (%d)\r\n",
                             (int) qual_index, (int) nof_quals);
            }
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Handling of qualifier at index (%d) on qualifier array: (class %s, stage %s, ID %s)\r\n",
                         action_index, dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_quals_p[action_index])),
                         dnx_field_stage_e_get_name(DNX_QUAL_STAGE(dnx_quals_p[action_index])),
                         dnx_field_dnx_qual_text(unit, dnx_quals_p[action_index]));
            LOG_DEBUG_EX(BSL_LOG_MODULE, "==> Qualifiers's size is %d bits, starting bit is %d. %s%s\r\n",
                         quals_sizes[qual_index], offset_on_current_qual, EMPTY, EMPTY);
            /*
             * Check whether the number of keys remaining in the qualifier is enough for the action.
             */
            if ((actions_sizes[action_index] - qual_bits_in_current_action) <=
                (quals_sizes[qual_index] - offset_on_current_qual))
            {
                /*
                 * Set the FES location
                 * Note that we locate the FES on the correct location on the key (and not on the key template).
                 */
                action_lsb = lsb_on_key + num_action_bits_used + bit_range_offset_within_key;

                /*
                 * Count the bits that were read, and mark qual_bits_in_current_action so that the loop will exit.
                 */
                offset_on_current_qual += actions_sizes[action_index] - qual_bits_in_current_action;
                num_qual_bits_used += actions_sizes[action_index] - qual_bits_in_current_action;
                qual_bits_in_current_action = actions_sizes[action_index];
                num_action_bits_used += actions_sizes[action_index];
            }
            else
            {
                /*
                 * Count the remaining bits, and mark the offset in current qual so that it will continue
                 * to the next qual.
                 */
                qual_bits_in_current_action += quals_sizes[qual_index] - offset_on_current_qual;
                num_qual_bits_used += quals_sizes[qual_index] - offset_on_current_qual;
                offset_on_current_qual = quals_sizes[qual_index];
            }
            /*
             * If there are no bits left in the qualifier, continue to the next qaulifire.
             */
            if (quals_sizes[qual_index] == offset_on_current_qual)
            {
                offset_on_current_qual = 0;
                qual_index++;
            }
            else
            {
                /*
                 * Sanity check.
                 */
                if (quals_sizes[qual_index] < offset_on_current_qual)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "The Direct extraction qualifier-action matching process read more bits "
                                 "from qualifier then there are bits in the qualifier.\r\n");
                }
            }
        }
        /*
         * Sanity check.
         */
        if (qual_bits_in_current_action < actions_sizes[action_index])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "The Direct extraction qualifier-action matching process exited loop without matching.\r\n");
        }
        /*
         * Add to action_info
         */
        actions_info[action_index].dnx_action = dnx_actions_p[action_index];
        actions_info[action_index].dont_use_valid_bit = action_group_info_p[action_index].dont_use_valid_bit;
        actions_info[action_index].lsb = action_lsb;
    }
    /*
     * Sanity check: Check that all of the bits were used.
     */
    if (num_qual_bits_used != total_qual_bits)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of qualifier bits used (%d) is not the total number of qualifier bits (%d).\r\n",
                     num_qual_bits_used, total_qual_bits);
    }
    if (num_action_bits_used > total_action_bits)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of action bits used (%d) is not the total number of action bits (%d).\r\n",
                     num_action_bits_used, total_action_bits);
    }
    /*
     * Sanity check: Check that all of the qualifiers were used.
     */
    if (qual_index != nof_quals)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Only %d out of %d qualifiers were used.\r\n", qual_index, nof_quals);
    }

    /*
     * Find the field io
     * Note that direct extraction doesn't use action type length.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                    (unit, field_stage, DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION, key_id_p,
                     DNX_FIELD_ACTION_LENGTH_TYPE_INVALID, &field_io));

    /*
     * Find any shift the field group may have.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &template_shift));

    /*
     * Allocate and write the EFESes.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_calc_feses
                    (unit, field_stage, field_io, actions_info, template_shift, action_attach_info_p, fes_inst_info));
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_set(unit, field_stage, fg_id, context_id, TRUE, FALSE, fes_inst_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Attach context to FG type of Direct Extraction
* \param [in] unit            - Device ID
* \param [in] flags        - context attach flags (see dnx_field_group_context_attach_flags_e)
* \param [in] fg_id           - Field group id to attach context to
* \param [in] context_id      - Context id to attach
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p  - holds the allocated key id and key part
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_de_context_attach(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    dnx_field_key_attach_info_in_t key_in_info;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    dnx_field_group_type_e fg_type;
    int fem_used;
    int msb_32_aligned;
    int payload_spread;
    int fem_key_select_granularity;
    dnx_field_key_de_bit_allocation_info_t bit_range_allocation_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_key_attach_info_in_t_init(unit, &key_in_info));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &key_in_info.field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &key_in_info.key_length));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    sal_memcpy(key_in_info.qual_info, attach_info_p->qual_info, sizeof(key_in_info.qual_info));
    fem_key_select_granularity =
        dnx_data_field.stage.stage_info_get(unit, key_in_info.field_stage)->fem_key_select_resolution_in_bits;
    /*
     * Save the attach info for this field group in this context
     */
    sal_memcpy(context_info_p->qual_attach_info, attach_info_p->qual_info, sizeof(context_info_p->qual_attach_info));
    /*
     * Initialize the bit allocation info.
     */
    sal_memset(&bit_range_allocation_info, 0x0, sizeof(bit_range_allocation_info));
    bit_range_allocation_info.bit_range_valid = TRUE;
    /*
     * Get field group type
     */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    key_in_info.fg_type = fg_type;
    {
        /*
         * Find out whether there are any FEMs which have been added to this field group
         * and load indication for dnx_field_key_attach()
         */
        dnx_field_fem_id_t fem_id;
        dnx_field_fem_program_t fem_program;
        fem_id = DNX_FIELD_FEM_ID_INVALID;
        fem_used = FALSE;
        SHR_IF_ERR_EXIT(dnx_field_fem_get_next_fg_owner(unit, fg_id, &fem_id, &fem_program));
        if (fem_id != DNX_FIELD_FEM_ID_INVALID)
        {
            /*
             * Enter if at least one FEM was added to this field group.
             */
            bit_range_allocation_info.bit_range_align_lsb = TRUE;
            /*
             * At this point, we set the alignment, hard coded, to zero. If we want it to
             * be flexible then, at this point, we need to go through all FEMs which have
             * been added to this field group, make sure they all differ by a multiple
             * of '16' and set the alignment bit to the smallest selected.
             */
            bit_range_allocation_info.bit_range_lsb_aligned_offset = 0;
            fem_used = TRUE;
            /** Sanity check*/
            if (fem_key_select_granularity <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "field group %d has FEMs added to it, the the stage's (%s) "
                             "FEM key select granularity is %d. \r\n",
                             fg_id, dnx_field_stage_text(unit, key_in_info.field_stage), fem_key_select_granularity);
            }
            bit_range_allocation_info.bit_range_align_lsb_step = fem_key_select_granularity;
        }
    }
    if (flags & DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN)
    {
        int min_payload_offset;
        int max_payload_offset;
        msb_32_aligned = TRUE;
        /*
         * Get the size of the field group
         */
        SHR_IF_ERR_EXIT(dnx_field_group_payload_template_min_max_get(unit, fg_id,
                                                                     &min_payload_offset, &max_payload_offset));
        payload_spread = max_payload_offset - min_payload_offset + 1;
        if (fem_used && ((payload_spread % fem_key_select_granularity) != 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "field group %d is a direct extraction field group with size %d and FEMS. "
                         "Only field groups with a size divisible by %d can use both "
                         "the 32_RESULT_MSB_ALIGN flag and FEM.\r\n",
                         fg_id, payload_spread, fem_key_select_granularity);
        }
        if (fem_used && (payload_spread < DNX_FIELD_KEY_SELECT_BIT_GRANULARITY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "field group %d is a direct extraction field group with size %d and FEMS. "
                         "Only field groups with a size at least %d can use FEMs and the 32_RESULT_MSB_ALIGN flag. "
                         "Context ID is %d.\r\n",
                         fg_id, payload_spread, DNX_FIELD_KEY_SELECT_BIT_GRANULARITY, context_id);
        }
        bit_range_allocation_info.bit_range_align_msb = TRUE;
        bit_range_allocation_info.bit_range_align_msb_step = DNX_FIELD_KEY_SELECT_BIT_GRANULARITY;
    }
    else
    {
        msb_32_aligned = FALSE;
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Direct extraction attach fg_id %d context_id %d. Align 16 bit LSB:%s. Align 32 bits MSB:%s\n",
                 fg_id, context_id, (fem_used) ? ("yes") : ("no"), (msb_32_aligned) ? ("yes") : ("no"));

    SHR_IF_ERR_EXIT(dnx_field_key_attach(unit, context_id, &key_in_info, &bit_range_allocation_info,
                                         &(context_info_p->key_id)));
    
    if (key_in_info.key_length == DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Key length type 'double' not yet supported for direct extraction.\r\n");
    }
    {
        /*
         * Update SW state with new context ID attached and key that was allocated for it
         * This is necessary here, for the 'new' direct extraction, since it involves adding 'entries' to
         * an fg which is attached to this context.
         */
        {
            context_info_p->context_info_valid = TRUE;
            SHR_IF_ERR_EXIT(dnx_field_group_context_info_set(unit, fg_id, context_id, context_info_p));
        }
        SHR_IF_ERR_EXIT(dnx_field_group_de_context_entries_load
                        (unit, fg_id, context_id, &actions_payload_info, attach_info_p, key_in_info.key_length,
                         &context_info_p->key_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Attach context to FG type of Const.
* \param [in] unit            - Device ID
* \param [in] fg_id           - Field group id to attach context to
* \param [in] context_id      - Context id to attach
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p  - holds the allocated key id and key part
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_const_context_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p)
{
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    dnx_field_stage_e field_stage;
    dbal_enum_value_field_field_io_e field_io;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    SHR_IF_ERR_EXIT(dnx_field_group_find_field_io
                    (unit, field_stage, DNX_FIELD_GROUP_TYPE_CONST, NULL, DNX_FIELD_ACTION_LENGTH_TYPE_INVALID,
                     &field_io));
    /** Configure FES*/
    SHR_IF_ERR_EXIT(dnx_field_group_fes_config
                    (unit, field_stage, fg_id, context_id, field_io, &actions_payload_info,
                     attach_info_p->action_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify parameters of dnx_field_group_context_attach()
* \param [in] unit         - Device id
* \param [in] flags        - Flags
* \param [in] fg_id        - Field Group id to attach programs to
* \param [in] context_id        - PMF PGM id to attach to FG
* \param [in] attach_info_p  -Pointer to attach info look at (dnx_field_group_attach_info_t)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_context_attach_verify(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_info_p)
{
    unsigned int fg_found;
    uint8 is_allocated = FALSE;
    dnx_field_stage_e stage = DNX_FIELD_STAGE_INVALID;
    dnx_field_group_type_e fg_type;
    uint32 action_ndx;
    int invalidated_fem_idx;
    int invalidated_fem_idx_2;
    dnx_field_fem_program_t fem_program;
    unsigned int nof_quals_fg;
    unsigned int nof_quals;
    dnx_field_qual_t dnx_quals_fg[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    unsigned int qual_ndx;
    unsigned int nof_actions_fg;
    unsigned int nof_actions;
    dnx_field_actions_fg_payload_info_t actions_payload_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(attach_info_p, _SHR_E_PARAM, "attach_info_p");

    /**Check whether FG was added*/
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Trying to attach context id %d to field group %d which has not yet been added\r\n",
                     context_id, fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    /** Check whether context was created*/
    is_allocated = FALSE;
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &stage));

    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, stage, context_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Context id %d needs to be created first on stage %s. \n", context_id,
                     dnx_field_stage_text(unit, stage));
    }

    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &fg_found));
    if (fg_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FG id %d was already attached to CTX id %d on stage %s. \n", fg_id, context_id,
                     dnx_field_stage_text(unit, stage));
    }

    /*
     * Verify that 32 bit msb alignment flag is only for direct extraction.
     */
    if ((flags & DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN)
        && (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FG id %d is not a direct extraction field group (is of type %d), and "
                     "32_RESULT_MSB_ALIGN flag can only be used for direct extraction. context_id %d.\n",
                     fg_id, fg_type, context_id);
    }

    /*
     * Verify that the same qualifiers appear in attach info as in the field group, and in the same order.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals_fg, &nof_quals_fg));
    for (nof_quals = 0; (nof_quals < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (attach_info_p->dnx_quals[nof_quals] != DNX_FIELD_QUAL_TYPE_INVALID); nof_quals++);
    if (nof_quals != nof_quals_fg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of qualifiers attached (%d) does not match number of qualifiers "
                     "in the field group (%d). Field group is %d, context is %d.\r\n",
                     nof_quals, nof_quals_fg, fg_id, context_id);
    }
    for (qual_ndx = 0; qual_ndx < nof_quals; qual_ndx++)
    {
        if (dnx_quals_fg[qual_ndx] != attach_info_p->dnx_quals[qual_ndx])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Attached qualifiers must be the same (and in the same order) as in the "
                         "field group. Qualifier number %d is different (\"%s\" in attach, \"%s\" in field group). "
                         "Field group is %d, context is %d.\n",
                         qual_ndx, dnx_field_dnx_qual_text(unit, attach_info_p->dnx_quals[qual_ndx]),
                         dnx_field_dnx_qual_text(unit, dnx_quals_fg[qual_ndx]), fg_id, context_id);
        }
    }
    /*
     * Verify that the same actions appear in attach info as in the field group, and in the same order.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    for (nof_actions_fg = 0;
         ((nof_actions_fg < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
          && (actions_payload_info.actions_on_payload_info[nof_actions_fg].dnx_action !=
              DNX_FIELD_ACTION_INVALID)); nof_actions_fg++);
    for (nof_actions = 0;
         nof_actions < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && attach_info_p->dnx_actions[nof_actions] != DNX_FIELD_ACTION_INVALID; nof_actions++);
    if (nof_actions != nof_actions_fg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions attached (%d) does not match number of actions "
                     "in the field group (%d). Field group is %d, context is %d.\r\n",
                     nof_actions, nof_actions_fg, fg_id, context_id);
    }
    for (action_ndx = 0; action_ndx < nof_actions; action_ndx++)
    {
        if (actions_payload_info.actions_on_payload_info[action_ndx].dnx_action !=
            attach_info_p->dnx_actions[action_ndx])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Attached actions must be the same (and in the same order) as in the "
                         "field group. Action number %d is different (\"%s\" in attach, \"%s\" in field group). "
                         "Field group is %d, context is %d.\n",
                         action_ndx, dnx_field_dnx_action_text(unit, attach_info_p->dnx_actions[action_ndx]),
                         dnx_field_dnx_action_text(unit,
                                                   actions_payload_info.actions_on_payload_info[action_ndx].dnx_action),
                         fg_id, context_id);
        }
    }

    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
         && attach_info_p->dnx_actions[action_ndx] != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        int action_is_void;
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_is_void
                        (unit, stage, attach_info_p->dnx_actions[action_ndx], &action_is_void));
        /** Verify that VOID actions are created with don't care priority. */
        if (action_is_void && (attach_info_p->action_info[action_ndx].priority != BCM_FIELD_ACTION_DONT_CARE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Void actions must be created with priority of don't care!\n");
        }

        /** Verify that actions are using valid bit, if the polarity is different than the default one. */
        if ((actions_payload_info.actions_on_payload_info[action_ndx].dont_use_valid_bit == TRUE) &&
            (attach_info_p->action_info[action_ndx].valid_bit_polarity != DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "DNX action 0x%08X don't use valid bit (dont_use_valid_bit == TRUE), but still have polarity (%d), which is "
                         "different than the default value (%d).\r\n", attach_info_p->dnx_actions[action_ndx],
                         attach_info_p->action_info[action_ndx].valid_bit_polarity,
                         DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE);
        }

        /** Verify that polarity of the actions is in range. */
        if (attach_info_p->action_info[action_ndx].valid_bit_polarity != DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE
            && attach_info_p->action_info[action_ndx].valid_bit_polarity !=
            DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Given value for valid_bit_polarity %d is not valid. Valid bit polarity must be 0 or 1!\n",
                         attach_info_p->action_info[action_ndx].valid_bit_polarity);
        }

        /** Verify that action with polarity '0' can be used only for DE. */
        if (attach_info_p->action_info[action_ndx].valid_bit_polarity == DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO
            && fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Valid bit polarity 0 is supported only for Direct Extraction field groups!\n");
        }

        /** Verify that action invalidated actions have default polarity. */
        if (attach_info_p->action_info[action_ndx].valid_bit_polarity != DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE
            && attach_info_p->action_info[action_ndx].priority == BCM_FIELD_ACTION_INVALIDATE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Action number %d is invalidated by priority, "
                         "so valid bit polarity is meaningless, but its value isn't the default '%d'.\n",
                         action_ndx, DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE);
        }
        if (attach_info_p->action_info[action_ndx].priority == BCM_FIELD_ACTION_INVALIDATE)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Action number %d in field group %c context id %d has priority "
                         "BCM_FIELD_ACTION_INVALIDATE.%s\n", action_ndx, fg_id, context_id, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_key_qual_attach_verify
                    (unit, stage, context_id, fg_id, attach_info_p->dnx_quals, attach_info_p->qual_info));

    /*
     * Verify that a KBP field group is attached before device lock.
     * Verify that external (KBP) field group has offset that is byte aligned
     */
    if (fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "External TCAM field groups cannot be attached after device lock.\n");
        }
        if ((attach_info_p->payload_offset % SAL_UINT8_NOF_BITS) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "For external TCAM, payload ID (%d) must be byte aligned.\n",
                         attach_info_p->payload_offset);
        }
    }

    /*
     * Verify that no invalidated FEM appears twice.
     */
    for (invalidated_fem_idx = 0; invalidated_fem_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID &&
         attach_info_p->fems_invalidate[invalidated_fem_idx] != DNX_FIELD_FEM_ID_INVALID; invalidated_fem_idx++)
    {
        for (invalidated_fem_idx_2 = 0; invalidated_fem_idx_2 < invalidated_fem_idx; invalidated_fem_idx_2++)
        {
            if (attach_info_p->fems_invalidate[invalidated_fem_idx] ==
                attach_info_p->fems_invalidate[invalidated_fem_idx_2])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalidated FEM ID %d appears twiche (indices %d and %d). "
                             "fg_id %d, context %d.\n",
                             attach_info_p->fems_invalidate[invalidated_fem_idx],
                             invalidated_fem_idx_2, invalidated_fem_idx, fg_id, context_id);
            }
        }
    }
    /*
     * Verify that all invalidated FEM IDs are added to the field group.
     */
    for (invalidated_fem_idx = 0; invalidated_fem_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID &&
         attach_info_p->fems_invalidate[invalidated_fem_idx] != DNX_FIELD_FEM_ID_INVALID; invalidated_fem_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner
                        (unit, attach_info_p->fems_invalidate[invalidated_fem_idx], fg_id, &fem_program));
        if (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalidated FEM ID %d (index %d) wasn't added to field group %d. "
                         "context %d.\n",
                         attach_info_p->fems_invalidate[invalidated_fem_idx], invalidated_fem_idx, fg_id, context_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This is a FEM-related utility used to update, in HW, fem context: the 'fem_key_select'
 *   and 'fem_program' fields.
 *   Update, correspondingly, using input 'FEM id' and 'context id', the following fields
 *   in HW table: 'fem_key_select' and 'fem_program'
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator of 'FEM id' for which (together with 'context_id') HW table is to be updated.
 * \param [in] context_id -
 *   Context ID for which HW table is to be updated.
 * \param [in] fem_key_select -
 *   Value to uspdate the 'fem_key_select' field. Caller is responsible for the validity
 *   of this input. This value indicates whether this context is active or inactive.
 * \param [in] fem_program -
 *   Value to uspdate the 'fem_program' field. Caller is responsible for the validity
 *   of this input.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Output is updated HW table
 *    * Note that, before HW update, we make sure FEM context is not occupied for this fem_id.
 *    * Note that HW update is read-modify-write so that only the specified fields are updated.
 * \see
 *   * dnx_field_group_fems_context_attach
 */
static shr_error_e
dnx_field_group_fem_key_select_update(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_pmf_a_fem_key_select_e fem_key_select,
    dnx_field_fem_program_t fem_program)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Make sure FEM context is not occupied for this fem_id.
     * Note that a pair (fem_id, context_id) is considered 'not occupied' if 'fem_key_select'
     * field is set to an illegal value.
     */
    DNX_FIELD_FEM_CONTEXT_NOT_OCCUPIED_VERIFY(unit, fem_id, context_id);
    /*
     * Update FEM context for this fem_id with new info (ONLY 'fem_program' and 'fem_context_select' -
     * Use read-modify-write).
     */
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
    dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_program = fem_program;
    dnx_field_pmf_fem_program_entry.pmf_fem_program.fem_key_select = fem_key_select;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_set(unit, &dnx_field_pmf_fem_program_entry));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This is a FEM-related utility used to deduce and update, in HW, the 'replace_lsb_select'
 *   field.
 *   For given 'field group id' (of TCAM type) and 'context id', find the corresponding IO.
 *   Use this IO to deduce the corresponding 'replace_lsb_select'.
 *   Update, correspondingly, using input 'FEM id' and 'context id', the following fields
 *   in HW table: 'replace_lsb_flag' and 'replace_lsb_select'
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fg_id -
 *   Identifier of a field group to use, together with 'context_id', to deduce the IO
 *   corresponding to the latter.
 * \param [in] context_id_for_io_source -
 *   Context ID from which the IO is calculated. In most cases, this is the same as
 *   'context_id' below but, in some cases, it is different (For example, in case it
 *   is a 'parent' context, on IPMF1, which 'injects' its IO to a 'sibling, on IPMF2.)
 * \param [in] context_id -
 *   Context ID for which the IO is required (to use to get 'replace_lsb_select')
 * \param [in] fem_id -
 *    Indicator of 'FEM id' for which (together with 'context_id') HW table is to be updated.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Output is updated HW table
 *    * Note that, before HW update, we make sure FEM context is not occupied for this fem_id.
 *    * Note that HW update is read-modify-write so that only the specified fields are updated.
 * \see
 *   * dnx_field_group_fems_context_attach
 */
static shr_error_e
dnx_field_group_fems_replace_lsb_select_update(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id_for_io_source,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fem_id)
{
    dbal_enum_value_field_field_fem_replace_lsb_select_e replace_lsb_select;

    SHR_FUNC_INIT_VARS(unit);

    {
        dbal_enum_value_field_field_io_e field_io;

        SHR_IF_ERR_EXIT(dnx_field_group_field_io_get(unit, fg_id, context_id_for_io_source, &field_io));
        /*
         * Convert IO to the proper encoding of 'replace lookup'. See FIELD_FEM_REPLACE_LSB_SELECT
         * on table FIELD_PMF_A_KEY_SELECT_N_FEM_PROG.
         */
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "'Field group' is %d. Use 'IO' (%s) to get 'replace' encoding for context %d. %s\r\n",
                     fg_id, dnx_field_io_text(field_io), context_id_for_io_source, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_actions_io_to_replace_select(unit, field_io, &replace_lsb_select));
    }
    {
        /*
         * Make sure FEM context is not occupied for this fem_id.
         * Note that a pair (fem_id, context_id) is considered 'not occupied' if 'fem_key_select'
         * field is set to an illegal value.
         */
        DNX_FIELD_FEM_CONTEXT_NOT_OCCUPIED_VERIFY(unit, fem_id, context_id);
    }
    {
        dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

        /*
         * Update FEM context for this fem_id with new info ('replace_lsb_flag' and 'replace_lsb_select')
         *   dbal_enum_value_field_field_fem_replace_lsb_flag_e replace_lsb_flag;
         *   dbal_enum_value_field_field_fem_replace_lsb_select_e replace_lsb_select;
         */
        dnx_field_pmf_fem_program_entry.fem_id = fem_id;
        dnx_field_pmf_fem_program_entry.context_id = context_id;
        /*
         * Do a 'read-modify-write' and update only the 'replace-related' fields. Other fields
         * ('key select', 'fem program') will be updated on the standard handling below.
         */
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
        dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_flag =
            DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_ACTIVE;
        dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_select = replace_lsb_select;
        SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_set(unit, &dnx_field_pmf_fem_program_entry));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This is a FEM-related utility used to clear (make 'invalid'), by HW update, the
 *   'replace_lsb_select' field.
 *   Update, correspondingly, using input 'FEM id' and 'context id', the following field
 *   in HW table: 'replace_lsb_flag' (also update 'replace_lsb_select' to 'default'
 *   value, although this is of no operational value)
 *   Updated HW table is: IPPC_PMF_FEM_PROGRAM
 *   Updated DBAL table is: FIELD_PMF_A_KEY_SELECT_N_FEM_PROG
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] context_id -
 *   Context ID to update entry of.
 * \param [in] fem_id -
 *    Indicator of 'FEM id' for which (together with 'context_id') HW table is to be updated.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Output is updated HW table
 *    * Note that, before HW update, we make sure FEM context is not occupied for this fem_id.
 *    * Note that HW update is read-modify-write so that only the specified fields are updated.
 * \see
 *   * dnx_field_group_fems_context_detach
 */
static shr_error_e
dnx_field_group_fems_replace_lsb_select_clear(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fem_id)
{
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update FEM context for this fem_id with 'clear' (default) info ('replace_lsb_flag' and 'replace_lsb_select')
     */
    dnx_field_pmf_fem_program_entry.fem_id = fem_id;
    dnx_field_pmf_fem_program_entry.context_id = context_id;
    /*
     * Do a 'read-modify-write' and update only the 'replace-related' fields. Other fields
     * ('key select', 'fem program') may be updated somewhere else.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get(unit, &dnx_field_pmf_fem_program_entry));
    dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_flag =
        DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_FLAG_REPLACE_LSB_IS_NOT_ACTIVE;
    dnx_field_pmf_fem_program_entry.pmf_fem_program.replace_lsb_select =
        DBAL_ENUM_FVAL_FIELD_FEM_REPLACE_LSB_SELECT_TCAM_0;
    SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_set(unit, &dnx_field_pmf_fem_program_entry));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Check whether specified Field Group resides on some FEMs and, in that case,
 *   update specified context with FEM info.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fg_id -
 *   Identifier of a field group to search for on the list of owners of all
 *   'fem_id's.
 * \param [in] context_id -
 *   context ID which is currently being attached to specified 'fg_id' and which
 *   needs to be updated with FEM info (if specified Field Group resides on any
 *   FEMs)
 * \param [in] fems_invalidate -
 *   An array (of up to DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID elements) that includes FEM IDs that belong to the
 *   field group, but should not be added to the context.
 * \param [in] attach_info_p -
 *   Pointer to structure of type 'dnx_field_attach_context_info_t' containing information
 *   regarding allocated key.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
  *    * This procedure assumes its input has been verified by the caller.
 *    * Specifically, 'input_offset' (in SWSTATE) is a multiple of 'fem key select' resolution.
 *      Below, we check that it fits (together with a 32-bits 'chunk') into the input block size.
 *    * Also, if FG type is 'direct extraction' then key is limited to single key, not double key.
 *    * This procedure assumes that 'context_info' has been loaded into SWSTATE. \see dnx_field_group_context_info_set
 *      Also \see dnx_field_group_de_context_attach
 * \see
 *    * dnx_field_group_fems_context_detach()
 *    * dnx_field_fem_is_fg_owner()
 *    * dnx_field_group_fes_config()
 */
static shr_error_e
dnx_field_group_fems_context_attach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fem_id_t fems_invalidate[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID],
    dnx_field_attach_context_info_t * context_attach_info_p)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;
    dnx_field_fem_id_t fem_id;
    dnx_field_fem_program_t fem_program;
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);

    if (context_attach_info_p->context_info_valid != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "'context_attach_info' is marked as invalid (%d). This indicates no 'attach' was made. Illegal. Quit. \n",
                     context_attach_info_p->context_info_valid);
    }
    /*
     * Get field group type
     */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /*
     * Load stage of 'fg_id' into 'field_stage' to be used both within the handling of the
     * 'second_fg_id' and for the case of cascading (I.e., 'context_id', on IPMF1, is
     * 'parent' to many 'sibling' contexts on IPMF2).
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    /*
     * Go through all 'fem id's and update 'fem context' with 'fem program' and 'fem key select'
     */
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        uint8 actions_size;
        uint8 actions_lsb;
        dbal_enum_value_field_field_pmf_a_fem_key_select_e action_input_select;
        dnx_field_group_t second_fg_id;
        int fem_id_invalidate_index;
        int fem_is_invalidated;

        /*
         * Filter out any FEM that should be invalidated.
         */
        fem_is_invalidated = FALSE;
        for (fem_id_invalidate_index = 0;
             (fem_id_invalidate_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID) &&
             (fems_invalidate[fem_id_invalidate_index] != DNX_FIELD_FEM_ID_INVALID); fem_id_invalidate_index++)
        {
            if (fems_invalidate[fem_id_invalidate_index] == fem_id_index)
            {
                fem_is_invalidated = TRUE;
                break;
            }
        }
        /*
         * We can do "continue" if fem_is_invalidated now, but we proceed to run dnx_field_fem_is_fg_owner
         * for a sanity check.
         */

        /*
         * Indicate whether 'fg_id' is marked as one of the owners of 'fem_id_index'
         */
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id_index, fg_id, &fem_program));
        if (fem_is_invalidated && (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID))
        {
            bcm_field_action_priority_t fem_position;
            DNX_FIELD_ACTION_FEM_ID_ENCODED_POSITION_GET(fem_position, unit, fem_id_index);
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "FEM ID %d (position %d) is marked to be invalidated when attached to context %d, "
                         "but is not used by field group %d.\r\n", fem_id_index, fem_position, context_id, fg_id);
        }
        if (fem_is_invalidated || (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID))
        {
            continue;
        }
        /*
         * Field group has been found on specified 'fem id' (fem_id_index) and
         * 'fem_program' is meaningful.
         * Find out the corresponding 'fem key select'
         */
        /*
         * Just for convenience, use 'fem_id'.
         */
        fem_id = fem_id_index;
        /*
         * Note that we verify that the context is not 'occupied' just before updating HW
         */
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, fem_id, fem_program, &(second_fg_id)));
        if (second_fg_id != DNX_FIELD_GROUP_INVALID)
        {
            /*
             * Enter if the 'replace' feature is enabled on this 'fem_id' (i.e., 'second_fg_id' is valid)
             * Because of previous validity checks, both the stage for 'fg_id' and the stage for 'second_fg_id'
             * are, at this point, either IPMF1 or IPMF2.
             */
            dnx_field_stage_e second_field_stage;
            unsigned int found;

            SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, second_fg_id, &second_field_stage));
            if (second_field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                /*
                 * 'Second_fg_id' is on stage IPMF2. Handle 'fg_id' on: IPMF1 or IPMF2.
                 */
                if (field_stage == DNX_FIELD_STAGE_IPMF1)
                {
                    /*
                     * 'Second_fg_id' is on stage IPMF2 while 'fg_id' (and, therefore, 'context_id') is on IPMF1.
                     * If 'context_id' is a 'parent' context, we need to find all 'sibling's which are 'cascaded
                     * from' 'context_id'. We denote each of them 'second_context_id'. These would be the contexts
                     * that would contribute the LS 16 bits.
                     * In addition, 'context_id' itself has an 'image' on IPMF2 and contributes to the LS 16 bits
                     * as well.
                     */
                    dnx_field_context_t second_context_id;
                    unsigned int num_attached_to_second_fg_id;

                    /*
                     * Now find the 'io' corresponding to the lookup of this ('field group','context') pair and
                     * convert it to the proper encoding of 'replace lookup'.
                     * Update FEM context for this fem_id with new info ('replace_lsb_flag' and 'replace_lsb_select')
                     * Note that at least one context must be found to have been attached to 'second_fg_id'. Otherwise,
                     * an error will be injected.
                     */
                    second_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
                    num_attached_to_second_fg_id = 0;
                    do
                    {
                        SHR_IF_ERR_EXIT(dnx_field_context_next_cascaded_find
                                        (unit, context_id, TRUE, &second_context_id));
                        if (second_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                        {
                            /*
                             * There are no more contexts on IPMF2 to search for a match.
                             */
                            break;
                        }
                        /*
                         * A context which is 'cascaded from' 'context_id' has been found (This includes 'contxt_id' itself).
                         *
                         * Verify that 'second_fg_id' has attached this context. However, if 'second_fg_id' is
                         * equal to 'context_id' (i.e., it is an 'image' of 'context_id' on IPMF2) then it does
                         * not have to have been attached to 'second_fg_id'. In that latter case, also do not update
                         * the HW.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr
                                        (unit, second_fg_id, second_context_id, &found));
                        if (found == FALSE)
                        {
                            if (second_context_id != context_id)
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG,
                                             "\r\n"
                                             "Context %d is marked as 'cascaded from' context %d but is not of the list of contexts of 'second_fg_id' (%d).\r\n"
                                             "Perhaps some contexts were created but not attached?. Quit with error\r\n",
                                             second_context_id, context_id, second_fg_id);
                            }
                            else
                            {
                                continue;
                            }
                        }
                        {
                            /*
                             * Now find the 'io' corresponding to the lookup of this ('field group','context') pair and
                             * convert it to the proper encoding of 'replace lookup'.
                             * Update FEM context for this fem_id with new info ('replace_lsb_flag' and 'replace_lsb_select')
                             */
                            SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_update
                                            (unit, second_fg_id, second_context_id, second_context_id, fem_id));
                            num_attached_to_second_fg_id++;
                        }
                    }
                    while (TRUE);
                    if (num_attached_to_second_fg_id == 0)
                    {
                        /*
                         * No context has been found to have been attached to 'second_fg_id'.
                         * Inject an error.
                         */
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "\r\n"
                                     "'second_fg_id' (%d) on IPMF2 was found to not have any attached context which is\r\n"
                                     "==> cascaded from context %d on fg %d (on IPMF1). This is bad setup. Quit.\r\n",
                                     second_context_id, context_id, fg_id);
                    }
                }
                else
                {
                    /*
                     * 'Second_fg_id' is on stage IPMF2 and primary 'fg_id' (and, therefore, 'context_id') is, also, on IPMF2.
                     */
                    /*
                     * Make sure 'context_id' is already attached to 'Second_fg_id'
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, second_fg_id, context_id, &found));
                    if (found == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "\r\n"
                                     "'Second_fg_id' (%d) and 'fg_id' (%d) are both on IPMF2 but 'context_id' (%d) is not attached to 'Second_fg_id'.\r\n"
                                     "Perhaps some contexts were created but not attached?. Quit with error\r\n",
                                     second_fg_id, fg_id, context_id);
                    }
                    /*
                     * Get IO info for the pair ('Second_fg_id','context_id') and load 'replace related' parameters
                     * accordingly.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_update
                                    (unit, second_fg_id, context_id, context_id, fem_id));
                }
            }
            else if (second_field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                /*
                 * 'Second_fg_id' is on stage IPMF1. Handle 'fg_id' on: IPMF1 or IPMF2.
                 */
                if (field_stage == DNX_FIELD_STAGE_IPMF1)
                {
                    /*
                     * 'Second_fg_id' is on stage IPMF1 while 'fg_id' (and, therefore, 'context_id') is, also, on IPMF1.
                     * Note that all 'siblings' of 'context_id' must also be updated with 'replace related' parameters
                     * from 'Second_fg_id'.
                     */
                    /*
                     * Make sure 'context_id' is already attached to 'Second_fg_id'
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, second_fg_id, context_id, &found));
                    if (found == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "\r\n"
                                     "'Second_fg_id' (%d) and 'fg_id' (%d) are both on IPMF1 but 'context_id' (%d) is not attached to 'Second_fg_id'.\r\n"
                                     "Perhaps some contexts were created but not attached?. Quit with error\r\n",
                                     second_fg_id, fg_id, context_id);
                    }
                    /*
                     * Get IO info for the pair ('Second_fg_id','context_id') and load 'replace related' parameters
                     * (into HW) accordingly.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_update
                                    (unit, second_fg_id, context_id, context_id, fem_id));
                    {
                        /*
                         * 'Second_fg_id' is on stage IPMF1 while 'fg_id' (and, therefore, 'context_id') is on IPMF1.
                         * If 'context_id' is a 'parent' context, we need to find all 'sibling's which are 'cascaded
                         * from' 'context_id'. We denote each of them 'second_context_id'. They need to be updated with
                         * 'Second_fg_id' as contributer of the LS 16 bits.
                         */
                        dnx_field_context_t second_context_id;

                        /*
                         * Now find the 'io' corresponding to the lookup of this ('field group','context') pair and
                         * convert it to the proper encoding of 'replace lookup'.
                         * Update FEM context for this fem_id with new info ('replace_lsb_flag' and 'replace_lsb_select')
                         */
                        second_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
                        do
                        {
                            SHR_IF_ERR_EXIT(dnx_field_context_next_cascaded_find
                                            (unit, context_id, FALSE, &second_context_id));
                            if (second_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                            {
                                /*
                                 * There are no more contexts on IPMF2 to search for a match.
                                 */
                                break;
                            }
                            /*
                             * A context which is 'cascaded from' 'context_id' has been found.
                             */
                            {
                                /*
                                 * Context has been found on list of contexts cascaded from 'context_id'.
                                 * Now find the 'io' corresponding to the lookup of this ('second field group','context')
                                 * pair and convert it to the proper encoding of 'replace lookup'.
                                 * Update FEM context for this fem_id with new info ('replace_lsb_flag' and 'replace_lsb_select')
                                 */
                                SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_update
                                                (unit, second_fg_id, context_id, second_context_id, fem_id));
                            }
                        }
                        while (TRUE);
                    }
                }
                else
                {
                    /*
                     * 'Second_fg_id' is on stage IPMF1 and primary 'fg_id' (and, therefore, 'context_id') is on IPMF2.
                     * If input 'context_id' is 'cascaded from', we need to find the 'parent' context (of input 'context_id')
                     * and use it to calculate the IO accordingly.
                     * Else, if 'context_id' is NOT 'cascaded from', then simply use the 'default' cascading (i.e., the
                     * same context for IPMF1 and for IPMF2).
                     */
                    dnx_field_context_t cascaded_from_context_id;

                    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));
                    /*
                     * At this point, 'cascaded_from_context_id' contains the context id corresponding to
                     * 'Second_fg_id', as it is on IPMF2: Either it is equal to 'context_id' ('default') or
                     * it is a context, on IPMF1, from which 'context_id' is cascaded.
                     */
                    /*
                     * Make sure 'cascaded_from_context_id' is already attached to 'Second_fg_id'
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr
                                    (unit, second_fg_id, cascaded_from_context_id, &found));
                    if (found == FALSE)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "\r\n"
                                     "'Second_fg_id' (%d) is on IPMF1 and 'fg_id' (%d) is on IPMF2 but 'parent context_id' (%d) is not attached to 'Second_fg_id'.\r\n"
                                     "Perhaps some contexts were created but not attached?. Quit with error\r\n",
                                     second_fg_id, fg_id, cascaded_from_context_id);
                    }

                    /*
                     * If 'cascaded_from_context_id' is not equal to 'context_id' then update HW using the IO of the 'parent' context.
                     */
                    if (cascaded_from_context_id != context_id)
                    {
                        /*
                         *
                         * Get IO info for the pair ('Second_fg_id','cascaded_from_context_id') and load 'replace related'
                         * parameters (into HW) into the entry of 'context_id' accordingly.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_update
                                        (unit, second_fg_id, cascaded_from_context_id, context_id, fem_id));
                    }
                    else
                    {
                        /*
                         * Now find the 'io' corresponding to the lookup of this ('field group','context') pair and
                         * convert it to the proper encoding of 'replace lookup'.
                         * Update FEM context for this fem_id with new info ('replace_lsb_flag' and 'replace_lsb_select')
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_update
                                        (unit, second_fg_id, context_id, context_id, fem_id));
                    }
                }
            }
            else
            {
                /*
                 * This is redundant, since it has already been checked on dnx_field_fem_action_add_verify()
                 * but we leave it here,just to make sure...
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Specified 'stage' (%d,%s), for 'second' Field Group %d,is illegal. For FEMs, it must be either IPMF1 or IPMF2.\r\n",
                             field_stage, dnx_field_stage_e_get_name(field_stage), second_fg_id);
            }
        }
        {
            /*
             * Get 'input_offset' from SWSTATE. It is required for all FG types.
             * See dnx_field_bit_range_t.
             * See dnx_field_key_id_t
             */
            dnx_field_attach_context_info_t context_info;
            uint8 relative_actions_lsb;
            /*
             *  This is a
             *    multiple of 16               <<<<<<<<<<<<<<<<
             *  Input offset                   V                  |
             *  Key
             *    template              |-------------------------|
             *  KEY           ...--------------------------------------------------...
             *  Bit aligned                                   |
             *    to 16                                       ^
             *  Physical                                          |
             *    Location of                                     |
             *    key template                                    ^
             */
            /*
             * First get offset from the beginning of allocated 'key template'.
             */
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_offset.get(unit, fem_id, fem_program, &relative_actions_lsb));
            /*
             * Now get the physical location on the key (of the 'key template') and the bit, on 'key template',
             * which has been aligned to '16' (DNX_FIELD_FEM_INPUT_OFFSET_RESOLUTION).
             */
            SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));
            if (context_info.context_info_valid != TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Specified 'context info' for this field group (%d) and context (%d) is marked as 'not loaded'\r\n"
                             "while, at this stage, it should be. Probably an internal sw error.\r\n",
                             fg_id, context_id);
            }
            if (context_info.key_id.bit_range.bit_range_valid == FALSE)
            {
                /*
                 * If 'bit-range' allocation has not been applied then assume allocation starts from 
                 * the beginning of the key.
                 */
                actions_lsb = relative_actions_lsb;
            }
            else
            {
                /*
                 * If bit-range has been applied then it should also have been aligned
                 * for any field group which has FEMs.
                 */
                if ((context_info.key_id.bit_range.bit_range_offset %
                     dnx_data_field.stage.stage_info_get(unit, field_stage)->fem_key_select_resolution_in_bits) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "This field group (%d) and context (%d) is not has offset %d, not aligned to %d.\r\n"
                                 "For field group with FEMs, it should be. Probably an internal sw error.\r\n",
                                 fg_id, context_id, context_info.key_id.bit_range.bit_range_offset,
                                 dnx_data_field.stage.stage_info_get(unit,
                                                                     field_stage)->fem_key_select_resolution_in_bits);
                }
                actions_lsb = context_info.key_id.bit_range.bit_range_offset + relative_actions_lsb;
            }
        }
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_size.get(unit, fem_id, fem_program, &actions_size));
        /*
         * Find out the 'fem key select' corresponding to IO type of the Field group and
         * to specified 'offset' on input.
         * See 'dbal_enum_value_field_field_io_e'
         */
        switch (fg_type)
        {
                unsigned int required_shift;
                int found;

            case DNX_FIELD_GROUP_TYPE_EXEM:
            case DNX_FIELD_GROUP_TYPE_TCAM:
            case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            {
                dbal_enum_value_field_field_io_e field_io;

                SHR_IF_ERR_EXIT(dnx_field_group_field_io_get(unit, fg_id, context_id, &field_io));
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Field group type is %s. Use 'input_offset' (%d) and 'IO' (%s) to get 'fem key select'. %s\r\n",
                             dnx_field_group_type_e_get_name(fg_type), actions_lsb, dnx_field_io_text(field_io), EMPTY);
                SHR_IF_ERR_EXIT(dnx_field_actions_fem_key_select_get
                                (unit, field_io, (unsigned int) actions_size, (unsigned int) actions_lsb,
                                 &action_input_select, &required_shift, &found));
                if (found != TRUE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "\r\n"
                                 "Field group type is %s. No 'fem key select' could be found for input_offset %d and input_size %d. Quit\r\n",
                                 dnx_field_group_type_e_get_name(fg_type), actions_lsb, actions_size);
                }
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "Results of dnx_field_actions_fem_key_select_get() for actions_lsb (%d) actions_size (%d): \r\n"
                             "action_input_select (%d) required_shift (%d): \r\n",
                             actions_lsb, actions_size, action_input_select, required_shift);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Group type = %d (%s)\n", fg_type,
                             dnx_field_group_type_e_get_name(fg_type));
                break;
            }
        }
        {
            /*
             * Update FEM context for this fem_id with new info (ONLY 'fem_program' and 'fem_context_select' -
             * Use read-modify-write).
             * The value of 'key select' is taken to be the result of the clause above ('action_input_select').
             * Other fields ('replace related') may have been updated above, within the handling of the 'second_fg_id'.
             */
            SHR_IF_ERR_EXIT(dnx_field_group_fem_key_select_update
                            (unit, fem_id, context_id, action_input_select, fem_program));
        }
        {
            dnx_field_context_t cascaded_context_id;

            if (field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                /*
                 * If 'fg_id' is on IMPF1 then it may have contexts, on IPMF2, which are 'cascaded from' it.
                 * In that case, they should all have the same context-configuration for 'fem key select' and
                 * 'fem program' but not necessarily the same 'replace related' fields.
                 * We assume, here, that 'action_input_select' (value for 'fem_key_select') and 'replace_lsb_flag'
                 * are properly loaded here.
                 */
                cascaded_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
                do
                {
                    /*
                     * The entry of 'context_id' has already been updated above so do not include it here.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_context_next_cascaded_find
                                    (unit, context_id, FALSE, &cascaded_context_id));
                    if (cascaded_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                    {
                        /*
                         * There are no more contexts on IPMF2 to search for a match.
                         */
                        break;
                    }
                    /*
                     * A context which is 'cascaded from' 'context_id' has been found.
                     * Load it with the same configuration (after validating it is not already
                     * occupied).
                     */
                    {
                        /*
                         * Make sure FEM context is not occupied for this fem_id.
                         */
                        DNX_FIELD_FEM_CONTEXT_NOT_OCCUPIED_VERIFY(unit, fem_id, cascaded_context_id);
                    }
                    {
                        /*
                         * Update FEM context for this fem_id with new info (ONLY 'fem_program' and 'fem_context_select' -
                         * Use read-modify-write).
                         * The value of 'key select' is taken to be the result of the clause above ('action_input_select').
                         * Other fields ('replace related') may have been updated above, within the handling of the 'second_fg_id'.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_fem_key_select_update
                                        (unit, fem_id, cascaded_context_id, action_input_select, fem_program));
                    }
                }
                while (TRUE);
            }
            else
            {
                /*
                 * If 'fg_id' is on IMPF2 then it may have a context, on IPMF1, which is its 'parent'.
                 * In that case, the 'parent' remains as is and is not involved in FEM operation.
                 */
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Check whether specified Field Group resides on some FEMs and, in that case,
 *   Remove those FEMs.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fg_id -
 *   Identifier of a field group to search for on the list of owners of all
 *   'fem_id's.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
  *    * This procedure assumes its input has been verified by the caller.
 *    * Specifically, 'input_offset' (in SWSTATE) is a multiple of 'fem key select' resolution.
 *      Below, we check that it fits (together with a 32-bits 'chunk') into the input block size.
 *    * Also, if FG type is 'direct extraction' then key is either of type DNX_FIELD_KEY_LENGTH_TYPE_SINGLE
 *      or of type DNX_FIELD_KEY_LENGTH_TYPE_HALF (single full key or half key but not double)
 *    * This procedure assumes that 'context_info' has been loaded into SWSTATE. \see dnx_field_group_context_info_set
 *      Also \see dnx_field_group_de_context_attach
 * \see
 *    * dnx_field_group_fems_context_detach()
 *    * dnx_field_group_fems_context_attach()
 *    * dnx_field_group_delete()
 */
static shr_error_e
dnx_field_group_fems_group_delete(
    int unit,
    dnx_field_group_t fg_id)
{
    dnx_field_fem_id_t fem_id_index;
    dnx_field_fem_program_t fem_program;
    int fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;

    SHR_FUNC_INIT_VARS(unit);

    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id_index, fg_id, &fem_program));
        if (fem_program != DNX_FIELD_FEM_PROGRAM_INVALID)
        {
            SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, fem_id_index, fg_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Returns an array of all cascaded FG IDs referenced by the given attach qualifiers.
*  Each FG ID will appear in the array the number of times it was referenced.
*
* \param[in] unit                 - Device ID
* \param[in] quals_attach_info_p  - The qualifiers attach information array
* \param[out] fg_ids              - The array of FG IDs referenced by the attach qualifiers
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_attach_cascaded_fgs_get(
    int unit,
    dnx_field_qual_attach_info_t * quals_attach_info_p,
    dnx_field_group_t fg_ids[])
{
    int qual_idx;
    int fg_idx;

    SHR_FUNC_INIT_VARS(unit);

    fg_idx = 0;
    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (quals_attach_info_p[qual_idx].input_type != DNX_FIELD_INPUT_TYPE_INVALID); qual_idx++)
    {
        if (quals_attach_info_p[qual_idx].input_type == DNX_FIELD_INPUT_TYPE_CASCADED)
        {
            dnx_field_group_t fg_id = quals_attach_info_p[qual_idx].input_arg;
            /*
             * Sanity check
             */
            if (fg_idx >= DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Something seriously wrong happened. Cascaded FG count from qualifiers in a certain FG is larger than number of qualifiers per FG\r\n");
            }
            fg_ids[fg_idx++] = fg_id;
        }
    }
    /*
     * Place the INVALID to mark the end of the array
     */
    if (fg_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        fg_ids[fg_idx] = DNX_FIELD_GROUP_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_group.h
 */
shr_error_e
dnx_field_group_convert_context_key_to_fg_id(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t fg_key,
    dnx_field_group_t * fg_id_p)
{
    int fg_id_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Go over all FGs in the system, searching for the one and only that meets the following conditions we are looking for:
     * 1) Is attached on the given context ID
     * 2) Has the exact same FG type
     * 3) The 'first' key ID it resides on is equal to the 'first' key ID given
     */
    for (fg_id_index = 0; fg_id_index < dnx_data_field.group.nof_fgs_get(unit); fg_id_index++)
    {
        uint8 is_fg_allocated;
        unsigned int is_fg_on_context;
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** FG ID is not allocated, move to next FG ID */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id_index, context_id, &is_fg_on_context));
        if (is_fg_on_context)
        {
            dnx_field_key_id_t fg_to_cmp_key_id;
            dnx_field_group_type_e fg_to_cmp_type;

            SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id_index, &fg_to_cmp_type));
            SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get(unit, fg_id_index, context_id, &fg_to_cmp_key_id));
            if (fg_to_cmp_type == fg_type && fg_to_cmp_key_id.id[0] == fg_key.id[0])
            {
                *fg_id_p = fg_id_index;
                break;
            }
        }
    }

    if (fg_id_index == dnx_data_field.group.nof_fgs_get(unit))
    {
        
        /*
         * SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting given Context(%d)/FG type(%d)/Key ID(%d) to the corresponding
         * FG: no such FG exists\r\n", context_id, fg_type, fg_key.id[0]);
         */
        *fg_id_p = DNX_FIELD_GROUP_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Increases the cascading reference counter of each FG in the "fg_ids" array in the given
*  'cascaded_from_context_id' context by 1.
*  The same FG ID can appear more than once in the array, and its cascading ref will be increased
*  by 1 for every instance of the FG ID in the array.
*
* \param[in] unit                     - Device ID
* \param[in] cascaded_from_context_id - The 'cascaded form' context ID, where the cascaded FGs are located
* \param[in] fg_ids                   - FG IDs that are being cascaded from
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_cascading_ref_count_inc(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_group_t fg_ids[])
{
    int fg_idx;
    uint16 nof_cascading_refs;
    SHR_FUNC_INIT_VARS(unit);

    for (fg_idx = 0; fg_ids[fg_idx] != DNX_FIELD_GROUP_INVALID && fg_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
         fg_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_ids[fg_idx], cascaded_from_context_id, &nof_cascading_refs));

        nof_cascading_refs++;

        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.set(unit, fg_ids[fg_idx], cascaded_from_context_id, nof_cascading_refs));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Decreases the cascading reference counter of each FG in the "fg_ids" array in the given
*  'cascaded_from_context_id' context by 1.
*  The same FG ID can appear more than once in the array, then its cascading ref will be decreased
*  by 1 for every instance of the FG ID in the array.
*
* \param[in] unit                     - Device ID
* \param[in] cascaded_from_context_id - The 'cascaded form' context ID, where the cascaded FGs are located
* \param[in] fg_ids                   - FG IDs that are being cascaded from
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_cascading_ref_count_dec(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_group_t fg_ids[])
{
    int fg_idx;
    uint16 nof_cascading_refs;
    SHR_FUNC_INIT_VARS(unit);

    for (fg_idx = 0; fg_ids[fg_idx] != DNX_FIELD_GROUP_INVALID && fg_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG;
         fg_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_ids[fg_idx], cascaded_from_context_id, &nof_cascading_refs));

        if (nof_cascading_refs <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error decreasing FG %d cascading reference count by 1: current cascading reference count value %d\r\n",
                         fg_ids[fg_idx], nof_cascading_refs);
        }
        nof_cascading_refs--;

        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.set(unit, fg_ids[fg_idx], cascaded_from_context_id, nof_cascading_refs));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_group.h
 */
shr_error_e
dnx_field_group_context_attach(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * group_attach_info_p)
{
    dnx_field_attach_context_info_t context_attach_info;
    dnx_field_group_type_e fg_type;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_group_context_attach_verify(unit, flags, fg_id, context_id, group_attach_info_p));

    SHR_IF_ERR_EXIT(dnx_field_attach_context_info_t_init(unit, &context_attach_info));

    context_attach_info.flags = flags;

    /** Get field group type*/
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_context_attach
                            (unit, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_de_context_attach
                            (unit, flags, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_const_context_attach
                            (unit, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_exem_context_attach
                            (unit, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_state_table_context_attach
                            (unit, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_mdb_dt_context_attach
                            (unit, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_kbp_opcode_attach
                            (unit, fg_id, context_id, group_attach_info_p, &context_attach_info));
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Group type = %d (%s)\n", fg_type,
                         dnx_field_group_type_e_get_name(fg_type));
            break;
        }
    }
    /**
     * Update SW state with new context ID attached and key that was allocated for it.
     * This is not necessary for direct extraction since it is already done
     * within dnx_field_group_de_context_attach()
     */
    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
    {
        /*
         * Save the attach info for this field group in this context
         */
        sal_memcpy(&context_attach_info.qual_attach_info, &group_attach_info_p->qual_info,
                   sizeof(context_attach_info.qual_attach_info));

        context_attach_info.context_info_valid = TRUE;

        if (fg_type == DNX_FIELD_GROUP_TYPE_KBP)
        {
            context_attach_info.payload_offset = group_attach_info_p->payload_offset;
            context_attach_info.payload_id = group_attach_info_p->payload_id;
        }

        SHR_IF_ERR_EXIT(dnx_field_group_context_info_set(unit, fg_id, context_id, &context_attach_info));
    }
    {
        dnx_field_stage_e field_stage;

        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
        if ((field_stage == DNX_FIELD_STAGE_IPMF1) || (field_stage == DNX_FIELD_STAGE_IPMF2))
        {
            /*
             * Check whether this FG had been added to any FEM. If so, update the context accordingly.
             * We assume that, at this point, 'context_attach_info' is loaded with valid information.
             */
            SHR_IF_ERR_EXIT(dnx_field_group_fems_context_attach
                            (unit, fg_id, context_id, group_attach_info_p->fems_invalidate, &context_attach_info));
        }
        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
            dnx_field_context_t cascaded_from_context_id;
            /*
             * When attaching an iPMF2 FG, we iterate over all FG qualifiers to locate all cascading qualifiers
             * in order to increase the 'cascading reference count' of the FG we're cascading from, so that
             * the FG won't get detached/destroyed while still being cascaded from
             */
            SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                            (unit, field_stage, context_id, &cascaded_from_context_id));
            SHR_IF_ERR_EXIT(dnx_field_group_attach_cascaded_fgs_get
                            (unit, context_attach_info.qual_attach_info, fg_ids));
            SHR_IF_ERR_EXIT(dnx_field_group_cascading_ref_count_inc(unit, cascaded_from_context_id, fg_ids));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See header in field_group.h
 */
shr_error_e
dnx_field_group_fems_context_detach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    dnx_field_fem_id_t fem_id_index, fem_id_max;
    dnx_field_fem_id_t fem_id;
    dnx_field_fem_program_t fem_program;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Load stage of 'fg_id' into 'field_stage' to be used for the case of cascading
     * (I.e., 'context_id', on IPMF1, is 'parent' to many 'sibling' contexts on IPMF2).
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    fem_id_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    /*
     * Go through all 'fem id's and update 'fem context' with 'fem program' and 'fem key select'
     * Note: Handling of the 'replace' feature' will be added later on.
     */
    for (fem_id_index = 0; fem_id_index < fem_id_max; fem_id_index++)
    {
        dnx_field_context_t context_id_for_replace;
        unsigned int consistent, valid_replace, valid_context;
        dnx_field_stage_e second_field_stage;
        dnx_field_group_t second_fg_id;
        unsigned int context_using_fem;

        /*
         * Just for convenience, use 'fem_id'.
         */
        fem_id = fem_id_index;

        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id, fg_id, &fem_program));
        if (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID)
        {
            /*
             * Input 'fg_id' is not an 'owner' field group.
             * If it is not 'secondary either then just keep looping.
             * If it is 'secondary'
             *   If specified 'fem_id' is marked as 'invalid' on specified 'context_id' then
             *     there is nothing to do: just keep looping. (This is legitimate since that entry has
             *     been detached, from the FEM point of view, when the 'main' context has been detached
             *     from the 'main' Field group and is now being detached from its containing
             *     ('secondary') Field Group)
             *   Else
             *     reject this detach request since 'secondary' can not be detached before the 'main'
             *     has been detached.
             */
            dnx_field_group_t main_fg_id;

            SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_secondary(unit, fem_id, fg_id, &fem_program));
            if (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID)
            {
                /*
                 * Input 'fg_id' is neither 'owner' nor 'secondary. There is nothing to do here.
                 * Keep looping.
                 */
                continue;
            }
            {
                /*
                 * If this field group is on IMPF1 then check whether any of the contexts which are
                 * 'cascaded from' this context is active (valid) for this fem_id. If so, then this
                 * context can not be detached before the primary ('main') field group detaches
                 * this context (In other words, the primary ('main') field group is using a context
                 * which expects this context to supply input on the 'overide' input).
                 */
                if (field_stage == DNX_FIELD_STAGE_IPMF1)
                {
                    dnx_field_context_t second_context_id;

                    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program, &main_fg_id));
                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, main_fg_id, &second_field_stage));
                    if (second_field_stage == DNX_FIELD_STAGE_IPMF2)
                    {
                        second_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
                        do
                        {
                            unsigned int found;

                            SHR_IF_ERR_EXIT(dnx_field_context_next_cascaded_find
                                            (unit, context_id, TRUE, &second_context_id));
                            if (second_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                            {
                                /*
                                 * There are no more contexts on IPMF2 to search for a match.
                                 */
                                break;
                            }
                            /*
                             * A context which is 'cascaded from' 'context_id' has been found.
                             * Now verify that 'second_fg_id' has attached this context.
                             */
                            SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr
                                            (unit, main_fg_id, second_context_id, &found));
                            if (found)
                            {
                                /*
                                 * The context may not be using a FEM at all due to invalidate FEMS,
                                 * even if the main field group uses the FEM ID.
                                 */
                                SHR_IF_ERR_EXIT(dnx_field_fem_is_context_using
                                                (unit, fem_id, second_context_id, main_fg_id, &context_using_fem));
                            }
                            if (found == FALSE || context_using_fem == FALSE)
                            {
                                if (context_id != second_context_id)
                                {
                                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                                 "\r\n"
                                                 "Context %d is marked as 'cascaded from' context %d but is not of the list of contexts of 'main_fg_id' (%d).\r\n"
                                                 "Perhaps some contexts were created but not attached?. Quit with error\r\n",
                                                 second_context_id, context_id, main_fg_id);
                                }
                                else
                                {
                                    continue;
                                }
                            }
                            else
                            {
                                /*
                                 * 'cascaded from' context has been found on list of attached contexts.
                                 * In that case, refuse to detach until the 'main' FG detaches this 'cascaded from' context.
                                 */
                                char *defualt_text;
                                second_fg_id = fg_id;
                                if (context_id == second_context_id)
                                {
                                    defualt_text = "(by default)";
                                }
                                else
                                {
                                    defualt_text = "";
                                }
                                SHR_ERR_EXIT(_SHR_E_PARAM,
                                             "\r\n"
                                             "This 'context_id' (%d) is on secondary field group (%d) and cascades %s to a context (%d)\r\n"
                                             "==> on the 'main' field group (%d). First, this context should be detached from\r\n"
                                             "==> the 'main' Field Group (%d). Parameters info: fem_id %d \r\n",
                                             context_id, fg_id, defualt_text, second_context_id, main_fg_id, main_fg_id,
                                             fem_id);
                            }
                        }
                        while (TRUE);
                    }
                }
            }
            /*
             * At this point, 'fem_program' is the one that contains all FGs data.
             * We use it to extract the 'main' FG.
             */
            {
                SHR_IF_ERR_EXIT(dnx_field_fem_is_context_valid(unit, fem_id, context_id, &valid_context));
                if (valid_context != TRUE)
                {
                    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program, &main_fg_id));
                    second_fg_id = fg_id;
                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "This 'fem_id' (%d) on this 'context_id' (%d), which is on 'secondary' \r\n"
                                 "Field Group (%d), has already been detached when the 'main' field group (%d)\r\n"
                                 "has been detached. This is OK. Nothing to do.\r\n",
                                 fem_id, context_id, second_fg_id, main_fg_id);
                    continue;
                }
            }
            {
                /*
                 * Input fg_id is 'secondary' (= 'Second_fg_id') and context is valid.
                 * First, make sure that all three ('fem_id', 'fem_program' and 'context_id') are
                 * consistent on HW. If they are, then user is trying to detach a 'secondary' context
                 * before the 'main' has been detached. Reject this request.
                 */
                SHR_IF_ERR_EXIT(dnx_field_fem_is_context_consistent
                                (unit, fem_id, fem_program, context_id, &consistent));
                if (consistent != TRUE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "\r\n"
                                 "This 'context_id' (%d) is not consistent with input parameters (fem_id %d, fem_program %d, fg_id %d). \r\n"
                                 "This may indicate some internal inconsistency. \r\n",
                                 context_id, fem_id, fem_program, fg_id);
                }
                else
                {
                    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program, &main_fg_id));
                    second_fg_id = fg_id;
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "\r\n"
                                 "This 'context_id' (%d) is on secondary field group (%d). First, the 'main' context should be\r\n"
                                 "detached from the 'main' Field Group (%d). Parameters info: fem_id %d \r\n",
                                 context_id, second_fg_id, main_fg_id, fem_id);
                }
            }
        }
        /*
         * Input 'fg_id' is an 'owner' field group.
         */
        {
            /*
             * The context may not be using a FEM at all due to invalidate FEMS, even if the field group uses the FEM ID.
             */
            SHR_IF_ERR_EXIT(dnx_field_fem_is_context_using(unit, fem_id, context_id, fg_id, &context_using_fem));
            if (context_using_fem == FALSE)
            {
                continue;
            }
        }
        /*
         *  if there is a 'second_fg_id'
         *    if input context is on 'main' fg ('owner'):
         *      if there is no valid context with a valid 'replace' that point to this 'fem program'
         *        internal error
         *      else
         *        remove 'replace' info that has been added to the various 'secondary' contexts
         *      endif
         *      If input 'fem context' still has valid 'replace' field, refuse to detach
         *      At this point, there are no context entries related to 'second_fg_id'
         *    endif
         *  endif
         */
        /*
         * If 'owner' is on IPMF1 Make sure to remove it and all fem contexts which are cascaded from this context.
         * Else ('owner' is on IPMF2), just remove it.
         */
        {
            /*
             * 'Owner' ('main') Field Group has been found on specified 'fem id' (fem_id_index) and
             * 'fem_program' is meaningful.
             */
            SHR_IF_ERR_EXIT(dnx_field_fem_is_context_consistent(unit, fem_id, fem_program, context_id, &consistent));
            if (consistent != TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "\r\n"
                             "This 'context_id' (%d) is not consistent with input parameters (fem_id %d, fem_program %d, fg_id %d) \r\n"
                             "This may indicate some internal inconsistency. \r\n",
                             context_id, fem_id, fem_program, fg_id);
            }
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, fem_id, fem_program, &(second_fg_id)));
            if (second_fg_id != DNX_FIELD_GROUP_INVALID)
            {
                /*
                 * Enter if there is a 'secondary' field group. In other words, enter if the 'replace
                 * of ls 16 bits' feature is active.
                 */
                do
                {
                    SHR_IF_ERR_EXIT(dnx_field_fem_is_any_context_with_replace
                                    (unit, fem_id, fem_program, &context_id_for_replace));
                    if (context_id_for_replace == DNX_FIELD_CONTEXT_ID_INVALID)
                    {
                        /*
                         * If there is no valid context with valid 'replace' that point to this 'fem program'
                         * then this is an internal error.
                         */
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "\r\n"
                                     "This main 'field group' (%d) has a 'second field group' (%d) but there are no contexts with 'replace' info.\r\n"
                                     "This may indicate some internal inconsistency. \r\n"
                                     "Parameters info: 'context_id' (%d) fem_id (%d) fem_program (%d) \r\n",
                                     fg_id, second_fg_id, context_id, fem_id, fem_program);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, second_fg_id, &second_field_stage));
                    if (second_field_stage == DNX_FIELD_STAGE_IPMF2)
                    {
                        /*
                         * 'Second_fg_id' is on stage IPMF2. Handle 'fg_id' on: IPMF1 or IPMF2.
                         */
                        if (field_stage == DNX_FIELD_STAGE_IPMF1)
                        {
                            /*
                             * 'Second_fg_id' is on stage IPMF2 while 'fg_id' (and, therefore, 'context_id') is on IPMF1.
                             * If 'context_id' is a 'parent' context, we need to find all 'sibling's which are 'cascaded
                             * from' 'context_id'. We denote each of them 'second_context_id'. These would be the contexts
                             * that would contribute the LS 16 bits.
                             * In addition, 'context_id' itself has an 'image' on IPMF2 and contributes to the LS 16 bits
                             * as well.
                             */
                            dnx_field_context_t second_context_id;

                            second_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
                            do
                            {
                                SHR_IF_ERR_EXIT(dnx_field_context_next_cascaded_find
                                                (unit, context_id, TRUE, &second_context_id));
                                if (second_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                                {
                                    /*
                                     * There are no more contexts on IPMF2 to search for a match.
                                     */
                                    break;
                                }
                                {
                                    /*
                                     * 'cascaded from' context has been found on list of attached contexts.
                                     * Update FEM context. Invalidate 'replace' info ('replace_lsb_flag' and 'replace_lsb_select')
                                     */
                                    SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_clear
                                                    (unit, second_context_id, fem_id));
                                }
                            }
                            while (TRUE);
                        }
                        else
                        {
                            /*
                             * 'Second_fg_id' is on stage IPMF2 and primary 'fg_id' (and, therefore, 'context_id') is, also, on IPMF2.
                             * Load 'replace related' parameters with 'clear' values on the same context that represents both.
                             */
                            SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_clear(unit, context_id, fem_id));
                        }
                    }
                    else if (second_field_stage == DNX_FIELD_STAGE_IPMF1)
                    {
                        /*
                         * 'Second_fg_id' is on stage IPMF1. Handle 'fg_id' on: IPMF1 or IPMF2.
                         */
                        if (field_stage == DNX_FIELD_STAGE_IPMF1)
                        {
                            /*
                             * 'Second_fg_id' is on stage IPMF1 while 'fg_id' (and, therefore, 'context_id') is, also, on IPMF1.
                             * Load 'replace related' parameters with 'clear' values on the same context that represents both.
                             */
                            SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_clear(unit, context_id, fem_id));
                        }
                        else
                        {
                            /*
                             * 'Second_fg_id' is on stage IPMF1 and primary 'fg_id' (and, therefore, 'context_id') is on IPMF2.
                             * Load 'replace related' parameters with 'clear' values on primary. If they are not on the same
                             * context then the 'secondary' context has no 'replace' info.
                             */
                            SHR_IF_ERR_EXIT(dnx_field_group_fems_replace_lsb_select_clear(unit, context_id, fem_id));
                        }
                    }
                    else
                    {
                        /*
                         * This is redundant, since it has already been checked on dnx_field_fem_action_add_verify()
                         * but we leave it here,just to make sure...
                         */
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Specified 'stage' (%d,%s), for 'second' Field Group %d,is illegal. For FEMs, it must be either IPMF1 or IPMF2.\r\n",
                                     field_stage, dnx_field_stage_e_get_name(field_stage), second_fg_id);
                    }
                }
                while (0);
                /*
                 * If, at this point, somehow, input context has valid 'replace' info then it should have been
                 * removed using a different context. Quit with error.
                 */
                SHR_IF_ERR_EXIT(dnx_field_fem_is_context_valid_replace
                                (unit, fem_id, fem_program, context_id, &valid_replace));
                if (valid_replace == TRUE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "This 'context_id' (%d) still has valid 'replace' info (fem_id %d, fem_program %d, fg_id %d). \r\n"
                                 "This may indicate some internal inconsistency. \r\n",
                                 context_id, fem_id, fem_program, fg_id);
                }
            }
        }
        /*
         * If 'owner' is on IPMF1 Make sure to remove it and all fem
         * contexts which are cascaded from this context.  else ('owner' is on IPMF2) Just remove it.
         * We only remove from the 'owner' side.
         */
        {
            dnx_field_context_t cascaded_context_id;

            {
                /*
                 * Update FEM context for this fem_id with default (clear) info.
                 * Always remove the input context.
                 */
                SHR_IF_ERR_EXIT(dnx_field_actions_single_fem_single_context_defaults_set(unit, context_id, fem_id));
            }
            if (field_stage == DNX_FIELD_STAGE_IPMF1)
            {
                /*
                 * If 'fg_id' is on IMPF1 then it may have contexts, on IPMF2, which are 'cascaded from' it.
                 * In that case, they should all be set to 'default' values.
                 */
                cascaded_context_id = DNX_FIELD_CONTEXT_ID_INVALID;
                do
                {
                    /*
                     * The entry of 'context_id' has already been updated above so do not include it here.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_context_next_cascaded_find
                                    (unit, context_id, FALSE, &cascaded_context_id));
                    if (cascaded_context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                    {
                        /*
                         * There are no more contexts on IPMF2 to search for a match.
                         */
                        break;
                    }
                    /*
                     * A context which is 'cascaded from' 'context_id' has been found.
                     * Load it with the same configuration (after validating it is not already
                     * occupied).
                     */
                    {
                        /*
                         * Make sure cascaded FEM context is occupied for this fem_id.
                         */
                        DNX_FIELD_FEM_CONTEXT_OCCUPIED_VERIFY(unit, fem_id, cascaded_context_id);
                    }
                    {
                        /*
                         * Update cascaded FEM context for this fem_id with default (clear) values
                         */
                        SHR_IF_ERR_EXIT(dnx_field_actions_single_fem_single_context_defaults_set
                                        (unit, cascaded_context_id, fem_id));
                    }
                }
                while (TRUE);
            }
            else
            {
                /*
                 * If 'fg_id' is on IMPF2 then it may have a context, on IPMF1, which is its 'parent'.
                 * In that case, the 'parent' remains as is and is not involved in FEM operation.
                 * The input context entry itself has been cleared above.
                 */
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_group_default_context_detach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    dnx_field_attach_context_info_t context_info;
    dnx_field_stage_e field_stage;
    dnx_field_key_attach_info_in_t key_in_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_GROUP_ID_VERIFY(fg_id);
    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &key_in_info.fg_type));
    /*
     * Key Algo knows no DT TCAM type, and since they share same key resources,
     * we treat them the same
     */
    if (key_in_info.fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        key_in_info.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
    }
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get(unit, fg_id, context_id, key_in_info.qual_info));
    if (context_info.context_info_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to detach a non attached CONTEXT_ID = %d FG=%d \n", context_id, fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    key_in_info.field_stage = field_stage;

    SHR_IF_ERR_EXIT(dnx_field_key_detach(unit, context_id, &key_in_info, &(context_info.key_id)));
    SHR_IF_ERR_EXIT(dnx_field_group_fes_unconfig(unit, field_stage, fg_id, context_id));
    /**Remove the Program Id from SW of the given Field Group*/
    SHR_IF_ERR_EXIT(dnx_field_attach_context_info_t_init(unit, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_context_info_set(unit, fg_id, context_id, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_fems_context_detach(unit, fg_id, context_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_group_state_table_context_detach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    dnx_field_attach_context_info_t context_info;
    dnx_field_key_attach_info_in_t key_in_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_in_info.key_template));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &key_in_info.field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &key_in_info.fg_type));

    if (context_info.context_info_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Trying to detach a non attached CONTEXT_ID = %d FG=%d \n", context_id, fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_context_state_table_detach(unit, key_in_info.field_stage, context_id));

    SHR_IF_ERR_EXIT(dnx_field_key_detach(unit, context_id, &key_in_info, &(context_info.key_id)));

    /**Remove the Program Id from SW of the given Field Group*/
    SHR_IF_ERR_EXIT(dnx_field_attach_context_info_t_init(unit, &context_info));
    SHR_IF_ERR_EXIT(dnx_field_group_context_info_set(unit, fg_id, context_id, &context_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_group_context_detach_verify(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    uint16 nof_cascading_refs;
    dnx_field_stage_e field_stage;
    uint8 is_allocated = FALSE;
    unsigned int is_attached;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(field_stage, context_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d is not attached to context %d. \n", fg_id, context_id);
    }

    if (field_stage == DNX_FIELD_STAGE_IPMF1)
    {
        /*
         * Check FG cascading reference count in this context (only for iPMF1 stage)
         */
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_id, context_id, &nof_cascading_refs));
        if (nof_cascading_refs > 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Can't detach FG %d from Context %d: FG payload is being used for cascading by %d qualifiers\r\n",
                         fg_id, context_id, nof_cascading_refs);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_group_cascading_ref_counter_update(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    dnx_field_group_t fg_ids[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_context_t cascaded_from_context_id;
    dnx_field_attach_context_info_t context_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));

    /*
     * When detaching an iPMF2 FG, we iterate over all FG qualifiers to locate all cascading qualifiers
     * in order to decrease the 'cascading reference count' of the FG we're cascading from, so that
     * the FG can get detached/destroyed when no more FGs are cascading from it.
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));
    SHR_IF_ERR_EXIT(dnx_field_group_attach_cascaded_fgs_get(unit, context_info.qual_attach_info, fg_ids));
    SHR_IF_ERR_EXIT(dnx_field_group_cascading_ref_count_dec(unit, cascaded_from_context_id, fg_ids));
exit:
    SHR_FUNC_EXIT;
}
/**
 * See header on field_group.h
 */
shr_error_e
dnx_field_group_context_detach(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id)
{
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_group_context_detach_verify(unit, fg_id, context_id));

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_cascading_ref_counter_update(unit, fg_id, context_id));
    }

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        case DNX_FIELD_GROUP_TYPE_CONST:
            SHR_IF_ERR_EXIT(dnx_field_group_default_context_detach(unit, fg_id, context_id));
            break;
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            SHR_IF_ERR_EXIT(dnx_field_group_state_table_context_detach(unit, fg_id, context_id));
            break;
        case DNX_FIELD_GROUP_TYPE_KBP:
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "FG type: %d not supported\r\n", fg_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_context_create_fem_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from)
{
    uint8 is_alloc;
    dnx_field_group_t fg_id_ndx, nof_fgs;
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    int is_attached;
    dnx_field_stage_e fg_stage;
    unsigned int context_id_ndx;
    dnx_field_fem_id_t fem_id_ndx, nof_fem_id;
    dnx_field_fem_program_t fem_program;
    dnx_field_group_t second_fg_id;
    dnx_field_stage_e second_field_stage;
    dnx_field_pmf_fem_context_entry_t dnx_field_pmf_fem_program_entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only iPMF2 needs to update FEMs from the context it cascades from.
     */
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * Sanity check: Although it should have been verified, we check that we cascade from a different context_id
         *               than the one we use.
         */
        if (context_id == cascaded_from)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "Attempting to allocate an iPMF2 context for default cascading (context id %d).\r\n",
                         context_id);
        }
        /*
         * Go over every field group that is attached to the context we cascade from.
         */
        nof_fgs = dnx_data_field.group.nof_fgs_get(unit);
        for (fg_id_ndx = 0; fg_id_ndx < nof_fgs; fg_id_ndx++)
        {
            /*
             * Check whether field group is allocated
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_ndx, &is_alloc));
            if (!is_alloc)
            {
                /*
                 * If field_group is not allocated, move to next FG ID
                 */
                continue;
            }
            else
            {
                /*
                 * Check whether the field group is at stage iPMF1.
                 */
                SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_ndx, &fg_stage));
                if (fg_stage == DNX_FIELD_STAGE_IPMF1)
                {
                    /*
                     * Get the list of all contexts that the FG is attached to.
                     */
                    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_ndx, context_id_arr));
                    /*
                     * Find out whether the context we cascade from is attached to this fg_id.
                     */
                    is_attached = FALSE;
                    for (context_id_ndx = 0; context_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
                         context_id_arr[context_id_ndx] != DNX_FIELD_CONTEXT_ID_INVALID; context_id_ndx++)
                    {
                        if (context_id_arr[context_id_ndx] == cascaded_from)
                        {
                            is_attached = TRUE;
                            break;
                        }
                    }
                    if (is_attached)
                    {
                        /*
                         * Loop over all FEM IDs to see if the field group is an owner.
                         */
                        nof_fem_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
                        for (fem_id_ndx = 0; fem_id_ndx < nof_fem_id; fem_id_ndx++)
                        {
                            SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_owner(unit, fem_id_ndx, fg_id_ndx, &fem_program));
                            if (fem_program == DNX_FIELD_FEM_PROGRAM_INVALID)
                            {
                                continue;
                            }
                            /*
                             * Verify whether there isn't a secondary field group in iPMF2.
                             * The secondary field group must be attached first, and as that cannot happen
                             * before creating the context, we return an error.
                             */
                            SHR_IF_ERR_EXIT(dnx_field_action_sw_db.fem_info.fg_id_info.
                                            second_fg_id.get(unit, fem_id_ndx, fem_program, &(second_fg_id)));
                            if (second_fg_id != DNX_FIELD_GROUP_INVALID)
                            {
                                SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get
                                                (unit, second_fg_id, &second_field_stage));
                                if (second_field_stage == DNX_FIELD_STAGE_IPMF2)
                                {
                                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                                 "Cannot create an iPMF2 context when the cascaded-from context has "
                                                 "FEM with secondary field group in iPMF2, since the secondary "
                                                 "field group must be attached first.\n"
                                                 "Attempted to creat an iPMF2 context %d cascaded from %d. "
                                                 "Cascaded from context is attached to a field group %d that uses "
                                                 "FEM ID %d with secondary field group %d in iPMF2.\n",
                                                 context_id, cascaded_from, fg_id_ndx, fem_id_ndx, second_fg_id);
                                }
                            }
                            /*
                             * Obtain the fem 'key_select', 'program', and 'replace_lsb_select' from the
                             * 'cascaded_from' context and copy to the new context.
                             */
                            dnx_field_pmf_fem_program_entry.fem_id = fem_id_ndx;
                            dnx_field_pmf_fem_program_entry.context_id = cascaded_from;
                            SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_get
                                            (unit, &dnx_field_pmf_fem_program_entry));
                            dnx_field_pmf_fem_program_entry.context_id = context_id;
                            SHR_IF_ERR_EXIT(dnx_field_actions_fem_context_hw_set
                                            (unit, &dnx_field_pmf_fem_program_entry));
                        }
                    }
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "\r\n"
                     "No stage other than iPMF2 has cascaded contexts and inter stage shared FEMs.\r\n"
                     "==> Attempted to update FEMs for stage %s context Id %d.\r\n",
                     dnx_field_stage_text(unit, stage), context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_context_destroy_fem_update(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_context_t cascaded_from)
{
    dnx_field_fem_id_t fem_id_ndx;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Only iPMF2 needs to update FEMs from the context it cascades from.
     */
    if (stage == DNX_FIELD_STAGE_IPMF2)
    {
        /*
         * Sanity check: Although it should have been verified, we check that we cascade from a different context_id
         *               than the one we use.
         */
        if (context_id == cascaded_from)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Attempting to allocate an iPMF2 context for default cascading (context id %d).\r\n",
                         context_id);
        }
        /*
         * Go over every FEM ID and check if it is in use by the context, and if so make the context stop using it.
         * Note that we assume that all field groups were detached from the context
         * (as should be verified by dnx_field_context_destroy_verify()), ans so all FEM IDs used by the context
         * should come from cascading.
         */
        for (fem_id_ndx = 0; fem_id_ndx < dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
             fem_id_ndx++)
        {
            /*
             * Stop the context from using the FEM ID.
             */
            SHR_IF_ERR_EXIT(dnx_field_actions_single_fem_single_context_defaults_set(unit, context_id, fem_id_ndx));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "No stage other than iPMF2 has cascaded contexts and inter stage shared FEMs. "
                     "Attempted to update FEMs for stage %s context Id %d.\r\n",
                     dnx_field_stage_text(unit, stage), context_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_dnx_action_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *nof_actions_p)
{
    unsigned int action_ndx;
    dnx_field_actions_fg_payload_info_t actions_payload_info;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_GROUP_ID_VERIFY(fg_id);
    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_payload_info));
    for (action_ndx = 0;
         (action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
          && actions_payload_info.actions_on_payload_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID);
         action_ndx++)
    {
        dnx_actions[action_ndx] = actions_payload_info.actions_on_payload_info[action_ndx].dnx_action;
    }
    if (action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        dnx_actions[action_ndx] = DNX_FIELD_ACTION_INVALID;
    }
    (*nof_actions_p) = action_ndx;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_ace_format_dnx_action_arr_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP],
    unsigned int *nof_actions_p)
{
    unsigned int action_ndx;
    dnx_field_actions_ace_payload_info_t actions_payload_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_ace_format_action_template_get(unit, ace_id, &actions_payload_info));
    
    for (action_ndx = 0;
         (action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP
          && actions_payload_info.actions_on_payload_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID);
         action_ndx++)
    {

        dnx_actions[action_ndx] = actions_payload_info.actions_on_payload_info[action_ndx].dnx_action;
    }
    if (action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        dnx_actions[action_ndx] = DNX_FIELD_ACTION_INVALID;
    }
    (*nof_actions_p) = action_ndx;
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_dnx_qual_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    unsigned int *nof_quals_p)
{
    unsigned int qual_ndx;
    dnx_field_key_template_t key_template;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_GROUP_ID_VERIFY(fg_id);
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));

    for (qual_ndx = 0;
         qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG
         && key_template.key_qual_map[qual_ndx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID; qual_ndx++)
    {
        dnx_quals[qual_ndx] = key_template.key_qual_map[qual_ndx].qual_type;
    }

    if (qual_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        dnx_quals[qual_ndx] = DNX_FIELD_QUAL_TYPE_INVALID;
    }
    (*nof_quals_p) = qual_ndx;

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_dnx_qual_is_ranged_arr_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 qual_is_ranged[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    unsigned int *nof_quals_p)
{
    unsigned int qual_ndx;
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dbal_fields_e field_id;
    dnx_field_stage_e field_stage;
    dbal_tables_e dbal_table_id;
    dnx_field_group_type_e fg_type;
    int qual_is_ranged_found = FALSE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_GROUP_ID_VERIFY(fg_id);
    sal_memset(qual_is_ranged, 0x0, (sizeof(qual_is_ranged[0]) * DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, dnx_quals, nof_quals_p));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    if (dbal_table_id != DBAL_TABLE_EMPTY)
    {
        for (qual_ndx = 0; qual_ndx < (*nof_quals_p); qual_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual(unit, field_stage, dnx_quals[qual_ndx], &field_id));
            SHR_IF_ERR_EXIT(dbal_table_field_is_ranged_get(unit, dbal_table_id, field_id, &qual_is_ranged[qual_ndx]));
            if (qual_is_ranged[qual_ndx])
            {
                qual_is_ranged_found = TRUE;
            }
        }
    }

    /** Sanity check.*/
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (qual_is_ranged_found && ((fg_type != DNX_FIELD_GROUP_TYPE_KBP) || (field_stage != DNX_FIELD_STAGE_EXTERNAL)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Group %d has a ranged qualifier "
                     "but is not an external field group (type %d). \n", fg_id, fg_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_context_basic_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_attach_info_t * attach_basic_info_p,
    dnx_field_actions_fes_context_group_get_info_t * fes_group_info_p)
{
    uint8 is_allocated = FALSE;
    unsigned int nof_quals;
    unsigned int nof_actions;
    dnx_field_attach_context_info_t context_info;
    dnx_field_qual_attach_info_t qual_attach_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    unsigned int is_attached;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(attach_basic_info_p, _SHR_E_PARAM, "attach_basic_info_p");

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_is_context_id_on_arr(unit, fg_id, context_id, &is_attached));
    if (is_attached == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d is not attached to context %d. \n", fg_id, context_id);
    }

    
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, attach_basic_info_p->dnx_quals, &nof_quals));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_action_arr_get(unit, fg_id, attach_basic_info_p->dnx_actions, &nof_actions));
    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));
    attach_basic_info_p->payload_offset = context_info.payload_offset;
    attach_basic_info_p->payload_id = context_info.payload_id;

    /*
     * Collect key attach info from sw state.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_qualifier_attach_info_get(unit, fg_id, context_id, qual_attach_info));
    sal_memcpy(attach_basic_info_p->qual_info, qual_attach_info, sizeof(attach_basic_info_p->qual_info));

    /*
     * Collect the invalidated FEMs.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_fem_invalidated_fems
                    (unit, fg_id, context_id, attach_basic_info_p->fems_invalidate));

    /*
     * Collect FES info from sw state and from HW.
     * Also fill the the payload attach info.
     */
    if (fes_group_info_p == NULL)
    {
        dnx_field_actions_fes_context_group_get_info_t fes_group_info;
        SHR_IF_ERR_EXIT(dnx_field_group_context_fes_info_payload_attach_info_get
                        (unit, fg_id, context_id, &fes_group_info, attach_basic_info_p->action_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_group_context_fes_info_payload_attach_info_get
                        (unit, fg_id, context_id, fes_group_info_p, attach_basic_info_p->action_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_context_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_group_context_full_info_t * attach_full_info_p)
{
    dnx_field_key_group_ffc_info_t *group_ffc_info_p;
    dbal_enum_value_field_field_io_e *field_io_p;
    uint8 is_allocated = FALSE;
    dnx_field_attach_context_info_t context_info;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(attach_full_info_p, _SHR_E_PARAM, "attach_full_info_p");

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }

    /*
     * Get the basic info and EFES info.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_context_basic_info_get(unit, fg_id, context_id,
                                                           &(attach_full_info_p->attach_basic_info),
                                                           &(attach_full_info_p->actions_fes_info)));

    group_ffc_info_p = &(attach_full_info_p->group_ffc_info);
    field_io_p = &(attach_full_info_p->field_io);
    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get(unit, fg_id, context_id, &(attach_full_info_p->key_id)));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, context_id, &context_info));

    /*
     * At this point,
     * specified context ID is attached to specified field group.
     */
    if ((fg_type != DNX_FIELD_GROUP_TYPE_CONST) && (fg_type != DNX_FIELD_GROUP_TYPE_KBP))
    {
        dnx_field_key_template_t key_template;
        dnx_field_stage_e field_stage;
        dnx_field_app_db_id_t app_db_id;

        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
        SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
        SHR_IF_ERR_EXIT(dnx_field_key_info_get
                        (unit, field_stage, context_id, fg_type, &attach_full_info_p->key_id, &key_template,
                         group_ffc_info_p, &app_db_id));
        attach_full_info_p->app_db_id = app_db_id;
    }
    else
    {
        attach_full_info_p->app_db_id = DNX_FIELD_APP_DB_ID_INVALID;
    }

    
    if (fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
        {
            int opcode_id = context_id;
            unsigned int fwd_context_idx;
            int nof_fwd_contexts;
            dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
            dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
            bcm_field_AppType_t apptype;

            SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype));
            SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_fwd_contexts
                            (unit, apptype, &nof_fwd_contexts, fwd_contexts, acl_contexts));
            for (fwd_context_idx = 0; fwd_context_idx < nof_fwd_contexts; fwd_context_idx++)
            {
                SHR_IF_ERR_EXIT(dnx_field_key_kbp_ffc_info_get
                                (unit, fg_id, opcode_id, fwd_contexts[fwd_context_idx], acl_contexts[fwd_context_idx],
                                 group_ffc_info_p));
            }
        }
        else
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Can't retrieve any information about FFCs, used by "
                         "External TCAM field groups, before device lock. %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_IF_ERR_EXIT(dnx_field_group_key_length_type_get(unit, fg_id, &(attach_full_info_p->key_length_type)));

    /**Fill info payload regarding input to FES (field_io).*/
    SHR_IF_ERR_EXIT(dnx_field_group_field_io_get(unit, fg_id, context_id, field_io_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Allocate Field Group ID
* \param [in] unit     - Device id
* \param [in] flags    - Can be WITH_ID
* \param [in] fg_id_p  - Pointer Field Group ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_id_alloc(
    int unit,
    dnx_field_group_add_flags_e flags,
    dnx_field_group_t * fg_id_p)
{
    int alloc_flag;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_id_p, _SHR_E_PARAM, "fg_id_p");
    if (flags & DNX_FIELD_GROUP_ADD_FLAG_WITH_ID)
    {
        alloc_flag = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
    }
    else
    {
        alloc_flag = 0;
    }
    SHR_IF_ERR_EXIT(dnx_algo_field_group_id_allocate(unit, alloc_flag, fg_id_p));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Allocate app_db_id for field group type of EXEM
* \param [in] unit     - Device id
* \param [in] field_stage    - Field Stage to allocate the App_db_id, each stage access different EXEM and has its own app_db_ids
* \param [out] app_db_id_p   - Pointer to allocated app_db_id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_app_db_id_alloc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_app_db_id_t * app_db_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_db_id_p, _SHR_E_PARAM, "app_db_id_p");
    alloc_flags = 0;
    
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_EPMF:
            /*
             * Allocate APP_DB_ID dor LEXEM.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_lexem_app_db_id_allocate(unit, alloc_flags, app_db_id_p));
            break;
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF3:
            /*
             * Allocate APP_DB_ID dor SEXEM.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_sexem_app_db_id_allocate(unit, alloc_flags, app_db_id_p));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d).\r\n", field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Deallocate app_db_id for field group type of EXEM
* \param [in] unit     - Device id
* \param [in] field_stage    - Field Stage to allocate the App_db_id, each stage access different EXEM and has its own app_db_ids
* \param [out] app_db_id     - Pointer to the app_db_id to deallocate.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_app_db_id_dealloc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_app_db_id_t app_db_id)
{
    SHR_FUNC_INIT_VARS(unit);
    
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_EPMF:
            /*
             * Deallocate APP_DB_ID dor LEXEM.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_lexem_app_db_id_deallocate(unit, app_db_id));
            break;
        case DNX_FIELD_STAGE_IPMF2:
        case DNX_FIELD_STAGE_IPMF3:
            /*
             * Deallocate APP_DB_ID dor SEXEM.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_sexem_app_db_id_deallocate(unit, app_db_id));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d).\r\n", field_stage);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Allocate ACE ID
* \param [in] unit     - Device id
* \param [in] flags    - Can be WITH_ID
* \param [in] ace_id_p - Pointer to the allocated ACE ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_id_alloc(
    int unit,
    dnx_field_ace_format_add_flags_e flags,
    dnx_field_ace_id_t * ace_id_p)
{
    int alloc_flag;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_id_p, _SHR_E_PARAM, "ace_id_p");
    if (flags & DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID)
    {
        alloc_flag = DNX_ALGO_TEMPLATE_ALLOCATE_WITH_ID;
    }
    else
    {
        alloc_flag = 0;
    }
    SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_allocate(unit, alloc_flag, ace_id_p));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Calculate the maximum possible size of prefix in the key
*  The prefix is located in the MSB bits of the key to differentiate between field group on same TCAM bank
* \param [in] unit          - Device ID
* \param [in] actual_key_size_in_bits - actual key size in bits
* \param [in] key_length_type         - key length type
* \param [out] prefix_size_p          - prefix size
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_prefix_calc(
    int unit,
    uint16 actual_key_size_in_bits,
    dnx_field_key_length_type_e key_length_type,
    uint32 *prefix_size_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(prefix_size_p, _SHR_E_PARAM, "prefix_size_p");
    switch (key_length_type)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
            /*
             * DT FGs do not have prefix support
             */
            *prefix_size_p = 0;
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
            *prefix_size_p =
                MIN((dnx_data_field.tcam.key_size_half_get(unit) - actual_key_size_in_bits),
                    dnx_data_field.tcam.max_prefix_size_get(unit));
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
            *prefix_size_p =
                MIN((dnx_data_field.tcam.key_size_single_get(unit) - actual_key_size_in_bits),
                    dnx_data_field.tcam.max_prefix_size_get(unit));
            break;
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
            *prefix_size_p =
                MIN((dnx_data_field.tcam.key_size_double_get(unit) - actual_key_size_in_bits),
                    dnx_data_field.tcam.max_prefix_size_get(unit));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid key_length=%d \n", key_length_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verifies for a FG of type TCAM DT that its key/actions sizes don't exceed MAX_KEY_DT_SIZE/ACTION_SIZE_HALF respectively.
*
* \param [in] unit - Device ID
* \param [in] key_size_in_bits - Size of key in bits
* \param [in] actions_size_in_bits - size of payload in bits
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
 */
static shr_error_e
dnx_field_group_tcam_dt_sizes_verify(
    int unit,
    uint32 key_size_in_bits,
    uint32 actions_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * TCAM DT is allowed to be configured only with half_action size
     */
    if (actions_size_in_bits > dnx_data_field.tcam.action_size_half_get(unit) ||
        key_size_in_bits > dnx_data_field.tcam.dt_max_key_size_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key or action length bigger than allowed, input_key_size=%d (max allowed: %d), input_actions_size=%d (max allowed %d)for Direct Table TCAM\n",
                     key_size_in_bits, dnx_data_field.tcam.dt_max_key_size_get(unit), actions_size_in_bits,
                     dnx_data_field.tcam.action_size_half_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of TCAM lookup
* \param [in] unit          - Device ID
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in,out] fg_info_p     - info that will be saved in FG SW state
*   As input
*     fg_info_p->field_stage
*     fg_info_p->fg_type
*     fg_info_p->flags
*   As output
*     All the rest
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_tcam_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    unsigned int actions_size_in_bits;
    uint32 tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    uint32 prefix_size = 0;
    uint8 use_hitbit;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_info_in_p, _SHR_E_PARAM, "fg_info_in_p");
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");
    /*
     * Create Key template
     * Note that we verify key size in dnx_field_group_tcam_key_and_action_length_type_calc
     */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_quals,
                     DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET, &fg_info_p->key_template));

    /** fill in_actions_info_arr. Also calculate the number of bits of the given actions.*/
    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, fg_info_p->field_stage, actions_info_arr, &actions_size_in_bits));

    /*
     * For TCAM DT, we just need to verify that sizes don't exceed maximums, and set key/action
     * to DT_KEY_SIZE/HALF_ACTION_SIZE respectively.
     * As for regular TCAM we need to calculate the key/actions sizes needed for it to work.
     */
    if (fg_info_in_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_dt_sizes_verify(unit, fg_info_p->key_template.key_size_in_bits,
                                                             actions_size_in_bits));
        fg_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_DT;
        fg_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF;
        use_hitbit = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_key_and_action_length_type_calc
                        (unit, fg_info_p->key_template.key_size_in_bits,
                         actions_size_in_bits, &fg_info_p->key_length_type, &fg_info_p->action_length_type));
        use_hitbit = TRUE;
    }
    /**Create a dynamic dbal table that will be later used by entry_add to write to HW*/
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_create
                    (unit, fg_id, fg_info_p->field_stage,
                     fg_info_in_p->dnx_quals, fg_info_in_p->qual_is_ranged, actions_info_arr, fg_info_in_p->name,
                     DBAL_TABLE_TYPE_TCAM_BY_ID, DBAL_ACCESS_METHOD_MDB, 0, FALSE, use_hitbit,
                     &fg_info_p->dbal_table_id));
    /**
     * Calculate the maximum prefix bits can be put in the MSB of key
     * to differentiate between field group on same TCAM bank
 */
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_prefix_calc
                    (unit, fg_info_p->key_template.key_size_in_bits, fg_info_p->key_length_type, &prefix_size));
    /** Call TCAM to allocate handler for currently created Field Group*/
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handle_create
                    (unit, fg_id, fg_info_p->field_stage,
                     fg_info_p->fg_type, fg_info_p->key_length_type,
                     prefix_size, fg_info_p->action_length_type, actions_size_in_bits,
                     fg_info_in_p->tcam_info.nof_tcam_banks, fg_info_in_p->tcam_info.bank_allocation_mode,
                     fg_info_in_p->tcam_info.tcam_bank_ids, &tcam_handler_id));
    /**
     * Set the application id of DBAL table, the created TCAM handler,
     * Same done for None PMF stages and hence used same way for saving TCAM handler id
     */
    SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_set(unit, fg_info_p->dbal_table_id, tcam_handler_id));
    /**Fill the payload info.*/
    SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                            fg_info_p->field_stage,
                                                            actions_info_arr,
                                                            fg_info_p->action_length_type,
                                                            actions_size_in_bits,
                                                            fg_info_p->dbal_table_id,
                                                            &fg_info_p->actions_payload_info));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_group_mdb_dt_key_and_action_length_type_calc(
    int unit,
    uint32 key_size_in_bits,
    uint32 actions_size_in_bits,
    dnx_field_key_length_type_e * key_length_type_p,
    dnx_field_action_length_type_e * actions_length_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the key size.
     * For Jr2 max key size is 20,19,18 according to payload size 30,60,120 respectively
     */
    if (key_size_in_bits > dnx_data_field.map.key_size_get(unit) - dnx_data_mdb.direct.map_payload_size_get(unit) / 60)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key size=%d for MDB DT is not supported. Maximum supported key size is %d.\r\n",
                     key_size_in_bits, dnx_data_field.map.key_size_get(unit));
    }
    /*
     * Verify payload
     */
    if (actions_size_in_bits > dnx_data_mdb.direct.map_payload_size_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Result size=%d for MDB DT is not supported. Currently configured maximumm result size is %d.\r\n",
                     actions_size_in_bits, dnx_data_mdb.direct.map_payload_size_get(unit));
    }

    /*
     * Set the key and action types.
     */
    *key_length_type_p = DNX_FIELD_KEY_LENGTH_TYPE_DT;
    *actions_length_type_p = DNX_FIELD_ACTION_LENGTH_TYPE_MDB_DT;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of MDB DT
*  Uses direct table of MDB.
* \param [in] unit          - Device ID
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in] fg_info_p     - info that will be saved in FG SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_mdb_dt_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    unsigned int actions_size_in_bits;
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dbal_table_mdb_access_info_t access_info = { 0 };
    int max_capacity;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create Key template
     * Note that we verify key size in dnx_field_group_mdb_dt_key_and_action_length_type_calc
     */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_quals,
                     DNX_FIELD_KEY_MDB_DT_STARTING_OFFSET(unit), &fg_info_p->key_template));

    /** fill in_actions_info_arr. Also calculate the number of bits of the given actions.*/
    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, fg_info_p->field_stage, actions_info_arr, &actions_size_in_bits));

    SHR_IF_ERR_EXIT(dnx_field_group_mdb_dt_key_and_action_length_type_calc
                    (unit, fg_info_p->key_template.key_size_in_bits, actions_size_in_bits,
                     &fg_info_p->key_length_type, &fg_info_p->action_length_type));

    /**Create a dynamic DBAL table that will be later used by entry_add to write to HW*/
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_create
                    (unit, fg_id, fg_info_p->field_stage,
                     fg_info_in_p->dnx_quals, fg_info_in_p->qual_is_ranged, actions_info_arr, fg_info_in_p->name,
                     DBAL_TABLE_TYPE_DIRECT, DBAL_ACCESS_METHOD_MDB, 0, FALSE, FALSE, &fg_info_p->dbal_table_id));

    access_info.nof_physical_tables = 1;
    access_info.tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
    access_info.app_id = 0;
    access_info.physical_db_id[0] = DBAL_PHYSICAL_TABLE_MAP;
    access_info.app_id_size = 0;
    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set
                    (unit, fg_info_p->dbal_table_id, DBAL_ACCESS_METHOD_MDB, &access_info));

    SHR_IF_ERR_EXIT(mdb_get_capacity(unit, DBAL_PHYSICAL_TABLE_MAP, &max_capacity));
    if ((1 << fg_info_p->key_template.key_size_in_bits) > max_capacity)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Key size %d for MDB DT FG is larger than what the max capacity (%d) can accomodate\r\n",
                     fg_info_p->key_template.key_size_in_bits, max_capacity);
    }

    /**Fill the payload info.*/
    SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                            fg_info_p->field_stage,
                                                            actions_info_arr,
                                                            fg_info_p->action_length_type,
                                                            actions_size_in_bits,
                                                            fg_info_p->dbal_table_id,
                                                            &fg_info_p->actions_payload_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of EXEM
*  Uses Exact match of MDB.
* \param [in] unit          - Device ID
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in] fg_info_p     - info that will be saved in FG SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_exem_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    dnx_field_app_db_id_t app_db_id = DNX_FIELD_APP_DB_ID_INVALID;
    unsigned int actions_size_in_bits;
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_group_exem_info_t exem_info;
    int max_capacity;
    SHR_FUNC_INIT_VARS(unit);
    /** Find the EXEM type (SEXEM or LEXEM and it's properties.*/
    SHR_IF_ERR_EXIT(dnx_field_group_exem_info(unit, fg_info_p->field_stage, &exem_info));
    /** Verify that the EXEM table has enough capacity fro EXEM.*/
    SHR_IF_ERR_EXIT(mdb_get_capacity(unit, exem_info.dbal_physical_table_id, &max_capacity));
    if (max_capacity < DNX_FIELD_EXEM_MINIMUM_CAPACITY)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Physocal EXEM table %d has capacity %d. MDB profile does not support exact match field group.\n",
                     exem_info.dbal_physical_table_id, max_capacity);
    }
    /*
     * Create Key template
     * Note that we verify key size in dnx_field_group_exem_key_and_action_length_type_calc
     */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_quals,
                     DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET, &fg_info_p->key_template));

    /** fill in_actions_info_arr. Also calculate the number of bits of the given actions.*/
    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, fg_info_p->field_stage, actions_info_arr, &actions_size_in_bits));

    SHR_IF_ERR_EXIT(dnx_field_group_exem_key_and_action_length_type_calc
                    (unit, fg_info_p->field_stage,
                     fg_info_p->key_template.key_size_in_bits, actions_size_in_bits, &exem_info,
                     &fg_info_p->key_length_type, &fg_info_p->action_length_type));
    /**Create a dynamic DBAL table that will be later used by entry_add to write to HW*/
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_create
                    (unit, fg_id, fg_info_p->field_stage,
                     fg_info_in_p->dnx_quals, fg_info_in_p->qual_is_ranged, actions_info_arr, fg_info_in_p->name,
                     DBAL_TABLE_TYPE_EM, DBAL_ACCESS_METHOD_MDB, 0, FALSE, FALSE, &fg_info_p->dbal_table_id));
    /** Allocate app_db_id for Field Group.*/
    SHR_IF_ERR_EXIT(dnx_field_group_exem_app_db_id_alloc(unit, fg_info_p->field_stage, &app_db_id));
    /** For an MDB tables, configure the MDB.*/
    SHR_IF_ERR_EXIT(dnx_field_group_exem_dbal_table_access_info_set
                    (unit, app_db_id, fg_info_p->action_length_type, fg_info_p->dbal_table_id));
    /**Fill the payload info.*/
    SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                            fg_info_p->field_stage,
                                                            actions_info_arr,
                                                            fg_info_p->action_length_type,
                                                            actions_size_in_bits,
                                                            fg_info_p->dbal_table_id,
                                                            &fg_info_p->actions_payload_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of State Table.
*
* \param [in] unit          - Device ID
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in] fg_info_p     - info that will be saved in FG SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_state_table_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    int use_acr;
    dnx_field_stage_e field_stage;
    dbal_enum_value_field_field_key_e key_id;
    int lsb_on_key;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_state_table_source_get(unit, &use_acr, &field_stage, &key_id, &lsb_on_key));
    /*
     * Sanity check
     */
    if (use_acr)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "State table actions not supported. \n");
    }

    if (fg_info_p->field_stage != field_stage)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "State table field group can only be configured for stage \"%s\" "
                     "(determined by SOC property). Attampt to configure for stage \"%s\".\r\n",
                     dnx_field_stage_text(unit, field_stage), dnx_field_stage_text(unit, fg_info_p->field_stage));
    }

    /**Create Key template */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_quals, lsb_on_key, &fg_info_p->key_template));
    /** Verify key size*/
    if (fg_info_p->key_template.key_size_in_bits != dnx_data_field.state_table.key_size_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given state stable key size is %d, is different than state table key size (%d)\n",
                     fg_info_p->key_template.key_size_in_bits, dnx_data_field.state_table.key_size_get(unit));
    }

    /*
     * Process the payload.
     * Note this can be ne based on state_table_acr_bus, but we do it based on state_table_atomic_rmw.
     */
    if (dnx_data_field.features.state_table_atomic_rmw_get(unit))
    {
        unsigned int actions_size_in_bits;
        dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

        /** fill in_actions_info_arr. Also calculate the number of bits of the given actions.*/
        SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                        (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                         actions_info_arr));
        SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                        (unit, fg_info_p->field_stage, actions_info_arr, &actions_size_in_bits));
        fg_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_STATE_TABLE;

        /** Verify paylaod size*/
        if (actions_size_in_bits != dnx_data_field.state_table.data_size_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Given state stable payload size (%d) is different than state table payload size (%d)\n",
                         actions_size_in_bits, dnx_data_field.state_table.data_size_get(unit));
        }

        SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                                fg_info_p->field_stage,
                                                                actions_info_arr,
                                                                fg_info_p->action_length_type,
                                                                actions_size_in_bits,
                                                                DBAL_TABLE_EMPTY, &fg_info_p->actions_payload_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of KBP.
* \param [in] unit          - Device ID
* \param [in] flags         - The flags used for adding the field group.
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in,out] fg_info_p - info that will be saved in FG SW state
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_kbp_add(
    int unit,
    dnx_field_group_add_flags_e flags,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    uint32 legal_payload_size;
    unsigned int actions_size_in_bits;
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    uint8 counters_enable = FALSE;
    uint32 initial_capacity;
    int algo_mode;
    int pack_qualifiers;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_info_in_p, _SHR_E_PARAM, "fg_info_in_p");

    DNX_FIELD_GROUP_ID_KBP_VERIFY(fg_id);

    /**Create Key template. This is used ONLY to store the dnx_quals in the sw-state. we don't use the template in KBP. */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_quals,
                     DNX_FIELD_KEY_LOOKUP_STARTING_OFFSET, &fg_info_p->key_template));
    /** Verify key size*/
    if (fg_info_p->key_template.key_size_in_bits >
        dnx_data_field.kbp.max_single_key_size_get(unit) * dnx_data_field.kbp.nof_acl_keys_fg_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given KBP key size (%d) is larger than maximum allowed (%d)",
                     fg_info_p->key_template.key_size_in_bits,
                     dnx_data_field.kbp.max_single_key_size_get(unit) *
                     dnx_data_field.kbp.nof_acl_keys_fg_max_get(unit));
    }

    /*
     * fill in_actions_info_arr. Also calculate the number of bits of the given actions.
     * Note that for a KBP field group, there can be only one action array, so the maximum of all
     */
    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, fg_info_p->field_stage, actions_info_arr, &actions_size_in_bits));

    /** Verify that the payload size is not out of bounds.*/
    SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, actions_size_in_bits, &legal_payload_size));
    /** Verify that the payload size is byte aligned.*/
    if (((actions_size_in_bits) % SAL_UINT8_NOF_BITS) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal payload size %d, must be byte aligned. Pad to %d.\r\n",
                     actions_size_in_bits,
                     ((((actions_size_in_bits) / (SAL_UINT8_NOF_BITS)) + 1)) * SAL_UINT8_NOF_BITS);
    }
    fg_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_KBP;

    pack_qualifiers = ((flags & DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS) != 0);

    /**Create a dynamic dbal table that will be later used by entry_add to write to HW, for KBP ACLs the table type
    is TCAM_DIRECT unlike internal TCAM*/
    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_create
                    (unit, fg_id, fg_info_p->field_stage,
                     fg_info_in_p->dnx_quals, fg_info_in_p->qual_is_ranged, actions_info_arr, fg_info_in_p->name,
                     DBAL_TABLE_TYPE_TCAM_BY_ID, DBAL_ACCESS_METHOD_KBP, SAL_UINT8_NOF_BITS, pack_qualifiers, FALSE,
                     &(fg_info_p->dbal_table_id)));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "dnx_field_group_kbp_add fg_id=%d, stage %s %s%s\n ", fg_id,
                 dnx_field_stage_text(unit, fg_info_p->field_stage), EMPTY, EMPTY);

    /**Fill the payload info.*/
    SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                            fg_info_p->field_stage,
                                                            actions_info_arr,
                                                            fg_info_p->action_length_type,
                                                            actions_size_in_bits,
                                                            fg_info_p->dbal_table_id,
                                                            &fg_info_p->actions_payload_info));

    if (fg_info_p->flags & DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS)
    {
        counters_enable = TRUE;
    }
    /*
     * Call KBP Manager to create the ACL database (cased on the dbal) in the KBP device
     */
    if (fg_info_in_p->kbp_info.min_capacity_valid)
    {
        if (fg_info_in_p->kbp_info.min_capacity == DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Value %d is blocked for min_capacity.\r\n",
                         DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION);
        }
        initial_capacity = fg_info_in_p->kbp_info.min_capacity;
    }
    else
    {
        initial_capacity = DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION;
    }
    if (fg_info_in_p->kbp_info.algorithmic_mode_valid)
    {
        if (fg_info_in_p->kbp_info.algorithmic_mode == DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Value %d is blocked for min_capacity.\r\n",
                         DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION);
        }
        algo_mode = fg_info_in_p->kbp_info.algorithmic_mode;
    }
    else
    {
        algo_mode = DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION;
    }

    SHR_IF_ERR_EXIT(kbp_mngr_db_create(unit, counters_enable, fg_info_p->dbal_table_id, initial_capacity, algo_mode));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of Direct Extraction
*  When action values are taken directly from build KEY
* \param [in] unit          - Device ID
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in,out] fg_info_p -
*   info that will be saved in FG SW state
*   As input, only the following elements are expected to be loaded
*     fg_info_p->field_stage - Stage correspopnding to this field group
*     fg_info_p->fg_type     - Field group type (See 'dnx_field_group_type_e')
*     fg_info_p->flags       - Flags for this field group. (See dnx_field_group_add_flags_e)
*   As output, all other elements. See dnx_field_group_fg_info_for_sw_t.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_de_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    unsigned int actions_size_in_bits;
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    SHR_FUNC_INIT_VARS(unit);
    /** Create Key template. */
    SHR_IF_ERR_EXIT(dnx_field_key_template_create
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_quals,
                     DNX_FIELD_KEY_DE_STARTING_OFFSET, &fg_info_p->key_template));

    /** Verify key size. */
    if (fg_info_p->key_template.key_size_in_bits >
        dnx_data_field.stage.stage_info_get(unit, fg_info_in_p->field_stage)->dir_ext_single_key_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given DE key size (%d) is larger than maximum allowed (%d)",
                     fg_info_p->key_template.key_size_in_bits,
                     dnx_data_field.stage.stage_info_get(unit, fg_info_in_p->field_stage)->dir_ext_single_key_size);
    }
    /** Calculate the key length size*/
    SHR_IF_ERR_EXIT(dnx_field_group_de_key_length_type_calc
                    (unit, fg_info_p->key_template.key_size_in_bits, &fg_info_p->key_length_type));
    /*
     *   Update the action types in actions_payload_info in SW state
     */
    fg_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_DIR_EXT;
    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size
                    (unit, fg_info_p->field_stage, actions_info_arr, &actions_size_in_bits));
    /*
     * Verify that the number of bits used for actions is the same as qualifiers.
     */
    if (fg_info_p->key_template.key_size_in_bits != actions_size_in_bits)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The number of bits on all qualifiers is %u, while the number of bits on all action is %d. "
                     "The two must match.\r\n"
                     "(Note: in case action_with_valid was set to TRUE, which is set by defualt,"
                     "one bit qualifier is required for it)\r\n", fg_info_p->key_template.key_size_in_bits,
                     actions_size_in_bits);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                            fg_info_p->field_stage,
                                                            actions_info_arr,
                                                            fg_info_p->action_length_type,
                                                            actions_size_in_bits,
                                                            DBAL_TABLE_EMPTY, &fg_info_p->actions_payload_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Adds a field group Type of const
*  When there are only actions without input, no key.
* \param [in] unit          - Device ID
* \param [in] fg_id         - Field group id to add
* \param [in] fg_info_in_p  - Field group user input info
* \param [in,out] fg_info_p -
*   info that will be saved in FG SW state
*   As input, only the following elements are expected to be loaded
*     fg_info_p->field_stage - Stage correspopnding to this field group
*     fg_info_p->fg_type     - Field group type (See 'dnx_field_group_type_e')
*     fg_info_p->flags       - Flags for this field group. (See dnx_field_group_add_flags_e)
*   As output, all other elements. See dnx_field_group_fg_info_for_sw_t.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_group_const_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_fg_info_for_sw_t * fg_info_p)
{
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];

    SHR_FUNC_INIT_VARS(unit);
    /*
     *   Update the action types in actions_payload_info in SW state
     */
    fg_info_p->key_length_type = DNX_FIELD_KEY_LENGTH_TYPE_NONE;
    fg_info_p->action_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_NONE;

    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, fg_info_p->field_stage, fg_info_in_p->dnx_actions, fg_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_actions_template_create(unit,
                                                            fg_info_p->field_stage,
                                                            actions_info_arr,
                                                            fg_info_p->action_length_type,
                                                            0, DBAL_TABLE_EMPTY, &fg_info_p->actions_payload_info));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function, per FG_ID, will check which of the Qualifiers/Action have CLASS_USER.
*  In case the FG_ID uses User Qualifier/Action (CLASS_USER), the reference will be incremented
* \param [in] unit   - Device ID
* \param [in] fg_id  - FG_ID that need to check if uses User Qualifier/Action
* \return
*   shr_error_e - Error Type
* \remark
*   Each time field group created with User define Qualifier/Action
*   need to increment the reference count to that Qualifier/Action
*   This is done to prevent User Qualifier/Action deletion until there is at least one FG that still uses it
* \see
*   * None
*/
static shr_error_e
dnx_field_group_user_qual_action_reference_inc(
    int unit,
    dnx_field_group_t fg_id)
{
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int qual_ndx;
    unsigned int action_ndx;
    unsigned int nof_quals;
    unsigned int nof_actions;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

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

    for (qual_ndx = 0; qual_ndx < nof_quals; qual_ndx++)
    {
        if (DNX_QUAL_CLASS(dnx_quals[qual_ndx]) == DNX_FIELD_QUAL_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_qual_ref_inc(unit, dnx_quals[qual_ndx]));
        }
    }

    for (action_ndx = 0; action_ndx < nof_actions; action_ndx++)
    {
        if (DNX_ACTION_CLASS(dnx_actions[action_ndx]) == DNX_FIELD_ACTION_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_action_ref_inc(unit, dnx_actions[action_ndx]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function, per FG_ID, will check which of the Qualifiers/Action have CLASS_USER.
*  In case the FG_ID uses User Qualifier/Action (CLASS_USER), the reference will be incremented
* \param [in] unit   - Device ID
* \param [in] fg_id  - FG_ID that need to check if uses User Qualifier/Action
* \return
*   shr_error_e - Error Type
* \remark
*   Each time field group deleted and uses User define Qualifier/Action
*   need to decrement the reference count to that Qualifier/Action
*   This is done to know when User Qualifier/Action can be destroyed (i.e. when reference reach 0)
* \see
*   * None
*/
static shr_error_e
dnx_field_group_user_qual_action_reference_dec(
    int unit,
    dnx_field_group_t fg_id)
{
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int qual_ndx;
    unsigned int action_ndx;
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

    for (qual_ndx = 0; qual_ndx < nof_quals; qual_ndx++)
    {
        if (DNX_QUAL_CLASS(dnx_quals[qual_ndx]) == DNX_FIELD_QUAL_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_qual_ref_dec(unit, dnx_quals[qual_ndx]));
        }
    }

    for (action_ndx = 0; action_ndx < nof_actions; action_ndx++)
    {
        if (DNX_ACTION_CLASS(dnx_actions[action_ndx]) == DNX_FIELD_ACTION_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_action_ref_dec(unit, dnx_actions[action_ndx]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function, Per ACE ID, will check which of the action have CLASS_USER.
*  In case the ACE ID uses User Action (CLASS_USER), the reference will be incremented
* \param [in] unit   - Device ID
* \param [in] ace_id - ACE ID that need to check if uses User Actions
* \return
*   shr_error_e - Error Type
* \remark
*   Each time field group/ACE format created with User define Qualifier/Action
*   need to increment the reference count to that Qualifier/Action
*   This is done to prevent User Qualifier/Action deletion until there is at least one FG that still uses it
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_user_action_reference_inc(
    int unit,
    dnx_field_ace_id_t ace_id)
{
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    int action_ndx;
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

    for (action_ndx = 0; action_ndx < nof_actions; action_ndx++)
    {
        if (DNX_ACTION_CLASS(dnx_actions[action_ndx]) == DNX_FIELD_ACTION_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_action_ref_inc(unit, dnx_actions[action_ndx]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function, Per ACE ID, will check which of the Action have CLASS_USER.
*  In case the FG_ID uses User Action (CLASS_USER), the reference will be incremented
* \param [in] unit   - Device ID
* \param [in] ace_id - ACE ID that need to check if uses User Actions
* \return
*   shr_error_e - Error Type
* \remark
*   Each time field group/ACE format deleted and uses User define Qualifier/Action
*   need to decrement the reference count to that Qualifier/Action
*   This is done to know when User Qualifier/Action can be destroyed (i.e. when reference reach 0)
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_user_action_reference_dec(
    int unit,
    dnx_field_ace_id_t ace_id)
{
    dnx_field_action_t dnx_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    int action_ndx;
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

    for (action_ndx = 0; action_ndx < nof_actions; action_ndx++)
    {
        if (DNX_ACTION_CLASS(dnx_actions[action_ndx]) == DNX_FIELD_ACTION_CLASS_USER)
        {
            SHR_IF_ERR_EXIT(dnx_field_action_ref_dec(unit, dnx_actions[action_ndx]));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_add(
    int unit,
    dnx_field_group_add_flags_e flags,
    dnx_field_group_info_t * fg_info_in_p,
    dnx_field_group_t * fg_id_p)
{
    dnx_field_group_fg_info_for_sw_t *fg_info_p = NULL;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate memory for fg_info_p, if on stack causes coverity error.*/
    SHR_ALLOC(fg_info_p, sizeof(*fg_info_p), "fg_info_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_group_add_verify(unit, fg_id_p, flags, fg_info_in_p));
    SHR_IF_ERR_EXIT(dnx_field_group_id_alloc(unit, flags, fg_id_p));
    SHR_IF_ERR_EXIT(dnx_field_group_fg_info_for_sw_t_init(unit, fg_info_p));
    /** Copy parameters From input that we would like to save in SW State*/
    fg_info_p->field_stage = fg_info_in_p->field_stage;
    fg_info_p->fg_type = fg_info_in_p->fg_type;
    fg_info_p->flags = flags;

    /** Set the name of the field group to the SW state */
    sal_strncpy_s(fg_info_p->group_name.value, fg_info_in_p->name, sizeof(fg_info_p->group_name.value));

    switch (fg_info_p->fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_add(unit, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_de_add(unit, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_const_add(unit, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_exem_add(unit, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_state_table_add(unit, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_kbp_add(unit, flags, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_mdb_dt_add(unit, *fg_id_p, fg_info_in_p, fg_info_p));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Group type = %d \n", fg_info_p->fg_type);
            break;
        }
    }

    /** Update SW state*/
    SHR_IF_ERR_EXIT(dnx_field_group_sw_state_update(unit, *fg_id_p, fg_info_p));
    /**
     * Each time field group created with User define Qualifier/Action
     * need to increment the reference count to that Qualifier/Action
     * This is done to prevent User Qualifier/Action deletion until there is at least one FG that still uses it
     * This is done based on FG SW state, hence needs to be invoked after the SW was set
 */
    SHR_IF_ERR_EXIT(dnx_field_group_user_qual_action_reference_inc(unit, *fg_id_p));
exit:
    SHR_FREE(fg_info_p);
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_delete(
    int unit,
    dnx_field_group_t fg_id)
{
    dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    dnx_field_group_fg_info_for_sw_t *fg_info_p = NULL;
    uint32 tcam_handler_id;
    dbal_tables_e dbal_table_id;
    dnx_field_group_type_e fg_type;
    dnx_field_app_db_id_t app_db_id;
    int app_db_size;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    /** Allocate memory for fg_info_p, if on stack causes coverity error.*/
    SHR_ALLOC(fg_info_p, sizeof(*fg_info_p), "fg_info_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);
    {
        /*
         * Do not delete a field group which is marked as 'secondary' on an active FEM.
         */
        dnx_field_fem_id_t fem_id;

        SHR_IF_ERR_EXIT(dnx_field_fem_is_fg_secondary_on_any(unit, fg_id, &fem_id));
        if (fem_id != DNX_FIELD_FEM_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "This field group (%d) is marked as 'secondary' on fem_id %d.\r\n"
                         "==> Before deleting, you need to remove this FEM.\r\n", fg_id, fem_id);
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id, context_id_arr));
    if (context_id_arr[0] != DNX_FIELD_CONTEXT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Before deleting Field Group=%d need to detach all contexts\n", fg_id);
    }

    /** Remove all FEMs used by the field group.*/
    SHR_IF_ERR_EXIT(dnx_field_group_fems_group_delete(unit, fg_id));

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    /** Destroy TCAM Handler*/
    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM || fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_destroy(unit, tcam_handler_id));
    }

    /** Deallocate the APP_DB_ID for EXEM.*/
    if (fg_type == DNX_FIELD_GROUP_TYPE_EXEM)
    {
        /*
         * We only get the stage inside the if because it isn't used anywhere else in the function.
         * Should field_stage be used anywhere outside the if, the following function will need to be
         * taken out of the if.
         */
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));
        SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_size, &app_db_id));
        SHR_IF_ERR_EXIT(dnx_field_group_exem_app_db_id_dealloc(unit, field_stage, app_db_id));
    }

    /*
     * Destroy DBAL table, exclude DE/State Table FGs since they don't create DBAL table.
     */
    if ((fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION) && (fg_type != DNX_FIELD_GROUP_TYPE_STATE_TABLE)
        && (fg_type != DNX_FIELD_GROUP_TYPE_CONST))
    {
        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, dbal_table_id));
    }

    /** Decrement the reference, since uses FG SW state HAVE to be invoked before SW state free*/
    SHR_IF_ERR_EXIT(dnx_field_group_user_qual_action_reference_dec(unit, fg_id));
    /** Deallocate FG ID*/
    SHR_IF_ERR_EXIT(dnx_algo_field_group_id_deallocate(unit, fg_id));
    /** Free SW state - Update SW state with init values*/
    SHR_IF_ERR_EXIT(dnx_field_group_fg_info_for_sw_t_init(unit, fg_info_p));
    SHR_IF_ERR_EXIT(dnx_field_group_sw_state_update(unit, fg_id, fg_info_p));
exit:
    SHR_FREE(fg_info_p);
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_basic_info_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_info_t * fg_basic_info_p)
{
    uint8 is_allocated;
    unsigned int nof_quals;
    unsigned int nof_actions;
    int core = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_basic_info_p, _SHR_E_PARAM, "fg_basic_info_p");

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &(fg_basic_info_p->fg_type)));
    
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_arr_get(unit, fg_id, fg_basic_info_p->dnx_quals, &nof_quals));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_qual_is_ranged_arr_get
                    (unit, fg_id, fg_basic_info_p->qual_is_ranged, &nof_quals));
    SHR_IF_ERR_EXIT(dnx_field_group_dnx_action_arr_get(unit, fg_id, fg_basic_info_p->dnx_actions, &nof_actions));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &(fg_basic_info_p->field_stage)));
    SHR_IF_ERR_EXIT(dnx_field_group_name_get(unit, fg_id, fg_basic_info_p->name));
   /**
     * In case of TCAM field group retrieve the TCAM info.
     */
    if (fg_basic_info_p->fg_type == DNX_FIELD_GROUP_TYPE_TCAM ||
        fg_basic_info_p->fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        int nof_banks = 0;
        int bank_id_iter;
        uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS];
        uint32 banks_bmp[1];
        uint32 tcam_handler_id;
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
        /*
         * Get TCAM info
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info
                        (unit, core, tcam_handler_id, bank_entries_count, banks_bmp));
        for (bank_id_iter = 0; bank_id_iter < DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS; bank_id_iter++)
        {
            if ((SHR_BITGET(banks_bmp, bank_id_iter)))
            {
                fg_basic_info_p->tcam_info.tcam_bank_ids[nof_banks++] = bank_id_iter;
            }
        }
        fg_basic_info_p->tcam_info.nof_tcam_banks = nof_banks;
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_allocation_mode_get
                        (unit, tcam_handler_id, &(fg_basic_info_p->tcam_info.bank_allocation_mode)));
    }

    /*
     * In case of EXTERNAL TCAM field group retrieve the KBP info.
     */
    if (fg_basic_info_p->fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        dbal_tables_e dbal_table_id;
        uint8 counters_enable;
        uint32 initial_capacity;
        int algo_mode;
        dnx_field_group_add_flags_e flags;

        SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
        SHR_IF_ERR_EXIT(kbp_mngr_db_properties_get(unit, dbal_table_id,
                                                   &counters_enable, &initial_capacity, &algo_mode));
        if (initial_capacity == DNX_KBP_DB_PROPERTIES_MIN_CAPACITY_DEFAULT_INDICATION)
        {
            fg_basic_info_p->kbp_info.min_capacity_valid = FALSE;
        }
        else
        {
            fg_basic_info_p->kbp_info.min_capacity_valid = TRUE;
            fg_basic_info_p->kbp_info.min_capacity = initial_capacity;
        }
        if (algo_mode == DNX_KBP_DB_PROPERTIES_ALGORITHMIC_MODE_DEFAULT_INDICATION)
        {
            fg_basic_info_p->kbp_info.algorithmic_mode_valid = FALSE;
        }
        else
        {
            fg_basic_info_p->kbp_info.algorithmic_mode_valid = TRUE;
            fg_basic_info_p->kbp_info.algorithmic_mode = algo_mode;
        }
        /** Sanity check.*/
        SHR_IF_ERR_EXIT(dnx_field_group_flags_get(unit, fg_id, &flags));
        if (((flags & DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS) && (counters_enable == FALSE)) ||
            (((flags & DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS) == 0) && counters_enable))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mismatch between external counters flag (0x%x) and DBAL information "
                         "about counters_enable (%d). \n",
                         (flags & DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS), counters_enable);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_group_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_full_info_t * fg_info_p)
{
    uint8 is_allocated;
    int app_db_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fg_info_p, _SHR_E_PARAM, "fg_info_p");

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }

    /**
      *  Fill in the  basic information for Field Group
      *  (given in field_goup_add function as defined in dnx_field_group_info_t)
      */
    SHR_IF_ERR_EXIT(dnx_field_group_basic_info_get(unit, fg_id, &(fg_info_p->group_basic_info)));
    /**
      * Fill in the detailed information for Field Group (retrieved from SW-state)
      * as defined in dnx_field_group_fg_info_for_sw_t
      */
    SHR_IF_ERR_EXIT(dnx_field_group_sw_state_get(unit, fg_id, &(fg_info_p->group_full_info)));
    /**
     * In case of TCAM field group retrieve the TCAM handler-ID and the access profile-ID (APP_DB_ID) for the
     * Field group
     */

    switch (fg_info_p->group_basic_info.fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &(fg_info_p->tcam_handler_id)));
            SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_size, &(fg_info_p->app_db_id)));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            /** In the case of EXEM, only get the APP_DB_ID.*/
            fg_info_p->tcam_handler_id = DNX_FIELD_TCAM_HANDLER_ID_INVALID;
            SHR_IF_ERR_EXIT(dnx_field_group_app_db_id_get(unit, fg_id, &app_db_size, &(fg_info_p->app_db_id)));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        case DNX_FIELD_GROUP_TYPE_CONST:
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        case DNX_FIELD_GROUP_TYPE_KBP:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        default:
        {
            fg_info_p->tcam_handler_id = DNX_FIELD_TCAM_HANDLER_ID_INVALID;
            fg_info_p->app_db_id = FIELD_TCAM_ACCESS_PROFILE_ID_INVALID;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function will check the validity of parameters for dnx_field_ace_format_add()
* \param [in] unit                - Device ID
* \param [in] flags               - Can be WITH_ID
* \param [in] ace_format_info_in_p - All the information required to configure the ACE format.
*                                   Includes an array of actions that the ACE format can perform.
* \param [in] ace_id_p            - ID of the ACE format.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_ace_format_add_verify(
    int unit,
    dnx_field_ace_format_add_flags_e flags,
    dnx_field_ace_format_info_t * ace_format_info_in_p,
    dnx_field_ace_id_t * ace_id_p)
{
    unsigned int action_count;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_format_info_in_p, _SHR_E_PARAM, "ace_format_info_in_p");
    SHR_NULL_CHECK(ace_id_p, _SHR_E_PARAM, "ace_id_p");
    if ((flags & (~DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported flags: 0x%08hhX\n", flags & (~DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID));
    }

    /*
     * Verify that the maximum number of actions does not reach DNX_DATA_MAX_FIELD_ACE_NOF_ACTION_PER_ACE_FORMAT,
     * for function reuse. Also check that the number of actions does not exceed the number of FESes in ACE ACR.
     */
    for (action_count = 0; action_count < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP; action_count++)
    {
        if (ace_format_info_in_p->dnx_actions[action_count] == DNX_FIELD_ACTION_INVALID)
        {
            break;
        }
    }
    if (action_count > dnx_data_field.ace.nof_action_per_ace_format_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of actions for ACE format cannot exceed %d.\n",
                     dnx_data_field.ace.nof_action_per_ace_format_get(unit));
    }
    if (action_count > dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_ACE)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of actions for ACE format cannot exceed the number of EFESes %d.\n",
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_ACE)->nof_fes_instruction_per_context);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_group.h*/
shr_error_e
dnx_field_ace_format_add(
    int unit,
    dnx_field_ace_format_add_flags_e flags,
    dnx_field_ace_format_info_t * ace_format_info_in_p,
    dnx_field_ace_id_t * ace_id_p)
{
    unsigned int actions_size_in_bits = 0;
    dnx_field_action_length_type_e actions_length_type = DNX_FIELD_ACTION_LENGTH_TYPE_INVALID;
    dnx_field_action_in_group_info_t actions_info_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    dnx_field_ace_format_info_for_sw_t ace_format_info;
    dnx_field_stage_e field_stage;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify input params*/
    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_format_add_verify(unit, flags, ace_format_info_in_p, ace_id_p));
    field_stage = DNX_FIELD_STAGE_ACE;

    /** Allocate ace_id*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_id_alloc(unit, flags, ace_id_p));
    /** fill in_actions_info_arr. Also calculate the number of bits of the given actions.*/
    SHR_IF_ERR_EXIT(dnx_field_group_fill_action_info_arr_dnx_action_type
                    (unit, field_stage, ace_format_info_in_p->dnx_actions, ace_format_info_in_p->use_valid_bit,
                     actions_info_arr));
    SHR_IF_ERR_EXIT(dnx_field_group_calc_actions_bit_size(unit, field_stage, actions_info_arr, &actions_size_in_bits));

    SHR_IF_ERR_EXIT(dnx_field_ace_format_action_length_type_calc(unit, actions_size_in_bits, &actions_length_type));

    /** Configure the new ACE ID in DBAL and MDB. Parallal to creating a dynamic table for a TCAM field group.*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_dbal_result_type_add
                    (unit, *ace_id_p, actions_info_arr, ace_format_info_in_p->name));
    /**Note we do not have a 'find_field_io' function for ace, as there is only one option.*/
    /**Fill the locations of the actions on the payload.*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_actions_template_create(unit,
                                                                 field_stage,
                                                                 *ace_id_p,
                                                                 actions_info_arr,
                                                                 actions_length_type,
                                                                 actions_size_in_bits,
                                                                 &(ace_format_info.actions_payload_info)));
    /** Configure FES*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_fes_config
                    (unit, *ace_id_p, DBAL_ENUM_FVAL_FIELD_IO_ACE, &(ace_format_info.actions_payload_info)));

    sal_strncpy_s(ace_format_info.name, ace_format_info_in_p->name, sizeof(ace_format_info.name));

    /** Update SW state*/
    ace_format_info.flags = flags;
    SHR_IF_ERR_EXIT(dnx_field_ace_format_sw_state_update(unit, *ace_id_p, &ace_format_info));
    /**
     * Each time field/ACE format created with User define Qualifier/Action
     * need to increment the reference count to that Qualifier/Action
     * This is done to prevent User Qualifier/Action deletion until there is at least one field/ACE format that still uses it
     * This is done based on field/ACE format SW state, hence needs to be invoked after the SW was set.
     * ACE formats only handle user defined actions, not qualifiers.
 */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_user_action_reference_inc(unit, *ace_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_ace_format_delete(
    int unit,
    dnx_field_ace_id_t ace_id)
{
    dnx_field_ace_format_info_for_sw_t ace_format_info;
    int result_type_index;
    SHR_FUNC_INIT_VARS(unit);
    
    /*
     * Delete the FES configuration for the ACE format.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_fes_ace_detach(unit, ace_id));
    /*
     * Destroy DBAL configuration for the ACE format.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_result_type_index_get(unit, ace_id, &result_type_index));
    SHR_IF_ERR_EXIT(dbal_tables_dynamic_result_type_delete
                    (unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, result_type_index));
    /*
     * Decrement the reference to user defined actions,
     * since it uses ACE SW state, so it needs to be invoked before the SW state is freed.
     */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_user_action_reference_dec(unit, ace_id));
    /*
     * Deallocate ACE ID.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_deallocate(unit, ace_id));
    /** Free SW state - Update SW state with init values*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_for_sw_t_init(unit, &ace_format_info));
    SHR_IF_ERR_EXIT(dnx_field_ace_format_sw_state_update(unit, ace_id, &ace_format_info));
exit:
    SHR_FUNC_EXIT;
}

/**
* See field_group.h
*/
shr_error_e
dnx_field_ace_format_get(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_field_ace_format_full_info_t * ace_id_info_p)
{
    uint8 is_allocated;
    unsigned int nof_actions;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ace_id_info_p, _SHR_E_PARAM, "ace_id_info_p");
    SHR_IF_ERR_EXIT(dnx_algo_field_ace_id_is_allocated(unit, ace_id, &is_allocated));
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ACE ID %d does not exist.\n", ace_id);
    }

    /** Initialize the output structure for predictability.*/
    sal_memset(ace_id_info_p, 0, sizeof(*ace_id_info_p));

    
    SHR_IF_ERR_EXIT(dnx_field_ace_format_dnx_action_arr_get
                    (unit, ace_id, ace_id_info_p->format_basic_info.dnx_actions, &nof_actions));

    /**
      * Fill in the detailed information for Field Group (retrieved from SW-state)
      * as defined in dnx_field_group_fg_info_for_sw_t
      */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_sw_state_get(unit, ace_id, &(ace_id_info_p->format_full_info)));
    sal_strncpy_s(ace_id_info_p->format_basic_info.name, ace_id_info_p->format_full_info.name,
                  sizeof(ace_id_info_p->format_basic_info.name));
    /*
     * Collect FES info from sw state and from HW.
     */
    SHR_IF_ERR_EXIT(dnx_field_actions_ace_id_fes_info_get(unit, ace_id, &(ace_id_info_p->actions_fes_info)));

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_group.h*/
shr_error_e
dnx_field_group_entry_first(
    int unit,
    int core,
    dnx_field_group_t fg_id,
    uint32 *first_entry_handle)
{
    dnx_field_group_type_e fg_type;
    uint32 tcam_handler_id;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
        /*
         * Entry handle is same as entry id returned by tcam_handler_entry_first() function
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_first(unit, core, tcam_handler_id, first_entry_handle));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This function does not support field groups of type different than TCAM, fg_id: %d", fg_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**See H in field_group.h*/
shr_error_e
dnx_field_group_entry_next(
    int unit,
    int core,
    dnx_field_group_t fg_id,
    uint32 cur_entry_handle,
    uint32 *next_entry_handle)
{
    dnx_field_group_type_e fg_type;
    uint32 tcam_handler_id;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
        /*
         * Entry handle is same as entry id returned by tcam_handler_entry_next() function
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_next
                        (unit, core, tcam_handler_id, cur_entry_handle, next_entry_handle));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "This function does not support field groups of type different than TCAM, fg_id: %d", fg_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/**See H in field_group.h*/
shr_error_e
dnx_field_group_entry_delete_all(
    int unit,
    dnx_field_group_t fg_id)
{
    dnx_field_group_type_e fg_type;
    dbal_tables_e dbal_table_id;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete_all(unit, fg_id));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table_id));
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "This function does not support the given field group type %d, fg_id: %d",
                         fg_type, fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See H in field_group.h*/
shr_error_e
dnx_field_group_all_entry_delete_all(
    int unit)
{
    dnx_field_group_type_e fg_type;
    dnx_field_group_t fg_id;
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    for (fg_id = 0; fg_id < dnx_data_field.group.nof_fgs_get(unit); fg_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
        if (is_allocated == FALSE)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
        /*
         * Direct extraction doesn't have any entries.
         * State table delete all entries is performed on all state table field groups.
         */
        if ((fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION) || (fg_type == DNX_FIELD_GROUP_TYPE_CONST) ||
            (fg_type == DNX_FIELD_GROUP_TYPE_STATE_TABLE))
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_entry_delete_all(unit, fg_id));
    }

    /*
     * Delete all entries on the state table
     */
    SHR_IF_ERR_EXIT(dnx_field_entry_state_table_all_delete_all(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_group_tcam_entries_info(
    int unit,
    dnx_field_group_t fg_id,
    uint32 bank_entries_count[DNX_DATA_MAX_FIELD_TCAM_NOF_BANKS],
    uint32 *banks_bmp_p)
{
    dnx_field_group_type_e fg_type;
    uint32 tcam_handler_id;
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_GROUP_ID_VERIFY(fg_id);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));
    if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "This function only supports field groups of type TCAM");
    }
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
    if (tcam_handler_id == DNX_FIELD_TCAM_HANDLER_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error retrieving the TCAM handler for the given field group");
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entries_info(unit, core, tcam_handler_id, bank_entries_count, banks_bmp_p));
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_group.h
 */
shr_error_e
dnx_field_group_action_offset_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_action_t dnx_action,
    int *offset_p)
{
    int action_ndx;
    uint8 found;
    dnx_field_actions_fg_payload_info_t actions_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_action_template_get(unit, fg_id, &actions_info));

    

    /*
     * Search for action in the action list of the FG and return its "lsb" in the payload buffer (AKA offset)
     */
    found = FALSE;
    for (action_ndx = 0;
         action_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         actions_info.actions_on_payload_info[action_ndx].dnx_action != DNX_FIELD_ACTION_INVALID; action_ndx++)
    {
        if (actions_info.actions_on_payload_info[action_ndx].dnx_action == dnx_action)
        {
            if (actions_info.actions_on_payload_info[action_ndx].dont_use_valid_bit)
            {
                *offset_p = actions_info.actions_on_payload_info[action_ndx].lsb;
            }
            else
            {
                *offset_p = actions_info.actions_on_payload_info[action_ndx].lsb + 1;   /* Skip valid bit */
            }
            found = TRUE;
            break;
        }
    }

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Action %s is not present in FG %d\r\n",
                     dnx_field_dnx_action_text(unit, dnx_action), fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_group.h
 */
shr_error_e
dnx_field_group_kbp_segment_indices(
    int unit,
    dnx_field_group_t fg_id,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    kbp_opcode_lookup_info_t lookup_info;
    dnx_field_attach_context_info_t context_info;
    int qual_idx;
    int seg_idx;
    int nof_quals;
    dnx_field_key_template_t key_template;
    uint8 qual_is_packed[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint8 qual_is_last_packed[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG] = { 0 };
    uint8 qual_is_packed_with[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    uint8 qual_offset_within_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG];
    int nof_quals_packed;
    int nof_segments_packed;
    int first_qual_packing;
    int qual_packing_size;
    dbal_tables_e dbal_table_id;
    dbal_fields_e dbal_field_qual;
    uint8 is_qual_packed;
    uint8 is_qual_packed_prev = FALSE;
    dnx_field_stage_e field_stage;
    uint32 qual_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&lookup_info, 0x0, sizeof(lookup_info));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &field_stage));

    SHR_IF_ERR_EXIT(dnx_field_group_context_info_get(unit, fg_id, opcode_id, &context_info));

    lookup_info.result_index = context_info.payload_id;

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_lookup_get(unit, opcode_id, &lookup_info));

    /*
     * Sanity check: count the number of qualifiers on the field group and verify that they are identical to the
     * number of segments on the KBP DB.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_key_template_get(unit, fg_id, &key_template));
    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_template.key_qual_map[qual_idx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++);
    nof_quals = qual_idx;
    if (nof_quals < lookup_info.nof_segments)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d, opcode_id %d, has less qualifers (%d) than segments (%d).\n",
                     fg_id, opcode_id, nof_quals, lookup_info.nof_segments);
    }
    if (lookup_info.nof_segments > DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG ||
        lookup_info.nof_segments > DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of segments on database is %d, but maximum segments on database is %d, "
                     "and maximum qualifiers on field group %d. Field group %d, opcode_id %d.\n",
                     lookup_info.nof_segments, DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP,
                     DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG, fg_id, opcode_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));

    /*
     * Find packed quals
     */
    nof_quals_packed = 0;
    nof_segments_packed = 0;
    first_qual_packing = 0;
    qual_packing_size = 0;
    for (qual_idx = 0; qual_idx < nof_quals; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_to_dbal_qual
                        (unit, DNX_FIELD_STAGE_EXTERNAL, key_template.key_qual_map[qual_idx].qual_type,
                         &dbal_field_qual));
        SHR_IF_ERR_EXIT(dbal_table_field_is_packed_get(unit, dbal_table_id, dbal_field_qual, &is_qual_packed));
        if (is_qual_packed_prev == FALSE)
        {
            first_qual_packing = qual_idx;
            qual_packing_size = 0;
        }
        if (is_qual_packed || is_qual_packed_prev)
        {
            qual_is_packed[qual_idx] = TRUE;
            qual_is_packed_with[qual_idx] = first_qual_packing;
            qual_offset_within_segment[qual_idx] = qual_packing_size;
            nof_quals_packed++;
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size
                            (unit, field_stage, key_template.key_qual_map[qual_idx].qual_type, &qual_size));
            qual_packing_size += qual_size;
        }
        if (is_qual_packed == FALSE && is_qual_packed_prev)
        {
            qual_is_last_packed[qual_idx] = TRUE;
            nof_segments_packed++;
        }
        is_qual_packed_prev = is_qual_packed;
    }

    if ((nof_quals - nof_quals_packed) != (lookup_info.nof_segments - nof_segments_packed))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "fg_id %d, opcode_id %d, has less qualifers (%d,%d) than segments (%d,%d).\n",
                     fg_id, opcode_id, nof_quals, nof_quals_packed, lookup_info.nof_segments, nof_segments_packed);
    }

    /*
     * Swap the order of the indices, as the order of the qualifiers and he segments are inverted.
     */
    for (qual_idx = 0, seg_idx = lookup_info.nof_segments - 1; qual_idx < nof_quals; qual_idx++)
    {
        seg_index_on_master_by_qual[qual_idx] = lookup_info.key_segment_index[seg_idx];
        if (qual_is_packed[qual_idx])
        {
            qual_index_on_segment[qual_idx] = qual_idx - qual_is_packed_with[qual_idx];
            qual_offset_on_segment[qual_idx] = qual_offset_within_segment[qual_idx];
        }
        else
        {
            qual_index_on_segment[qual_idx] = 0;
            qual_offset_on_segment[qual_idx] = 0;
        }
        /** Progress seg_idx when we end packed qualifer bunch, or non packed qualifer.*/
        if (qual_is_packed[qual_idx] == FALSE || qual_is_last_packed[qual_idx])
        {
            seg_idx--;
        }
    }
    for (; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        seg_index_on_master_by_qual[qual_idx] = DNX_KBP_INVALID_SEGMENT_ID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_group.h
 */
shr_error_e
dnx_field_group_kbp_opcode_nof_fwd_segments(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint8 *nof_fwd_segments_p)
{
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_predefined;
    bcm_field_AppType_t apptype_user;
    bcm_field_AppType_t apptype_pre;
    uint8 is_user_defined;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_opcode_is_dynamic(unit, opcode_id, &is_user_defined));
    if (is_user_defined)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype_user));
        SHR_IF_ERR_EXIT(dnx_field_context_user_apptype_to_predef_apptype(unit, apptype_user, &apptype_pre));
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, apptype_pre, &opcode_predefined));
    }
    else
    {
        opcode_predefined = opcode_id;
    }

    SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                    nof_fwd_segments.get(unit, opcode_predefined, nof_fwd_segments_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds FFCs for the filling of zero padding segments.
 *
 * \param [in] unit          - Device Id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_group_kbp_fwd_zero_padding_fill(
    int unit)
{
    dbal_enum_value_field_kbp_fwd_opcode_e predef_opcode;
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_user;
    dnx_field_group_kbp_info_t kbp_opcode_info;
    bcm_field_AppType_t predef_apptype;
    int nof_user_defiend_apptypes;
    bcm_field_AppType_t user_app_types[DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF];
    dbal_enum_value_field_kbp_fwd_opcode_e user_opcodes[DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF];
    int opcode_idx;
    int nof_user_defiend_opcodes_used;
    int seg_idx;
    uint8 segment_is_zero_filled[DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF + 1][DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    dnx_field_context_t contexts_to_share_ffc[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
    int nof_context_to_share_ffc;
    int context_idx;
    uint8 is_valid;
    int shared_ffc_id;
    dnx_field_group_kbp_segment_info_t segment_info;
    uint8 nof_fwd_segments;
    int segemnt_is_zero_padded;
    int predefined_opcode_used;
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_that_uses_zero_padding = DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID;
    kbp_mngr_key_segment_t ms_key_segments[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 nof_segments;
    bcm_field_AppType_t apptype_curr;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_KBP_OPCODE_IN_USE_ITERATOR(predef_opcode)
    {
        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.is_valid.get(unit, predef_opcode, &is_valid));
        /** Check if opcode is used by ACL.*/
        if (is_valid == FALSE)
        {
            predefined_opcode_used = FALSE;
        }
        else
        {
            predefined_opcode_used = TRUE;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, predef_opcode, &predef_apptype));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_to_child_apptypes
                        (unit, predef_apptype, &nof_user_defiend_apptypes, user_app_types));
        /** Sanity check.*/
        if (nof_user_defiend_apptypes > DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_user_defiend_apptypes %d, is bigget than maximum %d.\n",
                         nof_user_defiend_apptypes, DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF);
        }
        nof_user_defiend_opcodes_used = 0;
        for (opcode_idx = 0; opcode_idx < nof_user_defiend_apptypes; opcode_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, user_app_types[opcode_idx], &opcode_user));
            SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.is_valid.get(unit, opcode_user, &is_valid));
            if (is_valid)
            {
                user_opcodes[nof_user_defiend_opcodes_used] = opcode_user;
                nof_user_defiend_opcodes_used++;
            }
            else
            {
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "User defined apptype %d opcode %d was created but no field groups attached to it."
                             "%s%s\n", user_app_types[opcode_idx], opcode_user, EMPTY, EMPTY);
            }
        }
        /*
         * We now have all of the user defined opcodes being used based on the predefined opcode.
         * Go over each qualifier to see if it fills a zero padded segment.
         */
        for (opcode_idx = 0; opcode_idx < DNX_DATA_MAX_FIELD_KBP_APPTYPE_USER_NOF + 1; opcode_idx++)
        {
            for (seg_idx = 0; seg_idx < DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY; seg_idx++)
            {
                segment_is_zero_filled[opcode_idx][seg_idx] = FALSE;
            }
        }
        SHR_IF_ERR_EXIT(dnx_field_group_kbp_opcode_nof_fwd_segments(unit, predef_opcode, &nof_fwd_segments));
        for (opcode_idx = 0; opcode_idx < nof_user_defiend_opcodes_used + 1; opcode_idx++)
        {
            dbal_enum_value_field_kbp_fwd_opcode_e opcode_id_curr;
            if (opcode_idx == 0)
            {
                opcode_id_curr = predef_opcode;
                if (predefined_opcode_used == FALSE)
                {
                    continue;
                }
            }
            else
            {
                opcode_id_curr = user_opcodes[opcode_idx - 1];
            }
            SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.get(unit, opcode_id_curr, &kbp_opcode_info));
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_master_key_get(unit, opcode_id_curr, &nof_segments, ms_key_segments));
            for (seg_idx = 0; seg_idx < nof_fwd_segments; seg_idx++)
            {
                if (kbp_opcode_info.master_key_info.segment_info[seg_idx].fg_id != DNX_FIELD_GROUP_INVALID)
                {
                    segment_is_zero_filled[opcode_idx][seg_idx] = TRUE;
                    if (ms_key_segments[seg_idx].is_overlay_field)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode_id %d segment %d is FWD segment (nof_fwd_segemntd %d) "
                                     "but it uses overlay sharing (offset %d bytes on master key).\n",
                                     opcode_id_curr, seg_idx, nof_fwd_segments,
                                     ms_key_segments[seg_idx].overlay_offset_bytes);
                    }
                }
            }
        }
        /*
         * Now for each segment find an FFC shared by all opcodes, and for each opcode, allocate the FFCs.
         */
        for (seg_idx = 0; seg_idx < nof_fwd_segments; seg_idx++)
        {
            /*
             * Find an FFC used by all contexts that use the zero padding.
             */
            segemnt_is_zero_padded = FALSE;
            nof_context_to_share_ffc = 0;
            for (opcode_idx = 0; opcode_idx < nof_user_defiend_opcodes_used + 1; opcode_idx++)
            {
                dbal_enum_value_field_kbp_fwd_opcode_e opcode_id_curr;
                int nof_fwd_contexts;
                dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
                dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

                if (opcode_idx == 0)
                {
                    opcode_id_curr = predef_opcode;
                }
                else
                {
                    opcode_id_curr = user_opcodes[opcode_idx - 1];
                }

                SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id_curr, &apptype_curr));
                SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_fwd_contexts
                                (unit, apptype_curr, &nof_fwd_contexts, fwd_contexts, acl_contexts));
                for (context_idx = 0; context_idx < nof_fwd_contexts; context_idx++)
                {
                    if (nof_context_to_share_ffc >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Number of contexts that need to share an FFC exceeds maximum %d.\n",
                                     DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS);
                    }
                    if ((nof_context_to_share_ffc > 0) && (context_idx > 0) &&
                        (acl_contexts[context_idx] == contexts_to_share_ffc[nof_context_to_share_ffc - 1]))
                    {
                        continue;
                    }
                    contexts_to_share_ffc[nof_context_to_share_ffc] = acl_contexts[context_idx];
                    nof_context_to_share_ffc++;
                    if (segment_is_zero_filled[opcode_idx][seg_idx])
                    {
                        segemnt_is_zero_padded = TRUE;
                        opcode_that_uses_zero_padding = opcode_id_curr;
                    }
                }
            }
            /*
             * Sanity check: verify that no context appears twice.
             */
            for (context_idx = 0; context_idx < nof_context_to_share_ffc; context_idx++)
            {
                int context_idx_2;
                for (context_idx_2 = 0; context_idx_2 < context_idx; context_idx_2++)
                {
                    if (contexts_to_share_ffc[context_idx] == contexts_to_share_ffc[context_idx_2])
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Predefined opcode %d segment %d, context %d appears twice.\n",
                                     predef_opcode, seg_idx, contexts_to_share_ffc[context_idx]);
                    }
                }
            }

            /*
             * Allocate a shared FFC and configure it for each context, whether is uses zero padding or not.
             */
            if (segemnt_is_zero_padded)
            {
                rv = dnx_field_key_find_common_available_ffc(unit, DNX_FIELD_STAGE_EXTERNAL,
                                                             contexts_to_share_ffc, nof_context_to_share_ffc,
                                                             &shared_ffc_id);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    
                    SHR_ERR_EXIT(_SHR_E_RESOURCE,
                                 "Predefined opcode %d segment %d, cannot find FFC shared by all contexts to "
                                 "fill zero padding.\n", predef_opcode, seg_idx);
                }
                else
                {
                    SHR_IF_ERR_EXIT(rv);
                }
                /*
                 * Allocate FFCs for each opcode.
                 */
                for (opcode_idx = 0; opcode_idx < nof_user_defiend_opcodes_used + 1; opcode_idx++)
                {
                    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id_curr;

                    if (opcode_idx == 0)
                    {
                        opcode_id_curr = predef_opcode;
                    }
                    else
                    {
                        opcode_id_curr = user_opcodes[opcode_idx - 1];
                    }
                    if (segment_is_zero_filled[opcode_idx][seg_idx])
                    {
                        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                                        segment_info.get(unit, opcode_id_curr, seg_idx, &segment_info));
                        SHR_IF_ERR_EXIT(dnx_field_key_kbp_fwd_zero_padding_fill
                                        (unit, opcode_id_curr, seg_idx, &segment_info, TRUE, opcode_id_curr,
                                         shared_ffc_id));
                    }
                    else
                    {
                        /*
                         * If the opcode doesn't need zero padding, configure the FFC by another opcode that does
                         * use zero padding.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.master_key_info.
                                        segment_info.get(unit, opcode_that_uses_zero_padding, seg_idx, &segment_info));
                        SHR_IF_ERR_EXIT(dnx_field_key_kbp_fwd_zero_padding_fill
                                        (unit, opcode_id_curr, seg_idx, &segment_info, FALSE,
                                         opcode_that_uses_zero_padding, shared_ffc_id));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_group_kbp_set(
    int unit)
{
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
    uint8 is_valid;
    dnx_field_group_kbp_info_t kbp_opcode_info;
    bcm_field_AppType_t apptype;
    int nof_fwd_contexts;
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
    uint8 context_used[DNX_DATA_MAX_FIELD_KBP_MAX_ACL_CONTEXT_NUM] = { 0 };
    unsigned int fwd_context_idx;
    uint8 acl_context_is_user_defined;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (DNX_KBP_STATUS_IS_LOCKED(kbp_mngr_status))
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS, "Device lock cannot happen more than once.\n");
    }

    SHR_IF_ERR_EXIT(dnx_field_group_kbp_fwd_zero_padding_fill(unit));

    for (opcode_id = 0; opcode_id < DNX_KBP_MAX_NOF_OPCODES; opcode_id++)
    {
        uint8 context_used_for_opcode[DNX_DATA_MAX_FIELD_KBP_MAX_ACL_CONTEXT_NUM] = { 0 };
        /*
         * Check if opcode is used by ACL.
         */
        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.is_valid.get(unit, opcode_id, &is_valid));
        if (is_valid == FALSE)
        {
            continue;
        }

        /*
         * Get the kbp information for this opcode. kbp info includes the qualifier type
         * and the qualifier attach info for all segments in the master key.
         */
        SHR_IF_ERR_EXIT(dnx_field_group_sw.kbp_info.get(unit, opcode_id, &kbp_opcode_info));
        /*
         * Get Attached ACL (field iFWD2) contexts to this opcode.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype));
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_fwd_contexts
                        (unit, apptype, &nof_fwd_contexts, fwd_contexts, acl_contexts));
        for (fwd_context_idx = 0; fwd_context_idx < nof_fwd_contexts; fwd_context_idx++)
        {
            if (context_used_for_opcode[acl_contexts[fwd_context_idx]])
            {
                SHR_IF_ERR_EXIT(kbp_mngr_context_acl_is_dynamic
                                (unit, acl_contexts[fwd_context_idx], &acl_context_is_user_defined));
                if (acl_context_is_user_defined)
                {
                    continue;
                }
                else
                {
                    /** Sanity check*/
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "ACL context %d is being used by opcode_id %d more than once, "
                                 "even though it is a user define dcontext.\n", acl_contexts[fwd_context_idx],
                                 opcode_id);
                }
            }
            if (context_used[acl_contexts[fwd_context_idx]])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ACL context %d is being used by more than one opcode_id, including %d.\n",
                             acl_contexts[fwd_context_idx], opcode_id);
            }
            /*
             * Install the master key (opcode) for given context.
             */
            SHR_IF_ERR_EXIT(dnx_field_key_kbp_set(unit, opcode_id,
                                                  fwd_contexts[fwd_context_idx], acl_contexts[fwd_context_idx],
                                                  &kbp_opcode_info));
            context_used[acl_contexts[fwd_context_idx]] = TRUE;
            context_used_for_opcode[acl_contexts[fwd_context_idx]] = TRUE;

            LOG_DEBUG_EX(BSL_LOG_MODULE, "Set opcode_id %d on context %d.%s%s\n", opcode_id,
                         acl_contexts[fwd_context_idx], EMPTY, EMPTY);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_group_tcam_bank_evacuate(
    int unit,
    dnx_field_group_t fg_id,
    int nof_banks,
    int bank_ids[])
{
    uint32 tcam_handler_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_evacuate(unit, tcam_handler_id, nof_banks, bank_ids));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_group_tcam_bank_add(
    int unit,
    dnx_field_group_t fg_id,
    int nof_banks,
    int bank_ids[])
{
    uint32 tcam_handler_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_id_get(unit, fg_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_add(unit, tcam_handler_id, nof_banks, bank_ids));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_group_tcam_entry_fg_id(
    int unit,
    bcm_field_entry_t entry_id,
    bcm_field_group_t * fg_id)
{
    uint32 tcam_handler_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_handler_entry_handler(unit, entry_id, &tcam_handler_id));
    SHR_IF_ERR_EXIT(dnx_field_group_tcam_handler_to_fg_id(unit, tcam_handler_id, fg_id));

exit:
    SHR_FUNC_EXIT;
}
