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
	#include <tdm_th2_vec.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/tdm/tomahawk2/tdm_th2_vec.h>
#endif


/**
@name: tdm_th2_div_round_up
@param:
 Works only for positive integer numbers
 */
int tdm_th2_div_round_up(int nominator, int denominator)
{
	int round_up;

	if (denominator == 0) return 1;
	round_up = (nominator+denominator-1)/denominator;
	return round_up;
}

/**
@name: tdm_th2_div_round_down
@param:
 Works only for positive integer numbers
 */
int tdm_th2_div_round_down(int nominator, int denominator)
{
	int round_down;

	if (denominator == 0) return 1;
	round_down = nominator/denominator;
	return round_down;
}

/**
@name: tdm_th2_div_round
@param:
 Works only for positive integer numbers
 */
int tdm_th2_div_round(int nominator, int denominator)
{
	int modulo;

	if (denominator == 0) return 1;
	
	modulo = nominator % denominator;
	
	if (modulo >=((denominator+1)/2)) {
		return (tdm_th2_div_round_up(nominator,denominator));
	} else {
		return (tdm_th2_div_round_down(nominator,denominator));
	}
}


/**
@name: tdm_th2_vmap_alloc
@param:
 */
int
tdm_th2_vmap_alloc( tdm_mod_t *_tdm )
{
int iter;
int indx_start, indx_end;

    _tdm->_core_data.vars_pkg.os_enable = 1;
    _tdm->_core_data.vars_pkg.lr_enable = 0;

    if (_tdm->_chip_data.soc_pkg.flex_port_en == 0) { /* Init time */
        /* check if the pipe is in LR or OS */
        indx_start = _tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_start - 1;
        indx_end = _tdm->_chip_data.soc_pkg.soc_vars.th2.pipe_end;
    } else { /* FlexPort */
        /* check if the whole device is in LR or OS; 
         * covers cases where the whole pipe flexes down 
         */
        indx_start = 0;
        indx_end = TH2_NUM_EXT_PORTS - 8;
    }

    for (iter = indx_start; iter < indx_end; iter++) {
        if ( (_tdm->_chip_data.soc_pkg.state[iter] == PORT_STATE__LINERATE) ||
             (_tdm->_chip_data.soc_pkg.state[iter] == PORT_STATE__LINERATE_HG)) {
            _tdm->_core_data.vars_pkg.os_enable = 0;
            _tdm->_core_data.vars_pkg.lr_enable = 1;
        } else if ( (_tdm->_chip_data.soc_pkg.state[iter] == PORT_STATE__OVERSUB) ||
                    (_tdm->_chip_data.soc_pkg.state[iter] == PORT_STATE__OVERSUB_HG) ) {
            _tdm->_core_data.vars_pkg.os_enable = 1;
            _tdm->_core_data.vars_pkg.lr_enable = 0;
        }
    }

	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) );
}


/**
@name: tdm_th2_vbs_scheduler
@param:

Scheduler for the main TDM calendar (LR ports and OVS tokens)
 */
int
tdm_th2_vbs_scheduler( tdm_mod_t *_tdm )
{
	int i,j, k, s, token;
	int tdm_cal_length;
	int *tdm_pipe_main;
	int req_slots=0;
	int first_slot_pos;
	
	
	tdm_th2_vmap_alloc_v2(_tdm);
	
	TDM_PRINT3("tdm_th2_vbs_scheduler() PIPE=%d lr_enable=%d os_enable=%d\n", _tdm->_core_data.vars_pkg.cal_id, _tdm->_core_data.vars_pkg.lr_enable,_tdm->_core_data.vars_pkg.os_enable);
	
	tdm_cal_length = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;	

	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);


	/* If lr_enable place LR ports and AUX tokens
	   If os_enable place AUX tokens only
	*/
	if (_tdm->_core_data.vars_pkg.lr_enable || _tdm->_core_data.vars_pkg.os_enable) {
		for (i=0; i<(_tdm->_core_data.vmap_max_wid); i++) {
			int distinct_ports[8]; /* all distinct ports in this line*/
			int no_distinct_ports, match;
			
			/* Count the number of slots needed to be placed for this Port Group*/
			for (j=0; j<_tdm->_core_data.vmap_max_len; j++) {
				req_slots = j;
				if (_tdm->_core_data.vmap[i][j] == _tdm->_chip_data.soc_pkg.num_ext_ports) {
					break;
				}
			}
			
			if (req_slots == 0) continue; /* Nothing to be placed from this line */

			/*	STEP 4.3 Find the first available slot in the calendar PM<i>_first_slot;
				NOTE1: Also, the first slot could be found by randomly searching for an available slot in the range [0:PM<i>_distance-1];
				NOTE2: OR, to avoid clumping of unallocated ports, the first slot could be found in the middle of the biggest clump of unallocated ports in the range [0:PM<i>_distance-1];
				This will be good for mixed LR and OVS pipelines to spread OVS tokens as uniformly as possible.
			*/			
			first_slot_pos = tdm_th2_find_first_avail_slot(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports);
			
			no_distinct_ports = 1;
			distinct_ports[0] = _tdm->_core_data.vmap[i][0];
			for (j=0; j<req_slots; j++) {
				match = 0;
				for (k=0; k<no_distinct_ports;k++) {
					if ( (_tdm->_core_data.vmap[i][j] == distinct_ports[k]) || (_tdm->_core_data.vmap[i][j] == 555) ) {
						match=1;
					}
				}
				if (match==0) {
					distinct_ports[no_distinct_ports] = _tdm->_core_data.vmap[i][j];
					no_distinct_ports++;
				}
			}
			
			for (k=0; k<no_distinct_ports;k++) {
				TDM_PRINT2("tdm_th2_vbs_scheduler() Need to place in this line k=%d PhyPort=%d\n", k, distinct_ports[k]);
			}
			
		   for (k=0; k<no_distinct_ports;k++) {
			
			/*
			STEP4.3 for s=0: PM_num_slots-1 DO
			*/
			for (s=0; s < req_slots; s++) {
				int ideal_pos, port_req_slots, port_jitter;
				token = _tdm->_core_data.vmap[i][s];
				if (token == 555) continue; /* nothing to place here */
				if (token != distinct_ports[k]) continue; /* nothing to place here for now*/
			/*
				4.3.1 Pick the phy port to be placed based on subport placement table
				Skip steps 4.3.2/4.3.3/4.3.4 if Pl_Tbl[s%Pl_Tbl_Length]==N		
				PhyPort=PM_Phy_Port_Base + Pl_Tbl[s%Pl_Tbl_Length] (% - modulo)
				NOTE: PM_Phy_Port_Base is first physical port in PM<i> (subport 0)
				4.3.2 Compute the ideal position in the calendar where PhyPort needs to be placed
				Ideal_Pos = PM<i>_first_slot + floor((s* CAL_LENGTH)/PM_num_slots)
				4.3.3 Compute the jitter (tolerance from Ideal_Pos, PhyPort_Jitter)
				PhyPort_Jitter = rount((JITTER_PCT*CAL_LENGTH)/PM_num_slots/2/100)
				NOTE_1: uses floor to be more restrictive
				NOTE_2: to be even more restrictive use JITTER_PCT/2; mathematically this is the correct percentage to get 
				less then max jitter within JITTER_PCT between any two consecutive same port slots.
			*/
				ideal_pos = (first_slot_pos + tdm_th2_div_round(s*tdm_cal_length, req_slots)) % tdm_cal_length;  /* that is  round((s*tdm_cal_length)/pm_req_slots) */
				if (token < _tdm->_chip_data.soc_pkg.num_ext_ports) {
					port_req_slots = _tdm->_chip_data.soc_pkg.speed[token]/(BW_QUANTA*100);
					port_jitter = tdm_th2_div_round( (TH2_LR_JITTER_PCT*tdm_cal_length), (port_req_slots*100*2));
					port_jitter = (port_jitter == 0) ? 1 : port_jitter;
				} else {
					port_jitter = tdm_cal_length - 1;  /*  For AUX Ports use max possible jitter;  don't allow shifts */
				}
			/*	4.3.4 Place PhyPort in the calendar at Ideal_Pos with tolerance PhyPort_Jitter
			*/
				if (tdm_th2_place_lr_port(_tdm, tdm_pipe_main, token, ideal_pos, port_jitter, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports) == PASS) {
					/*TDM_PRINT3("tdm_th2_vbs_scheduler() Placing PhyPort=%d at Ideal_Pos=%d with Jitter=%d\n",phy_port, ideal_pos, port_jitter);*/
				} else {
					port_jitter = port_jitter / 2;
					port_jitter = (port_jitter == 0) ? 1 : port_jitter;
					TDM_PRINT3("tdm_th2_vbs_scheduler() 1st iter failed: Trying to place PhyPort=%d at Ideal_Pos=%d with Jitter=%d\n",token, ideal_pos, port_jitter);
					if (tdm_th2_place_lr_port(_tdm, tdm_pipe_main, token, ideal_pos, port_jitter, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports) == FAIL) {
						TDM_ERROR3("tdm_th2_vbs_scheduler() 2nd iter FAILED in Placing PhyPort=%d at Ideal_Pos=%d with Jitter=%d\n",token, ideal_pos, port_jitter);
					}
				}
			}
		   }

	TDM_PRINT2("tdm_th2_vbs_scheduler() AFter placing pm_num=%d pm_req_slots=%d\n", i, req_slots);
	for (s=0; s<tdm_cal_length; s++) {
		if (s%32==0) {
			TDM_PRINT0("\n");
			TDM_PRINT2("%3d : %3d\t",s, s+31);
		}
		
		if (tdm_pipe_main[s] != _tdm->_chip_data.soc_pkg.num_ext_ports)
			TDM_PRINT1("%5d",tdm_pipe_main[s]);
		else
			TDM_PRINT0("  ---");
	}
	TDM_PRINT0("\n\n");



		}
	}



   /* If PIPE is active then place AUX ports & OVS or IDLE tokens */
   if (_tdm->_core_data.vars_pkg.os_enable || _tdm->_core_data.vars_pkg.lr_enable) {


	/* Place AUX ports*/
	/*if (_tdm->_core_exec[TDM_CORE_EXEC__ACCESSORIZE]( _tdm )==FAIL) {*/
	if (tdm_th2_acc_alloc(_tdm)==FAIL) {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	/* OVS pipe: Place the OVS token for all unallocated slots */
	if (_tdm->_core_data.vars_pkg.os_enable && (!_tdm->_core_data.vars_pkg.lr_enable)) {
		for (j=0; j<tdm_cal_length; j++) {
			if (tdm_pipe_main[j]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
				tdm_pipe_main[j] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
			}
		}
	} else {
	/* LR pipe: Place IDLE tokens for all unallocated slots */
		for (j=0; j<tdm_cal_length; j++) {
			if (tdm_pipe_main[j]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
				tdm_pipe_main[j] = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token; /* Uncallocated slots in LR pipes get IDLE token */
			}
		}
	}
   }
	
	/* OVS grouping */
	if (_tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER_OVS](_tdm)==FAIL) {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__FILTER]( _tdm ) );
}



