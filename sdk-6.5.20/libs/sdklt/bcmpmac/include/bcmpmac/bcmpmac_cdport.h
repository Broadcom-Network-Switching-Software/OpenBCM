/*! \file bcmpmac_cdport.h
 *
 * CDPORT driver.
 *
 * A CDPORT contains 1 CDPORT and 2 CDMACs, and supports 8 ports at most.
 *
 * In this driver, we always use the port number 0~7 to access the CDPORT
 * and CDMAC per-port registers without considering which CDMAC should be
 * configured.
 *
 * The lower level register access driver would know how to determine which
 * CDMAC should be accessed, i.e. port 0~3 belongs the first CDMAC and port 4~7
 * belongs to the second CDMAC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_CDPORT_H
#define BCMPMAC_CDPORT_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>
#include <bcmpmac/chip/bcmpmac_common_cdmac_defs.h>
#include <shr/shr_bitop.h>


/*******************************************************************************
 * Local definitions
 */

/*! Drain cell waiting time. */
#define DRAIN_WAIT_MSEC 500

/*! Number of ports per CDMAC. */
#define PORTS_PER_CDMAC 4

/*! Minimum RUNT threshold. */
#define CDMAC_RUNT_THRESHOLD_MIN      64

/*! Maximum RUNT threshold. */
#define CDMAC_RUNT_THRESHOLD_MAX      96

/*! Minimun Average IPG */
#define CDMAC_AVE_IPG_MIN             8

/*! Maximum Average IPG */
#define CDMAC_AVE_IPG_MAX             60

/*! CDMAC header mode value per encap mode. */
typedef enum cdmac_hdr_mode_e {

    /*! IEEE Ethernet format. */
    CDMAC_HDR_MODE_IEEE = 0,

    /*! /S/ (K.SOP) character in header. */
    CDMAC_HDR_MODE_KSOP = 5,

    /*! HiGig3 encapsulation format. */
    CDMAC_HDR_MODE_HG3 = 6,

} cdmac_hdr_mode_t;

/*! CDPORT port mode value. */
typedef enum cdport_port_mode_e {

    /*! Quad Port Mode. All four ports are enabled. */
    CDPORT_PORT_MODE_QUAD = 0,

    /*! Tri Port Mode. Lanes 0, 1, and 2 active. lane 2 is dual. */
    CDPORT_PORT_MODE_TRI_012 = 1,

    /*! Tri Port Mode. Lanes 0, 2, and 3 active. lane 0 is dual. */
    CDPORT_PORT_MODE_TRI_023 = 2,

    /*! Dual Port Mode. Each of lanes 0 and 2 are dual. */
    CDPORT_PORT_MODE_DUAL = 3,

    /*! Single Port Mode. Lanes 0 through 3 are single XLGMII. */
    CDPORT_PORT_MODE_SINGLE = 4

} cdport_port_mode_t;

/*! CDMAC CRC MODE. */
typedef enum cdmac_crc_mode_e {

    /*! CRC is computed on incoming packet data and appended. */
    CDMAC_CRC_MODE_APPEND = 0,

    /*! Incoming pkt CRC is passed through without modifications. */
    CDMAC_CRC_MODE_KEEP = 1,

    /*! Incoming pkt CRC is replaced with CRC value computed by the MAC. */
    CDMAC_CRC_MODE_REPLACE = 2,

    /*! The CRC mode is determined by the HW. */
    CDMAC_CRC_MODE_AUTO = 3,

} cdmac_crc_mode_t;


/******************************************************************************
 * Private functions
 */

static inline int
cdport_port_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t reset)
{
    /* There is no per-port RESET control in CDPORT. */
    return SHR_E_NONE;
}

static inline int
cdport_port_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t *reset)
{
    /* There is no per-port RESET control in CDPORT. */
    *reset = 0;
    return SHR_E_NONE;
}

static inline int
cdport_port_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t enable)
{
    /* There is no per-port ENABLE control in CDPORT. */
    return SHR_E_NONE;
}

static inline int
cdport_port_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t *enable)
{
    /* There is no per-port ENABLE control in CDPORT. */
    *enable = 1;
    return SHR_E_NONE;
}

static inline int
cdport_mac_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t reset)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_SOFT_RESETf_SET(mac_ctrl, reset);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_mac_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *reset)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *reset = CDMAC_CTRLr_SOFT_RESETf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_rx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_RX_ENf_SET(mac_ctrl, enable);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_rx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *enable = CDMAC_CTRLr_RX_ENf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_tx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_TX_ENf_SET(mac_ctrl, enable);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_tx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *enable = CDMAC_CTRLr_TX_ENf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_speed_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t speed)
{
    /* There is no port SPEED control in CDMAC. */
    return SHR_E_NONE;
}

static inline int
cdport_speed_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t *speed)
{
    /* There is no port SPEED control in CDMAC. */
    *speed = 0;
    return SHR_E_NONE;
}

