/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip main functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_ap_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_ap_corereq( tdm_mod_t *_tdm )
{
	int idx1;
	
	/* X-Pipe: */
	if (_tdm->_core_data.vars_pkg.pipe==AP_PIPE_X_ID) {
		_tdm->_core_data.vars_pkg.cal_id=2;
		_tdm->_core_data.vmap=(unsigned short **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned short *), "vector_map_l1");
		for (idx1=0; idx1<(_tdm->_core_data.vmap_max_wid); idx1++) {
			_tdm->_core_data.vmap[idx1]=(unsigned short *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned short), "vector_map_l2");
		}
	}
	/* unrecognised Pipe number */
	else {
		TDM_ERROR1("Unrecgonized PIPE ID %d \n", _tdm->_core_data.vars_pkg.pipe);
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_ap_vbs_wrapper
@param:

Code wrapper for egress TDM scheduling
 */
int
tdm_ap_vbs_wrapper( tdm_mod_t *_tdm )
{
	int idx1, acc, clk, lr_buffer_idx=0, os_buffer_idx=0, /*pipe,*/ higig_mgmt=BOOL_FALSE, lr_idx_limit=LEN_760MHZ_EN, port;
	int *pgw_tdm_tbl_0, *pgw_tdm_tbl_1, *ovs_tdm_tbl_0, *ovs_tdm_tbl_1;

  /* to get actual pmap for spacing and other functions */
 /* tdm_ap_actual_pmap(_tdm);*/
	
	/* accessory slots */
	switch (_tdm->_chip_data.soc_pkg.clk_freq) {
		case 933:
			clk=933; acc=AP_ACC_PORT_NUM; break;
		case 840:
			clk=840; acc=AP_ACC_PORT_NUM; break;
		case 793:
		case 794: 
		case 795:
		case 796:
		case 797:
		case 798:
			clk=793; acc=(AP_ACC_PORT_NUM-1); break;
		case 575: 
			clk=575; acc=AP_ACC_PORT_NUM; break;
		case 510:
			clk=510; acc=(AP_ACC_PORT_NUM+4); break;
		case 435:
			clk=435; acc=(AP_ACC_PORT_NUM+1); break;
		default:
			TDM_ERROR1("Invalid frequency %d (MHz)\n", _tdm->_chip_data.soc_pkg.clk_freq);
			return 0;
	}
	
	switch (_tdm->_core_data.vars_pkg.pipe){
		case AP_PIPE_X_ID:   /*pipe = 0;*/ tdm_ap_print_quad(_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state, AP_NUM_PHY_PORTS, 1,   72); break;
		default:
			TDM_ERROR1("Invalid pipe ID %d \n", _tdm->_core_data.vars_pkg.pipe);
			return 0;
	}
	
	/* lr_buff and os_buff */
		pgw_tdm_tbl_0 = _tdm->_chip_data.cal_0.cal_main;
		pgw_tdm_tbl_1 = _tdm->_chip_data.cal_1.cal_main;
		ovs_tdm_tbl_0 = _tdm->_chip_data.cal_0.cal_grp[0];
		ovs_tdm_tbl_1 = _tdm->_chip_data.cal_1.cal_grp[0];
	
  for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
		_tdm->_core_data.vars_pkg.lr_buffer[idx1]=AP_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.os_buffer[idx1]=AP_NUM_EXT_PORTS;
	}
	for (idx1=0; idx1<AP_LR_LLS_LEN; idx1++) {
		AP_TOKEN_CHECK(pgw_tdm_tbl_0[idx1]) {
			_tdm->_core_data.vars_pkg.lr_buffer[lr_buffer_idx++]=pgw_tdm_tbl_0[idx1];
		}
	}
	for (idx1=0; idx1<AP_LR_LLS_LEN; idx1++) {
		AP_TOKEN_CHECK(pgw_tdm_tbl_1[idx1]) {
			_tdm->_core_data.vars_pkg.lr_buffer[lr_buffer_idx++]=pgw_tdm_tbl_1[idx1];
		}
	}
	for (idx1=0; idx1<AP_OS_LLS_GRP_LEN; idx1++) {
		AP_TOKEN_CHECK(ovs_tdm_tbl_0[idx1]) {
			_tdm->_core_data.vars_pkg.os_buffer[os_buffer_idx++]=ovs_tdm_tbl_0[idx1];
		}
	}
	for (idx1=0; idx1<AP_OS_LLS_GRP_LEN; idx1++) {
		AP_TOKEN_CHECK(ovs_tdm_tbl_1[idx1]) {
			_tdm->_core_data.vars_pkg.os_buffer[os_buffer_idx++]=ovs_tdm_tbl_1[idx1];
		}
	}
	
	/* higig_mgmt */
	/*if ( (_tdm->_chip_data.soc_pkg.soc_vars.ap.pm_encap_type[AP_NUM_PHY_PM]==PM_ENCAP__HIGIG2 && (pipe==1||pipe==2)) && 
	     ((_tdm->_core_data.vars_pkg.lr_buffer[0]!=AP_NUM_EXT_PORTS && _tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ) || (_tdm->_core_data.vars_pkg.lr_buffer[0]==AP_NUM_EXT_PORTS)) ) {
		higig_mgmt=BOOL_TRUE;
	}*/
	
	/* lr_idx_limit */
	port = _tdm->_core_data.vars_pkg.port;
	_tdm->_core_data.vars_pkg.port = _tdm->_core_data.vars_pkg.lr_buffer[0];
	/*if ( (tdm_ap_check_ethernet(_tdm)) && (higig_mgmt==BOOL_FALSE)){*/
		switch (clk) {
			case 933: lr_idx_limit=(LEN_933MHZ_EN-acc); break;
			case 840: lr_idx_limit=(LEN_840MHZ_EN-acc); break;
			case 793: 
                        case 794: 
                        case 795: 
                        case 796: 
                        case 797: 
                        case 798: 
                          lr_idx_limit=(LEN_793MHZ_EN-acc); break;
			case 575: lr_idx_limit=(LEN_575MHZ_EN-acc); break;
			case 510: lr_idx_limit=(LEN_510MHZ_EN-acc); break;
			case 435: lr_idx_limit=(LEN_435MHZ_EN-acc); break;
			default : break;
		}
	/*}
	else {
		switch (clk) {
			case 933: lr_idx_limit=(LEN_933MHZ_HG-acc); break;
			case 840: lr_idx_limit=(LEN_840MHZ_HG-acc); break;
			case 793: 
                        case 794: 
                        case 795: 
                        case 796: 
                        case 797: 
                        case 798: 
                          lr_idx_limit=(LEN_793MHZ_HG-acc); break;
			case 575: lr_idx_limit=(LEN_575MHZ_HG-acc); break;
			case 510: lr_idx_limit=(LEN_510MHZ_HG-acc); break;
			case 435: lr_idx_limit=(LEN_435MHZ_HG-acc); break;
			default : break;
		}
	}*/
	_tdm->_core_data.vars_pkg.port = port;
	
	/* core parameters */
	_tdm->_core_data.vmap_max_len        = AP_VMAP_MAX_LEN;
	_tdm->_core_data.vmap_max_wid        = AP_VMAP_MAX_WID;
	_tdm->_core_data.rule__same_port_min = LLS_MIN_SPACING;
	_tdm->_core_data.rule__prox_port_min = VBS_MIN_SPACING;
	
	_tdm->_chip_data.soc_pkg.clk_freq    = clk;
	_tdm->_chip_data.soc_pkg.tvec_size   = acc;
	_tdm->_chip_data.soc_pkg.lr_idx_limit= lr_idx_limit;
	_tdm->_chip_data.soc_pkg.soc_vars.ap.higig_mgmt = higig_mgmt;

	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_ap_lls_wrapper
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_ap_lls_wrapper( tdm_mod_t *_tdm )
{
	int idx;
  int n, pgw_ll_len, pgw_ll_len_max; 
  int ap_fb_num_ovs=0, ap_fb_num_lr=0, avg=0, token=0, len=0, pool=0, start_index=0;
	tdm_ap_chip_legacy_t *_ap_chip;
	
	_ap_chip = TDM_ALLOC(sizeof(tdm_ap_chip_legacy_t), "TDM chip legacy");
	if (!_ap_chip) {return FAIL;}
	tdm_chip_ap_shim__ing_wrap(_tdm, _ap_chip);
	
	TDM_PRINT0("TDM: Linked list round robin\n");
	for (idx=0; idx<AP_NUM_QUAD; idx++) {
		_tdm->_chip_data.soc_pkg.soc_vars.ap.pgw_num = idx;
		_tdm->_chip_data.soc_pkg.soc_vars.ap.start_port = (idx*(AP_NUM_PHY_PORTS/AP_NUM_QUAD));
		_tdm->_chip_data.soc_pkg.soc_vars.ap.stop_port = ((idx+1)*(AP_NUM_PHY_PORTS/AP_NUM_QUAD));
		{
			int i, j, first_wc=0, op_flags_str[3], relink, is_state_changed;
			struct ap_ll_node llist;
			ap_pgw_pntrs_t ap_pntr_pkg;
			ap_pgw_scheduler_vars_t ap_vars_pkg;
			int *pgw_tdm_tbl, *ovs_tdm_tbl, *ovs_spacing = 0;
      int cl_port[2] = {130,130};
      enum port_speed_e cl_speed[2], cxx_speed = 0;
			
			op_flags_str[0]=0; 
			op_flags_str[1]=0;
			op_flags_str[2]=0;
			llist.port=0;
			llist.next=NULL;
			ap_pntr_pkg.pgw_tdm_idx=0;
			ap_pntr_pkg.ovs_tdm_idx=0;
			ap_pntr_pkg.tdm_stk_idx=0;
			for (i=0; i<AP_OS_LLS_GRP_LEN; i++){
				ap_vars_pkg.swap_array[i] = 0;
			}
			
			/* PGW calendars */
			switch(_tdm->_chip_data.soc_pkg.soc_vars.ap.start_port) {
				case 0:
					pgw_tdm_tbl = _ap_chip->tdm_pgw.pgw_tdm_tbl_x0;
					ovs_tdm_tbl = _ap_chip->tdm_pgw.ovs_tdm_tbl_x0;
					ovs_spacing = _ap_chip->tdm_pgw.ovs_spacing_x0;
					break;
				case 36:
					pgw_tdm_tbl = _ap_chip->tdm_pgw.pgw_tdm_tbl_x1;
					ovs_tdm_tbl = _ap_chip->tdm_pgw.ovs_tdm_tbl_x1;
					ovs_spacing = _ap_chip->tdm_pgw.ovs_spacing_x1;
					break;		
			}

			/* wrap core index */
			switch(_tdm->_chip_data.soc_pkg.soc_vars.ap.pgw_num) {
				case 0: first_wc = 0; break;
				case 1: first_wc = 9; break;
			}
			/* port number range */
			ap_vars_pkg.first_port=_tdm->_chip_data.soc_pkg.soc_vars.ap.start_port;
			ap_vars_pkg.last_port =_tdm->_chip_data.soc_pkg.soc_vars.ap.stop_port;
			/* linked-list, and PGW oversub calendar */
      /* Round robin scheduler with 5G per slot */
			for (j=0; j<8; j++) { 
        if(j>3)  op_flags_str[1]=1; 
        if(j==7) op_flags_str[2]=1; 
				ap_vars_pkg.subport=j%4;
				ap_pntr_pkg.cur_idx=first_wc;
				ap_vars_pkg.cur_idx_max=(first_wc+9);
				tdm_ap_lls_scheduler(&llist, _ap_chip, &ap_pntr_pkg, &ap_vars_pkg, &pgw_tdm_tbl, &ovs_tdm_tbl, op_flags_str, &ap_fb_num_ovs, &ap_fb_num_lr);
			}

            switch(_tdm->_chip_data.soc_pkg.clk_freq){
                case 435:
                    if (ap_vars_pkg.first_port==0 &&
                        ap_vars_pkg.last_port==36){
                        pgw_ll_len_max = 26;
                    }
                    else {
                        pgw_ll_len_max = 24;
                    }
                    break;
                case 510:
                    pgw_ll_len_max = 30;
                    break;
                case 575:
                    pgw_ll_len_max = 34;
                    break;
                case 793:
                /* Added 795 to 798 clk for FB config*/
                case 794:
                case 795:
                case 796:
                case 797:
                case 798:
                case 840:
                    pgw_ll_len_max = 48;
                    break;
                case 933:
                    pgw_ll_len_max = 56;
                    break;
                default:
                    pgw_ll_len_max = 64;
                    TDM_ERROR1("TDM: unrecognized frequency %d in PGW\n", 
                               _tdm->_chip_data.soc_pkg.clk_freq);
                    break;
            }
            pgw_ll_len = tdm_ap_ll_len(&llist);
            if (pgw_ll_len>pgw_ll_len_max){
                TDM_PRINT2("TDM: Adjust PGW linked list, len %d, limit %d\n",
                           pgw_ll_len, pgw_ll_len_max);
                n = pgw_ll_len - pgw_ll_len_max;
                for (i=(pgw_ll_len-1); i>0; i--){
                    if (tdm_ap_ll_get(&llist,i)==AP_OVSB_TOKEN){
                        tdm_ap_ll_delete(&llist, i);
                        n--;
                        TDM_PRINT2("%s, index %d\n",
                                  "remove OVSB slot from PGW linked list",
                                  i);
                    }
                    if (n<=0){
                        break;
                    }
                }
                pgw_ll_len = tdm_ap_ll_len(&llist);
                if (pgw_ll_len>pgw_ll_len_max){
                    TDM_WARN5("TDM: %s, pipe %d, length %d, limit %d, max %d\n",
                               "PGW calendar length may overflow",
                               idx,
                               pgw_ll_len,
                               pgw_ll_len_max,
                               48);
                }
            }
    /* check for 25G ports in the llist */
		/* falcon port  numbers */
			switch(_tdm->_chip_data.soc_pkg.soc_vars.ap.pgw_num) {
				case 0:
          cl_port[0]    = 29;
          cl_port[1]    = 33;
          cxx_speed     =  _tdm->_chip_data.soc_pkg.speed[17];
          cl_speed[0]   =  _tdm->_chip_data.soc_pkg.speed[29];
          cl_speed[1]   =  _tdm->_chip_data.soc_pkg.speed[33];
          break;
				case 1: 
          cl_port[0]    = 65;
          cl_port[1]    = 69;
          cxx_speed     =  _tdm->_chip_data.soc_pkg.speed[53];
          cl_speed[0]   =  _tdm->_chip_data.soc_pkg.speed[65];
          cl_speed[1]   =  _tdm->_chip_data.soc_pkg.speed[69];
          break;          
			}
      /* to be called only for 25G lr ports*/
      tdm_ap_ll_retrace_cl(&llist,_ap_chip,cl_port,cl_speed);
      tdm_ap_ll_retrace_25(&llist,_ap_chip->pmap,cl_port,cl_speed);

			ap_pntr_pkg.pgw_tdm_idx=tdm_ap_ll_len(&llist);
			/* same-port-min-spacing */
			relink=BOOL_FALSE;
			for (i=1; i<tdm_ap_ll_len(&llist); i++) {
				AP_TOKEN_CHECK(tdm_ap_ll_get(&llist,i)) {
					if ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(&llist,i),_ap_chip->pmap)==tdm_ap_scan_which_tsc(tdm_ap_ll_get(&llist,(i-1)),_ap_chip->pmap) )  {
						relink=BOOL_TRUE;
						break;
					}
				}
			}
			if (relink) {
				TDM_PRINT0("TDM: Retracing calendar\n");
				tdm_ap_ll_retrace(&llist,_ap_chip->pmap);
				tdm_ap_ll_print(&llist);
			}

			/* same-port-min-spacing */
			relink=BOOL_FALSE;
			for (i=1; i<tdm_ap_ll_len(&llist); i++) {
				AP_TOKEN_CHECK(tdm_ap_ll_get(&llist,i)) {
					if ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(&llist,i),_ap_chip->pmap)==tdm_ap_scan_which_tsc(tdm_ap_ll_get(&llist,(i-1)),_ap_chip->pmap) )  {
						relink=BOOL_TRUE;
						break;
					}
				}
			}
			if ( (tdm_ap_ll_count(&llist,1)>0 && tdm_ap_ll_count(&llist,AP_OVSB_TOKEN)>1) || (tdm_ap_ll_single_100(&llist)) || (relink) ) {
				TDM_PRINT0("TDM: Reweaving calendar\n");
				tdm_ap_ll_weave(&llist,_ap_chip->pmap,AP_OVSB_TOKEN);
				tdm_ap_ll_print(&llist);
			}

      /* check for port configs if ports are still back to back*/
			for (i=1; i<tdm_ap_ll_len(&llist); i++) {
				AP_TOKEN_CHECK(tdm_ap_ll_get(&llist,i)) {
					if ( tdm_ap_scan_which_tsc(tdm_ap_ll_get(&llist,i),_ap_chip->pmap)==tdm_ap_scan_which_tsc(tdm_ap_ll_get(&llist,(i-1)),_ap_chip->pmap) )  {
						TDM_ERROR1("SPACING VOILATION:Found back to back ports in PGW TDM TBL: %0d\n",tdm_ap_ll_get(&llist,i));
                        TDM_FREE(_ap_chip);
            return (TDM_EXEC_CORE_SIZE+1);  
					}
				}
			}
			/* PGW main calendar */
     /* Begin Processing of FB  config where only one HG port in 40G speed. Need to space 40G port evenly */
     len=tdm_ap_ll_len(&llist);
     /* Check if PGW one has only one OS and other LR ports for FB  config 
        First remove all 40G port and insert again taking average space into consideration */
     if( (ap_fb_num_lr == 1) && (ap_fb_num_ovs > 0) && (first_wc == 0) && (_tdm->_chip_data.soc_pkg.speed[first_wc+25] == SPEED_40G) ) {
       TDM_PRINT0("TDM: FB  config smoothing 40G LR HIGIG  port in PGW Calendar\n");
       token = 25; /*40G port number */
       tdm_ap_ll_strip(&llist,&pool,token);
       avg = len/8;
       for (i = 0; i< 8; i++){
         tdm_ap_ll_insert(&llist,token, start_index+(i*avg)); 
       }
     }
    /* End processing of FB  config */
     tdm_ap_ll_print(&llist);
			tdm_ap_ll_deref(&llist,&pgw_tdm_tbl,AP_LR_LLS_LEN);
      if((_tdm->_chip_data.soc_pkg.state[cl_port[0]]==PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[cl_port[1]]==PORT_STATE__LINERATE) && 
         (_tdm->_chip_data.soc_pkg.clk_freq>=793 && _tdm->_chip_data.soc_pkg.clk_freq<=798) &&
         cxx_speed != SPEED_100G)
       tdm_ap_reconfig_pgw_tbl(pgw_tdm_tbl,cl_port,cl_speed);
			TDM_SML_BAR
			tdm_ap_print_tbl(pgw_tdm_tbl,AP_LR_LLS_LEN,"PGW Main Calendar",idx);

			/* PGW oversub spacer calendar */
      if (ovs_tdm_tbl[0]!=AP_NUM_EXT_PORTS) {
        tdm_ap_mv_fb_ovs_gen(_ap_chip, ovs_tdm_tbl, ovs_spacing, idx);
				
				is_state_changed = BOOL_FALSE;
				for (i=0; i<AP_OS_LLS_GRP_LEN; i++) {
					if ( (ovs_tdm_tbl[i]!=AP_NUM_EXT_PORTS) && 
						 (_tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]==PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]==PORT_STATE__LINERATE_HG) ){
						switch(_tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]){
							case PORT_STATE__LINERATE: 
								_tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]=PORT_STATE__OVERSUB;
								is_state_changed = BOOL_TRUE;
								break;
							case PORT_STATE__LINERATE_HG:
								_tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]=PORT_STATE__OVERSUB_HG;
								is_state_changed = BOOL_TRUE;
								break;
							default:
								break;
						}
						TDM_PRINT1("TDM: Port [%3d] state changes from LINERATE to OVERSUB\n", ovs_tdm_tbl[i]);
					}
				}
				if(is_state_changed==BOOL_TRUE) {tdm_print_stat( _tdm );}
			}
	    tdm_ap_ll_free(&llist);	
    }
	}
	
	/* Bind to TDM class object */
	TDM_COPY(_tdm->_chip_data.cal_0.cal_main,_ap_chip->tdm_pgw.pgw_tdm_tbl_x0,sizeof(int)*AP_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_0.cal_grp[0],_ap_chip->tdm_pgw.ovs_tdm_tbl_x0,sizeof(int)*AP_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_0.cal_grp[1],_ap_chip->tdm_pgw.ovs_spacing_x0,sizeof(int)*AP_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_main,_ap_chip->tdm_pgw.pgw_tdm_tbl_x1,sizeof(int)*AP_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_grp[0],_ap_chip->tdm_pgw.ovs_tdm_tbl_x1,sizeof(int)*AP_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_grp[1],_ap_chip->tdm_pgw.ovs_spacing_x1,sizeof(int)*AP_OS_LLS_GRP_LEN);
	
	/* Realign port state array to old specification */	
	for (idx=0; idx<((_tdm->_chip_data.soc_pkg.num_ext_ports)-57); idx++) {
		_tdm->_chip_data.soc_pkg.state[idx] = _tdm->_chip_data.soc_pkg.state[idx+1];
	}
	
	_tdm->_core_data.vars_pkg.pipe=AP_PIPE_X_ID;
	TDM_FREE(_ap_chip);
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__EGRESS_WRAP]( _tdm ) );
}

