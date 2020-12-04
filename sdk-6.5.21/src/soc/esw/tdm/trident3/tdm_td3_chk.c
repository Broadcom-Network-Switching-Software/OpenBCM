/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip self-check functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_td3_chk_get_speed_slots_5G
@param:

Return the number of slots (5G per slot) of the given speed
 */
int 
tdm_td3_chk_get_speed_slots_5G(enum port_speed_e port_speed)
{
    int slot_num;
    
    slot_num = tdm_td3_cmn_get_speed_slots(port_speed);
    if (slot_num > 1) {
        slot_num /= 2;
    }
    
    return (slot_num);
}


/**
@name: tdm_td3_chk_get_pipe_token_cnt
@param:

Return number of slots of the given token in selected calendar
 */
int 
tdm_td3_chk_get_pipe_token_cnt(tdm_mod_t *_tdm, int cal_id, int port_token)
{
    int i, cnt=0;
    int param_cal_len;
    int *param_cal_main=NULL;
    
    param_cal_len = tdm_td3_cmn_get_pipe_cal_len(_tdm, cal_id);
    TDM_SEL_CAL(cal_id, param_cal_main);
    if (param_cal_main != NULL) {
        for (i=0; i<param_cal_len; i++) {
            if (param_cal_main[i] == port_token) {
                cnt++;
            }
        }
    }
    
    return (cnt);
}

/**
@name: tdm_td3_chk_get_port_lanes
@param:

Return number of lanes of the given port
 */
int 
tdm_td3_chk_get_port_lanes(tdm_mod_t *_tdm, int port)
{
    int lanes = 0, port_start;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds=NULL;

    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;

    if (port >= param_phy_lo && port <= param_phy_hi) {
        switch (param_speeds[port]) {
            case SPEED_10G:  case SPEED_11G:  lanes = 1; break;
            case SPEED_20G:  case SPEED_21G:  lanes = 2; break;
            case SPEED_25G:  case SPEED_27G:  lanes = 1; break;
            case SPEED_40G:  case SPEED_42G:  lanes = 2; break;
            case SPEED_50G:  case SPEED_53G:  lanes = 2; break;
            case SPEED_100G: case SPEED_106G: lanes = 4; break;
            default: lanes = 0; break;
        }
        if (param_speeds[port] == SPEED_40G || param_speeds[port] == SPEED_42G) {
            port_start = ((port - 1) / TD3_NUM_PM_LNS) * TD3_NUM_PM_LNS + 1;
            if (port_start == port &&
                param_speeds[port_start + 1] == SPEED_0 &&
                param_speeds[port_start + 2] == SPEED_0 &&
                param_speeds[port_start + 3] == SPEED_0 ) {
                lanes = 4;
            }
        }
    }

    return (lanes);
}


/**
@name: tdm_td3_chk_struct
@param:

Checks frequency, calendar length, and speed/state type.
(Chip-Specific)
 */
int
tdm_td3_chk_struct(tdm_mod_t *_tdm)
{
    int i, cal_id, cal_len_limit=0, cal_len_act=0,
        port_phy, port_speed, result=PASS;
    int param_freq, param_phy_lo, param_phy_hi, param_token_ext;
    int *param_cal_main=NULL;

    param_freq      = _tdm->_chip_data.soc_pkg.clk_freq;
    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ext = _tdm->_chip_data.soc_pkg.num_ext_ports;

    /* frequency */
    switch (param_freq) {
        case TD3_CLK_1700MHZ:
        case TD3_CLK_1625MHZ:
        case TD3_CLK_1525MHZ:
        case TD3_CLK_1425MHZ:
        case TD3_CLK_1325MHZ:
        case TD3_CLK_1275MHZ:
        case TD3_CLK_1125MHZ:
        case TD3_CLK_1012MHZ:
        case TD3_CLK_850MHZ:
            break;
        default:
            TDM_ERROR2("%s, invalid frequency %d\n", 
                       "[Structure-Frequency]",
                       param_freq);
            result = FAIL;
            break;
    }
    /* calendar length */
    for (cal_id=0; cal_id<8; cal_id++){
        if (cal_id == TD3_IDB_PIPE_2_CAL_ID ||
            cal_id == TD3_IDB_PIPE_3_CAL_ID ||
            cal_id == TD3_MMU_PIPE_2_CAL_ID || 
            cal_id == TD3_MMU_PIPE_3_CAL_ID) {
            continue;
        }
        cal_len_act = tdm_td3_cmn_get_pipe_cal_len_max(_tdm, cal_id);
        cal_len_limit = tdm_td3_cmn_get_pipe_cal_len(_tdm, cal_id);
        TDM_SEL_CAL(cal_id, param_cal_main);
        for (i=cal_len_act; i>0; i--) {
            if (param_cal_main[i-1] == param_token_ext) {
                cal_len_act--;
            } else {
                break;
            }
        }
        
        if (cal_len_act>cal_len_limit) {
            result = FAIL;
            TDM_ERROR4("%s, cal_id %d, length %d, limit %d\n",
                "[Structure-CalLength], calendar length exceeded",
                cal_id, cal_len_act, cal_len_limit);
        }
    }
    /* speed/state */
    for (port_phy=param_phy_lo; port_phy<=param_phy_hi; port_phy++){
        port_speed = tdm_td3_cmn_get_port_speed(_tdm, port_phy);
        if (port_speed>SPEED_0) {
            if (BOOL_TRUE == tdm_td3_cmn_chk_port_is_lr(_tdm, port_phy) ||
                BOOL_TRUE == tdm_td3_cmn_chk_port_is_os(_tdm, port_phy) ||
                BOOL_TRUE == tdm_td3_cmn_chk_port_is_mgmt(_tdm, port_phy)) {
                switch(port_speed){
                    case SPEED_1G:
                    case SPEED_2p5G:
                    case SPEED_10G: case SPEED_11G:
                    case SPEED_20G: case SPEED_21G:
                    case SPEED_25G: case SPEED_27G:
                    case SPEED_40G: case SPEED_42G: case SPEED_42G_HG2:
                    case SPEED_50G: case SPEED_53G:
                    case SPEED_100G:case SPEED_106G:
                        break;
                    default:
                        result = FAIL;
                        TDM_ERROR3("%s, port %3d, speed %dG\n",
                            "[Structure-Speed], invalid speed",
                            port_phy, (port_speed/1000));
                        break;
                }
            } else {
                result = FAIL;
                TDM_ERROR3("%s, port %3d, state %d\n",
                    "[Structure-State], invalid state",
                    port_phy, tdm_td3_cmn_get_port_state(_tdm, port_phy));
            }
        }
    }

    return (result);
}


/**
@name: tdm_td3_chk_transcription
@param:

Verify Port Macro transcription caught all ports indexed by port_state array
 */
int 
tdm_td3_chk_transcription(tdm_mod_t *_tdm)
{
    int i, port_phy, port_speed, port_tsc, port_lanes,
        port_lanes_exp, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_pm_lanes, param_pm_num;
    int **param_pmap;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_num   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pmap     = _tdm->_chip_data.soc_pkg.pmap;

    /* check port transcription */
    for (port_phy=param_phy_lo; port_phy<=param_phy_hi; port_phy++) {
        port_speed = tdm_td3_cmn_get_port_speed(_tdm, port_phy);
        if (port_speed>SPEED_0) {
            port_tsc = tdm_td3_cmn_get_port_pm(_tdm, port_phy);
            if (port_tsc<param_pm_num) {
                port_lanes_exp = tdm_td3_cmn_get_speed_lanes(port_speed);
                port_lanes = 0;
                for (i=0; i<param_pm_lanes; i++) {
                    if(port_phy == param_pmap[port_tsc][i]) {
                        port_lanes++;
                    }
                }
                if (port_lanes != port_lanes_exp) {
                    result = FAIL;
                    TDM_ERROR5("%s, port %3d, speed %dG, lane_num %d, lane_num_limit %d\n",
                            "[Port Transcription], invalid lane number",
                            port_phy, (port_speed/1000), port_lanes, port_lanes_exp);
                }
            } else {
                result = FAIL;
                TDM_ERROR2("%s, port %3d is NOT transcribed\n",
                    "[Port Transcription]", 
                    port_phy);
            }
        }
    }

    return (result);
}


