/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:    dcccport.c
 * Purpose: 
 *
 */

/*! \file dcccport.c
 *
 * DC3MAC driver.
 *
 * A DC3PORT contains 1 DC3PORT and 1 DC3MACs, and supports 8 ports at most.
 *
 * In this driver, we always use the port number 0~7 to access the DC3PORT
 * and DC3MAC per-port registers
 *
 */
#define NO_MCM 1
#include <soc/portmod/portmod.h>
#include <soc/portmod/dcccport.h>
#include <soc/drv.h>
#include <soc/ua.h>

#if defined(PORTMOD_PM8X100_GEN2_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

/*******************************************************************************
 * Local definitions
 ******************************************************************************/

/*! Drain cell waiting time. */
#define DRAIN_WAIT_MSEC 500

/*! Number of ports per DC3MAC. */
#define PORTS_PER_DC3MAC 8

/*! Minimum RUNT threshold. */
#define DC3MAC_RUNT_THRESHOLD_MIN      64

/*! Maximum RUNT threshold. */
#define DC3MAC_RUNT_THRESHOLD_MAX      96

/*! Minimun Average IPG */
#define DC3MAC_AVE_IPG_MIN             8

/*! Maximum Average IPG */
#define DC3MAC_AVE_IPG_MAX             60

/* register per logical or physical port number accessing */
#define DC3PORT_REG_READ(unit, reg_name, phy_acc, reg_val) \
         ((phy_acc & SOC_REG_ADDR_PHY_ACC_MASK) ?\
          UA_REG_READ_PHY_PORT(unit, reg_name, phy_acc & (~SOC_REG_ADDR_PHY_ACC_MASK), 0, reg_val) : \
          UA_REG_READ_PORT(unit, reg_name, phy_acc, 0, reg_val))

#define DC3PORT_REG_WRITE(unit, reg_name, phy_acc, reg_val) \
        ((phy_acc & SOC_REG_ADDR_PHY_ACC_MASK) ?\
         UA_REG_WRITE_PHY_PORT(unit, reg_name, phy_acc & (~SOC_REG_ADDR_PHY_ACC_MASK), 0, reg_val) : \
         UA_REG_WRITE_PORT(unit, reg_name, phy_acc, 0, reg_val))

/*! DC3MAC header mode value per encap mode. */
typedef enum dc3mac_hdr_mode_e {

    /*! IEEE Ethernet format (K.SOP + 6Byte preamble + SFD). */
    DC3MAC_HDR_MODE_IEEE = 0,

    /*! /S/ (K.SOP) character in header. */
    DC3MAC_HDR_MODE_KSOP = 5,

    /*! Reduced Preamble mode (K.SOP + 2Byte preamble + SFD). */
    DC3MAC_HDR_MODE_REDUCED_PREAMBLE = 6,

} dc3mac_hdr_mode_t;

/*! DC3PORT port mode value. */
typedef enum dc3port_port_mode_e {

    /*! Quad Port Mode. All four ports are enabled. */
    DC3PORT_PORT_MODE_QUAD = 0,

    /*! Tri Port Mode. Lanes 0, 1, and 2 active. lane 2 is dual. */
    DC3PORT_PORT_MODE_TRI_012 = 1,

    /*! Tri Port Mode. Lanes 0, 2, and 3 active. lane 0 is dual. */
    DC3PORT_PORT_MODE_TRI_023 = 2,

    /*! Dual Port Mode. Each of lanes 0 and 2 are dual. */
    DC3PORT_PORT_MODE_DUAL = 3,

    /*! Single Port Mode. Lanes 0 through 3 are single XLGMII. */
    DC3PORT_PORT_MODE_SINGLE = 4,

    /*! Single 8 lane Port Mode. Lanes 0 through 7 are single XLGMII. */
    DC3PORT_PORT_MODE_SINGLE_8_LANE = 8

} dc3port_port_mode_t;

/*! DC3MAC CRC MODE. */
typedef enum dc3mac_crc_mode_e {

    /*! CRC is computed on incoming packet data and appended. */
    DC3MAC_CRC_MODE_APPEND = 0,

    /*! Incoming pkt CRC is passed through without modifications. */
    DC3MAC_CRC_MODE_KEEP = 1,

    /*! Incoming pkt CRC is replaced with CRC value computed by the MAC. */
    DC3MAC_CRC_MODE_REPLACE = 2,

    /*! The CRC mode is determined by the HW. */
    DC3MAC_CRC_MODE_AUTO = 3,

} dc3mac_crc_mode_t;


/******************************************************************************
 * Private functions
 ******************************************************************************/

int
dc3port_port_reset_set(int unit, soc_port_t port, uint32_t reset)
{
    /* There is no per-port RESET control in DC3PORT. */
    return SOC_E_NONE;
}

int
dc3port_port_reset_get(int unit, soc_port_t port, uint32_t *reset)
{
    /* There is no per-port RESET control in DC3PORT. */
    *reset = 0;
    return SOC_E_NONE;
}

int
dc3port_port_enable_set(int unit, soc_port_t port, uint32_t enable)
{
    /* There is no per-port ENABLE control in DC3PORT. */
    return SOC_E_NONE;
}

int
dc3port_port_enable_get(int unit, soc_port_t port, uint32_t *enable)
{
    /* There is no per-port ENABLE control in DC3PORT. */
    *enable = 1;
    return SOC_E_NONE;
}

int
dc3mac_reset_set(int unit, soc_port_t port, uint32_t reset)
{
    uint32 rval = 0;
    uint32 field_value = reset;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, SOFT_RESET, &field_value);

    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

    return (SOC_E_NONE);
}

int
dc3mac_reset_get(int unit, soc_port_t port, uint32_t *reset)
{
    uint32 rval = 0;
    uint32 field_value = 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_CTRL, &rval, SOFT_RESET, &field_value);
    *reset = field_value;

    return (SOC_E_NONE);
}

int
dc3mac_rx_enable_set(int unit, soc_port_t port, uint32_t enable)
{
    uint32 rval;
    uint32 field_value = enable ? 1 : 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &field_value);

    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));


    return (SOC_E_NONE);
}

int
dc3mac_rx_enable_get(int unit, soc_port_t port, uint32_t *enable)
{
    uint32 rval;
    uint32 field_value = 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_CTRL, &rval, RX_EN, &field_value);
    *enable = field_value;

    return (SOC_E_NONE);
}

int
dc3mac_tx_enable_set(int unit, soc_port_t port, uint32_t enable)
{
    uint32 rval;
    uint32 field_value = enable ? 1 : 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, TX_EN, &field_value);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

    return (SOC_E_NONE);
}

int
dc3mac_tx_enable_get(int unit, soc_port_t port, uint32_t *enable)
{
    uint32 rval;
    uint32 field_value = 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_CTRL, &rval, TX_EN, &field_value);
    *enable = field_value;

    return (SOC_E_NONE);
}

int
dc3mac_encap_set(int unit, soc_port_t port, portmod_encap_t encap)
{
    uint32_t dc3mac_mode;
    uint32_t hdr_mode;

    SOC_INIT_FUNC_DEFS;

    switch (encap) {
        case SOC_ENCAP_IEEE:
            hdr_mode = DC3MAC_HDR_MODE_IEEE;
            break;
        case SOC_ENCAP_HIGIG3:
        case SOC_ENCAP_IEEE_REDUCED_IPG:
            hdr_mode = DC3MAC_HDR_MODE_REDUCED_PREAMBLE;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("illegal encap mode %d"), encap));
            break;
    }

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_MODE, port, &dc3mac_mode));
    UA_REG_FIELD_SET(unit, DC3MAC_MODE, &dc3mac_mode, HDR_MODE, &hdr_mode);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_MODE, port, &dc3mac_mode));

exit:
    SOC_FUNC_RETURN;

}

int
dc3mac_encap_get(int unit, soc_port_t port, portmod_encap_t *encap)
{
    uint32_t dc3mac_mode;
    uint32_t hdr_mode;

    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_MODE, port, &dc3mac_mode));
    UA_REG_FIELD_GET(unit, DC3MAC_MODE, &dc3mac_mode, HDR_MODE, &hdr_mode);

    switch (hdr_mode) {
        case DC3MAC_HDR_MODE_IEEE:
            *encap = SOC_ENCAP_IEEE;
            break;
        /*
         * Both BCMPMAC_ENCAP_HG3 and BCMPMAC_ENCAP_IEEE_REDUCED_IPG
         * set DC3MAC_MODRr.HDR_MODE as REDUCED_PREAMBLE. Return HG3
         * type here, and BCMPC code will determine the corresponding
         * encap type.
         */
        case DC3MAC_HDR_MODE_REDUCED_PREAMBLE:
            *encap = SOC_ENCAP_HIGIG3;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("unknown encap mode %d"), hdr_mode));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int