void
tdm_ap_tdm_print(int *tdm_tbl, int length) 
{
        int i;
        for (i=0; i<length; i++) {      
                if (i%10==0) TDM_PRINT2("\nTDM-TBL %3d : %3d ", i, tdm_tbl[i]);
                else TDM_PRINT1(" %3d ", tdm_tbl[i]);
        }
        TDM_PRINT0("\n");
}   

/**
@name: tdm_ap_mv_fb_ovs_gen
@param:
Api to replace tdm_ap_ovs_spacer.
this api is helpful in generating tdm table based on the template provided by Brendan - SDK-113411
 */
void
tdm_ap_mv_fb_ovs_gen(tdm_ap_chip_legacy_t *_ap_chip,int *ovs_tdm_tbl, int *ovs_spacing, int pgw_num)
{
  ap_pgw_os_tdm_sel_e ap_pgw_os_tdm_sel;
  unsigned int mv_pgw0_ovs_tbl_template[7][40] = {
                                                  {1,5,9,13,17,21,25,29,33,AP_OVSB_TOKEN,1,5,9,13,17,21,25,29,33,AP_OVSB_TOKEN,1,5,9,13,17,21,25,29,33,AP_OVSB_TOKEN,1,5,9,13,17,21,25,29,33,AP_OVSB_TOKEN},             /* MV 720G and below */
					          {29,33,1,5,9,29,33,25,17,21,29,33,1,5,9,29,33,25,17,21,29,33,1,5,9,29,33,25,17,21,29,33,1,5,9,29,33,25,17,21},             /* MV 880G-- OBSELETE. Wil use MV720 template for M88 */
					          {29,33,1,29,33,5,29,33,21,25,29,33,1,29,33,5,29,33,21,25,29,33,1,29,33,5,29,33,21,25,29,33,1,29,33,5,29,33,21,25},         /* FB 56565 */
					          {29,33,21,29,33,25,29,33,29,33,21,29,33,25,29,33,29,33,21,29,33,25,29,33,29,33,21,29,33,25,29,33,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN}, /* FB 56567 */
					          {29,33,1,29,33,5,29,33,21,25,29,33,1,29,33,5,29,33,21,25,29,33,1,29,33,5,29,33,21,25,29,33,1,29,33,5,29,33,21,25},         /* FB 56568 - 1 */
					          {1,5,9,13,17,21,1,5,9,13,17,21,1,5,9,13,17,21,1,5,9,13,17,21,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN},              /* FB 56568 - 2 */
                              {1,5,9,13,17,21,29,3,7,11,15,19,23,2,6,10,14,18,22,30,4,8,12,16,20,24,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN}              /* FB 56568 - 3 */

                                                  };  /* MV 880G */ 
  unsigned int mv_pgw1_ovs_tbl_template[7][40] = {
                                                  {37,41,45,49,53,57,61,65,69,AP_OVSB_TOKEN,37,41,45,49,53,57,61,65,69,AP_OVSB_TOKEN,37,41,45,49,53,57,61,65,69,AP_OVSB_TOKEN,37,41,45,49,53,57,61,65,69,AP_OVSB_TOKEN}, /* MV 720G and below */
                                                  {65,69,37,41,45,65,69,61,53,57,65,69,37,41,45,65,69,61,53,57,65,69,37,41,45,65,69,61,53,57,65,69,37,41,45,65,69,61,53,57}, /* MV 880G-- OBSELETE. Wil use MV720 template for M88 */
                                                  {65,69,49,65,69,53,65,69,57,61,65,69,49,65,69,53,65,69,57,61,65,69,49,65,69,53,65,69,57,61,65,69,49,65,69,53,65,69,57,61}, /* FB 56565 */
                                                  {65,69,65,69,65,69,65,69,65,69,65,69,65,69,65,69,65,69,65,69,65,69,65,69,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN}, /* FB 56567 */
					          {65,69,49,65,69,53,65,69,57,61,65,69,49,65,69,53,65,69,57,61,65,69,49,65,69,53,65,69,57,61,65,69,49,65,69,53,65,69,57,61}, /* FB 56568 - 1 */
					          {41,45,49,53,57,61,41,45,49,53,57,61,41,45,49,53,57,61,41,45,49,53,57,61,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN,AP_OVSB_TOKEN},  /* FB 56568 - 2 */
                              {65,69,41,67,45,49,66,53,57,68,61,71,43,65,47,51,55,67,59,63,66,70,42,68,46,50,65,54,58,67,62,72,44,66,48,52,56,68,60,64} /* FB 56568 - 3 */

                                                 };

  

  tdm_ap_check_mv_fb_config(&ap_pgw_os_tdm_sel, _ap_chip);
  switch(pgw_num){
    case 0:
      tdm_ap_mv_fb_ovs_tbl(_ap_chip, ovs_tdm_tbl, mv_pgw0_ovs_tbl_template[ap_pgw_os_tdm_sel], &ap_pgw_os_tdm_sel, pgw_num);
      tdm_ap_mv_fb_ovs_tbl_spacing(ovs_tdm_tbl, ovs_spacing);
      tdm_ap_tdm_print((int *)mv_pgw0_ovs_tbl_template[ap_pgw_os_tdm_sel], 40);
      tdm_ap_tdm_print(ovs_tdm_tbl, 40);
      tdm_ap_tdm_print(ovs_spacing, 40);
      break;
    case 1:
      tdm_ap_mv_fb_ovs_tbl(_ap_chip, ovs_tdm_tbl, mv_pgw1_ovs_tbl_template[ap_pgw_os_tdm_sel], &ap_pgw_os_tdm_sel, pgw_num);
      tdm_ap_mv_fb_ovs_tbl_spacing(ovs_tdm_tbl, ovs_spacing);
      tdm_ap_tdm_print((int *)mv_pgw1_ovs_tbl_template[ap_pgw_os_tdm_sel], 40);
      tdm_ap_tdm_print(ovs_tdm_tbl, 40);
      tdm_ap_tdm_print(ovs_spacing, 40);
      break;
  }
}

