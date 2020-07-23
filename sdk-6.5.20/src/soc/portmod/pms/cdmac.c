/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    cdmac.c
 * Purpose: 
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/cdmac.h>
#include <soc/drv.h>

#if defined(PORTMOD_PM8X50_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

/* Minimun and maximum average IPG value in bits */
#define CDMAC_AVE_IPG_MIN 64
#define CDMAC_AVE_IPG_MAX 480

/*
 * version device
 * 0x8000a011  PM8X50 FLEXE
 * 0x8000a012  PM8X50 GEN2
 */
#define CDMAC_VERSION_PM8X50_FLEXE              0x8000a011
#define CDMAC_VERSION_PM8X50_GEN2               0x8000a012
#define CDMAC_VERSION_PM8X50_GEN2_7NM_B0        0x8000a014

int cdmac_init(int unit, soc_port_t port, uint32 init_flags)
{
    uint32 rval;
    portmod_pause_control_t pause_control;
    portmod_pfc_control_t pfc_control;
    portmod_remote_fault_control_t remote_fault_control;
    portmod_local_fault_control_t local_fault_control;
    int crc_mode;
    int is_strip_crc, is_append_crc, is_replace_crc, is_pass_through_crc;
    int ctr_enable = 0, ctr_clear = 0;
    SOC_INIT_FUNC_DEFS;

    is_strip_crc = (init_flags & CDMAC_INIT_F_RX_STRIP_CRC ? 1 : 0);
    is_append_crc = (init_flags & CDMAC_INIT_F_TX_APPEND_CRC ? 1 : 0);
    is_replace_crc = (init_flags & CDMAC_INIT_F_TX_REPLACE_CRC ? 1 : 0);
    is_pass_through_crc = (init_flags & CDMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE?
                           1: 0);

    if(is_append_crc + is_replace_crc + is_pass_through_crc > 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("CDMAC_INIT_F_TX_APPEND_CRC, "
                                    "CDMAC_INIT_F_TX_REPLACE_CRC, and "
                                    "CDMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE "
                                    "can't co-exist")));
    }

    /* RX Max size */
    _SOC_IF_ERR_EXIT(cdmac_rx_max_size_set(unit, port, CDMAC_JUMBO_MAXSZ));

    /*
     * Enable and Clear the CDMAC MIB counters
     * No need to explicitly set the clear flag to 0 since the
     * function internally unsets the clear flag
     */
    ctr_enable = TRUE;
    ctr_clear = TRUE;
    _SOC_IF_ERR_EXIT(cdmac_mib_counter_control_set(unit, port, ctr_enable,
                                                   ctr_clear));

    /* RX Control */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, STRIP_CRCf, is_strip_crc);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RX_PASS_PAUSEf, 0);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RX_PASS_PFCf, 0);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RUNT_THRESHOLDf,
                      CDMAC_RUNT_THRESHOLD_DEFAULT);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    /* TX Control */
    if(is_append_crc) {
        /* CRC is computed on incoming packet data and appended. */
        crc_mode = 0;
    } else if(is_replace_crc) {
        /* Incoming pkt CRC is replaced with CRC value computed by the MAC */
        crc_mode = 2;
    } else if(is_pass_through_crc) {
        /* Incoming pkt CRC is passed through without modifications */
        crc_mode = 1;
    } else {
        /* The CRC mode is determined by the HW */
        crc_mode = 3;
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, CRC_MODEf, crc_mode);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, DISCARDf, FALSE);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_ENf, FALSE);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_THRESHOLDf,
                      CDMAC_PAD_THRESHOLD_SIZE_DEFAULT);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, AVERAGE_IPGf,
                      CDMAC_AVERAGE_IPG_DEFAULT);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, TX_THRESHOLDf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

    /* ECC control */
    /* TX CDC ecc control */
    _SOC_IF_ERR_EXIT(READ_CDMAC_ECC_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_ECC_CTRLr, &rval,
                      TX_CDC_ECC_CTRL_ENf, 1);
    soc_reg_field_set(unit, CDMAC_ECC_CTRLr, &rval, TX_CDC_ECC_CTRL_ENf, TRUE);
    soc_reg_field_set(unit, CDMAC_ECC_CTRLr, &rval, MIB_COUNTER_ECC_CTRL_ENf,
                      TRUE);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_ECC_CTRLr(unit, port, rval));

    /* LSS */
    _SOC_IF_ERR_EXIT(
            portmod_remote_fault_control_t_init(unit,&remote_fault_control));
    remote_fault_control.enable = TRUE;
    remote_fault_control.drop_tx_on_fault = TRUE;
    _SOC_IF_ERR_EXIT(cdmac_remote_fault_control_set(unit, port,
                                                    &remote_fault_control));

    _SOC_IF_ERR_EXIT(
            portmod_local_fault_control_t_init(unit, &local_fault_control));
    local_fault_control.enable = TRUE;
    local_fault_control.drop_tx_on_fault = TRUE;
    _SOC_IF_ERR_EXIT(cdmac_local_fault_control_set(unit, port,
                                                   &local_fault_control));

    /* Pause */
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    pause_control.tx_enable = TRUE;
    pause_control.rx_enable = TRUE;
    _SOC_IF_ERR_EXIT(cdmac_pause_control_set(unit, port, &pause_control));

    /* PFC */
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_get(unit, port, &pfc_control));
    pfc_control.rx_enable = FALSE;
    pfc_control.tx_enable = FALSE;
    pfc_control.stats_en = TRUE;
    _SOC_IF_ERR_EXIT(cdmac_pfc_control_set(unit, port, &pfc_control));

    /* MAC control */
    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LOCAL_LPBKf, FALSE);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, TRUE);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, TRUE);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, MAC_LINK_DOWN_SEQ_ENf, TRUE);
    /* de-assert SOFT RESET */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, FALSE);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_speed_set(int unit, soc_port_t port, int flags, int speed)
{
    /*
     * There is nothing to be programmed in CDMAC for Speed.
     * Speed setting is handled in the serdes.
     */
    return (SOC_E_NONE);
}

int cdmac_speed_get(int unit, soc_port_t port, int *speed)
{
    /*
     * There is nothing to be programmed in CDMAC for Speed.
     * Speed setting is handled in the serdes.
     */
    return (SOC_E_NONE);
}

