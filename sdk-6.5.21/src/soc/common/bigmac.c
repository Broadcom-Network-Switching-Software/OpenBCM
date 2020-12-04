/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XGS 10Gigabit Media Access Controller Driver (bigmac)
 * This file is renamed from xmac.c.
 *
 * This module is used for:
 *
 *   - All ports on XGS Hercules family (10Gig x 8)
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>

#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/portmode.h>
#include <soc/ll.h>
#include <soc/counter.h>
#include <soc/macutil.h>
#include <soc/phyctrl.h>

#ifdef BCM_BIGMAC_SUPPORT

/*
 * BigMAC Register field definitions.
 */

/* Transmit CRC Modes (Receive has bit field definitions in register) */
#define BIGMAC_CRC_APPEND          0x00   /* Append CRC (Default) */
#define BIGMAC_CRC_KEEP            0x01   /* CRC Assumed correct */
#define BIGMAC_CRC_REGEN           0x02   /* Replace CRC with a new one */
#define BIGMAC_CRC_RSVP            0x03   /* Reserved (does Replace) */

#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

/* speed to SOC_PA_SPEED flags mapping table, need to be in ascending order */
struct {
    int speed;
    int pa_flag;
} mac_big_hg_speed_list[] = {
    { 10000, SOC_PA_SPEED_10GB },
    { 12000, SOC_PA_SPEED_12GB },
    { 13000, SOC_PA_SPEED_13GB },
    { 16000, SOC_PA_SPEED_16GB },
    { 20000, SOC_PA_SPEED_20GB },
    { 21000, SOC_PA_SPEED_21GB },
    { 24000, SOC_PA_SPEED_24GB },
    { 25000, SOC_PA_SPEED_25GB },
};

#ifdef BROADCOM_DEBUG
static char *mac_big_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *mac_big_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
/*
 * Returns the field in CMIC_XGXS_PLL_CONTROL_1 corresponding to
 * a Higig port on XGS3 device
 */
static int
_port2pllf(int unit, soc_port_t port)
{
    if (SOC_IS_FB_FX_HX(unit)) {
        switch(port) {
            case 24: return PLL_CONTROL_13f;
            case 25: return PLL_CONTROL_12f;
            case 26: return PLL_CONTROL_11f;
            case 27: return PLL_CONTROL_10f;
            default: return INVALIDf;
        }
    }

    return INVALIDf;
}

#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
        defined(BCM_FIREBOLT2_SUPPORT)
/*
 *      Check if specified speed is supported.
 */
static int
_mac_big_speed_support_check(int unit, soc_port_t port, int speed)
{
    if (IS_LMD_ENABLED_PORT(unit, port)) {
        if ((speed != 2500) && (speed != 3000) && (speed != 0)) {
            return SOC_E_PARAM;
        }
    } else {
        if ((speed != 10000) && (speed != 12000) && (speed != 0)) {
            return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT || \
          BCM_FIREBOLT2_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */

void
_mac_big_speed_to_pa_flag(int unit, soc_port_t port, int speed,
                          uint32 *pa_flag)
{
    *pa_flag = 0;
    if (soc_feature(unit, soc_feature_xgxs_lcpll)) {
        if (IS_LMD_ENABLED_PORT(unit, port)) {
            if (speed == 2500) {
                *pa_flag = SOC_PA_SPEED_2500MB;
            } else if (speed == 3000) {
                *pa_flag = SOC_PA_SPEED_3000MB;
            }
        } else {
            if (speed == 10000) {
                *pa_flag = SOC_PA_SPEED_10GB;
            } else if (speed == 12000) {
                *pa_flag = SOC_PA_SPEED_12GB;
            }
        }
    } else {
        *pa_flag = SOC_PA_SPEED(speed); 
    }
}

/*
 * Function:
 *      _mac_big_drain_cells
 * Purpose:
 *      Wait until MMU cell count reaches zero for a specified port.
 * Parameters:
 *      unit - StrataSwitch unit #.
 *      port - Port number being examined.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Keeps polling as long as LCCCOUNT/GCCCOUNT is decrementing.
 *      If LCCCOUNT/GCCCOUNT stops decrementing without reaching zero,
 *      after a timeout period a warning message is printed
 *      and it gives up.
 *
 *      Before calling this routine, make sure no packets are switching
 *      to the port or it might never finish draining.
 */

STATIC int
_mac_big_drain_cells(int unit, soc_port_t port)
{
    int                 rv;
    uint64              tx_ctrl;
    int                 pause_tx, pause_rx;
#if defined(BCM_HURRICANE1_SUPPORT)
    int                 pfc_en_rx;
#endif

    rv  = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return rv;
    }

    /* Disable pause function */
    SOC_IF_ERROR_RETURN
        (soc_mac_big.md_pause_get(unit, port, &pause_tx, &pause_rx));
#if defined(BCM_HURRICANE1_SUPPORT)
    if (SOC_IS_HURRICANE(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mac_big.md_control_get(unit, port,
                SOC_MAC_CONTROL_PFC_RX_ENABLE, &pfc_en_rx));
    }
#endif

    SOC_IF_ERROR_RETURN(soc_mac_big.md_pause_set(unit, port, 0, 0));
#if defined(BCM_HURRICANE1_SUPPORT)
    if (SOC_IS_HURRICANE(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mac_big.md_control_set(unit, port,
                SOC_MAC_CONTROL_PFC_RX_ENABLE, 0));
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d bigmac saved pause and pfc state\n"), port));

    /* Notify PHY driver */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, 
                            phyEventStop, PHY_STOP_DRAIN));

    /* First put the port in flush state */
    SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, TRUE));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d bigmac mmu flush enable completed\n"), port));
    /* Disable switch egress metering so that packet draining is not rate
     * limited.
     */
    rv = soc_egress_drain_cells(unit, port, 250000);
    if (SOC_E_NONE == rv) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                           "port %d bigmac egress packet draining completed\n"),
                           port));
    }

    /* Notify PHY driver */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventResume, PHY_STOP_DRAIN));
  
    /* Toggle the Tx Fifo reset to erase any packets left in the
     * BigMAC buffer. */
    if (soc_reg_field_valid(unit, MAC_TXCTRLr, TXFIFO_RESETf)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_TXCTRLr, port,
                                    TXFIFO_RESETf, 1)); 
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_TXCTRLr, port,
                                    TXFIFO_RESETf, 0));
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d bigmac TX fifo reset completed\n"), port));
    /* Restore original pause configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_big.md_pause_set(unit, port, pause_tx, pause_rx));
#if defined(BCM_HURRICANE1_SUPPORT)
    if (SOC_IS_HURRICANE(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mac_big.md_control_set(unit, port,
                SOC_MAC_CONTROL_PFC_RX_ENABLE, pfc_en_rx));
    }
#endif
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d bigmac restored pause and pfc state\n"), port));
    /* Bring the switch MMU out of flush */
    SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "port %d bigmac mmu flush disabled\n"), port));
    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx_ctrl));

    return rv;
}

/*
 * Function:
 *      mac_big_enable_set
 * Purpose:
 *      Enable or disable MAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - TRUE to enable, FALSE to disable
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 ctrl, octrl;
    pbmp_t              mask;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_enable_set: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 enable ? "True" : "False"));

    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, TXENf, 1);
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, RXENf, enable ? 1 : 0);
    if (COMPILER_64_NE(ctrl, octrl)) {
#if defined(BCM_SCORPION_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
        if (enable && soc_feature(unit, soc_feature_priority_flow_control)) {
            /* Flush MMU XOFF state with toggle bit before turning on RX */
            if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                            FORCE_PFC_XONf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                            FORCE_PFC_XONf, 0));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                            FORCE_PP_XONf, 1));
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                            FORCE_PP_XONf, 0));
            }
        }
