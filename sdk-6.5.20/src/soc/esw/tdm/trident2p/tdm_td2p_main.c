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

/*
#ifdef _TDM_DB_STACK
    size_t stack_size = 0;
#endif */

/**
@name: tdm_td2p_corereq
@param:

Allocate memory for core data execute request to core executive
 */
int
tdm_td2p_corereq( tdm_mod_t *_tdm )
{
	/* X-Pipe: */
	if (_tdm->_core_data.vars_pkg.pipe==TD2P_PIPE_X_ID) {
		_tdm->_core_data.vars_pkg.cal_id=4;
		_tdm->_core_exec[TDM_CORE_EXEC__POST]=&tdm_td2p_vbs_wrapper;
	}
	/* Y-Pipe: */
	else if (_tdm->_core_data.vars_pkg.pipe==TD2P_PIPE_Y_ID) {
		_tdm->_core_data.vars_pkg.cal_id=5;
		_tdm->_core_exec[TDM_CORE_EXEC__POST]=&tdm_core_post;
	}
	/* unrecognised Pipe number */
	else {
		TDM_ERROR1("Unrecgonized PIPE ID %d \n", _tdm->_core_data.vars_pkg.pipe);
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_td2p_vbs_wrapper
@param:

Code wrapper for egress TDM scheduling
 */
int
tdm_td2p_vbs_wrapper( tdm_mod_t *_tdm )
{
	int idx1, idx2, lr_buffer_idx=0, os_buffer_idx=0, 
		pipe, port_exist;
	int *pgw_tdm_tbl_0, *pgw_tdm_tbl_1, *ovs_tdm_tbl_0, *ovs_tdm_tbl_1;
	int param_freq, param_lr_limit, param_ancl_num,
        param_token_empty, param_higig_mgmt;

	/* print stack usage */
	/* TDM_PRINT_STACK_SIZE("tdm_td2p_vbs_wrapper"); */
	
	param_freq        = _tdm->_chip_data.soc_pkg.clk_freq;
	param_lr_limit    = LEN_760MHZ_EN;
	param_ancl_num    = TD2P_ACC_PORT_NUM;
	param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
	param_higig_mgmt  = BOOL_FALSE;

	/* accessory slots */
	switch (param_freq) {
		case 608:
		case 609: 
			param_freq=609; param_ancl_num=TD2P_ACC_PORT_NUM; break;
		case 517:
		case 518: 
			param_freq=518; param_ancl_num=(TD2P_ACC_PORT_NUM-2); break;
		case 416:
		case 415:
			param_freq=415; param_ancl_num=TD2P_ACC_PORT_NUM; break;
		case 760:
			param_freq=760; param_ancl_num=(TD2P_ACC_PORT_NUM-2); break;
		default:
			TDM_ERROR1("Invalid frequency %d MHz\n", param_freq);
			return 0;
	}
	
	switch (_tdm->_core_data.vars_pkg.pipe){
		case TD2P_PIPE_X_ID:   pipe = 0; tdm_td2p_print_quad(_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state, TD2P_NUM_PHY_PORTS, 1,   64); break;
		case TD2P_PIPE_Y_ID:   pipe = 1; tdm_td2p_print_quad(_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state, TD2P_NUM_PHY_PORTS, 65, 128); break;
		default:
			TDM_ERROR1("Invalid pipe ID %d \n", _tdm->_core_data.vars_pkg.pipe);
			return 0;
	}
	
	/* lr_buff and os_buff */
	if (_tdm->_core_data.vars_pkg.pipe>TD2P_PIPE_X_ID) {
		pgw_tdm_tbl_0 = _tdm->_chip_data.cal_2.cal_main;
		pgw_tdm_tbl_1 = _tdm->_chip_data.cal_3.cal_main;
		ovs_tdm_tbl_0 = _tdm->_chip_data.cal_2.cal_grp[0];
		ovs_tdm_tbl_1 = _tdm->_chip_data.cal_3.cal_grp[0];
	}
	else {
		pgw_tdm_tbl_0 = _tdm->_chip_data.cal_0.cal_main;
		pgw_tdm_tbl_1 = _tdm->_chip_data.cal_1.cal_main;
		ovs_tdm_tbl_0 = _tdm->_chip_data.cal_0.cal_grp[0];
		ovs_tdm_tbl_1 = _tdm->_chip_data.cal_1.cal_grp[0];
	}
	for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
		_tdm->_core_data.vars_pkg.lr_buffer[idx1]=param_token_empty;
		_tdm->_core_data.vars_pkg.os_buffer[idx1]=param_token_empty;
	}
    for (idx1=0; idx1<TD2P_LR_LLS_LEN; idx1++) {
        TD2P_TOKEN_CHECK(pgw_tdm_tbl_0[idx1]) {
            port_exist = BOOL_FALSE;
            for (idx2=0; idx2<TD2P_LR_LLS_LEN; idx2++){
                if (_tdm->_core_data.vars_pkg.lr_buffer[idx2]==pgw_tdm_tbl_0[idx1]){
                    port_exist = BOOL_TRUE;
                    break;
                }
            }
            if (port_exist==BOOL_FALSE){
                if (lr_buffer_idx<TDM_AUX_SIZE){
                    _tdm->_core_data.vars_pkg.lr_buffer[lr_buffer_idx++]=pgw_tdm_tbl_0[idx1];
                }
                else{
                    TDM_ERROR4("\n %s, port %3d, buffer_idx %3d, limit %3d\n",
                              "Number of LineRate ports overflow",
                              pgw_tdm_tbl_0[idx1],
                              lr_buffer_idx,
                              (TDM_AUX_SIZE-1));
                }
            }
        }
    }
    for (idx1=0; idx1<TD2P_LR_LLS_LEN; idx1++) {
        TD2P_TOKEN_CHECK(pgw_tdm_tbl_1[idx1]) {
            port_exist = BOOL_FALSE;
            for (idx2=0; idx2<TD2P_LR_LLS_LEN; idx2++){
                if (_tdm->_core_data.vars_pkg.lr_buffer[idx2]==pgw_tdm_tbl_1[idx1]){
                    port_exist = BOOL_TRUE;
                    break;
                }
            }
            if (port_exist==BOOL_FALSE){
                if (lr_buffer_idx<TDM_AUX_SIZE){
                    _tdm->_core_data.vars_pkg.lr_buffer[lr_buffer_idx++]=pgw_tdm_tbl_1[idx1];
                }
                else{
                    TDM_ERROR4("\n %s, port %3d, buffer_idx %3d, limit %3d\n",
                              "Number of LineRate ports overflow",
                              pgw_tdm_tbl_1[idx1],
                              lr_buffer_idx,
                              (TDM_AUX_SIZE-1));
                }
            }
        }
    }
	for (idx1=0; idx1<TD2P_OS_LLS_GRP_LEN; idx1++) {
		TD2P_TOKEN_CHECK(ovs_tdm_tbl_0[idx1]) {
			_tdm->_core_data.vars_pkg.os_buffer[os_buffer_idx++]=ovs_tdm_tbl_0[idx1];
		}
	}
	for (idx1=0; idx1<TD2P_OS_LLS_GRP_LEN; idx1++) {
		TD2P_TOKEN_CHECK(ovs_tdm_tbl_1[idx1]) {
			_tdm->_core_data.vars_pkg.os_buffer[os_buffer_idx++]=ovs_tdm_tbl_1[idx1];
		}
	}
	
	/* param_higig_mgmt */
	if ( (_tdm->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[TD2P_NUM_PHY_PM]==PM_ENCAP__HIGIG2 && (pipe==1||pipe==2)) && 
	     ((_tdm->_core_data.vars_pkg.lr_buffer[0]!=param_token_empty && param_freq>=MIN_HG_FREQ) || (_tdm->_core_data.vars_pkg.lr_buffer[0]==param_token_empty)) ) {
		param_higig_mgmt=BOOL_TRUE;
	}
	
	/* param_lr_limit: TD2P has same cal_len for both EN and HG */
    switch (param_freq) {
        case 760: param_lr_limit=(LEN_760MHZ_EN-param_ancl_num); break;
        case 609: param_lr_limit=(LEN_608MHZ_EN-param_ancl_num); break;
        case 518: param_lr_limit=(LEN_517MHZ_EN-param_ancl_num); break;
        case 415: param_lr_limit=(LEN_415MHZ_EN-param_ancl_num); break;
        default : break;
    }
	
	/* core parameters */
	_tdm->_chip_data.soc_pkg.clk_freq    = param_freq;
	_tdm->_chip_data.soc_pkg.tvec_size   = param_ancl_num;
	_tdm->_chip_data.soc_pkg.lr_idx_limit= param_lr_limit;
	_tdm->_chip_data.soc_pkg.soc_vars.td2p.higig_mgmt = param_higig_mgmt;

	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_td2p_lls_wrapper
@param:

Code wrapper for ingress TDM scheduling
 */
int
tdm_td2p_lls_wrapper( tdm_mod_t *_tdm )
{
    int idx;
    int n, pgw_ll_len, pgw_ll_len_max;
    enum port_speed_e *param_speeds;
    tdm_chip_legacy_t *_td2p_chip;

	/* print stack usage */
	/* TDM_PRINT_STACK_SIZE("tdm_td2p_lls_wrapper"); */
    
	param_speeds      = _tdm->_chip_data.soc_pkg.speed;
	
	_td2p_chip = TDM_ALLOC(sizeof(tdm_chip_legacy_t), "TDM chip legacy");
	if (!_td2p_chip) {return FAIL;}
	tdm_chip_td2p_shim__ing_wrap(_tdm, _td2p_chip);
	
	TDM_PRINT0("TDM: Linked list round robin\n");
	for (idx=0; idx<TD2P_NUM_QUAD; idx++) {
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.pgw_num = idx;
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.start_port = (idx*(TD2P_NUM_PHY_PORTS/TD2P_NUM_QUAD));
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.stop_port = ((idx+1)*(TD2P_NUM_PHY_PORTS/TD2P_NUM_QUAD));
		{
			int i, j, first_wc=0, op_flags_str[2], relink, is_state_changed;
			struct ll_node llist;
			td2p_pgw_pntrs_t td2p_pntr_pkg;
			td2p_pgw_scheduler_vars_t td2p_vars_pkg;
			int *pgw_tdm_tbl, *ovs_tdm_tbl, *ovs_spacing;
			
			op_flags_str[0]=0; 
			op_flags_str[1]=0;
			llist.port=0;
			llist.next=NULL;
			td2p_pntr_pkg.pgw_tdm_idx=0;
			td2p_pntr_pkg.ovs_tdm_idx=0;
			td2p_pntr_pkg.tdm_stk_idx=0;
			for (i=0; i<TD2P_OS_LLS_GRP_LEN; i++){
				td2p_vars_pkg.swap_array[i] = 0;
			}
			
			/* PGW calendars */
			switch(_tdm->_chip_data.soc_pkg.soc_vars.td2p.start_port) {
				case 0:
					pgw_tdm_tbl = _td2p_chip->tdm_pgw.pgw_tdm_tbl_x0;
					ovs_tdm_tbl = _td2p_chip->tdm_pgw.ovs_tdm_tbl_x0;
					ovs_spacing = _td2p_chip->tdm_pgw.ovs_spacing_x0;
					break;
				case 32:
					pgw_tdm_tbl = _td2p_chip->tdm_pgw.pgw_tdm_tbl_x1;
					ovs_tdm_tbl = _td2p_chip->tdm_pgw.ovs_tdm_tbl_x1;
					ovs_spacing = _td2p_chip->tdm_pgw.ovs_spacing_x1;
					break;		
				case 64:
					pgw_tdm_tbl = _td2p_chip->tdm_pgw.pgw_tdm_tbl_y0;
					ovs_tdm_tbl = _td2p_chip->tdm_pgw.ovs_tdm_tbl_y0;
					ovs_spacing = _td2p_chip->tdm_pgw.ovs_spacing_y0;
					break;		
				default:
					pgw_tdm_tbl = _td2p_chip->tdm_pgw.pgw_tdm_tbl_y1;
					ovs_tdm_tbl = _td2p_chip->tdm_pgw.ovs_tdm_tbl_y1;
					ovs_spacing = _td2p_chip->tdm_pgw.ovs_spacing_y1;
					break;
			}

			/* wrap core index */
			switch(_tdm->_chip_data.soc_pkg.soc_vars.td2p.pgw_num) {
				case 0: first_wc = 0; break;
				case 1: first_wc = 8; break;
				case 2: first_wc = 16; break;
				case 3: first_wc = 24; break;
			}
			/* port number range */
			td2p_vars_pkg.first_port=_tdm->_chip_data.soc_pkg.soc_vars.td2p.start_port;
			td2p_vars_pkg.last_port =_tdm->_chip_data.soc_pkg.soc_vars.td2p.stop_port;
			/* linked-list, and PGW oversub calendar */			
			for (j=0; j<4; j++) {
				td2p_vars_pkg.subport=j;
				td2p_pntr_pkg.cur_idx=first_wc;
				td2p_vars_pkg.cur_idx_max=(first_wc+8);
				tdm_td2p_lls_scheduler(&llist, _td2p_chip, &td2p_pntr_pkg, &td2p_vars_pkg, &pgw_tdm_tbl, &ovs_tdm_tbl, op_flags_str);
			}
            /* single 20G mode */
            switch(_tdm->_chip_data.soc_pkg.clk_freq){
                case 415:
                case 416:
                    pgw_ll_len_max = 12;
                    break;
                case 517:
                case 518:
                    pgw_ll_len_max = 16;
                    break;
                case 608:
                case 609:
                    if (td2p_vars_pkg.first_port==32 ||
                        td2p_vars_pkg.first_port==64){
                        pgw_ll_len_max = 20;
                    }
                    else {
                        pgw_ll_len_max = 16;
                    }
                    break;
                case 760:
                    pgw_ll_len_max = 24;
                    break;
                default:
                    pgw_ll_len_max = 32;
                    TDM_ERROR1("TDM: unrecognized frequency %d in PGW\n", 
                               _tdm->_chip_data.soc_pkg.clk_freq);
                    break;
            }
            pgw_ll_len = tdm_td2p_ll_len(&llist);
            if (pgw_ll_len>pgw_ll_len_max){
                TDM_PRINT2("TDM: Adjust PGW linked list, len %d, limit %d\n",
                           pgw_ll_len, pgw_ll_len_max);
                n = pgw_ll_len - pgw_ll_len_max;
                for (i=(pgw_ll_len-1); i>0; i--){
                    if (tdm_td2p_ll_get(&llist,i)==TD2P_OVSB_TOKEN){
                        tdm_td2p_ll_delete(&llist, i);
                        n--;
                        TDM_PRINT2("%s, index %d\n",
                                  "remove OVSB slot from PGW linked list",
                                  i);
                    }
                    if (n<=0){
                        break;
                    }
                }
            }
			td2p_pntr_pkg.pgw_tdm_idx=tdm_td2p_ll_len(&llist);
			/* same-port-min-spacing */
			relink=BOOL_FALSE;
			for (i=1; i<tdm_td2p_ll_len(&llist); i++) {
				TD2P_TOKEN_CHECK(tdm_td2p_ll_get(&llist,i)) {
					if ( tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(&llist,i),_td2p_chip->pmap)==tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(&llist,(i-1)),_td2p_chip->pmap) ) {
						relink=BOOL_TRUE;
						break;
					}
				}
			}
			if (relink) {
				TDM_PRINT0("TDM: Retracing calendar\n");
				tdm_td2p_ll_retrace(&llist,_td2p_chip->pmap);
				tdm_td2p_ll_print(&llist);
			}
			/* same-port-min-spacing */
			relink=BOOL_FALSE;
			for (i=1; i<tdm_td2p_ll_len(&llist); i++) {
				TD2P_TOKEN_CHECK(tdm_td2p_ll_get(&llist,i)) {
					if ( tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(&llist,i),_td2p_chip->pmap)==tdm_td2p_scan_which_tsc(tdm_td2p_ll_get(&llist,(i-1)),_td2p_chip->pmap) ) {
						relink=BOOL_TRUE;
						break;
					}
				}
			}
			if (relink) {
				TDM_PRINT0("TDM: Reweaving calendar\n");
				tdm_td2p_ll_weave(&llist,_td2p_chip->pmap,TD2P_OVSB_TOKEN);
				tdm_td2p_ll_print(&llist);
			}
			else if ( (tdm_td2p_ll_count(&llist,1)>0 && tdm_td2p_ll_count(&llist,TD2P_OVSB_TOKEN)>1) || (tdm_td2p_ll_single_100(&llist)) ) {
				TDM_PRINT0("TDM: Reweaving calendar\n");
				tdm_td2p_ll_weave(&llist,_td2p_chip->pmap,TD2P_OVSB_TOKEN);
				tdm_td2p_ll_print(&llist);
			}
			/* PGW main calendar */
			tdm_td2p_ll_deref(&llist,&pgw_tdm_tbl,TD2P_LR_LLS_LEN);
			/* PGW oversub spacer calendar */
			TDM_SML_BAR
			tdm_td2p_print_tbl(pgw_tdm_tbl,TD2P_LR_LLS_LEN,"PGW Main Calendar",idx);
			if (ovs_tdm_tbl[0]!=TD2P_NUM_EXT_PORTS) {
                tdm_td2p_ovs_spacer(&_td2p_chip->pmap,ovs_tdm_tbl,ovs_spacing, param_speeds);
                /* tdm_td2p_print_tbl_ovs(ovs_tdm_tbl,ovs_spacing,TD2P_OS_LLS_GRP_LEN,"PGW Oversub Calendar",idx); */
				
				is_state_changed = BOOL_FALSE;
				for (i=0; i<TD2P_OS_LLS_GRP_LEN; i++) {
					if ( (ovs_tdm_tbl[i]!=TD2P_NUM_EXT_PORTS) && 
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

            /* regenerate PGW oversub calendar and spacing calendar under flexport restriction */
            tdm_td2p_proc_pgw_os_cal(_td2p_chip, &td2p_vars_pkg, ovs_tdm_tbl, ovs_spacing);
            tdm_td2p_print_tbl_ovs(ovs_tdm_tbl,ovs_spacing,TD2P_OS_LLS_GRP_LEN,"PGW Oversub Calendar",idx);
			
			tdm_td2p_ll_free(&llist);
		}
	}
	
	/* Bind to TDM class object */
	TDM_COPY(_tdm->_chip_data.cal_0.cal_main,_td2p_chip->tdm_pgw.pgw_tdm_tbl_x0,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_0.cal_grp[0],_td2p_chip->tdm_pgw.ovs_tdm_tbl_x0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_0.cal_grp[1],_td2p_chip->tdm_pgw.ovs_spacing_x0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_main,_td2p_chip->tdm_pgw.pgw_tdm_tbl_x1,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_grp[0],_td2p_chip->tdm_pgw.ovs_tdm_tbl_x1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_grp[1],_td2p_chip->tdm_pgw.ovs_spacing_x1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_2.cal_main,_td2p_chip->tdm_pgw.pgw_tdm_tbl_y0,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_2.cal_grp[0],_td2p_chip->tdm_pgw.ovs_tdm_tbl_y0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_2.cal_grp[1],_td2p_chip->tdm_pgw.ovs_spacing_y0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_3.cal_main,_td2p_chip->tdm_pgw.pgw_tdm_tbl_y1,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_3.cal_grp[0],_td2p_chip->tdm_pgw.ovs_tdm_tbl_y1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_3.cal_grp[1],_td2p_chip->tdm_pgw.ovs_spacing_y1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	
	/* Realign port state array to old specification */	
	for (idx=0; idx<((_tdm->_chip_data.soc_pkg.num_ext_ports)-1); idx++) {
		_tdm->_chip_data.soc_pkg.state[idx] = _tdm->_chip_data.soc_pkg.state[idx+1];
	}
	
	_tdm->_core_data.vars_pkg.pipe=TD2P_PIPE_X_ID;
	TDM_FREE(_td2p_chip);
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__EGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_td2p_pmap_transcription_unsupported_quad_mode
@param:

Convert any unsupported quad mode to regular quad mode.
 */
static int
tdm_td2p_pmap_transcription_unsupported_quad_mode( tdm_mod_t *_tdm )
{
    int pm_num, lane_num, port, port_speed,
        pm_speed_max, pm_speed_min, pm_port_cnt, pm_port_lo;
    int param_phy_pms, param_pm_lanes;

    param_phy_pms  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;

    for (pm_num = 0; pm_num < param_phy_pms; pm_num++) {
        pm_port_cnt = 0;
        pm_speed_max = 0;
        pm_speed_min = SPEED_100G;
        pm_port_lo = pm_num * param_pm_lanes + 1;
        for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
            port = pm_port_lo + lane_num;
            port_speed = _tdm->_chip_data.soc_pkg.speed[port];
            if (port_speed > 0) {
                pm_port_cnt++;
                pm_speed_min = (pm_speed_min > port_speed) ? port_speed :
                                                             pm_speed_min;
                pm_speed_max = (pm_speed_max < port_speed) ? port_speed :
                                                             pm_speed_max;
            }
        }
        /* force to quad mode */
        if (pm_port_cnt == 4 && pm_speed_min != pm_speed_max) {
            for (lane_num = 0; lane_num < param_pm_lanes; lane_num++) {
                port = pm_port_lo + lane_num;
                port_speed = _tdm->_chip_data.soc_pkg.speed[port];
                if (port_speed != pm_speed_max) {
                    _tdm->_chip_data.soc_pkg.speed[port] = pm_speed_max;
                    TDM_PRINT5("%s, convert PM %0d port %0d from %0dG to %0dG\n",
                             "TDM: [unsupported PM Quad Mode]",
                             pm_num, port, port_speed/1000, pm_speed_max/1000);
                }
            }
        }
    }

    return PASS;
}


/**
@name: tdm_td2p_pmap_transcription
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
tdm_td2p_pmap_transcription( tdm_mod_t *_tdm )
{
	int i, j, last_port=TD2P_NUM_EXT_PORTS, tsc_active;
	int phy_lo, phy_hi, pm_lane_num, bcm_config_check=PASS;
    int p, spd_1g_en, spd_10g_plus_en;
    
	/* Management ports */
	if( (_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_0]==PORT_STATE__MANAGEMENT) ||
	    (_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_1]==PORT_STATE__MANAGEMENT) ||
	    (_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_2]==PORT_STATE__MANAGEMENT) ||
	    (_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_3]==PORT_STATE__MANAGEMENT) ){
		if( (((TD2P_MGMT_PORT_0-1)/TD2P_NUM_PM_LNS)==TD2P_MGMT_TSC_ID) &&
		    (((TD2P_MGMT_PORT_1-1)/TD2P_NUM_PM_LNS)==TD2P_MGMT_TSC_ID) &&
			(((TD2P_MGMT_PORT_2-1)/TD2P_NUM_PM_LNS)==TD2P_MGMT_TSC_ID) &&
			(((TD2P_MGMT_PORT_3-1)/TD2P_NUM_PM_LNS)==TD2P_MGMT_TSC_ID) ){
			_tdm->_chip_data.soc_pkg.pmap[TD2P_MGMT_TSC_ID][0] = ((_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_0]==PORT_STATE__MANAGEMENT) ? (TD2P_MGMT_TOKEN): (TD2P_NUM_EXT_PORTS) );
			_tdm->_chip_data.soc_pkg.pmap[TD2P_MGMT_TSC_ID][1] = ((_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_1]==PORT_STATE__MANAGEMENT) ? (TD2P_MGMT_TOKEN): (TD2P_NUM_EXT_PORTS) );
			_tdm->_chip_data.soc_pkg.pmap[TD2P_MGMT_TSC_ID][2] = ((_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_2]==PORT_STATE__MANAGEMENT) ? (TD2P_MGMT_TOKEN): (TD2P_NUM_EXT_PORTS) );
			_tdm->_chip_data.soc_pkg.pmap[TD2P_MGMT_TSC_ID][3] = ((_tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_3]==PORT_STATE__MANAGEMENT) ? (TD2P_MGMT_TOKEN): (TD2P_NUM_EXT_PORTS) );
		}
		else{
			TDM_ERROR0("Unmatched MANAGEMENT port numbering config between physical port number and TSC index. \n");
		}
	}

    /* regulate unsupported PM quad modes */
    tdm_td2p_pmap_transcription_unsupported_quad_mode(_tdm);

	/* Regular ports */
	for (i=1; i<=TD2P_NUM_PHY_PORTS; i+=TD2P_NUM_PM_LNS) {
		tsc_active=BOOL_FALSE;
		for (j=0; j<TD2P_NUM_PM_LNS; j++) {
			if ( _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__LINERATE    || _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__OVERSUB    ||
			     _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__LINERATE_HG || _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__OVERSUB_HG ){
				tsc_active=BOOL_TRUE;
				break;
			}
		}
		if(tsc_active==BOOL_TRUE){
			if ( _tdm->_chip_data.soc_pkg.speed[i]<=SPEED_42G_HG2 || _tdm->_chip_data.soc_pkg.state[i]==PORT_STATE__DISABLED ) {
				for (j=0; j<TD2P_NUM_PM_LNS; j++) {
					switch (_tdm->_chip_data.soc_pkg.state[i+j]) {
						case PORT_STATE__LINERATE:
						case PORT_STATE__OVERSUB:
						case PORT_STATE__LINERATE_HG:
						case PORT_STATE__OVERSUB_HG:
							_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][j]=(i+j);
							last_port=(i+j);
							break;
						case PORT_STATE__COMBINE:
							_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][j]=last_port;
							break;
						default:
							_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][j]=TD2P_NUM_EXT_PORTS;
							break;
					}
				}
                /* tri mode x_xx */
                if (_tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2] &&
                    _tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3] &&
                    _tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0 &&
                    _tdm->_chip_data.soc_pkg.speed[i+2]==SPEED_0) {
                    _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2];
                    _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0];
                }
                /* tri mode xxx_ */
                else if (_tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] &&
                         _tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2] &&
                         _tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0 &&
                         _tdm->_chip_data.soc_pkg.speed[i+3]!=SPEED_0) {
                    _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1];
                    _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3];
                }
                /* dual mode x_x_ */
                else if (_tdm->_chip_data.soc_pkg.speed[i]>SPEED_0 &&
                         _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+2] &&
                         _tdm->_chip_data.soc_pkg.speed[i+1]==SPEED_0 &&
                         _tdm->_chip_data.soc_pkg.speed[i+3]==SPEED_0) {
                    if ( _tdm->_chip_data.soc_pkg.state[i]==PORT_STATE__COMBINE && _tdm->_chip_data.soc_pkg.state[i+2]==PORT_STATE__COMBINE ) {
                        _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3];
                        _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0];
                    }
                }
			}
			else {
                if ( (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G || 
                      _tdm->_chip_data.soc_pkg.speed[i]==SPEED_106G ) &&
                     (_tdm->_chip_data.soc_pkg.state[i+10]!=PORT_STATE__COMBINE) ){
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][2] = TD2P_NUM_EXT_PORTS;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][3] = TD2P_NUM_EXT_PORTS;
				}
                else if ( _tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G ||
                          _tdm->_chip_data.soc_pkg.speed[i]==SPEED_106G ||
                          _tdm->_chip_data.soc_pkg.speed[i]==SPEED_120G ||
                          _tdm->_chip_data.soc_pkg.speed[i]==SPEED_127G ){
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0] = i;	
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][3] = i;
				}
				i+=8;
			}
		}
	}
	tdm_print_stat( _tdm );
	/* Management port bandwidth */
	_tdm->_chip_data.soc_pkg.soc_vars.td2p.mgmtbw=0;
	for (i=0; i<TD2P_NUM_PM_LNS; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[TD2P_MGMT_TSC_ID][i]==TD2P_MGMT_TOKEN) {
			((_tdm->_chip_data.soc_pkg.soc_vars.td2p.mgmtbw)++);
		}
	}
	
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
    
    /* 13G/16G support 
     *   -- 13G/16G will be taken as 21G.
     *   -- 13G/16G only support 4-lane mode, i.e. [x, 0, 0, 0]
     */
    for (i=1; i<=TD2P_NUM_PHY_PORTS; i+=TD2P_NUM_PM_LNS) {
        /* convert speed */
        if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_13G ||
            _tdm->_chip_data.soc_pkg.speed[i]==SPEED_16G) {
            _tdm->_chip_data.soc_pkg.speed[i] = SPEED_21G;
        }
        /* check TSC mode */
        for (j=1; j<TD2P_NUM_PM_LNS; j++) {
            if (_tdm->_chip_data.soc_pkg.speed[i+j]==SPEED_13G ||
                _tdm->_chip_data.soc_pkg.speed[i+j]==SPEED_16G) {
                TDM_ERROR1("Invalid port configuration, %s\n",
                           "13G/16G port must be 4-lane mode");
                return FAIL;
            }
        }
    }

    /* 1G/2.5G support:
     *   -- only apply on linerate PM config
     *   -- if 1G/2.5G is mixed with 10G in one PM, convert 1G/2.5G to 10G.
     */
    for (i=1; i<=(phy_hi-pm_lane_num); i+=pm_lane_num) {
        spd_1g_en = 0;
        spd_10g_plus_en = 0;
        for (j=0; j<pm_lane_num; j++) {
            p = i + j;
            if (_tdm->_chip_data.soc_pkg.state[p-1]==PORT_STATE__LINERATE) {
                if (_tdm->_chip_data.soc_pkg.speed[p]==SPEED_1G ||
                    _tdm->_chip_data.soc_pkg.speed[p]==SPEED_2p5G) {
                    spd_1g_en = 1;
                } else if (_tdm->_chip_data.soc_pkg.speed[p]>=SPEED_10G) {
                    spd_10g_plus_en = 1;
                }
            }
        }
        if (spd_1g_en && spd_10g_plus_en) {
            for (j=0; j<pm_lane_num; j++) {
                p = i + j;
                if ((_tdm->_chip_data.soc_pkg.state[p-1]==PORT_STATE__LINERATE) &&
                    (_tdm->_chip_data.soc_pkg.speed[p]==SPEED_1G ||
                     _tdm->_chip_data.soc_pkg.speed[p]==SPEED_2p5G)) {
                    TDM_PRINT3("%s port %3d, convert speed from %0dG to 10G\n",
                               "TDM: [1G CONFIG]",
                               p, _tdm->_chip_data.soc_pkg.speed[p]/1000);
                    _tdm->_chip_data.soc_pkg.speed[p] = SPEED_10G;
                }
            }
        }
    }

	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_chip_td2p_init
