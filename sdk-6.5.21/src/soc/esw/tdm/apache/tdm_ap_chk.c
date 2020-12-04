/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip self-check operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_ap_chk_ethernet
@param: int, enum[], int[][], int[]

Returns BOOL_TRUE or BOOL_FALSE depending on if pipe of the given port has traffic entirely Ethernet.
 */
int tdm_ap_chk_ethernet(tdm_mod_t *_tdm)
{
	int result, chk[2];
	chk[0] = BOOL_FALSE; chk[1] = BOOL_FALSE;
	
	/* PIPE_X */
	if(_tdm->_core_data.vars_pkg.pipe == AP_PIPE_X_ID){
		_tdm->_core_data.vars_pkg.port = 1;
		chk[0] = tdm_ap_check_ethernet(_tdm);
		_tdm->_core_data.vars_pkg.port = 37;
		chk[1] = tdm_ap_check_ethernet(_tdm);
	}
	result = (chk[0] && chk[1]) ? (BOOL_TRUE) : (BOOL_FALSE);
	return result;
	
}


/**
@name: tdm_ap_chk_struc
@param:

Checks length, overflow, and symmetry (determinism between IDB and MMU tables)
 */
int tdm_ap_chk_struc(tdm_mod_t *_tdm)
{
	int result, max_len, tbl_len=0, freq;
	
	result = PASS;
	freq   = _tdm->_chip_data.soc_pkg.clk_freq;
		
	/* Ethernet */
	if ( tdm_ap_chk_ethernet(_tdm) ) {
		switch (freq) {
      /*AP*/
			case 933: max_len=233; break;
			case 840: max_len=212; break;
			case 793: 
                        case 794: 
                        case 795: 
                        case 796: 
                        case 797: 
                          max_len=402; break;
			case 575: max_len=145; break;
			case 510: max_len=129; break;
			case 435: max_len=110; break;

			default:
				TDM_ERROR0("TDM structure check received unrecognized frequency\n");
				return UNDEF;
				break;
		}
	}
	/* Highgig */
	else {
		switch (freq) {
      /*AP*/
			case 933: max_len=233; break;
			case 840: max_len=212; break;
			case 793: 
                        case 794:
                        case 795: 
                        case 796: 
                        case 797: 
                          max_len=402; break;
			case 575: max_len=145; break;
			case 510: max_len=129; break;
			case 435: max_len=110; break;
			default:
				TDM_ERROR0("TDM structure check received unrecognized frequency\n");
				return UNDEF;
				break;
		}	
	}
	
	/* Actual calender length */
	switch (_tdm->_core_data.vars_pkg.pipe){
		case AP_PIPE_X_ID: tbl_len= _tdm->_chip_data.cal_2.cal_len; break;
		default:
			TDM_ERROR0("TDM structure check received unrecognized pipe ID\n");
			return UNDEF;
			break;
	}
	
	if (tbl_len>max_len){
		result=FAIL; 
		TDM_ERROR3("TDM pipe %0d TDM length %0d slots, spec %0d slots\n",_tdm->_core_data.vars_pkg.pipe,tbl_len,max_len);
	}
	return result;
}


/**
@name: tdm_ap_chk_tsc
@param:

Verifies TSC transcription of all ports indexed in bcm configuration
 */
int tdm_ap_chk_tsc(tdm_mod_t *_tdm)
{
	int i, j, k, result=PASS, result_i, num_lanes; 
	int **port_pmap, num_pm_lanes, num_ext_ports;
	enum port_speed_e *port_speeds;
	enum port_state_e *port_states;
	
	port_states  = _tdm->_chip_data.soc_pkg.state;
	port_speeds  = _tdm->_chip_data.soc_pkg.speed;
	port_pmap    = _tdm->_chip_data.soc_pkg.pmap;
	num_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
	num_ext_ports= _tdm->_chip_data.soc_pkg.num_ext_ports;
	/*num_phy_pm   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;*/
	
	for (i=1; i<(num_ext_ports-57); i++){
		/* check if port is active */
		if (  port_states[i-1]==PORT_STATE__LINERATE    || 
			  port_states[i-1]==PORT_STATE__OVERSUB     ||
			  port_states[i-1]==PORT_STATE__LINERATE_HG ||
			  port_states[i-1]==PORT_STATE__OVERSUB_HG  ){
			result_i = PASS;
			/* Check tsc numbering */
			_tdm->_core_data.vars_pkg.port = i;
			  if(tdm_ap_which_tsc(_tdm)==num_ext_ports){
			  	TDM_ERROR1("TDM TSC check failed to transcribe port %0d\n",i);
			  	result_i = FAIL;
			  }
			  /* Check legal lane configs to see if port speed can fit (AP) */
			  else{
			  	num_lanes=0;
			  	if ( (i ==17 || i==53) && port_speeds[i]>=SPEED_100G){
			  	  	for (k=0; k<3; k++){
			  	  		for(j=0; j<num_pm_lanes; j++){
			  	  			if(port_pmap[(i-1+k)/num_pm_lanes][j]==i){++num_lanes;}
			  	  		}
			  	  	}
            }
			  	else{
			  		for(j=0; j<num_pm_lanes; j++){
			  			if(port_pmap[(i-1)/num_pm_lanes][j]==i){++num_lanes;}
			  		}
			  	}
			  	
			  	switch (port_speeds[i]) {
			  		case SPEED_1G: 
			  		case SPEED_2p5G:
			  		case SPEED_10G: 
			  		case SPEED_10G_DUAL:
			  		case SPEED_10G_XAUI:
			  		case SPEED_25G:
			  			if (num_lanes!=1 && num_lanes !=4) {result_i = FAIL;} 
			  			break;
			  		case SPEED_20G:
              if((i>= 29 && i<= 35) || (i>= 65 && i<= 71)) {
			  			if (num_lanes!=1) {result_i = FAIL;} 
			  			break;
              }
              else { if (num_lanes!=2) {result_i = FAIL;}
              break;
              }
			  		case SPEED_50G:
			  			if (num_lanes!=2) {result_i = FAIL;} 
			  			break;
			  		case SPEED_40G: 
			  		case SPEED_42G:
              if((i>= 29 && i<= 35) || (i>= 65 && i<= 71)) {
			  			if (num_lanes!=2) {result_i = FAIL;} 
			  			break;
              }
              else { if (num_lanes!=4) {result_i = FAIL;}
              break;
              }
			  		case SPEED_100G:
              if( i==17 || i==53){ 
			  			if (num_lanes<10) {result_i = FAIL;}
			  			break;
              }
              else { if (num_lanes!=4) {result_i = FAIL;}
              break;
              }
			  		case SPEED_120G:
			  			if (num_lanes<12) {result_i = FAIL;}
			  			break;
			  		default:
			  			result_i = UNDEF;
			  			break;			
			  	}
			  	if (result_i==FAIL) {
			  		TDM_ERROR2("TDM TSC check found illegal TSC config for port %0d speed %0dG\n",i,(port_speeds[i]/1000));
			  		result = FAIL;
			  	}
			  	else if (result_i==UNDEF){
			  		TDM_ERROR2("TDM TSC check found unrecognized speed %0dG for port %0d\n",(port_speeds[i]/1000),i);
			  		result = FAIL;
			  	}
			  }
	  }
  }
	
	return result;
}


/**
@name: tdm_ap_chk_min
@param:

Checks min spacing in TDM (TSC sister port spacing)
 */
