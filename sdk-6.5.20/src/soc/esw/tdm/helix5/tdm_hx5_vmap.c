/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM core operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


int tdm_hx5_vmap_get_port_pm(int port_token, tdm_mod_t *_tdm);
int tdm_hx5_vmap_chk_same(tdm_mod_t *_tdm);
int tdm_hx5_vmap_get_port_speed(int port_token, enum port_speed_e *speeds, int phy_lo, int phy_hi);
int tdm_hx5_vmap_get_speed_slots(int port_speed);
/**
@name: tdm_hx5_vmap_print
@param:
 */
void
tdm_hx5_vmap_print(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, vmap_wid, vmap_len, slot_cnt, col_is_empty;
    int param_cal_len, param_vmap_wid, param_vmap_len, param_token_empty;
    
    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    
    vmap_len = (param_cal_len<param_vmap_len)? 
               (param_cal_len): 
               (param_vmap_len);
    vmap_wid = 0;
    for (i=param_vmap_wid-1; i>0; i--){
        col_is_empty = BOOL_TRUE;
        for (j=0; j<vmap_len; j++){
            if (vmap[i][j]!=param_token_empty){
                col_is_empty = BOOL_FALSE;
                break;
            }
        }
        if (col_is_empty==BOOL_FALSE){
            vmap_wid = i+1;
            break;
        }
    }
    /* print vmap */
    TDM_PRINT0("\t TDM vmap \n");
    TDM_PRINT1("%6s\t", " ");
    TDM_PRINT1("%6s\t", " ");
    for (i=0; i<vmap_wid; i++){
        TDM_PRINT1("%3d\t", i);
    }
    TDM_PRINT0("\n");
    for (j=0; j<vmap_len; j++){
        slot_cnt = 0;
        for (i=0; i<vmap_wid; i++){
            if (vmap[i][j]!=param_token_empty){
                slot_cnt++;
            }
        }
        if (slot_cnt>1) {
            TDM_PRINT1("%6s\t", "[FAIL]");
        }
        else {
            TDM_PRINT1("%6s\t", " ");
        }

        TDM_PRINT1("%3d : \t", j);
        for (i=0; i<vmap_wid; i++){
            if (vmap[i][j]!=param_token_empty){
                TDM_PRINT1("%3d\t", vmap[i][j]);
            }
            else{
                TDM_PRINT0("---\t");
            }
        }
        TDM_PRINT0("\n");
    }
    
    /* print overall allocated slots */
    slot_cnt = 0;
    for (j=0; j<vmap_len; j++){
        for (i=0; i<vmap_wid; i++){
            if (vmap[i][j]!=param_token_empty){
                slot_cnt++;
            } 
        }
    }
    TDM_PRINT2("\nTotal number of slots allocated in Vmap: %3d over %3d\n",
               slot_cnt, vmap_len);
    TDM_SML_BAR
}


/**
@name: tdm_hx5_vmap_chk_singularity
@param:
 */
int
tdm_hx5_vmap_chk_singularity(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, vmap_wid, vmap_len, slot_cnt, result=PASS;
    int param_cal_len, param_vmap_wid, param_vmap_len, param_token_empty;
    
    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    
    vmap_len = (param_cal_len<param_vmap_len)? 
               (param_cal_len): 
               (param_vmap_len);
    vmap_wid = param_vmap_wid;
    
    TDM_PRINT0("TDM: Check VMAP singularity\n\n");
    for (j=0; j<vmap_len; j++){
        slot_cnt = 0;
        for (i=0; i<vmap_wid; i++){
            if (vmap[i][j]!=param_token_empty){
                slot_cnt++;
            }
        }
        if (slot_cnt>1) {
            TDM_ERROR2("%s, row %03d\n",
                       "FAILED in vmap singularity check",
                       j);
            result=FAIL;
        }
    }
    TDM_SML_BAR
    
    return result;
}


/**
@name: tdm_hx5_vmap_chk_sister_1D
@param:
 */
int
tdm_hx5_vmap_chk_sister_1D(tdm_mod_t *_tdm)
{
    int i, j, k, port_i, port_j, tsc_i, tsc_j, result=PASS;
    int param_space_sister, param_lr_en,
        param_phy_lo, param_phy_hi,
        param_cal_len;
    int *param_cal_main;
    
    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit + 
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    TDM_PRINT1("TDM: Check VMAP sister port spacing (1-D): %d\n\n",
               param_space_sister);
    if (param_lr_en==BOOL_TRUE && param_space_sister>0){
        for (i=0; i<param_cal_len; i++){
            port_i = param_cal_main[i];
            if (port_i>=param_phy_lo && port_i<=param_phy_hi){
                tsc_i = tdm_hx5_vmap_get_port_pm(port_i, _tdm);
                for (k=1; k<param_space_sister; k++){
                    j = (i+k)%param_cal_len;
                    port_j = param_cal_main[j];
                    if (port_j>=param_phy_lo && port_j<=param_phy_hi){
                        tsc_j = tdm_hx5_vmap_get_port_pm(port_j, _tdm);
                        if (tsc_i==tsc_j){
                            result = FAIL;
                            TDM_PRINT5("%s, slot [%03d|%03d],port [%3d|%3d]\n",
                                    "[sister-port-space violation]",
                                    i, j, port_i, port_j);
                        }
                    }
                }
            }
        }
    }
    TDM_SML_BAR
    
    return result;
}


/**
@name: tdm_hx5_vmap_chk_sister_xy
@param:
 */
int
tdm_hx5_vmap_chk_sister_xy(tdm_mod_t *_tdm, unsigned short **vmap, int idx_x, int idx_y)
{
    int i, j, k, port_token, port_tsc, port_tmp, tsc_tmp, result=PASS;
    int param_space_sister,
        param_phy_lo, param_phy_hi,
        param_vmap_wid, param_vmap_len;
    
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    
    if ( !(idx_x>=0 && idx_x<param_vmap_wid && idx_y>=0 && idx_y<param_vmap_len)){
        result = FAIL;
    }
    else {
        port_token = vmap[idx_x][idx_y];
        if (port_token>=param_phy_lo &&
            port_token<=param_phy_hi ){
            port_tsc = tdm_hx5_vmap_get_port_pm(port_token, _tdm);
            for (i=0; i<param_vmap_wid; i++){
                for (k=1; k<param_space_sister; k++){
                    j = (idx_y+k)%param_vmap_len;
                    port_tmp = vmap[i][j];
                    if (port_tmp>=param_phy_lo &&
                        port_tmp<=param_phy_hi ){
                        tsc_tmp = tdm_hx5_vmap_get_port_pm(port_tmp, _tdm);
                        if (tsc_tmp==port_tsc){
                            result = FAIL;
                            break;
                        }
                    }
                }
                if (result==FAIL){
                    break;
                }
                for (k=1; k<param_space_sister; k++){
                    j = (param_vmap_len+idx_y-k)%param_vmap_len;
                    port_tmp = vmap[i][j];
                    if (port_tmp>=param_phy_lo &&
                        port_tmp<=param_phy_hi ){
                        tsc_tmp = tdm_hx5_vmap_get_port_pm(port_tmp, _tdm);
                        if (tsc_tmp==port_tsc){
                            result = FAIL;
                            break;
                        }
                    }
                }
                if (result==FAIL){
                    break;
                }
            }
        }
    }
    
    return result;
}



/**
@name: tdm_hx5_vmap_chk_sister_col
@param:
 */
