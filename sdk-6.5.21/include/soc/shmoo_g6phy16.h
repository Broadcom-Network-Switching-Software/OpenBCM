/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * GDDR6 Memory Support
 */


#ifndef _SHMOO_G6PHY16_H
#define _SHMOO_G6PHY16_H

#define SHMOO_G6PHY16_BIT                           1
#define SHMOO_G6PHY16_BYTE                          (SHMOO_G6PHY16_BIT << 3)
#define SHMOO_G6PHY16_HALFWORD                      (SHMOO_G6PHY16_BYTE << 1)
#define SHMOO_G6PHY16_WORD                          (SHMOO_G6PHY16_HALFWORD << 1)
#define SHMOO_G6PHY16_DOUBLEWORD                    (SHMOO_G6PHY16_WORD << 1)
#define SHMOO_G6PHY16_QUADWORD                      (SHMOO_G6PHY16_DOUBLEWORD << 1)

#define SHMOO_G6PHY16_SHORT_SLEEP                   1
#define SHMOO_G6PHY16_LONG_SLEEP                    10
#define SHMOO_G6PHY16_DEEP_SLEEP                    100
#define SHMOO_G6PHY16_HIBERNATION                   1000
#define SHMOO_G6PHY16_STASIS                        10000

#define SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT          17
#define SHMOO_G6PHY16_MAX_EFFECTIVE_UI_COUNT        16
#define SHMOO_G6PHY16_LAST_EFFECTIVE_UI             (SHMOO_G6PHY16_MAX_EFFECTIVE_UI_COUNT - 1)
#define SHMOO_G6PHY16_MIN_UI_SIZE_PS                3
#define SHMOO_G6PHY16_MIN_DATA_RATE_MBPS            1250
#define SHMOO_G6PHY16_MAX_STORAGE_ELEMENT_COUNT     ((((10000000 * SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT) / SHMOO_G6PHY16_MIN_DATA_RATE_MBPS) / SHMOO_G6PHY16_MIN_UI_SIZE_PS) + 1)
#define SHMOO_G6PHY16_MAX_VDL_LENGTH                (128 << 2)
#define SHMOO_G6PHY16_MIN_VDL_LENGTH                (32 << 2)
#define SHMOO_G6PHY16_MAX_VREF_RANGE                128

#define SHMOO_G6PHY16_RESULT2D_X                    SHMOO_G6PHY16_MAX_STORAGE_ELEMENT_COUNT
#define SHMOO_G6PHY16_RESULT2D_Y                    SHMOO_G6PHY16_MAX_VREF_RANGE

#define SHMOO_G6PHY16_STRING_LENGTH                 (SHMOO_G6PHY16_RESULT2D_X + 1)
#define SHMOO_G6PHY16_ERROR_ARRAY_LENGTH            4

#define SHMOO_G6PHY16_CTL_TYPE_RSVP                 0
#define SHMOO_G6PHY16_CTL_TYPE_RSVP_STR             "t0"
#define SHMOO_G6PHY16_CTL_TYPE_1                    1
#define SHMOO_G6PHY16_CTL_TYPE_1_STR                "t1"

#define SHMOO_G6PHY16_DRAM_TYPE_RSVP                0
#define SHMOO_G6PHY16_DRAM_TYPE_GDDR6               5

#define SHMOO_G6PHY16_CALIB_CENTER_PASS             111
#define SHMOO_G6PHY16_CALIB_RISING_EDGE             011
#define SHMOO_G6PHY16_CALIB_FALLING_EDGE            100
#define SHMOO_G6PHY16_CALIB_ANY_EDGE                101
#define SHMOO_G6PHY16_CALIB_PASS_START              001
#define SHMOO_G6PHY16_CALIB_FAIL_START              110
#define SHMOO_G6PHY16_CALIB_XY_AVERAGE              010

#define SHMOO_G6PHY16_CALIB_SWEEP_1                 1
#define SHMOO_G6PHY16_CALIB_SWEEP_BYTE_TO_BIT       2
#define SHMOO_G6PHY16_CALIB_SWEEP_HALFWORD_TO_BIT   3
#define SHMOO_G6PHY16_CALIB_SWEEP_WORD_TO_BIT       4

#define SHMOO_G6PHY16_CALIB_GROUP_ORDER_DONT_CARE   0x00000000
#define SHMOO_G6PHY16_CALIB_GROUP_ORDER_00112233    0x00112233
#define SHMOO_G6PHY16_CALIB_GROUP_ORDER_01230123    0x01230123

