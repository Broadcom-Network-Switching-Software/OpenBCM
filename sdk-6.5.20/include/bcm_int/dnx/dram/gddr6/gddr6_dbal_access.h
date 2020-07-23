

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_DRAM_GDDR6_GDDR6_DBAL_ACCESS_H_INCLUDED

#define _BCM_DNX_DRAM_GDDR6_GDDR6_DBAL_ACCESS_H_INCLUDED


#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/shmoo_g6phy16.h>


int dnx_gddr6_get_sequential_channel(
    int unit,
    int dram_index,
    int channel_in_dram);


shr_error_e dnx_gddr6_dbal_access_phy_channel_register_set(
    int unit,
    int dram_index,
    uint32 address,
    uint32 data);


shr_error_e dnx_gddr6_dbal_access_phy_channel_register_get(
    int unit,
    int dram_index,
    uint32 address,
    uint32 *data);

shr_error_e dnx_gddr6_dbal_access_tsm_banks_disable_set(
    int unit,
    int dram_index,
    uint32 disable);


shr_error_e dnx_gddr6_dbal_access_bist_run(
    int unit,
    int dram_index,
    int channel,
    uint32 start);

shr_error_e dnx_gddr6_dbal_access_training_bist_run(
    int unit,
    int dram_index,
    uint32 start);


shr_error_e dnx_gddr6_dbal_access_bist_run_done_poll(
    int unit,
    int dram_index,
    int channel);

shr_error_e dnx_gddr6_dbal_access_training_bist_run_done_poll(
    int unit,
    int dram_index);


shr_error_e dnx_gddr6_dbal_access_channel_soft_init_set(
    int unit,
    int dram_index,
    int channel,
    uint32 in_soft_init);


shr_error_e dnx_gddr6_dbal_access_channels_soft_reset_set(
    int unit,
    int dram_index,
    uint32 in_soft_reset);


shr_error_e dnx_gddr6_dbal_access_channels_soft_init_set(
    int unit,
    int dram_index,
    uint32 in_soft_init);


shr_error_e dnx_gddr6_dbal_access_enable_refresh_get(
    int unit,
    int dram_index,
    int channel,
    uint32 *enable_refresh,
    uint32 *enable_refresh_ab);


shr_error_e dnx_gddr6_dbal_access_enable_refresh_set(
    int unit,
    int dram_index,
    int channel,
    uint32 enable_refresh,
    uint32 enable_refresh_ab);

shr_error_e dnx_gddr6_dbal_access_pll_reset(
    int unit,
    int dram_index,
    uint32 pll_in_reset);

shr_error_e dnx_gddr6_dbal_access_pll_post_reset_set(
    int unit,
    int dram_index,
    uint32 pll_post_reset);


shr_error_e dnx_gddr6_dbal_access_pll_config_set(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);


shr_error_e dnx_gddr6_dbal_access_pll_config_get(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);

shr_error_e dnx_gddr6_dbal_access_pll_div_config_set(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);

shr_error_e dnx_gddr6_dbal_access_pll_init(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);

shr_error_e dnx_gddr6_dbal_access_pll_status_get(
    int unit,
    int dram_index,
    uint32 *pll_locked,
    uint32 *pll_stat_out);


shr_error_e dnx_gddr6_dbal_access_pll_status_locked_poll(
    int unit,
    int dram_index);

shr_error_e dnx_gddr6_dbal_access_phy_iddq_set(
    int unit,
    int dram_index,
    uint32 iddq);

shr_error_e dnx_gddr6_dbal_access_pll_refclk_sel(
    int unit,
    int dram_index,
    uint32 pll_clk_sel);


shr_error_e dnx_gddr6_dbal_access_phy_out_of_reset_config(
    int unit,
    int dram_index,
    int out_of_reset);

shr_error_e dnx_gddr6_dbal_access_phy_write_fifo_enable_config(
    int unit,
    int dram_index,
    uint32 write_fifo_enable);

shr_error_e dnx_gddr6_dbal_access_phy_ilm_mode_config(
    int unit,
    int dram_index,
    uint32 ilm_mode);


shr_error_e dnx_gddr6_dbal_access_dynamic_memory_access_set(
    int unit,
    int dram_index,
    int enable);


shr_error_e dnx_gddr6_dbal_access_dram_controller_config(
    int unit,
    uint32 dram_index,
    int ignore_vendor);

shr_error_e dnx_gddr6_dbal_access_dram_reset_config(
    int unit,
    int dram_index,
    uint32 out_of_reset);


shr_error_e dnx_gddr6_dbal_access_mr_set(
    int unit,
    uint32 dram_index,
    uint32 mr_index,
    uint32 value);


shr_error_e dnx_gddr6_dbal_access_channel_mr_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 mr_index,
    uint32 *value);

shr_error_e dnx_gddr6_dbal_access_channel_mr_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 mr_index,
    uint32 value);


shr_error_e dnx_gddr6_dbal_access_cpu_command_set(
    int unit,
    int dram_index,
    uint32 ca,
    uint32 cabi_n);


shr_error_e dnx_gddr6_dbal_force_constant_cpu_value_set(
    int unit,
    int dram_index,
    uint32 ca,
    uint32 cabi_n);

