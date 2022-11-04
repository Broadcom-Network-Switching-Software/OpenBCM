
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __EDK_LAYOUT_H__
#define __EDK_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int edk_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_EDK_STATE = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(EDK_MODULE_ID, 0),
    DNX_SW_STATE_EDK_STATE__FIRST,
    DNX_SW_STATE_EDK_STATE__FIRST_SUB = DNX_SW_STATE_EDK_STATE__FIRST - 1,
    DNX_SW_STATE_EDK_STATE__EDK_SEQ,
    DNX_SW_STATE_EDK_STATE__LAST,
    DNX_SW_STATE_EDK_STATE__LAST_SUB = DNX_SW_STATE_EDK_STATE__LAST - 1,
    
    DNX_SW_STATE_EDK_STATE_NOF_PARAMS,
} dnxc_sw_state_layout_edk_node_id_e;

#endif 
