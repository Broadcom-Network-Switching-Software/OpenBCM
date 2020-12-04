/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/clmac.h>
#include <soc/drv.h>

#if defined(PORTMOD_PM4X25_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define CLMAC_SPEED_100000 0x0
#define CLMAC_CRC_MODE_APPEND   0
#define CLMAC_CRC_MODE_REPLACE  2

#define CLMAC_AVERAGE_IPG_DEFAULT         12
#define CLMAC_AVERAGE_IPG_HIGIG           8

#define CLMAC_TX_PREAMBLE_LENGTH  8

#define CLMAC_JUMBO_MAXSZ 0x3fe8

#define CLMAC_RUNT_THRESHOLD_ETH    64
#define CLMAC_RUNT_THRESHOLD_HIGIG  72
#define CLMAC_RUNT_THRESHOLD_HIGIG2 76

/*
 *  WAN mode throttling
 *  THROT_10G_TO_5G: throt_num=21 throt_denom=21
 *  THROT_10G_TO_2P5G: throt_num=63 throt_denom=21
 */
#define CLMAC_THROT_10G_TO_5G    256
#define CLMAC_THROT_10G_TO_2P5G  257

#ifdef BCM_APACHE_SUPPORT

int soc_apache_clmac_gen2_ports[] = {
    0,                /* PHY port 0 is invalid              */
    0, 0, 0, 0,       /* PHY ports 1-4:   PM 4x10  inst 0   */
    0, 0, 0, 0,       /* PHY ports 5-8:   PM 4x10  inst 1   */
    0, 0, 0, 0,       /* PHY ports 9-12:  PM 4x10  inst 2   */
    0, 0, 0, 0,       /* PHY ports 13-16: PM 4x10  inst 3   */
    0, 0, 0, 0,       /* PHY ports 17-20: PM 12x10 inst 0.0 */
    0, 0, 0, 0,       /* PHY ports 21-24: PM 12x10 inst 0.1 */
    0, 0, 0, 0,       /* PHY ports 25-28: PM 12x10 inst 0.2 */
    1, 1, 1, 1,       /* PHY ports 29-32: PM 4x25  inst 0   */
    1, 1, 1, 1,       /* PHY ports 33-36: PM 4x25  inst 1   */
    0, 0, 0, 0,       /* PHY ports 37-40: PM 4x10  inst 4   */
    0, 0, 0, 0,       /* PHY ports 41-44: PM 4x10  inst 5   */
    0, 0, 0, 0,       /* PHY ports 45-48: PM 4x10  inst 6   */
    0, 0, 0, 0,       /* PHY ports 49-52: PM 4x10  inst 7   */
    0, 0, 0, 0,       /* PHY ports 53-56: PM 12x10 inst 1.0 */
    0, 0, 0, 0,       /* PHY ports 57-60: PM 12x10 inst 1.1 */
    0, 0, 0, 0,       /* PHY ports 61-64: PM 12x10 inst 1.2 */
    1, 1, 1, 1,       /* PHY ports 65-68: PM 4x25  inst 2   */
    1, 1, 1, 1,       /* PHY ports 69-72: PM 4x25  inst 3   */
};


int soc_apache_port_is_clg2_port(int unit, int port)
{
    if (soc_feature(unit, soc_feature_clport_gen2)) {
        return (soc_apache_clmac_gen2_ports[SOC_INFO(unit).port_l2p_mapping[port]]);
    } else {
        return 0;
    }
}


#define RD_CLMAC(name, unit, port, val) do { \
        if (soc_apache_port_is_clg2_port(unit, port)) { \
            SOC_IF_ERROR_RETURN(READ_CLG2MAC_##name(unit, port, val)); \
        } else { \
            SOC_IF_ERROR_RETURN(READ_CLMAC_##name(unit, port, val)); \
        } \
    } while(0)

#define WR_CLMAC(name, unit, port, val) do { \
        if (soc_apache_port_is_clg2_port(unit, port)) { \
            SOC_IF_ERROR_RETURN(WRITE_CLG2MAC_##name(unit, port, val)); \
        } else { \
            SOC_IF_ERROR_RETURN(WRITE_CLMAC_##name(unit, port, val)); \
        } \
    } while(0)


#else

#define RD_CLMAC(name, unit, port, val) SOC_IF_ERROR_RETURN(READ_CLMAC_##name(unit, port, val));
#define WR_CLMAC(name, unit, port, val) SOC_IF_ERROR_RETURN(WRITE_CLMAC_##name(unit, port, val));

#endif

STATIC
int _clmac_version_get(int unit, soc_port_t port, int32 *version)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    if (!SOC_REG_IS_VALID(unit, CLMAC_VERSION_IDr)) {
        *version = -1;
    }
    else {
        RD_CLMAC(VERSION_IDr, unit, port, &reg_val);
        *version = soc_reg64_field32_get(unit, CLMAC_VERSION_IDr, reg_val, CLMAC_VERSIONf);
    }
    SOC_FUNC_RETURN;
}

int clmac_version_get(int unit, soc_port_t port, int32 *version)
{
    SOC_INIT_FUNC_DEFS;

    _rv = _clmac_version_get(unit, port, version);

    SOC_FUNC_RETURN;
}

int clmac_init(int unit, soc_port_t port, uint32 init_flags)
{
    portmod_pause_control_t pause_control;
    portmod_pfc_control_t pfc_control;
    portmod_llfc_control_t llfc_control;
    portmod_remote_fault_control_t remote_fault_control;
    portmod_local_fault_control_t local_fault_control;
    int crc_mode, is_ipg_check_disable;
    int is_strip_crc, is_append_crc, is_replace_crc, is_higig, is_pass_through_crc;
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    is_strip_crc =          (init_flags & CLMAC_INIT_F_RX_STRIP_CRC                 ? 1 : 0);
    is_append_crc =         (init_flags & CLMAC_INIT_F_TX_APPEND_CRC                ? 1 : 0);
    is_replace_crc =        (init_flags & CLMAC_INIT_F_TX_REPLACE_CRC               ? 1 : 0);
    is_pass_through_crc =   (init_flags & CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE     ? 1 : 0);
    is_higig =              (init_flags & CLMAC_INIT_F_IS_HIGIG                     ? 1 : 0);
    is_ipg_check_disable =  (init_flags & CLMAC_INIT_F_IPG_CHECK_DISABLE            ? 1 : 0);

    if(is_append_crc + is_replace_crc + is_pass_through_crc > 1) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("CLAMC_INIT_F_TX_APPEND_CRC, CLAMC_INIT_F_TX_REPLACE_CRC, and CLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE can't co-exist")));
    }

    /* RX Control */
    RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRIP_CRCf, is_strip_crc);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRICT_PREAMBLEf, is_higig ? 0 : 1);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, CLMAC_RUNT_THRESHOLD_ETH);
    WR_CLMAC(RX_CTRLr, unit, port, reg_val);

    /* TX Control */
    if(is_append_crc) {
        crc_mode = 0;
    } else if(is_replace_crc) {
        crc_mode = 2;
    } else if(is_pass_through_crc) {
        crc_mode = 1;
    } else { /* CRC AUTO Mode */
        crc_mode = 3;
    }
    RD_CLMAC(TX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, CRC_MODEf, crc_mode);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, is_higig ? CLMAC_AVERAGE_IPG_HIGIG : CLMAC_AVERAGE_IPG_DEFAULT);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, TX_PREAMBLE_LENGTHf, CLMAC_TX_PREAMBLE_LENGTH);
    WR_CLMAC(TX_CTRLr, unit, port, reg_val);

    /*Pause*/
    _SOC_IF_ERR_EXIT(portmod_pause_control_t_init(unit, &pause_control));
    /* PAUSE */
    pause_control.tx_enable = TRUE;
    pause_control.rx_enable = TRUE;
    _SOC_IF_ERR_EXIT(clmac_pause_control_set(unit, port, &pause_control));
    
    /*PFC*/
    _SOC_IF_ERR_EXIT(clmac_pfc_control_get(unit, port, &pfc_control));
    pfc_control.rx_enable = 0;
    pfc_control.tx_enable = 0;
    pfc_control.stats_en = 0;
    _SOC_IF_ERR_EXIT(clmac_pfc_control_set(unit, port, &pfc_control));

    /*LLFC*/
    _SOC_IF_ERR_EXIT(portmod_llfc_control_t_init(unit, &llfc_control));
    llfc_control.rx_enable = 0;
    llfc_control.tx_enable = 0;
    _SOC_IF_ERR_EXIT(clmac_llfc_control_set(unit, port, &llfc_control));

    /*LSS*/
    _SOC_IF_ERR_EXIT(portmod_remote_fault_control_t_init(unit, &remote_fault_control));
    remote_fault_control.enable = 1;
    remote_fault_control.drop_tx_on_fault = 1;
    _SOC_IF_ERR_EXIT(clmac_remote_fault_control_set(unit, port, &remote_fault_control));

    _SOC_IF_ERR_EXIT(portmod_local_fault_control_t_init(unit, &local_fault_control));
    local_fault_control.enable = 1;
    local_fault_control.drop_tx_on_fault = 1;
    _SOC_IF_ERR_EXIT(clmac_local_fault_control_set(unit, port, &local_fault_control));

    /*MAC control*/
    RD_CLMAC(CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, RX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, SOFT_RESETf, 0); /*Deassert SOFT_RESET*/
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, XGMII_IPG_CHECK_DISABLEf, is_ipg_check_disable);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, ALLOW_40B_AND_GREATER_PKTSf, 1);
    WR_CLMAC(CTRLr, unit, port, reg_val);

    RD_CLMAC(RX_MAX_SIZEr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_MAX_SIZEr, &reg_val, RX_MAX_SIZEf, CLMAC_JUMBO_MAXSZ);
    WR_CLMAC(RX_MAX_SIZEr, unit, port, reg_val);


exit:
    SOC_FUNC_RETURN;
}

int clmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint64 reg_val, orig_reg_val;
    SOC_INIT_FUNC_DEFS;


    RD_CLMAC(CTRLr, unit, port, &reg_val);
    COMPILER_64_COPY(orig_reg_val, reg_val);

    if (flags & CLMAC_ENABLE_SET_FLAGS_TX_EN) {
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, TX_ENf, enable ? 1 : 0);
    } 
    if (flags & CLMAC_ENABLE_SET_FLAGS_RX_EN) {
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, RX_ENf, enable ? 1 : 0); 
    } 
    if (!(flags & CLMAC_ENABLE_SET_FLAGS_RX_EN) && !(flags & CLMAC_ENABLE_SET_FLAGS_TX_EN)) {
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, TX_ENf, enable ? 1 : 0);
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, RX_ENf, enable ? 1 : 0); 
    }
    if(COMPILER_64_NE(reg_val, orig_reg_val)) { /* write only if value changed */
        WR_CLMAC(CTRLr, unit, port, reg_val);
    }

    /*
     * check if mac soft reset operation is needed
     * if enable = 1, bring mac out of reset, else mac in reset.
     */
    if (!(flags & CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS)) {
        RD_CLMAC(CTRLr, unit, port, &reg_val);
        COMPILER_64_COPY(orig_reg_val, reg_val);
        soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val, SOFT_RESETf,
                               enable ? 0 : 1);
    }    

    if(COMPILER_64_NE(reg_val, orig_reg_val)) {/* write only if value changed */
       SOC_IF_ERROR_RETURN(clmac_soft_reset_set(unit, port, !enable));
    }

    SOC_FUNC_RETURN;
}

int clmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(CTRLr, unit, port, &reg_val);
    if (flags & CLMAC_ENABLE_SET_FLAGS_TX_EN) {
        *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, TX_ENf); 
    } else {
        *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, RX_ENf);
    }
    

    SOC_FUNC_RETURN;
}

int clmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val, 
                           SOFT_RESETf, enable? 1 : 0);
    WR_CLMAC(CTRLr, unit, port, reg_val);
    /*
     * Special handling for new mac version. Internally MAC loopback
     * looks for rising edge on MAC loopback configuration to enter
     * loopback state.
     * Do only if loopback is enabled on the port.
     */
    if (!enable &&
        soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, LOCAL_LPBKf)) {
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, LOCAL_LPBKf, 0);
        WR_CLMAC(CTRLr, unit, port, reg_val);
        /* Wait 10usec as suggested by MAC designer */
        sal_udelay(10);
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, LOCAL_LPBKf, 1);
        WR_CLMAC(CTRLr, unit, port, reg_val);
    }
    return (SOC_E_NONE);
}


int clmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr, unit, port, &reg_val);

    *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, SOFT_RESETf)?1:0;
    return (SOC_E_NONE);
}

int clmac_timestamp_byte_adjust_set(int unit, soc_port_t port, int flags, int speed)
{
    uint64 ctrl;
    uint32 byte_adjust = 0;
    SOC_INIT_FUNC_DEFS;

    if (100 == speed) {
        byte_adjust = 80;
    } else if (1000 == speed) {
        byte_adjust = 8;
    } else if (2500 == speed) {
        byte_adjust = 3;
    } else if (10000 == speed) {
        byte_adjust = 1;
    }

    if (SOC_REG_PORT_VALID(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, port)) {
        if ((speed > 10000) || (flags == CLMAC_TIMESTAMP_BYTE_ADJUST_CLEAR)) {
            /* This shouldn't be enabled when speed is more than 10G */
            RD_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              TX_TIMER_BYTE_ADJUSTf, 0);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              TX_TIMER_BYTE_ADJUST_ENf, 0);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              RX_TIMER_BYTE_ADJUSTf, 0);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              RX_TIMER_BYTE_ADJUST_ENf, 0);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);
        } else {
            RD_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              TX_TIMER_BYTE_ADJUSTf, byte_adjust);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);

            RD_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              TX_TIMER_BYTE_ADJUST_ENf, 1);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);

            RD_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              RX_TIMER_BYTE_ADJUSTf, byte_adjust);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);

            RD_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, &ctrl,
                              RX_TIMER_BYTE_ADJUST_ENf, 1);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);
        }
    } else {
        return (SOC_E_UNAVAIL);
    }

    SOC_FUNC_RETURN;
}

int clmac_timestamp_delay_set(int unit, soc_port_t port,
                                          portmod_port_ts_adjust_t ts_adjust)
{
    uint64 ctrl;
    int osts_delay = 0;
    int tsts_delay = 0;

    if (SOC_REG_IS_VALID(unit, CLMAC_TIMESTAMP_ADJUSTr)) {

#ifdef BCM_TIMESYNC_SUPPORT
        uint32 clk_rate, tx_clk_ns;
        /* use VCO  25.78125 as default, CLMAC clock is VCO/40*/
        clk_rate =  644;
        tx_clk_ns = 1000 / clk_rate;
#endif

        /* 6 TSC_CLK period + 1 TS_CLK period */
        if (ts_adjust.osts_adjust > 0) {
            osts_delay = ts_adjust.osts_adjust;
        }
#ifdef BCM_TIMESYNC_SUPPORT
        else {
            osts_delay = SOC_TIMESYNC_PLL_CLOCK_NS(unit) + ((12 * tx_clk_ns ) / 2);
        }
#endif
        RD_CLMAC(TIMESTAMP_ADJUSTr, unit, port, &ctrl);
        soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_OSTS_ADJUSTf, osts_delay);
        WR_CLMAC(TIMESTAMP_ADJUSTr, unit, port, ctrl);

        /* (2.5 TSC_CLK period + 1 TS_CLK period */
        if (ts_adjust.tsts_adjust > 0) {
            tsts_delay = ts_adjust.tsts_adjust;
        }
#ifdef BCM_TIMESYNC_SUPPORT
        else {
            tsts_delay = SOC_TIMESYNC_PLL_CLOCK_NS(unit) + ((5 * tx_clk_ns ) / 2);
        }
#endif
        RD_CLMAC(TIMESTAMP_ADJUSTr, unit, port, &ctrl);
        soc_reg64_field32_set(unit, CLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_TSTS_ADJUSTf, tsts_delay);
        WR_CLMAC(TIMESTAMP_ADJUSTr, unit, port, ctrl);
    }


    return SOC_E_NONE;
}

int clmac_timestamp_delay_get(int unit, soc_port_t port,
                              portmod_port_ts_adjust_t *ts_adjust)
{
    uint64 reg_val;

    if (SOC_REG_IS_VALID(unit, CLMAC_TIMESTAMP_ADJUSTr)) {
        RD_CLMAC(TIMESTAMP_ADJUSTr, unit, port, &reg_val);
        ts_adjust->osts_adjust = soc_reg64_field32_get (unit, CLMAC_TIMESTAMP_ADJUSTr, reg_val,
                                     TS_OSTS_ADJUSTf);
        ts_adjust->tsts_adjust = soc_reg64_field32_get (unit, CLMAC_TIMESTAMP_ADJUSTr, reg_val,
                                     TS_TSTS_ADJUSTf);
    } else {
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

int clmac_speed_set(int unit, soc_port_t port, int flags, int speed)
{
    uint64 reg_val, orig_reg_val;
    int enable, enable_flags = 0;
    uint32 rval = 0, fault = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(orig_reg_val);

    SOC_IF_ERROR_RETURN(clmac_enable_get(unit, port, 0, &enable));

    /* disable before updating the speed */	
    if (enable) {
        if (flags & CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(clmac_enable_set(unit, port, enable_flags, 0));
    }

    RD_CLMAC(MODEr, unit, port, &reg_val);
    COMPILER_64_ADD_64(orig_reg_val, reg_val);
    soc_reg64_field32_set(unit, CLMAC_MODEr, &reg_val, SPEED_MODEf, ((speed>= 10000) || (speed==0)) ? 4 : 2); 

    if(COMPILER_64_NE(orig_reg_val, reg_val)) {
        WR_CLMAC(MODEr, unit, port, reg_val);
    }

    /*
     * Set REMOTE_FAULT/LOCAL_FAULT for CL ports,
     * else HW Linkscan interrupt would be suppressed.
     */
    if (SOC_REG_IS_VALID(unit, CLPORT_FAULT_LINK_STATUSr)) {
        fault = speed <= 1000 ? 0 : 1;

        SOC_IF_ERROR_RETURN(READ_CLPORT_FAULT_LINK_STATUSr(unit, port, &rval));
        soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &rval,
                          REMOTE_FAULTf, fault);
        soc_reg_field_set(unit, CLPORT_FAULT_LINK_STATUSr, &rval,
                          LOCAL_FAULTf, fault);
        SOC_IF_ERROR_RETURN(WRITE_CLPORT_FAULT_LINK_STATUSr(unit, port, rval));
    }

    if (enable) {
        if (flags & CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(clmac_enable_set(unit, port, enable_flags, 1));
    }

    SOC_FUNC_RETURN;
}

int clmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb, int enable)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    switch(lb){
        case portmodLoopbackMacOuter:
            RD_CLMAC(CTRLr, unit, port, &reg_val);
            soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, LOCAL_LPBKf, enable ? 1: 0);
            WR_CLMAC(CTRLr, unit, port, reg_val);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unsupported loopback type %d"), lb));
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb, int *enable)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    switch(lb){
        case portmodLoopbackMacOuter:
            RD_CLMAC(CTRLr, unit, port, &reg_val);
            *enable = soc_reg64_field32_get(unit, CLMAC_CTRLr, reg_val, LOCAL_LPBKf);
            break;
        default:
            /* set to 0 - this loopback is not in use for this MAC */
            *enable = 0;
            break;
    }

    SOC_FUNC_RETURN;
}

int clmac_tx_enable_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr , unit, port, &reg_val);

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val,
                           TX_ENf, enable ? 1: 0);
    WR_CLMAC(CTRLr, unit, port, reg_val);

    return(SOC_E_NONE);
}


int clmac_tx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_CTRLr(unit, port, &reg_val));

    *enable = soc_reg64_field32_get (unit, CLMAC_CTRLr, reg_val,
                                     TX_ENf);

    return(SOC_E_NONE);
}

int clmac_rx_enable_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr , unit, port, &reg_val);

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val,
                           RX_ENf, enable ? 1: 0);
    WR_CLMAC(CTRLr, unit, port, reg_val);

    return(SOC_E_NONE);
}

