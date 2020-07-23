/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * XMAC driver
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
#ifdef BCM_KATANA2_SUPPORT
#include <soc/katana2.h>
#endif

#ifdef BCM_XMAC_SUPPORT

#define XMAC_RUNT_THRESHOLD_GE    0x40   /* Runt threshold value for GE ports */
#define XMAC_RUNT_THRESHOLD_XE    0x40   /* Runt threshold value for XE ports */
#define XMAC_RUNT_THRESHOLD_HG1   0x48   /* Runt threshold value for HG1 ports */
#define XMAC_RUNT_THRESHOLD_HG2   0x4c   /* Runt threshold value for HG2 ports */
#define IPG_BYTE(a)               ((a / 8) & 0x1f)

/*
 * XMAC Register field definitions.
 */

/* Transmit CRC Modes (Receive has bit field definitions in register) */
#define XMAC_CRC_APPEND          0x00   /* Append CRC (Default) */
#define XMAC_CRC_KEEP            0x01   /* CRC Assumed correct */
#define XMAC_CRC_REGEN           0x02   /* Replace CRC with a new one */
#define XMAC_CRC_RSVP            0x03   /* Reserved (does Replace) */

#define JUMBO_MAXSZ              0x3fe8 /* Max legal value (per regsfile) */

mac_driver_t soc_mac_x;

#ifdef BROADCOM_DEBUG
static char *mac_x_encap_mode[] = SOC_ENCAP_MODE_NAMES_INITIALIZER;
static char *mac_x_port_if_names[] = SOC_PORT_IF_NAMES_INITIALIZER;
#endif /* BROADCOM_DEBUG */

#define SOC_XMAC_SPEED_10     0x0
#define SOC_XMAC_SPEED_100    0x1
#define SOC_XMAC_SPEED_1000   0x2
#define SOC_XMAC_SPEED_2500   0x3
#define SOC_XMAC_SPEED_10000  0x4

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
typedef struct {
    uint64 mac_ctrl;
    uint64 rx_ctrl;
    uint64 tx_ctrl;
    uint64 pfc_ctrl;
    uint64 rx_max_size;
    uint64 mac_mode;
    uint64 pause_ctrl;
    uint64 rx_mac_sa;
    uint64 tx_mac_sa;
    uint64 pfc_da;
    uint64 llfc_ctrl;
    uint64 eee_ctrl;
    uint64 eee_timers;
    uint64 rx_lss_ctrl;
    uint64 rx_vlan_tag;
} _mac_x_cfg_t;
#endif

/* Forward declarations */
STATIC int mac_x_loopback_set(int unit, soc_port_t port, int lb);
STATIC int mac_x_loopback_get(int unit, soc_port_t port, int *lb);
STATIC int mac_x_frame_max_set(int unit, soc_port_t port, int size);
STATIC int mac_x_frame_max_get(int unit, soc_port_t port, int *size);
STATIC int mac_x_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx);
STATIC int mac_x_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx);

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/* xmac tx/rx clock rates for various speeds from xmac spec*/
struct {
    int speed;
    uint32 xmac_clk;
}_mac_x_clock_rate[] = {
    { 40000, 312 },
    { 20000, 156 },
    { 10000, 78  },
    { 2500,  312 },
    { 1000,  125 },
    { 100,   25  },
};
STATIC int
mac_x_speed_get(int unit, soc_port_t port, int *speed);

void
_mac_x_speed_to_clock_rate(int unit, soc_port_t port, int speed, uint32 *clk_rate)
{
    int idx;

    *clk_rate = 0;
    if (speed == _mac_x_clock_rate[0].speed) {
        /* Max Speed */
        *clk_rate = _mac_x_clock_rate[0].xmac_clk;
    } else {
        for (idx = 1; idx < sizeof(_mac_x_clock_rate) /
             sizeof(_mac_x_clock_rate[0]); idx++) {
            if ((speed >=_mac_x_clock_rate[idx].speed) &&
               (speed < _mac_x_clock_rate[idx-1].speed)) {
                *clk_rate = _mac_x_clock_rate[idx].xmac_clk;
                break;
            }
        } 
        if (*clk_rate == 0) {
            /* Min speed */
            *clk_rate = _mac_x_clock_rate[idx-1].xmac_clk;
        }
    }
}
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) */

#ifndef BCM_TRIUMPH3_SUPPORT
STATIC
#endif
int
_mac_x_drain_cells(int unit, soc_port_t port, int notify_phy)
{
    int         rv;
    int         pause_tx, pause_rx, pfc_rx, llfc_rx;
#ifdef BCM_KATANA_SUPPORT
    uint32      rval = 0;
#endif

    rv  = SOC_E_NONE;

    if (SOC_IS_RELOADING(unit)) {
        return rv;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    /* Skipping in for Disabled Ports */
    if (SOC_IS_TRIUMPH3(unit)) {
        if (SOC_PBMP_MEMBER(SOC_PORT_DISABLED_BITMAP(unit,all), port)) {
            return SOC_E_NONE;
        }
    }
#endif

#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANAX(unit)) {
        READ_XP_TXFIFO_PKT_DROP_CTLr(unit, port, &rval);
        soc_reg_field_set(unit, XP_TXFIFO_PKT_DROP_CTLr, &rval,
                               DROP_ENf, 1);
        WRITE_XP_TXFIFO_PKT_DROP_CTLr(unit, port, rval);
    }
#endif /* BCM_KATANA2_SUPPORT */
    /* Drain cells in mmu/port before cells entering TX FIFO */
    SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, TRUE));

    /* Disable pause/pfc/llfc function */
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_pause_get(unit, port, &pause_tx, &pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_pause_set(unit, port, pause_tx, 0));

    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_control_get(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                  &pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                  0));

    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_control_get(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                  &llfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                  0));

    /* Drain data in TX FIFO without egressing */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XMAC_TX_CTRLr, port, DISCARDf, 1));

    if (notify_phy == 1) {
    /* Notify PHY driver */
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventStop, PHY_STOP_DRAIN));
    } 
    /* Wait until mmu cell count is 0 */
    rv = soc_egress_drain_cells(unit, port, 250000);
    if (SOC_E_NONE == rv) {
        /* Wait until TX fifo cell count is 0 */
        rv = soc_txfifo_drain_cells(unit, port, 250000);
    }

    /* Notify PHY driver */
    if (notify_phy == 1) {
        SOC_IF_ERROR_RETURN
            (soc_phyctrl_notify(unit, port, phyEventResume, PHY_STOP_DRAIN));
    }
    /* Stop TX FIFO drainging */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XMAC_TX_CTRLr, port, DISCARDf, 0));

    /* Restore original pause/pfc/llfc configuration */
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_pause_set(unit, port, pause_tx, pause_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_control_set(unit, port, SOC_MAC_CONTROL_PFC_RX_ENABLE,
                                  pfc_rx));
    SOC_IF_ERROR_RETURN
        (soc_mac_x.md_control_set(unit, port, SOC_MAC_CONTROL_LLFC_RX_ENABLE,
                                  llfc_rx));
    
#ifdef BCM_KATANA_SUPPORT
     if (SOC_IS_KATANAX(unit)) {
         READ_XP_TXFIFO_PKT_DROP_CTLr(unit, port, &rval);
         soc_reg_field_set(unit, XP_TXFIFO_PKT_DROP_CTLr, &rval,
                               DROP_ENf, 0);
         WRITE_XP_TXFIFO_PKT_DROP_CTLr(unit, port, rval);
     } else
#endif
     {
         /* Stop draining cells in mmu/port */
         SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));
     }

    return rv;
}

#if defined(BCM_SHADOW_SUPPORT)
/* Shadow specific initializations */

STATIC int
__shadow_petrab_mac_x_init(int unit, soc_port_t port)
{
    uint64 reg64;

    /* add Rx+Tx CRC insert/terminate */
    SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &reg64));
    soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &reg64, CRC_MODEf, XMAC_CRC_APPEND);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, port, reg64));

    /* xmac Rx ctrl */
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &reg64));
    soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &reg64, STRIP_CRCf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, reg64));

    /* XMAC Pause Ctrl */
    SOC_IF_ERROR_RETURN (READ_XMAC_PAUSE_CTRLr(unit, port, &reg64));
    soc_reg64_field32_set(unit, XMAC_PAUSE_CTRLr, &reg64, 
                          RX_PAUSE_ENf, 0);
    soc_reg64_field32_set(unit, XMAC_PAUSE_CTRLr, &reg64, 
                          TX_PAUSE_ENf, 0);
    soc_reg64_field32_set(unit, XMAC_PAUSE_CTRLr, &reg64, 
                          PAUSE_REFRESH_ENf, 0);
    SOC_IF_ERROR_RETURN (WRITE_XMAC_PAUSE_CTRLr(unit, port, reg64));

    /* xmac pfc ctrl */ 
    SOC_IF_ERROR_RETURN (READ_XMAC_PFC_CTRLr(unit, port, &reg64));
    soc_reg64_field32_set(unit, XMAC_PFC_CTRLr, &reg64, 
                          PFC_REFRESH_ENf, 1);
    soc_reg64_field32_set(unit, XMAC_PFC_CTRLr, &reg64, 
                          PFC_STATS_ENf, 1);
    soc_reg64_field32_set(unit, XMAC_PFC_CTRLr, &reg64, 
                          RX_PFC_ENf, 1);
    soc_reg64_field32_set(unit, XMAC_PFC_CTRLr, &reg64, 
                          TX_PFC_ENf, 1);
    SOC_IF_ERROR_RETURN (WRITE_XMAC_PFC_CTRLr(unit, port, reg64));

    /* Set Max pkt Size */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XMAC_RX_MAX_SIZEr, port,
                                RX_MAX_SIZEf, 16256)); 

    return SOC_E_NONE;
}

