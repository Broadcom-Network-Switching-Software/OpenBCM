/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/clmac.h>
#include <soc/drv.h>
#include <soc/ua.h>

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
            SOC_IF_ERROR_RETURN(READ_CLG2MAC_##name##r(unit, port, val)); \
        } else { \
            SOC_IF_ERROR_RETURN(UA_REG_READ_PORT(unit, CLMAC_##name, port, 0, val)); \
        } \
    } while(0)

#define WR_CLMAC(name, unit, port, val) do { \
        if (soc_apache_port_is_clg2_port(unit, port)) { \
            SOC_IF_ERROR_RETURN(WRITE_CLG2MAC_##name##r(unit, port, val)); \
        } else { \
            SOC_IF_ERROR_RETURN(UA_REG_WRITE_PORT(unit, CLMAC_##name, port, 0, &val)); \
        } \
    } while(0)

#define RD_CLMAC_64_FROM_32_ARRAY(name, unit, port, val) do {   \
                    if (soc_apache_port_is_clg2_port(unit, port)) { \
                        SOC_IF_ERROR_RETURN(READ_CLG2MAC_##name##r(unit, port, val)); \
                    } else { \
                        uint32 _a[2] = {0};                                    \
                        SOC_IF_ERROR_RETURN(UA_REG_READ_PORT(unit, CLMAC_##name, port, 0, _a)); \
                        COMPILER_64_SET(*val, _a[1], _a[0]);                    \
                    } \
                } while(0)

#define WR_CLMAC_64_TO_32_ARRAY(name, unit, port, val) do { \
                    if (soc_apache_port_is_clg2_port(unit, port)) { \
                        SOC_IF_ERROR_RETURN(WRITE_CLG2MAC_##name##r(unit, port, val)); \
                    } else { \
                        uint32 _a[2] = {0};                                  \
                        _a[0] = COMPILER_64_LO(val);                         \
                        _a[1] = COMPILER_64_HI(val);                         \
                        SOC_IF_ERROR_RETURN(UA_REG_WRITE_PORT(unit, CLMAC_##name, port, 0, _a)); \
                    } \
                } while(0)

#else

#define RD_CLMAC(name, unit, port, val) SOC_IF_ERROR_RETURN(UA_REG_READ_PORT(unit, CLMAC_##name, port, 0, val));
#define WR_CLMAC(name, unit, port, val) SOC_IF_ERROR_RETURN(UA_REG_WRITE_PORT(unit, CLMAC_##name, port, 0, &val));

#define RD_CLMAC_64_FROM_32_ARRAY(name, unit, port, val) do {   \
            uint32 _a[2] = {0};                                    \
            SOC_IF_ERROR_RETURN(UA_REG_READ_PORT(unit, CLMAC_##name, port, 0, _a));\
            COMPILER_64_SET(*val, _a[1], _a[0]);                    \
        } while(0)

#define WR_CLMAC_64_TO_32_ARRAY(name, unit, port, val) do { \
            uint32 _a[2] = {0};                                    \
            _a[0] = COMPILER_64_LO(val);                         \
            _a[1] = COMPILER_64_HI(val);                         \
            SOC_IF_ERROR_RETURN(UA_REG_WRITE_PORT(unit, CLMAC_##name, port, 0, _a));\
        } while(0)

#endif

STATIC
int _clmac_version_get(int unit, soc_port_t port, int32 *version)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    if (!UA_REG_IS_VALID(unit, CLMAC_VERSION_ID)) {
        *version = -1;
    }
    else {
        RD_CLMAC(VERSION_ID, unit, port, &reg_val);
        UA_REG_FIELD_GET(unit, CLMAC_VERSION_ID, &reg_val, CLMAC_VERSION, version);
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
    uint32 crc_mode, is_ipg_check_disable;
    uint32 is_strip_crc, is_append_crc, is_replace_crc, is_higig, is_pass_through_crc;
    uint64 reg_val;
    uint32 fld_val = 0;
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
    RD_CLMAC(RX_CTRL, unit, port, &reg_val);
    fld_val =(uint32)is_strip_crc;
    UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, STRIP_CRC, &fld_val);
    fld_val = is_higig ? 0 : 1;
    UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, STRICT_PREAMBLE, &fld_val);
    fld_val = CLMAC_RUNT_THRESHOLD_ETH;
    UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, RUNT_THRESHOLD, &fld_val);
    WR_CLMAC(RX_CTRL, unit, port, reg_val);

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
    RD_CLMAC(TX_CTRL, unit, port, &reg_val);
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, CRC_MODE, &crc_mode);
    fld_val = is_higig ? CLMAC_AVERAGE_IPG_HIGIG : CLMAC_AVERAGE_IPG_DEFAULT;
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, AVERAGE_IPG, &fld_val);
    fld_val = CLMAC_TX_PREAMBLE_LENGTH;
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, TX_PREAMBLE_LENGTH, &fld_val);
    WR_CLMAC(TX_CTRL, unit, port, reg_val);

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
    RD_CLMAC(CTRL, unit, port, &reg_val);
    fld_val = 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, LOCAL_LPBK, &fld_val);
    fld_val = 1;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, TX_EN, &fld_val);
    fld_val = 1;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, RX_EN, &fld_val);
    fld_val = 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, SOFT_RESET, &fld_val); /*Deassert SOFT_RESET*/
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, XGMII_IPG_CHECK_DISABLE, &is_ipg_check_disable);
    fld_val = 1;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, ALLOW_40B_AND_GREATER_PKTS, &fld_val);
    WR_CLMAC(CTRL, unit, port, reg_val);

    RD_CLMAC(RX_MAX_SIZE, unit, port, &reg_val);
    fld_val = CLMAC_JUMBO_MAXSZ;
    UA_REG_FIELD_SET(unit, CLMAC_RX_MAX_SIZE, &reg_val, RX_MAX_SIZE, &fld_val);
    WR_CLMAC(RX_MAX_SIZE, unit, port, reg_val);


exit:
    SOC_FUNC_RETURN;
}

int clmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint64 reg_val, orig_reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    RD_CLMAC(CTRL, unit, port, &reg_val);
    COMPILER_64_COPY(orig_reg_val, reg_val);

    if (flags & CLMAC_ENABLE_SET_FLAGS_TX_EN) {
        fld_val = enable ? 1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, TX_EN, &fld_val);
    } 
    if (flags & CLMAC_ENABLE_SET_FLAGS_RX_EN) {
        fld_val = enable ? 1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, RX_EN, &fld_val);
    } 
    if (!(flags & CLMAC_ENABLE_SET_FLAGS_RX_EN) && !(flags & CLMAC_ENABLE_SET_FLAGS_TX_EN)) {
        fld_val = enable ? 1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, TX_EN, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, RX_EN, &fld_val);
    }
    if(COMPILER_64_NE(reg_val, orig_reg_val)) { /* write only if value changed */
        WR_CLMAC(CTRL, unit, port, reg_val);
    }

    /*
     * check if mac soft reset operation is needed
     * if enable = 1, bring mac out of reset, else mac in reset.
     */
    if (!(flags & CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS)) {
        RD_CLMAC(CTRL, unit, port, &reg_val);
        COMPILER_64_COPY(orig_reg_val, reg_val);
        fld_val = enable ? 0 : 1;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, SOFT_RESET, &fld_val);
    }    

    if(COMPILER_64_NE(reg_val, orig_reg_val)) {/* write only if value changed */
       SOC_IF_ERROR_RETURN(clmac_soft_reset_set(unit, port, !enable));
    }

    SOC_FUNC_RETURN;
}

int clmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(CTRL, unit, port, &reg_val);
    if (flags & CLMAC_ENABLE_SET_FLAGS_TX_EN) {
        UA_REG_FIELD_GET(unit, CLMAC_CTRL, &reg_val, TX_EN, &fld_val);
    } else {
        UA_REG_FIELD_GET(unit, CLMAC_CTRL, &reg_val, RX_EN, &fld_val);
    }
    *enable = fld_val;
    

    SOC_FUNC_RETURN;
}

int clmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;
    uint32 fld_val = 0;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    fld_val = enable? 1 : 0;
    UA_REG_FIELD_SET (unit, CLMAC_CTRL, &reg_val,
                           SOFT_RESET, &fld_val);
    WR_CLMAC(CTRL, unit, port, reg_val);
    /*
     * Special handling for new mac version. Internally MAC loopback
     * looks for rising edge on MAC loopback configuration to enter
     * loopback state.
     * Do only if loopback is enabled on the port.
     */
    UA_REG_FIELD_GET(unit, CLMAC_CTRL, &reg_val, LOCAL_LPBK, &fld_val);
    if (!enable && fld_val) {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, LOCAL_LPBK, &fld_val);
        WR_CLMAC(CTRL, unit, port, reg_val);
        /* Wait 10usec as suggested by MAC designer */
        sal_udelay(10);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, LOCAL_LPBK, &fld_val);
        WR_CLMAC(CTRL, unit, port, reg_val);
    }
    return (SOC_E_NONE);
}


int clmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;
    uint32 fld_val = 0;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_CTRL, &reg_val, SOFT_RESET, &fld_val);
    *enable = fld_val ? 1 : 0;

    return (SOC_E_NONE);
}

