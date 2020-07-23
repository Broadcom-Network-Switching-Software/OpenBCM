/*! \file bcmpmac_clport.h
 *
 * CLPORT driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_CLPORT_H
#define BCMPMAC_CLPORT_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>


/*******************************************************************************
* Local definitions
 */

/*! Drain cell waiting time. */
#define DRAIN_WAIT_MSEC 500

/*! Default IPG value. */
#define CLMAC_AVERAGE_IPG_DEFAULT 12

/*! Minimun Average IPG */
#define CLMAC_AVERAGE_IPG_MIN      8

/*! Maximum Average IPG */
#define CLMAC_AVERAGE_IPG_MAX      64

/*! HIGIG mode IPG value. */
#define CLMAC_AVERAGE_IPG_HIGIG 8

/*! Tx preamble default length. */
#define CLMAC_TX_PREAMBLE_LENGTH 8

/*! RUNT threshold for Ethernet mode. */
#define CLMAC_RUNT_THRESHOLD_IEEE 64

/*! RUNT threshold for HiGig mode. */
#define CLMAC_RUNT_THRESHOLD_HG 72

/*! RUNT threshold for HiGig2 mode. */
#define CLMAC_RUNT_THRESHOLD_HG2 76

/*! CLMAC hdr mode value per encap mode. */
typedef enum clmac_hdr_mode_e {

    /*! IEEE Ethernet format. */
    CLMAC_HDR_MODE_IEEE = 0,

    /*! HiGig+ format with 12 Byte header. */
    CLMAC_HDR_MODE_HG = 1,

    /*! HiGig2 format with 16  Byte header. */
    CLMAC_HDR_MODE_HG2 = 2

} clmac_hdr_mode_t;

/*! CLMAC speed mode value per speed value. */
typedef enum clmac_speed_mode_e {

    /*! 1Gbps. */
    CLMAC_SPEED_1G = 2,

    /*! 10Gbps, or other speed values. */
    CLMAC_SPEED_10G_PLUS = 4

} clmac_speed_mode_t;

/*! CLMAC CRC MODE. */
typedef enum clmac_crc_mode_e {

    /*! CRC is appended to the data.. */
    CLMAC_CRC_MODE_APPEND = 0,

    /*! CRC is passed through. */
    CLMAC_CRC_MODE_KEEP = 1,

    /*! CRC is replaced with that computed by the MAC. */
    CLMAC_CRC_MODE_REPLACE = 2,

    /*! The CRC mode is determined by the inputs pins on the port side. */
    CLMAC_CRC_MODE_AUTO = 3,

} clmac_crc_mode_t;

/*! CLPORT port mode value. */
typedef enum clport_port_mode_e {

    /*! Quad Port Mode. Lanes 0..3 individually active on XLGMII. */
    CLPORT_PORT_MODE_QUAD = 0,

    /*! Tri Port Mode XLGMII. Lanes 0, 1, and 2 active. lane 2 is dual. */
    CLPORT_PORT_MODE_TRI_012 = 1,

    /*! Tri Port Mode XLGMII. Lanes 0, 2, and 3 active. lane 0 is dual. */
    CLPORT_PORT_MODE_TRI_023 = 2,

    /*! Dual Port Mode. Each of lanes 0 and 2 are dual on XLGMII. */
    CLPORT_PORT_MODE_DUAL = 3,

    /*! Single Port Mode. One physical port on XLGMII. */
    CLPORT_PORT_MODE_SINGLE = 4

} clport_port_mode_t;


/******************************************************************************
 * Private functions
 */