STATIC int
__shadow_mac_x_init(int unit, soc_port_t port)
{
    uint64 tx_ctrl, rval, data;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "__shadow_mac_x_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    {
        if (IS_XE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(READ_XMAC_RX_MAX_SIZEr(unit, port, &data));
            soc_reg64_field32_set(unit, XMAC_RX_MAX_SIZEr, &data, RX_MAX_SIZEf, 9600);
            SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAX_SIZEr(unit, port, data));

            /*  Initialize additional device mode xmac configurations */
            if (soc_property_get(unit, spn_BCM88732_DEVICE_MODE, 0)) {
                SOC_IF_ERROR_RETURN(__shadow_petrab_mac_x_init(unit, port));
            }
        }
    }

    SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &tx_ctrl));
    soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, PAD_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, port, tx_ctrl));

    /* Setup header mode, check for property for bcm5632 mode */
    COMPILER_64_ZERO(rval);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_MODEr(unit, port, rval));

    return SOC_E_NONE;
}
#endif /* BCM_SHADOW_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT_SUPPORT)
STATIC int _mac_x_register_store(int unit, soc_port_t port, _mac_x_cfg_t *port_cfg)
{
    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &port_cfg->mac_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &port_cfg->rx_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &port_cfg->tx_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &port_cfg->pfc_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_MAX_SIZEr(unit, port, &port_cfg->rx_max_size));
    SOC_IF_ERROR_RETURN(READ_XMAC_MODEr(unit, port, &port_cfg->mac_mode));
    SOC_IF_ERROR_RETURN(READ_XMAC_PAUSE_CTRLr(unit, port, &port_cfg->pause_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_MAC_SAr(unit, port, &port_cfg->rx_mac_sa));
    SOC_IF_ERROR_RETURN(READ_XMAC_TX_MAC_SAr(unit, port, &port_cfg->tx_mac_sa));
    SOC_IF_ERROR_RETURN(READ_XMAC_PFC_DAr(unit, port, &port_cfg->pfc_da));
    SOC_IF_ERROR_RETURN(READ_XMAC_LLFC_CTRLr(unit, port, &port_cfg->llfc_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_EEE_CTRLr(unit, port, &port_cfg->eee_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_EEE_TIMERSr(unit, port, &port_cfg->eee_timers));
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_LSS_CTRLr(unit, port, &port_cfg->rx_lss_ctrl));
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_VLAN_TAGr(unit, port, &port_cfg->rx_vlan_tag));
    return SOC_E_NONE;
}

STATIC int _mac_x_register_restore(int unit, soc_port_t port, _mac_x_cfg_t *port_cfg)
{
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, port_cfg->mac_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, port_cfg->rx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, port, port_cfg->tx_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_PFC_CTRLr(unit, port, port_cfg->pfc_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAX_SIZEr(unit, port, port_cfg->rx_max_size));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_MODEr(unit, port, port_cfg->mac_mode));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_PAUSE_CTRLr(unit, port, port_cfg->pause_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAC_SAr(unit, port, port_cfg->rx_mac_sa));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_MAC_SAr(unit, port, port_cfg->tx_mac_sa));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_PFC_DAr(unit, port, port_cfg->pfc_da));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_LLFC_CTRLr(unit, port, port_cfg->llfc_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_EEE_CTRLr(unit, port, port_cfg->eee_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_EEE_TIMERSr(unit, port, port_cfg->eee_timers));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_LSS_CTRLr(unit, port, port_cfg->rx_lss_ctrl));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_VLAN_TAGr(unit, port, port_cfg->rx_vlan_tag));
    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcm_port_mac_reinit
 * Purpose:
 *      Resets the MAC and restores the configured values.
 */

int
_mac_x_reinit(int unit, soc_port_t port)
{
    _mac_x_cfg_t *pcfg;
    static const soc_field_t mac_field[] = {
        XMAC0_RESETf,
        XMAC1_RESETf,
        XMAC2_RESETf
    };

    int blk_num = 0, phy_port, phy_port_base = 0;
    soc_port_t l_port =0;
    soc_info_t *si;
    int core_index = -1, i;
    uint32 rval;
#if defined(BCM_KATANA2_SUPPORT)
    uint8 mxqblock = 0, mxqblock_port = 0;
#endif
    si = &SOC_INFO(unit);

    phy_port = (soc_feature(unit, soc_feature_logical_port_num))
               ? si->port_l2p_mapping[port] : port;
    if (phy_port == -1) {
        return SOC_E_INTERNAL;
    }

    core_index = SOC_PORT_BINDEX(unit, phy_port) / 4;
    pcfg = (_mac_x_cfg_t *) sal_alloc(
                    sizeof(_mac_x_cfg_t) * 4,"MAC config");
    if (pcfg == NULL) {
        return SOC_E_MEMORY;
    }
    /* Store MAC settings */
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_CLEAN_RETURN(soc_katana2_get_port_mxqblock(
                                  unit, port, &mxqblock), sal_free(pcfg));
        for (i = 0; i < KT2_MAX_MXQPORTS_PER_BLOCK; i++) {
             mxqblock_port = (*kt2_mxqblock_ports[unit])[mxqblock][i];
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
                 SOC_IF_ERROR_CLEAN_RETURN
                     (_mac_x_register_store(unit, mxqblock_port, &pcfg[i]),
                      sal_free(pcfg));
             }
        }
    } else
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) {
        phy_port_base = ((phy_port - 1)/4)*4 + 1;
        for (i = 0; i < 4; i++) {
            l_port = si->port_p2l_mapping[phy_port_base+i];
            if(l_port == -1) {
                continue;
            }
            if(0 == si->port_num_lanes[l_port]) {
                continue;
            }
            SOC_IF_ERROR_CLEAN_RETURN
                (_mac_x_register_store(unit, l_port, &pcfg[i]), sal_free(pcfg));
        }
    } else
#endif
    {
        blk_num = SOC_PORT_BLOCK(unit, phy_port);
        phy_port_base = si->blk_fpp[blk_num] + core_index*4;
        for (i = 0; i < 4; i++) {
            l_port = si->port_p2l_mapping[phy_port_base+i];
            if(l_port == -1) {
                continue;
            }
            if(0 == si->port_num_lanes[l_port]) {
                continue;
            }
            SOC_IF_ERROR_CLEAN_RETURN
                (_mac_x_register_store(unit, l_port, &pcfg[i]), sal_free(pcfg));
        }
    }
    /* Hard reset */
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        SOC_IF_ERROR_CLEAN_RETURN(READ_XPORT_XMAC_CONTROLr(unit, port, &rval), sal_free(pcfg));
        soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 1);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, rval), sal_free(pcfg));
        sal_udelay(10);
        soc_reg_field_set(unit, XPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 0);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_XPORT_XMAC_CONTROLr(unit, port, rval), sal_free(pcfg));
    } else
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) {
        SOC_IF_ERROR_CLEAN_RETURN(READ_XLPORT_XMAC_CONTROLr(unit, port, &rval), sal_free(pcfg));
        soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 1);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval), sal_free(pcfg));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_XMAC_CONTROLr, &rval, XMAC_RESETf, 0);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_XLPORT_XMAC_CONTROLr(unit, port, rval), sal_free(pcfg));
    } else
#endif
    {
        SOC_IF_ERROR_CLEAN_RETURN(READ_PORT_MAC_CONTROLr(unit, port, &rval),sal_free(pcfg));
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                          mac_field[core_index], 1);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval),sal_free(pcfg));
        sal_udelay(10);
        soc_reg_field_set(unit, PORT_MAC_CONTROLr, &rval,
                          mac_field[core_index], 0);
        SOC_IF_ERROR_CLEAN_RETURN(WRITE_PORT_MAC_CONTROLr(unit, port, rval),sal_free(pcfg));
    }
    sal_udelay(10);
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        /* Restore MAC settings. */
        SOC_IF_ERROR_CLEAN_RETURN(soc_katana2_get_port_mxqblock(
                                  unit, port, &mxqblock), sal_free(pcfg));
        for (i = 0; i < KT2_MAX_MXQPORTS_PER_BLOCK; i++) {
             mxqblock_port = (*kt2_mxqblock_ports[unit])[mxqblock][i];
             if (SOC_PBMP_MEMBER((PBMP_ALL(unit)), mxqblock_port)) {
                 SOC_IF_ERROR_CLEAN_RETURN
                     (_mac_x_register_restore(unit, mxqblock_port, &pcfg[i]),
                      sal_free(pcfg));
             }
        }
    } else
#endif
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) {
        for (i = 0; i < 4; i++) {
            l_port = si->port_p2l_mapping[phy_port_base+i];
            if(l_port == -1) {
                continue;
            }
            if(0 == si->port_num_lanes[l_port]) {
                continue;
            }
            /* Restore MAC settings. */
            SOC_IF_ERROR_CLEAN_RETURN
                (_mac_x_register_restore(unit, l_port, &pcfg[i]), sal_free(pcfg));
        }
    } else
#endif
    {
        for (i = 0; i < 4; i++) {
            l_port = si->port_p2l_mapping[phy_port_base+i];
            if(l_port == -1) {
                continue;
            }
            if(0 == si->port_num_lanes[l_port]) {
                continue;
            }
            /* Restore MAC settings. */
            SOC_IF_ERROR_CLEAN_RETURN
                (_mac_x_register_restore(unit, l_port, &pcfg[i]), sal_free(pcfg));
        }
    }
    sal_free(pcfg);
    return SOC_E_NONE;
}
#endif


/*
 * Function:
 *      mac_x_init
 * Purpose:
 *      Initialize Xmac into a known good state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_x_init(int unit, soc_port_t port)
{
    soc_info_t *si;
    uint64 mac_ctrl, rx_ctrl, tx_ctrl, rval;
    uint32 ipg;
    int mode;
    
#if defined(BCM_KATANA2_SUPPORT)
    int                 sgn_det = 0;
    uint32              sgn_crs;
#endif
    
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_init: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    si = &SOC_INFO(unit);

    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &mac_ctrl));
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, mac_ctrl));

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &mac_ctrl));

    if (IS_HG_PORT(unit, port)) {
        soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, SOFT_RESETf, 0);
    }

    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    if ((si->port_speed_max[port] == 40000) || 
        (si->port_speed_max[port]) == 42000) {
        int xlgmii_align = 0;

        if (IS_XE_PORT(unit, port)) { 
            xlgmii_align = 1; 
        } else { /* HG port with external PHY may also use IEEE 40G mld-4 */ 
            if (PHY_EXTERNAL_MODE(unit, port)
                && (!PHY_PASSTHRU_MODE(unit, port))
                && (!soc_property_port_get(unit, port, 
                                           spn_PORT_PHY_MODE_REVERSE, 0))) {
                xlgmii_align = 1; 
            } 
        } 
        soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, XLGMII_ALIGN_ENBf,
                              xlgmii_align);
    }
    if (soc_reg_field_valid(unit, XMAC_CTRLr, XGMII_IPG_CHECK_DISABLEf)) {
        soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl,
                              XGMII_IPG_CHECK_DISABLEf,
                              IS_HG_PORT(unit, port) ? 1 : 0);
    }

    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, mac_ctrl));

    SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &rx_ctrl));
    soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 0);