void tdm_ap_mv_fb_ovs_tbl(tdm_ap_chip_legacy_t *ap_chip,int *ovs_tdm_tbl, unsigned int *pgw0_ovs_tbl_template,
                           ap_pgw_os_tdm_sel_e *ap_pgw_os_tdm_sel, int pgw_num)
{
  int i, j, token;

  for(i=0; i<40; i++) {
    ovs_tdm_tbl[i] = pgw0_ovs_tbl_template[i];
  }
  for(i=1; i<= AP_NUM_PHY_PORTS; i=i+4) {
    switch((*ap_chip).tdm_globals.speed[i]){
      case SPEED_100G: /*As template is having all Single mode port entries and 100/40G ports in single modes so skip processing  */
        for (j=0, token=0; j<40;j++) {
	       if ((ovs_tdm_tbl[j] == i) || (ovs_tdm_tbl[j] == i+1) || (ovs_tdm_tbl[j] == i+2) || (ovs_tdm_tbl[j] == i+3)) {/*config-8 support - to handle template with 12 slots for 100G port */ 
	          token++;
              ovs_tdm_tbl[j]=i;
	          if (token>10) ovs_tdm_tbl[j]= AP_OVSB_TOKEN; /* To handle FB5 config. Inline-edit template has 12 slots allocated for 100G ports in Fb5 config. This would eatup bandwidth of other low-speed ports. To avoid the BW loss on low speed ports, 100G port should get only 10 slots. */
	       }
        }
	break;
      case SPEED_40G: 
	if (i == 29 || i == 33 || i== 65 || i==69) { /* Only for Falcon ports enter this processing. Reduce the tokens from 8 to 4*/
            for (j=0, token=0; j<40;j++) {
	        if ((ovs_tdm_tbl[j] == i) || (ovs_tdm_tbl[j] == i+1) || (ovs_tdm_tbl[j] == i+2) || (ovs_tdm_tbl[j] == i+3)) {/* config-8 support - to handle template with 12 slots for 100G port */ 
	          token++;
              if (*ap_pgw_os_tdm_sel == AP_PGW_OS_FB_568_3) {
                 ovs_tdm_tbl[j] = i; }
	          if (token>4) ovs_tdm_tbl[j]= AP_OVSB_TOKEN; /* To handle FB5 config: Falcon ports can be configured in 40G mode*/
	          else if (token%2) ovs_tdm_tbl[j]= ((*ap_chip).tdm_globals.speed[i+2] == SPEED_40G) ? i+2 : i;
	       }
           }
/* M88 to use M720 template	          } */
	}
	break;
      case SPEED_50G:
        for (j=0, token=0; j<40;j++) {
	   if (ovs_tdm_tbl[j] == i) { 
	      token++;
	      if (token>10) ovs_tdm_tbl[j]= AP_OVSB_TOKEN; /* To handle FB5 config. Inline-edit template has 12 slots allocated for 100G/dual-50G ports in Fb5 config. This would eatup bandwidth of other low-speed ports. To avoid the BW loss on low speed ports, 100G port should get only 10 slots. */
	      else if (token%2) ovs_tdm_tbl[j]=i;
	      else ovs_tdm_tbl[j]=((*ap_chip).tdm_globals.speed[i+2] == SPEED_50G) ? i+2: 
					((*ap_chip).tdm_globals.speed[i+1] == SPEED_25G) ? i+1 :
					((*ap_chip).tdm_globals.speed[i+3] == SPEED_25G) ? i+3 : AP_OVSB_TOKEN /*IDLE Port*/; 
	   }
        }
      break;
      case SPEED_25G:
	if (i == 29 || i == 33 || i== 65 || i== 66 || i== 67 || i== 68 || i==69) { /* Only for Falcon ports enter this processing Fix for F4.10 */ 
           for (j=0, token=0; j<40;j++) {

          if (ovs_tdm_tbl[j] >= i && ovs_tdm_tbl[j] <= (i+3) ) { /* 25G ports would require 3 slots in calendar for F4.10 */
	         token++;
	         switch (token) {
		   case 1:
		   case 5: 
		   case 9: /* 25 G */
			ovs_tdm_tbl[j] = i;
			break;
		   case 2:
		   case 6: 
		   case 10: /* Check for 50G first, then check for 25G */
			ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+2] == SPEED_50G) ?  i+2 : 
						 ((*ap_chip).tdm_globals.speed[i+2] == SPEED_25G) ? i+2 : AP_OVSB_TOKEN /*IDLE port*/;
			break;
		   case 3:
		   case 7: 
		   case 11: /* Say PM port 1, 2, 3, 4 are in trimode. then the mapping should be 1,3,2,3 considering port 3 as 50G port */
			ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+1] == SPEED_25G) ?  i+1 : AP_OVSB_TOKEN /*Idle Port*/;  
			break;
		   case 4: 
		   case 8: 
		   case 12: /* Check for 50G first, then check for 25G */
			ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+2] == SPEED_50G) ?  i+2 : 
						 ((*ap_chip).tdm_globals.speed[i+3] == SPEED_25G) ? i+3 : AP_OVSB_TOKEN /*IDLE port*/;
			break;
		 }
	      }
           }
        } else {
	/* For Eagle - insert one IDLE slot out of 4 valid slots */
	/* No spacing rules applicable in OS calendar. */
          for (j=0, token=0; j<40;j++) {
	     if (ovs_tdm_tbl[j] == i) { 
               ovs_tdm_tbl[j] = AP_OVSB_TOKEN;
               break;
	     }
          }
	}
      break;
      case SPEED_20G:
        for (j=0, token=0; j<40;j++) {
	   if (ovs_tdm_tbl[j] == i) { 
	      token++;
	      if ((i == 29 || i == 33 || i== 65 || i==69) && token>4) ovs_tdm_tbl[j] = AP_OVSB_TOKEN; /* Falcon ports when configured as 20G dont require more than 4 tokens (2x20G) */
	      else if (token%2) ovs_tdm_tbl[j]=i;
	      else ovs_tdm_tbl[j]=((*ap_chip).tdm_globals.speed[i+2] == SPEED_20G) ? i+2: 
					((*ap_chip).tdm_globals.speed[i+1] == SPEED_10G) ? i+1 :
					((*ap_chip).tdm_globals.speed[i+3] == SPEED_10G) ? i+3 : AP_OVSB_TOKEN /*IDLE Port*/; 
	   }
        }
      break;
      case SPEED_10G:
      case SPEED_1G:
      case SPEED_2p5G:
 	if (i == 29 || i == 33 || i== 65 || i==69) { /* Only for Falcon ports enter this processing Fix for F4.10 */ 
            for (j=0, token=0; j<40;j++) {
 	      if (ovs_tdm_tbl[j] >= i && ovs_tdm_tbl[j] <= (i+3) ) {
                     token ++;
                     switch (token) {
                             case  2:
 			                    ovs_tdm_tbl[j] = (((*ap_chip).tdm_globals.speed[i+2] == SPEED_50G) || ((*ap_chip).tdm_globals.speed[i+2] == SPEED_20G)) ? i+2 :  
                         					 ((*ap_chip).tdm_globals.speed[i+2] == SPEED_10G || 
                                              (*ap_chip).tdm_globals.speed[i+2] == SPEED_1G || 
                                              (*ap_chip).tdm_globals.speed[i+2] == SPEED_2p5G) ? i+2 : 
                                             (*ap_pgw_os_tdm_sel == AP_PGW_OS_FB_568_3) && (i>=29 && i<=32) ? ovs_tdm_tbl[j] : AP_OVSB_TOKEN /*IDLE port*/;
                             break;
                             case 3: /* Say PM port 1, 2, 3, 4 are in trimode. then the mapping should be 1,3,2,3 considering port 3 as 50G port */
                         		ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+1] == SPEED_10G || 
                                                                       (*ap_chip).tdm_globals.speed[i+1] == SPEED_1G || 
                                                                       (*ap_chip).tdm_globals.speed[i+1] == SPEED_2p5G) ?  i+1 : AP_OVSB_TOKEN /*Idle Port*/;  
                         	 break;
                             case 4: /* Check for 50G first, then check for 25G */
                        		ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+2] == SPEED_20G) ?  i+2 : 
                        					((*ap_chip).tdm_globals.speed[i+3] == SPEED_10G ||  
                                                                      (*ap_chip).tdm_globals.speed[i+3] == SPEED_1G ||
                                                                      (*ap_chip).tdm_globals.speed[i+3] == SPEED_2p5G) ? i+3 : AP_OVSB_TOKEN /*IDLE port*/;
                             break;
                             case  5:
                             case  9:
 			                    ovs_tdm_tbl[j] = AP_OVSB_TOKEN;
                             break;
                             case 6:
                             case 10:
 			                    ovs_tdm_tbl[j] = (((*ap_chip).tdm_globals.speed[i+2] == SPEED_50G) || ((*ap_chip).tdm_globals.speed[i+2] == SPEED_25G)) ? i+2 :   AP_OVSB_TOKEN;
                             break;
                             case 7:
                             case 11:
 			                    ovs_tdm_tbl[j] = ((*ap_chip).tdm_globals.speed[i+2] == SPEED_25G) ? i+2 :   AP_OVSB_TOKEN;
                             break;
                             case 8:
                             case 12:
 			                    ovs_tdm_tbl[j] = ((*ap_chip).tdm_globals.speed[i+2] == SPEED_50G) ? i+2 :  ((*ap_chip).tdm_globals.speed[i+3] == SPEED_25G) ? i+3 :   AP_OVSB_TOKEN;
                             break;
                     }
           }
            } 
     } else {
       for (j=0, token=0; j<40;j++) {
      if (ovs_tdm_tbl[j] == i) { 
            if((*ap_chip).tdm_globals.port_rates_array[i] == PORT_STATE__LINERATE){
              ovs_tdm_tbl[j] = AP_OVSB_TOKEN;
              continue;
            }
         token++;
         switch (token) {
	   case 1: /* 10G */
		ovs_tdm_tbl[j] = i;
		break;
	   case 2: /* Check for 20G first, then check for 10G */
		ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+2] == SPEED_20G) ?  i+2 : 
					 ((*ap_chip).tdm_globals.speed[i+2] == SPEED_10G || 
                                              (*ap_chip).tdm_globals.speed[i+2] == SPEED_1G || 
                                              (*ap_chip).tdm_globals.speed[i+2] == SPEED_2p5G) ? i+2 : AP_OVSB_TOKEN /*IDLE port*/;
		break;
	   case 3: /* Say PM port 1, 2, 3, 4 are in trimode. then the mapping should be 1,3,2,3 considering port 3 as 50G port */
		ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+1] == SPEED_10G || 
                                              (*ap_chip).tdm_globals.speed[i+1] == SPEED_1G || 
                                              (*ap_chip).tdm_globals.speed[i+1] == SPEED_2p5G) ?  i+1 : AP_OVSB_TOKEN /*Idle Port*/;  
		break;
	   case 4: /* Check for 50G first, then check for 25G */
		ovs_tdm_tbl[j] =  ((*ap_chip).tdm_globals.speed[i+2] == SPEED_20G) ?  i+2 : 
					((*ap_chip).tdm_globals.speed[i+3] == SPEED_10G ||  
                                              (*ap_chip).tdm_globals.speed[i+3] == SPEED_1G ||
                                              (*ap_chip).tdm_globals.speed[i+3] == SPEED_2p5G) ? i+3 : AP_OVSB_TOKEN /*IDLE port*/;
		break;
               default : ovs_tdm_tbl[j] = AP_OVSB_TOKEN; break;
	 }
      }
       }
          }
      break;
      case SPEED_0:
        /*for(j=0 ; j<40; j++) {
          if((*ap_chip).tdm_globals.speed[ovs_tdm_tbl[j]] == SPEED_0) ovs_tdm_tbl[j] = AP_OVSB_TOKEN;
          }*/
        for (j=0; j<40;j++) {
	   if ((ovs_tdm_tbl[j] == i) ||(ovs_tdm_tbl[j] == i+1) || (ovs_tdm_tbl[j] == i+2) || (ovs_tdm_tbl[j] == i+3))  {
             ovs_tdm_tbl[j] = AP_OVSB_TOKEN;
	   }
        }
      break;
      default:
        break;
    }

   }
