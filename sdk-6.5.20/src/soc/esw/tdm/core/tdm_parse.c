/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_class_data
@param:
 */
void
tdm_class_data( tdm_mod_t *_tdm )
{
	int idx0, idx1;
	
	TDM_BIG_BAR
	TDM_PRINT0("Contents of TDM class:\n");
	TDM_SML_BAR
	TDM_PRINT1("\t_tdm->_core_data.vmap_max_len = %0d\n",_tdm->_core_data.vmap_max_len);
	TDM_PRINT1("\t_tdm->_core_data.vmap_max_wid = %0d\n",_tdm->_core_data.vmap_max_wid);
	TDM_SML_BAR
	TDM_PRINT1("\t_tdm->_chip_data.soc_pkg.pmap_num_modules = %0d\n",_tdm->_chip_data.soc_pkg.pmap_num_modules);
	TDM_PRINT1("\t_tdm->_chip_data.soc_pkg.pmap_num_lanes = %0d\n",_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	TDM_PRINT1("\t_tdm->_chip_data.soc_pkg.clk_freq = %0d\n",_tdm->_chip_data.soc_pkg.clk_freq);
	TDM_PRINT1("\t_tdm->_chip_data.soc_pkg.lr_idx_limit = %0d\n",_tdm->_chip_data.soc_pkg.lr_idx_limit);
	TDM_PRINT1("\t_tdm->_chip_data.soc_pkg.num_ext_ports = %0d\n",_tdm->_chip_data.soc_pkg.num_ext_ports);
	TDM_SML_BAR
	for (idx0=0; idx0<_tdm->_chip_data.soc_pkg.num_ext_ports; idx0++) {
		TDM_PRINT4("\t_tdm->_chip_data.soc_pkg.speed[%03d] = %0d \t| _tdm->_chip_data.soc_pkg.state[%03d] = %0d\n",idx0,(_tdm->_chip_data.soc_pkg.speed[idx0]/1000),idx0,_tdm->_chip_data.soc_pkg.state[idx0]);
	}
	
	if (_tdm->_chip_data.cal_0.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_0.cal_len = %0d\n",_tdm->_chip_data.cal_0.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_0.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_0.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_0.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_0.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_0.grp_num = %0d\n",_tdm->_chip_data.cal_0.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_0.grp_len = %0d\n",_tdm->_chip_data.cal_0.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_0.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_0.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_0.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_0.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	if (_tdm->_chip_data.cal_1.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_1.cal_len = %0d\n",_tdm->_chip_data.cal_1.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_1.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_1.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_1.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_1.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_1.grp_num = %0d\n",_tdm->_chip_data.cal_1.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_1.grp_len = %0d\n",_tdm->_chip_data.cal_1.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_1.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_1.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_1.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_1.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	if (_tdm->_chip_data.cal_2.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_2.cal_len = %0d\n",_tdm->_chip_data.cal_2.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_2.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_2.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_2.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_2.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_2.grp_num = %0d\n",_tdm->_chip_data.cal_2.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_2.grp_len = %0d\n",_tdm->_chip_data.cal_2.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_2.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_2.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_2.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_2.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	if (_tdm->_chip_data.cal_3.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_3.cal_len = %0d\n",_tdm->_chip_data.cal_3.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_3.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_3.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_3.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_3.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_3.grp_num = %0d\n",_tdm->_chip_data.cal_3.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_3.grp_len = %0d\n",_tdm->_chip_data.cal_3.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_3.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_3.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_3.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_3.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	if (_tdm->_chip_data.cal_5.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_5.cal_len = %0d\n",_tdm->_chip_data.cal_5.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_5.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_5.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_5.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_5.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_5.grp_num = %0d\n",_tdm->_chip_data.cal_5.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_5.grp_len = %0d\n",_tdm->_chip_data.cal_5.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_5.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_5.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_5.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_5.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	if (_tdm->_chip_data.cal_6.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_6.cal_len = %0d\n",_tdm->_chip_data.cal_6.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_6.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_6.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_6.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_6.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_6.grp_num = %0d\n",_tdm->_chip_data.cal_6.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_6.grp_len = %0d\n",_tdm->_chip_data.cal_6.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_6.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_6.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_6.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_6.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	if (_tdm->_chip_data.cal_7.cal_len>0) {
		TDM_SML_BAR
		TDM_PRINT1("\t_tdm->_chip_data.cal_7.cal_len = %0d\n",_tdm->_chip_data.cal_7.cal_len);
		for (idx0=0; idx0<_tdm->_chip_data.cal_7.cal_len; idx0++) {
			TDM_PRINT2("\t\t_tdm->_chip_data.cal_7.cal_main[%03d] = %0d\n",idx0,_tdm->_chip_data.cal_7.cal_main[idx0]);
		}
		if (_tdm->_chip_data.cal_7.grp_num>0) {
			TDM_PRINT1("\t_tdm->_chip_data.cal_7.grp_num = %0d\n",_tdm->_chip_data.cal_7.grp_num);
			TDM_PRINT1("\t_tdm->_chip_data.cal_7.grp_len = %0d\n",_tdm->_chip_data.cal_7.grp_len);
			for (idx0=0; idx0<_tdm->_chip_data.cal_7.grp_num; idx0++) {
				for (idx1=0; idx1<_tdm->_chip_data.cal_7.grp_len; idx1++) {
					TDM_PRINT3("\t\t_tdm->_chip_data.cal_7.cal_grp[%02d][%02d] = %0d\n",idx0,idx1,_tdm->_chip_data.cal_7.cal_grp[idx0][idx1]);
				}
			}
		}
	}
	
	
	TDM_BIG_BAR

}


/**
@name: tdm_print_tsc
@param:
 */
void
tdm_print_stat( tdm_mod_t *_tdm )
{
	int pm_num, ln_num, log_port=0, last_phy_port, last_phy_port_2;
	last_phy_port  = _tdm->_chip_data.soc_pkg.num_ext_ports;
	last_phy_port_2= _tdm->_chip_data.soc_pkg.num_ext_ports;
	
	TDM_PRINT0("TDM: Portmap Summary\n");
  	for (pm_num=0; pm_num<_tdm->_chip_data.soc_pkg.pm_num_phy_modules; pm_num++) {
      	for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			if (ln_num==0) {
				TDM_PRINT1("\tPM_Inst [%02d] \n", pm_num);
			}
			TDM_PRINT1("\t\t\tPM_Lane [%0d]\t ",ln_num);
			
			if ( _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num]==_tdm->_chip_data.soc_pkg.num_ext_ports ) {
				TDM_PRINT0("->\tP_Port [---]\t ->\tL_Port [---] \n");
			}
			else if(_tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num] > _tdm->_chip_data.soc_pkg.num_ext_ports){
				if(_tdm->_chip_data.soc_pkg.speed[pm_num*(_tdm->_chip_data.soc_pkg.pmap_num_lanes) + ln_num + 1]/1000 > 0){
					TDM_PRINT2("->\tP_Port [%03d]\t ->\tL_Port [%03d]\t ", _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num], ++log_port);
					TDM_PRINT1("->\tSpeed [%d Gbps] ",_tdm->_chip_data.soc_pkg.speed[pm_num*(_tdm->_chip_data.soc_pkg.pmap_num_lanes) + ln_num + 1]/1000);
					switch ( _tdm->_chip_data.soc_pkg.state[pm_num*(_tdm->_chip_data.soc_pkg.pmap_num_lanes) + ln_num + 1] ) {
						case PORT_STATE__MANAGEMENT:
							TDM_PRINT0(" [MGMT]\n");
							break;
						default:
							TDM_PRINT0("\n");
							break;
					}
				}
				else{
					TDM_PRINT0("->\tP_Port [---]\t ->\tL_Port [---] \n");
				}
			}
			else {
				if ( _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num]!=last_phy_port   &&
				     _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num]!=last_phy_port_2 ) {
					TDM_PRINT2("->\tP_Port [%03d]\t ->\tL_Port [%03d]\t ", _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num], ++log_port);
					TDM_PRINT1("->\tSpeed [%d Gbps] ",_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num]]/1000);
										
					switch ( _tdm->_chip_data.soc_pkg.state[_tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num]] ) {
						case PORT_STATE__LINERATE:
							TDM_PRINT0(" [LNRT] [ETHN]\n");
							break;
						case PORT_STATE__OVERSUB:
							TDM_PRINT0(" [OVSB] [ETHN]\n");
							break;
						case PORT_STATE__LINERATE_HG:
							TDM_PRINT0(" [LNRT] [HIG2]\n");
							break;
						case PORT_STATE__OVERSUB_HG:
							TDM_PRINT0(" [OVSB] [HIG2]\n");
							break;
						default:
							TDM_PRINT0("\n");
							break;
					}
					last_phy_port_2 = last_phy_port;
					last_phy_port=_tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num];
				}
				else {
					TDM_PRINT1("->\tP_Port [%03d]\t ->\tL_Port [---]\n", _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num]);
				}
			}
		}
	}
	TDM_SML_BAR
}


