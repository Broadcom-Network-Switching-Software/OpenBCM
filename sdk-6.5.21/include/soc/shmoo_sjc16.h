/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * 16nm PHY Support
 */

#ifndef _SHMOO_SJC16_H
#define _SHMOO_SJC16_H

#include <sal/types.h>
#include <soc/defs.h>
#include <shared/types.h>

#define SHMOO_SJC16_CTL_TYPE_RSVP                       0
#define SHMOO_SJC16_CTL_TYPE_RSVP_STR                   "t0"
#define SHMOO_SJC16_CTL_TYPE_1                          1
#define SHMOO_SJC16_CTL_TYPE_1_STR                      "t1"

#define SHMOO_SJC16_DRAM_TYPE_RSVP                      0
#define SHMOO_SJC16_DRAM_TYPE_DDR4                      4

#define SHMOO_SJC16_PHY_REG_BASE_RSVP                   0
#define SHMOO_SJC16_PHY_REG_BASE_HELIX5                 0x00000000

#define SHMOO_SJC16_YDC_DDR_BIST_BIST_REG_BASE_RSVP     0
#define SHMOO_SJC16_YDC_DDR_BIST_BIST_REG_BASE_HELIX5   0x1023BC00

#define SHMOO_SJC16_CHIP_ID_RSVP                        0
#define SHMOO_SJC16_CHIP_ID_HELIX5                      0x85

#define CONFIG_HELIX5               1

#ifndef BCM_SJC16_SUPPORT
#define BCM_SJC16_SUPPORT
#endif

#if defined(CONFIG_HELIX5)
#define SOC_DDR4_SUPPORT
#define CHIP_IS_HELIX5              TRUE
#else
#define CHIP_IS_HELIX5              FALSE
#endif

#if(CHIP_IS_HELIX5)
    #define SHMOO_SJC16_CHIP_ID                     SHMOO_SJC16_CHIP_ID_HELIX5
    #define SHMOO_SJC16_PHY_REG_BASE                SHMOO_SJC16_PHY_REG_BASE_HELIX5
    #define SHMOO_SJC16_YDC_DDR_BIST_BIST_REG_BASE  SHMOO_SJC16_YDC_DDR_BIST_BIST_REG_BASE_HELIX5
    #define SHMOO_SJC16_PHY_BITWIDTH                32
    #define SHMOO_SJC16_PHY_CONSTANT_CONFIG         0
#endif

#define SHMOO_SJC16_PHY_BITWIDTH_IS_32              (SHMOO_SJC16_PHY_BITWIDTH >> 5)
#define YDC_DDR_BIST_BIST_PHY_BITWIDTH_IS_32        SHMOO_SJC16_PHY_BITWIDTH_IS_32
#define YDC_DDR_BIST_BIST_REG_BASE                  SHMOO_SJC16_YDC_DDR_BIST_BIST_REG_BASE

#define SHMOO_SJC16_BIT                             1
#define SHMOO_SJC16_BYTE                            (SHMOO_SJC16_BIT << 3)
#define SHMOO_SJC16_HALFWORD                        (SHMOO_SJC16_BYTE << 1)
#define SHMOO_SJC16_WORD                            (SHMOO_SJC16_HALFWORD << 1)
#define SHMOO_SJC16_DOUBLEWORD                      (SHMOO_SJC16_WORD << 1)
#define SHMOO_SJC16_QUADWORD                        (SHMOO_SJC16_DOUBLEWORD << 1)

#define SHMOO_SJC16_SHORT_SLEEP                     10
#define SHMOO_SJC16_LONG_SLEEP                      50
#define SHMOO_SJC16_DEEP_SLEEP                      100
#define SHMOO_SJC16_HIBERNATION                     1000
#define SHMOO_SJC16_STASIS                          1000000

#define SHMOO_SJC16_SHMOO_RSVP                      -1
#define SHMOO_SJC16_RD_EN_FISH                      24
#define SHMOO_SJC16_RD_EXTENDED                     50
#define SHMOO_SJC16_WR_EXTENDED                     60
#define SHMOO_SJC16_ADDR_CTRL_EXTENDED              70
#define SHMOO_SJC16_ADDR_EXTENDED                   71
#define SHMOO_SJC16_CTRL_EXTENDED                   72
#define SHMOO_SJC16_RD_EN_FISH_ECC                  29
#define SHMOO_SJC16_RD_EXTENDED_ECC                 59
#define SHMOO_SJC16_WR_EXTENDED_ECC                 69
#define SHMOO_SJC16_DDR4_SEQUENCE_COUNT             4
#define SHMOO_SJC16_DDR4_ECC_32P4_SEQUENCE_COUNT    7
/* BEGIN: EXTRA */
/* END: EXTRA */

