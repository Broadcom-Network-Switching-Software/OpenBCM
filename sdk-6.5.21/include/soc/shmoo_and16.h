/* 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SHMOO_AND16_H
#define _SHMOO_AND16_H

#include <sal/types.h>
#include <soc/defs.h>
#include <shared/types.h>

#define SHMOO_AND16_PHY_TYPE_RSVP                       0
#define SHMOO_AND16_PHY_TYPE_K1                         0x141

#define SHMOO_AND16_CTL_TYPE_RSVP                       0
#define SHMOO_AND16_CTL_TYPE_RSVP_STR                   "t0"
#define SHMOO_AND16_CTL_TYPE_1                          1
#define SHMOO_AND16_CTL_TYPE_1_STR                      "t1"

#define SHMOO_AND16_DRAM_TYPE_RSVP                      0
#define SHMOO_AND16_DRAM_TYPE_DDR3                      3
#define SHMOO_AND16_DRAM_TYPE_DDR3L                     31
#define SHMOO_AND16_DRAM_TYPE_DDR4                      4
#define SHMOO_AND16_DRAM_TYPE_GDDR5                     5

#define SHMOO_AND16_PHY_REG_BASE_RSVP                   0
#define SHMOO_AND16_PHY_REG_BASE_HURRICANE4             0x1023C000

#define SHMOO_AND16_YDC_DDR_BIST_REG_BASE_RSVP          0
#define SHMOO_AND16_YDC_DDR_BIST_REG_BASE_HURRICANE4    0x1023BC00

#define SHMOO_AND16_CHIP_ID_RSVP                        0
#define SHMOO_AND16_CHIP_ID_HURRICANE4                  0x84

#if(CHIP_IS_HURRICANE4)
    #define SHMOO_AND16_CHIP_ID                     SHMOO_AND16_CHIP_ID_HURRICANE4
    #define SHMOO_AND16_PHY_TYPE                    SHMOO_AND16_PHY_TYPE_K1
    #define SHMOO_AND16_PHY_REG_BASE                SHMOO_AND16_PHY_REG_BASE_HURRICANE4
    #define SHMOO_AND16_YDC_DDR_BIST_REG_BASE       SHMOO_AND16_YDC_DDR_BIST_REG_BASE_HURRICANE4
    #define SHMOO_AND16_CTL_TYPE                    SHMOO_AND16_CTL_TYPE_1
    #define SHMOO_AND16_PHY_BITWIDTH                32
    #define SHMOO_AND16_PHY_CONSTANT_CONFIG         0
    #define PHY_AND16_K1                            SHMOO_AND16_PHY_TYPE
#endif

#define SHMOO_AND16_PHY_BITWIDTH_IS_32              (SHMOO_AND16_PHY_BITWIDTH >> 5)
#define YDC_DDR_BIST_PHY_BITWIDTH_IS_32             SHMOO_AND16_PHY_BITWIDTH_IS_32
#define YDC_DDR_BIST_REG_BASE                       SHMOO_AND16_YDC_DDR_BIST_REG_BASE

#define SHMOO_AND16_BIT                             1
#define SHMOO_AND16_BYTE                            (SHMOO_AND16_BIT << 3)
#define SHMOO_AND16_HALFWORD                        (SHMOO_AND16_BYTE << 1)
#define SHMOO_AND16_WORD                            (SHMOO_AND16_HALFWORD << 1)
#define SHMOO_AND16_DOUBLEWORD                      (SHMOO_AND16_WORD << 1)
#define SHMOO_AND16_QUADWORD                        (SHMOO_AND16_DOUBLEWORD << 1)

#define SHMOO_AND16_SHORT_SLEEP                     1
#define SHMOO_AND16_LONG_SLEEP                      10
#define SHMOO_AND16_DEEP_SLEEP                      100
#define SHMOO_AND16_HIBERNATION                     1000
#define SHMOO_AND16_STASIS                          1000000

#define SHMOO_AND16_SHMOO_RSVP                      -1
#define SHMOO_AND16_RD_EN                           23
#define SHMOO_AND16_RD_EXTENDED                     50
#define SHMOO_AND16_WR_EXTENDED                     60
#define SHMOO_AND16_ADDR_EXTENDED                   71
#define SHMOO_AND16_CTRL_EXTENDED                   72
#define SHMOO_AND16_RD_EN_ECC                       29
#define SHMOO_AND16_RD_EXTENDED_ECC                 59
#define SHMOO_AND16_WR_EXTENDED_ECC                 69
#define SHMOO_AND16_DDR3_SEQUENCE_COUNT             5
#define SHMOO_AND16_DDR3_ECC_32P4_SEQUENCE_COUNT    8
#define SHMOO_AND16_DDR3L_SEQUENCE_COUNT            5
#define SHMOO_AND16_DDR3L_ECC_32P4_SEQUENCE_COUNT   8
#define SHMOO_AND16_DDR4_SEQUENCE_COUNT             5
#define SHMOO_AND16_DDR4_ECC_32P4_SEQUENCE_COUNT    8
/* BEGIN: EXTRA */
/* END: EXTRA */

