/*! \file bcmpmac_gport.h
 *
 * GPORT driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_GPORT_H
#define BCMPMAC_GPORT_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>


/*******************************************************************************
* Local definitions
 */

/*! UNIMAC speed mode value per speed value. */
typedef enum unimac_speed_mode_e {

    /*! 10 Mbps */
    UNIMAC_SPEED_10 = 0,

    /*! 100 Mbps */
    UNIMAC_SPEED_100 = 1,

    /*! 1 Gbps */
    UNIMAC_SPEED_1000 = 2,

    /*! 2.5 Gbps */
    UNIMAC_SPEED_2500 = 3

} unimac_speed_mode_t;

/******************************************************************************
 * Private functions
 */

static inline int
gport_port_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t reset)
{
    return SHR_E_NONE;
}

static inline int
gport_port_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *reset)
{
    return SHR_E_NONE;
}

static inline int
gport_port_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t enable)
{
    int ioerr = 0;
    GPORT_CONFIGr_t gport_config;

    if (port != 0) {
        /* this is a block-based setting */
        port = 0;
    }

    GPORT_CONFIGr_CLR(gport_config);

    ioerr += READ_GPORT_CONFIGr(pa, port, &gport_config);
    GPORT_CONFIGr_GPORT_ENf_SET(gport_config, enable);
    ioerr += WRITE_GPORT_CONFIGr(pa, port, gport_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_port_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t *enable)
{
    int ioerr = 0;
    GPORT_CONFIGr_t gport_config;

    if (port != 0) {
        /* this is a block-based setting */
        port = 0;
    }

    GPORT_CONFIGr_CLR(gport_config);

    ioerr += READ_GPORT_CONFIGr(pa, port, &gport_config);
    *enable = GPORT_CONFIGr_GPORT_ENf_GET(gport_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t init)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    if (!init) {
        /* Disable the port. */
        ioerr += gport_port_enable_set(pa, port, 0);

        /* Reset the port. */
        ioerr += gport_port_reset_set(pa, port, 1);

        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    /* Bring the port out of reset. */
    ioerr += gport_port_reset_set(pa, port, 0);

    /* Enable the port. */
    ioerr += gport_port_enable_set(pa, port, 1);

    /* Init configuration */
    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_CNTL_FRM_ENAf_SET(unimac_config, 1);
    COMMAND_CONFIGr_CRC_FWDf_SET(unimac_config, 1);
    COMMAND_CONFIGr_ENA_EXT_CONFIGf_SET(unimac_config, 0);
    COMMAND_CONFIGr_EN_INTERNAL_TX_CRSf_SET(unimac_config, 0);
    COMMAND_CONFIGr_FCS_CORRUPT_URUN_ENf_SET(unimac_config, 0);
    COMMAND_CONFIGr_FD_TX_URUN_FIX_ENf_SET(unimac_config, 0);
    COMMAND_CONFIGr_HD_ENAf_SET(unimac_config, 0);
    COMMAND_CONFIGr_LINE_LOOPBACKf_SET(unimac_config, 0);
    COMMAND_CONFIGr_LOOP_ENAf_SET(unimac_config, 0);
    COMMAND_CONFIGr_MAC_LOOP_CONf_SET(unimac_config, 0);
    COMMAND_CONFIGr_NO_LGTH_CHECKf_SET(unimac_config, 1);
    COMMAND_CONFIGr_OOB_EFC_ENf_SET(unimac_config, 0);
    COMMAND_CONFIGr_OVERFLOW_ENf_SET(unimac_config, 0);
    COMMAND_CONFIGr_PAD_ENf_SET(unimac_config, 0);
    COMMAND_CONFIGr_PAUSE_FWDf_SET(unimac_config, 0);
    COMMAND_CONFIGr_PROMIS_ENf_SET(unimac_config, 1);
    COMMAND_CONFIGr_RUNT_FILTER_DISf_SET(unimac_config, 0);
    COMMAND_CONFIGr_RX_ERR_DISCf_SET(unimac_config, 0);
    COMMAND_CONFIGr_RX_LOW_LATENCY_ENf_SET(unimac_config, 0);
    COMMAND_CONFIGr_SW_OVERRIDE_RXf_SET(unimac_config, 0);
    COMMAND_CONFIGr_SW_OVERRIDE_TXf_SET(unimac_config, 0);
    COMMAND_CONFIGr_TX_ADDR_INSf_SET(unimac_config, 0);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_mac_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t reset)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_SW_RESETf_SET(unimac_config, reset);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_mac_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t *reset)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    *reset = COMMAND_CONFIGr_SW_RESETf_GET(unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_rx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t enable)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_RX_ENAf_SET(unimac_config, enable);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_rx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t *enable)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    *enable = COMMAND_CONFIGr_RX_ENAf_GET(unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_tx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t enable)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_TX_ENAf_SET(unimac_config, enable);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_tx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t *enable)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    *enable = COMMAND_CONFIGr_TX_ENAf_GET(unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_speed_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                uint32_t speed)
{
    int ioerr = 0;
    unimac_speed_mode_t mode;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    switch (speed) {
        case 10:
            mode = UNIMAC_SPEED_10;
            break;
        case 100:
            mode = UNIMAC_SPEED_100;
            break;
        case 2500:
            mode = UNIMAC_SPEED_2500;
            break;
        default:
            mode = UNIMAC_SPEED_1000;
            break;
    }

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_ETH_SPEEDf_SET(unimac_config, mode);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_speed_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                uint32_t *speed)
{
    int ioerr = 0;
    unimac_speed_mode_t mode;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    mode = COMMAND_CONFIGr_ETH_SPEEDf_GET(unimac_config);

    switch (mode) {
        case UNIMAC_SPEED_10:
            *speed = 10;
            break;
        case UNIMAC_SPEED_100:
            *speed = 100;
            break;
        case UNIMAC_SPEED_2500:
            *speed = 2500;
            break;
        default:
            *speed = 1000;
            break;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_encap_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                bcmpmac_encap_mode_t encap)
{
    if (encap != BCMPMAC_ENCAP_IEEE) {
        return SHR_E_UNAVAIL;
    }
    return SHR_E_NONE;
}

static inline int
gport_encap_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t *encap)
{
    *encap = BCMPMAC_ENCAP_IEEE;

    return SHR_E_NONE;
}