int tdm_ap_chk_min(tdm_mod_t *_tdm)
{
	int i, j, result=PASS, cal_len, *cal_main; 
	const char *table;
	unsigned short mmu_mtbl[1024];
	

	/* check pipe ID */
	switch (_tdm->_core_data.vars_pkg.pipe) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 2: table = "MMU Pipe 0"; 
				cal_main=_tdm->_chip_data.cal_2.cal_main; 
				cal_len=_tdm->_chip_data.cal_2.cal_len; 
				break;

		default:
			result = UNDEF;
			TDM_ERROR1("Min spacing check failed, unrecognized PIPE ID %0d\n",_tdm->_core_data.vars_pkg.pipe);
			break;
	}
	if (result!=PASS){	return result;}
	
	/* Mirror the tables to make loopback easier to navigate */
	for (i=0; i<cal_len; i++) {
		mmu_mtbl[i]        = cal_main[i]; 
		mmu_mtbl[i+cal_len]= cal_main[i];
	}
	/* Check Min same port spacing for each active port */
	for (i=0; i<cal_len; i++){
 		AP_TOKEN_CHECK(mmu_mtbl[i]) {
      for (j=1; j<VBS_MIN_SPACING; j++) {
    	  if(mmu_mtbl[i+j] == mmu_mtbl[i]) {
						result = FAIL;
						TDM_ERROR4("%s MIN same port spacing violation port %0d, (#%03d | #%03d)\n",table,mmu_mtbl[i],i,(i+j));
				}
      }
    }
  }

  /*Nak , no sister port spacing requirement in AP */
/*	for (i=1; i<cal_len; i++){
		AP_TOKEN_CHECK(mmu_mtbl[i]) {
			tsc0 = tdm_ap_legacy_which_tsc(mmu_mtbl[i],tsc);
			for (j=1; j<VBS_MIN_SPACING; j++) {
				AP_TOKEN_CHECK(mmu_mtbl[i+j]) {
					tsc1 = tdm_ap_legacy_which_tsc(mmu_mtbl[i+j],tsc);
					if (tsc0==tsc1){
						result = FAIL;
						TDM_ERROR8("%s MIN spacing violation port %0d, (#%03d | #%03d), TSC: [%0d, %0d, %0d, %0d]\n",table,mmu_mtbl[i],i,(i+j),
							tsc[tdm_ap_legacy_which_tsc(mmu_mtbl[i+j],tsc)][0],
							tsc[tdm_ap_legacy_which_tsc(mmu_mtbl[i+j],tsc)][1],
							tsc[tdm_ap_legacy_which_tsc(mmu_mtbl[i+j],tsc)][2],
							tsc[tdm_ap_legacy_which_tsc(mmu_mtbl[i+j],tsc)][3]);
					}
				}
			}
		}
	}*/
	/* Check Min same port spacing for each CPU and LPBK port */
	for (i=0; i<cal_len; i++){
    if(mmu_mtbl[i] == AP_CPU_PORT) {
      for (j=1; j<LLS_MIN_SPACING; j++) {
    	  if(mmu_mtbl[i+j] == AP_CPU_PORT ) {
						result = FAIL;
						TDM_ERROR4("%s MIN same port spacing violation for CPU port %0d, (#%03d | #%03d)\n",table,mmu_mtbl[i],i,(i+j));
				}
      }
    }
  }
	for (i=0; i<cal_len; i++){
    if(mmu_mtbl[i] == AP_LOOPBACK_PORT) {
      for (j=1; j<LLS_MIN_SPACING; j++) {
    	  if(mmu_mtbl[i+j] == AP_LOOPBACK_PORT ) {
						result = FAIL;
						TDM_ERROR4("%s MIN same port spacing violation for LOOPBACK port %0d, (#%03d | #%03d)\n",table,mmu_mtbl[i],i,(i+j));
				}
      }
    }
  }	
	return result;
}


/**
@name: tdm_ap_chk_sub_lr
@param:

Checks that all line rate nodes were extracted from solution matrix
 */
int tdm_ap_chk_sub_lr( tdm_mod_t *_tdm)
{
	int i, j, idx_0, idx_1, result=PASS, result_i, port_spd_M, port_spd_G, found_nodes,
	    *cal_main, cal_len, pipe_id, clk_freq;
	const char *table;
	enum port_speed_e *port_speeds;
	enum port_state_e *port_states;
	
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
	port_states= _tdm->_chip_data.soc_pkg.state;
	
	/* check pipe ID */
	switch (pipe_id) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 2: table    = "MMU Pipe 0"; 
				cal_main = _tdm->_chip_data.cal_2.cal_main; 
				cal_len  = _tdm->_chip_data.cal_2.cal_len;
				idx_0    = 1;
				idx_1    = 72;
				break;
		default: 
			result = UNDEF;
			TDM_ERROR1("Linerate subscription check failed, unrecognized PIPE ID %0d\n",pipe_id);
			break;
	}
	if (result!=PASS){return result;}
	
	/* Check frequency */
	if (clk_freq>933) {
		TDM_ERROR2("Linerate subscription check failed in %s, unrecognized chip frequency %d\n",table,clk_freq );
		return UNDEF;
	}
	/* Check linerate port speed */
	for (i=idx_0; i<=idx_1; i++) {
		if (port_states[i-1]==PORT_STATE__LINERATE   || 
			port_states[i-1]==PORT_STATE__LINERATE_HG){
			result_i = PASS;
			
			/* Check speed type */
			port_spd_M = port_speeds[i];
			port_spd_G = port_spd_M/1000;
			switch (port_spd_G){
				case 1:
				case 2:
				case 10:
				case 20: case 21:
				case 25:
				case 40: case 42:
				case 50:
				case 100: 
				case 120:
					break;
				default:
					result_i = UNDEF;
					TDM_ERROR3("Linerate subscription check failed in %s, unrecognized speed %0d (Gbps) for port %0d\n",table,port_spd_G,i);
					break;
			}
			
			/* Check port rate */
			found_nodes=0;
			for (j=0; j<cal_len; j++) {
				if (cal_main[j]==i) {
					found_nodes++;
				}
			}
			if(found_nodes==0){
				TDM_ERROR3("Linerate subscription check failed in %s, port %0d is enabled but not scheduled in calender, %0d slots\n", table, i,found_nodes);
				result_i = FAIL;
			}
			else if ( found_nodes < ((port_spd_M*10)/1000/(BW_QUANTA)) ) {
				TDM_ERROR5("Linerate subscription failed in %s, %0dG vector for port %0d expected %0d nodes (found %0d)\n",table,port_spd_G,i,((port_spd_M*10)/1000/(BW_QUANTA)),found_nodes);
				result_i = FAIL;
			}
			
			result = (result==FAIL) ? (result): (result_i);
		}	
	}
	
	return result;
}


/**
@name: tdm_ap_chk_sub_os
@param:

Checks that the oversub node is sorted into oversub speed groups
 */
int tdm_ap_chk_sub_os(tdm_mod_t *_tdm)
{
	int i, j, k, idx_0, idx_1, result=PASS, result_i, group_idx, slot_idx, is_found,
	    **cal_grp, cal_grp_num, cal_grp_len,pipe_id, port_spd_M;
	const char *table;
	enum port_speed_e *port_speeds;
	enum port_state_e *port_states;
	
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
	port_states= _tdm->_chip_data.soc_pkg.state;
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	
	/* check pipe id */
	switch (pipe_id) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 2: table = "MMU Pipe 0"; 
				cal_grp     = _tdm->_chip_data.cal_2.cal_grp; 
				cal_grp_num = _tdm->_chip_data.cal_2.grp_num; 
				cal_grp_len = _tdm->_chip_data.cal_2.grp_len;
				idx_0       = 1;
				idx_1       = 72;
				break;
		default:
			result = UNDEF;
			TDM_ERROR1("Oversub subscription check failed, unrecognized PIPE ID %0d\n",pipe_id);
			break;
	}
	if (result!=PASS){return result;}
	
	
	for (i=idx_0; i<=idx_1; i++){
		if (port_states[i-1]==PORT_STATE__OVERSUB   ||
			port_states[i-1]==PORT_STATE__OVERSUB_HG){
			result_i   = PASS;
			port_spd_M = port_speeds[i];
			/* determine ovsb speed group */
			group_idx  = 0;
			slot_idx   = 0;
			is_found   = BOOL_FALSE;
			for (j=0; j<cal_grp_num ; j++) {
				for (k=0; k<cal_grp_len; k++){
					if((cal_grp[j][k]) == i){
						is_found  = BOOL_TRUE;
						group_idx = j;
						slot_idx  = k;
						break;
					}
				}
			}
			/* check port speed with the corresponding speed group */
			if (is_found == BOOL_FALSE) {
				TDM_ERROR1("Oversub subscription check failed, could not find passed port %0d in any oversub speed group\n",i);
				result_i = FAIL;
			}
			else if (port_spd_M>=SPEED_10G && (port_spd_M>(port_speeds[cal_grp[group_idx][0]]+1010) || port_spd_M<(port_speeds[cal_grp[group_idx][0]]-1010))) {
				TDM_ERROR6("Oversub subscription check failed in %s, OVSB group %0d slot #%0d port %0d speed %0dG mismatches %0dG group assginment\n",table,group_idx,slot_idx,i,(port_spd_M/1000),port_speeds[cal_grp[group_idx][0]]/1000);
				result_i = FAIL;
			}
			result = (result==FAIL) ? (result): (result_i);
		}
	}
	
	return result;
}