#define SHMOO_SJC16_QUICK_SHMOO_CTRL_EXTENDED       1

#define SHMOO_SJC16_REPEAT                          9
#define SHMOO_SJC16_REPEAT_HALF                     (SHMOO_SJC16_REPEAT >> 1)

#define SHMOO_SJC16_SEQUENTIAL                      0
#define SHMOO_SJC16_SINGLE                          1

#define SHMOO_SJC16_ACTION_RSVP                     -1
#define SHMOO_SJC16_ACTION_RESTORE                  0
#define SHMOO_SJC16_ACTION_RUN                      1
#define SHMOO_SJC16_ACTION_SAVE                     2
#define SHMOO_SJC16_ACTION_RUN_AND_SAVE             3

#define SHMOO_SJC16_CTL_FLAGS_STAT_BIT              0x1
#define SHMOO_SJC16_CTL_FLAGS_PLOT_BIT              0x2
#define SHMOO_SJC16_CTL_FLAGS_EXT_VREF_RANGE_BIT    0x4
#define SHMOO_SJC16_DEBUG_MODE_FLAG_BITS            SHMOO_SJC16_CTL_FLAGS_EXT_VREF_RANGE_BIT

#define SHMOO_SJC16_MAX_INTERFACES                  8
#define SHMOO_SJC16_INTERFACE_RSVP                  -1
#ifdef CONFIG_HELIX5
    #define SHMOO_SJC16_BYTES_PER_INTERFACE         5
#else
    #define SHMOO_SJC16_BYTES_PER_INTERFACE         4
#endif

#define SHMOO_SJC16_BIST_NOF_PATTERNS               8
#define SHMOO_SJC16_BIST_MPR_NOF_PATTERNS           4

/* Initial UI Shifts */
#define SHMOO_SJC16_DDR4_INIT_CK_UI_SHIFT           2
#define SHMOO_SJC16_DDR4_INIT_ADDR_CTRL_UI_SHIFT    1
#define SHMOO_SJC16_DDR4_INIT_DQS_UI_SHIFT          2
#define SHMOO_SJC16_DDR4_INIT_DATA_UI_SHIFT         1
#define SHMOO_SJC16_GDDR5_INIT_CK_UI_SHIFT          1
#define SHMOO_SJC16_GDDR5_INIT_ADDR_CTRL_UI_SHIFT   3
#define SHMOO_SJC16_GDDR5_INIT_DQS_UI_SHIFT         1
#define SHMOO_SJC16_GDDR5_INIT_DATA_UI_SHIFT        0

/* Initial VDL Positions */
#define SHMOO_SJC16_DDR4_INIT_WRITE_MIN_VDL_POS     0
#define SHMOO_SJC16_DDR4_INIT_READ_MIN_VDL_POS      30

/* Initial Configurations */
#define SHMOO_SJC16_DDR4_INIT_REN_RD2_2G_DELAY      4
#define SHMOO_SJC16_DDR4_INIT_EDCEN_RD2_2G_DELAY    4
#define SHMOO_SJC16_GDDR5_INIT_REN_RD2_2G_DELAY     4
#define SHMOO_SJC16_GDDR5_INIT_EDCEN_RD2_2G_DELAY   9

#define SHMOO_SJC16_MAX_VISIBLE_UI_COUNT            9
#define SHMOO_SJC16_MAX_EFFECTIVE_UI_COUNT          8
#define SHMOO_SJC16_LAST_EFFECTIVE_UI               (SHMOO_SJC16_MAX_EFFECTIVE_UI_COUNT - 1)
#define SHMOO_SJC16_MAX_VDL_LENGTH                  512
#define SHMOO_SJC16_MAX_VREF_RANGE                  32
#define SHMOO_SJC16_DDR4_VREF_RANGE_CROSSOVER       36      /* Must be between 23 - 50 */
#define SHMOO_SJC16_MAX_ZQ_CAL_RANGE                32

#define SHMOO_SJC16_RESULT2D_X                      SHMOO_SJC16_MAX_VDL_LENGTH
#define SHMOO_SJC16_RESULT2D_Y                      SHMOO_SJC16_MAX_VREF_RANGE

