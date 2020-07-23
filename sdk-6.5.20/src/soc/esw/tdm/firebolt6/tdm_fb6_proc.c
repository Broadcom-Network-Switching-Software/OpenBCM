/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_fb6_proc_cal_ancl
@param:

Allocate ancillary slots with matched speed rates accordingly
 */
int
tdm_fb6_proc_cal_ancl(tdm_mod_t *_tdm)
{
    int i, j, k, k_prev, k_idx, idx_up, idx_dn, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_token_empty, param_token_ancl,
        param_lr_en;
    int *param_cal_main;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;

    /* allocate ANCL slots */
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_ancl_num>0){
        TDM_PRINT1("TDM: Allocate %d ANCL slots\n\n", param_ancl_num);
        if (param_lr_en==BOOL_TRUE){
            k = 0;
            for (j=0; j<param_ancl_num; j++){
                idx_up = 0;
                idx_dn = 0;
                k_idx  = 0;
                k_prev = k;
                if (j==(param_ancl_num-1)){
                    k = param_cal_len - 1;
                } else {
                    k = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) -1;
                }
                for (i=k; i>0 && i<param_cal_len; i--){
                    if (param_cal_main[i]==param_token_empty){
                        idx_up = i;
                        break;
                    }
                }
                for (i=k; i<param_cal_len; i++){
                    if (param_cal_main[i]==param_token_empty){
                        idx_dn = i;
                        break;
                    }
                }
                if (idx_up>0 || idx_dn>0){
                    if (idx_up>0 && idx_dn>0){
                        k_idx = ((k-idx_up)<(idx_dn-k))?(idx_up):(idx_dn);
                    } else if (idx_up>0){
                        k_idx = idx_up;
                    } else if (idx_dn>0){
                        k_idx = idx_dn;
                    }
                }
                if (k_idx==0){
                    for (i=(param_cal_len-1); i>0; i--){
                        if (param_cal_main[i]==param_token_empty){
                            k_idx = i;
                            break;
                        }
                    }
                }
                if (k_idx>0 && k_idx<param_cal_len){
                    param_cal_main[k_idx] = param_token_ancl;
                    TDM_PRINT2("TDM: Allocate %d-th ANCL slot at index #%03d\n",
                                j+1, k_idx);
                } else {
                    TDM_WARN1("TDM: Failed in %d-th ANCL slot allocation\n",j);
                    result = FAIL;
                }
            }
        } else {
            k_prev = 0;
            for (j=0; j<param_ancl_num; j++){
                k_idx = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) - 1;
                param_cal_main[k_idx] = param_token_ancl;
                k_prev= k_idx+1;
                TDM_PRINT1("TDM: Allocate ANCL slot at index #%03d\n",
                            k_idx);
            }
        }
        TDM_SML_BAR
    }

    return result;
}


/**
@name: tdm_fb6_proc_cal_idle
@param:

Allocate idle slots with matched speed rates accordingly
 */
int
tdm_fb6_proc_cal_idle( tdm_mod_t *_tdm )
{
    int i, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_token_empty, param_token_ovsb,
        param_token_idl1, param_token_idl2,
        param_os_en;
    int *param_cal_main;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_idl1  = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;
    param_token_idl2  = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    if (param_os_en==BOOL_TRUE){
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                param_cal_main[i] = param_token_ovsb;
            }
        }
    }
    else {
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                if ((i%2)==0){
                    param_cal_main[i] = param_token_idl1;
                    TDM_PRINT2("PROC: Printing IDL1 i %d param_cal_main[i] %d\n", i, param_cal_main[i]);
                }
                else {
                    param_cal_main[i] = param_token_idl2;
                    TDM_PRINT2("PROC: Printing IDL2 i %d param_cal_main[i] %d\n", i, param_cal_main[i]);
                }
            }
        }
    }

    return result;
}

