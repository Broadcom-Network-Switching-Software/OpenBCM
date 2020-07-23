/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wc40_extra.h
 * Purpose:     non-register Macro, data structures and function prototypes for WC driver. 
 */


#ifndef WCMOD_EXTRA_H__
#define WCMOD_EXTRA_H__

/*
 * wc40.h has definitions related to WC registers. It is automatically generated 
 * from RDB files. This file has all non-register Macro, data structures and 
 * function prototypes the WC driver.
 */

/* additional missed register definitions */

#define WCMOD_XGXSBLK0_XGXSSTATUSr      0x8001
#define WCMOD_TX0_ANATXACONTROL0r       0x8061
#define WCMOD_RX0_ANARXCONTROLPCIr      0x80ba
#define WCMOD_COMBO_IEEE0_MIICNTLr      0xffe0
#define WCMOD_MICROBLK_DOWNLOAD_STATUSr 0xffc5
#define SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI  0x25
#define SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI  0x29

/* misc. */
#define WCMOD_PLL_WAIT   400000   /* 400ms */
#define WCMOD_AN_WAIT   400000   /* 400ms */
#define TX_DRIVER_DFT_LN_CTRL   0x7fff

#define SERDES_ID0_MODEL_NUMBER_WARPCORE 0x9   
#define WC_AER_BCST_OFS_STRAP   0x1ff
#define OVER1G_UP3_20GPLUS_MASK (DIGITAL3_UP3_DATARATE_25P45GX4_MASK | \
                                 DIGITAL3_UP3_DATARATE_21GX4_MASK | \
                                 DIGITAL3_UP3_DATARATE_31P5G_MASK | \
                                 DIGITAL3_UP3_DATARATE_40G_MASK | \
                                 DIGITAL3_UP3_LAST_MASK)
#define PLL_PLLACONTROL4_BIT12_MASK         0x1000
#define WCMOD_UCODE_MAX_LEN   (32*1024)
#define NUM_LANES             4
#define MAX_NUM_LANES         24

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

/* WC revision IDs */

#define WCMOD_SERDES_ID0_REVID_A0    0x0000
#define WCMOD_SERDES_ID0_REVID_A1    0x0800
#define WCMOD_SERDES_ID0_REVID_A2    0x1000
#define WCMOD_SERDES_ID0_REVID_B0    0x4000
#define WCMOD_SERDES_ID0_REVID_C0    0x8000
#define WCMOD_SERDES_ID0_REVID_D0    0xc000

#define REV_VER_MASK (SERDESID_SERDESID0_REV_LETTER_MASK | \
                         SERDESID_SERDESID0_REV_NUMBER_MASK)