int clmac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr, unit, port, &reg_val);

    *enable = soc_reg64_field32_get (unit, CLMAC_CTRLr, reg_val,
                                     RX_ENf);

    return(SOC_E_NONE);
}

/*
 * Function:
 *      _clmac_port_mode_update
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
_clmac_port_mode_update(int unit, soc_port_t port, int hg_mode)
{
    uint64 tx_ctrl, rx_ctrl, mac_ctrl, mac_ctrl_orig;

    RD_CLMAC(CTRLr, unit, port, &mac_ctrl);
    RD_CLMAC(RX_CTRLr, unit, port, &rx_ctrl);
    RD_CLMAC(TX_CTRLr, unit, port, &tx_ctrl);

    mac_ctrl_orig = mac_ctrl;

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);

    WR_CLMAC(CTRLr, unit, port, mac_ctrl);

    if (hg_mode != SOC_ENCAP_IEEE) { /* HiGig Mode */
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl_orig, XGMII_IPG_CHECK_DISABLEf, 1);
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, CLMAC_AVERAGE_IPG_HIGIG);
    } else {
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl_orig, XGMII_IPG_CHECK_DISABLEf, 0);
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, CLMAC_AVERAGE_IPG_DEFAULT);
    }

    WR_CLMAC(RX_CTRLr, unit, port, rx_ctrl);
    WR_CLMAC(TX_CTRLr, unit, port, tx_ctrl);
    WR_CLMAC(CTRLr, unit, port, mac_ctrl_orig); /* restore mac ctrl */

    return (SOC_E_NONE);
}

int clmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap)
{
    uint32 val = 0;
    uint64 reg_val;
    uint32 no_sop_for_crc_hg = 0;
    int enable, enable_flags = 0;
    SOC_INIT_FUNC_DEFS;

    switch(encap){
        case SOC_ENCAP_IEEE:
            val = 0;
            break;
        case SOC_ENCAP_HIGIG:
            val = 1;
            break;
        case SOC_ENCAP_HIGIG2:
            val = 2;
            break;
        case SOC_ENCAP_SOP_ONLY:
            val = 5;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("illegal encap mode %d"), encap));
    }

    SOC_IF_ERROR_RETURN(clmac_enable_get(unit, port, 0, &enable));

    if (enable) {
        if (flags & CLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(clmac_enable_set(unit, port, enable_flags, 0));
    }

    if ((IS_E_PORT(unit, port) && encap != SOC_ENCAP_IEEE) ||
            (IS_ST_PORT(unit, port) && encap == SOC_ENCAP_IEEE)) {

        SOC_IF_ERROR_RETURN(_clmac_port_mode_update(unit, port, encap));
    }

    if((encap == SOC_ENCAP_HIGIG) || (encap == SOC_ENCAP_HIGIG2)){
        if(flags & CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG){
            no_sop_for_crc_hg = 1;
        }
    }

    RD_CLMAC(MODEr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_MODEr, &reg_val, HDR_MODEf, val);
    soc_reg64_field32_set(unit, CLMAC_MODEr, &reg_val, NO_SOP_FOR_CRC_HGf, no_sop_for_crc_hg);
    WR_CLMAC(MODEr, unit, port, reg_val);

    if(encap == SOC_ENCAP_HIGIG2){
        RD_CLMAC(CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, EXTENDED_HIG2_ENf, (flags & CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN) ? 1 : 0 );
        WR_CLMAC(CTRLr, unit, port, reg_val);

        RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, CLMAC_RUNT_THRESHOLD_HIGIG2);
        WR_CLMAC(RX_CTRLr, unit, port, reg_val);
    } else if (encap == SOC_ENCAP_HIGIG) {
        RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, CLMAC_RUNT_THRESHOLD_HIGIG);
        WR_CLMAC(RX_CTRLr, unit, port, reg_val);
    } else {
        RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, CLMAC_RUNT_THRESHOLD_ETH);
        WR_CLMAC(RX_CTRLr, unit, port, reg_val);
    }

    if (enable) {
        if (flags & CLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(clmac_enable_set(unit, port, enable_flags, 1));
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_encap_get(int unit, soc_port_t port, int *flags, portmod_encap_t *encap){
    uint64 reg_val;
    uint32 fld_val;
    SOC_INIT_FUNC_DEFS;

    (*flags) = 0;

    RD_CLMAC(MODEr, unit, port, &reg_val);
    fld_val = soc_reg64_field32_get(unit, CLMAC_MODEr, reg_val, HDR_MODEf);

    switch(fld_val){
        case 0:
            *encap = SOC_ENCAP_IEEE;
            break;
        case 1:
            *encap = SOC_ENCAP_HIGIG;
            break;
        case 2:
            *encap = SOC_ENCAP_HIGIG2;
            break;
        case 5:
            *encap = SOC_ENCAP_SOP_ONLY;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("unknown encap mode %d"), fld_val));
            break;
    }

    if((*encap == SOC_ENCAP_HIGIG) || (*encap == SOC_ENCAP_HIGIG2)){
        fld_val = soc_reg64_field32_get(unit, CLMAC_MODEr, reg_val,
                                        NO_SOP_FOR_CRC_HGf);
        if(fld_val) {
            (*flags) |= CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG;
        }
    }

    if(*encap == SOC_ENCAP_HIGIG2){
        RD_CLMAC(CTRLr, unit, port, &reg_val);
        fld_val = soc_reg64_field32_get(unit, CLMAC_CTRLr,
                                        reg_val, EXTENDED_HIG2_ENf);
        if(fld_val) {
            (*flags) |= CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_MAX_SIZEr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_MAX_SIZEr, &reg_val, RX_MAX_SIZEf, value);
    WR_CLMAC(RX_MAX_SIZEr, unit, port, reg_val);

    SOC_FUNC_RETURN;
}

int clmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_MAX_SIZEr, unit, port, &reg_val);
    *value = soc_reg64_field32_get(unit, CLMAC_RX_MAX_SIZEr, reg_val, RX_MAX_SIZEf);

    SOC_FUNC_RETURN;
}

int clmac_pad_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(TX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, PAD_ENf, value ? 1 : 0);
    if(value){
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, PAD_THRESHOLDf, value);
    }
    WR_CLMAC(TX_CTRLr, unit, port, reg_val);

    SOC_FUNC_RETURN;
}

int clmac_pad_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    int pad_en;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(TX_CTRLr, unit, port, &reg_val);
    pad_en = soc_reg64_field32_get(unit, CLMAC_TX_CTRLr, reg_val, PAD_ENf);
    if(!pad_en){
        *value = 0;
    }
    else{
        *value = soc_reg64_field32_get(unit, CLMAC_TX_CTRLr, reg_val, PAD_THRESHOLDf);
    }

    SOC_FUNC_RETURN;
}

int clmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    if ((value < CLMAC_RUNT_THRESHOLD_MIN) ||
        (value > CLMAC_RUNT_THRESHOLD_MAX)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
            (_SOC_MSG("runt threshold should be the range of %d - %d. got %d"),
             CLMAC_RUNT_THRESHOLD_MIN, CLMAC_RUNT_THRESHOLD_MAX, value));
    }

    RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, value);
    WR_CLMAC(RX_CTRLr, unit, port, reg_val);

exit:
    SOC_FUNC_RETURN;
}

int clmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
    *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr, reg_val, RUNT_THRESHOLDf);

    SOC_FUNC_RETURN;
}

int clmac_strict_preamble_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    RD_CLMAC(RX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &reg_val, STRICT_PREAMBLEf, value);
    WR_CLMAC(RX_CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int clmac_strict_preamble_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_RX_CTRLr(unit, port, &reg_val));
    *value = (int)soc_reg64_field32_get (unit, CLMAC_RX_CTRLr,
                                         reg_val, STRICT_PREAMBLEf);
    return (SOC_E_NONE);
}

int clmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_VLAN_TAGr, unit, port, &reg_val);

    if(inner_vlan_tag == -1){
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAG_ENABLEf, 0);
    } else{
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAGf, inner_vlan_tag);
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, INNER_VLAN_TAG_ENABLEf, 1);

    }

    if(outer_vlan_tag == -1){
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAG_ENABLEf, 0);
    } else {
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAGf, outer_vlan_tag);
        soc_reg64_field32_set(unit, CLMAC_RX_VLAN_TAGr, &reg_val, OUTER_VLAN_TAG_ENABLEf, 1);
    }

    WR_CLMAC(RX_VLAN_TAGr, unit, port, reg_val);

    SOC_FUNC_RETURN;
}


int clmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_VLAN_TAGr, unit, port, &reg_val);
    field_val = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, INNER_VLAN_TAG_ENABLEf);

    if(field_val == 0){
        *inner_vlan_tag = -1;
    } else {
        *inner_vlan_tag = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, INNER_VLAN_TAGf);
    }

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, OUTER_VLAN_TAG_ENABLEf);
    if(field_val == 0){
        *outer_vlan_tag = -1;
    } else {
        *outer_vlan_tag = soc_reg64_field32_get(unit, CLMAC_RX_VLAN_TAGr, reg_val, OUTER_VLAN_TAGf);
    }

    SOC_FUNC_RETURN;
}

int clmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    /* high-order 8-bits of field value corresponds to mac[0],
       low-order 8-bits of field value corresponds to mac[5] */ 
    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[0]), _shr_uint32_read(&mac[2]));
    
    WR_CLMAC(RX_MAC_SAr, unit, port, mac_addr);

    SOC_FUNC_RETURN;
}


int clmac_sw_link_status_select_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, CLMAC_CTRLr, &reg_val, 
                           LINK_STATUS_SELECTf, enable);

    WR_CLMAC(CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int clmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr, unit, port, &reg_val);

    *enable = (int)soc_reg64_field32_get (unit, CLMAC_CTRLr, reg_val, 
                                          LINK_STATUS_SELECTf);
    return (SOC_E_NONE);
}

