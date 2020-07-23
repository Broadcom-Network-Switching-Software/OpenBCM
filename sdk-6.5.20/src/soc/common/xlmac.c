/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XLMAC driver
 */

#include <shared/bsl.h>
#include <bcm/types.h> 
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
#ifdef BCM_GREYHOUND_SUPPORT
#include <soc/greyhound.h>
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
#include <soc/firelight.h>
#endif
#ifdef BCM_SABER2_SUPPORT
#include <soc/saber2.h>
#endif
#ifdef BCM_GREYHOUND2_SUPPORT
#include <soc/greyhound2.h>
#endif

#ifdef BCM_XLMAC_SUPPORT

#define XLMAC_RUNT_THRESHOLD_IEEE  0x40   /* Runt threshold value for IEEE ports */
#define XLMAC_RUNT_THRESHOLD_HG1   0x48   /* Runt threshold value for HG1 ports */
#define XLMAC_RUNT_THRESHOLD_HG2   0x4c   /* Runt threshold value for HG2 ports */
#define XLMAC_RUNT_THRESHOLD_MIN   0x11
#define XLMAC_RUNT_THRESHOLD_MAX   0x60

/*
 * XLMAC Register field definitions.
 */

#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

mac_driver_t soc_mac_xl;

#ifdef BROADCOM_DEBUG
static char *mac_xl_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *mac_xl_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

#define SOC_XLMAC_SPEED_10     0x0
#define SOC_XLMAC_SPEED_100    0x1
#define SOC_XLMAC_SPEED_1000   0x2
#define SOC_XLMAC_SPEED_2500   0x3
#define SOC_XLMAC_SPEED_10000  0x4

#define SOC_XLMAC_XLPORT_MODE_SINGLE    4
#define SOC_XLMAC_XLPORT_MODE_QUAD      0

/* Transmit CRC Modes */
#define XLMAC_CRC_APPEND        0x0
#define XLMAC_CRC_KEEP          0x1
#define XLMAC_CRC_REPLACE       0x2
#define XLMAC_CRC_PER_PKT_MODE  0x3

#ifdef BCM_SABER2_SUPPORT
typedef struct {
    uint64 mac_ctrl;
    uint64 rx_ctrl;
    uint64 tx_ctrl;
    uint64 pfc_ctrl;
    uint64 pfc_type;
    uint64 pfc_opcode;
    uint64 pfc_da;
    uint64 rx_max_size;
    uint64 mac_mode;
    uint64 pause_ctrl;
    uint64 rx_mac_sa;
    uint64 tx_mac_sa;
    uint64 llfc_ctrl;
    uint64 eee_ctrl;
    uint64 eee_timers;
    uint64 rx_lss_ctrl;
    uint64 rx_vlan_tag;
    uint64 e2e_ctrl;
} _xlmac_x_cfg_t;
#endif

struct {
    int speed;
    uint32 clock_rate;
}_mac_xl_clock_rate[] = {
    { 40000, 312 },
    { 20000, 156 },
    { 10000, 78  },
    { 5000,  78  },
    { 2500,  312 },
    { 1000,  125 },
    { 0,     25  },
};

/*
 *  WAN mode throttling
 *  THROT_10G_TO_5G: throt_num=21 throt_denom=21
 *  THROT_10G_TO_2P5G: throt_num=63 throt_denom=21
 */
#define XLMAC_THROT_10G_TO_5G    256
#define XLMAC_THROT_10G_TO_2P5G  257

STATIC int
_mac_xl_timestamp_delay_set(int unit, soc_port_t port, int speed);
STATIC int
mac_xl_speed_get(int unit, soc_port_t port, int *speed);
STATIC int
mac_xl_encap_get(int unit, soc_port_t port, int *mode);
void
_mac_xl_speed_to_clock_rate(int unit, soc_port_t port, int speed,
                            uint32 *clock_rate)
{
    int idx;


    for (idx = 0;
         idx < sizeof(_mac_xl_clock_rate) / sizeof(_mac_xl_clock_rate[0]);
         idx++) {
        if (speed >=_mac_xl_clock_rate[idx].speed) {
            *clock_rate = _mac_xl_clock_rate[idx].clock_rate;
            return;
        }
    }
    *clock_rate = 0;
}

STATIC int
_mac_xl_drain_cells(int unit, soc_port_t port, int notify_phy, int queue_enable)
{
    int         rv;
    int         pause_tx, pause_rx, pfc_rx, llfc_rx;
    soc_field_t fields[2];
    uint32      values[2], fval;
    uint64      mac_ctrl, rval64;
    soc_timeout_t to;
#ifdef BCM_GREYHOUND_SUPPORT
    uint32      mmu_fc = 0;
#endif

    rv  = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return rv;
    }

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mmu_flush_enable(unit, port, TRUE));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d xlmac mmu flush enable completed\n"),
                     port));
        /* clear the MMU pause state*/
        SOC_IF_ERROR_RETURN(
            READ_MMU_FC_RX_ENr(unit, port, &mmu_fc));
        SOC_IF_ERROR_RETURN(
            WRITE_MMU_FC_RX_ENr(unit, port, 0));
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, TRUE));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d xlmac mmu flush enable completed\n"),
                     port));
    }
#endif

    /* Disable pause/pfc function */
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_set(unit, port, pause_tx, 0));

    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_get(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   &pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   0));

    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_get(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   &llfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   0));

    /* Assert SOFT_RESET before DISCARD just in case there is no credit left */
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 1));
    }
#endif

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 1);

    /* Drain data in TX FIFO without egressing at packet boundary */
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));
    fields[0] = DISCARDf;
    values[0] = 1;
    fields[1] = EP_DISCARDf;
    values[1] = 1;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, XLMAC_TX_CTRLr, port, 2,
                                                fields, values));

    /* Reset EP credit before de-assert SOFT_RESET */
    if (!SOC_IS_HURRICANE2(unit)) {
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
    }

    /* De-assert SOFT_RESET to let the drain start */
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
    }
#endif

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

    if (notify_phy == 1)
    {
    /* Notify PHY driver */
       SOC_IF_ERROR_RETURN
           (soc_phyctrl_notify(unit, port, phyEventStop, PHY_STOP_DRAIN));
    }

    /* Make sure port state done. */
    if (SOC_IS_HURRICANE3(unit)) {
        uint32 rval;
        rval = 0;
        soc_reg_field_set(unit, REMOTE_PFC_STATEr, &rval,
                          REMOTE_PFC_STATEf, 0xff);
        SOC_IF_ERROR_RETURN(WRITE_REMOTE_PFC_STATEr(unit, port, rval));
    }

    /* Wait until mmu cell count is 0 */
    rv = soc_egress_drain_cells(unit, port, 250000);
    if (SOC_E_NONE == rv) {
        /* Wait until TX fifo cell count is 0 */
        soc_timeout_init(&to, 250000, 0);
        for (;;) {
            rv = READ_XLMAC_TXFIFO_CELL_CNTr(unit, port, &rval64);
            if (SOC_E_NONE != rv) {
                break;
            }
            fval = soc_reg64_field32_get(unit, XLMAC_TXFIFO_CELL_CNTr, rval64,
                                         CELL_CNTf);
            if (fval == 0) {
                break;
            }
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_XLMAC,
                          (BSL_META_U(unit,
                                "ERROR: port %d:%s: "
                                "timeout draining TX FIFO (%d cells remain)\n"),
                                unit, SOC_PORT_NAME(unit, port), fval));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }

    if (notify_phy == 1)
    { /* Notify PHY driver */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventResume, PHY_STOP_DRAIN));
    }
   
    /* Stop TX FIFO drainging */
    values[0] = 0;
    values[1] = 0;
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, XLMAC_TX_CTRLr, port, 2,
                                                fields, values));

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_mmu_flush_enable(unit, port, FALSE));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d xlmac mmu flush disabled\n"),
                     port));
        /* restore the MMU pause state*/
        SOC_IF_ERROR_RETURN(
            WRITE_MMU_FC_RX_ENr(unit, port, mmu_fc));
    }
#endif

#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit) && queue_enable) {
        SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "port %d xlmac mmu flush disabled\n"),
                     port));
    }
