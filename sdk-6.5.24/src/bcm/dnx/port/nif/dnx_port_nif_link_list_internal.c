/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#include "dnx_port_nif_link_list_internal.h"
#include "dnx_port_nif_oft_internal.h"
#include <bcm/types.h>
#include <shared/pbmp.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_nif_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_nif_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/* Static functions */
/* General */
static shr_error_e dnx_port_link_list_db_client_allocated_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint8 is_client_allocated);
static shr_error_e dnx_port_link_list_db_client_allocated_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint8 *is_client_allocated);
static shr_error_e dnx_port_link_list_hw_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int first_section,
    int alloc_size,
    int client_id);
static shr_error_e dnx_port_link_list_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int *list_head);
static shr_error_e dnx_port_link_list_db_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type);

/* Sw state */
static shr_error_e dnx_port_link_list_db_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections,
    int nof_clients);
static shr_error_e dnx_port_nif_link_list_pointer_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections,
    int nof_clients);
static shr_error_e dnx_port_nif_link_list_db_head_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int head);
static shr_error_e dnx_port_nif_link_list_db_tail_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int tail);
static shr_error_e dnx_port_nif_link_list_db_size_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int size);
static shr_error_e dnx_port_nif_link_list_db_head_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *head);
static shr_error_e dnx_port_nif_link_list_db_tail_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *tail);
static shr_error_e dnx_port_nif_link_list_db_size_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int *size);
static shr_error_e dnx_port_nif_link_list_pointer_db_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int current_section,
    int next_section);
static shr_error_e dnx_port_nif_link_list_pointer_db_get(
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

/* dbal */
shr_error_e
dnx_port_link_list_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections,
    int nof_clients)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_init(unit, list_type, nof_sections, nof_clients));

exit:
    DBAL_FUNC_FREE_VARS;
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
    uint8 is_client_allocated;
    int list_head = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_client_allocated_get(unit, list_type, client_id, &is_client_allocated));

    /*
     * If a list is already allocated for this client free it first before re-allocation
     */
    if (is_client_allocated)
    {
        SHR_IF_ERR_EXIT(dnx_port_link_list_free(unit, list_type, client_id));
    }

    /*
     * Allocate link list in DB
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_db_alloc(unit, list_type, alloc_size, &list_head));

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_client_allocated_set(unit, list_type, client_id, TRUE));

    /*
     * Setting new list in hw
     */
    SHR_IF_ERR_EXIT(dnx_port_link_list_hw_set(unit, list_type, alloc_size, list_head, client_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_port_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id)
{
    uint8 is_client_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_link_list_db_client_allocated_get(unit, list_type, client_id, &is_client_allocated));

    if (is_client_allocated)
    {
        /*
         * Free link list in DB
         */
        SHR_IF_ERR_EXIT(dnx_port_link_list_db_free(unit, list_type));

        SHR_IF_ERR_EXIT(dnx_port_link_list_db_client_allocated_set(unit, list_type, client_id, FALSE));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "List for this client dosn't exist.");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * SW State static functions
 */

/* General */
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
dnx_port_link_list_db_client_allocated_set(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint8 is_client_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.
                            is_client_allocated.set(unit, client_id, is_client_allocated));
            break;
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.
                            is_client_allocated.set(unit, client_id, is_client_allocated));
            break;
        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.
                            is_client_allocated.set(unit, client_id, is_client_allocated));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_client_allocated_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint8 *is_client_allocated)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.
                            is_client_allocated.get(unit, client_id, is_client_allocated));
            break;
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.
                            is_client_allocated.get(unit, client_id, is_client_allocated));
            break;
        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.
                            is_client_allocated.get(unit, client_id, is_client_allocated));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "List type %u is not supported.", list_type);
            break;
    }

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
     * Get last section in the allocated list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_nth_linking_info_get
                    (unit, list_type, first_section, alloc_size - 1, &last_section));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_port_link_list_db_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type)
{
    int head, tail, size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get free list data
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_head_get(unit, list_type, &head));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_tail_get(unit, list_type, &tail));
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_db_size_get(unit, list_type, &size));

    /*
     * close circular list
     */
    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_set(unit, list_type, tail, head));

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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.head.set(unit, head));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.tail.set(unit, tail));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.set(unit, size));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.head.get(unit, head));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.tail.get(unit, tail));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.list_info.size.get(unit, size));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.
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
dnx_port_link_list_db_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections,
    int nof_clients)
{
    int section_index, next_section;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_port_nif_link_list_pointer_db_alloc(unit, list_type, nof_sections, nof_clients));
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
dnx_port_nif_link_list_pointer_db_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections,
    int nof_clients)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (list_type)
    {
        case DNX_PORT_NIF_ARB_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.linking_info.alloc(unit, nof_sections));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.arb_link_list.is_client_allocated.alloc(unit, nof_clients));
            break;

        case DNX_PORT_NIF_OFT_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.linking_info.alloc(unit, nof_sections));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.oft_link_list.is_client_allocated.alloc(unit, nof_clients));
            break;
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.alloc(unit, nof_sections));
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.is_client_allocated.alloc(unit, nof_clients));
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.
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
        case DNX_PORT_NIF_OFR_LINK_LIST:
            for (section_index = 0; section_index < nof_steps; ++section_index)
            {
                SHR_IF_ERR_EXIT(dnx_port_nif_db.ofr_link_list.linking_info.
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
