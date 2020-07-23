/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip based printing and parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_fb6_ovsb_get_pkt_shaper_cal
@param:
@desc: [chip_specific]

Return pionter of the packet shaper scheduler.
 */
int *
tdm_fb6_ovsb_get_pkt_shaper_cal(tdm_mod_t *_tdm, int hpipe_num)
{
    int * cal = NULL;
    int cal_offset;

    cal_offset = FB6_SHAPING_GRP_IDX_0 + hpipe_num;
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	cal = _tdm->_chip_data.cal_0.cal_grp[cal_offset]; break;
        case 1:	cal = _tdm->_chip_data.cal_1.cal_grp[cal_offset]; break;
        case 2:	cal = _tdm->_chip_data.cal_2.cal_grp[cal_offset]; break;
        case 3:	cal = _tdm->_chip_data.cal_3.cal_grp[cal_offset]; break;
        default:
            TDM_ERROR2("Invalid calendar ID %0d for %s\n",
                       _tdm->_core_data.vars_pkg.cal_id,
                       "tdm_fb6_ovsb_get_pkt_shaper_cal()");
            cal = NULL; break;
    }

    return cal;
}

/**
@name: tdm_fb6_ovsb_get_pkt_shaper_cal_prev
@param:
@desc: [chip_specific]

Return pionter of the previous packet shaper scheduler.
 */
int *
tdm_fb6_ovsb_get_pkt_shaper_cal_prev(tdm_mod_t *_tdm, int hpipe_num)
{
    int * cal = NULL;
    int cal_offset;

    cal_offset = FB6_SHAPING_GRP_IDX_0 + hpipe_num;
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	cal = _tdm->_prev_chip_data.cal_0.cal_grp[cal_offset]; break;
        case 1:	cal = _tdm->_prev_chip_data.cal_1.cal_grp[cal_offset]; break;
        case 2:	cal = _tdm->_prev_chip_data.cal_2.cal_grp[cal_offset]; break;
        case 3:	cal = _tdm->_prev_chip_data.cal_3.cal_grp[cal_offset]; break;
        default:
            TDM_ERROR2("Invalid calendar ID %0d for %s\n",
                       _tdm->_core_data.vars_pkg.cal_id,
                       "tdm_fb6_ovsb_get_pkt_shaper_cal_prev()");
            cal = NULL; break;
    }

    return cal;
}

/**
@name: tdm_fb6_ovsb_get_hpipe_grp_range
@param:

Return ovsb group range [grp_lo, grp_hi] for each halfpipe.
 */
void
tdm_fb6_ovsb_get_hpipe_grp_range(tdm_mod_t *_tdm, int hpipe, int *lo, int *hi)
{
    int grp_per_hpipe, grp_lo = 0, grp_hi = 0;
    tdm_calendar_t *cal;

    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal != NULL) {
        if (hpipe >= 0 && hpipe < FB6_NUM_HPIPE && cal->grp_num > 0) {
            grp_per_hpipe = cal->grp_num / FB6_NUM_HPIPE;
            grp_lo = (hpipe * grp_per_hpipe) % cal->grp_num;
            grp_hi = (grp_lo + grp_per_hpipe - 1) % cal->grp_num;
        }
    }
    (*lo) = grp_lo;
    (*hi) = grp_hi;

}


/**
@name: tdm_fb6_ovsb_get_grp_spd
@param:

Return group speed of the given ovsb group in ethernet speed format.
If ovsb group is empty, return 0.
 */
int
tdm_fb6_ovsb_get_grp_spd(tdm_mod_t *_tdm, int grp)
{
    int prt, pos;
    int param_token_empty;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return 0;

    if (grp >= 0 && grp < cal->grp_num) {
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) {
                return (tdm_fb6_cmn_get_port_speed_eth(_tdm, prt));
            }
        }
    }

    return 0;
}

/**
@name: tdm_fb6_ovsb_get_grp_prt_cnt
@param:

Return number of active ports in the given oversub speed group.
If ovsb group is empty, return 0.
 */
int
tdm_fb6_ovsb_get_grp_prt_cnt(tdm_mod_t *_tdm, int grp)
{
    int prt, pos, prt_cnt = 0;
    int param_token_empty;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return prt_cnt;

    if (grp >= 0 && grp < cal->grp_num) {
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) prt_cnt++;
        }
    }

    return prt_cnt;
}

/**
@name: tdm_fb6_ovsb_get_grp_empty_cnt
@param:

Return the number of empty slots in the given ovsb group.
 */
int
tdm_fb6_ovsb_get_grp_empty_cnt(tdm_mod_t *_tdm, int grp)
{
    tdm_calendar_t *cal;

    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return 0;

    return (cal->grp_len - tdm_fb6_ovsb_get_grp_prt_cnt(_tdm, grp));
}



/**
@name: tdm_fb6_ovsb_apply_constraints
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_fb6_ovsb_apply_constraints(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num, i;

    int speed_en_mtx[FB6_NUM_PHY_PM][6];
    int speed_en_mtx_reduced[6];
    /* <speed_indx> 0: 10G; 1: 20G; 2: 25G; 3: 40G; 4: 50G; 5: 100G */
    int no_of_speeds_in_pipe;
    int phy_base_port, phy_port;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    for (i=0; i<6; i++) {
        for (pm_num=0; pm_num<FB6_NUM_PHY_PM; pm_num++) {
            speed_en_mtx[pm_num][i]=0;
        }
        speed_en_mtx_reduced[i]=0;
    }

    /* Build speed_en_mtx matrix*/
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = -1;
        phy_base_port = FB6_NUM_PM_LNS*pm_num+1;
        for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = phy_base_port+ln_num;
            if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
            ((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
             (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
                /* Coding for pm_ovs_halfpipe:
                    -1 : not OVSB;
                    2 : OVSB but not assigned
                    0 : Half Pipe 0
                    1 : Half Pipe 1
                */
                _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 2; /* OVSB but not assigned */
                switch(_tdm->_chip_data.soc_pkg.speed[phy_port]) {
                    case SPEED_10G :
                        speed_en_mtx[pm_num][FB6_SPEED_IDX_10G] = 1; break;
                    case SPEED_20G :
                        speed_en_mtx[pm_num][FB6_SPEED_IDX_20G] = 1; break;
                    case SPEED_25G :
                        speed_en_mtx[pm_num][FB6_SPEED_IDX_25G] = 1; break;
                    case SPEED_40G :
                        speed_en_mtx[pm_num][FB6_SPEED_IDX_40G] = 1; break;
                    case SPEED_50G :
                        speed_en_mtx[pm_num][FB6_SPEED_IDX_50G] = 1; break;
                    case SPEED_100G :
                        speed_en_mtx[pm_num][FB6_SPEED_IDX_100G]= 1; break;
                    default : break;
                }
            }
        }
        for(i=0; i<6; i++) {
            speed_en_mtx_reduced[i] = (speed_en_mtx[pm_num][i]==1) ? 1 : speed_en_mtx_reduced[i];
        }
    }

    no_of_speeds_in_pipe=0;
    for (i=0; i<6; i++)  {
        if (speed_en_mtx_reduced[i]>0) {
            no_of_speeds_in_pipe++;
        }
    }

        if (no_of_speeds_in_pipe > 4) {
        /* Restriction 13:No port configurations with more than 4 port speed classes are supported. */
            TDM_ERROR2("tdm_fb6_ovsb_apply_constraints() PIPE %d No OVSB port configurations with more than 4 port speed classes are supported; no_of_speeds_in_pipe=%d\n", pipe_id, no_of_speeds_in_pipe);

        }
        else if (no_of_speeds_in_pipe == 4) {
        /*Restriction 14: The only supported port configurations with 4 port speed classes are:
            10G/20G/40G/100G
            10G/25G/50G/100G
        */
        if ( (speed_en_mtx_reduced[FB6_SPEED_IDX_20G] || speed_en_mtx_reduced[FB6_SPEED_IDX_40G]) && (speed_en_mtx_reduced[FB6_SPEED_IDX_25G] || speed_en_mtx_reduced[FB6_SPEED_IDX_50G]) ) {
        /* Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1*/
            TDM_PRINT1("tdm_fb6_ovsb_apply_constraints() PIPE %d applying Restriction 14 \n",pipe_id);
            for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
                if (speed_en_mtx[pm_num][FB6_SPEED_IDX_25G] || speed_en_mtx[pm_num][FB6_SPEED_IDX_50G]) {
                    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 0;
                } else if (speed_en_mtx[pm_num][FB6_SPEED_IDX_20G] || speed_en_mtx[pm_num][FB6_SPEED_IDX_40G]) {
                    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 1;
                }
            }
        }
    }
    else if (no_of_speeds_in_pipe > 1) {
    /*Restriction 15: All port configurations with 1-3 port speed classes are supported, except
        configurations that contain both 20G and 25G port speeds.*/
        if ( speed_en_mtx_reduced[FB6_SPEED_IDX_20G] && speed_en_mtx_reduced[FB6_SPEED_IDX_25G]) {
        /* Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1*/
            TDM_PRINT1("tdm_fb6_ovsb_apply_constraints() PIPE %d applying Restriction 15 \n",pipe_id);
            for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
                if (speed_en_mtx[pm_num][FB6_SPEED_IDX_25G]) { /*if (speed_en_mtx[pm_num][FB6_SPEED_IDX_25G] || speed_en_mtx[pm_num][FB6_SPEED_IDX_50G]) {*/
                    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 0;
                } else if (speed_en_mtx[pm_num][FB6_SPEED_IDX_20G]) { /*} else if (speed_en_mtx[pm_num][FB6_SPEED_IDX_20G] || speed_en_mtx[pm_num][FB6_SPEED_IDX_40G]) {*/
                    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 1;
                }
            }
        }
    }
    else {
        /* Do nothing; no constraints # of speeds is 0*/
    }

    return PASS;
}

/**
@name: tdm_fb6_ovsb_get_prt_hpipe
@param:

Return subpipe index of the given port.
    -1: unassigned
     n: subpipe index, 0 <= n < FB6_NUM_HPIPE
 */
int
tdm_fb6_ovsb_get_prt_hpipe(tdm_mod_t *_tdm, int port_token)
{
    int pm, hpipe = -1;

    pm = tdm_fb6_cmn_get_port_pm(port_token, _tdm);
    if (pm >= 0 && pm < FB6_NUM_PM_MOD) {
        hpipe = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm];
        /* if (spipe >= 0 && spipe < FB6_NUM_HPIPE) return spipe; */
    }

    return hpipe;
}


/**
@name: tdm_fb6_ovsb_get_prt_lane
@param:

Return relative lane index of the given port within PM.
 */
int
tdm_fb6_ovsb_get_prt_lane(tdm_mod_t *_tdm, int prt)
{
    if (prt >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        prt <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return ((prt - 1) % FB6_NUM_PM_LNS);
    }

    return 0;
}


/**
@name: tdm_fb6_ovsb_get_pm_shaper_prt_stride
@param:
@desc: [chip_specific]

Return incremental distance between two consecutive port slots in
PM shaper calendar.
 */