/**
@name: tdm_ap_chk_sub_mgmt
@param:

Checks that CPU bandwidth is satisfied
 */
int tdm_ap_chk_sub_cpu( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, cpu_slots_required,  
	    *cal_main, cal_len, clk_freq; 
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_main   = _tdm->_chip_data.cal_2.cal_main; 
	cal_len    = _tdm->_chip_data.cal_2.cal_len; 
	
	/* determine number of CPU slots  */
	cpu_slots_required = 2;
	if (clk_freq==793 || clk_freq==794 || clk_freq == 795 || clk_freq == 796 || clk_freq == 797) {
		cpu_slots_required=4;
	}
	/* calculate number of actual CPU slots */
	k=0;
	for (i=0; i<cal_len; i++){
		if(cal_main[i] == AP_CPU_PORT){
			k++;
		}
	}
	
	if (k<cpu_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe , CPU port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,cpu_slots_required);
	}
	
	return result;
}


/**
@name: tdm_ap_chk_sub_lpbk
@param:

Checks that LOOPBACK bandwidth is satisfied
 */
int tdm_ap_chk_sub_lpbk( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, lb_slots_required,
	    *cal_main, cal_len, clk_freq;
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_main   = _tdm->_chip_data.cal_2.cal_main; 
	cal_len    = _tdm->_chip_data.cal_2.cal_len; 
	
	/* determine number of loopback slots  */
	lb_slots_required = 2;
	if (clk_freq==793 || clk_freq==794 || clk_freq == 795 || clk_freq == 796 || clk_freq == 797) {
			lb_slots_required=4;
	}
	else if (clk_freq==575 || clk_freq==510) {
			lb_slots_required=2;
	}
	else if (clk_freq==435){
		lb_slots_required=3;
	}
	/* calculate number of actual loopback slots */
	k=0;
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==AP_LOOPBACK_PORT) {
			k++;
		}
	}
	if (k<lb_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe , loopback port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,lb_slots_required);
	}
	
	return result;
}


/**
@name: tdm_ap_chk_sub_lpbk
@param:

Checks that AGNOSTIC ACCESSORY bandwidth is satisfied
 */
int tdm_ap_chk_sub_acc( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, ac_slots_required,
	    *x_cal_main, x_cal_len, clk_freq;
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	x_cal_main = _tdm->_chip_data.cal_2.cal_main; 
	x_cal_len  = _tdm->_chip_data.cal_2.cal_len;

	/* check pipe X */
		ac_slots_required=9;

    if (clk_freq==793 || clk_freq==794  || clk_freq == 795 || clk_freq == 796 || clk_freq == 797){
		ac_slots_required=18;
	  }
		else if (clk_freq==575 || clk_freq==510) {
			ac_slots_required=9;
		}
	  else if (clk_freq==435){
		ac_slots_required=10;
	  }

		k=0;
		for (i=0; i<x_cal_len; i++) {
			if (x_cal_main[i]==AP_CPU_PORT      ||
				x_cal_main[i]==AP_LOOPBACK_PORT ||
				x_cal_main[i]==AP_AUX_TOKEN    ||
				x_cal_main[i]==AP_SBUS_TOKEN    ||
				x_cal_main[i]==AP_IDL1_TOKEN    ||
				x_cal_main[i]==AP_IDL2_TOKEN    ){
				k++;
			}
		}
		
		if (k<ac_slots_required) {
			result = FAIL;
			TDM_ERROR2("Subscription check failed in MMU pipe X, accessory bandwidth is insufficient in this configuration - %0d slots of %0d required\n",k,ac_slots_required);
		}
	
	return result;
}

/**
@name: tdm_ap_chk_subscription
@param:

Check subscription status per port 
 */
int tdm_ap_chk_subscription(tdm_mod_t *_tdm)
{
	int rst, result=PASS;
	
	/* 1. Check CPU port */
	if (FAIL==tdm_ap_chk_sub_cpu(_tdm)){
		result = FAIL;
	}
	/* 2. Check Loopback port */
	if (FAIL==tdm_ap_chk_sub_lpbk(_tdm)){
		result = FAIL;
	}
	/* 3. Check Agnostic accessory slots (must be equal to 8 or 10 in TD2/TD2+) */
	if (FAIL==tdm_ap_chk_sub_acc(_tdm)){
		result = FAIL;
	}
	
	/* 4. Check linerate ports */
	_tdm->_core_data.vars_pkg.pipe = AP_XPIPE_CAL_ID;
	rst  = tdm_ap_chk_sub_lr(_tdm);
	if(PASS!=rst){
		result = (result==FAIL) ? (result): (rst);
	}
	

	/* 5. Check oversub ports */
	_tdm->_core_data.vars_pkg.pipe = AP_XPIPE_CAL_ID;
	rst  = tdm_ap_chk_sub_os(_tdm);
	if(PASS!=rst){
		result = (result==FAIL) ? (result): (rst);
	}
	
	return result;
}

/**
@name: tdm_ap_chk_os_jitter
@param:

Checks distribution of oversub tokens to maximize performance of oversub scheduler
 */
