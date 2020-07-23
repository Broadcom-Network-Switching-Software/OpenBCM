

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_GDDR6_GDDR6_H_INCLUDED

#define _BCMINT_DNX_DRAM_GDDR6_GDDR6_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>

#ifdef DNX_DATA_INTERNAL
#undef DNX_DATA_INTERNAL
#endif
#include <soc/shmoo_g6phy16.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <bcm/switch.h>


#define DNX_GDDR6_ITER_ALL -1
#define DNX_GDDR6_CHANNEL_ITER_ALL -1

#define GDDR6_SWAP_SUFFIX_BUFFER_SIZE 256
#define GDDR6_SOC_PROPERTY_STR_BUFFER_SIZE 256
#define GDDR6_VAL_STR_BUFFER_SIZE 1024
#define GDDR6_STR_VAL_SIZE (32)

#define BURST_LENGTH 16
#define BURST_LENGTH_DIV_4 BURST_LENGTH/4
#define DNX_GDDR6_VAL_IS_IN_CLOCKS_BIT (31)


typedef enum
{
    FIRST_DNX_GDDR6_PHY_TUNE_ACTION_E = 0,
    
    DNX_GDDR6_RESTORE_TUNE_PARAMETERS = FIRST_DNX_GDDR6_PHY_TUNE_ACTION_E,
    
    DNX_GDDR6_RUN_TUNE,
    
    DNX_GDDR6_RESTORE_TUNE_PARAMETERS_OR_RUN_TUNE,
    
    DNX_GDDR6_SKIP_TUNE,
    
    NUM_DNX_GDDR6_PHY_TUNE_ACTION_E
} dnx_gddr6_phy_tune_action_e;


typedef enum dnx_gddr6_tdu_matrix_option_e
{
        
    DNX_GDDR6_SINGLE_DRAM_ATM = 0,
        
    DNX_GDDR6_2_DRAMS_ATM = 1,
        
    DNX_GDDR6_NOF_ATMS = 2
} dnx_gddr6_tdu_matrix_option_t;

typedef enum dnx_gddr6_bist_address_mode_e
{
    DNX_GDDR6_BIST_ADDRESS_MODE_INCREMENTAL = 0,
    DNX_GDDR6_BIST_ADDRESS_MODE_PRBS = 1,
} dnx_gddr6_bist_address_mode_t;

typedef enum dnx_gddr6_info_mode_e
{
    DNX_GDDR6_ID1 = 1,
    DNX_GDDR6_TEMP = 2,
    DNX_GDDR6_ID2 = 3,
} dnx_gddr6_info_mode_t;

typedef enum dnx_gddr6_bist_data_mode_e
{
    DNX_GDDR6_BIST_DATA_MODE_PATTERN = 0,
    DNX_GDDR6_BIST_DATA_MODE_PRBS = 1,
} dnx_gddr6_bist_data_mode_t;


typedef struct gddr6_dram_err_cnt_s
{
    
    uint32 read_crc_err_cnt[DNX_DATA_MAX_DRAM_GDDR6_BYTES_PER_CHANNEL][2];
    
    uint32 write_crc_err_cnt[DNX_DATA_MAX_DRAM_GDDR6_BYTES_PER_CHANNEL][2];
} dnx_gddr6_dram_err_cnt_t;

typedef struct gddr6_bist_err_cnt_s
{
    uint32 cadt_err_cnt;
    uint32 cadt_err_bitmap[1];
    uint32 bist_data_err_cnt;
    uint32 bist_data_err_bitmap[1];
    uint32 bist_edc_err_cnt;
    uint32 bist_edc_err_bitmap[1];
    uint32 bist_dbi_err_cnt;
    uint32 bist_dbi_err_bitmap[1];
} dnx_gddr6_bist_err_cnt_t;

typedef struct gddr6_bist_status_cnt_s
{
    uint32 write_command_cnt[2];
    uint32 read_command_cnt[2];
    uint32 read_data_cnt[2];
    uint32 read_training_data_cnt[2];
    uint32 read_edc_cnt[2];
    uint32 wrtr_command_cnt[2];
    uint32 rdtr_command_cnt[2];
    uint32 ldff_command_cnt[2];
} dnx_gddr6_bist_status_cnt_t;


shr_error_e dnx_gddr6_index_verify(
    int unit,
    int dram_index);


shr_error_e dnx_gddr6_sw_state_init(
    int unit);


shr_error_e dnx_gddr6_init(
    int unit);


shr_error_e dnx_gddr6_deinit(
    int unit);