int cdmac_encap_set(int unit, soc_port_t port, int flags,
                    portmod_encap_t encap)
{

    uint32 val = 0;
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Only IEEE encap support, HIGIG not supported.
     */
    switch(encap){
        case SOC_ENCAP_IEEE:
            val = 0;
            break;
        case SOC_ENCAP_SOP_ONLY:
        /* no preamble or sfd, 0xFB is followed by mac da */
            val = 5;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("illegal encap mode %d"), encap));
            break;
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_MODEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_MODEr, &rval, HDR_MODEf, val);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_MODEr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_encap_get(int unit, soc_port_t port, int *flags,
                    portmod_encap_t *encap)
{
    uint32 rval;
    uint32 fld_val;
    SOC_INIT_FUNC_DEFS;

    (*flags) = 0;

    _SOC_IF_ERR_EXIT(READ_CDMAC_MODEr(unit, port, &rval));
    fld_val = soc_reg_field_get(unit, CDMAC_MODEr, rval, HDR_MODEf);

    switch(fld_val){
        case 0:
            *encap = SOC_ENCAP_IEEE;
            break;
        case 5:
            *encap = SOC_ENCAP_SOP_ONLY;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("unknown encap mode %d"), fld_val));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, enable? 1: 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, enable? 0: 1);

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    if (flags & CDMAC_ENABLE_SET_FLAGS_TX_EN) {
        *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, TX_ENf);
    } else {
        *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, RX_ENf);
    }

    return (SOC_E_NONE);
}

/*
 * This function can be called to either set/reset
 * the TX/RX or put the MAC in reset/bring the MAC
 * out of reset.
 * Only one operation allowed in a single call.
 */
int cdmac_enable_selective_set(int unit, soc_port_t port,
                               int flags, int enable)
{
    uint32 rval, orig_rval;
    SOC_INIT_FUNC_DEFS;


    if (!(flags == CDMAC_ENABLE_SET_FLAGS_TX_EN) &&
        !(flags == CDMAC_ENABLE_SET_FLAGS_RX_EN) &&
        !(flags == CDMAC_ENABLE_SET_FLAGS_SOFT_RESET)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("unknown control flag - %x"), flags));
    }

    /*
     * based on the flags passed the required fields are set/reset
     */

    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
     orig_rval = rval;

    if (flags == CDMAC_ENABLE_SET_FLAGS_TX_EN) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, enable? 1: 0);
    }

    if (flags == CDMAC_ENABLE_SET_FLAGS_RX_EN) {
        if (enable) {
            uint32 cdmac_tx_ctrl_rval;

            SOC_IF_ERROR_RETURN(READ_CDMAC_TX_CTRLr(unit, port, &cdmac_tx_ctrl_rval));

            soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

            soc_reg_field_set(unit, CDMAC_TX_CTRLr, &cdmac_tx_ctrl_rval, DISCARDf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, cdmac_tx_ctrl_rval));
            sal_usleep(10000);

            soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 1);
            SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

            soc_reg_field_set(unit, CDMAC_TX_CTRLr, &cdmac_tx_ctrl_rval, DISCARDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, cdmac_tx_ctrl_rval));
        } else {
            soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));
        }
    }

    /*
     * if enable = 0, bring mac out of reset, else mac in reset.
     */
    if (flags == CDMAC_ENABLE_SET_FLAGS_SOFT_RESET) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, enable? 1: 0);
    }

    /* write only if value changed */
    if(rval != orig_rval) { /* write only if value changed */
       _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    SOC_INIT_FUNC_DEFS;
    if (!duplex) {
        _SOC_EXIT_WITH_ERR(SOC_E_UNAVAIL,
                          (_SOC_MSG("half-duplex unsupported")));
    }

    /* Only duplex support, nothing to be done here */
exit:
    SOC_FUNC_RETURN;
}

int cdmac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    SOC_INIT_FUNC_DEFS;

    /* Only duplex support */
    *duplex = TRUE;

    SOC_FUNC_RETURN;
}


int cdmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb,
                       int enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /* only line side loopback supported */
    if (lb == portmodLoopbackMacOuter) {
        _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LOCAL_LPBKf,
                          enable? 1: 0);
        _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("unsupported loopback type %d"), lb));
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb,
                       int *enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    if (lb == portmodLoopbackMacOuter) {
        _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
        *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, LOCAL_LPBKf);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("unsupported loopback type %d"), lb));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_tx_mac_enable_set is the caller function
 */
int cdmac_tx_enable_set (int unit, soc_port_t port, int enable)
{
    return(cdmac_enable_selective_set(unit, port,
                        CDMAC_ENABLE_SET_FLAGS_TX_EN, enable));
}

/*
 * portmod_port_tx_mac_enable_get is the caller function
 */
int cdmac_tx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, TX_ENf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_rx_mac_enable_set is the caller function
 */
int cdmac_rx_enable_set (int unit, soc_port_t port, int enable)
{

    return(cdmac_enable_selective_set(unit, port,
                        CDMAC_ENABLE_SET_FLAGS_RX_EN, enable));
}

/*
 * portmod_port_rx_mac_enable_get is the caller function
 */
int cdmac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, RX_ENf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_mac_reset_set is the caller function
 */
int cdmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    return(cdmac_enable_selective_set(unit, port,
                        CDMAC_ENABLE_SET_FLAGS_SOFT_RESET, enable));
}

/*
 * portmod_port_mac_reset_set is the caller function
 */
int cdmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, SOFT_RESETf);

    return (SOC_E_NONE);
}

int cdmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    /*
     * Write bytes in the following order.
     * mac[0] -> BYTE 5(MSB) of the MAC address in HW
     * mac[1] -> BYTE 4
     * mac[2] -> BYTE 3
     * mac[3] -> BYTE 2
     * mac[4] -> BYTE 1
     * mac[5] -> BYTE 0(LSB)
     */
    COMPILER_64_ZERO(rval64);
    COMPILER_64_SET(rval64, _shr_uint16_read(&mac[0]),
                    _shr_uint32_read(&mac[2]));

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_MAC_SAr(unit, port, rval64));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_MAC_SAr(unit, port, &rval64));
    field64 = soc_reg64_field_get(unit, CDMAC_TX_MAC_SAr, rval64, CTRL_SAf);

    mac_addr[0] = COMPILER_64_HI(field64);
    mac_addr[1] = COMPILER_64_LO(field64);

    /*
     * Extract bytes in the following order
     * BYTE 5(MSB) -> mac[0]
     * BYTE 4 -> mac[1]
     * BYTE 3 -> mac[2]
     * BYTE 2 -> mac[3]
     * BYTE 1 -> mac[4]
     * BYTE 0(LSB) -> mac[5]
     */
    mac[0] = (mac_addr[0] & 0x0000ff00) >> 8;
    mac[1] = (mac_addr[0] & 0x000000ff);
    mac[2] = (mac_addr[1] & 0xff000000) >> 24;
    mac[3] = (mac_addr[1] & 0x00ff0000) >> 16;
    mac[4] = (mac_addr[1] & 0x0000ff00) >> 8;
    mac[5] = (mac_addr[1] & 0x000000ff);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(rval64);
    COMPILER_64_SET(rval64, _shr_uint16_read(&mac[0]),
                    _shr_uint32_read(&mac[2]));

    /*
     * Write bytes in the following order.
     * mac[0] -> BYTE 5(MSB) of the MAC address in HW
     * mac[1] -> BYTE 4
     * mac[2] -> BYTE 3
     * mac[3] -> BYTE 2
     * mac[4] -> BYTE 1
     * mac[5] -> BYTE 0(LSB)
     */
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_MAC_SAr(unit, port, rval64));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAC_SAr(unit, port, &rval64));
    field64 = soc_reg64_field_get(unit, CDMAC_RX_MAC_SAr, rval64, RX_SAf);

    mac_addr[0] = COMPILER_64_HI(field64);
    mac_addr[1] = COMPILER_64_LO(field64);

    /*
     * Extract bytes in the following order
     * BYTE 5(MSB) -> mac[0]
     * BYTE 4 -> mac[1]
     * BYTE 3 -> mac[2]
     * BYTE 2 -> mac[3]
     * BYTE 1 -> mac[4]
     * BYTE 0(LSB) -> mac[5]
     */
    mac[0] = (mac_addr[0] & 0x0000ff00) >> 8;
    mac[1] = (mac_addr[0] & 0x000000ff);
    mac[2] = (mac_addr[1] & 0xff000000) >> 24;
    mac[3] = (mac_addr[1] & 0x00ff0000) >> 16;
    mac[4] = (mac_addr[1] & 0x0000ff00) >> 8;
    mac[5] = (mac_addr[1] & 0x000000ff);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag,
                          int inner_vlan_tag)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_VLAN_TAGr(unit, port, &rval64));

    if(inner_vlan_tag == -1) {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             INNER_VLAN_TAG_ENABLEf, 0);
    } else {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             INNER_VLAN_TAGf, inner_vlan_tag);
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             INNER_VLAN_TAG_ENABLEf, 1);
    }

    if(outer_vlan_tag == -1) {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             OUTER_VLAN_TAG_ENABLEf, 0);
    } else {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             OUTER_VLAN_TAGf, outer_vlan_tag);
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             OUTER_VLAN_TAG_ENABLEf, 1);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag,
                          int *inner_vlan_tag)
{
    uint64 rval64;
    uint32 is_enabled = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_VLAN_TAGr(unit, port, &rval64));

    is_enabled = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr, rval64,
                                       INNER_VLAN_TAG_ENABLEf);
    if(is_enabled == 0) {
       *inner_vlan_tag = -1;
    } else {
       *inner_vlan_tag = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr,
                                               rval64, INNER_VLAN_TAGf);
    }

    is_enabled = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr, rval64,
                                       OUTER_VLAN_TAG_ENABLEf);
    if(is_enabled == 0) {
       *outer_vlan_tag = -1;
    } else {
       *outer_vlan_tag = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr,
                                               rval64, OUTER_VLAN_TAGf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Maximum packet size in receive direction, exclusive of preamble
     * and  CRC in strip mode. Packets greater than this size are
     * truncated to this value.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAX_SIZEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf, value);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_MAX_SIZEr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAX_SIZEr(unit, port, &rval));
     *value = soc_reg_field_get(unit, CDMAC_RX_MAX_SIZEr, rval, RX_MAX_SIZEf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pad_size_set(int unit, soc_port_t port, int value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    if (((value < CDMAC_PAD_THRESHOLD_SIZE_MIN) ||
        (value > CDMAC_PAD_THRESHOLD_SIZE_MAX)) &&
        (value != 0)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (_SOC_MSG("unsupported pad threshold size %d"), value));
    }

    /*
     * If PAD_EN is set, packets smaller than PAD_THRESHOLD are padded
     * to this size. PAD_THRESHOLD must be >=64 and <= 96.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_ENf, value? 1: 0);
    if (value) {
        soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_THRESHOLDf, value);
    }
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pad_size_get(int unit, soc_port_t port, int *value)
{
    uint32 rval;
    uint32 pad_en;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    pad_en = soc_reg_field_get(unit, CDMAC_TX_CTRLr, rval, PAD_ENf);
    if(!pad_en) {
        *value = 0;
    } else {
        *value = soc_reg_field_get(unit, CDMAC_TX_CTRLr, rval, PAD_THRESHOLDf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_average_ipg_set(int unit, soc_port_t port, int val)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Average inter packet gap can be in the range 8 to 56 or 60.
     * should be 56 for XLGMII, 60 for XGMII,
     * default is 12.
     * Granularity is in bytes.
     * Input 'val' is in bits.
     */
    if ((val < CDMAC_AVE_IPG_MIN) || (val > CDMAC_AVE_IPG_MAX)) {
       _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("Average IPG is out of range.")));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, AVERAGE_IPGf, val/8);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_average_ipg_get(int unit, soc_port_t port, int *val)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Register value is in bytes.
     * Output 'val' is in bits.
     */

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));

    *val = soc_reg_field_get(unit, CDMAC_TX_CTRLr, rval, AVERAGE_IPGf) * 8;

exit:
    SOC_FUNC_RETURN;
}

int cdmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * The threshold, below which the packets are dropped or
     * marked as runt. Should be programmed >=64 and <= 96 bytes.
     */
    if ((value < CDMAC_RUNT_THRESHOLD_MIN) ||
        (value > CDMAC_RUNT_THRESHOLD_MAX)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
            (_SOC_MSG("runt size should be greater than %d and "
            "smaller than %d. got %d"), CDMAC_RUNT_THRESHOLD_MIN,
            CDMAC_RUNT_THRESHOLD_MAX,  value));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RUNT_THRESHOLDf, value);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RUNT_THRESHOLDf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_remote_fault_control_set is the caller of this function.
 */
int cdmac_remote_fault_control_set(int unit, soc_port_t port,
                          const portmod_remote_fault_control_t *control)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * REMOTE_FAULT_DISABLE determines the transmit response during remote
     * fault state. The REMOTE_FAULT_STATUS bit is always updated irrespective
     * of this configuration.
     * If set, MAC will continue to transmit data irrespective of
     *  REMOTE_FAULT_STATUS.
     * If reset, MAC transmit behavior is governed by
     * DROP_TX_DATA_ON_REMOTE_FAULT configuration.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, REMOTE_FAULT_DISABLEf,
                      control->enable? 0: 1); /* flip */
    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval,
                      DROP_TX_DATA_ON_REMOTE_FAULTf,
                      control->drop_tx_on_fault? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_remote_fault_control_get is the caller of this function.
 */
