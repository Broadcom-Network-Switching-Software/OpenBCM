/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip specific checker schedules
 */
#ifdef _TDM_STANDALONE
        #include <tdm_top.h>
#else
        #include <soc/tdm/core/tdm_top.h>
#endif



extern int chk_tdm_roundup(int m, int n);
extern int chk_tdm_rounddown(int m, int n);
extern int chk_tdm_round(int m, int n);

/**
@name: chk_tdm_mn_ethernet
@param: int, enum[], int[][], int[]

Returns BOOL_TRUE or BOOL_FALSE depending on if pipe of the given port has traffic entirely Ethernet.
 */
int chk_tdm_mn_ethernet(tdm_mod_t *_tdm)
{
	int result, chk[2];
	chk[0] = BOOL_FALSE; chk[1] = BOOL_FALSE;
	
	/* PIPE_X */
	if(_tdm->_core_data.vars_pkg.pipe == MN_PIPE_X_ID){
		_tdm->_core_data.vars_pkg.port = 1;
		chk[0] = tdm_mn_check_ethernet(_tdm);
		_tdm->_core_data.vars_pkg.port = 33;
		chk[1] = tdm_mn_check_ethernet(_tdm);
	}
	result = (chk[0] && chk[1]) ? (BOOL_TRUE) : (BOOL_FALSE);
	return result;
	
}


/**
@name: chk_tdm_mn_struc
@param:

Checks length, overflow, and symmetry (determinism between IDB and MMU tables)
 */
int chk_tdm_mn_struc(tdm_mod_t *_tdm)
{
	int result, max_len, tbl_len=0, freq;
	
	result = PASS;
	freq   = _tdm->_chip_data.soc_pkg.clk_freq;
		
        if(_tdm->_chip_data.soc_pkg.soc_vars.mn.customer10G_tdm == 1){
          freq=815;
        } else if (_tdm->_chip_data.soc_pkg.soc_vars.mn.customer25G_tdm == 1) {
          freq=816;
        }
	/* Ethernet */
	if ( chk_tdm_mn_ethernet(_tdm) ) {
		switch (freq) {
      /*AP*/
			case 933: max_len=233; break;
			case 840: max_len=212; break;
			case 815: max_len=321; break;
			case 816: max_len=297; break;
			case 550: max_len=217; break;
			case 817: max_len=215; break;
			case 818: max_len=334; break;
			case 819: max_len=283; break;
			case 820: max_len=335; break;
			case 861: case 862: case 700: max_len=227; break;
			case 705: max_len=200; break;
			case 793: case 794: max_len=402; break;
			case 575: max_len=145; break;
			case 510: max_len=129; break;
			case 435: max_len=110; break;
			case 525: max_len=138; break;
			case 666: max_len=140; break;
			case 667: max_len=152; break;
            case 450: max_len=294; break;

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
      case 794: max_len=402; break;
			case 575: max_len=145; break;
			case 510: max_len=129; break;
			case 435: max_len=110; break;
			case 815: max_len=321; break;
			case 550: max_len=217; break;
            case 450: max_len=294; break;
			default:
				TDM_ERROR0("TDM structure check received unrecognized frequency\n");
				return UNDEF;
				break;
		}	
	}
	
	/* Actual calender length */
	switch (_tdm->_core_data.vars_pkg.pipe){
		case MN_PIPE_X_ID: tbl_len= _tdm->_chip_data.cal_2.cal_len; break;
		default:
			TDM_ERROR0("TDM structure check received unrecognized pipe ID\n");
			return UNDEF;
			break;
	}
	
	if (tbl_len>max_len){
                if(_tdm->_chip_data.soc_pkg.soc_vars.mn.customer10G_tdm == 1 || _tdm->_chip_data.soc_pkg.soc_vars.mn.customer25G_tdm == 1){
		  result=FAIL; 
		  TDM_ERROR3("TDM pipe %0d TDM length %0d slots, spec %0d slots\n",_tdm->_core_data.vars_pkg.pipe,tbl_len,max_len);
                }
	}
	return result;
}


/**
@name: chk_tdm_mn_tsc
@param:

Verifies TSC transcription of all ports indexed in bcm configuration
 */
int chk_tdm_mn_tsc(tdm_mod_t *_tdm)
{
	int i, j, result=PASS, result_i, num_lanes; 
	int **port_pmap, num_pm_lanes, num_ext_ports;
	enum port_speed_e *port_speeds;
	enum port_state_e *port_states;
	
	port_states  = _tdm->_chip_data.soc_pkg.state;
	port_speeds  = _tdm->_chip_data.soc_pkg.speed;
	port_pmap    = _tdm->_chip_data.soc_pkg.pmap;
	num_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
	num_ext_ports= _tdm->_chip_data.soc_pkg.num_ext_ports;
	/*num_phy_pm   = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;*/
	
	for (i=1; i<(num_ext_ports-MN_EXT_PLUS_CPU_PORT); i++){
		/* check if port is active */
		if (  port_states[i-1]==PORT_STATE__LINERATE    || 
			  port_states[i-1]==PORT_STATE__OVERSUB     ||
			  port_states[i-1]==PORT_STATE__LINERATE_HG ||
			  port_states[i-1]==PORT_STATE__OVERSUB_HG  ){
			result_i = PASS;
			/* Check tsc numbering */
			_tdm->_core_data.vars_pkg.port = i;
			  if(tdm_mn_which_tsc(_tdm)==num_ext_ports){
          if(_tdm->_chip_data.soc_pkg.state[i] == PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i] == PORT_STATE__LINERATE_HG) {
			      TDM_ERROR1("TDM TSC check failed to transcribe port %0d\n",i);
			      result_i = FAIL;
          }
			  }
			  /* Check legal lane configs to see if port speed can fit (AP) */
			  else{
			  	num_lanes=0;
			        /*if ( (i ==17 || i==53) && port_speeds[i]>=SPEED_100G){
			          	for (k=0; k<3; k++){
			          		for(j=0; j<num_pm_lanes; j++){
			          			if(port_pmap[(i-1+k)/num_pm_lanes][j]==i){++num_lanes;}
			          		}
			          	}
                                }
			  	else{ */
			  	for(j=0; j<num_pm_lanes; j++){
			  		if(port_pmap[(i-1)/num_pm_lanes][j]==i){++num_lanes;}
			  	}
			  	/*} */
			  	
			  	switch (port_speeds[i]) {
			  	  case SPEED_1G: 
			  	  case SPEED_2p5G:
			  	  case SPEED_5G:
			  	  case SPEED_10G: 
			  	  case SPEED_12p5G: 
			  	  case SPEED_10G_DUAL:
			  	  case SPEED_10G_XAUI:
			  	  case SPEED_25G:
			  	    if (num_lanes!=1) {result_i = FAIL;} 
			  	      break;
			  	  case SPEED_20G:
                                    if(  mn_is_clport(i) ) {
			  			if (num_lanes!=1) {result_i = FAIL;} 
			  			  break;
                                                } else { if (num_lanes!=2) {result_i = FAIL;}
                                                  break;
                                                }
			  		case SPEED_50G:
			  			if (num_lanes!=2) {result_i = FAIL;} 
			  			break;
			  		case SPEED_40G: case SPEED_42G:
                      if (num_lanes == 3) {
                        result_i = FAIL; 
                      }
                    break;
			  		case SPEED_100G:
                                          if (num_lanes!=4) {result_i = FAIL;}
                                            break;
			  		case SPEED_120G:
			  			/*if (num_lanes<12) {result_i = FAIL;} */
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
@name: chk_tdm_mn_sub_lr
@param:

Checks that all line rate nodes were extracted from solution matrix
 */
int chk_tdm_mn_sub_lr( tdm_mod_t *_tdm)
{
	int i, j, idx_0, idx_1, result=PASS, result_i, port_spd_M, port_spd_G, found_nodes,
	    *cal_main, cal_len, pipe_id, clk_freq;
	const char *table;
	enum port_speed_e *port_speeds;
	enum port_state_e *port_states;
	int quanta ;
	
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
	port_states= _tdm->_chip_data.soc_pkg.state;
	
	if(_tdm->_core_data.vars_pkg.pipe_info.slot_unit == TDM_SLOT_UNIT_5G)
	quanta = 50;
	else
	quanta = 25;
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
				idx_1    = MN_NUM_PHY_PORTS;
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
				case 5:
				case 10:
				case 12:
				case 20: case 21:
				case 25:
				case 40: case 42:
				case 50:
				case 100: 
				case 120:
					break;
				default:
          if(_tdm->_chip_data.soc_pkg.state[i] == PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i] == PORT_STATE__LINERATE_HG) {
				    result_i = UNDEF;
				    TDM_ERROR3("Linerate subscription check failed in %s, unrecognized speed %0d (Gbps) for port %0d\n",table,port_spd_G,i);
          }
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
          if(_tdm->_chip_data.soc_pkg.state[i] == PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i] == PORT_STATE__LINERATE_HG) {
			      TDM_ERROR3("Linerate subscription check failed in %s, port %0d is enabled but not scheduled in calender, %0d slots\n", table, i,found_nodes);
			      result_i = FAIL;
			    }
			}
			else if ( found_nodes < ((port_spd_M*10)/1000/(quanta)) ) {
				TDM_ERROR5("Linerate subscription failed in %s, %0dG vector for port %0d expected %0d nodes (found %0d)\n",table,port_spd_G,i,((port_spd_M*10)/1000/(quanta)),found_nodes);
				result_i = FAIL;
			}
			
			result = (result==FAIL) ? (result): (result_i);
		}	
	}
	
	return result;
}


