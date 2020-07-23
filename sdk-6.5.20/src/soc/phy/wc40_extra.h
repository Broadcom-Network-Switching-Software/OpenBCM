/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        wc40_extra.h
 * Purpose:     non-register Macro, data structures and function prototypes for WC driver. 
 */

#ifndef WC40_EXTRA_H__
#define WC40_EXTRA_H__

/*
 * wc40.h has definitions related to WC registers. It is automatically generated 
 * from RDB files. This file has all non-register Macro, data structures and 
 * function prototypes the WC driver.
 */

/* additional missed register definitions */

#define WC40_XGXSBLK0_XGXSSTATUSr      0x8001
#define WC40_TX0_ANATXACONTROL0r       0x8061
#define WC40_RX0_ANARXCONTROLPCIr      0x80ba
#define WC40_COMBO_IEEE0_MIICNTLr      0xffe0
#define WC40_MICROBLK_DOWNLOAD_STATUSr 0xffc5
#define SERDESDIGITAL_FORCE_SPEED_dr_10G_XFI  0x25
#define SERDESDIGITAL_FORCE_SPEED_dr_10G_SFI  0x29
#define XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_KR2         0x39
#define XGXSBLK5_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_CR2         0x3a
#define DIGITAL4_MISC4_PCS_TYPE_AUTO_SELECT_MASK         0x8000
#define WC40_PER_LANE_TX_DRIVERr(lane) (0x8067 + (0x10*lane))

/* misc. */

#define WC40_PLL_WAIT  250000
#define WC40_AN_WAIT   400000   /* 400ms */
#define TX_DRIVER_DFT_LN_CTRL   0x7fff

#define SERDES_ID0_MODEL_NUMBER_WARPCORE 0x9
#define WC_AER_BCST_OFS_STRAP   0x1ff
#define OVER1G_UP3_20GPLUS_MASK (DIGITAL3_UP3_DATARATE_25P45GX4_MASK | \
                                 DIGITAL3_UP3_DATARATE_21GX4_MASK | \
                                 DIGITAL3_UP3_DATARATE_31P5G_MASK | \
                                 DIGITAL3_UP3_DATARATE_40G_MASK | \
                                 DIGITAL3_UP3_LAST_MASK)
#define PLL_PLLACONTROL4_BIT12_MASK         0x1000
#define WC40_UCODE_MAX_LEN   (32*1024)
#define NUM_LANES             4
#define SERDESDIGITAL_MISC1_FORCE_SPEED_dr_40G_KR4   0x11

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

#define WC40_SERDES_ID0_REVID_A0    0x0000
#define WC40_SERDES_ID0_REVID_A1    0x0800
#define WC40_SERDES_ID0_REVID_A2    0x1000
#define WC40_SERDES_ID0_REVID_A3    0x1800
#define WC40_SERDES_ID0_REVID_A4    0x2000
#define WC40_SERDES_ID0_REVID_B0    0x4000
#define WC40_SERDES_ID0_REVID_B1    0x4800
#define WC40_SERDES_ID0_REVID_C0    0x8000
#define WC40_SERDES_ID0_REVID_D0    0xc000

#define REV_VER_MASK (SERDESID_SERDESID0_REV_LETTER_MASK | \
                         SERDESID_SERDESID0_REV_NUMBER_MASK)
#define WC40_REVID(_pc) \
     ((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK)
#define WC40_REVID_A0(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_A0)
#define WC40_REVID_A1(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_A1)
#define WC40_REVID_A2(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_A2)
#define WC40_REVID_A3(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_A3)
#define WC40_REVID_A4(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_A4)

#define WC40_REVID_B0(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_B0)
#define WC40_REVID_B1(_pc) \
     (((((WC40_DEV_DESC_t *)((_pc) + 1))->info.serdes_id0) & REV_VER_MASK) == \
        WC40_SERDES_ID0_REVID_B1)
#define WC40_REVID_B(_pc) (WC40_REVID_B0(_pc) || WC40_REVID_B1(_pc))

#define WC40_LINK_WAR_REVS(_pc) (WC40_REVID_A0(_pc) || WC40_REVID_A1(_pc))

#define WC40_REVID_A(_pc) (WC40_REVID_A0(_pc) || WC40_REVID_A1(_pc) \
                           || WC40_REVID_A2(_pc) || WC40_REVID_A3(_pc) \
                           || WC40_REVID_A2(_pc))

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
#define DIGITAL6_UD_FIELD_UD_ADV_ID_MASK   (3 << 9)
#define DIGITAL6_UD_FIELD_UD_ADV_ID_WC     (1 << 10)

