/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        xgxs16g.c
 * Purpose:     Support Broadcom 1000/2500/10000/12000/13000/16000 SerDes(XGXS-16G)
 *              Support BCM 40nm Xenia core XGXS_13G_040_FC 
 */
#include "phydefs.h"      /* Must include before other phy related includes */ 
#if defined(INCLUDE_XGXS_16G)
#include "phyconfig.h"     /* Must include before other phy related includes */
#include <sal/types.h>
#include <shared/bsl.h>
#include <sal/types.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>
#include <soc/port_ability.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>


#include "phyident.h"
#include "phyreg.h"
#include "phynull.h"
#include "xgxs.h"
#include "serdesid.h"
#include "xgxs16g.h"

#define TX_DRIVER_DFT_LN_CTRL  0x4
#define PHY_XGXS16G_LANEPRBS_LANE_SHIFT   4

/* Notify event forward declaration */
STATIC int phy_xgxs16g_interface_get(int unit, soc_port_t port,
                                       soc_port_if_t *pif);
STATIC int phy_xgxs16g_master_get(int unit, soc_port_t port, int *master);
STATIC int _phy_xgxs16g_notify_duplex(int unit, soc_port_t port, uint32 duplex);
STATIC int _phy_xgxs16g_notify_speed(int unit, soc_port_t port, uint32 speed);
STATIC int _phy_xgxs16g_notify_stop(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_xgxs16g_notify_resume(int unit, soc_port_t port, uint32 flags);
STATIC int _phy_xgxs16g_notify_interface(int unit, soc_port_t port, uint32 intf);
STATIC int _phy_xgxs16g_notify_mac_loopback(int unit, soc_port_t port, uint32 enable);
#define XGXS16G_PLL_WAIT  250000
STATIC int phy_xgxs16g_an_set(int unit, soc_port_t port, int an);
STATIC int phy_xgxs16g_an_get(int unit, soc_port_t port,
                              int *an, int *an_done);
STATIC int phy_xgxs16g_speed_get(int unit, soc_port_t port, int *speed);
STATIC int phy_xgxs16g_ability_advert_set(int unit, soc_port_t port, 
                                         soc_port_ability_t *ability);
STATIC int phy_xgxs16g_ability_advert_get(int unit, soc_port_t port,
                                         soc_port_ability_t *ability);
STATIC int phy_xgxs16g_ability_local_get(int unit, soc_port_t port,
                                   soc_port_ability_t *ability); 
STATIC int phy_xgxs16g_speed_set(int unit, soc_port_t port, int speed);
STATIC int _phy_xgxs16g_control_vco_disturbed_set(int unit, soc_port_t port);
STATIC int _phy_xgxs16g_control_vco_disturbed_get(int unit, soc_port_t port,
                                                  uint32 *value);

STATIC int
_phy_xgxs16g_pll_lock_wait(int unit, soc_port_t port)
{
    uint16           data16;
    phy_ctrl_t      *pc;
    soc_timeout_t    to;
    int              rv;

    pc = INT_PHY_SW_STATE(unit, port);
    _phy_xgxs16g_control_vco_disturbed_set(unit, port);
    soc_timeout_init(&to, XGXS16G_PLL_WAIT, 0);
    do {
        rv = READ_XGXS16G_XGXSBLK0_XGXSSTATUSr(unit, pc, &data16);
        if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) ||
            SOC_FAILURE(rv)) {
            break;
        }
    } while (!soc_timeout_check(&to));
    if ((data16 & XGXSBLK0_XGXSSTATUS_TXPLL_LOCK_MASK) == 0) {
        LOG_WARN(BSL_LS_SOC_PHY,
                 (BSL_META_U(unit,
                             "XGXS_16G : TXPLL did not lock: u=%d p=%d\n"),
                  unit, port));
        return (SOC_E_TIMEOUT);
    }
    return (SOC_E_NONE);
}

