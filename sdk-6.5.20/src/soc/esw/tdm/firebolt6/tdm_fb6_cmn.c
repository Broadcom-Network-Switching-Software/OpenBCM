/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip-specific common facility functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif

/*#include <aLog.h>*/
#define TDM_LOG_VERBOSE(format, ...) aLOG_VERBOSE(format, ##__VA_ARGS__)
#define TDM_LOG_DEBUG(format, ...) aLOG_DEBUG(format, ##__VA_ARGS__)
#define TDM_LOG_NORMAL(format, ...) aLOG_NORMAL(format, ##__VA_ARGS__)
#define TDM_LOG_WARN(format, ...) aLOG_WARNING(format, ##__VA_ARGS__)
#define TDM_LOG_ERROR(format, ...) aLOG_ERROR(format, ##__VA_ARGS__)
#define TDM_LOG_FATAL(format, ...) aLOG_FATAL(format, ##__VA_ARGS__)
#define TDM_LOG_NL aLOG_NORMAL("%s", "");
#define TDM_LOG_NORMAL0(a) aLOG_NORMAL(a, "");
#define TDM_LOG_WARN0(a) aLOG_WARNING(a, "");
#define TDM_LOG_BAR_BIG aLOG_NORMAL("TDM: #################################################################################################################################", "\n");
#define TDM_LOG_BAR_SML aLOG_NORMAL("TDM: ---------------------------------------------------------------------------------------------------------------------------------", "\n");


#define FB6_BUFF_SIZE 256



/**
@name: tdm_fb6_cmn_get_port_pm
@param:

Return Port Macro number of the given port
 */
int
tdm_fb6_cmn_get_port_pm(int port_token, tdm_mod_t *_tdm)
{
    int port_pm;

    if (_tdm->_chip_data.soc_pkg.pmap_num_lanes > 0 &&
        port_token >=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        port_pm = (port_token - 1) / _tdm->_chip_data.soc_pkg.pmap_num_lanes ;
    } else {
        port_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    }

    return (port_pm);
/*
    _tdm->_core_data.vars_pkg.port = port_token;

    return (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)); */
}

/**
@name: tdm_fb6_cmn_get_pipe_pm_lo_hi
@param:

Return physical port number range of the given pipe
 */
void
tdm_fb6_cmn_get_pipe_pm_lo_hi(tdm_mod_t *_tdm, int *pm_lo, int *pm_hi)
{
    int prt_lo = 0, prt_hi = 0;

    if (pm_lo != NULL && pm_hi != NULL) {
        tdm_fb6_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);
        *pm_lo = tdm_fb6_cmn_get_port_pm(prt_lo, _tdm);
        *pm_hi = tdm_fb6_cmn_get_port_pm(prt_hi, _tdm);
    }
}



/**
@name: tdm_fb6_cmn_get_port_pm
@param:

Return Port Macro number of the given port
 */
int
tdm_fb6_cmn_get_port_pm_chk(int port_token, tdm_mod_t *_tdm)
{
    int port_pm;

    if (_tdm->_chip_data.soc_pkg.pmap_num_lanes > 0 &&
        port_token >=(_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo) &&
        port_token <=(_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi)) {
        port_pm = (port_token - 1) / _tdm->_chip_data.soc_pkg.pmap_num_lanes;

    } else {
        port_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    }

    return (port_pm);
/*
    _tdm->_core_data.vars_pkg.port = port_token;

    return (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)); */
}




/**
@name: tdm_fb6_cmn_get_port_speed
@param:

Return port speed of the given port
 */
int
tdm_fb6_cmn_get_port_speed(int port_token, tdm_mod_t *_tdm)
{
    if (port_token >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return (_tdm->_chip_data.soc_pkg.speed[port_token]);
    }

    return (SPEED_0);
}

/**
@name: tdm_fb6_cmn_get_port_speed_eth
@param:

Return port speed in ethernet mode. This converts HiGig speed to ethernet.
 */
