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

#define HX5_BUFF_SIZE 256



/**
@name: tdm_hx5_cmn_get_port_pm
@param:

Return Port Macro number of the given port
 */
int
tdm_hx5_cmn_get_port_pm(int port_token, tdm_mod_t *_tdm)
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
@name: tdm_hx5_cmn_get_port_pm
@param:

Return Port Macro number of the given port
 */
int
tdm_hx5_cmn_get_port_pm_chk(int port_token, tdm_mod_t *_tdm)
{
    int port_pm;

    if (_tdm->_chip_data.soc_pkg.pmap_num_lanes > 0 &&
        port_token >=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
	if(port_token > 48 ){ /* pm 4x10q sis port spacing is across 16 ports */ 
        port_pm = (port_token - 1) / _tdm->_chip_data.soc_pkg.pmap_num_lanes;
	}
	else
	{
        port_pm = (port_token - 1) / 16 ;
	}

    } else {
        port_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    }

    return (port_pm);
/*     
    _tdm->_core_data.vars_pkg.port = port_token;

    return (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)); */
}




/**
@name: tdm_hx5_cmn_get_port_speed
@param:

Return port speed of the given port
 */
int
tdm_hx5_cmn_get_port_speed(int port_token, tdm_mod_t *_tdm)
{
    if (port_token >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return (_tdm->_chip_data.soc_pkg.speed[port_token]);
    }

    return (SPEED_0);
}


/**
@name: tdm_hx5_cmn_get_port_state
@param:

Return port state of the given port
 */
int
tdm_hx5_cmn_get_port_state(int port_token, tdm_mod_t *_tdm)
{
    if (port_token >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return (_tdm->_chip_data.soc_pkg.state[port_token-1]);
    }

    return (PORT_STATE__DISABLED);
}


/**
@name: tdm_hx5_cmn_get_speed_lanes
@param:

Return the number of lanes required by the given speed
(Chip-Specific)
 */
int
tdm_hx5_cmn_get_speed_lanes(int port_speed)
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
@name: tdm_hx5_cmn_get_speed_slots
@param:

Return the number of slots (2.5G per slot) of the given speed
(Chip-Specific)
 */