@param:
 */
int
tdm_td2p_init( tdm_mod_t *_tdm )
{
	int index;
/*
#ifdef _TDM_DB_STACK
    stack_size = (size_t)&index;
#endif */
	
	/* Chip: parameters */
	_tdm->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PM_MOD;
	_tdm->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm->_chip_data.soc_pkg.pm_num_phy_modules = TD2P_NUM_PHY_PM;
	
	_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = TD2P_OVSB_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl1_token = TD2P_IDL1_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl2_token = TD2P_IDL2_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.ancl_token = TD2P_ANCL_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = TD2P_NUM_PHY_PORTS;
	
	_tdm->_chip_data.cal_0.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_0.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_0.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_1.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_1.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_1.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_2.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_2.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_2.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_3.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_3.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_3.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_4.cal_len = TD2P_LR_VBS_LEN;
	_tdm->_chip_data.cal_4.grp_num = TD2P_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_4.grp_len = TD2P_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_5.cal_len = TD2P_LR_VBS_LEN;
	_tdm->_chip_data.cal_5.grp_num = TD2P_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_5.grp_len = TD2P_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_6.cal_len = TD2P_LR_IARB_STATIC_LEN;
	_tdm->_chip_data.cal_6.grp_num = 0;
	_tdm->_chip_data.cal_6.grp_len = 0;
	_tdm->_chip_data.cal_7.cal_len = TD2P_LR_IARB_STATIC_LEN;
	_tdm->_chip_data.cal_7.grp_num = 0;
	_tdm->_chip_data.cal_7.grp_len = 0;
	
	/* Chip: management port state */
    if (_tdm->_chip_data.soc_pkg.soc_vars.td2p.tdm_mgmt_en!=BOOL_FALSE){
        /* 10_0_0_0 */
        if (_tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_0]==SPEED_10G &&
            _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_1]==SPEED_0   &&
            _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_2]==SPEED_0   &&
            _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_3]==SPEED_0){
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_0] = PORT_STATE__MANAGEMENT;
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_1] = PORT_STATE__DISABLED;
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_2] = PORT_STATE__DISABLED;
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_3] = PORT_STATE__DISABLED;
        }
        /* x_x_x_x*/
        else if (_tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_0]<SPEED_10G &&
                 _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_0]>SPEED_0   &&
                 _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_0]==_tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_1] &&
                 _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_1]==_tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_2] &&
                 _tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_2]==_tdm->_chip_data.soc_pkg.speed[TD2P_MGMT_PORT_3] ){
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_0] = PORT_STATE__MANAGEMENT;
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_1] = PORT_STATE__MANAGEMENT;
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_2] = PORT_STATE__MANAGEMENT;
            _tdm->_chip_data.soc_pkg.state[TD2P_MGMT_PORT_3] = PORT_STATE__MANAGEMENT;
        }
    }
	/* Chip: encap */
	for (index=0; index<TD2P_NUM_PM_MOD; index++) {
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*TD2P_NUM_PM_LNS+1]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index*TD2P_NUM_PM_LNS+1]==PORT_STATE__OVERSUB_HG)?(PM_ENCAP__HIGIG2):(PM_ENCAP__ETHRNT);
	}
	/* Chip: pmap */
	_tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		_tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	}
	/* Chip: PGW x0 calendar group */
	_tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");
	TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
	_tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		_tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
	}
	/* Chip: PGW x1 calendar group */
	_tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
	_tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		_tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
	}
	/* Chip: PGW y0 calendar group */
	_tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
	_tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		_tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
	}
	/* Chip: PGW y1 calendar group */
	_tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
	_tdm->_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_num)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		_tdm->_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.grp_len);
	}
	/* Chip: MMU x pipe calendar group */
	_tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 4 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);
	_tdm->_chip_data.cal_4.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_num)*sizeof(int *), "TDM inst 4 groups");
	for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
		_tdm->_chip_data.cal_4.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_len)*sizeof(int), "TDM inst 4 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.grp_len);
	}
	/* Chip: MMU y pipe calendar group */
	_tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 5 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
	_tdm->_chip_data.cal_5.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_num)*sizeof(int *), "TDM inst 5 groups");
	for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
		_tdm->_chip_data.cal_5.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_len)*sizeof(int), "TDM inst 5 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_5.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.grp_len);
	}
	/* Chip: IARB static calendar group */
	_tdm->_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.cal_len)*sizeof(int), "TDM inst 6 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_6.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.cal_len);
	_tdm->_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.cal_len)*sizeof(int), "TDM inst 7 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_7.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.cal_len);

	/* Core: parameters */
	_tdm->_core_data.vmap_max_len        = TD2P_VMAP_MAX_LEN;
	_tdm->_core_data.vmap_max_wid        = TD2P_VMAP_MAX_WID;
	_tdm->_core_data.rule__same_port_min = LLS_MIN_SPACING;
	_tdm->_core_data.rule__prox_port_min = VBS_MIN_SPACING;
	/* Core: vmap */
	_tdm->_core_data.vmap=(unsigned short **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned short *), "vector_map_l1");
	for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
		_tdm->_core_data.vmap[index]=(unsigned short *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned short), "vector_map_l2");
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/**
@name: tdm_td2p_post
@param:
 */
