
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_RX_TYPES_H__
#define __DNX_RX_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>



typedef struct {
    
    uint32 ftmh_lb_key_size;
    
    uint32 ftmh_stacking_ext_size;
    
    uint8 user_header_enable;
    
    uint32 pph_base_size;
    
    uint32 out_lif_x_1_ext_size;
    
    uint32 out_lif_x_2_ext_size;
    
    uint32 out_lif_x_3_ext_size;
    
    uint32 in_lif_ext_size;
    
    uint32 oamp_system_port_0;
    
    uint32 oamp_system_port_1;
    
    int* dma_channel_to_cpu_port;
    
    int* cpu_port_to_dma_channel;
} rx_paser_info_t;


#endif 