/* Process below OVS Spacing Table   */
}
void
tdm_ap_check_mv_fb_config(ap_pgw_os_tdm_sel_e *ap_pgw_os_tdm_sel, tdm_ap_chip_legacy_t *ap_chip)
{
  int port_num;
  unsigned int num_lr_ports = 0, num_os_ports = 0;
  for(port_num=0; port_num< AP_NUM_PHY_PORTS; port_num++) {
    if((*ap_chip).tdm_globals.port_rates_array[port_num] == PORT_STATE__OVERSUB   ||
        (*ap_chip).tdm_globals.port_rates_array[port_num] == PORT_STATE__OVERSUB_HG){
      num_os_ports++;
    }else if((*ap_chip).tdm_globals.port_rates_array[port_num] == PORT_STATE__LINERATE   ||
        (*ap_chip).tdm_globals.port_rates_array[port_num] == PORT_STATE__LINERATE_HG){
      num_lr_ports++;
    }
  }
  switch ((*ap_chip).tdm_globals.clk_freq) {
    case 793:
    case 840:/*  with new Inline edit template from Brendan, M88 would use the same template as M720*/
        *ap_pgw_os_tdm_sel = AP_PGW_OS_MV_720G;
        break;
    case 794:
        *ap_pgw_os_tdm_sel = AP_PGW_OS_FB_565;
        break;
    case 795:
        *ap_pgw_os_tdm_sel = AP_PGW_OS_FB_567;
        break;
    case 796:
        *ap_pgw_os_tdm_sel = AP_PGW_OS_FB_568_1;
        break;
    case 797:
        *ap_pgw_os_tdm_sel = AP_PGW_OS_FB_568_2;
        break;
    case 798:
        *ap_pgw_os_tdm_sel = AP_PGW_OS_FB_568_3;
        break;
  }

}
 void tdm_ap_mv_fb_ovs_tbl_spacing(int *ovs_tdm_tbl, int *ovs_spacing) {
  int i, j, wrap_arnd; 
  for (i=0; i<40; i++ ) {
    ovs_spacing[i] =0;
    for (j=i+1, wrap_arnd=0; j<=40 && wrap_arnd<2; j++ ) { /* j goes till 40 - to account for wrap-around condition. this is to take care of port slots allocated at the end of calendar */
      if (j == 40) {
        j = -1;
        wrap_arnd++;
        continue;
      } 
      ovs_spacing[i]++;
      if (ovs_tdm_tbl[i] == ovs_tdm_tbl[j]) break;
     }
   }
   
}