#endif /* BCM_SCORPION_SUPPORT || BCM_TRIUMPH2_SUPPORT */

        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, ctrl));
    }

    if (enable) {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    } else {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(_mac_big_drain_cells(unit, port));
    }

    return SOC_E_NONE;
}

/* This routine should go into xgxs1.c:phy_xgxs1_init and be invoked by
 * calling soc_phyctrl_init */
int
_fusioncore_phy_xgxs1_init(int unit, soc_port_t port)
{
    uint8               phy_addr;
    uint64              xgxs_stat;
    int                 preemph, idriver, pdriver;
    int                 plllock, phyrev;
    int                 pll_lock_usec;
    int                 locked;
    soc_timeout_t       to;

    if (SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit)){
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_xgxs_v1)) {
        phyrev = 1;
    } else if (soc_feature(unit, soc_feature_xgxs_v2)) {
        phyrev = 2;
    } else if (soc_feature(unit, soc_feature_xgxs_v3)) {
        phyrev = 3;
    } else if (soc_feature(unit, soc_feature_xgxs_v4)) {
        phyrev = 4;
    } else {
        return SOC_E_NONE;
    }

    /*
     * Allow properties to change important variables.
     */

    plllock = soc_property_port_get(unit, port, spn_XGXS_PLLLOCK, 0x7);
    /* NB: preemphasis, driver, pre-driver currents are bit-reversed in HW */
    preemph = soc_property_port_get(unit, port, spn_XGXS_PREEMPHASIS,
                                    (phyrev > 2) ? 0 : 0x8);
    preemph = _shr_bit_rev8(preemph) >> 4;

    idriver = soc_property_port_get(unit, port,
                                    spn_XGXS_DRIVER_CURRENT, 0xb);
    idriver = _shr_bit_rev8(idriver) >> 4;

    pdriver = soc_property_port_get(unit, port,
                                    spn_XGXS_PRE_DRIVER_CURRENT, 0xb);
    pdriver = _shr_bit_rev8(pdriver) >> 4;

    phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);
    pll_lock_usec = 500000;

    if (phyrev == 1) {
        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* PLL range select bypass */
            (soc_miim_write(unit, phy_addr, 0x11, 0x500e));

        SOC_IF_ERROR_RETURN             /* PLL lock range */
            (soc_miim_write(unit, phy_addr, 0x15, 0x2e00 | (plllock & 0xf)));
        SOC_IF_ERROR_RETURN             /* PLL clock divider bias */
            (soc_miim_write(unit, phy_addr, 0x1b, 0x0001));
        SOC_IF_ERROR_RETURN             /* ? */
            (soc_miim_write(unit, phy_addr, 0x1c, 0x0019));
        SOC_IF_ERROR_RETURN             /* PLL bias */
            (soc_miim_write(unit, phy_addr, 0x1d, 0x1005));

        SOC_IF_ERROR_RETURN             /* Select block 0xa */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
        SOC_IF_ERROR_RETURN             /* TX pre-emphasis */
            (soc_miim_write(unit, phy_addr, 0x17, ((preemph & 0xf) << 12)
                            | ((idriver & 0xf) << 8) | ((pdriver & 0xf) << 4)));
        /* Retain this for Herc A0, but not later Phys */
        SOC_IF_ERROR_RETURN             /* Select block 0xf */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));
        SOC_IF_ERROR_RETURN             /* RX loop bandwidth */
            (soc_miim_write(unit, phy_addr, 0x16, 0x7300));
        SOC_IF_ERROR_RETURN             /* RX integration mode */
            (soc_miim_write(unit, phy_addr, 0x19, 0x0110));

        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* Turn on PLL state machine */
            (soc_miim_write(unit, phy_addr, 0x11, 0xd00e));

        SOC_IF_ERROR_RETURN             /* Select block 0x0 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
    } else if (phyrev == 2) {
        uint16 plldiv = (SOC_IS_HERCULES15(unit) ? 0x0000 : 0x6c33);

        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* PLL range select bypass */
            (soc_miim_write(unit, phy_addr, 0x11, 0x500e));

        SOC_IF_ERROR_RETURN             /* Increase PLL clock divider bias */
            (soc_miim_write(unit, phy_addr, 0x1a, plldiv));
        SOC_IF_ERROR_RETURN             /* Decrease VCO bias */
            (soc_miim_write(unit, phy_addr, 0x1d, 0x1028));

        SOC_IF_ERROR_RETURN             /* Select block 0xa */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
        SOC_IF_ERROR_RETURN             /* TX pre-emphasis */
            (soc_miim_write(unit, phy_addr, 0x17, ((preemph & 0xf) << 12)
                            | ((idriver & 0xf) << 8) | ((pdriver & 0xf) << 4)));
        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* Turn on PLL state machine */
            (soc_miim_write(unit, phy_addr, 0x11, 0xd00e));

        SOC_IF_ERROR_RETURN             /* Select block 0x0 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
    } else if (phyrev == 3) {
        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* PLL range select bypass */
            (soc_miim_write(unit, phy_addr, 0x11, 0x500e));
        SOC_IF_ERROR_RETURN             /* Enable PLL tuning */
            (soc_miim_write(unit, phy_addr, 0x1c, 0x0019));

        SOC_IF_ERROR_RETURN             /* Select block 0xf */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));
        SOC_IF_ERROR_RETURN             /* Increase RX loop bandwidth */
            (soc_miim_write(unit, phy_addr, 0x16, 0x7300));
        SOC_IF_ERROR_RETURN             /* Clear RX integration mode */
            (soc_miim_write(unit, phy_addr, 0x19, 0x0900));

        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* Turn on PLL state machine */
            (soc_miim_write(unit, phy_addr, 0x11, 0xd00e));

        SOC_IF_ERROR_RETURN             /* Select block 0xa */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
        SOC_IF_ERROR_RETURN             /* TX pre-emphasis */
            (soc_miim_write(unit, phy_addr, 0x17, ((preemph & 0xf) << 12)
                            | ((idriver & 0xf) << 8) | ((pdriver & 0xf) << 4)));

        SOC_IF_ERROR_RETURN             /* Select block 0x0 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
        SOC_IF_ERROR_RETURN             /* Set TX clk to 1/2 speed, afifo on */
            (soc_miim_write(unit, phy_addr, 0x10, 0x212f));
        if (soc_property_port_get(
            unit, port, spn_PHY_XAUI_RX_LANE_SWAP, FALSE)) {
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x18, 0xa050));
        }
    } else if (phyrev == 4) {
        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* PLL range select bypass */
            (soc_miim_write(unit, phy_addr, 0x11, 0x500e));
        SOC_IF_ERROR_RETURN             /* Enable PLL tuning */
            (soc_miim_write(unit, phy_addr, 0x1c, 0x0019));

        SOC_IF_ERROR_RETURN             /* Select block 0xf */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00f0));
        SOC_IF_ERROR_RETURN             /* Set RX integration mode */
            (soc_miim_write(unit, phy_addr, 0x19, 0x0910));
        SOC_IF_ERROR_RETURN             /* Set sigdet offset control */
            (soc_miim_write(unit, phy_addr, 0x1b, 0x0008));

        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* Turn on PLL state machine */
            (soc_miim_write(unit, phy_addr, 0x11, 0xd00e));

        SOC_IF_ERROR_RETURN             /* Select block 0xa */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
        SOC_IF_ERROR_RETURN             /* TX pre-emphasis */
            (soc_miim_write(unit, phy_addr, 0x17, ((preemph & 0xf) << 12)
                            | ((idriver & 0xf) << 8) | ((pdriver & 0xf) << 4)));

        SOC_IF_ERROR_RETURN             /* Select block 0x0 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
        SOC_IF_ERROR_RETURN             /* Set TX clk to 1/2 speed, afifo on */
            (soc_miim_write(unit, phy_addr, 0x10, 0x212f));
        if (soc_property_port_get(
            unit, port, spn_PHY_XAUI_RX_LANE_SWAP, FALSE)) {
            SOC_IF_ERROR_RETURN(soc_miim_write(unit, phy_addr, 0x18, 0xa050));
        }
    }

    if (SAL_BOOT_QUICKTURN) {
        LOG_VERBOSE(BSL_LS_SOC_10G,
                    (BSL_META_U(unit,
                                "SIMULATION: _fusioncore_phy_xgxs1_init: unit %d "
                                 "port %s: skipped waiting for pll lock\n"),
                     unit, SOC_PORT_NAME(unit, port)));
        return SOC_E_NONE;
    }

    /* Wait up to 0.1 sec for TX PLL lock. */
    soc_timeout_init(&to, pll_lock_usec, 0);

    locked = 0;
    while (!soc_timeout_check(&to)) {
        SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &xgxs_stat));
        locked = soc_reg64_field32_get(unit, MAC_XGXS_STATr, xgxs_stat,
                                       TXPLL_LOCKf);
        if (locked) {
            break;
        }
    }

    if (!locked) {
        uint16  x_pll_stat;

        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0050));
        SOC_IF_ERROR_RETURN             /* Read PLL status */
            (soc_miim_read(unit, phy_addr, 0x10, &x_pll_stat));
        SOC_IF_ERROR_RETURN             /* Select block 0x5 */
            (soc_miim_write(unit, phy_addr, 0x1f, 0x0000));
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "unit %d port %s: FusionCore PLL did not lock PLL_STAT %04x\n"),
                   unit, SOC_PORT_NAME(unit, port), x_pll_stat));
    } else {
        LOG_VERBOSE(BSL_LS_SOC_10G,
                    (BSL_META_U(unit,
                                "_fusioncore_phy_xgxs1_init: unit %d port %s "
                                 "FusionCore PLL locked in %d usec\n"),
                     unit, SOC_PORT_NAME(unit, port),
                     soc_timeout_elapsed(&to)));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_fusioncore_reset (10(X)-Gig/Xaui/Serdes (XGXS) reset)
 * Purpose:
 *     Reset and initialize the BigMAC and Fusion core technology.
 * Parameters:
 *     unit - XGS unit #.
 *     port - Port number on unit.
 * Returns:
 *     SOC_E_XXX
 */
int
soc_fusioncore_reset(int unit, soc_port_t port)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "soc_fusioncore_reset: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));

    SOC_IF_ERROR_RETURN(_fusioncore_phy_xgxs1_init(unit, port));
    return SOC_E_NONE;
}

