/** \file dnx/swstate/auto_generated/types/dnx_rx_types.h
 *
 * sw state types (structs/enums/typedefs)
 *
 * DO NOT EDIT THIS FILE!
 * This file is auto-generated.
 * Edits to this file will be lost when it is regenerated.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_RX_TYPES_H__
#define __DNX_RX_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
/*
 * STRUCTs
 */

/**
 * rx parser configration information
 */
typedef struct {
    /**
     * Size of FTMH LB-Key extension.
     */
    uint32 ftmh_lb_key_size;
    /**
     * Size of FTMH stacking extension.
     */
    uint32 ftmh_stacking_ext_size;
    /**
     * User Defined Header(UDH) enable.
     */
    uint8 user_header_enable;
    /**
     * Size of PPH Base Header.
     */
    uint32 pph_base_size;
    /**
     * Size of 1XOutLif type of extension.
     */
    uint32 out_lif_x_1_ext_size;
    /**
     * Size of 2XOutLif type of extension.
     */
    uint32 out_lif_x_2_ext_size;
    /**
     * Size of 3XOutLif type of extension.
     */
    uint32 out_lif_x_3_ext_size;
    /**
     * Size of Lif extension for InLIF + InLIF-Profile
     */
    uint32 in_lif_ext_size;
    /**
     * System port of OAMP on core 0
     */
    uint32 oamp_system_port_0;
    /**
     * System port of OAMP on core 1
     */
    uint32 oamp_system_port_1;
    /**
     * DMA channel to CPU ports
     */
    int* dma_channel_to_cpu_port;
    /**
     * CPU port to DMA channel
     */
    int* cpu_port_to_dma_channel;
    /**
     * FTMH.PP_DSP to CPU port
     */
    int* ftmh_pp_dsp_to_cpu_port;
} rx_paser_info_t;


#endif /* __DNX_RX_TYPES_H__ */
