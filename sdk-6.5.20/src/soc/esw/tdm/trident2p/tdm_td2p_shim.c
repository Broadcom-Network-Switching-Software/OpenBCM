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
@name: tdm_chip_td2p_shim_core
@param:

Downward abstraction layer between TDM.4 and TDM.5 API core data
 */
int 
tdm_chip_td2p_shim_core( tdm_mod_t *_tdm, tdm_chip_legacy_t *_tdm_td2p)
{
	int idx1, idx2;
	
	if (_tdm==NULL || _tdm_td2p==NULL) {return FAIL;}
	
	/* port map */
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_td2p->pmap[idx1][idx2] = _tdm->_chip_data.soc_pkg.pmap[idx1][idx2];
		}
	}
	/* speed */
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_td2p->tdm_globals.speed[idx1] = _tdm->_chip_data.soc_pkg.speed[idx1];
	}
	/* freq */
	_tdm_td2p->tdm_globals.clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
	/* state : Realign port state array to old specification */
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_td2p->tdm_globals.port_rates_array[idx1] = _tdm->_chip_data.soc_pkg.state[idx1];
	}
	for (idx1=0; idx1<(TD2P_NUM_EXT_PORTS-1); idx1++) {
		_tdm_td2p->tdm_globals.port_rates_array[idx1] = _tdm_td2p->tdm_globals.port_rates_array[idx1+1];
	}
	/* encap_type */
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		if (idx1%TD2P_NUM_PM_LNS==0 && (idx1/TD2P_NUM_PM_LNS)<TD2P_NUM_PM_MOD) {
			_tdm_td2p->tdm_globals.pm_encap_type[idx1/TD2P_NUM_PM_LNS] = (_tdm->_chip_data.soc_pkg.state[idx1]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[idx1]==PORT_STATE__OVERSUB_HG)?(PM_ENCAP__HIGIG2):(PM_ENCAP__ETHRNT);
		}
	}
	
	return PASS;
}


/**
@name: tdm_chip_td2p_shim__ing_wrap
@param:

Downward abstraction layer between TDM.4 and TDM.5 API ingress scheduler
 */
int
tdm_chip_td2p_shim__ing_wrap( tdm_mod_t *_tdm, tdm_chip_legacy_t *_tdm_td2p)
{
	if (FAIL == tdm_chip_td2p_shim_core(_tdm, _tdm_td2p)) {return FAIL;}

	TDM_MSET(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_x0,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_x0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_spacing_x0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_x1,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_x1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_spacing_x1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_y0,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_y0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_spacing_y0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_y1,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_y1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p->tdm_pgw.ovs_spacing_y1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);	
	
	return PASS;
	
}


/**
@name: tdm_chip_td2p_shim__egr_wrap
@param:

Downward abstraction layer between TDM.4 and TDM.5 API egress scheduler
 */
int
tdm_chip_td2p_shim__egr_wrap( tdm_mod_t *_tdm, tdm_chip_legacy_t *_tdm_td2p )
{
	if (FAIL == tdm_chip_td2p_shim_core(_tdm, _tdm_td2p)) {return FAIL;}

	TDM_COPY(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_x0,_tdm->_chip_data.cal_0.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_x0,_tdm->_chip_data.cal_0.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_x1,_tdm->_chip_data.cal_1.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_x1,_tdm->_chip_data.cal_1.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_y0,_tdm->_chip_data.cal_2.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_y0,_tdm->_chip_data.cal_2.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.pgw_tdm_tbl_y1,_tdm->_chip_data.cal_3.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p->tdm_pgw.ovs_tdm_tbl_y1,_tdm->_chip_data.cal_3.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);	
	
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_x.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p->tdm_pipe_table_y.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);	
	
	
	return PASS;
	
}


/**
@name: tdm_chip_td2p_shim__which_tsc
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
Only returns enough of TDM.5 style struct to drive scan functions, do not use as class
 */
tdm_mod_t*
tdm_chip_td2p_shim__which_tsc( int port, int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}

	_tdm_s->_chip_data.soc_pkg.num_ext_ports = TD2P_NUM_EXT_PORTS;
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	
	_tdm_s->_core_data.vars_pkg.port = port;
	
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__check_ethernet
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
 */
tdm_mod_t*
tdm_chip_td2p_shim__check_ethernet( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS], int traffic[TD2P_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(TD2P_NUM_EXT_PORTS*sizeof(int *), "port speed list");
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	for (idx1=0; idx1<TD2P_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[idx1] = traffic[idx1];
	}
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__check_ethernet_d
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
 */
tdm_mod_t*
tdm_chip_td2p_shim__check_ethernet_d( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int **tsc, int traffic[TD2P_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(TD2P_NUM_EXT_PORTS*sizeof(int *), "port speed list");
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	for (idx1=0; idx1<TD2P_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[idx1] = traffic[idx1];
	}	
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__scan_which_tsc_alloc
@param:
 */
tdm_mod_t*
tdm_chip_td2p_shim__scan_which_tsc_alloc( int port, int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}

	_tdm_s->_chip_data.soc_pkg.num_ext_ports = TD2P_NUM_EXT_PORTS;
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	
	_tdm_s->_core_data.vars_pkg.port = port;
	
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__scan_which_tsc_free
@param:
 */
int
tdm_chip_td2p_shim__scan_which_tsc_free(tdm_mod_t *_tdm)
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
@name: tdm_chip_td2p_shim__check_pipe_ethernet_alloc
@param:
 */
tdm_mod_t*
tdm_chip_td2p_shim__check_pipe_ethernet_alloc( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int **tsc, int traffic[TD2P_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(TD2P_NUM_EXT_PORTS*sizeof(enum port_speed_e), "port speed list");
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	for (idx1=0; idx1<TD2P_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[idx1] = traffic[idx1];
	}	
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
}


/**
@name: tdm_chip_td2p_shim__check_pipe_ethernet_free
@param:
 */
int
tdm_chip_td2p_shim__check_pipe_ethernet_free(tdm_mod_t *_tdm)
{
	int idx1;
	tdm_mod_t *_tdm_s=_tdm;
	/* speed */
	TDM_FREE(_tdm_s->_chip_data.soc_pkg.speed);
	/* pmap */
	for(idx1=0;idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		TDM_FREE(_tdm_s->_chip_data.soc_pkg.pmap[idx1]);
	}
	TDM_FREE(_tdm_s->_chip_data.soc_pkg.pmap);
	/* _tdm */
	TDM_FREE(_tdm_s);
	
	return PASS;
	
}