dc3port_lpbk_set(int unit, soc_port_t port, uint32_t en)
{
    
    return SOC_E_UNAVAIL;
}

int
dc3port_lpbk_get(int unit, soc_port_t port, uint32_t *en)
{
    
    return SOC_E_UNAVAIL;
}

int
dc3mac_pause_set(int unit, soc_port_t port,
                 const portmod_pause_control_t *ctrl)
{
    uint64 rval64;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

    if(ctrl->rx_enable || ctrl->tx_enable) {
       if(ctrl->refresh_timer > 0 ) {
           field_val = (uint32)ctrl->refresh_timer;
           UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, PAUSE_REFRESH_TIMER, &field_val);
           field_val = 1;
           UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, PAUSE_REFRESH_EN, &field_val);
       } else {
           field_val = 0;
           UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, PAUSE_REFRESH_EN, &field_val);
       }
       field_val = (uint32)ctrl->xoff_timer;
       UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, PAUSE_XOFF_TIMER, &field_val);
    }

    field_val = (uint32)ctrl->tx_enable;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, TX_PAUSE_EN, &field_val);
    field_val = (uint32)ctrl->rx_enable;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, RX_PAUSE_EN, &field_val);

    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pause_get(int unit, soc_port_t port,
                 portmod_pause_control_t *ctrl)
{
    uint64 pause_ctrl;
    uint32 refresh_enable;
    uint32 refresh_timer;
    uint32 field_value = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &pause_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, TX_PAUSE_EN, &field_value);
    ctrl->tx_enable = field_value;

    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, RX_PAUSE_EN, &field_value);
    ctrl->rx_enable = field_value;

    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, PAUSE_REFRESH_EN, &field_value);
    refresh_enable = field_value;

    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, PAUSE_REFRESH_TIMER, &field_value);
    refresh_timer = field_value;

    ctrl->refresh_timer = (refresh_enable ? refresh_timer: -1);

    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, PAUSE_XOFF_TIMER, &field_value);
    ctrl->xoff_timer = field_value;

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pause_addr_set(int unit, soc_port_t port,
                      sal_mac_addr_t mac)
{
    uint64 rval64, fval64;
    uint32_t sa[2];

   SOC_INIT_FUNC_DEFS;

    sa[1] = (mac[5] << 8) | mac[4];
    sa[0] = (mac[3] << 24) | (mac[2] << 16) | (mac[1] << 8) | mac[0];

    COMPILER_64_ZERO(fval64);
    COMPILER_64_SET(fval64, sa[1], sa[0]);

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_MAC_SA, port, &rval64));
    UA_REG_FIELD_SET(unit, DC3MAC_TX_MAC_SA, &rval64, CTRL_SA, &fval64);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_MAC_SA, port, &rval64));

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_MAC_SA, port, &rval64));
    UA_REG_FIELD_SET(unit, DC3MAC_RX_MAC_SA, &rval64, RX_SA, &fval64);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_MAC_SA, port, &rval64));


exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pause_addr_get(int unit, soc_port_t port,
                      sal_mac_addr_t mac)
{
    uint64 tx_mac_sa, field64;
    uint32_t sa[2];

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_MAC_SA, port, &tx_mac_sa));
    UA_REG_FIELD_GET(unit, DC3MAC_TX_MAC_SA, &tx_mac_sa, CTRL_SA, &field64);

    sa[1] = COMPILER_64_HI(field64);
    sa[0] = COMPILER_64_LO(field64);

    mac[5] = (uint8_t)(sa[1] >> 8);
    mac[4] = (uint8_t)(sa[1]);
    mac[3] = (uint8_t)(sa[0] >> 24);
    mac[2] = (uint8_t)(sa[0] >> 16);
    mac[1] = (uint8_t)(sa[0] >> 8);
    mac[0] = (uint8_t)(sa[0]);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_frame_max_set(int unit, soc_port_t port,
                     uint32_t size)
{
    uint32 rx_max_size;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_MAX_SIZE, port, &rx_max_size));
    field_value = (uint32)size;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_MAX_SIZE, &rx_max_size, RX_MAX_SIZE, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_MAX_SIZE, port, &rx_max_size));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_frame_max_get(int unit, soc_port_t port,
                     uint32_t *size)
{
    uint32 rx_max_size;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_MAX_SIZE, port, &rx_max_size));
    UA_REG_FIELD_GET(unit, DC3MAC_RX_MAX_SIZE, &rx_max_size, RX_MAX_SIZE, &field_value);
    *size = field_value;

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_remote_fault_status_get(int unit, soc_port_t port,
                               int *status)
{
    uint32 lss_ctrl;
    uint32 lss_st;
    uint32 fault_disable = 0;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    /*
     *  The fault status bit in DC3MAC_RX_LSS_STATUSr is cleared-on-read.
     *  Read it twice to make sure the fault status is set.
     */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &lss_st));
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &lss_ctrl));
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &lss_st));

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &lss_ctrl, REMOTE_FAULT_DISABLE, &fault_disable);

    /* The fault status is vaild when the fault control is enabled. */
    if (fault_disable == 0) {
        UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_STATUS, &lss_st, REMOTE_FAULT_STATUS, &fld_val);
        *status = fld_val;
    }

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_local_fault_status_get(int unit, soc_port_t port,
                              int *status)
{
    uint32 lss_ctrl;
    uint32 lss_st;
    uint32 fault_disable = 0;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    /*
     *  The fault status bit in DC3MAC_RX_LSS_STATUSr is cleared-on-read.
     *  Read it twice to make sure the fault status is set.
     */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &lss_st));
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &lss_ctrl));
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &lss_st));

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &lss_ctrl, LOCAL_FAULT_DISABLE, &fault_disable);

    /* The fault status is vaild when the fault control is enabled. */
    if (fault_disable == 0) {
        UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_STATUS, &lss_st, LOCAL_FAULT_STATUS, &fld_val);
        *status = fld_val;
    }

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pfc_control_set(int unit, soc_port_t port,
                       const portmod_pfc_control_t  *cfg)
{
    uint64 rval64;
    uint32 rval;
    uint32 field_value = 0;

    SOC_INIT_FUNC_DEFS;

   _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

    field_value = (cfg->refresh_timer > 0) ? 1 : 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64,
                          PFC_REFRESH_EN, &field_value);
    field_value = (uint32)cfg->refresh_timer;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64,
                          PFC_REFRESH_TIMER, &field_value);
    field_value = (uint32)cfg->xoff_timer;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64,
                          PFC_XOFF_TIMER, &field_value);

    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &rval));
    field_value = (uint32)cfg->stats_en;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, PFC_STATS_EN,
                      &field_value);
    field_value = (uint32)cfg->force_xon;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, FORCE_PFC_XON,
                      &field_value);
    field_value = (uint32)cfg->tx_enable;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, TX_PFC_EN,
                      &field_value);
    field_value = (uint32)cfg->rx_enable;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, RX_PFC_EN,
                      &field_value);

    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &rval));


    /*
     * If pfc is disabled on RX, toggle FORCE_XON.
     * This forces the MAC to generate an XON indication to
     * the MMU for all classes of service in the receive direction.
     * Do not check the previous pfx rx enable state.
     * This is not needed when pfc is enabled on RX.
     */
    if (!cfg->rx_enable) {
        _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &rval));
        field_value = 1;
        UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, FORCE_PFC_XON, &field_value);
        _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &rval));

        _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &rval));
        field_value = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, FORCE_PFC_XON, &field_value);
        _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &rval));
    }


exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pfc_control_get(int unit, soc_port_t port,
                       portmod_pfc_control_t  *cfg)
{
    uint64_t pause_ctrl;
    uint32_t pfc_ctrl;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &pause_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, PFC_REFRESH_TIMER, &fld_val);
    cfg->refresh_timer = fld_val;

    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, PAUSE_XOFF_TIMER, &fld_val);
    cfg->xoff_timer = fld_val;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &pfc_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, FORCE_PFC_XON, &fld_val);
    cfg->force_xon = fld_val;

    UA_REG_FIELD_GET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, PFC_STATS_EN, &fld_val);
    cfg->stats_en = fld_val;

    UA_REG_FIELD_GET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, TX_PFC_EN, &fld_val);
    cfg->tx_enable = fld_val;

    UA_REG_FIELD_GET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, RX_PFC_EN, &fld_val);
    cfg->rx_enable = fld_val;

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pfc_config_set(int unit, soc_port_t port,
                      const portmod_pfc_config_t *cfg)
{
    uint64 rval64;
    uint32 rval;
    uint32 fval[2] = {0};
    uint64 fval64;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_TYPE_OPCODE, port, &rval));
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_TYPE_OPCODE, &rval, PFC_OPCODE, &cfg->opcode);
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_TYPE_OPCODE, &rval, PFC_ETH_TYPE, &cfg->type);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_TYPE_OPCODE, port, &rval));

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_DA, port, &rval64));
    fval[0] |= cfg->da_nonoui;
    fval[0] |= (cfg->da_oui & 0xff) << 24;
    fval[1] |= (cfg->da_oui & 0xffff00) >> 8;

    COMPILER_64_SET(fval64, fval[1], fval[0]);
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_DA, &rval64, PFC_MACDA, &fval64);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_DA, port, &rval64));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pfc_config_get(int unit, soc_port_t port,
                      portmod_pfc_config_t *cfg)
{
    uint32_t pfc_opcode;
    uint64_t pfc_da;
    uint32 fval[2] = {0};
    uint64 fval64;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_TYPE_OPCODE, port, &pfc_opcode));
    UA_REG_FIELD_GET(unit, DC3MAC_PFC_TYPE_OPCODE, &pfc_opcode, PFC_ETH_TYPE, &cfg->type);
    UA_REG_FIELD_GET(unit, DC3MAC_PFC_TYPE_OPCODE, &pfc_opcode, PFC_OPCODE, &cfg->opcode);

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_DA, port, &pfc_da));
    UA_REG_FIELD_GET(unit, DC3MAC_PFC_DA, &pfc_da, PFC_MACDA, &fval64);
    fval[0] = COMPILER_64_LO(fval64);
    fval[1] = COMPILER_64_HI(fval64);

    cfg->da_nonoui = fval[0] & 0xffffff;
    cfg->da_oui = (fval[1] << 8) | (fval[0] >> 24);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_local_fault_disable_set(int unit, soc_port_t port,
                               const portmod_local_fault_control_t *st)
{
    uint32 rval;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

    field_value = st->enable ? 0: 1;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, LOCAL_FAULT_DISABLE,
                      &field_value); /* flip */

    field_value = st->drop_tx_on_fault? 1: 0;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                      DROP_TX_DATA_ON_LOCAL_FAULT,
                      &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_local_fault_disable_get(int unit, soc_port_t port,
                               portmod_local_fault_control_t *st)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                     LOCAL_FAULT_DISABLE, &fval);
    st->enable = (fval? 0: 1);

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                     DROP_TX_DATA_ON_LOCAL_FAULT, &fval);
    st->drop_tx_on_fault = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_remote_fault_disable_set(int unit, soc_port_t port,
                                const portmod_remote_fault_control_t *st)
{
    uint32 rval;
    uint32 field_value = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

    field_value = st->enable? 0: 1;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, REMOTE_FAULT_DISABLE,
                     &field_value); /* flip */

    field_value = st->drop_tx_on_fault? 1: 0;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                     DROP_TX_DATA_ON_REMOTE_FAULT,
                     &field_value);

    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_remote_fault_disable_get(int unit, soc_port_t port,
                                portmod_remote_fault_control_t *st)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                     REMOTE_FAULT_DISABLE, &fval);
    /* if fval is reset, indicates enable */
    st->enable = (fval? 0: 1); 

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                             DROP_TX_DATA_ON_REMOTE_FAULT, &fval);
    st->drop_tx_on_fault = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_avg_ipg_set(int unit, soc_port_t port,
                   uint8_t ipg_size)
{
    uint32 rval;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    /*
     * Average inter packet gap can be in the range 8 to 56 or 60.
     * should be 56 for XLGMII, 60 for XGMII,
     * default is 12.
     */
    if ((ipg_size < DC3MAC_AVE_IPG_MIN) ||
        (ipg_size > DC3MAC_AVE_IPG_MAX)) {
       _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Average IPG is out of range.")));
    }

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &rval));
    field_value =(uint32)ipg_size;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &rval, AVERAGE_IPG, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_avg_ipg_get(int unit, soc_port_t port,
                   uint8_t *ipg_size)
{
    uint32 rval;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_TX_CTRL, &rval, AVERAGE_IPG, &fld_val);
    *ipg_size = fld_val;

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_INTR_ENABLE, port, &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval, 
                                       TX_PKT_UNDERFLOW, value);
            break;
        case portmodIntrTypeTxPktOverflow :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval, 
                                       TX_PKT_OVERFLOW, value);
            break;
        case portmodIntrTypeTxCdcSingleBitErr :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       TX_CDC_SINGLE_BIT_ERR, value);
            break;
        case portmodIntrTypeTxCdcDoubleBitErr :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       TX_CDC_DOUBLE_BIT_ERR, value);
            break;
        case portmodIntrTypeLocalFaultStatus :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       LOCAL_FAULT_STATUS, value);
            break;
        case portmodIntrTypeRemoteFaultStatus :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       REMOTE_FAULT_STATUS, value);
            break;
        case portmodIntrTypeMibMemSingleBitErr :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       MIB_COUNTER_SINGLE_BIT_ERR, value);
            break;
        case portmodIntrTypeMibMemDoubleBitErr :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       MIB_COUNTER_DOUBLE_BIT_ERR, value);
            break;
        case portmodIntrTypeMibMemMultipleBitErr :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       MIB_COUNTER_MULTIPLE_ERR, value);
            break;
        case portmodIntrTypeRxPfcFifoOverflow :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       RX_PFC_FIFO_OVERFLOW, value);
            break;
        case portmodIntrTypeTxPfcFifoOverflow :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       TX_PFC_FIFO_OVERFLOW, value);
            break;
        case portmodIntrTypeFdrInterrupt :
            UA_REG_FIELD_GET(unit, DC3MAC_INTR_ENABLE, &rval,
                                       FDR_INTERRUPT, value);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid interrupt type")));
            break;
     }

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value)
{
    uint32 rval;
    uint32 field_value =(uint32)value;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_INTR_ENABLE, port, &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              TX_PKT_UNDERFLOW, &field_value);
            break;
        case portmodIntrTypeTxPktOverflow :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              TX_PKT_OVERFLOW, &field_value);
            break;
        case portmodIntrTypeTxCdcSingleBitErr :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              TX_CDC_SINGLE_BIT_ERR, &field_value);
            break;
        case portmodIntrTypeTxCdcDoubleBitErr :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              TX_CDC_DOUBLE_BIT_ERR, &field_value);
            break;
        case portmodIntrTypeLocalFaultStatus :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              LOCAL_FAULT_STATUS, &field_value);
            break;
        case portmodIntrTypeRemoteFaultStatus :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              REMOTE_FAULT_STATUS, &field_value);
            break;
        case portmodIntrTypeMibMemSingleBitErr :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              MIB_COUNTER_SINGLE_BIT_ERR, &field_value);
            break;
        case portmodIntrTypeMibMemDoubleBitErr :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              MIB_COUNTER_DOUBLE_BIT_ERR, &field_value);
            break;
        case portmodIntrTypeMibMemMultipleBitErr :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              MIB_COUNTER_MULTIPLE_ERR, &field_value);
            break;
        case portmodIntrTypeRxPfcFifoOverflow :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              RX_PFC_FIFO_OVERFLOW, &field_value);
            break;
        case portmodIntrTypeTxPfcFifoOverflow :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              TX_PFC_FIFO_OVERFLOW, &field_value);
            break;
        case portmodIntrTypeFdrInterrupt :
            UA_REG_FIELD_SET(unit, DC3MAC_INTR_ENABLE, &rval,
                              FDR_INTERRUPT, &field_value);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid interrupt type")));
            break;
     }

    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_INTR_ENABLE, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FIFO_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval,
                                       TX_PKT_UNDERFLOW, value);
            break;
        case portmodIntrTypeTxPktOverflow :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FIFO_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval,
                                       TX_PKT_OVERFLOW, value);
            break;
        case portmodIntrTypeRxPfcFifoOverflow :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FIFO_STATUS, port, &rval));
            *value = UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval,
                                       RX_PFC_FIFO_OVERFLOW, value);
            break;
        case portmodIntrTypeTxPfcFifoOverflow :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FIFO_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval,
                                       TX_PFC_FIFO_OVERFLOW, value);
            break;
        case portmodIntrTypeFdrInterrupt :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FIFO_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval,
                                       FDR_INTERRUPT, value);
            break;
        case portmodIntrTypeLocalFaultStatus :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_STATUS, &rval,
                                       LOCAL_FAULT_STATUS, value);
            break;
        case portmodIntrTypeRemoteFaultStatus :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_STATUS, &rval,
                                       REMOTE_FAULT_STATUS, value);
            break;
        case portmodIntrTypeTxCdcSingleBitErr :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_ECC_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, TX_CDC_SINGLE_BIT_ERR, value);
            break;
        case portmodIntrTypeTxCdcDoubleBitErr :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_ECC_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, TX_CDC_DOUBLE_BIT_ERR, value);
            break;
        case portmodIntrTypeMibMemSingleBitErr :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_ECC_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, MIB_COUNTER_SINGLE_BIT_ERR, value);
            break;
        case portmodIntrTypeMibMemDoubleBitErr :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_ECC_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, MIB_COUNTER_DOUBLE_BIT_ERR, value);
            break;
        case portmodIntrTypeMibMemMultipleBitErr :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_ECC_STATUS, port, &rval));
            UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, MIB_COUNTER_MULTIPLE_ERR, value);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid interrupt type")));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_interrupts_status_get(int unit, int port, int arr_max_size,
                                 uint32* intr_arr, uint32* size)
{
    uint32 cnt = 0;
    uint32 rval;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    /* Clear on Read */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_INTR_STATUS, port, &rval));

    /* Read FIFO STATUS - Clear on Read Operation */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FIFO_STATUS, port, &rval));

    UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval, TX_PFC_FIFO_OVERFLOW, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPfcFifoOverflow;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval, RX_PFC_FIFO_OVERFLOW, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxPfcFifoOverflow;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval, FDR_INTERRUPT, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeFdrInterrupt;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval, TX_PKT_UNDERFLOW, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktUnderflow;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_FIFO_STATUS, &rval, TX_PKT_OVERFLOW, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktOverflow;
    }

    /* Read RX LSS STATUS register - Clear on Read Operation */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_STATUS, port, &rval));

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_STATUS, &rval, LOCAL_FAULT_STATUS, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLocalFaultStatus;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_STATUS, &rval, LOCAL_FAULT_STATUS, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRemoteFaultStatus;
    }

    /* Read ECC STATUS register - Clear on Read Operation */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_ECC_STATUS, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, TX_CDC_SINGLE_BIT_ERR, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcSingleBitErr;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, TX_CDC_DOUBLE_BIT_ERR, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcDoubleBitErr;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, MIB_COUNTER_SINGLE_BIT_ERR, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibMemSingleBitErr;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, MIB_COUNTER_DOUBLE_BIT_ERR, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibMemDoubleBitErr;
    }

    UA_REG_FIELD_GET(unit, DC3MAC_ECC_STATUS, &rval, MIB_COUNTER_MULTIPLE_ERR, &field_value);
    if (field_value) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibMemMultipleBitErr;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_mib_counter_control_set(int unit, soc_port_t port,
                               int enable, int clear)
{
    uint32 rval;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

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
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));
    field_value=(uint32)enable;
    UA_REG_FIELD_SET(unit, DC3MAC_MIB_COUNTER_CTRL, &rval,
                     ENABLE, &field_value);
    field_value=(uint32)clear;
    UA_REG_FIELD_SET(unit, DC3MAC_MIB_COUNTER_CTRL, &rval, CNT_CLEAR, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));

    /*
     * set CLEARf to 0, this operation is not mandatory, adding to
     * remove ambiguity in interpretation if a read on this register
     */
    if (clear) {
        _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));
        field_value = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_MIB_COUNTER_CTRL, &rval, CNT_CLEAR, &field_value);
        _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));
    }

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_mib_oversize_set(int unit, soc_port_t port,
                        uint32_t size)
{
    uint32 rval;
    uint32 field_value = 0;

    SOC_INIT_FUNC_DEFS;

    /*
     * The maximum packet size that is used in statistics counter updates.
     * default size is 1518. Note if RX_MAX_SIZE(max frame size received)
     * is greater than CNTMAXSIZE, a good packet that is valid CRC and
     * contains no other errors, will increment the OVR(oversize) counters
     * if the length of the packet > CNTMAXSIZE < RX_MAX_SIZE values.
     * Having CNTMAXSIZE > RX_MAX_SIZE is not recommended.
     * This is generally taken care in the statistics module while
     * accumulating the counts.
     */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));
    field_value = size;
    UA_REG_FIELD_SET(unit, DC3MAC_MIB_COUNTER_CTRL, &rval, CNTMAXSIZE, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_mib_oversize_get(int unit, soc_port_t port,
                        uint32_t *size)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_MIB_COUNTER_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_MIB_COUNTER_CTRL, &rval, CNTMAXSIZE, size);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_pass_control_frame_set(int unit, soc_port_t port,
                              uint32_t enable)
{
    
    return  SOC_E_NONE;

   /* int ioerr = 0;
    * DC3MAC_RX_CTRLr_t rx_ctrl;
    */

    /*
     * This configuration is used to drop or pass all control frames
     * (with ether type 0x8808) except pause packets.
     * If set, all control frames are passed to system side.
     * if reset, control frames (including pfc frames wih ether type 0x8808)i
     * are dropped in DC3MAC.
     */
   /*
    ioerr += READ_DC3MAC_RX_CTRLr(unit, port, &rx_ctrl);
    DC3MAC_RX_CTRLr_RX_PASS_CTRLf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_DC3MAC_RX_CTRLr(unit, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SOC_E_NONE;
    */
}


