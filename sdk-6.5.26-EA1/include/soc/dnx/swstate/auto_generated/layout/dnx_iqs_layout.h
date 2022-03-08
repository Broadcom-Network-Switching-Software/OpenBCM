
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IQS_LAYOUT_H__
#define __DNX_IQS_LAYOUT_H__

#include <include/soc/dnxc/swstate/types/dnxc_sw_state_layout.h>
#include <include/soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>


int dnx_iqs_init_layout_structure(int unit);

typedef enum
{
    DNX_SW_STATE_DNX_IQS_DB = DNXC_SW_STATE_LAYOUT_NODE_ID_GET(DNX_IQS_MODULE_ID, 0),
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_REQUEST_PROFILE,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG,
    DNX_SW_STATE_DNX_IQS_DB__DELETE_CREDIT_REQUEST_PROFILE,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_REQUEST_PROFILE__VALID,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_REQUEST_PROFILE__THRESHOLDS,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__QUEUE_MIN,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__QUEUE_MAX,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__ENABLE,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__RETRANSMIT_TH,
    DNX_SW_STATE_DNX_IQS_DB__CREDIT_WATCHDOG__DELETE_TH,
    DNX_SW_STATE_DNX_IQS_DB__DELETE_CREDIT_REQUEST_PROFILE__VALID,
    DNX_SW_STATE_DNX_IQS_DB__DELETE_CREDIT_REQUEST_PROFILE__ID,
    
    DNX_SW_STATE_DNX_IQS_DB_NOF_PARAMS,
} dnxc_sw_state_layout_dnx_iqs_node_id_e;

#endif 
