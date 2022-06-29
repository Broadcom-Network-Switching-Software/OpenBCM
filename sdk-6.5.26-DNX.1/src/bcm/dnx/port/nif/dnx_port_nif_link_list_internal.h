/** \file dnx_port_nif_link_list_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_LINK_LIST_INTERNAL_H_
#define _PORT_NIF_LINK_LIST_INTERNAL_H_

#include <soc/sand/shrextend/shrextend_error.h>
#include <bcm/types.h>

typedef enum
{
    DNX_PORT_NIF_LINK_LIST_FIRST = 0,

    DNX_PORT_NIF_ARB_LINK_LIST = DNX_PORT_NIF_LINK_LIST_FIRST,

    DNX_PORT_NIF_OFT_LINK_LIST = 1,

    DNX_PORT_NIF_OFR_LINK_LIST = 2,

    DNX_PORT_NIF_LINK_LIST_NOF
} dnx_port_nif_link_list_type_e;

typedef struct
{
    int m_first_section;
    int m_last_section;
    int m_size;
    int m_sub_list_id;
} dnx_port_nif_list_info_t;

shr_error_e dnx_port_link_list_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections);

shr_error_e dnx_port_double_priority_link_list_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sub_lists,
    int nof_sections);

shr_error_e dnx_port_link_list_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int client_id);

/**
 * \brief - Allocate and switch to new linked list and free old list
 */
shr_error_e dnx_port_link_list_switch(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int client_id);

shr_error_e dnx_port_double_priority_link_list_switch(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    uint8 **alloc_info,
    int client_id);

shr_error_e dnx_port_double_priority_link_list_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    uint8 **alloc_info,
    int client_id);

shr_error_e dnx_port_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id);

shr_error_e dnx_port_link_list_db_double_priority_nof_sections_allocated_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    uint32 *nof_sections_allocated);

shr_error_e dnx_port_double_priority_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id);

shr_error_e dnx_port_double_priority_link_list_used_sub_lists_info_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    int priority,
    dnx_port_nif_list_info_t * used_sub_lists_info);

shr_error_e dnx_port_nif_double_priority_link_list_check_if_size_available(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint8 **sub_lists_data);

#endif /* _PORT_NIF_LINK_LIST_INTERNAL_H_ */