STATIC int
_mac_big_init(int unit, soc_port_t port)
{
    uint64              rx_ctrl, tx_ctrl, mac_ctrl, tmp64;
    int                 encap, ipg;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "_mac_big_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rx_ctrl));
    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx_ctrl));
    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &mac_ctrl));

    if (soc_reg_field_valid(unit, MAC_CTRLr, TXRESETf)) {
        soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, TXRESETf, 0);
        soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RXRESETf, 0);
    }

    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RXENf, 0);
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, TXENf, 0);
    if (IS_ST_PORT(unit, port)) {
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, PAUSEENf, 0);
        soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, RXPAUSENf, 0);
    } else {
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, PAUSEENf, 1);
        soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, RXPAUSENf, 1);
    }
    if (IS_XE_PORT(unit, port)) {
        ipg = SOC_PERSIST(unit)->ipg[port].fd_xe;
    } else {
        ipg = SOC_PERSIST(unit)->ipg[port].fd_hg;
    }
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, AVGIPGf, ipg >> 3);

    SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, mac_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, tx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, rx_ctrl));

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int wan_mode = soc_property_port_get(unit, port, spn_PHY_WAN_MODE,
                                             FALSE);

        if (wan_mode) {
            /* Max speed for WAN mode is 9.294Gbps.
             * This setting gives 10Gbps * (13/14) or 9.286 Gbps */
            SOC_IF_ERROR_RETURN
                (soc_mac_big.md_control_set(unit, port,
                                  SOC_MAC_CONTROL_FRAME_SPACING_STRETCH, 13));
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* Set jumbo max size (8000 byte payload) */
    COMPILER_64_SET(tmp64, 0, JUMBO_MAXSZ);
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXMAXSZr(unit, port, tmp64));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXMAXSZr(unit, port, tmp64));

    /* Program MAC settings */
    /* Setup header mode, check for property for bcm5632 mode */
    if (IS_XE_PORT(unit, port)) {
        encap = SOC_ENCAP_IEEE;         /* Ethernet port, so only IEEE mode */
    } else if (soc_property_get(unit, spn_BCM5632_MODE, 0)) {
        encap = SOC_ENCAP_B5632;
    } else {
        encap = SOC_ENCAP_HIGIG;        /* Default: Enable HIGIG header mode */
    }
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, HDRMODEf, encap);
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, HDRMODEf, encap);

#if defined(BCM_BRADLEY_SUPPORT)
#ifdef BCM_HIGIG2_SUPPORT
    if (soc_feature(unit, soc_feature_higig2) && IS_HG_PORT(unit, port) &&
        soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE,
                              0))  {
	soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, HIGIG2MODEf, 1);
	soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, HIGIG2MODEf, 1);
    }
#endif /* BCM_HIGIG2_SUPPORT */
#endif

    /*
     * Power-up defaults are CRC_APPEND for TX and STRIPCRC for RX.
     * Change to CRC_KEEP for Hercules, CRC_REGEN for other XGS,
     * and turn off STRIPCRC to be compatible with other BCM56xx.
     *
     * To prevent an extra 4 bytes passing through the fabric, 
     */

    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, STRIPCRCf, 0);
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, CRC_MODEf,
                          BIGMAC_CRC_REGEN);
    if (soc_reg_field_valid(unit, MAC_TXCTRLr, TIME_STAMP_TX_ENf)) {
       soc_reg64_field32_set(unit, MAC_TXCTRLr, &tx_ctrl, TIME_STAMP_TX_ENf, 1);
    }
    if (soc_reg_field_valid(unit, MAC_RXCTRLr, TIME_STAMP_RX_ENf)) {
       soc_reg64_field32_set(unit, MAC_RXCTRLr, &rx_ctrl, TIME_STAMP_RX_ENf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, tx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, rx_ctrl));

    if (SOC_IS_XGS12_FABRIC(unit)) {
        uint64 pse, opse;

        SOC_IF_ERROR_RETURN(READ_MAC_TXPSETHRr(unit, port, &pse));
        opse = pse;
        soc_reg64_field32_set(unit, MAC_TXPSETHRr, &pse, XONf, 0xff80);
        if (COMPILER_64_NE(pse, opse)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_TXPSETHRr(unit, port, pse));
        }
    }

    /* disable loopback */
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RMTLOOPf, 0);
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, LCLLOOPf, 0);

#if defined(BCM_SCORPION_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_priority_flow_control)) {
        /* Flush MMU XOFF state with toggle bit */
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                        FORCE_PFC_XONf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                        FORCE_PFC_XONf, 0));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                        FORCE_PP_XONf, 1));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                        FORCE_PP_XONf, 0));
            }
    }
#endif /* BCM_SCORPION_SUPPORT || BCM_TRIUMPH2_SUPPORT */

    /* Finally, take MAC out of reset (program enables) */
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, RXENf, 1);
    soc_reg64_field32_set(unit, MAC_CTRLr, &mac_ctrl, TXENf, 1);

    SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, mac_ctrl));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_init
 * Purpose:
 *      Initialize Bigmac / FusionCore MAC into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_big_init(int unit, soc_port_t port)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (IS_HG_PORT(unit, port) && soc_feature(unit, soc_feature_xgxs_lcpll)) {
        uint32 val, lcpll;

        /*
         * LCPLL clock speed selection
         */
        lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL_12GBPS, 0) ?
            1 : 0,
        READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &val);
        soc_reg_field_set(unit, CMIC_XGXS_PLL_CONTROL_1r, &val,
                          _port2pllf(unit, port), lcpll);
        WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, val);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return _mac_big_init(unit, port);
}