shr_error_e dnx_gddr6_dbal_release_constant_cpu_set(
    int unit,
    int dram_index);


shr_error_e dnx_gddr6_dbal_access_wck_set(
    int unit,
    int dram_index,
    uint32 wck);

shr_error_e dnx_gddr6_dbal_access_update_dram_on_mode_register_change(
    int unit,
    int dram_index,
    int update);

shr_error_e dnx_gddr6_dbal_access_force_update_per_mode_register_bitmap_set(
    int unit,
    int dram_index,
    uint32 force);

shr_error_e dnx_gddr6_dbal_access_mask_update_per_mode_register_bitmap_set(
    int unit,
    int dram_index,
    uint32 mask);


shr_error_e dnx_gddr6_dbal_access_cke_set(
    int unit,
    int dram_index,
    uint32 cke_ui);


shr_error_e dnx_gddr6_dbal_access_tdu_configure(
    int unit);


shr_error_e dnx_gddr6_dbal_access_tdu_atm_configure(
    int unit,
    const uint32 *atm);


shr_error_e dnx_gddr6_dbal_access_dram_cpu_access_get(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);


shr_error_e dnx_gddr6_dbal_access_dram_cpu_access_set(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);

shr_error_e dnx_gddr6_dbal_access_ca_bit_map_set(
    int unit,
    uint32 dram_index);

shr_error_e dnx_gddr6_dbal_access_dq_bit_map_set(
    int unit,
    uint32 dram_index);

shr_error_e dnx_gddr6_dbal_access_cadt_byte_map_set(
    int unit,
    uint32 dram_index);

shr_error_e dnx_gddr6_dbal_access_wr_crc_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);

shr_error_e dnx_gddr6_dbal_access_rd_crc_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);

shr_error_e dnx_gddr6_dbal_access_wr_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);

shr_error_e dnx_gddr6_dbal_access_rd_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 enable);


shr_error_e dnx_gddr6_dbal_access_dram_err_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_dram_err_cnt_t * dram_err_cnt);


shr_error_e dnx_gddr6_dbal_access_bist_err_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt);


shr_error_e dnx_gddr6_dbal_access_bist_status_cnt_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt);


shr_error_e dnx_gddr6_dbal_access_rd_fifo_rstn_set(
    int unit,
    uint32 dram_index,
    uint32 rstn);


shr_error_e dnx_gddr6_dbal_access_channel_rd_fifo_rstn_set(
    int unit,
    uint32 dram_index,
    int channel,
    uint32 rstn);


shr_error_e dnx_gddr6_dbal_access_pipelines_interrupts_clear(
    int unit,
    uint32 dram_index,
    uint32 channel);


shr_error_e dnx_gddr6_dbal_access_pipelines_interrupts_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *is_underflow,
    uint32 *is_overflow);


shr_error_e dnx_gddr6_dbal_access_ldff_training_pattern_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_ldff_pattern * ldff_pattern);

shr_error_e dnx_gddr6_dbal_access_cadt_training_pattern_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_cadt_bist_configuration * cadt_bist);


shr_error_e dnx_gddr6_dbal_access_cmd_training_bist_configure_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_bist_cmd_configuration * training_bist_conf);


shr_error_e dnx_gddr6_dbal_access_periodic_temp_readout_config(
    int unit,
    uint32 dram_index,
    dnx_gddr6_periodic_temp_readout_config_t * temp_readout_config);


shr_error_e dnx_gddr6_dbal_access_periodic_temp_readout_enable(
    int unit,
    uint32 dram_index,
    uint32 channel,
    int enable);


shr_error_e dnx_gddr6_dbal_access_periodic_temp_readout_status_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_periodic_temp_readout_status_t * temp_readout_status);


shr_error_e dnx_gddr6_dbal_access_dynamic_calibration_config(
    int unit,
    uint32 dram_index,
    uint32 channel,
    int set_init_position);


shr_error_e dnx_gddr6_dbal_access_dynamic_calibration_enable_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *enable);


shr_error_e dnx_gddr6_dbal_access_dynamic_calibration_enable_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 enable);


shr_error_e dnx_gddr6_dbal_access_bist_configure_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_configuration_t * bist_conf);

shr_error_e dnx_gddr6_dbal_access_training_cadt_mode_set(
    int unit,
    uint32 dram_index,
    uint32 cadt_mode);

shr_error_e dnx_gddr6_dbal_access_mr_init_set(
    int unit,
    int dram_index,
    int mr_index);



shr_error_e dnx_gddr6_dbal_access_vendor_id_mode_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_info_mode_t mode);


shr_error_e dnx_gddr6_dbal_access_channel_bist_last_returned_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    dnx_gddr6_bist_last_returned_data_t * last_returned_data);

shr_error_e dnx_gddr6_dbal_access_channel_rd_dbi_enable_get(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 *enable);

shr_error_e dnx_gddr6_dbal_access_channel_rd_dbi_enable_set(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 enable);

shr_error_e dnx_gddr6_dbal_access_training_bist_mode_set(
    int unit,
    uint32 dram_index,
    dnx_gddr6_training_bist_mode mode);

#endif 