int cdmac_remote_fault_control_get(int unit, soc_port_t port,
                                   portmod_remote_fault_control_t *control)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             REMOTE_FAULT_DISABLEf);
    /* if fval is reset, indicates enable */
    control->enable = (fval? 0: 1); 

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             DROP_TX_DATA_ON_REMOTE_FAULTf);
    control->drop_tx_on_fault = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_remote_fault_status_get is the caller of this function.
 * The status bit is clear on read, no seperate api for status_clear  
 * required.
 */
int cdmac_remote_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
    /* fault status bits clear on read */
    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                             REMOTE_FAULT_STATUSf);
    *status = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_local_fault_control_set is the caller of this function.
 */
int cdmac_local_fault_control_set(int unit, soc_port_t port,
                                  const portmod_local_fault_control_t *control)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * LOCALFAULT_DISABLE determines the transmit response during remote
     * fault state. The LOCAL_FAULT_STATUS bit is always updated irrespective
     * of this configuration.
     * If set, MAC will continue to transmit data irrespective of
     *  REMOTE_FAULT_STATUS.
     * If reset, MAC transmit behavior is governed by 
     * DROP_TX_DATA_ON_LOCAL_FAULT configuration.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, LOCAL_FAULT_DISABLEf,
                      control->enable? 0: 1); /* flip */
    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval,
                      DROP_TX_DATA_ON_LOCAL_FAULTf,
                      control->drop_tx_on_fault? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_local_fault_control_get is the caller of this function.
 */
int cdmac_local_fault_control_get(int unit, soc_port_t port,
                                  portmod_local_fault_control_t *control)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             LOCAL_FAULT_DISABLEf);
    control->enable = (fval? 0: 1);

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             DROP_TX_DATA_ON_LOCAL_FAULTf);
    control->drop_tx_on_fault = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_local_fault_status_get is the caller of this function.
 * The status bit is clear on read, no seperate api for status_clear  
 * required.
 */
int cdmac_local_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
    /* fault status bits clear on read */
    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                             LOCAL_FAULT_STATUSf);
    *status = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_pfc_control_set is the caller of this function.
 */
int cdmac_pfc_control_set(int unit, soc_port_t port,
                         const portmod_pfc_control_t *control)
{
    uint64 rval64;
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    /** When control->refresh_timer=-1 PFC_REFRESH_TIMER must be set to 
      * 0xFFFF and the PFC_REFRESH_EN must be 1, to be consistent with clmac/xlmac */
    if(control->refresh_timer) {
        soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                PFC_REFRESH_TIMERf, control->refresh_timer);

        soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                PFC_REFRESH_ENf, 1);
    } else {
        soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                PFC_REFRESH_ENf, 0);
    }


    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
            PFC_XOFF_TIMERf, control->xoff_timer);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));


    _SOC_IF_ERR_EXIT(READ_CDMAC_PFC_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, PFC_STATS_ENf,
                      control->stats_en);
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, FORCE_PFC_XONf,
                      control->force_xon);
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, TX_PFC_ENf,
                      control->tx_enable);
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, RX_PFC_ENf,
                      control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_PFC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pfc_control_get(int unit, soc_port_t port,
                          portmod_pfc_control_t *control)
{
    uint64 rval64;
    uint32 rval;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    refresh_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                       PFC_REFRESH_ENf);
    refresh_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
            PFC_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable? refresh_timer: 0);

    control->xoff_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
            rval64, PFC_XOFF_TIMERf);

    _SOC_IF_ERR_EXIT(READ_CDMAC_PFC_CTRLr(unit, port, &rval));

    control->stats_en = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           PFC_STATS_ENf);
    control->force_xon = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           FORCE_PFC_XONf);
    control->tx_enable = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           TX_PFC_ENf);
    control->rx_enable = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           RX_PFC_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pause_control_set(int unit, soc_port_t port,
                            const portmod_pause_control_t *control)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    if(control->rx_enable || control->tx_enable) {
       if(control->refresh_timer > 0 ) {
          soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                            PAUSE_REFRESH_TIMERf, control->refresh_timer);
          soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                            PAUSE_REFRESH_ENf, 1);
       } else {
          soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                            PAUSE_REFRESH_ENf, 0);
       }
       soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                         PAUSE_XOFF_TIMERf, control->xoff_timer);
    }

    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, TX_PAUSE_ENf,
                      control->tx_enable);
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, RX_PAUSE_ENf,
                      control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pause_control_get(int unit, soc_port_t port,
                            portmod_pause_control_t *control)
{
    uint64 rval64;
    uint32 refresh_enable;
    uint32 refresh_timer;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    refresh_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                       PAUSE_REFRESH_ENf);
    refresh_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                      PAUSE_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable? refresh_timer: -1);
    control->xoff_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                rval64, PAUSE_XOFF_TIMERf);

    control->tx_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                           TX_PAUSE_ENf);
    control->rx_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                           RX_PAUSE_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pfc_config_set (int unit, int port,
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 rval;
    uint32 fval[2] = {0};
    uint64 rval64;
    uint64 fval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_TYPEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_TYPEr, &rval, PFC_ETH_TYPEf,
                      pfc_cfg->type);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_TYPEr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_OPCODEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_OPCODEr, &rval, PFC_OPCODEf,
                      pfc_cfg->opcode);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_OPCODEr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_DAr(unit, port, &rval64));
    fval[0] |= pfc_cfg->da_nonoui;
    fval[0] |= (pfc_cfg->da_oui & 0xff) << 24;
    fval[1] |= (pfc_cfg->da_oui & 0xffff00) >> 8;

    COMPILER_64_SET(fval64, fval[1], fval[0]);
    soc_reg64_field_set(unit, CDMAC_PFC_DAr, &rval64, PFC_MACDAf, fval64);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_DAr(unit, port, rval64));

    return (SOC_E_NONE);
}

int cdmac_pfc_config_get (int unit, int port, portmod_pfc_config_t* pfc_cfg)
{
    uint32 rval;
    uint32 fval[2] = {0};
    uint64 rval64;
    uint64 fval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_TYPEr(unit, port, &rval));
    pfc_cfg->type = soc_reg_field_get(unit, CDMAC_PFC_TYPEr, rval,
                                      PFC_ETH_TYPEf);

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_OPCODEr(unit, port, &rval));
    pfc_cfg->opcode = soc_reg_field_get(unit, CDMAC_PFC_OPCODEr, rval,
                                        PFC_OPCODEf);

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_DAr(unit, port, &rval64));
    fval64 = soc_reg64_field_get(unit, CDMAC_PFC_DAr, rval64, PFC_MACDAf);
    fval[0] = COMPILER_64_LO(fval64);
    fval[1] = COMPILER_64_HI(fval64);

    pfc_cfg->da_nonoui = fval[0] & 0xffffff;
    pfc_cfg->da_oui = (fval[1] << 8) | (fval[0] >> 24);

    return (SOC_E_NONE);
}


