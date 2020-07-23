/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CMAC driver
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

#ifdef BCM_CMAC_SUPPORT

/*
 * CMAC Register field definitions.
 */

/* Transmit CRC Modes (Receive has bit field definitions in register) */
#define CMAC_CRC_APPEND          0x00   /* Append CRC (Default) */
#define CMAC_CRC_KEEP            0x01   /* CRC Assumed correct */
#define CMAC_CRC_REGEN           0x02   /* Replace CRC with a new one */
#define CMAC_CRC_PER_PKT_MODE    0x03   /* Update CRC based on h/w signal
                                           from port block */

#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

/* 
 * CMAC timestamp delay definitions
 * From Katana/triumph3 spec, Timestamp clock period is 4ns (125Mhz) and 
 *                            Tx Line Clock period CMAC (341.8Mhz) is 2.9ns
 */
#define CMAC_TX_LINE_RATE_NS   0x3


mac_driver_t soc_mac_c;

#ifdef BROADCOM_DEBUG
static char *mac_c_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *mac_c_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

#define SOC_CMAC_SPEED_10     0x0
#define SOC_CMAC_SPEED_100    0x1
#define SOC_CMAC_SPEED_1000   0x2
#define SOC_CMAC_SPEED_2500   0x3
#define SOC_CMAC_SPEED_10000  0x4


/* Forward declarations */
STATIC int mac_c_loopback_set(int unit, soc_port_t port, int lb);
STATIC int mac_c_loopback_get(int unit, soc_port_t port, int *lb);
STATIC int mac_c_frame_max_set(int unit, soc_port_t port, int size);
STATIC int mac_c_frame_max_get(int unit, soc_port_t port, int *size);
STATIC int mac_c_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx);
STATIC int mac_c_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx);

STATIC int
_mac_c_drain_cells(int unit, soc_port_t port)
{
    int         rv;
    int         pause_tx, pause_rx, pfc_rx, llfc_rx;

    rv  = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return rv;
    }

    /* Drain cells in mmu/port before cells entering TX FIFO */
    SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, TRUE));

    /* Disable pause/pfc/llfc function */
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_pause_set(unit, port, pause_tx, 0));

    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_control_get(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                  &pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                  0));

    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_control_get(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                  &llfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                  0));

    /* Drain data in TX FIFO without egressing */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CMAC_TX_CTRLr, port, DISCARDf, 1));

    /* Notify PHY driver */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventStop, PHY_STOP_DRAIN));

    /* Wait until mmu cell count is 0 */
    rv = soc_egress_drain_cells(unit, port, 250000);
    if (SOC_E_NONE == rv) {
        /* Wait until TX fifo cell count is 0 */
        rv = soc_txfifo_drain_cells(unit, port, 250000);
    }

    /* Notify PHY driver */
    SOC_IF_ERROR_RETURN
        (soc_phyctrl_notify(unit, port, phyEventResume, PHY_STOP_DRAIN));
  
    /* Stop TX FIFO drainging */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CMAC_TX_CTRLr, port, DISCARDf, 0));

    /* Restore original pause/pfc/llfc configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_pause_set(unit, port, pause_tx, pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                  pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_c.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                  llfc_rx));
    
    /* Stop draining cells in mmu/port */
    SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));

    return rv;
}