int
dc3mac_pass_control_frame_get(int unit, soc_port_t port,
                              uint32_t *enable)
{
    
    return  SOC_E_NONE;
    /*
    int ioerr = 0;
    DC3MAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_DC3MAC_RX_CTRLr(unit, port, &rx_ctrl);
    *enable = DC3MAC_RX_CTRLr_RX_PASS_CTRLf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SOC_E_NONE;
    */
}


int
dc3mac_pass_pfc_frame_set(int unit, soc_port_t port,
                          uint32_t enable)
{
    
    return  SOC_E_NONE;

    /*
    int ioerr = 0;
    uint32_t status = 0;
    DC3MAC_RX_CTRLr_t rx_ctrl;
    */

    /*
     * This configuration is used to pass or drop PFC packets when
     * PFC_ETH_TYPE is not equal to 0x8808.
     * If set, PFC frames are passed to system side.
     * If reset, PFC frames are dropped in DC3MAC.
     */
    /*
    ioerr += READ_DC3MAC_RX_CTRLr(unit, port, &rx_ctrl);
    status = DC3MAC_RX_CTRLr_RX_PASS_PFCf_GET(rx_ctrl);

    if (status != enable) {
        DC3MAC_RX_CTRLr_RX_PASS_PFCf_SET(rx_ctrl, (enable? 1: 0));
    }
    ioerr += WRITE_DC3MAC_RX_CTRLr(unit, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SOC_E_NONE;
    */
}


int
dc3mac_pass_pfc_frame_get(int unit, soc_port_t port,
                          uint32_t *enable)
{
    
    return  SOC_E_NONE;

    /*
    int ioerr = 0;
    DC3MAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_DC3MAC_RX_CTRLr(unit, port, &rx_ctrl);
    *enable = DC3MAC_RX_CTRLr_RX_PASS_PFCf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SOC_E_NONE;
    */
}


int
dc3mac_pass_pause_frame_set(int unit, soc_port_t port,
                            uint32_t enable)
{
    
    return  SOC_E_NONE;
    /*
    int ioerr = 0;
    DC3MAC_RX_CTRLr_t rx_ctrl;
    */

    /*
     * If set, PAUSE frames are passed to sytem side.
     * If reset, PAUSE frames are dropped in DC3MAC
     */
    /*
    ioerr += READ_DC3MAC_RX_CTRLr(unit, port, &rx_ctrl);
    DC3MAC_RX_CTRLr_RX_PASS_PAUSEf_SET(rx_ctrl, (enable? 1: 0));
    ioerr += WRITE_DC3MAC_RX_CTRLr(unit, port, rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SOC_E_NONE;
    */
}


