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


/**
@name: tdm_hx5_main_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_hx5_main_corereq( tdm_mod_t *_tdm )
{
    _tdm->_core_data.vars_pkg.cal_id = _tdm->_core_data.vars_pkg.pipe;

    return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}

/**
@name: tdm_hx5_main_ingress_pipe
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_hx5_main_ingress_pipe( tdm_mod_t *_tdm )
{
    int iter, idx1=0, idx2=0, port_phy, result=PASS;
    int hg_mgmt_en=BOOL_FALSE;  /* HX5 does not support HG management port */
    int param_cal_len, param_ancl_num, 
        param_pipe_id,
        param_port_start, param_port_end,
        param_token_empty;
    int *param_lr_buff, *param_os_buff;
    enum port_state_e *param_states;


    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_port_start  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_start;
    param_port_end    = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_end;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_pipe_id     = _tdm->_core_data.vars_pkg.pipe;
    param_lr_buff     = _tdm->_core_data.vars_pkg.lr_buffer;
    param_os_buff     = _tdm->_core_data.vars_pkg.os_buffer;
    param_states      = _tdm->_chip_data.soc_pkg.state;

    param_cal_len     = tdm_hx5_cmn_get_pipe_cal_len(param_pipe_id, _tdm);
    result = (param_cal_len > 0) ? result : FAIL ;

    tdm_hx5_print_pipe_config(_tdm);
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
        } else if ( (param_states[port_phy-1] == PORT_STATE__OVERSUB) ||
                    (param_states[port_phy-1] == PORT_STATE__OVERSUB_HG) ) {
            if (idx2<TDM_AUX_SIZE){
                param_os_buff[idx2++] = port_phy;
            } else {
                result = FAIL;
                TDM_PRINT3("%s, pipe %d, port %d may have been skipped.\n",
                           "WARNING: OverSub buffer overflow",
                           param_pipe_id,
                           port_phy);
            }
        }
    }

    /* set parameter */
    _tdm->_chip_data.soc_pkg.lr_idx_limit = param_cal_len - param_ancl_num;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.higig_mgmt = hg_mgmt_en;

    if (result != PASS) {
        return result;
    }
    return (_tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ](_tdm));
}

/**
@name: tdm_hx5_main_ingress
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_hx5_main_ingress( tdm_mod_t *_tdm )
{
    int pipe_idx, pipe_port_start, pipe_port_end, result=PASS;
    int param_phy_lo, param_phy_hi;

    /* TDM_PRINT_STACK_SIZE("tdm_hx5_main_ingress"); */

    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (pipe_idx = 0; pipe_idx < HX5_NUM_PIPE; pipe_idx++){
        pipe_port_start = pipe_idx*HX5_NUM_PHY_PORTS_PER_PIPE + 1;
        pipe_port_end   = (pipe_idx+1)*HX5_NUM_PHY_PORTS_PER_PIPE;
        if (pipe_port_start >= param_phy_lo && pipe_port_end <= param_phy_hi) {
            _tdm->_core_data.vars_pkg.pipe                   = pipe_idx;
            _tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_start = pipe_port_start;
            _tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_end   = pipe_port_end;
            if (tdm_hx5_main_ingress_pipe(_tdm) != PASS) {
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
 * @name: tdm_hx5_main_transcription_1g_10G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 10G in both A0 and B0.
 *        updated for 2.5 and 5 G speeds
 */
static void
tdm_hx5_main_transcription_1g_10g_LR(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_1g_exit, pm_10g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;
    int *param_1g_en = NULL;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_1g_en;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_1g_exit = BOOL_FALSE;
        pm_10g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
               /* if (param_speeds[port_phy] == SPEED_1G) { */
        	if ((param_speeds[port_phy] == SPEED_1G)||(param_speeds[port_phy] == SPEED_5G) ||(param_speeds[port_phy] == SPEED_2p5G) || (param_speeds[port_phy] == SPEED_2G) ){
                    pm_1g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_10G ||
                           param_speeds[port_phy] == SPEED_11G) {
                    pm_10g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_1g_exit == BOOL_TRUE && pm_10g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                /*    if (param_speeds[port_phy] == SPEED_1G ){ */
        	if ((param_speeds[port_phy] == SPEED_1G)||(param_speeds[port_phy] == SPEED_5G) ||(param_speeds[port_phy] == SPEED_2p5G) || (param_speeds[port_phy] == SPEED_2G) ){
                        param_speeds[port_phy] = SPEED_10G;
            		param_1g_en[port_phy - 1] = HX5_1G_PORT_STATE_REGULAR ;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 1G to 10G");
                    }
                }
            }
        }
    }
}

/**
 * @name: tdm_hx5_main_transcription_1g_5G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 5G in both A0 and B0.
 */
