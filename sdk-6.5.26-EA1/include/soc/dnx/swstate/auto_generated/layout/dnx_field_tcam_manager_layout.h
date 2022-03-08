
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_MANAGER_LAYOUT_H__
#define __DNX_FIELD_TCAM_MANAGER_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_field_tcam_manager_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FIELD_TCAM_MANAGER_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS,
    DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS__MODE,
    DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW__TCAM_HANDLERS__STATE,
    
    DNX_SW_STATE_DNX_FIELD_TCAM_MANAGER_SW_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_field_tcam_manager_node_id_e;

#endif 