/*
 * Function:
 *      mac_c_init
 * Purpose:
 *      Initialize Cmac into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_c_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    soc_reg_t reg;
    uint64 mac_ctrl, rx_ctrl, tx_ctrl, rval;
    int ipg;
    int mode;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    si = &SOC_INFO(unit);

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    if (soc_reg_field_valid(unit, CMAC_CTRLr, XLGMII_ALIGN_ENBf)) {
                
        if (IS_HG_PORT(unit, port) && (si->port_speed_max[port] >= 106000)) {

            soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, 
                                  XLGMII_ALIGN_ENBf, 0);
            
        } else {
            soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, 
                                  XLGMII_ALIGN_ENBf, 1);
        }

    }
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl,
                          XGMII_IPG_CHECK_DISABLEf,
                          IS_HG_PORT(unit, port) ? 1 : 0);

    SOC_IF_ERROR_RETURN(WRITE_CMAC_CTRLr(unit, port, mac_ctrl));

    
    if (SOC_REG_IS_VALID(unit, CPORT_MAC_MODEr)) {
        reg = CPORT_MAC_MODEr;
    } else {
        reg = PORT_CMAC_MODEr;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, reg, port, CGMII_MODEf,
                                si->port_speed_max[port] > 106000 ? 1 : 0));

    SOC_IF_ERROR_RETURN(READ_CMAC_RX_CTRLr(unit, port, &rx_ctrl));
    soc_reg64_field32_set(unit, CMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CMAC_RX_CTRLr(unit, port, rx_ctrl));

    SOC_IF_ERROR_RETURN(READ_CMAC_TX_CTRLr(unit, port, &tx_ctrl));
    ipg = IS_HG_PORT(unit,port)?  SOC_PERSIST(unit)->ipg[port].fd_hg:
                                  SOC_PERSIST(unit)->ipg[port].fd_xe;
    soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf,
                          (ipg / 8) & 0x1f);

    soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf,
                          CMAC_CRC_PER_PKT_MODE);

    SOC_IF_ERROR_RETURN(WRITE_CMAC_TX_CTRLr(unit, port, tx_ctrl));

    if (IS_ST_PORT(unit, port)) {
        soc_mac_c.md_pause_set(unit, port, FALSE, FALSE);
    } else {
        soc_mac_c.md_pause_set(unit, port, TRUE, TRUE);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, PFC_REFRESH_ENf,
                                1));

    if (soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE)) {
        /* Max speed for WAN mode is 9.294Gbps.
         * This setting gives 10Gbps * (13/14) or 9.286 Gbps */
        SOC_IF_ERROR_RETURN
            (soc_mac_c.md_control_set(unit, port,
                                      SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
                                      13));
    }

    /* Set jumbo max size (8000 byte payload) */
    COMPILER_64_ZERO(rval);
#ifdef BCM_XGS_SUPPORT
    if (SOC_IS_XGS(unit)) {
        soc_reg64_field32_set(unit, CMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                              SOC_INFO(unit).max_mtu);
    } else