int
tdm_hx5_vmap_chk_sister_col(tdm_mod_t *_tdm, unsigned short **vmap, int col_idx)
{
    int row_idx, port_token, chk_result, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_vmap_wid, param_vmap_len;
    
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    
    if ( !(col_idx>=0 && col_idx<param_vmap_wid)){
        result = FAIL;
    }
    else {
        for (row_idx=0; row_idx<param_vmap_len; row_idx++){
            port_token = vmap[col_idx][row_idx];
            if (port_token>=param_phy_lo &&
                port_token<=param_phy_hi ){
                chk_result = tdm_hx5_vmap_chk_sister_xy(_tdm, vmap, col_idx, row_idx);
                if (chk_result!=PASS){
                    result = FAIL;
                    break;
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_hx5_vmap_chk_sister_2D
@param:
 */
int
tdm_hx5_vmap_chk_sister_2D(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, result=PASS;
    int param_space_sister, param_vmap_wid;
    
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    
    TDM_PRINT0("TDM: Check VMAP sister port spacing (2-D)\n\n");
    
    if (param_space_sister>0){
        for (i=0; i<param_vmap_wid; i++){
            if (tdm_hx5_vmap_chk_sister_col(_tdm, vmap, i)!=PASS){
                result = FAIL;
                break;
            }
        }
    }
    TDM_SML_BAR
    
    return result;
}


/**
@name: tdm_hx5_vmap_chk_shift_cond
@param:
 */
int
tdm_hx5_vmap_chk_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm)
{
    int k, src, dst, port, port_pm, idx, port_k, idx_k, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_space_same, param_space_sister;
    
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_same  = _tdm->_core_data.rule__same_port_min;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    
    if (slot < cal_len) {
        /* determine src and dst index */
        src = slot;
        if (dir == TDM_DIR_UP) {
            dst = (src + cal_len - 1) % cal_len;
        } else {
            dst = (src + 1) % cal_len;
        }

        /* check src port spacing */
        idx     = src;
        port    = cal_main[idx];
        port_pm = tdm_hx5_vmap_get_port_pm(port, _tdm);
        if (port >= param_phy_lo && port <= param_phy_hi) {
            /* same port spacing */
            for (k=1; k<=param_space_same; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + cal_len - k) % cal_len;
                } else {
                    idx_k = (idx + k) % cal_len;
                }
                if (cal_main[idx_k] == port) {
                    result = FAIL;
                    break;
                }
            }
            /* sister port spacing */
            for (k=1; k<=param_space_sister; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + cal_len - k) % cal_len;
                } else {
                    idx_k = (idx + k) % cal_len;
                }
                port_k = cal_main[idx_k];
                if (port_k >= param_phy_lo && port_k <= param_phy_hi) {
                    if (tdm_hx5_vmap_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }

        /* check dst port spacing */
        idx     = dst;
        port    = cal_main[idx];
        port_pm = tdm_hx5_vmap_get_port_pm(port, _tdm);
        if (port >= param_phy_lo && port <= param_phy_hi) {
            /* same port spacing */
            for (k=1; k<=param_space_same; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + k) % cal_len;
                } else {
                    idx_k = (idx + cal_len - k) % cal_len;
                }
                if (cal_main[idx_k] == port) {
                    result = FAIL;
                    break;
                }
            }
            /* sister port spacing */
            for (k=1; k<=param_space_sister; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + k) % cal_len;
                } else {
                    idx_k = (idx + cal_len - k) % cal_len;
                }
                port_k = cal_main[idx_k];
                if (port_k >= param_phy_lo && port_k <= param_phy_hi) {
                    if (tdm_hx5_vmap_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }
    }

    return (result);
}


/**
@name: tdm_hx5_vmap_chk_same
@param:
 */
int
tdm_hx5_vmap_chk_same(tdm_mod_t *_tdm)
{
    int i, j, k, port_i, port_j, result=PASS,
        port_speed, port_space, port_slots;
    int param_space_same, param_lr_en,
        param_phy_lo, param_phy_hi,
        param_cal_len;
    int *param_cal_main;
    enum port_speed_e *param_speeds;
    
    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit + 
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_same  = _tdm->_core_data.rule__same_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    TDM_PRINT0("TDM: Check VMAP same port spacing (1-D)\n\n");
    if (param_lr_en==BOOL_TRUE && param_space_same>0){
        for (i=0; i<param_cal_len; i++){
            port_i = param_cal_main[i];
            port_speed = tdm_hx5_vmap_get_port_speed(port_i, param_speeds, 
                                                 param_phy_lo, param_phy_hi);
            port_slots = tdm_hx5_vmap_get_speed_slots(port_speed);
            port_space = port_slots>0 ? param_cal_len/port_slots: 0;
            if (port_speed<SPEED_100G && port_speed>0 &&
                port_space>param_space_same ) {
                for (k=1; k<param_space_same; k++){
                    j = (i+k)%param_cal_len;
                    port_j = param_cal_main[j];
                    if (port_j==port_i){
                        result = FAIL;
                        TDM_PRINT4("%s, slot [%03d|%03d], port %3d\n",
                                "[same-port-space violation]",
                                i, j, port_i);
                    }
                }
            }
        }
    }
    TDM_SML_BAR
    
    return result;
}


/**
@name: tdm_hx5_vmap_get_port_pm
@param:
 */
int
tdm_hx5_vmap_get_port_pm(int port_token, tdm_mod_t *_tdm)
{
    _tdm->_core_data.vars_pkg.port = port_token;
    return (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm));
}


/**
@name: tdm_hx5_vmap_get_port_speed
@param:
 */
int
tdm_hx5_vmap_get_port_speed(int port_token, enum port_speed_e *speeds, int phy_lo, int phy_hi)
{
    if (port_token>=phy_lo && port_token<=phy_hi) {
        return (speeds[port_token]);
    } else {
        return (0);
    }
}


/**
@name: tdm_hx5_vmap_get_speed_slots
@param:
 */
int
tdm_hx5_vmap_get_speed_slots(int port_speed)
{
    int port_spd_slot;
    
    switch(port_speed){
        case SPEED_1G:
            port_spd_slot = 1;
            break;
        case SPEED_2p5G:
            port_spd_slot = 2;
            break;
        case SPEED_5G:
            port_spd_slot = 4;
            break;
        case SPEED_10G:
        case SPEED_10G_DUAL:
        case SPEED_10G_XAUI:
        case SPEED_11G:
            port_spd_slot = 8;
            break;
        case SPEED_15G:
            port_spd_slot = 12;
            break;
        case SPEED_20G:
        case SPEED_21G:
        case SPEED_21G_DUAL:
            port_spd_slot = 16;
            break;
        case SPEED_25G:
        case SPEED_27G:
            port_spd_slot = 20;
            break;
        case SPEED_40G:
        case SPEED_42G:
        case SPEED_42G_HG2:
            port_spd_slot = 32;
            break;
        case SPEED_50G:
        case SPEED_53G:
            port_spd_slot = 40;
            break;
        case SPEED_100G:
        case SPEED_106G:
            port_spd_slot = 80;
            break;
        case SPEED_120G:
        case SPEED_127G:
            port_spd_slot = 96;
            break;
        default:
            port_spd_slot = 0;
            break;
    }
    
    return port_spd_slot;
}


/**
@name: tdm_hx5_vmap_switch_col_xy
@param:
 */
int
tdm_hx5_vmap_switch_col_xy(tdm_mod_t *_tdm, unsigned short **vmap, int col_x, int col_y)
{
    int j, x, y, y_prev, slot_cnt_x, slot_cnt_y, token_tmp, result=PASS;
    int param_token_empty, param_vmap_wid, param_vmap_len;
    
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    
    if ( !(col_x>=0 && col_x<param_vmap_wid && 
           col_y>=0 && col_y<param_vmap_wid) ){
        result = FAIL;
    }
    else {
        slot_cnt_x = 0;
        slot_cnt_y = 0;
        for (j=0; j<param_vmap_len; j++){
            if (vmap[col_x][j]!=param_token_empty && vmap[col_x][j]>0){
                slot_cnt_x++;
            }
            if (vmap[col_y][j]!=param_token_empty && vmap[col_y][j]>0){
                slot_cnt_y++;
            }
        }
        if ((slot_cnt_x!=slot_cnt_y) ||
            (slot_cnt_x==slot_cnt_y && slot_cnt_x==0)){
            result = FAIL;
        }
        else {
            y_prev = 0;
            for (x=0; x<param_vmap_len; x++){
                if (vmap[col_x][x]!=param_token_empty){
                    for (y=y_prev; y<param_vmap_len; y++){
                        if (vmap[col_y][y]!=param_token_empty){
                            token_tmp = vmap[col_x][x];
                            vmap[col_x][x] = vmap[col_y][y];
                            vmap[col_y][y] = token_tmp;
                            
                            y_prev = y+1;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_hx5_vmap_rotate
@param:
 */
int
tdm_hx5_vmap_rotate(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, chk_result, result_i, result_j, result=PASS;
    int param_vmap_wid;
    
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    
    TDM_PRINT0("TDM: Rotate VMAP to remove sister spacing violation\n\n");
    for (i=(param_vmap_wid-1); i>0; i--){
        chk_result = tdm_hx5_vmap_chk_sister_col(_tdm, vmap, i);
        if (chk_result!=PASS){
            TDM_PRINT1("TDM: detect sister port violation at column %d\n", i);
            for (j=i-1; j>=0; j--){
                if(tdm_hx5_vmap_switch_col_xy(_tdm, vmap, i, j)==PASS){
                    result_i = tdm_hx5_vmap_chk_sister_col(_tdm, vmap, i);
                    result_j = tdm_hx5_vmap_chk_sister_col(_tdm, vmap, j);
                    if (result_i==PASS && result_j==PASS) {
                        TDM_PRINT2("\t\t (1) switch column %d with column %d\n",
                                  i, j);
                        break;
                    }
                    else if ( (result_i!=PASS && result_j==PASS)  ||
                              (result_i!=PASS && result_j!=PASS ) ){
                        tdm_hx5_vmap_switch_col_xy(_tdm, vmap, i, j);
                        TDM_PRINT2("\t\t (1) switch back column %d with %d\n", i, j);
                    }
                    else if (result_i==PASS && result_j!=PASS){
                        TDM_PRINT2("\t\t (1) switch column %d with column %d\n",
                                  i, j);
                    }
                }
            }
        }
    }
    TDM_SML_BAR
    
    return result;
}


/**
@name: tdm_hx5_vmap_shift_1D
@param:
 */
int
tdm_hx5_vmap_shift_1D(tdm_mod_t *_tdm, int port_token)
{
    int i, j, k, n, x1, x2, port_tsc, tsc_tmp, k_dn, k_up, 
        is_done=BOOL_FALSE;
    int param_cal_len, param_phy_lo, param_phy_hi, param_space_sister;
    int *param_cal_main;
    
    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    
    port_tsc = tdm_hx5_vmap_get_port_pm(port_token, _tdm);
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    for (n=0; n<param_space_sister; n++){
        is_done=BOOL_FALSE;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==port_token){
                k_dn = 0;
                k_up = 0;
                for (k=1; k<param_space_sister; k++){
                    j = (i+k)%param_cal_len;
                    if (param_cal_main[j]>=param_phy_lo &&
                        param_cal_main[j]<=param_phy_hi ){
                        tsc_tmp = tdm_hx5_vmap_get_port_pm(param_cal_main[j], _tdm);
                        if (tsc_tmp==port_tsc){
                            k_dn = k;
                            break;
                        }
                    }
                }
                for (k=1; k<param_space_sister; k++){
                    j = (param_cal_len+i-k)%param_cal_len;
                    if (param_cal_main[j]>=param_phy_lo &&
                        param_cal_main[j]<=param_phy_hi ){
                        tsc_tmp = tdm_hx5_vmap_get_port_pm(param_cal_main[j], _tdm);
                        if (tsc_tmp==port_tsc){
                            k_up = k;
                            break;
                        }
                    }
                }
                /* shift up */
                if (k_dn>0){
                    for (j=0; j<param_cal_len; j++){
                        if (param_cal_main[j]==port_token){
                            k = (param_cal_len+j-param_space_sister)%param_cal_len;
                            tsc_tmp = tdm_hx5_vmap_get_port_pm(param_cal_main[k], _tdm);
                            if (tsc_tmp!=port_tsc){
                                x1 = j;
                                x2 = (x1>0)?(x1-1):(param_cal_len-1);
                                param_cal_main[x1] = param_cal_main[x2];
                                param_cal_main[x2] = port_token;
                                TDM_PRINT3("TDM: shift port %d UP from slot #%3d to slot #%3d\n",
                                          port_token, x1, x2 );
                            }
                        }
                    }
                    is_done = BOOL_TRUE;
                }
                /* shift down */
                else if (k_up>0){
                    for (j=0; j<param_cal_len; j++){
                        if (param_cal_main[j]==port_token){
                            k = (j+param_space_sister)%param_cal_len;
                            tsc_tmp = tdm_hx5_vmap_get_port_pm(param_cal_main[k], _tdm);
                            if (tsc_tmp!=port_tsc){
                                x1 = j;
                                x2 = (x1+1)%param_cal_len;
                                param_cal_main[x1] = param_cal_main[x2];
                                param_cal_main[x2] = port_token;
                                TDM_PRINT3("TDM: shift port %d DOWN from slot #%03d to slot #%03d\n",
                                          port_token, x1, x2 );
                            }
                        }
                    }
                    is_done = BOOL_TRUE;
                }
            }
            if (is_done==BOOL_TRUE){
                break;
            }
        }
        if (is_done==BOOL_FALSE){
            break;
        }
    }
    
    return PASS;
}


/**
@name: tdm_hx5_vmap_filter_same
@param:
 */
int
tdm_hx5_vmap_filter_same(tdm_mod_t *_tdm)
{
    int i, j, v, m, port_i, port_j,
        x, y, result=PASS;
    int port, chk_x, chk_y;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_space_same,
        param_phy_lo, param_phy_hi;
    int *param_cal_main;
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_same  = _tdm->_core_data.rule__same_port_min;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    TDM_PRINT0("TDM: Adjust same port space\n\n");
    for (i=0; i<param_cal_len; i++) {
        port_i = param_cal_main[i];
        if (port_i>=param_phy_lo && port_i<=param_phy_hi) {
            /* UP */
            for (v=1; v<param_space_same; v++) {
                j = (i+v)%param_cal_len;
                port_j = param_cal_main[j];
                if (port_j==port_i) {
                    for (m=1; m<(param_space_same-v); m++) {
                        x = (i + param_cal_len - m) % param_cal_len;
                        y = (x + 1) % param_cal_len;
                        chk_x = tdm_hx5_vmap_chk_shift_cond(x, TDM_DIR_DN,
                                    param_cal_len, param_cal_main, _tdm);
                        chk_y = tdm_hx5_vmap_chk_shift_cond(y, TDM_DIR_UP,
                                    param_cal_len, param_cal_main, _tdm);
                        if (chk_x == PASS || chk_y == PASS) {
                            port = param_cal_main[y];
                            param_cal_main[y] = param_cal_main[x];
                            param_cal_main[x] = port;
                            TDM_PRINT3("\t Shift port %d UP from slot %d to slot %d\n",
                                      port, y, x);
                        } else {
                            TDM_PRINT5("TDM: %s port %3d, slot [%3d, %3d(%d)]\n",
                                       "[same-port-space violation]",
                                       param_cal_main[y], i, j, y);
                            break;
                        }
                    }
                    

                    break;
                }
            }
            /* DOWN */
            for (v=1; v<param_space_same; v++) {
                j = (i+v)%param_cal_len;
                port_j = param_cal_main[j];
                if (port_j==port_i) {
                    for (m=1; m<(param_space_same-v); m++) {
                        x = (j + m) % param_cal_len;
                        y = (x + param_cal_len - 1) % param_cal_len;
                        chk_x = tdm_hx5_vmap_chk_shift_cond(x, TDM_DIR_UP,
                                    param_cal_len, param_cal_main, _tdm);
                        chk_y = tdm_hx5_vmap_chk_shift_cond(y, TDM_DIR_DN,
                                    param_cal_len, param_cal_main, _tdm);
                        if (chk_x == PASS || chk_y == PASS) {
                            port = param_cal_main[y];
                            param_cal_main[y] = param_cal_main[x];
                            param_cal_main[x] = port;
                            TDM_PRINT3("\t Shift port %d DOWN from slot %d to slot %d\n",
                                      port, y, x);
                        } else {
                            TDM_PRINT5("TDM: %s port %3d, slot [%3d, %3d(%d)]\n",
                                       "[same-port-space violation]",
                                       param_cal_main[y], i, j, y);
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    result = tdm_hx5_vmap_chk_sister_1D(_tdm);
    return (result);
}


/**
@name: tdm_hx5_vmap_filter_sister
@param:
 */
int
tdm_hx5_vmap_filter_sister(tdm_mod_t *_tdm)
{
    int i, j, v, n, port, port_i, port_j, tsc_i, tsc_j,
        sister_swap, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_lr_cnt, param_space_sister,
        param_phy_lo, param_phy_hi;
    int *param_cal_main;
    enum port_speed_e *param_speeds;
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_lr_cnt      = _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    TDM_PRINT0("TDM: Adjust sister port space (0)\n\n");
    for (n=0; n<param_lr_cnt; n++){
        sister_swap = BOOL_FALSE;
        for (i=0; i<param_cal_len; i++){
            port_i = param_cal_main[i];
            if (port_i>=param_phy_lo && port_i<=param_phy_hi){
                tsc_i = tdm_hx5_vmap_get_port_pm(port_i, _tdm);
                for (v=1; v<param_space_sister; v++){
                    j = (i+v)%param_cal_len;
                    port_j = param_cal_main[j];
                    if (port_j>=param_phy_lo && port_j<=param_phy_hi){
                        tsc_j = tdm_hx5_vmap_get_port_pm(port_j, _tdm);
                        if (tsc_i==tsc_j && port_i!=port_j){
                            port = (param_speeds[port_i] < param_speeds[port_j]) ?
                                   port_i : port_j;
                            tdm_hx5_vmap_shift_1D(_tdm, port);
                            sister_swap = BOOL_TRUE;
                            break;
                        }
                    }
                }
            }
        }
        if (sister_swap==BOOL_FALSE){
            break;
        }
    }
    result = tdm_hx5_vmap_chk_sister_1D(_tdm);
    return (result);
}


/**
@name: tdm_hx5_vmap_calc_slot_pos
@param:
 */
int
tdm_hx5_vmap_calc_slot_pos(int vmap_idx, int slot_idx, int vmap_idx_max, int slot_idx_max, int **r_a_arr)
{
    int i, j, a, b, n, pos, check;

    pos = slot_idx_max+1;
    if (r_a_arr!=NULL &&
        vmap_idx>=0 && vmap_idx<vmap_idx_max && 
        slot_idx>=0 && slot_idx<slot_idx_max){
        check = PASS;
        for (i=0; i<=vmap_idx; i++){
            if (r_a_arr[i][0]<=0 || r_a_arr[i][1]<=0){
                check = FAIL;
                break;
            }
            if ( i!=vmap_idx && r_a_arr[i][0]==r_a_arr[i][1] ){
                check = FAIL;
                break;
            }
        }
        if (check == PASS){
            i = slot_idx+1;
            n = vmap_idx+1;
            for (j=n; j>0; j--){
                a = r_a_arr[j-1][0];
                b = r_a_arr[j-1][1];
                if (j==n){
                    pos = 1 + tdm_math_div_floor(((i-1)*b), a);
                }
                else{
                    pos = pos + tdm_math_div_ceil((pos*a), (b-a));
                }
            }
        }
    }

    return pos;
}


/**
@name: tdm_hx5_vmap_alloc_one_port
@param:
 */
int
tdm_hx5_vmap_alloc_one_port(int port_token, int slot_req, int slot_left, int cal_len, int vmap_idx, int vmap_wid_max, int vmap_len_max, unsigned short **vmap, int **r_a_arr)
{
    int i, pos, slot_cnt=0, result=FAIL;

    if (vmap!=NULL  && r_a_arr!=NULL &&
        vmap_idx>=0 && vmap_idx <vmap_wid_max &&
        slot_req>0  && slot_req<=slot_left){
        r_a_arr[vmap_idx][0] = slot_req;
        r_a_arr[vmap_idx][1] = slot_left;

        for (i=0; i<slot_req; i++){
            pos = tdm_hx5_vmap_calc_slot_pos(vmap_idx, i,
                                         vmap_wid_max, cal_len,
                                         r_a_arr);
            if(pos>0 && pos<=cal_len && pos<=vmap_len_max){
                vmap[vmap_idx][pos-1] = port_token;
                slot_cnt++;
            }
            else{
                TDM_ERROR7(
                    "%s, port %0d, %0d-th slot (over %0d), %s <%0d,%0d>\n",
                    "pos OVERRUN in vmap allocation",
                    port_token, i, slot_req, "<pos, pos_max>=", pos, cal_len);
            }
        }
        
        if (slot_cnt==slot_req){
            result = PASS;
        }
    }
    else{
        TDM_ERROR4("%s, slot_req %0d, slot_avail %0d, vmap_idx %0d\n",
                  "invalid condition for pos calculation in vmap allocation",
                  slot_req, slot_left, vmap_idx);
    }
    
    return result;
}


/**
@name: tdm_hx5_vmap_alloc
@param:
 */
int
tdm_hx5_vmap_alloc(tdm_mod_t *_tdm)
{
    int i, j, k, n, port_token, result_tmp, result=PASS;
    int port_phynum, port_state, port_speed, port_existed,
        port_pm, port_pm_existed, 
        pm_cnt, pm_idx, pm_slot_sum, pm_slot_sum_tmp,
        vmap_idx, slot_req, slot_left, lr_cnt, swap_exist, port_cnt,
        port_0, port_1, port_2, spd_0, spd_1, spd_2, 
        port_slot_num, lr_slot_num=0, os_slot_num;
    int os_2_lr_r, lr_2_os_r, abs_r, abs_n, pm_pseudo_num,
        empty_slot_num, empty_slot_left, empty_slot_allc, slot_allc;
    int a0, a1, a2, a3, b, 
        pm_slot_sum_curr, pm_slot_sum_prev;
    int spd_max_curr, spd_max_prev;
    char lr_en, os_en;
    
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty,
        param_num_phy_pm, param_num_pm_lanes,
        param_phy_lo, param_phy_hi,
        param_num_lr, param_num_os,
        param_vmap_wid, param_vmap_len;
    int param_token_unused;
    int *param_lr_buff, *param_os_buff;
    unsigned short **param_vmap;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    
    int *lr_buff, *pm_buff, *pm_port_cnt;
    int **pm_pmap;
    int **r_a_arr;
    unsigned short **vmap;
    
    /* print stack usage */
    /* TDM_PRINT_STACK_SIZE("tdm_hx5_core_vmap_alloc"); */
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_num_phy_pm  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_num_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_num_lr      = _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr;
    param_num_os      = _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;
    param_lr_buff     = _tdm->_core_data.vars_pkg.lr_buffer;
    param_os_buff     = _tdm->_core_data.vars_pkg.os_buffer;
    param_vmap        = _tdm->_core_data.vmap;
    param_token_unused= 0; /* used by vmap allocation only */

    /* allocate memory */
    lr_buff     = (int*) TDM_ALLOC(param_vmap_wid*sizeof(int), 
                                   "tdm_hx5_vmap_alloc, lr_buff");
    pm_buff     = (int*) TDM_ALLOC(param_num_phy_pm*sizeof(int), 
                                   "tdm_hx5_vmap_alloc, pm_a");
    pm_port_cnt = (int*) TDM_ALLOC(param_num_phy_pm*sizeof(int), 
                                   "tdm_hx5_vmap_alloc, pm_c");
    pm_pmap     = (int**) TDM_ALLOC(param_num_phy_pm*sizeof(int*), 
                                    "tdm_hx5_vmap_alloc, pm_d1");
    for (i=0; i<param_num_phy_pm; i++) {
        pm_pmap[i] = (int*) TDM_ALLOC(param_num_pm_lanes*sizeof(int),
                                      "tdm_hx5_vmap_alloc, pm_d2");
    }
    r_a_arr = (int**) TDM_ALLOC(param_vmap_wid*sizeof(int*), 
                                "tdm_hx5_vmap_alloc, r_a_arr_l1");
    for (i=0; i<param_vmap_wid; i++) {
        r_a_arr[i] = (int*) TDM_ALLOC(2*sizeof(int),
                                      "tdm_hx5_vmap_alloc, r_a_arr_l2");
    }
    vmap = (unsigned short**) TDM_ALLOC(param_vmap_wid*sizeof(unsigned short*),
                                        "tdm_hx5_vmap_alloc, vmap_l1");
    for (i=0; i<param_vmap_wid; i++) {
        vmap[i] = (unsigned short*)
                  TDM_ALLOC(param_vmap_len*sizeof(unsigned short),
                            "tdm_hx5_vmap_alloc, vmap_l2");
    }
    /* set allocated memory */
    for (i=0; i<param_vmap_wid; i++) {
        lr_buff[i] = param_token_empty;
    }
    for (i=0; i<param_num_phy_pm; i++){
        pm_buff[i]    = param_token_empty;
        pm_port_cnt[i]= 0;
    }
    for (i=0; i<param_num_phy_pm; i++){
        for (j=0; j<param_num_pm_lanes; j++){
            pm_pmap[i][j] = param_token_empty;
        }
    }
    for (i=0; i<param_vmap_wid; i++) {
        r_a_arr[i][0] = 0; 
        r_a_arr[i][1] = 0;
    }
    for (i=0; i<param_vmap_wid; i++) {
        for (j=0; j<param_vmap_len; j++){
            vmap[i][j] = param_token_empty;
        }
    }
    
    /* initialize variables */
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40 = BOOL_FALSE;
    lr_en     = (param_lr_buff[0]!=param_token_empty)?(BOOL_TRUE):(BOOL_FALSE);
    os_en     = (param_os_buff[0]!=param_token_empty)?(BOOL_TRUE):(BOOL_FALSE);
    slot_left = param_cal_len;
    vmap_idx  = 0;
    
    lr_cnt    = 0;
    for (i=0; i<TDM_AUX_SIZE; i++) {
        port_phynum = param_lr_buff[i];
        if (port_phynum!=param_token_empty){
            port_existed = BOOL_FALSE;
            for (j=0; j<lr_cnt && j<param_vmap_wid; j++){
                if (lr_buff[j]==port_phynum){
                    port_existed = BOOL_TRUE;
                    break;
                }
            }
            if (port_existed==BOOL_FALSE && lr_cnt<param_vmap_wid){
                lr_buff[lr_cnt++] = port_phynum;
            }
        }
    }
    
    /* check linerate ports */
    if (lr_en==BOOL_TRUE){
        for (i=0; i<param_vmap_wid ; i++){
            port_phynum = lr_buff[i];
            if (port_phynum==param_token_empty){
                break;
            }
            if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi){
                port_state = param_states[port_phynum-1];
                port_speed = param_speeds[port_phynum];
                if (port_state==PORT_STATE__LINERATE    ||
                    port_state==PORT_STATE__LINERATE_HG ){
                    port_slot_num = tdm_hx5_vmap_get_speed_slots(port_speed);
                    lr_slot_num  += port_slot_num;
                    if (!(port_slot_num>0)){
                        result  = FAIL;
                        TDM_ERROR4("%s, port %0d speed %0dM (%0dG)\n",
                           "Unrecognized port speed in vmap allocation",
                           port_phynum, 
                           port_speed,
                           port_speed/1000);
                    }
                }
                else {
                    result = FAIL;
                    TDM_ERROR3("%s, port %0d state %0d\n",
                        "Unrecognized port state in vmap allocation",
                        port_phynum, port_state);
                }
            }
            else {
                result = FAIL;
                TDM_ERROR2("%s, port %0d\n", 
                    "Unrecognized port number in vmap allocation",
                    port_phynum);
            }
        }
    }
    
    os_slot_num    = (os_en==BOOL_TRUE)?(param_lr_limit-lr_slot_num):(0);
    empty_slot_num = param_cal_len - lr_slot_num;
    empty_slot_left= empty_slot_num;
    
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Populating VMAP ... \n\n");
    TDM_PRINT1("\t -- number of lineRate ports: %3d\n", param_num_lr);
    TDM_PRINT1("\t -- number of OverSub  ports: %3d\n\n", param_num_os);
    TDM_PRINT1("\t -- number of lineRate slots: %3d\n", lr_slot_num);
    TDM_PRINT1("\t -- number of OverSub  slots: %3d\n\n", os_slot_num);
    
    TDM_PRINT1("\t -- cal_len  : %3d\n", param_cal_len);
    TDM_PRINT1("\t \t -- ancl_slot: %3d\n", param_ancl_num);
    TDM_PRINT1("\t \t -- lr_limit : %3d\n", param_lr_limit);
    TDM_PRINT1("\t \t \t -- number of linerate slots: %3d\n", lr_slot_num);
    TDM_PRINT1("\t \t \t -- number of oversub  slots: %3d\n", os_slot_num);
    TDM_SML_BAR
    
    if ((lr_slot_num>param_lr_limit ) ||
        (lr_slot_num+param_ancl_num)>param_cal_len){
        TDM_WARN5("TDM: %s, [lr_slot_req %d, lr_slot_limit %d], [slot_sum %d, cal_len %d]\n",
                  "Bandwidth overflow in vmap allocation",
                  lr_slot_num,
                  param_lr_limit,
                  lr_slot_num+param_ancl_num,
                  param_cal_len);
		  return FAIL;
    }
    
    if (lr_en==BOOL_TRUE && result==PASS){
        /* sort ports by speed from highest to lowest */
        for (i=0; i<param_vmap_wid; i++){
            if (lr_buff[i]==param_token_empty){
                break;
            }
            for (j=(param_vmap_wid-1); j>i; j--){
                if (lr_buff[j]==param_token_empty){
                    continue;
                }
                if (lr_buff[j-1]>=param_phy_lo && 
                    lr_buff[j-1]<=param_phy_hi &&
                    lr_buff[j]>=param_phy_lo   && 
                    lr_buff[j]<=param_phy_hi   ){
                    if (param_speeds[lr_buff[j-1]]<param_speeds[lr_buff[j]]){
                        port_phynum  = lr_buff[j-1];
                        lr_buff[j-1] = lr_buff[j];
                        lr_buff[j]   = port_phynum;
                    }
                }
            }
        }
        /* construct pm_pmap for linerate ports */
        TDM_PRINT0("TDM: Construct linerate pm_pmap\n\n");
        pm_cnt = 0;
        for (i=0; i<param_vmap_wid; i++){
            port_phynum = lr_buff[i];
            if (port_phynum==param_token_empty){
                break;;
            }
            if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi){
                port_pm = tdm_hx5_vmap_get_port_pm(port_phynum, _tdm);
                if (port_pm<param_num_phy_pm){
                    port_pm_existed = BOOL_FALSE;
                    port_speed      = param_speeds[port_phynum];
                    
                    for (j=0; j<param_num_phy_pm && j<pm_cnt; j++){
                        if (pm_buff[j]==param_token_empty){
                            break;
                        }
                        else if (pm_buff[j]==port_pm){
                            port_pm_existed = BOOL_TRUE;
                            break;
                        }
                    }
                    if (port_pm_existed==BOOL_FALSE) {
                        if (pm_cnt<param_num_phy_pm){
                            pm_idx = pm_cnt;
                            pm_buff[pm_cnt++] = port_pm;
                        }
                        else{
                            pm_idx = param_num_phy_pm-1;
                            result = FAIL;
                            TDM_ERROR3("%s, port %0d, pm_idx %d\n", 
                                "Number of PM(a) exceeded in vmap allocation",
                                port_phynum,
                                pm_cnt);
                        }
                    }
                    else {
                        if (j<param_num_phy_pm){
                            pm_idx = j;
                        }
                        else {
                            pm_idx = param_num_phy_pm-1;
                            result = FAIL;
                            TDM_ERROR3("%s, port %0d, pm_idx %d\n", 
                                "Number of PM(b) exceeded in vmap allocation",
                                port_phynum,
                                pm_cnt);
                        }
                    }
                    
                    if (pm_port_cnt[pm_idx]<param_num_pm_lanes){
                        pm_pmap[pm_idx][pm_port_cnt[pm_idx]] = port_phynum;
                        pm_port_cnt[pm_idx] +=1;
                    }
                    else {
                        result = FAIL;
                        TDM_ERROR4("%s, port %0d, pm_idx %d, pm_lanes %d\n", 
                            "Number of PM lanes exceeded in vmap allocation",
                            port_phynum,
                            pm_idx,
                            pm_port_cnt[pm_idx]);
                    }
                }
            }
        }
        for (i=0; i<pm_cnt; i++){
            TDM_PRINT2("TDM: Add pm %2d, port_num %d, port [", 
                       i, pm_port_cnt[i]);
            for (j=0; j<pm_port_cnt[i]; j++){
                port_phynum = pm_pmap[i][j];
                TDM_PRINT1("%3d",  port_phynum);
                if (j!=(pm_port_cnt[i]-1)){
                    TDM_PRINT0(",");
                }
            }
            TDM_PRINT0("], speed [");
            for (j=0; j<pm_port_cnt[i]; j++){
                port_phynum = pm_pmap[i][j];
                if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi){
                    port_speed = param_speeds[port_phynum];
                    TDM_PRINT1("%3d", port_speed/1000);
                    if (j!=(pm_port_cnt[i]-1)){
                        TDM_PRINT0(",");
                    }
                }
            }
            TDM_PRINT0("]\n");
        }
        TDM_SML_BAR
        /* adjust pm_pmap for non-regular dual-/tri- mode pm */
        TDM_PRINT0("TDM: Check and/or adjust non-regular PM mode\n\n");
        for (i=0; i<param_num_phy_pm && i<pm_cnt; i++){
            port_token = pm_pmap[i][0];
            if (port_token!=param_token_empty){
                if (pm_port_cnt[i]==2){
                    port_0 = pm_pmap[i][0];
                    port_1 = pm_pmap[i][1];
                    if (port_0>=param_phy_lo && port_0<=param_phy_hi &&
                        port_1>=param_phy_lo && port_1<=param_phy_hi ){
                        spd_0 = param_speeds[port_0];
                        spd_1 = param_speeds[port_1];
                        if ((spd_0/1000)!=(spd_1/1000)){
                            TDM_PRINT6("%s, pm %d, port [%3d,%3d], speed[%3d,%3d]\n",
                                      "Special Dual-Mode in vmap allocation",
                                      i,
                                      port_0, port_1, 
                                      spd_0/1000, spd_1/1000);
                            slot_req = tdm_hx5_vmap_get_speed_slots(spd_1);
                            if ( (spd_0/1000)==(2*(spd_1/1000)) && 
                                 (slot_req+lr_slot_num<=param_lr_limit) ){
                                pm_pmap[i][2]  = port_1;
                                pm_port_cnt[i] = 6;
                                lr_slot_num += slot_req;
                                TDM_PRINT3("\t%s %3d with %d slots (IDLE token)\n",
                                           "-- Insert extra pseudo linerate port",
                                           port_1,
                                           slot_req);
                            }
                            else if (pm_cnt<param_num_phy_pm){
                                pm_pmap[pm_cnt][0]  = port_1;
                                pm_port_cnt[pm_cnt] = 1;
                                pm_cnt++;
                                pm_pmap[i][1]  = param_token_empty;
                                pm_port_cnt[i] = 1;
                                TDM_PRINT4("\t-- Split pm into [%d, %d], port [%3d,%3d]\n",
                                           i, pm_cnt,
                                           port_0, port_1);
                            }
                            TDM_PRINT0("\n");
                        }
                    }
                }
                else if (pm_port_cnt[i]==3){
                    port_0 = pm_pmap[i][0];
                    port_1 = pm_pmap[i][1];
                    port_2 = pm_pmap[i][2];
                    if (port_0>=param_phy_lo && port_0<=param_phy_hi &&
                        port_1>=param_phy_lo && port_1<=param_phy_hi &&
                        port_2>=param_phy_lo && port_2<=param_phy_hi ){
                        spd_0 = param_speeds[port_0];
                        spd_1 = param_speeds[port_1];
                        spd_2 = param_speeds[port_2];
                        if (spd_0!=(spd_1*2) && spd_1==spd_2 && 
                            pm_cnt<param_num_phy_pm){
                            pm_pmap[pm_cnt][0]  = port_1;
                            pm_pmap[pm_cnt][1]  = port_2;
                            pm_port_cnt[pm_cnt] = 2;
                            pm_cnt++;
                            pm_pmap[i][1]  = param_token_empty;
                            pm_pmap[i][2]  = param_token_empty;
                            pm_port_cnt[i] = 1;
                            TDM_PRINT7("%s, port [%d, %d, %d], speed [%d, %d, %d]\n",
                                      "Special Tri-Mode in vmap allocation",
                                      port_0, port_1, port_2,
                                      spd_0/1000, spd_1/1000, spd_2/1000);
                            _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_TRUE;
                        }
                    }
                }
            }
        }
        TDM_SML_BAR
        /* adjust pm_pmap for high-speed PM with 20G, 25G, 40G, 50G */
        TDM_PRINT0("TDM: Sort linerate PMs\n\n");
        for (n=0; n<param_num_phy_pm && n<pm_cnt; n++){
            swap_exist = BOOL_FALSE;
            pm_slot_sum_curr = 0;
            spd_max_curr  = 0;
            for (i=0; i<param_num_phy_pm && i<pm_cnt; i++){
                port_token = pm_pmap[i][0];
                if (port_token>=param_phy_lo && port_token<=param_phy_hi){
                    spd_max_prev = spd_max_curr;
                    spd_max_curr = 0;
                    pm_slot_sum_prev = pm_slot_sum_curr;
                    pm_slot_sum_curr = 0;
                    for (j=0; j<param_num_pm_lanes; j++){
                        port_phynum = pm_pmap[i][j];
                        if (port_phynum>=param_phy_lo && 
                            port_phynum<=param_phy_hi){
                            port_speed = param_speeds[port_phynum];
                            pm_slot_sum_curr += tdm_hx5_vmap_get_speed_slots(port_speed);
                            spd_max_curr= (port_speed>spd_max_curr)?
                                          (port_speed):
                                          (spd_max_curr);
                        }
                    }
                    if (i>0 && pm_slot_sum_curr>pm_slot_sum_prev &&
                        spd_max_prev<SPEED_100G){
                        a0 = pm_pmap[i][0];
                        a1 = pm_pmap[i][1];
                        a2 = pm_pmap[i][2];
                        a3 = pm_pmap[i][3];
                        b  = pm_port_cnt[i];
                        
                        pm_pmap[i][0] = pm_pmap[i-1][0];
                        pm_pmap[i][1] = pm_pmap[i-1][1];
                        pm_pmap[i][2] = pm_pmap[i-1][2];
                        pm_pmap[i][3] = pm_pmap[i-1][3];
                        pm_port_cnt[i]= pm_port_cnt[i-1];
                        
                        pm_pmap[i-1][0] = a0;
                        pm_pmap[i-1][1] = a1;
                        pm_pmap[i-1][2] = a2;
                        pm_pmap[i-1][3] = a3;
                        pm_port_cnt[i-1]= b;
                        
                        TDM_PRINT4("TDM: Swap pm %d with pm %d, port %d with port %d\n",
                                  i-1, i, pm_pmap[i][0], pm_pmap[i-1][0]);
                        
                        swap_exist = BOOL_TRUE;
                    }
                }
                else {
                    break;
                }
            }
            if (swap_exist==BOOL_FALSE){
                break;
            }
        }
        TDM_SML_BAR
        /* create pseudo pm(s) for EMPTY slots */
        empty_slot_num = param_cal_len - lr_slot_num;
        empty_slot_left= empty_slot_num;
        if (empty_slot_num>0){
            lr_2_os_r    = 0;
            os_2_lr_r    = 0;
            if (empty_slot_num>lr_slot_num && lr_slot_num>0){
                /* threshold 5% */
                os_2_lr_r = (empty_slot_num*105)/(lr_slot_num*100);
            }
            else if (empty_slot_num<=lr_slot_num && empty_slot_num>0) {
                lr_2_os_r = lr_slot_num/empty_slot_num;
            }
            
            TDM_PRINT0("TDM: Insert pseudo PM for non-linerate (EMPTY) slots\n\n");
            TDM_PRINT1("\t -- number of linerate slots    : %3d\n", lr_slot_num);
            TDM_PRINT1("\t -- number of non-linerate slots: %3d\n", empty_slot_num);
            if (empty_slot_num>lr_slot_num){
                TDM_PRINT0("\t -- Linerate type   : Minority\n");
                TDM_PRINT1("\t -- empty_2_lr_ratio: %3d\n", os_2_lr_r);
            }
            else {
                TDM_PRINT0("\t -- Linerate type   : Majority\n");
                TDM_PRINT1("\t -- lr_2_empty_ratio: %3d\n", lr_2_os_r);
            }
            TDM_PRINT0("\n");
            /* bw_os > bw_lr */
            if (os_2_lr_r>0){
                for (i=0; i<(param_num_phy_pm-1); i++){
                    pm_slot_sum = 0;
                    if (pm_pmap[i][0]==param_token_empty){
                        break;
                    }
                    for (j=0; j<param_num_pm_lanes; j++){
                        port_phynum = pm_pmap[i][j];
                        if (port_phynum>=param_phy_lo && 
                            port_phynum<=param_phy_hi){
                            port_speed = param_speeds[port_phynum];
                            pm_slot_sum += tdm_hx5_vmap_get_speed_slots(port_speed);
                        }
                    }
                    if (pm_slot_sum>0 && empty_slot_left>0){
                        abs_r = lr_slot_num/pm_slot_sum;
                        /* abs_n = empty_slot_num/abs_r; */
                        abs_n = pm_slot_sum*os_2_lr_r;
                        abs_n = (abs_n>0)?(abs_n):(1);
                        slot_allc= (abs_n<=empty_slot_left)? 
                                   (abs_n): 
                                   (empty_slot_left);
                        pm_pseudo_num = tdm_math_div_ceil(slot_allc,
                                            pm_slot_sum);
                        for (n=0; n<pm_pseudo_num; n++){
                            if (i<(param_num_phy_pm-1)){
                                empty_slot_allc = (pm_slot_sum<=slot_allc)?
                                                  (pm_slot_sum):
                                                  (slot_allc);
                                if (empty_slot_allc<pm_slot_sum){
                                    break;
                                }
                                if (n==0){
                                    for (k=(param_num_phy_pm-1); k>i; k--){
                                        for (j=0; j<param_num_pm_lanes; j++){
                                            pm_pmap[k][j] = pm_pmap[k-1][j];
                                        }
                                        pm_port_cnt[k]= pm_port_cnt[k-1];
                                    }
                                    slot_allc      -= empty_slot_allc;
                                    empty_slot_left-= empty_slot_allc;
                                    pm_pmap[i+1][0] = param_token_unused;
                                    pm_pmap[i+1][1] = empty_slot_allc;
                                    pm_pmap[i+1][2] = 1;
                                    pm_pmap[i+1][3] = param_token_empty;
                                    pm_port_cnt[i+1]= 1;
                                    pm_cnt++;
                                    i++;
                                }
                                else {
                                    slot_allc      -= empty_slot_allc;
                                    empty_slot_left-= empty_slot_allc;
                                    pm_pmap[i][2]++;
                                }
                                TDM_PRINT6("TDM: %s %2d with %2dx%2d slots (%s=%3d)\n",
                                        "Insert pseudo pm for EMPTY slot at index",
                                        i,
                                        pm_pmap[i][2],
                                        empty_slot_allc, 
                                        "empty_slot_left", 
                                        empty_slot_left);
                            }
                            else {
                                break;
                            }
                        }
                    }
                    if (empty_slot_left<=0){
                        if (empty_slot_left<0){
                            result = FAIL;
                            TDM_ERROR1("TDM: %d extra EMPTY slots allocated\n",
                                       (-empty_slot_left));
                        }
                        break;
                    }
                }
            }
            /* bw_lr > bw_os */
            else if (lr_2_os_r>0){
                pm_slot_sum = 0;
                for (i=0; i<(param_num_phy_pm-1); i++){
                    pm_slot_sum_tmp = 0;
                    for (j=0; j<param_num_pm_lanes; j++){
                        port_phynum = pm_pmap[i][j];
                        if (port_phynum>=param_phy_lo && 
                            port_phynum<=param_phy_hi){
                            port_speed = param_speeds[port_phynum];
                            pm_slot_sum_tmp += tdm_hx5_vmap_get_speed_slots(port_speed);
                        }
                    }
                    pm_slot_sum += pm_slot_sum_tmp;
                    if (pm_slot_sum>0 && empty_slot_left>0){
                        abs_r = lr_slot_num/pm_slot_sum;
                        abs_n = empty_slot_num/abs_r;
                        abs_n = (lr_2_os_r==1)?(pm_slot_sum):(abs_n);
                        abs_n = (abs_n<pm_slot_sum_tmp)?
                                (abs_n):
                                (pm_slot_sum_tmp);
                        if ((pm_slot_sum/lr_2_os_r>=pm_slot_sum_tmp) ||
                            (pm_slot_sum_tmp>0 && pm_pmap[i][0]==param_token_unused) ){
                            empty_slot_allc  = (abs_n<=empty_slot_left)? 
                                            (abs_n): 
                                            (empty_slot_left);
                            if (empty_slot_allc<pm_slot_sum_tmp){
                                continue;
                            }
                            for (k=(param_num_phy_pm-1); k>i; k--){
                                for (j=0; j<param_num_pm_lanes; j++){
                                    pm_pmap[k][j] = pm_pmap[k-1][j];
                                }
                                pm_port_cnt[k]= pm_port_cnt[k-1];
                            }
                            empty_slot_left -= empty_slot_allc;
                            pm_pmap[i+1][0] = param_token_unused;
                            pm_pmap[i+1][1] = empty_slot_allc;
                            pm_pmap[i+1][2] = 1;
                            pm_pmap[i+1][3] = param_token_empty;
                            pm_port_cnt[i+1]= 1;
                            pm_cnt++;
                            i++;
                            pm_slot_sum = 0;
                            TDM_PRINT6("TDM: %s %2d with %2dx%2d EMPTY slots (%s=%3d)\n",
                                    "Insert pseudo pm at index",
                                    i,
                                    pm_pmap[i][2],
                                    empty_slot_allc, 
                                    "empty_slot_left", 
                                    empty_slot_left);
                        }
                    }
                    if (empty_slot_left<=0){
                        if (empty_slot_left<0){
                            result = FAIL;
                            TDM_ERROR1("TDM: %d extra EMPTY slots allocated\n",
                                       (-empty_slot_left));
                        }
                        break;
                    }
                }
            }
            TDM_SML_BAR
        }
        /* allocate slots */
        TDM_PRINT0("TDM: Allocate PM \n\n");
        for (i=0; i<param_num_phy_pm && i<pm_cnt; i++){
            port_token = pm_pmap[i][0];
            port_cnt   = pm_port_cnt[i];
            if (port_token==param_token_unused && port_cnt==1){
                slot_req = pm_pmap[i][1];
                k        = pm_pmap[i][2];
                for (j=0; j<k; j++){
                    result_tmp = tdm_hx5_vmap_alloc_one_port(
                                    port_token, 
                                    slot_req, slot_left, 
                                    param_cal_len, vmap_idx,
                                    param_vmap_wid, param_vmap_len,
                                    vmap, r_a_arr);
                    TDM_PRINT4("TDM: Allocate pm %2d port %3d with %2d slots from %d\n",
                               i, port_token, slot_req, slot_left);
                    if (result_tmp==PASS){
                        slot_left -= slot_req;
                        vmap_idx++;
                    }
                    else {
                        /* result = FAIL; */
                        TDM_ERROR3("%s, slot_req %d, slot_avail %d\n",
                                  "FAILED in EMPTY slot vmap allocation",
                                  slot_req,
                                  slot_left);
                        break;
                    }
                }
            }
            else if (port_token!=param_token_empty && 
                     port_token!=param_token_unused){
                slot_req = 0;
                for (j=0; j<port_cnt && j<param_num_pm_lanes; j++){
                    port_phynum = pm_pmap[i][j];
                    if (port_phynum>=param_phy_lo && 
                        port_phynum<=param_phy_hi){
                        port_speed = param_speeds[port_phynum];
                        slot_req += tdm_hx5_vmap_get_speed_slots(port_speed);
                    }
                }
                /* SPECIAL CASE: 4x106HG+3x40G uses a unique TDM in Tomahawk */
                if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G==BOOL_TRUE){
                    slot_req -= 1;
                }
                result_tmp = tdm_hx5_vmap_alloc_one_port(
                                port_token, 
                                slot_req, slot_left, 
                                param_cal_len, vmap_idx,
                                param_vmap_wid, param_vmap_len,
                                vmap, r_a_arr);
                TDM_PRINT4("TDM: Allocate pm %2d port %3d with %2d slots from %d\n",
                           i, port_token, slot_req, slot_left);
                if (result_tmp==PASS){
                    switch(port_cnt){
                        case 1:
                            break;
                        case 2:
                            k = 0;
                            for (j=0; j<param_cal_len; j++){
                                if(vmap[vmap_idx][j]==port_token){
                                    switch(k){
                                        case 0:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%2);
                                }
                            }
                            break;
                        case 3:
                            k = 0;
                            for (j=0; j<param_cal_len; j++){
                                if(vmap[vmap_idx][j]==port_token){
                                    switch(k){
                                        case 0:
                                        case 2:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        case 3:
                                            vmap[vmap_idx][j]=pm_pmap[i][2];
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%4);
                                }
                            }
                            break;
                        case 4:
                            k = 0;
                            for (j=0; j<param_cal_len; j++){
                                if(vmap[vmap_idx][j]==port_token){
                                    switch(k){
                                        case 0:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        case 2:
                                            vmap[vmap_idx][j]=pm_pmap[i][2];
                                            break;
                                        case 3:
                                            vmap[vmap_idx][j]=pm_pmap[i][3];
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%4);
                                }
                            }
                            break;
                        case 6:
                            k = 0;
                            for (j=0; j<param_cal_len; j++){
                                if(vmap[vmap_idx][j]==port_token){
                                    switch(k){
                                        case 0:
                                        case 2:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        case 3:
                                            vmap[vmap_idx][j]=param_token_unused;
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%4);
                                }
                            }
                            break;
                        default:
                            TDM_ERROR3("%s, port %d, tsc active lanes %d\n",
                                      "Number of TSC lanes exceeded",
                                      port_token,
                                      port_cnt);
                            break;
                    }
                    slot_left -= slot_req;
                    vmap_idx++;
                }
                else{
                    result = FAIL;
                    TDM_ERROR4("%s, port %d, slot_req %d, slot_avail %d\n",
                              "FAILED in linerate slot vmap allocation",
                              port_token,
                              slot_req,
                              slot_left);
                }
            }
        }
        TDM_SML_BAR
        /* check vmap sister port spacing */
        if (tdm_hx5_vmap_chk_sister_2D(_tdm, vmap)!=PASS){
            tdm_hx5_vmap_rotate(_tdm, vmap);
        }
        /* check vmap singularity */
        if (tdm_hx5_vmap_chk_singularity(_tdm, vmap)!=PASS){
            result = FAIL;
        }
    }
    tdm_hx5_vmap_print(_tdm, vmap);
    /* copy local vmap to core vmap */
    for (i=0; i<param_vmap_wid; i++){
        for (j=0; j<param_vmap_len; j++){
            if (vmap[i][j]==param_token_unused){
                param_vmap[i][j] = param_token_empty;
            }
            else {
                param_vmap[i][j] = vmap[i][j];
            }
        }
    }
    /* free memory */
    TDM_FREE(lr_buff);
    TDM_FREE(pm_buff);
    TDM_FREE(pm_port_cnt);
    for (i=0; i<param_num_phy_pm; i++){
        TDM_FREE(pm_pmap[i]);
    }
    TDM_FREE(pm_pmap);
    for (i=0; i<param_vmap_wid; i++){
        TDM_FREE(r_a_arr[i]);
        TDM_FREE(vmap[i]);
    }
    TDM_FREE(r_a_arr);
    TDM_FREE(vmap);
    
    return result;
}


/**
@name: tdm_hx5_core_vmap_alloc
@param:
 */
int
tdm_hx5_core_vmap_alloc(tdm_mod_t *_tdm)
{
    int i, j, v, n, spd_i, spd_j, port_i, port_j, tsc_i, tsc_j,
        port_token, sister_swap;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_lr_cnt, param_space_sister,
        param_token_empty,
        param_phy_lo, param_phy_hi,
        param_lr_en, 
        param_vmap_wid, param_vmap_len;
    int *param_cal_main;
    unsigned short **param_vmap;
    enum port_speed_e *param_speeds;
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_lr_cnt      = _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_vmap        = _tdm->_core_data.vmap;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    
    _tdm->_core_data.vars_pkg.refactor_done = BOOL_TRUE;
    
    /* allocate linerate slots */
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_lr_en==BOOL_TRUE){
        if(tdm_hx5_vmap_alloc(_tdm)!=PASS){
            return (TDM_EXEC_CORE_SIZE+1);
        }
        for (j=0; j<param_vmap_len && j<param_cal_len; j++){
            port_token = param_token_empty;
            for (i=0; i<param_vmap_wid; i++){
                if (param_vmap[i][j]!=param_token_empty){
                    port_token = param_vmap[i][j];
                    break;
                }
            }
            param_cal_main[j] = port_token;
        }
    }
    else {
        for (j=0; j<param_vmap_len && j<param_cal_len; j++){
            param_cal_main[j] = param_token_empty;
        }
    }
    /* check/adjust sister port space */
    if (tdm_hx5_vmap_chk_sister_1D(_tdm)!=PASS){
        TDM_PRINT0("TDM: Adjust sister port space (1)\n\n");
        for (n=0; n<param_lr_cnt; n++){
            sister_swap = BOOL_FALSE;
            for (i=0; i<param_cal_len; i++){
                port_i = param_cal_main[i];
                if (port_i>=param_phy_lo && port_i<=param_phy_hi){
                    tsc_i = tdm_hx5_vmap_get_port_pm(port_i, _tdm);
                    for (v=1; v<param_space_sister; v++){
                        j = (i+v)%param_cal_len;
                        port_j = param_cal_main[j];
                        if (port_j>=param_phy_lo && port_j<=param_phy_hi){
                            tsc_j = tdm_hx5_vmap_get_port_pm(port_j, _tdm);
                            if (tsc_i==tsc_j && port_i!=port_j){
                                spd_i = param_speeds[port_i];
                                spd_j = param_speeds[port_j];
                                if (spd_i<spd_j){
                                    tdm_hx5_vmap_shift_1D(_tdm, port_i);
                                }
                                else {
                                    tdm_hx5_vmap_shift_1D(_tdm, port_j);
                                }
                                sister_swap = BOOL_TRUE;
                                break;
                            }
                        }
                    }
                }
            }
            if (sister_swap==BOOL_FALSE){
                break;
            }
        }
        TDM_SML_BAR
        /* check sister spacing */
        if (tdm_hx5_vmap_chk_sister_1D(_tdm)!=PASS){
            _tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
        }
    }
    /* filter */
    
    return PASS;
}


/**
@name: tdm_hx5_vmap_alloc_mix
@param:
 */
int
tdm_hx5_vmap_alloc_mix(tdm_mod_t *_tdm)
{
    int i, j, k, n, port_token, result_tmp, result=PASS;
    int port_phynum, port_state, port_speed, port_existed,
        port_pm, port_pm_existed, 
        pm_cnt, pm_idx,
        vmap_idx=0, slot_req, slot_left, lr_cnt, swap_exist,
        port_slot_num, lr_slot_num=0, os_slot_num;
    char lr_en, os_en;
    int prt_slot, act_pm_num,
        prt_0, prt_1, prt_2, prt_3,
        spd_0, spd_1, spd_2, spd_3,
        pm_prt_cnt, pm_slot_sum=0, pm_pseudo_slot_sum=0,
        pm_mode, pm_special_mode_en, pm_extra_bw_en, pm_extra_bw;
    int os_2_lr_r, lr_2_os_r, abs_r, abs_n, pm_pseudo_num, pm_slot_sum_tmp,
        empty_slot_num, empty_slot_left, empty_slot_allc,
        lr_slot_num_e, slot_allc;
    int dice_2_1_0_0=0, dice_4_1_1_0=0;
    
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty, param_token_ovsb,
        param_num_phy_pm, param_num_pm_lanes,
        param_phy_lo, param_phy_hi,
        param_num_lr, param_num_os,
        param_vmap_wid, param_vmap_len;
    int *param_lr_buff, *param_os_buff;
    unsigned short **param_vmap;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    
    int *lr_buff, *pm_buff, *pm_port_cnt, *pm_slot_req, *pm_modes;
    int **pm_pmap;
    int **r_a_arr;
    unsigned short **vmap;
    
    /* print stack usage */
    /* TDM_PRINT_STACK_SIZE("tdm_hx5_core_vmap_alloc"); */
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_num_phy_pm  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_num_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_num_lr      = _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr;
    param_num_os      = _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;
    param_lr_buff     = _tdm->_core_data.vars_pkg.lr_buffer;
    param_os_buff     = _tdm->_core_data.vars_pkg.os_buffer;
    param_vmap        = _tdm->_core_data.vmap;

    /* allocate memory */
    lr_buff     = (int*) TDM_ALLOC(param_vmap_wid*sizeof(int), 
                                   "tdm_hx5_vmap_alloc_lr, lr_buff");
    pm_buff     = (int*) TDM_ALLOC(param_num_phy_pm*sizeof(int), 
                                   "tdm_hx5_vmap_alloc_lr, pm_buff");
    pm_port_cnt = (int*) TDM_ALLOC(param_num_phy_pm*sizeof(int), 
                                   "tdm_hx5_vmap_alloc_lr, pm_port_cnt");
    pm_slot_req = (int*) TDM_ALLOC(param_num_phy_pm*sizeof(int), 
                                   "tdm_hx5_vmap_alloc_lr, pm_slot_req");
    pm_modes    = (int*) TDM_ALLOC(param_num_phy_pm*sizeof(int), 
                                   "tdm_hx5_vmap_alloc_lr, pm_modes");
    pm_pmap     = (int**) TDM_ALLOC(param_num_phy_pm*sizeof(int*), 
                                    "tdm_hx5_vmap_alloc_lr, pm_pmap_l1");
    for (i=0; i<param_num_phy_pm; i++) {
        pm_pmap[i] = (int*) TDM_ALLOC(param_num_pm_lanes*sizeof(int),
                                      "tdm_hx5_vmap_alloc_lr, pm_pmap_l2");
    }
    r_a_arr = (int**) TDM_ALLOC(param_vmap_wid*sizeof(int*), 
                                "tdm_hx5_vmap_alloc_lr, r_a_arr_l1");
    for (i=0; i<param_vmap_wid; i++) {
        r_a_arr[i] = (int*) TDM_ALLOC(2*sizeof(int),
                                      "tdm_hx5_vmap_alloc_lr, r_a_arr_l2");
    }
    vmap = (unsigned short**) TDM_ALLOC(param_vmap_wid*sizeof(unsigned short*),
                                        "tdm_hx5_vmap_alloc_lr, vmap_l1");
    for (i=0; i<param_vmap_wid; i++) {
        vmap[i] = (unsigned short*)
                  TDM_ALLOC(param_vmap_len*sizeof(unsigned short),
                            "tdm_hx5_vmap_alloc_lr, vmap_l2");
    }

    /* set allocated memory */
    for (i=0; i<param_vmap_wid; i++) {
        lr_buff[i] = param_token_empty;
    }
    for (i=0; i<param_num_phy_pm; i++) {
        pm_buff[i]     = param_token_empty;
        pm_port_cnt[i] = 0;
        pm_slot_req[i] = 0;
        pm_modes[i]    = 0;
    }
    for (i=0; i<param_num_phy_pm; i++) {
        for (j=0; j<param_num_pm_lanes; j++) {
            pm_pmap[i][j] = param_token_empty;
        }
    }
    for (i=0; i<param_vmap_wid; i++) {
        r_a_arr[i][0] = 0; 
        r_a_arr[i][1] = 0;
    }
    for (i=0; i<param_vmap_wid; i++) {
        for (j=0; j<param_vmap_len; j++) {
            vmap[i][j] = param_token_empty;
        }
    }

    /* initialize variables */
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40 = BOOL_FALSE;
    lr_en      = (param_lr_buff[0]!=param_token_empty)?(BOOL_TRUE):(BOOL_FALSE);
    os_en      = (param_os_buff[0]!=param_token_empty)?(BOOL_TRUE):(BOOL_FALSE);
    slot_left  = param_cal_len;

    lr_cnt    = 0;
    for (i=0; i<TDM_AUX_SIZE; i++) {
        port_phynum = param_lr_buff[i];
        if (port_phynum!=param_token_empty) {
            port_existed = BOOL_FALSE;
            for (j=0; j<lr_cnt && j<param_vmap_wid; j++) {
                if (lr_buff[j]==port_phynum) {
                    port_existed = BOOL_TRUE;
                    break;
                }
            }
            if (port_existed==BOOL_FALSE && lr_cnt<param_vmap_wid) {
                lr_buff[lr_cnt++] = port_phynum;
            }
        }
    }
    
    /* check linerate port validation */
    for (i=0; i<param_vmap_wid ; i++) {
        port_phynum = lr_buff[i];
        if (port_phynum==param_token_empty) {
            break;
        }
        if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi) {
            port_state = param_states[port_phynum-1];
            port_speed = param_speeds[port_phynum];
            if (port_state==PORT_STATE__LINERATE    ||
                port_state==PORT_STATE__LINERATE_HG ) {
                port_slot_num = tdm_hx5_vmap_get_speed_slots(port_speed);
                lr_slot_num  += port_slot_num;
                if (!(port_slot_num>0)) {
                    result  = FAIL;
                    TDM_ERROR4("%s, port %0d speed %0dM (%0dG)\n",
                       "Unrecognized port speed in vmap allocation",
                       port_phynum, port_speed, port_speed/1000);
                }
            } else {
                result = FAIL;
                TDM_ERROR3("%s, port %0d state %0d\n",
                    "Unrecognized port state in vmap allocation",
                    port_phynum, port_state);
            }
        } else {
            result = FAIL;
            TDM_ERROR2("%s, port %0d\n", 
                "Unrecognized port number in vmap allocation",
                port_phynum);
        }
    }

    /* print bandwidth info summary */
    os_slot_num= (os_en==BOOL_TRUE)?(param_lr_limit-lr_slot_num):(0);
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Populating VMAP ... \n\n");
    TDM_PRINT1("\t -- number of lineRate ports: %3d\n", param_num_lr);
    TDM_PRINT1("\t -- number of OverSub  ports: %3d\n\n", param_num_os);
    TDM_PRINT1("\t -- number of lineRate slots: %3d\n", lr_slot_num);
    TDM_PRINT1("\t -- number of OverSub  slots: %3d\n\n", os_slot_num);
    
    TDM_PRINT1("\t -- cal_len  : %3d\n", param_cal_len);
    TDM_PRINT1("\t \t -- ancl_slot: %3d\n", param_ancl_num);
    TDM_PRINT1("\t \t -- lr_limit : %3d\n", param_lr_limit);
    TDM_PRINT1("\t \t \t -- number of linerate slots: %3d\n", lr_slot_num);
    TDM_PRINT1("\t \t \t -- number of oversub  slots: %3d\n", os_slot_num);
    TDM_SML_BAR
    
    if ((lr_slot_num>param_lr_limit ) ||
        (lr_slot_num+param_ancl_num)>param_cal_len) {
        TDM_WARN5("TDM: %s, [lr_slot_req, lr_slot_limit] = [%d, %d], [cal_len_req, cal_len_limit] = [%d, %d]\n",
                  "Bandwidth overflow in vmap allocation",
                  lr_slot_num, param_lr_limit,
                  lr_slot_num+param_ancl_num, param_cal_len);
    }
    
    if (lr_en==BOOL_TRUE && result==PASS) {
        /* sort ports by speed from highest to lowest */
        for (i=0; i<param_vmap_wid; i++){
            if (lr_buff[i]==param_token_empty) {
                break;
            }
            for (j=(param_vmap_wid-1); j>i; j--) {
                if (lr_buff[j]==param_token_empty) {
                    continue;
                }
                if (lr_buff[j-1]>=param_phy_lo && 
                    lr_buff[j-1]<=param_phy_hi &&
                    lr_buff[j]>=param_phy_lo   && 
                    lr_buff[j]<=param_phy_hi   ) {
                    if (param_speeds[lr_buff[j-1]]<param_speeds[lr_buff[j]]) {
                        port_phynum  = lr_buff[j-1];
                        lr_buff[j-1] = lr_buff[j];
                        lr_buff[j]   = port_phynum;
                    }
                }
            }
        }

        /* construct pm_pmap
         * - among diff PMs, from highest port speed to lowest port speed
         * - within each pm, from highest subport speed to lowest subport speed
         */
        TDM_PRINT0("TDM: Construct pm_pmap[][]\n\n");
        pm_cnt = 0;
        for (i=0; i<param_vmap_wid; i++) {
            port_phynum = lr_buff[i];
            if (port_phynum==param_token_empty) {
                break;;
            }
            if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi) {
                port_pm = tdm_hx5_vmap_get_port_pm(port_phynum, _tdm);
                if (port_pm<param_num_phy_pm){
                    port_pm_existed = BOOL_FALSE;
                    port_speed      = param_speeds[port_phynum];
                    for (j=0; j<param_num_phy_pm && j<pm_cnt; j++) {
                        if (pm_buff[j]==param_token_empty) {
                            break;
                        } else if (pm_buff[j]==port_pm) {
                            port_pm_existed = BOOL_TRUE;
                            break;
                        }
                    }
                    if (port_pm_existed==BOOL_FALSE) {
                        if (pm_cnt<param_num_phy_pm) {
                            pm_idx = pm_cnt;
                            pm_buff[pm_cnt++] = port_pm;
                        } else {
                            pm_idx = param_num_phy_pm-1;
                            result = FAIL;
                            TDM_ERROR3("%s, port %0d, pm_idx %d\n", 
                                "Number of PM(a) exceeded in vmap allocation",
                                port_phynum, pm_cnt);
                        }
                    } else {
                        if (j<param_num_phy_pm) {
                            pm_idx = j;
                        } else {
                            pm_idx = param_num_phy_pm-1;
                            result = FAIL;
                            TDM_ERROR3("%s, port %0d, pm_idx %d\n", 
                                "Number of PM(b) exceeded in vmap allocation",
                                port_phynum, pm_cnt);
                        }
                    }
                    if (pm_port_cnt[pm_idx]<param_num_pm_lanes) {
                        pm_pmap[pm_idx][pm_port_cnt[pm_idx]] = port_phynum;
                        pm_slot_req[pm_idx] += tdm_hx5_vmap_get_speed_slots(port_speed);
                        pm_port_cnt[pm_idx] += 1;
                    } else {
                        result = FAIL;
                        TDM_ERROR4("%s, port %0d, pm_idx %d, pm_lanes %d\n", 
                            "Number of PM lanes exceeded in vmap allocation",
                            port_phynum, pm_idx, pm_port_cnt[pm_idx]);
                    }
                }
            }
        }
        for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) {
            TDM_PRINT2("TDM: Add pm %2d, port_num %d, port [", 
                       i, pm_port_cnt[i]);
            for (j=0; j<pm_port_cnt[i]; j++) {
                port_phynum = pm_pmap[i][j];
                TDM_PRINT1("%3d",  port_phynum);
                if (j!=(pm_port_cnt[i]-1)) {
                    TDM_PRINT0(",");
                }
            }
            TDM_PRINT0("], speed [");
            for (j=0; j<pm_port_cnt[i]; j++) {
                port_phynum = pm_pmap[i][j];
                if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi) {
                    port_speed = param_speeds[port_phynum];
                    TDM_PRINT1("%3d", port_speed/1000);
                    if (j!=(pm_port_cnt[i]-1)) {
                        TDM_PRINT0(",");
                    }
                }
            }
            TDM_PRINT0("]\n");
        }
        TDM_SML_BAR

        /* construct pm_modes list */
        TDM_PRINT0("TDM: Construct pm_modes[] \n\n");
        for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) {
            prt_0 = pm_pmap[i][0];
            prt_1 = pm_pmap[i][1];
            prt_2 = pm_pmap[i][2];
            prt_3 = pm_pmap[i][3];
            spd_0 = (prt_0>=param_phy_lo && prt_0<=param_phy_hi) ? (param_speeds[prt_0]/1000) : (0);
            spd_1 = (prt_1>=param_phy_lo && prt_1<=param_phy_hi) ? (param_speeds[prt_1]/1000) : (0);
            spd_2 = (prt_2>=param_phy_lo && prt_2<=param_phy_hi) ? (param_speeds[prt_2]/1000) : (0);
            spd_3 = (prt_3>=param_phy_lo && prt_3<=param_phy_hi) ? (param_speeds[prt_3]/1000) : (0);
            
            pm_modes[i] = TDM_VMAP_PM_MODE_0_0_0_0;
            switch (pm_port_cnt[i]) {
                case 1: /* single */
                    pm_modes[i] = TDM_VMAP_PM_MODE_1_0_0_0;
                    break;
                case 2: /* dual */
                    if (spd_0==spd_1) {
                        pm_modes[i] = TDM_VMAP_PM_MODE_1_1_0_0;
                    } else if (spd_0==2*spd_1) {
                        pm_modes[i] = TDM_VMAP_PM_MODE_2_1_0_0;
                    }
                    break;
                case 3: /* tri */
                    if (spd_0==spd_1 && spd_1==spd_2) {
                        pm_modes[i] = TDM_VMAP_PM_MODE_1_1_1_0;
                    } else if (spd_0==2*spd_1 && spd_1==spd_2) {
                        pm_modes[i] = TDM_VMAP_PM_MODE_2_1_1_0;
                    } else if (spd_0==4*spd_1 && spd_1==spd_2) {
                        pm_modes[i] = TDM_VMAP_PM_MODE_4_1_1_0;
                    }
                    break;
                case 4: /* quad */
		    if (spd_0==spd_1 && spd_1==spd_2 && spd_2==spd_3) {
                    /* if (spd_0!=0  && spd_1!=0 && spd_2!=0 && spd_3!=0 ) { */
                        pm_modes[i] = TDM_VMAP_PM_MODE_1_1_1_1;
                    }
                    break;
                default:
                    break;
            }
        }
        TDM_SML_BAR

        /* check special PM modes: special-dual and special-tri */
        TDM_PRINT0("TDM: Check special PM modes \n\n");
        pm_special_mode_en = BOOL_FALSE;
        for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) { 
            switch (pm_modes[i]) {
                case TDM_VMAP_PM_MODE_1_1_1_0:
                case TDM_VMAP_PM_MODE_2_1_0_0:
                case TDM_VMAP_PM_MODE_4_1_1_0:
                    pm_special_mode_en = BOOL_TRUE;
                    break;
                default:
                    break;
            }
            if (pm_special_mode_en==BOOL_TRUE) {
                break;
            }
        }
        if (pm_special_mode_en==BOOL_TRUE) {
            TDM_PRINT0("\t\t <special PM EXISTS>\n");
        } else {
            TDM_PRINT0("\t\t <special PM DOESNOT EXIST>\n");
        }
        TDM_SML_BAR

        /* check extra bandwidth applicability if special PM mode exists */
        pm_extra_bw_en = BOOL_FALSE;
        if (pm_special_mode_en==BOOL_TRUE) {
            TDM_PRINT0("TDM: Check extra bandwidth applicability\n\n");
            pm_extra_bw = 0;
            for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) {
                if (pm_modes[i]==TDM_VMAP_PM_MODE_1_1_1_0 ||
                    pm_modes[i]==TDM_VMAP_PM_MODE_2_1_0_0 ||
                    pm_modes[i]==TDM_VMAP_PM_MODE_4_1_1_0 ) {
                    prt_1 = pm_pmap[i][1];
                    spd_1 = (prt_1>=param_phy_lo && prt_1<=param_phy_hi) ?
                            (param_speeds[prt_1]) : (0);
                    prt_slot = tdm_hx5_vmap_get_speed_slots(spd_1);
                    switch (pm_modes[i]) {
                        case TDM_VMAP_PM_MODE_1_1_1_0:
                        case TDM_VMAP_PM_MODE_2_1_0_0:
                            pm_extra_bw += prt_slot;
                            break;
                        case TDM_VMAP_PM_MODE_4_1_1_0:
                            pm_extra_bw += 2*prt_slot;
                            break;
                        default:
                            break;
                    }
                }
            }
            if ((pm_extra_bw+lr_slot_num)<=param_lr_limit) {
                pm_extra_bw_en = BOOL_TRUE;
                TDM_PRINT0("\t\t Extra bandwidth is SUFFICIENT\n");
            } else {
                TDM_PRINT0("\t\t Extra bandwidth is NOT SUFFICIENT\n");
            }
            TDM_SML_BAR
        }
        /* adjust bandwidth for special mode PMs if applicable */
        if (pm_special_mode_en==BOOL_TRUE) {
            TDM_PRINT0("TDM: Adjust PM with special PM modes\n\n");
            /* Insert pseudo slots for special PM */
            if (pm_extra_bw_en==BOOL_TRUE) {
                for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) {
                    if (pm_modes[i]==TDM_VMAP_PM_MODE_1_1_1_0 ||
                        pm_modes[i]==TDM_VMAP_PM_MODE_2_1_0_0 ||
                        pm_modes[i]==TDM_VMAP_PM_MODE_4_1_1_0 ) {
                        prt_1 = pm_pmap[i][1];
                        spd_1 = (prt_1>=param_phy_lo && prt_1<=param_phy_hi) ?
                                (param_speeds[prt_1]) : (0);
                        prt_slot = tdm_hx5_vmap_get_speed_slots(spd_1);
                        if (pm_modes[i]==TDM_VMAP_PM_MODE_4_1_1_0) {
                            prt_slot *= 2;
                        }
                        pm_slot_req[i] += prt_slot;
                        lr_slot_num += prt_slot;
                        pm_pseudo_slot_sum += prt_slot;
                        TDM_PRINT7("\t%s %3d port [%d, %d, %d, %d] with %d slots\n",
                                   "-- Insert IDLE slot for PM",
                                   i,
                                   pm_pmap[i][0], pm_pmap[i][1],
                                   pm_pmap[i][2], pm_pmap[i][3],
                                   prt_slot);
                    }
                }
            /* Split any special PM into two PMs */
            } else {
                for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) {
                    if ((pm_modes[i]==TDM_VMAP_PM_MODE_1_1_1_0 ||
                         pm_modes[i]==TDM_VMAP_PM_MODE_2_1_0_0 ||
                         pm_modes[i]==TDM_VMAP_PM_MODE_4_1_1_0 ) &&
                        (pm_cnt<param_num_phy_pm)) {
                        prt_1 = pm_pmap[i][1];
                        prt_2 = pm_pmap[i][2];
                        spd_1 = (prt_1>=param_phy_lo && prt_1<=param_phy_hi) ?
                                (param_speeds[prt_1]) : (0);
                        spd_2 = (prt_2>=param_phy_lo && prt_2<=param_phy_hi) ?
                                (param_speeds[prt_2]) : (0);
                        prt_slot = tdm_hx5_vmap_get_speed_slots(spd_1);
                        if (pm_modes[i]==TDM_VMAP_PM_MODE_2_1_0_0) {
                            pm_pmap[pm_cnt][0]  = prt_1;
                            pm_port_cnt[pm_cnt] = 1;
                            pm_slot_req[pm_cnt] = prt_slot;
                            pm_modes[pm_cnt]     = TDM_VMAP_PM_MODE_1_0_0_0;
                            pm_cnt++;
                            pm_pmap[i][1]  = param_token_empty;
                            pm_port_cnt[i] = 1;
                            pm_slot_req[i]-= prt_slot;
                            pm_modes[i]     = TDM_VMAP_PM_MODE_1_0_0_0;
                            TDM_PRINT4("\t-- Split PM into <%d, %d>, port <[%d], [%d]>\n",
                                       i, pm_cnt-1,
                                       pm_pmap[i][0], pm_pmap[pm_cnt-1][0]);
                        } else if (pm_modes[i]==TDM_VMAP_PM_MODE_4_1_1_0) {
                            prt_slot *= 2;
                            pm_pmap[pm_cnt][0]  = prt_1;
                            pm_pmap[pm_cnt][1]  = prt_2;
                            pm_port_cnt[pm_cnt] = 2;
                            pm_slot_req[pm_cnt] = prt_slot;
                            pm_modes[pm_cnt]    = TDM_VMAP_PM_MODE_1_1_0_0;
                            pm_cnt++;
                            pm_pmap[i][1]  = param_token_empty;
                            pm_pmap[i][2]  = param_token_empty;
                            pm_port_cnt[i] = 1;
                            pm_slot_req[i]-= prt_slot;
                            pm_modes[i]    = TDM_VMAP_PM_MODE_1_0_0_0;
                            TDM_PRINT5("\t-- Split PM into <%d, %d>, port <[%d], [%d, %d]>\n",
                                       i, pm_cnt-1,
                                       pm_pmap[i][0],
                                       pm_pmap[pm_cnt-1][0],
                                       pm_pmap[pm_cnt-1][1]);
                        }
                    } else if (pm_modes[i]==TDM_VMAP_PM_MODE_1_1_1_0 ||
                               pm_modes[i]==TDM_VMAP_PM_MODE_2_1_0_0 ||
                               pm_modes[i]==TDM_VMAP_PM_MODE_4_1_1_0) {
                        TDM_WARN6("%s PM %d port [%d, %d, %d, %d]\n",
                                  "Insufficient bandwidth in PM splitting,",
                                   i,
                                   pm_pmap[i][0], pm_pmap[i][1],
                                   pm_pmap[i][2], pm_pmap[i][3]);
                    }
                }
            }
            TDM_PRINT1("\t-- Special PM summary: Insert %d pseudo slots\n",
                       pm_pseudo_slot_sum);
            TDM_SML_BAR
        }

        /* adjust pm_pmap for high-speed PM with speed < 100G (Bubble Sort) */
        TDM_PRINT0("TDM: Adjust PM order\n\n");
        act_pm_num = pm_cnt < param_num_phy_pm ? pm_cnt : param_num_phy_pm;
        for (n=0; n<act_pm_num; n++) {
            swap_exist = BOOL_FALSE;
            for (i=(act_pm_num-1); i>n; i--) {
                if (pm_slot_req[i]>pm_slot_req[i-1]) {
                    port_speed = tdm_hx5_vmap_get_port_speed(pm_pmap[i][0],
                                    param_speeds, param_phy_lo, param_phy_hi);
                    if (port_speed<SPEED_100G) {
                        prt_0           = pm_pmap[i][0];
                        prt_1           = pm_pmap[i][1];
                        prt_2           = pm_pmap[i][2];
                        prt_3           = pm_pmap[i][3];
                        pm_prt_cnt      = pm_port_cnt[i];
                        pm_slot_sum     = pm_slot_req[i];
                        pm_mode         = pm_modes[i];
                        
                        pm_pmap[i][0]   = pm_pmap[i-1][0];
                        pm_pmap[i][1]   = pm_pmap[i-1][1];
                        pm_pmap[i][2]   = pm_pmap[i-1][2];
                        pm_pmap[i][3]   = pm_pmap[i-1][3];
                        pm_port_cnt[i]  = pm_port_cnt[i-1];
                        pm_slot_req[i]  = pm_slot_req[i-1];
                        pm_modes[i]     = pm_modes[i-1];
                        
                        pm_pmap[i-1][0] = prt_0;
                        pm_pmap[i-1][1] = prt_1;
                        pm_pmap[i-1][2] = prt_2;
                        pm_pmap[i-1][3] = prt_3;
                        pm_port_cnt[i-1]= pm_prt_cnt;
                        pm_slot_req[i-1]= pm_slot_sum;
                        pm_modes[i-1]   = pm_mode;
                        
                        swap_exist = BOOL_TRUE;
                        TDM_PRINT4("TDM: Swap pm %d with pm %d, port [%d, ...] with port [%d, ...]\n",
                                  i, i-1, pm_pmap[i-1][0], pm_pmap[i][0]);
                    }
                }
            }
            if (swap_exist==BOOL_FALSE) {
                break;
            }
        }
        TDM_SML_BAR

        /* create pseudo PMs for EMPTY slots if oversub port exists */
        if (os_en==BOOL_TRUE) {
            lr_slot_num_e  = lr_slot_num;
            empty_slot_num = param_cal_len - lr_slot_num;
            empty_slot_left= param_cal_len - lr_slot_num - pm_pseudo_slot_sum;
            
            if (empty_slot_num>0) {
                lr_2_os_r    = 0;
                os_2_lr_r    = 0;
                if (empty_slot_num>lr_slot_num_e && lr_slot_num_e>0) {
                    /* threshold 5% */
                    os_2_lr_r = (empty_slot_num*105)/(lr_slot_num_e*100);
                } else if (empty_slot_num<=lr_slot_num_e && empty_slot_num>0) {
                    lr_2_os_r = lr_slot_num_e/empty_slot_num;
                }
                
                TDM_PRINT0("TDM: Insert pseudo empty PM for oversub slots\n\n");
                TDM_PRINT0("\t -- number of linerate/non-linerate (pseudo) slots\n");
                TDM_PRINT1("\t\t -- linerate slots    : %3d\n", lr_slot_num_e);
                TDM_PRINT1("\t\t -- non-linerate slots: %3d\n", empty_slot_left);
                
                if (empty_slot_num>lr_slot_num_e) {
                    TDM_PRINT0("\t -- Linerate type   : Minority\n");
                    TDM_PRINT1("\t -- empty_2_lr_ratio: %3d\n", os_2_lr_r);
                } else {
                    TDM_PRINT0("\t -- Linerate type   : Majority\n");
                    TDM_PRINT1("\t -- lr_2_empty_ratio: %3d\n", lr_2_os_r);
                }
                TDM_PRINT0("\n");
                /* bw_os > bw_lr */
                if (os_2_lr_r>0) {
                    for (i=0; i<(param_num_phy_pm-1); i++) {
                        pm_slot_sum = 0;
                        if (pm_pmap[i][0]==param_token_empty) {
                            break;
                        }
                        pm_slot_sum = pm_slot_req[i];
                        if (pm_slot_sum>0 && empty_slot_left>0) {
                            abs_r = lr_slot_num_e/pm_slot_sum;
                            /* abs_n = empty_slot_num/abs_r; */
                            abs_n = pm_slot_sum*os_2_lr_r;
                            abs_n = (abs_n>0)?(abs_n):(1);
                            slot_allc= (abs_n<=empty_slot_left)? 
                                       (abs_n): 
                                       (empty_slot_left);
                            pm_pseudo_num = tdm_math_div_ceil(slot_allc,
                                                pm_slot_sum);
                            for (n=0; n<pm_pseudo_num; n++) {
                                if (i<(param_num_phy_pm-1)) {
                                    empty_slot_allc = (pm_slot_sum<=slot_allc)?
                                                      (pm_slot_sum):
                                                      (slot_allc);
                                    if (empty_slot_allc<pm_slot_sum) {
                                        break;
                                    }
                                    if (n==0) {
                                        for (k=(param_num_phy_pm-1); k>i; k--) {
                                            for (j=0; j<param_num_pm_lanes; j++) {
                                                pm_pmap[k][j] = pm_pmap[k-1][j];
                                            }
                                            pm_port_cnt[k] = pm_port_cnt[k-1];
                                            pm_slot_req[k] = pm_slot_req[k-1];
                                            pm_modes[k]    = pm_modes[k-1];
                                        }
                                        slot_allc      -= empty_slot_allc;
                                        empty_slot_left-= empty_slot_allc;
                                        
                                        pm_pmap[i+1][0] = param_token_ovsb;
                                        pm_pmap[i+1][1] = param_token_empty;
                                        pm_pmap[i+1][2] = param_token_empty;
                                        pm_pmap[i+1][3] = param_token_empty;
                                        pm_port_cnt[i+1]= 1;
                                        pm_slot_req[i+1]= empty_slot_allc;
                                        pm_modes[i+1]   = TDM_VMAP_PM_MODE_0_0_0_0;
                                        
                                        pm_cnt++;
                                        i++;
                                    } else {
                                        slot_allc      -= empty_slot_allc;
                                        empty_slot_left-= empty_slot_allc;
                                        pm_port_cnt[i]++;
                                    }
                                    TDM_PRINT6("TDM: %s %2d with %2dx%2d slots (%s=%3d)\n",
                                            "Insert pseudo pm for EMPTY slot at index",
                                            i,
                                            pm_port_cnt[i],
                                            empty_slot_allc, 
                                            "empty_slot_left", 
                                            empty_slot_left);
                                } else {
                                    break;
                                }
                            }
                        }
                        if (empty_slot_left<=0) {
                            if (empty_slot_left<0) {
                                result = FAIL;
                                TDM_ERROR1("TDM: %d extra EMPTY slots allocated\n",
                                           (-empty_slot_left));
                            }
                            break;
                        }
                    }
                }
                /* bw_lr > bw_os */
                else if (lr_2_os_r>0) {
                    pm_slot_sum = 0;
                    for (i=0; i<(param_num_phy_pm-1); i++) {
                        pm_slot_sum_tmp = pm_slot_req[i];
                        pm_slot_sum += pm_slot_sum_tmp;
                        if (pm_slot_sum>0 && empty_slot_left>0) {
                            abs_r = lr_slot_num_e/pm_slot_sum;
                            abs_n = empty_slot_num/abs_r;
                            abs_n = (lr_2_os_r==1)?(pm_slot_sum):(abs_n);
                            abs_n = (abs_n<pm_slot_sum_tmp)?
                                    (abs_n):
                                    (pm_slot_sum_tmp);
                            if ((pm_slot_sum/lr_2_os_r>=pm_slot_sum_tmp) ||
                                (pm_slot_sum_tmp>0 &&
                                 pm_pmap[i][0]==param_token_ovsb) ) {
                                empty_slot_allc = (abs_n<=empty_slot_left)? 
                                                  (abs_n): 
                                                  (empty_slot_left);
                                if (empty_slot_allc<pm_slot_sum_tmp) {
                                    continue;
                                }
                                for (k=(param_num_phy_pm-1); k>i; k--) {
                                    for (j=0; j<param_num_pm_lanes; j++) {
                                        pm_pmap[k][j] = pm_pmap[k-1][j];
                                    }
                                    pm_port_cnt[k] = pm_port_cnt[k-1];
                                    pm_slot_req[k] = pm_slot_req[k-1];
                                    pm_modes[k]    = pm_modes[k-1];
                                }
                                empty_slot_left-= empty_slot_allc;
                                pm_pmap[i+1][0] = param_token_ovsb;
                                pm_pmap[i+1][1] = param_token_empty;
                                pm_pmap[i+1][2] = param_token_empty;
                                pm_pmap[i+1][3] = param_token_empty;
                                pm_port_cnt[i+1]= 1;
                                pm_slot_req[i+1]= empty_slot_allc;
                                pm_modes[i+1]   = TDM_VMAP_PM_MODE_0_0_0_0;
                                pm_cnt++;
                                i++;
                                pm_slot_sum = 0;
                                TDM_PRINT6("TDM: %s %2d with %2dx%2d EMPTY slots (%s=%3d)\n",
                                        "Insert pseudo pm at index",
                                        i,
                                        pm_port_cnt[i],
                                        empty_slot_allc, 
                                        "empty_slot_left", 
                                        empty_slot_left);
                            }
                        }
                        if (empty_slot_left<=0) {
                            if (empty_slot_left<0){
                                result = FAIL;
                                TDM_ERROR1("TDM: %d extra EMPTY slots allocated\n",
                                           (-empty_slot_left));
                            }
                            break;
                        }
                    }
                }
                TDM_SML_BAR
            }
        }

        /* allocate slots */
        TDM_PRINT0("TDM: Allocate PM \n\n");
        for (i=0; i<param_num_phy_pm && i<pm_cnt; i++) {
            port_token = pm_pmap[i][0];
            /* empty slot */
            if (port_token==param_token_ovsb) {
                slot_req = pm_slot_req[i];
                k        = pm_port_cnt[i];
                for (j=0; j<k; j++) {
                    if (slot_req>slot_left) {
                        break;
                    }
                    result_tmp = tdm_hx5_vmap_alloc_one_port(
                                    port_token, 
                                    slot_req, slot_left, 
                                    param_cal_len, vmap_idx,
                                    param_vmap_wid, param_vmap_len,
                                    vmap, r_a_arr);
                    TDM_PRINT4("TDM: Allocate pm %2d port %3d with %2d slots from %d\n",
                               i, port_token, slot_req, slot_left);
                    if (result_tmp==PASS) {
                        slot_left -= slot_req;
                        vmap_idx++;
                    } else {
                        /* result = FAIL; */
                        TDM_ERROR3("%s, slot_req %d, slot_avail %d\n",
                                  "FAILED in EMPTY slot vmap allocation",
                                  slot_req, slot_left);
                        break;
                    }
                }
            }
            /* linerate slot */
            else if (port_token!=param_token_empty && 
                     port_token!=param_token_ovsb) {
                slot_req = pm_slot_req[i];
                /* SPECIAL CASE: 4x106HG+3x40G uses a unique TDM in Tomahawk */
                if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G==BOOL_TRUE){
                    slot_req -= 1;
                }
                /* allocate slots for PM */
                result_tmp = tdm_hx5_vmap_alloc_one_port(
                                port_token, 
                                slot_req, slot_left, 
                                param_cal_len, vmap_idx,
                                param_vmap_wid, param_vmap_len,
                                vmap, r_a_arr);
                TDM_PRINT4("TDM: Allocate pm %2d port %3d with %2d slots from %d\n",
                           i, port_token, slot_req, slot_left);
                /* parse slots for PM subports */
                if (result_tmp!=PASS) {
                    result = FAIL;
                    TDM_ERROR4("%s, port %d, slot_req %d, slot_avail %d\n",
                              "FAILED in linerate slot vmap allocation",
                              port_token,
                              slot_req,
                              slot_left);
                } else {
                    switch (pm_modes[i]) {
                        case TDM_VMAP_PM_MODE_1_0_0_0:
                            break;
                        case TDM_VMAP_PM_MODE_1_1_0_0:
                            k = 0;
                            for (j=0; j<param_cal_len; j++) {
                                if(vmap[vmap_idx][j]==port_token) {
                                    switch(k){
                                        case 0:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%2);
                                }
                            }
                            break;
                        case TDM_VMAP_PM_MODE_1_1_1_0:
                            k = 0;
                            for (j=0; j<param_cal_len; j++) {
                                if(vmap[vmap_idx][j]==port_token) {
                                    switch(k){
                                        case 0:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        case 2:
                                            vmap[vmap_idx][j]=pm_pmap[i][2];
                                            break;
                                        case 3:
                                            vmap[vmap_idx][j]=param_token_empty;
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%4);
                                }
                            }
                            break;
                        case TDM_VMAP_PM_MODE_1_1_1_1:
                            k = 0;
                            for (j=0; j<param_cal_len; j++) {
                                if(vmap[vmap_idx][j]==port_token) {
                                    switch(k){
                                        case 0:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        case 2:
                                            vmap[vmap_idx][j]=pm_pmap[i][2];
                                            break;
                                        case 3:
                                            vmap[vmap_idx][j]=pm_pmap[i][3];
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%4);
                                }
                            }
                            break;
                        case TDM_VMAP_PM_MODE_2_1_0_0:
                            if (pm_extra_bw_en==BOOL_TRUE) {
                                switch (dice_2_1_0_0) {
                                    case 0:
                                        k = 0;
                                        for (j=0; j<param_cal_len; j++) {
                                            if(vmap[vmap_idx][j]==port_token) {
                                                switch(k){
                                                    case 0:
                                                    case 2:
                                                        vmap[vmap_idx][j]=pm_pmap[i][0];
                                                        break;
                                                    case 1:
                                                        vmap[vmap_idx][j]=pm_pmap[i][1];
                                                        break;
                                                    case 3:
                                                        vmap[vmap_idx][j]=param_token_empty;
                                                        break;
                                                    default:
                                                        break;
                                                }
                                                k = ((k+1)%4);
                                            }
                                        }
                                        break;
                                    case 1:
                                        k = 0;
                                        for (j=0; j<param_cal_len; j++) {
                                            if(vmap[vmap_idx][j]==port_token) {
                                                switch(k){
                                                    case 1:
                                                    case 3:
                                                        vmap[vmap_idx][j]=pm_pmap[i][0];
                                                        break;
                                                    case 2:
                                                        vmap[vmap_idx][j]=pm_pmap[i][1];
                                                        break;
                                                    case 0:
                                                        vmap[vmap_idx][j]=param_token_empty;
                                                        break;
                                                    default:
                                                        break;
                                                }
                                                k = ((k+1)%4);
                                            }
                                        }
                                        break;
                                    case 2:
                                        k = 0;
                                        for (j=0; j<param_cal_len; j++) {
                                            if(vmap[vmap_idx][j]==port_token) {
                                                switch(k){
                                                    case 0:
                                                    case 2:
                                                        vmap[vmap_idx][j]=pm_pmap[i][0];
                                                        break;
                                                    case 3:
                                                        vmap[vmap_idx][j]=pm_pmap[i][1];
                                                        break;
                                                    case 1:
                                                        vmap[vmap_idx][j]=param_token_empty;
                                                        break;
                                                    default:
                                                        break;
                                                }
                                                k = ((k+1)%4);
                                            }
                                        }
                                        break;
                                    case 3:
                                        k = 0;
                                        for (j=0; j<param_cal_len; j++) {
                                            if(vmap[vmap_idx][j]==port_token) {
                                                switch(k){
                                                    case 1:
                                                    case 3:
                                                        vmap[vmap_idx][j]=pm_pmap[i][0];
                                                        break;
                                                    case 0:
                                                        vmap[vmap_idx][j]=pm_pmap[i][1];
                                                        break;
                                                    case 2:
                                                        vmap[vmap_idx][j]=param_token_empty;
                                                        break;
                                                    default:
                                                        break;
                                                }
                                                k = ((k+1)%4);
                                            }
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                dice_2_1_0_0 = ((dice_2_1_0_0+1)%4);
                            } else {
                                k = 0;
                                for (j=0; j<param_cal_len; j++) {
                                    if(vmap[vmap_idx][j]==port_token) {
                                        switch(k){
                                            case 0:
                                            case 2:
                                                vmap[vmap_idx][j]=pm_pmap[i][0];
                                                break;
                                            case 1:
                                                vmap[vmap_idx][j]=pm_pmap[i][1];
                                                break;
                                            default:
                                                break;
                                        }
                                        k = ((k+1)%3);
                                    }
                                }
                            }
                            break;
                        case TDM_VMAP_PM_MODE_2_1_1_0:
                            k = 0;
                            for (j=0; j<param_cal_len; j++) {
                                if(vmap[vmap_idx][j]==port_token) {
                                    switch(k){
                                        case 0:
                                        case 2:
                                            vmap[vmap_idx][j]=pm_pmap[i][0];
                                            break;
                                        case 1:
                                            vmap[vmap_idx][j]=pm_pmap[i][1];
                                            break;
                                        case 3:
                                            vmap[vmap_idx][j]=pm_pmap[i][2];
                                            break;
                                        default:
                                            break;
                                    }
                                    k = ((k+1)%4);
                                }
                            }
                            break;
                        case TDM_VMAP_PM_MODE_4_1_1_0:
                            if (pm_extra_bw_en==BOOL_TRUE) {
                                switch (dice_4_1_1_0) {
                                    case 0:
                                        k = 0;
                                        for (j=0; j<param_cal_len; j++) {
                                            if(vmap[vmap_idx][j]==port_token) {
                                                switch(k){
                                                    case 0:
                                                    case 2:
                                                    case 4:
                                                    case 6:
                                                        vmap[vmap_idx][j]=pm_pmap[i][0];
                                                        break;
                                                    case 1:
                                                        vmap[vmap_idx][j]=pm_pmap[i][1];
                                                        break;
                                                    case 3:
                                                        vmap[vmap_idx][j]=pm_pmap[i][2];
                                                        break;
                                                    case 5:
                                                    case 7:
                                                        vmap[vmap_idx][j]=param_token_empty;
                                                        break;
                                                    default:
                                                        break;
                                                }
                                                k = ((k+1)%8);
                                            }
                                        }
                                        break;
                                    case 1:
                                        k = 0;
                                        for (j=0; j<param_cal_len; j++) {
                                            if(vmap[vmap_idx][j]==port_token) {
                                                switch(k){
                                                    case 0:
                                                    case 2:
                                                    case 4:
                                                    case 6:
                                                        vmap[vmap_idx][j]=pm_pmap[i][0];
                                                        break;
                                                    case 5:
                                                        vmap[vmap_idx][j]=pm_pmap[i][1];
                                                        break;
                                                    case 7:
                                                        vmap[vmap_idx][j]=pm_pmap[i][2];
                                                        break;
                                                    case 1:
                                                    case 3:
                                                        vmap[vmap_idx][j]=param_token_empty;
                                                        break;
                                                    default:
                                                        break;
                                                }
                                                k = ((k+1)%8);
                                            }
                                        }
                                        break;
                                    default:
                                        break;
                                }
                                dice_4_1_1_0 = ((dice_4_1_1_0+1)%2);
                            } else {
                                k = 0;
                                for (j=0; j<param_cal_len; j++) {
                                    if(vmap[vmap_idx][j]==port_token) {
                                        switch(k){
                                            case 0:
                                            case 1:
                                            case 3:
                                            case 4:
                                                vmap[vmap_idx][j]=pm_pmap[i][0];
                                                break;
                                            case 2:
                                                vmap[vmap_idx][j]=pm_pmap[i][1];
                                                break;
                                            case 5:
                                                vmap[vmap_idx][j]=pm_pmap[i][2];
                                                break;
                                            default:
                                                break;
                                        }
                                        k = ((k+1)%6);
                                    }
                                }
                            }
                            break;
                        default:
                            TDM_ERROR6("%s port [%d, %d, %d, %d], pm mode %d\n",
                                      "Invalid PM mode in vmap allocation,",
                                      pm_pmap[i][0], pm_pmap[i][1],
                                      pm_pmap[i][2], pm_pmap[i][3],
                                      pm_modes[i]);
                            break;
                    }
                    slot_left -= slot_req;
                    vmap_idx++;
                }
            }
        }
        TDM_SML_BAR
        /* check vmap sister port spacing */
        if (tdm_hx5_vmap_chk_sister_2D(_tdm, vmap)!=PASS) {
            tdm_hx5_vmap_rotate(_tdm, vmap);
        }
        /* check vmap singularity */
        if (tdm_hx5_vmap_chk_singularity(_tdm, vmap)!=PASS) {
            result = FAIL;
        }
    }
    tdm_hx5_vmap_print(_tdm, vmap);
    /* copy local vmap to core vmap */
    for (i=0; i<param_vmap_wid; i++) {
        for (j=0; j<param_vmap_len; j++) {
            if (vmap[i][j]==param_token_ovsb) {
                param_vmap[i][j] = param_token_empty;
            } else {
                param_vmap[i][j] = vmap[i][j];
            }
        }
    }
    /* free memory */
    TDM_FREE(lr_buff);
    TDM_FREE(pm_buff);
    TDM_FREE(pm_port_cnt);
    TDM_FREE(pm_slot_req);
    TDM_FREE(pm_modes);
    for (i=0; i<param_num_phy_pm; i++) {
        TDM_FREE(pm_pmap[i]);
    }
    TDM_FREE(pm_pmap);
    for (i=0; i<param_vmap_wid; i++) {
        TDM_FREE(r_a_arr[i]);
        TDM_FREE(vmap[i]);
    }
    TDM_FREE(r_a_arr);
    TDM_FREE(vmap);
    
    return (result);
}


