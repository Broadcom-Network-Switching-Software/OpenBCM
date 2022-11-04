
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __SRV6_LAYOUT_H__
#define __SRV6_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int srv6_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_SRV6_MODES = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(SRV6_MODULE_ID, 0),
    DNX_SW_STATE_SRV6_MODES__FIRST,
    DNX_SW_STATE_SRV6_MODES__FIRST_SUB = DNX_SW_STATE_SRV6_MODES__FIRST - 1,
    DNX_SW_STATE_SRV6_MODES__EGRESS_IS_PSP,
    DNX_SW_STATE_SRV6_MODES__ENCAP_IS_REDUCED,
    DNX_SW_STATE_SRV6_MODES__GSID_PREFIX_IS_64B,
    DNX_SW_STATE_SRV6_MODES__USID_PREFIX_IS_48B,
    DNX_SW_STATE_SRV6_MODES__LPM_DESTINATION_FOR_USP_TRAP,
    DNX_SW_STATE_SRV6_MODES__LAST,
    DNX_SW_STATE_SRV6_MODES__LAST_SUB = DNX_SW_STATE_SRV6_MODES__LAST - 1,
    
    DNX_SW_STATE_SRV6_MODES_NOF_PARAMS,
} dnxc_sw_state_layout_srv6_node_id_e;

#endif 
