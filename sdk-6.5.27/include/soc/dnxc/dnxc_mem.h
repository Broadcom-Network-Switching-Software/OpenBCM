/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_DNXC_MEM_H
#define _SOC_DNXC_MEM_H

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX and DNXF family only!"
#endif

#include <soc/mcm/allenum.h>
#include <soc/register.h>

#define SOC_DNXC_SET_DYN_MEM_ACCESS(unit, block_name, mem_name, inst, value)                                                                     \
        soc_reg_above_64_field32_modify(unit, block_name ## _ENABLE_DYNAMIC_MEMORY_ACCESSr, inst, 0, ENABLE_DYNAMIC_MEMORY_ACCESS_ ## mem_name ## f, value)

int dnxc_fill_table_with_index_val(
    int unit,
    soc_mem_t mem,
    soc_field_t field,
    int is_seq_array);

#ifdef BCM_SBUSDMA_SUPPORT
#define SOC_DNXC_MAX_SBUSDMA_CHANNELS    3
#define SOC_DNXC_TDMA_CHANNEL            0
#define SOC_DNXC_TSLAM_CHANNEL           1
#define SOC_DNXC_DESC_CHANNEL            2
#define SOC_DNXC_MEM_CLEAR_CHUNK_SIZE    4
#endif

#ifdef BCM_DNX_SUPPORT
int dnx_mem_read_eps_protect_start(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *eps_protect_flag,
    soc_reg_above_64_val_t * eps_shaper_set);

int dnx_mem_read_eps_protect_end(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *eps_protect_flag,
    soc_reg_above_64_val_t * eps_shaper_set);

int dnx_egr_esb_nif_calendar_mux_switch_start(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 *switched);

int dnx_egr_esb_nif_calendar_mux_switch_end(
    int unit,
    soc_mem_t mem,
    int blk,
    uint32 switched);

#endif

#endif