#endif
    {
        soc_reg64_field32_set(unit, CMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                              JUMBO_MAXSZ);
    }
    SOC_IF_ERROR_RETURN(WRITE_CMAC_RX_MAX_SIZEr(unit, port, rval));

    /* Setup header mode, check for property for higig2 mode */
    COMPILER_64_ZERO(rval);
    if (IS_HG_PORT(unit, port)) {
        mode = soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0) ?
            2 : 1;
        soc_reg64_field32_set(unit, CMAC_MODEr, &rval, HDR_MODEf, mode);
    }
    soc_reg64_field32_set(unit, CMAC_MODEr, &rval, SPEED_MODEf, SOC_CMAC_SPEED_10000);
    SOC_IF_ERROR_RETURN(WRITE_CMAC_MODEr(unit, port, rval));

    /* Disable loopback and bring CMAC out of reset */
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, CORE_REMOTE_LPBKf, 0);
    if (soc_reg_field_valid(unit, CMAC_CTRLr, LINE_REMOTE_LPBKf)) {
        soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, LINE_REMOTE_LPBKf,
                              0);
    }
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, CORE_LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, LINE_LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, CMAC_CTRLr, &mac_ctrl, TX_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMAC_CTRLr(unit, port, mac_ctrl));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_enable_set
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
mac_c_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 ctrl, octrl;
    pbmp_t mask;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_enable_set: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 enable ? "True" : "False"));

    SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);
    if (COMPILER_64_NE(ctrl, octrl)) {
        SOC_IF_ERROR_RETURN(WRITE_CMAC_CTRLr(unit, port, ctrl));
        sal_udelay(10);
    }

    /* Put the CMAC in a soft reset state when disabled to allow packets to
     * drain from the TX FIFO */ 
    soc_reg64_field32_set(unit, CMAC_CTRLr, &ctrl, SOFT_RESETf,
                          enable ? 0 : 1); 
    if (COMPILER_64_NE(ctrl, octrl)) {
        SOC_IF_ERROR_RETURN(WRITE_CMAC_CTRLr(unit, port, ctrl));
    }

    if (enable) {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventResume,
                                               PHY_STOP_MAC_DIS));
    } else {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(_mac_c_drain_cells(unit, port));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventStop,
                                               PHY_STOP_MAC_DIS));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_enable_get
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
mac_c_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &ctrl));

    *enable = soc_reg64_field32_get(unit, CMAC_CTRLr, ctrl, RX_ENf);

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_enable_get: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *enable ? "True" : "False"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _mac_c_timestamp_delay_set
 * Purpose:
 *      Set Timestamp delay for one-step to account for lane and pipeline delay.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_c_timestamp_delay_set(int unit, soc_port_t port, int speed)
{
    uint64 ctrl;
    int osts_delay, tsts_delay, conversion_delay;
    uint32 field;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_timestamp_delay_set: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    if (SOC_REG_IS_VALID(unit, CMAC_TIMESTAMP_ADJUSTr)) {
        SOC_IF_ERROR_RETURN(READ_CMAC_TIMESTAMP_ADJUSTr(unit, port, &ctrl));

        /*
         * CMAC pipeline delay is : ( as in CMAC specification)
         *          = (7.5 * TX line clock period) + (Timestamp clock period)
         */
        
        
        /* 2's complement signed value of pipeline delay in ns */
        osts_delay = soc_property_port_get(unit, port, spn_TIMESTAMP_ADJUST(speed),
                                           SOC_TIMESYNC_PLL_CLOCK_NS(unit) -
                                           ((15 * CMAC_TX_LINE_RATE_NS ) / 2));

        if (SOC_E_NONE != soc_reg_signed_field_mask(unit, CMAC_TIMESTAMP_ADJUSTr,
                              TS_OSTS_ADJUSTf, osts_delay, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "%s property out of bounds (is %d)\n"),
                spn_TIMESTAMP_ADJUST(speed), osts_delay));
            field = 0;
        }

        soc_reg64_field32_set(unit, CMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_OSTS_ADJUSTf, field);

        /* Note: TSTS delay uses same SOC property as OSTS delay, in order to allow
           specification based on platform.  However, the resulting timestamps in
           the FIFO are only used by the application, so the application can make
           other timestamp adjustments as required.
        */
        tsts_delay = soc_property_port_get(unit, port, spn_TIMESTAMP_ADJUST(speed),
                                           SOC_TIMESYNC_PLL_CLOCK_NS(unit) -
                                           ((5 * CMAC_TX_LINE_RATE_NS ) / 2));

        if (SOC_E_NONE != soc_reg_signed_field_mask(unit, CMAC_TIMESTAMP_ADJUSTr,
                                                    TS_TSTS_ADJUSTf, tsts_delay, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "%s property out of bounds (is %d)\n"),
                spn_TIMESTAMP_ADJUST(speed), tsts_delay));
            field = 0;
        }

        soc_reg64_field32_set(unit, CMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_TSTS_ADJUSTf, field);

        /* 5x mode delay for cmac: 1 * TX line clock period */
        conversion_delay = 1 * CMAC_TX_LINE_RATE_NS;
        soc_reg64_field32_set(unit, CMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_OSTS_ADJUST_CONVERSION_DELAYf, conversion_delay);

        SOC_IF_ERROR_RETURN(WRITE_CMAC_TIMESTAMP_ADJUSTr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_duplex_set
 * Purpose:
 *      Set CMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_c_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_duplex_get
 * Purpose:
 *      Get CMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_pause_set
 * Purpose:
 *      Configure CMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    soc_field_t fields[2] = { TX_PAUSE_ENf, RX_PAUSE_ENf };
    uint32 values[2];

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_pause_set: unit %d port %s TX=%s RX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_tx != FALSE ? "on" : "off",
                 pause_rx != FALSE ? "on" : "off"));

    values[0] = pause_tx != FALSE ? 1 : 0;
    values[1] = pause_rx != FALSE ? 1 : 0;
    return soc_reg_fields32_modify(unit, CMAC_PAUSE_CTRLr, port, 2,
                                   fields, values);
}

