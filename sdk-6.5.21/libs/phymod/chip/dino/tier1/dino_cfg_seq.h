/*----------------------------------------------------------------------
 *
 * Broadcom Corporation
 * Proprietary and Confidential information
 * All rights reserved
 * This source file is the property of Broadcom Corporation, and
 * may not be copied or distributed in any isomorphic form without the
 * prior written consent of Broadcom Corporation.
 *---------------------------------------------------------------------
 * File       : dino_cfg_seq.h
 * Description: c functions implementing Tier1
 *---------------------------------------------------------------------*/
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
*/


#ifndef DINO_CFG_SEQ_H 
#define DINO_CFG_SEQ_H
#include <phymod/phymod_acc.h>
#include "dino_serdes/common/srds_api_err_code.h"
#include "dino_types.h"

#define DEV1_SLICE_SLICE_ADR                      0x8000
#define DINO_DEV_PMA_PMD                          1
#define DINO_DEV_AN                               7
#define DINO_FW_DLOAD_RETRY_CNT                   1000
#define DINO_FW_ENABLE_RETRY_CNT                  100
#define DINO_ENABLE                               1
#define DINO_DISABLE                              0
#define DINO_SPD_100G                             100000
#define DINO_SPD_106G                             106000
#define DINO_SPD_111G                             111000
#define DINO_SPD_1G                               1000
#define DINO_SPD_10G                              10000
#define DINO_SPD_11G                              11000
#define DINO_SPD_11P18G                           11800
#define DINO_SPD_20G                              20000
#define DINO_SPD_21G                              21000
#define DINO_SPD_40G                              40000
#define DINO_SPD_42G                              42000
#define DINO_MAX_FALCON_LANE                      4
#define DINO_MAX_MERLIN_LANE                      4
#define DINO_MAX_CORE_LANE                        4
#define DINO_MAX_LANE                             12 
#define DINO_FW_ALREADY_DOWNLOADED                0xFAD
#define DINO_PMD_ID0                              0xAE02
#define DINO_PMD_ID1                              0x5390
#define DINO_CHIP_ID_82332                        0x82332
#define DINO_CHIP_ID_82793                        0x82793
#define DINO_CHIP_ID_82795                        0x82795
#define DINO_FW_HEADER_SIZE                       32
#define DINO_40G_PORT0_LANE_MASK                  0xf
#define DINO_40G_PORT1_LANE_MASK                  0xf0
#define DINO_40G_PORT2_LANE_MASK                  0xf00
#define DINO_100G_TYPE1_LANE_MASK                 0x3ff
#define DINO_100G_TYPE2_LANE_MASK                 0x7fe
#define DINO_100G_TYPE3_LANE_MASK                 0xffc
#define DINO_LANE_MASK_ALL_LANES                  0xfff
#define DINO_GPREG_0_ADR                          0x8250
#define DINO_GPREG_F_ADR                          0x825F
#define DINO_100G_CR10_AN_INSTANCE_MASK           0x3
#define DINO_100G_CR10_AN_INSTANCE_SHIFT          0
#define DINO_100G_SKIP_TX_TRAINING_MASK           0x10
#define DINO_40G_PORT0_SKIP_TX_TRAINING_MASK      0x10
#define DINO_40G_PORT1_SKIP_TX_TRAINING_MASK      0x20
#define DINO_40G_PORT2_SKIP_TX_TRAINING_MASK      0x40
#define DINO_100G_SKIP_TX_TRAINING_SHIFT          4 
#define DINO_40G_PORT0_SKIP_TX_TRAINING_SHIFT     4 
#define DINO_40G_PORT1_SKIP_TX_TRAINING_SHIFT     5 
#define DINO_40G_PORT2_SKIP_TX_TRAINING_SHIFT     6 
#define DINO_SW_GPREG_0_ADR                       0x8b90
#define DINO_MER_PLL_MAP_VAL_REG_ADR              0x8b9f
#define DINO_REF_CLK_REG_ADR                      0x8b9e
#define DINO_AN_MST_LANE_REG_ADR                  0x8b9d
#define DINO_AN_MST_LANE_100G_MASK                0xf
#define DINO_AN_MST_LANE_100G_SHIFT               0 
#define DINO_AN_MST_LANE_40G_P0_MASK              0xf0
#define DINO_AN_MST_LANE_40G_P0_SHIFT             4 
#define DINO_AN_MST_LANE_40G_P1_MASK              0xf00
#define DINO_AN_MST_LANE_40G_P1_SHIFT             8 
#define DINO_AN_MST_LANE_40G_P2_MASK              0xf000
#define DINO_AN_MST_LANE_40G_P2_SHIFT             12 
#define DINO_REF_CLK_VAL_MASK                     0x1f
#define DINO_REF_CLK_VAL_SHIFT                    0
#define DINO_LINE_INTF_MASK                       0x1f
#define DINO_LINE_INTF_SHIFT                      0
#define DINO_SYS_INTF_MASK                        0x3e0
#define DINO_SYS_INTF_SHIFT                       5
#define DINO_DEV_OP_MODE_MASK                     0xc00
#define DINO_DEV_OP_MODE_SHIFT                    10 
#define DINO_DEV_OP_MODE_PT                       0
#define DINO_DEV_OP_MODE_GBOX                     1
#define DINO_DEV_OP_MODE_INVALID                  0xf
#define DINO_PHY_TYPE_MASK                        0x3000
#define DINO_PHY_TYPE_SHIFT                       12
#define DINO_PHY_TYPE_ETH                         0
#define DINO_PHY_TYPE_HIGIG                       1
#define DINO_PHY_TYPE_OTN                         2
#define DINO_PHY_TYPE_INVALID                     0xf
#define DINO_INTF_TYPE_SR                         0
#define DINO_INTF_TYPE_LR                         1
#define DINO_INTF_TYPE_ER                         2
#define DINO_INTF_TYPE_CAUI4                      3
#define DINO_INTF_TYPE_CAUI                       4
#define DINO_INTF_TYPE_CAUI4_C2C                  5
#define DINO_INTF_TYPE_CAUI4_C2M                  6
#define DINO_INTF_TYPE_XLAUI                      7
#define DINO_INTF_TYPE_XFI                        8
#define DINO_INTF_TYPE_KX                         9
#define DINO_INTF_TYPE_KR                         10
#define DINO_INTF_TYPE_SFI                        11
#define DINO_INTF_TYPE_XLPPI                      12
#define DINO_INTF_TYPE_CX                         13
#define DINO_INTF_TYPE_CR                         14
#define DINO_INTF_TYPE_VSR                        15
#define DINO_INTF_TYPE_OTN                        16
#define DINO_INTF_TYPE_SGMII                      17
#define DINO_LINE_DFE_OPTION_MASK                 0xc00
#define DINO_LINE_DFE_OPTION_SHIFT                10
#define DINO_LINE_MEDIA_TYPE_MASK                 0x300
#define DINO_LINE_MEDIA_TYPE_SHIFT                8
#define DINO_LINE_TX_TRAINING_MASK                0x1000
#define DINO_LINE_TX_TRAINING_SHIFT               12
#define DINO_LINE_TX_TIMING_MODE_MASK             0x6000
#define DINO_LINE_TX_TIMING_MODE_SHIFT            13 
#define DINO_SYS_TX_TIMING_MODE_MASK              0x60
#define DINO_SYS_TX_TIMING_MODE_SHIFT             5 
#define DINO_AN_MASK                              0x8000
#define DINO_AN_SHIFT                             15
#define DINO_SYS_DFE_OPTION_MASK                  0xc
#define DINO_SYS_DFE_OPTION_SHIFT                 2
#define DINO_SYS_MEDIA_TYPE_MASK                  0x3
#define DINO_SYS_MEDIA_TYPE_SHIFT                 0 
#define DINO_SYS_TX_TRAINING_MASK                 0x10
#define DINO_SYS_TX_TRAINING_SHIFT                4 
#define DINO_FW_SM_ENABLE_MASK                    (1 << 7)
#define DINO_MODULE_CNTRL_RAM_NVR0_ADR            0x8800
#define DINO_MAX_INTR_SUPPORT                     10
#define DINO_FRC_TRAINING_DELAY_MASK              0x3
#define DINO_FRC_TRAINING_DELAY_SHIFT             0
#define DINO_FRC_TRAINING_DELAY_3SEC              0
#define DINO_FRC_TRAINING_DELAY_6SEC              1
#define DINO_FRC_TRAINING_DELAY_9SEC              2
#define DINO_FRC_TRAINING_DELAY_12SEC             3