int clmac_timestamp_byte_adjust_set(int unit, soc_port_t port, int flags, int speed)
{
    uint64 ctrl;
    uint32 byte_adjust = 0;
    uint32 fld_val = 0;
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

#ifdef BCM_ACCESS_SUPPORT
    if (UA_REG_IS_VALID(unit, CLMAC_TIMESTAMP_BYTE_ADJUST)) {
#else
    if (SOC_REG_PORT_VALID(unit, CLMAC_TIMESTAMP_BYTE_ADJUSTr, port)) {
#endif
        if ((speed > 10000) || (flags == CLMAC_TIMESTAMP_BYTE_ADJUST_CLEAR)) {
            /* This shouldn't be enabled when speed is more than 10G */
            RD_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, &ctrl);
            fld_val = 0;
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              TX_TIMER_BYTE_ADJUST, &fld_val);
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              TX_TIMER_BYTE_ADJUST_EN, &fld_val);
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              RX_TIMER_BYTE_ADJUST, &fld_val);
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              RX_TIMER_BYTE_ADJUST_EN, &fld_val);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, ctrl);
        } else {
            RD_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, &ctrl);
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              TX_TIMER_BYTE_ADJUST, &byte_adjust);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, ctrl);

            RD_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, &ctrl);
            fld_val = 1;
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              TX_TIMER_BYTE_ADJUST_EN, &fld_val);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, ctrl);

            RD_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, &ctrl);
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              RX_TIMER_BYTE_ADJUST, &byte_adjust);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, ctrl);

            RD_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, &ctrl);
            fld_val = 1;
            UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_BYTE_ADJUST, &ctrl,
                              RX_TIMER_BYTE_ADJUST_EN, &fld_val);
            WR_CLMAC(TIMESTAMP_BYTE_ADJUST, unit, port, ctrl);
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
    uint32 osts_delay = 0;
    uint32 tsts_delay = 0;

    if (UA_REG_IS_VALID(unit, CLMAC_TIMESTAMP_ADJUST)) {

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
        RD_CLMAC(TIMESTAMP_ADJUST, unit, port, &ctrl);
        UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_ADJUST, &ctrl,
                              TS_OSTS_ADJUST, &osts_delay);
        WR_CLMAC(TIMESTAMP_ADJUST, unit, port, ctrl);

        /* (2.5 TSC_CLK period + 1 TS_CLK period */
        if (ts_adjust.tsts_adjust > 0) {
            tsts_delay = ts_adjust.tsts_adjust;
        }
#ifdef BCM_TIMESYNC_SUPPORT
        else {
            tsts_delay = SOC_TIMESYNC_PLL_CLOCK_NS(unit) + ((5 * tx_clk_ns ) / 2);
        }
#endif
        RD_CLMAC(TIMESTAMP_ADJUST, unit, port, &ctrl);
        UA_REG_FIELD_SET(unit, CLMAC_TIMESTAMP_ADJUST, &ctrl,
                              TS_TSTS_ADJUST, &tsts_delay);
        WR_CLMAC(TIMESTAMP_ADJUST, unit, port, ctrl);
    }


    return SOC_E_NONE;
}

int clmac_timestamp_delay_get(int unit, soc_port_t port,
                              portmod_port_ts_adjust_t *ts_adjust)
{
    uint64 reg_val;
    uint32 fld_val = 0;

    if (UA_REG_IS_VALID(unit, CLMAC_TIMESTAMP_ADJUST)) {
        RD_CLMAC(TIMESTAMP_ADJUST, unit, port, &reg_val);
        UA_REG_FIELD_GET (unit, CLMAC_TIMESTAMP_ADJUST, &reg_val,
                                     TS_OSTS_ADJUST, &fld_val);
        ts_adjust->osts_adjust = fld_val;
        UA_REG_FIELD_GET (unit, CLMAC_TIMESTAMP_ADJUST, &reg_val,
                                     TS_TSTS_ADJUST, &fld_val);
        ts_adjust->tsts_adjust = fld_val;
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
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    COMPILER_64_ZERO(orig_reg_val);

    SOC_IF_ERROR_RETURN(clmac_enable_get(unit, port, 0, &enable));

    /* disable before updating the speed */	
    if (enable) {
        if (flags & CLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= CLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(clmac_enable_set(unit, port, enable_flags, 0));
    }

    RD_CLMAC(MODE, unit, port, &reg_val);
    COMPILER_64_ADD_64(orig_reg_val, reg_val);
    fld_val = ((speed>= 10000) || (speed==0)) ? 4 : 2;
    UA_REG_FIELD_SET(unit, CLMAC_MODE, &reg_val, SPEED_MODE, &fld_val);

    if(COMPILER_64_NE(orig_reg_val, reg_val)) {
        WR_CLMAC(MODE, unit, port, reg_val);
    }

    /*
     * Set REMOTE_FAULT/LOCAL_FAULT for CL ports,
     * else HW Linkscan interrupt would be suppressed.
     */
    if (UA_REG_IS_VALID(unit, CLPORT_FAULT_LINK_STATUS)) {
        fault = speed <= 1000 ? 0 : 1;
        SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLPORT_FAULT_LINK_STATUS, port, 0, &rval));
        UA_REG_FIELD_SET(unit, CLPORT_FAULT_LINK_STATUS, &rval,
                         REMOTE_FAULT, &fault);
        UA_REG_FIELD_SET(unit, CLPORT_FAULT_LINK_STATUS, &rval,
                         LOCAL_FAULT, &fault);
        SOC_IF_ERROR_RETURN(UA_REG_WRITE(unit, CLPORT_FAULT_LINK_STATUS, port, 0, &rval));
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
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    switch(lb){
        case portmodLoopbackMacOuter:
            RD_CLMAC(CTRL, unit, port, &reg_val);
            fld_val = enable ? 1: 0;
            UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, LOCAL_LPBK, &fld_val);
            WR_CLMAC(CTRL, unit, port, reg_val);
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
            RD_CLMAC(CTRL, unit, port, &reg_val);
            UA_REG_FIELD_GET(unit, CLMAC_CTRL, &reg_val, LOCAL_LPBK, enable);
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
    uint32 fld_val = 0;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    fld_val = enable ? 1: 0;
    UA_REG_FIELD_SET (unit, CLMAC_CTRL, &reg_val,
                           TX_EN, &fld_val);
    WR_CLMAC(CTRL, unit, port, reg_val);

    return(SOC_E_NONE);
}


int clmac_tx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_CTRL, port, 0,  &reg_val));

    UA_REG_FIELD_GET (unit, CLMAC_CTRL, &reg_val, TX_EN, enable);

    return(SOC_E_NONE);
}

int clmac_rx_enable_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;
    uint32 fld_value = 0;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    fld_value = enable ? 1: 0;
    UA_REG_FIELD_SET (unit, CLMAC_CTRL, &reg_val,
                           RX_EN, &fld_value);
    WR_CLMAC(CTRL, unit, port, reg_val);

    return(SOC_E_NONE);
}

int clmac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET (unit, CLMAC_CTRL, &reg_val, RX_EN, enable);

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
    uint32 fld_value = 0;

    RD_CLMAC(CTRL, unit, port, &mac_ctrl);
    RD_CLMAC(RX_CTRL, unit, port, &rx_ctrl);
    RD_CLMAC(TX_CTRL, unit, port, &tx_ctrl);

    mac_ctrl_orig = mac_ctrl;

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    fld_value = 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, RX_EN, &fld_value);
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, TX_EN, &fld_value);

    WR_CLMAC(CTRL, unit, port, mac_ctrl);

    if (hg_mode != SOC_ENCAP_IEEE) { /* HiGig Mode */
        fld_value = 1;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl_orig, XGMII_IPG_CHECK_DISABLE, &fld_value);
        fld_value = CLMAC_AVERAGE_IPG_HIGIG;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &tx_ctrl, AVERAGE_IPG, &fld_value);
    } else {
        fld_value = 0;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl_orig, XGMII_IPG_CHECK_DISABLE, &fld_value);
        fld_value = CLMAC_AVERAGE_IPG_DEFAULT;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &tx_ctrl, AVERAGE_IPG, &fld_value);
    }

    WR_CLMAC(RX_CTRL, unit, port, rx_ctrl);
    WR_CLMAC(TX_CTRL, unit, port, tx_ctrl);
    WR_CLMAC(CTRL, unit, port, mac_ctrl_orig); /* restore mac ctrl */

    return (SOC_E_NONE);
}

int clmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap)
{
    uint32 val = 0, fld_value = 0;
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

    RD_CLMAC(MODE, unit, port, &reg_val);
    UA_REG_FIELD_SET(unit, CLMAC_MODE, &reg_val, HDR_MODE, &val);
    UA_REG_FIELD_SET(unit, CLMAC_MODE, &reg_val, NO_SOP_FOR_CRC_HG, &no_sop_for_crc_hg);
    WR_CLMAC(MODE, unit, port, reg_val);

    if(encap == SOC_ENCAP_HIGIG2){
        RD_CLMAC(CTRL, unit, port, &reg_val);
        fld_value = (flags & CLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN) ? 1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, EXTENDED_HIG2_EN, &fld_value);
        WR_CLMAC(CTRL, unit, port, reg_val);

        RD_CLMAC(RX_CTRL, unit, port, &reg_val);
        fld_value = CLMAC_RUNT_THRESHOLD_HIGIG2;
        UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, RUNT_THRESHOLD, &fld_value);
        WR_CLMAC(RX_CTRL, unit, port, reg_val);
    } else if (encap == SOC_ENCAP_HIGIG) {
        RD_CLMAC(RX_CTRL, unit, port, &reg_val);
        fld_value = CLMAC_RUNT_THRESHOLD_HIGIG;
        UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, RUNT_THRESHOLD, &fld_value);
        WR_CLMAC(RX_CTRL, unit, port, reg_val);
    } else {
        RD_CLMAC(RX_CTRL, unit, port, &reg_val);
        fld_value = CLMAC_RUNT_THRESHOLD_ETH;
        UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, RUNT_THRESHOLD, &fld_value);
        WR_CLMAC(RX_CTRL, unit, port, reg_val);
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

    RD_CLMAC(MODE, unit, port, &reg_val);
    UA_REG_FIELD_GET(unit, CLMAC_MODE, &reg_val, HDR_MODE, &fld_val);

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
         UA_REG_FIELD_GET(unit, CLMAC_MODE, &reg_val, NO_SOP_FOR_CRC_HG, &fld_val);
        if(fld_val) {
            (*flags) |= CLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG;
        }
    }

    if(*encap == SOC_ENCAP_HIGIG2){
        RD_CLMAC(CTRL, unit, port, &reg_val);
        UA_REG_FIELD_GET(unit, CLMAC_CTRL, &reg_val, EXTENDED_HIG2_EN, &fld_val);
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

    RD_CLMAC(RX_MAX_SIZE, unit, port, &reg_val);
    UA_REG_FIELD_SET(unit, CLMAC_RX_MAX_SIZE, &reg_val, RX_MAX_SIZE, &value);
    WR_CLMAC(RX_MAX_SIZE, unit, port, reg_val);

    SOC_FUNC_RETURN;
}

int clmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_MAX_SIZE, unit, port, &reg_val);
    UA_REG_FIELD_GET(unit, CLMAC_RX_MAX_SIZE, &reg_val, RX_MAX_SIZE, value);

    SOC_FUNC_RETURN;
}

int clmac_pad_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(TX_CTRL, unit, port, &reg_val);
    fld_val = value ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, PAD_EN, &fld_val);
    if(value){
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, PAD_THRESHOLD, &value);
    }
    WR_CLMAC(TX_CTRL, unit, port, reg_val);

    SOC_FUNC_RETURN;
}

int clmac_pad_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    uint32 pad_en;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(TX_CTRL, unit, port, &reg_val);
    UA_REG_FIELD_GET(unit, CLMAC_TX_CTRL, &reg_val, PAD_EN, &pad_en);
    if(!pad_en){
        *value = 0;
    }
    else{
        UA_REG_FIELD_GET(unit, CLMAC_TX_CTRL, &reg_val, PAD_THRESHOLD, value);
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

    RD_CLMAC(RX_CTRL, unit, port, &reg_val);
    UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, RUNT_THRESHOLD, &value);
    WR_CLMAC(RX_CTRL, unit, port, reg_val);

exit:
    SOC_FUNC_RETURN;
}

int clmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_CTRL, unit, port, &reg_val);
    UA_REG_FIELD_GET(unit, CLMAC_RX_CTRL, &reg_val, RUNT_THRESHOLD, value);

    SOC_FUNC_RETURN;
}

int clmac_strict_preamble_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    RD_CLMAC(RX_CTRL, unit, port, &reg_val);
    UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &reg_val, STRICT_PREAMBLE, &value);
    WR_CLMAC(RX_CTRL, unit, port, reg_val);

    return (SOC_E_NONE);
}

int clmac_strict_preamble_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_RX_CTRL, port, 0,  &reg_val));
    UA_REG_FIELD_GET (unit, CLMAC_RX_CTRL, &reg_val, STRICT_PREAMBLE, value);
    return (SOC_E_NONE);
}

int clmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag, int inner_vlan_tag)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_VLAN_TAG, unit, port, &reg_val);

    if(inner_vlan_tag == -1){
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_RX_VLAN_TAG, &reg_val, INNER_VLAN_TAG_ENABLE, &fld_val);
    } else{
        UA_REG_FIELD_SET(unit, CLMAC_RX_VLAN_TAG, &reg_val, INNER_VLAN_TAG, &inner_vlan_tag);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_RX_VLAN_TAG, &reg_val, INNER_VLAN_TAG_ENABLE, &fld_val);

    }

    if(outer_vlan_tag == -1){
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_RX_VLAN_TAG, &reg_val, OUTER_VLAN_TAG_ENABLE, &fld_val);
    } else {
        UA_REG_FIELD_SET(unit, CLMAC_RX_VLAN_TAG, &reg_val, OUTER_VLAN_TAG, &outer_vlan_tag);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_RX_VLAN_TAG, &reg_val, OUTER_VLAN_TAG_ENABLE, &fld_val);
    }

    WR_CLMAC(RX_VLAN_TAG, unit, port, reg_val);

    SOC_FUNC_RETURN;
}


int clmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_VLAN_TAG, unit, port, &reg_val);
    UA_REG_FIELD_GET(unit, CLMAC_RX_VLAN_TAG, &reg_val, INNER_VLAN_TAG_ENABLE, &field_val);

    if(field_val == 0){
        *inner_vlan_tag = -1;
    } else {
        UA_REG_FIELD_GET(unit, CLMAC_RX_VLAN_TAG, &reg_val, INNER_VLAN_TAG, inner_vlan_tag);
    }

    UA_REG_FIELD_GET(unit, CLMAC_RX_VLAN_TAG, &reg_val, OUTER_VLAN_TAG_ENABLE, &field_val);
    if(field_val == 0){
        *outer_vlan_tag = -1;
    } else {
        UA_REG_FIELD_GET(unit, CLMAC_RX_VLAN_TAG, &reg_val, OUTER_VLAN_TAG, outer_vlan_tag);
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

    WR_CLMAC_64_TO_32_ARRAY(RX_MAC_SA, unit, port, mac_addr);

    SOC_FUNC_RETURN;
}


int clmac_sw_link_status_select_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;
    uint32 fld_val = (uint32) enable;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    UA_REG_FIELD_SET (unit, CLMAC_CTRL, &reg_val, LINK_STATUS_SELECT, &fld_val);

    WR_CLMAC(CTRL, unit, port, reg_val);

    return (SOC_E_NONE);
}

int clmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET (unit, CLMAC_CTRL, &reg_val, LINK_STATUS_SELECT, enable);
    return (SOC_E_NONE);
}

int clmac_sw_link_status_set(int unit, soc_port_t port, int link)
{
    uint64 reg_val;
    uint32 fld_val = (uint32)link;

    RD_CLMAC(CTRL, unit, port, &reg_val);

    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &reg_val, SW_LINK_STATUS, &fld_val);
    WR_CLMAC(CTRL, unit, port, reg_val);
    return (SOC_E_NONE);
}
int clmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC_64_FROM_32_ARRAY(RX_MAC_SA, unit, port, &rval64);

    values[0] = COMPILER_64_HI(rval64);
    values[1] = COMPILER_64_LO(rval64);

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
    WR_CLMAC_64_TO_32_ARRAY(TX_MAC_SA, unit, port, mac_addr);

    SOC_FUNC_RETURN;
}

int clmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC_64_FROM_32_ARRAY(TX_MAC_SA, unit, port, &rval64);
    COMPILER_64_TO_32_HI(values[0], rval64);
    COMPILER_64_TO_32_LO(values[1], rval64);

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
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    RD_CLMAC(TX_CTRL, unit, port, &reg_val);

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 64 bytes (i.e. multiple of 8 bits) */
    val = val < 64 ? 64 : (val > 512 ? 512 : (val + 7) & (0x7f << 3));

    fld_val = val/8;
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, AVERAGE_IPG, &fld_val);

    WR_CLMAC(TX_CTRL, unit, port, reg_val);

    SOC_FUNC_RETURN
}

int clmac_tx_average_ipg_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);

    RD_CLMAC(TX_CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_TX_CTRL, &reg_val, AVERAGE_IPG, &fld_val);
    *val = fld_val * 8;

    SOC_FUNC_RETURN
}

int clmac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint64 reg_val;
    uint32 fld_val = length;

    if(length > 8){
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                       "runt size should be small than 8. got %d"), length));
        return (SOC_E_PARAM);
    }

    COMPILER_64_ZERO(reg_val);

    RD_CLMAC(TX_CTRL, unit, port, &reg_val);

    /* coverity[incompatible_cast:FALSE] */
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &reg_val, TX_PREAMBLE_LENGTH, &fld_val);
    WR_CLMAC(TX_CTRL, unit, port, reg_val);

    return (SOC_E_NONE);
}

/***************************************************************************** 
 * Remote/local Fault                                                        *
 *****************************************************************************/

int clmac_remote_fault_control_set(int unit, soc_port_t port, const portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &reg_val);
    fld_val = control->enable ? 0 : 1;/*flip*/
    UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &reg_val, REMOTE_FAULT_DISABLE, &fld_val);
    fld_val = control->drop_tx_on_fault ? 1: 0;
    UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &reg_val, DROP_TX_DATA_ON_REMOTE_FAULT, &fld_val);
    WR_CLMAC(RX_LSS_CTRL, unit, port, reg_val);

    SOC_FUNC_RETURN;
}

int clmac_remote_fault_control_get(int unit, soc_port_t port, portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_CTRL, &reg_val, REMOTE_FAULT_DISABLE, &field_val);
    control->enable = (field_val ? 0 : 1);/*flip*/

    UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_CTRL, &reg_val, DROP_TX_DATA_ON_REMOTE_FAULT, &field_val);
    control->drop_tx_on_fault  = (field_val ? 1 : 0);

    SOC_FUNC_RETURN;
}

int clmac_local_fault_control_set(int unit, soc_port_t port, const portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &reg_val);
    fld_val = control->enable ? 0 : 1; /*flip*/
    UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &reg_val, LOCAL_FAULT_DISABLE, &fld_val);
    fld_val = control->drop_tx_on_fault ? 1: 0;
    UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &reg_val, DROP_TX_DATA_ON_LOCAL_FAULT, &fld_val);
    WR_CLMAC(RX_LSS_CTRL, unit, port, reg_val);

    SOC_FUNC_RETURN;
}


int clmac_local_fault_control_get(int unit, soc_port_t port, portmod_local_fault_control_t *control)
{
    uint64 reg_val;
    uint32 field_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_CTRL, &reg_val, LOCAL_FAULT_DISABLE, &field_val);
    control->enable = (field_val ? 0 : 1);/*flip*/

    UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_CTRL, &reg_val, DROP_TX_DATA_ON_LOCAL_FAULT, &field_val);
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
        field_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_CLEAR_RX_LSS_STATUS, &reg_val_clear, CLEAR_REMOTE_FAULT_STATUS, &field_val);
    }

    RD_CLMAC(RX_LSS_STATUS, unit, port, &reg_val);

    if(clear_status) {
        WR_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, reg_val_clear);
        COMPILER_64_ZERO(reg_val_clear);
        WR_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, reg_val_clear);
    }

    UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_STATUS, &reg_val, REMOTE_FAULT_STATUS, &field_val);
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
        field_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_CLEAR_RX_LSS_STATUS, &reg_val_clear, CLEAR_LOCAL_FAULT_STATUS, &field_val);
    }

    RD_CLMAC(RX_LSS_STATUS, unit, port, &reg_val);

    if(clear_status) {
        WR_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, reg_val_clear);
        COMPILER_64_ZERO(reg_val_clear);
        WR_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, reg_val_clear);
    }

    UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_STATUS, &reg_val, LOCAL_FAULT_STATUS, &field_val);
    *status = field_val ? 1: 0;

    SOC_FUNC_RETURN;
}


