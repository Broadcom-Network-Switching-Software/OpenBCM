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
@name: tdm_gh2_chk_get_port_tsc
@param:

Returns the TSC to which the input port belongs
 */
int
tdm_gh2_chk_get_port_tsc(tdm_mod_t *_tdm, int port)
{
    int i, j, port_tsc;
    int num_ext_ports;
    int num_pm_lanes;
    int num_phy_pm;
    int **port_pmap;
    
    num_ext_ports= _tdm->_chip_data.soc_pkg.num_ext_ports;
    num_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    num_phy_pm   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    port_pmap    = _tdm->_chip_data.soc_pkg.pmap;
    
    port_tsc = num_ext_ports;
    GH2_TOKEN_CHECK(port) {
        for (i=0; i<num_phy_pm; i++) {
            for (j=0; j<num_pm_lanes; j++) {
                if (port==port_pmap[i][j]){
                    port_tsc = i;
                    break;
                }
            }
            if (port_tsc!=num_ext_ports){
                break;
            }
        }
    }
    
    return port_tsc;
}


/**
@name: tdm_gh2_chk_get_cal_len
@param:

Returns the length of selected calendar
 */
int
tdm_gh2_chk_get_cal_len(tdm_mod_t *_tdm, int cal_id)
{
    int i, idx_s=0, idx_e=0, higig_port, higig_mgmt, higig_boot, cal_len=0,
        cnt_100g=0, cnt_40g=0;
    int clk_freq;
    int *cal_main=NULL;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;
    
    clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
    port_speeds= _tdm->_chip_data.soc_pkg.speed;
    port_states= _tdm->_chip_data.soc_pkg.state;
    
    higig_port = BOOL_FALSE;
    higig_mgmt = BOOL_FALSE;
    higig_boot = BOOL_FALSE;
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: idx_s=1;  idx_e=32; break;
        case GH2_IDB_PIPE_1_CAL_ID: idx_s=33; idx_e=64; break;
        case GH2_IDB_PIPE_2_CAL_ID: idx_s=65; idx_e=96; break;
        case GH2_IDB_PIPE_3_CAL_ID: idx_s=97; idx_e=128;break;
        case GH2_MMU_PIPE_0_CAL_ID: idx_s=1;  idx_e=32; break;
        case GH2_MMU_PIPE_1_CAL_ID: idx_s=33; idx_e=64; break;
        case GH2_MMU_PIPE_2_CAL_ID: idx_s=65; idx_e=96; break;
        case GH2_MMU_PIPE_3_CAL_ID: idx_s=97; idx_e=128;break;
        default:                   idx_s=0;  idx_e=0;  break;
    }
    TDM_SEL_CAL(cal_id, cal_main);
    if (idx_s!=0 && idx_e!=0 && cal_main!=NULL){
        /* check regular ports */
        for (i=idx_s; i<=idx_e; i++){
            if ( (port_speeds[i]>SPEED_0) &&
                 (port_states[i-1]==PORT_STATE__LINERATE_HG ||
                  port_states[i-1]==PORT_STATE__OVERSUB_HG   ) ){
                higig_port = BOOL_TRUE;
                break;
            }
        }
        /* check management port */
        if ( (_tdm->_chip_data.soc_pkg.soc_vars.gh2.mgmt_pm_hg==BOOL_TRUE) &&
             (cal_id==1 || cal_id==2) &&
             ( (cal_main[i]!=GH2_NUM_EXT_PORTS && clk_freq>=MIN_HG_FREQ)||
               (cal_main[i]==GH2_NUM_EXT_PORTS) ) ){
            higig_mgmt = BOOL_TRUE;
        }
        /* check booting time cal_type */
        if (_tdm->_chip_data.soc_pkg.soc_vars.gh2.cal_hg_en==BOOL_TRUE){
            higig_boot = BOOL_TRUE;
        }
        
        /* set calendar length */
        if (higig_port==BOOL_FALSE && 
            higig_mgmt==BOOL_FALSE && 
            higig_boot==BOOL_FALSE ){
            switch (clk_freq){
                case GH2_CLK_950MHZ: cal_len = GH2_LEN_950MHZ_EN; break;
                case GH2_CLK_850MHZ: cal_len = GH2_LEN_850MHZ_EN; break;
                case GH2_CLK_765MHZ: cal_len = GH2_LEN_765MHZ_EN; break;
                case GH2_CLK_672MHZ: cal_len = GH2_LEN_672MHZ_EN; break;
                case GH2_CLK_585MHZ: cal_len = GH2_LEN_585MHZ_EN; break;
                case GH2_CLK_545MHZ: cal_len = GH2_LEN_545MHZ_EN; break;
                default:  cal_len = 0; break;
            }
        }
        else {
            switch (clk_freq){
                case GH2_CLK_950MHZ: cal_len = GH2_LEN_950MHZ_HG; break;
                case GH2_CLK_850MHZ: cal_len = GH2_LEN_850MHZ_HG; break;
                case GH2_CLK_765MHZ: cal_len = GH2_LEN_765MHZ_HG; break;
                case GH2_CLK_672MHZ: cal_len = GH2_LEN_672MHZ_HG; break;
                case GH2_CLK_585MHZ: cal_len = GH2_LEN_585MHZ_HG; break;
                case GH2_CLK_545MHZ: cal_len = GH2_LEN_545MHZ_HG; break;
                default:  cal_len = 0; break;
            }
        }
    }
    
    /* SPECIAL CASE: 4x106HG+3x40G uses a unique TDM in Tomahawk*/
    _tdm->_core_data.vars_pkg.HG4X106G_3X40G = BOOL_FALSE;
    if ((clk_freq==GH2_CLK_850MHZ) && 
        (higig_port==BOOL_TRUE ||
         higig_mgmt==BOOL_TRUE || 
         higig_boot==BOOL_TRUE ) ){
        for (i=idx_s; i<=idx_e; i++){
            if (port_states[i-1]==PORT_STATE__LINERATE   ||
                port_states[i-1]==PORT_STATE__LINERATE_HG){
                if (port_speeds[i]==SPEED_100G || 
                    port_speeds[i]==SPEED_106G){
                    cnt_100g++;
                }
                else if (port_speeds[i]==SPEED_40G || 
                         port_speeds[i]==SPEED_42G ||
                         port_speeds[i]==SPEED_42G_HG2){
                    cnt_40g++;
                }
            }
        }
        if (cnt_100g==4 && cnt_40g==3){
            _tdm->_core_data.vars_pkg.HG4X106G_3X40G = BOOL_TRUE;
            _tdm->_chip_data.soc_pkg.tvec_size   = 9;
            _tdm->_chip_data.soc_pkg.lr_idx_limit= 201;
            cal_len = _tdm->_chip_data.soc_pkg.tvec_size +
                      _tdm->_chip_data.soc_pkg.lr_idx_limit;
        }
    }
    
    return cal_len;
}


/**
@name: tdm_gh2_chk_get_speed_slot_num
@param:

Return 
 */