/**
@name: tdm_hx5_core_vmap_alloc_mix
@param:
 */
int
tdm_hx5_core_vmap_alloc_mix(tdm_mod_t *_tdm)
{
    int i, j, port_token;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty,
        param_lr_en, 
        param_vmap_wid, param_vmap_len;
    int *param_cal_main;
    unsigned short **param_vmap;
    
    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_vmap        = _tdm->_core_data.vmap;
    
    /* allocate linerate slots */
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_lr_en==BOOL_TRUE){
        if(tdm_hx5_vmap_alloc_mix(_tdm)!=PASS){
            return (TDM_EXEC_CORE_SIZE+1);
        }
        for (j=0; j<param_vmap_len && j<param_cal_len; j++){
            port_token = param_token_empty;
            for (i=0; i<param_vmap_wid; i++){
                if (param_vmap[i][j]!=param_token_empty){
                    port_token = param_vmap[i][j];
                    break;
                }
            }
            param_cal_main[j] = port_token;
        }
    } else {
        for (j=0; j<param_vmap_len && j<param_cal_len; j++){
            param_cal_main[j] = param_token_empty;
        }
    }
    
    /* check/adjust sister port space */
    _tdm->_core_data.vars_pkg.refactor_done = BOOL_TRUE;
    if (tdm_hx5_vmap_chk_sister_1D(_tdm)!=PASS){
        if (tdm_hx5_vmap_filter_sister(_tdm)!=PASS) {
            _tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
        }
    }
    
    /* check/adjust same port space */
    if (tdm_hx5_vmap_chk_same(_tdm)!=PASS) {
        if (tdm_hx5_vmap_filter_same(_tdm)!=PASS) {
            _tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
        }
    }
    
    return PASS;
}