/*
 * Function:
 *      mac_c_pause_get
 * Purpose:
 *      Return the pause ability of CMAC
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
mac_c_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CMAC_PAUSE_CTRLr(unit, port, &rval));
    *pause_tx =
        soc_reg64_field32_get(unit, CMAC_PAUSE_CTRLr, rval, TX_PAUSE_ENf);
    *pause_rx =
        soc_reg64_field32_get(unit, CMAC_PAUSE_CTRLr, rval, RX_PAUSE_ENf);
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_pause_get: unit %d port %s TX=%s RX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *pause_tx ? "on" : "off",
                 *pause_rx ? "on" : "off"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_speed_set
 * Purpose:
 *      Set CMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 100000, 120000.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_speed_set(int unit, soc_port_t port, int speed)
{
    soc_reg_t reg;
    int loopback, frame_max;
    int pause_rx, pause_tx;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_speed_set: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed));

    /* Save MAC state before reset */
    SOC_IF_ERROR_RETURN(mac_c_loopback_get(unit, port, &loopback));
    SOC_IF_ERROR_RETURN(mac_c_frame_max_get(unit, port, &frame_max));
    SOC_IF_ERROR_RETURN(mac_c_pause_get(unit, port, &pause_tx, &pause_rx));

    /* mac init */
    
    SOC_IF_ERROR_RETURN(mac_c_init(unit,port));

    /* Restore MAC state after reset */
    SOC_IF_ERROR_RETURN(mac_c_loopback_set(unit, port, loopback));
    SOC_IF_ERROR_RETURN(mac_c_frame_max_set(unit, port, frame_max));
    SOC_IF_ERROR_RETURN(mac_c_pause_set(unit, port, pause_tx, pause_rx));

    /* Set cmac timestamp delay */
    SOC_IF_ERROR_RETURN(_mac_c_timestamp_delay_set(unit,port, speed));

    /* program the XLGMII field */
    if (soc_reg_field_valid(unit, CMAC_CTRLr, XLGMII_ALIGN_ENBf)) {
        if (IS_HG_PORT(unit, port) && (speed >= 106000)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, CMAC_CTRLr, port, 
                                        XLGMII_ALIGN_ENBf, 0));
            
        } else {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, CMAC_CTRLr, port, XLGMII_ALIGN_ENBf,
                                        speed >= 100000 ? 1 : 0));
        }
    }

    /* And the PORT CGMII field */
    
    if (SOC_REG_IS_VALID(unit, CPORT_MAC_MODEr)) {
        reg = CPORT_MAC_MODEr;
    } else {
        reg = PORT_CMAC_MODEr;
    }
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, reg, port, CGMII_MODEf,
                                speed > 106000 ? 1 : 0));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_speed_get
 * Purpose:
 *      Get CMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, CMAC_MODEr, rval, SPEED_MODEf)) {
    case SOC_CMAC_SPEED_10:
        *speed = 10;
        break;
    case SOC_CMAC_SPEED_100:
        *speed = 100;
        break;
    case SOC_CMAC_SPEED_1000:
        *speed = 1000;
        break;
    case SOC_CMAC_SPEED_2500:
        *speed = 2500;
        break;
    case SOC_CMAC_SPEED_10000:
    default:
        *speed = 10000;
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_loopback_set
 * Purpose:
 *      Set a CMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On Cmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_loopback_set(int unit, soc_port_t port, int lb)
{
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_loopback_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "local" : "no"));

    /* need to enable clock compensation for applicable serdes device */
    (void)soc_phyctrl_notify(unit, port, phyEventMacLoopback, lb? 1: 0);

    return soc_reg_field32_modify(unit, CMAC_CTRLr, port, LINE_LOCAL_LPBKf,
                                  lb ? 1 : 0);

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_loopback_get
 * Purpose:
 *      Get current CMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint64 ctrl;
    int local, remote;

    SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &ctrl));

    remote = soc_reg64_field32_get(unit, CMAC_CTRLr, ctrl, CORE_REMOTE_LPBKf);
    if (soc_reg_field_valid(unit, CMAC_CTRLr, LINE_REMOTE_LPBKf)) {
        remote |= soc_reg64_field32_get(unit, CMAC_CTRLr, ctrl,
                                        LINE_REMOTE_LPBKf);
    }
    local = soc_reg64_field32_get(unit, CMAC_CTRLr, ctrl, CORE_LOCAL_LPBKf) |
        soc_reg64_field32_get(unit, CMAC_CTRLr, ctrl, LINE_LOCAL_LPBKf);
    *lb = local | remote;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_loopback_get: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *lb ? (remote ? "remote" : "local") : "no"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_pause_addr_set
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
mac_c_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 r, tmp;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_pause_addr_set: unit %d port %s MAC=<"
                             "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 m[0], m[1], m[2], m[3], m[4], m[5]));

    COMPILER_64_ZERO(r);
    for (i = 0; i< 6; i++) {
        COMPILER_64_SET(tmp, 0, m[i]);
        COMPILER_64_SHL(r, 8);
        COMPILER_64_OR(r, tmp);
    }

    SOC_IF_ERROR_RETURN(WRITE_CMAC_TX_MAC_SAr(unit, port, r));
    SOC_IF_ERROR_RETURN(WRITE_CMAC_RX_MAC_SAr(unit, port, r));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_pause_addr_get
 * Purpose:
 *      Retrieve PAUSE frame source address.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_mac - (OUT) MAC address used for pause transmission.
 * Returns:
 *      SOC_E_XXX
 * NOTE: We always write the same thing to TX & RX SA
 *       so, we just return the contects on RX_MAC_SA.
 */
