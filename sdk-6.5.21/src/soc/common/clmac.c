/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * CLMAC driver
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/cmic.h>
#include <soc/portmode.h>
#include <soc/ll.h>
#include <soc/macutil.h>
#include <soc/phyctrl.h>
#include <soc/debug.h>
#ifdef BCM_GREYHOUND2_SUPPORT
#include <soc/greyhound2.h>
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
#include <soc/firelight.h>
#endif
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif

#ifdef BCM_CLMAC_SUPPORT

/* Gen version of PM4x25G
 * Gen1:0xA023 include TH, TD2P, Apache, Jericho, Jericho+, QAX, QMX
 * Gen2:0xA030 include TH+, TH2,
 * Gen3:0xA033 include FL
 */
enum clmac_version_e {
    CLMAC_VERSION_A023 = 0xA023,
    CLMAC_VERSION_A030 = 0xA030,
    CLMAC_VERSION_A033 = 0xA033
};

#define CLMAC_RUNT_THRESHOLD_IEEE  0x40   /* Runt threshold value for XE ports */
#define CLMAC_RUNT_THRESHOLD_HG1   0x48   /* Runt threshold value for HG1 ports */
#define CLMAC_RUNT_THRESHOLD_HG2   0x4c   /* Runt threshold value for HG2 ports */
#define CLMAC_RUNT_THRESHOLD_MIN   0x31
#define CLMAC_RUNT_THRESHOLD_MAX   0x60

/*
 * CLMAC Register field definitions.
 */

#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

#define SOC_CLMAC_SPEED_1000   0x2
#define SOC_CLMAC_SPEED_100000 0x4

/*
 * CLMAC timestamp delay definitions
 */
#define CLMAC_TX_LINE_RATE_NS   0x3 /* TSC CLK FREQ is 644MHz */
#define CLMAC_TSC_CLK_NS        4

/* Transmit CRC Modes */
#define CLMAC_CRC_APPEND        0x0
#define CLMAC_CRC_KEEP          0x1
#define CLMAC_CRC_REPLACE       0x2
#define CLMAC_CRC_PER_PKT_MODE  0x3

/*
 *  WAN mode throttling
 *  THROT_10G_TO_5G: throt_num=21 throt_denom=21
 *  THROT_10G_TO_2P5G: throt_num=63 throt_denom=21
 */
#define CLMAC_THROT_10G_TO_5G    256
#define CLMAC_THROT_10G_TO_2P5G  257

mac_driver_t soc_mac_cl;

#ifdef BROADCOM_DEBUG
static char *mac_cl_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *mac_cl_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

STATIC int
_mac_cl_timestamp_delay_set(int unit, soc_port_t port, int speed);
STATIC int
mac_cl_speed_get(int unit, soc_port_t port, int *speed);

STATIC int
_clmac_gen_version_get(int unit, soc_port_t port, int *ver)
{
    int         rv;
    uint64 reg_val;
    rv  = SOC_E_NONE;

    if (!SOC_REG_IS_VALID(unit, CLMAC_VERSION_IDr)) {
        *ver = -1;
    } else {
        SOC_IF_ERROR_RETURN(READ_CLMAC_VERSION_IDr(unit, port, &reg_val));
        *ver = soc_reg64_field32_get(unit, CLMAC_VERSION_IDr, reg_val, CLMAC_VERSIONf);
    }

    return rv;
}

STATIC int
_mac_cl_drain_cells(int unit, soc_port_t port, int notify_phy)
{
    int         rv;
    int         pause_tx = 0, pause_rx = 0, pfc_rx = 0, llfc_rx = 0;
    soc_field_t fields[2];
    uint32      values[2], fval, drain_timeout;
    uint64      mac_ctrl, rval64;
    soc_timeout_t to;

    rv  = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return rv;
    }

    drain_timeout = SAL_BOOT_QUICKTURN ? 250000000 : 250000;
    if (!soc_feature(unit, soc_feature_mmu_hw_flush)) {
        /* Drain cells in mmu/port before cells entering TX FIFO */
        SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, TRUE));

        /* Disable pause/pfc/llfc function */
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_pause_get(unit, port, &pause_tx, &pause_rx));
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_pause_set(unit, port, pause_tx, 0));

        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_get(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                       &pfc_rx));
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                       0));

        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_get(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                       &llfc_rx));
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                       0));
    }

    if (!soc_feature(unit, soc_feature_mmu_hw_flush)) {
        /* Assert SOFT_RESET before DISCARD just in case there is no credit left */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 1));
        }
#endif

        SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 1);
        /* Drain data in TX FIFO without egressing */
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, mac_ctrl));
    }

    fields[0] = DISCARDf;
    values[0] = 1;
    fields[1] = EP_DISCARDf;
    values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, CLMAC_TX_CTRLr, port, 2,
                                                fields, values));

    /* Reset EP credit before de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));

    if (!soc_feature(unit, soc_feature_mmu_hw_flush)) {
        /* De-assert SOFT_RESET to let the drain start */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
        }
#endif
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, mac_ctrl));
    }

    if (notify_phy == 1) {
        /* Notify PHY driver */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventStop, PHY_STOP_DRAIN));
    }

    /* Wait until mmu cell count is 0 */
    rv = soc_egress_drain_cells(unit, port, drain_timeout);
    if (SOC_E_NONE == rv) {
        /* Wait until TX fifo cell count is 0 */
        soc_timeout_init(&to, drain_timeout, 0);
        for (;;) {
            rv =  READ_CLMAC_TXFIFO_CELL_CNTr(unit, port, &rval64);
            if (SOC_E_NONE != rv) {
                break;
            }
            fval = soc_reg64_field32_get(unit, CLMAC_TXFIFO_CELL_CNTr, rval64,
                                         CELL_CNTf);
            if (fval == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                "mac_cl_drain_cells: unit %d "
                                "ERROR: port %s: "
                                "timeout draining TX FIFO (%d cells remain)\n"),
                                unit, SOC_PORT_NAME(unit, port), fval));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    if (notify_phy == 1) {
        /* Notify PHY driver */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventResume, PHY_STOP_DRAIN));
    }
  
    /* Stop TX FIFO draining */
    values[0] = 0;
    values[1] = 0;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, CLMAC_TX_CTRLr, port, 2,
                                                fields, values));
    /* Restore config below only if modified above, i.e. h/w flush not used */
    if (!soc_feature(unit, soc_feature_mmu_hw_flush)) {
        /* Restore original pause/pfc/llfc configuration */
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_pause_set(unit, port, pause_tx, pause_rx));
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_set(unit, port,
                                       SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                       pfc_rx));
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_set(unit, port,
                                       SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                       llfc_rx));
    
        /* Stop draining cells in mmu/port */
        SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));
    }

    return rv;
}