int 
tdm_gh2_chk_get_speed_slot_num(int port_speed)
{
    int slot_num, param_spd=0;
    
    switch(port_speed){
        case SPEED_1G:
        case SPEED_2p5G:
            param_spd = 1;
            break;
        case SPEED_10G:
        case SPEED_11G:
            param_spd = 10;  
            break;
        case SPEED_20G:
        case SPEED_21G:
            param_spd = 20;  
            break;
        case SPEED_25G:
        case SPEED_27G:
            param_spd = 25;  
            break;
        case SPEED_40G:  
        case SPEED_42G:
        case SPEED_42G_HG2:
            param_spd = 40;  
            break;
        case SPEED_50G:
        case SPEED_53G:
            param_spd = 50;  
            break;
        case SPEED_100G:
        case SPEED_106G:
            param_spd = 100; 
            break;
        case SPEED_120G:
        case SPEED_127G:
            param_spd = 120; 
            break;
        default:
            TDM_ERROR4("%s, %s -- %dM (%dG)\n",
                       "[Speed]",
                       "invalided port speed",
                       port_speed, port_speed/1000);
            break;
    }
    if (param_spd==1){
        slot_num = 1;
    }
    else {
        slot_num = (param_spd*10)/BW_QUANTA;
    }
    
    return slot_num;
}


/**
@name: tdm_gh2_chk_get_cal_token_num
@param:

Return number of slots of the given token in selected calendar
 */
int 
tdm_gh2_chk_get_cal_token_num(tdm_mod_t *_tdm, int cal_id, int port_token)
{
    int i, cnt=0;
    int *cal_main=NULL;
    int cal_main_len;
    
    TDM_SEL_CAL(cal_id, cal_main);
    cal_main_len = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
    
    if (cal_main!=NULL){
        for (i=0; i<cal_main_len; i++){
            if (cal_main[i]==port_token){
                cnt++;
            }
        }
    }
    return cnt;
}


/**
@name: tdm_gh2_chk_is_pipe_ethernet
@param:

Returns BOOL_TRUE if all (active) ports of the given pipe have traffic 
entirely Ethernet, otherwise returns FALSE.
 */
int 
tdm_gh2_chk_is_pipe_ethernet(tdm_mod_t *_tdm) 
{
    int i, idx_s=0, idx_e=0, result=BOOL_TRUE;
    int pipe_id;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;

    pipe_id     = _tdm->_core_data.vars_pkg.pipe;
    port_speeds = _tdm->_chip_data.soc_pkg.speed;
    port_states = _tdm->_chip_data.soc_pkg.state;

    switch(pipe_id){
        case GH2_MMU_PIPE_0_CAL_ID: idx_s =1;  idx_e =32;  break;
        case GH2_MMU_PIPE_1_CAL_ID: idx_s =33; idx_e =64;  break;
        case GH2_MMU_PIPE_2_CAL_ID: idx_s =65; idx_e =96;  break;
        case GH2_MMU_PIPE_3_CAL_ID: idx_s =97; idx_e =128; break;
        default:                   idx_s =0;  idx_e =0;   break;
    }
    if(idx_s>0 && idx_e<GH2_NUM_EXT_PORTS){
        for (i=idx_s; i<=idx_e; i++) {
            if (port_speeds[i]!=SPEED_0){
                if (port_states[i-1]==PORT_STATE__LINERATE_HG ||
                    port_states[i-1]==PORT_STATE__OVERSUB_HG  ){
                    result = BOOL_FALSE;
                    break;
                }
            }
        }
    }

    return result;
}


/**
@name: tdm_gh2_chk_struct_freq
@param:

Check frequency validity
 */
int 
tdm_gh2_chk_struct_freq(tdm_mod_t *_tdm)
{
    int result=PASS;
    int freq;
    
    freq    = _tdm->_chip_data.soc_pkg.clk_freq;
    
    switch (freq) {
        case GH2_CLK_950MHZ:
        case GH2_CLK_850MHZ:
        case GH2_CLK_765MHZ:
        case GH2_CLK_672MHZ:
        case GH2_CLK_585MHZ:
        case GH2_CLK_545MHZ:
            break;
        default:
            TDM_ERROR1("Unrecognized frequency %d\n", freq);
            result = UNDEF;
            break;
    }
    return result;
}


/**
@name: tdm_gh2_chk_struct_calLength
@param:

Checks calendar length of all MMU/IDB pipes
 */
int 
tdm_gh2_chk_struct_calLength(tdm_mod_t *_tdm)
{
    int i, len_max=0, pipe_idx=0, result=PASS;
    int cal_id;
    int cal_main_len;
    int *cal_main=NULL;
    
    /* check calendar length for each pipe */
    for (cal_id=0; cal_id<8; cal_id++){
        switch(cal_id){
            case GH2_IDB_PIPE_0_CAL_ID: pipe_idx = 0; break;
            case GH2_IDB_PIPE_1_CAL_ID: pipe_idx = 1; break;
            case GH2_IDB_PIPE_2_CAL_ID: pipe_idx = 2; break;
            case GH2_IDB_PIPE_3_CAL_ID: pipe_idx = 3; break;
            case GH2_MMU_PIPE_0_CAL_ID: pipe_idx = 0; break;
            case GH2_MMU_PIPE_1_CAL_ID: pipe_idx = 1; break;
            case GH2_MMU_PIPE_2_CAL_ID: pipe_idx = 2; break;
            case GH2_MMU_PIPE_3_CAL_ID: pipe_idx = 3; break;
/*             default:
                pipe_idx = 0;
                result   = (result==FAIL)?(result):(UNDEF);
                break; */
        }
        /* calendar length limit */
        len_max = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
        /* actual calendar length */
        TDM_SEL_CAL(cal_id, cal_main);
        cal_main_len = GH2_LR_VBS_LEN;
        for (i=(GH2_LR_VBS_LEN-1); i>=0; i--){
            if(cal_main[i]==GH2_NUM_EXT_PORTS){
                cal_main_len--;
            }
            else{
                break;
            }
        }
        /* checker */
        if (cal_main_len>len_max) {
            result=FAIL;
            TDM_ERROR5("%s, %s %d, length %d, limit %d\n",
                "Calendar length exceeded",
                "MMU/IDB pipe", 
                pipe_idx, cal_main_len, len_max);
        }
    }

    return result;
}


/**
@name: tdm_gh2_chk_struct_speed
@param:

Check if speed type is supported
 */
