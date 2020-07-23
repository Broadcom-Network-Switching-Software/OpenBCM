/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/xlmac.h>
#include <soc/drv.h>

#if defined(PORTMOD_PM4X10_SUPPORT)


#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define JUMBO_MAXSZ  0x3fe8 /* Max legal value (per regsfile) */

/*
 * version device
 * 0xA035  Apache, Jericho, QAX, QMX
 * 0xA036  TD2P
 * 0xA037  Jericho+, Apache
 * 0xA041  TH2
 */
typedef enum {
    XLMAC_VERSION_A035 = 0xA035,
    XLMAC_VERSION_A036 = 0xA036,
    XLMAC_VERSION_A037 = 0xA037,
    XLMAC_VERSION_A041 = 0xA041
}xlmac_version_t;

#define XLMAC_AVERAGE_IPG_DEFAULT         12
#define XLMAC_AVERAGE_IPG_HIGIG           8

#define XLMAC_RUNT_THRESHOLD_ETH    64
#define XLMAC_RUNT_THRESHOLD_HIGIG  72
#define XLMAC_RUNT_THRESHOLD_HIGIG2 76

/*
 *  WAN mode throttling
 *  THROT_10G_TO_5G: throt_num=21 throt_denom=21
 *  THROT_10G_TO_2P5G: throt_num=63 throt_denom=21
 */
#define XLMAC_THROT_10G_TO_5G    256
#define XLMAC_THROT_10G_TO_2P5G  257

#ifdef BCM_APACHE_SUPPORT

int soc_apache_xlmac_b0_ports[] = {
    0,                /* PHY port 0 is invalid              */
    1, 1, 1, 1,       /* PHY ports 1-4:   PM 4x10  inst 0   */
    1, 1, 1, 1,       /* PHY ports 5-8:   PM 4x10  inst 1   */
    1, 1, 1, 1,       /* PHY ports 9-12:  PM 4x10  inst 2   */
    1, 1, 1, 1,       /* PHY ports 13-16: PM 4x10  inst 3   */
    0, 0, 0, 0,       /* PHY ports 17-20: PM 12x10 inst 0.0 */
    0, 0, 0, 0,       /* PHY ports 21-24: PM 12x10 inst 0.1 */
    0, 0, 0, 0,       /* PHY ports 25-28: PM 12x10 inst 0.2 */
    0, 0, 0, 0,       /* PHY ports 29-32: PM 4x25  inst 0   */
    0, 0, 0, 0,       /* PHY ports 33-36: PM 4x25  inst 1   */
    1, 1, 1, 1,       /* PHY ports 37-40: PM 4x10  inst 4   */
    1, 1, 1, 1,       /* PHY ports 41-44: PM 4x10  inst 5   */
    1, 1, 1, 1,       /* PHY ports 45-48: PM 4x10  inst 6   */
    1, 1, 1, 1,       /* PHY ports 49-52: PM 4x10  inst 7   */
    0, 0, 0, 0,       /* PHY ports 53-56: PM 12x10 inst 1.0 */
    0, 0, 0, 0,       /* PHY ports 57-60: PM 12x10 inst 1.1 */
    0, 0, 0, 0,       /* PHY ports 61-64: PM 12x10 inst 1.2 */
    0, 0, 0, 0,       /* PHY ports 65-68: PM 4x25  inst 2   */
    0, 0, 0, 0,       /* PHY ports 69-72: PM 4x25  inst 3   */
};


int soc_apache_port_is_xlb0_port(int unit, int port)
{
    if (soc_feature(unit, soc_feature_xlportb0)) {
        return (soc_apache_xlmac_b0_ports[SOC_INFO(unit).port_l2p_mapping[port]]);
    } else {
        return 0;
    }
}

#define RD_XLMAC(name, unit, port, val) do { \
        if (soc_apache_port_is_xlb0_port(unit, port)) { \
            SOC_IF_ERROR_RETURN(READ_XLMAC_B0_##name(unit, port, val)); \
        } else { \
            SOC_IF_ERROR_RETURN(READ_XLMAC_##name(unit, port, val)); \
        } \
    } while(0)

#define WR_XLMAC(name, unit, port, val) do { \
        if (soc_apache_port_is_xlb0_port(unit, port)) { \
            SOC_IF_ERROR_RETURN(WRITE_XLMAC_B0_##name(unit, port, val)); \
        } else { \
            SOC_IF_ERROR_RETURN(WRITE_XLMAC_##name(unit, port, val)); \
        } \
    } while(0)


#else

#define RD_XLMAC(name, unit, port, val) SOC_IF_ERROR_RETURN(READ_XLMAC_##name(unit, port, val));
#define WR_XLMAC(name, unit, port, val) SOC_IF_ERROR_RETURN(WRITE_XLMAC_##name(unit, port, val));

#endif
/*
 * Functions to Review for Coverage:
 * xlmac_loopback_set
 * xlmac_encap_set
 *
 * Fucntions to be added (may be)
 * mac_xl_egress_queue_drain
 * mac_xl_control_set
 * mac_xl_control_get
 * mac_xl_ability_local_get
 */

#define XLMAC_SPEED_10     0x0
#define XLMAC_SPEED_100    0x1
#define XLMAC_SPEED_1000   0x2
#define XLMAC_SPEED_2500   0x3
#define XLMAC_SPEED_10000  0x4

#ifdef BCM_TIMESYNC_SUPPORT
struct {
    int speed;
    uint32 clock_rate;
}_xlmac_clock_rate[] = {
    { 40000, 644 },
    { 20000, 322 },
    { 10000, 161  },
    { 5000,  390  },
    { 2500,  390 },
    { 1000,  390 },
    { 0,     390  },
};

void
_xlmac_speed_to_clock_rate(int unit, soc_port_t port, int speed,
                            uint32 *clock_rate)
{
    int idx;
    for (idx = 0;
         idx < sizeof(_xlmac_clock_rate) / sizeof(_xlmac_clock_rate[0]);
         idx++) {
        if (speed >=_xlmac_clock_rate[idx].speed) {
            *clock_rate = _xlmac_clock_rate[idx].clock_rate;
            return;
        }
    }
    *clock_rate = 0;
}
#endif

STATIC
int _xlmac_version_get(int unit, soc_port_t port, int32 *version)
{
    uint64 reg_val;

    if (!SOC_REG_IS_VALID(unit, XLMAC_VERSION_IDr)) {
        *version = -1;
    }
    else {
        RD_XLMAC(VERSION_IDr, unit, port, &reg_val);
        *version = soc_reg64_field32_get(unit, XLMAC_VERSION_IDr, reg_val, XLMAC_VERSIONf);
    }

    return (SOC_E_NONE);
}

int xlmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint64 reg_val, orig_reg_val;
    uint32 soft_reset;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    orig_reg_val = reg_val;

    if (flags & XLMAC_ENABLE_SET_FLAGS_TX_EN) {
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, TX_ENf, enable ? 1 : 0); 
    } 
    if (flags & XLMAC_ENABLE_SET_FLAGS_RX_EN) {
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, RX_ENf, enable ? 1 : 0); 
    } 
    if (!(flags & XLMAC_ENABLE_SET_FLAGS_RX_EN) && !(flags & XLMAC_ENABLE_SET_FLAGS_TX_EN)) {
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, TX_ENf, enable ? 1 : 0); 
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, RX_ENf, enable ? 1 : 0); 
    }
    /* write only if value changed */
    if(COMPILER_64_EQ(reg_val, orig_reg_val)) { 
        soft_reset = 
           soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, SOFT_RESETf);

        if (enable || (!enable && soft_reset)) {
            return (SOC_E_NONE);
        }
    }

    if (!(flags & XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS)) {
        soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, SOFT_RESETf, enable ? 0 : 1);
    }
    WR_XLMAC(CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}


int xlmac_discard_set(int unit, soc_port_t port, int discard)
{
    uint64 rval;

    RD_XLMAC(TX_CTRLr, unit, port, &rval);

    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, DISCARDf, discard);
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &rval, EP_DISCARDf, discard);

    WR_XLMAC(TX_CTRLr, unit, port, rval);

    return (SOC_E_NONE);
}

int xlmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    if (flags & XLMAC_ENABLE_SET_FLAGS_TX_EN) {
        *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, TX_ENf); 
    } else {
        *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, RX_ENf); 
    }

    return (SOC_E_NONE);
}


int xlmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                           SOFT_RESETf, enable? 1 : 0);
    WR_XLMAC(CTRLr, unit, port, reg_val);
  
    return (SOC_E_NONE);
}


int xlmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    *enable = soc_reg64_field32_get(unit, XLMAC_CTRLr, reg_val, SOFT_RESETf)?1:0;
    return (SOC_E_NONE);
}

/*
 * Function:
 *      xlmac_timestamp_byte_adjust_set
 * Purpose:
 *      Set timestamp byte adjust values.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      flags - CLMAC_TIMESTAMP_BYTE_ADJUST_XX, for special operations.
 *      speed - data rate of the port.
 * Returns:
 *      SOC_E_XXX
 */
int
xlmac_timestamp_byte_adjust_set(int unit, soc_port_t port, int flags, int speed)
{
    uint64 ctrl;
    uint32 byte_adjust = 0;
    int32 version;
    soc_reg_t byte_adjust_reg = -1;

    if (100 == speed) {
        byte_adjust = 80;
    } else if (1000 == speed) {
        byte_adjust = 8;
    } else if (2500 == speed) {
        byte_adjust = 3;
    } else if (10000 == speed) {
        byte_adjust = 1;
    }

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));
    if (SOC_REG_PORT_VALID(unit, XLMAC_B0_TIMESTAMP_BYTE_ADJUSTr, port) &&
        (version == XLMAC_VERSION_A037)) {
        byte_adjust_reg = XLMAC_B0_TIMESTAMP_BYTE_ADJUSTr;
    } else if (SOC_REG_PORT_VALID(unit, XLMAC_TIMESTAMP_BYTE_ADJUSTr, port)) {
        byte_adjust_reg = XLMAC_TIMESTAMP_BYTE_ADJUSTr;
    }

    if (byte_adjust_reg != -1) {
        if ((speed > 10000) || (flags == XLMAC_TIMESTAMP_BYTE_ADJUST_CLEAR)) {
            /* This shouldn't be enabled when speed is more than 10G */
            RD_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              TX_TIMER_BYTE_ADJUSTf, 0);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              TX_TIMER_BYTE_ADJUST_ENf, 0);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              RX_TIMER_BYTE_ADJUSTf, 0);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              RX_TIMER_BYTE_ADJUST_ENf, 0);
            WR_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);
        } else {
            RD_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              TX_TIMER_BYTE_ADJUSTf, byte_adjust);
            WR_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);

            RD_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              TX_TIMER_BYTE_ADJUST_ENf, 1);
            WR_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);

            RD_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              RX_TIMER_BYTE_ADJUSTf, byte_adjust);
            WR_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);

            RD_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, &ctrl);
            soc_reg64_field32_set(unit, byte_adjust_reg, &ctrl,
                              RX_TIMER_BYTE_ADJUST_ENf, 1);
            WR_XLMAC(TIMESTAMP_BYTE_ADJUSTr, unit, port, ctrl);
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      xlmac_timestamp_delay_set
 * Purpose:
 *      Set Timestamp delay for one-step to account for lane and pipeline delay.
 * Parameters:
 *      unit - XGS unit #.
 *      port - Port number on unit.
 *      speed - Speed
 * Returns:
 *      SOC_E_XXX
 */