#define SHMOO_AND16_QUICK_SHMOO_CTRL_EXTENDED       1

#define SHMOO_AND16_REPEAT                          9
#define SHMOO_AND16_REPEAT_HALF                     (SHMOO_COMBO28_REPEAT >> 1)

#define SHMOO_AND16_SEQUENTIAL                      0
#define SHMOO_AND16_SINGLE                          1

#define SHMOO_AND16_ACTION_RSVP                     -1
#define SHMOO_AND16_ACTION_RESTORE                  0
#define SHMOO_AND16_ACTION_RUN                      1
#define SHMOO_AND16_ACTION_SAVE                     2
#define SHMOO_AND16_ACTION_RUN_AND_SAVE             3

#define SHMOO_AND16_MAX_INTERFACES                  8
#define SHMOO_AND16_INTERFACE_RSVP                  -1
#ifdef CONFIG_HURRICANE4
    #define SHMOO_AND16_BYTES_PER_INTERFACE         5
#else
    #define SHMOO_AND16_BYTES_PER_INTERFACE         4
#endif

#define SHMOO_AND16_BIST_NOF_PATTERNS               8
#define SHMOO_AND16_BIST_MPR_NOF_PATTERNS           4

#define SHMOO_AND16_MAX_VISIBLE_UI_COUNT            4
#define SHMOO_AND16_MAX_EFFECTIVE_UI_COUNT          4
#define SHMOO_AND16_LAST_EFFECTIVE_UI               (SHMOO_AND16_MAX_EFFECTIVE_UI_COUNT - 1)
#define SHMOO_AND16_MAX_VDL_LENGTH                  256
#define SHMOO_AND16_MAX_VREF_RANGE                  64
#define SHMOO_AND16_MAX_ZQ_CAL_RANGE                32

#define SHMOO_AND16_RESULT2D_X                      SHMOO_AND16_MAX_VDL_LENGTH
#define SHMOO_AND16_RESULT2D_Y                      SHMOO_AND16_MAX_VREF_RANGE

#define SHMOO_AND16_STRING_LENGTH                   (SHMOO_AND16_RESULT2D_X + 1)
#define SHMOO_AND16_ERROR_ARRAY_LENGTH              4

#define SHMOO_AND16_CALIB_CENTER_PASS               111
#define SHMOO_AND16_CALIB_RISING_EDGE               011
#define SHMOO_AND16_CALIB_FALLING_EDGE              100
#define SHMOO_AND16_CALIB_PASS_START                001
#define SHMOO_AND16_CALIB_FAIL_START                110
#define SHMOO_AND16_CALIB_VDL_ZERO                  000

#define SHMOO_AND16_VDL_TFIX                        18

#define SHMOO_AND16_RD_DATA_DLY_INIT                7

#define SHMOO_AND16_RD_EN_CYC_INIT                  1
#define SHMOO_AND16_RD_EN_VDL_INIT                  0

#define SHMOO_AND16_RD_DQ_VDL_INIT                  0
#define SHMOO_AND16_RD_DQ_FAIL_CAP                  5

#define SHMOO_AND16_RD_DQS_VDL_OFFSET               SHMOO_AND16_RD_DQ_VDL_INIT

