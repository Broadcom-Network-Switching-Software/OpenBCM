
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __TDM_LAYOUT_H__
#define __TDM_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int tdm_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_TDM_FTMH_INFO = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(TDM_MODULE_ID, 0),
    DNX_SW_STATE_TDM_FTMH_INFO__FIRST,
    DNX_SW_STATE_TDM_FTMH_INFO__FIRST_SUB = DNX_SW_STATE_TDM_FTMH_INFO__FIRST - 1,
    DNX_SW_STATE_TDM_FTMH_INFO__TDM_HEADER_IS_FTMH,
    DNX_SW_STATE_TDM_FTMH_INFO__USE_OPTIMIZED_FTMH,
    DNX_SW_STATE_TDM_FTMH_INFO__LAST,
    DNX_SW_STATE_TDM_FTMH_INFO__LAST_SUB = DNX_SW_STATE_TDM_FTMH_INFO__LAST - 1,
    
    DNX_SW_STATE_TDM_FTMH_INFO_NOF_PARAMS,
} dnxc_sw_state_layout_tdm_node_id_e;

#endif 
