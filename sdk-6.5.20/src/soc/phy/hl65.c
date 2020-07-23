/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        hl65.c
 * Purpose:     Broadcom 10/100/1000/2500/10000/12000/13000/16000/20000 SerDes 
 *              (HyperLite 65nm with x1 and x4 lane support)
 */
#include "phydefs.h"      /* Must include before other phy related includes */
#if defined(INCLUDE_XGXS_HL65)
#include "phyconfig.h"     /* Must include before other phy related includes */


#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/spl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>
#include <soc/phyctrl.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyreg.h"
#include "phyfege.h"
#include "phynull.h"
#include "serdesid.h"
#include "hl65.h"

/* 
 * HL/HC defines not contained in hl65.h which is generated from RDB files 
 */

#define HL65_PLL_WAIT  250000
#define HL65_AN_WAIT   400000   /* 400ms */
#define HL65_NUM_LANES 4
#define TX_DRIVER_DFT_LN_CTRL  0x4

#define HL_SERDES_ID0_REVID_A0    0x0000
#define HL_SERDES_ID0_REVID_A1    0x0800
#define HL_SERDES_ID0_REVID_B0    0x4000
#define HL_SERDES_ID0_REVID_C0    0x8000
#define HL_SERDES_ID0_REVID_D0    0xc000

#define HL_ALL_LANES  0xf
#define HL_AER_BCST_OFS_STRAP   0x3ff

#define HL65_A0A1_DSC_DFE_BRCDR 0x2180
#define HL65_B0C0_DSC_DFE_BRCDR 0x2300
#define OVER1G_UP3_20GPLUS_MASK (DIGITAL3_UP3_DATARATE_25P45GX4_MASK | \
				 DIGITAL3_UP3_DATARATE_21GX4_MASK)
#define PLL_PLLACONTROL4_BIT12_MASK         0x1000

/* Broadcast to all lanes with AER::MMD_PORT set to aer_bcst_ofs_strap */
#define HL65_BROADCAST_WRITE(_unit, _pc, _bcst_strap, _reg_addr, _data) \
        HL65_REG_WRITE((_unit), (_pc), 0x00, \
                       ((_bcst_strap) << 16) | (_reg_addr), (_data))

typedef enum {
    PHY_HL65_DSC_CFG_1X_OSR,
    PHY_HL65_DSC_CFG_2X_OSR,
    PHY_HL65_DSC_CFG_4X_OSR,
    PHY_HL65_DSC_CFG_5X_OSR,
    PHY_HL65_DSC_CFG_BR_CDR,
    PHY_HL65_DSC_CFG_UNKNOWN
} phy_hl65_dsc_cfg_t;

/* function forward declaration */
STATIC int _phy_hl65_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_hl65_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_hl65_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_hl65_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_hl65_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int phy_hl65_an_set(int unit, soc_port_t port, int an);
STATIC int phy_hl65_lb_set(int unit, soc_port_t port, int enable);
STATIC int phy_hl65_ability_advert_set(int unit, soc_port_t port,
                                      soc_port_ability_t *ability);
STATIC int phy_hl65_ability_local_get(int unit, soc_port_t port,
                                soc_port_ability_t *ability);
STATIC int phy_hl65_diag_ctrl(int, soc_port_t, uint32, int, int, void *); 
STATIC int phy_hl65_speed_set(int unit, soc_port_t port, int speed);
STATIC int phy_hl65_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_hl65_an_get(int unit, soc_port_t port, int *an, int *an_done); 
STATIC int _phy_hl65_analog_dsc_init (int unit,int port,int lane);
STATIC int _hl65_xgmii_scw_config (int unit, phy_ctrl_t *pc);
STATIC int _hl65_rxaui_config(int unit, phy_ctrl_t  *pc,int rxaui);
STATIC int _hl65_soft_reset(int unit, phy_ctrl_t  *pc);
STATIC int _hl65_lane_reg_init(int unit, phy_ctrl_t  *pc);
STATIC int phy_hl65_uc_status_dump(int unit, soc_port_t port);



STATIC int
_phy_hl65_pll_lock_wait(int unit, soc_port_t port)
{
    uint16           data16;
    phy_ctrl_t      *pc;
    soc_timeout_t    to;
    int              rv;

    pc = INT_PHY_SW_STATE(unit, port);
    soc_timeout_init(&to, HL65_PLL_WAIT, 0);
    do {
        rv = READ_HL65_XGXSBLK0_XGXSSTATUSr(unit, pc, &data16);
        if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "HL65 : TXPLL did not lock: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_hl65_half_rate_set(int unit, soc_port_t port,int speed)
{
    phy_ctrl_t     *pc;
    int tx_halfrate_bit;
    uint16 serdes_id0;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                            TEST_SERDESID0_REV_LETTER_MASK);

    /* only apply to these revisions */

    if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_A1) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_B0)) {
        
        if (speed == 2500 || speed == 10000) {
            tx_halfrate_bit = TX0_ANATXACONTROL1_TX_SEL_HALFRATE_MASK;
        } else {
            tx_halfrate_bit = 0;
        }
                                                                               
        /* set tx half rate bits after the link is established at 2.5G or 10G
         * in both force mode or autoneg mode.
         */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_TX0_ANATXACONTROL1r(unit, pc,
                       tx_halfrate_bit,
                       TX0_ANATXACONTROL1_TX_SEL_HALFRATE_MASK));
        if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX1_ANATXACONTROL1r(unit, pc,
                       tx_halfrate_bit,
                       TX0_ANATXACONTROL1_TX_SEL_HALFRATE_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX2_ANATXACONTROL1r(unit, pc,
                       tx_halfrate_bit,
                       TX0_ANATXACONTROL1_TX_SEL_HALFRATE_MASK));
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX3_ANATXACONTROL1r(unit, pc,
                       tx_halfrate_bit,
                       TX0_ANATXACONTROL1_TX_SEL_HALFRATE_MASK));
        }
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_hl65_tx_control_set(int unit, soc_port_t port)
{
    phy_ctrl_t     *pc;
    uint32          preemph;
    uint32          idriver;
    uint32          pdriver;
    uint16          data16;
 
    pc = INT_PHY_SW_STATE(unit, port);

    
    if (PHY_EXTERNAL_MODE(unit, port)) {
        /* HL65 connected to external PHY */
        if (IS_HG_PORT(unit, port)) {
            preemph = 0x0;
            idriver = 0x9;
            pdriver = 0x9;
        } else if (IS_XE_PORT(unit, port)) {
            preemph = 0x0;
            idriver = 0x9;
            pdriver = 0x9;
        } else {
            preemph = 0x0;
            idriver = 0x9;
            pdriver = 0x9;
        }
    } else {
        /* No external PHY */
        if (IS_HG_PORT(unit, port)) {
            preemph = 0x0;
            idriver = 0x9;
            pdriver = 0x9;
        } else if (IS_XE_PORT(unit, port)) {
            preemph = 0x0;
            idriver = 0x9;
            pdriver = 0x9;
        } else {
            preemph = 0x0;
            idriver = 0x9;
            pdriver = 0x9;
        }
    }

    /* Read config override */
    preemph = soc_property_port_get(unit, port,
                                    spn_SERDES_PREEMPHASIS, preemph);
    idriver = soc_property_port_get(unit, port,
                                    spn_SERDES_DRIVER_CURRENT, idriver);
    pdriver = soc_property_port_get(unit, port,
                                    spn_SERDES_PRE_DRIVER_CURRENT, pdriver);

    
    preemph = (preemph << TXB_TX_OS_DRIVER_PREEMPHASIS_POST_SHIFT) &
              TXB_TX_OS_DRIVER_PREEMPHASIS_POST_MASK;
    idriver = (idriver << TXB_TX_OS_DRIVER_IDRIVER_SHIFT) & 
              TXB_TX_OS_DRIVER_IDRIVER_MASK;
    pdriver = (pdriver << TXB_TX_OS_DRIVER_IPREDRIVER_SHIFT) &
              TXB_TX_OS_DRIVER_IPREDRIVER_MASK;
    data16  = (uint16)(preemph | idriver | pdriver);

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_TXB_TX_OS_DRIVERr(unit, pc, data16,
                              TXB_TX_OS_DRIVER_PREEMPHASIS_POST_MASK |
                              TXB_TX_OS_DRIVER_IDRIVER_MASK |
                              TXB_TX_OS_DRIVER_IPREDRIVER_MASK));

    /* set for BR-CDR mode as well */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_TXB_TX_BR_DRIVERr(unit, pc, data16,
                              TXB_TX_OS_DRIVER_PREEMPHASIS_POST_MASK |
                              TXB_TX_OS_DRIVER_IDRIVER_MASK |
                              TXB_TX_OS_DRIVER_IPREDRIVER_MASK));
    return SOC_E_NONE;
}

STATIC int
_hl65_ind_lane_polarity_set (int unit, soc_port_t port)
{
    int rxpol;
    int txpol;
    uint16 data16;
    uint16 mask16;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    txpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
    rxpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_RX_POLARITY_FLIP, 0);
    if (rxpol) {
        /* Flip Rx polarity */
        mask16 = RX0_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK |
                 RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK;
        data16 = mask16;
        if (pc->lane_num == 0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX0_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        } else if (pc->lane_num == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX1_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        } else if (pc->lane_num == 2) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX2_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        } else if (pc->lane_num == 3) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX3_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        }
        if (pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) {
            /* Also flip for the next lane */
            if (pc->lane_num == 0) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_RX1_ANARXCONTROLPCIr(unit,pc,data16,mask16));
            } else if (pc->lane_num == 2) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_RX3_ANARXCONTROLPCIr(unit,pc,data16,mask16));
            }
        }
    }
    if (txpol) {
        /* Flip TX polarity */
        data16 = TX0_ANATXACONTROL0_TXPOL_FLIP_MASK;
        mask16 = TX0_ANATXACONTROL0_TXPOL_FLIP_MASK;
        if (pc->lane_num == 0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX0_ANATXACONTROL0r(unit, pc, data16, mask16));
        } else if (pc->lane_num == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX1_ANATXACONTROL0r(unit, pc, data16, mask16));
        } else if (pc->lane_num == 2) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX2_ANATXACONTROL0r(unit, pc, data16, mask16));
        } else if (pc->lane_num == 3) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TX3_ANATXACONTROL0r(unit, pc, data16, mask16));
        }
        if (pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) {
            /* Also flip for the next lane */
            if (pc->lane_num == 0) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_TX1_ANATXACONTROL0r(unit, pc, data16, mask16));
            } else if (pc->lane_num == 2) {
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_TX3_ANATXACONTROL0r(unit, pc, data16, mask16));
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int
_hl65_combo_lane_polarity_set (int unit, soc_port_t port)
{
    int rxpol;
    int txpol;
    uint16 data16;
    uint16 mask16;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    txpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
    rxpol = soc_property_port_get(unit, port,
                            spn_PHY_XAUI_RX_POLARITY_FLIP, 0);

    if (txpol) {             /* Flip TX polarity */
        data16 = TX0_ANATXACONTROL0_TXPOL_FLIP_MASK;
        mask16 = TX0_ANATXACONTROL0_TXPOL_FLIP_MASK;
        if (txpol == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TXB_ANATXACONTROL0r(unit, pc, data16, mask16));
        }
        if ((txpol & 0x000F) == 0x000F) {
            SOC_IF_ERROR_RETURN
              (MODIFY_HL65_TX0_ANATXACONTROL0r(unit, pc, data16, mask16));
        }
        if ((txpol & 0x00F0) == 0x00F0) {
            SOC_IF_ERROR_RETURN
              (MODIFY_HL65_TX1_ANATXACONTROL0r(unit, pc, data16, mask16));
        }
        if ((txpol & 0x0F00) == 0x0F00) {
            SOC_IF_ERROR_RETURN
              (MODIFY_HL65_TX2_ANATXACONTROL0r(unit, pc, data16, mask16));
        }
        if ((txpol & 0xF000) == 0xF000) {
            SOC_IF_ERROR_RETURN
              (MODIFY_HL65_TX3_ANATXACONTROL0r(unit, pc, data16, mask16));
        }
    }
    if (rxpol) {
        /* Flip Rx polarity */
        mask16 = RX0_ANARXCONTROLPCI_RX_POLARITY_FORCE_SM_MASK |
             RX0_ANARXCONTROLPCI_RX_POLARITY_R_MASK;
        data16 = mask16;
        if (rxpol == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RXB_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        }
        if ((rxpol & 0x000F) == 0x000F) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX0_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        }
        if ((rxpol & 0x00F0) == 0x00F0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX1_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        }
        if ((rxpol & 0x0F00) == 0x0F00) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX2_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        }
        if ((rxpol & 0xF000) == 0xF000) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_RX3_ANARXCONTROLPCIr(unit,pc,data16,mask16));
        }
    }
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_independent_lane_init(int unit, soc_port_t port)
{
    int                 fiber;
    uint16              data16, mask16;
    uint16              mode_1000x;
    int                 rv;
    soc_timeout_t       to;
    phy_ctrl_t         *pc;
    soc_port_ability_t  ability;

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    /* Only need to track fiber mode state.
     * Copper mode state is tracked by external PHY.
     */
    pc                       = INT_PHY_SW_STATE(unit, port);
    fiber                    = PHY_FIBER_MODE(unit, port);
    pc->fiber.enable         = fiber;
    pc->fiber.preferred      = fiber;
    pc->fiber.autoneg_enable = 1;
    pc->fiber.force_speed    = 1000;
    pc->fiber.force_duplex   = TRUE;
    pc->fiber.master         = SOC_PORT_MS_NONE;
    pc->fiber.mdix           = SOC_PORT_MDIX_NORMAL;

    /* if the port is using two lanes and not using as dxgxs mode,
     * then disable the second lane
     */
    if ((pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) && IS_GE_PORT(unit,port)) {
        int mdio_addr_share = FALSE;

        /* clear next lane default dxgxs configuration */
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            pc->phy_id++;
        } else {
            pc->lane_num++;
        }
        SOC_IF_ERROR_RETURN
             (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, 0, 0x1f));

        SOC_IF_ERROR_RETURN
             (MODIFY_HL65_DIGITAL4_MISC3r(unit, pc, 0,
                     DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
        if (!(pc->flags & PHYCTRL_MDIO_ADDR_SHARE)) {
            pc->phy_id--;
        } else {
            pc->lane_num--;
        }

        /* disable the next lane
         * the lane disable register can only be accessed from lane 0
         */
        if (pc->flags & PHYCTRL_MDIO_ADDR_SHARE) {
            mdio_addr_share = TRUE;
            pc->flags &= ~PHYCTRL_MDIO_ADDR_SHARE;
        } else {
            pc->phy_id -= pc->lane_num;
        }

        mask16 = (1 << (pc->lane_num + 1));    /* rx lane */
        mask16 |= (mask16 << 4); /* add tx lane */
        mask16 |= 0x800;         /* add force bit */
        data16 = mask16;

        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_XGXSBLK1_LANECTRL3r(unit,pc,data16,mask16));
        
        /* restore the previous MDIO address mode */
        if (mdio_addr_share == TRUE) {
            pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
        } else {
            pc->phy_id += pc->lane_num;
        }
    }

    /* Since driver reset function vector does not reset
     * the SerDes, reset the SerDes in initialization first.
     * The internal SERDES PHY's reset bit is self-clearing.
     */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, MII_CTRL_RESET));

    soc_timeout_init(&to, 10000, 0);
    while (!soc_timeout_check(&to)) {
        rv = READ_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, &data16);
        if (((data16 & MII_CTRL_RESET) == 0) ||
            SOC_FAILURE(rv)) {
            break;
        }
    }
    if ((data16 & MII_CTRL_RESET) != 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Combo SerDes reset failed: u=%d p=%d\n"),
                  unit, port));
    }

    /* disable in-band MDIO. PHY-443 */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_XGXSBLK3_LOCALCONTROL0r(unit,pc,
                XGXSBLK3_LOCALCONTROL0_RX_INBANDMDIO_RST_MASK,
                XGXSBLK3_LOCALCONTROL0_RX_INBANDMDIO_RST_MASK));

    /* check if need to flip any polarity for the lane */
    SOC_IF_ERROR_RETURN
        (_hl65_ind_lane_polarity_set (unit, port));

    /* Select recovered clock from MII fifo read */
    if (IS_GE_PORT(unit, port) && (fiber == 0)) {
        mask16 = DIGITAL4_MISC3_RXCK_MII_OVERRIDE_VAL_MASK |
                 DIGITAL4_MISC3_RXCK_MII_OVERRIDE_MASK;
        data16 = mask16;
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL4_MISC3r(unit, pc, data16, mask16));
    }

    /* Enable output clock to be present when the core is commanded into
     * power down state.
     */
    data16 = XGXSBLK1_LANETEST_PWRDWN_CLKS_EN_MASK;
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK1_LANETESTr(unit, pc, data16)); 

    /* Set Local Advertising Configuration */
    SOC_IF_ERROR_RETURN
        (phy_hl65_ability_local_get(unit, port, &ability)); 
    /* Do not advertise 2.5G. 2.5 Gbps AN is not supported */
    ability.speed_full_duplex &= ~SOC_PA_SPEED_2500MB;

    pc->fiber.advert_ability = ability;
    SOC_IF_ERROR_RETURN
        (phy_hl65_ability_advert_set(unit, port, &ability));

    /* Disable BAM in Independent Lance mode. Over1G AN not supported  */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_DIGITAL6_MP5_NEXTPAGECTRLr(unit, pc, 0));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_DIGITAL6_UD_FIELDr(unit, pc, 0));

    data16 = DIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_MASK |
             DIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
    mask16 = data16 | DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;

    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        /* Enable 1000X parallel detect */
        data16 |= DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    }
    
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_CONTROL1000X2r(unit, pc, data16, mask16));

    /* Initialialize autoneg and fullduplex */
    data16 = MII_CTRL_FD | MII_CTRL_SS_1000;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port) ||
        PHY_SGMII_AUTONEG_MODE(unit, port)) {
        data16 |= MII_CTRL_AE | MII_CTRL_RAN;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, data16));

    /* Configure default preemphasis, predriver, idriver values */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_tx_control_set(unit, port));

    /* Configuring operating mode */
    data16 = DIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
    mask16 = DIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK |
             DIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
    /*
     * Configure signal auto-detection between SGMII and fiber.
     * This detection only works if auto-negotiation is enabled.
     */
    if (soc_property_port_get(unit, port,
                              spn_SERDES_AUTOMEDIUM, FALSE)) {
        data16 |= DIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
    }

    /*
     * Put the Serdes in Fiber or SGMII mode
     */
    mode_1000x = 0;
    if ((PHY_FIBER_MODE(unit, port) && !PHY_EXTERNAL_MODE(unit, port)) ||
        PHY_PASSTHRU_MODE(unit, port)) {
        mode_1000x = DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
    /* Allow property to override */
    if (soc_property_port_get(unit, port,
                              spn_SERDES_FIBER_PREF, mode_1000x)) {
        data16 |= DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    } else {
        data16 &= ~DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_SGMII_MASTER, FALSE)) {
        data16 |= DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK;
        mask16 |= DIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_CONTROL1000X1r(unit, pc, data16, mask16));

    SOC_IF_ERROR_RETURN
        (_hl65_lane_reg_init(unit,pc));

    SOC_IF_ERROR_RETURN
        (_phy_hl65_analog_dsc_init(unit,port,pc->lane_num));

    /* config the 64B/66B for dxgxs mode */
    SOC_IF_ERROR_RETURN
        (_hl65_xgmii_scw_config (unit,pc));

    /* autoneg is not supported in dxgxs mode */
    if ((pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) && 
        (IS_HG_PORT(unit,port) || IS_XE_PORT(unit,port))) {
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, 0, MII_CTRL_AE));
        SOC_IF_ERROR_RETURN
             (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, 0x1, 0x1f));

         SOC_IF_ERROR_RETURN
             (MODIFY_HL65_DIGITAL4_MISC3r(unit, pc, DIGITAL4_MISC3_FORCE_SPEED_B5_MASK,
                     DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));
    } else { /* all other modes, enable autoneg by default */
        /* clear forced speed control. Some two-lane ports are strapped
         * in forced dxgxs speed.
         */ 
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, 0, 
                               DIGITAL_MISC1_FORCE_SPEED_MASK));
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL4_MISC3r(unit, pc, 0,
                    DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

        /* enable autoneg */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, MII_CTRL_AE, 
                          MII_CTRL_AE));
    }
    /* PRBS Polynomial for all lanes: 0x8019=0x0000 */
	
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK1_LANEPRBSr(unit, pc, 0)); 

     LOG_INFO(BSL_LS_SOC_PHY,
              (BSL_META_U(unit,
                          "phy_hl65_init: u=%d p=%d %s\n"),
               unit, port, (fiber) ? "Fiber" : "Copper"));
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_combo_core_init(int unit, soc_port_t port)
{
    phy_ctrl_t        *pc;
    soc_port_ability_t ability;
    uint16             data16, mask16;

    pc = INT_PHY_SW_STATE(unit, port);

    /* must be done after reset, otherwise will be cleared  */
    data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                        XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT);
    SOC_IF_ERROR_RETURN
            (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc,data16,
                                       (XGXSBLK0_XGXSCONTROL_MODE_10G_MASK)));

    /* disable in-band MDIO. PHY-443 */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_XGXSBLK3_LOCALCONTROL0r(unit,pc,
                XGXSBLK3_LOCALCONTROL0_RX_INBANDMDIO_RST_MASK,
                XGXSBLK3_LOCALCONTROL0_RX_INBANDMDIO_RST_MASK));

    /* check if need to flip any polarity for the lanes */
    SOC_IF_ERROR_RETURN
        (_hl65_combo_lane_polarity_set (unit, port));

    /* disable CL73 BAM */
    SOC_IF_ERROR_RETURN
        (HL65_REG_MODIFY(unit,pc,0,0x38008372,
               0,
               CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK));
                                                                                
    if (!PHY_CLAUSE73_MODE(unit, port)) {
        /* disable CL73 AN device*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit,pc,0));
    }

    /* PLL VCO step time */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_TXPLL_ANAPLLTIMER1r(unit, pc, 0x04ff));

    /* Disable IEEE block select auto-detect. 
     * The driver will select desired block as necessary.
     * By default, the driver keeps the mapping for XAUI block in
     * IEEE address space.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_XGXSBLK0_MISCCONTROL1r(unit, pc, 
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK,
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK));

    SOC_IF_ERROR_RETURN
        (phy_hl65_ability_local_get(unit, port, &ability));
    if (PHY_EXTERNAL_MODE(unit, port)) {
            ability.speed_full_duplex = SOC_PA_SPEED_10GB;
    }
    SOC_IF_ERROR_RETURN
        (phy_hl65_ability_advert_set(unit, port, &ability));

    /* Configure 10G parallel detect */
    SOC_IF_ERROR_RETURN    /* Adjust parallel detect link timer to 60ms */
        (WRITE_HL65_AN73_PDET_PARDET10GLINKr(unit, pc, 0x16E2));

    data16 = 0;
    mask16 = AN73_PDET_PARDET10GCONTROL_PD_12G_TXDON_DISABLE_MASK |
             AN73_PDET_PARDET10GCONTROL_PD_12G_DISABLE_MASK |
             AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    if (soc_property_port_get(unit, port, spn_XGXS_PDETECT_10G, 1)) {
        data16 |= AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
        if (pc->speed_max <= 10000) {
            /* Disable parallel detect for 12Gbps. */
            data16 |= AN73_PDET_PARDET10GCONTROL_PD_12G_TXDON_DISABLE_MASK |
                      AN73_PDET_PARDET10GCONTROL_PD_12G_DISABLE_MASK;
        }
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_AN73_PDET_PARDET10GCONTROLr(unit, pc, data16, mask16));

    /* Configure default preemphasis, predriver, idriver values */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_tx_control_set(unit, port));

    /* Enable output clock to be present when the core is commanded into
     * power down state. 
     */
    data16 = XGXSBLK1_LANETEST_PWRDWN_CLKS_EN_MASK |
             XGXSBLK1_LANETEST_PWRDN_SAFE_DIS_MASK;
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK1_LANETESTr(unit, pc, data16)); 

    /*
     * Transform CX4 pin out on the board to HG pinout
     */
    if (soc_property_port_get(unit, port, spn_CX4_TO_HIGIG, FALSE)) {
        SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
            (WRITE_HL65_XGXSBLK2_TXLNSWAPr(unit, pc, 0x80e4));

        SOC_IF_ERROR_RETURN     /* Flip TX Lane polarity */
            (WRITE_HL65_TXB_ANATXACONTROL0r(unit, pc, 0x1020));
    } else {
        /* If CX4 to HG conversion is enabled, do not allow individual lane
         * swapping.
         */
        uint16 lane_map, i;
        uint16 lane, hw_map, chk_map;

        /* Update RX lane map */
        lane_map = soc_property_port_get(unit, port,
                                        spn_XGXS_RX_LANE_MAP, 0x0123) & 0xffff;
        if (lane_map != 0x0123) {
            hw_map  = 0;
            chk_map = 0;
            for (i = 0; i < 4; i++) {
                lane     = (lane_map >> (i * 4)) & 0xf;
                hw_map  |= lane << (i * 2);
                chk_map |= 1 << lane;
            }
            if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
                   (MODIFY_HL65_XGXSBLK2_RXLNSWAPr(unit, pc,
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE_EN_MASK | hw_map,
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE_EN_MASK |
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE0_MASK |
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE1_MASK |
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE2_MASK |
                       XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE3_MASK));
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "unit %d port %s: Invalid RX lane map 0x%04x.\n"),
                           unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }

        /* Update TX lane map */
        lane_map = soc_property_port_get(unit, port,
                                        spn_XGXS_TX_LANE_MAP, 0x0123) & 0xffff;
        if (lane_map != 0x0123) {
            hw_map  = 0;
            chk_map = 0;
            for (i = 0; i < 4; i++) {
                lane     = (lane_map >> (i * 4)) & 0xf;
                hw_map  |= lane << (i * 2);
                chk_map |= 1 << lane;
            }
            if (chk_map == 0xf) {
                SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                   (MODIFY_HL65_XGXSBLK2_TXLNSWAPr(unit, pc, 0x8000 | hw_map,
                                                   0x80ff));
            } else {
                LOG_ERROR(BSL_LS_SOC_PHY,
                          (BSL_META_U(unit,
                                      "unit %d port %s: Invalid TX lane map 0x%04x.\n"),
                           unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }
    }

    SOC_IF_ERROR_RETURN
         (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    SOC_IF_ERROR_RETURN
        (_phy_hl65_analog_dsc_init(unit,port,HL_ALL_LANES));

    /* config the 64B/66B for 25G, won't affect other speeds and AN  */
    SOC_IF_ERROR_RETURN
        (_hl65_xgmii_scw_config (unit,pc));

    /* broadcast to all lanes */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_AERBLK_AERr(unit,pc, HL_AER_BCST_OFS_STRAP));

    SOC_IF_ERROR_RETURN
        (_hl65_lane_reg_init(unit,pc));

    /* reset AER */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_AERBLK_AERr(unit,pc, 0));
    
    /* enable autoneg or force a speed depending on the port type */
    
    if (PHY_EXTERNAL_MODE(unit, port) &&
        (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port))) {
        int force_speed;

        if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
            force_speed= DIGITAL_MISC1_FORCE_SPEED_dr_10GBASE_CX4; /* 10G CX4 */
        } else {
            force_speed= DIGITAL_MISC1_FORCE_SPEED_dr_10GHiGig_X4; /* 10G HiG */
        }

        /* disable autoneg */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             0,
                                             MII_CTRL_AE));
        if (PHY_CLAUSE73_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc,
                                              0,
                                              MII_CTRL_AE));
        }

        /* force speed  to 10G */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, force_speed,
                                DIGITAL_MISC1_FORCE_SPEED_MASK));
    } else { /* autoneg */
        /* clear forced bit */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, 0, 0x10));

        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             MII_CTRL_AE | MII_CTRL_RAN,
                                             MII_CTRL_AE | MII_CTRL_RAN));
        if (PHY_CLAUSE73_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc,
                                              MII_CTRL_AE | MII_CTRL_RAN,
                                              MII_CTRL_AE | MII_CTRL_RAN));
        }
    }

    SOC_IF_ERROR_RETURN
         (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    /*
     * Configure signal auto-detection between SGMII and fiber.
     */
    data16 = DIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK;
    mask16 = DIGITAL_CONTROL1000X1_DISABLE_PLL_PWRDWN_MASK |
             DIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    if (soc_property_port_get(unit, port, spn_SERDES_AUTOMEDIUM, TRUE)) {
        data16 |= DIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
    }
    if (soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF, TRUE)) {
        data16 |= DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_CONTROL1000X1r(unit, pc, data16, mask16));

    /* set filter_force_link and disable_false_link */

    mask16 = DIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_MASK |
             DIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_MASK;
    data16 = mask16;
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_CONTROL1000X2r(unit, pc, data16, mask16));

    (void) _phy_hl65_pll_lock_wait(unit, port);

    /* select recovery clock from lane0 for rxck0_10g */
    SOC_IF_ERROR_RETURN
         (MODIFY_HL65_XGXSBLK1_TESTTXr(unit,pc,
                 0,
                 XGXSBLK1_TESTTX_RX_CK4X1MUXSEL_MASK));
    return SOC_E_NONE;
}


