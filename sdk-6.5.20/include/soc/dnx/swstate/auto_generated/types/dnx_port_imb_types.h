
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_PORT_IMB_TYPES_H__
#define __DNX_PORT_IMB_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>



typedef struct {
    
    uint32 imbs_in_use;
    
    imb_create_info_t* imb;
    
    imb_dispatch_type_t imb_type[SOC_MAX_NUM_PORTS];
    
    uint32 pmd_lock_counter[SOC_MAX_NUM_PORTS];
    
    sw_state_mutex_t credit_tx_reset_mutex;
} dnx_imbm_info_t;


#endif 