int clmac_link_fault_os_set(int unit, int port, int is_remote, uint32 enable)
{
    uint64 rval;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    enable = enable ? 1 : 0;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &rval);

    if (enable) {
        if (is_remote) {
            fld_val = 1;
            UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, &fld_val);
            fld_val = 0;
            UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, &fld_val);
            fld_val = 2;
            UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FAULT_SOURCE_FOR_TX, &fld_val);
        } else {
            fld_val = 1;
            UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, &fld_val);
            fld_val = 0;
            UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, &fld_val);
            fld_val = 2;
            UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FAULT_SOURCE_FOR_TX, &fld_val);
        }
    } else {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, FAULT_SOURCE_FOR_TX, &fld_val);
    }

    WR_CLMAC(RX_LSS_CTRL, unit, port, rval);

    SOC_FUNC_RETURN;
}

int clmac_link_fault_os_get(int unit, int port, int is_remote, uint32 * enable)
{
    uint64 rval;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &rval);

    if (is_remote) {
        UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_REMOTE_FAULT_OS, enable);
    } else {
        UA_REG_FIELD_GET(unit, CLMAC_RX_LSS_CTRL, &rval, FORCE_LOCAL_FAULT_OS, enable);
    }

    SOC_FUNC_RETURN;
}


int clmac_clear_rx_lss_status_set(int unit, soc_port_t port, int lcl_fault, int rmt_fault)
{
    uint64 reg_val;
    uint32 fld_val = 0;

    RD_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, &reg_val);
    fld_val =(uint32)rmt_fault;
    UA_REG_FIELD_SET(unit, CLMAC_CLEAR_RX_LSS_STATUS, &reg_val,
                                     CLEAR_REMOTE_FAULT_STATUS, &fld_val);
    fld_val = (uint32) lcl_fault;
    UA_REG_FIELD_SET(unit, CLMAC_CLEAR_RX_LSS_STATUS, &reg_val,
                                     CLEAR_LOCAL_FAULT_STATUS, &fld_val);
    WR_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, reg_val);

    return (SOC_E_NONE);
}

int clmac_clear_rx_lss_status_get(int unit, soc_port_t port, int *lcl_fault, int *rmt_fault)
{
    uint64 reg_val;
    uint32 fld_val = 0;

    RD_CLMAC(CLEAR_RX_LSS_STATUS, unit, port, &reg_val);
    UA_REG_FIELD_GET(unit, CLMAC_CLEAR_RX_LSS_STATUS, &reg_val,
                                     CLEAR_REMOTE_FAULT_STATUS, &fld_val);
    *rmt_fault = fld_val;
    UA_REG_FIELD_GET(unit, CLMAC_CLEAR_RX_LSS_STATUS, &reg_val,
                                     CLEAR_LOCAL_FAULT_STATUS, &fld_val);
    *lcl_fault = fld_val;

    return (SOC_E_NONE);
}

/**************************************************************************** 
 Flow Control
*****************************************************************************/

int clmac_pause_control_set(int unit, soc_port_t port, const portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PAUSE_CTRL, unit, port , &reg_val);

    if(control->rx_enable || control->tx_enable){
        if (control->refresh_timer > 0) {
            fld_val = (uint32)control->refresh_timer;
            UA_REG_FIELD_SET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_REFRESH_TIMER, &fld_val);
            fld_val = 1;
            UA_REG_FIELD_SET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_REFRESH_EN, &fld_val);
        } else {
            fld_val = 0;
            UA_REG_FIELD_SET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_REFRESH_EN, &fld_val);
        }
        fld_val = (uint32)control->xoff_timer;
        UA_REG_FIELD_SET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_XOFF_TIMER, &fld_val);
    }

    fld_val = control->tx_enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_PAUSE_CTRL, &reg_val, TX_PAUSE_EN, &fld_val);
    fld_val = control->rx_enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_PAUSE_CTRL, &reg_val, RX_PAUSE_EN, &fld_val);

    WR_CLMAC(PAUSE_CTRL, unit, port , reg_val);

    SOC_FUNC_RETURN;
}

int clmac_pfc_control_set(int unit, soc_port_t port, const portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    uint32 fld_len = 0, mask = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PFC_CTRL, unit, port , &reg_val);

    if (control->refresh_timer) {
        fld_len = UA_REG_FIELD_SIZE(unit, CLMAC_PFC_CTRL, PFC_REFRESH_TIMER);
        mask = (1 << fld_len) - 1;
        fld_val = control->refresh_timer & mask;
        UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_REFRESH_TIMER, &fld_val);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_REFRESH_EN, &fld_val);
    } else {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_REFRESH_EN, &fld_val);
    }
    fld_val = control->stats_en ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_STATS_EN, &fld_val);

    fld_len = UA_REG_FIELD_SIZE(unit, CLMAC_PFC_CTRL, PFC_REFRESH_TIMER);
    mask = (1 << fld_len) - 1;
    fld_val = control->xoff_timer & mask;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_XOFF_TIMER, &fld_val);
    fld_val = control->force_xon ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, FORCE_PFC_XON, &fld_val);

    fld_val = control->tx_enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, TX_PFC_EN, &fld_val);

    fld_val = control->rx_enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &reg_val, RX_PFC_EN, &fld_val);

    WR_CLMAC(PFC_CTRL, unit, port , reg_val);

    SOC_FUNC_RETURN;
}

int clmac_llfc_control_set(int unit, soc_port_t port, const portmod_llfc_control_t *control)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(LLFC_CTRL, unit, port , &reg_val);

    if(control->rx_enable || control->tx_enable){
        fld_val =(uint32)control->in_ipg_only;
        UA_REG_FIELD_SET(unit, CLMAC_LLFC_CTRL, &reg_val, LLFC_IN_IPG_ONLY, &fld_val);
        fld_val =(uint32)control->crc_ignore;
        UA_REG_FIELD_SET(unit, CLMAC_LLFC_CTRL, &reg_val, LLFC_CRC_IGNORE, &fld_val);
    }
    fld_val =(uint32)control->tx_enable;
    UA_REG_FIELD_SET(unit, CLMAC_LLFC_CTRL, &reg_val, TX_LLFC_EN, &fld_val);
    fld_val =(uint32)control->rx_enable;
    UA_REG_FIELD_SET(unit, CLMAC_LLFC_CTRL, &reg_val, RX_LLFC_EN, &fld_val);

    WR_CLMAC(LLFC_CTRL, unit, port , reg_val);

    SOC_FUNC_RETURN;
}

int clmac_pause_control_get(int unit, soc_port_t port, portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PAUSE_CTRL, unit, port , &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_REFRESH_EN, &refresh_enable);
    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_REFRESH_TIMER, &refresh_timer);
    control->refresh_timer = (refresh_enable ? refresh_timer : -1);
    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &reg_val, PAUSE_XOFF_TIMER, &fld_val);
    control->xoff_timer = fld_val;
    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &reg_val, RX_PAUSE_EN, &fld_val);
    control->rx_enable = fld_val;
    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &reg_val, TX_PAUSE_EN, &fld_val);
    control->tx_enable = fld_val;

    SOC_FUNC_RETURN;
}

int clmac_pfc_control_get (int unit, soc_port_t port,
                           portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(PFC_CTRL, unit, port , &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_REFRESH_EN, &refresh_enable);
    if (refresh_enable) {
        UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_REFRESH_TIMER, &refresh_timer);
    }
    control->refresh_timer = refresh_timer;
    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_STATS_EN, &fld_val);
    control->stats_en = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, PFC_XOFF_TIMER, &fld_val);
    control->xoff_timer = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, FORCE_PFC_XON, &fld_val);
    control->force_xon = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, RX_PFC_EN, &fld_val);
    control->rx_enable = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL, &reg_val, TX_PFC_EN, &fld_val);
    control->tx_enable = fld_val;

    SOC_FUNC_RETURN;
}

int clmac_llfc_control_get(int unit, soc_port_t port, portmod_llfc_control_t *control)
{
    uint64 reg_val;
    uint32 fld_val = 0;
    SOC_INIT_FUNC_DEFS;

    RD_CLMAC(LLFC_CTRL, unit, port , &reg_val);

    UA_REG_FIELD_GET(unit, CLMAC_LLFC_CTRL, &reg_val, LLFC_IN_IPG_ONLY, &fld_val);
    control->in_ipg_only = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_LLFC_CTRL, &reg_val, LLFC_CRC_IGNORE, &fld_val);
    control->crc_ignore = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_LLFC_CTRL, &reg_val, RX_LLFC_EN, &fld_val);
    control->rx_enable = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_LLFC_CTRL, &reg_val, TX_LLFC_EN, &fld_val);
    control->tx_enable = fld_val;

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
    uint32 fld_val = 0;

    RD_CLMAC(MODE, unit, port, &rval);
    UA_REG_FIELD_GET(unit, CLMAC_MODE, &rval, SPEED_MODE, &fld_val);
    switch (fld_val) {
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

    RD_CLMAC(EEE_CTRL, unit, port, &rval);
    UA_REG_FIELD_SET(unit, CLMAC_EEE_CTRL, &rval, EEE_EN, &eee->enable);
    WR_CLMAC(EEE_CTRL, unit, port, rval);

    RD_CLMAC(EEE_TIMERS, unit, port, &rval);
    UA_REG_FIELD_SET(unit, CLMAC_EEE_TIMERS, &rval, EEE_DELAY_ENTRY_TIMER, &eee->tx_idle_time);
    UA_REG_FIELD_SET(unit, CLMAC_EEE_TIMERS, &rval, EEE_WAKE_TIMER, &eee->tx_wake_time);
    WR_CLMAC(EEE_TIMERS, unit, port, rval);

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

    RD_CLMAC(EEE_CTRL, unit, port, &rval);
    UA_REG_FIELD_GET(unit, CLMAC_EEE_CTRL, &rval, EEE_EN, &pEEE->enable);

    RD_CLMAC(EEE_TIMERS, unit, port, &rval);

    UA_REG_FIELD_GET (unit, CLMAC_EEE_TIMERS, &rval, EEE_DELAY_ENTRY_TIMER, &pEEE->tx_idle_time);
    UA_REG_FIELD_GET (unit, CLMAC_EEE_TIMERS, &rval, EEE_WAKE_TIMER, &pEEE->tx_wake_time);

    return(SOC_E_NONE);
}