int clmac_sw_link_status_set(int unit, soc_port_t port, int link)
{
    uint64 reg_val;

    RD_CLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &reg_val, SW_LINK_STATUSf, link);
    WR_CLMAC(CTRLr, unit, port, reg_val);
    return (SOC_E_NONE);
}
int clmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 values[2];
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_MAC_SAr, unit, port, &rval64);
    field64 = soc_reg64_field_get(unit, CLMAC_RX_MAC_SAr, rval64, RX_SAf);

    values[0] = COMPILER_64_HI(field64);
    values[1] = COMPILER_64_LO(field64);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

    SOC_FUNC_RETURN;
}


int clmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[0]), _shr_uint32_read(&mac[2]));
    
    WR_CLMAC(TX_MAC_SAr, unit, port, mac_addr);
   
    SOC_FUNC_RETURN;
}

int clmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(TX_MAC_SAr, unit, port, &rval64);
    values[0] = soc_reg64_field32_get(unit, CLMAC_TX_MAC_SAr, rval64, SA_HIf);
    values[1] = soc_reg64_field32_get(unit, CLMAC_TX_MAC_SAr, rval64, SA_LOf);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

    SOC_FUNC_RETURN;
}

int clmac_tx_average_ipg_set(int unit, soc_port_t port, int val)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    RD_CLMAC(TX_CTRLr, unit, port, &reg_val);

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 64 bytes (i.e. multiple of 8 bits) */
    val = val < 64 ? 64 : (val > 512 ? 512 : (val + 7) & (0x7f << 3));

    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, val/8);

    WR_CLMAC(TX_CTRLr, unit, port, reg_val);

    SOC_FUNC_RETURN
}

int clmac_tx_average_ipg_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    RD_CLMAC(TX_CTRLr, unit, port, &reg_val);

    *val = soc_reg64_field32_get(unit, CLMAC_TX_CTRLr, reg_val, AVERAGE_IPGf) * 8;

    SOC_FUNC_RETURN
}

int clmac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint64 reg_val;

    if(length > 8){
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                       "runt size should be small than 8. got %d"), length));
        return (SOC_E_PARAM);
    }

    COMPILER_64_ZERO(reg_val);

    RD_CLMAC(TX_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, CLMAC_TX_CTRLr, &reg_val, 
                           TX_PREAMBLE_LENGTHf, length);
    WR_CLMAC(TX_CTRLr, unit, port, reg_val);
    return (SOC_E_NONE);
}

/***************************************************************************** 
 * Remote/local Fault                                                        *
 */

int clmac_remote_fault_control_set(int unit, soc_port_t port, const portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, REMOTE_FAULT_DISABLEf, control->enable ? 0 : 1 /*flip*/);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf, control->drop_tx_on_fault ? 1: 0);
    WR_CLMAC(RX_LSS_CTRLr, unit, port, reg_val);

    SOC_FUNC_RETURN;
}

int clmac_remote_fault_control_get(int unit, soc_port_t port, portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, REMOTE_FAULT_DISABLEf);
    control->enable = (field_val ? 0 : 1);/*flip*/

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf);
    control->drop_tx_on_fault  = (field_val ? 1 : 0);

    SOC_FUNC_RETURN;
}

int clmac_local_fault_control_set(int unit, soc_port_t port, const portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, LOCAL_FAULT_DISABLEf, control->enable ? 0 : 1 /*flip*/);
    soc_reg64_field32_set(unit, CLMAC_RX_LSS_CTRLr, &reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf, control->drop_tx_on_fault ? 1: 0);
    WR_CLMAC(RX_LSS_CTRLr, unit, port, reg_val);

    SOC_FUNC_RETURN;
}


int clmac_local_fault_control_get(int unit, soc_port_t port, portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, LOCAL_FAULT_DISABLEf);
    control->enable = (field_val ? 0 : 1);/*flip*/

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_CTRLr, reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf);
    control->drop_tx_on_fault  = (field_val ? 1 : 0);

    SOC_FUNC_RETURN;
}

int clmac_remote_fault_status_get(int unit, soc_port_t port, int clear_status, int *status)
{
    uint64 reg_val, reg_val_clear;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_clear);
    if(clear_status) {
        soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val_clear, CLEAR_REMOTE_FAULT_STATUSf, 1);
    }

    RD_CLMAC(RX_LSS_STATUSr, unit, port, &reg_val);

    if(clear_status) {
        WR_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, reg_val_clear);
        COMPILER_64_ZERO(reg_val_clear);
        WR_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, reg_val_clear);
    }

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_STATUSr, reg_val, REMOTE_FAULT_STATUSf);
    *status = field_val ? 1: 0;

    SOC_FUNC_RETURN;
}

int clmac_local_fault_status_get(int unit, soc_port_t port, int clear_status, int *status)
{
    uint64 reg_val, reg_val_clear;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_clear);
    if(clear_status) {
        soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val_clear, CLEAR_LOCAL_FAULT_STATUSf, 1);
    }

    RD_CLMAC(RX_LSS_STATUSr, unit, port, &reg_val);

    if(clear_status) {
        WR_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, reg_val_clear);
        COMPILER_64_ZERO(reg_val_clear);
        WR_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, reg_val_clear);
    }

    field_val = soc_reg64_field32_get(unit, CLMAC_RX_LSS_STATUSr, reg_val, LOCAL_FAULT_STATUSf);
    *status = field_val ? 1: 0;

    SOC_FUNC_RETURN;
}

int clmac_clear_rx_lss_status_set(int unit, soc_port_t port, int lcl_fault, int rmt_fault)
{
    uint64 reg_val;

    RD_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val, 
                                     CLEAR_REMOTE_FAULT_STATUSf, rmt_fault);
    soc_reg64_field32_set(unit, CLMAC_CLEAR_RX_LSS_STATUSr, &reg_val, 
                                     CLEAR_LOCAL_FAULT_STATUSf, lcl_fault);
    WR_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int clmac_clear_rx_lss_status_get(int unit, soc_port_t port, int *lcl_fault, int *rmt_fault)
{
    uint64 reg_val;

    RD_CLMAC(CLEAR_RX_LSS_STATUSr, unit, port, &reg_val);
    *rmt_fault = soc_reg64_field32_get(unit, CLMAC_CLEAR_RX_LSS_STATUSr, reg_val, 
                                     CLEAR_REMOTE_FAULT_STATUSf);
    *lcl_fault = soc_reg64_field32_get(unit, CLMAC_CLEAR_RX_LSS_STATUSr, reg_val, 
                                     CLEAR_LOCAL_FAULT_STATUSf);

    return (SOC_E_NONE);
}

/**************************************************************************** 
 Flow Control
 */

int clmac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PAUSE_CTRLr, unit, port , &reg_val);

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_TIMERf, control->refresh_timer);
            soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_ENf, 1);
        }else{
            soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, PAUSE_XOFF_TIMERf, control->xoff_timer);
    }

    soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, TX_PAUSE_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, CLMAC_PAUSE_CTRLr, &reg_val, RX_PAUSE_ENf, control->rx_enable);

    WR_CLMAC(PAUSE_CTRLr, unit, port , reg_val);

    SOC_FUNC_RETURN;
}

int clmac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PFC_CTRLr, unit, port , &reg_val);

    if (control->refresh_timer) {
        soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_TIMERf, control->refresh_timer);
        soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_ENf, 1);
    } else {
        soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_REFRESH_ENf, 0);
    }
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_STATS_ENf, control->stats_en);
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, PFC_XOFF_TIMERf, control->xoff_timer);
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, FORCE_PFC_XONf, control->force_xon);

    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, TX_PFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &reg_val, RX_PFC_ENf, control->rx_enable);

    WR_CLMAC(PFC_CTRLr, unit, port , reg_val);

    SOC_FUNC_RETURN;
}

int clmac_llfc_control_set(int unit, soc_port_t port, const portmod_llfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(LLFC_CTRLr, unit, port , &reg_val);

    if(control->rx_enable || control->tx_enable){
        soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, LLFC_IN_IPG_ONLYf, control->in_ipg_only);
        soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, LLFC_CRC_IGNOREf, control->crc_ignore);
    }
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, TX_LLFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &reg_val, RX_LLFC_ENf, control->rx_enable);

    WR_CLMAC(LLFC_CTRLr, unit, port , reg_val);

    SOC_FUNC_RETURN;
}

int clmac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PAUSE_CTRLr, unit, port , &reg_val);

    refresh_enable = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, PAUSE_REFRESH_ENf);
    refresh_timer =  soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, PAUSE_REFRESH_TIMERf);
    control->refresh_timer = (refresh_enable ? refresh_timer : -1);
    control->xoff_timer = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, PAUSE_XOFF_TIMERf);
    control->rx_enable = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, RX_PAUSE_ENf);
    control->tx_enable = soc_reg64_field32_get(unit, CLMAC_PAUSE_CTRLr, reg_val, TX_PAUSE_ENf);

    SOC_FUNC_RETURN;
}

int clmac_pfc_control_get (int unit, soc_port_t port,
                           portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PFC_CTRLr, unit, port , &reg_val);

    refresh_enable = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_REFRESH_ENf);
    if (refresh_enable) {
        refresh_timer = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr,
                                              reg_val, PFC_REFRESH_TIMERf);
    }
    control->refresh_timer = refresh_timer;
    control->stats_en = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_STATS_ENf);
    control->xoff_timer = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, PFC_XOFF_TIMERf);
    control->force_xon = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, FORCE_PFC_XONf);
    control->rx_enable = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, RX_PFC_ENf);
    control->tx_enable = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, reg_val, TX_PFC_ENf);

    SOC_FUNC_RETURN;
}

int clmac_llfc_control_get(int unit, soc_port_t port, portmod_llfc_control_t *control)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(LLFC_CTRLr, unit, port , &reg_val);

    control->in_ipg_only = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, LLFC_IN_IPG_ONLYf);
    control->crc_ignore = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, LLFC_CRC_IGNOREf);
    control->rx_enable = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, RX_LLFC_ENf);
    control->tx_enable = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, reg_val, TX_LLFC_ENf);

    SOC_FUNC_RETURN;
}

