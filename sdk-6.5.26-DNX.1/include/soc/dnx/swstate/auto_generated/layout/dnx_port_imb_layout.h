
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_IMB_LAYOUT_H__
#define __DNX_PORT_IMB_LAYOUT_H__

#include <include/soc/dnxc/swstate/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_port_imb_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_IMBM = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_PORT_IMB_MODULE_ID, 0),
    DNX_SW_STATE_IMBM__FIRST,
    DNX_SW_STATE_IMBM__FIRST_SUB = DNX_SW_STATE_IMBM__FIRST - 1,
    DNX_SW_STATE_IMBM__IMBS_IN_USE,
    DNX_SW_STATE_IMBM__IMB,
    DNX_SW_STATE_IMBM__IMB_TYPE,
    DNX_SW_STATE_IMBM__PMD_LOCK_COUNTER,
    DNX_SW_STATE_IMBM__CREDIT_TX_RESET_MUTEX,
    DNX_SW_STATE_IMBM__LAST,
    DNX_SW_STATE_IMBM__LAST_SUB = DNX_SW_STATE_IMBM__LAST - 1,
    
    DNX_SW_STATE_IMBM_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_port_imb_node_id_e;

#endif 