#define SHMOO_G6PHY16_CALIB_PASS_ANY                0x00000001
#define SHMOO_G6PHY16_CALIB_PASS_ALL                0xFFFFFFFF

typedef enum shmoo_g6phy16_shmoo_type_e
{
    SHMOO_G6PHY16_FIRST = 0,

    /** Do all SHMOO types according to chosen action */
    SHMOO_G6PHY16_SHMOO_ALL = SHMOO_G6PHY16_FIRST,

    /** This SHMOO type is used to get the Address line VDL to the correct place */
    SHMOO_G6PHY16_ADDR_CTRL_SHORT,

    /** This SHMOO type is used to align the command clock(CK) to the data clock(WCK) */
    SHMOO_G6PHY16_WCK2CK,

    /** WR2RD is a FIFO that sits PHY.
     * The DRAM is writing to this FIFO and the controller is reading from it.
     * when the controller issues a read command form the FIFO it gets the data and a data valid indications, those need to be aligned.
     * This SHMOO type searches the Data_valid WR2RD_DELAY, meaning the delay to add to the data_valid to align to the data. */
    SHMOO_G6PHY16_RD_START_FISH,

    /** same as for the SHMOO_G6PHY16_RD_START_FISH, only for EDC_valid instead of data valid.
     * for each read command an EDC is retrieved as well, this has additional delay because the EDC has to be calculated. */
    SHMOO_G6PHY16_EDC_START_FISH,

    /** This SHMOO type de-skews the different bits in each byte on the read direction in the data lines */
    /** This is done to in order to later shift the whole byte together and not individual bits */
    SHMOO_G6PHY16_RD_DESKEW,

    /** This SHMOO type is used to get the find the approximated VDL, to be later refined in the extended SHMOO type */
    SHMOO_G6PHY16_RD_SHORT,

    /** This SHMOO type de-skews the different bits in each byte on the read direction in the DBI and EDC lines */
    /** This is done to in order to later shift the whole byte together and not individual bits */
    SHMOO_G6PHY16_DBI_EDC_RD_DESKEW,

    /** This SHMOO type de-skews the different bits in each byte on the write direction in the data lines */
    /** This is done to in order to later shift the whole byte together and not individual bits */
    SHMOO_G6PHY16_WR_DESKEW,

    /** This SHMOO type is used to get the find the approximated VDL delay, to be later refined in the extended SHMOO type */
    SHMOO_G6PHY16_WR_SHORT,

    /** This SHMOO type de-skews the different bits in each byte on the write direction in the DBI lines */
    /** This is done to in order to later shift the whole byte together and not individual bits */
    SHMOO_G6PHY16_DBI_WR_DESKEW,

    /** This SHMOO type is used to get both the VREF and the VDL to the exact point */
    SHMOO_G6PHY16_RD_EXTENDED,

    /** This SHMOO type is used to get both the VREF and the VDL to the exact point */
    SHMOO_G6PHY16_WR_EXTENDED,

    /** This SHMOO type is used to enable the CDR */
    SHMOO_G6PHY16_CDR,

    SHMOO_G6PHY16_NOF
} shmoo_g6phy16_shmoo_type_t;

/** Vendor code per manufacturer as defined in JEDEC */
typedef enum shmoo_g6phy16_dram_vendor_type_e
{
    SHMOO_G6PHY16_VENDOR_SAMSUNG = 0x1,
    SHMOO_G6PHY16_VENDOR_HYNIX = 0x6,
    SHMOO_G6PHY16_VENDOR_MICRON = 0xF
} shmoo_g6phy16_dram_vendor_type_t;


#define SHMOO_G6PHY16_REPEAT                        9
#define SHMOO_G6PHY16_REPEAT_HALF                   (SHMOO_G6PHY16_REPEAT >> 1)

#define SHMOO_G6PHY16_ACTION_RSVP                   -1
#define SHMOO_G6PHY16_ACTION_RESTORE                0
#define SHMOO_G6PHY16_ACTION_RUN                    1
#define SHMOO_G6PHY16_ACTION_SAVE                   2
#define SHMOO_G6PHY16_ACTION_RUN_AND_SAVE           3

