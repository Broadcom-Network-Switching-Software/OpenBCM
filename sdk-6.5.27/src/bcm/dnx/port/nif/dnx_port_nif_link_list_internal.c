/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include "dnx_port_nif_link_list_internal.h"
#include "dnx_port_nif_oft_internal.h"
#include "dnx_port_nif_ofr_internal.h"
#include <shared/pbmp.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/sand/shrextend/shrextend_error.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_nif.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/* Static functions */
/* General */
static shr_error_e dnx_port_single_priority_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int priority,
    int client_id);
static shr_error_e dnx_port_link_list_db_allocated_list_info_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int list_head,
    int nof_sections_allocated);
static shr_error_e dnx_port_link_list_db_allocated_list_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int *list_head,
    int *nof_sections_allocated);
static shr_error_e dnx_port_link_list_db_double_priority_allocated_list_info_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    int list_head,
    int nof_sections_allocated);
static shr_error_e dnx_port_link_list_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int first_section,
    int alloc_size,
    int client_id);
static shr_error_e dnx_port_double_priority_link_list_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dnx_port_nif_list_info_t * sub_lists_info,
    int nof_active_sub_lists,
    int client_id,
    int priority,
    int alloc_size);
static shr_error_e dnx_port_link_list_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int *list_head);
static shr_error_e dnx_port_double_priority_link_list_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    uint8 *sub_lists_data,
    dnx_port_nif_list_info_t * sub_lists_info);
static shr_error_e dnx_port_link_list_hw_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int list_head,
    int nof_sections);
static shr_error_e dnx_port_double_priority_link_list_hw_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dnx_port_nif_list_info_t * used_sub_list_info);
static shr_error_e dnx_port_link_list_db_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int list_head,
    int list_size);
static shr_error_e dnx_port_double_priority_link_list_db_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int priority,
    dnx_port_nif_list_info_t * used_sub_list_info);
static shr_error_e dnx_port_nif_double_priority_link_list_parameters_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *nof_sections_per_group,
    int *nof_groups);
static shr_error_e dnx_port_nif_double_priority_link_list_new_sub_list_parameters_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int list_head,
    int *sub_list_index,
    int *sub_list_size,
    int *sub_list_head);

/* Sw state */
static shr_error_e dnx_port_link_list_db_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections);
static shr_error_e dnx_port_nif_link_list_pointer_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type);

static shr_error_e dnx_port_double_priority_link_list_db_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sub_lists,
    int nof_sections);

static shr_error_e dnx_port_nif_double_priority_link_list_pointer_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type);

static shr_error_e dnx_port_nif_link_list_db_head_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int head);
static shr_error_e dnx_port_nif_double_priority_free_link_list_db_head_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int head);
static shr_error_e dnx_port_nif_link_list_db_tail_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int tail);
static shr_error_e dnx_port_nif_double_priority_free_link_list_db_tail_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int tail);
static shr_error_e dnx_port_nif_link_list_db_size_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int size);
static shr_error_e dnx_port_nif_double_priority_free_link_list_db_size_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int size);
static shr_error_e dnx_port_nif_link_list_db_head_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *head);
static shr_error_e dnx_port_nif_double_priority_free_link_list_db_head_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int *head);
static shr_error_e dnx_port_nif_link_list_db_tail_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *tail);
static shr_error_e dnx_port_nif_double_priority_free_link_list_db_tail_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int *tail);
static shr_error_e dnx_port_nif_link_list_db_size_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *size);
static shr_error_e dnx_port_nif_double_priority_free_link_list_db_size_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int *size);
static shr_error_e dnx_port_nif_link_list_pointer_db_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section);
static shr_error_e dnx_port_nif_double_priority_link_list_next_section_db_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section);
static shr_error_e dnx_port_nif_link_list_pointer_db_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int *next_section);
static shr_error_e dnx_port_nif_double_priority_link_list_next_section_db_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int *next_section);
static shr_error_e dnx_port_nif_link_list_nth_linking_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int section_index,
    int nof_steps,
    int *nth_section);
static shr_error_e dnx_port_nif_double_priority_link_list_nth_linking_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int nof_steps,
    int *nth_section);

static shr_error_e dnx_port_link_list_switch_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int first_section,
    int alloc_size,
    int client_id);

static shr_error_e dnx_port_double_priority_link_list_switch_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dnx_port_nif_list_info_t * sub_lists_info,
    int nof_active_sub_lists,
    int client_id,
    int priority);

static shr_error_e dnx_port_link_list_hw_internal_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int list_head,
    int nof_sections);

static shr_error_e dnx_port_nif_link_list_client_switch_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client,
    int head,
    int next,
    int nof_sections);

static shr_error_e dnx_port_nif_double_priority_link_list_client_switch_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    int head,
    int nof_sections);

static shr_error_e dnx_port_nif_arb_link_list_client_switch_set(
    int unit,
    int client,
    int head,
    int next,
    int nof_sections);

/* dbal */
static shr_error_e dnx_port_nif_link_list_pointers_table_id_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dbal_tables_e * table_id);
static shr_error_e dnx_port_nif_link_list_client_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client,
    int head,
    int next,
    int nof_sections);
static shr_error_e dnx_port_double_priority_nif_link_list_client_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client,
    int priority,
    int head,
    int next,
    int nof_sections);
static shr_error_e dnx_port_nif_link_list_pointer_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section);
static shr_error_e dnx_port_nif_double_priority_link_list_next_section_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section);
/*static*/ shr_error_e dnx_port_nif_link_list_next_section_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int *next_section);
static shr_error_e dnx_port_nif_ofr_link_list_client_set(
    int unit,
    int client,
    int priority,
    int head,
    int next,
    int nof_sections);
