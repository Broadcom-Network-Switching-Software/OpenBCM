/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wc40_extra.h
 * Purpose:     non-register Macro, data structures and function prototypes for WC driver. 
 */


#ifndef TSCMOD_EXTRA_H__
#define TSCMOD_EXTRA_H__

/*
 * wc40.h has definitions related to WC registers. It is automatically generated 
 * from RDB files. This file has all non-register Macro, data structures and 
 * function prototypes the WC driver.
 */

/* additional missed register definitions */

#define TSCMOD_XGXSBLK0_XGXSSTATUSr      0x8001
#define TSCMOD_TX0_ANATXACONTROL0r       0x8061
#define TSCMOD_RX0_ANARXCONTROLPCIr      0x80ba
#define TSCMOD_COMBO_IEEE0_MIICNTLr      0xffe0
#define TSCMOD_MICROBLK_DOWNLOAD_STATUSr 0xffc5
#define SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI  0x25
#define SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI  0x29

/* misc. */
#define TSCMOD_PLL_WAIT   400000   /* 400ms */
#define TSCMOD_AN_WAIT   400000   /* 400ms */
#define TX_DRIVER_DFT_LN_CTRL   0x7fff

#define SERDES_ID0_MODEL_NUMBER_WARPCORE 0x9   
#define WC_AER_BCST_OFS_STRAP   0x1ff
#define OVER1G_UP3_20GPLUS_MASK (DIGITAL3_UP3_DATARATE_25P45GX4_MASK | \
                                 DIGITAL3_UP3_DATARATE_21GX4_MASK | \
                                 DIGITAL3_UP3_DATARATE_31P5G_MASK | \
                                 DIGITAL3_UP3_DATARATE_40G_MASK | \
                                 DIGITAL3_UP3_LAST_MASK)
#define PLL_PLLACONTROL4_BIT12_MASK         0x1000
#define TSCMOD_UCODE_MAX_LEN   (32*1024)
#define NUM_LANES             4    /* num of lanes per core */
#define MAX_NUM_LANES         4    /* max num of lanes per port */
#define PROG_DATA_NUM  	      8    /* 8 * 32 = 256 bits */

/* no clock comp in HG combo mode */
#define COMBO_HG_XGXS_nLQnCC \
        (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_XGXS_nLQnCC << \
         XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_SHIFT)
#define COMBO_HG_XGXS_nCC \
        (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_XGXG_nCC << \
         XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_SHIFT)

/* no clock comp in XE combo mode */
#define COMBO_XE_XGXG_nCC \
        (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_XGXG_nCC << \
         XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_SHIFT)

 /* clock comp in HG combo mode */
#define COMBO_HG_XGXS_nLQ \
        (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_XGXS_nLQ << \
        XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_SHIFT)

/*  clock comp in XE combo mode */
#define COMBO_XE_XGXS \
        (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_XGXS << \
        XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_SHIFT)

#define COMBO_CLOCK_COMP_DEFAULT   (COMBO_HG_XGXS_nLQnCC | COMBO_XE_XGXG_nCC)
#define COMBO_CLOCK_COMP_MASK (XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK | \
                XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_MASK)
#define IND_CLOCK_COMP_DEFAULT        (COMBO_HG_XGXS_nLQnCC | COMBO_XE_XGXG_nCC)
#define IND_CLOCK_COMP_DEFAULT_REV_A  (COMBO_HG_XGXS_nLQ | COMBO_XE_XGXS)

/* TSC revision IDs */

#define TSCMOD_SERDES_ID0_REVID_A0    0x0000
#define TSCMOD_SERDES_ID0_REVID_A1    0x0800
#define TSCMOD_SERDES_ID0_REVID_A2    0x1000
/*
#define TSCMOD_SERDES_ID0_REVID_B0    0x4000
#define TSCMOD_SERDES_ID0_REVID_C0    0x8000
#define TSCMOD_SERDES_ID0_REVID_D0    0xc000
*/

#define REV_VER_MASK (MAIN0_SERDESID_REV_LETTER_MASK | \
                         MAIN0_SERDESID_REV_NUMBER_MASK)