/*
 * Function:
 *      clmac_duplex_set
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
int
clmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_cl_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return (duplex) ? (SOC_E_NONE) : (SOC_E_UNAVAIL) ;
}


/*
 * Function:
 *      clmac_duplex_get
 * Purpose:
 *      Get CLMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
int
clmac_duplex_get(int unit, soc_port_t port, int *duplex)
{
    *duplex = TRUE; /* Always full duplex */

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_cl_duplex_get: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *duplex ? "Full" : "Half"));
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

int
clmac_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval;

    RD_CLMAC(MODEr, unit, port, &rval);
    switch (soc_reg64_field32_get(unit, CLMAC_MODEr, rval, SPEED_MODEf)) {
    case CLMAC_SPEED_100000:
    default:
        *speed = 100000;
        break;
    }

    LOG_VERBOSE(BSL_LS_SOC_100G,
                (BSL_META_U(unit,
                            "mac_xl_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}


/*! 
 * clmac_eee_set
 *
 * @brief EEE Control and Timer set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int clmac_eee_set(int unit, int port, const portmod_eee_t* eee)
{
    uint64 rval;

    RD_CLMAC(EEE_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_EEE_CTRLr, &rval, EEE_ENf, eee->enable);
    WR_CLMAC(EEE_CTRLr, unit, port, rval);

    RD_CLMAC(EEE_TIMERSr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_EEE_TIMERSr, &rval, EEE_DELAY_ENTRY_TIMERf, eee->tx_idle_time);
    soc_reg64_field32_set(unit, CLMAC_EEE_TIMERSr, &rval, EEE_WAKE_TIMERf, eee->tx_wake_time);
    WR_CLMAC(EEE_TIMERSr, unit, port, rval);

    return SOC_E_NONE;
}


/*! 
 * clmac_eee_get
 *
 * @brief EEE Control and Timer get
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int clmac_eee_get(int unit, int port, portmod_eee_t* eee)
{
    uint64 rval;
    portmod_eee_t* pEEE = (portmod_eee_t*)eee;

    RD_CLMAC(EEE_CTRLr, unit, port, &rval);
    pEEE->enable = soc_reg64_field32_get(unit, CLMAC_EEE_CTRLr, rval, EEE_ENf);

    RD_CLMAC(EEE_TIMERSr, unit, port, &rval);

    pEEE->tx_idle_time = soc_reg64_field32_get (unit, CLMAC_EEE_TIMERSr, rval, 
                                               EEE_DELAY_ENTRY_TIMERf);
    pEEE->tx_wake_time = soc_reg64_field32_get (unit, CLMAC_EEE_TIMERSr, rval, 
                                               EEE_WAKE_TIMERf);
    return(SOC_E_NONE);
}

int clmac_pass_control_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRLr, unit, port, &rval);

    if (soc_reg_field_valid(unit, CLMAC_RX_CTRLr, RX_PASS_PFCf)) {
        soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rval,
                                                         RX_PASS_CTRLf, (value? 1:0));
    }
    WR_CLMAC(RX_CTRLr, unit, port, rval);

    return(SOC_E_NONE);
}

int clmac_pass_control_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRLr, unit, port, &rval);
    if (soc_reg_field_valid(unit, CLMAC_RX_CTRLr, RX_PASS_PFCf)) {
        *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr, rval, RX_PASS_CTRLf);
    }

    return(SOC_E_NONE);
}

/*!
 * clmac_pass_pfc_frame_set
 *
 * @brief send PFC frame to system side
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  value           - 1 or 0
 */
int clmac_pass_pfc_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRLr, unit, port, &rval);
    if (soc_reg_field_valid(unit, CLMAC_RX_CTRLr, RX_PASS_PFCf)) {
        soc_reg64_field32_set(unit, CLMAC_RX_CTRLr, &rval,
                                                        RX_PASS_PFCf, (value? 1:0));
    }
    WR_CLMAC(RX_CTRLr, unit, port, rval);

    return(SOC_E_NONE);
}

int clmac_pass_pfc_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRLr, unit, port, &rval);
    if (soc_reg_field_valid(unit, CLMAC_RX_CTRLr, RX_PASS_PFCf)) {
        *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr, rval, RX_PASS_PFCf);
    }

    return(SOC_E_NONE);
}

/*!
 * clmac_pass_pause_frame_set
 *
 * @brief send pause frame to system side
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  value           - 1 or 0
 */
int clmac_pass_pause_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRLr, unit, port, &rval);
    if (soc_reg_field_valid(unit, CLMAC_RX_CTRLr, RX_PASS_PFCf)) {
        soc_reg64_field32_set (unit, CLMAC_RX_CTRLr, &rval,
                                                        RX_PASS_PAUSEf, (value? 1:0));
    }
    WR_CLMAC(RX_CTRLr, unit, port, rval);

    return(SOC_E_NONE);
}

int clmac_pass_pause_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRLr, unit, port, &rval);
    if (soc_reg_field_valid(unit, CLMAC_RX_CTRLr, RX_PASS_PFCf)) {
        *value = soc_reg64_field32_get(unit, CLMAC_RX_CTRLr, rval, RX_PASS_PAUSEf);
    }

    return(SOC_E_NONE);
}

int clmac_lag_failover_loopback_set(int unit, int port, int val)
{
    uint64 rval;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    RD_CLMAC(LAG_FAILOVER_STATUSr, unit, port, &rval);

    soc_reg64_field32_set (unit, CLMAC_LAG_FAILOVER_STATUSr, &rval, 
                                            LAG_FAILOVER_LOOPBACKf, val);
    WR_CLMAC(LAG_FAILOVER_STATUSr, unit, port, rval);

    SOC_FUNC_RETURN;
}

int clmac_lag_failover_loopback_get(int unit, int port, int *val)
{
    uint64 rval;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    RD_CLMAC(LAG_FAILOVER_STATUSr, unit, port, &rval);

    *val = soc_reg64_field32_get (unit, CLMAC_LAG_FAILOVER_STATUSr, rval, 
                                                  LAG_FAILOVER_LOOPBACKf);

    SOC_FUNC_RETURN;
}

/*!
 * clmac_lag_failover_disable
 *
 * @brief Port Mac Control Lag Failover Disable. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 */
int clmac_lag_failover_disable(int unit, int port)
{
    uint64 mac_ctrl;

    RD_CLMAC(CTRLr, unit, port, &mac_ctrl);

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LAG_FAILOVER_ENf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LINK_STATUS_SELECTf, 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, REMOVE_FAILOVER_LPBKf, 0);

    WR_CLMAC(CTRLr, unit, port, mac_ctrl);
    return (SOC_E_NONE);
}   

        
/*!
 * clmac_lag_failover_en_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int clmac_lag_failover_en_set(int unit, int port, int val)
{
    uint64 mac_ctrl;

    RD_CLMAC(CTRLr, unit, port, &mac_ctrl);

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, LAG_FAILOVER_ENf, val);

    WR_CLMAC(CTRLr, unit, port, mac_ctrl);
    return (SOC_E_NONE);
}   
        
/*!     
 * clmac_lag_failover_en_get
 *      
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  val         -
 */
int clmac_lag_failover_en_get(int unit, int port, int *val)
{                                 
    uint64 mac_ctrl;
 
    RD_CLMAC(CTRLr, unit, port, &mac_ctrl);
 
    *val = soc_reg64_field32_get (unit, CLMAC_CTRLr,
                                      mac_ctrl, LAG_FAILOVER_ENf);
    return (SOC_E_NONE);
}


/*!
 * clmac_lag_remove_failover_lpbk_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int clmac_lag_remove_failover_lpbk_set(int unit, int port, int val)
{
    uint64 mac_ctrl;

    RD_CLMAC(CTRLr, unit, port, &mac_ctrl);

    soc_reg64_field32_set(unit, CLMAC_CTRLr, &mac_ctrl, REMOVE_FAILOVER_LPBKf, val);

    WR_CLMAC(CTRLr, unit, port, mac_ctrl);
    return (SOC_E_NONE);
}   
        
/*!     
 * clmac_lag_remove_failover_lpbk_get
 *      
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  val         -
 */
int clmac_lag_remove_failover_lpbk_get(int unit, int port, int *val)
{                                 
    uint64 mac_ctrl;
 
    RD_CLMAC(CTRLr, unit, port, &mac_ctrl);
 
    *val = soc_reg64_field32_get (unit, CLMAC_CTRLr,
                                      mac_ctrl, REMOVE_FAILOVER_LPBKf);
    return (SOC_E_NONE);
}

int clmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val)
{
    uint64 reg_val;

    RD_CLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, CLMAC_RX_LSS_CTRLr, &reg_val,
                           RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf, val);

    WR_CLMAC(RX_LSS_CTRLr, unit, port, reg_val);
    return (SOC_E_NONE);
}


int clmac_reset_fc_timers_on_link_dn_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;

    RD_CLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    *val = soc_reg64_field32_get (unit, CLMAC_RX_LSS_CTRLr, reg_val,
                           RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf);
    return (SOC_E_NONE);
}


int clmac_drain_cell_get (int unit, int port, portmod_drain_cells_t *drain_cells)
{   
    uint64 rval;

    RD_CLMAC(PFC_CTRLr, unit, port, &rval);
    drain_cells->rx_pfc_en = soc_reg64_field32_get(unit, CLMAC_PFC_CTRLr, 
                                   rval, RX_PFC_ENf);

    RD_CLMAC(LLFC_CTRLr, unit, port, &rval);
    drain_cells->llfc_en = soc_reg64_field32_get(unit, CLMAC_LLFC_CTRLr, 
                                 rval, RX_LLFC_ENf);

    RD_CLMAC(PAUSE_CTRLr, unit, port , &rval);
    drain_cells->rx_pause  = soc_reg64_field32_get (unit, CLMAC_PAUSE_CTRLr,
                                                 rval, RX_PAUSE_ENf);
    drain_cells->tx_pause  = soc_reg64_field32_get (unit, CLMAC_PAUSE_CTRLr,
                                                 rval, TX_PAUSE_ENf);

    return (0);
}    

int clmac_discard_set(int unit, soc_port_t port, int discard)
{
    uint64 rval;

    /* Clear Discard fields */
    RD_CLMAC(TX_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, DISCARDf, discard);
    soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &rval, EP_DISCARDf, discard);
    WR_CLMAC(TX_CTRLr, unit, port, rval);
    return (SOC_E_NONE);
}