int xlmac_timestamp_delay_set(int unit, soc_port_t port,
                                          portmod_port_ts_adjust_t ts_adjust, int speed)
{
    uint64 ctrl;
    int osts_delay = 0;
    int tsts_delay = 0;

    if (SOC_REG_IS_VALID(unit, XLMAC_TIMESTAMP_ADJUSTr)) {

#ifdef BCM_TIMESYNC_SUPPORT
        uint32 clk_rate, tx_clk_ns;
    int    divisor;
    _xlmac_speed_to_clock_rate(unit, port, speed, &clk_rate);

        /* Tx clock rate for single/dual/quad phy mode */
    if ((speed >= 5000) && (speed <= 40000)) {
        divisor   = speed > 20000 ? 1 : speed > 10000 ? 2 : 4;
            /* tx clock rate in ns */
            tx_clk_ns = ((1000 / clk_rate) / divisor);
    } else {
            /* Same tx clk rate for < 10G  for all phy modes*/
        tx_clk_ns = 1000 / clk_rate;   
    }
#endif
    
    /* 
     * MAC pipeline delay for XGMII/XGMII mode is:
         *          = (6 * TX line clock period) + (Timestamp clock period)
     */
    /* signed value of pipeline delay in ns */
        if (ts_adjust.osts_adjust > 0) {
            osts_delay = ts_adjust.osts_adjust;
        }
#ifdef BCM_TIMESYNC_SUPPORT
        else {
            osts_delay = SOC_TIMESYNC_PLL_CLOCK_NS(unit) + ((12 * tx_clk_ns ) / 2);
        }
#endif
        RD_XLMAC(TIMESTAMP_ADJUSTr, unit, port, &ctrl);
    soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_OSTS_ADJUSTf, osts_delay);
        WR_XLMAC(TIMESTAMP_ADJUSTr, unit, port, ctrl);

        /* (2.5 TSC_CLK period + 1 TS_CLK period */
        if (ts_adjust.tsts_adjust > 0) {
            tsts_delay = ts_adjust.tsts_adjust;
        }
#ifdef BCM_TIMESYNC_SUPPORT
        else {
            tsts_delay = SOC_TIMESYNC_PLL_CLOCK_NS(unit) + ((5 * tx_clk_ns ) / 2);
        }
#endif
        RD_XLMAC(TIMESTAMP_ADJUSTr, unit, port, &ctrl);
        soc_reg64_field32_set(unit, XLMAC_TIMESTAMP_ADJUSTr, &ctrl,
                              TS_TSTS_ADJUSTf, tsts_delay);
    WR_XLMAC(TIMESTAMP_ADJUSTr, unit, port, ctrl);
    }

    return SOC_E_NONE;
}

int xlmac_timestamp_delay_get(int unit, soc_port_t port,
                              portmod_port_ts_adjust_t *ts_adjust)
{
    uint64 reg_val;

    if (SOC_REG_IS_VALID(unit, XLMAC_TIMESTAMP_ADJUSTr)) {
        RD_XLMAC(TIMESTAMP_ADJUSTr, unit, port, &reg_val);
        ts_adjust->osts_adjust = soc_reg64_field32_get (unit, XLMAC_TIMESTAMP_ADJUSTr, reg_val,
                                     TS_OSTS_ADJUSTf);
        ts_adjust->tsts_adjust = soc_reg64_field32_get (unit, XLMAC_TIMESTAMP_ADJUSTr, reg_val,
                                     TS_TSTS_ADJUSTf);
    } else {
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}


int xlmac_speed_set(int unit, soc_port_t port, int flags, int speed)
{
    uint32 spd_fld;
    int    enable, enable_flags = 0;
    uint64 rval;
    uint32 reg32_val = 0, fault = 0;

    if (speed >= 10000)       /* 10G Speed */
        spd_fld = 4; 
    else if (speed >= 2500)   /* 2.5G Speed */
        spd_fld = 3; 
    else if (speed >= 1000)   /* 1G Speed */
        spd_fld = 2; 
    else if (speed >= 100)    /* 100M Speed */
        spd_fld = 1; 
    else
        spd_fld = 0;          /* 10M Speed */ 

    SOC_IF_ERROR_RETURN(xlmac_enable_get(unit, port, 0, &enable));

    /* disable before updating the speed */ 
    if (enable) {
        if (flags & XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, enable_flags, 0));
    }

    /* Update the speed */
    RD_XLMAC(MODEr, unit ,port, &rval);
    soc_reg64_field32_set(unit, XLMAC_MODEr, &rval, SPEED_MODEf, spd_fld);
    WR_XLMAC(MODEr, unit ,port, rval);

    /*
     * Set REMOTE_FAULT/LOCAL_FAULT for XL ports,
     * else HW Linkscan interrupt would be suppressed.
     */
    if (SOC_REG_IS_VALID(unit, XLPORT_FAULT_LINK_STATUSr)) {
        fault = speed <= 1000 ? 0 : 1;

        SOC_IF_ERROR_RETURN(READ_XLPORT_FAULT_LINK_STATUSr(unit, port, &reg32_val));
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &reg32_val,
                          REMOTE_FAULTf, fault);
        soc_reg_field_set(unit, XLPORT_FAULT_LINK_STATUSr, &reg32_val,
                          LOCAL_FAULTf, fault);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_FAULT_LINK_STATUSr(unit, port, reg32_val));
    }

    if (enable) {
        if (flags & XLMAC_SPEED_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, enable_flags, 1));
    }

    return SOC_E_NONE;
}


int xlmac_loopback_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                           LOCAL_LPBKf, enable ? 1: 0);
    WR_XLMAC(CTRLr, unit, port, reg_val);

    return(SOC_E_NONE);
}

int xlmac_loopback_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    *enable = soc_reg64_field32_get (unit, XLMAC_CTRLr, reg_val, 
                                     LOCAL_LPBKf);
    return(SOC_E_NONE);
}

int xlmac_tx_enable_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val,
                           TX_ENf, enable ? 1: 0);
    WR_XLMAC(CTRLr, unit, port, reg_val);

    return(SOC_E_NONE);
}

int xlmac_tx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    *enable = soc_reg64_field32_get (unit, XLMAC_CTRLr, reg_val,
                                     TX_ENf);

    return(SOC_E_NONE);
}

int xlmac_rx_enable_set (int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val,
                           RX_ENf, enable ? 1: 0);
    WR_XLMAC(CTRLr, unit, port, reg_val);

    return(SOC_E_NONE);
}

int xlmac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    *enable = soc_reg64_field32_get (unit, XLMAC_CTRLr, reg_val,
                                     RX_ENf);

    return(SOC_E_NONE);
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
_xlmac_port_mode_update(int unit, soc_port_t port, int hg_mode)
{
    uint64 tx_ctrl, rx_ctrl, mac_ctrl, mac_ctrl_orig;

    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);
    RD_XLMAC(RX_CTRLr, unit, port, &rx_ctrl);
    RD_XLMAC(TX_CTRLr, unit, port, &tx_ctrl);

    mac_ctrl_orig = mac_ctrl; 

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    WR_XLMAC(CTRLr, unit, port, mac_ctrl);

    if (hg_mode != SOC_ENCAP_IEEE) { /* HiGig Mode */
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl_orig, XGMII_IPG_CHECK_DISABLEf, 1);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, XLMAC_AVERAGE_IPG_HIGIG);
    } else {
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl_orig, XGMII_IPG_CHECK_DISABLEf, 0);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, XLMAC_AVERAGE_IPG_DEFAULT);
    }

    WR_XLMAC(RX_CTRLr, unit, port, rx_ctrl);
    WR_XLMAC(TX_CTRLr, unit, port, tx_ctrl);
    WR_XLMAC(CTRLr, unit, port, mac_ctrl_orig); /* restore mac ctrl */

    return (SOC_E_NONE);
}


int xlmac_encap_set(int unit, soc_port_t port, int flags, portmod_encap_t encap)
{
    uint64 reg_val;
    uint32 encap_val= 0, no_sop_for_crc_hg = 0, ext_hi2=0;
    int    enable, enable_flags = 0;

    switch(encap){
        case SOC_ENCAP_IEEE:
            encap_val = 0;
            break;
        case SOC_ENCAP_HIGIG:
            encap_val = 1;
            no_sop_for_crc_hg = 
               (flags & XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG)?1:0;
            break;
        case SOC_ENCAP_HIGIG2:
        case SOC_ENCAP_HIGIG2_LITE:
            encap_val = 2;
            no_sop_for_crc_hg = 
               (flags & XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG)?1:0;
            ext_hi2 = (flags & XLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN) ? 1 : 0;
            break;
        case SOC_ENCAP_SOP_ONLY:
            encap_val = 5;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_XLMAC,
                      (BSL_META_U(unit, "illegal encap mode %d"), encap));
            return (SOC_E_PARAM); 
            break;
    }

    SOC_IF_ERROR_RETURN(xlmac_enable_get(unit, port, 0, &enable));
    
    if (enable) {
        if (flags & XLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        /* Turn off TX/RX enable */
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, enable_flags, 0));
    }

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit)) {
        /* Greyhound need mode update for all encap mode change! */
        SOC_IF_ERROR_RETURN(_xlmac_port_mode_update(unit, port, encap));

        /* GH specific : for HG2-LITE is sepecial case in PortMacro(PM).
         * - encap mode in PM must be IEEE (out of PM must be HG2-LITE)
         */
        encap_val = (encap == SOC_ENCAP_HIGIG2_LITE) ? 0 : encap_val;
    } else
#endif
    if ((IS_E_PORT(unit, port) && encap != SOC_ENCAP_IEEE) ||
            (IS_ST_PORT(unit, port) && encap == SOC_ENCAP_IEEE)) {

        SOC_IF_ERROR_RETURN(_xlmac_port_mode_update(unit, port, encap));
    }                       
                 
    RD_XLMAC(MODEr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_MODEr, &reg_val, HDR_MODEf, encap_val);
    soc_reg64_field32_set (unit, XLMAC_MODEr, &reg_val, NO_SOP_FOR_CRC_HGf, 
                           no_sop_for_crc_hg);

    WR_XLMAC(MODEr, unit, port, reg_val);

    /* Enable HiGig2 Mode */
    if(encap == SOC_ENCAP_HIGIG2) {
        RD_XLMAC(CTRLr, unit, port, &reg_val);

        soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                               EXTENDED_HIG2_ENf, ext_hi2); 
        WR_XLMAC(CTRLr, unit, port, reg_val);

        RD_XLMAC(RX_CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, XLMAC_RUNT_THRESHOLD_HIGIG2);
        WR_XLMAC(RX_CTRLr, unit, port, reg_val);
    } else if (encap == SOC_ENCAP_HIGIG) {
        RD_XLMAC(RX_CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, XLMAC_RUNT_THRESHOLD_HIGIG);
        WR_XLMAC(RX_CTRLr, unit, port, reg_val);
    } else {
        RD_XLMAC(RX_CTRLr, unit, port, &reg_val);
        soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &reg_val, RUNT_THRESHOLDf, XLMAC_RUNT_THRESHOLD_ETH);
        WR_XLMAC(RX_CTRLr, unit, port, reg_val);
    }

    if (enable) {
        if (flags & XLMAC_ENCAP_SET_FLAGS_SOFT_RESET_DIS) {
            enable_flags |= XLMAC_ENABLE_SET_FLAGS_SOFT_RESET_DIS;
        }
        SOC_IF_ERROR_RETURN(xlmac_enable_set(unit, port, enable_flags, 1));
    }

    return (SOC_E_NONE);
}