int
tdm_fb6_cmn_get_port_speed_eth(tdm_mod_t *_tdm, int port_token)
{
    /* return (tdm_cmn_get_port_speed_eth(_tdm, port_token)); */
    int port_speed, port_speed_eth;

    port_speed = tdm_fb6_cmn_get_port_speed(port_token, _tdm);
    switch(port_speed) {
        case SPEED_1G:
        case SPEED_1p25G:
        case SPEED_2p5G:
        case SPEED_5G:
            port_speed_eth = port_speed;
            break;
        case SPEED_10G:
        case SPEED_10G_DUAL:
        case SPEED_10G_XAUI:
        case SPEED_11G:
            port_speed_eth = SPEED_10G;
            break;
        case SPEED_12p5G:
            port_speed_eth = SPEED_12p5G;
            break;
        case SPEED_20G:
        case SPEED_21G:
        case SPEED_21G_DUAL:
            port_speed_eth = SPEED_20G;
            break;
        case SPEED_25G:
        case SPEED_27G:
            port_speed_eth = SPEED_25G;
            break;
        case SPEED_40G:
        case SPEED_42G:
        case SPEED_42G_HG2:
            port_speed_eth = SPEED_40G;
            break;
        case SPEED_50G:
        case SPEED_53G:
            port_speed_eth = SPEED_50G;
            break;
        case SPEED_100G:
        case SPEED_106G:
            port_speed_eth = SPEED_100G;
            break;
        case SPEED_120G:
        case SPEED_127G:
            port_speed_eth = SPEED_120G;
            break;
        case SPEED_200G:
            port_speed_eth = SPEED_200G;
            break;
        case SPEED_400G:
            port_speed_eth = SPEED_400G;
            break;
        default:
            port_speed_eth = 0;
            break;
    }

    return port_speed_eth;
}



/**
@name: tdm_fb6_cmn_get_port_state
@param:

Return port state of the given port
 */
int
tdm_fb6_cmn_get_port_state(int port_token, tdm_mod_t *_tdm)
{
    if (port_token >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return (_tdm->_chip_data.soc_pkg.state[port_token-1]);
    }

    return (PORT_STATE__DISABLED);
}


/**
@name: tdm_fb6_cmn_get_speed_lanes
@param:

Return the number of lanes required by the given speed
(Chip-Specific)
 */
int
tdm_fb6_cmn_get_speed_lanes(int port_speed)
{
    int lanes = 0;

    switch(port_speed){
        case SPEED_1G:  case SPEED_2p5G:
        case SPEED_10G: case SPEED_11G:
        case SPEED_20G: case SPEED_21G:
        case SPEED_25G: case SPEED_27G:
            lanes = 1;
            break;
        case SPEED_40G: case SPEED_42G: case SPEED_42G_HG2:
        case SPEED_50G: case SPEED_53G:
            lanes = 2;
            break;
        case SPEED_100G: case SPEED_106G:
            lanes = 4;
            break;
        default:
            break;
    }

    return (lanes);
}


/**
@name: tdm_fb6_cmn_get_speed_slots
@param:

Return the number of slots (2.5G per slot) of the given speed
(Chip-Specific)
 */
int
tdm_fb6_cmn_get_speed_slots(int port_speed)
{
    int slot_num=0;

    switch (port_speed) {
        case SPEED_1G:
            slot_num = 1;
            break;
        case SPEED_2G:  case SPEED_2p5G:
            slot_num = 1;
            break;
        case SPEED_10G: case SPEED_11G:
            slot_num = 4;
            break;
        case SPEED_20G: case SPEED_21G:
            slot_num = 8;
            break;
        case SPEED_25G: case SPEED_27G:
            slot_num = 10;
            break;
        case SPEED_40G: case SPEED_42G: case SPEED_42G_HG2:
            slot_num = 16;
            break;
        case SPEED_50G: case SPEED_53G:
            slot_num = 20;
            break;
        case SPEED_100G: case SPEED_106G:
            slot_num = 40;
            break;
        default:
            TDM_ERROR4("%s, %s -- %dM (%dG)\n",
                       "[Port Speed]",
                       "invalid port speed",
                       port_speed, port_speed/1000);
            break;
    }

    return (slot_num);
}


/**
@name: tdm_fb6_cmn_get_pipe_port_range
@param:

Return physical port number range of the given pipe
 */