STATIC int
_phy_hl65_fabric_init(int unit, soc_port_t port)
{
    uint16      data;
    uint16      serdes_id0;
    uint16      aer_bcst_ofs_strap;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_fabric_init: u=%d p=%d\n"), unit, port));
    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                            TEST_SERDESID0_REV_LETTER_MASK);

    if (serdes_id0 == HL_SERDES_ID0_REVID_C0) {
        aer_bcst_ofs_strap = 0x1ff;
    } else {
        aer_bcst_ofs_strap = 0x3ff;
    }
        
    PHY_FLAGS_SET(unit, port, PHY_FLAGS_INDEPENDENT_LANE);

    /* Broadcast all lanes only when initializing the first lane */
    if (pc->lane_num == 0) {
        /* 
         * Force the sync character to the lower byte of the parallel 
         * interface 
         */
        /* hwBm9600HcBroadcastWrite(pInitParams, HC_ffde_ADDR, 0x03ff) */
        /* hwBm9600HcRead(pInitParams, HC_PORT_ZERO, HC_80b9_ADDR, &uReadData)*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit, pc, 0x00, 0x80b9, &data));
        
        /* data = data | 0x0020;*/
        data = data | (RX0_ANARXCONTROL1G_COMMA_LOW_BYTE_SM_BITS <<
                       RX0_ANARXCONTROL1G_COMMA_LOW_BYTE_SM_SHIFT);

	/* Enable the comma byte adjust on the opposite byte */
        data = data | (RX0_ANARXCONTROL1G_COMMA_BYTE_ADJ_EN_SM_BITS <<
                       RX0_ANARXCONTROL1G_COMMA_BYTE_ADJ_EN_SM_SHIFT);


        /* hwBm9600HcBroadcastWrite(pInitParams, HC_80b9_ADDR, uReadData) */
        SOC_IF_ERROR_RETURN
            (HL65_BROADCAST_WRITE(unit, pc, aer_bcst_ofs_strap,  0x80b9, data));
    }

    /* PLL Sequencer disable */
    /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, 
     *  0x8000, 0x062f)
     *  >> Clear START_SEQUENCER_BITS 13
     */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x062f));

    /* Set force_lane_mode so that lane rates can be configured independently */
    /* soc_bm9600_mdio_hc_read(pInitParams->m_nBaseAddr, uPhyAddr, uLane, 
     *  0x8308, &uData)
     */
    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL_MISC1r(unit, pc, &data));

    /* uData |= 0x0020 */ 
    data |=  (DIGITAL_MISC1_FORCE_LN_MODE_BITS <<  DIGITAL_MISC1_FORCE_LN_MODE_SHIFT);
	
    /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, uLane, 0x8308, uData) */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_DIGITAL_MISC1r(unit, pc, data));

    if (serdes_id0 == HL_SERDES_ID0_REVID_C0) {
      
	SOC_IF_ERROR_RETURN
	    (READ_HL65_XGXSBLK1_LANECTRL0r(unit, pc, &data));

        /* XgxsBlk1 Lanectrl0 (8015) :: Clear CL36- keep already set up 64/66 settings  */
        data &= 0xff00;

        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL0r(unit, pc, data));

        /*
         *  XgxsBlk1 Shared (0x800d) ::
         *    Clear SELECT_DEVPMD_EN_BITS | MMDSELECT_DEVAN_EN_BITS
         *  This setup matches other SBX fabric (88130)
         */
        data = (XGXSBLK0_MMDSELECT_DEVDEVAD_EN_BITS <<  
                XGXSBLK0_MMDSELECT_DEVDEVAD_EN_SHIFT) |
               (XGXSBLK0_MMDSELECT_DEVCL22_EN_BITS <<
                XGXSBLK0_MMDSELECT_DEVCL22_EN_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK0_MMDSELECTr(unit, pc, data));
 
        /* SerdesDigital_Control1000X3 (0x8302) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL_CONTROL1000X3r(unit, pc, 0));

        /* Set pf_br_init to 4 for C0 */
        /* DSC2B0_SM_CTRL7    uRegAddr = 0x8267 + (uLane*0x10) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8267 + (0x10 * pc->lane_num), 0x14A5));


        /* Enable hysteresis, enable tuning state machine set postc_metric_ctrl to 1 */
        /* DSC2B0_SM_CTRL0     uRegAddr = 0x8260 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x0021) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8260 + (0x10 * pc->lane_num), 0x1821));

    } else {

        /* Perform suggested writes during init */
        /* Set DSC2B0_SM_CTRL11[14:10] msr_br_vga_timeout = 0 */
        /* uRegAddr = 0x826b + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x0312) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x826b + (0x10 * pc->lane_num), 0x0312));
        
        /* Set DSC2B0_SM_CTRL12[13:12] br_vga_lms_gain= 0 */
        /* uRegAddr = 0x826c + (uLane*0x10); */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x046a) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x826c + (0x10 * pc->lane_num), 0x046a));
	
        /* Disable hysteresis, enable tuning state machine  */
        /* DSC2B0_SM_CTRL0     uRegAddr = 0x8260 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x0021) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8260 + (0x10 * pc->lane_num), 0x0021));
    	
        /* Increase ACQVGA timeout to 580K bits */
        /* DSC2B0_SM_CTRL2    uRegAddr = 0x8262 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x3800)*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8262 + (0x10 * pc->lane_num), 0x3800));
    	
        /* Set pf_br_init to 2 */
        /* DSC2B0_SM_CTRL7    uRegAddr = 0x8267 + (uLane*0x10) */
        /*  rv = soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x1495) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8267 + (0x10 * pc->lane_num), 0x1495));
	
        
        /* Set vga_min=0, vga_max=31 */
        /* DSC2B0_SM_CTRL8    uRegAddr = 0x8268 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x03e0) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8268 + (0x10 * pc->lane_num), 0x03e0));
        
        /* Set dfe_min=0, dfe_max=63 */
        /* DSC2B0_SM_CTRL9    uRegAddr = 0x8269 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x0fc0) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8269 + (0x10 * pc->lane_num), 0x0fc0));
        
        /* Set BR ACQCDR kp=4 */
        /* DSC2B0_SM_CTRL3     uRegAddr = 0x8263 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x421b) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8263 + (0x10 * pc->lane_num), 0x421b));
        
        /* Set BR ACQ1, ACQ2 kp=4 */
        /* DSC2B0_SM_CTRL4     uRegAddr = 0x8264 + (uLane*0x10) */
        /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, uRegAddr, 0x53e4) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x8264 + (0x10 * pc->lane_num), 0x53e4));
        
        /* Jitter performance improvement over PVT (process voltage temperature) */
        /* soc_bm9600_mdio_hc_read(pInitParams->m_nBaseAddr, uPhyAddr, 0, 0x805e, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_TXPLL_ANAPLLACONTROL4r(unit, pc, &data));
        
        /* uData |= 0x1000 */
        data |= 0x1000;
        
        /*soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, 0x805e, uData) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_TXPLL_ANAPLLACONTROL4r(unit, pc, data));
    }

        
    /* Set IQP to 200 uA  0x805c */
    SOC_IF_ERROR_RETURN
      (READ_HL65_TXPLL_ANAPLLACONTROL2r(unit, pc, &data));
    
    /* uData |= 0x0004 */
    data |= 0x0004;
    
    /*soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, 0x805c, uData) */
    SOC_IF_ERROR_RETURN
      (WRITE_HL65_TXPLL_ANAPLLACONTROL2r(unit, pc, data));


    
    /* PLL Sequencer enable */
    /* soc_bm9600_mdio_hc_write(pInitParams->m_nBaseAddr, uPhyAddr, 0, 0x8000, 0x262f) */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x262f));
    
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_init
 * Purpose:     
 *      Initialize hc phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_init(int unit, soc_port_t port)
{
    phy_ctrl_t         *pc;               
    uint16             xgxs_ctrl;
    uint16             vco_freq;
    int mode_1000x;
    uint16 id_rev;

    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        return _phy_hl65_fabric_init(unit, port);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    if (!PHY_EXTERNAL_MODE(unit, port)) {
         /* PHY reset callbacks */
        SOC_IF_ERROR_RETURN(soc_phy_reset(unit, port));

        if (soc_property_port_get(unit, port,
                                  spn_SERDES_FIBER_PREF, 1)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
        } else {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
        }


        if (soc_property_port_get(unit, port,
                              spn_PHY_AN_C73, FALSE)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
        }
    }

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        /* In this mode, the device's 4 lanes connect to 4 different ports.
         * It is necessary to configure and initialize the shared resource
         * once before going to each lane. It is assumed the port connecting
         * to lane 0 must be initialized first. 
         */

        if ((!PHY_FLAGS_TST(unit, port, PHY_FLAGS_INIT_DONE)) && 
            (pc->lane_num == 0)) {
            /* configure and initialize the resource shared by all 4 lanes*/

            SOC_IF_ERROR_RETURN(_hl65_soft_reset(unit,pc)); /* reset once */
                  
            /* Force to asymetric 1G/2.5G independent lane mode.
             * Support SGMII 10/100/100Mbps, 1000X, and 2500X speeds.
             */
            /* workaround for a Rx problem running at 10M bps(PR20236).
             * Use mode 6 for speed at 10/100/1000M bps, SGMII interface
             * Use mode 5 for speed at 1000/2500 bps, optical device.
             *
             * Rev B and up has fixed the problem. No need to switch mode
             * between fiber and SGMII.
             */

            SOC_IF_ERROR_RETURN
                (READ_HL65_TEST_SERDESID0r(unit,pc,&id_rev));
            if (!(id_rev & TEST_SERDESID0_REV_LETTER_MASK)) {

                mode_1000x = FALSE;
                if (PHY_FIBER_MODE(unit, port) && 
                              (!PHY_EXTERNAL_MODE(unit, port))) {
                    mode_1000x = TRUE;
                }

                /* Allow property to override */
                mode_1000x = soc_property_port_get(unit, port,
                               spn_SERDES_FIBER_PREF, mode_1000x);
     
                if (mode_1000x) {
                    xgxs_ctrl = 5;
                } else {       
                    xgxs_ctrl = 6;
                }
            } else {
                /* REV B and above, use mode 6 if speed =<1000 */
                if (pc->speed_max > 1000) {
                    xgxs_ctrl = 5;
                } else {
                    xgxs_ctrl = 6;
                }                   
            }
 
            /* allow mode override by configuration */

            xgxs_ctrl = soc_property_port_get(unit, port, 
                          spn_PHY_HL65_1LANE_MODE, xgxs_ctrl);
            if (IS_HG_PORT(unit, port)) {
                vco_freq = 0x7800;
            } else {
                vco_freq = xgxs_ctrl == 5? 0x7700:0x7500;
            }
            xgxs_ctrl <<= XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT;

            /* set device's mode and disable PLL sequencer */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 
                           xgxs_ctrl,
                           XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK |
                           XGXSBLK0_XGXSCONTROL_MODE_10G_MASK));

            /* broadcast to all lanes */ 
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_AERBLK_AERr(unit,pc, HL_AER_BCST_OFS_STRAP));

            /* configure VCO frequency */
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_DIGITAL_MISC1r(unit,pc,vco_freq));

            /* disable 10G parallel detect */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_AN73_PDET_PARDET10GCONTROLr(unit, pc,
                      0,
                      AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK));

            /* reset AER */
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_AERBLK_AERr(unit,pc, 0));

            /* enable PLL sequencer */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                             XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

            /* select recovery clock from lane0 for rxck0_10g */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK1_TESTTXr(unit,pc,
                       0,
                       XGXSBLK1_TESTTX_RX_CK4X1MUXSEL_MASK));
        }

        SOC_IF_ERROR_RETURN
            (_phy_hl65_independent_lane_init(unit, port));
    } else {
        SOC_IF_ERROR_RETURN(_hl65_soft_reset(unit,pc)); /* soft reset */

        /* Force to comboCore mode.
         * Support SGMII 10/100/1000Mbps, 1000X, 2500X, 10G, and 12G.
         */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_combo_core_init(unit, port));
    }  

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_init: u=%d p=%d\n"), unit, port));
    return SOC_E_NONE;
}

/*
 *      phy_hl65_link_get
 * Purpose:
 *      Get layer2 connection status.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      link - address of memory to store link up/down state.
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_link_get(int unit, soc_port_t port, int *link)
{
    uint16      mii_stat;
    phy_ctrl_t *pc;
 
    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    *link = FALSE;
    /* Check XAUI link first if XAUI mode */
    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (READ_HL65_DTE_IEEE0BLK_DTE_IEEESTATUS1r(unit, pc, &mii_stat));
        if (mii_stat & MII_STAT_LA) {
            *link = TRUE;
        }
    }

    /* Check combo link only if no XAUI link */
    if (*link == FALSE) {
        SOC_IF_ERROR_RETURN
            (READ_HL65_COMBO_IEEE0_MIISTATr(unit, pc, &mii_stat));
        if (mii_stat & MII_STAT_LA) {
            *link = TRUE;
        } else if (PHY_INDEPENDENT_LANE_MODE(unit,port)) {
            /* Check dxgxs link */
            if ((pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) && 
                    (IS_HG_PORT(unit,port) || IS_XE_PORT(unit,port))) {
                SOC_IF_ERROR_RETURN
                    (READ_HL65_DTE_IEEE0BLK_DTE_IEEESTATUS1r(unit, pc,
                                                           &mii_stat));
                if (mii_stat & MII_STAT_LA) {
                    *link = TRUE;
                }
            }
        }
    }

    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_hl65_enable_set
 * Purpose:
 *      Enable/Disable phy 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - on/off state to set
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_enable_set(int unit, soc_port_t port, int enable)
{
    int         rv;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    rv = SOC_E_NONE;
    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        if (enable) {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
            rv = _phy_hl65_notify_resume(unit, port, PHY_STOP_PHY_DIS);
        } else {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
            rv = _phy_hl65_notify_stop(unit, port, PHY_STOP_PHY_DIS);
        }
    } else {
        if (enable) {
            PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
            rv = MODIFY_HL65_XGXSBLK0_MISCCONTROL1r(unit, pc, 0,
                             XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK);
          
            /* enable Rx */
            rv = MODIFY_HL65_DSC2BB_DSC_MISC_CTRL0r(unit, pc, 0,
                             DSC2BB_DSC_MISC_CTRL0_RXSEQSTART_MASK);

        } else {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
            rv = MODIFY_HL65_XGXSBLK0_MISCCONTROL1r(unit, pc,
                             XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK,
                             XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK);

            /* disable Rx */
            rv = MODIFY_HL65_DSC2BB_DSC_MISC_CTRL0r(unit, pc, 
                             DSC2BB_DSC_MISC_CTRL0_RXSEQSTART_MASK,
                             DSC2BB_DSC_MISC_CTRL0_RXSEQSTART_MASK);
        }
    }
    return rv;
}