STATIC int
mac_c_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 reg;
    uint32 msw;
    uint32 lsw;

    SOC_IF_ERROR_RETURN(READ_CMAC_RX_MAC_SAr(unit, port, &reg));
    COMPILER_64_TO_32_HI(msw, reg);
    COMPILER_64_TO_32_LO(lsw, reg);

    m[0] = (uint8) ( ( msw & 0x0000ff00 ) >> 8 );
    m[1] = (uint8) ( msw & 0x000000ff );

    m[2] = (uint8)  ( ( lsw & 0xff000000) >> 24 );
    m[3] = (uint8)  ( ( lsw & 0x00ff0000) >> 16 );
    m[4] = (uint8)  ( ( lsw & 0x0000ff00) >> 8 );
    m[5] = (uint8)  ( lsw & 0x000000ff );

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_pause_addr_get: unit %d port %s MAC=<"
                             "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 m[0], m[1], m[2], m[3], m[4], m[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_interface_set
 * Purpose:
 *      Set a CMAC interface type
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
mac_c_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_interface_set: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_c_port_if_names[pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_interface_get
 * Purpose:
 *      Retrieve CMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_c_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_CGMII;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_interface_get: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_c_port_if_names[*pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_frame_max_set
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
mac_c_frame_max_set(int unit, soc_port_t port, int size)
{
    uint64 rval;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_frame_max_set: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 size));

    if (IS_CE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        size += 4;
    }
    COMPILER_64_ZERO(rval);
    soc_reg64_field32_set(unit, CMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf, size);
    SOC_IF_ERROR_RETURN(WRITE_CMAC_RX_MAX_SIZEr(unit, port, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_frame_max_get
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
mac_c_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CMAC_RX_MAX_SIZEr(unit, port, &rval));
    *size = soc_reg64_field32_get(unit, CMAC_RX_MAX_SIZEr, rval, RX_MAX_SIZEf);
    if (IS_CE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        *size -= 4;
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_frame_max_get: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *size));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_ifg_set
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
mac_c_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int ifg)
{
    int         cur_speed;
    int         cur_duplex;
    int         real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    uint64      rval, orval;
    soc_port_ability_t ability;
    uint32      pa_flag;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_ifg_set: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", ifg));

    pa_flag = SOC_PA_SPEED(speed); 
    soc_mac_c.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 31 bytes (i.e. multiple of 8 bits) */
    real_ifg = ifg < 64 ? 64 : (ifg + 7) & (0x1f << 3);

    if (IS_CE_PORT(unit, port)) {
        si->fd_xe = real_ifg;
    } else {
        si->fd_hg = real_ifg;
    }

    SOC_IF_ERROR_RETURN(mac_c_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_c_speed_get(unit, port, &cur_speed));

    if (cur_speed == speed &&
        cur_duplex == (duplex == SOC_PORT_DUPLEX_FULL ? TRUE : FALSE)) {
        SOC_IF_ERROR_RETURN(READ_CMAC_TX_CTRLr(unit, port, &rval));
        orval = rval;
        soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &rval, AVERAGE_IPGf,
                              real_ifg / 8);
        if (COMPILER_64_NE(rval, orval)) {
            SOC_IF_ERROR_RETURN(WRITE_CMAC_TX_CTRLr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_ifg_get
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
mac_c_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    soc_port_ability_t ability;
    uint32      pa_flag;

    if (!duplex) {
        return SOC_E_PARAM;
    }

    pa_flag = SOC_PA_SPEED(speed); 
    soc_mac_c.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    if (IS_CE_PORT(unit, port)) {
        *ifg = si->fd_xe;
    } else {
        *ifg = si->fd_hg;
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_ifg_get: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", *ifg));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_loopback_remote_set
 * Purpose:
 *      Set the CMAC into remote-loopback state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) loopback enable state
 * Returns:
 *      SOC_E_XXX
 */
int
mac_c_loopback_remote_set(int unit, soc_port_t port, int lb)
{
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_loopback_remote_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "remote" : "no"));

    return soc_reg_field32_modify(unit, CMAC_CTRLr, port, CORE_REMOTE_LPBKf,
                                  lb ? 1 : 0);
}

/*
 * Function:
 *      _mac_c_port_mode_update
 * Purpose:
 *      Set the CMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      to_hg_port - (TRUE/FALSE)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_c_port_mode_update(int unit, soc_port_t port, int to_hg_port)
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

#if defined (BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
       /* no need to hard reset the Warpcore serdes device. Soft init
        * should be good enough.  Hard reset brings
        * down all other ports using the same Warpcore in quad-port mode. 
        */
    } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        rv = soc_xgxs_reset(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = soc_phyctrl_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_c_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_c_enable_set(unit, port, 0);
    }

    if (SOC_SUCCESS(rv)) {
        SOC_PBMP_CLEAR(ctr_pbmp);
        SOC_PBMP_PORT_SET(ctr_pbmp, port);
        COMPILER_64_SET(val64, 0, 0);
        rv = soc_counter_set_by_port(unit, ctr_pbmp, val64);
    }

    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit);
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_PORT_CONFIGr(unit, port, &rval));
    if (to_hg_port) {
        soc_reg_field_set(unit, PORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        if (soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0)) {
            soc_reg_field_set(unit, PORT_CONFIGr, &rval, HIGIG2_MODEf, 1);
        }
    } else {
        soc_reg_field_set(unit, PORT_CONFIGr, &rval, HIGIG_MODEf, 0);
    }
    SOC_IF_ERROR_RETURN(WRITE_PORT_CONFIGr(unit, port, rval));
    return rv;
}