shr_error_e dnx_gddr6_configure_plls(
    int unit,
    int dram_index,
    const g6phy16_drc_pll_t * pll_info);


shr_error_e dnx_gddr6_phy_out_of_reset(
    int unit,
    int dram_index);


shr_error_e dnx_gddr6_phy_pll_init(
    int unit,
    int dram_index);


shr_error_e dnx_gddr6_mode_register_get(
    int unit,
    int dram_index,
    int channel,
    int mr_index,
    uint32 *value);

shr_error_e dnx_gddr6_mode_register_set(
    int unit,
    int dram_index,
    int channel,
    int mr_index,
    uint32 value);

shr_error_e dnx_gddr6_channels_soft_init(
    int unit,
    int dram_ndx);

shr_error_e dnx_gddr6_ind_access_physical_address_read(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *data);

shr_error_e dnx_gddr6_ind_access_physical_address_write(
    int unit,
    uint32 dram_index,
    uint32 channel,
    uint32 bank,
    uint32 row,
    uint32 column,
    uint32 *pattern);

shr_error_e dnx_gddr6_ind_access_logical_buffer_read(
    int unit,
    int buffer,
    int index,
    uint32 *data);


shr_error_e dnx_gddr6_ind_access_logical_buffer_write(
    int unit,
    int buffer,
    int index,
    uint32 *pattern);

shr_error_e soc_dnx_gddr6_access_init(
    int unit);


shr_error_e dnx_gddr6_tdu_configure(
    int unit);


shr_error_e dnx_gddr6_channels_soft_reset_set(
    int unit,
    int dram_index,
    int in_soft_reset);


shr_error_e dnx_gddr6_channels_soft_init_set(
    int unit,
    int dram_index,
    int in_soft_init);


shr_error_e dnx_gddr6_phy_init(
    int unit);

shr_error_e dnx_gddr6_dram_refresh_enable_bitmap_get(
    int unit,
    int dram_index,
    uint32 *refresh_enable_bitmap,
    uint32 *refresh_ab_enable_bitmap);

shr_error_e dnx_gddr6_dram_refresh_enable_bitmap_set(
    int unit,
    int dram_index,
    uint32 refresh_enable_bitmap,
    uint32 refresh_ab_enable_bitmap);


shr_error_e dnx_gddr6_bist_start(
    int unit,
    int dram_index,
    int channel);


shr_error_e dnx_gddr6_bist_stop(
    int unit,
    int dram_index,
    int channel);


shr_error_e dnx_gddr6_bist_run_done_poll(
    int unit,
    int dram_index,
    int channel);


shr_error_e dnx_gddr6_run_phy_tune(
    int unit,
    int dram_index,
    int shmoo_type,
    uint32 shmoo_flags,
    dnx_gddr6_phy_tune_action_e action);


shr_error_e dnx_gddr6_bist_errors_check(
    int unit,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt,
    int *bist_error_indicated);


shr_error_e dnx_gddr6_redirect_traffic_to_ocb(
    int unit,
    uint32 enable,
    uint8 *dram_is_empty);


shr_error_e dnx_gddr6_bist_status_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_status_cnt_t * bist_status_cnt);


shr_error_e dnx_gddr6_dynamic_calibration_enable(
    int unit,
    int dram_index,
    int channel,
    int set_init_position);


shr_error_e dnx_gddr6_dynamic_calibration_disable(
    int unit,
    int dram_index,
    int channel);


shr_error_e dnx_gddr6_dynamic_calibration_is_enabled(
    int unit,
    int dram_index,
    int channel,
    uint32 *is_enabled);


shr_error_e dnx_gddr6_dram_temp_get(
    int unit,
    int dram_index,
    bcm_switch_thermo_sensor_t * sensor_data);


shr_error_e dnx_gddr6_dram_vendor_id_get(
    int unit,
    int dram,
    uint32 *vendor_id);


shr_error_e dnx_gddr6_dram_vendor_id_to_vendor_info_decode(
    int unit,
    uint32 vendor_id,
    gddr6_dram_vendor_info_t * vendor_info);


shr_error_e dnx_gddr6_vendor_info_from_sw_state_get(
    int unit,
    int dram_index,
    gddr6_dram_vendor_info_t * vendor_info);


shr_error_e dnx_gddr6_vendor_info_from_sw_state_set(
    int unit,
    int dram_index,
    gddr6_dram_vendor_info_t * vendor_info);


extern shr_error_e dnx_gddr6_call_backs_register(
    int unit);

shr_error_e dnx_gddr6_dcc_blocks_enable_set(
    int unit);

#endif