int tdm_ap_chk_os_jitter(tdm_mod_t *_tdm)
{	
	int i, result=PASS, *cal_main, cal_len, os_range_thresh, os_clump_ave, os_clump_size, os_cnt_idx, lr_cnt_idx, group_size,	clump_cnt=0, lr_bw=0, os_bw=0;
	int ratio_opt, ratio_ave=0, ratio_sum1=0, ratio_sum2=0, ratio_var=0, ratio_i, ratio_floor, ratio_ceil;
	int dist_opt, dist_floor, dist_ceil, dist_i, dist_sum1=0, dist_sum2=0, dist_ave=0, dist_var=0;
	const char *table;
	
	switch (_tdm->_core_data.vars_pkg.pipe) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 2: table = "MMU Pipe 0"; 
				cal_main=_tdm->_chip_data.cal_2.cal_main; 
				cal_len=_tdm->_chip_data.cal_2.cal_len; 
				break;
		default: 
			TDM_ERROR1("Oversub jitter check failed, unrecognized PIPE ID %0d\n",_tdm->_core_data.vars_pkg.pipe);
			result = UNDEF; 
			break;
	}
	if (result!=PASS) {return result;}
	
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==AP_OVSB_TOKEN) {
			os_bw+=BW_QUANTA;
		}
		else {
			lr_bw+=BW_QUANTA;
		}
      TDM_PRINT3("os bw=%d lr bw=%d Ap_token=%d \n",  os_bw, lr_bw, AP_OVSB_TOKEN );
	}
	if (lr_bw==0 || os_bw==0) {
		TDM_PRINT1("TDM: Not applicable in - %s\n",table);
		TDM_SML_BAR
		return result;
	}
	
	TDM_PRINT1("TDM: Analyzing oversub - %s\n",table);
	dist_opt    =  (100*((os_bw+lr_bw)/BW_QUANTA)/(os_bw/BW_QUANTA));
	ratio_opt   = dist_opt;
	os_clump_ave= (100*(os_bw/BW_QUANTA))/((lr_bw/BW_QUANTA)+1);
	
	if ((os_bw/10)>=200) {
		os_range_thresh = 100*OS_RANGE_BW_HI;
	}
	else if ( ((os_bw/10)<200) && ((os_bw/10)>=100) ) {
		os_range_thresh = 100*OS_RANGE_BW_MD;
	}
	else {
		os_range_thresh = 100*JITTER_THRESH_MH;
	}
	ratio_floor = ((10000/(dist_opt+os_range_thresh)));
	ratio_ceil  = ((10000/(dist_opt-os_range_thresh)));
	dist_floor  = ((dist_opt-os_range_thresh)>0)? (dist_opt-os_range_thresh): (100);
	dist_ceil   = dist_opt+os_range_thresh;
	
	TDM_PRINT4("TDM:%12s|%12s|%12s|%12s\n","Ratio","Optimal", "Floor","Ceiling");
	TDM_PRINT4("TDM:%12s|%12d|%12d|%12d\n"," ",ratio_opt,ratio_floor,ratio_ceil);
	
	for (i=1; i<cal_len; i++) {
		if (cal_main[i]==AP_OVSB_TOKEN) {
			os_clump_size=100; os_cnt_idx=i; lr_cnt_idx=i;
			while (os_cnt_idx<cal_len && cal_main[++os_cnt_idx]==AP_OVSB_TOKEN) {os_clump_size += 100;}
			group_size=os_clump_size;
			while(lr_cnt_idx>0 && cal_main[--lr_cnt_idx]!=AP_OVSB_TOKEN) {group_size += 100;}
			
			ratio_i = (100*os_clump_size/(group_size));
			dist_i  = (100*group_size)/(os_clump_size);
			
			TDM_PRINT4("\t\t[%3d]\t|\tSlots = %2d\t|\tSlice size = %2d\t|\tGrant ratio = %0d\n",i,os_clump_size,group_size,ratio_i);
			if ( (ratio_i<ratio_floor) || (ratio_i>ratio_ceil) ) {
				TDM_PRINT2("\t\tGrant ratio suboptimal range is [%0d, %0d]\n",ratio_floor,ratio_ceil);
			}
			ratio_sum1+=ratio_i;
			ratio_sum2+=((ratio_i-ratio_opt)*(ratio_i-ratio_opt))/100;
			dist_sum1 += dist_i;
			dist_sum2 += ((dist_i-dist_opt)*(dist_i-dist_opt))/100;
			clump_cnt++;
			
			if (os_clump_size>(os_clump_ave+os_range_thresh)) {
				TDM_PRINT3("\t\tOversub slice suboptimal, size %0d index %0d, limit %0d\n",os_clump_size,i,(os_clump_ave+os_range_thresh));
			}
			while (cal_main[i+1]==AP_OVSB_TOKEN) {i++;}
		}
	}
	if(clump_cnt!=0){
		ratio_ave = (ratio_sum1)/clump_cnt;
		ratio_var = tdm_sqrt(ratio_sum2/clump_cnt);
		dist_ave  = dist_sum1/clump_cnt;
		dist_var  = tdm_sqrt(dist_sum2/clump_cnt);
	
		if ( (ratio_ave<ratio_floor) || (ratio_ave>ratio_ceil) || (dist_var>os_range_thresh) ) {
			result = FAIL;
		}
		if ( (dist_ave<dist_floor) || (dist_ave>dist_ceil) || (dist_var>os_range_thresh) ) {
			result = FAIL;
		}
	}
	
	TDM_SML_BAR
	TDM_PRINT1("TDM:\tTracked %0d oversub slices in calendar\n",clump_cnt);
	TDM_PRINT2("TDM:\t\t\tRatio    - Optimal Range  = [%0d, %0d]\n",ratio_floor, ratio_ceil);
	TDM_PRINT1("TDM:\t\t\tRatio    - Optimal Value  = %0d\n",ratio_opt);
	TDM_PRINT1("TDM:\t\t\tRatio    - Grant Average  = %0d\n",ratio_ave);
	TDM_PRINT1("TDM:\t\t\tRatio    - Grant Variance = %0d\n",ratio_var);
	TDM_PRINT0("\n");
	TDM_PRINT2("TDM:\t\t\tDistance - Optimal Range    = [%0d, %0d]\n",dist_floor, dist_ceil);
	TDM_PRINT1("TDM:\t\t\tDistance - Optimal Value    = %0d\n",dist_opt);
	TDM_PRINT1("TDM:\t\t\tDistance - Jitter Tolerance = %0d slots\n",os_range_thresh);
	TDM_PRINT1("TDM:\t\t\tDistance - Grant Average    = %0d\n",dist_ave);
	TDM_PRINT1("TDM:\t\t\tDistance - Grant Variance   = %0d\n",dist_var);
	TDM_SML_BAR
	
	return result;
}


/**
@name: tdm_ap_chk_lr_jitter
@param:

Checks jitter of line rate ports
 */
int tdm_ap_chk_lr_jitter(tdm_mod_t *_tdm, int fail[16])
{
	int i, j, num=1, avg=0, rate=0, ports_done[AP_NUM_EXT_PORTS], done_idx=0, exists, jitter, is_spd_valid;
	int *cal_main, cal_len, pipe_id, clk_freq, result=PASS, distance, port_spd_G, port_phyID, mirror_tbl[1024], tracker[AP_LR_VBS_LEN];
	const char *table;
	enum port_speed_e *port_speeds;
	
	fail[6] = PASS;
	fail[5] = (fail[5]==UNDEF || fail[5]==PASS) ? (PASS) : (fail[5]);
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	
	for (i=0; i<(AP_NUM_EXT_PORTS-56); i++) {ports_done[i]=AP_NUM_EXT_PORTS;}
	
	switch (pipe_id) {
		/*
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break; */
		case 2: table = "MMU Pipe 0"; 
				cal_main=_tdm->_chip_data.cal_2.cal_main; 
				cal_len=_tdm->_chip_data.cal_2.cal_len; 
				break;
		default:
			fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(UNDEF);
			fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(UNDEF);
			TDM_ERROR1("Line-rate jitter check failed, unrecognized PIPE ID %0d\n",pipe_id);
			result = UNDEF; 
			break;
	}
	if (result!=PASS) {return result;}
	
	/* Construct "tracker" based on a mirror table of cal_main */
	for (i=0; i<cal_len; i++) {mirror_tbl[i] = cal_main[i]; mirror_tbl[i+cal_len] = cal_main[i];}
	for (i=0; i<cal_len; i++) {tracker[i] = 0;}
	for (i=0; i<cal_len; i++) {
		AP_TOKEN_CHECK(mirror_tbl[i]){
			distance=0;
			for (j=1; j<cal_len; j++) {
				distance++;
				if (mirror_tbl[i+j]==mirror_tbl[i]) {tracker[i]=distance; break;}
			}
		}
	}
	
	/* Check Line-rate ports */
	for (i=0; i<cal_len; i++) {
		if (tracker[i]==0) {continue;}
			port_phyID = cal_main[i];
			
			exists=BOOL_FALSE;
			for (j=0; j<done_idx; j++) {
				if (ports_done[j]==port_phyID) {
					exists=BOOL_TRUE;
					break;
				}
			}
			if (exists==BOOL_FALSE) {
				ports_done[done_idx++]=port_phyID;
				
				/* Set jitter threshold */
				port_spd_G   = port_speeds[port_phyID]/1000;
				is_spd_valid = BOOL_TRUE;
				switch(port_spd_G) {
					case 1: case 2:
						jitter = cal_len;
						break;
					case 10:
						jitter = 21; /* Jitter range from SW TDM Generation Spec for Apache */
						break;
					case 20: case 21: case 25:
						jitter = JITTER_THRESH_MH;
						break;
					case 40:case 42:case 50:
						jitter = 5;
						break;
					case 100: case 120:
						jitter = 2;
						break;
					default:
						fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(UNDEF);
						fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(UNDEF);
						TDM_ERROR3("Line-rate jitter check failed in %s, unrecognized port speed %0d at port %0d\n",table,port_spd_G,port_phyID);
						is_spd_valid = BOOL_FALSE;
						break;
				}
				
				/* Check linerate port jitter */
				if(is_spd_valid==BOOL_TRUE){
					/* Calculate grant speed */
					num=0;
					for (j=0; j<cal_len; j++) {
						if (cal_main[j]==port_phyID) {
							++num;
						}
					}
					avg  = (cal_len/num);
					rate = (((PKT_SIZE+PKT_PREAMBLE+PKT_IPG)*800)/(2*avg*(100000/clk_freq)));
					/* Check grant speed with expected speed */
					if (rate < port_spd_G) {
						/* 
						fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(FAIL);
						TDM_PRINT4("TDM: _____WARNING: Line-rate jitter checker in %s, port %0d projected rate %0dG to operate under line rate %0dG\n",table,port_phyID,rate,port_spd_G); */
						fail[6] = FAIL;
						TDM_ERROR4("Line-rate jitter checker in %s, port %0d projected rate %0dG to operate under line rate %0dG\n",table,port_phyID,rate,port_spd_G);
					}
					/* Check jitters for all slots allocated to the same port. */
					for (j=0; j<cal_len; j++){
						if (cal_main[j]==port_phyID) {
							if (tracker[j] > (avg+1+jitter) ) {
								/* fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(FAIL);
								TDM_PRINT6("TDM: _____WARNING: Line-rate jitter checker in %s, excess jitter, %0dG port %0d index %0d past threshold %0d with %0d\n",table,port_spd_G,port_phyID,j,(avg+1+jitter),tracker[j]); */
								fail[5] = FAIL;
								TDM_ERROR6("Line-rate jitter checker in %s, excess jitter, %0dG port %0d index %0d past threshold %0d with %0d\n",table,port_spd_G,port_phyID,j,(avg+1+jitter),tracker[j]);
							}
						}
					}
				}
			}
	  }

	return ((fail[5]==PASS && fail[6]==PASS) ? (PASS) : (FAIL));
}

