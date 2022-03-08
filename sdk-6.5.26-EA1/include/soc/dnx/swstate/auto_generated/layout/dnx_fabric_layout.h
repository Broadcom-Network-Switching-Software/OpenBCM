
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FABRIC_LAYOUT_H__
#define __DNX_FABRIC_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_fabric_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FABRIC_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FABRIC_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP,
    DNX_SW_STATE_DNX_FABRIC_DB__LINKS,
    DNX_SW_STATE_DNX_FABRIC_DB__LOAD_BALANCING_SHR_THREAD_MANAGER_HANDLER,
    DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE,
    DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP,
    DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY,
    DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__NOF_MODIDS,
    DNX_SW_STATE_DNX_FABRIC_DB__MODIDS_TO_GROUP_MAP__MODIDS,
    DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ALLOWED,
    DNX_SW_STATE_DNX_FABRIC_DB__LINKS__IS_LINK_ISOLATED,
    DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__STATE_0,
    DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__STATE_1,
    DNX_SW_STATE_DNX_FABRIC_DB__SYSTEM_UPGRADE__STATE_2,
    DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_ENABLE,
    DNX_SW_STATE_DNX_FABRIC_DB__CABLE_SWAP__IS_MASTER,
    DNX_SW_STATE_DNX_FABRIC_DB__MESH_TOPOLOGY__LINKS,
    
    DNX_SW_STATE_DNX_FABRIC_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_fabric_node_id_e;

#endif 