/*
 * Function:
 *      mac_cl_init
 * Purpose:
 *      Initialize Clmac into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_cl_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    uint64 mac_ctrl, rx_ctrl, tx_ctrl, rval;
    int ipg, runt;
    int mode;
    int encap = SOC_ENCAP_IEEE;


    si = &SOC_INFO(unit);

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &mac_ctrl));

    /* Reset EP credit before de-assert SOFT_RESET */
    if (soc_reg64_field32_get(unit, CLMAC_CTRLr, mac_ctrl, SOFT_RESETf)) {
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
    }

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
    }
#endif

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    if (soc_reg_field_valid(unit, CLMAC_CTRLr, XLGMII_ALIGN_ENBf)) {
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, XLGMII_ALIGN_ENBf,
                              1);
    }
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl,
                          XGMII_IPG_CHECK_DISABLEf,
                          IS_HG_PORT(unit, port) ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, mac_ctrl));

    SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &tx_ctrl));
    ipg = IS_HG_PORT(unit,port)?  SOC_PERSIST(unit)->ipg[port].fd_hg:
                                  SOC_PERSIST(unit)->ipg[port].fd_xe;
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf,
                          (ipg / 8) & 0x1f);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf,
                          CLMAC_CRC_PER_PKT_MODE);
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &tx_ctrl, TX_THRESHOLDf, 4);
    }
#endif
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_TX_CTRLr(unit, port, tx_ctrl));

    if (IS_ST_PORT(unit, port)) {
        soc_mac_cl.md_pause_set(unit, port, FALSE, FALSE);
    } else {
        soc_mac_cl.md_pause_set(unit, port, TRUE, TRUE);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, PFC_REFRESH_ENf,
                                1));

    if (soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE)) {
        /* Max speed for WAN mode is 9.294Gbps.
         * This setting gives 10Gbps * (13/14) or 9.286 Gbps */
        SOC_IF_ERROR_RETURN
            (soc_mac_cl.md_control_set(unit, port,
                                       SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
                                       13));
    }

    /* Set jumbo max size (8000 byte payload) */
    COMPILER_64_ZERO(rval);
#ifdef BCM_XGS_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint32 rval;
        soc_reg_t reg;

        reg = SOC_REG_IS_VALID(unit, EGR_MTUr) ? EGR_MTUr : EGR_MTU_SIZEr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, MTU_SIZEf, SOC_INFO(unit).max_mtu);
        if (soc_reg_field_valid(unit, reg, MTU_ENABLEf)) {
            soc_reg_field_set(unit, reg, &rval, MTU_ENABLEf, 1);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    if (SOC_IS_XGS(unit)) {
        soc_reg64_field32_set(unit, CLMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                              SOC_INFO(unit).max_mtu);
    } else
#endif
    {
        soc_reg64_field32_set(unit, CLMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                              JUMBO_MAXSZ);
    }
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_RX_MAX_SIZEr(unit, port, rval));

    /* Setup header mode, check for property for higig2 mode */
    SOC_IF_ERROR_RETURN(READ_CLMAC_MODEr(unit, port, &rval));
    if (IS_HG_PORT(unit, port)) {
        mode = soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE,
               soc_feature(unit, soc_feature_no_higig_plus) ? 1 : 0) ? 2 : 1;
        encap = (mode == 2) ? SOC_ENCAP_HIGIG2 : SOC_ENCAP_HIGIG;
        soc_reg64_field32_set(unit, CLMAC_MODEr, &rval, HDR_MODEf, mode);
    }
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_MODEr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CLMAC_RX_CTRLr(unit, port, &rx_ctrl));
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 0);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rx_ctrl, STRICT_PREAMBLEf,
                          si->port_speed_max[port] >= 10000 &&
                          IS_CL_PORT(unit, port) &&
                          !IS_HG_PORT(unit, port) ? 1 : 0);
    /* assigning RUNT_THRESHOLD per header mode setting */
    runt = (encap == SOC_ENCAP_HIGIG2) ? CLMAC_RUNT_THRESHOLD_HG2 :
           ((encap == SOC_ENCAP_HIGIG) ? CLMAC_RUNT_THRESHOLD_HG1 :
                                         CLMAC_RUNT_THRESHOLD_IEEE);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf,
                          runt);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_RX_CTRLr(unit, port, rx_ctrl));

    SOC_IF_ERROR_RETURN(READ_CLMAC_RX_LSS_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LOCAL_FAULTf, 1);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_REMOTE_FAULTf, 1);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LINK_INTERRUPTf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_RX_LSS_CTRLr(unit, port, rval));

    /* Disable loopback and bring CLMAC out of reset */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, TX_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, mac_ctrl));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_egress_queue_drain
 * Purpose:
 *      Drain the egress queues with out bringing down the port
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_egress_queue_drain(int unit, soc_port_t port)
{
    uint64 ctrl, octrl;
    pbmp_t mask;
    int rx_enable = 0;
    int is_active = 0;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_cl_egress_queue_drain: unit %d port %s \n"),
                 unit, SOC_PORT_NAME(unit, port)));

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;

    rx_enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, RX_ENf);
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, 0);
        /* Disable RX */
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));

        /* Remove port from EPC_LINK */
    soc_link_mask2_get(unit, &mask);
    if (SOC_PBMP_MEMBER(mask, port)) {
        is_active = 1;
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }

        /* Drain cells */
    SOC_IF_ERROR_RETURN(_mac_cl_drain_cells(unit, port, 0));

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 1));
    }
#endif

        /* Put port into SOFT_RESET */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));

        /* Reset EP credit before de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));

#ifdef BCM_FIRELIGHT_SUPPORT
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
    }
#endif

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, rx_enable ? 1 : 0);

      /* Enable both TX and RX, deassert SOFT_RESET */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));

        /* Restore CLMAC_CTRL to original value */
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, octrl));        

        /* Add port to EPC_LINK */
    if(is_active) {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_enable_set
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
mac_cl_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 ctrl, octrl;
    pbmp_t mask;
    int speed = 1000;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_enable_set: unit %d port %s enable=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            enable ? "True" : "False"));

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        if (enable) {
            return SOC_E_NONE;
        } else {
            if (soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, SOFT_RESETf)) {
                return SOC_E_NONE;
            }
        }
    }


    if (enable) {
        /* Reset EP credit before de-assert SOFT_RESET */
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
        /* Deassert SOFT_RESET */
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
        }
