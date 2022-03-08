
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_CACHE_LAYOUT_H__
#define __DNX_FIELD_TCAM_CACHE_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_field_tcam_cache_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FIELD_TCAM_CACHE_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK,
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE,
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__ENTRY_KEY,
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__TCAM_BANK__ENTRY_KEY__KEY_DATA,
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__ENTRY_PAYLOAD,
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW__PAYLOAD_TABLE__ENTRY_PAYLOAD__PAYLOAD_DATA,
    
    DNX_SW_STATE_DNX_FIELD_TCAM_CACHE_SHADOW_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_field_tcam_cache_node_id_e;

#endif 