int
tdm_fb6_ovsb_get_pm_shaper_prt_stride(tdm_mod_t *_tdm, int prt)
{
    int spd, stride;

    spd = tdm_fb6_cmn_get_port_speed_eth(_tdm, prt);
    switch (spd) {
        case SPEED_10G:  stride = 8; break;
        case SPEED_20G:  stride = 4; break;
        case SPEED_25G:  stride = 4; break;
        case SPEED_40G:  stride = 2; break;
        case SPEED_50G:  stride = 2; break;
        case SPEED_100G: stride = 1; break;
        default:         stride = 0; break;
    }
    /* for any speed less than 10G, take it as 10G */
    if (spd > 0 && spd < SPEED_10G) stride = 8;

    return stride;
}

/**
@name: tdm_fb6_ovsb_get_pm_shaper_prt_offset
@param:
@desc: [chip_specific]

Return port offset within pseudo PM shaper calendar.
 */
int
tdm_fb6_ovsb_get_pm_shaper_prt_offset(tdm_mod_t *_tdm, int prt)
{
    int ln_base, ln_offset;

    ln_base = tdm_fb6_ovsb_get_prt_lane(_tdm, prt);
    switch (ln_base) {
        case 0: ln_offset = 0; break;
        case 1: ln_offset = 2; break;
        case 2: ln_offset = 1; break;
        case 3: ln_offset = 3; break;
        default:ln_offset = 0; break;
    }

    return ln_offset;
}



/**
@name: tdm_fb6_ovsb_get_pm_shaper_prt_pos
@param:
@desc: [chip_specific]

Return the n-th slot pos of the given port in pm shaper calendar.
 */
int
tdm_fb6_ovsb_get_pm_shaper_prt_pos(tdm_mod_t *_tdm, int prt, int slot_cnt)
{
    int pos, prt_offset, prt_stride, prt_spd, ln_base;

    prt_offset = tdm_fb6_ovsb_get_pm_shaper_prt_offset(_tdm, prt);
    prt_stride = tdm_fb6_ovsb_get_pm_shaper_prt_stride(_tdm, prt);

    prt_spd = tdm_fb6_cmn_get_port_speed_eth(_tdm, prt);
    ln_base = tdm_fb6_ovsb_get_prt_lane(_tdm, prt);
    /* [chip_specific] (pre-opt) special case: 10G */
    if (prt_spd == SPEED_10G) {
        switch (ln_base) {
            case 1: prt_offset += 2*prt_stride; break;
            case 2: prt_offset +=   prt_stride; break;
            default: break;
        }
    }
    /* [chip_specific] (pre-opt) special case: 20G */
    else if (prt_spd == SPEED_20G) {
        if (slot_cnt == 2 || slot_cnt == 3) prt_offset += 2;
    }
    /* [chip_specific] (pre-opt) special case: 40G */
    else if (prt_spd == SPEED_40G) {
        if (slot_cnt >= 4 && slot_cnt <= 7) prt_offset += 2;
    }

    /* calculate slot position */
    pos = (prt_offset + prt_stride * slot_cnt) % FB6_SHAPER_NUM_PM_SLOTS;

    return pos;
}

/**
@name: tdm_fb6_ovsb_get_pkt_shaper_prt_pm_idx
@param:

Find an existing or available pm_idx in pkt_shaper
 */
int
tdm_fb6_ovsb_get_pkt_shaper_prt_pm_idx(tdm_mod_t *_tdm, int prt)
{
    int i, j, max_start_indx, distance, max_distance;
    int prt_hpipe, prt_pm, pos, pm_idx;
    int *pkt_shaper_cal = NULL;
    int pm_indx_avail[FB6_NUM_PMS_PER_HPIPE];
    int param_cal_id, param_token_empty;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    prt_pm = tdm_fb6_cmn_get_port_pm(prt, _tdm);

    prt_hpipe = tdm_fb6_ovsb_get_prt_hpipe(_tdm, prt);
    if (!(prt_hpipe >= 0 && prt_hpipe < FB6_NUM_HPIPE)) return FAIL;

    pkt_shaper_cal = tdm_fb6_ovsb_get_pkt_shaper_cal(_tdm, prt_hpipe);
    if (pkt_shaper_cal == NULL) return FAIL;

    /* Case 1: there are sister ports of this port already in pkt scheduler,
       which means find that PM and place port in the corresponding lanes. */
    pm_idx = -1;
    for (i = 0; i < FB6_SHAPING_GRP_LEN; i++) {
        if (pkt_shaper_cal[i] != param_token_empty) {
            if (prt_pm == tdm_fb6_cmn_get_port_pm(pkt_shaper_cal[i], _tdm)) {
                /* Find that PM has already been placed */
                pm_idx = i % FB6_NUM_PMS_PER_HPIPE;
                break;
            }
        }
    }

    /* Case 2: there is no sister port of this port already in pkt scheduler,
       which means find an empty PM and place port in the corresponding lanes
       Find an available PM */
    if (pm_idx == -1) {
        for (i = 0; i < FB6_NUM_PMS_PER_HPIPE; i++) {
            pm_indx_avail[i] = 1;
            for (j = 0; j < FB6_NUM_PKT_SLOTS_PER_PM; j++) {
                pos = (j * FB6_NUM_PMS_PER_HPIPE + i) % FB6_SHAPING_GRP_LEN;
                if (pkt_shaper_cal[pos] != param_token_empty) {
                    pm_indx_avail[i] = 0;
                    break;
                }
            }
        }

        /* Find the biggest clump of 1's in pm_indx_avail array, and
           choose the pm_idx in the middle of the clump */
        max_start_indx = 0;
        distance       = 0;
        max_distance   = 0;
        for (i = 0; i < FB6_NUM_PMS_PER_HPIPE; i++) {
            if (pm_indx_avail[i] == 1) {
                distance = 0;
                for (j = 0; j < FB6_NUM_PMS_PER_HPIPE; j++){
                    if (pm_indx_avail[(i+j) % FB6_NUM_PMS_PER_HPIPE] == 1) {
                        distance++;
                    } else {
                        break;
                    }
                }
                if (distance > max_distance) {
                    max_start_indx = i;
                    max_distance   = distance;
                }
            }
        }
        /* If all available make it 0, else middle of the clump*/
        pm_idx = (max_distance == FB6_NUM_PMS_PER_HPIPE) ? 0 :
                 ((max_start_indx + FB6_NUM_PMS_PER_HPIPE) % FB6_NUM_PMS_PER_HPIPE);

        if (pm_indx_avail[pm_idx] == 0) {
            pm_idx = -1;
            TDM_ERROR4("%s, PIPE %d, pm %0d, port %0d\n",
                       "Failed to find an available PM",
                       param_cal_id, prt_pm, prt);
        }
    }

    if (pm_idx == -1) {
        TDM_ERROR4("%s, PIPE %d, pm %0d, phy_port %0d\n",
                   "Failed to find an available PM",
                   param_cal_id, prt_pm, prt);
    }

    return pm_idx;
}


/**
@name: tdm_fb6_ovsb_get_pkt_shaper_prt_pos
@param:
@desc: [chip_specific]

For the n-th slot in pm_shaper_calendar, calculate the corresponding slot
postion in packet_shaper_calendar.
 */
int
tdm_fb6_ovsb_get_pkt_shaper_prt_pos(tdm_mod_t *_tdm, int prt,
int pm_shaper_slot_idx)
{
    int pos = -1;
    int num_blocks, block_idx = 0, block_offset, lane, lane_offset, pm_offset;

    /* calc block_offset: one block is one minimum repetition pattern */
    num_blocks = FB6_SHAPING_GRP_LEN / FB6_NUM_PHY_PORTS_PER_HPIPE;
    if (num_blocks > 0) {
        block_idx = (pm_shaper_slot_idx / FB6_NUM_PM_LNS) % num_blocks;
    }
    block_offset = block_idx * FB6_NUM_PHY_PORTS_PER_HPIPE;

    /* calc lane_offset: one subblock contains slots with the same lane num */
    lane = pm_shaper_slot_idx % FB6_NUM_PM_LNS; /* lane num in pm_shaper_cal */
    switch (lane) {
        case 0:  lane_offset = 0;                       break; /* subport 0 */
        case 1:  lane_offset = 1*FB6_NUM_PMS_PER_HPIPE; break; /* subport 2 */
        case 2:  lane_offset = 2*FB6_NUM_PMS_PER_HPIPE; break; /* subport 1 */
        default: lane_offset = 3*FB6_NUM_PMS_PER_HPIPE; break; /* subport 3 */
    }

    /* calc pm_offset: within each subblock, slot is determined by pm_offset */
    pm_offset = tdm_fb6_ovsb_get_pkt_shaper_prt_pm_idx(_tdm, prt);
    if (pm_offset == -1) return pos;

    /* calculate slot position of pm shaper slot in packet shaper calendar */
    pos = (block_offset + lane_offset + pm_offset) % FB6_SHAPING_GRP_LEN;

    return pos;
}


/**
@name: tdm_fb6_ovsb_part_halfpipe
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_fb6_ovsb_part_halfpipe(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id, tmp;
    int pm_speed[FB6_NUM_PHY_PM/FB6_NUM_QUAD];
    int pm_num_subports[FB6_NUM_PHY_PM/FB6_NUM_QUAD];
    int pm_num_sort[FB6_NUM_PHY_PM/FB6_NUM_QUAD];
    int pm_num, ln_num, pm_indx, i, j;
    int phy_base_port, phy_port;
    int hp0_speed, hp1_speed;
    int no_pms_hp0, no_pms_hp1;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    TDM_PRINT2("tdm_fb6_avs_part_halfpipe1() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);

    pm_indx=0;
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        phy_base_port = FB6_NUM_PM_LNS*pm_num+1;
        pm_speed[pm_indx] = 0;
        pm_num_subports[pm_indx] = 0;
        for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = phy_base_port+ln_num;
            if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
            ((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
             (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
                TDM_PRINT4("tdm_fb6_avs_part_halfpipe2() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
                pm_speed[pm_indx] = pm_speed[pm_indx] + _tdm->_chip_data.soc_pkg.speed[phy_port]/1000;
                pm_num_subports[pm_indx]++;
            }
        }
        pm_num_sort[pm_indx] = pm_num;
        TDM_PRINT4("tdm_fb6_avs_part_halfpipe2() pm_indx=%d pm_num=%d pm_speed=%d pm_num_subports=%d\n",pm_indx, pm_num_sort[pm_indx], pm_speed[pm_indx]/1000, pm_num_subports[pm_indx]);
        pm_indx++;
    }

    /* Sort speeds of the PMs in descending order - bubble sort*/
    for (i=0; i<pms_per_pipe-1; i++) {
        for (j=pms_per_pipe-1; j>i; j--) {
            /* swap j with j-1*/
            if ((pm_speed[j] > pm_speed[j-1]) || ((pm_speed[j] == pm_speed[j-1]) &&
                (pm_num_subports[j] > pm_num_subports[j-1]))) {
                tmp = pm_num_sort[j];
                pm_num_sort[j] = pm_num_sort[j-1];
                pm_num_sort[j-1] = tmp;
                tmp = pm_speed[j];
                pm_speed[j] = pm_speed[j-1];
                pm_speed[j-1] = tmp;
                tmp = pm_num_subports[j];
                pm_num_subports[j] = pm_num_subports[j-1];
                pm_num_subports[j-1] = tmp;
            }
        }
    }

    hp0_speed=0;
    hp1_speed=0;
    no_pms_hp0=0;
    no_pms_hp1=0;
    /* Compute HP 0 & 1 BW based on already allocated PMs in constraints*/
    for (i=0; i<pms_per_pipe; i++) {
        pm_num = pm_num_sort[i];
        TDM_PRINT1("OVSB: pm_num %d\n", pm_num);
        if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] == 0) {
            hp0_speed += pm_speed[i];
            no_pms_hp0++;
        TDM_PRINT2("OVSB: hp0_speed %d\n no_pms_hp0 %d \n", hp0_speed, no_pms_hp0);
        }
        if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] == 1) {
            hp1_speed += pm_speed[i];
            no_pms_hp1++;
        TDM_PRINT2("OVSB: hp1_speed %d no_pms_hp1 %d\n", hp1_speed, no_pms_hp1);
        }
    }

    /* Do partition of the PMs into two half pipes */
    for (i=0; i<pms_per_pipe; i++) {
        pm_num = pm_num_sort[i];
        if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] == 2) { /* is OVSB unallocated PM */
            if (((no_pms_hp1 < (pms_per_pipe/2)) && (hp0_speed > hp1_speed)) ||
                (no_pms_hp0 >= (pms_per_pipe/2)) ) {
                hp1_speed = hp1_speed + pm_speed[i];
                _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 1;
                no_pms_hp1++;
            } else {
                hp0_speed = hp0_speed + pm_speed[i];
                _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] = 0;
                no_pms_hp0++;
            }
        }
    }
    TDM_PRINT2("tdm_fb6_avs_part_halfpipe3_1() hp0_bw=%d hp1_bw=%d\n", hp0_speed, hp1_speed);

    return PASS;
}