int clmac_drain_cell_stop (int unit, int port, const portmod_drain_cells_t *drain_cells)
{   
    uint64 rval;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(clmac_discard_set(unit, port, 0));

    /* set pause fields */ 
    RD_CLMAC(PAUSE_CTRLr, unit, port , &rval);
    soc_reg64_field32_set (unit, CLMAC_PAUSE_CTRLr,
                           &rval, RX_PAUSE_ENf, drain_cells->rx_pause);
    soc_reg64_field32_set (unit, CLMAC_PAUSE_CTRLr,
                           &rval, TX_PAUSE_ENf, drain_cells->tx_pause);
    WR_CLMAC(PAUSE_CTRLr, unit, port , rval);

    /* set pfc rx_en fields */ 
    RD_CLMAC(PFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, 
                          &rval, RX_PFC_ENf, drain_cells->rx_pfc_en);
    WR_CLMAC(PFC_CTRLr, unit, port, rval);

    /* set llfc rx_en fields */ 
    RD_CLMAC(LLFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, 
                          &rval, RX_LLFC_ENf, drain_cells->llfc_en);
    WR_CLMAC(LLFC_CTRLr, unit, port, rval);

    return (0);
}

int clmac_drain_cell_start(int unit, int port)
{   
    uint64 rval;

    RD_CLMAC(CTRLr, unit, port, &rval);

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval, TX_ENf, 1);
    /* Disable RX */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval, RX_ENf, 0);

    WR_CLMAC(CTRLr, unit, port, rval);

    RD_CLMAC(PAUSE_CTRLr, unit, port , &rval);
    soc_reg64_field32_set (unit, CLMAC_PAUSE_CTRLr, &rval, RX_PAUSE_ENf, 0);
    WR_CLMAC(PAUSE_CTRLr, unit, port, rval);

    RD_CLMAC(PFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_PFC_CTRLr, &rval, RX_PFC_ENf, 0);
    WR_CLMAC(PFC_CTRLr, unit, port, rval);

    RD_CLMAC(LLFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_LLFC_CTRLr, &rval, RX_LLFC_ENf, 0);
    WR_CLMAC(LLFC_CTRLr, unit, port, rval);

    RD_CLMAC(CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &rval, SOFT_RESETf, 1);
    WR_CLMAC(CTRLr, unit, port, rval);
 
    SOC_IF_ERROR_RETURN(clmac_discard_set(unit, port, 1));

    return (0);
}

int
clmac_mac_ctrl_set(int unit, int port, uint64 mac_ctrl)
{
    WR_CLMAC(CTRLr, unit, port, mac_ctrl);
    return (0);
}


int clmac_txfifo_cell_cnt_get (int unit, int port, uint32* fval)
{
    uint64 rval;

    RD_CLMAC(TXFIFO_CELL_CNTr, unit, port, &rval);
    *fval = soc_reg64_field32_get(unit, CLMAC_TXFIFO_CELL_CNTr, rval,
                                     CELL_CNTf);
    return (0);
}

int
clmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl, int *rx_en)
{
    uint64 ctrl;

    RD_CLMAC(CTRLr, unit, port, &ctrl);
    *mac_ctrl = ctrl;

    *rx_en = soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, RX_ENf);

    return (0);
}

int
clmac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint64 ctrl;
    /* Enable both TX and RX, de-assert SOFT_RESET */
    RD_CLMAC(CTRLr, unit, port, &ctrl);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, rx_en ? 1 : 0);
    WR_CLMAC(CTRLr, unit, port, ctrl);
    /*Bring mac out of reset */
    SOC_IF_ERROR_RETURN(clmac_soft_reset_set(unit, port, 0));

    return (0);
}

int
clmac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint64 ctrl, octrl;
    uint32 soft_reset = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    RD_CLMAC(CTRLr, unit, port, &ctrl);
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, rx_en ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing senario, CLMAC_CTRLr.SOFT_RESETf is 
         *      used to early check to see if this port is at disabled state 
         *      already.
         */
        soft_reset = soc_reg64_field32_get(unit, 
                                           CLMAC_CTRLr, ctrl, SOFT_RESETf);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    WR_CLMAC(CTRLr, unit, port, ctrl);

    /*Bring mac out of reset */
    if (rx_en) {
        SOC_IF_ERROR_RETURN(clmac_soft_reset_set(unit, port, 0));
    }

    return (0);
}

/*
 * Function:
 *      
 * Purpose:
 *     This function checks if the MAC control values are already
 *     set.
 *
 *     This function should be called before enabling/disabling the
 *     MAC to prevent the following:
 *     1. Problem occurred for disabling process only.
 *     2. To comply original designing senario, CLMAC_CTRLr.SOFT_RESETf is 
 *        used to early check to see if this port is at disabled state 
 *        already.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Port number.
 *      enable  - (IN) Indicates whether to check for enable or disable.
 *      reset   - (OUT) Indicates MAC control needs to be reset or not.
 * Returns:
 *      SOC_E_XXX
 */
int clmac_reset_check(int unit, int port, int enable, int *reset)
{
    uint64 ctrl, octrl;

    *reset = 1;

    RD_CLMAC(CTRLr, unit, port, &ctrl);
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, TX_ENf, enable ? 1 : 0);
    soc_reg64_field32_set(unit, CLMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        if (enable) {
            *reset = 0;
        } else {
            if (soc_reg64_field32_get(unit, CLMAC_CTRLr, ctrl, SOFT_RESETf)) {
                *reset = 0;
            }
        }
    }

    return SOC_E_NONE;
}


/*! 
 * clmac_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int clmac_pfc_config_set (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 fval; 
    uint64 rval;

    RD_CLMAC(PFC_TYPEr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_PFC_TYPEr, &rval, PFC_ETH_TYPEf, pfc_cfg->type);
    WR_CLMAC(PFC_TYPEr, unit, port, rval);

    RD_CLMAC(PFC_OPCODEr, unit, port, &rval);
    soc_reg64_field32_set(unit, CLMAC_PFC_OPCODEr, &rval, PFC_OPCODEf, pfc_cfg->opcode);
    WR_CLMAC(PFC_OPCODEr, unit, port, rval);

    RD_CLMAC(PFC_DAr, unit, port, &rval);
    fval = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
    fval &= 0x00ffffff;
    fval |= (pfc_cfg->da_oui & 0xff) << 24;
    soc_reg64_field32_set(unit, CLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

    soc_reg64_field32_set(unit, CLMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                          pfc_cfg->da_oui >> 8);
    WR_CLMAC(PFC_DAr, unit, port, rval);

    RD_CLMAC(PFC_DAr, unit, port, &rval);
    fval = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
    fval &= 0xff000000;
    fval |= pfc_cfg->da_nonoui;
    soc_reg64_field32_set(unit, CLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);
 
    WR_CLMAC(PFC_DAr, unit, port, rval);
    return (SOC_E_NONE);
}



/*! 
 * clmac_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int clmac_pfc_config_get (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint64 rval;
    uint32 fval;
    uint32 fval0,fval1;
    portmod_pfc_config_t* pcfg = (portmod_pfc_config_t*) pfc_cfg;

    RD_CLMAC(PFC_TYPEr, unit, port, &rval);

    pcfg->type = soc_reg64_field32_get (unit, CLMAC_PFC_TYPEr, 
                                          rval, PFC_ETH_TYPEf);

    RD_CLMAC(PFC_OPCODEr, unit, port, &rval);
    pcfg->opcode = soc_reg64_field32_get (unit, CLMAC_PFC_OPCODEr, 
                                            rval, PFC_OPCODEf);

    RD_CLMAC(PFC_DAr, unit, port, &rval);
    fval0 = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval,
                                  PFC_MACDA_LOf);
    fval1 = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval,
                                  PFC_MACDA_HIf);
    pcfg->da_oui = (fval0 >> 24) | (fval1 << 8);

    RD_CLMAC(PFC_DAr, unit, port, &rval);
    fval = soc_reg64_field32_get(unit, CLMAC_PFC_DAr, rval,
                                  PFC_MACDA_LOf);
    pcfg->da_nonoui = fval & 0xFFFFFF;

    return (SOC_E_NONE);
}

int clmac_e2ecc_hdr_get (int unit, int port, uint32_t *words)
{
    uint64 rval64;

    RD_CLMAC(E2ECC_MODULE_HDR_0r, unit, port, &rval64);
    words[0] = soc_reg64_field32_get (unit, CLMAC_E2ECC_MODULE_HDR_0r, rval64,
                                      E2ECC_MODULE_HDR_0_HIf);
    words[1] = soc_reg64_field32_get(unit, CLMAC_E2ECC_MODULE_HDR_0r, rval64,
                                      E2ECC_MODULE_HDR_0_LOf);

    RD_CLMAC(E2ECC_MODULE_HDR_1r, unit, port, &rval64);
    words[2] = soc_reg64_field32_get (unit, CLMAC_E2ECC_MODULE_HDR_1r, rval64,
                                      E2ECC_MODULE_HDR_1_HIf);
    words[3] = soc_reg64_field32_get(unit, CLMAC_E2ECC_MODULE_HDR_1r, rval64,
                                      E2ECC_MODULE_HDR_1_LOf);

    RD_CLMAC(E2ECC_DATA_HDR_0r, unit, port, &rval64);
    words[4] = soc_reg64_field32_get (unit, CLMAC_E2ECC_DATA_HDR_0r, rval64,
                                      E2ECC_DATA_HDR_0_HIf);
    words[5] = soc_reg64_field32_get(unit, CLMAC_E2ECC_DATA_HDR_0r, rval64,
                                      E2ECC_DATA_HDR_0_LOf);

    RD_CLMAC(E2ECC_DATA_HDR_1r, unit, port, &rval64);
    words[6] = soc_reg64_field32_get (unit, CLMAC_E2ECC_DATA_HDR_1r, rval64,
                                      E2ECC_DATA_HDR_1_HIf);
    words[7] = soc_reg64_field32_get(unit, CLMAC_E2ECC_DATA_HDR_1r, rval64,
                                      E2ECC_DATA_HDR_1_LOf);

    return SOC_E_NONE;
}

int clmac_e2ecc_hdr_set (int unit, int port, uint32_t *words)
{
    uint64 rval64;
    COMPILER_64_ZERO(rval64);

    soc_reg64_field32_set(unit, CLMAC_E2ECC_MODULE_HDR_0r, &rval64,
                                      E2ECC_MODULE_HDR_0_HIf, words[0]);
    soc_reg64_field32_set(unit, CLMAC_E2ECC_MODULE_HDR_0r, &rval64,
                                      E2ECC_MODULE_HDR_0_LOf, words[1]);
    WR_CLMAC(E2ECC_MODULE_HDR_0r, unit, port, rval64);


    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set (unit, CLMAC_E2ECC_MODULE_HDR_1r, &rval64,
                                      E2ECC_MODULE_HDR_1_HIf, words[2]);
    soc_reg64_field32_set(unit, CLMAC_E2ECC_MODULE_HDR_1r, &rval64,
                                      E2ECC_MODULE_HDR_1_LOf, words[3]);
    WR_CLMAC(E2ECC_MODULE_HDR_1r, unit, port, rval64);


    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set (unit, CLMAC_E2ECC_DATA_HDR_0r, &rval64,
                                      E2ECC_DATA_HDR_0_HIf, words[4]);
    soc_reg64_field32_set(unit, CLMAC_E2ECC_DATA_HDR_0r, &rval64,
                                      E2ECC_DATA_HDR_0_LOf, words[5]);
    WR_CLMAC(E2ECC_DATA_HDR_0r, unit, port, rval64);


    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set (unit, CLMAC_E2ECC_DATA_HDR_1r, &rval64,
                                      E2ECC_DATA_HDR_1_HIf, words[6]);
    soc_reg64_field32_set(unit, CLMAC_E2ECC_DATA_HDR_1r, &rval64,
                                      E2ECC_DATA_HDR_1_LOf, words[7]);
    WR_CLMAC(E2ECC_DATA_HDR_1r, unit, port, rval64);

    return SOC_E_NONE;
}

int clmac_e2e_enable_set(int unit, int port, int enable)
{
    uint64 rval64;

    RD_CLMAC(E2E_CTRLr, unit, port, &rval64);
    soc_reg64_field32_set(unit, CLMAC_E2E_CTRLr, &rval64, E2E_ENABLEf, enable);
    WR_CLMAC(E2E_CTRLr, unit, port, rval64);

    return (0);
}

int clmac_e2e_enable_get(int unit, int port, int *enable)
{
    uint64 rval64;

    RD_CLMAC(E2E_CTRLr, unit, port, &rval64);
    *enable = soc_reg64_field32_get(unit, CLMAC_E2E_CTRLr, rval64, E2E_ENABLEf);

    return (0);
}

/*!
 * clmac_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int clmac_frame_spacing_stretch_set(int unit, int port, int spacing)
{
    uint64 mac_ctrl;

    if (spacing < 0 || spacing > 257) {
        return SOC_E_PARAM;
    }

    RD_CLMAC(TX_CTRLr, unit, port, &mac_ctrl);

    if (spacing == CLMAC_THROT_10G_TO_5G) {
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 21);
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 21);
    } else if (spacing == CLMAC_THROT_10G_TO_2P5G) {
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 21);
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 63);
    } else if (spacing >= 8) {
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, spacing);
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 1);
    } else {
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 0);
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 0);
    }

    WR_CLMAC(TX_CTRLr, unit, port, mac_ctrl);

    return (SOC_E_NONE);
}

/*!
 * clmac_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch
 * 
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  spacing         -
 */