#ifdef  BCM_TRIUMPH3_SUPPORT 
    if (SOC_IS_TRIUMPH3(unit) && IS_HG_PORT(unit, port)) {
        soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf,
                              XMAC_RUNT_THRESHOLD_HG1);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, rx_ctrl));


    /* XMAC_TX_CTRL settings */
    SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &tx_ctrl));
    if(IS_XE_PORT(unit,port) || IS_GE_PORT(unit, port)) {
       ipg = SOC_PERSIST(unit)->ipg[port].fd_xe;
    } else {
        ipg = soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0) ?
              SOC_PERSIST(unit)->ipg[port].fd_hg2 :
              SOC_PERSIST(unit)->ipg[port].fd_hg;
    }
    soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf,
                          IPG_BYTE(ipg));
    soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf, 3);
#if defined(BCM_HELIX4_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
     if (SOC_IS_TRIUMPH3(unit)) {
         if ((si->port_speed_max[port] > 1000 &&
             si->port_speed_max[port] <= 3000) ||
             IS_XL_PORT(unit, port)) {
             soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, 
                                    TX_64BYTE_BUFFER_ENf, 1);
         }
     }
#endif /* defined(BCM_HELIX4_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) */
#ifdef BCM_KATANA2_SUPPORT
    if (SOC_IS_KATANA2(unit)) {
        if (si->port_speed_max[port] <= 2500) {
            soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, 
                                    TX_64BYTE_BUFFER_ENf, 1);
        }
        if (SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
            soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf, 0);
        }
    }
#endif
    SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, port, tx_ctrl));

    if (IS_ST_PORT(unit, port)) {
        soc_mac_x.md_pause_set(unit, port, FALSE, FALSE);
    } else {
        soc_mac_x.md_pause_set(unit, port, TRUE, TRUE);
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, PFC_REFRESH_ENf,
                                1));

    if (soc_property_port_get(unit, port, spn_PHY_WAN_MODE, FALSE)) {
        /* Max speed for WAN mode is 9.294Gbps.
         * This setting gives 10Gbps * (13/14) or 9.286 Gbps */
        SOC_IF_ERROR_RETURN
            (soc_mac_x.md_control_set(unit, port,
                                      SOC_MAC_CONTROL_FRAME_SPACING_STRETCH,
                                      13));
    }

    /* Set jumbo max size (8000 byte payload) */
    COMPILER_64_ZERO(rval);
#ifdef BCM_XGS_SUPPORT
    if (SOC_IS_XGS(unit)) {
        soc_reg64_field32_set(unit, XMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                              SOC_INFO(unit).max_mtu); 
    } else
#endif
    {
        soc_reg64_field32_set(unit, XMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf,
                              JUMBO_MAXSZ); 
    }
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAX_SIZEr(unit, port, rval));

    /* Setup header mode, check for property for higig2 mode */
    COMPILER_64_ZERO(rval);
    if (!IS_XE_PORT(unit, port) && !IS_GE_PORT(unit, port)) {
        mode = soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0) ?
            2 : 1;
        soc_reg64_field32_set(unit, XMAC_MODEr, &rval, HDR_MODEf, mode);
    }
    if (soc_reg_field_valid(unit, XMAC_MODEr, SPEED_MODEf)) {
        switch (si->port_speed_max[port]) {
        case 10:
            mode = SOC_XMAC_SPEED_10;
            break;
        case 100:
            mode = SOC_XMAC_SPEED_100;
            break;
        case 1000:
            mode = SOC_XMAC_SPEED_1000;
            break;
        case 2500:
        case 3000:
            mode = SOC_XMAC_SPEED_2500;
            break;
        default:
            mode = SOC_XMAC_SPEED_10000;
            break;
        }
        soc_reg64_field32_set(unit, XMAC_MODEr, &rval, SPEED_MODEf, mode);
    }
    SOC_IF_ERROR_RETURN(WRITE_XMAC_MODEr(unit, port, rval));

    /* XMAC_RX_CTRL settings */
    SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &rx_ctrl));

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT) || \
    defined(BCM_KATANA_SUPPORT)

    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_KATANA2(unit) ) {
        if (IS_GE_PORT(unit, port)) {
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  STRICT_PREAMBLEf, 0);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  PROCESS_VARIABLE_PREAMBLEf, 1);
	    soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval,
                                  RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_GE);
            
        } else if (IS_XE_PORT(unit, port)) {
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  STRICT_PREAMBLEf, 1);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  PROCESS_VARIABLE_PREAMBLEf, 0);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval,
                                  RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_XE);

        } else if (IS_HG_PORT(unit, port)) {
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  STRICT_PREAMBLEf, 0);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  PROCESS_VARIABLE_PREAMBLEf, 0);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, 
                    RX_PASS_CTRLf, 1);
            if (soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0)) {
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, 
                                      RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_HG2);
            } else {
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, 
                                      RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_HG1);
            }

        }
    } else if (SOC_IS_KATANA(unit)) {
          if (IS_GE_PORT(unit, port) || IS_XE_PORT(unit, port)) {
              soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                    STRICT_PREAMBLEf, 0);
              soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                  PROCESS_VARIABLE_PREAMBLEf, 1);
              soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                    RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_XE);
          } else if (IS_HG_PORT(unit, port)) {
              soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                    STRICT_PREAMBLEf, 0);
              soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl,
                                    PROCESS_VARIABLE_PREAMBLEf, 0);
              soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, 
                       RX_PASS_CTRLf, 1);
              if (soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0)) {
                  soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, 
                                    RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_HG2);
              } else {
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, 
                                      RUNT_THRESHOLDf, XMAC_RUNT_THRESHOLD_HG1);
              }
          }
   }

   if (SOC_IS_KATANA2(unit) && SOC_PBMP_MEMBER(si->linkphy_pbm, port)) {
      soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 1);
   }
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_KATANA2_SUPPORT || 
          BCM_KATANA_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT)
    if ((SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) && IS_HG_PORT(unit, port)) {
        soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf,
                      soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE, 0)?
                             XMAC_RUNT_THRESHOLD_HG2 : XMAC_RUNT_THRESHOLD_HG1);
    }
#endif /* BCM_TRIDENT_SUPPORT*/
   SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, rx_ctrl));


#if defined(BCM_SHADOW_SUPPORT)
    /* Shadow Specific Initialization */
    if (SOC_IS_SHADOW(unit)) {
        SOC_IF_ERROR_RETURN(__shadow_mac_x_init(unit, port));
    }
#endif /* BCM_SHADOW_SUPPORT */

    /* Disable loopback and bring XMAC out of reset */
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, CORE_REMOTE_LPBKf, 0);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, LINE_REMOTE_LPBKf, 0);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, CORE_LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, LINE_LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &mac_ctrl, TX_ENf, 1);

    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, mac_ctrl));

#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit)) {
        sgn_det = soc_property_port_get(unit, port,  spn_SERDES_RX_LOS, 0);
        SOC_IF_ERROR_RETURN(READ_X_GPORT_SGNDET_EARLYCRSr(unit, port, &sgn_crs));
        soc_reg_field_set(unit, X_GPORT_SGNDET_EARLYCRSr, &sgn_crs, SGN_DETf, sgn_det);
        SOC_IF_ERROR_RETURN(WRITE_X_GPORT_SGNDET_EARLYCRSr(unit, port, sgn_crs));
    }
#endif

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_enable_set
 * Purpose:
 *      Drain the egress queues with out bringing down the port
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_egress_queue_drain(int unit, soc_port_t port)
{
    uint64 ctrl;
    pbmp_t mask;
    int rx_enable = 0;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_egress_queue_drain_set: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &ctrl));

    rx_enable = soc_reg64_field32_get(unit, XMAC_CTRLr, ctrl, RX_ENf);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, RX_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));
    sal_udelay(10);

    soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));

    soc_link_mask2_get(unit, &mask);
    SOC_PBMP_PORT_REMOVE(mask, port);
    SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
    SOC_IF_ERROR_RETURN(_mac_x_drain_cells(unit, port, 0));

    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &ctrl));

        /* Remove SOFT_RESET before RX_EN */
    soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));

    soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, RX_ENf, rx_enable ? 1 : 0);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));
    sal_udelay(10);

    soc_link_mask2_get(unit, &mask);
    SOC_PBMP_PORT_ADD(mask, port);
    SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));

    return SOC_E_NONE;
}
/*
 * Function:
 *      mac_x_enable_set
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
mac_x_enable_set(int unit, soc_port_t port, int enable)
{
    uint64 ctrl;
    pbmp_t mask;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_enable_set: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 enable ? "True" : "False"));

    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &ctrl));

    if (enable) {
        /* Remove SOFT_RESET before RX_EN */
#ifdef BCM_KATANA_SUPPORT
        if (SOC_IS_KATANAX(unit)) {
            SOC_IF_ERROR_RETURN(soc_mmu_flush_enable(unit, port, FALSE));
        }
#endif
        soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));

        soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, TX_ENf, 1);
        soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, RX_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));
        sal_udelay(10);

        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_ADD(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventResume,
                                               PHY_STOP_MAC_DIS));
    } else {
        soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, RX_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));
        sal_udelay(10);

        soc_reg64_field32_set(unit, XMAC_CTRLr, &ctrl, SOFT_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, ctrl));

        soc_link_mask2_get(unit, &mask);
        SOC_PBMP_PORT_REMOVE(mask, port);
        SOC_IF_ERROR_RETURN(soc_link_mask2_set(unit, mask));
        SOC_IF_ERROR_RETURN(_mac_x_drain_cells(unit, port, 1));
        SOC_IF_ERROR_RETURN(soc_phyctrl_notify(unit, port, phyEventStop,
                                               PHY_STOP_MAC_DIS));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_enable_get
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
mac_x_enable_get(int unit, soc_port_t port, int *enable)
{
    uint64 ctrl;

    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &ctrl));

    *enable = soc_reg64_field32_get(unit, XMAC_CTRLr, ctrl, RX_ENf);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_enable_get: unit %d port %s enable=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *enable ? "True" : "False"));

    return SOC_E_NONE;
}

