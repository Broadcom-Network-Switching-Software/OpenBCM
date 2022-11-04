
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SW_STATE_EXTERNAL_LAYOUT_H__
#define __SW_STATE_EXTERNAL_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int sw_state_external_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_SW_STATE_EXTERNAL = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SW_STATE_EXTERNAL_MODULE_ID, 0),
    DNX_SW_STATE_SW_STATE_EXTERNAL__FIRST,
    DNX_SW_STATE_SW_STATE_EXTERNAL__FIRST_SUB = DNX_SW_STATE_SW_STATE_EXTERNAL__FIRST - 1,
    DNX_SW_STATE_SW_STATE_EXTERNAL__OFFSETS,
    DNX_SW_STATE_SW_STATE_EXTERNAL__LAST,
    DNX_SW_STATE_SW_STATE_EXTERNAL__LAST_SUB = DNX_SW_STATE_SW_STATE_EXTERNAL__LAST - 1,
    
    DNX_SW_STATE_SW_STATE_EXTERNAL_NOF_PARAMS,
} dnxc_sw_state_layout_sw_state_external_node_id_e;

#endif 