#define SHMOO_SJC16_STRING_LENGTH                   ((SHMOO_SJC16_RESULT2D_X << 1) + 1)
#define SHMOO_SJC16_ERROR_ARRAY_LENGTH              4

#define SHMOO_SJC16_CALIB_CENTER_PASS               111
#define SHMOO_SJC16_CALIB_RISING_EDGE               011
#define SHMOO_SJC16_CALIB_FALLING_EDGE              100
#define SHMOO_SJC16_CALIB_PASS_START                001
#define SHMOO_SJC16_CALIB_FAIL_START                110
#define SHMOO_SJC16_CALIB_VDL_ZERO                  000
#define SHMOO_SJC16_CALIB_XY_AVERAGE                010

#define SHMOO_SJC16_PHY_NOF_AD                      16
#define SHMOO_SJC16_PHY_NOF_BA                      3
#define SHMOO_SJC16_PHY_NOF_AUX                     3
#define SHMOO_SJC16_PHY_NOF_CS                      2

/* Config params */
#define SHMOO_SJC16_DQ_CONFIG_NOF_BYTE              SHMOO_SJC16_BYTES_PER_INTERFACE

typedef struct sjc16_shmoo_dram_info_s
{ 
    uint32 ctl_type;
    uint32 dram_type;
    uint32 dram_bitmap;
    uint32 interface_bitwidth;
    uint32 ecc_32p4;
    int    num_columns;
    int    num_rows;
    int    num_banks;
    int    num_bank_groups;
    int    data_rate_mbps;
    int    ref_clk_mhz; 
    uint32 refi;
    uint32 command_parity_latency;
    int sim_system_mode;
} sjc16_shmoo_dram_info_t;

typedef struct sjc16_shmoo_config_param_s
{
    uint8   dq_byte_wr_min_vdl_bit[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE][SHMOO_SJC16_BYTE];
    uint8   dq_byte_wr_min_vdl_dbi[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint8   dq_byte_wr_min_vdl_edc[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_wr_max_vdl_data[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_wr_max_vdl_dqs[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];

    uint8   dq_byte_rd_min_vdl_bit[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE][SHMOO_SJC16_BYTE];
    uint8   dq_byte_rd_min_vdl_dbi[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint8   dq_byte_rd_min_vdl_edc[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_rd_max_vdl_dqsp[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_rd_max_vdl_dqsn[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];

    uint32  dq_byte_ren_fifo_config[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_edcen_fifo_config[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];    
    uint32  dq_byte_rd_max_vdl_fsm[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_vref_dac_config[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
    uint32  dq_byte_macro_reserved_reg[SHMOO_SJC16_DQ_CONFIG_NOF_BYTE];
   
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
    
} sjc16_shmoo_config_param_t;

typedef struct sjc16_vendor_info_s { 
    uint32 dram_density;
    uint32 fifo_depth;
    uint32 revision_id;        
    uint32 manufacture_id;
} sjc16_vendor_info_t;

typedef struct sjc16_shmoo_container_s
{
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 yJump;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 result2D[(SHMOO_SJC16_RESULT2D_X)*(SHMOO_SJC16_RESULT2D_Y)];
    uint32 resultData[SHMOO_SJC16_WORD];
    uint32 shmooType;
    uint32 dramType;
    uint32 ctlType;
    uint32 trefi;
    uint32 step1000;
    uint32 size1000UI;
    uint32 endUI[SHMOO_SJC16_MAX_VISIBLE_UI_COUNT];
    uint32 engageUIshift;
    uint32 restore;
    uint32 debugMode;
} sjc16_shmoo_container_t;

typedef struct sjc16_step_size_s
{
    uint32 step1000;
    uint32 size1000UI;
} sjc16_step_size_t;

typedef struct sjc16_ui_position_s
{
    uint32 ui;
    uint32 position;
} sjc16_ui_position_t;

typedef uint32 sjc16_shmoo_error_array_t[SHMOO_SJC16_ERROR_ARRAY_LENGTH];

extern int soc_sjc16_shmoo_dram_info_set(int unit, sjc16_shmoo_dram_info_t *sdi);
extern int soc_sjc16_shmoo_phy_init(int unit, int phy_ndx);
extern int soc_sjc16_shmoo_ctl(int unit, int phy_ndx, int shmoo_type, int stat, int plot, int action, sjc16_shmoo_config_param_t *config_param);

#endif /* _SHMOO_SJC16_H */