void
tdm_fb6_cmn_get_pipe_port_range(int cal_id, int *phy_lo, int *phy_hi)
{
    int prt_lo = 0, prt_hi = 0;

    switch(cal_id){
        case 0: prt_lo = FB6_CAL_0_PORT_LO; prt_hi = FB6_CAL_0_PORT_HI; break;
        case 1: prt_lo = FB6_CAL_1_PORT_LO; prt_hi = FB6_CAL_1_PORT_HI; break;
        case 4: prt_lo = FB6_CAL_4_PORT_LO; prt_hi = FB6_CAL_4_PORT_HI; break;
        case 5: prt_lo = FB6_CAL_5_PORT_LO; prt_hi = FB6_CAL_5_PORT_HI; break;
        default: break;
    }
    if (phy_lo != NULL && phy_hi != NULL) {
        *phy_lo = prt_lo;
        *phy_hi = prt_hi;
    }
}

/**
@name: tdm_fb6_cmn_get_pipe_port_lo_hi
@param:

Return physical port number range of the current pipe
 */
void
tdm_fb6_cmn_get_pipe_port_lo_hi(tdm_mod_t *_tdm, int *phy_lo, int *phy_hi)
{
    tdm_fb6_cmn_get_pipe_port_range(_tdm->_core_data.vars_pkg.cal_id,
                                    phy_lo, phy_hi);
}




/**
@name: tdm_fb6_cmn_get_pipe_ethernet
@param:

Return BOOL_TRUE if the given pipe has traffic entirely Ethernet;
otherwise, return BOOL_FALSE.
 */
int
tdm_fb6_cmn_get_pipe_ethernet(int cal_id, tdm_mod_t *_tdm)
{
    int port_phy, phy_lo, phy_hi, result=BOOL_TRUE;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;

    tdm_fb6_cmn_get_pipe_port_range(cal_id, &phy_lo, &phy_hi);
    if (phy_lo >= param_phy_lo && phy_hi <= param_phy_hi) {
        for (port_phy=phy_lo; port_phy<=phy_hi; port_phy++) {
            if (param_speeds[port_phy] > SPEED_0) {
                if (param_states[port_phy-1] == PORT_STATE__LINERATE_HG
                   ) {
                    result = BOOL_FALSE;
                    break;
                }
            }
        }
    }

    return (result);
}


/**
@name: tdm_fb6_cmn_get_pipe_higig
@param:

Return BOOL_TRUE if the given pipe has HiGig traffic;
otherwise, return BOOL_FALSE.
(Chip-Specific)
 */
int
tdm_fb6_cmn_get_pipe_higig(int cal_id, tdm_mod_t *_tdm)
{
    int result = BOOL_FALSE;

    /* check regular ports */
    if (tdm_fb6_cmn_get_pipe_ethernet(cal_id, _tdm) == BOOL_FALSE) {
        result = BOOL_TRUE;
    }

    return (result);
}


/**
@name: tdm_fb6_cmn_get_pipe_cal_len_max
@param:

Return the size of selected calendar (regardless of frequency/higig)
 */
int
tdm_fb6_cmn_get_pipe_cal_len_max(int cal_id, tdm_mod_t *_tdm)
{
    int cal_len = 0;

    switch (cal_id) {
        case 0: cal_len = _tdm->_chip_data.cal_0.cal_len; break;
        case 1: cal_len = _tdm->_chip_data.cal_1.cal_len; break;
        case 2: cal_len = _tdm->_chip_data.cal_2.cal_len; break;
        case 3: cal_len = _tdm->_chip_data.cal_3.cal_len; break;
        case 4: cal_len = _tdm->_chip_data.cal_4.cal_len; break;
        case 5: cal_len = _tdm->_chip_data.cal_5.cal_len; break;
        case 6: cal_len = _tdm->_chip_data.cal_6.cal_len; break;
        case 7: cal_len = _tdm->_chip_data.cal_7.cal_len; break;
        default: break;
    }

    return (cal_len);
}


/**
@name: tdm_fb6_cmn_get_pipe_cal_len
@param:

Return valid length of selected calendar under frequency/higig constraint
(Chip-Specific)
 */
