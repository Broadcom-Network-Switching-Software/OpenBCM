/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip main functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


enum port_speed_e original_speeds[FL_NUM_EXT_PORTS];
enum port_state_e original_states[FL_NUM_EXT_PORTS];

/**
@name: tdm_fl_main_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_fl_main_corereq( tdm_mod_t *_tdm )
{
    _tdm->_core_data.vars_pkg.cal_id = _tdm->_core_data.vars_pkg.pipe;

    return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}

/**
@name: tdm_fl_main_ingress_pipe
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_fl_main_ingress_pipe( tdm_mod_t *_tdm )
{
    int iter, idx1=0, port_phy, result=PASS;
    int param_cal_len, param_ancl_num,
        param_pipe_id,
        param_port_start, param_port_end,
        param_token_empty;
    int *param_lr_buff, *param_os_buff;
    enum port_state_e *param_states;

    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_port_start  = _tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_start;
    param_port_end    = _tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_end;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_pipe_id     = _tdm->_core_data.vars_pkg.pipe;
    param_lr_buff     = _tdm->_core_data.vars_pkg.lr_buffer;
    param_os_buff     = _tdm->_core_data.vars_pkg.os_buffer;
    param_states      = _tdm->_chip_data.soc_pkg.state;

    param_cal_len     = tdm_fl_cmn_get_pipe_cal_len(param_pipe_id, _tdm);
    result = (param_cal_len > 0) ? result : FAIL ;

    tdm_fl_print_pipe_config(_tdm);
    for (iter = 0; iter < TDM_AUX_SIZE; iter++) {
        param_lr_buff[iter] = param_token_empty;
        param_os_buff[iter] = param_token_empty;
    }
    /* construct lr_buffer and os_buffer */
    for (port_phy = param_port_start; port_phy <= param_port_end; port_phy++) {
        if ( param_states[port_phy-1] == PORT_STATE__LINERATE   ||
             param_states[port_phy-1] == PORT_STATE__LINERATE_HG ) {
            if (idx1<TDM_AUX_SIZE){
                param_lr_buff[idx1++] = port_phy;
            } else {
                result = FAIL;
                TDM_PRINT3("%s, pipe %d, port %d may have been skipped.\n",
                           "WARNING: Line-Rate buffer overflow",
                           param_pipe_id,
                           port_phy);
            }
        }
    }

    /* set parameter */
    _tdm->_chip_data.soc_pkg.lr_idx_limit = param_cal_len - param_ancl_num;

    if (result != PASS) {
        return result;
    }
    return (_tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ](_tdm));
}

/**
@name: tdm_fl_main_ingress
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_fl_main_ingress( tdm_mod_t *_tdm )
{
    int pipe_idx, pipe_port_start, pipe_port_end, result=PASS;
    int param_phy_lo, param_phy_hi;

    /* TDM_PRINT_STACK_SIZE("tdm_fl_main_ingress"); */

    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (pipe_idx = 0; pipe_idx < FL_NUM_PIPE; pipe_idx++){
        pipe_port_start = pipe_idx*FL_NUM_PHY_PORTS_PER_PIPE + 1;
        pipe_port_end   = (pipe_idx+1)*FL_NUM_PHY_PORTS_PER_PIPE;
        if (pipe_port_start >= param_phy_lo && pipe_port_end <= param_phy_hi) {
            _tdm->_core_data.vars_pkg.pipe                   = pipe_idx;
            _tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_start = pipe_port_start;
            _tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_end   = pipe_port_end;
            if (tdm_fl_main_ingress_pipe(_tdm) != PASS) {
                result = FAIL;
            }
        } else {
            TDM_ERROR3("Pipe %d, invalid port numbering [%0d, %0d]\n",
                       pipe_idx, pipe_port_start, pipe_port_end);
        }
    }

    return result;
}

/**
@name: tdm_fl_main_spcl_store_cfg
@param:
 */