#endif

        soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
        /* Deassert EGR_XLPORT_BUFFER_SFT_RESET */
        SOC_IF_ERROR_RETURN(soc_port_egress_buffer_sft_reset(unit, port, 0));
        /* Release Ingress buffers from reset */
        SOC_IF_ERROR_RETURN(soc_port_ingress_buffer_reset(unit, port, 0));
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));

#if defined(BCM_TOMAHAWKPLUS_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        /* Special handling for new mac version for TH+. Internally MAC loopback looks for rising */
        /* edge on MAC loopback configuration to enter loopback state */
        if (SOC_IS_TOMAHAWKPLUS(unit) || SOC_IS_GREYHOUND2(unit)) {
            uint32 ctrl_32;
            COMPILER_64_TO_32_LO(ctrl_32, ctrl);
            if (ctrl_32 & 0x4) {      /* Do only if loopback bit is intented to be set */
                soc_reg_field32_modify(unit, CLMAC_CTRLr, port, LOCAL_LPBKf, 0);
                sal_usleep(10);    /* Wait 10usec as suggested by MAC designer */
                soc_reg_field32_modify(unit, CLMAC_CTRLr, port, LOCAL_LPBKf, 1);
            }
        }
#endif /* TOMAHAWKPLUS || GREYHOUHD2 */

        /* Enable MMU port */
        SOC_IF_ERROR_RETURN(soc_port_mmu_buffer_enable(unit, port, TRUE));

        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventResume,
                                               PHY_STOP_MAC_DIS));

        /* set timestamp adjust delay */
        SOC_IF_ERROR_RETURN(mac_cl_speed_get(unit, port, &speed));
        SOC_IF_ERROR_RETURN(_mac_cl_timestamp_delay_set(unit, port, speed));

    } else {
        /* Disable MAC RX */
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        /*
         * Disable MMU port
         * for some devices where EPC_LINK can not block the SOBMH from cpu and
         * no mechanism is avalible to reset EDATABUF.
         */
        SOC_IF_ERROR_RETURN(soc_port_mmu_buffer_enable(unit, port, FALSE));

        /* Delay to ensure EOP is received at Ingress */
        sal_udelay(1000);
        /* Reset Ingress buffers */
        SOC_IF_ERROR_RETURN(soc_port_ingress_buffer_reset(unit, port, 1));
        SOC_IF_ERROR_RETURN(_mac_cl_drain_cells(unit, port, 1));
        /* Reset egress_buffer */
        SOC_IF_ERROR_RETURN(soc_port_egress_buffer_sft_reset(unit, port, 1));
        /* Put port into SOFT_RESET */
#if defined (BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 1));
        }
#endif

        soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 1); 
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventStop,
                                               PHY_STOP_MAC_DIS));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_enable_get
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
mac_cl_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));

    *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, RX_ENf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_enable_get: unit %d port %s enable=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            *enable ? "True" : "False"));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _mac_cl_timestamp_delay_set
 * Purpose:
 *      Set Timestamp delay for one-step to account for lane and pipeline delay.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      speed - New port speed
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_cl_timestamp_delay_set(int unit, soc_port_t port, int speed)
{
    uint64 ctrl;
    int osts_delay = 0;
    uint32 tsts_delay = 0;
    uint32 field;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                 "mac_cl_timestamp_delay_set: unit %d port %s speed %d\n"),
                 unit, SOC_PORT_NAME(unit, port), speed));

    if (SOC_REG_IS_VALID(unit, CLMAC_TIMESTAMP_ADJUSTr)) {
        SOC_IF_ERROR_RETURN(READ_CLMAC_TIMESTAMP_ADJUSTr(unit, port, &ctrl));

        /*
         * CLMAC pipeline latency is : ( as in CLMAC specification)
         *          = (6 * TX line clock period + Timestamp clock period)
         */
        
        
        /* This is a signed value of pipeline delay in ns */
        osts_delay = soc_property_port_get(unit, port, spn_TIMESTAMP_ADJUST(speed),
                                           CLMAC_TSC_CLK_NS +
                                           (3 * CLMAC_TX_LINE_RATE_NS));

        if (SOC_E_NONE != soc_reg_signed_field_mask(unit, CLMAC_TIMESTAMP_ADJUSTr,
                              TS_OSTS_ADJUSTf, osts_delay, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "%s osts property out of bounds (is %d)\n"),
                spn_TIMESTAMP_ADJUST(speed), osts_delay));
            field = 0;
        }
        soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_OSTS_ADJUSTf, field);

        /* Note:
           TSTS delay uses different SOC property as OSTS delay, in order to
           allow specification based on platform.  However, the resulting timestamps
           in the FIFO are only used by the application, so the application can make
           other timestamp adjustments as required.
           CLMAC CDC FIFO latency is :
                   = (2.5 * TX line clock period + Timestamp clock period)
           This is an unsigned value and the range is 0~63.
        */
        tsts_delay = soc_property_port_get(unit, port, spn_TIMESTAMP_TSTS_ADJUST(speed),
                                           CLMAC_TSC_CLK_NS +
                                           ((5 * CLMAC_TX_LINE_RATE_NS) / 4));

        if (SOC_E_NONE != soc_reg_unsigned_field_mask(unit, CLMAC_TIMESTAMP_ADJUSTr,
                              TS_TSTS_ADJUSTf, tsts_delay, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "%s tsts property out of bounds (is %d)\n"),
                spn_TIMESTAMP_TSTS_ADJUST(speed), tsts_delay));
            field = 0;
        }
        soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_TSTS_ADJUSTf, field);


        SOC_IF_ERROR_RETURN(WRITE_CLMAC_TIMESTAMP_ADJUSTr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_duplex_set
 * Purpose:
 *      Set CLMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_cl_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_duplex_set: unit %d port %s duplex=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_duplex_get
 * Purpose:
 *      Get CLMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_duplex_get: unit %d port %s duplex=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_pause_set
 * Purpose:
 *      Configure CLMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    soc_field_t fields[2] = { TX_PAUSE_ENf, RX_PAUSE_ENf };
    uint32 values[2];

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_pause_set: unit %d port %s TX=%s RX=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            pause_tx != FALSE ? "on" : "off",
                            pause_rx != FALSE ? "on" : "off"));

    values[0] = pause_tx != FALSE ? 1 : 0;
    values[1] = pause_rx != FALSE ? 1 : 0;
    return soc_reg_fields32_modify(unit, CLMAC_PAUSE_CTRLr, port, 2,
                                   fields, values);
}

