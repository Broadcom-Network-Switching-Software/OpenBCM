

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_HBMC_HBMCCB_H_INCLUDED

#define _BCMINT_DNX_DRAM_HBMC_HBMCCB_H_INCLUDED

#include <soc/hbmc_shmoo.h>


shr_error_e dnx_hbmc_modify_mrs_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int mr_index,
    uint32 data,
    uint32 mask);


shr_error_e dnx_hbmc_pll_set_cb(
    int unit,
    int hbm_ndx,
    const hbmc_shmoo_pll_t * pll_info);


shr_error_e dnx_hbmc_phy_reg_read_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 *data);


shr_error_e dnx_hbmc_phy_reg_write_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data);


shr_error_e dnx_hbmc_phy_reg_modify_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data,
    uint32 mask);



shr_error_e dnx_hbmc_enable_wr_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_rd_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_addr_parity_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_wr_dbi_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
shr_error_e dnx_hbmc_enable_rd_dbi_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);


shr_error_e dnx_hbmc_soft_reset_controller_without_dram_cb(
    int unit,
    int hbm_ndx,
    int channel);




shr_error_e dnx_hbmc_bist_conf_set_cb(
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    const hbmc_shmoo_bist_info_t * info);


shr_error_e dnx_hbmc_bist_err_cnt_cb(
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters);


shr_error_e dnx_hbmc_enable_refresh_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int enable);


shr_error_e dnx_hbmc_dram_init_cb(
    int unit,
    int hbm_ndx,
    int phase);



shr_error_e dnx_hbmc_dram_info_access_cb(
    int unit,
    hbmc_shmoo_dram_info_t * shmoo_info);


shr_error_e dnx_hbmc_model_part_get_cb(
    int unit,
    int hbm_index,
    hbmc_shmoo_hbm_model_part_t * model_part);


shr_error_e dnx_hbmc_bist_status_get_cb(
    int unit,
    int hbm_ndx,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);


shr_error_e dnx_hbmc_phy_channel_dwords_alignment_check_cb(
    int unit,
    int hbm_ndx,
    int channel,
    int *is_aligned);

#endif 