STATIC int
_phy_xgxs16g_pin_out_set(int unit, soc_port_t port)
{
    uint16      data16;
    uint16      mask16;
    phy_ctrl_t *pc;
    uint32      phy_xaui_tx_polarity_flip;
    uint32      phy_xaui_rx_polarity_flip=0;

    pc = INT_PHY_SW_STATE(unit, port);

    /* 
     * Transform CX4 pin out on the board to HG pinout
     */
    if (soc_property_port_get(unit, port, spn_CX4_TO_HIGIG, FALSE)) {
        if (FEATURE_NEW_LANESWAP(pc)) {
            SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                (WRITE_XGXS16G_XGXSBLK8_TXLNSWAP1r(unit, pc, 0x80e4));
        } else {
            /* Swap TX lanes */
            data16 = XGXSBLK2_TXLNSWAP_TX_LNSWAP_EN_MASK; 
            mask16 = XGXSBLK2_TXLNSWAP_TX_LNSWAP_EN_MASK;
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_XGXSBLK2_TXLNSWAPr(unit, pc, data16, mask16));
        }

        /* Flip TX polarity */
        data16 = TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
        mask16 = TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_TX_ALL_TX_ACONTROL0r(unit, pc, data16, mask16));
    } else {
        /* If CX4 to HG conversion is enabled, do not allow individual lane
         * swapping.
         */
        int i;
        uint16 lane_map;
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
                if (FEATURE_NEW_LANESWAP(pc)) {
                    /* swap the hw_map */
                    for (hw_map=0, i = 3; i >= 0; i--) {
                        lane  = (lane_map >> (i * 4)) & 0xf;
                        hw_map  |= lane << ((3 - i) * 2);
                    }
                    SOC_IF_ERROR_RETURN     /* Enable RX Lane swap */
                       (MODIFY_XGXS16G_XGXSBLK8_RXLNSWAP1r(unit, pc, 
                           hw_map,
                           XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                           XGXSBLK8_RXLNSWAP1_RX0_LNSWAP_SEL_MASK |
                           XGXSBLK8_RXLNSWAP1_RX1_LNSWAP_SEL_MASK |
                           XGXSBLK8_RXLNSWAP1_RX2_LNSWAP_SEL_MASK |
                           XGXSBLK8_RXLNSWAP1_RX3_LNSWAP_SEL_MASK));

                } else {
                    hw_map |= XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                              XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE_EN_MASK;
                    mask16  = XGXSBLK2_RXLNSWAP_RX_LNSWAP_EN_MASK |
                              XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE_EN_MASK |
                              XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE0_MASK |
                              XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE1_MASK |
                              XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE2_MASK |
                              XGXSBLK2_RXLNSWAP_RX_LNSWAP_FORCE3_MASK;
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_XGXSBLK2_RXLNSWAPr(unit, pc, 
                                                       hw_map, mask16));
                }
            } else {
                LOG_WARN(BSL_LS_SOC_PHY,
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
                if (FEATURE_NEW_LANESWAP(pc)) {
                    /* swap the hw_map */
                    for (hw_map=0, i = 3; i >= 0; i--) {
                        lane  = (lane_map >> (i * 4)) & 0xf;
                        hw_map  |= lane << ((3 - i) * 2);
                    }
                    SOC_IF_ERROR_RETURN     /* Enable TX Lane swap */
                       (MODIFY_XGXS16G_XGXSBLK8_TXLNSWAP1r(unit, pc, 
                            hw_map,
                            XGXSBLK8_TXLNSWAP1_TX3_LNSWAP_SEL_MASK |
                            XGXSBLK8_TXLNSWAP1_TX2_LNSWAP_SEL_MASK |
                            XGXSBLK8_TXLNSWAP1_TX1_LNSWAP_SEL_MASK |
                            XGXSBLK8_TXLNSWAP1_TX0_LNSWAP_SEL_MASK));
                } else {
                    hw_map |= XGXSBLK2_TXLNSWAP_TX_LNSWAP_EN_MASK;
                    mask16  = XGXSBLK2_TXLNSWAP_TX_LNSWAP_EN_MASK |
                              XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE0_MASK |
                              XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE1_MASK |
                              XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE2_MASK |
                              XGXSBLK2_TXLNSWAP_TX_LNSWAP_FORCE3_MASK;
                    SOC_IF_ERROR_RETURN
                        (MODIFY_XGXS16G_XGXSBLK2_TXLNSWAPr(unit, pc,
                                                       hw_map, mask16));
                }
            } else {
                LOG_WARN(BSL_LS_SOC_PHY,
                         (BSL_META_U(unit,
                                     "unit %d port %s: Invalid TX lane map 0x%04x.\n"),
                          unit, SOC_PORT_NAME(unit, port), lane_map));
            }
        }
        phy_xaui_tx_polarity_flip = soc_property_port_get(unit, port,
                                              spn_PHY_XAUI_TX_POLARITY_FLIP, 0);
        if (phy_xaui_tx_polarity_flip) {
            /* Flip TX polarity */
            data16 = TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
            mask16 = TX0_TX_ACONTROL0_TXPOL_FLIP_MASK;
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "unit %d port %s: TX Polarity swap 0x%04x.\n"),
                      unit, SOC_PORT_NAME(unit, port), data16));
            if (phy_xaui_tx_polarity_flip == 1) {
                SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_TX_ALL_TX_ACONTROL0r(unit, pc, data16, mask16));
            }
            if ((phy_xaui_tx_polarity_flip & 0x000F) == 0x000F) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_TX0_TX_ACONTROL0r(unit, pc, data16, mask16));
            }
            if ((phy_xaui_tx_polarity_flip & 0x00F0) == 0x00F0) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_TX1_TX_ACONTROL0r(unit, pc, data16, mask16));
            }
            if ((phy_xaui_tx_polarity_flip & 0x0F00) == 0x0F00) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_TX2_TX_ACONTROL0r(unit, pc, data16, mask16));
            }
            if ((phy_xaui_tx_polarity_flip & 0xF000) == 0xF000) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_TX3_TX_ACONTROL0r(unit, pc, data16, mask16));
            }
        }
        phy_xaui_rx_polarity_flip = soc_property_port_get(unit, port,
                                              spn_PHY_XAUI_RX_POLARITY_FLIP, 0);
        if (phy_xaui_rx_polarity_flip) {
            /* Flip RX polarity */
            data16 = 0x06; /* RX_POLARITY_FORCE_R | RX_POLARIRT_R */
            mask16 = 0x06;
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "unit %d port %s: RX Polarity swap 0x%04x.\n"),
                      unit, SOC_PORT_NAME(unit, port), data16));
            if (phy_xaui_rx_polarity_flip == 1) {
                SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_RX_CONTROL_PCIEr(unit, pc, data16, mask16));
            }
            if ((phy_xaui_rx_polarity_flip & 0x000F) == 0x000F) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_RX0_ANA_RX_CONTROL_PCIEr(unit, pc, data16, mask16));
            }
            if ((phy_xaui_rx_polarity_flip & 0x00F0) == 0x00F0) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_RX1_ANA_RX_CONTROL_PCIEr(unit, pc, data16, mask16));
            }
            if ((phy_xaui_rx_polarity_flip & 0x0F00) == 0x0F00) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_RX2_ANA_RX_CONTROL_PCIEr(unit, pc, data16, mask16));
            }
            if ((phy_xaui_rx_polarity_flip & 0xF000) == 0xF000) {
                SOC_IF_ERROR_RETURN
                  (MODIFY_XGXS16G_RX3_ANA_RX_CONTROL_PCIEr(unit, pc, data16, mask16));
            }
        }
    }
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_init
 * Purpose:     
 *      Initialize xgxs6 phys
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g_init(int unit, soc_port_t port)
{
    uint16              data16;
    uint16              mask16;
    int                 preemph, idriver, pdriver;
    soc_port_ability_t  ability;
    phy_ctrl_t         *pc;
    int                 serdes_fiber_pref_def;
    XGXS16G_DEV_DESC *pDesc;
    soc_phy_info_t *pi;
    int len;
    int serdes_id0;

    pc = INT_PHY_SW_STATE(unit, port);
    pDesc = (XGXS16G_DEV_DESC *)(pc + 1);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID2r(unit, pc, &pDesc->serdes_id0));
    serdes_id0 = pDesc->serdes_id0;
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID2r(unit, pc, &pDesc->serdes_id2));

    /* compose device name/instance/lane information for display 
     * XGXS_16G device uses existing method
     */
    if (XGXS16G_13G_ID(pc)) {
        /*
         * COVERITY
         *
         * Checked that buffer overflow will not happen for destination string.
         */
        /* coverity[secure-coding] */

        sal_strcpy(pDesc->name,"XGXS13G-");
        len = sal_strlen(pDesc->name);
        /* add rev letter */
        pDesc->name[len++] = 'A' + ((serdes_id0 >>
                          SERDESID_SERDESID0_REV_LETTER_SHIFT) & 0x3);
        /* add rev number */
        pDesc->name[len++] = '0' + ((serdes_id0 >>
                          SERDESID_SERDESID0_REV_NUMBER_SHIFT) & 0x7);
        pDesc->name[len++] = '/';
        pDesc->name[len++] = (pc->chip_num/10)%10 + '0';
        pDesc->name[len++] = pc->chip_num%10 + '0';
        pDesc->name[len++] = '/';
        pDesc->name[len++] = '4';
        pDesc->name[len] = 0;  /* string terminator */

        if (len > XGXS16G_LANE_NAME_LEN) {
            LOG_ERROR(BSL_LS_SOC_PHY,
                      (BSL_META_U(unit,
                                  "XGXS13G info string length %d exceeds max length 0x%x: u=%d p=%d\n"),
                       len,XGXS16G_LANE_NAME_LEN,unit, port));
            return SOC_E_MEMORY;
        }

        pi = &SOC_PHY_INFO(unit, port);
        if (!PHY_EXTERNAL_MODE(unit, port)) {
            pi->phy_name = pDesc->name;
        }

    }
    
    /* Reset the core */

    /* Stop PLL Sequencer and configure the core into correct mode */
    data16 = (XGXSBLK0_XGXSCONTROL_MODE_10G_ComboCoreMode <<
                XGXSBLK0_XGXSCONTROL_MODE_10G_SHIFT) |
             XGXSBLK0_XGXSCONTROL_HSTL_MASK |
             XGXSBLK0_XGXSCONTROL_CDET_EN_MASK |
             XGXSBLK0_XGXSCONTROL_EDEN_MASK |
             XGXSBLK0_XGXSCONTROL_AFRST_EN_MASK |
             XGXSBLK0_XGXSCONTROL_TXCKO_DIV_MASK;
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, data16)); 

    /* disable in-band MDIO. PHY-443 */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_MODIFY(unit, pc, 0x00, 0x8111, 1 << 3, 1 << 3));

    if (soc_property_port_get(unit, port,
                          spn_PHY_AN_C73, FALSE)) {
         PHY_FLAGS_SET(unit, port, PHY_FLAGS_C73);
    }

    /* disable CL73 BAM */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN_CL73_REG(unit,pc,0x8372,
               0,
               CL73_USERB0_CL73_BAMCTRL1_CL73_BAMEN_MASK));

    if (!PHY_CLAUSE73_MODE(unit, port)) {
        /* disable CL73 AN device*/
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_AN_CL73_REG(unit,pc,MII_CTRL_REG,0));
    }

    /* Disable IEEE block select auto-detect. 
     * The driver will select desired block as necessary.
     * By default, the driver keeps the mapping for XAUI block in
     * IEEE address space.
     */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_MISCCONTROL1r(unit, pc, 
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK,
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_AUTODET_MASK |
                           XGXSBLK0_MISCCONTROL1_IEEE_BLKSEL_VAL_MASK));

    /*
     * Configure signal auto-detection between SGMII and fiber.
     */
    data16 = SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK |
             SERDESDIGITAL_CONTROL1000X1_CRC_CHECKER_DISABLE_MASK |
             SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    if (soc_property_port_get(unit, port, spn_SERDES_AUTOMEDIUM, TRUE)) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_AUTODET_EN_MASK;
    }
    serdes_fiber_pref_def = TRUE;
    if (PHY_EXTERNAL_MODE(unit, port) && IS_GE_PORT(unit, port)) {
        serdes_fiber_pref_def = FALSE;
    }
    if (soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF,
                                serdes_fiber_pref_def)) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
    if (soc_property_port_get(unit, port, 
                              spn_SERDES_SGMII_MASTER, FALSE)) {
        data16 |= SERDESDIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK;
        mask16 |= SERDESDIGITAL_CONTROL1000X1_SGMII_MASTER_MODE_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, data16, mask16));
    
    /* Disable SerDes parallel detect */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, 0));

    /* Initialize combo SerDes to reasonable default settings */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, 
                               MII_CTRL_AE  | MII_CTRL_FD | MII_CTRL_SS_1000));

    /* Disable 10G parallel detect */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_AN73_PDET_PARDET10GCONTROLr(unit, pc, 0));

    /* Enable BAM mode */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr(unit, pc, 1));

    /* Initialize XAUI to reasonable default settings */
	if (XGXS16G_13G_ID(pc)) {
		/* 40nm */
		preemph = soc_property_port_get(unit, port, spn_XGXS_PREEMPHASIS, 0);
		idriver = soc_property_port_get(unit, port, spn_XGXS_DRIVER_CURRENT, 4);
		data16  = ((preemph & 0xf) << TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_SHIFT) |
				  ((idriver & 0x7) << TX_ALL_040_TX_DRIVER_IDRIVER_SHIFT);
		mask16  = TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_MASK |
				  TX_ALL_040_TX_DRIVER_IDRIVER_MASK;
		SOC_IF_ERROR_RETURN
			(MODIFY_XGXS13G_040_TX_ALL_ACONTROL1r(unit, pc, data16, mask16));		
	} else {
		preemph = soc_property_port_get(unit, port, spn_XGXS_PREEMPHASIS, 0);
		idriver = soc_property_port_get(unit, port, spn_XGXS_DRIVER_CURRENT, 9);
		pdriver = soc_property_port_get(unit, port, spn_XGXS_PRE_DRIVER_CURRENT, 9);
		data16  = ((preemph & 0xf) << TX_ALL_TX_DRIVER_PREEMPHASIS_SHIFT) |
				  ((idriver & 0xf) << TX_ALL_TX_DRIVER_IDRIVER_SHIFT) |
				  ((pdriver & 0xf) << TX_ALL_TX_DRIVER_IPREDRIVER_SHIFT);
		mask16  = TX_ALL_TX_DRIVER_PREEMPHASIS_MASK |
				  TX_ALL_TX_DRIVER_IDRIVER_MASK |
				  TX_ALL_TX_DRIVER_IPREDRIVER_MASK;
		SOC_IF_ERROR_RETURN
			(MODIFY_XGXS16G_TX_ALL_TX_DRIVERr(unit, pc, data16, mask16));
	}

    /* Advertise all property by default */
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_ability_local_get(unit, port, &ability));

    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_ability_advert_set(unit, port, &ability));

    /* Adjust parallel detect link timer to 60ms */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_AN73_PDET_PARDET10GLINKr(unit, pc, 0x16E2));

    /* Enable 10G parallel detect by default */
    data16 = 0;
    mask16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    if (soc_property_port_get(unit, port, spn_XGXS_PDETECT_10G, 1)) {
        data16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN73_PDET_PARDET10GCONTROLr(unit, pc, data16, mask16));
 
    /* Allow config settings to adjust pin out for XAUI lane */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_pin_out_set(unit, port));

    /* set elasticity fifo size to 13.5k to support 12k jumbo pkt size*/
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X3r(unit,pc,
                   (1 << 2),
                   SERDESDIGITAL_CONTROL1000X3_FIFO_ELASICITY_TX_RX_MASK));

    /* enable autoneg or force a speed depending on the port type */

    if (PHY_EXTERNAL_MODE(unit, port) &&
        (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port))) {
        int force_speed;

        force_speed = SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK;
        if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
            force_speed |= 4; /* 10G CX4 */
        } else {
            force_speed |= 3; /* 10G HiG */
        }

        /* disable autoneg */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             0,
                                             MII_CTRL_AE));
        if (PHY_CLAUSE73_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_AN_CL73_REG(unit, pc,MII_CTRL_REG,
                                              0,
                                              MII_CTRL_AE));
        }

        /* force speed  to 10G */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, force_speed,
                                SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
                                SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
    } else { /* autoneg */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             MII_CTRL_AE | MII_CTRL_RAN,
                                             MII_CTRL_AE | MII_CTRL_RAN));
        if (PHY_CLAUSE73_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_AN_CL73_REG(unit, pc,MII_CTRL_REG,
                                              MII_CTRL_AE | MII_CTRL_RAN,
                                              MII_CTRL_AE | MII_CTRL_RAN));
        }
    }

    if (XGXS16G_13G_ID(pc)) {
        /* Enabble LPI passthru' for native mode EEE */
        SOC_IF_ERROR_RETURN
             (MODIFY_XGXS16G_REMOTEPHY_MISC5r(unit, pc, 0xc000, 0xc000));
        SOC_IF_ERROR_RETURN
             (MODIFY_XGXS16G_XGXSBLK7_EEECONTROLr(unit, pc, 0x0007, 0x0007));
    }

    /* Start PLL Sequencer and wait for PLL to lock */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    (void) _phy_xgxs16g_pll_lock_wait(unit, port);
   
    /* PRBS Polynomial for all lanes: 0x8019=0x0000 */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, 0)); 

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_link_get
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
phy_xgxs16g_link_get(int unit, soc_port_t port, int *link)
{
    phy_ctrl_t *pc;
    uint16  xgxs_stat1;
    uint16  xgxs_stat3;
    int     autoneg;
    int     an_done;

    if (PHY_DISABLED_MODE(unit, port)) {
        *link = FALSE;
        return SOC_E_NONE;
    }

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_GP_STATUS_XGXSSTATUS3r(unit, pc, &xgxs_stat3));

    *link =  (xgxs_stat3 & GP_STATUS_XGXSSTATUS3_LINK_LATCHDOWN_MASK) ? 1 : 0;

    /*
     * Following logic is to ensure that there is no false link up status
     * in an mode. Link status is set as UP only if an_done is complete.
     */
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_an_get(unit, port, &autoneg, NULL));

    if (autoneg) {
        /* If autoneg is enabled, check for autoneg done. */
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_GP_STATUS_XGXSSTATUS1r(unit, pc, &xgxs_stat1));

        an_done = ((xgxs_stat1 & GP_STATUS_XGXSSTATUS1_AUTONEG_COMPLETE_MASK) 
                  != 0);
        if (!an_done) {
            uint16 an_status;

            /* check Clause73 autoneg */
            if (PHY_CLAUSE73_MODE(unit, port)) {
                SOC_IF_ERROR_RETURN
                    (READ_XGXS16G_GP_STATUS_TOPANSTATUS1r(unit, pc, &an_status));

                if (an_status & 
                    GP_STATUS_TOPANSTATUS1_CL73_AUTONEG_COMPLETE_MASK) {
                    an_done = TRUE; 
                }
            }
        }

        *link = (*link) && (an_done);
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_enable_set
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
phy_xgxs16g_enable_set(int unit, soc_port_t port, int enable)
{
    uint16      pmd_tx_disable;
    phy_ctrl_t *pc;
   
    pc = INT_PHY_SW_STATE(unit, port);

    if (enable) {
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_DISABLE);
        pmd_tx_disable = 0;
    } else {
        pmd_tx_disable = XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK;
        PHY_FLAGS_SET(unit, port, PHY_FLAGS_DISABLE);
    }

    /* disable the Tx */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_MISCCONTROL1r(unit, pc, pmd_tx_disable,
               XGXSBLK0_MISCCONTROL1_GLOBAL_PMD_TX_DISABLE_MASK));

    /* disable the Rx */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_RX_ALL_RX_CONTROLr(unit, pc, 
               enable? 0:DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK,
               DSC_2_0_DSC_CTRL0_RXSEQSTART_MASK));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_speed_set
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
phy_xgxs16g_speed_set(int unit, soc_port_t port, int speed)
{
    phy_ctrl_t *pc;
    uint16      force_speed;
    uint16      data16;
    uint16       speed_mii;
    uint16      sgmii_status;

    speed_mii = 0;

    /* Check for valid speed and convert valid speed to HW values */
    force_speed = SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK;
    switch (speed) {
    case 10:
        speed_mii = MII_CTRL_SS_10;
        break;
    case 100:
        speed_mii = MII_CTRL_SS_100;
        break;
    case 1000:
        speed_mii = MII_CTRL_SS_1000;
        force_speed = 0;
        break;
    case 2500:   
        speed_mii = MII_CTRL_SS_MSB | MII_CTRL_SS_LSB;
        force_speed |= 0; 
        break;
    case 5000:   
        force_speed |= 1; 
        break;
    case 6000:   
        force_speed |= 2; 
        break;
    case 0:      /* set to reasonable speed */
        /* fall through */
    case 10000:
        if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
            force_speed |= 4; /* 10G CX4 */
        } else {
            force_speed |= 3; /* 10G HiG */
        }
        break;
    case 12000:  
        force_speed |= 5;
        break;
    case 12500:
        force_speed |= 6;
        break;
    case 13000:
        force_speed |= 7;
        break;
    case 15000:
        force_speed |= 8;
        break;
    case 16000:
        force_speed |= 9;
        break;
    default:
        return (SOC_E_PARAM);
    }
   
    pc = INT_PHY_SW_STATE(unit, port);

    /* Disable autoneg */
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_an_set(unit, port, 0));

    /* Puts PLL in reset state and forces all datapath into reset state */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    if (FEATURE_SPEED_100FX(pc)) {
        /* disable 100FX and 100FX auto-detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_FX100_CONTROL1r(unit,pc,
                              0,
                              FX100_CONTROL1_AUTODET_EN_MASK |
                              FX100_CONTROL1_ENABLE_MASK));

        /* disable 100FX idle detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_FX100_CONTROL3r(unit,pc,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK,
                              FX100_CONTROL3_CORRELATOR_DISABLE_MASK));
    }

    /* Set force speed */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, force_speed,
                                SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK |
                                SERDESDIGITAL_MISC1_FORCE_SPEED_MASK));
                                    
    /* Set forced speed in IEEE control register. */
            
    if (speed <= 2500) {
        if (FEATURE_SPEED_100FX(pc)) {
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_SERDESDIGITAL_STATUS1000X1r(unit, pc, &sgmii_status));

            sgmii_status &= SERDESDIGITAL_STATUS1000X1_SGMII_MODE_MASK;
            if (!sgmii_status && (speed == 100)) {

                /* fiber mode 100fx, enable */
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_FX100_CONTROL1r(unit,pc,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK,
                                  FX100_CONTROL1_FAR_END_FAULT_EN_MASK |
                                  FX100_CONTROL1_ENABLE_MASK));

                /* enable 100fx extended packet size */
                SOC_IF_ERROR_RETURN
                    (MODIFY_XGXS16G_FX100_CONTROL2r(unit,pc,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK,
                                FX100_CONTROL2_EXTEND_PKT_SIZE_MASK));
            } else {
                SOC_IF_ERROR_RETURN
                    (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &data16));
                data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
                data16 |= speed_mii;
                SOC_IF_ERROR_RETURN
                    (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, data16));
            }

        } else {
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &data16));
            data16 &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
            data16 |= speed_mii;
            SOC_IF_ERROR_RETURN
                (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, data16));
        }
    }

    /* Bring PLL out of reset */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                  XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    (void) _phy_xgxs16g_pll_lock_wait(unit, port);

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_speed_get
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
phy_xgxs16g_speed_get(int unit, soc_port_t port, int *speed)
{
    phy_ctrl_t *pc;
    uint16      hw_speed;
    int         autoneg;

    static int  actual_speed[] = {10,      /*  0  */
                                  100,     /*  1  */
                                  1000,    /*  2  */
                                  2500,    /*  3  */
                                  5000,    /*  4  */
                                  6000,    /*  5  */
                                  10000,   /*  6  */
                                  10000,   /*  7  */
                                  12000,   /*  8  */
                                  12500,   /*  9  */
                                  13000,   /*  A  */
                                  15000,   /*  B  */
                                  16000,   /*  C  */
                                  1000,    /*  D  KX */
                                  10000};  /*  E  KX4*/
    static int  forced_speed[] = {2500,    /*  0  */
                                  5000,    /*  1  */
                                  6000,    /*  2  */
                                  10000,   /*  3  */
                                  10000,   /*  4  */
                                  12000,   /*  5  */
                                  12500,   /*  6  */
                                  13000,   /*  7  */
                                  15000,   /*  8  */
                                  16000};  /*  9  */ 

    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_an_get(unit, port, &autoneg, NULL));

    /* Set reasonable default speed */
    *speed = 10000; 
    pc = INT_PHY_SW_STATE(unit, port);
    if (autoneg) {
        /* Get actual speed */
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_GP_STATUS_TOPANSTATUS1r(unit, pc, &hw_speed));
        hw_speed = (hw_speed & GP_STATUS_TOPANSTATUS1_ACTUAL_SPEED_MASK) >>
                   GP_STATUS_TOPANSTATUS1_ACTUAL_SPEED_SHIFT;
        if (hw_speed < COUNTOF(actual_speed)) {
            *speed = actual_speed[hw_speed];
        } else {
            return (SOC_E_INTERNAL);
        }
    } else {
        /* Get forced speed */
        SOC_IF_ERROR_RETURN 
            (READ_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, &hw_speed));
        if (hw_speed & SERDESDIGITAL_MISC1_FORCE_SPEED_SEL_MASK) {
            hw_speed = (hw_speed & SERDESDIGITAL_MISC1_FORCE_SPEED_MASK) >>
                       SERDESDIGITAL_MISC1_FORCE_SPEED_SHIFT;
            if (hw_speed < COUNTOF(forced_speed)){
                *speed = forced_speed[hw_speed];
            } else {
                return (SOC_E_INTERNAL);
            }
        } else {
            uint16       speed_mii;
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &speed_mii));
            speed_mii &= (MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);
            switch (speed_mii) {
            case MII_CTRL_SS_10:
                *speed = 10;
                break;
            case MII_CTRL_SS_100:
                *speed = 100;
                break;
            case MII_CTRL_SS_1000:
                *speed = 1000;
                break;
            case (MII_CTRL_SS_MSB | MII_CTRL_SS_LSB):   
                *speed = 2500;
                break;
            default:
                *speed = pc->speed_max;
                break;
            }
        }
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g_speed_get: u=%d p=%d GP_STATUS_TOPANSTATUS1 %04x speed= %d\n"),
              unit, port, hw_speed,*speed));

    return (SOC_E_NONE);
}
/*
 * Function:
 *      phy_xgxs16g_master_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_MS_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      The master mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_xgxs16g_master_get(int unit, soc_port_t port, int *master)
{
    *master = SOC_PORT_MS_NONE;
    return SOC_E_NONE;
}
/*
 * Function:
 *      phy_xgxs16g_interface_get
 * Purpose:
 *      this function is meant for 1000Base-T PHY. Added here to support
 *      internal PHY regression test
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      master - (OUT) SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 * Notes:
 *      Interface mode is retrieved for the ACTIVE medium.
 */

