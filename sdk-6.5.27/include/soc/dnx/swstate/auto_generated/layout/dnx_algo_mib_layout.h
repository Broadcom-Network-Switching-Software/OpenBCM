
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_ALGO_MIB_LAYOUT_H__
#define __DNX_ALGO_MIB_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_algo_mib_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_ALGO_MIB_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_ALGO_MIB_MODULE_ID, 0),
    DNX_SW_STATE_DNX_ALGO_MIB_DB__FIRST,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__FIRST_SUB = DNX_SW_STATE_DNX_ALGO_MIB_DB__FIRST - 1,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__LAST,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__LAST_SUB = DNX_SW_STATE_DNX_ALGO_MIB_DB__LAST - 1,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__FIRST,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__FIRST_SUB = DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__FIRST - 1,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__COUNTER_PBMP,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__INTERVAL,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__LAST,
    DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__LAST_SUB = DNX_SW_STATE_DNX_ALGO_MIB_DB__MIB__LAST - 1,
    
    DNX_SW_STATE_DNX_ALGO_MIB_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_algo_mib_node_id_e;

#endif 