int clmac_pass_control_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRL, unit, port, &rval);

    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_RX_CTRL, RX_PASS_PFC)) {
        fld_val = value ?  1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &rval, RX_PASS_CTRL, &fld_val);
    }
    WR_CLMAC(RX_CTRL, unit, port, rval);

    return(SOC_E_NONE);
}

int clmac_pass_control_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRL, unit, port, &rval);
    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_RX_CTRL, RX_PASS_PFC)) {
        UA_REG_FIELD_GET(unit, CLMAC_RX_CTRL, &rval, RX_PASS_CTRL, &fld_val);
        *value = fld_val;
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
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRL, unit, port, &rval);
    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_RX_CTRL, RX_PASS_PFC)) {
        fld_val = value? 1:0;
        UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &rval, RX_PASS_PFC, &fld_val);
    }
    WR_CLMAC(RX_CTRL, unit, port, rval);

    return(SOC_E_NONE);
}

int clmac_pass_pfc_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRL, unit, port, &rval);
    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_RX_CTRL, RX_PASS_PFC)) {
        UA_REG_FIELD_GET(unit, CLMAC_RX_CTRL, &rval, RX_PASS_PFC, &fld_val);
        *value = fld_val;
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
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRL, unit, port, &rval);
    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_RX_CTRL, RX_PASS_PFC)) {
        fld_val = value ? 1 : 0;
        UA_REG_FIELD_SET (unit, CLMAC_RX_CTRL, &rval, RX_PASS_PAUSE, &fld_val);
    }
    WR_CLMAC(RX_CTRL, unit, port, rval);

    return(SOC_E_NONE);
}

int clmac_pass_pause_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    RD_CLMAC(RX_CTRL, unit, port, &rval);
    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_RX_CTRL, RX_PASS_PFC)) {
        UA_REG_FIELD_GET(unit, CLMAC_RX_CTRL, &rval, RX_PASS_PAUSE, &fld_val);
        *value = fld_val;
    }

    return(SOC_E_NONE);
}

int clmac_lag_failover_loopback_set(int unit, int port, int val)
{
    uint64 rval;
    uint32 fld_val = val;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    RD_CLMAC(LAG_FAILOVER_STATUS, unit, port, &rval);

    UA_REG_FIELD_SET (unit, CLMAC_LAG_FAILOVER_STATUS, &rval,
                      LAG_FAILOVER_LOOPBACK, &fld_val);
    WR_CLMAC(LAG_FAILOVER_STATUS, unit, port, rval);

    SOC_FUNC_RETURN;
}

int clmac_lag_failover_loopback_get(int unit, int port, int *val)
{
    uint64 rval;
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    /* Toggle link bit to notify IPIPE on link up */
    RD_CLMAC(LAG_FAILOVER_STATUS, unit, port, &rval);

    UA_REG_FIELD_GET(unit, CLMAC_LAG_FAILOVER_STATUS, &rval,
                     LAG_FAILOVER_LOOPBACK, &fld_val);
    *val = fld_val;

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
    uint32 fld_val = 0;

    RD_CLMAC(CTRL, unit, port, &mac_ctrl);

    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, LAG_FAILOVER_EN, &fld_val);
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, LINK_STATUS_SELECT, &fld_val);
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, REMOVE_FAILOVER_LPBK, &fld_val);

    WR_CLMAC(CTRL, unit, port, mac_ctrl);
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
    uint32 fld_val = val;

    RD_CLMAC(CTRL, unit, port, &mac_ctrl);

    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, LAG_FAILOVER_EN, &fld_val);

    WR_CLMAC(CTRL, unit, port, mac_ctrl);
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
 
    RD_CLMAC(CTRL, unit, port, &mac_ctrl);
 
    UA_REG_FIELD_GET (unit, CLMAC_CTRL, &mac_ctrl, LAG_FAILOVER_EN, val);
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

    RD_CLMAC(CTRL, unit, port, &mac_ctrl);

    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &mac_ctrl, REMOVE_FAILOVER_LPBK, &val);

    WR_CLMAC(CTRL, unit, port, mac_ctrl);
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
 
    RD_CLMAC(CTRL, unit, port, &mac_ctrl);
 
    UA_REG_FIELD_GET (unit, CLMAC_CTRL,
                             &mac_ctrl, REMOVE_FAILOVER_LPBK, val);
    return (SOC_E_NONE);
}

int clmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val)
{
    uint64 reg_val;
    uint32 fld_val = (uint32)val;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &reg_val);

    UA_REG_FIELD_SET (unit, CLMAC_RX_LSS_CTRL, &reg_val,
                           RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWN, &fld_val);

    WR_CLMAC(RX_LSS_CTRL, unit, port, reg_val);
    return (SOC_E_NONE);
}


int clmac_reset_fc_timers_on_link_dn_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;

    RD_CLMAC(RX_LSS_CTRL, unit, port, &reg_val);

    UA_REG_FIELD_GET (unit, CLMAC_RX_LSS_CTRL, &reg_val,
                           RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWN, val);
    return (SOC_E_NONE);
}


int clmac_drain_cell_get (int unit, int port, portmod_drain_cells_t *drain_cells)
{   
    uint64 rval;
    uint32 fld_val = 0;

    RD_CLMAC(PFC_CTRL, unit, port, &rval);
    UA_REG_FIELD_GET(unit, CLMAC_PFC_CTRL,
                     &rval, RX_PFC_EN, &fld_val);
    drain_cells->rx_pfc_en = fld_val;

    RD_CLMAC(LLFC_CTRL, unit, port, &rval);
    UA_REG_FIELD_GET(unit, CLMAC_LLFC_CTRL,
                     &rval, RX_LLFC_EN, &fld_val);
    drain_cells->llfc_en = fld_val;

    RD_CLMAC(PAUSE_CTRL, unit, port , &rval);
    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &rval, RX_PAUSE_EN, &fld_val);
    drain_cells->rx_pause = fld_val;

    UA_REG_FIELD_GET(unit, CLMAC_PAUSE_CTRL, &rval, TX_PAUSE_EN, &fld_val);
    drain_cells->tx_pause = fld_val;

    return (0);
}    

int clmac_discard_set(int unit, soc_port_t port, int discard)
{
    uint64 rval;
    uint32 fld_val = (uint32)discard;

    /* Clear Discard fields */
    RD_CLMAC(TX_CTRL, unit, port, &rval);
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &rval, DISCARD, &fld_val);
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &rval, EP_DISCARD, &fld_val);
    WR_CLMAC(TX_CTRL, unit, port, rval);
    return (SOC_E_NONE);
}

int clmac_drain_cell_stop (int unit, int port, const portmod_drain_cells_t *drain_cells)
{   
    uint64 rval;
    uint32 fld_val = 0;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(clmac_discard_set(unit, port, 0));

    /* set pause fields */
    RD_CLMAC(PAUSE_CTRL, unit, port , &rval);
    fld_val = (uint32)drain_cells->rx_pause;
    UA_REG_FIELD_SET (unit, CLMAC_PAUSE_CTRL,
                           &rval, RX_PAUSE_EN, &fld_val);
    fld_val = (uint32)drain_cells->tx_pause;
    UA_REG_FIELD_SET (unit, CLMAC_PAUSE_CTRL,
                           &rval, TX_PAUSE_EN, &fld_val);
    WR_CLMAC(PAUSE_CTRL, unit, port , rval);

    /* set pfc rx_en fields */
    RD_CLMAC(PFC_CTRL, unit, port, &rval);
    fld_val = (uint32)drain_cells->rx_pfc_en;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL,
                          &rval, RX_PFC_EN, &fld_val);
    WR_CLMAC(PFC_CTRL, unit, port, rval);

    /* set llfc rx_en fields */
    RD_CLMAC(LLFC_CTRL, unit, port, &rval);
    fld_val = (uint32)drain_cells->llfc_en;
    UA_REG_FIELD_SET(unit, CLMAC_LLFC_CTRL,
                          &rval, RX_LLFC_EN, &fld_val);
    WR_CLMAC(LLFC_CTRL, unit, port, rval);

    return (0);
}

int clmac_drain_cell_start(int unit, int port)
{   
    uint64 rval;
    uint32 fld_val = 0;

    RD_CLMAC(CTRL, unit, port, &rval);

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    fld_val = 1;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &rval, TX_EN, &fld_val);
    /* Disable RX */
    fld_val = 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &rval, RX_EN, &fld_val);

    WR_CLMAC(CTRL, unit, port, rval);

    RD_CLMAC(PAUSE_CTRL, unit, port , &rval);
    fld_val = 0;
    UA_REG_FIELD_SET (unit, CLMAC_PAUSE_CTRL, &rval, RX_PAUSE_EN, &fld_val);
    WR_CLMAC(PAUSE_CTRL, unit, port, rval);

    RD_CLMAC(PFC_CTRL, unit, port, &rval);
    fld_val = 0;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_CTRL, &rval, RX_PFC_EN, &fld_val);
    WR_CLMAC(PFC_CTRL, unit, port, rval);

    RD_CLMAC(LLFC_CTRL, unit, port, &rval);
    fld_val = 0;
    UA_REG_FIELD_SET(unit, CLMAC_LLFC_CTRL, &rval, RX_LLFC_EN, &fld_val);
    WR_CLMAC(LLFC_CTRL, unit, port, rval);

    RD_CLMAC(CTRL, unit, port, &rval);
    fld_val = 1;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &rval, SOFT_RESET, &fld_val);
    WR_CLMAC(CTRL, unit, port, rval);
  
    SOC_IF_ERROR_RETURN(clmac_discard_set(unit, port, 1));

    return (0);
}