static shr_error_e dnx_port_nif_oft_link_list_client_set(
    int unit,
    int client,
    int head,
    int nof_sections);
static shr_error_e dnx_port_nif_arb_link_list_client_set(
    int unit,
    int client,
    int head,
    int next,
    int nof_sections);

shr_error_e
dnx_port_link_list_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections)
{
    int section, next_section;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_init(unit, list_type, nof_sections));

    for (section = 0; section < nof_sections; ++section)
    {
        next_section = (section + 1) % nof_sections;
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_set(unit, list_type, section, next_section));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_double_priority_link_list_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sub_lists,
    int nof_sections)
{
    int list_index, section_index, current_section, next_section;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_db_init(unit, list_type, nof_sub_lists, nof_sections));

    for (list_index = 0; list_index < nof_sub_lists; ++list_index)
    {
        for (section_index = 0; section_index < nof_sections; ++section_index)
        {
            current_section = section_index + list_index * nof_sections;
            next_section = (current_section + 1) % nof_sections + list_index * nof_sections;
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_set
                            (unit, list_type, current_section, next_section));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_link_list_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int client_id)
{
    /*
     * default initialize of list_head
     */
    int list_head = 0, nof_sections_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_allocated_list_info_get
                    (unit, list_type, client_id, &list_head, &nof_sections_allocated));

    /*
     * If a list is already allocated for this client free it first before re-allocation
     */
    if (nof_sections_allocated > 0)
    {
        SHR_IF_ERR_EXIT(dnx_port_link_list_free(unit, list_type, client_id));
    }

    /*
     * Allocate link list in DB
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_db_alloc(unit, list_type, alloc_size, &list_head));

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_allocated_list_info_set(unit, list_type, client_id, list_head, alloc_size));

    /*
     * Initializing OFT link list's registers
     */
    if (list_type == DNX_PORT_NIF_OFT_LINK_LIST)
    {
        SHR_IF_ERR_EXIT(dnx_port_oft_link_list_pre_alloc_init(unit, list_head, client_id));
    }

    /*
     * Setting new list in hw
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_hw_set(unit, list_type, list_head, alloc_size, client_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_link_list_switch(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int client_id)
{

    int list_head = 0, old_list_head, old_nof_sections;
    SHR_FUNC_INIT_VARS(unit);

    /** get old list data */
    SHR_IF_ERR_EXIT(dnx_port_link_list_db_allocated_list_info_get
                    (unit, list_type, client_id, &old_list_head, &old_nof_sections));

    /** Allocate new link list in db */
    SHR_IF_ERR_EXIT(dnx_port_link_list_db_alloc(unit, list_type, alloc_size, &list_head));

    /** Switch to new list in hw */
    SHR_IF_ERR_EXIT(dnx_port_link_list_switch_hw_set(unit, list_type, list_head, alloc_size, client_id));

    /** free old list */
    {
        /** free old list in hw */
        SHR_IF_ERR_EXIT(dnx_port_link_list_hw_internal_free(unit, list_type, old_list_head, old_nof_sections));

        /** free old list in db */
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_free(unit, list_type, old_list_head, old_nof_sections));
    }

    /** set new linked list size in sw db */
    SHR_IF_ERR_EXIT(dnx_port_link_list_db_allocated_list_info_set(unit, list_type, client_id, list_head, alloc_size));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_double_priority_link_list_switch(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    uint8 **alloc_info,
    int client_id)
{
    int nof_sections_per_group, nof_sub_lists, sub_list_index, priority;
    uint32 new_list_size, nof_active_sub_lists;
    dnx_port_nif_list_info_t new_sub_lists_info[DNX_DATA_MAX_NIF_OFR_NOF_RX_MEMORY_GROUPS] = { {0} };
    dnx_port_nif_list_info_t old_sub_lists_info[DNX_DATA_MAX_NIF_OFR_NOF_RX_MEMORY_GROUPS] = { {0} };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get sections groups parameters
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_sub_lists));

    /*
     * Iterating per priority
     */
    for (priority = 0; priority < dnx_data_nif.eth.priority_groups_nof_get(unit); ++priority)
    {
        /*
         * Summing total allocation size of this priority.
         */
        new_list_size = 0;
        nof_active_sub_lists = 0;
        for (sub_list_index = 0; sub_list_index < nof_sub_lists; ++sub_list_index)
        {
            if (alloc_info[priority][sub_list_index] > 0)
            {
                new_list_size += alloc_info[priority][sub_list_index];
                ++nof_active_sub_lists;
            }
        }

        /*
         * Check if need to allocate list for this priority
         */
        if (new_list_size == 0)
        {
            continue;
        }

        /*
         * Fill old_sub_lists_info with four-tuple: {sub list index, head, tail, size} per sub list.
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_used_sub_lists_info_get
                        (unit, list_type, client_id, priority, old_sub_lists_info));

        /*
         * Allocate new link list in DB.
         * Elements in alloc_info contains pairs of {free_list_index, size_to_alloc}.
         * Elements in sub_lists_info are four-tuples of {head, tail, size, sub_list_id}.
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_db_alloc
                        (unit, list_type, alloc_info[priority], new_sub_lists_info));

        /*
         * Switch to new list in hw
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_switch_hw_set
                        (unit, list_type, new_sub_lists_info, nof_active_sub_lists, client_id, priority));

        /*
         * Free old link list in hw
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_hw_free(unit, list_type, old_sub_lists_info));

        /*
         * Free old link list in DB
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_db_free(unit, list_type, priority, old_sub_lists_info));

        /*
         * Set new nof sections allocated
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_allocated_list_info_set
                        (unit, list_type, client_id, priority, new_sub_lists_info[0].m_first_section, new_list_size));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_double_priority_link_list_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    uint8 **alloc_info,
    int client_id)
{
    int list_head, nof_sections_per_group, nof_sub_lists, sub_list_index, priority, nof_sections_allocated;
    uint32 new_list_size, nof_active_sub_lists;
    dnx_port_nif_list_info_t sub_lists_info[DNX_DATA_MAX_NIF_OFR_NOF_RX_MEMORY_GROUPS] = { {0} };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify alloc_size is available.
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_check_if_size_available
                    (unit, list_type, client_id, alloc_info));

    /*
     * Get sections groups parameters
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_sub_lists));

    /*
     * Iterating per priority
     */
    for (priority = 0; priority < dnx_data_nif.eth.priority_groups_nof_get(unit); ++priority)
    {
        /*
         * Summing total allocation size of this priority.
         */
        new_list_size = 0;
        nof_active_sub_lists = 0;
        for (sub_list_index = 0; sub_list_index < nof_sub_lists; ++sub_list_index)
        {
            if (alloc_info[priority][sub_list_index] > 0)
            {
                new_list_size += alloc_info[priority][sub_list_index];
                ++nof_active_sub_lists;
            }
        }

        /*
         * Check if need to allocate list for this priority
         */
        if (new_list_size == 0)
        {
            continue;
        }

        /*
         * If a list is already allocated for this client free it first before re-allocation
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_allocated_list_info_get
                        (unit, list_type, client_id, priority, &list_head, &nof_sections_allocated));
        if (nof_sections_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_port_single_priority_link_list_free(unit, list_type, priority, client_id));
        }

        /*
         * Allocate link list in DB.
         * Elements in alloc_info contains pairs of {free_list_index, size_to_alloc}.
         * Elements in sub_lists_info are four-tuples of {head, tail, size, sub_list_id}.
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_db_alloc
                        (unit, list_type, alloc_info[priority], sub_lists_info));

        /*
         * Setting new list in hw
         */
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_hw_set
                        (unit, list_type, sub_lists_info, nof_active_sub_lists, client_id, priority, new_list_size));
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_allocated_list_info_set
                        (unit, list_type, client_id, priority, sub_lists_info[0].m_first_section, new_list_size));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id)
{
    int list_head = 0, nof_sections_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_allocated_list_info_get
                    (unit, list_type, client_id, &list_head, &nof_sections_allocated));

    if (nof_sections_allocated > 0)
    {
        /*
         * Free link list in hw
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_hw_free(unit, list_type, client_id, list_head, nof_sections_allocated));

        /*
         * Free link list in DB
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_free(unit, list_type, list_head, nof_sections_allocated));

        SHR_IF_ERR_EXIT(dnx_port_link_list_db_allocated_list_info_set(unit, list_type, client_id, 0, 0));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "List doesn't exist for this client: %d ", client_id);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_double_priority_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id)
{
    int list_head, priority, nof_priorities, nof_allocated_sections;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Iterating per priority
     */
    nof_priorities = dnx_data_nif.eth.priority_groups_nof_get(unit);
    for (priority = 0; priority < nof_priorities; ++priority)
    {
        /*
         * If a list is already allocated for this client free it first before re-allocation
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_allocated_list_info_get
                        (unit, list_type, client_id, priority, &list_head, &nof_allocated_sections));
        if (nof_allocated_sections != 0)
        {
            SHR_IF_ERR_EXIT(dnx_port_single_priority_link_list_free(unit, list_type, priority, client_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_double_priority_link_list_used_sub_lists_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    dnx_port_nif_list_info_t * used_sub_lists_info)
{
    int sub_list_index, sub_list_size, sub_list_head, list_head, list_size;
    int nof_sections_per_group, nof_groups, section_index, current_section, prev_section;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_allocated_list_info_get
                    (unit, list_type, client_id, priority, &list_head, &list_size));

    if (list_size != 0)
    {
        /*
         * Get sections groups parameters
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                        (unit, list_type, &nof_sections_per_group, &nof_groups));

        /*
         * Initialize sub list parameters according to first section before iterating the list
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_new_sub_list_parameters_init
                        (unit, list_type, list_head, &sub_list_index, &sub_list_size, &sub_list_head));
        /*
         * Get next section of sub_list_head
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_get
                        (unit, list_type, sub_list_head, &current_section));
        prev_section = list_head;

        /*
         * Iterate through the list and for each sub-list gets {size, head, tail}
         */
        for (section_index = 1; section_index < list_size; ++section_index)
        {
            if (current_section / nof_sections_per_group != sub_list_index || current_section == sub_list_head)
            {
                /*
                 * Saves current sub list info and re-calculating new sub list local parameters
                 */
                used_sub_lists_info[sub_list_index].m_sub_list_id = sub_list_index;
                used_sub_lists_info[sub_list_index].m_size = sub_list_size;
                used_sub_lists_info[sub_list_index].m_first_section = sub_list_head;
                used_sub_lists_info[sub_list_index].m_last_section = prev_section;

                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_new_sub_list_parameters_init
                                (unit, list_type, current_section, &sub_list_index, &sub_list_size, &sub_list_head));
                prev_section = current_section;
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_get
                                (unit, list_type, sub_list_head, &current_section));
                continue;
            }
            prev_section = current_section;
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_get
                            (unit, list_type, prev_section, &current_section));
            ++sub_list_size;
        }

        /*
         * Saves last sub list info
         */
        used_sub_lists_info[sub_list_index].m_sub_list_id = sub_list_index;
        used_sub_lists_info[sub_list_index].m_size = sub_list_size;
        used_sub_lists_info[sub_list_index].m_first_section = sub_list_head;
        used_sub_lists_info[sub_list_index].m_last_section = prev_section;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * SW State static functions
 */