int
dc3mac_pass_pause_frame_get(int unit, soc_port_t port,
                            uint32_t *enable)
{
    
    return  SOC_E_NONE;

    /*
    int ioerr = 0;
    DC3MAC_RX_CTRLr_t rx_ctrl;

    ioerr += READ_DC3MAC_RX_CTRLr(unit, port, &rx_ctrl);
    *enable = DC3MAC_RX_CTRLr_RX_PASS_PAUSEf_GET(rx_ctrl);

    return ioerr ? SHR_E_ACCESS : SOC_E_NONE;
    */
}

int
dc3mac_discard_set(int unit, soc_port_t port,
                   uint32_t discard)
{
    uint32 rval;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    /* Clear Discard fields */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &rval));
    field_value = discard;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &rval, DISCARD, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_discard_get(int unit, soc_port_t port,
                   uint32_t *discard)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_TX_CTRL, &rval, DISCARD, discard);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_stall_tx_enable_get(int unit, soc_port_t port,
                           int *enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_TX_CTRL, &rval, STALL_TX, enable);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_stall_tx_enable_set(int unit, soc_port_t port,
                           int enable)
{
    uint32 rval;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &rval));
    field_value =(uint32)enable;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &rval, STALL_TX, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_runt_threshold_get(int unit, soc_port_t port,
                          uint32_t *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_RX_CTRL, &rval, RUNT_THRESHOLD, value);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_init(int unit, soc_port_t port,
                  uint32_t init)
{
    uint32 rx_ctrl, tx_ctrl, mac_ctrl, pfc_ctrl, pfc_opcode;
    uint64 pause_ctrl, pfc_da, fval64;
    uint32_t da[2];
    portmod_remote_fault_control_t remote_fault_control;
    portmod_local_fault_control_t local_fault_control;
    uint32 field_value = 0;

    SOC_INIT_FUNC_DEFS;

    if (!init) {
        return SOC_E_NONE;
    }

    /* Set recieve maximum frame size to reset value 1518. */
    _SOC_IF_ERR_EXIT
        (dc3mac_frame_max_set(unit, port, 0x5ee));

    /*
     * Enable and Clear the DC3MAC MIB counters
     * No need to explicitly set the clear flag to 0 since the
     * function internally unsets the clear flag
     */
    _SOC_IF_ERR_EXIT
        (dc3mac_mib_counter_control_set(unit, port, 1, 1));

    /* RX Control */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_CTRL, &rx_ctrl, STRIP_CRC, &field_value);
    
    /*
    DC3MAC_RX_CTRLr_RX_PASS_PAUSEf_SET(rx_ctrl, 0);
    DC3MAC_RX_CTRLr_RX_PASS_PFCf_SET(rx_ctrl, 0);
    */
    field_value = 0x40;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_CTRL, &rx_ctrl, RUNT_THRESHOLD, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));

    /* Configure MAC Tx. */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &tx_ctrl, DISCARD, &field_value);
    
    /*
    DC3MAC_TX_CTRLr_PAD_ENf_SET(tx_ctrl, 0);
    DC3MAC_TX_CTRLr_PAD_THRESHOLDf_SET(tx_ctrl, 64);
    */
    field_value = 12;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &tx_ctrl, AVERAGE_IPG, &field_value);
    field_value = 1;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &tx_ctrl, TX_THRESHOLD, &field_value);
   _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));

    /* Disable local and remote faults. */
    _SOC_IF_ERR_EXIT(
            portmod_remote_fault_control_t_init(unit,&remote_fault_control));
    remote_fault_control.enable = TRUE;
    remote_fault_control.drop_tx_on_fault = TRUE;
    _SOC_IF_ERR_EXIT(dc3mac_remote_fault_disable_set(unit, port,
                                                    &remote_fault_control));

    _SOC_IF_ERR_EXIT(
            portmod_local_fault_control_t_init(unit, &local_fault_control));
    local_fault_control.enable = TRUE;
    local_fault_control.drop_tx_on_fault = TRUE;
    _SOC_IF_ERR_EXIT(dc3mac_local_fault_disable_set(unit, port,
                                                   &local_fault_control));

    /* Disable pause settings. */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &pause_ctrl));
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, TX_PAUSE_EN, &field_value);
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &pause_ctrl, RX_PAUSE_EN, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PAUSE_CTRL, port, &pause_ctrl));

    /* Disable pfc settings. */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &pfc_ctrl));
    field_value = 1;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, PFC_STATS_EN, &field_value);
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, TX_PFC_EN, &field_value);
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, RX_PFC_EN, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &pfc_ctrl));

    /* Reset PFC MAC DA. */
    da[1] = 0x0180;
    da[0] = 0xc2000001;
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_DA, port, &pfc_da));
    COMPILER_64_SET(fval64, da[1], da[0]);
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_DA, &pfc_da, PFC_MACDA, &fval64);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_DA, port, &pfc_da));

    /* Reset PFC OPCODE. */
    /* Reset PFC ETH TYPE. */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_TYPE_OPCODE, port, &pfc_opcode));
    field_value = 0x101;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_TYPE_OPCODE, &pfc_opcode, PFC_OPCODE, &field_value);
    field_value = 0x8808;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_TYPE_OPCODE, &pfc_opcode, PFC_ETH_TYPE, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_TYPE_OPCODE, port, &pfc_opcode));

    /* MAC control settings. */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &mac_ctrl));
    field_value = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, TX_EN, &field_value);
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, RX_EN, &field_value);
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, MAC_LINK_DOWN_SEQ_EN, &field_value);
    field_value = 1;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, SOFT_RESET, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &mac_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_force_pfc_xon_set(int unit, soc_port_t port,
                         uint32_t value)
{
    uint32 pfc_ctrl;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &pfc_ctrl));
    field_value = value;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, FORCE_PFC_XON, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &pfc_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_force_pfc_xon_get(int unit, soc_port_t port,
                         uint32_t *value)
{
    uint32 pfc_ctrl;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &pfc_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_PFC_CTRL, &pfc_ctrl, FORCE_PFC_XON, value);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_rsv_mask_set(int unit, soc_port_t port,
                    uint32_t rsv_mask)
{
    uint32 reg_mask;
    uint32 field_value = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RSV_MASK, port, &reg_mask));
    field_value = rsv_mask;
    UA_REG_FIELD_SET(unit, DC3MAC_RSV_MASK, &reg_mask, MASK, &field_value);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RSV_MASK, port, &rsv_mask));

exit:
   SOC_FUNC_RETURN;
}

int
dc3mac_rsv_mask_get(int unit, soc_port_t port,
                    uint32_t *rsv_mask)
{
    uint32 reg_mask;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RSV_MASK, port, &reg_mask));
    UA_REG_FIELD_GET(unit, DC3MAC_RSV_MASK, &reg_mask, MASK, rsv_mask);

exit:
   SOC_FUNC_RETURN;
}