/**
@name: tdm_fb6_ovsb_fill_group
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_fb6_ovsb_fill_group(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id, tmp;
    int pm_num_subports[FB6_NUM_PHY_PM/FB6_NUM_QUAD];
    int pm_num_sort[FB6_NUM_PHY_PM/FB6_NUM_QUAD];
    int pm_num, ln_num, pm_indx, i, j;

    int pms_with_grp_speed;
    int grp_speed;
    int half_pipe_num;
    int speed_max_num_ports_per_pm;
    int min_num_ovs_groups;
    tdm_calendar_t *cal;
    int start_ovs_group;

    int start_group;
    int grp_index[FB6_OS_VBS_GRP_NUM];
    int phy_base_port;
    int phy_port;

    int t_ovs_grp_num;

    pipe_id       = _tdm->_core_data.vars_pkg.cal_id;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.fb6.half_pipe_num;
    grp_speed     = _tdm->_chip_data.soc_pkg.soc_vars.fb6.grp_speed;
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    /* Count the number of subports per PM with grp_speed for PMs that belongs to this half pipe*/
    pm_indx=0;
    pms_with_grp_speed = 0;
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        phy_base_port = FB6_NUM_PM_LNS*pm_num+1;
        pm_num_subports[pm_indx] = 0;
        if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] == half_pipe_num){
            for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
                phy_port = phy_base_port+ln_num;
                if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
                    pm_num_subports[pm_indx]++;
                }
            }
            if (pm_num_subports[pm_indx] > 0) {pms_with_grp_speed++;}
        }
        pm_num_sort[pm_indx] = pm_num;
        pm_indx++;
    }

    /* Compute  the minimum number of oversub groups required for the group speed*/
    switch (grp_speed) {
        case SPEED_10G:	speed_max_num_ports_per_pm=4; break;
        case SPEED_20G:	speed_max_num_ports_per_pm=2; break;
        case SPEED_40G:	speed_max_num_ports_per_pm=2; break;
        case SPEED_25G:	speed_max_num_ports_per_pm=4; break;
        case SPEED_50G:	speed_max_num_ports_per_pm=2; break;
        case SPEED_100G: speed_max_num_ports_per_pm=1; break;
        default:
            TDM_PRINT1("tdm_fb6_ovsb_fill_group3() Invalid group speed %0d\n",grp_speed);
            return FAIL;
    }
    /* ceil [(speed_max_num_ports_per_pm*pms_with_grp_speed) / FB6_OS_VBS_GRP_LEN ]*/
    min_num_ovs_groups = ((speed_max_num_ports_per_pm*pms_with_grp_speed) + FB6_OS_VBS_GRP_LEN-1) / FB6_OS_VBS_GRP_LEN;

    /* Execute only if ports with this speed exist*/
    if (min_num_ovs_groups > 0) {
        /* Sort PMs in descending order of their number of subports with grp_speed*/
        for (i=0; i<pms_per_pipe-1; i++) {
            for (j=pms_per_pipe-1; j>i; j--) {
                /* swap j with j-1*/
                if ( pm_num_subports[j] > pm_num_subports[j-1]) {
                    tmp = pm_num_sort[j];
                    pm_num_sort[j] = pm_num_sort[j-1];
                    pm_num_sort[j-1] = tmp;
                    tmp = pm_num_subports[j];
                    pm_num_subports[j] = pm_num_subports[j-1];
                    pm_num_subports[j-1] = tmp;
                }
            }
        }

        /* Find the first empty ovs group*/
        switch (_tdm->_core_data.vars_pkg.cal_id) {
            case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
            case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
            case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
            case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
            case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
            case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
            case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
            case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
            default:
                TDM_PRINT1("tdm_fb6_ovsb_fill_group()Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
                return FAIL;
        }
        start_ovs_group = half_pipe_num*(FB6_OS_VBS_GRP_NUM/2);
        start_group = start_ovs_group;
        for (i=start_group; i< (start_group+(FB6_OS_VBS_GRP_NUM/2)); i++) {
            if (cal->cal_grp[i][0] != FB6_NUM_EXT_PORTS) {
                start_ovs_group++;
            } else {
                break;
            }
        }

        /* Fill ovs groups with ports having grp_speed*/
        for (i=0; i< FB6_OS_VBS_GRP_NUM; i++) { grp_index[i] = 0;}
        for (i=0; i<pms_with_grp_speed; i++) {
            pm_num = pm_num_sort[i];
            phy_base_port = FB6_NUM_PM_LNS*pm_num+1;
            if (_tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num] == half_pipe_num){
                for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
                    phy_port = phy_base_port+ln_num;
                    if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
                        t_ovs_grp_num = start_ovs_group + (i % min_num_ovs_groups);
                        cal->cal_grp[t_ovs_grp_num][grp_index[t_ovs_grp_num]] = phy_port;
                        grp_index[t_ovs_grp_num]++;
                    }
                }
            }
        }

    }

    return PASS;
}

/**
@name: tdm_fb6_ovsb_gen_pm2hpipe_map_static
@param:
@desc: [chip_specific]

Partition each PM into a subpipe.
     -- pipe 0,  pm[0 - 7], port[1 -  32]: hpipe 0
     -- pipe 0,  pm[8 -15], port[33-  64]: hpipe 1
     -- pipe 1,  pm[16-23], port[65-  96]: hpipe 0
     -- pipe 1,  pm[24-31], port[97- 128]: hpipe 1
 */
int
tdm_fb6_ovsb_gen_pm2hpipe_map_static(tdm_mod_t *_tdm)
{
    int pm;
    int *pm2hpipe_map = NULL;
    int half_pipe_map_en[17];

    /* init variables */
    pm2hpipe_map = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe;
    for (pm = 0; pm < FB6_NUM_PM_MOD; pm++) {
        pm2hpipe_map[pm] = -1; /* -1:unassigned, 0:hp0, 1:hp1 */
    }

     tdm_fb6_cmn_get_half_pipe_map_en(_tdm, half_pipe_map_en);
    for (pm = 0; pm < FB6_NUM_PM_MOD; pm++) {
      pm2hpipe_map[pm] = !half_pipe_map_en[pm];
      TDM_PRINT2("TDM: hpipe map PM %2d pm2pipe_map %0d\n",
                 pm, pm2hpipe_map[pm]);
    }

    TDM_SML_BAR

    return PASS;
}



/**
@name: tdm_fb6_ovsb_gen_pkt_shaper_init
@param:

Populates Pkt shaper calendar
 */
int
tdm_fb6_ovsb_gen_pkt_shaper_init(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num;
    int phy_base_port, phy_port;
    int pm_s, pm_e;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    pm_s = pipe_id*pms_per_pipe;
    pm_e = (pipe_id+1)*pms_per_pipe;

    for (pm_num=pm_s; pm_num<pm_e; pm_num++) {
        phy_base_port = FB6_NUM_PM_LNS*pm_num+1;
        for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = phy_base_port+ln_num;
            TDM_PRINT1("ovsb pkt shaper phy_port %d\n", phy_port);
            if (phy_port<=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
                if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
                   ((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
                    (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
                    TDM_PRINT4("tdm_fb6_ovsb_pkt_shaper2() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",
                               pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]);
                    _tdm->_core_data.vars_pkg.port = phy_port;
                    tdm_fb6_ovsb_pkt_shaper_per_port(_tdm);
                }
            }
        }
    }

    return PASS;
}


/**
@name: tdm_fb6_ovsb_add_prt_to_grp
@param:

Add port into given ovsb speed group.
If success, return pos of port in given speed group, otherwise return -1.
 */
int
tdm_fb6_ovsb_add_prt_to_grp(tdm_mod_t *_tdm, int port_token, int grp)
{
    int pos;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    /* get pipe calendar */
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return (-1);

    if (grp >= 0 && grp < cal->grp_num) {
        for (pos = 0; pos < cal->grp_len; pos++) {
            if (cal->cal_grp[grp][pos] == param_token_empty) {
                cal->cal_grp[grp][pos] = port_token;
                return pos;
            }
        }
    }

    return (-1);
}


/**
@name: tdm_fb6_ovsb_pkt_shaper_per_port
@param:

Populates Pkt shaper calendar - per port
 */