/**
@name: tdm_td3_chk_sister
@param:

Check sister port spacing constraint
 */
int 
tdm_td3_chk_sister(tdm_mod_t *_tdm)
{
    int i, j, k, cal_id, port_i, port_k, tsc_i, tsc_k, result=PASS;
    int param_cal_len, param_sister_min, param_phy_lo, param_phy_hi;
    int *param_cal_main=NULL;
    enum port_speed_e *param_speeds;
    
    param_sister_min = _tdm->_core_data.rule__prox_port_min;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;
    
    for (cal_id=0; cal_id<8; cal_id++){
        TDM_SEL_CAL(cal_id, param_cal_main);
        if (param_cal_main == NULL) {
            continue;
        }
        param_cal_len = tdm_td3_cmn_get_pipe_cal_len(_tdm, cal_id);
        for (i=0; i<param_cal_len; i++){
            port_i = param_cal_main[i];
            if (port_i>=param_phy_lo && port_i<=param_phy_hi) {
                tsc_i = tdm_td3_cmn_get_port_pm(_tdm, port_i);
                for (j=1; j<param_sister_min; j++){
                    k = (i+j)%param_cal_len;
                    port_k = param_cal_main[k];
                    if (port_k>=param_phy_lo && port_k<=param_phy_hi) {
                        tsc_k = tdm_td3_cmn_get_port_pm(_tdm, port_k);
                        if (tsc_i == tsc_k &&
                            param_speeds[port_i] > 0 &&
                            param_speeds[port_k] > 0) {
                            result = FAIL;
                            TDM_ERROR5("%s, port[%3d,%3d], index[%3d,%3d],\n",
                                       "[SISTER Port Spacing]",
                                       port_i, port_k, i, k);
                        }
                    }
                }
            }
        }
    }
    
    return (result);
}


/**
@name: tdm_td3_chk_same (Version 2.0)
@param:

Check same port spacing constraint
 */
int 
tdm_td3_chk_same(tdm_mod_t *_tdm)
{
    int i, j, k, cal_id, port_i, port_k, port_speed, result=PASS;
    int param_cal_len, param_same_min, param_phy_lo, param_phy_hi;
	int *param_cal_main=NULL;
    enum port_speed_e *param_speeds;
	
    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;
    for (cal_id=0; cal_id<8; cal_id++) {
        TDM_SEL_CAL(cal_id, param_cal_main);
        if (param_cal_main == NULL) {
            continue;
        }
        param_cal_len = tdm_td3_cmn_get_pipe_cal_len(_tdm, cal_id);
        for (i=0; i<param_cal_len; i++) {
            port_i = param_cal_main[i];
            if (port_i>=param_phy_lo && port_i<=param_phy_hi) {
                port_speed = param_speeds[port_i];
                if((cal_id == TD3_MMU_PIPE_0_CAL_ID || 
                    cal_id == TD3_MMU_PIPE_1_CAL_ID) &&
                   (port_speed == SPEED_10G ||
                    port_speed == SPEED_11G ||
                    port_speed == SPEED_25G ||
                    port_speed == SPEED_27G)) {
                    param_same_min = TD3_MIN_SPACING_SAME_PORT_LSP;
                } else {
                    param_same_min = TD3_MIN_SPACING_SAME_PORT_HSP;
                }
                for (j=1; j<param_same_min; j++){
                    k = (i+j)%param_cal_len;
                    port_k = param_cal_main[k];
                    if (port_i == port_k) {
                        result = FAIL;
                        TDM_ERROR7("%s, port %3d, speed %3dG, index[%3d,%3d], %d<%d\n",
                            "[SAME Port Spacing]",
                            port_i, port_speed/1000, i, k, j, param_same_min);
                    }
                }
            }
        }
    }
    
    return (result);
}

/**
@name: tdm_td3_chk_bandwidth_ancl
@param:

Check bandwidth for Ancillary port, i.e. CPU, Management, Loopback, PURGE, SBUBS, ect.
(Chip-Specific)
 */