STATIC int
phy_xgxs16g_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{  
   int serdes_fiber_pref = 0; 
   int speed = 0;
   SOC_IF_ERROR_RETURN(phy_xgxs16g_speed_get(unit, port, &speed));
   switch(speed)
   {
   case 10:
   case 100:
        *pif = SOC_PORT_IF_MII;
        break;
   case 1000:
        serdes_fiber_pref = soc_property_port_get(unit, port, spn_SERDES_FIBER_PREF,
                                0); 
            if(serdes_fiber_pref == TRUE) {
                *pif = SOC_PORT_IF_GMII;
            } else {
                *pif = SOC_PORT_IF_SGMII;
            }
        break;
   case 2500:
        *pif = SOC_PORT_IF_GMII;
        break;
   case 10000:
   case 13000:
   default:
        *pif = SOC_PORT_IF_XGMII;
        break;
   }
   return SOC_E_NONE;
}


/*
 * Function:    
 *      phy_xgxs16g_an_set
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
STATIC int
phy_xgxs16g_an_set(int unit, soc_port_t port, int an)
{
    uint16      data16;
    uint16      mask16;
    int         cur_autoneg;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_an_get(unit, port, &cur_autoneg, NULL));

    /* If request to disable autoneg and autoneg is already disabled,
     * do nothing.
     */ 
    if ((!cur_autoneg) && (!an)) {
        return (SOC_E_NONE);
    }

    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_BAM_NEXTPAGE_MP5_NEXTPAGECTRLr(unit, pc, an ? 1 : 0));

    /* If request to enable autoneg and autoneg is already enabled,
     * restart autoneg.
     */
    if (cur_autoneg && an) {
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc,
                                             MII_CTRL_AE | MII_CTRL_RAN,
                                             MII_CTRL_AE | MII_CTRL_RAN));
        if (PHY_CLAUSE73_MODE(unit, port)) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_AN_CL73_REG(unit, pc,MII_CTRL_REG,
                                              MII_CTRL_AE,
                                              MII_CTRL_AE));
        }
    }

    /* Do the following sequence if changing autoneg setting. */
    /* Stop PLL Sequencer */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));

    /* Disable/Enable 10G parallel detect */
    data16 = 0;
    mask16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    if (soc_property_port_get(unit, port, spn_XGXS_PDETECT_10G, 1) && an) {
        data16 = AN73_PDET_PARDET10GCONTROL_PARDET10G_EN_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN73_PDET_PARDET10GCONTROLr(unit, pc, data16, mask16));
    
    /* Disable/Enable 1000X parallel detect */
    data16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    mask16 = SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK;
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data16, mask16));

    /* Clause 73 backplane support */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        
    }

    /* Enable autoneg and restart autoneg */
    data16 = (an) ? MII_CTRL_AE | MII_CTRL_RAN : 0;
    mask16 = MII_CTRL_AE | MII_CTRL_RAN;
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, data16, mask16));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_AN_CL73_REG(unit, pc, MII_CTRL_REG,
                                          data16,
                                          mask16));
    }

    /* Start PLL Sequencer and wait for PLL to lock */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK,
                                 XGXSBLK0_XGXSCONTROL_START_SEQUENCER_MASK));
    (void) _phy_xgxs16g_pll_lock_wait(unit, port);

    return (SOC_E_NONE);
}

