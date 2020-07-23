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

#define TDM_MV2_HPIPE_STATIC_MAPPING_ENABLE 1


/**
@name: tdm_mv2_ovsb_apply_constraints
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_mv2_ovsb_apply_constraints(tdm_mod_t *_tdm)
{
    int pm_num, ln_num, i, pm_num_lo, pm_num_hi, pms_per_pipe;
    int num_of_speeds, phy_base_port, phy_port;
    int speed_en_mtx[MV2_NUM_PHY_PM][MV2_PORT_SPEED_INDX_SIZE];
    int speed_en_mtx_reduced[MV2_PORT_SPEED_INDX_SIZE];
    /* <speed_indx> 0: 10G; 1: 20G; 2: 25G; 3: 40G; 4: 50G; 5: 100G */
    int param_pipe_id,param_pm_lanes, param_pm_num;
    int *param_pm_hpipe;
    enum port_speed_e *param_speeds;
    int result = PASS;

    param_pipe_id  = _tdm->_core_data.vars_pkg.cal_id;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_num   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_hpipe = _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;

    pms_per_pipe = param_pm_num / MV2_NUM_PIPE;
    pm_num_lo = param_pipe_id * pms_per_pipe;
    pm_num_hi = (param_pipe_id + 1) * pms_per_pipe - 1;

    /* initialize variables */
    for (i=0; i<MV2_PORT_SPEED_INDX_SIZE; i++) {
        for (pm_num=0; pm_num<MV2_NUM_PHY_PM; pm_num++) {
            speed_en_mtx[pm_num][i]=0;
        }
        speed_en_mtx_reduced[i]=0;
    }
    for (pm_num = pm_num_lo; pm_num <= pm_num_hi; pm_num++) {
        /* Coding rule for param_pm_hpipe:
         *       -1 : not OVSB;
         *        2 : OVSB but not assigned
         *        0 : Half Pipe 0
         *        1 : Half Pipe 1
         */
        param_pm_hpipe[pm_num] = -1;
        phy_base_port = MV2_NUM_PM_LNS * pm_num + 1;
        for (ln_num = 0; ln_num < param_pm_lanes; ln_num++) {
            phy_port = phy_base_port + ln_num;
            if (tdm_mv2_cmn_chk_port_is_os(phy_port, _tdm) == BOOL_TRUE) {
                param_pm_hpipe[pm_num] = 2;
                break;
            }
        }
    }

    /* Construct speed_en_mtx matrix */
    for (pm_num = pm_num_lo; pm_num <= pm_num_hi; pm_num++) {
        if (param_pm_hpipe[pm_num] == 2) {
            phy_base_port = MV2_NUM_PM_LNS * pm_num + 1;
            for (ln_num = 0; ln_num < param_pm_lanes; ln_num++) {
                phy_port = phy_base_port + ln_num;
                if (tdm_mv2_cmn_chk_port_is_os(phy_port, _tdm) == BOOL_TRUE) {
                    switch(param_speeds[phy_port]) {
                        case SPEED_10G : 
                            speed_en_mtx[pm_num][MV2_SPEED_IDX_10G] = 1; break;
                        case SPEED_20G : 
                            speed_en_mtx[pm_num][MV2_SPEED_IDX_20G] = 1; break;
                        case SPEED_25G : 
                            speed_en_mtx[pm_num][MV2_SPEED_IDX_25G] = 1; break;
                        case SPEED_40G : 
                            speed_en_mtx[pm_num][MV2_SPEED_IDX_40G] = 1; break;
                        case SPEED_50G : 
                            speed_en_mtx[pm_num][MV2_SPEED_IDX_50G] = 1; break;
                        case SPEED_100G : 
                            speed_en_mtx[pm_num][MV2_SPEED_IDX_100G]= 1; break;
                        default : break;
                    }
                }
            }
            for(i = 0; i < MV2_PORT_SPEED_INDX_SIZE; i++) {
                speed_en_mtx_reduced[i] = (speed_en_mtx[pm_num][i] == 1) ? 1 :
                                          speed_en_mtx_reduced[i];
            }
        }
    }

    num_of_speeds = 0;
    for (i = 0; i < MV2_PORT_SPEED_INDX_SIZE; i++) {
        if (speed_en_mtx_reduced[i] > 0) {
            num_of_speeds++;
        }
    }

    /* Restriction 13:
     * No port configs with more than 4 port speed classes are supported.
     */
    if (num_of_speeds > 5) {
        result = FAIL;
        TDM_ERROR3("%s PIPE %d, speed_types=%d, max_speed_types=4\n",
                   "[ovsb_halfpipe_constraints] Invalid port config,",
                   param_pipe_id, num_of_speeds);
    }
    /* Restriction 14:
     * The only supported port configs with 4 port speed classes are:
     *       10G/20G/40G/100G
     *       10G/25G/50G/100G
     * Rule: Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1.
     */
    else if (num_of_speeds == 4) {
        if ((speed_en_mtx_reduced[MV2_SPEED_IDX_20G] ||
             speed_en_mtx_reduced[MV2_SPEED_IDX_40G]) &&
            (speed_en_mtx_reduced[MV2_SPEED_IDX_25G] ||
             speed_en_mtx_reduced[MV2_SPEED_IDX_50G])) {
            TDM_PRINT2("%s PIPE %d applying Restriction 14 \n",
                       "[ovsb_halfpipe_constraints]", param_pipe_id);
            for (pm_num = pm_num_lo; pm_num <= pm_num_hi; pm_num++) {
                if (speed_en_mtx[pm_num][MV2_SPEED_IDX_25G] ||
                    speed_en_mtx[pm_num][MV2_SPEED_IDX_50G]) {
                    param_pm_hpipe[pm_num] = 0;
                } else if (speed_en_mtx[pm_num][MV2_SPEED_IDX_20G] ||
                           speed_en_mtx[pm_num][MV2_SPEED_IDX_40G]) {
                    param_pm_hpipe[pm_num] = 1;
                }
            }
        }
    }
    /* Restriction 15:
     * All port configs with 1-3 port speed classes are supported,
     * except configs that contain both 20G and 25G port speeds.
     * Rule: Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1
     */
    else if (num_of_speeds > 1) {
        if (speed_en_mtx_reduced[MV2_SPEED_IDX_20G] &&
            speed_en_mtx_reduced[MV2_SPEED_IDX_25G]) {
            TDM_PRINT2("%s PIPE %d applying Restriction 15 \n",
                       "[ovsb_halfpipe_constraints]", param_pipe_id);
            for (pm_num = pm_num_lo; pm_num <= pm_num_hi; pm_num++) {
                if (speed_en_mtx[pm_num][MV2_SPEED_IDX_25G]) {
                    param_pm_hpipe[pm_num] = 0;
                } else if (speed_en_mtx[pm_num][MV2_SPEED_IDX_20G]) {
                    param_pm_hpipe[pm_num] = 1;
                }
            }
        }
    }
    else {
        /* Do nothing; no constraints # of speeds is 0*/
    }

    return (result);
}


/**
@name: tdm_mv2_ovsb_pm_2_hpipe_mapping
@param:
     0: half pipe 0 
     1: half pipe 0 
    -1: invalid
Return halfpipe number of the given port (STATIC MAPPING)
 */
int
tdm_mv2_ovsb_pm_2_hpipe_mapping(int pipe_id, int port)
{
    int hpipe_num = -1, port_lo, port_hi;

    /*  Static PM_2_HalfPipe mapping for MV2
     *      Pipe 0 -- halfpipe 0:  1 - 40
     *      Pipe 0 -- halfpipe 1: 41 - 80
     */
    if (pipe_id < MV2_NUM_PIPE) {
        port_lo = pipe_id * MV2_NUM_PHY_PORTS_PER_PIPE + 1;
        port_hi = (pipe_id + 1) * MV2_NUM_PHY_PORTS_PER_PIPE;
        if (port >= port_lo && port <= port_hi) {
            hpipe_num = ((port - 1) % MV2_NUM_PHY_PORTS_PER_PIPE) /
                        MV2_NUM_PHY_PORTS_PER_HPIPE;
        }
    }
    if (hpipe_num != 0 && hpipe_num != 1) {
        TDM_PRINT3("WARNING: Invalid hpipe_num %0d for port %d in pipe %d\n",
                    hpipe_num, port, pipe_id);
    }
    return hpipe_num;
}