int
clmac_mac_ctrl_set(int unit, int port, uint64 mac_ctrl)
{
    uint64 fld_val;

    COMPILER_64_COPY(fld_val, mac_ctrl);
    WR_CLMAC(CTRL, unit, port, fld_val);

    return (0);
}


int clmac_txfifo_cell_cnt_get (int unit, int port, uint32* fval)
{
    uint64 rval;
    uint32 fld_val = 0;

    /* coverity[incompatible_cast:FALSE] */
    RD_CLMAC(TXFIFO_CELL_CNT, unit, port, &rval);
    UA_REG_FIELD_GET(unit, CLMAC_TXFIFO_CELL_CNT, &rval,
                                     CELL_CNT, &fld_val);
    *fval = fld_val;
    return (0);
}

int
clmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl, int *rx_en)
{
    uint64 ctrl;

    RD_CLMAC(CTRL, unit, port, &ctrl);
    *mac_ctrl = ctrl;

    UA_REG_FIELD_GET(unit, CLMAC_CTRL, &ctrl, RX_EN, rx_en);

    return (0);
}

int
clmac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint64 ctrl;
    uint32 fld_val = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    RD_CLMAC(CTRL, unit, port, &ctrl);
    fld_val = rx_en ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &ctrl, RX_EN, &fld_val);
    WR_CLMAC(CTRL, unit, port, ctrl);
    /*Bring mac out of reset */
    SOC_IF_ERROR_RETURN(clmac_soft_reset_set(unit, port, 0));

    return (0);
}

int
clmac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint64 ctrl, octrl;
    uint32 soft_reset = 0;
    uint32 fld_val = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    RD_CLMAC(CTRL, unit, port, &ctrl);
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    fld_val = 1;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &ctrl, TX_EN, &fld_val);
    fld_val = rx_en ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &ctrl, RX_EN, &fld_val);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing senario, CLMAC_CTRLr.SOFT_RESETf is 
         *      used to early check to see if this port is at disabled state 
         *      already.
         */
        UA_REG_FIELD_GET(unit, CLMAC_CTRL, &ctrl, SOFT_RESET, &soft_reset);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    WR_CLMAC(CTRL, unit, port, ctrl);

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
    uint32 fld_val = 0;

    *reset = 1;

    RD_CLMAC(CTRL, unit, port, &ctrl);
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    fld_val = enable ? 1 : 0;
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &ctrl, TX_EN, &fld_val);
    UA_REG_FIELD_SET(unit, CLMAC_CTRL, &ctrl, RX_EN, &fld_val);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        if (enable) {
            *reset = 0;
        } else {
            UA_REG_FIELD_GET(unit, CLMAC_CTRL, &ctrl, SOFT_RESET, &fld_val);
            if (fld_val) {
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
    uint32 fval = 0;
    uint64 rval;

    RD_CLMAC(PFC_TYPE, unit, port, &rval);
    fval = pfc_cfg->type;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_TYPE, &rval, PFC_ETH_TYPE, &fval);
    WR_CLMAC(PFC_TYPE, unit, port, rval);

    RD_CLMAC(PFC_OPCODE, unit, port, &rval);
    fval = pfc_cfg->opcode;
    UA_REG_FIELD_SET(unit, CLMAC_PFC_OPCODE, &rval, PFC_OPCODE, &fval);
    WR_CLMAC(PFC_OPCODE, unit, port, rval);

    RD_CLMAC_64_FROM_32_ARRAY(PFC_DA, unit, port, &rval);
    COMPILER_64_TO_32_LO(fval, rval);
    fval &= 0x00ffffff;
    fval |= (pfc_cfg->da_oui & 0xff) << 24;
    COMPILER_64_SET(rval,pfc_cfg->da_oui >> 8,fval);
    WR_CLMAC_64_TO_32_ARRAY(PFC_DA, unit, port, rval);

    RD_CLMAC_64_FROM_32_ARRAY(PFC_DA, unit, port, &rval);
    COMPILER_64_TO_32_LO(fval, rval);
    fval &= 0xff000000;
    fval |= pfc_cfg->da_nonoui;
    COMPILER_64_SET(rval,pfc_cfg->da_oui >> 8,fval);

    WR_CLMAC_64_TO_32_ARRAY(PFC_DA, unit, port, rval);

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
    uint32 fld_val = 0;
    portmod_pfc_config_t* pcfg = (portmod_pfc_config_t*) pfc_cfg;

    RD_CLMAC(PFC_TYPE, unit, port, &rval);

    UA_REG_FIELD_GET(unit, CLMAC_PFC_TYPE,
                     &rval, PFC_ETH_TYPE, &fld_val);
    pcfg->type = fld_val;

    RD_CLMAC(PFC_OPCODE, unit, port, &rval);
    UA_REG_FIELD_GET (unit, CLMAC_PFC_OPCODE, &rval, PFC_OPCODE, &fld_val);
    pcfg->opcode = fld_val;

    RD_CLMAC_64_FROM_32_ARRAY(PFC_DA, unit, port, &rval);
    COMPILER_64_TO_32_LO(fval0, rval);
    COMPILER_64_TO_32_HI(fval1, rval);
    pcfg->da_oui = (fval0 >> 24) | (fval1 << 8);

    RD_CLMAC_64_FROM_32_ARRAY(PFC_DA, unit, port, &rval);
    COMPILER_64_TO_32_LO(fval, rval);
    pcfg->da_nonoui = fval & 0xFFFFFF;

    return (SOC_E_NONE);
}

int clmac_e2ecc_hdr_get (int unit, int port, uint32_t *words)
{
    uint64 rval64;

    RD_CLMAC(E2ECC_MODULE_HDR_0, unit, port, &rval64);

    COMPILER_64_TO_32_HI(words[0], rval64);
    COMPILER_64_TO_32_LO(words[1], rval64);

    RD_CLMAC(E2ECC_MODULE_HDR_1, unit, port, &rval64);

    COMPILER_64_TO_32_HI(words[2], rval64);
    COMPILER_64_TO_32_LO(words[3], rval64);

    RD_CLMAC(E2ECC_DATA_HDR_0, unit, port, &rval64);

    COMPILER_64_TO_32_HI(words[4], rval64);
    COMPILER_64_TO_32_LO(words[5], rval64);

    RD_CLMAC(E2ECC_DATA_HDR_1, unit, port, &rval64);

    COMPILER_64_TO_32_HI(words[6], rval64);
    COMPILER_64_TO_32_LO(words[7], rval64);

    return SOC_E_NONE;
}

int clmac_e2ecc_hdr_set (int unit, int port, uint32_t *words)
{
    uint64 rval64;
    COMPILER_64_ZERO(rval64);

    COMPILER_64_SET(rval64,words[0],words[1]);

    WR_CLMAC(E2ECC_MODULE_HDR_0, unit, port, rval64);

    COMPILER_64_ZERO(rval64);

    COMPILER_64_SET(rval64,words[2],words[3]);

    WR_CLMAC(E2ECC_MODULE_HDR_1, unit, port, rval64);

    COMPILER_64_ZERO(rval64);

    COMPILER_64_SET(rval64,words[4],words[5]);

    WR_CLMAC(E2ECC_DATA_HDR_0, unit, port, rval64);

    COMPILER_64_ZERO(rval64);

    COMPILER_64_SET(rval64,words[6],words[7]);

    WR_CLMAC(E2ECC_DATA_HDR_1, unit, port, rval64);

    return SOC_E_NONE;
}

int clmac_e2e_enable_set(int unit, int port, int enable)
{
    uint64 rval64;
    uint32 fld_val =(uint32)enable;

    RD_CLMAC(E2E_CTRL, unit, port, &rval64);
    UA_REG_FIELD_SET(unit, CLMAC_E2E_CTRL, &rval64, E2E_ENABLE, &fld_val);
    WR_CLMAC(E2E_CTRL, unit, port, rval64);

    return (0);
}