int
tdm_fb6_ovsb_pkt_shaper_per_port(tdm_mod_t *_tdm)
{
    int pms_per_pipe;
    int pm_num, i;
    int phy_port;
    int half_pipe_num;

    int pkt_shpr_pm_indx;
    int no_of_lanes;
    int num_slots_for_port;
    int total_num_slots;
    int port_slot_tbl[FB6_SHAPING_GRP_LEN/8];
    int port_slot_tbl_shift[FB6_SHAPING_GRP_LEN/8];
    int slot_tbl_shift;
    int subport_no;
    int is_20G;
    int *pkt_shed_cal;
    int pkt_sched_repetitions;
    int base_offset;
    int max_pms_per_halfpipe;

    /*pkt_shed_cal = (int*) malloc(320 * sizeof(int));*/
    phy_port = _tdm->_core_data.vars_pkg.port;
    pm_num = (phy_port-1)/FB6_NUM_PM_LNS;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num];

    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;
    max_pms_per_halfpipe = pms_per_pipe/2;

    pkt_sched_repetitions = FB6_SHAPING_GRP_LEN/(pms_per_pipe/2)/FB6_NUM_PM_LNS; /* TH2: 5 = 160/8/4 */

    /*TDM_PRINT2("tdm_fb6_ovsb_pkt_shaper() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/

    /* Determine the pm_indx in pkt shceduler calendar - 0 to 7*/
    pkt_shpr_pm_indx = tdm_fb6_ovsb_pkt_shaper_find_pm_indx(_tdm);

    /* Find the number of lanes allocated to the port */
    is_20G=0;
    switch (_tdm->_chip_data.soc_pkg.speed[phy_port]) {
        case SPEED_10G:	no_of_lanes=1; break;
        case SPEED_20G:	no_of_lanes=2; is_20G = 1; break;
        case SPEED_40G:
            if ((_tdm->_chip_data.soc_pkg.speed[FB6_NUM_PM_LNS*pm_num+1+1] == SPEED_0) &&
                (_tdm->_chip_data.soc_pkg.speed[FB6_NUM_PM_LNS*pm_num+1+2] == SPEED_0) &&
                (_tdm->_chip_data.soc_pkg.speed[FB6_NUM_PM_LNS*pm_num+1+3] == SPEED_0) ) { /* SINGLE */
                no_of_lanes=4;
            } else { /* DUAL */
                no_of_lanes=2;
            }
            break;
        case SPEED_25G:	no_of_lanes=1; break;
        case SPEED_50G:	no_of_lanes=2; break;
        case SPEED_100G: no_of_lanes=4; break;
        default:
            TDM_PRINT1("tdm_fb6_ovsb_fill_group3() Invalid group speed %0d\n",_tdm->_chip_data.soc_pkg.speed[phy_port]);
            return FAIL;
    }

    /* Compute the number of slots in pkt calendar for this port*/
    num_slots_for_port = _tdm->_chip_data.soc_pkg.speed[phy_port]/5000; /* each slots is 5G */

    /* Compute the total number of slots consumed by the lanes*/
    total_num_slots = 5*no_of_lanes;

    /* First, place num_slots_for_port slots in a table with total_num_slots equally spaced */
    for (i=0; i<FB6_SHAPING_GRP_LEN/max_pms_per_halfpipe; i++) {
        port_slot_tbl[i] = FB6_NUM_EXT_PORTS;
        port_slot_tbl_shift[i]= FB6_NUM_EXT_PORTS;
    }
    for (i=0; i<num_slots_for_port; i++) {
        port_slot_tbl[(i*total_num_slots)/num_slots_for_port] = phy_port;
    }


    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("tdm_fb6_ovsb_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
    }

    /*  In order to spread tokens as uniformly as possible shift slot_tbl based on subport number */
    subport_no = (phy_port-1)%FB6_NUM_PM_LNS;
    slot_tbl_shift = 0;
    if (no_of_lanes==1) {
        switch (subport_no) {
            case 0 : slot_tbl_shift=0; break;
            case 1 : slot_tbl_shift=2; break;
            case 2 : slot_tbl_shift=1; break;
            case 3 : slot_tbl_shift=3; break;
            default : slot_tbl_shift=0; break;
        }
    }
    if (no_of_lanes==2) {
        switch (subport_no) {
            case 0 : slot_tbl_shift=0; break;
            case 2 : slot_tbl_shift= (is_20G==1) ? 3 : 2; break;
            default : slot_tbl_shift=0; break;
        }
    }

    for (i=0; i<total_num_slots; i++) {
        port_slot_tbl_shift[(i+slot_tbl_shift) % total_num_slots] = port_slot_tbl[i];
    }


    /* Populate pkt scheduler calendar */
    base_offset = pkt_shpr_pm_indx % pkt_sched_repetitions;
    for (i=0; i<total_num_slots; i++) {
        int cal_pos;
        int base_pos;
        int lane, lane_pos;

        lane = subport_no + (i%no_of_lanes);
        base_pos = 32*( (base_offset + (i/no_of_lanes)) % pkt_sched_repetitions);
        if (no_of_lanes==4) { /* SINGLE port mode */
            lane_pos = max_pms_per_halfpipe*lane;
        } else { /*  DUAL, and QUAD port modes */
            switch (lane) {
                case 0 : lane_pos = 0;                      break;
                case 1 : lane_pos = 2*max_pms_per_halfpipe; break;
                case 2 : lane_pos =   max_pms_per_halfpipe; break;
                case 3 : lane_pos = 3*max_pms_per_halfpipe; break;
                default:
                    TDM_PRINT1("tdm_fb6_ovsb_pkt_shaper_per_port() phy_port lane for phy_port=%d\n",phy_port);
                    return FAIL;
            }
        }

        cal_pos = base_pos + lane_pos + pkt_shpr_pm_indx;
        pkt_shed_cal[cal_pos] = port_slot_tbl_shift[i];
        TDM_PRINT1("PKT SCHED CAL() phy_port lane for phy_port=%d\n",phy_port);
    }

    return PASS;
}

/**
@name: tdm_fb6_ovsb_migrate_prt
@param:

Migrate all allocated same-speed sister ports of the given port from
current ovsb grp to an appropriate grp.
 */
int
tdm_fb6_ovsb_migrate_prt(tdm_mod_t *_tdm, int port_token, int slot_req)
{
    int prt_buff[FB6_NUM_PM_LNS];
    int port_pm, port_spd;
    int prt, prt_hp, prt_cnt, prt_done;
    int grp, grp_lo, grp_hi, dst_grp, dst_pos, prev_grp, prev_pos;
    int pos;
    tdm_calendar_t *cal = NULL, *cal_prev = NULL;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    for (prt_cnt = 0; prt_cnt < FB6_NUM_PM_LNS; prt_cnt++) {
        prt_buff[prt_cnt] = param_token_empty;
    }

    /* get pipe calendar */
    cal      = tdm_fb6_cmn_get_pipe_cal(_tdm);
    cal_prev = tdm_fb6_cmn_get_pipe_cal_prev(_tdm);
    if (cal == NULL || cal_prev == NULL) return (-1);

    /* case 1: select group containing sister ports with the same speed */
    port_pm  = tdm_fb6_cmn_get_port_pm(port_token, _tdm);
    port_spd = tdm_fb6_cmn_get_port_speed_eth(_tdm, port_token);
    prt_hp   = tdm_fb6_ovsb_get_prt_hpipe(_tdm, port_token);
    tdm_fb6_ovsb_get_hpipe_grp_range(_tdm, prt_hp, &grp_lo, &grp_hi);

    /* move same-speed sister ports to prt_buff[] */
    prt_cnt = 0;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        if (tdm_fb6_ovsb_get_grp_spd(_tdm, grp) != port_spd) continue;
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) {
                if (tdm_fb6_cmn_get_port_pm(prt, _tdm) == port_pm) {
                    if (prt_cnt < FB6_NUM_PM_LNS) {
                        prt_buff[prt_cnt] = prt;
                        cal->cal_grp[grp][pos] = param_token_empty;
                        prt_cnt++;
                        TDM_PRINT3("%s, prt %0d, src_grp %0d\n",
                                   "TDM: [a] move port from group to buffer",
                                   prt, grp);
                    }
                }
            }
        }
    }

    /* select a group which can accomodate all same-speed sister ports */
    dst_grp = tdm_fb6_ovsb_sel_grp_4_prt(_tdm, port_token);
    if (dst_grp == -1) return (-1);

    /* move all ports in prt_buff[] to the selected group */
    if (dst_grp >= grp_lo && dst_grp <= grp_hi ) {
        for (prt_cnt = 0; prt_cnt < FB6_NUM_PM_LNS; prt_cnt++) {
            prt      = prt_buff[prt_cnt];
            prev_grp = -1;
            prev_pos = -1;
            dst_pos  = -1;
            if (prt == param_token_empty) continue;
            /* determine port's grp and pos before flexing */
            prt_done = BOOL_FALSE;
            for (grp = grp_lo; grp <= grp_hi; grp++) {
                for (pos = 0; pos < cal_prev->grp_len; pos++) {
                    if (cal_prev->cal_grp[grp][pos] == prt) {
                        prev_grp = grp;
                        prev_pos = pos;
                        prt_done = BOOL_TRUE;
                        break;
                    }
                }
                if (prt_done == BOOL_TRUE) break;
            }
            /* determine port's pos in dst_grp in current calendar */
            prt_done = BOOL_FALSE;
            if (dst_grp == prev_grp &&
                prev_pos >= 0 && prev_pos < cal->grp_len) {
                if (cal->cal_grp[dst_grp][prev_pos] == param_token_empty) {
                    dst_pos = prev_pos;
                    prt_done = BOOL_TRUE;
                } else {
                    TDM_WARN4("%s, prt %0d, grp_prev %0d, pos_prev %0d\n",
                              "OVSB speed group violation on flex",
                              prt, prev_grp, prev_pos);
                }
            }
            if (prt_done  == BOOL_FALSE) {
                for (pos = 0; pos < cal->grp_len; pos++) {
                    if (cal->cal_grp[dst_grp][pos] == param_token_empty) {
                        dst_pos = pos;
                        prt_done = BOOL_TRUE;
                        break;
                    }
                }
            }
            /* migrate port to cal_grp[dst_grp][dst_pos] */
            if (prt_done == BOOL_TRUE &&
                dst_pos >= 0 && dst_pos < cal->grp_len) {
                cal->cal_grp[dst_grp][dst_pos] = prt;
                TDM_PRINT4("%s, prt %0d, dst_grp %0d, dst_pos %0d\n",
                           "TDM: [b] move port from buffer to group",
                           prt, dst_grp, dst_pos);
            }
        }
    }

    return dst_grp;
}


/**
@name: tdm_fb6_ovsb_sel_grp_4_prt
@param:

Select an ovsb group to accommodate given port.
Return group index if found, otherwise return -1.
 */