/**
@name: tdm_mv2_ovsb_part_halfpipe
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_mv2_ovsb_part_halfpipe(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id, tmp;
    int pm_speed[MV2_NUM_PMS_PER_PIPE];
    int pm_num_subports[MV2_NUM_PMS_PER_PIPE];
    int pm_num_sort[MV2_NUM_PMS_PER_PIPE];
    int pm_num, ln_num, pm_indx, i, j;
    int phy_base_port, phy_port;
    int hp0_speed, hp1_speed;
    int no_pms_hp0, no_pms_hp1;
    int *param_pm_hpipe;
    enum port_speed_e *param_speeds;

    param_pm_hpipe = _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/MV2_NUM_PIPE;

    for (pm_indx = 0; pm_indx < MV2_NUM_PMS_PER_PIPE; pm_indx++) {
        pm_speed[pm_indx] = 0;
        pm_num_subports[pm_indx] = 0;
        pm_num_sort[pm_indx] = 0;
    }
    TDM_PRINT2("tdm_mv2_avs_part_halfpipe1() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);

    /* Calculate PM speeds for all PMs in the current pipe */
    pm_indx=0;
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        phy_base_port = MV2_NUM_PM_LNS*pm_num+1;
        for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = phy_base_port+ln_num;
            if (tdm_mv2_cmn_chk_port_is_os(phy_port, _tdm) == BOOL_TRUE) {
                TDM_PRINT5("%s pm_num=%2d ln_num=%0d port=%3d speed=%0dG\n",
                           "[ovsb_part_pm]",
                           pm_num, ln_num, phy_port,
                           param_speeds[phy_port]/1000);
                pm_speed[pm_indx] = pm_speed[pm_indx] + param_speeds[phy_port]/1000;
                pm_num_subports[pm_indx]++;
            }
        }
        pm_num_sort[pm_indx] = pm_num;
        pm_indx = (pm_indx + 1) % MV2_NUM_PMS_PER_PIPE;
    }
    /* print PM speed info and lanes info */
    for (pm_indx = 0; pm_indx < MV2_NUM_PMS_PER_PIPE; pm_indx++) {
        TDM_PRINT5("%s pm_indx=%2d pm_num=%2d pm_speed=%3dG pm_lanes=%d\n",
                   "[ovsb_part_pm]",
                   pm_indx, pm_num_sort[pm_indx], pm_speed[pm_indx],
                   pm_num_subports[pm_indx]);
    }

    /* Sort speeds of the PMs in descending order - bubble sort*/
    for (i=0; i<pms_per_pipe-1; i++) {
        for (j=pms_per_pipe-1; j>i; j--) {
            /* swap j with j-1*/
            if ((pm_speed[j] > pm_speed[j-1]) ||
                ((pm_speed[j] == pm_speed[j-1]) &&
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
        if (param_pm_hpipe[pm_num] == 0) {
            hp0_speed += pm_speed[i];
            no_pms_hp0++;
        }
        if (param_pm_hpipe[pm_num] == 1) {
            hp1_speed += pm_speed[i];
            no_pms_hp1++;
        }
    }

    /* Do partition of the PMs into two half pipes */
    for (i=0; i<pms_per_pipe; i++) {
        pm_num = pm_num_sort[i];
        if (param_pm_hpipe[pm_num] == 2) { /* is OVSB unallocated PM */
            if (((no_pms_hp1 < (pms_per_pipe/2)) && (hp0_speed > hp1_speed)) ||
                (no_pms_hp0 >= (pms_per_pipe/2)) ) {
                hp1_speed = hp1_speed + pm_speed[i];
                param_pm_hpipe[pm_num] = 1;
                no_pms_hp1++;
            } else {
                hp0_speed = hp0_speed + pm_speed[i];
                param_pm_hpipe[pm_num] = 0;
                no_pms_hp0++;
            }
        }
    }
    TDM_PRINT2("[ovsb_part_pm] hp0_bw=%d hp1_bw=%d\n",
               hp0_speed, hp1_speed);
    return PASS;
}


/**
@name: tdm_mv2_ovsb_fill_group
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_mv2_ovsb_fill_group(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id, tmp;
    int pm_num_subports[MV2_NUM_PMS_PER_PIPE];
    int pm_num_sort[MV2_NUM_PMS_PER_PIPE];
    int pm_num, ln_num, pm_indx, i, j;

    int pms_with_grp_speed;
    int grp_speed;
    int half_pipe_num;
    int speed_max_num_ports_per_pm;
    int min_num_ovs_groups;
    tdm_calendar_t *cal;
    int start_ovs_group;
    
    int start_group;
    int grp_index[MV2_OS_VBS_GRP_NUM];
    int phy_base_port;
    int phy_port;
    
    int t_ovs_grp_num;
    
    pipe_id       = _tdm->_core_data.vars_pkg.cal_id;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num;
    grp_speed     = _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed;
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/MV2_NUM_PIPE;

    for (pm_indx = 0; pm_indx < MV2_NUM_PMS_PER_PIPE; pm_indx++) {
        pm_num_subports[pm_indx] = 0;
        pm_num_sort[pm_indx] = 0;
    }

    /* Count the number of subports per PM with grp_speed for PMs that belongs to this half pipe*/
    pm_indx=0;
    pms_with_grp_speed = 0;
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        phy_base_port = MV2_NUM_PM_LNS*pm_num+1;
        if (_tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe[pm_num] == half_pipe_num){
            for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
                phy_port = phy_base_port+ln_num;
                if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
                    pm_num_subports[pm_indx]++;
                }
            }
            if (pm_num_subports[pm_indx] > 0) {pms_with_grp_speed++;}
        }
        pm_num_sort[pm_indx] = pm_num;
        pm_indx = (pm_indx + 1) % MV2_NUM_PMS_PER_PIPE;
    }

    /* Compute  the minimum number of oversub groups required for the group speed*/
    switch (grp_speed) {
        case SPEED_10G:	 speed_max_num_ports_per_pm=4; break;
        case SPEED_20G:	 speed_max_num_ports_per_pm=2; break;
        case SPEED_40G:	 speed_max_num_ports_per_pm=2; break;
        case SPEED_25G:	 speed_max_num_ports_per_pm=4; break;
        case SPEED_50G:	 speed_max_num_ports_per_pm=2; break;
        case SPEED_100G: speed_max_num_ports_per_pm=1; break;
        default:													
            TDM_PRINT1("[speed group partition] Invalid group speed %0d\n",
                       grp_speed);
            return FAIL;							
    }
    /* ceil [(speed_max_num_ports_per_pm*pms_with_grp_speed) / MV2_OS_VBS_GRP_LEN ]*/
    min_num_ovs_groups = ((speed_max_num_ports_per_pm*pms_with_grp_speed) + MV2_OS_VBS_GRP_LEN-1) / MV2_OS_VBS_GRP_LEN;

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
                TDM_PRINT1("[speed group partition] Invalid calendar ID - %0d\n",
                            _tdm->_core_data.vars_pkg.cal_id);
                return FAIL;
        }
        start_ovs_group = half_pipe_num*(MV2_OS_VBS_GRP_NUM/2);
        start_group = start_ovs_group;
        for (i=start_group; i< (start_group+(MV2_OS_VBS_GRP_NUM/2)); i++) {
            if (cal->cal_grp[i][0] != MV2_NUM_EXT_PORTS) {
                start_ovs_group++;
            } else {
                break;
            }
        }
        
        /* Fill ovs groups with ports having grp_speed*/
        for (i=0; i< MV2_OS_VBS_GRP_NUM; i++) { grp_index[i] = 0;}
        for (i=0; i<pms_with_grp_speed; i++) {
            pm_num = pm_num_sort[i];
            phy_base_port = MV2_NUM_PM_LNS*pm_num+1;
            if (_tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe[pm_num] == half_pipe_num){
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
@name: tdm_mv2_ovsb_pkt_shaper_per_pm
@param:

Populates Pkt shaper calendar - per PM
 */
int
tdm_mv2_ovsb_pkt_shaper_per_pm(tdm_mod_t *_tdm, int shift_step)
{
    int pms_per_pipe, pms_per_hpipe, pms_per_hpipe_max;
    int pm_num, i, pm_done;
    int port_phy;
    int half_pipe_num;

    int pkt_shpr_pm_indx;
    int pm_slot_tbl[MV2_NUM_PKT_SLOTS_PER_PM];
    int *pkt_shed_cal;
    int pkt_sched_repetitions;

    int pm_port_lo, block_num, lane_offset, pm_offset, block_offset;
    int lane, cal_pos, tbl_offset;
    enum port_speed_e *param_speeds;
    int result = PASS;

    param_speeds   = _tdm->_chip_data.soc_pkg.speed;

    port_phy = _tdm->_core_data.vars_pkg.port;
    pm_num = (port_phy-1)/MV2_NUM_PM_LNS;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe[pm_num];

    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/MV2_NUM_PIPE;
    pms_per_hpipe = pms_per_pipe / 2;
    pms_per_hpipe_max = MV2_SHAPING_GRP_LEN / MV2_NUM_PKT_SLOTS_PER_PM;
    /* constraint: max PMs for pkt_scheduler, x*20 <= 160 */
    if (pms_per_hpipe > pms_per_hpipe_max) {
        pms_per_hpipe = pms_per_hpipe_max;
    }

    /* number of slots per port: 5 = 200/10/4 (MV2) */
    pkt_sched_repetitions = MV2_SHAPING_GRP_LEN/pms_per_hpipe/MV2_NUM_PM_LNS;

    for (i=0; i<MV2_NUM_PKT_SLOTS_PER_PM; i++) {
        pm_slot_tbl[i] = MV2_NUM_EXT_PORTS;
    }

    /* Determine the pm_indx in pkt shceduler calendar - 0 to 7*/
    pkt_shpr_pm_indx = tdm_mv2_ovsb_pkt_shaper_find_pm_indx(_tdm);
    if (pkt_shpr_pm_indx < 0) {
        return FAIL;
    }

    /* allocate slots for PM */
    pm_done = BOOL_TRUE;
    pm_port_lo = MV2_NUM_PM_LNS * pm_num + 1;
    switch (param_speeds[port_phy]) {
        case SPEED_10G:
            /* 10G SINGLE */
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                tbl_offset = 0;
                pm_slot_tbl[(tbl_offset + 0) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 8) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
            }
            /* 10G DUAL */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_10G) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                tbl_offset = 0;
                pm_slot_tbl[(tbl_offset + 0 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 10) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 5 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 15) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
            }
            /* 10G QUAD */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_10G) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_10G) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_10G)) {
                tbl_offset = (shift_step % 5) * 4;
                /* tbl_offset = 0; */
                pm_slot_tbl[(tbl_offset + 0 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 8 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 10) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 1;
                pm_slot_tbl[(tbl_offset + 18) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 1;
                pm_slot_tbl[(tbl_offset + 5 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 13) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 3 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 3;
                pm_slot_tbl[(tbl_offset + 15) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 3;
            }
            else {
                pm_done = BOOL_FALSE;
            }
            break;
        case SPEED_20G:
            /* 20G SINGLE */
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                tbl_offset = 0;
                pm_slot_tbl[(tbl_offset + 0 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 4 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 10) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 14) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
            }
            /* 20G DUAL */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_20G) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                pm_slot_tbl[(0 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(4 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(10) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(14) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(1 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(5 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(11) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(15) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
            }
            else {
                pm_done = BOOL_FALSE;
            }
            break;
        case SPEED_40G:
            /* 40G SINGLE: 40G_0_0_0 */
            if (param_speeds[pm_port_lo]     == SPEED_40G &&
                param_speeds[pm_port_lo + 1] == SPEED_0   &&
                param_speeds[pm_port_lo + 2] == SPEED_0   &&
                param_speeds[pm_port_lo + 3] == SPEED_0) {
                tbl_offset = 0;
                pm_slot_tbl[(tbl_offset + 0 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 2 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 4)  % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 6)  % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 10) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 12) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 14) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 16) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
            }
            /* 40G SINGLE: 0_0_40G_0 */
            else if (param_speeds[pm_port_lo]     == SPEED_0   &&
                     param_speeds[pm_port_lo + 1] == SPEED_0   &&
                     param_speeds[pm_port_lo + 2] == SPEED_40G &&
                     param_speeds[pm_port_lo + 3] == SPEED_0) {
                tbl_offset = 0;
                pm_slot_tbl[(tbl_offset + 1 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 3 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 5 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 7 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 11) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 13) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 15) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 17) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
            }
            /* 40G DUAL */
            else if (param_speeds[pm_port_lo]     == SPEED_40G &&
                     param_speeds[pm_port_lo + 1] == SPEED_0   &&
                     param_speeds[pm_port_lo + 2] == SPEED_40G &&
                     param_speeds[pm_port_lo + 3] == SPEED_0) {
                tbl_offset = 0;
                pm_slot_tbl[(tbl_offset + 0 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 2 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 4) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 6) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 10) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 12) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 14) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 16) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                pm_slot_tbl[(tbl_offset + 1 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 3 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 5 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 7 ) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 11) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 13) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 15) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                pm_slot_tbl[(tbl_offset + 17) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
            }
            else {
                pm_done = BOOL_FALSE;
            }
            break;
        case SPEED_25G:
            /* 25G QUAD: 25G_25G_25G_25G */
            if ((param_speeds[pm_port_lo + 1] == SPEED_25G) &&
                (param_speeds[pm_port_lo + 2] == SPEED_25G) &&
                (param_speeds[pm_port_lo + 3] == SPEED_25G)) {
                for (i = 0; i < pkt_sched_repetitions; i++) {
                    pm_slot_tbl[(i * 4 + 0) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                    pm_slot_tbl[(i * 4 + 2) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 1;
                    pm_slot_tbl[(i * 4 + 1) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                    pm_slot_tbl[(i * 4 + 3) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 3;
                }
            }
            /* 25G DUAL: 25G_0_25G_0 */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_25G) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                for (i = 0; i < pkt_sched_repetitions; i++) {
                    pm_slot_tbl[(i * 4 + 0) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                    pm_slot_tbl[(i * 4 + 1) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                }
            }
            /* 25G SINGLE: 25G_0_0_0 */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                for (i = 0; i < pkt_sched_repetitions; i++) {
                    pm_slot_tbl[(i * 4 + 0) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                    pm_slot_tbl[(i * 4 + 1) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                }
            }
            /* 25G others: 25G_25G_0_0 */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_25G) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                for (i = 0; i < pkt_sched_repetitions; i++) {
                    pm_slot_tbl[(i * 4 + 0) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                    pm_slot_tbl[(i * 4 + 2) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 1;
                }
            }
            else {
                int is_same_speed = BOOL_TRUE;
                int ln;

                for (ln = 0; ln < MV2_NUM_PM_LNS; ln++) {
                    if (param_speeds[pm_port_lo + ln] != SPEED_0 &&
                        param_speeds[pm_port_lo + ln] != SPEED_25G) {
                        is_same_speed = BOOL_FALSE;
                    }
                }

                if (is_same_speed == BOOL_TRUE) {
                    for (i = 0; i < pkt_sched_repetitions; i++) {
                        for (ln = 0; ln < MV2_NUM_PM_LNS; ln++) {
                            int idx = (i * 4 + ln) % MV2_NUM_PKT_SLOTS_PER_PM;
                            if (param_speeds[pm_port_lo + ln] != SPEED_0)
                                pm_slot_tbl[idx] = pm_port_lo + ln;
                        }
                    }
                }
                else {
                    pm_done = BOOL_FALSE;
                }
            }
            break;
        case SPEED_50G:
            /* 50G SINGLE */
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                tbl_offset = 0;
                for (i = 0; i < 10; i++) {
                    pm_slot_tbl[(tbl_offset + i*2) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                }
            }
            /* 50G DUAL */
            else if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                     (param_speeds[pm_port_lo + 2] == SPEED_50G) &&
                     (param_speeds[pm_port_lo + 3] == SPEED_0)) {
                for (i = 0; i < 10; i++) {
                    pm_slot_tbl[(i * 2)     % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo;
                    pm_slot_tbl[(i * 2 + 1) % MV2_NUM_PKT_SLOTS_PER_PM] = pm_port_lo + 2;
                }
            }
            else {
                pm_done = BOOL_FALSE;
            }
            break;
        case SPEED_100G:
            /* 100G SINGLE */
            for (i = 0; i < MV2_NUM_PKT_SLOTS_PER_PM; i++) {
                pm_slot_tbl[i] = pm_port_lo;
            }
            break;
        default:
            pm_done = BOOL_FALSE;
            break;							
    }

    /* allocate slots for unrecognised pm speed mode */
    if (pm_done == BOOL_FALSE) {
        int ln, num_lanes, slot_cnt, slot_idx, slot_offset, phy_port;

        /* result = FAIL; */
        TDM_ERROR6("%s, Invalid PM config, PM %0d, [%0d,%0d,%0d,%0d,]\n",
                    "[pkt_shaper_per_pm]", pm_num,
                    param_speeds[pm_port_lo + 0]/1000,
                    param_speeds[pm_port_lo + 1]/1000,
                    param_speeds[pm_port_lo + 2]/1000,
                    param_speeds[pm_port_lo + 3]/1000);

        TDM_PRINT1("[pkt_shaper_per_pm] slots for invalid PM %0d\n", pm_num);
        for (ln = 0; ln < MV2_NUM_PM_LNS; ln++) {
            phy_port = pm_port_lo + ln;
            if (param_speeds[phy_port] > 0) {
                num_lanes = param_speeds[phy_port] / SPEED_5G;
                switch (num_lanes) {
                    case 2 : slot_offset = 10; break; /* SPEED_10G  */
                    case 4 : slot_offset = 5 ; break; /* SPEED_20G  */
                    case 5 : slot_offset = 4 ; break; /* SPEED_25G  */
                    case 8 : slot_offset = 2 ; break; /* SPEED_40G  */
                    case 10: slot_offset = 2 ; break; /* SPEED_50G  */
                    case 20: slot_offset = 1 ; break; /* SPEED_100G */
                    default: slot_offset = 20; break; /* SPEED_5G   */
                }
                for (slot_cnt = 0; slot_cnt < num_lanes; slot_cnt++) {
                    slot_idx = ln + slot_cnt * slot_offset;
                    pm_slot_tbl[slot_idx % MV2_NUM_PKT_SLOTS_PER_PM] = phy_port;
                }
            }
        }
    }

    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("[pkt_shaper_per_pm] Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
            break;
    }

    /* Populate pkt scheduler calendar */
    pm_offset = pkt_shpr_pm_indx;
    for (i=0; i<MV2_NUM_PKT_SLOTS_PER_PM; i++) {
        if (pm_slot_tbl[i] != MV2_NUM_EXT_PORTS) {
            lane = i % MV2_NUM_PM_LNS;
            switch (lane) {
                case 0:  lane_offset = 0;                       break; /* subport 0 */
                case 1:  lane_offset = 1*MV2_NUM_PMS_PER_HPIPE; break; /* subport 2 */
                case 2:  lane_offset = 2*MV2_NUM_PMS_PER_HPIPE; break; /* subport 1 */
                default: lane_offset = 3*MV2_NUM_PMS_PER_HPIPE; break; /* subport 3 */
            }
            block_num = (i / MV2_NUM_PM_LNS) % pkt_sched_repetitions;
            /* block_offset = block_num * 40; */
            block_offset = block_num * MV2_NUM_PHY_PORTS_PER_HPIPE;
            cal_pos = (block_offset + lane_offset + pm_offset) % MV2_SHAPING_GRP_LEN;
            pkt_shed_cal[cal_pos] = pm_slot_tbl[i];
        }
    }

    return (result);
}


/**
@name: tdm_mv2_ovsb_chk_same_spd_port
@param:

Return BOOL_TRUE if two input ports have the same speed,
otherwise, return BOOL_FALSE.
 */
int
tdm_mv2_ovsb_chk_same_spd_port(tdm_mod_t *_tdm, int port1, int port2)
{
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;

    param_phy_lo = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds = _tdm->_chip_data.soc_pkg.speed;
    if (port1 >= param_phy_lo && port1 <= param_phy_hi &&
        port2 >= param_phy_lo && port2 <= param_phy_hi) {
        if (param_speeds[port1] == param_speeds[port2]) {
            return BOOL_TRUE;
        }
    }
    return BOOL_FALSE;
}


/**
@name: tdm_mv2_ovsb_pkt_shaper
@param:

Populates Pkt shaper calendar
 */
int
tdm_mv2_ovsb_pkt_shaper(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num;
    int pm_s, pm_e;
    int port_phy, port_phy_base;
    int shift_step;
    int param_pm_lanes;
    int result = PASS;

    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/MV2_NUM_PIPE;

    pm_s = pipe_id*pms_per_pipe;
    pm_e = (pipe_id+1)*pms_per_pipe;

    for (pm_num=pm_s; pm_num<pm_e; pm_num++) {
        port_phy_base = param_pm_lanes * pm_num + 1;
        shift_step = 0;
        for (ln_num=0; ln_num<param_pm_lanes; ln_num++) {
            port_phy = port_phy_base + ln_num;
            if (tdm_mv2_cmn_chk_port_is_os(port_phy, _tdm) == BOOL_TRUE) {
                _tdm->_core_data.vars_pkg.port = port_phy;
                if (tdm_mv2_ovsb_pkt_shaper_per_pm(_tdm, shift_step) != PASS)
                    result = FAIL;
                break;
            }
        }
    }

    return (result);
}


/**
@name: tdm_mv2_ovsb_pkt_shaper_find_pm_indx
@param:

Find an existing or available pm_indx in pkt_shaper
 */
int
tdm_mv2_ovsb_pkt_shaper_find_pm_indx(tdm_mod_t *_tdm)
{
    int pm_num, half_pipe_num;
    int i, j, pm_num_t;
    int phy_port;
    int *pkt_shed_cal;
    int pkt_shpr_pm_indx, pms_per_pipe, pms_per_hpipe, pms_per_hpipe_max;
    int pm_indx_avail[MV2_NUM_PMS_PER_HPIPE];
    int max_start_indx;
    int distance;
    int max_distance;
    int result = PASS;

    phy_port = _tdm->_core_data.vars_pkg.port;
    pm_num = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe[pm_num];
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/MV2_NUM_PIPE;
    pms_per_hpipe = pms_per_pipe / 2;
    pms_per_hpipe_max = MV2_SHAPING_GRP_LEN / MV2_NUM_PKT_SLOTS_PER_PM;
    if (pms_per_hpipe > pms_per_hpipe_max) {
        pms_per_hpipe = pms_per_hpipe_max;
    }
    pkt_shpr_pm_indx = -1;

    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[MV2_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return pkt_shpr_pm_indx;
    }

    /* Two cases possible: 
    1. there are sister ports of this port already in the pkt scheduler; 
       which means find that PM and place phy_port in the corresponding lanes
    2. there is no sister port of this port already in the pkt scheduler; 
       which means find an empty PM and place phy_port in the corresponding lanes*/

    for (i=0; i<MV2_SHAPING_GRP_LEN; i++) {
        if (pkt_shed_cal[i] != _tdm->_chip_data.soc_pkg.num_ext_ports) {
            _tdm->_core_data.vars_pkg.port = pkt_shed_cal[i];
            pm_num_t = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
            if (pm_num == pm_num_t) { /* Found that PM is already placed */
                pkt_shpr_pm_indx = (i% pms_per_hpipe);
                break;
            }
        }
    }

    /* Case 2: there is no sister port of this port already in the pkt scheduler; 
       which means find an empty PM and place phy_port in the corresponding lanes 
       Find an available PM */
    if (pkt_shpr_pm_indx == -1) {
        for (i=0; i<pms_per_hpipe && i<MV2_NUM_PMS_PER_HPIPE; i++){
            pm_indx_avail[i]=1;
            for (j=0; j<(MV2_SHAPING_GRP_LEN/pms_per_hpipe); j++){
                if (pkt_shed_cal[j*pms_per_hpipe+i] != _tdm->_chip_data.soc_pkg.num_ext_ports) {
                    pm_indx_avail[i]=0;
                    break;
                }
            }
        }

        /* Find the biggest clump of 1's in pm_indx_avail array and choose the pm_indx in the middle of the clump*/
        max_start_indx = 0;
        distance=0;
        max_distance =  0;	    
        for (i=0; i<pms_per_hpipe && i<MV2_NUM_PMS_PER_HPIPE; i++){
            if (pm_indx_avail[i]==1) {
                distance=0;
                for (j=0; j<pms_per_hpipe; j++){
                    if (pm_indx_avail[(i+j)%pms_per_hpipe]==1) {
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
        pkt_shpr_pm_indx = (max_distance==pms_per_hpipe) ? 0 : ((max_start_indx + (max_distance/2)) % pms_per_hpipe);
        if (pkt_shpr_pm_indx < MV2_NUM_PMS_PER_HPIPE) {
            if (pm_indx_avail[pkt_shpr_pm_indx] == 0) {
                result = FAIL;
                TDM_ERROR3("%s pm_num=%0d phy_port=%0d\n",
                           "[pkt_shaper_find_pm_indx] failed to find pm for",
                           pm_num, phy_port);
            }
        }
    }

    pkt_shpr_pm_indx = (result == PASS) ? (pkt_shpr_pm_indx) : (-1);
    return pkt_shpr_pm_indx;
}


/**
@name: tdm_mv2_ovsb_map_pm_num_to_pblk
@param:

Maps PM num (block_id) to OVSB pblk id
 */
int
tdm_mv2_ovsb_map_pm_num_to_pblk(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num;
    int half_pipe_num;
    int pblk_indx[2];
    int result = PASS;

    pipe_id = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/MV2_NUM_PIPE;

    pblk_indx[0]=0;	pblk_indx[1]=0;
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_num_to_pblk[pm_num] = -1;
        half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe[pm_num];
        if ((half_pipe_num==0) || (half_pipe_num==1)){
            /* _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_num_to_pblk[pm_num] = pblk_indx[half_pipe_num]; */
            switch (pipe_id) {
                case 0:
                    _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_num_to_pblk[pm_num] = pblk_indx[half_pipe_num];
                    break;
                case 1:
                    /* _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_num_to_pblk[pm_num] = (7 - pblk_indx[half_pipe_num]); */
                    _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_num_to_pblk[pm_num] = (9 - pblk_indx[half_pipe_num]);
                    break;
                default:
                    result = FAIL;
                    TDM_ERROR2("%s Invalid Calendar ID %d \n",
                              "[map_pm_num_to_pblk]", pipe_id);
            }
            pblk_indx[half_pipe_num]++;
        }
        else {
            half_pipe_num = tdm_mv2_ovsb_pm_2_hpipe_mapping(pipe_id,
                                (pm_num * MV2_NUM_PM_LNS + 1));
            if (half_pipe_num == 0 || half_pipe_num == 1) {
                pblk_indx[half_pipe_num]++;
            }
        }
    }
    return (result);
}


/**
@name: tdm_mv2_ovsb_map_pms_to_hpipe_dynamic
@param:
 */
int
tdm_mv2_ovsb_map_pms_to_hpipe_dynamic(tdm_mod_t *_tdm)
{
    int param_pipe_id;
    int result = PASS;

    param_pipe_id  = _tdm->_core_data.vars_pkg.cal_id;
    /* Pre-Partition PMs into 2 half pipes by applying constraints */
    TDM_PRINT1("Apply OVSB constraints on PIPE %d\n", param_pipe_id);
    if (tdm_mv2_ovsb_apply_constraints(_tdm) != PASS) {
        result = FAIL;
    }

    /* Partition all PMs into 2 half pipes */
    TDM_PRINT1("Partition PMs into 2 Half Pipes on PIPE %d\n", param_pipe_id);
    if (tdm_mv2_ovsb_part_halfpipe(_tdm) != PASS) {
        result = FAIL;
    }

    return (result);
}


/**
@name: tdm_mv2_ovsb_map_pms_to_hpipe_static
@param:
[pipe 0] 1 - 40: hpipe 0
[pipe 0]41 - 80: hpipe 1
 */
int
tdm_mv2_ovsb_map_pms_to_hpipe_static(tdm_mod_t *_tdm)
{
    int i, j;
    int pm_num_lo, pm_num_hi, port_phy,
        pms_per_pipe;
    int param_pipe_id, param_phy_lo, param_phy_hi,
        param_pm_lanes, param_pm_num;
    int *param_pm_hpipe;

    param_pipe_id = _tdm->_core_data.vars_pkg.cal_id;
    param_phy_lo  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_num  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_hpipe= _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe;

    pms_per_pipe  = param_pm_num/MV2_NUM_PIPE;
    pm_num_lo     = param_pipe_id*pms_per_pipe;
    pm_num_hi     = (param_pipe_id+1)*pms_per_pipe-1;
    for (i=pm_num_lo; i<=pm_num_hi && i<param_pm_num; i++) {
        param_pm_hpipe[i] = -1;
        for (j=0; j<param_pm_lanes; j++) {
            port_phy = i*param_pm_lanes+j+1;
            if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
                if ( (_tdm->_chip_data.soc_pkg.speed[port_phy] != SPEED_0) &&
                     (_tdm->_chip_data.soc_pkg.state[port_phy-1] == PORT_STATE__OVERSUB ||
                      _tdm->_chip_data.soc_pkg.state[port_phy-1] == PORT_STATE__OVERSUB_HG)) {
                    param_pm_hpipe[i] = tdm_mv2_ovsb_pm_2_hpipe_mapping(
                                            param_pipe_id, port_phy);
                    break;
                }
            }
        }
    }

    return PASS;
}


/**
@name: tdm_mv2_ovsb_map_pms_to_hpipe
@param:
 */
int
tdm_mv2_ovsb_map_pms_to_hpipe(tdm_mod_t *_tdm)
{
    /* static mapping */
    if (TDM_MV2_HPIPE_STATIC_MAPPING_ENABLE == 1) {
        return(tdm_mv2_ovsb_map_pms_to_hpipe_static(_tdm));
    }
    /* dynamic mapping */
    else {
        return(tdm_mv2_ovsb_map_pms_to_hpipe_dynamic(_tdm));
    }
}


/**
@name: tdm_mv2_ovsb_gen_spd_grp
@param:

Partition PMs into 2 half-pipes belonging to the same single pipe,
and generate oversub speed groups accordingly.
 */
int
tdm_mv2_ovsb_gen_spd_grp(tdm_mod_t *_tdm)
{
    int i, j, num_spd_types;
    int param_phy_lo, param_phy_hi, param_cal_id;
    int port_cnt_1g = 0, port_cnt_10g = 0, port_cnt_20g = 0,
        port_cnt_25g = 0, port_cnt_40g = 0, port_cnt_50g = 0,
        port_cnt_100g = 0, port_cnt_120g = 0;
    tdm_calendar_t *cal;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    tdm_mv2_cmn_get_pipe_port_range(param_cal_id, &param_phy_lo,
                                                  &param_phy_hi);

    switch (param_cal_id) {
        case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
        case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
        case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
        case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
        case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
        case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
        case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
        case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n", param_cal_id);
            return (TDM_EXEC_CORE_SIZE+1);
    }
    for (i=0; i<cal->grp_num; i++) {
        for (j=0; j<cal->grp_len; j++) {
            cal->cal_grp[i][j] = _tdm->_chip_data.soc_pkg.num_ext_ports;
        }
    }

    for (i = param_phy_lo; i <= param_phy_hi; i++) {
        if ((_tdm->_chip_data.soc_pkg.speed[i] != SPEED_0) &&
            (_tdm->_chip_data.soc_pkg.state[i-1] == PORT_STATE__OVERSUB ||
             _tdm->_chip_data.soc_pkg.state[i-1] == PORT_STATE__OVERSUB_HG)) {
            switch (_tdm->_chip_data.soc_pkg.speed[i]) {
                case SPEED_1G  : port_cnt_1g++;   break;
                case SPEED_10G : port_cnt_10g++;  break;
                case SPEED_20G : port_cnt_20g++;  break;
                case SPEED_25G : port_cnt_25g++;  break;
                case SPEED_40G : port_cnt_40g++;  break;
                case SPEED_50G : port_cnt_50g++;  break;
                case SPEED_100G: port_cnt_100g++; break;
                case SPEED_120G: port_cnt_120g++; break;
                default: break;
            }
        }
    }

    num_spd_types = _tdm->_core_data.vars_pkg.os_1 +
                    _tdm->_core_data.vars_pkg.os_10 +
                    _tdm->_core_data.vars_pkg.os_20 +
                    _tdm->_core_data.vars_pkg.os_25 +
                    _tdm->_core_data.vars_pkg.os_40 +
                    _tdm->_core_data.vars_pkg.os_50 +
                    _tdm->_core_data.vars_pkg.os_100 +
                    _tdm->_core_data.vars_pkg.os_120;

    TDM_BIG_BAR
    TDM_SML_BAR
    TDM_PRINT1("  1G ports : %0d\n", port_cnt_1g);
    TDM_PRINT1(" 10G ports : %0d\n", port_cnt_10g);
    TDM_PRINT1(" 20G ports : %0d\n", port_cnt_20g);
    TDM_PRINT1(" 25G ports : %0d\n", port_cnt_25g);
    TDM_PRINT1(" 40G ports : %0d\n", port_cnt_40g);
    TDM_PRINT1(" 50G ports : %0d\n", port_cnt_50g);
    TDM_PRINT1("100G ports : %0d\n", port_cnt_100g);
    TDM_PRINT1("120G ports : %0d\n", port_cnt_120g);
    TDM_PRINT1("Number of speed types: %0d\n", num_spd_types);

    if (num_spd_types > cal->grp_num) {
        TDM_ERROR0("Oversub speed type limit exceeded\n");
        return FAIL;
    }

    /* Partition OVSB PMs into 2 half pipes */
    if (tdm_mv2_ovsb_map_pms_to_hpipe(_tdm) != PASS) {
        TDM_ERROR0("PM to Half Pipe mapping failed\n");
        return FAIL;
    }

    /* Allocate ports into oversub speed groups */
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed = SPEED_10G;
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("10G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("10G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed = SPEED_20G;
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("20G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("20G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed = SPEED_25G;
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("25G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("25G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed = SPEED_40G;
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("40G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("40G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed = SPEED_50G;
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("50G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("50G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.mv2.grp_speed = SPEED_100G;
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("100G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
    if(tdm_mv2_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("100G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num);
    }

    return PASS;

    
    
    
}


/**
@name: tdm_mv2_ovsb_get_grp_spd_slots
@param:

Return unified slot number of group speed for the given ovsb group.
If ovsb group is empty, return 0.
 */
int
tdm_mv2_ovsb_get_grp_spd_slots(tdm_mod_t *_tdm, int *grp, int grp_len )
{
    int i, port_phy;
    int grp_spd_slots = 0;
    int param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;

    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;

    for (i = 0; i < grp_len; i++) {
        port_phy = grp[i];
        if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
            grp_spd_slots = tdm_mv2_cmn_get_speed_slots(param_speeds[port_phy]);
            break;
        }
    }
    return grp_spd_slots;
}


/**
@name: tdm_mv2_ovsb_get_grp_size
@param:

Return number of active ports in the given oversub speed group.
If ovsb group is empty, return 0.
 */
int
tdm_mv2_ovsb_get_grp_size(tdm_mod_t *_tdm, int *grp, int grp_len )
{
    int i, port_phy;
    int grp_size = 0;
    int param_phy_lo, param_phy_hi;

    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    for (i = 0; i < grp_len; i++) {
        port_phy = grp[i];
        if (port_phy >= param_phy_lo && port_phy <= param_phy_hi) {
            grp_size++;
        }
    }

    return grp_size;
}


/**
@name: tdm_mv2_ovsb_chk_grp_feasible
@param:

Return BOOL_TRUE if the input speed group can satify speed requirement
and space requirement; otherwise, return BOOL_FALSE.
 */
int
tdm_mv2_ovsb_chk_grp_feasible(tdm_mod_t *_tdm, int *grp, int grp_len,
                              int port_cnt, int port_spd_slots)
{
    int grp_size, grp_spd_slots, space_required, space_left;

    space_required = port_cnt;
    grp_spd_slots = tdm_mv2_ovsb_get_grp_spd_slots(_tdm, grp, grp_len);
    grp_size = tdm_mv2_ovsb_get_grp_size(_tdm, grp, grp_len);
    space_left = grp_len - grp_size;
    if (grp_spd_slots == port_spd_slots &&
        space_required <= space_left) {
        return BOOL_TRUE;
    }

    return BOOL_FALSE;
}


/**
@name: tdm_mv2_ovsb_chk_grp_empty
@param:

Return BOOL_TRUE if the input speed group is empty;
otherwise, return BOOL_FALSE.
 */
int
tdm_mv2_ovsb_chk_grp_empty(tdm_mod_t *_tdm, int *grp, int grp_len)
{
    int grp_size, grp_spd_slots;

    grp_spd_slots = tdm_mv2_ovsb_get_grp_spd_slots(_tdm, grp, grp_len);
    grp_size = tdm_mv2_ovsb_get_grp_size(_tdm, grp, grp_len);
    if (grp_size == 0 && grp_spd_slots == 0) {
        return BOOL_TRUE;
    }

    return BOOL_FALSE;
}


/**
@name: tdm_mv2_ovsb_find_grp_4_port
@param:

Find the position (i,j) of one feasible speed group to accommodate input port.
Return BOOL_TRUE if found, otherwise return BOOL_FALSE.
 */
int
tdm_mv2_ovsb_find_grp_4_port(tdm_mod_t *_tdm, int port_phy,
                             tdm_calendar_t *cal,
                             int *pos_i, int *pos_j)
{
    int i, j, port_pm, port_phy_tmp, port_pm_tmp;
    int port_spd_slots, grp_spd_slots;
    int grp_idx_s, grp_idx_e, grp_idx_i, grp_idx_j;
    int grp_found = BOOL_FALSE;
    int same_spd_sister_exist, same_spd_sister_cnt, same_spd_sister_grp;
    int grp_size, grp_idx, grp_sister_cnt;
    int idx, space_required, space_left;
    int param_cal_id, param_token_empty, param_phy_lo, param_phy_hi,
        param_hpipe_num, param_num_pm_lanes;
    enum port_speed_e *param_speeds;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_num_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_hpipe_num   = _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;

    tdm_mv2_cmn_get_pipe_port_range(param_cal_id, &param_phy_lo, &param_phy_hi);

    grp_idx_s = (param_hpipe_num == 0) ? 0 : (cal->grp_num / 2);
    grp_idx_e = (param_hpipe_num == 0) ? (cal->grp_num / 2) : (cal->grp_num);
    grp_idx_i = cal->grp_num;
    grp_idx_j = cal->grp_len;
    grp_idx   = cal->grp_num;
    port_pm   = (port_phy - 1) / MV2_NUM_PM_LNS;
    port_spd_slots = tdm_mv2_cmn_get_speed_slots(param_speeds[port_phy]);

    /* check if sister port with same speed exists */
    same_spd_sister_exist = BOOL_FALSE;
    same_spd_sister_cnt = 1;
    for (i = 0; i < param_num_pm_lanes; i++) {
        port_phy_tmp = port_pm * param_num_pm_lanes + i + 1;
        if (port_phy_tmp != port_phy &&
            tdm_mv2_ovsb_chk_same_spd_port(_tdm, port_phy, port_phy_tmp) ==
            BOOL_TRUE) {
            same_spd_sister_exist = BOOL_TRUE;
            same_spd_sister_cnt++;
        }
    }

    /* case 1: if same speed sister port exists, then find a speed group that
     *         can accommodate all same speed sister ports.
     *
     *    1.1: same-speed sister port has been allocated.
     *    1.2: same-speed sister port has NOT been allocated.
     */
    if (same_spd_sister_exist == BOOL_TRUE) {
        /* get speed group index of allocated sister port */
        same_spd_sister_grp = cal->grp_num;
        grp_sister_cnt = 0;
        for (i = grp_idx_s; i < grp_idx_e; i++) {
            for (j = 0; j < cal->grp_len; j++) {
                port_phy_tmp = cal->cal_grp[i][j];
                port_pm_tmp = (port_phy_tmp - 1) / 4;
                if (port_pm_tmp == port_pm) {
                    if (BOOL_TRUE == tdm_mv2_ovsb_chk_same_spd_port(
                                            _tdm, port_phy, port_phy_tmp)) {
                        same_spd_sister_grp = i;
                        grp_sister_cnt++;
                    }
                }
            }
            if (same_spd_sister_grp < cal->grp_num) {
                break;
            }
        }
        /* 1.1: if exists allocated same-speed sister port */
        if (same_spd_sister_grp < cal->grp_num) {
            grp_size = tdm_mv2_ovsb_get_grp_size(_tdm,
                        cal->cal_grp[same_spd_sister_grp], cal->grp_len);
            space_required = same_spd_sister_cnt - grp_sister_cnt;
            space_left     = cal->grp_len - grp_size;
            /* 1.1.1: if feasible, record the targeted speed group */
            if (space_required <= space_left) {
                grp_found = BOOL_TRUE;
                grp_idx = same_spd_sister_grp;
            }
            /* 1.1.2: if infeasible, move allocated same-speed-sister-ports
             * into a feasible speed group
             */
            else {
                /* find feasible speed group */
                space_required = same_spd_sister_cnt;
                for (i = grp_idx_s; i < grp_idx_e; i++) {
                    if (BOOL_TRUE == tdm_mv2_ovsb_chk_grp_feasible(_tdm,
                                        cal->cal_grp[i], cal->grp_len,
                                        space_required, port_spd_slots)) {
                        grp_found = BOOL_TRUE;
                        grp_idx = i;
                        break;
                    }
                }
                if (grp_found == BOOL_FALSE) {
                    for (i = grp_idx_s; i < grp_idx_e; i++) {
                        if (BOOL_TRUE == tdm_mv2_ovsb_chk_grp_empty(_tdm,
                                            cal->cal_grp[i], cal->grp_len)) {
                            grp_found = BOOL_TRUE;
                            grp_idx = i;
                            break;
                        }
                    }
                }
                /* migrate allocated same-speed sister ports */
                if (grp_found == BOOL_TRUE) {
                    idx = 0;
                    i = same_spd_sister_grp;
                    for (j = 0; j < cal->grp_len; j++) {
                        port_phy_tmp = cal->cal_grp[i][j];
                        port_pm_tmp = (port_phy_tmp - 1) / 4;
                        if (port_pm_tmp == port_pm) {
                            if (BOOL_TRUE == tdm_mv2_ovsb_chk_same_spd_port(
                                            _tdm, port_phy, port_phy_tmp)) {
                                while (idx < cal->grp_len) {
                                    if (cal->cal_grp[grp_idx][idx] == param_token_empty) {
                                        cal->cal_grp[grp_idx][idx] = port_phy_tmp;
                                        cal->cal_grp[i][j] = param_token_empty;
                                        break;
                                    }
                                    idx++;
                                }
                            }
                        }
                    }
                }
            }
        }
        /* 1.2: find a speed group with enough space for all same-speed sister ports */
        else {
            /* same speed group */
            space_required = same_spd_sister_cnt;
            for (i = grp_idx_s; i < grp_idx_e; i++) {
                if (BOOL_TRUE == tdm_mv2_ovsb_chk_grp_feasible(_tdm,
                                    cal->cal_grp[i], cal->grp_len,
                                    space_required, port_spd_slots)) {
                    grp_found = BOOL_TRUE;
                    grp_idx = i;
                    break;
                }
            }
            /* empty speed group */
            if (grp_found == BOOL_FALSE) {
                for (i=grp_idx_s; i<grp_idx_e; i++) {
                    grp_spd_slots = tdm_mv2_ovsb_get_grp_spd_slots(_tdm,
                                    cal->cal_grp[i], cal->grp_len);
                    if (grp_spd_slots == 0) {
                        grp_found = BOOL_TRUE;
                        grp_idx = i;
                        break;
                    }
                }
            }
        }
    }
    /* case 2: if same speed sister port does not exist, then find a speed
     *         group that can accommodate .
     */
    else {
        /* same speed group */
        space_required = 1;
        for (i = grp_idx_s; i < grp_idx_e; i++) {
            if (BOOL_TRUE == tdm_mv2_ovsb_chk_grp_feasible(_tdm,
                                cal->cal_grp[i], cal->grp_len,
                                space_required, port_spd_slots)) {
                grp_found = BOOL_TRUE;
                grp_idx = i;
                break;
            }
        }
        /* empty speed group */
        if (grp_found == BOOL_FALSE) {
            for (i=grp_idx_s; i<grp_idx_e; i++) {
                grp_spd_slots = tdm_mv2_ovsb_get_grp_spd_slots(_tdm,
                                cal->cal_grp[i], cal->grp_len);
                if (grp_spd_slots == 0) {
                    grp_found = BOOL_TRUE;
                    grp_idx = i;
                    break;
                }
            }
        }
    }

    /* determine pos_i and pos_j */
    if (grp_found == BOOL_TRUE) {
        i = grp_idx;
        for (j = 0; j < cal->grp_len; j++) {
            if (cal->cal_grp[i][j] == param_token_empty) {
                grp_idx_i = i;
                grp_idx_j = j;
                break;
            }
        }
    } else {
        TDM_PRINT2("WARNING: TDM:[flex] Failed to allocate port %0d speed %0dG\n",
                   port_phy, param_speeds[port_phy]/1000);
    }
    if (pos_i != NULL && pos_j != NULL) {
        (*pos_i) = grp_idx_i;
        (*pos_j) = grp_idx_j;
    }

    return grp_found;
}


/**
@name: tdm_mv2_ovsb_consolidate_spd_grp
@param:

Minimize the number of oversub speed groups
 */
int
tdm_mv2_ovsb_consolidate_spd_grp(tdm_mod_t *_tdm)
{
    int i, j, k, n;
    int grp_num_s, grp_num_e;
    int grp_spd_k, grp_spd_i, grp_size_k, grp_size_i;
    int param_cal_id, param_token_empty, param_phy_lo, param_phy_hi,
        param_hpipe_num;
    tdm_calendar_t *cal;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_hpipe_num   = _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    tdm_mv2_cmn_get_pipe_port_range(param_cal_id, &param_phy_lo, &param_phy_hi);

    switch (param_cal_id) {
        case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
        case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
        case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
        case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
        case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
        case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
        case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
        case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n", param_cal_id);
            return (TDM_EXEC_CORE_SIZE+1);
    }
    TDM_PRINT1("TDM: [FLEX] run consolidation for pipe %0d\n", param_cal_id);

    grp_num_s = (param_hpipe_num == 0) ? 0 : (cal->grp_num / 2);
    grp_num_e = (param_hpipe_num == 0) ? (cal->grp_num / 2) : (cal->grp_num);

    for (k = grp_num_s; k < (grp_num_e - 1); k++) {
        grp_spd_k = tdm_mv2_ovsb_get_grp_spd_slots(_tdm,
                                    cal->cal_grp[k], cal->grp_len);
        grp_size_k = tdm_mv2_ovsb_get_grp_size(_tdm,
                                    cal->cal_grp[k], cal->grp_len);
		if((grp_spd_k == 0) || (grp_size_k == cal->grp_len)) continue;

        /* for each non-empty group, do consolidation */
        for (i = k+1; i < grp_num_e; i++) {
            grp_spd_i = tdm_mv2_ovsb_get_grp_spd_slots(_tdm,
                                            cal->cal_grp[i], cal->grp_len);
            grp_size_i = tdm_mv2_ovsb_get_grp_size(_tdm,
                                            cal->cal_grp[i], cal->grp_len);
			if(grp_spd_i != grp_spd_k) continue;
			if((grp_size_i + grp_size_k) > cal->grp_len) continue;

            for (j = 0; (j < cal->grp_len) && (grp_size_i > 0); j++) {
				if(cal->cal_grp[i][j] == param_token_empty) continue;
            	for (n = 0; n < cal->grp_len; n++) {
                    if (cal->cal_grp[k][n] == param_token_empty) {
                        cal->cal_grp[k][n] = cal->cal_grp[i][j];
                        cal->cal_grp[i][j] = param_token_empty;
                        grp_size_k ++;
						grp_size_i --;
						break;
				    }
				}
			}
        }
    }

    return PASS;
}


/**
@name: tdm_mv2_ovsb_flex_port_dn
@param:

Remove down ports from ovsb speed groups
 */
int
tdm_mv2_ovsb_flex_port_dn(tdm_mod_t *_tdm)
{
    int i, j, port_done, port_phy;
    int hpipe0_consolidate_en, hpipe1_consolidate_en;
    int param_cal_id, param_token_empty, param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    tdm_calendar_t *cal;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds      = _tdm->_prev_chip_data.soc_pkg.speed;
    param_states      = _tdm->_prev_chip_data.soc_pkg.state;

    tdm_mv2_cmn_get_pipe_port_range(param_cal_id, &param_phy_lo, &param_phy_hi);

    switch (param_cal_id) {
        case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
        case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
        case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
        case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
        case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
        case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
        case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
        case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n", param_cal_id);
            return (TDM_EXEC_CORE_SIZE+1);
    }

    hpipe0_consolidate_en = BOOL_FALSE;
    hpipe1_consolidate_en = BOOL_FALSE;

    /* remove down_ports and spd_changed_ports */
    for (port_phy = param_phy_lo; port_phy <= param_phy_hi; port_phy++) {
        if (param_states[port_phy-1] == PORT_STATE__FLEX_OVERSUB_DN ||
            param_states[port_phy-1] == PORT_STATE__FLEX_OVERSUB_CH) {
            port_done = BOOL_FALSE;
            for (i=0; i<cal->grp_num; i++) {
                for (j=0; j<cal->grp_len; j++) {
                    if (cal->cal_grp[i][j] == port_phy) {
                        cal->cal_grp[i][j] = param_token_empty;
                        port_done = BOOL_TRUE;
                        TDM_PRINT5("%s %0d, spd %0dG, grp %0d, pos %0d\n",
                                   "TDM: [FLEX] remove down port",
                                   port_phy, param_speeds[port_phy]/1000,
                                   i, j);
                        if (i / (MV2_OS_VBS_GRP_NUM / 2) == 0) {
                            hpipe0_consolidate_en = BOOL_TRUE;
                        } else {
                            hpipe1_consolidate_en = BOOL_TRUE;
                        }
                        break;
                    }
                }
                if (port_done == BOOL_TRUE) {
                    break;
                }
            }
        }
    }

    /* do consolidation */
    if (hpipe0_consolidate_en == BOOL_TRUE) {
        _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 0;
        tdm_mv2_ovsb_consolidate_spd_grp(_tdm);
    }
    if (hpipe1_consolidate_en == BOOL_TRUE) {
        _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num = 1;
        tdm_mv2_ovsb_consolidate_spd_grp(_tdm);
    }

    return PASS;
}


/**
@name: tdm_mv2_ovsb_flex_port_up
@param:

Add up ports into ovsb speed groups
 */
int
tdm_mv2_ovsb_flex_port_up(tdm_mod_t *_tdm)
{
    int port_phy, port_pm;
    int grp_idx_i, grp_idx_j, grp_found;
    int param_cal_id, param_phy_lo, param_phy_hi;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;
    tdm_calendar_t *cal;
    int result = PASS;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    param_states      = _tdm->_prev_chip_data.soc_pkg.state;

    tdm_mv2_cmn_get_pipe_port_range(param_cal_id, &param_phy_lo, &param_phy_hi);

    switch (param_cal_id) {
        case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
        case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
        case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
        case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
        case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
        case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
        case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
        case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
        default:
            TDM_PRINT1("Invalid calendar ID - %0d\n", param_cal_id);
            return (TDM_EXEC_CORE_SIZE+1);
    }

    /* add up_ports and spd_changed_ports */
    for (port_phy = param_phy_lo; port_phy <= param_phy_hi; port_phy++) {
        if (param_states[port_phy-1] == PORT_STATE__FLEX_OVERSUB_UP ||
            param_states[port_phy-1] == PORT_STATE__FLEX_OVERSUB_CH) {
            port_pm = (port_phy - 1) / MV2_NUM_PM_LNS;
            _tdm->_chip_data.soc_pkg.soc_vars.mv2.half_pipe_num =
                _tdm->_chip_data.soc_pkg.soc_vars.mv2.pm_ovs_halfpipe[port_pm];

            /* find a feasible pos (i,j) for up_port */
            grp_found = tdm_mv2_ovsb_find_grp_4_port(_tdm, port_phy, cal,
                                                     &grp_idx_i, &grp_idx_j);
            if (grp_found == BOOL_TRUE &&
                grp_idx_i < cal->grp_num && grp_idx_j < cal->grp_len) {
                cal->cal_grp[grp_idx_i][grp_idx_j] = port_phy;
                TDM_PRINT5("%s %0d, spd %0dG, grp %0d, pos %0d\n",
                           "TDM: [FLEX] add up_port",
                           port_phy, param_speeds[port_phy]/1000,
                           grp_idx_i, grp_idx_j);
            } else {
                result = FAIL;
                TDM_ERROR3("%s %0d, spd %0dG, no available ovsb speed group\n",
                           "TDM: [FLEX] failed to add up_port",
                           port_phy, param_speeds[port_phy]/1000);
            }
        }
    }

    return (result);
}


/**
@name: tdm_mv2_ovsb_flex
@param:

Generate sortable weightable groups for oversub round robin arbitration
Fixed mapping:
[pipe 0] 1 - 40: hpipe 0
[pipe 0]41 - 80: hpipe 1
 */
int
tdm_mv2_ovsb_flex(tdm_mod_t *_tdm)
{
    int i, j, port_phy;
    int flex_up = BOOL_FALSE, flex_dn = BOOL_FALSE;
    int prev_grp_empty = 1;
    int param_cal_id, param_token_empty, param_phy_lo, param_phy_hi;
    enum port_state_e *param_states_prev;
    tdm_calendar_t *cal = NULL, *cal_prev = NULL;

    param_cal_id      = _tdm->_core_data.vars_pkg.cal_id;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_states_prev = _tdm->_prev_chip_data.soc_pkg.state;

    tdm_mv2_cmn_get_pipe_port_range(param_cal_id, &param_phy_lo, &param_phy_hi);

    /* check whether having port to flex up/down */
    for (port_phy = param_phy_lo; port_phy <= param_phy_hi; port_phy++) {
        if (param_states_prev[port_phy-1] == PORT_STATE__FLEX_OVERSUB_UP) {
            flex_up = BOOL_TRUE;
        } else if (param_states_prev[port_phy-1] == PORT_STATE__FLEX_OVERSUB_DN) {
            flex_dn = BOOL_TRUE;
        } else if (param_states_prev[port_phy-1] == PORT_STATE__FLEX_OVERSUB_CH) {
            flex_up = BOOL_TRUE;
            flex_dn = BOOL_TRUE;
        }
    }

    /* allocate all but up ports into appropriate speed groups */
    if (tdm_mv2_ovsb_gen_spd_grp(_tdm) != PASS) {
        return FAIL;
    }

    {
        switch (param_cal_id) {
            case 0:	cal=(&(_tdm->_chip_data.cal_0)); break;
            case 1:	cal=(&(_tdm->_chip_data.cal_1)); break;
            case 2:	cal=(&(_tdm->_chip_data.cal_2)); break;
            case 3:	cal=(&(_tdm->_chip_data.cal_3)); break;
            case 4:	cal=(&(_tdm->_chip_data.cal_4)); break;
            case 5:	cal=(&(_tdm->_chip_data.cal_5)); break;
            case 6:	cal=(&(_tdm->_chip_data.cal_6)); break;
            case 7:	cal=(&(_tdm->_chip_data.cal_7)); break;
            default: break;
        }
        switch (param_cal_id) {
            case 0:	cal_prev=(&(_tdm->_prev_chip_data.cal_0)); break;
            case 1:	cal_prev=(&(_tdm->_prev_chip_data.cal_1)); break;
            case 2:	cal_prev=(&(_tdm->_prev_chip_data.cal_2)); break;
            case 3:	cal_prev=(&(_tdm->_prev_chip_data.cal_3)); break;
            case 4:	cal_prev=(&(_tdm->_prev_chip_data.cal_4)); break;
            case 5:	cal_prev=(&(_tdm->_prev_chip_data.cal_5)); break;
            case 6:	cal_prev=(&(_tdm->_prev_chip_data.cal_6)); break;
            case 7:	cal_prev=(&(_tdm->_prev_chip_data.cal_7)); break;
            default: break;
        }

        if (cal != NULL && cal_prev != NULL) {
            for (i=0; i<cal->grp_num; i++) {
                for (j=0; j<cal->grp_len; j++) {
                    if (cal_prev->cal_grp[i][j] != param_token_empty) {
                        prev_grp_empty = 0;
                        break;
                    }
                }
            }
            if (!prev_grp_empty) {
                TDM_PRINT1("TDM: Pipe %0d, init ovsb grp by pre_flex values\n",
                           param_cal_id);
                for (i=0; i<cal->grp_num; i++) {
                    for (j=0; j<cal->grp_len; j++) {
                        cal->cal_grp[i][j] = cal_prev->cal_grp[i][j];
                    }
                }
            }
        }
    }

    /* remove down_ports */
    if (flex_dn) {
        tdm_mv2_ovsb_flex_port_dn(_tdm);
    }

    /* add up_ports */
    if (flex_up) {
        tdm_mv2_ovsb_flex_port_up(_tdm);
    }

    return PASS;
}

/**
@name: tdm_mv2_vbs_scheduler_ovs
@param:

*/
int
tdm_mv2_vbs_scheduler_ovs(tdm_mod_t *_tdm)
{
    /* generate OVSB groups */
    TDM_PRINT1("TDM: flex_port_en = %0d\n",
           _tdm->_chip_data.soc_pkg.flex_port_en);
    if (_tdm->_chip_data.soc_pkg.flex_port_en == 1) {
        if(tdm_mv2_ovsb_flex(_tdm) != PASS) {
            return FAIL;
        }
    } else {
        if (tdm_mv2_ovsb_gen_spd_grp(_tdm) != PASS) {
            return FAIL;
        }
    }

    /* generate PKT scheduler */
    TDM_PRINT1("OVSB partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    if (tdm_mv2_ovsb_pkt_shaper(_tdm) != PASS){
        return FAIL;
    }
    TDM_PRINT1("OVSB partition for PIPE %d PM_NUM to PBLK mapping DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    if(tdm_mv2_ovsb_map_pm_num_to_pblk(_tdm) != PASS) {
        return FAIL;
    }

    return PASS;
}