/*
 * Function:
 *      mac_big_enable_get
 * Purpose:
 *      Get MAC enable state
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));

    *enable = soc_reg64_field32_get(unit, MAC_CTRLr, ctrl, RXENf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_enable_get: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *enable ? "True" : "False"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_duplex_set
 * Purpose:
 *      Set BigMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_big_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_duplex_get
 * Purpose:
 *      Get BigMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_pause_set
 * Purpose:
 *      Configure BigMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    uint64 tmp, otmp;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_pause_set: unit %d port %s RX=%s TX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_rx ? "on" : "off",
                 pause_tx ? "on" : "off"));

    if (pause_tx >= 0) {
        /* Transmit Pause frame function */
        SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tmp));
        otmp = tmp;
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &tmp, PAUSEENf,
                              pause_tx ? 1 : 0);
        if (COMPILER_64_NE(tmp, otmp)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, tmp));
        }
    }

    if (pause_rx >= 0) {
        /* Receive Pause frame function */
        SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &tmp));
        otmp = tmp;
        soc_reg64_field32_set(unit, MAC_RXCTRLr, &tmp, RXPAUSENf,
                              pause_rx ? 1 : 0);
        if (COMPILER_64_NE(tmp, otmp)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, tmp));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_pause_get
 * Purpose:
 *      Return the pause ability of BigMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause
 *      pause_rx - Boolean: receive pause
 *      pause_mac - MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint64 rx, tx;

    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rx));
    *pause_rx = soc_reg64_field32_get(unit, MAC_RXCTRLr, rx, RXPAUSENf);
    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &tx));
    *pause_tx = soc_reg64_field32_get(unit, MAC_TXCTRLr, tx, PAUSEENf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_pause_get: unit %d port %s RX=%s TX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *pause_rx ? "on" : "off",
                 *pause_tx ? "on" : "off"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_speed_set
 * Purpose:
 *      Set BigMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 2500, 3000, 10000, 12000 for speed.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_speed_set(int unit, soc_port_t port, int speed)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_speed_set: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed));

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (IS_HG_PORT(unit, port) &&
        soc_feature(unit, soc_feature_xgxs_lcpll)) {
        pbmp_t pbmp;
        int usec, enable, cur_speed, cur_max, cur_lb;
        uint32 flags, rval;

        SOC_IF_ERROR_RETURN
            (soc_mac_big.md_speed_get(unit, port, &cur_speed));
        if (speed != cur_speed) {
#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
        defined(BCM_FIREBOLT2_SUPPORT)
        SOC_IF_ERROR_RETURN
            (_mac_big_speed_support_check(unit, port, speed));
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */
            switch (speed) {
            case 0:
                /* Leave speed unchanged; */
                break;
#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT)
            case 2500:
            case 3000:
                speed <<= 2;    /* specified speed is div by 4 */
                /* FALLTHRU */
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */
            case 10000:
            case 12000:
                /* Save counter DMA settings and port state */
                flags = SOC_CONTROL(unit)->counter_flags;
                usec = SOC_CONTROL(unit)->counter_interval;
                SOC_PBMP_ASSIGN(pbmp, SOC_CONTROL(unit)->counter_pbmp);
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_enable_get(unit, port, &enable));
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_frame_max_get(unit, port, &cur_max));
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_lb_get(unit, port, &cur_lb));
                /* Stop counter DMA and disable MAC */
                SOC_IF_ERROR_RETURN
                    (soc_counter_stop(unit));
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_enable_set(unit, port, 0));

                READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &rval);
                soc_reg_field_set(unit, CMIC_XGXS_PLL_CONTROL_1r, &rval,
                                  _port2pllf(unit, port),
                                  speed == 12000 ? 1 : 0);
                WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, rval);

                /* Reset */
                SOC_IF_ERROR_RETURN(soc_xgxs_reset(unit, port));
                SOC_IF_ERROR_RETURN(_fusioncore_phy_xgxs1_init(unit, port));
                SOC_IF_ERROR_RETURN
                    (soc_phyctrl_init(unit, port));
                SOC_IF_ERROR_RETURN
                    (_mac_big_init(unit, port));
                /* Restore counter DMA settings and port state */
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_frame_max_set(unit, port, cur_max));
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_lb_set(unit, port, cur_lb));
                SOC_IF_ERROR_RETURN
                    (soc_mac_big.md_enable_set(unit, port, enable));
                SOC_IF_ERROR_RETURN
                    (soc_counter_start(unit, flags, usec, pbmp));
                break;
            default:
                return SOC_E_PARAM;
            }
        }

    } else {
        /* Just update IPG */
        int    ipg;
        uint64 rval64, orval64;

        if (IS_XE_PORT(unit, port)) {
            ipg = SOC_PERSIST(unit)->ipg[port].fd_xe;
        } else {
            ipg = SOC_PERSIST(unit)->ipg[port].fd_hg;
        }

        SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &rval64));
        orval64 = rval64;
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &rval64, AVGIPGf, ipg >> 3);
        if (COMPILER_64_NE(rval64, orval64)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, rval64));
        }
    }

#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_speed_get
 * Purpose:
 *      Get BigMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb (2500/3000/10000/12000)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_speed_get(int unit, soc_port_t port, int *speed)
{
    *speed = 10000;
#if defined(BCM_XGS3_SWITCH_SUPPORT)
#ifdef BCM_GXPORT_SUPPORT
    if (soc_reg_field_valid(unit, MAC_XGXS_STATr, SPEED_10000f)) {
        uint64 rval64;
        uint32 rval32;
        uint32 i;
        static struct {
            soc_field_t field;
            int speed;
        } _sp_sel[] = {
            { SPEED_16000f,     16000 },
            { SPEED_15000f,     15000 },
            { SPEED_13000f,     13000 },
            { SPEED_12500f,     12500 },
            { SPEED_12000f,     12000 },
            { SPEED_10000_CX4f, 10000 },
            { SPEED_10000f,     10000 },
            { SPEED_6000f,      6000  },
            { SPEED_5000f,      5000  },
            { SPEED_2500f,      2500  },
            { SPEED_1000f,      1000  },
            { SPEED_100f,       100   },
            { SPEED_10f,        10    }
        };

        /* Get speed from internal PHY by reading BigMAC register */
        SOC_IF_ERROR_RETURN(READ_MAC_XGXS_STATr(unit, port, &rval64));
        for (i = 0; i < COUNTOF(_sp_sel); i++) {
            if (soc_reg64_field32_get(unit, MAC_XGXS_STATr, rval64,
                                      _sp_sel[i].field)) {
                *speed = _sp_sel[i].speed;
                break;
            }
        }
        if (i == COUNTOF(_sp_sel)) {
            if (SOC_IS_SHADOW(unit)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_NEWSTATUS2_REGr(unit, port,
                                                                    &rval32));
                if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS2_REGr,
                                      rval32, XGXS0_SPEED20000f) & 1) {
                    *speed = 20000;
                } else if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS2_REGr,
                                      rval32, XGXS0_SPEED25455f) & 1) {
                    *speed = 25000;
                }
            }

            if (SOC_IS_SC_CQ(unit)) {
                SOC_IF_ERROR_RETURN(READ_XPORT_XGXS_NEWSTATUS2_REGr(unit, port,
                                                                    &rval32));
                if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS2_REGr,
                                      rval32, XGXS0_SPEED20000f) & 1) {
                    *speed = 20000;
                } else if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS2_REGr,
                                      rval32, XGXS0_SPEED21000f) & 1) {
                    *speed = 21000;
                }
            }
            if (!IS_XQ_PORT(unit, port) &&
                (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                SOC_IS_VALKYRIE2(unit))) {
                SOC_IF_ERROR_RETURN
                    (READ_XPORT_XGXS_NEWSTATUS1_REGr(unit, port, &rval32));
                if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS1_REGr,
                                      rval32, XGXS0_SPEED20000f)) {
                    *speed = 20000;
                } else if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS1_REGr,
                                             rval32, XGXS0_SPEED21000f)) {
                    *speed = 21000;
                } else if (soc_reg_field_get(unit, XPORT_XGXS_NEWSTATUS1_REGr,
                                             rval32, XGXS0_SPEED25455f)) {
                    *speed = 24000;
                }
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_10G,
                    (BSL_META_U(unit,
                                "mac_big_speed_get: unit %d port %s speed=%dMb\n"),
                     unit, SOC_PORT_NAME(unit, port),
                     *speed));
        return SOC_E_NONE;
    }