static inline int
gport_lpbk_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t en)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_LOOP_ENAf_SET(unimac_config, en);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t *en)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    *en = COMMAND_CONFIGr_LOOP_ENAf_GET(unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_pause_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    COMMAND_CONFIGr_PAUSE_IGNOREf_SET(unimac_config,
                                      ctrl->rx_enable ? 0 : 1);
    COMMAND_CONFIGr_IGNORE_TX_PAUSEf_SET(unimac_config,
                                         ctrl->tx_enable ? 0 : 1);
    ioerr += WRITE_COMMAND_CONFIGr(pa, port, unimac_config);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_pause_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    COMMAND_CONFIGr_t unimac_config;

    COMMAND_CONFIGr_CLR(unimac_config);

    ioerr += READ_COMMAND_CONFIGr(pa, port, &unimac_config);
    ctrl->rx_enable =
        COMMAND_CONFIGr_PAUSE_IGNOREf_GET(unimac_config) ? 0 : 1;
    ctrl->tx_enable =
        COMMAND_CONFIGr_IGNORE_TX_PAUSEf_GET(unimac_config) ? 0 : 1;

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_pause_addr_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     bcmpmac_mac_t mac)
{
    int ioerr = 0;
    MAC_0r_t tx_mac_sa_lo;
    MAC_1r_t tx_mac_sa_hi;
    uint32_t sa_hi, sa_lo;

    MAC_0r_CLR(tx_mac_sa_lo);
    MAC_1r_CLR(tx_mac_sa_hi);

    sa_hi = (mac[1] << 8) | mac[0];
    sa_lo = (mac[5] << 24) | (mac[4] << 16) | (mac[3] << 8) | mac[2];

    ioerr += READ_MAC_0r(pa, port, &tx_mac_sa_lo);
    ioerr += READ_MAC_1r(pa, port, &tx_mac_sa_hi);
    MAC_0r_MAC_ADDR0f_SET(tx_mac_sa_lo, sa_lo);
    MAC_1r_MAC_ADDR1f_SET(tx_mac_sa_hi, sa_hi);
    ioerr += WRITE_MAC_0r(pa, port, tx_mac_sa_lo);
    ioerr += WRITE_MAC_1r(pa, port, tx_mac_sa_hi);

    return SHR_E_NONE;
}

