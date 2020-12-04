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
@name: tdm_td3_ovsb_get_pkt_shaper_cal
@param:
@desc: [chip_specific]

Return pionter of the packet shaper scheduler.
 */
int *
tdm_td3_ovsb_get_pkt_shaper_cal(tdm_mod_t *_tdm, int spipe_num)
{
    int * cal = NULL;
    int cal_offset;

    cal_offset = TD3_SHAPING_GRP_IDX_0 + spipe_num;
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	cal = _tdm->_chip_data.cal_0.cal_grp[cal_offset]; break;
        case 1:	cal = _tdm->_chip_data.cal_1.cal_grp[cal_offset]; break;
        case 2:	cal = _tdm->_chip_data.cal_2.cal_grp[cal_offset]; break;
        case 3:	cal = _tdm->_chip_data.cal_3.cal_grp[cal_offset]; break;
        default:
            TDM_ERROR2("Invalid calendar ID %0d for %s\n",
                       _tdm->_core_data.vars_pkg.cal_id,
                       "tdm_td3_ovsb_get_pkt_shaper_cal()");
            cal = NULL; break;
    }

    return cal;
}

/**
@name: tdm_td3_ovsb_get_pkt_shaper_cal_prev
@param:
@desc: [chip_specific]

Return pionter of the previous packet shaper scheduler.
 */
int *
tdm_td3_ovsb_get_pkt_shaper_cal_prev(tdm_mod_t *_tdm, int spipe_num)
{
    int * cal = NULL;
    int cal_offset;

    cal_offset = TD3_SHAPING_GRP_IDX_0 + spipe_num;
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	cal = _tdm->_prev_chip_data.cal_0.cal_grp[cal_offset]; break;
        case 1:	cal = _tdm->_prev_chip_data.cal_1.cal_grp[cal_offset]; break;
        case 2:	cal = _tdm->_prev_chip_data.cal_2.cal_grp[cal_offset]; break;
        case 3:	cal = _tdm->_prev_chip_data.cal_3.cal_grp[cal_offset]; break;
        default:
            TDM_ERROR2("Invalid calendar ID %0d for %s\n",
                       _tdm->_core_data.vars_pkg.cal_id,
                       "tdm_td3_ovsb_get_pkt_shaper_cal_prev()");
            cal = NULL; break;
    }

    return cal;
}

/**
@name: tdm_td3_ovsb_get_spipe_grp_range
@param:

Return ovsb group range [grp_lo, grp_hi] for given subpipe.
 */
void
tdm_td3_ovsb_get_spipe_grp_range(tdm_mod_t *_tdm, int spipe, int *lo, int *hi)
{
    int grp_per_spipe, grp_lo = 0, grp_hi = 0;
    tdm_calendar_t *cal;

    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal != NULL) {
        if (spipe >= 0 && spipe < TD3_NUM_SPIPE && cal->grp_num > 0) {
            grp_per_spipe = cal->grp_num / TD3_NUM_SPIPE;
            grp_lo = (spipe * grp_per_spipe) % cal->grp_num;
            grp_hi = (grp_lo + grp_per_spipe - 1) % cal->grp_num;
        }
    }
    (*lo) = grp_lo;
    (*hi) = grp_hi;
}

/**
@name: tdm_td3_ovsb_get_grp_spd
@param:

Return group speed of the given ovsb group in ethernet speed format.
If ovsb group is empty, return 0.
 */
int
tdm_td3_ovsb_get_grp_spd(tdm_mod_t *_tdm, int grp)
{
    int prt, pos;
    int param_token_empty;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return 0;

    if (grp >= 0 && grp < cal->grp_num) {
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) {
                return (tdm_td3_cmn_get_port_speed_eth(_tdm, prt));
            }
        }
    }

    return 0;
}

/**
@name: tdm_td3_ovsb_get_grp_prt_cnt
@param:

Return number of active ports in the given oversub speed group.
If ovsb group is empty, return 0.
 */