/*
 * Function:
 *      mac_cl_pause_get
 * Purpose:
 *      Return the pause ability of CLMAC
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
mac_cl_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_PAUSE_CTRLr(unit, port, &rval));
    *pause_tx =
        soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, rval, TX_PAUSE_ENf);
    *pause_rx =
        soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, rval, RX_PAUSE_ENf);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_pause_get: unit %d port %s TX=%s RX=%s\n"),
                             unit, SOC_PORT_NAME(unit, port),
                             *pause_tx ? "on" : "off",
                             *pause_rx ? "on" : "off"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_speed_set
 * Purpose:
 *      Set CLMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 100000, 120000.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_speed_set(int unit, soc_port_t port, int speed)
{
    int enable;
    uint32 rval;
    uint32 fault;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_speed_set: unit %d port %s speed=%dMb\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            speed));

    SOC_IF_ERROR_RETURN(mac_cl_enable_get(unit, port, &enable));
        
    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_cl_enable_set(unit, port, 0));
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CLMAC_RX_CTRLr, port, STRICT_PREAMBLEf,
                                speed >= 10000 &&
                                IS_CL_PORT(unit, port) &&
                                !IS_HG_PORT(unit, port) ? 1 : 0));
    /*
     * Set REMOTE_FAULT/LOCAL_FAULT for CL ports,
     * else HW Linkscan interrupt would be suppressed.
     */
    if (SOC_REG_IS_VALID(unit, CLPORT_FAULT_LINK_STATUSr)) {
        rval = 0;
        fault = speed <= 1000 ? 0 : 1;

        soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &rval,
                          REMOTE_FAULTf, fault);
        soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &rval,
                          LOCAL_FAULTf, fault);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_FAULT_LINK_STATUSr(unit,
                            port, rval));
    }

    /* Update port speed related setting in components other than MAC/SerDes*/
    SOC_IF_ERROR_RETURN(soc_port_speed_update(unit, port, speed));

    /*
     * Notify internal PHY driver of speed change in case it is being
     * used as pass-through to an external PHY.
     */
    if (!PHY_REPEATER(unit, port)) {
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventSpeed, speed));
    }

    if (enable) {
        /* Re-enable transmitter and receiver */
        SOC_IF_ERROR_RETURN(mac_cl_enable_set(unit, port, 1));
    }

    /* Set Timestamp Mac Delays */
    SOC_IF_ERROR_RETURN(_mac_cl_timestamp_delay_set(unit, port, speed));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_speed_get
 * Purpose:
 *      Get CLMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval64;
    soc_error_t rc;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MODEr(unit, port, &rval64));
    switch (soc_reg64_field32_get(unit, CLMAC_MODEr, rval64, SPEED_MODEf)) {
    case SOC_CLMAC_SPEED_1000:
        *speed = 1000;
        break;
    case SOC_CLMAC_SPEED_100000:
    default:
        /* Obtain fine grained port speed, since
         * SOC_CLMAC_SPEED_1000000 implies >= 10Gbps
         */
        rc = soc_granular_speed_get(unit, port, speed);
        if (SOC_FAILURE(rc)) {
            /* Error determining speed, set default value */
            *speed = 100000;
        }
        break;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_speed_get: unit %d port %s speed=%dMb\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_loopback_set
 * Purpose:
 *      Set a CLMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On Clmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_loopback_set(int unit, soc_port_t port, int lb)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_loopback_set: unit %d port %s loopback=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            lb ? "local" : "no"));

    /* need to enable clock compensation for applicable serdes device */
    (void)soc_phyctrl_notify(unit, port, phyEventMacLoopback, lb? 1: 0);

    return soc_reg_field32_modify(unit, CLMAC_CTRLr, port, LOCAL_LPBKf,
                                  lb ? 1 : 0);

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_loopback_get
 * Purpose:
 *      Get current CLMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));

    *lb = soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, LOCAL_LPBKf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_loopback_get: unit %d port %s loopback=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            *lb ? "local" : "no"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_pause_addr_set
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
mac_cl_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    static soc_field_t fields[2] = { SA_HIf, SA_LOf };
    uint32 values[2];

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_pause_addr_set: unit %d port %s MAC=<"
                            "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));

    values[0] = (mac[0] << 8) | mac[1];
    values[1] = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];

    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, CLMAC_TX_MAC_SAr, port, 2, fields,
                                 values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, CLMAC_RX_MAC_SAr, port, 2, fields,
                                 values));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_pause_addr_get
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
mac_cl_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];

    SOC_IF_ERROR_RETURN(READ_CLMAC_RX_MAC_SAr(unit, port, &rval64));
    values[0] = soc_reg64_field32_get(unit, CLMAC_RX_MAC_SAr, rval64, SA_HIf);
    values[1] = soc_reg64_field32_get(unit, CLMAC_RX_MAC_SAr, rval64, SA_LOf);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_pause_addr_get: unit %d port %s MAC=<"
                            "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_interface_set
 * Purpose:
 *      Set a CLMAC interface type
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
mac_cl_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_interface_set: unit %d port %s interface=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            mac_cl_port_if_names[pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_interface_get
 * Purpose:
 *      Retrieve CLMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_cl_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_CGMII;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_interface_get: unit %d port %s interface=%s\n"),
                             unit, SOC_PORT_NAME(unit, port),
                             mac_cl_port_if_names[*pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_frame_max_set
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
mac_cl_frame_max_set(int unit, soc_port_t port, int size)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_frame_max_set: unit %d port %s size=%d\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            size));

    if (IS_CE_PORT(unit, port) || IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        size += 4;
    }
    return soc_reg_field32_modify(unit, CLMAC_RX_MAX_SIZEr, port, RX_MAX_SIZEf,
                                  size);
}