#endif

    /* Restore original pause/pfc/llfc configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_pause_set(unit, port, pause_tx, pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                   pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                   llfc_rx));

    return rv;
}

/*
 * Function:
 *      _mac_xl_timestamp_byte_adjust_set
 * Purpose:
 *      Set timestamp byte adjust values.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_xl_timestamp_byte_adjust_set(int unit, soc_port_t port)
{
    uint64 ctrl;
    uint32 ctrl32;

    if (SOC_REG_PORT_VALID(unit, MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr, port) &&
        (IS_GE_PORT(unit, port)) ) {
        /* for viper ports on SB2 A0 */
        SOC_IF_ERROR_RETURN(READ_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl32));
        soc_reg_field_set(unit, MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl32,
                          TX_TIMER_BYTE_ADJUSTf, 8);
        SOC_IF_ERROR_RETURN(WRITE_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl32));

        SOC_IF_ERROR_RETURN(READ_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl32));
        soc_reg_field_set(unit, MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl32,
                          TX_TIMER_BYTE_ADJUST_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl32));

        SOC_IF_ERROR_RETURN(READ_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl32));
        soc_reg_field_set(unit, MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl32,
                          RX_TIMER_BYTE_ADJUSTf, 8);
        SOC_IF_ERROR_RETURN(WRITE_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl32));

        SOC_IF_ERROR_RETURN(READ_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl32));
        soc_reg_field_set(unit, MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl32,
                          RX_TIMER_BYTE_ADJUST_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MXQ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl32));
    }

    if (SOC_REG_PORT_VALID(unit, XLMAC_TIMESTAMP_BYTE_ADJUSTr, port) && 
        (IS_GE_PORT(unit, port)) ) {
        /* for viper and eagle ports on SB2 B0 */
        SOC_IF_ERROR_RETURN(READ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl));
        soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                          TX_TIMER_BYTE_ADJUSTf, 8);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl));

        SOC_IF_ERROR_RETURN(READ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl));
        soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                          TX_TIMER_BYTE_ADJUST_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl));

        SOC_IF_ERROR_RETURN(READ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl));
        soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                          RX_TIMER_BYTE_ADJUSTf, 8);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl));

        SOC_IF_ERROR_RETURN(READ_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, &ctrl));
        soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                          RX_TIMER_BYTE_ADJUST_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_TIMESTAMP_BYTE_ADJUSTr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_init
 * Purpose:
 *      Initialize Xlmac into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_xl_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    uint64 mac_ctrl, rx_ctrl, tx_ctrl, rval;
    uint32 ipg;
    int mode;
    int encap = 0;
    int runt;
#if defined(BCM_SABER2_SUPPORT)
    int                 sgn_det = 0;
    uint32              sgn_crs;
#endif



    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    si = &SOC_INFO(unit);

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));

    /* Reset EP credit before de-assert SOFT_RESET */
    if (soc_reg64_field32_get(unit, XLMAC_CTRLr, mac_ctrl, SOFT_RESETf)) {
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
    }

#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
    }
#endif

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl,
                          XGMII_IPG_CHECK_DISABLEf,
                          IS_HG_PORT(unit, port) ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

    if (IS_ST_PORT(unit, port)) {
        soc_mac_xl.md_pause_set(unit, port, FALSE, FALSE);
    } else {
        soc_mac_xl.md_pause_set(unit, port, TRUE, TRUE);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, PFC_REFRESH_ENf,
                                1));

    if (soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE)) {
        SOC_IF_ERROR_RETURN
            (soc_mac_xl.md_control_set(unit, port,
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
        soc_reg64_field32_set(unit, XLMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                          SOC_INFO(unit).max_mtu);
    } else
#endif
    {
        soc_reg64_field32_set(unit, XLMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                          JUMBO_MAXSZ);
    }
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_MAX_SIZEr(unit, port, rval));

    /* Setup header mode, check for property for higig2 mode */
    COMPILER_64_ZERO(rval);
    if (!IS_XE_PORT(unit, port) && !IS_GE_PORT(unit, port)) {
        mode = soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE,
               soc_feature(unit, soc_feature_no_higig_plus) ? 1 : 0) ? 2 : 1;
        soc_reg64_field32_set(unit, XLMAC_MODEr, &rval, HDR_MODEf, mode);
        encap = mode;
    }
    switch (si->port_speed_max[port]) {
    case 10:
        mode = SOC_XLMAC_SPEED_10;
        break;
    case 100:
        mode = SOC_XLMAC_SPEED_100;
        break;
    case 1000:
        mode = SOC_XLMAC_SPEED_1000;
        break;
    case 2500:
        mode = SOC_XLMAC_SPEED_2500;
        break;
    default:
        mode = SOC_XLMAC_SPEED_10000;
        break;
    }
    soc_reg64_field32_set(unit, XLMAC_MODEr, &rval, SPEED_MODEf, mode);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_MODEr(unit, port, rval));

    /* init IPG and RUNT_THRESHOLD after port encap mode been established. */
    if (encap == 1) {
        ipg = SOC_PERSIST(unit)->ipg[port].fd_hg;
        runt = XLMAC_RUNT_THRESHOLD_HG1;
    } else if (encap == 2) {
        ipg = SOC_PERSIST(unit)->ipg[port].fd_hg2;
        runt = XLMAC_RUNT_THRESHOLD_HG2;
    } else {
        ipg = SOC_PERSIST(unit)->ipg[port].fd_xe;
        runt = XLMAC_RUNT_THRESHOLD_IEEE;
    }

    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &tx_ctrl));
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf,
                          (ipg / 8) & 0x1f);
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf,
                          XLMAC_CRC_PER_PKT_MODE);
#if defined (BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            if (soc_fl_macsec_idx_get(unit, port) != -1) {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl,
                                      TX_THRESHOLDf, 6);
            } else {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl,
                                      TX_THRESHOLDf, 1);
            }
        }
#endif
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, tx_ctrl));

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &rx_ctrl));
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 0);
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rx_ctrl, STRICT_PREAMBLEf,
                          si->port_speed_max[port] >= 10000 &&
                          IS_XE_PORT(unit, port) ? 1 : 0);

    /* assigning RUNT_THRESHOLD (per encap mode) */
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf,
                          runt);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_CTRLr(unit, port, rx_ctrl));

    /* assigning proper init setting for EEE feature per speed */
    if (soc_feature(unit, soc_feature_eee)) {
        SOC_IF_ERROR_RETURN(soc_port_eee_timers_init(unit,
                port, si->port_speed_max[port]));
    }

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LOCAL_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_REMOTE_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LINK_INTERRUPTf, 1);    
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, rval));

    if (SOC_REG_IS_VALID(unit, XLMAC_MACSEC_CTRLr)) {
        /* Enable Tx CRC corruption */
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_MACSEC_CTRLr, port,
                                                   MACSEC_TX_CRC_CORRUPT_ENf, 1));
    }
#if defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_SABER2(unit)){
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, EXTENDED_HIG2_ENf, 1);
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, SW_LINK_STATUSf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

        sgn_det = soc_property_port_get(unit, port,  spn_SERDES_RX_LOS, 0);
        SOC_IF_ERROR_RETURN(READ_X_GPORT_SGNDET_EARLYCRSr(unit, port, &sgn_crs));
        soc_reg_field_set(unit, X_GPORT_SGNDET_EARLYCRSr, &sgn_crs, SGN_DETf, sgn_det);
        SOC_IF_ERROR_RETURN(WRITE_X_GPORT_SGNDET_EARLYCRSr(unit, port, sgn_crs));
    }

#endif

    /* Disable loopback and bring XLMAC out of reset */
    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, mac_ctrl));

    SOC_IF_ERROR_RETURN(_mac_xl_timestamp_byte_adjust_set(unit, port));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_egress_queue_drain
 * Purpose:
 *      Drain the egress queues with out bringing down the port
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_egress_queue_drain(int unit, soc_port_t port)
{
    uint64 ctrl, octrl;
    pbmp_t mask;
    int rx_enable = 0;
    int is_active = 0;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_egress_queue_drain: unit %d port %s \n"),
                 unit, SOC_PORT_NAME(unit, port)));

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;

    rx_enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, ctrl, RX_ENf);
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, RX_ENf, 0);
        /* Disable RX */
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, ctrl));

        /* Remove port from EPC_LINK */
    soc_link_mask2_get(unit, &mask);
    if (SOC_PBMP_MEMBER(mask, port)) {
        is_active = 1;
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }

        /* Drain cells */
    SOC_IF_ERROR_RETURN(_mac_xl_drain_cells(unit, port, 0, TRUE));

        /* Reset port FIFO */
    SOC_IF_ERROR_RETURN(soc_port_fifo_reset(unit, port));

        /* Put port into SOFT_RESET */
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 1));
    }