int 
tdm_hx5_cmn_get_speed_slots(int port_speed)
{
    int slot_num=0;

    switch (port_speed) {
        case SPEED_1G:  
            slot_num = 1;
            break;
        case SPEED_2G:  case SPEED_2p5G:
            slot_num = 2;
            break;
        case SPEED_10G: case SPEED_11G:
            slot_num = 8;  
            break;
        case SPEED_20G: case SPEED_21G:
            slot_num = 16;  
            break;
        case SPEED_25G: case SPEED_27G:
            slot_num = 20;  
            break;
        case SPEED_40G: case SPEED_42G: case SPEED_42G_HG2:
            slot_num = 32;  
            break;
        case SPEED_50G: case SPEED_53G:
            slot_num = 40;  
            break;
        case SPEED_100G: case SPEED_106G:
            slot_num = 80; 
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
@name: tdm_hx5_cmn_get_pipe_port_range
@param:

Return physical port number range of the given pipe
 */
void 
tdm_hx5_cmn_get_pipe_port_range(int cal_id, int *phy_lo, int *phy_hi)
{
    int prt_lo = 0, prt_hi = 0;

    switch(cal_id){
        case 0: prt_lo = HX5_CAL_0_PORT_LO; prt_hi = HX5_CAL_0_PORT_HI; break;
        case 1: prt_lo = HX5_CAL_1_PORT_LO; prt_hi = HX5_CAL_1_PORT_HI; break;
        case 4: prt_lo = HX5_CAL_4_PORT_LO; prt_hi = HX5_CAL_4_PORT_HI; break;
        case 5: prt_lo = HX5_CAL_5_PORT_LO; prt_hi = HX5_CAL_5_PORT_HI; break;
        default: break;
    }
    if (phy_lo != NULL && phy_hi != NULL) {
        *phy_lo = prt_lo;
        *phy_hi = prt_hi;
    }
}


/**
@name: tdm_hx5_cmn_get_pipe_ethernet
@param:

Return BOOL_TRUE if the given pipe has traffic entirely Ethernet;
otherwise, return BOOL_FALSE.
 */
int 
tdm_hx5_cmn_get_pipe_ethernet(int cal_id, tdm_mod_t *_tdm) 
{
    int port_phy, phy_lo, phy_hi, result=BOOL_TRUE;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;

    tdm_hx5_cmn_get_pipe_port_range(cal_id, &phy_lo, &phy_hi);
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
@name: tdm_hx5_cmn_get_pipe_higig
@param:

Return BOOL_TRUE if the given pipe has HiGig traffic;
otherwise, return BOOL_FALSE.
(Chip-Specific)
 */
int 
tdm_hx5_cmn_get_pipe_higig(int cal_id, tdm_mod_t *_tdm)
{
    int result = BOOL_FALSE;

    /* check regular ports */
    if (tdm_hx5_cmn_get_pipe_ethernet(cal_id, _tdm) == BOOL_FALSE) {
        result = BOOL_TRUE;
    }

    return (result);
}


/**
@name: tdm_hx5_cmn_get_pipe_cal_len_max
@param:

Return the size of selected calendar (regardless of frequency/higig)
 */
int
tdm_hx5_cmn_get_pipe_cal_len_max(int cal_id, tdm_mod_t *_tdm)
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
@name: tdm_hx5_cmn_get_pipe_cal_len
@param:

Return valid length of selected calendar under frequency/higig constraint
(Chip-Specific)
 */
int
tdm_hx5_cmn_get_pipe_cal_len(int cal_id, tdm_mod_t *_tdm)
{
    int cal_len = 0;
    int param_freq;

    param_freq = _tdm->_chip_data.soc_pkg.clk_freq;

    /* get calendar length */
    if (tdm_hx5_cmn_get_pipe_higig(cal_id, _tdm) == BOOL_FALSE) {
        switch (param_freq) {
            case HX5_CLK_450MHZ:  cal_len = HX5_LEN_450MHZ; break;
            case HX5_CLK_400MHZ:  cal_len = HX5_LEN_400MHZ; break;
            case HX5_CLK_543MHZ:  cal_len = HX5_LEN_543MHZ; break;
            case HX5_CLK_587MHZ:  cal_len = HX5_LEN_587MHZ; break;
            case HX5_CLK_668MHZ:  cal_len = HX5_LEN_668MHZ; break;
            case HX5_CLK_812MHZ:  cal_len = HX5_LEN_812MHZ; break;
            case HX5_CLK_893MHZ:  cal_len = HX5_LEN_893MHZ; break;
            case HX5_CLK_931MHZ:  cal_len = HX5_LEN_931MHZ; break;
            default:  break;
        }
    } else {
        switch (param_freq) {
            case HX5_CLK_450MHZ:  cal_len = HX5_LEN_450HMHZ; break;
            case HX5_CLK_400MHZ:  cal_len = HX5_LEN_400HMHZ; break;
            case HX5_CLK_543MHZ:  cal_len = HX5_LEN_543MHZ; break;
            case HX5_CLK_587MHZ:  cal_len = HX5_LEN_587HMHZ; break;
            case HX5_CLK_668MHZ:  cal_len = HX5_LEN_668MHZ; break;
            case HX5_CLK_812MHZ:  cal_len = HX5_LEN_812MHZ; break;
            case HX5_CLK_893MHZ:  cal_len = HX5_LEN_893MHZ; break;
            case HX5_CLK_931MHZ:  cal_len = HX5_LEN_931MHZ; break;
            default:  break;
        }
    }
    /* check validity of calendar length */
    if (cal_len == 0) {
        TDM_ERROR1("Invalid core clock frequency %0d\n", param_freq);
    } else if (cal_len > tdm_hx5_cmn_get_pipe_cal_len_max(cal_id, _tdm)) {
        cal_len = 0;
        TDM_ERROR1("Invalid calendar length %0d (overflow)\n", cal_len);
    }

    return (cal_len);
}


/**
@name: tdm_hx5_cmn_get_same_port_dist
@param:
 */
int
tdm_hx5_cmn_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len)
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
@name: tdm_hx5_cmn_get_speed_jitter
@param:
 */
void
tdm_hx5_cmn_get_speed_jitter(int port_speed, int cal_len, int *space_min, int *space_max)
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
 /*  TDM_LOG_NORMAL("%d, port_speed %d, cal_len %d, jitter_min %d, jitter_max ",port_speed , cal_len ,jitter_min , jitter_max ); */
}
