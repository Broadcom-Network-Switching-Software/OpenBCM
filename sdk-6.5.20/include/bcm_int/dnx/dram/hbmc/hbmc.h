

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_HBMC_HBMC_H_INCLUDED

#define _BCMINT_DNX_DRAM_HBMC_HBMC_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <soc/hbmc_shmoo.h>
#include <soc/dnx/dnx_hbmc.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_dram.h>
#include <bcm/switch.h>



#define DNX_HBMC_ITER_ALL -1
#define DNX_HBMC_STR_VAL_SIZE (32)
#define DNX_HBMC_HBM_SERIAL_NUMBER_SIZE 2


typedef struct dnx_hbmc_mrs_values_s
{
    
    uint32
        mr_val[DNX_DATA_MAX_DRAM_GENERAL_INFO_MAX_NOF_DRAMS][DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_CHANNELS]
        [DNX_DATA_MAX_DRAM_GENERAL_INFO_NOF_MRS];
} dnx_hbmc_mrs_values_t;




typedef enum
{
    FIRST_DNX_HBMC_PHY_TUNE_ACTION_E = 0,
    
    DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES = FIRST_DNX_HBMC_PHY_TUNE_ACTION_E,
    
    DNX_HBMC_RESTORE_TUNE_PARAMETERS_FROM_OTP,
    
    DNX_HBMC_RUN_TUNE,
    
    DNX_HBMC_SKIP_TUNE,
    
    DNX_HBMC_RESTORE_TUNE_PARAMS_FROM_SOC_PROPS_OR_OTP_OR_TUNE,
    
    NUM_DNX_HBMC_PHY_TUNE_ACTION_E
} dnx_hbmc_phy_tune_action_e;


typedef enum dnx_hbmc_tdu_matrix_option_e
{
        
    DNX_HBMC_SINGLE_HBM_ATM = 0,
        
    DNX_HBMC_2_HBMS_ATM = 1,
        
    DNX_HBMC_NOF_ATMS = 2
} dnx_hbmc_tdu_matrix_option_t;


typedef enum dnx_hbmc_diag_registers_type_e
{
    
    DNX_HBMC_DIAG_REG_HISTOGRAM_32_8,
    
    DNX_HBMC_NOF_DIAG_REG_TYPES
} dnx_hbmc_diag_registers_type_t;


shr_error_e dnx_hbmc_configure_plls(
    int unit,
    int hbm_index,
    const hbmc_shmoo_pll_t * pll_info);


shr_error_e dnx_hbmc_pll_init(
    int unit);


shr_error_e dnx_hbmc_hbm_init(
    int unit,
    int hbm_index);


shr_error_e dnx_hbmc_mode_register_get(
    int unit,
    int hbm_index,
    int channel,
    int mr_index,
    uint32 *value);


shr_error_e dnx_hbmc_mode_register_set(
    int unit,
    int hbm_index,
    int channel,
    int mr_index,
    uint32 value);


shr_error_e dnx_hbmc_run_phy_tune(
    int unit,
    int hbm_index,
    int channel,
    int shmoo_type,
    uint32 flags,
    dnx_hbmc_phy_tune_action_e action);


shr_error_e dnx_hbmc_init_with_apd_phy(
    int unit);


shr_error_e dnx_hbmc_init(
    int unit);


shr_error_e dnx_hbmc_deinit(
    int unit);


shr_error_e dnx_hbmc_sw_state_init(
    int unit);


shr_error_e dnx_hbmc_temp_monitor_deinit(
    int unit);


shr_error_e dnx_hbmc_temp_monitor_init(
    int unit);


shr_error_e dnx_hbmc_temp_monitor_is_active(
    int unit,
    int *is_active);


shr_error_e dnx_hbmc_temp_monitor_resume(
    int unit);


shr_error_e dnx_hbmc_temp_monitor_pause(
    int unit);