/*
 * Function:
 *      phy_hl65_enable_get
 * Purpose:
 *      Get Enable/Disable status
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of where to store on/off state
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_enable_get(int unit, soc_port_t port, int *enable)
{
    *enable = !PHY_FLAGS_TST(unit, port, PHY_FLAGS_DISABLE);

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_dsp_cfg (int unit, phy_ctrl_t *pc, int speed)
{
    uint16      serdes_id0;
    uint16      brcdr;

    /* For speed at 20G, configure the device for BR CDR.  */
    if (speed == 20000) {
        SOC_IF_ERROR_RETURN
            (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

        serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                     TEST_SERDESID0_REV_LETTER_MASK);

        if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
            (serdes_id0 == HL_SERDES_ID0_REVID_A1) ) {
            brcdr = HL65_A0A1_DSC_DFE_BRCDR;
        } else if ((serdes_id0 == HL_SERDES_ID0_REVID_B0) ||
                   (serdes_id0 == HL_SERDES_ID0_REVID_C0) ) {
            brcdr = HL65_B0C0_DSC_DFE_BRCDR;
        } else {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "HL65 : BR-CDR setting may not be correct: u=%d p=%d\n"),                        unit, pc->port));
            brcdr = HL65_B0C0_DSC_DFE_BRCDR;  /* assumed for future versions */
        }

        /* enable DFE/BR CDR */

        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DSC2BB_DSC_MISC_CTRL0r(unit, pc, brcdr));
    } else {
        /* default value for CDR */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DSC2BB_DSC_MISC_CTRL0r(unit, pc, 0x2000));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_fabric_mode_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - SBX device unit #.
 *      port - SBX device port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_fabric_mode_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16 data;
    int rv = SOC_E_NONE;
    uint16 serdes_id0;

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                            TEST_SERDESID0_REV_LETTER_MASK);

    switch (speed) {

    case 3125:
        /* Disable baud rate CDR  */
        /* uRegAddr = 0x826e + (uLaneAddr*0x10)*/
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, 0x2000) */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * pc->lane_num),
                            DSC2B0_DSC_MISC_CTRL0_ENABLE_ACOR_PICW_BITS <<
                            DSC2B0_DSC_MISC_CTRL0_ENABLE_ACOR_PICW_SHIFT));



        /* 6.25G rate half speed 0x603a */
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x8308, 0x603A)*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL_MISC1r(unit, pc, 0x603a));

        /* Set 6.25G speed through the backdoor method clock divisor is in shared lane register  */
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8016, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_XGXSBLK1_LANECTRL1r(unit, pc, &data));
        
        /* clear bits */
        data &= ~(0x0303 << (pc->lane_num * 2));
	data |= (0x0202 << (pc->lane_num * 2));
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8016, uData) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL1r(unit, pc, data));

        /* Set half-rate indicator for equalization algorithm to work */
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8065, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_TX0_ANATXACONTROL1r(unit, pc, &data));

        /*  uData &= ~(BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_MASK << BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_SHIFT) */
        /* uData |= (1 << BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_SHIFT) */
        data &= ~(TX0_ANATXACONTROL1_TX_SEL_HALFRATE_BITS <<  
                  TX0_ANATXACONTROL1_TX_SEL_HALFRATE_SHIFT);
        data |= (TX0_ANATXACONTROL1_TX_SEL_HALFRATE_BITS <<  
                 TX0_ANATXACONTROL1_TX_SEL_HALFRATE_SHIFT);

        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x8065, uData) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_TX0_ANATXACONTROL1r(unit, pc, data));
   
        break;
    
    case 6250:

        /* Force baud rate CDR */
        /* uRegAddr = 0x826e + (uLaneAddr*0x10) */
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, 0x2180) */
        if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
	    (serdes_id0 == HL_SERDES_ID0_REVID_A1)) {
	    SOC_IF_ERROR_RETURN
	        (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * pc->lane_num),
				0x2180));
	} else {
	    SOC_IF_ERROR_RETURN
	        (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * pc->lane_num),
				0x2300));
	}
  
        /* 6.25G rate 156.25MHz then mult by 40 + independent lane mode setting */
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x8308, 0x7720) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL_MISC1r(unit, pc, 0x7720));

        /* Set 6.25G speed through the backdoor method clock divisor is in shared lane register  */
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8016, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_XGXSBLK1_LANECTRL1r(unit, pc, &data));
        
        /* DWSDR_div1 = 0x3 for receive and transmit */
        /*  uData |= 0x0303 << (uLaneAddr * 2) */
        data |= 0x0303 << (pc->lane_num * 2);	    

        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8016, uData) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL1r(unit, pc, data));


        /* Clear half-rate indicator for equalization algorithm to work */
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8065, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_TX0_ANATXACONTROL1r(unit, pc, &data));

        /* uData &= ~(BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_MASK << BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_SHIFT) */
        /* uData |= (0 << BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_SHIFT) */
        data &= ~(TX0_ANATXACONTROL1_TX_SEL_HALFRATE_BITS <<  
                  TX0_ANATXACONTROL1_TX_SEL_HALFRATE_SHIFT);

        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x8065, uData) */	
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_TX0_ANATXACONTROL1r(unit, pc, data));

        break;


    case 6500:
        /* Jitter performance improvement over PVT - kvh_force=1 */

        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x805e, 0x1877) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_TXPLL_ANAPLLACONTROL4r(unit, pc, 0x1877));

    
        /* Force baud rate CDR */
        /* uRegAddr = 0x826e + (uLaneAddr*0x10) */
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, 0x2180) */
        if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
	    (serdes_id0 == HL_SERDES_ID0_REVID_A1)) {
	    SOC_IF_ERROR_RETURN
	        (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * pc->lane_num),
				0x2180));
	} else {
	    SOC_IF_ERROR_RETURN
	        (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * pc->lane_num),
				0x2300));
	}

        /* 6.5G independent lane mode setting */
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x8308, 0x7820) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL_MISC1r(unit, pc, 0x7820));


        /* Set 6.5G speed through the backdoor method clock divisor is in shared lane register  */
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8016, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_XGXSBLK1_LANECTRL1r(unit, pc, &data));

        /* DWSDR_div1 = 0x3 for receive and transmit */
        /* uData |= 0x0303 << (uLaneAddr * 2) */
        data  |= 0x0303 << (pc->lane_num * 2);	    

        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8016, uData) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL1r(unit, pc, data));


        /* Clear half-rate indicator for equalization algorithm to work */
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8065, &uData) */
        SOC_IF_ERROR_RETURN
            (READ_HL65_TX0_ANATXACONTROL1r(unit, pc, &data));

        /* uData &= ~(BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_MASK << BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_SHIFT) */
        /* uData |= (0 << BM9600_HYPERCORE_8065_TX_SEL_HALF_RATE_SHIFT) */
        data &= ~(TX0_ANATXACONTROL1_TX_SEL_HALFRATE_BITS <<  
                  TX0_ANATXACONTROL1_TX_SEL_HALFRATE_SHIFT);
    
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x8065, uData) */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_TX0_ANATXACONTROL1r(unit, pc, data));

        break;

    default:
        rv = SOC_E_PARAM;
    }

    return rv;
}


/*
 * Function:
 *      phy_hl65_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t  *pc;
    uint16       speed_val, mask;
    uint16       speed_mii;
    uint16       sgmii_status = 0;
    int          hg10g_port = FALSE;
    int          rxaui;

    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        return phy_hl65_fabric_mode_speed_set(unit, port, speed);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_INDEPENDENT_LANE_MODE(unit, port) && (speed > 12000)) {
        return SOC_E_PARAM;
    } 

    /* check if RXAUI is set in combo mode */
    rxaui = soc_property_port_get(unit, port,
                                    spn_SERDES_2WIRE_XAUI, FALSE);

    /* HC rev D0 supports 10G and 12G speeds in dxgxs mode in ind. lane mode */
    if (pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) {
        rxaui = TRUE;
    }

    if (IS_HG_PORT(unit, port) &&
        soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE) == FALSE) {
        hg10g_port = TRUE; 
    }

    speed_val = 0;
    speed_mii = 0;
    mask      =  DIGITAL_MISC1_FORCE_SPEED_MASK;
    switch (speed) {
    case 0:
        /* Do not change speed */
        return SOC_E_NONE;
    case 10:
        speed_mii = MII_CTRL_SS_10;
        break;
    case 100:
        speed_mii = MII_CTRL_SS_100;
        break;
    case 1000:
        speed_mii = MII_CTRL_SS_1000;
        break;
    case 2500:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_2500BRCM_X1; 
        break;
    case 10000:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_10GBASE_CX4; /* 10G CX4 */
        if ((hg10g_port == TRUE) && rxaui) {
            speed_val = 0x21;  /* 10.5HiG dual-XGXS */
        } else if (rxaui) {
            speed_val = 0x20; /* 10G ethernet dual-XGXS */
        } else if (hg10g_port == TRUE) {
            speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_10GHiGig_X4; /* 10G HiG */
        }
        break;
    case 12000:
        speed_val = rxaui? 0x23: /* 12.7HiG dual-XGXS*/
                     DIGITAL_MISC1_FORCE_SPEED_dr_12GHiGig_X4;   /* 12 HiG */
        break;
    case 13000:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_13GHiGig_X4;
        break;
    case 16000:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_16GHiGig_X4;
        break;
    case 20000:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_20GHiGig_X4;
        break;
    case 21000:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_21GHiGig_X4;
        break;
    case 25000:
        speed_val = DIGITAL_MISC1_FORCE_SPEED_dr_25p45GHiGig_X4;
        break;
    default:
        return SOC_E_PARAM;
    }

    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        /* set scrambler controls */
        if (speed == 20000) {
            SOC_IF_ERROR_RETURN
                 (MODIFY_HL65_XGXSBLK0_XGMIIRCONTROLr(unit,pc,
                       XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK,
                       XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK));
        } else {
            SOC_IF_ERROR_RETURN
                 (MODIFY_HL65_XGXSBLK0_XGMIIRCONTROLr(unit,pc,
                       0,
                       XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK));
        }
        _phy_hl65_dsp_cfg(unit,pc,speed); 

        /* Puts PLL in reset state and forces all datapath into reset state */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

        /* 2wire XAUI configuration */
        SOC_IF_ERROR_RETURN
            (_hl65_rxaui_config(unit,pc,rxaui));

    } else {
        /* Hold rxSeqStart */
        SOC_IF_ERROR_RETURN
            (HL65_REG_MODIFY(unit,pc,0x0,0x826E + (0x10 * pc->lane_num),
                              DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK,
                              DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));

        /* hold TX FIFO in reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_CONTROL1000X3r(unit, pc, 
                               DIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK,
                               DIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK));
    }
 
    /* disable 100FX and 100FX auto-detect */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_FX100_CONTROL1r(unit,pc,
                              0,
                              FX100_CONTROL1_AUTODET_EN_MASK |
                              FX100_CONTROL1_ENABLE_MASK));

    /* disable 100FX idle detect */
    SOC_IF_ERROR_RETURN
            (MODIFY_HL65_FX100_CONTROL3r(unit,pc,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, speed_val, mask));

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL4_MISC3r(unit, pc, 
                (speed_val & 0x20)?  DIGITAL4_MISC3_FORCE_SPEED_B5_MASK:0,
                DIGITAL4_MISC3_FORCE_SPEED_B5_MASK));

    if (speed <= 1000) {
        SOC_IF_ERROR_RETURN
            (READ_HL65_DIGITAL_STATUS1000X1r(unit, pc, &sgmii_status));

        sgmii_status &= DIGITAL_STATUS1000X1_SGMII_MODE_MASK;
        if (!sgmii_status && (speed == 100)) {

            /* fiber mode 100fx, enable */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_FX100_CONTROL1r(unit,pc,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK));

            /* enable 100fx extended packet size */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_FX100_CONTROL2r(unit,pc,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, speed_mii,
                                              MII_CTRL_SS_MASK));
        }
    }

    /* for speed above 10G, txcko_div must be cleared */
    speed_val = (speed > 10000) ? 0: XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, speed_val, 
                                     XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK));

    /* set tx half rate based on speed */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_half_rate_set(unit,port,speed));

    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        /* Bring PLL out of reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
        (void) _phy_hl65_pll_lock_wait(unit, port);
    } else {
        /* release rxSeqStart */
        SOC_IF_ERROR_RETURN
            (HL65_REG_MODIFY(unit,pc,0x0,0x826E + (0x10 * pc->lane_num),
                              0,
                              DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_MASK));

        /* release TX FIFO reset */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_CONTROL1000X3r(unit, pc, 
                               0,
                               DIGITAL_CONTROL1000X3_TX_FIFO_RST_MASK));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_tx_fifo_reset(int unit, soc_port_t port,uint32 speed)
{
    uint16 data16;
    phy_ctrl_t  *pc;
    pc = INT_PHY_SW_STATE(unit, port);

    if (speed == 100) {
        /* check if it is in 100fx mode */
        SOC_IF_ERROR_RETURN
            (READ_HL65_FX100_CONTROL1r(unit,pc,&data16));

        if (data16 & FX100_CONTROL1_ENABLE_MASK) {
            /* reset TX FIFO  */
            SOC_IF_ERROR_RETURN
                (HL65_REG_MODIFY(unit,pc,0x0,0x8061 + (0x10 * pc->lane_num),
                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK,
                                 TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK));

            SOC_IF_ERROR_RETURN
                (HL65_REG_MODIFY(unit,pc,0x0,0x8061 + (0x10 * pc->lane_num),
                              0,
                              TX0_ANATXACONTROL0_TX1G_FIFO_RST_MASK));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_fabric_mode_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - SBX device unit #.
 *      port - SBX device port #. 
 *      speed - link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_fabric_mode_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t  *pc;
    uint16 data;

    pc = INT_PHY_SW_STATE(unit, port);

    /* read speed from hardware */
    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8308, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL_MISC1r(unit, pc, &data));

    if (data == 0x7720) {
	*speed = 6250;
    } else if (data == 0x7820) {
	*speed = 6500;
    } else {
	*speed = 3125;
    }
    return SOC_E_NONE;

}

/*
 * Function:
 *      phy_hl65_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      speed - current link speed in Mbps
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_hl65_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16 speed_val;

    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        return phy_hl65_fabric_mode_speed_get(unit, port, speed);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        uint16      sd_stat;

        SOC_IF_ERROR_RETURN
            (READ_HL65_DIGITAL_STATUS1000X1r(unit, pc, &sd_stat));
        sd_stat = (sd_stat & DIGITAL_STATUS1000X1_SPEED_STATUS_MASK)
                      >> DIGITAL_STATUS1000X1_SPEED_STATUS_SHIFT;
        switch (sd_stat) {
        case 2:
            *speed = 1000;
            break;
        case 3:
            *speed = 2500;
            break;
        case 0:
            *speed = 10;
            break;
        case 1:
            *speed = 100;
            break;
        default:
            *speed = 0;
        }
        /* check dxgxs speeds 
         * There is no speed status for second dxgxs block. We'll simply
         * check what the speed is set. It should be OK since the dxgxs mode
         * does not support autoneg 
         */
        if ((pc->phy_mode == PHYCTRL_DUAL_LANE_PORT) && 
            (IS_HG_PORT(unit,port) || IS_XE_PORT(unit,port))) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_DIGITAL4_MISC3r(unit, pc, &speed_val));
            if (speed_val & DIGITAL4_MISC3_FORCE_SPEED_B5_MASK) {
                SOC_IF_ERROR_RETURN
                    (READ_HL65_DIGITAL_MISC1r(unit, pc, &speed_val));
                if ((speed_val & DIGITAL_MISC1_FORCE_SPEED_MASK) == 3) {
                    *speed=12000;
                } else if ((speed_val & DIGITAL_MISC1_FORCE_SPEED_MASK) == 0 ||
                           (speed_val & DIGITAL_MISC1_FORCE_SPEED_MASK) == 1) {
                    *speed=10000;
                }
            }
        }
    } else {
        uint16      xgxs_stat;
        uint16   xgxs20g_stat = 0;

        SOC_IF_ERROR_RETURN
            (READ_HL65_GP_STATUS_XGXSSTATUS1r(unit, pc, &xgxs_stat));

        switch (xgxs_stat & GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_MASK) {
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_10M:
            *speed = 10;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_100M:
            *speed = 100;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_1G:
            *speed = 1000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_2p5G:
            *speed = 2500;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_5G_X4:
            *speed = 5000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_6G_X4:
            *speed = 6000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_10G_HiG:
            /* fall through */
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_10G_CX4:
            *speed = 10000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_12G_HiG:
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_12p5G_X4:
            *speed = 12000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_13G_X4:
            *speed = 13000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_15G_X4:
            *speed = 15000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_16G_X4:
            *speed = 16000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_1G_KX:
            *speed = 1000;
            break;
        case GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_dr_10G_KX4:
            *speed = 10000;
            break;
        default:
            *speed = 0;
        }

        /* check 20G and above and dxgxs speed */
        SOC_IF_ERROR_RETURN
          (READ_HL65_AN73_PDET_XGXSSTATUS4r(unit, pc, &xgxs20g_stat));
        xgxs20g_stat &= AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_MASK;
        switch (xgxs20g_stat) {
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_20G_X4:
                *speed=20000;
                break;
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_21G_X4:
                *speed=21000;
                break;
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_25G_X4:
                *speed=25000;
                break;
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_HiG_DXGXS:
                /* fall through */
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10G_DXGXS:
                /* fall through */
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p5G_HiG_DXGXS:
                /* fall through */
            case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_10p5G_DXGXS:
                *speed = 10000;
                break;
           case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p773G_HiG_DXGXS:                    /* fall through */
           case AN73_PDET_XGXSSTATUS4_ACTUAL_SPEED_LN0_dr_12p773G_DXGXS:
                *speed = 12000;
                break;
            default:
                break;
        }

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_hl65_speed_get: u=%d p=%d GP_STATUS_TOPANSTATUS1 %04x speed= %d\n"),
                  unit, port,
                  ((xgxs_stat & GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_MASK) >>
                  GP_STATUS_XGXSSTATUS1_ACTUAL_SPEED_LN0_SHIFT), *speed));

    }
    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_hl65_an_set
 * Purpose:     
 *      Enable or disabled auto-negotiation on the specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - Boolean, if true, auto-negotiation is enabled 
 *              (and/or restarted). If false, autonegotiation is disabled.
 * Returns:     
 *      SOC_E_XXX
 */
int
phy_hl65_an_set(int unit, soc_port_t port, int an)
{
    phy_ctrl_t  *pc;
    uint16             an_enable;
    uint16             auto_det;
    uint16 data16;
    uint16 mask16;

    pc = INT_PHY_SW_STATE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_an_set: u=%d p=%d an=%d\n"),
              unit, port, an));

    an_enable = 0;
    auto_det  = 0;

    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        data16 = 0;
        mask16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
        if (soc_property_port_get(unit, port, spn_XGXS_PDETECT_10G, 1) && an) {
            data16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
        }
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_AN73_PDET_PARDET10GCONTROLr(unit, pc, data16,
                                                         mask16));

        /* Always enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_CONTROL1000X2r(unit, pc, 
              DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK, 
              DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
    }

    if (an) {

        /* disable 100FX and 100FX auto-detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_FX100_CONTROL1r(unit,pc,
                                  0,
                                  FX100_CONTROL1_AUTODET_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK));

        /* disable 100FX idle detect */
        SOC_IF_ERROR_RETURN
                (MODIFY_HL65_FX100_CONTROL3r(unit,pc,
                                  FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                                  FX100_CONTROL3_CORRELATOR_DISABLE_MASK));

        /* set tx half rate with default value before starting AN */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_half_rate_set(unit,port,0)); 

        an_enable = MII_CTRL_AE | MII_CTRL_RAN;

        /*
         * Should read one during init and cache it in Phy flags
         */
        if (soc_property_port_get(unit, port,
                                  spn_SERDES_AUTOMEDIUM,
                                  (PHY_INDEPENDENT_LANE_MODE(unit, port)) ?
                                  FALSE : TRUE)) {
            auto_det = DIGITAL_CONTROL1000X1_AUTODET_EN_MASK; 
        }

        /* If auto-neg is enabled, make sure not forcing any speed */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_MISC1r(unit, pc, 0, 
                                   DIGITAL_MISC1_FORCE_SPEED_MASK));
        /* Enable/Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_HL65_DIGITAL_CONTROL1000X1r(unit, pc, auto_det,
                                 DIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

        if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {

            /* 
             * Set the default value for DSP related registers which may be
             * changed for 20G forced speed mode, otherwise AN won't work 
             */
            /* clear scrambler controls */
            SOC_IF_ERROR_RETURN
                 (MODIFY_HL65_XGXSBLK0_XGMIIRCONTROLr(unit,pc,
                       0,
                       XGXSBLK0_XGMIIRCONTROL_SCR_EN_4LANE_MASK));

            _phy_hl65_dsp_cfg(unit,pc,0); 

            /* clear 2wire XAUI configuration */
            SOC_IF_ERROR_RETURN
                (_hl65_rxaui_config(unit,pc,FALSE));

            /* only in combo mode, reset the sequence. In independent mode,
             * resetting sequence affects all lanes
             */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));


            /* set BAM enable */
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,1));

            /* enable autoneg */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, MII_CTRL_AE,
                                      MII_CTRL_AE));
            if (PHY_CLAUSE73_MODE(unit, port)) { 
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 
                                              MII_CTRL_AE,
                                              MII_CTRL_AE));
            }
            /* restart the sequence */
            SOC_IF_ERROR_RETURN
                 (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
            /* wait for PLL lock */
            _phy_hl65_pll_lock_wait(unit, port);
        }
    } else {
        /* Disable auto detect */
        SOC_IF_ERROR_RETURN
             (MODIFY_HL65_DIGITAL_CONTROL1000X1r(unit, pc, auto_det,
                                 DIGITAL_CONTROL1000X1_AUTODET_EN_MASK));

        if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

            /* reset BAM enable */
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_DIGITAL6_MP5_NEXTPAGECTRLr(unit,pc,0));

            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, 0,
                                         MII_CTRL_AE));
            if (PHY_CLAUSE73_MODE(unit, port)) { 
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, 
                                              0,
                                              MII_CTRL_AE));
            }
            SOC_IF_ERROR_RETURN
                 (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, 
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                              XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
            /* wait for PLL lock */
            _phy_hl65_pll_lock_wait(unit, port);
        }
    }

    /* restart the autoneg if enabled, or disable the autoneg */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, an_enable,
                                      MII_CTRL_AE | MII_CTRL_RAN));

    if (PHY_CLAUSE73_MODE(unit, port)) { 
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, an_enable,
                                      MII_CTRL_AE | MII_CTRL_RAN));
    }
    pc->fiber.autoneg_enable = an;

    return SOC_E_NONE;
}

/*
 * Function:    
 *      phy_hl65_an_get
 * Purpose:     
 *      Get the current auto-negotiation status (enabled/busy)
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      an   - (OUT) if true, auto-negotiation is enabled.
 *      an_done - (OUT) if true, auto-negotiation is complete. This
 *              value is undefined if an == false.
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
phy_hl65_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      mii_stat;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    SOC_IF_ERROR_RETURN
        (READ_HL65_COMBO_IEEE0_MIISTATr(unit, pc, &mii_stat));

    *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
    *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
    if (!((*an == TRUE) && (*an_done == TRUE))) {
        if (PHY_CLAUSE73_MODE(unit, port)) { 
            /* check clause 73 */
            SOC_IF_ERROR_RETURN
                (READ_HL65_AN_IEEE0BLK_AN_IEEECONTROL1r(unit, pc, &mii_ctrl));
            SOC_IF_ERROR_RETURN
                (READ_HL65_AN_IEEE0BLK_AN_IEEESTATUS1r(unit, pc, &mii_stat));

            *an = (mii_ctrl & MII_CTRL_AE) ? TRUE : FALSE;
            *an_done = (mii_stat & MII_STAT_AN_DONE) ? TRUE : FALSE;
        }
    }
    return SOC_E_NONE; 
}