/**
@name: tdm_th2_get_place_table
@param:
 */
int
tdm_th2_get_pm_speed_and_place_table( enum port_speed_e *_pm_speeds, int num_lanes, int *place_tbl,  int *place_table_length)
{
	int is_100G=0, is_80G=0, is_40G=0;
	int i;
	int pm_sum=0;
	
	for (i=0; i<num_lanes; i++) {
		if (_pm_speeds[i] == SPEED_0) continue;
		if (_pm_speeds[i] % SPEED_25G == 0) {
			is_100G=1;
		}
		if ((_pm_speeds[i]==SPEED_40G) || (_pm_speeds[i]==SPEED_20G) || (_pm_speeds[i]==SPEED_10G)) {
			is_40G=1;
		}
		pm_sum += _pm_speeds[i];
	}
	
	if (is_40G && ((pm_sum > SPEED_40G) || (_pm_speeds[2]==SPEED_40G))) { /* Could be 0, 0, 40G, 0 */
		is_80G=1;
		is_40G=0;
	}
	
	
	if (is_100G) {
		(*place_table_length) = 4;
		if (_pm_speeds[0]==SPEED_100G) { place_tbl[0]=0; place_tbl[1]=0; place_tbl[2]=0; place_tbl[3]=0;} /* SINGLE */
		else {
			if (_pm_speeds[0]==SPEED_50G) {
				if (_pm_speeds[2]==SPEED_50G) {place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=2; }  /* DUAL */
				else { place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=3;}   /* TRI_0 */
			} else {
				if (_pm_speeds[2]==SPEED_50G) {place_tbl[0]=2; place_tbl[1]=0; place_tbl[2]=2; place_tbl[3]=1; }    /* TRI_1 */
				else { place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=1; place_tbl[3]=3;}    /* QUAD */
			}
		}
		return (SPEED_100G);
	} else if (is_40G) {
		(*place_table_length) = 4;
		if (_pm_speeds[0]==SPEED_40G) { place_tbl[0]=0; place_tbl[1]=0; place_tbl[2]=0; place_tbl[3]=0;} /* SINGLE */
		else {
			if (_pm_speeds[0]==SPEED_20G) {
				if (_pm_speeds[2]==SPEED_20G) {place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=2; }  /* DUAL */
				else { place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=3;}   /* TRI_0 */
			} else {
				if (_pm_speeds[2]==SPEED_20G) {place_tbl[0]=2; place_tbl[1]=0; place_tbl[2]=2; place_tbl[3]=1; }    /* TRI_1 */
				else { place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=1; place_tbl[3]=3;}    /* QUAD */
			}
		}
		return (SPEED_40G);
	} else if (is_80G){ /* is_80G */
		if ((_pm_speeds[0]==SPEED_40G) && (_pm_speeds[2]==SPEED_40G)) {
			(*place_table_length) = 4;
			place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=2;  /* DUAL 40/40 */
		} else if (_pm_speeds[0]==SPEED_20G) {                                   /* DUAL 20/40 */
			(*place_table_length) = 4;
			place_tbl[0]=2; place_tbl[1]=0; place_tbl[2]=2; place_tbl[3]=5;
		} else if (_pm_speeds[2]==SPEED_20G) {                                   /* DUAL 40/20 */
			(*place_table_length) = 4;
			place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=5;
		} else { /* TRI cases */
			(*place_table_length) = 8;
			if (_pm_speeds[0]==SPEED_40G) {
				place_tbl[0]=0; place_tbl[1]=2; place_tbl[2]=0; place_tbl[3]=5;    /* TRI_0 4:1:1;  5 - means don't place anything*/
				place_tbl[4]=0; place_tbl[5]=3; place_tbl[6]=0; place_tbl[7]=5;
			} else {
				place_tbl[0]=2; place_tbl[1]=0; place_tbl[2]=2; place_tbl[3]=5;    /* TRI_0 1:1:4 */
				place_tbl[4]=2; place_tbl[5]=1; place_tbl[6]=2; place_tbl[7]=5;
			}
		}
		return (2*SPEED_40G);
	} else {
		return (SPEED_0);
	}
}