static void
tdm_hx5_main_transcription_1g_5g_LR(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_1g_exit, pm_5g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;
    int *param_1g_en = NULL;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_1g_en;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_1g_exit = BOOL_FALSE;
        pm_5g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
               /* if (param_speeds[port_phy] == SPEED_1G) { */
        	if ((param_speeds[port_phy] == SPEED_1G) || (param_speeds[port_phy] == SPEED_2p5G) || (param_speeds[port_phy] == SPEED_2G) ){
                    pm_1g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_5G ){
                    pm_5g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_1g_exit == BOOL_TRUE && pm_5g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                /*    if (param_speeds[port_phy] == SPEED_1G ){ */
        	if ((param_speeds[port_phy] == SPEED_1G) ||(param_speeds[port_phy] == SPEED_2p5G) || (param_speeds[port_phy] == SPEED_2G) ){
                        param_speeds[port_phy] = SPEED_5G;
            		param_1g_en[port_phy - 1] = HX5_1G_PORT_STATE_REGULAR ;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 1G to 10G");
                    }
                }
            }
        }
    }
}

/**
 * @name: tdm_hx5_main_transcription_1g_2G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 2G in both A0 and B0.
 */
static void
tdm_hx5_main_transcription_1g_2g_LR(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_1g_exit, pm_2g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;
    int *param_1g_en = NULL;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_1g_en;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_1g_exit = BOOL_FALSE;
        pm_2g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
               /* if (param_speeds[port_phy] == SPEED_1G) { */
        	if ((param_speeds[port_phy] == SPEED_1G)) {
                    pm_1g_exit = BOOL_TRUE;
                } else if ((param_speeds[port_phy] == SPEED_2G ) ||
                           (param_speeds[port_phy] == SPEED_2p5G)) 
		  
		  {
                    pm_2g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_1g_exit == BOOL_TRUE && pm_2g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                /*    if (param_speeds[port_phy] == SPEED_1G ){ */
        	if ((param_speeds[port_phy] == SPEED_1G) ){
                        param_speeds[port_phy] = SPEED_2p5G;
            		param_1g_en[port_phy - 1] = HX5_1G_PORT_STATE_REGULAR ;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 1G to 10G");
                    }
                }
            }
        }
    }
}


/**
 * @name: tdm_hx5_main_transcription_1g_25G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 10G in both A0 and B0.
 */
static void
tdm_hx5_main_transcription_1g_25g_LR(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_1g_exit, pm_25g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;
    int *param_1g_en = NULL;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_1g_en;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_1g_exit = BOOL_FALSE;
        pm_25g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (param_speeds[port_phy] == SPEED_1G) {
                    pm_1g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_25G ||
                           param_speeds[port_phy] == SPEED_27G) {
                    pm_25g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_1g_exit == BOOL_TRUE && pm_25g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                    if (param_speeds[port_phy] == SPEED_1G ){
            		param_1g_en[port_phy - 1] = HX5_1G_PORT_STATE_REGULAR ;
                        param_speeds[port_phy] = SPEED_25G;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 1G to 25G");
                    }
                }
            }
        }
    }
}


/**
 * @name: tdm_hx5_main_transcription_1g_10g
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 10G in both A0 and B0.
 *	  updated for 2.5 and 5G modes
 */
static void
tdm_hx5_main_transcription_1g_10g(tdm_mod_t *_tdm)
{
    int port_phy, port_speed, port_state, pm_num;
    int param_phy_lo, param_phy_hi, param_pm_lanes;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int *param_1g_en = NULL;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;
    param_states   = _tdm->_chip_data.soc_pkg.state;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_1g_en;

    for (port_phy=param_phy_lo; port_phy<=param_phy_hi; port_phy++) {
        port_speed = param_speeds[port_phy];
        port_state = param_states[port_phy - 1];
        if ((port_speed == SPEED_1G)||(port_speed == SPEED_5G) ||(port_speed == SPEED_2p5G) || (port_speed == SPEED_2G) )
            {
            if(port_state == PORT_STATE__OVERSUB ||
               port_state == PORT_STATE__OVERSUB_HG) {
                param_speeds[port_phy] = SPEED_10G;
            	param_1g_en[port_phy - 1] = HX5_1G_PORT_STATE_REGULAR ;
                pm_num = (port_phy - 1) / param_pm_lanes;
                TDM_PRINT5("%s PM %2d, Port %3d, %s %0dG to 10G\n",
                           "TDM: [SPECIAL_OVSB]",
                           pm_num, port_phy,
                           "Convert speed from",
                           (port_speed / 1000));
            }
        }
    }
}
/**
@name: tdm_hx5_main_transcription_16x1G
@param:
       -- _tdm: ptr to TDM global struct.
       -- flag: 0-> encode super PM from 16x1G to 1x40G;
                1-> decode super PM from 1x40G to 16x1G.

Encode/Decode PM4x10Q between 16x1G and 1x40G.
Encode/Decode PM4x10Q between 12x1G and 3x10G.
Encode/Decode PM4x10Q between 8x1G  and 2x10G.
Encode/Decode PM4x10Q between 4x1G  and 1x10G.

Note: A super PM consists of 16 ports within 4 consecutive PMs
configured as 16x1G, and suffurs the sister-port-spacing restriction.
 */