/**
@name: chk_tdm_mn_sub_os
@param:

Checks that the oversub node is sorted into oversub speed groups
 */
int chk_tdm_mn_sub_os(tdm_mod_t *_tdm)
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
				idx_1       = MN_NUM_PHY_PORTS;
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
@name: chk_tdm_mn_sub_mgmt
@param:

Checks that CPU bandwidth is satisfied
 */
int chk_tdm_mn_sub_cpu( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, cpu_slots_required,  
	    *cal_main, cal_len, clk_freq; 
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_main   = _tdm->_chip_data.cal_2.cal_main; 
	cal_len    = _tdm->_chip_data.cal_2.cal_len; 
	
	/* determine number of CPU slots  */
	cpu_slots_required = 2;
	if (clk_freq==793 || clk_freq==794) {
		cpu_slots_required=4;
	} else if(
              clk_freq == 861 || clk_freq == 862 || 
              ((_tdm->_chip_data.soc_pkg.soc_vars.mn.os_config == 1) &&
              (clk_freq == 817 || clk_freq == 667 || clk_freq == 450 ||
               clk_freq == 550))
             ){
               if(_tdm->_chip_data.soc_pkg.soc_vars.mn.os_config && clk_freq == 862)
                 cpu_slots_required = 2;
               else
                 cpu_slots_required = 1;
  }
	/* calculate number of actual CPU slots */
	k=0;
	for (i=0; i<cal_len; i++){
		if(cal_main[i] == MN_CPU_PORT){
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
@name: chk_tdm_mn_sub_lpbk
@param:

Checks that LOOPBACK bandwidth is satisfied
 */
int chk_tdm_mn_sub_lpbk( tdm_mod_t *_tdm)
{
	int i, result=PASS, lb_slots_required=0,cpu_slots_required=0,anc_slots_required=0,
      sbus_slots_required=0, macsec_slots_required=0, *cal_main, cal_len, clk_freq;
  int actual_lb_slots=0, actual_cpu_slots=0, actual_anc_slots=0, actual_sbus_slots=0,
      actual_macsec_slots=0;
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	cal_main   = _tdm->_chip_data.cal_2.cal_main; 
	cal_len    = _tdm->_chip_data.cal_2.cal_len; 
	
	/* determine number of loopback slots  */
	lb_slots_required = 1;
	if (clk_freq==861 || clk_freq==862) {
      if(_tdm->_chip_data.soc_pkg.soc_vars.mn.os_config)
	    lb_slots_required = 2;
      else
	    lb_slots_required = 1;
	} else if (clk_freq==819) {
			lb_slots_required=5;
	} else{
		lb_slots_required=6;
	} 
  switch(clk_freq){
    case 815: case 816: case 550: case 821:
         switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
           case 0:
             lb_slots_required=6;
             cpu_slots_required=3;
             anc_slots_required=3;
             sbus_slots_required=2;
             macsec_slots_required=15;
             break; 
           case 580:case 460: case 400:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=2;
             sbus_slots_required=1;
             if(clk_freq == 550 && _tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw == 400)
               macsec_slots_required=2;
             else
               macsec_slots_required=8;
           break; 
           case 280:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=1;
             sbus_slots_required=1;
             macsec_slots_required=8;
           break; 
           /*case 400:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=1;
             sbus_slots_required=1;
             macsec_slots_required=2;
           break; */
           case 300: case 340:
             lb_slots_required=1;
             cpu_slots_required=1;
             anc_slots_required=1;
             sbus_slots_required=1;
             if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
               macsec_slots_required=8;
             } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
               macsec_slots_required=0;
             }
           break; 
         }
         break;
    case 450:
         switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
           case 460:case 360: case 400:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=2;
             sbus_slots_required=1;
             if(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw == 400)
               macsec_slots_required=1;
             else
               macsec_slots_required=8;
           break; 
           case 200:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=1;
             sbus_slots_required=1;
             macsec_slots_required=8;
           break; 
           case 235: case 275:
             lb_slots_required=1;
             cpu_slots_required=1;
             anc_slots_required=1;
             sbus_slots_required=1;
             if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
               macsec_slots_required=8;
             } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
               macsec_slots_required=0;
             }
           break; 
         }
         break;
    case 817:
         switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
           case 0:
             lb_slots_required=6;
             cpu_slots_required=3;
             anc_slots_required=3;
             sbus_slots_required=1;
             macsec_slots_required=0;
             break; 
           case 800:case 720:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=2;
             sbus_slots_required=1;
             macsec_slots_required=8;
           break; 
           case 460:
             lb_slots_required=2;
             cpu_slots_required=2;
             anc_slots_required=1;
             sbus_slots_required=1;
             macsec_slots_required=8;
           break; 
           case 475: case 440: case 363: case 297: case 515:
             lb_slots_required=1;
             cpu_slots_required=1;
             anc_slots_required=1;
             sbus_slots_required=1;
             if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
               macsec_slots_required=8;
             } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
               macsec_slots_required=0;
             }
           break; 
         }
         break;
    case 818:
         lb_slots_required=6;
         cpu_slots_required=3;
         anc_slots_required=3;
         sbus_slots_required=2;
         macsec_slots_required=0;
         break;
    case 819:
         lb_slots_required=5;
         cpu_slots_required=3;
         anc_slots_required=3;
         sbus_slots_required=1;
         macsec_slots_required=15;
         break;
    case 820:
         lb_slots_required=6;
         cpu_slots_required=3;
         anc_slots_required=4;
         sbus_slots_required=1;
         macsec_slots_required=17;
         break;
	 case 666:
          lb_slots_required=3;
          cpu_slots_required=2;
          anc_slots_required=2;
         sbus_slots_required=1;
          macsec_slots_required=8;
      break;
      case 667:
        switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw) {
          case 0:
            lb_slots_required=3;
            cpu_slots_required=2;
            anc_slots_required=2;
            sbus_slots_required=1;
            macsec_slots_required=8;
          break; 
          case 720:case 580:
            lb_slots_required=2;
            cpu_slots_required=2;
            anc_slots_required=2;
            sbus_slots_required=1;
            macsec_slots_required=8;
          break; 
          case 360: case 400:
            lb_slots_required=2;
            cpu_slots_required=2;
            anc_slots_required=1;
            sbus_slots_required=1;
            macsec_slots_required=8;
          break; 
          case 380: case 420:
	        _tdm->_core_data.vars_pkg.pipe_info.slot_unit = TDM_SLOT_UNIT_5G; 
            lb_slots_required=1;
            cpu_slots_required=1;
            anc_slots_required=1;
            sbus_slots_required=1;
            if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1) {
              macsec_slots_required=8;
            } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 0) {
              macsec_slots_required=0;
            }
          break;
        }
        
      break;

    case 861: case 862:
      if(_tdm->_chip_data.soc_pkg.soc_vars.mn.os_config == 1) {
         lb_slots_required=2;
         cpu_slots_required=2;
         anc_slots_required=3;
         sbus_slots_required=1;
         if(_tdm->_chip_data.soc_pkg.soc_vars.mn.is_macsec == 1)
           macsec_slots_required = 16;
         else
           macsec_slots_required= 0;
      } else if(_tdm->_chip_data.soc_pkg.soc_vars.mn.os_config ==0) {
         lb_slots_required=1;
         cpu_slots_required=1;
         anc_slots_required=2;
         sbus_slots_required=1;
         macsec_slots_required=0;
      }
         break;
    default: break;
  }
	/* calculate number of actual loopback slots */
	for (i=0; i<cal_len; i++) {
		if (cal_main[i]==MN_LOOPBACK_PORT) {
			actual_lb_slots++;
		}
		if (cal_main[i]==MN_CPU_PORT) {
			actual_cpu_slots++;
		}
		if (cal_main[i]==MN_AUX_TOKEN) {
			actual_anc_slots++;
		}
		if (cal_main[i]==MN_SBUS_TOKEN) {
			actual_sbus_slots++;
		}
		if (cal_main[i]==MN_MACSEC_PORT) {
			actual_macsec_slots++;
		}
	}
	if (actual_lb_slots != lb_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe , loopback port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",actual_lb_slots,lb_slots_required);
	}
	if (actual_cpu_slots != cpu_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe , CPU bandwidth is insufficient in this configuration - %0d slots of %0d required\n",actual_cpu_slots,cpu_slots_required);
	}
	if (actual_anc_slots != anc_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe1 , ANC port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",actual_anc_slots,anc_slots_required);
	}
	if (actual_sbus_slots != sbus_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe , SBUS port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",actual_sbus_slots,sbus_slots_required);
	}
	if (actual_macsec_slots != macsec_slots_required) {
		result = FAIL;
		TDM_ERROR2("Subscription check failed in MMU pipe , MACSEC port bandwidth is insufficient in this configuration - %0d slots of %0d required\n",actual_macsec_slots,macsec_slots_required);
	}
	
	return result;
}