static void
tdm_fl_main_spcl_store_cfg(tdm_mod_t *_tdm)
{
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int p, param_phy_hi;

    TDM_MEMSET(original_speeds, 0, sizeof(enum port_speed_e)*FL_NUM_EXT_PORTS);
    TDM_MEMSET(original_states, 0, sizeof(enum port_speed_e)*FL_NUM_EXT_PORTS);

    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (p = 0; p <= param_phy_hi + 1 && p < FL_NUM_EXT_PORTS; p++) {
        original_speeds[p] = param_speeds[p];
        original_states[p] = param_states[p];
    }
}

/**
@name: tdm_fl_main_spcl_restore_cfg
@param:
 */
static void
tdm_fl_main_spcl_restore_cfg(tdm_mod_t *_tdm)
{
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int p, param_phy_hi;

    TDM_PRINT0("TDM: restore speed/state config\n");
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (p = 0; p <= param_phy_hi + 1 && p < FL_NUM_EXT_PORTS; p++) {
        param_speeds[p] = original_speeds[p];
        param_states[p] = original_states[p];
    }
}

/**
@name: tdm_fl_main_spcl_map_cfg_phy2pphy
@param:

Map fp port numbering from phy to speudo_phy.
 */
static void
tdm_fl_main_spcl_map_cfg_phy2pphy(tdm_mod_t *_tdm)
{
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int p, param_phy_hi;

    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (p = 1; p <= param_phy_hi + 1; p++) {
        if (p >= FL_PHY_PORT_NUMBER_OFFSET) {
            param_speeds[p - FL_PHY_PORT_NUMBER_OFFSET] = param_speeds[p];
            param_states[p - FL_PHY_PORT_NUMBER_OFFSET] = param_states[p];
        }
    }
}

/**
@name: tdm_fl_main_spcl_aggregate_cfg_gport
@param:

Aggregate Gports to signle pseudo port.
 */
static void
tdm_fl_main_spcl_aggregate_cfg_gport(tdm_mod_t *_tdm)
{
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int p;

    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;

    for (p = FL_GPORT_PSEUDO_PHY_LO; p <= FL_GPORT_PSEUDO_PHY_HI;
         p+= FL_GPORT_PSEUDO_SUBP_PER_PM) {
        int active_subp_cnt = 0;
        int tmp_port_state = 0;
        int subp, pport;
        for (subp = 0; subp < FL_GPORT_PSEUDO_SUBP_PER_PM; subp++) {
            pport = p + subp;
            if (param_speeds[pport] > 0) {
                active_subp_cnt++;
                if (tmp_port_state == 0)
                    tmp_port_state = param_states[pport - 1];
                param_speeds[pport] = 0;
                param_states[pport - 1] = 0;
            }
        }
        if (active_subp_cnt > 0) {
            if (active_subp_cnt <= FL_GPORT_PSEUDO_SUBP_PER_PM / 2) {
                param_speeds[p] = SPEED_20G;
            } else {
                param_speeds[p] = SPEED_40G;
            }
            param_states[p-1] = tmp_port_state;
            TDM_PRINT2("Aggregate pseudo_pport %0d as %0dG\n", p,
                       param_speeds[p]/1000);
        }
    }
}

/**
@name: tdm_fl_main_spcl_segregate_cal_gport_per_pipe
@param:

De-aggregate single Gport to subports.
 */