/**
@name: tdm_th2_find_first_avail_slot
@param:
 */
int
tdm_th2_find_first_avail_slot(int *tdm_pipe_main, int tdm_cal_length, int avail_token)
{
	int i, j, max_clump_start, max_clump_slots, clump_start, clump_slots;
	int first_avail;

	/* Just return the first available token starting with index 0 */
/*
	for (i=0; i<tdm_cal_length; i++) {
		if (tdm_pipe_main[i] == avail_token) {
			return i;
		}
	}
	return -1;
*/

	/* Return the middle of the biggest clump in the calendar (including wraps) */
	max_clump_start=0;
	max_clump_slots=0;
	for (i=0; i<tdm_cal_length; i++) {
		if (tdm_pipe_main[i] != avail_token) {
			continue; 
		} else {
			clump_start= i;
			clump_slots=0;
			for (j=0; j<tdm_cal_length; j++) {
				if (tdm_pipe_main[(i+j)%tdm_cal_length] == avail_token) {
					clump_slots++;
				} else {
					break;
				}
			}
			if (clump_slots > max_clump_slots){
				max_clump_start = clump_start;
				max_clump_slots = clump_slots;
			}
			
		}
	}
	
	if (max_clump_slots==tdm_cal_length) {
		first_avail=0;
	} else {
		first_avail = (max_clump_start + (max_clump_slots/2)) % tdm_cal_length;
	}
	
	TDM_PRINT3("tdm_th2_find_first_avail_slot() finding at %d max_clump_start=%d max_clump_slots=%d\n", first_avail, max_clump_start, max_clump_slots);
	
	return (first_avail);


}



/**
@name: tdm_th2_find_first_avail_slot_w_start
@param:
 */
int
tdm_th2_find_first_avail_slot_w_start(int *tdm_pipe_main, int tdm_cal_length, int avail_token, int start_pos)
{
	int i, pos;

	/* Just return the first available token starting with index start_pos */
	for (i=0; i<tdm_cal_length; i++) {
		pos= (start_pos+i) % tdm_cal_length;
		if (tdm_pipe_main[pos] == avail_token) {
			return pos;
		}
	}
	return -1;
}




/**
@name: tdm_th2_vmap_alloc_v2
@param:
 */
int
tdm_th2_vmap_alloc_v2( tdm_mod_t *_tdm )
{
	int i,j, s, ln_num;
	int pipe_id, pms_per_pipe, pm_indx, pm_num, phy_base_port, phy_port;
	int pm_max_slots[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_total_slots[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_num_sort[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_max_speed, pm_req_slots;
	/*enum port_speed_e pm_max_speed=SPEED_0;*/

	for (i=0; i<(_tdm->_core_data.vmap_max_wid); i++) {
		TDM_MSET(_tdm->_core_data.vmap[i],_tdm->_chip_data.soc_pkg.num_ext_ports,_tdm->_core_data.vmap_max_len);
	}

	pipe_id = _tdm->_core_data.vars_pkg.cal_id;

   if (_tdm->_core_data.vars_pkg.lr_enable) {
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	/* STEP 1. Group ports by their corresponding PMs (PM<n>).*/
	/* STEP 2. Find the biggest speed port in each PM and compute the jitter for this port (PM<n>_min_jitter).
		PM_num_slots=PM_speed/QUANTA_BW
		PhyPort_Jitter = floor((20*CAL_LENGTH)/PM_num_slots/100)
	*/
	pm_indx=0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		pm_max_speed = 0;
		pm_total_slots[pm_indx] = 0;
		phy_base_port = _tdm->_chip_data.soc_pkg.pmap_num_lanes*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;

			if( (_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__LINERATE   ) ||
				 (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__LINERATE_HG) ) ) {
				if (_tdm->_chip_data.soc_pkg.speed[phy_port] > pm_max_speed) {
					pm_max_speed = _tdm->_chip_data.soc_pkg.speed[phy_port];
				}
				pm_total_slots[pm_indx] += _tdm->_chip_data.soc_pkg.speed[phy_port]/BW_QUANTA/100;
			}
		}
		pm_max_slots[pm_indx] = pm_max_speed/BW_QUANTA/100;
		pm_num_sort[pm_indx]  = pm_num;
		/*TDM_PRINT4("tdm_th2_avs_part_halfpipe2() pm_indx=%d pm_num=%d pm_speed=%d pm_num_subports=%d\n",pm_indx, pm_num_sort[pm_indx], pm_speed[pm_indx], pm_num_subports[pm_indx]);*/
		pm_indx++;
	}


	for (i=0; i<pms_per_pipe; i++)
	{
		TDM_PRINT3("tdm_th2_vmap_alloc_v2() pm_num=%d pm_max_slots=%d pm_num_sort=%d\n",i, pm_max_slots[i], pm_num_sort[i]);
	}

	/* STEP3. Sort PMs by their PM<n>_min_jitter in descending order (PM_min_jitter_sort_list). */
	/* This is equvalent with sorting pm_max_slots in descending order - bubble sort*/
	for (i=0; i<pms_per_pipe-1; i++)
	{
		for (j=pms_per_pipe-1; j>i; j--)
		{
			if ( (pm_max_slots[j] > pm_max_slots[j-1]) || 
			     ((pm_max_slots[j] == pm_max_slots[j-1]) && (pm_total_slots[j] > pm_total_slots[j-1]) ) ) /* swap j with j-1*/
			{
				int tmp;
				tmp = pm_num_sort[j];
				pm_num_sort[j] = pm_num_sort[j-1];
				pm_num_sort[j-1] = tmp;
				tmp = pm_max_slots[j];
				pm_max_slots[j] = pm_max_slots[j-1];
				pm_max_slots[j-1] = tmp;
				tmp = pm_total_slots[j];
				pm_total_slots[j] = pm_total_slots[j-1];
				pm_total_slots[j-1] = tmp;
			}
		}
	}

	for (i=0; i<pms_per_pipe; i++)
	{
		TDM_PRINT4("tdm_th2_vmap_alloc_v2() pm_num=%d pm_max_slots=%d pm_num_sort=%d pm_total_slots=%0d\n",i, pm_max_slots[i], pm_num_sort[i], pm_total_slots[i]);
	}


	
	/* STEP4. Foreach PM<i> in PM_min_jitter_sort_list DO: */
	for (i=0; i<pms_per_pipe; i++)
	{
		enum port_speed_e pm_speeds[TH2_NUM_PM_LNS];
		int place_tbl[8], place_table_length;

		TDM_PRINT3("tdm_th2_vmap_alloc_v2() pm_indx=%d pm_num=%d pm_max_slots=%d\n",i, pm_num_sort[i], pm_max_slots[i]);
		if (pm_max_slots[i] > 0) {
			pm_num = pm_num_sort[i];
			phy_base_port = _tdm->_chip_data.soc_pkg.pmap_num_lanes*pm_num+1;
			_tdm->_core_data.vars_pkg.port = phy_base_port; /* Pass to _tdm which PM needs to be placed */

		/*	STEP 4.1 Compute number of slots needed by PM 
			PM_num_slots=PM<i>_speed/QUANTA_BW
			(NOTE: for PM 40/10/10 configurations, PM_speed will be 80G and not 60G; thus, some slots will be left unoccupied)
		*/
		/* 	STEP 4.2 Generate a subport placement table (Pl_Tbl) based on port_mode and subport speeds; See table 1.1.
		*/
			
			for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
				phy_port = phy_base_port+ln_num;
				pm_speeds[ln_num] = _tdm->_chip_data.soc_pkg.speed[phy_port];
			}
			
			pm_max_speed = tdm_th2_get_pm_speed_and_place_table(pm_speeds, _tdm->_chip_data.soc_pkg.pmap_num_lanes, place_tbl, &place_table_length);
			pm_req_slots = pm_max_speed/(BW_QUANTA*100);
			
			TDM_PRINT3("tdm_th2_vmap_alloc_v2() pm_num=%d pm_max_speed=%dG pm_req_slots=%d\n",pm_num, pm_max_speed/1000, pm_req_slots);
			
			TDM_PRINT1("tdm_th2_vmap_alloc_v2() pm_num=%d Placing table is\n",pm_num);
			for (s=0; s<place_table_length; s++){
				TDM_PRINT1("%5d",place_tbl[s]);
			}
			TDM_PRINT0("\n\n");

		/*
			STEP4.3 for s=0: PM_num_slots-1 DO
		*/
			for (s=0; s<pm_req_slots; s++) {
				ln_num = place_tbl[s % place_table_length];
				if ((ln_num == 5)) {
					_tdm->_core_data.vmap[i][s]=555;
				} else {
					phy_port = phy_base_port+ln_num;
					if  (_tdm->_chip_data.soc_pkg.speed[phy_port] == SPEED_0) {
						_tdm->_core_data.vmap[i][s]=555;
					} else {
						_tdm->_core_data.vmap[i][s]=phy_port;					
					}
				}
			}
		}
	}
	

   }	

   /* If PIPE is active then place AUX ports & OVS or IDLE tokens */
   if (_tdm->_core_data.vars_pkg.os_enable || _tdm->_core_data.vars_pkg.lr_enable) {
   
	/* Find first available line */
	for (i=0; i<(_tdm->_core_data.vmap_max_wid); i++) {
		if (_tdm->_core_data.vmap[i][0] == _tdm->_chip_data.soc_pkg.num_ext_ports) {
			break;
		}
	}
	/* Place AUX tokens */
	
	/* CPU/MGM slots */
/*
	for (j=0; j<TH2_CPU_SLOTS; j++) {
		_tdm->_core_data.vmap[i][j] = TH2_ANCL_TOKEN;
	}
*/
	/* LBK slots */
/*
	i++;
	for (j=0; j<TH2_LBK_SLOTS; j++) {
		_tdm->_core_data.vmap[i][j] = TH2_ANCL_TOKEN+1;
	}
*/
	/* LBK slots */
/*
	i++;
	for (j=0; j<TH2_ANC_SLOTS; j++) {
		_tdm->_core_data.vmap[i][j] = TH2_ANCL_TOKEN+2;
	}
*/
   }