/**
@name: tdm_print_config
@param:
 */
void
tdm_print_config( tdm_mod_t *_tdm )
{
	int pm_num, ln_num, lgc_port=0, phy_port, last_phy_port, last_phy_port_2;
	last_phy_port  = _tdm->_chip_data.soc_pkg.num_ext_ports;
	last_phy_port_2= _tdm->_chip_data.soc_pkg.num_ext_ports;
	
	TDM_PRINT0("TDM: Portmap Summary\n");
  	for (pm_num=0; pm_num<_tdm->_chip_data.soc_pkg.pm_num_phy_modules; pm_num++) {
      	for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
            phy_port = _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num];
			if (ln_num==0) {
				TDM_PRINT1("\tPM_Inst [%02d] \n", pm_num);
			}
			TDM_PRINT1("\t\t\tPM_Lane [%0d]\t ",ln_num);
			
			if ( phy_port==_tdm->_chip_data.soc_pkg.num_ext_ports ) {
				TDM_PRINT0("->\tP_Port [---]\t ->\tL_Port [---] \n");
			}
			else if(phy_port>_tdm->_chip_data.soc_pkg.num_ext_ports){
                phy_port = pm_num*(_tdm->_chip_data.soc_pkg.pmap_num_lanes) + ln_num + 1;
				if(_tdm->_chip_data.soc_pkg.speed[phy_port]/1000 > 0){
					TDM_PRINT2("->\tP_Port [%03d]\t ->\tL_Port [%03d]\t ", _tdm->_chip_data.soc_pkg.pmap[pm_num][ln_num], ++lgc_port);
					TDM_PRINT1("->\tSpeed [%d Gbps] ",_tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
					switch ( _tdm->_chip_data.soc_pkg.state[phy_port-1] ) {
						case PORT_STATE__MANAGEMENT:
							TDM_PRINT0(" [MGMT]\n");
							break;
						default:
							TDM_PRINT0("\n");
							break;
					}
				}
				else{
					TDM_PRINT0("->\tP_Port [---]\t ->\tL_Port [---] \n");
				}
			}
			else {
				if ( phy_port!=last_phy_port   &&
				     phy_port!=last_phy_port_2 &&
                     phy_port<=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi &&
                     phy_port>=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo) {
					TDM_PRINT2("->\tP_Port [%03d]\t ->\tL_Port [%03d]\t ", phy_port, ++lgc_port);
					TDM_PRINT1("->\tSpeed [%d Gbps] ",_tdm->_chip_data.soc_pkg.speed[phy_port]/1000);
										
					switch ( _tdm->_chip_data.soc_pkg.state[phy_port-1] ) {
						case PORT_STATE__LINERATE:
							TDM_PRINT0(" [LNRT] [ETHN]\n");
							break;
						case PORT_STATE__OVERSUB:
							TDM_PRINT0(" [OVSB] [ETHN]\n");
							break;
						case PORT_STATE__LINERATE_HG:
							TDM_PRINT0(" [LNRT] [HIG2]\n");
							break;
						case PORT_STATE__OVERSUB_HG:
							TDM_PRINT0(" [OVSB] [HIG2]\n");
							break;
						default:
							TDM_PRINT0("\n");
							break;
					}
					last_phy_port_2 = last_phy_port;
					last_phy_port   = phy_port;
				}
				else {
					TDM_PRINT1("->\tP_Port [%03d]\t ->\tL_Port [---]\n", phy_port);
				}
			}
		}
	}
	TDM_SML_BAR
}