#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
/*
 * Function:
 *      _mac_x_timestamp_delay_set
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
_mac_x_timestamp_delay_set(int unit, soc_port_t port, int speed, int phy_mode)
{
    uint64 ctrl;
    uint32 clk_rate, tx_clk_ns;
    int osts_delay, delay;
    uint32 field;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_timestamp_delay_set: unit %d port %s\n"),
                 unit, SOC_PORT_NAME(unit, port)));

    if (SOC_REG_IS_VALID(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr)) {
        SOC_IF_ERROR_RETURN(READ_XMAC_OSTS_TIMESTAMP_ADJUSTr(unit, port, &ctrl));

        _mac_x_speed_to_clock_rate(unit, port, speed, &clk_rate);
        /* Tx clock rate for single/dual/quad phy mode */
        if ((speed >= 10000) && (speed <= 40000)) {
            /* tx clock rate in ns */
            tx_clk_ns = (((1000/clk_rate))/ (1 << phy_mode));
        } else {
            tx_clk_ns = 0;
        }

        
        
        /*
         * MAC pipeline delay for XGMII/XLGMII mode is : ( as in XMAC specification)
         *          = (5.5 * TX line clock period) + (Timestamp clock period)
         */
        /* signed value of pipeline delay in ns */
        osts_delay = soc_property_port_get(unit, port, spn_TIMESTAMP_ADJUST(speed),
                                           SOC_TIMESYNC_PLL_CLOCK_NS(unit) -
                                           ((11 * tx_clk_ns ) / 2));

        if (SOC_E_NONE != soc_reg_signed_field_mask(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr,
                                                    TS_ADJUSTf, osts_delay, &field)) {
            LOG_WARN(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "%s property out of bounds (is %d)\n"),
                spn_TIMESTAMP_ADJUST(speed), osts_delay));
            field = 0;
        }

        soc_reg64_field32_set(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUSTf, field);
        /*
         * Lane delay for xmac lanes
         *   Lane_0(0-3)  : 1 * TX line clock period
         *   Lane_1(4-7)  : 2 * TX line clock period
         *   Lane_2(8-11) : 3 * TX line clock period
         *   Lane_3(12-15): 4 * TX line clock period
         */
        /* unsigned value of lane delay in ns */
        delay = 1 * tx_clk_ns;
        soc_reg64_field32_set(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_0f, delay );
        delay = 2 * tx_clk_ns;
        soc_reg64_field32_set(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_1f, delay );
        delay = 3 * tx_clk_ns;
        soc_reg64_field32_set(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_2f, delay );
        delay = 4 * tx_clk_ns;
        soc_reg64_field32_set(unit, XMAC_OSTS_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_ADJUST_DEMUX_DELAY_3f, delay );

        SOC_IF_ERROR_RETURN(WRITE_XMAC_OSTS_TIMESTAMP_ADJUSTr(unit, port, ctrl));
    }

    return SOC_E_NONE;
}
#endif /* defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) */

/*
 * Function:
 *      mac_x_duplex_set
 * Purpose:
 *      Set XMAC in the specified duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      duplex - Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
mac_x_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_duplex_get
 * Purpose:
 *      Get XMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_pause_set
 * Purpose:
 *      Configure XMAC to transmit/receive pause frames.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pause_tx - Boolean: transmit pause or -1 (don't change)
 *      pause_rx - Boolean: receive pause or -1 (don't change)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx)
{
    soc_field_t fields[2] = { TX_PAUSE_ENf, RX_PAUSE_ENf };
    uint32 values[2];

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_pause_set: unit %d port %s TX=%s RX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 pause_tx != FALSE ? "on" : "off",
                 pause_rx != FALSE ? "on" : "off"));

    if (SOC_IS_TD_TT(unit)) {
        fields[1] = RX_PASS_PAUSEf;
    }
    values[0] = pause_tx != FALSE ? 1 : 0;
    values[1] = pause_rx != FALSE ? 1 : 0;
    return soc_reg_fields32_modify(unit, XMAC_PAUSE_CTRLr, port, 2,
                                   fields, values);
}

/*
 * Function:
 *      mac_x_pause_get
 * Purpose:
 *      Return the pause ability of XMAC
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
mac_x_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx)
{
    soc_field_t field = RX_PAUSE_ENf;
    uint64 rval;

    if (SOC_IS_TD_TT(unit)) {
        field = RX_PASS_PAUSEf;
    }

    SOC_IF_ERROR_RETURN(READ_XMAC_PAUSE_CTRLr(unit, port, &rval));
    *pause_tx =
        soc_reg64_field32_get(unit, XMAC_PAUSE_CTRLr, rval, TX_PAUSE_ENf);
    *pause_rx = soc_reg64_field32_get(unit, XMAC_PAUSE_CTRLr, rval, field);

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_pause_get: unit %d port %s TX=%s RX=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *pause_tx ? "on" : "off",
                 *pause_rx ? "on" : "off"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_speed_set
 * Purpose:
 *      Set XMAC in the specified speed.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - 10000, 15000, 20000, 30000, 40000.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_speed_set(int unit, soc_port_t port, int speed)
{
    soc_port_if_t intf;
#ifdef BCM_UNIPORT_SUPPORT
    static const soc_field_t core_mode_field[] = {
        XPORT0_CORE_PORT_MODEf, 
        XPORT1_CORE_PORT_MODEf,
        XPORT2_CORE_PORT_MODEf
    };
    static const soc_field_t phy_mode_field[] = {
        XPORT0_PHY_PORT_MODEf,
        XPORT1_PHY_PORT_MODEf,
        XPORT2_PHY_PORT_MODEf
    };
    static const soc_field_t xpc_gmii_enable_field[] = {
        XPC0_GMII_MII_ENABLEf,
        XPC1_GMII_MII_ENABLEf,
        XPC2_GMII_MII_ENABLEf
    };
#endif /* BCM_UNIPORT_SUPPORT */
    uint64 rval64;
    int xlgmii_align;
#if defined(BCM_TRIDENT_SUPPORT)
    int ipg_check_disable;
#endif

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_speed_set: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed));

#ifdef BCM_KATANA_SUPPORT
    /* We should be looking for Ver B0 in  XMAC_VERSION_ID reg. But this reg
     is not implemented in Katana A0. So, check for SPEED_MODE field instead */
    if (SOC_IS_KATANAX(unit) && 
        soc_reg_field_valid(unit, XMAC_MODEr, SPEED_MODEf)) {
        int speed_select, enable;
        uint32 phy_mode = 0;
        uint32 rval, rval_orig;
        uint32 strict_preamble = 1;
        int  same_mxqblock_max_port_speed = 0;
#ifdef BCM_KATANA2_SUPPORT
        uint32 core_mode = 0;
        uint32 wc_10g_21g_sel = 0;
        uint8  mxqblock = 0;
        bcmMxqConnection_t connection_mode;
        int  other_port_speed = 0;
        int loop;
#endif

        switch (speed) {
        case 10:
            strict_preamble = 0;
            speed_select = SOC_XMAC_SPEED_10;
            break;
        /* support non-standard speed in Broadreach mode */
        case 20:
        case 25:
        case 33:
        case 50:
            /* fall through to case 100 */
        case 100:
            strict_preamble = 0;
            speed_select = SOC_XMAC_SPEED_100;
            break;
        case 1000:
            strict_preamble = 0;
            speed_select = SOC_XMAC_SPEED_1000;
            break;
        case 2500:
            strict_preamble = 0;
            speed_select = SOC_XMAC_SPEED_2500;
            break;
        case 10000:
        case 12000:
        case 13000:
        case 15000:
        case 16000:
            speed_select = SOC_XMAC_SPEED_10000;
            break;
        case 0:
            return (SOC_E_NONE);              /* Support NULL PHY */            
        default:
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit) && ((speed == 21000) || (speed == 20000))){
                  speed_select = SOC_XMAC_SPEED_10000;
                  break;
            }
#endif
            return (SOC_E_CONFIG);
        }
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &rval64));
        if (IS_HG_PORT(unit, port)) {
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                          STRICT_PREAMBLEf, 0);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                PROCESS_VARIABLE_PREAMBLEf, 0);
        } else {
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                            STRICT_PREAMBLEf, strict_preamble ? 1 : 0);
            soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                        PROCESS_VARIABLE_PREAMBLEf, strict_preamble ? 0 : 1);
        }

        SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, rval64));
        SOC_IF_ERROR_RETURN(mac_x_enable_get(unit, port, &enable));
        
        if (enable) {
            /* Turn off TX/RX enable */
            SOC_IF_ERROR_RETURN(mac_x_enable_set(unit, port, 0));
        }
        
        /* Update the speed */
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_MODEr, port,
                                    SPEED_MODEf, speed_select));

        if (IS_MXQ_PORT(unit,port)) {
            phy_mode = (speed >= 10000) ? 0 : 2;
            SOC_IF_ERROR_RETURN(READ_XPORT_MODE_REGr(unit, port, &rval));
            rval_orig = rval;
#ifdef BCM_KATANA2_SUPPORT
            if (SOC_IS_KATANA2(unit)) {
                SOC_IF_ERROR_RETURN(soc_katana2_get_phy_port_mode(unit,
                                    port, speed,
                                    &phy_mode));
                SOC_IF_ERROR_RETURN(soc_katana2_get_core_port_mode(unit,
                                    port, &core_mode));
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  CORE_PORT_MODEf, core_mode);
                SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                                    unit,port,&mxqblock));
                if ((mxqblock == 8) || (mxqblock == 9)) {
                     same_mxqblock_max_port_speed = 0;
                     if (core_mode == (bcmMxqCorePortMode_t )
                                       bcmMxqCorePortModeDual) {
                         for (loop = 0; loop < 4; loop ++) {
                              if ((*kt2_mxqblock_ports[unit])[mxqblock][loop] 
                                  != port) {
                                  if (mac_x_speed_get(unit, 
                                      (*kt2_mxqblock_ports[unit])[mxqblock]
                                      [loop], &other_port_speed)==SOC_E_NONE) {
                                      if (other_port_speed > 
                                          same_mxqblock_max_port_speed) {
                                          same_mxqblock_max_port_speed = 
                                                        other_port_speed;
                                      }
                                  }
                              }
                         }
                     } else {
                         same_mxqblock_max_port_speed = 0;
                     }
                     if ((speed == 10000) || (speed == 21000) || 
                         (same_mxqblock_max_port_speed == 10000) || 
                         (same_mxqblock_max_port_speed == 21000)) {
                          wc_10g_21g_sel = 1;
                     }
                }
                if ((mxqblock == 6) || (mxqblock == 7)) {
                     if (speed == 10000) {
                         SOC_IF_ERROR_RETURN(
                         soc_katana2_get_phy_connection_mode(unit,port,
                              mxqblock,&connection_mode)); 
                         /* XFI Mode */ 
                         if (connection_mode == bcmMqxConnectionWarpCore) {
                             wc_10g_21g_sel = 1;
                         }
                     }
                }
                soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                  WC_10G_21G_SELf, wc_10g_21g_sel);
            }