/**
@name: tdm_ap_pmap_transcription
@param:

For Trident2+ BCM56850
Transcription algorithm for generating port module mapping

	40G 	- xxxx
      	20G 	- xx
	      	    xx
      	10G	- x
		   x
		    x
		     x
     	1X0G 	- xxxx_xxxx_xxxx
 */
int
tdm_ap_pmap_transcription( tdm_mod_t *_tdm )
{
	int i, j, last_port=AP_NUM_EXT_PORTS, tsc_active;
  int phy_lo, phy_hi, pm_lane_num, bcm_config_check=PASS;
	
	/* Regular ports */
	for (i=1; i<=AP_NUM_PHY_PORTS; i+=AP_NUM_PM_LNS) {
		tsc_active=BOOL_FALSE;
		for (j=0; j<AP_NUM_PM_LNS; j++) {
			if ( _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__LINERATE    || _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__OVERSUB    ||
			     _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__LINERATE_HG || _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__OVERSUB_HG ){
				tsc_active=BOOL_TRUE;
				break;
			}
		}
		if(tsc_active==BOOL_TRUE){
			if ( (i ==17 || i==53)  && _tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G ) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/AP_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/AP_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/AP_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/AP_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/AP_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/AP_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/AP_NUM_PM_LNS][2] = AP_NUM_EXT_PORTS;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/AP_NUM_PM_LNS][3] = AP_NUM_EXT_PORTS;
				i+=8;
				}
			else {
		  	if ( _tdm->_chip_data.soc_pkg.speed[i]>SPEED_0 || _tdm->_chip_data.soc_pkg.state[i]==PORT_STATE__DISABLED ) {
		  		for (j=0; j<AP_NUM_PM_LNS; j++) {
		  			switch (_tdm->_chip_data.soc_pkg.state[i+j]) {
		  				case PORT_STATE__LINERATE:
		  				case PORT_STATE__OVERSUB:
		  				case PORT_STATE__LINERATE_HG:
		  				case PORT_STATE__OVERSUB_HG:
		  					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][j]=(i+j);
		  					last_port=(i+j);
		  					break;
		  				case PORT_STATE__COMBINE:
		  					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][j]=last_port;
		  					break;
		  				default:
		  					_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][j]=AP_NUM_EXT_PORTS;
		  					break;
		  			}
		  		}
		  		if ( _tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3] && _tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0 &&_tdm->_chip_data.soc_pkg.speed[i]>=SPEED_40G ) {
		  			_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2];
		  			_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][0];
		  		}
		  		else if ( _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] && _tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0 &&_tdm->_chip_data.soc_pkg.speed[i+2]>=SPEED_40G )  {
		  			_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1];
		  			_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][3];
		  		}
			/* dual mode x_x_ */
			    else if (_tdm->_chip_data.soc_pkg.speed[i]!=_tdm->_chip_data.soc_pkg.speed[i+1]&&_tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i]>=SPEED_40G) {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][3];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/AP_NUM_PM_LNS][0];
			    }
		  	}
		  }
	  }
  }
	tdm_print_stat( _tdm );

    /* Check if invalid port config exists */
    phy_lo     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    phy_hi     = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    pm_lane_num= _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    for (i=phy_lo; i<=(phy_hi-pm_lane_num); i+=pm_lane_num) {
        if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_0){
            for (j=1; j<pm_lane_num; j++) {
                if (_tdm->_chip_data.soc_pkg.speed[i+j]>SPEED_0){
                    bcm_config_check = FAIL;
                    break;
                }
            }
        }
        if (bcm_config_check==FAIL){
            TDM_ERROR8("Invalid port configuration, port [%3d, %3d, %3d, %3d], speed [%3dG, %3dG, %3dG, %3dG]\n",
                i, i+1, i+2, i+3,
                _tdm->_chip_data.soc_pkg.speed[i]/1000,
                _tdm->_chip_data.soc_pkg.speed[i+1]/1000,
                _tdm->_chip_data.soc_pkg.speed[i+2]/1000,
                _tdm->_chip_data.soc_pkg.speed[i+3]/1000);
            return FAIL;
        }
    }
    for (i=1; i<=AP_NUM_PHY_PORTS; i+=AP_NUM_PM_LNS) {
      if(i==AP_CL1_PORT || i==AP_CL2_PORT || i==AP_CL4_PORT || i==AP_CL5_PORT ){
				if ( ( _tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3] && _tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0 && _tdm->_chip_data.soc_pkg.speed[i]>=SPEED_40G ) || ( _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] && _tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0 && _tdm->_chip_data.soc_pkg.speed[i+2]>=SPEED_40G )) {
            TDM_ERROR8("tri port configuration(25GE+50GE)on Falcon not supported in APACHE, port [%3d, %3d, %3d, %3d], speed [%3dG, %3dG, %3dG, %3dG]\n", i, i+1, i+2, i+3, _tdm->_chip_data.soc_pkg.speed[i]/1000, _tdm->_chip_data.soc_pkg.speed[i+1]/1000, _tdm->_chip_data.soc_pkg.speed[i+2]/1000, _tdm->_chip_data.soc_pkg.speed[i+3]/1000);
          return FAIL;
				}
      }
    }
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_chip_ap_init
@param:
 */