int cdmac_pass_control_frame_set(int unit, int port, int value)
{
    uint32 rval;

    /* 
     * This configuration is used to drop or pass all control frames
     * (with ether type 0x8808) except pause packets.
     * If set, all control frames are passed to system side.
     * if reset, control frames (including pfc frames wih ether type 0x8808)i
     * are dropped in CDMAC.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval,
                      RX_PASS_CTRLf, (value? 1: 0));
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_pass_control_frame_get(int unit, int port, int *value)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RX_PASS_CTRLf);

    return (SOC_E_NONE);
}


int cdmac_pass_pfc_frame_set(int unit, int port, int value)
{
    uint32 rval;

    /*
     * This configuration is used to pass or drop PFC packets when 
     * PFC_ETH_TYPE is not equal to 0x8808.
     * If set, PFC frames are passed to system side.
     * If reset, PFC frames are dropped in CDMAC.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval,
                      RX_PASS_PFCf, (value? 1: 0));
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_pass_pfc_frame_get(int unit, int port, int *value)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RX_PASS_PFCf);

    return (SOC_E_NONE);
}


int cdmac_pass_pause_frame_set(int unit, int port, int value)
{
    uint32 rval;

    /*
     * If set, PAUSE frames are passed to sytem side.
     * If reset, PAUSE frames are dropped in CDMAC
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval,
                      RX_PASS_PAUSEf, (value? 1: 0));
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_pass_pause_frame_get(int unit, int port, int *value)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RX_PASS_PAUSEf);

    return (SOC_E_NONE);
}

/*
 * LAG FAILOVER
 * Following routines are used and invoked for the LAG
 * failover feature.
 * When a LAG(Trunk) member port goes down, if LAG failover
 * is enabled all the tx packets are loopback to RX path.
 * This is done by enabling loopback on the port which went
 * down, when the port comes back up, the software detects
 * the LINK UP event and removes the loopback.
 * Programming Sequence:
 * CDMAC_CTRL.LAG_FAILOVER_ENf 1/0, enable/disable feature.
 *                             If set, enable LAG Failover.
 * This bit has priority over LOCAL_LPBK. The lag failover
 * kicks in when the link status selected by LINK_STATUS_SELECT
 * transitions from 1 to 0. TSC clock and TSC credits must
 * be active for lag failover.
 *
 * portmod_port_trunk_hwfailover_config_set is the caller
 * of this function, which in turn is called by the bcm
 * application layer function calls handling the lag  hardware
 * failover feature.
 */
int cdmac_lag_failover_en_set(int unit, int port, int val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    /* need to disable MAC_LINK_DOWN_SEQ_ENf when faliover is enabled */
    if (val) {
        uint32  cdmac_tx_ctrl_rval;
        SOC_IF_ERROR_RETURN(READ_CDMAC_TX_CTRLr(unit, port, &cdmac_tx_ctrl_rval));
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, MAC_LINK_DOWN_SEQ_ENf, FALSE);
        /* need to reable rx and disable tx discard */
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));
        soc_reg_field_set(unit, CDMAC_TX_CTRLr, &cdmac_tx_ctrl_rval, DISCARDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, cdmac_tx_ctrl_rval));
    } else {
        /*once lag failover is disable, need to restore the MAC_LINK_DOWN_SEQ_EN bit */
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, MAC_LINK_DOWN_SEQ_ENf, TRUE);
    }

    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LAG_FAILOVER_ENf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_trunk_hwfailover_config_get is the caller
 * of this function, returns if the hw lag failover feature
 * is enabled on a port.
 */
int cdmac_lag_failover_en_get(int unit, int port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_CTRLr, rval, LAG_FAILOVER_ENf);

    return (SOC_E_NONE);
}

/*
 * This function is used to disable the LAG failover feature
 * on a port.
 * portmod_port_lag_failover_disable is the caller
 * of this function, which in turn is called by the bcm
 * application layer function calls handling the lag hardware
 * failover feature.
 */
int cdmac_lag_failover_disable(int unit, int port)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LAG_FAILOVER_ENf, 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LINK_STATUS_SELECTf, 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, REMOVE_FAILOVER_LPBKf, 0);

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_failover_loopback_set is the caller
 * of this function.
 */
int cdmac_lag_failover_loopback_set(int unit, int port, int val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_LAG_FAILOVER_STATUSr(unit, port, &rval));
    /* If set, indicates that the port is in lag failover state */
    soc_reg_field_set(unit, CDMAC_LAG_FAILOVER_STATUSr, &rval,
                      LAG_FAILOVER_LOOPBACKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_LAG_FAILOVER_STATUSr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_failover_loopback_get is the caller
 * of this function. This function is invoked as part of
 * LINK events from LINKSCAN thread
 */
int cdmac_lag_failover_loopback_get(int unit, int port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_LAG_FAILOVER_STATUSr(unit, port, &rval));
    /*
     * The LAG_FAILOVER_LOOPBACK is SET by the hardware if
     * the LINK goes down and LAG_FAILOVER_ENf is set on the port.
     */
    *val = soc_reg_field_get(unit, CDMAC_LAG_FAILOVER_STATUSr, rval,
                             LAG_FAILOVER_LOOPBACKf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_remove_failover_lpbk_set is the caller
 * of this function. This function is invoked as part of
 * LINK events from LINKSCAN thread
 */
int cdmac_lag_remove_failover_lpbk_set(int unit, int port, int val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    /*
     * If set, CDMAC port will move from lag failover state to
     * normal operation. This bit should be set after link is up.
     * A transition from 0 to 1 is required, so first a write with
     * 0 should be followed by a write with 1.
     * The BCM layer should take care of the calling sequnce as
     * part of LINK events.
     */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, REMOVE_FAILOVER_LPBKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_remove_failover_lpbk_get is the caller
 * of this function.
 */
int cdmac_lag_remove_failover_lpbk_get(int unit, int port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_CTRLr, rval, REMOVE_FAILOVER_LPBKf);

    return (SOC_E_NONE);
}

int cdmac_reset_fc_timers_on_link_dn_get(int unit, soc_port_t port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_trunk_hwfailover_config_set is the caller of this
 * function. This function is invoked during lag failover config
 * on a port.
 */
int cdmac_reset_fc_timers_on_link_dn_set(int unit, soc_port_t port, int val)
{
    uint32 rval;

    /*
     * If set, the receive pause and PFC timers are reset whenever the link
     * status is down, or local or remote faults are received.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval,
                      RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_mac_ctrl_set(int unit, int port, uint64 ctrl)
{
    uint32 rval;

    COMPILER_64_TO_32_LO(rval, ctrl);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));
    return (SOC_E_NONE);
}

int cdmac_drain_cell_get(int unit, int port,
                         portmod_drain_cells_t *drain_cells)
{
    uint32 rval;
    uint64 rval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_CTRLr(unit, port, &rval));
    drain_cells->rx_pfc_en = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                               RX_PFC_ENf);

    SOC_IF_ERROR_RETURN(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));
    drain_cells->rx_pause = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                  rval64, RX_PAUSE_ENf);
    drain_cells->tx_pause = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                  rval64, TX_PAUSE_ENf);

    return (SOC_E_NONE);
}


int cdmac_discard_set(int unit, soc_port_t port, int discard)
{
    uint32 rval;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, DISCARDf, discard);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_drain_cell_stop(int unit, int port,
                           const portmod_drain_cells_t *drain_cells)
{
    uint32 rval;
    uint64 rval64;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(cdmac_discard_set(unit, port, 0));

    /* set pause fields */
    SOC_IF_ERROR_RETURN(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, RX_PAUSE_ENf,
                      drain_cells->rx_pause);
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, TX_PAUSE_ENf,
                      drain_cells->tx_pause);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));

    /* set pfc rx_en fields */
    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, RX_PFC_ENf,
                      drain_cells->rx_pfc_en);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_drain_cell_start(int unit, int port)
{
    uint32 rval;
    uint64 rval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    /* Disable RX */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 0);

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, RX_PAUSE_ENf,0);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, RX_PFC_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(cdmac_discard_set(unit, port, 1));

    return (SOC_E_NONE);
}