static int
tdm_fl_main_spcl_segregate_cal_gport_per_pipe(tdm_mod_t *_tdm, int cal_id,
                                              int port)
{
    int param_cal_len;
    int *param_cal_main;
    enum port_speed_e *param_speeds;
    int subp_list[FL_GPORT_PSEUDO_SUBP_PER_PM];

    TDM_SEL_CAL(cal_id, param_cal_main);
    param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    if (param_cal_main != NULL && param_cal_len > 0) {
        int i, cnt = 0;
        TDM_PRINT2("TDM: cal_id %0d, de-aggregate port %0d\n", cal_id, port);

        if (param_speeds[port] == SPEED_40G) {
            int gport_0 = port;
            int gport_1 = port + FL_GPORT_PSEUDO_SUBP_PER_PM / 2;
            for (i = 0; i < param_cal_len; i++) {
                if (param_cal_main[i] == port) {
                    param_cal_main[i] = (cnt % 2 == 0) ? (gport_0++):
                                                         (gport_1++);
                    cnt++;
                    TDM_PRINT4("port %0d, %2d-th slot, idx #%03d, subp %0d\n",
                                port, cnt, i, param_cal_main[i]);
                }
            }
        }
        else {
            int subp, pport;
            for (subp = 0; subp < FL_GPORT_PSEUDO_SUBP_PER_PM; subp++) {
                pport = port + subp;
                subp_list[subp] = 0;
                if (original_speeds[pport + FL_PHY_PORT_NUMBER_OFFSET] > 0)
                    subp_list[subp] = pport;
            }
            subp = 0;
            for (i = 0; i < param_cal_len; i++) {
                if (param_cal_main[i] == port) {
                    while(subp < FL_GPORT_PSEUDO_SUBP_PER_PM &&
                          subp_list[subp] == 0) {
                        subp++;
                    }
                    if (subp < FL_GPORT_PSEUDO_SUBP_PER_PM) {
                        param_cal_main[i] = subp_list[subp];
                        cnt++;
                        TDM_PRINT4("port %0d, %2d-th slot, idx #%03d, subp %0d\n",
                                    port, cnt, i, param_cal_main[i]);
			subp++;
                    }
                    else {
                        break;
                    }
                }
            }
        }
    }
    return PASS;
}

/**
@name: tdm_fl_main_spcl_segregate_cal_gport
@param:

De-aggregate single Gport to subports.
 */
static void
tdm_fl_main_spcl_segregate_cal_gport(tdm_mod_t *_tdm)
{
    enum port_speed_e *param_speeds;
    int p, cal_id, cal_len, special_gport_en = 0;

    TDM_PRINT0("TDM: de-aggregate gport\n");
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    for (p = FL_GPORT_PSEUDO_PHY_LO; p <= FL_GPORT_PSEUDO_PHY_HI;
         p += FL_GPORT_PSEUDO_SUBP_PER_PM) {
        if (param_speeds[p] > 0) {
            special_gport_en = 1;
            break;
        }
    }

    if (special_gport_en == 1) {
        for (cal_id = 0; cal_id < 8; cal_id++) {
            cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
            if (cal_len > 0) {
                TDM_PRINT1("TDM: de-aggregate gport for cal_id %0d", cal_id);
                for (p = FL_GPORT_PSEUDO_PHY_LO; p <= FL_GPORT_PSEUDO_PHY_HI;
                     p += FL_GPORT_PSEUDO_SUBP_PER_PM) {
                    if (param_speeds[p] > 0) {
                        tdm_fl_main_spcl_segregate_cal_gport_per_pipe(
                                                        _tdm, cal_id, p);
                    }
                }
            }
        }
    }
}

/**
@name: tdm_fl_main_spcl_map_cal_pphy2phy
@param:

For all TDM calendars, map pseudo phy_port number to actual phy_port number.
 */
static int
tdm_fl_main_spcl_map_cal_pphy2phy(tdm_mod_t *_tdm)
{
    int cal_id, i;
    int param_cal_len, *param_cal_main, param_phy_lo, param_phy_hi;

    TDM_PRINT0("TDM: Map pseudo physical port to actual physical port\n");
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    for (cal_id = 0; cal_id < 8; cal_id++) {
        TDM_SEL_CAL(cal_id, param_cal_main);
        param_cal_len = tdm_fl_cmn_get_pipe_cal_len(cal_id, _tdm);
        if (param_cal_main != NULL && param_cal_len > 0) {
            for (i = 0; i < param_cal_len; i++) {
                if (param_cal_main[i] >= param_phy_lo &&
                    param_cal_main[i] <= param_phy_hi) {
                    param_cal_main[i] += FL_PHY_PORT_NUMBER_OFFSET;
                }
            }
        }

    }
    return PASS;
}

/**
@name: tdm_fl_main_spcl_process_pre
@param:

Special handler for FL.
 */
static void
tdm_fl_main_spcl_process_pre(tdm_mod_t *_tdm)
{
    tdm_fl_main_spcl_store_cfg(_tdm);
    tdm_fl_main_spcl_map_cfg_phy2pphy(_tdm);
    tdm_fl_main_spcl_aggregate_cfg_gport(_tdm);
}

