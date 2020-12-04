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
@name: tdm_fl_proc_cal_ancl
@param:

Allocate ancillary slots with matched speed rates accordingly
 */
int
tdm_fl_proc_cal_ancl(tdm_mod_t *_tdm)
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
@name: tdm_fl_proc_cal_idle
@param:

Allocate idle slots with matched speed rates accordingly
 */
int
tdm_fl_proc_cal_idle( tdm_mod_t *_tdm )
{
    int i, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_token_empty, param_token_idl1;
    int *param_cal_main;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_idl1  = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    if (param_cal_main != NULL && param_cal_len > 0) {
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                param_cal_main[i] = param_token_idl1;
            }
        }
    }

    return result;
}

/**
@name: tdm_fl_proc_cal_lr
@param:

Allocate linerate slots with matched speed rates accordingly
 */
int
tdm_fl_proc_cal_lr( tdm_mod_t *_tdm )
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
@name: tdm_fl_proc_cal
@param:

Allocate slots for linerate/oversub/ancl ports with matched speed rates accordingly
 */
int
tdm_fl_proc_cal(tdm_mod_t *_tdm)
{
    /* allocate LINERATE slots */
    if(tdm_fl_proc_cal_lr(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    /* allocate ANCILLARY slots */
    if(tdm_fl_proc_cal_ancl(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    /* allocate IDLE/OVSB slots */
    if(tdm_fl_proc_cal_idle(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }

    return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER](_tdm) );
}