#endif
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                PHY_PORT_MODEf, phy_mode);
            soc_reg_field_set(unit, XPORT_MODE_REGr, &rval,
                                PORT_GMII_MII_ENABLEf, 
                                ((speed >= 10000) || 
                                 (same_mxqblock_max_port_speed >= 10000))
                                ? 0 : 1);
            if (rval != rval_orig) {
                SOC_IF_ERROR_RETURN(WRITE_XPORT_MODE_REGr(unit, port, rval));
#ifdef BCM_TRIUMPH3_SUPPORT
                /* On TR3/KT2 hard reset XMAC following mode change */
                if (soc_feature(unit, soc_feature_xmac_reset_on_mode_change)) {
                    SOC_IF_ERROR_RETURN(_mac_x_reinit(unit, port));
                }
#endif
            }
        }

        if (enable) {
            /* Re-enable transmitter and receiver */
            SOC_IF_ERROR_RETURN(mac_x_enable_set(unit, port, 1));
        }

        /* Set Timestamp Mac Delays */
        if (!IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(_mac_x_timestamp_delay_set(
                                    unit, port, speed, phy_mode));
        }
    }
#endif /* BCM_KATANA_SUPPORT */

#ifdef BCM_UNIPORT_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        
        soc_info_t *si;
        int phy_port;
        int bindex = -1;
        uint32 rval;
        uint32 core_mode, core_mode_org = -1, phy_mode, phy_mode_org = -1;
        uint32 xpc_enable = 0, xpc_enable_org = -1;
        uint32 strict_preamble = 1;
        uint64 rval64;

        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];
        if (phy_port == -1) {
            return SOC_E_INTERNAL;
        }
        bindex = SOC_PORT_BINDEX(unit, phy_port) / 4;
        SOC_IF_ERROR_RETURN(READ_PORT_MODE_REGr(unit, port, &rval));

        xpc_enable_org = soc_reg_field_get(unit, PORT_MODE_REGr, rval, 
                                           xpc_gmii_enable_field[bindex]);

        if ((speed < 10000) && (IS_XT_PORT(unit, port) ||
            (port == si->management_port))) {
            if (!SOC_IS_HELIX4(unit)) {
                xpc_enable = 1;
            }
        }

        if (xpc_enable != xpc_enable_org) {
            soc_reg_field_set(unit, PORT_MODE_REGr, &rval, 
                              xpc_gmii_enable_field[bindex],
                              xpc_enable);
            SOC_IF_ERROR_RETURN(WRITE_PORT_MODE_REGr(unit, port, rval));
        }
        core_mode_org = soc_reg_field_get(unit, PORT_MODE_REGr, rval, 
                                         core_mode_field[bindex]);
        phy_mode_org = soc_reg_field_get(unit, PORT_MODE_REGr, rval, 
                                         phy_mode_field[bindex]);

        if (IS_XT_PORT(unit, port) && (speed >= 10000)) {
            core_mode = 0; /* XAUI */
            phy_mode = 1;
        } else if (!IS_XT_PORT(unit, port)) {
            core_mode = 0;
            phy_mode = 0;
            if ((speed >= 10000) && (speed <= 11000)) {
                phy_mode = 2; /* XAUI */
                if (si->port_num_lanes[port] == 1) { /* XFI */
                    core_mode = 2;
                } else if (si->port_num_lanes[port] == 2) { /* RXAUI */
                    core_mode = 1;
                }
            } else if ((speed > 11000) && (si->port_num_lanes[port] == 2)) { /* HG-Duo */
                core_mode = 1;
                phy_mode = 1;
            } else if (speed <= 2500) {
                core_mode = 2;
                phy_mode = 2;
            } else if (speed <= 20000) {
                phy_mode = 1; /* HG 12/13/16/20 */
            }
        } else { /* GE */
            core_mode = 2;
            phy_mode = 2;
        }
        if ((core_mode_org != core_mode) || (phy_mode_org != phy_mode)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "cmo: %d -> cm: %d, pmo: %d -> pm: %d\n"), 
                         core_mode_org, core_mode, phy_mode_org, phy_mode));

            soc_reg_field_set(unit, PORT_MODE_REGr, &rval, 
                              core_mode_field[bindex], core_mode);
            soc_reg_field_set(unit, PORT_MODE_REGr, &rval, 
                              phy_mode_field[bindex], phy_mode);
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Port: %d set port mode: %d, phy mode: %d\n"), 
                         port, core_mode, phy_mode));
            SOC_IF_ERROR_RETURN(WRITE_PORT_MODE_REGr(unit, port, rval));

#ifdef BCM_TRIUMPH3_SUPPORT
            if (SOC_IS_TRIUMPH3(unit)) {
                SOC_IF_ERROR_RETURN(_mac_x_reinit(unit, port));
            }
#endif
        }
        if (soc_reg_field_valid(unit, XMAC_MODEr, SPEED_MODEf)) {
            int speed_select, enable;
            switch (speed) {
            case 10:
                strict_preamble = 0;
                speed_select = SOC_XMAC_SPEED_10;
                break;
            /* support non-standard speed in Broadreach mode */
            case 20:
            case 25:
            case 33:
            case 50:
                /* fall through to case 100 */
            case 100:
                strict_preamble = 0;
                speed_select = SOC_XMAC_SPEED_100;
                break;
            case 1000:
                speed_select = SOC_XMAC_SPEED_1000;
                strict_preamble = 0; 
                break;
            case 2500:
            case 3000:
                strict_preamble = 0;
                speed_select = SOC_XMAC_SPEED_2500;
                break;
            case 10000:
            case 11000:
            case 12000:
            case 13000:
            case 15000:
            case 16000:
            case 20000:
            case 21000:
            case 23000:
            case 25000:
            case 30000:
            case 32000:
            case 40000:
            case 42000:
                speed_select = SOC_XMAC_SPEED_10000;
                break;
            case 0:
                return (SOC_E_NONE);              /* Support NULL PHY */            
            default:
                return (SOC_E_CONFIG);
            }
            /* Update preamble processing */
            SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &rval64));
            if (IS_HG_PORT(unit, port)) {
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                      STRICT_PREAMBLEf, 0);
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                      PROCESS_VARIABLE_PREAMBLEf, 0);
            } else {
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                      STRICT_PREAMBLEf, strict_preamble ?
                                      1 : 0);
                soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval64,
                                      PROCESS_VARIABLE_PREAMBLEf, 
                                      strict_preamble ? 0 : 1);
            }
            SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, rval64));

            SOC_IF_ERROR_RETURN(mac_x_enable_get(unit, port, &enable));
            
            if (enable) {
                /* Turn off TX/RX enable */
                SOC_IF_ERROR_RETURN(mac_x_enable_set(unit, port, 0));
            }
            
            /* Update the speed */
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, XMAC_MODEr, port,
                                        SPEED_MODEf, speed_select));



            if (enable) {
                /* Re-enable transmitter and receiver */
                SOC_IF_ERROR_RETURN(mac_x_enable_set(unit, port, 1));
            }

        }

        /* Set Timestamp Mac Delays */
        if (!IS_GE_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(_mac_x_timestamp_delay_set(
                                    unit, port, speed, phy_mode));
        }
    }
#endif /* BCM_UNIPORT_SUPPORT */
    /* Enable XLGMII_ALIGN_ENB when using Multi-Lane Distribution (MLD) */
    xlgmii_align = 0;

#if defined(BCM_TRIDENT_SUPPORT)
    ipg_check_disable = 0;
#endif

    SOC_IF_ERROR_RETURN(soc_phyctrl_interface_get(unit,port,&intf));
    if (IS_XE_PORT(unit, port)) {
        /* xe port, 40G mld-4 and 20G mld-2 */
        if ((speed == 40000) || (speed == 42000) || 
            ((speed == 20000) && (intf==SOC_PORT_IF_KR))) {
            xlgmii_align = 1;
        }
    } else { /* HG port with external PHY may also use IEEE 40G mld-4 */
        if (((speed == 40000) || (speed == 42000))
            && PHY_EXTERNAL_MODE(unit, port)
            && (!PHY_PASSTHRU_MODE(unit, port))
            && (!soc_property_port_get(unit, port, 
                                       spn_PORT_PHY_MODE_REVERSE, 0))) {
            xlgmii_align = 1;
        }
#if defined(BCM_TRIDENT_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT(unit)) {
            if (intf == SOC_PORT_IF_KR4) {
                xlgmii_align = 1;
            }
            else {
                ipg_check_disable = 1;
            }
        }
#endif
#if defined(BCM_HELIX4_SUPPORT)
        if (SOC_IS_HELIX4(unit)) {
            if ((speed==21000) && (intf == SOC_PORT_IF_KR)) {
                xlgmii_align = 1;
            }
        }
#endif
    }
    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &rval64));
    soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64, XLGMII_ALIGN_ENBf,
                          xlgmii_align);