/* General */
static shr_error_e
dnx_port_single_priority_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int priority,
    int client_id)
{
    dnx_port_nif_list_info_t used_sub_lists_info[DNX_DATA_MAX_NIF_OFR_NOF_RX_MEMORY_GROUPS] = { {0} };
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Fill used_sub_lists_info with four-tuple: {sub list index, head, tail, size} per sub list.
     */
    SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_used_sub_lists_info_get
                    (unit, list_type, client_id, priority, used_sub_lists_info));

    /*
     * Free sub link list in hw
     */
    SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_hw_free(unit, list_type, used_sub_lists_info));

    /*
     * Free sub link list in DB
     */
    SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_db_free(unit, list_type, priority, used_sub_lists_info));

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_double_priority_allocated_list_info_set
                    (unit, list_type, client_id, priority, 0, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int *list_head)
{
    int first_section, next_section, last_section, free_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get free list data
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_get(unit, list_type, &first_section));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_get(unit, list_type, &free_size));

    if (alloc_size > 0 && free_size >= alloc_size)
    {
        /*
         * Return new list head
         */
        *list_head = first_section;

        /*
         * Get last section in the allocated list
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get
                        (unit, list_type, first_section, alloc_size - 1, &last_section));

        /*
         * Update free list in sw state
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_get(unit, list_type, last_section, &next_section));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_set(unit, list_type, next_section));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_set(unit, list_type, free_size - alloc_size));

        /*
         * close circular allocated list
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_set(unit, list_type, last_section, first_section));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Not enough free sections to allocate list. alloc_size: %d.", alloc_size);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_allocated_list_info_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int list_head,
    int nof_sections_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.allocated_list_info.head.set(unit, client_id, list_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.allocated_list_info.
                            size.set(unit, client_id, nof_sections_allocated));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.allocated_list_info.head.set(unit, client_id, list_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.allocated_list_info.
                            size.set(unit, client_id, nof_sections_allocated));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_allocated_list_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int *list_head,
    int *nof_sections_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.allocated_list_info.head.get(unit, client_id, list_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.allocated_list_info.
                            size.get(unit, client_id, nof_sections_allocated));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.allocated_list_info.head.get(unit, client_id, list_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.allocated_list_info.
                            size.get(unit, client_id, nof_sections_allocated));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_double_priority_allocated_list_info_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    int list_head,
    int nof_sections_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.allocated_list_info.
                            head.set(unit, priority, client_id, list_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.allocated_list_info.
                            size.set(unit, priority, client_id, nof_sections_allocated));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_link_list_db_double_priority_allocated_list_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    int *list_head,
    int *nof_sections_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.allocated_list_info.
                            head.get(unit, priority, client_id, list_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.allocated_list_info.
                            size.get(unit, priority, client_id, nof_sections_allocated));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Elements in sub_lists_data are alloc_size of given priority group (by index)
 * Elements in sub_lists_info are three-tuples  of {head, tail, size}
 */
static shr_error_e
dnx_port_double_priority_link_list_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    uint8 *sub_lists_data,
    dnx_port_nif_list_info_t * sub_lists_info)
{
    int sub_list_index, first_section, next_section, last_section, free_size;
    int nof_sections_per_group, nof_sub_lists, nof_active_sub_lists = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get sections groups parameters
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_sub_lists));

    for (sub_list_index = 0; sub_list_index < nof_sub_lists; ++sub_list_index)
    {
        if (sub_lists_data[sub_list_index] > 0)
        {
            /*
             * Get free list data
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_head_get
                            (unit, list_type, sub_list_index, &first_section));
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_get
                            (unit, list_type, sub_list_index, &free_size));

            /*
             * Get last section in the allocated list
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_nth_linking_info_get
                            (unit, list_type, first_section, sub_lists_data[sub_list_index] - 1, &last_section));

            /*
             * Save new list data
             */
            sub_lists_info[nof_active_sub_lists].m_sub_list_id = sub_list_index;
            sub_lists_info[nof_active_sub_lists].m_size = sub_lists_data[sub_list_index];
            sub_lists_info[nof_active_sub_lists].m_first_section = first_section;
            sub_lists_info[nof_active_sub_lists].m_last_section = last_section;
            ++nof_active_sub_lists;

            /*
             * Update free list in sw state
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_get
                            (unit, list_type, last_section, &next_section));
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_head_set
                            (unit, list_type, sub_list_index, next_section));
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_set
                            (unit, list_type, sub_list_index, free_size - sub_lists_data[sub_list_index]));
        }
    }

    /*
     * close circular allocated lists
     */
    for (sub_list_index = 0; sub_list_index < nof_active_sub_lists; ++sub_list_index)
    {
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_set
                        (unit, list_type, sub_lists_info[sub_list_index].m_last_section,
                         sub_lists_info[(sub_list_index + 1) % nof_active_sub_lists].m_first_section));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_new_sub_list_parameters_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int list_head,
    int *sub_list_index,
    int *sub_list_size,
    int *sub_list_head)
{
    int nof_sections_per_group, nof_groups;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get sections groups parameters
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_groups));

    *sub_list_index = list_head / nof_sections_per_group;
    *sub_list_size = 1;
    *sub_list_head = list_head;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int first_section,
    int alloc_size,
    int client_id)
{
    int second_section, last_section;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get second section in the allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_get(unit, list_type, first_section, &second_section));

    /*
     * Updating allocated list's head in hw
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_client_set
                    (unit, list_type, client_id, first_section, second_section, alloc_size));

    /*
     * Get last section in the allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get
                    (unit, list_type, first_section, alloc_size - 1, &last_section));

    /*
     * close circular allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_set(unit, list_type, last_section, first_section));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_switch_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int first_section,
    int alloc_size,
    int client_id)
{
    int second_section, last_section;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get second section in the allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_get(unit, list_type, first_section, &second_section));

    /*
     * Get last section in the allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get
                    (unit, list_type, first_section, alloc_size - 1, &last_section));

    /*
     * close circular allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_set(unit, list_type, last_section, first_section));

    /*
     * Switch to new linked list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_client_switch_set
                    (unit, list_type, client_id, first_section, second_section, alloc_size));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_double_priority_link_list_switch_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dnx_port_nif_list_info_t * sub_lists_info,
    int nof_active_sub_lists,
    int client_id,
    int priority)
{
    int sub_list_index, new_list_size = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (sub_list_index = 0; sub_list_index < nof_active_sub_lists; ++sub_list_index)
    {
        if (sub_lists_info[sub_list_index].m_size > 0)
        {
            /*
             * close circular allocated lists
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_set
                            (unit, list_type, sub_lists_info[sub_list_index].m_last_section,
                             sub_lists_info[(sub_list_index + 1) % nof_active_sub_lists].m_first_section));
            new_list_size += sub_lists_info[sub_list_index].m_size;
        }
    }

    /*
     * Switch to new linked list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_client_switch_set
                    (unit, list_type, client_id, priority, sub_lists_info[0].m_first_section, new_list_size));
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_double_priority_link_list_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dnx_port_nif_list_info_t * sub_lists_info,
    int nof_active_sub_lists,
    int client_id,
    int priority,
    int alloc_size)
{
    int sub_list_index, second_section;
    SHR_FUNC_INIT_VARS(unit);

    for (sub_list_index = 0; sub_list_index < nof_active_sub_lists; ++sub_list_index)
    {
        if (sub_lists_info[sub_list_index].m_size > 0)
        {
            /*
             * close circular allocated lists
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_set
                            (unit, list_type, sub_lists_info[sub_list_index].m_last_section,
                             sub_lists_info[(sub_list_index + 1) % nof_active_sub_lists].m_first_section));
        }
    }

    /*
     * Get second section in the allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_get
                    (unit, list_type, sub_lists_info[0].m_first_section, &second_section));

    /*
     * Updating allocated list's head in hw
     */
    SHR_IF_ERR_EXIT(dnx_port_double_priority_nif_link_list_client_set
                    (unit, list_type, client_id, priority, sub_lists_info[0].m_first_section, second_section,
                     alloc_size));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int list_head,
    int list_size)
{
    int free_head, free_tail, free_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get free list data
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_get(unit, list_type, &free_head));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_tail_get(unit, list_type, &free_tail));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_get(unit, list_type, &free_size));

    if (free_size > 0)
    {
        /*
         * Concatenate the two lists
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_set(unit, list_type, free_tail, list_head));

        /*
         * Update tail & size of free list
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get
                        (unit, list_type, list_head, list_size - 1, &free_tail));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_tail_set(unit, list_type, free_tail));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_set(unit, list_type, free_size + list_size));

        /*
         * close circular list
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_set(unit, list_type, free_tail, free_head));
    }
    else
    {
        /*
         * Update head, tail & size of free list
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_set(unit, list_type, list_head));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get
                        (unit, list_type, list_head, list_size - 1, &free_tail));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_tail_set(unit, list_type, free_tail));
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_set(unit, list_type, list_size));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_double_priority_link_list_db_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int priority,
    dnx_port_nif_list_info_t * used_sub_list_info)
{
    int free_head, free_tail, free_size;
    int sub_list_index, nof_sections_per_group, nof_sub_lists;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_sub_lists));

    /*
     * Iterate through sub lists
     */
    for (sub_list_index = 0; sub_list_index < nof_sub_lists; ++sub_list_index)
    {
        if (used_sub_list_info[sub_list_index].m_size)
        {
            /*
             * Get free list data
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_head_get
                            (unit, list_type, sub_list_index, &free_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_tail_get
                            (unit, list_type, sub_list_index, &free_tail));
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_get
                            (unit, list_type, sub_list_index, &free_size));
            if (free_size > 0)
            {
                /*
                 * Concatenate the two lists (circular way)
                 */
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_set
                                (unit, list_type, free_tail, used_sub_list_info[sub_list_index].m_first_section));
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_set
                                (unit, list_type, used_sub_list_info[sub_list_index].m_last_section, free_head));

                /*
                 * Update tail of free list
                 */
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_tail_set
                                (unit, list_type, sub_list_index, used_sub_list_info[sub_list_index].m_last_section));
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_set
                                (unit, list_type, sub_list_index,
                                 free_size + used_sub_list_info[sub_list_index].m_size));
            }
            else
            {
                /*
                 * Update tail of free list
                 */
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_head_set
                                (unit, list_type, sub_list_index, used_sub_list_info[sub_list_index].m_first_section));
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_tail_set
                                (unit, list_type, sub_list_index, used_sub_list_info[sub_list_index].m_last_section));
                SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_set
                                (unit, list_type, sub_list_index, used_sub_list_info[sub_list_index].m_size));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_hw_internal_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int list_head,
    int nof_sections)
{
    int head, tail;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get lists data
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_get(unit, list_type, &head));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_tail_get(unit, list_type, &tail));

    /*
     * Concatenate the two lists
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_set(unit, list_type, tail, list_head));

    /*
     * Update head of free list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get(unit, list_type, list_head, nof_sections - 1, &tail));

    /*
     * close circular list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_set(unit, list_type, tail, head));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_hw_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int list_head,
    int nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Free the list in hw
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_hw_internal_free(unit, list_type, list_head, nof_sections));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_double_priority_link_list_hw_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dnx_port_nif_list_info_t * used_sub_list_info)
{
    int sub_list_index, free_head, free_tail;
    int nof_sections_per_group, nof_sub_lists;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_sub_lists));
    /*
     * Iterate through sub lists info which has size (was part of the link list).
     */
    for (sub_list_index = 0; sub_list_index < nof_sub_lists; ++sub_list_index)
    {
        if (used_sub_list_info[sub_list_index].m_size)
        {
            /*
             * Get free list data
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_head_get
                            (unit, list_type, sub_list_index, &free_head));
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_tail_get
                            (unit, list_type, sub_list_index, &free_tail));

            /*
             * Concatenate the two lists (circular way)
             */
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_set
                            (unit, list_type, free_tail, used_sub_list_info[sub_list_index].m_first_section));

            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_set
                            (unit, list_type, used_sub_list_info[sub_list_index].m_last_section, free_head));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_nif_double_priority_link_list_check_if_size_available(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint8 **sub_lists_data)
{
    int sub_list_index, free_size, alloc_size;
    int priority, nof_sections_per_group, nof_sub_lists;
    dnx_port_nif_list_info_t **used_sub_lists_info = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_parameters_get
                    (unit, list_type, &nof_sections_per_group, &nof_sub_lists));

    /*
     * Allocating memory for used_sub_lists_info.
     */
    SHR_ALLOC_SET_ZERO_ERR_EXIT(used_sub_lists_info,
                                sizeof(dnx_port_nif_list_info_t *) * dnx_data_nif.eth.priority_groups_nof_get(unit),
                                "Alloc info per priority", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (priority = 0; priority < dnx_data_nif.eth.priority_groups_nof_get(unit); ++priority)
    {
        SHR_ALLOC_SET_ZERO_ERR_EXIT(used_sub_lists_info[priority], sizeof(dnx_port_nif_list_info_t) * nof_sub_lists,
                                    "array of used sub-lists info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_port_double_priority_link_list_used_sub_lists_info_get
                        (unit, list_type, client_id, priority, used_sub_lists_info[priority]));
    }

    for (sub_list_index = 0; sub_list_index < nof_sub_lists; ++sub_list_index)
    {
        /*
         * Summing nof_sections w/o nof_sections which is already allocated to this client for both priorities
         */
        alloc_size = 0;
        for (priority = 0; priority < dnx_data_nif.eth.priority_groups_nof_get(unit); ++priority)
        {
            alloc_size +=
                sub_lists_data[priority][sub_list_index] - used_sub_lists_info[priority][sub_list_index].m_size;
        }

        /*
         * Get free list data
         */
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_get
                        (unit, list_type, sub_list_index, &free_size));
        if (free_size < alloc_size)
        {
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Not enough free sections to allocate list. sub list id: %d, alloc_size: %d.",
                         sub_list_index, alloc_size);
        }
    }