/**
@name: chk_tdm_mn_sub_lpbk
@param:

Checks that AGNOSTIC ACCESSORY bandwidth is satisfied
 */
int chk_tdm_mn_sub_acc( tdm_mod_t *_tdm)
{
	int i, k, result=PASS, ac_slots_required,
	    *x_cal_main, x_cal_len, clk_freq;
	
	clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	x_cal_main = _tdm->_chip_data.cal_2.cal_main; 
	x_cal_len  = _tdm->_chip_data.cal_2.cal_len;

	/* check pipe X */
		ac_slots_required=9;

    if (clk_freq==793 || clk_freq==794 ){
		ac_slots_required=18;
	  }
		else if (clk_freq==666 || clk_freq==667 ){
          switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw){
            case 0:
		      ac_slots_required=8;
            break;
            case 720: case 580:
		      ac_slots_required=7;
            break;
            case 360: case 400:
		      ac_slots_required=6;
            break;
            case 380: case 420:
		      ac_slots_required=4;
            break;
          }
		} else if (clk_freq == 817){
          switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw){
            case 0:
		      ac_slots_required=9;
            break;
            case 800: case 720:
		      ac_slots_required=7;
            break;
            case 460:
		      ac_slots_required=6;
            break;
            case 475: case 440: case 363: case 297: case 515:
		      ac_slots_required=4;
            break;
          }
		} else if (clk_freq==575 || clk_freq==510) {
			ac_slots_required=9;
		}
	  else if (clk_freq==435){
		ac_slots_required=10;
	  }
	  else if (clk_freq == 861 || clk_freq == 862){
        if(_tdm->_chip_data.soc_pkg.soc_vars.mn.os_config == 1)
          ac_slots_required=8;
        else
          ac_slots_required=5;
	  } else if(clk_freq == 550 || clk_freq == 450) {
          switch(_tdm->_chip_data.soc_pkg.soc_vars.mn.sku_core_bw){
            case 0:
		      ac_slots_required=9;
            break;
            case 580: case 460: case 400:
		      ac_slots_required=7;
            break;
            case 280: /*case 400:*/
		      ac_slots_required=6;
            break;
            case 300: case 340: case 235: case 275:
		      ac_slots_required=4;
            break;
          }

      }

		k=0;
		for (i=0; i<x_cal_len; i++) {
			if (x_cal_main[i]==MN_CPU_PORT      ||
				x_cal_main[i]==MN_LOOPBACK_PORT ||
				x_cal_main[i]==MN_AUX_TOKEN    ||
				x_cal_main[i]==MN_SBUS_TOKEN    ||
				x_cal_main[i]==MN_IDL1_TOKEN    ||
				x_cal_main[i]==MN_IDL2_TOKEN    ){
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
@name: chk_tdm_mn_subscription
@param:

Check subscription status per port 
 */
int chk_tdm_mn_subscription(tdm_mod_t *_tdm)
{
	int rst, result=PASS;
	
	/* 1. Check CPU port */
	if (FAIL==chk_tdm_mn_sub_cpu(_tdm)){
		result = FAIL;
	}
	/* 2. Check Loopback port */
	if (FAIL==chk_tdm_mn_sub_lpbk(_tdm)){
		result = FAIL;
	}
	/* 3. Check Agnostic accessory slots (must be equal to 8 or 10 in TD2/TD2+) */
	if (FAIL==chk_tdm_mn_sub_acc(_tdm)){
		result = FAIL;
	}
	
	/* 4. Check linerate ports */
	_tdm->_core_data.vars_pkg.pipe = MN_XPIPE_CAL_ID;
	rst  = chk_tdm_mn_sub_lr(_tdm);
	if(PASS!=rst){
		result = (result==FAIL) ? (result): (rst);
	}
	

	/* 5. Check oversub ports */
	_tdm->_core_data.vars_pkg.pipe = MN_XPIPE_CAL_ID;
	rst  = chk_tdm_mn_sub_os(_tdm);
	if(PASS!=rst){
		result = (result==FAIL) ? (result): (rst);
	}
	
	return result;
}

/**
@name: chk_tdm_mn_os_jitter
@param:

Checks distribution of oversub tokens to maximize performance of oversub scheduler
 */
int chk_tdm_mn_os_jitter(tdm_mod_t *_tdm)
{	
	int i, result=PASS, *cal_main, cal_len, os_range_thresh, os_clump_ave, os_clump_size, os_cnt_idx, lr_cnt_idx, group_size,	clump_cnt=0, lr_bw=0, os_bw=0;
	int ratio_opt, ratio_ave=0, ratio_sum1=0, ratio_sum2=0, ratio_var=0, ratio_i, ratio_floor, ratio_ceil;
	int dist_opt, dist_floor, dist_ceil, dist_i, dist_sum1=0, dist_sum2=0, dist_ave=0, dist_var=0;
	const char *table;
	int quanta ;
	
	if(_tdm->_core_data.vars_pkg.pipe_info.slot_unit == TDM_SLOT_UNIT_5G)
	quanta = 50;
	else
	quanta = 25;
	switch (_tdm->_core_data.vars_pkg.pipe) {
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
		if (cal_main[i]==MN_OVSB_TOKEN) {
			os_bw+=quanta;
		}
		else {
			lr_bw+=quanta;
		}
	}
	if (lr_bw==0 || os_bw==0) {
		TDM_PRINT1("TDM: Not applicable in - %s\n",table);
		TDM_SML_BAR
		return result;
	}
	
	TDM_PRINT1("TDM: Analyzing oversub - %s\n",table);
	dist_opt    =  (chk_tdm_round(chk_tdm_round(os_bw+lr_bw,quanta) , chk_tdm_round(os_bw,quanta)  ));
	ratio_opt   = chk_tdm_round(1, dist_opt);
	os_clump_ave= (os_bw/quanta)/((lr_bw/quanta)+1);
	
	if ((os_bw/10)>=200) {
		os_range_thresh = OS_RANGE_BW_HI;
	}
	else if ( ((os_bw/10)<200) && ((os_bw/10)>=100) ) {
		os_range_thresh = OS_RANGE_BW_MD;
	}
	else {
		os_range_thresh = JITTER_THRESH_MH;
	}
	ratio_floor = ( chk_tdm_round(1, (dist_opt+os_range_thresh)));
	ratio_ceil  = ( chk_tdm_round(1, (dist_opt-os_range_thresh)));
	dist_floor  = ((dist_opt-os_range_thresh)>0)? (dist_opt-os_range_thresh): (1);
	dist_ceil   = dist_opt+os_range_thresh;
	
	TDM_PRINT4("TDM:%12s|%12s|%12s|%12s\n","Ratio","Optimal", "Floor","Ceiling");
	TDM_PRINT4("TDM:%12s|%12d|%12d|%12d\n"," ",ratio_opt,ratio_floor,ratio_ceil);
	
	for (i=1; i<cal_len; i++) {
		if (cal_main[i]==MN_OVSB_TOKEN) {
			os_clump_size=1; os_cnt_idx=i; lr_cnt_idx=i; 
			while (os_cnt_idx<cal_len && cal_main[++os_cnt_idx]==MN_OVSB_TOKEN) {os_clump_size++;}
			group_size=os_clump_size;
			while(lr_cnt_idx>0 && cal_main[--lr_cnt_idx]!=MN_OVSB_TOKEN) {group_size++;}
			
			ratio_i = chk_tdm_round(os_clump_size, group_size);
			dist_i  = chk_tdm_round(group_size, os_clump_size);
			
			TDM_PRINT4("\t\t[%3d]\t|\tSlots = %2d\t|\tSlice size = %2d\t|\tGrant ratio = %0d\n",i,os_clump_size,group_size,ratio_i);
			if ( (ratio_i<ratio_floor) || (ratio_i>ratio_ceil) ) {
				TDM_PRINT2("\t\tGrant ratio suboptimal range is [%0d, %0d]\n",ratio_floor,ratio_ceil);
			}
			ratio_sum1+=ratio_i;
			ratio_sum2+=(ratio_i-ratio_opt)*(ratio_i-ratio_opt);
			dist_sum1 += dist_i;
			dist_sum2 += (dist_i-dist_opt)*(dist_i-dist_opt);
			clump_cnt++;
			
			if (os_clump_size>(os_clump_ave+os_range_thresh)) {
				TDM_PRINT3("\t\tOversub slice suboptimal, size %0d index %0d, limit %0d\n",os_clump_size,i,(os_clump_ave+os_range_thresh));
			}
			while (cal_main[i+1]==MN_OVSB_TOKEN) {i++;}
		}
	}
	/*if(clump_cnt!=0){
		ratio_ave = (ratio_sum1/clump_cnt);
		ratio_var = sqrt(ratio_sum2/clump_cnt);
		dist_ave  = dist_sum1/clump_cnt;
		dist_var  = sqrt(dist_sum2/clump_cnt);
	
		if ( (ratio_ave<ratio_floor) || (ratio_ave>ratio_ceil) || (dist_var>os_range_thresh) ) {
			result = FAIL;
		}
		if ( (dist_ave<dist_floor) || (dist_ave>dist_ceil) || (dist_var>os_range_thresh) ) {
			result = FAIL;
		}
	}*/
	
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
@name: chk_tdm_mn_lr_jitter
@param:

Checks jitter of line rate ports
 */
int chk_tdm_mn_lr_jitter(tdm_mod_t *_tdm, int fail[16])
{

	int i=0, j=0;
	int distance=0, port_phyID;
	const char *table;
	int tbl_len=0;
	int tdm_tbl[1024]; /* double the size of TDM table */
	int port_slots=0, pipe_id=0 ;
	int ideal_spacing_roundup=0,ideal_spacing_rounddown=0, rem=0, jitter_range=0, min_spacing=0, max_spacing=0, start=0, prev_pos=0;
	tdm_calendar_t tdm_cal;
        sal_memset(&tdm_cal, 0, sizeof(tdm_calendar_t));
	fail[6] = (fail[6]==UNDEF || fail[6]==PASS) ? (PASS):(fail[6]);
	fail[5] = (fail[5]==UNDEF || fail[5]==PASS) ? (PASS):(fail[5]);
	
	port_phyID = _tdm->_core_data.vars_pkg.port;
	for (i=0; i<1024; i++) tdm_tbl[i]=MN_NUM_EXT_PORTS;
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	switch (pipe_id) {
		case 2: table = "MMU Pipe 0"; 
				tdm_cal =_tdm->_chip_data.cal_2; 
				/*cal_len =_tdm->_chip_data.cal_2.cal_len; */
				break;
		default:
			fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(UNDEF);
			fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(UNDEF);
			TDM_ERROR1("Line-rate jitter check failed, unrecognized PIPE ID %0d\n",pipe_id);
			break;
	}

	/* Get actual TDM table*/

	if(tdm_cal.cal_main[0] == MN_NUM_EXT_PORTS) {  /*skip jitter chk for unused tdm*/ 
		TDM_PRINT0("Skip jitter  chk for unused tdm");
		return BOOL_TRUE; 
	} 
	
	tbl_len = tdm_cal.cal_len;
	for (i=tdm_cal.cal_len-1; i>0; i--) {
		if (tdm_cal.cal_main[i]==MN_NUM_EXT_PORTS) {
			tbl_len--;
		} else { break; }
	}	
	
	/* Find the number of LR slots for port_phyID */
	port_slots=0;
	for (i=0; i<tbl_len; i++) {
		if (port_phyID==tdm_cal.cal_main[i]) {
			port_slots++;
		}
		tdm_tbl[i] = tdm_cal.cal_main[i];
	}

	/* Check Min same port spacing for each CPU and LPBK port */
	for (i=0; i<tbl_len; i++){
      if(tdm_tbl[i] == MN_CPU_PORT) {
        for (j=1; j<MMU_AUX_MIN_SPACING; j++) {
          if(tdm_tbl[i+j] == MN_CPU_PORT ) {
      	  TDM_ERROR4("%s MIN same port spacing violation for CPU port %0d, (#%03d | #%03d)\n",table,tdm_tbl[i],i,(i+j));
          }
          }
        }
    }
	for (i=0; i<tbl_len; i++){
      if(tdm_tbl[i] == MN_LOOPBACK_PORT) {
        for (j=1; j<MMU_AUX_MIN_SPACING; j++) {
          if(tdm_tbl[i+j] == MN_LOOPBACK_PORT ) {
        	  TDM_ERROR4("%s MIN same port spacing violation for LOOPBACK port %0d, (#%03d | #%03d)\n",table,tdm_tbl[i],i,(i+j));
        	}
        }
      }
    }	
	
	/* Compute expected min & max spacing*/
	if (port_slots==0) {
		/*TDM_ERROR2("%s LR port=%d not scheduled\n", table, port_phyID);*/
	} else
	{
	/*TDM_ERROR4("%s tbl_len=%d, LR port=%d port_slots=%d\n", table, tbl_len, port_phyID, port_slots);*/
	ideal_spacing_roundup = chk_tdm_roundup(tbl_len, port_slots);
    ideal_spacing_rounddown = chk_tdm_rounddown(tbl_len, port_slots);
	rem = tbl_len % port_slots;
	jitter_range = chk_tdm_round(2*tbl_len*30,port_slots*100); 	/* 30% from ideal_spacing 2*tabl_len*30/port_slots/100 */

	if(rem < ((port_slots+1)/2)) {
		min_spacing = ideal_spacing_rounddown - chk_tdm_rounddown(jitter_range, 2);         /*ROUNDDOWN(ideal_spacing[port_speed],0) - ROUNDDOWN(range[port_speed]/2, 0);*/
		max_spacing = ideal_spacing_rounddown + chk_tdm_roundup(jitter_range, 2);     /*ROUNDDOWN(ideal_spacing[port_speed],0) + ROUNDUP(range[port_speed]/2, 0);*/
	} else {
		min_spacing = ideal_spacing_roundup - chk_tdm_roundup(jitter_range,2);   /*ROUNDUP(ideal_spacing[port_speed],0) - ROUNDUP(range[port_speed]/2, 0);*/
		max_spacing = ideal_spacing_roundup + chk_tdm_rounddown(jitter_range,2);       /*ROUNDUP(ideal_spacing[port_speed],0) + ROUNDDOWN(range[port_speed]/2, 0);*/
	}
	
	/* Create a Circular table from TDM table up to first port occurence*/
	for (i=0; i<tbl_len; i++) {
		tdm_tbl[tbl_len+i] = tdm_cal.cal_main[i];
		if (port_phyID==tdm_cal.cal_main[i]) { break; }
	}
	
	/* Check the jitter for all same port distances*/
	start=1;
	for (i=0; i<1024; i++) {
		if (port_phyID==tdm_tbl[i]) {
			if (start==1) {
				start=0;
				prev_pos=i;
			}
			else { /* actual check*/
				distance = i-prev_pos;
				if (distance<min_spacing){
					TDM_ERROR7("%s Line-rate jitter min spacing check failed, port=%0d, distance[pos=%d - pos%d]=%d min_spacing=%d max_spacing=%d\n", table, port_phyID, i%tbl_len, prev_pos, distance,min_spacing, max_spacing);
					fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(FAIL);
				}
				if (distance>max_spacing){
					TDM_ERROR7("%s Line-rate jitter max spacing check failed, port=%0d, distance[pos=%d - pos%d]=%d min_spacing=%d max_spacing=%d\n", table, port_phyID, i%tbl_len, prev_pos, distance,min_spacing, max_spacing);
					fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(FAIL);
				}				
				/*TDM_PRINT7("%s Line-rate jitter spacing, port=%0d, distance[pos=%d - pos%d]=%d min_spacing=%d max_spacing=%d\n", table, port_phyID, i%tbl_len, prev_pos, distance,min_spacing, max_spacing);*/
				prev_pos=i;
			}
		}
	}
	}

	return ((fail[5]==PASS && fail[6]==PASS) ? (PASS) : (FAIL));
}

/**
@name: chk_tdm_mn_lr_jitter_pgw
@param:

Checks jitter of line rate ports
 */
int chk_tdm_mn_lr_jitter_pgw(tdm_mod_t *_tdm)
{

	int i,loop=0;
	int distance, port_phyID;
	/*const char *table;*/
	int tbl_len;
	int tdm_tbl[1024]; /* double the size of TDM table */
	int port_slots, result=PASS;
	int ideal_spacing_roundup,ideal_spacing_rounddown, rem, jitter_range, min_spacing, max_spacing, start, prev_pos = 0;
	tdm_calendar_t tdm_cal;
	
	port_phyID = _tdm->_core_data.vars_pkg.port;

  for(loop=0; loop<2; loop++){
    for (i=0; i<1024; i++) tdm_tbl[i]=MN_NUM_EXT_PORTS;
    if(loop==1) {
      tbl_len = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len ;
      tdm_cal =_tdm->_chip_data.cal_0; 
    } else {
      tbl_len = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len ;
      tdm_cal =_tdm->_chip_data.cal_1; 
    }
    /* Find the number of LR slots for port_phyID */
    port_slots=0;
    for (i=0; i<tbl_len; i++) {
    	if (port_phyID==tdm_cal.cal_main[i]) {
    		port_slots++;
    	}
    	tdm_tbl[i] = tdm_cal.cal_main[i];
    }
    if(tdm_tbl[0] == tdm_tbl[tbl_len-1]){
       MN_TOKEN_CHECK(tdm_tbl[i]) {
       result = FAIL;
       TDM_ERROR1("1 SPACING VOILATION:Found back to back ports in PGW TDM TBL0:port %0d\n",tdm_tbl[i]);
       }
      }
    if(tdm_tbl[0] == tdm_tbl[tbl_len-1]){
      MN_TOKEN_CHECK(tdm_tbl[i]) {
      result = FAIL;
      TDM_ERROR1("2 SPACING VOILATION:Found back to back ports in PGW TDM TBL1:port %0d\n",tdm_tbl[i]);
        }
    }
    
    for (i=0; i<tbl_len; i++){
      MN_TOKEN_CHECK(tdm_tbl[i]) {
        if(tdm_tbl[i] == tdm_tbl[i+1]){
          result = FAIL;
          TDM_ERROR1("3 SPACING VOILATION:Found back to back ports in PGW TDM TBL0:port %0d\n",tdm_tbl[i]);
        }
      }
    }
    
    /* Get actual TDM table*/
    
    if(tdm_cal.cal_main[0] == MN_NUM_EXT_PORTS) {  /*skip jitter chk for unused tdm*/ 
    	TDM_PRINT0("Skip jitter  chk for unused tdm");
    	return BOOL_TRUE; 
    } 
    
    /*tbl_len = tdm_cal.cal_len;
    for (i=tdm_cal.cal_len-1; i>0; i--) {
    	if (tdm_cal.cal_main[i]==MN_NUM_EXT_PORTS) {
    		tbl_len--;
    	} else { break; }
    }	*/
    
    
    /* Compute expected min & max spacing*/
    if (port_slots==0) {
    	/*TDM_ERROR2("%s LR port=%d not scheduled\n", table, port_phyID);*/
    } else
    {
    /*TDM_ERROR4("%s tbl_len=%d, LR port=%d port_slots=%d\n", table, tbl_len, port_phyID, port_slots);*/
    ideal_spacing_roundup = chk_tdm_roundup(tbl_len, port_slots);
    ideal_spacing_rounddown = chk_tdm_rounddown(tbl_len, port_slots);
    rem = tbl_len % port_slots;
    jitter_range = chk_tdm_round(2*tbl_len*30,port_slots*100); 	/* 30% from ideal_spacing 2*tabl_len*30/port_slots/100 */
    
    if(rem < ((port_slots+1)/2)) {
    	min_spacing = ideal_spacing_rounddown - chk_tdm_rounddown(jitter_range, 2);         /*ROUNDDOWN(ideal_spacing[port_speed],0) - ROUNDDOWN(range[port_speed]/2, 0);*/
    	max_spacing = ideal_spacing_rounddown + chk_tdm_roundup(jitter_range, 2);     /*ROUNDDOWN(ideal_spacing[port_speed],0) + ROUNDUP(range[port_speed]/2, 0);*/
    } else {
    	min_spacing = ideal_spacing_roundup - chk_tdm_roundup(jitter_range,2);   /*ROUNDUP(ideal_spacing[port_speed],0) - ROUNDUP(range[port_speed]/2, 0);*/
    	max_spacing = ideal_spacing_roundup + chk_tdm_rounddown(jitter_range,2);       /*ROUNDUP(ideal_spacing[port_speed],0) + ROUNDDOWN(range[port_speed]/2, 0);*/
    }
    
    /* Create a Circular table from TDM table up to first port occurence*/
    for (i=0; i<tbl_len; i++) {
    	tdm_tbl[tbl_len+i] = tdm_cal.cal_main[i];
    	if (port_phyID==tdm_cal.cal_main[i]) { break; }
    }
    
    /* Check the jitter for all same port distances*/
    start=1;
    for (i=0; i<1024; i++) {
    	if (port_phyID==tdm_tbl[i]) {
    		if (start==1) {
    			start=0;
    			prev_pos=i;
    		}
    		else { /* actual check*/
    			distance = i-prev_pos;
    			if (distance<min_spacing){
    				TDM_ERROR6("Line-rate jitter min spacing check failed, port=%0d, distance[pos=%d - pos%d]=%d min_spacing=%d max_spacing=%d\n", port_phyID, i%tbl_len, prev_pos, distance,min_spacing, max_spacing);
    				result = FAIL;
    			}
    			if (distance>max_spacing){
    				TDM_ERROR6("Line-rate jitter max spacing check failed, port=%0d, distance[pos=%d - pos%d]=%d min_spacing=%d max_spacing=%d\n", port_phyID, i%tbl_len, prev_pos, distance,min_spacing, max_spacing);
    				result = FAIL;
    			}				
    			/*TDM_PRINT7("Line-rate jitter spacing, port=%0d, distance[pos=%d - pos%d]=%d min_spacing=%d max_spacing=%d\n", port_phyID, i%tbl_len, prev_pos, distance,min_spacing, max_spacing);*/
    			prev_pos=i;
    		}
    	}
    }
    }
	}

	return (result);
}
/**
@name: chk_tdm_mn_lls
@param:

Checks linked list scheduler
 */
int chk_tdm_mn_lls(tdm_mod_t *_tdm)
{
	int i, j, k, port_phyID, result=PASS, *cal_main, cal_len, pipe_id, clk_freq, min_spacing=0, line_rate=BOOL_TRUE;
	enum port_speed_e *port_speeds;
	
	port_speeds= _tdm->_chip_data.soc_pkg.speed;
        clk_freq   = _tdm->_chip_data.soc_pkg.clk_freq;
	pipe_id    = _tdm->_core_data.vars_pkg.pipe;
	
	switch (pipe_id) {
		case 2:  
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
		if(cal_main[i] == MN_OVSB_TOKEN){
    line_rate= BOOL_FALSE;
    break;
    }
  }
 if(line_rate) {
	for (i=0; i<cal_len; i++) {
		port_phyID = cal_main[i];
		MN_TOKEN_CHECK(port_phyID){
	  	switch (clk_freq) {
	  		case 933: 
          min_spacing=LLS_MIN_SPACING; break;
	  		case 793: case 794:
          if (port_speeds[port_phyID]<SPEED_42G) {min_spacing=(LLS_MIN_SPACING-2);}
          else if (port_speeds[port_phyID]>=SPEED_100G) {min_spacing=(LLS_MIN_SPACING-6);}
          else if (port_speeds[port_phyID]==SPEED_50G) {min_spacing=(LLS_MIN_SPACING-4);}
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
					/*result=FAIL; 
					TDM_ERROR4("LLS check failed in port %0d between [%0d,%0d] violates LLS min spacing requirement\n", port_phyID,i,k);
					break;*/
				}
			}
		}
	}
   
	/*for (i=0; i<cal_len; i++){
 		MN_TOKEN_CHECK(cal_main[i]) {
				k = ( (i+6)<cal_len) ? (i+6) : (i+6-cal_len) ;
    	  if(cal_main[k] == cal_main[i]) {
						result = FAIL;
						TDM_ERROR4(" LLS 1:6 same port spacing violation port %0d, (#%03d | #%03d)\n",cal_main[i],i,k);
				}
				k = ( (i+8)<cal_len) ? (i+8) : (i+8-cal_len) ;
    	  if(cal_main[k] == cal_main[i]) {
						result = FAIL;
						TDM_ERROR4(" LLS 1:8 same port spacing violation port %0d, (#%03d | #%03d)\n",cal_main[i],i,k);
				}
      }
    }*/
  }
	return result;
}

/**
@name: chk_tdm_mn_ovs_space
@param:

Checks that no port is back to back in the OVS table
 */
int chk_tdm_mn_ovs_space( tdm_mod_t *_tdm)
{
	int i, result=PASS, cal0_len, cal1_len,
	    *ovs_cal0, *ovs_cal1;

  if(_tdm->_core_data.vars_pkg.os_enable == 1){
    ovs_cal0 = _tdm->_chip_data.cal_0.cal_grp[0];
    ovs_cal1 = _tdm->_chip_data.cal_1.cal_grp[0];
    
    for(i =0; i<MN_OS_LLS_GRP_LEN; i++) {
      if(ovs_cal0[i] == MN_NUM_EXT_PORTS) break;
     }
    cal0_len = 48;
    
    for(i =0; i<MN_OS_LLS_GRP_LEN; i++) {
      if(ovs_cal1[i] == MN_NUM_EXT_PORTS) break;
     }
    cal1_len = 48;
    
    
    	if(ovs_cal0[0] == ovs_cal0[cal0_len-1] && ovs_cal0[0] != MN_OVSB_TOKEN ){
    	 result = FAIL;
       TDM_ERROR1("1SPACING VOILATION:Found back to back ports in OVS TDM TBL0:port %0d\n",ovs_cal0[i]);
      }
     	if(ovs_cal1[0] == ovs_cal1[cal1_len-1] && ovs_cal1[0] != MN_OVSB_TOKEN ){
    	 result = FAIL;
       TDM_ERROR1("2SPACING VOILATION:Found back to back ports in OVS TDM TBL1:port %0d\n",ovs_cal1[i]);
      }
    
    for (i=0; i<cal0_len; i++){
      MN_TOKEN_CHECK(ovs_cal0[i]) {
    	 if(ovs_cal0[i] == ovs_cal0[i+1]){
    	  result = FAIL;
        TDM_ERROR1("3SPACING VOILATION:Found back to back ports in OVS TDM TBL0:port %0d\n",ovs_cal0[i]);
       }
      }
    }
    for (i=0; i<cal1_len; i++){
      MN_TOKEN_CHECK(ovs_cal1[i]) {
     	 if(ovs_cal1[i] == ovs_cal1[i+1]){
    	  result = FAIL;
        TDM_ERROR1("4SPACING VOILATION:Found back to back ports in OVS TDM TBL1:port %0d\n",ovs_cal1[i]);
       }
      }
    }
	}
	
	return result;
}

/**
@name: chk_tdm_mn_td2p
@param:

Checks chip specific 
 */
void chk_tdm_mn_td2p(tdm_mod_t *_tdm, int fail[16])
{
	int i,j,k, clk, core_bw, accessories, port_spd, port_state, port_phyID, phy_idx_s = 0, phy_idx_e = 0, msub_x0, msub_x1, pgw_x0_len, pgw_x1_len, t2,t3,t4,t5;
	int s1, s1xcnt=0, s1xavg=0, s2, s2xcnt=0, s2xavg=0, s3, s3xcnt=0, s3xavg=0, /*s5=4,*/ s5xcnt=0, s5xavg=0, s4=5, s4xcnt=0, s4xavg=0;
	int pgw_tbl_len = 0, *pgw_tbl_ptr = NULL, **ovsb_tbl_ptr, wc_principle, **wc_checker, pgw_mtbl[MN_LR_LLS_LEN*2], pgw_tracker[MN_NUM_EXT_PORTS];
	const char *str_pgw_tbl_x0, *str_pgw_tbl_x1,*str_ovs_tbl_x0,*str_ovs_tbl_x1;
	char *str_pgw_ptr = NULL , *str_ovs_ptr = NULL;
	
	/* Initialize */
	str_pgw_tbl_x0 = "PGW table x0";
	str_pgw_tbl_x1 = "PGW table x1";
	str_ovs_tbl_x0 = "PGW OVSB table x0";
	str_ovs_tbl_x1 = "PGW OVSB table x1";
	
	pgw_x0_len=0; pgw_x1_len=0; 
	clk = _tdm->_chip_data.soc_pkg.clk_freq;
		
	for (i=0; i<MN_LR_LLS_LEN; i++) {
		if (_tdm->_chip_data.cal_0.cal_main[i]!=MN_NUM_EXT_PORTS) pgw_x0_len++;
		if (_tdm->_chip_data.cal_1.cal_main[i]!=MN_NUM_EXT_PORTS) pgw_x1_len++;
	}
	
	switch(clk) {
    case 840: core_bw=511; accessories=9; break; 
    case 793: case 794: core_bw=480; accessories=18; break; 
    case 575: core_bw=340; accessories=9; break; 
    case 510: core_bw=300; accessories=13; break; 
    case 435: core_bw=260; accessories=10; break; 
		default: 			core_bw=567; accessories=9 ; break; 
	}
	TDM_PRINT3("TDM: _____VERBOSE: (clk - %0d) (core bw - %0d) (mmu acc - %0d)\n", clk, core_bw, accessories);
	s4=(((4.95*(((double)clk/76))))/10); /*100G*/
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
		
		for (i=1; i<(MN_NUM_EXT_PORTS-MN_EXT_PLUS_CPU_PORT); i++) {
			if((_tdm->_chip_data.soc_pkg.state[i-1]== PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i-1]== PORT_STATE__LINERATE_HG) && 
			   _tdm->_chip_data.soc_pkg.state[i]  != PORT_STATE__COMBINE  &&
			   _tdm->_chip_data.soc_pkg.speed[i]  <  SPEED_10G){
				if     (i>=1  && i<=(MN_NUM_PHY_PORTS/2) ){msub_x0++;}
				else if(i>=((MN_NUM_PHY_PORTS/2)+1) && i<=MN_NUM_PHY_PORTS ){msub_x1++;}
			}
		}

		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table x0 is %0d\n", pgw_x0_len);
		TDM_PRINT1("TDM: _____VERBOSE: length of pgw table x1 is %0d\n", pgw_x1_len);
		
		if (msub_x0==0 && msub_x1==0) { 
			switch (core_bw) {
				case 567:
					if (pgw_x0_len>160 || pgw_x1_len>160)  {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					  if (pgw_x0_len!=pgw_x1_len) {TDM_WARN0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 511: case 480:
					if (pgw_x0_len>48 || pgw_x1_len>48 ) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len ) {TDM_WARN0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 340:
					if (pgw_x0_len>34 || pgw_x1_len>34 ) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len ) {TDM_WARN0(" PGW table asymmetry on symmetrically subscribed core bw\n");}
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
				case 1:  pgw_tbl_ptr = _tdm->_chip_data.cal_1.cal_main; 
						 pgw_tbl_len = pgw_x1_len; 
						 str_pgw_ptr = (char *) str_pgw_tbl_x1;
						 break;
			}
			
			/* check transcription and TSC violations */
			for (i=0; i<pgw_tbl_len; i++) {
				port_phyID = pgw_tbl_ptr[i];
				MN_TOKEN_CHECK(port_phyID){
					if(port_phyID>0 && port_phyID<(MN_NUM_EXT_PORTS-MN_EXT_PLUS_CPU_PORT)){
						wc_principle = -1;
						for (j=0; j<MN_NUM_PHY_PM; j++) {
							if (wc_checker[j][0] == port_phyID || wc_checker[j][1] == port_phyID || wc_checker[j][2] == port_phyID || wc_checker[j][3] == port_phyID){
								wc_principle = j;
								break;
							}
            }
						if (wc_principle == -1) {
							fail[8] = 1; 
							TDM_ERROR2("TSC transcription corruption %s port %0d\n", str_pgw_ptr, port_phyID);
						}
						else if ((pgw_tbl_len>8) && 
								 (pgw_tbl_ptr[i+1]==wc_checker[wc_principle][0]  || 
								  pgw_tbl_ptr[i+1]==wc_checker[wc_principle][1]  || 
								  pgw_tbl_ptr[i+1]==wc_checker[wc_principle][2]  || 
								  pgw_tbl_ptr[i+1]==wc_checker[wc_principle][3]) && (pgw_tbl_ptr[i+1] != MN_NUM_EXT_PORTS) ){
								/*fail[9]=1; 
								TDM_ERROR3("TSC proximity violation in %s between index %0d and index %0d\n", str_pgw_ptr, i, (i+1));*/
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
						 phy_idx_e   = (MN_NUM_PHY_PORTS/2);
						 str_pgw_ptr = (char *) str_pgw_tbl_x0;
						 break;
				/* pgw_tdm_tbl_x1 */
				case 1:  pgw_tbl_ptr = _tdm->_chip_data.cal_1.cal_main; 
						 pgw_tbl_len = pgw_x1_len; 
						 phy_idx_s   = ((MN_NUM_PHY_PORTS/2)+1);
						 phy_idx_e   = MN_NUM_PHY_PORTS;
						 str_pgw_ptr = (char *) str_pgw_tbl_x1;
						 break;
			}
                        t2 = (pgw_tbl_len/4); /* Per slot BW is now 5G and not 2.5G */
                        t3 = (pgw_tbl_len/8);
                        t4 = (pgw_tbl_len/20);
                        t5 = (pgw_tbl_len/10);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 20G PGW spacing %0d\n",str_pgw_ptr, t2);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 40G PGW spacing %0d\n",str_pgw_ptr, t3);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 100G PGW spacing %0d\n",str_pgw_ptr, t4);
			TDM_PRINT2("TDM: _____VERBOSE: %s calculated 50G PGW spacing %0d\n",str_pgw_ptr, t5);
			
			/* set tracker */
			for (i=0; i<(MN_NUM_EXT_PORTS-MN_NUM_PHY_PORTS); i++) {pgw_tracker[i]=0;}
			for (i=0; i<pgw_tbl_len; i++) { pgw_mtbl[i]=pgw_tbl_ptr[i]; pgw_mtbl[i+pgw_tbl_len]=pgw_tbl_ptr[i];}
			for (i=0; i<pgw_tbl_len; i++) {
				port_phyID = pgw_tbl_ptr[i];
				MN_TOKEN_CHECK(port_phyID){
					if(port_phyID>0 && port_phyID<(MN_NUM_EXT_PORTS-MN_EXT_PLUS_CPU_PORT)){
						pgw_tracker[port_phyID] = 1;
						for (j=1; j<=pgw_tbl_len; j++) {
							if (pgw_mtbl[i+j]!=pgw_mtbl[i]) {pgw_tracker[port_phyID]++;} 
							else {break;}
						}
					}
				}
			}
			
			/* check pgw spacing */
			for (i=phy_idx_s; i<phy_idx_e; i++){
				if(pgw_tracker[i]!=0){
					switch (_tdm->_chip_data.soc_pkg.speed[i]) {
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							if (pgw_tracker[i] != t2) {
								/*fail[10]=1;*/ 
								TDM_WARN3(" %s, 20G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);
							}
							break;
						case SPEED_42G:
						case SPEED_40G:
							if (pgw_tracker[i] != t3) {
								/*fail[10]=1;*/ 
                                                              if (clk == 705 && pgw_tracker[i] > 12){
								TDM_WARN3(" %s, 40G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);
                                                              }
							}
							break;
						case SPEED_50G:
							if (pgw_tracker[i] != t5) {
								/*fail[10]=1;*/ 
								/*TDM_WARN3(" %s, 50G port spacing deviation from average port %0d (%0d)\n", str_pgw_ptr, i,pgw_tracker[i]);*/
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

		if (s1xavg>0) if ( (s1xavg > (s1*101) || s1xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("10G line rate not within 1 percent tolerance in x pipe, (s1xavg = %0d)\n", s1xavg);}
		if (s2xavg>0) if ( (s2xavg > (s1*101) || s2xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("20G line rate not within 1 percent tolerance in x pipe, (s2xavg = %0d)\n", s2xavg);}
		if (s3xavg>0) if ( (s3xavg > (s1*101) || s3xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("40G line rate not within 1 percent tolerance in x pipe, (s3xavg = %0d)\n", s3xavg);}
		if (s4xavg>0) if ( (s4xavg > (s1*101) || s4xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("100G line rate not within 1 percent tolerance in x pipe, (s4xavg = %0d)\n", s4xavg);}
		if (s5xavg>0) if ( (s5xavg > (s1*101) || s5xavg < (s1*99)) ) {/*fail[11]=1;*/ TDM_WARN1("120G line rate not within 1 percent tolerance in x pipe, (s5xavg = %0d)\n", s5xavg);}
		fail[11]= (fail[11]==2) ? (PASS) : (FAIL);
	}
	
	/* [12] Check 100G/120G port numbering */
	if (fail[12]==2) {
		for (i=1; i<(MN_NUM_EXT_PORTS-MN_EXT_PLUS_CPU_PORT); i++) {
			if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G || _tdm->_chip_data.soc_pkg.speed[i]==SPEED_120G) {
				if (!mn_is_clport(i)) {
					fail[12]=1; 
					TDM_ERROR1("port number %0d is a 100G/120G port that is improperly subscribed\n",i);
				}
			}
		}
		fail[12]= (fail[12]==2) ? (PASS) : (FAIL);
	}
	
	/* [13] Check port subscription */
	if (fail[13]==2) {
		for(i=1; i<(MN_NUM_EXT_PORTS-MN_EXT_PLUS_CPU_PORT); i++){
			/* pgw_x_0 */
			if(i<=(MN_NUM_PHY_PORTS/2)) {
				pgw_tbl_ptr  = _tdm->_chip_data.cal_0.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_0.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_x0;
				str_ovs_ptr  = (char *) str_ovs_tbl_x0;
				pgw_tbl_len = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw0_len; 
			}
			/* pgw_x_1 */
			else {
				pgw_tbl_ptr  = _tdm->_chip_data.cal_1.cal_main;
				ovsb_tbl_ptr = _tdm->_chip_data.cal_1.cal_grp;
				str_pgw_ptr  = (char *) str_pgw_tbl_x1;
				str_ovs_ptr  = (char *) str_ovs_tbl_x1;
				pgw_tbl_len = _tdm->_chip_data.soc_pkg.soc_vars.mn.pgw1_len; 
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
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i, str_pgw_ptr, k);}
							break;
						case SPEED_10G:
						case SPEED_10G_XAUI:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=2 && k!=4 && k!=20) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_12p5G:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=5) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=4 && k!=8) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_25G:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=5 && k !=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_42G_HG2:
						case SPEED_42G:
						case SPEED_40G:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							   if (k != 5 && k != 16 && k != 8) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);} 
							break;
						case SPEED_50G:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							  if (k!=20 && k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);} 
							break;
						case SPEED_120G:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=48) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
							break;
						case SPEED_100G:
							for (j=0; j<pgw_tbl_len; j++) {if (pgw_tbl_ptr[j]==i) {k++;}}
							if (k!=40 && k!=20) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_pgw_ptr,k);}
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
						case SPEED_5G:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=1) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_10G:
						case SPEED_10G_XAUI:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_21G:
						case SPEED_20G:
						case SPEED_21G_DUAL:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_25G:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=3 && k!=4 && k!=1 && k != 2 && k!=8 && k!=6) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
                        break;
						case SPEED_42G_HG2:
						case SPEED_42G:
						case SPEED_40G:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=4) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
							break;
						case SPEED_50G:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=5 && k!=4 && k!=2) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
              break;
						case SPEED_100G:
							for (j=0; j<48; j++) {if (ovsb_tbl_ptr[0][j]==i) {k++;}}
							if (k!=10 && k!=8 && k!=4) {fail[13]=1; TDM_ERROR3("port %0d subscription error in %s (k=%0d)\n",i,str_ovs_ptr,k);}
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
@name: chk_tdm_mn_tbl
@param: 
 */
int
chk_tdm_mn_tbl( tdm_mod_t *_tdm)
{	
	int i, result, mmu_x_len;
        int fail[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};  	
	mmu_x_len= _tdm->_chip_data.cal_2.cal_len;
	for (i=(_tdm->_chip_data.cal_2.cal_len-1); i>0; i--){
		if(_tdm->_chip_data.cal_2.cal_main[i]==MN_NUM_EXT_PORTS){
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
		_tdm->_core_data.vars_pkg.pipe = MN_PIPE_X_ID;
		result  = chk_tdm_mn_struc(_tdm);
		fail[0] = (fail[0]==FAIL || fail[0]==UNDEF) ? (fail[0]):(result);
	}
	
	/* [1] Check TSC transcription in MMU tables */
	if (fail[1]==2) {
		TDM_PRINT0("\nTDM: [1] Checking TSC transcription\n"); TDM_SML_BAR
		fail[1] = chk_tdm_mn_tsc(_tdm);	
	}
	
	/* [2] Check sister port spacing in MMU tables */
	if (fail[2]==2) {
		TDM_PRINT0("\nTDM: [2] Checking min spacing\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = MN_XPIPE_CAL_ID;
		result  = PASS; /*This check is alredy taken care in API chk_tdm_mn_lr_jitter for MMU  */
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
	}
	
	/* [3] Check (CPU/loopback/regular) port subscription and accessory slot allocation. */
	if (fail[3]==2) {
		TDM_PRINT0("\nTDM: [3] Checking per port subscription stats\n"); TDM_SML_BAR
		fail[3] = chk_tdm_mn_subscription(_tdm);
	}
	
	/* [4] Check Oversub jitter */
	if (fail[4]==2) {
		TDM_PRINT0("\nTDM: [4] Checking Oversub jitter\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = MN_XPIPE_CAL_ID;
		result  = chk_tdm_mn_os_jitter(_tdm);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
	}
	
	/* [5] [6] Check Linerate jitter */
	if (fail[5]==2 || fail[6]==2) {
		TDM_PRINT0("\nTDM: [5] [6] Checking line rate jitter\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = MN_XPIPE_CAL_ID;
		chk_tdm_mn_lr_jitter(_tdm, fail);
	}
	
	/* [14] Check LLS min spacing requiement */
/*#ifdef _LLS_SCHEDULER*/
	if (fail[14]==2 && _tdm->_chip_data.soc_pkg.clk_freq>=435) {
		TDM_PRINT0("\nTDM: [14] Checking LLS min spacing requirement\n"); TDM_SML_BAR
		/* X Pipe */
		_tdm->_core_data.vars_pkg.pipe = MN_XPIPE_CAL_ID;
		result = chk_tdm_mn_lls(_tdm);
		fail[14] = (fail[14]==FAIL || fail[14]==UNDEF) ? (fail[14]):(result);
	}
/*#endif*/

	/* [15] Check back to back ports in PGW/OVS TDM table */
	if (fail[15]==2) {
		TDM_PRINT0("\nTDM: [15] Checking min spacing in PGW/OVS table\n"); TDM_SML_BAR
		fail[15] = chk_tdm_mn_lr_jitter_pgw(_tdm)  && chk_tdm_mn_ovs_space(_tdm);	
	}

	/* [7] [8] [9] [10] [11] [12] [13] Check chip specific constraints */
	if (fail[7]==2 || fail[8]==2 || fail[9]==2 || fail[10]==2 || fail[11]==2 || fail[12]==2 || fail[13]==2){
		TDM_PRINT0("\nTDM: --- Chip specific checkers [7] [8] [9] [10] [11] [12] [13]\n"); TDM_SML_BAR
		chk_tdm_mn_td2p( _tdm, fail );
	}
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
                TDM_ERROR0("Monterey TDM Self Check: *** FAILED ***\n");
                TDM_SML_BAR
                TDM_ERROR0("TDM: Fail vector: [");
                for (i=0; i<16; i++) {
                        TDM_ERROR1(" %0d ",fail[i]);
                }
                TDM_ERROR0("]\n");
        return FAIL;
        }
        else {
                TDM_PRINT0("Monterey TDM Self Check: *** PASSED ***\n");
        }
        TDM_SML_BAR
        TDM_PRINT0("\n");
        TDM_PRINT0("TDM: TDM Self Check Complete.\n");
        TDM_BIG_BAR

        return PASS; 
}

