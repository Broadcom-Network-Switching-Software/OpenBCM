

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _BCMINT_DNX_DRAM_GDDR6_GDDR6_FUNC_H_INCLUDED
#define _BCMINT_DNX_DRAM_GDDR6_GDDR6_FUNC_H_INCLUDED


#include <soc/shmoo_g6phy16.h>
#include <bcm_int/dnx/dram/gddr6/gddr6.h>





typedef enum
{
    
    GDDR6_DRAM_BIST_DATA_PATTERN_CUSTOM,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_DIFF,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_DIFF_REVERT,

    
    GDDR6_DRAM_BIST_DATA_PATTERN_ONE,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_ZERO,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_BIT_MODE,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_SHIFT_MODE,
    
    GDDR6_DRAM_BIST_DATA_PATTERN_ADDR_MODE,
    
    GDDR6_DRAM_BIST_NOF_DATA_PATTERN_MODES
} dnx_gddr6_bist_pattern_mode;
typedef enum
{
    LDFF = 0,
    RDTR = 1,
    WRTR = 2,
    CADT = 3,
    VENDOR_ID = 4
} dnx_gddr6_training_bist_mode;

typedef struct ldff_pattern_s
{
    uint32 dq_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS][4];
    uint32 dbi_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
    uint32 edc_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
} dnx_gddr6_ldff_pattern;

typedef struct training_bist_cmd_configuration_s
{
    uint64 commands;
    int write_weight;
    int read_weight;
    int fifo_depth;
    dnx_gddr6_training_bist_mode training_bist_mode;
} dnx_gddr6_training_bist_cmd_configuration;

typedef struct dnx_gddr6_periodic_temp_readout_config_s
{
    uint32 readout_to_readout_prd;
    uint32 refresh_to_readout_prd;
    uint32 readout_done_to_done_prd;
} dnx_gddr6_periodic_temp_readout_config_t;

typedef struct dnx_gddr6_periodic_temp_readout_status_s
{
    uint32 temperature;
    uint32 is_valid;
    uint32 max_temperature;
} dnx_gddr6_periodic_temp_readout_status_t;

typedef struct training_cadt_bist_configuration_s
{
    int commands;
    shmoo_g6phy16_cadt_mode_t cadt_mode;
    int cadt_to_cadt_prd;
    int cadt_to_rden_prd;
    int cadt_prbs_mode;
    int cadt_seed;
    int cadt_invert_caui2;
    uint32 cadt_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
} dnx_gddr6_training_cadt_bist_configuration;

typedef struct training_bist_configuration_s
{
    dnx_gddr6_bist_pattern_mode dq_pattern_mode;
    dnx_gddr6_bist_pattern_mode dbi_pattern_mode;
    dnx_gddr6_bist_pattern_mode edc_pattern_mode;
    dnx_gddr6_ldff_pattern pattern;
    dnx_gddr6_training_bist_cmd_configuration cmd_cfg;
    dnx_gddr6_training_cadt_bist_configuration cadt_cfg;
} dnx_gddr6_training_bist_configuration;

typedef struct bist_configuration_s
{
    uint64 nof_commands;
    int write_weight;
    int read_weight;
    int same_row;

    dnx_gddr6_bist_address_mode_t bank_mode;
    int bank_start;
    int bank_end;
    int bank_seed;
    dnx_gddr6_bist_address_mode_t column_mode;
    int column_start;
    int column_end;
    int column_seed;
    dnx_gddr6_bist_address_mode_t row_mode;
    int row_start;
    int row_end;
    int row_seed;

    dnx_gddr6_bist_data_mode_t data_mode;
    uint32 dbi_mode;

    uint32 data_seed[8];
    uint32 dbi_seed;

    uint32 bist_dq_pattern[8][8];
    uint32 bist_dbi_pattern[8];

    uint32 bist_timer_us;
} dnx_gddr6_bist_configuration_t;

typedef struct last_returned_data
{
    uint32 last_returned_dq[8];
    uint32 last_returned_dbi;
    uint32 last_returned_edc;
} dnx_gddr6_bist_last_returned_data_t;


shr_error_e dnx_gddr6_controller_init(
    int unit,
    int dram_index,
    int ignore_vendor);


shr_error_e dnx_gddr6_dram_init(
    int unit,
    int dram_index,
    gddr6_dram_init_phase_t phase);

shr_error_e dnx_gddr6_shmoo_dram_info_get(
    int unit,
    g6phy16_shmoo_dram_info_t * shmoo_info);

shr_error_e dnx_gddr6_wck2clk_training_start(
    int unit,
    int dram_index);


shr_error_e dnx_gddr6_bist_error_counter_get(
    int unit,
    int dram_index,
    dnx_gddr6_bist_err_cnt_t * bist_error_count);

shr_error_e dnx_gddr6_data_bist_configure(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_configuration_t * bist_cfg);


shr_error_e dnx_gddr6_vendor_info_get(
    int unit,
    int dram_index,
    uint32 mode,
    uint32 *gddr6_vendor);

shr_error_e dnx_gddr6_load_precharge_all_command(
    int unit,
    int dram_index,
    uint32 cabi_enabled);

shr_error_e dnx_gddr6_load_activate_command(
    int unit,
    int dram_index,
    uint32 bank,
    uint32 cabi_enabled);


shr_error_e dnx_gddr6_dram_err_cnt_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_dram_err_cnt_t * dram_err_cnt);


shr_error_e dnx_gddr6_bist_err_cnt_get(
    int unit,
    int dram_index,
    int channel,
    dnx_gddr6_bist_err_cnt_t * bist_err_cnt);

#endif
