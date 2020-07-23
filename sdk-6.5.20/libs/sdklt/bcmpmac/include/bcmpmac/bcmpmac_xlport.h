/*! \file bcmpmac_xlport.h
 *
 * XLPORT driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPMAC_XLPORT_H
#define BCMPMAC_XLPORT_H

#include <bcmpmac/bcmpmac_drv.h>
#include <bcmpmac/bcmpmac_util_internal.h>
#include <bcmpmac/chip/bcmpmac_common_xlmac_defs.h>
#include <shr/shr_util.h>
#include <shr/shr_bitop.h>


/*******************************************************************************
* Local definitions
 */

/*! Drain cell waiting time. */
#define DRAIN_WAIT_MSEC 500

/*! Default IPG value. */
#define XLMAC_AVERAGE_IPG_DEFAULT 12

/*! Minimun Average IPG */
#define XLMAC_AVERAGE_IPG_MIN      8

/*! Maximum Average IPG */
#define XLMAC_AVERAGE_IPG_MAX      64

/*! HIGIG mode IPG value. */
#define XLMAC_AVERAGE_IPG_HIGIG 8

/*! Tx preamble default length. */
#define XLMAC_TX_PREAMBLE_LENGTH 8

/*! RUNT threshold for Ethernet mode. */
#define XLMAC_RUNT_THRESHOLD_IEEE 64

/*! RUNT threshold for HiGig mode. */
#define XLMAC_RUNT_THRESHOLD_HG 72

/*! RUNT threshold for HiGig2 mode. */
#define XLMAC_RUNT_THRESHOLD_HG2 76

/*! Minimum RUNT threshold value. */
#define XLMAC_RUNT_THRESHOLD_MIN 17

/*! Maximum RUNT threshold value. */
#define XLMAC_RUNT_THRESHOLD_MAX 96

/*! XLMAC hdr mode value per encap mode. */
typedef enum xlmac_hdr_mode_e {

    /*! IEEE Ethernet format. */
    XLMAC_HDR_MODE_IEEE = 0,

    /*! HiGig+ format with 12 Byte header. */
    XLMAC_HDR_MODE_HG = 1,

    /*! HiGig2 format with 16  Byte header. */
    XLMAC_HDR_MODE_HG2 = 2

} xlmac_hdr_mode_t;

/*! XLMAC speed mode value per speed value. */
typedef enum xlmac_speed_mode_e {

    /*! 10Mbps. */
    XLMAC_SPEED_10M = 0,

    /*! 100Mbps. */
    XLMAC_SPEED_100M = 1,

    /*! 1Gbps. */
    XLMAC_SPEED_1G = 2,

    /*! 2.5Gbps. */
    XLMAC_SPEED_2P5G = 3,

    /*! 10Gbps, or other speed values. */
    XLMAC_SPEED_10G_PLUS = 4

} xlmac_speed_mode_t;

/*! XLMAC CRC MODE. */
typedef enum xlmac_crc_mode_e {

    /*! CRC is appended to the data.. */
    XLMAC_CRC_MODE_APPEND = 0,

    /*! CRC is passed through. */
    XLMAC_CRC_MODE_KEEP = 1,

    /*! CRC is replaced with that computed by the MAC. */
    XLMAC_CRC_MODE_REPLACE = 2,

    /*! The CRC mode is determined by the inputs pins on the port side. */
    XLMAC_CRC_MODE_AUTO = 3,

} xlmac_crc_mode_t;

/*! XLPORT port mode value. */
typedef enum xlport_port_mode_e {

    /*! Quad Port Mode. Lanes 0..3 individually active on XLGMII. */
    XLPORT_PORT_MODE_QUAD = 0,

    /*! Tri Port Mode XLGMII. Lanes 0, 1, and 2 active. lane 2 is dual. */
    XLPORT_PORT_MODE_TRI_012 = 1,

    /*! Tri Port Mode XLGMII. Lanes 0, 2, and 3 active. lane 0 is dual. */
    XLPORT_PORT_MODE_TRI_023 = 2,

    /*! Dual Port Mode. Each of lanes 0 and 2 are dual on XLGMII. */
    XLPORT_PORT_MODE_DUAL = 3,

    /*! Single Port Mode. One physical port on XLGMII. */
    XLPORT_PORT_MODE_SINGLE = 4

} xlport_port_mode_t;


/******************************************************************************
 * Private functions
 */