/**
@name: tdm_ap_chk_lls
@param:

Checks linked list scheduler
 */
int tdm_ap_chk_lls(tdm_mod_t *_tdm)
{
	int i, j, k, port_phyID, result=PASS, *cal_main, cal_len, pipe_id, clk_freq, min_spacing, line_rate=BOOL_TRUE;
	const char *table;
	enum port_speed_e *port_speeds;
	
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
  clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	
	switch (pipe_id) {
		case 2: table = "MMU Pipe 0"; 
				cal_main=_tdm->_chip_data.cal_2.cal_main; 
				cal_len=_tdm->_chip_data.cal_2.cal_len; 
				break;
		default: 
			TDM_ERROR1("LLS check failed, unrecognized PIPE ID %0d\n",pipe_id);
			result = UNDEF; 
			break;
	}
	if (result!=PASS) {return result;}

	for (i=0; i<cal_len; i++) {
		if(cal_main[i] == AP_OVSB_TOKEN){
    line_rate= BOOL_FALSE;
    break;
    }
  }
 if(line_rate) {
	for (i=0; i<cal_len; i++) {
		port_phyID = cal_main[i];
		AP_TOKEN_CHECK(port_phyID){
	  	switch (clk_freq) {
	  		case 933: 
          min_spacing=LLS_MIN_SPACING; break;
	  		case 793: 
                        case 794:
                        case 795:
                        case 796:
                        case 797:
          if (port_speeds[port_phyID]<SPEED_42G) {min_spacing=(LLS_MIN_SPACING-2);}
          else if (port_speeds[port_phyID]>=SPEED_100G) {min_spacing=(LLS_MIN_SPACING-6);}
          else if (port_speeds[port_phyID]==SPEED_50G) {min_spacing=(LLS_MIN_SPACING-4);}
          else {min_spacing=LLS_MIN_SPACING;}
        break;
	  		case 575: 
          min_spacing=(LLS_MIN_SPACING-4); break;
	  		case 435: 
          min_spacing=(LLS_MIN_SPACING-6); break;
	  		default: 
          min_spacing=LLS_MIN_SPACING; break;
	  	}
			for (j=1; j<min_spacing; j++) {
				k = ( (i+j)<cal_len) ? (i+j) : (i+j-cal_len) ;
				if (cal_main[k]==port_phyID) {
					result=FAIL; 
					TDM_ERROR4("LLS check failed in %s, port %0d between [%0d,%0d] violates LLS min spacing requirement\n",table, port_phyID,i,k);
					break;
				}
			}
		}
	}
   
	for (i=0; i<cal_len; i++){
 		AP_TOKEN_CHECK(cal_main[i]) {
				k = ( (i+6)<cal_len) ? (i+6) : (i+6-cal_len) ;
    	  if(cal_main[k] == cal_main[i]) {
						result = FAIL;
						TDM_ERROR4("%s LLS 1:6 same port spacing violation port %0d, (#%03d | #%03d)\n",table,cal_main[i],i,k);
				}
				k = ( (i+8)<cal_len) ? (i+8) : (i+8-cal_len) ;
    	  if(cal_main[k] == cal_main[i]) {
						result = FAIL;
						TDM_ERROR4("%s LLS 1:8 same port spacing violation port %0d, (#%03d | #%03d)\n",table,cal_main[i],i,k);
				}
      }
    }
  }
	return result;
}

/**
@name: tdm_ap_chk_ovs_space
@param:

Checks that no port is back to back in the OVS table
 */
int tdm_ap_chk_ovs_space( tdm_mod_t *_tdm)
{
	int i, result=PASS, cal0_len, cal1_len,
	    *ovs_cal0, *ovs_cal1;


	ovs_cal0 = _tdm->_chip_data.cal_0.cal_grp[0];
  ovs_cal1 = _tdm->_chip_data.cal_1.cal_grp[0];

  for(i =0; i<AP_OS_LLS_GRP_LEN; i++) {
    if(ovs_cal0[i] == AP_NUM_EXT_PORTS) break;
   }
  cal0_len = i;

  for(i =0; i<AP_OS_LLS_GRP_LEN; i++) {
    if(ovs_cal1[i] == AP_NUM_EXT_PORTS) break;
   }
  cal1_len = i;
  
   if (cal0_len >40 || cal1_len >40) { result=FAIL; TDM_ERROR0("OVS table overscheduled\n"); }

		if(ovs_cal0[0] == ovs_cal0[cal0_len-1]){
		 result = FAIL;
     TDM_ERROR1("SPACING VOILATION:Found back to back ports in OVS TDM TBL0:port %0d\n",ovs_cal0[i]);
	  }
   	if(ovs_cal1[0] == ovs_cal1[cal1_len-1]){
		 result = FAIL;
     TDM_ERROR1("SPACING VOILATION:Found back to back ports in OVS TDM TBL1:port %0d\n",ovs_cal1[i]);
	  }
	
	for (i=0; i<cal0_len; i++){
    AP_TOKEN_CHECK(ovs_cal0[i]) {
		 if(ovs_cal0[i] == ovs_cal0[i+1]){
		  result = FAIL;
      TDM_ERROR1("SPACING VOILATION:Found back to back ports in OVS TDM TBL0:port %0d\n",ovs_cal0[i]);
	   }
    }
  }
	for (i=0; i<cal1_len; i++){
    AP_TOKEN_CHECK(ovs_cal1[i]) {
   	 if(ovs_cal1[i] == ovs_cal1[i+1]){
		  result = FAIL;
      TDM_ERROR1("SPACING VOILATION:Found back to back ports in OVS TDM TBL1:port %0d\n",ovs_cal1[i]);
	   }
    }
	}
	
	return result;
}

