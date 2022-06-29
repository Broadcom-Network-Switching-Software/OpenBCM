
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SAT_LAYOUT_H__
#define __SAT_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int sat_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_SAT_PKT_HEADER_INFO = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SAT_MODULE_ID, 0),
    DNX_SW_STATE_SAT_PKT_HEADER_INFO__FIRST,
    DNX_SW_STATE_SAT_PKT_HEADER_INFO__FIRST_SUB = DNX_SW_STATE_SAT_PKT_HEADER_INFO__FIRST - 1,
    DNX_SW_STATE_SAT_PKT_HEADER_INFO__PKT_HEADER_BASE,
    DNX_SW_STATE_SAT_PKT_HEADER_INFO__LAST,
    DNX_SW_STATE_SAT_PKT_HEADER_INFO__LAST_SUB = DNX_SW_STATE_SAT_PKT_HEADER_INFO__LAST - 1,
    
    DNX_SW_STATE_SAT_PKT_HEADER_INFO_NOF_PARAMS,
} dnxc_sw_state_layout_sat_node_id_e;

#endif 
