
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SWITCH_LAYOUT_H__
#define __SWITCH_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int switch_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_SWITCH_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SWITCH_MODULE_ID, 0),
    DNX_SW_STATE_SWITCH_DB__MODULE_VERIFICATION,
    DNX_SW_STATE_SWITCH_DB__MODULE_ERROR_RECOVERY,
    DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV4_FWD_TYPE,
    DNX_SW_STATE_SWITCH_DB__L3MCASTL2_IPV6_FWD_TYPE,
    DNX_SW_STATE_SWITCH_DB__HEADER_ENABLERS_HASHING,
    DNX_SW_STATE_SWITCH_DB__TUNNEL_ROUTE_DISABLED,
    DNX_SW_STATE_SWITCH_DB__PER_PORT_SPECIAL_LABEL_TERMINATION_DISABLED,
    
    DNX_SW_STATE_SWITCH_DB_NOF_PARAMS,
} dnxc_sw_state_layout_switch_node_id_e;

#endif 
