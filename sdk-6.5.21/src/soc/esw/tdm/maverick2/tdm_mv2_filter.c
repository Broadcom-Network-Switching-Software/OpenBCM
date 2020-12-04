/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip based calendar post-processing filters
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_mv2_filter_chk_slot_shift_cond
@param:
 */
int
tdm_mv2_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm)
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
        port_pm = tdm_mv2_cmn_get_port_pm(port, _tdm);
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
                    if (tdm_mv2_cmn_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }

        /* check dst port spacing */
        idx     = dst;
        port    = cal_main[idx];
        port_pm = tdm_mv2_cmn_get_port_pm(port, _tdm);
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
                    if (tdm_mv2_cmn_get_port_pm(port_k, _tdm) == port_pm) {
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
@name: tdm_mv2_filter_lr_jitter
@param:
 */
int
tdm_mv2_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed)
{
    int i, j, k, filter_cnt=0, x, y, chk_x, chk_y;
    int m, port, port_speed, idx_curr,
        space_min, space_max,
        dist_up, dist_dn, dist_mv;
    int *port_bmp;
    int param_lr_limit, param_ancl_num, param_cal_len, param_ext_phy_num,
        param_lr_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_ext_phy_num = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id, param_cal_main);

    /* allocate memory */
    port_bmp = (int*) TDM_ALLOC(param_ext_phy_num*sizeof(int), 
                                "mv2_filter_lr_jitter, port_bmp");
    for (i=0; i<param_ext_phy_num; i++) {
        port_bmp[i] = 0;
    }
    
    TDM_PRINT0("Smooth LR port jitter\n\n");
    if (param_lr_en==BOOL_TRUE && min_speed>=SPEED_10G) {
        for (i=0; i<param_cal_len; i++) {
            port = param_cal_main[i];
            port_speed = tdm_mv2_cmn_get_port_speed(port, _tdm);
            if (port_speed == SPEED_0 || port_speed < min_speed) {
                continue;
            }
            if (port < param_ext_phy_num) {
                if (port_bmp[port] == 1) {
                    continue;
                }
                port_bmp[port] = 1;
                tdm_mv2_cmn_get_speed_jitter(port_speed, param_cal_len,
                                             &space_min, &space_max);
                for (j=0; j<param_cal_len; j++) {
                    k = (i+param_cal_len-j)%param_cal_len;
                    if (param_cal_main[k] == port) {
                        idx_curr = k;
                        dist_up = tdm_mv2_cmn_get_same_port_dist(idx_curr,
                                    TDM_DIR_UP, param_cal_main, param_cal_len);
                        dist_dn = tdm_mv2_cmn_get_same_port_dist(idx_curr,
                                    TDM_DIR_DN, param_cal_main, param_cal_len);
                        /* filter port if space violation is detected: up */
                        if (dist_up > space_max || dist_dn < space_min) {
                            TDM_PRINT7("%s, port %d, slot %d, dist_up %d > %d, dist_dn %d < %d\n",
                                      "TDM: [Linerate jitter 1]",
                                      port, idx_curr,
                                      dist_up, space_max, dist_dn, space_min);
                            dist_mv = (dist_up-dist_dn)/2;
                            for (m=1; m<=dist_mv; m++) {
                                /* check spacing for neighbor and target port */
                                x = (idx_curr + param_cal_len - m) % param_cal_len;
                                y = (x + 1) % param_cal_len;
                                chk_x = tdm_mv2_filter_chk_slot_shift_cond(x, TDM_DIR_DN,
                                            param_cal_len, param_cal_main, _tdm);
                                chk_y = tdm_mv2_filter_chk_slot_shift_cond(y, TDM_DIR_UP,
                                            param_cal_len, param_cal_main, _tdm);
                                /* shift port */
                                if (chk_x == PASS && chk_y == PASS) {
                                    param_cal_main[y] = param_cal_main[x];
                                    param_cal_main[x] = port;
                                    filter_cnt++;
                                    TDM_PRINT3("\t Shift port %d UP from slot %d to slot %d\n",
                                              port, y, x);
                                } else {
                                    break;
                                }
                            }
                        }
                        /* filter port if space violation is detected: down */
                        else if (dist_up < space_min || dist_dn > space_max) {
                            TDM_PRINT7("%s port %d, slot %d, dist_up %d < %d, dist_dn %d > %d\n",
                                      "TDM: [Linerate jitter 2]",
                                      port, idx_curr,
                                      dist_up, space_min, dist_dn, space_max);
                            dist_mv = (dist_dn-dist_up)/2;
                            for (m=1; m<=dist_mv; m++) {
                                /* check spacing for neighbor and target port */
                                x = (idx_curr + m) % param_cal_len;
                                y = (x + param_cal_len - 1) % param_cal_len;
                                chk_x = tdm_mv2_filter_chk_slot_shift_cond(x, TDM_DIR_UP,
                                            param_cal_len, param_cal_main, _tdm);
                                chk_y = tdm_mv2_filter_chk_slot_shift_cond(y, TDM_DIR_DN,
                                            param_cal_len, param_cal_main, _tdm);
                                /* shift port */
                                if (chk_x == PASS && chk_y == PASS) {
                                    param_cal_main[y] = param_cal_main[x];
                                    param_cal_main[x] = port;
                                    filter_cnt++;
                                    TDM_PRINT3("\t Shift port %d DOWN from slot %d to slot %d\n",
                                              port, y, x);
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    TDM_SML_BAR
    
    TDM_FREE(port_bmp);
    return (filter_cnt);
}

/**
@name: tdm_mv2_filter_lr
@param:
 */
int
tdm_mv2_filter_lr(tdm_mod_t *_tdm)
{
    if (tdm_mv2_filter_lr_jitter(_tdm, SPEED_10G) == 0){
        return PASS;
    }
    if (tdm_mv2_filter_lr_jitter(_tdm, SPEED_20G) == 0) {
        return PASS;
    }
    if (tdm_mv2_filter_lr_jitter(_tdm, SPEED_40G) == 0) {
        return PASS;
    }
    return PASS;
}


/**
@name: tdm_mv2_filter
@param:
 */
int
tdm_mv2_filter( tdm_mod_t *_tdm )
{
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Filters Applied to smooth TDM calendar\n\n");
    TDM_SML_BAR
    
    /* Smooth Line-rate port jitter */
    tdm_mv2_filter_lr(_tdm);

    return (_tdm->_chip_exec[TDM_CHIP_EXEC__PARSE](_tdm));
}