/**
@name: tdm_vector_dump
@param:

Dumps current content of TDM vector map
 */
void
tdm_vector_dump( tdm_mod_t *_tdm )
{
#ifdef _SET_TDM_DEV
	int j, v;
	TDM_PRINT0("\n");
	TDM_PRINT0("           ");
	for (v=0; v<VEC_MAP_WID; v++) {
		TDM_PRINT1("%d",v);
		if ((v+1)<VEC_MAP_WID) {
			if ((v+1)/10==0) {
				TDM_PRINT0("___");
			}
			else {
				TDM_PRINT0("__");
			}
		}
	}
	TDM_PRINT0("\n");
	for (v=0; v<_tdm->_chip_data.soc_pkg.lr_idx_limit; v++) {
		TDM_PRINT1("\n %d___\t", v);
		for (j=0; j<VEC_MAP_WID && j<_tdm->_core_data.vmap_max_wid; j++) {
			if (_tdm->_core_data.vmap[j][v]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				TDM_PRINT1(" %03d", _tdm->_core_data.vmap[j][v]);
			}
			else {
				TDM_PRINT0(" ---");
			}
		}
	}
	TDM_PRINT0("\n");
#endif
}


/**
@name: tdm_print_vmap_vector
@param:

Dumps current content of TDM vector map
 */