/*
	TDM_PRINT0("\n\n");
	TDM_SML_BAR
	TDM_PRINT1("\tdm_th2_vmap_alloc_v2() ALLOCATION Solution vmap pipe %0d:\n\n           ",((_tdm->_core_data.vmap[0][0])/64));
	tdm_print_vmap_vector(_tdm);
	TDM_PRINT0("\n\n\n");
	TDM_SML_BAR
*/



/*	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) );*/
	return PASS;
}




/**
@name: tdm_th2_get_min_max_jitter
@param:
 */
int
tdm_th2_get_min_max_jitter(int tbl_len, int port_slots, int lr_jitter_pct, int *min_spacing, int *max_spacing)
{
	int rem, jitter_range;

	if (port_slots == 0) {
		(*min_spacing) = 0;
		(*max_spacing) = 0;
		return FAIL;
	}

	rem = tbl_len % port_slots;
	jitter_range = tdm_th2_div_round( (2*tbl_len*lr_jitter_pct), (port_slots*100) ); /* 20% from ideal_spacing */
	
	if(rem >= ((port_slots+1)/2)) {
		(*min_spacing) = tdm_th2_div_round_up(tbl_len,port_slots)   - tdm_th2_div_round_up(jitter_range,2);       /*ROUNDUP(ideal_spacing[port_speed],0) - ROUNDUP(range[port_speed]/2, 0);*/
		(*max_spacing) = tdm_th2_div_round_up(tbl_len,port_slots)   + tdm_th2_div_round_down(jitter_range,2);     /*ROUNDUP(ideal_spacing[port_speed],0) + ROUNDDOWN(range[port_speed]/2, 0);*/
	} else {
		(*min_spacing) = tdm_th2_div_round_down(tbl_len,port_slots) - tdm_th2_div_round_down(jitter_range,2);	  /*ROUNDDOWN(ideal_spacing[port_speed],0) - ROUNDDOWN(range[port_speed]/2, 0);*/
		(*max_spacing) = tdm_th2_div_round_down(tbl_len,port_slots) + tdm_th2_div_round_up(jitter_range,2);	  /*ROUNDDOWN(ideal_spacing[port_speed],0) + ROUNDUP(range[port_speed]/2, 0);*/
	}

	return PASS;
}




/**
@name: tdm_th2_check_lr_placement
@param:

Checks if placement of phy_port in position pos will violate any sister port spacing or min/max jitter
 */
