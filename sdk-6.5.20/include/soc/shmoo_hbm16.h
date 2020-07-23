/** \file include/soc/shmoo_hbm16.h
 * 
 * HBM shmoo (HBM PHY tuning) definition. 
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
 
#include <soc/hbmc_shmoo.h>

#ifndef _SHMOO_HBM16_H
#define _SHMOO_HBM16_H

#define SHMOO_HBM16_BIT                           1
#define SHMOO_HBM16_BYTE                          (SHMOO_HBM16_BIT << 3)
#define SHMOO_HBM16_HALFWORD                      (SHMOO_HBM16_BYTE << 1)
#define SHMOO_HBM16_WORD                          (SHMOO_HBM16_HALFWORD << 1)
#define SHMOO_HBM16_DOUBLEWORD                    (SHMOO_HBM16_WORD << 1)
#define SHMOO_HBM16_QUADWORD                      (SHMOO_HBM16_DOUBLEWORD << 1)

#define SHMOO_HBM16_SHORT_SLEEP                   1
#define SHMOO_HBM16_LONG_SLEEP                    10
#define SHMOO_HBM16_DEEP_SLEEP                    100
#define SHMOO_HBM16_HIBERNATION                   1000
#define SHMOO_HBM16_STASIS                        10000

#define SHMOO_HBM16_MAX_VISIBLE_UI_COUNT          3
#define SHMOO_HBM16_MAX_EFFECTIVE_UI_COUNT        2
#define SHMOO_HBM16_LAST_EFFECTIVE_UI             (SHMOO_HBM16_MAX_EFFECTIVE_UI_COUNT - 1)
#define SHMOO_HBM16_MIN_UI_SIZE_PS                3
#define SHMOO_HBM16_MIN_DATA_RATE_MBPS            1000
#define SHMOO_HBM16_MAX_STORAGE_ELEMENT_COUNT     ((((10000000 * SHMOO_HBM16_MAX_VISIBLE_UI_COUNT) / SHMOO_HBM16_MIN_DATA_RATE_MBPS) / SHMOO_HBM16_MIN_UI_SIZE_PS) + 1)
#define SHMOO_HBM16_MAX_VDL_LENGTH                (128 << 2)
#define SHMOO_HBM16_MIN_VDL_LENGTH                (32 << 2)
#define SHMOO_HBM16_MAX_VREF_RANGE                8

#define SHMOO_HBM16_RESULT2D_X                    SHMOO_HBM16_MAX_STORAGE_ELEMENT_COUNT
#define SHMOO_HBM16_RESULT2D_Y                    SHMOO_HBM16_MAX_VREF_RANGE

#define SHMOO_HBM16_STRING_LENGTH                 (SHMOO_HBM16_RESULT2D_X + 1)

#define SHMOO_HBM16_CTL_TYPE_RSVP                 0
#define SHMOO_HBM16_CTL_TYPE_RSVP_STR             "t0"
#define SHMOO_HBM16_CTL_TYPE_1                    1
#define SHMOO_HBM16_CTL_TYPE_1_STR                "t1"

#define SHMOO_HBM16_DRAM_TYPE_RSVP                0
#define SHMOO_HBM16_DRAM_TYPE_GEN2                2

#define SHMOO_HBM16_CALIB_CENTER_PASS             111
#define SHMOO_HBM16_CALIB_RISING_EDGE             011
#define SHMOO_HBM16_CALIB_FALLING_EDGE            100
#define SHMOO_HBM16_CALIB_ANY_EDGE                010
#define SHMOO_HBM16_CALIB_PASS_START              001
#define SHMOO_HBM16_CALIB_FAIL_START              110

#define SHMOO_HBM16_CALIB_SWEEP_1                 1
#define SHMOO_HBM16_CALIB_SWEEP_BYTE_TO_BIT       2
#define SHMOO_HBM16_CALIB_SWEEP_HALFWORD_TO_BIT   3
#define SHMOO_HBM16_CALIB_SWEEP_WORD_TO_BIT       4

#define SHMOO_HBM16_CALIB_GROUP_ORDER_DONT_CARE   0x00000000
#define SHMOO_HBM16_CALIB_GROUP_ORDER_00112233    0x00112233
#define SHMOO_HBM16_CALIB_GROUP_ORDER_01230123    0x01230123

#define SHMOO_HBM16_CALIB_PASS_ANY                0x00000001
#define SHMOO_HBM16_CALIB_PASS_ALL                0xFFFFFFFF

#define SHMOO_HBM16_SHMOO_RSVP                    -1
#define SHMOO_HBM16_RD_EN_FISH                    24
#define SHMOO_HBM16_RD_EXTENDED                   50
#define SHMOO_HBM16_WR_EXTENDED                   60
#define SHMOO_HBM16_ADDR_CTRL_EXTENDED            70
#define SHMOO_HBM16_GEN2_SEQUENCE_COUNT           3
#define SHMOO_HBM16_GEN2_RESTORE_SEQUENCE_COUNT   0
/* BEGIN: EXTRA */
/* END: EXTRA */

#define SHMOO_HBM16_REPEAT                        9
#define SHMOO_HBM16_REPEAT_HALF                   (SHMOO_HBM16_REPEAT >> 1)

#define SHMOO_HBM16_SEQUENTIAL                    0
#define SHMOO_HBM16_SINGLE                        1

#define SHMOO_HBM16_ACTION_RSVP                   -1
#define SHMOO_HBM16_ACTION_RESTORE                0
#define SHMOO_HBM16_ACTION_RUN                    1
#define SHMOO_HBM16_ACTION_SAVE                   2
#define SHMOO_HBM16_ACTION_RUN_AND_SAVE           3

