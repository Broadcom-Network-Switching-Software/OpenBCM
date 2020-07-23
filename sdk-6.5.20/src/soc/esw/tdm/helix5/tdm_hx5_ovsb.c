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
@name: tdm_hx5_ovsb_apply_constraints
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_hx5_ovsb_apply_constraints(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num, i;

    int speed_en_mtx[HX5_NUM_PHY_PM][6];
    int speed_en_mtx_reduced[6];
    /* <speed_indx> 0: 10G; 1: 20G; 2: 25G; 3: 40G; 4: 50G; 5: 100G */
    int no_of_speeds_in_pipe;
    int phy_base_port, phy_port;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;

    for (i=0; i<6; i++) {
        for (pm_num=0; pm_num<HX5_NUM_PHY_PM; pm_num++) {
            speed_en_mtx[pm_num][i]=0;
        }
        speed_en_mtx_reduced[i]=0;
    }

    /* Build speed_en_mtx matrix*/
    for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
        _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] = -1;
        phy_base_port = HX5_NUM_PM_LNS*pm_num+1;
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
                _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] = 2; /* OVSB but not assigned */
                switch(_tdm->_chip_data.soc_pkg.speed[phy_port]) {
                    case SPEED_10G : 
                        speed_en_mtx[pm_num][HX5_SPEED_IDX_10G] = 1; break;
                    case SPEED_20G : 
                        speed_en_mtx[pm_num][HX5_SPEED_IDX_20G] = 1; break;
                    case SPEED_25G : 
                        speed_en_mtx[pm_num][HX5_SPEED_IDX_25G] = 1; break;
                    case SPEED_40G : 
                        speed_en_mtx[pm_num][HX5_SPEED_IDX_40G] = 1; break;
                    case SPEED_50G : 
                        speed_en_mtx[pm_num][HX5_SPEED_IDX_50G] = 1; break;
                    case SPEED_100G : 
                        speed_en_mtx[pm_num][HX5_SPEED_IDX_100G]= 1; break;
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
        TDM_ERROR2("tdm_hx5_ovsb_apply_constraints() PIPE %d No OVSB port configurations with more than 4 port speed classes are supported; no_of_speeds_in_pipe=%d\n", pipe_id, no_of_speeds_in_pipe);

    }
    else if (no_of_speeds_in_pipe == 4) {
    /*Restriction 14: The only supported port configurations with 4 port speed classes are:
        10G/20G/40G/100G
        10G/25G/50G/100G
    */
        if ( (speed_en_mtx_reduced[HX5_SPEED_IDX_20G] || speed_en_mtx_reduced[HX5_SPEED_IDX_40G]) && (speed_en_mtx_reduced[HX5_SPEED_IDX_25G] || speed_en_mtx_reduced[HX5_SPEED_IDX_50G]) ) {
        /* Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1*/
            TDM_PRINT1("tdm_hx5_ovsb_apply_constraints() PIPE %d applying Restriction 14 \n",pipe_id);
            for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
                if (speed_en_mtx[pm_num][HX5_SPEED_IDX_25G] || speed_en_mtx[pm_num][HX5_SPEED_IDX_50G]) {
                    _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] = 0;
                } else if (speed_en_mtx[pm_num][HX5_SPEED_IDX_20G] || speed_en_mtx[pm_num][HX5_SPEED_IDX_40G]) {
                    _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] = 1;
                }
            }
        }
    } 
    else if (no_of_speeds_in_pipe > 1) {
    /*Restriction 15: All port configurations with 1-3 port speed classes are supported, except
        configurations that contain both 20G and 25G port speeds.*/
        if ( speed_en_mtx_reduced[HX5_SPEED_IDX_20G] && speed_en_mtx_reduced[HX5_SPEED_IDX_25G]) {
        /* Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1*/
            TDM_PRINT1("tdm_hx5_ovsb_apply_constraints() PIPE %d applying Restriction 15 \n",pipe_id);
            for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
                if (speed_en_mtx[pm_num][HX5_SPEED_IDX_25G]) { /*if (speed_en_mtx[pm_num][HX5_SPEED_IDX_25G] || speed_en_mtx[pm_num][HX5_SPEED_IDX_50G]) {*/
                    _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] = 0;
                } else if (speed_en_mtx[pm_num][HX5_SPEED_IDX_20G]) { /*} else if (speed_en_mtx[pm_num][HX5_SPEED_IDX_20G] || speed_en_mtx[pm_num][HX5_SPEED_IDX_40G]) {*/
                    _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] = 1;
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
@name: tdm_hx5_ovsb_part_halfpipe
@param:

Partition OVSB PMs into 2 halfpipes
 */

/* new half pipe partition 
 * all pm4x10q on hp 0 - PM4x10Q#0	PM4x10Q#1	PM4x10Q#2 [1-48]
 * all 3 pm4x10 on hp1 - PM4x10#0	PM4x10#1	PM4x10#2  [49-60]
 * UL ports LR which is controlled through ox bitmap [61-64] , [73 -78] PM4x10#3	PM4x25#2
 * hp 0 -	PM4x25#0 - falcon0 - [65-68]
 * hp 1 -	PM4x25#1 - falcon1 - [69-72]
 */
int
tdm_hx5_ovsb_part_halfpipe(tdm_mod_t *_tdm)
{
    int pm_num_hi,pm_num_lo,pms_per_pipe, pipe_id, tmp;
    int pm_speed[HX5_NUM_PHY_PM];
    int pm_num_subports[HX5_NUM_PHY_PM];
    int pm_num_sort[HX5_NUM_PHY_PM];
    int pm_num, ln_num, pm_indx, i, j;
    int phy_base_port, phy_port;
    int hp0_speed, hp1_speed;
    int no_pms_hp0, no_pms_hp1;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;
    pm_num_lo  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    pm_num_hi  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;

    COMPILER_REFERENCE(pipe_id);
    COMPILER_REFERENCE(pms_per_pipe);
    COMPILER_REFERENCE(pm_num_hi);
    COMPILER_REFERENCE(pm_num_lo);

    pm_indx=0;
    for (pm_num= 0 ; pm_num < HX5_NUM_PHY_PM ; pm_num++) { /* considering only OS pm's */
        phy_base_port = HX5_NUM_PM_LNS*pm_num+1;
        pm_speed[pm_indx] = 0;
        pm_num_subports[pm_indx] = 0;
        for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = phy_base_port+ln_num;
          /*    TDM_PRINT5("tdm_hx5_part_halfpipe() pm_num=%d ln_num=%d port=%d pm_speed=%d pm_indx=%d \n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000, pm_indx); */ 
            if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
            ((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
             (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {

	     if ( phy_port <= HX5_NUM_PHY_GPORTS) {
                   TDM_PRINT4("tdm_hx5_part_halfpipe1() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
                   pm_speed[pm_indx] = pm_speed[pm_indx] + _tdm->_chip_data.soc_pkg.speed[phy_port]/1000;
                   pm_num_subports[pm_indx]++;
                   } 
	    else if (( phy_port <=72 ) && (phy_port >= 69 )) { /* to include first half pipe first falcon */
                   TDM_PRINT4("tdm_hx5_part_halfpipe1() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
                   pm_speed[pm_indx] = pm_speed[pm_indx] + _tdm->_chip_data.soc_pkg.speed[phy_port]/1000;
                   pm_num_subports[pm_indx]++;
                          } 
	    else if (( phy_port <=64 ) && (phy_port >= 61 )) { /* to include first half pipe first falcon */
                   TDM_PRINT4("tdm_hx5_part_halfpipe1() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
                   pm_speed[pm_indx] = pm_speed[pm_indx] + _tdm->_chip_data.soc_pkg.speed[phy_port]/1000;
                   pm_num_subports[pm_indx]++;
                          } 
	    else if ( phy_port <=HX5_NUM_PHY_PORTS) {
                    TDM_PRINT4("tdm_hx5_part_halfpipe2() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
                    pm_speed[pm_indx] = pm_speed[pm_indx] + _tdm->_chip_data.soc_pkg.speed[phy_port]/1000;
                    pm_num_subports[pm_indx]++;
                    }
            }
        }
        pm_num_sort[pm_indx] = pm_num;
        pm_indx++;
    }

    /* Sort speeds of the PMs in descending order - bubble sort */ 
    for (i=0; i<HX5_NUM_PHY_PM; i++) {
        for (j=(HX5_NUM_PHY_PM-1); j>i; j--) {
        /*    TDM_PRINT2("sorting i=%d j=%d \n", i,j ); */
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
    /* Compute HP 0 & 1 BW based on already allocated PMs in constraints */
    for (i=0; i<HX5_NUM_PHY_PM; i++) {
        pm_num = pm_num_sort[i];
        if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] == 0) {
            hp0_speed += pm_speed[i];
            no_pms_hp0++;
          /*  TDM_PRINT3(" num of half_pipe0 ports =%d  half_pipe1 ports =%d ,i=%d \n", no_pms_hp0,no_pms_hp1,i ); */
        }
        if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] == 1) {
            hp1_speed += pm_speed[i];
            no_pms_hp1++;
         /*  TDM_PRINT3(" num of half_pipe1 ports =%d  half_pipe1 ports =%d ,i=%d \n", no_pms_hp0,no_pms_hp1,i ); */
        }
    }

          /*  TDM_PRINT2(" num of half_pipe0 ports =%d  half_pipe1 ports =%d \n", no_pms_hp0,no_pms_hp1 ); */
    return PASS;
}


/**
@name: tdm_hx5_ovsb_fill_group
@param:

Partition OVSB PMs into 2 halfpipes
 */
int
tdm_hx5_ovsb_fill_group(tdm_mod_t *_tdm)
{
    /*int pipe_id, tmp; */
    int tmp;
    int pm_num_subports[HX5_NUM_PHY_PM];
    int pm_num_sort[HX5_NUM_PHY_PM];
    int pm_num, ln_num, pm_indx, i, j;

    int pms_with_grp_speed;
    int grp_speed;
    int half_pipe_num;
    int speed_max_num_ports_per_pm;
    int min_num_ovs_groups;
    tdm_calendar_t *cal;
    int start_ovs_group;
    
    int grp_index[HX5_OS_VBS_GRP_NUM];
    int phy_base_port;
    int phy_port;
    
    int t_ovs_grp_num;
    
   /* pipe_id       = _tdm->_core_data.vars_pkg.cal_id; */
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num;
    grp_speed     = _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed;
  /*  TDM_PRINT1("grp_speed %0d \n",grp_speed);	*/	
  /*  pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD; */

    /* Count the number of subports per PM with grp_speed for PMs that belongs to this half pipe*/
    pm_indx=0;
    pms_with_grp_speed = 0;
    for (pm_num=0 ; pm_num<HX5_NUM_PHY_PM ; pm_num++) {
        phy_base_port = HX5_NUM_PM_LNS*(pm_num)+1;
        pm_num_subports[pm_indx] = 0;
        if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] == half_pipe_num){
            for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
                phy_port = phy_base_port+ln_num;
                if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
                    pm_num_subports[pm_indx]++;
                 /*  TDM_PRINT2("found %0d , pm_indx= %d\n",grp_speed, pm_indx); */		
                }
		else{
                 /*  TDM_PRINT3("no match %0d , pm_indx= %d , speed[phy_port] %d \n",grp_speed, pm_indx,_tdm->_chip_data.soc_pkg.speed[phy_port]); 	*/	
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
            TDM_PRINT1("tdm_hx5_ovsb_fill_group3() Invalid group speed %0d\n",grp_speed);		
            return FAIL;							
    }
    /* ceil [(speed_max_num_ports_per_pm*pms_with_grp_speed) / HX5_OS_VBS_GRP_LEN ]*/
    min_num_ovs_groups = ((speed_max_num_ports_per_pm*pms_with_grp_speed) + HX5_OS_VBS_GRP_LEN-1) / HX5_OS_VBS_GRP_LEN;

    /* Execute only if ports with this speed exist*/
    if (min_num_ovs_groups > 0) {
        /* Sort PMs in descending order of their number of subports with grp_speed*/
        for (i=0; i<HX5_NUM_PHY_PM; i++) {
            for (j= (HX5_NUM_PHY_PM-1) ; j>i; j--) {
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
                TDM_PRINT1("tdm_hx5_ovsb_fill_group()Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
                return FAIL;
        }
        start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2);
        /* Fill ovs groups with ports having grp_speed*/
        for (i=0; i< HX5_OS_VBS_GRP_NUM; i++) { grp_index[i] = 0;}
            pm_num = pm_num_sort[0];
            phy_base_port = HX5_NUM_PM_LNS*pm_num+1;
          /*  TDM_PRINT3("min_num_ovs_groupsstart_ovs_group %d start_pm_eagle %d phy_base_port %d \n"min_num_ovs_groups,start_ovs_group,start_pm_eagle,phy_base_port); */		
        for (i=0; i<pms_with_grp_speed; i++) {
            pm_num = pm_num_sort[i];
            phy_base_port = HX5_NUM_PM_LNS*pm_num+1;
            if (_tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num] == half_pipe_num){
                for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
                    phy_port = phy_base_port+ln_num;
		    if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
		    	if(phy_port >= 61 && phy_port <= 68) /* allocate UL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 4;
		    	else if(phy_port >= 69 && phy_port <= 76) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 5;
		    	else if(phy_port >= 1 && phy_port <= 4) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 0;
		    	else if(phy_port >= 17 && phy_port <= 20) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 1;
		    	else if(phy_port >= 33 && phy_port <= 36) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 2;
		    	else if(phy_port >= 49 && phy_port <= 52) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 0;
		    	else if(phy_port >= 53 && phy_port <= 56) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 1;
		    	else if(phy_port >= 57 && phy_port <= 60) /* allocate SL OS group */
                	   start_ovs_group = half_pipe_num *(HX5_OS_VBS_GRP_NUM/2) + 2;
                        t_ovs_grp_num = start_ovs_group ;
                        TDM_PRINT3("min_num_ovs_groups  %d t_ovs_grp_num %d phy_port %d \n",grp_index[t_ovs_grp_num],t_ovs_grp_num,phy_port); 		
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
@name: tdm_hx5_ovsb_pkt_shaper
@param:

Populates Pkt shaper calendar
 */
int
tdm_hx5_ovsb_pkt_shaper(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num;
    int phy_base_port, phy_port;
    int pm_s, pm_e;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;

    pm_s = pipe_id*pms_per_pipe;
    pm_e = (pipe_id+1)*pms_per_pipe;

    for (pm_num=pm_s; pm_num<pm_e; pm_num++) {
        phy_base_port = HX5_NUM_PM_LNS*pm_num+1;
        for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = phy_base_port+ln_num;
            if (phy_port<=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
                if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
                   ((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
                    (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
                    TDM_PRINT4("tdm_hx5_ovsb_pkt_shaper2() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",
                               pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]);
                    _tdm->_core_data.vars_pkg.port = phy_port;
                    tdm_hx5_ovsb_pkt_shaper_per_port(_tdm);
                }
            }
        }
    }

    return PASS;
}



/**
@name: tdm_hx5_ovsb_pkt_shaper_per_port
@param:

Populates Pkt shaper calendar - per port
 */
int
tdm_hx5_ovsb_pkt_shaper_per_port(tdm_mod_t *_tdm)
{
    int pms_per_pipe;
    int pm_num, i;
    int phy_port;
    int half_pipe_num;

    int pkt_shpr_pm_indx;
    int no_of_lanes;
    int num_slots_for_port;
    int total_num_slots;
    int port_slot_tbl[HX5_SHAPING_GRP_LEN/8];
    int port_slot_tbl_shift[HX5_SHAPING_GRP_LEN/8];
    int slot_tbl_shift;
    int subport_no;
    int is_20G;
    int *pkt_shed_cal;
    int pkt_sched_repetitions;
    int base_offset;
    int max_pms_per_halfpipe;

    phy_port = _tdm->_core_data.vars_pkg.port;
    pm_num = (phy_port-1)/HX5_NUM_PM_LNS;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num];

    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;
    max_pms_per_halfpipe = pms_per_pipe/2;

    pkt_sched_repetitions = HX5_SHAPING_GRP_LEN/(pms_per_pipe/2)/HX5_NUM_PM_LNS; /* TH2: 5 = 160/8/4 */

    /*TDM_PRINT2("tdm_hx5_ovsb_pkt_shaper() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/

    /* Determine the pm_indx in pkt shceduler calendar - 0 to 7*/
    pkt_shpr_pm_indx = tdm_hx5_ovsb_pkt_shaper_find_pm_indx(_tdm);

    /* Find the number of lanes allocated to the port */
    is_20G=0;
    switch (_tdm->_chip_data.soc_pkg.speed[phy_port]) {
        case SPEED_10G:	no_of_lanes=1; break;
        case SPEED_20G:	no_of_lanes=2; is_20G = 1; break;
        case SPEED_40G:	
            if ((_tdm->_chip_data.soc_pkg.speed[HX5_NUM_PM_LNS*pm_num+1+1] == SPEED_0) &&
                (_tdm->_chip_data.soc_pkg.speed[HX5_NUM_PM_LNS*pm_num+1+2] == SPEED_0) &&
                (_tdm->_chip_data.soc_pkg.speed[HX5_NUM_PM_LNS*pm_num+1+3] == SPEED_0) ) { /* SINGLE */
                no_of_lanes=4;
            } else { /* DUAL */
                no_of_lanes=2;
            }
            break;
        case SPEED_25G:	no_of_lanes=1; break;
        case SPEED_50G:	no_of_lanes=2; break;
        case SPEED_100G: no_of_lanes=4; break;
        default:													
            TDM_PRINT1("tdm_hx5_ovsb_fill_group3() Invalid group speed %0d\n",_tdm->_chip_data.soc_pkg.speed[phy_port]);		
            return FAIL;							
    }	

    /* Compute the number of slots in pkt calendar for this port*/
    num_slots_for_port = _tdm->_chip_data.soc_pkg.speed[phy_port]/5000; /* each slots is 5G */

    /* Compute the total number of slots consumed by the lanes*/
    total_num_slots = 5*no_of_lanes;

    /* First, place num_slots_for_port slots in a table with total_num_slots equally spaced */
    for (i=0; i<HX5_SHAPING_GRP_LEN/max_pms_per_halfpipe; i++) {
        port_slot_tbl[i] = HX5_NUM_EXT_PORTS;
        port_slot_tbl_shift[i]= HX5_NUM_EXT_PORTS;
    }
    for (i=0; i<num_slots_for_port; i++) {
        port_slot_tbl[(i*total_num_slots)/num_slots_for_port] = phy_port;
    }


    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("tdm_hx5_ovsb_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
    }

    /*  In order to spread tokens as uniformly as possible shift slot_tbl based on subport number */	
    subport_no = (phy_port-1)%HX5_NUM_PM_LNS;
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
                    TDM_PRINT1("tdm_hx5_ovsb_pkt_shaper_per_port() phy_port lane for phy_port=%d\n",phy_port);
                    return FAIL;			
            }
        }

        cal_pos = base_pos + lane_pos + pkt_shpr_pm_indx;
        pkt_shed_cal[cal_pos] = port_slot_tbl_shift[i];
    }

    return PASS;
}


/**
@name: tdm_hx5_ovsb_pkt_shaper_per_pm
@param:

Populates Pkt shaper calendar - per PM
 */
int
tdm_hx5_ovsb_pkt_shaper_per_pm(tdm_mod_t *_tdm, int shift_step)
{
    int pms_per_pipe;
    int pm_num, i;
    int port_phy;
    int half_pipe_num;

    int pkt_shpr_pm_indx;
    int port_slot_tbl[HX5_SHAPING_GRP_LEN/8];
    int *pkt_shed_cal;
    int pkt_sched_repetitions;

    int pm_port_lo, block_num, lane_offset, pm_offset, block_offset;
    int lane, cal_pos, base_offset;
    enum port_speed_e *param_speeds;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;

    port_phy = _tdm->_core_data.vars_pkg.port;
    pm_num = (port_phy-1)/HX5_NUM_PM_LNS;
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num];

    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;

    pkt_sched_repetitions = HX5_SHAPING_GRP_LEN/(pms_per_pipe/2)/HX5_NUM_PM_LNS; /* TH2: 5 = 160/8/4 */

    for (i=0; i<HX5_SHAPING_GRP_LEN/8; i++) {
        port_slot_tbl[i] = HX5_NUM_EXT_PORTS;
    }

    /* Determine the pm_indx in pkt shceduler calendar - 0 to 7*/
    pkt_shpr_pm_indx = tdm_hx5_ovsb_pkt_shaper_find_pm_indx(_tdm);

    /* allocate slots for PM */
    pm_port_lo = HX5_NUM_PM_LNS * pm_num + 1;
    switch (param_speeds[port_phy]) {
        case SPEED_10G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 8) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
            } else {
                /* QUAD */
                base_offset = (shift_step % 5) * 4;
                /* base_offset = 0; */
                port_slot_tbl[(base_offset + 0 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 8 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                port_slot_tbl[(base_offset + 18) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                port_slot_tbl[(base_offset + 5 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 13) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 3 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
                port_slot_tbl[(base_offset + 15) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
            }
            break;
        case SPEED_20G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 4 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 14) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
            } else {
                /* DUAL */
                port_slot_tbl[(0 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(4 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(10) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(14) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(1 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(5 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(11) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(15) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
            }
            break;
        case SPEED_40G:	
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 8 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 18) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 5 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 13) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 3 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 15) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
            } else {
                /* DUAL */
                base_offset = 0;
                port_slot_tbl[(base_offset + 0 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 2 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 4) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 6) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 10) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 12) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 14) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 16) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(base_offset + 1 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 3 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 5 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 7 ) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 11) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 13) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 15) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(base_offset + 17) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
            }
            break;
        case SPEED_25G:
            /* QUAD */
            for (i = 0; i < pkt_sched_repetitions; i++) {
                port_slot_tbl[(i * 4 + 0) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                port_slot_tbl[(i * 4 + 2) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 1;
                port_slot_tbl[(i * 4 + 1) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                port_slot_tbl[(i * 4 + 3) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 3;
            }
            break;
        case SPEED_50G:
            if ((param_speeds[pm_port_lo + 1] == SPEED_0) &&
                (param_speeds[pm_port_lo + 2] == SPEED_0) &&
                (param_speeds[pm_port_lo + 3] == SPEED_0) ) {
                /* SINGLE */
                base_offset = 0;
                for (i = 0; i < 10; i++) {
                    port_slot_tbl[(base_offset + i*2) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                }
            } else {
                /* DUAL */
                for (i = 0; i < 10; i++) {
                    port_slot_tbl[(i * 2)     % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo;
                    port_slot_tbl[(i * 2 + 1) % (HX5_SHAPING_GRP_LEN/8)] = pm_port_lo + 2;
                }
            }
            break;
        case SPEED_100G:
            for (i = 0; i < HX5_SHAPING_GRP_LEN/8; i++) {
                port_slot_tbl[i] = pm_port_lo;
            }
            break;
        default:
            break;							
    }
    

    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("tdm_hx5_ovsb_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
            break;
    }

    /* Populate pkt scheduler calendar */
    pm_offset = pkt_shpr_pm_indx;
    for (i=0; i<HX5_SHAPING_GRP_LEN/8; i++) {
        if (port_slot_tbl[i] != HX5_NUM_EXT_PORTS) {
            lane = i % HX5_NUM_PM_LNS;
            switch (lane) {
                case 0:  lane_offset = 0;  break; /* subport 0 */
                case 1:  lane_offset = 8;  break; /* subport 2 */
                case 2:  lane_offset = 16; break; /* subport 1 */
                /* case 3:  lane_offset = 24; break; */
                default: lane_offset = 24;  break; /* subport 3 */
            }
            block_num = (i / HX5_NUM_PM_LNS) % pkt_sched_repetitions;
            block_offset = block_num * 32;
            cal_pos = (block_offset + lane_offset + pm_offset) % HX5_SHAPING_GRP_LEN;
            pkt_shed_cal[cal_pos] = port_slot_tbl[i];
        }
    }

    return (PASS);
}


/**
@name: tdm_hx5_ovsb_chk_port_is_os
@param:

Return BOOL_TRUE if given port is an oversub port,
Otherwise, return FALSE.
 */
int
tdm_hx5_ovsb_chk_port_is_os(tdm_mod_t *_tdm, int port)
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
@name: tdm_hx5_ovsb_chk_pm_is_4x10g
@param:

Return BOOL_TRUE if given PM is 4x10g,
Otherwise, return FALSE.
 */
int
tdm_hx5_ovsb_chk_pm_is_4x10g(tdm_mod_t *_tdm, int pm_num)
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
@name: tdm_hx5_ovsb_hpipe_has_100g
@param:

Return BOOL_TRUE if selected hpipe contains 100G oversub port,
Otherwise, return FALSE.
 */
int
tdm_hx5_ovsb_hpipe_has_100g(tdm_mod_t *_tdm, int pipe_id, int hp_num)
{
    int result = BOOL_FALSE;
    int pms_per_pipe, pms_per_hpipe;
    int pm_s, pm_e, port, port_s, port_e;
    int param_phy_lo, param_phy_hi, param_pm_lanes, param_pm_num;
    enum port_speed_e *param_speeds;
    enum port_state_e *param_states;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_num   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_speeds   = _tdm->_chip_data.soc_pkg.speed;
    param_states   = _tdm->_chip_data.soc_pkg.state;

    pms_per_pipe = param_pm_num / HX5_NUM_QUAD;
    pms_per_hpipe= pms_per_pipe / 2;
    if (hp_num == 0) {
        pm_s = pipe_id * pms_per_pipe;
        pm_e = (pipe_id+1) * pms_per_pipe - pms_per_hpipe;
    } else {
        pm_s = pipe_id * pms_per_pipe + pms_per_hpipe;
        pm_e = (pipe_id+1) * pms_per_pipe;
    }
    port_s = pm_s * param_pm_lanes + 1;
    port_e = pm_e * param_pm_lanes;

    if (port_s >= param_phy_lo && port_e <= param_phy_hi) {
        for (port = port_s; port <= port_e; port++) {
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
@name: tdm_hx5_ovsb_pkt_shaper_new
@param:

Populates Pkt shaper calendar
 */
int
tdm_hx5_ovsb_pkt_shaper_new(tdm_mod_t *_tdm)
{
    int pms_per_pipe, pipe_id;
    int pm_num, ln_num;
    int pm_s, pm_e;
    int port_phy, port_phy_base;
    int i, hp_num, shift_step, hp_pm_cnt_4x10g[2];
    int param_pm_lanes;
    int *param_pm_hp_id;

    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    param_pm_hp_id = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe;

    pipe_id      = _tdm->_core_data.vars_pkg.cal_id;
    pms_per_pipe = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;

    pm_s = pipe_id*pms_per_pipe;
    pm_e = (pipe_id+1)*pms_per_pipe;
    for (i = 0; i < 2; i++) {
        hp_pm_cnt_4x10g[i] = 0;
    }

    for (pm_num=pm_s; pm_num<pm_e; pm_num++) {
        port_phy_base = param_pm_lanes * pm_num + 1;
        shift_step = 0;
        /* if pm is 4x10g and hpipe contains 100G, then shift 4x10g */
        if (tdm_hx5_ovsb_chk_pm_is_4x10g(_tdm, pm_num) == BOOL_TRUE) {
            hp_num = param_pm_hp_id[pm_num];
            if (tdm_hx5_ovsb_hpipe_has_100g(_tdm, pipe_id, hp_num)
                                           == BOOL_TRUE) {
                shift_step = hp_pm_cnt_4x10g[hp_num%2];
                hp_pm_cnt_4x10g[hp_num%2]++;
            }
        }
        for (ln_num=0; ln_num<param_pm_lanes; ln_num++) {
            port_phy = port_phy_base + ln_num;
            if (tdm_hx5_ovsb_chk_port_is_os(_tdm, port_phy) == BOOL_TRUE) {
                _tdm->_core_data.vars_pkg.port = port_phy;
                tdm_hx5_ovsb_pkt_shaper_per_pm(_tdm, shift_step);
                break;
            }
        }
    }

    return (PASS);
}


/**
@name: tdm_hx5_ovsb_pkt_shaper_find_pm_indx
@param:

Find an existing or available pm_indx in pkt_shaper
 */
int
tdm_hx5_ovsb_pkt_shaper_find_pm_indx(tdm_mod_t *_tdm)
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
    half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[pm_num];
    pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/HX5_NUM_QUAD;

    /* Get the right pkt scheduler calendar*/
    switch (_tdm->_core_data.vars_pkg.cal_id) {
        case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[HX5_SHAPING_GRP_IDX_0+half_pipe_num]; break;
        default:
            TDM_PRINT1("tdm_hx5_ovsb_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
            return FAIL;
    }

    /* Two cases possible: 
    1. there are sister ports of this port already in the pkt scheduler; 
       which means find that PM and place phy_port in the corresponding lanes
    2. there is no sister port of this port already in the pkt scheduler; 
       which means find an empty PM and place phy_port in the corresponding lanes*/

    pkt_shpr_pm_indx = -1;
    for (i=0; i<HX5_SHAPING_GRP_LEN; i++) {
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
            for (j=0; j<(HX5_SHAPING_GRP_LEN/(pms_per_pipe/2)); j++){
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
            TDM_ERROR3("tdm_hx5_ovsb_pkt_shaper_find_pm_indx() PIPE %d pm_num=%d phy_port=%d Unable to find an available PM \n",pipe_id, pm_num, phy_port);
        }
    }

    if (pkt_shpr_pm_indx == -1) {
        TDM_ERROR3("tdm_hx5_ovsb_pkt_shaper_find_pm_indx() PIPE %d pm_num=%d phy_port=%d Unable to find an available PM \n",pipe_id, pm_num, phy_port);
    }

    return pkt_shpr_pm_indx;
}




/**
@name: tdm_hx5_ovsb_map_pm_num_to_pblk
@param:

Maps PM num (block_id) to OVSB pblk id
 */
int
tdm_hx5_ovsb_map_pm_num_to_pblk(tdm_mod_t *_tdm)
{
int pm; 
    int *pm2pblk_map = NULL;

    pm2pblk_map  = _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_num_to_pblk;

    for (pm = 0 ; pm < HX5_NUM_PM_MOD; pm++) {
        pm2pblk_map[pm] = -1;
        if(pm < 4)
	pm2pblk_map[pm] = 0x2; 
	else if (pm < 8)
	pm2pblk_map[pm] = 0x3; 
	else if (pm < 12)
	pm2pblk_map[pm] = 0x4; 
	else if (pm == 12)
	pm2pblk_map[pm] = 0x2; 
	else if (pm == 13)
	pm2pblk_map[pm] = 0x3; 
	else if (pm == 14)
	pm2pblk_map[pm] = 0x4; 
	else if (pm == 15)
	pm2pblk_map[pm] = 0x0; 
	else if (pm == 16)
	pm2pblk_map[pm] = 0x0; 
	else if (pm == 17)
	pm2pblk_map[pm] = 0x1; 
	else if (pm == 18)
	pm2pblk_map[pm] = 0x1; 

        TDM_PRINT2("TDM: map pm %2d to pblk %0d\n",
                       pm, pm2pblk_map[pm]);
        }
    TDM_SML_BAR

    return PASS;
}




/**
@name: tdm_hx5_ovsb_flexmapping
@param:

Generate sortable weightable groups for oversub round robin arbitration.
Flexible mapping:
[pipe 0] 1 - 64: hpipe0 and hpipe1
[pipe 1]65 -128: hpipe0 and hpipe1
 */
int
tdm_hx5_ovsb_flexmapping(tdm_mod_t *_tdm)
{	
    int i, j;
    tdm_calendar_t *cal;

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
    if(tdm_hx5_ovsb_apply_constraints(_tdm))
      TDM_PRINT1("OVSB Apply constraints PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    /* Partition  */
    if(tdm_hx5_ovsb_part_halfpipe(_tdm))
      TDM_PRINT1("OVSB PM Half Pipe pre-partition for PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_10G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("10G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("10G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_20G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("20G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("20G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_25G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("25G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("25G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_40G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("40G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("40G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_50G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("50G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("50G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_100G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("100G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("100G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    if(tdm_hx5_ovsb_pkt_shaper(_tdm)) {
      TDM_PRINT1("OVSB partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    }

    if(tdm_hx5_ovsb_map_pm_num_to_pblk(_tdm)) {
      TDM_PRINT1("OVSB partition for PIPE %d PM_NUM to PBLK mapping DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    }
    return PASS;

    
    
    
}


/**
@name: tdm_hx5_ovsb_fixmapping
@param:

Generate sortable weightable groups for oversub round robin arbitration
Fixed mapping:
[pipe 0] 1 - 32: hpipe 0
[pipe 0]33 - 64: hpipe 1
[pipe 1]65 - 96: hpipe 0
[pipe 1]97 -128: hpipe 1
 */
int
tdm_hx5_ovsb_fixmapping(tdm_mod_t *_tdm)
{	
    int i, j;
    tdm_calendar_t *cal;

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
        int port_phy, pms_per_pipe;
        int param_pipe_id, param_phy_lo, param_phy_hi,
            param_pm_lanes, param_pm_num;
        
        param_pipe_id = _tdm->_core_data.vars_pkg.cal_id;
        param_phy_lo  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
        param_phy_hi  = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
        param_pm_lanes= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
        param_pm_num  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
        
        pms_per_pipe  = param_pm_num/HX5_NUM_QUAD;

        COMPILER_REFERENCE(pms_per_pipe);

       /* pm_num_lo     = param_pipe_id*pms_per_pipe;
       * pm_num_hi     = (param_pipe_id+1)*pms_per_pipe-1;
       */
        for (i=0; i < (param_pm_num ); i++) {
            _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[i] = -1;
            for (j=0; j<param_pm_lanes; j++) {
                port_phy = i*param_pm_lanes+j+1;
                if (port_phy>=param_phy_lo && port_phy<=param_phy_hi) {
                    if ( (_tdm->_chip_data.soc_pkg.speed[port_phy] != SPEED_0) &&
                         (_tdm->_chip_data.soc_pkg.state[port_phy-1] == PORT_STATE__OVERSUB ||
                          _tdm->_chip_data.soc_pkg.state[port_phy-1] == PORT_STATE__OVERSUB_HG)) {
                            if (port_phy<= HX5_NUM_PHY_GPORTS) {
                                _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[i] = 0;
                           /*     TDM_PRINT1("on half pipe 0 port_phy %d \n ",port_phy); */
                            } 
			    else if ((port_phy<=72 ) && (port_phy >= 69 )) { /* to include first halfpipe first falcon */
                                _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[i] = 0;
                             /*   TDM_PRINT1("on half pipe 0 port_phy %d \n ",port_phy); */
                            } 
			    else if ((port_phy<=64 ) && (port_phy >= 61 )) { /* to include first halfpipe first falcon */
                                _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[i] = 0;
                             /*   TDM_PRINT1("on half pipe 0 port_phy %d \n ",port_phy); */
                            } 
			    else if (port_phy<=HX5_NUM_PHY_PORTS) {
                                _tdm->_chip_data.soc_pkg.soc_vars.hx5.pm_ovs_halfpipe[i] = 1;
                             /*   TDM_PRINT1("on half pipe 1 port_phy %d \n ",port_phy); */
                            } else {
                                TDM_ERROR2("Invalid port %d in pipe %d\n", port_phy, param_pipe_id);
                            }
                        break;
                    }
                }
            }
        }
    }
    /* Partition  */
    if(tdm_hx5_ovsb_part_halfpipe(_tdm))
      TDM_PRINT1("OVSB PM Half Pipe pre-partition for PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_10G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("10G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("10G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_20G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("20G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("20G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_25G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("25G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("25G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_40G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("40G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("40G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_50G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("50G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("50G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

    _tdm->_chip_data.soc_pkg.soc_vars.hx5.grp_speed = SPEED_100G;
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 0;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("100G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num); 
    }
    _tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num = 1;
    if(tdm_hx5_ovsb_fill_group(_tdm)) {
      TDM_PRINT2("100G OVSB partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.hx5.half_pipe_num);
    }

/*
 *   if (tdm_hx5_ovsb_pkt_shaper_new(_tdm)){
 *     TDM_PRINT1("OVSB partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
 *   }
 */
    if(tdm_hx5_ovsb_map_pm_num_to_pblk(_tdm)) {
      TDM_PRINT1("OVSB partition for PIPE %d PM_NUM to PBLK mapping DONE\n", _tdm->_core_data.vars_pkg.cal_id);
    }

    return PASS;

    
    
    
}

int
tdm_hx5_vbs_scheduler_ovs(tdm_mod_t *_tdm)
{	
    /* return (tdm_hx5_ovsb_flexmapping(_tdm)); */
    
    return (tdm_hx5_ovsb_fixmapping(_tdm));
}