int
tdm_th2_check_lr_placement(tdm_mod_t *_tdm, int *tdm_pipe_main, int tbl_len, int phy_port, int pos, int lr_jitter_pct, int avail_token)
{

	int i, left_distance;
	int no_occur;
	int first_slot;
	int port_slots;
	int min_spacing, max_spacing;
	int pos_t, tsc_pos, tsc_pos_t;
	
	/* Check only regular port */
	if (phy_port < avail_token) {

		/* Check sister spacing */
		/* Sister port spacing for regular ports only */
		_tdm->_core_data.vars_pkg.port = phy_port;
		tsc_pos = tdm_find_pm(_tdm);
		for (i=1; i< 4; i++) {
			/* check sister port at left */
			pos_t = (tbl_len+pos-i)%tbl_len;
			if ( tdm_pipe_main[pos_t] < avail_token) {
				_tdm->_core_data.vars_pkg.port = tdm_pipe_main[pos_t];
				tsc_pos_t = tdm_find_pm(_tdm);
				if ((tsc_pos_t == tsc_pos)) {
					TDM_PRINT4("tdm_th2_check_lr_placement() ___WARNING RIGHT - Port=%d is violating sister spacing if placed in pos=%d; sister port is in pos=%d sis_port=%d \n", phy_port, pos, pos_t, tdm_pipe_main[pos_t]);
					return FAIL;
				}
			}
			/* check sister port at right */
			pos_t = (pos+i)%tbl_len;
			if ( tdm_pipe_main[pos_t] < avail_token) {
				_tdm->_core_data.vars_pkg.port = tdm_pipe_main[pos_t];
				tsc_pos_t = tdm_find_pm(_tdm);
				if ((tsc_pos_t == tsc_pos)) {
					TDM_PRINT3("tdm_th2_check_lr_placement() ___WARNING RIGHT - Port=%d is violating sister spacing if placed in pos=%d; sister port is in pos=%d\n", phy_port , pos, pos_t);
					return FAIL;
				}
			}
		}
		
		/* compute the distance from the same port located on the left side to ideal_pos;  If first slot to be placed ignore distance*/
		no_occur = 0 ;
		for (i=0; i < tbl_len; i++) {
			if (phy_port == tdm_pipe_main[i]) {
				no_occur++;
			}
		}
		first_slot = (no_occur==0) ? 1 : 0;
	
		if (first_slot==1) { /* just put something there to pass */
			return PASS;
		} else {
			/* compute min & max jitter */	
			port_slots = _tdm->_chip_data.soc_pkg.speed[phy_port]/(BW_QUANTA*100);
			tdm_th2_get_min_max_jitter(tbl_len, port_slots, TH2_LR_JITTER_PCT, &min_spacing, &max_spacing);
			/* compute the distance */
			left_distance=0;
			for (i=1; i < tbl_len; i++) {
				left_distance++;
				if (tdm_pipe_main[(tbl_len+pos-i)%tbl_len] == phy_port) { break; }
			}
			if ( (left_distance < min_spacing)  ||  (left_distance > max_spacing)  ||  (left_distance < _tdm->_core_data.rule__prox_port_min) ) {
				TDM_PRINT4("tdm_th2_check_lr_placement() ___WARNING Port=%d placed at pos=%d will be out of the jitter range min_spacing=%d max_spacing=%d\n", phy_port, pos, min_spacing, max_spacing);
				return FAIL;
			}
		}
	}
	
	return PASS;
}





/**
@name: tdm_th2_place_lr_port

@param:
 */
int
tdm_th2_place_lr_port(tdm_mod_t *_tdm, int *tdm_pipe_main, int phy_port, int ideal_pos, int port_jitter, int tdm_cal_length, int avail_token)
{
	int t, pos;
	int avail_pos_left=0, avail_pos_right=0;
	int left_slice_length, right_slice_length, i;
	int shift_jitter;
	int result, result_left, result_right;
	int is_sist_fail_left, is_sist_fail_right;
	int execute_left, execute_right;
	
/*	Try to place PhyPort in Ideal_Pos; if slot unavailable continue, else place & exit 4.3.4
*/
	if (tdm_pipe_main[ideal_pos] == avail_token &&
		(tdm_th2_check_lr_placement(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, ideal_pos, TH2_LR_JITTER_PCT, avail_token)==PASS) )
	{
		tdm_pipe_main[ideal_pos] = phy_port;
		TDM_PRINT4("tdm_th2_place_lr_port() Placing PhyPort=%d at position %d; IdealPos=%d t=%d\n", phy_port, ideal_pos, ideal_pos, 0);
		return PASS;
	}
	

/*
	for t= 1:PhyPort_Jitter DO {
try to place in Ideal_Pos-t; if slot unavailable continue, else place & exit 4.3.4
try to place in Ideal_Pos+t; if slot unavailable continue or (Ideal_Pos+t)>= CAL_LENGTH, else place & exit 4.3.4
}
*/
	for (t=1; t<= port_jitter; t++) {
		pos = (tdm_cal_length+ideal_pos-t) % tdm_cal_length;
		if ((tdm_pipe_main[pos] == avail_token) &&
			(tdm_th2_check_lr_placement(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, pos, TH2_LR_JITTER_PCT, avail_token)==PASS) )
		{
			tdm_pipe_main[pos] = phy_port;
			TDM_PRINT4("tdm_th2_place_lr_port() Placing PhyPort=%d at position %d; IdealPos=%d t=%d\n", phy_port, pos, ideal_pos, -t);
			return PASS;
		}
		pos = (ideal_pos+t) % tdm_cal_length;
		if ( (tdm_pipe_main[pos] == avail_token) &&
			(tdm_th2_check_lr_placement(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, pos, TH2_LR_JITTER_PCT, avail_token)==PASS) )
		{
			tdm_pipe_main[pos] = phy_port;
			TDM_PRINT4("tdm_th2_place_lr_port() Placing PhyPort=%d at position %d; IdealPos=%d t=%d\n", phy_port, pos, ideal_pos, t);
			return PASS;
		}
	}

	TDM_PRINT3("tdm_th2_place_lr_port() ___WARNING Failing to place PhyPort=%d at Ideal_Pos=%d with Jitter=%d; Try shifting slices\n",phy_port, ideal_pos, port_jitter);

/* If not placed in [ideal_pos-port_jitter ideal_pos+port_jitter] range the shift a slice of ports and place it within that range*/
/* First search for the smallest slice at left or right of the ideal_pos*/
			
	for (i=port_jitter; i<tdm_cal_length; i++) {
		avail_pos_left = (tdm_cal_length+ideal_pos-i) % tdm_cal_length;
		if (tdm_pipe_main[avail_pos_left] == avail_token) {
			break;
		}
	}
	for (i=port_jitter; i<tdm_cal_length; i++) {
		avail_pos_right = (ideal_pos+i) % tdm_cal_length;
		if (tdm_pipe_main[avail_pos_right] == avail_token) {
			break;
		}
	}
	
	shift_jitter=port_jitter-1;
	shift_jitter = (shift_jitter==0) ? 1: shift_jitter;

	left_slice_length  = (tdm_cal_length + ideal_pos - avail_pos_left) % tdm_cal_length;
	right_slice_length = (tdm_cal_length + avail_pos_right - ideal_pos) % tdm_cal_length;

	/* Try left and righ shift (its more of a swap) */
	is_sist_fail_left = 0; is_sist_fail_right = 0;
	result_left  = tdm_th2_shift_left(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, ideal_pos, shift_jitter, left_slice_length, avail_token, TH2_LR_JITTER_PCT, 1, &is_sist_fail_left); /* 1 means try */
	result_right = tdm_th2_shift_right(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, ideal_pos, shift_jitter, right_slice_length, avail_token, TH2_LR_JITTER_PCT, 1, &is_sist_fail_right);
	
	/* Take decision to shift and how to shift */
	execute_left=0;
	execute_right=0;
	if ( left_slice_length <= right_slice_length ) {
		if (result_left == PASS) {
			execute_left = 1;
		} else if (result_right == PASS) {
			execute_right = 1;
		} else {
			if (is_sist_fail_left == 0) {
				execute_left = 1;
			} else if (is_sist_fail_right == 0) {
				execute_right = 1;
			}
		}
	} else {
		if (result_right == PASS) {
			execute_right = 1;
		} else if (result_left == PASS) {
			execute_left = 1;
		} else {
			if (is_sist_fail_right == 0) {
				execute_right = 1;
			} else if (is_sist_fail_left == 0) {
				execute_left = 1;
			}
		}
	}

	if (execute_left == 1) {
		tdm_th2_shift_left(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, ideal_pos, shift_jitter, left_slice_length, avail_token, TH2_LR_JITTER_PCT, 0, &is_sist_fail_left);
	} else if (execute_right == 1){
		tdm_th2_shift_right(_tdm, tdm_pipe_main, tdm_cal_length, phy_port, ideal_pos, shift_jitter, right_slice_length, avail_token, TH2_LR_JITTER_PCT, 0, &is_sist_fail_right);
	}
	

	if ((execute_left == 1) || (execute_right == 1)) {
		result = PASS;
	} else {
		TDM_ERROR3("tdm_th2_place_lr_port() Unable to shift RIGHT/LEFT properly port=%d at ideal_pos=%d within jitter=%d\n", phy_port, ideal_pos, shift_jitter);
		result = FAIL;
	}

	return result;
}