static int
tdm_hx5_main_transcription_16x1G(tdm_mod_t *_tdm, int flag)
{
    int n, port, port_phy, is_super_pm;
    int i,j,port_cnt,port_2g_cnt,param_token_empty,num_g_ports,num_1g_ports,num_2g_ports,cal_len, cal_id, *cal_main = NULL;
    int param_phy_lo, param_phy_hi;
    int is_2g = 0 ;
    int is_null_quad = 0 ;
    int num_2g_port_lane = 0 ;
    int *param_1g_en = NULL;
    int *port_num_1g_ports = NULL;
    int *port_num_2g_ports = NULL;
    int mix_mode_1g_2g = 0 ;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;


    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_1g_en;
    port_num_1g_ports  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_num_1g_ports;
    port_num_2g_ports  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.port_num_2g_ports;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;


    if (flag == 0) { /* encode super PM from 16x1G to 1x40G */
        for (port_phy = 0; port_phy < HX5_NUM_PHY_PORTS; port_phy++) {
            param_1g_en[port_phy] = HX5_1G_PORT_STATE_NONE;
            port_num_1g_ports[port_phy] = HX5_1G_PORT_STATE_NONE;
		port_num_1g_ports[port_phy] = 0 ;
		port_num_2g_ports[port_phy] = 0 ;
        }
        for (port_phy = param_phy_lo;
             port_phy <= HX5_NUM_PHY_GPORTS;
             port_phy += 16) {
            if (((param_speeds[port_phy] == SPEED_1G) || (param_speeds[port_phy] == SPEED_2p5G) ) &&
                (param_states[port_phy - 1] == PORT_STATE__LINERATE ||
                 param_states[port_phy - 1] == PORT_STATE__LINERATE_HG)) {
                is_super_pm = BOOL_TRUE;
		num_1g_ports = 0;
		num_2g_ports = 0;
                for (n = 0; n < 16; n++) {
                    port = port_phy + n;
                    TDM_PRINT2("%d, port_num %d, port_speed \n", port , param_speeds[port]);
                    if (port <= param_phy_hi && port < HX5_NUM_PHY_PORTS) {
                        if (param_speeds[port] > SPEED_2p5G) {
			   if (param_speeds[port] != SPEED_0) {
                              is_super_pm = BOOL_FALSE;
			      num_1g_ports = 0;
			      num_2g_ports = 0;
                             TDM_PRINT2("%d, no_super_pm %d port num \n", is_super_pm, port);
                            }
			    }
		       else
		          {
                        /*  TDM_PRINT2("%d, is_super_pm1 %d port num \n", is_super_pm, port); */
			   if (param_speeds[port] == SPEED_1G){
			   num_1g_ports = num_1g_ports + 1;}
			   else if ((param_speeds[port] == SPEED_2p5G) || (param_speeds[port] == SPEED_2G)) 
			   {
			   num_2g_ports = num_2g_ports + 1;
			   }
			  }
                    } else {
                        is_super_pm = BOOL_FALSE;
                    }
                }
                    TDM_PRINT2("%d, is_super_pm_at_ecode %d num_1g_ports_at_ecode \n", is_super_pm,num_1g_ports);
                    TDM_PRINT2("%d, is_super_pm_at_ecode %d num_2g_ports_at_ecode \n", is_super_pm,num_2g_ports);
                    
			 if( (num_2g_ports > 0) && (num_1g_ports > 1))
		     mix_mode_1g_2g = 1;
					if (num_1g_ports > 16 ) 
		    TDM_ERROR1("num_1g_ports greater than 16 %d", num_1g_ports);
                    if (num_2g_ports > 16 ) 
		    TDM_ERROR1("num_2g_ports greater than 16 %d", num_2g_ports);
		port_num_1g_ports[port_phy - 1] = num_1g_ports ;
		port_num_2g_ports[port_phy - 1] = num_2g_ports ;
		num_g_ports = num_1g_ports + num_2g_ports ;
                if (is_super_pm == BOOL_TRUE) {
                    for (n = 0; n < 16 ; n++) { /* recursive loop for setting tokens on first pm,
		    			        each quad speed is -> first pm 4 lanes */    	
                        port = port_phy + n;
			if( param_speeds[port] == SPEED_1G )
                           param_1g_en[port - 1] = HX5_1G_PORT_STATE_16x1G;
			else if( param_speeds[port] == SPEED_2p5G )
                           param_1g_en[port - 1] = HX5_1G_PORT_STATE_16x2G;

                        if (n == 0 ) {
			 if (num_g_ports >= 1){
			    if((param_speeds[port_phy] == SPEED_2p5G)|| (param_speeds[port_phy+1] == SPEED_2p5G)|| (param_speeds[port_phy+2] == SPEED_2p5G)||(param_speeds[port_phy+3] == SPEED_2p5G))
			    num_2g_port_lane = 1;
			    else
			    num_2g_port_lane = 0;

			    if((param_speeds[port_phy] == SPEED_0) && (param_speeds[port_phy+1] == SPEED_0 ) && (param_speeds[port_phy+2] == SPEED_0 ) && (param_speeds[port_phy+3] == SPEED_0 ))
			    is_null_quad = 1;
			    else
			    is_null_quad = 0;

			    if(is_null_quad == 1){
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
                           }
			   else if( mix_mode_1g_2g )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[port] == SPEED_1G && (num_2g_port_lane == 0) )
			     param_speeds[port] = SPEED_5G;
			    else if( param_speeds[port] == SPEED_1G && (num_2g_port_lane > 0) )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[port] == SPEED_2p5G )
			     param_speeds[port] = SPEED_10G;
			     }
			     }
			else if (n == 1 ) {
			 if (num_g_ports >= 1){ /* checking for second lane */
			    if((param_speeds[port_phy+4] == SPEED_2p5G)|| (param_speeds[port_phy+4+1] == SPEED_2p5G)|| (param_speeds[port_phy+4+2] == SPEED_2p5G)||(param_speeds[port_phy+4+3] == SPEED_2p5G))
			    num_2g_port_lane = 1;
			    else
			    num_2g_port_lane = 0;
			    if((param_speeds[port_phy+4] == SPEED_0) && (param_speeds[port_phy+4+1] == SPEED_0 ) && (param_speeds[port_phy+4+2] == SPEED_0 ) && (param_speeds[port_phy+4+3] == SPEED_0 ))
			    is_null_quad = 1;
			    else
			    is_null_quad = 0;
			    if(is_null_quad == 1){
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			    }
			   else if( mix_mode_1g_2g )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[(port_phy+4)] == SPEED_1G && (num_2g_port_lane == 0) )
			     param_speeds[port] = SPEED_5G;
			    else if( param_speeds[(port_phy+4)] == SPEED_1G && (num_2g_port_lane > 0) )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[(port_phy+4)] == SPEED_2p5G )
			     param_speeds[port] = SPEED_10G;
			     }
			 else{
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			  }
			     }
			else if (n == 2 ) {
			 if (num_g_ports >= 1){ /* checking for second lane */
			    if((param_speeds[port_phy+8] == SPEED_2p5G)|| (param_speeds[port_phy+8+1] == SPEED_2p5G)|| (param_speeds[port_phy+8+2] == SPEED_2p5G)||(param_speeds[port_phy+8+3] == SPEED_2p5G))
			    num_2g_port_lane = 1;
			    else
			    num_2g_port_lane = 0;
			    if((param_speeds[port_phy+8] == SPEED_0) && (param_speeds[port_phy+8+1] == SPEED_0 ) && (param_speeds[port_phy+8+2] == SPEED_0 ) && (param_speeds[port_phy+8+3] == SPEED_0 ))
			    is_null_quad = 1;
			    else
			    is_null_quad = 0;
			    if(is_null_quad == 1){
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			    }
			    else if( mix_mode_1g_2g )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[(port_phy+8)] == SPEED_1G && (num_2g_port_lane == 0) )
			     param_speeds[port] = SPEED_5G;
			    else if( param_speeds[(port_phy+8)] == SPEED_1G && (num_2g_port_lane > 0) )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[(port_phy+8)] == SPEED_2p5G )
			     param_speeds[port] = SPEED_10G;
			     }
			 else{
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			  }
			     }
			else if (n == 3 ) {
			 if (num_g_ports >= 1){ /* checking for second lane */
			    if((param_speeds[port_phy+12] == SPEED_2p5G)|| (param_speeds[port_phy+12+1] == SPEED_2p5G)|| (param_speeds[port_phy+12+2] == SPEED_2p5G)||(param_speeds[port_phy+12+3] == SPEED_2p5G))
			    num_2g_port_lane = 1;
			    else
			    num_2g_port_lane = 0;
			    if((param_speeds[port_phy+12] == SPEED_0) && (param_speeds[port_phy+12+1] == SPEED_0 ) && (param_speeds[port_phy+12+2] == SPEED_0 ) && (param_speeds[port_phy+12+3] == SPEED_0 ))
			    is_null_quad = 1;
			    else
			    is_null_quad = 0;

			    if(is_null_quad == 1){
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			    }
			    else if( mix_mode_1g_2g )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[(port_phy+12)] == SPEED_1G && (num_2g_port_lane == 0) )
			     param_speeds[port] = SPEED_5G;
			    else if( param_speeds[(port_phy+12)] == SPEED_1G && (num_2g_port_lane > 0) )
			     param_speeds[port] = SPEED_10G;
			    else if( param_speeds[(port_phy+12)] == SPEED_2p5G )
			     param_speeds[port] = SPEED_10G;
			     }
			 else{
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			  }
			     }
                         else {
                            param_speeds[port] = SPEED_0;
                            param_states[port - 1] = PORT_STATE__DISABLED;
			    }
                        }
                    }
                    TDM_PRINT2("%s encode super PM %2d from 16x1G to 1x40G \n",
                               "[SPECIAL 1G CONFIG]",
                               (port_phy-1)/4);
                }
                }
   /* checking the updates done */ 
   for (port_phy = param_phy_lo;
             port_phy <= param_phy_hi && port_phy < HX5_NUM_PHY_PORTS;
             port_phy += 1){
                    TDM_PRINT2("%d PM port speed after encode %d \n",port_phy, param_speeds[port_phy]);
		}

    } else if (flag == 1) { /* decode super PM from 1x40G to 16x1G */
        num_1g_ports = 0 ;
        num_2g_ports = 0 ;
        for (port_phy = param_phy_lo;
             port_phy <= param_phy_hi && port_phy <= HX5_NUM_PHY_GPORTS;
             port_phy+= 16) {
               if (num_1g_ports == 0)   
               num_1g_ports = (port_num_1g_ports[port_phy - 1]); 
               if (num_2g_ports == 0)   
               num_2g_ports = (port_num_2g_ports[port_phy - 1]); 
                    TDM_PRINT3("phy_port_decap_state %d,  is_super_pm %d, num_1g_ports %d \n",port_phy , param_1g_en[port_phy - 1],num_1g_ports);
                    TDM_PRINT3("phy_port_decap_state %d,  is_super_pm %d, num_2g_ports %d \n",port_phy , param_1g_en[port_phy - 1],num_2g_ports);
		    TDM_PRINT0(" \n");
            if (param_1g_en[port_phy - 1] >= HX5_1G_PORT_STATE_16x1G) {
	       for(j=port_phy ; j < (16+port_phy)  ; j++){
                param_speeds[j] = SPEED_0;
                  /*  TDM_PRINT3("phy_port_decap_state %d,  is_super_pm %d, num_2g_ports %d \n",j , param_speeds[j],num_2g_ports); */

            if (param_1g_en[j - 1] == HX5_1G_PORT_STATE_16x1G) {
                if(num_1g_ports > 0){
                param_speeds[j] = SPEED_1G;
                param_states[j - 1] = PORT_STATE__LINERATE;
		num_1g_ports = num_1g_ports -1 ;
  		}
  		}
            if (param_1g_en[j - 1] == HX5_1G_PORT_STATE_16x2G) {
                if(num_2g_ports > 0){
                param_speeds[j] = SPEED_2p5G;
                param_states[j - 1] = PORT_STATE__LINERATE;
		num_2g_ports = num_2g_ports -1 ;
  		}
            }
            }
            }
        }
	/* token allocation */
        for (port_phy = param_phy_lo;
             port_phy <= param_phy_hi && port_phy < HX5_NUM_PHY_GPORTS;
             port_phy += 16) {

            if (param_1g_en[port_phy - 1] >= HX5_1G_PORT_STATE_16x1G) {
                for (cal_id = (port_phy - 1) / HX5_NUM_PHY_PORTS_PER_PIPE;
                     cal_id < 8; cal_id += 4) {
                    TDM_SEL_CAL(cal_id, cal_main);
                    cal_len = tdm_hx5_cmn_get_pipe_cal_len_max(cal_id, _tdm);
                    num_2g_ports = (port_num_2g_ports[port_phy - 1]); 
                    num_1g_ports = (port_num_1g_ports[port_phy - 1]); 
	            num_g_ports  = num_1g_ports + num_2g_ports ;
				   TDM_PRINT3(" num_2g_ports %d num_1g_ports` %d num_g_ports %d \n ",num_2g_ports,num_1g_ports, num_g_ports  ); 
                    if (cal_main != NULL && cal_len > 0) {
                        port_cnt = 0;
                        port_2g_cnt = 0;
		    /* replace all tokens to initial pm_base */ 	
                        for (i = 0; i < cal_len; i++) {
                           if((cal_main[i] == (port_phy+1)))
 			    cal_main[i] = port_phy ;
			   else if((cal_main[i] == (port_phy+2))) 
 			    cal_main[i] = port_phy ;
			   else if((cal_main[i] == (port_phy+3))) 
 			    cal_main[i] = port_phy ;
			}
                        for (i = 0; i < cal_len; i++) {
                            if (cal_main[i] == port_phy) {
			         /* complete allocation of 1 g tokens across all ports(1g/2g)  */  
				 if(num_g_ports > 0){
				 while ((param_1g_en[(port_phy+port_cnt) - 1] == HX5_1G_PORT_STATE_NONE))
				 {
				 port_cnt++ ;
				   TDM_PRINT2(" skiping port num  %d curr num_g_ports %d \n ",((port_phy+port_cnt)-1), num_g_ports  ); 
				 }
				 cal_main[i] = port_phy + port_cnt; 
				 num_g_ports-- ; 
				 port_cnt++ ;
				 }
				 else{ /* all 1g tokens allocated 
				   	  one token of 2G is allocated, lookup only 2G ports for other token */ 
				if(num_2g_ports >0){
				 for(j=port_2g_cnt ; j < 16  ; j++){
				      if(param_1g_en[(port_phy+j) - 1] == HX5_1G_PORT_STATE_16x2G) 
				        {
				          port_2g_cnt = j+1 ; /* nxt index */
				     	  cal_main[i] = port_phy + j; 
				    	  port_cnt++ ;
				    	  num_2g_ports-- ;
				    	  TDM_PRINT3(" allocating 2g tokens %d= port_2g_cnt  %d=cal_slot  %d=num_g_ports \n",port_2g_cnt,i,num_g_ports); 
				    	  break;
				        }
				       else if (num_1g_ports > 0 &&(param_1g_en[(port_phy+j) - 1] == HX5_1G_PORT_STATE_16x1G))
				    	{
				     	  port_2g_cnt = j+1 ; /* nxt index */
				     	  cal_main[i] = param_token_empty ; 
				          TDM_PRINT2(" extra token on slot  %d set to %d \n ",i, cal_main[i]  ); 
				     	break;
				        }
				    }
				 }
				    else{
				     cal_main[i] = param_token_empty ; 
				     TDM_PRINT2(" extra token on slot  %d set to %d \n ",i, cal_main[i]  ); 
				    }
				 }

				TDM_PRINT3("%d= is_2g  %d=num_2g_ports  %d=port_phy \n",is_2g,num_2g_ports,port_cnt); 

                                TDM_PRINT5("%s decode super PM %2d, cal_id %0d, slot_cnt %0d, port %3d\n",
                                          "[SPECIAL 1G CONFIG]",
                                          (port_phy - 1) / 4,
                                          cal_id,
                                          port_cnt,
                                          cal_main[i]);
                            }
                        }
                    }
                }
            }
        }
    }

    return PASS;
}