typedef enum {
	MSGOUT_DONTCARE      = 0x0000,
	MSGOUT_GET_CNT	     = 0x8888,
	MSGOUT_GET_LSB	     = 0xABCD,
	MSGOUT_GET_MSB	     = 0x4321,
	MSGOUT_GET_2B	     = 0xEEEE,
	MSGOUT_GET_B	     = 0xF00D,
	MSGOUT_ERR		     = 0x0BAD,
	MSGOUT_NEXT		     = 0x2222, 
	MSGOUT_NOT_DWNLD     = 0x0101, 
	MSGOUT_DWNLD_ALREADY = 0x0202,
	MSGOUT_DWNLD_DONE    = 0x0303,
	MSGOUT_PRGRM_DONE    = 0x0404,
    MSGOUT_HDR_ERR       = 0x0E0E,
    MSGOUT_FLASH         = 0xF1AC
} DINO_MSGOUT_E;

typedef enum {
    DINO_SLICE_UNICAST    = 0,
    DINO_SLICE_MULTICAST  = 1,
    DINO_SLICE_BROADCAST  = 2,
    DINO_SLICE_RESET      = 3,
    DINO_CAST_INVALID     = 4
}DINO_SLICE_OP_TYPE;

typedef enum {
    DINO_IF_LINE = 0,
    DINO_IF_SYS  = 1
}DINO_IF_SIDE;