static inline int
gport_pause_addr_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     bcmpmac_mac_t mac)
{
    int ioerr = 0;
    MAC_0r_t tx_mac_sa_lo;
    MAC_1r_t tx_mac_sa_hi;
    uint32_t sa_hi, sa_lo;

    MAC_0r_CLR(tx_mac_sa_lo);
    MAC_1r_CLR(tx_mac_sa_hi);

    ioerr += READ_MAC_0r(pa, port, &tx_mac_sa_lo);
    ioerr += READ_MAC_1r(pa, port, &tx_mac_sa_hi);
    sa_lo = MAC_0r_MAC_ADDR0f_GET(tx_mac_sa_lo);
    sa_hi = MAC_1r_MAC_ADDR1f_GET(tx_mac_sa_hi);

    mac[0] = (uint8_t)(sa_hi);
    mac[1] = (uint8_t)(sa_hi >> 8);
    mac[2] = (uint8_t)(sa_lo);
    mac[3] = (uint8_t)(sa_lo >> 8);
    mac[4] = (uint8_t)(sa_lo >> 16);
    mac[5] = (uint8_t)(sa_lo >> 24);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_frame_max_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t size)
{
    int ioerr = 0;
    FRM_LENGTHr_t rx_max_size;

    FRM_LENGTHr_CLR(rx_max_size);

    ioerr += READ_FRM_LENGTHr(pa, port, &rx_max_size);
    FRM_LENGTHr_MAXFRf_SET(rx_max_size, size);
    ioerr += WRITE_FRM_LENGTHr(pa, port, rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_frame_max_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t *size)
{
    int ioerr = 0;
    FRM_LENGTHr_t rx_max_size;

    FRM_LENGTHr_CLR(rx_max_size);

    ioerr += READ_FRM_LENGTHr(pa, port, &rx_max_size);
    *size = FRM_LENGTHr_MAXFRf_GET(rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_fault_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       bcmpmac_fault_status_t *st)
{
    sal_memset(st, 0, sizeof(*st));

    return SHR_E_NONE;
}

static inline int
gport_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    bcmpmac_core_port_mode_t *mode)
{
    return SHR_E_NONE;
}

static inline int
gport_tx_flush(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t flush)
{
    int ioerr = 0;
    FLUSH_CONTROLr_t flush_control;

    FLUSH_CONTROLr_CLR(flush_control);

    ioerr += READ_FLUSH_CONTROLr(pa, port, &flush_control);
    FLUSH_CONTROLr_FLUSHf_SET(flush_control, flush);
    ioerr += WRITE_FLUSH_CONTROLr(pa, port, flush_control);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_failover_status_toggle(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t arg)
{
    return SHR_E_NONE;
}

static inline int
gport_failover_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t *failover_lpbk)
{
    return SHR_E_NONE;
}

static inline int
gport_failover_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          uint32_t en)
{
    return SHR_E_NONE;
}

static inline int
gport_failover_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          uint32_t *en)
{
    return SHR_E_NONE;
}

static inline int
gport_failover_lpbk_remove(bcmpmac_access_t *pa, bcmpmac_pport_t port)
{
    return SHR_E_NONE;
}