int
tdm_ap_init( tdm_mod_t *_tdm )
{
	int index;
	
	_tdm->_chip_data.soc_pkg.pmap_num_modules = AP_NUM_PM_MOD;
	_tdm->_chip_data.soc_pkg.pmap_num_lanes = AP_NUM_PM_LNS;
	_tdm->_chip_data.soc_pkg.pm_num_phy_modules = AP_NUM_PHY_PM;
	
	_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = AP_OVSB_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl1_token = AP_IDL1_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl2_token = AP_IDL2_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.ancl_token = AP_ANCL_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = AP_NUM_PHY_PORTS;
	
	_tdm->_chip_data.cal_0.cal_len = AP_LR_LLS_LEN;
	_tdm->_chip_data.cal_0.grp_num = AP_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_0.grp_len = AP_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_1.cal_len = AP_LR_LLS_LEN;
	_tdm->_chip_data.cal_1.grp_num = AP_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_1.grp_len = AP_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_2.cal_len = AP_LR_VBS_LEN;
	_tdm->_chip_data.cal_2.grp_num = AP_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_2.grp_len = AP_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_3.cal_len = AP_LR_IARB_STATIC_LEN;
	_tdm->_chip_data.cal_3.grp_num = 0;
	_tdm->_chip_data.cal_3.grp_len = 0;

	
	/* management port state */
	/*if (_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_0] > SPEED_0   && 
		_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_0]<= SPEED_10G &&
		_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_1]==_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_2] && 
		_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_2]==_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_3] ){
		if (_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_0]==SPEED_10G && _tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_1]==SPEED_0){
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_0] = PORT_STATE__MANAGEMENT;
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_1] = PORT_STATE__DISABLED;
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_2] = PORT_STATE__DISABLED;
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_3] = PORT_STATE__DISABLED;
		}
		else if (_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_1]==_tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_0] &&
		         _tdm->_chip_data.soc_pkg.speed[AP_MGMT_PORT_1]<SPEED_10G){
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_0] = PORT_STATE__MANAGEMENT;
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_1] = PORT_STATE__MANAGEMENT;
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_2] = PORT_STATE__MANAGEMENT;
			_tdm->_chip_data.soc_pkg.state[AP_MGMT_PORT_3] = PORT_STATE__MANAGEMENT;
		}
	}*/
	/* encap */
	for (index=0; index<AP_NUM_PM_MOD; index++) {
		_tdm->_chip_data.soc_pkg.soc_vars.ap.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*4]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index]==PORT_STATE__OVERSUB_HG)?(PM_ENCAP__HIGIG2):(PM_ENCAP__ETHRNT);
	}
	/* pmap */
	_tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		_tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	}
	/* PGW x0 calendar group */
	_tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");
	TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
	_tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		_tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
	}
	/* PGW x1 calendar group */
	_tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
	_tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		_tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
	}

	/* MMU x pipe calendar group */
	_tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
	_tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		_tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
	}

	/* IARB static calendar group */
	_tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/**
@name: tdm_ap_post
@param:
 */
int
tdm_ap_post( tdm_mod_t *_tdm )
{
   /* TDM self-check */
    if (_tdm->_chip_data.soc_pkg.soc_vars.ap.tdm_chk_en==BOOL_TRUE){
        return (_tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm));
    }
	return PASS;
}

/**
@name: tdm_ap_free
@param:
 */
int
tdm_ap_free( tdm_mod_t *_tdm )
{
    int index;
    /* Chip: pmap */
    for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
        TDM_FREE(_tdm->_chip_data.soc_pkg.pmap[index]);
    }
    TDM_FREE(_tdm->_chip_data.soc_pkg.pmap);
    /* Chip: PGW x0 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_0.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_0.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_0.cal_grp);
    /* Chip: PGW x1 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_1.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_1.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_1.cal_grp);
    /* Chip: MMU x pipe calendar group */
    TDM_FREE(_tdm->_chip_data.cal_2.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_2.cal_grp);
    /* Chip: IARB static calendar group */
    TDM_FREE(_tdm->_chip_data.cal_3.cal_main);
    /* Core: vmap */
    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        TDM_FREE(_tdm->_core_data.vmap[index]);
    }
    TDM_FREE(_tdm->_core_data.vmap);

    return PASS;
}