#if defined(BCM_TRIDENT_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT(unit)) {
        if (soc_reg_field_valid(unit, XMAC_CTRLr, XGMII_IPG_CHECK_DISABLEf)) {
            soc_reg64_field32_set(unit, XMAC_CTRLr, &rval64,XGMII_IPG_CHECK_DISABLEf, ipg_check_disable);
        }
    }
#endif
    SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, rval64));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_speed_get
 * Purpose:
 *      Get XMAC speed
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      speed - (OUT) speed in Mb
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval;
#ifdef BCM_KATANA_SUPPORT
    uint32 index=0;
    uint32 rval32=0;
    soc_field_t speed_fields[]={XGXS0_SPEED16000f,XGXS0_SPEED15000f,
                                XGXS0_SPEED13000f,XGXS0_SPEED12500f,
                                XGXS0_SPEED12000f,XGXS0_SPEED10000_CX4f,
                                XGXS0_SPEED10000f,XGXS0_SPEED1000f};
    int         speed_values[]={16000,15000,
                               13000,12500,
                               12000,10000,
                               10000,1000};
#endif /* BCM_KATANA_SUPPORT */

    *speed = 10000;

    /* We should be looking for Ver B0 in  XMAC_VERSION_ID reg. But this reg
     is not implemented in Katana A0. So, check for SPEED_MODE field instead */
    if (soc_reg_field_valid(unit, XMAC_MODEr, SPEED_MODEf)) {

        SOC_IF_ERROR_RETURN(READ_XMAC_MODEr(unit, port, &rval));
        switch (soc_reg64_field32_get(unit, XMAC_MODEr, rval, SPEED_MODEf)) {
        case SOC_XMAC_SPEED_10:
            *speed = 10;
            break;
        case SOC_XMAC_SPEED_100:
            *speed = 100;
            break;
        case SOC_XMAC_SPEED_1000:
            *speed = 1000;
            break;
        case SOC_XMAC_SPEED_2500:
            *speed = 2500;
            break;
        case SOC_XMAC_SPEED_10000:
        default:
#ifdef BCM_KATANA_SUPPORT
            if (SOC_IS_KATANA(unit)) {
                if (IS_HG_PORT(unit, port)) {
                    SOC_IF_ERROR_RETURN
                    (READ_XPORT_XGXS_NEWSTATUS1_REGr(unit, port, &rval32));
                    for(index=0;
                        index <sizeof(speed_values)/sizeof(speed_values[0]);
                        index++) {
                        if (soc_reg_field_get(
                            unit, XPORT_XGXS_NEWSTATUS1_REGr,
                            rval32, speed_fields[index])) {
                            *speed = speed_values[index];
                            break;
                        }
                    }
                    if (index != sizeof(speed_values)/sizeof(speed_values[0])) {
                        break;
                    }
                }
            }
            /* No other option left*/
#endif /* BCM_KATANA_SUPPORT */
            *speed = 10000;
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_loopback_set
 * Purpose:
 *      Set a XMAC into/out-of loopback mode
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS unit # on unit.
 *      loopback - Boolean: true -> loopback mode, false -> normal operation
 * Note:
 *      On Xmac, when setting loopback, we enable the TX/RX function also.
 *      Note that to test the PHY, we use the remote loopback facility.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_loopback_set(int unit, soc_port_t port, int lb)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_loopback_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "local" : "no"));

    /* need to enable clock compensation for applicable serdes device */
    (void)soc_phyctrl_notify(unit, port, phyEventMacLoopback, lb? 1: 0);

    if (lb) {
        SOC_IF_ERROR_RETURN(soc_mac_x_sync_fifo_reset(unit, port));
    }

    return soc_reg_field32_modify(unit, XMAC_CTRLr, port,
                                  soc_property_port_get(unit, port,
                                                        "mac_line_lb", 1) ?
                                  LINE_LOCAL_LPBKf : CORE_LOCAL_LPBKf,
                                  lb ? 1 : 0);
}

/*
 * Function:
 *      mac_x_loopback_get
 * Purpose:
 *      Get current XMAC loopback mode setting.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      loopback - (OUT) Boolean: true = loopback, false = normal
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_loopback_get(int unit, soc_port_t port, int *lb)
{
    uint64 ctrl;
    int local, remote;

    SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &ctrl));

    remote = soc_reg64_field32_get(unit, XMAC_CTRLr, ctrl, CORE_REMOTE_LPBKf) |
        soc_reg64_field32_get(unit, XMAC_CTRLr, ctrl, LINE_REMOTE_LPBKf);
    local = soc_reg64_field32_get(unit, XMAC_CTRLr, ctrl, CORE_LOCAL_LPBKf) |
        soc_reg64_field32_get(unit, XMAC_CTRLr, ctrl, LINE_LOCAL_LPBKf);
    *lb = local | remote;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_loopback_get: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *lb ? (remote ? "remote" : "local") : "no"));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_pause_addr_set
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
mac_x_pause_addr_set(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 r, tmp;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_pause_addr_set: unit %d port %s MAC=<"
                             "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 m[0], m[1], m[2], m[3], m[4], m[5]));

    COMPILER_64_ZERO(r);
    for (i = 0; i< 6; i++) {
        COMPILER_64_SET(tmp, 0, m[i]);
        COMPILER_64_SHL(r, 8);
        COMPILER_64_OR(r, tmp);
    }

    SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_MAC_SAr(unit, port, r));
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAC_SAr(unit, port, r));

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_pause_addr_get
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
mac_x_pause_addr_get(int unit, soc_port_t port, sal_mac_addr_t m)
{
    uint64 reg;
    uint32 msw;
    uint32 lsw;

    SOC_IF_ERROR_RETURN(READ_XMAC_RX_MAC_SAr(unit, port, &reg));
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
                            "mac_x_pause_addr_get: unit %d port %s MAC=<"
                             "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 m[0], m[1], m[2], m[3], m[4], m[5]));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_interface_set
 * Purpose:
 *      Set a XMAC interface type
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
mac_x_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_interface_set: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_x_port_if_names[pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_interface_get
 * Purpose:
 *      Retrieve XMAC interface type
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      pif - (OUT) one of SOC_PORT_IF_*
 * Returns:
 *      SOC_E_NONE
 */
STATIC int
mac_x_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);

    *pif = SOC_PORT_IF_MII;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_interface_get: unit %d port %s interface=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_x_port_if_names[*pif]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_frame_max_set
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
mac_x_frame_max_set(int unit, soc_port_t port, int size)
{
    uint64 rval;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_frame_max_set: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 size));

    if (IS_GE_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        size += 4;
    }
    COMPILER_64_ZERO(rval);
    soc_reg64_field32_set(unit, XMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf, size);
    SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_MAX_SIZEr(unit, port, rval));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_frame_max_get
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
mac_x_frame_max_get(int unit, soc_port_t port, int *size)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN(READ_XMAC_RX_MAX_SIZEr(unit, port, &rval));
    *size = soc_reg64_field32_get(unit, XMAC_RX_MAX_SIZEr, rval, RX_MAX_SIZEf);
    if (IS_GE_PORT(unit, port) || IS_XE_PORT(unit, port)) {
        /* For VLAN tagged packets */
        *size -= 4;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_frame_max_get: unit %d port %s size=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *size));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_ifg_set
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
mac_x_ifg_set(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int ifg)
{
    int         real_ifg, encap;
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    uint64      rval, orval;
    soc_port_ability_t ability;
    uint32      pa_flag;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_ifg_set: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", ifg));

    pa_flag = SOC_PA_SPEED(speed); 
    soc_mac_x.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 31 bytes (i.e. multiple of 8 bits) */
    real_ifg = ifg < 64 ? 64 : (ifg > 248 ? 248 : (ifg + 7) & (0x1f << 3));

    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit,port)) {
        si->fd_xe = real_ifg;
    } else {
        SOC_IF_ERROR_RETURN
            (soc_mac_x.md_encap_get(unit, port, &encap));
        if(encap == SOC_ENCAP_HIGIG2) {
            si->fd_hg2 = real_ifg;
        } else {
            si->fd_hg = real_ifg;
        }
    }


    SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &rval));
    orval = rval;
    soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval, AVERAGE_IPGf,
                          IPG_BYTE(real_ifg));
    if (COMPILER_64_NE(rval, orval)) {
        SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, port, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_ifg_get
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
mac_x_ifg_get(int unit, soc_port_t port, int speed,
                soc_port_duplex_t duplex, int *ifg)
{
    soc_ipg_t  *si = &SOC_PERSIST(unit)->ipg[port];
    soc_port_ability_t ability;
    uint32      pa_flag;
    int encap;

    if (!duplex) {
        return SOC_E_PARAM;
    }

    pa_flag = SOC_PA_SPEED(speed); 
    soc_mac_x.md_ability_local_get(unit, port, &ability);
    if (!(pa_flag & ability.speed_full_duplex)) {
        return SOC_E_PARAM;
    }

    if (IS_XE_PORT(unit, port) || IS_GE_PORT(unit, port)) {
        *ifg = si->fd_xe;
    } else {
        SOC_IF_ERROR_RETURN
            (soc_mac_x.md_encap_get(unit, port, &encap));
        if(encap == SOC_ENCAP_HIGIG2) {
            *ifg = si->fd_hg2;
        } else {
            *ifg = si->fd_hg;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_ifg_get: unit %d port %s speed=%dMb duplex=%s "
                             "ifg=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 speed, duplex ? "True" : "False", *ifg));
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_loopback_remote_set
 * Purpose:
 *      Set the XMAC into remote-loopback state.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) loopback enable state
 * Returns:
 *      SOC_E_XXX
 */
int
mac_x_loopback_remote_set(int unit, soc_port_t port, int lb)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_loopback_remote_set: unit %d port %s loopback=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 lb ? "remote" : "no"));

    return soc_reg_field32_modify(unit, XMAC_CTRLr, port, CORE_REMOTE_LPBKf,
                                  lb ? 1 : 0);
}