/**
 * @name: tdm_hx5_main_transcription_pm_25g_10g
 * @param:
 *        Regulate PM speed for special PM configs containing both 25G and 10G.
 *               -- 25g_25g_25g_10g -> 25g_25g_25g_25g
 *               -- 25g_25g_10g_10g -> 25g_25g_25g_25g
 *               -- 25g_10g_10g_10g -> 25g_25g_25g_25g
 *
 * For any PM, if both 25g and 10g exist, change 10g to 25g.
 */
static void
tdm_hx5_main_transcription_pm_25g_10g(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_10g_exit, pm_25g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_10g_exit = BOOL_FALSE;
        pm_25g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (param_speeds[port_phy] == SPEED_10G ||
                    param_speeds[port_phy] == SPEED_11G) {
                    pm_10g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_25G ||
                           param_speeds[port_phy] == SPEED_27G) {
                    pm_25g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_10g_exit == BOOL_TRUE && pm_25g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                    if (param_speeds[port_phy] == SPEED_10G ||
                        param_speeds[port_phy] == SPEED_11G) {
                        param_speeds[port_phy] = SPEED_25G;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 10G to 25G");
                    }
                }
            }
        }
    }
}

/**
@name: tdm_hx5_main_transcription
@param:

Transcription algorithm for generating port module mapping
 */