#endif

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, ctrl));

        /* Reset EP credit before de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));
    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &ctrl));
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, RX_ENf, rx_enable ? 1 : 0);

      /* Enable both TX and RX, deassert SOFT_RESET */
#if defined (BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
    }
#endif

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, ctrl));

        /* Restore XLMAC_CTRL to original value */
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, octrl));        

        /* Add port to EPC_LINK */
    if(is_active) {
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    }
 
    return SOC_E_NONE;
}

#if defined(BCM_SABER2_SUPPORT)

STATIC int _xlmac_x_register_store(int unit, soc_port_t port, _xlmac_x_cfg_t *port_cfg)
{
    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &port_cfg->mac_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &port_cfg->rx_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &port_cfg->tx_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &port_cfg->pfc_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_TYPEr(unit, port, &port_cfg->pfc_type));
    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_OPCODEr(unit, port, &port_cfg->pfc_opcode));
    SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &port_cfg->pfc_da));
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAX_SIZEr(unit, port, &port_cfg->rx_max_size));
    SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit, port, &port_cfg->mac_mode));
    SOC_IF_ERROR_RETURN(READ_XLMAC_PAUSE_CTRLr(unit, port, &port_cfg->pause_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAC_SAr(unit, port, &port_cfg->rx_mac_sa));
    SOC_IF_ERROR_RETURN(READ_XLMAC_TX_MAC_SAr(unit, port, &port_cfg->tx_mac_sa));
    SOC_IF_ERROR_RETURN(READ_XLMAC_LLFC_CTRLr(unit, port, &port_cfg->llfc_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_CTRLr(unit, port, &port_cfg->eee_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_TIMERSr(unit, port, &port_cfg->eee_timers));
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &port_cfg->rx_lss_ctrl));
    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_VLAN_TAGr(unit, port, &port_cfg->rx_vlan_tag));
    SOC_IF_ERROR_RETURN(READ_XLMAC_E2E_CTRLr(unit, port, &port_cfg->e2e_ctrl));
    return SOC_E_NONE;
}

STATIC int _xlmac_x_register_restore(int unit, soc_port_t port, _xlmac_x_cfg_t *port_cfg)
{
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, port_cfg->mac_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_CTRLr(unit, port, port_cfg->rx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, port_cfg->tx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_CTRLr(unit, port, port_cfg->pfc_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_TYPEr(unit, port, port_cfg->pfc_type));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_OPCODEr(unit, port, port_cfg->pfc_opcode));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_DAr(unit, port, port_cfg->pfc_da));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_MAX_SIZEr(unit, port, port_cfg->rx_max_size));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_MODEr(unit, port, port_cfg->mac_mode));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_PAUSE_CTRLr(unit, port, port_cfg->pause_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_MAC_SAr(unit, port, port_cfg->rx_mac_sa));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_MAC_SAr(unit, port, port_cfg->tx_mac_sa));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_LLFC_CTRLr(unit, port, port_cfg->llfc_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_EEE_CTRLr(unit, port, port_cfg->eee_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_EEE_TIMERSr(unit, port, port_cfg->eee_timers));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_LSS_CTRLr(unit, port, port_cfg->rx_lss_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_VLAN_TAGr(unit, port, port_cfg->rx_vlan_tag));
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_E2E_CTRLr(unit, port, port_cfg->e2e_ctrl));
    return SOC_E_NONE;
}

int _xlmac_sb2_shim_war(int unit, soc_port_t port)
{
    uint32 soft_reset;
    uint32 rval;
    int p, index;
    uint64 ctrl;
    pbmp_t block_pbmp;
    _xlmac_x_cfg_t *pcfg;
    int speed = 1000;
    
    BCM_PBMP_ASSIGN(block_pbmp, SOC_BLOCK_BITMAP(unit, SOC_PORT_BLOCK(unit, port)));

    SOC_PBMP_ITER(block_pbmp, p) {
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), p)) {
            SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, p, &ctrl));
            soft_reset = soc_reg64_field32_get(unit, 
                    XLMAC_CTRLr, ctrl, SOFT_RESETf);
            /* If any of the port in block is active, dont apply war */
            if(soft_reset != 1) {
                return SOC_E_NONE;
            }
        }
    }
    /* Come here only when there is active war */
    pcfg = (_xlmac_x_cfg_t *) sal_alloc(
            sizeof(_xlmac_x_cfg_t) * SB2_MAX_PORTS_PER_BLOCK,"XLMAC config");
    if (pcfg == NULL) {
        return SOC_E_MEMORY;
    }

    index = 0;
    SOC_PBMP_ITER(block_pbmp, p) {
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), p)) {
            if(index >= SB2_MAX_PORTS_PER_BLOCK) {
                sal_free(pcfg);
                return SOC_E_INTERNAL;
            }
            SOC_IF_ERROR_CLEAN_RETURN
                (_xlmac_x_register_store(unit, p, &pcfg[index]),sal_free(pcfg));
            index++;     
        }
    }
    p = SOC_BLOCK_PORT(unit, SOC_PORT_BLOCK(unit, port));
    SOC_IF_ERROR_CLEAN_RETURN(READ_XPORT_XMAC_CONTROLr(unit, p, &rval), sal_free(pcfg));
    soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 1);
    SOC_IF_ERROR_CLEAN_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, p, rval), sal_free(pcfg));
    soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 0);
    SOC_IF_ERROR_CLEAN_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, p, rval), sal_free(pcfg));
    
    index = 0;
    SOC_PBMP_ITER(block_pbmp, p) {
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), p)) {
            SOC_IF_ERROR_CLEAN_RETURN
                (_xlmac_x_register_restore(unit, p, &pcfg[index]),
                 sal_free(pcfg));
            index++;
        }
    }
    sal_free(pcfg);

    /* reconfigure timestamp adjust after port block reset */
    index = 0;
    SOC_PBMP_ITER(block_pbmp, p) {
        if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), p)) {
            mac_xl_speed_get(unit, p, &speed);
            _mac_xl_timestamp_delay_set(unit, p, speed);
            index++;
        }
    }
    return SOC_E_NONE;
}
#endif

/*
 * Function:
 *      mac_xl_enable_set
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
mac_xl_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 ctrl, octrl;
    pbmp_t mask;
    uint32 soft_reset = 0;
    int speed = 1000;
    uint64 version, merge_config;
    int xlmac_version;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                        "mac_xl_enable_set: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 enable ? "True" : "False"));

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &ctrl));
    octrl = ctrl;
    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        /* SDK-49952 fix soluition :
         *  >> to avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply origianl designing senario, XLMAC_CTRLr.SOFT_RESETf is 
         *      used to early check to see if this port is at disabled state 
         *      already.
         */
        soft_reset = soc_reg64_field32_get(unit, 
                XLMAC_CTRLr, ctrl, SOFT_RESETf);
        if ((enable) || (!enable && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (enable) {
        /* Reset EP credit before de-assert SOFT_RESET */
        SOC_IF_ERROR_RETURN(soc_port_credit_reset(unit, port));

#if defined(BCM_SABER2_SUPPORT)
        if (SOC_IS_SABER2(unit)) {
            SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));
        }
#endif

        /* Deassert SOFT_RESET */
#if defined (BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 0));
        }
#endif

        soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);

        /* Deassert EGR_XLPORT_BUFFER_SFT_RESET */
        SOC_IF_ERROR_RETURN(soc_port_egress_buffer_sft_reset(unit, port, 0));

        /* Enable both TX and RX */
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, ctrl));

        /* Enable MMU port */
        SOC_IF_ERROR_RETURN(soc_port_mmu_buffer_enable(unit, port, TRUE));

        /* Add port to EPC_LINK */
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));

        /* Enable output threshold RX */
        SOC_IF_ERROR_RETURN
            (soc_port_thdo_rx_enable_set(unit, port, 1));        

        /* set timestamp adjust delay */
        mac_xl_speed_get(unit, port, &speed);
        _mac_xl_timestamp_delay_set(unit, port, speed);
    } else {
        /* Disable RX */
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, ctrl));

        /* Remove port from EPC_LINK */
        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        sal_udelay(1000);

        /*
         * Disable MMU port
         * for some devices where EPC_LINK can not block the SOBMH from cpu and
         * no mechanism is avalible to reset EDATABUF.
         */
        SOC_IF_ERROR_RETURN(soc_port_mmu_buffer_enable(unit, port, FALSE));

        /* Drain cells */
        SOC_IF_ERROR_RETURN(_mac_xl_drain_cells(unit, port, 1, FALSE));
        /* Reset egress_buffer */
        SOC_IF_ERROR_RETURN(soc_port_egress_buffer_sft_reset(unit, port, 1));

        /* Reset port FIFO */
        SOC_IF_ERROR_RETURN(soc_port_fifo_reset(unit, port));

        /* Put port into SOFT_RESET */