int xlmac_encap_get(int unit, soc_port_t port, int *flags, portmod_encap_t *encap)
{
    uint64 reg_val;
    uint32 fld_val;

    RD_XLMAC(MODEr, unit, port, &reg_val);
    fld_val = soc_reg64_field32_get(unit, XLMAC_MODEr, reg_val, HDR_MODEf);

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
            return(SOC_E_PARAM);
            break;
    }

    if((*encap == SOC_ENCAP_HIGIG) || (*encap == SOC_ENCAP_HIGIG2)){
        fld_val = soc_reg64_field32_get(unit, XLMAC_MODEr, reg_val,
                                        NO_SOP_FOR_CRC_HGf);
        if(fld_val) {
            (*flags) |= XLMAC_ENCAP_SET_FLAGS_NO_SOP_FOR_CRC_HG;
        }
    }

    if(*encap == SOC_ENCAP_HIGIG2){
        RD_XLMAC(CTRLr, unit, port, &reg_val);
        fld_val = soc_reg64_field32_get(unit, XLMAC_CTRLr,
                                        reg_val, EXTENDED_HIG2_ENf);
        if(fld_val) {
            (*flags) |= XLMAC_ENCAP_SET_FLAGS_EXTENDED_HIGIG2_EN;
        }
    }


    return (SOC_E_NONE);
}


int xlmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    RD_XLMAC(RX_MAX_SIZEr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_RX_MAX_SIZEr, &reg_val, 
                           RX_MAX_SIZEf, value);
    WR_XLMAC(RX_MAX_SIZEr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int xlmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    RD_XLMAC(RX_MAX_SIZEr, unit, port, &reg_val);
    *value = (int)soc_reg64_field32_get (unit, XLMAC_RX_MAX_SIZEr, 
                                         reg_val, RX_MAX_SIZEf);
    return (SOC_E_NONE);
}

int xlmac_pad_size_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    RD_XLMAC(TX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val, PAD_ENf, value ? 1 : 0);
    if(value){
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val, PAD_THRESHOLDf, value);
    }
    WR_XLMAC(TX_CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int xlmac_pad_size_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;
    int pad_en;

    RD_XLMAC(TX_CTRLr, unit, port, &reg_val);
    pad_en = soc_reg64_field32_get(unit, XLMAC_TX_CTRLr, reg_val, PAD_ENf);
    if(!pad_en){
        *value = 0;
    }
    else{
        *value = soc_reg64_field32_get(unit, XLMAC_TX_CTRLr, reg_val, PAD_THRESHOLDf);
    }

    return (SOC_E_NONE);
}

int xlmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    if ((value < XLMAC_RUNT_THRESHOLD_MIN) ||
        (value > XLMAC_RUNT_THRESHOLD_MAX)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                     "runt threshold should be the range of %d - %d. got %d"),
                     XLMAC_RUNT_THRESHOLD_MIN, XLMAC_RUNT_THRESHOLD_MAX, value));
        return (SOC_E_PARAM);
    }

    RD_XLMAC(RX_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &reg_val, 
                           RUNT_THRESHOLDf, value);

    WR_XLMAC(RX_CTRLr, unit, port, reg_val);
    return (SOC_E_NONE);
}

int xlmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    RD_XLMAC(RX_CTRLr, unit, port, &reg_val);
    *value = (int)soc_reg64_field32_get (unit, XLMAC_RX_CTRLr, 
                                         reg_val, RUNT_THRESHOLDf);
    return (SOC_E_NONE);
}

int xlmac_strict_preamble_set(int unit, soc_port_t port, int value)
{
    uint64 reg_val;

    RD_XLMAC(RX_CTRLr, unit, port, &reg_val);
    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &reg_val, 
                           STRICT_PREAMBLEf, value);
    WR_XLMAC(RX_CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int xlmac_strict_preamble_get(int unit, soc_port_t port, int *value)
{
    uint64 reg_val;

    RD_XLMAC(RX_CTRLr, unit, port, &reg_val);
    *value = (int)soc_reg64_field32_get (unit, XLMAC_RX_CTRLr,
                                         reg_val, STRICT_PREAMBLEf);
    return (SOC_E_NONE);
}

int xlmac_rx_vlan_tag_set (int unit, soc_port_t port, 
                           int outer_vlan_tag, int inner_vlan_tag)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(RX_VLAN_TAGr, unit, port, &reg_val);

    if(inner_vlan_tag == -1) {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               INNER_VLAN_TAG_ENABLEf, 0);
    } else {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               INNER_VLAN_TAGf, inner_vlan_tag);
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               INNER_VLAN_TAG_ENABLEf, 1);
    }

    if(outer_vlan_tag == -1) {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               OUTER_VLAN_TAG_ENABLEf, 0);
    } else {
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               OUTER_VLAN_TAGf, outer_vlan_tag);
        soc_reg64_field32_set (unit, XLMAC_RX_VLAN_TAGr, &reg_val, 
                               OUTER_VLAN_TAG_ENABLEf, 1);
    }

    WR_XLMAC(RX_VLAN_TAGr, unit, port, reg_val);
    return (SOC_E_NONE);
}


int xlmac_rx_vlan_tag_get (int unit, soc_port_t port, 
                           int *outer_vlan_tag, int *inner_vlan_tag)
{
    uint64 reg_val;
    uint32 enable = 0;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(RX_VLAN_TAGr, unit, port, &reg_val);

    enable = soc_reg64_field32_get (unit, XLMAC_RX_VLAN_TAGr, reg_val, 
                                    INNER_VLAN_TAG_ENABLEf);

    *inner_vlan_tag = enable ? 
                         soc_reg64_field32_get (unit, XLMAC_RX_VLAN_TAGr, 
                                                reg_val, INNER_VLAN_TAGf) : -1;

    enable = soc_reg64_field32_get (unit, XLMAC_RX_VLAN_TAGr, reg_val, 
                                    OUTER_VLAN_TAG_ENABLEf);

    *outer_vlan_tag = enable ? soc_reg64_field32_get(unit, XLMAC_RX_VLAN_TAGr, 
                                    reg_val, OUTER_VLAN_TAGf) : -1;
    return (SOC_E_NONE);
}


int xlmac_sw_link_status_select_set(int unit, soc_port_t port, int enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_CTRLr, &reg_val, 
                           LINK_STATUS_SELECTf, enable);

    WR_XLMAC(CTRLr, unit, port, reg_val);
    return (SOC_E_NONE);
}

int xlmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    *enable = (int)soc_reg64_field32_get (unit, XLMAC_CTRLr, reg_val, 
                                          LINK_STATUS_SELECTf);
    return (SOC_E_NONE);
}

int xlmac_sw_link_status_set(int unit, soc_port_t port, int link)
{
    uint64 reg_val;

    RD_XLMAC(CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &reg_val, SW_LINK_STATUSf, link);
    WR_XLMAC(CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int xlmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;

    COMPILER_64_ZERO(mac_addr);

    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[0]), _shr_uint32_read(&mac[2]));

    WR_XLMAC(RX_MAC_SAr, unit, port, mac_addr);

    return (SOC_E_NONE);
}

int xlmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 values[2];

    RD_XLMAC(RX_MAC_SAr, unit, port, &rval64);
    field64 = soc_reg64_field_get(unit, XLMAC_RX_MAC_SAr, rval64, RX_SAf);

    values[0] = COMPILER_64_HI(field64);
    values[1] = COMPILER_64_LO(field64);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

    return SOC_E_NONE;
}

int xlmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 mac_addr;

    COMPILER_64_ZERO(mac_addr);
    COMPILER_64_SET(mac_addr, _shr_uint16_read(&mac[0]), _shr_uint32_read(&mac[2]));

    WR_XLMAC(TX_MAC_SAr, unit, port, mac_addr);

    return (SOC_E_NONE);
}


int xlmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    uint32 values[2];

    RD_XLMAC(TX_MAC_SAr, unit, port, &rval64);
    values[0] = soc_reg64_field32_get(unit, XLMAC_TX_MAC_SAr, rval64, SA_HIf);
    values[1] = soc_reg64_field32_get(unit, XLMAC_TX_MAC_SAr, rval64, SA_LOf);

    mac[0] = (values[0] & 0x0000ff00) >> 8;
    mac[1] = values[0] & 0x000000ff;
    mac[2] = (values[1] & 0xff000000) >> 24;
    mac[3] = (values[1] & 0x00ff0000) >> 16;
    mac[4] = (values[1] & 0x0000ff00) >> 8;
    mac[5] = values[1] & 0x000000ff;

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "xlmac_tx_mac_sa_get: unit %d port %s MAC=<"
                            "%02x:%02x:%02x:%02x:%02x:%02x>\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    return SOC_E_NONE;
}

int xlmac_tx_average_ipg_set(int unit, soc_port_t port, int val)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(TX_CTRLr, unit, port, &reg_val);

    /* Silently adjust the specified ifp bits to valid value */
    /* valid value: 8 to 64 bytes (i.e. multiple of 8 bits) */
    val = val < 64 ? 64 : (val > 512 ? 512 : (val + 7) & (0x7f << 3));

    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &reg_val, AVERAGE_IPGf, val/8);

    WR_XLMAC(TX_CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int xlmac_tx_average_ipg_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(TX_CTRLr, unit, port, &reg_val);

    *val = soc_reg64_field32_get(unit, XLMAC_TX_CTRLr, reg_val, AVERAGE_IPGf) * 8;
   
    return (SOC_E_NONE);
}

int xlmac_tx_preamble_length_set(int unit, soc_port_t port, int length)
{
    uint64 reg_val;

    if(length > 8){
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, 
                       "runt size should be small than 8. got %d"), length));
        return (SOC_E_PARAM);
    }

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(TX_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_TX_CTRLr, &reg_val, 
                           TX_PREAMBLE_LENGTHf, length);
    WR_XLMAC(TX_CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}

/***************************************************************************** 
 * Remote/local Fault                                                        *
 */

int xlmac_remote_fault_control_set (int unit, soc_port_t port, 
                    const portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
 
    RD_XLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
        REMOTE_FAULT_DISABLEf, control->enable ? 0 : 1); 
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
        DROP_TX_DATA_ON_REMOTE_FAULTf, control->drop_tx_on_fault ? 1: 0);

    WR_XLMAC(RX_LSS_CTRLr, unit, port, reg_val);

    return (SOC_E_NONE);
}


int xlmac_remote_fault_control_get(int unit, soc_port_t port, 
                     portmod_remote_fault_control_t *control)
{
    uint64 reg_val;
    
    RD_XLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    control->enable = soc_reg64_field32_get (unit, XLMAC_RX_LSS_CTRLr, 
                                  reg_val, REMOTE_FAULT_DISABLEf)? 0 : 1;
    control->drop_tx_on_fault = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr,
                                  reg_val, DROP_TX_DATA_ON_REMOTE_FAULTf)?1:0;
    return (SOC_E_NONE);
}


