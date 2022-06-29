
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __KBP_FWD_TCAM_ACCESS_MAPPER_LAYOUT_H__
#define __KBP_FWD_TCAM_ACCESS_MAPPER_LAYOUT_H__

#if defined(INCLUDE_KBP)
#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int kbp_fwd_tcam_access_mapper_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(KBP_FWD_TCAM_ACCESS_MAPPER_MODULE_ID, 0),
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__FIRST,
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__FIRST_SUB = DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__FIRST - 1,
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__KEY_2_ENTRY_ID_HASH,
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__LAST,
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__LAST_SUB = DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER__LAST - 1,
    
    DNX_SW_STATE_KBP_FWD_TCAM_ACCESS_MAPPER_NOF_PARAMS,
} dnxc_sw_state_layout_kbp_fwd_tcam_access_mapper_node_id_e;
#endif  

#endif 