/*
 * Function:    
 *      phy_xgxs16g_an_get
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
phy_xgxs16g_an_get(int unit, soc_port_t port, int *an, int *an_done)
{
    uint16      mii_ctrl;
    uint16      an_status;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    *an = ((mii_ctrl & MII_CTRL_AE) != 0);

    if (NULL != an_done) {
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_GP_STATUS_TOPANSTATUS1r(unit, pc, &an_status));
        *an_done = ((an_status & 
                     GP_STATUS_TOPANSTATUS1_CL37_AUTONEG_COMPLETE_MASK) != 0);

        if (PHY_CLAUSE73_MODE(unit, port)) {
            *an_done |= ((an_status &
                         GP_STATUS_TOPANSTATUS1_CL73_AUTONEG_COMPLETE_MASK) != 
                        0);
        }
    }

    return (SOC_E_NONE);
}

STATIC int
_phy_xgxs16g_c73_adv_local_set(int unit, soc_port_t port,
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
        (MODIFY_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT1_REG, an_adv,
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
        break;
    }

    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT0_REG, pause,
                                     (CL73_AN_ADV_PAUSE |
                                      CL73_AN_ADV_ASYM_PAUSE)));

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_c73_adv_local_set: u=%d p=%d pause=%08x speeds=%04x,adv=0x%x\n"),
              unit, port, pause, an_adv,ability->speed_full_duplex));
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_c73_adv_local_get(int unit, soc_port_t port,
                            soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   speeds,pause;
    phy_ctrl_t       *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT1_REG, &an_adv));

    speeds = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    speeds |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= speeds;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_ADVERT0_REG, &an_adv));

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
            break;
    }
    ability->pause = pause;

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_c73_adv_local_get: u=%d p=%d pause=%08x speeds=%04x\n"),
              unit, port, pause, speeds));
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_c73_adv_remote_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;
     pc = INT_PHY_SW_STATE(unit, port);
     SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc,CL73_AN_LP_ABILITY1_REG, &an_adv));

    mode = (an_adv & CL73_AN_ADV_TECH_1G_KX) ? SOC_PA_SPEED_1000MB : 0;
    mode |= (an_adv & CL73_AN_ADV_TECH_10G_KX4) ? SOC_PA_SPEED_10GB : 0;
    ability->speed_full_duplex |= mode;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_AN_CL73_REG(unit, pc, CL73_AN_LP_ABILITY0_REG,&an_adv));

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
 *      phy_xgxs16g_ability_advert_set
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
phy_xgxs16g_ability_advert_set(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    uint16           adv_over1g;
    uint16           an_adv;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    /* Support only full duplex */
    an_adv = (ability->speed_full_duplex & SOC_PA_SPEED_1000MB) ? MII_ANP_C37_FD : 0;
    switch (ability->pause & (SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX)) {
    case SOC_PA_PAUSE_TX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE;
        break;
    case SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE;
        break;
    case SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX:
        an_adv |= MII_ANP_C37_PAUSE;
        break;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_COMBO_IEEE0_AUTONEGADVr(unit, pc, an_adv));

    mode = ability->speed_full_duplex;
    adv_over1g = 0;
    adv_over1g |= (mode & SOC_PA_SPEED_2500MB) ? OVER1G_UP1_DATARATE_2P5GX1_MASK : 0;
    if (mode & SOC_PA_SPEED_10GB) {
        if (soc_property_port_get(unit, port, spn_10G_IS_CX4, TRUE)) {
            adv_over1g |= OVER1G_UP1_DATARATE_10GCX4_MASK;
        } else {
            adv_over1g |= OVER1G_UP1_DATARATE_10GX4_MASK;
        }
    }
    adv_over1g |= (mode & SOC_PA_SPEED_12GB) ? OVER1G_UP1_DATARATE_12GX4_MASK : 0;
    adv_over1g |= (mode & SOC_PA_SPEED_13GB) ? OVER1G_UP1_DATARATE_13GX4_MASK : 0;
    adv_over1g |= (mode & SOC_PA_SPEED_16GB) ? OVER1G_UP1_DATARATE_16GX4_MASK : 0;
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_OVER1G_UP1r(unit, pc, adv_over1g));

    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_xgxs16g_c73_adv_local_set(unit, port, ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g_ability_advert_set:u=%d p=%d pause=%08x OVER1G_UP1 %04x\n"),
              unit, port, ability->pause, an_adv));
    
    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_ability_advert_get
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
phy_xgxs16g_ability_advert_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    uint16           adv_over1g;
    uint16           an_adv;
    soc_port_mode_t  mode;
    phy_ctrl_t      *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    sal_memset(ability, 0, sizeof(*ability));
    mode = 0;
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_AUTONEGADVr(unit, pc, &an_adv));

    switch (an_adv & (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE)) {
    case MII_ANP_C37_ASYM_PAUSE:
        mode |= SOC_PA_PAUSE_TX;
        break;
    case (MII_ANP_C37_ASYM_PAUSE | MII_ANP_C37_PAUSE):
        mode |= SOC_PA_PAUSE_RX;
        break;
    case MII_ANP_C37_PAUSE:
        mode |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
        break;
    }
    ability->pause = mode;

    mode = 0;
    mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_OVER1G_UP1r(unit, pc, &adv_over1g));
    mode |= (adv_over1g & OVER1G_UP1_DATARATE_2P5GX1_MASK) ? 
              SOC_PA_SPEED_2500MB : 0;
    mode |= (adv_over1g & OVER1G_UP1_DATARATE_10GCX4_MASK) ?
              SOC_PA_SPEED_10GB : 0;
    mode |= (adv_over1g & OVER1G_UP1_DATARATE_10GX4_MASK) ?
              SOC_PA_SPEED_10GB : 0;
    mode |= (adv_over1g & OVER1G_UP1_DATARATE_12GX4_MASK) ?
              SOC_PA_SPEED_12GB : 0;
    mode |= (adv_over1g & OVER1G_UP1_DATARATE_13GX4_MASK) ?
              SOC_PA_SPEED_13GB : 0;
    mode |= (adv_over1g & OVER1G_UP1_DATARATE_16GX4_MASK) ?
              SOC_PA_SPEED_16GB : 0;

    ability->speed_full_duplex = mode;

    /* check for clause73 */
    if (PHY_CLAUSE73_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_phy_xgxs16g_c73_adv_local_get(unit, port, ability));
    }

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g_ability_advert_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_ability_remote_get
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
phy_xgxs16g_ability_remote_get(int unit, soc_port_t port,
                               soc_port_ability_t *ability)
{
    uint16            an_status;
    uint16            an_done;
    uint16            link_stat_gp;
    uint16            an_adv;
    soc_port_mode_t   mode;
    phy_ctrl_t       *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }
 
    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
            (READ_XGXS16G_GP_STATUS_TOPANSTATUS1r(unit, pc, &an_status));

    an_done = an_status & (GP_STATUS_TOPANSTATUS1_CL37_AUTONEG_COMPLETE_MASK |
                           GP_STATUS_TOPANSTATUS1_CL73_AUTONEG_COMPLETE_MASK);
    link_stat_gp = an_status & GP_STATUS_TOPANSTATUS1_LINK_STATUS_MASK;
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "u=%d p=%d an_status=%04x link_stat_gp=%04x\n"),
              unit, port, an_status, link_stat_gp));

    sal_memset(ability, 0, sizeof(*ability));

    mode = 0;
    if (link_stat_gp && an_done) {
        /* Decode remote advertisement only when link is up and autoneg is
         * completed.
         */

        if (an_status & GP_STATUS_TOPANSTATUS1_CL37_AUTONEG_COMPLETE_MASK) {
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_GP_STATUS_LP_UP1r(unit, pc, &an_adv));
            mode |= (an_adv & OVER1G_LP_UP1_DATARATE_16GX4_MASK) ?
                     SOC_PA_SPEED_16GB : 0;
            mode |= (an_adv & OVER1G_LP_UP1_DATARATE_13GX4_MASK) ?
                     SOC_PA_SPEED_13GB : 0;
            mode |= (an_adv & OVER1G_LP_UP1_DATARATE_12GX4_MASK) ?
                     SOC_PA_SPEED_12GB : 0;
            mode |= (an_adv & OVER1G_LP_UP1_DATARATE_10GCX4_MASK) ?
                     SOC_PA_SPEED_10GB : 0;
            mode |= (an_adv & OVER1G_LP_UP1_DATARATE_10GX4_MASK) ?
                     SOC_PA_SPEED_10GB : 0;
            mode |= (an_adv & OVER1G_LP_UP1_DATARATE_2P5GX1_MASK) ?
                     SOC_PA_SPEED_2500MB : 0;

            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "u=%d p=%d over1G an_adv=%04x\n"),
                      unit, port, an_adv));
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_COMBO_IEEE0_AUTONEGLPABILr(unit, pc, &an_adv));
            LOG_INFO(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "u=%d p=%d combo an_adv=%04x\n"),
                      unit, port, an_adv));

            mode |= (an_adv & MII_ANP_C37_FD) ? SOC_PA_SPEED_1000MB : 0;
            ability->speed_full_duplex = mode;

            mode = 0; 
            switch (an_adv & (MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE)) {
                case MII_ANP_C37_PAUSE:
                    mode |= SOC_PA_PAUSE_TX | SOC_PA_PAUSE_RX;
                    break;
                case MII_ANP_C37_ASYM_PAUSE:
                    mode |= SOC_PA_PAUSE_TX;
                    break;
                case MII_ANP_C37_PAUSE | MII_ANP_C37_ASYM_PAUSE:
                    mode |= SOC_PA_PAUSE_RX;
                    break;
            }
            ability->pause = mode;
        }
        if (an_status & GP_STATUS_TOPANSTATUS1_CL73_AUTONEG_COMPLETE_MASK) {
            if (PHY_CLAUSE73_MODE(unit, port)) {
                SOC_IF_ERROR_RETURN
                    (_phy_xgxs16g_c73_adv_remote_get(unit, port, ability));
            }
        }
    } else {
        /* Simply return local abilities */
        phy_xgxs16g_ability_advert_get(unit, port, ability);
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "phy_xgxs16g_ability_remote_get:unit=%d p=%d pause=%08x sp=%08x\n"),
              unit, port, ability->pause, ability->speed_full_duplex));

    return (SOC_E_NONE);
}


