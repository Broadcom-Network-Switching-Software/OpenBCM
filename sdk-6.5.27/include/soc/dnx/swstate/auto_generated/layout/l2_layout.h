
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __L2_LAYOUT_H__
#define __L2_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int l2_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_L2_DB_CONTEXT = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(L2_MODULE_ID, 0),
    DNX_SW_STATE_L2_DB_CONTEXT__FIRST,
    DNX_SW_STATE_L2_DB_CONTEXT__FIRST_SUB = DNX_SW_STATE_L2_DB_CONTEXT__FIRST - 1,
    DNX_SW_STATE_L2_DB_CONTEXT__FID_PROFILES_CHANGED,
    DNX_SW_STATE_L2_DB_CONTEXT__ROUTING_LEARNING,
    DNX_SW_STATE_L2_DB_CONTEXT__LAST,
    DNX_SW_STATE_L2_DB_CONTEXT__LAST_SUB = DNX_SW_STATE_L2_DB_CONTEXT__LAST - 1,
    
    DNX_SW_STATE_L2_DB_CONTEXT_NOF_PARAMS,
} dnxc_sw_state_layout_l2_node_id_e;

#endif 