int cdmac_txfifo_cell_cnt_get(int unit, int port, uint32* val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_VERSION_IDr(unit, port, &rval));

    if ((rval == CDMAC_VERSION_PM8X50_GEN2) || (rval == CDMAC_VERSION_PM8X50_FLEXE) ||
                    (rval == CDMAC_VERSION_PM8X50_GEN2_7NM_B0)) {
        if (SOC_REG_IS_VALID(unit, CDMAC_TXFIFO_STATUSr)) {
            SOC_IF_ERROR_RETURN(READ_CDMAC_TXFIFO_STATUSr(unit, port, &rval));
            *val = soc_reg_field_get(unit, CDMAC_TXFIFO_STATUSr, rval, CELL_CNTf);
        } else {
            return (SOC_E_NOT_FOUND);
        }
    } else {
        if (SOC_REG_IS_VALID(unit, CDMAC_TXFIFO_CELL_CNTr)) {
            SOC_IF_ERROR_RETURN(READ_CDMAC_TXFIFO_CELL_CNTr(unit, port, &rval));
            *val = soc_reg_field_get(unit, CDMAC_TXFIFO_CELL_CNTr, rval, CELL_CNTf);
        } else {
            return (SOC_E_NOT_FOUND);
        }
    }

    return (SOC_E_NONE);
}

int cdmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl,
                                 int *rx_en)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    COMPILER_64_SET(*mac_ctrl, 0, rval);

    *rx_en = soc_reg_field_get(unit, CDMAC_CTRLr, rval, RX_ENf);

    return (SOC_E_NONE);
}


int cdmac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint32 rval, orig_rval;
    uint32 soft_reset = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    orig_rval = rval;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, rx_en ? 1: 0);

    if (rval == orig_rval) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing scenario, CDMAC_CTRLr.SOFT_RESETf
         *     is used to early check to see if this port is at disabled state
         *     already.
         */
        soft_reset = soc_reg_field_get(unit, CDMAC_CTRLr, rval, SOFT_RESETf);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (rx_en) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, 0);
    }

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint32 rval;
    uint32 drain_timeout, txfifo_cell_cnt;
    soc_timeout_t to;

    drain_timeout = SAL_BOOT_QUICKTURN ? 250000000 : 250000;

    /* Enable RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    if (rx_en) {
        uint32 cdmac_tx_ctrl_rval;

        SOC_IF_ERROR_RETURN(READ_CDMAC_TX_CTRLr(unit, port, &cdmac_tx_ctrl_rval));

        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

        soc_reg_field_set(unit, CDMAC_TX_CTRLr, &cdmac_tx_ctrl_rval, DISCARDf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, cdmac_tx_ctrl_rval));

        /* Wait until TX fifo cell count is 0 */
        if (!SAL_BOOT_SIMULATION){
            soc_timeout_init(&to, drain_timeout, 0);
            for (;;) {
                SOC_IF_ERROR_RETURN(cdmac_txfifo_cell_cnt_get(unit, port, &txfifo_cell_cnt));
                if (txfifo_cell_cnt == 0) {
                    break;
                }
                if (soc_timeout_check(&to)) {
                    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                                (BSL_META_U(unit,
                                    "cdmac_egress_queue_drain_rx_en: unit %d "
                                    "ERROR: port %s: "
                                    "timeout draining TX FIFO (%d cells remain)\n"),
                                    unit, SOC_PORT_NAME(unit, port), txfifo_cell_cnt));
                    return (SOC_E_INTERNAL);
                }
            }
        }

        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

        soc_reg_field_set(unit, CDMAC_TX_CTRLr, &cdmac_tx_ctrl_rval, DISCARDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, cdmac_tx_ctrl_rval));
        sal_usleep(10000);
    } else {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));
    }

    /*Bring mac out of reset */
    SOC_IF_ERROR_RETURN(cdmac_soft_reset_set(unit, port, 0));

    return (SOC_E_NONE);
}

int cdmac_reset_check(int unit, int port, int enable, int *reset)
{
    uint32 rval, orig_rval;

    *reset = 1;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    orig_rval = rval;

    /* In cdmac_enable_set, we never disable TX since it stops egress
     * and hangs if CPU sends.
     * However, TX_EN will be disabled in bcmi_th3_port_disable
     * as part of port disable sequence within SDK.
     * As a result, in this functoin, if enable==0, the expected
     * register value will be TX_EN==0 && RX_EN==0.
     */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, enable? 1: 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, enable? 1: 0);

    if (rval == orig_rval) {
        if (enable) {
            *reset = 0;
        } else {
            if (soc_reg_field_get(unit, CDMAC_CTRLr, rval, SOFT_RESETf)) {
                *reset = 0;
            }
        }
    }

    return (SOC_E_NONE);
}

/*
 * portmod_port_trunk_hwfailover_config_set is the caller of this
 * function. This function is invoked during lag failover config
 * on a port.
 */