/**
@name: tdm_fl_main_spcl_process_post
@param:

Special handler for FL.
 */
static void
tdm_fl_main_spcl_process_post(tdm_mod_t *_tdm)
{
    tdm_fl_main_spcl_segregate_cal_gport(_tdm);
    tdm_fl_main_spcl_map_cal_pphy2phy(_tdm);
    tdm_fl_main_spcl_restore_cfg(_tdm);
}

/**
@name: tdm_fl_main_transcription
@param:

Transcription algorithm for generating port module mapping
 */
int
tdm_fl_main_transcription( tdm_mod_t *_tdm )
{
    int i, j, port_phy, port_lanenum, last_port;
    int **param_pmap, param_pmap_wid, param_pmap_len,
        param_token_empty,
        param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int param_hg_en;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pmap_wid    = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pmap_len    = _tdm->_chip_data.soc_pkg.pmap_num_modules;
    param_pmap        = _tdm->_chip_data.soc_pkg.pmap;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;
    param_hg_en       = _tdm->_chip_data.soc_pkg.soc_vars.fl.cal_hg_en;

    /* initialize pmap */
    for (i=0; i<param_pmap_len; i++) {
        for (j=0; j<param_pmap_wid; j++) {
            param_pmap[i][j] = param_token_empty;
        }
    }

    /* FL special handler */
    tdm_fl_main_spcl_process_pre(_tdm);

    /* check validity of input config */
    if (tdm_fl_main_chk(_tdm) != PASS) return FAIL;

    /* set combination state */
    for (i=1; i<=param_phy_hi; i++) {
        if (param_speeds[i]>=SPEED_40G) {
            switch(param_speeds[i]){
                case SPEED_106G:
                case SPEED_100G:
                    port_lanenum = 4;
                    break;
                case SPEED_53G :
                case SPEED_50G :
                case SPEED_42G_HG2 :
                case SPEED_42G :
                case SPEED_40G :
                    port_lanenum = 2;
                    break;
                default:
                    port_lanenum = 0;
                    break;
            }
            for (j=1; j<port_lanenum && (i+j)<=param_phy_hi; j++) {
                param_states[i+j-1] = PORT_STATE__COMBINE;
            }
        }
        else if ( param_speeds[i]==SPEED_20G || param_speeds[i]==SPEED_21G ) {
            if (param_speeds[i+1]==SPEED_0 && (i+1)<=param_phy_hi ){
                param_states[i] = PORT_STATE__DISABLED;
            }
        }
    }
    /* set pmap */
    last_port= param_token_empty;
    for (i=0; i<param_pmap_len; i++) {
        for (j=0; j<param_pmap_wid; j++) {
            port_phy = i*param_pmap_wid+1+j;
            if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
                switch (param_states[port_phy-1]) {
                    case PORT_STATE__LINERATE:
                    case PORT_STATE__OVERSUB:
                    case PORT_STATE__LINERATE_HG:
                    case PORT_STATE__OVERSUB_HG:
                        last_port = port_phy;
                        break;
                    case PORT_STATE__COMBINE:
                        break;
                    default:
                        last_port = param_token_empty;
                        break;
                }
                param_pmap[i][j] = last_port;
            }
        }
        port_phy = i*param_pmap_wid+1;
        if (port_phy>=param_phy_lo && (port_phy+3)<=param_phy_hi) {
            if (param_speeds[port_phy]>SPEED_0) {
                /* tri mode x_xx */
                if (param_speeds[port_phy]>param_speeds[port_phy+2] &&
                    param_speeds[port_phy+2]==param_speeds[port_phy+3] &&
                    param_speeds[port_phy+2]>SPEED_0 &&
                    param_speeds[port_phy]>=SPEED_40G) {
                    param_pmap[i][1] = param_pmap[i][2];
                    param_pmap[i][2] = param_pmap[i][0];
                }
                /* tri mode xxx_ */
                else if (param_speeds[port_phy]==param_speeds[port_phy+1] &&
                         param_speeds[port_phy]<param_speeds[port_phy+2]  &&
                         param_speeds[port_phy]!=SPEED_0 &&
                         param_speeds[port_phy+2]>=SPEED_40G) {
                    param_pmap[i][2] = param_pmap[i][1];
                    param_pmap[i][1] = param_pmap[i][3];
                }
                /* dual mode x_x_ */
                else if (param_speeds[port_phy]!=param_speeds[port_phy+1] &&
                         param_speeds[port_phy]==param_speeds[port_phy+2] &&
                         param_speeds[port_phy]>=SPEED_40G) {
                    param_pmap[i][1] = param_pmap[i][3];
                    param_pmap[i][2] = param_pmap[i][0];
                }
            }
        }
    }
    /* print port config after transcription */
    tdm_print_config(_tdm);
    if (param_hg_en == 0)
        TDM_PRINT2("cal_hg_en: %0d -> %s.\n", param_hg_en, "Ethernet");
    else
        TDM_PRINT2("cal_hg_en: %0d -> %s.\n", param_hg_en, "Higig");

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP](_tdm) );
}


