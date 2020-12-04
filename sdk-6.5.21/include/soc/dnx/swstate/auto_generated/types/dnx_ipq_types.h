
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IPQ_TYPES_H__
#define __DNX_IPQ_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>



typedef struct {
    
    uint8 num_cos;
    
    int sys_port_ref_counter;
} dnx_ipq_base_queue_t;


typedef struct {
    dnx_ipq_base_queue_t** base_queues;
    
    SHR_BITDCL* base_queue_is_asymm;
} dnx_ipq_db_t;


#endif 
