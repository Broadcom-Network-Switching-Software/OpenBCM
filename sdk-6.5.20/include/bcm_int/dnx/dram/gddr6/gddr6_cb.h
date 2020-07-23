

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_GDDR6_GDDR6_CB_H_INCLUDED

#define _BCMINT_DNX_DRAM_GDDR6_GDDR6_CB_H_INCLUDED

#include <soc/shmoo_g6phy16.h>

shr_error_e dnx_gddr6_modify_mrs_cb(
    int unit,
    int dram_index,
    int channel_index,
    uint32 mr_index,
    uint32 data,
    uint32 mask);

shr_error_e dnx_gddr6_pll_set_cb(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);


shr_error_e dnx_gddr6_phy_reg_read_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 *data);

shr_error_e dnx_gddr6_phy_reg_write_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 data);

shr_error_e dnx_gddr6_phy_reg_modify_cb(
    int unit,
    int dram_index,
    uint32 addr,
    uint32 data,
    uint32 mask);

shr_error_e dnx_gddr6_enable_wr_crc_cb(
    int unit,
    int dram_index,
    int enable);

shr_error_e dnx_gddr6_enable_rd_crc_cb(
    int unit,
    int dram_index,
    int enable);

shr_error_e dnx_gddr6_enable_wr_dbi_cb(
    int unit,
    int dram_index,
    int enable);

shr_error_e dnx_gddr6_enable_rd_dbi_cb(
    int unit,
    int dram_index,
    int enable);

shr_error_e dnx_gddr6_soft_reset_controller_without_dram_cb(
    int unit,
    int dram_index);

shr_error_e dnx_gddr6_bist_conf_set_cb(
    int unit,
    int dram_index,
    const g6phy16_bist_info_t * info);

shr_error_e dnx_gddr6_bist_run_cb(
    int unit,
    int dram_index,
    const g6phy16_bist_info_t * info);

shr_error_e dnx_gddr6_bist_err_cnt_cb(
    int unit,
    int dram_index,
    g6phy16_bist_err_cnt_t * info);


shr_error_e dnx_gddr6_dram_init_cb(
    int unit,
    int dram_index,
    gddr6_dram_init_phase_t phase);


shr_error_e dnx_gddr6_dram_info_access_cb(
    int unit,
    g6phy16_shmoo_dram_info_t * shmoo_info);


shr_error_e dnx_gddr6_enable_wck2ck_training_cb(
    int unit,
    int dram_index,
    int enable);

shr_error_e dnx_gddr6_enable_write_leveling_cb(
    int unit,
    int dram_index,
    uint32 command_parity_lattency,
    int use_continious_dqs,
    int enable);

shr_error_e dnx_gddr6_mpr_en_cb(
    int unit,
    int dram_index,
    int enable);

shr_error_e dnx_gddr6_mpr_load_cb(
    int unit,
    int dram_index,
    uint8 *mpr_pattern);


shr_error_e dnx_gddr6_vendor_info_get_cb(
    int unit,
    int dram_index,
    g6phy16_vendor_info_t * vendor_info);


shr_error_e dnx_gddr6_dqs_pulse_gen_cb(
    int unit,
    int dram_index,
    int use_continious_dqs);


shr_error_e dnx_gddr6_training_bist_conf_set_cb(
    int unit,
    int dram_index,
    g6phy16_training_bist_info_t * info);


shr_error_e dnx_gddr6_training_bist_err_cnt_cb(
    int unit,
    int dram_index,
    g6phy16_training_bist_err_cnt_t * info);


shr_error_e dnx_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get_cb(
    int unit,
    int dram_index,
    int *pairs_were_swapped);


shr_error_e dnx_gddr6_force_dqs_cb(
    int unit,
    int dram_index,
    uint32 force_dqs);


shr_error_e dnx_gddr6_enable_refresh_cb(
    int unit,
    int dram_index,
    int enable,
    uint32 new_trefi,
    uint32 new_trefi_ab,
    uint32 *curr_refi,
    uint32 *curr_refi_ab);

#endif
