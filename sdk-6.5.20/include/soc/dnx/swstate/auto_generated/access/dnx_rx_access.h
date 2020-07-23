
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_RX_ACCESS_H__
#define __DNX_RX_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_rx_types.h>



typedef int (*dnx_rx_parser_info_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_rx_parser_info_init_cb)(
    int unit);


typedef int (*dnx_rx_parser_info_ftmh_lb_key_size_set_cb)(
    int unit, uint32 ftmh_lb_key_size);


typedef int (*dnx_rx_parser_info_ftmh_lb_key_size_get_cb)(
    int unit, uint32 *ftmh_lb_key_size);


typedef int (*dnx_rx_parser_info_ftmh_stacking_ext_size_set_cb)(
    int unit, uint32 ftmh_stacking_ext_size);


typedef int (*dnx_rx_parser_info_ftmh_stacking_ext_size_get_cb)(
    int unit, uint32 *ftmh_stacking_ext_size);


typedef int (*dnx_rx_parser_info_user_header_enable_set_cb)(
    int unit, uint8 user_header_enable);


typedef int (*dnx_rx_parser_info_user_header_enable_get_cb)(
    int unit, uint8 *user_header_enable);


typedef int (*dnx_rx_parser_info_pph_base_size_set_cb)(
    int unit, uint32 pph_base_size);


typedef int (*dnx_rx_parser_info_pph_base_size_get_cb)(
    int unit, uint32 *pph_base_size);


typedef int (*dnx_rx_parser_info_out_lif_x_1_ext_size_set_cb)(
    int unit, uint32 out_lif_x_1_ext_size);


typedef int (*dnx_rx_parser_info_out_lif_x_1_ext_size_get_cb)(
    int unit, uint32 *out_lif_x_1_ext_size);


typedef int (*dnx_rx_parser_info_out_lif_x_2_ext_size_set_cb)(
    int unit, uint32 out_lif_x_2_ext_size);


typedef int (*dnx_rx_parser_info_out_lif_x_2_ext_size_get_cb)(
    int unit, uint32 *out_lif_x_2_ext_size);


typedef int (*dnx_rx_parser_info_out_lif_x_3_ext_size_set_cb)(
    int unit, uint32 out_lif_x_3_ext_size);


typedef int (*dnx_rx_parser_info_out_lif_x_3_ext_size_get_cb)(
    int unit, uint32 *out_lif_x_3_ext_size);


typedef int (*dnx_rx_parser_info_in_lif_ext_size_set_cb)(
    int unit, uint32 in_lif_ext_size);


typedef int (*dnx_rx_parser_info_in_lif_ext_size_get_cb)(
    int unit, uint32 *in_lif_ext_size);


typedef int (*dnx_rx_parser_info_oamp_system_port_0_set_cb)(
    int unit, uint32 oamp_system_port_0);


typedef int (*dnx_rx_parser_info_oamp_system_port_0_get_cb)(
    int unit, uint32 *oamp_system_port_0);


typedef int (*dnx_rx_parser_info_oamp_system_port_1_set_cb)(
    int unit, uint32 oamp_system_port_1);


typedef int (*dnx_rx_parser_info_oamp_system_port_1_get_cb)(
    int unit, uint32 *oamp_system_port_1);


typedef int (*dnx_rx_parser_info_dma_channel_to_cpu_port_set_cb)(
    int unit, uint32 dma_channel_to_cpu_port_idx_0, int dma_channel_to_cpu_port);


typedef int (*dnx_rx_parser_info_dma_channel_to_cpu_port_get_cb)(
    int unit, uint32 dma_channel_to_cpu_port_idx_0, int *dma_channel_to_cpu_port);


typedef int (*dnx_rx_parser_info_dma_channel_to_cpu_port_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);


typedef int (*dnx_rx_parser_info_cpu_port_to_dma_channel_set_cb)(
    int unit, uint32 cpu_port_to_dma_channel_idx_0, int cpu_port_to_dma_channel);


typedef int (*dnx_rx_parser_info_cpu_port_to_dma_channel_get_cb)(
    int unit, uint32 cpu_port_to_dma_channel_idx_0, int *cpu_port_to_dma_channel);


typedef int (*dnx_rx_parser_info_cpu_port_to_dma_channel_alloc_cb)(
    int unit, uint32 nof_instances_to_alloc_0);




typedef struct {
    dnx_rx_parser_info_ftmh_lb_key_size_set_cb set;
    dnx_rx_parser_info_ftmh_lb_key_size_get_cb get;
} dnx_rx_parser_info_ftmh_lb_key_size_cbs;