typedef enum {
    DINO_AN_NONE = 0,
    DINO_AN_CL73 = 1,
    DINO_AN_PROP = 2
}DINO_AN_MODE;

typedef enum 
{
    SERDES_MEDIA_TYPE_BACK_PLANE             = 0,  /* KR link: back plane media */
    SERDES_MEDIA_TYPE_COPPER_CABLE           = 1,  /* CR link: direct-attached copper cable */
    SERDES_MEDIA_TYPE_OPTICAL_RELIABLE_LOS   = 2,  /* SR/LR link: optical fiber with reliable LOS detection */
    SERDES_MEDIA_TYPE_OPTICAL_UNRELIABLE_LOS = 3   /* SR/LR link: optical fiber with unreliable LOS detection */
} DINO_MEDIA_TYPE;

typedef enum 
{
    SERDES_DFE_OPTION_NO_DFE      = 0,  /* no DFE is used */
    SERDES_DFE_OPTION_DFE         = 1,  /* DFE is enabled */
    SERDES_DFE_OPTION_BRDFE       = 2,  /* Force BRDFE mode, DFE will be on, too */
                                        /* only available in Falcon core */
    SERDES_DFE_OPTION_DFE_LP_MODE = 3   /* DFE is enabled in low-power mode */
                                        /* only available in Falcon core */
    /* DFE_LP_MODE and BRDEF are NOT supported in Merlin core,
     * so all non-zero values has the same effect as 1 for Merlin core.
     */
} DINO_DFE_OPTION;


typedef enum {
  DINO_PLL_MODE_64      = 0x0, /*4'b0000*/ 
  DINO_PLL_MODE_66      = 0x1, /*4'b0001*/ 
  DINO_PLL_MODE_80      = 0x2, /*4'b0010*/ 
  DINO_PLL_MODE_128     = 0x3, /*4'b0011*/ 
  DINO_PLL_MODE_132     = 0x4, /*4'b0100*/ 
  DINO_PLL_MODE_140     = 0x5, /*4'b0101*/ 
  DINO_PLL_MODE_160     = 0x6, /*4'b0110*/ 
  DINO_PLL_MODE_165     = 0x7, /*4'b0111*/ 
  DINO_PLL_MODE_168     = 0x8, /*4'b1000*/ 
  DINO_PLL_MODE_170     = 0x9, /*4'b1001*/ 
  DINO_PLL_MODE_175     = 0xa, /*4'b1010*/ 
  DINO_PLL_MODE_180     = 0xb, /*4'b1011*/ 
  DINO_PLL_MODE_184     = 0xc, /*4'b1100*/ 
  DINO_PLL_MODE_200     = 0xd, /*4'b1101*/ 
  DINO_PLL_MODE_224     = 0xe, /*4'b1110*/ 
  DINO_PLL_MODE_264     = 0xf, /*4'b1111*/ 
  DINO_PLL_MODE_33      = 0x10,
  DINO_PLL_MODE_32      = 0x11,
  DINO_PLL_MODE_16      = 0x12,
  DINO_PLL_MODE_82P5    = 0x13,
  DINO_PLL_MODE_40      = 0x14,
  DINO_PLL_MODE_70      = 0x15,
  DINO_PLL_MODE_INVALID = -1
} DINO_PLL_MODE_E;