int
tdm_fb6_cmn_get_pipe_cal_len(int cal_id, tdm_mod_t *_tdm)
{
    int cal_len = 0;
    int param_freq;

    param_freq = _tdm->_chip_data.soc_pkg.clk_freq;

    /* get calendar length */
    if (tdm_fb6_cmn_get_pipe_higig(cal_id, _tdm) == BOOL_FALSE) {
        switch (param_freq) {
    case FB6_CLK_4730_1012:  cal_len  = FB6_LEN_4730_1012; break;
    case FB6_CLK_4731_1012:  cal_len  = FB6_LEN_4731_1012; break;
    case FB6_CLK_4732_1012:  cal_len  = FB6_LEN_4732_1012; break;
    case FB6_CLK_4720_1250:  cal_len  = FB6_LEN_4720_1250; break;
    case FB6_CLK_4721_1250:  cal_len  = FB6_LEN_4721_1250; break;
    case FB6_CLK_4700_1250:  cal_len  = FB6_LEN_4700_1250; break;
    case FB6_CLK_4701_1250:  cal_len  = FB6_LEN_4701_1250; break;
    case FB6_CLK_4702_1250:  cal_len  = FB6_LEN_4702_1250; break;
    case FB6_CLK_4703_1250:  cal_len  = FB6_LEN_4703_1250; break;
    case FB6_CLK_4704_1087:  cal_len  = FB6_LEN_4704_1087; break;
    case FB6_CLK_4705_1250:  cal_len  = FB6_LEN_4703_1250; break;
    case FB6_CLK_1000_1012:  cal_len  = FB6_LEN_1000_1012; break;
    case FB6_CLK_1001_1250:  cal_len  = FB6_LEN_1001_1250; break;
            default:  break;
        }
    } else {
        switch (param_freq) {
    case FB6_CLK_4730_1012:  cal_len  = FB6_LEN_4730_1012; break;
    case FB6_CLK_4731_1012:  cal_len  = FB6_LEN_4731_1012; break;
    case FB6_CLK_4732_1012:  cal_len  = FB6_LEN_4732_1012; break;
    case FB6_CLK_4720_1250:  cal_len  = FB6_LEN_4720_1250; break;
    case FB6_CLK_4721_1250:  cal_len  = FB6_LEN_4721_1250; break;
    case FB6_CLK_4700_1250:  cal_len  = FB6_LEN_4700_1250; break;
    case FB6_CLK_4701_1250:  cal_len  = FB6_LEN_4701_1250; break;
    case FB6_CLK_4702_1250:  cal_len  = FB6_LEN_4702_1250; break;
    case FB6_CLK_4703_1250:  cal_len  = FB6_LEN_4703_1250; break;
    case FB6_CLK_4704_1087:  cal_len  = FB6_LEN_4704_1087; break;
    case FB6_CLK_4705_1250:  cal_len  = FB6_LEN_4703_1250; break;
    case FB6_CLK_1000_1012:  cal_len  = FB6_LEN_1000_1012; break;
    case FB6_CLK_1001_1250:  cal_len  = FB6_LEN_1001_1250; break;

            default:  break;
        }
    }
    /* check validity of calendar length */
    if (cal_len == 0) {
        TDM_ERROR1("Invalid core clock frequency %0d\n", param_freq);
    } else if (cal_len > tdm_fb6_cmn_get_pipe_cal_len_max(cal_id, _tdm)) {
        cal_len = 0;
        TDM_ERROR1("Invalid calendar length %0d (overflow)\n", cal_len);
    }

    return (cal_len);
}


/**
@name: tdm_fb6_cmn_get_same_port_dist
@param:
 */
int
tdm_fb6_cmn_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len)
{
    int n, k, dist = 0;

    if (slot < cal_len && cal_len > 0) {
        if (dir == TDM_DIR_UP) { /* UP direction */
            for (n = 1; n < cal_len; n++) {
                k = (slot + cal_len - n) % cal_len;
                if (cal_main[k] == cal_main[slot]) {
                    dist  = n;
                    break;
                }
            }
        } else { /* DOWN direction */
            for (n = 1; n < cal_len; n++) {
                k = (slot + n) % cal_len;
                if (cal_main[k] == cal_main[slot]) {
                    dist  = n;
                    break;
                }
            }
        }
    }

    return (dist);
}


