/** \file include/soc/hbmc_shmoo.h
 * 
 * shmoo definitions for CBs implementation
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _HBMC_SHMOO_H
#define _HBMC_SHMOO_H

#include <sal/types.h>
#include <shared/bitop.h>
#include <soc/hbm_phy_regs.h>



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

/** force bist to run with received configuration */
#define HBMC_SHMOO_CFG_FLAG_FORCE_BIST_CONFIGURATION    0x1
#define HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_READ_LFSR     0x2
#define HBMC_SHMOO_CFG_FLAG_LOOPBACK_MODE_WRITE_LFSR    0x4

#define HBMC_SHMOO_ERROR_ARRAY_LENGTH            4

/* defines the level of the tune config param field according to priority */
/* level_0 is highest priority */
#define HBMC_SHMOO_LEVEL_0 (0x1)
#define HBMC_SHMOO_LEVEL_1 (0x2)
#define HBMC_SHMOO_LEVEL_2 (0x4)
#define HBMC_SHMOO_OTP_VER_1 (0x10)
#define HBMC_SHMOO_OTP_VER_2 (0x20)
#define HBMC_SHMOO_OTP_VER_1_2 (HBMC_SHMOO_OTP_VER_1 | HBMC_SHMOO_OTP_VER_2)

typedef uint32 hbmc_shmoo_error_array_t[HBMC_SHMOO_ERROR_ARRAY_LENGTH];

typedef struct hbmc_shmoo_pll_s{
    uint32 ndiv_p;                                                /* HbmPllConfig -> HbmPllNdivP */
    uint32 ndiv_q;                                                /* HbmPllConfig -> HbmPllNdivQ */
    uint32 ndiv_frac_mode_sel;                                    /* HbmPllConfig -> HbmPllNdivFracModeSel */
    uint32 pdiv;                                                  /* HbmPllConfig -> HbmPllPdiv */
    uint32 ndiv_int;                                              /* HbmPllConfig -> HbmPllNdivInt */
    uint32 ssc_limit;                                             /* HbmPllConfig -> HbmPllSscLimit */
    uint32 ssc_mode;                                              /* HbmPllConfig -> HbmPllSscMode */
    uint32 ssc_step;                                              /* HbmPllConfig -> HbmPllSscStep */
    uint32 vco_fb_div2;                                           /* HbmPllConfig -> HbmPllVcoFbDiv2 */
    uint32 mdiv_aux_post;                                         /* HbmPllConfig -> HbmPllAuxPostMdiv */
    uint32 mdiv_ch[HBMC_SHMOO_PLL_NOF_NDIV_CHANNELS];             /* HbmPllConfig -> HbmPllCh0Mdiv ... HbmPllCh5Mdiv */
    uint32 fref_eff_info;                                         /* HbmPllConfig -> HbmPllFrefeffInfo */
    uint32 enableb_aux_post;                                      /* HbmPllConfig -> HbmPllAuxPostEnableb */
    uint32 enableb_ch_bitmap;                                     /* HbmPllConfig -> HbmPllChEnableb */
} hbmc_shmoo_pll_t;

typedef struct hbmc_shmoo_bist_err_cnt_s{
    uint32 bist_global_err_cnt;                 /* sum all error counters */
    uint32 bist_data_err_cnt;                   /* BistErrCounters -> BistDataErrCnt */
    uint32 bist_dm_err_cnt;                     /* BistErrCounters -> BistDmErrCnt */
    uint32 bist_dbi_err_cnt;                    /* BistErrCounters -> BistDbiErrCnt */
    SHR_BITDCLNAME(bist_data_err_bits, HBMC_SHMOO_BIST_DATA_ERR_BITS);    /* BistErrCounters -> BistDataErrBitmap */
    SHR_BITDCLNAME(bist_dm_err_bits, HBMC_SHMOO_BIST_DM_ERR_BITS);       /* BistErrCounters -> BistDmErrBitmap */
    SHR_BITDCLNAME(bist_dbi_err_bits, HBMC_SHMOO_BIST_DBI_ERR_BITS);      /* BistErrCounters -> BistDbiErrBitmap */
} hbmc_shmoo_bist_err_cnt_t;