#endif /* BCM_GXPORT_SUPPORT */

    if (IS_HG_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_xgxs_lcpll)) {
            uint32 val, lcpll;
            uint64 xgxs_ctrl;

            /*
             * Check for internal/external ref clock.
             * If external clock is used then leave speed at 10G
             * even if this may be incorrect.
             */
            SOC_IF_ERROR_RETURN(READ_MAC_XGXS_CTRLr(unit, port, &xgxs_ctrl));
            if (soc_reg64_field32_get(unit, MAC_XGXS_CTRLr,
                                      xgxs_ctrl, LCREFENf) == 1) {
                /* Internal ref clock, get speed select */
                READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &val);
                lcpll = soc_reg_field_get(unit, CMIC_XGXS_PLL_CONTROL_1r,
                                          val, _port2pllf(unit, port));
                if (lcpll == 1) {
                    *speed = 12000;
                }
            }
        }
    }
#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT)
    if (soc_feature(unit, soc_feature_lmd)) {
        if (IS_LMD_ENABLED_PORT(unit, port)) {
            *speed >>= 2;   /* Div by 4 */
        }
    }
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT */
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_loopback_set
 * Purpose:
 *      Set a BigMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On bigmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_loopback_set(int unit, soc_port_t port, int lb)
{
    uint64 ctrl, octrl;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_loopback_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "local" : "no"));

    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, LCLLOOPf,
                          lb ? 1 : 0);
    if (COMPILER_64_NE(ctrl, octrl)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_loopback_get
 * Purpose:
 *      Get current BigMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint64 ctrl;
    int local, remote;

    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));

    remote = soc_reg64_field32_get(unit, MAC_CTRLr, ctrl, RMTLOOPf);
    local = soc_reg64_field32_get(unit, MAC_CTRLr, ctrl, LCLLOOPf);
    *lb = local | remote;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_loopback_get: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *lb ? (remote ? "remote" : "local") : "no"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_pause_addr_set
 * Purpose:
 *      Configure PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 r, tmp;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_pause_addr_set: unit %d port %s MAC=<"
                             "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 m[0], m[1], m[2], m[3], m[4], m[5]));

    COMPILER_64_ZERO(r);
    for (i = 0; i< 6; i++) {
        COMPILER_64_SET(tmp, 0, m[i]);
        COMPILER_64_SHL(r, 8);
        COMPILER_64_OR(r, tmp);
    }

    SOC_IF_ERROR_RETURN(WRITE_MAC_TXMACSAr(unit, port, r));
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXMACSAr(unit, port, r));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_pause_addr_get
 * Purpose:
 *      Retrieve PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 * NOTE: We always write the same thing to TX & RX SA
 *       so, we just return the contects on RXMACSA.
 */
STATIC int
mac_big_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 reg;
    uint32 msw;
    uint32 lsw;

    SOC_IF_ERROR_RETURN(READ_MAC_RXMACSAr(unit, port, &reg));
    COMPILER_64_TO_32_HI(msw, reg);
    COMPILER_64_TO_32_LO(lsw, reg);

    m[0] = (uint8) ( ( msw & 0x0000ff00 ) >> 8 );
    m[1] = (uint8) ( msw & 0x000000ff );

    m[2] = (uint8)  ( ( lsw & 0xff000000) >> 24 );
    m[3] = (uint8)  ( ( lsw & 0x00ff0000) >> 16 );
    m[4] = (uint8)  ( ( lsw & 0x0000ff00) >> 8 );
    m[5] = (uint8)  ( lsw & 0x000000ff );

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_pause_addr_get: unit %d port %s MAC=<"
                             "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 m[0], m[1], m[2], m[3], m[4], m[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_interface_set
 * Purpose:
 *      Set a BigMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_UNAVAIL - requested mode not supported.
 * Notes:
 */
STATIC int
mac_big_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_interface_set: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_big_port_if_names[pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_interface_get
 * Purpose:
 *      Retrieve BigMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_big_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_MII;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_interface_get: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_big_port_if_names[*pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_big_frame_max_set(int unit, soc_port_t port, int size)
{
    uint64 val64;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_frame_max_set: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 size));

    if (IS_XE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        size += 4;
    }
    COMPILER_64_SET(val64, 0, size);
    SOC_IF_ERROR_RETURN(WRITE_MAC_RXMAXSZr(unit, port, val64));

    if (SOC_IS_XGS12_SWITCH(unit) && IS_XE_PORT(unit, port)) {
        size += 8; /* Preamble included in Ethernet TX size */
    }
    COMPILER_64_SET(val64, 0, size);
    SOC_IF_ERROR_RETURN(WRITE_MAC_TXMAXSZr(unit, port, val64));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 */
STATIC int
mac_big_frame_max_get(int unit, soc_port_t port, int *size)
{
    int    rv;
    uint64 val64;

    rv = READ_MAC_TXMAXSZr(unit, port, &val64);
    if (rv == SOC_E_NONE) {
        COMPILER_64_TO_32_LO(*size, val64);
        if (IS_XE_PORT(unit, port)) {
            /* For VLAN tagged packets */
            *size -= 4;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_frame_max_get: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *size));
    return rv;
}

/*
 * Function:
 *      mac_big_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg - number of bits to use for average inter-frame gap
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_big_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int ifg)
{
    int         cur_speed;
    int         cur_duplex;
    int         real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    uint64      rval, orval;
    soc_port_ability_t ability;
    uint32      pa_flag;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_ifg_set: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", ifg));

    if (!duplex) {
        return SOC_E_PARAM;
    }

    _mac_big_speed_to_pa_flag(unit, port, speed, &pa_flag); 
    soc_mac_big.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 31 bytes (i.e. multiple of 8 bits) */
    real_ifg = ifg < 64 ? 64 : (ifg > 248 ? 248 : (ifg + 7) & (0x1f << 3));

    if (IS_XE_PORT(unit, port)) {
        si->fd_xe = real_ifg;
    } else {
        si->fd_hg = real_ifg;
    }

    SOC_IF_ERROR_RETURN(mac_big_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_big_speed_get(unit, port, &cur_speed));

    if (cur_speed == speed && ((soc_port_duplex_t)cur_duplex == duplex)) {
        SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &rval));
        orval = rval;
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &rval, AVGIPGf,
                              real_ifg >> 3);
        if (COMPILER_64_NE(rval, orval)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_ifg_get
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      size - Maximum frame size in bytes
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_big_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    soc_port_ability_t ability;
    uint32      pa_flag;

    if (!duplex) {
        return SOC_E_PARAM;
    }

    _mac_big_speed_to_pa_flag(unit, port, speed, &pa_flag); 
    soc_mac_big.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    if (IS_XE_PORT(unit, port)) {
        *ifg = si->fd_xe;
    } else {
        *ifg = si->fd_hg;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_ifg_get: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", *ifg));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_loopback_remote_set
 * Purpose:
 *      Set the BigMAC into remote-loopback state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) loopback enable state
 * Returns:
 *      SOC_E_XXX
 */
int
mac_big_loopback_remote_set(int unit, soc_port_t port, int lb)
{
    uint64 ctrl, octrl;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_loopback_remote_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "remote" : "no"));

    SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    soc_reg64_field32_set(unit, MAC_CTRLr, &ctrl, RMTLOOPf,
                          lb ? 1 : 0);
    if (COMPILER_64_NE(ctrl, octrl)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _mac_big_port_mode_update
 * Purpose:
 *      Set the BigMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      to_hg_port - (TRUE/FALSE)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_big_port_mode_update(int unit, soc_port_t port, int to_hg_port)
{
    uint32              rval;
    uint64              val64;
    soc_pbmp_t          ctr_pbmp;
    int                 rv = SOC_E_NONE;

    /* Pause linkscan */
    soc_linkscan_pause(unit);

    /* Pause counter collection */
    COUNTER_LOCK(unit);

    soc_xport_type_update(unit, port, to_hg_port);

    rv = soc_xgxs_reset(unit, port);

    if (SOC_SUCCESS(rv)) {
        rv = _fusioncore_phy_xgxs1_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = soc_phyctrl_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = _mac_big_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_big_enable_set(unit, port, 0);
    }

    if (SOC_SUCCESS(rv)) {
        SOC_PBMP_CLEAR(ctr_pbmp);
        SOC_PBMP_PORT_SET(ctr_pbmp, port);
        COMPILER_64_SET(val64, 0, 0);
        rv = soc_counter_set_by_port(unit, ctr_pbmp, val64);
    }

    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit);

    SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &rval));
    if (to_hg_port) {
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
    } else {
        soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
    return rv;
}