int
tdm_td3_ovsb_get_grp_prt_cnt(tdm_mod_t *_tdm, int grp)
{
    int prt, pos, prt_cnt = 0;
    int param_token_empty;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
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
@name: tdm_td3_ovsb_get_grp_empty_cnt
@param:

Return the number of empty slots in the given ovsb group.
 */
int
tdm_td3_ovsb_get_grp_empty_cnt(tdm_mod_t *_tdm, int grp)
{
    tdm_calendar_t *cal;

    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return 0;

    return (cal->grp_len - tdm_td3_ovsb_get_grp_prt_cnt(_tdm, grp));
}

/**
@name: tdm_td3_ovsb_get_prt_spipe
@param:

Return subpipe index of the given port.
    -1: unassigned
     n: subpipe index, 0 <= n < TD3_NUM_SPIPE
 */
int
tdm_td3_ovsb_get_prt_spipe(tdm_mod_t *_tdm, int port_token)
{
    int pm, spipe = -1;

    pm = tdm_td3_cmn_get_port_pm(_tdm, port_token);
    if (pm >= 0 && pm < TD3_NUM_PM_MOD) {
        spipe = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe[pm];
        /* if (spipe >= 0 && spipe < TD3_NUM_SPIPE) return spipe; */
    }

    return spipe;
}

/**
@name: tdm_td3_ovsb_get_prt_lane
@param:

Return relative lane index of the given port within PM.
 */
int
tdm_td3_ovsb_get_prt_lane(tdm_mod_t *_tdm, int prt)
{
    if (prt >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        prt <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return ((prt - 1) % TD3_NUM_PM_LNS);
    }

    return 0;
}

/**
@name: tdm_td3_ovsb_get_pm_shaper_prt_stride
@param:
@desc: [chip_specific]

Return incremental distance between two consecutive port slots in
PM shaper calendar.
 */
int
tdm_td3_ovsb_get_pm_shaper_prt_stride(tdm_mod_t *_tdm, int prt)
{
    int spd, stride;

    spd = tdm_td3_cmn_get_port_speed_eth(_tdm, prt);
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
@name: tdm_td3_ovsb_get_pm_shaper_prt_offset
@param:
@desc: [chip_specific]

Return port offset within pseudo PM shaper calendar.
 */
int
tdm_td3_ovsb_get_pm_shaper_prt_offset(tdm_mod_t *_tdm, int prt)
{
    int ln_base, ln_offset;

    ln_base = tdm_td3_ovsb_get_prt_lane(_tdm, prt);
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
@name: tdm_td3_ovsb_get_pm_shaper_prt_pos
@param:
@desc: [chip_specific]

Return the n-th slot pos of the given port in pm shaper calendar.
 */
int
tdm_td3_ovsb_get_pm_shaper_prt_pos(tdm_mod_t *_tdm, int prt, int slot_cnt)
{
    int pos, prt_offset, prt_stride, prt_spd, ln_base;

    prt_offset = tdm_td3_ovsb_get_pm_shaper_prt_offset(_tdm, prt);
    prt_stride = tdm_td3_ovsb_get_pm_shaper_prt_stride(_tdm, prt);

    prt_spd = tdm_td3_cmn_get_port_speed_eth(_tdm, prt);
    ln_base = tdm_td3_ovsb_get_prt_lane(_tdm, prt);
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
    pos = (prt_offset + prt_stride * slot_cnt) % TD3_SHAPER_NUM_PM_SLOTS;

    return pos;
}

/**
@name: tdm_td3_ovsb_get_pkt_shaper_prt_pm_idx
@param:

Find an existing or available pm_idx in pkt_shaper
 */
int
tdm_td3_ovsb_get_pkt_shaper_prt_pm_idx(tdm_mod_t *_tdm, int prt)
{
    int i, j, max_start_indx, distance, max_distance;
    int prt_spipe, prt_pm, pos, pm_idx;
    int *pkt_shaper_cal = NULL;
    int pm_indx_avail[TD3_NUM_PMS_PER_SPIPE];
    int param_cal_id, param_token_empty;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);

    prt_spipe = tdm_td3_ovsb_get_prt_spipe(_tdm, prt);
    if (!(prt_spipe >= 0 && prt_spipe < TD3_NUM_SPIPE)) return FAIL;

    pkt_shaper_cal = tdm_td3_ovsb_get_pkt_shaper_cal(_tdm, prt_spipe);
    if (pkt_shaper_cal == NULL) return FAIL;

    /* Case 1: there are sister ports of this port already in pkt scheduler,
       which means find that PM and place port in the corresponding lanes. */
    pm_idx = -1;
    for (i = 0; i < TD3_SHAPING_GRP_LEN; i++) {
        if (pkt_shaper_cal[i] != param_token_empty) {
            if (prt_pm == tdm_td3_cmn_get_port_pm(_tdm, pkt_shaper_cal[i])) {
                /* Find that PM has already been placed */
                pm_idx = i % TD3_NUM_PMS_PER_SPIPE;
                break;
            }
        }
    }

    /* Case 2: there is no sister port of this port already in pkt scheduler,
       which means find an empty PM and place port in the corresponding lanes
       Find an available PM */
    if (pm_idx == -1) {
        for (i = 0; i < TD3_NUM_PMS_PER_SPIPE; i++) {
            pm_indx_avail[i] = 1;
            for (j = 0; j < TD3_NUM_PKT_SLOTS_PER_PM; j++) {
                pos = (j * TD3_NUM_PMS_PER_SPIPE + i) % TD3_SHAPING_GRP_LEN;
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
        for (i = 0; i < TD3_NUM_PMS_PER_SPIPE; i++) {
            if (pm_indx_avail[i] == 1) {
                distance = 0;
                for (j = 0; j < TD3_NUM_PMS_PER_SPIPE; j++){
                    if (pm_indx_avail[(i+j) % TD3_NUM_PMS_PER_SPIPE] == 1) {
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
        pm_idx = (max_distance == TD3_NUM_PMS_PER_SPIPE) ? 0 :
                 ((max_start_indx + TD3_NUM_PMS_PER_SPIPE) % TD3_NUM_PMS_PER_SPIPE);

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
@name: tdm_td3_ovsb_get_pkt_shaper_prt_pos
@param:
@desc: [chip_specific]

For the n-th slot in pm_shaper_calendar, calculate the corresponding slot
postion in packet_shaper_calendar.
 */
int
tdm_td3_ovsb_get_pkt_shaper_prt_pos(tdm_mod_t *_tdm, int prt,
int pm_shaper_slot_idx)
{
    int pos = -1;
    int num_blocks, block_idx = 0, block_offset, lane, lane_offset, pm_offset;

    /* calc block_offset: one block is one minimum repetition pattern */
    num_blocks = TD3_SHAPING_GRP_LEN / TD3_NUM_PHY_PORTS_PER_SPIPE;
    if (num_blocks > 0) {
        block_idx = (pm_shaper_slot_idx / TD3_NUM_PM_LNS) % num_blocks;
    }
    block_offset = block_idx * TD3_NUM_PHY_PORTS_PER_SPIPE;

    /* calc lane_offset: one subblock contains slots with the same lane num */
    lane = pm_shaper_slot_idx % TD3_NUM_PM_LNS; /* lane num in pm_shaper_cal */
    switch (lane) {
        case 0:  lane_offset = 0;                       break; /* subport 0 */
        case 1:  lane_offset = 1*TD3_NUM_PMS_PER_SPIPE; break; /* subport 2 */
        case 2:  lane_offset = 2*TD3_NUM_PMS_PER_SPIPE; break; /* subport 1 */
        default: lane_offset = 3*TD3_NUM_PMS_PER_SPIPE; break; /* subport 3 */
    }

    /* calc pm_offset: within each subblock, slot is determined by pm_offset */
    pm_offset = tdm_td3_ovsb_get_pkt_shaper_prt_pm_idx(_tdm, prt);
    if (pm_offset == -1) return pos;

    /* calculate slot position of pm shaper slot in packet shaper calendar */
    pos = (block_offset + lane_offset + pm_offset) % TD3_SHAPING_GRP_LEN;

    return pos;
}

/**
@name: tdm_td3_ovsb_apply_constraints_init
@param:
@desc: [chip_specific]

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_td3_ovsb_apply_constraints_init(tdm_mod_t *_tdm)
{
    int i, j, prt, prt_pm, num_spd_types = 0, result = PASS;
    int param_cal_id;
    int *pm2spipe_map = NULL;
    m_tdm_pipe_info_t *pipe_info;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    pm2spipe_map = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe;
    pipe_info    = &(_tdm->_core_data.vars_pkg.pipe_info);

    for (i = 0; i < MAX_SPEED_TYPES; i++) {
        if (pipe_info->os_spd_en[i]) num_spd_types++;
    }

    /* check necessary condition */
    tdm_cmn_chk_pipe_os_spd_types(_tdm);

    /* Restriction 13: No port configurations with more than 4 port speed
     *                 classes are supported.
     */
    if (num_spd_types > 5) {
        result = FAIL;
        TDM_ERROR4("%s, PIPE %d, num_spd_types %0d, limit %0d\n",
                   "Failed in ovsb partition, number of speed types overflow",
                   param_cal_id, num_spd_types, 4);
    }
    /* Restriction 14: The only supported port configurations with 4 port
                       speed classes are:
            10G/20G/40G/100G
            10G/25G/50G/100G
       Rule: Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1.
    */
    if (num_spd_types == 4) {
        if ((pipe_info->os_spd_en[TDM_SPEED_IDX_10G] &&
             pipe_info->os_spd_en[TDM_SPEED_IDX_20G] &&
             pipe_info->os_spd_en[TDM_SPEED_IDX_40G] &&
             pipe_info->os_spd_en[TDM_SPEED_IDX_100G]) ||
            (pipe_info->os_spd_en[TDM_SPEED_IDX_10G] &&
             pipe_info->os_spd_en[TDM_SPEED_IDX_25G] &&
             pipe_info->os_spd_en[TDM_SPEED_IDX_50G] &&
             pipe_info->os_spd_en[TDM_SPEED_IDX_100G])) {
            TDM_PRINT0("TDM: Apply [Restriction 14]\n");
            for (i = 0; i < MAX_SPEED_TYPES; i++) {
                if (!(pipe_info->os_spd_en[i])) continue;
                if (i == TDM_SPEED_IDX_25G || i == TDM_SPEED_IDX_50G) {
                    for (j = 0; j < pipe_info->os_prt_cnt[i]; j++) {
                        prt    = pipe_info->os_prt[i][j];
                        prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
                        pm2spipe_map[prt_pm] = 0;
                    }
                } else if (i == TDM_SPEED_IDX_20G || i == TDM_SPEED_IDX_40G) {
                    for (j = 0; j < pipe_info->os_prt_cnt[i]; j++) {
                        prt    = pipe_info->os_prt[i][j];
                        prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
                        pm2spipe_map[prt_pm] = 1;
                    }
                }
            }
        } else {
            result = FAIL;
            TDM_ERROR1("%s, invalid config with 4 speed types\n",
                       "Failed in ovsb partition");
        }
    }
    /* Restriction 15: All port configurations with 1-3 port speed classes
     *                 are supported, except configurations that contain both
     *                 20G and 25G port speeds.
     * Rule: Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1.
     */
    if (num_spd_types > 1 && num_spd_types <= 3) {
        if ((pipe_info->os_spd_en[TDM_SPEED_IDX_20G] ||
             pipe_info->os_spd_en[TDM_SPEED_IDX_40G]) &&
            (pipe_info->os_spd_en[TDM_SPEED_IDX_25G] ||
             pipe_info->os_spd_en[TDM_SPEED_IDX_50G])) {
            TDM_PRINT0("Apply [Restriction 15]\n");
            for (i = 0; i < MAX_SPEED_TYPES; i++) {
                if (!(pipe_info->os_spd_en[i])) continue;
                if (i == TDM_SPEED_IDX_25G || i == TDM_SPEED_IDX_50G) {
                    for (j = 0; j < pipe_info->os_prt_cnt[i]; j++) {
                        prt    = pipe_info->os_prt[i][j];
                        prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
                        pm2spipe_map[prt_pm] = 0;
                    }
                } else if (i == TDM_SPEED_IDX_20G || i == TDM_SPEED_IDX_40G) {
                    for (j = 0; j < pipe_info->os_prt_cnt[i]; j++) {
                        prt    = pipe_info->os_prt[i][j];
                        prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
                        pm2spipe_map[prt_pm] = 1;
                    }
                }
            }
        }
    }

    return result;
}

/**
@name: tdm_td3_ovsb_apply_constraints_flex
@param:
@desc: [chip_specific]

Partition OVSB PMs into 2 halfpipes.
 */
int
tdm_td3_ovsb_apply_constraints_flex(tdm_mod_t *_tdm)
{
    int i, prt, prt_pm, num_spd_types = 0, result = PASS;
    int spipe, pos, grp, grp_lo = 0, grp_hi = 0;
    int param_cal_id, param_token_empty;
    int *pm2spipe_map = NULL;
    m_tdm_pipe_info_t *pipe_info;
    tdm_calendar_t *cal;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    pm2spipe_map = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe;
    pipe_info    = &(_tdm->_core_data.vars_pkg.pipe_info);

    /* get pipe calendar */
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return result;

    /* Restriction 13: No port configurations with more than 4 port speed
     *                 classes are supported.
     */
    for (i = 0; i < MAX_SPEED_TYPES; i++) {
        if (pipe_info->os_spd_en[i]) num_spd_types++;
    }
    if (num_spd_types > 4) {
        result = FAIL;
        TDM_ERROR4("%s, PIPE %d, num_spd_types %0d, limit %0d\n",
                   "Failed in ovsb partition, number of speed types overflow",
                   param_cal_id, num_spd_types, 4);
    }

    /* retrieve subpipe number for allocated PMs */
    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) {
        tdm_td3_ovsb_get_spipe_grp_range(_tdm, spipe, &grp_lo, &grp_hi);
        for (grp = grp_lo; grp <= grp_hi; grp++) {
            for (pos = 0; pos < cal->grp_len; pos++) {
                prt = cal->cal_grp[grp][pos];
                if (prt != param_token_empty) {
                    prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
                    pm2spipe_map[prt_pm] = spipe;
                }
            }
        }
    }

    return result;
}

/**
@name: tdm_td3_ovsb_init_pmlist
@param:
 */
void
tdm_td3_ovsb_init_pmlist(tdm_mod_t *_tdm, tdm_td3_ovsb_pm_t *pmlist, int pmlist_size)
{
    int i, spd_idx, pos, prt, pm, spd;
    m_tdm_pipe_info_t *pipe_info;

    /* reset pmlist */
    for (i = 0; i < pmlist_size; i++) {
        pmlist[i].pm_en      = 0;
        pmlist[i].pm_num     = 0;
        pmlist[i].pm_bw      = 0;
        pmlist[i].pm_max_spd = 0;
    }

    /* initialize pmlist based pipe_info */
    pipe_info = &(_tdm->_core_data.vars_pkg.pipe_info);
    for (spd_idx = MAX_SPEED_TYPES - 1; spd_idx >=0 ; spd_idx--) {
        if (pipe_info->os_spd_en[spd_idx]) {
            for (pos = 0; pos < pipe_info->os_prt_cnt[spd_idx]; pos++) {
                prt = pipe_info->os_prt[spd_idx][pos];
                pm  = tdm_td3_cmn_get_port_pm(_tdm, prt);
                spd = tdm_td3_cmn_get_port_speed(_tdm, prt);
                if (pm >= 0 && pm < pmlist_size && spd > 0) {
                    pmlist[pm].pm_en      = 1;
                    pmlist[pm].pm_num     = pm;
                    pmlist[pm].pm_bw     += spd;
                    pmlist[pm].pm_max_spd = (pmlist[pm].pm_max_spd < spd) ?
                                            spd : pmlist[pm].pm_max_spd;
                }
            }
        }
    }
}

/**
@name: tdm_td3_ovsb_swap_pm
@param:
 */
void
tdm_td3_ovsb_swap_pm(tdm_mod_t *_tdm, tdm_td3_ovsb_pm_t *pmlist, int src, int dst)
{
    tdm_td3_ovsb_pm_t pm_tmp;

    pm_tmp.pm_en      = pmlist[src].pm_en;
    pm_tmp.pm_num     = pmlist[src].pm_num;
    pm_tmp.pm_bw      = pmlist[src].pm_bw;
    pm_tmp.pm_max_spd = pmlist[src].pm_max_spd;

    pmlist[src].pm_en      = pmlist[dst].pm_en;
    pmlist[src].pm_num     = pmlist[dst].pm_num;
    pmlist[src].pm_bw      = pmlist[dst].pm_bw;
    pmlist[src].pm_max_spd = pmlist[dst].pm_max_spd;

    pmlist[dst].pm_en      = pm_tmp.pm_en;
    pmlist[dst].pm_num     = pm_tmp.pm_num;
    pmlist[dst].pm_bw      = pm_tmp.pm_bw;
    pmlist[dst].pm_max_spd = pm_tmp.pm_max_spd;
}

/**
@name: tdm_td3_ovsb_gen_pm2spipe_map_static
@param:
@desc: [chip_specific]

Partition each PM into a subpipe.
     -- pipe 0,  pm[0 - 7], port[1 -  32]: hpipe 0
     -- pipe 0,  pm[8 -15], port[33-  64]: hpipe 1
     -- pipe 1,  pm[16-23], port[65-  96]: hpipe 0
     -- pipe 1,  pm[24-31], port[97- 128]: hpipe 1
 */
int
tdm_td3_ovsb_gen_pm2spipe_map_static(tdm_mod_t *_tdm)
{
    int prt, pm, spipe_num, spd_idx, prt_idx;
    int pms_per_pipe, pms_per_spipe;
    int *pm2spipe_map = NULL;
    m_tdm_pipe_info_t *pipe_info;

    /* init variables */
    pm2spipe_map = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe;
    pipe_info    = &(_tdm->_core_data.vars_pkg.pipe_info);
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules /
                    TD3_NUM_PIPE;
    pms_per_spipe= pms_per_pipe / TD3_NUM_SPIPE;
    for (pm = 0; pm < TD3_NUM_PM_MOD; pm++) {
        pm2spipe_map[pm] = -1; /* -1:unassigned, 0:hp0, 1:hp1 */
    }

    /* partition PMs into subpipes */
    if (pms_per_pipe > 0 && pms_per_spipe > 0) {
        for (spd_idx = 0; spd_idx < MAX_SPEED_TYPES; spd_idx++) {
            if (pipe_info->os_spd_en[spd_idx]) {
                for (prt_idx = 0; prt_idx < pipe_info->os_prt_cnt[spd_idx];
                     prt_idx++) {
                    prt = pipe_info->os_prt[spd_idx][prt_idx];
                    pm  = tdm_td3_cmn_get_port_pm(_tdm, prt);
                    if (pm2spipe_map[pm] == -1) {
                        /* [chip_specific]: static mapping rule */
                        spipe_num = (pm % pms_per_pipe) / pms_per_spipe;
                        pm2spipe_map[pm] = spipe_num;
                        TDM_PRINT2("TDM: partition PM %2d to subpipe %0d\n",
                                   pm, spipe_num);
                    }
                }
            }
        }
    }
    TDM_SML_BAR

    return PASS;
}

/**
@name: tdm_td3_ovsb_gen_pm2spipe_map_dynamic
@param:

Partition each PM into a subpipe.
 */
int
tdm_td3_ovsb_gen_pm2spipe_map_dynamic(tdm_mod_t *_tdm)
{
    int i, j, spd_max, idx_src, idx_dst, pm, pm_idx, result = PASS;
    int spipe, dst_spipe, dst_spipe_bw, pms_per_spipe;
    int spipe_bw[TD3_NUM_SPIPE], spipe_pm_cnt[TD3_NUM_SPIPE];
    tdm_td3_ovsb_pm_t pmlist[TD3_NUM_PHY_PM];
    int *pm2spipe_map = NULL;

    /* initialize variables */
    pm2spipe_map  = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe;
    pms_per_spipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules /
                    TD3_NUM_PIPE / TD3_NUM_SPIPE;
    for (pm = 0; pm < TD3_NUM_PM_MOD; pm++) {
        pm2spipe_map[pm] = -1; /* -1:unassigned, 0:hp0, 1:hp1 */
    }
    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) {
        spipe_bw[spipe]     = 0;
        spipe_pm_cnt[spipe] = 0;
    }

    /* step1: apply constraints by grouping certain PMs into same subpipe */
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        if (tdm_td3_ovsb_apply_constraints_flex(_tdm) != PASS) result = FAIL;
    } else {
        if (tdm_td3_ovsb_apply_constraints_init(_tdm) != PASS) result = FAIL;
    }

    /* step2-1: construct pmlist */
    tdm_td3_ovsb_init_pmlist(_tdm, pmlist, TD3_NUM_PHY_PM);

    /* step2-2: calculate bw for each subpipe in prepartition */
    for (pm = 0; pm < TD3_NUM_PHY_PM && pm < TD3_NUM_PM_MOD; pm++) {
        spipe = pm2spipe_map[pm];
        if (spipe >= 0 && spipe < TD3_NUM_SPIPE) {
            spipe_bw[spipe] += pmlist[pm].pm_bw;
            spipe_pm_cnt[spipe]++;
        }
    }

    /* step2-3: sort pmlist by decreasing pm_max_speed (insert sort) */
    for (i = 0; i < TD3_NUM_PHY_PM; i++) {
        spd_max = 0;
        idx_src = i;
        idx_dst = i;
        for (j = i; j < TD3_NUM_PHY_PM; j++) {
            if (pmlist[j].pm_en && spd_max < pmlist[j].pm_max_spd) {
                spd_max = pmlist[j].pm_max_spd;
                idx_src = j;
            }
        }
        if (idx_src != idx_dst) tdm_td3_ovsb_swap_pm(_tdm, pmlist,
                                                     idx_src, idx_dst);
        if (idx_src == idx_dst && !pmlist[idx_src].pm_en) break;
    }

    /* step2-4: partition PMs into subpipes */
    for (pm_idx = 0; pm_idx < TD3_NUM_PHY_PM; pm_idx++) {
        if (!pmlist[pm_idx].pm_en) break;
        pm = pmlist[pm_idx].pm_num;
        if (!(pm >=0 && pm < TD3_NUM_PM_MOD)) continue;
        if (pm2spipe_map[pm] != -1) continue;
        /* apply worst-fit to partition PM into a subpipe */
        dst_spipe    = -1;
        dst_spipe_bw = 0;
        for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) {
            if (spipe_pm_cnt[spipe] < pms_per_spipe) {
                if (dst_spipe == -1 || spipe_bw[spipe] < dst_spipe_bw) {
                    dst_spipe   = spipe;
                    dst_spipe_bw= spipe_bw[spipe];
                }
            }
        }
        if (dst_spipe != -1) {
            spipe_bw[dst_spipe] += pmlist[pm_idx].pm_bw;
            pm2spipe_map[pm]     = dst_spipe;
            TDM_PRINT2("TDM: partition PM %2d to subpipe %0d\n",
                       pm, dst_spipe);
        } else {
            result = FAIL;
            TDM_ERROR1("Failed to map PM %0d to ovsb subpipe\n", pm);
        }
    }
    TDM_SML_BAR

    return result;
}

/**
@name: tdm_td3_ovsb_gen_pm2spipe_map
@param:
    -- mapping_mode: 0->static, 1->dynamic.
 */
int
tdm_td3_ovsb_gen_pm2spipe_map(tdm_mod_t *_tdm, int mapping_mode)
{
    if (mapping_mode == 0) {
        /* static mapping */
        return (tdm_td3_ovsb_gen_pm2spipe_map_static(_tdm));
    } else {
        /* dynamic mapping */
        return (tdm_td3_ovsb_gen_pm2spipe_map_dynamic(_tdm));
    }
}

/**
@name: tdm_td3_ovsb_add_prt_to_grp
@param:

Add port into given ovsb speed group.
If success, return pos of port in given speed group, otherwise return -1.
 */
int
tdm_td3_ovsb_add_prt_to_grp(tdm_mod_t *_tdm, int port_token, int grp)
{
    int pos;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    /* get pipe calendar */
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
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
@name: tdm_td3_ovsb_migrate_prt
@param:

Migrate all allocated same-speed sister ports of the given port from
current ovsb grp to an appropriate grp.
 */
int
tdm_td3_ovsb_migrate_prt(tdm_mod_t *_tdm, int port_token, int slot_req)
{
    int prt_buff[TD3_NUM_PM_LNS];
    int port_pm, port_spd;
    int prt, prt_sp, prt_cnt, prt_done;
    int grp, grp_lo, grp_hi, dst_grp, dst_pos, prev_grp, prev_pos;
    int pos;
    tdm_calendar_t *cal = NULL, *cal_prev = NULL;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    for (prt_cnt = 0; prt_cnt < TD3_NUM_PM_LNS; prt_cnt++) {
        prt_buff[prt_cnt] = param_token_empty;
    }

    /* get pipe calendar */
    cal      = tdm_td3_cmn_get_pipe_cal(_tdm);
    cal_prev = tdm_td3_cmn_get_pipe_cal_prev(_tdm);
    if (cal == NULL || cal_prev == NULL) return (-1);

    /* case 1: select group containing sister ports with the same speed */
    port_pm  = tdm_td3_cmn_get_port_pm(_tdm, port_token);
    port_spd = tdm_td3_cmn_get_port_speed_eth(_tdm, port_token);
    prt_sp   = tdm_td3_ovsb_get_prt_spipe(_tdm, port_token);
    tdm_td3_ovsb_get_spipe_grp_range(_tdm, prt_sp, &grp_lo, &grp_hi);

    /* move same-speed sister ports to prt_buff[] */
    prt_cnt = 0;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        if (tdm_td3_ovsb_get_grp_spd(_tdm, grp) != port_spd) continue;
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) {
                if (tdm_td3_cmn_get_port_pm(_tdm, prt) == port_pm) {
                    if (prt_cnt < TD3_NUM_PM_LNS) {
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
    dst_grp = tdm_td3_ovsb_sel_grp_4_prt(_tdm, port_token);
    if (dst_grp == -1) return (-1);

    /* move all ports in prt_buff[] to the selected group */
    if (dst_grp >= grp_lo && dst_grp <= grp_hi ) {
        for (prt_cnt = 0; prt_cnt < TD3_NUM_PM_LNS; prt_cnt++) {
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
@name: tdm_td3_ovsb_sel_grp_4_prt
@param:

Select an ovsb group to accommodate given port.
Return group index if found, otherwise return -1.
 */
int
tdm_td3_ovsb_sel_grp_4_prt(tdm_mod_t *_tdm, int port_token)
{
    int port_pm, port_spd;
    int prt, prt_pm, prt_spd, prt_sp, prt_done, prt_lo = 0, prt_hi = 0;
    int pos, grp, grp_lo, grp_hi, grp_num, grp_spd;
    int slot_req, slot_avail, slot_alloc;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    /* get pipe calendar */
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return (-1);

    /* get range of ovsb groups for the given port */
    prt_sp  = tdm_td3_ovsb_get_prt_spipe(_tdm, port_token);
    tdm_td3_ovsb_get_spipe_grp_range(_tdm, prt_sp, &grp_lo, &grp_hi);

    /* count number of ports with the same speed of given port within
       the same PM */
    slot_req = 0;
    port_pm  = tdm_td3_cmn_get_port_pm(_tdm, port_token);
    port_spd = tdm_td3_cmn_get_port_speed_eth(_tdm, port_token);
    tdm_td3_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if ((tdm_td3_cmn_get_port_speed(_tdm, prt) > 0) &&
            (BOOL_TRUE == tdm_td3_cmn_chk_port_is_os(_tdm, prt))) {
            prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
            if (prt_pm == port_pm) {
                prt_spd = tdm_td3_cmn_get_port_speed_eth(_tdm, prt);
                if (prt_spd == port_spd) slot_req++;
            }
        }
    }
    if (!(slot_req > 0)) return (-1);

    /* case 1: select group containing sister ports with the same speed */
    grp_num  = -1;
    prt_done = BOOL_FALSE;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        slot_alloc = 0;
        grp_spd = tdm_td3_ovsb_get_grp_spd(_tdm, grp);
        if (grp_spd != port_spd) continue;
        /* check if current group contains sister ports */
        for (pos = 0; pos < cal->grp_len; pos++) {
            prt = cal->cal_grp[grp][pos];
            if (prt != param_token_empty) {
                prt_pm = tdm_td3_cmn_get_port_pm(_tdm, prt);
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
                slot_avail = tdm_td3_ovsb_get_grp_empty_cnt(_tdm, grp_num);
                if (!(slot_avail >= (slot_req - slot_alloc))) {
                    grp_num = tdm_td3_ovsb_migrate_prt(_tdm, port_token,
                                                       slot_req);
                }
            }
            break;
        }
    }
    if (prt_done == BOOL_TRUE) return grp_num;

    /* case 2: select group containing same speed ports */
    grp_num = -1;
    prt_done = BOOL_FALSE;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        /* find same speed group, and check available resource */
        grp_spd = tdm_td3_ovsb_get_grp_spd(_tdm, grp);
        if (grp_spd == port_spd) {
            slot_avail = tdm_td3_ovsb_get_grp_empty_cnt(_tdm, grp);
            if (slot_avail >= slot_req) {
                grp_num  = grp;
                prt_done = BOOL_TRUE;
                break;
            }
        }
    }
    if (prt_done == BOOL_TRUE) return grp_num;

    /* case 3: select group containing no ports (empty group) */
    grp_num  = -1;
    prt_done = BOOL_FALSE;
    for (grp = grp_lo; grp <= grp_hi; grp++) {
        slot_avail = tdm_td3_ovsb_get_grp_empty_cnt(_tdm, grp);
        if (slot_avail == cal->grp_len) {
            grp_num  = grp;
            prt_done = BOOL_TRUE;
            break;
        }
    }
    if (prt_done == BOOL_TRUE) return grp_num;

    return (-1);
}

/**
@name: tdm_td3_ovsb_fill_os_grps
@param:
 */
int
tdm_td3_ovsb_fill_os_grps(tdm_mod_t *_tdm)
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
            spd = tdm_td3_cmn_get_port_speed(_tdm, prt);
            grp = tdm_td3_ovsb_sel_grp_4_prt(_tdm, prt);
            pos = tdm_td3_ovsb_add_prt_to_grp(_tdm, prt, grp);
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
@name: tdm_td3_ovsb_consolidate_spipe
@param:

Minimize the number of oversub speed groups of the given subpipe.
Note: All ports belonging to the same subpipe should be considered.
 */
int
tdm_td3_ovsb_consolidate_spipe(tdm_mod_t *_tdm, int spipe)
{
    int grp_lo, grp_hi, grp_dst, grp_src, grp_spd_dst, grp_spd_src,
        grp_size_dst, grp_size_src, idx_dst, idx_src, prt_src;
    int param_token_empty;
    tdm_calendar_t *cal;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;

    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return PASS;

    TDM_PRINT0("TDM: [FLEX] run consolidation\n");
    tdm_td3_ovsb_get_spipe_grp_range(_tdm, spipe, &grp_lo, &grp_hi);
    for (grp_dst = grp_lo; grp_dst < grp_hi; grp_dst++) {
        grp_spd_dst = tdm_td3_ovsb_get_grp_spd(_tdm, grp_dst);
        grp_size_dst= tdm_td3_ovsb_get_grp_prt_cnt(_tdm, grp_dst);
        /* for each non-empty group, do consolidation */
        if(grp_spd_dst == 0 || grp_size_dst == cal->grp_len) continue;
        for (grp_src = grp_dst + 1; grp_src <= grp_hi; grp_src++) {
            /* find two same speed groups */
            grp_spd_src = tdm_td3_ovsb_get_grp_spd(_tdm, grp_src);
            grp_size_src= tdm_td3_ovsb_get_grp_prt_cnt(_tdm, grp_src);
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
@name: tdm_td3_ovsb_flex_port_dn
@param:

Remove down ports from ovsb speed groups
 */
int
tdm_td3_ovsb_flex_port_dn(tdm_mod_t *_tdm)
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
    tdm_td3_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);
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
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
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
@name: tdm_td3_ovsb_flex_port_up
@param:

Add up ports into ovsb speed groups
 */
int
tdm_td3_ovsb_flex_port_up(tdm_mod_t *_tdm)
{
    int prt_lo, prt_hi, prt, prt_spd, pos, grp, spipe, result = PASS;
    int spipe_done[TD3_NUM_SPIPE];
    int flex_up_exist = BOOL_FALSE;
    enum port_state_e *param_states_prev;

    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;
    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) spipe_done[spipe] = 0;
    tdm_td3_cmn_get_pipe_port_lo_hi(_tdm, &prt_lo, &prt_hi);

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

    /* do consolidation under conflict */
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_UP ||
            param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
            spipe = tdm_td3_ovsb_get_prt_spipe(_tdm, prt);
            if (spipe >= 0 && spipe < TD3_NUM_SPIPE) {
                if (spipe_done[spipe] == 0) {
                    tdm_td3_ovsb_consolidate_spipe(_tdm, spipe);
                    spipe_done[spipe] = 1;
                }
            }
        }
    }

    /* add up_ports and spd_changed_ports */
    for (prt = prt_lo; prt <= prt_hi; prt++) {
        if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_UP ||
            param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
            prt_spd = tdm_td3_cmn_get_port_speed(_tdm, prt);
            grp = tdm_td3_ovsb_sel_grp_4_prt(_tdm, prt);
            pos = tdm_td3_ovsb_add_prt_to_grp(_tdm, prt, grp);
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
@name: tdm_td3_ovsb_gen_os_grps_init
@param:
 */
int
tdm_td3_ovsb_gen_os_grps_init(tdm_mod_t *_tdm)
{
    int result = PASS;

    /* partition PMs into subpipes */
    if (tdm_td3_ovsb_gen_pm2spipe_map(_tdm, 0) != PASS) result = FAIL;

    /* allocate ports into ovsb groups based on partitioned subpipes */
    if (tdm_td3_ovsb_fill_os_grps(_tdm) != PASS) result = FAIL;

    return result;
}

/**
@name: tdm_td3_ovsb_gen_os_grps_flex
@param:
 */
int
tdm_td3_ovsb_gen_os_grps_flex(tdm_mod_t *_tdm)
{
    int grp, pos, result = PASS;
    tdm_calendar_t *cal = NULL, *cal_prev = NULL;

    /* step 1: copy ovsb grps from cal_prev into cal */
    TDM_PRINT0("TDM: Restore ovsb grps by pre_flex values\n");
    cal      = tdm_td3_cmn_get_pipe_cal(_tdm);
    cal_prev = tdm_td3_cmn_get_pipe_cal_prev(_tdm);
    if (cal == NULL || cal_prev == NULL) return FAIL;
    for (grp = 0; grp < cal->grp_num; grp++) {
        for (pos=0; pos<cal->grp_len; pos++) {
            cal->cal_grp[grp][pos] = cal_prev->cal_grp[grp][pos];
        }
    }

    /* step 2: partition PMs into subpipes */
    if (tdm_td3_ovsb_gen_pm2spipe_map(_tdm, 0) != PASS) result = FAIL;

    /* step 3: flex up-ports */
    if (tdm_td3_ovsb_flex_port_dn(_tdm) != PASS) result = FAIL;

    /* step 4: flex down-ports */
    if (tdm_td3_ovsb_flex_port_up(_tdm) != PASS) result = FAIL;

    return result;
}

/**
@name: tdm_td3_ovsb_gen_os_grps
@param:
 */
int
tdm_td3_ovsb_gen_os_grps(tdm_mod_t *_tdm)
{
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        return (tdm_td3_ovsb_gen_os_grps_flex(_tdm));
    } else {
        return (tdm_td3_ovsb_gen_os_grps_init(_tdm));
    }
}

/**
@name: tdm_td3_ovsb_gen_pkt_shaper_per_prt
@param:

Populates Pkt shaper calendar - per port
 */
int
tdm_td3_ovsb_gen_pkt_shaper_per_prt(tdm_mod_t *_tdm, int prt)
{
    int i, pos, prt_slots, prt_pm, prt_spipe, slot_cnt, result = PASS;
    int pm_shaper_tbl[TD3_SHAPER_NUM_PM_SLOTS];
    int *pkt_shaper_cal, *pm2spipe_map;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    pm2spipe_map = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe;

    for (i = 0; i < TD3_SHAPER_NUM_PM_SLOTS; i++)
        pm_shaper_tbl[i] = param_token_empty;

    /* generate pm shaper calendar for the current port */
    prt_slots = tdm_td3_cmn_get_port_speed_eth(_tdm, prt) /
                TD3_SHAPER_SLOT_UNIT;
    for (slot_cnt = 0; slot_cnt < prt_slots; slot_cnt++) {
        pos = tdm_td3_ovsb_get_pm_shaper_prt_pos(_tdm, prt, slot_cnt) %
              TD3_SHAPER_NUM_PM_SLOTS;
        pm_shaper_tbl[pos] = prt;
    }

    TDM_PRINT1("TDM: prt %3d, pm_shaper_cal[", prt);
    for (pos = 0; pos < TD3_SHAPER_NUM_PM_SLOTS; pos++) {
        TDM_PRINT2("%0d:%0d, ", pos, pm_shaper_tbl[pos]);
    }
    TDM_PRINT0("]\n");

    /* Get the right pkt scheduler calendar*/
    prt_pm    = tdm_td3_cmn_get_port_pm(_tdm, prt);
    prt_spipe = pm2spipe_map[prt_pm];
    pkt_shaper_cal = tdm_td3_ovsb_get_pkt_shaper_cal(_tdm, prt_spipe);
    if (pkt_shaper_cal == NULL) return FAIL;

    /* Populate pkt scheduler calendar */
    slot_cnt = 0;
    for (i = 0; i < TD3_SHAPER_NUM_PM_SLOTS; i++) {
        if (pm_shaper_tbl[i] != param_token_empty) {
            pos = tdm_td3_ovsb_get_pkt_shaper_prt_pos(_tdm, prt, i);
            slot_cnt++;
            if (pos >=0 && pos < TD3_SHAPING_GRP_LEN) {
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
@name: tdm_td3_ovsb_gen_pkt_shaper_init
@param:

Populates Pkt shaper calendar
 */
int
tdm_td3_ovsb_gen_pkt_shaper_init(tdm_mod_t *_tdm)
{
    int spipe, grp, grp_lo, grp_hi, pos, prt;
    tdm_calendar_t *cal;
    int param_token_empty;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return FAIL;

    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) {
        tdm_td3_ovsb_get_spipe_grp_range(_tdm, spipe, &grp_lo, &grp_hi);
        for (grp = grp_lo; grp <= grp_hi; grp++) {
            for (pos = 0; pos < cal->grp_len; pos++) {
                prt = cal->cal_grp[grp][pos];
                if (prt != param_token_empty)
                    tdm_td3_ovsb_gen_pkt_shaper_per_prt(_tdm, prt);
            }
        }
    }
    TDM_SML_BAR

    return PASS;
}

/**
@name: tdm_td3_ovsb_gen_pkt_shaper_flex
@param:

Populates Pkt shaper calendar
 */
int
tdm_td3_ovsb_gen_pkt_shaper_flex(tdm_mod_t *_tdm)
{
    int i, spipe, grp, grp_lo, grp_hi, pos, prt;
    int port_done[TD3_NUM_EXT_PORTS];
    tdm_calendar_t *cal;
    int *pkt_cal, *pkt_cal_prev;
    int param_token_empty;
    enum port_state_e *param_states_prev;

    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;
    cal = tdm_td3_cmn_get_pipe_cal(_tdm);
    if (cal == NULL) return FAIL;
    for (i = 0; i < TD3_NUM_EXT_PORTS; i++) port_done[i] = 0;

    /* step 1: copy pkt shaper calendar from pkt_cal_prev into pkt_cal */
    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) {
        pkt_cal      = tdm_td3_ovsb_get_pkt_shaper_cal(_tdm, spipe);
        pkt_cal_prev = tdm_td3_ovsb_get_pkt_shaper_cal_prev(_tdm, spipe);
        if (pkt_cal == NULL || pkt_cal_prev == NULL) continue;
        for (i = 0; i < TD3_SHAPING_GRP_LEN; i++) {
            prt = pkt_cal_prev[i];
            if (param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_DN ||
                param_states_prev[prt - 1] == PORT_STATE__FLEX_OVERSUB_CH) {
                prt = param_token_empty;
            }
            pkt_cal[i] = prt;
            if (prt!=param_token_empty && prt>0 && prt<TD3_NUM_EXT_PORTS) {
                port_done[prt] = 1;
            }
        }
    }

    /* step 2: allocate slots for up-ports in pkt shaper calendar */
    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) {
        tdm_td3_ovsb_get_spipe_grp_range(_tdm, spipe, &grp_lo, &grp_hi);
        for (grp = grp_lo; grp <= grp_hi; grp++) {
            for (pos = 0; pos < cal->grp_len; pos++) {
                prt = cal->cal_grp[grp][pos];
                if (prt != param_token_empty &&
                    prt > 0 && prt < TD3_NUM_EXT_PORTS) {
                    if (port_done[prt] == 0) {
                        tdm_td3_ovsb_gen_pkt_shaper_per_prt(_tdm, prt);
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
@name: tdm_td3_ovsb_gen_pkt_shaper
@param:

Populates Pkt shaper calendar
 */
int
tdm_td3_ovsb_gen_pkt_shaper(tdm_mod_t *_tdm)
{
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        return (tdm_td3_ovsb_gen_pkt_shaper_flex(_tdm));
        /* return (tdm_td3_ovsb_gen_pkt_shaper_init(_tdm)); */
    } else {
        return (tdm_td3_ovsb_gen_pkt_shaper_init(_tdm));
    }

    return PASS;
}

/**
@name: tdm_td3_ovsb_gen_pm2pblk_map
@param:
@desc: [chip_specific]

Maps PM num (block_id) to OVSB pblk id
 */
int
tdm_td3_ovsb_gen_pm2pblk_map(tdm_mod_t *_tdm)
{
    int pm, pm_lo, pm_hi, spipe;
    int pblk_idx[TD3_NUM_SPIPE];
    int *pm2pblk_map = NULL, *pm2spipe_map = NULL;

    pm2pblk_map  = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_num_to_pblk;
    pm2spipe_map = _tdm->_chip_data.soc_pkg.soc_vars.td3.pm_ovs_halfpipe;
    tdm_td3_cmn_get_pipe_pm_lo_hi(_tdm, &pm_lo, &pm_hi);
    for (spipe = 0; spipe < TD3_NUM_SPIPE; spipe++) pblk_idx[spipe] = 0;

    for (pm = pm_lo; pm <= pm_hi && pm < TD3_NUM_PM_MOD; pm++) {
        pm2pblk_map[pm] = -1;
        spipe = pm2spipe_map[pm];
        if (spipe >= 0 && spipe < TD3_NUM_SPIPE) {
            /* [chip_specific] halpipe 0 uses increment order: [0,1,...7]
             *                 halpipe 1 uses decrement order: [7,6,...0]
             */
            if (spipe == 0) {
                pm2pblk_map[pm] = pblk_idx[spipe];
            } else {
                pm2pblk_map[pm] = TD3_NUM_PMS_PER_SPIPE - pblk_idx[spipe] - 1;
            }
            pblk_idx[spipe]++;
            TDM_PRINT3("TDM: map pm %2d to subpipe %0d pblk %0d\n",
                       pm, spipe, pm2pblk_map[pm]);
        }
    }
    TDM_SML_BAR

    return PASS;
}

/**
@name: tdm_td3_vbs_scheduler_ovs_2
@param:
 */
int
tdm_td3_vbs_scheduler_ovs(tdm_mod_t *_tdm)
{
    int result = PASS;
    int param_cal_id = _tdm->_core_data.vars_pkg.cal_id;

    /* generate ovsb groups */
    TDM_PRINT1("Generating OVSB groups for PIPE %0d\n\n", param_cal_id);
    if (tdm_td3_ovsb_gen_os_grps(_tdm) != PASS) result = FAIL;

    /* generate PKT scheduler */
    TDM_PRINT1("Generating OVSB PKT Scheduler for PIPE %0d\n\n", param_cal_id);
    /* if (tdm_td3_ovsb_pkt_shaper_new(_tdm) != PASS) result = FAIL; */
    if (tdm_td3_ovsb_gen_pkt_shaper(_tdm) != PASS) result = FAIL;

    /* generate PM_NUM to PBLK mapping */
    TDM_PRINT1("Generating OVSB PM2PBLK mapping for PIPE %0d\n\n", param_cal_id);
    if(tdm_td3_ovsb_gen_pm2pblk_map(_tdm) != PASS) result = FAIL;

    return result;
}