int clmac_e2e_enable_get(int unit, int port, int *enable)
{
    uint64 rval64;

    RD_CLMAC(E2E_CTRL, unit, port, &rval64);
    UA_REG_FIELD_GET(unit, CLMAC_E2E_CTRL, &rval64, E2E_ENABLE, enable);

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
    uint32 fld_val = 0;

    if (spacing < 0 || spacing > 257) {
        return SOC_E_PARAM;
    }

    RD_CLMAC(TX_CTRL, unit, port, &mac_ctrl);

    if (spacing == CLMAC_THROT_10G_TO_5G) {
        fld_val = 21;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_DENOM, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_NUM, &fld_val);
    } else if (spacing == CLMAC_THROT_10G_TO_2P5G) {
        fld_val = 21;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_DENOM, &fld_val);
        fld_val = 63;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_NUM, &fld_val);
    } else if (spacing >= 8) {
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_DENOM, &spacing);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_NUM, &fld_val);
    } else {
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_DENOM, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl, THROT_NUM, &fld_val);
    }

    WR_CLMAC(TX_CTRL, unit, port, mac_ctrl);

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

    RD_CLMAC(TX_CTRL, unit, port, &mac_ctrl);

    UA_REG_FIELD_GET (unit, CLMAC_TX_CTRL,
                      &mac_ctrl, THROT_DENOM, spacing);

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

    _SOC_IF_ERR_EXIT(UA_REG_READ(unit, CLMAC_INTR_ENABLE, port, 0, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TX_PKT_UNDERFLOW, value);
                break;
        case portmodIntrTypeTxPktOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TX_PKT_OVERFLOW, value);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TX_LLFC_MSG_OVERFLOW, value);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TX_TS_FIFO_OVERFLOW, value);
                break;
        case portmodIntrTypeRxPktOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_RX_PKT_OVERFLOW, value);
                break;
        case portmodIntrTypeRxMsgOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_RX_MSG_OVERFLOW, value);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TX_CDC_SINGLE_BIT_ERR, value);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TX_CDC_DOUBLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_RX_CDC_SINGLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_RX_CDC_DOUBLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxTsMemSingleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_RX_TS_MEM_SINGLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxTsMemDoubleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_RX_TS_MEM_DOUBLE_BIT_ERR, value);
                break;
        case portmodIntrTypeLocalFaultStatus :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_LOCAL_FAULT_STATUS, value);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_REMOTE_FAULT_STATUS, value);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_LINK_INTERRUPTION_STATUS, value);
                break;
        case portmodIntrTypeTsEntryValid :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_ENABLE, &reg_val, EN_TS_ENTRY_VALID, value);
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

    _SOC_IF_ERR_EXIT(UA_REG_READ(unit, CLMAC_INTR_ENABLE, port, 0,  &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TX_PKT_UNDERFLOW, &value);
                break;
        case portmodIntrTypeTxPktOverflow :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TX_PKT_OVERFLOW, &value);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TX_LLFC_MSG_OVERFLOW, &value);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TX_TS_FIFO_OVERFLOW, &value);
                break;
        case portmodIntrTypeRxPktOverflow :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_RX_PKT_OVERFLOW, &value);
                break;
        case portmodIntrTypeRxMsgOverflow :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_RX_MSG_OVERFLOW, &value);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TX_CDC_SINGLE_BIT_ERR, &value);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TX_CDC_DOUBLE_BIT_ERR, &value);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_RX_CDC_SINGLE_BIT_ERR, &value);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_RX_CDC_DOUBLE_BIT_ERR, &value);
                break;
        case portmodIntrTypeRxTsMemSingleBitErr :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_RX_TS_MEM_SINGLE_BIT_ERR, &value);
                break;
        case portmodIntrTypeRxTsMemDoubleBitErr :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_RX_TS_MEM_DOUBLE_BIT_ERR, &value);
                break;
        case portmodIntrTypeLocalFaultStatus :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_LOCAL_FAULT_STATUS, &value);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_REMOTE_FAULT_STATUS, &value);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_LINK_INTERRUPTION_STATUS, &value);
                break;
        case portmodIntrTypeTsEntryValid :
                UA_REG_FIELD_SET(unit, CLMAC_INTR_ENABLE, &rval, EN_TS_ENTRY_VALID, &value);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
        break;
    }

    _SOC_IF_ERR_EXIT(UA_REG_WRITE(unit, CLMAC_INTR_ENABLE, port, 0, &rval));

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

    _SOC_IF_ERR_EXIT(UA_REG_READ(unit, CLMAC_INTR_STATUS, port, 0,  &reg_val));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_PKT_UNDERFLOW, value);
                break;
        case portmodIntrTypeTxPktOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_PKT_OVERFLOW, value);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_LLFC_MSG_OVERFLOW, value);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_TS_FIFO_OVERFLOW, value);
                break;
        case portmodIntrTypeRxPktOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_PKT_OVERFLOW, value);
                break;
        case portmodIntrTypeRxMsgOverflow :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_MSG_OVERFLOW, value);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_CDC_SINGLE_BIT_ERR, value);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_CDC_DOUBLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_CDC_SINGLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_CDC_DOUBLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxTsMemSingleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_TS_MEM_SINGLE_BIT_ERR, value);
                break;
        case portmodIntrTypeRxTsMemDoubleBitErr :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_TS_MEM_DOUBLE_BIT_ERR, value);
                break;
        case portmodIntrTypeLocalFaultStatus :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_LOCAL_FAULT_STATUS, value);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_REMOTE_FAULT_STATUS, value);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_LINK_INTERRUPTION_STATUS, value);
                break;
        case portmodIntrTypeTsEntryValid :
                 UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TS_ENTRY_VALID, value);
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
    uint32 fld_val = 0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_clmac_version_get(unit, port, &version));

    if (version < CLMAC_VERSION_A030) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(UA_REG_READ(unit, CLMAC_INTR_STATUS, port, 0,  &reg_val));

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_PKT_UNDERFLOW, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktUnderflow;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_PKT_OVERFLOW, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktOverflow;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_LLFC_MSG_OVERFLOW, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxLlfcMsgOverflow;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_TS_FIFO_OVERFLOW, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxTSFifoOverflow;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_PKT_OVERFLOW, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxPktOverflow;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_MSG_OVERFLOW, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxMsgOverflow;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_CDC_SINGLE_BIT_ERR, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcSingleBitErr;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TX_CDC_DOUBLE_BIT_ERR, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcDoubleBitErr;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_CDC_SINGLE_BIT_ERR, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxCdcSingleBitErr;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_CDC_DOUBLE_BIT_ERR, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxCdcDoubleBitErr;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_TS_MEM_SINGLE_BIT_ERR, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxTsMemSingleBitErr;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_RX_TS_MEM_DOUBLE_BIT_ERR, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxTsMemDoubleBitErr;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_LOCAL_FAULT_STATUS, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLocalFaultStatus;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_REMOTE_FAULT_STATUS, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRemoteFaultStatus;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_LINK_INTERRUPTION_STATUS, &fld_val);
    if (fld_val) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLinkInterruptionStatus;
    }

    UA_REG_FIELD_GET(unit, CLMAC_INTR_STATUS, &reg_val, SUM_TS_ENTRY_VALID, &fld_val);
    if (fld_val) {
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
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                                  &reg_val, PREEMPTION_MODE, &value);

    if (value != enable) {
        fld_val = enable ? 1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                              &reg_val, PREEMPTION_MODE, &fld_val);
        SOC_IF_ERROR_RETURN(UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_mode_get(int32 unit, soc_port_t port, uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG, &reg_val, PREEMPTION_MODE, enable);

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
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, PREEMPT_ENABLE, &value);

    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_ENABLE, &is_verify_set);


    /* if preemption already enabled, return */
    if (value == enable) {
        return rc;
    }

    /* Static configuration - no preemption negotaition with link partner*/
    fld_val = enable ? 1: 0;
    UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                          &reg_val, PREEMPT_ENABLE, &fld_val);
    SOC_IF_ERROR_RETURN(UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));

    if (is_verify_set) {
        /*
         * preemption negotiation enabled - wait for the negotiation
         * to complete.
         * Wait time >= (num verify attempts + 1) * (verify timeout + 1)
         */
        UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                         &reg_val, VERIFY_TIME, &timeout);
        UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                         &reg_val, VERIFY_ATTEMPT_LIMIT, &num_attempts);
        wait_time = (num_attempts + 1) * (timeout + 1);

        /* wait for (timeout + 1) millsecs, and check verify status */
        sal_udelay((timeout + 1) * 1000);

        /* check verification status */
        SOC_IF_ERROR_RETURN(
               UA_REG_READ(unit, CLMAC_MERGE_VERIFY_STATE, port, 0,  &v_reg_val));
        UA_REG_FIELD_GET(unit,
                         CLMAC_MERGE_VERIFY_STATE,
                         &v_reg_val,
                         PREEMPTION_VERIFY_STATUS, &verify_status);
        if ((verify_status == CLMAC_PREEMPT_VERIFY_SUCCEED) ||
            (verify_status == CLMAC_PREEMPT_VERIFY_FAILED)) {
            verify_op_done = 1;
        }

        while (!verify_op_done) { 
            sal_udelay((timeout + 1) * 1000);

            /* check verification status */
            SOC_IF_ERROR_RETURN(
                   UA_REG_READ(unit, CLMAC_MERGE_VERIFY_STATE, port, 0,  &v_reg_val));
            UA_REG_FIELD_GET(unit,
                             CLMAC_MERGE_VERIFY_STATE,
                             &v_reg_val,
                             PREEMPTION_VERIFY_STATUS, &verify_status);

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

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, PREEMPT_ENABLE, enable);

    return (SOC_E_NONE);
}

int clmac_preemption_verify_enable_set(int32 unit, soc_port_t port,
                                       uint32 enable)
{
    uint64 reg_val;
    uint32 value = 0;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_ENABLE, &value);

    if (value != enable) {
        value = enable? 1: 0;
        UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                         &reg_val, VERIFY_ENABLE, &value);
        SOC_IF_ERROR_RETURN(UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_verify_enable_get(int32 unit, soc_port_t port,
                                       uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_ENABLE, enable);

    return (SOC_E_NONE);
}

int clmac_preemption_verify_time_set(int32 unit, soc_port_t port,
                                     uint32 time_val)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_TIME, &value);

    if (value != time_val) {
        UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                         &reg_val, VERIFY_TIME, &time_val);
        SOC_IF_ERROR_RETURN(UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_verify_time_get(int32 unit, soc_port_t port,
                                     uint32 *time_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_TIME, time_val);

    return (SOC_E_NONE);
}

int clmac_preemption_verify_attempts_set(int32 unit, soc_port_t port,
                                         uint32 num_attempts)
{
    uint64 reg_val;
    int32 value = 0;
    uint32 fld_len = 0, mask = 0;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_ATTEMPT_LIMIT, &value);

    if (value != num_attempts) {
        fld_len = UA_REG_FIELD_SIZE(unit, CLMAC_MERGE_CONFIG, VERIFY_ATTEMPT_LIMIT);
        mask = (1 << fld_len) - 1;
        if  (((num_attempts & ~mask) != 0)) {
              return SOC_E_PARAM;
        }
        UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                              &reg_val, VERIFY_ATTEMPT_LIMIT, &num_attempts);
        SOC_IF_ERROR_RETURN(UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
    }

    return (SOC_E_NONE);
}

int clmac_preemption_verify_attempts_get(int32 unit, soc_port_t port,
                                         uint32 *num_attempts)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                     &reg_val, VERIFY_ATTEMPT_LIMIT, num_attempts);

    return (SOC_E_NONE);
}

