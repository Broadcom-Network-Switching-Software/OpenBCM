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
@name: tdm_fb6_main_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_fb6_main_corereq( tdm_mod_t *_tdm )
{
    _tdm->_core_data.vars_pkg.cal_id = _tdm->_core_data.vars_pkg.pipe;

    return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}

/**
@name: tdm_fb6_main_ingress_pipe
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_fb6_main_ingress_pipe( tdm_mod_t *_tdm )
{
    int iter, idx1=0, idx2=0, port_phy, result=PASS;
    int hg_mgmt_en=BOOL_FALSE;  /* FB6 does not support HG management port */
    int param_cal_len, param_ancl_num,
        param_pipe_id,
        param_port_start, param_port_end,
        param_token_empty;
    int *param_lr_buff, *param_os_buff;
    enum port_state_e *param_states;
    int pm_no;


    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_port_start  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_start;
    param_port_end    = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_end;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_pipe_id     = _tdm->_core_data.vars_pkg.pipe;
    param_lr_buff     = _tdm->_core_data.vars_pkg.lr_buffer;
    param_os_buff     = _tdm->_core_data.vars_pkg.os_buffer;
    param_states      = _tdm->_chip_data.soc_pkg.state;

    param_cal_len     = tdm_fb6_cmn_get_pipe_cal_len(param_pipe_id, _tdm);
    result = (param_cal_len > 0) ? result : FAIL ;

    tdm_fb6_print_pipe_config(_tdm);
    for (iter = 0; iter < TDM_AUX_SIZE; iter++) {
        param_lr_buff[iter] = param_token_empty;
        param_os_buff[iter] = param_token_empty;
    }

    /* construct lr_buffer and os_buffer */
    for (port_phy = param_port_start; port_phy <= param_port_end; port_phy++) {
        if ( param_states[port_phy-1] == PORT_STATE__LINERATE   ||
             param_states[port_phy-1] == PORT_STATE__LINERATE_HG ) {
            if (idx1<TDM_AUX_SIZE){
               if((_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4731_1012) ||
                  (_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4732_1012)) {
                   pm_no = (port_phy-1)/4;
                   param_lr_buff[0] = pm_no*4+1;
                   param_lr_buff[1] = pm_no*4+3;
                   param_states[pm_no*4+0]  = PORT_STATE__LINERATE;
                   param_states[pm_no*4+1]  = PORT_STATE__LINERATE;
                   param_states[pm_no*4+2]  = PORT_STATE__LINERATE;
                   param_states[pm_no*4+3]  = PORT_STATE__LINERATE;
                }
                else {
                  param_lr_buff[idx1++] = port_phy;
                }
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
    _tdm->_chip_data.soc_pkg.soc_vars.fb6.higig_mgmt = hg_mgmt_en;

    if (result != PASS) {
        return result;
    }
    return (_tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ](_tdm));
}

/**
@name: tdm_fb6_main_ingress
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_fb6_main_ingress( tdm_mod_t *_tdm )
{
    int pipe_idx, pipe_port_start, pipe_port_end, result=PASS;
    int param_phy_lo, param_phy_hi;

    /* TDM_PRINT_STACK_SIZE("tdm_fb6_main_ingress"); */

    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (pipe_idx = 0; pipe_idx < FB6_NUM_PIPE; pipe_idx++){
        pipe_port_start = pipe_idx*FB6_NUM_PHY_PORTS_PER_PIPE + 1;
        pipe_port_end   = (pipe_idx+1)*FB6_NUM_PHY_PORTS_PER_PIPE+4;
        if (pipe_port_start >= param_phy_lo && pipe_port_end <= param_phy_hi) {
            _tdm->_core_data.vars_pkg.pipe                   = pipe_idx;
            _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_start = pipe_port_start;
            _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_end   = pipe_port_end;
            if (tdm_fb6_main_ingress_pipe(_tdm) != PASS) {
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
 * @name: tdm_fb6_main_transcription_all_10g
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 10G.
 *	  updated for 2.5 and 5G modes
 */
static void
tdm_fb6_main_transcription_all_10g(tdm_mod_t *_tdm)
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
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.port_1g_en;

    for (port_phy=param_phy_lo; port_phy<=param_phy_hi; port_phy++) {
        port_speed = param_speeds[port_phy];
        port_state = param_states[port_phy - 1];
        if ((port_speed == SPEED_1G)||(port_speed == SPEED_5G) ||(port_speed == SPEED_2p5G) || (port_speed == SPEED_2G) )
            {
            if(port_state == PORT_STATE__OVERSUB ||
               port_state == PORT_STATE__OVERSUB_HG) {
                param_speeds[port_phy] = SPEED_10G;
            	param_1g_en[port_phy - 1] = FB6_1G_PORT_STATE_REGULAR ;
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
 * @name: tdm_fb6_main_transcription_1g_10G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 10G.
 *        updated for 2.5 and 5 G speeds
 */
static void
tdm_fb6_main_transcription_1g_10g_LR(tdm_mod_t *_tdm)
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
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.port_1g_en;

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
            		param_1g_en[port_phy - 1] = FB6_1G_PORT_STATE_REGULAR ;
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
 * @name: tdm_fb6_main_transcription_1g_5G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 5G.
 */
static void
tdm_fb6_main_transcription_1g_5g_LR(tdm_mod_t *_tdm)
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
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.port_1g_en;

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
            		param_1g_en[port_phy - 1] = FB6_1G_PORT_STATE_REGULAR ;
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
 * @name: tdm_fb6_main_transcription_1g_2G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 2G.
 */
static void
tdm_fb6_main_transcription_1g_2g_LR(tdm_mod_t *_tdm)
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
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.port_1g_en;

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
            		param_1g_en[port_phy - 1] = FB6_1G_PORT_STATE_REGULAR ;
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
 * @name: tdm_fb6_main_transcription_1g_25G_LR
 * @param:
 *        For all front panel physical ports,
 *        1G will be treated as 10G.
 */
static void
tdm_fb6_main_transcription_1g_25g_LR(tdm_mod_t *_tdm)
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
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.port_1g_en;

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
            		param_1g_en[port_phy - 1] = FB6_1G_PORT_STATE_REGULAR ;
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
 * @name: tdm_fb6_main_transcription_pm_50g_25g
 * @param:
 *        Regulate PM speed for special PM configs containing both 50G and 25G.
 *               -- 50g_25g_25g ->
 *               -- 25g_25g_50g ->
 *
 * For any PM, if both 50g and 25g exist, change 25g to 50g.
 */
/*
static void
tdm_fb6_main_transcription_pm_50g_25g(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_25g_exit, pm_50g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_25g_exit = BOOL_FALSE;
        pm_50g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (param_speeds[port_phy] == SPEED_25G ||
                    param_speeds[port_phy] == SPEED_27G) {
                    pm_25g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_50G ||
                           param_speeds[port_phy] == SPEED_53G) {
                    pm_50g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_25g_exit == BOOL_TRUE && pm_50g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                    if (param_speeds[port_phy] == SPEED_25G ||
                        param_speeds[port_phy] == SPEED_27G) {
                        param_speeds[port_phy] = SPEED_50G;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 25G to 50G");
                    }
                }
            }
        }
    }
}
*/



/**
 * @name: tdm_fb6_main_transcription_pm_25g_10g
 * @param:
 *        Regulate PM speed for special PM configs containing both 25G and 10G.
 *               -- 25g_25g_25g_10g -> 25g_25g_25g_25g
 *               -- 25g_25g_10g_10g -> 25g_25g_25g_25g
 *               -- 25g_10g_10g_10g -> 25g_25g_25g_25g
 *
 * For any PM, if both 25g and 10g exist, change 10g to 25g.
 */
static void
tdm_fb6_main_transcription_pm_25g_10g(tdm_mod_t *_tdm)
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
 * @name: tdm_fb6_main_transcription_pm_25g_10g_2p5g_1g
 * @param:
 *        Regulate PM speed for special PM configs containing both 25G and 10G.
 *               -- 25g_25g_25g_25g -> 25g_1010101010101010101010g_2p5g_1g
 *
 * For any PM, if 25g, 10g, 2.5g and 1 g exist, change to 25g.
 */
/*static void
tdm_fb6_main_transcription_pm_25g_10g_2p5g_1g(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_2p5g_exit, pm_1g_exit, pm_10g_exit, pm_25g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_1g_exit = BOOL_FALSE;
        pm_2p5g_exit = BOOL_FALSE;
        pm_10g_exit = BOOL_FALSE;
        pm_25g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (param_speeds[port_phy] == SPEED_10G ||
                    param_speeds[port_phy] == SPEED_11G) {
                    pm_10g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_2p5G) {
                    pm_2p5g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_1G) {
                    pm_1g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_25G ||
                           param_speeds[port_phy] == SPEED_27G) {
                    pm_25g_exit = BOOL_TRUE;
                }
            }
        }
        if (pm_1g_exit == BOOL_TRUE && pm_2p5g_exit == BOOL_TRUE && pm_10g_exit == BOOL_TRUE && pm_25g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                    if (param_speeds[port_phy] == SPEED_10G ||
                        param_speeds[port_phy] == SPEED_11G ||
                        param_speeds[port_phy] == SPEED_2p5G ||
                        param_speeds[port_phy] == SPEED_1G
                       ) {
                        param_speeds[port_phy] = SPEED_25G;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 1G, 2p5G, 10G to 25G");
                    }
                }
            }
        }
    }
}
*/

/**
 * @name: tdm_fb6_main_transcription_pm_10g_5g_1g
 * @param:
 *        Regulate PM speed for special PM configs containing both 25G and 10G.
 *               -- 10g_10g_10g_10g -> 10g_10g_5g_1g
 *
 * For any PM, if 10g, 10g, 5g and 1 g exist, change to 10g.
 */
/*static void
tdm_fb6_main_transcription_pm_10g_5g_1g(tdm_mod_t *_tdm)
{
    int port_phy, lane_num, pm_num, pm_5g_exit, pm_1g_exit, pm_10g_exit;
    int param_phy_lo, param_phy_hi, param_num_phy_pm, param_pm_lanes;
    enum port_speed_e *param_speeds;

    param_num_phy_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes   = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    for (pm_num = 0; pm_num < param_num_phy_pm; pm_num++) {
        pm_1g_exit = BOOL_FALSE;
        pm_5g_exit = BOOL_FALSE;
        pm_10g_exit = BOOL_FALSE;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port_phy = pm_num * param_pm_lanes + 1 + lane_num;
            if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                if (param_speeds[port_phy] == SPEED_10G ||
                    param_speeds[port_phy] == SPEED_11G) {
                    pm_10g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_5G) {
                    pm_5g_exit = BOOL_TRUE;
                } else if (param_speeds[port_phy] == SPEED_1G) {
                    pm_1g_exit = BOOL_TRUE;
                }
            }
        }

        if (pm_1g_exit == BOOL_TRUE && pm_5g_exit == BOOL_TRUE && pm_10g_exit == BOOL_TRUE) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port_phy = pm_num * param_pm_lanes + 1 + lane_num;
                if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
                    if (param_speeds[port_phy] == SPEED_10G ||
                        param_speeds[port_phy] == SPEED_11G ||
                        param_speeds[port_phy] == SPEED_5G ||
                        param_speeds[port_phy] == SPEED_1G
                       ) {
                        param_speeds[port_phy] = SPEED_10G;
                        TDM_PRINT3("TDM: [SPECIAL] PM %2d, Port %3d, %s\n",
                                  pm_num, port_phy,
                                  "Convert speed from 1G, 5G, 10G to 10G");
                    }
                }
            }
        }
    }
}

*/



/**
@name: tdm_fb6_main_transcription_1G
@param:
       -- _tdm: ptr to TDM global struct.
       -- flag: 0-> transfer 1G to 10G, 1-> revert 1G port to original.

Any 1G port is treated as 10G, and is given the same alloc as 10G.
 */
/*
static void
tdm_fb6_main_transcription_1G(tdm_mod_t *_tdm, int flag)
{
    int port_phy, spd_1g_en = 0;
    int param_phy_lo, param_phy_hi;
    int *param_1g_en;
    enum port_speed_e *param_speeds;

    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_1g_en  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.port_1g_en;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;

    if (flag == 0) { *//* transfer 1G to 2.5G */
/*        for (port_phy = param_phy_lo;
             port_phy <= param_phy_hi;
             port_phy++) {
            switch(param_speeds[port_phy]){
                case SPEED_1G:
                    spd_1g_en = 1;
                    param_speeds[port_phy] = SPEED_2p5G;
                    param_1g_en[port_phy - 1] = BOOL_TRUE;
                    break;
                default:
                    param_1g_en[port_phy - 1] = BOOL_FALSE;
                    break;
            }
        }
        if (spd_1g_en == 1) {
            TDM_PRINT0("[SPECIAL 1G CONFIG] transfer speed 1G to 2G \n");
        }
    } else if (flag == 1) {*/ /* revert original 1G port from 10G back to 1G */
/*        for (port_phy = param_phy_lo;
             port_phy <= param_phy_hi;
             port_phy++) {
            if (param_1g_en[port_phy - 1] == BOOL_TRUE) {
                param_speeds[port_phy] = SPEED_1G;
            }
        }
    }
}
*/

/**
@name: tdm_fb6_main_transcription_flex
@param:

Transcription algorithm for generating flex port state
 */
int
tdm_fb6_main_transcription_flex( tdm_mod_t *_tdm )
{
    int i;
    int param_phy_hi, param_flex_en;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    enum port_speed_e *param_speeds_prev;
    enum port_state_e *param_states_prev;

    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;
    param_speeds_prev = _tdm->_prev_chip_data.soc_pkg.speed;
    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;
    param_flex_en     = _tdm->_chip_data.soc_pkg.flex_port_en;

    if (param_flex_en == 1) {
        /* modify states for oversub ports */
        for (i=1; i<=param_phy_hi; i++) {
            /* skip linerate port */
            if (!(param_states[i-1] == PORT_STATE__OVERSUB ||
                  param_states[i-1] == PORT_STATE__OVERSUB_HG ||
                  param_states_prev[i-1] == PORT_STATE__OVERSUB ||
                  param_states_prev[i-1] == PORT_STATE__OVERSUB_HG)) {
                continue;
            }
            /* flex port up */
            if (param_speeds_prev[i] == 0 && param_speeds[i] > 0) {
                param_states_prev[i-1] = PORT_STATE__FLEX_OVERSUB_UP;
            }
            /* flex port down */
            else if (param_speeds_prev[i] > 0 && param_speeds[i] == 0) {
                param_states_prev[i-1] = PORT_STATE__FLEX_OVERSUB_DN;
            }
            /* flex port from one speed to another speed */
            else if (param_speeds_prev[i] > 0 && param_speeds[i] > 0 &&
                     param_speeds_prev[i] != param_speeds[i]) {

                /* 1G/10G/25G should be treated as 25G.
                   IF both of speed_prev and speed_curr are in [1G, 10G, 25G],
                   THEN port should NOT be marked as flexport.
                 */
                if ((param_speeds_prev[i] == SPEED_1G  ||
                     param_speeds_prev[i] == SPEED_10G ||
                     param_speeds_prev[i] == SPEED_11G ||
                     param_speeds_prev[i] == SPEED_25G ||
                     param_speeds_prev[i] == SPEED_27G) &&
                    (param_speeds[i] == SPEED_1G  ||
                     param_speeds[i] == SPEED_10G ||
                     param_speeds[i] == SPEED_11G ||
                     param_speeds[i] == SPEED_25G ||
                     param_speeds[i] == SPEED_27G)) {
                    continue;
                }
                param_states_prev[i-1] = PORT_STATE__FLEX_OVERSUB_CH;
            }
        }

        /* print */
        TDM_SML_BAR
        TDM_PRINT0("TDM: Portmap Comparision (FLEX) \n");
        TDM_PRINT5("\n%4s %8s %8s %8s %8s\n", "port", "spd_prev", "spd_curr", "sta_prev", "sta_curr");
        for (i = 1; i <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi; i++) {
            TDM_PRINT1("%4d", i);
            TDM_PRINT1("%8d", param_speeds_prev[i]/1000);
            TDM_PRINT1("%8d", param_speeds[i]/1000);
            TDM_PRINT2("%6s0x%02x", " ", param_states_prev[i-1]);
            TDM_PRINT2("%6s0x%02x\n", " ", param_states[i-1]);
        }
    }

    return PASS;
}




/**
@name: tdm_fb6_main_transcription
@param:

Transcription algorithm for generating port module mapping
 */
int
tdm_fb6_main_transcription( tdm_mod_t *_tdm )
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
    tdm_fb6_main_transcription_all_10g(_tdm); /* OVSB only: any speed between 1G to 10G will be treated as 10G. */
    tdm_fb6_main_transcription_1g_10g_LR(_tdm);
    tdm_fb6_main_transcription_1g_5g_LR(_tdm);
    tdm_fb6_main_transcription_1g_2g_LR(_tdm);
    tdm_fb6_main_transcription_1g_25g_LR(_tdm);

    /*tdm_fb6_main_transcription_pm_50g_25g(_tdm);*/
    tdm_fb6_main_transcription_pm_25g_10g(_tdm);

/*tdm_fb6_main_transcription_pm_25g_10g_2p5g_1g(_tdm);
tdm_fb6_main_transcription_pm_10g_5g_1g(_tdm);*/


  /* flex support */
    TDM_PRINT1("TDM: flex_port_en = %0d\n",
               _tdm->_chip_data.soc_pkg.flex_port_en);
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        tdm_fb6_main_transcription_flex(_tdm);
    }

    /* SPECIAL SUPPORT for 16x1G */
    /* SPECIAL SUPPORT for 1G */
    /* tdm_fb6_main_transcription_1G(_tdm, 0); */

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP](_tdm) );
}

/**
@name: tdm_fb6_main_init
@param:
 */
int
tdm_fb6_main_init( tdm_mod_t *_tdm )
{
    int index;

    /* initialize chip/core parameters */
    _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_2P5G;
    _tdm->_chip_data.soc_pkg.pmap_num_modules = FB6_NUM_PM_MOD;
    _tdm->_chip_data.soc_pkg.pmap_num_lanes = FB6_NUM_PM_LNS;
    _tdm->_chip_data.soc_pkg.pm_num_phy_modules = FB6_NUM_PHY_PM;

    _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = FB6_OVSB_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl1_token = FB6_IDL1_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.idl2_token = FB6_IDL2_TOKEN;
    _tdm->_chip_data.soc_pkg.soc_vars.ancl_token = FB6_ANCL_TOKEN;
/*    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 5; */
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
    _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = FB6_NUM_PHY_PORTS+4;

    _tdm->_chip_data.cal_0.cal_len = FB6_LR_VBS_LEN;
    _tdm->_chip_data.cal_0.grp_num = FB6_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_0.grp_len = FB6_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_1.cal_len = FB6_LR_VBS_LEN;
    _tdm->_chip_data.cal_1.grp_num = FB6_OS_VBS_GRP_NUM;
    _tdm->_chip_data.cal_1.grp_len = FB6_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_2.cal_len = 0;
    _tdm->_chip_data.cal_2.grp_num = 0;
    _tdm->_chip_data.cal_2.grp_len = 0;
    _tdm->_chip_data.cal_3.cal_len = 0;
    _tdm->_chip_data.cal_3.grp_num = 0;
    _tdm->_chip_data.cal_3.grp_len = 0;
    _tdm->_chip_data.cal_4.cal_len = FB6_LR_VBS_LEN;
    _tdm->_chip_data.cal_4.grp_num = FB6_OS_VBS_GRP_NUM;;
    _tdm->_chip_data.cal_4.grp_len = FB6_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_5.cal_len = FB6_LR_VBS_LEN;
    _tdm->_chip_data.cal_5.grp_num = FB6_OS_VBS_GRP_NUM;;
    _tdm->_chip_data.cal_5.grp_len = FB6_OS_VBS_GRP_LEN;
    _tdm->_chip_data.cal_6.cal_len = 0;
    _tdm->_chip_data.cal_6.grp_num = 0;
    _tdm->_chip_data.cal_6.grp_len = 0;
    _tdm->_chip_data.cal_7.cal_len = 0;
    _tdm->_chip_data.cal_7.grp_num = 0;
    _tdm->_chip_data.cal_7.grp_len = 0;

    _tdm->_chip_data.soc_pkg.lr_idx_limit           = FB6_LEN_1000_1012;

    switch(_tdm->_chip_data.soc_pkg.clk_freq) {
    case FB6_CLK_4730_1012:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4730_1012;
                             break;
    case FB6_CLK_4731_1012:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4731_1012;
                             break;
    case FB6_CLK_4732_1012:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4732_1012;
                             break;
    case FB6_CLK_4720_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4720_1250;
                             break;
    case FB6_CLK_4721_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4721_1250;
                             break;
    case FB6_CLK_4700_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4700_1250;
                             break;
    case FB6_CLK_4701_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4701_1250;
                             break;
    case FB6_CLK_4702_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4702_1250;
                             break;
    case FB6_CLK_4703_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4703_1250;
                             break;
    case FB6_CLK_4704_1087:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4704_1087;
                             break;
    case FB6_CLK_4705_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_4705_1250;
                             break;
    case FB6_CLK_1000_1012:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_1000_1012;
                             break;
    case FB6_CLK_1001_1250:  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_1001_1250;
                             break;
    default :  _tdm->_chip_data.soc_pkg.tvec_size              = FB6_NUM_ANCL_1000_1012;
       break;
    }
    _tdm->_chip_data.soc_pkg.soc_vars.fb6.higig_mgmt= BOOL_FALSE;
    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_start= 1;
    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_end  = FB6_NUM_PHY_PORTS_PER_PIPE;
  /* if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode==FB6_MGMT_MODE_1_PORT_EN || */
  /*     _tdm->_chip_data.soc_pkg.soc_vars.fb6.mgmt_mode==FB6_MGMT_MODE_1_PORT_HG) { */
  /*     _tdm->_chip_data.soc_pkg.tvec_size = FB6_NUM_ANCL-4;  10G per mgmt port */
  /* } */

    _tdm->_core_data.vars_pkg.pipe       = 0;
    _tdm->_core_data.rule__same_port_min = FB6_MIN_SPACING_SAME_PORT_HSP;
    _tdm->_core_data.rule__prox_port_min = FB6_MIN_SPACING_SISTER_PORT;
    _tdm->_core_data.vmap_max_wid        = FB6_VMAP_MAX_WID;
    _tdm->_core_data.vmap_max_len        = FB6_VMAP_MAX_LEN;

    if (_tdm->_chip_data.soc_pkg.num_ext_ports != FB6_NUM_EXT_PORTS) {
        TDM_WARN1("Invalid num_ext_port %d\n", _tdm->_chip_data.soc_pkg.num_ext_ports);
        _tdm->_chip_data.soc_pkg.num_ext_ports = FB6_NUM_EXT_PORTS;
    }

    /* Chip: pm_encap_type, pm_ovs_halfpipe, pm_num_to_pblk */
    for (index=0; index<FB6_NUM_PM_MOD; index++) {
        _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[index] = 0;
        _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[index]  = 0;
        /* encap */
        if (_tdm->_chip_data.soc_pkg.state[index*FB6_NUM_PM_LNS] == PORT_STATE__LINERATE_HG ||
            _tdm->_chip_data.soc_pkg.state[index*FB6_NUM_PM_LNS] == PORT_STATE__OVERSUB_HG) {
            _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_encap_type[index] = PM_ENCAP__HIGIG2;
        } else {
            _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_encap_type[index] = PM_ENCAP__ETHRNT;
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
    _tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num+FB6_SHAPING_GRP_NUM)*sizeof(int *), "TDM inst 0 groups");
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        _tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
    }

    /* Chip: IDB Pipe 0 Packet Scheduler */
    for (index=0; index<FB6_SHAPING_GRP_NUM; index++) {
        _tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0+index]=(int *) TDM_ALLOC((FB6_SHAPING_GRP_LEN+160)*sizeof(int), "TDM inst 0 shaping calendars");
        TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0+index],(_tdm->_chip_data.soc_pkg.num_ext_ports),FB6_SHAPING_GRP_LEN+160);
    }

   /* Chip: IDB Pipe 1 calendar group */
 /*  _tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");
   TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
   _tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num+FB6_SHAPING_GRP_NUM)*sizeof(int *), "TDM inst 1 groups");
   for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
       _tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
       TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
   }*/

   /* Chip: IDB Pipe 1 Packet Scheduler */
  /* for (index=0; index<FB6_SHAPING_GRP_NUM; index++) {
       _tdm->_chip_data.cal_1.cal_grp[FB6_SHAPING_GRP_IDX_0+index]=(int *) TDM_ALLOC((FB6_SHAPING_GRP_LEN)*sizeof(int), "TDM inst 1 shaping calendars");
       TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[FB6_SHAPING_GRP_IDX_0+index],(_tdm->_chip_data.soc_pkg.num_ext_ports),FB6_SHAPING_GRP_LEN);
   }*/

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
    /*_tdm->_chip_data.cal_4.cal_grp = NULL;*/
    _tdm->_chip_data.cal_4.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_num+FB6_SHAPING_GRP_NUM)*sizeof(int *), "TDM inst 0 groups");
    for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
        _tdm->_chip_data.cal_4.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_len)*sizeof(int), "TDM inst 0 group calendars");
        TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.grp_len);
    }

    /* Chip: MMU Pipe 0 Packet Scheduler */
    for (index=0; index<FB6_SHAPING_GRP_NUM; index++) {
        _tdm->_chip_data.cal_4.cal_grp[FB6_SHAPING_GRP_IDX_0+index]=(int *) TDM_ALLOC((FB6_SHAPING_GRP_LEN+160)*sizeof(int), "TDM inst 0 shaping calendars");
        TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[FB6_SHAPING_GRP_IDX_0+index],(_tdm->_chip_data.soc_pkg.num_ext_ports),FB6_SHAPING_GRP_LEN+160);
    }

   /* Chip: MMU Pipe 1 calendar group */
 /*  _tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 1 main calendar");
   TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
   _tdm->_chip_data.cal_5.cal_grp = NULL;*/

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
    if (tdm_fb6_main_chk(_tdm) != PASS) {
        return FAIL;
    }

    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION](_tdm) );
}

/**
@name: tdm_fb6_main_chk
@param:
 */
int
tdm_fb6_main_chk( tdm_mod_t *_tdm )
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
@name: tdm_fb6_main_post
@param:
 */
int
tdm_fb6_main_post( tdm_mod_t *_tdm )
{
    /* TDM_PRINT_STACK_SIZE("tdm_fb6_main_post"); */

    if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.pipe_end>=64){
        TDM_SML_BAR
        TDM_PRINT0("\nTDM: TDM algorithm is completed.\n\n");
        TDM_SML_BAR

        /* SPECIAL SUPPORT for 16x1G */

        /* TDM self-check */
        if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.tdm_chk_en==BOOL_TRUE){
            _tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm);
        }

        /* SPECIAL SUPPORT for 1G */
        /*tdm_fb6_main_transcription_1G(_tdm, 1); */
    }

    return (PASS);
}


/**
@name: tdm_fb6_main_free
@param:
 */
int
tdm_fb6_main_free( tdm_mod_t *_tdm )
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
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0]);
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_1]);
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
        for (index=0; index<(FB6_OS_VBS_GRP_NUM); index++) {
            TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[index]);
        }
        TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0]);
        TDM_FREE(_tdm->_prev_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_1]);
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