int xlmac_local_fault_control_set (int unit, soc_port_t port, 
                 const portmod_local_fault_control_t *control)
{
    uint64 reg_val;
 
    RD_XLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
            LOCAL_FAULT_DISABLEf, control->enable ? 0 : 1);    

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &reg_val, 
            DROP_TX_DATA_ON_LOCAL_FAULTf, control->drop_tx_on_fault ? 1: 0); 

    WR_XLMAC(RX_LSS_CTRLr, unit, port, reg_val);

    return   (SOC_E_NONE);
}


int xlmac_local_fault_control_get(int unit, soc_port_t port, 
                      portmod_local_fault_control_t *control)
{
    uint64 reg_val;

    RD_XLMAC(RX_LSS_CTRLr, unit, port, &reg_val);
    
    control->enable = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr, 
                                  reg_val, LOCAL_FAULT_DISABLEf)? 0 : 1;
    control->drop_tx_on_fault  = soc_reg64_field32_get(unit, XLMAC_RX_LSS_CTRLr,
                                  reg_val, DROP_TX_DATA_ON_LOCAL_FAULTf)?1:0;
    return (SOC_E_NONE);
}


int xlmac_remote_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint64 reg_val;

    RD_XLMAC(RX_LSS_STATUSr, unit, port, &reg_val);
    *status = soc_reg64_field32_get (unit, XLMAC_RX_LSS_STATUSr, reg_val, 
                                     REMOTE_FAULT_STATUSf)? 1 : 0;
    return (SOC_E_NONE);
}

int xlmac_local_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint64 reg_val;

    RD_XLMAC(RX_LSS_STATUSr, unit, port, &reg_val);

    *status = soc_reg64_field32_get (unit, XLMAC_RX_LSS_STATUSr, reg_val, 
                                     LOCAL_FAULT_STATUSf)?1:0;

    return (SOC_E_NONE);
}

int xlmac_clear_rx_lss_status_set(int unit, soc_port_t port, int lcl_fault, int rmt_fault)
{
    uint64 reg_val;

    RD_XLMAC(CLEAR_RX_LSS_STATUSr, unit, port, &reg_val);
    soc_reg64_field32_set(unit, XLMAC_CLEAR_RX_LSS_STATUSr, &reg_val, 
                                     CLEAR_REMOTE_FAULT_STATUSf, rmt_fault);
    soc_reg64_field32_set(unit, XLMAC_CLEAR_RX_LSS_STATUSr, &reg_val, 
                                     CLEAR_LOCAL_FAULT_STATUSf, lcl_fault);
    WR_XLMAC(CLEAR_RX_LSS_STATUSr, unit, port, reg_val);

    return (SOC_E_NONE);
}

int xlmac_clear_rx_lss_status_get(int unit, soc_port_t port, int *lcl_fault, int *rmt_fault)
{
    uint64 reg_val;

    RD_XLMAC(CLEAR_RX_LSS_STATUSr, unit, port, &reg_val);
    *rmt_fault = soc_reg64_field32_get(unit, XLMAC_CLEAR_RX_LSS_STATUSr, reg_val, 
                                     CLEAR_REMOTE_FAULT_STATUSf);
    *lcl_fault = soc_reg64_field32_get(unit, XLMAC_CLEAR_RX_LSS_STATUSr, reg_val, 
                                     CLEAR_LOCAL_FAULT_STATUSf);

    return (SOC_E_NONE);
}

/**************************************************************************** 
 Flow Control
 */

int xlmac_pause_control_set (int unit, soc_port_t port, 
                             const portmod_pause_control_t *control)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(PAUSE_CTRLr, unit, port , &reg_val);

    if(control->rx_enable || control->tx_enable){
        if(control->refresh_timer > 0){
            soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_REFRESH_TIMERf,control->refresh_timer);
            soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_REFRESH_ENf, 1);
        } else {
            soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_REFRESH_ENf, 0);
        }
        soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   PAUSE_XOFF_TIMERf, control->xoff_timer);
    }

    soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   TX_PAUSE_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_PAUSE_CTRLr, &reg_val, 
                                   RX_PAUSE_ENf, control->rx_enable);

    WR_XLMAC(PAUSE_CTRLr, unit, port , reg_val);
    return (SOC_E_NONE);
}


int xlmac_pfc_control_set (int unit, soc_port_t port, 
                           const portmod_pfc_control_t *control)
{
    uint64 reg_val;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(PFC_CTRLr, unit, port , &reg_val);

    if(control->refresh_timer){
        soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                               PFC_REFRESH_TIMERf, control->refresh_timer);
        soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                               PFC_REFRESH_ENf, 1);
    } else {
        soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                               PFC_REFRESH_ENf, 0);
    }

    soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                           PFC_STATS_ENf, control->stats_en);
    soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                           PFC_XOFF_TIMERf, control->xoff_timer);
    soc_reg64_field32_set (unit, XLMAC_PFC_CTRLr, &reg_val, 
                           FORCE_PFC_XONf, control->force_xon);

    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, 
                                TX_PFC_ENf, control->tx_enable);
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &reg_val, 
                                RX_PFC_ENf, control->rx_enable);

    WR_XLMAC(PFC_CTRLr, unit, port , reg_val);
    return (SOC_E_NONE);
}


int xlmac_llfc_control_set (int unit, soc_port_t port, 
                            const portmod_llfc_control_t *control)
{
    uint64 reg_val;

    RD_XLMAC(LLFC_CTRLr, unit, port , &reg_val);

    if(control->rx_enable || control->tx_enable){
        soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                               LLFC_IN_IPG_ONLYf, control->in_ipg_only);
        soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                               LLFC_CRC_IGNOREf, control->crc_ignore);
    }

    soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                           TX_LLFC_ENf, control->tx_enable);
    soc_reg64_field32_set (unit, XLMAC_LLFC_CTRLr, &reg_val, 
                           RX_LLFC_ENf, control->rx_enable);

    WR_XLMAC(LLFC_CTRLr, unit, port , reg_val);

    return (SOC_E_NONE);
}


int xlmac_pause_control_get (int unit, soc_port_t port, 
                             portmod_pause_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    COMPILER_64_ZERO(reg_val);
    RD_XLMAC(PAUSE_CTRLr, unit, port , &reg_val);

    refresh_enable = soc_reg64_field32_get  (unit, XLMAC_PAUSE_CTRLr, 
                                             reg_val, PAUSE_REFRESH_ENf);
    refresh_timer  =  soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                             reg_val, PAUSE_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable) ? refresh_timer : -1;

    control->xoff_timer = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                 reg_val, PAUSE_XOFF_TIMERf);
    control->rx_enable  = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                 reg_val, RX_PAUSE_ENf);
    control->tx_enable  = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                 reg_val, TX_PAUSE_ENf);
    return (SOC_E_NONE);
}


int xlmac_pfc_control_get (int unit, soc_port_t port,
                           portmod_pfc_control_t *control)
{
    uint64 reg_val;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;

    COMPILER_64_ZERO(reg_val);

    RD_XLMAC(PFC_CTRLr, unit, port , &reg_val);

    refresh_enable = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                            reg_val, PFC_REFRESH_ENf);
    if (refresh_enable) {
        refresh_timer  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                reg_val, PFC_REFRESH_TIMERf);
    }
    control->refresh_timer = refresh_timer;

    control->stats_en   = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, PFC_STATS_ENf);
    control->xoff_timer = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, PFC_XOFF_TIMERf);
    control->force_xon  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, FORCE_PFC_XONf);
    control->rx_enable  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, RX_PFC_ENf);
    control->tx_enable  = soc_reg64_field32_get (unit, XLMAC_PFC_CTRLr, 
                                                 reg_val, TX_PFC_ENf);
    return (SOC_E_NONE);
}


int xlmac_llfc_control_get (int unit, soc_port_t port, 
                            portmod_llfc_control_t *control)
{
    uint64 reg_val;

    RD_XLMAC(LLFC_CTRLr, unit, port , &reg_val);

    control->in_ipg_only = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, LLFC_IN_IPG_ONLYf);
    control->crc_ignore  = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, LLFC_CRC_IGNOREf);
    control->rx_enable   = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, RX_LLFC_ENf);
    control->tx_enable   = soc_reg64_field32_get (unit, XLMAC_LLFC_CTRLr, 
                                                  reg_val, TX_LLFC_ENf);
    return (SOC_E_NONE);
}

/*
 * Function:
 *      xlmac_duplex_set
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
int
xlmac_duplex_set(int unit, soc_port_t port, int duplex)
{
    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_duplex_set: unit %d port %s duplex=%s\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 duplex ? "Full" : "Half"));
    return (duplex) ? (SOC_E_NONE) : (SOC_E_UNAVAIL) ;
}


/*
 * Function:
 *      xlmac_duplex_get
 * Purpose:
 *      Get XLMAC duplex mode.
 * Parameters:
 *      unit - XGS unit #.
 *      duplex - (OUT) Boolean: true --> full duplex, false --> half duplex.
 * Returns:
 *      SOC_E_XXX
 */
int
xlmac_duplex_get(int unit, soc_port_t port, int *duplex)
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

int
xlmac_speed_get(int unit, soc_port_t port, int *speed)
{
    uint64 rval;

    RD_XLMAC(MODEr, unit, port, &rval);
    switch (soc_reg64_field32_get(unit, XLMAC_MODEr, rval, SPEED_MODEf)) {
    case XLMAC_SPEED_10:
        *speed = 10;
        break;
    case XLMAC_SPEED_100:
        *speed = 100;
        break;
    case XLMAC_SPEED_1000:
        *speed = 1000;
        break;
    case XLMAC_SPEED_2500:
        *speed = 2500;
        break;
    case XLMAC_SPEED_10000:
    default:
        *speed = 10000;
        break;
    }

    LOG_VERBOSE(BSL_LS_SOC_10G,
                (BSL_META_U(unit,
                            "mac_xl_speed_get: unit %d port %s speed=%dMb\n"),
                 unit, SOC_PORT_NAME(unit, port),
                 *speed));
    return SOC_E_NONE;
}


/**
 * 
 * @brief init xlmac with default configuration
 * @param unit 
 * @param instance 
 * 
 * @return int 
 */