exit:
    if (used_sub_lists_info != NULL)
    {
        for (priority = 0; priority < dnx_data_nif.eth.priority_groups_nof_get(unit); ++priority)
        {
            SHR_FREE(used_sub_lists_info[priority]);
        }
    }
    SHR_FREE(used_sub_lists_info);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_parameters_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *nof_sections_per_group,
    int *nof_groups)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            *nof_sections_per_group = dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            *nof_groups = dnx_data_nif.ofr.nof_rx_memory_groups_get(unit);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * SW State static functions
 */
static shr_error_e
dnx_port_nif_link_list_db_head_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int head)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.list_info.head.set(unit, head));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.list_info.head.set(unit, head));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_free_link_list_db_head_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int head)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            head %= dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.head.set(unit, sub_list_id, head));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_db_tail_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int tail)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.list_info.tail.set(unit, tail));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.list_info.tail.set(unit, tail));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_free_link_list_db_tail_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int tail)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            tail %= dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.tail.set(unit, sub_list_id, tail));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_db_size_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int size)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.list_info.size.set(unit, size));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.list_info.size.set(unit, size));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_free_link_list_db_size_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int size)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.set(unit, sub_list_id, size));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_db_head_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *head)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.list_info.head.get(unit, head));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.list_info.head.get(unit, head));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_free_link_list_db_head_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int *head)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.head.get(unit, sub_list_id, head));
            (*head) += sub_list_id * dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_db_tail_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *tail)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.list_info.tail.get(unit, tail));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.list_info.tail.get(unit, tail));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_free_link_list_db_tail_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int *tail)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.tail.get(unit, sub_list_id, tail));
            (*tail) += sub_list_id * dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_db_size_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.list_info.size.get(unit, size));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.list_info.size.get(unit, size));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_free_link_list_db_size_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int sub_list_id,
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.get(unit, sub_list_id, size));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_pointer_db_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.linking_info.
                            next_section.set(unit, current_section, next_section));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.linking_info.
                            next_section.set(unit, current_section, next_section));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_next_section_db_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section)
{
    int sub_list_id, nof_sections_per_group;
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            nof_sections_per_group = dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            sub_list_id = current_section / nof_sections_per_group;
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.
                            next_section.set(unit, sub_list_id, current_section % nof_sections_per_group,
                                             next_section));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections)
{
    int section_index, next_section;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_alloc(unit, list_type));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_set(unit, list_type, 0));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_tail_set(unit, list_type, nof_sections - 1));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_set(unit, list_type, nof_sections));

    for (section_index = 0; section_index < nof_sections; ++section_index)
    {
        next_section = (section_index + 1) % nof_sections;
        SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_set(unit, list_type, section_index, next_section));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_double_priority_link_list_db_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sub_lists,
    int nof_sections)
{
    int list_index, section_index, current_section, next_section;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_pointer_db_alloc(unit, list_type));
    for (list_index = 0; list_index < nof_sub_lists; ++list_index)
    {
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_head_set(unit, list_type, list_index, 0));
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_tail_set
                        (unit, list_type, list_index, nof_sections - 1));
        SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_free_link_list_db_size_set
                        (unit, list_type, list_index, nof_sections));

        for (section_index = 0; section_index < nof_sections - 1; ++section_index)
        {
            current_section = section_index + list_index * nof_sections;
            next_section = (current_section + 1) % nof_sections + list_index * nof_sections;
            SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_set
                            (unit, list_type, current_section, next_section));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_pointer_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.
                            linking_info.alloc(unit, dnx_data_nif.arb.tx_tmac_nof_sections_get(unit)));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.
                            allocated_list_info.alloc(unit, dnx_data_nif.arb.nof_contexts_get(unit)));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.
                            linking_info.alloc(unit, dnx_data_nif.oft.nof_sections_get(unit)));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.
                            allocated_list_info.alloc(unit, dnx_data_nif.oft.nof_contexts_get(unit)));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_pointer_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.
                            list_info.alloc(unit, dnx_data_nif.ofr.nof_rx_memory_groups_get(unit)));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.
                            linking_info.alloc(unit, dnx_data_nif.ofr.nof_rx_memory_groups_get(unit),
                                               dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit)));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.
                            allocated_list_info.alloc(unit, dnx_data_nif.eth.priority_groups_nof_get(unit),
                                                      dnx_data_nif.ofr.nof_contexts_get(unit)));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_pointer_db_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int *next_section)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.linking_info.
                            next_section.get(unit, current_section, next_section));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.linking_info.
                            next_section.get(unit, current_section, next_section));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_next_section_db_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int *next_section)
{
    int sub_list_id, nof_sections_per_group;
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            nof_sections_per_group = dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            sub_list_id = current_section / nof_sections_per_group;
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.
                            next_section.get(unit, sub_list_id, current_section % nof_sections_per_group,
                                             next_section));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_nth_linking_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int nof_steps,
    int *nth_section)
{
    int section_index, running_section = current_section;
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            for (section_index = 0; section_index < nof_steps; ++section_index)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.linking_info.
                                next_section.get(unit, running_section, &running_section));
            }
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            for (section_index = 0; section_index < nof_steps; ++section_index)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.linking_info.
                                next_section.get(unit, running_section, &running_section));
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

    *nth_section = running_section;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_nth_linking_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int nof_steps,
    int *nth_section)
{
    int sub_list_id, nof_sections_per_group;
    int section_index, running_section = current_section;
    SHR_FUNC_INIT_VARS(unit);

    *nth_section = -1;
    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            nof_sections_per_group = dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);
            sub_list_id = running_section / nof_sections_per_group;
            for (section_index = 0; section_index < nof_steps; ++section_index)
            {

                SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.
                                next_section.get(unit, sub_list_id, running_section % nof_sections_per_group,
                                                 &running_section));
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

    *nth_section = running_section;