static inline int
clport_port_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t reset)
{
    int ioerr = 0;
    CLPORT_SOFT_RESETr_t port_reset;

    CLPORT_SOFT_RESETr_CLR(port_reset);
    ioerr += READ_CLPORT_SOFT_RESETr(pa, port, &port_reset);

    switch (port) {
        case 0:
            CLPORT_SOFT_RESETr_PORT0f_SET(port_reset, reset);
            break;
        case 1:
            CLPORT_SOFT_RESETr_PORT1f_SET(port_reset, reset);
            break;
        case 2:
            CLPORT_SOFT_RESETr_PORT2f_SET(port_reset, reset);
            break;
        case 3:
            CLPORT_SOFT_RESETr_PORT3f_SET(port_reset, reset);
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += WRITE_CLPORT_SOFT_RESETr(pa, port, port_reset);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_port_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t *reset)
{
    int ioerr = 0;
    CLPORT_SOFT_RESETr_t port_reset;

    CLPORT_SOFT_RESETr_CLR(port_reset);
    ioerr += READ_CLPORT_SOFT_RESETr(pa, port, &port_reset);

    switch (port) {
        case 0:
            *reset = CLPORT_SOFT_RESETr_PORT0f_GET(port_reset);
            break;
        case 1:
            *reset = CLPORT_SOFT_RESETr_PORT1f_GET(port_reset);
            break;
        case 2:
            *reset = CLPORT_SOFT_RESETr_PORT2f_GET(port_reset);
            break;
        case 3:
            *reset = CLPORT_SOFT_RESETr_PORT3f_GET(port_reset);
            break;
        default:
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_port_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t enable)
{
    int ioerr = 0;
    CLPORT_ENABLE_REGr_t port_enable;

    CLPORT_ENABLE_REGr_CLR(port_enable);
    ioerr += READ_CLPORT_ENABLE_REGr(pa, port, &port_enable);

    switch (port) {
        case 0:
            CLPORT_ENABLE_REGr_PORT0f_SET(port_enable, enable);
            break;
        case 1:
            CLPORT_ENABLE_REGr_PORT1f_SET(port_enable, enable);
            break;
        case 2:
            CLPORT_ENABLE_REGr_PORT2f_SET(port_enable, enable);
            break;
        case 3:
            CLPORT_ENABLE_REGr_PORT3f_SET(port_enable, enable);
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += WRITE_CLPORT_ENABLE_REGr(pa, port, port_enable);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_port_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t *enable)
{
    int ioerr = 0;
    CLPORT_ENABLE_REGr_t port_enable;

    CLPORT_ENABLE_REGr_CLR(port_enable);
    ioerr += READ_CLPORT_ENABLE_REGr(pa, port, &port_enable);

    switch (port) {
        case 0:
            *enable = CLPORT_ENABLE_REGr_PORT0f_GET(port_enable);
            break;
        case 1:
            *enable = CLPORT_ENABLE_REGr_PORT1f_GET(port_enable);
            break;
        case 2:
            *enable = CLPORT_ENABLE_REGr_PORT2f_GET(port_enable);
            break;
        case 3:
            *enable = CLPORT_ENABLE_REGr_PORT3f_GET(port_enable);
            break;
        default:
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t init)
{
    int ioerr = 0;
    CLPORT_FLOW_CONTROL_CONFIGr_t flowctrl;
    CLMAC_RX_CTRLr_t rx_ctrl;
    CLMAC_TX_CTRLr_t tx_ctrl;

    CLPORT_FLOW_CONTROL_CONFIGr_CLR(flowctrl);
    CLMAC_RX_CTRLr_CLR(rx_ctrl);
    CLMAC_TX_CTRLr_CLR(tx_ctrl);

    if (!init) {
        /* Disable the port. */
        ioerr += clport_port_enable_set(pa, port, 0);

        /* Reset the port. */
        ioerr += clport_port_reset_set(pa, port, 1);

        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    /* Bring the port out of reset. */
    ioerr += clport_port_reset_set(pa, port, 0);

    /* Enable the port. */
    ioerr += clport_port_enable_set(pa, port, 1);

    /* Configure MAC Rx. */
    ioerr += READ_CLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    CLMAC_RX_CTRLr_STRIP_CRCf_SET(rx_ctrl, 0);
    ioerr += WRITE_CLMAC_RX_CTRLr(pa, port, rx_ctrl);

    /* Configure MAC Tx. */
    ioerr += READ_CLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CLMAC_TX_CTRLr_CRC_MODEf_SET(tx_ctrl, CLMAC_CRC_MODE_AUTO);
    CLMAC_TX_CTRLr_TX_PREAMBLE_LENGTHf_SET(tx_ctrl, CLMAC_TX_PREAMBLE_LENGTH);
    ioerr += WRITE_CLMAC_TX_CTRLr(pa, port, tx_ctrl);

    /* Enable flow control. */
    ioerr += READ_CLPORT_FLOW_CONTROL_CONFIGr(pa, port, &flowctrl);
    CLPORT_FLOW_CONTROL_CONFIGr_MERGE_MODE_ENf_SET(flowctrl, 1);
    ioerr += WRITE_CLPORT_FLOW_CONTROL_CONFIGr(pa, port, flowctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_mac_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t reset)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    CLMAC_CTRLr_SOFT_RESETf_SET(clmac_ctrl, reset);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_mac_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *reset)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    *reset = CLMAC_CTRLr_SOFT_RESETf_GET(clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_rx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    CLMAC_CTRLr_RX_ENf_SET(clmac_ctrl, enable);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_rx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    *enable = CLMAC_CTRLr_RX_ENf_GET(clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_tx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    CLMAC_CTRLr_TX_ENf_SET(clmac_ctrl, enable);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_tx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    *enable = CLMAC_CTRLr_TX_ENf_GET(clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_speed_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t speed)
{
    int ioerr = 0;
    CLMAC_MODEr_t clmac_mode;
    clmac_speed_mode_t mode;

    CLMAC_MODEr_CLR(clmac_mode);

    switch (speed) {
        case 1000:
            mode = CLMAC_SPEED_1G;
            break;
        default:
            mode = CLMAC_SPEED_10G_PLUS;
            break;
    }

    ioerr += READ_CLMAC_MODEr(pa, port, &clmac_mode);
    CLMAC_MODEr_SPEED_MODEf_SET(clmac_mode, mode);
    ioerr += WRITE_CLMAC_MODEr(pa, port, clmac_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_speed_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t *speed)
{
    int ioerr = 0;
    CLMAC_MODEr_t clmac_mode;
    clmac_speed_mode_t mode;

    CLMAC_MODEr_CLR(clmac_mode);

    ioerr += READ_CLMAC_MODEr(pa, port, &clmac_mode);
    mode = CLMAC_MODEr_SPEED_MODEf_GET(clmac_mode);

    switch (mode) {
        case CLMAC_SPEED_1G:
            *speed = 1000;
            break;
        default:
            *speed = 10000;
            break;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_encap_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t encap)
{
    int ioerr = 0, hdr_mode, runt_threshold, ipg_bytes;
    bool is_hg, is_hg2, strict_preamble, ipg_check_disable;
    CLPORT_CONFIGr_t port_config;
    CLMAC_MODEr_t mac_mode;
    CLMAC_CTRLr_t mac_ctrl;
    CLMAC_TX_CTRLr_t tx_ctrl;
    CLMAC_RX_CTRLr_t rx_ctrl;

    CLPORT_CONFIGr_CLR(port_config);
    CLMAC_MODEr_CLR(mac_mode);
    CLMAC_CTRLr_CLR(mac_ctrl);
    CLMAC_TX_CTRLr_CLR(tx_ctrl);
    CLMAC_RX_CTRLr_CLR(rx_ctrl);

    is_hg = (encap == BCMPMAC_ENCAP_HIGIG);
    is_hg2 = (encap == BCMPMAC_ENCAP_HIGIG2);

    if (is_hg || is_hg2) {
        strict_preamble = 0;
        ipg_check_disable = 1;
        ipg_bytes = CLMAC_AVERAGE_IPG_HIGIG;
    } else {
        strict_preamble = 1;
        ipg_check_disable = 0;
        ipg_bytes = CLMAC_AVERAGE_IPG_DEFAULT;
    }

    switch (encap) {
        case BCMPMAC_ENCAP_IEEE:
            hdr_mode = CLMAC_HDR_MODE_IEEE;
            runt_threshold = CLMAC_RUNT_THRESHOLD_IEEE;
            break;
        case BCMPMAC_ENCAP_HIGIG:
            hdr_mode = CLMAC_HDR_MODE_HG;
            runt_threshold = CLMAC_RUNT_THRESHOLD_HG;
            break;
        case BCMPMAC_ENCAP_HIGIG2:
            hdr_mode = CLMAC_HDR_MODE_HG2;
            runt_threshold = CLMAC_RUNT_THRESHOLD_HG2;
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += READ_CLPORT_CONFIGr(pa, port, &port_config);
    CLPORT_CONFIGr_HIGIG_MODEf_SET(port_config, is_hg);
    CLPORT_CONFIGr_HIGIG2_MODEf_SET(port_config, is_hg2);
    ioerr += WRITE_CLPORT_CONFIGr(pa, port, port_config);

    ioerr += READ_CLMAC_MODEr(pa, port, &mac_mode);
    CLMAC_MODEr_HDR_MODEf_SET(mac_mode, hdr_mode);
    ioerr += WRITE_CLMAC_MODEr(pa, port, mac_mode);

    ioerr += READ_CLMAC_CTRLr(pa, port, &mac_ctrl);
    CLMAC_CTRLr_XGMII_IPG_CHECK_DISABLEf_SET(mac_ctrl, ipg_check_disable);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CLMAC_TX_CTRLr_AVERAGE_IPGf_SET(tx_ctrl, ipg_bytes);
    ioerr += WRITE_CLMAC_TX_CTRLr(pa, port, tx_ctrl);

    ioerr += READ_CLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    CLMAC_RX_CTRLr_STRICT_PREAMBLEf_SET(rx_ctrl, strict_preamble);
    CLMAC_RX_CTRLr_RUNT_THRESHOLDf_SET(rx_ctrl, runt_threshold);
    ioerr += WRITE_CLMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_encap_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t *encap)
{
    int ioerr = 0;
    CLMAC_MODEr_t clmac_mode;
    uint32_t hdr_mode;

    CLMAC_MODEr_CLR(clmac_mode);

    ioerr += READ_CLMAC_MODEr(pa, port, &clmac_mode);
    hdr_mode = CLMAC_MODEr_HDR_MODEf_GET(clmac_mode);

    switch (hdr_mode) {
        case CLMAC_HDR_MODE_IEEE:
            *encap = BCMPMAC_ENCAP_IEEE;
            break;
        case CLMAC_HDR_MODE_HG:
            *encap = BCMPMAC_ENCAP_HIGIG;
            break;
        case CLMAC_HDR_MODE_HG2:
            *encap = BCMPMAC_ENCAP_HIGIG2;
            break;
        default:
            *encap = BCMPMAC_ENCAP_COUNT;
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_lpbk_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t en)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);

    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    CLMAC_CTRLr_LOCAL_LPBKf_SET(clmac_ctrl, en ? 1 : 0);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t *en)
{
    int ioerr = 0;
    CLMAC_CTRLr_t clmac_ctrl;

    CLMAC_CTRLr_CLR(clmac_ctrl);

    ioerr += READ_CLMAC_CTRLr(pa, port, &clmac_ctrl);
    *en = CLMAC_CTRLr_LOCAL_LPBKf_GET(clmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_pause_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    CLMAC_PAUSE_CTRLr_t pause_ctrl;

    CLMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_CLMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    CLMAC_PAUSE_CTRLr_TX_PAUSE_ENf_SET(pause_ctrl, ctrl->tx_enable);
    CLMAC_PAUSE_CTRLr_RX_PAUSE_ENf_SET(pause_ctrl, ctrl->rx_enable);
    ioerr += WRITE_CLMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_pause_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    CLMAC_PAUSE_CTRLr_t pause_ctrl;

    CLMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_CLMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    ctrl->tx_enable = CLMAC_PAUSE_CTRLr_TX_PAUSE_ENf_GET(pause_ctrl);
    ctrl->rx_enable = CLMAC_PAUSE_CTRLr_RX_PAUSE_ENf_GET(pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_pause_addr_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    CLMAC_TX_MAC_SAr_t tx_mac_sa;
    CLMAC_RX_MAC_SAr_t rx_mac_sa;
    uint32_t sa_hi, sa_lo;

    CLMAC_TX_MAC_SAr_CLR(tx_mac_sa);
    CLMAC_RX_MAC_SAr_CLR(rx_mac_sa);

    sa_hi = (mac[5] << 8) | mac[4];
    sa_lo = (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0];

    ioerr += READ_CLMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    CLMAC_TX_MAC_SAr_SA_HIf_SET(tx_mac_sa, sa_hi);
    CLMAC_TX_MAC_SAr_SA_LOf_SET(tx_mac_sa, sa_lo);
    ioerr += WRITE_CLMAC_TX_MAC_SAr(pa, port, tx_mac_sa);

    ioerr += READ_CLMAC_RX_MAC_SAr(pa, port, &rx_mac_sa);
    CLMAC_RX_MAC_SAr_SA_HIf_SET(rx_mac_sa, sa_hi);
    CLMAC_RX_MAC_SAr_SA_LOf_SET(rx_mac_sa, sa_lo);
    ioerr += WRITE_CLMAC_RX_MAC_SAr(pa, port, rx_mac_sa);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_pause_addr_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    CLMAC_TX_MAC_SAr_t tx_mac_sa;
    uint32_t sa_hi, sa_lo;

    CLMAC_TX_MAC_SAr_CLR(tx_mac_sa);


    ioerr += READ_CLMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    sa_hi = CLMAC_TX_MAC_SAr_SA_HIf_GET(tx_mac_sa);
    sa_lo = CLMAC_TX_MAC_SAr_SA_LOf_GET(tx_mac_sa);

    mac[5] = (uint8_t)(sa_hi >> 8);
    mac[4] = (uint8_t)(sa_hi);
    mac[3] = (uint8_t)(sa_lo >> 24);
    mac[2] = (uint8_t)(sa_lo >> 16);
    mac[1] = (uint8_t)(sa_lo >> 8);
    mac[0] = (uint8_t)(sa_lo);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_frame_max_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t size)
{
    int ioerr = 0;
    CLMAC_RX_MAX_SIZEr_t rx_max_size;

    CLMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    

    ioerr += READ_CLMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    CLMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_SET(rx_max_size, size);
    ioerr += WRITE_CLMAC_RX_MAX_SIZEr(pa, port, rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_frame_max_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *size)
{
    int ioerr = 0;
    CLMAC_RX_MAX_SIZEr_t rx_max_size;

    CLMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    ioerr += READ_CLMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    *size = CLMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_GET(rx_max_size);

    

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_fault_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        bcmpmac_fault_status_t *st)
{
    int ioerr = 0;
    CLMAC_RX_LSS_CTRLr_t lss_ctrl;
    CLMAC_RX_LSS_STATUSr_t lss_st;
    CLMAC_CLEAR_RX_LSS_STATUSr_t clr_lss;

    sal_memset(st, 0, sizeof(*st));
    CLMAC_RX_LSS_CTRLr_CLR(lss_ctrl);
    CLMAC_RX_LSS_STATUSr_CLR(lss_st);
    CLMAC_CLEAR_RX_LSS_STATUSr_CLR(clr_lss);

    ioerr += READ_CLMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);
    ioerr += READ_CLMAC_RX_LSS_STATUSr(pa, port, &lss_st);
    ioerr += READ_CLMAC_CLEAR_RX_LSS_STATUSr(pa, port, &clr_lss);

    /* The fault status is vaild when the fault control is enabled. */
    if (CLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->local_fault =
            CLMAC_RX_LSS_STATUSr_LOCAL_FAULT_STATUSf_GET(lss_st);
    }

    if (CLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->remote_fault =
            CLMAC_RX_LSS_STATUSr_REMOTE_FAULT_STATUSf_GET(lss_st);
    }

    /* Clear fault status after read. */
    CLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_LOCAL_FAULT_STATUSf_SET(clr_lss, 0);
    CLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_REMOTE_FAULT_STATUSf_SET(clr_lss, 0);
    ioerr += WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(pa, port, clr_lss);
    CLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_LOCAL_FAULT_STATUSf_SET(clr_lss, 1);
    CLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_REMOTE_FAULT_STATUSf_SET(clr_lss, 1);
    ioerr += WRITE_CLMAC_CLEAR_RX_LSS_STATUSr(pa, port, clr_lss);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     bcmpmac_core_port_mode_t *mode)
{
    int rv, ioerr = 0;
    CLPORT_MODE_REGr_t clport_mode;
    uint32_t fval;
    const bcmpmac_port_mode_map_t port_mode_map[] = {
        { { 0 }, CLPORT_PORT_MODE_QUAD },
        { { 0x1, 0x2, 0x4, 0x8, 0 }, CLPORT_PORT_MODE_QUAD },
        { { 0x3, 0, 0xc, 0 }, CLPORT_PORT_MODE_DUAL },
        { { 0x1, 0x2, 0xc, 0 }, CLPORT_PORT_MODE_TRI_012 },
        { { 0x3, 0x0, 0x4, 0x8, 0 }, CLPORT_PORT_MODE_TRI_023 },
        { { 0xf, 0 }, CLPORT_PORT_MODE_SINGLE }
    };

    CLPORT_MODE_REGr_CLR(clport_mode);

    rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                               mode->lane_map, &fval);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    ioerr += READ_CLPORT_MODE_REGr(pa, port, &clport_mode);
    CLPORT_MODE_REGr_XPORT0_CORE_PORT_MODEf_SET(clport_mode, fval);
    CLPORT_MODE_REGr_XPORT0_PHY_PORT_MODEf_SET(clport_mode, fval);
    ioerr += WRITE_CLPORT_MODE_REGr(pa, port, clport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_tx_flush(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                uint32_t flush)
{
    int ioerr = 0, cnt;
    CLMAC_TX_CTRLr_t tx_ctrl;
    CLMAC_TXFIFO_CELL_CNTr_t cell_cnt;

    CLMAC_TX_CTRLr_CLR(tx_ctrl);
    CLMAC_TXFIFO_CELL_CNTr_CLR(cell_cnt);

    ioerr += READ_CLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CLMAC_TX_CTRLr_DISCARDf_SET(tx_ctrl, flush);
    CLMAC_TX_CTRLr_EP_DISCARDf_SET(tx_ctrl, flush);
    ioerr += WRITE_CLMAC_TX_CTRLr(pa, port, tx_ctrl);

    if (flush == 0) {
        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    cnt = DRAIN_WAIT_MSEC / 10;
    while (--cnt >= 0) {
        ioerr += READ_CLMAC_TXFIFO_CELL_CNTr(pa, port, &cell_cnt);
        if (CLMAC_TXFIFO_CELL_CNTr_CELL_CNTf_GET(cell_cnt) == 0) {
            break;
        }
    }

    if (cnt < 0) {
        return SHR_E_BUSY;
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
clport_failover_status_toggle(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t arg)
{
    int ioerr = 0;
    CLPORT_LAG_FAILOVER_CONFIGr_t cfg;

    CLPORT_LAG_FAILOVER_CONFIGr_CLR(cfg);
    ioerr += READ_CLPORT_LAG_FAILOVER_CONFIGr(pa, port, &cfg);
    CLPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 1);
    ioerr += WRITE_CLPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);
    CLPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 0);
    ioerr += WRITE_CLPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_failover_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *failover_lpbk)
{
    int ioerr = 0;
    CLMAC_LAG_FAILOVER_STATUSr_t st;

    CLMAC_LAG_FAILOVER_STATUSr_CLR(st);
    ioerr += READ_CLMAC_LAG_FAILOVER_STATUSr(pa, port, &st);
    *failover_lpbk = CLMAC_LAG_FAILOVER_STATUSr_LAG_FAILOVER_LOOPBACKf_GET(st);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_failover_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t en)
{
    int ioerr = 0;
    CLMAC_CTRLr_t mac_ctrl;
    CLMAC_RX_LSS_CTRLr_t rx_lss;

    CLMAC_CTRLr_CLR(mac_ctrl);
    CLMAC_RX_LSS_CTRLr_CLR(rx_lss);

    ioerr += READ_CLMAC_CTRLr(pa, port, &mac_ctrl);
    CLMAC_CTRLr_LINK_STATUS_SELECTf_SET(mac_ctrl, en);
    CLMAC_CTRLr_LAG_FAILOVER_ENf_SET(mac_ctrl, en);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CLMAC_RX_LSS_CTRLr(pa, port, &rx_lss);
    CLMAC_RX_LSS_CTRLr_DROP_TX_DATA_ON_LINK_INTERRUPTf_SET(rx_lss, en);
    CLMAC_RX_LSS_CTRLr_RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf_SET(rx_lss, en);
    ioerr += WRITE_CLMAC_RX_LSS_CTRLr(pa, port, rx_lss);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_failover_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t *en)
{
    int ioerr = 0;
    CLMAC_CTRLr_t mac_ctrl;

    CLMAC_CTRLr_CLR(mac_ctrl);
    ioerr += READ_CLMAC_CTRLr(pa, port, &mac_ctrl);
    *en = CLMAC_CTRLr_LINK_STATUS_SELECTf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_failover_lpbk_remove(bcmpmac_access_t *pa, bcmpmac_pport_t port)
{
    int ioerr = 0;
    CLMAC_CTRLr_t mac_ctrl;

    CLMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_CLMAC_CTRLr(pa, port, &mac_ctrl);
    CLMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 1);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_CLMAC_CTRLr(pa, port, &mac_ctrl);
    CLMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 0);
    ioerr += WRITE_CLMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_pfc_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    CLMAC_PFC_CTRLr_t pfc_ctrl;
    CLMAC_PFC_TYPEr_t pfc_type;
    CLMAC_PFC_OPCODEr_t pfc_opcode;
    CLMAC_PFC_DAr_t pfc_da;
    uint32_t da_hi, da_lo;

    ioerr += READ_CLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    CLMAC_PFC_CTRLr_PFC_REFRESH_ENf_SET(pfc_ctrl, (cfg->refresh_timer > 0));
    CLMAC_PFC_CTRLr_PFC_REFRESH_TIMERf_SET(pfc_ctrl, cfg->refresh_timer);
    CLMAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, cfg->stats_enable);
    CLMAC_PFC_CTRLr_PFC_XOFF_TIMERf_SET(pfc_ctrl, cfg->xoff_timer);
    CLMAC_PFC_CTRLr_TX_PFC_ENf_SET(pfc_ctrl, cfg->tx_enable);
    CLMAC_PFC_CTRLr_RX_PFC_ENf_SET(pfc_ctrl, cfg->rx_enable);
    ioerr += WRITE_CLMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    ioerr += READ_CLMAC_PFC_TYPEr(pa, port, &pfc_type);
    CLMAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, cfg->eth_type);
    ioerr += WRITE_CLMAC_PFC_TYPEr(pa, port, pfc_type);

    ioerr += READ_CLMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    CLMAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, cfg->opcode);
    ioerr += WRITE_CLMAC_PFC_OPCODEr(pa, port, pfc_opcode);

    da_hi = (cfg->da[5] << 8) | cfg->da[4];
    da_lo = (cfg->da[3] << 24) | (cfg->da[2] << 16) |
            (cfg->da[1] << 8) | cfg->da[0];
    ioerr += READ_CLMAC_PFC_DAr(pa, port, &pfc_da);
    CLMAC_PFC_DAr_PFC_MACDA_HIf_SET(pfc_da, da_hi);
    CLMAC_PFC_DAr_PFC_MACDA_LOf_SET(pfc_da, da_lo);
    ioerr += WRITE_CLMAC_PFC_DAr(pa, port, pfc_da);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_pfc_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    CLMAC_PFC_CTRLr_t pfc_ctrl;
    CLMAC_PFC_TYPEr_t pfc_type;
    CLMAC_PFC_OPCODEr_t pfc_opcode;
    CLMAC_PFC_DAr_t pfc_da;
    uint32_t da_hi, da_lo;

    ioerr += READ_CLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    cfg->refresh_timer = CLMAC_PFC_CTRLr_PFC_REFRESH_TIMERf_GET(pfc_ctrl);
    cfg->stats_enable = CLMAC_PFC_CTRLr_PFC_STATS_ENf_GET(pfc_ctrl);
    cfg->xoff_timer = CLMAC_PFC_CTRLr_PFC_XOFF_TIMERf_GET(pfc_ctrl);
    cfg->tx_enable = CLMAC_PFC_CTRLr_TX_PFC_ENf_GET(pfc_ctrl);
    cfg->rx_enable = CLMAC_PFC_CTRLr_RX_PFC_ENf_GET(pfc_ctrl);

    ioerr += READ_CLMAC_PFC_TYPEr(pa, port, &pfc_type);
    cfg->eth_type = CLMAC_PFC_TYPEr_PFC_ETH_TYPEf_GET(pfc_type);

    ioerr += READ_CLMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    cfg->opcode = CLMAC_PFC_OPCODEr_PFC_OPCODEf_GET(pfc_opcode);

    ioerr += READ_CLMAC_PFC_DAr(pa, port, &pfc_da);
    da_hi = CLMAC_PFC_DAr_PFC_MACDA_HIf_GET(pfc_da);
    da_lo = CLMAC_PFC_DAr_PFC_MACDA_LOf_GET(pfc_da);
    cfg->da[5] = (uint8_t)(da_hi >> 8);
    cfg->da[4] = (uint8_t)(da_hi);
    cfg->da[3] = (uint8_t)(da_lo >> 24);
    cfg->da[2] = (uint8_t)(da_lo >> 16);
    cfg->da[1] = (uint8_t)(da_lo >> 8);
    cfg->da[0] = (uint8_t)(da_lo);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_op_exec(bcmpmac_access_t *pa, bcmpmac_pport_t port, const char *op,
               uint32_t op_param)
{
    bcmpmac_drv_op_f op_func;
    const bcmpmac_drv_op_hdl_t ops[] = {
        { "port_reset", clport_port_reset_set },
        { "port_enable", clport_port_enable_set },
        { "mac_reset", clport_mac_reset_set },
        { "rx_enable", clport_rx_enable_set },
        { "tx_enable", clport_tx_enable_set },
        { "tx_flush", clport_tx_flush },
        { "speed_set", clport_speed_set },
        { "failover_toggle", clport_failover_status_toggle}
    };

    op_func = bcmpmac_drv_op_func_get(ops, COUNTOF(ops), op);
    if (!op_func) {
        return SHR_E_UNAVAIL;
    }

    return op_func(pa, port, op_param);
}

static inline int
clport_fault_disable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_fault_disable_t *st)
{
    int ioerr = 0;
    CLMAC_RX_LSS_CTRLr_t lss_ctrl;

    sal_memset(st, 0, sizeof(*st));
    CLMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr += READ_CLMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    CLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_SET(lss_ctrl,
                                                st->local_fault_disable);
    CLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_SET(lss_ctrl,
                                                 st->remote_fault_disable);
    ioerr += WRITE_CLMAC_RX_LSS_CTRLr(pa, port, lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_fault_disable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_fault_disable_t *st)
{
    int ioerr = 0;
    CLMAC_RX_LSS_CTRLr_t lss_ctrl;

    sal_memset(st, 0, sizeof(*st));
    CLMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr = READ_CLMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    st->local_fault_disable =
        CLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl);

    st->remote_fault_disable =
        CLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_avg_ipg_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint8_t ipg_size)
{
    int ioerr = 0;
    CLMAC_TX_CTRLr_t tx_ctrl;

    /*
     * Average inter packet gap can be in the range 8 to 64.
     * default is 12.
     */
    if ((ipg_size < CLMAC_AVERAGE_IPG_MIN) ||
        (ipg_size > CLMAC_AVERAGE_IPG_MAX)) {
        return SHR_E_PARAM;
    }

    CLMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_CLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    CLMAC_TX_CTRLr_AVERAGE_IPGf_SET(tx_ctrl, ipg_size);
    ioerr += WRITE_CLMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
clport_avg_ipg_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint8_t *ipg_size)
{
    int ioerr = 0;
    CLMAC_TX_CTRLr_t tx_ctrl;

    CLMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr = READ_CLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *ipg_size = CLMAC_TX_CTRLr_AVERAGE_IPGf_GET(tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

#endif /* BCMPMAC_CLPORT_H */

