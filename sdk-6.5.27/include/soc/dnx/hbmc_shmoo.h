
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef _HBMC_SHMOO_H
#define _HBMC_SHMOO_H

#include <sal/types.h>
#include <shared/bitop.h>
#include <soc/dnx/hbm_phy_regs.h>

#define HBMC_SHMOO_BIST_NOF_PATTERNS 8
#define HBMC_SHMOO_BIST_NOF_WORDS_IN_PATTERN 8
#define HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS 16
#define HBMC_SHMOO_PLL_NOF_NDIV_CHANNELS 6
#define HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS 2
#define HBMC_SHMOO_BIST_DATA_ERR_BITS 128
#define HBMC_SHMOO_BIST_DM_ERR_BITS 16
#define HBMC_SHMOO_BIST_DBI_ERR_BITS 16

#define HBMC_SHMOO_NOF_HBM_CHANNELS 8
#define HBMC_SHMOO_CHANNEL_METADATA_SIZE 121
#define HBMC_SHMOO_MIDSTACK_METADATA_SIZE 3
#define HBMC_SHMOO_FIELDS_ALL (-1)

#define HBMC_SHMOO_CFG_FLAG_FORCE_BIST_CONFIGURATION    0x1
#define HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_READ_LFSR     0x2
#define HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_WRITE_LFSR    0x4

#define HBMC_SHMOO_ERROR_ARRAY_LENGTH            4

#define HBMC_SHMOO_LEVEL_0 (0x1)
#define HBMC_SHMOO_LEVEL_1 (0x2)
#define HBMC_SHMOO_LEVEL_2 (0x4)
#define HBMC_SHMOO_OTP_VER_1 (0x10)
#define HBMC_SHMOO_OTP_VER_2 (0x20)
#define HBMC_SHMOO_OTP_VER_1_2 (HBMC_SHMOO_OTP_VER_1 | HBMC_SHMOO_OTP_VER_2)

typedef uint32 hbmc_shmoo_error_array_t[HBMC_SHMOO_ERROR_ARRAY_LENGTH];

typedef struct hbmc_shmoo_pll_s
{
    uint32 ndiv_p;
    uint32 ndiv_q;
    uint32 ndiv_frac_mode_sel;
    uint32 pdiv;
    uint32 ndiv_int;
    uint32 ssc_limit;
    uint32 ssc_mode;
    uint32 ssc_step;
    uint32 vco_fb_div2;
    uint32 mdiv_aux_post;
    uint32 mdiv_ch[HBMC_SHMOO_PLL_NOF_NDIV_CHANNELS];
    uint32 fref_eff_info;
    uint32 enableb_aux_post;
    uint32 enableb_ch_bitmap;
} hbmc_shmoo_pll_t;

typedef struct hbmc_shmoo_bist_err_cnt_s
{
    uint32 bist_global_err_cnt;
    uint32 bist_data_err_cnt;
    uint32 bist_dm_err_cnt;
    uint32 bist_dbi_err_cnt;
        SHR_BITDCLNAME(
    bist_data_err_bits,
    HBMC_SHMOO_BIST_DATA_ERR_BITS);
        SHR_BITDCLNAME(
    bist_dm_err_bits,
    HBMC_SHMOO_BIST_DM_ERR_BITS);
        SHR_BITDCLNAME(
    bist_dbi_err_bits,
    HBMC_SHMOO_BIST_DBI_ERR_BITS);
} hbmc_shmoo_bist_err_cnt_t;

typedef struct hbmc_shmoo_bist_status_cnt_t
{
    uint32 write_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS];
    uint32 read_cmd_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS];
    uint32 read_data_counter[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS];
} hbmc_shmoo_bist_status_cnt_t;

typedef struct hbmc_shmoo_bist_info_s
{

    uint32 write_weight;

    uint32 read_weight;

    uint32 bist_timer_us;

    uint32 bist_num_actions[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS];

    uint32 row_start_index;
    uint32 column_start_index;
    uint32 bank_start_index;

    uint32 row_end_index;
    uint32 column_end_index;
    uint32 bank_end_index;

    uint32 bist_refresh_enable;

    uint32 bist_ignore_address;

    uint32 same_row_commands;

    uint32 data_mode;

    uint32 data_pattern[HBMC_SHMOO_BIST_NOF_PATTERNS][HBMC_SHMOO_BIST_NOF_WORDS_IN_PATTERN];

    uint32 prbs_seeds[HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS];

} hbmc_shmoo_bist_info_t;

typedef struct hbmc_shmoo_dram_info_s
{
    uint32 ctl_type;
    uint32 dram_type;
    uint32 dram_bitmap;
    int num_columns;
    int num_rows;
    int num_banks;
    int data_rate_mbps;
    int ref_clk_mhz;
    uint32 refi;
    uint32 command_parity_latency;
    uint32 parity_latency;
    int sim_system_mode;
} hbmc_shmoo_dram_info_t;

typedef enum hbmc_shmoo_hbm_model_part_type_e
{
    HBM_TYPE_INVALID = 0,
    HBM_TYPE_SAMSUNG_CORE_GROUP_B = 1,
    HBM_TYPE_SAMSUNG_CORE_GROUP_X = 2,
    HBM_TYPE_NOF = 3
} hbmc_shmoo_hbm_model_part_t;

typedef int (
    *soc_hbm16_phy_reg_read_t) (
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 *data);
typedef int (
    *soc_hbm16_phy_reg_write_t) (
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data);
typedef int (
    *soc_hbm16_phy_reg_modify_t) (
    int unit,
    int hbm_ndx,
    int channel,
    uint32 addr,
    uint32 data,
    uint32 mask);