int
tdm_fb6_ovsb_sel_grp_4_prt(tdm_mod_t *_tdm, int port_token)
{
    int port_pm, port_spd;
    int prt, prt_pm, prt_spd, prt_hp, prt_done, prt_lo = 0, prt_hi = 0;
    int pos, grp, grp_lo, grp_hi, grp_num, grp_spd;
    int slot_req, slot_avail, slot_alloc;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    /* get pipe calendar */
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return (-1);

    /* get range of ovsb groups for the given port */
    prt_hp  = tdm_fb6_ovsb_get_prt_hpipe(_tdm, port_token);
    tdm_fb6_ovsb_get_hpipe_grp_range(_tdm, prt_hp, &grp_lo, &grp_hi);
   TDM_PRINT2("OVSB grp range grp_lo %d grp_hi %d ", grp_lo, grp_hi);

    /* count number of ports with the same speed of given port within
       the same PM */
    slot_req = 0;
    port_pm  = tdm_fb6_cmn_get_port_pm(port_token, _tdm);
    port_spd = tdm_fb6_cmn_get_port_speed_eth(_tdm, port_token);
    tdm_fb6_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if ((tdm_fb6_cmn_get_port_speed(prt, _tdm) > 0) &&
            (BOOL_TRUE == tdm_fb6_cmn_chk_port_is_os(_tdm, prt))) {
            prt_pm = tdm_fb6_cmn_get_port_pm(prt, _tdm);
            if (prt_pm == port_pm) {
                prt_spd = tdm_fb6_cmn_get_port_speed_eth(_tdm, prt);
                if (prt_spd == port_spd) slot_req++;
            }
        }
    }
   TDM_PRINT1("OVSB slot_req %d ", slot_req);
    if (!(slot_req > 0)) return (-1);

    /* case 1: select group containing sister ports with the same speed */
    grp_num  = -1;
    prt_done = BOOL_FALSE;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        slot_alloc = 0;
        grp_spd = tdm_fb6_ovsb_get_grp_spd(_tdm, grp);
        if (grp_spd != port_spd) continue;
        /* check if current group contains sister ports */
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) {
                prt_pm = tdm_fb6_cmn_get_port_pm( prt, _tdm);
                if (prt_pm == port_pm) {
                    grp_num  = grp;
                    prt_done = BOOL_TRUE;
                    slot_alloc++;
                    if (prt == port_token) {
                        grp_num = -1;
                        break;
                    }
                }
            }
        }
        /* check available resource in selected group */
        if (prt_done == BOOL_TRUE) {
            if (grp_num != -1) {
                slot_avail = tdm_fb6_ovsb_get_grp_empty_cnt(_tdm, grp_num);
                if (!(slot_avail >= (slot_req - slot_alloc))) {
                    grp_num = tdm_fb6_ovsb_migrate_prt(_tdm, port_token,
                                                       slot_req);
                }
            }
            break;
        }
    }
    if (prt_done == BOOL_TRUE) return grp_num;

   TDM_PRINT1("OVSB debug1 grp_num %d ", grp_num);
    /* case 2: select group containing same speed ports */
    grp_num = -1;
    prt_done = BOOL_FALSE;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        /* find same speed group, and check available resource */
        grp_spd = tdm_fb6_ovsb_get_grp_spd(_tdm, grp);
        if (grp_spd == port_spd) {
            slot_avail = tdm_fb6_ovsb_get_grp_empty_cnt(_tdm, grp);
            if (slot_avail >= slot_req) {
                grp_num  = grp;
                prt_done = BOOL_TRUE;
                break;
            }
        }
    }
    if (prt_done == BOOL_TRUE) return grp_num;

   TDM_PRINT1("OVSB debug2 grp_num %d ", grp_num);
    /* case 3: select group containing no ports (empty group) */
    grp_num  = -1;
    prt_done = BOOL_FALSE;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        slot_avail = tdm_fb6_ovsb_get_grp_empty_cnt(_tdm, grp);
        if (slot_avail == cal->grp_len) {
            grp_num  = grp;
            prt_done = BOOL_TRUE;
            break;
        }
    }
    if (prt_done == BOOL_TRUE) return grp_num;

   TDM_PRINT1("OVSB debug3 grp_num %d ", grp_num);
    return (-1);
}


/**
@name: tdm_fb6_ovsb_gen_pm2hpipe_map
@param:
    -- mapping_mode: 0->static, 1->dynamic.
 */
int
tdm_fb6_ovsb_gen_pm2hpipe_map(tdm_mod_t *_tdm, int mapping_mode)
{
        return (tdm_fb6_ovsb_gen_pm2hpipe_map_static(_tdm));
}


/**
@name: tdm_fb6_ovsb_pkt_shaper_per_pm
@param:

Populates Pkt shaper calendar - per PM
 */