int xlmac_init(int unit, soc_port_t port, uint32_t init_flags)
{
    uint64 mac_ctrl, tx_ctrl, rx_ctrl, rval;
    int    is_strip_crc, is_append_crc, is_replace_crc, is_higig, is_pass_through_crc;
    uint32 crc_mode, is_ipg_check_disable;
    portmod_pause_control_t control;

    is_strip_crc         = init_flags & XLMAC_INIT_F_RX_STRIP_CRC               ? 1 : 0;
    is_append_crc        = init_flags & XLMAC_INIT_F_TX_APPEND_CRC              ? 1 : 0;
    is_replace_crc       = init_flags & XLMAC_INIT_F_TX_REPLACE_CRC             ? 1 : 0;
    is_pass_through_crc  = init_flags & XLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE   ? 1 : 0;
    is_higig             = init_flags & XLMAC_INIT_F_IS_HIGIG                   ? 1 : 0;
    is_ipg_check_disable = init_flags & XLMAC_INIT_F_IPG_CHECK_DISABLE          ? 1 : 0;

    if(is_append_crc + is_replace_crc + is_pass_through_crc > 1) {
        LOG_VERBOSE(BSL_LS_SOC_10G,
                    (BSL_META_U(unit, "XLAMC_INIT_F_TX_APPEND_CRC, XLAMC_INIT_F_TX_REPLACE_CRC and XLMAC_INIT_F_TX_PASS_THROUGH_CRC_MODE can't co-exist")));
        return (SOC_E_PARAM);
    }

    /* Disable Tx/Rx, assume that MAC is stable (or out of reset) */
    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, 
                                XGMII_IPG_CHECK_DISABLEf,
                                is_ipg_check_disable);
    WR_XLMAC(CTRLr, unit, port, mac_ctrl);

    /* XLMAC_RX_CTRL */
    RD_XLMAC(RX_CTRLr, unit, port, &rx_ctrl);

    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rx_ctrl, STRIP_CRCf, 
                           is_strip_crc);
    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rx_ctrl, STRICT_PREAMBLEf, 
                           is_higig ? 0 : 1);
    soc_reg64_field32_set (unit, XLMAC_RX_CTRLr, &rx_ctrl, RUNT_THRESHOLDf, XLMAC_RUNT_THRESHOLD_ETH);

    WR_XLMAC(RX_CTRLr, unit, port, rx_ctrl);

    /* XLMAC_TX_CTRL */
    RD_XLMAC(TX_CTRLr, unit, port, &tx_ctrl);

    if(is_append_crc) {
        crc_mode = 0;
    } else if(is_replace_crc) {
        crc_mode = 2;
    } else if(is_pass_through_crc) {
        crc_mode = 1;
    } else { /* CRC AUTO Mode */
        crc_mode = 3;
    }
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, CRC_MODEf, crc_mode);
    soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &tx_ctrl, AVERAGE_IPGf, 
               is_higig ? XLMAC_AVERAGE_IPG_HIGIG : XLMAC_AVERAGE_IPG_DEFAULT);

    WR_XLMAC(TX_CTRLr, unit, port, tx_ctrl);

    /* PAUSE */
    if (IS_ST_PORT(unit, port)) {
        control.tx_enable = FALSE;
        control.rx_enable = FALSE;
    } else {
        control.tx_enable = TRUE;
        control.rx_enable = TRUE;
    }

    /* Use the existing timer values for now */
    RD_XLMAC(PAUSE_CTRLr, unit, port , &rval);

    control.refresh_timer = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                rval, PAUSE_REFRESH_TIMERf);
    control.xoff_timer = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr, 
                                                rval, PAUSE_XOFF_TIMERf);
    SOC_IF_ERROR_RETURN(xlmac_pause_control_set(unit, port, &control));

    /* PFC */
    RD_XLMAC(PFC_CTRLr, unit ,port, &rval);
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &rval, PFC_REFRESH_ENf, 1);
    WR_XLMAC(PFC_CTRLr, unit ,port, rval);

    /* Set jumbo max size (8000 byte payload) */
    SOC_IF_ERROR_RETURN(xlmac_rx_max_size_set(unit, port, JUMBO_MAXSZ));

    /* XLMAC_RX_LSS_CTRL */
    RD_XLMAC(RX_LSS_CTRLr, unit, port, &rval);

    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LOCAL_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_REMOTE_FAULTf, 1);
    soc_reg64_field32_set(unit, XLMAC_RX_LSS_CTRLr, &rval,
                          DROP_TX_DATA_ON_LINK_INTERRUPTf, 1);    
    WR_XLMAC(RX_LSS_CTRLr, unit, port, rval);

    /* Disable loopback and bring XLMAC out of reset */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LOCAL_LPBKf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, RX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, TX_ENf, 1);

    WR_XLMAC(CTRLr, unit, port, mac_ctrl);
    return (SOC_E_NONE);
}

/*! 
 * xlmac_frame_spacing_stretch_set
 *
 * @brief Port Mac Control Spacing Stretch 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         - 
 */
int xlmac_frame_spacing_stretch_set(int unit, int port, int spacing)
{
    uint64 mac_ctrl;

    if (spacing < 0 || spacing > 257) {
        return SOC_E_PARAM;
    } 

    RD_XLMAC(TX_CTRLr, unit, port, &mac_ctrl);

    if ( spacing == XLMAC_THROT_10G_TO_5G ) {
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 21);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 21);
    } else if (spacing == XLMAC_THROT_10G_TO_2P5G) {
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 21);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 63);
    } else if (spacing >= 8) {
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, spacing);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 1);
    } else {
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_DENOMf, 0);
        soc_reg64_field32_set(unit, XLMAC_TX_CTRLr, &mac_ctrl, THROT_NUMf, 0);
    }

    WR_XLMAC(TX_CTRLr, unit, port, mac_ctrl);

    return (SOC_E_NONE);
}

/*! 
 * xlmac_frame_spacing_stretch_get
 *
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  spacing         -
 */ 
int xlmac_frame_spacing_stretch_get(int unit, int port, int *spacing)
{                                               
    uint64 mac_ctrl;

    RD_XLMAC(TX_CTRLr, unit, port, &mac_ctrl);

    *spacing = soc_reg64_field32_get (unit, XLMAC_TX_CTRLr, 
                                      mac_ctrl, THROT_DENOMf);
    return (SOC_E_NONE);
}

/*! 
 * xlmac_diag_fifo_status_get
 *
 * @brief get port timestamps in fifo 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  diag_info       - 
 */
int xlmac_diag_fifo_status_get (int unit, int port, 
                                const portmod_fifo_status_t* diag_info)
{
    uint64 rval;
    portmod_fifo_status_t* pinfo =(portmod_fifo_status_t*)diag_info;

    RD_XLMAC(TX_TIMESTAMP_FIFO_STATUSr, unit, port, &rval);

    if (soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                  ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    RD_XLMAC(TX_TIMESTAMP_FIFO_DATAr, unit, port, &rval);

    pinfo->timestamps_in_fifo = soc_reg64_field32_get(unit, 
                     XLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);

    return (SOC_E_NONE);
}

/*! 
 * xlmac_pfc_config_set
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int xlmac_pfc_config_set (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 fval; 
    uint64 rval;

    RD_XLMAC(PFC_TYPEr, unit ,port, &rval);
    soc_reg64_field32_set(unit, XLMAC_PFC_TYPEr, &rval, PFC_ETH_TYPEf, pfc_cfg->type);
    WR_XLMAC(PFC_TYPEr, unit ,port, rval);

    RD_XLMAC(PFC_OPCODEr, unit ,port, &rval);
    soc_reg64_field32_set(unit, XLMAC_PFC_OPCODEr, &rval, PFC_OPCODEf, pfc_cfg->opcode);
    WR_XLMAC(PFC_OPCODEr, unit ,port, rval);

    RD_XLMAC(PFC_DAr, unit, port, &rval);
    fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
    fval &= 0x00ffffff;
    fval |= (pfc_cfg->da_oui & 0xff) << 24;
    soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);

    soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_HIf,
                          pfc_cfg->da_oui >> 8);
    WR_XLMAC(PFC_DAr, unit, port, rval);

    RD_XLMAC(PFC_DAr, unit, port, &rval);
    fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval, PFC_MACDA_LOf);
    fval &= 0xff000000;
    fval |= pfc_cfg->da_nonoui;
    soc_reg64_field32_set(unit, XLMAC_PFC_DAr, &rval, PFC_MACDA_LOf, fval);
 
    WR_XLMAC(PFC_DAr, unit, port, rval);

    return(SOC_E_NONE);
}



/*! 
 * xlmac_pfc_config_get
 *
 * @brief set pass control frames. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  pfc_cfg         - 
 */
int xlmac_pfc_config_get (int unit, int port, 
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint64 rval;
    uint32 fval;
    uint32 fval0,fval1;
    portmod_pfc_config_t* pcfg = (portmod_pfc_config_t*) pfc_cfg;

    RD_XLMAC(PFC_TYPEr, unit, port, &rval);

    pcfg->type = soc_reg64_field32_get (unit, XLMAC_PFC_TYPEr, 
                                          rval, PFC_ETH_TYPEf);

    RD_XLMAC(PFC_OPCODEr, unit, port, &rval);
    pcfg->opcode = soc_reg64_field32_get (unit, XLMAC_PFC_OPCODEr, 
                                            rval, PFC_OPCODEf);

    RD_XLMAC(PFC_DAr, unit, port, &rval);
    fval0 = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval,
                                  PFC_MACDA_LOf);
    fval1 = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval,
                                  PFC_MACDA_HIf);
    pcfg->da_oui = (fval0 >> 24) | (fval1 << 8);

    RD_XLMAC(PFC_DAr, unit, port, &rval);
    fval = soc_reg64_field32_get(unit, XLMAC_PFC_DAr, rval,
                                  PFC_MACDA_LOf);
    pcfg->da_nonoui = fval & 0xFFFFFF;

    return (SOC_E_NONE);
}


/*! 
 * xlmac_eee_set
 *
 * @brief EEE Control and Timer set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int xlmac_eee_set(int unit, int port, const portmod_eee_t* eee)
{
    uint64 rval;

    RD_XLMAC(EEE_CTRLr, unit ,port, &rval);
    soc_reg64_field32_set(unit, XLMAC_EEE_CTRLr, &rval, EEE_ENf, eee->enable);
    WR_XLMAC(EEE_CTRLr, unit ,port, rval);

    RD_XLMAC(EEE_TIMERSr, unit ,port, &rval);
    soc_reg64_field32_set(unit, XLMAC_EEE_TIMERSr, &rval, EEE_DELAY_ENTRY_TIMERf, eee->tx_idle_time);
    soc_reg64_field32_set(unit, XLMAC_EEE_TIMERSr, &rval, EEE_WAKE_TIMERf, eee->tx_wake_time);
    WR_XLMAC(EEE_TIMERSr, unit ,port, rval);

    return SOC_E_NONE;
}


/*!
 * xlmac_eee_get
 *
 * @brief EEE Control and Timer get
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  eee             - 
 */
int xlmac_eee_get(int unit, int port, const portmod_eee_t* eee)
{
    uint64 rval;
    portmod_eee_t* pEEE = (portmod_eee_t*)eee;

    RD_XLMAC(EEE_CTRLr, unit, port, &rval);
    pEEE->enable = soc_reg64_field32_get(unit, XLMAC_EEE_CTRLr, rval, EEE_ENf);

    RD_XLMAC(EEE_TIMERSr, unit, port, &rval);

    pEEE->tx_idle_time = soc_reg64_field32_get (unit, XLMAC_EEE_TIMERSr, rval, 
                                               EEE_DELAY_ENTRY_TIMERf);
    pEEE->tx_wake_time = soc_reg64_field32_get (unit, XLMAC_EEE_TIMERSr, rval, 
                                               EEE_WAKE_TIMERf);
    return(SOC_E_NONE);
}

/*!
 * xlmac_eee_ref_count_set
 *
 * @brief EEE ref count set
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  value             -
 */
int xlmac_eee_ref_count_set(int unit, int port, uint32 value)
{
    uint64 rval;

    if (soc_feature(unit, soc_feature_eee)) {
        RD_XLMAC(EEE_TIMERSr, unit ,port, &rval);
        soc_reg64_field32_set(unit, XLMAC_EEE_TIMERSr, &rval, EEE_REF_COUNTf, value);
        WR_XLMAC(EEE_TIMERSr, unit ,port, rval);
    }

    return SOC_E_NONE;
}

/*!
 * xlmac_eee_ref_count_get
 *
 * @brief EEE ref count get
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  value             -
 */