int 
tdm_gh2_chk_struct_speed(tdm_mod_t *_tdm)
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
                case SPEED_20G: case SPEED_21G:
                case SPEED_25G: case SPEED_27G:
                case SPEED_40G: case SPEED_42G: case SPEED_42G_HG2:
                case SPEED_50G: case SPEED_53G:
                case SPEED_100G:case SPEED_106G:
                    port_result = PASS;
                    break;
                default:
                    port_result = UNDEF;
                    TDM_ERROR3("%s, unrecognized speed %dG for port %d\n",
                        "[Structure-Speed]",
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
@name: tdm_gh2_chk_struct
@param:

Checks length, frequency, and speed type
 */
int tdm_gh2_chk_struct(tdm_mod_t *_tdm)
{
    int result_x, result=PASS;
    /* frequency */
    result_x = tdm_gh2_chk_struct_freq(_tdm);
    result = (result_x!=PASS)? (result_x): (result);
    /* calendar length */
    result_x = tdm_gh2_chk_struct_calLength(_tdm);
    result = (result_x!=PASS)? (result_x): (result);
    /* speed */
    result_x = tdm_gh2_chk_struct_speed(_tdm);
    result = (result_x!=PASS)? (result_x): (result);
    
    return result;
}


/**
@name: tdm_gh2_chk_transcription
@param:

Verifies TSC transcription caught all ports indexed by port_state_array
 */
int 
tdm_gh2_chk_transcription(tdm_mod_t *_tdm)
{
    int i, port_phynum, port_speed, port_state, port_tsc, port_lanes,
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
        if (port_speed>SPEED_0 && 
            (port_state==PORT_STATE__LINERATE    || 
             port_state==PORT_STATE__LINERATE_HG ||
             port_state==PORT_STATE__OVERSUB     ||
             port_state==PORT_STATE__OVERSUB_HG  ||
             port_state==PORT_STATE__MANAGEMENT ) ){
            port_result = PASS;
            port_tsc = tdm_gh2_chk_get_port_tsc(_tdm, port_phynum);
            
            if (!(port_tsc<num_phy_pm)){
                port_result = FAIL;
                TDM_ERROR2("%s, un-transcribed port %0d\n",
                    "FAILED in TDM check, [Port Transcription]", 
                    port_phynum);
            }
            else{
                port_lanes = 0;
                for (i=0; i<num_pm_lanes; i++){
                    if(port_phynum==port_pmap[port_tsc][i]){
                        port_lanes++;
                    }
                }
                switch(port_speed){
                    case SPEED_1G:
                    case SPEED_2p5G:
                    case SPEED_10G:
                    case SPEED_11G:
                    case SPEED_20G:
                    case SPEED_21G:
                    case SPEED_25G:
                    case SPEED_27G:
                        if (port_lanes!=1){
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
                    default:
                        port_result = UNDEF;
                        TDM_ERROR3("%s, unrecognized speed %dG for port %d\n",
                            "[Port Transcription]",
                            (port_speed/1000),port_phynum);
                        break;
                }
                if (port_result==FAIL){
                    TDM_ERROR4("%s, %s, port %d speed %dG\n",
                            "[Port Transcription]",
                            "illegal TSC configuration",
                            port_phynum, (port_speed/1000));
                }
            }
            
            if (port_result!=PASS){
                result = (result==PASS)?(port_result):(result);
            }
        }
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_sister
@param:

Checks sister spacing constraint
 */
int 
tdm_gh2_chk_sister(tdm_mod_t *_tdm)
{
    int i, j, k, cal_id, port_i, port_k, tsc_i, tsc_k, 
        port_result, result=PASS;
    int *cal_main=NULL;
    int cal_main_len;
    int sister_min;
   
    sister_min   = _tdm->_core_data.rule__prox_port_min;
    
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
                    "[SISTER Spacing]", 
                    cal_id);
                break; */
        }
        
        /* get calendar length */
        cal_main_len = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
         
        /* check all slots within calendar */
        if (cal_main!=NULL){
            for (i=0; i<cal_main_len; i++){
                port_result = PASS;
                port_i = cal_main[i];
                GH2_TOKEN_CHECK(port_i){
                    tsc_i = tdm_gh2_chk_get_port_tsc(_tdm, port_i);
                    for (j=1; j<sister_min; j++){
                        k = ((i+j)<cal_main_len)?(i+j):(i+j-cal_main_len);
                        port_k = cal_main[k];
                        tsc_k  = tdm_gh2_chk_get_port_tsc(_tdm,port_k);
                        if (tsc_i==tsc_k){
                            port_result = FAIL;
                            TDM_ERROR5("%s, port[%3d,%3d], index[%3d,%3d],\n",
                                "[SISTER Spacing]",
                                port_i, port_k, i, k);
                        }
                    }
                }
                result = (result==PASS)?(port_result):(result);
            }
        }
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_sub_cpu
@param:

Check bandwidth for CPU and/or Management port in MMU/IDB calendars
 */
int 
tdm_gh2_chk_sub_cpu(tdm_mod_t *_tdm)
{
    int slot_cnt, result=PASS;
    
    /* IDB Pipe 0: CPU port */
    slot_cnt = tdm_gh2_chk_get_cal_token_num(
                    _tdm, GH2_IDB_PIPE_0_CAL_ID, GH2_CMIC_TOKEN);
    if (slot_cnt<1){
        result = FAIL;
        TDM_ERROR3("%s, %s - %d slots\n",
                "[Port Subscription]",
                "IDB Pipe 0, insufficient CPU port bandwidth",
                slot_cnt);
    }  
    return result;
}


/**
@name: tdm_gh2_chk_sub_lpbk
@param:

Check bandwidth for Loopback port in MMU/IDB calendars
 */
int 
tdm_gh2_chk_sub_lpbk(tdm_mod_t *_tdm)
{   
    int result = PASS;

    return result;
}


/**
@name: tdm_gh2_chk_sub_acc
@param:

Check bandwidth for overall auxiliary port in MMU/IDB calendars
 */
int 
tdm_gh2_chk_sub_acc(tdm_mod_t *_tdm)
{
    int slot_cnt=0, ovsb_slot_cnt, result=PASS;
    int acc_num;
    
    acc_num = _tdm->_chip_data.soc_pkg.tvec_size;
    
    /* IDB Pipe 0 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_0_CAL_ID, GH2_CMIC_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_0_CAL_ID, GH2_LPB0_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_0_CAL_ID, GH2_IDL1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_0_CAL_ID, GH2_IDL2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_0_CAL_ID, GH2_NULL_TOKEN);
    if (slot_cnt<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "IDB Pipe 0, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    /* IDB Pipe 1 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_1_CAL_ID, GH2_MGM1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_1_CAL_ID, GH2_LPB1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_1_CAL_ID, GH2_IDL1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_1_CAL_ID, GH2_IDL2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_1_CAL_ID, GH2_NULL_TOKEN);
    if (slot_cnt<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "IDB Pipe 1, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    /* IDB Pipe 2 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_2_CAL_ID, GH2_MGM2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_2_CAL_ID, GH2_LPB2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_2_CAL_ID, GH2_IDL1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_2_CAL_ID, GH2_IDL2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_2_CAL_ID, GH2_NULL_TOKEN);
    if (slot_cnt<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "IDB Pipe 2, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    /* IDB Pipe 3 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_3_CAL_ID, GH2_LPB3_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_3_CAL_ID, GH2_IDL1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_3_CAL_ID, GH2_IDL2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_3_CAL_ID, GH2_NULL_TOKEN);
    ovsb_slot_cnt = tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_IDB_PIPE_3_CAL_ID, GH2_OVSB_TOKEN);
    if (slot_cnt<acc_num && (slot_cnt+ovsb_slot_cnt)<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "IDB Pipe 3, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    
    /* MMU Pipe 0 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_0_CAL_ID, GH2_CMIC_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_0_CAL_ID, GH2_LPB0_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_0_CAL_ID, GH2_NULL_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_0_CAL_ID, GH2_IDL2_TOKEN);
    if (slot_cnt<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "MMU Pipe 0, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    /* MMU Pipe 1 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_1_CAL_ID, GH2_MGM1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_1_CAL_ID, GH2_LPB1_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_1_CAL_ID, GH2_NULL_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_1_CAL_ID, GH2_IDL2_TOKEN);
    if (slot_cnt<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "MMU Pipe 1, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    /* MMU Pipe 2 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_2_CAL_ID, GH2_MGM2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_2_CAL_ID, GH2_LPB2_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_2_CAL_ID, GH2_NULL_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_2_CAL_ID, GH2_IDL2_TOKEN);
    if (slot_cnt<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "MMU Pipe 2, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    /* MMU Pipe 3 */
    slot_cnt=0;
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_3_CAL_ID, GH2_LPB3_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_3_CAL_ID, GH2_NULL_TOKEN);
    slot_cnt+= tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_3_CAL_ID, GH2_IDL2_TOKEN);
    ovsb_slot_cnt = tdm_gh2_chk_get_cal_token_num(
                _tdm, GH2_MMU_PIPE_3_CAL_ID, GH2_OVSB_TOKEN);
    if (slot_cnt<acc_num && (slot_cnt+ovsb_slot_cnt)<acc_num){
        result = FAIL;
        TDM_ERROR4("%s, %s - allocated %d slots, required %d slots\n",
                "[Port Subscription]",
                "MMU Pipe 3, insufficient AUXILIARY port bandwidth",
                slot_cnt, acc_num);
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_sub_lr_pipe
@param:

Check bandwidth for linerate ports for the given pipe
 */
int 
tdm_gh2_chk_sub_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, port_speed, port_state, slot_req, slot_cnt=0, idx_s=0, idx_e=0,
        result=PASS;
    int num_40g=0, num_100g=0, case_gh2_4x100g_3x40g=BOOL_FALSE;
    const char *pipe_str;
    int clk_freq;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;
    
    clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
    port_speeds= _tdm->_chip_data.soc_pkg.speed;
    port_states= _tdm->_chip_data.soc_pkg.state;
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: 
            pipe_str = "IDB Pipe 0"; idx_s=1;  idx_e=32; break;
        case GH2_IDB_PIPE_1_CAL_ID: 
            pipe_str = "IDB Pipe 1"; idx_s=33; idx_e=64; break;
        case GH2_IDB_PIPE_2_CAL_ID: 
            pipe_str = "IDB Pipe 2"; idx_s=65; idx_e=96; break;
        case GH2_IDB_PIPE_3_CAL_ID: 
            pipe_str = "IDB Pipe 3"; idx_s=97; idx_e=128;break;
        case GH2_MMU_PIPE_0_CAL_ID: 
            pipe_str = "MMU Pipe 0"; idx_s=1;  idx_e=32; break;
        case GH2_MMU_PIPE_1_CAL_ID: 
            pipe_str = "MMU Pipe 1"; idx_s=33; idx_e=64; break;
        case GH2_MMU_PIPE_2_CAL_ID: 
            pipe_str = "MMU Pipe 2"; idx_s=65; idx_e=96; break;
        case GH2_MMU_PIPE_3_CAL_ID: 
            pipe_str = "MMU Pipe 3"; idx_s=97; idx_e=128;break;
        default:
            pipe_str = "Unknown Pipe";
            idx_s = 0;
            idx_e = 0;
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized calendar ID %d\n",
               "[Linerate Subscription]",
               cal_id);
            break;
    }
    
    /* SPECIAL CASE: 4x106HG+3x40G uses a unique TDM in Tomahawk */
    if (result==PASS){
        if (clk_freq==GH2_CLK_850MHZ){
            for (i=idx_s; i<=idx_e; i++){
                port_state = port_states[i-1];
                port_speed = port_speeds[i];
                if (port_state==PORT_STATE__LINERATE    ||
                    port_state==PORT_STATE__LINERATE_HG){
                    if (port_speed==SPEED_40G||
                        port_speed==SPEED_42G||
                        port_speed==SPEED_42G_HG2){
                        num_40g++;
                    }
                    else if (port_speed==SPEED_100G||
                             port_speed==SPEED_106G){
                        num_100g++;
                    }
                }
            }
            if (num_40g==3 && num_100g==4){
                case_gh2_4x100g_3x40g = BOOL_TRUE;
            }
        }
    }
    
    /* special case: 4x106HG+3x40G */
    if (result==PASS && case_gh2_4x100g_3x40g==BOOL_TRUE){
        for (i=idx_s; i<=idx_e; i++){
            port_state = port_states[i-1];
            port_speed = port_speeds[i];
            if (port_state==PORT_STATE__LINERATE    ||
                port_state==PORT_STATE__LINERATE_HG ){
                slot_req = tdm_gh2_chk_get_speed_slot_num(port_speed);
                slot_cnt = tdm_gh2_chk_get_cal_token_num(_tdm, cal_id, i);
                if ( slot_cnt<(slot_req-1) ){
                    result = FAIL;
                    TDM_ERROR7("%s %s %s, port %d, speed %dG, slots %d/%d\n",
                       "[Linerate Subscription]",
                       pipe_str,
                       "insufficient port bandwidth",
                       i, port_speed/1000, slot_cnt, slot_req);
                }
            }
        }
    }
    /* regular case */
    else if (result==PASS) {
        for (i=idx_s; i<=idx_e; i++){
            port_state = port_states[i-1];
            port_speed = port_speeds[i];
            if (port_state==PORT_STATE__LINERATE    ||
                port_state==PORT_STATE__LINERATE_HG ){
                slot_req = tdm_gh2_chk_get_speed_slot_num(port_speed);
                slot_cnt = tdm_gh2_chk_get_cal_token_num(_tdm, cal_id, i);
                if (slot_cnt<slot_req){
                    result = FAIL;
                    TDM_ERROR7("%s %s %s, port %d, speed %dG, slots %d/%d\n",
                       "[Linerate Subscription]",
                       pipe_str,
                       "insufficient port bandwidth",
                       i, port_speed/1000, slot_cnt, slot_req);
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_sub_os_pipe
@param:

Check bandwidth for Oversub ports for the given pipe
 */
int 
tdm_gh2_chk_sub_os_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, k, port_speed, port_state, port_phynum,
        idx_s=0, idx_e=0, port_is_found=BOOL_FALSE, port_speed_0, 
        result=PASS;
    const char *pipe_str;
    int cal_grp_num;
    int cal_grp_len;
    int **cal_grp;
    enum port_speed_e *port_speeds;
    enum port_state_e *port_states;
    
    port_speeds= _tdm->_chip_data.soc_pkg.speed;
    port_states= _tdm->_chip_data.soc_pkg.state;
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: 
            pipe_str = "IDB Pipe 0"; idx_s = 1; idx_e = 32;
            cal_grp_num = _tdm->_chip_data.cal_0.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_0.grp_len;
            cal_grp     = _tdm->_chip_data.cal_0.cal_grp;
            break;
        case GH2_IDB_PIPE_1_CAL_ID: 
            pipe_str = "IDB Pipe 1"; idx_s = 33; idx_e = 64;
            cal_grp_num = _tdm->_chip_data.cal_1.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_1.grp_len;
            cal_grp     = _tdm->_chip_data.cal_1.cal_grp;
            break;
        case GH2_IDB_PIPE_2_CAL_ID: 
            pipe_str = "IDB Pipe 2"; idx_s = 65; idx_e = 96;
            cal_grp_num = _tdm->_chip_data.cal_2.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_2.grp_len;
            cal_grp     = _tdm->_chip_data.cal_2.cal_grp;            
            break;
        case GH2_IDB_PIPE_3_CAL_ID: 
            pipe_str = "IDB Pipe 3"; idx_s = 97; idx_e = 128;
            cal_grp_num = _tdm->_chip_data.cal_3.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_3.grp_len;
            cal_grp     = _tdm->_chip_data.cal_3.cal_grp;
            break;
        case GH2_MMU_PIPE_0_CAL_ID: 
            pipe_str = "MMU Pipe 0";  idx_s = 1; idx_e = 32;
            cal_grp_num = _tdm->_chip_data.cal_4.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_4.grp_len;
            cal_grp     = _tdm->_chip_data.cal_4.cal_grp;
            break;
        case GH2_MMU_PIPE_1_CAL_ID: 
            pipe_str = "MMU Pipe 1"; idx_s = 33; idx_e = 64;
            cal_grp_num = _tdm->_chip_data.cal_5.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_5.grp_len;
            cal_grp     = _tdm->_chip_data.cal_5.cal_grp;
            break;
        case GH2_MMU_PIPE_2_CAL_ID: 
            pipe_str = "MMU Pipe 2"; idx_s = 65; idx_e = 96;
            cal_grp_num = _tdm->_chip_data.cal_6.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_6.grp_len;
            cal_grp     = _tdm->_chip_data.cal_6.cal_grp;
            break;
        case GH2_MMU_PIPE_3_CAL_ID: 
            pipe_str = "MMU Pipe 3"; idx_s = 97; idx_e = 128;
            cal_grp_num = _tdm->_chip_data.cal_7.grp_num;
            cal_grp_len = _tdm->_chip_data.cal_7.grp_len;
            cal_grp     = _tdm->_chip_data.cal_7.cal_grp;
            break;
        default:
            pipe_str = "Unknown Pipe";
            cal_grp_num = 0;
            cal_grp_len = 0;
            cal_grp = NULL;
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized calendar ID %d\n",
               "[Oversub Subscription]",
               cal_id);
            break;
    }
    
    if (result==PASS){
        /* check port subscription */
        for (k=idx_s; k<=idx_e; k++){
            port_state = port_states[k-1];
            port_speed = port_speeds[k];
            if (port_state==PORT_STATE__OVERSUB    ||
                port_state==PORT_STATE__OVERSUB_HG ){ 
                port_is_found = BOOL_FALSE;
                for (i=0; i<cal_grp_num; i++){
                    for (j=0; j<cal_grp_len; j++){
                        port_phynum = cal_grp[i][j];
                        if (port_phynum==k){
                            port_is_found = BOOL_TRUE;
                            break;
                        }
                    }
                    if (port_is_found){
                        break;
                    }
                }
                if (port_is_found==BOOL_FALSE){
                    result = FAIL;
                    TDM_ERROR5("%s, %s, %s -- port %d, speed %dG\n",
                       "[Oversub Subscription]",
                       pipe_str,
                       "Unfounded Oversub port",
                       k, port_speed/1000);
                }
            }
        }
        /* check ovsb group speed */
        for (i=0; i<cal_grp_num; i++){
            port_phynum = cal_grp[i][0];
            port_speed_0= 0;
            GH2_TOKEN_CHECK(port_phynum){
                port_speed_0 = port_speeds[port_phynum];
            }
            for (j=1; j<cal_grp_len; j++){
                port_phynum = cal_grp[i][j];
                GH2_TOKEN_CHECK(port_phynum){
                    port_speed = port_speeds[port_phynum];
                    if ((port_speed>=SPEED_10G) &&
                        (port_speed>(port_speed_0+1010)||
                         port_speed<(port_speed_0-1010) ) ){
                        result = FAIL;
                        TDM_ERROR7("%s, %s, %s, [%d, %dG], port %d spd %dG\n",
                           "[Oversub Subscription]",
                           pipe_str,
                           "mismatched OVSB group speed, [grp, spd] = ",
                           j,port_speed_0/1000,port_phynum,port_speed/1000);
                    }
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_sub_lr
@param:

Check bandwidth for linerate ports in MMU/IDB calendars
 */
int 
tdm_gh2_chk_sub_lr(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* IDB pipe 0 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_IDB_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 1 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_IDB_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 2 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_IDB_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 3 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_IDB_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 0 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_MMU_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 1 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_MMU_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 2 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_MMU_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 3 */
    pipe_result = tdm_gh2_chk_sub_lr_pipe(_tdm, GH2_MMU_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_gh2_chk_sub_os
@param:

Check bandwidth for linerate ports in MMU/IDB calendars
 */
int 
tdm_gh2_chk_sub_os(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* IDB pipe 0 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_IDB_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 1 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_IDB_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 2 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_IDB_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 3 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_IDB_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 0 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_MMU_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 1 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_MMU_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 2 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_MMU_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 3 */
    pipe_result = tdm_gh2_chk_sub_os_pipe(_tdm, GH2_MMU_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_gh2_chk_sub
@param:

Verifies IDB/MMU calendars allocating enough slots for port bandwidth
 */
int 
tdm_gh2_chk_sub(tdm_mod_t *_tdm)
{
    int result=PASS;
    
    /* CPU and Management */
    if (tdm_gh2_chk_sub_cpu(_tdm)!=PASS){
        result = FAIL;
    }
    /* Loopback */
    if (tdm_gh2_chk_sub_lpbk(_tdm)!=PASS){
        result = FAIL;
    }
    /* ACC */
    if (tdm_gh2_chk_sub_acc(_tdm)!=PASS){
        result = FAIL;
    }
    /* Linerate ports */
    if(tdm_gh2_chk_sub_lr(_tdm)!=PASS){
        result = FAIL;
    }
    /* Oversub ports */
    if(tdm_gh2_chk_sub_os(_tdm)!=PASS){
        result = FAIL;
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_jitter_os_pipe_tdm5
@param:

Checks distribution of oversub tokens for the given pipe
 */
int 
tdm_gh2_chk_jitter_os_pipe_tdm5(tdm_mod_t *_tdm, int cal_id)
{	
	int os_range_thresh, os_clump_ave, os_clump_size, os_cnt_idx, 
        lr_cnt_idx, group_size,	clump_cnt=0, lr_bw=0, os_bw=0;
	int dist_opt, dist_floor, dist_ceil, dist_i, dist_sum1=0, dist_sum2=0,
        dist_ave=0, dist_var=0;
    int i, result=PASS;
    const char *pipe_str;
    int cal_main_len;
    int *cal_main=NULL;
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: pipe_str = "IDB Pipe 0"; break;
        case GH2_IDB_PIPE_1_CAL_ID: pipe_str = "IDB Pipe 1"; break;
        case GH2_IDB_PIPE_2_CAL_ID: pipe_str = "IDB Pipe 2"; break;
        case GH2_IDB_PIPE_3_CAL_ID: pipe_str = "IDB Pipe 3"; break;
        case GH2_MMU_PIPE_0_CAL_ID: pipe_str = "MMU Pipe 0"; break;
        case GH2_MMU_PIPE_1_CAL_ID: pipe_str = "MMU Pipe 1"; break;
        case GH2_MMU_PIPE_2_CAL_ID: pipe_str = "MMU Pipe 2"; break;
        case GH2_MMU_PIPE_3_CAL_ID: pipe_str = "MMU Pipe 3"; break;
        default:
            pipe_str = "Unknown Pipe";
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized calendar ID %d\n",
               "[Oversub Jitter]",
               cal_id);
            break;
    }
    if (result!=PASS){
        return result;
    }
    
    TDM_SEL_CAL(cal_id, cal_main);
    cal_main_len = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
    for (i=0; i<cal_main_len; i++) {
        if (cal_main[i]==GH2_OVSB_TOKEN) {
            os_bw+=BW_QUANTA;
        }
        else {
            lr_bw+=BW_QUANTA;
        }
    }
    if (lr_bw==0 || os_bw==0) {
        TDM_PRINT1("TDM: Not applicable in - %s\n",pipe_str);
        TDM_SML_BAR
        return result;
    }
    
    TDM_PRINT1("TDM: Analyzing oversub - %s\n",pipe_str);
    dist_opt    =  ((os_bw+lr_bw)/BW_QUANTA)/(os_bw/BW_QUANTA);
    os_clump_ave= (os_bw/BW_QUANTA)/((lr_bw/BW_QUANTA)+1);
    
    if ((os_bw/10)>=200) {
        os_range_thresh = OS_RANGE_BW_HI;
    }
    else if ( ((os_bw/10)<200) && ((os_bw/10)>=100) ) {
        os_range_thresh = OS_RANGE_BW_MD;
    }
    else {
        os_range_thresh = JITTER_THRESH_MH;
    }
    dist_floor  = ((dist_opt-os_range_thresh)>0)?
                  (dist_opt-os_range_thresh):
                  (1);
    dist_ceil   = dist_opt+os_range_thresh;

    for (i=1; i<cal_main_len; i++) {
        if (cal_main[i]==GH2_OVSB_TOKEN) {
            os_clump_size=1; 
            os_cnt_idx=i; 
            lr_cnt_idx=i; 
            while (os_cnt_idx<cal_main_len && 
                   cal_main[++os_cnt_idx]==GH2_OVSB_TOKEN) {
                os_clump_size++;
            }
            group_size=os_clump_size;
            while(lr_cnt_idx>0 && 
                  cal_main[--lr_cnt_idx]!=GH2_OVSB_TOKEN) {
                group_size++;
            }

            dist_i     = group_size/os_clump_size;
            dist_sum1 += dist_i;
            dist_sum2 += (dist_i-dist_opt)*(dist_i-dist_opt);
            clump_cnt++;

            if (os_clump_size>(os_clump_ave+os_range_thresh)) {
            TDM_PRINT4("\t\t%s, size %0d index %0d, limit %0d\n",
                "Oversub slice suboptimal",
                os_clump_size,i,(os_clump_ave+os_range_thresh));
            }
            while (cal_main[i+1]==GH2_OVSB_TOKEN) {i++;}
        }
    }
    if(clump_cnt!=0){
        dist_ave  = dist_sum1/clump_cnt;
        dist_var  = tdm_sqrt(dist_sum2/clump_cnt);
        if ( dist_ave<dist_floor || 
             dist_ave>dist_ceil  || 
             dist_var>os_range_thresh ) {
            result = FAIL;
        }
    }
	
	TDM_SML_BAR
	TDM_PRINT1("TDM:\tTracked %0d oversub slices in calendar\n",clump_cnt);
	TDM_PRINT2("TDM:\t\t\tDistance - Optimal Range    = [%0d, %0d]\n",
        dist_floor, dist_ceil);
	TDM_PRINT1("TDM:\t\t\tDistance - Optimal Value    = %0d\n",dist_opt);
	TDM_PRINT1("TDM:\t\t\tDistance - Jitter Tolerance = %0d slots\n",
        os_range_thresh);
	TDM_PRINT1("TDM:\t\t\tDistance - Grant Average    = %0d\n",dist_ave);
	TDM_PRINT1("TDM:\t\t\tDistance - Grant Variance   = %0d\n",dist_var);
	TDM_SML_BAR
	
	return result;
}


/**
@name: tdm_gh2_chk_jitter_os_tdm5
@param:

Checks distribution of oversub tokens for IDB/MMU calendars
 */
int 
tdm_gh2_chk_jitter_os_tdm5(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* IDB pipe 0 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_IDB_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 1 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_IDB_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 2 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_IDB_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 3 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_IDB_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 0 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_MMU_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 1 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_MMU_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 2 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_MMU_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 3 */
    pipe_result = tdm_gh2_chk_jitter_os_pipe_tdm5(_tdm, GH2_MMU_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_gh2_chk_jitter_lr_pipe_tdm5
@param:

Checks distribution of linerate slots of the same port for the given pipe
 */
int 
tdm_gh2_chk_jitter_lr_pipe_tdm5(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, k, port_i, port_j, port_k, port_speed, 
        slot_req, distance, tracker[512], 
        has_checked, ports_done[GH2_NUM_EXT_PORTS], done_idx=0, 
        result=PASS;
    int space_min=0, space_max=0, space_frac, jitter_range;
    
    const char *pipe_str;
    int cal_main_len;
    int *cal_main=NULL;
    enum port_speed_e *port_speeds;
    
    port_speeds  = _tdm->_chip_data.soc_pkg.speed;
    cal_main_len = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
    
    for (i=0; i<GH2_NUM_EXT_PORTS; i++) {ports_done[i]=GH2_NUM_EXT_PORTS;}
    for (i=0; i<cal_main_len && i<512; i++) {tracker[i] = 0;}
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: pipe_str = "IDB Pipe 0"; break;
        case GH2_IDB_PIPE_1_CAL_ID: pipe_str = "IDB Pipe 1"; break;
        case GH2_IDB_PIPE_2_CAL_ID: pipe_str = "IDB Pipe 2"; break;
        case GH2_IDB_PIPE_3_CAL_ID: pipe_str = "IDB Pipe 3"; break;
        case GH2_MMU_PIPE_0_CAL_ID: pipe_str = "MMU Pipe 0"; break;
        case GH2_MMU_PIPE_1_CAL_ID: pipe_str = "MMU Pipe 1"; break;
        case GH2_MMU_PIPE_2_CAL_ID: pipe_str = "MMU Pipe 2"; break;
        case GH2_MMU_PIPE_3_CAL_ID: pipe_str = "MMU Pipe 3"; break;
        default:
            pipe_str = "Unknown Pipe";
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized calendar ID %d\n",
               "[Linerate Jitter]",
               cal_id);
            break;
    }
    if (result!=PASS){
        return result;
    }
    /* construct space tracker */
    TDM_SEL_CAL(cal_id, cal_main);
    for (i=0; i<cal_main_len && i<512; i++) {
        port_i = cal_main[i];
        GH2_TOKEN_CHECK(port_i){
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
    /* Check jitter constraint for Linerate ports */
    for (i=0; i<cal_main_len && i<512; i++) {
        if (tracker[i]==0) {continue;}
        port_i = cal_main[i];
        GH2_TOKEN_CHECK(port_i){
            has_checked = BOOL_FALSE;
            for (j=0; j<done_idx && j<GH2_NUM_EXT_PORTS; j++) {
                if (ports_done[j]==port_i) {
                    has_checked = BOOL_TRUE;
                    break;
                }
            }
            if (has_checked==BOOL_FALSE){
                if (done_idx<GH2_NUM_EXT_PORTS){
                    ports_done[done_idx++] = port_i;
                }
                
                port_speed = port_speeds[port_i];
                slot_req   = tdm_gh2_chk_get_speed_slot_num(port_speed);
                /* calculate min/max */
                if (slot_req>1){
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
                
                    /* check jitter for each slot of the given port */
                    for (j=0; j<cal_main_len && j<512; j++) {
                        port_j = cal_main[j];
                        if (port_i==port_j){
                            /* max */
                            if (tracker[j]>(space_max+GH2_LR_CHK_TRHD)){
                                /* 
                                result = FAIL; */
                                TDM_WARN6("%s, %s, port %d slot %d, %d > %d\n",
                                   "[Linerate Jitter (MAX)]",
                                   pipe_str,
                                   port_i,
                                   j,
                                   tracker[j],
                                   space_max);
                            }
                            /* min */
                            if (tracker[j]<(space_min-GH2_LR_CHK_TRHD) && tracker[j]>0){
                                /* 
                                result = FAIL;
                                TDM_ERROR6("%s, %s, port %d slot %d, %d < %d\n",
                                   "[Linerate Jitter (MIN)]",
                                   pipe_str,
                                   port_i,
                                   j,
                                   tracker[j],
                                   space_min); */
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_jitter_lr_tdm5
@param:

Checks distribution of linerate tokens for IDB/MMU calendars
 */
int 
tdm_gh2_chk_jitter_lr_tdm5(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* IDB pipe 0 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_IDB_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 1 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_IDB_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 2 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_IDB_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 3 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_IDB_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 0 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_MMU_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 1 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_MMU_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 2 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_MMU_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 3 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe_tdm5(_tdm, GH2_MMU_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_gh2_chk_tdm5
@param: 
 */
int
tdm_gh2_chk_tdm5( tdm_mod_t *_tdm)
{
    int i, fail[6];
    
    for (i=0; i<6; i++){
        fail[i] = 2;
    }
    
    /* [0]: Check structure */
    if (fail[0]==2) {
        TDM_PRINT0("TDM: Checking Structure (length, frequency)\n");
        TDM_SML_BAR
        fail[0] = tdm_gh2_chk_struct(_tdm);
    }
    
    /* [1]: Check port transcription */
    if (fail[1]==2) {
        TDM_PRINT0("\nTDM: Checking Port Transcription\n"); 
        TDM_SML_BAR
        fail[1] = tdm_gh2_chk_transcription(_tdm);
    }
    
    /* [2]: Check sister port (and/or min-same-port) spacing */
    if (fail[2]==2) {
        TDM_PRINT0("\nTDM: Checking Sister Spacing\n");
        TDM_SML_BAR
        fail[2] = tdm_gh2_chk_sister(_tdm);
    }
    
    /* [3]: Check port subscription */
    if (fail[3]==2) {
        TDM_PRINT0("\nTDM: Checking Port Subscription\n");
        TDM_SML_BAR
        fail[3] = tdm_gh2_chk_sub(_tdm);
    }
    
    /* [4]: Check oversub jitter */
    if (fail[4]==2) {
        TDM_PRINT0("\nTDM: Checking Oversub Jitter\n");
        TDM_SML_BAR
        fail[4] = tdm_gh2_chk_jitter_os_tdm5(_tdm);
    }
    
    /* [5]: Check linerate jitter */
    if (fail[5]==2) {
        TDM_PRINT0("\nTDM: Checking Linerate Jitter\n");
        TDM_SML_BAR
        fail[5] = tdm_gh2_chk_jitter_lr_tdm5(_tdm);
    }
    
    /* Summarize check results */
    TDM_PRINT0("\n"); TDM_SML_BAR
    if (fail[0]==FAIL ||
        fail[1]==FAIL ||
        fail[2]==FAIL ||
        fail[3]==FAIL /* || */
        /* fail[4]==FAIL || */
        /* fail[5]==FAIL */ ){
        TDM_PRINT0("TDM: *** FAILED ***\n");
        TDM_SML_BAR
        TDM_PRINT0("TDM: Fail vector: [");
        for (i=0; i<6; i++) {
            TDM_PRINT1(" %0d ",fail[i]);
        }
        TDM_PRINT0("]\n");
    }
    else {
        TDM_PRINT0("TDM: *** PASSED ***\n");
    }
    TDM_SML_BAR
    TDM_PRINT0("TDM: TDM Self Check Complete.\n");
    TDM_BIG_BAR	
    
    return PASS;
}


/**
@name: tdm_gh2_chk_jitter_os_pipe
@param:

Checks distribution of oversub tokens for the given pipe
 */
int 
tdm_gh2_chk_jitter_os_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, k, result=PASS;
    int os_cnt, lr_bw=0, os_bw=0, os_bw_ratio;
    const char *pipe_str;
    int param_cal_len, param_token_ovsb,
        param_os_window, param_os_slots;
    int *param_cal_main=NULL;
    
    /* set parameters */
    param_cal_len     = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: pipe_str = "IDB Pipe 0"; break;
        case GH2_IDB_PIPE_1_CAL_ID: pipe_str = "IDB Pipe 1"; break;
        case GH2_IDB_PIPE_2_CAL_ID: pipe_str = "IDB Pipe 2"; break;
        case GH2_IDB_PIPE_3_CAL_ID: pipe_str = "IDB Pipe 3"; break;
        case GH2_MMU_PIPE_0_CAL_ID: pipe_str = "MMU Pipe 0"; break;
        case GH2_MMU_PIPE_1_CAL_ID: pipe_str = "MMU Pipe 1"; break;
        case GH2_MMU_PIPE_2_CAL_ID: pipe_str = "MMU Pipe 2"; break;
        case GH2_MMU_PIPE_3_CAL_ID: pipe_str = "MMU Pipe 3"; break;
        default:
            pipe_str = "Unknown Pipe";
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized calendar ID %d\n",
               "[Oversub Jitter]",
               cal_id);
            break;
    }
    if (result!=PASS){
        return result;
    }
    
    TDM_SEL_CAL(cal_id, param_cal_main);
    if (result!=PASS || param_cal_main==NULL){
        TDM_ERROR2("%s, unrecognized calendar ID %d\n",
           "[OverSub Jitter]",
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
    
    TDM_PRINT1("TDM: %s - Analyzing oversub distribution...\n",pipe_str);
    
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
                     "[OverSub Jitter]",
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
@name: tdm_gh2_chk_jitter_os
@param:

Checks distribution of oversub tokens for IDB/MMU calendars
 */
int 
tdm_gh2_chk_jitter_os(tdm_mod_t *_tdm)
{
    int i, pipe_result[8], result=PASS;
    for (i=0; i<8; i++){
        pipe_result[i] = PASS;
    }
    
    /* IDB pipe 0 - 4 */
    pipe_result[0] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_IDB_PIPE_0_CAL_ID);
    pipe_result[1] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_IDB_PIPE_1_CAL_ID);
    pipe_result[2] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_IDB_PIPE_2_CAL_ID);
    pipe_result[3] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_IDB_PIPE_3_CAL_ID);
    /* MMU pipe 0 - 4 */
/*     
    pipe_result[4] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_MMU_PIPE_0_CAL_ID);
    pipe_result[5] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_MMU_PIPE_1_CAL_ID);
    pipe_result[6] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_MMU_PIPE_2_CAL_ID);
    pipe_result[7] = tdm_gh2_chk_jitter_os_pipe(_tdm, GH2_MMU_PIPE_3_CAL_ID); */
    
    if (pipe_result[0]!=PASS ||
        pipe_result[1]!=PASS ||
        pipe_result[2]!=PASS ||
        pipe_result[3]!=PASS ||
        pipe_result[4]!=PASS ||
        pipe_result[5]!=PASS ||
        pipe_result[6]!=PASS ||
        pipe_result[7]!=PASS ){
        result = FAIL;
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_jitter_lr_pipe
@param:

Checks distribution of linerate slots of the same port for the given pipe
 */
int 
tdm_gh2_chk_jitter_lr_pipe(tdm_mod_t *_tdm, int cal_id)
{
    int i, j, k, port_i, port_j, port_k, port_speed, 
        slot_req, distance, tracker[512], 
        has_checked, ports_done[GH2_NUM_EXT_PORTS], done_idx=0, 
        result=PASS;
    int space_min, space_max, space_frac, jitter_range;
    
    const char *pipe_str;
    int cal_main_len;
    int *cal_main=NULL;
    enum port_speed_e *port_speeds;
    
    port_speeds  = _tdm->_chip_data.soc_pkg.speed;
    
    TDM_SEL_CAL(cal_id, cal_main);
    cal_main_len = tdm_gh2_chk_get_cal_len(_tdm, cal_id);
    
    for (i=0; i<GH2_NUM_EXT_PORTS; i++) {ports_done[i]=GH2_NUM_EXT_PORTS;}
    for (i=0; i<cal_main_len && i<512; i++) {tracker[i] = 0;}
    
    switch(cal_id){
        case GH2_IDB_PIPE_0_CAL_ID: pipe_str = "IDB Pipe 0"; break;
        case GH2_IDB_PIPE_1_CAL_ID: pipe_str = "IDB Pipe 1"; break;
        case GH2_IDB_PIPE_2_CAL_ID: pipe_str = "IDB Pipe 2"; break;
        case GH2_IDB_PIPE_3_CAL_ID: pipe_str = "IDB Pipe 3"; break;
        case GH2_MMU_PIPE_0_CAL_ID: pipe_str = "MMU Pipe 0"; break;
        case GH2_MMU_PIPE_1_CAL_ID: pipe_str = "MMU Pipe 1"; break;
        case GH2_MMU_PIPE_2_CAL_ID: pipe_str = "MMU Pipe 2"; break;
        case GH2_MMU_PIPE_3_CAL_ID: pipe_str = "MMU Pipe 3"; break;
        default:
            pipe_str = "Unknown Pipe";
            result = UNDEF;
            TDM_ERROR2("%s, unrecognized calendar ID %d\n",
               "[Linerate Jitter]",
               cal_id);
            break;
    }
    
    /* construct space tracker */
    for (i=0; i<cal_main_len && i<512; i++) {
        port_i = cal_main[i];
        GH2_TOKEN_CHECK(port_i){
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
    /* Check jitter constraint for Linerate ports */
    for (i=0; i<cal_main_len && i<512; i++) {
        if (tracker[i]==0) {continue;}
        port_i = cal_main[i];
        GH2_TOKEN_CHECK(port_i){
            has_checked = BOOL_FALSE;
            for (j=0; j<done_idx && j<GH2_NUM_EXT_PORTS; j++) {
                if (ports_done[j]==port_i) {
                    has_checked = BOOL_TRUE;
                    break;
                }
            }
            if (has_checked==BOOL_FALSE){
                if (done_idx<GH2_NUM_EXT_PORTS){
                    ports_done[done_idx++] = port_i;
                }
                
                port_speed = port_speeds[port_i];
                slot_req   = tdm_gh2_chk_get_speed_slot_num(port_speed);
                /* calculate min/max */
                if (slot_req>1){
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
                
                    /* check jitter for each slot of the given port */
                    for (j=0; j<cal_main_len && j<512; j++) {
                        port_j = cal_main[j];
                        if (port_i==port_j){
                            /* max */
                            if (tracker[j]>(space_max+GH2_LR_CHK_TRHD)){                                
                                result = FAIL;
                                TDM_ERROR7("%s, %s, port %d, speed %dG, slot %d, %d > %d\n",
                                   "[Linerate Jitter (MAX)]",
                                   pipe_str,
                                   port_i,
                                   port_speed/1000,
                                   j,
                                   tracker[j],
                                   space_max);
                            }
                            /* min */
                            if (tracker[j]<(space_min-GH2_LR_CHK_TRHD) && tracker[j]>0){
                                /* result = FAIL; */
                                TDM_PRINT7("%s, %s, port %d, speed %dG slot %d, %d < %d\n",
                                   "TDM: WARNING [Linerate Jitter (MIN)]",
                                   pipe_str,
                                   port_i,
                                   port_speed/1000,
                                   j,
                                   tracker[j],
                                   space_min);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_gh2_chk_jitter_lr
@param:

Checks distribution of linerate tokens for IDB/MMU calendars
 */
int 
tdm_gh2_chk_jitter_lr(tdm_mod_t *_tdm)
{
    int pipe_result, result=PASS;
    
    /* IDB pipe 0 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_IDB_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 1 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_IDB_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 2 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_IDB_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* IDB pipe 3 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_IDB_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 0 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_MMU_PIPE_0_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 1 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_MMU_PIPE_1_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 2 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_MMU_PIPE_2_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    /* MMU pipe 3 */
    pipe_result = tdm_gh2_chk_jitter_lr_pipe(_tdm, GH2_MMU_PIPE_3_CAL_ID);
    result = (pipe_result!=PASS)? (pipe_result): (result);
    
    return result;
}


/**
@name: tdm_gh2_chk
@param: 
 */
int
tdm_gh2_chk( tdm_mod_t *_tdm)
{
    int i, fail[6], result=PASS;
    
    for (i=0; i<6; i++){
        fail[i] = 2;
    }
    
    /* [0]: Check structure */
    if (fail[0]==2) {
        TDM_PRINT0("TDM: Checking Structure (length, frequency)\n");
        TDM_SML_BAR
        fail[0] = tdm_gh2_chk_struct(_tdm);
    }
    
    /* [1]: Check port transcription */
    if (fail[1]==2) {
        TDM_PRINT0("\nTDM: Checking Port Transcription\n"); 
        TDM_SML_BAR
        fail[1] = tdm_gh2_chk_transcription(_tdm);
    }
    
    /* [2]: Check sister port (and/or min-same-port) spacing */
    if (fail[2]==2) {
        TDM_PRINT0("\nTDM: Checking Sister Spacing\n");
        TDM_SML_BAR
        fail[2] = tdm_gh2_chk_sister(_tdm);
    }
    
    /* [3]: Check port subscription */
    if (fail[3]==2) {
        TDM_PRINT0("\nTDM: Checking Port Subscription\n");
        TDM_SML_BAR
        fail[3] = tdm_gh2_chk_sub(_tdm);
    }
    
    /* [4]: Check oversub jitter */
    if (fail[4]==2) {
        TDM_PRINT0("\nTDM: Checking Oversub Jitter\n");
        TDM_SML_BAR
        fail[4] = tdm_gh2_chk_jitter_os(_tdm);
    }
    
    /* [5]: Check linerate jitter */
    if (fail[5]==2) {
        TDM_PRINT0("\nTDM: Checking Linerate Jitter\n");
        TDM_SML_BAR
        fail[5] = tdm_gh2_chk_jitter_lr(_tdm);
    }
    
    /* Summarize check results */
    TDM_PRINT0("\n"); TDM_SML_BAR
    if (fail[0]==FAIL ||
        fail[1]==FAIL ||
        fail[2]==FAIL ||
        fail[3]==FAIL ||
        /* fail[4]==FAIL || */
        fail[5]==FAIL ){
        result = FAIL;
        TDM_PRINT0("TDM: *** FAILED ***\n");
        TDM_SML_BAR
        TDM_PRINT0("TDM: Fail vector: [");
        for (i=0; i<6; i++) {
            TDM_PRINT1(" %0d ",fail[i]);
        }
        TDM_PRINT0("]\n");
    }
    else {
        TDM_PRINT0("TDM: *** PASSED ***\n");
    }
    TDM_SML_BAR
    TDM_PRINT0("TDM: TDM Self Check Complete.\n");
    TDM_BIG_BAR	
    
    return result;
}