int
tdm_hx5_main_transcription( tdm_mod_t *_tdm )
{
    int i, j, port_phy, port_lanenum, last_port;
    int **param_pmap, param_pmap_wid, param_pmap_len,
        param_token_empty,
        param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pmap_wid    = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pmap_len    = _tdm->_chip_data.soc_pkg.pmap_num_modules;
    param_pmap        = _tdm->_chip_data.soc_pkg.pmap;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;

    /* initialize pmap */
    for (i=0; i<param_pmap_len; i++) {
        for (j=0; j<param_pmap_wid; j++) {
            param_pmap[i][j] = param_token_empty;
        }
    }
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

    tdm_hx5_main_transcription_1g_10g(_tdm); /* OVSB only: any speed between 1G to 10G will be treated as 10G. */
    tdm_hx5_main_transcription_1g_10g_LR(_tdm);
    tdm_hx5_main_transcription_1g_5g_LR(_tdm);
    tdm_hx5_main_transcription_1g_2g_LR(_tdm);
    tdm_hx5_main_transcription_1g_25g_LR(_tdm);

    /* SPECIAL SUPPORT for 16x1G */
    tdm_hx5_main_transcription_16x1G(_tdm, 0);


    tdm_hx5_main_transcription_pm_25g_10g(_tdm);
    /* SPECIAL SUPPORT for 1G */
   /*  tdm_hx5_main_transcription_1G(_tdm, 0); */

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP](_tdm) );
}

