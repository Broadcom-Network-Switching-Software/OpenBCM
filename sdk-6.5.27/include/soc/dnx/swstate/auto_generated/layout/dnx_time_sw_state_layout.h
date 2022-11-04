
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_TIME_SW_STATE_LAYOUT_H__
#define __DNX_TIME_SW_STATE_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_time_sw_state_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_TIME_SW_STATE_MODULE_ID, 0),
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__FIRST,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__FIRST_SUB = DNX_SW_STATE_DNX_TIME_INTERFACE_DB__FIRST - 1,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__FLAGS,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__ID,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__HEARTBEAT_HZ,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__CLK_RESOLUTION,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__BITCLOCK_HZ,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__STATUS,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__LAST,
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB__LAST_SUB = DNX_SW_STATE_DNX_TIME_INTERFACE_DB__LAST - 1,
    
    DNX_SW_STATE_DNX_TIME_INTERFACE_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_time_sw_state_node_id_e;

#endif 