int 
tdm_td3_chk_bandwidth_ancl(tdm_mod_t *_tdm)
{
    int cal_id, slot_req, slot_cnt, slot_token,
        slot_cpu,  token_cpu,
        slot_mgmt, token_mgmt,
        slot_lpbk, token_lpbk,
        slot_opt1, token_opt1,
        slot_null, token_null,
        slot_idle, token_idle,
        result=PASS;
    int param_token_cmic, param_token_null,
        param_token_mgm1, param_token_mgm2,
        param_token_idl1, param_token_idl2, 
        param_token_lpb0, param_token_lpb1,
        param_token_ext, param_mgmt_mode;
    enum port_speed_e *param_speeds;
    
    param_token_mgm1= TD3_MGM1_TOKEN;
    param_token_mgm2= TD3_MGM2_TOKEN;
    param_token_cmic= TD3_CMIC_TOKEN;
    param_token_idl1= TD3_IDL1_TOKEN;
    param_token_idl2= TD3_IDL2_TOKEN;
    param_token_null= TD3_NULL_TOKEN;
    param_token_lpb0= TD3_LPB0_TOKEN;
    param_token_lpb1= TD3_LPB1_TOKEN;
    param_token_ext = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_mgmt_mode = _tdm->_chip_data.soc_pkg.soc_vars.td3.mgmt_mode;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;
    
    for (cal_id=0; cal_id<8; cal_id++) {
        if (cal_id == TD3_IDB_PIPE_0_CAL_ID) {
            /* IDB 0 */
            slot_cpu  = TD3_NUM_ANCL_CPU;      token_cpu  = param_token_cmic;
            slot_mgmt = 0;                     token_mgmt = param_token_ext;
            slot_lpbk = TD3_NUM_ANCL_LPBK;     token_lpbk = param_token_lpb0;
            slot_opt1 = TD3_NUM_ANCL_IDB_OPT1; token_opt1 = param_token_idl1;
            slot_null = TD3_NUM_ANCL_IDB_NULL; token_null = param_token_null;
            slot_idle = TD3_NUM_ANCL_IDB_IDLE; token_idle = param_token_idl2;
        } else if (cal_id == TD3_IDB_PIPE_1_CAL_ID) {
            /* IDB 1 */
            slot_cpu  = 0;                     token_cpu  = param_token_ext;
            slot_mgmt = TD3_NUM_ANCL_MGMT;     token_mgmt = param_token_mgm1;
            slot_lpbk = TD3_NUM_ANCL_LPBK;     token_lpbk = param_token_lpb1;
            slot_opt1 = TD3_NUM_ANCL_IDB_OPT1; token_opt1 = param_token_idl1;
            slot_null = TD3_NUM_ANCL_IDB_NULL; token_null = param_token_null;
            slot_idle = TD3_NUM_ANCL_IDB_IDLE; token_idle = param_token_idl2;
        } else if (cal_id == TD3_MMU_PIPE_0_CAL_ID) {
            /* MMU 0 */
            slot_cpu  = TD3_NUM_ANCL_CPU;      token_cpu  = param_token_cmic;
            slot_mgmt = 0;                     token_mgmt = param_token_ext;
            slot_lpbk = TD3_NUM_ANCL_LPBK;     token_lpbk = param_token_lpb0;
            slot_opt1 = TD3_NUM_ANCL_MMU_OPT1; token_opt1 = param_token_ext;
            slot_null = TD3_NUM_ANCL_MMU_NULL; token_null = param_token_null;
            slot_idle = TD3_NUM_ANCL_MMU_IDLE; token_idle = param_token_idl2;
        } else if (cal_id == TD3_MMU_PIPE_1_CAL_ID) {
            /* MMU 1 */
            slot_cpu  = 0;                     token_cpu  = param_token_ext;
            slot_mgmt = TD3_NUM_ANCL_MGMT;     token_mgmt = param_token_mgm1;
            slot_lpbk = TD3_NUM_ANCL_LPBK;     token_lpbk = param_token_lpb1;
            slot_opt1 = TD3_NUM_ANCL_MMU_OPT1; token_opt1 = param_token_ext;
            slot_null = TD3_NUM_ANCL_MMU_NULL; token_null = param_token_null;
            slot_idle = TD3_NUM_ANCL_MMU_IDLE; token_idle = param_token_idl2;
        } else {
            slot_cpu  = 0; token_cpu  = param_token_ext;
            slot_mgmt = 0; token_mgmt = param_token_ext;
            slot_lpbk = 0; token_lpbk = param_token_ext;
            slot_opt1 = 0; token_opt1 = param_token_ext;
            slot_null = 0; token_null = param_token_ext;
            slot_idle = 0; token_idle = param_token_ext;
        }
        /* re-adjust CPU/MGMT slot number based on mgmt_mode */
        if (param_mgmt_mode==TD3_MGMT_MODE_1_PORT_EN ||
            param_mgmt_mode==TD3_MGMT_MODE_1_PORT_HG) {
            slot_cpu /= 2;
            slot_mgmt/= 2;
        }
        /* CPU */
        if (slot_cpu>0) {
            slot_token = token_cpu;
            slot_req   = slot_cpu;
            slot_cnt   = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient CPU bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
        }
        /* Management */
        if (slot_mgmt>0) {
            slot_token = token_mgmt;
            slot_req   = slot_mgmt;
            slot_cnt   = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            slot_cnt  += tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, param_token_mgm2);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient MANAGEMENT bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
            /* second management port constraint */
            if ( (param_mgmt_mode==TD3_MGMT_MODE_2_PORT_EN ||
                  param_mgmt_mode==TD3_MGMT_MODE_2_PORT_HG) && 
                 (param_speeds[128]>0) ) {
                result = FAIL;
                TDM_ERROR3("%s, %s, cal_id %d\n",
                        "[Ancillary Bandwidth]",
                        "invalid config of the second MANAGEMENT port",
                        cal_id);
            }
        }
        /* Loopback */
        if (slot_lpbk>0) {
            slot_token = token_lpbk;
            slot_req   = slot_lpbk;
            slot_cnt   = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient LOOPBACK bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
        }
        /* opportunistic-1 */
        if (slot_opt1>0) {
            slot_token = token_opt1;
            slot_req   = slot_opt1;
            slot_cnt   = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient OPPORTUNISTIC-1 bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
        }
        /* Null */
        if (slot_null>0) {
            slot_token = token_null;
            slot_req   = slot_null;
            slot_cnt   = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient NULL bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
        }
        /* Idle */
        if (slot_idle>0) {
            slot_token = token_idle;
            slot_req   = slot_idle;
            slot_cnt   = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient IDLE bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
        }
    }
    
    return (result);
}


/**
@name: tdm_td3_chk_bandwidth_lr_pipe
@param:

Check bandwidth for linerate ports for the given pipe
 */
int 
tdm_td3_chk_bandwidth_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int port_speed, slot_req, slot_cnt=0, result=PASS;
    int port_phy, phy_lo, phy_hi;
    int param_phy_lo, param_phy_hi;

    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    tdm_td3_cmn_get_pipe_port_range(cal_id, &phy_lo, &phy_hi);
    if (phy_lo>=param_phy_lo && phy_hi<=param_phy_hi) {
        for (port_phy=phy_lo; port_phy<=phy_hi; port_phy++) {
            port_speed = tdm_td3_cmn_get_port_speed(_tdm, port_phy);
            if (BOOL_TRUE == tdm_td3_cmn_chk_port_is_lr(_tdm, port_phy)) {
                slot_req = tdm_td3_cmn_get_speed_slots(port_speed);
                slot_cnt = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, port_phy);
                if (slot_cnt<slot_req){
                    result = FAIL;
                    TDM_ERROR7("%s %s, cal_id %d, port %d, speed %dG, slots %d/%d\n",
                       "[Linerate Bandwidth]",
                       "insufficient port bandwidth",
                       cal_id, port_phy, (port_speed/1000), slot_cnt, slot_req);
                }
            }
        }
    }

    return (result);
}


/**
@name: tdm_td3_chk_bandwidth_os_pipe
@param:

Check bandwidth for Oversub ports for the given pipe
 */
int 
tdm_td3_chk_bandwidth_os_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, port_speed, port_phy, phy_lo, phy_hi,
        grp_speed, grp_speed_slots, port_speed_slots,
        port_is_found=BOOL_FALSE, result=PASS;
    int param_phy_lo, param_phy_hi, 
        param_cal_grp_num, param_cal_grp_len;
    int **param_cal_grp;
    
    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    
    switch(cal_id){
        case TD3_IDB_PIPE_0_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_0.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_0.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_0.cal_grp;
            break;
        case TD3_IDB_PIPE_1_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_1.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_1.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_1.cal_grp;
            break;
        case TD3_IDB_PIPE_2_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_2.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_2.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_2.cal_grp;
            break;
        case TD3_IDB_PIPE_3_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_3.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_3.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_3.cal_grp;
            break;
        case TD3_MMU_PIPE_0_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_4.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_4.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_4.cal_grp;
            break;
        case TD3_MMU_PIPE_1_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_5.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_5.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_5.cal_grp;
            break;
        case TD3_MMU_PIPE_2_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_6.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_6.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_6.cal_grp;
            break;
        case TD3_MMU_PIPE_3_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_7.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_7.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_7.cal_grp;
            break;
        default:
            param_cal_grp_num = 0;
            param_cal_grp_len = 0;
            param_cal_grp = NULL;
            break;
    }
    
    tdm_td3_cmn_get_pipe_port_range(cal_id, &phy_lo, &phy_hi);
    if (phy_lo>=param_phy_lo && phy_hi<=param_phy_hi) {
        /* check port subscription */
        for (port_phy=phy_lo; port_phy<=phy_hi; port_phy++) {
            if (BOOL_TRUE == tdm_td3_cmn_chk_port_is_os(_tdm, port_phy)) {
                port_speed = tdm_td3_cmn_get_port_speed(_tdm, port_phy);
                port_is_found = BOOL_FALSE;
                for (i=0; i<param_cal_grp_num; i++){
                    for (j=0; j<param_cal_grp_len; j++) {
                        if (port_phy==param_cal_grp[i][j]) {
                            port_is_found = BOOL_TRUE;
                            break;
                        }
                    }
                    if (port_is_found) {
                        break;
                    }
                }
                if (port_is_found==BOOL_FALSE){
                    result = FAIL;
                    TDM_ERROR5("%s %s, cal_id %d, port %d, speed %dG\n",
                       "[Oversub Bandwidth]",
                       "Unfounded Oversub port",
                       cal_id, port_phy, (port_speed/1000));
                }
            }
        }
        /* check if all ports within the same ovsb group have the same speed */
        for (i=0; i<param_cal_grp_num; i++) {
            port_phy = param_cal_grp[i][0];
            grp_speed = 0;
            grp_speed_slots = 0;
            for (j=1; j<param_cal_grp_len; j++){
                port_phy = param_cal_grp[i][j];
                if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
                    port_speed = tdm_td3_cmn_get_port_speed(_tdm, port_phy);
                    if (grp_speed == SPEED_0) {
                        grp_speed = port_speed;
                        grp_speed_slots = tdm_td3_cmn_get_speed_slots(grp_speed);
                    }
                    if ( (grp_speed==SPEED_20G  || grp_speed==SPEED_40G) && 
                         (port_speed==SPEED_20G || port_speed==SPEED_40G)&&
                         (grp_speed != port_speed) ) {
                        TDM_PRINT5("TDM: Group port %d (%dG) with port %d (%dG) in the speed group %d\n",
                                  param_cal_grp[i][0], (grp_speed/1000),
                                  port_phy, (port_speed/1000),
                                  i);
                    } else if ( (grp_speed==SPEED_25G  || grp_speed==SPEED_50G) && 
                                (port_speed==SPEED_25G || port_speed==SPEED_50G)&&
                                (grp_speed != port_speed) ) {
                        TDM_PRINT5("TDM: Group port %d (%dG) with port %d (%dG) in the speed group %d\n",
                                  param_cal_grp[i][0], (grp_speed/1000),
                                  port_phy, (port_speed/1000),
                                  i);
                    } else {
                        port_speed_slots = tdm_td3_cmn_get_speed_slots(port_speed);
                        if (grp_speed_slots != port_speed_slots) {
                            result = FAIL;
                            TDM_ERROR6("%s %s, ovsb_grp %d, grp_speed %dG, port %d, port_speed %dG\n",
                               "[Oversub Bandwidth]",
                               "invalid OVSB speed group",
                               i, (grp_speed/1000), port_phy, (port_speed/1000));
                        }
                    }
                }
            }
        }
        /* check oversub ratio */
    }
    
    return (result);
}


