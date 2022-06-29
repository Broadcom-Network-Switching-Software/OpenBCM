
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_EGR_DB_LAYOUT_H__
#define __DNX_EGR_DB_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_egr_db_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_EGR_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_EGR_DB_MODULE_ID, 0),
    DNX_SW_STATE_DNX_EGR_DB__FIRST,
    DNX_SW_STATE_DNX_EGR_DB__FIRST_SUB = DNX_SW_STATE_DNX_EGR_DB__FIRST - 1,
    DNX_SW_STATE_DNX_EGR_DB__PS,
    DNX_SW_STATE_DNX_EGR_DB__INTERFACE_OCC,
    DNX_SW_STATE_DNX_EGR_DB__TOTAL_EGR_IF_CREDITS,
    DNX_SW_STATE_DNX_EGR_DB__EGR_IF_CREDITS_CONFIGURED,
    DNX_SW_STATE_DNX_EGR_DB__TOTAL_ESB_QUEUES_ALLOCATED,
    DNX_SW_STATE_DNX_EGR_DB__NOF_NEGATIVE_COMPENSATION_PORTS,
    DNX_SW_STATE_DNX_EGR_DB__LAST,
    DNX_SW_STATE_DNX_EGR_DB__LAST_SUB = DNX_SW_STATE_DNX_EGR_DB__LAST - 1,
    DNX_SW_STATE_DNX_EGR_DB__PS__FIRST,
    DNX_SW_STATE_DNX_EGR_DB__PS__FIRST_SUB = DNX_SW_STATE_DNX_EGR_DB__PS__FIRST - 1,
    DNX_SW_STATE_DNX_EGR_DB__PS__ALLOCATION_BMAP,
    DNX_SW_STATE_DNX_EGR_DB__PS__PRIO_MODE,
    DNX_SW_STATE_DNX_EGR_DB__PS__IF_IDX,
    DNX_SW_STATE_DNX_EGR_DB__PS__LAST,
    DNX_SW_STATE_DNX_EGR_DB__PS__LAST_SUB = DNX_SW_STATE_DNX_EGR_DB__PS__LAST - 1,
    
    DNX_SW_STATE_DNX_EGR_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_egr_db_node_id_e;

#endif 