/**
@name: tdm_th2_shift_left
@param:
 */
int
tdm_th2_shift_left(tdm_mod_t *_tdm, int *tdm_pipe_main, int tdm_cal_length, int phy_port, int ideal_pos, int shift_jitter, int left_slice_length, int avail_token, int lr_jitter_pct, int try_en, int *is_sist_fail)
{
	int i, pos1, pos2, pos;
	int *tdm_pipe_main_tmp;
	int result;
	int tsc_for_phy_port, tsc_pos;
	
	if (try_en == 1) { /* create a copy of the tdm_pipe_main */
		tdm_pipe_main_tmp=(int *) TDM_ALLOC(tdm_cal_length*sizeof(int), "temp main calendar");
		for (i=0; i<tdm_cal_length; i++) {
			tdm_pipe_main_tmp[i] = tdm_pipe_main[i];
		}
	} else { /* make actual change on tdm_pipe_main */
		tdm_pipe_main_tmp = tdm_pipe_main;
	}
	
	*is_sist_fail = 0;
	result=PASS;
	
	pos1 = (tdm_cal_length+ideal_pos-left_slice_length) % tdm_cal_length;
	pos2 = (tdm_cal_length+ideal_pos-left_slice_length+1) % tdm_cal_length;
	TDM_PRINT2("tdm_th2_shift_left() ___WARNING Shifting LEFT initial pos_1=%d with pos_2=%d\n", pos1, pos2);
	for (i=left_slice_length; i > shift_jitter ; i--) {
		if (tdm_th2_check_swap(_tdm, tdm_pipe_main_tmp, tdm_cal_length, pos1, pos2, avail_token, lr_jitter_pct)==PASS) {
			int tmp;
			tmp = tdm_pipe_main_tmp[pos1];
			tdm_pipe_main_tmp[pos1] = tdm_pipe_main_tmp[pos2];
			tdm_pipe_main_tmp[pos2] = tmp;
			TDM_PRINT2("tdm_th2_shift_left() ___WARNING Shifting LEFT swap pos_1=%d with pos_2=%d\n", pos1, pos2);
			pos1=pos2; /*pos1 = (pos1+1) % tdm_cal_length;*/
			pos2 = (pos2+1) % tdm_cal_length;
			result = PASS;
		} else { /* don't swap; just advance to another token */
			pos2 = (pos2+1) % tdm_cal_length;
			result = FAIL;
		}
	}
	
	if (result==FAIL) {
		TDM_PRINT4("tdm_th2_shift_left() ___WARNING Unable to swap LEFT and place properly port=%d at ideal_pos=%d pos1=%d pos2=%d\n", phy_port, ideal_pos, pos1, pos2);
		tdm_pipe_main_tmp[pos1] = phy_port; /* place it on unallocated position*/
		pos = pos1;
	} else {
		pos = (tdm_cal_length+ideal_pos-shift_jitter) % tdm_cal_length;
		tdm_pipe_main_tmp[pos] = phy_port;
		TDM_PRINT5("tdm_th2_shift_left() ___WARNING Shifting LEFT slice (left_slice_length=%d) pos [%d - %d] and placing PhyPort=%d at pos=%d\n",
			left_slice_length, (tdm_cal_length+ideal_pos-left_slice_length+1)%tdm_cal_length, pos2, phy_port, (tdm_cal_length+ideal_pos-shift_jitter)%tdm_cal_length );
	}
	
	
	if (try_en == 1) {
		/* Sister spacing violation for the port to be placed */
		_tdm->_core_data.vars_pkg.port = phy_port;
		tsc_for_phy_port = tdm_find_pm(_tdm);
		for (i=-3; i< 4; i++) {
			if (i == 0) continue;
			_tdm->_core_data.vars_pkg.port = tdm_pipe_main_tmp[(tdm_cal_length+pos+i) % tdm_cal_length];
			tsc_pos = tdm_find_pm(_tdm);
			if ((tsc_for_phy_port == tsc_pos)) {
				TDM_PRINT2("tdm_th2_shift_left() ___WARNING LEFT NEW - Port=%d is violating sister spacing if moved in pos=%d\n", phy_port, pos);
				result = FAIL;
				*is_sist_fail = 1;
			}
		}
		/* if just try then deallocate the copy of the tdm_pipe_main */
		TDM_FREE(tdm_pipe_main_tmp);
	}
	
	return result;
}



/**
@name: tdm_th2_shift_right
@param:
 */