int
tdm_fb6_ovsb_pkt_shaper_per_pm(tdm_mod_t *_tdm, int shift_step)
{
    int pms_per_pipe;
    int pm_num, i;
    int port_phy;
    int half_pipe_num;

    int pkt_shpr_pm_indx;
    int port_slot_tbl[FB6_SHAPING_GRP_LEN/8];
    int *pkt_shed_cal;
    int pkt_sched_repetitions;

    int pm_port_lo, block_num, lane_offset, pm_offset, block_offset;
    int lane, cal_pos, base_offset;
    enum port_speed_e *param_speeds;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;

    /*pkt_shed_cal = (int*) malloc(320 * sizeof(int));*/
    port_phy = _tdm->_core_data.vars_pkg.port;
    pm_num = (port_phy-1)/FB6_NUM_PM_LNS;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num];

    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    pkt_sched_repetitions = FB6_SHAPING_GRP_LEN/(pms_per_pipe/2)/FB6_NUM_PM_LNS; /* TH2: 5 = 160/8/4 */

    for (i=0; i<FB6_SHAPING_GRP_LEN/8; i++) {
        port_slot_tbl[i] = FB6_NUM_EXT_PORTS;
    }

    /* Determine the pm_indx in pkt shceduler calendar - 0 to 7*/
    pkt_shpr_pm_indx = tdm_fb6_ovsb_pkt_shaper_find_pm_indx(_tdm);

    /* allocate slots for PM */
    pm_port_lo = FB6_NUM_PM_LNS * pm_num + 1;
    switch (param_speeds[port_phy]) {
        case SPEED_10G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 8) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
            } else {
                /* QUAD */
                base_offset = (shift_step % 5) * 4;
                /* base_offset = 0; */
                port_slot_tbl[(base_offset + 0 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 8 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                port_slot_tbl[(base_offset + 18) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                port_slot_tbl[(base_offset + 5 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 13) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 3 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
                port_slot_tbl[(base_offset + 15) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
            }
            break;
        case SPEED_20G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 4 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 14) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
            } else {
                /* DUAL */
                port_slot_tbl[(0 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(4 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(10) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(14) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(1 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(5 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(11) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(15) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
            }
            break;
        case SPEED_40G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 8 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 18) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 5 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 13) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 3 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 15) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
            } else {
                /* DUAL */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 2 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 4) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 6) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 12) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 14) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 16) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 1 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 3 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 5 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 7 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 11) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 13) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 15) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 17) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
            }
            break;
        case SPEED_25G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_25G) &&
                (param_speeds[pm_port_lo + 2] == SPEED_50G) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                  /* TRI MODE - 1:1:2 -> Port 1,2 - 25G Port 3 - 40G or 50G */
                for (i = 0; i < 10; i++) {
                    if(i%2==0) {
                      port_slot_tbl[(i * 2 ) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo ;
                    } else {
                      port_slot_tbl[(i * 2) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                    }
                    port_slot_tbl[(i * 2+1)     % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                }
            } else {
            /* QUAD */
              for (i = 0; i < pkt_sched_repetitions; i++) {
                  port_slot_tbl[(i * 4 + 0) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                  port_slot_tbl[(i * 4 + 2) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                  port_slot_tbl[(i * 4 + 1) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                  port_slot_tbl[(i * 4 + 3) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
              }
            }
            break;
        case SPEED_50G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                for (i = 0; i < 10; i++) {
                    port_slot_tbl[(base_offset + i*2) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                }
            } else if ( (param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_25G) &&
                (param_speeds[pm_port_lo + 3] == SPEED_25G) ) {
                  /* TRI MODE - 2:1:1 -> Port 1 - 50G and Port 3, Port 4 > 0G */
                for (i = 0; i < 10; i++) {
                    port_slot_tbl[(i * 2)     % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                    if(i%2==0) {
                      port_slot_tbl[(i * 2 + 1) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                    } else {
                      port_slot_tbl[(i * 2 + 1) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
                    }
                }
            } else {
                /* DUAL */
                for (i = 0; i < 10; i++) {
                    port_slot_tbl[(i * 2)     % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo;
                    port_slot_tbl[(i * 2 + 1) % (FB6_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                }
            }
            break;
        case SPEED_100G:
            for (i = 0; i < FB6_SHAPING_GRP_LEN/8; i++) {
                port_slot_tbl[i] = pm_port_lo;
            }
            break;
        default:
            break;
    }


    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("tdm_fb6_ovsb_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
            break;
    }

    /* Populate pkt scheduler calendar */
    pm_offset = pkt_shpr_pm_indx;
    for (i=FB6_SHAPING_GRP_LEN; i<320; i++) {
            pkt_shed_cal[i] = FB6_NUM_EXT_PORTS;
            /*TDM_PRINT2("FOR GRT 160 PKT SCHED CAL i %0d , pkt_sched_cal[i]  %0d\n",i, pkt_shed_cal[i]);*/
    }
    for (i=0; i<FB6_SHAPING_GRP_LEN/8; i++) {
        if (port_slot_tbl[i] != FB6_NUM_EXT_PORTS) {
            lane = i % FB6_NUM_PM_LNS;
            switch (lane) {
                case 0:  lane_offset = 0;  break; /* subport 0 */
                case 1:  lane_offset = 8;  break; /* subport 2 */
                case 2:  lane_offset = 16; break; /* subport 1 */
                /* case 3:  lane_offset = 24; break; */
                default: lane_offset = 24;  break; /* subport 3 */
            }
            block_num = (i / FB6_NUM_PM_LNS) % pkt_sched_repetitions;
            block_offset = block_num * 32;
            cal_pos = (block_offset + lane_offset + pm_offset) % FB6_SHAPING_GRP_LEN;
            pkt_shed_cal[cal_pos] = port_slot_tbl[i];
            /*TDM_PRINT3("PKT SCHED CAL block_offset %0d , lane_offset %0d pm_offset %0d\n", block_offset, lane_offset, pm_offset);
            TDM_PRINT3("PKT SCHED CAL i %0d , cal_pos %0d pkt_sched_cal[cal_pos]  %0d\n",i, cal_pos, pkt_shed_cal[cal_pos]);*/
        }
    }

    return (PASS);
}


/**
@name: tdm_fb6_ovsb_chk_port_is_os
@param:

Return BOOL_TRUE if given port is an oversub port,
Otherwise, return FALSE.
 */
int
tdm_fb6_ovsb_chk_port_is_os(tdm_mod_t *_tdm, int port)
{
    int result = BOOL_FALSE;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;
    param_states   = _tdm->_chip_data.soc_pkg.state;

    if (port >= param_phy_lo && port <= param_phy_hi) {
        if((param_speeds[port] != SPEED_0) &&
           ((param_states[port-1] == PORT_STATE__OVERSUB) ||
            (param_states[port-1] == PORT_STATE__OVERSUB_HG)) ) {
            result = BOOL_TRUE;
        }
    }

    return (result);
}

/**
@name: tdm_fb6_ovsb_chk_pm_is_4x10g
@param:

Return BOOL_TRUE if given PM is 4x10g,
Otherwise, return FALSE.
 */
int
tdm_fb6_ovsb_chk_pm_is_4x10g(tdm_mod_t *_tdm, int pm_num)
{
    int result = BOOL_FALSE, port;
    int param_phy_lo, param_phy_hi, param_pm_lanes;
    enum port_speed_e *param_speeds;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;

    port = param_pm_lanes * pm_num + 1;
    if (port >= param_phy_lo && port <= param_phy_hi) {
        if((param_speeds[port]   == SPEED_10G || param_speeds[port]   == SPEED_11G) &&
           (param_speeds[port+1] == SPEED_10G || param_speeds[port+1] == SPEED_11G) &&
           (param_speeds[port+2] == SPEED_10G || param_speeds[port+2] == SPEED_11G) &&
           (param_speeds[port+3] == SPEED_10G || param_speeds[port+3] == SPEED_11G)) {
            result = BOOL_TRUE;
        }
    }

    return (result);
}


/**
@name: tdm_fb6_ovsb_hpipe_has_100g
@param:

Return BOOL_TRUE if selected hpipe contains 100G oversub port,
Otherwise, return FALSE.
 */
int
tdm_fb6_ovsb_hpipe_has_100g(tdm_mod_t *_tdm, int pipe_id, int hp_num)
{
    int result = BOOL_FALSE;
    int port;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    int hp_var;
    int half_pipe_map_en[17];

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;
    param_states   = _tdm->_chip_data.soc_pkg.state;


    tdm_fb6_cmn_get_half_pipe_map_en(_tdm, half_pipe_map_en);

  /*for (i=0; i<=17; i++) {
       TDM_PRINT2("100G Half pipe map en i %d half_pipe_e[i] %d\n",
        i, half_pipe_map_en[i]);
    }*/

    for(port=param_phy_lo; port<=param_phy_hi; port++) {
            hp_var = port/4;
            /*TDM_PRINT3("tdm_fb6_ovsb_hpipe_has_100g port %0d hp_var
                  %0d hp_num %0d \n", port, hp_var, hp_num);*/
            if(((half_pipe_map_en[hp_var] == 0) && (hp_num==1))
                || ((half_pipe_map_en[hp_var] == 1) && (hp_num==0)))  {
            /*TDM_PRINT1("tdm_fb6_ovsb_hpipe_has_100g Entered hp_num
               %0d \n", hp_num);*/
              if ((param_speeds[port] == SPEED_100G ||
                   param_speeds[port] == SPEED_106G ) &&
                  (param_states[port-1] == PORT_STATE__OVERSUB ||
                   param_states[port-1] == PORT_STATE__OVERSUB_HG)) {
                  result = BOOL_TRUE;
                  break;
              }
            }
    }

    return (result);
}


/**
@name: tdm_fb6_ovsb_pkt_shaper_new
@param:

Populates Pkt shaper calendar
 */
int
tdm_fb6_ovsb_pkt_shaper_new(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num;
    int pm_s, pm_e;
    int port_phy, port_phy_base;
    int i, hp_num, shift_step, hp_pm_cnt_4x10g[2];
    int param_pm_lanes;
    int *param_pm_hp_id;

    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_hp_id = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    pm_s = pipe_id*pms_per_pipe;
    pm_e = (pipe_id+1)*pms_per_pipe;
    for (i = 0; i < 2; i++) {
        hp_pm_cnt_4x10g[i] = 0;
    }

    for (pm_num=pm_s; pm_num<pm_e; pm_num++) {
        port_phy_base = param_pm_lanes * pm_num + 1;
        shift_step = 0;
        /* if pm is 4x10g and hpipe contains 100G, then shift 4x10g */
        if (tdm_fb6_ovsb_chk_pm_is_4x10g(_tdm, pm_num) == BOOL_TRUE) {
            hp_num = param_pm_hp_id[pm_num];
            if (tdm_fb6_ovsb_hpipe_has_100g(_tdm, pipe_id, hp_num)
                                           == BOOL_TRUE) {
                shift_step = hp_pm_cnt_4x10g[hp_num%2];
                hp_pm_cnt_4x10g[hp_num%2]++;
            }
        }
        for (ln_num=0; ln_num<param_pm_lanes; ln_num++) {
            port_phy = port_phy_base + ln_num;
            if (tdm_fb6_ovsb_chk_port_is_os(_tdm, port_phy) == BOOL_TRUE) {
                _tdm->_core_data.vars_pkg.port = port_phy;
                tdm_fb6_ovsb_pkt_shaper_per_pm(_tdm, shift_step);
                break;
            }
        }
    }

    return (PASS);
}


/**
@name: tdm_fb6_ovsb_pkt_shaper_find_pm_indx
@param:

Find an existing or available pm_indx in pkt_shaper
 */
int
tdm_fb6_ovsb_pkt_shaper_find_pm_indx(tdm_mod_t *_tdm)
{
    int pipe_id, pm_num, half_pipe_num;
    int i, j, pm_num_t;
    int phy_port;
    int *pkt_shed_cal;
    int pkt_shpr_pm_indx, pms_per_pipe;
    int pm_indx_avail[8];
    int max_start_indx;
    int distance;
    int max_distance;

    pipe_id = _tdm->_core_data.vars_pkg.cal_id;
    phy_port = _tdm->_core_data.vars_pkg.port;
    pm_num = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num];
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;

    /*pkt_shed_cal = (int*) malloc(320 * sizeof(int));*/
    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[FB6_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("tdm_fb6_ovsb_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
    }

    /* Two cases possible:
    1. there are sister ports of this port already in the pkt scheduler;
       which means find that PM and place phy_port in the corresponding lanes
    2. there is no sister port of this port already in the pkt scheduler;
       which means find an empty PM and place phy_port in the corresponding lanes*/

    pkt_shpr_pm_indx = -1;
    for (i=0; i<FB6_SHAPING_GRP_LEN; i++) {
        if (pkt_shed_cal[i] != _tdm->_chip_data.soc_pkg.num_ext_ports) {
            _tdm->_core_data.vars_pkg.port = pkt_shed_cal[i];
            pm_num_t = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
            if (pm_num == pm_num_t) { /* Found that PM is already placed */
                pkt_shpr_pm_indx = (i% (pms_per_pipe/2)); /* i%8 */
                break;
            }
        }
    }

    /* Case 2: there is no sister port of this port already in the pkt scheduler;
       which means find an empty PM and place phy_port in the corresponding lanes
       Find an available PM */
    if (pkt_shpr_pm_indx == -1) {
        for (i=0; i<(pms_per_pipe/2); i++){
            pm_indx_avail[i]=1;
            for (j=0; j<(FB6_SHAPING_GRP_LEN/(pms_per_pipe/2)); j++){
                if (pkt_shed_cal[j*(pms_per_pipe/2)+i] != _tdm->_chip_data.soc_pkg.num_ext_ports) {
                    pm_indx_avail[i]=0;
                    break;
                }
            }
        }

        /* Find the biggest clump of 1's in pm_indx_avail array and choose the pm_indx in the middle of the clump*/
        max_start_indx = 0;
        distance=0;
        max_distance =  0;
        for (i=0; i<(pms_per_pipe/2); i++){
            if (pm_indx_avail[i]==1) {
                distance=0;
                for (j=0; j<(pms_per_pipe/2); j++){
                    if (pm_indx_avail[(i+j)%(pms_per_pipe/2)]==1) {
                        distance++;
                    } else {
                        break;
                    }
                }
                if (distance > max_distance) {
                    max_start_indx = i;
                    max_distance = distance;
                }
            }
        }
        /* If all available make it 0, else middle of the clump*/
        pkt_shpr_pm_indx = (max_distance==(pms_per_pipe/2)) ? 0 : ((max_start_indx + (max_distance/2)) % (pms_per_pipe/2));

        if (pm_indx_avail[pkt_shpr_pm_indx] == 0) {
            pkt_shpr_pm_indx = -1;
            TDM_ERROR3("tdm_fb6_ovsb_pkt_shaper_find_pm_indx() PIPE %d pm_num=%d phy_port=%d Unable to find an available PM \n",pipe_id, pm_num, phy_port);
        }
    }

    if (pkt_shpr_pm_indx == -1) {
        TDM_ERROR3("tdm_fb6_ovsb_pkt_shaper_find_pm_indx() PIPE %d pm_num=%d phy_port=%d Unable to find an available PM \n",pipe_id, pm_num, phy_port);
    }

    return pkt_shpr_pm_indx;
}

/**
@name: tdm_fb6_ovsb_fill_os_grps
@param:
 */
int
tdm_fb6_ovsb_fill_os_grps(tdm_mod_t *_tdm)
{
    int prt, prt_idx, pos, spd_idx, spd, grp, result = PASS;
    m_tdm_pipe_info_t *pipe_info;

    /* iterate each speed class */
    pipe_info = &(_tdm->_core_data.vars_pkg.pipe_info);
    for (spd_idx = 0; spd_idx < MAX_SPEED_TYPES; spd_idx++) {
        if (!pipe_info->os_spd_en[spd_idx]) continue;
        /* fill all ports in current speed class into ovsb group */
        for (prt_idx=0; prt_idx<pipe_info->os_prt_cnt[spd_idx]; prt_idx++) {
            prt = pipe_info->os_prt[spd_idx][prt_idx];
            spd = tdm_fb6_cmn_get_port_speed(prt, _tdm);
            grp = tdm_fb6_ovsb_sel_grp_4_prt(_tdm, prt);
            pos = tdm_fb6_ovsb_add_prt_to_grp(_tdm, prt, grp);
            if (grp >= 0 && pos >= 0) {
                TDM_PRINT5("%s, spd %0dG, prt %3d, grp %2d, pos %0d\n",
                           "TDM: add port to ovsb group",
                           (spd / 1000), prt, grp, pos);
            } else {
                result = FAIL;
                TDM_ERROR5("%s, spd %0dG, prt %3d, grp %0d, pos %0d\n",
                           "TDM: failed to add port to ovsb group",
                           (spd / 1000), prt, grp, pos);
            }
        }
    }
    TDM_SML_BAR

    return result;
}


/**
@name: tdm_fb6_ovsb_consolidate
@param:

Minimize the number of oversub speed groups of the given half pipe.
Note: All ports belonging to the same subpipe should be considered.
 */
int
tdm_fb6_ovsb_consolidate_hpipe(tdm_mod_t *_tdm, int hpipe)
{
    int grp_lo, grp_hi, grp_dst, grp_src, grp_spd_dst, grp_spd_src,
        grp_size_dst, grp_size_src, idx_dst, idx_src, prt_src;
    int param_token_empty;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return PASS;

    TDM_PRINT0("TDM: [FLEX] run consolidation\n");
    tdm_fb6_ovsb_get_hpipe_grp_range(_tdm, hpipe, &grp_lo, &grp_hi);
    for (grp_dst = grp_lo; grp_dst < grp_hi; grp_dst++) {
        grp_spd_dst = tdm_fb6_ovsb_get_grp_spd(_tdm, grp_dst);
        grp_size_dst= tdm_fb6_ovsb_get_grp_prt_cnt(_tdm, grp_dst);
        /* for each non-empty group, do consolidation */
        if(grp_spd_dst == 0 || grp_size_dst == cal->grp_len) continue;
        for (grp_src = grp_dst + 1; grp_src <= grp_hi; grp_src++) {
            /* find two same speed groups */
            grp_spd_src = tdm_fb6_ovsb_get_grp_spd(_tdm, grp_src);
            grp_size_src= tdm_fb6_ovsb_get_grp_prt_cnt(_tdm, grp_src);
            if (grp_spd_src != grp_spd_dst) continue;
            if ((grp_size_src + grp_size_dst) > cal->grp_len) continue;
            /* merge two same speed groups into one */
            idx_dst = 0;
            for (idx_src = 0; idx_src < cal->grp_len; idx_src++) {
                prt_src = cal->cal_grp[grp_src][idx_src];
                if(prt_src == param_token_empty) continue;
                while (idx_dst < cal->grp_len) {
                    if (cal->cal_grp[grp_dst][idx_dst] == param_token_empty) {
                        cal->cal_grp[grp_dst][idx_dst] = prt_src;
                        cal->cal_grp[grp_src][idx_src] = param_token_empty;
                        break;
                    }
                    idx_dst++;
                }
            }
        }
    }

    return PASS;
}


/**
@name: tdm_fb6_ovsb_flex_port_dn
@param:

Remove down ports from ovsb speed groups
 */
int
tdm_fb6_ovsb_flex_port_dn(tdm_mod_t *_tdm)
{
    int pos, prt, prt_done, prt_lo = 0, prt_hi = 0;
    int grp, flex_dn_exist = BOOL_FALSE;
    int param_token_empty;
    enum port_speed_e *param_speeds_prev;
    enum port_state_e *param_states_prev;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_speeds_prev = _tdm->_prev_chip_data.soc_pkg.speed;
    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;

    /* check whether exists down port in current pipe */
    tdm_fb6_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_DN ||
            param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
            flex_dn_exist = BOOL_TRUE;
            break;
        }
    }
    if (flex_dn_exist == BOOL_FALSE) return PASS;

    TDM_PRINT0("TDM: Flex DOWN ports\n");
    /* remove down_ports and spd_changed_ports */
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return PASS;
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt-1] == PORT_STATE__FLEX_OVERSUB_DN ||
            param_states_prev[prt-1] == PORT_STATE__FLEX_OVERSUB_CH) {
            prt_done = BOOL_FALSE;
            for (grp = 0; grp <= cal->grp_num; grp++) {
                for (pos = 0; pos < cal->grp_len; pos++) {
                    if (cal->cal_grp[grp][pos] == prt) {
                        cal->cal_grp[grp][pos] = param_token_empty;
                        prt_done = BOOL_TRUE;
                        TDM_PRINT5("%s %0d, spd %0dG, grp %0d, pos %0d\n",
                                   "TDM: [FLEX DN] remove down port",
                                   prt, param_speeds_prev[prt]/1000,
                                   grp, pos);
                        break;
                    }
                }
                if (prt_done == BOOL_TRUE) break;
            }
        }
    }
    TDM_SML_BAR

    return PASS;
}

/**
@name: tdm_fb6_ovsb_flex_port_up
@param:

Add up ports into ovsb speed groups
 */
int
tdm_fb6_ovsb_flex_port_up(tdm_mod_t *_tdm)
{
    int prt_lo, prt_hi, prt, prt_spd, pos, grp, hpipe, result = PASS;
    int hpipe_done[FB6_NUM_HPIPE];
    int flex_up_exist = BOOL_FALSE;
    enum port_state_e *param_states_prev;

    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;
    for (hpipe = 0; hpipe < FB6_NUM_HPIPE; hpipe++) hpipe_done[hpipe] = 0;
    tdm_fb6_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);

    /* check whether exists up port in current pipe */
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_UP ||
            param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
            flex_up_exist = BOOL_TRUE;
            break;
        }
    }
    if (flex_up_exist == BOOL_FALSE) return result;
    TDM_PRINT0("TDM: Flex UP ports\n");

    /* do consolidation */
    
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_UP ||
            param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
            hpipe = tdm_fb6_ovsb_get_prt_hpipe(_tdm, prt);
            if (hpipe >= 0 && hpipe < FB6_NUM_HPIPE) {
                if (hpipe_done[hpipe] == 0) {
                    tdm_fb6_ovsb_consolidate_hpipe(_tdm, hpipe);
                    hpipe_done[hpipe] = 1;
                }
            }
        }
    }

    /* add up_ports and spd_changed_ports */
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_UP ||
            param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
            prt_spd = tdm_fb6_cmn_get_port_speed(prt, _tdm);
            grp = tdm_fb6_ovsb_sel_grp_4_prt(_tdm, prt);
            pos = tdm_fb6_ovsb_add_prt_to_grp(_tdm, prt, grp);
            if (grp != -1 && pos != -1) {
                TDM_PRINT5("%s, prt %0d, spd %0dG, grp %0d, pos %0d\n",
                           "TDM: [FLEX] add up_port",
                           prt, (prt_spd / 1000), grp, pos);
            } else {
                result = FAIL;
                TDM_ERROR3("%s, prt %0d, spd %0dG, no available ovsb speed group\n",
                           "TDM: [FLEX] failed to add up_port",
                           prt, (prt_spd / 1000));
            }
        }
    }
    TDM_SML_BAR

    return result;
}