/*
 * Function:
 *      mac_cl_frame_max_get
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
mac_cl_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_CLMAC_RX_MAX_SIZEr(unit, port, &rval));
    *size = soc_reg64_field32_get(unit, CLMAC_RX_MAX_SIZEr, rval, RX_MAX_SIZEf);
    if (IS_CE_PORT(unit, port) || IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        *size -= 4;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_frame_max_get: unit %d port %s size=%d\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            *size));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_ifg_set
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
mac_cl_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int ifg)
{
    int         cur_speed;
    int         cur_duplex;
    int         real_ifg;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    uint64      rval, orval;
    soc_port_ability_t ability;
    uint32      pa_flag;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_ifg_set: unit %d port %s speed=%dMb duplex=%s "
                            "ifg=%d\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            speed, duplex ? "True" : "False", ifg));

    pa_flag = SOC_PA_SPEED(speed); 
    sal_memset(&ability, 0, sizeof(ability));
    soc_mac_cl.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 64 bytes (i.e. multiple of 8 bits) */
    real_ifg = ifg < 64 ? 64 : (ifg > 512 ? 512 : (ifg + 7) & (0x7f << 3));

    if (IS_CE_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        si->fd_xe = real_ifg;
    } else {
        si->fd_hg = real_ifg;
    }

    SOC_IF_ERROR_RETURN(mac_cl_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_cl_speed_get(unit, port, &cur_speed));

    /* CLMAC_MODE supports only 4 speeds with 4 being max as LINK_10G_PLUS */
    /* Unlike the corresponding XLMAC function call, mac_cl_speed_get()
     * returns a fine grained speed value when CLMAC_MODE.SPEEDf=LINK_10G_PLUS
     * Hence the check below uses cur_speed >= 10000, unlike the
     * cur_speed == 10000 check used in xlmac */
    if ((speed >= 10000) && (cur_speed >= 10000)) {
        cur_speed = speed;
    }

    if (cur_speed == speed &&
        cur_duplex == (duplex == SOC_PORT_DUPLEX_FULL ? TRUE : FALSE)) {
        SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &rval));
        orval = rval;
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, AVERAGE_IPGf,
                              real_ifg / 8);
        if (COMPILER_64_NE(rval, orval)) {
            SOC_IF_ERROR_RETURN(WRITE_CLMAC_TX_CTRLr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_ifg_get
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
mac_cl_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    soc_port_ability_t ability;
    uint32      pa_flag;

    if (!duplex) {
        return SOC_E_PARAM;
    }

    pa_flag = SOC_PA_SPEED(speed); 
    sal_memset(&ability, 0, sizeof(ability));
    soc_mac_cl.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    if (IS_CE_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        *ifg = si->fd_xe;
    } else {
        *ifg = si->fd_hg;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_ifg_get: unit %d port %s speed=%dMb duplex=%s "
                            "ifg=%d\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            speed, duplex ? "True" : "False", *ifg));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _mac_cl_port_mode_update
 * Purpose:
 *      Set the CLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      hg_mode - SOC HG encap modes. value={HG/HG2/HG2-LITE}
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_cl_port_mode_update(int unit, soc_port_t port, int hg_mode)
{
    uint32              rval;
    uint64              val64, rval64, ctrl;
    soc_pbmp_t          ctr_pbmp;
    int                 speed = 0;
    int                 rv = SOC_E_NONE;
    int                 to_hg_port = 0 ;
    int                 phy_enable = 0;
    int                 encap_mode = 0;
    int                 en_able = 0;

    /* The current encap mode */
    SOC_IF_ERROR_RETURN(READ_CLMAC_MODEr(unit, port, &rval64));
    encap_mode = soc_reg64_field32_get(unit, CLMAC_MODEr, rval64, HDR_MODEf);

    /* The current port enable status */
    SOC_IF_ERROR_RETURN(mac_cl_enable_get(unit, port, &en_able));

    /* Pause linkscan */
    soc_linkscan_pause(unit);

    /* Pause counter collection */
    COUNTER_LOCK(unit);

    /* for original hg_mode parameter is at value={TRUE|FALSE} */
    to_hg_port = (hg_mode != SOC_ENCAP_IEEE) ? TRUE : FALSE;
    if (soc_feature(unit, soc_feature_hg2_light_in_portmacro)) {
        soc_xport_type_mode_update(unit, port, hg_mode);
    } else {
        soc_xport_type_update(unit, port, to_hg_port);
    }

    if (to_hg_port) {
        SOC_IF_ERROR_RETURN(soc_port_hg_speed_get(unit, port, &speed));
        SOC_IF_ERROR_RETURN(soc_phyctrl_set_speed_max(unit, port, speed));
    } else {
        speed = SOC_INFO(unit).port_init_speed[port];
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit)) {
            SOC_IF_ERROR_RETURN(soc_th_port_ie_speed_get(unit, port, &speed));
        }
#endif
        SOC_IF_ERROR_RETURN(soc_phyctrl_set_speed_max(unit, port, speed));
    }
    rv = soc_phyctrl_enable_get(unit, port, &phy_enable);

    if (SOC_SUCCESS(rv)) {
        rv = soc_phyctrl_init(unit, port);
    }

    if (soc_feature(unit, soc_feature_port_enable_encap_restore)){
        if (SOC_SUCCESS(rv)) {
            rv = soc_phyctrl_enable_set(unit, port, phy_enable);
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_cl_init(unit, port);
    } else {
        goto  _failed;
    }

    if (SOC_SUCCESS(rv)) {
        if (soc_feature(unit, soc_feature_disable_rx_on_port) && (0 == en_able)) {
            SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &ctrl));
            /* Enable MAC TX if CPU sends packet out */
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
            /* Disable MAC RX */
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, 0);
            /* Put port into SOFT_RESET */
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, SOFT_RESETf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, ctrl));
        } else {
            rv = mac_cl_enable_set(unit, port, 0);
        }
    } else {
        goto _failed;
    }

    if (SOC_SUCCESS(rv)) {
        SOC_PBMP_CLEAR(ctr_pbmp);
        SOC_PBMP_PORT_SET(ctr_pbmp, port);
        COMPILER_64_SET(val64, 0, 0);
        rv = soc_counter_set_by_port(unit, ctr_pbmp, val64);
    } else {
        goto _failed;
    }

    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit);
    rval = 0;
    
    if (SOC_REG_IS_VALID(unit, CLPORT_CONFIGr)) {

    SOC_IF_ERROR_RETURN(READ_CLPORT_CONFIGr(unit, port, &rval));
#ifdef BCM_GREYHOUND2_SUPPORT
        if (SOC_IS_GREYHOUND2(unit)) {
            uint32  clp_hg_mode;

            /*
             * CLPORT and PGW register
             *  - for HIGIG_MODEf :
             *    will be '1' for HG/HG+ setting only and '0' for other
             *    encap modes{IEEE/HG2/HG2-LITE}
             *  - for HGIGIG2_MODEf :
             *    will be 1 for HG2 and will be 0 for other
             *    encap modes{IEEE/HG/HG+/HG2-LITE}
             */
            clp_hg_mode = (hg_mode == SOC_ENCAP_HIGIG) ? 1 : 0;
#ifdef BCM_FIRELIGHT_SUPPORT
            if (soc_feature(unit, soc_feature_fl)) {
                SOC_IF_ERROR_RETURN
                        (soc_firelight_pgw_encap_field_modify(unit, port,
                                                               HIGIG_MODEf,
                                                               clp_hg_mode));
            } else
#endif /* BCM_FIRELIGHT_SUPPORT */
            {
                SOC_IF_ERROR_RETURN
                        (soc_greyhound2_pgw_encap_field_modify(unit, port,
                                                               HIGIG_MODEf,
                                                               clp_hg_mode));
            }

            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG_MODEf,
                              clp_hg_mode);

            clp_hg_mode = (hg_mode == SOC_ENCAP_HIGIG2) ? 1 : 0;
            soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG2_MODEf,
                              clp_hg_mode);
        } else 