#define SHMOO_G6PHY16_CTL_FLAGS_STAT_BIT            0x1
#define SHMOO_G6PHY16_CTL_FLAGS_PLOT_BIT            0x2
#define SHMOO_G6PHY16_CTL_FLAGS_EXT_VREF_RANGE_BIT  0x4
#define SHMOO_G6PHY16_DEBUG_MODE_FLAG_BITS          SHMOO_G6PHY16_CTL_FLAGS_EXT_VREF_RANGE_BIT

#define SHMOO_G6PHY16_MAX_INTERFACES                8
#define SHMOO_G6PHY16_INTERFACE_ALL                -1
#define SHMOO_G6PHY16_BYTES_PER_INTERFACE           4
#define SHMOO_G6PHY16_DQ_BIT_NOT_SWAPPED            0xFF 

#define SHMOO_G6PHY16_BIST_NOF_PATTERNS             8
#define SHMOO_G6PHY16_BIST_MPR_NOF_PATTERNS         4

#define SHMOO_G6PHY16_REFRESH_SHUTDOWN_REFI         0

#define SHMOO_G6PHY16_MR_CMD_ALL_CHANNELS           -1

/* Initial UI Shifts */
#define SHMOO_G6PHY16_GDDR6_INIT_CK_UI_SHIFT        2
#define SHMOO_G6PHY16_GDDR6_INIT_ADDR_CTRL_UI_SHIFT 3
#define SHMOO_G6PHY16_GDDR6_INIT_DQS_UI_SHIFT       1
#define SHMOO_G6PHY16_GDDR6_INIT_DATA_UI_SHIFT      0

/* Initial VDL Positions */

/* Initial Configurations */
#define SHMOO_G6PHY16_GDDR6_INIT_REN_RD2_2G_DELAY   4
#define SHMOO_G6PHY16_GDDR6_INIT_EDCEN_RD2_2G_DELAY 9

/* CDR */
#define SHMOO_G6PHY16_CDR_UNDEFINED_VALUE           0x80000000

/* Config params */
#define SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE            SHMOO_G6PHY16_BYTES_PER_INTERFACE

typedef struct g6phy16_shmoo_dram_info_s
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
    int sim_system_mode;
    uint32 ref_clk_bitmap;
    uint8  zq_cal_array[SHMOO_G6PHY16_MAX_INTERFACES];
    uint32 dq_swap[SHMOO_G6PHY16_MAX_INTERFACES][SHMOO_G6PHY16_BYTES_PER_INTERFACE][SHMOO_G6PHY16_BYTE];
} g6phy16_shmoo_dram_info_t;