/**
@name: tdm_hx5_main_init
@param:
 */
int
tdm_hx5_main_init( tdm_mod_t *_tdm )
{
    int index;

    /* initialize chip/core parameters */
    _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_1P25G;
    _tdm->_chip_data.soc_pkg.pmap_num_modules = HX5_NUM_PM_MOD;
    _tdm->_chip_data.soc_pkg.pmap_num_lanes = HX5_NUM_PM_LNS;
    _tdm->_chip_data.soc_pkg.pm_num_phy_modules = HX5_NUM_PHY_PM;

    _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = HX5_OVSB_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl1_token = HX5_IDL1_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl2_token = HX5_IDL2_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.ancl_token = HX5_ANCL_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = HX5_NUM_PHY_PORTS;

    _tdm->_chip_data.cal_0.cal_len = HX5_LR_VBS_LEN;
    _tdm->_chip_data.cal_0.grp_num = HX5_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_0.grp_len = HX5_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_1.cal_len = HX5_LR_VBS_LEN;
    _tdm->_chip_data.cal_1.grp_num = HX5_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_1.grp_len = HX5_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_2.cal_len = 0;
    _tdm->_chip_data.cal_2.grp_num = 0;
    _tdm->_chip_data.cal_2.grp_len = 0;
    _tdm->_chip_data.cal_3.cal_len = 0;
    _tdm->_chip_data.cal_3.grp_num = 0;
    _tdm->_chip_data.cal_3.grp_len = 0;
    _tdm->_chip_data.cal_4.cal_len = HX5_LR_VBS_LEN;
    _tdm->_chip_data.cal_4.grp_num = HX5_OS_VBS_GRP_NUM;;
    _tdm->_chip_data.cal_4.grp_len = HX5_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_5.cal_len = HX5_LR_VBS_LEN;
    _tdm->_chip_data.cal_5.grp_num = HX5_OS_VBS_GRP_NUM;;
    _tdm->_chip_data.cal_5.grp_len = HX5_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_6.cal_len = 0;
    _tdm->_chip_data.cal_6.grp_num = 0;
    _tdm->_chip_data.cal_6.grp_len = 0;
    _tdm->_chip_data.cal_7.cal_len = 0;
    _tdm->_chip_data.cal_7.grp_num = 0;
    _tdm->_chip_data.cal_7.grp_len = 0;	

    _tdm->_chip_data.soc_pkg.lr_idx_limit           = HX5_LEN_893MHZ;
    _tdm->_chip_data.soc_pkg.tvec_size              = HX5_NUM_ANCL;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.higig_mgmt= BOOL_FALSE;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_start= 1;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_end  = HX5_NUM_PHY_PORTS_PER_PIPE;
  /* if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.mgmt_mode==HX5_MGMT_MODE_1_PORT_EN || */
  /*     _tdm->_chip_data.soc_pkg.soc_vars.hx5.mgmt_mode==HX5_MGMT_MODE_1_PORT_HG) { */
  /*     _tdm->_chip_data.soc_pkg.tvec_size = HX5_NUM_ANCL-4;  10G per mgmt port */
  /* } */

    _tdm->_core_data.vars_pkg.pipe       = 0;
    _tdm->_core_data.rule__same_port_min = HX5_MIN_SPACING_SAME_PORT_HSP;
    _tdm->_core_data.rule__prox_port_min = HX5_MIN_SPACING_SISTER_PORT;
    _tdm->_core_data.vmap_max_wid        = HX5_VMAP_MAX_WID;
    _tdm->_core_data.vmap_max_len        = HX5_VMAP_MAX_LEN;

    if (_tdm->_chip_data.soc_pkg.num_ext_ports != HX5_NUM_EXT_PORTS) {
        TDM_WARN1("Invalid num_ext_port %d\n", _tdm->_chip_data.soc_pkg.num_ext_ports);
        _tdm->_chip_data.soc_pkg.num_ext_ports = HX5_NUM_EXT_PORTS;
    }

    /* Chip: pm_encap_type, pm_ovs_halfpipe, pm_num_to_pblk */
    for (index=0; index<HX5_NUM_PM_MOD; index++) {
        _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[index] = 0;
        _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_num_to_pblk[index]  = 0;
        /* encap */
        if (_tdm->_chip_data.soc_pkg.state[index*HX5_NUM_PM_LNS] == PORT_STATE__LINERATE_HG ||
            _tdm->_chip_data.soc_pkg.state[index*HX5_NUM_PM_LNS] == PORT_STATE__OVERSUB_HG) {
            _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_encap_type[index] = PM_ENCAP__HIGIG2;
        } else {
            _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_encap_type[index] = PM_ENCAP__ETHRNT;
        }
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
    _tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num+HX5_SHAPING_GRP_NUM)*sizeof(int *), "TDM inst 0 groups");
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        _tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
    }

    /* Chip: IDB Pipe 0 Packet Scheduler */
    for (index=0; index<HX5_SHAPING_GRP_NUM; index++) {
        _tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0+index]=(int *) TDM_ALLOC((HX5_SHAPING_GRP_LEN)*sizeof(int), "TDM inst 0 shaping calendars");
        TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0+index],(_tdm->_chip_data.soc_pkg.num_ext_ports),HX5_SHAPING_GRP_LEN);
    }

 /*   Chip: IDB Pipe 1 calendar group 
 *  _tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
 *  TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
 *  _tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num+HX5_SHAPING_GRP_NUM)*sizeof(int *), "TDM inst 1 groups");
 *  for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
 *      _tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
 *      TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
 *  }

 *   Chip: IDB Pipe 1 Packet Scheduler 
 *  for (index=0; index<HX5_SHAPING_GRP_NUM; index++) {
 *      _tdm->_chip_data.cal_1.cal_grp[HX5_SHAPING_GRP_IDX_0+index]=(int *) TDM_ALLOC((HX5_SHAPING_GRP_LEN)*sizeof(int), "TDM inst 1 shaping calendars");
 *      TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[HX5_SHAPING_GRP_IDX_0+index],(_tdm->_chip_data.soc_pkg.num_ext_ports),HX5_SHAPING_GRP_LEN);
 *  }
 */
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

 /* Chip: MMU Pipe 1 calendar group 
  * _tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
  * TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
  * _tdm->_chip_data.cal_5.cal_grp = NULL;
 */
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

    /* check validity of input config */
    if (tdm_hx5_main_chk(_tdm) != PASS) {
        return FAIL;
    }

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION](_tdm) );
}

