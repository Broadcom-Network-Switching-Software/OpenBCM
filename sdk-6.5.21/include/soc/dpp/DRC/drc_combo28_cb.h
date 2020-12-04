/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains DPP DRC CallBack structure and routine declarations for the Dram operation using PHY Combo28.
 */
#ifndef _SOC_DPP_DRC_COMBO28_CB_H
#define _SOC_DPP_DRC_COMBO28_CB_H

 
#include <soc/dpp/DRC/drc_combo28.h>


int soc_dpp_drc_combo28_shmoo_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data);
int soc_dpp_drc_combo28_shmoo_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data);
int soc_dpp_drc_combo28_shmoo_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask);
int soc_dpp_drc_combo16_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo16_bist_info_t info);
int soc_dpp_drc_combo28_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo28_bist_info_t info);
int soc_dpp_drc_combo16_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo16_bist_err_cnt_t *info);
int soc_dpp_drc_combo28_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo28_bist_err_cnt_t *info);
int soc_dpp_drc_combo28_shmoo_dram_init(int unit, int drc_ndx, int phase);
int soc_dpp_drc_combo16_shmoo_drc_pll_set(int unit, int drc_ndx, CONST combo16_drc_pll_t *pll_info);
int soc_dpp_drc_combo28_shmoo_drc_pll_set(int unit, int drc_ndx, CONST combo28_drc_pll_t *pll_info);
int soc_dpp_drc_combo28_shmoo_modify_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 data, uint32 mask);
int soc_dpp_drc_combo28_shmoo_drc_enable_adt(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_drc_enable_wck2ck_training(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_drc_enable_write_leveling(int unit, int drc_ndx, uint32 command_parity_lattency, int use_continious_gddr5_dqs, int enable);
int soc_dpp_drc_combo28_shmoo_drc_mpr_en(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_mpr_load(int unit, int drc_ndx, uint8 *mpr_pattern);
int soc_dpp_drc_combo28_shmoo_drc_enable_gddr5_training_protocol(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo16_shmoo_vendor_info_get(int unit, int drc_ndx, combo16_vendor_info_t *info);
int soc_dpp_drc_combo28_shmoo_vendor_info_get(int unit, int drc_ndx, combo28_vendor_info_t *info);
int soc_dpp_drc_combo28_shmoo_drc_dqs_pulse_gen(int unit, int drc_ndx, int use_continious_gddr5_dqs);
int soc_dpp_drc_combo16_gddr5_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo16_gddr5_bist_info_t info);
int soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_conf_set(int unit, int drc_ndx, combo28_gddr5_bist_info_t info);
int soc_dpp_drc_combo16_gddr5_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo16_gddr5_bist_err_cnt_t *info);
int soc_dpp_drc_combo28_gddr5_shmoo_drc_bist_err_cnt(int unit, int drc_ndx, combo28_gddr5_bist_err_cnt_t *info);
int soc_dpp_drc_combo28_gddr5_shmoo_drc_dq_byte_pairs_swap_info_get(int unit, int drc_ndx, int* pairs_were_swapped);
int soc_dpp_drc_combo28_shmoo_enable_wr_crc(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_rd_crc(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_wr_dbi(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_rd_dbi(int unit, int drc_ndx, int enable);
int soc_dpp_drc_combo28_shmoo_enable_refresh(int unit, int drc_ndx , int enable, uint32 new_trefi, uint32 * curr_refi);
int soc_dpp_drc_combo28_shmoo_force_dqs(int unit, int drc_ndx , uint32 force_dqs_val, uint32 force_dqs_oeb);
int soc_dpp_drc_combo28_soft_reset_drc_without_dram(int unit, int drc_ndx);
int soc_dpp_drc_combo16_shmoo_dram_info_access(int unit, combo16_shmoo_dram_info_t** shmoo_info);
int soc_dpp_drc_combo28_shmoo_dram_info_access(int unit, combo28_shmoo_dram_info_t** shmoo_info);
int soc_dpp_drc_combo16_shmoo_vendor_info_access(int unit, combo16_vendor_info_t** vendor_info);
int soc_dpp_drc_combo28_shmoo_vendor_info_access(int unit, combo28_vendor_info_t** vendor_info);
int soc_dpp_drc_combo28_shmoo_cas_latency_get(int unit, int* cas_latency);


int soc_dpp_drc_combo28_shmoo_drc_precharge_all(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_drc_active_gddr5_cmd(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_write_mpr(int unit, int drc_ndx, uint32 intial_calib_mpr_addr, uint32 mpr_mode, uint32 mpr_page,uint32 mrs_readout,int enable_mpr);
int soc_dpp_drc_combo28_shmoo_load_mpr_pattern(int unit, int drc_ndx, uint32 mpr_location, uint32 mpr_pattern);
int soc_dpp_drc_combo28_shmoo_drc_trigger_dram_init(int unit, int drc_ndx);
int soc_dpp_drc_combo28_shmoo_wait_dram_init_done(int unit, int drc_ndx);
#endif 