/**
@name: tdm_fl_main_init
@param:
 */
int
tdm_fl_main_init( tdm_mod_t *_tdm )
{
    int index;

    /* initialize chip/core parameters */
    _tdm->_chip_data.soc_pkg.pmap_num_modules = FL_NUM_PM_MOD;
    _tdm->_chip_data.soc_pkg.pmap_num_lanes = FL_NUM_PM_LNS;
    _tdm->_chip_data.soc_pkg.pm_num_phy_modules = FL_NUM_PHY_PM;

    _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = FL_OVSB_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl1_token = FL_IDL1_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl2_token = FL_IDL2_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.ancl_token = FL_ANCL_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = FL_NUM_PHY_PORTS;


    _tdm->_chip_data.cal_0.cal_len = FL_LR_VBS_LEN;
    _tdm->_chip_data.cal_0.grp_num = FL_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_0.grp_len = FL_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_1.cal_len = 0;
    _tdm->_chip_data.cal_1.grp_num = 0;
    _tdm->_chip_data.cal_1.grp_len = 0;
    _tdm->_chip_data.cal_2.cal_len = 0;
    _tdm->_chip_data.cal_2.grp_num = 0;
    _tdm->_chip_data.cal_2.grp_len = 0;
    _tdm->_chip_data.cal_3.cal_len = 0;
    _tdm->_chip_data.cal_3.grp_num = 0;
    _tdm->_chip_data.cal_3.grp_len = 0;
    _tdm->_chip_data.cal_4.cal_len = FL_LR_VBS_LEN;
    _tdm->_chip_data.cal_4.grp_num = 0;
    _tdm->_chip_data.cal_4.grp_len = 0;
    _tdm->_chip_data.cal_5.cal_len = 0;
    _tdm->_chip_data.cal_5.grp_num = 0;
    _tdm->_chip_data.cal_5.grp_len = 0;
    _tdm->_chip_data.cal_6.cal_len = 0;
    _tdm->_chip_data.cal_6.grp_num = 0;
    _tdm->_chip_data.cal_6.grp_len = 0;
    _tdm->_chip_data.cal_7.cal_len = 0;
    _tdm->_chip_data.cal_7.grp_num = 0;
    _tdm->_chip_data.cal_7.grp_len = 0;

    _tdm->_chip_data.soc_pkg.lr_idx_limit           = FL_LEN_688MHZ_EN;
    _tdm->_chip_data.soc_pkg.tvec_size              = FL_NUM_ANCL;
    _tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_start = 1;
    _tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_end   = FL_NUM_PHY_PORTS_PER_PIPE;

    _tdm->_core_data.vars_pkg.pipe       = 0;
    _tdm->_core_data.rule__same_port_min = FL_MIN_SPACING_SAME_PORT_HSP;
    _tdm->_core_data.rule__prox_port_min = 0; /* FL_MIN_SPACING_SISTER_PORT; */
    _tdm->_core_data.vmap_max_wid        = FL_VMAP_MAX_WID;
    _tdm->_core_data.vmap_max_len        = FL_VMAP_MAX_LEN;

    if (_tdm->_chip_data.soc_pkg.clk_freq == FL_CLK_667MHZ)
        _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_1G;
    else
        _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_2P5G;

    if (_tdm->_chip_data.soc_pkg.num_ext_ports != FL_NUM_EXT_PORTS) {
        TDM_WARN1("Invalid num_ext_port %d\n", _tdm->_chip_data.soc_pkg.num_ext_ports);
        _tdm->_chip_data.soc_pkg.num_ext_ports = FL_NUM_EXT_PORTS;
    }

    /* Chip: pmap */
    _tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
    for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
        _tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
        TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
    }

    /* Chip: IDB Pipe 0 calendar group */
    _tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
    _tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num+FL_SHAPING_GRP_NUM)*sizeof(int *), "TDM inst 0 groups");
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        _tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
    }

    /* Chip: IDB Pipe 1 calendar group */
    _tdm->_chip_data.cal_1.cal_main= NULL;
    _tdm->_chip_data.cal_1.cal_grp = NULL;

    /* Chip: IDB Pipe 2 calendar group */
    _tdm->_chip_data.cal_2.cal_main= NULL;
    _tdm->_chip_data.cal_2.cal_grp = NULL;

    /* Chip: IDB Pipe 3 calendar group */
    _tdm->_chip_data.cal_3.cal_main= NULL;
    _tdm->_chip_data.cal_3.cal_grp = NULL;

    /* Chip: MMU Pipe 0 calendar group */
    _tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 0 main calendar");
    TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);
    _tdm->_chip_data.cal_4.cal_grp = NULL;

    /* Chip: MMU Pipe 1 calendar group */
    _tdm->_chip_data.cal_5.cal_main= NULL;
    _tdm->_chip_data.cal_5.cal_grp = NULL;

    /* Chip: MMU Pipe 2 calendar group */
    _tdm->_chip_data.cal_6.cal_main= NULL;
    _tdm->_chip_data.cal_6.cal_grp = NULL;

    /* Chip: MMU Pipe 3 calendar group */
    _tdm->_chip_data.cal_7.cal_main= NULL;
    _tdm->_chip_data.cal_7.cal_grp = NULL;

    /* Core: VMAP */
    _tdm->_core_data.vmap=(unsigned short **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned short *), "vector_map_l1");
    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        _tdm->_core_data.vmap[index]=(unsigned short *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned short), "vector_map_l2");
    }

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION](_tdm) );
}