int clmac_preemption_tx_status_get(int unit, soc_port_t port,
                                   int flags, uint32 *value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_VERIFY_STATE, port, 0,  &reg_val));
    
    switch (flags) {
        case CLMAC_PREEMPTION_TX_STATUS:
            UA_REG_FIELD_GET(unit, CLMAC_MERGE_VERIFY_STATE,
                             &reg_val, PREEMPTION_TX_STATUS, value);
            break;
        case CLMAC_PREEMPTION_VERIFY_STATUS:
            UA_REG_FIELD_GET(unit, CLMAC_MERGE_VERIFY_STATE,
                             &reg_val, PREEMPTION_VERIFY_STATUS, value);
            break;
        case CLMAC_PREEMPTION_VERIFY_CNT_STATUS:
            UA_REG_FIELD_GET(unit, CLMAC_MERGE_VERIFY_STATE,
                             &reg_val, TX_VERIFY_CNT_STATUS, value);
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
    uint32 fld_val = 0;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
    
    switch (flags) {
        case CLMAC_PREEMPTION_FORCE_SW_RX_EOP:
            /*
             * for forcing sw based rx lost eop, the sequence is to
             * write a zero to the field SW_FORCE_RX_LOST_EOPf wait
             * for a delay 10usecs, write a 1 to the field 
             * SW_FORCE_RX_LOST_EOP
             */
            if (value) {
                fld_val = 0;
                UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                                      &reg_val, SW_FORCE_RX_LOST_EOP, &fld_val);
                SOC_IF_ERROR_RETURN(
                        UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
                sal_udelay(10);
            }
            UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                                  &reg_val, SW_FORCE_RX_LOST_EOP, &value);
            SOC_IF_ERROR_RETURN(
                    UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
            break;
        case CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP:
            UA_REG_FIELD_SET(unit, CLMAC_MERGE_CONFIG,
                              &reg_val, AUTO_HW_FORCE_RX_LOST_EOP, &value);
            SOC_IF_ERROR_RETURN(
                    UA_REG_WRITE(unit, CLMAC_MERGE_CONFIG, port, 0,  &reg_val));
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

    SOC_IF_ERROR_RETURN(UA_REG_READ(unit, CLMAC_MERGE_CONFIG, port, 0, &reg_val));
    
    switch (flags) {
        case CLMAC_PREEMPTION_FORCE_SW_RX_EOP:
            /* get the sw rx lost eop setting */
            UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                             &reg_val, SW_FORCE_RX_LOST_EOP, value);
            break;
        case CLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP:
            /* get the hw rx lost eop setting */
            UA_REG_FIELD_GET(unit, CLMAC_MERGE_CONFIG,
                             &reg_val, AUTO_HW_FORCE_RX_LOST_EOP, value);
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
    uint32 fld_val = 0;

    SOC_IF_ERROR_RETURN(
            UA_REG_READ(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG, port, 0,  &reg_val));

    fld_val = enable? 1: 0;
    UA_REG_FIELD_SET(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG,
                          &reg_val, TIMEOUT_ENABLE, &fld_val);
    
    UA_REG_FIELD_SET(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG,
                          &reg_val, TIMEOUT_CNT, &timeout_val);

    SOC_IF_ERROR_RETURN(
            UA_REG_WRITE(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG, port, 0,  &reg_val));
    
    return SOC_E_NONE;
}
                                    
int clmac_preemption_rx_timeout_info_get(int unit, soc_port_t port,
                                         uint32 *enable, uint32 *timeout_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(
            UA_REG_READ(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG, port, 0,  &reg_val));
    
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG,
                     &reg_val, TIMEOUT_ENABLE, enable);
    
    UA_REG_FIELD_GET(unit, CLMAC_MERGE_RX_TIMEOUT_CONFIG,
                     &reg_val, TIMEOUT_CNT, timeout_val);

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
    uint32 fld_val = 0;

    RD_CLMAC(TX_TIMESTAMP_FIFO_STATUS, unit, port, &rval);

    UA_REG_FIELD_GET(unit, CLMAC_TX_TIMESTAMP_FIFO_STATUS, &rval,
                     ENTRY_COUNT, &fld_val);
    if (fld_val == 0) {
        return SOC_E_EMPTY;
    }

    RD_CLMAC(TX_TIMESTAMP_FIFO_DATA, unit, port, &rval);

    UA_REG_FIELD_GET(unit, CLMAC_TX_TIMESTAMP_FIFO_DATA, &rval, TIME_STAMP, &tx_info->timestamps_in_fifo);

    tx_info->timestamps_in_fifo_hi = 0;

    UA_REG_FIELD_GET(unit, CLMAC_TX_TIMESTAMP_FIFO_DATA, &rval, SEQUENCE_ID, &tx_info->sequence_id);

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
    uint32 fld_val = 0;

    portmod_fifo_status_t* pinfo =(portmod_fifo_status_t*)ts_info;

    RD_CLMAC(TX_TIMESTAMP_FIFO_STATUS, unit, port, &rval);

    UA_REG_FIELD_GET(unit, CLMAC_TX_TIMESTAMP_FIFO_STATUS, &rval,
                     ENTRY_COUNT, &fld_val);
    if (fld_val == 0) {
        return SOC_E_EMPTY;
    }

    RD_CLMAC(TX_TIMESTAMP_FIFO_DATA, unit, port, &rval);

    UA_REG_FIELD_GET(unit,
                     CLMAC_TX_TIMESTAMP_FIFO_DATA,
                     &rval, TIME_STAMP, &pinfo->timestamps_in_fifo);

    return (SOC_E_NONE);
}

int clmac_tx_stall_enable_get(int unit, int port, int* enable)
{
    uint64 mac_ctrl;
    uint32 fld_val = 0;

    RD_CLMAC(TX_CTRL, unit, port, &mac_ctrl);

    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_TX_CTRL, STALL_TX)) {
        /* coverity[incompatible_cast:FALSE] */
        UA_REG_FIELD_GET(unit, CLMAC_TX_CTRL,
                         &mac_ctrl, STALL_TX, &fld_val);
        *enable = fld_val;
    } else {
        return (SOC_E_UNAVAIL);
    }

    return (SOC_E_NONE);
}

int clmac_tx_stall_enable_set(int unit, int port, int enable)
{
    uint64 mac_ctrl;
    uint32 fld_val = 0;

    COMPILER_64_ZERO(mac_ctrl);

    RD_CLMAC(TX_CTRL, unit, port, &mac_ctrl);

    if (UA_REG_FIELD_IS_VALID(unit, CLMAC_TX_CTRL, STALL_TX)) {
        fld_val = enable ? 1 : 0;
        UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_ctrl,
                           STALL_TX, &fld_val);
        /* coverity[incompatible_cast:FALSE] */
        WR_CLMAC(TX_CTRL, unit, port, mac_ctrl);
    } else {
        return (SOC_E_UNAVAIL);
    }

    return (SOC_E_NONE);
}

int clmac_faults_status_get(int unit, int port,
                            portmod_port_fault_status_t *faults)
{
    SOC_INIT_FUNC_DEFS;

    /* Get local fault status. */
    _SOC_IF_ERR_EXIT(clmac_local_fault_status_get(unit, port, 0,
                                                  &faults->local));

    /* Get remote fault status. */
    _SOC_IF_ERR_EXIT(clmac_remote_fault_status_get(unit, port, 0,
                                                   &faults->remote));

exit:
    SOC_FUNC_RETURN;
}

int clmac_drop_tx_data_on_ordered_set_enable(int unit, int port, uint32 enable)
{
    uint64 rval;
    uint32 fld_val = 0;

    COMPILER_64_ZERO(rval);

    RD_CLMAC(RX_LSS_CTRL, unit, port, &rval);
    if (enable) {
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, DROP_TX_DATA_ON_LINK_INTERRUPT, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, DROP_TX_DATA_ON_REMOTE_FAULT, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, DROP_TX_DATA_ON_LOCAL_FAULT, &fld_val);
    } else {
        /* Set to the default of  HW when disable*/
        fld_val = 0;
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, DROP_TX_DATA_ON_LINK_INTERRUPT, &fld_val);
        fld_val = 1;
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, DROP_TX_DATA_ON_REMOTE_FAULT, &fld_val);
        UA_REG_FIELD_SET(unit, CLMAC_RX_LSS_CTRL, &rval, DROP_TX_DATA_ON_LOCAL_FAULT, &fld_val);
    }
    WR_CLMAC(RX_LSS_CTRL, unit, port, rval);

    return (SOC_E_NONE);
}

int clmac_tx_crc_mode_set(int unit, int port, clmac_tx_crc_mode_t crc_mode)
{
    uint64 mac_tx_ctrl;
    uint32 fld_val =(uint32)crc_mode;

    RD_CLMAC(TX_CTRL, unit, port, &mac_tx_ctrl);
    UA_REG_FIELD_SET(unit, CLMAC_TX_CTRL, &mac_tx_ctrl, CRC_MODE, &fld_val);
    WR_CLMAC(TX_CTRL, unit, port, mac_tx_ctrl);

    return (SOC_E_NONE);
}

int clmac_tx_crc_mode_get(int unit, int port, clmac_tx_crc_mode_t* crc_mode)
{
    uint64 mac_tx_ctrl;

    RD_CLMAC(TX_CTRL, unit, port, &mac_tx_ctrl);
    UA_REG_FIELD_GET(unit, CLMAC_TX_CTRL, &mac_tx_ctrl, CRC_MODE, crc_mode);

    return (SOC_E_NONE);
}

int clmac_strip_crc_set(int unit, int port, uint32 enable)
{
    uint64 mac_rx_ctrl;
    uint32 fld_val = enable;

    RD_CLMAC(RX_CTRL, unit, port, &mac_rx_ctrl);
    UA_REG_FIELD_SET(unit, CLMAC_RX_CTRL, &mac_rx_ctrl, STRIP_CRC, &fld_val);
    WR_CLMAC(RX_CTRL, unit, port, mac_rx_ctrl);

    return (SOC_E_NONE);
}

int clmac_strip_crc_get(int unit, int port, uint32* enable)
{
    uint64 mac_rx_ctrl;

    RD_CLMAC(RX_CTRL, unit, port, &mac_rx_ctrl);
    UA_REG_FIELD_GET(unit, CLMAC_RX_CTRL, &mac_rx_ctrl, STRIP_CRC, enable);

    return (SOC_E_NONE);
}

#undef _ERR_MSG_MODULE_NAME


#endif
