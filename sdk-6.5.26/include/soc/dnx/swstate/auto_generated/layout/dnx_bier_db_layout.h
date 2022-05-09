
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_BIER_DB_LAYOUT_H__
#define __DNX_BIER_DB_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_bier_db_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_BIER_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_BIER_DB_MODULE_ID, 0),
    DNX_SW_STATE_DNX_BIER_DB__FIRST,
    DNX_SW_STATE_DNX_BIER_DB__FIRST_SUB = DNX_SW_STATE_DNX_BIER_DB__FIRST - 1,
    DNX_SW_STATE_DNX_BIER_DB__SET_SIZE,
    DNX_SW_STATE_DNX_BIER_DB__INGRESS,
    DNX_SW_STATE_DNX_BIER_DB__EGRESS,
    DNX_SW_STATE_DNX_BIER_DB__LAST,
    DNX_SW_STATE_DNX_BIER_DB__LAST_SUB = DNX_SW_STATE_DNX_BIER_DB__LAST - 1,
    DNX_SW_STATE_DNX_BIER_DB__INGRESS__FIRST,
    DNX_SW_STATE_DNX_BIER_DB__INGRESS__FIRST_SUB = DNX_SW_STATE_DNX_BIER_DB__INGRESS__FIRST - 1,
    DNX_SW_STATE_DNX_BIER_DB__INGRESS__ACTIVE,
    DNX_SW_STATE_DNX_BIER_DB__INGRESS__LAST,
    DNX_SW_STATE_DNX_BIER_DB__INGRESS__LAST_SUB = DNX_SW_STATE_DNX_BIER_DB__INGRESS__LAST - 1,
    DNX_SW_STATE_DNX_BIER_DB__EGRESS__FIRST,
    DNX_SW_STATE_DNX_BIER_DB__EGRESS__FIRST_SUB = DNX_SW_STATE_DNX_BIER_DB__EGRESS__FIRST - 1,
    DNX_SW_STATE_DNX_BIER_DB__EGRESS__ACTIVE,
    DNX_SW_STATE_DNX_BIER_DB__EGRESS__LAST,
    DNX_SW_STATE_DNX_BIER_DB__EGRESS__LAST_SUB = DNX_SW_STATE_DNX_BIER_DB__EGRESS__LAST - 1,
    
    DNX_SW_STATE_DNX_BIER_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_bier_db_node_id_e;

#endif 