int xlmac_eee_ref_count_get(int unit, int port, uint32 *value)
{
    uint64 rval;

    if (soc_feature(unit, soc_feature_eee)) {
        RD_XLMAC(EEE_TIMERSr, unit ,port, &rval);
        *value = soc_reg64_field32_get (unit, XLMAC_EEE_TIMERSr, rval, EEE_REF_COUNTf);
    }

    return SOC_E_NONE;
}


int xlmac_pass_control_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    RD_XLMAC(RX_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval, RX_PASS_CTRLf,
                              value ? 1 : 0);
    WR_XLMAC(RX_CTRLr, unit, port, rval);

    return(SOC_E_NONE);
}

int xlmac_pass_control_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    RD_XLMAC(RX_CTRLr, unit, port, &rval);
    *value = soc_reg64_field32_get(unit, XLMAC_RX_CTRLr, rval, RX_PASS_CTRLf);

    return(SOC_E_NONE);
}

/*!
 * xlmac_pass_pfc_frame_set
 *
 * @brief send PFC frame to system side
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  value           - 1 or 0
 */
int xlmac_pass_pfc_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    RD_XLMAC(RX_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval, RX_PASS_PFCf,
                              value ? 1 : 0);
    WR_XLMAC(RX_CTRLr, unit, port, rval);

    return(SOC_E_NONE);
}

int xlmac_pass_pfc_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    RD_XLMAC(RX_CTRLr, unit, port, &rval);
    *value = soc_reg64_field32_get(unit, XLMAC_RX_CTRLr, rval, RX_PASS_PFCf);

    return(SOC_E_NONE);
}

/*!
 * xlmac_pass_pause_frame_set
 *
 * @brief send pause frame to system side
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  value           - 1 or 0
 */
int xlmac_pass_pause_frame_set(int unit, int port, int value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    RD_XLMAC(RX_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_RX_CTRLr, &rval, RX_PASS_PAUSEf,
                              value ? 1 : 0);
    WR_XLMAC(RX_CTRLr, unit, port, rval);

    return(SOC_E_NONE);
}

int xlmac_pass_pause_frame_get(int unit, int port, int *value)
{
    int32 version;
    uint64 rval;

    SOC_IF_ERROR_RETURN(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    RD_XLMAC(RX_CTRLr, unit, port, &rval);
    *value = soc_reg64_field32_get(unit, XLMAC_RX_CTRLr, rval, RX_PASS_PAUSEf);

    return(SOC_E_NONE);
}

int xlmac_lag_failover_loopback_set(int unit, int port, int val)
{
    uint64 rval;

    /* Toggle link bit to notify IPIPE on link up */
    RD_XLMAC(LAG_FAILOVER_STATUSr, unit, port, &rval);

    soc_reg64_field32_set (unit, XLMAC_LAG_FAILOVER_STATUSr, &rval, 
                                            LAG_FAILOVER_LOOPBACKf, val);
    WR_XLMAC(LAG_FAILOVER_STATUSr, unit, port, rval);

    return(SOC_E_NONE);
}

int xlmac_lag_failover_loopback_get(int unit, int port, int *val)
{
    uint64 rval;

    /* Toggle link bit to notify IPIPE on link up */
    RD_XLMAC(LAG_FAILOVER_STATUSr, unit, port, &rval);

    *val = soc_reg64_field32_get (unit, XLMAC_LAG_FAILOVER_STATUSr, rval, 
                                                  LAG_FAILOVER_LOOPBACKf);

    return (SOC_E_NONE);
}

/*!
 * xlmac_lag_failover_disable
 *
 * @brief Port Mac Control Lag Failover Disable. 
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 */
int xlmac_lag_failover_disable(int unit, int port)
{
    uint64 mac_ctrl;

    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LAG_FAILOVER_ENf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LINK_STATUS_SELECTf, 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, REMOVE_FAILOVER_LPBKf, 0);

    WR_XLMAC(CTRLr, unit, port, mac_ctrl);
    return (SOC_E_NONE);
}   

        
/*!
 * xlmac_lag_failover_en_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int xlmac_lag_failover_en_set(int unit, int port, int val)
{
    uint64 mac_ctrl;

    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, LAG_FAILOVER_ENf, val);

    WR_XLMAC(CTRLr, unit, port, mac_ctrl);

    return (SOC_E_NONE);
}   
        
/*!     
 * xlmac_lag_failover_en_get
 *      
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  val         -
 */
int xlmac_lag_failover_en_get(int unit, int port, int *val)
{                                 
    uint64 mac_ctrl;
 
    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);
 
    *val = soc_reg64_field32_get (unit, XLMAC_CTRLr,
                                      mac_ctrl, LAG_FAILOVER_ENf);
    return (SOC_E_NONE);
}


/*!
 * xlmac_lag_remove_failover_lpbk_set
 *
 * @brief Port Mac Control Spacing Stretch
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  spacing         -
 */
int xlmac_lag_remove_failover_lpbk_set(int unit, int port, int val)
{
    uint64 mac_ctrl;

    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);

    soc_reg64_field32_set(unit, XLMAC_CTRLr, &mac_ctrl, REMOVE_FAILOVER_LPBKf, val);

    WR_XLMAC(CTRLr, unit, port, mac_ctrl);

    return (SOC_E_NONE);
}   
        
/*!     
 * xlmac_lag_remove_failover_lpbk_get
 *      
 * @brief Port Mac Control Spacing Stretch 
 *  
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [out]  val         -
 */
int xlmac_lag_remove_failover_lpbk_get(int unit, int port, int *val)
{                                 
    uint64 mac_ctrl;
 
    RD_XLMAC(CTRLr, unit, port, &mac_ctrl);
 
    *val = soc_reg64_field32_get (unit, XLMAC_CTRLr,
                                      mac_ctrl, REMOVE_FAILOVER_LPBKf);
    return (SOC_E_NONE);
}

int xlmac_reset_fc_timers_on_link_dn_set (int unit, soc_port_t port, int val)
{
    uint64 reg_val;

    RD_XLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    soc_reg64_field32_set (unit, XLMAC_RX_LSS_CTRLr, &reg_val,
                           RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf, val);

    WR_XLMAC(RX_LSS_CTRLr, unit, port, reg_val);
    return (SOC_E_NONE);
}


int xlmac_reset_fc_timers_on_link_dn_get(int unit, soc_port_t port, int *val)
{
    uint64 reg_val;
   
    RD_XLMAC(RX_LSS_CTRLr, unit, port, &reg_val);

    *val = soc_reg64_field32_get (unit, XLMAC_RX_LSS_CTRLr, reg_val,
                           RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf);
    return (SOC_E_NONE);
}


int xlmac_drain_cell_get (int unit, int port, portmod_drain_cells_t *drain_cells)
{   
    uint64 rval;

    RD_XLMAC(PFC_CTRLr, unit, port, &rval);
    drain_cells->rx_pfc_en = soc_reg64_field32_get(unit, XLMAC_PFC_CTRLr, 
                                   rval, RX_PFC_ENf);

    RD_XLMAC(LLFC_CTRLr, unit, port, &rval);
    drain_cells->llfc_en = soc_reg64_field32_get(unit, XLMAC_LLFC_CTRLr, 
                                 rval, RX_LLFC_ENf);

    RD_XLMAC(PAUSE_CTRLr, unit, port , &rval);
    drain_cells->rx_pause  = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr,
                                                 rval, RX_PAUSE_ENf);
    drain_cells->tx_pause  = soc_reg64_field32_get (unit, XLMAC_PAUSE_CTRLr,
                                                 rval, TX_PAUSE_ENf);

    return (0);
}    

int xlmac_drain_cell_stop (int unit, int port, const portmod_drain_cells_t *drain_cells)
{   
    uint64 rval;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(xlmac_discard_set (unit, port, 0));

    /* set pause fields */ 
    RD_XLMAC(PAUSE_CTRLr, unit, port , &rval);
    soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr,
                           &rval, RX_PAUSE_ENf, drain_cells->rx_pause);
    soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr,
                           &rval, TX_PAUSE_ENf, drain_cells->tx_pause);
    WR_XLMAC(PAUSE_CTRLr, unit, port , rval);

    /* set pfc rx_en fields */ 
    RD_XLMAC(PFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, 
                          &rval, RX_PFC_ENf, drain_cells->rx_pfc_en);
    WR_XLMAC(PFC_CTRLr, unit, port, rval);

    /* set llfc rx_en fields */ 
    RD_XLMAC(LLFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_LLFC_CTRLr, 
                          &rval, RX_LLFC_ENf, drain_cells->llfc_en);
    WR_XLMAC(LLFC_CTRLr, unit, port, rval);

    return (0);
}

int xlmac_drain_cell_start(int unit, int port)
{   
    uint64 rval;

    RD_XLMAC(CTRLr, unit, port, &rval);

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval, TX_ENf, 1);
    /* Disable RX */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval, RX_ENf, 0);

    WR_XLMAC(CTRLr, unit, port, rval);

    RD_XLMAC(PAUSE_CTRLr, unit, port , &rval);
    soc_reg64_field32_set (unit, XLMAC_PAUSE_CTRLr, &rval, RX_PAUSE_ENf, 0);
    WR_XLMAC(PAUSE_CTRLr, unit, port, rval);

    RD_XLMAC(PFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_PFC_CTRLr, &rval, RX_PFC_ENf, 0);
    WR_XLMAC(PFC_CTRLr, unit, port, rval);

    RD_XLMAC(LLFC_CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_LLFC_CTRLr, &rval, RX_LLFC_ENf, 0);
    WR_XLMAC(LLFC_CTRLr, unit, port, rval);

    RD_XLMAC(CTRLr, unit, port, &rval);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &rval, SOFT_RESETf, 1);
    WR_XLMAC(CTRLr, unit, port, rval);
 
    SOC_IF_ERROR_RETURN(xlmac_discard_set(unit, port, 1)); 

    return (0);
}

int
xlmac_mac_ctrl_set(int unit, int port, uint64 mac_ctrl)
{
    WR_XLMAC(CTRLr, unit, port, mac_ctrl);
    return (0);
}


int xlmac_txfifo_cell_cnt_get (int unit, int port, uint32* fval)
{
    uint64 rval;

    RD_XLMAC(TXFIFO_CELL_CNTr, unit, port, &rval);
    *fval = soc_reg64_field32_get(unit, XLMAC_TXFIFO_CELL_CNTr, rval,
                                     CELL_CNTf);
    return (0);
}

int
xlmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl, int *rx_en)
{
    uint64 ctrl;

    RD_XLMAC(CTRLr, unit, port, &ctrl);
    *mac_ctrl = ctrl;

    *rx_en = soc_reg64_field32_get(unit, XLMAC_CTRLr, ctrl, RX_ENf);

    return (0);
}

int
xlmac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint64 ctrl;
    /* Enable both TX and RX, de-assert SOFT_RESET */
    RD_XLMAC(CTRLr, unit, port, &ctrl);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, RX_ENf, rx_en ? 1 : 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);
    WR_XLMAC(CTRLr, unit, port, ctrl);

    return (0);
}