/*
 * Function:
 *      mac_c_encap_set
 * Purpose:
 *      Set the CMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_encap_set(int unit, soc_port_t port, int mode)
{
    int enable, encap, rv;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_encap_set: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_c_encap_mode[mode]));
#endif

    switch (mode) {
    case SOC_ENCAP_IEEE:
        encap = 0;
        break;
    case SOC_ENCAP_HIGIG:
        encap = 1;
        break;
    case SOC_ENCAP_HIGIG2:
        encap = 2;
        break;
    default:
        return SOC_E_PARAM;
    }

    if (!soc_feature(unit, soc_feature_xport_convertible)) {
        if ((IS_E_PORT(unit, port) && mode != SOC_ENCAP_IEEE) ||
            (IS_ST_PORT(unit, port) && mode == SOC_ENCAP_IEEE)) {
            return SOC_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(mac_c_enable_get(unit, port, &enable));

    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_c_enable_set(unit, port, 0));
    }

    if (IS_E_PORT(unit, port) && mode != SOC_ENCAP_IEEE) {
        /* CE -> HG */
        SOC_IF_ERROR_RETURN(_mac_c_port_mode_update(unit, port, TRUE));
    } else if (IS_ST_PORT(unit, port) && mode == SOC_ENCAP_IEEE) {
        /* HG -> CE */
        SOC_IF_ERROR_RETURN(_mac_c_port_mode_update(unit, port, FALSE));
    }

    /* Update the encapsulation mode */
    rv = soc_reg_field32_modify(unit, CMAC_MODEr, port, HDR_MODEf, encap);

    if (enable) {
        /* Re-enable transmitter and receiver */
        SOC_IF_ERROR_RETURN(mac_c_enable_set(unit, port, 1));
    }

    return rv;
}