/**
@name: tdm_ap_chk_pgw_min
@param:

Checks min spacing in  the PGW table
 */
int tdm_ap_chk_pgw_min( tdm_mod_t *_tdm)
{
	int i, result=PASS, pgw0_len, pgw1_len,
	    *pgw_cal0, *pgw_cal1;


	pgw_cal0 = _tdm->_chip_data.cal_0.cal_main;
  pgw_cal1 = _tdm->_chip_data.cal_1.cal_main;

  for(i =0; i<AP_LR_LLS_LEN; i++) {
    if(pgw_cal0[i] == AP_NUM_EXT_PORTS) break;
   }
  pgw0_len = i;

  for(i =0; i<AP_LR_LLS_LEN; i++) {
    if(pgw_cal1[i] == AP_NUM_EXT_PORTS) break;
   }
  pgw1_len = i;
  

	if(pgw_cal0[0] == pgw_cal0[pgw0_len-1]){
     AP_TOKEN_CHECK(pgw_cal0[i]) {
		 result = FAIL;
     TDM_ERROR1("1 SPACING VOILATION:Found back to back ports in PGW TDM TBL0:port %0d\n",pgw_cal0[i]);
	   }
   }
   if(pgw_cal1[0] == pgw_cal1[pgw1_len-1]){
     AP_TOKEN_CHECK(pgw_cal0[i]) {
		 result = FAIL;
     TDM_ERROR1("2 SPACING VOILATION:Found back to back ports in PGW TDM TBL1:port %0d\n",pgw_cal1[i]);
	   }
   }
	
	for (i=0; i<pgw0_len; i++){
    AP_TOKEN_CHECK(pgw_cal0[i]) {
		 if(pgw_cal0[i] == pgw_cal0[i+1]){
		  result = FAIL;
      TDM_ERROR1("3 SPACING VOILATION:Found back to back ports in PGW TDM TBL0:port %0d\n",pgw_cal0[i]);
	   }
    }
  }
	for (i=0; i<pgw1_len; i++){
    AP_TOKEN_CHECK(pgw_cal1[i]) {
   	 if(pgw_cal1[i] == pgw_cal1[i+1]){
		  result = FAIL;
      TDM_ERROR1("SPACING VOILATION:Found back to back ports in PGW TDM TBL1:port %0d\n",pgw_cal1[i]);
	   }
    }
	}
	
	return result;
}

/**
@name: tdm_ap_chk_chip_specific
@param:

Checks chip specific 
 */