int cdmac_sw_link_status_select_set(int unit, soc_port_t port, int enable)
{
    uint32 rval;

    /*
     * This configuration chooses between link status indication from software
     * (SW_LINK_STATUSf) or the hardware link status indication from the PCS i
     * to MAC. If set, hardware link status is used.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LINK_STATUS_SELECTf, enable);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, LINK_STATUS_SELECTf);

    return (SOC_E_NONE);
}

int cdmac_sw_link_status_set (int unit, soc_port_t port, int link)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * This is valid only if LINK_STATUS_SELECT is 0, which means the 
     * software drives the link status. If SW_LINK_STATUS is set,
     * it indicates that the link is active. The use case is if there is  
     * some other mechanism used for the link status determination other
     * than hardware.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SW_LINK_STATUSf, link);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_sw_link_status_get (int unit, soc_port_t port, int *link)
{
    uint32 rval;
    int link_status_sel = 0;
    SOC_INIT_FUNC_DEFS;

    /*
     * check if link status selection is software based, if yes return 
     * sw_link_status.
     */
    _SOC_IF_ERR_EXIT(
        cdmac_sw_link_status_select_get(unit, port, &link_status_sel));

    /* link status selection is software based */
    if (!link_status_sel) {
        _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
        *link = soc_reg_field_get(unit, CDMAC_CTRLr, rval, SW_LINK_STATUSf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_mib_counter_control_set(int unit, soc_port_t port,
                                  int enable, int clear)
{
    uint32 rval;

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
    SOC_IF_ERROR_RETURN(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval,
                      ENABLEf, enable);
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CNT_CLEARf, clear);

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, rval));

    /*
     * set CLEARf to 0, this operation is not mandatory, adding to 
     * remove ambiguity in interpretation if a read on this register
     */
    if (clear) {
        SOC_IF_ERROR_RETURN(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
        soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CNT_CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, rval));
    }

    return (SOC_E_NONE);
}

/*
 * portmod_port_cntmaxsize_set is caller of this function
 */
int cdmac_cntmaxsize_set(int unit, int port, int val)
{
    uint32 rval;
    int field_len = 0;
    SOC_INIT_FUNC_DEFS;

    field_len = soc_reg_field_length(unit, CDMAC_MIB_COUNTER_CTRLr,
                                     CNTMAXSIZEf);

    if ((!field_len) || (val > ((1 << field_len) - 1))) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("invalid field length(%d) or value(%d)"),
                          field_len, val));
    }
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
    _SOC_IF_ERR_EXIT(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CNTMAXSIZEf, val);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_cntmaxsize_get is caller of this function
 */
int cdmac_cntmaxsize_get(int unit, int port, int *val)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_MIB_COUNTER_CTRLr, rval, CNTMAXSIZEf);

exit:
    SOC_FUNC_RETURN;
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
int cdmac_rsv_mask_control_set(int unit, int port, uint32 flags, uint32 value)
{

    int i = 0;
    uint32 tmp_mask = CDMAC_RSV_MASK_MIN;
    uint32 rsv_mask = 0;
    SOC_INIT_FUNC_DEFS;

    if (flags > CDMAC_RSV_MASK_ALL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_get(unit, port, &rsv_mask));

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

    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_set(unit, port, rsv_mask));

exit:
   SOC_FUNC_RETURN;
}

int cdmac_rsv_mask_control_get(int unit, int port, uint32 flags, uint32 *value)
{

    uint32 rsv_mask;
    SOC_INIT_FUNC_DEFS;

    /* Check if only 1 bit is set in flags */
    if ((flags > CDMAC_RSV_MASK_MAX) || ((flags) & (flags - 1))) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(cdmac_rsv_mask_get(unit, port, &rsv_mask));

    /*
     * if bit in rsv_mask = 0 means Enable, return 1.
     * if bit in rsv_mask = 1 means Purge, return 0.
     */
    *value = (rsv_mask & flags)? 0: 1;

exit:
   SOC_FUNC_RETURN;
}

int cdmac_rsv_mask_set(int unit, int port, uint32 rsv_mask)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    if (rsv_mask > CDMAC_RSV_MASK_ALL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), rsv_mask));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_RSV_MASKr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RSV_MASKr, &rval, MASKf, rsv_mask);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RSV_MASKr(unit, port, rsv_mask));

exit:
   SOC_FUNC_RETURN;
}

int cdmac_rsv_mask_get(int unit, int port, uint32 *rsv_mask)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RSV_MASKr(unit, port, &rval));
    *rsv_mask = soc_reg_field_get(unit, CDMAC_RSV_MASKr, rval, MASKf);

exit:
   SOC_FUNC_RETURN;
}