/* VCO frequency ctrl */

#define WC40_OS5_VCO_FREQ  0x7200
#define WC40_OS8_VCO_FERQ  0x7A00
#define WC40_TXCK_MASK     0x1E0
#define WC40_RXCK_OVERIDE_MASK  (1 << 11)
#define WC40_TXCK_SHIFTER  5

/* WC40_CL73_KR2 is the cl73 KR2 autoneg mode. In this mode
 * CL37,CL37BAM and CL73BAM is disabled. The autoneg is in software
 * controlled mode.
 *
 * WC40_CL73_KR2_CUSTOM_WAR: 
 * To workaroud the interop issue with legacy devices
 * such as 8073. 8073 goes into lockup state if 
 * a base page with next page (KR2_CUSTOM) bit set 
 * is sent to it. It is a WAR placed on top of
 * WC40_CL73_KR2_CUSTOM implementation of SOFT KR2.
 * Similarly, WC40_CL73_KR2_WAR is for WC40_CL73_KR2.
 *
 * STATE_TIMER & STATE_WAIT_4_BP are added under this WAR
 * STATE_TIMER: It starts the timer to squelch the tx for 
 * a fixed amount of time.
 * STATE_WAIT_4_BP: It handles the base page received
 * following the release of tx squelch. Drops it on 
 * the floor since h/w resolves the speed in that state.
 */
#define WC40_CL73_KR2_CUSTOM_WAR  6
#define WC40_CL73_KR2_WAR         5
#define WC40_CL73_KR2_CUSTOM   4
#define WC40_CL73_KR2          3
#define WC40_CL73_WO_CL73BAM   2
#define WC40_CL73_AND_CL73BAM  1

#define WC40_CL37_WO_CL37BAM   2
#define WC40_CL37_AND_CL37BAM  1

/* KR2 control */
#define WC40_SOFT_KR2_STATE_INIT        0
#define WC40_SOFT_KR2_STATE_BASE_PAGE   1
#define WC40_SOFT_KR2_STATE_MP5         2
#define WC40_SOFT_KR2_STATE_UP1         3
#define WC40_SOFT_KR2_STATE_DONE        4
#define WC40_SOFT_KR2_STATE_TIMER       5
#define WC40_SOFT_KR2_STATE_WAIT_4_BP   6
#define WC40_SOFT_KR2_STATE_WAIT_4_NP   7

#define WC40_AUTONEG_MSG_MP5  0x5
#define WC40_AUTONEG_MSG_NULL 0x1
#define WC40_BRCM_OUI0        0
#define WC40_BRCM_OUI1        0x2bd
#define WC40_BRCM_CODE_BAM3   0x603
#define WC40_BRCM_ETA_KR2     0x1

/* lane swap functions in XGXSBLK2 are obsolete, use ones in XGXSBLK8
 * i.e. define WC40_PCS_LANE_SWAP
 */
#define WC40_PCS_LANE_SWAP
#ifdef WC40_PCS_LANE_SWAP
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
#define WC40_RX_LANE_MAP_DEFAULT    0x3210
#define WC40_TX_LANE_MAP_DEFAULT    0x3210
#else
#define WC40_RX_LANE_MAP_DEFAULT    0x0123
#define WC40_TX_LANE_MAP_DEFAULT    0x0123
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
#define IND_LANE_MODE(_mode) (((_mode)==xgxs_operationModes_Indlane_OS8) || \
         ((_mode)==xgxs_operationModes_IndLane_OS5))