/**
@name: tdm_fl_main_chk
@param:
 */
int
tdm_fl_main_chk( tdm_mod_t *_tdm )
{
    int i, j, fp_bw, fp_bw_slots, ancl_bw_slots, max_bw_slots;
    int result = PASS;
    int param_phy_lo, param_phy_hi, param_pmap_wid;
    int slot_unit;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pmap_wid   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;
    param_states     = _tdm->_chip_data.soc_pkg.state;

    /* 1. check validity of input port config:
       -- If one PM is active, then the first lane must have speed > SPEED_0
       -- All ports are linerate.
    */
    for (i = param_phy_lo; i <= param_phy_hi; i += param_pmap_wid) {
        if (param_speeds[i] == SPEED_0) {
            for (j = 1; j < param_pmap_wid; j++) {
                if ((i+j) <= param_phy_hi) {
                    if (param_speeds[i+j] > SPEED_0) {
                        result = FAIL;
                        TDM_ERROR8("TDM: Invalid PM config, port [%3d, %3d, %3d, %3d], speed [%3dG, %3dG, %3dG, %3dG]\n",
                            i, i+1, i+2, i+3,
                            param_speeds[i]/1000,
                            param_speeds[i+1]/1000,
                            param_speeds[i+2]/1000,
                            param_speeds[i+3]/1000);
                    }
                }
            }
        }
    }
    for (i = 1; i <= param_phy_hi; i += param_pmap_wid) {
        if (param_speeds[i] > SPEED_0 &&
            param_states[i-1] != PORT_STATE__LINERATE) {
            result = FAIL;
            TDM_ERROR2("TDM: Invalid port state, pport %0d, state %0d (non_linerate)\n",
                      i, param_states[i - 1]);
        }
    }
    /* 2. check validity of input IO bandwidth:
       -- LineRate_bandwidth <= core_bandwidth
    */
    fp_bw = 0;
    for (i = param_phy_lo; i <= param_phy_hi; i ++) {
        if (param_speeds[i] > SPEED_0) {
            if (param_speeds[i] <= SPEED_2p5G)
                fp_bw += SPEED_2p5G;
            else
                fp_bw += param_speeds[i];
        }
    }
    slot_unit   = _tdm->_core_data.vars_pkg.pipe_info.slot_unit > 0 ?
                  _tdm->_core_data.vars_pkg.pipe_info.slot_unit : TDM_SLOT_UNIT_2P5G;
    fp_bw_slots = fp_bw / slot_unit;
    ancl_bw_slots = _tdm->_chip_data.soc_pkg.tvec_size;
    max_bw_slots = tdm_fl_cmn_get_pipe_cal_len(0, _tdm);
    if (fp_bw_slots + ancl_bw_slots > max_bw_slots) {
        int clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
        int max_fp_bw = (max_bw_slots - ancl_bw_slots) *  slot_unit;
        result = FAIL;
        TDM_ERROR5("%s, clk=%0dMHz, cal_len=%0d, max_fp_bw=%4.1fG, req_fp_bw=%4.1fG\n",
                  "TDM: Invalid config -- bandwidth overflow",
                  clk_freq, _tdm->_chip_data.soc_pkg.lr_idx_limit,
                  (max_fp_bw*1.0)/1000, (fp_bw*1.0)/1000);
    }

    return (result);
}