/*
 * This function controls which RSV(Receive statistics vector) event
 * causes a purge event that triggers RXERR to be set for the packet
 * sent by the MAC to the IP. These bits are used to mask RSV[34:16]
 * for DC3MAC; bit[18] of MASK maps to bit[34] of RSV, bit[0] of MASK
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
int
dc3mac_rsv_selective_mask_set(int unit, soc_port_t port,
                              uint32_t flags, uint32_t value)
{
    int i = 0;
    uint32 tmp_mask = DC3MAC_RSV_MASK_MIN;
    uint32 rsv_mask_bmap[1] = {0};

    SOC_INIT_FUNC_DEFS;

    if (flags > DC3MAC_RSV_MASK_ALL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(dc3mac_rsv_mask_get(unit, port, rsv_mask_bmap));

    while(tmp_mask <= DC3MAC_RSV_MASK_MAX) {
        if (flags & tmp_mask) {
            /*
             * if value = 1 means Enable, set the mask to 0.
             * if value = 0 means Purge, set the mask to 1.
             */
            if (value) {
                SHR_BITCLR(rsv_mask_bmap, i);
            } else {
                SHR_BITSET(rsv_mask_bmap, i);
            }
        }
        tmp_mask = (1 << ++i);
    }

    _SOC_IF_ERR_EXIT(dc3mac_rsv_mask_set(unit, port, rsv_mask_bmap[0]));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_rsv_selective_mask_get(int unit, soc_port_t port,
                              uint32_t flags, uint32_t *value)
{
    uint32 rsv_mask;

    SOC_INIT_FUNC_DEFS;

    /* Check if only 1 bit is set in flags */
    if ((flags) & (flags - 1)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(dc3mac_rsv_mask_get(unit, port, &rsv_mask));

    /*
     * if bit in rsv_mask = 0 means Enable, return 1.
     * if bit in rsv_mask = 1 means Purge, return 0.
     */
    *value = (rsv_mask & flags)? 0: 1;

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_strip_crc_get(int unit, soc_port_t port,
                     uint32_t *enable)
{
    uint32 rval;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_RX_CTRL, &rval, STRIP_CRC, enable);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_strip_crc_set(int unit, soc_port_t port,
                     uint32_t enable)
{
    uint32_t status = 0;
    uint32 rx_ctrl;
    uint32 field_value = 0;

    SOC_INIT_FUNC_DEFS;

    /* If set, CRC is stripped from the received packet */
     _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_RX_CTRL, &rx_ctrl, STRIP_CRC, &status);
    if (status != enable) {
        field_value = enable ? 1 : 0;
        UA_REG_FIELD_SET(unit, DC3MAC_RX_CTRL, &rx_ctrl, STRIP_CRC, &field_value);
    }
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_tx_crc_mode_get(int unit, soc_port_t port,
                       uint32_t *crc_mode)
{
    uint32 tx_ctrl;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_TX_CTRL, &tx_ctrl, CRC_MODE, crc_mode);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_tx_crc_mode_set(int unit, soc_port_t port,
                       uint32_t crc_mode)
{
    uint32 tx_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    /* Validate crc_mode.  */
    if (crc_mode > DC3MAC_CRC_MODE_AUTO) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("illegal CRC mode %d"), crc_mode));
    }

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));
    fld_val = crc_mode;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &tx_ctrl, CRC_MODE, &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_tx_threshold_get(int unit, soc_port_t port,
                        uint32_t *threshold)
{
    uint32 tx_ctrl;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_TX_CTRL, &tx_ctrl, TX_THRESHOLD, threshold);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_tx_threshold_set(int unit, soc_port_t port,
                        uint32_t threshold)
{
    uint32 tx_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));
    fld_val = threshold;
    UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &tx_ctrl, TX_THRESHOLD, &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &tx_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3port_port_mode_set(int unit, soc_port_t port,
                      uint32_t flags, uint32_t lane_mask)
{
    int num_lanes = 0, port_is_upper_half = 0;
    uint32_t port_mode_upper_lower;
    uint32_t port_mode = DC3PORT_PORT_MODE_QUAD;
    uint32_t new_port_mode = DC3PORT_PORT_MODE_QUAD;
    uint32_t new_port_mode_mask = 0xf0;
    uint32_t dc3port_mode;

   SOC_INIT_FUNC_DEFS;

    port_mode_upper_lower = DC3PORT_PORT_MODE_QUAD << 4 |
                            DC3PORT_PORT_MODE_QUAD;

    num_lanes = shr_util_popcount(lane_mask);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_MODE_REG, port, &dc3port_mode));

    /* next need to check if port is lower half or not */
    if ((lane_mask & 0xf0) && (lane_mask != 0xff)) {
        port_is_upper_half = 1;
        new_port_mode_mask = 0xf;
    }

    UA_REG_FIELD_GET(unit, DC3PORT_MODE_REG, &dc3port_mode, MAC_PORT_MODE, &port_mode_upper_lower);
    /* need to get the correct port mode based on port location */
    port_mode = (port_mode_upper_lower >> (port_is_upper_half * 4)) & 0xf;

    /* need to clear 8 lane port mode */
    port_mode &= ~DC3PORT_PORT_MODE_SINGLE_8_LANE;

    if (num_lanes == 8) {
        new_port_mode = DC3PORT_PORT_MODE_SINGLE_8_LANE;
    } else if (num_lanes == 4) {
            new_port_mode = DC3PORT_PORT_MODE_SINGLE;
    } else if (num_lanes == 2) {
        switch (port_mode) {
            case DC3PORT_PORT_MODE_QUAD:
                if ((lane_mask == 0x0c) || (lane_mask == 0xc0)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_012;
                } else if ((lane_mask == 0x03) || (lane_mask == 0x30)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_023;
                }
                break;
            case DC3PORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x3) || (lane_mask == 0x30)) {
                    new_port_mode = DC3PORT_PORT_MODE_DUAL;
                } else if ((lane_mask == 0x0c) || (lane_mask == 0xc0)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_012;
                }
                break;
            case DC3PORT_PORT_MODE_TRI_023:
                if ((lane_mask == 0x3) || (lane_mask == 0x30)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_023;
                } else if ((lane_mask == 0x0c) || (lane_mask == 0xc0)) {
                    new_port_mode = DC3PORT_PORT_MODE_DUAL;
                }
                break;
            case DC3PORT_PORT_MODE_DUAL:
                new_port_mode = DC3PORT_PORT_MODE_DUAL;
                break;
            case DC3PORT_PORT_MODE_SINGLE:
                if ((lane_mask == 0x3) || (lane_mask == 0x30)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_023;
                } else if ((lane_mask == 0xc) || (lane_mask == 0xc0)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_012;
                }
                break;
            default:
                return SOC_E_PARAM;
        }
    } else {
        /* num_lanes == 1 */
        switch (port_mode) {
            case DC3PORT_PORT_MODE_QUAD:
                new_port_mode = DC3PORT_PORT_MODE_QUAD;
                break;
            case DC3PORT_PORT_MODE_TRI_012:
                if ((lane_mask == 0x1) || (lane_mask == 0x2) ||
                    (lane_mask == 0x10) || (lane_mask == 0x20) ) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = DC3PORT_PORT_MODE_QUAD;
                }
                break;
            case DC3PORT_PORT_MODE_TRI_023:
                if ((lane_mask == 0x1) || (lane_mask == 0x2) ||
                    (lane_mask == 0x10) || (lane_mask == 0x20)) {
                    new_port_mode = DC3PORT_PORT_MODE_QUAD;
                } else {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_023;
                }
                break;
            case DC3PORT_PORT_MODE_DUAL:
                if ((lane_mask == 0x1) || (lane_mask == 0x2) ||
                    (lane_mask == 0x10) || (lane_mask == 0x20)) {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_012;
                } else {
                    new_port_mode = DC3PORT_PORT_MODE_TRI_023;
                }
                break;
            case DC3PORT_PORT_MODE_SINGLE:
                new_port_mode = DC3PORT_PORT_MODE_QUAD;
                break;
           default:
                return SOC_E_PARAM;
        }
    }

    /* if not 8 lane port, need to keep the other half port mode config */
    if (num_lanes != 8) {
        /* first clear 800G mode */
        port_mode_upper_lower &= ~DC3PORT_PORT_MODE_SINGLE_8_LANE;
        new_port_mode <<= port_is_upper_half * 4;
        new_port_mode |= port_mode_upper_lower & new_port_mode_mask;
    }

    UA_REG_FIELD_SET(unit, DC3PORT_MODE_REG, &dc3port_mode, MAC_PORT_MODE, &new_port_mode);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3PORT_MODE_REG, port, &dc3port_mode));

exit:
    SOC_FUNC_RETURN;
}

int
dc3port_dc3mac_control_set(int unit, soc_port_t port,
                           uint32_t reset)
{
    uint32 pmac_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_MAC_CONTROL, port, &pmac_ctrl));

    if (reset) {
        fld_val = 1;
        UA_REG_FIELD_SET(unit, DC3PORT_MAC_CONTROL, &pmac_ctrl, MAC_RESET, &fld_val);
    } else {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3PORT_MAC_CONTROL, &pmac_ctrl, MAC_RESET, &fld_val);
    }
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3PORT_MAC_CONTROL, port, &pmac_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3port_dc3mac_control_get(int unit, soc_port_t port,
                           uint32_t *reset)
{
    uint32 pmac_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_MAC_CONTROL, port, &pmac_ctrl));
    UA_REG_FIELD_GET(unit, DC3PORT_MAC_CONTROL, &pmac_ctrl, MAC_RESET, &fld_val);
    *reset = fld_val;

exit:
    SOC_FUNC_RETURN;

}

int
dc3port_tsc_ctrl_get(int unit, soc_port_t port,
                     uint32_t *tsc_rstb, uint32_t *tsc_pwrdwn)
{
    uint32 tsc_ctrl;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_CTRL_REG, port, &tsc_ctrl));
    UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_RSTB, tsc_rstb);
    UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_PWRDWN, tsc_pwrdwn);

exit:
    SOC_FUNC_RETURN;
}