/*
 * Function:
 *      phy_xgxs16g_lb_set
 * Purpose:
 *      Put XGXS6/FusionCore in PHY loopback
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g_lb_set(int unit, soc_port_t port, int enable)
{
    uint16      misc_ctrl;
    uint16      lb_bit;
    uint16      lb_mask;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);
    
    /* Configure Loopback in XAUI */
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK0_MISCCONTROL1r(unit, pc, &misc_ctrl));
    if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PCS_DEV_EN_OVERRIDE_MASK) {
          /* PCS */
          lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
          lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
    } else if (misc_ctrl & XGXSBLK0_MISCCONTROL1_PMD_DEV_EN_OVERRIDE_MASK) {
          /* PMA/PMD */
          lb_bit  = (enable) ? 1 : 0;
          lb_mask = 1;
    } else {
          /* PHY XS, DTE XS */
          lb_bit  = (enable) ? IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK : 0;
          lb_mask = IEEE0BLK_IEEECONTROL0_GLOOPBACK_MASK;
    }
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_IEEE0BLK_IEEECONTROL0r(unit, pc, lb_bit, lb_mask));

    /* Configure Loopback in SerDes */
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, 
                                             (enable) ? MII_CTRL_LE : 0,
                                             MII_CTRL_LE));

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_lb_get
 * Purpose:
 *      Get XGXS6/FusionCore PHY loopback state
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #. 
 *      enable - address of location to store binary value for on/off (1/0)
 * Returns:     
 *      SOC_E_NONE
 */

STATIC int
phy_xgxs16g_lb_get(int unit, soc_port_t port, int *enable)
{
    uint16      mii_ctrl;
    phy_ctrl_t *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));
    *enable = ((mii_ctrl & MII_CTRL_LE) == MII_CTRL_LE);

    return (SOC_E_NONE);
}


STATIC int
phy_xgxs16g_ability_local_get(int unit, soc_port_t port,
                              soc_port_ability_t *ability)
{
    phy_ctrl_t *pc;

    if (NULL == ability) {
        return (SOC_E_PARAM);
    }

    pc = INT_PHY_SW_STATE(unit, port);

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_MEDIUM_COPPER;
    ability->loopback  = SOC_PA_LB_PHY;
    ability->flags     = SOC_PA_AUTONEG;

    ability->speed_full_duplex  = SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB;
    if (FEATURE_SPEED_100FX(pc)) {
        ability->speed_full_duplex  |= SOC_PA_SPEED_100MB;
    }

    /*
     * If not in fiber mode or in copper mode, advertise 10/100 Mbps
     * capability. Other wise assume 1000X mode of operation for
     * below 10Gbps speed operation.
     */
    if (!PHY_FIBER_MODE(unit, port) || PHY_COPPER_MODE(unit, port)) {
        if (IS_XE_PORT(unit, port)) {
            ability->speed_half_duplex  |= SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
        } else if (IS_GE_PORT(unit, port)) {
            ability->speed_half_duplex  |= SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
            ability->speed_full_duplex  |= SOC_PA_SPEED_10MB |
                                          SOC_PA_SPEED_100MB;
            ability->speed_full_duplex &= ~SOC_PA_SPEED_2500MB;
        }
    }
    switch(pc->speed_max) {
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
            break; 
    }

    return (SOC_E_NONE);
}


STATIC int
_phy_xgxs16g_tx_driver_field_get(phy_ctrl_t *pc, soc_phy_control_t type,
								int *ln_ctrl, uint16 *mask, int *shfter)
{
    int lane_ctrl;
    int is_40nm;
	
    is_40nm = XGXS16G_13G_ID(pc);
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
         *shfter = is_40nm ? TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_SHIFT :
						TX_ALL_TX_DRIVER_PREEMPHASIS_SHIFT;
         *mask = is_40nm ? TX_ALL_040_TX_DRIVER_PREEMPHASIS_POST_MASK : 
						TX_ALL_TX_DRIVER_PREEMPHASIS_MASK;
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
         *shfter = is_40nm ? TX_ALL_040_TX_DRIVER_IDRIVER_SHIFT : 
		 TX_ALL_TX_DRIVER_IDRIVER_SHIFT;
         *mask = is_40nm ? TX_ALL_040_TX_DRIVER_IDRIVER_MASK : 
		 TX_ALL_TX_DRIVER_IDRIVER_MASK;
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
        if(is_40nm) {
            /* 40nm analog does not support pre driver configuration */
            *shfter = 0;
            *mask = 0;
        } else {		 
            *shfter = TX_ALL_TX_DRIVER_IPREDRIVER_SHIFT;
            *mask = TX_ALL_TX_DRIVER_IPREDRIVER_MASK;
        }
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
    default:
         /* should never get here */
         return SOC_E_PARAM;
    }
    *ln_ctrl = lane_ctrl;
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_tx_driver_set(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 value)
{
    uint16  data;             /* Temporary holder of reg value to be written */
    uint16  mask;             /* Bit mask of reg value to be updated */
    int     lane_ctrl;
    int     shifter;
	int 	is_40nm;

    if (value > 15) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_tx_driver_field_get(pc, type,&lane_ctrl,&mask,&shifter));

	is_40nm = XGXS16G_13G_ID(pc);
    data = value << shifter;
	
	switch (lane_ctrl) {
		case 0: 
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					MODIFY_XGXS13G_040_TX0_ACONTROL1r(unit, pc, data, mask) :
					MODIFY_XGXS16G_TX0_TX_DRIVERr(unit, pc, data, mask));
			break;
		case 1: 
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					MODIFY_XGXS13G_040_TX1_ACONTROL1r(unit, pc, data, mask) :
					MODIFY_XGXS16G_TX1_TX_DRIVERr(unit, pc, data, mask));
			break;
		case 2: 
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					MODIFY_XGXS13G_040_TX2_ACONTROL1r(unit, pc, data, mask) :
					MODIFY_XGXS16G_TX2_TX_DRIVERr(unit, pc, data, mask));
			break;
		case 3: 
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					MODIFY_XGXS13G_040_TX3_ACONTROL1r(unit, pc, data, mask) :
					MODIFY_XGXS16G_TX3_TX_DRIVERr(unit, pc, data, mask));
			break;
		case TX_DRIVER_DFT_LN_CTRL: 
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					MODIFY_XGXS13G_040_TX_ALL_ACONTROL1r(unit, pc, data, mask) :
					MODIFY_XGXS16G_TX_ALL_TX_DRIVERr(unit, pc, data, mask));
			break;
		default:
			break;
	}	
#if 0
    int     lane_num;
    if (lane_ctrl != TX_DRIVER_DFT_LN_CTRL) {
        lane_num = lane_ctrl;
        if (lane_num == 0) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_TX0_TX_DRIVERr(unit, pc, data, mask));
        } else if (lane_num == 1) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_TX1_TX_DRIVERr(unit, pc, data, mask));
        } else if (lane_num == 2) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_TX2_TX_DRIVERr(unit, pc, data, mask));
        } else if (lane_num == 3) {
            SOC_IF_ERROR_RETURN
                (MODIFY_XGXS16G_TX3_TX_DRIVERr(unit, pc, data, mask));
        }
    } else {
        /* Update preemphasis/driver/pre-driver current */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_TX0_TX_DRIVERr(unit, pc, data, mask));
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_TX1_TX_DRIVERr(unit, pc, data, mask));
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_TX2_TX_DRIVERr(unit, pc, data, mask));
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_TX3_TX_DRIVERr(unit, pc, data, mask));
    }
