
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_FIELD_QUALIFIER_LAYOUT_H__
#define __DNX_FIELD_QUALIFIER_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_field_qualifier_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_FIELD_QUALIFIER_MODULE_ID, 0),
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__VALID,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FIELD_ID,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__SIZE,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__BCM_ID,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__REF_COUNT,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__INFO__FLAGS,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__PREDEFINED__FIELD_ID,
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB__LAYER_RECORD__FIELD_ID,
    
    DNX_SW_STATE_DNX_FIELD_QUAL_SW_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_field_qualifier_node_id_e;

#endif 
