
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_IRE_PACKET_GENERATOR_ACCESS_H__
#define __DNX_IRE_PACKET_GENERATOR_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ire_packet_generator_types.h>



typedef int (*ire_packet_generator_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*ire_packet_generator_info_init_cb)(
    int unit);


typedef int (*ire_packet_generator_info_ire_datapath_enable_state_set_cb)(
    int unit, uint32 ire_datapath_enable_state);


typedef int (*ire_packet_generator_info_ire_datapath_enable_state_get_cb)(
    int unit, uint32 *ire_datapath_enable_state);




typedef struct {
    ire_packet_generator_info_ire_datapath_enable_state_set_cb set;
    ire_packet_generator_info_ire_datapath_enable_state_get_cb get;
} ire_packet_generator_info_ire_datapath_enable_state_cbs;


typedef struct {
    ire_packet_generator_info_is_init_cb is_init;
    ire_packet_generator_info_init_cb init;
    
    ire_packet_generator_info_ire_datapath_enable_state_cbs ire_datapath_enable_state;
} ire_packet_generator_info_cbs;





extern ire_packet_generator_info_cbs ire_packet_generator_info;

#endif 