int
tdm_th2_shift_right(tdm_mod_t *_tdm, int *tdm_pipe_main, int tdm_cal_length, int phy_port, int ideal_pos, int shift_jitter, int right_slice_length, int avail_token, int lr_jitter_pct, int try_en, int *is_sist_fail)
{
	int i, pos1, pos2, pos;
	int *tdm_pipe_main_tmp;
	int result;
	int tsc_for_phy_port, tsc_pos;
	
	if (try_en == 1) { /* create a copy of the tdm_pipe_main */
		tdm_pipe_main_tmp=(int *) TDM_ALLOC(tdm_cal_length*sizeof(int), "temp main calendar");
		for (i=0; i<tdm_cal_length; i++) {
			tdm_pipe_main_tmp[i] = tdm_pipe_main[i];
		}
	} else { /* make actual change on tdm_pipe_main */
		tdm_pipe_main_tmp = tdm_pipe_main;
	}
	
	*is_sist_fail = 0;
	result=PASS;
	
	pos2 = (ideal_pos+right_slice_length) % tdm_cal_length;
	pos1 = (ideal_pos+right_slice_length-1) % tdm_cal_length;
	for (i=right_slice_length; i > shift_jitter ; i--) {
		if (tdm_th2_check_swap(_tdm, tdm_pipe_main_tmp, tdm_cal_length, pos1, pos2, avail_token, lr_jitter_pct)==PASS) {
			int tmp;
			tmp = tdm_pipe_main_tmp[pos1];
			tdm_pipe_main_tmp[pos1] = tdm_pipe_main_tmp[pos2];
			tdm_pipe_main_tmp[pos2] = tmp;
			TDM_PRINT2("tdm_th2_shift_right() ___WARNING Shifting RIGHT swap pos_1=%d with pos_2=%d\n", pos1, pos2);
			pos2=pos1; /*pos2 = (tdm_cal_length+pos2-1) % tdm_cal_length;*/
			pos1 = (tdm_cal_length+pos1-1) % tdm_cal_length;				
			result = PASS;
		} else { /* don't swap; just advance to another token */
			pos1 = (tdm_cal_length+pos1-1) % tdm_cal_length;
			result = FAIL;
		}
	}
	if (result==FAIL) {
		TDM_PRINT4("tdm_th2_shift_right() ___WARNING Unable to swap RIGHT and place properly port=%d at ideal_pos=%d pos1=%d pos2=%d\n", phy_port, ideal_pos, pos1, pos2);
		tdm_pipe_main_tmp[pos2] = phy_port; /* place it on unallocated position*/
		pos = pos2;
	} else {
		pos = (ideal_pos+shift_jitter) % tdm_cal_length;
		tdm_pipe_main_tmp[pos] = phy_port;
		TDM_PRINT5("tdm_th2_shift_right() ___WARNING Shifting RIGHT slice (right_slice_length=%d) pos [%d - %d] and placing PhyPort=%d at pos=%d\n",
			right_slice_length, pos1, (ideal_pos+right_slice_length-1)%tdm_cal_length, phy_port, (ideal_pos+shift_jitter)%tdm_cal_length );
	}
	
	if (try_en == 1) {
		/* Sister spacing violation for the port to be placed */
		_tdm->_core_data.vars_pkg.port = phy_port;
		tsc_for_phy_port = tdm_find_pm(_tdm);
		for (i=-3; i< 4; i++) {
			if (i == 0) continue;
			_tdm->_core_data.vars_pkg.port = tdm_pipe_main_tmp[(tdm_cal_length+pos+i) % tdm_cal_length];
			tsc_pos = tdm_find_pm(_tdm);
			if ((tsc_for_phy_port == tsc_pos)) {
				TDM_PRINT2("tdm_th2_shift_right() ___WARNING LEFT NEW - Port=%d is violating sister spacing if moved in pos=%d\n", phy_port, pos);
				result = FAIL;
				*is_sist_fail = 1;
			}
		}
		/* if just try then deallocate the copy of the tdm_pipe_main */
		TDM_FREE(tdm_pipe_main_tmp);
	}
	
	return result;
}



/**
@name: tdm_th2_check_swap
@param:

Checks if swapping pos_1 with pos_2 will violate any min_spacing or jitter
Assumes that all tokens from pos_1 & pos_2 are placed in the table
Assumes that pos_2 is at the right of pos_1 in the table;
Thus, swap assumes that token in pos_1 is moving towards right and token in pos_2 is moving towards left

Before:
 - - - - pos_1 - - pos_2 - - 
After:
 - - - - pos_2 - - pos_1 - - 
 */
int
tdm_th2_check_swap(tdm_mod_t *_tdm, int *tdm_pipe_main, int tbl_len, int pos_1, int pos_2, int avail_token, int lr_jitter_pct)
{
	int i, pos, gap;
	int port_slots_1;
	int port_slots_2;
	
	int min_spacing_1, max_spacing_1, left_space_1, right_space_1;
	int min_spacing_2, max_spacing_2, left_space_2, right_space_2;
	
	int tsc_pos_1, tsc_pos_2, tsc_pos;
	
	int result=PASS;


	gap = (tbl_len+pos_2-pos_1) % tbl_len;
	
	if (tdm_pipe_main[pos_1] != avail_token) {
		port_slots_1 = 0;
		for (i=0; i< tbl_len; i++) {
			if (tdm_pipe_main[pos_1] == tdm_pipe_main[i]) {  port_slots_1++; }
		}

		tdm_th2_get_min_max_jitter(tbl_len, port_slots_1, lr_jitter_pct, &min_spacing_1, &max_spacing_1);

		right_space_1 = 0;
		for (i=1; i< tbl_len; i++) {
			pos = (pos_1+i) % tbl_len;
			right_space_1++;
			if(tdm_pipe_main[pos] ==  tdm_pipe_main[pos_1]) {
				break;
			}
		}
		left_space_1 = 0;
		for (i=1; i< tbl_len; i++) {
			pos = (tbl_len+pos_1-i) % tbl_len;
			left_space_1++;
			if(tdm_pipe_main[pos] ==  tdm_pipe_main[pos_1]) {
				break;
			}
		}

		/* If, by swapping, the distance to adjacent same tokens is outside jitter range, then return FAIL*/
		if ( ((left_space_1+gap) < min_spacing_1)  ||  ((left_space_1+gap) > max_spacing_1) || ((left_space_1+gap) < _tdm->_core_data.rule__prox_port_min) ) {
			TDM_PRINT4("tdm_th2_check_swap() ___WARNING LEFT + Port=%d at pos=%d  is out of jitter range min_spacing=%d max_spacing=%d\n", tdm_pipe_main[pos_1], pos_1, min_spacing_1, max_spacing_1);
			result = FAIL;
		}
		if ( ((right_space_1-gap) < min_spacing_1)  ||  ((right_space_1-gap) > max_spacing_1) || ((right_space_1-gap) < _tdm->_core_data.rule__prox_port_min) ) {
			TDM_PRINT4("tdm_th2_check_swap() ___WARNING LEFT - Port=%d at pos=%d  is out of jitter range min_spacing=%d max_spacing=%d\n", tdm_pipe_main[pos_1], pos_1, min_spacing_1, max_spacing_1);
			result = FAIL;
		}
		
		/* Sister port spacing for regular ports only */
		if (tdm_pipe_main[pos_1] < avail_token) {
			_tdm->_core_data.vars_pkg.port = tdm_pipe_main[pos_1];
			tsc_pos_1 = tdm_find_pm(_tdm);
			for (i=1; i< 4; i++) {
				_tdm->_core_data.vars_pkg.port = tdm_pipe_main[(pos_2+i)%tbl_len];
				tsc_pos = tdm_find_pm(_tdm);
				if ((tsc_pos_1 == tsc_pos)) {
					TDM_PRINT3("tdm_th2_check_swap() ___WARNING LEFT - Port=%d at pos=%d  is violating sister spacing if moved in pos=%d\n", tdm_pipe_main[pos_1], pos_1, pos_2);
					result = FAIL;
				}
			}
			
		}
	}
	
	if (tdm_pipe_main[pos_2] != avail_token) {
		port_slots_2 = 0;
		for (i=0; i< tbl_len; i++) {
			if (tdm_pipe_main[pos_2] == tdm_pipe_main[i]) {  port_slots_2++; }
		}

		tdm_th2_get_min_max_jitter(tbl_len, port_slots_2, lr_jitter_pct, &min_spacing_2, &max_spacing_2);
		right_space_2 = 0;
		for (i=1; i< tbl_len; i++) {
			pos = (pos_2+i) % tbl_len;
			right_space_2++;
			if(tdm_pipe_main[pos] ==  tdm_pipe_main[pos_2]) {
				break;
			}
		}
		left_space_2 = 0;
		for (i=1; i< tbl_len; i++) {
			pos = (tbl_len+pos_2-i) % tbl_len;
			left_space_2++;
			if(tdm_pipe_main[pos] ==  tdm_pipe_main[pos_2]) {
				break;
			}
		}
		if ( ((left_space_2-gap) < min_spacing_2)  ||  ((left_space_2-gap) > max_spacing_2) || ((left_space_2-gap) < _tdm->_core_data.rule__prox_port_min) ) {
			TDM_PRINT4("tdm_th2_check_swap() ___WARNING RIGHT - Port=%d at pos=%d  is out of jitter range min_spacing=%d max_spacing=%d\n", tdm_pipe_main[pos_2], pos_2, min_spacing_2, max_spacing_2);
			result = FAIL;
		}
		if ( ((right_space_2+gap) < min_spacing_2)  ||  ((right_space_2+gap) > max_spacing_2)  ||  ((right_space_2+gap) < _tdm->_core_data.rule__prox_port_min) ) {
			TDM_PRINT4("tdm_th2_check_swap() ___WARNING RIGHT + Port=%d at pos=%d  is out of jitter range min_spacing=%d max_spacing=%d\n", tdm_pipe_main[pos_2], pos_2, min_spacing_2, max_spacing_2);
			result = FAIL;
		}

		/* Sister port spacing for regular ports only */
		if (tdm_pipe_main[pos_2] < avail_token) {
			_tdm->_core_data.vars_pkg.port = tdm_pipe_main[pos_2];
			tsc_pos_2 = tdm_find_pm(_tdm);
			for (i=1; i< 4; i++) {
				_tdm->_core_data.vars_pkg.port = tdm_pipe_main[(tbl_len+pos_1-i)%tbl_len];
				tsc_pos = tdm_find_pm(_tdm);
				if ((tsc_pos_2 == tsc_pos)) {
					TDM_PRINT3("tdm_th2_check_swap() ___WARNING RIGHT - Port=%d at pos=%d  is violating sister spacing if moved in pos=%d\n", tdm_pipe_main[pos_2], pos_2, pos_1);
					result = FAIL;
				}
			}
			
		}

	}
	
	return result;
}