STATIC int
_phy_hl65_c73_adv_local_set(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    uint16            pause;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? 
              CL73_AN_ADV_TECH_1G_KX : 0;
    an_adv |= (ability->speed_full_duplex & SOC_PA_SPEED_10GB) ? 
              CL73_AN_ADV_TECH_10G_KX4 : 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, pc, an_adv,
                                     CL73_AN_ADV_TECH_SPEEDS_MASK));

    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        pause = CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        pause = CL73_AN_ADV_PAUSE;
        break;
    default:
        pause = 0;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, pc, pause,
                                     (CL73_AN_ADV_PAUSE |
                                      CL73_AN_ADV_ASYM_PAUSE)));
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_hl65_c73_adv_local_set: u=%d p=%d pause=%08x speeds=%04x,adv=0x%x\n"),
              unit, port, pause, an_adv,ability->speed_full_duplex));
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_c73_adv_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   speeds,pause;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_AN_IEEE1BLK_AN_ADVERTISEMENT1r(unit, pc, &an_adv));

    speeds = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= speeds;

    SOC_IF_ERROR_RETURN
        (READ_HL65_AN_IEEE1BLK_AN_ADVERTISEMENT0r(unit, pc, &an_adv));

    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            pause = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            pause = SOC_PA_PAUSE_RX;
            break;
        default:
            pause = 0;
    }
    ability->pause = pause;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_hl65_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, pause, speeds));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_ability_advert_set
 * Purpose:
 *      Set the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is set only for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_hl65_ability_advert_set(int unit, soc_port_t port,
                       soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           an_sp_20g;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    /*
     * Set advertised duplex (only FD supported).
     */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? MII_ANA_C37_FD : 0;

    /*
     * Set advertised pause bits in link code word.
     */
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANA_C37_PAUSE;
        break;
    }
    /* Update less than 1G capability */ 
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_COMBO_IEEE0_AUTONEGADVr(unit, pc, an_adv));
  
    mode = ability->speed_full_duplex;
    an_adv = 0;
    an_sp_20g = 0;
    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        an_adv |= (mode & SOC_PA_SPEED_2500MB) ? 
                    DIGITAL3_UP1_DATARATE_2P5GX1_MASK : 0;
        an_adv |= (mode & SOC_PA_SPEED_12GB) ? 
                    DIGITAL3_UP1_DATARATE_12GX4_MASK : 0;
        an_adv |= (mode & SOC_PA_SPEED_13GB) ? 
                    DIGITAL3_UP1_DATARATE_13GX4_MASK : 0;
        an_adv |= (mode & SOC_PA_SPEED_16GB) ? 
                    DIGITAL3_UP1_DATARATE_16GX4_MASK : 0;

        /* 20G operation in autoneg is not reliable.
         * Use forced mode only. 
         *an_adv |= (mode & SOC_PA_SPEED_20GB) ? 
         *          DIGITAL3_UP1_DATARATE_20GX4_MASK : 0;
         */

        an_sp_20g |= (mode & SOC_PA_SPEED_21GB) ? 
                      DIGITAL3_UP3_DATARATE_21GX4_MASK : 0;
        an_sp_20g |= (mode & SOC_PA_SPEED_25GB) ? 
                      DIGITAL3_UP3_DATARATE_25P45GX4_MASK : 0;
    }

    if (mode & SOC_PA_SPEED_10GB) {
        if (IS_HG_PORT(unit, port)) {
            /* For Draco and Hercules, use pre-CX4 signalling */
            an_adv |= DIGITAL3_UP1_DATARATE_10GX4_MASK; 
            if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
                /* Also include 10G CX4 signalling by default */
                an_adv |= DIGITAL3_UP1_DATARATE_10GCX4_MASK; 
            }
        } else {
            an_adv |= DIGITAL3_UP1_DATARATE_10GCX4_MASK; 
        }
    }
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_DIGITAL3_UP1r(unit, pc, an_adv));

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL3_UP3r(unit, pc, an_sp_20g,
                   OVER1G_UP3_20GPLUS_MASK));

    if (PHY_CLAUSE73_MODE(unit, port)) { 
        SOC_IF_ERROR_RETURN
            (_phy_hl65_c73_adv_local_set(unit, port, ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_ability_advert_set: u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
              unit, port, ability->pause, an_adv));
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_ability_advert_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_hl65_ability_advert_get(int unit, soc_port_t port,
                           soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           an_sp_20g;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));
    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL3_UP1r(unit, pc, &an_adv));

    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL3_UP3r(unit, pc, &an_sp_20g));

    mode = 0;
    mode |= (an_sp_20g & DIGITAL3_UP3_DATARATE_25P45GX4_MASK) ?
              SOC_PA_SPEED_25GB : 0;
    mode |= (an_sp_20g & DIGITAL3_UP3_DATARATE_21GX4_MASK) ?
              SOC_PA_SPEED_21GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_20GX4_MASK) ? SOC_PA_SPEED_20GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_16GX4_MASK) ? SOC_PA_SPEED_16GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_13GX4_MASK) ? SOC_PA_SPEED_13GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_12GX4_MASK) ? SOC_PA_SPEED_12GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_10GCX4_MASK) ? SOC_PA_SPEED_10GB: 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_10GX4_MASK) ? SOC_PA_SPEED_10GB : 0;
    mode |= (an_adv & DIGITAL3_UP1_DATARATE_2P5GX1_MASK) ? 
                    SOC_PA_SPEED_2500MB : 0;

    SOC_IF_ERROR_RETURN
        (READ_HL65_COMBO_IEEE0_AUTONEGADVr(unit, pc, &an_adv));
      
    mode |= (an_adv & MII_ANA_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
    ability->speed_full_duplex = mode;

    mode = 0;
    switch (an_adv & (MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE)) {
        case MII_ANA_C37_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case MII_ANA_C37_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case MII_ANA_C37_PAUSE | MII_ANA_C37_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    /* check for clause73 */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_c73_adv_local_get(unit, port, ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_ability_advert_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_c73_adv_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);
     SOC_IF_ERROR_RETURN
        (READ_HL65_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY1r(unit, pc, &an_adv));

    mode = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= mode;

    SOC_IF_ERROR_RETURN
        (READ_HL65_AN_IEEE1BLK_AN_LP_BASEPAGEABILITY0r(unit, pc, &an_adv));

    mode = 0;
    switch (an_adv & (CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE)) {
        case CL73_AN_ADV_PAUSE:
            mode = SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
            break;
        case CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_TX;
            break;
        case CL73_AN_ADV_PAUSE | CL73_AN_ADV_ASYM_PAUSE:
            mode = SOC_PA_PAUSE_RX;
            break;
    }
    ability->pause = mode;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_ability_remote_get
 * Purpose:
 *      Get the current advertisement for auto-negotiation.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      mode - (OUT) Port mode mask indicating supported options/speeds.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      The advertisement is retrieved for the ACTIVE medium.
 *      No synchronization performed at this level.
 */

STATIC int
phy_hl65_ability_remote_get(int unit, soc_port_t port,
                        soc_port_ability_t *ability)
{
    uint16           an_adv;
    uint16           an_sp_20g;
    uint16           data16;
    int              an_enable;
    int              link_1000x = FALSE;
    int              link_combo = FALSE;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, &data16));

    an_enable = (data16 & MII_CTRL_AE) ? TRUE : FALSE;

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (READ_HL65_DIGITAL_STATUS1000X1r(unit, pc, &data16));
        if (data16 & DIGITAL_STATUS1000X1_LINK_STATUS_MASK) {
            link_1000x = TRUE;
        }
    } else {
        SOC_IF_ERROR_RETURN
            (READ_HL65_GP_STATUS_XGXSSTATUS1r(unit, pc, &data16));
        if (data16 & (GP_STATUS_XGXSSTATUS1_LINKSTAT_MASK |
                               GP_STATUS_XGXSSTATUS1_LINK10G_MASK)) {
            link_combo = TRUE;
        }
    } 

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d an_enable=%04x link_1000x=%04x link_combo=%04x\n"),
              unit, port, an_enable, link_1000x,link_combo));

    sal_memset(ability, 0, sizeof(*ability));
    mode = 0;
    if (an_enable && (link_1000x || link_combo)) {
        /* Decode remote advertisement only when link is up and autoneg is 
         * completed.
         */
 
        SOC_IF_ERROR_RETURN
            (READ_HL65_GP_STATUS_LP_UP3r(unit, pc, &an_sp_20g));

        SOC_IF_ERROR_RETURN
            (READ_HL65_GP_STATUS_LP_UP1r(unit, pc, &an_adv));

        mode |= (an_sp_20g & GP_STATUS_LP_UP3_DATARATE_25P45GX4_MASK)?
                SOC_PA_SPEED_25GB: 0;
        mode |= (an_sp_20g & GP_STATUS_LP_UP3_DATARATE_21GX4_MASK)?
                SOC_PA_SPEED_21GB: 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_20GX4_MASK) ? 
                SOC_PA_SPEED_20GB : 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_16GX4_MASK) ? 
                SOC_PA_SPEED_16GB : 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_13GX4_MASK) ? 
                SOC_PA_SPEED_13GB : 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_12GX4_MASK) ? 
                SOC_PA_SPEED_12GB : 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_10GCX4_MASK) ? 
                SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_10GX4_MASK) ? 
                SOC_PA_SPEED_10GB : 0;
        mode |= (an_adv & GP_STATUS_LP_UP1_DATARATE_2P5GX1_MASK) ?
                 SOC_PA_SPEED_2500MB : 0;

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d over1G an_adv=%04x\n"),
                  unit, port, an_adv));

        SOC_IF_ERROR_RETURN
            (READ_HL65_COMBO_IEEE0_AUTONEGLPABILr(unit, pc, &an_adv));

        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "u=%d p=%d combo an_adv=%04x\n"),
                  unit, port, an_adv));

        mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
        ability->speed_full_duplex = mode;
          
        switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
            case MII_ANP_C37_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                break;
            case MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_TX;
                break;
            case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                ability->pause |= SOC_PA_PAUSE_RX;
                break;
        }
        if (PHY_CLAUSE73_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_AN_IEEE0BLK_AN_IEEESTATUS1r(unit, pc, &data16));
            if (data16 & MII_STAT_AN_DONE) {
                SOC_IF_ERROR_RETURN
                    (_phy_hl65_c73_adv_remote_get(unit, port, ability));
            }
        }
    } else {
        /* Simply return local abilities */
        phy_hl65_ability_advert_get(unit, port, ability);
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_lb_set
 * Purpose:
 *      Put hc/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_lb_set(int unit, soc_port_t port, int enable)
{
    phy_ctrl_t *pc;
    uint16 serdes_id0;

    pc = INT_PHY_SW_STATE(unit, port);
    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                                TEST_SERDESID0_REV_LETTER_MASK);

    if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_A1) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_B0)) {
        SOC_IF_ERROR_RETURN(MODIFY_HL65_DSC2BB_SM_CTRL0r(unit, pc, 
                       enable? 0x0: DSC2BB_SM_CTRL0_TUNING_SM_EN_MASK,
                       DSC2BB_SM_CTRL0_TUNING_SM_EN_MASK));
    }

    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        uint16 lane_mask = 1 << pc->lane_num;
        
        if (enable) {
            /* Set global and individual loopback control */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_IEEE0BLK_MIICNTLr(unit, pc, MII_CTRL_LE,
                                               MII_CTRL_LE));

            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK1_LANECTRL2r(unit, pc, lane_mask, lane_mask));
        } else {
            /* Clear global and individual loopback control */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_IEEE0BLK_MIICNTLr(unit, pc, 0,
                                               MII_CTRL_LE));
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK1_LANECTRL2r(unit, pc, 0, lane_mask));
        }
    } else if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        if (enable) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, 
                                                  MII_CTRL_LE,
                                                  MII_CTRL_AE | MII_CTRL_RAN |
                                                  MII_CTRL_LE));
            if(!IS_GE_PORT(unit, port)) {
                /* in case it is in dxgxs mode */
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_DTE_IEEE0BLK_DTE_IEEECONTROL1r(unit, pc, 
                                  MII_CTRL_LE,MII_CTRL_LE));
            }
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, 
                                                  0,
                                                  MII_CTRL_LE));
            if(!IS_GE_PORT(unit, port)) {
                /* in case it is in dxgxs mode */
                SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_DTE_IEEE0BLK_DTE_IEEECONTROL1r(unit, pc, 
                                         0,MII_CTRL_LE));
            }
        }
    } else {
        uint16      misc_ctrl;
        uint16      lb_bit;
        uint16      lb_mask;
        
        if (enable) {
            /* Disable TX serializer output in gloopback mode. Link partner will
             * show link off
             */
            SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_TXB_ANATXACONTROL0r(unit, pc, 0x200,0x200));
            
        } else {
            SOC_IF_ERROR_RETURN
                    (MODIFY_HL65_TXB_ANATXACONTROL0r(unit, pc, 0x000,0x200));
        }
        /* Configure Loopback in XAUI */
        SOC_IF_ERROR_RETURN
            (READ_HL65_XGXSBLK0_MISCCONTROL1r(unit, pc, &misc_ctrl));
        if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PCS_DEV_EN_OVERRIDE_MASK) {
              /* PCS */
              lb_bit  = (enable) ? IEEE0BLK_MIICNTL_GLOOPBACK_MASK : 0;
              lb_mask = IEEE0BLK_MIICNTL_GLOOPBACK_MASK;
              SOC_IF_ERROR_RETURN
                  (MODIFY_HL65_DTE_IEEE0BLK_DTE_IEEECONTROL1r(unit, pc, 
                                                           lb_bit, lb_mask));
        } else if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PMD_DEV_EN_OVERRIDE_MASK) {
              /* PMA/PMD */
              lb_bit  = (enable) ? 1 : 0;
              lb_mask = 1;
              SOC_IF_ERROR_RETURN
                  (MODIFY_HL65_PMD_IEEE0BLK_PMD_IEEECONTROL1r(unit, pc, 
                                                           lb_bit, lb_mask));
        } else {
              /* PHY XS, DTE XS */
              lb_bit  = (enable) ? IEEE0BLK_MIICNTL_GLOOPBACK_MASK : 0;
              lb_mask = IEEE0BLK_MIICNTL_GLOOPBACK_MASK;
              SOC_IF_ERROR_RETURN
                  (MODIFY_HL65_DTE_IEEE0BLK_DTE_IEEECONTROL1r(unit, pc, 
                                                           lb_bit, lb_mask));
        }

        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, 
                                              (enable) ?  MII_CTRL_LE : 0,
                                              MII_CTRL_LE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_lb_get
 * Purpose:
 *      Get hc/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_hl65_lb_get(int unit, soc_port_t port, int *enable)
{
    phy_ctrl_t *pc;
    uint16      mii_ctrl;
 
    pc = INT_PHY_SW_STATE(unit, port);

   if (PHY_HC65_FABRIC_MODE(unit, port)) {
        uint16 data;
        uint16 lane_mask = 1 << pc->lane_num;

        SOC_IF_ERROR_RETURN
            (READ_HL65_XGXSBLK1_LANECTRL2r(unit, pc, &data));
 
        *enable = ((data & lane_mask) == lane_mask);
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "phy_hc65_loopback_get: "
                             "u=%d p=%d lb=%d\n"), unit, port, *enable));

    } else {
        SOC_IF_ERROR_RETURN
            (READ_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

        *enable = mii_ctrl & MII_CTRL_LE;
    }
    return SOC_E_NONE;
}

STATIC int
phy_hl65_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(pif);

    return SOC_E_NONE;
}

STATIC int
phy_hl65_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        *pif = SOC_PORT_IF_SGMII;
    } else {
        *pif = SOC_PORT_IF_XGMII;
    }
    return SOC_E_NONE;
}

STATIC int
phy_hl65_ability_local_get(int unit, soc_port_t port, soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;
    uint16 id_rev;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));

    if (PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        /* dxgxs mode */
        if (IS_HG_PORT(unit, port)) {
            ability->speed_full_duplex  = SOC_PA_SPEED_10GB |
                                           SOC_PA_SPEED_12GB;
            ability->pause     = 0;
            ability->interface = SOC_PA_INTF_XGMII;
            ability->medium    = SOC_PA_MEDIUM_FIBER;
            ability->loopback  = SOC_PA_LB_PHY;
            ability->flags     = 0;
        } else {
            ability->speed_full_duplex  = SOC_PA_SPEED_1000MB;
            if (IS_XE_PORT(unit,port)) {
                ability->speed_full_duplex  += SOC_PA_SPEED_10GB;
            }

            ability->speed_half_duplex = SOC_PA_ABILITY_NONE;
            if (PHY_FIBER_MODE(unit, port))   {
                ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
                SOC_IF_ERROR_RETURN
                    (READ_HL65_TEST_SERDESID0r(unit,pc,&id_rev));
                if (id_rev & TEST_SERDESID0_REV_LETTER_MASK) {
                    ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
                    ability->speed_half_duplex  = SOC_PA_SPEED_100MB;
                }
            } else {
                ability->speed_half_duplex  = SOC_PA_SPEED_10MB |
                                              SOC_PA_SPEED_100MB;
                ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                           SOC_PA_SPEED_100MB;
            }

            ability->interface = SOC_PA_INTF_GMII | SOC_PA_INTF_SGMII;
            ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
            ability->medium    = SOC_PA_MEDIUM_FIBER;
            ability->loopback  = SOC_PA_LB_PHY;
            ability->flags     = SOC_PA_AUTONEG;
       }
    } else {

        if ( PHY_EXTERNAL_MODE(unit, port) ) {
            ability->speed_half_duplex  = SOC_PA_SPEED_100MB | 
                                          SOC_PA_SPEED_10MB;
            ability->speed_full_duplex  = SOC_PA_SPEED_2500MB | 
                                          SOC_PA_SPEED_1000MB |
                                          SOC_PA_SPEED_100MB | 
                                          SOC_PA_SPEED_10MB;
        } else {
            ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
            ability->speed_full_duplex  = SOC_PA_SPEED_1000MB | 
                                          SOC_PA_SPEED_2500MB;
        }

        switch(pc->speed_max) {
            case 25000:
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
                /* fall through */
            case 21000:
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
                /* fall through */
            case 20000:
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
                /* fall through */
            case 16000:
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB; 
                /* fall through */
            case 13000:
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
                /* fall through */
            case 12000:
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
                /* fall through */
            default:
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB; 
        }
        ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
        ability->interface = SOC_PA_INTF_XGMII;
        ability->medium    = SOC_PA_MEDIUM_FIBER;
        ability->loopback  = SOC_PA_LB_PHY;
        ability->flags     = SOC_PA_AUTONEG;
    }

    return (SOC_E_NONE);
}


STATIC int
_phy_hl65_control_scrambler_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc;
    int         rv = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        
        /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x833c, &uData);*/
        SOC_IF_ERROR_RETURN
            (READ_HL65_DIGITAL4_MISC3r(unit, pc, &data));
   
        if (value) { 
            data |= (DIGITAL4_MISC3_SCR_EN_PER_LANE_BITS <<  
                     DIGITAL4_MISC3_SCR_EN_PER_LANE_SHIFT);
        } else {
            data &= ~(DIGITAL4_MISC3_SCR_EN_PER_LANE_BITS <<  
                      DIGITAL4_MISC3_SCR_EN_PER_LANE_SHIFT);
        }

        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, 0x833c, uData);*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL4_MISC3r(unit, pc, data));

    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}

STATIC int
_phy_hl65_control_scrambler_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc;
    int         rv = SOC_E_NONE;

    pc = INT_PHY_SW_STATE(unit, port);
    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        
        SOC_IF_ERROR_RETURN
            (READ_HL65_DIGITAL4_MISC3r(unit, pc, &data));
   
	*value = ((data & DIGITAL4_MISC3_SCR_EN_PER_LANE_MASK) == 0x100);

    } else {
        rv = SOC_E_UNAVAIL;
    }
    return rv;
}

STATIC int
_phy_hl65_control_encoding_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc;
    int         rv = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);
    if (PHY_HC65_FABRIC_MODE(unit, port)) {
        switch (value) {
        case phyControlEncoding8b10b:

            /* 
             * 8b10b only, disable 64/66 
             * soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8015, &uData); 
             */
            SOC_IF_ERROR_RETURN
                (READ_HL65_XGXSBLK1_LANECTRL0r(unit, pc, &data));

            data &= ~(1 << (pc->lane_num + 12));	    
            /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8015, uData); */
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_XGXSBLK1_LANECTRL0r(unit, pc, data));

            /* 
             * Force the sync character to the lower byte of the parallel 
             * interface 
             * uRegAddr = 0x80b9 + (uLaneAddr * 0x10);
             * soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData);
             */
            SOC_IF_ERROR_RETURN
                (HL65_REG_READ(unit, pc, 0x0, 0x80b9 + (0x10 * pc->lane_num),
                               &data));
            data = data | 0x0020;

            /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, uData); */
            SOC_IF_ERROR_RETURN
                (HL65_REG_WRITE(unit, pc, 0x0, 0x80b9 + (0x10 * pc->lane_num),
                                data));
            break;

        case phyControlEncoding64b66b:

            /* 
             * Force the sync character to the upper byte of the parallel 
             * interface 
             * uRegAddr = 0x80b9 + (uLaneAddr * 0x10);
             * soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData);
             */
            SOC_IF_ERROR_RETURN
                (HL65_REG_READ(unit, pc, 0x0, 0x80b9 + (0x10 * pc->lane_num),
                               &data));

            data = data & ~0x0020;

            /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, uData); */
            SOC_IF_ERROR_RETURN
                (HL65_REG_WRITE(unit, pc, 0x0, 0x80b9 + (0x10 * pc->lane_num),
                                data));

            /* 
             * enable 64b66b 
             * socd c_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8015, &uData);
             */
            SOC_IF_ERROR_RETURN
                (READ_HL65_XGXSBLK1_LANECTRL0r(unit, pc, &data));

            data |= 1 << (pc->lane_num + 12);	    

            /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8015, uData); */
            SOC_IF_ERROR_RETURN
                (WRITE_HL65_XGXSBLK1_LANECTRL0r(unit, pc, data));

            break;

        default:
            rv = SOC_E_UNAVAIL;
        } 
    } else {
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

#define PHY_HL65_LANEPRBS_LANE_SHIFT   4

STATIC int
_phy_hl65_control_prbs_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data = 0;

    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
        
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    if (PHY_HC65_FABRIC_MODE(unit, pc->port)) {
      uint32      prbs_lane_mask = 0;
      prbs_lane_mask = 0x3 << (PHY_HL65_LANEPRBS_LANE_SHIFT * pc->lane_num);
      data &= ~prbs_lane_mask;
      data |= value << (PHY_HL65_LANEPRBS_LANE_SHIFT * pc->lane_num);
    } else {
        data &= ~(XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK |
                  XGXSBLK1_LANEPRBS_PRBS_ORDER1_MASK |
                  XGXSBLK1_LANEPRBS_PRBS_ORDER2_MASK |
                  XGXSBLK1_LANEPRBS_PRBS_ORDER3_MASK);

        data |= ((value << XGXSBLK1_LANEPRBS_PRBS_ORDER0_SHIFT) |
                 (value << XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT) |
                 (value << XGXSBLK1_LANEPRBS_PRBS_ORDER2_SHIFT) |
                 (value << XGXSBLK1_LANEPRBS_PRBS_ORDER3_SHIFT));
    }

    SOC_IF_ERROR_RETURN
      (WRITE_HL65_XGXSBLK1_LANEPRBSr(unit, pc, data));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    /*  soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8019, &uData);*/
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    if (PHY_HC65_FABRIC_MODE(unit, pc->port)) {
      /* Extract prbs polynomial setting from register */
      *value = ((data >> (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num)) &
	      XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK);
    } else {
        *value = ((data & 0x0003) | ((data >> 4) & 0x0003) |
                ((data >> 8) & 0x0003) | ((data >> 12) & 0x0003));
    }  

    return SOC_E_NONE;
}