#endif
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g_control_linkdown_transmit_set(int unit, soc_port_t port, 
                                           uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (value) {
        data = (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
                SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data));
    } else {
        data = (SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_DISABLE_FALSE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_BITS <<
                SERDESDIGITAL_CONTROL1000X2_FILTER_FORCE_LINK_SHIFT) |
               (SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_BITS <<
                SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_SHIFT);
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, data));
    }
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_linkdown_transmit_get(int unit, soc_port_t port, 
                                           uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
            (READ_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc, &data));

    *value = (data & 
              (SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_BITS <<
               SERDESDIGITAL_CONTROL1000X2_FORCE_XMIT_DATA_ON_TXSIDE_SHIFT))
             ? 1 : 0;

    return SOC_E_NONE;
}



STATIC int
_phy_xgxs16g_control_prbs_polynomial_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    value &= 7;
    data = 0x8888 | value 
        |(value << PHY_XGXS16G_LANEPRBS_LANE_SHIFT)
        |(value << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT*2))
        |(value << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT*3));
    
      
    /* 
     * write back updated configuration 
     */
    SOC_IF_ERROR_RETURN
          (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_prbs_polynomial_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    /* Extract prbs polynomial setting from register */
    data = ((data >> (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num)) &
            XGXSBLK1_LANEPRBS_PRBS_ORDER0_MASK);
    *value = (uint32) data;

    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g_control_prbs_tx_invert_data_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);		
    uint32      lane_mask = 1 << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT * pc->lane_num);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    
    /* prbs_inv is bit 2 per lane */
    if (value) {
        data |= lane_mask << XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    } else {
        data &= ~(lane_mask << XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT);
    }

    /* write back updated configuration */
    if (value > 0x8000) {
        SOC_IF_ERROR_RETURN
	    (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, value));
    } else {
        SOC_IF_ERROR_RETURN
	    (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));
    } 

    return SOC_E_NONE;

}

STATIC int
_phy_xgxs16g_control_prbs_tx_invert_data_get(int unit, soc_port_t port, uint32 *value)
{
    uint16      data;
    uint16      inv_shifter = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));
    
   
    inv_shifter = (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num) + 
                 XGXSBLK1_LANEPRBS_PRBS_INV0_SHIFT;
    data &= (1 << inv_shifter);
 
    *value = (data) ?  1 : 0;
    
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g_control_xgxs_prbs_tx_enable_set(int unit, soc_port_t port, 
                                          uint32 value)
{
    uint16      data = 0;
    uint16      pll_mode = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    if (value) {

        /* First set XAUI mode: 0x8000 = 0x203f */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x203f));
        
        sal_usleep(1000);
        
        /* Set double wide div/1: 0x8016=0xffff */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANECTRL1r(unit, pc, 0xffff));
        
        /* clear multiPRT mode: 0x800d=0x0003 */
        /*SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_MMDSELECTr(unit, pc, 0x4003));
	*/

        /* Set in individual lane mode, Disable PLL Sequencer: 0x8000=0x063f */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x063f));
        
        /* Force VCO based on current operating frequency */
        SOC_IF_ERROR_RETURN 
            (READ_XGXS16G_PLL_PLLSTATUSr(unit, pc, &pll_mode)); 
        pll_mode &= 
            (PLL_PLLSTATUS_PLL_MODE_AFE_MASK | PLL_PLLSTATUS_RESERVED0_MASK); 
        SOC_IF_ERROR_RETURN 
            (READ_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, &data)); 
        data &= ~SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_MASK; 
        data |= SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SEL_MASK | 
            (pll_mode << SERDESDIGITAL_MISC1_FORCE_PLL_MODE_AFE_SHIFT); 
        SOC_IF_ERROR_RETURN 
            (WRITE_XGXS16G_SERDESDIGITAL_MISC1r(unit, pc, data)); 

        /* Disable asymmetric mode for 40nm UC_B0 */
        if(XGXS16G_13G_REV_B0(pc)){
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_XGXSBLK8_TXLNSWAP1r(unit, pc, 0,
                     XGXSBLK8_TXLNSWAP1_ASYMM_EN_MASK));        
        }
        /* Turn AN off: 0x0=0x0 */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, 0));
        
        /* Turn CL36 PCS off: 0x8015=0x0 */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANECTRL0r(unit, pc, 0));
        
        /* Put port in loopback if requested */
        data = ((value & 0x8000) == 0x8000) ? 
               XGXSBLK1_LANECTRL2_GLOOP1G_MASK : 0;
        /* CDED & EDEN off: 0x8017 */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANECTRL2r(unit, pc, data));
 
        /* PRBS Polynomial for all lanes: 0x8019=0x8888 */
        SOC_IF_ERROR_RETURN
           (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));
        data |= 0x8888;
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));
        
 
        /* Enable PRBS read: 0x80f1=0x1c47 */ 
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_RX_ALL_RX_CONTROLr(unit, pc, 0x1c47));
        
        /* Enable PLL Sequencer: 0x8000=0x263f */
        SOC_IF_ERROR_RETURN
            (WRITE_XGXS16G_XGXSBLK0_XGXSCONTROLr(unit, pc, 0x263f));
        
        /* Delay a while */
        sal_usleep(250000);
    } else {
        /* Restore port */
        return phy_xgxs16g_init(unit, port);
    }
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_prbs_tx_enable_set(int unit, soc_port_t port, uint32 value)
{

    SOC_IF_ERROR_RETURN(
			_phy_xgxs16g_control_xgxs_prbs_tx_enable_set(unit, port, value));
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g_control_prbs_tx_rx_enable_get(int unit, soc_port_t port, 
                                        uint32 *value)
{
    uint16      data;
    uint16       inv_shifter = 0;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /*
     * Hypercore PRBS - note that in the Hypercore there is only 1 enable 
     * for both TX/RX 
     */
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    inv_shifter = (XGXSBLK1_LANEPRBS_PRBS_ORDER1_SHIFT * pc->lane_num) +
                XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    data &= (1 << inv_shifter);
 
    *value = (data) ?  1 : 0;
  
    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_prbs_tx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    return _phy_xgxs16g_control_prbs_tx_rx_enable_get(unit, port, value);
}

STATIC int
_phy_xgxs16g_control_prbs_rx_enable_set(int unit, soc_port_t port, uint32 value)
{
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    uint32      lane_mask = 1 << (PHY_XGXS16G_LANEPRBS_LANE_SHIFT * pc->lane_num);

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, &data));

    if (value) {
        data |= lane_mask <<  XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT;
    }  else {
        data &= ~(lane_mask << XGXSBLK1_LANEPRBS_PRBS_EN0_SHIFT);
    }
    
    /* write back updated configuration */
    SOC_IF_ERROR_RETURN
        (WRITE_XGXS16G_XGXSBLK1_LANEPRBSr(unit, pc, data));
    
    /* Set up status collection */
    /* uPrbsRxControlReg = 0x80b1;
     * uPrbsRxControlReg += uLaneAddr * 0x10;
     */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), &data));
    
    data &= ~RX0_RX_CONTROL_STATUS_SEL_MASK;
    if (value) {
        data |= RX0_RX_CONTROL_STATUS_SEL_prbsStatus;
    } else {
        data &= ~RX0_RX_CONTROL_STATUS_SEL_prbsStatus;
    }
    
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), data));

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_prbs_rx_enable_get(int unit, soc_port_t port, uint32 *value)
{
    return _phy_xgxs16g_control_prbs_tx_rx_enable_get(unit, port, value);
}

/*
 * Returns value 
 *      ==  0: PRBS receive is in sync
 *      == -1: PRBS receive never got into sync
 *      ==  n: number of errors
 */
STATIC int
_phy_xgxs16g_control_xgxs_prbs_rx_status_get(int unit, soc_port_t port, 
                                          uint32 *value)
{
    int         lane;
    uint16      data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);;

    
    /* Get status for all 4 lanes and check for sync
     * 0x80b0, 0x80c0, 0x80d0, 0x80e0 
     */
    *value = 0;
    for (lane = 0; lane < 4; lane++) {
        SOC_IF_ERROR_RETURN
            (XGXS16G_REG_READ(unit, pc, 0x0, 0x80b0 + (0x10 * lane), &data));
        if (data == (RX0_RX_STATUS_PRBS_STATUS_PRBS_LOCK_BITS  <<
                     RX0_RX_STATUS_PRBS_STATUS_PRBS_LOCK_SHIFT)) {
            /* PRBS is in sync */
            continue;
        } else if (data == 0) {
            /* PRBS not in sync */
            *value = -1;            
	    break;
        } else {
            /* Get errors */
            *value += data & RX0_RX_STATUS_PRBS_STATUS_PTBS_ERRORS_MASK;
        }
    }
    return SOC_E_NONE;
}


STATIC int
_phy_xgxs16g_control_prbs_rx_status_get(int unit, soc_port_t port, uint32 *value)
{
    int         rv;
    uint16      rx_sts, saved_rx_sts;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);

    /* Set up prbs rx status collection */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_READ(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), &rx_sts));
    saved_rx_sts = rx_sts;
    rx_sts = (rx_sts & ~RX0_RX_CONTROL_STATUS_SEL_MASK) |
             RX0_RX_CONTROL_STATUS_SEL_prbsStatus;
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), rx_sts));

    rv = _phy_xgxs16g_control_xgxs_prbs_rx_status_get(unit, port, value);


    /* Restore rx status */
    SOC_IF_ERROR_RETURN
        (XGXS16G_REG_WRITE(unit,pc,0x0,0x80B1 + (0x10 * pc->lane_num), saved_rx_sts));

    return rv;
}



/*
 * Function:
 *      phy_xgxs16g_control_set
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
phy_xgxs16g_control_set(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 value)
{
    int rv;
    phy_ctrl_t *pc;
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
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_xgxs16g_control_tx_driver_set(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_EQUALIZER_BOOST:
        rv = MODIFY_XGXS16G_RX_ALL_RX_ANALOGBIAS0r(unit, pc, value,
                                  RX_ALL_RX_EQ_BOOST_EQUALIZER_CONTROL_MASK);
        break;
    case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_xgxs16g_control_linkdown_transmit_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_xgxs16g_control_prbs_polynomial_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_xgxs16g_control_prbs_tx_invert_data_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = _phy_xgxs16g_control_prbs_tx_enable_set(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_xgxs16g_control_prbs_rx_enable_set(unit, port, value);
        break;

    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        /* enable 1000X parallel detect */
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc,
            value? SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK:0,              SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK));
        rv = SOC_E_NONE;
        break;

    default:
        rv = (SOC_E_UNAVAIL);
        break;
    }
    return rv;
}