/*
 * Function:
 *      _mac_x_port_mode_update
 * Purpose:
 *      Set the XMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      to_hg_port - (TRUE/FALSE)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_mac_x_port_mode_update(int unit, soc_port_t port, int to_hg_port)
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

#if defined(BCM_TRIDENT_SUPPORT) || defined (BCM_TRIUMPH3_SUPPORT) || \
    defined(BCM_SHADOW_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TD_TT(unit) || SOC_IS_TRIUMPH3(unit) ||
                       SOC_IS_SHADOW(unit) || SOC_IS_KATANA2(unit)) {
       /* no need to hard reset the Warpcore serdes device. Soft init
        * should be good enough.  Hard reset brings
        * down all other ports using the same Warpcore in quad-port mode. 
        */
    } else 
#endif /* BCM_TRIDENT_SUPPORT || BCM_TRIUMPH3_SUPPORT || BCM_SHADOW_SUPPORT */
    {
        rv = soc_xgxs_reset(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = soc_phyctrl_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_x_init(unit, port);
    }

    if (SOC_SUCCESS(rv)) {
        rv = mac_x_enable_set(unit, port, 0);
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
        SOC_IF_ERROR_RETURN(READ_XLPORT_CONFIGr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, HIGIG_MODEf,
                          to_hg_port ? 1 : 0);
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, XPAUSE_RX_ENf,
                          to_hg_port ? 1 : 0);
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, XPAUSE_TX_ENf,
                          to_hg_port ? 1 : 0);
        soc_reg_field_set(unit, XLPORT_CONFIGr, &rval, XPAUSE_ENf,
                          to_hg_port ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_CONFIGr(unit, port, rval));
    }
    if (SOC_REG_IS_VALID(unit, XPORT_CONFIGr)) {
        SOC_IF_ERROR_RETURN(READ_XPORT_CONFIGr(unit, port, &rval));
        if (to_hg_port) {
            soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 1);
        } else {
            soc_reg_field_set(unit, XPORT_CONFIGr, &rval, HIGIG_MODEf, 0);
        }
        SOC_IF_ERROR_RETURN(WRITE_XPORT_CONFIGr(unit, port, rval));
    }
    return rv;
}

/*
 * Function:
 *      mac_x_encap_set
 * Purpose:
 *      Set the XMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (IN) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_encap_set(int unit, soc_port_t port, int mode)
{
    int enable, encap, rv;
    int runt, ipg;

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_encap_set: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_x_encap_mode[mode]));
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

    SOC_IF_ERROR_RETURN(mac_x_enable_get(unit, port, &enable));

    if (enable) {
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(mac_x_enable_set(unit, port, 0));
    }

    if (IS_E_PORT(unit, port) && mode != SOC_ENCAP_IEEE) {
        /* XE -> HG */
        SOC_IF_ERROR_RETURN(_mac_x_port_mode_update(unit, port, TRUE));
    } else if (IS_ST_PORT(unit, port) && mode == SOC_ENCAP_IEEE) {
        /* HG -> XE */
        SOC_IF_ERROR_RETURN(_mac_x_port_mode_update(unit, port, FALSE));
    }

    /* Update the encapsulation mode */
    rv = soc_reg_field32_modify(unit, XMAC_MODEr, port, HDR_MODEf, encap);
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
        if (mode == SOC_ENCAP_HIGIG2_LITE) {
            rv = soc_reg_field32_modify(unit, XMAC_MODEr, port, SPEED_MODEf, 3);
        }
    }
#endif

    /* Update the RUNT */
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit) ||
                                   SOC_IS_KATANAX(unit)) {
        runt = ((mode == SOC_ENCAP_HIGIG2) || (mode == SOC_ENCAP_HIGIG2_LITE)) ?
                                                    XMAC_RUNT_THRESHOLD_HG2 : 
                ((mode == SOC_ENCAP_HIGIG) ? 
                             XMAC_RUNT_THRESHOLD_HG1 : XMAC_RUNT_THRESHOLD_XE);
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_CTRLr, port, RUNT_THRESHOLDf,runt));
    }

    ipg = (mode == SOC_ENCAP_HIGIG2) ? SOC_PERSIST(unit)->ipg[port].fd_hg2 :
        ((mode == SOC_ENCAP_HIGIG) ? SOC_PERSIST(unit)->ipg[port].fd_hg :
         SOC_PERSIST(unit)->ipg[port].fd_xe);
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, XMAC_TX_CTRLr, port, AVERAGE_IPGf,
                                IPG_BYTE(ipg)));

    if (enable) {
        /* Re-enable transmitter and receiver */
        SOC_IF_ERROR_RETURN(mac_x_enable_set(unit, port, 1));
    }

    return rv;
}