#define COMBO_LANE_MODE(_mode) ((_mode) == xgxs_operationModes_ComboCoreMode)
#define IS_SINGLE_LANE_IN_DUAL_PORT_MODE(_pc) ((_pc)->phy_mode == PHYCTRL_SINGLE_LANE_IN_DUAL_PORT_MODE)
#define IS_DUAL_LANE_PORT(_pc) (((_pc)->phy_mode == PHYCTRL_DUAL_LANE_PORT) || \
                                CUSTOM1_MODE(_pc))
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

#define WC40_REG_READ(_unit, _pc, _flags, _reg_addr, _val) \
        phy_wc40_reg_aer_read((_unit), (_pc),(_flags), (_reg_addr), (_val))

#define WC40_REG_WRITE(_unit, _pc, _flags, _reg_addr, _val) \
        phy_wc40_reg_aer_write((_unit), (_pc),(_flags), (_reg_addr), (_val))

#define WC40_REG_MODIFY(_unit, _pc, _flags, _reg_addr, _val, _mask) \
        phy_wc40_reg_aer_modify((_unit), (_pc),(_flags), (_reg_addr), (_val),(_mask))

#define WC40_MEM_ALLOC(_size,_name) sal_alloc(_size,_name)
#define WC40_MEM_SET(_dst,_val,_len) sal_memset(_dst,_val,_len)
#define WC40_UDELAY(_time) sal_udelay(_time)
#define WC40_USLEEP(_time) sal_usleep(_time)

#define FX100_CONTROL1_AUTODET_EN_MASK 4
#define WC40_IF_SR     (1 << SOC_PORT_IF_SR)
#define WC40_IF_KR     (1 << SOC_PORT_IF_KR)
#define WC40_IF_KR4    (1 << SOC_PORT_IF_KR4)
#define WC40_IF_CR     (1 << SOC_PORT_IF_CR)
#define WC40_IF_CR4    (1 << SOC_PORT_IF_CR4)
#define WC40_IF_XFI    (1 << SOC_PORT_IF_XFI)
#define WC40_IF_SFI    (1 << SOC_PORT_IF_SFI)
#define WC40_IF_XLAUI  (1 << SOC_PORT_IF_XLAUI)
#define WC40_IF_XGMII  (1 << SOC_PORT_IF_XGMII)


#define WC40_40G_10G_INTF(_if)  ((_if) == SOC_PORT_IF_KR4 || (_if) == SOC_PORT_IF_XLAUI || \
                             (_if) == SOC_PORT_IF_CR4 || (_if) == SOC_PORT_IF_SR || \
                             (_if) == SOC_PORT_IF_KR || (_if) == SOC_PORT_IF_CR || \
                             (_if) == SOC_PORT_IF_XFI || (_if) == SOC_PORT_IF_SFI || \
                              (_if) == SOC_PORT_IF_XGMII)

#define WC40_40G_10G_INTF_ALL   (WC40_IF_KR4 | WC40_IF_XLAUI | WC40_IF_CR4 | WC40_IF_SR | \
                        WC40_IF_KR | WC40_IF_CR | WC40_IF_XFI | WC40_IF_SFI | WC40_IF_XGMII)

#define WC40_UC_CTRL_OPT_PF_TEMP_COMP 0x8888
#define WC40_UC_CTRL_OPT_PF_TEMP_COMP_PER_LANE 0x8
#define WC40_UC_CTRL_CL72_WO_AN 0x7777
#define WC40_UC_CTRL_XLAUI   0x3333
#define WC40_UC_CTRL_SR4     0x1111
#define WC40_UC_CTRL_SFP_DAC 0x2
#define WC40_UC_CTRL_DEFAULT 0x0000

#define WC40_AN_WAIT_INTERVAL 1000000  /* 1 second */

#define WC40_SP_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.start_time)
#define WC40_SP_VALID(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state)
#define WC40_SP_CNT(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt)

#define WC40_SP_RECORD_TIME(_pc)  \
        (DEV_CTRL_PTR(_pc)->lkwa.start_time) = sal_time_usecs()
#define WC40_SP_CNT_INC(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt)++
#define WC40_SP_CNT_CLEAR(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.cnt) = 0
#define WC40_SP_VALID_SET(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state) = TRUE
#define WC40_SP_VALID_RESET(_pc) \
        (DEV_CTRL_PTR(_pc)->lkwa.state) = FALSE