STATIC int
_phy_xgxs16g_control_tx_driver_get(int unit, phy_ctrl_t *pc,
                                soc_phy_control_t type, uint32 *value)
{
    uint16  data16;           /* Temporary holder of 16 bit reg value */
    uint16  mask;
    int     shifter;
    int 	lane_ctrl;
	int 	is_40nm;

    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_tx_driver_field_get(pc, type,&lane_ctrl,&mask,&shifter));
		
	is_40nm = XGXS16G_13G_ID(pc);

    /* Read preemphasis/driver/pre-driver current */
	switch (lane_ctrl) {
		case 0:
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					READ_XGXS13G_040_TX0_ACONTROL1r(unit, pc, &data16) :
					READ_XGXS16G_TX0_TX_DRIVERr(unit, pc, &data16));
			break;
		case 1:
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					READ_XGXS13G_040_TX1_ACONTROL1r(unit, pc, &data16) :
					READ_XGXS16G_TX1_TX_DRIVERr(unit, pc, &data16));
			break;
		case 2:
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					READ_XGXS13G_040_TX2_ACONTROL1r(unit, pc, &data16) :
					READ_XGXS16G_TX2_TX_DRIVERr(unit, pc, &data16));
			break;
		case 3:
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					READ_XGXS13G_040_TX3_ACONTROL1r(unit, pc, &data16) :
					READ_XGXS16G_TX3_TX_DRIVERr(unit, pc, &data16));
			break;
		case TX_DRIVER_DFT_LN_CTRL:
			SOC_IF_ERROR_RETURN
				(is_40nm ?
					READ_XGXS13G_040_TX_ALL_ACONTROL1r(unit, pc, &data16) :
					READ_XGXS16G_TX_ALL_TX_DRIVERr(unit, pc, &data16));
			break;
		default:
			return SOC_E_NONE;
	}	

    *value = (data16 & mask) >> shifter;

    return (SOC_E_NONE);
}

/* Make use of the SERDESDIGITAL_PATGENCTRL.PKT_SIZE[0] as a scratch pad for VCO status */
STATIC int
_phy_xgxs16g_control_vco_disturbed_get(int unit, soc_port_t port, uint32 *value)
{
    uint16 data;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    if(!pc) {
        return SOC_E_NONE;
    }
    SOC_IF_ERROR_RETURN(READ_XGXS16G_SERDESDIGITAL_PATGENCTRLr(unit, pc, &data));
    SOC_IF_ERROR_RETURN(MODIFY_XGXS16G_SERDESDIGITAL_PATGENCTRLr(unit, pc, 0,  SERDESDIGITAL_PATGENCTRL_PKT_SIZE_MASK));

    *value = (data & SERDESDIGITAL_PATGENCTRL_PKT_SIZE_MASK) ? 1 : 0;

    return SOC_E_NONE;
}

STATIC int
_phy_xgxs16g_control_vco_disturbed_set(int unit, soc_port_t port)
{
    uint16 data, mask;
    phy_ctrl_t *pc = INT_PHY_SW_STATE(unit, port);
    if(!pc) {
        return SOC_E_NONE;
    }
    data = 1 << SERDESDIGITAL_PATGENCTRL_PKT_SIZE_SHIFT;
    mask = SERDESDIGITAL_PATGENCTRL_PKT_SIZE_MASK;

    SOC_IF_ERROR_RETURN(MODIFY_XGXS16G_SERDESDIGITAL_PATGENCTRLr(unit, pc, data, mask));

    return SOC_E_NONE;
}

/*
 * Function:
 *      phy_xgxs16g_control_get
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
phy_xgxs16g_control_get(int unit, soc_port_t port,
                     soc_phy_control_t type, uint32 *value)
{
    int rv;
    phy_ctrl_t *pc;
    uint16 data16;
    /* coverity[mixed_enums] */
    if ((type < 0) || (type >= SOC_PHY_CONTROL_COUNT)) {
        return (SOC_E_PARAM);
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
    case SOC_PHY_CONTROL_DRIVER_CURRENT:
        /* fall through */
    case SOC_PHY_CONTROL_PRE_DRIVER_CURRENT:
        rv = _phy_xgxs16g_control_tx_driver_get(unit, pc, type, value);
        break;
    case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        rv = _phy_xgxs16g_control_prbs_polynomial_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        rv = _phy_xgxs16g_control_prbs_tx_invert_data_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        rv = _phy_xgxs16g_control_prbs_tx_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        rv = _phy_xgxs16g_control_prbs_rx_enable_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PRBS_RX_STATUS:
        rv = _phy_xgxs16g_control_prbs_rx_status_get(unit, port, value);
        break;

    case SOC_PHY_CONTROL_EQUALIZER_BOOST:
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_RX_ALL_RX_ANALOGBIAS0r(unit, pc, &data16));
        *value = data16 & RX_ALL_RX_EQ_BOOST_EQUALIZER_CONTROL_MASK;
        rv = SOC_E_NONE;
        break;
   case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        rv = _phy_xgxs16g_control_linkdown_transmit_get(unit, port, value);
        break;
    case SOC_PHY_CONTROL_PARALLEL_DETECTION:
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_SERDESDIGITAL_CONTROL1000X2r(unit, pc,&data16));
        *value =
           data16 & SERDESDIGITAL_CONTROL1000X2_ENABLE_PARALLEL_DETECTION_MASK?
             TRUE: FALSE;
        rv = SOC_E_NONE;
        break;
    case SOC_PHY_CONTROL_VCO_DISTURBED:
         rv = _phy_xgxs16g_control_vco_disturbed_get(unit, port, value);
         break;
    default:
        rv = (SOC_E_UNAVAIL);
        break;
    }
    return rv;
}

/*
 * Function:
 *      phy_xgxs16g_reg_read
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
phy_xgxs16g_reg_read(int unit, soc_port_t port, uint32 flags,
                  uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_read(unit, pc, phy_reg_addr, &data));

   *phy_data = data;

    return (SOC_E_NONE);
}

/*
 * Function:
 *      phy_xgxs16g_reg_write
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
phy_xgxs16g_reg_write(int unit, soc_port_t port, uint32 flags,
                   uint32 phy_reg_addr, uint32 phy_data)
{
    uint16               data;     /* Temporary holder for phy_data */
    phy_ctrl_t          *pc;      /* PHY software state */

    pc = INT_PHY_SW_STATE(unit, port);

    data      = (uint16) (phy_data & 0x0000FFFF);

    SOC_IF_ERROR_RETURN
        (phy_reg_aer_write(unit, pc, phy_reg_addr, data));

    return (SOC_E_NONE);
}  

/*
 * Function:
 *      phy_xgxs16g_reg_modify
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
phy_xgxs16g_reg_modify(int unit, soc_port_t port, uint32 flags,
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

    return (SOC_E_NONE);
}

STATIC int
phy_xgxs16g_probe(int unit, phy_ctrl_t *pc)
{
    uint16      serdes_id0;
    uint16      serdes_id2;
    soc_info_t *si;
    int phy_port;  /* physical port number */

    si = &SOC_INFO(unit);
    if (soc_feature(unit, soc_feature_logical_port_num)) {
        phy_port = si->port_l2p_mapping[pc->port];
    } else {
        phy_port = pc->port;
    }

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID0r(unit, pc, &serdes_id0));
    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESID_SERDESID2r(unit, pc, &serdes_id2));

    if ((serdes_id0 & SERDESID_SERDESID0_MODEL_NUMBER_MASK) !=
        SERDES_ID0_MODEL_NUMBER_XGXS_16G) {
        return SOC_E_NOT_FOUND;
    }

    if (serdes_id2 & SERDESID_SERDESID2_DR_16G_4L_MASK) {
        /* XGXS16G */
        pc->size = sizeof(XGXS16G_DEV_DESC);
        return SOC_E_NONE;
    }

    if (serdes_id2 & SERDESID_SERDESID2_DR_13G_4L_MASK) {
        /* XGXS13G new 40nm serdes */
        pc->lane_num = SOC_PORT_BINDEX(unit, phy_port);
        pc->chip_num = SOC_BLOCK_NUMBER(unit, SOC_PORT_BLOCK(unit, phy_port));
        if (pc->lane_num) {
            LOG_WARN(BSL_LS_SOC_PHY,
                     (BSL_META_U(unit,
                                 "_phy_xgxs16g_probe u=%d p=%d: wrong lane# 0x%x, set to 0\n"),
                      unit, pc->port, pc->lane_num));
            pc->lane_num = 0;
        }
    
        pc->dev_name = "XGXS13G";
        
        pc->size = sizeof(XGXS16G_DEV_DESC);
        return SOC_E_NONE;
    }
    return SOC_E_NOT_FOUND;
}

STATIC int
phy_xgxs16g_notify(int unit, soc_port_t port,
                       soc_phy_event_t event, uint32 value)
{
    int             rv;

    if (event >= phyEventCount) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch(event) {
    case phyEventInterface:
        rv = (_phy_xgxs16g_notify_interface(unit, port, value));
        break;
    case phyEventDuplex:
        rv = (_phy_xgxs16g_notify_duplex(unit, port, value));
        break;
    case phyEventSpeed:
        rv = (_phy_xgxs16g_notify_speed(unit, port, value));
        break;
    case phyEventStop:
        rv = (_phy_xgxs16g_notify_stop(unit, port, value));
        break;
    case phyEventResume:
        rv = (_phy_xgxs16g_notify_resume(unit, port, value));
        break;
    case phyEventAutoneg:
        rv = (phy_xgxs16g_an_set(unit, port, value));
        break;
    case phyEventMacLoopback:
        rv = (_phy_xgxs16g_notify_mac_loopback(unit, port, value));
        break;
    default:
        rv = SOC_E_UNAVAIL;
        break;
    }

    return rv;
}