/*
 * Function:
 *      mac_x_encap_get
 * Purpose:
 *      Get the XMAC port encapsulation mode.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (INT) encap bits (defined above)
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_encap_get(int unit, soc_port_t port, int *mode)
{
    uint64 rval;

    if (!mode) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_XMAC_MODEr(unit, port, &rval));
    switch (soc_reg64_field32_get(unit, XMAC_MODEr, rval, HDR_MODEf)) {
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
                            "mac_x_encap_get: unit %d port %s encapsulation=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac_x_encap_mode[*mode]));
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_expected_rx_latency_get
 * Purpose:
 *      Get the XMAC port expected Rx latency
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      latency - (OUT) Latency in NS
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_expected_rx_latency_get(int unit, soc_port_t port, int *latency)
{
    int speed = 0;
    SOC_IF_ERROR_RETURN(mac_x_speed_get(unit, port, &speed));

    
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
 *      mac_x_control_set
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
mac_x_control_set(int unit, soc_port_t port, soc_mac_control_t type,
                  int value)
{
    uint64 rval, copy;
    uint32 fval;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_control_set: unit %d port %s type=%d value=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, value));

    switch (type) {
    case SOC_MAC_CONTROL_RX_SET:
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, XMAC_CTRLr, &rval, RX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_FAILOVER_RX_SET:
        
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &rval));
        copy = rval;
        soc_reg64_field32_set(unit, XMAC_CTRLr, &rval, TX_ENf, value ? 1 : 0);
        if (COMPILER_64_NE(rval, copy)) {
            SOC_IF_ERROR_RETURN(WRITE_XMAC_CTRLr(unit, port, rval));
        }    
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        if (value < 0 || value > 255) {
            return SOC_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &rval));
            if (value >= 8) {
                soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval, THROT_DENOMf,
                                      value);
                soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      1);
            } else {
                soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval, THROT_DENOMf,
                                      0);
                soc_reg64_field32_set(unit, XMAC_TX_CTRLr, &rval, THROT_NUMf,
                                      0);
            }
            SOC_IF_ERROR_RETURN(WRITE_XMAC_TX_CTRLr(unit, port, rval));
        }
        return SOC_E_NONE;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &rval));
        soc_reg64_field32_set(unit, XMAC_RX_CTRLr, &rval, RX_PASS_CTRLf,
                              value ? 1 : 0);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_RX_CTRLr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XMAC_PFC_TYPEr, port,
                                                   PFC_ETH_TYPEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XMAC_PFC_OPCODEr,
                                                   port, PFC_OPCODEf, value));
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        if (value != 8) {
            return SOC_E_PARAM;
        }
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, XMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0x00ffffff;
        fval |= (value & 0xff) << 24;
        soc_reg64_field32_set(unit, XMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

        soc_reg64_field32_set(unit, XMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                              value >> 8);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_DAr(unit, port, &rval));
        fval = soc_reg64_field32_get(unit, XMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval &= 0xff000000;
        fval |= value;
        soc_reg64_field32_set(unit, XMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);
        SOC_IF_ERROR_RETURN(WRITE_XMAC_PFC_DAr(unit, port, rval));
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, RX_PASS_PFCf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, RX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, TX_PFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, FORCE_PFC_XONf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, PFC_STATS_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, 
                                    PFC_REFRESH_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_PFC_CTRLr, port, 
                                    PFC_XOFF_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_LLFC_CTRLr, port, RX_LLFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_LLFC_CTRLr, port, TX_LLFC_ENf,
                                    value ? 1 : 0));
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XMAC_EEE_CTRLr, 
                                                   port, EEE_ENf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XMAC_EEE_TIMERSr, 
                                    port, EEE_DELAY_ENTRY_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, XMAC_EEE_TIMERSr, 
                                    port, EEE_WAKE_TIMERf, value));
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_LSS_CTRLr, port,
                                    LOCAL_FAULT_DISABLEf, value ? 0 : 1));
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_LSS_CTRLr, port,
                                    REMOTE_FAULT_DISABLEf, value ? 0 : 1));
        break;
    case SOC_MAC_CONTROL_EGRESS_DRAIN:
        SOC_IF_ERROR_RETURN(mac_x_egress_queue_drain(unit, port));
        break;

    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_VLAN_TAGr, port, 
                                    OUTER_VLAN_TAGf, value));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_VLAN_TAGr, port, 
                                    OUTER_VLAN_TAG_ENABLEf, value ? 1 : 0));
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_VLAN_TAGr, port, 
                                    INNER_VLAN_TAGf, value));
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, XMAC_RX_VLAN_TAGr, port, 
                                    INNER_VLAN_TAG_ENABLEf, value ? 1 : 0));
        break;

    default:
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      mac_x_control_get
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
mac_x_control_get(int unit, soc_port_t port, soc_mac_control_t type,
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
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_CTRLr, rval, RX_ENf);
        break;
    case SOC_MAC_CONTROL_TX_SET:
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_CTRLr, rval, TX_ENf);
        break;
    case SOC_MAC_CONTROL_SW_RESET:
        SOC_IF_ERROR_RETURN(READ_XMAC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_CTRLr, rval, SOFT_RESETf);
        break;
    case SOC_MAC_CONTROL_FRAME_SPACING_STRETCH:
        SOC_IF_ERROR_RETURN(READ_XMAC_TX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_TX_CTRLr, rval,
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
                (READ_XMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
            if (soc_reg64_field32_get(unit, XMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                      ENTRY_COUNTf) == 0) {
                return SOC_E_EMPTY;
            }
            SOC_IF_ERROR_RETURN
                (READ_XMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
            *value = soc_reg64_field32_get(unit, XMAC_TX_TIMESTAMP_FIFO_DATAr,
                                       rval, TIME_STAMPf);
        }
        break;

    case SOC_MAC_PASS_CONTROL_FRAME:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_CTRLr, rval,
                                       RX_PASS_CTRLf);
        break;

    case SOC_MAC_CONTROL_PFC_TYPE:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_TYPEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_TYPEr, rval,
                                       PFC_ETH_TYPEf);
        break;

    case SOC_MAC_CONTROL_PFC_OPCODE:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_OPCODEr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_OPCODEr, rval,
                                       PFC_OPCODEf);
        break;

    case SOC_MAC_CONTROL_PFC_CLASSES:
        *value = 8;
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_OUI:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_DAr(unit, port, &rval));
        fval0 = soc_reg64_field32_get(unit, XMAC_PFC_DAr, rval, PFC_MACDA_LOf);
        fval1 = soc_reg64_field32_get(unit, XMAC_PFC_DAr, rval, PFC_MACDA_HIf);
        *value = (fval0 >> 24) | (fval1 << 8);
        break;

    case SOC_MAC_CONTROL_PFC_MAC_DA_NONOUI:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_DAr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_DAr, rval,
                                       PFC_MACDA_LOf) & 0x00ffffff;
        break;

    case SOC_MAC_CONTROL_PFC_RX_PASS:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       RX_PASS_PFCf);
        break;

    case SOC_MAC_CONTROL_PFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       RX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       TX_PFC_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_FORCE_XON:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       FORCE_PFC_XONf);
        break;

    case SOC_MAC_CONTROL_PFC_STATS_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       PFC_STATS_ENf);
        break;

    case SOC_MAC_CONTROL_PFC_REFRESH_TIME:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       PFC_REFRESH_TIMERf);
        break;

    case SOC_MAC_CONTROL_PFC_XOFF_TIME:
        SOC_IF_ERROR_RETURN(READ_XMAC_PFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_PFC_CTRLr, rval,
                                       PFC_XOFF_TIMERf);
        break;

    case SOC_MAC_CONTROL_LLFC_RX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_LLFC_CTRLr, rval,
                                       RX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_LLFC_TX_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_LLFC_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_LLFC_CTRLr, rval,
                                       TX_LLFC_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_ENABLE:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, XMAC_EEE_CTRLr, EEE_ENf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_XMAC_EEE_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_EEE_CTRLr, rval, EEE_ENf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_IDLE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, XMAC_EEE_TIMERSr, EEE_DELAY_ENTRY_TIMERf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_XMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_EEE_TIMERSr, rval,
                EEE_DELAY_ENTRY_TIMERf);
        break;

    case SOC_MAC_CONTROL_EEE_TX_WAKE_TIME:
        if (!soc_feature(unit, soc_feature_eee)) {
            return SOC_E_UNAVAIL;
        }
        if (!soc_reg_field_valid(unit, XMAC_EEE_TIMERSr, EEE_WAKE_TIMERf)) {
            return SOC_E_UNAVAIL;
        }
        SOC_IF_ERROR_RETURN(READ_XMAC_EEE_TIMERSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_EEE_TIMERSr, rval,
                EEE_WAKE_TIMERf);
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_LSS_CTRLr, rval,
                                       LOCAL_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_LOCAL_STATUS:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_LSS_STATUSr, rval,
                                       LOCAL_FAULT_STATUSf);
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_LSS_CTRLr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_LSS_CTRLr, rval,
                                       REMOTE_FAULT_DISABLEf) ? 0 : 1;
        break;

    case SOC_MAC_CONTROL_FAULT_REMOTE_STATUS:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_LSS_STATUSr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_LSS_STATUSr, rval,
                                       REMOTE_FAULT_STATUSf);
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_OUTER_TPID:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_VLAN_TAGr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_VLAN_TAGr, 
                                       rval, OUTER_VLAN_TAGf);
        break;
    case SOC_MAC_CONTROL_RX_VLAN_TAG_INNER_TPID:
        SOC_IF_ERROR_RETURN(READ_XMAC_RX_VLAN_TAGr(unit, port, &rval));
        *value = soc_reg64_field32_get(unit, XMAC_RX_VLAN_TAGr, 
                                       rval, INNER_VLAN_TAGf);
        break;
    case SOC_MAC_CONTROL_EXPECTED_RX_LATENCY:
        SOC_IF_ERROR_RETURN(mac_x_expected_rx_latency_get(unit, port, value));
        break;
    default:
        return SOC_E_UNAVAIL;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_control_get: unit %d port %s type=%d value=%d "
                             "rv=%d\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 type, *value, rv));
    return rv;
}

/*
 * Function:
 *      mac_x_ability_local_get
 * Purpose:
 *      Return the abilities of XMAC
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (OUT) Supported operating modes as a mask of abilities.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
mac_x_ability_local_get(int unit, soc_port_t port,
                          soc_port_ability_t *ability)
{
#ifdef BCM_KATANA2_SUPPORT
    uint8  mxqblock = 0;
    if (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) {
        SOC_IF_ERROR_RETURN(soc_katana2_get_port_mxqblock(
                            unit,port,&mxqblock));
    }
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
#ifdef BCM_KATANA2_SUPPORT
    if ((SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit)) && ((mxqblock == 6) || (mxqblock == 7))) {
        ability->encap |= SOC_PA_ENCAP_HIGIG2_LITE; 
    } 
#endif

    if (IS_HL_PORT(unit , port)) {
        if (SOC_INFO(unit).port_speed_max[port]) {
            ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
        }
    } 
    if (IS_HG_PORT(unit , port)) {
        switch (SOC_INFO(unit).port_speed_max[port]) {
        case 42000:
            ability->speed_full_duplex |= SOC_PA_SPEED_42GB;
            /* fall through */
        case 40000:
            ability->speed_full_duplex |= SOC_PA_SPEED_40GB;
            if(soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_42GB;                 
            }
            /* fall through */
        case 30000:
            ability->speed_full_duplex |= SOC_PA_SPEED_30GB;
            /* fall through */
        case 25000:
            ability->speed_full_duplex |= SOC_PA_SPEED_25GB;
            /* fall through */
        case 23000:
            ability->speed_full_duplex |= SOC_PA_SPEED_23GB;
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
            if (soc_feature(unit, soc_feature_higig_misc_speed_support)) {
                ability->speed_full_duplex |= SOC_PA_SPEED_11GB;
            }
            break;
        default:
            break;
        }
    } else {
        if (SOC_INFO(unit).port_speed_max[port] >= 40000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_40GB |
                SOC_PA_SPEED_42GB;
        }
        
         if (SOC_INFO(unit).port_speed_max[port] >= 20000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_20GB;
        }
        if (SOC_INFO(unit).port_speed_max[port] >= 10000) {
            ability->speed_full_duplex |= SOC_PA_SPEED_10GB;
        }
        if (soc_feature(unit, soc_feature_unified_port) || IS_MXQ_PORT(unit, port)) {
            if (SOC_INFO(unit).port_speed_max[port] >= 2500) {
                ability->speed_full_duplex |= SOC_PA_SPEED_2500MB;
            }
            if (SOC_INFO(unit).port_speed_max[port] >= 1000) {
                ability->speed_full_duplex |= SOC_PA_SPEED_1000MB;
            }
            if (SOC_INFO(unit).port_speed_max[port] >= 100) {
                ability->speed_full_duplex |= SOC_PA_SPEED_100MB;
            }
            if (SOC_INFO(unit).port_speed_max[port] >= 10) {
                ability->speed_full_duplex |= SOC_PA_SPEED_10MB;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_x_ability_local_get: unit %d port %s "
                             "speed_half=0x%x speed_full=0x%x encap=0x%x pause=0x%x "
                             "interface=0x%x medium=0x%x loopback=0x%x flags=0x%x\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 ability->speed_half_duplex, ability->speed_full_duplex,
                 ability->encap, ability->pause, ability->interface,
                 ability->medium, ability->loopback, ability->flags));
    return SOC_E_NONE;
}

static int
mac_x_timesync_tx_info_get (int unit, soc_port_t port,
                                        soc_port_timesync_tx_info_t *tx_info)
{
    uint64 rval;

    SOC_IF_ERROR_RETURN
        (READ_XMAC_TX_TIMESTAMP_FIFO_STATUSr(unit, port, &rval));
    if (soc_reg64_field32_get(unit, XMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                              ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    SOC_IF_ERROR_RETURN
        (READ_XMAC_TX_TIMESTAMP_FIFO_DATAr(unit, port, &rval));
    tx_info->timestamps_in_fifo = soc_reg64_field32_get(unit,
                     XMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);

    tx_info->timestamps_in_fifo_hi = 0;

    tx_info->sequence_id = soc_reg64_field32_get(unit,
                     XMAC_TX_TIMESTAMP_FIFO_DATAr, rval, SEQUENCE_IDf);

    return (SOC_E_NONE);
}


/* Exported XMAC driver structure */
mac_driver_t soc_mac_x = {
    "XMAC Driver",                /* drv_name */
    mac_x_init,                   /* md_init  */
    mac_x_enable_set,             /* md_enable_set */
    mac_x_enable_get,             /* md_enable_get */
    mac_x_duplex_set,             /* md_duplex_set */
    mac_x_duplex_get,             /* md_duplex_get */
    mac_x_speed_set,              /* md_speed_set */
    mac_x_speed_get,              /* md_speed_get */
    mac_x_pause_set,              /* md_pause_set */
    mac_x_pause_get,              /* md_pause_get */
    mac_x_pause_addr_set,         /* md_pause_addr_set */
    mac_x_pause_addr_get,         /* md_pause_addr_get */
    mac_x_loopback_set,           /* md_lb_set */
    mac_x_loopback_get,           /* md_lb_get */
    mac_x_interface_set,          /* md_interface_set */
    mac_x_interface_get,          /* md_interface_get */
    NULL,                         /* md_ability_get - Deprecated */
    mac_x_frame_max_set,          /* md_frame_max_set */
    mac_x_frame_max_get,          /* md_frame_max_get */
    mac_x_ifg_set,                /* md_ifg_set */
    mac_x_ifg_get,                /* md_ifg_get */
    mac_x_encap_set,              /* md_encap_set */
    mac_x_encap_get,              /* md_encap_get */
    mac_x_control_set,            /* md_control_set */
    mac_x_control_get,            /* md_control_get */
    mac_x_ability_local_get,      /* md_ability_local_get */
    mac_x_timesync_tx_info_get    /* md_timesync_tx_info_get */
 };

#endif /* BCM_XMAC_SUPPORT */