#define WCMOD_REVID(_pc) \
     ((((WCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK)
#define WCMOD_REVID_A0(_pc) \
     (((((WCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WCMOD_SERDES_ID0_REVID_A0)
#define WCMOD_REVID_A1(_pc) \
     (((((WCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WCMOD_SERDES_ID0_REVID_A1)
#define WCMOD_REVID_B0(_pc) \
     (((((WCMOD_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WCMOD_SERDES_ID0_REVID_B0)
#define WCMOD_LINK_WAR_REVS(_pc) (WCMOD_REVID_A0(_pc) || WCMOD_REVID_A1(_pc))

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

#define WCMOD_OS5_VCO_FREQ  0x7200
#define WCMOD_OS8_VCO_FERQ  0x7A00
#define WCMOD_TXCK_MASK     0x1E0
#define WCMOD_RXCK_OVERIDE_MASK  (1 << 11)
#define WCMOD_TXCK_SHIFTER  5

/* WCMOD_CL73_KR2 is the HP cl73 KR2 autoneg mode. In this mode
 * CL37,CL37BAM and CL73BAM is disabled. The autoneg is in software
 * controlled mode.
 */
#define WCMOD_CL73_KR2_CUSTOM   4
#define WCMOD_CL73_KR2          3
#define WCMOD_CL73_WO_CL73BAM   2
#define WCMOD_CL73_AND_CL73BAM  1

#define WCMOD_CL37_WO_CL37BAM   2
#define WCMOD_CL37_AND_CL37BAM  1
/* lane swap functions in XGXSBLK2 are obsolete, use ones in XGXSBLK8
 * i.e. define WCMOD_PCS_LANE_SWAP
 */
#define WCMOD_PCS_LANE_SWAP
#ifdef WCMOD_PCS_LANE_SWAP
/* For Tx lane swap
 * bits 15-12 11-8 7-4 3-0  represents pma/pmd lane 3,2,1,0
 * value in each field represents corresponding pcs lane number.
 * the default map is pma/pmd lane3 maps to pcs lane3, pma lane2
 * to pcs lane2 and so on.
 *
 * For Rx lane swap
 * The same bit fields represents the pcs lane 3,2,1,0
 * and the value corresponds the pma/pmd lane number.
 */
#define WCMOD_RX_LANE_MAP_DEFAULT    0x3210
#define WCMOD_TX_LANE_MAP_DEFAULT    0x3210
#else
#define WCMOD_RX_LANE_MAP_DEFAULT    0x0123
#define WCMOD_TX_LANE_MAP_DEFAULT    0x0123
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
#define IND_LANE_MODE(_mode) (((_mode)==xgxs_operationModes_IndLane_OS8) || \
         ((_mode)==xgxs_operationModes_IndLane_OS5))
#define COMBO_LANE_MODE(_mode) ((_mode) == xgxs_operationModes_ComboCoreMode)
#define XENIA_MODE(_mode) ((_mode) == xgxs_operationModes_XGXS)
#define IS_DUAL_LANE_PORT(_pc) (((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) || \
                                CUSTOM1_MODE(_pc))
#define IS_MULTI_CORE_PORT(_pc) ((_pc)->phy_mode == PHYCTRL_MULTI_CORE_PORT) 

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

#define WCMOD_MEM_ALLOC(_size,_name) sal_alloc(_size,_name)
#define WCMOD_MEM_SET(_dst,_val,_len) sal_memset(_dst,_val,_len)
#define WCMOD_UDELAY(_time) sal_udelay(_time)
#define WCMOD_USLEEP(_time) sal_usleep(_time)

#define FX100_CONTROL1_AUTODET_EN_MASK 4
#define WCMOD_IF_NULL   (1 << SOC_PORT_IF_NULL)
#define WCMOD_IF_SR     (1 << SOC_PORT_IF_SR)
#define WCMOD_IF_KR     (1 << SOC_PORT_IF_KR)
#define WCMOD_IF_KR4    (1 << SOC_PORT_IF_KR4)
#define WCMOD_IF_CR     (1 << SOC_PORT_IF_CR)
#define WCMOD_IF_CR4    (1 << SOC_PORT_IF_CR4)
#define WCMOD_IF_XFI    (1 << SOC_PORT_IF_XFI)
#define WCMOD_IF_SFI    (1 << SOC_PORT_IF_SFI)
#define WCMOD_IF_XLAUI  (1 << SOC_PORT_IF_XLAUI)
#define WCMOD_IF_XGMII  (1 << SOC_PORT_IF_XGMII)
#define WCMOD_IF_ILKN   (1 << SOC_PORT_IF_ILKN)
#define WCMOD_40G_10G_INTF(_if)  ((_if) == SOC_PORT_IF_KR4 || (_if) == SOC_PORT_IF_XLAUI || \
                             (_if) == SOC_PORT_IF_CR4 || (_if) == SOC_PORT_IF_SR || \
                             (_if) == SOC_PORT_IF_KR || (_if) == SOC_PORT_IF_CR || \
                             (_if) == SOC_PORT_IF_XFI || (_if) == SOC_PORT_IF_SFI || \
                              (_if) == SOC_PORT_IF_XGMII)

#define WCMOD_40G_10G_INTF_ALL   (WCMOD_IF_KR4 | WCMOD_IF_XLAUI | WCMOD_IF_CR4 | WCMOD_IF_SR | \
                        WCMOD_IF_KR | WCMOD_IF_CR | WCMOD_IF_XFI | WCMOD_IF_SFI| WCMOD_IF_XGMII)

#define WCMOD_UC_CTRL_OPT_PF_TEMP_COMP 0x8888
#define WCMOD_UC_CTRL_FORCE_OSDFE  0x4
#define WCMOD_UC_CTRL_CL72_WO_AN 0x7777
#define WCMOD_UC_CTRL_XLAUI   0x3333
#define WCMOD_UC_CTRL_SR4     0x1111
#define WCMOD_UC_CTRL_SFP_DAC 0x2
#define WCMOD_UC_CTRL_DEFAULT 0x0000


#define WCMOD_AN_WAIT_INTERVAL 1000000  /* 1 second */

#define WCMOD_SP_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.start_time)
#define WCMOD_SP_VALID(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state)
#define WCMOD_SP_CNT(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt)

#define WCMOD_SP_RECORD_TIME(_pc)  \
        (DEV_CTRL_PTR(_pc)->lkwa.start_time) = sal_time_usecs()
#define WCMOD_SP_CNT_INC(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt)++
#define WCMOD_SP_CNT_CLEAR(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt) = 0
#define WCMOD_SP_VALID_SET(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state) = TRUE
#define WCMOD_SP_VALID_RESET(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state) = FALSE

#define WCMOD_AN_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->an.start_time)
#define WCMOD_AN_VALID(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state)
#define WCMOD_AN_RECORD_TIME(_pc)  \
        (DEV_CTRL_PTR(_pc)->an.start_time) = sal_time_usecs()
#define WCMOD_AN_VALID_SET(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state) = TRUE
#define WCMOD_AN_VALID_RESET(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state) = FALSE
#define WCMOD_AN_CHECK_TIMEOUT(_pc) \
        ((uint32)SAL_USECS_SUB(sal_time_usecs(), WCMOD_AN_TIME((_pc))) \
                >= WCMOD_AN_WAIT_INTERVAL)
#define WCMOD_CL73_SOFT_KR2(_pc)  ((DEV_CFG_PTR(_pc)->cl73an == WCMOD_CL73_KR2) || \
                                   (DEV_CFG_PTR(_pc)->cl73an == WCMOD_CL73_KR2_CUSTOM) )
/* TX drive configuration  */

#define WCMOD_NO_CFG_VALUE     (-1)
#define TXDRV_PREEMPH(_tdrv)  (((_tdrv).force << 15) | ((_tdrv).post << 10) | \
			((_tdrv).main << 4) | ((_tdrv).pre))

/* CL49 PRBS control */

#define WCMOD_PRBS_TYPE_STD   0
#define WCMOD_PRBS_TYPE_CL49  1
#define WCMOD_PRBS_CFG_POLY31 3
#define WCMOD_PRBS_CL49_POLY31  (PCS_IEEE2BLK_PCS_TPCONTROL_PRBS31TX_EN_MASK | \
                                PCS_IEEE2BLK_PCS_TPCONTROL_PRBS31RX_EN_MASK)
#define WCMOD_PRBS_CL49_POLY9   (PCS_IEEE2BLK_PCS_TPCONTROL_PRBS9TX_EN_MASK | \
                                0x80)

/* typedefs */
/* CL72 in force mode */
#define WCMOD_CL72_STATE_LINK_GOOD          0
#define WCMOD_CL72_STATE_WAIT_FOR_LINK      1
#define WCMOD_CL72_STATE_RESTART_PMD        2
#define WCMOD_CL72_LINK_WAIT_CNT_LIMIT      6

/*serdes_rx_los defines */
#define WCMOD_RX_LOS_SW_ENABLE             1
#define WCMOD_RX_LOS_EXT_PIN_ENABLE        2
#define WCMOD_RXLOS_LINK_FLAP_WAR          0x4


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
} WCMOD_TX_TAP_t;

typedef enum sw_rx_los_states {
    RESET,
    INITIAL_LINK,
    LINK,
    START_TIMER,
    RX_RESTART,
    IDLE
} SOFTWARE_RX_LOS_STATES_t;

/* The structure is only for the use 
 * for RX LOS (Loss of Signal) workaround.
 * The workaround is to handle the situations
 * through s/w where no reliable LOS detect 
 * is available to WC 
 */ 
typedef struct {
    uint8 enable;
    uint8 sys_link;
    uint8 link_status;
    uint8 timer_count;
    uint8 rf_dis; 
    SOFTWARE_RX_LOS_STATES_t state;
} WCMOD_SOFTWARE_RX_LOS_t;



typedef struct {
    union {
        WCMOD_TX_TAP_t tap;
        uint16 preemph;
    } u;
    uint8 post2;   /* TX post2 coeff */
    uint8 idrive;  /* TX idrive  */ 
    uint8 ipredrive;  /* TX pre-idrive */
} WCMOD_TX_DRIVE_t;

typedef struct {
    int preemph[MAX_NUM_LANES];   /* pre-emphasis */
    int idriver[MAX_NUM_LANES];
    int pdriver[MAX_NUM_LANES];
    int post2driver[MAX_NUM_LANES];
    int mld_lane_swap[48];
    int auto_medium;
    int fiber_pref;
    int sgmii_mstr;
    int pdetect10g;   /* 10G paralell detect */
    int pdetect1000x; /* 1000X paralell detect */
    int cx42hg;
    int rxlane_map[MAX_NUM_LANES/4];
    int txlane_map[MAX_NUM_LANES/4];
    int cl73an;
    int cl37an;
    int cl72;
    int forced_init_cl72;
    int lane_mode;
    int cx4_10g;
    int lane0_rst;
    int rxaui;
    int rxaui_mode;
    int dxgxs;
    int custom;
    uint32 line_intf;
    int scrambler_en;
    int custom1;
    int txpol;
    int rxpol;
    int load_mthd;
    int uc_cksum;
    int medium;
    int hg_mode;  /* choose HG speed mode */
    int refclk;
    int vco_freq;
    int pll_divider;
    int oversample_mode; 
    int pcs_bypass; 
    int firmware_mode;
    int speed; 
    int max_speed; 
    int phy_passthru;
    int phy_sgmii_an;
    int asymmetric_mode;   /*for xenia core mixed 1G/2.5G */
    int rx_los_ext_enable;  /*rx_los_external pin enable */
    int linkscan_interval;
    int active_lane_map;
    int serdes_1000x_at_6250_vco;
    mac_driver_t *macd;  /* Attached MAC driver */
    WCMOD_TX_DRIVE_t tx_drive[TXDRV_ENTRY_NUM];
    WCMOD_SOFTWARE_RX_LOS_t sw_rx_los;
    int mld_war_timer_count;
} WCMOD_DEV_CFG_t;

#define WCMOD_LANE_NAME_LEN   30

typedef struct {
    uint16 serdes_id0;
    char   name[WCMOD_LANE_NAME_LEN];
} WCMOD_DEV_INFO_t;

typedef struct {
    uint32 start_time;
    uint32 state;
    uint32 cnt;
} WCMOD_LINK_WAR;

typedef struct {
    uint32 start_time;
    uint32 state;
} WCMOD_AN_MODE;

typedef struct {
    int type;
    int poly;
} WCMOD_PRBS_CTRL;

typedef struct {
    uint16 state;
    uint16 enabled;
    uint16 tick_cnt;
    uint16 restart_cnt;
} WCMOD_FORCE_CL72_t;

typedef struct {
    WCMOD_LINK_WAR lkwa;
    WCMOD_AN_MODE an;
    int clk_comp;
    WCMOD_PRBS_CTRL prbs;
    WCMOD_FORCE_CL72_t cl72;
} WCMOD_DEV_CTRL_t;

typedef struct {
    WCMOD_DEV_CFG_t   cfg;
    WCMOD_DEV_INFO_t  info;
    WCMOD_DEV_CTRL_t  ctrl;
} WCMOD_DEV_DESC_t;

#define DEV_CFG_PTR(_pc) (&(((WCMOD_DEV_DESC_t *)((_pc) + 1))->cfg))
#define DEV_INFO_PTR(_pc) (&(((WCMOD_DEV_DESC_t *)((_pc) + 1))->info))
#define DEV_CTRL_PTR(_pc) (&(((WCMOD_DEV_DESC_t *)((_pc) + 1))->ctrl))

/* force cl72 */
#define FORCE_CL72_ENABLED(_pc) (DEV_CTRL_PTR(_pc)->cl72.enabled)
#define FORCE_CL72_STATE(_pc) (DEV_CTRL_PTR(_pc)->cl72.state)
#define FORCE_CL72_TICK_CNT(_pc) (DEV_CTRL_PTR(_pc)->cl72.tick_cnt)
#define FORCE_CL72_RESTART_CNT(_pc) (DEV_CTRL_PTR(_pc)->cl72.restart_cnt)


typedef struct {
    uint8 *pdata;
    int   *plen;
    uint16 chip_rev;
} WCMOD_UCODE_DESC;

/* external functions shared within WC driver files  */
/* extern int phy_wc40_config_init(phy_ctrl_t *pc); */


#endif /* WL_WCMOD_H__ */