/**
@name: tdm_hx5_core_vmap_prealloc
@param:

Shim for TDM5
 */
int
tdm_hx5_core_vmap_prealloc(tdm_mod_t *_tdm)
{
    int i, port_phynum, result=PASS;
    int param_token_empty, param_phy_lo, param_phy_hi;
    int lr_cnt,
        lr_cnt_1g, lr_cnt_5g, lr_cnt_10g, lr_cnt_20g,  lr_cnt_25g, 
        lr_cnt_40g, lr_cnt_50g, lr_cnt_100g, lr_cnt_120g;
    char lr_en,
         lr_en_1g, lr_en_5g,  lr_en_10g, lr_en_20g,  lr_en_25g, 
         lr_en_40g, lr_en_50g, lr_en_100g, lr_en_120g;

    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    /* set parameters */
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    param_states = _tdm->_chip_data.soc_pkg.state;
    
    /* initialize variables */
    lr_cnt_1g   = 0;
    lr_cnt_5g   = 0;
    lr_cnt_10g  = 0;
    lr_cnt_20g  = 0;
    lr_cnt_25g  = 0;
    lr_cnt_40g  = 0;
    lr_cnt_50g  = 0;
    lr_cnt_100g = 0;
    lr_cnt_120g = 0;
    for (i=0; i<TDM_AUX_SIZE; i++) {
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[i]  = param_token_empty; 
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[i] = param_token_empty;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[i] = param_token_empty; 
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[i] = param_token_empty;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[i] = param_token_empty; 
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[i] = param_token_empty; 
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[i]= param_token_empty;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120[i]= param_token_empty;
    }
    
    /* shim for linerate ports */
    for (i=0; i<TDM_AUX_SIZE; i++) {
        port_phynum = _tdm->_core_data.vars_pkg.lr_buffer[i];
        if (port_phynum==param_token_empty){
            continue;
        }
        if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi){
            if ( param_states[port_phynum-1] == PORT_STATE__LINERATE    || 
                 param_states[port_phynum-1] == PORT_STATE__LINERATE_HG ){
                switch(param_speeds[port_phynum]){
                    case SPEED_1G:
                    case SPEED_2p5G:
                        lr_cnt_1g++;
                        break;
                    case SPEED_5G:
                         lr_cnt_5g++;
                        break;
                    case SPEED_10G:
                    case SPEED_10G_DUAL:
                    case SPEED_10G_XAUI:
                    case SPEED_11G:
                        lr_cnt_10g++;
                        break;
                    case SPEED_20G:
                    case SPEED_21G:
                    case SPEED_21G_DUAL:
                        lr_cnt_20g++;
                        break;
                    case SPEED_25G:
                    case SPEED_27G:
                        lr_cnt_25g++;
                        break;
                    case SPEED_40G:
                    case SPEED_42G:
                    case SPEED_42G_HG2:
                        lr_cnt_40g++;
                        break;
                    case SPEED_50G:
                    case SPEED_53G:
                        lr_cnt_50g++;
                        break;
                    case SPEED_100G:
                    case SPEED_106G:
                        lr_cnt_100g++;
                        break;
                    case SPEED_120G:
                    case SPEED_127G:
                        lr_cnt_120g++;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    lr_en_1g   = (lr_cnt_1g  >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_5g   = (lr_cnt_5g  >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_10g  = (lr_cnt_10g >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_20g  = (lr_cnt_20g >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_25g  = (lr_cnt_25g >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_40g  = (lr_cnt_40g >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_50g  = (lr_cnt_50g >0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_100g = (lr_cnt_100g>0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en_120g = (lr_cnt_120g>0)?(BOOL_TRUE):(BOOL_FALSE);
    lr_en      = (lr_en_1g || lr_en_5g  || lr_en_10g || lr_en_20g  || lr_en_25g || 
                  lr_en_40g || lr_en_50g || lr_en_100g || lr_en_120g);
    lr_cnt     = lr_cnt_1g  + lr_cnt_10g + lr_cnt_20g  + lr_cnt_25g + 
                 lr_cnt_40g + lr_cnt_50g + lr_cnt_100g + lr_cnt_120g;

    _tdm->_core_data.vars_pkg.lr_enable                   = lr_en;
    _tdm->_core_data.vars_pkg.lr_1                        = lr_en_1g;
   /* _tdm->_core_data.vars_pkg.lr_5                        = lr_en_5g;*/
    _tdm->_core_data.vars_pkg.lr_10                       = lr_en_10g;
    _tdm->_core_data.vars_pkg.lr_20                       = lr_en_20g;
    _tdm->_core_data.vars_pkg.lr_25                       = lr_en_25g;
    _tdm->_core_data.vars_pkg.lr_40                       = lr_en_40g;
    _tdm->_core_data.vars_pkg.lr_50                       = lr_en_50g;
    _tdm->_core_data.vars_pkg.lr_100                      = lr_en_100g;
    _tdm->_core_data.vars_pkg.lr_120                      = lr_en_120g;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr     = lr_cnt;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g   = lr_cnt_100g;
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g    = lr_cnt_40g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8 = lr_cnt_1g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 = lr_cnt_10g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 = lr_cnt_20g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 = lr_cnt_25g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 = lr_cnt_40g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 = lr_cnt_50g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 = lr_cnt_100g;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7 = lr_cnt_120g;

    /* shim for oversub ports */
    for (i=0; i<TDM_AUX_SIZE; i++) {
        /* Pre-allocate oversub ports */
        port_phynum = _tdm->_core_data.vars_pkg.os_buffer[i];
        if (port_phynum==param_token_empty){
            continue;
        }
        if (port_phynum>=param_phy_lo && port_phynum<=param_phy_hi){
            if ( param_states[port_phynum-1] == PORT_STATE__OVERSUB    || 
                 param_states[port_phynum-1] == PORT_STATE__OVERSUB_HG ){
                switch (param_speeds[port_phynum]) {
                    case SPEED_1G:
                    case SPEED_2p5G:
                        tdm_core_prealloc(
                            _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8), &(_tdm->_core_data.vars_pkg.os_1), i);
                        break;
                    case SPEED_10G:
                    case SPEED_10G_DUAL:
                    case SPEED_10G_XAUI:
                    case SPEED_11G:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1), &(_tdm->_core_data.vars_pkg.os_10), i);
                        break;
                    case SPEED_20G:
                    case SPEED_21G:
                    case SPEED_21G_DUAL:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2), &(_tdm->_core_data.vars_pkg.os_20), i);
                        break;
                    case SPEED_40G:
                    case SPEED_42G:
                    case SPEED_42G_HG2:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3), &(_tdm->_core_data.vars_pkg.os_40), i);
                        break;
                    case SPEED_100G:
                    case SPEED_106G:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4), &(_tdm->_core_data.vars_pkg.os_100), i);
                        break;
                    case SPEED_50G:
                    case SPEED_53G:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5), &(_tdm->_core_data.vars_pkg.os_50), i);
                        break;
                    case SPEED_25G:
                    case SPEED_27G:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6), &(_tdm->_core_data.vars_pkg.os_25), i);
                        break;
                    case SPEED_120G:
                    case SPEED_127G:
                        tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7), &(_tdm->_core_data.vars_pkg.os_120), i);
                        break;					
                    default:
                        result = FAIL;
                        TDM_ERROR3("%s, port %0d speed %0d(Gb)\n",
                                   "Skipped port in oversub preallocation",
                                    port_phynum, param_speeds[port_phynum]/1000);
                }
            }
            else{
                result = FAIL;
                TDM_ERROR3("%s, port %0d state %0d\n", 
                           "Unrecognized port state in oversub preallocation",
                           port_phynum, param_states[port_phynum-1]);
            }
        }
        else{
            result = FAIL;
            TDM_ERROR1("Unrecognized oversub port %0d in oversub preallocation\n", port_phynum);
        } 
    }
    _tdm->_core_data.vars_pkg.os_enable = (_tdm->_core_data.vars_pkg.os_1 ||
                                           _tdm->_core_data.vars_pkg.os_10||
                                           _tdm->_core_data.vars_pkg.os_20||
                                           _tdm->_core_data.vars_pkg.os_25||
                                           _tdm->_core_data.vars_pkg.os_40||
                                           _tdm->_core_data.vars_pkg.os_50||
                                           _tdm->_core_data.vars_pkg.os_100||
                                           _tdm->_core_data.vars_pkg.os_120);
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os = (
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6+
                    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7);
    
    return result;
}