STATIC int
_phy_hl65_control_prbs_tx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    if (PHY_HC65_FABRIC_MODE(unit, pc->port)) {
      uint32      lane_mask = 0;
      lane_mask = 0x4 << (PHY_HL65_LANEPRBS_LANE_SHIFT * pc->lane_num);
      data &= ~lane_mask;
      data |= value << (PHY_HL65_LANEPRBS_LANE_SHIFT * pc->lane_num);
    } else {
        data &= ~(XGXSBLK1_LANEPRBS_PRBS_INV0_MASK |
                  XGXSBLK1_LANEPRBS_PRBS_INV1_MASK |
                  XGXSBLK1_LANEPRBS_PRBS_INV2_MASK |
                  XGXSBLK1_LANEPRBS_PRBS_INV3_MASK);

        value &= (1 << XGXSBLK1_LANEPRBS_PRBS_INV0_BITS) - 1;
        data |= ((value << XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT) |
                 (value << XGXSBLK1_LANEPRBS_PRBS_INV1_SHIFT) |
                 (value << XGXSBLK1_LANEPRBS_PRBS_INV2_SHIFT) |
                 (value << XGXSBLK1_LANEPRBS_PRBS_INV3_SHIFT));
    }

    SOC_IF_ERROR_RETURN
      (WRITE_HL65_XGXSBLK1_LANEPRBSr(unit, pc, data));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    uint16       inv_shifter = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8019, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    inv_shifter = (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num) +
                    XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    data &= (1 << inv_shifter);
    *value = (data) ?  1 : 0;
    
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_fabric_prbs_tx_enable_set(int unit, soc_port_t port, 
                                            uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      lane_mask = 1 << (PHY_HL65_LANEPRBS_LANE_SHIFT * pc->lane_num);

    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8019, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    
    if (value) {
        data |= lane_mask << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    } else {
        data &= ~(lane_mask << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
    }
    /* write back updated configuration */
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8019, uData) */ 
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK1_LANEPRBSr(unit, pc, data));

    SOC_IF_ERROR_RETURN(
        phy_hl65_lb_set(unit, port, (value & 0x8000) ? 1 : 0));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_xgxs_prbs_tx_enable_set(int unit, soc_port_t port,
                                          uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint16      pll_mode;

    if (value) {
        /* Assume that the core has already been forced to the desired speed */
        /* Get the current PLL operating mode */
        SOC_IF_ERROR_RETURN(READ_HL65_TXPLL_ANAPLLSTATUSr(unit, pc, &data));
        pll_mode = data & TXPLL_ANAPLLSTATUS_PLL_MODE_AFE_MASK;

        /* Change to independent lane mode */
        SOC_IF_ERROR_RETURN(READ_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, &data));
        data &= ~(XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK |
                  XGXSBLK0_XGXSCONTROL_MODE_10G_MASK);
        data |= ((XGXSBLK0_XGXSCONTROL_MODE_10G_Indlanes <<
                    XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
                  XGXSBLK0_XGXSCONTROL_MDIO_CONT_EN_MASK);
        SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, data));

        /* Set appropriate data width mode */
        if (data & XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK) {
            /* Half rate mode */
            SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK1_LANECTRL1r(unit, pc,
                                                               0x5555));
        } else {
            /* Full rate mode */
            SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK1_LANECTRL1r(unit, pc,
                                                               0xffff));
        }

        /* Force the VCO based on the previously read mode */
        SOC_IF_ERROR_RETURN(READ_HL65_DIGITAL_MISC1r(unit, pc, &data));
        data &= ~(DIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK |
                  DIGITAL_MISC1_FORCE_LN_MODE_MASK |
                  DIGITAL_MISC1_FORCE_SPEED_MASK);
        data |= (DIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK |
                 (pll_mode << DIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT) |
                 DIGITAL_MISC1_FORCE_LN_MODE_MASK);
        SOC_IF_ERROR_RETURN(WRITE_HL65_DIGITAL_MISC1r(unit, pc, data));

        /* Disable CL36 PCS */
        SOC_IF_ERROR_RETURN(READ_HL65_XGXSBLK1_LANECTRL0r(unit, pc, &data));
        data &= ~(XGXSBLK1_LANECTRL0_CL36_PCS_EN_RX_MASK |
                  XGXSBLK1_LANECTRL0_CL36_PCS_EN_TX_MASK);
        SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK1_LANECTRL0r(unit, pc, data));

        /* Disable comma detect, error detect */
        SOC_IF_ERROR_RETURN(READ_HL65_XGXSBLK1_LANECTRL2r(unit, pc, &data));
        data &= ~(XGXSBLK1_LANECTRL2_CDET_EN1G_MASK |
                  XGXSBLK1_LANECTRL2_EDEN1G_MASK);
        SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK1_LANECTRL2r(unit, pc, data));

        /* Set loopback (if requested) */
        if (value & 0x8000) {
            SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK1_LANECTRL2r(unit, pc,
                            XGXSBLK1_LANECTRL2_GLOOP1G_MASK));
        }

        /* Enable PRBS transmit function */
        SOC_IF_ERROR_RETURN(READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));
        data |= (XGXSBLK1_LANEPRBS_PRBS_EN0_MASK |
                 XGXSBLK1_LANEPRBS_PRBS_EN1_MASK |
                 XGXSBLK1_LANEPRBS_PRBS_EN2_MASK |
                 XGXSBLK1_LANEPRBS_PRBS_EN3_MASK);
        SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK1_LANEPRBSr(unit, pc, data));

        /* Set receiver status to PRBS */
        SOC_IF_ERROR_RETURN(READ_HL65_RXB_ANARXCONTROLr(unit, pc, &data));
        data &= ~RXB_ANARXCONTROL_STATUS_SEL_MASK;
        data |= 0x7; /* PRBS status */
        SOC_IF_ERROR_RETURN(WRITE_HL65_RXB_ANARXCONTROLr(unit, pc, data));

        /* Start the PLL sequencer */
        SOC_IF_ERROR_RETURN(READ_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, &data));
        data |= XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK;
        SOC_IF_ERROR_RETURN(WRITE_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc, data));

        /* Delay a while */
        sal_usleep(250000);
    } else {
        /* Restore port */
        return phy_hl65_init(unit, port);
    }
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_prbs_tx_enable_set(int unit, soc_port_t port, uint32 value)
{
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (PHY_HC65_FABRIC_MODE(unit, pc->port)) {
        SOC_IF_ERROR_RETURN(
            _phy_hl65_control_fabric_prbs_tx_enable_set(unit, port, value));
    } else {
        SOC_IF_ERROR_RETURN(
            _phy_hl65_control_xgxs_prbs_tx_enable_set(unit, port, value));
    }

    return SOC_E_NONE;
}


STATIC int
_phy_hl65_control_prbs_tx_rx_enable_get(int unit, soc_port_t port, 
                                        uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    /*
     * Hypercore PRBS - note that in the Hypercore there is only 1 enable 
     * for both TX/RX 
     */
    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8019, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    data = data & ((1 << (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num))
                   << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
    
    *value = (data) ?  1 : 0;
  
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_prbs_tx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    return _phy_hl65_control_prbs_tx_rx_enable_get(unit, port, value);
}

STATIC int
_phy_hl65_control_prbs_rx_enable_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      lane_mask = 1 << (PHY_HL65_LANEPRBS_LANE_SHIFT * pc->lane_num);

    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, 0x8019, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    
    /* if (value) {
     *	    uData |= uLaneMask << BM9600_HYPERCORE_8019_PRBS_ENABLE_SHIFT;
     * } else {
     *    uData &= ~(uLaneMask << BM9600_HYPERCORE_8019_PRBS_ENABLE_SHIFT);
     * }
     */
    if (value) {
        data |= lane_mask <<  XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    }  else {
        data &= ~(lane_mask << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
    }
    
    /* write back updated configuration */
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8019, uData) */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK1_LANEPRBSr(unit, pc, data));
    
    /* Set up status collection */
    /* uPrbsRxControlReg = 0x80b1;
     * uPrbsRxControlReg += uLaneAddr * 0x10;
     * soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uPrbsRxControlReg, &uData)
     */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), &data));
    
    /* if (value) {
     *   uData &= ~BM9600_HYPERCORE_80B1_STATUS_MASK;
     *   uData |= BM9600_HYPERCORE_80B1_PRBS_STATUS_VALUE;
     * } else {
     *   uData &= ~BM9600_HYPERCORE_80B1_STATUS_MASK;
     *   uData &= ~BM9600_HYPERCORE_80B1_PRBS_STATUS_VALUE;
     * }
     */
    data &= ~RX0_ANARXCONTROL_STATUS_SEL_MASK;
    if (value) {
        data |= RX0_ANARXCONTROL_STATUS_SEL_prbsStatus;
    } else {
        data &= ~RX0_ANARXCONTROL_STATUS_SEL_prbsStatus;
    }
    
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uPrbsRxControlReg, uData) */
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), data));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_prbs_rx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    return _phy_hl65_control_prbs_tx_rx_enable_get(unit, port, value);
}

/*
 * Returns value 
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
STATIC int
_phy_hl65_control_fabric_prbs_rx_status_get(int unit, soc_port_t port, 
                                            uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    /* Get status */
    /* uPrbsRxControlReg = 0x80b0;
     * uPrbsRxControlReg += uLaneAddr * 0x10;
     * soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uPrbsRxControlReg, &uData);
     */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit, pc, 0x0, 0x80b0 + (0x10 * pc->lane_num), &data));


    if (data == (RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_BITS  <<
                 RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_SHIFT)) {
        /* PRBS is in sync */
        *value = 0;
    } else if (data == 0) {
        /* PRBS not in sync */
        *value = -1;
    } else { 
        /* Get errors */
        *value = data & RX0_ANARXSTATUS_PRBS_STATUS_PTBS_ERRORS_MASK;
    }

    return SOC_E_NONE;
}
/*
 * Returns value 
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
STATIC int
_phy_hl65_control_xgxs_prbs_rx_status_get(int unit, soc_port_t port, 
                                          uint32 *value)
{
    int         lane;
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    /* JOE */
    /* Get status for all 4 lanes and check for sync
     * 0x80b0, 0x80c0, 0x80d0, 0x80e0 
     */
    *value = 0;
    for (lane = 0; lane < 4; lane++) {
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit, pc, 0x0, 0x80b0 + (0x10 * lane), &data));
        if (data == (RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_BITS  <<
                     RX0_ANARXSTATUS_PRBS_STATUS_PRBS_LOCK_SHIFT)) {
            /* PRBS is in sync */
            continue;
        } else if (data == 0) {
            /* PRBS not in sync */
            *value = -1;
            break;
        } else {
            /* Get errors */
            *value += data & RX0_ANARXSTATUS_PRBS_STATUS_PTBS_ERRORS_MASK;
        }
    }
    return SOC_E_NONE;
}


STATIC int
_phy_hl65_control_prbs_rx_status_get(int unit, soc_port_t port, uint32 *value)
{
    int         rv;
    uint16      rx_sts, saved_rx_sts;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* Set up prbs rx status collection */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), &rx_sts));
    saved_rx_sts = rx_sts;
    rx_sts = (rx_sts & ~RX0_ANARXCONTROL_STATUS_SEL_MASK) |
             RX0_ANARXCONTROL_STATUS_SEL_prbsStatus;
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), rx_sts));

    if (PHY_HC65_FABRIC_MODE(unit, pc->port)) {
        rv =  _phy_hl65_control_fabric_prbs_rx_status_get(unit, port, value);
    } else {
        rv = _phy_hl65_control_xgxs_prbs_rx_status_get(unit, port, value);
    }

    /* Restore rx status */
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), saved_rx_sts));

    return rv;
}



STATIC int
_phy_hl65_control_serdes_driver_tune_set(int unit, soc_port_t port,
                                         int lane_num, 
                                         uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (SOC_IS_RELOADING(unit)) {
        return SOC_E_NONE;
    }

    /* Start receive tuning via rxSeqStart */
    /* uRegAddr = 0x826e + (uLaneAddr*0x10);
     * soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr , &uData)
     */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit, pc, 0x0, 0x826E + (0x10 * lane_num), &data));
    
    /* tune by toggling rxSeqStart */
    /* uData |= 0x8000; */
    data |=  (DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_BITS <<  
              DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_SHIFT);
    
    /* write back updated configuration */
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData) */
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * lane_num), data));
    
    /* tune by toggling rxSeqStart */
    data &= ~ (DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_BITS <<  
               DSC2B0_DSC_MISC_CTRL0_RXSEQSTART_SHIFT);
    
    /* write back updated configuration */
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData) */
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit, pc, 0x0, 0x826E + (0x10 * lane_num), data));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "hl_65: Equalization tune start\n")));
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_64b66b_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      lane_mask = 1 << pc->lane_num;

    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8015, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANECTRL0r(unit, pc, &data));
    
    if (value) {
        data |= lane_mask << XGXSBLK1_LANECTRL0_ED66EN_SHIFT;

        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8015, uData);*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL0r(unit, pc, data));
    } else {

        data &= ~(lane_mask << XGXSBLK1_LANECTRL0_ED66EN_SHIFT);

	/* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8015, uData);*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL0r(unit, pc, data));
    }
    return SOC_E_NONE;
}


STATIC int
_phy_hl65_control_8b10b_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      lane_mask = 1 << pc->lane_num;

    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, 0x8017, &uData) */
    SOC_IF_ERROR_RETURN
        (READ_HL65_XGXSBLK1_LANECTRL2r(unit, pc, &data));
    
    if (value) {
        /* uData |= uLaneMask << BM9600_HYPERCORE_8017_8B10B_SHIFT;
         * uData |= uLaneMask << BM9600_HYPERCORE_8017_KCHAR_SHIFT;
         */
        data |= lane_mask << XGXSBLK1_LANECTRL2_CDET_EN1G_SHIFT;
        data |= lane_mask << XGXSBLK1_LANECTRL2_EDEN1G_SHIFT;
        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8017, uData);*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL2r(unit, pc, data));
    } else {
        /* uData &= ~(uLaneMask << BM9600_HYPERCORE_8017_8B10B_SHIFT);
         * uData &= ~(uLaneMask << BM9600_HYPERCORE_8017_KCHAR_SHIFT);
         */
        data &= ~(lane_mask << XGXSBLK1_LANECTRL2_CDET_EN1G_SHIFT);
        data &= ~(lane_mask << XGXSBLK1_LANECTRL2_EDEN1G_SHIFT);
        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, 0x8017, uData);*/
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_XGXSBLK1_LANECTRL2r(unit, pc, data));
    }
    return SOC_E_NONE;
}



STATIC int
_phy_hl65_tx_driver_field_get(soc_phy_control_t type,int *ln_ctrl,uint16 *mask,int *shfter)
{
    int lane_ctrl;

    lane_ctrl = TX_DRIVER_DFT_LN_CTRL;
    *mask = 0;
    *shfter = 0;
    /* _LANEn(n=0-3) control type only applies to combo mode or dxgxs in
     * independent channel mode
     */
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
    /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
    /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
    /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
    /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        *shfter = TXB_TX_OS_DRIVER_PREEMPHASIS_POST_SHIFT;
        *mask = TXB_TX_OS_DRIVER_PREEMPHASIS_POST_MASK;
        if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE2) {
            lane_ctrl = 2;
        } else if (type == SOC_PHY_CONTROL_PREEMPHASIS_LANE3) {
            lane_ctrl = 3;
        }
        break;

    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
    /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        *shfter = TXB_TX_OS_DRIVER_IDRIVER_SHIFT;
        *mask = TXB_TX_OS_DRIVER_IDRIVER_MASK;
        if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2) {
            lane_ctrl = 2;
        } else if (type == SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3) {
            lane_ctrl = 3;
        }
        break;

    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
    /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        *shfter = TXB_TX_OS_DRIVER_IPREDRIVER_SHIFT;
        *mask = TXB_TX_OS_DRIVER_IPREDRIVER_MASK;
        if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0) {
            lane_ctrl = 0;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1) {
            lane_ctrl = 1;
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2) {
            lane_ctrl = 2;
          /* coverity[mixed_enums:FALSE] */
        } else if (type == SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3) {
            lane_ctrl = 3;
        }
        break;

    case SOC_PHY_CONTROL_PRE_PREEMPHASIS:
        *shfter = TXB_TX_OS_DRIVER_PREEMPHASIS_PRE_SHIFT;
        *mask = TXB_TX_OS_DRIVER_PREEMPHASIS_PRE_MASK;
        break;
    default:
     /* should never get here */
        return SOC_E_PARAM;
    }
    *ln_ctrl = lane_ctrl;
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    uint16  data;             /* Temporary holder of reg value to be written */
    uint16  mask;             /* Bit mask of reg value to be updated */
    uint16  ivalue;
    int     lane_ctrl;
    int     lane_num;
    int     shifter;

    /* Both cases 6.5G/3.125G handled the same, ignore MSB though it may be passed in*/
    ivalue = value & 0x7fff;
    if (ivalue > 15) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (_phy_hl65_tx_driver_field_get(type,&lane_ctrl,&mask,&shifter));

    data = ivalue << shifter;

    if ((lane_ctrl != TX_DRIVER_DFT_LN_CTRL) ||
        PHY_INDEPENDENT_LANE_MODE(unit, pc->port) ||
        PHY_HC65_FABRIC_MODE(unit, pc->port)) {
        if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
            lane_num = lane_ctrl;
        } else {
            lane_num = pc->lane_num;
        }
	
	if (lane_num == 0) {
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX0_TX_BR_DRIVERr(unit, pc, data, mask));
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX0_TX_OS_DRIVERr(unit, pc, data, mask));
	} else if (lane_num == 1) {
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX1_TX_BR_DRIVERr(unit, pc, data, mask));
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX1_TX_OS_DRIVERr(unit, pc, data, mask));
	} else if (lane_num == 2) {
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX2_TX_BR_DRIVERr(unit, pc, data, mask));
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX2_TX_OS_DRIVERr(unit, pc, data, mask));
	} else if (lane_num == 3) {
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX3_TX_BR_DRIVERr(unit, pc, data, mask));
	    SOC_IF_ERROR_RETURN
		(MODIFY_HL65_TX3_TX_OS_DRIVERr(unit, pc, data, mask));
	} 
    } else { /* default control in combo mode: bcst to all lanes */
	/* set for both BR mode and OS mode */
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX0_TX_BR_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX0_TX_OS_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX1_TX_BR_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX1_TX_OS_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX2_TX_BR_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX2_TX_OS_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX3_TX_BR_DRIVERr(unit, pc, data, mask));
	SOC_IF_ERROR_RETURN
	    (MODIFY_HL65_TX3_TX_OS_DRIVERr(unit, pc, data, mask));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */
    uint16  mask;
    int     shifter;
    int     lane_ctrl;
    int     lane_num;

    SOC_IF_ERROR_RETURN
        (_phy_hl65_tx_driver_field_get(type,&lane_ctrl,&mask,&shifter));

     
    /* Read preemphasis/driver/pre-driver current */
    if ((lane_ctrl != TX_DRIVER_DFT_LN_CTRL) ||
        PHY_INDEPENDENT_LANE_MODE(unit, pc->port) ||
        PHY_HC65_FABRIC_MODE(unit, pc->port)) {
        if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
            lane_num = lane_ctrl;
        } else {
            lane_num = pc->lane_num;
        }
        if (lane_num == 0) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_TX0_TX_OS_DRIVERr(unit, pc, &data16));
        } else if (lane_num == 1) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_TX1_TX_OS_DRIVERr(unit, pc, &data16));
        } else if (lane_num == 2) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_TX2_TX_OS_DRIVERr(unit, pc, &data16));
        } else if (lane_num == 3) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_TX3_TX_OS_DRIVERr(unit, pc, &data16));
        } else {
            return (SOC_E_PARAM);
        }
    } else {
        SOC_IF_ERROR_RETURN
            (READ_HL65_TX0_TX_OS_DRIVERr(unit, pc, &data16));
    }

    *value = (data16 & mask) >> shifter;

    return SOC_E_NONE;
}


STATIC int
_phy_hl65_tuning_done_get(int unit, soc_port_t port,int lane_num, uint16 *done)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* *bTuningDone = FALSE;*/
    *done = 0;

    /* Read tuning status */
    /* uRegAddr = 0x82b7 + (uLaneAddr*0x10);
     * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData);
     */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,0x82B7 + (0x10 * lane_num), &data));

#if 0
    /* If tuning is done */
    if (uData & 0x2000) {
	LOG_ERROR(BSL_LS_SOC_PHY,
		  (BSL_META_U(unit,
			      "hc(%02d:%02d) Tuning Done indication received\n"),
		   uPhyAddr, uLaneAddr));
	*bTuningDone = TRUE;
    }
#endif
    *done = (data & 
             (DSC3B0_SM_STATUS0_TUNING_DONE_BITS <<
              DSC3B0_SM_STATUS0_TUNING_DONE_SHIFT)) ? 1 : 0;

    return SOC_E_NONE;
}


/* #define BCM_BM9600_HYPERCORE_PORT_TUNING_AVG 40 */
#define PHY_HL65_PORT_TUNING_AVG    40

STATIC int
_phy_hl65_tuning_status_get(int unit, soc_port_t port, 
                            int lane_num,
                            uint16 *vga_sum_avg, 
                            uint16 *dfe_tap_bin_avg)
{
    uint16      data, cnt;
    uint16      cur_vga, avg_vga, cur_dfe, avg_dfe;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    avg_vga = 0;
    avg_dfe = 0;

    for (cnt=0; cnt < PHY_HL65_PORT_TUNING_AVG; cnt++) {

	/* uRegAddr = 0x82b5 + (uLaneAddr*0x10);
         * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData);
         */
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,0x82b5 + (0x10 * lane_num), &data));

	/* uCurrentVga = (uData & 0x07c0)>>6;
         * uCurrentDfe = (uData & 0x003f);
         */
        cur_vga = (data & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK) >>  
                  DSC3B0_DFE_VGA_STATUS0_VGA_SUM_SHIFT;
        cur_dfe = (data & DSC3B0_DFE_VGA_STATUS0_DFE_TAP_BIN_MASK);

	/*uAvgVga += uCurrentVga;
         *uAvgDfe += uCurrentDfe;
         */
        avg_vga += cur_vga;
        avg_dfe += cur_dfe;
    }

    /* Divide to get the average value */
    /* uAvgVga = uAvgVga/BCM_BM9600_HYPERCORE_PORT_TUNING_AVG;
     * uAvgDfe = uAvgDfe/BCM_BM9600_HYPERCORE_PORT_TUNING_AVG;
     */
    avg_vga = avg_vga / PHY_HL65_PORT_TUNING_AVG;
    avg_dfe = avg_dfe / PHY_HL65_PORT_TUNING_AVG;

    /*   *pVgaSumAvg = uAvgVga;
     *   *pDfeTapBinAvg = uAvgDfe;
     */
    *vga_sum_avg = avg_vga;


    *dfe_tap_bin_avg = avg_dfe;

#if 0
    LOG_ERROR(BSL_LS_SOC_PHY,
              (BSL_META_U(unit,
                          "hc(%02d:%02d) VGA avg(%d)\n"),
               uPhyAddr, uLaneAddr, uAvgVga));
    LOG_ERROR(BSL_LS_SOC_PHY,
              (BSL_META_U(unit,
                          "hc(%02d:%02d) DFE avg(%d)\n"),
               uPhyAddr, uLaneAddr, uAvgDfe));