int clmac_frame_spacing_stretch_get(int unit, int port, int *spacing)
{
    uint64 mac_ctrl;

    RD_CLMAC(TX_CTRLr, unit, port, &mac_ctrl);

    *spacing = soc_reg64_field32_get (unit, CLMAC_TX_CTRLr,
                                      mac_ctrl, THROT_DENOMf);
    return (SOC_E_NONE);
}

int clmac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value)
{
    int32 version;
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_INTR_ENABLEr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TX_PKT_UNDERFLOWf);
                break;
        case portmodIntrTypeTxPktOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TX_LLFC_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TX_TS_FIFO_OVERFLOWf);
                break;
        case portmodIntrTypeRxPktOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_RX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeRxMsgOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_RX_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_RX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_RX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxTsMemSingleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_RX_TS_MEM_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxTsMemDoubleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_RX_TS_MEM_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeLocalFaultStatus :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_LOCAL_FAULT_STATUSf);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_REMOTE_FAULT_STATUSf);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_LINK_INTERRUPTION_STATUSf);
                break;
        case portmodIntrTypeTsEntryValid :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_ENABLEr, reg_val, EN_TS_ENTRY_VALIDf);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
                break;
     }

exit:
    SOC_FUNC_RETURN;
}

int clmac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value)
{
    uint64 rval;
    int32 version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_INTR_ENABLEr(unit, port, &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TX_PKT_UNDERFLOWf, value);
                break;
        case portmodIntrTypeTxPktOverflow :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TX_PKT_OVERFLOWf, value);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TX_LLFC_MSG_OVERFLOWf, value);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TX_TS_FIFO_OVERFLOWf, value);
                break;
        case portmodIntrTypeRxPktOverflow :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_RX_PKT_OVERFLOWf, value);
                break;
        case portmodIntrTypeRxMsgOverflow :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_RX_MSG_OVERFLOWf, value);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TX_CDC_SINGLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TX_CDC_DOUBLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_RX_CDC_SINGLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_RX_CDC_DOUBLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeRxTsMemSingleBitErr :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_RX_TS_MEM_SINGLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeRxTsMemDoubleBitErr :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_RX_TS_MEM_DOUBLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeLocalFaultStatus :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_LOCAL_FAULT_STATUSf, value);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_REMOTE_FAULT_STATUSf, value);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_LINK_INTERRUPTION_STATUSf, value);
                break;
        case portmodIntrTypeTsEntryValid :
                soc_reg64_field32_set(unit, CLMAC_INTR_ENABLEr, &rval, EN_TS_ENTRY_VALIDf, value);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
        break;
    }

    _SOC_IF_ERR_EXIT(WRITE_CLMAC_INTR_ENABLEr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int clmac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value)
{
    int32 version;
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_INTR_STATUSr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_UNDERFLOWf);
                break;
        case portmodIntrTypeTxPktOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_LLFC_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_TS_FIFO_OVERFLOWf);
                break;
        case portmodIntrTypeRxPktOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeRxMsgOverflow :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxTsMemSingleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_TS_MEM_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxTsMemDoubleBitErr :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_TS_MEM_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeLocalFaultStatus :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_LOCAL_FAULT_STATUSf);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_REMOTE_FAULT_STATUSf);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_LINK_INTERRUPTION_STATUSf);
                break;
        case portmodIntrTypeTsEntryValid :
                *value = soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TS_ENTRY_VALIDf);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
                break;
    }

exit:
    SOC_FUNC_RETURN;
}

int clmac_interrupts_status_get(int unit, int port, int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 cnt = 0;
    int32 version;
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_CLMAC_INTR_STATUSr(unit, port, &reg_val));

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_UNDERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktUnderflow;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktOverflow;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_LLFC_MSG_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxLlfcMsgOverflow;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_TS_FIFO_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxTSFifoOverflow;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_PKT_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxPktOverflow;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_MSG_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxMsgOverflow;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcSingleBitErr;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcDoubleBitErr;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxCdcSingleBitErr;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxCdcDoubleBitErr;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_TS_MEM_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxTsMemSingleBitErr;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_RX_TS_MEM_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxTsMemDoubleBitErr;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_LOCAL_FAULT_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLocalFaultStatus;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_REMOTE_FAULT_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRemoteFaultStatus;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_LINK_INTERRUPTION_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLinkInterruptionStatus;
    }

    if (soc_reg64_field32_get(unit, CLMAC_INTR_STATUSr, reg_val, SUM_TS_ENTRY_VALIDf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsEntryValid;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

int clmac_preemption_mode_set(int32 unit, soc_port_t port, uint32 enable)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                  reg_val, PREEMPTION_MODEf);

    if (value != enable) {
        soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                              &reg_val, PREEMPTION_MODEf, enable? 1: 0);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_mode_get(int32 unit, soc_port_t port, uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                    reg_val, PREEMPTION_MODEf);

    return (SOC_E_NONE);
}

int clmac_preemption_tx_enable_set(int32 unit, soc_port_t port, uint32 enable)
{
    uint64 reg_val, v_reg_val;
    int32 value = 0, rc = SOC_E_NONE;
    uint32 is_verify_set;
    uint32 timeout = 0, num_attempts = 0;
    uint32 verify_status = 0, verify_op_done = 0;
    uint32 wait_time, iter = 0;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                  reg_val, PREEMPT_ENABLEf);

    is_verify_set = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                          reg_val, VERIFY_ENABLEf);  


    /* if preemption already enabled, return */
    if (value == enable) {
        return rc;
    }

    /* Static configuration - no preemption negotaition with link partner*/
    soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                          &reg_val, PREEMPT_ENABLEf, enable? 1: 0);
    SOC_IF_ERROR_RETURN(WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));

    if (is_verify_set) {
        /*
         * preemption negotiation enabled - wait for the negotiation
         * to complete.
         * Wait time >= (num verify attempts + 1) * (verify timeout + 1)
         */
        timeout = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                        reg_val, VERIFY_TIMEf);  
        num_attempts = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                         reg_val, VERIFY_ATTEMPT_LIMITf);  
        wait_time = (num_attempts + 1) * (timeout + 1);

        /* wait for (timeout + 1) millsecs, and check verify status */
        sal_udelay((timeout + 1) * 1000);

        /* check verification status */
        SOC_IF_ERROR_RETURN(
               READ_CLMAC_MERGE_VERIFY_STATEr(unit, port, &v_reg_val));
        verify_status = soc_reg64_field32_get(unit,
                                              CLMAC_MERGE_VERIFY_STATEr,
                                              v_reg_val,
                                              PREEMPTION_VERIFY_STATUSf);
        if ((verify_status == CLMAC_PREEMPT_VERIFY_SUCCEED) ||
            (verify_status == CLMAC_PREEMPT_VERIFY_FAILED)) {
            verify_op_done = 1;
        }

        while (!verify_op_done) { 
            sal_udelay((timeout + 1) * 1000);

            /* check verification status */
            SOC_IF_ERROR_RETURN(
                   READ_CLMAC_MERGE_VERIFY_STATEr(unit, port, &v_reg_val));
            verify_status = soc_reg64_field32_get(unit,
                                              CLMAC_MERGE_VERIFY_STATEr,
                                              v_reg_val,
                                              PREEMPTION_VERIFY_STATUSf);

            /* break if verify operation done or timeout */
            if ((verify_status == CLMAC_PREEMPT_VERIFY_SUCCEED) ||
                (verify_status == CLMAC_PREEMPT_VERIFY_FAILED)  ||
                (((timeout + 1) * iter++) > wait_time)) {
                verify_op_done = 1;
                break;
            }
        }  
    }

    if (verify_op_done) {
        rc = (verify_status == CLMAC_PREEMPT_VERIFY_SUCCEED)? SOC_E_NONE:
                                                             SOC_E_FAIL;
    }
    return (rc);
}