static inline int
cdport_encap_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t encap)
{
    int ioerr = 0;
    CDMAC_MODEr_t cdmac_mode;
    uint32_t hdr_mode;

    CDMAC_MODEr_CLR(cdmac_mode);

    switch (encap) {
        case BCMPMAC_ENCAP_IEEE:
            hdr_mode = CDMAC_HDR_MODE_IEEE;
            break;
        case BCMPMAC_ENCAP_HG3:
            hdr_mode = CDMAC_HDR_MODE_HG3;
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += READ_CDMAC_MODEr(pa, port, &cdmac_mode);
    CDMAC_MODEr_HDR_MODEf_SET(cdmac_mode, hdr_mode);
    ioerr += WRITE_CDMAC_MODEr(pa, port, cdmac_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_encap_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t *encap)
{
    int ioerr = 0;
    CDMAC_MODEr_t cdmac_mode;
    uint32_t hdr_mode;

    CDMAC_MODEr_CLR(cdmac_mode);

    ioerr += READ_CDMAC_MODEr(pa, port, &cdmac_mode);
    hdr_mode = CDMAC_MODEr_HDR_MODEf_GET(cdmac_mode);

    switch (hdr_mode) {
        case CDMAC_HDR_MODE_IEEE:
            *encap = BCMPMAC_ENCAP_IEEE;
            break;
        case BCMPMAC_ENCAP_HG3:
            *encap = CDMAC_HDR_MODE_HG3;
            break;
        default:
            *encap = BCMPMAC_ENCAP_COUNT;
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_lpbk_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_LOCAL_LPBKf_SET(mac_ctrl, en ? 1 : 0);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t *en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *en = CDMAC_CTRLr_LOCAL_LPBKf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;

    CDMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    CDMAC_PAUSE_CTRLr_TX_PAUSE_ENf_SET(pause_ctrl, ctrl->tx_enable);
    CDMAC_PAUSE_CTRLr_RX_PAUSE_ENf_SET(pause_ctrl, ctrl->rx_enable);
    ioerr += WRITE_CDMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;

    CDMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    ctrl->tx_enable = CDMAC_PAUSE_CTRLr_TX_PAUSE_ENf_GET(pause_ctrl);
    ctrl->rx_enable = CDMAC_PAUSE_CTRLr_RX_PAUSE_ENf_GET(pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_addr_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    CDMAC_TX_MAC_SAr_t tx_mac_sa;
    CDMAC_RX_MAC_SAr_t rx_mac_sa;
    uint32_t sa[2];

    CDMAC_TX_MAC_SAr_CLR(tx_mac_sa);
    CDMAC_RX_MAC_SAr_CLR(rx_mac_sa);

    sa[1] = (mac[5] << 8) | mac[4];
    sa[0] = (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0];

    ioerr += READ_CDMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    CDMAC_TX_MAC_SAr_CTRL_SAf_SET(tx_mac_sa, sa);
    ioerr += WRITE_CDMAC_TX_MAC_SAr(pa, port, tx_mac_sa);

    ioerr += READ_CDMAC_RX_MAC_SAr(pa, port, &rx_mac_sa);
    CDMAC_RX_MAC_SAr_RX_SAf_SET(rx_mac_sa, sa);
    ioerr += WRITE_CDMAC_RX_MAC_SAr(pa, port, rx_mac_sa);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pause_addr_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    CDMAC_TX_MAC_SAr_t tx_mac_sa;
    uint32_t sa[2];

    CDMAC_TX_MAC_SAr_CLR(tx_mac_sa);

    ioerr += READ_CDMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    CDMAC_TX_MAC_SAr_CTRL_SAf_GET(tx_mac_sa, sa);

    mac[5] = (uint8_t)(sa[1] >> 8);
    mac[4] = (uint8_t)(sa[1]);
    mac[3] = (uint8_t)(sa[0] >> 24);
    mac[2] = (uint8_t)(sa[0] >> 16);
    mac[1] = (uint8_t)(sa[0] >> 8);
    mac[0] = (uint8_t)(sa[0]);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_frame_max_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t size)
{
    int ioerr = 0;
    CDMAC_RX_MAX_SIZEr_t rx_max_size;

    CDMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    ioerr += READ_CDMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    CDMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_SET(rx_max_size, size);
    ioerr += WRITE_CDMAC_RX_MAX_SIZEr(pa, port, rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_frame_max_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *size)
{
    int ioerr = 0;
    CDMAC_RX_MAX_SIZEr_t rx_max_size;

    CDMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    ioerr += READ_CDMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    *size = CDMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_GET(rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_fault_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        bcmpmac_fault_status_t *st)
{
    int ioerr = 0;
    CDMAC_RX_LSS_CTRLr_t lss_ctrl;
    CDMAC_RX_LSS_STATUSr_t lss_st;

    sal_memset(st, 0, sizeof(*st));
    CDMAC_RX_LSS_CTRLr_CLR(lss_ctrl);
    CDMAC_RX_LSS_STATUSr_CLR(lss_st);

    /*
     *  The fault status bit in CDMAC_RX_LSS_STATUSr is cleared-on-read.
     *  Read it twice to make sure the fault status is set.
     */
    ioerr += READ_CDMAC_RX_LSS_STATUSr(pa, port, &lss_st);
    ioerr += READ_CDMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);
    ioerr += READ_CDMAC_RX_LSS_STATUSr(pa, port, &lss_st);

    /* The fault status is vaild when the fault control is enabled. */
    if (CDMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->local_fault =
            CDMAC_RX_LSS_STATUSr_LOCAL_FAULT_STATUSf_GET(lss_st);
    }

    if (CDMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->remote_fault =
            CDMAC_RX_LSS_STATUSr_REMOTE_FAULT_STATUSf_GET(lss_st);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

/*!
 * \brief Toggle LAG failover status.
 *
 * Toggle link_status_up bit to notify IPIPE on link up.
 *
 * When a link comes up, hardware will not update the LINK_STATUS register
 * until software has toggled the the LAG_FAILOVER_CONFIG.LINK_STATUS_UP field.
 * When a link goes down, hardware will update the LINK_STATUS register without
 * software intervention.
 *
 * \param [in] pa PMAC access info.
 * \param [in] pm_pport Port macro physical port number.
 * \param [in] arg Not used.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_ACCESS Failed to access the register.
 */
static inline int
cdport_failover_status_toggle(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t arg)
{
    int ioerr = 0;
    CDPORT_LAG_FAILOVER_CONFIGr_t cfg;

    CDPORT_LAG_FAILOVER_CONFIGr_CLR(cfg);
    ioerr += READ_CDPORT_LAG_FAILOVER_CONFIGr(pa, port, &cfg);
    CDPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 1);
    ioerr += WRITE_CDPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);
    CDPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 0);
    ioerr += WRITE_CDPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *failover_lpbk)
{
    int ioerr = 0;
    CDMAC_LAG_FAILOVER_STATUSr_t st;

    CDMAC_LAG_FAILOVER_STATUSr_CLR(st);

    ioerr += READ_CDMAC_LAG_FAILOVER_STATUSr(pa, port, &st);
    *failover_lpbk = CDMAC_LAG_FAILOVER_STATUSr_LAG_FAILOVER_LOOPBACKf_GET(st);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;
    CDMAC_RX_LSS_CTRLr_t rx_lss;

    CDMAC_CTRLr_CLR(mac_ctrl);
    CDMAC_RX_LSS_CTRLr_CLR(rx_lss);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_LINK_STATUS_SELECTf_SET(mac_ctrl, en);
    CDMAC_CTRLr_LAG_FAILOVER_ENf_SET(mac_ctrl, en);
    CDMAC_CTRLr_MAC_LINK_DOWN_SEQ_ENf_SET(mac_ctrl, !en);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CDMAC_RX_LSS_CTRLr(pa, port, &rx_lss);
    CDMAC_RX_LSS_CTRLr_DROP_TX_DATA_ON_LINK_INTERRUPTf_SET(rx_lss, en);
    CDMAC_RX_LSS_CTRLr_RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf_SET(rx_lss, en);
    ioerr += WRITE_CDMAC_RX_LSS_CTRLr(pa, port, rx_lss);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t *en)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *en = CDMAC_CTRLr_LINK_STATUS_SELECTf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_failover_lpbk_remove(bcmpmac_access_t *pa, bcmpmac_pport_t port)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 1);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 0);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    /* Toggle the lag failover status. */
    ioerr += cdport_failover_status_toggle(pa, port, 0);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pfc_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;
    CDMAC_PFC_CTRLr_t pfc_ctrl;
    CDMAC_PFC_TYPEr_t pfc_type;
    CDMAC_PFC_OPCODEr_t pfc_opcode;
    CDMAC_PFC_DAr_t pfc_da;
    uint32_t da[2];

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    CDMAC_PAUSE_CTRLr_PFC_REFRESH_ENf_SET(pause_ctrl, (cfg->refresh_timer > 0));
    CDMAC_PAUSE_CTRLr_PFC_REFRESH_TIMERf_SET(pause_ctrl, cfg->refresh_timer);
    CDMAC_PAUSE_CTRLr_PFC_XOFF_TIMERf_SET(pause_ctrl, cfg->xoff_timer);
    ioerr += WRITE_CDMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    CDMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, cfg->xon_enable);
    CDMAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, cfg->stats_enable);
    CDMAC_PFC_CTRLr_TX_PFC_ENf_SET(pfc_ctrl, cfg->tx_enable);
    CDMAC_PFC_CTRLr_RX_PFC_ENf_SET(pfc_ctrl, cfg->rx_enable);
    ioerr += WRITE_CDMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    /*
     * If pfc is disabled on RX, toggle FORCE_XON.
     * This forces the MAC to generate an XON indication to
     * the MMU for all classes of service in the receive direction.
     * Do not check the previous pfx rx enable state.
     * This is not needed when pfc is enabled on RX.
     */
    if (!cfg->rx_enable) {
        ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
        CDMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, 1);
        ioerr += WRITE_CDMAC_PFC_CTRLr(pa, port, pfc_ctrl);

        ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
        CDMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, 0);
        ioerr += WRITE_CDMAC_PFC_CTRLr(pa, port, pfc_ctrl);
    }

    ioerr += READ_CDMAC_PFC_TYPEr(pa, port, &pfc_type);
    CDMAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, cfg->eth_type);
    ioerr += WRITE_CDMAC_PFC_TYPEr(pa, port, pfc_type);

    ioerr += READ_CDMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    CDMAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, cfg->opcode);
    ioerr += WRITE_CDMAC_PFC_OPCODEr(pa, port, pfc_opcode);

    da[1] = (cfg->da[5] << 8) | cfg->da[4];
    da[0] = (cfg->da[3] << 24) | (cfg->da[2] << 16) |
            (cfg->da[1] << 8) | cfg->da[0];
    ioerr += READ_CDMAC_PFC_DAr(pa, port, &pfc_da);
    CDMAC_PFC_DAr_PFC_MACDAf_SET(pfc_da, da);
    ioerr += WRITE_CDMAC_PFC_DAr(pa, port, pfc_da);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_pfc_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;
    CDMAC_PFC_CTRLr_t pfc_ctrl;
    CDMAC_PFC_TYPEr_t pfc_type;
    CDMAC_PFC_OPCODEr_t pfc_opcode;
    CDMAC_PFC_DAr_t pfc_da;
    uint32_t da[2];

    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    cfg->refresh_timer = CDMAC_PAUSE_CTRLr_PFC_REFRESH_TIMERf_GET(pause_ctrl);
    cfg->xoff_timer = CDMAC_PAUSE_CTRLr_PFC_XOFF_TIMERf_GET(pause_ctrl);

    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    cfg->xon_enable = CDMAC_PFC_CTRLr_FORCE_PFC_XONf_GET(pfc_ctrl);
    cfg->stats_enable = CDMAC_PFC_CTRLr_PFC_STATS_ENf_GET(pfc_ctrl);
    cfg->tx_enable = CDMAC_PFC_CTRLr_TX_PFC_ENf_GET(pfc_ctrl);
    cfg->rx_enable = CDMAC_PFC_CTRLr_RX_PFC_ENf_GET(pfc_ctrl);

    ioerr += READ_CDMAC_PFC_TYPEr(pa, port, &pfc_type);
    cfg->eth_type = CDMAC_PFC_TYPEr_PFC_ETH_TYPEf_GET(pfc_type);

    ioerr += READ_CDMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    cfg->opcode = CDMAC_PFC_OPCODEr_PFC_OPCODEf_GET(pfc_opcode);

    ioerr += READ_CDMAC_PFC_DAr(pa, port, &pfc_da);
    CDMAC_PFC_DAr_PFC_MACDAf_GET(pfc_da, da);
    cfg->da[5] = (uint8_t)(da[1] >> 8);
    cfg->da[4] = (uint8_t)(da[1]);
    cfg->da[3] = (uint8_t)(da[0] >> 24);
    cfg->da[2] = (uint8_t)(da[0] >> 16);
    cfg->da[1] = (uint8_t)(da[0] >> 8);
    cfg->da[0] = (uint8_t)(da[0]);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_fault_disable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_fault_disable_t *st)
{
    int ioerr = 0;
    CDMAC_RX_LSS_CTRLr_t lss_ctrl;

    CDMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr += READ_CDMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    CDMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_SET(lss_ctrl,
                                                st->local_fault_disable);
    CDMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_SET(lss_ctrl,
                                                 st->remote_fault_disable);
    ioerr += WRITE_CDMAC_RX_LSS_CTRLr(pa, port, lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_fault_disable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_fault_disable_t *st)
{
    int ioerr = 0;
    CDMAC_RX_LSS_CTRLr_t lss_ctrl;

    CDMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr = READ_CDMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    st->local_fault_disable =
        CDMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl);

    st->remote_fault_disable =
        CDMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_avg_ipg_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint8_t ipg_size)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    /*
     * Average inter packet gap can be in the range 8 to 56 or 60.
     * should be 56 for XLGMII, 60 for XGMII,
     * default is 12.
     */
    if ((ipg_size < CDMAC_AVE_IPG_MIN) ||
        (ipg_size > CDMAC_AVE_IPG_MAX)) {
        return SHR_E_PARAM;
    }

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_AVERAGE_IPGf_SET(tx_ctrl, ipg_size);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_avg_ipg_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint8_t *ipg_size)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr = READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *ipg_size = CDMAC_TX_CTRLr_AVERAGE_IPGf_GET(tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_interrupt_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                            uint32_t enable)
{
    int ioerr = 0;
    CDMAC_INTR_ENABLEr_t intr_en;

    CDMAC_INTR_ENABLEr_CLR(intr_en);

    ioerr = READ_CDMAC_INTR_ENABLEr(pa, port, &intr_en);
    CDMAC_INTR_ENABLEr_MIB_COUNTER_MULTIPLE_ERRf_SET(intr_en, enable);
    CDMAC_INTR_ENABLEr_MIB_COUNTER_DOUBLE_BIT_ERRf_SET(intr_en, enable);
    CDMAC_INTR_ENABLEr_MIB_COUNTER_SINGLE_BIT_ERRf_SET(intr_en, enable);
    CDMAC_INTR_ENABLEr_TX_CDC_DOUBLE_BIT_ERRf_SET(intr_en, enable);
    CDMAC_INTR_ENABLEr_TX_CDC_SINGLE_BIT_ERRf_SET(intr_en, enable);
    ioerr += WRITE_CDMAC_INTR_ENABLEr(pa, port, intr_en);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_interrupt_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                            uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_INTR_ENABLEr_t intr_en;

    CDMAC_INTR_ENABLEr_CLR(intr_en);

    ioerr = READ_CDMAC_INTR_ENABLEr(pa, port, &intr_en);
    *enable = CDMAC_INTR_ENABLEr_MIB_COUNTER_MULTIPLE_ERRf_GET(intr_en);
    *enable |= CDMAC_INTR_ENABLEr_MIB_COUNTER_DOUBLE_BIT_ERRf_GET(intr_en);
    *enable |= CDMAC_INTR_ENABLEr_MIB_COUNTER_SINGLE_BIT_ERRf_GET(intr_en);
    *enable |= CDMAC_INTR_ENABLEr_TX_CDC_DOUBLE_BIT_ERRf_GET(intr_en);
    *enable |= CDMAC_INTR_ENABLEr_TX_CDC_SINGLE_BIT_ERRf_GET(intr_en);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_mib_counter_control_set (bcmpmac_access_t *pa, bcmpmac_pport_t port,
                               int enable, int clear)
{
    int ioerr = 0;
    CDMAC_MIB_COUNTER_CTRLr_t mib_ctrl;

    CDMAC_MIB_COUNTER_CTRLr_CLR(mib_ctrl);

    /*
     * For enabling the MIB statistics counters for a port, the ENABLE field
     * must be set.
     * To reset/clear MIB statistics counters, CLEAR field should be set 1.
     * A low-to-high(0->1) transition on this bit(CLEAR field) will trigger
     * the counter-clear operation.
     * Please Note:
     * If a subsequent counter-clear operation is required, this bit has to
     * be first written to 0 and then written to 1. Instead of doing this
     * setting CLEAR field to 0 after setting to 1.
     * SW name for CLEAR field is CNT_CLEARf.
     */
    ioerr += READ_CDMAC_MIB_COUNTER_CTRLr(pa, port, &mib_ctrl);
    CDMAC_MIB_COUNTER_CTRLr_ENABLEf_SET(mib_ctrl, enable);
    CDMAC_MIB_COUNTER_CTRLr_CNT_CLEARf_SET(mib_ctrl, clear);
    ioerr += WRITE_CDMAC_MIB_COUNTER_CTRLr(pa, port, mib_ctrl);

    /*
     * set CLEARf to 0, this operation is not mandatory, adding to
     * remove ambiguity in interpretation if a read on this register
     */
    if (clear) {
        ioerr += READ_CDMAC_MIB_COUNTER_CTRLr(pa, port, &mib_ctrl);
        CDMAC_MIB_COUNTER_CTRLr_CNT_CLEARf_SET(mib_ctrl, 0);
        ioerr += WRITE_CDMAC_MIB_COUNTER_CTRLr(pa, port, mib_ctrl);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_mib_oversize_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t size)
{
    int ioerr = 0;
    CDMAC_MIB_COUNTER_CTRLr_t mib_ctrl;

    CDMAC_MIB_COUNTER_CTRLr_CLR(mib_ctrl);

    /*
     * The maximum packet size that is used in statistics counter updates.
     * default size is 1518. Note if RX_MAX_SIZE(max frame size received)
     * is greater than CNTMAXSIZEf, a good packet that is valid CRC and
     * contains no other errors, will increment the OVR(oversize) counters
     * if the length of the packet > CNTMAXSIZE < RX_MAX_SIZE values.
     * Having CNTMAXSIZE > RX_MAX_SIZE is not recommended.
     * This is generally taken care in the statistics module while
     * accumulating the counts.
     */
    ioerr += READ_CDMAC_MIB_COUNTER_CTRLr(pa, port, &mib_ctrl);
    CDMAC_MIB_COUNTER_CTRLr_CNTMAXSIZEf_SET(mib_ctrl, size);
    ioerr += WRITE_CDMAC_MIB_COUNTER_CTRLr(pa, port, mib_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_mib_oversize_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t *size)
{
    int ioerr = 0;
    CDMAC_MIB_COUNTER_CTRLr_t mib_ctrl;

    CDMAC_MIB_COUNTER_CTRLr_CLR(mib_ctrl);

    ioerr += READ_CDMAC_MIB_COUNTER_CTRLr(pa, port, &mib_ctrl);
    *size = CDMAC_MIB_COUNTER_CTRLr_CNTMAXSIZEf_GET(mib_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_pass_control_frame_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t enable)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    /*
     * This configuration is used to drop or pass all control frames
     * (with ether type 0x8808) except pause packets.
     * If set, all control frames are passed to system side.
     * if reset, control frames (including pfc frames wih ether type 0x8808)i
     * are dropped in CDMAC.
     */
    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    CDMAC_RX_CTRLr_RX_PASS_CTRLf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_CDMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdmac_pass_control_frame_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = CDMAC_RX_CTRLr_RX_PASS_CTRLf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdmac_pass_pfc_frame_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t enable)
{
    int ioerr = 0;
    uint32_t status = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    /*
     * This configuration is used to pass or drop PFC packets when
     * PFC_ETH_TYPE is not equal to 0x8808.
     * If set, PFC frames are passed to system side.
     * If reset, PFC frames are dropped in CDMAC.
     */
    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    status = CDMAC_RX_CTRLr_RX_PASS_PFCf_GET(rx_ctrl);

    if (status != enable) {
        CDMAC_RX_CTRLr_RX_PASS_PFCf_SET(rx_ctrl, (enable? 1: 0));
    }
    ioerr += WRITE_CDMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdmac_pass_pfc_frame_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = CDMAC_RX_CTRLr_RX_PASS_PFCf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdmac_pass_pause_frame_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t enable)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    /*
     * If set, PAUSE frames are passed to sytem side.
     * If reset, PAUSE frames are dropped in CDMAC
     */
    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    CDMAC_RX_CTRLr_RX_PASS_PAUSEf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_CDMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdmac_pass_pause_frame_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = CDMAC_RX_CTRLr_RX_PASS_PAUSEf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_discard_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                  uint32_t discard)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_DISCARDf_SET(tx_ctrl, discard);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_discard_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                  uint32_t *discard)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr = READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *discard = CDMAC_TX_CTRLr_DISCARDf_GET(tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_sw_link_status_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         int link)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    /*
     * This is valid only if LINK_STATUS_SELECT is 0, which means the
     * software drives the link status. If SW_LINK_STATUS is set,
     * it indicates that the link is active. The use case is if there is
     * some other mechanism used for the link status determination other
     * than hardware.
     */
    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_SW_LINK_STATUSf_SET(mac_ctrl, link);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_sw_link_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         int *link)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *link = CDMAC_CTRLr_SW_LINK_STATUSf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_sw_link_status_select_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                int enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    /*
     * This configuration chooses between link status indication from software
     * (SW_LINK_STATUSf) or the hardware link status indication from the PCS
     * to MAC. If set, hardware link status is used.
     */
    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_LINK_STATUS_SELECTf_SET(mac_ctrl, enable);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_sw_link_status_select_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                                int *enable)
{
    int ioerr = 0;
    CDMAC_CTRLr_t mac_ctrl;

    CDMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    *enable = CDMAC_CTRLr_LINK_STATUS_SELECTf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_link_interrupt_ordered_set_enable(bcmpmac_access_t *pa,
                                        bcmpmac_pport_t port, uint32_t enable)
{
    int ioerr = 0;
    CDMAC_RX_LSS_CTRLr_t lss_ctrl;

    CDMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr = READ_CDMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    if (enable) {
        CDMAC_RX_LSS_CTRLr_FORCE_LINK_INTERRUPT_OSf_SET(lss_ctrl, 1);
        CDMAC_RX_LSS_CTRLr_FAULT_SOURCE_FOR_TXf_SET(lss_ctrl, 2);
    } else {
        CDMAC_RX_LSS_CTRLr_FORCE_LINK_INTERRUPT_OSf_SET(lss_ctrl, 0);
        CDMAC_RX_LSS_CTRLr_FAULT_SOURCE_FOR_TXf_SET(lss_ctrl, 0);
    }

    ioerr += WRITE_CDMAC_RX_LSS_CTRLr(pa, port, lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_link_interrupt_ordered_set_enable_get(bcmpmac_access_t *pa,
                                            bcmpmac_pport_t port,
                                            uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_RX_LSS_CTRLr_t lss_ctrl;

    CDMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr = READ_CDMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    *enable = CDMAC_RX_LSS_CTRLr_FORCE_LINK_INTERRUPT_OSf_GET(lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_link_interruption_live_status_get(bcmpmac_access_t *pa,
                                        bcmpmac_pport_t port,
                                        uint32_t *status)
{
    int ioerr = 0;
    CDMAC_RX_LSS_STATUSr_t lss_ctrl;

    CDMAC_RX_LSS_STATUSr_CLR(lss_ctrl);

    ioerr = READ_CDMAC_RX_LSS_STATUSr(pa, port, &lss_ctrl);

    /* fault status bits clear on read */
    *status = CDMAC_RX_LSS_STATUSr_LINK_INTERRUPTION_LIVE_STATUSf_GET(lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_stall_tx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          bool *enable)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *enable = CDMAC_TX_CTRLr_STALL_TXf_GET(tx_ctrl);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_stall_tx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                          bool enable)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    /*
     * When asserted, if the MAC Tx is in the middle of transmitting
     * a packet, it waits for the next packet boundary to stop
     * transmitting any more packets.
     */

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_STALL_TXf_SET(tx_ctrl, enable);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_runt_threshold_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t value)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    /*
     * The threshold, below which the packets are dropped or
     * marked as runt. Should be programmed >=64 and <= 96 bytes.
     */
    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);

    if ((value < CDMAC_RUNT_THRESHOLD_MIN) ||
        (value > CDMAC_RUNT_THRESHOLD_MAX)) {
        return SHR_E_PARAM;
    }

    CDMAC_RX_CTRLr_RUNT_THRESHOLDf_SET(rx_ctrl, value);
    ioerr += WRITE_CDMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
cdmac_runt_threshold_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *value)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *value = CDMAC_RX_CTRLr_RUNT_THRESHOLDf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t init)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;
    CDMAC_TX_CTRLr_t tx_ctrl;
    CDMAC_CTRLr_t mac_ctrl;
    CDMAC_PAUSE_CTRLr_t pause_ctrl;
    CDMAC_PFC_CTRLr_t pfc_ctrl;
    CDMAC_PFC_DAr_t pfc_da;
    CDMAC_PFC_OPCODEr_t pfc_opcode;
    CDMAC_PFC_TYPEr_t pfc_type;
    uint32_t da[2];
    bcmpmac_fault_disable_t st;

    if (!init) {
        return SHR_E_NONE;
    }

    /* Set recieve maximum frame size to reset value 1518. */
    ioerr += cdport_frame_max_set(pa, port, 0x5ee);

    /*
     * Enable and Clear the CDMAC MIB counters
     * No need to explicitly set the clear flag to 0 since the
     * function internally unsets the clear flag
     */
    ioerr += cdmac_mib_counter_control_set(pa, port, 1, 1);

    /* RX Control */
    CDMAC_RX_CTRLr_CLR(rx_ctrl);
    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    CDMAC_RX_CTRLr_STRIP_CRCf_SET(rx_ctrl, 0);
    CDMAC_RX_CTRLr_RX_PASS_PAUSEf_SET(rx_ctrl, 0);
    CDMAC_RX_CTRLr_RX_PASS_PFCf_SET(rx_ctrl, 0);
    CDMAC_RX_CTRLr_RUNT_THRESHOLDf_SET(rx_ctrl, 0x40);
    ioerr += WRITE_CDMAC_RX_CTRLr(pa, port, rx_ctrl);

    /* Configure MAC Tx. */
    CDMAC_TX_CTRLr_CLR(tx_ctrl);
    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_DISCARDf_SET(tx_ctrl, 0);
    CDMAC_TX_CTRLr_PAD_ENf_SET(tx_ctrl, 0);
    CDMAC_TX_CTRLr_PAD_THRESHOLDf_SET(tx_ctrl, 64);
    CDMAC_TX_CTRLr_AVERAGE_IPGf_SET(tx_ctrl, 12);
    CDMAC_TX_CTRLr_TX_THRESHOLDf_SET(tx_ctrl, 1);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    /* Disable local and remote faults. */
    sal_memset(&st, 0, sizeof(bcmpmac_fault_disable_t));
    st.local_fault_disable = 0;
    st.remote_fault_disable = 0;
    ioerr += cdport_fault_disable_set(pa, port, &st);

    /* Disable pause settings. */
    CDMAC_PAUSE_CTRLr_CLR(pause_ctrl);
    ioerr += READ_CDMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    CDMAC_PAUSE_CTRLr_TX_PAUSE_ENf_SET(pause_ctrl, 0);
    CDMAC_PAUSE_CTRLr_RX_PAUSE_ENf_SET(pause_ctrl, 0);
    ioerr += WRITE_CDMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    /* Disable pfc settings. */
    CDMAC_PFC_CTRLr_CLR(pfc_ctrl);
    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    CDMAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, 1);
    CDMAC_PFC_CTRLr_TX_PFC_ENf_SET(pfc_ctrl, 0);
    CDMAC_PFC_CTRLr_RX_PFC_ENf_SET(pfc_ctrl, 0);
    ioerr += WRITE_CDMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    /* Reset PFC MAC DA. */
    da[1] = 0x0180;
    da[0] = 0xc2000001;
    ioerr += READ_CDMAC_PFC_DAr(pa, port, &pfc_da);
    CDMAC_PFC_DAr_PFC_MACDAf_SET(pfc_da, da);
    ioerr += WRITE_CDMAC_PFC_DAr(pa, port, pfc_da);

    /* Reset PFC OPCODE. */
    ioerr += READ_CDMAC_PFC_TYPEr(pa, port, &pfc_type);
    CDMAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, 0x8808);
    ioerr += WRITE_CDMAC_PFC_TYPEr(pa, port, pfc_type);

    /* Reset PFC ETH TYPE. */
    ioerr += READ_CDMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    CDMAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, 0x101);
    ioerr += WRITE_CDMAC_PFC_OPCODEr(pa, port, pfc_opcode);

    /* MAC control settings. */
    CDMAC_CTRLr_CLR(mac_ctrl);
    ioerr += READ_CDMAC_CTRLr(pa, port, &mac_ctrl);
    CDMAC_CTRLr_LOCAL_LPBKf_SET(mac_ctrl, 0);
    CDMAC_CTRLr_TX_ENf_SET(mac_ctrl, 1);
    CDMAC_CTRLr_RX_ENf_SET(mac_ctrl, 1);
    CDMAC_CTRLr_MAC_LINK_DOWN_SEQ_ENf_SET(mac_ctrl, 1);
    /* de-assert SOFT RESET */
    CDMAC_CTRLr_SOFT_RESETf_SET(mac_ctrl, 0);
    ioerr += WRITE_CDMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_force_pfc_xon_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t value)
{
    int ioerr = 0;
    CDMAC_PFC_CTRLr_t pfc_ctrl;

    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    CDMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, value);
    ioerr += WRITE_CDMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_force_pfc_xon_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t *value)
{
    int ioerr = 0;
    CDMAC_PFC_CTRLr_t pfc_ctrl;

    ioerr += READ_CDMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    *value = CDMAC_PFC_CTRLr_FORCE_PFC_XONf_GET(pfc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_rsv_mask_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint32_t rsv_mask)
{
    int ioerr = 0;
    CDMAC_RSV_MASKr_t reg_mask;

    if (rsv_mask > CDMAC_RSV_MASK_ALL) {
        return SHR_E_PARAM;
    }

    CDMAC_RSV_MASKr_CLR(reg_mask);

    ioerr += READ_CDMAC_RSV_MASKr(pa, port, &reg_mask);
    CDMAC_RSV_MASKr_MASKf_SET(reg_mask, rsv_mask);
    ioerr += WRITE_CDMAC_RSV_MASKr(pa, port, reg_mask);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_rsv_mask_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint32_t *rsv_mask)
{
    int ioerr = 0;
    CDMAC_RSV_MASKr_t reg_mask;

    CDMAC_RSV_MASKr_CLR(reg_mask);

    ioerr += READ_CDMAC_RSV_MASKr(pa, port, &reg_mask);
     *rsv_mask = CDMAC_RSV_MASKr_MASKf_GET(reg_mask);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

/*
 * This function controls which RSV(Receive statistics vector) event
 * causes a purge event that triggers RXERR to be set for the packet
 * sent by the MAC to the IP. These bits are used to mask RSV[34:16]
 * for CDMAC; bit[18] of MASK maps to bit[34] of RSV, bit[0] of MASK
 * maps to bit[16] of RSV.
 * Enable : Set 0. Go through
 * Disable: Set 1. Purged.
 * bit[18] --> PFC frame detected
 * bit[17] --> Reserved
 * bit[16] --> Reserved
 * bit[15] --> Unicast detected
 * bit[14] --> VLAN tag detected
 * bit[13] --> Unsupported opcode detected
 * bit[12] --> Pause frame received
 * bit[11] --> Control frame received
 * bit[10] --> Promiscuous packet detected
 * bit[ 9] --> Broadcast detected
 * bit[ 8] --> Multicast detected
 * bit[ 7] --> Receive OK
 * bit[ 6] --> Truncated/Frame out of Range
 * bit[ 5] --> Frame length not out of range, but incorrect -
 *             IEEE length check failed
 * bit[ 4] --> CRC error
 * bit[ 3] --> Receive terminate/code error
 * bit[ 2] --> Unsupported DA for pause/PFC packets detected
 * bit[ 1] --> Stack VLAN detected
 * bit[ 0] --> Wrong SA
 */
static inline int
cdmac_rsv_selective_mask_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t flags, uint32_t value)
{

    int i = 0;
    int ioerr = 0;
    uint32_t tmp_mask = CDMAC_RSV_MASK_MIN;
    uint32_t rsv_mask = 0;

    if (flags > CDMAC_RSV_MASK_ALL) {
        return SHR_E_PARAM;
    }

    ioerr += cdmac_rsv_mask_get(pa, port, &rsv_mask);

    while(tmp_mask <= CDMAC_RSV_MASK_MAX) {
        if (flags & tmp_mask) {
            /*
             * if value = 1 means Enable, set the mask to 0.
             * if value = 0 means Purge, set the mask to 1.
             */
            if (value) {
                SHR_BITCLR(&rsv_mask, i);
            } else {
                SHR_BITSET(&rsv_mask, i);
            }
        }
        tmp_mask = (1 << ++i);
    }

    ioerr += cdmac_rsv_mask_set(pa, port, rsv_mask);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_rsv_selective_mask_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             uint32_t flags, uint32_t *value)
{
    int ioerr = 0;
    uint32_t rsv_mask;

    /* Check if only 1 bit is set in flags */
    if ((flags > CDMAC_RSV_MASK_MAX) || ((flags) & (flags - 1))) {
        return SHR_E_PARAM;
    }

    ioerr += cdmac_rsv_mask_get(pa, port, &rsv_mask);

    /*
     * if bit in rsv_mask = 0 means Enable, return 1.
     * if bit in rsv_mask = 1 means Purge, return 0.
     */
    *value = (rsv_mask & flags)? 0: 1;

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_strip_crc_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t *enable)
{
    int ioerr = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *enable = CDMAC_RX_CTRLr_STRIP_CRCf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_strip_crc_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t enable)
{
    int ioerr = 0;
    uint32_t status = 0;
    CDMAC_RX_CTRLr_t rx_ctrl;

    /* If set, CRC is stripped from the received packet */
    ioerr += READ_CDMAC_RX_CTRLr(pa, port, &rx_ctrl);
    status = CDMAC_RX_CTRLr_STRIP_CRCf_GET(rx_ctrl);

    if (status != enable) {
        CDMAC_RX_CTRLr_STRIP_CRCf_SET(rx_ctrl, (enable? 1: 0));
    }
    ioerr += WRITE_CDMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_tx_crc_mode_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t *crc_mode)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr = READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *crc_mode = CDMAC_TX_CTRLr_CRC_MODEf_GET(tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_tx_crc_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t crc_mode)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    /* Validate crc_mode.  */
    if (crc_mode > CDMAC_CRC_MODE_AUTO) {
        return SHR_E_PARAM;
    }

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_CRC_MODEf_SET(tx_ctrl, crc_mode);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_tx_threshold_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t *threshold)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr = READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *threshold = CDMAC_TX_CTRLr_TX_THRESHOLDf_GET(tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
cdmac_tx_threshold_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t threshold)
{
    int ioerr = 0;
    CDMAC_TX_CTRLr_t tx_ctrl;

    CDMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_CDMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CDMAC_TX_CTRLr_TX_THRESHOLDf_SET(tx_ctrl, threshold);
    ioerr += WRITE_CDMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

#endif /* BCMPMAC_CDPORT_H */