exit:
    SHR_FUNC_EXIT;
}

/*
 * DBAL static functions
 */
static shr_error_e
dnx_port_nif_link_list_pointers_table_id_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    dbal_tables_e * table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            *table_id = DBAL_TABLE_NIF_ARB_TX_TMAC_LINK_LIST_POINTERS;
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            *table_id = DBAL_TABLE_NIF_OFT_TX_LINK_LIST_POINTERS;
            break;

        case DNX_PORT_NIF_OFR_LINK_LIST:
            *table_id = DBAL_TABLE_NIF_OFR_RX_MEM_LINK_LIST_POINTERS;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_arb_link_list_client_set(
    int unit,
    int client,
    int head,
    int next,
    int nof_sections)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TX_TMAC_LINK_LIST, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, client);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION, INST_SINGLE, head);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION_RD, INST_SINGLE, head);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, next);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, INST_SINGLE, nof_sections);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_ofr_link_list_client_set(
    int unit,
    int client,
    int priority,
    int head,
    int next,
    int nof_sections)
{
    uint32 entry_handle_id, rmc_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    rmc_id = client + priority * dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_RX_MEM_LINK_LIST, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, rmc_id);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION, INST_SINGLE, head);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, next);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE_WR, INST_SINGLE, nof_sections);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE_RD, INST_SINGLE, nof_sections);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_oft_link_list_client_set(
    int unit,
    int client,
    int head,
    int nof_sections)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFT_TX_LINK_LIST, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFT_CONTEXT, client);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION, INST_SINGLE, head);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, INST_SINGLE, nof_sections);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_client_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client,
    int head,
    int next,
    int nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_link_list_client_set(unit, client, head, next, nof_sections));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_oft_link_list_client_set(unit, client, head, nof_sections));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_arb_link_list_client_switch_set(
    int unit,
    int client,
    int head,
    int next,
    int nof_sections)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set new linked list to be switched to */
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_ARB_TX_TMAC_LINK_LIST_SWITCH, &entry_handle_id));

        /** set value fields*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, client);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION, INST_SINGLE, head);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, next);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, INST_SINGLE, nof_sections);

        /** commit the value */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Start switch */
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ARB_TX_TMAC_LINK_LIST_SWITCH, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Poll for switch to be over */
    {
        /** It's enough to read only READ status, as it indicates whether the whole switch process is done */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ARB_TX_TMAC_LINK_LIST_SWITCH_STATUS, entry_handle_id));
        SHR_IF_ERR_EXIT(dnxcmn_polling(unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id,
                                       DBAL_FIELD_READ_SWITCHED, 1));
    }

    /** Stop switch */
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_ARB_TX_TMAC_LINK_LIST_SWITCH, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_oft_link_list_client_switch_set(
    int unit,
    int client,
    int head,
    int nof_sections)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** set new linked list to be switched to */
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFT_FIFO_LL_SWITCH_CONFIG, &entry_handle_id));

        /** set value fields*/
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT, INST_SINGLE, client);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION, INST_SINGLE, head);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, INST_SINGLE, nof_sections);

        /** commit the value */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Start switch */
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OFT_FIFO_LL_SWITCH_CONFIG, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Poll for switch to be over */
    {
        /** It's enough to read only READ status, as it indicates whether the whole switch process is done */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OFT_FIFO_LL_SWITCH_STATUS, entry_handle_id));
        SHR_IF_ERR_EXIT(dnxcmn_polling(unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id,
                                       DBAL_FIELD_READ_PTR_UPDATED, 1));
    }

    /** Stop switch */
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OFT_FIFO_LL_SWITCH_CONFIG, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_client_switch_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client,
    int head,
    int next,
    int nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_arb_link_list_client_switch_set(unit, client, head, next, nof_sections));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_oft_link_list_client_switch_set(unit, client, head, nof_sections));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_ofr_link_list_client_switch_set(
    int unit,
    int client_id,
    int rmc_machine_id,
    int first_section,
    int nof_sections)
{
    uint32 *threshold = NULL;
    int second_section, rmc_fifo_size, rmc_id, priority;
    uint32 entry_handle_id, thr_after_ovf, is_hard_stage_enable;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get relevant data
     */
    rmc_fifo_size = nof_sections * dnx_data_nif.ofr.nof_bytes_per_memory_section_get(unit);
    rmc_id = client_id + rmc_machine_id * dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit);
    SHR_IF_ERR_EXIT(dnx_port_nif_double_priority_link_list_next_section_db_get
                    (unit, DNX_PORT_NIF_OFR_LINK_LIST, first_section, &second_section));
    SHR_IF_ERR_EXIT(dnx_algo_port_imb_threshold_after_ovf_get(unit, rmc_fifo_size, &thr_after_ovf));
    SHR_IF_ERR_EXIT(dnx_ofr_prd_hard_stage_enable_hw_get(unit, rmc_id, &is_hard_stage_enable));

    SHR_ALLOC_SET_ZERO_ERR_EXIT(threshold, sizeof(uint32) * dnx_data_nif.prd.nof_priorities_get(unit),
                                "threshold", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    for (priority = 0; priority < dnx_data_nif.prd.nof_priorities_get(unit); ++priority)
    {
        SHR_IF_ERR_EXIT(dnx_ofr_prd_threshold_hw_get(unit, rmc_id, priority, &(threshold[priority])));
    }

    /*
     * Set PRD priorities
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_OFR_LINK_LIST_SWITCH_THRESHOLDS, &entry_handle_id));
    for (priority = 0; priority < dnx_data_nif.prd.nof_priorities_get(unit); ++priority)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold[priority]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Set new linked list to be switched to
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OFR_RX_MEM_LINK_LIST_SWITCH_CONFIG, entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC_ID, INST_SINGLE, rmc_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIRST_SECTION, INST_SINGLE, first_section);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, second_section);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, INST_SINGLE, nof_sections);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_EN, INST_SINGLE, is_hard_stage_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_AFTER_OVF, INST_SINGLE, thr_after_ovf);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_ALL_DATA, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INIT_PTR, INST_SINGLE, 0);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Start switch
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_SWITCH, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Poll for switch to be over
     */
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OFR_RX_MEM_LINK_LIST_SWITCH_STATUS, entry_handle_id));
        SHR_IF_ERR_EXIT(dnxcmn_polling(unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id,
                                       DBAL_FIELD_IS_SWITCH_DONE, 1));
    }

    /*
     * Setting all post switch data
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_ofr_link_list_client_post_switch(unit, rmc_id, nof_sections, threshold,
                                                                  thr_after_ovf, is_hard_stage_enable));

    /*
     * Stop switch
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_OFR_RX_MEM_LINK_LIST_SWITCH_CONFIG, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_SWITCH, INST_SINGLE, FALSE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(threshold);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_client_switch_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    int head,
    int nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_ofr_link_list_client_switch_set
                            (unit, client_id, priority, head, nof_sections));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_double_priority_nif_link_list_client_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client,
    int priority,
    int head,
    int next,
    int nof_sections)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_ofr_link_list_client_set(unit, client, priority, head, next, nof_sections));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_link_list_pointer_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointers_table_id_get(unit, list_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_SECTION, current_section);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, next_section);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_nif_double_priority_link_list_next_section_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointers_table_id_get(unit, list_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_SECTION, current_section);

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, next_section);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*static*/ shr_error_e
dnx_port_nif_link_list_next_section_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int *next_section)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id, dbal_next_section;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointers_table_id_get(unit, list_type, &table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURRENT_SECTION, current_section);

    /*
     * set value fields
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_NEXT_SECTION, INST_SINGLE, &dbal_next_section);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *next_section = dbal_next_section;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