/*
 * Function:
 *      mac_c_encap_get
 * Purpose:
 *      Get the CMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_encap_get(int unit, soc_port_t port, int *mode)
{
    uint64 rval;

    if (!mode) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_CMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, CMAC_MODEr, rval, HDR_MODEf)) {
    case 0:
        *mode = SOC_ENCAP_IEEE;
        break;
    case 1:
        *mode = SOC_ENCAP_HIGIG;
        break;
    case 2:
        *mode = SOC_ENCAP_HIGIG2;
        break;
    default:
        *mode = SOC_ENCAP_COUNT;
    }

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_encap_get: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_c_encap_mode[*mode]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_expected_rx_latency_get
 * Purpose:
 *      Get the CMAC port expected Rx latency
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      latency - (OUT) Latency in NS
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_expected_rx_latency_get(int unit, soc_port_t port, int *latency)
{
    int speed = 0;
    SOC_IF_ERROR_RETURN(mac_c_speed_get(unit, port, &speed));

    
    switch (speed) {
    case 1000:  /* GigE */
        *latency = 0;
        break;

    case 10000:  /* 10G */
        *latency = 0;
        break;

    case 100000:  /* 100G */
        *latency = 0;
        break;

    default:
        *latency = 0;
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_control_set
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
mac_c_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    uint64 rval, copy;
    uint32 fval;

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_control_set: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, CMAC_CTRLr, &rval, RX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_CMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, CMAC_CTRLr, &rval, TX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_CMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if (value < 0 || value > 255) {
            return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(READ_CMAC_TX_CTRLr(unit, port, &rval));
            if (value >= 8) {
                soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &rval, THROT_DENOMf,
                                      value);
                soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      1);
            } else {
                soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &rval, THROT_DENOMf,
                                      0);
                soc_reg64_field32_set(unit, CMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      0);
            }
            SOC_IF_ERROR_RETURN(WRITE_CMAC_TX_CTRLr(unit, port, rval));
        }
        return SOC_E_NONE;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(READ_CMAC_RX_CTRLr(unit, port, &rval));
        soc_reg64_field32_set(unit, CMAC_RX_CTRLr, &rval, RX_PASS_CTRLf,
                              value ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_CMAC_RX_CTRLr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CMAC_PFC_TYPEr, port,
                                                   PFC_ETH_TYPEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CMAC_PFC_OPCODEr,
                                                   port, PFC_OPCODEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (value != 8) {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, CMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0x00ffffff;
        fval |= (value & 0xff) << 24;
        soc_reg64_field32_set(unit, CMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

        soc_reg64_field32_set(unit, CMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                              value >> 8);
        SOC_IF_ERROR_RETURN(WRITE_CMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, CMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0xff000000;
        fval |= value;
        soc_reg64_field32_set(unit, CMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);
        SOC_IF_ERROR_RETURN(WRITE_CMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, RX_PASS_PFCf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, RX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, TX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, FORCE_PFC_XONf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, PFC_STATS_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, 
                                    PFC_REFRESH_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_PFC_CTRLr, port, 
                                    PFC_XOFF_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_LLFC_CTRLr, port, RX_LLFC_ENf,
                                    value ? 1 : 0));
        break;
    
    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_LLFC_CTRLr, port, TX_LLFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CMAC_EEE_CTRLr, 
                                                   port, EEE_ENf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CMAC_EEE_TIMERSr, 
                                    port, EEE_DELAY_ENTRY_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CMAC_EEE_TIMERSr, 
                                    port, EEE_WAKE_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_RX_LSS_CTRLr, port,
                                    LOCAL_FAULT_DISABLEf, value ? 0 : 1));
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CMAC_RX_LSS_CTRLr, port,
                                    REMOTE_FAULT_DISABLEf, value ? 0 : 1));
        break;


    case SOC_MAC_CONTROL_FAILOVER_RX_SET:
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_c_control_get
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
mac_c_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  int *value)
{
    int rv;
    uint64 rval;
    uint32 fval0, fval1;

    if (value == NULL) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_CTRLr, rval, RX_ENf);
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_CTRLr, rval, TX_ENf);
        break;
    case SOC_MAC_CONTROL_SW_RESET:
        SOC_IF_ERROR_RETURN(READ_CMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_CTRLr, rval, SOFT_RESETf);
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        SOC_IF_ERROR_RETURN(READ_CMAC_TX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_TX_CTRLr, rval,
                                       THROT_DENOMf);
        rv = SOC_E_NONE;
        break;

    case SOC_MAC_CONTROL_TIMESTAMP_TRANSMIT:
        {
            uint32  timestamp = 0;
            if (soc_feature(unit, soc_feature_timestamp_counter) &&
                soc_property_get(unit, spn_SW_TIMESTAMP_FIFO_ENABLE, 1)) {
                if (soc_counter_timestamp_get(unit, port, &timestamp) !=
                    SOC_E_NOT_FOUND) {
                    *value = (int)timestamp;
                    break;
                }
            }

            SOC_IF_ERROR_RETURN
                (READ_CMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
            if (soc_reg64_field32_get(unit, CMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                      ENTRY_COUNTf) == 0) {
                return SOC_E_EMPTY;
            }
            SOC_IF_ERROR_RETURN
                (READ_CMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
            *value = soc_reg64_field32_get(unit, CMAC_TX_TIMESTAMP_FIFO_DATAr,
                                           rval, TIME_STAMPf);
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(READ_CMAC_RX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_RX_CTRLr, rval,
                                       RX_PASS_CTRLf);
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_TYPEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_TYPEr, rval,
                                       PFC_ETH_TYPEf);
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_OPCODEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_OPCODEr, rval,
                                       PFC_OPCODEf);
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        *value = 8;
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_DAr(unit, port, &rval));
        fval0 = soc_reg64_field32_get(unit, CMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval1 = soc_reg64_field32_get(unit, CMAC_PFC_DAr, rval, PFC_MACDA_HIf);
        *value = (fval0 >> 24) | (fval1 << 8);
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_DAr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_DAr, rval,
                                       PFC_MACDA_LOf) & 0x00ffffff;
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       RX_PASS_PFCf);
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       RX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       TX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       FORCE_PFC_XONf);
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       PFC_STATS_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       PFC_REFRESH_TIMERf);
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN(READ_CMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_PFC_CTRLr, rval,
                                       PFC_XOFF_TIMERf);
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_LLFC_CTRLr, rval,
                                       RX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_LLFC_CTRLr, rval,
                                       TX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, CMAC_EEE_CTRLr, EEE_ENf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_CMAC_EEE_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_EEE_CTRLr, rval, EEE_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, CMAC_EEE_TIMERSr, EEE_DELAY_ENTRY_TIMERf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_CMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_EEE_TIMERSr, rval,
                EEE_DELAY_ENTRY_TIMERf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, CMAC_EEE_TIMERSr, EEE_WAKE_TIMERf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_CMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_EEE_TIMERSr, rval,
                EEE_WAKE_TIMERf);
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_RX_LSS_CTRLr, rval,
                                       LOCAL_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOC_IF_ERROR_RETURN(READ_CMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_RX_LSS_STATUSr, rval,
                                       LOCAL_FAULT_STATUSf);
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_RX_LSS_CTRLr, rval,
                                       REMOTE_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOC_IF_ERROR_RETURN(READ_CMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CMAC_RX_LSS_STATUSr, rval,
                                       REMOTE_FAULT_STATUSf);
        break;
    case SOC_MAC_CONTROL_EXPECTED_RX_LATENCY:
        SOC_IF_ERROR_RETURN(mac_c_expected_rx_latency_get(unit, port, value));;
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_control_get: unit %d port %s type=%d value=%d "
                             "rv=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, *value, rv));
    return rv;
}