#if defined (BCM_FIRELIGHT_SUPPORT)
        if (SOC_IS_FIRELIGHT(unit)) {
            SOC_IF_ERROR_RETURN(soc_fl_macsec_ctrl_reset(unit, port, 1));
        }
#endif

        soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, ctrl));
#if defined(BCM_SABER2_SUPPORT) 
        if(SOC_IS_SABER2(unit) && IS_MXQ_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(_xlmac_sb2_shim_war(unit, port));
        }
#endif
        /* Disable output threshold RX */
        SOC_IF_ERROR_RETURN
            (soc_port_thdo_rx_enable_set(unit, port, 0));          

        /* Forces rx_lost_eop sequence */
        if (SOC_REG_IS_VALID(unit, XLMAC_VERSION_IDr) &&
            SOC_REG_IS_VALID(unit, XLMAC_MERGE_CONFIGr)) {

            SOC_IF_ERROR_RETURN(READ_XLMAC_VERSION_IDr(unit, port, &version));
            xlmac_version =
                soc_reg64_field32_get(
                    unit, XLMAC_VERSION_IDr, version, XLMAC_VERSIONf);
            if (xlmac_version >= 0xA043) {
                /* Set SW force */
                SOC_IF_ERROR_RETURN(
                    READ_XLMAC_MERGE_CONFIGr(unit, port, &merge_config));
                soc_reg64_field32_set(
                    unit, XLMAC_MERGE_CONFIGr, &merge_config,
                    SW_FORCE_RX_LOST_EOPf, 1);
                SOC_IF_ERROR_RETURN(
                    WRITE_XLMAC_MERGE_CONFIGr(unit, port, merge_config));

                /* Clear SW force */
                SOC_IF_ERROR_RETURN(
                    READ_XLMAC_MERGE_CONFIGr(unit, port, &merge_config));
                soc_reg64_field32_set(
                    unit, XLMAC_MERGE_CONFIGr, &merge_config,
                    SW_FORCE_RX_LOST_EOPf, 0);
                SOC_IF_ERROR_RETURN(
                    WRITE_XLMAC_MERGE_CONFIGr(unit, port, merge_config));
            }
        }

    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_enable_get
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
mac_xl_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &ctrl));

    *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, ctrl, RX_ENf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_enable_get: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *enable ? "True" : "False"));

    return SOC_E_NONE;
}



/*
 * Function:
 *      _mac_xl_timestamp_delay_set
 * Purpose:
 *      Set Timestamp delay for one-step to account for lane and pipeline delay.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      speed - Speed
 *      phy_mode - single/dual/quad phy mode
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_xl_timestamp_delay_set(int unit, soc_port_t port, int speed)
{
    uint64 ctrl;
    uint32 clk_rate, tx_clk_ns;
    int osts_delay;
    uint32 field;
    int divisor;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                 "mac_xl_timestamp_delay_set: unit %d speed=%dMb port %s\n"),
                 unit, speed, SOC_PORT_NAME(unit, port)));

    if (SOC_REG_IS_VALID(unit, XLMAC_TIMESTAMP_ADJUSTr)) {
        SOC_IF_ERROR_RETURN(READ_XLMAC_TIMESTAMP_ADJUSTr(unit, port, &ctrl));

        _mac_xl_speed_to_clock_rate(unit, port, speed, &clk_rate);
        /* Tx clock rate for single/dual/quad phy mode */
        if ((speed >= 5000) && (speed <= 40000)) {
            divisor = speed > 20000 ? 1 : speed > 10000 ? 2 : 4;
            /* tx clock rate in ns */
            tx_clk_ns = ((1000 / clk_rate) / divisor);
        } else {
            /* Same tx clk rate for < 10G  for all phy modes*/
            tx_clk_ns = 1000 / clk_rate;
        }

        
        
        /*
         * MAC pipeline delay for XGMII/XGMII mode is:
         *          = (5.5 * TX line clock period) + (Timestamp clock period)
         */
        /* signed value of pipeline delay in ns */
        osts_delay = soc_property_port_get(unit, port, spn_TIMESTAMP_ADJUST(speed),
                                           SOC_TIMESYNC_PLL_CLOCK_NS(unit) -
                                           ((11 * tx_clk_ns ) / 2));

        if (SOC_E_NONE != soc_reg_signed_field_mask(unit, XLMAC_TIMESTAMP_ADJUSTr,
                                                    TS_OSTS_ADJUSTf, osts_delay, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                 "%s property out of bounds (is %d)\n"),
                 spn_TIMESTAMP_ADJUST(speed), osts_delay));
            field = 0;
        }

        soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_OSTS_ADJUSTf, field);

#if 0 
        /*
         * Lane delay for xlmac lanes
         *   Lane_0(0-3)  : 1 * TX line clock period
         *   Lane_1(4-7)  : 2 * TX line clock period
         *   Lane_2(8-11) : 3 * TX line clock period
         *   Lane_3(12-15): 4 * TX line clock period
         */
        /* unsigned value of lane delay in ns */
        delay = 1 * tx_clk_ns;
        soc_reg64_field32_set(unit, XLMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_0f, delay );
        delay = 2 * tx_clk_ns;
        soc_reg64_field32_set(unit, XLMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_1f, delay );
        delay = 3 * tx_clk_ns;
        soc_reg64_field32_set(unit, XLMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_2f, delay );
        delay = 4 * tx_clk_ns;
        soc_reg64_field32_set(unit, XLMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_3f, delay );
#endif
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_TIMESTAMP_ADJUSTr(unit, port, ctrl));
    }

    /* Set Timestamp byte adjust values */
    SOC_IF_ERROR_RETURN(_mac_xl_timestamp_byte_adjust_set(unit, port));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_duplex_set
 * Purpose:
 *      Set XLMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_xl_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_duplex_get
 * Purpose:
 *      Get XLMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_pause_set
 * Purpose:
 *      Configure XLMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    static soc_field_t fields[2] = { TX_PAUSE_ENf, RX_PAUSE_ENf };
    uint32 values[2];

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_pause_set: unit %d port %s TX=%s RX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_tx != FALSE ? "on" : "off",
                 pause_rx != FALSE ? "on" : "off"));

    values[0] = pause_tx != FALSE ? 1 : 0;
    values[1] = pause_rx != FALSE ? 1 : 0;
    return soc_reg_fields32_modify(unit, XLMAC_PAUSE_CTRLr, port, 2,
                                   fields, values);
}

/*
 * Function:
 *      mac_xl_pause_get
 * Purpose:
 *      Return the pause ability of XLMAC
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
mac_xl_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_XLMAC_PAUSE_CTRLr(unit, port, &rval));
    *pause_tx =
        soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, rval, TX_PAUSE_ENf);
    *pause_rx =
        soc_reg64_field32_get(unit, XLMAC_PAUSE_CTRLr, rval, RX_PAUSE_ENf);
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_pause_get: unit %d port %s TX=%s RX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *pause_tx ? "on" : "off",
                 *pause_rx ? "on" : "off"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_speed_set
 * Purpose:
 *      Set XLMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 10, 100, 1000, 2500, 10000, ...
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_speed_set(int unit, soc_port_t port, int speed)
{
    uint32 mode;
    int enable;
    static soc_field_t fields[2] = {
        LOCAL_FAULT_DISABLEf, REMOTE_FAULT_DISABLEf
    };
    uint32 values[2];
    uint32 rval = 0;

#ifdef BCM_GREYHOUND2_SUPPORT
    /* SDK-141840 Workaround for TSCE core connecting to Aquantia PHY */
    if (SOC_IS_GREYHOUND2(unit)) {
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_TX_CTRLr, port, TX_ANY_STARTf,
                                    0));
    }
