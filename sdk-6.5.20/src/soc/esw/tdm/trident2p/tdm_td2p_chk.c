/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip self-check operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif



/**
@name: tdm_td2p_chk_calc_speed_slots
@param:

Return 
 */
int 
tdm_td2p_chk_calc_speed_slots(int port_speed)
{
    int slot_num=0, spd_norm=0;
    
    switch(port_speed){
        case SPEED_1G:
        case SPEED_2p5G:
            spd_norm = 1;
            break;
        case SPEED_10G:
        case SPEED_11G:
            spd_norm = 10;  
            break;
        case SPEED_20G:
        case SPEED_21G:
            spd_norm = 20;  
            break;
        case SPEED_25G:
        case SPEED_27G:
            spd_norm = 25;  
            break;
        case SPEED_40G:  
        case SPEED_42G:
        case SPEED_42G_HG2:
            spd_norm = 40;  
            break;
        case SPEED_50G:
        case SPEED_53G:
            spd_norm = 50;  
            break;
        case SPEED_100G:
        case SPEED_106G:
            spd_norm = 100; 
            break;
        case SPEED_120G:
        case SPEED_127G:
            spd_norm = 120; 
            break;
        default:
            spd_norm = 0;
            TDM_ERROR4("%s, %s -- %dM (%dG)\n",
                       "[Checker: Calc Speed Slots]",
                       "invalided port speed",
                       port_speed, port_speed/1000);
            break;
    }
    
    if (BW_QUANTA>0){
        slot_num = (spd_norm*10)/BW_QUANTA;
    }
    /* special case for 1G/2.5G speed under granulatiry of 2.5G per slot */
    if (spd_norm == 1 && slot_num == 0) {
        slot_num = 1;
    }
    
    return slot_num;
}


/**
@name: tdm_td2p_chk_get_port_tsc
@param:

Returns the TSC to which the input port belongs
 */
int
tdm_td2p_chk_get_port_tsc(tdm_mod_t *_tdm, int port)
{
    int i, j, port_tsc;
    int param_num_ext_ports;
    int param_num_pm_lanes;
    int param_num_phy_pm;
    int **param_port_pmap;
    
    param_num_ext_ports= _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_num_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_num_phy_pm   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_port_pmap    = _tdm->_chip_data.soc_pkg.pmap;
    
    port_tsc = param_num_ext_ports;
    TD2P_TOKEN_CHECK(port) {
        for (i=0; i<param_num_phy_pm; i++) {
            for (j=0; j<param_num_pm_lanes; j++) {
                if (port==param_port_pmap[i][j]){
                    port_tsc = i;
                    break;
                }
            }
            if (port_tsc!=param_num_ext_ports){
                break;
            }
        }
    }
    
    return port_tsc;
}


/**
@name: tdm_td2p_chk_get_cal_len
@param:

Returns the length of selected calendar
 */
int
tdm_td2p_chk_get_cal_len(tdm_mod_t *_tdm)
{
    int cal_len=0;
    int param_freq;
    
    param_freq = _tdm->_chip_data.soc_pkg.clk_freq;
    /* For TD2P/TD2, length_EN is equal to length_HG */
    switch (param_freq){
        case 760: cal_len = LEN_760MHZ_EN; break;
        case 608: 
        case 609: cal_len = LEN_608MHZ_EN; break;
        case 517: 
        case 518: cal_len = LEN_517MHZ_EN; break;
        case 415: 
        case 416: cal_len = LEN_415MHZ_EN; break;
        default:  cal_len = 0;             break;
    }

    return cal_len;
}


/**
@name: tdm_td2p_chk_get_pipe_port_range
@param:
 */
void 
tdm_td2p_chk_get_pipe_port_range(tdm_mod_t *_tdm, int cal_id, int *port_lo, int *port_hi)
{
    if (port_lo != NULL && port_hi != NULL) {
        switch(cal_id) {
            case 4: /* MMU Pipe X */
                *port_lo = 1;
                *port_hi = 64;
                break;
            case 5:  /* MMU Pipe Y */
                *port_lo = 65;
                *port_hi = 128;
                break;
            default:
                *port_lo = 0;
                *port_hi = 0;
                break;
        }
    }
}


/**
@name: tdm_td2p_chk_pipe_ovsb_enable
@param:

Return BOOL_TRUE if pipe contains ovsb ports, otherwise BOOL_FALSE.
 */
int 
tdm_td2p_chk_pipe_ovsb_enable(tdm_mod_t *_tdm, int cal_id)
{
    int port, port_lo, port_hi, os_en = BOOL_FALSE;
    int param_ext_port_num;
	enum port_state_e *param_states;
    
    param_states      = _tdm->_chip_data.soc_pkg.state;
    param_ext_port_num= _tdm->_chip_data.soc_pkg.num_ext_ports;
    
    tdm_td2p_chk_get_pipe_port_range(_tdm, cal_id, &port_lo, &port_hi);
    if (port_lo > 0 && port_lo < param_ext_port_num &&
        port_hi > 0 && port_hi < param_ext_port_num) {
        for (port = port_lo; port <= port_hi; port++) {
            if (param_states[port-1] == PORT_STATE__OVERSUB ||
                param_states[port-1] == PORT_STATE__OVERSUB_HG) {
                os_en = BOOL_TRUE;
                break;
            }
        }
    }
    
    return (os_en);
}


/**
@name: tdm_td2p_chk_struct_freq
@param:

Check frequency validity
 */
int 
tdm_td2p_chk_struct_freq(tdm_mod_t *_tdm)
{
    int result=PASS;
    int param_freq;

    param_freq = _tdm->_chip_data.soc_pkg.clk_freq;
    switch (param_freq) {
        case 760:
        case 608: case 609:
        case 517: case 518:
        case 415: case 416:
            break;
        default:
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized frequency %d\n",
                       "[Checker: Structure-Frequency]",
                       param_freq);
            break;
    }

    return result;
}


/**
@name: tdm_td2p_chk_struct_calLength
@param:

Checks calendar length of all MMU and/or IDB/PGW pipes
 */