#define TSCMOD_REVID(_pc) \
     ((((TSCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK)
#define TSCMOD_REVID_A0(_pc) \
     (((((TSCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        TSCMOD_SERDES_ID0_REVID_A0)
#define TSCMOD_REVID_A1(_pc) \
     (((((TSCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        TSCMOD_SERDES_ID0_REVID_A1)
#define TSCMOD_REVID_B0(_pc) \
     (((((TSCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        TSCMOD_SERDES_ID0_REVID_B0)
#define TSCMOD_LINK_WAR_REVS(_pc) (TSCMOD_REVID_A0(_pc) || TSCMOD_REVID_A1(_pc))

/* div16 and div33 clocks */

#define CL72_MISC4_CTRL_DIV16_SHIFT       5
#define CL72_MISC4_CTRL_DIV33_SHIFT       3
#define CL72_MISC4_CTRL_DIV16_FIELD(_fd)  ((_fd) << CL72_MISC4_CTRL_DIV16_SHIFT)
#define CL72_MISC4_CTRL_DIV33_FIELD(_fd)  ((_fd) << CL72_MISC4_CTRL_DIV33_SHIFT)
#define CL72_MISC4_CTRL_DIV16_MASK        (0xf << CL72_MISC4_CTRL_DIV16_SHIFT)
#define CL72_MISC4_CTRL_DIV33_MASK        (0x3 << CL72_MISC4_CTRL_DIV33_SHIFT)

/* CL73 */

#define CL73_AN_ADV_TECH_20G_KR2   (1 << 13)
#define CL73_AN_ADV_TECH_100G_CR10 (1 << 10)
#define CL73_AN_ADV_TECH_40G_CR4 (1 << 9)
#define CL73_AN_ADV_TECH_40G_KR4 (1 << 8)
#define CL73_AN_ADV_TECH_10G_KR  (1 << 7)
#define CL73_AN_ADV_TECH_10G_KX4 (1 << 6)
#define CL73_AN_ADV_TECH_1G_KX   (1 << 5)
#define CL73_AN_ADV_TECH_SPEEDS_MASK  (0x3F << 5)
#define CL73_AN_ADV_PLUS_KR2_MASK  (CL73_AN_ADV_TECH_SPEEDS_MASK | \
                CL73_AN_ADV_TECH_20G_KR2)
#define CL73_AN_ADV_PAUSE                                (1 << 10)
#define CL73_AN_ADV_ASYM_PAUSE                           (1 << 11)

/* VCO frequency ctrl */

#define TSCMOD_OS5_VCO_FREQ  0x7200
#define TSCMOD_OS8_VCO_FERQ  0x7A00
#define TSCMOD_TXCK_MASK     0x1E0
#define TSCMOD_RXCK_OVERIDE_MASK  (1 << 11)
#define TSCMOD_TXCK_SHIFTER  5

/*  Bit 0   CL73 BAM
    Bit 1   CL73
    Bit 2   HPBAM
    Bit 3   HPBAM_vs_SW
      AutoDetect to
    Bit 4   CL73
    Bit 5
    Bit 6
    Bit 7
       SW switch to
    Bit 8   CL37

    Only 0x5 is the exception. (= 0x102)
*/
#define TSCMOD_CL73_HPAM_CL37          0x104
#define TSCMOD_CL73_HPAM_AUTOCL73_CL37 0x114
#define TSCMOD_CL73_HPAM_AUTOCL73      0x014
#define TSCMOD_CL73_SW_CL37            0x102

#define TSCMOD_CL73_HG                0x6
#define TSCMOD_CL73_CL37              0x5
#define TSCMOD_CL73_HPAM              0x4
#define TSCMOD_CL73_HPAM_VS_SW        0x8
#define TSCMOD_CL73_WO_BAM            0x2
#define TSCMOD_CL73_W_BAM             0x1
#define TSCMOD_CL73_DISABLE           0x0

#define TSCMOD_CL37_W_BAM_HG     6
#define TSCMOD_CL37_HR2SPM_W_10G 5
#define TSCMOD_CL37_HR2SPM       4
#define TSCMOD_CL37_W_10G        3
#define TSCMOD_CL37_WO_BAM       2
#define TSCMOD_CL37_W_BAM        1
#define TSCMOD_CL37_DISABLE      0


/* lane swap functions in XGXSBLK2 are obsolete, use ones in XGXSBLK8
 * i.e. define TSCMOD_PCS_LANE_SWAP
 */
#define TSCMOD_PCS_LANE_SWAP
#ifdef TSCMOD_PCS_LANE_SWAP
/* For Tx lane swap
 * bits 15-12 11-8 7-4 3-0  represents pcs lane 3,2,1,0
 * value in each field represents corresponding pmd/pma lane number.
 * the default map is pma/pmd lane3 maps to pcs lane3, pma lane2
 * to pcs lane2 and so on.
 *
 * For Rx lane swap
 * The same bit fields represents the pmd/pma lane 3,2,1,0
 * and the value corresponds the pma/pmd lane number.
 */
#define TSCMOD_RX_LANE_MAP_DEFAULT    0x3210
#define TSCMOD_TX_LANE_MAP_DEFAULT    0x3210
#else
#define TSCMOD_RX_LANE_MAP_DEFAULT    0x0123
#define TSCMOD_TX_LANE_MAP_DEFAULT    0x0123
#endif

/* Register lane access. fixed value, must not change */

#define LANE0_ACCESS      1
#define LANE1_ACCESS      2
#define LANE2_ACCESS      3
#define LANE3_ACCESS      4
#define LANE_BCST         0xF
#define LANE_ACCESS_MASK  0xF

/* operation modes */

#define CUSTOM1_MODE(_pc) ((_pc)->phy_mode == PHYCTRL_LANE_MODE_CUSTOM1)
#define CUSTOM_MODE(_pc) (((_pc)->phy_mode == PHYCTRL_LANE_MODE_CUSTOM) || \
                                ((_pc)->phy_mode == PHYCTRL_LANE_MODE_CUSTOM_3p125MHZ))
#define CUSTOMX_MODE(_pc) (CUSTOM_MODE(_pc) || CUSTOM1_MODE(_pc))

#define IS_DUAL_LANE_PORT(_pc) (((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) || \
                                CUSTOM1_MODE(_pc))
#define IS_QUAD_LANE_PORT(_pc) ((_pc)->phy_mode == PHYCTRL_QUAD_LANE_PORT)
#define IS_ONE_LANE_PORT(_pc) ((_pc)->phy_mode == PHYCTRL_ONE_LANE_PORT)

#define DUAL_LANE_BCST_ENABLE(_pc)  \
        do { \
            if (IS_DUAL_LANE_PORT(_pc)) { \
                DEV_CFG_PTR(_pc)->dxgxs = (_pc)->lane_num? 2 :1; \
            } \
        } while (0)
#define DUAL_LANE_BCST_DISABLE(_pc)  \
        do { \
            if (IS_DUAL_LANE_PORT(_pc)) { \
                DEV_CFG_PTR(_pc)->dxgxs = 0; \
            } \
        } while (0)
#define AUTONEG_MODE_UNAVAIL(_pc) (CUSTOMX_MODE(_pc))

#define TSCMOD_MEM_ALLOC(_size,_name) sal_alloc(_size,_name)
#define TSCMOD_MEM_SET(_dst,_val,_len) sal_memset(_dst,_val,_len)
#define TSCMOD_UDELAY(_time) sal_udelay(_time)
#define TSCMOD_USLEEP(_time) sal_usleep(_time)

#define FX100_CONTROL1_AUTODET_EN_MASK 4

/* similar to SOC_PORT_IF_, but within 32 bit range */
typedef enum tscmod_port_if_e {
    TSCMOD_PORT_IF_NOCXN, /* No physical connection */
    TSCMOD_PORT_IF_NULL,  /* Pass-through connection without PHY */
    TSCMOD_PORT_IF_MII,
    TSCMOD_PORT_IF_GMII,
    TSCMOD_PORT_IF_SGMII,
    TSCMOD_PORT_IF_TBI,
    TSCMOD_PORT_IF_XGMII,
    TSCMOD_PORT_IF_RGMII,
    TSCMOD_PORT_IF_RvMII,
    TSCMOD_PORT_IF_SFI,
    TSCMOD_PORT_IF_XFI,
    TSCMOD_PORT_IF_KR,
    TSCMOD_PORT_IF_KR4,
    TSCMOD_PORT_IF_CR,
    TSCMOD_PORT_IF_CR4,
    TSCMOD_PORT_IF_XLAUI,
    TSCMOD_PORT_IF_SR,
    TSCMOD_PORT_IF_RXAUI,
    TSCMOD_PORT_IF_XAUI,
    /*
    TSCMOD_PORT_IF_SPAUI,
    TSCMOD_PORT_IF_QSGMII,
    TSCMOD_PORT_IF_ILKN,
    TSCMOD_PORT_IF_RCY,
    TSCMOD_PORT_IF_FAT_PIPE,
    TSCMOD_PORT_IF_CGMII,
    TSCMOD_PORT_IF_CAUI,
     */
    TSCMOD_PORT_IF_LR,
    TSCMOD_PORT_IF_LR4,
    TSCMOD_PORT_IF_SR4,
    TSCMOD_PORT_IF_KX,
    /*
    TSCMOD_PORT_IF_ZR,
    TSCMOD_PORT_IF_SR10,
    TSCMOD_PORT_IF_OTL,
    TSCMOD_PORT_IF_CPU,
    TSCMOD_PORT_IF_OLP,
    TSCMOD_PORT_IF_OAMP,
    TSCMOD_PORT_IF_ERP,
    TSCMOD_PORT_IF_TM_INTERNAL_PKT,
    */
    TSCMOD_PORT_IF_SR2,
    TSCMOD_PORT_IF_KR2,
    TSCMOD_PORT_IF_CR2,
    TSCMOD_PORT_IF_XFI2,
    TSCMOD_PORT_IF_XLAUI2,
    TSCMOD_PORT_IF_COUNT /* last, please */
} tscmod_port_if_t;

#define TSCMOD_IF_SR     (1 << TSCMOD_PORT_IF_SR)
#define TSCMOD_IF_SR4    (1 << TSCMOD_PORT_IF_SR4)
#define TSCMOD_IF_LR     (1 << TSCMOD_PORT_IF_LR)
#define TSCMOD_IF_LR4    (1 << TSCMOD_PORT_IF_LR4)
#define TSCMOD_IF_KR     (1 << TSCMOD_PORT_IF_KR)
#define TSCMOD_IF_KR2    (1 << TSCMOD_PORT_IF_KR2)
#define TSCMOD_IF_KR4    (1 << TSCMOD_PORT_IF_KR4)
#define TSCMOD_IF_CR     (1 << TSCMOD_PORT_IF_CR)
#define TSCMOD_IF_CR2    (1 << TSCMOD_PORT_IF_CR2)
#define TSCMOD_IF_CR4    (1 << TSCMOD_PORT_IF_CR4)
#define TSCMOD_IF_XFI    (1 << TSCMOD_PORT_IF_XFI)
#define TSCMOD_IF_SFI    (1 << TSCMOD_PORT_IF_SFI)
#define TSCMOD_IF_XLAUI  (1 << TSCMOD_PORT_IF_XLAUI)
#define TSCMOD_IF_XGMII  (1 << TSCMOD_PORT_IF_XGMII)
#define TSCMOD_IF_ILKN   (1 << TSCMOD_PORT_IF_ILKN)
#define TSCMOD_40G_10G_INTF(_if)  ((_if) == TSCMOD_PORT_IF_KR4 || (_if) == TSCMOD_PORT_IF_XLAUI || \
                             (_if) == TSCMOD_PORT_IF_CR4 || (_if) == TSCMOD_PORT_IF_SR || \
                             (_if) == TSCMOD_PORT_IF_SR4 || \
                             (_if) == TSCMOD_PORT_IF_LR4 || (_if) == TSCMOD_PORT_IF_LR || \
                             (_if) == TSCMOD_PORT_IF_KR2 || (_if) == TSCMOD_PORT_IF_CR2 || \
                             (_if) == TSCMOD_PORT_IF_KR || (_if) == TSCMOD_PORT_IF_CR || \
                             (_if) == TSCMOD_PORT_IF_XFI || (_if) == TSCMOD_PORT_IF_SFI || \
                             (_if) == TSCMOD_PORT_IF_XGMII)

#define TSCMOD_40G_10G_INTF_ALL   (TSCMOD_IF_KR4 | TSCMOD_IF_XLAUI | TSCMOD_IF_CR4 | TSCMOD_IF_SR4 | \
                        TSCMOD_IF_LR4 | TSCMOD_IF_LR | \
                        TSCMOD_IF_KR2 | TSCMOD_IF_CR2 | \
                        TSCMOD_IF_SR | TSCMOD_IF_KR | TSCMOD_IF_CR | TSCMOD_IF_XFI | TSCMOD_IF_SFI | \
                        TSCMOD_IF_XGMII)

#define TSCMOD_UC_CTRL_XLAUI   0x3
#define TSCMOD_UC_CTRL_SR4     0x1
#define TSCMOD_UC_CTRL_SFP_DAC 0x2
#define TSCMOD_UC_CTRL_OPT_PF_TEMP_COMP  0x8

#define TSCMOD_AN_WAIT_INTERVAL    1000000  /* 1 second */

#define TSCMOD_AN_WAIT_RESTART_TMR 4000000
#define TSCMOD_AN_WAIT_CL73_TMR    4000000
#define TSCMOD_AN_WAIT_HPBAM_TMR   6000000
#define TSCMOD_AN_WAIT_CL37_TMR    1000000

#define TSCMOD_AN_CL73_CL37_OFF   0x0
#define TSCMOD_AN_CL73_CL37_ON    0x1
#define TSCMOD_AN_WAIT_CL73       0x2
#define TSCMOD_AN_WAIT_CL37       0x3
#define TSCMOD_AN_CL73_CL37_DONE  0x4

#define TSCMOD_AN_MODE_CL37_TRIED 0x1

#define TSCMOD_SP_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.start_time)
#define TSCMOD_SP_VALID(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state)
#define TSCMOD_SP_CNT(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt)

#define TSCMOD_SP_RECORD_TIME(_pc)  \
        (DEV_CTRL_PTR(_pc)->lkwa.start_time) = sal_time_usecs()
#define TSCMOD_SP_CNT_INC(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt)++
#define TSCMOD_SP_CNT_CLEAR(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt) = 0
#define TSCMOD_SP_VALID_SET(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state) = TRUE
#define TSCMOD_SP_VALID_RESET(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state) = FALSE

#define TSCMOD_AN_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->an.start_time)
#define TSCMOD_AN_STATE(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state)
#define TSCMOD_AN_RECORD_TIME(_pc)  \
        (DEV_CTRL_PTR(_pc)->an.start_time) = sal_time_usecs()
#define TSCMOD_AN_VALID_SET(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state) = TRUE
#define TSCMOD_AN_VALID_RESET(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state) = FALSE
#define TSCMOD_AN_CHECK_TIMEOUT(_pc) \
        ((uint32)SAL_USECS_SUB(sal_time_usecs(), TSCMOD_AN_TIME((_pc))) \
                >= TSCMOD_AN_WAIT_INTERVAL)
#define TSCMOD_AN_TICK_CNT(_pc)    \
        (DEV_CTRL_PTR(_pc)->an.tick_cnt)
#define TSCMOD_AN_MODE(_pc)       \
        (DEV_CTRL_PTR(_pc)->an.mode)

/* Indicate whether to power down TX on lane n */
#define PWR_DOWN_TX0 0x1
#define PWR_DOWN_TX1 0x2
#define PWR_DOWN_TX2 0x4
#define PWR_DOWN_TX3 0x8

/* Indicate whether to power down RX on lane n */
#define PWR_DOWN_RX0 0x100
#define PWR_DOWN_RX1 0x200
#define PWR_DOWN_RX2 0x400
#define PWR_DOWN_RX3 0x800

/* Indicate whether to power down TX on lane n */
#define CHANGE_TX0_PWRED_STATE 0x10
#define CHANGE_TX1_PWRED_STATE 0x20
#define CHANGE_TX2_PWRED_STATE 0x40
#define CHANGE_TX3_PWRED_STATE 0x80

/* Indicate whether to power down RX on lane n */
#define CHANGE_RX0_PWRED_STATE 0x1000
#define CHANGE_RX1_PWRED_STATE 0x2000
#define CHANGE_RX2_PWRED_STATE 0x4000
#define CHANGE_RX3_PWRED_STATE 0x8000


/* TX drive configuration  */

#define TSCMOD_NO_CFG_VALUE     (-1)
#define TXDRV_PREEMPH(_tdrv)  (((_tdrv).force << 15) | ((_tdrv).post << 10) | \
			((_tdrv).main << 4) | ((_tdrv).pre))

/* CL49 PRBS control */

#define TSCMOD_PRBS_TYPE_STD   0
#define TSCMOD_PRBS_TYPE_CL49  1
#define TSCMOD_PRBS_CFG_POLY31 3
#define TSCMOD_PRBS_CL49_POLY31  (PCS_IEEE2BLK_PCS_TPCONTROL_PRBS31TX_EN_MASK | \
                                PCS_IEEE2BLK_PCS_TPCONTROL_PRBS31RX_EN_MASK)
#define TSCMOD_PRBS_CL49_POLY9   (PCS_IEEE2BLK_PCS_TPCONTROL_PRBS9TX_EN_MASK | \
                                0x80)

/* typedefs */

/* index to TX drive configuration table for each VCO setting.
 */
typedef enum txdrv_inxs {
    TXDRV_6250_INX,     /* 6.26G, 3.125G 1.25G */
    TXDRV_103125_INX,   /* 10.3125G  */
    TXDRV_109375_INX,   /* 10.9375G  */
    TXDRV_12500_INX,    /* 12.5G  */
    TXDRV_11500_INX,    /* 11.5G, 6.25G 5.75G */
    TXDRV_XFI_INX,     /* 10G XFI */
    TXDRV_XLAUI_INX,   /* 40G XLAUI mode  */
    TXDRV_SFI_INX,     /* 10G SR fiber mode */
    TXDRV_SFIDAC_INX,  /* 10G SFI DAC mode */
    TXDRV_SR4_INX,     /* 40G SR4 */
    TXDRV_6GOS1_INX,   /* BRCM 40G mode */
    TXDRV_6GOS2_INX,   /*  */
    TXDRV_6GOS2_CX4_INX, /*  */
    TXDRV_AN_INX,      /* a common entry for autoneg mode */
    TXDRV_DFT_INX,     /* temp for any missing speed modes */
    TXDRV_ILKN,
    TXDRV_LAST_INX      /* always last */
} TXDRV_INXS_t;

#define TXDRV_ENTRY_NUM     (TXDRV_LAST_INX)

typedef struct 
{
    unsigned int force: 1; /* forced */
    unsigned int post : 5; /* TX post tap */
    unsigned int main : 6; /* TX main tap */
    unsigned int pre  : 4; /* TX pre tap */
    unsigned int rsvd : 16; /* reserved */
} TSCMOD_TX_TAP_t;


typedef struct {
   union {
      TSCMOD_TX_TAP_t tap;
      uint16 preemph;
   } u;
   union {
      TSCMOD_TX_TAP_t tap;
      uint16 preemph;
   } u_kx;
   union {
      TSCMOD_TX_TAP_t tap;
      uint16 preemph;
   } u_os;
   union {
      TSCMOD_TX_TAP_t tap;
      uint16 preemph;
   } u_br;
   union {
      TSCMOD_TX_TAP_t tap;
      uint16 preemph;
   } u_kr;
   union {
      TSCMOD_TX_TAP_t tap;
      uint16 preemph;
   } u_2p5;
  
   uint8 post2;   /* TX post2 coeff */
   uint8 idrive;  /* TX idrive  */ 
   uint8 ipredrive;  /* TX pre-idrive */
} TSCMOD_TX_DRIVE_t;


typedef enum tscmod_rx_los_states {
    RXLOS_RESET,
    RXLOS_LINK_WAIT_P1,
    RXLOS_INITIAL_LINK_UP,
    RXLOS_RESTART,
    RXLOS_LINK_WAIT_P2,
    RXLOS_LINK_UP
} TSCMOD_RX_LOS_STATES_t;

/* The structure is only for the use 
 * for RX LOS (Loss of Signal) workaround.
 * The workaround is to handle the situations
 * through s/w where no reliable LOS detect 
 * is available to WC 
 */ 
typedef struct {
   uint8  enable;
   uint8  sys_link;
   uint8  link_status;
   uint8  count ;
   uint8  rf_dis ;
   uint32 start_time  ;
   uint32 count_time  ;
   TSCMOD_RX_LOS_STATES_t state;
} TSCMOD_SOFTWARE_RX_LOS_t;

typedef struct {
    int preemph[MAX_NUM_LANES];   /* pre-emphasis */
    int idriver[MAX_NUM_LANES];
    int pdriver[MAX_NUM_LANES];
    int post2driver[MAX_NUM_LANES];
    int auto_medium;
    int fiber_pref;
    int medium;
    int sgmii_mstr;
    int pdetect10g;   /* 10G paralell detect */
    int pdetect1000x; /* 1000X paralell detect */
    int cx42hg;
    int rxlane_map[MAX_NUM_LANES/4];
    int txlane_map[MAX_NUM_LANES/4];
    int cl73an;
    int cl37an;
    int an_cl72;
    int init_mode ;
    int forced_init_cl72;
    int sw_init_drive; 
    int lane_mode;
    int pattern_data[PROG_DATA_NUM]; 
    int cx4_10g;
    int lane0_rst;
    int rxaui;
    int dxgxs;
    int custom;
    uint32 line_intf;
    int scrambler_en;
    int custom1;
    int txpol;
    int rxpol;
    int load_mthd;
    int load_ucode_done;
    int uc_cksum;
    int uc_ver ; 
    int uc_crc ; 
    int firmware_mode ; 
    int hg_mode;  /* choose HG speed mode */
    int refclk;
    int pll_divider;
    int init_speed ; 
    int oversample_mode; 
    TSCMOD_TX_DRIVE_t tx_drive[TXDRV_ENTRY_NUM];
    TSCMOD_SOFTWARE_RX_LOS_t sw_rx_los;
    int diag_regs ; 
    int diag_port ;
    int diag_verb ; 
    int linkup_bert ;
    int fault_disable ;
    int svk_id ; 
    uint32 los_usec ; 
    int an_ctl  ; 
    int an_link_fail_nocl72_wait_timer ;
    int pll_vco_step ;
    int ability_mask ;
    int ctrl_flag    ;
    int cidL ; 
    int cidH ; 
    int cidU ;
    int cidV ;
    mac_driver_t *macd;  /* Attached MAC driver */
    int am_reserved ;
    uint8 *scache_ptr ;
    uint32 scache_ver ;
    int    scache_size ;
    int    pcs_mode ;
    uint32 rxlos_link_wait_timer ;
    uint32 rxlos_restart_timer ;
    uint32 rxlos_first_linkup_time ;
} TSCMOD_DEV_CFG_t;

#define TSCMOD_LANE_NAME_LEN   30

typedef struct {
    uint16 serdes_id0;
    char   name[TSCMOD_LANE_NAME_LEN];
} TSCMOD_DEV_INFO_t;

typedef struct {
    uint32 start_time;
    uint32 state;
    uint32 cnt;
} TSCMOD_LINK_WAR;

typedef struct {
    uint32 start_time;
    uint32 state;
    uint8  tick_cnt ;
    uint8  mode ;
} TSCMOD_AN_TIMER;

typedef struct {
    int type;
    int poly;
} TSCMOD_PRBS_CTRL;

typedef struct {
    uint16 state;
    uint16 mode;
    uint16 enabled;
    uint16 tick_cnt;
    uint16 restart_cnt;
    uint32 start_time;
} TSCMOD_FORCE_CL72_t;

typedef struct {
   TSCMOD_LINK_WAR lkwa;
   TSCMOD_AN_TIMER an;
   /* TSCMOD_PRBS_CTRL prbs; */
   TSCMOD_FORCE_CL72_t cl72;
} TSCMOD_DEV_CTRL_t;

typedef struct {
    TSCMOD_DEV_CFG_t   cfg;
    TSCMOD_DEV_INFO_t  info;
    TSCMOD_DEV_CTRL_t  ctrl;
} TSCMOD_DEV_DESC_t;

#define DEV_CFG_PTR(_pc) (&(((TSCMOD_DEV_DESC_t *)((_pc) + 1))->cfg))
#define DEV_INFO_PTR(_pc) (&(((TSCMOD_DEV_DESC_t *)((_pc) + 1))->info))
#define DEV_CTRL_PTR(_pc) (&(((TSCMOD_DEV_DESC_t *)((_pc) + 1))->ctrl))

/* force cl72 */
#define FORCE_CL72_MODE(_pc)        (DEV_CTRL_PTR(_pc)->cl72.mode)
#define FORCE_CL72_ENABLED(_pc)     (DEV_CTRL_PTR(_pc)->cl72.enabled)
#define FORCE_CL72_STATE(_pc)       (DEV_CTRL_PTR(_pc)->cl72.state)
#define FORCE_CL72_TICK_CNT(_pc)    (DEV_CTRL_PTR(_pc)->cl72.tick_cnt)
#define FORCE_CL72_RESTART_CNT(_pc) (DEV_CTRL_PTR(_pc)->cl72.restart_cnt)
#define FORCE_CL72_START_TIME(_pc)  (DEV_CTRL_PTR(_pc)->cl72.start_time)

#define TSCMOD_FORCE_CL72_IDLE               0 
#define TSCMOD_FORCE_CL72_WAIT_TRAINING_LOCK 1 
#define TSCMOD_FORCE_CL72_WAIT_TRAINING_DONE 2
#define TSCMOD_FORCE_CL72_TXP_ENABLED        3
#define TSCMOD_FORCE_CL72_DONE               4
#define TSCMOD_FORCE_CL72_RESTART            5
#define TSCMOD_FORCE_CL72_LINK_DOWN          6
#define TSCMOD_FORCE_CL72_CMD_DIS_IDLE       7
#define TSCMOD_FORCE_CL72_RESTART_ACTION     8

#define TSCMOD_FORCE_CL72_RESTART_TIME       1000000
#define TSCMOD_FORCE_CL72_RESTART_CNT              6
#define TSCMOD_FORCE_CL72_RESTART_TXIDLE_TIME  75000
#define TSCMOD_FORCE_CL72_RESTART_TXIDLE_CNT       1

#define TSCMOD_RXLOS_TIMEOUT2           2000000
#define TSCMOD_RXLOS_TIMEOUT1           1000000
#define TSCMOD_RXLOS_TIMEOUTp5           500000
#define TSCMOD_RXLOS_TIMEOUTp25          250000
#define TSCMOD_RXLOS_TIMEOUTp1           100000
#define TSCMOD_RXLOS_TIMEOUTp2           200000
#define TSCMOD_RXLOS_LINK_FLAP_WAR          0x2
#define TSCMOD_RXLOS_LINK_UP_FAST_WAR       0x4
#define TSCMOD_INIT_MODE_SKIP           0x1

typedef struct {
    uint8 *pdata;
    int   *plen;
    uint16 chip_rev;
} TSCMOD_UCODE_DESC;

/* external functions shared within WC driver files  */
/* extern int phy_wc40_config_init(phy_ctrl_t *pc); */

#define DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_MASK  0xfc00 
#define DSC2B0_DSC_ANA_CTRL3_RESERVED_FOR_ECO0_SHIFT     10   
#define DSC2B0_DSC_ANA_CTRL0_RESERVED_FOR_ECO0_MASK  0x8000
#define DSC2B0_DSC_ANA_CTRL0_RESERVED_FOR_ECO0_SHIFT     15

/* (*_offset_p) and _size will accumulate by the sizeof(_var) */
#define TSCMOD_WB_ID_CMD(_ptr, _var, _t, _offset_p,_size_p)  \
        do {   \
            uint8  *_vptr_ ; \
            int     _vsize_ ; \
            _vptr_  = (_ptr) + (*(_offset_p)) ; \
            _vsize_ = sizeof((_var)) ;          \
            if((_t)==TSCMOD_WB_SC_UPDATE){              \
                sal_memcpy(_vptr_,(&(_var)),_vsize_) ;  \
            } else if((_t)==TSCMOD_WB_SC_RETRIEVE) {    \
                sal_memcpy((&(_var)),_vptr_,_vsize_);   \
            } \
            (*(_size_p))   = (*(_size_p)) + _vsize_ ;   \
            (*(_offset_p)) += _vsize_ ;                 \
        } while(0)

typedef enum tscmod_wb_handler_type {
    TSCMOD_WB_SC_COUNT,
    TSCMOD_WB_SC_RETRIEVE,
    TSCMOD_WB_SC_UPDATE
} TSCMOD_WB_HANDLER_TYPE_t ;

typedef enum tscmod_wb_data_id {
    TSCMOD_WB_SC_VER=0,
    TSCMOD_WB_FW_MODE,
    TSCMOD_WB_CTRL_TYPE,
    TSCMOD_WB_RXLOS,
    TSCMOD_WB_CL72_WAR,
    TSCMOD_WB_INTF,
    TSCMOD_WB_AN_MODE,
    TSCMOD_WB_SC_LAST 
} TSCMOD_WB_DATA_ID_t ;

#define TSCMOD_WB_SC_VER1    0x6401
#define TSCMOD_WB_SC_VER2    0x6402


#endif /* WL_TSCMOD_H__ */
