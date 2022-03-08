
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_TCAM_LOCATION_LAYOUT_H__
#define __DNX_FIELD_TCAM_LOCATION_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_field_tcam_location_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FIELD_TCAM_LOCATION_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__PRIORITIES_RANGE,
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO,
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__LOCATION_PRIORITY_ARR,
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_BANKS_OCCUPATION_BITMAP,
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__ENTRIES_OCCUPATION_BITMAP,
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__PRIO_LIST_FIRST_ITER,
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW__TCAM_HANDLERS_INFO__PRIO_LIST_LAST_ITER,
    
    DNX_SW_STATE_DNX_FIELD_TCAM_LOCATION_SW_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_field_tcam_location_node_id_e;

#endif 