typedef enum {
    DINO_PORT0    = 0,
    DINO_PORT1    = 1,
    DINO_PORT2    = 2,
    DINO_PORT3    = 3,
    DINO_PORT4    = 4,
    DINO_PORT5    = 5,
    DINO_PORT6    = 6,
    DINO_PORT7    = 7,
    DINO_PORT8    = 8,
    DINO_PORT9    = 9,
    DINO_PORT10   = 10,
    DINO_PORT11   = 11
} DINO_PORT_E;

#define DINO_IF_ERR_RETURN_FREE(B, A)                                           \
    do {                                                                        \
        int loc_err ;                                                           \
        if ((loc_err = (A)) != PHYMOD_E_NONE)                                   \
        {  if (B) { PHYMOD_FREE(B) ; } ;  return loc_err ; }                    \
    } while (0)

#define DINO_IF_ERR_RETURN(A)                                                   \
    do {                                                                        \
        if ((rv = (A)) != PHYMOD_E_NONE)                                        \
        {  goto ERR; }                                                          \
    } while (0)

#define DINO_RETURN_WITH_ERR(A, B)                                              \
    do {                                                                        \
        PHYMOD_DEBUG_ERROR(B);                                                  \
        rv = (A);                                                               \
        { goto ERR; }                                                           \
    } while (0)


#define DINO_GET_INTF_SIDE(phy, if_side)   (if_side = (phy->port_loc == phymodPortLocSys) ? DINO_IF_SYS : DINO_IF_LINE)

#define DINO_GET_PORT_FROM_MODE(_C, _CHIP_ID, _PRT)                                                                    \
    do {                                                                                                               \
        if (_CHIP_ID == DINO_CHIP_ID_82332) {                                                                          \
            if (_C->data_rate == DINO_SPD_100G || _C->data_rate == DINO_SPD_106G || _C->data_rate == DINO_SPD_111G) {                                        \
                _PRT = 1;                                                                                                  \
            } else if ((_C->data_rate == DINO_SPD_40G || _C->data_rate == DINO_SPD_42G)) {          \
                _PRT = 3;                                                                                                  \
            } else if (_C->data_rate == DINO_SPD_10G || _C->data_rate == DINO_SPD_11G || _C->data_rate == DINO_SPD_11P18G || _C->data_rate == DINO_SPD_1G) {   \
                _PRT = 12;                                                                                                 \
            }                                                                                                              \
        } else if (_CHIP_ID == DINO_CHIP_ID_82793) {                                                                       \
            if (_C->data_rate == DINO_SPD_100G || _C->data_rate == DINO_SPD_106G || _C->data_rate == DINO_SPD_111G) {                                        \
                _PRT = 1;                                                                                                  \
            } else if ((_C->data_rate == DINO_SPD_40G || _C->data_rate == DINO_SPD_42G)) {          \
                _PRT = 2;                                                                                                  \
            } else if (_C->data_rate == DINO_SPD_10G || _C->data_rate == DINO_SPD_11G || _C->data_rate == DINO_SPD_11P18G || _C->data_rate == DINO_SPD_1G) {   \
                _PRT = 10;                                                                                                 \
            }                                                                                                              \
        } else {                                                                                                           \
            if (_C->data_rate == DINO_SPD_100G || _C->data_rate == DINO_SPD_106G || _C->data_rate == DINO_SPD_111G) {                                        \
                _PRT = 1;                                                                                                  \
            } else if ((_C->data_rate == DINO_SPD_40G || _C->data_rate == DINO_SPD_42G)) {          \
                _PRT = 1;                                                                                                  \
            } else if (_C->data_rate == DINO_SPD_10G || _C->data_rate == DINO_SPD_11G || _C->data_rate == DINO_SPD_11P18G || _C->data_rate == DINO_SPD_1G) {   \
                _PRT = 4;                                                                                                 \
            }                                                                                                              \
        }                                                                                                                 \
    }while(0);


