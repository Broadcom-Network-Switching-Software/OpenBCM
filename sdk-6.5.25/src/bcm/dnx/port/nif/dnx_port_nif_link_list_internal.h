/** \file dnx_port_nif_link_list_internal.h
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _PORT_NIF_LINK_LIST_INTERNAL_H_
#define _PORT_NIF_LINK_LIST_INTERNAL_H_

#include <shared/shrextend/shrextend_error.h>
#include <bcm/types.h>

typedef enum
{
    DNX_PORT_NIF_LINK_LIST_FIRST = 0,
    DNX_PORT_NIF_ARB_LINK_LIST = DNX_PORT_NIF_LINK_LIST_FIRST,
    DNX_PORT_NIF_OFT_LINK_LIST = 1,
    DNX_PORT_NIF_OFR_LINK_LIST = 2,

    DNX_PORT_NIF_LINK_LIST_NOF
} dnx_port_nif_link_list_type_e;

shr_error_e dnx_port_link_list_init(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int nof_sections,
    int nof_clients);
shr_error_e dnx_port_link_list_alloc(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int alloc_size,
    int client_id);
shr_error_e dnx_port_link_list_free(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id);
shr_error_e dnx_port_link_list_db_nof_sections_allocated_get(
    int unit,
    dnx_port_nif_link_list_type_e list_type,
    int client_id,
    uint32 *nof_sections_allocated);

#endif /* _PORT_NIF_LINK_LIST_INTERNAL_H_ */