void tdm_ap_chk_chip_specific(tdm_mod_t *_tdm, int fail[16])
{
	int i,j,k, clk, core_bw, accessories, port_spd, port_state, port_phyID, phy_idx_s, phy_idx_e, msub_x0, msub_x1, pgw_x0_len, pgw_x1_len, t1, t2,t3,t4,t5;
	int s1, s1xcnt=0, s1xavg=0, s2, s2xcnt=0, s2xavg=0, s3, s3xcnt=0, s3xavg=0, /*s5=4,*/ s5xcnt=0, s5xavg=0, s4=5, s4xcnt=0, s4xavg=0;
	int pgw_tbl_len, *pgw_tbl_ptr, **ovsb_tbl_ptr, wc_principle, **wc_checker, pgw_mtbl[AP_LR_LLS_LEN*2], pgw_tracker[AP_NUM_EXT_PORTS];
	const char *str_pgw_tbl_x0, *str_pgw_tbl_x1,*str_ovs_tbl_x0,*str_ovs_tbl_x1;
	char *str_pgw_ptr, *str_ovs_ptr;
	
	/* Initialize */
	str_pgw_tbl_x0 = "PGW table x0";
	str_pgw_tbl_x1 = "PGW table x1";
	str_ovs_tbl_x0 = "PGW OVSB table x0";
	str_ovs_tbl_x1 = "PGW OVSB table x1";
	
	pgw_x0_len=0; pgw_x1_len=0; 
	clk = _tdm->_chip_data.soc_pkg.clk_freq;
		
	for (i=0; i<AP_LR_LLS_LEN; i++) {
		if (_tdm->_chip_data.cal_0.cal_main[i]!=AP_NUM_EXT_PORTS) pgw_x0_len++;
		if (_tdm->_chip_data.cal_1.cal_main[i]!=AP_NUM_EXT_PORTS) pgw_x1_len++;
	}
	
	switch(clk) {
    case 840: core_bw=511; accessories=9; break; 
    case 793: 
    case 794: 
    case 795:
    case 796:
    case 797:
      core_bw=480; accessories=18; break; 
    case 575: core_bw=340; accessories=9; break; 
    case 510: core_bw=300; accessories=13; break; 
    case 435: core_bw=260; accessories=10; break; 
		default: 			core_bw=567; accessories=9 ; break; 
	}
	TDM_PRINT3("TDM: _____VERBOSE: (clk - %0d) (core bw - %0d) (mmu acc - %0d)\n", clk, core_bw, accessories);
	s4=(((495*((clk/76))))/1000); /*100G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 100G MMU spacing as %0d\n", s4);
	s3=(((12*((clk/76))))/10); /*40G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 40G MMU spacing as %0d\n", s3);
	s2=(((24*((clk/76))))/10); /*20G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 20G MMU spacing as %0d\n", s2);
	s1=(((495*((clk/76))))/100); /*10G*/
	TDM_PRINT1("TDM: _____VERBOSE: calculated 10G MMU spacing as %0d\n", s1);	
	
	/* [7] Check length (overflow and symmetry) */
	if (fail[7]==2) {
		msub_x0=0; msub_x1=0; 
		
		for (i=1; i<(AP_NUM_EXT_PORTS-57); i++) {
			if((_tdm->_chip_data.soc_pkg.state[i-1]== PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i-1]== PORT_STATE__LINERATE_HG) && 
			   _tdm->_chip_data.soc_pkg.state[i]  != PORT_STATE__COMBINE  &&
			   _tdm->_chip_data.soc_pkg.speed[i]  <  SPEED_10G){
				if     (i>=1  && i<=36 ){msub_x0++;}
				else if(i>=37 && i<=72 ){msub_x1++;}
			}
		}

		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table x0 is %0d\n", pgw_x0_len);
		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table x1 is %0d\n", pgw_x1_len);
		
		if (msub_x0==0 && msub_x1==0) { 
			switch (core_bw) {
				case 567:
					if (pgw_x0_len>56 || pgw_x1_len>56)  {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len) {TDM_PRINT0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 511: case 480:
					if (pgw_x0_len>48 || pgw_x1_len>48 ) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len ) {TDM_PRINT0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 340:
					if (pgw_x0_len>34 || pgw_x1_len>34 ) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len ) {TDM_PRINT0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 260:
					if (pgw_x0_len>26 || pgw_x1_len>24 ) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					break;
				default: break;
			}
		}
		
		fail[7]= (fail[7]==2) ? (PASS) : (FAIL);
	}
	
	/* [8] [9] Check pgw table TSC transcription and TSC proximity */
	if (fail[8]==2 && fail[9]==2) {
		/* Length and loopback passes are implicitly combined */
		wc_checker = _tdm->_chip_data.soc_pkg.pmap;
		for (k=0; k<2; k++){
			/* select pgw table */
			switch(k){
				/* pgw_tdm_tbl_x0 */
				case 0:  pgw_tbl_ptr = _tdm->_chip_data.cal_0.cal_main; 
						 pgw_tbl_len = pgw_x0_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_x0;
						 break;
				/* pgw_tdm_tbl_x1 */
				default: pgw_tbl_ptr = _tdm->_chip_data.cal_1.cal_main; 
						 pgw_tbl_len = pgw_x1_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_x1;
						 break;
			}
			
			/* check transcription and TSC violations */
			for (i=0; i<pgw_tbl_len; i++) {
				port_phyID = pgw_tbl_ptr[i];
				AP_TOKEN_CHECK(port_phyID){
					if(port_phyID>0 && port_phyID<(AP_NUM_EXT_PORTS-57)){
						wc_principle = -1;
						for (j=0; j<AP_NUM_PHY_PM; j++) {
							if (wc_checker[j][0] == port_phyID || wc_checker[j][1] == port_phyID || wc_checker[j][2] == port_phyID || wc_checker[j][3] == port_phyID){
								wc_principle = j;
								break;
							}
            }
						if (wc_principle == -1) {
							fail[8] = 1; 
							TDM_ERROR2("TSC transcription corruption %s port %0d\n", str_pgw_ptr, port_phyID);
						}
						else if ((pgw_tbl_len>8) && (i<pgw_tbl_len-1) && /*proximity check to be done for pgw calendar between index 0 & 47 */ 
								 (pgw_tbl_ptr[i+1]==wc_checker[wc_principle][0]  || 
								  pgw_tbl_ptr[i+1]==wc_checker[wc_principle][1]  || 
								  pgw_tbl_ptr[i+1]==wc_checker[wc_principle][2]  || 
								  pgw_tbl_ptr[i+1]==wc_checker[wc_principle][3]) ){
								fail[9]=2; /* proximity check disabled for Apache as sister port spacing is 0 for Apache. */
								TDM_ERROR4("TSC proximity violation in %s between index %0d & +1 pgw_tbl_ptr %0d %0d\n", str_pgw_ptr, i,  pgw_tbl_ptr[i], pgw_tbl_ptr[i+1]);
                                TDM_PRINT4("\nwc_checker 0=%0d 1=%0d 2=%0d 3=%0d",wc_checker[wc_principle][0] , wc_checker[wc_principle][1], wc_checker[wc_principle][2], wc_checker[wc_principle][3]    );
						}
					}
				}
			}
		}
		fail[8]= (fail[8]==2) ? (PASS) : (FAIL);
		fail[9]= (fail[9]==2) ? (PASS) : (FAIL);
	}
	
	/* [10] Check pgw table spacing*/
	if (fail[10]==2) {
		for (k=0; k<2; k++){
			/* select pgw table */
			switch(k){
				/* pgw_tdm_tbl_x0 */
				case 0:  pgw_tbl_ptr = _tdm->_chip_data.cal_0.cal_main; 
						 pgw_tbl_len = pgw_x0_len; 
						 phy_idx_s   = 1;
						 phy_idx_e   = 36;
						 str_pgw_ptr = (char *) str_pgw_tbl_x0;
						 break;
				/* pgw_tdm_tbl_x1 */
				default: pgw_tbl_ptr = _tdm->_chip_data.cal_1.cal_main; 
						 pgw_tbl_len = pgw_x1_len; 
						 phy_idx_s   = 37;
						 phy_idx_e   = 72;
						 str_pgw_ptr = (char *) str_pgw_tbl_x1;
						 break;
			}
			t1 = (pgw_tbl_len/2);/*10G*/
			t2 = (pgw_tbl_len/4);/*20G*/
			t3 = (pgw_tbl_len/8);/*40G*/
			t4 = (pgw_tbl_len/20);/*100G*/
			t5 = (pgw_tbl_len/10);/*50G*/
			TDM_PRINT3("TDM: _____VERBOSE: %s calculated 10G PGW spacing %0d pgw_tbl_len %0d\n",str_pgw_ptr, t1, pgw_tbl_len);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 20G PGW spacing %0d\n",str_pgw_ptr, t2);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 40G PGW spacing %0d\n",str_pgw_ptr, t3);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 100G PGW spacing %0d\n",str_pgw_ptr, t4);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 50G PGW spacing %0d\n",str_pgw_ptr, t5);
			
			/* set tracker */
			for (i=0; i<(AP_NUM_EXT_PORTS-56); i++) {pgw_tracker[i]=0;}
			for (i=0; i<pgw_tbl_len; i++) { pgw_mtbl[i]=pgw_tbl_ptr[i]; pgw_mtbl[i+pgw_tbl_len]=pgw_tbl_ptr[i];}
			for (i=0; i<pgw_tbl_len; i++) {
				port_phyID = pgw_tbl_ptr[i];
				AP_TOKEN_CHECK(port_phyID){
					if(port_phyID>0 && port_phyID<(AP_NUM_EXT_PORTS-57)){
						pgw_tracker[port_phyID] = 1;
						for (j=1; j<=pgw_tbl_len; j++) {
							if (pgw_mtbl[i+j]!=pgw_mtbl[i]) {pgw_tracker[port_phyID]++; }
							else {break;}
						}
					}
				}
			}
			
			/* check pgw spacing */
			for (i=phy_idx_s; i<phy_idx_e; i++){
				if(pgw_tracker[i]!=0){
					switch (_tdm->_chip_data.soc_pkg.speed[i]) {
						case SPEED_10G:
							if (pgw_tracker[i] != t1) {
								/*fail[10]=1;*/ 
								TDM_PRINT3(" %s, 10G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);
							}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							if (pgw_tracker[i] != t2) {
								/*fail[10]=1;*/ 
								TDM_PRINT3(" %s, 20G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);
							}
							break;
						case SPEED_42G:
						case SPEED_40G:
							if (pgw_tracker[i] != t3) {
								/*fail[10]=1;*/ 
								TDM_PRINT3(" %s, 40G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);
							}
							break;
						case SPEED_50G:
							if (pgw_tracker[i] != t5) {
								/*fail[10]=1;*/ 
								TDM_PRINT3(" %s, 50G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);
							}
							break;
						case SPEED_120G:
						case SPEED_100G:
							if (pgw_tracker[i] != t4) {
								/*fail[10]=1;*/ 
								/*TDM_WARN3(" %s, 100G/120G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);*/
							}
							break;
						default:
							/* 10G/1G spacing is implicitly checked by both the TSC spacing and the spacing of all other speeds */ 
							break;
					}
				}
			}
		}
		fail[10]= (fail[10]==2) ? (PASS) : (FAIL);
	}
	
	/* [11] */
	if (fail[11]==2) {
		
		s1xavg = (s1xcnt==0) ? 0 : ((s1xavg*100)/s1xcnt);
		s2xavg = (s2xcnt==0) ? 0 : ((s2xavg*100)/s2xcnt);
		s3xavg = (s3xcnt==0) ? 0 : ((s3xavg*100)/s3xcnt);
		s4xavg = (s4xcnt==0) ? 0 : ((s4xavg*100)/s4xcnt);
		s5xavg = (s5xcnt==0) ? 0 : ((s5xavg*100)/s5xcnt);

		if (s1xavg>0) if ( (s1xavg > (s1*101) || s1xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_PRINT1("10G line rate not within 1 percent tolerance in x pipe, (s1xavg = %0d)\n", s1xavg);}
		if (s2xavg>0) if ( (s2xavg > (s1*101) || s2xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_PRINT1("20G line rate not within 1 percent tolerance in x pipe, (s2xavg = %0d)\n", s2xavg);}
		if (s3xavg>0) if ( (s3xavg > (s1*101) || s3xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_PRINT1("40G line rate not within 1 percent tolerance in x pipe, (s3xavg = %0d)\n", s3xavg);}
		if (s4xavg>0) if ( (s4xavg > (s1*101) || s4xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_PRINT1("100G line rate not within 1 percent tolerance in x pipe, (s4xavg = %0d)\n", s4xavg);}
		if (s5xavg>0) if ( (s5xavg > (s1*101) || s5xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_PRINT1("120G line rate not within 1 percent tolerance in x pipe, (s5xavg = %0d)\n", s5xavg);}
		fail[11]= (fail[11]==2) ? (PASS) : (FAIL);
	}
	
	/* [12] Check 100G/120G port numbering */
	if (fail[12]==2) {
		for (i=1; i<(AP_NUM_EXT_PORTS-57); i++) {
			if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G || _tdm->_chip_data.soc_pkg.speed[i]==SPEED_120G) {
				if (i!=17 && i!=29 && i!=33 && i!=53 && i!=65 && i!=69) {
					fail[12]=1; 
					TDM_ERROR1("port number %0d is a 100G/120G port that is improperly subscribed\n",i);
				}
			}
		}
		fail[12]= (fail[12]==2) ? (PASS) : (FAIL);
	}
	
	/* [13] Check port subscription */
	if (fail[13]==2) {
		for(i=1; i<(AP_NUM_EXT_PORTS-57); i++){
			/* pgw_x_0 */
			if(i<=36) {
				pgw_tbl_ptr  = _tdm->_chip_data.cal_0.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_0.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_x0;
				str_ovs_ptr  = (char *) str_ovs_tbl_x0;
			}
			/* pgw_x_1 */
			else {
				pgw_tbl_ptr  = _tdm->_chip_data.cal_1.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_1.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_x1;
				str_ovs_ptr  = (char *) str_ovs_tbl_x1;
			}
			
				k=0;
				port_spd  = _tdm->_chip_data.soc_pkg.speed[i];
				port_state= _tdm->_chip_data.soc_pkg.state[i-1];
				/* line-rate ports */
				if (port_state==PORT_STATE__LINERATE   || 
				    port_state==PORT_STATE__LINERATE_HG){
					switch (port_spd) {
						case SPEED_1G:
						case SPEED_2p5G:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i, str_pgw_ptr, k);}
							break;
						case SPEED_10G:
						case SPEED_10G_XAUI:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=2 && k!=8 && k!=20) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=4) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_25G:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=5) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_42G_HG2:
						case SPEED_42G:
						case SPEED_40G:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=8) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_50G:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_120G:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=24) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_100G:
							for (j=0; j<64; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=24 && k!=20) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						default:
							break;
					}
				}
				/* oversub ports */
				else if (port_state==PORT_STATE__OVERSUB   ||
				         port_state==PORT_STATE__OVERSUB_HG){
					switch (port_spd) {
						case SPEED_1G:
						case SPEED_2p5G:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_10G:
						case SPEED_10G_XAUI:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_25G:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=3 && k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
              break;
						case SPEED_42G_HG2:
						case SPEED_42G:
						case SPEED_40G:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=4 && k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_50G:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=5 && k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
              break;
						case SPEED_100G:
							for (j=0; j<40; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=10 && k!=8 && k!=4 && k!=12) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						default:
							break;
					}
				}
		}
		fail[13]= (fail[13]==2) ? (PASS) : (FAIL);
	}
}


