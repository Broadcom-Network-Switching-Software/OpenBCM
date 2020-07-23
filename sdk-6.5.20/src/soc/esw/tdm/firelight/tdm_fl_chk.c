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
@name: tdm_fl_chk_get_pipe_token_cnt
@param:

Return number of slots of the given token in selected calendar
 */
int
tdm_fl_chk_get_pipe_token_cnt(tdm_mod_t *_tdm, int cal_id, int port_token)
{
    int i, cnt=0;
    int param_cal_len;
    int *param_cal_main=NULL;

    param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
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
@name: tdm_fl_chk_get_port_lanes
@param:

Return number of lanes of the given port
 */
int
tdm_fl_chk_get_port_lanes(tdm_mod_t *_tdm, int port)
{
    int lanes = 0, port_start;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds=NULL;

    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;

    if (port >= param_phy_lo && port <= param_phy_hi) {
        switch (param_speeds[port]) {
            case SPEED_1G:   case SPEED_2p5G:  case SPEED_5G: lanes = 1; break;
            case SPEED_10G:  case SPEED_11G:  lanes = 1; break;
            case SPEED_20G:  case SPEED_21G:  lanes = 2; break;
            case SPEED_25G:  case SPEED_27G:  lanes = 1; break;
            case SPEED_40G:  case SPEED_42G:  lanes = 2; break;
            case SPEED_50G:  case SPEED_53G:  lanes = 2; break;
            case SPEED_100G: case SPEED_106G: lanes = 4; break;
            default: lanes = 0; break;
        }
        if (param_speeds[port] == SPEED_40G || param_speeds[port] == SPEED_42G) {
            port_start = ((port - 1) / FL_NUM_PM_LNS) * FL_NUM_PM_LNS + 1;
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
@name: tdm_fl_chk_struct
@param:

Checks frequency, calendar length, and speed/state type.
(Chip-Specific)
 */
int
tdm_fl_chk_struct(tdm_mod_t *_tdm)
{
    int i, cal_id, cal_len_limit=0, cal_len_act=0,
        port_phy, port_speed, port_state,
        result=PASS;
    int param_freq, param_phy_lo, param_phy_hi, param_token_ext;
    int *param_cal_main=NULL;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_freq      = _tdm->_chip_data.soc_pkg.clk_freq;
    param_phy_lo    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi    = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ext = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_speeds    = _tdm->_chip_data.soc_pkg.speed;
    param_states    = _tdm->_chip_data.soc_pkg.state;

    /* frequency */
    switch (param_freq) {
        case FL_CLK_850MHZ:
        case FL_CLK_680MHZ:
        case FL_CLK_667MHZ:
        case FL_CLK_219MHZ:
        case FL_CLK_313MHZ:
        case FL_CLK_375MHZ:
        case FL_CLK_438MHZ:
        case FL_CLK_500MHZ:
        case FL_CLK_625MHZ:
        case FL_CLK_688MHZ:
        case FL_CLK_700MHZ:
        case FL_CLK_250MHZ:
        case FL_CLK_282MHZ:
        case FL_CLK_344MHZ:
        case FL_CLK_407MHZ:
        case FL_CLK_469MHZ:
        case FL_CLK_532MHZ:
        case FL_CLK_563MHZ:
        case FL_CLK_594MHZ:
        case FL_CLK_719MHZ:
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
        if (_tdm->_chip_data.cal_1.cal_len == 0) continue;

        cal_len_act = tdm_fl_cmn_get_pipe_cal_len_max(cal_id, _tdm);
        cal_len_limit = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
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
        port_speed = param_speeds[port_phy];
        port_state = param_states[port_phy-1];
        if (port_speed>SPEED_0) {
            if ( port_state==PORT_STATE__LINERATE    ||
                 port_state==PORT_STATE__LINERATE_HG ||
                 port_state==PORT_STATE__OVERSUB     ||
                 port_state==PORT_STATE__OVERSUB_HG  ||
                 port_state==PORT_STATE__MANAGEMENT) {
                switch(port_speed){
                    case SPEED_1G:
                    case SPEED_2p5G:
                    case SPEED_5G:
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
                    port_phy, port_state);
            }

        }
    }

    return (result);
}


/**
@name: tdm_fl_chk_transcription
@param:

Verify Port Macro transcription caught all ports indexed by port_state array
 */
int
tdm_fl_chk_transcription(tdm_mod_t *_tdm)
{
    int i, port_phy, port_speed, port_state, port_tsc, port_lanes,
        port_lanes_exp, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_pm_lanes, param_pm_num;
    int **param_pmap;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_num   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pmap     = _tdm->_chip_data.soc_pkg.pmap;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;
    param_states   = _tdm->_chip_data.soc_pkg.state;

    /* check port transcription */
    for (port_phy=param_phy_lo; port_phy<=param_phy_hi; port_phy++) {
        port_speed = param_speeds[port_phy];
        port_state = param_states[port_phy-1];
        if ((port_speed > SPEED_0) &&
            (port_state == PORT_STATE__LINERATE    ||
             port_state == PORT_STATE__LINERATE_HG ||
             port_state == PORT_STATE__OVERSUB     ||
             port_state == PORT_STATE__OVERSUB_HG  ||
             port_state == PORT_STATE__MANAGEMENT ) ) {
            port_tsc = tdm_fl_cmn_get_port_pm(port_phy, _tdm);
            if (port_tsc<param_pm_num) {
                port_lanes_exp = tdm_fl_cmn_get_speed_lanes(port_speed);
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
@name: tdm_fl_chk_sister
@param:

Check sister port spacing constraint
 */
int
tdm_fl_chk_sister(tdm_mod_t *_tdm)
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
        param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
        for (i=0; i<param_cal_len; i++){
            port_i = param_cal_main[i];
            if (port_i>=param_phy_lo && port_i<=param_phy_hi) {
                tsc_i = tdm_fl_cmn_get_port_pm(port_i, _tdm);
                for (j=1; j<param_sister_min; j++){
                    k = (i+j)%param_cal_len;
                    port_k = param_cal_main[k];
                    if (port_k>=param_phy_lo && port_k<=param_phy_hi) {
                        tsc_k = tdm_fl_cmn_get_port_pm(port_k, _tdm);
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
@name: tdm_fl_chk_same (Version 2.0)
@param:

Check same port spacing constraint
 */
int
tdm_fl_chk_same(tdm_mod_t *_tdm)
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
        param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
        for (i=0; i<param_cal_len; i++) {
            port_i = param_cal_main[i];
            if (port_i>=param_phy_lo && port_i<=param_phy_hi) {
                port_speed = param_speeds[port_i];
                if((cal_id == FL_MMU_PIPE_0_CAL_ID ||
                    cal_id == FL_MMU_PIPE_1_CAL_ID) &&
                   (port_speed == SPEED_1G  ||
                    port_speed == SPEED_2p5G ||
                    port_speed == SPEED_5G  ||
                    port_speed == SPEED_10G ||
                    port_speed == SPEED_11G ||
                    port_speed == SPEED_25G ||
                    port_speed == SPEED_27G)) {
                    param_same_min = FL_MIN_SPACING_SAME_PORT_LSP;
                } else {
                    param_same_min = FL_MIN_SPACING_SAME_PORT_HSP;
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
@name: tdm_fl_chk_bandwidth_ancl
@param:

Check bandwidth for Ancillary port, i.e. CPU, Management, Loopback, PURGE, SBUBS, ect.
(Chip-Specific)
 */
int
tdm_fl_chk_bandwidth_ancl(tdm_mod_t *_tdm)
{
    int cal_id, slot_req, slot_cnt, slot_token,
        slot_cpu,  token_cpu,
        result=PASS;
    int param_token_cmic, param_token_ext;

    param_token_cmic= FL_CMIC_TOKEN;
    param_token_ext = _tdm->_chip_data.soc_pkg.num_ext_ports;

    for (cal_id=0; cal_id<8; cal_id++) {
        slot_cpu  = 0;
        token_cpu = param_token_ext;
        if (cal_id == FL_IDB_PIPE_0_CAL_ID ||
            cal_id == FL_MMU_PIPE_0_CAL_ID) {
            slot_cpu  = FL_NUM_ANCL_CPU;
            token_cpu = param_token_cmic;
        }
        /* CPU */
        if (slot_cpu > 0) {
            slot_token = token_cpu;
            slot_req   = slot_cpu;
            slot_cnt   = tdm_fl_chk_get_pipe_token_cnt(_tdm, cal_id, slot_token);
            if (slot_cnt<slot_req) {
                result = FAIL;
                TDM_ERROR5("%s, %s, cal_id %d, slot_required %d, slot_allocated %d\n",
                        "[Ancillary Bandwidth]",
                        "insufficient CPU bandwidth",
                        cal_id, slot_req, slot_cnt);
            }
        }
    }

    return (result);
}


/**
@name: tdm_fl_chk_bandwidth_lr_pipe
@param:

Check bandwidth for linerate ports for the given pipe
 */
int
tdm_fl_chk_bandwidth_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int port_speed, port_state, slot_req, slot_cnt=0,
        result=PASS;
    int port_phy, phy_lo, phy_hi;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;

    tdm_fl_cmn_get_pipe_port_range(cal_id, &phy_lo, &phy_hi);
    if (phy_lo>=param_phy_lo && phy_hi<=param_phy_hi) {
        for (port_phy=phy_lo; port_phy<=phy_hi; port_phy++) {
            port_state = param_states[port_phy-1];
            port_speed = param_speeds[port_phy];
            if (port_state==PORT_STATE__LINERATE    ||
                port_state==PORT_STATE__LINERATE_HG ){
                slot_req = tdm_fl_cmn_get_speed_slots(port_speed);
                slot_cnt = tdm_fl_chk_get_pipe_token_cnt(_tdm, cal_id, port_phy);
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
@name: tdm_fl_chk_bandwidth_lr
@param:

Check bandwidth for linerate ports in MMU/IDB calendars
(Chip-Specific)
 */
int
tdm_fl_chk_bandwidth_lr(tdm_mod_t *_tdm)
{
    int result=PASS;

    /* IDB pipe 0 */
    if (tdm_fl_chk_bandwidth_lr_pipe(_tdm, FL_IDB_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* IDB pipe 1 */
    if (tdm_fl_chk_bandwidth_lr_pipe(_tdm, FL_IDB_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* MMU pipe 0 */
    if ( tdm_fl_chk_bandwidth_lr_pipe(_tdm, FL_MMU_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }
    /* MMU pipe 1 */
    if (tdm_fl_chk_bandwidth_lr_pipe(_tdm, FL_MMU_PIPE_1_CAL_ID) != PASS) {
        result = FAIL;
    }

    return result;
}

/**
@name: tdm_fl_chk_bandwidth
@param:

Verifies IDB/MMU calendars allocating enough slots for port bandwidth
 */
int
tdm_fl_chk_bandwidth(tdm_mod_t *_tdm)
{
    int result=PASS;

    /* Ancillary ports */
    if (tdm_fl_chk_bandwidth_ancl(_tdm)!=PASS){
        result = FAIL;
    }
    /* Linerate ports */
    if(tdm_fl_chk_bandwidth_lr(_tdm)!=PASS){
        result = FAIL;
    }
    /* Oversub ports */

    return result;
}

/**
@name: tdm_fl_chk_jitter_cmic
@param:

Checks distribution of cmic port for the given pipe
 */
int
tdm_fl_chk_jitter_cmic(tdm_mod_t *_tdm, int cal_id)
{
	int i, j, space_min, space_max, cmic_num, port_speed, result = PASS;
	int param_cal_len;
    int *param_cal_main = NULL;

    param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
	TDM_SEL_CAL(cal_id, param_cal_main);
	cmic_num = tdm_fl_chk_get_pipe_token_cnt(_tdm, cal_id, FL_CMIC_TOKEN);
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
    tdm_fl_cmn_get_speed_jitter(port_speed, param_cal_len, &space_min, &space_max);
	for(i = 0; i < param_cal_len; i++) {
		if(param_cal_main[i] != FL_CMIC_TOKEN) {
            continue;
        }
		for(j=1; j< param_cal_len;j++) {
			if(param_cal_main[(i+j)%param_cal_len] == FL_CMIC_TOKEN) {
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
@name: tdm_fl_chk_jitter_lr_pipe
@param:

Checks distribution of linerate slots of the same port for the given pipe
 */
int
tdm_fl_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, k, port_phy, port_speed, port_space,
        space_max, space_min, pos_prev, pos_curr,
        result=PASS;
    int *port_exist=NULL;
    int param_cal_len, param_phy_lo, param_phy_hi;
    int *param_cal_main=NULL;
    enum port_speed_e *param_speeds;

    /* set parameters */
    param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
    param_phy_lo  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds  = _tdm->_chip_data.soc_pkg.speed;
    TDM_SEL_CAL(cal_id, param_cal_main);

    /* initialize variables */
    port_exist = (int*) TDM_ALLOC(param_phy_hi*sizeof(int),
                                 "tdm_fl_chk, port_exist");
    for (i=0; i<param_phy_hi; i++) {
        port_exist[i] = BOOL_FALSE;
    }

    /* check jitter for each linerate port in calendar */
    for (i=0; i<param_cal_len; i++) {
        port_phy = param_cal_main[i];
        if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
            port_speed = param_speeds[port_phy];

            /* ignore jitter check on aggregated ports */
            if (port_speed == SPEED_20G || port_speed == SPEED_40G) continue;

            if (port_exist[port_phy-1] == BOOL_FALSE &&
                port_speed>0) {
                tdm_fl_cmn_get_speed_jitter(port_speed, param_cal_len, &space_min, &space_max);
                pos_prev   = i;
                for (k=1; k<param_cal_len; k++) {
                    pos_curr = (i+k)%param_cal_len;
                    if (param_cal_main[pos_curr] == port_phy) {
                        port_space = (pos_curr+param_cal_len-pos_prev)%param_cal_len;
                        if (port_space > space_max) {
                            if (port_speed < SPEED_10G) { /* Low speed port: 10G, 20G */
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
                                if (port_space > (space_max+FL_LR_CHK_TRHD)) {
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
@name: tdm_fl_chk_jitter_lr
@param:

Checks distribution of linerate tokens for IDB/MMU calendars
 */
int
tdm_fl_chk_jitter_lr(tdm_mod_t *_tdm)
{
    int result=PASS;

    /* IDB pipe 0 */
    if (tdm_fl_chk_jitter_lr_pipe(_tdm, FL_IDB_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }

    /* MMU pipe 0 */
    if (tdm_fl_chk_jitter_lr_pipe(_tdm, FL_MMU_PIPE_0_CAL_ID) != PASS) {
        result = FAIL;
    }

    return (result);
}


/**
@name: tdm_fl_chk
@param:
 */
int
tdm_fl_chk(tdm_mod_t *_tdm)
{
    int result=PASS;

    TDM_BIG_BAR
    TDM_SML_BAR

    /* unit and dev_id */
    TDM_PRINT3("TDM: unit %0d, dev_id %0d / 0x%x\n",
               _tdm->_chip_data.soc_pkg.unit,
               _tdm->_chip_data.soc_pkg.dev_id,
               _tdm->_chip_data.soc_pkg.dev_id);

    /* check structure */
    TDM_PRINT0("TDM: Checking Structure (speed, state, frequency, length)\n\n");
    if (tdm_fl_chk_struct(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check port transcription */
    TDM_PRINT0("TDM: Checking Port Transcription\n\n");
    if (tdm_fl_chk_transcription(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check sister port spacing */
    TDM_PRINT0("TDM: Checking Sister-Port Spacing\n\n");
    if (tdm_fl_chk_sister(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check same port spacing */
    TDM_PRINT0("TDM: Checking Same-Port Spacing\n\n");
    if (tdm_fl_chk_same(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check port subscription */
    TDM_PRINT0("TDM: Checking Port Subscription\n\n");
    if (tdm_fl_chk_bandwidth(_tdm) != PASS) {
         result = FAIL;
    }
    TDM_SML_BAR

    /* check linerate jitter */
    TDM_PRINT0("TDM: Checking Linerate Jitter\n\n");
    if (tdm_fl_chk_jitter_lr(_tdm) != PASS) {
        result = FAIL;
    }
    TDM_SML_BAR

    /* check cmic jitter */
    TDM_PRINT0("TDM: Checking CMIC Jitter\n\n");
    if(tdm_fl_chk_jitter_cmic(_tdm, 0) != PASS){
        TDM_WARN0("CMIC port jitter constraint is violated\n");
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
