
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_SCH_CONFIG_LAYOUT_H__
#define __DNX_SCH_CONFIG_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_sch_config_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_COSQ_CONFIG = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_SCH_CONFIG_MODULE_ID, 0),
    DNX_SW_STATE_COSQ_CONFIG__HR_GROUP_BW,
    DNX_SW_STATE_COSQ_CONFIG__GROUPS_BW,
    DNX_SW_STATE_COSQ_CONFIG__IPF_CONFIG_MODE,
    DNX_SW_STATE_COSQ_CONFIG__FLOW,
    DNX_SW_STATE_COSQ_CONFIG__CONNECTOR,
    DNX_SW_STATE_COSQ_CONFIG__SE,
    DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC,
    DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__WEIGHT,
    DNX_SW_STATE_COSQ_CONFIG__FLOW__CREDIT_SRC__MODE,
    DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__NUM_COS,
    DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__CONNECTION_VALID,
    DNX_SW_STATE_COSQ_CONFIG__CONNECTOR__SRC_MODID,
    DNX_SW_STATE_COSQ_CONFIG__SE__CHILD_COUNT,
    
    DNX_SW_STATE_COSQ_CONFIG_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_sch_config_node_id_e;

#endif 