#endif

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_speed_set: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed));

    switch (speed) {
    case 10:
        mode = SOC_XLMAC_SPEED_10;
        break;
    case 100:
        mode = SOC_XLMAC_SPEED_100;
        break;
    case 1000:
        mode = SOC_XLMAC_SPEED_1000;
        break;
    case 2500:
#ifdef BCM_GREYHOUND2_SUPPORT
        /* SDK-141840 Workaround for TSCE core connecting to Aquantia PHY */
        if (SOC_IS_GREYHOUND2(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, XLMAC_TX_CTRLr, port,
                                        TX_ANY_STARTf, 1));
        }
#endif
        mode = SOC_XLMAC_SPEED_2500;
        break;
    case 5000:
        mode = SOC_XLMAC_SPEED_10000;
        break;
    case 0:
        return SOC_E_NONE;              /* Support NULL PHY */
    default:
        if (speed < 10000) {
            return SOC_E_PARAM;
        }
        mode = SOC_XLMAC_SPEED_10000;
        break;
    }

    SOC_IF_ERROR_RETURN(mac_xl_enable_get(unit, port, &enable));
        
    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_xl_enable_set(unit, port, 0));
    }

    /* Update the speed */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLMAC_MODEr, port, SPEED_MODEf, mode));

    values[0] = values[1] = speed < 5000 ? 1 : 0;
    if (speed < 5000) {
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, XLMAC_RX_LSS_CTRLr, port, 2, fields,
                                 values));
    }

    /*
     * Not set REMOTE_FAULT/LOCAL_FAULT for 1G ports,
     * else HW Linkscan interrupt would be suppressed.
     */
    if (SOC_IS_SABER2(unit) && IS_MXQ_PORT(unit, port)) {
         /* XLPORT_FAULT_LINK_STATUS is NA for MXQ ports on Saber2*/
    } else {
        values[0] = values[1] = speed <= 1000 ? 0 : 1;
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &rval,
                      REMOTE_FAULTf, values[0]);
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &rval,
                      LOCAL_FAULTf, values[1]);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_FAULT_LINK_STATUSr(unit, port, rval));
    }

    /* Update port speed related setting in components other than MAC/SerDes*/
    SOC_IF_ERROR_RETURN(soc_port_speed_update(unit, port, speed));

    /* Update port strict preamble */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLMAC_RX_CTRLr, port, STRICT_PREAMBLEf,
                                speed >= 10000 && IS_XE_PORT(unit, port) ?
                                1 : 0));
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
        SOC_IF_ERROR_RETURN(mac_xl_enable_set(unit, port, 1));
    }

    /* Set Timestamp Mac Delays */
    SOC_IF_ERROR_RETURN(_mac_xl_timestamp_delay_set(unit, port, speed));

    /* assigning proper setting for Native EEE per speed */
    if (soc_feature(unit, soc_feature_eee)) {
        SOC_IF_ERROR_RETURN(soc_port_eee_timers_setup(unit, port, speed));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_speed_get
 * Purpose:
 *      Get XLMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval;
#ifdef BCM_GREYHOUND2_SUPPORT
    int val = 0;
#endif

    SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, XLMAC_MODEr, rval, SPEED_MODEf)) {
    case SOC_XLMAC_SPEED_10:
        *speed = 10;
        break;
    case SOC_XLMAC_SPEED_100:
        *speed = 100;
        break;
    case SOC_XLMAC_SPEED_1000:
        *speed = 1000;
        break;
    case SOC_XLMAC_SPEED_2500:
        *speed = 2500;
        break;
    case SOC_XLMAC_SPEED_10000:
    default:
        *speed = 10000;
        break;
    }
#ifdef BCM_GREYHOUND2_SUPPORT
    if (soc_feature(unit, soc_feature_xlmac_speed_display_above_10g)) {
        if (SOC_SUCCESS(soc_granular_speed_get(unit, port, &val))) {
            *speed = val;
        }
    }
#endif
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_loopback_set
 * Purpose:
 *      Set a XLMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On Xlmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_loopback_set(int unit, soc_port_t port, int lb)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_loopback_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "local" : "no"));

    /* need to enable clock compensation for applicable serdes device */
    (void)soc_phyctrl_notify(unit, port, phyEventMacLoopback, lb ? 1 : 0);

    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port,
                                   SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE,
                                   lb ? 0 : 1));
    SOC_IF_ERROR_RETURN
        (soc_mac_xl.md_control_set(unit, port,
                                   SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE,
                                   lb ? 0 : 1));

    return soc_reg_field32_modify(unit, XLMAC_CTRLr, port, LOCAL_LPBKf,
                                  lb ? 1 : 0);

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_loopback_get
 * Purpose:
 *      Get current XLMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &ctrl));

    *lb = soc_reg64_field32_get(unit, XLMAC_CTRLr, ctrl, LOCAL_LPBKf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_loopback_get: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *lb ? "True" : "False"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_pause_addr_set
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
mac_xl_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    static soc_field_t fields[2] = { SA_HIf, SA_LOf };
    uint32 values[2];

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_pause_addr_set: unit %d port %s MAC=<"
                            "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));

    values[0] = (mac[0] << 8) | mac[1];
    values[1] = (mac[2] << 24) | (mac[3] << 16) | (mac[4] << 8) | mac[5];

    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, XLMAC_TX_MAC_SAr, port, 2, fields,
                                 values));
    SOC_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, XLMAC_RX_MAC_SAr, port, 2, fields,
                                 values));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_pause_addr_get
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
mac_xl_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAC_SAr(unit, port, &rval64));
    values[0] = soc_reg64_field32_get(unit, XLMAC_RX_MAC_SAr, rval64, SA_HIf);
    values[1] = soc_reg64_field32_get(unit, XLMAC_RX_MAC_SAr, rval64, SA_LOf);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_pause_addr_get: unit %d port %s MAC=<"
                            "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_interface_set
 * Purpose:
 *      Set a XLMAC interface type
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
mac_xl_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_interface_set: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_xl_port_if_names[pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_interface_get
 * Purpose:
 *      Retrieve XLMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_xl_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    *pif = SOC_PORT_IF_MII;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_interface_get: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_xl_port_if_names[*pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_frame_max_set
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_frame_max_set(int unit, soc_port_t port, int size)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_frame_max_set: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 size));

    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        size += 4;
    }
    return soc_reg_field32_modify(unit, XLMAC_RX_MAX_SIZEr, port, RX_MAX_SIZEf,
                                  size);
}

/*
 * Function:
 *      mac_xl_frame_max_get
 * Description:
 *      Set the maximum receive frame size for the port
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      size - Maximum frame size in bytes
 * Return Value:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_XLMAC_RX_MAX_SIZEr(unit, port, &rval));
    *size = soc_reg64_field32_get(unit, XLMAC_RX_MAX_SIZEr, rval,
                                  RX_MAX_SIZEf);
    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        *size -= 4;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_frame_max_get: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *size));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_ifg_set
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      ifg - number of bits to use for average inter-frame gap
 * Return Value:
 *      SOC_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_xl_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int ifg)
{
    int         cur_speed;
    int         cur_duplex;
    int         real_ifg;
    int         encap = 0;

    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    uint64      rval, orval;
    soc_port_ability_t ability;
    uint32      pa_flag;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_ifg_set: unit %d port %s speed=%dMb duplex=%s "
                            "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", ifg));

    pa_flag = SOC_PA_SPEED(speed); 
    sal_memset(&ability, 0, sizeof(soc_port_ability_t));
    soc_mac_xl.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 64 bytes (i.e. multiple of 8 bits) */
    real_ifg = ifg < 64 ? 64 : (ifg > 512 ? 512 : (ifg + 7) & (0x7f << 3));

    SOC_IF_ERROR_RETURN(mac_xl_encap_get(unit, port, &encap));
    if (encap == SOC_ENCAP_HIGIG) {
        si->fd_hg = real_ifg;
    } else if (encap == SOC_ENCAP_HIGIG2) {
        si->fd_hg2 = real_ifg;
    } else {
        /* for encap == IEEE */
        si->fd_xe = real_ifg;
    }

    SOC_IF_ERROR_RETURN(mac_xl_duplex_get(unit, port, &cur_duplex));
    SOC_IF_ERROR_RETURN(mac_xl_speed_get(unit, port, &cur_speed));

    /* XLMAC_MODE supports only 4 speeds with 4 being max as LINK_10G_PLUS */ 
    if ((speed > 10000) && (cur_speed == 10000)) { 
        cur_speed = speed; 
    } 

    if (cur_speed == speed &&
        cur_duplex == (duplex == SOC_PORT_DUPLEX_FULL ? TRUE : FALSE)) {
        SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &rval));
        orval = rval;
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, AVERAGE_IPGf,
                              real_ifg / 8);
        if (COMPILER_64_NE(rval, orval)) {
            SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, rval));
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_ifg_get
 * Description:
 *      Sets the new ifg (Inter-frame gap) value
 * Parameters:
 *      unit   - Device number
 *      port   - Port number
 *      speed  - the speed for which the IFG is being set
 *      duplex - the duplex for which the IFG is being set
 *      size - Maximum frame size in bytes
 * Return Value:
 *      SOC_E_XXX
 * Notes:
 *      The function makes sure the IFG value makes sense and updates the
 *      IPG register in case the speed/duplex match the current settings
 */