#define SHMOO_HBM16_CTL_FLAGS_STAT_BIT            0x1
#define SHMOO_HBM16_CTL_FLAGS_PLOT_BIT            0x2
#define SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT  0x4
#define SHMOO_HBM16_CTL_FLAGS_CUSTOM_BIST_BIT     0x8
#define SHMOO_HBM16_CTL_FLAGS_DWORD0              0x10
#define SHMOO_HBM16_CTL_FLAGS_DWORD1              0x20
#define SHMOO_HBM16_CTL_FLAGS_DWORD2              0x40
#define SHMOO_HBM16_CTL_FLAGS_DWORD3              0x80
#define SHMOO_HBM16_CTL_FLAGS_DWORDS_ALL          (SHMOO_HBM16_CTL_FLAGS_DWORD0 | SHMOO_HBM16_CTL_FLAGS_DWORD1 | SHMOO_HBM16_CTL_FLAGS_DWORD2 | SHMOO_HBM16_CTL_FLAGS_DWORD3)
#define SHMOO_HBM16_DEBUG_MODE_FLAG_BITS          SHMOO_HBM16_CTL_FLAGS_EXT_VREF_RANGE_BIT
#define SHMOO_HBM16_LOOPBACK_MODE_READ_LFSR       0x100
#define SHMOO_HBM16_LOOPBACK_MODE_WRITE_LFSR      0x200

#define SHMOO_HBM16_MAX_INTERFACES                8
#define SHMOO_HBM16_INTERFACE_RSVP                -1
#define SHMOO_HBM16_BITS_PER_WORD                 32
#define SHMOO_HBM16_BYTES_PER_WORD                4
#define SHMOO_HBM16_WORDS_PER_CHANNEL             4
#define SHMOO_HBM16_CHANNELS_PER_INTERFACE        8
#define SHMOO_HBM16_CHANNEL_RSVP                  -1
#define SHMOO_HBM16_DQ_BIT_NOT_SWAPPED            0xFF 

#define SHMOO_HBM16_PLL_NOF_MDIV_CHANNELS         6

#define SHMOO_HBM16_REFRESH_SHUTDOWN_REFI         0

#define SHMOO_HBM16_MIDSTACK_CHANNEL			  -1

/* Initial UI Shifts */
#define SHMOO_HBM16_GEN2_INIT_CK_UI_SHIFT         0
#define SHMOO_HBM16_GEN2_INIT_ADDR_CTRL_UI_SHIFT  0
#define SHMOO_HBM16_GEN2_INIT_DQS_UI_SHIFT        1
#define SHMOO_HBM16_GEN2_INIT_DATA_UI_SHIFT       0

/* Initial VDL Positions */
#define SHMOO_HBM16_GEN2_INIT_READ_MIN_VDL_POS    4

/** channel reset flags */
#define SHMOO_HBM16_PHY_RESET_FLAG_DWORD_ALIGNMENT_NOT_NEEDED 0X1

typedef struct hbm16_shmoo_container_s
{
    uint32 sizeX;
    uint32 sizeY;
    uint32 yCapMin;
    uint32 yCapMax;
    uint32 calibMode;
    uint32 calibPos;
    uint32 calibStart;
    uint32 result2D[SHMOO_HBM16_WORDS_PER_CHANNEL][(SHMOO_HBM16_RESULT2D_X)*(SHMOO_HBM16_RESULT2D_Y)];
    uint32 resultData[SHMOO_HBM16_WORDS_PER_CHANNEL];
    uint32 shmooType;
    uint32 dramType;
    uint32 ctlType;
    uint32 trefi;
    uint32 step1000;
    uint32 size1000UI;
    uint32 endUI[SHMOO_HBM16_MAX_VISIBLE_UI_COUNT];
    uint32 engageUIshift;
    uint32 wckInv;
    uint32 restore;
    uint32 debugMode;
    uint32 customBistMode;
    uint32 dwordsBitmap;
    uint32 read_lfsr_loopback;
    uint32 write_lfsr_loopback;
} hbm16_shmoo_container_t;

typedef struct hbm16_step_size_s
{
    uint32 step1000;
    uint32 size1000UI;
} hbm16_step_size_t;

typedef struct hbm16_ui_position_s
{
    uint32 ui;
    uint32 position;
} hbm16_ui_position_t;

soc_hbm16_phy_reg_read_t soc_hbm16_phy_reg_read;
soc_hbm16_phy_reg_write_t soc_hbm16_phy_reg_write;
soc_hbm16_phy_reg_modify_t soc_hbm16_phy_reg_modify;
int soc_hbm16_shmoo_interface_cb_register(int unit, hbmc_shmoo_cbi_t shmoo_cbi);
int soc_hbm16_calculate_step_size(int unit, int hbc_ndx, hbm16_step_size_t *ssPtr);
int soc_hbm16_shmoo_dram_info_set(int unit, CONST hbmc_shmoo_dram_info_t *sdi);
int soc_hbm16_shmoo_phy_cfg_pll(int unit, int hbc_ndx);
int soc_hbm16_shmoo_phy_init(int unit, int hbc_ndx, int channel);
int soc_hbm16_shmoo_ctl(int unit, int hbc_ndx, int channel, int shmoo_type, uint32 flags, int action, hbmc_shmoo_config_param_t *config_param);
const hbmc_shmoo_channel_config_param_t * soc_hbm16_shmoo_channel_config_param_metadata_get  (void);
const hbmc_shmoo_midstack_config_param_t  * soc_hbm16_shmoo_midstack_config_param_metadata_get (void);

int _hbm16_phy_channel_reset(int unit, int flags, int hbm_ndx, int channel);

#endif /* _SHMOO_HBM16_H */