/**
@name: tdm_fb6_cmn_get_speed_jitter
@param:
 */
void
tdm_fb6_cmn_get_speed_jitter(int port_speed, int cal_len, int *space_min, int *space_max)
{
    int slot_req, space_frac, jitter_range, jitter_min=0, jitter_max=0;

    /*  TDM_LOG_NORMAL("jitter compute for port_speed %d, cal_len %d ",port_speed , cal_len); */


    if (port_speed<SPEED_10G) {
        slot_req = (port_speed/100)/25;
    } else {
        slot_req = ((port_speed/10000)*100)/25;
    }

    slot_req = slot_req * 2 ;
    if(port_speed == SPEED_25G)
     slot_req = slot_req + 4 ;

    if (slot_req>0) {
        space_frac  = ((cal_len*10)/slot_req)%10;
        jitter_range= (2*cal_len)/(slot_req*5);
        jitter_range= ((((2*cal_len*10)/(slot_req*5))%10)<5)?
                      (jitter_range):
                      (jitter_range+1);
    /*  TDM_LOG_NORMAL("%d, space_frac %d, jitter_range , %d slot_req ",space_frac , jitter_range , slot_req); */

        if (space_frac<5){
            jitter_min = cal_len/slot_req - jitter_range/2;
            jitter_max = cal_len/slot_req + jitter_range/2;
            jitter_max = ((jitter_range%2)==0)?
                        (jitter_max):
                        (jitter_max+1);
        } else {
            jitter_min = cal_len/slot_req - jitter_range/2;
            jitter_min = ((cal_len%slot_req)==0)?
                        (jitter_min):
                        (jitter_min+1);
            jitter_min = ((jitter_range%2)==0)?
                        (jitter_min):
                        (jitter_min-1);
            jitter_max = cal_len/slot_req + jitter_range/2;
            jitter_max = ((cal_len%slot_req)==0)?
                        (jitter_max):
                        (jitter_max+1);
        }
        jitter_min = (jitter_min<1)? (1): (jitter_min);
        jitter_max = (jitter_max<1)? (1): (jitter_max);
    }
    if (space_min != NULL && space_max != NULL) {
        *space_min = jitter_min;
        *space_max = jitter_max;
    }
   /*TDM_LOG_NORMAL("port_speed %d, cal_len %d, jitter_min %d, jitter_max %d\n",port_speed , cal_len ,jitter_min , jitter_max );*/
}


/**
@name: tdm_cmn_chk_port_fp
@param:
@desc: Return BOOL_TRUE if given port is a front panel port,
       otherwise return BOOL_FALSE.
 */
int
tdm_fb6_cmn_chk_port_fp(tdm_mod_t *_tdm, int port_token)
{
    int param_clk_freq;
    int start_port;
    int end_port;

    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;

    switch(param_clk_freq){
        case FB6_CLK_4730_1012:
        case     FB6_CLK_4731_1012:
        case     FB6_CLK_4732_1012:
        case     FB6_CLK_4720_1250:
        case     FB6_CLK_4721_1250:
        case     FB6_CLK_4701_1250:
        case     FB6_CLK_4702_1250:
        case     FB6_CLK_4703_1250:
        case     FB6_CLK_4705_1250:
        case     FB6_CLK_1000_1012:
        case     FB6_CLK_1001_1250:  start_port= 5; end_port = 68; break;
        case     FB6_CLK_4700_1250:
        case     FB6_CLK_4704_1087:  start_port = 1; end_port = 64; break;
        default :  start_port = 5; end_port = 68;
       break;

    }
    return ((port_token >= start_port &&
             port_token <= end_port) ?
            BOOL_TRUE : BOOL_FALSE);
}



/**
@name: tdm_fb6_cmn_chk_port_is_os
@param:
@desc: Return BOOL_TRUE if given port is an oversubscript port,
       otherwise return BOOL_FALSE.
 */