/**
@name: tdm_fl_main_post
@param:
 */
int
tdm_fl_main_post( tdm_mod_t *_tdm )
{
    /* TDM_PRINT_STACK_SIZE("tdm_fl_main_post"); */

    if (_tdm->_chip_data.soc_pkg.soc_vars.fl.pipe_end >=
        _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi){

        /* TDM self-check */
        if (_tdm->_chip_data.soc_pkg.soc_vars.fl.tdm_chk_en==BOOL_TRUE){
            _tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm);
        }

        /* FL chip special handler */
        tdm_fl_main_spcl_process_post(_tdm);

        /* print final linerate calendars */
        tdm_fl_cmn_print_pipe_lr_cal_matrix(_tdm, 0);
        tdm_fl_cmn_print_pipe_lr_cal_matrix(_tdm, 4);

        /* Generate MacSec TDM calendar */
        tdm_fl_main_spcl_map_cfg_phy2pphy(_tdm);
        tdm_fl_macsec_proc(_tdm);
        tdm_fl_main_spcl_restore_cfg(_tdm);
        tdm_fl_cmn_print_ms_cal_matrix(_tdm);

        TDM_SML_BAR
        TDM_PRINT0("\nTDM: TDM algorithm is completed.\n\n");
        TDM_SML_BAR
    }

    return (PASS);
}

/**
@name: tdm_fl_main_free
@param:
 */
int
tdm_fl_main_free( tdm_mod_t *_tdm )
{
    int index;
    TDM_SML_BAR
    TDM_PRINT0("Star free TDM internal memory \n");

    /* Chip: pmap */
    TDM_PRINT0("Free pmap\n");
    for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
        TDM_FREE(_tdm->_chip_data.soc_pkg.pmap[index]);
    }
    TDM_FREE(_tdm->_chip_data.soc_pkg.pmap);

    /* Chip: IDB Pipe 0 calendar group */
    TDM_PRINT0("Free IDB pipe 0\n");
    TDM_FREE(_tdm->_chip_data.cal_0.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp);

    /* Chip: MMU Pipe 0 calendar group */
    TDM_PRINT0("Free MMU pipe 0\n");
    TDM_FREE(_tdm->_chip_data.cal_4.cal_main);

    /* Core: vmap */
    TDM_PRINT0("Free CORE vmap\n");
    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        TDM_FREE(_tdm->_core_data.vmap[index]);
    }
    TDM_FREE(_tdm->_core_data.vmap);

    TDM_PRINT0("Finish free TDM internal memory \n");
    TDM_SML_BAR
    return (PASS);
}