/*
 * Function:
 *      mac_big_encap_set
 * Purpose:
 *      Set the BigMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_encap_set(int unit, soc_port_t port, int mode)
{
    uint64              rval, orval;
    int                 enable, rv = SOC_E_NONE;
    int                 encap = 0; /* IEEE */
    int                 cur_mode, higig2 = 0;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_encap_set: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_big_encap_mode[mode]));
#endif

    SOC_IF_ERROR_RETURN(soc_mac_big.md_encap_get(unit, port, &cur_mode));
    if (cur_mode == mode) {
        /* Nothing to do */
        return SOC_E_NONE;
    }

    switch (mode) {
    case SOC_ENCAP_IEEE:
        break;
    case SOC_ENCAP_HIGIG:
        encap = 1;
        break;
    case SOC_ENCAP_B5632:
        encap = 2;
        break;
    case SOC_ENCAP_HIGIG2:
        if (soc_feature(unit, soc_feature_higig2)) {
            higig2 = 1;
            encap = 1;
        } else {
            return SOC_E_PARAM;
        }
        break;
    default:
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(mac_big_enable_get(unit, port, &enable));

    if (enable) {
    /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_big_enable_set(unit, port, 0));
    }

    /* Need to test for E port and XE port because for embedded Higig
     * ports an XE port may not be considered an E port. */
    if ((mode == SOC_ENCAP_HIGIG || mode == SOC_ENCAP_HIGIG2) &&
        (IS_E_PORT(unit, port) || IS_XE_PORT(unit, port))) {
        /* XE -> HG */
        if (soc_feature(unit, soc_feature_xport_convertible)) {
            SOC_IF_ERROR_RETURN
                (_mac_big_port_mode_update(unit, port, TRUE));
        } else {
            rv = SOC_E_PARAM;
        }
    } else if ((mode == SOC_ENCAP_IEEE) && IS_ST_PORT(unit, port)) {
        /* HG -> XE */
        if (soc_feature(unit, soc_feature_xport_convertible)) {
            SOC_IF_ERROR_RETURN
                (_mac_big_port_mode_update(unit, port, FALSE));
        } else {
            rv = SOC_E_PARAM;
        }
    }

    /* Update the encapsulation mode */
    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rval));
    orval = rval;
    soc_reg64_field32_set(unit, MAC_RXCTRLr, &rval, HDRMODEf, encap);
#if defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_higig2)) {
        soc_reg64_field32_set(unit, MAC_RXCTRLr, &rval, HIGIG2MODEf, higig2);
    }
#endif
    if (COMPILER_64_NE(rval, orval)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, rval));
    }

    SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &rval));
    orval = rval;
    soc_reg64_field32_set(unit, MAC_TXCTRLr, &rval, HDRMODEf, encap);
#if defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_higig2)) {
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &rval, HIGIG2MODEf, higig2);
    }
#endif
    if (COMPILER_64_NE(rval, orval)) {
        SOC_IF_ERROR_RETURN(WRITE_MAC_TXCTRLr(unit, port, rval));
    }

    if (enable) {
    /* Re-enable transmitter and receiver */
        SOC_IF_ERROR_RETURN(mac_big_enable_set(unit, port, 1));
    }

    return rv;
}

/*
 * Function:
 *      mac_big_encap_get
 * Purpose:
 *      Get the BigMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_encap_get(int unit, soc_port_t port, int *mode)
{
    uint64              rx_ctrl;

    /* Bad pointer */
    if (!mode) {
        return SOC_E_PARAM;
    }

    /*
     * TX/RX mode should always be programmed to the same
     * value (or all sorts of weird things will happen).
     * So, for purpose of S/W state, we only need to read
     * one or the other and return the state.
     *
     * WARNING: The following assumes BCM_PORT_ENCAP_xxx values equal
     * the hardware register field values.
     */

    SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &rx_ctrl));
    *mode = soc_reg64_field32_get(unit, MAC_RXCTRLr, rx_ctrl, HDRMODEf);

#if defined(BCM_BRADLEY_SUPPORT)
    if (soc_feature(unit, soc_feature_higig2) &&
        (*mode == SOC_ENCAP_HIGIG) &&
        soc_reg64_field32_get(unit, MAC_RXCTRLr, rx_ctrl, HIGIG2MODEf)) {
        *mode = SOC_ENCAP_HIGIG2;
    }