int
tdm_fb6_cmn_chk_port_is_os(tdm_mod_t *_tdm, int port_token)
{
    /* return (tdm_cmn_chk_port_is_os(_tdm, port_token)); */
    int port_state;
    port_state = tdm_fb6_cmn_get_port_state(port_token, _tdm);
    return ((port_state == PORT_STATE__OVERSUB    ||
             port_state == PORT_STATE__OVERSUB_HG) ?
            BOOL_TRUE : BOOL_FALSE);
}





/**
@name: tdm_fb6_cmn_get_mgmt_port_range
@param:

Return Management port number range of the given pipe
 */
void
tdm_fb6_cmn_get_mgmt_port_range(tdm_mod_t *_tdm, int *mgmt_lo, int *mgmt_hi)
{
    int prt_lo = 0, prt_hi = 0;
    int param_clk_freq;

    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
    switch(param_clk_freq){
        case FB6_CLK_4730_1012:
        case     FB6_CLK_4731_1012:
        case     FB6_CLK_4732_1012:
        case     FB6_CLK_4720_1250:
        case     FB6_CLK_4721_1250:
        case     FB6_CLK_4701_1250:
        case     FB6_CLK_4702_1250:
        case     FB6_CLK_4703_1250:
        case     FB6_CLK_4705_1250:
        case     FB6_CLK_1000_1012:
        case     FB6_CLK_1001_1250:  prt_lo = 1; prt_hi = 4; break;
        case     FB6_CLK_4700_1250:
        case     FB6_CLK_4704_1087:  prt_lo = 65; prt_hi = 68; break;
        default :  prt_lo = 1; prt_hi = 4;
       break;

    }
    *mgmt_lo = prt_lo;
    *mgmt_hi = prt_hi;
}


/**
@name: tdm_fb6_cmn_get_mgmt_tokens
@param:

Return Management tokens given pipe
 */
void
tdm_fb6_cmn_get_mgmt_tokens(tdm_mod_t *_tdm, int *mgmt1_token, int *mgmt2_token, int *mgmt3_token, int *mgmt4_token)
{

    int param_mgmt_mode[FB6_NUM_EXT_PORTS];
    int j, param_mgmt_pm_num= 0;
    int mgmt_en = 0;
    int param_clk_freq;

    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;

    for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      param_mgmt_mode[j] = _tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode[j];
    }

   *mgmt1_token = 1;
   *mgmt2_token = 2;
   *mgmt3_token = 3;
   *mgmt4_token = 4;

   for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      if(param_mgmt_mode[j] == 1) {
        param_mgmt_pm_num = j/4;
        mgmt_en = 1;
      }
   }

   /*TDM_PRINT4("CMN::MGMT PM::Before mgmt1_token %d mgmt2_token %d mgmt3_token %d mgmt4_token %d\n",  *mgmt1_token, *mgmt2_token, *mgmt3_token, *mgmt4_token);
   TDM_PRINT1("CMN::MGMT PM::mgmt_en %d \n",  mgmt_en);*/
   if(mgmt_en==1) {
   *mgmt1_token = param_mgmt_pm_num*4+1;
   *mgmt2_token = param_mgmt_pm_num*4+2;
   *mgmt3_token = param_mgmt_pm_num*4+3;
   *mgmt4_token = param_mgmt_pm_num*4+4;
   }

   if(mgmt_en==0) {
     switch(param_clk_freq){
        case     FB6_CLK_4701_1250:
        case     FB6_CLK_4702_1250:
        case     FB6_CLK_4703_1250:
        case     FB6_CLK_4705_1250:
        case     FB6_CLK_4730_1012:
        case     FB6_CLK_4731_1012:
        case     FB6_CLK_4732_1012:
        case     FB6_CLK_4720_1250:
        case     FB6_CLK_4721_1250:
        case     FB6_CLK_1000_1012:
        case     FB6_CLK_1001_1250: *mgmt1_token = 1; *mgmt2_token = 2; *mgmt3_token = 3; *mgmt4_token = 4;
        break;
        case     FB6_CLK_4700_1250:
        case     FB6_CLK_4704_1087: *mgmt1_token = 65; *mgmt2_token = 66; *mgmt3_token = 67; *mgmt4_token = 68;
        break;
        default : *mgmt1_token = 1; *mgmt2_token = 2; *mgmt3_token = 3; *mgmt4_token = 4;
        break;


   }
 }

   TDM_PRINT4("CMN::MGMT PM::mgmt1_token %d mgmt2_token %d mgmt3_token %d mgmt4_token %d\n",  *mgmt1_token, *mgmt2_token, *mgmt3_token, *mgmt4_token);


}