#define DINO_IS_FALCON_SIDE(_IF, _LANE)  ((_IF == DINO_IF_LINE) && (_LANE < DINO_MAX_FALCON_LANE))

#define DINO_IS_BCAST_SET(_IF, _LM)                                                                                                 \
        (((_IF == DINO_IF_SYS) &&                                                                                                   \
         ((_LM == 0xf) || (_LM == 0xf0) || (_LM == 0xf00) || (_LM == 0x3ff) || (_LM == 0x7fe) || (_LM == 0xffc))) ||                \
        ((_IF == DINO_IF_LINE) && ((_LM == 0xf) || (_LM == 0xf0) || (_LM == 0xf00))))                                

#define READ_DINO_PMA_PMD_REG(acc, addr, value)                                  \
    do {                                                                         \
        uint32_t read_tmp;                                                       \
        PHYMOD_IF_ERR_RETURN                                                     \
          (PHYMOD_BUS_READ(acc, ((DINO_DEV_PMA_PMD << 16) | addr), &read_tmp));  \
        value = (read_tmp & 0xffff);                                             \
      } while (0);

#define WRITE_DINO_PMA_PMD_REG(acc, addr, value)                                 \
        PHYMOD_IF_ERR_RETURN                                                     \
	        (PHYMOD_BUS_WRITE(acc, (DINO_DEV_PMA_PMD << 16) | addr, value));

#define READ_DINO_AN_REG(acc, addr, value)                                       \
    do {                                                                         \
        uint32_t an_read_tmp;                                                    \
        PHYMOD_IF_ERR_RETURN                                                     \
          (PHYMOD_BUS_READ(acc, ((DINO_DEV_AN << 16) | addr), &an_read_tmp));    \
        value = (an_read_tmp & 0xffff);                                          \
      } while (0);

#define WRITE_DINO_AN_REG(acc, addr, value)                                      \
        PHYMOD_IF_ERR_RETURN                                                     \
	        (PHYMOD_BUS_WRITE(acc, (DINO_DEV_AN << 16) | addr, value));

#define DINO_GET_NUM_LANES(cid, nl)             \
    do {                                        \
        if (cid == DINO_CHIP_ID_82332) {        \
            nl = 12;                            \
        } else {                                \
            nl = 10;                            \
        }                                       \
    } while (0);
typedef enum {
    DINO_CL73_NO_TECH = 0,
    DINO_CL73_1000BASE_KX = 0x1,
    DINO_CL73_10GBASE_KX4 = 0x2,
    DINO_CL73_10GBASE_KR = 0x4,
    DINO_CL73_40GBASE_KR4 = 0x8,
    DINO_CL73_40GBASE_CR4 = 0x10,
    DINO_CL73_100GBASE_CR10 = 0x20,
    DINO_CL73_100GBASE_KP4 = 0x40,
    DINO_CL73_100GBASE_KR4 = 0x80,
    DINO_CL73_100GBASE_CR4 = 0x100
} DINO_CL73_SPEED_T;

typedef enum {
    DINO_NO_PAUSE = 0,
    DINO_ASYM_PAUSE,
    DINO_SYMM_PAUSE,
    DINO_ASYM_SYMM_PAUSE,
    DINO_AN_PAUSE_COUNT
} DINO_AN_PAUSE_T;

typedef struct DINO_AN_ADV_ABILITY_S{
  DINO_CL73_SPEED_T an_base_speed;
  DINO_AN_PAUSE_T an_pause;
  uint16_t an_fec;
  uint16_t an_cl72;
} dino_an_adv_ability_t;

typedef struct dino_an_ability_s {
  dino_an_adv_ability_t cl73_adv; /*includes cl73 related */
  uint32_t an_master_lane; /* Master lane belongs to port. For 10 lane port[0-9], for 4 lane port [0-3], for 2 lane port [0-1]*/
} dino_an_ability_t;

int dino_get_chipid(const phymod_access_t *pa, uint32_t *chipid, uint32_t *rev);

int _dino_wait_mst_msgout(const phymod_access_t *pa, DINO_MSGOUT_E exp_message, int poll_time);

int _dino_core_init(const phymod_core_access_t* core, const phymod_core_init_config_t* init_config);