STATIC int
phy_xgxs16g_duplex_get(int unit, soc_port_t port, int *duplex)
{
    uint16       reg0_16;
    uint16       mii_ctrl;
    phy_ctrl_t  *pc;

    pc = INT_PHY_SW_STATE(unit, port);

    *duplex = TRUE;

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_GP_STATUS_STATUS1000X1r(unit, pc, &reg0_16));

    if (reg0_16 & GP_STATUS_STATUS1000X1_SGMII_MODE_MASK) {

    /* retrieve the duplex setting in SGMII mode */
        SOC_IF_ERROR_RETURN
            (READ_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, &mii_ctrl));

        if (mii_ctrl & MII_CTRL_AE) {
            SOC_IF_ERROR_RETURN
                (READ_XGXS16G_COMBO_IEEE0_AUTONEGLPABILr(unit,pc,&reg0_16));

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
 * Variable:
 *      phy_xgxs16g_drv
 * Purpose:
 *      Phy Driver for 10G (XAUI x 4) Serdes PHY. 
 */

phy_driver_t phy_xgxs16g_hg = {
    /* .drv_name                      = */ "XGXS16G Unicore PHY Driver",
    /* .pd_init                       = */ phy_xgxs16g_init,
    /* .pd_reset                      = */ phy_null_reset,
    /* .pd_link_get                   = */ phy_xgxs16g_link_get,
    /* .pd_enable_set                 = */ phy_xgxs16g_enable_set,
    /* .pd_enable_get                 = */ phy_null_enable_get,
    /* .pd_duplex_set                 = */ phy_null_set,
    /* .pd_duplex_get                 = */ phy_xgxs16g_duplex_get,
    /* .pd_speed_set                  = */ phy_xgxs16g_speed_set,
    /* .pd_speed_get                  = */ phy_xgxs16g_speed_get,
    /* .pd_master_set                 = */ phy_null_set,
    /* .pd_master_get                 = */ phy_xgxs16g_master_get,
    /* .pd_an_set                     = */ phy_xgxs16g_an_set,
    /* .pd_an_get                     = */ phy_xgxs16g_an_get,
    /* .pd_adv_local_set              = */ NULL, /* Deprecated */
    /* .pd_adv_local_get              = */ NULL, /* Deprecated */
    /* .pd_adv_remote_get             = */ NULL, /* Deprecated */ 
    /* .pd_lb_set                     = */ phy_xgxs16g_lb_set,
    /* .pd_lb_get                     = */ phy_xgxs16g_lb_get,
    /* .pd_interface_set              = */ phy_null_interface_set,
    /* .pd_interface_get              = */ phy_xgxs16g_interface_get,
    /* .pd_ability                    = */ NULL, /* Deprecated */ 
    /* .pd_linkup_evt                 = */ NULL,
    /* .pd_linkdn_evt                 = */ NULL,
    /* .pd_mdix_set                   = */ phy_null_mdix_set,
    /* .pd_mdix_get                   = */ phy_null_mdix_get,
    /* .pd_mdix_status_get            = */ phy_null_mdix_status_get,
    /* .pd_medium_config_set          = */ NULL,
    /* .pd_medium_config_get          = */ NULL,
    /* .pd_medium_get                 = */ phy_null_medium_get,
    /* .pd_cable_diag                 = */ NULL,
    /* .pd_link_change                = */ NULL,
    /* .pd_control_set                = */ phy_xgxs16g_control_set,
    /* .pd_control_get                = */ phy_xgxs16g_control_get,
    /* .pd_reg_read                   = */ phy_xgxs16g_reg_read,
    /* .pd_reg_write                  = */ phy_xgxs16g_reg_write,
    /* .pd_reg_modify                 = */ phy_xgxs16g_reg_modify,
    /* .pd_notify                     = */ phy_xgxs16g_notify,
    /* .pd_probe                      = */ phy_xgxs16g_probe,
    /* .pd_ability_advert_set         = */ phy_xgxs16g_ability_advert_set, 
    /* .pd_ability_advert_get         = */ phy_xgxs16g_ability_advert_get,
    /* .pd_ability_remote_get         = */ phy_xgxs16g_ability_remote_get,
    /* .pd_ability_local_get          = */ phy_xgxs16g_ability_local_get
};


/***********************************************************************
 *
 * PASS-THROUGH NOTIFY ROUTINES
 */

/*
 * Function:
 *      _phy_xgxs16g_notify_duplex
 * Purpose:
 *      Program duplex if (and only if) serdesxgxs16g is an intermediate PHY.
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
_phy_xgxs16g_notify_duplex(int unit, soc_port_t port, uint32 duplex)
{
    int                 fiber;
    uint16              mii_ctrl;
    phy_ctrl_t  *pc;

    fiber = PHY_FIBER_MODE(unit, port);
    pc    = INT_PHY_SW_STATE(unit, port);
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_notify_duplex: "
                         "u=%d p=%d duplex=%d fiber=%d\n"),
              unit, port, duplex, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    if (fiber) {
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc,
                                          MII_CTRL_FD, MII_CTRL_FD));
        return SOC_E_NONE;
    }

    /* Put SERDES PHY in reset */

    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_notify_stop(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Update duplexity */
    mii_ctrl = (duplex) ? MII_CTRL_FD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, mii_ctrl, MII_CTRL_FD));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_notify_resume(unit, port, PHY_STOP_DUPLEX_CHG));

    /* Autonegotiation must be turned off to talk to external PHY if
     * SGMII autoneg is not enabled.
     */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_xgxs16g_an_set(unit, port, FALSE));
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_xgxs16g_notify_speed
 * Purpose:
 *      Program duplex if (and only if) serdesxgxs16g is an intermediate PHY.
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
_phy_xgxs16g_notify_speed(int unit, soc_port_t port, uint32 speed)
{
    int          fiber;
    uint16       fiber_status;
    phy_ctrl_t  *pc;

    pc    = INT_PHY_SW_STATE(unit, port);
    fiber = PHY_FIBER_MODE(unit, port);

    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d\n"),
              unit, port, speed, fiber));

    if (SAL_BOOT_SIMULATION) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN
        (READ_XGXS16G_SERDESDIGITAL_STATUS1000X1r(unit, pc, &fiber_status));

    /* Put SERDES PHY in reset */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_notify_stop(unit, port, PHY_STOP_SPEED_CHG));

    /* Update speed */
    SOC_IF_ERROR_RETURN
        (phy_xgxs16g_speed_set(unit, port, speed));

    /* Take SERDES PHY out of reset */
    SOC_IF_ERROR_RETURN
        (_phy_xgxs16g_notify_resume(unit, port, PHY_STOP_SPEED_CHG));

    /* Autonegotiation must be turned off to talk to external PHY */
    if (!PHY_SGMII_AUTONEG_MODE(unit, port) && PHY_EXTERNAL_MODE(unit, port)) {
        SOC_IF_ERROR_RETURN
            (phy_xgxs16g_an_set(unit, port, FALSE));
    }
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_notify_speed: "
                         "u=%d p=%d speed=%d fiber=%d rv=%d\n"),
              unit, port, speed, fiber,SOC_E_NONE));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_xgxs16g_stop
 * Purpose:
 *      Put serdesxgxs16g SERDES in or out of reset depending on conditions
 */

STATIC int
_phy_xgxs16g_stop(int unit, soc_port_t port)
{
    uint16              mii_ctrl;
    int                 stop, copper;
    phy_ctrl_t  *pc;

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
                         "phy_xgxs16g_stop: u=%d p=%d copper=%d stop=%d flg=0x%x\n"),
              unit, port, copper, stop,
              pc->stop));

    mii_ctrl = (stop) ? MII_CTRL_PD : 0;
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_COMBO_IEEE0_MIICNTLr(unit, pc, mii_ctrl, MII_CTRL_PD));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_xgxs16g_notify_stop
 * Purpose:
 *      Add a reason to put serdesxgxs16g PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_xgxs16g_notify_stop(int unit, soc_port_t port, uint32 flags)
{
    INT_PHY_SW_STATE(unit, port)->stop |= flags;

    return SOC_E_NONE;
/*    return _phy_xgxs16g_stop(unit, port); */
}

/*  
 * Function:
 *      _phy_xgxs16g_notify_resume
 * Purpose:
 *      Remove a reason to put serdesxgxs16g PHY in reset.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      flags - Reason to stop
 * Returns:     
 *      SOC_E_XXX
 */

STATIC int
_phy_xgxs16g_notify_resume(int unit, soc_port_t port, uint32 flags)
{   
    INT_PHY_SW_STATE(unit, port)->stop &= ~flags;
    
    return _phy_xgxs16g_stop(unit, port);
}

/*
 * Function:
 *      phy_xgxs16g_media_setup
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
_phy_xgxs16g_notify_interface(int unit, soc_port_t port, uint32 intf)
{
    phy_ctrl_t  *pc;
    uint16       data16;
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_notify_interface: u=%d p=%d IF=0x%x\n"),
              unit, port, intf));


    pc = INT_PHY_SW_STATE(unit, port);

    data16 = 0;
    if (intf != SOC_PORT_IF_SGMII) {
        data16 = SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK;
    }
 
    SOC_IF_ERROR_RETURN
        (MODIFY_XGXS16G_SERDESDIGITAL_CONTROL1000X1r(unit, pc, data16,
                           SERDESDIGITAL_CONTROL1000X1_FIBER_MODE_1000X_MASK));
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      _phy_xgxs16g_notify_mac_loopback
 * Purpose:
 *      turn on rx clock compensation in mac loopback mode for
 *      applicable XGXS devices
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - StrataSwitch port #.
 *      enable - TRUE mac loopback mode, FALSE not in mac loopback mode 
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_phy_xgxs16g_notify_mac_loopback(int unit, soc_port_t port, uint32 enable)
{
    phy_ctrl_t  *pc;
    
    LOG_INFO(BSL_LS_SOC_PHY,
             (BSL_META_U(unit,
                         "_phy_xgxs16g_notify_mac_loopback: u=%d p=%d enable=0x%x\n"),
              unit, port, enable));

    pc = INT_PHY_SW_STATE(unit, port);

    /* XMAC loopback mode needs to enable clock comp */
    if (XGXS16G_13G_ID(pc)) {
        SOC_IF_ERROR_RETURN
            (MODIFY_XGXS16G_XGXSBLK2_UNICOREMODE10Gr(unit, pc,
                 enable? 0x80: 0x91,
                 XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GHIG_MASK |
                 XGXSBLK2_UNICOREMODE10G_UNICOREMODE10GCX4_MASK));
    }
    return SOC_E_NONE;
}

#ifdef BROADCOM_DEBUG
void
phy_xgxs16g_state_dump(int unit, soc_port_t port)
{

    /* Show PHY configuration summary */
    /* lane status */
    

    /* Show Counters */
    /* Digital.CrcErr_RxPkt_A */
    /* ieee0Blk_A.MIIStat_A */

    /* Dump Registers */
}
#endif /* BROADCOM_DEBUG */

#else
typedef int _phy_xgxs16g_not_empty; /* Make ISO compilers happy. */
#endif /*  INCLUDE_XGXS_16G */