int
dc3port_tsc_ctrl_set(int unit, soc_port_t port,
                     int tsc_pwr_on)
{
    uint32 tsc_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_CTRL_REG, port, &tsc_ctrl));
    if (tsc_pwr_on) {
        fld_val = 1;
        UA_REG_FIELD_SET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_PWRDWN, &fld_val);
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_RSTB, &fld_val);
        _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3PORT_XGXS0_CTRL_REG, port, &tsc_ctrl));
        sal_usleep(1000);
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_PWRDWN, &fld_val);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_RSTB, &fld_val);
        _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3PORT_XGXS0_CTRL_REG, port, &tsc_ctrl));
    } else {
        fld_val = 1;
        UA_REG_FIELD_SET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_PWRDWN, &fld_val);
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3PORT_XGXS0_CTRL_REG, &tsc_ctrl, TSC_RSTB, &fld_val);
        _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3PORT_XGXS0_CTRL_REG, port, &tsc_ctrl));
    }

exit:
    SOC_FUNC_RETURN;
}

int
dc3port_link_status_get(int unit, soc_port_t port,
                        uint32_t start_lane, int* link)
{
    uint32 ln_status;
    SOC_INIT_FUNC_DEFS;

    switch(start_lane) {
        case 0:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN0_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN0_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 1:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN1_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN1_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 2:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN2_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN2_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 3:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN3_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN3_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 4:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN4_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN4_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 5:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN5_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN5_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 6:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN6_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN6_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        case 7:
            _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_XGXS0_LN7_STATUS_REG, port, &ln_status));
            UA_REG_FIELD_GET(unit, DC3PORT_XGXS0_LN7_STATUS_REG, &ln_status, LINK_STATUS, link);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid starting lane %d"), start_lane));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_enable_set(int unit, soc_port_t port,
                       uint32_t enable)
{
    uint32 mac_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &mac_ctrl));
    fld_val = enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, TX_EN, &fld_val);
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, RX_EN, &fld_val);
    fld_val = enable ? 0: 1;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, SOFT_RESET, &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &mac_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_fdr_symbol_error_window_size_set(int unit, soc_port_t port,
                                             uint32_t window_size)
{
    uint32 fdr_control;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FDR_CTRL, port, &fdr_control));
    fld_val = window_size;
    UA_REG_FIELD_SET(unit, DC3MAC_FDR_CTRL, &fdr_control, SYMBOL_ERROR_WINDOW_MODE,
                     &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_FDR_CTRL, port, &fdr_control));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_fdr_symbol_error_window_size_get(int unit, soc_port_t port,
                                             uint32_t* window_size)
{
    uint32 fdr_control;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FDR_CTRL, port, &fdr_control));
    UA_REG_FIELD_GET(unit, DC3MAC_FDR_CTRL, &fdr_control, SYMBOL_ERROR_WINDOW_MODE, window_size);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_fdr_symbol_error_count_threshold_set(int unit, soc_port_t port,
                       uint32_t threshold)
{
    uint32 fdr_control;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FDR_CTRL, port, &fdr_control));
    fld_val = threshold;
    UA_REG_FIELD_SET(unit, DC3MAC_FDR_CTRL, &fdr_control, SYMBOL_ERROR_COUNT_THRESHOLD,
                     &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_FDR_CTRL, port, &fdr_control));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_fdr_symbol_error_count_threshold_get(int unit, soc_port_t port,
                                                 uint32_t* threshold)
{
    uint32 fdr_control;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_FDR_CTRL, port, &fdr_control));
    UA_REG_FIELD_GET(unit, DC3MAC_FDR_CTRL, &fdr_control, SYMBOL_ERROR_COUNT_THRESHOLD, threshold);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_port_mac_link_down_seq_enable_set(int unit, soc_port_t port,
                                         uint32_t enable)
{
    uint32 mac_ctrl;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &mac_ctrl));
    fld_val = enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &mac_ctrl, MAC_LINK_DOWN_SEQ_EN,
                     &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &mac_ctrl));

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_rx_da_timestmap_enable_get(int unit, soc_port_t port,
                                  uint32_t *enable)
{
    uint32 rx_ctrl;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));
    UA_REG_FIELD_GET(unit, DC3MAC_RX_CTRL, &rx_ctrl, DA_TIMESTAMP_EN, enable);

exit:
    SOC_FUNC_RETURN;
}

int
dc3mac_rx_da_timestmap_enable_set(int unit, soc_port_t port,
                                  uint32_t enable)
{
    uint32 rx_ctrl;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    /*
     * This configuration is used to enabel mac da timestamp
     * If set, dc3mac will use the mac da for timestamping.
     * If reset, sfd will be used.
     */
    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));
    fld_val = enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, DC3MAC_RX_CTRL, &rx_ctrl, DA_TIMESTAMP_EN, &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_CTRL, port, &rx_ctrl));

exit:
    SOC_FUNC_RETURN;

}

int
dc3mac_reset_check(int unit, int port, int enable, int *reset)
{
    uint32 rval, orig_rval, fld_val = 0;

    *reset = 1;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    orig_rval = rval;

    fld_val = enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, TX_EN, &fld_val);
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);

    if (rval == orig_rval) {
        if (enable) {
            *reset = 0;
        } else {
            UA_REG_FIELD_GET(unit, DC3MAC_CTRL, &rval, SOFT_RESET, &fld_val);
            if (fld_val) {
                *reset = 0;
            }
        }
    }

    return (SOC_E_NONE);
}

int
dc3mac_txfifo_cell_cnt_get(int unit, int port, uint32* val)
{
    uint32 rval;
    uint32 fld_val = 0;

    if (UA_REG_IS_VALID(unit, DC3MAC_TXFIFO_STATUS)) {
        SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_TXFIFO_STATUS, port, &rval));
        UA_REG_FIELD_GET(unit, DC3MAC_TXFIFO_STATUS, &rval, CELL_CNT, &fld_val);
        *val = fld_val;
    } else {
            return (SOC_E_NOT_FOUND);
    }

    return (SOC_E_NONE);
}

int
dc3mac_mac_ctrl_set(int unit, int port, uint64 ctrl)
{
    uint32 rval;

    COMPILER_64_TO_32_LO(rval, ctrl);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));
    return (SOC_E_NONE);
}

int
dc3port_port_fault_link_status_set(int unit, int port, int enable)
{
    uint32 reg_val;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3PORT_FAULT_LINK_STATUS, port, &reg_val));
    fld_val =(uint32)enable;
    UA_REG_FIELD_SET(unit, DC3PORT_FAULT_LINK_STATUS, &reg_val, REMOTE_FAULT, &fld_val);
    UA_REG_FIELD_SET(unit, DC3PORT_FAULT_LINK_STATUS, &reg_val, LOCAL_FAULT, &fld_val);
    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3PORT_FAULT_LINK_STATUS, port, &reg_val));

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag,
                        int inner_vlan_tag)
{
    uint64 rval64;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_VLAN_TAG, port, &rval64));

    if(inner_vlan_tag == -1) {
       fld_val = 0;
       UA_REG_FIELD_SET(unit, DC3MAC_VLAN_TAG, &rval64,
                             INNER_VLAN_TAG_ENABLE, &fld_val);
    } else {
       fld_val =(uint32)inner_vlan_tag;
       UA_REG_FIELD_SET(unit, DC3MAC_VLAN_TAG, &rval64,
                             INNER_VLAN_TAG, &fld_val);
       fld_val = 1;
       UA_REG_FIELD_SET(unit, DC3MAC_VLAN_TAG, &rval64,
                             INNER_VLAN_TAG_ENABLE, &fld_val);
    }

    if(outer_vlan_tag == -1) {
       fld_val = 0;
       UA_REG_FIELD_SET(unit, DC3MAC_VLAN_TAG, &rval64,
                             OUTER_VLAN_TAG_ENABLE, &fld_val);
    } else {
       fld_val =(uint32)outer_vlan_tag;
       UA_REG_FIELD_SET(unit, DC3MAC_VLAN_TAG, &rval64,
                             OUTER_VLAN_TAG, &fld_val);
       fld_val = 1;
       UA_REG_FIELD_SET(unit, DC3MAC_VLAN_TAG, &rval64,
                             OUTER_VLAN_TAG_ENABLE, &fld_val);
    }

    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_VLAN_TAG, port, &rval64));

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag,
                        int *inner_vlan_tag)
{
    uint64 rval64;
    uint32 is_enabled = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_VLAN_TAG, port, &rval64));

    UA_REG_FIELD_GET(unit, DC3MAC_VLAN_TAG, &rval64,
                INNER_VLAN_TAG_ENABLE, &is_enabled);
    if(is_enabled == 0) {
       *inner_vlan_tag = -1;
    } else {
       UA_REG_FIELD_GET(unit, DC3MAC_VLAN_TAG,
                   &rval64, INNER_VLAN_TAG, inner_vlan_tag);
    }

    UA_REG_FIELD_GET(unit, DC3MAC_VLAN_TAG, &rval64,
                OUTER_VLAN_TAG_ENABLE, &is_enabled);
    if(is_enabled == 0) {
       *outer_vlan_tag = -1;
    } else {
       UA_REG_FIELD_GET(unit, DC3MAC_VLAN_TAG,
                   &rval64, OUTER_VLAN_TAG, outer_vlan_tag);
    }

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_drain_cell_get(int unit, int port,
                          portmod_drain_cells_t *drain_cells)
{
    uint32 rval;
    uint64 rval64;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &rval));
    UA_REG_FIELD_GET(unit, DC3MAC_PFC_CTRL, &rval,
                     RX_PFC_EN, &fld_val);
     drain_cells->rx_pfc_en = fld_val;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &rval64));
    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL,
                &rval64, RX_PAUSE_EN, &fld_val);
    drain_cells->rx_pause = fld_val;

    UA_REG_FIELD_GET(unit, DC3MAC_PAUSE_CTRL,
                &rval64, TX_PAUSE_EN, &fld_val);
    drain_cells->tx_pause = fld_val;

    return (SOC_E_NONE);
}