/**
@name: tdm_fb6_ovsb_gen_os_grps_init
@param:
 */
int
tdm_fb6_ovsb_gen_os_grps_init(tdm_mod_t *_tdm)
{
    int result = PASS;

    /* partition PMs into subpipes */
    /*if (tdm_fb6_ovsb_gen_pm2hpipe_map(_tdm, 0) != PASS) result = FAIL;*/

    /* allocate ports into ovsb groups based on partitioned subpipes */
    if (tdm_fb6_ovsb_fill_os_grps(_tdm) != PASS) result = FAIL;

    return result;
}

/**
@name: tdm_fb6_ovsb_gen_os_grps_flex
@param:
 */
int
tdm_fb6_ovsb_gen_os_grps_flex(tdm_mod_t *_tdm)
{
    int grp, pos, result = PASS;
    tdm_calendar_t *cal = NULL, *cal_prev = NULL;

    /* step 1: copy ovsb grps from cal_prev into cal */
    TDM_PRINT0("TDM: Restore ovsb grps by pre_flex values\n");
    cal      = tdm_fb6_cmn_get_pipe_cal(_tdm);
    cal_prev = tdm_fb6_cmn_get_pipe_cal_prev(_tdm);
    if (cal == NULL || cal_prev == NULL) return FAIL;
    for (grp = 0; grp < cal->grp_num; grp++) {
        for (pos=0; pos<cal->grp_len; pos++) {
            cal->cal_grp[grp][pos] = cal_prev->cal_grp[grp][pos];
        }
    }

    /* step 2: partition PMs into subpipes */
    if (tdm_fb6_ovsb_gen_pm2hpipe_map(_tdm, 0) != PASS) result = FAIL;

    /* step 3: flex up-ports */
    if (tdm_fb6_ovsb_flex_port_dn(_tdm) != PASS) result = FAIL;

    /* step 4: flex down-ports */
    if (tdm_fb6_ovsb_flex_port_up(_tdm) != PASS) result = FAIL;

    return result;
}


/**
@name: tdm_fb6_ovsb_gen_os_grps
@param:
 */
int
tdm_fb6_ovsb_gen_os_grps(tdm_mod_t *_tdm)
{
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        return (tdm_fb6_ovsb_gen_os_grps_flex(_tdm));
    } else {
        return (tdm_fb6_ovsb_gen_os_grps_init(_tdm));
    }
}

/**
@name: tdm_fb6_ovsb_gen_pkt_shaper_per_prt
@param:

Populates Pkt shaper calendar - per port
 */
int
tdm_fb6_ovsb_gen_pkt_shaper_per_prt(tdm_mod_t *_tdm, int prt)
{
    int i, pos, prt_slots, prt_pm, prt_hpipe, slot_cnt, result = PASS;
    int pm_shaper_tbl[FB6_SHAPER_NUM_PM_SLOTS];
    int *pkt_shaper_cal, *pm2hpipe_map;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    pm2hpipe_map = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe;

    for (i = 0; i < FB6_SHAPER_NUM_PM_SLOTS; i++)
        pm_shaper_tbl[i] = param_token_empty;

    /* generate pm shaper calendar for the current port */
    prt_slots = tdm_fb6_cmn_get_port_speed_eth(_tdm, prt) /
                FB6_SHAPER_SLOT_UNIT;
    for (slot_cnt = 0; slot_cnt < prt_slots; slot_cnt++) {
        pos = tdm_fb6_ovsb_get_pm_shaper_prt_pos(_tdm, prt, slot_cnt) %
              FB6_SHAPER_NUM_PM_SLOTS;
        pm_shaper_tbl[pos] = prt;
    }

    TDM_PRINT1("TDM: prt %3d, pm_shaper_cal[", prt);
    for (pos = 0; pos < FB6_SHAPER_NUM_PM_SLOTS; pos++) {
        TDM_PRINT2("%0d:%0d, ", pos, pm_shaper_tbl[pos]);
    }
    TDM_PRINT0("]\n");

    /* Get the right pkt scheduler calendar*/
    prt_pm    = tdm_fb6_cmn_get_port_pm(prt, _tdm);
    prt_hpipe = pm2hpipe_map[prt_pm];
    pkt_shaper_cal = tdm_fb6_ovsb_get_pkt_shaper_cal(_tdm, prt_hpipe);
    if (pkt_shaper_cal == NULL) return FAIL;

    /* Populate pkt scheduler calendar */
    slot_cnt = 0;
    for (i = 0; i < FB6_SHAPER_NUM_PM_SLOTS; i++) {
        if (pm_shaper_tbl[i] != param_token_empty) {
            pos = tdm_fb6_ovsb_get_pkt_shaper_prt_pos(_tdm, prt, i);
            slot_cnt++;
            if (pos >=0 && pos < FB6_SHAPING_GRP_LEN) {
                pkt_shaper_cal[pos] = pm_shaper_tbl[i];
                TDM_PRINT4("%s, prt %3d, slot_cnt %0d, index %0d\n",
                           "TDM: [pkt_shaper_cal]",
                           pm_shaper_tbl[i], slot_cnt, pos);
            } else {
                result = FAIL;
                TDM_ERROR3("%s, prt %3d, slot_cnt %0d\n",
                           "[pkt_shaper_cal] Failed to allocate port slots",
                           prt, slot_cnt);
            }
        }
    }

    return result;
}

/**
@name: tdm_fb6_ovsb_gen_pkt_shaper_init
@param:

Populates Pkt shaper calendar
 */
/*int
tdm_fb6_ovsb_gen_pkt_shaper_init(tdm_mod_t *_tdm)
{
    int hpipe, grp, pos, prt;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return FAIL;

    for (hpipe = 0; hpipe < FB6_NUM_HPIPE; hpipe++) {
        for (grp = 0; grp < cal->grp_num; grp++) {
            for (pos = 0; pos < cal->grp_len; pos++) {
                prt = cal->cal_grp[grp][pos];
                if (prt != param_token_empty)
                    tdm_fb6_ovsb_gen_pkt_shaper_per_prt(_tdm, prt);
            }
        }
    }
    TDM_SML_BAR

    return PASS;
}*/




/**
@name: tdm_fb6_ovsb_gen_pkt_shaper_flex
@param:

Populates Pkt shaper calendar
 */