#endif

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_encap_get: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_big_encap_mode[*mode]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_frame_spacing_stretch_set
 * Purpose:
 *      Set frame spacing stretch parameters.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_big_frame_spacing_stretch_set(int unit, soc_port_t port, int value)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int    field_width, max_value;
        uint64 reg_val;
        field_width = soc_reg_field_length(unit, MAC_TXCTRLr, THROTDENOMf);
        max_value = (1 << (field_width + 1)) - 1;
        if ((value > max_value) || (value < 0)) {
            return SOC_E_PARAM;
        }

        SOC_IF_ERROR_RETURN
            (READ_MAC_TXCTRLr(unit, port, &reg_val));

        /* Set 0 will be automatically changed to 8 by ASIC; so 8 will be gotten for read */
        soc_reg64_field32_set(unit, MAC_TXCTRLr, &reg_val,
                              THROTDENOMf, value);

        if (SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
            soc_reg64_field32_set(unit, MAC_TXCTRLr, &reg_val,
                              THROTNUMERf, value ? 1 : 0);
        } else {
            soc_reg64_field32_set(unit, MAC_TXCTRLr, &reg_val,
                              THROTNUMf, value ? 1 : 0);
        }

        SOC_IF_ERROR_RETURN
            (WRITE_MAC_TXCTRLr(unit, port, reg_val));

        return SOC_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
 
    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      mac_big_control_set
 * Purpose:
 *      To configure MAC control properties.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                    int value)
{
    uint64 reg_val, copy;
    uint32 fval;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_control_set: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    switch(type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &reg_val));
        copy = reg_val;
        soc_reg64_field32_set(unit, MAC_CTRLr, &reg_val, RXENf, value ? 1 : 0);
        if (COMPILER_64_NE(reg_val, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, reg_val));
        }
        break;
    case SOC_MAC_CONTROL_FAILOVER_RX_SET:
        SOC_IF_ERROR_RETURN
           (soc_reg_field32_modify(unit, MAC_CTRLr, port, RXENf, value? 1 : 0));
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        /* 'value == 0' means disable Spacing Stretch Function */
        if ( value >= 8 || value == 0) {
             return _mac_big_frame_spacing_stretch_set(unit, port, value);
        } else {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &reg_val));
        soc_reg64_field32_set(unit, MAC_RXCTRLr, &reg_val, RXPASSCTRLf,
                              value ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_MAC_RXCTRLr(unit, port, reg_val));
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_TYPEr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_TYPEr, port,
                                        PFC_ETH_TYPEf, value));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_FIELDr, port,
                                        ETHERTYPEf, value));
        }
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_OPCODEr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_OPCODEr,
                                        port, PFC_OPCODEf, value));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_FIELDr,
                                        port, OPCODEf, value));
        }
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (value == 16) {
            fval = 0;
        } else if (value == 8) {
            fval = 1;
        } else {
            return SOC_E_PARAM;
        }
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                        PFC_EIGHT_CLASSf, fval));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port,
                                        PP_EIGHT_CLASSf, fval));
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_DA_LOr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_DA_LOr(unit, port, &reg_val));
            fval = soc_reg64_field32_get(unit, BMAC_PFC_DA_LOr, reg_val,
                                         PFC_MACDA_LOf);
            fval &= 0x00ffffff;
            fval |= (value & 0xff) << 24;
            soc_reg64_field32_set(unit, BMAC_PFC_DA_LOr, &reg_val, 
                                  PFC_MACDA_LOf, fval);
            SOC_IF_ERROR_RETURN(WRITE_BMAC_PFC_DA_LOr(unit, port, reg_val));

            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_DA_HIr(unit, port, &reg_val));
            soc_reg64_field32_set(unit, BMAC_PFC_DA_HIr, &reg_val, 
                                  PFC_MACDA_HIf, value >> 8);
            SOC_IF_ERROR_RETURN(WRITE_BMAC_PFC_DA_HIr(unit, port, reg_val));
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_DAr(unit, port, &reg_val));
            fval = soc_reg64_field32_get(unit, MAC_PFC_DAr, reg_val,
                                         DA_LOf);
            fval &= 0x00ffffff;
            fval |= (value & 0xff) << 24;
            soc_reg64_field32_set(unit, MAC_PFC_DAr, &reg_val, 
                                  DA_LOf, fval);
            soc_reg64_field32_set(unit, MAC_PFC_DAr, &reg_val, 
                                  DA_HIf, value >> 8);
            SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DAr(unit, port, reg_val));
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_DA_LOr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_DA_LOr(unit, port, &reg_val));
            fval = soc_reg64_field32_get(unit, BMAC_PFC_DA_LOr, reg_val,
                                         PFC_MACDA_LOf);
            fval &= 0xff000000;
            fval |= value & 0x00ffffff;
            soc_reg64_field32_set(unit, BMAC_PFC_DA_LOr, &reg_val, 
                                  PFC_MACDA_LOf, fval);
            SOC_IF_ERROR_RETURN(WRITE_BMAC_PFC_DA_LOr(unit, port, reg_val));
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_DAr(unit, port, &reg_val));
            fval = soc_reg64_field32_get(unit, MAC_PFC_DAr, reg_val,
                                         DA_LOf);
            fval &= 0xff000000;
            fval |= value & 0x00ffffff;
            soc_reg64_field32_set(unit, MAC_PFC_DAr, &reg_val, 
                                  DA_LOf, fval);
            SOC_IF_ERROR_RETURN(WRITE_MAC_PFC_DAr(unit, port, reg_val));
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port,
                                        RX_PASS_PFC_FRMf, value ? 1 : 0));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_RXCTRLr, port,
                                        RXPASSCTRLf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        if (soc_reg_field_valid(unit, MAC_CTRLr, PP_RX_ENABLEf)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_CTRLr, port, PP_RX_ENABLEf,
                                        value ? 1 : 0));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port, PP_RX_ENBLf,
                                        value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        if (soc_reg_field_valid(unit, MAC_CTRLr, PP_TX_ENABLEf)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_CTRLr, port, PP_TX_ENABLEf,
                                        value ? 1 : 0));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port, PP_TX_ENBLf,
                                        value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port, 
                                        FORCE_PFC_XONf, value ? 1 : 0));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port, 
                                        FORCE_PP_XONf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, BMAC_PFC_CTRLr, port, 
                                        PFC_STATS_ENf, value ? 1 : 0));
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MAC_PFC_CTRLr, port, 
                                        PFC_STATS_ENf, value ? 1 : 0));
        }
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_RXLSSCTRLr, port,
                                    LOCALFAULTDISABLEf, value ? 0 : 1));
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, MAC_RXLSSCTRLr, port,
                                    REMOTEFAULTDISABLEf, value ? 0 : 1));
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_big_control_get
 * Purpose:
 *      To get current MAC control setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      type - MAC control property to set.
 *      int  - New setting for MAC control.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                    int *value)
{
    int rv = SOC_E_NONE;
    uint64 reg_val;
    uint32 fval0, fval1;

    if (NULL == value) {
        return SOC_E_PARAM;
    }

    switch(type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_CTRLr, reg_val, RXENf);
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if ((!SOC_REG_IS_VALID(unit, MAC_TXCTRLr)) ||
            (!soc_reg_field_valid(unit, MAC_TXCTRLr, THROTDENOMf))){
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_MAC_TXCTRLr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_TXCTRLr, reg_val,
                                       THROTDENOMf);

        /* when THROTNUMf/THROTNUMERf is 0, THROTDENOMf is not effective.
         * overwrite *value to 0 for the meaning of Spacing Stretch function is disabled  */
        if (soc_reg_field_valid(unit, MAC_TXCTRLr, THROTNUMf)) {
            if (0 == soc_reg64_field32_get(unit, MAC_TXCTRLr, reg_val, THROTNUMf)) {
                *value = 0;
            }
        }
        else if (soc_reg_field_valid(unit, MAC_TXCTRLr, THROTNUMERf)) {
            if (0 == soc_reg64_field32_get(unit, MAC_TXCTRLr, reg_val, THROTNUMERf)) {
                *value = 0;
            }
        }
        break;

    case SOC_MAC_CONTROL_TIMESTAMP_TRANSMIT:
        {
            uint32  timestamp = 0;
            if (soc_feature(unit, soc_feature_timestamp_counter) &&
                soc_property_get(unit, spn_SW_TIMESTAMP_FIFO_ENABLE, 1)) {
                rv = soc_counter_timestamp_get(unit, port, &timestamp);
                if (rv != SOC_E_NOT_FOUND) {
                    *value = (int)timestamp;
                    return rv;
                }
            }

            if (!SOC_REG_IS_VALID(unit, MAC_TXTIMESTAMPFIFOSTATUSr)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN
                (READ_MAC_TXTIMESTAMPFIFOSTATUSr(unit, port, &reg_val));
            if (soc_reg64_field32_get(unit, MAC_TXTIMESTAMPFIFOSTATUSr,
                                      reg_val, ENTRY_COUNTf) == 0) {
                return SOC_E_EMPTY;
            }
            SOC_IF_ERROR_RETURN
                (READ_MAC_TXTIMESTAMPFIFOREADr(unit, port, &reg_val));
            timestamp = soc_reg64_field32_get(unit, MAC_TXTIMESTAMPFIFOREADr,
                                       reg_val, TIMESTAMPf);
            *value = (int)timestamp;
            rv = SOC_E_NONE;
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_RXCTRLr, reg_val,
                                       RXPASSCTRLf);
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_TYPEr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_TYPEr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_TYPEr, reg_val,
                                           PFC_ETH_TYPEf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_FIELDr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_FIELDr, reg_val,
                                           ETHERTYPEf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_OPCODEr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_OPCODEr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_OPCODEr, reg_val,
                                           PFC_OPCODEf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_FIELDr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_FIELDr, reg_val,
                                           OPCODEf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_CTRLr, reg_val,
                                           PFC_EIGHT_CLASSf) ? 8 : 16;
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_CTRLr, reg_val,
                                           PP_EIGHT_CLASSf) ? 8 : 16;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_DA_LOr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_DA_LOr(unit, port, &reg_val));
            fval0 = soc_reg64_field32_get(unit, BMAC_PFC_DA_LOr, reg_val,
                                         PFC_MACDA_LOf);
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_DA_HIr(unit, port, &reg_val));
            fval1 = soc_reg64_field32_get(unit, BMAC_PFC_DA_HIr, reg_val,
                                          PFC_MACDA_HIf);
            *value = (fval0 >> 24) | (fval1 << 8);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_DAr(unit, port, &reg_val));
            fval0 = soc_reg64_field32_get(unit, MAC_PFC_DAr, reg_val,
                                         DA_LOf);
            fval1 = soc_reg64_field32_get(unit, MAC_PFC_DAr, reg_val,
                                          DA_HIf);
            *value = (fval0 >> 24) | (fval1 << 8);
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_DA_LOr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_DA_LOr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_DA_LOr, reg_val,
                                           PFC_MACDA_LOf) & 0x00ffffff;
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_DAr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_DAr, reg_val,
                                           DA_LOf) & 0x00ffffff;
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_CTRLr, reg_val,
                                           RX_PASS_PFC_FRMf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_RXCTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_RXCTRLr, reg_val,
                                           RXPASSCTRLf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        if (soc_reg_field_valid(unit, MAC_CTRLr, PP_RX_ENABLEf)) {
            SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_CTRLr, reg_val,
                                           PP_RX_ENABLEf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_CTRLr, reg_val,
                                           PP_RX_ENBLf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        if (soc_reg_field_valid(unit, MAC_CTRLr, PP_TX_ENABLEf)) {
            SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_CTRLr, reg_val,
                                           PP_TX_ENABLEf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_CTRLr, reg_val,
                                           PP_TX_ENBLf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) {
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_CTRLr, reg_val,
                                           FORCE_PFC_XONf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_CTRLr, reg_val,
                                           FORCE_PP_XONf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        if (SOC_REG_IS_VALID(unit, BMAC_PFC_CTRLr)) { 
            SOC_IF_ERROR_RETURN(READ_BMAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, BMAC_PFC_CTRLr, reg_val,
                                           PFC_STATS_ENf);
        } else {
            SOC_IF_ERROR_RETURN(READ_MAC_PFC_CTRLr(unit, port, &reg_val));
            *value = soc_reg64_field32_get(unit, MAC_PFC_CTRLr, reg_val,
                                           PP_TX_ENBLf);
        }
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(READ_MAC_RXLSSCTRLr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_RXLSSCTRLr, reg_val,
                                       LOCALFAULTDISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOC_IF_ERROR_RETURN(READ_MAC_RXLSSSTATr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_RXLSSSTATr, reg_val,
                                       LOCALFAULTSTATf);
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_MAC_RXLSSCTRLr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_RXLSSCTRLr, reg_val,
                                       REMOTEFAULTDISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOC_IF_ERROR_RETURN(READ_MAC_RXLSSSTATr(unit, port, &reg_val));
        *value = soc_reg64_field32_get(unit, MAC_RXLSSSTATr, reg_val,
                                       REMOTEFAULTSTATf);
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_control_get: unit %d port %s type=%d value=%d "
                             "rv=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, *value, rv));

    return rv;
}

/*
 * Function:
 *      mac_big_ability_local_get
 * Purpose:
 *      Return the abilities of BigMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_big_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    int idx;
    int port_speed_max;

    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->speed_full_duplex  = SOC_PA_ABILITY_NONE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_ABILITY_NONE;
    ability->loopback  = SOC_PA_LB_MAC;
    ability->flags     = SOC_PA_ABILITY_NONE;

    if (soc_feature(unit, soc_feature_xport_convertible)) {
        ability->encap = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG |
            SOC_PA_ENCAP_B5632;
    } else {
        ability->encap = 0;
        if (IS_E_PORT(unit, port)) {
            ability->encap |= SOC_PA_ENCAP_IEEE;
        }
        if (IS_ST_PORT(unit, port)) {
            ability->encap |= SOC_PA_ENCAP_HIGIG | SOC_PA_ENCAP_B5632;
        }
    }
    if (soc_feature(unit, soc_feature_higig2) &&
        (ability->encap & SOC_PA_ENCAP_HIGIG)) {
        ability->encap |= SOC_PA_ENCAP_HIGIG2;
    }

    if (IS_LMD_ENABLED_PORT(unit, port)) {
        ability->speed_full_duplex  |= SOC_PA_SPEED_2500MB;
    } else {
        ability->speed_full_duplex  |= SOC_PA_SPEED_10GB;
    }

    if (!IS_HG_PORT(unit , port)) {
        LOG_VERBOSE(BSL_LS_SOC_10G,
                    (BSL_META_U(unit,
                                "mac_big_ability_local_get: unit %d port %s "
                                 "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                                 "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                     unit, SOC_PORT_NAME(unit, port),
                     ability->speed_half_duplex, ability->speed_full_duplex,
                     ability->encap, ability->pause, ability->interface,
                     ability->medium, ability->loopback, ability->flags));
        return SOC_E_NONE;
    }

    if (soc_feature(unit, soc_feature_xgxs_lcpll)) {
        if (IS_LMD_ENABLED_PORT(unit, port)) {
            ability->speed_full_duplex |= SOC_PA_SPEED_3000MB;
        } else {
            ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
        }
    } else {
        port_speed_max = SOC_INFO(unit).port_speed_max[port];
        for (idx = 0; idx < sizeof(mac_big_hg_speed_list) /
                 sizeof(mac_big_hg_speed_list[0]); idx++) {
            if (port_speed_max < mac_big_hg_speed_list[idx].speed) {
                break;
            }
            ability->speed_full_duplex |= mac_big_hg_speed_list[idx].pa_flag;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_big_ability_local_get: unit %d port %s "
                             "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                             "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 ability->speed_half_duplex, ability->speed_full_duplex,
                 ability->encap, ability->pause, ability->interface,
                 ability->medium, ability->loopback, ability->flags));
    return SOC_E_NONE;
}

/* Exported BigMAC driver structure */
mac_driver_t soc_mac_big = {
    "BigMAC Driver",              /* drv_name */
    mac_big_init,                 /* md_init  */
    mac_big_enable_set,           /* md_enable_set */
    mac_big_enable_get,           /* md_enable_get */
    mac_big_duplex_set,           /* md_duplex_set */
    mac_big_duplex_get,           /* md_duplex_get */
    mac_big_speed_set,            /* md_speed_set */
    mac_big_speed_get,            /* md_speed_get */
    mac_big_pause_set,            /* md_pause_set */
    mac_big_pause_get,            /* md_pause_get */
    mac_big_pause_addr_set,       /* md_pause_addr_set */
    mac_big_pause_addr_get,       /* md_pause_addr_get */
    mac_big_loopback_set,         /* md_lb_set */
    mac_big_loopback_get,         /* md_lb_get */
    mac_big_interface_set,        /* md_interface_set */
    mac_big_interface_get,        /* md_interface_get */
    NULL,                         /* md_ability_get - Deprecated */
    mac_big_frame_max_set,        /* md_frame_max_set */
    mac_big_frame_max_get,        /* md_frame_max_get */
    mac_big_ifg_set,              /* md_ifg_set */
    mac_big_ifg_get,              /* md_ifg_get */
    mac_big_encap_set,            /* md_encap_set */
    mac_big_encap_get,            /* md_encap_get */
    mac_big_control_set,          /* md_control_set */
    mac_big_control_get,          /* md_control_get */
    mac_big_ability_local_get,    /* md_ability_local_get */
    NULL                          /* md_timesync_tx_info_get */
 };

#endif /* BCM_BIGMAC_SUPPORT */
