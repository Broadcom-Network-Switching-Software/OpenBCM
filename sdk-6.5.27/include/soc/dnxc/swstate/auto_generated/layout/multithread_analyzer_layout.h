
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __MULTITHREAD_ANALYZER_LAYOUT_H__
#define __MULTITHREAD_ANALYZER_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int multithread_analyzer_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_MULTITHREAD_ANALYZER = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(MULTITHREAD_ANALYZER_MODULE_ID, 0),
    DNX_SW_STATE_MULTITHREAD_ANALYZER__FIRST,
    DNX_SW_STATE_MULTITHREAD_ANALYZER__FIRST_SUB = DNX_SW_STATE_MULTITHREAD_ANALYZER__FIRST - 1,
    DNX_SW_STATE_MULTITHREAD_ANALYZER__HTB,
    DNX_SW_STATE_MULTITHREAD_ANALYZER__LAST,
    DNX_SW_STATE_MULTITHREAD_ANALYZER__LAST_SUB = DNX_SW_STATE_MULTITHREAD_ANALYZER__LAST - 1,
    
    DNX_SW_STATE_MULTITHREAD_ANALYZER_NOF_PARAMS,
} dnxc_sw_state_layout_multithread_analyzer_node_id_e;

#endif 