/**
@name: tdm_td3_chk_bandwidth_lr
@param:

Check bandwidth for linerate ports in MMU/IDB calendars
(Chip-Specific)
 */
int 
tdm_td3_chk_bandwidth_lr(tdm_mod_t *_tdm)
{
    int result=PASS;
    
    /* IDB pipe 0 */
    if (tdm_td3_chk_bandwidth_lr_pipe(_tdm, TD3_IDB_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* IDB pipe 1 */
    if (tdm_td3_chk_bandwidth_lr_pipe(_tdm, TD3_IDB_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* MMU pipe 0 */
    if ( tdm_td3_chk_bandwidth_lr_pipe(_tdm, TD3_MMU_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* MMU pipe 1 */
    if (tdm_td3_chk_bandwidth_lr_pipe(_tdm, TD3_MMU_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }
    
    return result;
}


/**
@name: tdm_td3_chk_bandwidth_os
@param:

Check bandwidth for linerate ports in MMU/IDB calendars
(Chip-Specific)
 */
int 
tdm_td3_chk_bandwidth_os(tdm_mod_t *_tdm)
{
    int result=PASS;
    
    /* IDB pipe 0 */
    if (tdm_td3_chk_bandwidth_os_pipe(_tdm, TD3_IDB_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* IDB pipe 1 */
    if (tdm_td3_chk_bandwidth_os_pipe(_tdm, TD3_IDB_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }
    
    return result;
}


/**
@name: tdm_td3_chk_bandwidth
@param:

Verifies IDB/MMU calendars allocating enough slots for port bandwidth
 */
int 
tdm_td3_chk_bandwidth(tdm_mod_t *_tdm)
{
    int result=PASS;
    
    /* Ancillary ports */
    if (tdm_td3_chk_bandwidth_ancl(_tdm)!=PASS){
        result = FAIL;
    }
    /* Linerate ports */
    if(tdm_td3_chk_bandwidth_lr(_tdm)!=PASS){
        result = FAIL;
    }
    /* Oversub ports */
    if(tdm_td3_chk_bandwidth_os(_tdm)!=PASS){
        result = FAIL;
    }
    
    return result;
}


/**
@name: tdm_td3_chk_jitter_cmic
@param:

Checks distribution of cmic port for the given pipe
 */
int
tdm_td3_chk_jitter_cmic(tdm_mod_t *_tdm, int cal_id)
{
	int i, j, space_min, space_max, cmic_num, port_speed, result = PASS;
	int param_cal_len;
    int *param_cal_main = NULL;
    
    param_cal_len = tdm_td3_cmn_get_pipe_cal_len(_tdm, cal_id);
	TDM_SEL_CAL(cal_id, param_cal_main);
	cmic_num = tdm_td3_chk_get_pipe_token_cnt(_tdm, cal_id, TD3_CMIC_TOKEN);
	switch(cmic_num) {
		case 4: 
			port_speed = SPEED_10G;
			break;
		case 8:
			port_speed = SPEED_20G;
			break;
		default:
            port_speed = SPEED_10G;
			break;
	}
    tdm_td3_cmn_get_speed_jitter(port_speed, param_cal_len, &space_min, &space_max);
	for(i = 0; i < param_cal_len; i++) {
		if(param_cal_main[i] != TD3_CMIC_TOKEN) {
            continue;
        }
		for(j=1; j< param_cal_len;j++) {
			if(param_cal_main[(i+j)%param_cal_len] == TD3_CMIC_TOKEN) {
				if(j <= space_max && j>= space_min) {
                    break;
                } else {
                    TDM_PRINT7("%s, cal_id %d, slot [%03d, %03d], space %d, [min, max] = [%d, %d]\n",
                              "TDM: _____WARNING: CMIC port jitter violation",
                              cal_id, i, (i+j)%param_cal_len, j,
                              space_min, space_max);
                    return FAIL;
                }
			}
		}
	}
	return result;
}


/**
@name: tdm_td3_chk_jitter_lr_pipe
@param:

Checks distribution of linerate slots of the same port for the given pipe
 */
int 
tdm_td3_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, k, port_phy, port_speed, port_space,
        space_max, space_min, pos_prev, pos_curr,
        result=PASS;
    int *port_exist=NULL;
    int param_cal_len, param_phy_lo, param_phy_hi;
    int *param_cal_main=NULL;
    enum port_speed_e *param_speeds;
    
    /* set parameters */
    param_cal_len = tdm_td3_cmn_get_pipe_cal_len(_tdm, cal_id);
    param_phy_lo  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds  = _tdm->_chip_data.soc_pkg.speed;
    TDM_SEL_CAL(cal_id, param_cal_main);
    
    /* initialize variables */
    port_exist = (int*) TDM_ALLOC(param_phy_hi*sizeof(int),
                                 "tdm_td3_chk, port_exist");
    for (i=0; i<param_phy_hi; i++) {
        port_exist[i] = BOOL_FALSE; 
    }
    
    /* check jitter for each linerate port in calendar */
    for (i=0; i<param_cal_len; i++) {
        port_phy = param_cal_main[i];
        if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
            port_speed = param_speeds[port_phy];
            if (port_exist[port_phy-1] == BOOL_FALSE &&
                port_speed>0) {
                tdm_td3_cmn_get_speed_jitter(port_speed, param_cal_len, &space_min, &space_max);
                pos_prev   = i;
                for (k=1; k<param_cal_len; k++) {
                    pos_curr = (i+k)%param_cal_len;
                    if (param_cal_main[pos_curr] == port_phy) {
                        port_space = (pos_curr+param_cal_len-pos_prev)%param_cal_len;
                        if (port_space > space_max) {
                            if (port_speed<SPEED_10G) { /* Low speed port: 10G, 20G, 25G */
                                TDM_PRINT8("%s, calendar %d, port %d, speed %dG, slot [%03d,%03d], %d > %d\n",
                                   "TDM: WARNING [Linerate Jitter (MAX)]",
                                   cal_id,
                                   port_phy,
                                   (port_speed/1000),
                                   pos_prev,
                                   pos_curr,
                                   port_space,
                                   space_max);
                            } else { /* High speed port: 40G, 50G */
                                if (port_space > (space_max+TD3_LR_CHK_TRHD)) {
                                    result = FAIL;
                                    TDM_ERROR8("%s, calendar %d, port %d, speed %dG, slot [%03d,%03d], %d > %d\n",
                                       "[Linerate Jitter (MAX)]",
                                       cal_id,
                                       port_phy,
                                       (port_speed/1000),
                                       pos_prev,
                                       pos_curr,
                                       port_space,
                                       space_max);
                                } else {
                                    /* result = FAIL; */
                                    TDM_PRINT8("%s, calendar %d, port %d, speed %dG, slot [%03d,%03d], %d > %d\n",
                                       "TDM: WARNING: [Linerate Jitter (MAX)]",
                                       cal_id,
                                       port_phy,
                                       (port_speed/1000),
                                       pos_prev,
                                       pos_curr,
                                       port_space,
                                       space_max);
                                }
                            }
                        }
                        if (port_space < space_min) {
                            /* result = FAIL; */
                            TDM_PRINT8("%s, calendar %d, port %d, speed %dG, slot [%03d,%03d], %d < %d\n",
                               "TDM: VERBOSE [Linerate Jitter (MIN)]",
                               cal_id,
                               port_phy,
                               (port_speed/1000),
                               pos_prev,
                               pos_curr,
                               port_space,
                               space_min);
                        }
                        pos_prev = pos_curr;
                    }
                }
                port_exist[port_phy-1] = BOOL_TRUE;
            }
        }
    }
    
    TDM_FREE(port_exist);
    
    return (result);
}


/**
@name: tdm_td3_chk_jitter_lr
@param:

Checks distribution of linerate tokens for IDB/MMU calendars
 */
int 
tdm_td3_chk_jitter_lr(tdm_mod_t *_tdm)
{
    int result=PASS;
    
    /* IDB pipe 0 */
    if (tdm_td3_chk_jitter_lr_pipe(_tdm, TD3_IDB_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* IDB pipe 1 */
    if (tdm_td3_chk_jitter_lr_pipe(_tdm, TD3_IDB_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* MMU pipe 0 */
    if (tdm_td3_chk_jitter_lr_pipe(_tdm, TD3_MMU_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* MMU pipe 1 */
    if (tdm_td3_chk_jitter_lr_pipe(_tdm, TD3_MMU_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }

    return (result);
}


/**
@name: tdm_td3_chk_os_halfpipe
@param:

Check oversub Half Pipes (only for IDB)
 */
int
tdm_td3_chk_os_halfpipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, hp0_bw=0, hp1_bw=0, hp0_port_cnt=0, hp1_port_cnt=0,
        hp0_pm_cnt=0, hp1_pm_cnt=0, bw_diff, bw_diff_max,
        port_phy, port_speed, port_pm, port_hp_id, result=PASS;
    int *pm_2_hp_map=NULL, *pm_2_spd_grp_map=NULL;
    int param_hp_pm_max,
        param_phy_lo, param_phy_hi,
        param_pm_num, param_token_ext,
        param_cal_grp_num, param_cal_grp_len;
    int **param_cal_grp=NULL;
    enum port_speed_e *param_speeds=NULL;

    /* set parameters */
    param_hp_pm_max = 8; /* Maximum number of PMs in one Half-Pipe */
    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_num    = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_token_ext = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;
    
    switch(cal_id){
        case TD3_IDB_PIPE_0_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_0.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_0.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_0.cal_grp;
            break;
        case TD3_IDB_PIPE_1_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_1.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_1.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_1.cal_grp;
            break;
        case TD3_IDB_PIPE_2_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_2.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_2.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_2.cal_grp;
            break;
        case TD3_IDB_PIPE_3_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_3.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_3.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_3.cal_grp;
            break;
        default:
            param_cal_grp_num = 0;
            param_cal_grp_len = 0;
            param_cal_grp = NULL;
            break;
    }

    /* initialize variables */
    pm_2_hp_map = (int*) TDM_ALLOC(param_pm_num*sizeof(int),
                                   "tdm_td3_chk, pm_2_hp_map");
    pm_2_spd_grp_map = (int*) TDM_ALLOC(param_pm_num*sizeof(int),
                                        "tdm_td3_chk, pm_2_spd_grp_map");
    for (i=0; i<param_pm_num; i++) {
        pm_2_hp_map[i] = param_token_ext;
        pm_2_spd_grp_map[i] = param_token_ext;
    }
    for (i=0; i<param_cal_grp_num; i++) {
        for (j=0; j<param_cal_grp_len; j++) {
            port_phy = param_cal_grp[i][j];
            if (port_phy>=param_phy_lo && port_phy<=param_phy_lo) {
                port_speed = param_speeds[port_phy];
                if (i<(param_cal_grp_num/2)) {
                    hp0_port_cnt++;
                    hp0_bw += port_speed;
                } else {
                    hp1_port_cnt++;
                    hp1_bw += port_speed;
                }
                
            }
        }
    }
    /* check half-pipe constraints */
    if (hp0_port_cnt==0 && hp1_port_cnt==0) {
        TDM_PRINT1("TDM: Not applicable on calendar %d, pipe has no OVSB ports \n", cal_id);
    } else {
        /* check bandwidth balance constraint: Tolerate 50G BW gap between Half Pipes */
        bw_diff     = 0;
        bw_diff_max = SPEED_100G;
        if (hp0_bw>hp1_bw && (hp0_bw-hp1_bw)>bw_diff_max) {
            bw_diff = hp0_bw - hp1_bw;
        } else if (hp1_bw>hp0_bw && (hp1_bw-hp0_bw)>bw_diff_max) {
            bw_diff = hp1_bw - hp0_bw;
        }
        if (bw_diff>bw_diff_max) {
            result = FAIL;
            TDM_ERROR5("%s, calendar %d, HP0_BW=%dG,  HP1_BW=%dG (bw_diff %dG)\n",
                       "[Half Pipes] Unbalanced Bandwidth",
                       cal_id, (hp0_bw/1000), (hp1_bw/1000), (bw_diff/1000));
        }
        /* check PM constraint: All subports of any PM are in the same Half Pipe */
        for (i=0; i<param_cal_grp_num; i++) {
            port_hp_id = (i<(param_cal_grp_num/2)) ? (0) : (1);
            for (j=0; j<param_cal_grp_len; j++) {
                port_phy = param_cal_grp[i][j];
                if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
                    port_pm = tdm_td3_cmn_get_port_pm(_tdm, port_phy);
                    if (port_pm<param_pm_num) {
                        if (pm_2_hp_map[port_pm] == param_token_ext) {
                            pm_2_hp_map[port_pm] = port_hp_id;
                        } else if (pm_2_hp_map[port_pm] != port_hp_id) {
                            result = FAIL;
                            TDM_ERROR7("%s, calendar %d, ovsb_grp %2d, port %3d, PM %2d, HP %d (expected HP %d)\n",
                                      "[Half Pipes] Subports within the same PM are in different Half Pipes",
                                      cal_id, i,
                                      port_phy, port_pm,
                                      port_hp_id, pm_2_hp_map[port_pm]);
                        }
                    } else {
                        result = FAIL;
                        TDM_ERROR4("%s, port %d, PM_idx %d (PM_idx_max %d)\n",
                                  "[ Half-Pipe] invalid PM number",
                                  port_phy, port_pm, (param_pm_num-1));
                    }
                }
            }
        }
        /* check PM constraint: Total number of PMs allocated to each Half Pipe doesn't exceed 8 */
        for (port_pm=0; port_pm<param_pm_num; port_pm++) {
            switch (pm_2_hp_map[port_pm]) {
                case 0: hp0_pm_cnt++; break;
                case 1: hp1_pm_cnt++; break;
                default: break;
            }
        }
        if (hp0_pm_cnt > param_hp_pm_max) {
            result = FAIL;
            TDM_ERROR5("%s, calendar %d, half-pipe %d, PM_num %d (PM_num_max %d)",
                      "[Half Pipes] number of PMs exceeded",
                      cal_id, 0, hp0_pm_cnt, param_hp_pm_max);
        } else if (hp1_pm_cnt > param_hp_pm_max) {
            result = FAIL;
            TDM_ERROR5("%s, calendar %d, half-pipe %d, PM_num %d (PM_num_max %d)",
                      "[Half Pipes] number of PMs exceeded",
                      cal_id, 1, hp1_pm_cnt, param_hp_pm_max);
        }
        /* Additional checks
           -- Same speed groups within a PIPE are balanced
           -- Same speed ports from a PM are all assigned to the same group
        */
    }

    TDM_FREE(pm_2_hp_map);
    TDM_FREE(pm_2_spd_grp_map);
    return (result);
}


/**
@name: tdm_td3_chk_pkt_sched_print
@param:

Print out PKT scheduler calender
 */
int
tdm_td3_chk_pkt_sched_print(tdm_mod_t *_tdm, int cal_id, int hpipe_id)
{
    int i, j, k, x, y, port_phy, port_space, slot_cnt, 
        hp_id, result=PASS;
    int dist_max[TD3_NUM_PHY_PORTS_PER_SPIPE],
        dist_min[TD3_NUM_PHY_PORTS_PER_SPIPE],
        dist_port[TD3_NUM_PHY_PORTS_PER_SPIPE][TD3_NUM_PKT_SLOTS_PER_PM];
    int port_cnt = 0, port_buff[TD3_NUM_PHY_PORTS_PER_SPIPE];
    int max_tmp, min_tmp,
        max_10g = 0, max_20g = 0, max_25g = 0,
        max_40g = 0, max_50g = 0, max_100g = 0,
        min_10g = TD3_SHAPING_GRP_LEN, min_20g = TD3_SHAPING_GRP_LEN,
        min_25g = TD3_SHAPING_GRP_LEN, min_40g = TD3_SHAPING_GRP_LEN,
        min_50g = TD3_SHAPING_GRP_LEN, min_100g= TD3_SHAPING_GRP_LEN;
    int param_phy_lo, param_phy_hi,
        param_cal_grp_num, param_cal_grp_len;
    int *param_pkt_cal;
    int **param_cal_grp=NULL;
    enum port_speed_e *param_speeds=NULL;

    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;

    for (i = 0; i < TD3_NUM_PHY_PORTS_PER_SPIPE; i++) {
        port_buff[i]= TD3_NUM_EXT_PORTS;
        dist_max[i] = 0;
        dist_min[i] = TD3_SHAPING_GRP_LEN;
        for (j = 0; j < TD3_NUM_PKT_SLOTS_PER_PM; j++) {
            dist_port[i][j] = 0;
        }
    }

    /* get TDM calendars */
    switch(cal_id){
        case TD3_IDB_PIPE_0_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_0.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_0.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_0.cal_grp;
            break;
        case TD3_IDB_PIPE_1_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_1.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_1.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_1.cal_grp;
            break;
        default:
            return (UNDEF);
            break;
    }
    switch (hpipe_id) {
        case 0: param_pkt_cal = param_cal_grp[TD3_SHAPING_GRP_IDX_0]; break;
        case 1: param_pkt_cal = param_cal_grp[TD3_SHAPING_GRP_IDX_1]; break;
        default: return (UNDEF); break;
    }
    /* construct port array for current halfpipe */
    for (i=0; i<param_cal_grp_num; i++) {
        hp_id = i / (param_cal_grp_num/2);
        if (hp_id != hpipe_id) {
            continue;
        }
        for (j=0; j<param_cal_grp_len; j++){
            port_phy = param_cal_grp[i][j];
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (port_cnt < TD3_NUM_PHY_PORTS_PER_SPIPE) {
                    port_buff[port_cnt++] = port_phy;
                }
            }
        }
    }
    /* construct slot distance array for each port */
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        slot_cnt = 0;
        port_space = 0;
        for (j=0; j <TD3_SHAPING_GRP_LEN; j++) {
            if (port_phy == param_pkt_cal[j]) {
                port_space = 0;
                for (x = 0; x < TD3_SHAPING_GRP_LEN; x++) {
                    y = (x + j + 1) % TD3_SHAPING_GRP_LEN;
                    if (param_pkt_cal[y] == TD3_NUM_EXT_PORTS) {
                        continue;
                    } else if (param_pkt_cal[y] == port_phy) {
                        dist_port[i][slot_cnt] = port_space;
                        if (dist_max[i] < port_space) {
                            dist_max[i] = port_space;
                        }
                        if (dist_min[i] > port_space) {
                            dist_min[i] = port_space;
                        }
                        slot_cnt = (slot_cnt + 1) % TD3_NUM_PKT_SLOTS_PER_PM;
                        break;
                    } else {
                        port_space++;
                    }
                }
            }
        }
    }
    /* calculate max/min for each speed */
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        max_tmp = dist_max[i];
        min_tmp = dist_min[i];
        switch (param_speeds[port_phy]) {
            case SPEED_10G: case SPEED_11G:
                max_10g = (max_10g > max_tmp) ? (max_10g) : (max_tmp);
                min_10g = (min_10g < min_tmp) ? (min_10g) : (min_tmp);
                break;
            case SPEED_20G: case SPEED_21G:
                max_20g = (max_20g > max_tmp) ? (max_20g) : (max_tmp);
                min_20g = (min_20g < min_tmp) ? (min_20g) : (min_tmp);
                break;
            case SPEED_25G: case SPEED_27G:
                max_25g = (max_25g > max_tmp) ? (max_25g) : (max_tmp);
                min_25g = (min_25g < min_tmp) ? (min_25g) : (min_tmp);
                break;
            case SPEED_40G: case SPEED_42G:
                max_40g = (max_40g > max_tmp) ? (max_40g) : (max_tmp);
                min_40g = (min_40g < min_tmp) ? (min_40g) : (min_tmp);
                break;
            case SPEED_50G: case SPEED_53G:
                max_50g = (max_50g > max_tmp) ? (max_50g) : (max_tmp);
                min_50g = (min_50g < min_tmp) ? (min_50g) : (min_tmp);
                break;
            case SPEED_100G: case SPEED_106G:
                max_100g = (max_100g > max_tmp) ? (max_100g) : (max_tmp);
                min_100g = (min_100g < min_tmp) ? (min_100g) : (min_tmp);
                break;
            default:
                break;
        }
    }

    /* print slot spacing for each port */
    TDM_SML_BAR
    TDM_PRINT2("cal_id %d, halfpipe_id %d\n", cal_id, hpipe_id);
    TDM_SML_BAR
    TDM_PRINT5("%4s %4s %4s %4s %4s\n", "port", "spd", "max", "min", "diff");
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        TDM_PRINT5("%4d %3dG %4d %4d %4d  [",
               port_phy,
               param_speeds[port_phy]/1000,
               dist_max[i],
               dist_min[i],
               dist_max[i] - dist_min[i]);
        for (k = 0; k < TD3_NUM_PKT_SLOTS_PER_PM; k++) {
            if (dist_port[i][k] == 0) {
                break;
            }
            TDM_PRINT1("%2d,", dist_port[i][k]);
        }
        TDM_PRINT0("]\n");
    }
    TDM_PRINT4("\n%4s%12s%12s%12s\n", "spd", "dist_min", "dist_max", "diff");
    if (min_10g == TD3_SHAPING_GRP_LEN && max_10g == 0) {
        TDM_PRINT4("%4s%12s%12s%12s\n", "10G", "--", "--", "--");
    } else {
        TDM_PRINT4("%4s%12d%12d%12d\n", "10G", min_10g, max_10g, max_10g - min_10g);
    }
    if (min_20g == TD3_SHAPING_GRP_LEN && max_20g == 0) {
        TDM_PRINT4("%4s%12s%12s%12s\n", "20G", "--", "--", "--");
    } else {
        TDM_PRINT4("%4s%12d%12d%12d\n", "20G", min_20g, max_20g, max_20g - min_20g);
    }
    if (min_25g == TD3_SHAPING_GRP_LEN && max_25g == 0) {
        TDM_PRINT4("%4s%12s%12s%12s\n", "25G", "--", "--", "--");
    } else {
        TDM_PRINT4("%4s%12d%12d%12d\n", "25G", min_25g, max_25g, max_25g - min_25g);
    }
    if (min_40g == TD3_SHAPING_GRP_LEN && max_40g == 0) {
        TDM_PRINT4("%4s%12s%12s%12s\n", "40G", "--", "--", "--");
    } else {
        TDM_PRINT4("%4s%12d%12d%12d\n", "40G", min_40g, max_40g, max_40g - min_40g);
    }
    if (min_50g == TD3_SHAPING_GRP_LEN && max_50g == 0) {
        TDM_PRINT4("%4s%12s%12s%12s\n", "50G", "--", "--", "--");
    } else {
        TDM_PRINT4("%4s%12d%12d%12d\n", "50G", min_50g, max_50g, max_50g - min_50g);
    }
    if (min_100g == TD3_SHAPING_GRP_LEN && max_100g == 0) {
        TDM_PRINT4("%4s%12s%12s%12s\n", "100G", "--", "--", "--");
    } else {
        TDM_PRINT4("%4s%12d%12d%12d\n", "100G", min_100g, max_100g, max_100g - min_100g);
    }

    TDM_SML_BAR

    return (result);
}


/**
@name: tdm_td3_chk_pkt_sched
@param:

Checks same port jitter for PKT scheduler calender
 */
int
tdm_td3_chk_pkt_sched(tdm_mod_t *_tdm, int cal_id, int hpipe_id)
{
	int i, j, k, port_phy, port_speed, slot_req, slot_cnt,
        hp_id, result=PASS;
    int dist_max[TD3_NUM_PKT_SLOTS_PER_PM], dist_min[TD3_NUM_PKT_SLOTS_PER_PM];
    int x, y, port_space, pkt_jitter, slot_pos_base, slot_pos,
        port_lanes, pos_step;
    int port_cnt = 0, port_buff[TD3_NUM_PKT_SLOTS_PER_PM];
    int param_phy_lo, param_phy_hi,
        param_cal_grp_num, param_cal_grp_len;
    int *param_pkt_cal;
    int **param_cal_grp=NULL;
    enum port_speed_e *param_speeds=NULL;
	
    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;
    
    for (i = 0; i < TD3_NUM_PKT_SLOTS_PER_PM; i++) {
        dist_max[i] = 0;
        dist_min[i] = TD3_SHAPING_GRP_LEN;
        port_buff[i]= TD3_NUM_EXT_PORTS;
    }

	/* get TDM calendars */
    switch(cal_id){
        case TD3_IDB_PIPE_0_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_0.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_0.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_0.cal_grp;
            break;
        case TD3_IDB_PIPE_1_CAL_ID: 
            param_cal_grp_num = _tdm->_chip_data.cal_1.grp_num;
            param_cal_grp_len = _tdm->_chip_data.cal_1.grp_len;
            param_cal_grp     = _tdm->_chip_data.cal_1.cal_grp;
            break;
        default:
            TDM_ERROR2("%s, invalid cal_id %d\n",
                       "[Packet Scheduler jitter]",
                       cal_id);
            return (UNDEF);
            break;
    }
    switch (hpipe_id) {
        case 0: param_pkt_cal = param_cal_grp[TD3_SHAPING_GRP_IDX_0]; break;
        case 1: param_pkt_cal = param_cal_grp[TD3_SHAPING_GRP_IDX_1]; break;
        default:
            TDM_ERROR3("%s, cal_id %d, invalid half-pipe number %d\n",
                       "[Packet Scheduler jitter]",
                       cal_id, hpipe_id);
            return (UNDEF);
            break;
    }
    /* construct port array for current halfpipe */
    for (i=0; i<param_cal_grp_num; i++) {
        hp_id = i / (param_cal_grp_num/2);
        if (hp_id != hpipe_id) {
            continue;
        }
        for (j=0; j<param_cal_grp_len; j++){
            port_phy = param_cal_grp[i][j];
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (port_cnt < TD3_NUM_PKT_SLOTS_PER_PM) {
                    port_buff[port_cnt++] = port_phy;
                }
            }
        }
    }
    /* construct slot distance array for each port */
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        slot_cnt = 0;
        port_space = 0;
        for (j=0; j <TD3_SHAPING_GRP_LEN; j++) {
            if (port_phy == param_pkt_cal[j]) {
                port_space = 0;
                for (x = 0; x < TD3_SHAPING_GRP_LEN; x++) {
                    y = (x + j + 1) % TD3_SHAPING_GRP_LEN;
                    if (param_pkt_cal[y] == TD3_NUM_EXT_PORTS) {
                        continue;
                    } else if (param_pkt_cal[y] == port_phy) {
                        if (dist_max[i] < port_space) {
                            dist_max[i] = port_space;
                        }
                        if (dist_min[i] > port_space) {
                            dist_min[i] = port_space;
                        }
                        slot_cnt = (slot_cnt + 1) % TD3_NUM_PKT_SLOTS_PER_PM;
                        break;
                    } else {
                        port_space++;
                    }
                }
            }
        }
    }
    /* check port bandwidth */
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        port_speed   = param_speeds[port_phy];
        slot_req = tdm_td3_chk_get_speed_slots_5G(port_speed);
        slot_cnt = 0;
        for (k=0; k <TD3_SHAPING_GRP_LEN; k++) {
            if (port_phy == param_pkt_cal[k]) {
                slot_cnt++;
            }
        }
        if ( slot_req != slot_cnt) {
            TDM_ERROR7("%s, calendar %d, half-pipe %d, port %3d, speed %dG, slot_act=%d, slot_exp=%d\n",
                       "[Packet Scheduler], insufficient bandwidth",
                       cal_id, hpipe_id, port_phy, (port_speed/1000),
                       slot_cnt, slot_req);
            result = FAIL;
        }
    }
    /* check slot postion (fixed position) */
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        slot_pos_base = 0;
        port_speed = param_speeds[port_phy];
        port_lanes = tdm_td3_chk_get_port_lanes(_tdm, port_phy);
        switch (port_lanes) {
            case 1: pos_step = 32; break;
            case 2: pos_step = 16; break;
            case 4: pos_step = 8;  break;
            default:pos_step = 8;  break;
        }
        for (j = 0; j < TD3_SHAPING_GRP_LEN; j++) {
            if (port_phy == param_pkt_cal[j]) {
                slot_pos_base = j % pos_step;
                break;
            }
        }
        for (k=0; k <TD3_SHAPING_GRP_LEN/8; k++) {
            /* quad mode (lanes == 1) */
            if (port_lanes == 1 && k % 4 != 0) {
                continue;
            }
            /* dual mode (lanes == 2 */
            else if (port_lanes == 2 && k % 2 != 0) {
                continue;
            }
            /* single mode (lanes == 4) */
            else if (port_lanes == 4) {
                /* keep empty */
            }
            slot_pos = (slot_pos_base + k * 8) % TD3_SHAPING_GRP_LEN;
            if(param_pkt_cal[slot_pos] != port_phy &&
               param_pkt_cal[slot_pos] != TD3_NUM_EXT_PORTS) {
                TDM_ERROR6("%s, port %0d, speed %0dG, lanes %0d, cal[%d] = %0d \n",
                           "[Packet Sched] Invalid slot_pos",
                           port_phy, port_speed/1000, port_lanes,
                           slot_pos, param_pkt_cal[slot_pos]);
                result = FAIL;
            }
        }
    }
    /* check port jitter (space deviation) */
    for (i = 0; i < port_cnt; i++) {
        port_phy = port_buff[i];
        switch (param_speeds[port_phy]) {
            case SPEED_10G:  case SPEED_11G:  pkt_jitter = 22; break;
            case SPEED_20G:  case SPEED_21G:  pkt_jitter = 22; break;
            case SPEED_25G:  case SPEED_27G:  pkt_jitter = 22; break;
            case SPEED_40G:  case SPEED_42G:  pkt_jitter = 2;  break;
            case SPEED_50G:  case SPEED_53G:  pkt_jitter = 2;  break;
            case SPEED_100G: case SPEED_106G: pkt_jitter = 2;  break;
            default: pkt_jitter = 22;  break;
        }
        if (dist_max[i] - dist_min[i] > pkt_jitter) {
/*             TDM_PRINT5("%s port %3d, speed %dG, jitter/limit %d/%d\n",
                   "WARNING: [pkt_sched_jitter]",
                   port_phy, param_speeds[port_phy]/1000,
                   dist_max[i] - dist_min[i],
                   pkt_jitter); */
        }
    }

    /* check flexport constraints */
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        int port_state, port_done[TD3_NUM_EXT_PORTS];
        int grp_offset;
        int **param_cal_grp_prev = NULL;
        int *param_pkt_cal_prev = NULL;
        enum port_state_e *param_states_prev = NULL;

        param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;
        if (cal_id == TD3_IDB_PIPE_0_CAL_ID) {
            param_cal_grp_prev= _tdm->_prev_chip_data.cal_0.cal_grp;
        } else if (cal_id == TD3_IDB_PIPE_1_CAL_ID) {
            param_cal_grp_prev= _tdm->_prev_chip_data.cal_1.cal_grp;
        }
        if (param_cal_grp_prev != NULL) {
            grp_offset = (hpipe_id == 0) ? TD3_SHAPING_GRP_IDX_0 :
                                           TD3_SHAPING_GRP_IDX_1;
            param_pkt_cal_prev = param_cal_grp_prev[grp_offset];
        }
        if (param_pkt_cal_prev != NULL && param_states_prev != NULL) {
            TDM_MEMSET(port_done, 0, sizeof(int)*TD3_NUM_EXT_PORTS);
            for (i = 0; i < TD3_SHAPING_GRP_LEN; i++) {
                port_phy = param_pkt_cal[i];
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                    if (port_done[port_phy % TD3_NUM_EXT_PORTS]) continue;
                    port_state = param_states_prev[port_phy - 1];
                    if (port_state != PORT_STATE__FLEX_OVERSUB_UP &&
                        port_state != PORT_STATE__FLEX_OVERSUB_DN &&
                        port_state != PORT_STATE__FLEX_OVERSUB_CH &&
                        param_pkt_cal_prev[i] != param_pkt_cal[i]) {
                        result = FAIL;
                        port_done[port_phy % TD3_NUM_EXT_PORTS] = 1;
                        TDM_ERROR3("TDM: %s, port %0d, speed %0dG\n",
                            "[Packet Sched] Invalid slot_pos of unchanged port",
                            port_phy, param_speeds[port_phy]/1000);
                    }
                }
            }
        }
    }

    /* print out space info of each port for packet scheduler */
    tdm_td3_chk_pkt_sched_print(_tdm, cal_id, hpipe_id);
    
	return (result);
}


/**
@name: tdm_td3_chk
@param: 
 */
int
tdm_td3_chk(tdm_mod_t *_tdm)
{
    int result=PASS;

    TDM_BIG_BAR	
    TDM_SML_BAR

    /* check structure */
    TDM_PRINT0("TDM: Checking Structure (speed, state, frequency, length)\n\n");
    if (tdm_td3_chk_struct(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check port transcription */
    TDM_PRINT0("TDM: Checking Port Transcription\n\n"); 
    if (tdm_td3_chk_transcription(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check sister port spacing */
    TDM_PRINT0("TDM: Checking Sister-Port Spacing\n\n");
    if (tdm_td3_chk_sister(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check same port spacing */
    TDM_PRINT0("TDM: Checking Same-Port Spacing\n\n");
    if (tdm_td3_chk_same(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check port subscription */
    TDM_PRINT0("TDM: Checking Port Subscription\n\n");
    if (tdm_td3_chk_bandwidth(_tdm) != PASS) {
         result = FAIL;
    }
    TDM_SML_BAR

    /* check linerate jitter */
    TDM_PRINT0("TDM: Checking Linerate Jitter\n\n");
    if (tdm_td3_chk_jitter_lr(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check cmic jitter */
    TDM_PRINT0("TDM: Checking CMIC Jitter\n\n");
    if(tdm_td3_chk_jitter_cmic(_tdm, 0) != PASS){
        TDM_WARN0("CMIC port jitter constraint is violated\n");
    }
    TDM_SML_BAR
    /* check oversub half-pipes */
    TDM_PRINT0("TDM: Checking Half-Pipe constraints\n\n");
    if (tdm_td3_chk_os_halfpipe(_tdm, TD3_IDB_PIPE_0_CAL_ID) != PASS ||
        tdm_td3_chk_os_halfpipe(_tdm, TD3_IDB_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check packet scheduler: bandwidth, position and jitter */
    TDM_PRINT0("\nTDM: Checking Pkt scheduler: bandwidth, position, jitter \n");
    if (tdm_td3_chk_pkt_sched(_tdm, TD3_IDB_PIPE_0_CAL_ID, 0) != PASS ||
        tdm_td3_chk_pkt_sched(_tdm, TD3_IDB_PIPE_0_CAL_ID, 1) != PASS ||
        tdm_td3_chk_pkt_sched(_tdm, TD3_IDB_PIPE_1_CAL_ID, 0) != PASS ||
        tdm_td3_chk_pkt_sched(_tdm, TD3_IDB_PIPE_1_CAL_ID, 1) != PASS ) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* summarize check results */
    TDM_PRINT0("\n");
    TDM_SML_BAR
    if (result == FAIL ) {
        TDM_PRINT0("TDM: *** FAILED ***\n");
    } else {
        TDM_PRINT0("TDM: *** PASSED ***\n");
    }
    TDM_SML_BAR
    TDM_PRINT0("TDM: TDM Self Check is completed.\n");
    TDM_BIG_BAR	

    return (result);
}