void
tdm_print_vmap_vector( tdm_mod_t *_tdm )
{
	int j, v, token_empty, cal_len;
	cal_len     = _tdm->_chip_data.soc_pkg.lr_idx_limit + 
                  _tdm->_chip_data.soc_pkg.tvec_size;
    token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
	TDM_PRINT0("\n");
	TDM_PRINT0("           ");
	for (v=0; v<VEC_MAP_WID; v++) {
		TDM_PRINT1("%d",v);
		if ((v+1)<VEC_MAP_WID) {
			if ((v+1)/10==0) {
				TDM_PRINT0("___");
			}
			else {
				TDM_PRINT0("__");
			}
		}
	}
	TDM_PRINT0("\n");
	for (v=0; v<cal_len && v<VEC_MAP_LEN; v++) {
		TDM_PRINT1("\n %d___\t", v);
		for (j=0; j<VEC_MAP_WID && j<_tdm->_core_data.vmap_max_wid; j++) {
			if (_tdm->_core_data.vmap[j][v]!=token_empty) {
				TDM_PRINT1(" %03d", _tdm->_core_data.vmap[j][v]);
			}
			else {
				TDM_PRINT0(" ---");
			}
		}
	}
	TDM_PRINT0("\n");
}


/**
@name: tdm_vector_zrow
@param:

Dumps current content of TDM vector map at row 0
 */
void
tdm_vector_zrow( tdm_mod_t *_tdm )
{
	int j;
	
	TDM_PRINT0("\nVector Map Header Dump:\n 0___\t");
	for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
		if (_tdm->_core_data.vmap[j][0]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			TDM_PRINT1(" %03d", _tdm->_core_data.vmap[j][0]);
		}
		else {
			TDM_PRINT0(" ---");
		}
	}
	TDM_PRINT0("\n");
}
