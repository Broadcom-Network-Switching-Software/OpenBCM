
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_PREFIX_LAYOUT_H__
#define __DNX_FIELD_TCAM_PREFIX_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_field_tcam_prefix_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FIELD_TCAM_PREFIX_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX,
    DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP,
    DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW__BANKS_PREFIX__CORE_PREFIX_MAP__PREFIX_HANDLER_MAP,
    
    DNX_SW_STATE_DNX_FIELD_TCAM_PREFIX_SW_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_field_tcam_prefix_node_id_e;

#endif 