#endif
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "hc(%02d) VGA avg(%d)\n"), port, avg_vga));
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "hc(%02d) DFE avg(%d)\n"), port, avg_dfe));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_fine_tune(int unit, soc_port_t port, int lane_num)
{
    uint16   data, cnt;
    uint16   tuning_done;
    uint32   reg_addr;
    int      avg_vga_sum;
    int      rv = SOC_E_NONE;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint16      serdes_id0;

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));
    
    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                               TEST_SERDESID0_REV_LETTER_MASK);
    
    if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_A1) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_B0)) {
        /* Disable DSC tuning state machine */
        /* uRegAddr = 0x8260 + (uLaneAddr*0x10);
         * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);
         */
        reg_addr = 0x8260 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
        
        /* tuning_sm_en: bit 0 */
        /* uData &= ~0x1; */
        data &= ~DSC2B0_SM_CTRL0_TUNING_SM_EN_BITS; 
        
	    /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
        /* Enable training mode in state machine */
        /* uRegAddr = 0x8262 + (uLaneAddr*0x10);
         * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);
         */
        reg_addr = 0x8262 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
        
        /* uData |= 1; */ /* train_mode_en */
        data |= DSC2B0_SM_CTRL2_TRAIN_MODE_EN_BITS;
        
        /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
    }
    
    /* Bypass BR peaking filter calibration */
    /* uRegAddr = 0x8260 + (uLaneAddr*0x10);
     *  soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);
     */
    reg_addr = 0x8260 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    /* bypass_br_pf_cal */
    /* uData |= 0x200; */
    data |=  (DSC2B0_SM_CTRL0_BYPASS_BR_PF_CAL_BITS <<  
              DSC2B0_SM_CTRL0_BYPASS_BR_PF_CAL_SHIFT);

    if (serdes_id0 == HL_SERDES_ID0_REVID_C0) {
        /* bypass _tx_postc_calibration */
        data |=  (DSC2B0_SM_CTRL0_BYPASS_TX_POSTC_CAL_BITS <<  
                  DSC2B0_SM_CTRL0_BYPASS_TX_POSTC_CAL_SHIFT);
    }
    /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_A1) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_B0)) {
        
        /* DSC Analog control 0 odd ctrl register setup */
        /* uRegAddr = 0x821a + (uLaneAddr*0x10);
         * uData = 0x8000; 
         */
        /* Write 0 to p1_odd_ctrl, d_odd_ctrl and m1_odd_ctrl and set force_odd_ctrl to 1 */
        reg_addr = 0x821a + (lane_num * 0x10);
        data = (DSC1B0_DSC_ANA_CTRL0_FORCE_ODD_CTRL_BITS <<
                DSC1B0_DSC_ANA_CTRL0_FORCE_ODD_CTRL_SHIFT);
        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
        
        /* DSC Analog control 0 even ctrl register setup */
        /* uRegAddr = 0x821b + (uLaneAddr*0x10);
         * uData = 0x8000; 
         */
        reg_addr =  0x821b + (lane_num * 0x10);
        /* Write 0 to p1_evn_ctrl, d_evn_ctrl and m1_evn_ctrl and set force_evn_ctrl to 1 */
        data = (DSC1B0_DSC_ANA_CTRL1_FORCE_EVN_CTRL_BITS <<
                DSC1B0_DSC_ANA_CTRL1_FORCE_EVN_CTRL_SHIFT);
        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
        
        /* read vga_sum 10 times and average */
        /* uRegAddr = 0x82b5 + (uLaneAddr*0x10);
         * uAvgVgaSum = 0;
         */
        reg_addr = 0x82b5 + (lane_num * 0x10);
        avg_vga_sum = 0;
        for (cnt=0; cnt < 10; cnt++) {
            /* rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData); */
            SOC_IF_ERROR_RETURN
                (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
            /*uAvgVgaSum += (uData & 0x07c0) >> 6;*/
            avg_vga_sum += ((data & DSC3B0_DFE_VGA_STATUS0_VGA_SUM_MASK) >> 
                            DSC3B0_DFE_VGA_STATUS0_VGA_SUM_SHIFT);
        }
        /* uAvgVgaSum = uAvgVgaSum/10; */
        avg_vga_sum = avg_vga_sum / 10;
        
        LOG_INFO(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "hc(%02d) VGA avg sum(%d)\n"), 
                  port, avg_vga_sum));
        
        if (avg_vga_sum > 31) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "ERROR: hc(%02d) VGA avg sum out of range(%d) maximum allowed 31\n"), 
                       port, avg_vga_sum));
            return SOC_E_FAIL;
        }
        
        /* write vga average into vga_write_val */
        /* uRegAddr = 0x8215 + (uLaneAddr*0x10);
         * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);
         */
        reg_addr = 0x8215 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
        
        /*uData = uData & ~0x03e;
         * uData |= uAvgVgaSum << 1;
         */
        data = data &  (~DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_VAL_MASK);
        data |= (avg_vga_sum << DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_VAL_SHIFT);
        
        /* rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
        
        /* Toggle vga_write_en to complete the VGA write */
        /* uData |= 1;*/
        data |= DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_BITS;
        
        /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
        /* uData &=~1; */
        data &= ~DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_BITS;
        
        /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        

        
        /* DSC Analog control 0 odd ctrl register setup */
        /* uRegAddr = 0x821a + (uLaneAddr*0x10);
         * uData = 0x0000; 
         */
        /* Write 0 to p1_odd_ctrl, d_odd_ctrl and m1_odd_ctrl and set force_odd_ctrl to 0 */
        reg_addr = 0x821a + (lane_num * 0x10);
        data = 0;
        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
        
        /* DSC Analog control 0 even ctrl register setup */
        /* uRegAddr = 0x821b + (uLaneAddr*0x10);
         * uData = 0x0000; 
         */
        reg_addr =  0x821b + (lane_num * 0x10);
        /* Write 0 to p1_evn_ctrl, d_evn_ctrl and m1_evn_ctrl and set force_evn_ctrl to 0 */
        data = 0;
        
        /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
        
    }
    /* Enable DSC tuning state machine */
    /* uRegAddr = 0x8260 + (uLaneAddr*0x10);*/
    reg_addr =  0x8260 + (lane_num * 0x10 );
    /*rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    
    /* uData |= 0x1; */
    /* tuning_sm_en: bit 0 */
    data |= DSC2B0_SM_CTRL0_TUNING_SM_EN_BITS;
    
    /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
    
    /* Restart tuning */
    /* uData |= 2;*/
    data |= (DSC2B0_SM_CTRL0_RESTART_TUNING_BITS << 
             DSC2B0_SM_CTRL0_RESTART_TUNING_SHIFT);
    
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
    
    
    /* Wait for tuning done signal (timeout) */
    for (cnt = 0; cnt < 50; cnt++) {
        /* _bcm_bm9600_hypercore_tuning_done_get(unit, uPhyAddr, uLaneAddr, &bTuningDone);*/
        SOC_IF_ERROR_RETURN
            (_phy_hl65_tuning_done_get(unit, port, lane_num,&tuning_done));
        
        if (tuning_done) {
            break;
        }
        
        /*thin_delay(10000000);*/
        /* 10 milliseconds */
        sal_usleep(10000);
    }
    
    if (!tuning_done) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: hc(%02d) tuning done timed out during BR fine tuning\n"), 
                   port));
        
        rv = SOC_E_TIMEOUT;
    }
    
    /* restore br_pf_calibration and disable training mode */
    /* uRegAddr = 0x8260 + (uLaneAddr*0x10);*/
    reg_addr =  0x8260 + (lane_num * 0x10);
    
    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    /*uData &= ~0x200; */
    /* bypass_br_pf_cal */
    data &= ~(DSC2B0_SM_CTRL0_BYPASS_BR_PF_CAL_BITS <<
               DSC2B0_SM_CTRL0_BYPASS_BR_PF_CAL_SHIFT);

    if (serdes_id0 == HL_SERDES_ID0_REVID_C0) {
        /* bypass _tx_postc_calibration */
        data &=  ~(DSC2B0_SM_CTRL0_BYPASS_TX_POSTC_CAL_BITS <<  
                   DSC2B0_SM_CTRL0_BYPASS_TX_POSTC_CAL_SHIFT);
    }
    
    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
    
    /* disable training mode */
    /* uRegAddr = 0x8262 + (uLaneAddr*0x10); */
    reg_addr =  0x8262 + (lane_num * 0x10);
    
    /* soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    
    /* uData &= ~1; */
    /* train_mode_en */
    data &= ~DSC2B0_SM_CTRL2_TRAIN_MODE_EN_BITS;

    /* soc_bm9600_mdio_hc_write(unit, uPhyAddr, uLaneAddr, uRegAddr, uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return rv;
}


STATIC int
_phy_hl65_control_eq_tune_status_get(int unit, 
                                     soc_port_t port, 
                                     int lane_num,
                                     uint32 *value)
{
    uint16      data;
    uint16      sig_detected;
    uint16      saved_status_select;
    uint16      tuning_done;
    uint16      avg_vga, avg_dfe;
    uint16      postc_metric, negative;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    int cnt;
    uint16      serdes_id0;

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                            TEST_SERDESID0_REV_LETTER_MASK);


    *value = FALSE;
    
    /*  Make sure we have signal detect */
    /* status = _bcm_bm9600_hypercore_sigdet_status_get(unit, uPhyAddr, uLaneAddr, &bSigDetected)
     * --->  code for this function brought here---
     * uRegAddr = 0x80b1;
     * uRegAddr += uLaneAddr * 0x10;
     *
     * Save off current value of 0x80b1 status_sel 
     *
     * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData);
     */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * lane_num), &data));

    /* uSavedStatusSelect = uData; */
    saved_status_select = data;

    /* Set register 0x80b1 status_sel to get sigDetStatus=0 */
    /* uData &= ~BM9600_HYPERCORE_80B1_STATUS_MASK;
     * uData |= BM9600_HYPERCORE_80B1_SIGDET_STATUS_VALUE;
     */
    data &= ~RX0_ANARXCONTROL_STATUS_SEL_MASK;
    data |= RX0_ANARXCONTROL_STATUS_SEL_sigdetStatus;

    /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, uData);*/
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * lane_num), data));

    /* read to clear latched status */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,0x80B0 + (0x10 * lane_num), &data));

    /* read register 0x80b0 to get status and fill in bSigDetected */
    /* uRegAddr = 0x80b0;
     * uRegAddr += uLaneAddr * 0x10;
     * rv = soc_bm9600_mdio_hc_read(unit, uPhyAddr, 0, uRegAddr, &uData);
     */
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,0x80B0 + (0x10 * lane_num), &data));

    /* if (uData & 0x8000) {
     *	*bSigDetected = TRUE;
     * }else {
     *	*bSigDetected = FALSE;
     * }
     */
    sig_detected = (data & 
                    (RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_BITS <<  
                     RX0_ANARXSTATUS_SIGDET_STATUS_CX4_SIGDET_SHIFT)) ? 1 : 0;

    /* write back old value of 0x80b1 status_sel */
    /*rv = soc_bm9600_mdio_hc_write(unit, uPhyAddr, 0, uRegAddr, uSavedStatusSelect); */
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * lane_num), saved_status_select));

#if 0
    if (!bSigDetected) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: Receiver indicates that no signal detected, check far end enable.\n")));
    }
#endif
    if (!sig_detected) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Receiver indicates that no signal detected. "
                              "Checking far end enable.\n")));
    }

    /* Wait for tuning done signal (timeout) */
    for (cnt = 0; cnt < 50; cnt++) {
        /* _bcm_bm9600_hypercore_tuning_done_get(unit, uPhyAddr, uLaneAddr, &bTuningDone);*/
        SOC_IF_ERROR_RETURN
            (_phy_hl65_tuning_done_get(unit, port, lane_num,&tuning_done));

        if (tuning_done) {
            break;
        }

        /*thin_delay(10000000);*/         /* 10 milliseconds */
        sal_usleep(10000);
    }

#if 0
    if (!bTuningDone) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: Tuning error port(%d), no tuning done indication from hardware\n"),
                   port));
    }
#endif
    if (!tuning_done) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "No tune done indication received on port %d\n"),
                   port));

    } else {
        /* _bcm_bm9600_hypercore_tuning_status_get(unit, uPhyAddr, uLaneAddr, &uAvgVga, &uAvgDfe);*/
        SOC_IF_ERROR_RETURN
            (_phy_hl65_tuning_status_get(unit, port, lane_num,
                                        &avg_vga, &avg_dfe));

	if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
	    (serdes_id0 == HL_SERDES_ID0_REVID_A1) ||
	    (serdes_id0 == HL_SERDES_ID0_REVID_B0)) {

	    /* If (uAvgDfe < 8) {
	     *   postc_metric = 0x400;     -1023 11 bit 2s complement number 
	     * } else if (uAvgDfe > 56) {
	     *   postc_metric = 0x3ff;      1023 11 bit 2s complement number 
	     * } else {
	     */
	    if (avg_dfe < 8) {
		postc_metric = 0x400; /* -1023 11 bit 2s complement number */
	    } else if (avg_dfe > 56) {
		postc_metric = 0x3ff; /* 1023 11 bit 2s complement number */
	    } else {
		/* uRegAddr = 0x82b8 + (uLaneAddr*0x10);
		 * status = soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);
		 */
		SOC_IF_ERROR_RETURN
		    (HL65_REG_READ(unit,pc,0x0,0x82b8 + (0x10 * lane_num), &data));
		
		/* postc_metric = uData & 0x7ff;*/
		postc_metric = data &  DSC3B0_SM_STATUS1_POSTC_METRIC_MASK;
	    }
	} else { /* C0 */
	    /* uRegAddr = 0x82b8 + (uLaneAddr*0x10);
	     * status = soc_bm9600_mdio_hc_read(unit, uPhyAddr, uLaneAddr, uRegAddr, &uData);
	     */
	    SOC_IF_ERROR_RETURN
		(HL65_REG_READ(unit,pc,0x0,0x82b8 + (0x10 * lane_num), &data));
	    
	    /* postc_metric = uData & 0x7ff;*/
	    postc_metric = data &  DSC3B0_SM_STATUS1_POSTC_METRIC_MASK;
	}

	/* negative = postc_metric >> 10; */
	/* Extract the sign bit */
	negative = postc_metric >> (DSC3B0_SM_STATUS1_POSTC_METRIC_BITS - 1);
	
	/* Get rid of the sign on the metric now */
	if (postc_metric > 1023) postc_metric -= 1024;
	   	
	/* if (!negative && (uAvgDfe < 30) && (postc_metric < 15)) { */
	if ((!negative) && (avg_dfe < 30)) { 
	    LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "port(%d) postc_metric(0x%x) is positive\n"),
                      port, postc_metric));
	    
	    *value = FALSE;
	    
        } else {
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "port(%d) remote equalization calibration complete\n"), 
                      port));

	    *value = TRUE;
        }
        
        /* Perform fine tuning operation */
        /*status = _bcm_bm9600_hypercore_fine_tune(unit, uPhyAddr, uLaneAddr);*/

        if (_phy_hl65_fine_tune(unit, port,lane_num) != SOC_E_NONE) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "port(%d) hl65_fine_tune fail\n"), port));
        }
    }
    return SOC_E_NONE;
}


STATIC int
_phy_hl65_control_linkdown_transmit_set(int unit, soc_port_t port, uint32 value)
{
    uint16      ctrl2_data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (value) {
        ctrl2_data = (DIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS << 
                      DIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
                     (DIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS  <<
                      DIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
                     (DIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                      DIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL_CONTROL1000X2r(unit, pc, ctrl2_data));

    } else {
        ctrl2_data = (DIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                      DIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |   
                     (DIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                      DIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) | 
                     (DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_BITS <<
                      DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_DIGITAL_CONTROL1000X2r(unit, pc, ctrl2_data));
    }

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_control_linkdown_transmit_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL_CONTROL1000X2r(unit, pc, &data));

    /* Check if FORCE_XMIT_DATA_ON_TXSIDE is set */
    *value = (data & (DIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                      DIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT))
             ? 1 : 0;

    return SOC_E_NONE;
}


    
/*
 * Get current DSC config
 */
STATIC int
_phy_hl65_dsc_cfg_get(int unit, soc_port_t port, int lane_num, 
                         phy_hl65_dsc_cfg_t *dsc_cfg) 
                                       
{
    int         rv = SOC_E_NONE;
    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    reg_addr = 0x82b2 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    if (data & DSC3B0_CDR_STATUS2_BR_CDR_ENABLED_MASK) {
        *dsc_cfg = PHY_HL65_DSC_CFG_BR_CDR;
    } else {
        data = (data & DSC3B0_CDR_STATUS2_OSCDR_MODE_MASK) >>
               DSC3B0_CDR_STATUS2_OSCDR_MODE_SHIFT;
        switch (data) {
        case 0:
            *dsc_cfg = PHY_HL65_DSC_CFG_1X_OSR;
            break;
        case 1:
            *dsc_cfg = PHY_HL65_DSC_CFG_2X_OSR;
            break;
        case 2:
            *dsc_cfg = PHY_HL65_DSC_CFG_4X_OSR;
            break;
        case 3:
            *dsc_cfg = PHY_HL65_DSC_CFG_5X_OSR;
            break;
        default:
            *dsc_cfg = PHY_HL65_DSC_CFG_UNKNOWN;
            rv = SOC_E_INTERNAL;
            break;
        }
    }

    return rv;
}

/*
 * Return value range
 */
STATIC int
_phy_hl65_control_serdes_tune_margin_max_get(int unit, soc_port_t port, 
                                             int lane_num, uint32 *max)
                                       
{
    phy_hl65_dsc_cfg_t dsc_cfg;

    /* Get DSC config */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_dsc_cfg_get(unit, port, lane_num, &dsc_cfg));

    switch  (dsc_cfg) {
    case PHY_HL65_DSC_CFG_BR_CDR:
        /* vertical diag */
        *max = (1 << DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_BITS);
        break;
    case PHY_HL65_DSC_CFG_1X_OSR:
    case PHY_HL65_DSC_CFG_2X_OSR:
        /* horizontal diag 1xOSR 
         * pi_phase_delta for 1xOSR == 1 so return full range
         */
        *max = (1 << DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_BITS);
        break;
    default:
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Tune margin supported for 1XOSR, 2XOSR, "
                              "BR CDR DSC")));
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;

}

STATIC int
_phy_hl65_serdes_tune_margin_common_start(int unit, soc_port_t port, 
                                          int lane_num)
{
    uint16      data;
    uint16      vga_sum_avg;
    uint16      dfe_tap_bin_avg;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* Step 1: check tuning done */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 1: check tuning done\n")));
    reg_addr = 0x82b7 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    if (!(data & DSC3B0_SM_STATUS0_TUNING_DONE_MASK)) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: hc(%02d) tuning has not finished\n"), port));
        return SOC_E_FAIL;
    }

    /* Step 2: average vga_sum and dfe_tap_bin */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 2: compute vga_sum, dfe_tap_bin average\n")));
    SOC_IF_ERROR_RETURN
        (_phy_hl65_tuning_status_get(unit, port, pc->lane_num,
                                     &vga_sum_avg, &dfe_tap_bin_avg));

    /* Step 3: fix vga_sum and dfe_tab_bin  */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 3: fix vga_sum, dfe_tap_bin\n")));
    reg_addr = 0x8215 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    data &= ~(DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_VAL_MASK |
              DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_MASK |
              DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_VAL_MASK | 
              DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_MASK);
    data |= (vga_sum_avg << DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_VAL_SHIFT) |
            (dfe_tap_bin_avg << DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_VAL_SHIFT);

    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    /* Now toggle the write enable bits */
    data &= ~((DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_BITS << 
               DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_SHIFT) |
              (DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_BITS <<
               DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_SHIFT));

    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    data |= (DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_BITS << 
             DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_SHIFT) |
            (DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_BITS <<
             DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_SHIFT);
	
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));
    /*
     * Leave vga_write and dfe_write enabled - will be disabled
     * when diag stops
     */
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_serdes_tune_margin_horizontal_start(int unit, soc_port_t port, 
                                              int lane_num, 
                                              phy_hl65_dsc_cfg_t dsc_cfg)
{
    uint16      data, delta, exp_clk90_po;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (_phy_hl65_serdes_tune_margin_common_start(unit, port, lane_num));

    /* 
     * Step 4: verify clk90_phase_offset
     *     ==32  1xOSR 
     *     ==0   2xOSR 
     */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 4: verify clk90_phase_offset\n")));
    reg_addr = 0x82b3 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    data = (data & DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK) >>
           DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT;

    exp_clk90_po = (dsc_cfg == PHY_HL65_DSC_CFG_1X_OSR) ? 32 : 0;
    if (data != exp_clk90_po) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WARNING: hc(%02d) diag clk90 phase offset=%d " 
                             "expected %d\nMake sure DSC is properly configured "
                             "and tuning is done before running diag\n"),
                  port, data, exp_clk90_po));
    }

    /* Step 5: clk90 offset override */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 5: enable clk90 offset override\n")));
    reg_addr = 0x8213 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    data |= (DSC1B0_PI_CTRL0_PI_CLK90_OFFSET_OVERRIDE_BITS  << 
             DSC1B0_PI_CTRL0_PI_CLK90_OFFSET_OVERRIDE_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    /* Step 6:  pi_phase_delta */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 6: set pi_phase_delta\n")));
    if (dsc_cfg == PHY_HL65_DSC_CFG_1X_OSR) {
        /* 1xOSR */
        delta = 1;
    } else if (dsc_cfg == PHY_HL65_DSC_CFG_2X_OSR) {
        delta = 2;
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: hc(%02d) invalid DSC config\n"), 
                              port));
        return SOC_E_PARAM;
    }
        
    data = (data & ~(DSC1B0_PI_CTRL0_PI_PHASE_DELTA_MASK)) |
           (delta << DSC1B0_PI_CTRL0_PI_PHASE_DELTA_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_serdes_tune_margin_horizontal_adjust(int unit, soc_port_t port, 
                                               int lane_num, uint32 inc)
{
    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* 
     * Step 7: set phase offset moving direction 
     */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 7: set phase offset moving direction\n")));
    reg_addr = 0x8213 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    data &= ~(DSC1B0_PI_CTRL0_PI_PHASE_INC_MASK |
              DSC1B0_PI_CTRL0_PI_PHASE_DEC_MASK);

    if (inc == 1) {
        data |= (DSC1B0_PI_CTRL0_PI_PHASE_INC_BITS <<
                 DSC1B0_PI_CTRL0_PI_PHASE_INC_SHIFT);
    } else if (inc == 0) {
        data |= (DSC1B0_PI_CTRL0_PI_PHASE_DEC_BITS <<
                 DSC1B0_PI_CTRL0_PI_PHASE_DEC_SHIFT);
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: hc(%02d) diag invalid adjust\n"),
                   port));
        return SOC_E_PARAM;
    }
             
    /* Step 8: toggle pi phase strobe */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 8: toggle pi phase strobe\n")));
    data &= ~(DSC1B0_PI_CTRL0_PI_PHASE_STROBE_BITS <<
              DSC1B0_PI_CTRL0_PI_PHASE_STROBE_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    data |= (DSC1B0_PI_CTRL0_PI_PHASE_STROBE_BITS <<
             DSC1B0_PI_CTRL0_PI_PHASE_STROBE_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    data &= ~(DSC1B0_PI_CTRL0_PI_PHASE_STROBE_BITS <<
              DSC1B0_PI_CTRL0_PI_PHASE_STROBE_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return SOC_E_NONE;
}


STATIC int
_phy_hl65_serdes_tune_margin_horizontal_stop(int unit, soc_port_t port,
                                             int lane_num, 
                                             phy_hl65_dsc_cfg_t dsc_cfg) 
                                       
{
    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* 
     * Step 11: restore pi_clk90_offset_override, 
     *          pi_phase_data, pi_phase_inc/dec
     */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 11: restore values\n")));
    reg_addr = 0x8213 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    data &= ~(DSC1B0_PI_CTRL0_PI_CLK90_OFFSET_OVERRIDE_MASK |
              DSC1B0_PI_CTRL0_PI_PHASE_DELTA_MASK |
              DSC1B0_PI_CTRL0_PI_PHASE_INC_MASK |
              DSC1B0_PI_CTRL0_PI_PHASE_DEC_MASK);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    /* Step 12,13: release vga_write and dfe_write */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 12: release vga_write, dfe_write enable\n")));
    reg_addr = 0x8215 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    data &= ~(DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_VAL_MASK |
              DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_VAL_MASK);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    data &= ~((DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_BITS <<
              DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_SHIFT) |
              (DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_BITS <<
               DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_SHIFT));
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_serdes_tune_margin_horizontal_value_get(int unit, soc_port_t port, 
                                                    int lane_num, 
                                                    uint32 *value)
{
    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    reg_addr = 0x82b3 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    *value = (data & DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK) >>
             DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT;

    /* 
     * External values are scaled 
     *
     * Internal range 0..127
     *
     *    Internal Value        Scaled Value
     *   Signed / Unsigned       Unsigned
     *      0        0               64
     *      1        1               65
     *      2        2               66
     *     ..       ..               ..
     *     63       63               127
     *    -64       64               0 
     *    -63       65               1
     *     -1      127               63
 */
    *value = (*value + 64) & 
             (DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_MASK >>
              DSC3B0_PI_STATUS0_CLK90_PHASE_OFFSET_SHIFT);
    return SOC_E_NONE;
}

/* 
 * Horizontal adjust 
 */
STATIC int
_phy_hl65_serdes_tune_margin_horizontal_value_set(int unit, soc_port_t port, 
                                                  int lane_num, 
                                                  phy_hl65_dsc_cfg_t dsc_cfg,
                                                  uint32 value)
{
    uint32              max, cur_value, 
                        inc = 0;
    int                 i, steps;

    SOC_IF_ERROR_RETURN
        (_phy_hl65_control_serdes_tune_margin_max_get(unit, port, lane_num, 
                                                      &max));
    
    if (value > max) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Invalid value %d (0..%d)\n"), 
                              value, max-1));
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (_phy_hl65_serdes_tune_margin_horizontal_value_get(unit, port, 
                                                           lane_num, 
                                                           &cur_value));
    if (value < cur_value) {
        steps = cur_value - value;
        inc = 0;
    } else if (value > cur_value) {
        steps = value - cur_value;
        inc = 1;
    } else {
        return SOC_E_NONE;
    }
    if (dsc_cfg == PHY_HL65_DSC_CFG_2X_OSR) {
        /* 2xOSR steps by 2 */
        steps /= 2;
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "horizontal adjusting to %d from %d steps %d\n"),
                         value, cur_value, steps));

    /* Adjust to value */
    for (i = 0; i < steps; i++) {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_horizontal_adjust(unit, port, 
                                                            lane_num, inc));
    }

    /* Verify if got there */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_serdes_tune_margin_horizontal_value_get(unit, port, 
                                                           lane_num, 
                                                           &cur_value));

    /* Adjust if 2xOSR to increment in steps by 2 */
    if ((dsc_cfg == PHY_HL65_DSC_CFG_2X_OSR) && (value % 2)) {
        value--;
    }

    if (cur_value != value) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "WARNING: Adjusted value %d does not match requested "
                             "value  %d\n"), cur_value, value));
    }
    return SOC_E_NONE;
}
    