typedef struct g6phy16_shmoo_config_param_s
{
    uint32   dq_byte_wr_min_vdl_bit[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE][SHMOO_G6PHY16_BYTE];
    uint32   dq_byte_wr_min_vdl_dbi[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32   dq_byte_wr_min_vdl_edc[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_wr_max_vdl_data[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_wr_max_vdl_dqs[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];

    uint32   dq_byte_rd_min_vdl_bit[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE][SHMOO_G6PHY16_BYTE];
    uint32   dq_byte_rd_min_vdl_dbi[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32   dq_byte_rd_min_vdl_edc[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_rd_max_vdl_dqsp[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_rd_max_vdl_dqsn[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];

    uint32  dq_byte_ren_fifo_config[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_edcen_fifo_config[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];    
    uint32  dq_byte_rd_max_vdl_fsm[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_vref_dac_config[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_macro_reserved_reg[SHMOO_G6PHY16_DQ_CONFIG_NOF_BYTE];
   
    uint32  aq_l_max_vdl_addr;
    uint32  aq_l_max_vdl_ctrl;
    uint32  aq_l_macro_reserved_reg;
    uint32  aq_u_max_vdl_addr;
    uint32  aq_u_max_vdl_ctrl;
    uint32  aq_u_macro_reserved_reg;
    
    uint32  common_macro_reserved_reg;
    
    uint32  control_regs_read_clock_config;
    uint32  control_regs_input_shift_ctrl;
    uint32  control_regs_ren_fifo_central_init;
    uint32  control_regs_edcen_fifo_central_init;
    uint32  control_regs_shared_vref_dac_config;
    uint32  control_regs_reserved_reg;
    
} g6phy16_shmoo_config_param_t;


typedef struct g6phy16_bist_info_s
{ 
    uint32 write_weight;
    uint32 read_weight;
    uint32 bist_timer_us;        /* if bist_timer_us != 0, ignore bist_num_actions and wait bist_timer_us before stoping bist */
    uint32 bist_num_actions;
    uint32 prbs_mode;
    uint32 mpr_mode;
    uint32 data_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS][8];
    uint8  mpr_pattern[SHMOO_G6PHY16_BIST_MPR_NOF_PATTERNS];
} g6phy16_bist_info_t;

typedef struct g6phy16_bist_err_cnt_s
{
    uint32 bist_err_bmp;
    uint32 bist_err_cnt;
    uint32 bist_dbi_err_bmp;
    uint32 bist_dbi_err_cnt;
    uint32 bist_edc_err_bmp;
    uint32 bist_edc_err_cnt;
} g6phy16_bist_err_cnt_t;

/** Dram Init phase */
typedef enum shmoo_gddr6_dram_init_phase_e
{
    SHMOO_GDDR6_DRAM_INIT_PHASE_FIRST = 0,
    SHMOO_GDDR6_DRAM_INIT_UNTIL_CADT = SHMOO_GDDR6_DRAM_INIT_PHASE_FIRST,
    SHMOO_GDDR6_DRAM_INIT_AFTER_CADT = 1,
    SHMOO_GDDR6_DRAM_INIT_NOF = 2,
    SHMOO_GDDR6_DRAM_INIT_ALL = SHMOO_GDDR6_DRAM_INIT_NOF
} gddr6_dram_init_phase_t;

/** Training BIST modes*/
typedef enum shmoo_g6phy16_training_bist_mode_e
{
    SHMOO_G6PHY16_TRAINING_BIST_LOAD_READ_FIFO = 0,
    SHMOO_G6PHY16_TRAINING_BIST_READ_FROM_LDFF = 1,
    SHMOO_G6PHY16_TRAINING_BIST_WRITE_THEN_READ_FROM_LDFF = 2,
    SHMOO_G6PHY16_TRAINING_BIST_ADDR_TRAINING = 3,
    SHMOO_G6PHY16_TRAINING_BIST_READ_VENDOR_ID = 4,
    SHMOO_G6PHY16_TRAINING_BIST_NOF
} shmoo_g6phy16_training_bist_mode_t;

typedef enum shmoo_g6phy16_cadt_mode_e
{
    OFF = 0,
    RISING_EDGE = 1,
    FALLING_EDGE = 2,
    CABI_CA10 = 3
} shmoo_g6phy16_cadt_mode_t;

/** Training BIST info */
typedef struct g6phy16_training_bist_info_s
{
    /** general parameters */
    shmoo_g6phy16_training_bist_mode_t training_bist_mode;
    uint32 nof_commands;

    /** parameters for cadt */
    uint32 cadt_to_cadt_prd;
    uint32 cadt_to_rden_prd;
    uint32 cadt_invert_caui2;
    shmoo_g6phy16_cadt_mode_t cadt_mode;
    uint32 cadt_prbs_enabled;
    uint32 cadt_seed;
    uint32 cadt_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];
    /* uint32 nof_commands; */ /* commented out because it is already mentioned above in a different line */
    /* shmoo_g6phy16_training_bist_mode_t bist_mode; */ /* commented out because it is already mentioned above in a different line */

    /** load read FIFO using address line */
    uint32 ldff_data_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS][4];      /* used for LDFF and read  (8 * 16) */
    uint32 ldff_dbi_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];          /* used for LDFF and read */
    uint32 ldff_edc_pattern[SHMOO_G6PHY16_BIST_NOF_PATTERNS];          /* used for LDFF and read */
    uint32 fifo_depth; /* has a constraint, has to be exactly fifo_depth */
    /* shmoo_g6phy16_training_bist_mode_t bist_mode; */ /* commented out because it is already mentioned above in a different line */

    /* read from loaded FIFO */
    /* ldff_data_pattern has to be the same as when it was loaded to the FIFO */ /* commented out because it is already mentioned above in a different line */
    /* ldff_dbi_pattern has to be the same as when it was loaded to the FIFO */ /* commented out because it is already mentioned above in a different line */
    /* ldff_edc_pattern has to be the same as when it was loaded to the FIFO */ /* commented out because it is already mentioned above in a different line */
    /* nof_commands has a constraint, has to be in fifo_depth granularity */ /* commented out because it is already mentioned above in a different line */
    /* shmoo_g6phy16_training_bist_mode_t bist_mode; */ /* commented out because it is already mentioned above in a different line */


    /* write to FIFO then read from FIFO */
    uint32 data_mode; /* determine which bits in the data are PRBS or pattern */
    uint32 dbi_mode;  /* determine which bits in the DBI are PRBS or pattern */
    uint32 bist_data_pattern[8][8];
    uint32 bist_dbi_pattern[8];
    uint32 prbs_data_seed[8];
    uint32 prbs_dbi_seed;
    /* nof_commands has a constraint, has to be in (fifo_depth * 2) granularity */ /* commented out because it is already mentioned above in a different line */
    uint32 read_weight; /* has to be equal to FIFO depth */
    uint32 write_weight; /* has to be equal to FIFO depth */
    /* shmoo_g6phy16_training_bist_mode_t bist_mode; */ /* commented out because it is already mentioned above in a different line */
} g6phy16_training_bist_info_t;

/** Training BIST error structure */
typedef struct g6phy16_training_bist_err_cnt_s {
    uint32 cadt_err_cnt;
    uint32 cadt_err_bitmap[1];
    uint32 bist_data_err_cnt;
    uint32 bist_data_err_bitmap[1];
    uint32 bist_edc_err_cnt;
    uint32 bist_edc_err_bitmap[1];
    uint32 bist_dbi_err_cnt;
    uint32 bist_dbi_err_bitmap[1];
    uint32 bist_read_command_counter;
    uint32 bist_write_command_counter;
    uint32 bist_data_counter;
    uint32 bist_edc_counter;
    /** per byte logic to verify that number of bist commands made sense */
    uint32 bist_sanity_err_bitmap[1];
} g6phy16_training_bist_err_cnt_t;


typedef struct g6phy16_drc_pll_s
{  
    uint32 iso_in;
    uint32 ldo_ctrl;
    uint32 ndiv_frac;
    uint32 pdiv;
    uint32 ndiv_int;
    uint32 ssc_limit;
    uint32 ssc_mode;
    uint32 ssc_step;
    uint32 pll_ctrl;
    uint32 vco_sel;
    uint32 mdiv;
    uint32 fref_eff_info;
    uint32 en_ctrl;
} g6phy16_drc_pll_t;

typedef struct g6phy16_vendor_info_s { 
    uint32 dram_density;
    uint32 fifo_depth;
    uint32 revision_id;
    uint32 manufacture_id;
} g6phy16_vendor_info_t;

typedef struct g6phy16_shmoo_cbi_s { 
    /* PHY register Read */
    int (*g6phy16_phy_reg_read)(int unit, int drc_ndx, uint32 addr, uint32 *data);
    /* PHY register write */
    int (*g6phy16_phy_reg_write)(int unit, int drc_ndx, uint32 addr, uint32 data);
    /* PHY register Modify */
    int (*g6phy16_phy_reg_modify)(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask);
    /* BIST Configuration */
    int (*g6phy16_drc_bist_conf_set)(int unit, int drc_ndx, const g6phy16_bist_info_t *info);
    /* BIST Run */
    int (*g6phy16_drc_bist_run)(int unit, int drc_ndx, const g6phy16_bist_info_t *info);
    /* Get BIST error count */
    int (*g6phy16_drc_bist_err_cnt)(int unit, int drc_ndx, g6phy16_bist_err_cnt_t *info);    
    /* Set Dram Init Phase (details in enum) */
    int (*g6phy16_drc_dram_init)(int unit, int drc_ndx, gddr6_dram_init_phase_t phase);
    /* Configure DRC PLL */
    int (*g6phy16_drc_pll_set)(int unit, int drc_ndx, const g6phy16_drc_pll_t *pll_info); 
    /* Modify mrs */    
    int (*g6phy16_drc_modify_mrs)(int unit, int drc_ndx, int channel_index, uint32 mr_ndx, uint32 data, uint32 mask);
    /* Enable/Disable Wck2CK training */
    int (*g6phy16_drc_enable_wck2ck_training)(int unit, int drc_ndx, int enable);
    /* Enable/Disable write leveling */
    int (*g6phy16_drc_enable_write_leveling)(int unit, int drc_ndx, uint32 command_parity_lattency, int use_continious_gddr6_dqs, int enable);
    /* Enable/Disable MPR */
    int (*g6phy16_drc_mpr_en)(int unit, int drc_ndx, int enable);    
    /* Load MPR pattern */
    int (*g6phy16_drc_mpr_load)(int unit, int drc_ndx, uint8 *mpr_pattern);
    /* Get vendor ID info */
    int (*g6phy16_drc_vendor_info_get)(int unit, int drc_ndx, g6phy16_vendor_info_t *info);    
    /* Generate DQs Pulse */
    int (*g6phy16_drc_dqs_pulse_gen)(int unit, int drc_ndx, int use_continious_gddr6_dqs);
    /* GDDR6 BIST Configuration */
    int (*g6phy16_training_bist_conf_set)(int unit, int drc_ndx, g6phy16_training_bist_info_t *info);
    /* GDDR6 Get BIST error count */
    int (*g6phy16_training_bist_err_cnt)(int unit, int drc_ndx, g6phy16_training_bist_err_cnt_t *info);
    /* Get DQ Byte Swap Info */
    int (*g6phy16_gddr6_shmoo_drc_dq_byte_pairs_swap_info_get)(int unit, int drc_ndx, int* pairs_were_swapped);
    /* Enable CRC write */
    int (*g6phy16_drc_enable_wr_crc)(int unit, int drc_ndx, int enable);
    /* Enable CRC read */
    int (*g6phy16_drc_enable_rd_crc)(int unit, int drc_ndx, int enable);
    /* Enable DBI write */
    int (*g6phy16_drc_enable_wr_dbi)(int unit, int drc_ndx, int enable);
    /* Enable DBI read */
    int (*g6phy16_drc_enable_rd_dbi)(int unit, int drc_ndx, int enable);
    /* Force DQs */
    int (*g6phy16_drc_force_dqs)(int unit, int drc_ndx , uint32 force_dqs);
    /* Soft Reset Drc Without Dram */
    int (*g6phy16_drc_soft_reset_drc_without_dram)(int unit, int drc_ndx);
    /* Access specific unit dram info */
    int (*g6phy16_shmoo_dram_info_access)(int unit, g6phy16_shmoo_dram_info_t* shmoo_info);
} g6phy16_shmoo_cbi_t;

typedef struct g6phy16_shmoo_container_s
{
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 result2D[(SHMOO_G6PHY16_RESULT2D_X)*(SHMOO_G6PHY16_RESULT2D_Y)];
    uint32 resultData[SHMOO_G6PHY16_WORD];
    shmoo_g6phy16_shmoo_type_t shmooType;
    uint32 dramType;
    uint32 ctlType;
    uint32 trefi;
    uint32 trefi_ab;
    uint32 step1000;
    uint32 size1000UI;
    uint32 endUI[SHMOO_G6PHY16_MAX_VISIBLE_UI_COUNT];
    uint32 engageUIshift;
    uint32 wckInv;
    uint32 restore;
    uint32 debugMode;
} g6phy16_shmoo_container_t;

typedef struct g6phy16_step_size_s
{
    uint32 step1000;
    uint32 size1000UI;
} g6phy16_step_size_t;

typedef struct g6phy16_ui_position_s
{
    uint32 ui;
    uint32 position;
} g6phy16_ui_position_t;

typedef uint32 g6phy16_shmoo_error_array_t[SHMOO_G6PHY16_ERROR_ARRAY_LENGTH];

typedef struct g6phy16_cdr_config_param_s
{
    int byte_map;
    int reset_n;
    int enable;
    int ep;
    int dp;
    int en;
    int dn;
    int init_track_transition;
    int accu_pos_threshold;
    int update_gap;
    int update_mode;
    int auto_copy;
} g6phy16_cdr_config_param_t;

typedef int (*soc_g6phy16_phy_reg_read_t)(int unit, int drc_ndx, uint32 addr, uint32 *data);
typedef int (*soc_g6phy16_phy_reg_write_t)(int unit, int drc_ndx, uint32 addr, uint32 data);
typedef int (*soc_g6phy16_phy_reg_modify_t)(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask);
typedef int (*_shmoo_g6phy16_drc_bist_conf_set_t)(int unit, int drc_ndx, const g6phy16_bist_info_t *info);
typedef int (*_shmoo_g6phy16_drc_bist_run_t)(int unit, int drc_ndx, const g6phy16_bist_info_t *info);
typedef int (*_shmoo_g6phy16_drc_bist_err_cnt_t)(int unit, int drc_ndx, g6phy16_bist_err_cnt_t *info);
typedef int (*_shmoo_g6phy16_drc_dram_init_t)(int unit, int drc_ndx, gddr6_dram_init_phase_t phase);
typedef int (*_shmoo_g6phy16_drc_pll_set_t)(int unit, int drc_ndx, const g6phy16_drc_pll_t *pll_info); 
typedef int (*_shmoo_g6phy16_drc_modify_mrs_t)(int unit, int drc_ndx, int channel_index, uint32 mr_ndx, uint32 data, uint32 mask);
typedef int (*_shmoo_g6phy16_drc_enable_wck2ck_training_t)(int unit, int drc_ndx, int enable);
typedef int (*_shmoo_g6phy16_drc_enable_write_leveling_t)(int unit, int drc_ndx, uint32 command_parity_lattency, int use_continious_gddr6_dqs, int enable);
typedef int (*_shmoo_g6phy16_drc_mpr_en_t)(int unit, int drc_ndx, int enable);
typedef int (*_shmoo_g6phy16_drc_mpr_load_t)(int unit, int drc_ndx, uint8 *mpr_pattern);
typedef int (*_shmoo_g6phy16_drc_vendor_info_get_t)(int unit, int drc_ndx, g6phy16_vendor_info_t *info);
typedef int (*_shmoo_g6phy16_drc_dqs_pulse_gen_t)(int unit, int drc_ndx, int use_continious_gddr6_dqs);
typedef int (*_shmoo_g6phy16_training_bist_conf_set_t)(int unit, int drc_ndx, g6phy16_training_bist_info_t *info);
typedef int (*_shmoo_g6phy16_drc_gddr6_bist_err_cnt_t)(int unit, int drc_ndx, g6phy16_training_bist_err_cnt_t *info);
typedef int (*_shmoo_g6phy16_drc_gddr6_dq_byte_pairs_swap_info_get_t)(int unit, int drc_ndx, int* pairs_were_swapped);
typedef int (*_shmoo_g6phy16_drc_enable_wr_crc_t)(int unit, int drc_ndx, int enable);
typedef int (*_shmoo_g6phy16_drc_enable_rd_crc_t)(int unit, int drc_ndx, int enable);
typedef int (*_shmoo_g6phy16_drc_enable_wr_dbi_t)(int unit, int drc_ndx, int enable);
typedef int (*_shmoo_g6phy16_drc_enable_rd_dbi_t)(int unit, int drc_ndx, int enable);
typedef int (*_shmoo_g6phy16_drc_force_dqs_t)(int unit, int drc_ndx , uint32 force_dqs);
typedef int (*_shmoo_g6phy16_drc_soft_reset_drc_without_dram_t)(int unit, int drc_ndx);
typedef int (*_shmoo_g6phy16_drc_dram_info_access_t)(int unit, g6phy16_shmoo_dram_info_t* shmoo_info);

extern soc_g6phy16_phy_reg_read_t soc_g6phy16_phy_reg_read;
extern soc_g6phy16_phy_reg_write_t soc_g6phy16_phy_reg_write;
extern soc_g6phy16_phy_reg_modify_t soc_g6phy16_phy_reg_modify;
extern int soc_g6phy16_shmoo_interface_cb_register(int unit, g6phy16_shmoo_cbi_t shmoo_cbi);
extern int soc_g6phy16_calculate_step_size(int unit, int drc_ndx, g6phy16_step_size_t *ssPtr);
extern int soc_g6phy16_shmoo_dram_info_set(int unit, CONST g6phy16_shmoo_dram_info_t *sdi);
extern int soc_g6phy16_shmoo_phy_cfg_pll(int unit, int drc_ndx);
extern int soc_g6phy16_shmoo_phy_init(int unit, int drc_ndx);
extern int soc_g6phy16_shmoo_ctl(int unit, int drc_ndx, shmoo_g6phy16_shmoo_type_t shmoo_type, uint32 flags, int action, g6phy16_shmoo_config_param_t *config_param);
extern int soc_g6phy16_cdr_ctl(int unit, int drc_ndx, int stat, g6phy16_cdr_config_param_t *config_param);

int shmoo_g6phy16_restore(int unit, int drc_ndx, g6phy16_shmoo_config_param_t *config_param);

int soc_g6phy16_cdr_enable(
    int unit,
    int dram_index);

int
soc_g6phy16_cdr_disable(
    int unit,
    int dram_index);

int
soc_g6phy16_cdr_is_enabled(
    int unit,
    int dram_index,
    int * cdr_is_enabled);

int
soc_g6phy16_cdr_status(
    int unit,
    int dram_index);

#endif /* _SHMOO_G6PHY16_H */