STATIC int
mac_xl_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    soc_port_ability_t ability;
    uint32      pa_flag;
    int         encap = 0;

    if (!duplex) {
        return SOC_E_PARAM;
    }

    pa_flag = SOC_PA_SPEED(speed); 
    sal_memset(&ability, 0, sizeof(soc_port_ability_t));
    soc_mac_xl.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(mac_xl_encap_get(unit, port, &encap));
    if (encap == SOC_ENCAP_HIGIG) {
        *ifg = si->fd_hg;
    } else if (encap == SOC_ENCAP_HIGIG2) {
        *ifg = si->fd_hg2;
    } else {
        /* for encap == IEEE */
        *ifg = si->fd_xe;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_ifg_get: unit %d port %s speed=%dMb duplex=%s "
                            "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", *ifg));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _mac_xl_port_mode_update
 * Purpose:
 *      Set the XLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      hg_mode - SOC HG encap modes. value={HG/HG2/HG2-LITE}
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_xl_port_mode_update(int unit, soc_port_t port, int hg_mode)
{
    uint32              rval;
    uint64              val64;
    soc_pbmp_t          ctr_pbmp;
    int                 rv = SOC_E_NONE;
    int                 to_hg_port = 0 ;
    int                 phy_enable = 0;
    int                 speed = 0;

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

    if (soc_feature(unit, soc_feature_port_encap_speed_max_config)) {
        if (to_hg_port) {
            SOC_IF_ERROR_RETURN(soc_port_hg_speed_get(unit, port, &speed));
            SOC_IF_ERROR_RETURN(soc_phyctrl_set_speed_max(unit, port, speed));
        } else {
            speed = SOC_INFO(unit).port_init_speed[port];
            SOC_IF_ERROR_RETURN(soc_phyctrl_set_speed_max(unit, port, speed));
        }
    }
    rv = soc_phyctrl_enable_get(unit, port, &phy_enable);

    if (SOC_SUCCESS(rv)) {
        rv = soc_phyctrl_init(unit, port);
    }

    if (soc_feature(unit, soc_feature_port_enable_encap_restore)){
        if (SOC_SUCCESS(rv)) {
            rv = soc_phyctrl_enable_set(unit, port, phy_enable);
        }
    } else {
        /* Re-enable phy after init.*/
        if (SOC_SUCCESS(rv) && phy_enable) {
            rv = soc_phyctrl_enable_set(unit, port, phy_enable);
        }
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_xl_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_xl_enable_set(unit, port, 0);
    }

    if (SOC_SUCCESS(rv)) {
        SOC_PBMP_CLEAR(ctr_pbmp);
        SOC_PBMP_PORT_SET(ctr_pbmp, port);
        COMPILER_64_SET(val64, 0, 0);
        rv = soc_counter_set_by_port(unit, ctr_pbmp, val64);
    }

    COUNTER_UNLOCK(unit);
    soc_linkscan_continue(unit);

    if (SOC_REG_IS_VALID(unit, XLPORT_CONFIGr)) {
        uint32  xlp_hg_mode; 

        xlp_hg_mode = (to_hg_port) ? 1 : 0;
#ifdef BCM_GREYHOUND_SUPPORT
        /* GH specific design is to apply HG setting PGW interface */
        if (SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) || 
                SOC_IS_GREYHOUND2(unit)) {

            /* XLPORT and PGW register for HIGIG_MODEf is confirmed 
             *  will be '1' for HG/HG+ setting only and '0' for other
             *  encap mode{IEEE/HG2/HG2-LITE}
             */
            xlp_hg_mode = (hg_mode == SOC_ENCAP_HIGIG) ? 1 : 0; 

            /* only port in XLMAC allows PGW encap setting below */
#ifdef BCM_GREYHOUND2_SUPPORT
            if (SOC_IS_GREYHOUND2(unit)) {
#ifdef BCM_FIRELIGHT_SUPPORT
                if (soc_feature(unit, soc_feature_fl)) {
                    SOC_IF_ERROR_RETURN
                        (soc_firelight_pgw_encap_field_modify(unit, port,
                            HIGIG_MODEf, xlp_hg_mode));
                } else
#endif /* BCM_FIRELIGHT_SUPPORT */
                {
                    SOC_IF_ERROR_RETURN
                        (soc_greyhound2_pgw_encap_field_modify(unit, port,
                            HIGIG_MODEf, xlp_hg_mode));
                }
            } else 
#endif  /* GREYHOUND2 */
            {
            SOC_IF_ERROR_RETURN
                (soc_greyhound_pgw_encap_field_modify(unit, port,
                     HIGIG_MODEf, xlp_hg_mode));
        }
        }
#endif /* BCM_GREYHOUND_SUPPORT */
        SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf,
                          xlp_hg_mode);
        if (soc_feature(unit, soc_feature_no_higig_plus)) {
            soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG2_MODEf,
                              xlp_hg_mode); 
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
    }
    if (SOC_REG_IS_VALID(unit, PORT_CONFIGr)) {
        SOC_IF_ERROR_RETURN(READ_PORT_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, PORT_CONFIGr, &rval, HIGIG_MODEf,
                          to_hg_port);
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
    }
    return rv;
}

/*
 * Function:
 *      mac_xl_encap_set
 * Purpose:
 *      Set the XLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_encap_set(int unit, soc_port_t port, int mode)
{
    int enable, encap, rv;
    int runt;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_encap_set: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_xl_encap_mode[mode]));
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

    SOC_IF_ERROR_RETURN(mac_xl_enable_get(unit, port, &enable));

    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_xl_enable_set(unit, port, 0));
    }

    if (soc_feature(unit, soc_feature_hg2_light_in_portmacro)) {
        /* mode update for all encap mode change! */
        SOC_IF_ERROR_RETURN(_mac_xl_port_mode_update(unit, port, mode));
    } else if ((IS_E_PORT(unit, port) && mode != SOC_ENCAP_IEEE) || 
            (IS_ST_PORT(unit, port) && mode == SOC_ENCAP_IEEE)) {
        /* IEEE -> HG or HG -> IEEE */
        SOC_IF_ERROR_RETURN(_mac_xl_port_mode_update(unit, port, mode));
    }

    /* apply encap setting */
    rv = soc_reg_field32_modify(unit, XLMAC_MODEr, port, HDR_MODEf, encap);

    /* assigning runt value per encap setting */
    if ((mode == SOC_ENCAP_HIGIG2) || (mode == SOC_ENCAP_HIGIG2_LITE)) {
        runt = XLMAC_RUNT_THRESHOLD_HG2;
    } else if (mode == SOC_ENCAP_HIGIG) {
        runt = XLMAC_RUNT_THRESHOLD_HG1;
    } else {
        runt = XLMAC_RUNT_THRESHOLD_IEEE;
    }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_CTRLr, port, RUNT_THRESHOLDf,
                                    runt));

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XLMAC_RX_CTRLr, port, STRICT_PREAMBLEf,
                                mode == SOC_ENCAP_IEEE ? 1 : 0));

    if (enable) {
        /* Re-enable transmitter and receiver */
        SOC_IF_ERROR_RETURN(mac_xl_enable_set(unit, port, 1));
    }

    return rv;
}