STATIC int
_phy_hl65_serdes_tune_margin_vertical_start(int unit, soc_port_t port, 
                                            int lane_num)
{
    uint16      data;
    uint16      slicer_offset_me, slicer_offset_ze, slicer_offset_pe;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);


    SOC_IF_ERROR_RETURN
        (_phy_hl65_serdes_tune_margin_common_start(unit, port, lane_num));

    /* Step 4: read slicer offset status */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 4: read slice offset status\n")));
    reg_addr = 0x82ba + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    slicer_offset_me = (data & DSC3B0_SM_STATUS3_SLICER_OFFSET_ME_MASK) >>
                       DSC3B0_SM_STATUS3_SLICER_OFFSET_ME_SHIFT;

    slicer_offset_ze = (data & DSC3B0_SM_STATUS3_SLICER_OFFSET_ZE_MASK) >>
                       DSC3B0_SM_STATUS3_SLICER_OFFSET_ZE_SHIFT;

    slicer_offset_pe = (data & DSC3B0_SM_STATUS3_SLICER_OFFSET_PE_MASK) >>
                       DSC3B0_SM_STATUS3_SLICER_OFFSET_PE_SHIFT;

    /* Step 5: write slicer offsets to control */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 5: write slice offset status\n")));
    reg_addr = 0x821b + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    data &= ~( DSC1B0_DSC_ANA_CTRL1_P1_EVN_CTRL_MASK |
               DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK |
               DSC1B0_DSC_ANA_CTRL1_M1_EVN_CTRL_MASK);
    data |= (slicer_offset_me << DSC1B0_DSC_ANA_CTRL1_M1_EVN_CTRL_SHIFT) |
            (slicer_offset_ze << DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT) |
            (slicer_offset_pe << DSC1B0_DSC_ANA_CTRL1_P1_EVN_CTRL_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return SOC_E_NONE;
}

STATIC int
_phy_hl65_serdes_tune_margin_vertical_adjust(int unit, soc_port_t port, 
                                             int lane_num, uint32 inc)
{
    uint16      data;
    int16       tmp;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* 
     * Step 6: perturb data even slicer
     */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 6: perturb data even slicer\n")));
    reg_addr = 0x821b + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    
    tmp = (data & DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK)
           >> DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT;
    
    if (inc == 1) {
        /* increment */
        if (tmp == 0xf) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "ERROR: hc(%02d) cannot increment anymore\n"),
                       port));
            return SOC_E_PARAM;
        }
        tmp++;
    } else if (inc == 0) {
        /* decrement */
        if (tmp == 0x10) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "ERROR: hc(%02d) cannot decrement anymore\n"),
                       port));
            return SOC_E_PARAM;
        }
        tmp--;
    } else {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "ERROR: hc(%02d) diag invalid operation\n"), 
                              port));
        return SOC_E_PARAM;
    }
    data &= ~(DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK);
    data |= (tmp << DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT) |
            (DSC1B0_DSC_ANA_CTRL1_FORCE_EVN_CTRL_BITS <<
             DSC1B0_DSC_ANA_CTRL1_FORCE_EVN_CTRL_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    /* Toggle force even - required? */
    data &= ~(DSC1B0_DSC_ANA_CTRL1_FORCE_EVN_CTRL_BITS <<
              DSC1B0_DSC_ANA_CTRL1_FORCE_EVN_CTRL_SHIFT);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return SOC_E_NONE;
}


STATIC int
_phy_hl65_serdes_tune_margin_vertical_value_get(int unit, soc_port_t port, 
                                                int lane_num, uint32 *value)
{
    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    reg_addr = 0x821b + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    
    *value = (data & DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK)
               >> DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT;

    /* Scale value */
    *value = (*value + 16) &
             (DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK >>
              DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_SHIFT);
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_serdes_tune_margin_vertical_value_set(int unit, soc_port_t port, 
                                                int lane_num, uint32 value)
{
    int         i, steps;
    uint32      cur_value, max, inc = 0;

    
    SOC_IF_ERROR_RETURN
        (_phy_hl65_control_serdes_tune_margin_max_get(unit, port, lane_num, 
                                                      &max));
    
    if (value > max) {
        LOG_ERROR(BSL_LS_SOC_PHY,
                  (BSL_META_U(unit,
                              "Invalid value %d (0..%d)\n"), value, max));
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (_phy_hl65_serdes_tune_margin_vertical_value_get(unit, port, 
                                                         lane_num, 
                                                         &cur_value));
    if (value < cur_value) {
        steps = cur_value - value;
        inc = 0;
    } else if (value > cur_value) {
        steps = value - cur_value;
        inc = 1;
    } else {
        return SOC_E_NONE;
    }
    
    /* Adjust to value */
    for (i = 0; i < steps; i++) {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_vertical_adjust(unit, port, 
                                                          lane_num, 
                                                          inc));
    }

    /* Verify if got there */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_serdes_tune_margin_vertical_value_get(unit, port, 
                                                         lane_num, 
                                                         &cur_value));
    if (cur_value != value) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Adjusted value %d does not match "
                             "requested value %d\n"), cur_value, value));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_hl65_serdes_tune_margin_vertical_stop(int unit, soc_port_t port, 
                                           int lane_num)
{
    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* 
     * Step 9: restore all slice offset control registers to 0
     */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 9: restore slice offset\n")));
    reg_addr = 0x821b + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    data &= ~( DSC1B0_DSC_ANA_CTRL1_P1_EVN_CTRL_MASK |
               DSC1B0_DSC_ANA_CTRL1_D_EVN_CTRL_MASK |
               DSC1B0_DSC_ANA_CTRL1_M1_EVN_CTRL_MASK);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    /*
     * Step 10: release and restore vga_write and dfe_write 
     */
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "Step 10: release vga_write, dfe_write enable\n")));
    reg_addr = 0x8215 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));
    data &= ~(DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_VAL_MASK |
              DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_VAL_MASK);
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    data &= ~((DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_BITS <<
              DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_SHIFT) |
              (DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_BITS <<
               DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_SHIFT));
    SOC_IF_ERROR_RETURN
        (HL65_REG_WRITE(unit,pc,0x0,reg_addr, data));

    return SOC_E_NONE;
}


STATIC int
_phy_hl65_serdes_tune_margin_horizontal_mode_set(int unit, 
                                                 soc_port_t port, int lane_num, 
                                                 uint32 start,
                                                 phy_hl65_dsc_cfg_t dsc_cfg) 
{
    if (start) {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_horizontal_start(unit, port, 
                                                           lane_num,
                                                           dsc_cfg));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_horizontal_stop(unit, port, 
                                                          lane_num,
                                                          dsc_cfg));
    }
    return SOC_E_NONE;
}


STATIC int
_phy_hl65_serdes_tune_margin_vertical_mode_set(int unit, 
                                               soc_port_t port, 
                                               int lane_num,
                                               uint32 start) 
{
    if (start) {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_vertical_start(unit, port, 
                                                         lane_num));
    } else {
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_vertical_stop(unit, port, 
                                                        lane_num));
    }
    return SOC_E_NONE;

}

/*
 * Start/stop HC diag 
 */
STATIC int
_phy_hl65_control_serdes_tune_margin_mode_set(int unit, soc_port_t port, 
                                              int lane_num, uint32 start) 
                                       
{
    phy_hl65_dsc_cfg_t dsc_cfg;

    /* Get DSC config */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_dsc_cfg_get(unit, port, lane_num, &dsc_cfg));

    switch  (dsc_cfg) {
    case PHY_HL65_DSC_CFG_BR_CDR:
        /* vertical diag */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_vertical_mode_set(unit, port, 
                                                            lane_num, start));
        break;
    case PHY_HL65_DSC_CFG_1X_OSR:
    case PHY_HL65_DSC_CFG_2X_OSR:
        /* horizontal diag */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_horizontal_mode_set(unit, port, 
                                                              lane_num, start,
                                                              dsc_cfg));
        break;
    default:
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Tune margin supported for 1XOSR, 2XOSR, BR CDR DSC")));
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}

/*
 * Check whether diag is running or not 
 */
STATIC int
_phy_hl65_control_serdes_tune_margin_mode_get(int unit, soc_port_t port, 
                                              int lane_num, uint32 *value) 
                                       
{

    uint16      data;
    uint32      reg_addr;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* Read write enable */    
    reg_addr = 0x8215 + (lane_num * 0x10);
    SOC_IF_ERROR_RETURN
        (HL65_REG_READ(unit,pc,0x0,reg_addr, &data));

    /* 
     * Assumption here is that DFE and VGA write is only enabled
     * while running diag
     */
    *value =  ((data & DSC1B0_DFE_VGA_CTRL1_DFE_WRITE_EN_MASK) &&
               (data & DSC1B0_DFE_VGA_CTRL1_VGA_WRITE_EN_MASK)) ? 1 : 0;

    return SOC_E_NONE;
}



/*
 * Return value depending on DSC configuration
 */
STATIC int
_phy_hl65_control_serdes_tune_margin_value_get(int unit, soc_port_t port, 
                                               int lane_num, uint32 *value)
                                       
{
    phy_hl65_dsc_cfg_t dsc_cfg;

    /* Get DSC config */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_dsc_cfg_get(unit, port, lane_num, &dsc_cfg));

    switch  (dsc_cfg) {
    case PHY_HL65_DSC_CFG_BR_CDR:
        /* vertical diag */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_vertical_value_get(unit, port, 
                                                             lane_num,
                                                             value));
        break;
    case PHY_HL65_DSC_CFG_1X_OSR:
    case PHY_HL65_DSC_CFG_2X_OSR:
        /* horizontal diag 1xOSR */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_horizontal_value_get(unit, port, 
                                                               lane_num,
                                                               value));
        break;
    default:
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Tune margin supported for 1XOSR, 2XOSR, "
                             "BR CDR DSC\n")));
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}


/*
 * Adjust to requested value
 */
STATIC int
_phy_hl65_control_serdes_tune_margin_value_set(int unit, soc_port_t port, 
                                               int lane_num, uint32 value)
                                       
{
    phy_hl65_dsc_cfg_t dsc_cfg;

    /* Get DSC config */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_dsc_cfg_get(unit, port, lane_num, &dsc_cfg));

    switch  (dsc_cfg) {
    case PHY_HL65_DSC_CFG_BR_CDR:
        /* vertical diag */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_vertical_value_set(unit, port, 
                                                             lane_num, value));
        break;
    case PHY_HL65_DSC_CFG_1X_OSR:
    case PHY_HL65_DSC_CFG_2X_OSR:
        SOC_IF_ERROR_RETURN
            (_phy_hl65_serdes_tune_margin_horizontal_value_set(unit, port, 
                                                               lane_num, 
                                                               dsc_cfg,
                                                               value));
        break;
    default:
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "Tune margin supported for 1XOSR, 2XOSR, BR CDR DSC")));
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_control_set
 * Purpose:
 *      Configure PHY device specific control fucntion. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      type  - Control to update 
 *      value - New setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_hl65_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
    int i;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
   
    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_PREEMPHASIS:
        rv = _phy_hl65_control_tx_driver_set(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_ENCODING:
        rv = _phy_hl65_control_encoding_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_SCRAMBLER:
        rv = _phy_hl65_control_scrambler_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_hl65_control_prbs_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_hl65_control_prbs_tx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = _phy_hl65_control_prbs_tx_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_hl65_control_prbs_rx_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_SERDES_DRIVER_TUNE:
        if (PHY_INDEPENDENT_LANE_MODE(unit, port) || 
            PHY_HC65_FABRIC_MODE(unit, port)) {
            rv = _phy_hl65_control_serdes_driver_tune_set(unit, port, 
                  pc->lane_num,value);
        } else {
            for (i = 0; i < HL65_NUM_LANES; i++) {
                rv = _phy_hl65_control_serdes_driver_tune_set(unit, port, 
                          i ,value);
                if (rv != SOC_E_NONE) {
                    break;
                }
            }
        } 
        break;
    case SOC_PHY_CONTROL_8B10B:
        rv = _phy_hl65_control_8b10b_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_64B66B:
        rv = _phy_hl65_control_64b66b_set(unit, port, value);
	break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_hl65_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_SERDES_TUNE_MARGIN_MODE:
        if (PHY_HC65_FABRIC_MODE(unit, port)) {    
            rv = _phy_hl65_control_serdes_tune_margin_mode_set(unit, port, 
                                                               pc->lane_num, 
                                                               value);
        } else {
           rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_SERDES_TUNE_MARGIN_VALUE:
        if (PHY_HC65_FABRIC_MODE(unit, port)) {    
            rv = _phy_hl65_control_serdes_tune_margin_value_set(unit, port, 
                                                            pc->lane_num,
                                                            value);
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL_CONTROL1000X2r(unit, pc, 
              value? DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK:0, 
              DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_EQUALIZER_BOOST:
       /*
          RX Equalizer Boost settings for stack ports
          So modifying register 0x1C(RX0/1/2/3_ANACONTROL) 
          of block 0x8250 (reg 0x825C)
        */
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0x0, 0x825C, value));
        rv = SOC_E_NONE;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}

/*
 * Function:
 *      phy_hl65_control_get
 * Purpose:
 *      Get current control settign of the PHY. 
 * Parameters:
 *      unit  - StrataSwitch unit #.
 *      port  - StrataSwitch port #. 
 *      type  - Control to update 
 *      value - (OUT)Current setting for the control 
 * Returns:     
 *      SOC_E_NONE
 */
STATIC int
phy_hl65_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
    int i;
    uint16 data16 = 0;

    if (NULL == value) {
        return SOC_E_PARAM;
    }
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return SOC_E_PARAM;
    }
   
    pc = INT_PHY_SW_STATE(unit, port);
    rv = SOC_E_UNAVAIL;
    switch(type) {
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE0:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE1:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE2:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT_LANE3:
        /* fall through */
    case SOC_PHY_CONTROL_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_PREEMPHASIS:
        /* fall through */
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_hl65_control_tx_driver_get(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_SCRAMBLER:
        rv = _phy_hl65_control_scrambler_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_hl65_control_prbs_polynomial_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_hl65_control_prbs_tx_invert_data_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = _phy_hl65_control_prbs_tx_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_hl65_control_prbs_rx_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = _phy_hl65_control_prbs_rx_status_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_SERDES_DRIVER_EQUALIZATION_TUNE_STATUS_FAR_END:
        if (PHY_INDEPENDENT_LANE_MODE(unit, port) || 
            PHY_HC65_FABRIC_MODE(unit, port)) {
            rv = _phy_hl65_control_eq_tune_status_get(unit, port, 
                                                     pc->lane_num,value);
        } else {
            for (i = 0; i < HL65_NUM_LANES; i++) {
                rv = _phy_hl65_control_eq_tune_status_get(unit, port, 
                                                     i,value);
                if ((*value != TRUE) || (rv != SOC_E_NONE)) {
                    break;
                }
            }
        }
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_hl65_control_linkdown_transmit_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_SERDES_TUNE_MARGIN_MODE:
        if (PHY_HC65_FABRIC_MODE(unit, port)) {    
            rv = _phy_hl65_control_serdes_tune_margin_mode_get(unit, port, 
                                                               pc->lane_num,
                                                               value);
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_SERDES_TUNE_MARGIN_MAX:
        if (PHY_HC65_FABRIC_MODE(unit, port)) {    
            rv = _phy_hl65_control_serdes_tune_margin_max_get(unit, port, 
                                                              pc->lane_num,
                                                              value);
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_SERDES_TUNE_MARGIN_VALUE:
        if (PHY_HC65_FABRIC_MODE(unit, port)) {    
            rv = _phy_hl65_control_serdes_tune_margin_value_get(unit, port, 
                                                                pc->lane_num,
                                                                value);
        } else {
            rv = SOC_E_UNAVAIL;
        }
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_HL65_DIGITAL_CONTROL1000X2r(unit, pc,&data16));
        *value = data16 & DIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK?
                 TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_EQUALIZER_BOOST:
       /*
          RX Equalizer Boost settings for stack ports
          Reading register 0x1C(RX0/1/2/3_ANACONTROL) 
          of block 0x8250 (reg 0x825C)
        */
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit, pc, 0x0, 0x825C, &data16));
        *value = data16;
        rv = SOC_E_NONE;
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break; 
    }
    return rv;
}     

/*
 * Function:
 *      phy_hl65_reg_read
 * Purpose:
 *      Routine to read PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - (OUT) Value read from PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_hl65_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_read(unit, pc, phy_reg_addr, &data));

   *phy_data = data;

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_reg_write
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_data     - Value write to PHY register
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_hl65_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_write(unit, pc, phy_reg_addr, data));

    return SOC_E_NONE;
}  

/*
 * Function:
 *      phy_hl65_reg_modify
 * Purpose:
 *      Routine to write PHY register
 * Parameters:
 *      uint         - BCM unit number
 *      pc           - PHY state
 *      flags        - Flags which specify the register type
 *      phy_reg_addr - Encoded register address
 *      phy_mo_data  - New value for the bits specified in phy_mo_mask
 *      phy_mo_mask  - Bit mask to modify
 * Note:
 *      This register read function is not thread safe. Higher level
 * function that calls this function must obtain a per port lock
 * to avoid overriding register page mapping between threads.
 */
STATIC int
phy_hl65_reg_modify(int unit, soc_port_t port, uint32 flags,
                    uint32 phy_reg_addr, uint32 phy_data,
                    uint32 phy_data_mask)
{
    uint16               data;     /* Temporary holder for phy_data */
    uint16               mask;
    phy_ctrl_t    *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);
    mask      = (uint16) (phy_data_mask & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_modify(unit, pc, phy_reg_addr, data, mask));

    return SOC_E_NONE;
}

STATIC int
phy_hl65_probe(int unit, phy_ctrl_t *pc)
{
    uint16      serdes_id0;
   
    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    if (!(((serdes_id0 & TEST_SERDESID0_MODEL_NUMBER_MASK) ==
             SERDES_ID0_MODEL_NUMBER_HYPERLITE) || 
          ((serdes_id0 & TEST_SERDESID0_MODEL_NUMBER_MASK) ==
             SERDES_ID0_MODEL_NUMBER_HYPERCORE))) {
        return SOC_E_NOT_FOUND;
    }

    if ((serdes_id0 & TEST_SERDESID0_TECH_PROC_MASK) !=
        SERDES_ID0_TECH_PROC_65NM) {
        return SOC_E_NOT_FOUND;
    }

    return SOC_E_NONE;
}

STATIC int
phy_hl65_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_hl65_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_hl65_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_hl65_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_hl65_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_hl65_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_hl65_an_set(unit, port, value));
        break;
    case phyEventTxFifoReset:
        rv = (_phy_hl65_tx_fifo_reset(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
    }

    return rv;
}

STATIC int
phy_hl65_linkup_evt (int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    int an,an_done,speed;
                                                                                
    pc = INT_PHY_SW_STATE(unit, port);

    phy_hl65_an_get(unit,port,&an,&an_done);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_linkup_evt: "
                         "u=%d p=%d an=%d\n"), unit, port, an));
    /* check if the link is established due to the auto-negotiation. If so,
     * configure the tx half rate and DSP related registers. These settings
     * can only be done after the autoneg speed is determined.
     */
    if (an == TRUE) {

        /* check the autoneg speed */
        phy_hl65_speed_get(unit,port,&speed);
                                                                                
        /* set tx half rate. */
        SOC_IF_ERROR_RETURN
            (_phy_hl65_half_rate_set(unit,port,speed));
                                                                                
        if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
                                                                                
            /* configure DSP related registers based on speed */
            _phy_hl65_dsp_cfg(unit,pc,speed);
        }

        /* for speed above 10G, txcko_div must be cleared */
        if (speed > 10000) {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                      0,
                                      XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK));
        } else {
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                      XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK,
                                      XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK));
        }
    }
    return SOC_E_NONE;
}