int cdmac_link_down_sequence_enable_set(int unit, int port, uint32 value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, MAC_LINK_DOWN_SEQ_ENf,
                      (value)? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_link_down_sequence_enable_get(int unit, int port, uint32 *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_CTRLr, rval,
                               MAC_LINK_DOWN_SEQ_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_INTR_ENABLEr(unit, port, &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval, 
                                       TX_PKT_UNDERFLOWf);
            break;
        case portmodIntrTypeTxPktOverflow :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval, 
                                       TX_PKT_OVERFLOWf);
            break;
        case portmodIntrTypeTxCdcSingleBitErr :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       TX_CDC_SINGLE_BIT_ERRf);
            break;
        case portmodIntrTypeTxCdcDoubleBitErr :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       TX_CDC_DOUBLE_BIT_ERRf);
            break;
        case portmodIntrTypeLocalFaultStatus :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       LOCAL_FAULT_STATUSf);
            break;
        case portmodIntrTypeRemoteFaultStatus :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       REMOTE_FAULT_STATUSf);
            break;
        case portmodIntrTypeLinkInterruptionStatus :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       LINK_INTERRUPTION_STATUSf);
            break;
        case portmodIntrTypeMibMemSingleBitErr :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       MIB_COUNTER_SINGLE_BIT_ERRf);
            break;
        case portmodIntrTypeMibMemDoubleBitErr :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       MIB_COUNTER_DOUBLE_BIT_ERRf);
            break;
        case portmodIntrTypeMibMemMultipleBitErr :
            *value = soc_reg_field_get(unit, CDMAC_INTR_ENABLEr, rval,
                                       MIB_COUNTER_MULTIPLE_ERRf);
            break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                  (_SOC_MSG("Invalid interrupt type")));
                break;
     }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_INTR_ENABLEr(unit, port, &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval, 
                              TX_PKT_UNDERFLOWf, value);
            break;
        case portmodIntrTypeTxPktOverflow :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval, 
                              TX_PKT_OVERFLOWf, value);
            break;
        case portmodIntrTypeTxCdcSingleBitErr :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              TX_CDC_SINGLE_BIT_ERRf, value);
            break;
        case portmodIntrTypeTxCdcDoubleBitErr :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              TX_CDC_DOUBLE_BIT_ERRf, value);
            break;
        case portmodIntrTypeLocalFaultStatus :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              LOCAL_FAULT_STATUSf, value);
            break;
        case portmodIntrTypeRemoteFaultStatus :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              REMOTE_FAULT_STATUSf, value);
            break;
        case portmodIntrTypeLinkInterruptionStatus :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              LINK_INTERRUPTION_STATUSf, value);
            break;
        case portmodIntrTypeMibMemSingleBitErr :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              MIB_COUNTER_SINGLE_BIT_ERRf, value);
            break;
        case portmodIntrTypeMibMemDoubleBitErr :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              MIB_COUNTER_DOUBLE_BIT_ERRf, value);
            break;
        case portmodIntrTypeMibMemMultipleBitErr :
            soc_reg_field_set(unit, CDMAC_INTR_ENABLEr, &rval,
                              MIB_COUNTER_MULTIPLE_ERRf, value);
            break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                                  (_SOC_MSG("Invalid interrupt type")));
                break;
     }

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_INTR_ENABLEr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value)
{
    uint32 rval;
    uint32 read_ecc_status = FALSE;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(READ_CDMAC_FIFO_STATUSr(unit, port, &rval));
            *value = soc_reg_field_get(unit, CDMAC_FIFO_STATUSr, rval,
                                       TX_PKT_UNDERFLOWf);
            break;
        case portmodIntrTypeTxPktOverflow :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(READ_CDMAC_FIFO_STATUSr(unit, port, &rval));
            *value = soc_reg_field_get(unit, CDMAC_FIFO_STATUSr, rval,
                                       TX_PKT_OVERFLOWf);
            break;
        case portmodIntrTypeLocalFaultStatus :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
            *value = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                                       LOCAL_FAULT_STATUSf);
            break;
        case portmodIntrTypeRemoteFaultStatus :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
            *value = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                                       REMOTE_FAULT_STATUSf);
            break;
        case portmodIntrTypeLinkInterruptionStatus :
            /* Clear on Read Operation */
            _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
            *value = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                                       LINK_INTERRUPTION_STATUSf);
            break;
        case portmodIntrTypeTxCdcSingleBitErr :
            read_ecc_status = TRUE;
            field = TX_CDC_SINGLE_BIT_ERRf;
            break;
        case portmodIntrTypeTxCdcDoubleBitErr :
            read_ecc_status = TRUE;
            field = TX_CDC_DOUBLE_BIT_ERRf;
            break;
        case portmodIntrTypeMibMemSingleBitErr :
            read_ecc_status = TRUE;
            field = MIB_COUNTER_SINGLE_BIT_ERRf;
            break;
        case portmodIntrTypeMibMemDoubleBitErr :
            read_ecc_status = TRUE;
            field = MIB_COUNTER_DOUBLE_BIT_ERRf;
            break;
        case portmodIntrTypeMibMemMultipleBitErr :
            read_ecc_status = TRUE;
            field = MIB_COUNTER_MULTIPLE_ERRf;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Invalid interrupt type")));
            break;
    }

    /* Read ECC status register */
    if (read_ecc_status) {
        /* Clear on Read Operation */
        _SOC_IF_ERR_EXIT(READ_CDMAC_ECC_STATUSr(unit, port, &rval));

        *value = soc_reg_field_get(unit, CDMAC_ECC_STATUSr, rval, field);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_interrupts_status_get(int unit, int port, int arr_max_size,
                                uint32* intr_arr, uint32* size)
{
    uint32 cnt = 0;
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /* Clear on Read */
    _SOC_IF_ERR_EXIT(READ_CDMAC_INTR_STATUSr(unit, port, &rval));

    /* Read FIFO STATUS - Clear on Read Operation */
    _SOC_IF_ERR_EXIT(READ_CDMAC_FIFO_STATUSr(unit, port, &rval));

    if (soc_reg_field_get(unit, CDMAC_FIFO_STATUSr, rval, TX_PKT_UNDERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktUnderflow;
    }

    if (soc_reg_field_get(unit, CDMAC_FIFO_STATUSr, rval, TX_PKT_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktOverflow;
    }

    /* Read RX LSS STATUS register - Clear on Read Operation */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));

    if (soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                          LOCAL_FAULT_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLocalFaultStatus;
    }

    if (soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                          REMOTE_FAULT_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRemoteFaultStatus;
    }

    if (soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                          LINK_INTERRUPTION_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLinkInterruptionStatus;
    }

    /* Read ECC STATUS register - Clear on Read Operation */
    _SOC_IF_ERR_EXIT(READ_CDMAC_ECC_STATUSr(unit, port, &rval));

    if (soc_reg_field_get(unit, CDMAC_ECC_STATUSr, rval,
                          TX_CDC_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcSingleBitErr;
    }

    if (soc_reg_field_get(unit, CDMAC_ECC_STATUSr, rval,
                          TX_CDC_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcDoubleBitErr;
    }

    if (soc_reg_field_get(unit, CDMAC_ECC_STATUSr, rval,
                          MIB_COUNTER_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibMemSingleBitErr;
    }

    if (soc_reg_field_get(unit, CDMAC_ECC_STATUSr, rval,
                          MIB_COUNTER_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeMibMemDoubleBitErr;
    }

    if (soc_reg_field_get(unit, CDMAC_ECC_STATUSr, rval,
                          MIB_COUNTER_MULTIPLE_ERRf)) {
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

int cdmac_link_interrupt_ordered_set_enable(int unit, int port, uint32 enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));
    if (enable) {
        soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, FORCE_LINK_INTERRUPT_OSf, 1);
        soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, FAULT_SOURCE_FOR_TXf, 2);
    } else {
        soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, FORCE_LINK_INTERRUPT_OSf, 0);
        soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, FAULT_SOURCE_FOR_TXf, 0);
    }
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_link_interrupt_ordered_set_enable_get(int unit, int port, uint32* enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,  FORCE_LINK_INTERRUPT_OSf);

exit:
    SOC_FUNC_RETURN;
}


int cdmac_link_interruption_live_status_get(int unit, int port, uint32* status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
    /* fault status bits clear on read */
    *status = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
              LINK_INTERRUPTION_LIVE_STATUSf);
exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_stall_enable_set(int unit, int port, uint32 enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, STALL_TXf, enable);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}
#undef _ERR_MSG_MODULE_NAME

#endif
