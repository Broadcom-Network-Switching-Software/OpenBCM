/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip to core API shim layer
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_chip_ap_shim_core
@param:

Downward abstraction layer between TDM.4 and TDM.5 API core data
 */
int 
tdm_chip_ap_shim_core( tdm_mod_t *_tdm, tdm_ap_chip_legacy_t *_tdm_ap)
{
	int idx1, idx2;
	
	if (_tdm==NULL || _tdm_ap==NULL) {return FAIL;}
	
	/* port map */
	for (idx1=0; idx1<AP_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<AP_NUM_PM_LNS; idx2++) {
			_tdm_ap->pmap[idx1][idx2] = _tdm->_chip_data.soc_pkg.pmap[idx1][idx2];
		}
	}
	/* speed */
	for (idx1=0; idx1<(AP_NUM_EXT_PORTS-56); idx1++) {
		_tdm_ap->tdm_globals.speed[idx1] = _tdm->_chip_data.soc_pkg.speed[idx1];
	}
	/* freq */
	_tdm_ap->tdm_globals.clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
	/* state : Realign port state array to old specification */
	for (idx1=0; idx1<(AP_NUM_EXT_PORTS-56); idx1++) {
		_tdm_ap->tdm_globals.port_rates_array[idx1] = _tdm->_chip_data.soc_pkg.state[idx1];
	}
	for (idx1=0; idx1<(AP_NUM_EXT_PORTS-57); idx1++) {
		_tdm_ap->tdm_globals.port_rates_array[idx1] = _tdm_ap->tdm_globals.port_rates_array[idx1+1];
	}
	/* encap_type */
	for (idx1=0; idx1<(AP_NUM_EXT_PORTS-56); idx1++) {
		if (idx1%AP_NUM_PM_LNS==0 && (idx1/AP_NUM_PM_LNS)<AP_NUM_PM_MOD) {
			_tdm_ap->tdm_globals.pm_encap_type[idx1/AP_NUM_PM_LNS] = (_tdm->_chip_data.soc_pkg.state[idx1]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[idx1]==PORT_STATE__OVERSUB_HG)?(PM_ENCAP__HIGIG2):(PM_ENCAP__ETHRNT);
		}
	}
 /*set : if clport is in 100G or 25G*/
  for (idx1=1; idx1<=AP_NUM_PHY_PORTS; idx1+=AP_NUM_PM_LNS) {
    _tdm_ap->tdm_globals.cl_flag = BOOL_FALSE;
    if( (idx1==AP_CL1_PORT  || idx1==AP_CL2_PORT || idx1==AP_CL4_PORT || idx1==AP_CL5_PORT ) && ( _tdm->_chip_data.soc_pkg.speed[idx1]==SPEED_100G || _tdm->_chip_data.soc_pkg.speed[idx1]==SPEED_50G || _tdm->_chip_data.soc_pkg.speed[idx1]==SPEED_25G) ) {
     _tdm_ap->tdm_globals.cl_flag = BOOL_TRUE;
      break;
    }
  }
 /*num_1g ports: required for f3_2 TDM*/
  _tdm_ap->tdm_globals.num_1g=0;
	for (idx1=0; idx1<(AP_NUM_EXT_PORTS-56); idx1++) {
		if(_tdm_ap->tdm_globals.speed[idx1] == SPEED_1G || _tdm_ap->tdm_globals.speed[idx1]==SPEED_2p5G) { _tdm_ap->tdm_globals.num_1g++ ;}
	}	
	return PASS;
}


/**
@name: tdm_chip_ap_shim__ing_wrap
@param:

Downward abstraction layer between TDM.4 and TDM.5 API ingress scheduler
 */
int
tdm_chip_ap_shim__ing_wrap( tdm_mod_t *_tdm, tdm_ap_chip_legacy_t *_tdm_ap)
{
	if (FAIL == tdm_chip_ap_shim_core(_tdm, _tdm_ap)) {return FAIL;}

	TDM_MSET(_tdm_ap->tdm_pgw.pgw_tdm_tbl_x0,AP_NUM_EXT_PORTS,AP_LR_LLS_LEN);
	TDM_MSET(_tdm_ap->tdm_pgw.ovs_tdm_tbl_x0,AP_NUM_EXT_PORTS,AP_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_ap->tdm_pgw.ovs_spacing_x0,AP_NUM_EXT_PORTS,AP_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pgw.pgw_tdm_tbl_x1,AP_NUM_EXT_PORTS,AP_LR_LLS_LEN);
	TDM_MSET(_tdm_ap->tdm_pgw.ovs_tdm_tbl_x1,AP_NUM_EXT_PORTS,AP_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_ap->tdm_pgw.ovs_spacing_x1,AP_NUM_EXT_PORTS,AP_OS_LLS_GRP_LEN);
	
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_pipe_main,AP_NUM_EXT_PORTS,AP_LR_VBS_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_0,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_1,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_2,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_3,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_4,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_5,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_6,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_7,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	
	return PASS;
	
}


/**
@name: tdm_chip_ap_shim__egr_wrap
@param:

Downward abstraction layer between TDM.4 and TDM.5 API egress scheduler
 */