/**
@name: tdm_fb6_cmn_get_mgmt_pm
@param:

Return Management PM
 */
void
tdm_fb6_cmn_get_mgmt_pm(tdm_mod_t *_tdm, int *mgmt_pm_num)
{
    int param_mgmt_mode[FB6_NUM_EXT_PORTS];
    int j;
    int mgmt_en=0;
    int param_clk_freq;
    enum port_state_e *param_states;

    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
    param_states   = _tdm->_chip_data.soc_pkg.state;

    for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      param_mgmt_mode[j] = _tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode[j];
    }

   for(j=0; j<FB6_NUM_EXT_PORTS; j++) {
      if((_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4731_1012) ||
         (_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4732_1012)) {
         if(param_states[j] == PORT_STATE__LINERATE) {
           *mgmt_pm_num = j/4;
           mgmt_en = 1;
         }
      }
      else {
        if(param_mgmt_mode[j] == 1) {
          *mgmt_pm_num = j/4;
          mgmt_en = 1;
        }
      }
   }


   if(mgmt_en==0) {
     switch(param_clk_freq){
        case     FB6_CLK_4701_1250:
        case     FB6_CLK_4702_1250:
        case     FB6_CLK_4703_1250:
        case     FB6_CLK_4705_1250:
        case     FB6_CLK_4730_1012:
        case     FB6_CLK_4731_1012:
        case     FB6_CLK_4732_1012:
        case     FB6_CLK_4720_1250:
        case     FB6_CLK_4721_1250:
        case     FB6_CLK_1000_1012:
        case     FB6_CLK_1001_1250: *mgmt_pm_num = 0;
        break;
        case     FB6_CLK_4700_1250:
        case     FB6_CLK_4704_1087: *mgmt_pm_num = 16;
        break;
        default : *mgmt_pm_num = 0;
        break;

     }
   }

   TDM_PRINT1("CMN::MGMT PM::mgmt_pm %d \n",  *mgmt_pm_num);
}


/**
@name: tdm_fb6_cmn_get_half_pipe_map_en
@param:

Return Half Pipe Mapping
A1 - 4701, 4702, 4703, 4732, 4731, 4730, 4720, 4721, 1000, 1001
    PM0 - 1,2,3,4 - MGMT PORTS
    PM1, PM3, PM5-10 - HP0
    PM2, PM4, PM11-16 - HP1
A2 - 4700, 4704, 4732
    PM16 - 65, 66, 67, 68 - MGMT PORTS
    PM0-PM5, PM12, PM14 - HP0
    PM6-PM11, PM13, PM15 - HP1
B1 - 4705
    PM0 - 1,2,3,4 - MGMT PORTS
    PM1-PM6, PM13, PM15 - HP0
    PM7-PM12, PM14, PM16 - HP1
B2 -
    PM0 - 1,2,3,4 - MGMT PORTS
    PM1-PM6, PM7, PM9 - HP0
    PM11-PM16, PM8, PM10 - HP1
 */