STATIC int
phy_hl65_linkdown_evt (int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    int an,an_done; 
    uint16 dfe_brcdr;
                                                  
    pc = INT_PHY_SW_STATE(unit, port);
                                                                                
    phy_hl65_an_get(unit,port,&an,&an_done);
                                                                                
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_linkdown_evt: "
                         "u=%d p=%d an=%d\n"), unit, port, an));

    /* The DSP related registers must be set to the default
     * values before the autoneg starts.
     */
    if (an == TRUE) {
        if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {

            /* check if DFE/BR CDR(20G speed). if so, restart AN */
            SOC_IF_ERROR_RETURN
                (READ_HL65_DSC2BB_DSC_MISC_CTRL0r(unit, pc, &dfe_brcdr));

            if ((dfe_brcdr == HL65_A0A1_DSC_DFE_BRCDR) ||
                (dfe_brcdr == HL65_B0C0_DSC_DFE_BRCDR) ) {
                phy_hl65_an_set(unit,port,TRUE);
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_duplex_set
 * Purpose:
 *      Set the current duplex mode (forced).
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, true indicates full duplex, false indicates half.
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
phy_hl65_duplex_set(int unit, soc_port_t port, int duplex)
{
    uint16       data16;
    phy_ctrl_t  *pc;
    uint16      serdes_id0;
    int duplex_value;

    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL_STATUS1000X1r(unit, pc, &data16));

    if (!(data16 & DIGITAL_STATUS1000X1_SGMII_MODE_MASK)) {

        SOC_IF_ERROR_RETURN
            (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

        /* 1000X fiber mode, 100fx  */
            
        if ((serdes_id0 & TEST_SERDESID0_REV_LETTER_MASK) > 0) { 
            duplex_value = duplex? FX100_CONTROL1_FULL_DUPLEX_MASK:0;
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_FX100_CONTROL1r(unit,pc,
                       duplex_value,
                       FX100_CONTROL1_FULL_DUPLEX_MASK));
        }
        /* 1000X should always be full duplex */
        duplex = TRUE;
    }

    data16 = duplex? MII_CTRL_FD: 0;

    /* program the duplex setting */
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, data16,MII_CTRL_FD));

    return SOC_E_NONE;
}

STATIC int
phy_hl65_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        *duplex = TRUE;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL_STATUS1000X1r(unit, pc, &reg0_16));

    /* default to fiber mode duplex */
    *duplex = (reg0_16 & DIGITAL_STATUS1000X1_DUPLEX_STATUS_MASK)?
              TRUE:FALSE;

    if (reg0_16 & DIGITAL_STATUS1000X1_SGMII_MODE_MASK) {

    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_HL65_COMBO_IEEE0_AUTONEGLPABILr(unit,pc,&reg0_16));

            /* make sure link partner is also in SGMII mode
             * otherwise fall through to use the FD bit in MII_CTRL reg
             */
            if (reg0_16 & MII_ANP_SGMII_MODE) {
                if (reg0_16 & MII_ANP_SGMII_FD) {
                    *duplex = TRUE;
                } else {
                    *duplex = FALSE;
                }
                return SOC_E_NONE;
            }
        }
        *duplex = (mii_ctrl & MII_CTRL_FD) ? TRUE : FALSE;
    } 

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_hl65_diag_ctrl
 * Purpose:
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 * Returns:
 *      SOC_E_NONE
 * Notes:
 */

STATIC int
phy_hl65_diag_ctrl(
   int unit, /* unit */
   soc_port_t port, /* port */
   uint32 inst, /* the specific device block the control action directs to */
   int op_type,  /* operation types: read,write or command sequence */
   int op_cmd,   /* command code */
   void *arg)     /* command argument based on op_type/op_cmd */
{
    int rv;

    rv = SOC_E_NONE;

    switch(op_cmd) {

        case PHY_DIAG_CTRL_DSC:
             LOG_INFO(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "phy_hl65_diag_ctrl: "
                                  "u=%d p=%d PHY_DIAG_CTRL_DSC 0x%x\n"),
                       unit, port, PHY_DIAG_CTRL_DSC));
            (void)phy_hl65_uc_status_dump(unit, port);
        break;

        default:
            if (op_type == PHY_DIAG_CTRL_SET) {
                rv = phy_hl65_control_set(unit, port, op_cmd, PTR_TO_INT(arg));
            } else if (op_type == PHY_DIAG_CTRL_GET) {
                rv = phy_hl65_control_get(unit, port, op_cmd, (uint32 *)arg);
            }
        break;
    }
    return rv;
}

#define HL65_UC_DEBUG
#ifdef HL65_UC_DEBUG
typedef struct {
    int postc_metric;
    int pf_ctrl;
    int vga_sum;
    int dfe_bin;
    int integ_reg;
    int integ_reg_xfer;
    int clk90_offset;
    int slicer_target;
    int offset_pe;
    int offset_ze;
    int offset_me;
    int offset_po;
    int offset_zo;
    int offset_mo;
    int tx_os;
    int tx_br;
} HL65_DESC;
#define LANE0_ACCESS      1
#define LANE1_ACCESS      2
#define LANE2_ACCESS      3
#define LANE3_ACCESS      4
#define MAX_LANES 4
static HL65_DESC hl65_desc;
    
int
phy_hl65_uc_status_dump(int unit, soc_port_t port)
{
    phy_ctrl_t  *pc;
    HL65_DESC * pDesc;
    int lane_num;
    uint16 data16;
    int regval, reg_addr;
    uint8 print_menu = TRUE;

    pc = INT_PHY_SW_STATE(unit, port);

    for (lane_num = 0; lane_num < MAX_LANES; lane_num++) {
        sal_memset((char *)&hl65_desc, 0, (sizeof(HL65_DESC)));
        pDesc = &hl65_desc;
        
        /* integ_reg(lane) = rd22_integ_reg (phy, lane) */
        reg_addr = 0x82b0 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 65535;
        if (regval >= 32768) {
            regval -=65536;
        }
        regval /= 84;
        pDesc->integ_reg = regval;
     
        /* integ_reg_xfer(lane) = rd22_integ_reg_xfer (phy, lane)   */
        reg_addr = 0x82b1 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 65535;
        if (regval >= 32768) {
            regval -=65536;
        }
        regval /= 84;
        pDesc->integ_reg_xfer = regval;

        /* clk90_offset(lane) = rd22_clk90_phase_offset (phy, lane)   */
        reg_addr = 0x82b3 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval /= 128;
        regval &= 127;
        pDesc->clk90_offset = regval;
        
        /* pf_ctrl(lane) = rd22_pf_ctrl (phy, lane) */
        reg_addr = 0x82bb + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 0x7;
        pDesc->pf_ctrl = regval;
        
        /* slicer_target(lane) = ((25*rd22_rx_thresh_sel (phy, lane)) + 125)   */
        reg_addr = 0x821c + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval /= 32;
        regval &= 3;
        pDesc->slicer_target = regval * 25 + 125;

        /* vga_sum(lane) = rd22_vga_sum (phy, lane) */
        reg_addr = 0x82b5 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval /= 64;
        regval &= 0x1f;
        pDesc->vga_sum = regval;
       
        /* dfe_bin(lane) = rd22_dfe_tap_bin (phy, lane) */ 
        reg_addr = 0x82b5 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 0x3f;
        pDesc->dfe_bin = regval;
         
        /* postc_metric(lane) = rd22_postc_metric (phy, lane)*/
        reg_addr = 0x82b8 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 0x7ff;
        if (regval >=1024) {
            regval -= 2048;
        } 
        pDesc->postc_metric = regval;

        /* offset_mo(lane) = rd22_slicer_offset_mo (phy, lane) */
        reg_addr = 0x82b9 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 0x1f;
        if(regval > 16) {
            regval -= 32;
        }
        pDesc->offset_mo = regval;

        /* offset_zo(lane) = rd22_slicer_offset_zo (phy, lane) */
        regval = data16;
        regval /= 32;
        regval &= 0x1f;
        if(regval > 16) {
            regval -= 32;
        }
        pDesc->offset_zo = regval;

        /* offset_po(lane) = rd22_slicer_offset_po (phy, lane) */
        regval = data16;
        regval /= 1024;
        regval &= 0x1f;
        if(regval > 16) {
            regval -= 32;
        }
        pDesc->offset_po = regval;
       
        /* offset_me(lane) = rd22_slicer_offset_me (phy, lane)   */
        reg_addr = 0x82ba + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        regval &= 0x1f;
        if(regval > 16) {
            regval -= 32;
        }
        pDesc->offset_me = regval;

        /* offset_ze(lane) = rd22_slicer_offset_ze (phy, lane)   */
        regval = data16;
        regval /= 32;
        regval &= 0x1f;
        if(regval > 16) {
            regval -= 32;
        }
        pDesc->offset_ze = regval;

        /* offset_pe(lane) = rd22_slicer_offset_pe (phy, lane)   */
        regval = data16;
        regval /= 1024;
        regval &= 0x1f;
        if(regval > 16) {
            regval -= 32;
        }
        pDesc->offset_pe = regval;
    
        reg_addr = 0x8067 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        pDesc->tx_os = regval;
        
        reg_addr = 0x8068 + (lane_num * 0x10);
        SOC_IF_ERROR_RETURN
            (HL65_REG_READ(unit,pc,0x0,reg_addr, &data16));
        regval = data16;
        pDesc->tx_br = regval;
        
        SOC_IF_ERROR_RETURN
            (READ_HL65_TXPLL_ANAPLLASTATUS1r(unit, pc, &data16));
        /* regval = (data16 >> 8) & 0xf; */

        if( print_menu == TRUE) { 
            LOG_CLI((BSL_META_U(unit,
                                "\n\nDSC parameters for port %d\n\n"), port));
            LOG_CLI((BSL_META_U(unit,
                                "PLL Range: %d\n\n"),data16));

            LOG_CLI((BSL_META_U(unit,
                                "LN PPM  PPM_XFR clk90_ofs " 
                     "PF   SL_TRGT VGA  DFE  " 
                                "TXDIR_OS TXDR_BR  MTRC PE   "
                                "ZE   ME  PO  ZO  "
                                "MO\n")));
            print_menu = FALSE;
        }
        LOG_CLI((BSL_META_U(unit,
                            "%02d %04d %07d %09d "
                 "%04d %07d %04d %04d "
                            "0x%06x 0x%06x %04d %04d "
                            "%04d %2d %3d %3d "
                            "%2d\n"),
                 lane_num, pDesc->integ_reg, pDesc->integ_reg_xfer,
                 pDesc->clk90_offset, pDesc->pf_ctrl,
                 pDesc->slicer_target, pDesc->vga_sum, pDesc->dfe_bin,
                 pDesc->tx_os, pDesc->tx_br,
                 pDesc->postc_metric, pDesc->offset_pe,
                 pDesc->offset_ze, pDesc->offset_me,
                 pDesc->offset_po, pDesc->offset_zo, 
                 pDesc->offset_mo));

    }

    return SOC_E_NONE;
}
#endif

/*
 * Variable:
 *      phy_hl65_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */
phy_driver_t phy_hl65_hg = {
    /* .drv_name                      = */ "HyperLite PHY Driver",
    /* .pd_init                       = */ phy_hl65_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_hl65_link_get,
    /* .pd_enable_set                 = */ phy_hl65_enable_set,
    /* .pd_enable_get                 = */ phy_hl65_enable_get,
    /* .pd_duplex_set                 = */ phy_hl65_duplex_set,
    /* .pd_duplex_get                 = */ phy_hl65_duplex_get,
    /* .pd_speed_set                  = */ phy_hl65_speed_set,
    /* .pd_speed_get                  = */ phy_hl65_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_null_zero_get,
    /* .pd_an_set                     = */ phy_hl65_an_set,
    /* .pd_an_get                     = */ phy_hl65_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */
    /* .pd_lb_set                     = */ phy_hl65_lb_set,
    /* .pd_lb_get                     = */ phy_hl65_lb_get,
    /* .pd_interface_set              = */ phy_hl65_interface_set,
    /* .pd_interface_get              = */ phy_hl65_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */
    /* .pd_linkup_evt                 = */ phy_hl65_linkup_evt,
    /* .pd_linkdn_evt                 = */ phy_hl65_linkdown_evt,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ NULL,
    /* .pd_medium_config_get          = */ NULL, 
    /* .pd_medium_get                 = */ phy_null_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_hl65_control_set,    
    /* .pd_control_get                = */ phy_hl65_control_get,
    /* .pd_reg_read                   = */ phy_hl65_reg_read,
    /* .pd_reg_write                  = */ phy_hl65_reg_write, 
    /* .pd_reg_modify                 = */ phy_hl65_reg_modify,
    /* .pd_notify                     = */ phy_hl65_notify,
    /* .pd_probe                      = */ phy_hl65_probe,
    /* .pd_ability_advert_set         = */ phy_hl65_ability_advert_set,
    /* .pd_ability_advert_get         = */ phy_hl65_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_hl65_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_hl65_ability_local_get,
    /* .pd_firmware_set               = */ NULL,
    /* .pdpd_timesync_config_set      = */ NULL,
    /* .pdpd_timesync_config_get      = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pdpd_timesync_control_set     = */ NULL,
    /* .pd_diag_ctrl                  = */ phy_hl65_diag_ctrl
};

/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_hl65_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdeshl65 is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      duplex - Boolean, TRUE indicates full duplex, FALSE
 *              indicates half.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_hl65_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_hl65_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc,
                                          MII_CTRL_FD, MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_hl65_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_COMBO_IEEE0_MIICNTLr(unit, pc, mii_ctrl, MII_CTRL_FD));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_hl65_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_hl65_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdeshl65 is an intermediate PHY.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      speed - Speed to program.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      If PHY_FLAGS_FIBER is set, it indicates the PHY is being used to
 *      talk directly to an external fiber module.
 *
 *      If PHY_FLAGS_FIBER is clear, the PHY is being used in
 *      pass-through mode to talk to an external SGMII PHY.
 *
 *      When used in pass-through mode, autoneg must be turned off and
 *      the speed/duplex forced to match that of the external PHY.
 */

STATIC int
_phy_hl65_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_hl65_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_HL65_DIGITAL_STATUS1000X1r(unit, pc, &fiber_status));

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_hl65_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_hl65_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_hl65_an_set(unit, port, FALSE));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_hl65_stop
 * Purpose:
 *      Put serdeshl65 SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_hl65_stop(int unit, soc_port_t port)
{
    int                 stop, copper;
    uint16 mask16,data16;
    phy_ctrl_t  *pc;
    int mdio_addr_share = FALSE;

    if (!PHY_INDEPENDENT_LANE_MODE(unit, port)) {
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    copper = (pc->stop &
              PHY_STOP_COPPER) != 0;

    stop = ((pc->stop &
             (PHY_STOP_PHY_DIS |
              PHY_STOP_DRAIN)) != 0 ||
            (copper &&
             (pc->stop &
              (PHY_STOP_MAC_DIS |
               PHY_STOP_DUPLEX_CHG |
               PHY_STOP_SPEED_CHG)) != 0));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    /* check for dxgxs ports. Don't disable RX lane, we're using rx_ck clock
     * for MAC Rx block registers in 10G mode
     */
    mask16 = (1 << pc->lane_num);    /* rx lane */
    if (IS_HG_PORT(unit, port) || IS_XE_PORT(unit,port)) {
        mask16 <<= 4; /* assign tx lane */
    } else {
        mask16 |= (mask16 << 4); /* add tx lane */
    }

    if (stop) {
        mask16 |= 0x800;
        data16 = mask16;
    } else {
        data16 = 0;
    }

    /* point to lane0 mdio addressing. 
     * This register can only be accessed from lane0.
     */
    if (pc->flags & PHYCTRL_MDIO_ADDR_SHARE) {
        mdio_addr_share = TRUE;
        pc->flags &= ~PHYCTRL_MDIO_ADDR_SHARE;
    } else {
        pc->phy_id -= pc->lane_num;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_XGXSBLK1_LANECTRL3r(unit,pc,data16,mask16));

    /* restore the previous MDIO address mode */
    if (mdio_addr_share == TRUE) {
        pc->flags |= PHYCTRL_MDIO_ADDR_SHARE;
    } else {
        pc->phy_id += pc->lane_num;
    }
 
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_hl65_stop: u=%d p=%d mask=0x%x value=0x%x\n"),
              unit, port, mask16, data16));
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_hl65_notify_stop
 * Purpose:
 *      Add a reason to put serdeshl65 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_hl65_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return _phy_hl65_stop(unit, port);
}

/*  
 * Function:
 *      _phy_hl65_notify_resume
 * Purpose:
 *      Remove a reason to put serdeshl65 PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_hl65_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    
    return _phy_hl65_stop(unit, port);
}

/*
 * Function:
 *      phy_hl65_media_setup
 * Purpose:     
 *      Configure 
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      fiber_mode - Configure for fiber mode
 *      fiber_pref - Fiber preferrred (if fiber mode)
 * Returns:     
 *      SOC_E_XXX
 */
STATIC int
_phy_hl65_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;
    
    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
 
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_CONTROL1000X1r(unit, pc, data16,
                           DIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
 
    return SOC_E_NONE;
}

STATIC
int _phy_hl65_analog_dsc_init (int unit,int port, int lane)
{
    uint16      serdes_id0;
    int         reg;
    phy_ctrl_t  *pc;
    
    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                            TEST_SERDESID0_REV_LETTER_MASK);

    /* use default if wrong lane specified */
    if (lane < 0 || lane > 3) {
        lane = HL_ALL_LANES;
    }

    if (lane == HL_ALL_LANES) {
        reg = 0x82A0;
    } else {
        reg = 0x8260 + lane * 0x10;
    }

    if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_A1) || 
        (serdes_id0 == HL_SERDES_ID0_REVID_B0) ) { 

        if ((serdes_id0 == HL_SERDES_ID0_REVID_A0) ||
            (serdes_id0 == HL_SERDES_ID0_REVID_A1)) {

            /*  Set this bit for the best possible PLL jitter performance */
            SOC_IF_ERROR_RETURN
                (MODIFY_HL65_TXPLL_ANAPLLACONTROL4r(unit, pc, 
                               PLL_PLLACONTROL4_BIT12_MASK,
                               PLL_PLLACONTROL4_BIT12_MASK));
        }

        /* Set lpq = 200uA */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_TXPLL_ANAPLLACONTROL2r(unit, pc, (1 << 2),(1 << 2)));

        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x0, 0x21));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x2, 0x3800));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x3,  0x421b));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x4, 0x53e4));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x7, 0x14A5));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x8, 0x03e0));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0x9, 0x0fc0));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0xb, 0x0312));
        SOC_IF_ERROR_RETURN
            (HL65_REG_WRITE(unit, pc, 0, reg + 0xc, 0x046a));

    } else if (serdes_id0 == HL_SERDES_ID0_REVID_C0) {
        /* Set lpq = 200uA */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_TXPLL_ANAPLLACONTROL2r(unit, pc, (1 << 2),(1 << 2)));
    }
    return SOC_E_NONE;
}

STATIC
int _hl65_xgmii_scw_config (int unit, phy_ctrl_t *pc)
{
    uint16      serdes_id0;

    /* only apply to hypercore */

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    if ((serdes_id0 & TEST_SERDESID0_MODEL_NUMBER_MASK) ==
             SERDES_ID0_MODEL_NUMBER_HYPERLITE) { 
        return(SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW0r(unit,pc,0xE070));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW1r(unit,pc,0xC0D0));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW2r(unit,pc,0xA0B0));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW3r(unit,pc,0x8090));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW0_MASKr(unit,pc,0xF0F0));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW1_MASKr(unit,pc,0xF0F0));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW2_MASKr(unit,pc,0xF0F0));
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_RX66_A_SCW3_MASKr(unit,pc,0xF0F0));
    
    return(SOC_E_NONE);
}

STATIC 
int _hl65_rxaui_config(int unit, phy_ctrl_t  *pc,int rxaui)
{
    uint16 mask16,data16;
    mask16 = XGXSBLK1_LANECTRL3_PWRDWN_FORCE_MASK |
                0xc |    /* lane 2 and 3 RX */
                (0xc << XGXSBLK1_LANECTRL3_PWRDN_TX_SHIFT);  /* TX */

    /* if in reduced XAUI mode, disable lane 2 and 3 */
    data16 = rxaui? mask16: 0;

    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_XGXSBLK1_LANECTRL3r(unit,pc, data16,mask16));

    return(SOC_E_NONE);
}

STATIC
int _hl65_soft_reset(int unit, phy_ctrl_t  *pc)
{
    /* Set AER to DTE */    
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_AERBLK_AERr(unit,pc, 0x2800));

    /* select multi mmd */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_XGXSBLK0_MMDSELECTr(unit, pc, 0x400F));

    if (soc_property_get(unit, spn_SERDES_LANE0_RESET, 1)) {
        /* soft reset via DTE  */
        SOC_IF_ERROR_RETURN
            (WRITE_HL65_IEEE0BLK_MIICNTLr(unit,pc, 0xA040));
        
        /* wait 2us for reset to complete */
        sal_usleep(2);
    }
    /* restore the AER to default */
    SOC_IF_ERROR_RETURN
        (WRITE_HL65_AERBLK_AERr(unit,pc, 0x0));

    return(SOC_E_NONE);
}

/*
 * This routine initializes some of the device's lane specific registers 
 * applying to all modes. In the combo mode, the AER register should be
 * set to broadcast to all lanes before entering this function.
 */
STATIC
int _hl65_lane_reg_init(int unit, phy_ctrl_t  *pc)
{
    uint16      serdes_id0;
    uint16      data16;

    SOC_IF_ERROR_RETURN
        (READ_HL65_TEST_SERDESID0r(unit, pc, &serdes_id0));

    serdes_id0 = serdes_id0 & (TEST_SERDESID0_REV_NUMBER_MASK |
                            TEST_SERDESID0_REV_LETTER_MASK);

    if ((serdes_id0 == HL_SERDES_ID0_REVID_B0) ||
        (serdes_id0 == HL_SERDES_ID0_REVID_C0)) {

        /* Revert Back to original Rx fifo if C0/B0 version */
        SOC_IF_ERROR_RETURN
            (MODIFY_HL65_DIGITAL4_MISC3r(unit, pc,
                   0,
                   DIGITAL4_MISC3_FIFO_IPG_CYA_MASK));
    }

    data16 = (1 << 2);   /* Set FIFO Elasticity to 13.5k */
    data16 |= soc_feature(unit, soc_feature_unimac_tx_crs)?
              DIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK:
              0;  /* set disable_tx_crs bit accordingly */
 
    SOC_IF_ERROR_RETURN
        (MODIFY_HL65_DIGITAL_CONTROL1000X3r(unit, pc,
             data16,
             DIGITAL_CONTROL1000X3_DISABLE_TX_CRS_MASK |
             DIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_RX_MASK));

    return(SOC_E_NONE);
}


#else /* INCLUDE_XGXS_HL65 */
typedef int _xgxs_hl65_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_XGXS_HL65 */