/*
 * Function:
 *      mac_xl_encap_get
 * Purpose:
 *      Get the XLMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_encap_get(int unit, soc_port_t port, int *mode)
{
    uint64 rval;

    if (!mode) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_XLMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, XLMAC_MODEr, rval, HDR_MODEf)) {
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
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_encap_get: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_xl_encap_mode[*mode]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_expected_rx_latency_get
 * Purpose:
 *      Get the XLMAC port expected Rx latency
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      latency - (OUT) Latency in NS
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_expected_rx_latency_get(int unit, soc_port_t port, int *latency)
{
    int speed = 0;
    SOC_IF_ERROR_RETURN(mac_xl_speed_get(unit, port, &speed));

    switch (speed) {
    case 1000:  /* GigE */
        *latency = 510; /* From SDK-69340 */
        break;

    case 10000:  /* 10G */
        *latency = 230; /* From SDK-69340 */
        break;

    default:
        *latency = 0;
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_control_set
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
mac_xl_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    uint64 rval, copy;
    uint32 fval;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_control_set: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval, RX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval, TX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_XLMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if (value < 0 || value > XLMAC_THROT_10G_TO_2P5G) {
            return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &rval));
            if ( value == XLMAC_THROT_10G_TO_5G ) {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, THROT_DENOMf, 21);
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, THROT_NUMf, 21);
            } else if (value == XLMAC_THROT_10G_TO_2P5G) {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, THROT_DENOMf, 21);
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, THROT_NUMf, 63);
            } else if (value >= 8) {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval,
                                      THROT_DENOMf, value);
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      1);
            } else {
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval,
                                      THROT_DENOMf, 0);
                soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      0);
            }
            SOC_IF_ERROR_RETURN(WRITE_XLMAC_TX_CTRLr(unit, port, rval));
        }
        return SOC_E_NONE;

    case SOC_MAC_PASS_CONTROL_FRAME:
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            if (SOC_REG_IS_VALID(unit, XLMAC_RX_CTRLr)) {
                SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &rval));
                soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rval,
                        RX_PASS_PAUSEf, (value ? 1 : 0));
                SOC_IF_ERROR_RETURN(WRITE_XLMAC_RX_CTRLr(unit, port, rval));
            }
            break;
        } else
#endif
        if (SOC_REG_IS_VALID(unit, XLMAC_PAUSE_CTRLr)) {
            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_PAUSE_CTRLr, port,
                                                   RX_PAUSE_ENf, value ? 0 : 1));

            SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_PAUSE_CTRLr, port,
                                                   TX_PAUSE_ENf, value ? 0 : 1));
        }
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_PFC_TYPEr, port,
                                                   PFC_ETH_TYPEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_PFC_OPCODEr,
                                                   port, PFC_OPCODEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (value != 8) {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0x00ffffff;
        fval |= (value & 0xff) << 24;
        soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

        soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                              value >> 8);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0xff000000;
        fval |= value;
        soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        /* this is always true */
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, RX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, TX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, FORCE_PFC_XONf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, PFC_STATS_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, 
                                    PFC_REFRESH_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_PFC_CTRLr, port, 
                                    PFC_XOFF_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_LLFC_CTRLr, port, RX_LLFC_ENf,
                                    value ? 1 : 0));
        SOC_IF_ERROR_RETURN 
            (soc_llfc_xon_set(unit, port, value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_LLFC_CTRLr, port, TX_LLFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_EEE_CTRLr, 
                                                   port, EEE_ENf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_EEE_TIMERSr, 
                                    port, EEE_DELAY_ENTRY_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XLMAC_EEE_TIMERSr, 
                                    port, EEE_WAKE_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_LSS_CTRLr, port,
                                    LOCAL_FAULT_DISABLEf, value ? 0 : 1));
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_LSS_CTRLr, port,
                                    REMOTE_FAULT_DISABLEf, value ? 0 : 1));
        break;
    case SOC_MAC_CONTROL_FAILOVER_RX_SET:
        break;
    case SOC_MAC_CONTROL_EGRESS_DRAIN:
        SOC_IF_ERROR_RETURN(mac_xl_egress_queue_drain(unit, port));
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_VLAN_TAGr, port, 
                                    OUTER_VLAN_TAGf, value));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_VLAN_TAGr, port, 
                                    OUTER_VLAN_TAG_ENABLEf, value ? 1 : 0));
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_VLAN_TAGr, port, 
                                    INNER_VLAN_TAGf, value));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_VLAN_TAGr, port, 
                                    INNER_VLAN_TAG_ENABLEf, value ? 1 : 0));
        break;
    case SOC_MAC_CONTROL_RX_RUNT_THRESHOLD:
        if ((value < XLMAC_RUNT_THRESHOLD_MIN) ||
            (value > XLMAC_RUNT_THRESHOLD_MAX)) {
            return SOC_E_PARAM;
        }
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XLMAC_RX_CTRLr, port, RUNT_THRESHOLDf,
                                    value));
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_xl_control_get
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
mac_xl_control_get(int unit, soc_port_t port, soc_mac_control_t type,
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
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_CTRLr, rval, RX_ENf);
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_CTRLr, rval, TX_ENf);
        break;
    case SOC_MAC_CONTROL_SW_RESET:
        SOC_IF_ERROR_RETURN(READ_XLMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_CTRLr, rval, SOFT_RESETf);
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        SOC_IF_ERROR_RETURN(READ_XLMAC_TX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_TX_CTRLr, rval,
                                       THROT_DENOMf);
        break;

    case SOC_MAC_CONTROL_TIMESTAMP_TRANSMIT:
         { 
            uint32  timestamp = 0;
            if (soc_feature(unit, soc_feature_timestamp_counter) &&
                soc_property_get(unit, spn_SW_TIMESTAMP_FIFO_ENABLE, 1)) {
                if (soc_counter_timestamp_get(unit, port,
                                              &timestamp) != SOC_E_NOT_FOUND) {
                    *value = (int)timestamp;
                    break;
                }
            }
        SOC_IF_ERROR_RETURN
            (READ_XLMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
        if (soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                  ENTRY_COUNTf) == 0) {
            return SOC_E_EMPTY;
        }
        SOC_IF_ERROR_RETURN
            (READ_XLMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_DATAr,
                                       rval, TIME_STAMPf);
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
#ifdef BCM_FIRELIGHT_SUPPORT
        if (SOC_IS_FIRELIGHT(unit)) {
            if (SOC_REG_IS_VALID(unit, XLMAC_RX_CTRLr)) {
                SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &rval));
                *value = soc_reg64_field32_get(unit,
                        XLMAC_RX_CTRLr, rval, RX_PASS_PAUSEf);
            }
            break;
        }
#endif
        *value = TRUE;
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_TYPEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_TYPEr, rval,
                                       PFC_ETH_TYPEf);
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_OPCODEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_OPCODEr, rval,
                                       PFC_OPCODEf);
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        *value = 8;
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &rval));
        fval0 = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval,
                                      PFC_MACDA_LOf);
        fval1 = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval,
                                      PFC_MACDA_HIf);
        *value = (fval0 >> 24) | (fval1 << 8);
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_DAr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval,
                                       PFC_MACDA_LOf) & 0x00ffffff;
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        *value = TRUE;
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, rval,
                                       RX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, rval,
                                       TX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, rval,
                                       FORCE_PFC_XONf);
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, rval,
                                       PFC_STATS_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, rval,
                                       PFC_REFRESH_TIMERf);
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN(READ_XLMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, rval,
                                       PFC_XOFF_TIMERf);
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, rval,
                                       RX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, rval,
                                       TX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_EEE_CTRLr, rval, EEE_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_EEE_TIMERSr, rval,
                EEE_DELAY_ENTRY_TIMERf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_XLMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_EEE_TIMERSr, rval,
                EEE_WAKE_TIMERf);
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, rval,
                                       LOCAL_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        if (soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, rval,
                                  LOCAL_FAULT_DISABLEf) == 1) {
            *value = 0;
        } else {
            SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_STATUSr(unit, port, &rval));
            *value = soc_reg64_field32_get(unit, XLMAC_RX_LSS_STATUSr, rval,
                                           LOCAL_FAULT_STATUSf);
        }
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, rval,
                                       REMOTE_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_CTRLr(unit, port, &rval));
        if (soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, rval,
                                  REMOTE_FAULT_DISABLEf) == 1) {
            *value = 0;
        } else {
            SOC_IF_ERROR_RETURN(READ_XLMAC_RX_LSS_STATUSr(unit, port, &rval));
            *value = soc_reg64_field32_get(unit, XLMAC_RX_LSS_STATUSr, rval,
                                           REMOTE_FAULT_STATUSf);
        }
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, 
                                       rval, OUTER_VLAN_TAGf);
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_VLAN_TAGr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, 
                                       rval, INNER_VLAN_TAGf);
        break;
    case SOC_MAC_CONTROL_EXPECTED_RX_LATENCY:
        SOC_IF_ERROR_RETURN(mac_xl_expected_rx_latency_get(unit, port, value));
        break;
    case SOC_MAC_CONTROL_RX_RUNT_THRESHOLD:
        SOC_IF_ERROR_RETURN(READ_XLMAC_RX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XLMAC_RX_CTRLr,
                                       rval, RUNT_THRESHOLDf);
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_control_get: unit %d port %s type=%d value=%d "
                            "rv=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, *value, rv));
    return rv;
}