/**
@name: tdm_hx5_main_chk
@param:
 */
int
tdm_hx5_main_chk( tdm_mod_t *_tdm )
{
    int i, j, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_pmap_wid;
    enum port_speed_e *param_speeds;

    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pmap_wid   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_speeds     = _tdm->_chip_data.soc_pkg.speed;

    /* 1. check validity of input port config:
       -- If one PM is active, then the first lane must have speed > SPEED_0
       -- 100G port cannot be configured as LINERATE.
    */
    for (i=param_phy_lo; i<=param_phy_hi; i+=param_pmap_wid) {
        if (param_speeds[i]==SPEED_0){
            for (j=1; j<param_pmap_wid; j++) {
                if ((i+j)<=param_phy_hi) {
                    if (param_speeds[i+j]>SPEED_0) {
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
    /* not sure of this check
    for (i=param_phy_lo; i<=param_phy_hi; i++) {
        if (param_speeds[i] == SPEED_100G) {
            if (param_states[i-1] == PORT_STATE__LINERATE ||
                param_states[i-1] == PORT_STATE__LINERATE_HG) {
                result = FAIL;
                TDM_ERROR3("%s, port %3d, speed %3dG, LINERATE\n",
                          "TDM: Invalid Port config",
                          i, param_speeds[i]/1000);
            }
        }
    }
    */
    /* 2. check validity of input IO bandwidth:
       -- LineRate_bandwidth <= core_bandwidth
       -- Oversub_bandwidth_ratio <= 2.0
    */

    return (result);
}

/**
@name: tdm_hx5_main_post
@param:
 */
int
tdm_hx5_main_post( tdm_mod_t *_tdm )
{
    /* TDM_PRINT_STACK_SIZE("tdm_hx5_main_post"); */

    if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.pipe_end>=76){
        TDM_SML_BAR
        TDM_PRINT0("\nTDM: TDM algorithm is completed.\n\n");
        TDM_SML_BAR

        /* SPECIAL SUPPORT for 16x1G */
        tdm_hx5_main_transcription_16x1G(_tdm, 1);

        /* TDM self-check */
        if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.tdm_chk_en==BOOL_TRUE){
            _tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm);
        }

        /* SPECIAL SUPPORT for 1G, reconvert speed config */
        /* tdm_hx5_main_transcription_1G(_tdm, 1);*/
    }

    return (PASS);
}


/**
@name: tdm_hx5_main_free
@param:
 */
int
tdm_hx5_main_free( tdm_mod_t *_tdm )
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
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0]);
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_1]);
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

    /* for flexport */
    if (_tdm->_chip_data.soc_pkg.flex_port_en == BOOL_TRUE) {
        TDM_PRINT0("Start free FlexPort memory \n");
        /* IDB 0 calendar group */
        TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_main);
        for (index=0; index<(HX5_OS_VBS_GRP_NUM); index++) {
            TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[index]);
        }
        TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0]);
        TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_1]);
        TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp);

        /* MMU 0 calendar group */
        TDM_FREE(_tdm->_prev_chip_data.cal_4.cal_main);

        TDM_PRINT0("Finish free FlexPort memory \n");
        TDM_SML_BAR
    }

    TDM_PRINT0("Finish free TDM internal memory \n");
    TDM_SML_BAR
    return (PASS);
}
