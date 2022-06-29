
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IRE_PACKET_GENERATOR_LAYOUT_H__
#define __DNX_IRE_PACKET_GENERATOR_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_ire_packet_generator_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_IRE_PACKET_GENERATOR_MODULE_ID, 0),
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__FIRST,
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__FIRST_SUB = DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__FIRST - 1,
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__IRE_DATAPATH_ENABLE_STATE,
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__LAST,
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__LAST_SUB = DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO__LAST - 1,
    
    DNX_SW_STATE_IRE_PACKET_GENERATOR_INFO_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_ire_packet_generator_node_id_e;

#endif 