int 
tdm_td2p_chk_struct_calLength(tdm_mod_t *_tdm)
{
    int i, cal_id, cal_main_len, len_max=0, result=PASS;
    int *param_cal_main=NULL;
    
    len_max    = tdm_td2p_chk_get_cal_len(_tdm);
    
    /* check calendar length for X-Pipe and Y-Pipe */
    for (cal_id=0; cal_id<8; cal_id++){
        if (cal_id!=TD2P_XPIPE_CAL_ID &&
            cal_id!=TD2P_YPIPE_CAL_ID ){
            continue;
        }
        
        TDM_SEL_CAL(cal_id, param_cal_main);
        if (param_cal_main!=NULL){
            /* actual calendar length */
            cal_main_len = TD2P_LR_VBS_LEN;
            for (i=(TD2P_LR_VBS_LEN-1); i>=0; i--){
                if(param_cal_main[i]==TD2P_NUM_EXT_PORTS){
                    cal_main_len--;
                }
                else{
                    break;
                }
            }
            /* checker */
            if (cal_main_len>len_max) {
                result=FAIL;
                if (cal_id==TD2P_XPIPE_CAL_ID){
                    TDM_ERROR3("%s, MMU X-Pipe, length %d, limit %d\n",
                        "Calendar length exceeded",
                        cal_main_len, len_max);
                }
                else {
                    TDM_ERROR3("%s, MMU Y-Pipe, length %d, limit %d\n",
                        "Calendar length exceeded",
                        cal_main_len, len_max);
                }
            }
        }
        else{
            result = (result==FAIL || result==UNDEF)? (result): (FAIL);
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_struct_speed
@param:

Check if speed type is supported
 */
int 
tdm_td2p_chk_struct_speed(tdm_mod_t *_tdm)
{
    int port_phynum, port_speed, port_state,
        port_result, result=PASS;
    int port_lo;
    int port_hi;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;
    
    port_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    port_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    port_speeds  = _tdm->_chip_data.soc_pkg.speed;
    port_states  = _tdm->_chip_data.soc_pkg.state;

    /* check port speed */
    for (port_phynum=port_lo; port_phynum<=port_hi; port_phynum++){
        port_speed = port_speeds[port_phynum];
        port_state = port_states[port_phynum-1];
        if (port_speed>SPEED_0 && 
            (port_state==PORT_STATE__LINERATE    || 
             port_state==PORT_STATE__LINERATE_HG ||
             port_state==PORT_STATE__OVERSUB     ||
             port_state==PORT_STATE__OVERSUB_HG  ||
             port_state==PORT_STATE__MANAGEMENT ) ){
            switch(port_speed){
                case SPEED_1G:
                case SPEED_2p5G:
                case SPEED_10G: case SPEED_11G:
                case SPEED_20G: case SPEED_21G: case SPEED_21G_DUAL:
                case SPEED_40G: case SPEED_42G: case SPEED_42G_HG2:
                case SPEED_100G:case SPEED_106G:
                case SPEED_120G:case SPEED_127G:
                    port_result = PASS;
                    break;
                default:
                    port_result = UNDEF;
                    TDM_ERROR3("%s, unrecognized speed %dG for port %d\n",
                        "[Checker: Structure-Speed]",
                        (port_speed/1000),port_phynum);
                    break;
            }
            
            if (port_result!=PASS){
                result = (result==FAIL || result==UNDEF)?
                         (result):
                         (port_result);
            }
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_struct_os_ratio
@param:

Check oversub ratio constraint
 */
int 
tdm_td2p_chk_struct_os_ratio(tdm_mod_t *_tdm)
{
    int i, port, chk_pass_x = BOOL_TRUE, chk_pass_y = BOOL_TRUE,
        port_lo, port_hi, slot_req, slot_allc, result;
    int *param_cal_main, param_cal_len, param_token_ovsb, param_ovsb_ratio;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    
    param_ovsb_ratio  = 2;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;
    
    /* MMU Pipe X */
    if (tdm_td2p_chk_pipe_ovsb_enable(_tdm, 4) == BOOL_TRUE) {
        slot_req  = 0;
        slot_allc = 0;
        param_cal_main = _tdm->_chip_data.cal_4.cal_main; 
        param_cal_len  = _tdm->_chip_data.cal_4.cal_len;
        tdm_td2p_chk_get_pipe_port_range(_tdm, 4, &port_lo, &port_hi);
        for (port = port_lo; port <= port_hi; port++) {
            if (param_states[port-1] == PORT_STATE__OVERSUB ||
                param_states[port-1] == PORT_STATE__OVERSUB_HG) {
                slot_req += tdm_td2p_chk_calc_speed_slots(param_speeds[port]);
            }
        }
        for (i = 0; i < param_cal_len; i++) {
            if (param_cal_main[i] == param_token_ovsb) {
                slot_allc++;
            }
        }
        if (slot_allc > 0) {
            if (slot_req / slot_allc > param_ovsb_ratio) {
                chk_pass_x = BOOL_FALSE;
            }
        } else {
            chk_pass_x = BOOL_FALSE;
        }
        if (chk_pass_x == BOOL_FALSE) {
            TDM_ERROR3("%s, MMU pipe X, slot_req %d, slot_allc %d, (ovesrub_ratio > 2)\n",
                       "[oversub_ratio] Invalid port config",
                       slot_req, slot_allc);
        }
    }
    /* MMU Pipe Y */
    if (tdm_td2p_chk_pipe_ovsb_enable(_tdm, 5) == BOOL_TRUE) {
        slot_req  = 0;
        slot_allc = 0;
        param_cal_main = _tdm->_chip_data.cal_5.cal_main; 
        param_cal_len  = _tdm->_chip_data.cal_5.cal_len;
        tdm_td2p_chk_get_pipe_port_range(_tdm, 5, &port_lo, &port_hi);
        for (port = port_lo; port <= port_hi; port++) {
            if (param_states[port-1] == PORT_STATE__OVERSUB ||
                param_states[port-1] == PORT_STATE__OVERSUB_HG) {
                slot_req += tdm_td2p_chk_calc_speed_slots(param_speeds[port]);
            }
        }
        for (i = 0; i < param_cal_len; i++) {
            if (param_cal_main[i] == param_token_ovsb) {
                slot_allc++;
            }
        }
        if (slot_allc > 0) {
            if (slot_req / slot_allc > param_ovsb_ratio) {
                chk_pass_y = BOOL_FALSE;
            }
        } else {
            chk_pass_y = BOOL_FALSE;
        }
        if (chk_pass_y == BOOL_FALSE) {
            TDM_ERROR3("%s, MMU pipe Y, slot_req %d, slot_allc %d, (ovesrub_ratio > 2)\n",
                       "[oversub_ratio] Invalid port config",
                       slot_req, slot_allc);
        }
    }

    result = FAIL;
    if (chk_pass_x == BOOL_TRUE && chk_pass_y == BOOL_TRUE) {
        result = PASS;
    }
    return (result);
}


/**
@name: tdm_td2p_chk_struct
@param:

Checks length, frequency, and speed type
 */
int tdm_td2p_chk_struct(tdm_mod_t *_tdm)
{
    int result_x, result=PASS;
    
    /* frequency */
    result_x = tdm_td2p_chk_struct_freq(_tdm);
    result   = (result_x != PASS)? (result_x): (result);
    
    /* calendar length */
    result_x = tdm_td2p_chk_struct_calLength(_tdm);
    result   = (result_x != PASS)? (result_x): (result);
    
    /* speed */
    result_x = tdm_td2p_chk_struct_speed(_tdm);
    result   = (result_x != PASS)? (result_x): (result);
    
    /* oversub ratio */
    result_x = tdm_td2p_chk_struct_os_ratio(_tdm);
    result   = (result_x != PASS)? (result_x): (result);
    
    return result;
}


/**
@name: tdm_td2p_chk_transcription
@param:

Verifies TSC transcription caught all ports indexed by port_state_array
 */
int 
tdm_td2p_chk_transcription(tdm_mod_t *_tdm)
{
    int i, j, port_phynum, port_speed, port_state, port_tsc, port_lanes,
        port_result, result=PASS;
    int port_lo;
    int port_hi;
    int num_pm_lanes;
    int num_phy_pm;
    int **port_pmap;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;
    
    port_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    port_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    num_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    num_phy_pm   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    port_pmap    = _tdm->_chip_data.soc_pkg.pmap;
    port_speeds  = _tdm->_chip_data.soc_pkg.speed;
    port_states  = _tdm->_chip_data.soc_pkg.state;

    /* check port transcription */
    for (port_phynum=port_lo; port_phynum<=port_hi; port_phynum++){
        port_speed = port_speeds[port_phynum];
        port_state = port_states[port_phynum-1];
        /* ingonre management port */
        if (port_phynum==13 ||
            port_phynum==14 ||
            port_phynum==15 ||
            port_phynum==16){
            continue;
        }
        if (port_speed>SPEED_0 && 
            (port_state==PORT_STATE__LINERATE    || 
             port_state==PORT_STATE__LINERATE_HG ||
             port_state==PORT_STATE__OVERSUB     ||
             port_state==PORT_STATE__OVERSUB_HG  ||
             port_state==PORT_STATE__MANAGEMENT ) ){
            port_result = PASS;
            port_tsc = tdm_td2p_chk_get_port_tsc(_tdm, port_phynum);
            
            if (!(port_tsc<num_phy_pm)){
                port_result = FAIL;
                TDM_ERROR2("%s, un-transcribed port %0d\n",
                    "[Port Transcription]", 
                    port_phynum);
            }
            else{
                port_lanes = 0;
                if (port_speed>=SPEED_100G){
                    for (j=0; j<3; j++){
                        if (port_tsc<num_phy_pm){
                            for (i=0; i<num_pm_lanes; i++){
                                if(port_phynum==port_pmap[port_tsc][i]){
                                    port_lanes++;
                                }
                            }
                        }
                        port_tsc++;
                    }
                }
                else {
                    for (i=0; i<num_pm_lanes; i++){
                        if(port_phynum==port_pmap[port_tsc][i]){
                            port_lanes++;
                        }
                    } 
                }
                switch(port_speed){
                    case SPEED_1G:
                    case SPEED_2p5G:
                    case SPEED_10G:
                    case SPEED_11G:
                        if (port_lanes<1){
                            port_result = FAIL;
                        }
                        break;
                    case SPEED_20G:
                    case SPEED_21G:
                    case SPEED_25G:
                    case SPEED_27G:
                        if (port_lanes<2){
                            port_result = FAIL;
                        }
                        break;
                    case SPEED_40G:
                    case SPEED_42G:
                    case SPEED_42G_HG2:
                    case SPEED_50G:
                    case SPEED_53G:
                        if ((port_lanes==0) ||
                            (port_lanes!=2 && port_lanes!=4) ){
                            port_result = FAIL;
                        }
                        break;
                    case SPEED_100G:
                    case SPEED_106G:
                        if ((port_lanes==0) || 
                            (port_lanes!=4 && port_lanes!=10 &&
                             port_lanes!=12) ){
                            port_result = FAIL;
                        }
                        break;
                    case SPEED_120G:
                    case SPEED_127G:
                        if (port_lanes!=12){
                            port_result = FAIL;
                        }
                        break;
                    default:
                        port_result = UNDEF;
                        TDM_ERROR3("%s, unrecognized speed %dG for port %d\n",
                            "[Checker: Port Transcription]",
                            (port_speed/1000),port_phynum);
                        
                        break;
                }
                if (port_result==FAIL){
                    TDM_ERROR4("%s, %s, port %d speed %dG\n",
                            "[Checker: Port Transcription]",
                            "illegal TSC configuration",
                            port_phynum, (port_speed/1000));
                }
            }
            
            if (port_result!=PASS){
                result = (result==FAIL || result==UNDEF)?
                         (result):
                         (port_result);
            }
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_samePortMin
@param:

Checks same-port-min spacing constraint
 */
int 
tdm_td2p_chk_samePortMin(tdm_mod_t *_tdm)
{
    int i, j, k, cal_id, port_i, port_k, 
        port_result, result=PASS;
    int *cal_main=NULL;
    int cal_main_len;
    int e_same_min;
   
    e_same_min   = _tdm->_core_data.rule__same_port_min;
    cal_main_len = tdm_td2p_chk_get_cal_len(_tdm);
    
    for (cal_id=0; cal_id<8; cal_id++){
        /* get calendar */
        switch(cal_id){
            case 0: cal_main = _tdm->_chip_data.cal_0.cal_main; break;
            case 1: cal_main = _tdm->_chip_data.cal_1.cal_main; break;
            case 2: cal_main = _tdm->_chip_data.cal_2.cal_main; break;
            case 3: cal_main = _tdm->_chip_data.cal_3.cal_main; break;
            case 4: cal_main = _tdm->_chip_data.cal_4.cal_main; break;
            case 5: cal_main = _tdm->_chip_data.cal_5.cal_main; break;
            case 6: cal_main = _tdm->_chip_data.cal_6.cal_main; break;
            case 7: cal_main = _tdm->_chip_data.cal_7.cal_main; break;
/*             default:
                TDM_ERROR2("%s, invalid calendar ID %0d\n",
                    "[Checker: Min-Same-Port Spacing]", 
                    cal_id);
                cal_main = NULL;
                result   = (result==FAIL)?(result):(UNDEF);
                break; */
        }
        if ((cal_main!=NULL) &&
            (cal_id==TD2P_XPIPE_CAL_ID || cal_id==TD2P_YPIPE_CAL_ID)){
            /* check all slots within calendar */
            for (i=0; i<cal_main_len; i++){
                port_result = PASS;
                port_i = cal_main[i];
                TD2P_TOKEN_CHECK(port_i){
                    for (j=1; j<e_same_min; j++){
                        k = ((i+j)<cal_main_len)?(i+j):(i+j-cal_main_len);
                        port_k = cal_main[k];
                        if (port_i==port_k){
                            port_result = FAIL;
                            TDM_ERROR5("%s, port %d slot[%d,%d] space %d\n",
                                "[Checker: Min-Same-Port Spacing]",
                                port_i, i, k, j);
                        }
                    }
                }
                result = (result==FAIL || result==UNDEF)? 
                         (result):
                         (port_result);
            }
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_sisterMin
@param:

Checks sister port spacing constraint
 */
int 
tdm_td2p_chk_sisterMin(tdm_mod_t *_tdm)
{
    int i, j, k, cal_id, port_i, port_k, tsc_i, tsc_k, 
        port_result, result=PASS;
    int *cal_main=NULL;
    int cal_main_len;
    int e_sister_min;
   
    e_sister_min = _tdm->_core_data.rule__prox_port_min;
    cal_main_len = tdm_td2p_chk_get_cal_len(_tdm);
    
    for (cal_id=0; cal_id<8; cal_id++){
        /* get calendar */
        switch(cal_id){
            case 0: cal_main = _tdm->_chip_data.cal_0.cal_main; break;
            case 1: cal_main = _tdm->_chip_data.cal_1.cal_main; break;
            case 2: cal_main = _tdm->_chip_data.cal_2.cal_main; break;
            case 3: cal_main = _tdm->_chip_data.cal_3.cal_main; break;
            case 4: cal_main = _tdm->_chip_data.cal_4.cal_main; break;
            case 5: cal_main = _tdm->_chip_data.cal_5.cal_main; break;
            case 6: cal_main = _tdm->_chip_data.cal_6.cal_main; break;
            case 7: cal_main = _tdm->_chip_data.cal_7.cal_main; break;
/*             default:
                cal_main = NULL;
                TDM_ERROR2("%s, invalid calendar ID %0d\n",
                    "[Checker: Min-Sister-Port Spacing]", 
                    cal_id);
                break; */
        }
        /* Check MMU Pipe 0 and MMu Pipe 1 */
        if ((cal_main==NULL) || 
            (cal_id!=TD2P_XPIPE_CAL_ID && cal_id!=TD2P_YPIPE_CAL_ID)){
            continue;
        }
        /* check all slots within calendar */
        for (i=0; i<cal_main_len; i++){
            port_result = PASS;
            port_i = cal_main[i];
            TD2P_TOKEN_CHECK(port_i){
                tsc_i = tdm_td2p_chk_get_port_tsc(_tdm, port_i);
                for (j=1; j<e_sister_min; j++){
                    k = ((i+j)<cal_main_len)?(i+j):(i+j-cal_main_len);
                    port_k = cal_main[k];
                    TD2P_TOKEN_CHECK(port_k){
                        tsc_k  = tdm_td2p_chk_get_port_tsc(_tdm,port_k);
                        if (tsc_i==tsc_k){
                            port_result = FAIL;
                            TDM_ERROR6("%s port[%d,%d] slot[%d,%d] space %d\n",
                                "[Checker: Min-Sister-Port Spacing]",
                                port_i, port_k, i, k, j);
                        }
                    }
                }
            }
            result = (result==FAIL || result==UNDEF)?
                     (result):
                     (port_result);
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_sub_lr
@param:

Checks that all line rate nodes were extracted from solution matrix
 */
int 
tdm_td2p_chk_sub_lr( tdm_mod_t *_tdm, int cal_id)
{
    int i, j, idx_0, idx_1, result=PASS, result_i, port_spd_M, found_nodes,
        *cal_main, cal_len;
    const char *table;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;

    port_speeds= _tdm->_chip_data.soc_pkg.speed;
    port_states= _tdm->_chip_data.soc_pkg.state;

    TDM_SEL_CAL(cal_id, cal_main);
    cal_len = tdm_td2p_chk_get_cal_len(_tdm);

    /* check pipe ID */
    switch (cal_id) {
        /*
        case 0: table = "IDB Pipe 0"; break;
        case 1: table = "IDB Pipe 1"; break;		
        case 2: table = "IDB Pipe 2"; break;		
        case 3: table = "IDB Pipe 3"; break; */
        case 4: table = "MMU Pipe 0"; idx_0 = 1;  idx_1 = 64;  break;
        case 5: table = "MMU Pipe 1"; idx_0 = 65; idx_1 = 128; break;
        /*
        case 6: table = "MMU Pipe 2"; break;		
        case 7: table = "MMU Pipe 3"; break; */
        default:
            table  = "Unknown Pipe";
            result = UNDEF;
            TDM_ERROR1("Linerate subscription check failed, unrecognized table ID %0d\n",cal_id);
            break;
    }
    if (result!=PASS){return result;}

    /* Check linerate port speed */
    for (i=idx_0; i<=idx_1; i++) {
        if (port_states[i-1]==PORT_STATE__LINERATE   || 
            port_states[i-1]==PORT_STATE__LINERATE_HG){
            result_i = PASS;
            port_spd_M = port_speeds[i];
            /* Check port rate */
            found_nodes=0;
            for (j=0; j<cal_len; j++) {
                if (cal_main[j]==i) {
                    found_nodes++;
                }
            }
            if(found_nodes==0){
                TDM_ERROR3("Linerate subscription check failed in %s, port %0d is enabled but not scheduled in calender, %0d slots\n", table, i,found_nodes);
                result_i = FAIL;
            }
            else if ( found_nodes < ((port_spd_M*10)/1000/(BW_QUANTA)) ) {
                TDM_ERROR5("Linerate subscription failed in %s, %0dG vector for port %0d expected %0d nodes (found %0d)\n",table,port_spd_M/1000,i,((port_spd_M*10)/1000/(BW_QUANTA)),found_nodes);
                result_i = FAIL;
            }

            result = (result==FAIL || result==UNDEF)?
                     (result):
                     (result_i);
        }	
    }
	
	return result;
}


/**
@name: tdm_td2p_chk_sub_os
@param:

Checks that the oversub node is sorted into oversub speed groups
 */
int 
tdm_td2p_chk_sub_os(tdm_mod_t *_tdm, int cal_id)
{
	int i, j, k, idx_0=0, idx_1=0, result=PASS, result_i, group_idx, slot_idx, is_found,
	    **cal_grp=NULL, cal_grp_num=0, cal_grp_len=0, port_spd_M;
	const char *table;
	enum port_speed_e *port_speeds;
	enum port_state_e *port_states;
	
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
	port_states= _tdm->_chip_data.soc_pkg.state;
	
	
	/* check pipe id */
	switch (cal_id) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 4: table = "MMU Pipe 0"; 
				cal_grp     = _tdm->_chip_data.cal_4.cal_grp; 
				cal_grp_num = _tdm->_chip_data.cal_4.grp_num; 
				cal_grp_len = _tdm->_chip_data.cal_4.grp_len;
				idx_0       = 1;
				idx_1       = 64;
				break;
		case 5: table = "MMU Pipe 1"; 
				cal_grp     = _tdm->_chip_data.cal_5.cal_grp; 
				cal_grp_num = _tdm->_chip_data.cal_5.grp_num; 
				cal_grp_len = _tdm->_chip_data.cal_5.grp_len;
				idx_0       = 65;
				idx_1       = 128;
				break;
		/*
		case 6: table = "MMU Pipe 2"; break;		
		case 7: table = "MMU Pipe 3"; break; */
		default:
            table  = "Unknown Pipe";
			result = UNDEF;
			TDM_ERROR1("Oversub subscription check failed, unrecognized table ID %0d\n",cal_id);
			break;
	}
	if (result!=PASS){return result;}
	
	
	for (i=idx_0; i<=idx_1; i++){
		if (port_states[i-1]==PORT_STATE__OVERSUB   ||
			port_states[i-1]==PORT_STATE__OVERSUB_HG){
			result_i   = PASS;
			port_spd_M = port_speeds[i];
			/* determine ovsb speed group */
			group_idx  = 0;
			slot_idx   = 0;
			is_found   = BOOL_FALSE;
			for (j=0; j<cal_grp_num ; j++) {
				for (k=0; k<cal_grp_len; k++){
					if((cal_grp[j][k]) == i){
						is_found  = BOOL_TRUE;
						group_idx = j;
						slot_idx  = k;
						break;
					}
				}
			}
			/* check port speed with the corresponding speed group */
			if (is_found == BOOL_FALSE) {
				TDM_ERROR1("Oversub subscription check failed, could not find passed port %0d in any oversub speed group\n",i);
				result_i = FAIL;
			}
			else if (port_spd_M>=SPEED_10G && (port_spd_M>(port_speeds[cal_grp[group_idx][0]]+1010) || port_spd_M<(port_speeds[cal_grp[group_idx][0]]-1010))) {
				TDM_ERROR6("Oversub subscription check failed in %s, OVSB group %0d slot #%0d port %0d speed %0dG mismatches %0dG group assginment\n",table,group_idx,slot_idx,i,(port_spd_M/1000),port_speeds[cal_grp[group_idx][0]]/1000);
				result_i = FAIL;
			}
			result = (result==FAIL || result==UNDEF)?
                     (result):
                     (result_i);
		}
	}
	
	return result;
}


/**
@name: tdm_td2p_chk_sub_cpu
@param:

Checks that CPU bandwidth is satisfied
 */
int 
tdm_td2p_chk_sub_cpu( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, cpu_slots_required, mgmt_type, mgmt_type_check, oversub_x,
	    *cal_main, cal_len, clk_freq;
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_main   = _tdm->_chip_data.cal_4.cal_main; 
	cal_len    = _tdm->_chip_data.cal_4.cal_len; 
	
	/* determine management port type */
	mgmt_type = 0;
	mgmt_type_check = 0;
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==TD2P_MGMT_PORT_0) {
			mgmt_type_check+=2;
		}
		if (cal_main[i]==TD2P_MGMT_PORT_1) {
			mgmt_type_check++;
		}
		if (cal_main[i]==TD2P_MGMT_PORT_2) {
			mgmt_type_check++;
		}
		if (cal_main[i]==TD2P_MGMT_PORT_3) {
			mgmt_type_check++;
		}
	}
	if (mgmt_type_check==8) {
		mgmt_type=1;
	}
	else if (mgmt_type_check==5) {
		mgmt_type=4;
	}
	/* determine pipe ovsb property */
	oversub_x = BOOL_FALSE;
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==TD2P_OVSB_TOKEN) {
			oversub_x=BOOL_TRUE;
			break;
		}
	}
    if (oversub_x == BOOL_FALSE) {
        if (tdm_td2p_chk_pipe_ovsb_enable(_tdm, 4) == BOOL_TRUE) {
            oversub_x = BOOL_TRUE;
        }
    }
	/* determine number of CPU slots  */
	cpu_slots_required = 4;
	if (clk_freq==760 || clk_freq==517 || clk_freq==518) {
		if (oversub_x==BOOL_TRUE){
			switch(mgmt_type){
				case  0: cpu_slots_required = 3; break;
				case  4: cpu_slots_required = 1; break;
				default: break;
			}
		}
		else{
			switch(mgmt_type){
				case  1: cpu_slots_required = 0; break;
				case  4: cpu_slots_required = 2; break;
				default: break;
			}
		}
	}
	else if (clk_freq==608 || clk_freq==609 || clk_freq==415 || clk_freq==416) {
		switch(mgmt_type){
			case  0: cpu_slots_required = 5; break;
			case  1: cpu_slots_required = 1; break;
			case  4: cpu_slots_required = 3; break;
			default: break;
		}
	}
	
	/* calculate number of actual CPU slots */
	k=0;
	for (i=0; i<cal_len; i++){
		if(cal_main[i] == TD2P_CPU_PORT){
			k++;
		}
	}
	
	if (k<cpu_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe X, CPU port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,cpu_slots_required);
	}
	
	return result;
}


/**
@name: tdm_td2p_chk_sub_lpbk
@param:

Checks that LOOPBACK bandwidth is satisfied
 */
int 
tdm_td2p_chk_sub_lpbk( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, lb_slots_required, oversub_y,
	    *cal_main, cal_len, clk_freq;
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_main   = _tdm->_chip_data.cal_5.cal_main; 
	cal_len    = _tdm->_chip_data.cal_5.cal_len; 
	
	/* determine pipe ovsb property */
	oversub_y = BOOL_FALSE;
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==TD2P_OVSB_TOKEN) {
			oversub_y=BOOL_TRUE;
			break;
		}
	}
    if (oversub_y == BOOL_FALSE) {
        if (tdm_td2p_chk_pipe_ovsb_enable(_tdm, 5) == BOOL_TRUE) {
            oversub_y = BOOL_TRUE;
        }
    }
	/* determine number of loopback slots  */
	lb_slots_required = 4;
	if (clk_freq==760 || clk_freq==517 || clk_freq==518) {
		if (oversub_y) {
			lb_slots_required=3;
		}
	}
	else if (clk_freq==608 || clk_freq==609 || clk_freq==415 || clk_freq==416) {
		lb_slots_required=5;
	}
	/* calculate number of actual loopback slots */
	k=0;
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==TD2P_LOOPBACK_PORT) {
			k++;
		}
	}
	if (k<lb_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe Y, loopback port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,lb_slots_required);
	}
	
	return result;
}


/**
@name: tdm_td2p_chk_sub_acc
@param:

Checks that AGNOSTIC ACCESSORY bandwidth is satisfied
 */
int 
tdm_td2p_chk_sub_acc( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, ac_slots_required, x_oversub, y_oversub,
	    *x_cal_main, x_cal_len, *y_cal_main, y_cal_len, clk_freq;
	enum port_state_e *port_states;	
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	x_cal_main = _tdm->_chip_data.cal_4.cal_main; 
	x_cal_len  = _tdm->_chip_data.cal_4.cal_len;
	y_cal_main = _tdm->_chip_data.cal_5.cal_main; 
	y_cal_len  = _tdm->_chip_data.cal_5.cal_len; 
	port_states= _tdm->_chip_data.soc_pkg.state;
	
	/* determine pipe ovsb property */
	x_oversub = BOOL_FALSE;
	for (i=0; i<x_cal_len; i++) {
		if (x_cal_main[i]==TD2P_OVSB_TOKEN) {
			x_oversub = BOOL_TRUE;
			break;
		}
	}
	y_oversub = BOOL_FALSE;
	for (i=0; i<y_cal_len; i++) {
		if (y_cal_main[i]==TD2P_OVSB_TOKEN) {
			y_oversub = BOOL_TRUE;
			break;
		}
	}
	/* check pipe X */
	if (x_oversub) {
		ac_slots_required=10;
		if (clk_freq==760 || clk_freq==517 || clk_freq==518) {
			ac_slots_required=8;
		}

		k=0;
		for (i=0; i<x_cal_len; i++) {
			if (x_cal_main[i]==TD2P_CPU_PORT      ||
				x_cal_main[i]==TD2P_LOOPBACK_PORT ||
				x_cal_main[i]==TD2P_IDL1_TOKEN    ||
				x_cal_main[i]==TD2P_IDL2_TOKEN    ){
				k++;
			}
			else if (x_cal_main[i]==TD2P_MGMT_PORT_0 ||
				     x_cal_main[i]==TD2P_MGMT_PORT_1 ||
			 	     x_cal_main[i]==TD2P_MGMT_PORT_2 ||
				     x_cal_main[i]==TD2P_MGMT_PORT_3 ){
				if (port_states[x_cal_main[i]-1]==PORT_STATE__MANAGEMENT){
					k++;
				}
			}
		}
		
		if (k<ac_slots_required) {
			result = FAIL;
			TDM_ERROR2("Subscription check failed in MMU pipe X, accessory bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,ac_slots_required);
		}
	}
	/* check pipe Y */
	if (y_oversub) {
		ac_slots_required=10;
		if (clk_freq==760 || clk_freq==517 || clk_freq==518) {
			ac_slots_required=8;
		}

		k=0;
		for (i=0; i<y_cal_len; i++) {
			if (y_cal_main[i]==TD2P_CPU_PORT      ||
				y_cal_main[i]==TD2P_LOOPBACK_PORT ||
				y_cal_main[i]==TD2P_IDL1_TOKEN    ||
				y_cal_main[i]==TD2P_IDL2_TOKEN    ){
				k++;
			}
			else if (y_cal_main[i]==TD2P_MGMT_PORT_0 ||
				     y_cal_main[i]==TD2P_MGMT_PORT_1 ||
				     y_cal_main[i]==TD2P_MGMT_PORT_2 ||
				     y_cal_main[i]==TD2P_MGMT_PORT_3 ){
				if (port_states[y_cal_main[i]-1]==PORT_STATE__MANAGEMENT){
					k++;
				}
			}
		}
		
		if (k<ac_slots_required) {
			result = FAIL;
			TDM_ERROR2("Subscription check failed in MMU pipe Y, accessory bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,ac_slots_required);
		}
	}
	
	return result;
}

/**
@name: tdm_td2p_chk_subscription
@param:

Check port subscription status (bandwidth and rate)
 */
int 
tdm_td2p_chk_subscription(tdm_mod_t *_tdm)
{
    int result_x, result=PASS;

    /* 1. Check CPU port */
    result_x = tdm_td2p_chk_sub_cpu(_tdm);
    result = (result_x!=PASS)?(result_x):(result);

    /* 2. Check Loopback port */
    result_x = tdm_td2p_chk_sub_lpbk(_tdm);
    result = (result_x!=PASS)?(result_x):(result);

    /* 3. Check Agnostic accessory slots (8 or 10 for TD2/TD2+) */
    result_x = tdm_td2p_chk_sub_acc(_tdm);
    result = (result_x!=PASS)?(result_x):(result);

    /* 4. Check linerate ports */
    result_x = tdm_td2p_chk_sub_lr(_tdm, TD2P_XPIPE_CAL_ID);
    result = (result_x!=PASS)?(result_x):(result);

    result_x = tdm_td2p_chk_sub_lr(_tdm, TD2P_YPIPE_CAL_ID);
    result = (result_x!=PASS)?(result_x):(result);

    /* 5. Check oversub ports */
    result_x = tdm_td2p_chk_sub_os(_tdm, TD2P_XPIPE_CAL_ID);
    result = (result_x!=PASS)?(result_x):(result);
    
    result_x = tdm_td2p_chk_sub_os(_tdm, TD2P_YPIPE_CAL_ID);
    result = (result_x!=PASS)?(result_x):(result);

    return result;
}


/**
@name: tdm_td2p_chk_jitter_lr_pipe
@param:

Checks distribution of linerate slots of the same port for the given pipe
 */
int 
tdm_td2p_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, k, port_i, port_j, port_k, port_speed, 
        slot_req, distance, tracker[512], 
        has_checked, ports_done[TD2P_NUM_EXT_PORTS], done_idx=0, 
        result=PASS;
    int space_min=0, space_max=0, space_frac, jitter_range;
    
    const char *pipe_str;
    int cal_main_len;
    int *cal_main=NULL;
    enum port_speed_e *port_speeds;
    
    port_speeds  = _tdm->_chip_data.soc_pkg.speed;
    cal_main_len = tdm_td2p_chk_get_cal_len(_tdm);
    
    for (i=0; i<TD2P_NUM_EXT_PORTS; i++) {ports_done[i]=TD2P_NUM_EXT_PORTS;}
    for (i=0; i<cal_main_len && i<512; i++) {tracker[i] = 0;}
    
    switch(cal_id){
        case TD2P_XPIPE_CAL_ID: pipe_str = "MMU Pipe 0"; break;
        case TD2P_YPIPE_CAL_ID: pipe_str = "MMU Pipe 1"; break;
        default:
            pipe_str = "Unknown Pipe";
            result = UNDEF;
            break;
    }
    TDM_SEL_CAL(cal_id, cal_main);
    if (result==UNDEF || cal_main==NULL){
        TDM_ERROR2("%s, unrecognized calendar ID %d\n",
           "[Checker: LineRate Jitter]",
           cal_id);
        result = FAIL;
        return result;
    }
    
    /* construct space tracker */
    for (i=0; i<cal_main_len && i<512; i++) {
        port_i = cal_main[i];
        TD2P_TOKEN_CHECK(port_i){
            distance = 0;
            for (j=1; j<cal_main_len; j++) {
                distance++;
                k = ((i+j)<cal_main_len)? (i+j): (i+j-cal_main_len);
                port_k = cal_main[k];
                if (port_i==port_k){
                    tracker[i] = distance;
                    break;
                }
            }
        }
    }
    /* check jitter constraint for Linerate ports */
    for (i=0; i<cal_main_len && i<512; i++) {
        if (tracker[i]==0) {continue;}
        port_i = cal_main[i];
        /* ignore management port */
        if (port_i==13 ||port_i==14 ||port_i==15 ||port_i==16){
            continue;
        }
        TD2P_TOKEN_CHECK(port_i){
            has_checked = BOOL_FALSE;
            for (j=0; j<done_idx && j<TD2P_NUM_EXT_PORTS; j++) {
                if (ports_done[j]==port_i) {
                    has_checked = BOOL_TRUE;
                    break;
                }
            }
            if (has_checked==BOOL_FALSE && done_idx<TD2P_NUM_EXT_PORTS){
                ports_done[done_idx++] = port_i;
                port_speed = port_speeds[port_i];
                slot_req   = tdm_td2p_chk_calc_speed_slots(port_speed);
                /* calculate min/max */
                if (slot_req>0){
                    space_frac = ((cal_main_len*10)/slot_req)%10;
                    jitter_range= (2*cal_main_len)/(slot_req*5);
                    jitter_range= ((((2*cal_main_len*10)/(slot_req*5))%10)<5)?
                                 (jitter_range):
                                 (jitter_range+1);
                    
                    if (space_frac<5){
                        space_min = cal_main_len/slot_req - jitter_range/2;
                        space_max = cal_main_len/slot_req + jitter_range/2;
                        space_max = ((jitter_range%2)==0)?
                                    (space_max):
                                    (space_max+1);
                    }
                    else {
                        space_min = cal_main_len/slot_req - jitter_range/2;
                        space_min = ((cal_main_len%slot_req)==0)?
                                    (space_min):
                                    (space_min+1);
                        space_min = ((jitter_range%2)==0)?
                                    (space_min):
                                    (space_min-1);
                        space_max = cal_main_len/slot_req + jitter_range/2;
                        space_max = ((cal_main_len%slot_req)==0)?
                                    (space_max):
                                    (space_max+1);
                    }
                    space_min = (space_min<1)? (1): (space_min);
                    space_max = (space_max<1)? (1): (space_max);
                }
                
                /* check jitter for each slot of the given port */
                for (j=0; j<cal_main_len && j<512; j++) {
                    port_j = cal_main[j];
                    if (port_i==port_j){
                        /* max */
                        if (tracker[j]>space_max){
                            result = FAIL;
                            TDM_ERROR6("%s, %s, port %d slot %d, %d > %d\n",
                               "[Checker: Linerate Jitter (MAX)]",
                               pipe_str,
                               port_i,
                               j,
                               tracker[j],
                               space_max);
                        }
                        /* min */
                        if (tracker[j]<space_min && tracker[j]>0){
                            result = FAIL;
                            TDM_ERROR6("%s, %s, port %d slot %d, %d < %d\n",
                               "[Checker: Linerate Jitter (MIN)]",
                               pipe_str,
                               port_i,
                               j,
                               tracker[j],
                               space_min);
                        }
                    }
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_jitter_lr
@param:

Checks distribution of linerate tokens for MMU calendars
 */
int 
tdm_td2p_chk_jitter_lr(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* MMU X-pipe */
    pipe_result = tdm_td2p_chk_jitter_lr_pipe(_tdm, TD2P_XPIPE_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU Y-pipe */
    pipe_result = tdm_td2p_chk_jitter_lr_pipe(_tdm, TD2P_YPIPE_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_td2p_chk_jitter_os_pipe
@param:

Checks distribution of oversub tokens for the given pipe
 */
int 
tdm_td2p_chk_jitter_os_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, k, result=PASS;
    int os_cnt, lr_bw=0, os_bw=0, os_bw_ratio;
    const char *pipe_str;
    int param_lr_limit, param_ancl_num, param_cal_len, param_token_ovsb,
        param_os_window, param_os_slots;
    int *param_cal_main=NULL;
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    
    switch(cal_id){
        case TD2P_XPIPE_CAL_ID: pipe_str = "MMU Pipe 0"; break;
        case TD2P_YPIPE_CAL_ID: pipe_str = "MMU Pipe 1"; break;
        default:
            pipe_str = "Unknown Pipe";
            result = UNDEF;
            break;
    }
    TDM_SEL_CAL(cal_id, param_cal_main);
    if (result!=PASS || param_cal_main==NULL){
        TDM_ERROR2("%s, unrecognized calendar ID %d\n",
           "[Checker: OverSub Jitter]",
           cal_id);
        result = FAIL;
        return result;
    }
    
    for (i=0; i<param_cal_len; i++) {
        if (param_cal_main[i]==param_token_ovsb) {
            os_bw+=BW_QUANTA;
        }
        else {
            lr_bw+=BW_QUANTA;
        }
    }
    if (lr_bw==0 || os_bw==0) {
        TDM_PRINT1("TDM: %s - Not applicable\n",pipe_str);
        TDM_SML_BAR
        return result;
    }
    
    TDM_PRINT1("TDM: %s - analyzing oversub distribution...\n",pipe_str);
    
    os_bw_ratio = (os_bw*100)/(os_bw+lr_bw);
    if (os_bw_ratio<=10){
        return PASS;
    }
    else if (os_bw_ratio<=20){
        param_os_window = 6;
        param_os_slots  = 2;
    }
    else if (os_bw_ratio<=30){
        param_os_window = 7;
        param_os_slots  = 3;
    }
    else if (os_bw_ratio<=40){
        param_os_window = 6;
        param_os_slots  = 3;
    }
    else if (os_bw_ratio<=50){
        param_os_window = 7;
        param_os_slots  = 4;
    }
    else if (os_bw_ratio<=60){
        param_os_window = 6;
        param_os_slots  = 4;
    }
    else if (os_bw_ratio<=70){
        param_os_window = 8;
        param_os_slots  = 6;
    }
    else if (os_bw_ratio<=80){
        param_os_window = 7;
        param_os_slots  = 6;
    }
    else if (os_bw_ratio<=90){
        param_os_window = 15;
        param_os_slots  = 14;
    }
    else {
        return PASS;
    }
    
    for (i=0; i<param_cal_len; i++) {
        os_cnt = 0;
        for (j=0; j<param_os_window; j++) {
            k = (i+j)%param_cal_len;
            if (param_cal_main[k]==param_token_ovsb){
                os_cnt++;
            }
        }
        if (os_cnt>param_os_slots){
            result = FAIL;
            TDM_WARN6("%s %s, window [%3d,%3d], ovsb_cnt %2d, ovsb_max %2d\n",
                     "[Checker: OverSub Jitter]",
                     pipe_str,
                     i,
                     (i+param_os_window)%param_cal_len,
                     os_cnt,
                     param_os_slots);
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_chk_jitter_os
@param:

Checks distribution of oversub tokens for MMU calendars
 */
int 
tdm_td2p_chk_jitter_os(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* MMU X-Pipe */
    pipe_result = tdm_td2p_chk_jitter_os_pipe(_tdm, TD2P_XPIPE_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU Y-Pipe */
    pipe_result = tdm_td2p_chk_jitter_os_pipe(_tdm, TD2P_YPIPE_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_td2p_chk_lls
@param:

Checks linked list scheduler
 */
int 
tdm_td2p_chk_lls(tdm_mod_t *_tdm)
{
	int i, j, k, port_phyID, result=PASS, *cal_main=NULL, cal_len, pipe_id;
	const char *table;
	enum port_speed_e *port_speeds;
    
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	
	switch (pipe_id) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 4: table = "MMU Pipe 0"; 
				cal_main=_tdm->_chip_data.cal_4.cal_main; 
				break;
		case 5: table = "MMU Pipe 1"; 
				cal_main=_tdm->_chip_data.cal_5.cal_main; 
				break;
		/*
		case 6: table = "MMU Pipe 2"; break;		
		case 7: table = "MMU Pipe 3"; break; */
		default:
            table = "Unknown Pipe";
            result = UNDEF; 
            TDM_ERROR1("LLS check failed, unrecognized table ID %0d\n",pipe_id);
            break;
	}
	if (result!=PASS || cal_main==NULL) {return result;}
	TDM_PRINT1("Checking min LLS spacing for %s\n", table);
    cal_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + 
              _tdm->_chip_data.soc_pkg.tvec_size;
    
	for (i=0; i<cal_len; i++) {
		port_phyID = cal_main[i];

		TD2P_TOKEN_CHECK(port_phyID){
			if (port_speeds[port_phyID]<SPEED_100G) {
				for (j=1; j<LLS_MIN_SPACING; j++) {
					k = ( (i+j)<cal_len) ? (i+j) : (i+j-cal_len) ;
					if (cal_main[k]==port_phyID) {
						result=FAIL; 
						TDM_ERROR4("LLS check failed in %s, port %0d between [%0d,%0d] violates LLS min spacing requirement\n",table, port_phyID,i,k);
					}
				}
			}
		}
	}
    
	return result;
}

/**
@name: tdm_td2p_chk_chip_specific
@param:

Checks chip specific 
 */
void 
tdm_td2p_chk_chip_specific(tdm_mod_t *_tdm, int fail[15])
{
	int i,j,k, clk, core_bw, accessories, port_spd, port_state, port_phyID, phy_idx_s, phy_idx_e, msub_x0, msub_x1, msub_y0, msub_y1, pgw_x0_len, pgw_x1_len, pgw_y0_len, pgw_y1_len,t2,t3,t4;
	int s1, s1xcnt=0, s1xavg=0, s1ycnt=0, s1yavg=0, s2, s2xcnt=0, s2xavg=0, s2ycnt=0, s2yavg=0, s3, s3xcnt=0, s3xavg=0, s3ycnt=0, s3yavg=0, /*s5=4,*/ s5xcnt=0, s5xavg=0, s5ycnt=0, s5yavg=0, /*s4=5,*/ s4xcnt=0, s4xavg=0, s4ycnt=0, s4yavg=0;
	int pgw_tbl_len, *pgw_tbl_ptr, **ovsb_tbl_ptr, wc_principle, **wc_checker, pgw_mtbl[TD2P_OS_LLS_GRP_LEN*2], pgw_tracker[TD2P_NUM_EXT_PORTS];
	const char *str_pgw_tbl_x0, *str_pgw_tbl_x1,*str_pgw_tbl_y0,*str_pgw_tbl_y1,*str_ovs_tbl_x0,*str_ovs_tbl_x1,*str_ovs_tbl_y0,*str_ovs_tbl_y1;
	char *str_pgw_ptr, *str_ovs_ptr;
	
	/* Initialize */
	str_pgw_tbl_x0 = "PGW table x0";
	str_pgw_tbl_x1 = "PGW table x1";
	str_pgw_tbl_y0 = "PGW table y0";
	str_pgw_tbl_y1 = "PGW table y1";
	str_ovs_tbl_x0 = "PGW OVSB table x0";
	str_ovs_tbl_x1 = "PGW OVSB table x1";
	str_ovs_tbl_y0 = "PGW OVSB table y0";
	str_ovs_tbl_y1 = "PGW OVSB table y1";
	
	pgw_x0_len=0; pgw_x1_len=0; pgw_y0_len=0; pgw_y1_len=0;
	clk = _tdm->_chip_data.soc_pkg.clk_freq;
		
	for (i=0; i<TD2P_OS_LLS_GRP_LEN; i++) {
		if (_tdm->_chip_data.cal_0.cal_main[i]!=TD2P_NUM_EXT_PORTS) pgw_x0_len++;
		if (_tdm->_chip_data.cal_1.cal_main[i]!=TD2P_NUM_EXT_PORTS) pgw_x1_len++;
		if (_tdm->_chip_data.cal_2.cal_main[i]!=TD2P_NUM_EXT_PORTS) pgw_y0_len++;
		if (_tdm->_chip_data.cal_3.cal_main[i]!=TD2P_NUM_EXT_PORTS) pgw_y1_len++;
	}
	
	switch(clk) {
		case 416: case 415: core_bw=480; accessories=10; break;
		case 518: case 517: core_bw=640; accessories=8 ; break;
		case 609: case 608: core_bw=720; accessories=10; break;
		default: 			core_bw=960; accessories=8 ; break;
	}
	TDM_PRINT3("TDM: _____VERBOSE: (clk - %0d) (core bw - %0d) (mmu acc - %0d)\n", clk, core_bw, accessories);
	s3=(((12*((clk/76))))/10); /*40G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 40G MMU spacing as %0d\n", s3);
	s2=(((24*((clk/76))))/10); /*20G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 20G MMU spacing as %0d\n", s2);
	s1=(((495*((clk/76))))/100); /*10G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 10G MMU spacing as %0d\n", s1);	
	
	/* [7] Check length (overflow and symmetry) */
	if (fail[7]==2) {
		msub_x0=0; msub_x1=0; msub_y0=0; msub_y1=0;
		
		for (i=1; i<(TD2P_NUM_EXT_PORTS-1); i++) {
			if((_tdm->_chip_data.soc_pkg.state[i-1]== PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i-1]== PORT_STATE__LINERATE_HG) && 
			   _tdm->_chip_data.soc_pkg.state[i]  != PORT_STATE__COMBINE  &&
			   _tdm->_chip_data.soc_pkg.speed[i]  <  SPEED_10G){
				if     (i>=1  && i<=32 ){msub_x0++;}
				else if(i>=33 && i<=64 ){msub_x1++;}
				else if(i>=65 && i<=96 ){msub_y0++;}
				else if(i>=97 && i<=128){msub_y1++;}
			}
		}

		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table x0 is %0d\n", pgw_x0_len);
		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table x1 is %0d\n", pgw_x1_len);
		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table y0 is %0d\n", pgw_y0_len);
		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table y1 is %0d\n", pgw_y1_len);	
		
		if (msub_x0==0 && msub_x1==0 && msub_y0==0 && msub_y1==0) {
			switch (core_bw) {
				case 960:
                    if (pgw_x0_len>24 || pgw_x1_len>24 || pgw_y0_len>24 || pgw_y1_len>24) {
                        fail[7]=1;
                        if      (pgw_x0_len>24){TDM_ERROR1("PGW table x0 overscheduled, len %0d, len_limit 24\n", pgw_x0_len);}
                        else if (pgw_x1_len>24){TDM_ERROR1("PGW table x1 overscheduled, len %0d, len_limit 24\n", pgw_x1_len);}
                        else if (pgw_y0_len>24){TDM_ERROR1("PGW table y0 overscheduled, len %0d, len_limit 24\n", pgw_y0_len);}
                        else                   {TDM_ERROR1("PGW table y1 overscheduled, len %0d, len_limit 24\n", pgw_y1_len);}
                    }
					if (pgw_x0_len!=pgw_x1_len || pgw_y0_len!=pgw_y1_len) {/*fail[7]=1; */TDM_WARN0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 720:
					if (pgw_x0_len>16 || pgw_y1_len>16) {fail[7]=1; TDM_ERROR0("PGW table short quadrant overscheduled\n");}
					if (pgw_x1_len>20 || pgw_y0_len>20) {fail[7]=1; TDM_ERROR0("PGW table long quadrant overscheduled\n");}
					break;
				case 640:
                    if (pgw_x0_len>16 || pgw_x1_len>16 || pgw_y0_len>16 || pgw_y1_len>16) {
                        fail[7]=1;
                        if      (pgw_x0_len>16){TDM_ERROR1("PGW table x0 overscheduled, len %0d, len_limit 16\n", pgw_x0_len);}
                        else if (pgw_x1_len>16){TDM_ERROR1("PGW table x1 overscheduled, len %0d, len_limit 16\n", pgw_x1_len);}
                        else if (pgw_y0_len>16){TDM_ERROR1("PGW table y0 overscheduled, len %0d, len_limit 16\n", pgw_y0_len);}
                        else                   {TDM_ERROR1("PGW table y1 overscheduled, len %0d, len_limit 16\n", pgw_y1_len);}
                    }
					if (pgw_x0_len!=pgw_x1_len || pgw_y0_len!=pgw_y1_len) {/*fail[7]=1; */TDM_WARN0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 480:
                    if (pgw_x0_len>12 || pgw_x1_len>12 || pgw_y0_len>12 || pgw_y1_len>12) {
                        fail[7]=1;
                        if      (pgw_x0_len>12){TDM_ERROR1("PGW table x0 overscheduled, len %0d, len_limit 12\n", pgw_x0_len);}
                        else if (pgw_x1_len>12){TDM_ERROR1("PGW table x1 overscheduled, len %0d, len_limit 12\n", pgw_x1_len);}
                        else if (pgw_y0_len>12){TDM_ERROR1("PGW table y0 overscheduled, len %0d, len_limit 12\n", pgw_y0_len);}
                        else                   {TDM_ERROR1("PGW table y1 overscheduled, len %0d, len_limit 12\n", pgw_y1_len);}
                    }
					if (pgw_x0_len!=pgw_x1_len || pgw_y0_len!=pgw_y1_len) {/*fail[7]=1; */TDM_WARN0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				default: break;
			}
		}
		
		fail[7]= (fail[7]==2) ? (PASS) : (FAIL);
	}
	
	/* [8] [9] Check pgw table TSC transcription and TSC proximity */
	if (fail[8]==2 && fail[9]==2) {
		/* Length and loopback passes are implicitly combined */
		wc_checker = _tdm->_chip_data.soc_pkg.pmap;
		for (k=0; k<4; k++){
			/* select pgw table */
			switch(k){
				/* pgw_tdm_tbl_x0 */
				case 0:  pgw_tbl_ptr = _tdm->_chip_data.cal_0.cal_main; 
						 pgw_tbl_len = pgw_x0_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_x0;
						 break;
				/* pgw_tdm_tbl_x1 */
				case 1:  pgw_tbl_ptr = _tdm->_chip_data.cal_1.cal_main; 
						 pgw_tbl_len = pgw_x1_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_x1;
						 break;
				/* pgw_tdm_tbl_y0 */
				case 2:  pgw_tbl_ptr = _tdm->_chip_data.cal_2.cal_main; 
						 pgw_tbl_len = pgw_y0_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_y0;
						 break;
				/* pgw_tdm_tbl_y1 */
				default: pgw_tbl_ptr = _tdm->_chip_data.cal_3.cal_main; 
						 pgw_tbl_len = pgw_y1_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_y1;
						 break;
			}
			
			/* check transcription and TSC violations */
			for (i=0; i<pgw_tbl_len; i++) {
				port_phyID = pgw_tbl_ptr[i];
				if(port_phyID>=1 && port_phyID<=TD2P_NUM_PHY_PORTS){
					wc_principle = -1;
					for (j=0; j<TD2P_NUM_PHY_PM; j++) {
						if (wc_checker[j][0] == port_phyID || wc_checker[j][1] == port_phyID || wc_checker[j][2] == port_phyID || wc_checker[j][3] == port_phyID){
							wc_principle = j;
							break;
						}
					}
					if (wc_principle == -1) {
						fail[8] = 1; 
						TDM_ERROR2("TSC transcription corruption %s port %0d\n", str_pgw_ptr, port_phyID);
					}
                    else if (pgw_tbl_len>8){
                        j = (i+1)%pgw_tbl_len;
                        if ( (pgw_tbl_ptr[j]!=TD2P_NUM_PHY_PORTS) &&
                             (pgw_tbl_ptr[j]==wc_checker[wc_principle][0]  ||
                              pgw_tbl_ptr[j]==wc_checker[wc_principle][1]  ||
                              pgw_tbl_ptr[j]==wc_checker[wc_principle][2]  ||
                              pgw_tbl_ptr[j]==wc_checker[wc_principle][3]) ){
                            fail[9]=1; 
                            TDM_ERROR3("TSC proximity violation in %s between index %0d and index %0d\n", str_pgw_ptr, i, j);
                        }
                    }
				}
			}
		}
		fail[8]= (fail[8]==2) ? (PASS) : (FAIL);
		fail[9]= (fail[9]==2) ? (PASS) : (FAIL);
	}
	
	/* [10] Check pgw table spacing*/
	if (fail[10]==2) {
		for (k=0; k<4; k++){
			/* select pgw table */
			switch(k){
				/* pgw_tdm_tbl_x0 */
				case 0:  pgw_tbl_ptr = _tdm->_chip_data.cal_0.cal_main; 
						 pgw_tbl_len = pgw_x0_len; 
						 phy_idx_s   = 1;
						 phy_idx_e   = 32;
						 str_pgw_ptr = (char *) str_pgw_tbl_x0;
						 break;
				/* pgw_tdm_tbl_x1 */
				case 1:  pgw_tbl_ptr = _tdm->_chip_data.cal_1.cal_main; 
						 pgw_tbl_len = pgw_x1_len; 
						 phy_idx_s   = 33;
						 phy_idx_e   = 64;
						 str_pgw_ptr = (char *) str_pgw_tbl_x1;
						 break;
				/* pgw_tdm_tbl_y0 */
				case 2:  pgw_tbl_ptr = _tdm->_chip_data.cal_2.cal_main; 
						 pgw_tbl_len = pgw_y0_len; 
						 phy_idx_s   = 65;
						 phy_idx_e   = 96;
						 str_pgw_ptr = (char *) str_pgw_tbl_y0;
						 break;
				/* pgw_tdm_tbl_y1 */
				default: pgw_tbl_ptr = _tdm->_chip_data.cal_3.cal_main; 
						 pgw_tbl_len = pgw_y1_len; 
						 phy_idx_s   = 97;
						 phy_idx_e   = 128;
						 str_pgw_ptr = (char *) str_pgw_tbl_y1;
						 break;
			}
			t2 = (pgw_tbl_len==(core_bw/40)) ? (((12*clk/76))/10): (pgw_tbl_len/2);
			t3 = (pgw_tbl_len==(core_bw/40)) ? (((6 *clk/76))/10): (pgw_tbl_len/4);
			t4 = 2;
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 20G PGW spacing %0d\n",str_pgw_ptr, t2);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 40G PGW spacing %0d\n",str_pgw_ptr, t3);
			
			/* set tracker */
			for (i=0; i<TD2P_NUM_EXT_PORTS; i++) {pgw_tracker[i]=0;}
			for (i=0; i<pgw_tbl_len; i++) { pgw_mtbl[i]=pgw_tbl_ptr[i]; pgw_mtbl[i+pgw_tbl_len]=pgw_tbl_ptr[i];}
			for (i=0; i<pgw_tbl_len; i++) {
				port_phyID = pgw_tbl_ptr[i];
				if(port_phyID>=1 && port_phyID<=TD2P_NUM_PHY_PORTS){
					pgw_tracker[port_phyID] = 1;
					for (j=1; j<=pgw_tbl_len; j++) {
						if (pgw_mtbl[i+j]!=pgw_mtbl[i]) {pgw_tracker[port_phyID]++; }
						else {break;}
					}
				}
			}
			/* check pgw spacing */
			for (i=phy_idx_s; i<phy_idx_e; i++){
				if(pgw_tracker[i]!=0){
					switch (_tdm->_chip_data.soc_pkg.speed[i]) {
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							if (pgw_tracker[i] != t2) {
								TDM_WARN3("%s, 20G port spacing deviation from average, port %0d,  (%0d)\n", str_pgw_ptr, i, pgw_tracker[i]);
							}
							break;
						case SPEED_42G:
						case SPEED_40G:
							if (pgw_tracker[i] != t3) {
								TDM_WARN3("%s, 40G port spacing deviation from average, port %0d (%0d)\n", str_pgw_ptr, i, pgw_tracker[i]);
							}
							break;
						case SPEED_120G:
							if (pgw_tracker[i] != t4) {
								TDM_WARN3("%s, 120G port spacing deviation from average, port %0d (%0d)\n", str_pgw_ptr, i, pgw_tracker[i]);
							}
							break;
						case SPEED_100G:
							if (pgw_tracker[i] > (t4+1)) {
								TDM_WARN3("%s, 100G port spacing deviation from average, port %0d (%0d)\n", str_pgw_ptr, i, pgw_tracker[i]);
							}
							break;
						default:
							/* 10G/1G spacing is implicitly checked by both the TSC spacing and the spacing of all other speeds */ 
							break;
					}
				
				}
			}
		}
		fail[10]= (fail[10]==2) ? (PASS) : (FAIL);
	}
	
	/* [11] */
	if (fail[11]==2) {
		
		s1xavg = (s1xcnt==0) ? 0 : ((s1xavg*100)/s1xcnt);
		s2xavg = (s2xcnt==0) ? 0 : ((s2xavg*100)/s2xcnt);
		s3xavg = (s3xcnt==0) ? 0 : ((s3xavg*100)/s3xcnt);
		s4xavg = (s4xcnt==0) ? 0 : ((s4xavg*100)/s4xcnt);
		s5xavg = (s5xcnt==0) ? 0 : ((s5xavg*100)/s5xcnt);
		s1yavg = (s1ycnt==0) ? 0 : ((s1yavg*100)/s1ycnt);
		s2yavg = (s2ycnt==0) ? 0 : ((s2yavg*100)/s2ycnt);
		s3yavg = (s3ycnt==0) ? 0 : ((s3yavg*100)/s3ycnt);
		s4yavg = (s4ycnt==0) ? 0 : ((s4yavg*100)/s4ycnt);
		s5yavg = (s5ycnt==0) ? 0 : ((s5yavg*100)/s5ycnt);
		if (s1xavg>0) if ( (s1xavg > (s1*101) || s1xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("10G line rate not within 1 percent tolerance in x pipe, (s1xavg = %0d)\n", s1xavg);}
		if (s1yavg>0) if ( (s1yavg > (s1*101) || s1yavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("10G line rate not within 1 percent tolerance in y pipe, (s1yavg = %0d)\n", s1yavg);}
		if (s2xavg>0) if ( (s2xavg > (s1*101) || s2xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("20G line rate not within 1 percent tolerance in x pipe, (s2xavg = %0d)\n", s2xavg);}
		if (s2yavg>0) if ( (s2yavg > (s1*101) || s2yavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("20G line rate not within 1 percent tolerance in y pipe, (s2yavg = %0d)\n", s2yavg);}
		if (s3xavg>0) if ( (s3xavg > (s1*101) || s3xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("40G line rate not within 1 percent tolerance in x pipe, (s3xavg = %0d)\n", s3xavg);}
		if (s3yavg>0) if ( (s3yavg > (s1*101) || s3yavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("40G line rate not within 1 percent tolerance in y pipe, (s3yavg = %0d)\n", s3yavg);}
		if (s4xavg>0) if ( (s4xavg > (s1*101) || s4xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("100G line rate not within 1 percent tolerance in x pipe, (s4xavg = %0d)\n", s4xavg);}
		if (s4yavg>0) if ( (s4yavg > (s1*101) || s4yavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("100G line rate not within 1 percent tolerance in y pipe, (s4yavg = %0d)\n", s4yavg);}
		if (s5xavg>0) if ( (s5xavg > (s1*101) || s5xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("120G line rate not within 1 percent tolerance in x pipe, (s5xavg = %0d)\n", s5xavg);}
		if (s5yavg>0) if ( (s5yavg > (s1*101) || s5yavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("120G line rate not within 1 percent tolerance in y pipe, (s5yavg = %0d)\n", s5yavg);}
		fail[11]= (fail[11]==2) ? (PASS) : (FAIL);
	}
	
	/* [12] Check 100G/120G port numbering */
	if (fail[12]==2) {
		for (i=1; i<(TD2P_NUM_EXT_PORTS-1); i++) {
			if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G || _tdm->_chip_data.soc_pkg.speed[i]==SPEED_120G) {
				if (i!=1 && i!=21 && i!=33 && i!=53 && i!=65 && i!=85 && i!=97 && i!=117) {
					fail[12]=1; 
					TDM_ERROR1("port number %0d is a 100G/120G port that is improperly subscribed\n",i);
				}
			}
		}
		fail[12]= (fail[12]==2) ? (PASS) : (FAIL);
	}
	
	/* [13] Check port subscription */
	if (fail[13]==2) {
		for(i=1; i<(TD2P_NUM_EXT_PORTS-1); i++){
			/* pgw_x_0 */
			if(i<=32) {
				pgw_tbl_ptr  = _tdm->_chip_data.cal_0.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_0.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_x0;
				str_ovs_ptr  = (char *) str_ovs_tbl_x0;
			}
			/* pgw_x_1 */
			else if(i<=64){
				pgw_tbl_ptr  = _tdm->_chip_data.cal_1.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_1.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_x1;
				str_ovs_ptr  = (char *) str_ovs_tbl_x1;
			}
			/* pgw_y_0 */
			else if(i<=96){
				pgw_tbl_ptr  = _tdm->_chip_data.cal_2.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_2.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_y0;
				str_ovs_ptr  = (char *) str_ovs_tbl_y0;
			}
			/* pgw_y_1 */
			else{
				pgw_tbl_ptr  = _tdm->_chip_data.cal_3.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_3.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_y1;
				str_ovs_ptr  = (char *) str_ovs_tbl_y1;
			}
			
			TD2P_MGMT_CHECK(i){
				k=0;
				port_spd  = _tdm->_chip_data.soc_pkg.speed[i];
				port_state= _tdm->_chip_data.soc_pkg.state[i-1];
				/* line-rate ports */
				if (port_state==PORT_STATE__LINERATE   || 
				    port_state==PORT_STATE__LINERATE_HG){
					switch (port_spd) {
						case SPEED_1G:
						case SPEED_2p5G:
							for (j=0; j<32; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i, str_pgw_ptr, k);}
							break;
						case SPEED_10G:
						case SPEED_10G_XAUI:
							for (j=0; j<32; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							for (j=0; j<32; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_42G_HG2:
						case SPEED_42G:
						case SPEED_40G:
							for (j=0; j<32; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=4) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_120G:
							for (j=0; j<32; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=12) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_100G:
							for (j=0; j<32; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=12 && k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						default:
							break;
					}
				}
				/* oversub ports */
				else if (port_state==PORT_STATE__OVERSUB   ||
				         port_state==PORT_STATE__OVERSUB_HG){
					switch (port_spd) {
						case SPEED_1G:
						case SPEED_2p5G:
							for (j=0; j<32; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_10G:
						case SPEED_10G_XAUI:
							for (j=0; j<32; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							for (j=0; j<32; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_42G_HG2:
						case SPEED_42G:
						case SPEED_40G:
							for (j=0; j<32; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=4) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						default:
							break;
					}
				}
			}
		}
		fail[13]= (fail[13]==2) ? (PASS) : (FAIL);
	}
}


/**
@name: tdm_td2p_chk
@param: 
 */
int
tdm_td2p_chk( tdm_mod_t *_tdm)
{
    int i, result, fail[15];
    
    /* Enable TDM C checkers */
    for(i=0;i<15; i++){fail[i]=2;}
    
    TDM_BIG_BAR
    TDM_PRINT0("TDM: TDM Self Check\n\n");
    
    /* [0]: Check structure:  */
    if (fail[0]==2) {
        TDM_PRINT0("TDM: Checking Structure (length, frequency)\n");
        TDM_SML_BAR
        fail[0] = tdm_td2p_chk_struct(_tdm);
    }
    /* [1]: Check port transcription */
    if (fail[1]==2) {
        TDM_PRINT0("\nTDM: Checking Port Transcription\n"); 
        TDM_SML_BAR
        fail[1] = tdm_td2p_chk_transcription(_tdm);
    }
    /* [2]: Check min same port spacing */
    if (fail[2]==2) {
        TDM_PRINT0("\nTDM: Checking Same Port Spacing\n"); 
        TDM_SML_BAR
        /* fail[2] = tdm_td2p_chk_samePortMin(_tdm); */
    }
    /* [3]: Check sister port spacing */
    if (fail[3]==2) {
        TDM_PRINT0("\nTDM: Checking Sister Port Spacing\n"); 
        TDM_SML_BAR
        fail[3] = tdm_td2p_chk_sisterMin(_tdm);
    }
    
    /* [4]: Check port subscription: CPU/loopback/ACC and regular port */
    if (fail[4]==2) {
        TDM_PRINT0("\nTDM: Checking Port Rate/Bandwidth \n"); 
        TDM_SML_BAR
        fail[4] = tdm_td2p_chk_subscription(_tdm);
    }
    /* [5]: Check linerate jitter */
    if (fail[5]==2) {
        TDM_PRINT0("\nTDM: Checking LineRate Jitter \n"); 
        TDM_SML_BAR
        fail[5] = tdm_td2p_chk_jitter_lr(_tdm);
    }
    /* [6]: Check oversub jitter */
    if (fail[6]==2) {
        TDM_PRINT0("\nTDM: Checking OverSub Jitter \n"); 
        TDM_SML_BAR
        fail[6] = tdm_td2p_chk_jitter_os(_tdm);
    }
    /* [14] Check LLS min spacing: 1-in-11 */
    if (fail[14]==2 && _tdm->_chip_data.soc_pkg.clk_freq>=760) {
        TDM_PRINT0("\nTDM: [14] Checking LLS min spacing requirement\n"); 
        TDM_SML_BAR
        /* X Pipe */
        _tdm->_core_data.vars_pkg.pipe = TD2P_XPIPE_CAL_ID;
        result = tdm_td2p_chk_lls(_tdm);
        fail[14] = (fail[14]==FAIL || fail[14]==UNDEF) ? (fail[14]):(result);
        /* Y Pipe */
        _tdm->_core_data.vars_pkg.pipe = TD2P_YPIPE_CAL_ID;
        result = tdm_td2p_chk_lls(_tdm);
        fail[14] = (fail[14]==FAIL || fail[14]==UNDEF) ? (fail[14]):(result);
    }
    /* [7] [8] [9] [10] [11] [12] [13] Check chip specific constraints */
    if (fail[7]==2 ||
        fail[8]==2 ||
        fail[9]==2 ||
        fail[10]==2||
        fail[11]==2||
        fail[12]==2||
        fail[13]==2){
        TDM_PRINT1("\nTDM: --- Chip specific checkers %s\n",
            "[7] [8] [9] [10] [11] [12] [13]"); 
        TDM_SML_BAR
        tdm_td2p_chk_chip_specific(_tdm, fail);
    }
    
    
    /* Summarize check results */
    TDM_PRINT0("\n");
    TDM_SML_BAR
    if (fail[14]==FAIL ||
        fail[13]==FAIL ||
        fail[12]==FAIL ||
        fail[11]==FAIL ||
        /* fail[10]==FAIL || */
        fail[9]==FAIL ||
        fail[8]==FAIL ||
        fail[7]==FAIL ||
        /* fail[6]==FAIL || */
        fail[5]==FAIL ||
        fail[4]==FAIL ||
        fail[3]==FAIL ||
        fail[2]==FAIL ||
        fail[1]==FAIL ||
        fail[0]==FAIL ){
        TDM_PRINT0("TDM: *** FAILED ***\n");
        TDM_SML_BAR
        TDM_PRINT0("TDM: Fail vector: [");
        for (i=0; i<15; i++) {
            TDM_PRINT1(" %0d ",fail[i]);
        }
        TDM_PRINT0("]\n");
    }
    else {
        TDM_PRINT0("TDM: *** PASSED ***\n");
    }
    TDM_SML_BAR
    TDM_PRINT0("\n");
    TDM_PRINT0("TDM: TDM Self Check Complete.\n");
    TDM_BIG_BAR

    return PASS;
}