int _dino_set_slice_reg(const phymod_access_t* pa, DINO_SLICE_OP_TYPE slice_op_type, uint16_t if_side, uint16_t lane); 

int _dino_rx_pmd_lock_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t* rx_pmd_locked);

int _dino_get_rx_pmd_lock_status(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, uint32_t *link_sts);

int _dino_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _dino_udms_config(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config, DINO_AN_MODE mode);

int _dino_phy_interface_config_set(const phymod_phy_access_t *phy, uint32_t flags, const phymod_phy_inf_config_t* config);

int dino_is_lane_mask_set(const phymod_phy_access_t *phy, uint16_t prt, const phymod_phy_inf_config_t *config);

int _dino_config_dut_mode_reg(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _dino_udms_config(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config, DINO_AN_MODE mode);

int _dino_interface_set(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* config);

int _dino_phy_interface_config_set(const phymod_phy_access_t* pa, uint32_t flags, const phymod_phy_inf_config_t* config);

int _dino_phy_interface_config_get(phymod_phy_access_t *phy, uint32_t flags, phymod_phy_inf_config_t *config);

int _dino_configure_ref_clock(const phymod_access_t *pa, phymod_ref_clk_t ref_clk);

int _dino_get_pll_modes(const phymod_phy_access_t *phy, phymod_ref_clk_t ref_clk, const phymod_phy_inf_config_t *config, uint16_t *fal_pll_mode, uint16_t *mer_pll_mode);

int _dino_map_mer_pll_div(DINO_PLL_MODE_E pll_mode);

int _dino_config_pll_div(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config);

int _dino_core_reset_set(const phymod_access_t *pa, phymod_reset_mode_t reset_mode, phymod_reset_direction_t direction);

int _dino_fw_enable(const phymod_access_t *pa);

int _dino_save_interface(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config, phymod_interface_t intf);

int _dino_restore_interface(phymod_phy_access_t *phy, phymod_phy_inf_config_t *config, phymod_interface_t *intf);

int _dino_loopback_set(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable);

int _dino_falcon_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable);

int _dino_merlin_lpbk_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t enable);

int _dino_falcon_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable);

int _dino_merlin_lpbk_get(const phymod_access_t *pa, uint16_t if_side, phymod_loopback_mode_t loopback, uint32_t* enable);

int _dino_loopback_get(const phymod_access_t *pa, uint16_t if_side, uint16_t lane, phymod_loopback_mode_t loopback, uint32_t *enable);

int _dino_phy_status_dump(const phymod_access_t *pa, uint16_t if_side, uint16_t lane);

int _dino_port_from_lane_map_get(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t* cfg, uint16_t *port);

err_code_t _dino_merlin_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);

err_code_t _dino_falcon_read_pll_range(const phymod_access_t *pa, uint32_t *pll_range);

int _dino_restore_phy_type(const phymod_access_t *pa, phymod_phy_inf_config_t* config, uint16_t *phy_type); 

int _dino_save_phy_type(const phymod_access_t *pa, const phymod_phy_inf_config_t *config); 

int _dino_fifo_reset(const phymod_phy_access_t *phy, const phymod_phy_inf_config_t *config);

int _dino_phy_polarity_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t tx_polarity, uint16_t rx_polarity);

int _dino_phy_polarity_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, uint16_t *tx_polarity, uint16_t *rx_polarity);

int _dino_phy_fec_set(const phymod_phy_access_t* phy, uint16_t enable);

int _dino_phy_fec_get(const phymod_phy_access_t* phy, uint32_t *enable);

int _dino_phy_power_set(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, const phymod_phy_power_t* power);

int _dino_phy_power_get(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, phymod_phy_power_t* power);

int _dino_phy_reset_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, const phymod_phy_reset_t* reset);

int _dino_phy_reset_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_reset_t* reset);

int _dino_core_pll_sequencer_restart(const phymod_access_t* pa, uint16_t if_side, phymod_sequencer_operation_t operation);


/*TX transmission control*/
int _dino_phy_tx_lane_control_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_tx_lane_control_t tx_control);
int _dino_phy_tx_lane_control_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_tx_lane_control_t* tx_control);

/*Rx control*/
int _dino_phy_rx_lane_control_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_rx_lane_control_t rx_control);

int _dino_phy_rx_lane_control_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_phy_rx_lane_control_t* rx_control);

