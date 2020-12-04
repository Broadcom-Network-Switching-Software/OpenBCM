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


/**
@name: tdm_vmap_print_vmap
@param:
 */
void
tdm_vmap_print_vmap(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, vmap_wid, vmap_len, slot_cnt;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    vmap_wid = tdm_vmap_get_vmap_wid(_tdm, vmap);
    vmap_len = tdm_vmap_get_vmap_len(_tdm, vmap);
    /* print vmap */
    TDM_PRINT0("\t TDM vmap \n");
    TDM_PRINT1("%6s\t", " ");
    TDM_PRINT1("%6s\t", " ");
    for (i = 0; i < vmap_wid; i++) TDM_PRINT1("%3d\t", i);
    TDM_PRINT0("\n");
    for (j = 0; j < vmap_len; j++) {
        slot_cnt = 0;
        for (i = 0; i < vmap_wid; i++) {
            if (vmap[i][j] != param_token_empty) slot_cnt++;
        }
        if (slot_cnt > 1) {
            TDM_PRINT1("%6s\t", "[FAIL]");
        } else {
            TDM_PRINT1("%6s\t", " ");
        }

        TDM_PRINT1("%3d : \t", j);
        for (i = 0; i < vmap_wid; i++) {
            if (vmap[i][j] != param_token_empty) {
                TDM_PRINT1("%3d\t", vmap[i][j]);
            } else {
                TDM_PRINT0("---\t");
            }
        }
        TDM_PRINT0("\n");
    }

    /* print overall allocated slots */
    slot_cnt = 0;
    for (j = 0; j < vmap_len; j++) {
        for (i = 0; i < vmap_wid; i++) {
            if (vmap[i][j] != param_token_empty) {
                slot_cnt++;
            } 
        }
    }
    TDM_PRINT2("\nTotal number of slots allocated in Vmap: %3d over %3d\n",
               slot_cnt, vmap_len);
    TDM_SML_BAR
}

/**
@name: tdm_vmap_print_pmlist
@param:
 */
void
tdm_vmap_print_pmlist(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist, int pmlist_size)
{
    int i, j;
    if (_tdm != NULL && pmlist != NULL) {
        for (i = 0; i < pmlist_size; i++) {
            if (pmlist[i].pm_en == 0) break;
            TDM_PRINT3("TDM: PM[%2d] %2d, port_cnt %d, port [",
                       i, pmlist[i].pm_num, pmlist[i].subport_cnt);
            for (j = 0; j < pmlist[i].subport_cnt; j++) {
                TDM_PRINT1("%3d",  pmlist[i].subport_phy[j]);
                if (j != pmlist[i].subport_cnt - 1) TDM_PRINT0(", ");
            }
            TDM_PRINT0("], speed [");
            for (j = 0; j < pmlist[i].subport_cnt; j++) {
                TDM_PRINT1("%3d", (tdm_cmn_get_port_speed(_tdm,
                           pmlist[i].subport_phy[j]) / 1000));
                if (j != pmlist[i].subport_cnt - 1) TDM_PRINT0(", ");
            }
            TDM_PRINT0("]\n");
        }
        TDM_SML_BAR
    }
}


/**
@name: tdm_core_vmap_alloc_mix
@param:
 */
int
tdm_core_vmap_alloc_mix(tdm_mod_t *_tdm)
{
    return (tdm_core_vmap_alloc(_tdm));
}


/**
@name: tdm_vmap_chk_pipe_bandwidth
@param:
 */
int
tdm_vmap_chk_pipe_bandwidth(tdm_mod_t *_tdm, int *lr_buff, int lr_buff_size)
{
    int i, slot_num, lr_slot_num = 0, os_slot_num = 0, result = PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_lr_en, param_os_en, param_num_lr, param_num_os;

    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_num_lr      = _tdm->_core_data.vars_pkg.pipe_info.num_lr_prt;
    param_num_os      = _tdm->_core_data.vars_pkg.pipe_info.num_os_prt;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;

    /* calculate total number of slots required by linerate ports */
    if (param_lr_en == BOOL_TRUE) {
        for (i = 0; i < lr_buff_size; i++) {
            slot_num = tdm_cmn_get_port_slots(_tdm, lr_buff[i]);
            if (slot_num > 0) {
                lr_slot_num += slot_num;
            } else {
                break;
            }
        }
    }

    /* calculate total number of slots available for oversub ports */
    if (param_os_en == BOOL_TRUE) {
        os_slot_num = param_lr_limit - lr_slot_num;
    }

    /* print pipe bandwidth info */
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Populating VMAP ... \n\n");
    TDM_PRINT1("\t [SLOT_UNIT: %0dMbps]\n", tdm_cmn_get_slot_unit(_tdm));
    TDM_PRINT1("\t -- number of lineRate ports: %3d\n", param_num_lr);
    TDM_PRINT1("\t -- number of OverSub  ports: %3d\n\n", param_num_os);
    TDM_PRINT1("\t -- cal_len : %3d\n", param_cal_len);
    TDM_PRINT1("\t\t -- ancl_limit : %3d\n", param_ancl_num);
    TDM_PRINT1("\t\t -- lr_os_limit: %3d\n", param_lr_limit);
    TDM_PRINT1("\t\t\t -- number of linerate slots: %3d\n", lr_slot_num);
    TDM_PRINT1("\t\t\t -- number of oversub  slots: %3d\n", os_slot_num);
    TDM_SML_BAR

    /* check io bandwidth against core bandwidth */
    if (lr_slot_num > param_lr_limit) {
        /* result = FAIL; */
        TDM_WARN3("TDM: %s, [lr_slot_req %d, lr_slot_limit %d]\n",
                  "Linerate bandwidth overflow in vmap allocation",
                  lr_slot_num, param_lr_limit);
    }

    return result;
}


/**
@name: tdm_vmap_chk_singularity
@param:
 */
int
tdm_vmap_chk_singularity(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, vmap_wid, vmap_len, slot_cnt, result = PASS;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    vmap_len = tdm_vmap_get_vmap_len(_tdm, vmap);
    vmap_wid = tdm_vmap_get_vmap_wid(_tdm, vmap);

    TDM_PRINT0("TDM: Check VMAP singularity\n\n");
    for (j = 0; j < vmap_len; j++) {
        slot_cnt = 0;
        for (i = 0; i < vmap_wid; i++) {
            if (vmap[i][j] != param_token_empty) slot_cnt++;
        }
        if (slot_cnt > 1) {
            TDM_ERROR1("FAILED in vmap singularity check, row %03d\n", j);
            result = FAIL;
        }
    }
    TDM_SML_BAR

    return result;
}


/**
@name: tdm_vmap_chk_port_space
@param:
 */
int
tdm_vmap_chk_port_space(tdm_mod_t *_tdm, int *cal, int cal_len, int espace,
                        int idx, int dir)
{
    int d1, d2, result = PASS;

    if (_tdm != NULL && cal != NULL && idx >= 0 && idx < cal_len) {
        /* check src port spacing */
        if (tdm_cmn_chk_port_is_fp(_tdm, cal[idx])) {
            d1 = tdm_vmap_calc_port_dist_same(_tdm, cal, cal_len, espace,
                                              idx, dir);
            d2 = tdm_vmap_calc_port_dist_sister(_tdm, cal, cal_len, espace,
                                                idx, dir);
            if (d1 != 0 || d2 != 0) result = FAIL;
        }

        /* check dst port spacing */
        idx = (dir == TDM_DIR_DN) ? ((idx + 1) % cal_len) :
                                    ((cal_len + idx - 1) % cal_len);
        if (tdm_cmn_chk_port_is_fp(_tdm, cal[idx])) {
            d1 = tdm_vmap_calc_port_dist_same(_tdm, cal, cal_len, espace,
                                              idx, dir);
            d2 = tdm_vmap_calc_port_dist_sister(_tdm, cal, cal_len, espace,
                                                idx, dir);
            if (d1 != 0 || d2 != 0) result = FAIL;
        }
    }

    return result;
}


/**
@name: tdm_vmap_chk_sister
@param:
 */