typedef struct hbmc_shmoo_bist_info_s{
    /* defines how many writes to before moving to reads */
    uint32 write_weight;            /* BistCommandConfig -> ConsecutiveWriteCommands */
    /* defines how many reads to before moving to writes */
    uint32 read_weight;             /* BistCommandConfig -> ConsecutiveReadCommands */
    /* Bist will be performed untill timer has expired and not according to number of actions */
    uint32 bist_timer_us;           /* run bist until timer expires, value of 0 is Infinite run that is not limited by time */
    /* Bist will perform bist_num_actions and then stop, value of 0 is Infinite run that is not limited by number of actions */
    uint32 bist_num_actions[HBMC_SHMOO_BIST_NOF_ACTIONS_WORDS];        /* BistCommandConfig -> NumOfCommands, if = 0 it mean Infinite run! */

    /* start bist in the following <row | columm | bank> */
    uint32 row_start_index;         /* BistAddrConfig -> RowStart */
    uint32 column_start_index;      /* BistAddrConfig -> ColumnStart */
    uint32 bank_start_index;        /* BistAddrConfig -> BankStart */

    /* finish bist in the following <row | column | bank> */
    uint32 row_end_index;           /* BistAddrConfig -> RowEnd */
    uint32 column_end_index;        /* BistAddrConfig -> ColumnEnd */
    uint32 bank_end_index;          /* BistAddrConfig -> BankEnd */

    /* hbm refresh enable */
    uint32 bist_refresh_enable;
    /* hbm ignore address, used for phy loopback or dword loopback */
    uint32 bist_ignore_address;

	/* number of consecutive commands to perform on the same row */
    uint32 same_row_commands;

    /* data mode: 0-prbs, 1-pattern */
    uint32 data_mode;

    /* 8 elements of 256b of data pattern */
    uint32 data_pattern[HBMC_SHMOO_BIST_NOF_PATTERNS][HBMC_SHMOO_BIST_NOF_WORDS_IN_PATTERN];

    /* 16 elements of prbs seeds, each up to 20b */
    uint32 prbs_seeds[HBMC_SHMOO_BIST_NOF_PRBS_DATA_SEEDS];

} hbmc_shmoo_bist_info_t;

typedef struct hbmc_shmoo_dram_info_s
{ 
    uint32 ctl_type;
    uint32 dram_type;
    uint32 dram_bitmap;
    int    num_columns;
    int    num_rows;
    int    num_banks;
    int    data_rate_mbps;
    int    ref_clk_mhz; 
    uint32 refi;
    uint32 command_parity_latency;
    uint32 parity_latency;
    int sim_system_mode;
} hbmc_shmoo_dram_info_t;

typedef enum hbmc_shmoo_hbm_model_part_type_e
{
    HBM_TYPE_INVALID = 0,
    HBM_TYPE_SAMSUNG_DIE_B = 1,
    HBM_TYPE_SAMSUNG_DIE_X = 2,
    HBM_TYPE_NOF = 3
}hbmc_shmoo_hbm_model_part_t;

typedef int (*soc_hbm16_phy_reg_read_t)(int unit, int hbm_ndx, int channel, uint32 addr, uint32 *data);
typedef int (*soc_hbm16_phy_reg_write_t)(int unit, int hbm_ndx, int channel, uint32 addr, uint32 data);
typedef int (*soc_hbm16_phy_reg_modify_t)(int unit, int hbm_ndx, int channel, uint32 addr, uint32 data, uint32 mask);
typedef int (*shmoo_hbm16_hbmc_bist_conf_set_t)(int unit, int flags, int hbm_ndx, int channel, const hbmc_shmoo_bist_info_t *info);
typedef int (*shmoo_hbm16_hbmc_bist_err_cnt_t)(int unit, int flags, int hbm_ndx, int channel, hbmc_shmoo_bist_err_cnt_t *info);
typedef int (*shmoo_hbm16_hbmc_custom_bist_run_t)(int unit, int hbm_ndx, int channel, hbmc_shmoo_bist_info_t *bist_info, hbmc_shmoo_error_array_t *shmoo_error_array);
typedef int (*shmoo_hbm16_hbmc_dram_init_t)(int unit, int hbm_ndx, int phase);
typedef int (*shmoo_hbm16_hbmc_pll_set_t)(int unit, int hbm_ndx, const hbmc_shmoo_pll_t *pll_info); 
typedef int (*shmoo_hbm16_hbmc_modify_mrs_t)(int unit, int hbm_ndx, int channel, int mr_ndx, uint32 data, uint32 mask);
typedef int (*shmoo_hbm16_hbmc_enable_wr_parity_t)(int unit, int hbm_ndx, int channel, int enable);
typedef int (*shmoo_hbm16_hbmc_enable_rd_parity_t)(int unit, int hbm_ndx, int channel, int enable);
typedef int (*shmoo_hbm16_hbmc_enable_addr_parity_t)(int unit, int hbm_ndx, int channel, int enable);
typedef int (*shmoo_hbm16_hbmc_enable_wr_dbi_t)(int unit, int hbm_ndx, int channel, int enable);
typedef int (*shmoo_hbm16_hbmc_enable_rd_dbi_t)(int unit, int hbm_ndx, int channel, int enable);
typedef int (*shmoo_hbm16_hbmc_enable_refresh_t)(int unit, int hbm_ndx , int channel, int enable);
typedef int (*shmoo_hbm16_hbmc_soft_reset_controller_without_dram_t)(int unit, int hbm_ndx, int channel);
typedef int (*shmoo_hbm16_hbmc_dram_info_access_t)(int unit, hbmc_shmoo_dram_info_t* shmoo_info);
typedef int (*shmoo_hbm16_hbmc_bist_status_get_t)(int unit, int hbm_ndx, int channel, uint32 *read_cmd_counter, uint32 *write_cmd_counter, uint32 *read_data_counter);
typedef int (*shmoo_hbm16_hbmc_model_part_get_t)(int unit, int hbm_index, hbmc_shmoo_hbm_model_part_t * model_part);
typedef int (*shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check_t)(int unit, int hbm_index, int channel, int * is_aligned);

