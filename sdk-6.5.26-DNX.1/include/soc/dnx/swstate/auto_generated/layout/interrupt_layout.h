
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __INTERRUPT_LAYOUT_H__
#define __INTERRUPT_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int interrupt_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_INTR_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(INTERRUPT_MODULE_ID, 0),
    DNX_SW_STATE_INTR_DB__FIRST,
    DNX_SW_STATE_INTR_DB__FIRST_SUB = DNX_SW_STATE_INTR_DB__FIRST - 1,
    DNX_SW_STATE_INTR_DB__FLAGS,
    DNX_SW_STATE_INTR_DB__STORM_TIMED_COUNT,
    DNX_SW_STATE_INTR_DB__STORM_TIMED_PERIOD,
    DNX_SW_STATE_INTR_DB__STORM_NOMINAL,
    DNX_SW_STATE_INTR_DB__LAST,
    DNX_SW_STATE_INTR_DB__LAST_SUB = DNX_SW_STATE_INTR_DB__LAST - 1,
    
    DNX_SW_STATE_INTR_DB_NOF_PARAMS,
} dnxc_sw_state_layout_interrupt_node_id_e;

#endif 
