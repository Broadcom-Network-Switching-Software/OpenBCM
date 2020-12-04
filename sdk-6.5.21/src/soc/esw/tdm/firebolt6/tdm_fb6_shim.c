/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip to core API shim layer
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_fb6_shim_get_port_pm
@param:

Return PM number to which the input port belongs
 */
int
tdm_fb6_shim_get_port_pm(tdm_mod_t *_tdm)
{
    return (tdm_fb6_cmn_get_port_pm(_tdm->_core_data.vars_pkg.port, _tdm));
}


/**
@name: tdm_fb6_shim_get_pipe_ethernet
@param:

Returns BOOL_TRUE if pipe of the given port has traffic entirely Ethernet,
otherwise returns BOOL_FALSE.
 */
int
tdm_fb6_shim_get_pipe_ethernet(tdm_mod_t *_tdm)
{
    int result = BOOL_TRUE;

    if (_tdm->_core_data.vars_pkg.port<=64) {
        result = tdm_fb6_cmn_get_pipe_ethernet(0, _tdm);
    } else if (_tdm->_core_data.vars_pkg.port<=128){
        result = tdm_fb6_cmn_get_pipe_ethernet(1, _tdm);
    }

    return (result);
}


/**
@name: tdm_chip_th2_shim__core_vbs_scheduler_ovs
@param:

Passthru function for oversub scheduling request from TD2/TD2+
 */
int
tdm_fb6_shim_core_vbs_scheduler_ovs(tdm_mod_t *_tdm)
{
    if (_tdm->_chip_data.soc_pkg.flex_port_en) {
        /* Used for FlexPort */
        /* return tdm_fb6_vbs_scheduler_ovs_flex_port(_tdm); */
        /* return tdm_fb6_vbs_scheduler_ovs_flex_port_delta(_tdm) */;
        return tdm_core_vbs_scheduler_ovs(_tdm);
    } else {
        /* Used for initial config */
        return tdm_core_vbs_scheduler_ovs(_tdm);
    }
}


/**
@name: tdm_fb6_shim_core_vmap_prealloc
@param:

Shim for TDM5
 */
int
tdm_fb6_shim_core_vmap_prealloc(tdm_mod_t *_tdm)
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
    int phy_no = 0;
    m_tdm_pipe_info_t *pipe_info;
    int first_time = 1;

    /* set parameters */
    param_states = _tdm->_chip_data.soc_pkg.state;

    /* NG TDM: pipe_info */
    pipe_info = &(_tdm->_core_data.vars_pkg.pipe_info);
    for (i = 0; i < TDM_AUX_SIZE; i++) {
        port_phynum = _tdm->_core_data.vars_pkg.lr_buffer[i];
        /*if (tdm_fb6_cmn_chk_port_fp(_tdm, port_phynum)) {*/
        if ((port_phynum>=1 && port_phynum<=68) &&
               ((_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4731_1012) ||
                  (_tdm->_chip_data.soc_pkg.clk_freq== FB6_CLK_4732_1012))) {
            if (param_states[port_phynum-1] == PORT_STATE__LINERATE ||
                param_states[port_phynum-1] == PORT_STATE__LINERATE_HG) {
               if(first_time==1) {
                phy_no = (port_phynum-1)/4;
                spd_idx = TDM_SPEED_IDX_50G;
                pipe_info->lr_en = 1;
                pipe_info->lr_spd_en[spd_idx] = 1;
                prt_idx = pipe_info->lr_prt_cnt[spd_idx];
                pipe_info->lr_prt[spd_idx][prt_idx] = phy_no*4+1;
                pipe_info->lr_prt_cnt[spd_idx]++;
                pipe_info->num_lr_prt++;
                spd_idx = TDM_SPEED_IDX_10G;
                pipe_info->lr_en = 1;
                pipe_info->lr_spd_en[spd_idx] = 1;
                prt_idx = pipe_info->lr_prt_cnt[spd_idx];
                pipe_info->lr_prt[spd_idx][prt_idx] = phy_no*4+3;
                pipe_info->lr_prt_cnt[spd_idx]++;
                pipe_info->num_lr_prt++;
                /*_tdm->_chip_data.soc_pkg.speed[(phy_no*4)+1] = SPEED_50G;
                _tdm->_chip_data.soc_pkg.speed[(phy_no*4)+2] = SPEED_0;
                _tdm->_chip_data.soc_pkg.speed[(phy_no*4)+3] = SPEED_10G;
                _tdm->_chip_data.soc_pkg.speed[(phy_no*4)+4] = SPEED_0;
                _tdm->_chip_data.soc_pkg.state[(phy_no*4)+0] = PORT_STATE__LINERATE;
                _tdm->_chip_data.soc_pkg.state[(phy_no*4)+1] = PORT_STATE__LINERATE;
                _tdm->_chip_data.soc_pkg.state[(phy_no*4)+2] = PORT_STATE__LINERATE;
                _tdm->_chip_data.soc_pkg.state[(phy_no*4)+3] = PORT_STATE__LINERATE;*/


                TDM_PRINT1("tdm_fb6_shim():: NUM LR PORT: %d\n",  pipe_info->num_lr_prt);
                first_time = 0;
                }
                /*if (spd_idx < MAX_SPEED_TYPES) {
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
                }*/
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