#endif  /* GREYHOUND2 */
        {
    soc_reg_field_set(unit, CLPORT_CONFIGr, &rval, HIGIG2_MODEf,
                      to_hg_port);
        }

    SOC_IF_ERROR_RETURN(WRITE_CLPORT_CONFIGr(unit, port, rval));
    }

_failed:
    if (SOC_FAILURE(rv)) {
        COUNTER_UNLOCK(unit);
        soc_linkscan_continue(unit);
        if (soc_feature(unit, soc_feature_hg2_light_in_portmacro)) {
            soc_xport_type_mode_update(unit, port, encap_mode);
        } else {
            soc_xport_type_update(unit, port, encap_mode);
        }
        SOC_IF_ERROR_RETURN(READ_CLMAC_MODEr(unit, port, &rval64));
        soc_reg64_field32_set(unit, CLMAC_MODEr, &rval64, HDR_MODEf, encap_mode);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_MODEr(unit, port, rval64));
    }

    return rv;
}

/*
 * Function:
 *      mac_cl_encap_set
 * Purpose:
 *      Set the CLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_encap_set(int unit, soc_port_t port, int mode)
{
    int enable, encap, rv = SOC_E_NONE;
    int runt;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_encap_set: unit %d port %s encapsulation=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            mac_cl_encap_mode[mode]));
#endif

    switch (mode) {
    case SOC_ENCAP_IEEE:
    case SOC_ENCAP_HIGIG2_LITE:
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

    SOC_IF_ERROR_RETURN(mac_cl_enable_get(unit, port, &enable));
    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_cl_enable_set(unit, port, 0));
    }

    if (soc_feature(unit, soc_feature_hg2_light_in_portmacro)) {
        /* mode update for all encap mode change! */
        SOC_IF_ERROR_RETURN(_mac_cl_port_mode_update(unit, port, mode));
    } else if ((IS_E_PORT(unit, port) && mode != SOC_ENCAP_IEEE) || 
            (IS_ST_PORT(unit, port) && mode == SOC_ENCAP_IEEE)) {
        /* IEEE -> HG or HG -> IEEE */
        SOC_IF_ERROR_RETURN(_mac_cl_port_mode_update(unit, port, mode));
    }
    
    /* Update the encapsulation mode */
    rv = soc_reg_field32_modify(unit, CLMAC_MODEr, port, HDR_MODEf, encap);

    runt = (mode == SOC_ENCAP_HIGIG2) ? CLMAC_RUNT_THRESHOLD_HG2 :
           ((mode == SOC_ENCAP_HIGIG) ? CLMAC_RUNT_THRESHOLD_HG1 :
                                        CLMAC_RUNT_THRESHOLD_IEEE);
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_RX_CTRLr, port, RUNT_THRESHOLDf, runt));
    
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, CLMAC_RX_CTRLr, port, STRICT_PREAMBLEf,
                                mode == SOC_ENCAP_IEEE ? 1 : 0));

        if (enable) {
            /* Re-enable transmitter and receiver */
            SOC_IF_ERROR_RETURN(mac_cl_enable_set(unit, port, 1));
        }

    return rv;
}