static inline int
gport_pfc_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    MAC_PFC_CTRLr_t pfc_ctrl;
    MAC_PFC_TYPEr_t pfc_type;
    MAC_PFC_OPCODEr_t pfc_opcode;
    MAC_PFC_DA_0r_t pfc_da_lo;
    MAC_PFC_DA_1r_t pfc_da_hi;
    MAC_PFC_REFRESH_CTRLr_t pfc_refresh_ctrl;
    PFC_XOFF_TIMERr_t pfc_xoff_timer;
    uint32_t da_hi, da_lo;

    MAC_PFC_TYPEr_CLR(pfc_type);
    MAC_PFC_OPCODEr_CLR(pfc_opcode);
    MAC_PFC_DA_0r_CLR(pfc_da_lo);
    MAC_PFC_DA_1r_CLR(pfc_da_hi);
    MAC_PFC_REFRESH_CTRLr_CLR(pfc_refresh_ctrl);
    MAC_PFC_CTRLr_CLR(pfc_ctrl);
    PFC_XOFF_TIMERr_CLR(pfc_xoff_timer);

    ioerr += READ_MAC_PFC_TYPEr(pa, port, &pfc_type);
    MAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, cfg->eth_type);
    ioerr += WRITE_MAC_PFC_TYPEr(pa, port, pfc_type);

    ioerr += READ_MAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    MAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, cfg->opcode);
    ioerr += WRITE_MAC_PFC_OPCODEr(pa, port, pfc_opcode);

    da_hi = (cfg->da[0] << 8) | cfg->da[1];
    da_lo = (cfg->da[2] << 24) | (cfg->da[3] << 16) |
            (cfg->da[4] << 8) | cfg->da[5];
    ioerr += READ_MAC_PFC_DA_0r(pa, port, &pfc_da_lo);
    ioerr += READ_MAC_PFC_DA_1r(pa, port, &pfc_da_hi);
    MAC_PFC_DA_0r_PFC_MACDA_0f_SET(pfc_da_lo, da_lo);
    MAC_PFC_DA_1r_PFC_MACDA_1f_SET(pfc_da_hi, da_hi);
    ioerr += WRITE_MAC_PFC_DA_0r(pa, port, pfc_da_lo);
    ioerr += WRITE_MAC_PFC_DA_1r(pa, port, pfc_da_hi);

    ioerr += READ_MAC_PFC_REFRESH_CTRLr(pa, port, &pfc_refresh_ctrl);
    MAC_PFC_REFRESH_CTRLr_PFC_REFRESH_ENf_SET
        (pfc_refresh_ctrl, (cfg->refresh_timer > 0) ? 1 : 0);
    MAC_PFC_REFRESH_CTRLr_PFC_REFRESH_TIMERf_SET
        (pfc_refresh_ctrl, cfg->refresh_timer);
    ioerr += WRITE_MAC_PFC_REFRESH_CTRLr(pa, port, pfc_refresh_ctrl);

    ioerr += READ_MAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    MAC_PFC_CTRLr_PFC_TX_ENBLf_SET(pfc_ctrl, cfg->tx_enable);
    MAC_PFC_CTRLr_PFC_RX_ENBLf_SET(pfc_ctrl, cfg->rx_enable);
    MAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, cfg->stats_enable);
    ioerr += WRITE_MAC_PFC_CTRLr(pa, port, pfc_ctrl);

    ioerr += READ_PFC_XOFF_TIMERr(pa, port, &pfc_xoff_timer);
    PFC_XOFF_TIMERr_PFC_XOFF_TIMERf_SET(pfc_xoff_timer, cfg->xoff_timer);
    ioerr += WRITE_PFC_XOFF_TIMERr(pa, port, pfc_xoff_timer);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_pfc_get(bcmpmac_access_t *pa, bcmpmac_pport_t port, bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    MAC_PFC_CTRLr_t pfc_ctrl;
    MAC_PFC_TYPEr_t pfc_type;
    MAC_PFC_OPCODEr_t pfc_opcode;
    MAC_PFC_DA_0r_t pfc_da_lo;
    MAC_PFC_DA_1r_t pfc_da_hi;
    MAC_PFC_REFRESH_CTRLr_t pfc_refresh_ctrl;
    PFC_XOFF_TIMERr_t pfc_xoff_timer;
    uint32_t da_hi, da_lo;

    MAC_PFC_TYPEr_CLR(pfc_type);
    MAC_PFC_OPCODEr_CLR(pfc_opcode);
    MAC_PFC_DA_0r_CLR(pfc_da_lo);
    MAC_PFC_DA_1r_CLR(pfc_da_hi);
    MAC_PFC_REFRESH_CTRLr_CLR(pfc_refresh_ctrl);
    MAC_PFC_CTRLr_CLR(pfc_ctrl);
    PFC_XOFF_TIMERr_CLR(pfc_xoff_timer);

    ioerr += READ_MAC_PFC_TYPEr(pa, port, &pfc_type);
    cfg->eth_type = MAC_PFC_TYPEr_PFC_ETH_TYPEf_GET(pfc_type);

    ioerr += READ_MAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    cfg->opcode = MAC_PFC_OPCODEr_PFC_OPCODEf_GET(pfc_opcode);

    ioerr += READ_MAC_PFC_DA_0r(pa, port, &pfc_da_lo);
    ioerr += READ_MAC_PFC_DA_1r(pa, port, &pfc_da_hi);
    da_lo = MAC_PFC_DA_0r_PFC_MACDA_0f_GET(pfc_da_lo);
    da_hi = MAC_PFC_DA_1r_PFC_MACDA_1f_GET(pfc_da_hi);
    cfg->da[0] = (uint8_t)(da_hi >> 8);
    cfg->da[1] = (uint8_t)(da_hi);
    cfg->da[2] = (uint8_t)(da_lo >> 24);
    cfg->da[3] = (uint8_t)(da_lo >> 16);
    cfg->da[4] = (uint8_t)(da_lo >> 8);
    cfg->da[5] = (uint8_t)(da_lo);

    ioerr += READ_MAC_PFC_REFRESH_CTRLr(pa, port, &pfc_refresh_ctrl);
    cfg->refresh_timer =
        MAC_PFC_REFRESH_CTRLr_PFC_REFRESH_TIMERf_GET(pfc_refresh_ctrl);

    ioerr += READ_MAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    cfg->tx_enable = MAC_PFC_CTRLr_PFC_TX_ENBLf_GET(pfc_ctrl);
    cfg->rx_enable = MAC_PFC_CTRLr_PFC_RX_ENBLf_GET(pfc_ctrl);
    cfg->stats_enable = MAC_PFC_CTRLr_PFC_STATS_ENf_GET(pfc_ctrl);

    ioerr += READ_PFC_XOFF_TIMERr(pa, port, &pfc_xoff_timer);
    cfg->xoff_timer = PFC_XOFF_TIMERr_PFC_XOFF_TIMERf_GET(pfc_xoff_timer);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
gport_op_exec(bcmpmac_access_t *pa, bcmpmac_pport_t port, const char *op,
              uint32_t op_param)
{
    bcmpmac_drv_op_f op_func;
    const bcmpmac_drv_op_hdl_t ops[] = {
        { "port_reset", gport_port_reset_set },
        { "port_enable", gport_port_enable_set },
        { "mac_reset", gport_mac_reset_set },
        { "rx_enable", gport_rx_enable_set },
        { "tx_enable", gport_tx_enable_set },
        { "tx_flush", gport_tx_flush },
        { "speed_set", gport_speed_set },
        { "failover_toggle", gport_failover_status_toggle}
    };

    op_func = bcmpmac_drv_op_func_get(ops, COUNTOF(ops), op);
    if (!op_func) {
        return SHR_E_UNAVAIL;
    }

    return op_func(pa, port, op_param);
}

static inline int
gport_fault_disable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        bcmpmac_fault_disable_t *st)
{
    sal_memset(st, 0, sizeof(*st));

    return SHR_E_NONE;
}

#endif /* BCMPMAC_GPORT_H */