static inline int
xlport_port_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t reset)
{
    int ioerr = 0;
    XLPORT_SOFT_RESETr_t port_reset;

    XLPORT_SOFT_RESETr_CLR(port_reset);
    ioerr += READ_XLPORT_SOFT_RESETr(pa, port, &port_reset);

    switch (port) {
        case 0:
            XLPORT_SOFT_RESETr_PORT0f_SET(port_reset, reset);
            break;
        case 1:
            XLPORT_SOFT_RESETr_PORT1f_SET(port_reset, reset);
            break;
        case 2:
            XLPORT_SOFT_RESETr_PORT2f_SET(port_reset, reset);
            break;
        case 3:
            XLPORT_SOFT_RESETr_PORT3f_SET(port_reset, reset);
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += WRITE_XLPORT_SOFT_RESETr(pa, port, port_reset);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_port_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      uint32_t *reset)
{
    int ioerr = 0;
    XLPORT_SOFT_RESETr_t port_reset;

    XLPORT_SOFT_RESETr_CLR(port_reset);
    ioerr += READ_XLPORT_SOFT_RESETr(pa, port, &port_reset);

    switch (port) {
        case 0:
            *reset = XLPORT_SOFT_RESETr_PORT0f_GET(port_reset);
            break;
        case 1:
            *reset = XLPORT_SOFT_RESETr_PORT1f_GET(port_reset);
            break;
        case 2:
            *reset = XLPORT_SOFT_RESETr_PORT2f_GET(port_reset);
            break;
        case 3:
            *reset = XLPORT_SOFT_RESETr_PORT3f_GET(port_reset);
            break;
        default:
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_port_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t enable)
{
    int ioerr = 0;
    XLPORT_ENABLE_REGr_t port_enable;

    XLPORT_ENABLE_REGr_CLR(port_enable);
    ioerr += READ_XLPORT_ENABLE_REGr(pa, port, &port_enable);

    switch (port) {
        case 0:
            XLPORT_ENABLE_REGr_PORT0f_SET(port_enable, enable);
            break;
        case 1:
            XLPORT_ENABLE_REGr_PORT1f_SET(port_enable, enable);
            break;
        case 2:
            XLPORT_ENABLE_REGr_PORT2f_SET(port_enable, enable);
            break;
        case 3:
            XLPORT_ENABLE_REGr_PORT3f_SET(port_enable, enable);
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += WRITE_XLPORT_ENABLE_REGr(pa, port, port_enable);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_port_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                       uint32_t *enable)
{
    int ioerr = 0;
    XLPORT_ENABLE_REGr_t port_enable;

    XLPORT_ENABLE_REGr_CLR(port_enable);
    ioerr += READ_XLPORT_ENABLE_REGr(pa, port, &port_enable);

    switch (port) {
        case 0:
            *enable = XLPORT_ENABLE_REGr_PORT0f_GET(port_enable);
            break;
        case 1:
            *enable = XLPORT_ENABLE_REGr_PORT1f_GET(port_enable);
            break;
        case 2:
            *enable = XLPORT_ENABLE_REGr_PORT2f_GET(port_enable);
            break;
        case 3:
            *enable = XLPORT_ENABLE_REGr_PORT3f_GET(port_enable);
            break;
        default:
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_port_init(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t init)
{
    int ioerr = 0;
    XLPORT_FLOW_CONTROL_CONFIGr_t flowctrl;
    XLMAC_RX_CTRLr_t rx_ctrl;
    XLMAC_TX_CTRLr_t tx_ctrl;
    XLMAC_PAUSE_CTRLr_t pause_ctrl;
    XLMAC_PFC_CTRLr_t pfc_ctrl;
    XLMAC_PFC_DAr_t pfc_da;
    XLMAC_PFC_OPCODEr_t pfc_opcode;
    XLMAC_PFC_TYPEr_t pfc_type;
    uint32_t da[2];

    XLPORT_FLOW_CONTROL_CONFIGr_CLR(flowctrl);
    XLMAC_RX_CTRLr_CLR(rx_ctrl);
    XLMAC_TX_CTRLr_CLR(tx_ctrl);

    if (!init) {
        /* Disable the port. */
        ioerr += xlport_port_enable_set(pa, port, 0);

        /* Reset the port. */
        ioerr += xlport_port_reset_set(pa, port, 1);

        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    /* Bring the port out of reset. */
    ioerr += xlport_port_reset_set(pa, port, 0);

    /* Enable the port. */
    ioerr += xlport_port_enable_set(pa, port, 1);

    /* Configure MAC Rx. */
    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    XLMAC_RX_CTRLr_STRIP_CRCf_SET(rx_ctrl, 0);
    ioerr += WRITE_XLMAC_RX_CTRLr(pa, port, rx_ctrl);

    /* Configure MAC Tx. */
    ioerr += READ_XLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    XLMAC_TX_CTRLr_CRC_MODEf_SET(tx_ctrl, XLMAC_CRC_MODE_AUTO);
    XLMAC_TX_CTRLr_TX_PREAMBLE_LENGTHf_SET(tx_ctrl, XLMAC_TX_PREAMBLE_LENGTH);
    ioerr += WRITE_XLMAC_TX_CTRLr(pa, port, tx_ctrl);

    /* Enable flow control. */
    ioerr += READ_XLPORT_FLOW_CONTROL_CONFIGr(pa, port, &flowctrl);
    XLPORT_FLOW_CONTROL_CONFIGr_MERGE_MODE_ENf_SET(flowctrl, 1);
    ioerr += WRITE_XLPORT_FLOW_CONTROL_CONFIGr(pa, port, flowctrl);

    /* Disable pause settings. */
    XLMAC_PAUSE_CTRLr_CLR(pause_ctrl);
    ioerr += READ_XLMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    XLMAC_PAUSE_CTRLr_TX_PAUSE_ENf_SET(pause_ctrl, 0);
    XLMAC_PAUSE_CTRLr_RX_PAUSE_ENf_SET(pause_ctrl, 0);
    ioerr += WRITE_XLMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    /* Disable pfc settings. */
    XLMAC_PFC_CTRLr_CLR(pfc_ctrl);
    ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    XLMAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, 1);
    XLMAC_PFC_CTRLr_TX_PFC_ENf_SET(pfc_ctrl, 0);
    XLMAC_PFC_CTRLr_RX_PFC_ENf_SET(pfc_ctrl, 0);
    ioerr += WRITE_XLMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    /* Reset PFC MAC DA. */
    da[1] = 0x0180;
    da[0] = 0xc2000001;
    ioerr += READ_XLMAC_PFC_DAr(pa, port, &pfc_da);
    XLMAC_PFC_DAr_PFC_MACDAf_SET(pfc_da, da);
    ioerr += WRITE_XLMAC_PFC_DAr(pa, port, pfc_da);

    /* Reset PFC OPCODE. */
    ioerr += READ_XLMAC_PFC_TYPEr(pa, port, &pfc_type);
    XLMAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, 0x8808);
    ioerr += WRITE_XLMAC_PFC_TYPEr(pa, port, pfc_type);

    /* Reset PFC ETH TYPE. */
    ioerr += READ_XLMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    XLMAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, 0x101);
    ioerr += WRITE_XLMAC_PFC_OPCODEr(pa, port, pfc_opcode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_mac_reset_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t reset)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    XLMAC_CTRLr_SOFT_RESETf_SET(xlmac_ctrl, reset);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_mac_reset_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *reset)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    *reset = XLMAC_CTRLr_SOFT_RESETf_GET(xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_rx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    XLMAC_CTRLr_RX_ENf_SET(xlmac_ctrl, enable);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_rx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    *enable = XLMAC_CTRLr_RX_ENf_GET(xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_tx_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t enable)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    XLMAC_CTRLr_TX_ENf_SET(xlmac_ctrl, enable);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_tx_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *enable)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    *enable = XLMAC_CTRLr_TX_ENf_GET(xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_speed_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t speed)
{
    int ioerr = 0;
    XLMAC_MODEr_t xlmac_mode;
    xlmac_speed_mode_t mode;

    XLMAC_MODEr_CLR(xlmac_mode);

    switch (speed) {
        case 10:
            mode = XLMAC_SPEED_10M;
            break;
        case 100:
            mode = XLMAC_SPEED_100M;
            break;
        case 1000:
            mode = XLMAC_SPEED_1G;
            break;
        case 2500:
            mode = XLMAC_SPEED_2P5G;
            break;
        default:
            mode = XLMAC_SPEED_10G_PLUS;
            break;
    }

    ioerr += READ_XLMAC_MODEr(pa, port, &xlmac_mode);
    XLMAC_MODEr_SPEED_MODEf_SET(xlmac_mode, mode);
    ioerr += WRITE_XLMAC_MODEr(pa, port, xlmac_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_speed_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 uint32_t *speed)
{
    int ioerr = 0;
    XLMAC_MODEr_t xlmac_mode;
    xlmac_speed_mode_t mode;

    XLMAC_MODEr_CLR(xlmac_mode);

    ioerr += READ_XLMAC_MODEr(pa, port, &xlmac_mode);
    mode = XLMAC_MODEr_SPEED_MODEf_GET(xlmac_mode);

    switch (mode) {
        case XLMAC_SPEED_10M:
            *speed = 10;
            break;
        case XLMAC_SPEED_100M:
            *speed = 100;
            break;
        case XLMAC_SPEED_1G:
            *speed = 1000;
            break;
        case XLMAC_SPEED_2P5G:
            *speed = 2500;
            break;
        default:
            *speed = 10000;
            break;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_encap_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t encap)
{
    int ioerr = 0, hdr_mode, runt_threshold, ipg_bytes;
    bool is_hg, is_hg2, strict_preamble, ipg_check_disable;
    XLPORT_CONFIGr_t port_config;
    XLMAC_MODEr_t mac_mode;
    XLMAC_CTRLr_t mac_ctrl;
    XLMAC_TX_CTRLr_t tx_ctrl;
    XLMAC_RX_CTRLr_t rx_ctrl;

    XLPORT_CONFIGr_CLR(port_config);
    XLMAC_MODEr_CLR(mac_mode);
    XLMAC_CTRLr_CLR(mac_ctrl);
    XLMAC_TX_CTRLr_CLR(tx_ctrl);
    XLMAC_RX_CTRLr_CLR(rx_ctrl);

    is_hg = (encap == BCMPMAC_ENCAP_HIGIG);
    is_hg2 = (encap == BCMPMAC_ENCAP_HIGIG2);

    if (is_hg || is_hg2) {
        strict_preamble = 0;
        ipg_check_disable = 1;
        ipg_bytes = XLMAC_AVERAGE_IPG_HIGIG;
    } else {
        strict_preamble = 1;
        ipg_check_disable = 0;
        ipg_bytes = XLMAC_AVERAGE_IPG_DEFAULT;
    }

    switch (encap) {
        case BCMPMAC_ENCAP_IEEE:
            hdr_mode = XLMAC_HDR_MODE_IEEE;
            runt_threshold = XLMAC_RUNT_THRESHOLD_IEEE;
            break;
        case BCMPMAC_ENCAP_HIGIG:
            hdr_mode = XLMAC_HDR_MODE_HG;
            runt_threshold = XLMAC_RUNT_THRESHOLD_HG;
            break;
        case BCMPMAC_ENCAP_HIGIG2:
            hdr_mode = XLMAC_HDR_MODE_HG2;
            runt_threshold = XLMAC_RUNT_THRESHOLD_HG2;
            break;
        default:
            return SHR_E_PARAM;
    }

    ioerr += READ_XLPORT_CONFIGr(pa, port, &port_config);
    XLPORT_CONFIGr_HIGIG_MODEf_SET(port_config, is_hg);
    XLPORT_CONFIGr_HIGIG2_MODEf_SET(port_config, is_hg2);
    ioerr += WRITE_XLPORT_CONFIGr(pa, port, port_config);

    ioerr += READ_XLMAC_MODEr(pa, port, &mac_mode);
    XLMAC_MODEr_HDR_MODEf_SET(mac_mode, hdr_mode);
    ioerr += WRITE_XLMAC_MODEr(pa, port, mac_mode);

    ioerr += READ_XLMAC_CTRLr(pa, port, &mac_ctrl);
    XLMAC_CTRLr_XGMII_IPG_CHECK_DISABLEf_SET(mac_ctrl, ipg_check_disable);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_XLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    XLMAC_TX_CTRLr_AVERAGE_IPGf_SET(tx_ctrl, ipg_bytes);
    ioerr += WRITE_XLMAC_TX_CTRLr(pa, port, tx_ctrl);

    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    XLMAC_RX_CTRLr_STRICT_PREAMBLEf_SET(rx_ctrl, strict_preamble);
    XLMAC_RX_CTRLr_RUNT_THRESHOLDf_SET(rx_ctrl, runt_threshold);
    ioerr += WRITE_XLMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_encap_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_encap_mode_t *encap)
{
    int ioerr = 0;
    XLMAC_MODEr_t xlmac_mode;
    uint32_t hdr_mode;

    XLMAC_MODEr_CLR(xlmac_mode);

    ioerr += READ_XLMAC_MODEr(pa, port, &xlmac_mode);
    hdr_mode = XLMAC_MODEr_HDR_MODEf_GET(xlmac_mode);

    switch (hdr_mode) {
        case XLMAC_HDR_MODE_IEEE:
            *encap = BCMPMAC_ENCAP_IEEE;
            break;
        case XLMAC_HDR_MODE_HG:
            *encap = BCMPMAC_ENCAP_HIGIG;
            break;
        case XLMAC_HDR_MODE_HG2:
            *encap = BCMPMAC_ENCAP_HIGIG2;
            break;
        default:
            *encap = BCMPMAC_ENCAP_COUNT;
            return SHR_E_PARAM;
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_lpbk_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t en)
{
    int ioerr = 0;
    XLMAC_RX_LSS_CTRLr_t rx_lss_ctrl;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_RX_LSS_CTRLr_CLR(rx_lss_ctrl);
    XLMAC_CTRLr_CLR(xlmac_ctrl);

    ioerr += READ_XLMAC_RX_LSS_CTRLr(pa, port, &rx_lss_ctrl);
    XLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_SET(rx_lss_ctrl, en ? 1 : 0);
    XLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_SET(rx_lss_ctrl, en ? 1 : 0);
    ioerr += WRITE_XLMAC_RX_LSS_CTRLr(pa, port, rx_lss_ctrl);

    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    XLMAC_CTRLr_LOCAL_LPBKf_SET(xlmac_ctrl, en ? 1 : 0);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port, uint32_t *en)
{
    int ioerr = 0;
    XLMAC_CTRLr_t xlmac_ctrl;

    XLMAC_CTRLr_CLR(xlmac_ctrl);

    ioerr += READ_XLMAC_CTRLr(pa, port, &xlmac_ctrl);
    *en = XLMAC_CTRLr_LOCAL_LPBKf_GET(xlmac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_pause_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    XLMAC_PAUSE_CTRLr_t pause_ctrl;

    XLMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_XLMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    XLMAC_PAUSE_CTRLr_TX_PAUSE_ENf_SET(pause_ctrl, ctrl->tx_enable);
    XLMAC_PAUSE_CTRLr_RX_PAUSE_ENf_SET(pause_ctrl, ctrl->rx_enable);
    ioerr += WRITE_XLMAC_PAUSE_CTRLr(pa, port, pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_pause_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                 bcmpmac_pause_ctrl_t *ctrl)
{
    int ioerr = 0;
    XLMAC_PAUSE_CTRLr_t pause_ctrl;

    XLMAC_PAUSE_CTRLr_CLR(pause_ctrl);

    ioerr += READ_XLMAC_PAUSE_CTRLr(pa, port, &pause_ctrl);
    ctrl->tx_enable = XLMAC_PAUSE_CTRLr_TX_PAUSE_ENf_GET(pause_ctrl);
    ctrl->rx_enable = XLMAC_PAUSE_CTRLr_RX_PAUSE_ENf_GET(pause_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_pause_addr_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    XLMAC_TX_MAC_SAr_t tx_mac_sa;
    XLMAC_RX_MAC_SAr_t rx_mac_sa;
    uint32_t sa_hi, sa_lo;

    XLMAC_TX_MAC_SAr_CLR(tx_mac_sa);
    XLMAC_RX_MAC_SAr_CLR(rx_mac_sa);

    sa_hi = (mac[5] << 8) | mac[4];
    sa_lo = (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0];

    ioerr += READ_XLMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    XLMAC_TX_MAC_SAr_SA_HIf_SET(tx_mac_sa, sa_hi);
    XLMAC_TX_MAC_SAr_SA_LOf_SET(tx_mac_sa, sa_lo);
    ioerr += WRITE_XLMAC_TX_MAC_SAr(pa, port, tx_mac_sa);

    ioerr += READ_XLMAC_RX_MAC_SAr(pa, port, &rx_mac_sa);
    XLMAC_RX_MAC_SAr_SA_HIf_SET(rx_mac_sa, sa_hi);
    XLMAC_RX_MAC_SAr_SA_LOf_SET(rx_mac_sa, sa_lo);
    ioerr += WRITE_XLMAC_RX_MAC_SAr(pa, port, rx_mac_sa);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_pause_addr_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                      bcmpmac_mac_t mac)
{
    int ioerr = 0;
    XLMAC_TX_MAC_SAr_t tx_mac_sa;
    uint32_t sa_hi, sa_lo;

    XLMAC_TX_MAC_SAr_CLR(tx_mac_sa);

    ioerr += READ_XLMAC_TX_MAC_SAr(pa, port, &tx_mac_sa);
    sa_hi = XLMAC_TX_MAC_SAr_SA_HIf_GET(tx_mac_sa);
    sa_lo = XLMAC_TX_MAC_SAr_SA_LOf_GET(tx_mac_sa);

    mac[5] = (uint8_t)(sa_hi >> 8);
    mac[4] = (uint8_t)(sa_hi);
    mac[3] = (uint8_t)(sa_lo >> 24);
    mac[2] = (uint8_t)(sa_lo >> 16);
    mac[1] = (uint8_t)(sa_lo >> 8);
    mac[0] = (uint8_t)(sa_lo);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_frame_max_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t size)
{
    int ioerr = 0;
    XLMAC_RX_MAX_SIZEr_t rx_max_size;

    XLMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    

    ioerr += READ_XLMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    XLMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_SET(rx_max_size, size);
    ioerr += WRITE_XLMAC_RX_MAX_SIZEr(pa, port, rx_max_size);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_frame_max_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t *size)
{
    int ioerr = 0;
    XLMAC_RX_MAX_SIZEr_t rx_max_size;

    XLMAC_RX_MAX_SIZEr_CLR(rx_max_size);

    ioerr += READ_XLMAC_RX_MAX_SIZEr(pa, port, &rx_max_size);
    *size = XLMAC_RX_MAX_SIZEr_RX_MAX_SIZEf_GET(rx_max_size);

    

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_fault_status_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        bcmpmac_fault_status_t *st)
{
    int ioerr = 0;
    XLMAC_RX_LSS_CTRLr_t lss_ctrl;
    XLMAC_RX_LSS_STATUSr_t lss_st;
    XLMAC_CLEAR_RX_LSS_STATUSr_t clr_lss;

    sal_memset(st, 0, sizeof(*st));
    XLMAC_RX_LSS_CTRLr_CLR(lss_ctrl);
    XLMAC_RX_LSS_STATUSr_CLR(lss_st);
    XLMAC_CLEAR_RX_LSS_STATUSr_CLR(clr_lss);

    /* Clear fault status before read. */
    ioerr += READ_XLMAC_CLEAR_RX_LSS_STATUSr(pa, port, &clr_lss);
    XLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_LOCAL_FAULT_STATUSf_SET(clr_lss, 0);
    XLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_REMOTE_FAULT_STATUSf_SET(clr_lss, 0);
    ioerr += WRITE_XLMAC_CLEAR_RX_LSS_STATUSr(pa, port, clr_lss);
    XLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_LOCAL_FAULT_STATUSf_SET(clr_lss, 1);
    XLMAC_CLEAR_RX_LSS_STATUSr_CLEAR_REMOTE_FAULT_STATUSf_SET(clr_lss, 1);
    ioerr += WRITE_XLMAC_CLEAR_RX_LSS_STATUSr(pa, port, clr_lss);

    ioerr += READ_XLMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);
    ioerr += READ_XLMAC_RX_LSS_STATUSr(pa, port, &lss_st);

    /* The fault status is vaild when the fault control is enabled. */
    if (XLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->local_fault =
            XLMAC_RX_LSS_STATUSr_LOCAL_FAULT_STATUSf_GET(lss_st);
    }

    if (XLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl) == 0) {
        st->remote_fault =
            XLMAC_RX_LSS_STATUSr_REMOTE_FAULT_STATUSf_GET(lss_st);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_core_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     bcmpmac_core_port_mode_t *mode)
{
    int rv, ioerr = 0;
    uint32_t fval;
    XLPORT_MODE_REGr_t xlport_mode;
    const bcmpmac_port_mode_map_t port_mode_map[] = {
        { { 0 }, XLPORT_PORT_MODE_QUAD },
        { { 0x1, 0x2, 0x4, 0x8, 0 }, XLPORT_PORT_MODE_QUAD },
        { { 0x3, 0, 0xc, 0 }, XLPORT_PORT_MODE_DUAL },
        { { 0x1, 0x2, 0xc, 0 }, XLPORT_PORT_MODE_TRI_012 },
        { { 0x3, 0x0, 0x4, 0x8, 0 }, XLPORT_PORT_MODE_TRI_023 },
        { { 0xf, 0 }, XLPORT_PORT_MODE_SINGLE }
    };

    XLPORT_MODE_REGr_CLR(xlport_mode);

    rv = bcmpmac_port_mode_get(port_mode_map, COUNTOF(port_mode_map),
                               mode->lane_map, &fval);
    if (SHR_FAILURE(rv)) {
        return rv;
    }

    ioerr += READ_XLPORT_MODE_REGr(pa, port, &xlport_mode);
    XLPORT_MODE_REGr_XPORT0_CORE_PORT_MODEf_SET(xlport_mode, fval);
    XLPORT_MODE_REGr_XPORT0_PHY_PORT_MODEf_SET(xlport_mode, fval);
    ioerr += WRITE_XLPORT_MODE_REGr(pa, port, xlport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_tx_flush(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                uint32_t flush)
{
    int ioerr = 0, cnt;
    XLMAC_TX_CTRLr_t tx_ctrl;
    XLMAC_TXFIFO_CELL_CNTr_t cell_cnt;

    XLMAC_TX_CTRLr_CLR(tx_ctrl);
    XLMAC_TXFIFO_CELL_CNTr_CLR(cell_cnt);

    ioerr += READ_XLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    XLMAC_TX_CTRLr_DISCARDf_SET(tx_ctrl, flush);
    XLMAC_TX_CTRLr_EP_DISCARDf_SET(tx_ctrl, flush);
    ioerr += WRITE_XLMAC_TX_CTRLr(pa, port, tx_ctrl);

    if (flush == 0) {
        return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
    }

    cnt = DRAIN_WAIT_MSEC / 10;
    while (--cnt >= 0) {
        ioerr += READ_XLMAC_TXFIFO_CELL_CNTr(pa, port, &cell_cnt);
        if (XLMAC_TXFIFO_CELL_CNTr_CELL_CNTf_GET(cell_cnt) == 0) {
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
xlport_failover_status_toggle(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t arg)
{
    int ioerr = 0;
    XLPORT_LAG_FAILOVER_CONFIGr_t cfg;

    XLPORT_LAG_FAILOVER_CONFIGr_CLR(cfg);
    ioerr += READ_XLPORT_LAG_FAILOVER_CONFIGr(pa, port, &cfg);
    XLPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 1);
    ioerr += WRITE_XLPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);
    XLPORT_LAG_FAILOVER_CONFIGr_LINK_STATUS_UPf_SET(cfg, 0);
    ioerr += WRITE_XLPORT_LAG_FAILOVER_CONFIGr(pa, port, cfg);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_failover_lpbk_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *failover_lpbk)
{
    int ioerr = 0;
    XLMAC_LAG_FAILOVER_STATUSr_t st;

    XLMAC_LAG_FAILOVER_STATUSr_CLR(st);
    ioerr += READ_XLMAC_LAG_FAILOVER_STATUSr(pa, port, &st);
    *failover_lpbk = XLMAC_LAG_FAILOVER_STATUSr_LAG_FAILOVER_LOOPBACKf_GET(st);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_failover_enable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t en)
{
    int ioerr = 0;
    XLMAC_CTRLr_t mac_ctrl;
    XLMAC_RX_LSS_CTRLr_t rx_lss;

    XLMAC_CTRLr_CLR(mac_ctrl);
    XLMAC_RX_LSS_CTRLr_CLR(rx_lss);

    ioerr += READ_XLMAC_CTRLr(pa, port, &mac_ctrl);
    XLMAC_CTRLr_LINK_STATUS_SELECTf_SET(mac_ctrl, en);
    XLMAC_CTRLr_LAG_FAILOVER_ENf_SET(mac_ctrl, en);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_XLMAC_RX_LSS_CTRLr(pa, port, &rx_lss);
    XLMAC_RX_LSS_CTRLr_RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf_SET(rx_lss, en);
    ioerr += WRITE_XLMAC_RX_LSS_CTRLr(pa, port, rx_lss);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_failover_enable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                           uint32_t *en)
{
    int ioerr = 0;
    XLMAC_CTRLr_t mac_ctrl;

    XLMAC_CTRLr_CLR(mac_ctrl);
    ioerr += READ_XLMAC_CTRLr(pa, port, &mac_ctrl);
    *en = XLMAC_CTRLr_LINK_STATUS_SELECTf_GET(mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_failover_lpbk_remove(bcmpmac_access_t *pa, bcmpmac_pport_t port)
{
    int ioerr = 0;
    XLMAC_CTRLr_t mac_ctrl;

    XLMAC_CTRLr_CLR(mac_ctrl);

    ioerr += READ_XLMAC_CTRLr(pa, port, &mac_ctrl);
    XLMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 1);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, mac_ctrl);

    ioerr += READ_XLMAC_CTRLr(pa, port, &mac_ctrl);
    XLMAC_CTRLr_REMOVE_FAILOVER_LPBKf_SET(mac_ctrl, 0);
    ioerr += WRITE_XLMAC_CTRLr(pa, port, mac_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_pfc_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    XLMAC_PFC_CTRLr_t pfc_ctrl;
    XLMAC_PFC_TYPEr_t pfc_type;
    XLMAC_PFC_OPCODEr_t pfc_opcode;
    XLMAC_PFC_DAr_t pfc_da;
    uint32_t da_hi, da_lo;

    ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    XLMAC_PFC_CTRLr_PFC_REFRESH_ENf_SET(pfc_ctrl, (cfg->refresh_timer > 0));
    XLMAC_PFC_CTRLr_PFC_REFRESH_TIMERf_SET(pfc_ctrl, cfg->refresh_timer);
    XLMAC_PFC_CTRLr_PFC_STATS_ENf_SET(pfc_ctrl, cfg->stats_enable);
    XLMAC_PFC_CTRLr_PFC_XOFF_TIMERf_SET(pfc_ctrl, cfg->xoff_timer);
    XLMAC_PFC_CTRLr_TX_PFC_ENf_SET(pfc_ctrl, cfg->tx_enable);
    XLMAC_PFC_CTRLr_RX_PFC_ENf_SET(pfc_ctrl, cfg->rx_enable);
    XLMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, cfg->xon_enable);
    ioerr += WRITE_XLMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    /*
     * If pfc is disabled on RX, toggle FORCE_XON.
     * This forces the MAC to generate an XON indication to
     * the MMU for all classes of service in the receive direction.
     * Do not check the previous pfx rx enable state.
     * This is not needed when pfc is enabled on RX.
     */
    if (!cfg->rx_enable) {
        ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
        XLMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, 1);
        ioerr += WRITE_XLMAC_PFC_CTRLr(pa, port, pfc_ctrl);

        ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
        XLMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, 0);
        ioerr += WRITE_XLMAC_PFC_CTRLr(pa, port, pfc_ctrl);
    }
    ioerr += READ_XLMAC_PFC_TYPEr(pa, port, &pfc_type);
    XLMAC_PFC_TYPEr_PFC_ETH_TYPEf_SET(pfc_type, cfg->eth_type);
    ioerr += WRITE_XLMAC_PFC_TYPEr(pa, port, pfc_type);

    ioerr += READ_XLMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    XLMAC_PFC_OPCODEr_PFC_OPCODEf_SET(pfc_opcode, cfg->opcode);
    ioerr += WRITE_XLMAC_PFC_OPCODEr(pa, port, pfc_opcode);

    da_hi = (cfg->da[5] << 8) | cfg->da[4];
    da_lo = (cfg->da[3] << 24) | (cfg->da[2] << 16) |
            (cfg->da[1] << 8) | cfg->da[0];
    ioerr += READ_XLMAC_PFC_DAr(pa, port, &pfc_da);
    XLMAC_PFC_DAr_PFC_MACDA_HIf_SET(pfc_da, da_hi);
    XLMAC_PFC_DAr_PFC_MACDA_LOf_SET(pfc_da, da_lo);
    ioerr += WRITE_XLMAC_PFC_DAr(pa, port, pfc_da);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_pfc_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
               bcmpmac_pfc_t *cfg)
{
    int ioerr = 0;
    XLMAC_PFC_CTRLr_t pfc_ctrl;
    XLMAC_PFC_TYPEr_t pfc_type;
    XLMAC_PFC_OPCODEr_t pfc_opcode;
    XLMAC_PFC_DAr_t pfc_da;
    uint32_t da_hi, da_lo;

    ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    cfg->refresh_timer = XLMAC_PFC_CTRLr_PFC_REFRESH_TIMERf_GET(pfc_ctrl);
    cfg->stats_enable = XLMAC_PFC_CTRLr_PFC_STATS_ENf_GET(pfc_ctrl);
    cfg->xoff_timer = XLMAC_PFC_CTRLr_PFC_XOFF_TIMERf_GET(pfc_ctrl);
    cfg->tx_enable = XLMAC_PFC_CTRLr_TX_PFC_ENf_GET(pfc_ctrl);
    cfg->rx_enable = XLMAC_PFC_CTRLr_RX_PFC_ENf_GET(pfc_ctrl);
    cfg->xon_enable = XLMAC_PFC_CTRLr_FORCE_PFC_XONf_GET(pfc_ctrl);

    ioerr += READ_XLMAC_PFC_TYPEr(pa, port, &pfc_type);
    cfg->eth_type = XLMAC_PFC_TYPEr_PFC_ETH_TYPEf_GET(pfc_type);

    ioerr += READ_XLMAC_PFC_OPCODEr(pa, port, &pfc_opcode);
    cfg->opcode = XLMAC_PFC_OPCODEr_PFC_OPCODEf_GET(pfc_opcode);

    ioerr += READ_XLMAC_PFC_DAr(pa, port, &pfc_da);
    da_hi = XLMAC_PFC_DAr_PFC_MACDA_HIf_GET(pfc_da);
    da_lo = XLMAC_PFC_DAr_PFC_MACDA_LOf_GET(pfc_da);
    cfg->da[5] = (uint8_t)(da_hi >> 8);
    cfg->da[4] = (uint8_t)(da_hi);
    cfg->da[3] = (uint8_t)(da_lo >> 24);
    cfg->da[2] = (uint8_t)(da_lo >> 16);
    cfg->da[1] = (uint8_t)(da_lo >> 8);
    cfg->da[0] = (uint8_t)(da_lo);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlmac_force_pfc_xon_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t value)
{
    int ioerr = 0;
    XLMAC_PFC_CTRLr_t pfc_ctrl;

    ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    XLMAC_PFC_CTRLr_FORCE_PFC_XONf_SET(pfc_ctrl, value);
    ioerr += WRITE_XLMAC_PFC_CTRLr(pa, port, pfc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlmac_force_pfc_xon_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t *value)
{
    int ioerr = 0;
    XLMAC_PFC_CTRLr_t pfc_ctrl;

    ioerr += READ_XLMAC_PFC_CTRLr(pa, port, &pfc_ctrl);
    *value = XLMAC_PFC_CTRLr_FORCE_PFC_XONf_GET(pfc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_op_exec(bcmpmac_access_t *pa, bcmpmac_pport_t port, const char *op,
               uint32_t op_param)
{
    bcmpmac_drv_op_f op_func;
    const bcmpmac_drv_op_hdl_t ops[] = {
        { "port_reset", xlport_port_reset_set },
        { "port_enable", xlport_port_enable_set },
        { "mac_reset", xlport_mac_reset_set },
        { "rx_enable", xlport_rx_enable_set },
        { "tx_enable", xlport_tx_enable_set },
        { "tx_flush", xlport_tx_flush },
        { "speed_set", xlport_speed_set },
        { "failover_toggle", xlport_failover_status_toggle },
        { "force_pfc_xon_set", xlmac_force_pfc_xon_set }
    };

    op_func = bcmpmac_drv_op_func_get(ops, COUNTOF(ops), op);
    if (!op_func) {
        return SHR_E_UNAVAIL;
    }

    return op_func(pa, port, op_param);
}

static inline int
xlport_port_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                     uint32_t flags, uint32_t lane_mask)
{
    int ioerr = 0, num_lanes = 0;
    uint32_t cur_port_mode = XLPORT_PORT_MODE_QUAD;
    uint32_t new_port_mode = XLPORT_PORT_MODE_QUAD;
    XLPORT_MODE_REGr_t xlport_mode;

    XLPORT_MODE_REGr_CLR(xlport_mode);
    num_lanes = shr_util_popcount(lane_mask);
    ioerr += READ_XLPORT_MODE_REGr(pa, port, &xlport_mode);
    cur_port_mode = XLPORT_MODE_REGr_XPORT0_CORE_PORT_MODEf_GET(xlport_mode);

    if (num_lanes == 4) {
        new_port_mode = XLPORT_PORT_MODE_SINGLE;
    } else if (num_lanes == 2) {
        if (cur_port_mode == XLPORT_PORT_MODE_DUAL) {
            new_port_mode = XLPORT_PORT_MODE_DUAL;
        } else if (cur_port_mode == XLPORT_PORT_MODE_QUAD) {
            if (lane_mask & 0x3) {
                new_port_mode = XLPORT_PORT_MODE_TRI_023;
            } else if (lane_mask & 0xc) {
                new_port_mode = XLPORT_PORT_MODE_TRI_012;
            }
        } else if (cur_port_mode == XLPORT_PORT_MODE_TRI_023) {
            if (lane_mask & 0x1) {
                new_port_mode = XLPORT_PORT_MODE_TRI_023;
            } else if (lane_mask & 0x4) {
                new_port_mode = XLPORT_PORT_MODE_DUAL;
            }
        } else if (cur_port_mode == XLPORT_PORT_MODE_TRI_012) {
            if (lane_mask & 0x1) {
                new_port_mode = XLPORT_PORT_MODE_DUAL;
            } else if (lane_mask & 0x4) {
                new_port_mode = XLPORT_PORT_MODE_TRI_012;
            }
        } else if (cur_port_mode == XLPORT_PORT_MODE_SINGLE) {
            if (lane_mask & 0x1) {
                new_port_mode = XLPORT_PORT_MODE_TRI_023;
            } else if (lane_mask & 0x4) {
                new_port_mode = XLPORT_PORT_MODE_TRI_012;
            }
        }
    } else if (num_lanes == 1) {
        if (cur_port_mode == XLPORT_PORT_MODE_QUAD) {
            new_port_mode = XLPORT_PORT_MODE_QUAD;
        } else if (cur_port_mode == XLPORT_PORT_MODE_DUAL) {
            if ((lane_mask & 0x4) || (lane_mask & 0x8)) {
                new_port_mode = XLPORT_PORT_MODE_TRI_023;
            } else if ((lane_mask & 0x1) || (lane_mask & 0x2)) {
                new_port_mode = XLPORT_PORT_MODE_TRI_012;
            }
        } else if (cur_port_mode == XLPORT_PORT_MODE_TRI_012) {
            if ((lane_mask & 0x4) || (lane_mask & 0x8)) {
                new_port_mode = XLPORT_PORT_MODE_QUAD;
            } else if ((lane_mask & 0x1) || (lane_mask & 0x2)) {
                new_port_mode = XLPORT_PORT_MODE_TRI_012;
            }
        } else if (cur_port_mode == XLPORT_PORT_MODE_TRI_023) {
            if ((lane_mask & 0x4) || (lane_mask & 0x8)) {
                new_port_mode = XLPORT_PORT_MODE_TRI_023;
            } else if ((lane_mask & 0x1) || (lane_mask & 0x2)) {
                new_port_mode = XLPORT_PORT_MODE_QUAD;
            }
        } else if (cur_port_mode == XLPORT_PORT_MODE_SINGLE) {
            new_port_mode = XLPORT_PORT_MODE_QUAD;
        }
    }

    XLPORT_MODE_REGr_XPORT0_CORE_PORT_MODEf_SET(xlport_mode, new_port_mode);
    XLPORT_MODE_REGr_XPORT0_PHY_PORT_MODEf_SET(xlport_mode, new_port_mode);
    ioerr += WRITE_XLPORT_MODE_REGr(pa, port, xlport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_fault_disable_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         bcmpmac_fault_disable_t *st)
{
    int ioerr = 0;
    XLMAC_RX_LSS_CTRLr_t lss_ctrl;

    sal_memset(st, 0, sizeof(*st));
    XLMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr += READ_XLMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    XLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_SET(lss_ctrl,
                                                st->local_fault_disable);
    XLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_SET(lss_ctrl,
                                                 st->remote_fault_disable);
    ioerr += WRITE_XLMAC_RX_LSS_CTRLr(pa, port, lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_fault_disable_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        bcmpmac_fault_disable_t *st)
{
    int ioerr = 0;
    XLMAC_RX_LSS_CTRLr_t lss_ctrl;

    sal_memset(st, 0, sizeof(*st));
    XLMAC_RX_LSS_CTRLr_CLR(lss_ctrl);

    ioerr = READ_XLMAC_RX_LSS_CTRLr(pa, port, &lss_ctrl);

    st->local_fault_disable =
        XLMAC_RX_LSS_CTRLr_LOCAL_FAULT_DISABLEf_GET(lss_ctrl);

    st->remote_fault_disable =
        XLMAC_RX_LSS_CTRLr_REMOTE_FAULT_DISABLEf_GET(lss_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_avg_ipg_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint8_t ipg_size)
{
    int ioerr = 0;
    XLMAC_TX_CTRLr_t tx_ctrl;

    /*
     * Average inter packet gap can be in the range 8 to 64.
     * default is 12.
     */
    if ((ipg_size < XLMAC_AVERAGE_IPG_MIN) ||
        (ipg_size > XLMAC_AVERAGE_IPG_MAX)) {
        return SHR_E_PARAM;
    }

    XLMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr += READ_XLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    XLMAC_TX_CTRLr_AVERAGE_IPGf_SET(tx_ctrl, ipg_size);
    ioerr += WRITE_XLMAC_TX_CTRLr(pa, port, tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_avg_ipg_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                   uint8_t *ipg_size)
{
    int ioerr = 0;
    XLMAC_TX_CTRLr_t tx_ctrl;

    XLMAC_TX_CTRLr_CLR(tx_ctrl);

    ioerr = READ_XLMAC_TX_CTRLr(pa, port, &tx_ctrl);
    *ipg_size = XLMAC_TX_CTRLr_AVERAGE_IPGf_GET(tx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_mib_oversize_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t size)
{
    int ioerr = 0;
    XLPORT_CNTMAXSIZEr_t mib_ctrl;

    XLPORT_CNTMAXSIZEr_CLR(mib_ctrl);

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
    ioerr += READ_XLPORT_CNTMAXSIZEr(pa, port, &mib_ctrl);
    XLPORT_CNTMAXSIZEr_CNTMAXSIZEf_SET(mib_ctrl, size);
    ioerr += WRITE_XLPORT_CNTMAXSIZEr(pa, port, mib_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_mib_oversize_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                        uint32_t *size)
{
    int ioerr = 0;
    XLPORT_CNTMAXSIZEr_t mib_ctrl;

    XLPORT_CNTMAXSIZEr_CLR(mib_ctrl);

    ioerr += READ_XLPORT_CNTMAXSIZEr(pa, port, &mib_ctrl);
    *size = XLPORT_CNTMAXSIZEr_CNTMAXSIZEf_GET(mib_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_tsc_ctrl_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    bcmpmac_tsc_ctrl_t *tsc_cfg)
{
    int ioerr = 0;
    XLPORT_XGXS0_CTRL_REGr_t tsc_ctrl;

    XLPORT_XGXS0_CTRL_REGr_CLR(tsc_ctrl);

    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(pa, port, &tsc_ctrl);
    tsc_cfg->tsc_rstb = XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_GET(tsc_ctrl);
    tsc_cfg->tsc_pwrdwn = XLPORT_XGXS0_CTRL_REGr_PWRDWNf_GET(tsc_ctrl);
    tsc_cfg->tsc_iddq = XLPORT_XGXS0_CTRL_REGr_IDDQf_GET(tsc_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_tsc_ctrl_set(bcmpmac_access_t *pa, bcmpmac_pport_t port, int tsc_pwr_on)
{

    int ioerr = 0;
    XLPORT_XGXS0_CTRL_REGr_t tsc_ctrl;

    XLPORT_XGXS0_CTRL_REGr_CLR(tsc_ctrl);

    ioerr += READ_XLPORT_XGXS0_CTRL_REGr(pa, port, &tsc_ctrl);
    if (tsc_pwr_on) {
        XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(tsc_ctrl, 1);
        XLPORT_XGXS0_CTRL_REGr_PWRDWNf_SET(tsc_ctrl, 0);
        XLPORT_XGXS0_CTRL_REGr_IDDQf_SET(tsc_ctrl, 0);
        ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(pa, port, tsc_ctrl);
    } else {
        XLPORT_XGXS0_CTRL_REGr_RSTB_HWf_SET(tsc_ctrl, 0);
        XLPORT_XGXS0_CTRL_REGr_PWRDWNf_SET(tsc_ctrl, 1);
        XLPORT_XGXS0_CTRL_REGr_IDDQf_SET(tsc_ctrl, 1);
        ioerr += WRITE_XLPORT_XGXS0_CTRL_REGr(pa, port, tsc_ctrl);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlmac_runt_threshold_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t value)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    /*
     * The threshold, below which the packets are dropped or
     * marked as runt. Should be programmed >=17 and <= 96 bytes.
     * Refer to the XMLAC specification for the range.
     */
    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);

    if ((value < XLMAC_RUNT_THRESHOLD_MIN) ||
        (value > XLMAC_RUNT_THRESHOLD_MAX)) {
        return SHR_E_PARAM;
    }

    XLMAC_RX_CTRLr_RUNT_THRESHOLDf_SET(rx_ctrl, value);
    ioerr += WRITE_XLMAC_RX_CTRLr(pa, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}


static inline int
xlmac_runt_threshold_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                         uint32_t *value)
{
    int ioerr = 0;
    XLMAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_XLMAC_RX_CTRLr(pa, port, &rx_ctrl);
    *value = XLMAC_RX_CTRLr_RUNT_THRESHOLDf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_rsv_mask_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t rsv_mask)
{
    int ioerr = 0;
    XLPORT_MAC_RSV_MASKr_t reg_mask;

    if (rsv_mask > XLMAC_RSV_MASK_ALL) {
        return SHR_E_PARAM;
    }

    XLPORT_MAC_RSV_MASKr_CLR(reg_mask);

    ioerr += READ_XLPORT_MAC_RSV_MASKr(pa, port, &reg_mask);
    XLPORT_MAC_RSV_MASKr_MASKf_SET(reg_mask, rsv_mask);
    ioerr += WRITE_XLPORT_MAC_RSV_MASKr(pa, port, reg_mask);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_rsv_mask_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                    uint32_t *rsv_mask)
{
    int ioerr = 0;
    XLPORT_MAC_RSV_MASKr_t reg_mask;

    XLPORT_MAC_RSV_MASKr_CLR(reg_mask);

    ioerr += READ_XLPORT_MAC_RSV_MASKr(pa, port, &reg_mask);
     *rsv_mask = XLPORT_MAC_RSV_MASKr_MASKf_GET(reg_mask);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

/*
 * This function controls which RSV(Receive statistics vector) event
 * causes a purge event that triggers RXERR to be set for the packet
 * sent by the MAC to the IP. These bits are used to mask RSV[34:16]
 * for XLMAC; bit[18] of MASK maps to bit[34] of RSV, bit[0] of MASK
 * maps to bit[16] of RSV.
 * Enable : Set 0. Go through
 * Disable: Set 1. Purged.
 * bit[21] --> Nibble dribble error
 * bit[20] --> Good MACSEC frame
 * bit[19] --> SCH CRC error frame
 * bit[18] --> PFC frame detected
 * bit[17] --> Runt frame detected
 * bit[16] --> RX FIFO full.
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
xlport_rsv_selective_mask_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t flags, uint32_t value)
{

    int i = 0;
    int ioerr = 0;
    uint32_t tmp_mask = XLMAC_RSV_MASK_MIN;
    uint32_t rsv_mask = 0;

    if (flags > XLMAC_RSV_MASK_ALL) {
        return SHR_E_PARAM;
    }

    ioerr += xlport_rsv_mask_get(pa, port, &rsv_mask);

    while(tmp_mask <= XLMAC_RSV_MASK_MAX) {
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

    ioerr += xlport_rsv_mask_set(pa, port, rsv_mask);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_rsv_selective_mask_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                              uint32_t flags, uint32_t *value)
{
    int ioerr = 0;
    uint32_t rsv_mask;

    /* Check if only 1 bit is set in flags */
    if ((flags > XLMAC_RSV_MASK_MAX) || ((flags) & (flags - 1))) {
        return SHR_E_PARAM;
    }

    ioerr += xlport_rsv_mask_get(pa, port, &rsv_mask);

    /*
     * if bit in rsv_mask = 0 means Enable, return 1.
     * if bit in rsv_mask = 1 means Purge, return 0.
     */
    *value = (rsv_mask & flags)? 0: 1;

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlmac_tx_timestamp_info_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                            bcmpmac_tx_timestamp_info_t *timestamp_info)
{
    int ioerr = 0;
    bool is_valid = false;
    uint32_t num_entries;
    XLMAC_TX_TIMESTAMP_FIFO_STATUSr_t ts_status;
    XLMAC_TX_TIMESTAMP_FIFO_DATAr_t ts_data;

    XLMAC_TX_TIMESTAMP_FIFO_STATUSr_CLR(ts_status);

    /* Check the TX FIFO status if there are entries in the FIFO. */
    ioerr += READ_XLMAC_TX_TIMESTAMP_FIFO_STATUSr(pa, port, &ts_status);
    num_entries = XLMAC_TX_TIMESTAMP_FIFO_STATUSr_ENTRY_COUNTf_GET(ts_status);

    /* Return on FIFO empty. */
    if (num_entries == 0) {
        return SHR_E_EMPTY;
    }

    XLMAC_TX_TIMESTAMP_FIFO_DATAr_CLR(ts_data);

    /* Get the timestamp information from the FIFO. */
    ioerr += READ_XLMAC_TX_TIMESTAMP_FIFO_DATAr(pa, port, &ts_data);
    is_valid = XLMAC_TX_TIMESTAMP_FIFO_DATAr_TS_ENTRY_VALIDf_GET(ts_data);

    if (is_valid) {
        timestamp_info->timestamp_lo =
                     XLMAC_TX_TIMESTAMP_FIFO_DATAr_TIME_STAMPf_GET(ts_data);
        timestamp_info->sequence_id =
                     XLMAC_TX_TIMESTAMP_FIFO_DATAr_SEQUENCE_IDf_GET(ts_data);
    }

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_egr_timestap_mode_get(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             bcmpmac_egr_timestamp_mode_t *timestamp_mode)
{
    int ioerr = 0;
    uint32_t egr_mode;
    XLPORT_MODE_REGr_t xlport_mode;

    XLPORT_MODE_REGr_CLR(xlport_mode);

    ioerr += READ_XLPORT_MODE_REGr(pa, port, &xlport_mode);
    egr_mode = XLPORT_MODE_REGr_EGR_1588_TIMESTAMPING_MODEf_GET(xlport_mode);
    *timestamp_mode = egr_mode? BCMPMAC_48_BIT_EGRESS_TIMESTAMP:
                                               BCMPMAC_32_BIT_EGRESS_TIMESTAMP;

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

static inline int
xlport_egr_timestap_mode_set(bcmpmac_access_t *pa, bcmpmac_pport_t port,
                             bcmpmac_egr_timestamp_mode_t timestamp_mode)
{
    int ioerr = 0;
    uint32_t egr_mode;
    XLPORT_MODE_REGr_t xlport_mode;

    XLPORT_MODE_REGr_CLR(xlport_mode);

    ioerr += READ_XLPORT_MODE_REGr(pa, port, &xlport_mode);
    egr_mode = (timestamp_mode == BCMPMAC_48_BIT_EGRESS_TIMESTAMP)? 1: 0;

    XLPORT_MODE_REGr_EGR_1588_TIMESTAMPING_MODEf_SET(xlport_mode, egr_mode);
    XLPORT_MODE_REGr_EGR_1588_TIMESTAMPING_CMIC_48_ENf_SET(xlport_mode,
                                                   egr_mode? 1: 0);
    ioerr += WRITE_XLPORT_MODE_REGr(pa, port, xlport_mode);

    return ioerr ? SHR_E_ACCESS : SHR_E_NONE;
}

#endif /* BCMPMAC_XLPORT_H */