/*
 * Function:
 *      mac_cl_encap_get
 * Purpose:
 *      Get the CLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_encap_get(int unit, soc_port_t port, int *mode)
{
    uint64 rval;

    if (!mode) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_CLMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, CLMAC_MODEr, rval, HDR_MODEf)) {
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
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_encap_get: unit %d port %s encapsulation=%s\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            mac_cl_encap_mode[*mode]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_expected_rx_latency_get
 * Purpose:
 *      Get the CLMAC port expected Rx latency
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      latency - (OUT) Latency in NS
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_expected_rx_latency_get(int unit, soc_port_t port, int *latency)
{
    int speed = 0;
    SOC_IF_ERROR_RETURN(mac_cl_speed_get(unit, port, &speed));

    
    switch (speed) {
    case 1000:  /* GigE */
        *latency = 0;
        break;

    case 10000:  /* 10G */
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
 *      mac_cl_control_set
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
mac_cl_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    uint64 rval, copy;
    uint32 fval;
    int    ver;

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_control_set: unit %d port %s type=%d value=%d\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            type, value));

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval, RX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_CLMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if (value < 0 || value > CLMAC_THROT_10G_TO_2P5G) {
            return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &rval));
            if (value == CLMAC_THROT_10G_TO_5G) {
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_DENOMf, 21);
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_NUMf, 21);
            } else if (value == CLMAC_THROT_10G_TO_2P5G) {
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_DENOMf, 21);
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_NUMf, 63);
            } else if (value >= 8) {
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_DENOMf,
                                      value);
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      1);
            } else {
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_DENOMf,
                                      0);
                soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      0);
            }
            SOC_IF_ERROR_RETURN(WRITE_CLMAC_TX_CTRLr(unit, port, rval));
        }
        return SOC_E_NONE;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(_clmac_gen_version_get(unit, port, &ver));
        if ((ver == CLMAC_VERSION_A030) || (ver == CLMAC_VERSION_A033)) {
            SOC_IF_ERROR_RETURN(READ_CLMAC_RX_CTRLr(unit, port, &rval));
            soc_reg64_field32_set (unit, CLMAC_RX_CTRLr, &rval,
                    RX_PASS_PAUSEf, (value? 1:0));
            SOC_IF_ERROR_RETURN(WRITE_CLMAC_RX_CTRLr(unit, port, rval));
        }
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CLMAC_PFC_TYPEr, port,
                                                   PFC_ETH_TYPEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CLMAC_PFC_OPCODEr,
                                                   port, PFC_OPCODEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (value != 8) {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0x00ffffff;
        fval |= (value & 0xff) << 24;
        soc_reg64_field32_set(unit, CLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

        soc_reg64_field32_set(unit, CLMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                              value >> 8);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0xff000000;
        fval |= value;
        soc_reg64_field32_set(unit, CLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        if (!SOC_REG_FIELD_VALID(unit, CLMAC_PFC_CTRLr, RX_PASS_PFCf)) {
            return SOC_E_UNAVAIL;
        }

        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, RX_PASS_PFCf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, RX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, TX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, FORCE_PFC_XONf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, PFC_STATS_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, 
                                    PFC_REFRESH_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_PFC_CTRLr, port, 
                                    PFC_XOFF_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_LLFC_CTRLr, port, RX_LLFC_ENf,
                                    value ? 1 : 0));
        
        /* Feature check inside the soc_llfc_xon_set() should take care
         * of devices supporting this functionality.
         */
        SOC_IF_ERROR_RETURN 
            (soc_llfc_xon_set(unit, port, value ? 1 : 0));
        break;
    
    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_LLFC_CTRLr, port, TX_LLFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CLMAC_EEE_CTRLr, 
                                                   port, EEE_ENf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CLMAC_EEE_TIMERSr, 
                                    port, EEE_DELAY_ENTRY_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, CLMAC_EEE_TIMERSr, 
                                    port, EEE_WAKE_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_RX_LSS_CTRLr, port,
                                    LOCAL_FAULT_DISABLEf, value ? 0 : 1));
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_RX_LSS_CTRLr, port,
                                    REMOTE_FAULT_DISABLEf, value ? 0 : 1));
        break;

    case SOC_MAC_CONTROL_EGRESS_DRAIN:
        SOC_IF_ERROR_RETURN(mac_cl_egress_queue_drain(unit, port));
        break;

    case SOC_MAC_CONTROL_FAILOVER_RX_SET:
        break;
    case SOC_MAC_CONTROL_RX_RUNT_THRESHOLD:
        if ((value < CLMAC_RUNT_THRESHOLD_MIN) ||
            (value > CLMAC_RUNT_THRESHOLD_MAX)) {
            return SOC_E_PARAM;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, CLMAC_RX_CTRLr, port,
                                    RUNT_THRESHOLDf, value));
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_cl_control_get
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
mac_cl_control_get(int unit, soc_port_t port, soc_mac_control_t type,
                  int *value)
{
    int rv;
    uint64 rval;
    uint32 fval0, fval1;
    int    ver;

    if (value == NULL) {
        return SOC_E_PARAM;
    }

    rv = SOC_E_NONE;
    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_CTRLr, rval, RX_ENf);
        break;
    case SOC_MAC_CONTROL_SW_RESET:
        SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_CTRLr, rval, SOFT_RESETf);
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        SOC_IF_ERROR_RETURN(READ_CLMAC_TX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_TX_CTRLr, rval,
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
            (READ_CLMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
        if (soc_reg64_field32_get(unit, CLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                  ENTRY_COUNTf) == 0) {
            return SOC_E_EMPTY;
        }
        SOC_IF_ERROR_RETURN
            (READ_CLMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_TX_TIMESTAMP_FIFO_DATAr,
                                       rval, TIME_STAMPf);
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        *value = TRUE;
        SOC_IF_ERROR_RETURN(_clmac_gen_version_get(unit, port, &ver));
        if ((ver == CLMAC_VERSION_A030) || (ver == CLMAC_VERSION_A033)) {
            SOC_IF_ERROR_RETURN(READ_CLMAC_RX_CTRLr(unit, port, &rval));
            *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr, rval, RX_PASS_CTRLf);
        }
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_TYPEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_TYPEr, rval,
                                       PFC_ETH_TYPEf);
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_OPCODEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_OPCODEr, rval,
                                       PFC_OPCODEf);
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        *value = 8;
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_DAr(unit, port, &rval));
        fval0 = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval1 = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval, PFC_MACDA_HIf);
        *value = (fval0 >> 24) | (fval1 << 8);
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_DAr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval,
                                       PFC_MACDA_LOf) & 0x00ffffff;
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       RX_PASS_PFCf);
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       RX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       TX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       FORCE_PFC_XONf);
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       PFC_STATS_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       PFC_REFRESH_TIMERf);
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN(READ_CLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, rval,
                                       PFC_XOFF_TIMERf);
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, rval,
                                       RX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, rval,
                                       TX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, CLMAC_EEE_CTRLr, EEE_ENf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_EEE_CTRLr, rval, EEE_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, CLMAC_EEE_TIMERSr, EEE_DELAY_ENTRY_TIMERf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_EEE_TIMERSr, rval,
                EEE_DELAY_ENTRY_TIMERf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, CLMAC_EEE_TIMERSr, EEE_WAKE_TIMERf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_CLMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_EEE_TIMERSr, rval,
                EEE_WAKE_TIMERf);
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, rval,
                                       LOCAL_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOC_IF_ERROR_RETURN(READ_CLMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_RX_LSS_STATUSr, rval,
                                       LOCAL_FAULT_STATUSf);
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_CLMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, rval,
                                       REMOTE_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOC_IF_ERROR_RETURN(READ_CLMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_RX_LSS_STATUSr, rval,
                                       REMOTE_FAULT_STATUSf);
        break;
    case SOC_MAC_CONTROL_EXPECTED_RX_LATENCY:
        SOC_IF_ERROR_RETURN(mac_cl_expected_rx_latency_get(unit, port, value));
        break;
    case SOC_MAC_CONTROL_RX_RUNT_THRESHOLD:
        SOC_IF_ERROR_RETURN(READ_CLMAC_RX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr,
                                       rval, RUNT_THRESHOLDf);
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_control_get: unit %d port %s type=%d value=%d "
                            "rv=%d\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            type, *value, rv));
    return rv;
}