/**
@name: tdm_th2_acc_alloc
@param:
 */
int
tdm_th2_acc_alloc( tdm_mod_t *_tdm )
{
	int s, j, existing_avail_tokens;
	int tdm_cal_length, req_slots, first_slot_pos, ideal_pos;
	int *tdm_pipe_main;
	int round_aux_tokens;
	int port_jitter;
	
	tdm_cal_length = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	round_aux_tokens = _tdm->_chip_data.soc_pkg.tvec_size+2; /* in TH2 is 12*/
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	
	existing_avail_tokens=0;
	for (j=0; j<tdm_cal_length; j++) {
		if (tdm_pipe_main[j] == _tdm->_chip_data.soc_pkg.num_ext_ports) {
			existing_avail_tokens++;
		}
	}
	
	if (existing_avail_tokens < _tdm->_chip_data.soc_pkg.tvec_size) {
		TDM_ERROR2("tdm_th2_acc_alloc() Not enough available tokens for AUX ports Required=%d Actual%d\n",_tdm->_chip_data.soc_pkg.tvec_size, existing_avail_tokens);
		return FAIL;
	}
	
	/* Place TOKEN (TH2_ANCL_TOKEN) for CMIC/MGM; the actual token will be placed in parse function */
	req_slots = TH2_CPU_SLOTS; /* in TH2 is same as TH2_MGM_SLOTS */
	if (existing_avail_tokens < round_aux_tokens) {
		first_slot_pos = tdm_th2_find_first_avail_slot(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports);
	} else {
		first_slot_pos = tdm_th2_find_first_avail_slot_w_start(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports, 0 );
	}
	for (s=0; s<req_slots; s++) {
		/*ideal_pos = (first_slot_pos + (( (s*tdm_cal_length) + req_slots-1) / req_slots)) % tdm_cal_length;*/ /* that is  round((s*tdm_cal_length)/req_slots) */
		ideal_pos = (first_slot_pos + tdm_th2_div_round(s*tdm_cal_length, req_slots)) % tdm_cal_length; /* that is  round((s*tdm_cal_length)/req_slots) */
		port_jitter = tdm_th2_div_round( (TH2_LR_JITTER_PCT*tdm_cal_length), (req_slots*100*2));
		if (tdm_th2_place_lr_port(_tdm, tdm_pipe_main, TH2_ANCL_TOKEN, ideal_pos, port_jitter, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports))
			TDM_PRINT2("tdm_th2_acc_alloc() Placing CMIC/MGM token at Ideal_Pos=%d with Jitter=%d\n", ideal_pos, tdm_cal_length-1);		
	}
	
	
	/* Place TOKEN (TH2_ANCL_TOKEN+1) for LBK; the actual token will be placed in parse function */
	req_slots = TH2_LBK_SLOTS;
	if (existing_avail_tokens < round_aux_tokens) {
		first_slot_pos = tdm_th2_find_first_avail_slot(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports);
	} else {
		first_slot_pos = tdm_th2_find_first_avail_slot_w_start(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports, (tdm_cal_length/round_aux_tokens) );
	}	
	for (s=0; s<req_slots; s++) {
		/*ideal_pos = (first_slot_pos + (( (s*tdm_cal_length) + req_slots-1) / req_slots)) % tdm_cal_length;*/ /* that is  round((s*tdm_cal_length)/req_slots) */
		ideal_pos = (first_slot_pos + tdm_th2_div_round(s*tdm_cal_length, req_slots)) % tdm_cal_length; /* that is  round((s*tdm_cal_length)/req_slots) */
		if (tdm_th2_place_lr_port(_tdm, tdm_pipe_main, TH2_ANCL_TOKEN+1, ideal_pos, tdm_cal_length-1, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports))
			TDM_PRINT2("tdm_th2_acc_alloc() Placing LBK token at Ideal_Pos=%d with Jitter=%d\n", ideal_pos, tdm_cal_length-1);		
	}
	
	/* Place TOKEN (TH2_ANCL_TOKEN+2) for OPPORTUNISTIC, IDLE, NUL slots;  the actual token will be placed in parse function  */
	req_slots = TH2_ANC_SLOTS; /* OPPT + IDLE + NULL */
	if (existing_avail_tokens < round_aux_tokens) {
		first_slot_pos = tdm_th2_find_first_avail_slot(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports);
	} else {
		first_slot_pos = tdm_th2_find_first_avail_slot_w_start(tdm_pipe_main, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports, ((2*tdm_cal_length)/round_aux_tokens) );
	}
	for (s=0; s<req_slots; s++) {
		/*ideal_pos = (first_slot_pos + (( (s*tdm_cal_length) + req_slots-1) / req_slots)) % tdm_cal_length;*/ /* that is  round((s*tdm_cal_length)/req_slots) */
		ideal_pos = (first_slot_pos + tdm_th2_div_round(s*tdm_cal_length, req_slots)) % tdm_cal_length; /* that is  round((s*tdm_cal_length)/req_slots) */
		if (tdm_th2_place_lr_port(_tdm, tdm_pipe_main, TH2_ANCL_TOKEN+2, ideal_pos, tdm_cal_length-1, tdm_cal_length, _tdm->_chip_data.soc_pkg.num_ext_ports))
			TDM_PRINT2("tdm_th2_acc_alloc() Placing OPPT/IDLE/NULL token at Ideal_Pos=%d with Jitter=%d\n", ideal_pos, tdm_cal_length-1);		
	}
	
	
	return PASS;
}