int clmac_preemption_tx_enable_get(int32 unit, soc_port_t port, uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                    reg_val, PREEMPT_ENABLEf);

    return (SOC_E_NONE);
}

int clmac_preemption_verify_enable_set(int32 unit, soc_port_t port,
                                       uint32 enable)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                  reg_val, VERIFY_ENABLEf);

    if (value != enable) {
        soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                              &reg_val, VERIFY_ENABLEf, enable? 1: 0);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_verify_enable_get(int32 unit, soc_port_t port,
                                       uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                    reg_val, VERIFY_ENABLEf);

    return (SOC_E_NONE);
}

int clmac_preemption_verify_time_set(int32 unit, soc_port_t port,
                                     uint32 time_val)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                  reg_val, VERIFY_TIMEf);

    if (value != time_val) {
        soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                              &reg_val, VERIFY_TIMEf, time_val);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_verify_time_get(int32 unit, soc_port_t port,
                                     uint32 *time_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *time_val = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                    reg_val, VERIFY_TIMEf);

    return (SOC_E_NONE);
}

int clmac_preemption_verify_attempts_set(int32 unit, soc_port_t port,
                                         uint32 num_attempts)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                  reg_val, VERIFY_ATTEMPT_LIMITf);

    if (value != num_attempts) {
        SOC_IF_ERROR_RETURN(soc_reg_field_validate(unit, CLMAC_MERGE_CONFIGr,
                                                   VERIFY_ATTEMPT_LIMITf,
                                                   num_attempts));
        soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                              &reg_val, VERIFY_ATTEMPT_LIMITf, num_attempts);
        SOC_IF_ERROR_RETURN(WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_verify_attempts_get(int32 unit, soc_port_t port,
                                         uint32 *num_attempts)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *num_attempts = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                    reg_val, VERIFY_ATTEMPT_LIMITf);

    return (SOC_E_NONE);
}

int clmac_preemption_tx_status_get(int unit, soc_port_t port,
                                   int flags, uint32 *value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_VERIFY_STATEr(unit, port, &reg_val));
    
    switch (flags) {
        case CLMAC_PREEMPTION_TX_STATUS:
            *value = soc_reg64_field32_get(unit, CLMAC_MERGE_VERIFY_STATEr,
                                           reg_val, PREEMPTION_TX_STATUSf); 
            break;
        case CLMAC_PREEMPTION_VERIFY_STATUS:
            *value = soc_reg64_field32_get(unit, CLMAC_MERGE_VERIFY_STATEr,
                                           reg_val, PREEMPTION_VERIFY_STATUSf); 
            break;
        case CLMAC_PREEMPTION_VERIFY_CNT_STATUS:
            *value = soc_reg64_field32_get(unit, CLMAC_MERGE_VERIFY_STATEr,
                                           reg_val, TX_VERIFY_CNT_STATUSf); 
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}
                                    
int clmac_preemption_force_rx_lost_eop_set(int unit, soc_port_t port,
                                           int flags, uint32 value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    
    switch (flags) {
        case CLMAC_PREEMPTION_FORCE_SW_RX_EOP:
            /*
             * for forcing sw based rx lost eop, the sequence is to
             * write a zero to the field SW_FORCE_RX_LOST_EOPf wait
             * for a delay 10usecs, write a 1 to the field 
             * SW_FORCE_RX_LOST_EOP
             */
            if (value) {
                soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                                      &reg_val, SW_FORCE_RX_LOST_EOPf, 0);
                SOC_IF_ERROR_RETURN(
                        WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
                sal_udelay(10);
            }
            soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                                  &reg_val, SW_FORCE_RX_LOST_EOPf, value);
            SOC_IF_ERROR_RETURN(
                    WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
            break;
        case CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP:
            soc_reg64_field32_set(unit, CLMAC_MERGE_CONFIGr,
                              &reg_val, AUTO_HW_FORCE_RX_LOST_EOPf, value);
            SOC_IF_ERROR_RETURN(
                    WRITE_CLMAC_MERGE_CONFIGr(unit, port, reg_val));
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}
                                    
int clmac_preemption_force_rx_lost_eop_get(int unit, soc_port_t port,
                                           int flags, uint32 *value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_CLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    
    switch (flags) {
        case CLMAC_PREEMPTION_FORCE_SW_RX_EOP:
            /* get the sw rx lost eop setting */
            *value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                           reg_val, SW_FORCE_RX_LOST_EOPf); 
            break;
        case CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP:
            /* get the hw rx lost eop setting */
            *value = soc_reg64_field32_get(unit, CLMAC_MERGE_CONFIGr,
                                      reg_val, AUTO_HW_FORCE_RX_LOST_EOPf);
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}

int clmac_preemption_rx_timeout_info_set(int unit, soc_port_t port,
                                         uint32 enable, uint32 timeout_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(
            READ_CLMAC_MERGE_RX_TIMEOUT_CONFIGr(unit, port, &reg_val));
    
    soc_reg64_field32_set(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                          &reg_val, TIMEOUT_ENABLEf, enable? 1: 0);
    
    soc_reg64_field32_set(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                          &reg_val, TIMEOUT_CNTf, timeout_val);

    SOC_IF_ERROR_RETURN(
            WRITE_CLMAC_MERGE_RX_TIMEOUT_CONFIGr(unit, port, reg_val));
    
    return SOC_E_NONE;
}
                                    
int clmac_preemption_rx_timeout_info_get(int unit, soc_port_t port,
                                         uint32 *enable, uint32 *timeout_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(
            READ_CLMAC_MERGE_RX_TIMEOUT_CONFIGr(unit, port, &reg_val));
    
    *enable = soc_reg64_field32_get(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                                    reg_val, TIMEOUT_ENABLEf);
    
    *timeout_val = soc_reg64_field32_get(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                                         reg_val, TIMEOUT_CNTf);

    return SOC_E_NONE;
}
/*!
 * clmac_timesync_tx_info_get
 *
 * @brief get port timestamps and sequence id form fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  tx _info        - timestamp and seq id form fifo
 */
int clmac_timesync_tx_info_get (int unit, int port,
                                portmod_fifo_status_t* tx_info)
{
    uint64 rval;

    RD_CLMAC(TX_TIMESTAMP_FIFO_STATUSr, unit, port, &rval);

    if (soc_reg64_field32_get(unit, CLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                  ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    RD_CLMAC(TX_TIMESTAMP_FIFO_DATAr, unit, port, &rval);

    tx_info->timestamps_in_fifo = soc_reg64_field32_get(unit,
                     CLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);

    tx_info->timestamps_in_fifo_hi = 0;

    tx_info->sequence_id = soc_reg64_field32_get(unit,
                     CLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, SEQUENCE_IDf);

    return (SOC_E_NONE);

}
                                    
/*!
 * clmac_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  ts_info        - timestamp from fifo
 */
int clmac_diag_fifo_status_get (int unit, int port,
                                const portmod_fifo_status_t* ts_info)
{
    uint64 rval;
    portmod_fifo_status_t* pinfo =(portmod_fifo_status_t*)ts_info;

    RD_CLMAC(TX_TIMESTAMP_FIFO_STATUSr, unit, port, &rval);

    if (soc_reg64_field32_get(unit, CLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                              ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    RD_CLMAC(TX_TIMESTAMP_FIFO_DATAr, unit, port, &rval);

    pinfo->timestamps_in_fifo = soc_reg64_field32_get(unit,
                                                CLMAC_TX_TIMESTAMP_FIFO_DATAr,
                                                rval, TIME_STAMPf);

    return (SOC_E_NONE);
}

int clmac_tx_stall_enable_get(int unit, int port, int* enable)
{
    uint64 mac_ctrl;

    RD_CLMAC(TX_CTRLr, unit, port, &mac_ctrl);

    if (soc_reg_field_valid(unit, CLMAC_TX_CTRLr, STALL_TXf)) {
        *enable = (int)soc_reg64_field32_get(unit, CLMAC_TX_CTRLr,
                                      mac_ctrl, STALL_TXf);
    } else {
        return (SOC_E_UNAVAIL);
    }

    return (SOC_E_NONE);
}

int clmac_tx_stall_enable_set(int unit, int port, int enable)
{
    uint64 mac_ctrl;

    COMPILER_64_ZERO(mac_ctrl);

    RD_CLMAC(TX_CTRLr, unit, port, &mac_ctrl);

    if (soc_reg_field_valid(unit, CLMAC_TX_CTRLr, STALL_TXf)) {
        soc_reg64_field32_set(unit, CLMAC_TX_CTRLr, &mac_ctrl,
                           STALL_TXf, enable ? 1 : 0);
        WR_CLMAC(TX_CTRLr, unit, port, mac_ctrl);
    } else {
        return (SOC_E_UNAVAIL);
    }

    return (SOC_E_NONE);
}

#undef _ERR_MSG_MODULE_NAME


#endif