#define WC40_AN_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->an.start_time)
#define WC40_AN_VALID(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state)
#define WC40_AN_RECORD_TIME(_pc)  \
        (DEV_CTRL_PTR(_pc)->an.start_time) = sal_time_usecs()
#define WC40_AN_VALID_SET(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state) = TRUE
#define WC40_AN_VALID_RESET(_pc) \
        (DEV_CTRL_PTR(_pc)->an.state) = FALSE
#define WC40_AN_CHECK_TIMEOUT(_pc) \
        ((uint32)SAL_USECS_SUB(sal_time_usecs(), WC40_AN_TIME((_pc))) \
                >= WC40_AN_WAIT_INTERVAL)

/* TX drive configuration  */

#define WC40_NO_CFG_VALUE     (-1)
#define TXDRV_PREEMPH(_tdrv)  (((_tdrv).force << 15) | ((_tdrv).post << 10) | \
			((_tdrv).main << 4) | ((_tdrv).pre))

/* CL49 PRBS control */

#define WC40_PRBS_TYPE_STD   0
#define WC40_PRBS_TYPE_CL49  1
#define WC40_PRBS_CFG_POLY31 3
#define WC40_PRBS_CL49_POLY31  (PCS_IEEE2BLK_PCS_TPCONTROL_PRBS31TX_EN_MASK | \
                                PCS_IEEE2BLK_PCS_TPCONTROL_PRBS31RX_EN_MASK)
#define WC40_PRBS_CL49_POLY9   (PCS_IEEE2BLK_PCS_TPCONTROL_PRBS9TX_EN_MASK | \
                                0x80)

/* CL72 in force mode */
#define WC40_CL72_STATE_TX_DISABLED        0
#define WC40_CL72_STATE_INIT_CL72          1
#define WC40_CL72_STATE_LINK_CHECK         2
#define WC40_CL72_STATE_LINK_GOOD          3
#define WC40_CL72_LINK_WAIT_CNT_LIMIT      6
 
/* CL72 in force mode (firmware) */
#define WC40_FORCED_CL72_STATE_RESET         1
#define WC40_FORCED_CL72_STATE_WAIT_4_LINK   2
#define WC40_FORCED_CL72_STATE_LINK_GOOD     3
#define WC40_FORCED_CL72_LINK_WAIT_CNT_LIMIT 6
/* typedefs */

/* index to TX drive configuration table for each PMD type.
 */
typedef enum txdrv_inxs {
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
    TXDRV_LAST_INX     /* always last */
} TXDRV_INXS_t;

#define TXDRV_ENTRY_NUM     (TXDRV_LAST_INX)

typedef struct 
{
    unsigned int force: 1; /* forced */
    unsigned int post : 5; /* TX post tap */
    unsigned int main : 6; /* TX main tap */
    unsigned int pre  : 4; /* TX pre tap */
    unsigned int rsvd : 16; /* reserved */
} WC40_TX_TAP_t;

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
    uint8 fault_report_dis;
    SOFTWARE_RX_LOS_STATES_t state;
} WC40_SOFTWARE_RX_LOS_t;



typedef struct {
    union {
        WC40_TX_TAP_t tap;
        uint16 preemph;
    } u;
    uint8 post2;   /* TX post2 coeff */
    uint8 idrive;  /* TX idrive  */ 
    uint8 ipredrive;  /* TX pre-idrive */
} WC40_TX_DRIVE_t;

typedef struct {
    /* Enable the cl73 fsm auto 
       recovery feature */
    uint8 enable;

    /* Store FSM previous state */
	uint16 fsm_state_prev; 
   
    /* Tracks how many times we've seen the
	   port in the particular state. */
	int fsm_state_count;

	/* Statistics counter for how many times
	   the patch recovery action was taken
	   for each of the FSM states. */
	uint32 action_count_state02;
	uint32 action_count_state04;
	uint32 action_count_state08;
	uint32 action_count_state10;
	uint32 action_count_state20;
} WC40_CL73_AUTO_RECOVER_t;