int
xlmac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint64 ctrl, octrl;
    uint32 soft_reset = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    RD_XLMAC(CTRLr, unit, port, &ctrl);
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, TX_ENf, 1);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, RX_ENf, rx_en ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing senario, XLMAC_CTRLr.SOFT_RESETf is 
         *      used to early check to see if this port is at disabled state 
         *      already.
         */
        soft_reset = soc_reg64_field32_get(unit, 
                                           XLMAC_CTRLr, ctrl, SOFT_RESETf);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (rx_en)
        soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, SOFT_RESETf, 0);

    WR_XLMAC(CTRLr, unit, port, ctrl);

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
 *     2. To comply original designing senario, XLMAC_CTRLr.SOFT_RESETf is 
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
int xlmac_reset_check(int unit, int port, int enable, int *reset)
{
    uint64 ctrl, octrl;

    *reset = 1;

    RD_XLMAC(CTRLr, unit, port, &ctrl);
    octrl = ctrl;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, TX_ENf, enable ? 1 : 0);
    soc_reg64_field32_set(unit, XLMAC_CTRLr, &ctrl, RX_ENf, enable ? 1 : 0);

    if (COMPILER_64_EQ(ctrl, octrl)) {
        if (enable) {
            *reset = 0;
        } else {
            if (soc_reg64_field32_get(unit, XLMAC_CTRLr, ctrl, SOFT_RESETf)) {
                *reset = 0;
            }
        }
    }

    return SOC_E_NONE;
}

int xlmac_e2ecc_hdr_get (int unit, int port, uint32_t *words)
{
    uint64 rval64;

    RD_XLMAC(E2ECC_MODULE_HDR_0r, unit, port, &rval64);
    words[0] = soc_reg64_field32_get (unit, XLMAC_E2ECC_MODULE_HDR_0r, rval64, 
                                      E2ECC_MODULE_HDR_0_HIf);
    words[1] = soc_reg64_field32_get(unit, XLMAC_E2ECC_MODULE_HDR_0r, rval64, 
                                      E2ECC_MODULE_HDR_0_LOf);

    RD_XLMAC(E2ECC_MODULE_HDR_1r, unit, port, &rval64);
    words[2] = soc_reg64_field32_get (unit, XLMAC_E2ECC_MODULE_HDR_1r, rval64, 
                                      E2ECC_MODULE_HDR_1_HIf);
    words[3] = soc_reg64_field32_get(unit, XLMAC_E2ECC_MODULE_HDR_1r, rval64, 
                                      E2ECC_MODULE_HDR_1_LOf);

    RD_XLMAC(E2ECC_DATA_HDR_0r, unit, port, &rval64);
    words[4] = soc_reg64_field32_get (unit, XLMAC_E2ECC_DATA_HDR_0r, rval64, 
                                      E2ECC_DATA_HDR_0_HIf);
    words[5] = soc_reg64_field32_get(unit, XLMAC_E2ECC_DATA_HDR_0r, rval64, 
                                      E2ECC_DATA_HDR_0_LOf);

    RD_XLMAC(E2ECC_DATA_HDR_1r, unit, port, &rval64);
    words[6] = soc_reg64_field32_get (unit, XLMAC_E2ECC_DATA_HDR_1r, rval64, 
                                      E2ECC_DATA_HDR_1_HIf);
    words[7] = soc_reg64_field32_get(unit, XLMAC_E2ECC_DATA_HDR_1r, rval64, 
                                      E2ECC_DATA_HDR_1_LOf);

    return SOC_E_NONE;
}

int xlmac_e2ecc_hdr_set (int unit, int port, uint32_t *words)
{
    uint64 rval64;
    COMPILER_64_ZERO(rval64);

    soc_reg64_field32_set(unit, XLMAC_E2ECC_MODULE_HDR_0r, &rval64, 
                                      E2ECC_MODULE_HDR_0_HIf, words[0]);
    soc_reg64_field32_set(unit, XLMAC_E2ECC_MODULE_HDR_0r, &rval64, 
                                      E2ECC_MODULE_HDR_0_LOf, words[1]);
    WR_XLMAC(E2ECC_MODULE_HDR_0r, unit, port, rval64);


    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set (unit, XLMAC_E2ECC_MODULE_HDR_1r, &rval64, 
                                      E2ECC_MODULE_HDR_1_HIf, words[2]);
    soc_reg64_field32_set(unit, XLMAC_E2ECC_MODULE_HDR_1r, &rval64, 
                                      E2ECC_MODULE_HDR_1_LOf, words[3]);
    WR_XLMAC(E2ECC_MODULE_HDR_1r, unit, port, rval64);


    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set (unit, XLMAC_E2ECC_DATA_HDR_0r, &rval64, 
                                      E2ECC_DATA_HDR_0_HIf, words[4]);
    soc_reg64_field32_set(unit, XLMAC_E2ECC_DATA_HDR_0r, &rval64, 
                                      E2ECC_DATA_HDR_0_LOf, words[5]);
    WR_XLMAC(E2ECC_DATA_HDR_0r, unit, port, rval64);


    COMPILER_64_ZERO(rval64);
    soc_reg64_field32_set (unit, XLMAC_E2ECC_DATA_HDR_1r, &rval64, 
                                      E2ECC_DATA_HDR_1_HIf, words[6]);
    soc_reg64_field32_set(unit, XLMAC_E2ECC_DATA_HDR_1r, &rval64, 
                                      E2ECC_DATA_HDR_1_LOf, words[7]);
    WR_XLMAC(E2ECC_DATA_HDR_1r, unit, port, rval64);


    return SOC_E_NONE;
}


int xlmac_e2e_enable_set(int unit, int port, int enable)
{
    uint64 rval64;

    RD_XLMAC(E2E_CTRLr, unit, port, &rval64);
    soc_reg64_field32_set(unit, XLMAC_E2E_CTRLr, &rval64, E2E_ENABLEf, enable);
    WR_XLMAC(E2E_CTRLr, unit, port, rval64);

    return (0);
}

int xlmac_e2e_enable_get(int unit, int port, int *enable)
{
    uint64 rval64;

    RD_XLMAC(E2E_CTRLr, unit, port, &rval64);
    *enable = soc_reg64_field32_get(unit, XLMAC_E2E_CTRLr, rval64, E2E_ENABLEf);

    return (0);
}

int xlmac_interrupt_enable_get(int unit, int port, int intr_type, uint32 *value)
{
    uint64 reg_val;
    int32 version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_XLMAC_INTR_ENABLEr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TX_PKT_UNDERFLOWf);
                break;
        case portmodIntrTypeTxPktOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TX_LLFC_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TX_TS_FIFO_OVERFLOWf);
                break;
        case portmodIntrTypeRxPktOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_RX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeRxMsgOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_RX_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_RX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_RX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeLocalFaultStatus :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_LOCAL_FAULT_STATUSf);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_REMOTE_FAULT_STATUSf);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_LINK_INTERRUPTION_STATUSf);
                break;
        case portmodIntrTypeTsEntryValid :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_ENABLEr, reg_val, EN_TS_ENTRY_VALIDf);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
                break;
    }

exit:
    SOC_FUNC_RETURN;
}

int xlmac_interrupt_enable_set(int unit, int port, int intr_type, uint32 value)
{
    uint64 rval;
    int32 version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_XLMAC_INTR_ENABLEr(unit, port, &rval));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TX_PKT_UNDERFLOWf, value);
                break;
        case portmodIntrTypeTxPktOverflow :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TX_PKT_OVERFLOWf, value);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TX_LLFC_MSG_OVERFLOWf, value);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TX_TS_FIFO_OVERFLOWf, value);
                break;
        case portmodIntrTypeRxPktOverflow :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_RX_PKT_OVERFLOWf, value);
                break;
        case portmodIntrTypeRxMsgOverflow :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_RX_MSG_OVERFLOWf, value);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TX_CDC_SINGLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TX_CDC_DOUBLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_RX_CDC_SINGLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_RX_CDC_DOUBLE_BIT_ERRf, value);
                break;
        case portmodIntrTypeLocalFaultStatus :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_LOCAL_FAULT_STATUSf, value);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_REMOTE_FAULT_STATUSf, value);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_LINK_INTERRUPTION_STATUSf, value);
                break;
        case portmodIntrTypeTsEntryValid :
                soc_reg64_field32_set(unit, XLMAC_INTR_ENABLEr, &rval, EN_TS_ENTRY_VALIDf, value);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
                break;
     }

     _SOC_IF_ERR_EXIT(WRITE_XLMAC_INTR_ENABLEr(unit, port, rval));

exit:
     SOC_FUNC_RETURN;
}

int xlmac_interrupt_status_get(int unit, int port, int intr_type, uint32 *value)
{
    uint64 reg_val;
    int32 version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_XLMAC_INTR_STATUSr(unit, port, &reg_val));

    switch(intr_type) {
        case portmodIntrTypeTxPktUnderflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_UNDERFLOWf);
                break;
        case portmodIntrTypeTxPktOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeTxLlfcMsgOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_LLFC_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxTSFifoOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_TS_FIFO_OVERFLOWf);
                break;
        case portmodIntrTypeRxPktOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_PKT_OVERFLOWf);
                break;
        case portmodIntrTypeRxMsgOverflow :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_MSG_OVERFLOWf);
                break;
        case portmodIntrTypeTxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeTxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcSingleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_SINGLE_BIT_ERRf);
                break;
        case portmodIntrTypeRxCdcDoubleBitErr :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_DOUBLE_BIT_ERRf);
                break;
        case portmodIntrTypeLocalFaultStatus :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_LOCAL_FAULT_STATUSf);
                break;
        case portmodIntrTypeRemoteFaultStatus :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_REMOTE_FAULT_STATUSf);
                break;
        case portmodIntrTypeLinkInterruptionStatus :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_LINK_INTERRUPTION_STATUSf);
                break;
        case portmodIntrTypeTsEntryValid :
                *value = soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TS_ENTRY_VALIDf);
                break;
        default:
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Invalid interrupt type")));
                break;
    }

exit:
    SOC_FUNC_RETURN;
}

int xlmac_interrupts_status_get(int unit, int port, int arr_max_size, uint32* intr_arr, uint32* size)
{
    uint32 cnt = 0;
    uint64 reg_val;
    int32 version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_xlmac_version_get(unit, port, &version));

    if (version < XLMAC_VERSION_A041) {
        return SOC_E_UNAVAIL;
    }

    _SOC_IF_ERR_EXIT(READ_XLMAC_INTR_STATUSr(unit, port, &reg_val));

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_UNDERFLOWf)) {
        if (cnt >= arr_max_size) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktUnderflow;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_PKT_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxPktOverflow;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_LLFC_MSG_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxLlfcMsgOverflow;
    }
    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_TS_FIFO_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxTSFifoOverflow;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_PKT_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxPktOverflow;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_MSG_OVERFLOWf)) {
        if (cnt >= arr_max_size) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxMsgOverflow;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTxCdcSingleBitErr;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TX_CDC_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
            intr_arr[cnt++] = portmodIntrTypeTxCdcDoubleBitErr;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_SINGLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxCdcSingleBitErr;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_RX_CDC_DOUBLE_BIT_ERRf)) {
        if (cnt >= arr_max_size) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRxCdcDoubleBitErr;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_LOCAL_FAULT_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLocalFaultStatus;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_REMOTE_FAULT_STATUSf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeRemoteFaultStatus;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_LINK_INTERRUPTION_STATUSf)) {
        if (cnt >= arr_max_size) {
             _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeLinkInterruptionStatus;
    }

    if (soc_reg64_field32_get(unit, XLMAC_INTR_STATUSr, reg_val, SUM_TS_ENTRY_VALIDf)) {
        if (cnt >= arr_max_size) {
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, (_SOC_MSG("Insufficient Array size")));
        }
        intr_arr[cnt++] = portmodIntrTypeTsEntryValid;
    }

    *size = cnt;