/**
@name: tdm_ap_chk
@param: 
 */
int
tdm_ap_chk( tdm_mod_t *_tdm)
{	
	int i, result, mmu_x_len, fail[16];

	for (i=0; i<16; i++) {
		fail[i] = 2;
	}

	mmu_x_len= _tdm->_chip_data.cal_2.cal_len;
	for (i=(_tdm->_chip_data.cal_2.cal_len-1); i>0; i--){
		if(_tdm->_chip_data.cal_2.cal_main[i]==AP_NUM_EXT_PORTS){
			mmu_x_len--;
		}
		else{
			break;
		}
	}

	_tdm->_chip_data.cal_2.cal_len = mmu_x_len;
	
	/**********************************/
	TDM_BIG_BAR
	TDM_PRINT0("TDM: TDM Self Check\n\n");
	
	/* [0] Check MMU table length (overflow and symmetry) */
	if (fail[0]==2) {
		TDM_PRINT0("TDM: [0] Checking length (overflow and symmetry)\n"); TDM_SML_BAR
		/* Pipe X */
		_tdm->_core_data.vars_pkg.pipe = AP_PIPE_X_ID;
		result  = tdm_ap_chk_struc(_tdm);
		fail[0] = (fail[0]==FAIL || fail[0]==UNDEF) ? (fail[0]):(result);
	}
	
	/* [1] Check TSC transcription in MMU tables */
	if (fail[1]==2) {
		TDM_PRINT0("\nTDM: [1] Checking TSC transcription\n"); TDM_SML_BAR
		fail[1] = tdm_ap_chk_tsc(_tdm);
	}
	
	/* [2] Check sister port spacing in MMU tables */
	if (fail[2]==2) {
		TDM_PRINT0("\nTDM: [2] Checking min spacing\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = AP_XPIPE_CAL_ID;
		result  = tdm_ap_chk_min(_tdm);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
	}
	
	/* [3] Check (CPU/loopback/regular) port subscription and accessory slot allocation. */
	if (fail[3]==2) {
		TDM_PRINT0("\nTDM: [3] Checking per port subscription stats\n"); TDM_SML_BAR
		fail[3] = tdm_ap_chk_subscription(_tdm);
	}
	
	/* [4] Check Oversub jitter */
	if (fail[4]==2) {
		TDM_PRINT0("\nTDM: [4] Checking Oversub jitter\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = AP_XPIPE_CAL_ID;
		result  = tdm_ap_chk_os_jitter(_tdm);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
        TDM_PRINT2("result = %d fail[4] = %d\n", result, fail[4]);
	}
	
	/* [5] [6] Check Linerate jitter */
	if (fail[5]==2 || fail[6]==2) {
		TDM_PRINT0("\nTDM: [5] [6] Checking line rate jitter\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = AP_XPIPE_CAL_ID;
		tdm_ap_chk_lr_jitter(_tdm, fail);
	}
	
	/* [14] Check LLS min spacing requiement */
/*#ifdef _LLS_SCHEDULER*/
	if (fail[14]==2 && _tdm->_chip_data.soc_pkg.clk_freq>=435) {
		TDM_PRINT0("\nTDM: [14] Checking LLS min spacing requirement\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = AP_XPIPE_CAL_ID;
		result = tdm_ap_chk_lls(_tdm);
		fail[14] = (fail[14]==FAIL || fail[14]==UNDEF) ? (fail[14]):(result);
	}
/*#endif*/

	/* [15] Check back to back ports in PGW/OVS TDM table */
	if (fail[15]==2) {
		TDM_PRINT0("\nTDM: [15] Checking min spacing in PGW/OVS table\n"); TDM_SML_BAR
		fail[15] = tdm_ap_chk_pgw_min(_tdm) || tdm_ap_chk_ovs_space(_tdm);	
	}

	/* [7] [8] [9] [10] [11] [12] [13] Check chip specific constraints */
	if (fail[7]==2 || fail[8]==2 || fail[9]==2 || fail[10]==2 || fail[11]==2 || fail[12]==2 || fail[13]==2){
		TDM_PRINT0("\nTDM: --- Chip specific checkers [7] [8] [9] [10] [11] [12] [13]\n"); TDM_SML_BAR
		tdm_ap_chk_chip_specific( _tdm, fail );
	}
	
	/* Summarize check results */
	TDM_PRINT0("\n");
	TDM_SML_BAR
	if (fail[15]==FAIL ||
		fail[14]==FAIL ||
		fail[13]==FAIL ||
		fail[12]==FAIL ||
		fail[11]==FAIL ||
		/*fail[10]==FAIL ||*/
		fail[9]==FAIL ||
		fail[8]==FAIL ||
		fail[7]==FAIL ||
		fail[6]==FAIL ||
		/*fail[5]==FAIL ||*/
		fail[4]==FAIL ||
		fail[3]==FAIL ||
		fail[2]==FAIL ||
		fail[1]==FAIL ||
		fail[0]==FAIL ){
		TDM_ERROR0("Apache TDM Self Check: *** FAILED ***\n");
		TDM_SML_BAR
		TDM_ERROR0("TDM: Fail vector: [");
		for (i=0; i<16; i++) {
			TDM_ERROR1(" %0d ",fail[i]);
		}
		TDM_ERROR0("]\n");
        return FAIL;
	}
	else {
		TDM_PRINT0("Apache TDM Self Check: *** PASSED ***\n");
	}
	TDM_SML_BAR
	TDM_PRINT0("\n");
	TDM_PRINT0("TDM: TDM Self Check Complete.\n");
	TDM_BIG_BAR

	return PASS;
}