int
tdm_td2p_post( tdm_mod_t *_tdm )
{
    /* TDM self-check */
    if (_tdm->_chip_data.soc_pkg.soc_vars.td2p.tdm_chk_en==BOOL_TRUE){
        _tdm->_chip_exec[TDM_CHIP_EXEC__CHECK](_tdm);
    }

    return PASS;
}


/**
@name: tdm_td2p_free
@param:
 */
int
tdm_td2p_free( tdm_mod_t *_tdm )
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
    /* Chip: PGW y0 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_2.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_2.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_2.cal_grp);
    /* Chip: PGW y1 calendar group */
    TDM_FREE(_tdm->_chip_data.cal_3.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_3.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_3.cal_grp);
    /* Chip: MMU x pipe calendar group */
    TDM_FREE(_tdm->_chip_data.cal_4.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_4.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_4.cal_grp);
    /* Chip: MMU y pipe calendar group */
    TDM_FREE(_tdm->_chip_data.cal_5.cal_main);
    for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
        TDM_FREE(_tdm->_chip_data.cal_5.cal_grp[index]);
    }
    TDM_FREE(_tdm->_chip_data.cal_5.cal_grp);
    /* Chip: IARB static calendar group */
    TDM_FREE(_tdm->_chip_data.cal_6.cal_main);
    TDM_FREE(_tdm->_chip_data.cal_7.cal_main);
    /* Core: vmap */
    for (index=0; index<(_tdm->_core_data.vmap_max_wid); index++) {
        TDM_FREE(_tdm->_core_data.vmap[index]);
    }
    TDM_FREE(_tdm->_core_data.vmap);

    return PASS;
}