/*Set\Get TX Parameters*/
int _dino_phy_tx_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, const phymod_tx_t* tx);
int _dino_phy_tx_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_tx_t* tx);

/*Set\Get RX Parameters*/
int _dino_phy_rx_set(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, const phymod_rx_t* rx);
int _dino_phy_rx_get(const phymod_access_t* pa, uint16_t if_side, uint16_t lane, phymod_rx_t* rx);

/*PHY Rx adaptation resume*/
int _dino_phy_rx_adaptation_resume(const phymod_access_t* pa, uint16_t if_side, uint16_t lane);


int _dino_phy_cl72_set(const phymod_phy_access_t* phy, uint32_t cl72_en);

int _dino_phy_cl72_get(const phymod_phy_access_t* phy, uint32_t* cl72_en);

int _dino_phy_cl72_status_get(const phymod_phy_access_t* phy, uint16_t if_side, uint16_t lane, phymod_cl72_status_t* status);

int _dino_phy_firmware_lane_config_set(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t fw_config);

int _dino_phy_firmware_lane_config_get(const phymod_phy_access_t* phy, phymod_firmware_lane_config_t* fw_config);

int _dino_phy_autoneg_ability_set(const phymod_phy_access_t *phy, dino_an_ability_t *an_ability);

int _dino_phy_autoneg_set(const phymod_phy_access_t *phy, const phymod_autoneg_control_t* an);

int _dino_phy_autoneg_get(const phymod_phy_access_t *phy, phymod_autoneg_control_t* an, uint32_t *an_done);

int _dino_phy_autoneg_ability_get(const phymod_phy_access_t *phy, dino_an_ability_t *an_ablity);

int _dino_phy_autoneg_remote_ability_get(const phymod_phy_access_t *phy, phymod_autoneg_ability_t* an_ability_get);

int _dino_phy_intr_enable_set(const phymod_phy_access_t* phy, uint32_t intr_type, uint32_t enable);

int _dino_phy_intr_enable_get(const phymod_phy_access_t* phy, uint32_t intr_type, uint32_t* enable);

int _dino_phy_intr_status_get(const phymod_phy_access_t* phy, uint32_t* intr_status);

int _dino_phy_intr_status_clear(const phymod_phy_access_t* phy, uint32_t intr_type);

int  _dino_get_intr_reg (const phymod_phy_access_t *phy, uint32_t intr_type, uint16_t *bit_pos, uint16_t *int_grp);

DINO_INTR_TYPE_E convert_phymod_to_dino( uint16_t intr_type, uint16_t intf_side, uint16_t lane_mask);

int _dino_set_module_command(const phymod_access_t *pa, uint16_t xfer_addr, uint32_t slv_addr, unsigned char xfer_cnt, DINO_I2CM_CMD_E cmd);

int _dino_phy_i2c_read(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr, uint32_t no_of_bytes, uint8_t* read_data);

int _dino_phy_i2c_write(const phymod_access_t *pa, uint32_t slv_dev_addr, uint32_t start_addr, uint32_t no_of_bytes, const uint8_t* write_data);

int _dino_phy_rptr_rtmr_mode_set(const phymod_phy_access_t* phy, uint16_t op_mode);

int _dino_bcast_enable_set(const phymod_core_access_t* core, uint16_t bcast_en);

int _dino_phy_gpio_config_set(const phymod_access_t* pa, int pin_number, phymod_gpio_mode_t gpio_mode);

int _dino_phy_gpio_config_get(const phymod_access_t* pa, int pin_number, phymod_gpio_mode_t* gpio_mode);

int _dino_phy_gpio_pin_value_set(const phymod_access_t* pa, int pin_number, int value);

int _dino_phy_gpio_pin_value_get(const phymod_access_t* pa, int pin_number, int* value);

int _dino_cfg_fw_ull_dp(const phymod_core_access_t* core, uint16_t op_datapath);

int dino_download_prog_eeprom(const phymod_access_t *pa, uint8_t *new_fw, uint32_t fw_length, uint8_t prg_eeprom);

int _dino_phy_multi_get(const phymod_access_t* pa, phymod_multi_data_t* multi_data);

int _dino_phy_shift_pos_get(uint32_t lane_map, uint32_t *shift_pos);

#endif /* DINO_CFG_SEQ_H */