typedef struct hbmc_shmoo_cbi_s { 
    /* PHY register Read */
    soc_hbm16_phy_reg_read_t hbmc_phy_reg_read;
    /* PHY register write */
    soc_hbm16_phy_reg_write_t hbmc_phy_reg_write;
    /* PHY register Modify */
    soc_hbm16_phy_reg_modify_t hbmc_phy_reg_modify;
    /* BIST Configuration */
    shmoo_hbm16_hbmc_bist_conf_set_t hbmc_shmoo_bist_conf_set;
    /* Get BIST error count */
    shmoo_hbm16_hbmc_bist_err_cnt_t hbmc_shmoo_bist_err_cnt;
    /* run custom BIST or BIST equivalent */
    shmoo_hbm16_hbmc_custom_bist_run_t hbmc_shmoo_custom_bist_run;
    /* Set Dram Init Phase (First - 1/Second - 2) */
    shmoo_hbm16_hbmc_dram_init_t hbmc_shmoo_dram_init;
    /* Configure DRC PLL */
    shmoo_hbm16_hbmc_pll_set_t hbmc_shmoo_pll_set;
    /* Modify mrs */
    shmoo_hbm16_hbmc_modify_mrs_t hbmc_shmoo_modify_mrs;
    /* Enable Write Parity */
    shmoo_hbm16_hbmc_enable_wr_parity_t hbmc_shmoo_enable_wr_parity;
    /* Enable Read Parity */
    shmoo_hbm16_hbmc_enable_rd_parity_t hbmc_shmoo_enable_rd_parity;
    /* Enable Address/Command Parity  */
    shmoo_hbm16_hbmc_enable_addr_parity_t hbmc_shmoo_enable_addr_parity;
    /* Enable Write DBI */
    shmoo_hbm16_hbmc_enable_wr_dbi_t hbmc_shmoo_enable_wr_dbi;
    /* Enable Read DBI */
    shmoo_hbm16_hbmc_enable_rd_dbi_t hbmc_shmoo_enable_rd_dbi;
    /* Enable refresh */
    shmoo_hbm16_hbmc_enable_refresh_t hbmc_shmoo_enable_refresh;
    /* Soft Reset Drc Without Dram */
    shmoo_hbm16_hbmc_soft_reset_controller_without_dram_t hbmc_shmoo_soft_reset_controller_without_dram;
    /* Access specific unit dram info */
    shmoo_hbm16_hbmc_dram_info_access_t hbmc_shmoo_dram_info_access;
    /** BIST Status get */
    shmoo_hbm16_hbmc_bist_status_get_t hbmc_shmoo_bist_status_get;
    /** Model part type get */
    shmoo_hbm16_hbmc_model_part_get_t hbmc_shmoo_model_part_get;
    /** dword alignment check */
    shmoo_hbm16_hbmc_phy_channel_dwords_alignment_check_t hbmc_shmoo_phy_channel_dwords_alignment_check;
} hbmc_shmoo_cbi_t;

/* interface structure between the Shmoo and hbmc - define the channel parameter that need to restore/save and its level (priority)*/
typedef struct hbmc_shmoo_channel_config_param_s {
  soc_hbm16_channel_reg_t reg;
  int field; 
  int level;
  char * name;
} hbmc_shmoo_channel_config_param_t;

/* interface structure between the Shmoo and hbmc - define the midstack parameter that need to restore/save and its level (priority)*/
typedef struct hbmc_shmoo_midstack_config_param_s {
  soc_hbm16_midstack_reg_t reg;
  int field; 
  int level;  
  char * name;
} hbmc_shmoo_midstack_config_param_t;

/* interface structure between the Shmoo and hbmc - hold the value for one parameter of the shmoo tuning and valid indication */
typedef struct
{
    int valid;
    uint32 value;
}hbmc_shmoo_config_param_data_t;

/* interface structure between the Shmoo and hbmc - hold the values for all channels and midstack tune parameters */
typedef struct hbmc_shmoo_config_param_s
{
   hbmc_shmoo_config_param_data_t channel_data[HBMC_SHMOO_NOF_HBM_CHANNELS][HBMC_SHMOO_CHANNEL_METADATA_SIZE];
   hbmc_shmoo_config_param_data_t midstack_data [HBMC_SHMOO_MIDSTACK_METADATA_SIZE];
} hbmc_shmoo_config_param_t;

#endif /* _HBMC_SHMOO_H */