/* WC40_CL73_AUTO_RECOVER feature addresses a link issue 
   found with WarpCore connected to a certain Broadcom PHY, 
   primarily BCM8073. Following autoneg FSM states are 
   known to be symptoms of the issue:
       0x002: Transmit Disable
       0x004: Ability Detect
       0x008: Acknowledge
       0x010: Acknowledge Complete
       0x020: Next Page Wait

   All states except 0x008 has some dependencies on the
   Link Partner (LP); therefore, we allow more time for
   a port to be in those states.  These constants define
   how many times we're allowed to see these FSM states
   before taking recovery action.
 */
#define CL73_AUTO_RECOVER_STATE08_WAIT_COUNT	1
#define CL73_AUTO_RECOVER_LP_STATE_WAIT_COUNT	8

/*
  In f/w ver 0103 war_tx_os8_fifo_reset routine is added. 
  The purpose of this routine is to ensure that the txfifo 
  (in OS8.25 mode) gets a reset when the clocks are stable, 
  and while the reset cannot cause a link flap in the LP.

  This routine is the f/w WAR that performs a targeted reset 
  of the tx_os8_fifo  whenever, the CL73 Arbitration FSM 
  enters either the TX_DISABLE or the AN_GOOD_CHECK states. 
  Only perform WAR routine when mode10g = 0x4 (i.e. Independent 
  0s8.25 channel mode), AND when CL73 AN is enabled.
 
  'cl73an_tx_fifo_reset_enable' field is used manage/setup
  the required condition for the f/w war.

  Watch out for register access of these registers as these
  are also used by the war when active.
  0x8061, 0x8378, 0x8345
*/
typedef enum cl73an_tx_fifo_reset_status{
    DISABLE,
    ENABLE,
    ACTIVE,
    REACTIVATE
} CL73AN_TX_FIFO_RESET_STATUS_t;

typedef struct {
    int preemph[NUM_LANES];   /* pre-emphasis */
    int idriver[NUM_LANES];
    int pdriver[NUM_LANES];
    int post2driver[NUM_LANES];
    int auto_medium;
    int fiber_pref;
    int sgmii_mstr;
    int pdetect10g;   /* 10G paralell detect */
    int pdetect1000x; /* 1000X paralell detect */
    int cx42hg;
    int rxlane_map;
    int txlane_map;
    int cl73an;
    int cl37an;
    int lane_mode;
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
    int uc_cksum;

    int medium;
    int hg_mode;  /* choose HG speed mode */
    int refclk; 
    int rx_los;
    int rx_los_invert;
    int phy_passthru;
    WC40_TX_DRIVE_t tx_drive[TXDRV_ENTRY_NUM];
    WC40_SOFTWARE_RX_LOS_t sw_rx_los;
    WC40_CL73_AUTO_RECOVER_t cl73_fsm_auto_recover;
    int force_cl72_fw;   /* Select firmware based forced cl72 implementation */
    int force_cl72;      /* Enable/disable forced cl72 */
    int fw_dfe;
    mac_driver_t *macd;  /* Attached MAC driver */
    CL73AN_TX_FIFO_RESET_STATUS_t cl73an_tx_fifo_reset_status;   
    uint8 *scache_ptr ;
    uint16 scache_ver ;
    int    scache_size ;
} WC40_DEV_CFG_t;

#define WC40_LANE_NAME_LEN   30

typedef struct {
    uint16 serdes_id0;
    char   name[WC40_LANE_NAME_LEN];
} WC40_DEV_INFO_t;

typedef struct {
    uint32 start_time;
    uint32 state;
    uint32 cnt;
} WC40_LINK_WAR;

typedef struct {
    uint32 start_time;
    uint32 state;
} WC40_AN_MODE;

typedef struct {
    int type;
    int poly;
} WC40_PRBS_CTRL;

typedef struct {
    uint16 state;
    uint16 lp_adv;
    uint16 ld_kr2;
    uint16 timer_state;
    uint32 start_time;
    uint16 attempt;
} WC40_SOFT_KR2;

typedef struct {
    uint16 state;
    uint16 enabled;
    uint16 tick_cnt;
} WC40_FORCE_CL72_t;

