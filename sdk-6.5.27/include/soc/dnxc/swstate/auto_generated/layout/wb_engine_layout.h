
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __WB_ENGINE_LAYOUT_H__
#define __WB_ENGINE_LAYOUT_H__

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int wb_engine_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_SW_STATE_WB_ENGINE = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(WB_ENGINE_MODULE_ID, 0),
    DNX_SW_STATE_SW_STATE_WB_ENGINE__FIRST,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__FIRST_SUB = DNX_SW_STATE_SW_STATE_WB_ENGINE__FIRST - 1,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__LAST,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__LAST_SUB = DNX_SW_STATE_SW_STATE_WB_ENGINE__LAST - 1,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__FIRST,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__FIRST_SUB = DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__FIRST - 1,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__INSTANCE,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__LAST,
    DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__LAST_SUB = DNX_SW_STATE_SW_STATE_WB_ENGINE__BUFFER__LAST - 1,
    
    DNX_SW_STATE_SW_STATE_WB_ENGINE_NOF_PARAMS,
} dnxc_sw_state_layout_wb_engine_node_id_e;
#endif  

#endif 