/*
 * Function:
 *      mac_xl_ability_local_get
 * Purpose:
 *      Return the abilities of XLMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_xl_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
    soc_info_t *si = &SOC_INFO(unit);
    int blk, bindex, port_speed_max, i;
    int phy_port, active_port;
    uint32 active_mask;
#ifdef BCM_METROLITE_SUPPORT
    uint32 rval = 0;
    uint32 port_mode = 0xF;
#endif

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

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit) && IS_XL_PORT(unit,port)) {
        ability->encap |= SOC_PA_ENCAP_HIGIG2_LITE; 
        ability->speed_full_duplex = SOC_PA_ABILITY_NONE;
    } 
#endif

    /* Adjust port_speed_max according to the port config */
    port_speed_max = SOC_INFO(unit).port_speed_max[port];
    if (soc_feature(unit, soc_feature_logical_port_num) && 
        !soc_feature(unit, soc_feature_tsce)) {
        phy_port = si->port_l2p_mapping[port];
        bindex = -1;
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            if (SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_XLPORT) {
                bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
                break;
            }
        }
        if (port_speed_max > 10000) {
            active_mask = 0;
            for (i = bindex + 1; i <= 3; i++) {
                active_port = si->port_p2l_mapping[phy_port - bindex + i];
                if (active_port != -1 &&
                    !SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit, all),
                                     active_port)) {
                    active_mask |= 1 << i;
                }
            }
            if (bindex == 0) { /* Lanes 0 */
                if (active_mask & 0x2) { /* lane 1 is in use */
                    port_speed_max = 10000;
                } else if (port_speed_max > 20000 && active_mask & 0xc) {
                    /* Lane 1 isn't in use, lane 2 or 3 (or both) is (are) in use */
                    port_speed_max = 20000;
                }
            } else { /* (Must be) lanes 2 */
                if (active_mask & 0x8) { /* lane 3 is in use */
                    port_speed_max = 10000;
                }
            }
        }
    }
    if (IS_HL_PORT(unit , port)) {
        if (SOC_INFO(unit).port_speed_max[port]) {
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
            if (SOC_IS_HURRICANE2(unit)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
#ifdef BCM_METROLITE_SUPPORT
            /* HL is supported in 4x1G */
            if (SOC_IS_METROLITE(unit))  {
                if (SOC_REG_PORT_VALID(unit, XLPORT_MODE_REGr, port)) {
                    SOC_IF_ERROR_RETURN(READ_XLPORT_MODE_REGr(unit, port, &rval));
                    port_mode = soc_reg_field_get(unit, XLPORT_MODE_REGr, rval,
                                              XPORT0_CORE_PORT_MODEf);
                    /* Quad Port Mode. Lanes 0..3 individually active */
                    if (port_mode == 0x0){
                        ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
                    }
                }
            }
#endif
        }
    } else if (IS_HG_PORT(unit , port)) {
        switch (port_speed_max) {
        case 42000:
            ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            /* fall through */
        case 40000:
            ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            if(soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            }
            /* fall through */
        case 32000:
            ability->speed_full_duplex |= SOC_PA_SPEED_32GB;
            /* fall through */
        case 30000:
            ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
            /* fall through */
        case 25000:
            ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            /* fall through */
        case 21000:
            ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            /* fall through */
        case 20000:
            ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
            if(soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_21GB;
            }
            /* fall through */
        case 16000:
            ability->speed_full_duplex |= SOC_PA_SPEED_16GB;
            /* fall through */
        case 15000:
            ability->speed_full_duplex |= SOC_PA_SPEED_15GB;
            /* fall through */
        case 13000:
            ability->speed_full_duplex |= SOC_PA_SPEED_13GB;
            /* fall through */
        case 12000:
            ability->speed_full_duplex |= SOC_PA_SPEED_12GB;
            /* fall through */
        case 11000:
            ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            /* fall through */
        case 10000:
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
            if(soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
                ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
                /* For 11G and 5G speeds, MAC should be in 10G */
            }
            break;
        case 5000:
            if(soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
                /* For 5G speed, MAC will actually be set to 10G */
            }
            break;
        default:
            break;
        }
    } else {
        if (port_speed_max >= 40000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
        }
        if (port_speed_max >= 20000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
        }
        if (port_speed_max >= 10000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        if (port_speed_max >= 5000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_5000MB;
            /* For 5G speed, MAC will actually be set to 10G */
        }
        /* Temp fix running regression, saber2 check 
         * In Saber2, the xlmac is used for MXQ ports as well. 
         */
        if (soc_feature(unit, soc_feature_unified_port) || (SOC_IS_SABER2(unit))) {
            if (port_speed_max >= 2500) {
                ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
            }
            if (port_speed_max >= 1000) {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
            if (port_speed_max >= 100) {
                ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
            }
            if (port_speed_max >= 10) {
                ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_ability_local_get: unit %d port %s "
                            "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                            "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 ability->speed_half_duplex, ability->speed_full_duplex,
                 ability->encap, ability->pause, ability->interface,
                 ability->medium, ability->loopback, ability->flags));
    return SOC_E_NONE;
}

static int
mac_xl_timesync_tx_info_get (int unit, soc_port_t port,
                                        soc_port_timesync_tx_info_t *tx_info)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN
        (READ_XLMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
    if (soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                              ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }
    SOC_IF_ERROR_RETURN
        (READ_XLMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
    tx_info->timestamps_in_fifo = soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_DATAr,
                                   rval, TIME_STAMPf);
    tx_info->timestamps_in_fifo_hi = 0;
    tx_info->sequence_id = soc_reg64_field32_get(unit,
                XLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, SEQUENCE_IDf);

    return (SOC_E_NONE);
}

/* Exported XLMAC driver structure */
mac_driver_t soc_mac_xl = {
    "XLMAC Driver",               /* drv_name */
    mac_xl_init,                  /* md_init  */
    mac_xl_enable_set,            /* md_enable_set */
    mac_xl_enable_get,            /* md_enable_get */
    mac_xl_duplex_set,            /* md_duplex_set */
    mac_xl_duplex_get,            /* md_duplex_get */
    mac_xl_speed_set,             /* md_speed_set */
    mac_xl_speed_get,             /* md_speed_get */
    mac_xl_pause_set,             /* md_pause_set */
    mac_xl_pause_get,             /* md_pause_get */
    mac_xl_pause_addr_set,        /* md_pause_addr_set */
    mac_xl_pause_addr_get,        /* md_pause_addr_get */
    mac_xl_loopback_set,          /* md_lb_set */
    mac_xl_loopback_get,          /* md_lb_get */
    mac_xl_interface_set,         /* md_interface_set */
    mac_xl_interface_get,         /* md_interface_get */
    NULL,                         /* md_ability_get - Deprecated */
    mac_xl_frame_max_set,         /* md_frame_max_set */
    mac_xl_frame_max_get,         /* md_frame_max_get */
    mac_xl_ifg_set,               /* md_ifg_set */
    mac_xl_ifg_get,               /* md_ifg_get */
    mac_xl_encap_set,             /* md_encap_set */
    mac_xl_encap_get,             /* md_encap_get */
    mac_xl_control_set,           /* md_control_set */
    mac_xl_control_get,           /* md_control_get */
    mac_xl_ability_local_get,     /* md_ability_local_get */
    mac_xl_timesync_tx_info_get   /* md_timesync_tx_info_get */
 };

#endif /* BCM_XLMAC_SUPPORT */