int dc3mac_drain_cell_stop(int unit, int port,
                           const portmod_drain_cells_t *drain_cells)
{
    uint32 rval;
    uint64 rval64;
    uint32 fld_val = 0;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(dc3mac_discard_set(unit, port, 0));

    /* set pause fields */
    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

    fld_val =(uint32)drain_cells->rx_pause;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, RX_PAUSE_EN,
                     &fld_val);
    fld_val =(uint32)drain_cells->tx_pause;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, TX_PAUSE_EN,
                     &fld_val);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

    /* set pfc rx_en fields */
    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &rval));
    fld_val =(uint32)drain_cells->rx_pfc_en;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, RX_PFC_EN,
                     &fld_val);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &rval));

    return (SOC_E_NONE);
}

int dc3mac_drain_cell_start(int unit, int port)
{
    uint32 rval;
    uint64 rval64;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    fld_val = 1;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, TX_EN, &fld_val);
    /* Disable RX */
    fld_val = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_PAUSE_CTRL, port, &rval64));
    fld_val = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PAUSE_CTRL, &rval64, RX_PAUSE_EN, &fld_val);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_PAUSE_CTRL, port, &rval64));

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_PFC_CTRL, port, &rval));
    fld_val = 0;
    UA_REG_FIELD_SET(unit, DC3MAC_PFC_CTRL, &rval, RX_PFC_EN, &fld_val);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_PFC_CTRL, port, &rval));

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    fld_val = 1;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, SOFT_RESET, &fld_val);
    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

    SOC_IF_ERROR_RETURN(dc3mac_discard_set(unit, port, 1));

    return (SOC_E_NONE);
}

int dc3mac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint32 rval, orig_rval;
    uint32 soft_reset = 0;
    uint32 fld_val = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    orig_rval = rval;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    fld_val = 1;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, TX_EN, &fld_val);
    fld_val = rx_en ? 1: 0;
    UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);

    if (rval == orig_rval) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing scenario, DC3MAC_DC_3_MAC_CTRLr.SOFT_RESETf
         *     is used to early check to see if this port is at disabled state
         *     already.
         */
        UA_REG_FIELD_GET(unit, DC3MAC_CTRL, &rval, SOFT_RESET, &soft_reset);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (rx_en) {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, SOFT_RESET, &fld_val);
    }

    SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

    return (SOC_E_NONE);
}

int dc3mac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint32 rval;
    uint32 drain_timeout, txfifo_cell_cnt;
    uint32 fld_val = 0;
    soc_timeout_t to;

    drain_timeout = SAL_BOOT_QUICKTURN ? 250000000 : 250000;

    /* Enable RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    if (rx_en) {
        uint32 dc3mac_tx_ctrl_rval;

        SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_TX_CTRL, port, &dc3mac_tx_ctrl_rval));

        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);
        SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

        fld_val = 1;
        UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &dc3mac_tx_ctrl_rval, DISCARD, &fld_val);
        SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &dc3mac_tx_ctrl_rval));

        /* Wait until TX fifo cell count is 0 */
        if (!SAL_BOOT_SIMULATION){
            soc_timeout_init(&to, drain_timeout, 0);
            for (;;) {
                SOC_IF_ERROR_RETURN(dc3mac_txfifo_cell_cnt_get(unit, port, &txfifo_cell_cnt));
                if (txfifo_cell_cnt == 0) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                    "dc3mac_egress_queue_drain_rx_en: unit %d "
                                    "ERROR: port %s: "
                                    "timeout draining TX FIFO (%d cells remain)\n"),
                                    unit, SOC_PORT_NAME(unit, port), txfifo_cell_cnt));
                    return (SOC_E_INTERNAL);
                }
            }
        }

        fld_val = 1;
        UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);
        SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));

        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_TX_CTRL, &dc3mac_tx_ctrl_rval, DISCARD, &fld_val);
        SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_TX_CTRL, port, &dc3mac_tx_ctrl_rval));
        sal_usleep(10000);
    } else {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);
        SOC_IF_ERROR_RETURN(DC3PORT_REG_WRITE(unit, DC3MAC_CTRL, port, &rval));
    }

    /*Bring mac out of reset */
    SOC_IF_ERROR_RETURN(dc3mac_reset_set(unit, port, 0));

    return (SOC_E_NONE);
}

int dc3mac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl,
                                  int *rx_en)
{
    uint32 rval;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(DC3PORT_REG_READ(unit, DC3MAC_CTRL, port, &rval));
    COMPILER_64_SET(*mac_ctrl, 0, rval);

    UA_REG_FIELD_GET(unit, DC3MAC_CTRL, &rval, RX_EN, &fld_val);
    *rx_en = fld_val;

    return (SOC_E_NONE);
}

int dc3mac_link_fault_os_set(int unit, int port, int is_remote, uint32 enable)
{
    uint32 rval;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    enable = enable ? 1 : 0;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

    if (enable) {
        if (is_remote) {
            fld_val = 1;
            UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, &fld_val);
            fld_val = 0;
            UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, &fld_val);
            fld_val = 2;
            UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FAULT_SOURCE_FOR_TX, &fld_val);
        } else {
            fld_val = 1;
            UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, &fld_val);
            fld_val = 0;
            UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, &fld_val);
            fld_val = 2;
            UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FAULT_SOURCE_FOR_TX, &fld_val);
        }
    } else {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, &fld_val);
        UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, &fld_val);
        UA_REG_FIELD_SET(unit, DC3MAC_RX_LSS_CTRL, &rval, FAULT_SOURCE_FOR_TX, &fld_val);
    }

    _SOC_IF_ERR_EXIT(DC3PORT_REG_WRITE(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

exit:
    SOC_FUNC_RETURN;
}

int dc3mac_link_fault_os_get(int unit, int port, int is_remote, uint32 * enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(DC3PORT_REG_READ(unit, DC3MAC_RX_LSS_CTRL, port, &rval));

    if (is_remote) {
        UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                         FORCE_REMOTE_FAULT_OS, enable);
    } else {
        UA_REG_FIELD_GET(unit, DC3MAC_RX_LSS_CTRL, &rval,
                         FORCE_LOCAL_FAULT_OS, enable);
    }

exit:
    SOC_FUNC_RETURN;
}


int dc3port_timestamp_fifo_status_get(int unit, int port, int start_lane, uint32 *ts_valid)
{
    uint32 timestamp_status;
    uint32 valid_bits;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (DC3PORT_REG_READ(unit, DC3PORT_TWOSTEP_TIMESTAMP_STATUS,
                          port, &timestamp_status));
    UA_REG_FIELD_GET(unit, DC3PORT_TWOSTEP_TIMESTAMP_STATUS,
                     &timestamp_status, TWOSTEP_FIFO_ENTRY_VALID,
                     &valid_bits);
    *ts_valid  = (valid_bits & (0x1 << start_lane))?1:0;

exit:
    SOC_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

#endif