int
tdm_chip_ap_shim__egr_wrap( tdm_mod_t *_tdm, tdm_ap_chip_legacy_t *_tdm_ap )
{
	if (FAIL == tdm_chip_ap_shim_core(_tdm, _tdm_ap)) {return FAIL;}

	TDM_COPY(_tdm_ap->tdm_pgw.pgw_tdm_tbl_x0,_tdm->_chip_data.cal_0.cal_main,sizeof(int)*AP_LR_LLS_LEN);
	TDM_COPY(_tdm_ap->tdm_pgw.ovs_tdm_tbl_x0,_tdm->_chip_data.cal_0.cal_grp[0],sizeof(int)*AP_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_ap->tdm_pgw.pgw_tdm_tbl_x1,_tdm->_chip_data.cal_1.cal_main,sizeof(int)*AP_LR_LLS_LEN);
	TDM_COPY(_tdm_ap->tdm_pgw.ovs_tdm_tbl_x1,_tdm->_chip_data.cal_1.cal_grp[0],sizeof(int)*AP_OS_LLS_GRP_LEN);
	
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_pipe_main,AP_NUM_EXT_PORTS,AP_LR_VBS_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_0,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_1,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_2,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_3,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_4,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_5,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_6,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_ap->tdm_pipe_table_x.tdm_ovs_grp_7,AP_NUM_EXT_PORTS,AP_OS_VBS_GRP_LEN);
	
	return PASS;
	
}


/**
@name: tdm_chip_ap_shim__which_tsc_alloc
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
Only returns enough of TDM.5 style struct to drive scan functions, do not use as class
 */
tdm_mod_t*
tdm_chip_ap_shim__which_tsc_alloc( int port, int tsc[AP_NUM_PHY_PM][AP_NUM_PM_LNS] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}

	_tdm_s->_chip_data.soc_pkg.num_ext_ports = AP_NUM_EXT_PORTS;
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = AP_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = AP_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<AP_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<AP_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
  /* to get mapping of mngmnt ports*/
	for (idx1=0; idx1<AP_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<AP_NUM_PM_LNS; idx2++) {
      if( (_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2]== 29 || 
           _tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2]== 33 ||
           _tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2]== 65 || 
           _tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2]== 69) && 
          (idx1 < 7 || (idx1>8 && idx1<16)) ) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = AP_NUM_EXT_PORTS;
      }
		}
	}
  
  
	
	_tdm_s->_core_data.vars_pkg.port = port;
	
	return _tdm_s;
	
}

/**
@name: tdm_chip_ap_shim__which_tsc_free
@param:
 */
int
tdm_chip_ap_shim__which_tsc_free(tdm_mod_t *_tdm)
{
	int idx1;
	tdm_mod_t *_tdm_s=_tdm;
	/* pmap */
	for(idx1=0;idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		TDM_FREE(_tdm_s->_chip_data.soc_pkg.pmap[idx1]);
	}
	TDM_FREE(_tdm_s->_chip_data.soc_pkg.pmap);
	/* _tdm */
	TDM_FREE(_tdm_s);
	
	return PASS;
	
}


/**
@name: tdm_chip_ap_shim__check_ethernet
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
 */
tdm_mod_t*
tdm_chip_ap_shim__check_ethernet( int port, enum port_speed_e speed[AP_NUM_EXT_PORTS], int tsc[AP_NUM_PHY_PM][AP_NUM_PM_LNS], int traffic[AP_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(AP_NUM_EXT_PORTS*sizeof(int *), "port speed list");
	for (idx1=0; idx1<AP_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = AP_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = AP_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<AP_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<AP_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	for (idx1=0; idx1<AP_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.ap.pm_encap_type[idx1] = traffic[idx1];
	}
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_ap_shim__check_ethernet
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
 */
tdm_mod_t*
tdm_chip_ap_shim__check_ethernet_d( int port, enum port_speed_e speed[AP_NUM_EXT_PORTS], enum port_state_e state[AP_NUM_EXT_PORTS], int **tsc, int traffic[AP_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;

	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(AP_NUM_EXT_PORTS*sizeof(int *), "port speed list");
	_tdm_s->_chip_data.soc_pkg.state=TDM_ALLOC(AP_NUM_EXT_PORTS*sizeof(int *), "port state list");
	for (idx1=0; idx1<AP_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
		_tdm_s->_chip_data.soc_pkg.state[idx1] = state[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = AP_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = AP_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}

	for (idx1=0; idx1<AP_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<AP_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	for (idx1=0; idx1<AP_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.ap.pm_encap_type[idx1] = traffic[idx1];
	}	
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_ap_shim__core_vbs_scheduler_ovs
@param:

Passthru function for oversub scheduling request from TD2/TD2+
 */
int
tdm_chip_ap_shim__core_vbs_scheduler_ovs( tdm_mod_t *_tdm )
{
	/* TD2/TD2+ doesn't use 25G and 50G */
	/*_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5=0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6=0;*/
	
	return tdm_core_vbs_scheduler_ovs(_tdm);
}