exit:
    SOC_FUNC_RETURN;
}

int xlmac_preemption_mode_set(int32 unit, soc_port_t port, uint32 enable)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                  reg_val, PREEMPTION_MODEf);

    if (value != enable) {
        soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                              &reg_val, PREEMPTION_MODEf, enable? 1: 0);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int xlmac_preemption_mode_get(int32 unit, soc_port_t port, uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                    reg_val, PREEMPTION_MODEf);

    return (SOC_E_NONE);
}

int xlmac_preemption_tx_enable_set(int32 unit, soc_port_t port, uint32 enable)
{
    uint64 reg_val, v_reg_val;
    int32 value = 0, rc = SOC_E_NONE;
    uint32 is_verify_set;
    uint32 timeout = 0, num_attempts = 0;
    uint32 verify_status = 0, verify_op_done = 0;
    uint32 wait_time, iter = 0;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                  reg_val, PREEMPT_ENABLEf);

    is_verify_set = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                          reg_val, VERIFY_ENABLEf);  


    /* if preemption already enabled, return */
    if (value == enable) {
        return rc;
    }

    /* Static configuration - no preemption negotaition with link partner*/
    soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                          &reg_val, PREEMPT_ENABLEf, enable? 1: 0);
    SOC_IF_ERROR_RETURN(WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));

    if (is_verify_set) {
        /*
         * preemption negotiation enabled - wait for the negotiation
         * to complete.
         * Wait time >= (num verify attempts + 1) * (verify timeout + 1)
         */
        timeout = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                        reg_val, VERIFY_TIMEf);  
        num_attempts = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                         reg_val, VERIFY_ATTEMPT_LIMITf);  
        wait_time = (num_attempts + 1) * (timeout + 1);

        /* wait for (timeout + 1) millsecs, and check verify status */
        sal_udelay((timeout + 1) * 1000);

        /* check verification status */
        SOC_IF_ERROR_RETURN(
               READ_XLMAC_MERGE_VERIFY_STATEr(unit, port, &v_reg_val));
        verify_status = soc_reg64_field32_get(unit,
                                              XLMAC_MERGE_VERIFY_STATEr,
                                              v_reg_val,
                                              PREEMPTION_VERIFY_STATUSf);
        if ((verify_status == XLMAC_PREEMPT_VERIFY_SUCCEED) ||
            (verify_status == XLMAC_PREEMPT_VERIFY_FAILED)) {
            verify_op_done = 1;
        }

        while (!verify_op_done) { 
            sal_udelay((timeout + 1) * 1000);

            /* check verification status */
            SOC_IF_ERROR_RETURN(
                   READ_XLMAC_MERGE_VERIFY_STATEr(unit, port, &v_reg_val));
            verify_status = soc_reg64_field32_get(unit,
                                              XLMAC_MERGE_VERIFY_STATEr,
                                              v_reg_val,
                                              PREEMPTION_VERIFY_STATUSf);

            /* break if verify operation done or timeout */
            if ((verify_status == XLMAC_PREEMPT_VERIFY_SUCCEED) ||
                (verify_status == XLMAC_PREEMPT_VERIFY_FAILED)  ||
                (((timeout + 1) * iter++) > wait_time)) {
                verify_op_done = 1;
                break;
            }
        }  
    }

    if (verify_op_done) {
        rc = (verify_status == XLMAC_PREEMPT_VERIFY_SUCCEED)? SOC_E_NONE:
                                                             SOC_E_FAIL;
    }
    return (rc);
}

int xlmac_preemption_tx_enable_get(int32 unit, soc_port_t port, uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                    reg_val, PREEMPT_ENABLEf);

    return (SOC_E_NONE);
}

int xlmac_preemption_verify_enable_set(int32 unit, soc_port_t port,
                                       uint32 enable)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                  reg_val, VERIFY_ENABLEf);

    if (value != enable) {
        soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                              &reg_val, VERIFY_ENABLEf, enable? 1: 0);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int xlmac_preemption_verify_enable_get(int32 unit, soc_port_t port,
                                       uint32 *enable)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *enable = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                    reg_val, VERIFY_ENABLEf);

    return (SOC_E_NONE);
}

int xlmac_preemption_verify_time_set(int32 unit, soc_port_t port,
                                     uint32 time_val)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                  reg_val, VERIFY_TIMEf);

    if (value != time_val) {
        soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                              &reg_val, VERIFY_TIMEf, time_val);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}

int xlmac_preemption_verify_time_get(int32 unit, soc_port_t port,
                                     uint32 *time_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *time_val = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                    reg_val, VERIFY_TIMEf);

    return (SOC_E_NONE);
}

int xlmac_preemption_verify_attempts_set(int32 unit, soc_port_t port,
                                         uint32 num_attempts)
{
    uint64 reg_val;
    int32 value = 0;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                  reg_val, VERIFY_ATTEMPT_LIMITf);

    if (value != num_attempts) {
        SOC_IF_ERROR_RETURN(soc_reg_field_validate(unit, XLMAC_MERGE_CONFIGr,
                                                   VERIFY_ATTEMPT_LIMITf,
                                                   num_attempts));
        soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                              &reg_val, VERIFY_ATTEMPT_LIMITf, num_attempts);
        SOC_IF_ERROR_RETURN(WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
    }

    return (SOC_E_NONE);
}
/*!
 * xlmac_timesync_tx_info_get
 *
 * @brief get port timestamps and sequence id form fifo
 *
 * @param [in]  unit            - unit id
 * @param [in]  port            - logical port
 * @param [in]  tx _info        - timestamp and seq id form fifo
 */

int xlmac_timesync_tx_info_get (int unit, int port,
                                portmod_fifo_status_t* tx_info)
{
    uint64 rval;

    RD_XLMAC(TX_TIMESTAMP_FIFO_STATUSr, unit, port, &rval);

    if (soc_reg64_field32_get(unit, XLMAC_TX_TIMESTAMP_FIFO_STATUSr, rval,
                                  ENTRY_COUNTf) == 0) {
        return SOC_E_EMPTY;
    }

    RD_XLMAC(TX_TIMESTAMP_FIFO_DATAr, unit, port, &rval);

    tx_info->timestamps_in_fifo = soc_reg64_field32_get(unit,
                     XLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, TIME_STAMPf);
    tx_info->timestamps_in_fifo_hi = 0;
    tx_info->sequence_id = soc_reg64_field32_get(unit,
                    XLMAC_TX_TIMESTAMP_FIFO_DATAr, rval, SEQUENCE_IDf);

    return (SOC_E_NONE);
}

int xlmac_preemption_verify_attempts_get(int32 unit, soc_port_t port,
                                         uint32 *num_attempts)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    *num_attempts = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                    reg_val, VERIFY_ATTEMPT_LIMITf);

    return (SOC_E_NONE);
}

int xlmac_preemption_tx_status_get(int unit, soc_port_t port,
                                   int flags, uint32 *value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_VERIFY_STATEr(unit, port, &reg_val));
    
    switch (flags) {
        case XLMAC_PREEMPTION_TX_STATUS:
            *value = soc_reg64_field32_get(unit, XLMAC_MERGE_VERIFY_STATEr,
                                           reg_val, PREEMPTION_TX_STATUSf); 
            break;
        case XLMAC_PREEMPTION_VERIFY_STATUS:
            *value = soc_reg64_field32_get(unit, XLMAC_MERGE_VERIFY_STATEr,
                                           reg_val, PREEMPTION_VERIFY_STATUSf); 
            break;
        case XLMAC_PREEMPTION_VERIFY_CNT_STATUS:
            *value = soc_reg64_field32_get(unit, XLMAC_MERGE_VERIFY_STATEr,
                                           reg_val, TX_VERIFY_CNT_STATUSf); 
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}
                                    
int xlmac_preemption_force_rx_lost_eop_set(int unit, soc_port_t port,
                                           int flags, uint32 value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    
    switch (flags) {
        case XLMAC_PREEMPTION_FORCE_SW_RX_EOP:
            /*
             * for forcing sw based rx lost eop, the sequence is to
             * write a zero to the field SW_FORCE_RX_LOST_EOPf wait
             * for a delay 10usecs, write a 1 to the field 
             * SW_FORCE_RX_LOST_EOP
             */
            if (value) {
                soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                                      &reg_val, SW_FORCE_RX_LOST_EOPf, 0);
                SOC_IF_ERROR_RETURN(
                        WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
                sal_udelay(10);
            }
            soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                                  &reg_val, SW_FORCE_RX_LOST_EOPf, value);
            SOC_IF_ERROR_RETURN(
                    WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
            break;
        case XLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP:
            soc_reg64_field32_set(unit, XLMAC_MERGE_CONFIGr,
                              &reg_val, AUTO_HW_FORCE_RX_LOST_EOPf, value);
            SOC_IF_ERROR_RETURN(
                    WRITE_XLMAC_MERGE_CONFIGr(unit, port, reg_val));
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}
                                    
int xlmac_preemption_force_rx_lost_eop_get(int unit, soc_port_t port,
                                           int flags, uint32 *value)
{
    uint64 reg_val;
    int rv = SOC_E_NONE;

    SOC_IF_ERROR_RETURN(READ_XLMAC_MERGE_CONFIGr(unit, port, &reg_val));
    
    switch (flags) {
        case XLMAC_PREEMPTION_FORCE_SW_RX_EOP:
            /* get the sw rx lost eop setting */
            *value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                           reg_val, SW_FORCE_RX_LOST_EOPf); 
            break;
        case XLMAC_PREEMPTION_FORCE_AUTO_HW_RX_EOP:
            /* get the hw rx lost eop setting */
            *value = soc_reg64_field32_get(unit, XLMAC_MERGE_CONFIGr,
                                      reg_val, AUTO_HW_FORCE_RX_LOST_EOPf);
            break;
        default:
            rv = SOC_E_PARAM;
    }
    return rv;
}

int xlmac_preemption_rx_timeout_info_set(int unit, soc_port_t port,
                                         uint32 enable, uint32 timeout_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(
            READ_XLMAC_MERGE_RX_TIMEOUT_CONFIGr(unit, port, &reg_val));
    
    soc_reg64_field32_set(unit, XLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                          &reg_val, TIMEOUT_ENABLEf, enable? 1: 0);
    
    soc_reg64_field32_set(unit, XLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                          &reg_val, TIMEOUT_CNTf, timeout_val);

    SOC_IF_ERROR_RETURN(
            WRITE_XLMAC_MERGE_RX_TIMEOUT_CONFIGr(unit, port, reg_val));
    
    return SOC_E_NONE;
}
                                    
int xlmac_preemption_rx_timeout_info_get(int unit, soc_port_t port,
                                         uint32 *enable, uint32 *timeout_val)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN(
            READ_XLMAC_MERGE_RX_TIMEOUT_CONFIGr(unit, port, &reg_val));
    
    *enable = soc_reg64_field32_get(unit, XLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                                    reg_val, TIMEOUT_ENABLEf);
    
    *timeout_val = soc_reg64_field32_get(unit, XLMAC_MERGE_RX_TIMEOUT_CONFIGr,
                                         reg_val, TIMEOUT_CNTf);

    return SOC_E_NONE;
}
                                    
#undef _ERR_MSG_MODULE_NAME


#endif /* PORTMOD_PM4X10_SUPPORT */