shr_error_e dnx_hbmc_bist_configuration_get(
    int unit,
    int hbm_index,
    int channel,
    hbmc_shmoo_bist_info_t * info);


shr_error_e dnx_hbmc_bist_configuration_set(
    int unit,
    int hbm_index,
    int channel,
    const hbmc_shmoo_bist_info_t * info);


shr_error_e dnx_hbmc_bist_start(
    int unit,
    int hbm_index,
    int channel);


shr_error_e dnx_hbmc_bist_stop(
    int unit,
    int hbm_index,
    int channel);


shr_error_e dnx_hbmc_bist_run_done_poll(
    int unit,
    int hbm_index,
    int channel);


shr_error_e dnx_hbmc_channel_soft_init(
    int unit,
    int hbm_ndx,
    int channel);


shr_error_e dnx_hbmc_channels_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);


shr_error_e dnx_hbmc_channels_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);


shr_error_e dnx_hbmc_hbmc_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);


shr_error_e dnx_hbmc_hbmc_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);


shr_error_e dnx_hbmc_hrc_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);


shr_error_e dnx_hbmc_hrc_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);


shr_error_e dnx_hbmc_tdu_soft_init_set(
    int unit,
    int hbm_index,
    int in_soft_init);


shr_error_e dnx_hbmc_tdu_soft_reset_set(
    int unit,
    int hbm_index,
    int in_soft_reset);


shr_error_e dnx_hbmc_soft_init_mrs_save(
    int unit,
    dnx_hbmc_mrs_values_t * mr_values_p);


shr_error_e dnx_hbmc_soft_init_mrs_restore(
    int unit,
    dnx_hbmc_mrs_values_t * mr_values_p);


shr_error_e dnx_hbmc_ind_access_physical_address_read(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);


shr_error_e dnx_hbmc_ind_access_physical_address_write(
    int unit,
    uint32 hbm_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);


shr_error_e dnx_hbmc_ind_access_logical_buffer_read(
    int unit,
    int core,
    int buffer,
    int index,
    uint32 *data);


shr_error_e dnx_hbmc_ind_access_logical_buffer_write(
    int unit,
    int core,
    int buffer,
    int index,
    uint32 *pattern);


shr_error_e dnx_hbmc_redirect_traffic_to_ocb(
    int unit,
    uint32 enable,
    uint8 *hbm_is_empty);


shr_error_e dnx_hbmc_is_traffic_allowed_into_dram(
    int unit,
    uint8 *allow_traffic_to_dram);


shr_error_e dnx_hbmc_bist_status_get(
    int unit,
    int hbm_index,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);


shr_error_e dnx_hbmc_power_down_cb_register(
    int unit,
    uint32 flags,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);


shr_error_e dnx_hbmc_power_down_cb_unregister(
    int unit,
    bcm_switch_dram_power_down_callback_t callback,
    void *userdata);


shr_error_e dnx_hbmc_power_down(
    int unit);


shr_error_e dnx_hbmc_reinit(
    int unit,
    uint32 flags);


shr_error_e dnx_hbmc_temp_get(
    int unit,
    int interface_id,
    int *value);


shr_error_e dnx_hbmc_vendor_info_to_sw_state_set(
    int unit,
    int hbm_index,
    hbm_dram_vendor_info_hbm_t * vendor_info);


shr_error_e dnx_hbmc_vendor_info_from_sw_state_get(
    int unit,
    int hbm_index,
    hbm_dram_vendor_info_hbm_t * vendor_info);


shr_error_e dnx_hbmc_device_id_to_vendor_info_decode(
    int unit,
    soc_dnx_hbm_device_id_t * device_id,
    hbm_dram_vendor_info_hbm_t * vendor_info);


shr_error_e dnx_hbmc_read_lfsr_loopback_bist_run(
    int unit,
    uint32 hbm_index,
    uint32 channel);

shr_error_e dnx_hbmc_hbc_blocks_enable_set(
    int unit);


#endif 
