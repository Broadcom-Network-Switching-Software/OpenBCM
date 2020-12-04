/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core oversub algorithms
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif

/**
@name: tdm_th2_ovs_apply_constraints
@param:

Partition OVS PMs into 2 halfpipes
 */
int
tdm_th2_ovs_apply_constraints( tdm_mod_t *_tdm )
{
	int pms_per_pipe, pipe_id;
	int pm_num, ln_num, i;
	
	int speed_en_mtx[TH2_NUM_PHY_PM][6]; /* [pm_indx][speed_indx]  0 if <pm_indx> has <speed_indx> speed*/
	int speed_en_mtx_reduced[6];
	/* <speed_indx> 0: 10G; 1: 20G; 2: 25G; 3: 40G; 4: 50G; 5: 100G */
	int no_of_speeds_in_pipe;
	int phy_base_port, phy_port;
	int no_pms_hp[2];
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	/*TDM_PRINT2("tdm_th2_avs_part_halfpipe1() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/
	
	for (i=0; i<6; i++)  {
		for (pm_num=0; pm_num<TH2_NUM_PHY_PM; pm_num++) {
			speed_en_mtx[pm_num][i]=0;
		}
		speed_en_mtx_reduced[i]=0;
	}
	
	/* Build speed_en_mtx matrix*/
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = -1;
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
			((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
			 (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
				/* Coding for pm_ovs_halfpipe:
					-1 : not OVS;
					2 : OVS but not assigned
					0 : Half Pipe 0
					1 : Half Pipe 1 
				*/
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 2; /* OVS but not assigned */
				switch(_tdm->_chip_data.soc_pkg.speed[phy_port]) {
					case SPEED_10G : 
						speed_en_mtx[pm_num][INDX_10G] = 1; break;
					case SPEED_20G : 
						speed_en_mtx[pm_num][INDX_20G] = 1; break;
					case SPEED_25G : 
						speed_en_mtx[pm_num][INDX_25G] = 1; break;
					case SPEED_40G : 
						speed_en_mtx[pm_num][INDX_40G] = 1; break;
					case SPEED_50G : 
						speed_en_mtx[pm_num][INDX_50G] = 1; break;
					case SPEED_100G : 
						speed_en_mtx[pm_num][INDX_100G] = 1; break;
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
	
	no_pms_hp[0] = 0; no_pms_hp[1] = 0;
	if (no_of_speeds_in_pipe > 5) {
	/* Restriction 13:No port configurations with more than 4 port speed classes are supported. */
		TDM_ERROR2("tdm_th2_ovs_apply_constraints() PIPE %d No OVS port configurations with more than 4 port speed classes are supported; no_of_speeds_in_pipe=%d\n", pipe_id, no_of_speeds_in_pipe);
	
	}
	else if (no_of_speeds_in_pipe == 4) {
	/*Restriction 14: The only supported port configurations with 4 port speed classes are:
		10G/20G/40G/100G
		10G/25G/50G/100G
	*/
		if ( (speed_en_mtx_reduced[INDX_20G] || speed_en_mtx_reduced[INDX_40G]) && (speed_en_mtx_reduced[INDX_25G] || speed_en_mtx_reduced[INDX_50G]) ) {
		/* Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1*/
			TDM_PRINT1("tdm_th2_ovs_apply_constraints() PIPE %d applying Restriction 14 \n",pipe_id);
			for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
				if (speed_en_mtx[pm_num][INDX_25G] || speed_en_mtx[pm_num][INDX_50G]) {
					if (no_pms_hp[0] < (pms_per_pipe/2)) {
						_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
						no_pms_hp[0]++;
					}
				} else if (speed_en_mtx[pm_num][INDX_20G] || speed_en_mtx[pm_num][INDX_40G]) {
					if (no_pms_hp[1] < (pms_per_pipe/2)) {
						_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
						no_pms_hp[1]++;
					}
				}
			}
		}
	} 
	else if (no_of_speeds_in_pipe > 1) {
	/*Restriction 15: All port configurations with 1-3 port speed classes are supported, except
		configurations that contain both 20G and 25G port speeds.*/
		if ( speed_en_mtx_reduced[INDX_20G] && speed_en_mtx_reduced[INDX_25G]) {
		/* Group PMs with 25G/50G ports in HP0 and PMs with 20G/40G in HP1*/
			TDM_PRINT1("tdm_th2_ovs_apply_constraints() PIPE %d applying Restriction 15 \n",pipe_id);
			for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
				if (speed_en_mtx[pm_num][INDX_25G]) { /*if (speed_en_mtx[pm_num][INDX_25G] || speed_en_mtx[pm_num][INDX_50G]) {*/
					if (no_pms_hp[0] < (pms_per_pipe/2)) {
						_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
						no_pms_hp[0]++;
					}
				} else if (speed_en_mtx[pm_num][INDX_20G]) { /*} else if (speed_en_mtx[pm_num][INDX_20G] || speed_en_mtx[pm_num][INDX_40G]) {*/
					if (no_pms_hp[1] < (pms_per_pipe/2)) {
						_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
						no_pms_hp[1]++;
					}
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
@name: tdm_th2_ovs_part_halfpipe
@param:

Partition OVS PMs into 2 halfpipes
 */
int
tdm_th2_ovs_part_halfpipe( tdm_mod_t *_tdm )
{
	int pms_per_pipe, pipe_id;
	int pm_speed[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_num_subports[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_num_sort[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_num, ln_num, pm_indx, i, j;
	int phy_base_port, phy_port;
	int hp0_speed, hp1_speed;
	int no_pms_hp0, no_pms_hp1;
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	/*TDM_PRINT2("tdm_th2_avs_part_halfpipe1() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/
	
	pm_indx=0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		pm_speed[pm_indx] = 0;
		pm_num_subports[pm_indx] = 0;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
			((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
			 (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
				/*TDM_PRINT4("tdm_th2_avs_part_halfpipe2() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]);*/
				pm_speed[pm_indx] = pm_speed[pm_indx] + _tdm->_chip_data.soc_pkg.speed[phy_port]/1000;
				pm_num_subports[pm_indx]++;
			}
		}
		pm_num_sort[pm_indx] = pm_num;
		/*TDM_PRINT4("tdm_th2_avs_part_halfpipe2() pm_indx=%d pm_num=%d pm_speed=%d pm_num_subports=%d\n",pm_indx, pm_num_sort[pm_indx], pm_speed[pm_indx], pm_num_subports[pm_indx]);*/
		pm_indx++;
	}

        /* Sort speeds of the PMs in descending order - bubble sort*/
        for (i=0; i<pms_per_pipe-1; i++)
	{
		for (j=pms_per_pipe-1; j>i; j--)
		{
			if ( (pm_speed[j] > pm_speed[j-1]) || ((pm_speed[j] == pm_speed[j-1]) && (pm_num_subports[j] > pm_num_subports[j-1]))) /* swap j with j-1*/
			{
			int tmp;
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
		if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] == 0) {
			hp0_speed += pm_speed[i];
			no_pms_hp0++;
		}
		if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] == 1) {
			hp1_speed += pm_speed[i];
			no_pms_hp1++;
		}

	}
	
	/* Do partition of the PMs into two half pipes */
	for (i=0; i<pms_per_pipe; i++) {
		pm_num = pm_num_sort[i];
		if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] == 2) { /* is OVS unallocated PM */
			if ( ((no_pms_hp1 < (pms_per_pipe/2)) && (hp0_speed > hp1_speed)) || (no_pms_hp0 >= (pms_per_pipe/2)) )
			{
				hp1_speed = hp1_speed + pm_speed[i];
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
				no_pms_hp1++;
			}
			else
			{
				hp0_speed = hp0_speed + pm_speed[i];
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
				no_pms_hp0++;
			}
		}
	}
/*TDM_PRINT2("tdm_th2_avs_part_halfpipe3_1() hp0_speed=%d hp1_speed=%d\n", hp0_speed, hp1_speed);*/
	
	return PASS;
}




/**
@name: tdm_th2_ovs_fill_group
@param:

Partition OVS PMs into 2 halfpipes
 */
int
tdm_th2_ovs_fill_group( tdm_mod_t *_tdm)
{
	int pms_per_pipe, pipe_id;
	int pm_num_subports[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_num_sort[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_num, ln_num, pm_indx, i, j;

	int pms_with_grp_speed;
	int grp_speed;
	int half_pipe_num;
	int speed_max_num_ports_per_pm;
	int min_num_ovs_groups;
	tdm_calendar_t *cal;
	int start_ovs_group;

	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;
	
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	/*TDM_PRINT4("tdm_th2_ovs_fill_group1() pipe_id=%d pms_per_pipe=%d half_pipe_num=%d grp_speed=%d \n",pipe_id, pms_per_pipe, half_pipe_num, grp_speed);*/
	
	/* Count the number of subports per PM with grp_speed for PMs that belongs to this half pipe*/
	pm_indx=0;
	pms_with_grp_speed = 0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		int phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		pm_num_subports[pm_indx] = 0;
		if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] == half_pipe_num){
			for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
				int phy_port = phy_base_port+ln_num;
				if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
					/*TDM_PRINT4("tdm_th2_ovs_fill_group2() pm_num=%d ln_num=%d port=%d half_pipe=%d\n",pm_num, ln_num, phy_port, half_pipe_num);*/
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
			TDM_PRINT1("tdm_th2_ovs_fill_group3() Invalid group speed %0d\n",grp_speed);		
			return FAIL;							
	}
	/* ceil [(speed_max_num_ports_per_pm*pms_with_grp_speed) / TH2_OS_VBS_GRP_LEN ]*/
	min_num_ovs_groups = ((speed_max_num_ports_per_pm*pms_with_grp_speed) + TH2_OS_VBS_GRP_LEN-1) / TH2_OS_VBS_GRP_LEN;

	/* Execute only if ports with this speed exist*/
	if (min_num_ovs_groups > 0) {
		int start_group;
		int grp_index[TH2_OS_VBS_GRP_NUM];
		int phy_base_port;
		int phy_port;
		
	        /* Sort PMs in descending order of their number of subports with grp_speed*/
		for (i=0; i<pms_per_pipe-1; i++)
		{
			for (j=pms_per_pipe-1; j>i; j--)
			{
				if ( pm_num_subports[j] > pm_num_subports[j-1]) /* swap j with j-1*/
				{
				int tmp;
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
				TDM_PRINT1("tdm_th2_ovs_fill_group()Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
				return FAIL;
		}
		start_ovs_group = half_pipe_num*(TH2_OS_VBS_GRP_NUM/2);
		start_group = start_ovs_group;
		for (i=start_group; i< (start_group+(TH2_OS_VBS_GRP_NUM/2)); i++) {
			if (cal->cal_grp[i][0] != TH2_NUM_EXT_PORTS) {
				start_ovs_group++;
			} else { break;}
		}
		
		
		/* Fill ovs groups with ports having grp_speed*/
		for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) { grp_index[i] = 0;}
		for (i=0; i<pms_with_grp_speed; i++) {
			pm_num = pm_num_sort[i];
			phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] == half_pipe_num){
				for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
					phy_port = phy_base_port+ln_num;
					if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
						int t_ovs_grp_num;
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
@name: tdm_th2_ovs_pkt_shaper
@param:

Populates Pkt shaper calendar
 */
int
tdm_th2_ovs_pkt_shaper( tdm_mod_t *_tdm )
{
	int pms_per_pipe, pipe_id;
	int pm_num, ln_num;
	int phy_base_port, phy_port;
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
		
	
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
			((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
			 (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
				/*TDM_PRINT4("tdm_th2_ovs_pkt_shaper2() pm_num=%d ln_num=%d port=%d pm_speed=%d\n",pm_num, ln_num, phy_port, _tdm->_chip_data.soc_pkg.speed[phy_port]);*/
				_tdm->_core_data.vars_pkg.port = phy_port;
				tdm_th2_ovs_pkt_shaper_per_port(_tdm);
			}
		}
	}
	return PASS;
}



/**
@name: tdm_th2_ovs_pkt_shaper_per_port
@param:

Populates Pkt shaper calendar - per port
 */
int
tdm_th2_ovs_pkt_shaper_per_port( tdm_mod_t *_tdm )
{
	int pms_per_pipe;
	int pm_num, i;
	int phy_port;
	int half_pipe_num;
	
	int pkt_shpr_pm_indx;
	int no_of_lanes;
	int num_slots_for_port;
	int total_num_slots;
	int port_slot_tbl[SHAPING_GRP_LEN/8];
	int port_slot_tbl_shift[SHAPING_GRP_LEN/8];
	int slot_tbl_shift;
	int subport_no;
	int is_20G;
	int *pkt_shed_cal;
	int pkt_sched_repetitions;
	int base_offset;
	int max_pms_per_halfpipe;

	phy_port = _tdm->_core_data.vars_pkg.port;
	pm_num = (phy_port-1)/TH2_NUM_PM_LNS;
	half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	max_pms_per_halfpipe = pms_per_pipe/2;
	
	pkt_sched_repetitions = SHAPING_GRP_LEN/(pms_per_pipe/2)/TH2_NUM_PM_LNS; /* TH2: 5 = 160/8/4 */
	
	/*TDM_PRINT2("tdm_th2_ovs_pkt_shaper() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/
	
	/* Determine the pm_indx in pkt shceduler calendar - 0 to 7*/
	pkt_shpr_pm_indx = tdm_th2_ovs_pkt_shaper_find_pm_indx(_tdm);
	
	/* Find the number of lanes allocated to the port */
	is_20G=0;
	switch (_tdm->_chip_data.soc_pkg.speed[phy_port]) {
		case SPEED_10G:	no_of_lanes=1; break;
		case SPEED_20G:	no_of_lanes=2; is_20G = 1; break;
		case SPEED_40G:	no_of_lanes=2; break;
		case SPEED_25G:	no_of_lanes=1; break;
		case SPEED_50G:	no_of_lanes=2; break;
		case SPEED_100G: no_of_lanes=4; break;
		default:													
			TDM_PRINT1("tdm_th2_ovs_fill_group3() Invalid group speed %0d\n",_tdm->_chip_data.soc_pkg.speed[phy_port]);		
			return FAIL;							
	}	
	
	/* Compute the number of slots in pkt calendar for this port*/
	num_slots_for_port = _tdm->_chip_data.soc_pkg.speed[phy_port]/5000; /* each slots is 5G */
	
	/* Compute the total number of slots consumed by the lanes*/
	total_num_slots = 5*no_of_lanes;
	
	/* First, place num_slots_for_port slots in a table with total_num_slots equally spaced */
	for (i=0; i<SHAPING_GRP_LEN/max_pms_per_halfpipe; i++) {
		port_slot_tbl[i] = TH2_NUM_EXT_PORTS;
		port_slot_tbl_shift[i]= TH2_NUM_EXT_PORTS;
	}
	for (i=0; i<num_slots_for_port; i++) {
		port_slot_tbl[(i*total_num_slots)/num_slots_for_port] = phy_port;
	}
	
	
	/* Get the right pkt scheduler calendar*/
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		default:
			TDM_PRINT1("tdm_th2_ovs_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return FAIL;
	}
	
	/*  In order to spread tokens as uniformly as possible shift slot_tbl based on subport number */	
	subport_no = (phy_port-1)%TH2_NUM_PM_LNS;
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
		/*base_pos = 32*(i/no_of_lanes);*/
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
					TDM_PRINT1("tdm_th2_ovs_pkt_shaper_per_port() phy_port lane for phy_port=%d\n",phy_port);
					return FAIL;			
			}
		}

		cal_pos = base_pos + lane_pos + pkt_shpr_pm_indx;
		pkt_shed_cal[cal_pos] = port_slot_tbl_shift[i];
	}

	return PASS;
}


/**
@name: tdm_th2_ovs_pkt_shaper_find_pm_indx
@param:

Find an existing or available pm_indx in pkt_shaper
 */
int
tdm_th2_ovs_pkt_shaper_find_pm_indx( tdm_mod_t *_tdm )
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
	pm_num = tdm_th2_which_tsc(_tdm);
	half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;


	/* Get the right pkt scheduler calendar*/
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	pkt_shed_cal = _tdm->_chip_data.cal_0.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		case 1:	pkt_shed_cal = _tdm->_chip_data.cal_1.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		case 2:	pkt_shed_cal = _tdm->_chip_data.cal_2.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		case 3:	pkt_shed_cal = _tdm->_chip_data.cal_3.cal_grp[SHAPING_GRP_IDX_0+half_pipe_num]; break;
		default:
			TDM_PRINT1("tdm_th2_ovs_pkt_shaper_per_port() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return FAIL;
	}
	
	/* Two cases possible: 
	1. there are sister ports of this port already in the pkt scheduler; 
	   which means find that PM and place phy_port in the corresponding lanes
	2. there is no sister port of this port already in the pkt scheduler; 
	   which means find an empty PM and place phy_port in the corresponding lanes*/
	
	pkt_shpr_pm_indx = -1;
	for (i=0; i<SHAPING_GRP_LEN; i++) {
		if (pkt_shed_cal[i] != _tdm->_chip_data.soc_pkg.num_ext_ports) {
			_tdm->_core_data.vars_pkg.port = pkt_shed_cal[i];
			pm_num_t = tdm_th2_which_tsc(_tdm);
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
			for (j=0; j<(SHAPING_GRP_LEN/(pms_per_pipe/2)); j++){
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
			TDM_ERROR3("tdm_th2_ovs_pkt_shaper_find_pm_indx() PIPE %d pm_num=%d phy_port=%d Unable to find an available PM \n",pipe_id, pm_num, phy_port);
		}
	}
	
	if (pkt_shpr_pm_indx == -1) {
		TDM_ERROR3("tdm_th2_ovs_pkt_shaper_find_pm_indx() PIPE %d pm_num=%d phy_port=%d Unable to find an available PM \n",pipe_id, pm_num, phy_port);
	}

	return pkt_shpr_pm_indx;
}




/**
@name: tdm_th2_ovs_map_pm_num_to_pblk
@param:

Maps PM num (block_id) to OVS pblk id
 */
int
tdm_th2_ovs_map_pm_num_to_pblk( tdm_mod_t *_tdm )
{
	int pms_per_pipe, pipe_id;
	int pm_num;
	int half_pipe_num;
	int pblk_indx[2];
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	pblk_indx[0]=0;	pblk_indx[1]=0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[pm_num] = -1;
		half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
		if ((half_pipe_num==0) || (half_pipe_num==1)){
			_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[pm_num] = pblk_indx[half_pipe_num];
			pblk_indx[half_pipe_num]++;
		}
	}
	return PASS;
}




/**
@name: tdm_th2_vbs_scheduler_ovs
@param:

Generate sortable weightable groups for oversub round robin arbitration
 */
int
tdm_th2_vbs_scheduler_ovs( tdm_mod_t *_tdm )
{	
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
	TDM_BIG_BAR
	TDM_PRINT9("(1G - %0d) (10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) (120G - %0d) (Number of Oversub Types - %0d)\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7, (_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100 + _tdm->_core_data.vars_pkg.os_120));
	if ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100 + _tdm->_core_data.vars_pkg.os_120) > cal->grp_num) {
		TDM_ERROR0("Oversub speed type limit exceeded\n");
		return FAIL;
	}
	if (((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)==cal->grp_num && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>cal->grp_len || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>cal->grp_len || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2>cal->grp_len)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-1) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>32 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>32 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2>32)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-2) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>48 || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>48)) || ((_tdm->_core_data.vars_pkg.os_1 + _tdm->_core_data.vars_pkg.os_10 + _tdm->_core_data.vars_pkg.os_20 + _tdm->_core_data.vars_pkg.os_25 + _tdm->_core_data.vars_pkg.os_40 + _tdm->_core_data.vars_pkg.os_50 + _tdm->_core_data.vars_pkg.os_100)>=(cal->grp_num-3) && (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6>TDM_AUX_SIZE || _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1>TDM_AUX_SIZE))) {
		TDM_ERROR0("Oversub bucket overflow\n");
		return FAIL;
	}

	if(tdm_th2_ovs_apply_constraints(_tdm))
	  TDM_PRINT1("OVS Apply constraints PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

	if(tdm_th2_ovs_part_halfpipe(_tdm))
	  TDM_PRINT1("OVS PM Half Pipe partition for PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_10G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_20G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_25G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_40G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_50G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_100G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}

	if(tdm_th2_ovs_pkt_shaper(_tdm)) {
	  TDM_PRINT1("OVS partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}
	
	if(tdm_th2_ovs_map_pm_num_to_pblk(_tdm)) {
	  TDM_PRINT1("OVS partition for PIPE %d PM_NUM to PBLK mapping DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}	
	
	return PASS;

	
	
	
}
