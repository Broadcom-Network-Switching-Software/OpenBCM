

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCM_DNX_DRAM_HBMC_HBMCDBALACCESS_H_INCLUDED

#define _BCM_DNX_DRAM_HBMC_HBMCDBALACCESS_H_INCLUDED


#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_debug.h>
#include <soc/hbmc_shmoo.h>


int dnx_hbmc_get_sequential_channel(
    int unit,
    int hbm_index,
    int channel_in_hbm);


shr_error_e dnx_hbmc_dbal_access_phy_channel_register_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 address,
    uint32 data);


shr_error_e dnx_hbmc_dbal_access_phy_channel_register_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 address,
    uint32 *data);


shr_error_e dnx_hbmc_dbal_access_phy_midstack_register_set(
    int unit,
    int hbm_index,
    uint32 address,
    uint32 data);


shr_error_e dnx_hbmc_dbal_access_phy_midstack_register_get(
    int unit,
    int hbm_index,
    uint32 address,
    uint32 *data);


shr_error_e dnx_hbmc_dbal_access_bist_configuration_set(
    int unit,
    int hbm_index,
    int channel,
    const hbmc_shmoo_bist_info_t * info);


shr_error_e dnx_hbmc_dbal_access_bist_configuration_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_info_t * info);


shr_error_e dnx_hbmc_dbal_access_bist_status_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);


shr_error_e dnx_hbmc_dbal_access_bist_run(
    int unit,
    int hbm_index,
    int channel,
    uint32 start);


shr_error_e dnx_hbmc_dbal_access_bist_run_done_poll(
    int unit,
    int hbm_index,
    int channel);


shr_error_e dnx_hbmc_dbal_access_bist_write_lfsr_error_counters_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters);


shr_error_e dnx_hbmc_dbal_access_bist_error_counters_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * error_counters);


shr_error_e dnx_hbmc_dbal_access_channel_soft_init_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 in_soft_init);


shr_error_e dnx_hbmc_dbal_access_channels_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);


shr_error_e dnx_hbmc_dbal_access_channels_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);


shr_error_e dnx_hbmc_dbal_access_channel_soft_init_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *in_soft_init);


shr_error_e dnx_hbmc_dbal_access_hbmc_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);


shr_error_e dnx_hbmc_dbal_access_hbmc_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);


shr_error_e dnx_hbmc_dbal_access_hrc_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);


shr_error_e dnx_hbmc_dbal_access_hrc_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);


shr_error_e dnx_hbmc_dbal_access_tdu_soft_reset_set(
    int unit,
    int hbm_index,
    uint32 in_soft_reset);


shr_error_e dnx_hbmc_dbal_access_tdu_soft_init_set(
    int unit,
    int hbm_index,
    uint32 in_soft_init);


shr_error_e dnx_hbmc_dbal_access_enable_refresh_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *enable_refresh);


shr_error_e dnx_hbmc_dbal_access_enable_refresh_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 enable_refresh);


shr_error_e dnx_hbmc_dbal_access_pll_config_set(
    int unit,
    int hbm_index,
    const hbmc_shmoo_pll_t * pll_info);


shr_error_e dnx_hbmc_dbal_access_pll_config_get(
    int unit,
    int hbm_index,
    hbmc_shmoo_pll_t * pll_info);


shr_error_e dnx_hbmc_dbal_access_pll_configuration_for_apd_phy(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_pll_control_set(
    int unit,
    int hbm_index,
    uint32 reset,
    uint32 post_reset);


shr_error_e dnx_hbmc_dbal_access_pll_status_locked_poll(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_phy_control_out_of_reset_config(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_phy_channel_out_of_reset_config(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_hbmc_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable);


shr_error_e dnx_hbmc_dbal_access_hcc_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable);


shr_error_e dnx_hbmc_dbal_access_dynamic_memory_access_set(
    int unit,
    int hbm_index,
    int enable);


shr_error_e dnx_hbmc_dbal_access_hcc_configuration_ecc_dbi_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *ecc,
    uint32 *dbi_read,
    uint32 *dbi_write);


shr_error_e dnx_hbmc_dbal_access_hcc_configuration_ecc_dbi_set(
    int unit,
    int hbm_index,
    int channel,
    uint32 ecc,
    uint32 dbi_read,
    uint32 dbi_write);


shr_error_e dnx_hbmc_dbal_access_hcc_hbm_config(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_hcc_tsm_config(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_hcc_data_source_config(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_hcc_data_path_enable_set(
    int unit,
    int hbm_index,
    uint32 enable);


shr_error_e dnx_hbmc_dbal_access_hcc_request_path_enable_set(
    int unit,
    int hbm_index,
    uint32 enable);


shr_error_e dnx_hbmc_dbal_access_hrc_enable_set(
    int unit,
    int hbm_index,
    uint32 enable);


shr_error_e dnx_hbmc_dbal_access_ecc_enable(
    int unit,
    int hbm_index,
    int channel,
    uint32 enable);


shr_error_e dnx_hbmc_dbal_access_cattrip_indication_mask_set(
    int unit,
    int hbm_index,
    uint32 mask_cattrip_indication);


shr_error_e dnx_hbmc_dbal_access_reset_control_set(
    int unit,
    int hbm_index,
    uint32 out_of_reset);


shr_error_e dnx_hbmc_dbal_access_cattrip_interrupt_clear(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_output_enable_set(
    int unit,
    int hbm_index,
    uint32 output_enable);


shr_error_e dnx_hbmc_dbal_access_clocks_set(
    int unit,
    int hbm_index,
    uint32 ck_ui_0,
    uint32 ck_ui_1);


shr_error_e dnx_hbmc_dbal_access_clocks_output_enable_set(
    int unit,
    int hbm_index,
    uint32 ck_oen_ui_0,
    uint32 ck_oen_ui_1);


shr_error_e dnx_hbmc_dbal_access_mr_set(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 mr_index,
    uint32 value);


shr_error_e dnx_hbmc_dbal_access_mr_get(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 mr_index,
    uint32 *value);


shr_error_e dnx_hbmc_dbal_access_mr_init_channel_set(
    int unit,
    int hbm_index,
    int channel_index);


shr_error_e dnx_hbmc_dbal_access_mr_init_set(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_dbal_access_update_hbm_on_mode_register_change(
    int unit,
    int hbm_index,
    int channel,
    int update);


shr_error_e dnx_hbmc_dbal_access_cke_set(
    int unit,
    int hbm_index,
    uint32 cke_ui);


shr_error_e dnx_hbmc_dbal_access_data_source_id_get(
    int unit,
    int core,
    uint32 *data_source_id);


shr_error_e dnx_hbmc_dbal_access_tdu_configure(
    int unit);


shr_error_e dnx_hbmc_dbal_access_tdu_atm_configure(
    int unit,
    const uint32 *atm);


shr_error_e dnx_hbmc_dbal_access_dram_cpu_access_get(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);


shr_error_e dnx_hbmc_dbal_access_dram_cpu_access_set(
    int unit,
    uint32 module,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);

#endif 
