
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * $Copyright:.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX



#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_action_access.h>






typedef struct
{
    
    int place_in_alloc;
    
    dnx_field_action_priority_t priority;
    
    dnx_field_group_t fg_id;
    
    unsigned int place_in_fg;
    
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    
    dnx_field_action_type_t fes_action_type[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
} dnx_algo_field_action_fes_alloc_fes_quartet_t;


typedef struct
{
    
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    
        dnx_algo_field_action_fes_alloc_fes_quartet_t
        fes_quartet_info[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    
    uint8 is_shared[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
} dnx_algo_field_action_fes_alloc_current_state_inclusive_t;


typedef enum
{
    
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS,
    
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS,
    
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_TIE,
    
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE,
    
    DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_NOF
} dnx_algo_field_action_fes_alloc_priority_compare_result_e;






#define DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(_unit,_dnx_stage) \
    (((_dnx_stage == DNX_FIELD_STAGE_IPMF1) || (_dnx_stage == DNX_FIELD_STAGE_IPMF2)) ? \
     ("IPMF1/2") : (dnx_field_stage_text(_unit, _dnx_stage)))





static shr_error_e dnx_algo_field_action_fes_allocate_non_mandatory_position(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    int allow_contention_moving,
    int allow_non_contention_moving,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p);





shr_error_e
dnx_algo_field_action_pmf_a_fem_pgm_id_allocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    int alloc_flags,
    dnx_field_fem_program_t * fem_program_p)
{
    int alloc_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fem_program_p, _SHR_E_PARAM, "fem_program_p");
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    alloc_id = 0;
    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_pgm_id.allocate_single(unit, fem_id, alloc_flags, NULL, &alloc_id));
    *fem_program_p = alloc_id;
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, *fem_program_p);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_action_pmf_a_fem_pgm_id_deallocate(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_FIELD_FEM_ID_VERIFY(unit, fem_id);
    DNX_FIELD_FEM_PROGRAM_VERIFY(unit, fem_program);

    SHR_IF_ERR_EXIT(algo_field_info_sw.ipmf_a_fem_pgm_id.free_single(unit, fem_id, fem_program));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_action_find_available_fem_program(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t * available_fem_program_p,
    uint8 *sum_allocated_actions_p)
{
    int alloc_flags;
    uint8 sw_ignore_actions;
    dnx_field_group_t sw_fg_id;
    dnx_field_fem_program_t fem_program_index, fem_program_max;

    SHR_FUNC_INIT_VARS(unit);
    alloc_flags = 0;
    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_pmf_a_fem_pgm_id_allocate
                    (unit, fem_id, alloc_flags, available_fem_program_p));
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    *sum_allocated_actions_p = 0;
    for (fem_program_index = FIRST_AVAILABLE_FEM_PROGRAM; fem_program_index < fem_program_max; fem_program_index++)
    {
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, fem_id, fem_program_index, &sw_fg_id));
        if (sw_fg_id != DNX_FIELD_GROUP_INVALID)
        {
            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.
                            ignore_actions.get(unit, fem_id, fem_program_index, &sw_ignore_actions));
            
            (*sum_allocated_actions_p) |= (~(sw_ignore_actions));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_fem_copy_replace_map_indices(
    int unit,
    dnx_field_fem_condition_entry_t fem_condition_entry_source[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_condition_entry_t
    fem_condition_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_fem_map_index_t fem_map_index_to_replace,
    dnx_field_fem_map_index_t fem_map_index_new)
{
    dnx_field_fem_condition_t fem_condition_index, fem_condition_max;

    SHR_FUNC_INIT_VARS(unit);
    fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
    for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
    {
        if (fem_condition_entry_source[fem_condition_index].fem_map_index == fem_map_index_to_replace)
        {
            fem_condition_entry_destination[fem_condition_index] = fem_condition_entry_source[fem_condition_index];
            fem_condition_entry_destination[fem_condition_index].fem_map_index = fem_map_index_new;
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_fem_is_hw_action_the_same(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_action_t dnx_encoded_fem_action,
    dnx_field_fem_action_entry_t * fem_action_entry_p,
    uint8 sum_allocated_actions,
    dnx_field_fem_map_index_t * fem_map_index_p,
    unsigned int *already_in_hw_p)
{
    uint32 template_flags;
    int profile;
    uint8 first_reference;
    shr_error_e shr_error;
    dnx_field_fem_map_index_t fem_map_index_max;

    SHR_FUNC_INIT_VARS(unit);
    
    *fem_map_index_p = DNX_FIELD_FEM_MAP_INDEX_INVALID;
    
    *already_in_hw_p = 0;
    
    SHR_IF_ERR_EXIT(dnx_field_fem_num_action_bits_verify(unit, fem_id, dnx_encoded_fem_action, fem_action_entry_p));
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;

    template_flags = 0;

    shr_error = algo_field_info_sw.action_template_per_fem.allocate_single
        (unit, fem_id, template_flags, (void *) fem_action_entry_p, NULL, &profile, &first_reference);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "\r\n"
                 "Attempted to allocate template using dnx_algo_template_allocate(). fem_id %d. Reported error: %s\r\n"
                 "Allocated profile %d first_reference %d\r\n", fem_id, shrextend_errmsg_get(shr_error), profile,
                 first_reference);
    if (shr_error == _SHR_E_RESOURCE)
    {
        
        if ((sum_allocated_actions & SAL_UPTO_BIT(fem_map_index_max)) != SAL_UPTO_BIT(fem_map_index_max))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For FEM id %d: Template manager sees 'no free profile' local bitmap (0x%08lX) indicates some are not 'occupied'. Quit.\r\n",
                         fem_id, (unsigned long) sum_allocated_actions);
        }
        SHR_EXIT();
    }
    
    *fem_map_index_p = (dnx_field_fem_map_index_t) profile;
    if (first_reference != 0)
    {
        
        if ((sum_allocated_actions & SAL_BIT(*fem_map_index_p)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For FEM id %d: Template manager sees 'profile' %d as 'not occupied' but local bitmap (0x%08lX) indicates 'occupied'. Quit.\r\n",
                         fem_id, profile, (unsigned long) sum_allocated_actions);
        }
    }
    else
    {
        
        if ((sum_allocated_actions & SAL_BIT(*fem_map_index_p)) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For FEM id %d: Template manager sees 'profile' %d as 'occupied' but local bitmap (0x%08lX) indicates 'not occupied'. Quit.\r\n",
                         fem_id, profile, (unsigned long) sum_allocated_actions);
        }
        *already_in_hw_p = 1;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_field_action_update_conditions(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_condition_entry_t fem_condition_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    dnx_field_action_t fem_encoded_actions[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    uint8 sum_allocated_actions,
    dnx_field_fem_condition_entry_t
    fem_condition_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_CONDITION],
    uint8 *ignore_actions_p,
    uint8 *already_in_hw_p,
    dnx_field_action_t fem_encoded_actions_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX],
    dnx_field_fem_action_entry_t fem_action_entry_destination[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_MAP_INDEX])
{
    
    dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
    dnx_field_fem_map_index_t fem_map_index_matched;
    uint8 ignore_actions_image;
    uint8 sum_allocated_actions_destination;

    SHR_FUNC_INIT_VARS(unit);
    
    sum_allocated_actions_destination = sum_allocated_actions;
    
    SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_array_init(unit, fem_condition_entry_destination));
    
    *already_in_hw_p = 0;
    
    ignore_actions_image = DNX_FIELD_IGNORE_ALL_ACTIONS;
    fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
    for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
    {
        unsigned int already_in_hw_flag;
        if ((*ignore_actions_p & SAL_BIT(fem_map_index)) == 0)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_fem_is_hw_action_the_same(unit, fem_id, fem_encoded_actions[fem_map_index],
                                                                &(fem_action_entry[fem_map_index]),
                                                                sum_allocated_actions |
                                                                sum_allocated_actions_destination,
                                                                &fem_map_index_matched, &already_in_hw_flag));
            if (fem_map_index_matched != DNX_FIELD_FEM_MAP_INDEX_INVALID)
            {
                if (already_in_hw_flag != 0)
                {
                    
                    
                    *already_in_hw_p |= SAL_BIT(fem_map_index_matched);
                    
                    ignore_actions_image &= SAL_RBIT(fem_map_index_matched);
                    
                    SHR_IF_ERR_EXIT(dnx_field_fem_copy_replace_map_indices
                                    (unit, fem_condition_entry, fem_condition_entry_destination, fem_map_index,
                                     fem_map_index_matched));
                    
                    fem_encoded_actions_destination[fem_map_index_matched] = fem_encoded_actions[fem_map_index];
                    
                }
                else
                {
                    
                    sum_allocated_actions_destination |= SAL_BIT(fem_map_index_matched);
                    
                    ignore_actions_image &= SAL_RBIT(fem_map_index_matched);
                    
                    SHR_IF_ERR_EXIT(dnx_field_fem_copy_replace_map_indices
                                    (unit, fem_condition_entry, fem_condition_entry_destination,
                                     fem_map_index, fem_map_index_matched));
                    
                    fem_encoded_actions_destination[fem_map_index_matched] = fem_encoded_actions[fem_map_index];
                    
                    memcpy(&fem_action_entry_destination[fem_map_index_matched],
                           &fem_action_entry[fem_map_index], sizeof(fem_action_entry_destination[0]));

                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "No free 'action' space was found. sum_allocated_actions (bitmap of allocated actions) 0x%08lX Quit.\r\n",
                             (unsigned long) sum_allocated_actions);
            }
        }
    }
    
    *ignore_actions_p = ignore_actions_image;
exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
dnx_algo_field_action_fes_update_sw_state_priority(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    unsigned int fes_id,
    dnx_field_action_priority_t priority)
{
    SHR_FUNC_INIT_VARS(unit);

    
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.set(unit, context_id, fes_id, priority));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                            priority.set(unit, context_id, fes_id, priority));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                            priority.set(unit, context_id, fes_id, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" FES ID %d context ID %d priority was set to 0x%08X.\r\n.",
                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), fes_id, context_id, priority);
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_update_sw_state_fes_state(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    unsigned int fes_id,
    unsigned int fes_pgm_id,
    unsigned int action_number)
{
    SHR_FUNC_INIT_VARS(unit);

    
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.set(unit, fes_id, fes_pgm_id, fg_id));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            place_in_fg.set(unit, fes_id, fes_pgm_id, action_number));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.set(unit, fes_id, fes_pgm_id, fg_id));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            place_in_fg.set(unit, fes_id, fes_pgm_id, action_number));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.set(unit, fes_id, fes_pgm_id, fg_id));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                            place_in_fg.set(unit, fes_id, fes_pgm_id, action_number));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" FES ID %d FES program ID %d was set to:%s\r\n.",
                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), fes_id, fes_pgm_id, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "==> field group ID %d place in field group %d.%s%s\r\n.",
                 fg_id, action_number, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_update_sw_state_mask(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int fes_id,
    dnx_field_fes_mask_id_t mask_id,
    int set)
{
    uint8 is_alloc;

    SHR_FUNC_INIT_VARS(unit);

    if (set)
    {
        is_alloc = TRUE;
    }
    else
    {
        is_alloc = FALSE;
    }

    
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            mask_is_alloc.set(unit, fes_id, mask_id, is_alloc));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            mask_is_alloc.set(unit, fes_id, mask_id, is_alloc));

            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.mask_is_alloc.set(unit, fes_id, mask_id, is_alloc));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "In stage \"%s\" FES ID %d mask %d was set to \"%s\".\r\n.",
                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), fes_id, mask_id,
                 (set ? "allocated" : "free"));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_update_sw_state_single(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    uint8 is_shared,
    unsigned int place_in_fg,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_field_fes_mask_id_t fes_mask_id_delete[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_field_fes_mask_id_t dest_fes_mask_id_add[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_field_action_priority_t priority)
{
    unsigned int fes2msb_ndx;
    unsigned int mask_ndx;
    int mask_delete[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int mask_add[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_mask_id_delete, _SHR_E_PARAM, "fes_mask_id_delete");
    SHR_NULL_CHECK(dest_fes_mask_id_add, _SHR_E_PARAM, "dest_fes_mask_id_add");

    
    if (is_shared == FALSE)
    {
        
        for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
        {
            mask_delete[mask_ndx] = FALSE;
            mask_add[mask_ndx] = FALSE;
        }
        for (fes2msb_ndx = 0; fes2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes2msb_ndx++)
        {
            if (fes_mask_id_delete[fes2msb_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                mask_delete[fes_mask_id_delete[fes2msb_ndx]] = TRUE;
            }
            if (dest_fes_mask_id_add[fes2msb_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                mask_add[dest_fes_mask_id_add[fes2msb_ndx]] = TRUE;
            }
        }
        for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
        {
            if (mask_add[mask_ndx])
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_mask
                                (unit, field_stage, fes_id, mask_ndx, TRUE));
            }
            else if (mask_delete[mask_ndx])
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_mask
                                (unit, field_stage, fes_id, mask_ndx, FALSE));
            }
        }

        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_fes_state
                        (unit, field_stage, fg_id, fes_id, fes_pgm_id, place_in_fg));
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_priority
                    (unit, field_stage, context_id, fes_id, priority));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_collect_current_state_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    int post_attach,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    unsigned int *first_place_in_fg_p)
{
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes2msb_ndx;
    int invalidate_next_found;
    signed int previous_last_ordinal_place;
    dnx_field_group_t stored_fg_id;
    uint8 stored_place_in_fg;
    dnx_field_action_priority_t stored_priority;
    dnx_field_action_type_t invalidate_next_action_type;
    unsigned int nof_existing_actions_in_fg;
    uint8 occupied_spot_post_attach[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP] = { 0 };
    int available_place_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_in_p, _SHR_E_PARAM, "current_state_in_p");
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(first_place_in_fg_p, _SHR_E_PARAM, "first_place_in_fg_p");

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type(unit, field_stage, &invalidate_next_action_type));

    
    sal_memset(current_state_changing_p, 0x0, sizeof(*current_state_changing_p));
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        after_invalidate_next[fes_id_ndx] = FALSE;
    }

    
    nof_existing_actions_in_fg = 0;
    previous_last_ordinal_place = -1;
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        current_state_changing_p->fes_pgm_id[fes_id_ndx] =
            current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id;
        if (current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.get
                                    (unit, fes_id_ndx,
                                     current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id,
                                     &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.place_in_fg.get(unit, fes_id_ndx,
                                                                                             current_state_in_p->
                                                                                             context_state.
                                                                                             fes_id_info[fes_id_ndx].
                                                                                             fes_pgm_id,
                                                                                             &stored_place_in_fg));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.get(unit, context_id, fes_id_ndx, &stored_priority));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.get
                                    (unit, fes_id_ndx,
                                     current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id,
                                     &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.place_in_fg.get(unit, fes_id_ndx,
                                                                                             current_state_in_p->
                                                                                             context_state.
                                                                                             fes_id_info[fes_id_ndx].
                                                                                             fes_pgm_id,
                                                                                             &stored_place_in_fg));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.get(unit, context_id, fes_id_ndx, &stored_priority));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.get
                                    (unit, fes_id_ndx,
                                     current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id,
                                     &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.place_in_fg.get(unit, fes_id_ndx,
                                                                                            current_state_in_p->
                                                                                            context_state.
                                                                                            fes_id_info[fes_id_ndx].
                                                                                            fes_pgm_id,
                                                                                            &stored_place_in_fg));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.get(unit, context_id, fes_id_ndx, &stored_priority));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            if (stored_fg_id == fg_id)
            {
                if (previous_last_ordinal_place < (signed int) stored_place_in_fg)
                {
                    previous_last_ordinal_place = stored_place_in_fg;
                }
                if (stored_place_in_fg >= DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG)
                {
                    if ((stored_place_in_fg - DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG) >=
                        (sizeof(occupied_spot_post_attach) / sizeof(occupied_spot_post_attach[0])))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "stored_place_in_fg (%d) above maximum (%d). \r\n",
                                     stored_place_in_fg,
                                     (int) (DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG +
                                            (sizeof(occupied_spot_post_attach) /
                                             sizeof(occupied_spot_post_attach[0]))));
                    }
                    occupied_spot_post_attach[stored_place_in_fg -
                                              DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG] = 1;
                }
                nof_existing_actions_in_fg++;
            }
            
            current_state_changing_p->is_shared[fes_id_ndx] =
                current_state_in_p->context_state.fes_id_info[fes_id_ndx].is_shared;
            sal_memcpy(current_state_changing_p->fes_quartet_info[fes_id_ndx].fes_action_type,
                       current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_action_type,
                       sizeof(current_state_changing_p->fes_quartet_info[0].fes_action_type));
            sal_memcpy(current_state_changing_p->fes_quartet_info[fes_id_ndx].fes_mask_id,
                       current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_mask_id,
                       sizeof(current_state_changing_p->fes_quartet_info[0].fes_mask_id));
            current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg = stored_place_in_fg;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg = stored_place_in_fg;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].fg_id = stored_fg_id;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].priority = stored_priority;
            current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_alloc = DNX_ALGO_FIELD_ACTION_MOVEMENT;

            
            invalidate_next_found = FALSE;
            for (fes2msb_ndx = 0;
                 fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes; fes2msb_ndx++)
            {
                if (current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_action_type[fes2msb_ndx] ==
                    invalidate_next_action_type)
                {
                    invalidate_next_found = TRUE;
                    break;
                }
            }
            if (invalidate_next_found && (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(stored_priority) == FALSE))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "In context ID %d, field group %d, action number %d in FES ID %d "
                             "FES program Id %d was found an invalidate next with priority that "
                             "does not mandate position 0x%x.\r\n",
                             context_id, stored_fg_id, stored_place_in_fg, fes_id_ndx,
                             current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id, stored_priority);
            }
            
            if (invalidate_next_found
                && (((fes_id_ndx + 1) % (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array)) != 0))
            {
                
                if (fes_id_ndx ==
                    (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Last FES ID %d is not the end of an array.\r\n", fes_id_ndx);
                }
                after_invalidate_next[fes_id_ndx + 1] = TRUE;
            }
        }
    }
    
    if ((post_attach == FALSE) &&
        (nof_existing_actions_in_fg > 0) && (nof_existing_actions_in_fg - 1 != previous_last_ordinal_place))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of existing actions in field group %d context %d is %d, does not match "
                     "maximum ordinal place %d.\r\n",
                     fg_id, context_id, nof_existing_actions_in_fg, previous_last_ordinal_place);
    }

    if (post_attach)
    {
        for (available_place_index = 0;
             available_place_index < (sizeof(occupied_spot_post_attach) / sizeof(occupied_spot_post_attach[0]));
             available_place_index++)
        {
            if (occupied_spot_post_attach[available_place_index] == 0)
            {
                (*first_place_in_fg_p) = available_place_index + DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG;
                break;
            }

        }
        if (available_place_index >= (sizeof(occupied_spot_post_attach) / sizeof(occupied_spot_post_attach[0])))
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "No available place to allocate an EFES post attach. "
                         "fg_id %d context_id %d.\r\n", fg_id, context_id);
        }
    }
    else
    {
        (*first_place_in_fg_p) = nof_existing_actions_in_fg;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_collect_new_actions_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    unsigned int first_place_in_fg,
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next_new[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t fes_quartet[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP])
{
    unsigned int fes_quartet_ndx;
    int invalidate_next_found;
    unsigned int fes2msb_ndx;
    dnx_field_fes_id_t fes_id_ndx;
    dnx_field_fes_id_t fes_id;
    dnx_field_action_type_t invalidate_next_action_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(fes_quartet, _SHR_E_PARAM, "fes_quartet");

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type(unit, field_stage, &invalidate_next_action_type));

    
    sal_memset(fes_quartet, 0x0, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP * sizeof(fes_quartet[0]));
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        after_invalidate_next_new[fes_id_ndx] = FALSE;
    }

    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        
        if (fes_quartet_ndx >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of FES quartets to allocate %d larger than number of action per "
                         "field group %d.\r\n",
                         fes_fg_in_p->nof_fes_quartets, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
        }
        
        sal_memcpy(fes_quartet[fes_quartet_ndx].fes_action_type,
                   fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_action_type, sizeof(fes_quartet[0].fes_action_type));
        sal_memcpy(fes_quartet[fes_quartet_ndx].fes_mask_id,
                   fes_fg_in_p->fes_quartet[fes_quartet_ndx].fes_mask_id, sizeof(fes_quartet[0].fes_mask_id));
        fes_quartet[fes_quartet_ndx].fg_id = fg_id;
        fes_quartet[fes_quartet_ndx].priority = fes_fg_in_p->fes_quartet[fes_quartet_ndx].priority;
        fes_quartet[fes_quartet_ndx].place_in_alloc = fes_quartet_ndx;
        fes_quartet[fes_quartet_ndx].place_in_fg = fes_quartet_ndx + first_place_in_fg;
        
        if ((first_place_in_fg < DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG) &&
            (fes_quartet[fes_quartet_ndx].place_in_fg >= DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "first_place_in_fg is %d, but allocating after %d with place_in_fg %d. action %d.\r\n",
                         first_place_in_fg, DNX_ALGO_FIELD_ACTION_POST_ATTACH_FIRST_PLACE_IN_FG,
                         fes_quartet[fes_quartet_ndx].place_in_fg, fes_quartet_ndx);
        }

        
        invalidate_next_found = FALSE;
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (fes_quartet[fes_quartet_ndx].fes_action_type[fes2msb_ndx] == invalidate_next_action_type)
            {
                invalidate_next_found = TRUE;
                break;
            }
        }
        if (invalidate_next_found
            && (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(fes_quartet[fes_quartet_ndx].priority) == FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Cannot allocate a FES quartet with invalidate next and priority that doesn't mandate a "
                         "FES position. Attempted to do so in context ID %d, field group %d, action number %d, "
                         "priority 0x%x.\r\n",
                         context_id, fg_id, fes_quartet[fes_quartet_ndx].place_in_fg,
                         fes_quartet[fes_quartet_ndx].priority);
        }
        
        if (invalidate_next_found
            && (DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(fes_quartet[fes_quartet_ndx].priority) !=
                dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array - 1))
        {
            DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id, unit, field_stage,
                                                          fes_quartet[fes_quartet_ndx].priority);
            
            if (fes_id == (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Last FES ID %d is not the end of an array.\r\n", fes_id);
            }
            after_invalidate_next[fes_id + 1] = TRUE;
            after_invalidate_next_new[fes_id + 1] = TRUE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_priority_compare(
    int unit,
    dnx_field_action_priority_t first_priority,
    dnx_field_group_t first_fg_id,
    unsigned int first_place_in_fg,
    dnx_field_action_priority_t second_priority,
    dnx_field_group_t second_fg_id,
    unsigned int second_place_in_fg,
    dnx_algo_field_action_fes_alloc_priority_compare_result_e * compare_result_p)
{
    uint32 first_fes_array;
    uint32 second_fes_array;
    uint32 first_fes_position;
    uint32 second_fes_position;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(compare_result_p, _SHR_E_PARAM, "compare_result_p");

    if (first_priority == DNX_FIELD_ACTION_PRIORITY_INVALID || second_priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot compare invalid priorities.\r\n");
    }
    if (first_priority == BCM_FIELD_ACTION_INVALIDATE || second_priority == BCM_FIELD_ACTION_INVALIDATE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot compare invalidate priorities.\r\n");
    }
    if (first_priority == BCM_FIELD_ACTION_DONT_CARE || second_priority == BCM_FIELD_ACTION_DONT_CARE)
    {
        (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE;
    }
    else if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(first_priority)
             || DNX_FIELD_ACTION_PRIORITY_IS_POSITION(second_priority))
    {
        (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE;
    }
    else
    {
        first_fes_array = DNX_FIELD_ACTION_PRIORITY_ARRAY(first_priority);
        second_fes_array = DNX_FIELD_ACTION_PRIORITY_ARRAY(second_priority);
        first_fes_position = DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(first_priority);
        second_fes_position = DNX_FIELD_ACTION_PRIORITY_PRIORITY_POSITION(second_priority);

        if (first_fes_array > second_fes_array)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_fes_array > first_fes_array)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else if (first_fes_position > second_fes_position)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_fes_position > first_fes_position)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else if (first_fg_id > second_fg_id)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_fg_id > first_fg_id)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else if (first_place_in_fg > second_place_in_fg)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS;
        }
        else if (second_place_in_fg > first_place_in_fg)
        {
            (*compare_result_p) = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to perform tie breaking between priorities 0x%x and 0x%x.\r\n",
                         first_priority, second_priority);
        }
    }

    
    if ((*compare_result_p) == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\"don't care\" priority compares, unexpected.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Given two priorites of actions that have contention with one another, check if the two priorities are allowed 
 *  to have contention.
 *  Contention is allowed if there is a way to tell which action has a higher priority.
 * \param [in] unit -
 *  Device ID.
 * \param [in] field_stage -
 *  The PMF block we allocate for. Used for error logging.
 * \param [in] context_id -
 *  The context ID. Used for error logging.
 * \param [in] first_priority -
 *  The priority of the first action in contention.
 * \param [in] first_fg_id -
 *  The field group ID of the first action. Used for error logging.
 * \param [in] first_place_in_fg -
 *  The place in the field group of the first action. Used for error logging.
 * \param [in] second_priority -
 *  The priority of the second action in contention.
 * \param [in] second_fg_id -
 *  The field group ID of the second action. Used for error logging.
 * \param [in] second_place_in_fg -
 *  The place in the field group of the second action. Used for error logging.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None.
 * \see
 *   * dnx_algo_field_action_fes_allocate_contention_find()
 */
static shr_error_e
dnx_algo_field_action_fes_allocate_contention_priorities_allowed(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_action_priority_t first_priority,
    dnx_field_group_t first_fg_id,
    unsigned int first_place_in_fg,
    dnx_field_action_priority_t second_priority,
    dnx_field_group_t second_fg_id,
    unsigned int second_place_in_fg)
{
    int first_is_dont_care;
    int second_is_dont_care;
    int first_is_mandatory_position;
    int second_is_mandatory_position;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If any priority is invalid, send an error.
     */
    if (first_priority == DNX_FIELD_ACTION_PRIORITY_INVALID || second_priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid priority encountered.\r\n");
    }

    first_is_dont_care = (first_priority == BCM_FIELD_ACTION_DONT_CARE);
    second_is_dont_care = (second_priority == BCM_FIELD_ACTION_DONT_CARE);
    first_is_mandatory_position = DNX_FIELD_ACTION_PRIORITY_IS_POSITION(first_priority);
    second_is_mandatory_position = DNX_FIELD_ACTION_PRIORITY_IS_POSITION(second_priority);

    /*
     * If only one action is "don't care" we cannot compare them.
     */
    if ((first_is_dont_care && second_is_dont_care == FALSE) || (first_is_dont_care == FALSE && second_is_dont_care))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                     "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                     "Cannot compare \"don't care\" priority with a non \"don't care\" priority.\r\n",
                     first_place_in_fg, first_fg_id, first_priority, second_place_in_fg, second_fg_id, second_priority,
                     context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
    }
    /*
     * If only one action mandates position we cannot compare them.
     */
    if ((first_is_mandatory_position && second_is_mandatory_position == FALSE) ||
        (first_is_mandatory_position == FALSE && second_is_mandatory_position))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                     "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                     "Cannot compare mandatory position priority with a non mandatory position priority.\r\n",
                     first_place_in_fg, first_fg_id, first_priority, second_place_in_fg, second_fg_id, second_priority,
                     context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
    }
    /*
     * If both priorities are indenticle, they cannot be compared.
     */
    if (first_priority == second_priority)
    {
        /*
         * If both proirities are "don't care", provide a special error log.
         */
        if (first_is_dont_care && second_is_dont_care)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                         "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                         "Both actions use \"don't care\" priorities.\r\n",
                         first_place_in_fg, first_fg_id, first_priority,
                         second_place_in_fg, second_fg_id, second_priority,
                         context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unresolved collision between non void action %d of fg_id %d (priority 0x%x) "
                     "and non void action %d of fg_id %d (priority 0x%x) on context %d, stage %s. "
                     "The priorities are identical.\r\n",
                     first_place_in_fg, first_fg_id, first_priority, second_place_in_fg, second_fg_id, second_priority,
                     context_id, DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_contention_check(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_action_type_t fes_action_type_1,
    dnx_field_action_type_t fes_action_type_2,
    uint8 *contention_exists)
{
    dnx_field_action_type_t invalidate_next_action_type;
    dnx_field_action_type_t invalid_action_type;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(contention_exists, _SHR_E_PARAM, "contention_exists");

    SHR_IF_ERR_EXIT(dnx_field_map_get_invalidate_next_action_type(unit, field_stage, &invalidate_next_action_type));
    SHR_IF_ERR_EXIT(dnx_field_map_get_invalid_action_type(unit, field_stage, &invalid_action_type));

    (*contention_exists) = FALSE;
    
    if ((fes_action_type_1 == invalid_action_type) ||
        (fes_action_type_2 == invalid_action_type) ||
        (fes_action_type_1 == invalidate_next_action_type) || (fes_action_type_2 == invalidate_next_action_type))
    {
        (*contention_exists) = FALSE;
    }
    else if (fes_action_type_1 == fes_action_type_2)
    {
        (*contention_exists) = TRUE;;
    }
    else
    {
        (*contention_exists) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_contention_find(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    uint8 contention[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT])
{
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes2msb_ndx_1;
    unsigned int fes2msb_ndx_2;
    dnx_field_fes_id_t min_fes_id_intern;
    dnx_field_fes_id_t max_fes_id_intern;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(contention, _SHR_E_PARAM, "contention");

    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        contention[fes_id_ndx] = FALSE;
    }

    if (min_fes_id < 0)
    {
        min_fes_id_intern = 0;
    }
    else
    {
        min_fes_id_intern = min_fes_id;
    }
    if (max_fes_id >=
        (signed int) (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context))
    {
        max_fes_id_intern = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1;
    }
    else
    {
        max_fes_id_intern = max_fes_id;
    }

    if (max_fes_id >= 0)
    {
        for (fes_id_ndx = min_fes_id_intern; (signed int) fes_id_ndx <= max_fes_id_intern; fes_id_ndx++)
        {
            
            if (fes_id_ndx == fes_id_currently_placed)
            {
                continue;
            }
            
            if (current_state_changing_p->fes_pgm_id[fes_id_ndx] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                for (fes2msb_ndx_1 = 0;
                     (fes2msb_ndx_1 < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes)
                     && (contention[fes_id_ndx] == FALSE); fes2msb_ndx_1++)
                {
                    for (fes2msb_ndx_2 = 0;
                         (fes2msb_ndx_2 < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes)
                         && (contention[fes_id_ndx] == FALSE); fes2msb_ndx_2++)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_check
                                        (unit, field_stage,
                                         fes_quartet_p->fes_action_type[fes2msb_ndx_1],
                                         current_state_changing_p->
                                         fes_quartet_info[fes_id_ndx].fes_action_type[fes2msb_ndx_2],
                                         &(contention[fes_id_ndx])));
                    }
                }
                if (contention[fes_id_ndx])
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_priorities_allowed
                                    (unit, field_stage, context_id,
                                     fes_quartet_p->priority,
                                     fes_quartet_p->fg_id,
                                     fes_quartet_p->place_in_fg,
                                     current_state_changing_p->fes_quartet_info[fes_id_ndx].priority,
                                     current_state_changing_p->fes_quartet_info[fes_id_ndx].fg_id,
                                     current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_shareble_fes_pgm(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    unsigned int place_in_fg,
    int allow_fes_pgm_sharing,
    uint8 *is_shareable_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p)
{
    unsigned int fes_pgm_ndx;
    dnx_field_group_t stored_fg_id;
    uint8 stored_place_in_fg;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(is_shareable_p, _SHR_E_PARAM, "is_shareable_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");

    if (allow_fes_pgm_sharing)
    {
        
        (*is_shareable_p) = FALSE;
        (*fes_pgm_id_p) = (dnx_field_fes_pgm_id_t) (-1);
        for (fes_pgm_ndx = 0; fes_pgm_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs;
             fes_pgm_ndx++)
        {
            
            if (fes_pgm_ndx == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                continue;
            }
            
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.place_in_fg.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_place_in_fg));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                    place_in_fg.get(unit, fes_id, fes_pgm_ndx, &stored_place_in_fg));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_fg_id));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.place_in_fg.get
                                    (unit, fes_id, fes_pgm_ndx, &stored_place_in_fg));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            
            if (fg_id == stored_fg_id && place_in_fg == stored_place_in_fg)
            {
                
                if (current_state_changing_p->fes_pgm_id[fes_id] == fes_pgm_ndx)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached a result where FES quartet may share itself.\r\n");
                }
                
                (*is_shareable_p) = TRUE;
                (*fes_pgm_id_p) = fes_pgm_ndx;
                break;
            }
        }
    }
    else
    {
        (*is_shareable_p) = FALSE;
        (*fes_pgm_id_p) = (dnx_field_fes_pgm_id_t) (-1);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_masks(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_force_evacuation,
    uint8 *masks_available_p,
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES])
{
    unsigned int fes_mask_ndx;
    unsigned int fes_inst_ndx;
    uint8 mask_allocated;
    int found;
    uint8 masks_used_vacated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    dnx_field_fes_mask_id_t mask_requested_allocated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int run_check;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(masks_available_p, _SHR_E_PARAM, "masks_available_p");
    SHR_NULL_CHECK(fes_mask_id, _SHR_E_PARAM, "fes_mask_id");

    
    for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; fes_mask_ndx++)
    {
        masks_used_vacated[fes_mask_ndx] = FALSE;
        mask_requested_allocated[fes_mask_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }

    run_check = TRUE;

    
    if ((current_state_changing_p->fes_pgm_id[fes_id] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM) &&
        (current_state_changing_p->is_shared[fes_id] == FALSE) && allow_force_evacuation)
    {
        for (fes_mask_ndx = 0; fes_mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
             fes_mask_ndx++)
        {
            if (fes_mask_ndx != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                for (fes_inst_ndx = 0;
                     fes_inst_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
                     fes_inst_ndx++)
                {
                    if (current_state_changing_p->fes_quartet_info[fes_id].fes_mask_id[fes_inst_ndx] == fes_mask_ndx)
                    {
                        masks_used_vacated[fes_mask_ndx] = TRUE;
                    }
                }
            }
        }
    }
    
    if ((current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM) && allow_force_evacuation)
    {
        run_check = FALSE;
    }

    if (run_check)
    {
        
        fes_mask_ndx = 0;
        found = TRUE;
        for (fes_inst_ndx = 0; fes_inst_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes_inst_ndx++)
        {
            
            if (fes_quartet_p->fes_mask_id[fes_inst_ndx] == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                fes_mask_id[fes_inst_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
                continue;
            }
            else if (mask_requested_allocated[fes_quartet_p->fes_mask_id[fes_inst_ndx]] !=
                     DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                fes_mask_id[fes_inst_ndx] = mask_requested_allocated[fes_quartet_p->fes_mask_id[fes_inst_ndx]];
                continue;
            }

            
            found = FALSE;
            for (; fes_mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                 fes_mask_ndx++)
            {
                
                if (fes_mask_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
                {
                    continue;
                }
                
                switch (field_stage)
                {
                    case DNX_FIELD_STAGE_IPMF1:
                    case DNX_FIELD_STAGE_IPMF2:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                        mask_is_alloc.get(unit, fes_id, fes_mask_ndx, &mask_allocated));
                        break;
                    case DNX_FIELD_STAGE_IPMF3:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                        mask_is_alloc.get(unit, fes_id, fes_mask_ndx, &mask_allocated));
                        break;
                    case DNX_FIELD_STAGE_EPMF:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                        mask_is_alloc.get(unit, fes_id, fes_mask_ndx, &mask_allocated));
                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                }
                
                if (mask_allocated && masks_used_vacated[fes_mask_ndx] == FALSE)
                {
                    continue;
                }
                
                fes_mask_id[fes_inst_ndx] = fes_mask_ndx;
                mask_requested_allocated[fes_quartet_p->fes_mask_id[fes_inst_ndx]] = fes_mask_ndx;
                found = TRUE;
                fes_mask_ndx++;
                break;
            }
            if (found == FALSE)
            {
                break;
            }
        }
        if (found == FALSE)
        {
            (*masks_available_p) = FALSE;
        }
        else
        {
            (*masks_available_p) = TRUE;
        }
    }
    else
    {
        
        (*masks_available_p) = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_space_available(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_evacuation,
    uint8 *space_available_p,
    uint8 *space_available_if_evacuated_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p,
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    dnx_field_group_t field_group_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(space_available_p, _SHR_E_PARAM, "space_available_p");
    SHR_NULL_CHECK(space_available_if_evacuated_p, _SHR_E_PARAM, "space_available_if_evacuated_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");
    SHR_NULL_CHECK(fes_mask_id, _SHR_E_PARAM, "fes_mask_id");

    (*space_available_p) = FALSE;
    (*space_available_if_evacuated_p) = FALSE;
    
    if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM || allow_evacuation)
    {

        for ((*fes_pgm_id_p) = 0;
             (*fes_pgm_id_p) < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs;
             (*fes_pgm_id_p)++)
        {
            
            if ((*fes_pgm_id_p) == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                continue;
            }
            
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.field_group.get
                                    (unit, fes_id, (*fes_pgm_id_p), &field_group_id));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.field_group.get
                                    (unit, fes_id, (*fes_pgm_id_p), &field_group_id));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.field_group.get
                                    (unit, fes_id, (*fes_pgm_id_p), &field_group_id));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            
            if (field_group_id == DNX_FIELD_GROUP_INVALID)
            {
                if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
                {
                    (*space_available_p) = TRUE;
                }
                (*space_available_if_evacuated_p) = TRUE;
                break;
            }
        }
        
        if ((*space_available_p) == FALSE
            && current_state_changing_p->fes_pgm_id[fes_id] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM
            && DNX_FIELD_ACTION_PRIORITY_IS_POSITION(current_state_changing_p->fes_quartet_info[fes_id].priority) ==
            FALSE && current_state_changing_p->is_shared[fes_id] == FALSE)
        {
            (*space_available_if_evacuated_p) = TRUE;
            if (current_state_changing_p->fes_pgm_id[fes_id] < (*fes_pgm_id_p))
            {
                (*fes_pgm_id_p) = current_state_changing_p->fes_pgm_id[fes_id];
            }
        }

        
        if ((*space_available_if_evacuated_p) == TRUE)
        {
            
            if (*space_available_p)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_masks
                                (unit, field_stage, current_state_changing_p, fes_id, fes_quartet_p,
                                 FALSE, space_available_p, fes_mask_id));
            }
            
            if ((*space_available_p) == FALSE)
            {
                
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_masks
                                (unit, field_stage, current_state_changing_p, fes_id, fes_quartet_p,
                                 TRUE, space_available_if_evacuated_p, fes_mask_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_is_allocable(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_fes_pgm_sharing,
    int allow_evacuation,
    uint8 *is_shareable_p,
    uint8 *is_allocable_p,
    uint8 *is_allocable_if_evacuated_p,
    dnx_field_fes_pgm_id_t * fes_pgm_id_p,
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(is_allocable_p, _SHR_E_PARAM, "is_allocable_p");
    SHR_NULL_CHECK(fes_pgm_id_p, _SHR_E_PARAM, "fes_pgm_id_p");
    SHR_NULL_CHECK(fes_mask_id, _SHR_E_PARAM, "fes_mask_id");

    (*is_allocable_p) = FALSE;
    (*is_allocable_if_evacuated_p) = FALSE;

    
    if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM ||
        DNX_FIELD_ACTION_PRIORITY_IS_POSITION(current_state_changing_p->fes_quartet_info[fes_id].priority) == FALSE)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_shareble_fes_pgm
                        (unit, field_stage, fes_quartet_p->fg_id, current_state_changing_p, fes_id,
                         fes_quartet_p->place_in_fg, allow_fes_pgm_sharing, is_shareable_p, fes_pgm_id_p));
        
        if (*is_shareable_p)
        {
            if (current_state_changing_p->fes_pgm_id[fes_id] == DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                (*is_allocable_p) = TRUE;
                (*is_allocable_if_evacuated_p) = TRUE;
            }
            else if (allow_evacuation)
            {
                (*is_allocable_if_evacuated_p) = TRUE;
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_space_available
                            (unit, field_stage, current_state_changing_p, fes_id,
                             fes_quartet_p, allow_evacuation, is_allocable_p, is_allocable_if_evacuated_p,
                             fes_pgm_id_p, fes_mask_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_order(
    int unit,
    dnx_field_stage_e field_stage,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_field_fes_id_t fes_id_order[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    unsigned int *nof_fes_id_order_p,
    uint8 shareable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 allocable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 allocable_if_evacuated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_field_fes_mask_id_t
    fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
    [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES])
{
    unsigned int fes_id_ndx;
    unsigned int fes_id_order_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(fes_id_order, _SHR_E_PARAM, "fes_id_order");
    SHR_NULL_CHECK(nof_fes_id_order_p, _SHR_E_PARAM, "nof_fes_id_order_p");
    SHR_NULL_CHECK(shareable, _SHR_E_PARAM, "shareable");
    SHR_NULL_CHECK(allocable, _SHR_E_PARAM, "allocable");
    SHR_NULL_CHECK(allocable_if_evacuated, _SHR_E_PARAM, "allocable_if_evacuated");

    
    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(fes_quartet_p->priority))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Function for allocation order called for mandatory placement priority 0x%x field group %d "
                     "action number %d.\r\n",
                     fes_quartet_p->priority, fes_quartet_p->fg_id, fes_quartet_p->place_in_fg);
    }

    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((min_fes_id > (signed int) fes_id_ndx) || (max_fes_id < (signed int) fes_id_ndx) ||
            (banned_fes_id[fes_id_ndx] == TRUE) || (after_invalidate_next[fes_id_ndx] == TRUE) ||
            (fes_id_ndx == fes_id_currently_placed))
        {
            shareable[fes_id_ndx] = FALSE;
            allocable[fes_id_ndx] = FALSE;
            allocable_if_evacuated[fes_id_ndx] = FALSE;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_is_allocable
                            (unit, field_stage, current_state_changing_p, fes_id_ndx, fes_quartet_p,
                             allow_fes_pgm_sharing, TRUE,
                             &shareable[fes_id_ndx], &allocable[fes_id_ndx], &allocable_if_evacuated[fes_id_ndx],
                             &fes_pgm_id[fes_id_ndx], fes_mask_id[fes_id_ndx]));
        }
    }
    for (; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        shareable[fes_id_ndx] = FALSE;
        allocable[fes_id_ndx] = FALSE;
        allocable_if_evacuated[fes_id_ndx] = FALSE;
    }

    
    
    fes_id_order_ndx = 0;
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if (allocable[fes_id_ndx] && shareable[fes_id_ndx])
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if (allocable[fes_id_ndx] && (shareable[fes_id_ndx] == FALSE))
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((allocable[fes_id_ndx] == FALSE) && allocable_if_evacuated[fes_id_ndx] && shareable[fes_id_ndx])
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((allocable[fes_id_ndx] == FALSE) && allocable_if_evacuated[fes_id_ndx] && (shareable[fes_id_ndx] == FALSE))
        {
            fes_id_order[fes_id_order_ndx] = fes_id_ndx;
            fes_id_order_ndx++;
        }
    }

    
    (*nof_fes_id_order_p) = fes_id_order_ndx;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_add(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int is_new,
    dnx_field_fes_id_t source_fes_id,
    dnx_field_fes_id_t dest_fes_id,
    dnx_field_fes_pgm_id_t dest_fes_pgm_id,
    int dest_is_shared,
    dnx_field_fes_mask_id_t dest_fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES],
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    unsigned int nof_elements_out_array;
    unsigned int fes_quartet_ndx;
    unsigned int fes_2msb_ndx;
    dnx_field_fes_mask_id_t masks_unchanged[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(dest_fes_mask_id, _SHR_E_PARAM, "dest_fes_mask_id");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    
    if (is_new && fes_quartet_p->place_in_alloc == DNX_ALGO_FIELD_ACTION_MOVEMENT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "FES quartet movement was called from a place that allocates a new FES quartet.\r\n");
    }
    if (fes_quartet_p->place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT && fes_quartet_p->place_in_alloc < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal 'place_in_alloc' value %d.\r\n", fes_quartet_p->place_in_alloc);
    }
    if (is_new == FALSE && source_fes_id == dest_fes_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "FES movement moved from a FES ID to itself %d.\r\n", source_fes_id);
    }
    

    
    nof_elements_out_array =
        (sizeof(alloc_result_p->fes_quartet_change) / sizeof(alloc_result_p->fes_quartet_change[0]));
    if (alloc_result_p->nof_fes_quartet_changes >= nof_elements_out_array)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of FES changes exceeds array maximum size %d.\r\n", nof_elements_out_array);
    }

    fes_quartet_ndx = alloc_result_p->nof_fes_quartet_changes;
    
    sal_memset(&(alloc_result_p->fes_quartet_change[fes_quartet_ndx]), 0x0,
               sizeof(alloc_result_p->fes_quartet_change[0]));
    for (fes_2msb_ndx = 0; fes_2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_ndx++)
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id[fes_2msb_ndx] =
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id[fes_2msb_ndx] =
            DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }

    
    if (is_new)
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc = fes_quartet_p->place_in_alloc;
    }
    else
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc = DNX_ALGO_FIELD_ACTION_MOVEMENT;
    }
    
    alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_id = dest_fes_id;
    alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id = dest_fes_pgm_id;
    alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_is_shared = dest_is_shared;
    if (dest_is_shared == FALSE)
    {
        sal_memcpy(alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id, dest_fes_mask_id,
                   sizeof(alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id));
    }
    
    if (is_new == FALSE)
    {
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_id = source_fes_id;
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_pgm_id =
            current_state_changing_p->fes_pgm_id[source_fes_id];
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared =
            current_state_changing_p->is_shared[source_fes_id];
        if (alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared == FALSE)
        {
            sal_memcpy(alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id,
                       current_state_changing_p->fes_quartet_info[source_fes_id].fes_mask_id,
                       sizeof(alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id));
        }
    }
    
    (alloc_result_p->nof_fes_quartet_changes)++;

    
    sal_memcpy(&(current_state_changing_p->fes_quartet_info[dest_fes_id]), fes_quartet_p,
               sizeof(current_state_changing_p->fes_quartet_info[0]));
    current_state_changing_p->fes_quartet_info[dest_fes_id].place_in_alloc =
        alloc_result_p->fes_quartet_change[fes_quartet_ndx].place_in_alloc;
    sal_memcpy(current_state_changing_p->fes_quartet_info[dest_fes_id].fes_mask_id,
               alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id,
               sizeof(current_state_changing_p->fes_quartet_info[dest_fes_id].fes_mask_id));
    current_state_changing_p->fes_pgm_id[dest_fes_id] = dest_fes_pgm_id;
    current_state_changing_p->is_shared[dest_fes_id] = dest_is_shared;
    if (is_new == FALSE)
    {
        current_state_changing_p->fes_pgm_id[source_fes_id] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
    }

    
    for (fes_2msb_ndx = 0; fes_2msb_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_ndx++)
    {
        masks_unchanged[fes_2msb_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID;
    }
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_single
                    (unit, field_stage, fes_quartet_p->fg_id, context_id,
                     dest_is_shared,
                     fes_quartet_p->place_in_fg,
                     alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_id,
                     alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_pgm_id,
                     masks_unchanged, alloc_result_p->fes_quartet_change[fes_quartet_ndx].dest_fes_mask_id,
                     fes_quartet_p->priority));
    if (is_new == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_update_sw_state_single
                        (unit, field_stage, DNX_FIELD_GROUP_INVALID, context_id,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_is_shared,
                         0,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_id,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_pgm_id,
                         alloc_result_p->fes_quartet_change[fes_quartet_ndx].source_fes_mask_id, masks_unchanged,
                         DNX_FIELD_ACTION_PRIORITY_INVALID));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_non_mandatory_position_push(
    int unit,
    dnx_field_stage_e field_stage,
    int is_direction_up,
    int min_fes_id_intern,
    int max_fes_id_intern,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int is_new,
    int min_fes_id,
    int max_fes_id,
    int min_in_array,
    int max_in_array,
    int allow_contention_moving,
    int allow_non_contention_moving,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p)
{
    dnx_field_fes_id_t fes_id_ndx;
    uint8 shareable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable_if_evacuated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    int from_here_cannot_move;
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_mask_id_t
        fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int push_succeeded;
    int found_evacuated_only_place;
    int move_succeeded;
    int min_fes_id_hard;
    int max_fes_id_hard;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(banned_fes_id, _SHR_E_PARAM, "banned_fes_id");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(found_allocation_p, _SHR_E_PARAM, "found_allocation_p");

    found_evacuated_only_place = FALSE;

    min_fes_id_hard = MAX(min_fes_id, min_in_array);
    max_fes_id_hard = MIN(max_fes_id, max_in_array);

    
    
    if (is_direction_up)
    {
        fes_id_ndx = MAX(max_fes_id_intern + 1, min_fes_id_intern);
    }
    else
    {
        fes_id_ndx = MIN(min_fes_id_intern - 1, max_fes_id_intern);
    }
    while (TRUE)
    {
        
        if (is_direction_up)
        {
            if (!(((signed int) fes_id_ndx) <= max_fes_id_hard && (*found_allocation_p) == FALSE))
            {
                break;
            }
        }
        else
        {
            
            if (!(((signed int) fes_id_ndx) >= min_fes_id_hard && (*found_allocation_p) == FALSE &&
                  fes_id_ndx <= dnx_data_field.stage.stage_info_get(unit,
                                                                    field_stage)->nof_fes_instruction_per_context))
            {
                break;
            }
        }
        if (banned_fes_id[fes_id_ndx] == FALSE && after_invalidate_next[fes_id_ndx] == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_is_allocable
                            (unit, field_stage, current_state_changing_p, fes_id_ndx, fes_quartet_p,
                             allow_fes_pgm_sharing, TRUE, &shareable[fes_id_ndx], &allocable[fes_id_ndx],
                             &allocable_if_evacuated[fes_id_ndx], &fes_pgm_id[fes_id_ndx], fes_mask_id[fes_id_ndx]));
            if (allocable[fes_id_ndx] ||
                (allocable_if_evacuated[fes_id_ndx] &&
                 fes_id_ndx == (is_direction_up ? (max_fes_id_intern + 1) : (min_fes_id_intern - 1))))
            {
                
                banned_fes_id[fes_id_ndx] = TRUE;
                if (is_direction_up)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[max_fes_id_intern + 1]),
                                     max_fes_id_intern + 1,
                                     fes_id_ndx + 1,
                                     dnx_data_field.stage.stage_info_get(unit,
                                                                         field_stage)->nof_fes_instruction_per_context,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[min_fes_id_intern - 1]),
                                     min_fes_id_intern - 1,
                                     0, fes_id_ndx - 1,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                banned_fes_id[fes_id_ndx] = FALSE;
                if (push_succeeded)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                    (unit, field_stage, context_id,
                                     fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                                     fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                                     current_state_changing_p, alloc_result_p));
                    (*found_allocation_p) = TRUE;
                }
                else
                {
                    
                    break;
                }
            }
            else if (allocable_if_evacuated[fes_id_ndx])
            {
                found_evacuated_only_place = TRUE;
            }
        }
        
        if (is_direction_up)
        {
            fes_id_ndx++;
        }
        else
        {
            if (fes_id_ndx == 0)
            {
                break;
            }
            fes_id_ndx--;
        }
    }
    if (found_evacuated_only_place && allow_contention_moving)
    {
        
        from_here_cannot_move = fes_id_ndx;
        push_succeeded = FALSE;
        
        if (is_direction_up)
        {
            fes_id_ndx = MIN(from_here_cannot_move, max_fes_id_hard);
        }
        else
        {
            fes_id_ndx = MAX(from_here_cannot_move, min_fes_id_hard);
        }
        while (TRUE)
        {
            
            if (is_direction_up)
            {
                if (!(push_succeeded == FALSE && (signed int) fes_id_ndx >= max_fes_id_intern))
                {
                    break;
                }
            }
            else
            {
                if (!(push_succeeded == FALSE && (signed int) fes_id_ndx <= min_fes_id_intern))
                {
                    break;
                }
            }
            if (allocable_if_evacuated[fes_id_ndx])
            {
                
                banned_fes_id[fes_id_ndx] = TRUE;
                if (is_direction_up)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[max_fes_id_intern + 1]),
                                     max_fes_id_intern + 1,
                                     fes_id_ndx + 1,
                                     dnx_data_field.stage.stage_info_get(unit,
                                                                         field_stage)->nof_fes_instruction_per_context,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[min_fes_id_intern - 1]),
                                     min_fes_id_intern - 1,
                                     0, fes_id_ndx - 1,
                                     TRUE, allow_non_contention_moving, allow_fes_pgm_sharing,
                                     current_state_changing_p, alloc_result_p, &push_succeeded));
                }
                banned_fes_id[fes_id_ndx] = FALSE;
            }
            
            if (is_direction_up)
            {
                if (fes_id_ndx == 0)
                {
                    break;
                }
                fes_id_ndx--;
            }
            else
            {
                fes_id_ndx++;
            }
        }
        
        
        if (push_succeeded)
        {
            
            if (is_direction_up)
            {
                fes_id_ndx = MAX(max_fes_id_intern + 1, min_fes_id_intern);
            }
            else
            {
                fes_id_ndx = MIN(min_fes_id_intern - 1, max_fes_id_intern);
            }
            while (TRUE)
            {
                
                if (is_direction_up)
                {
                    if (!(((signed int) fes_id_ndx) <= max_fes_id_hard && (*found_allocation_p) == FALSE))
                    {
                        break;
                    }
                }
                else
                {
                    if (!(((signed int) fes_id_ndx) >= min_fes_id_hard && (*found_allocation_p) == FALSE))
                    {
                        break;
                    }
                }

                if (banned_fes_id[fes_id_ndx] == FALSE && after_invalidate_next[fes_id_ndx] == FALSE
                    && allocable[fes_id_ndx] == FALSE && allocable_if_evacuated[fes_id_ndx])
                {
                    
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                                    (unit, field_stage, context_id,
                                     banned_fes_id, after_invalidate_next,
                                     &(current_state_changing_p->fes_quartet_info[fes_id_ndx]),
                                     fes_id_ndx,
                                     0, dnx_data_field.stage.stage_info_get(unit,
                                                                            field_stage)->nof_fes_instruction_per_context,
                                     FALSE, FALSE, allow_fes_pgm_sharing, current_state_changing_p, alloc_result_p,
                                     &move_succeeded));
                    if (move_succeeded)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                        (unit, field_stage, context_id,
                                         fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                                         fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                                         current_state_changing_p, alloc_result_p));
                        (*found_allocation_p) = TRUE;
                    }
                }

                
                if (is_direction_up)
                {
                    fes_id_ndx++;
                }
                else
                {
                    if (fes_id_ndx == 0)
                    {
                        break;
                    }
                    fes_id_ndx--;
                }
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_non_mandatory_position(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    dnx_field_fes_id_t fes_id_currently_placed,
    int min_fes_id,
    int max_fes_id,
    int allow_contention_moving,
    int allow_non_contention_moving,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p)
{
    int min_fes_id_intern;
    int max_fes_id_intern;
    dnx_field_fes_id_t fes_id_order[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_id_order_ndx;
    uint8 contention[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_algo_field_action_fes_alloc_priority_compare_result_e compare_result;
    uint8 shareable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 allocable_if_evacuated[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int nof_fes_id_order;
    int min_in_array;
    int max_in_array;
    dnx_field_fes_pgm_id_t fes_pgm_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_mask_id_t
        fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT]
        [DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int is_new;
    int move_succeeded;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(banned_fes_id, _SHR_E_PARAM, "banned_fes_id");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");
    SHR_NULL_CHECK(found_allocation_p, _SHR_E_PARAM, "found_allocation_p");

    
    nof_fes_id_order = 0;
    compare_result = DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_NOF;

    is_new =
        (fes_id_currently_placed >=
         dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context);

    (*found_allocation_p) = FALSE;

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_find
                    (unit, field_stage, context_id, current_state_changing_p, fes_id_currently_placed,
                     min_fes_id, max_fes_id, fes_quartet_p, contention));

    min_fes_id_intern = min_fes_id;
    max_fes_id_intern = max_fes_id;

    
    if (fes_quartet_p->priority != BCM_FIELD_ACTION_DONT_CARE)
    {
        min_in_array =
            (DNX_FIELD_ACTION_PRIORITY_ARRAY(fes_quartet_p->priority) / 2) *
            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array;
        max_in_array = min_in_array + dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_id_per_array - 1;
    }
    else
    {
        min_in_array = 0;
        max_in_array = dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1;
    }
    if (min_fes_id_intern < min_in_array)
    {
        min_fes_id_intern = min_in_array;
    }
    if (max_fes_id_intern > max_in_array)
    {
        max_fes_id_intern = max_in_array;
    }

    if (min_fes_id > max_in_array || max_fes_id < min_in_array)
    {
        SHR_EXIT();
    }

    
    if (fes_quartet_p->priority != BCM_FIELD_ACTION_DONT_CARE)
    {
        for (fes_id_ndx = 0;
             fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
             fes_id_ndx++)
        {
            if (fes_id_ndx == fes_id_currently_placed)
            {
                continue;
            }
            
            if (contention[fes_id_ndx] &&
                (current_state_changing_p->fes_quartet_info[fes_id_ndx].priority != BCM_FIELD_ACTION_DONT_CARE) &&
                (FALSE ==
                 DNX_FIELD_ACTION_PRIORITY_IS_POSITION(current_state_changing_p->fes_quartet_info
                                                       [fes_id_ndx].priority)))
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_priority_compare
                                (unit,
                                 fes_quartet_p->priority, fes_quartet_p->fg_id, fes_quartet_p->place_in_fg,
                                 current_state_changing_p->fes_quartet_info[fes_id_ndx].priority,
                                 current_state_changing_p->fes_quartet_info[fes_id_ndx].fg_id,
                                 current_state_changing_p->fes_quartet_info[fes_id_ndx].place_in_fg, &compare_result));
                if ((compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_FISRT_WINS) &&
                    (min_fes_id_intern <= (signed int) fes_id_ndx))
                {
                    min_fes_id_intern = fes_id_ndx + 1;
                }
                if ((compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_SECOND_WINS) &&
                    (max_fes_id_intern >= (signed int) fes_id_ndx))
                {
                    max_fes_id_intern = fes_id_ndx - 1;
                }
                
                if (compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_NOF ||
                    compare_result == DNX_ALGO_FILED_ACTION_FES_ALLOC_PRIORITY_COMPARE_RESULT_INCOMPARABLE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Priority comparison not performed. \r\n");
                }
            }
        }
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_order
                    (unit, field_stage, banned_fes_id, after_invalidate_next,
                     fes_quartet_p, fes_id_currently_placed, min_fes_id_intern, max_fes_id_intern,
                     allow_fes_pgm_sharing, current_state_changing_p,
                     fes_id_order, &nof_fes_id_order, shareable, allocable, allocable_if_evacuated,
                     fes_pgm_id, fes_mask_id));

    
    for (fes_id_order_ndx = 0; fes_id_order_ndx < nof_fes_id_order && (*found_allocation_p) == FALSE;
         fes_id_order_ndx++)
    {
        fes_id_ndx = fes_id_order[fes_id_order_ndx];
        (*found_allocation_p) = TRUE;
        if (allocable[fes_id_ndx])
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                            (unit, field_stage, context_id,
                             fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                             fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                             current_state_changing_p, alloc_result_p));
        }
        else if (allocable_if_evacuated[fes_id_ndx] && allow_non_contention_moving)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id,
                             banned_fes_id, after_invalidate_next,
                             &(current_state_changing_p->fes_quartet_info[fes_id_ndx]),
                             fes_id_ndx,
                             0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             FALSE, FALSE, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, &move_succeeded));
            if (move_succeeded)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                (unit, field_stage, context_id,
                                 fes_quartet_p, is_new, fes_id_currently_placed, fes_id_ndx,
                                 fes_pgm_id[fes_id_ndx], shareable[fes_id_ndx], fes_mask_id[fes_id_ndx],
                                 current_state_changing_p, alloc_result_p));
            }
            else
            {
                (*found_allocation_p) = FALSE;
            }
        }
        else
        {
            (*found_allocation_p) = FALSE;
        }
    }

    
    if ((*found_allocation_p) == TRUE && alloc_result_p == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation for allocable FES ID failed. \r\n");
    }

    
    if (allow_contention_moving)
    {
        
        if ((*found_allocation_p) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position_push
                            (unit, field_stage, TRUE, min_fes_id_intern, max_fes_id_intern,
                             context_id, banned_fes_id, after_invalidate_next,
                             fes_quartet_p, fes_id_currently_placed,
                             is_new, min_fes_id, max_fes_id, min_in_array, max_in_array,
                             allow_contention_moving, allow_non_contention_moving, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, found_allocation_p));
        }
        
        if ((*found_allocation_p) == FALSE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position_push
                            (unit, field_stage, FALSE, min_fes_id_intern, max_fes_id_intern,
                             context_id, banned_fes_id, after_invalidate_next,
                             fes_quartet_p, fes_id_currently_placed,
                             is_new, min_fes_id, max_fes_id, min_in_array, max_in_array,
                             allow_contention_moving, allow_non_contention_moving, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, found_allocation_p));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_mandatory_position(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT],
    dnx_algo_field_action_fes_alloc_fes_quartet_t * fes_quartet_p,
    int allow_fes_pgm_sharing,
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t * current_state_changing_p,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p,
    int *found_allocation_p)
{
    uint8 contention[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_field_fes_id_t requested_fes_id;
    uint8 shareable;
    uint8 allocable;
    uint8 allocable_if_evacuated;
    dnx_field_fes_pgm_id_t fes_pgm_id;
    dnx_field_fes_mask_id_t fes_mask_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES];
    int move_succeeded;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_changing_p, _SHR_E_PARAM, "current_state_changing_p");
    SHR_NULL_CHECK(banned_fes_id, _SHR_E_PARAM, "banned_fes_id");
    SHR_NULL_CHECK(after_invalidate_next, _SHR_E_PARAM, "after_invalidate_next");
    SHR_NULL_CHECK(fes_quartet_p, _SHR_E_PARAM, "fes_quartet_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");
    SHR_NULL_CHECK(found_allocation_p, _SHR_E_PARAM, "found_allocation_p");

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_contention_find
                    (unit, field_stage, context_id, current_state_changing_p,
                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                     0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context - 1,
                     fes_quartet_p, contention));

    
    DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(requested_fes_id, unit, field_stage, fes_quartet_p->priority);

    
    (*found_allocation_p) = FALSE;
    if (banned_fes_id[requested_fes_id] == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_is_allocable
                        (unit, field_stage, current_state_changing_p, requested_fes_id, fes_quartet_p,
                         allow_fes_pgm_sharing, TRUE, &shareable, &allocable, &allocable_if_evacuated,
                         &fes_pgm_id, fes_mask_id));
        (*found_allocation_p) = TRUE;
        if (allocable)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                            (unit, field_stage, context_id,
                             fes_quartet_p,
                             TRUE, dnx_data_field.stage.stage_info_get(unit,
                                                                       field_stage)->nof_fes_instruction_per_context,
                             requested_fes_id, fes_pgm_id, shareable, fes_mask_id, current_state_changing_p,
                             alloc_result_p));
        }
        else if (allocable_if_evacuated)
        {
            
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id,
                             banned_fes_id, after_invalidate_next,
                             &(current_state_changing_p->fes_quartet_info[requested_fes_id]),
                             requested_fes_id,
                             0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             TRUE, TRUE, allow_fes_pgm_sharing,
                             current_state_changing_p, alloc_result_p, &move_succeeded));
            if (move_succeeded)
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_add
                                (unit, field_stage, context_id,
                                 fes_quartet_p,
                                 TRUE, dnx_data_field.stage.stage_info_get(unit,
                                                                           field_stage)->nof_fes_instruction_per_context,
                                 requested_fes_id, fes_pgm_id, shareable, fes_mask_id, current_state_changing_p,
                                 alloc_result_p));
            }
            else
            {
                (*found_allocation_p) = FALSE;
            }
        }
        else
        {
            (*found_allocation_p) = FALSE;
        }
    }
    else
    {
        
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached mandatory placement with a banned FES ID, shouldn't happen.\r\n");
    }

    
    if ((*found_allocation_p) == TRUE && alloc_result_p == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation for allocable FES ID failed. \r\n");
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_fes_allocate_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    int post_attach,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    unsigned int action_ndx;
    unsigned int action_ndx_2;
    dnx_field_fes_id_t fes_id_priority;
    dnx_field_action_priority_t priority;
    dnx_field_action_priority_t priority_2;
    unsigned int fes_id_ndx;
    unsigned int num_existing_fes_quartets;
    unsigned int fes2msb_ndx;
    unsigned int fes2msb_2_ndx;
    unsigned int nof_masks;
    int mask_repeats;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_in_p, _SHR_E_PARAM, "current_state_in_p");
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    
    if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\" (%d). \r\n",
                     DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), field_stage);
    }
    
    if (fg_id == DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid field group id (%d) received. Context ID is %d.\r\n", fg_id, context_id);
    }
    
    if (context_id > dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID (%d) cannot exceed %d.\r\n",
                     context_id, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1);
    }
    
    if (fes_fg_in_p->nof_fes_quartets >
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) exceeds the maximum number of FESes for the stage (%d).\r\n",
                     fes_fg_in_p->nof_fes_quartets, dnx_data_field.stage.stage_info_get(unit,
                                                                                        field_stage)->nof_fes_instruction_per_context);
    }
    if (fes_fg_in_p->nof_fes_quartets > DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) exceeds the maximum number of actions per field group (%d).\r\n",
                     fes_fg_in_p->nof_fes_quartets, DNX_DATA_MAX_FIELD_GROUP_NOF_ACTION_PER_FG);
    }
    if (fes_fg_in_p->nof_fes_quartets <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) 0 is not supported at the moment. Field group %d, context ID %d.\r\n",
                     fes_fg_in_p->nof_fes_quartets, fg_id, context_id);
    }
    
    num_existing_fes_quartets = 0;
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        if (current_state_in_p->context_state.fes_id_info[fes_id_ndx].fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            if (fes_id_ndx >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "FES ID %d is marked as allocated, even though there are only %d FES IDs "
                             "in stage %d.\r\n",
                             fes_id_ndx,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             field_stage);
            }
            num_existing_fes_quartets++;
        }
    }
    
    if (fes_fg_in_p->nof_fes_quartets + num_existing_fes_quartets >
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "nof_fes_quartets (%d) plus existing fes quartets in context ID %d (%d) is %d, "
                     " exceeds the maximum number of FESes for the stage (%d).\r\n",
                     fes_fg_in_p->nof_fes_quartets, context_id, num_existing_fes_quartets,
                     fes_fg_in_p->nof_fes_quartets + num_existing_fes_quartets,
                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context);
    }
    
    for (action_ndx = 0; action_ndx < fes_fg_in_p->nof_fes_quartets; action_ndx++)
    {
        nof_masks = 0;
        for (fes2msb_ndx = 0; fes2msb_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_prog_per_fes;
             fes2msb_ndx++)
        {
            if (fes_fg_in_p->fes_quartet[action_ndx].fes_mask_id[fes2msb_ndx] != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
            {
                mask_repeats = FALSE;
                for (fes2msb_2_ndx = 0; fes2msb_2_ndx < fes2msb_ndx; fes2msb_2_ndx++)
                {
                    if (fes_fg_in_p->fes_quartet[action_ndx].fes_mask_id[fes2msb_ndx] ==
                        fes_fg_in_p->fes_quartet[action_ndx].fes_mask_id[fes2msb_2_ndx])
                    {
                        mask_repeats = TRUE;
                        break;
                    }
                }
                if (mask_repeats == FALSE)
                {
                    nof_masks++;
                }
            }
        }
        if (nof_masks > dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes - 1)
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Cannot allocate more than %d action masks per FES quartet.\r\n",
                         dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes - 1);
    }
    
    for (action_ndx = 0; action_ndx < fes_fg_in_p->nof_fes_quartets; action_ndx++)
    {
        priority = fes_fg_in_p->fes_quartet[action_ndx].priority;
        DNX_FIELD_ACTION_PRIORITY_VERIFY(priority);
        
        if (priority == DNX_FIELD_ACTION_PRIORITY_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%04x is invalid!\r\n", priority);
        }
        
        if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority))
        {
            DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id_priority, unit, field_stage, priority)
                
                if (fes_id_priority >=
                    dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%08x points to FES ID %d, "
                             "but there are only %d FESes in stage %s.\r\n",
                             priority,
                             fes_id_priority,
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
            }
        }
        
        if (priority != BCM_FIELD_ACTION_DONT_CARE)
        {
            
            if (DNX_FIELD_ACTION_PRIORITY_ARRAY(priority) >=
                dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array * 2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%04x points to FES array %d, "
                             "but there are only %d FES arrays in stage %s.\r\n",
                             priority,
                             DNX_FIELD_ACTION_PRIORITY_ARRAY(priority),
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array,
                             DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
            }
            if (DNX_FIELD_ACTION_PRIORITY_ARRAY(priority) % 2 != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Priority 0x%04x points to array %d, "
                             "which is not a FEM array in stage %s.\r\n",
                             priority,
                             DNX_FIELD_ACTION_PRIORITY_ARRAY(priority),
                             DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
            }
        }
    }
    
    for (action_ndx = 0; action_ndx < fes_fg_in_p->nof_fes_quartets; action_ndx++)
    {
        priority = fes_fg_in_p->fes_quartet[action_ndx].priority;
        if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority))
        {
            
            DNX_FIELD_ACTION_PRIORITY_POSITION_FES_ID_GET(fes_id_priority, unit, field_stage, priority)
                if (current_state_in_p->context_state.fes_id_info[fes_id_priority].fes_pgm_id !=
                    DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
            {
                switch (field_stage)
                {
                    case DNX_FIELD_STAGE_IPMF1:
                    case DNX_FIELD_STAGE_IPMF2:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                        priority.get(unit, context_id, fes_id_priority, &priority_2));
                        break;
                    case DNX_FIELD_STAGE_IPMF3:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                        priority.get(unit, context_id, fes_id_priority, &priority_2));
                        break;
                    case DNX_FIELD_STAGE_EPMF:
                        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                        priority.get(unit, context_id, fes_id_priority, &priority_2));
                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                }
                if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(priority_2))
                {
                    if (priority != priority_2)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Two position pirorities 0x%x and 0x%x on the same EFES "
                                     "ID %d. \r\n", priority, priority_2, fes_id_priority);
                    }
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The position priority 0x%x was provided twice on context %d "
                                 "stage \"%s\". \r\n", priority, context_id,
                                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
                }
            }
            
            for (action_ndx_2 = 0; action_ndx_2 < action_ndx; action_ndx_2++)
            {
                priority_2 = fes_fg_in_p->fes_quartet[action_ndx_2].priority;
                if (priority == priority_2)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "The position priority 0x%x was provided twice on context %d "
                                 "stage \"%s\". \r\n", priority, context_id,
                                 DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage));
                }
            }
        }
    }

    
    if (post_attach && (fes_fg_in_p->nof_fes_quartets > 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Post attach, we only support adding one EFES at a time. nof_fes_quartets is %d. "
                     "Field group %d, context ID %d.\r\n", fes_fg_in_p->nof_fes_quartets, fg_id, context_id);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_fes_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_fes_alloc_in_t * fes_fg_in_p,
    int allow_fes_pgm_sharing,
    int post_attach,
    dnx_algo_field_action_fes_alloc_out_t * alloc_result_p)
{
    int found_allocation;
    dnx_algo_field_action_fes_alloc_current_state_inclusive_t current_state_changing;
    unsigned int first_place_in_fg;
    uint8 after_invalidate_next[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 after_invalidate_next_new[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    uint8 banned_fes_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT];
    dnx_algo_field_action_fes_alloc_fes_quartet_t fes_quartet[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP];
    unsigned int fes_quartet_ndx;
    dnx_field_fes_id_t fes_id_ndx;
    unsigned int fes_change_ndx;
    unsigned int nof_invalidated_quartets;
    unsigned int nof_allocation_holes;
    int last_new_fes_change_ndx;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_action_fes_allocate_verify
                          (unit, field_stage, fg_id, context_id, current_state_in_p, fes_fg_in_p, post_attach,
                           alloc_result_p));

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_collect_current_state_info
                    (unit, field_stage, fg_id, context_id, post_attach, current_state_in_p,
                     &current_state_changing, after_invalidate_next, &first_place_in_fg));
    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_collect_new_actions_info
                    (unit, field_stage, fg_id, context_id, fes_fg_in_p, first_place_in_fg,
                     after_invalidate_next, after_invalidate_next_new, fes_quartet));

    
    alloc_result_p->nof_fes_quartet_changes = 0;
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        banned_fes_id[fes_id_ndx] = FALSE;
    }

    
    

    
    found_allocation = FALSE;
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((after_invalidate_next_new[fes_id_ndx]) &&
            (current_state_changing.fes_pgm_id[fes_id_ndx] != DNX_ALGO_FIELD_ZERO_FES_PROGRAM) &&
            (DNX_FIELD_ACTION_PRIORITY_IS_POSITION
             (current_state_changing.fes_quartet_info[fes_id_ndx].priority) == FALSE))
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id,
                             banned_fes_id, after_invalidate_next,
                             &(current_state_changing.fes_quartet_info[fes_id_ndx]),
                             fes_id_ndx, 0, dnx_data_field.stage.stage_info_get(unit,
                                                                                field_stage)->nof_fes_instruction_per_context,
                             TRUE, TRUE, allow_fes_pgm_sharing, &current_state_changing, alloc_result_p,
                             &found_allocation));

            if (found_allocation == FALSE)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE,
                             "Failed to move aside existing FES instructions in field group (%d), context ID (%d), "
                             "action %d with non mandatory placement (priority 0x%x) from FES ID %d, to clear "
                             "the FES ID after a new invalidate next.\r\n",
                             fg_id, context_id, current_state_changing.fes_quartet_info[fes_id_ndx].place_in_fg,
                             current_state_changing.fes_quartet_info[fes_id_ndx].priority, fes_id_ndx);
            }
        }
    }

    
    nof_invalidated_quartets = 0;
    found_allocation = FALSE;
    for (fes_quartet_ndx = 0; fes_quartet_ndx < fes_fg_in_p->nof_fes_quartets; fes_quartet_ndx++)
    {
        if (fes_quartet[fes_quartet_ndx].priority == BCM_FIELD_ACTION_INVALIDATE)
        {
            nof_invalidated_quartets++;
            continue;
        }
        if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(fes_quartet[fes_quartet_ndx].priority))
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_mandatory_position
                            (unit, field_stage, context_id, banned_fes_id, after_invalidate_next,
                             &fes_quartet[fes_quartet_ndx], allow_fes_pgm_sharing,
                             &current_state_changing, alloc_result_p, &found_allocation));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_allocate_non_mandatory_position
                            (unit, field_stage, context_id, banned_fes_id,
                             after_invalidate_next, &fes_quartet[fes_quartet_ndx],
                             dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context,
                             TRUE, TRUE, allow_fes_pgm_sharing,
                             &current_state_changing, alloc_result_p, &found_allocation));
        }

        if (found_allocation == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "\r\n"
                         "Failed to allocate FES instructions for field group (%d) context ID (%d). "
                         "Failed allocating action %d of the field group.\r\n",
                         fg_id, context_id, fes_quartet[fes_quartet_ndx].place_in_fg);
        }
    }

    
    last_new_fes_change_ndx = -1;
    for (fes_change_ndx = 0, fes_quartet_ndx = 0, nof_allocation_holes = 0;
         fes_change_ndx < alloc_result_p->nof_fes_quartet_changes; fes_change_ndx++)
    {
        if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc != DNX_ALGO_FIELD_ACTION_MOVEMENT)
        {
            if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Negative 'place_in_alloc' that isn't movement found "
                             "(value %d fes quartet change %d).\r\n",
                             alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc, fes_change_ndx);
            }
            if (fes_quartet_ndx == 0)
            {
                nof_allocation_holes += alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc;
            }
            else
            {
                if (alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc <=
                    alloc_result_p->fes_quartet_change[last_new_fes_change_ndx].place_in_alloc)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Non consecutive 'place_in_alloc' found (value %d fes quartet change %d "
                                 "action %d previous %d).\r\n",
                                 alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc,
                                 fes_change_ndx, fes_quartet_ndx, last_new_fes_change_ndx);
                }
                else
                {
                    nof_allocation_holes +=
                        alloc_result_p->fes_quartet_change[fes_change_ndx].place_in_alloc -
                        alloc_result_p->fes_quartet_change[last_new_fes_change_ndx].place_in_alloc - 1;
                }
            }
            last_new_fes_change_ndx = fes_change_ndx;
            fes_quartet_ndx++;
        }
    }
    if (nof_allocation_holes > nof_invalidated_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "FES allocation algorithm has %d allocation holes, but only %d invalidate priority quartets.\r\n",
                     nof_allocation_holes, nof_invalidated_quartets);
    }
    if (fes_quartet_ndx + nof_invalidated_quartets != fes_fg_in_p->nof_fes_quartets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "FES allocation algorithm should have allocated %d FES quartets, allocated %d.\r\n",
                     fes_fg_in_p->nof_fes_quartets, fes_quartet_ndx);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_fes_dealloc_sw_state(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int fes_pgm_id;
    unsigned int mask_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dealloc_result_p, _SHR_E_PARAM, "dealloc_result_p");

    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        
        fes_pgm_id = dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx];
        if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                                    priority.set(unit, context_id, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                    field_group.set(unit, fes_id_ndx, fes_pgm_id, DNX_FIELD_GROUP_INVALID));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                    place_in_fg.set(unit, fes_id_ndx, fes_pgm_id, 0));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                                    priority.set(unit, context_id, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                    field_group.set(unit, fes_id_ndx, fes_pgm_id, DNX_FIELD_GROUP_INVALID));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                    place_in_fg.set(unit, fes_id_ndx, fes_pgm_id, 0));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                                    priority.set(unit, context_id, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                    field_group.set(unit, fes_id_ndx, fes_pgm_id, DNX_FIELD_GROUP_INVALID));
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                    place_in_fg.set(unit, fes_id_ndx, fes_pgm_id, 0));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            
            for (mask_ndx = 0; mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                 mask_ndx++)
            {
                
                if (dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx])
                {
                    
                    if (mask_ndx == DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Zero action mask marked for deletion for context ID %d, FES ID %d. \r\n",
                                     context_id, fes_id_ndx);
                    }
                    
                    switch (field_stage)
                    {
                        case DNX_FIELD_STAGE_IPMF1:
                        case DNX_FIELD_STAGE_IPMF2:
                            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                            mask_is_alloc.set(unit, fes_id_ndx, mask_ndx, FALSE));
                            break;
                        case DNX_FIELD_STAGE_IPMF3:
                            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                            mask_is_alloc.set(unit, fes_id_ndx, mask_ndx, FALSE));
                            break;
                        case DNX_FIELD_STAGE_EPMF:
                            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                            mask_is_alloc.set(unit, fes_id_ndx, mask_ndx, FALSE));
                            break;
                            
                             
                        default:
                        {
                            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_fes_dealloc_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_algo_field_action_fes_dealloc_state_in_t * current_state_dealloc_in_p,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_dealloc_in_p, _SHR_E_PARAM, "current_state_dealloc_in_p");
    SHR_NULL_CHECK(dealloc_result_p, _SHR_E_PARAM, "dealloc_result_p");

    
    if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\" (%d). \r\n",
                     DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), field_stage);
    }
    
    if (fg_id == DNX_FIELD_GROUP_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid field group id received (%d). Context ID is %d.\r\n", fg_id, context_id);
    }
    
    if ((unsigned int) context_id > (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Invalid context ID id received (%d), must be between %d and %d. Field group id is %d.\r\n",
                     context_id, 0, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1, fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_fes_dealloc(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_group_t fg_id,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_algo_field_action_fes_dealloc_state_in_t * current_state_dealloc_in_p,
    dnx_algo_field_action_fes_dealloc_out_t * dealloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int mask_ndx;
    unsigned int fes_pgm_id;
    dnx_field_group_t curr_fg_id;
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_action_fes_dealloc_verify
                          (unit, field_stage, fg_id, context_id, current_state_dealloc_in_p, dealloc_result_p));
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        dealloc_result_p->belongs_to_fg[fes_id_ndx] = FALSE;
        dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx] = DNX_ALGO_FIELD_ZERO_FES_PROGRAM;
        for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
        {
            dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx] = FALSE;
        }
    }
    
    for (fes_id_ndx = 0;
         fes_id_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context;
         fes_id_ndx++)
    {
        if ((fes_id != DNX_FIELD_EFES_ID_INVALID) && (fes_id != fes_id_ndx))
        {
            continue;
        }
        fes_pgm_id = current_state_dealloc_in_p->context_state.fes_pgm_id[fes_id_ndx];
        if (fes_pgm_id != DNX_ALGO_FIELD_ZERO_FES_PROGRAM)
        {
            
            switch (field_stage)
            {
                case DNX_FIELD_STAGE_IPMF1:
                case DNX_FIELD_STAGE_IPMF2:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                                    field_group.get(unit, fes_id_ndx, fes_pgm_id, &curr_fg_id));
                    break;
                case DNX_FIELD_STAGE_IPMF3:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                                    field_group.get(unit, fes_id_ndx, fes_pgm_id, &curr_fg_id));
                    break;
                case DNX_FIELD_STAGE_EPMF:
                    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                                    field_group.get(unit, fes_id_ndx, fes_pgm_id, &curr_fg_id));
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
            }
            if (curr_fg_id == fg_id)
            {
                dealloc_result_p->belongs_to_fg[fes_id_ndx] = TRUE;
                
                if (current_state_dealloc_in_p->context_state.fes_quartet_nof_refs[fes_id_ndx] == 1)
                {
                    
                    dealloc_result_p->fes_pgm_id_to_delete[fes_id_ndx] =
                        current_state_dealloc_in_p->context_state.fes_pgm_id[fes_id_ndx];
                    for (mask_ndx = 0;
                         mask_ndx < dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_masks_per_fes;
                         mask_ndx++)
                    {
                        if ((mask_ndx != DNX_ALGO_FIELD_ZERO_FES_ACTION_MASK_ID) &&
                            (current_state_dealloc_in_p->context_state.mask_nof_refs[fes_id_ndx][mask_ndx] == 1))
                        {
                            
                            dealloc_result_p->delete_mask[fes_id_ndx][mask_ndx] = TRUE;
                        }
                    }
                }
            }
        }
    }
    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_dealloc_sw_state(unit, field_stage, context_id, dealloc_result_p));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_algo_field_action_ace_fes_allocate_verify(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_algo_field_action_ace_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_ace_fes_alloc_in_t * fes_fg_in_p,
    dnx_algo_field_action_ace_fes_alloc_out_t * alloc_result_p)
{
    unsigned int fes_id_ndx;
    unsigned int fes_mask_ndx;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(current_state_in_p, _SHR_E_PARAM, "current_state_in_p");
    SHR_NULL_CHECK(fes_fg_in_p, _SHR_E_PARAM, "fes_fg_in_p");
    SHR_NULL_CHECK(alloc_result_p, _SHR_E_PARAM, "alloc_result_p");

    
    if (ace_id > dnx_data_field.ace.nof_ace_id_get(unit) - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ACE ID (%d) cannot exceed %d.\r\n",
                     ace_id, dnx_data_field.ace.nof_ace_id_get(unit) - 1);
    }
    
    if (fes_fg_in_p->nof_actions > dnx_data_field.ace.nof_fes_instruction_per_context_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of actions (%d) exceeds the maximum number of FESes for ACE (%d).\r\n",
                     fes_fg_in_p->nof_actions, dnx_data_field.ace.nof_fes_instruction_per_context_get(unit));
    }
    else if (fes_fg_in_p->nof_actions == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions must be at least one.\r\n");
    }
    if (fes_fg_in_p->nof_actions > dnx_data_field.ace.nof_action_per_ace_format_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_fes_quartets (%d) exceeds the maximum number of actions per field group (%d).\r\n",
                     fes_fg_in_p->nof_actions, dnx_data_field.ace.nof_action_per_ace_format_get(unit));
    }
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
        {
            if (fes_mask_ndx == DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
            {
                if (current_state_in_p->masks[fes_id_ndx][fes_mask_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "In FES ID %d in ACE ACR, zero mask isn't composed of all zeroes.\r\n", fes_id_ndx);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_ace_fes_allocate(
    int unit,
    dnx_field_ace_id_t ace_id,
    dnx_algo_field_action_ace_fes_alloc_state_in_t * current_state_in_p,
    dnx_algo_field_action_ace_fes_alloc_in_t * fes_fg_in_p,
    dnx_algo_field_action_ace_fes_alloc_out_t * alloc_result_p)
{
    unsigned int num_masks_to_alloc;
    unsigned int num_fes_available_for_mask;
    unsigned int num_shareable;
    unsigned int fes_inst_ndx;
    unsigned int fes_id_ndx;
    unsigned int fes_mask_ndx;
    unsigned int fes_available_for_mask[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int available_mask_id[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int fes_allocated[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    unsigned int instr_allocated[DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT];
    int shared_fes_id_found;

    SHR_FUNC_INIT_VARS(unit);

    

    
    SHR_INVOKE_VERIFY_DNX(dnx_algo_field_action_ace_fes_allocate_verify
                          (unit, ace_id, current_state_in_p, fes_fg_in_p, alloc_result_p));

    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        fes_allocated[fes_id_ndx] = FALSE;
    }
    
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        instr_allocated[fes_inst_ndx] = FALSE;
    }

    
    num_masks_to_alloc = 0;
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        if (fes_fg_in_p->masks[fes_inst_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
        {
            num_masks_to_alloc++;
        }
    }

    
    if (num_masks_to_alloc > 0)
    {
        
        num_fes_available_for_mask = 0;
        for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
        {
            fes_available_for_mask[fes_id_ndx] = FALSE;
            for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
            {
                if (fes_mask_ndx != DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
                {
                    if (current_state_in_p->masks[fes_id_ndx][fes_mask_ndx] == DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
                    {
                        fes_available_for_mask[fes_id_ndx] = TRUE;
                        available_mask_id[fes_id_ndx] = fes_mask_ndx;
                        num_fes_available_for_mask++;
                        break;
                    }
                }
            }
        }
        
        num_shareable = 0;
        for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
        {
            shared_fes_id_found = FALSE;
            if (fes_fg_in_p->masks[fes_inst_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK)
            {
                for (fes_id_ndx = 0;
                     fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT
                     && shared_fes_id_found == FALSE; fes_id_ndx++)
                {
                    if (fes_allocated[fes_id_ndx] == FALSE)
                    {
                        for (fes_mask_ndx = 0; fes_mask_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_MASKS_PER_FES; fes_mask_ndx++)
                        {
                            if (fes_mask_ndx != DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID)
                            {
                                if (current_state_in_p->masks[fes_id_ndx][fes_mask_ndx] ==
                                    fes_fg_in_p->masks[fes_inst_ndx])
                                {
                                    fes_allocated[fes_id_ndx] = TRUE;
                                    instr_allocated[fes_inst_ndx] = TRUE;
                                    alloc_result_p->fes_id[fes_inst_ndx] = fes_id_ndx;
                                    alloc_result_p->mask_id[fes_inst_ndx] = fes_mask_ndx;
                                    num_shareable++;
                                    shared_fes_id_found = TRUE;
                                    
                                    if (fes_available_for_mask[fes_id_ndx])
                                    {
                                        fes_available_for_mask[fes_id_ndx] = FALSE;
                                        num_fes_available_for_mask--;
                                    }
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (num_fes_available_for_mask < num_masks_to_alloc - num_shareable)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE,
                         "ACE ID %d: Number of FESes with action masks available for allocation is %d, "
                         "while umber of actions that require action mask allocation "
                         "that could not be shared is %d (shareable %d).\r\n",
                         ace_id, num_fes_available_for_mask, num_masks_to_alloc - num_shareable, num_shareable);
        }

        
        for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
        {
            if (fes_fg_in_p->masks[fes_inst_ndx] != DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK
                && instr_allocated[fes_inst_ndx] == FALSE)
            {
                for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
                {
                    if (fes_allocated[fes_id_ndx] == FALSE && fes_available_for_mask[fes_id_ndx])
                    {
                        fes_allocated[fes_id_ndx] = TRUE;
                        instr_allocated[fes_inst_ndx] = TRUE;
                        alloc_result_p->fes_id[fes_inst_ndx] = fes_id_ndx;
                        alloc_result_p->mask_id[fes_inst_ndx] = available_mask_id[fes_id_ndx];;
                        break;
                    }
                }
            }
        }
    }

    
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        if (fes_fg_in_p->masks[fes_inst_ndx] == DNX_ALGO_FIELD_ALL_ZERO_FES_ACTION_MASK
            && instr_allocated[fes_inst_ndx] == FALSE)
        {
            
            for (fes_id_ndx = fes_inst_ndx; fes_id_ndx < DNX_DATA_MAX_FIELD_ACE_NOF_FES_INSTRUCTION_PER_CONTEXT;
                 fes_id_ndx++)
            {
                if (fes_allocated[fes_id_ndx] == FALSE)
                {
                    fes_allocated[fes_id_ndx] = TRUE;
                    instr_allocated[fes_inst_ndx] = TRUE;
                    alloc_result_p->fes_id[fes_inst_ndx] = fes_id_ndx;
                    alloc_result_p->mask_id[fes_inst_ndx] = DNX_ALGO_FIELD_ZERO_FES_ACE_ACTION_MASK_ID;
                    break;
                }
            }
        }
    }
    
    alloc_result_p->nof_actions = fes_fg_in_p->nof_actions;

    
    for (fes_inst_ndx = 0; fes_inst_ndx < fes_fg_in_p->nof_actions; fes_inst_ndx++)
    {
        instr_allocated[fes_inst_ndx] = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_fes_quartet_sw_state_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t * fes_quartet_sw_state_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fes_quartet_sw_state_info_p, _SHR_E_PARAM, "fes_quartet_sw_state_info_p");

    
    if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_array <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage \"%s\" (%d). \r\n",
                     DNX_ALGO_FIELD_ACTION_STAGE_NAME_COMBINED(unit, field_stage), field_stage);
    }
    
    if ((unsigned int) context_id >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID (%d) must be between zero and %d. \r\n",
                     context_id, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_contexts - 1);
    }
    if ((unsigned int) fes_id >=
        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_instruction_per_context)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FES ID (%d) must be between zero and %d. \r\n",
                     fes_id, dnx_data_field.stage.stage_info_get(unit,
                                                                 field_stage)->nof_fes_instruction_per_context - 1);
    }
    if ((unsigned int) fes_pgm_id >= dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FES program ID (%d) must be between zero and %d. \r\n",
                     fes_pgm_id, dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_fes_programs - 1);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_fes_quartet_sw_state_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_fes_id_t fes_id,
    dnx_field_fes_pgm_id_t fes_pgm_id,
    dnx_algo_field_action_fes_quartet_sw_state_get_info_t * fes_quartet_sw_state_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_fes_quartet_sw_state_info_get_verify
                    (unit, field_stage, context_id, fes_id, fes_pgm_id, fes_quartet_sw_state_info_p));

    
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.get(unit, context_id, fes_id, &(fes_quartet_sw_state_info_p->priotrity)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            field_group.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->fg_id)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.
                            place_in_fg.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->place_in_fg)));
            break;
        case DNX_FIELD_STAGE_IPMF3:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                            priority.get(unit, context_id, fes_id, &(fes_quartet_sw_state_info_p->priotrity)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            field_group.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->fg_id)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.
                            place_in_fg.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->place_in_fg)));
            break;
        case DNX_FIELD_STAGE_EPMF:
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                            priority.get(unit, context_id, fes_id, &(fes_quartet_sw_state_info_p->priotrity)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                            field_group.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->fg_id)));
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.
                            place_in_fg.get(unit, fes_id, fes_pgm_id, &(fes_quartet_sw_state_info_p->place_in_fg)));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage (%d). \r\n", field_stage);
    }
    
    if (fes_quartet_sw_state_info_p->priotrity == DNX_FIELD_ACTION_PRIORITY_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context ID %d has no FES quartet allocated in FES ID %d. \r\n", context_id, fes_id);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_init(
    int unit)
{
    unsigned int fes_id_ndx;
    unsigned int fes_pgm_ndx;
    unsigned int mask_ndx;
    dnx_algo_field_action_fes_state_t zero_fes_state;
    unsigned int context_ndx;
    SHR_FUNC_INIT_VARS(unit);

    
    if (DNX_FIELD_ACTION_PRIORITY_INVALID & (1 << BCM_FIELD_ACTION_VALID_OFFSET))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX_FIELD_ACTION_PRIORITY_INVALID (0x%x) has "
                     "the bit BCM_FIELD_ACTION_VALID_OFFSET (0x%x) set.\r\n",
                     DNX_FIELD_ACTION_PRIORITY_INVALID, (1 << BCM_FIELD_ACTION_VALID_OFFSET));
    }
    if (BCM_FIELD_ACTION_INVALIDATE & (1 << BCM_FIELD_ACTION_VALID_OFFSET))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "BCM_FIELD_ACTION_INVALIDATE (0x%x) has "
                     "the bit BCM_FIELD_ACTION_VALID_OFFSET (0x%x) set.\r\n",
                     BCM_FIELD_ACTION_INVALIDATE, (1 << BCM_FIELD_ACTION_VALID_OFFSET));
    }
    if (DNX_FIELD_ACTION_PRIORITY_INVALID == BCM_FIELD_ACTION_INVALIDATE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DNX_FIELD_ACTION_PRIORITY_INVALID (0x%x) is the "
                     "same as BCM_FIELD_ACTION_INVALIDATE (0x%x).\r\n",
                     DNX_FIELD_ACTION_PRIORITY_INVALID, BCM_FIELD_ACTION_INVALIDATE);
    }

    
    SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.init(unit));
    
    for (fes_pgm_ndx = 0; fes_pgm_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FES_PROGRAMS; fes_pgm_ndx++)
    {
        zero_fes_state.field_group[fes_pgm_ndx] = DNX_FIELD_GROUP_INVALID;
        zero_fes_state.place_in_fg[fes_pgm_ndx] = 0;
    }
    for (mask_ndx = 0; mask_ndx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_MASKS_PER_FES; mask_ndx++)
    {
        zero_fes_state.mask_is_alloc[mask_ndx] = FALSE;
    }
    
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
        
        for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF2_NOF_CONTEXTS; context_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf2.context_state.
                            priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
        }
    }
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
        
        for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_IPMF3_NOF_CONTEXTS; context_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.ipmf3.context_state.
                            priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
        }
    }
    
    for (fes_id_ndx = 0; fes_id_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_FES_INSTRUCTION_PER_CONTEXT; fes_id_ndx++)
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.fes_state.set(unit, fes_id_ndx, &zero_fes_state));
        
        for (context_ndx = 0; context_ndx < DNX_DATA_MAX_FIELD_BASE_EPMF_NOF_CONTEXTS; context_ndx++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_action_sw.epmf.context_state.
                            priority.set(unit, context_ndx, fes_id_ndx, DNX_FIELD_ACTION_PRIORITY_INVALID));
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_algo_field_action_sw_state_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