typedef struct {
    WC40_LINK_WAR lkwa;
    WC40_AN_MODE an;
    WC40_PRBS_CTRL prbs;
    int clk_comp;
    WC40_SOFT_KR2 soft_kr2;
    WC40_FORCE_CL72_t cl72;
} WC40_DEV_CTRL_t;

typedef struct {
    WC40_DEV_CFG_t   cfg;
    WC40_DEV_INFO_t  info;
    WC40_DEV_CTRL_t  ctrl;
} WC40_DEV_DESC_t;

#define DEV_CFG_PTR(_pc) (&(((WC40_DEV_DESC_t *)((_pc) + 1))->cfg))
#define DEV_INFO_PTR(_pc) (&(((WC40_DEV_DESC_t *)((_pc) + 1))->info))
#define DEV_CTRL_PTR(_pc) (&(((WC40_DEV_DESC_t *)((_pc) + 1))->ctrl))

/* force cl72 */
#define FORCE_CL72_IS_ENABLED(_pc) (DEV_CTRL_PTR(_pc)->cl72.enabled \
                                 && DEV_CFG_PTR(_pc)->force_cl72)
#define FORCE_CL72_ENABLE(_pc) (DEV_CTRL_PTR(_pc)->cl72.enabled) 
#define FORCE_CL72_STATE(_pc) (DEV_CTRL_PTR(_pc)->cl72.state)
#define FORCE_CL72_TICK_CNT(_pc) (DEV_CTRL_PTR(_pc)->cl72.tick_cnt)


#define SOFT_KR2_TX_SQUELCH_WAR_INTERVAL (750000)
#define SOFT_KR2_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->soft_kr2.start_time)
#define SOFT_KR2_TIMER_VALID(_pc) \
        (DEV_CTRL_PTR(_pc)->soft_kr2.timer_state)
#define SOFT_KR2_TIMER_EN(_pc) \
        (DEV_CTRL_PTR(_pc)->soft_kr2.timer_state) = TRUE
#define SOFT_KR2_TIMER_DIS(_pc) \
        (DEV_CTRL_PTR(_pc)->soft_kr2.timer_state) = FALSE
#define SOFT_KR2_RECORD_TIME(_pc) \
        (DEV_CTRL_PTR(_pc)->soft_kr2.start_time) = sal_time_usecs()
#define SOFT_KR2_CHECK_TIMEOUT(_pc) \
       ((uint32)SAL_USECS_SUB(sal_time_usecs(), SOFT_KR2_TIME((_pc))) \
                >= SOFT_KR2_TX_SQUELCH_WAR_INTERVAL)


typedef struct {
    uint8 *pdata;
    int   *plen;
    uint16 chip_rev;
} WC40_UCODE_DESC;

/* external functions shared within WC driver files  */
extern int phy_wc40_config_init(phy_ctrl_t *pc);

/* (*_offset_p) and _size will accumulate by the sizeof(_var) */
#define WC40_WB_ID_CMD(_ptr, _var, _t, _offset_p,_size_p)  \
        do {   \
            uint8  *_vptr_ ; \
            int     _vsize_ ; \
            _vptr_  = (_ptr) + (*(_offset_p)) ; \
            _vsize_ = sizeof((_var)) ;          \
            if((_t)==WC40_WB_SC_UPDATE){              \
                sal_memcpy(_vptr_,(&(_var)),_vsize_) ;  \
            } else if((_t)==WC40_WB_SC_RETRIEVE) {    \
                sal_memcpy((&(_var)),_vptr_,_vsize_);   \
            } \
            (*(_size_p))   = (*(_size_p)) + _vsize_ ;   \
            (*(_offset_p)) += _vsize_ ;                 \
        } while(0)

typedef enum wc40_wb_handler_type {
    WC40_WB_SC_COUNT,
    WC40_WB_SC_RETRIEVE,
    WC40_WB_SC_UPDATE
} WC40_WB_HANDLER_TYPE_t ;

typedef enum wc40_wb_data_id {
    WC40_WB_SC_VER=0,
    WC40_WB_RXLOS,
    WC40_WB_SC_LAST 
} WC40_WB_DATA_ID_t ;

#endif /* WC40_EXTRA_H__ */