/**
@name: tdm_fb6_proc_pre_cal_lr
@param:

  Pre Cal lr API to modify speeds for 4721/4722 (4731/4732) TDMs to 50G/10G and
  store the configuration speed for distributing slots evenly in post cal api
 */
void
tdm_fb6_proc_pre_cal_lr( tdm_mod_t *_tdm, int *old_speed, int *pseudo_lr_num )
{
   int port_phy;
   enum port_state_e *param_states;
   int lr_pm_num=0;
   int i;

    /* set parameters */
    param_states = _tdm->_chip_data.soc_pkg.state;

    for (i=0; i<4; i++) {
      old_speed[i] = 0;
    }
    for (port_phy = 1; port_phy <= 68; port_phy++) {
            if (param_states[port_phy-1] == PORT_STATE__LINERATE ||
                param_states[port_phy-1] == PORT_STATE__LINERATE_HG) {
              lr_pm_num = (port_phy-1)/4;
             old_speed[(port_phy-1)%4] =  _tdm->_chip_data.soc_pkg.speed[port_phy];
             /*TDM_PRINT3("PROC_CAL: port_phy : %d LR PM num %d old_speed %d\n", port_phy, lr_pm_num, old_speed[(port_phy-1)%4] );*/
            }
    }

    for (i=0; i<4; i++) {
      pseudo_lr_num[i] = lr_pm_num*4+i+1;
    }
    _tdm->_chip_data.soc_pkg.speed[(lr_pm_num*4)+1] = SPEED_50G;
    _tdm->_chip_data.soc_pkg.speed[(lr_pm_num*4)+2] = SPEED_0;
    _tdm->_chip_data.soc_pkg.speed[(lr_pm_num*4)+3] = SPEED_10G;
    _tdm->_chip_data.soc_pkg.speed[(lr_pm_num*4)+4] = SPEED_0;
    /*_tdm->_chip_data.soc_pkg.state[(lr_pm_num*4)+0] = PORT_STATE__LINERATE;
    _tdm->_chip_data.soc_pkg.state[(lr_pm_num*4)+1] = PORT_STATE__DISABLED;
    _tdm->_chip_data.soc_pkg.state[(lr_pm_num*4)+2] = PORT_STATE__LINERATE;
    _tdm->_chip_data.soc_pkg.state[(lr_pm_num*4)+3] = PORT_STATE__DISABLED;*/

    /*for (i=0; i<4; i++) {
     TDM_PRINT2(" OLD SPEED: %d pseudo_lr_num %d\n", old_speed[i], pseudo_lr_num[i]);
    }*/
}


/**
@name: tdm_fb6_proc_post_cal_lr
@param:
 */