int
tdm_vmap_chk_sister(tdm_mod_t *_tdm)
{
    int i, j, dist, result = PASS;
    int param_space_sister, param_lr_en, param_cal_len;
    int *param_cal_main;

    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id, param_cal_main);

    TDM_PRINT1("TDM: Check VMAP sister port spacing (1-D): limit %d\n\n",
               param_space_sister);
    if (param_lr_en == BOOL_TRUE && param_space_sister > 0) {
        for (i = 0; i < param_cal_len; i++) {
            if (tdm_cmn_chk_port_is_fp(_tdm, param_cal_main[i])) {
                dist = tdm_vmap_calc_port_dist_sister(_tdm, param_cal_main,
                            param_cal_len, 0, i, TDM_DIR_DN);
                if (dist != 0) {
                    result = FAIL;
                    j = (i + dist) % param_cal_len;
                    TDM_PRINT5("%s slot [%03d|%03d], port [%3d|%3d]\n",
                               "[sister-port-space violation]",
                               i, j, param_cal_main[i], param_cal_main[j]);
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}


/**
@name: tdm_vmap_chk_same
@param:
 */
int
tdm_vmap_chk_same(tdm_mod_t *_tdm)
{
    int i, dist, result = PASS;
    int param_space_same, param_lr_en, param_cal_len;
    int *param_cal_main;

    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit + 
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_space_same  = _tdm->_core_data.rule__same_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    TDM_PRINT1("TDM: Check VMAP same port spacing (1-D): limit %0d\n\n",
               param_space_same);
    if (param_lr_en == BOOL_TRUE && param_space_same > 0) {
        for (i = 0; i < param_cal_len; i++) {
            if (tdm_cmn_chk_port_is_fp(_tdm, param_cal_main[i])) {
                dist = tdm_vmap_calc_port_dist_same(_tdm, param_cal_main,
                            param_cal_len, 0, i, TDM_DIR_DN);
                if (dist != 0) {
                    result = FAIL;
                    TDM_PRINT4("%s, slot [%03d|%03d], port %3d\n",
                               "[same-port-space violation]",
                               i, ((i + dist) % param_cal_len),
                               param_cal_main[i]);
                }
            }
        }
    }
    TDM_SML_BAR

    return result;
}


/**
@name: tdm_vmap_chk_vmap_sister_xy
@param:
 */
int
tdm_vmap_chk_vmap_sister_xy(tdm_mod_t *_tdm, unsigned short **vmap, int col, int row)
{
    int i, j, k, port_token, port_pm, pm_tmp, min_sister_space, result = PASS;
    int vmap_wid, vmap_len;
    int param_vmap_wid, param_vmap_len;

    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;

    if ( !(col >= 0 && col < param_vmap_wid &&
           row >= 0 && row < param_vmap_len)) {
        result = FAIL;
    } else {
        port_token = vmap[col][row];
        if (tdm_cmn_chk_port_is_fp(_tdm, port_token)) {
            vmap_len = tdm_vmap_get_vmap_len(_tdm, vmap);
            vmap_wid = tdm_vmap_get_vmap_wid(_tdm, vmap);
            port_pm  = tdm_cmn_get_port_pm(_tdm, port_token);
            min_sister_space = tdm_vmap_get_port_space_sister(_tdm, port_token);
            for (i = 0; i < vmap_wid; i++) {
                for (k = 1; k < min_sister_space; k++) {
                    /* TDM_DIR_DN */
                    j = (row + k) % vmap_len;
                    if (tdm_cmn_chk_port_is_fp(_tdm, vmap[i][j])) {
                        pm_tmp = tdm_cmn_get_port_pm(_tdm, vmap[i][j]);
                        if (pm_tmp == port_pm) {
                            result = FAIL;
                            break;
                        }
                    }
                    /* TDM_DIR_UP */
                    j = (vmap_len + row - k) % vmap_len;
                    if (tdm_cmn_chk_port_is_fp(_tdm, vmap[i][j])) {
                        pm_tmp = tdm_cmn_get_port_pm(_tdm, vmap[i][j]);
                        if (pm_tmp == port_pm) {
                            result = FAIL;
                            break;
                        }
                    }
                }
                if (result == FAIL) break;
            }
        }
    }

    return result;
}


/**
@name: tdm_vmap_chk_vmap_sister_col
@param:
 */
int
tdm_vmap_chk_vmap_sister_col(tdm_mod_t *_tdm, unsigned short **vmap, int col)
{
    int row, vmap_len, vmap_wid, result = PASS;
    int param_vmap_wid;

    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;

    if ( !(col >= 0 && col < param_vmap_wid)) {
        result = FAIL;
    } else {
        vmap_len = tdm_vmap_get_vmap_len(_tdm, vmap);
        vmap_wid = tdm_vmap_get_vmap_wid(_tdm, vmap);
        if (col < vmap_wid) {
            for (row = 0; row < vmap_len; row++) {
                if (tdm_cmn_chk_port_is_fp(_tdm, vmap[col][row])) {
                    if (PASS != tdm_vmap_chk_vmap_sister_xy(_tdm, vmap,
                                    col, row)) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }
    }

    return result;
}


/**
@name: tdm_vmap_chk_vmap_sister
@param:
 */
int
tdm_vmap_chk_vmap_sister(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, vmap_wid, result = PASS;
    int param_space_sister;

    param_space_sister= _tdm->_core_data.rule__prox_port_min;

    TDM_PRINT0("TDM: Check VMAP sister port spacing (2-D)\n\n");
    if (param_space_sister > 0) {
        vmap_wid = tdm_vmap_get_vmap_wid(_tdm, vmap);
        for (i = 0; i < vmap_wid; i++) {
            if (tdm_vmap_chk_vmap_sister_col(_tdm, vmap, i) != PASS) {
                result = FAIL;
                break;
            }
        }
    }
    TDM_SML_BAR

    return result;
}


/**
@name: tdm_vmap_chk_lr_ports
@param:
 */
int
tdm_vmap_chk_lr_ports(tdm_mod_t *_tdm, int *lr_buff, int lr_buff_size)
{
    int result = PASS;
    int i, port_phynum, port_speed, port_state;
    int param_token_empty;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_chip_data.soc_pkg.state;

    if (lr_buff != NULL && lr_buff_size > 0) {
        for (i = 0; i < lr_buff_size; i++) {
            port_phynum = lr_buff[i];
            if (port_phynum == param_token_empty) break;
            if (tdm_cmn_chk_port_is_fp(_tdm, port_phynum)) {
                port_state = param_states[port_phynum-1];
                port_speed = param_speeds[port_phynum];
                if (port_state==PORT_STATE__LINERATE    ||
                    port_state==PORT_STATE__LINERATE_HG ) {
                    if (!(port_speed > 0)) {
                        result = FAIL;
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
    }

    return result;
}


/**
@name: tdm_vmap_op_pm_reset
@param:
 */
void
tdm_vmap_op_pm_reset(tdm_mod_t *_tdm, tdm_vmap_pm_t *pm)
{
    int j;
    int param_token_empty;

    if (_tdm != NULL && pm != NULL) {
        param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

        (*pm).pm_en       = 0;
        (*pm).pm_num      = 0;
        (*pm).pm_slots    = 0;
        (*pm).pm_mode     = 0;
        (*pm).subport_cnt = 0;
        for (j = 0; j < TDM_VMAP_PM_LANES; j++) {
            (*pm).subport_phy[j]      = param_token_empty;
            (*pm).subport_slot_req[j] = 0;
            (*pm).subport_slot_rsv[j] = 0;
        }
    }
}


/**
@name: tdm_vmap_op_pm_update
@param:
 */
void
tdm_vmap_op_pm_update(tdm_mod_t *_tdm, tdm_vmap_pm_t *pm)
{
    int j, subport_cnt;

    if (_tdm != NULL && pm != NULL) {
        if ((*pm).pm_en) {
            /* subport_slot_rsv */
            subport_cnt = (*pm).subport_cnt;
            for (j = 0; j < TDM_VMAP_PM_LANES && j < subport_cnt; j++) {
                if ((*pm).subport_slot_rsv[j] < (*pm).subport_slot_req[j]) {
                    (*pm).subport_slot_rsv[j] = (*pm).subport_slot_req[j];
                }
            }
            /* pm_num */
            (*pm).pm_num = tdm_cmn_get_port_pm(_tdm, (*pm).subport_phy[0]);
            /* pm_slots */
            (*pm).pm_slots = 0;
            for (j = 0; j < TDM_VMAP_PM_LANES && j < subport_cnt; j++) {
                (*pm).pm_slots += (*pm).subport_slot_rsv[j];
            }
        } else {
            tdm_vmap_op_pm_reset(_tdm, pm);
        }
    }
}


/**
@name: tdm_vmap_op_pm_swap
@param:
 */
void
tdm_vmap_op_pm_swap(tdm_vmap_pm_t *pmlist, int pmlist_size,
                    int src_idx, int dst_idx)
{
    int i, j, n;
    tdm_vmap_pm_t tmp;

    i = src_idx;
    j = dst_idx;
    if (pmlist != NULL && i < pmlist_size && j < pmlist_size && i != j) {
        /* tmp pm */
        tmp.pm_en       = pmlist[j].pm_en;
        tmp.pm_num      = pmlist[j].pm_num;
        tmp.pm_slots    = pmlist[j].pm_slots;
        tmp.pm_mode     = pmlist[j].pm_mode;
        tmp.subport_cnt = pmlist[j].subport_cnt;
        for (n = 0; n < TDM_VMAP_PM_LANES; n++) {
            tmp.subport_phy[n]      = pmlist[j].subport_phy[n];
            tmp.subport_slot_req[n] = pmlist[j].subport_slot_req[n];
            tmp.subport_slot_rsv[n] = pmlist[j].subport_slot_rsv[n];
        }
        /* dst pm */
        pmlist[j].pm_en       = pmlist[i].pm_en;
        pmlist[j].pm_num      = pmlist[i].pm_num;
        pmlist[j].pm_slots    = pmlist[i].pm_slots;
        pmlist[j].pm_mode     = pmlist[i].pm_mode;
        pmlist[j].subport_cnt = pmlist[i].subport_cnt;
        for (n = 0; n < TDM_VMAP_PM_LANES; n++) {
            pmlist[j].subport_phy[n]      = pmlist[i].subport_phy[n];
            pmlist[j].subport_slot_req[n] = pmlist[i].subport_slot_req[n];
            pmlist[j].subport_slot_rsv[n] = pmlist[i].subport_slot_rsv[n];
        }
        /* src pm */
        pmlist[i].pm_en       = tmp.pm_en;
        pmlist[i].pm_num      = tmp.pm_num;
        pmlist[i].pm_slots    = tmp.pm_slots;
        pmlist[i].pm_mode     = tmp.pm_mode;
        pmlist[i].subport_cnt = tmp.subport_cnt;
        for (n = 0; n < TDM_VMAP_PM_LANES; n++) {
            pmlist[i].subport_phy[n]      = tmp.subport_phy[n];
            pmlist[i].subport_slot_req[n] = tmp.subport_slot_req[n];
            pmlist[i].subport_slot_rsv[n] = tmp.subport_slot_rsv[n];
        }
    }
}


/**
@name: tdm_vmap_op_pm_migrate
@param:
 */
void
tdm_vmap_op_pm_migrate(tdm_vmap_pm_t *pmlist, int pmlist_size,
                       int src_idx, int dst_idx)
{
    int i, j, n;

    i = src_idx;
    j = dst_idx;
    if (pmlist != NULL && i < pmlist_size && j < pmlist_size && i != j) {
        pmlist[j].pm_en       = pmlist[i].pm_en;
        pmlist[j].pm_num      = pmlist[i].pm_num;
        pmlist[j].pm_slots    = pmlist[i].pm_slots;
        pmlist[j].pm_mode     = pmlist[i].pm_mode;
        pmlist[j].subport_cnt = pmlist[i].subport_cnt;
        for (n = 0; n < TDM_VMAP_PM_LANES; n++) {
            pmlist[j].subport_phy[n]      = pmlist[i].subport_phy[n];
            pmlist[j].subport_slot_req[n] = pmlist[i].subport_slot_req[n];
            pmlist[j].subport_slot_rsv[n] = pmlist[i].subport_slot_rsv[n];
        }
    }
}


/**
@name: tdm_vmap_op_pm_addPort
@param:
 */
int
tdm_vmap_op_pm_addPort(tdm_mod_t *_tdm, tdm_vmap_pm_t *pm,
                       int port_token, int port_slots)
{
    int result = PASS;
    int subport_idx;

    if (_tdm != NULL && pm != NULL) {
        subport_idx = (*pm).subport_cnt;
        if (subport_idx < TDM_VMAP_PM_LANES) {
            (*pm).pm_en = 1;
            (*pm).subport_cnt++;
            (*pm).subport_phy[subport_idx]      = port_token;
            (*pm).subport_slot_req[subport_idx] = port_slots;
            (*pm).subport_slot_rsv[subport_idx] = port_slots;
            tdm_vmap_op_pm_update(_tdm, pm);
        } else {
            result = FAIL;
            TDM_ERROR5("%s, pm %0d, port %0d, cnt %d, size %0d\n",
                "Number of PM_lanes overflow in vmap allocation",
                tdm_cmn_get_port_pm(_tdm, port_token), port_token,
                subport_idx, TDM_VMAP_PM_LANES);
        }
    }

    return result;
}


/**
@name: tdm_vmap_op_pmlist_init
@param:
 */
void
tdm_vmap_op_pmlist_init(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist, int pmlist_size)
{
    int i;

    if (_tdm != NULL && pmlist != NULL) {
        for (i = 0; i < pmlist_size; i++) {
            tdm_vmap_op_pm_reset(_tdm, &(pmlist[i]));
        }
    }
}


/**
@name: tdm_vmap_op_pmlist_gen
@param:
 */
int
tdm_vmap_op_pmlist_gen(tdm_mod_t *_tdm, int *lr_buff, int lr_buff_size,
                       tdm_vmap_pm_t *pmlist, int pmlist_size)
{
    int i, j, prt_tmp, slots1, slots2, result = PASS;
    int port_phynum, port_pm, port_slots, pm_idx, pm_cnt, pm_existed;
    int param_token_empty;

    TDM_PRINT0("TDM: Construct pmlist for linerate ports\n\n");
    if (_tdm != NULL && lr_buff != NULL && pmlist != NULL) {
        param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
        /* step 1: sort ports in lr_buff by speed from highest to lowest */
        for (i = 0; i < lr_buff_size; i++) {
            if (lr_buff[i] == param_token_empty) break;
            for (j = lr_buff_size - 1; j > i; j--) {
                if (lr_buff[j] == param_token_empty) continue;
                slots1 = tdm_cmn_get_port_slots(_tdm, lr_buff[j-1]);
                slots2 = tdm_cmn_get_port_slots(_tdm, lr_buff[j]);
                if (slots1 > 0 && slots1 < slots2) {
                    prt_tmp      = lr_buff[j-1];
                    lr_buff[j-1] = lr_buff[j];
                    lr_buff[j]   = prt_tmp;
                }
            }
        }
        /* step 2: construct pmlist based on lr_buff */
        pm_cnt = 0;
        for (i = 0; i < lr_buff_size; i++) {
            port_phynum = lr_buff[i];
            if (port_phynum == param_token_empty) break;
            /* determine pm_idx for current port */
            port_pm = tdm_cmn_get_port_pm(_tdm, port_phynum);
            pm_existed = BOOL_FALSE;
            for (j = 0; j < pmlist_size && j < pm_cnt; j++) {
                if (pmlist[j].pm_num == port_pm) {
                    pm_existed = BOOL_TRUE;
                    break;
                }
            }
            pm_idx = (pm_existed == BOOL_FALSE) ? (pm_cnt++) : (j);
            /* add port into the selected pm in pm list */
            if (pm_idx < pmlist_size) {
                port_slots = tdm_cmn_get_port_slots(_tdm, port_phynum);
                if (tdm_vmap_op_pm_addPort(_tdm, &(pmlist[pm_idx]),
                    port_phynum, port_slots) != PASS) {
                    result = FAIL;
                }
            } else {
                result = FAIL;
                TDM_ERROR4("%s, port %0d, pm_idx %d, pm_size %0d\n",
                    "Number of PMs overflowed in vmap allocation",
                    port_phynum, pm_idx, pmlist_size);
            }
        }

        /* print pm list */
        tdm_vmap_print_pmlist(_tdm, pmlist, pmlist_size);
    }

    return result;
}


/**
@name: tdm_vmap_op_pmlist_sort
@param:

@desc: sort pms in pmlist.
        -- rule 1: sort by port speed from highest to lowest.
        -- rule 2: sort by pm_total_speed from highest to lowest.
 */
void
tdm_vmap_op_pmlist_sort(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                        int pmlist_size)
{
    int i, j, swap_happen;

    TDM_PRINT0("TDM: Sort pm list\n\n");
    if (_tdm != NULL && pmlist != NULL) {
        /* rule 0: sort PMs by pm bandwidth from highest to lowest */
        for (i = 0; i < pmlist_size; i++) {
            if (pmlist[i].pm_en == 0) break;
            swap_happen = BOOL_FALSE;
            for (j = pmlist_size - 1; j > i; j--) {
                if (pmlist[j].pm_en == 0) continue;
                if (pmlist[j].pm_slots >
                    pmlist[j-1].pm_slots) {
                    tdm_vmap_op_pm_swap(pmlist, pmlist_size, j, j-1);
                    swap_happen = BOOL_TRUE;
                }
            }
            if (swap_happen == BOOL_FALSE) break;
        }

        /* rule 1: for PMs with the same pm bandwidth, sort PMs by
         * port speed from highest to lowest
         */
        for (i = 0; i < pmlist_size; i++) {
            if (pmlist[i].pm_en == 0) break;
            swap_happen = BOOL_FALSE;
            for (j = pmlist_size - 1; j > i; j--) {
                if (pmlist[j].pm_en == 0) continue;
                if (pmlist[j].pm_slots == pmlist[j-1].pm_slots &&
                    pmlist[j].subport_slot_req[0] >
                    pmlist[j-1].subport_slot_req[0]) {
                    tdm_vmap_op_pm_swap(pmlist, pmlist_size, j, j-1);
                    swap_happen = BOOL_TRUE;
                }
            }
            if (swap_happen == BOOL_FALSE) break;
        }

        /* print pm list */
        tdm_vmap_print_pmlist(_tdm, pmlist, pmlist_size);
    }
}


/**
@name: tdm_vmap_op_pmlist_sort
@param:

@desc: sort pms in pmlist.
        -- rule 1: sort by port speed from highest to lowest.
        -- rule 2: sort by pm_total_speed from highest to lowest.
 */
void
tdm_vmap_op_pmlist_sort_2(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                        int pmlist_size)
{
    int i, j, swap_happen;

    TDM_PRINT0("TDM: Sort pm list\n\n");
    if (_tdm != NULL && pmlist != NULL) {
        /* rule 1: sort PMs by port speed from highest to lowest */
        for (i = 0; i < pmlist_size; i++) {
            if (pmlist[i].pm_en == 0) break;
            swap_happen = BOOL_FALSE;
            for (j = pmlist_size - 1; j > i; j--) {
                if (pmlist[j].pm_en == 0) continue;
                if (pmlist[j].subport_slot_req[0] >
                    pmlist[j-1].subport_slot_req[0]) {
                    tdm_vmap_op_pm_swap(pmlist, pmlist_size, j, j-1);
                    swap_happen = BOOL_TRUE;
                }
            }
            if (swap_happen == BOOL_FALSE) break;
        }

        /* rule 2: for PMs with the same highest port speed, sort by
         * PM_total_slots from highest to lowest
         */
        for (i = 0; i < pmlist_size; i++) {
            if (pmlist[i].pm_en == 0) break;
            swap_happen = BOOL_FALSE;
            for (j = pmlist_size - 1; j > i; j--) {
                if (pmlist[j].pm_en == 0) continue;
                if ((pmlist[j].subport_slot_req[0] ==
                     pmlist[j-1].subport_slot_req[0]) &&
                    (pmlist[j].pm_slots > pmlist[j-1].pm_slots)) {
                    tdm_vmap_op_pm_swap(pmlist, pmlist_size, j, j-1);
                    swap_happen = BOOL_TRUE;
                }
            }
            if (swap_happen == BOOL_FALSE) break;
        }

        /* print pm list */
        tdm_vmap_print_pmlist(_tdm, pmlist, pmlist_size);
    }
}


/**
@name: tdm_vmap_op_pmlist_shift
@param:

@desc: shift all pms starting from pm_idx forward by 1 step.
 */
void
tdm_vmap_op_pmlist_shift(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                         int pmlist_size, int pm_idx)
{
    int i, pm_idx_max;

    if (_tdm != NULL && pmlist != NULL) {
        /* find the last active pm in pmlist */
        pm_idx_max = pmlist_size - 1;
        while (!pmlist[pm_idx_max].pm_en && pm_idx_max > 0) pm_idx_max--;

        /* shift pm[pm_idx,...,pm_idx_max,...,(pmlist_size-1)] */
        for (i = pm_idx_max; i >= pm_idx && i > 0 ; i--) {
            tdm_vmap_op_pm_migrate(pmlist, pmlist_size, i, i+1);
        }

        /* clear pm at pm_idx */
        if (pm_idx < pmlist_size) {
            tdm_vmap_op_pm_reset(_tdm, &(pmlist[pm_idx]));
        }
    }
}


/**
@name: tdm_vmap_op_pmlist_adjust_lr_4lanes
@param:

Adjust special PM patterns by 1) inserting empty slots, or
                              2) splitting PM into two or more PMs.
special dual: s0_s1_0_0,   where !(s0==s1)
special tri : s0_s1_s2_0,  where !(s0==2*s1 && s1==s2)
special quad: s0_s1_s2_s3, where !(s0==s1 && s1==s2 && s2==s3)
 */
int
tdm_vmap_op_pmlist_adjust_lr_4lanes(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                                    int pmlist_size)
{
    int i, j, subport_cnt = 0, pm_cnt = 0;
    int slot_lr, slot_emp_dual, slot_emp_tri, slot_emp_quad;
    int *s, s_e;
    int split_en_dual = BOOL_TRUE, split_en_tri = BOOL_TRUE,
        split_en_quad = BOOL_TRUE;
    int param_lr_limit;

    if (_tdm == NULL || pmlist == NULL) return FAIL;

    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;

    TDM_PRINT0("TDM: Adjust pm list for special mode\n\n");
    /* calculate requested slots: slot_lr */
    slot_lr = 0;
    for (i = 0; i < pmlist_size; i++) {
        if (pmlist[i].pm_en == 0) break;
        for (j = 0; j < pmlist[i].subport_cnt && j < TDM_VMAP_PM_LANES; j++) {
            slot_lr += pmlist[i].subport_slot_req[j];
        }
        pm_cnt++;
    }

    /* calculate speculatively inserted empty slots */
    slot_emp_dual = 0;
    slot_emp_tri  = 0;
    slot_emp_quad = 0;
    for (i = 0; i < pmlist_size; i++) {
        subport_cnt = pmlist[i].subport_cnt;
        if (subport_cnt == 0) break;
        s = pmlist[i].subport_slot_req;
        /* special dual */
        if (subport_cnt==2 && !(s[0]==s[1])) {
            /* x_y_0_0, where x!=y */
            slot_emp_dual += (s[0] - s[1]);
        }
        /* special tri */
        else if (subport_cnt==3 && !(s[0]==2*s[1] && s[1]==s[2])) {
            /* x_x_x_0 */
            if (s[0]==s[1] && s[1]==s[2]) {
                slot_emp_tri += s[0];
            }
            /* x_y_z_0, where x!= 2*y and/or x!=2*z */
            else {
                slot_emp_tri += s[0] - s[1];
                slot_emp_tri += s[0] - s[2];
            }
        }
        /* special quad */
        else if (subport_cnt==4 && !(s[0]==s[1] && s[1]==s[2] && s[2]==s[3])) {
            /* x_y_z_w, where x!=y or x!=z or x!=w */
            slot_emp_quad += s[0] - s[1];
            slot_emp_quad += s[0] - s[2];
            slot_emp_quad += s[0] - s[3];
        }
    }
    if (slot_emp_dual > 0) TDM_PRINT0("TDM: Detect special DUAL PM mode\n");
    if (slot_emp_tri  > 0) TDM_PRINT0("TDM: Detect special TRI  PM mode\n");
    if (slot_emp_quad > 0) TDM_PRINT0("TDM: Detect special QUAD PM mode\n");

    /* determine split or not */
    if (slot_emp_dual > 0 && (slot_emp_dual + slot_lr) <= param_lr_limit) {
        split_en_dual = BOOL_FALSE;
        slot_lr += slot_emp_dual;
    }
    if (slot_emp_tri > 0 && (slot_emp_tri + slot_lr) <= param_lr_limit) {
        split_en_tri = BOOL_FALSE;
        slot_lr += slot_emp_tri;
    }
    if (slot_emp_quad > 0 && (slot_emp_quad + slot_lr) <= param_lr_limit) {
        split_en_quad = BOOL_FALSE;
        slot_lr += slot_emp_quad;
    }

    /* adjust special dual/tri/quad PMs
     *   -- case 1: insert empty slots to convert special mode PM to standard
     *   -- case 2: split a special mode PM into two or more standard mode PMs
     */
    /* special dual */
    if (slot_emp_dual > 0) {
        for (i = 0; i < pmlist_size; i++) {
            subport_cnt = pmlist[i].subport_cnt;
            if (subport_cnt == 0) break;
            s = pmlist[i].subport_slot_req;
            if (subport_cnt==2 && !(s[0]==s[1])) {
                /* case 1: split */
                if (split_en_dual == BOOL_TRUE) {
                    tdm_vmap_op_pm_addPort(_tdm, &(pmlist[pm_cnt]),
                                pmlist[i].subport_phy[1], s[1]);
                    pmlist[i].subport_cnt-= 1;
                    pmlist[i].pm_slots   -= s[1];
                    pm_cnt++;
                    TDM_PRINT4("%s, from PM[%2d] to PM[%2d], PM[%2d]\n",
                               "TDM: [special dual] Split PM", i, i, pm_cnt-1);
                }
                /* case 2: insert, x_y_0_0 -> x_x_0_0, where x>y */
                else {
                    s_e = s[0] - s[1];
                    pmlist[i].subport_slot_rsv[1] = s[0];
                    pmlist[i].pm_slots += s_e;
                    TDM_PRINT5("%s, PM[%2d] %2d, port %3d, emp_slot %0d\n",
                               "TDM: [special dual] Insert empty slots",
                               i, pmlist[i].pm_num,
                               pmlist[i].subport_phy[1], s_e);
                }
            }
        }
    }
    /* special tri */
    if (slot_emp_tri > 0) {
        for (i = 0; i < pmlist_size; i++) {
            subport_cnt = pmlist[i].subport_cnt;
            if (subport_cnt == 0) break;
            s = pmlist[i].subport_slot_req;
            if (subport_cnt==3 && !(s[0]==2*s[1] && s[1]==s[2])) {
                /* case 1: split */
                if (split_en_tri == BOOL_TRUE) {
                    if (s[0]>2*s[1] && s[1]==s[2]) {
                        tdm_vmap_op_pm_addPort(_tdm, &(pmlist[pm_cnt]),
                                    pmlist[i].subport_phy[1], s[1]);
                        tdm_vmap_op_pm_addPort(_tdm, &(pmlist[pm_cnt]),
                                    pmlist[i].subport_phy[2], s[2]);
                        pmlist[i].subport_cnt-= 2;
                        pmlist[i].pm_slots   -= (s[1] + s[2]);
                        pm_cnt++;
                        TDM_PRINT4("%s, from PM[%2d] to PM[%2d], PM[%2d]\n",
                                   "TDM: [special tri] Split PM",
                                   i, i, pm_cnt-1);
                    }
                }
                /* case 2: insert
                 *    -- 1. x_x_x_0    -> 2x_x_x_0
                 *    -- 2. 2p5x_x_x_0 -> 2p5x_1p25x_1p25x_0
                 *    -- 3. 4x_x_x_0   -> 4x_2x_2x_0
                 */
                else {
                    if (s[0]==s[1] && s[1]==s[2]) {
                        s_e = s[0];
                        pmlist[i].subport_slot_rsv[0] += s_e;
                        pmlist[i].pm_slots += s_e;
                        TDM_PRINT5("%s, PM[%2d] %2d, port %3d, emp_slot %0d\n",
                                   "TDM: [special tri] Insert empty slots",
                                   i, pmlist[i].pm_num,
                                   pmlist[i].subport_phy[0], s_e);
                    } else if (s[0]>2*s[1] && s[1]==s[2]) {
                        s_e = (s[0] - s[1] - s[2]) / 2;
                        for (j = 1; j < subport_cnt; j++) {
                            pmlist[i].subport_slot_rsv[j] += s_e;
                            pmlist[i].pm_slots += s_e;
                            TDM_PRINT5("%s, PM[%2d] %2d, port %3d, emp_slot %0d\n",
                                       "TDM: [special tri] Insert empty slots",
                                       i, pmlist[i].pm_num,
                                       pmlist[i].subport_phy[j], s_e);
                        }
                    }
                }
            }
        }
    }
    /* special quad */
    if (slot_emp_quad > 0) {
        for (i = 0; i < pmlist_size; i++) {
            subport_cnt = pmlist[i].subport_cnt;
            if (subport_cnt == 0) break;
            s = pmlist[i].subport_slot_req;
            if (subport_cnt==4 && !(s[0]==s[1] && s[1]==s[2] && s[2]==s[3])) {
                /* case 1: split */
                if (split_en_quad == BOOL_TRUE) {
                    for (j = 1; j < subport_cnt; j++) {
                        if (s[0] != s[j]) {
                            tdm_vmap_op_pm_addPort(_tdm, &(pmlist[pm_cnt]),
                                        pmlist[i].subport_phy[j], s[j]);
                            pmlist[i].subport_cnt-= 1;
                            pmlist[i].pm_slots   -= s[j];
                        }
                    }
                    pm_cnt++;
                    TDM_PRINT4("%s, from PM[%2d] to PM[%2d], PM[%2d]\n",
                               "TDM: [special quad] Split PM", i, i, pm_cnt-1);
                }
                /* case 2: insert, x_y_z_w -> x_x_x_x */
                else {
                    for (j = 1; j < subport_cnt; j++) {
                        if (s[0] != s[j]) {
                            s_e = (s[0] - s[j]);
                            pmlist[i].subport_slot_rsv[j] += s_e;
                            pmlist[i].pm_slots += s_e;
                            TDM_PRINT5("%s, PM[%2d] %2d, port %3d, emp_slot %0d\n",
                                       "TDM: [special quad] Insert empty slots",
                                       i, pmlist[i].pm_num,
                                       pmlist[i].subport_phy[j], s_e);
                        }
                    }
                }
            }
        }
    }

    /* print pm list */
    tdm_vmap_print_pmlist(_tdm, pmlist, pmlist_size);

    return PASS;
}


/**
@name: tdm_vmap_op_pmlist_adjust_lr
@param:
 */
int
tdm_vmap_op_pmlist_adjust_lr(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                             int pmlist_size)
{
    int param_num_pm_lanes;

    if (_tdm == NULL || pmlist == NULL) return FAIL;
    param_num_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;

    /* case 1: 4-lane PM */
    tdm_vmap_op_pmlist_adjust_lr_4lanes(_tdm, pmlist, pmlist_size);

    /* case 2: 8-lane PM */
    if (param_num_pm_lanes == 8) {
        /* add further optimization for 8-lane PM */
    }

    return PASS;
}


/**
@name: tdm_vmap_op_pmlist_adjust_os_4lanes
@param:

Create pseudo pm(s) for EMPTY slots. The purpose is to evenly distribute
oversub slots among linerate slots.
 */
int
tdm_vmap_op_pmlist_adjust_os_4lanes(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                                    int pmlist_size)
{
    int i, n, result = PASS;
    int lr_slot_num, empty_slot_num, empty_slot_left, empty_pm_num;
    int os_2_lr_r, lr_2_os_r, abs_r, abs_n, empty_slot_allc, slot_allc;
    int pm_slot_num, pm_slot_sum;
    int fac; /* correction factor */
    int param_cal_len, param_token_unused;

    if (_tdm == NULL || pmlist == NULL) return FAIL;
    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
    param_token_unused= TDM_VMAP_TOKEN_UNUSED;

    /* calculate lr_slot_num and empty_slot_num */
    lr_slot_num = 0;
    for (i = 0; i < pmlist_size; i++) {
        if (pmlist[i].subport_cnt > 0) {
            lr_slot_num += pmlist[i].pm_slots;
        }
    }

    empty_slot_num = param_cal_len - lr_slot_num;
    empty_slot_left= empty_slot_num;

    if (empty_slot_num > 0) {
        /* calculate bandwidth ratio between linerate and oversub */
        lr_2_os_r = 0;
        os_2_lr_r = 0;
        fac       = 105; /* margin 5% */
        if (empty_slot_num > lr_slot_num) {
            if (lr_slot_num > 0) {
                os_2_lr_r = (empty_slot_num * fac) / (lr_slot_num * 100);
            }
        } else {
            if (empty_slot_num > 0) {
                lr_2_os_r = (lr_slot_num * fac) / (empty_slot_num * 100);
            }
        }

        /* print bandwidth ratio */
        TDM_PRINT0("TDM: Insert pseudo PM for non-linerate slots\n\n");
        TDM_PRINT1("\t-- number of linerate slots    : %3d\n", lr_slot_num);
        TDM_PRINT1("\t-- number of non-linerate slots: %3d\n", empty_slot_num);
        if (empty_slot_num > lr_slot_num) {
            TDM_PRINT0("\t-- Linerate type   : Minority\n");
            TDM_PRINT1("\t-- empty_2_lr_ratio: %3d\n", os_2_lr_r);
        }
        else {
            TDM_PRINT0("\t-- Linerate type   : Majority\n");
            TDM_PRINT1("\t-- lr_2_empty_ratio: %3d\n", lr_2_os_r);
        }
        TDM_PRINT0("\n");

        /* bw_os > bw_lr */
        if (os_2_lr_r > 0) {
            for (i = 0; i < (pmlist_size - 1); i++) {
                if (pmlist[i].subport_cnt == 0) break;
                if (empty_slot_left <= 0) break;
                pm_slot_num = pmlist[i].pm_slots;
                if (pm_slot_num > 0 && empty_slot_left > 0) {
                    abs_n        = pm_slot_num * os_2_lr_r;
                    abs_n        = (abs_n > 0) ? abs_n : 1;
                    slot_allc    = (abs_n <= empty_slot_left) ?
                                   abs_n : empty_slot_left;
                    empty_pm_num = tdm_math_div_ceil(slot_allc, pm_slot_num);
                    for (n=0; n<empty_pm_num && i<(pmlist_size-1); n++) {
                        empty_slot_allc = (pm_slot_num <= slot_allc) ?
                                          pm_slot_num : slot_allc;
                        if (empty_slot_allc < pm_slot_num) break;
                        tdm_vmap_op_pmlist_shift(_tdm, pmlist, pmlist_size,
                                               i+1);
                        tdm_vmap_op_pm_addPort(_tdm, &(pmlist[i+1]),
                                        param_token_unused, empty_slot_allc);
                        slot_allc      -= empty_slot_allc;
                        empty_slot_left-= empty_slot_allc;
                        i++;
                        TDM_PRINT6("%s[%2d], port %0d, slots %0d (%s=%0d)\n",
                                   "TDM: Insert pseudo pm, PM",
                                   i,
                                   pmlist[i].subport_phy[0],
                                   pmlist[i].subport_slot_req[0],
                                   "empty_slot_left", empty_slot_left);
                    }
                }
            }
        }
        /* bw_lr > bw_os */
        else if (lr_2_os_r > 0) {
            pm_slot_sum = 0;
            for (i = 0; i < (pmlist_size - 1); i++) {
                if (pmlist[i].subport_cnt == 0) break;
                if (empty_slot_left <= 0) break;
                pm_slot_num = pmlist[i].pm_slots;
                pm_slot_sum += pm_slot_num;
                if (pm_slot_sum > 0 && empty_slot_left > 0) {
                    abs_r = lr_slot_num / pm_slot_sum;
                    abs_n = empty_slot_num / abs_r;
                    abs_n = (lr_2_os_r == 1) ? pm_slot_sum : abs_n;
                    abs_n = (abs_n < pm_slot_num) ? abs_n : pm_slot_num;
                    if (pm_slot_sum / lr_2_os_r >= pm_slot_num) {
                        empty_slot_allc = (abs_n <= empty_slot_left) ?
                                          abs_n : empty_slot_left;
                        if (empty_slot_allc < pm_slot_num) continue;
                        tdm_vmap_op_pmlist_shift(_tdm, pmlist, pmlist_size,
                                               i+1);
                        tdm_vmap_op_pm_addPort(_tdm, &(pmlist[i+1]),
                                        param_token_unused, empty_slot_allc);
                        
                        empty_slot_left -= empty_slot_allc;
                        i++;
                        pm_slot_sum = 0;
                        TDM_PRINT6("%s[%2d], port %0d, slots %0d (%s=%0d)\n",
                                   "TDM: Insert pseudo pm, PM",
                                   i,
                                   pmlist[i].subport_phy[0],
                                   pmlist[i].subport_slot_req[0],
                                   "empty_slot_left", empty_slot_left);
                    }
                }
            }
            /* improve the last pseudo PM */
            if (empty_slot_left > 0) {
                /* if 1) last active PM (pmlist[i]) is a pseudo PM, and
                 *    2) second-last PM (pmlist[i-1])is not a pseudo PM, and
                 *    3) pmlist[i].pm_slots >= pmlist[i-1].pm_slots, and
                 *    4) empty_slot_left >= pmlist[i].pm_slots * fac
                 * then swap the the last two PMs.
                 */
                fac = 150;
                for (i = pmlist_size - 1; i > 0; i--) {
                    if (pmlist[i].subport_cnt != 0) break;
                }
                if (pmlist[i].subport_phy[0] == param_token_unused && i > 1) {
                    if (pmlist[i-1].subport_phy[0] != param_token_unused &&
                        pmlist[i].pm_slots >= pmlist[i-1].pm_slots &&
                        empty_slot_left * fac >= pmlist[i].pm_slots * 100) {
                        tdm_vmap_op_pm_swap(pmlist, pmlist_size, i, i-1);
                        TDM_PRINT3("%s, PM[%2d] to PM[%2d]\n",
                                   "TDM: Migrate pseudo pm",
                                   i, i-1);
                    }
                }
            }
        }

        /* check validity */
        if (empty_slot_left < 0) {
            /* note: this branch should never be touched */
            result = FAIL;
            TDM_ERROR1("TDM: %d extra EMPTY slots allocated\n",
                       (-empty_slot_left));
        }
        /* print pm list */
        tdm_vmap_print_pmlist(_tdm, pmlist, pmlist_size);
    }

    return result;
}


/**
@name: tdm_vmap_op_pmlist_adjust_os
@param:
 */
int
tdm_vmap_op_pmlist_adjust_os(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                             int pmlist_size)
{
    int param_num_pm_lanes;

    if (_tdm == NULL || pmlist == NULL) return FAIL;
    param_num_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;

    /* case 1: 4-lane PM */
    tdm_vmap_op_pmlist_adjust_os_4lanes(_tdm, pmlist, pmlist_size);

    /* case 2: 8-lane PM */
    if (param_num_pm_lanes == 8) {
        /* add further optimization for 8-lane PM */
    }

    return PASS;
}


/**
@name: tdm_vmap_get_vmap_wid
@param:
 */
int
tdm_vmap_get_vmap_wid(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, vmap_wid = 0;
    int param_vmap_wid, param_vmap_len, param_token_empty;

    if (_tdm == NULL || vmap == NULL) return vmap_wid;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    for (i = param_vmap_wid - 1; i > 0; i--) {
        for (j = 0; j < param_vmap_len; j++) {
            if (vmap[i][j] != param_token_empty) {
                vmap_wid = i + 1;
                break;
            }
        }
        if (vmap_wid > 0) break;
    }

    return vmap_wid;
}


/**
@name: tdm_vmap_get_vmap_len
@param:
 */
int
tdm_vmap_get_vmap_len(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int vmap_len = 0;
    int param_cal_len, param_vmap_len;

    if (_tdm == NULL) return vmap_len;
    param_cal_len  = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                     _tdm->_chip_data.soc_pkg.tvec_size;
    param_vmap_len = _tdm->_core_data.vmap_max_len;

    vmap_len = (param_cal_len < param_vmap_len) ?
               param_cal_len : param_vmap_len;

    return vmap_len;
}


/**
@name: tdm_vmap_get_port_space_sister
@param:
@desc: Return sister port space limit of the given port.
 */
int
tdm_vmap_get_port_space_sister(tdm_mod_t *_tdm, int port_token)
{
    int min_sister_space = 0;

    if (_tdm != NULL) {
        min_sister_space = _tdm->_core_data.rule__prox_port_min;
        /* for device with diff sister port spaces, retrieve space limit */
        /*
        if (_tdm->_core_exec[TDM_CORE_EXEC__SISTER_PORT_SPACE] != NULL) {
            _tdm->_core_data.vars_pkg.port = port_token;
            _tdm->_core_exec[TDM_CORE_EXEC__SISTER_PORT_SPACE](_tdm);
        } */
    }

    return min_sister_space;
}


/**
@name: tdm_vmap_get_port_space_same
@param:
@desc: Return same port space limit of the given port.
 */
int
tdm_vmap_get_port_space_same(tdm_mod_t *_tdm, int port_token)
{
    int min_same_space = 0;

    if (_tdm != NULL) {
        min_same_space = _tdm->_core_data.rule__same_port_min;
        /* for device with diff sister spaces, retrieve space limit */
        /*
        if (_tdm->_core_exec[TDM_CORE_EXEC__SAME_PORT_SPACE] != NULL) {
            _tdm->_core_data.vars_pkg.port = port_token;
            _tdm->_core_exec[TDM_CORE_EXEC__SAME_PORT_SPACE](_tdm);
        } */
    }

    return min_same_space;
}


/**
@name: tdm_vmap_calc_port_dist_sister
@param:
    -- cal    : slot array
    -- cal_len: size of slot array
    -- espace : extra space added into space limit, default is 0.
    -- idx    : slot index in input array, which designated checking slot.
    -- dir    : checking direction, TDM_DIR_UP or TDM_DIR_DN.

Return 0 if sister-port-space violation is detected on the port at given slot,
otherwise return the distance between given slot and conflicted slot.
 */
int
tdm_vmap_calc_port_dist_sister(tdm_mod_t *_tdm, int *cal, int cal_len,
                              int espace, int idx, int dir)
{
    int n, k, space, port_pm, distance = 0;

    if (cal != NULL && cal_len > 0 && idx >= 0 && idx < cal_len) {
        if (tdm_cmn_chk_port_is_fp(_tdm, cal[idx])) {
            space = tdm_vmap_get_port_space_sister(_tdm, cal[idx]) + espace;
            port_pm = tdm_cmn_get_port_pm(_tdm, cal[idx]);
            for (n = 1; n < space; n++) {
                k = (dir == TDM_DIR_DN) ? ((idx + n) % cal_len) :
                                          ((cal_len + idx - n) % cal_len);
                if (tdm_cmn_chk_port_is_fp(_tdm, cal[k])) {
                    if (port_pm == tdm_cmn_get_port_pm(_tdm, cal[k])) {
                        distance = n;
                        break;
                    }
                }
            }
        }
    }

    return distance;
}


/**
@name: tdm_vmap_calc_port_dist_same
@param:
    -- cal    : slot array
    -- cal_len: size of slot array
    -- espace : extra space added into space limit, default is 0.
    -- idx    : slot index in input array, which designated checking slot.
    -- dir    : checking direction, TDM_DIR_UP or TDM_DIR_DN.

Return 0 if same-port-space violation is detected on the port at given slot,
otherwise return the distance between given slot and conflicted slot.
 */
int
tdm_vmap_calc_port_dist_same(tdm_mod_t *_tdm, int *cal, int cal_len,
                            int espace, int idx, int dir)
{
    int n, k, space, distance = 0;

    if (cal != NULL && cal_len > 0 && idx >= 0 && idx < cal_len) {
        if (tdm_cmn_chk_port_is_fp(_tdm, cal[idx])) {
            space = tdm_vmap_get_port_space_same(_tdm, cal[idx]) + espace;
            for (n = 1; n < space; n++) {
                k = (dir == TDM_DIR_DN) ? ((idx + n) % cal_len) :
                                          ((cal_len + idx - n) % cal_len);
                if (cal[idx] == cal[k]) {
                    distance = n;
                    break;
                }
            }
        }
    }

    return distance;
}


/**
@name: tdm_vmap_gen_lr_buff
@param:
 */
int
tdm_vmap_gen_lr_buff(int *src_buff, int src_buff_size,
                     int *dst_buff, int dst_buff_size, int token_empty)
{
    int i, j, port_cnt = 0, port_existed, result = PASS;

    if (src_buff != NULL && dst_buff != NULL) {
        for (i = 0; i < src_buff_size; i++) {
            if (src_buff[i] == token_empty) continue;
            port_existed = BOOL_FALSE;
            for (j = 0; j < port_cnt && j < dst_buff_size; j++) {
                if (dst_buff[j] == src_buff[i]) {
                    port_existed = BOOL_TRUE;
                    break;
                }
            }
            if (port_existed == BOOL_FALSE) {
                if (port_cnt < dst_buff_size) {
                    dst_buff[port_cnt++] = src_buff[i];
                } else {
                    result = FAIL;
                    TDM_ERROR4("%s, port %0d, cnt %0d, buff_size %0d\n",
                               "lr_buff overflow in vmap allocation",
                               src_buff[i], port_cnt, dst_buff_size);
                }
            }
        }
    }

    return result;
}


/**
@name: tdm_vmap_switch_vmap_col
@param:
@desc: switch couterpart non-empty slots between column x and y if number
       of physical ports for column x and y is equivalent.
 */
int
tdm_vmap_switch_vmap_col(tdm_mod_t *_tdm, unsigned short **vmap,
                         int col_x, int col_y)
{
    int j, row_x, row_y, y_prev, slot_cnt_x, slot_cnt_y,
        token_tmp, result = FAIL;
    int param_token_empty, param_vmap_wid, param_vmap_len;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;

    if (col_x >= 0 && col_x < param_vmap_wid &&
        col_y >= 0 && col_y < param_vmap_wid) {
        slot_cnt_x = 0;
        slot_cnt_y = 0;
        for (j = 0; j < param_vmap_len; j++) {
            if (vmap[col_x][j] != param_token_empty) slot_cnt_x++;
            if (vmap[col_y][j] != param_token_empty) slot_cnt_y++;
        }
        if (slot_cnt_x == slot_cnt_y && slot_cnt_x > 0) {
            result = PASS;
            y_prev = 0;
            for (row_x = 0; row_x < param_vmap_len; row_x++) {
                if (vmap[col_x][row_x] != param_token_empty) {
                    for (row_y = y_prev; row_y < param_vmap_len; row_y++) {
                        if (vmap[col_y][row_y] != param_token_empty) {
                            token_tmp = vmap[col_x][row_x];
                            vmap[col_x][row_x] = vmap[col_y][row_y];
                            vmap[col_y][row_y] = token_tmp;
                            y_prev = row_y + 1;
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
@name: tdm_vmap_rotate_vmap
@param:
 */
int
tdm_vmap_rotate_vmap(tdm_mod_t *_tdm, unsigned short **vmap)
{
    int i, j, vmap_wid, chk_i, chk_j, result = PASS;

    TDM_PRINT0("TDM: Rotate VMAP to remove sister spacing violation\n\n");
    vmap_wid = tdm_vmap_get_vmap_wid(_tdm, vmap);
    for (i = vmap_wid - 1; i > 0; i--) {
        if (tdm_vmap_chk_vmap_sister_col(_tdm, vmap, i) != PASS) {
            TDM_PRINT1("TDM: detect sister port violation at column %d\n", i);
            for (j = i - 1; j >= 0; j--) {
                if(tdm_vmap_switch_vmap_col(_tdm, vmap, i, j) == PASS) {
                    chk_i = tdm_vmap_chk_vmap_sister_col(_tdm, vmap, i);
                    chk_j = tdm_vmap_chk_vmap_sister_col(_tdm, vmap, j);
                    if (chk_i != PASS) {
                        /* switch back column i with j */
                        tdm_vmap_switch_vmap_col(_tdm, vmap, i, j);
                    } else {
                        if (chk_j == PASS) {
                            TDM_PRINT2("\t\t(a) switch column %d with %d\n",
                                       i, j);
                            break;
                        } else {
                            TDM_PRINT2("\t\t(b) switch column %d with %d\n",
                                       i, j);
                            i = j;
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
@name: tdm_vmap_shift_port
@param:
 */
int
tdm_vmap_shift_port(tdm_mod_t *_tdm, int port_token)
{
    int i, j, n, x1, x2, d_dn, d_up, d_x1, d_x2,
        min_sister_space, shift_happen;
    int param_cal_len;
    int *param_cal_main;

    param_cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id, param_cal_main);
    min_sister_space = tdm_vmap_get_port_space_sister(_tdm, port_token);

    for (n = 0; n < min_sister_space; n++) {
        shift_happen = BOOL_FALSE;
        for (i = 0; i < param_cal_len; i++) {
            if (param_cal_main[i] == port_token) {
                d_dn = tdm_vmap_calc_port_dist_sister(_tdm, param_cal_main,
                        param_cal_len, 0, i, TDM_DIR_DN);
                d_up = tdm_vmap_calc_port_dist_sister(_tdm, param_cal_main,
                        param_cal_len, 0, i, TDM_DIR_UP);
                /* shift up */
                if (d_dn > 0) {
                    for (j = 0; j < param_cal_len; j++) {
                        if (param_cal_main[j] == port_token) {
                            x1 = j;
                            x2 = (param_cal_len + j - 1) % param_cal_len;
                            d_x1 = tdm_vmap_calc_port_dist_sister(_tdm,
                                        param_cal_main, param_cal_len, 0,
                                        x1, TDM_DIR_UP);
                            d_x2 = tdm_vmap_calc_port_dist_sister(_tdm,
                                        param_cal_main, param_cal_len, 0,
                                        x2, TDM_DIR_DN);
                            if (d_x1 == 0 && d_x2 == 0) {
                                param_cal_main[x1] = param_cal_main[x2];
                                param_cal_main[x2] = port_token;
                                shift_happen = BOOL_TRUE;
                                TDM_PRINT4("%s, port %3d, slot %03d to %03d\n",
                                           "TDM: shift port   UP",
                                           port_token, x1, x2 );
                            }
                        }
                    }
                }
                /* shift down */
                else if (d_up > 0) {
                    for (j = 0; j < param_cal_len; j++) {
                        if (param_cal_main[j] == port_token) {
                            x1 = j;
                            x2 = (j + 1) % param_cal_len;
                            d_x1 = tdm_vmap_calc_port_dist_sister(_tdm,
                                        param_cal_main, param_cal_len, 0,
                                        x1, TDM_DIR_DN);
                            d_x2 = tdm_vmap_calc_port_dist_sister(_tdm,
                                        param_cal_main, param_cal_len, 0,
                                        x2, TDM_DIR_UP);
                            if (d_x1 == 0 && d_x2 == 0) {
                                param_cal_main[x1] = param_cal_main[x2];
                                param_cal_main[x2] = port_token;
                                shift_happen = BOOL_TRUE;
                                TDM_PRINT4("%s, port %3d, slot %03d to %03d\n",
                                           "TDM: shift port DOWN",
                                           port_token, x1, x2 );
                            }
                        }
                    }
                }
            }
            if (shift_happen == BOOL_TRUE) break;
        }
        if (shift_happen == BOOL_FALSE) break;
    }

    return PASS;
}


/**
@name: tdm_vmap_filter_same
@param:
 */
int
tdm_vmap_filter_same(tdm_mod_t *_tdm)
{
    int i, m, port_i, dist_i, space_i, x, y, result;
    int port, chk_x, chk_y;
    int param_lr_limit, param_ancl_num, param_cal_len;
    int *param_cal_main;

    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    TDM_PRINT0("TDM: Adjust same port space\n\n");
    /* shift up */
    for (i = 0; i < param_cal_len; i++) {
        port_i = param_cal_main[i];
        if (tdm_cmn_chk_port_is_fp(_tdm, port_i)) {
            dist_i = tdm_vmap_calc_port_dist_same(_tdm, param_cal_main,
                        param_cal_len, 0, i, TDM_DIR_DN);
            if (dist_i != 0) {
                space_i = tdm_vmap_get_port_space_same(_tdm, port_i);
                for (m = 1; m < (space_i - dist_i); m++) {
                    x = (param_cal_len + i - m) % param_cal_len;
                    y = (x + 1) % param_cal_len;
                    chk_x = tdm_vmap_chk_port_space(_tdm, param_cal_main,
                                param_cal_len, 1, x, TDM_DIR_DN);
                    chk_y = tdm_vmap_chk_port_space(_tdm, param_cal_main,
                                param_cal_len, 1, y, TDM_DIR_UP);
                    if (chk_x == PASS && chk_y == PASS) {
                        port = param_cal_main[y];
                        param_cal_main[y] = param_cal_main[x];
                        param_cal_main[x] = port;
                        TDM_PRINT4("%s, port %3d from slot %03d to %03d\n",
                                   "[same-port-space] Shift port   UP",
                                   port, y, x);
                    } else {
                        TDM_PRINT4("TDM: %s port %3d, slot [%3d, %3d]\n",
                                   "[same-port-space violation]",
                                   port_i, i, ((i + dist_i) % param_cal_len));
                        break;
                    }
                }
            }
        }
    }
    /* shift down */
    for (i = 0; i < param_cal_len; i++) {
        port_i = param_cal_main[i];
        if (tdm_cmn_chk_port_is_fp(_tdm, port_i)) {
            dist_i = tdm_vmap_calc_port_dist_same(_tdm, param_cal_main,
                        param_cal_len, 0, i, TDM_DIR_UP);
            if (dist_i != 0) {
                space_i = tdm_vmap_get_port_space_same(_tdm, port_i);
                for (m = 1; m < (space_i - dist_i); m++) {
                    x = (i + m) % param_cal_len;
                    y = (param_cal_len + x - 1) % param_cal_len;
                    chk_x = tdm_vmap_chk_port_space(_tdm, param_cal_main,
                                param_cal_len, 1, x, TDM_DIR_UP);
                    chk_y = tdm_vmap_chk_port_space(_tdm, param_cal_main,
                                param_cal_len, 1, y, TDM_DIR_DN);
                    if (chk_x == PASS && chk_y == PASS) {
                        port = param_cal_main[y];
                        param_cal_main[y] = param_cal_main[x];
                        param_cal_main[x] = port;
                        TDM_PRINT4("%s, port %3d from slot %03d to %03d\n",
                                   "[same-port-space] Shift port   UP",
                                   port, y, x);
                    } else {
                        TDM_PRINT4("TDM: %s port %3d, slot [%3d, %3d]\n",
                                   "[same-port-space violation]",
                                   port_i, i, ((i + dist_i) % param_cal_len));
                        break;
                    }
                    if (chk_x == PASS && chk_y == PASS) {
                        port = param_cal_main[y];
                        param_cal_main[y] = param_cal_main[x];
                        param_cal_main[x] = port;
                        TDM_PRINT4("%s, port %3d from slot %03d to %03d\n",
                                   "[same-port-space] Shift port DOWN",
                                   port, y, x);
                    } else {
                        TDM_PRINT4("TDM: %s port %3d, slot [%03d, %03d]\n",
                                   "[same-port-space violation]",
                                   port_i, (i - dist_i), i);
                        break;
                    }
                }
            }
        }
    }

    result = tdm_vmap_chk_same(_tdm);
    return result;
}


/**
@name: tdm_vmap_filter_sister
@param:
 */
int
tdm_vmap_filter_sister(tdm_mod_t *_tdm)
{
    int i, j, n, port, port_i, port_j, sister_swap, dist, result;
    int param_lr_limit, param_ancl_num, param_cal_len, param_lr_cnt;
    int *param_cal_main;
    enum port_speed_e *param_speeds;

    /* set parameters */
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_lr_cnt      = _tdm->_core_data.vars_pkg.pipe_info.num_lr_prt;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);

    TDM_PRINT0("TDM: Adjust sister port space (0)\n\n");
    for (n=0; n<param_lr_cnt; n++) {
        sister_swap = BOOL_FALSE;
        for (i=0; i<param_cal_len; i++) {
            port_i = param_cal_main[i];
            if (tdm_cmn_chk_port_is_fp(_tdm, port_i)) {
                dist = tdm_vmap_calc_port_dist_sister(_tdm, param_cal_main,
                            param_cal_len, 0, i, TDM_DIR_DN);
                if (dist != 0) {
                    j = (i + dist) % param_cal_len;
                    port_j = param_cal_main[j];
                    if (tdm_cmn_chk_port_is_fp(_tdm, port_j)) {
                        port = (param_speeds[port_i] < param_speeds[port_j]) ?
                               port_i : port_j;
                        tdm_vmap_shift_port(_tdm, port);
                        sister_swap = BOOL_TRUE;
                        break;
                    }
                }
            }
        }
        if (sister_swap==BOOL_FALSE) {
            break;
        }
    }
    result = tdm_vmap_chk_sister(_tdm);
    return result;
}


/**
@name: tdm_vmap_calc_slot_pos
@param:
 */
int
tdm_vmap_calc_slot_pos(int vmap_idx, int slot_idx, int vmap_idx_max, int slot_idx_max, int **r_a_arr)
{
    int i, j, a, b, n, pos, check;

    pos = slot_idx_max+1;
    if (r_a_arr!=NULL &&
        vmap_idx>=0 && vmap_idx<vmap_idx_max &&
        slot_idx>=0 && slot_idx<slot_idx_max) {
        check = PASS;
        for (i=0; i<=vmap_idx; i++) {
            if (r_a_arr[i][0]<=0 || r_a_arr[i][1]<=0) {
                check = FAIL;
                break;
            }
            if ( i!=vmap_idx && r_a_arr[i][0]==r_a_arr[i][1] ) {
                check = FAIL;
                break;
            }
        }
        if (check == PASS) {
            i = slot_idx+1;
            n = vmap_idx+1;
            for (j=n; j>0; j--) {
                a = r_a_arr[j-1][0];
                b = r_a_arr[j-1][1];
                if (j==n) {
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
@name: tdm_vmap_alloc_one_port
@param:
 */
int
tdm_vmap_alloc_one_port(int port_token, int slot_req, int slot_left, int cal_len, int vmap_idx, int vmap_wid_max, int vmap_len_max, unsigned short **vmap, int **r_a_arr)
{
    int i, pos, slot_cnt=0, result=FAIL;

    if (vmap != NULL  && r_a_arr != NULL &&
        vmap_idx >= 0 && vmap_idx < vmap_wid_max &&
        slot_req > 0  && slot_req <= slot_left) {
        r_a_arr[vmap_idx][0] = slot_req;
        r_a_arr[vmap_idx][1] = slot_left;

        for (i=0; i<slot_req; i++) {
            pos = tdm_vmap_calc_slot_pos(vmap_idx, i, vmap_wid_max, cal_len,
                                         r_a_arr);
            if(pos > 0 && pos <= cal_len && pos <= vmap_len_max) {
                vmap[vmap_idx][pos-1] = port_token;
                slot_cnt++;
            } else{
                TDM_ERROR7(
                    "%s, port %0d, %0d-th slot (over %0d), %s <%0d,%0d>\n",
                    "pos OVERRUN in vmap allocation",
                    port_token, i, slot_req, "<pos, pos_max>=", pos, cal_len);
            }
        }

        if (slot_cnt == slot_req) result = PASS;
    } else{
        TDM_ERROR4("%s, slot_req %0d, slot_avail %0d, vmap_idx %0d\n",
                  "invalid condition for pos calculation in vmap allocation",
                  slot_req, slot_left, vmap_idx);
    }

    return result;
}


/**
@name: tdm_vmap_part_pm_subports
@param:
 */
int
tdm_vmap_part_pm_subports(tdm_mod_t *_tdm, int port_token,
tdm_vmap_pm_t *pm, int vmap_idx, unsigned short **vmap)
{
    int i, j, slot_cnt, result = PASS;
    int tmp_slot_req, tmp_slot_left, tmp_idx, tmp_wid, tmp_len, tmp_result,
        tmp_port_token;
    unsigned short **tmp_vmap;
    int **tmp_r_a_arr;
    int param_token_empty, param_vmap_wid, param_vmap_len;

    /* set parameters */
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;


    tmp_wid = (*pm).subport_cnt;
    tmp_len = (*pm).pm_slots;
    /* malloc: tmp_vmap */
    tmp_vmap = (unsigned short**) TDM_ALLOC(
               tmp_wid * sizeof(unsigned short*), "tmp_vmap_l1");
    for (i = 0; i < tmp_wid; i++) {
        tmp_vmap[i] = (unsigned short*) TDM_ALLOC(
                      tmp_len * sizeof(unsigned short), "tmp_vmap_l2");
    }
    /* malloc: tmp_r_a_arr */
    tmp_r_a_arr = (int**) TDM_ALLOC(tmp_wid * sizeof(int*), "tmp_r_a_arr_l1");
    for (i = 0; i < tmp_wid; i++) {
        tmp_r_a_arr[i] = (int*) TDM_ALLOC(2 * sizeof(int), "tmp_r_a_arr_l2");
    }
    /* init */
    for (i = 0; i < tmp_wid; i++) {
        for (j = 0; j < tmp_len; j++) {
            tmp_vmap[i][j] = param_token_empty;
        }
    }
    for (i = 0; i < tmp_wid; i++) {
        tmp_r_a_arr[i][0] = 0;
        tmp_r_a_arr[i][1] = 0;
    }

    /* alloc slot for each port */
    tmp_idx       = 0;
    tmp_slot_left = tmp_len;
    for (i = 0; i < tmp_wid; i++) {
        tmp_port_token = (*pm).subport_phy[i];
        tmp_slot_req   = (*pm).subport_slot_req[i];
        tmp_result = tdm_vmap_alloc_one_port(
                        tmp_port_token, tmp_slot_req, tmp_slot_left, tmp_len,
                        tmp_idx, tmp_wid, tmp_len, tmp_vmap, tmp_r_a_arr);
        if (tmp_result != PASS) {
            result = FAIL;
            TDM_ERROR4("%s, port %d, slot_req %d, slot_avail %d\n",
                      "FAILED in tmp_vmap allocation",
                      tmp_port_token, tmp_slot_req, tmp_slot_left);
            break;
        }
        tmp_slot_left -= tmp_slot_req;
        tmp_idx++;
    }

    /* fill vmap[][] */
    if (vmap != NULL && vmap_idx < param_vmap_wid) {
        slot_cnt = 0;
        for (i = 0; i < param_vmap_len; i++) {
            if (vmap[vmap_idx][i] == port_token && slot_cnt < tmp_len) {
                tmp_port_token = param_token_empty;
                for (j = 0; j < tmp_wid; j++) {
                    if (tmp_vmap[j][slot_cnt] != param_token_empty) {
                        tmp_port_token = tmp_vmap[j][slot_cnt];
                        break;
                    }
                }
                vmap[vmap_idx][i] = tmp_port_token;
                slot_cnt++;
            }
            if (slot_cnt >= tmp_len) break;
        }
    }

    for (i = 0; i < tmp_wid; i++) {
        TDM_FREE(tmp_vmap[i]);
        TDM_FREE(tmp_r_a_arr[i]);
    }
    TDM_FREE(tmp_vmap);
    TDM_FREE(tmp_r_a_arr);

    return result;
}


/**
@name: tdm_vmap_patch_special_case
@param:
 */
void
tdm_vmap_patch_special_case(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                            int pmlist_size)
{
    int i, j;

    /* SPECIAL CASE [Tomahawk]: 4x106HG+3x40G uses a unique TDM */
    if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G == BOOL_TRUE) {
        TDM_PRINT1("TDM: [SPECIAL CASE] Tomahawk 4x106HG+3x40G, %s\n",
                   "decrement 1 slot (2.5G) for each port");
        for (i = 0; i < pmlist_size; i++) {
            if (pmlist[i].subport_cnt > 0 && pmlist[i].pm_slots > 0) {
                for (j = 0; j < pmlist[i].subport_cnt &&
                            j < TDM_VMAP_PM_LANES; j++) {
                    pmlist[i].subport_slot_req[j] -= 1;
                    pmlist[i].subport_slot_rsv[j] -= 1;
                    pmlist[i].pm_slots -= 1;
                }
            }
        }
        TDM_SML_BAR
    }
}


/**
@name: tdm_vmap_gen_vmap
@param:
 */
int
tdm_vmap_gen_vmap(tdm_mod_t *_tdm, tdm_vmap_pm_t *pmlist,
                  int pmlist_size, unsigned short **vmap)
{
    int result = PASS;
    int i, j, vmap_idx, port_token, port_cnt, slot_req, slot_left, result_tmp;
    int param_cal_len, param_vmap_wid, param_vmap_len;
    int **r_a_arr;

    param_cal_len  = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                     _tdm->_chip_data.soc_pkg.tvec_size;
    param_vmap_wid = _tdm->_core_data.vmap_max_wid;
    param_vmap_len = _tdm->_core_data.vmap_max_len;

    /* allocate memory */
    r_a_arr = (int**) TDM_ALLOC(param_vmap_wid * sizeof(int*), "r_a_arr_l1");
    for (i = 0; i < param_vmap_wid; i++) {
        r_a_arr[i] = (int*) TDM_ALLOC(2 * sizeof(int), "r_a_arr_l2");
    }
    for (i=0; i<param_vmap_wid; i++) {
        r_a_arr[i][0] = 0;
        r_a_arr[i][1] = 0;
    }

    /* allocate slots for each PM in pm list.
     *      -- step 1: allocate slots for all ports within a PM.
     *      -- step 2: partition slots for each subport within a PM.
     */
    TDM_PRINT0("TDM: Allocate slots for each PM \n\n");
    vmap_idx  = 0;
    slot_left = param_cal_len;
    for (i = 0; i < pmlist_size; i++) {
        port_cnt = pmlist[i].subport_cnt;
        slot_req = pmlist[i].pm_slots;
        if (port_cnt > 0 && slot_req > 0) {
            TDM_PRINT2("TDM: Allocating PM[%2d] %2d port <",
                       i, pmlist[i].pm_num);
            for (j = 0; j < port_cnt && j < TDM_VMAP_PM_LANES; j++) {
                TDM_PRINT1("%3d", pmlist[i].subport_phy[j]);
                if (j != port_cnt - 1) TDM_PRINT0(", ");
            }
            TDM_PRINT2("> with %3d slots from %0d\n", slot_req, slot_left);

            /* step 1: allocate slots for the entire PM */
            port_token = pmlist[i].subport_phy[0];
            result_tmp = tdm_vmap_alloc_one_port(port_token, slot_req,
                            slot_left, param_cal_len, vmap_idx,
                            param_vmap_wid, param_vmap_len, vmap, r_a_arr);

            if (result_tmp != PASS) {
                result = FAIL;
                TDM_ERROR4("%s, port %d, slot_req %d, slot_avail %d\n",
                          "FAILED in linerate slot vmap allocation",
                          port_token, slot_req, slot_left);
                break;
            }
            /* step 2: partition slots for all subports */
            /* if (port_cnt > 1) { */
            if (1) {
                tdm_vmap_part_pm_subports(_tdm, port_token,
                                          &(pmlist[i]), vmap_idx, vmap);
            }
            slot_left -= slot_req;
            vmap_idx++;
        }
    }
    TDM_SML_BAR

    /* free memory */
    for (i = 0; i < param_vmap_wid; i++) {
        TDM_FREE(r_a_arr[i]);
    }
    TDM_FREE(r_a_arr);

    return result;
}


/**
@name: tdm_vmap_alloc
@param:
 */
int
tdm_vmap_alloc(tdm_mod_t *_tdm)
{
    int i, j, result = PASS;
    int *lr_buff, lr_buff_size;
    unsigned short **vmap;
    int pmlist_size;
    tdm_vmap_pm_t *pmlist;
    int param_token_empty, param_token_unused,
        param_vmap_wid, param_vmap_len, param_lr_en;
    int *param_lr_buff;
    unsigned short **param_vmap;

    /* print stack usage */
    /* TDM_PRINT_STACK_SIZE("tdm_core_vmap_alloc"); */

    /* set parameters */
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_unused= TDM_VMAP_TOKEN_UNUSED;
    param_vmap_wid    = _tdm->_core_data.vmap_max_wid;
    param_vmap_len    = _tdm->_core_data.vmap_max_len;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_lr_buff     = _tdm->_core_data.vars_pkg.lr_buffer;
    param_vmap        = _tdm->_core_data.vmap;

    /* set memory size */
    pmlist_size = param_vmap_wid; /* 2 * param_num_phy_pm; */
    lr_buff_size = param_vmap_wid;

    /* allocate memory */
    pmlist  = (tdm_vmap_pm_t *) TDM_ALLOC(
                        pmlist_size * sizeof(tdm_vmap_pm_t), "pmlist");
    lr_buff = (int*) TDM_ALLOC(lr_buff_size * sizeof(int), "lr_buff");
    vmap    = (unsigned short**) TDM_ALLOC(
                        param_vmap_wid * sizeof(unsigned short*), "vmap_l1");
    for (i = 0; i < param_vmap_wid; i++) {
        vmap[i] = (unsigned short*)
                  TDM_ALLOC(param_vmap_len * sizeof(unsigned short), "vmap_l2");
    }

    /* initialize memory */
    tdm_vmap_op_pmlist_init(_tdm, pmlist, pmlist_size);
    for (i = 0; i < lr_buff_size; i++) {
        lr_buff[i] = param_token_empty;
    }
    for (i = 0; i < param_vmap_wid; i++) {
        for (j = 0; j < param_vmap_len; j++) {
            vmap[i][j] = param_token_empty;
        }
    }

    /* construct lr_buff */
    if (param_lr_en == BOOL_TRUE) {
        /* construct lr_buff */
        tdm_vmap_gen_lr_buff(param_lr_buff, TDM_AUX_SIZE,
                             lr_buff, lr_buff_size, param_token_empty);
        /* check all ports in lr_buff */
        if (tdm_vmap_chk_lr_ports(_tdm, lr_buff, lr_buff_size) != PASS) {
            result = FAIL;
        }
    }

    /* print/check pipe bandwidth */
    tdm_vmap_chk_pipe_bandwidth(_tdm, lr_buff, lr_buff_size);

    if (param_lr_en == BOOL_TRUE && result == PASS) {
        /* construct pmlist based on ports in lr_buff */
        if (tdm_vmap_op_pmlist_gen(_tdm, lr_buff, lr_buff_size, pmlist,
                                 pmlist_size) != PASS) {
            result = FAIL;
        }


        /* adjust pmlist for lr: convert special PM to standard PM */
        tdm_vmap_op_pmlist_adjust_lr(_tdm, pmlist, pmlist_size);

        /* sort pmlist by max_pm_port_speed from highest to lowest */
        tdm_vmap_op_pmlist_sort(_tdm, pmlist, pmlist_size);

        /* adjust pmlist for os: insert pm(s) for EMPTY slots (optional) */
        tdm_vmap_op_pmlist_adjust_os(_tdm, pmlist, pmlist_size);

        /* apply patch for SPECIAL CASE */
        tdm_vmap_patch_special_case(_tdm, pmlist, pmlist_size);

        /* allocate slots for each PM in pmlist */
        if (PASS != tdm_vmap_gen_vmap(_tdm, pmlist, pmlist_size, vmap)) {
            result = FAIL;
        }

        /* check and fix vmap sister port spacing violation */
        if (tdm_vmap_chk_vmap_sister(_tdm, vmap) != PASS) {
            tdm_vmap_rotate_vmap(_tdm, vmap);
        }

        /* check vmap singularity */
        if (tdm_vmap_chk_singularity(_tdm, vmap) != PASS) {
            result = FAIL;
        }

        /* print vmap */
        tdm_vmap_print_vmap(_tdm, vmap);
    }

    /* copy local vmap to core vmap */
    for (i = 0; i < param_vmap_wid; i++) {
        for (j = 0; j < param_vmap_len; j++) {
            param_vmap[i][j] = (vmap[i][j] == param_token_unused) ?
                               param_token_empty : vmap[i][j];
        }
    }

    /* free memory */
    TDM_FREE(pmlist);
    TDM_FREE(lr_buff);
    for (i = 0; i < param_vmap_wid; i++) {
        TDM_FREE(vmap[i]);
    }
    TDM_FREE(vmap);
    
    return result;
}


/**
@name: tdm_core_vmap_alloc
@param:
 */
int
tdm_core_vmap_alloc(tdm_mod_t *_tdm)
{
    int i, j, port_token;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_token_empty, param_lr_en, param_vmap_wid, param_vmap_len;
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
    if (param_lr_en == BOOL_TRUE) {
        if(tdm_vmap_alloc(_tdm) != PASS) {
            return (TDM_EXEC_CORE_SIZE+1);
        }
        for (j = 0; j < param_vmap_len && j < param_cal_len; j++) {
            port_token = param_token_empty;
            for (i = 0; i < param_vmap_wid; i++) {
                if (param_vmap[i][j] != param_token_empty) {
                    port_token = param_vmap[i][j];
                    break;
                }
            }
            param_cal_main[j] = port_token;
        }
    } else {
        for (j = 0; j < param_vmap_len && j < param_cal_len; j++) {
            param_cal_main[j] = param_token_empty;
        }
    }

    /* check/adjust sister port space */
    _tdm->_core_data.vars_pkg.refactor_done = BOOL_TRUE;
    if (tdm_vmap_chk_sister(_tdm) != PASS) {
        if (tdm_vmap_filter_sister(_tdm) != PASS) {
            _tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
        }
    }

    /* check/adjust same port space */
    if (tdm_vmap_chk_same(_tdm) != PASS) {
        if (tdm_vmap_filter_same(_tdm) != PASS) {
            _tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
        }
    }

    return PASS;
}


/**
@name: tdm_core_vmap_prealloc
@param:

Shim for TDM5
 */
int
tdm_core_vmap_prealloc(tdm_mod_t *_tdm)
{
    int i, n, port_phynum, port_existed, result=PASS;
    int spd_idx, prt_idx;
    int lr_cnt,
        lr_cnt_1g,  lr_cnt_10g, lr_cnt_20g,  lr_cnt_25g, 
        lr_cnt_40g, lr_cnt_50g, lr_cnt_100g, lr_cnt_120g;
    char lr_en,
         lr_en_1g,  lr_en_10g, lr_en_20g,  lr_en_25g, 
         lr_en_40g, lr_en_50g, lr_en_100g, lr_en_120g;

    enum port_state_e *param_states;
    m_tdm_pipe_info_t *pipe_info;

    /* set parameters */
    param_states = _tdm->_chip_data.soc_pkg.state;

    /* NG TDM: pipe_info */
    pipe_info = &(_tdm->_core_data.vars_pkg.pipe_info);
    for (i = 0; i < TDM_AUX_SIZE; i++) {
        port_phynum = _tdm->_core_data.vars_pkg.lr_buffer[i];
        if (tdm_cmn_chk_port_is_fp(_tdm, port_phynum)) {
            if (param_states[port_phynum-1] == PORT_STATE__LINERATE ||
                param_states[port_phynum-1] == PORT_STATE__LINERATE_HG) {
                spd_idx = tdm_cmn_get_port_speed_idx(_tdm, port_phynum);
                if (spd_idx < MAX_SPEED_TYPES) {
                    pipe_info->lr_en = 1;
                    pipe_info->num_lr_prt++;
                    pipe_info->lr_spd_en[spd_idx] = 1;
                    prt_idx = pipe_info->lr_prt_cnt[spd_idx];
                    if (prt_idx < TDM_AUX_SIZE) {
                        pipe_info->lr_prt[spd_idx][prt_idx] = port_phynum;
                        pipe_info->lr_prt_cnt[spd_idx]++;
                    } else {
                        result = FAIL;
                        TDM_ERROR4("%s, LR port %3d, prt_idx %0d, size %0d\n",
                                   "prt_idx overflow in vmap preallocation",
                                   port_phynum, prt_idx, TDM_AUX_SIZE);
                    }
                } else {
                        result = FAIL;
                        TDM_ERROR4("%s, LR port %3d, spd_idx %0d, size %0d\n",
                                   "spd_idx overflow in vmap preallocation",
                                   port_phynum, spd_idx, MAX_SPEED_TYPES);
                }
            } else {
                        result = FAIL;
                        TDM_ERROR3("%s, LR port %3d, state %0d\n",
                                   "Invalid port state in vmap preallocation",
                                   port_phynum, param_states[port_phynum-1]);
            }
        }
    }
    for (i = 0; i < TDM_AUX_SIZE; i++) {
        port_phynum = _tdm->_core_data.vars_pkg.os_buffer[i];
        if (tdm_cmn_chk_port_is_fp(_tdm, port_phynum)) {
            if (param_states[port_phynum-1] == PORT_STATE__OVERSUB ||
                param_states[port_phynum-1] == PORT_STATE__OVERSUB_HG) {
                spd_idx = tdm_cmn_get_port_speed_idx(_tdm, port_phynum);
                if (spd_idx < MAX_SPEED_TYPES) {
                    pipe_info->os_en = 1;
                    pipe_info->num_os_prt++;
                    pipe_info->os_spd_en[spd_idx] = 1;
                    prt_idx = pipe_info->os_prt_cnt[spd_idx];
                    if (prt_idx < TDM_AUX_SIZE) {
                        port_existed = BOOL_FALSE;
                        for (n = 0; n < prt_idx; n++) {
                            if (pipe_info->os_prt[spd_idx][n] == port_phynum) {
                                port_existed = BOOL_TRUE;
                                break;
                            }
                        }
                        if (port_existed == BOOL_FALSE) {
                            pipe_info->os_prt[spd_idx][prt_idx] = port_phynum;
                            pipe_info->os_prt_cnt[spd_idx]++;
                        }
                    } else {
                        result = FAIL;
                        TDM_ERROR4("%s, OS port %3d, prt_idx %0d, size %0d\n",
                                   "prt_idx overflow in vmap preallocation",
                                   port_phynum, prt_idx, TDM_AUX_SIZE);
                    }
                } else {
                        result = FAIL;
                        TDM_ERROR4("%s, OS port %3d, spd_idx %0d, size %0d\n",
                                   "spd_idx overflow in vmap preallocation",
                                   port_phynum, spd_idx, MAX_SPEED_TYPES);
                }
            } else {
                        result = FAIL;
                        TDM_ERROR3("%s, OS port %3d, state %0d\n",
                                   "Invalid port state in vmap preallocation",
                                   port_phynum, param_states[port_phynum-1]);
            }
        }
    }

    /* (legacy) shim layer for linerate ports */
    lr_en_1g    = pipe_info->lr_spd_en[TDM_SPEED_IDX_1G];
    lr_en_10g   = pipe_info->lr_spd_en[TDM_SPEED_IDX_10G];
    lr_en_20g   = pipe_info->lr_spd_en[TDM_SPEED_IDX_20G];
    lr_en_25g   = pipe_info->lr_spd_en[TDM_SPEED_IDX_25G];
    lr_en_40g   = pipe_info->lr_spd_en[TDM_SPEED_IDX_40G];
    lr_en_50g   = pipe_info->lr_spd_en[TDM_SPEED_IDX_50G];
    lr_en_100g  = pipe_info->lr_spd_en[TDM_SPEED_IDX_100G];
    lr_en_120g  = pipe_info->lr_spd_en[TDM_SPEED_IDX_120G];
    lr_en       = pipe_info->lr_en;
    lr_cnt_1g   = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_1G];
    lr_cnt_10g  = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_10G];
    lr_cnt_20g  = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_20G];
    lr_cnt_25g  = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_25G];
    lr_cnt_40g  = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_40G];
    lr_cnt_50g  = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_50G];
    lr_cnt_100g = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_100G];
    lr_cnt_120g = pipe_info->lr_prt_cnt[TDM_SPEED_IDX_120G];
    lr_cnt      = pipe_info->num_lr_prt;

    _tdm->_core_data.vars_pkg.lr_enable                   = lr_en;
    _tdm->_core_data.vars_pkg.lr_1                        = lr_en_1g;
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

    /* (legacy) shim layer for oversub ports */
    _tdm->_core_data.vars_pkg.os_1   = pipe_info->os_spd_en[TDM_SPEED_IDX_1G];
    _tdm->_core_data.vars_pkg.os_10  = pipe_info->os_spd_en[TDM_SPEED_IDX_10G];
    _tdm->_core_data.vars_pkg.os_20  = pipe_info->os_spd_en[TDM_SPEED_IDX_20G];
    _tdm->_core_data.vars_pkg.os_25  = pipe_info->os_spd_en[TDM_SPEED_IDX_25G];
    _tdm->_core_data.vars_pkg.os_40  = pipe_info->os_spd_en[TDM_SPEED_IDX_40G];
    _tdm->_core_data.vars_pkg.os_50  = pipe_info->os_spd_en[TDM_SPEED_IDX_50G];
    _tdm->_core_data.vars_pkg.os_100 = pipe_info->os_spd_en[TDM_SPEED_IDX_100G];
    _tdm->_core_data.vars_pkg.os_120 = pipe_info->os_spd_en[TDM_SPEED_IDX_120G];
    _tdm->_core_data.vars_pkg.os_enable = pipe_info->os_en;
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_1G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_10G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_20G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_25G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_40G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_50G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_100G];
    _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7 = pipe_info->os_prt_cnt[TDM_SPEED_IDX_120G];
    _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os = pipe_info->num_os_prt;
    for (i = 1; i < TDM_AUX_SIZE; i++) {
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[i]   = pipe_info->os_prt[TDM_SPEED_IDX_1G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[i]  = pipe_info->os_prt[TDM_SPEED_IDX_10G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[i]  = pipe_info->os_prt[TDM_SPEED_IDX_20G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[i]  = pipe_info->os_prt[TDM_SPEED_IDX_25G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[i]  = pipe_info->os_prt[TDM_SPEED_IDX_40G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[i]  = pipe_info->os_prt[TDM_SPEED_IDX_50G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[i] = pipe_info->os_prt[TDM_SPEED_IDX_100G][i-1];
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120[i] = pipe_info->os_prt[TDM_SPEED_IDX_120G][i-1];
    }

    return result;
}