typedef int (
    *shmoo_hbm16_hbmc_bist_conf_set_t) (
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    const hbmc_shmoo_bist_info_t * info);
typedef int (
    *shmoo_hbm16_hbmc_bist_err_cnt_t) (
    int unit,
    int flags,
    int hbm_ndx,
    int channel,
    hbmc_shmoo_bist_err_cnt_t * info);
typedef int (
    *shmoo_hbm16_hbmc_custom_bist_run_t) (
    int unit,
    int hbm_ndx,
    int channel,
    hbmc_shmoo_bist_info_t * bist_info,
    hbmc_shmoo_error_array_t * shmoo_error_array);
typedef int (
    *shmoo_hbm16_hbmc_dram_init_t) (
    int unit,
    int hbm_ndx,
    int phase);
typedef int (
    *shmoo_hbm16_hbmc_pll_set_t) (
    int unit,
    int hbm_ndx,
    const hbmc_shmoo_pll_t * pll_info);
typedef int (
    *shmoo_hbm16_hbmc_modify_mrs_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int mr_ndx,
    uint32 data,
    uint32 mask);
typedef int (
    *shmoo_hbm16_hbmc_enable_wr_parity_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
typedef int (
    *shmoo_hbm16_hbmc_enable_rd_parity_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
typedef int (
    *shmoo_hbm16_hbmc_enable_addr_parity_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
typedef int (
    *shmoo_hbm16_hbmc_enable_wr_dbi_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
typedef int (
    *shmoo_hbm16_hbmc_enable_rd_dbi_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
typedef int (
    *shmoo_hbm16_hbmc_enable_refresh_t) (
    int unit,
    int hbm_ndx,
    int channel,
    int enable);
typedef int (
    *shmoo_hbm16_hbmc_soft_reset_controller_without_dram_t) (
    int unit,
    int hbm_ndx,
    int channel);
typedef int (
    *shmoo_hbm16_hbmc_dram_info_access_t) (
    int unit,
    hbmc_shmoo_dram_info_t * shmoo_info);
typedef int (
    *shmoo_hbm16_hbmc_bist_status_get_t) (
    int unit,
    int hbm_ndx,
    int channel,
    uint32 *read_cmd_counter,
    uint32 *write_cmd_counter,
    uint32 *read_data_counter);
typedef int (
    *shmoo_hbm16_hbmc_model_part_get_t) (
    int unit,
    int hbm_index,
    hbmc_shmoo_hbm_model_part_t * model_part);
typedef int (
    *shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check_t) (
    int unit,
    int hbm_index,
    int channel,
    int *is_aligned);

typedef struct hbmc_shmoo_cbi_s
{

    soc_hbm16_phy_reg_read_t hbmc_phy_reg_read;

    soc_hbm16_phy_reg_write_t hbmc_phy_reg_write;

    soc_hbm16_phy_reg_modify_t hbmc_phy_reg_modify;

    shmoo_hbm16_hbmc_bist_conf_set_t hbmc_shmoo_bist_conf_set;

    shmoo_hbm16_hbmc_bist_err_cnt_t hbmc_shmoo_bist_err_cnt;

    shmoo_hbm16_hbmc_custom_bist_run_t hbmc_shmoo_custom_bist_run;

    shmoo_hbm16_hbmc_dram_init_t hbmc_shmoo_dram_init;

    shmoo_hbm16_hbmc_pll_set_t hbmc_shmoo_pll_set;

    shmoo_hbm16_hbmc_modify_mrs_t hbmc_shmoo_modify_mrs;

    shmoo_hbm16_hbmc_enable_wr_parity_t hbmc_shmoo_enable_wr_parity;

    shmoo_hbm16_hbmc_enable_rd_parity_t hbmc_shmoo_enable_rd_parity;

    shmoo_hbm16_hbmc_enable_addr_parity_t hbmc_shmoo_enable_addr_parity;

    shmoo_hbm16_hbmc_enable_wr_dbi_t hbmc_shmoo_enable_wr_dbi;

    shmoo_hbm16_hbmc_enable_rd_dbi_t hbmc_shmoo_enable_rd_dbi;

    shmoo_hbm16_hbmc_enable_refresh_t hbmc_shmoo_enable_refresh;

    shmoo_hbm16_hbmc_soft_reset_controller_without_dram_t hbmc_shmoo_soft_reset_controller_without_dram;

    shmoo_hbm16_hbmc_dram_info_access_t hbmc_shmoo_dram_info_access;

    shmoo_hbm16_hbmc_bist_status_get_t hbmc_shmoo_bist_status_get;

    shmoo_hbm16_hbmc_model_part_get_t hbmc_shmoo_model_part_get;

    shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check_t hbmc_shmoo_phy_channel_dwords_alignment_check;
} hbmc_shmoo_cbi_t;

typedef struct hbmc_shmoo_channel_config_param_s
{
    soc_hbm16_channel_reg_t reg;
    int field;
    int level;
    char *name;
} hbmc_shmoo_channel_config_param_t;

typedef struct hbmc_shmoo_midstack_config_param_s
{
    soc_hbm16_midstack_reg_t reg;
    int field;
    int level;
    char *name;
} hbmc_shmoo_midstack_config_param_t;

typedef struct
{
    int valid;
    uint32 value;
} hbmc_shmoo_config_param_data_t;

typedef struct hbmc_shmoo_config_param_s
{
    hbmc_shmoo_config_param_data_t channel_data[HBMC_SHMOO_NOF_HBM_CHANNELS][HBMC_SHMOO_CHANNEL_METADATA_SIZE];
    hbmc_shmoo_config_param_data_t midstack_data[HBMC_SHMOO_MIDSTACK_METADATA_SIZE];
} hbmc_shmoo_config_param_t;

#endif