/*
 * Function:
 *      mac_cl_ability_local_get
 * Purpose:
 *      Return the abilities of CLMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_cl_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port_speed_max, use_hg_port_speeds = FALSE;

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

    /*
     * portmap_* config variables allow users to specifiy a HG speed as the max
     * speed for a port that initializes as an ethernet port. This allows the user
     * to optionally set the port speed to a HG speed after an encap change is
     * performed. Under such situations allow HG speeds to be part of the MAC
     * ability mask for the ethernet port.
     */
    if (soc_feature(unit, soc_feature_flexport_based_speed_set)) {
        use_hg_port_speeds = TRUE;
    }
    /* Adjust port_speed_max according to the port config */
    port_speed_max = si->port_speed_max[port];
    /* Use current number of lanes per port to determine the supported speeds */
    if (use_hg_port_speeds || IS_HG_PORT(unit , port)) {
        switch (port_speed_max) {
        case 127000:
            ability->speed_full_duplex |= SOC_PA_SPEED_127GB;
            /* fall through */
        case 120000:
            ability->speed_full_duplex |= SOC_PA_SPEED_120GB;
            /* fall through */
        case 106000:
            if (si->port_num_lanes[port] == 4)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            }
            /* fall through */
        case 100000:
            if (si->port_num_lanes[port] == 4)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            }
            /* fall through */
        case 53000:
            if (si->port_num_lanes[port] == 2)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_53GB;
            }
            /* fall through */
        case 50000:
            if (si->port_num_lanes[port] == 2)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
            }
            /* fall through */
        case 42000:
            if (si->port_num_lanes[port] != 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            }
            /* fall through */
        case 40000:
            if (si->port_num_lanes[port] != 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
                if(soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
                }
            }
            /* fall through */
        case 30000:
            if (!SOC_IS_TOMAHAWKX(unit)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
            }
            /* fall through */
        case 27000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_27GB;
            }
            /* fall through */
        case 25000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            }
            /* fall through */
        case 21000:
            if (si->port_num_lanes[port] == 2) {
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            }
            /* fall through */
        case 20000:
            if (si->port_num_lanes[port] == 2) {
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
            }
            /* fall through */
        case 16000: 
            if (!SOC_IS_TOMAHAWKX(unit)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_16GB;
            }
            /* fall through */
        case 15000: 
            if (!SOC_IS_TOMAHAWKX(unit)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
            }
            /* fall through */
        case 13000: 
            if (!SOC_IS_TOMAHAWKX(unit)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
            }
            /* fall through */
        case 12000: 
            if (!SOC_IS_TOMAHAWKX(unit)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
            }
            /* fall through */
        case 11000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            }
            /* fall through */
        case 10000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            }
            /* fall through */
        case 1000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
            break;
        default:
            break;
        }
    } else {
        switch (port_speed_max) {
        case 106000:
            ability->speed_full_duplex |= SOC_PA_SPEED_106GB;
            /* fall through */
        case 100000:
            if (si->port_num_lanes[port] == 4)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_100GB;
            }
            /* fall through */
        case 53000:
        case 50000:
            if (si->port_num_lanes[port] == 2)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_50GB;
            }
            /* fall through */
        case 42000:
        case 40000:

            if (si->port_num_lanes[port] != 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            }
            /* fall through */
        case 25000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            }
            /* fall through */
        case 21000:
        case 20000:
            if (si->port_num_lanes[port] == 2)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
            }
            /* fall through */
        case 11000:
        case 10000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            }
            /* fall through */
        case 1000:
            if (si->port_num_lanes[port] == 1)  {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
        default:
            break;
        }
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            if (si->port_num_lanes[port] == 1)  {
                if (port_speed_max >= 5000) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
                }
                if (port_speed_max >= 2500) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
                }
                if (port_speed_max >= 1000) {
                    ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
                }
            }
        }
#endif /* BCM_FIRELIGHT_SUPPORT */
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mac_cl_ability_local_get: unit %d port %s "
                            "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                            "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                            unit, SOC_PORT_NAME(unit, port),
                            ability->speed_half_duplex, ability->speed_full_duplex,
                            ability->encap, ability->pause, ability->interface,
                            ability->medium, ability->loopback, ability->flags));
    return SOC_E_NONE;
}

static int
mac_cl_timesync_tx_info_get (int unit, soc_port_t port,
                                        soc_port_timesync_tx_info_t *tx_info)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN
        (READ_CLMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
    if (soc_reg64_field32_get(unit, CLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                              ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    SOC_IF_ERROR_RETURN
        (READ_CLMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
    tx_info->timestamps_in_fifo = soc_reg64_field32_get(unit,
                     CLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);

    tx_info->timestamps_in_fifo_hi = 0;

    tx_info->sequence_id = soc_reg64_field32_get(unit,
                     CLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, SEQUENCE_IDf);

    return (SOC_E_NONE);
}


/* Exported CLMAC driver structure */
mac_driver_t soc_mac_cl = {
    "CLMAC Driver",               /* drv_name */
    mac_cl_init,                  /* md_init  */
    mac_cl_enable_set,            /* md_enable_set */
    mac_cl_enable_get,            /* md_enable_get */
    mac_cl_duplex_set,            /* md_duplex_set */
    mac_cl_duplex_get,            /* md_duplex_get */
    mac_cl_speed_set,             /* md_speed_set */
    mac_cl_speed_get,             /* md_speed_get */
    mac_cl_pause_set,             /* md_pause_set */
    mac_cl_pause_get,             /* md_pause_get */
    mac_cl_pause_addr_set,        /* md_pause_addr_set */
    mac_cl_pause_addr_get,        /* md_pause_addr_get */
    mac_cl_loopback_set,          /* md_lb_set */
    mac_cl_loopback_get,          /* md_lb_get */
    mac_cl_interface_set,         /* md_interface_set */
    mac_cl_interface_get,         /* md_interface_get */
    NULL,                         /* md_ability_get - Deprecated */
    mac_cl_frame_max_set,         /* md_frame_max_set */
    mac_cl_frame_max_get,         /* md_frame_max_get */
    mac_cl_ifg_set,               /* md_ifg_set */
    mac_cl_ifg_get,               /* md_ifg_get */
    mac_cl_encap_set,             /* md_encap_set */
    mac_cl_encap_get,             /* md_encap_get */
    mac_cl_control_set,           /* md_control_set */
    mac_cl_control_get,           /* md_control_get */
    mac_cl_ability_local_get,     /* md_ability_local_get */
    mac_cl_timesync_tx_info_get   /* md_timesync_tx_info_get */
 };

#endif /* BCM_CLMAC_SUPPORT */