/*
 * Function:
 *      mac_c_ability_local_get
 * Purpose:
 *      Return the abilities of CMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_c_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    if (NULL == ability) {
        return SOC_E_PARAM;
    }

    ability->speed_half_duplex  = SOC_PA_ABILITY_NONE;
    ability->pause     = SOC_PA_PAUSE | SOC_PA_PAUSE_ASYMM;
    ability->interface = SOC_PA_INTF_MII | SOC_PA_INTF_XGMII;
    ability->medium    = SOC_PA_ABILITY_NONE;
    ability->loopback  = SOC_PA_LB_MAC;
    ability->flags     = SOC_PA_ABILITY_NONE;
    ability->encap = SOC_PA_ENCAP_IEEE | SOC_PA_ENCAP_HIGIG |
        SOC_PA_ENCAP_HIGIG2;

    if (IS_HG_PORT(unit , port)) {
        switch (SOC_INFO(unit).port_speed_max[port]) {
        case 127000:
            ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
            /* fall through */
        case 120000:
            ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
            /* fall through */
        case 106000:
            ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            /* fall through */
        case 100000:
            ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            /* fall through */
        default:
            break;
        }
    } else {
        switch (SOC_INFO(unit).port_speed_max[port]) {
        case 106000:
            ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            /* fall through */
        case 100000:
            ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            /* fall through */
        default:
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_c_ability_local_get: unit %d port %s "
                             "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                             "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 ability->speed_half_duplex, ability->speed_full_duplex,
                 ability->encap, ability->pause, ability->interface,
                 ability->medium, ability->loopback, ability->flags));
    return SOC_E_NONE;
}

static int
mac_c_timesync_tx_info_get (int unit, soc_port_t port,
                                        soc_port_timesync_tx_info_t *tx_info)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN
        (READ_CMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
    if (soc_reg64_field32_get(unit, CMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                              ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    SOC_IF_ERROR_RETURN
        (READ_CMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
    tx_info->timestamps_in_fifo = soc_reg64_field32_get(unit,
                     CMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);

    tx_info->timestamps_in_fifo_hi = 0;

    tx_info->sequence_id = soc_reg64_field32_get(unit,
                     CMAC_TX_TIMESTAMP_FIFO_DATAr, rval, SEQUENCE_IDf);

    return (SOC_E_NONE);
}


/* Exported CMAC driver structure */
mac_driver_t soc_mac_c = {
    "CMAC Driver",                /* drv_name */
    mac_c_init,                   /* md_init  */
    mac_c_enable_set,             /* md_enable_set */
    mac_c_enable_get,             /* md_enable_get */
    mac_c_duplex_set,             /* md_duplex_set */
    mac_c_duplex_get,             /* md_duplex_get */
    mac_c_speed_set,              /* md_speed_set */
    mac_c_speed_get,              /* md_speed_get */
    mac_c_pause_set,              /* md_pause_set */
    mac_c_pause_get,              /* md_pause_get */
    mac_c_pause_addr_set,         /* md_pause_addr_set */
    mac_c_pause_addr_get,         /* md_pause_addr_get */
    mac_c_loopback_set,           /* md_lb_set */
    mac_c_loopback_get,           /* md_lb_get */
    mac_c_interface_set,          /* md_interface_set */
    mac_c_interface_get,          /* md_interface_get */
    NULL,                         /* md_ability_get - Deprecated */
    mac_c_frame_max_set,          /* md_frame_max_set */
    mac_c_frame_max_get,          /* md_frame_max_get */
    mac_c_ifg_set,                /* md_ifg_set */
    mac_c_ifg_get,                /* md_ifg_get */
    mac_c_encap_set,              /* md_encap_set */
    mac_c_encap_get,              /* md_encap_get */
    mac_c_control_set,            /* md_control_set */
    mac_c_control_get,            /* md_control_get */
    mac_c_ability_local_get,      /* md_ability_local_get */
    mac_c_timesync_tx_info_get    /* md_timesync_tx_info_get */
 };

#endif /* BCM_CMAC_SUPPORT */