int
tdm_fb6_ovsb_gen_pkt_shaper_flex(tdm_mod_t *_tdm)
{
    int i, hpipe, grp, pos, prt;
    int port_done[FB6_NUM_EXT_PORTS];
    tdm_calendar_t *cal;
    int *pkt_cal, *pkt_cal_prev;
    int param_token_empty;
    enum port_state_e *param_states_prev;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;
    cal = tdm_fb6_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return FAIL;
    for (i = 0; i < FB6_NUM_EXT_PORTS; i++) port_done[i] = 0;

    /* step 1: copy pkt shaper calendar from pkt_cal_prev into pkt_cal */
    for (hpipe = 0; hpipe < FB6_NUM_HPIPE; hpipe++) {
        pkt_cal      = tdm_fb6_ovsb_get_pkt_shaper_cal(_tdm, hpipe);
        pkt_cal_prev = tdm_fb6_ovsb_get_pkt_shaper_cal_prev(_tdm, hpipe);
        if (pkt_cal == NULL || pkt_cal_prev == NULL) continue;
        for (i = 0; i < FB6_SHAPING_GRP_LEN; i++) {
            prt = pkt_cal_prev[i];
            if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_DN ||
                param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
                prt = param_token_empty;
            }
            pkt_cal[i] = prt;
            if (prt!=param_token_empty && prt>0 && prt<FB6_NUM_EXT_PORTS) {
                port_done[prt] = 1;
            }
        }
    }

    /* step 2: allocate slots for up-ports in pkt shaper calendar */
    for (hpipe = 0; hpipe < FB6_NUM_HPIPE; hpipe++) {
        for (grp = 0; grp < cal->grp_num; grp++) {
            for (pos = 0; pos < cal->grp_len; pos++) {
                prt = cal->cal_grp[grp][pos];
                if (prt != param_token_empty &&
                    prt > 0 && prt < FB6_NUM_EXT_PORTS) {
                    if (port_done[prt] == 0) {
                        tdm_fb6_ovsb_gen_pkt_shaper_per_prt(_tdm, prt);
                        port_done[prt] = 1;
                    }
                }
            }
        }
    }
    TDM_SML_BAR

    return PASS;
}



/**
@name: tdm_fb6_ovsb_gen_pkt_shaper
@param:

Populates Pkt shaper calendar
 */
int
tdm_fb6_ovsb_gen_pkt_shaper(tdm_mod_t *_tdm)
{
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        return (tdm_fb6_ovsb_gen_pkt_shaper_flex(_tdm));
        /* return (tdm_fb6_ovsb_gen_pkt_shaper_init(_tdm)); */
    } else {
        return (tdm_fb6_ovsb_gen_pkt_shaper_init(_tdm));
    }

    return PASS;
}


/**
@name: tdm_fb6_ovsb_map_pm_num_to_pblk
@param:

Maps PM num (block_id) to OVSB pblk id
 */
int
tdm_fb6_ovsb_map_pm_num_to_pblk(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num;
    int half_pipe_num;
    int pblk_indx[2];
    static int dis_pm_num_to_pblk=0;
    static int prev_pblk[17];
    int *pm2pblk_map=NULL;

    pipe_id = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/FB6_NUM_QUAD;
    pm2pblk_map  = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk;


    TDM_PRINT1("OVSB Mapping dis_pm_num_to_pblk %d\n", dis_pm_num_to_pblk);
    pblk_indx[0]=0;	pblk_indx[1]=0;
   if(dis_pm_num_to_pblk==0) {
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[pm_num] = -1;
        half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[pm_num];
        TDM_PRINT3("OVSB Mapping pm_num %d half_pipe_num %d pblk_indx[half_pipe_num] %d\n", pm_num, half_pipe_num, pblk_indx[half_pipe_num]);
        if ((half_pipe_num==0) || (half_pipe_num==1)){
            pm2pblk_map[pm_num]  = pblk_indx[half_pipe_num];
            prev_pblk[pm_num] = pblk_indx[half_pipe_num];
            /*switch (pipe_id) {
                case 0:
                    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[pm_num] = pblk_indx[half_pipe_num];
                    break;
                case 1:
                    _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[pm_num] = (7 - pblk_indx[half_pipe_num]);
                    break;
                default:
                    TDM_ERROR2("%s Invalid Calendar ID %d \n",
                              "[tdm_fb6_ovsb_map_pm_num_to_pblk]", pipe_id);
            }*/
            pblk_indx[half_pipe_num]++;
        }
    }
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        TDM_PRINT3("OVSB Mapping Prev pm_num %d _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[pm_num] %d prev_pblk[pm_num] %d\n", pm_num, pm2pblk_map[pm_num], prev_pblk[pm_num]);
    }
    dis_pm_num_to_pblk++;

    }
    else {

      for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
            pm2pblk_map[pm_num]  = prev_pblk[pm_num];
        TDM_PRINT3("OVSB Mapping Prev pm_num %d _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_num_to_pblk[pm_num] %d prev_pblk[pm_num] %d\n", pm_num, pm2pblk_map[pm_num], prev_pblk[pm_num]);
      }
    }
    return PASS;
}




/**
@name: tdm_fb6_ovsb_fixmapping
@param:

Generate sortable weightable groups for oversub round robin arbitration
Fixed mapping:
[pipe 0] 1 - 32: hpipe 0
[pipe 0]33 - 64: hpipe 1
[pipe 1]65 - 96: hpipe 0
[pipe 1]97 -128: hpipe 1
 */
int
tdm_fb6_ovsb_fixmapping(tdm_mod_t *_tdm)
{
    int i, j;
    tdm_calendar_t *cal;
    int half_pipe_en[17] = {0};
    int half_pipe_map_en[17];

    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
        case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
        case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
        case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
        case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
        case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
        case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
        case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return (TDM_EXEC_CORE_SIZE+1);
    }
    for (i=0; i<cal->grp_num; i++) {
        for (j=0; j<cal->grp_len; j++) {
            cal->cal_grp[i][j] = _tdm->_chip_data.soc_pkg.num_ext_ports;
        }
    }

    TDM_BIG_BAR
    TDM_SML_BAR
    TDM_PRINT1("  1G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8);
    TDM_PRINT1(" 10G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1);
    TDM_PRINT1(" 20G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2);
    TDM_PRINT1(" 25G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6);
    TDM_PRINT1(" 40G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3);
    TDM_PRINT1(" 50G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5);
    TDM_PRINT1("100G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4);
    TDM_PRINT1("120G ports : %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7);
    TDM_PRINT1("\n Number of speed types: %0d\n", (_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100 + _tdm->_core_data.vars_pkg.os_120));

    if ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100 + _tdm->_core_data.vars_pkg.os_120) > cal->grp_num) {
        TDM_ERROR0("Oversub speed type limit exceeded\n");
        return FAIL;
    }
    if (((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)==cal->grp_num && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>cal->grp_len || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>cal->grp_len || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2>cal->grp_len)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-1) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>32 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>32 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2>32)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-2) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>48 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>48)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-3) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>TDM_AUX_SIZE || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>TDM_AUX_SIZE))) {
        TDM_ERROR0("Oversub bucket overflow\n");
        return FAIL;
    }
    /* Apply Half-Pipe constraints */
    {
        int pm_num_lo, pm_num_hi, port_phy,
            pms_per_pipe;
        int param_pipe_id, param_phy_lo, param_phy_hi,
            param_pm_lanes, param_pm_num;

        param_pipe_id = _tdm->_core_data.vars_pkg.cal_id;
        param_phy_lo  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
        param_phy_hi  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
        param_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
        param_pm_num  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;

        pms_per_pipe  = param_pm_num/FB6_NUM_QUAD;
        pm_num_lo     = param_pipe_id*pms_per_pipe;
        pm_num_hi     = (param_pipe_id+1)*pms_per_pipe-1;



   for (i=pm_num_lo; i<=pm_num_hi; i++) {
       if(i==0) half_pipe_en[i] = 0;
       if(i==1) half_pipe_en[i] = 1;
       if(i==2) half_pipe_en[i] = 0;
       if(i==3) half_pipe_en[i] = 1;
       if(i==4) half_pipe_en[i] = 0;
       if(i>=5 &&  i<=10) half_pipe_en[i] = 1;
       if(i>=11 && i<=16) half_pipe_en[i] = 0;
       TDM_PRINT4("OVSB:i %d pm_num_lo %d pm_num_hi %d param_pm_num %d\n",i, pm_num_lo, pm_num_hi, param_pm_num);
    }


   for (i=pm_num_lo; i<=pm_num_hi; i++) {
       TDM_PRINT2("Printing Half pipe en i %d half_pipe_e[i] %d\n", i, half_pipe_en[i]);
    }

     tdm_fb6_cmn_get_half_pipe_map_en(_tdm, half_pipe_map_en);

   for (i=pm_num_lo; i<=pm_num_hi; i++) {
       TDM_PRINT2("Printing Half pipe map en i %d half_pipe_e[i] %d\n", i, half_pipe_map_en[i]);
    }
        for (i=pm_num_lo; i<=pm_num_hi && i<param_pm_num; i++) {
            _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[i] = -1;
            for (j=0; j<param_pm_lanes; j++) {
                port_phy = i*param_pm_lanes+j+1;
       TDM_PRINT3("OVSB: Before Printing port phy %d  param_phy_lo %d param_phy_hi %d\n", port_phy, param_phy_lo, param_phy_hi);
       TDM_PRINT2("OVSB: Before Speed %d State %d\n\n", _tdm->_chip_data.soc_pkg.speed[port_phy], _tdm->_chip_data.soc_pkg.state[port_phy-1]);
                if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
                    if ( (_tdm->_chip_data.soc_pkg.speed[port_phy] != SPEED_0) &&
                         (_tdm->_chip_data.soc_pkg.state[port_phy-1] == PORT_STATE__OVERSUB ||
                          _tdm->_chip_data.soc_pkg.state[port_phy-1] == PORT_STATE__OVERSUB_HG)) {
                          if(half_pipe_map_en[i] == 1) {
                                _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[i] = 0;
       TDM_PRINT1("OVSB: Printing port phy %d \n", port_phy);
                          }
                          else {
                                _tdm->_chip_data.soc_pkg.soc_vars.fb6.pm_ovs_halfpipe[i] = 1;
       TDM_PRINT1("OVSB: HP 1 Printing port phy %d \n", port_phy);
                          }
                        break;
                    }
                }
            }
        }
    }
    /* Partition  */
   /* generate ovsb groups */
    if (tdm_fb6_ovsb_gen_os_grps(_tdm)) {
      TDM_PRINT1("Generating OVSB groups for PIPE %0d\n\n",  _tdm->_core_data.vars_pkg.cal_id);
    }

    if (tdm_fb6_ovsb_gen_pkt_shaper(_tdm)){
      TDM_PRINT1("OVSB partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    }

    if(tdm_fb6_ovsb_map_pm_num_to_pblk(_tdm)) {
      TDM_PRINT1("OVSB partition for PIPE %d PM_NUM to PBLK mapping DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    }

    return PASS;

    
    
    
}

int
tdm_fb6_vbs_scheduler_ovs(tdm_mod_t *_tdm)
{
    /* return (tdm_fb6_ovsb_flexmapping(_tdm)); */

    return (tdm_fb6_ovsb_fixmapping(_tdm));
}