#define SHMOO_AND16_WR_CYC_INIT                     0

#define SHMOO_AND16_PHY_NOF_AD                      16
#define SHMOO_AND16_PHY_NOF_BA                      3
#define SHMOO_AND16_PHY_NOF_AUX                     3
#define SHMOO_AND16_PHY_NOF_CS                      2

typedef struct and16_shmoo_dram_info_s
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
} and16_shmoo_dram_info_t;

typedef struct and16_shmoo_config_param_s
{
    uint16  control_regs_ad[SHMOO_AND16_PHY_NOF_AD];
    uint16  control_regs_ba[SHMOO_AND16_PHY_NOF_BA];
    uint16  control_regs_aux[SHMOO_AND16_PHY_NOF_AUX];
    uint16  control_regs_cs[SHMOO_AND16_PHY_NOF_CS];
    uint16  control_regs_par;
    uint16  control_regs_ras_n;
    uint16  control_regs_cas_n;
    uint16  control_regs_cke;
    uint16  control_regs_rst_n;
    uint16  control_regs_odt;
    uint16  control_regs_we_n;
    uint32  control_regs_vref_dac_control;

    uint16  wr_vdl_dqsp[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  wr_vdl_dqsn[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  wr_vdl_dq[SHMOO_AND16_BYTES_PER_INTERFACE][SHMOO_AND16_BYTE];
    uint16  wr_vdl_dm[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  wr_vdl_edc[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint8   wr_chan_dly_cyc[SHMOO_AND16_BYTES_PER_INTERFACE];

    uint16  rd_vdl_dqsp[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  rd_vdl_dqsn[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  rd_vdl_dqp[SHMOO_AND16_BYTES_PER_INTERFACE][SHMOO_AND16_BYTE];
    uint16  rd_vdl_dqn[SHMOO_AND16_BYTES_PER_INTERFACE][SHMOO_AND16_BYTE];
    uint16  rd_vdl_dmp[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  rd_vdl_dmn[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint16  rd_en_vdl_cs[SHMOO_AND16_BYTES_PER_INTERFACE][SHMOO_AND16_PHY_NOF_CS];
    uint16  rd_en_dly_cyc[SHMOO_AND16_BYTES_PER_INTERFACE];
    uint8   rd_control[SHMOO_AND16_BYTES_PER_INTERFACE];
} and16_shmoo_config_param_t;

typedef struct and16_vendor_info_s {
    uint32 dram_density;
    uint32 fifo_depth;
    uint32 revision_id;
    uint32 manufacture_id;
} and16_vendor_info_t;

typedef struct and16_shmoo_container_s
{
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 yJump;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 result2D[(SHMOO_AND16_RESULT2D_X)*(SHMOO_AND16_RESULT2D_Y)];
    uint32 resultData[SHMOO_AND16_WORD];
    uint32 shmooType;
    uint32 dramType;
    uint32 ctlType;
    uint32 trefi;
    uint32 step1000;
    uint32 size1000UI;
    uint32 endUI[SHMOO_AND16_MAX_VISIBLE_UI_COUNT];
    uint32 engageUIshift;
    uint32 wckInv;
} and16_shmoo_container_t;

typedef struct and16_step_size_s
{
    uint32 step1000;
    uint32 size1000UI;
} and16_step_size_t;

typedef struct and16_ui_position_s
{
    uint32 ui;
    uint32 position;
} and16_ui_position_t;

typedef uint32 and16_shmoo_error_array_t[SHMOO_AND16_ERROR_ARRAY_LENGTH];

extern int soc_and16_shmoo_dram_info_set(int unit, and16_shmoo_dram_info_t *sdi);
extern int soc_and16_shmoo_phy_init(int unit, int phy_ndx);
extern int soc_and16_shmoo_ctl(int unit, int phy_ndx, int shmoo_type, int stat, int plot, int action, and16_shmoo_config_param_t *config_param);

extern int iproc16_enable_ecc(int unit);
extern int iproc16_clear_ecc_syndrome(int unit);
extern int iproc16_read_ecc_syndrome(int unit);

#endif /* _SHMOO_AND16_H */