int
tdm_fb6_proc_post_cal_lr( tdm_mod_t *_tdm, int *old_speeds, int *pseudo_lr_num)
{
   int param_cal_id, param_cal_len;
    int *param_cal_main=NULL;
   int num_lanes[4];
   int i,j;
   int total_num_lanes = 0;
   int num_lanes_empty = 0;
   int result = PASS;
    int cnt = 0;
    int pseudo_lr_val[24];
    int num_lanes_cnt[4];

    for(j=0; j<4; j++) {
      num_lanes[j] = 0;
    }

    for(j=0; j<4; j++) {
      /*TDM_PRINT1(" PROC CAL DBG0: %d\n", old_speeds[j]);*/
      if(old_speeds[j]<=25000 && old_speeds[j] !=0) num_lanes[j] = 1;
			if(old_speeds[j]==40000) num_lanes[j] = 2;
      if(old_speeds[j]==50000) num_lanes[j] = 2;
      if(old_speeds[j]==100000) num_lanes[j] = 4;
      if(old_speeds[j]>0) {
        total_num_lanes = num_lanes[j]+total_num_lanes;
      }
       num_lanes_cnt[j] = num_lanes[j];
    }

    if(total_num_lanes > 4) {
            TDM_ERROR1("Invalid Pseudo LR Cfg Number of Lanes - %0d\n", total_num_lanes);
      return FAIL;
    }
    else {
      num_lanes_empty = 4 - total_num_lanes;
    }


    /*for(i=0; i<4; i++) {
     TDM_PRINT2(" PROC CAL DBG1: i %d num_lanes_cnt %d\n", i, num_lanes_cnt[i]);
    }*/

    for(i=0; i<6; i++) {
      for(j=0; j<16; j++) {
       if(num_lanes_cnt[j%4]>0){
         pseudo_lr_val[cnt] = pseudo_lr_num[j%4];
         /*TDM_PRINT4(" PROC CAL DBG2: j %d num_lanes_cnt %d cnt %d, pseudo_lr_val %d\n", j, num_lanes_cnt[j%4], cnt, pseudo_lr_val[cnt]);*/
         num_lanes_cnt[j%4]--;
         cnt++;
       }
      }
      for(j=0; j<4; j++) {
        num_lanes_cnt[j] = num_lanes[j];
      }
      for(j=0; j<num_lanes_empty; j++) {
         pseudo_lr_val[cnt] = 127;
         cnt++;
      }
    }
    /*for(i=0; i<24; i++) {
               TDM_PRINT2(" PROC CAL DBG1: i %d pseudo_lr_val %d\n", i, pseudo_lr_val[i]);
    }*/
    param_cal_id = _tdm->_core_data.vars_pkg.cal_id;
    TDM_SEL_CAL(param_cal_id, param_cal_main);
    param_cal_len   = tdm_fb6_cmn_get_pipe_cal_len(param_cal_id, _tdm);

    TDM_PRINT1(" MAIN CAL: CAL ID POST PROC %d\n", param_cal_id);
    cnt = 0;

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (j=0; j<param_cal_len; j++) {
            if (param_cal_main[j] >= 1 && param_cal_main[j] <= 68) {
               param_cal_main[j] = pseudo_lr_val[cnt];
               TDM_PRINT1(" %d  ", param_cal_main[j]);
               cnt++;
            }
            else {
               TDM_PRINT1(" %d  ", param_cal_main[j]);
            }
            if(j%16==15) {
               TDM_PRINT0(" \n ");
            }
        }
    }

    return result;
}

/**
@name: tdm_fb6_proc_cal_lr
@param:

Allocate linerate slots with matched speed rates accordingly
 */
int
tdm_fb6_proc_cal_lr( tdm_mod_t *_tdm )
{
    int result=PASS;

    /* pre-allocation */
    if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_PREALLOC](_tdm)!=PASS){
        result = FAIL;
    }
    /* allocation */
    if (result == PASS) {
        if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_ALLOC](_tdm)!=PASS){
            result = FAIL;
        }
    }

    return result;
}


/**
@name: tdm_fb6_proc_cal
@param:

Allocate slots for linerate/oversub/ancl ports with matched speed rates accordingly
 */
int
tdm_fb6_proc_cal(tdm_mod_t *_tdm)
{
   int p_old_speeds[4];
   int p_pseudo_lr_num[4];

   if((_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4731_1012) ||
       (_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4732_1012)) {
      tdm_fb6_proc_pre_cal_lr(_tdm, p_old_speeds, p_pseudo_lr_num);
   }
    /* allocate LINERATE slots */
    if(tdm_fb6_proc_cal_lr(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    /* allocate ANCILLARY slots */
    if(tdm_fb6_proc_cal_ancl(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    /* allocate IDLE/OVSB slots */
    if(tdm_fb6_proc_cal_idle(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }

    /*TDM_PRINT0("Before Post cal DBG1\n");*/

   if((_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4731_1012) ||
       (_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4732_1012)) {
     if(tdm_fb6_proc_post_cal_lr(_tdm, p_old_speeds, p_pseudo_lr_num)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
     }
   }

    return(_tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER](_tdm));

}