typedef struct {
    dnx_rx_parser_info_ftmh_stacking_ext_size_set_cb set;
    dnx_rx_parser_info_ftmh_stacking_ext_size_get_cb get;
} dnx_rx_parser_info_ftmh_stacking_ext_size_cbs;


typedef struct {
    dnx_rx_parser_info_user_header_enable_set_cb set;
    dnx_rx_parser_info_user_header_enable_get_cb get;
} dnx_rx_parser_info_user_header_enable_cbs;


typedef struct {
    dnx_rx_parser_info_pph_base_size_set_cb set;
    dnx_rx_parser_info_pph_base_size_get_cb get;
} dnx_rx_parser_info_pph_base_size_cbs;


typedef struct {
    dnx_rx_parser_info_out_lif_x_1_ext_size_set_cb set;
    dnx_rx_parser_info_out_lif_x_1_ext_size_get_cb get;
} dnx_rx_parser_info_out_lif_x_1_ext_size_cbs;


typedef struct {
    dnx_rx_parser_info_out_lif_x_2_ext_size_set_cb set;
    dnx_rx_parser_info_out_lif_x_2_ext_size_get_cb get;
} dnx_rx_parser_info_out_lif_x_2_ext_size_cbs;


typedef struct {
    dnx_rx_parser_info_out_lif_x_3_ext_size_set_cb set;
    dnx_rx_parser_info_out_lif_x_3_ext_size_get_cb get;
} dnx_rx_parser_info_out_lif_x_3_ext_size_cbs;


typedef struct {
    dnx_rx_parser_info_in_lif_ext_size_set_cb set;
    dnx_rx_parser_info_in_lif_ext_size_get_cb get;
} dnx_rx_parser_info_in_lif_ext_size_cbs;


typedef struct {
    dnx_rx_parser_info_oamp_system_port_0_set_cb set;
    dnx_rx_parser_info_oamp_system_port_0_get_cb get;
} dnx_rx_parser_info_oamp_system_port_0_cbs;


typedef struct {
    dnx_rx_parser_info_oamp_system_port_1_set_cb set;
    dnx_rx_parser_info_oamp_system_port_1_get_cb get;
} dnx_rx_parser_info_oamp_system_port_1_cbs;


typedef struct {
    dnx_rx_parser_info_dma_channel_to_cpu_port_set_cb set;
    dnx_rx_parser_info_dma_channel_to_cpu_port_get_cb get;
    dnx_rx_parser_info_dma_channel_to_cpu_port_alloc_cb alloc;
} dnx_rx_parser_info_dma_channel_to_cpu_port_cbs;


typedef struct {
    dnx_rx_parser_info_cpu_port_to_dma_channel_set_cb set;
    dnx_rx_parser_info_cpu_port_to_dma_channel_get_cb get;
    dnx_rx_parser_info_cpu_port_to_dma_channel_alloc_cb alloc;
} dnx_rx_parser_info_cpu_port_to_dma_channel_cbs;


typedef struct {
    dnx_rx_parser_info_is_init_cb is_init;
    dnx_rx_parser_info_init_cb init;
    
    dnx_rx_parser_info_ftmh_lb_key_size_cbs ftmh_lb_key_size;
    
    dnx_rx_parser_info_ftmh_stacking_ext_size_cbs ftmh_stacking_ext_size;
    
    dnx_rx_parser_info_user_header_enable_cbs user_header_enable;
    
    dnx_rx_parser_info_pph_base_size_cbs pph_base_size;
    
    dnx_rx_parser_info_out_lif_x_1_ext_size_cbs out_lif_x_1_ext_size;
    
    dnx_rx_parser_info_out_lif_x_2_ext_size_cbs out_lif_x_2_ext_size;
    
    dnx_rx_parser_info_out_lif_x_3_ext_size_cbs out_lif_x_3_ext_size;
    
    dnx_rx_parser_info_in_lif_ext_size_cbs in_lif_ext_size;
    
    dnx_rx_parser_info_oamp_system_port_0_cbs oamp_system_port_0;
    
    dnx_rx_parser_info_oamp_system_port_1_cbs oamp_system_port_1;
    
    dnx_rx_parser_info_dma_channel_to_cpu_port_cbs dma_channel_to_cpu_port;
    
    dnx_rx_parser_info_cpu_port_to_dma_channel_cbs cpu_port_to_dma_channel;
} dnx_rx_parser_info_cbs;





extern dnx_rx_parser_info_cbs dnx_rx_parser_info;

#endif 