void
tdm_fb6_cmn_get_half_pipe_map_en(tdm_mod_t *_tdm, int half_pipe_map_en[])
{
    int param_clk_freq;
    int i;
    int pm_num_lo=0;
    int pm_num_hi=16;
    int mgmt_pm_val=0;
    int default_mgmt_pm = 0;
    int half_pipe_val;

    param_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
    switch(param_clk_freq){
        case     FB6_CLK_4701_1250:
        case     FB6_CLK_4702_1250:
        case     FB6_CLK_4703_1250:
        case     FB6_CLK_4730_1012:
        case     FB6_CLK_4731_1012:
        case     FB6_CLK_4732_1012:
        case     FB6_CLK_4720_1250:
        case     FB6_CLK_4721_1250:
        case     FB6_CLK_1000_1012:
        case     FB6_CLK_1001_1250:
          for (i=pm_num_lo; i<=pm_num_hi; i++) {
            if((i==1) || (i==3) || (i>=5 && i<=10))
             half_pipe_map_en[i] = 1;
            else if((i==2) || (i==4) || (i>=11 && i<=16))
             half_pipe_map_en[i] = 0;
            else
             half_pipe_map_en[i] = 0;
          }
          default_mgmt_pm = 0;
          break;
        case     FB6_CLK_4700_1250:
        case     FB6_CLK_4704_1087:
          for (i=pm_num_lo; i<=pm_num_hi; i++) {
            if((i==12) || (i==14) || (i>=0 && i<=5))
             half_pipe_map_en[i] = 1;
            else if((i==13) || (i==15) || (i>=6 && i<=11))
             half_pipe_map_en[i] = 0;
            else
             half_pipe_map_en[i] = 0;
          }
          default_mgmt_pm = 16;
          break;
        case     FB6_CLK_4705_1250:
          for (i=pm_num_lo; i<=pm_num_hi; i++) {
            if((i==13) || (i==15) || (i>=1 && i<=6))
             half_pipe_map_en[i] = 1;
            else if((i==14) || (i==16) || (i>=7 && i<=12))
             half_pipe_map_en[i] = 0;
            else
             half_pipe_map_en[i] = 0;
          }
          default_mgmt_pm = 0;
          break;
        default : half_pipe_map_en[0] = 0;
          default_mgmt_pm = 0;
       break;

    }

   tdm_fb6_cmn_get_mgmt_pm(_tdm, &mgmt_pm_val);
   half_pipe_val = half_pipe_map_en[mgmt_pm_val];
   half_pipe_map_en[default_mgmt_pm] = half_pipe_val;


}

/**
@name: tdm_fb6_cmn_get_pipe_cal
@param:

Return calendar pointer of the current pipe.
 */
tdm_calendar_t *tdm_fb6_cmn_get_pipe_cal(tdm_mod_t *_tdm)
{
    /* return (tdm_cmn_get_pipe_cal(_tdm)); */
    tdm_calendar_t * cal = NULL;

    if (_tdm != NULL) {
        switch (_tdm->_core_data.vars_pkg.cal_id) {
            case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
            case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
            case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
            case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
            case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
            case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
            case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
            case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
            default:
                TDM_PRINT1("Invalid calendar ID - %0d\n",
                           _tdm->_core_data.vars_pkg.cal_id);
                cal = NULL;
                break;
        }
    }

    return cal;
}


/**
@name: tdm_fb6_cmn_get_pipe_cal_prev
@param:

Return calendar pointer of the previous pipe.
 */
tdm_calendar_t *tdm_fb6_cmn_get_pipe_cal_prev(tdm_mod_t *_tdm)
{
    /* return (tdm_cmn_get_pipe_cal_prev(_tdm)); */
    tdm_calendar_t * cal = NULL;

    if (_tdm != NULL) {
        switch (_tdm->_core_data.vars_pkg.cal_id) {
            case 0:	cal=(&(_tdm->_prev_chip_data.cal_0)); break;
            case 1:	cal=(&(_tdm->_prev_chip_data.cal_1)); break;
            case 2:	cal=(&(_tdm->_prev_chip_data.cal_2)); break;
            case 3:	cal=(&(_tdm->_prev_chip_data.cal_3)); break;
            case 4:	cal=(&(_tdm->_prev_chip_data.cal_4)); break;
            case 5:	cal=(&(_tdm->_prev_chip_data.cal_5)); break;
            case 6:	cal=(&(_tdm->_prev_chip_data.cal_6)); break;
            case 7:	cal=(&(_tdm->_prev_chip_data.cal_7)); break;
            default:
                TDM_PRINT1("Invalid calendar ID - %0d\n",
                           _tdm->_core_data.vars_pkg.cal_id);
                cal = NULL;
                break;
        }
    }

    return cal;
}


