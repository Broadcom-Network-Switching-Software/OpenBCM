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


/* ========================================  FLEX PORT APIs ==================================== */

/**
@name: tdm_th2_vbs_scheduler_ovs_flex_port_delta
@param:

Incremental FlexPort for OVS tables
 */
int
tdm_th2_vbs_scheduler_ovs_flex_port_delta( tdm_mod_t *_tdm )
{	
	tdm_calendar_t *cal;
	enum flexport_pm_state_e pm_flexport_state[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	th2_flxport_t _flxport;
	
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

	if(tdm_th2_ovs_get_pm_flexport_state(_tdm, pm_flexport_state))
	  TDM_PRINT1("OVS FlexPort: get FlexPort PM state in PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

	if(tdm_th2_ovs_apply_constraints_flexport(_tdm, pm_flexport_state))
	  TDM_PRINT1("OVS FlexPort: Apply constraints PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

	if(tdm_th2_ovs_part_halfpipe(_tdm))
	  TDM_PRINT1("OVS FlexPort: PM Half Pipe partition for PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	

		
	if(tdm_th2_flex_ovs_tables_init(_tdm, &_flxport)){
		TDM_PRINT1("OVS FlexPort: PIPE %d tdm_th2_flex_ovs_tables_init DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}
	
	/*tdm_th2_print_th2_flxport_t(_tdm, &_flxport);*/
		
	if(tdm_th2_flex_ovs_tables_remove(_tdm, &_flxport)){
		TDM_PRINT1("OVS FlexPort: PIPE %d tdm_th2_flex_ovs_tables_remove DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}	

	/*tdm_th2_print_th2_flxport_t(_tdm, &_flxport);*/
	
	if(tdm_th2_flex_ovs_tables_consolidate(_tdm, &_flxport)){
		TDM_PRINT1("OVS FlexPort: PIPE %d tdm_th2_flex_ovs_tables_consolidate DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}

	/*tdm_th2_print_th2_flxport_t(_tdm, &_flxport);*/
	
	if(tdm_th2_flex_ovs_tables_add_ports(_tdm, &_flxport)){	
		TDM_PRINT1("OVS FlexPort: PIPE %d tdm_th2_flex_ovs_tables_add_ports DONE\n", _tdm->_core_data.vars_pkg.cal_id);		
	}

	/*tdm_th2_print_th2_flxport_t(_tdm, &_flxport);*/
	
	if(tdm_th2_flex_ovs_tables_rebalance(_tdm, &_flxport)){	
		TDM_PRINT1("OVS FlexPort: PIPE %d tdm_th2_flex_ovs_tables_rebalance DONE\n", _tdm->_core_data.vars_pkg.cal_id);		
	}

	/*tdm_th2_print_th2_flxport_t(_tdm, &_flxport);*/

	if(tdm_th2_flex_ovs_tables_post(_tdm, &_flxport)){	
		TDM_PRINT1("OVS FlexPort: PIPE %d tdm_th2_flex_ovs_tables_post DONE\n", _tdm->_core_data.vars_pkg.cal_id);		
	}
	
	
	if(tdm_th2_ovs_pkt_shaper_flex_port(_tdm)) {
	  TDM_PRINT1("OVS FlexPort: partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}

	if(tdm_th2_ovs_map_pm_num_to_pblk_flex_port(_tdm, pm_flexport_state)) {
	  TDM_PRINT1("OVS FlexPort: partition for PIPE %d PM_NUM to PBLK DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}
	
	return PASS;

	
	
	
}




/**
@name: tdm_th2_vbs_scheduler_ovs
@param:

FlexPort for OVS tables
In each HalfPipe the OVS tables are generated from scratch (used as an alternative to tdm_th2_vbs_scheduler_ovs_flex_port_delta)
 */
int
tdm_th2_vbs_scheduler_ovs_flex_port( tdm_mod_t *_tdm )
{	
	tdm_calendar_t *cal;
	enum flexport_pm_state_e pm_flexport_state[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	
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

	if(tdm_th2_ovs_get_pm_flexport_state(_tdm, pm_flexport_state))
	  TDM_PRINT1("OVS FlexPort: get FlexPort PM state in PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

	if(tdm_th2_ovs_apply_constraints_flexport(_tdm, pm_flexport_state))
	  TDM_PRINT1("OVS FlexPort: Apply constraints PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);

	if(tdm_th2_ovs_part_halfpipe(_tdm))
	  TDM_PRINT1("OVS FlexPort: PM Half Pipe partition for PIPE %d DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_10G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_20G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_25G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_40G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_50G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}
	
	_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = SPEED_100G;
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 0;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num); 
	}
	_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = 1;
	if(tdm_th2_ovs_fill_group(_tdm)) {
	  TDM_PRINT2("OVS FlexPort: partition for PIPE %d HALF_PIPE=%d DONE\n", _tdm->_core_data.vars_pkg.cal_id,_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num);
	}

	if(tdm_th2_ovs_pkt_shaper_flex_port(_tdm)) {
	  TDM_PRINT1("OVS FlexPort: partition for PIPE %d PKT_SHAPER CALENDAR DONE\n", _tdm->_core_data.vars_pkg.cal_id);
	}

	
	return PASS;

	
	
	
}







/**
@name: tdm_th2_ovs_get_pm_flexport_state
@param:

Returns the PM flexport state within that pipe:
Values coding:
0 : PM down before and after FlexPort;
1 : PM not changed;
2 : PM changed but some ports are active during FlexPort;
3 : PM down - some ports are active before FlexPort; all ports are down after FlexPort
4 : PM up   - all ports are down before FlexPort; some ports are active after FlexPort
5 : PM down & up  - some ports are active before FlexPort; some ports are active after FlexPort; all ports are down during FlexPort

enum port_speed_indx_e { FLEXPORT_PM_DOWN_DOWN=0, FLEXPORT_PM_SAME=1, FLEXPORT_PM_SOME_SAME=2, FLEXPORT_PM_UP_DOWN=3, FLEXPORT_PM_DOWN_UP=4, FLEXPORT_PM_UP_UP=5};
 */
int
tdm_th2_ovs_get_pm_flexport_state( tdm_mod_t *_tdm, enum flexport_pm_state_e *pm_flexport_state)
{
	int pms_per_pipe, pipe_id;
	int pm_num, ln_num, indx;
	int phy_base_port, phy_port;
	int all_speed_match, some_speed_match, all_down_before, all_down_after;
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	indx = 0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		all_speed_match = 1;
		some_speed_match = 0;
		all_down_before = 1;
		all_down_after  = 1;
		_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = -1;
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			
			if((_tdm->_prev_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
			((_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
			 (_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
				all_down_before = 0;
			}
			
			if((_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
			((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
			 (_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) ) {
				all_down_after = 0;
			}
			
			if(	((_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ||
				(_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != _tdm->_prev_chip_data.soc_pkg.speed[phy_port]) ) {
					all_speed_match = 0;
			}
			if(	((_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				(_tdm->_prev_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] == _tdm->_prev_chip_data.soc_pkg.speed[phy_port]) ) {
					some_speed_match = 1;
			}
			TDM_PRINT7("tdm_th2_ovs_get_pm_flexport_state() phy_port=%d prev_speed=%dG new_speed=%dG  all_down_before=%d all_down_after=%d all_speed_match=%d some_speed_match=%d \n", 
				phy_port, _tdm->_prev_chip_data.soc_pkg.speed[phy_port]/1000, _tdm->_chip_data.soc_pkg.speed[phy_port]/1000,
				all_down_before, all_down_after, all_speed_match, some_speed_match);
			
		}
		
		if ((all_speed_match==1) && (all_down_before==1) && (all_down_after==1)) {
			/* 0 : PM down before and after FlexPort; */
			pm_flexport_state[indx] = FLEXPORT_PM_DOWN_DOWN;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - FLEXPORT_PM_DOWN_DOWN\n", pipe_id, pm_num);
		} else if ((all_speed_match==1) && (all_down_before==0) && (all_down_after==0)) {
			/* 1 : PM has active ports and its not changed; */
			pm_flexport_state[indx] = FLEXPORT_PM_SAME;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - FLEXPORT_PM_SAME\n", pipe_id, pm_num);
		} else if ((all_speed_match==0) && (some_speed_match==1) && (all_down_before==0) && (all_down_after==0)) {
			/* 2 : PM changed but some ports are active during FlexPort; */
			pm_flexport_state[indx] = FLEXPORT_PM_SOME_SAME;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - FLEXPORT_PM_SOME_SAME\n", pipe_id, pm_num);
		} else if ((all_down_before==0) && (all_down_after==1)) {
			/* 3 : PM down - some ports are active before FlexPort; all ports are down after FlexPort */
			pm_flexport_state[indx] = FLEXPORT_PM_UP_DOWN;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - FLEXPORT_PM_UP_DOWN\n", pipe_id, pm_num);
		} else if ((all_down_before==1) && (all_down_after==0)) {
			/* 4 : PM up   - all ports are down before FlexPort; some ports are active after FlexPort */
			pm_flexport_state[indx] = FLEXPORT_PM_DOWN_UP;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - FLEXPORT_PM_DOWN_UP\n", pipe_id, pm_num);
		} else if ((some_speed_match==0) && (all_down_before==0) && (all_down_after==0)) {
			/* 5 : PM down & up  - some ports are active before FlexPort; some ports are active after FlexPort; all ports are down during FlexPort */
			pm_flexport_state[indx] = FLEXPORT_PM_UP_UP;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - FLEXPORT_PM_UP_UP\n", pipe_id, pm_num);
		} else {
			pm_flexport_state[indx] = FLEXPORT_PM_DOWN_DOWN;
			TDM_PRINT2("tdm_th2_ovs_get_pm_flexport_state() PIPE %d pm_num=%d - def FLEXPORT_PM_DOWN_DOWN\n", pipe_id, pm_num);
		}
		indx++;
	}
	
	return PASS;
}





/**
@name: tdm_th2_ovs_apply_constraints
@param:

Partition OVS PMs into 2 halfpipes
 */
int
tdm_th2_ovs_apply_constraints_flexport( tdm_mod_t *_tdm, enum flexport_pm_state_e *pm_flexport_state)
{
	int pms_per_pipe, pipe_id;
	int pm_num, ln_num, i;
	
	int new_speed_en_mtx[TH2_NUM_PHY_PM][6]; /* [pm_indx][speed_indx]  0 if <pm_indx> has <speed_indx> speed*/
	int new_speed_en_mtx_reduced[6];
	int hp0_new_speed_en_mtx_reduced[6], hp1_new_speed_en_mtx_reduced[6];
	/* <speed_indx> 0: 10G; 1: 20G; 2: 25G; 3: 40G; 4: 50G; 5: 100G */
	int no_of_speeds_in_pipe;
	int phy_base_port, phy_port;
	int indx=0;
	int no_pms_hp[2];
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	/*TDM_PRINT2("tdm_th2_avs_part_halfpipe1() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/

	/* Firstly, assign HP to PMs that cannot be moved */
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = -1;
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		switch(pm_flexport_state[indx]) {
			/* This PM is down in new configuration */
			case FLEXPORT_PM_DOWN_DOWN :
			case FLEXPORT_PM_UP_DOWN   :
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = -1;
				break;
			/* This PM needs to be assigned during FlexPort */
			case FLEXPORT_PM_DOWN_UP :
			case FLEXPORT_PM_UP_UP   :
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 2;  /* OVS but not assigned yet */
				break;
			/* This PM is active during FlexPort and has already a HP assigned in old config */
			case FLEXPORT_PM_SAME :
			case FLEXPORT_PM_SOME_SAME   :
				_tdm->_core_data.vars_pkg.port = pm_num; /* use port to pass pm_num */
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = tdm_th2_ovs_flexport_find_hp(_tdm);
				break;
			default : break;
		}
		indx++;
	}


	/* Build speed_en_mtx matrix for old and new configurations */	
	for (i=0; i<6; i++)  {
		for (pm_num=0; pm_num<TH2_NUM_PHY_PM; pm_num++) {
			new_speed_en_mtx[pm_num][i]=0;
		}
		new_speed_en_mtx_reduced[i]=0;
		hp0_new_speed_en_mtx_reduced[i]=0;
		hp1_new_speed_en_mtx_reduced[i]=0;
	}
	
	no_pms_hp[0] = 0; no_pms_hp[1] = 0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
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
				switch(_tdm->_chip_data.soc_pkg.speed[phy_port]) {
					case SPEED_10G : 
						new_speed_en_mtx[pm_num][INDX_10G] = 1; break;
					case SPEED_20G : 
						new_speed_en_mtx[pm_num][INDX_20G] = 1; break;
					case SPEED_25G : 
						new_speed_en_mtx[pm_num][INDX_25G] = 1; break;
					case SPEED_40G : 
						new_speed_en_mtx[pm_num][INDX_40G] = 1; break;
					case SPEED_50G : 
						new_speed_en_mtx[pm_num][INDX_50G] = 1; break;
					case SPEED_100G : 
						new_speed_en_mtx[pm_num][INDX_100G] = 1; break;
					default : break;
				}
			}
		}
		for(i=0; i<6; i++) {
			new_speed_en_mtx_reduced[i] = (new_speed_en_mtx[pm_num][i]==1) ? 1 : new_speed_en_mtx_reduced[i];
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num]==0) {
				hp0_new_speed_en_mtx_reduced[i] = (new_speed_en_mtx[pm_num][i]==1) ? 1 : hp0_new_speed_en_mtx_reduced[i];
			}
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num]==1) {
				hp1_new_speed_en_mtx_reduced[i] = (new_speed_en_mtx[pm_num][i]==1) ? 1 : hp1_new_speed_en_mtx_reduced[i];
			}
		}
		if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num]==0) {
			no_pms_hp[0]++;
		}
		if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num]==1) {
			no_pms_hp[1]++;
		}
	}
	
	
	/* Apply constraints */
	
	no_of_speeds_in_pipe=0;
	for (i=0; i<6; i++)  {
		if (new_speed_en_mtx_reduced[i]>0) {
			no_of_speeds_in_pipe++;
		}
	}
	
	if (no_of_speeds_in_pipe > 5) {
	/* Restriction 13:No port configurations with more than 4 port speed classes are supported. */
		TDM_ERROR2("tdm_th2_ovs_apply_constraints_flexport() PIPE %d No OVS port configurations with more than 4 port speed classes are supported; no_of_speeds_in_pipe=%d\n", pipe_id, no_of_speeds_in_pipe);
	
	}
	else if (no_of_speeds_in_pipe == 4) {
	/*Restriction 14: The only supported port configurations with 4 port speed classes are:
		10G/20G/40G/100G
		10G/25G/50G/100G
	*/
		for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] != 2) {
				continue; /* Applies only to nonallocated PMs */
			}
			/* check if PM cannot be placed in  HP 0 due to Restriction 14*/
			if (	((new_speed_en_mtx[pm_num][INDX_20G] || new_speed_en_mtx[pm_num][INDX_40G]) && (hp0_new_speed_en_mtx_reduced[INDX_25G] || hp0_new_speed_en_mtx_reduced[INDX_50G])) ||
				((new_speed_en_mtx[pm_num][INDX_25G] || new_speed_en_mtx[pm_num][INDX_50G]) && (hp0_new_speed_en_mtx_reduced[INDX_20G] || hp0_new_speed_en_mtx_reduced[INDX_40G])) ) {
				if (no_pms_hp[1] < (pms_per_pipe/2)) {
					TDM_PRINT2("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 14 for pm_num=%d and HalfPipe 0 \n",pipe_id, pm_num);
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
					no_pms_hp[1]++;
				} else {
					TDM_PRINT3("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 14 for pm_num=%d and HalfPipe 0; HalfPipe 1 has >= than %d PMs\n",pipe_id, pm_num, (pms_per_pipe/2));
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
					no_pms_hp[0]++;
				}
			}
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] != 2) {
				continue; /* Applies only to nonallocated PMs */
			}
			/* check if PM cannot be placed in  HP 1 due to Restriction 14*/
			if (	((new_speed_en_mtx[pm_num][INDX_20G] || new_speed_en_mtx[pm_num][INDX_40G]) && (hp1_new_speed_en_mtx_reduced[INDX_25G] || hp1_new_speed_en_mtx_reduced[INDX_50G])) ||
				((new_speed_en_mtx[pm_num][INDX_25G] || new_speed_en_mtx[pm_num][INDX_50G]) && (hp1_new_speed_en_mtx_reduced[INDX_20G] || hp1_new_speed_en_mtx_reduced[INDX_40G])) ) {
				if (no_pms_hp[0] < (pms_per_pipe/2)) {
					TDM_PRINT2("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 14 for pm_num=%d and HalfPipe 1 \n",pipe_id, pm_num);
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
					no_pms_hp[0]++;
				} else {
					TDM_PRINT3("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 14 for pm_num=%d and HalfPipe 1; HalfPipe 0 has >= than %d PMs\n",pipe_id, pm_num, (pms_per_pipe/2));
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
					no_pms_hp[1]++;
				}
			}
		}
	} 
	else if (no_of_speeds_in_pipe > 1) {
	/*Restriction 15: All port configurations with 1-3 port speed classes are supported, except
		configurations that contain both 20G and 25G port speeds.*/
		for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] != 2) {
				continue; /* Applies only to nonallocated PMs */
			}
			/* check if PM cannot be placed in  HP 0 due to Restriction 14*/
			if (	(new_speed_en_mtx[pm_num][INDX_20G] && hp0_new_speed_en_mtx_reduced[INDX_25G]) ||
				(new_speed_en_mtx[pm_num][INDX_25G] && hp0_new_speed_en_mtx_reduced[INDX_20G]) ) {
				if (no_pms_hp[1] < (pms_per_pipe/2)) {
					TDM_PRINT2("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 15 for pm_num=%d and HalfPipe 0 \n",pipe_id, pm_num);
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
					no_pms_hp[1]++;
				} else {
					TDM_PRINT3("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 15 for pm_num=%d and HalfPipe 0; HalfPipe 1 has >= than %d PMs\n",pipe_id, pm_num, (pms_per_pipe/2));
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
					no_pms_hp[0]++;
				}
			}
			if (_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] != 2) {
				continue; /* Applies only to nonallocated PMs */
			}
			/* check if PM cannot be placed in  HP 1 due to Restriction 14*/
			if (	(new_speed_en_mtx[pm_num][INDX_20G] && hp1_new_speed_en_mtx_reduced[INDX_25G]) ||
				(new_speed_en_mtx[pm_num][INDX_25G] && hp1_new_speed_en_mtx_reduced[INDX_20G]) ) {
				if (no_pms_hp[0] < (pms_per_pipe/2)) {
					TDM_PRINT2("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 15 for pm_num=%d and HalfPipe 1 \n",pipe_id, pm_num);
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 0;
					no_pms_hp[0]++;
				} else {
					TDM_PRINT3("tdm_th2_ovs_apply_constraints_flexport() PIPE %d applying Restriction 15 for pm_num=%d and HalfPipe 1; HalfPipe 0 has >= than %d PMs\n",pipe_id, pm_num, (pms_per_pipe/2));
					_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num] = 1;
					no_pms_hp[1]++;
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
@name: tdm_th2_ovs_flexport_find_hp
@param:

Find PMs Half Pipe in old configuration
 */
int
tdm_th2_ovs_flexport_find_hp( tdm_mod_t *_tdm)
{
	int pipe_id;
	tdm_calendar_t *cal;
	int pm_num, pm_num_t;
	int i, j;
	
	pm_num = _tdm->_core_data.vars_pkg.port;
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	cal=(&(_tdm->_prev_chip_data.cal_0)); break;
		case 1:	cal=(&(_tdm->_prev_chip_data.cal_1)); break;
		case 2:	cal=(&(_tdm->_prev_chip_data.cal_2)); break;
		case 3:	cal=(&(_tdm->_prev_chip_data.cal_3)); break;
		case 4:	cal=(&(_tdm->_prev_chip_data.cal_4)); break;
		case 5:	cal=(&(_tdm->_prev_chip_data.cal_5)); break;
		case 6:	cal=(&(_tdm->_prev_chip_data.cal_6)); break;
		case 7:	cal=(&(_tdm->_prev_chip_data.cal_7)); break;
		default:													
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);		
			return (TDM_EXEC_CORE_SIZE+1);							
	}	
	
	
	for (i=0; i<_tdm->_chip_data.cal_0.grp_num; i++) {
		for (j=0; j<_tdm->_chip_data.cal_0.grp_len; j++) {
			if (cal->cal_grp[i][j] != _tdm->_chip_data.soc_pkg.num_ext_ports) {
				_tdm->_core_data.vars_pkg.port = cal->cal_grp[i][j];
				pm_num_t = tdm_find_pm(_tdm);
				if (pm_num == pm_num_t) {
					return (i/(_tdm->_chip_data.cal_0.grp_num/2));
				}
			}
		}
	}
	
	TDM_ERROR2("tdm_th2_ovs_flexport_find_hp() PIPE %d pm_num=%d should already be assigned \n",pipe_id, pm_num);
	return 0;
}









/**
@name: tdm_th2_ovs_pkt_shaper_flex_port
@param:

Removes and add new ports to Pkt shaper calendar
 */
int
tdm_th2_ovs_pkt_shaper_flex_port( tdm_mod_t *_tdm )
{
	tdm_calendar_t *old_cal, *new_cal;
	int pms_per_pipe, pipe_id;
	int pm_num, ln_num, i;
	int phy_base_port, phy_port;
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
		
	/*TDM_PRINT2("tdm_th2_ovs_pkt_shaper() pipe_id=%d pms_per_pipe=%d \n",pipe_id, pms_per_pipe);*/


	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	old_cal=(&(_tdm->_prev_chip_data.cal_0)); new_cal=(&(_tdm->_chip_data.cal_0)); break;
		case 1:	old_cal=(&(_tdm->_prev_chip_data.cal_1)); new_cal=(&(_tdm->_chip_data.cal_1)); break;
		case 2:	old_cal=(&(_tdm->_prev_chip_data.cal_2)); new_cal=(&(_tdm->_chip_data.cal_2)); break;
		case 3:	old_cal=(&(_tdm->_prev_chip_data.cal_3)); new_cal=(&(_tdm->_chip_data.cal_3)); break;
		case 4:	old_cal=(&(_tdm->_prev_chip_data.cal_4)); new_cal=(&(_tdm->_chip_data.cal_4)); break;
		case 5:	old_cal=(&(_tdm->_prev_chip_data.cal_5)); new_cal=(&(_tdm->_chip_data.cal_5)); break;
		case 6:	old_cal=(&(_tdm->_prev_chip_data.cal_6)); new_cal=(&(_tdm->_chip_data.cal_6)); break;
		case 7:	old_cal=(&(_tdm->_prev_chip_data.cal_7)); new_cal=(&(_tdm->_chip_data.cal_7)); break;
		default:													
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);		
			return (TDM_EXEC_CORE_SIZE+1);							
	}

	/* Copy old pkt scheduler in the new pkt scheduler */
	for (i=0; i<SHAPING_GRP_LEN; i++) {
		new_cal->cal_grp[SHAPING_GRP_IDX_0][i] = old_cal->cal_grp[SHAPING_GRP_IDX_0][i];
		new_cal->cal_grp[SHAPING_GRP_IDX_1][i] = old_cal->cal_grp[SHAPING_GRP_IDX_1][i];
	}

	/* Remove ports that are going down before FlexPort */
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			/* If old speed for phy_port was not zero and port went down or changed to another speed then this port needs to be removed from pkt_scheduler before FlexPort */
			if (	((_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				(_tdm->_prev_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != _tdm->_prev_chip_data.soc_pkg.speed[phy_port]) ) {
					_tdm->_core_data.vars_pkg.port = phy_port;
					tdm_th2_ovs_pkt_shaper_flexport_remove_port(_tdm);
			}
		}
	}


	/* Add ports that are coming up after FlexPort */
	/* These are the ports that were 0 speed before FlexPort and now are !=0 or the speed changed*/
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			/* If new speed for phy_port is not zero and port changed to another speed then this port needs to be added to pkt_scheduler */
			if (	((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != _tdm->_prev_chip_data.soc_pkg.speed[phy_port]) ) {
					_tdm->_core_data.vars_pkg.port = phy_port;
					tdm_th2_ovs_pkt_shaper_per_port(_tdm );
			}
		}
	}
	
	return PASS;
}


/**
@name: tdm_th2_ovs_pkt_shaper_flexport_remove_port
@param:

Removes ports from Pkt shaper calendar
 */
int
tdm_th2_ovs_pkt_shaper_flexport_remove_port( tdm_mod_t *_tdm )
{
	int phy_port;
	int i;
	tdm_calendar_t *cal; /* That's new calendar*/
	
	phy_port = _tdm->_core_data.vars_pkg.port;
	
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

	/* Since the info about which HP this old port belongs is lost search and remove it from either pkt_schedulers */
	for (i=0; i<SHAPING_GRP_LEN; i++) {
		if (cal->cal_grp[SHAPING_GRP_IDX_0][i] == phy_port) {
			cal->cal_grp[SHAPING_GRP_IDX_0][i] = _tdm->_chip_data.soc_pkg.num_ext_ports;
		}
		if (cal->cal_grp[SHAPING_GRP_IDX_1][i] == phy_port) {
			cal->cal_grp[SHAPING_GRP_IDX_1][i] = _tdm->_chip_data.soc_pkg.num_ext_ports;
		}
	}

	return PASS;
}



/*=============================  FLEX PORT  OVS TABLES INCREMENTAL   ===================================*/



/**
@name: tdm_th2_flex_ovs_tables_init
@param:

STEP0. Initialize 
ovs_grp_speed & ovs_grp_weight based on _prev_chip_data
Evaluate prev_num_grps[] & new_num_grps[]
Evaluate ports_to_be_removed[] & ports_to_be_added[]
 */

int tdm_th2_flex_ovs_tables_init( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int i, j, pipe_id, half_pipe_num, pms_per_pipe, hp_num, hp_indx, ln_num;
	int grp_speed, grp_speed_indx;
	int pm_num, phy_base_port, phy_port;
	int pm_num_subports;
	tdm_calendar_t *old_cal;

	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;



	/*  Compute _flxport->new_num_grps[hp_num][grp_speed_indx] for the whole PIPE */
	for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
		int speed_max_num_ports_per_pm;
		int pms_with_grp_speed[2] = {0, 0};
		
		grp_speed = tdm_th2_vbs_scheduler_ovs_indx_to_speed(grp_speed_indx);
		
		switch (grp_speed_indx) {
			case INDX_10G:	speed_max_num_ports_per_pm=4; break;
			case INDX_20G:	speed_max_num_ports_per_pm=2; break;
			case INDX_40G:	speed_max_num_ports_per_pm=2; break;
			case INDX_25G:	speed_max_num_ports_per_pm=4; break;
			case INDX_50G:	speed_max_num_ports_per_pm=2; break;
			case INDX_100G: speed_max_num_ports_per_pm=1; break;
                        /* COVERITY
                         * This default is unreachable. It is kept intentionally as a defensive default for future development.
                         */
                        /* coverity[dead_error_begin] */
			default:        speed_max_num_ports_per_pm=1; break;
		}
		
		
		for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
			phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
			pm_num_subports = 0;
			half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
			if ((half_pipe_num==0) || (half_pipe_num==1)) {
				for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
					phy_port = phy_base_port+ln_num;
					if(_tdm->_chip_data.soc_pkg.speed[phy_port] == grp_speed) {
						/*TDM_PRINT4("tdm_th2_ovs_fill_group2() pm_num=%d ln_num=%d port=%d half_pipe=%d\n",pm_num, ln_num, phy_port, half_pipe_num);*/
						pm_num_subports++;
					}
				}
				if (pm_num_subports > 0) {
					pms_with_grp_speed[half_pipe_num]++;
				}
				
			}
		}

		for (hp_num=0; hp_num<2; hp_num++) {
			_flxport->new_num_grps[hp_num][grp_speed_indx] = 0;
			if (pms_with_grp_speed[hp_num]!=0) {
				_flxport->new_num_grps[hp_num][grp_speed_indx] = ((speed_max_num_ports_per_pm*pms_with_grp_speed[hp_num]) + TH2_OS_VBS_GRP_LEN-1) / TH2_OS_VBS_GRP_LEN;
			}
			_flxport->prev_num_grps[hp_num][grp_speed_indx] = 0;
		}
		
	}

	/*  Compute _flxport->prev_num_grps[hp_num][grp_speed_indx] for the whole PIPE */
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	old_cal=(&(_tdm->_prev_chip_data.cal_0)); break;
		case 1:	old_cal=(&(_tdm->_prev_chip_data.cal_1)); break;
		case 2:	old_cal=(&(_tdm->_prev_chip_data.cal_2)); break;
		case 3:	old_cal=(&(_tdm->_prev_chip_data.cal_3)); break;
		case 4:	old_cal=(&(_tdm->_prev_chip_data.cal_4)); break;
		case 5:	old_cal=(&(_tdm->_prev_chip_data.cal_5)); break;
		case 6:	old_cal=(&(_tdm->_prev_chip_data.cal_6)); break;
		case 7:	old_cal=(&(_tdm->_prev_chip_data.cal_7)); break;
		default:													
			TDM_PRINT1("tdm_th2_flex_ovs_tables_init() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);		
			return (TDM_EXEC_CORE_SIZE+1);							
	}


	/*  Copy old OVS tables over new OVS tables  */
	for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
		hp_num = i / (TH2_OS_VBS_GRP_NUM/2);
		hp_indx = i % (TH2_OS_VBS_GRP_NUM/2);
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			_flxport->ovs_tables[hp_num][hp_indx][j] = old_cal->cal_grp[i][j];
		}
		
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			_flxport->ovs_grp_pms[hp_num][hp_indx][j] = -1; /* -1 means no PM assigned*/
		}
	}


	for (i=0; i<TH2_NUM_EXT_PORTS; i++) {
		_flxport->ports_to_be_removed[i] = 0;
		_flxport->ports_to_be_added[i] = 0;
	}

	/* Init _flxport->ports_to_be_removed _flxport->ports_to_be_added*/
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			_flxport->ports_to_be_removed[phy_port] = 0;
			/* If old speed for phy_port was not zero and port went down or changed to another speed then this port needs to be removed from pkt_scheduler before FlexPort */
			if (	((_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_prev_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				(_tdm->_prev_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != _tdm->_prev_chip_data.soc_pkg.speed[phy_port]) ) {
					_flxport->ports_to_be_removed[phy_port] = 1;
			}
			/* If new speed for phy_port is not zero and port changed to another speed then this port needs to be added to pkt_scheduler */
			_flxport->ports_to_be_added[phy_port] = 0;
			if (	((_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB) ||
				(_tdm->_chip_data.soc_pkg.state[phy_port-1] == PORT_STATE__OVERSUB_HG) ) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != SPEED_0) &&
				(_tdm->_chip_data.soc_pkg.speed[phy_port] != _tdm->_prev_chip_data.soc_pkg.speed[phy_port]) ) {
					_flxport->ports_to_be_added[phy_port] = 1;
			}
		}
	}

	tdm_th2_flex_ovs_tables_evaluate(_tdm, _flxport, 0);

	for (hp_num=0; hp_num<2; hp_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = hp_num;
		tdm_th2_flex_ovs_tables_update_grp_pms(_tdm, _flxport);
	}



	/* Check if there are speeds that do not need any change */
	for (hp_num=0; hp_num<2; hp_num++) {
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
			_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx] = 1;
		}
	}
	
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			
			if (_flxport->ports_to_be_added[phy_port] == 1) {
			
				grp_speed_indx = tdm_th2_vbs_scheduler_ovs_speed_to_indx(_tdm->_chip_data.soc_pkg.speed[phy_port]);
				
				hp_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
				_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx] = 0;
			}
				
			if (_flxport->ports_to_be_removed[phy_port] == 1) {
				
				grp_speed_indx = tdm_th2_vbs_scheduler_ovs_speed_to_indx(_tdm->_prev_chip_data.soc_pkg.speed[phy_port]);
				
				for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
					hp_num = i / (TH2_OS_VBS_GRP_NUM/2);
					hp_indx = i % (TH2_OS_VBS_GRP_NUM/2);
					for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
						if (phy_port==_flxport->ovs_tables[hp_num][hp_indx][j]) {
							_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx] = 0;
							break;
						}
					}
					if (_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx] == 0) {break;}
				}
			}
		}
	}


	return PASS;

}








/**
@name: tdm_th2_flex_ovs_tables_evaluate
@param:

evaluate ovs_tables
 */
int tdm_th2_flex_ovs_tables_evaluate( tdm_mod_t *_tdm, th2_flxport_t *_flxport, int only_weight)
{
	int i,j, hp_num, grp_speed_indx;
	int hp_indx;
	int phy_port;
	int grp_speed;

if (only_weight==0) {
	/*  Evaluate  ovs_grp_speed */
	for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
		hp_num = i/(TH2_OS_VBS_GRP_NUM/2);
		hp_indx = i%(TH2_OS_VBS_GRP_NUM/2);
		_flxport->ovs_grp_speed[hp_num][hp_indx] = SPEED_0;
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			if (_flxport->ovs_tables[hp_num][hp_indx][j] != TH2_NUM_EXT_PORTS) {
				phy_port = _flxport->ovs_tables[hp_num][hp_indx][j];
				_flxport->ovs_grp_speed[hp_num][hp_indx]  = _tdm->_prev_chip_data.soc_pkg.speed[phy_port];
				break;
			}
		}
	}
}

	/*  Evaluate  ovs_grp_weight */
	for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
		hp_num = i/(TH2_OS_VBS_GRP_NUM/2);
		hp_indx = i%(TH2_OS_VBS_GRP_NUM/2);
		_flxport->ovs_grp_weight[hp_num][hp_indx] = 0;
		if (_flxport->ovs_grp_speed[hp_num][hp_indx] == SPEED_0) continue;
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			if (_flxport->ovs_tables[hp_num][hp_indx][j] != TH2_NUM_EXT_PORTS) {
				phy_port = _flxport->ovs_tables[hp_num][hp_indx][j];
				_flxport->ovs_grp_weight[hp_num][hp_indx] += _flxport->ovs_grp_speed[hp_num][hp_indx];
			}
		}
	}

/*  Compute _flxport->prev_num_grps[hp_num][grp_speed_indx] for the whole PIPE */
if (only_weight==0) {
	for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
		
		grp_speed = tdm_th2_vbs_scheduler_ovs_indx_to_speed(grp_speed_indx);

		_flxport->prev_num_grps[0][grp_speed_indx]=0;
		_flxport->prev_num_grps[1][grp_speed_indx]=0;
		for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
			hp_num = i/(TH2_OS_VBS_GRP_NUM/2);
			hp_indx = i%(TH2_OS_VBS_GRP_NUM/2);
			if (_flxport->ovs_grp_speed[hp_num][hp_indx] == grp_speed) {
				_flxport->prev_num_grps[hp_num][grp_speed_indx]++;
			}
		}
	}
}
	return PASS;
}





/**
@name: tdm_th2_flex_ovs_tables_remove
@param:

STEP1. Remove ports: every time a port is removed the ovs_grp_weight[] is updated for the corresponding group
	Re-evaluate ovs_grp_speed[]
		ovs_grp_weight[]
 */
int tdm_th2_flex_ovs_tables_remove( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int pm_num, pipe_id, pms_per_pipe, phy_base_port, phy_port, ln_num, hp_num;

	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			if (_flxport->ports_to_be_removed[phy_port] == 1) {
				_tdm->_core_data.vars_pkg.port = phy_port;
				tdm_th2_flex_ovs_tables_remove_port(_tdm, _flxport);
				_flxport->ports_to_be_removed[phy_port] = 0;
			}
		}
	}

	for (hp_num=0; hp_num<2; hp_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num = hp_num;
		tdm_th2_flex_ovs_tables_update_grp_pms(_tdm, _flxport);
	}

	return PASS;
}



/**
@name: tdm_th2_flex_ovs_tables_remove_port
@param:

Remove one port
 */
int tdm_th2_flex_ovs_tables_remove_port( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int i, j, phy_port, hp_num, hp_indx;
	
	phy_port = _tdm->_core_data.vars_pkg.port;

	/*  Copy old OVS tables over new OVS tables  */
	for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
		hp_num = i/(TH2_OS_VBS_GRP_NUM/2);
		hp_indx = i%(TH2_OS_VBS_GRP_NUM/2);
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			if (_flxport->ovs_tables[hp_num][hp_indx][j] == phy_port) {
				_flxport->ovs_tables[hp_num][hp_indx][j] = TH2_NUM_EXT_PORTS;
				_flxport->ovs_grp_weight[hp_num][hp_indx] -= _flxport->ovs_grp_speed[hp_num][hp_indx];
				if (_flxport->ovs_grp_weight[hp_num][hp_indx]==0) {
					_flxport->ovs_grp_speed[hp_num][hp_indx] = SPEED_0;
				}
				TDM_PRINT5("tdm_th2_flex_ovs_tables_remove_port() grp_speed=%dG hp_num=%d hp_ind=%d grp_indx=%d Remove port=%d\n", 
				_flxport->ovs_grp_speed[hp_num][hp_indx]/1000, hp_num, hp_indx, j, phy_port);
			}
		}
	}
	return PASS;
}








/* == WITHIN each Half Pipe DO: */


/* 
 Foreach speed if new_num_grps[speed] <= prev_num_grps[speed] DO:

STEP 2.A	Consolidate the speed in that Half Pipe - reduce number of groups
STEP 3.		Add new ports to the speed groups - with balance
STEP 4.		Rebalance if still necessary
*/


/* 
 Foreach speed if new_num_grps[speed] > prev_num_grps[speed] DO:

STEP 2.B	Consolidate the speed in that Half Pipe - increase number of groups
STEP 3.		Add new ports to the speed groups - with balance
STEP 4.		Rebalance if still necessary
*/



/*

*/




/**
@name: tdm_th2_flex_ovs_tables_consolidate
@param:

STEP 2.A	Consolidate the speed in that Half Pipe - reduce the number of groups and move ports with balance

	Sort ovs_grp_weight[] for that group speed and find (prev_num_grps[speed] -new_num_grps[speed]) groups with the smallest weight
	
	Move ports from these to be removed groups to ports_to_be_added[] array.
	
	After that all removed groups become empty ovs_grp_speed[grp]=SPEED_0

STEP 2.B	Consolidate the speed in that Half Pipe - increase the number of groups and move ports with balance

	Find (new_num_grps[speed]-prev_num_grps[speed]) groups available (with SPEED_0)
	
	Assign them the new grp speed.
 */
int tdm_th2_flex_ovs_tables_consolidate( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int hp_num, grp_speed_indx;
	int grp_speed;


	for (hp_num=0; hp_num<2; hp_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num=hp_num;
		
		/* Consolidate first for speeds that need to reduce the number of OVS groups*/
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
		
			if (_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx]==1) continue;
			
			grp_speed = tdm_th2_vbs_scheduler_ovs_indx_to_speed(grp_speed_indx);
			
			if (_flxport->new_num_grps[hp_num][grp_speed_indx] <= _flxport->prev_num_grps[hp_num][grp_speed_indx]) {
				_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = grp_speed;
				tdm_th2_flex_ovs_tables_consolidate_speed(_tdm, _flxport);
			}
		}

		/* Consolidate secondly the speeds that need to increase the number of OVS groups*/
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {

			if (_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx]==1) continue;
					
			grp_speed = tdm_th2_vbs_scheduler_ovs_indx_to_speed(grp_speed_indx);
		
			if (_flxport->new_num_grps[hp_num][grp_speed_indx] > _flxport->prev_num_grps[hp_num][grp_speed_indx]) {
				_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = grp_speed;
				tdm_th2_flex_ovs_tables_consolidate_speed(_tdm, _flxport);
			}
		}
		
		tdm_th2_flex_ovs_tables_update_grp_pms(_tdm, _flxport);
		
	}
	return PASS;
}

/**
@name: tdm_th2_flex_ovs_tables_consolidate_speed
@param:

Consolidate speed in one half pipe for a given speed

*/
int
tdm_th2_flex_ovs_tables_consolidate_speed( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int i, j, grp_speed_indx, phy_port;
	int ovs_grp_weight[TH2_OS_VBS_GRP_NUM/2];
	int ovs_grp_indx[TH2_OS_VBS_GRP_NUM/2];
	int hp_num, hp_indx;
	int grp_speed;
	int curr_num_grps;
	
	hp_num    = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;
	
	grp_speed_indx=tdm_th2_vbs_scheduler_ovs_speed_to_indx(grp_speed);
	
	if (_flxport->new_num_grps[hp_num][grp_speed_indx] == 0) {
		return PASS; /* Nothing to consolidate */
	}
	
	/* Get the current number of groups assigned to grp_speed in hp_num;
	 * Due to port removals some of the groups may be DOWN
	 */
	curr_num_grps = 0;
	for (hp_indx=0; hp_indx<TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
		if (_flxport->ovs_grp_speed[hp_num][hp_indx] == grp_speed) {
			curr_num_grps++;
		}
	}
	TDM_PRINT3("tdm_th2_flex_ovs_tables_consolidate_speed() hp_num=%0d grp_speed=%d curr_num_grps=%0d\n", hp_num, grp_speed/1000, curr_num_grps);
	
	/* If the number of groups after FlexPort needs to be less than curr number of groups then consolidate 
	 *  by moving active ports from the smallest groups to the biggest ones
	 */
	if (_flxport->new_num_grps[hp_num][grp_speed_indx] < curr_num_grps) {
	
		/* Find (curr_num_grps-new_num_grps[hp_num][grp_speed_indx]) with the smallest weights
		   Move ports from these to be removed groups to ports_to_be_added[] array. 
		   Make to be removed groups available */
		
		for (hp_indx=0; hp_indx<TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			ovs_grp_weight[hp_indx] = -1;
			ovs_grp_indx[hp_indx] = hp_indx;
			if (_flxport->ovs_grp_speed[hp_num][hp_indx] == grp_speed) {
				ovs_grp_weight[hp_indx] = _flxport->ovs_grp_weight[hp_num][hp_indx];
			}
		}
		
		tdm_th2_sort_desc(TH2_OS_VBS_GRP_NUM/2, ovs_grp_weight, ovs_grp_indx);
		
		
		for (i=0; i < (curr_num_grps - _flxport->new_num_grps[hp_num][grp_speed_indx]); i++) {
			hp_indx = ovs_grp_indx[_flxport->new_num_grps[hp_num][grp_speed_indx]+i];
			for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
				phy_port = _flxport->ovs_tables[hp_num][hp_indx][j];
				if (phy_port != TH2_NUM_EXT_PORTS) {
					_flxport->ports_to_be_added[phy_port] = 1;
					_tdm->_core_data.vars_pkg.port = phy_port;
					tdm_th2_flex_ovs_tables_remove_port(_tdm, _flxport);
				}
			}
			_flxport->ovs_grp_speed[hp_num][hp_indx] = SPEED_0; /* Make group available */
		}
	}
	
	/*  Add  _flxport->new_num_grps[hp_num][grp_speed_indx] - curr_num_grps  groups with grp_speed */
	if (_flxport->new_num_grps[hp_num][grp_speed_indx] > curr_num_grps) {
		
		for (i=0; i < (_flxport->new_num_grps[hp_num][grp_speed_indx] - curr_num_grps); i++) {
			for (hp_indx=0; hp_indx< TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
				if (_flxport->ovs_grp_speed[hp_num][hp_indx] == SPEED_0) {
					_flxport->ovs_grp_speed[hp_num][hp_indx] = grp_speed;
					break;
				}
			}
		}	
	}
	
	return PASS;
}




/**
@name: tdm_th2_flex_ovs_tables_consolidate_add_ports
@param:

Add new ports to the speed groups - with balance
	
*/
int
tdm_th2_flex_ovs_tables_add_ports( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int hp_num, grp_speed_indx;
	int grp_speed;


	for (hp_num=0; hp_num<2; hp_num++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num=hp_num;
		
		/* Consolidate first for speeds that need to reduce the number of OVS groups*/
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {

			if (_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx]==1) continue;
			
			grp_speed = tdm_th2_vbs_scheduler_ovs_indx_to_speed(grp_speed_indx);
			
			_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = grp_speed;
			tdm_th2_flex_ovs_tables_add_ports_speed(_tdm, _flxport);
		}
		
		/*tdm_th2_print_th2_flxport_t(_tdm, _flxport);*/
		
	}
	return PASS;
}


/**
@name: tdm_th2_flex_ovs_tables_consolidate_add_ports
@param:

Add new ports to the speed groups - with balance
	
	Sort PMs to be added in descending order of their number of active ports with this speed
	Add one PM at a time from ports_to_be_added[] to the groups with current speed with two conditions:
		a) add to the group with the smallest weight;
		b) add to a group if that group is not full (full doesn't mean that the group has all entries occupied but rather that there is no place for a new PM)	


	Use APIs like tdm_th2_ovs_move_pm( src_grp, dest_grp)  ??
	use a sot function on ovs_grp_weight[] array.

*/
int
tdm_th2_flex_ovs_tables_add_ports_speed( tdm_mod_t *_tdm, th2_flxport_t *_flxport )
{
	int pm_num, ln_num, hp_num, phy_base_port, phy_port, pipe_id;
	int grp_speed;
	int pm_weight[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_indx[TH2_NUM_PHY_PM/TH2_NUM_QUAD];
	int pm_i;
	int hp_indx;
	int i, j, pms_per_pipe, found_sister_port;
	int ovs_grp_weight[TH2_OS_VBS_GRP_NUM/2];
	int ovs_grp_indx[TH2_OS_VBS_GRP_NUM/2];
	int no_port_to_add;
	
	hp_num    = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;


	/* Init pm_weight[pm_num]*/
	no_port_to_add = 1;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		pm_i = pm_num-(pipe_id*pms_per_pipe);
		pm_weight[pm_i]=0;
		pm_indx[pm_i]=pm_num;

		if (hp_num != _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num]) continue;
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
			phy_port = phy_base_port+ln_num;
			if ((_flxport->ports_to_be_added[phy_port]==1) && (_tdm->_chip_data.soc_pkg.speed[phy_port]==grp_speed) ) {
				pm_weight[pm_i] += _tdm->_chip_data.soc_pkg.speed[phy_port];
				no_port_to_add=0;
			}
		}
	}

	if (no_port_to_add==1) {
		return PASS; /* STOP here since nothing to add */
	}
	
	/* First add ports that already have sister ports in OVS groups for that speed */
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		pm_i = pm_num-(pipe_id*pms_per_pipe);
		phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
		if (pm_weight[pm_i]==0) continue;
		/*  Check if this PM is allready in any speed groups */
		found_sister_port=0;
		for (hp_indx=0; hp_indx<TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			if (_flxport->ovs_grp_speed[hp_num][hp_indx] == grp_speed) {
				for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
					if (pm_num == _flxport->ovs_grp_pms[hp_num][hp_indx][j]) {
						found_sister_port=1;
						break;
					}
				}
				
				if (found_sister_port==1) {
					/* Add all ports from this PM with speed grp_speed into this hp_indx */
					tdm_th2_flex_ovs_tables_add_pm( _tdm, _flxport, pm_num, hp_indx);
					pm_weight[pm_i] = 0;
					break; /* Because all sister ports are in the same group for a given speed */
				}
			}
		}
	}
	
	
	/* Then add PMs that do not have any ports in OVS groups for that speed 
	Balance groups weight too */
	
	/* Sort PMs to be added with this speed in their descending weight*/
	/* Sort groups with this speed in their descending weight */
	
	/*LOOP:
		Add the greatest PM in the smallest group 
		Re-sort  groups with this speed in their descending weight
	 */
	
	/* Sort PMs with this speed in their descending order */
	tdm_th2_sort_desc(TH2_NUM_PHY_PM/TH2_NUM_QUAD, pm_weight, pm_indx);
	
	/* Main LOOP */
	for (i=0; i < TH2_NUM_PHY_PM/TH2_NUM_QUAD; i++) {
		int added;
		int num_groups_speed;
		
		if (pm_weight[i]==0) continue;
		pm_num = pm_indx[i];
		num_groups_speed=0;
		
		for (hp_indx=0; hp_indx<TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			ovs_grp_weight[hp_indx] = -1;
			ovs_grp_indx[hp_indx] = hp_indx;
			if (_flxport->ovs_grp_speed[hp_num][hp_indx] == grp_speed) {
				ovs_grp_weight[hp_indx] = _flxport->ovs_grp_weight[hp_num][hp_indx];
				num_groups_speed++;
			}
		}
		
		tdm_th2_sort_desc(TH2_OS_VBS_GRP_NUM/2, ovs_grp_weight, ovs_grp_indx);

		/* TRy to place pm_num in the smallest group */
		added=0;
		for (j=num_groups_speed-1 ; j>=0; j--) {
			if ( tdm_th2_flex_ovs_tables_add_pm( _tdm, _flxport, pm_num, ovs_grp_indx[j])==PASS ) {
				pm_weight[i] = 0;
				added=1;
				break;
			}
		}
		if (added==0) { TDM_ERROR3("tdm_th2_flex_ovs_tables_add_ports_speed() grp_speed=%d Cannot add pm_num=%d in hp_num=%d\n", grp_speed/1000, pm_num, hp_num); }
	}
	
	return PASS;
}



/**
@name: tdm_th2_flex_ovs_tables_update_grp_pms
@param:

*/
int
tdm_th2_flex_ovs_tables_update_grp_pms( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int i,j, pm_num, hp_num, hp_indx, phy_port;
	int pm_exists;


	hp_num    = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	/*grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;*/
	
	
	for (hp_indx=0; hp_indx< TH2_OS_VBS_GRP_NUM/2; hp_indx++) {	
		for (i=0; i<TH2_OS_VBS_GRP_LEN; i++) {
			_flxport->ovs_grp_pms[hp_num][hp_indx][i] = -1;
		}
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			phy_port = _flxport->ovs_tables[hp_num][hp_indx][j];
			if (phy_port != TH2_NUM_EXT_PORTS) { /* valid port */
				pm_num = (phy_port-1)/_tdm->_chip_data.soc_pkg.pmap_num_lanes;
				/*TDM_PRINT5("tdm_th2_flex_ovs_tables_update_grp_pms() hp_num=%d hp_indx=%d grp_indx=%d phy_port=%d pm_num=%d\n", 
					hp_num, hp_indx, j, phy_port, pm_num);*/
				pm_exists = 0;
				for (i=0; i<TH2_OS_VBS_GRP_LEN; i++) { /* check if exists */
					if (_flxport->ovs_grp_pms[hp_num][hp_indx][i] == pm_num) {
						pm_exists = 1;
						break;
					}
				}
				if (pm_exists==0) {	/* add pm_num to _flxport->ovs_grp_pms[hp_num][hp_indx] */
					for (i=0; i<TH2_OS_VBS_GRP_LEN; i++) {
						if (_flxport->ovs_grp_pms[hp_num][hp_indx][i] == -1) {
							_flxport->ovs_grp_pms[hp_num][hp_indx][i] = pm_num;
							break;
						}
					}
				}
			}
		}
	}
	
	return PASS;
}




/**
@name: tdm_th2_flex_ovs_tables_consolidate_add_ports
@param:

*/
int
tdm_th2_flex_ovs_tables_add_pm( tdm_mod_t *_tdm, th2_flxport_t *_flxport, int pm_num, int hp_indx )
{
	int j, hp_num, pm_exists;
	int grp_speed;
	int phy_base_port, ln_num, phy_port;
	int max_pms_per_group=0;
	int act_pms_per_group=0;

	hp_num    = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;

	pm_exists = 0;
	/* Add PM if it doesn't exist */
	for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
		if (_flxport->ovs_grp_pms[hp_num][hp_indx][j] == pm_num) {
			pm_exists = 1;
			break;
		}
	}
	
	/* if PM doesn't exist check if num of PMs is not exceeded in this group and, if not, add it */
	if (pm_exists == 0) {
		int speed_max_num_ports_per_pm=1;
		
		switch (grp_speed) {
			case SPEED_10G:  speed_max_num_ports_per_pm=4; break;
			case SPEED_20G:  speed_max_num_ports_per_pm=2; break;
			case SPEED_40G:  speed_max_num_ports_per_pm=2; break;
			case SPEED_25G:  speed_max_num_ports_per_pm=4; break;
			case SPEED_50G:  speed_max_num_ports_per_pm=2; break;
			case SPEED_100G: speed_max_num_ports_per_pm=1; break;
			default:													
				TDM_PRINT1("tdm_th2_flex_ovs_tables_add_pm() Invalid group speed grp_speed %0d\n",grp_speed/1000);
				return FAIL;							
		}
		
		max_pms_per_group = TH2_OS_VBS_GRP_LEN / speed_max_num_ports_per_pm;
		
		act_pms_per_group=0;
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			if (_flxport->ovs_grp_pms[hp_num][hp_indx][j] != -1) {
				act_pms_per_group++;
			}
		}
		
		if (act_pms_per_group >= max_pms_per_group) {
			TDM_PRINT4("tdm_th2_flex_ovs_tables_add_pm() __WARNING grp_speed=%d: Cannot add pm_num=%d in hp_num=%d hp_indx=%d\n", grp_speed/1000, pm_num, hp_num, hp_indx);
			return FAIL;
		} /*else {
			for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
				if (_flxport->ovs_grp_pms[hp_num][hp_indx][j] == -1) {
					_flxport->ovs_grp_pms[hp_num][hp_indx][j] = pm_num;
					break;
				}
			}
		}*/
	}
	
	/* Add ports here */ 
	phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
	for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
		phy_port = phy_base_port+ln_num;
		if ((_flxport->ports_to_be_added[phy_port]==1) && (_tdm->_chip_data.soc_pkg.speed[phy_port]==grp_speed) ) {
			for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
				if (_flxport->ovs_tables[hp_num][hp_indx][j] == TH2_NUM_EXT_PORTS) {
					_flxport->ovs_tables[hp_num][hp_indx][j] = phy_port;
					_flxport->ovs_grp_weight[hp_num][hp_indx] += grp_speed;
					_flxport->ports_to_be_added[phy_port] = 0;
					break;
				}
			}
		}
	}

	tdm_th2_flex_ovs_tables_update_grp_pms(_tdm, _flxport);

	TDM_PRINT6("tdm_th2_flex_ovs_tables_add_pm() grp_speed=%d: Add pm_num=%d in hp_num=%d hp_indx=%d act_pms_per_group=%d max_pms_per_group=%d\n", grp_speed/1000, pm_num, hp_num, hp_indx, act_pms_per_group, max_pms_per_group);

	return PASS;
}



/* STEP 4. Rebalance

   WHILE Loop:{
	Sort groups with a given speed in descending order
	Try to move a PM (with the smallest number of ports in the group) from the group with highest weight to the group with lowest weight. The destination group should have enough
	space for a new PM.
	If this results in a lower difference continue.
	Else, break
	}

Compute the group bandwidth of each oversub group for that speed, noting the group with the maximum
port bandwidth sum (Max_Group) and the group with the minimum port bandwidth sum (Min_Group).
Determine if it is possible to move a Port Macro from Max_Group to any other group for that speed that
would result in a lower difference between the Max_Group and Min_Group sums. The receiving group
must have sufficient empty entries for all the ports in the Port Macro, not just the enabled ports. Move
the Port Macro from the source group to the destination group.


*/

/**
@name: tdm_th2_flex_ovs_tables_rebalance
@param:

*/
int tdm_th2_flex_ovs_tables_rebalance( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int hp_num, grp_speed_indx;
	int grp_speed;


	for (hp_num=0; hp_num<2; hp_num++) {
		/* Rebalance per speed*/
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
		
			if (_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx]==1) continue;
		
			grp_speed = tdm_th2_vbs_scheduler_ovs_indx_to_speed(grp_speed_indx);
			
			/* If zero or one speed group in Half Pipe there is no need for rebalancing */
			if (_flxport->new_num_grps[hp_num][grp_speed_indx] <=1) continue;
			
			_tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num=hp_num;
			_tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed = grp_speed;
			tdm_th2_flex_ovs_tables_rebalance_speed(_tdm, _flxport);
		}
	}
	return PASS;
}

/**
@name: tdm_th2_flex_ovs_tables_rebalance_speed
@param:

*/
int tdm_th2_flex_ovs_tables_rebalance_speed( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	/* Its necessary only for speeds that requirea an increase in no of OVS groups*/
	int j, hp_num, hp_indx;
	int ovs_grp_weight[TH2_OS_VBS_GRP_NUM/2];
	int ovs_grp_indx[TH2_OS_VBS_GRP_NUM/2];
	int grp_speed_indx;
	int pm_moved;
	int grp_speed;

	hp_num    = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;
	
	grp_speed_indx = tdm_th2_vbs_scheduler_ovs_speed_to_indx(grp_speed);

	/* Main LOOP */
	do {
		for (hp_indx=0; hp_indx<TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			ovs_grp_weight[hp_indx] = -1;
			ovs_grp_indx[hp_indx] = hp_indx;
			if (_flxport->ovs_grp_speed[hp_num][hp_indx] == grp_speed) {
				ovs_grp_weight[hp_indx] = _flxport->ovs_grp_weight[hp_num][hp_indx];
			}
		}
		
		tdm_th2_sort_desc(TH2_OS_VBS_GRP_NUM/2, ovs_grp_weight, ovs_grp_indx);

		/* Try to move the smallest PM from the greatest group to a smaller group */
		pm_moved=0;
		for (j=_flxport->new_num_grps[hp_num][grp_speed_indx]-1 ; j>0; j--) {
			if ( tdm_th2_flex_ovs_tables_move_pm( _tdm, _flxport, ovs_grp_indx[0], ovs_grp_indx[j])==PASS ) {
				pm_moved=1;
				break;
			}
		}
	} while (pm_moved==1);


	return PASS;
}



/**
@name: tdm_th2_flex_ovs_tables_move_pm
@param:

*/
int tdm_th2_flex_ovs_tables_move_pm( tdm_mod_t *_tdm, th2_flxport_t *_flxport, int src_grp, int dst_grp)
{
	int i, j, hp_num;
	int grp_speed;
	int speed_max_num_ports_per_pm=1;
	int max_pms_per_group, act_pms_per_group;
	int pm_weight[TH2_OS_VBS_GRP_LEN];
	int pm_indx[TH2_OS_VBS_GRP_LEN];
	int pm_num, pm_num_t, pm_num_weight, num_grp_pms;
	int phy_port, diff_before,diff_after;

	hp_num    = _tdm->_chip_data.soc_pkg.soc_vars.th2.half_pipe_num;
	grp_speed = _tdm->_chip_data.soc_pkg.soc_vars.th2.grp_speed;
	
	
	TDM_PRINT4("tdm_th2_flex_ovs_tables_move_pm() grp_speed=%d hp_num=%d: Try moving a PM from src_grp=%d to dst_grp=%d\n", grp_speed/1000, hp_num, src_grp, dst_grp);
	
	/* Check to see if there is place for another PM in dst_grp*/
	switch (grp_speed) {
		case SPEED_10G:  speed_max_num_ports_per_pm=4; break;
		case SPEED_20G:  speed_max_num_ports_per_pm=2; break;
		case SPEED_40G:  speed_max_num_ports_per_pm=2; break;
		case SPEED_25G:  speed_max_num_ports_per_pm=4; break;
		case SPEED_50G:  speed_max_num_ports_per_pm=2; break;
		case SPEED_100G: speed_max_num_ports_per_pm=1; break;
		default:													
			TDM_PRINT1("tdm_th2_flex_ovs_tables_add_pm() Invalid group speed grp_speed %0d\n",grp_speed/1000);
			return FAIL;							
	}
	
	max_pms_per_group = TH2_OS_VBS_GRP_LEN / speed_max_num_ports_per_pm;
	
	act_pms_per_group=0;
	for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
		if (_flxport->ovs_grp_pms[hp_num][dst_grp][j] != -1) {
			act_pms_per_group++;
		}
	}
	
	if (act_pms_per_group >= max_pms_per_group) {
		TDM_PRINT6("tdm_th2_flex_ovs_tables_move_pm() __WARNING grp_speed=%d hp_num=%d: Cannot move a PM from src_grp=%d to dst_grp=%d max_pms_per_group=%d act_pms_per_group=%d\n", 
			grp_speed/1000, hp_num, src_grp, dst_grp, max_pms_per_group, act_pms_per_group);
		return FAIL;
	}
	
	
	/* Find the smallest PM in src_grp */
	num_grp_pms = 0;
	for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
		pm_weight[j] = 0;
		pm_indx[j] = -1;
		if (_flxport->ovs_grp_pms[hp_num][src_grp][j] != -1) {
			num_grp_pms++;
			pm_num = _flxport->ovs_grp_pms[hp_num][src_grp][j];
			pm_indx[j] = pm_num;
			/* Find pm weight */
			for (i=0; i< TH2_OS_VBS_GRP_LEN; i++) {
				phy_port = _flxport->ovs_tables[hp_num][src_grp][i];
				if (phy_port != TH2_NUM_EXT_PORTS) { /* valid port */
					pm_num_t = (phy_port-1)/_tdm->_chip_data.soc_pkg.pmap_num_lanes;
					if (pm_num == pm_num_t) {
						pm_weight[j] += grp_speed;
					}
				}
			}
		}
	}
	
	tdm_th2_sort_desc(TH2_OS_VBS_GRP_LEN, pm_weight, pm_indx);
	
	
	/* Check to see if by moving this PM to the dst_grp the weight difference will decrease */
	pm_num = pm_indx[num_grp_pms-1]; /* Thats the smallest PM */
	pm_num_weight = pm_weight[num_grp_pms-1];
	
	
	diff_before = _flxport->ovs_grp_weight[hp_num][src_grp] - _flxport->ovs_grp_weight[hp_num][dst_grp]; /* always >=0*/
	diff_after  = diff_before - 2*(pm_num_weight); /* NOT always >=0*/
	diff_after = (diff_after<0) ? (-diff_after) : diff_after;
	
	if (diff_after >= diff_before) {
		TDM_PRINT6("tdm_th2_flex_ovs_tables_move_pm() __WARNING grp_speed=%d hp_num=%d: Cannot move a PM from src_grp=%d to dst_grp=%d diff_before=%d diff_after=%d\n", 
			grp_speed/1000, hp_num, src_grp, dst_grp, diff_before, diff_after);
		return FAIL;
	}
	
	TDM_PRINT5("tdm_th2_flex_ovs_tables_move_pm() grp_speed=%d hp_num=%d: MOVING pm_num=%d from src_grp=%d to dst_grp=%d\n", grp_speed/1000, hp_num, pm_num, src_grp, dst_grp);
	
	/* Move pm_num from src_grp to dst_grp */
	for (i=0; i< TH2_OS_VBS_GRP_LEN; i++) {
		phy_port = _flxport->ovs_tables[hp_num][src_grp][i];
		if (phy_port != TH2_NUM_EXT_PORTS) { /* valid port */
			pm_num_t = (phy_port-1)/_tdm->_chip_data.soc_pkg.pmap_num_lanes;
			if (pm_num == pm_num_t) {
				/* Remove phy_port from src_grp*/
				_flxport->ovs_tables[hp_num][src_grp][i] = TH2_NUM_EXT_PORTS;
				_flxport->ovs_grp_weight[hp_num][src_grp] -= grp_speed;
				/* Add phy_port to dst_grp*/
				for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
					if (_flxport->ovs_tables[hp_num][dst_grp][j] == TH2_NUM_EXT_PORTS) {
						_flxport->ovs_tables[hp_num][dst_grp][j] = phy_port;
						_flxport->ovs_grp_weight[hp_num][dst_grp] += grp_speed;
						break;
					}
				}
			}
		}
	}
	
	
	tdm_th2_flex_ovs_tables_update_grp_pms(_tdm, _flxport);

	return PASS;
}



/**
@name: tdm_th2_flex_ovs_tables_post
@param:

Copy from tmp struct into new calendar groups
 */
int tdm_th2_flex_ovs_tables_post( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{
	int i, j, hp_num, hp_indx;
	tdm_calendar_t *new_cal;

	/*  Compute _flxport->prev_num_grps[hp_num][grp_speed_indx] for the whole PIPE */
	switch (_tdm->_core_data.vars_pkg.cal_id) {
		case 0:	 new_cal=(&(_tdm->_chip_data.cal_0)); break;
		case 1:	 new_cal=(&(_tdm->_chip_data.cal_1)); break;
		case 2:	 new_cal=(&(_tdm->_chip_data.cal_2)); break;
		case 3:	 new_cal=(&(_tdm->_chip_data.cal_3)); break;
		case 4:	 new_cal=(&(_tdm->_chip_data.cal_4)); break;
		case 5:	 new_cal=(&(_tdm->_chip_data.cal_5)); break;
		case 6:	 new_cal=(&(_tdm->_chip_data.cal_6)); break;
		case 7:	 new_cal=(&(_tdm->_chip_data.cal_7)); break;
		default:													
			TDM_PRINT1("tdm_th2_flex_ovs_tables_post() Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);		
			return (TDM_EXEC_CORE_SIZE+1);							
	}


	/*  Copy old OVS tables over new OVS tables  */
	for (i=0; i< TH2_OS_VBS_GRP_NUM; i++) {
		hp_num = i / (TH2_OS_VBS_GRP_NUM/2);
		hp_indx = i % (TH2_OS_VBS_GRP_NUM/2);
		for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
			new_cal->cal_grp[i][j] = _flxport->ovs_tables[hp_num][hp_indx][j];
		}
	}

	return PASS;
}


/**
@name: tdm_th2_vbs_scheduler_ovs_indx_to_speed
@param:
 */
enum port_speed_e
tdm_th2_vbs_scheduler_ovs_indx_to_speed(enum port_speed_indx_e _speed_indx)
{
	enum port_speed_e grp_speed=SPEED_0;
	
	switch (_speed_indx) {
		case INDX_10G:	grp_speed= SPEED_10G; break;
		case INDX_20G:	grp_speed= SPEED_20G; break;
		case INDX_25G:	grp_speed= SPEED_25G; break;
		case INDX_40G:	grp_speed= SPEED_40G; break;
		case INDX_50G:	grp_speed= SPEED_50G; break;
		case INDX_100G: grp_speed= SPEED_100G; break;
		default:													
			TDM_ERROR1("tdm_th2_vbs_scheduler_ovs_indx_to_speed() Invalid _speed_indx= %0d\n",_speed_indx);		
	}
	
	return grp_speed;
}

/**
@name: tdm_th2_vbs_scheduler_ovs_indx_to_speed
@param:
 */
enum port_speed_indx_e
tdm_th2_vbs_scheduler_ovs_speed_to_indx(enum port_speed_e _speed)
{
	enum port_speed_indx_e speed_indx=INDX_10G;

	switch (_speed) {
		case SPEED_10G:  speed_indx=INDX_10G;  break;
		case SPEED_20G:  speed_indx=INDX_20G;  break;
		case SPEED_25G:  speed_indx=INDX_25G;  break;
		case SPEED_40G:  speed_indx=INDX_40G;  break;
		case SPEED_50G:  speed_indx=INDX_50G;  break;
		case SPEED_100G: speed_indx=INDX_100G; break;
		default:													
			TDM_PRINT1("tdm_th2_vbs_scheduler_ovs_speed_to_indx() Invalid group speed grp_speed %0d\n",_speed/1000);						
	}

	return speed_indx;
}




/**
@name: tdm_th2_sort_desc
@param:

Desc.: Sort vales in descending order - bubble sort
 */
int tdm_th2_sort_desc(int size, int *val_array, int *indx_array)
{
	int i,j, tmp;

/*
	TDM_PRINT0("\ntdm_th2_sort_desc BEFORE\n");
	for (i=0; i< size; i++) {
		TDM_PRINT1("\t%d",indx_array[i]);
	}
	TDM_PRINT0("\n");
	for (i=0; i< size; i++) {
		TDM_PRINT1("\t%d",val_array[i]);
	}
	TDM_PRINT0("\n");
*/

	/* This is equvalent with sorting pm_max_slots in descending order - bubble sort*/
	for (i=0; i<size-1; i++)
	{
		for (j=size-1; j>i; j--)
		{
			if ( val_array[j] > val_array[j-1]) /* swap j with j-1*/
			{
				tmp = indx_array[j];
				indx_array[j] = indx_array[j-1];
				indx_array[j-1] = tmp;
				tmp = val_array[j];
				val_array[j] = val_array[j-1];
				val_array[j-1] = tmp;
			}
		}
	}
/*
	TDM_PRINT0("\ntdm_th2_sort_desc AFTER SORTING\n");
	for (i=0; i< size; i++) {
		TDM_PRINT1("\t%d",indx_array[i]);
	}
	TDM_PRINT0("\n");
	for (i=0; i< size; i++) {
		TDM_PRINT1("\t%d",val_array[i]);
	}
	TDM_PRINT0("\n");
*/
	return PASS;
}


int tdm_th2_print_th2_flxport_t( tdm_mod_t *_tdm, th2_flxport_t *_flxport)
{

	int j, pipe_id, pms_per_pipe, hp_num, hp_indx, ln_num;
	int pm_num, phy_base_port, phy_port, grp_speed_indx;

	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;

	for (hp_num=0; hp_num<2; hp_num++) {
		TDM_PRINT1("\nHP_NUM=%d\n",hp_num);
		TDM_PRINT0("prev_num_grps:");
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
			TDM_PRINT1("\t%d",_flxport->prev_num_grps[hp_num][grp_speed_indx]);
		}
		TDM_PRINT0("\nnew_num_grps:");
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
			TDM_PRINT1("\t%d",_flxport->new_num_grps[hp_num][grp_speed_indx]);
		}
		

		TDM_PRINT0("\novs_grp_speed:");
		for (hp_indx=0; hp_indx< TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			TDM_PRINT1("\t%dG",_flxport->ovs_grp_speed[hp_num][hp_indx]/1000);
		}
		TDM_PRINT0("\novs_grp_weight:");
		for (hp_indx=0; hp_indx< TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			TDM_PRINT1("\t%dG",_flxport->ovs_grp_weight[hp_num][hp_indx]/1000);
		}		
		
		
		for (hp_indx=0; hp_indx< TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			TDM_PRINT2("\n HP_NUM=%d hp_indx %d :", hp_num, hp_indx);
			for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
				TDM_PRINT1("\t%d",_flxport->ovs_tables[hp_num][hp_indx][j]);
			}
		}

		for (hp_indx=0; hp_indx< TH2_OS_VBS_GRP_NUM/2; hp_indx++) {
			TDM_PRINT2("\n HP_NUM=%d hp_indx %d ovs_grp_pms:", hp_num, hp_indx);
			for (j=0; j< TH2_OS_VBS_GRP_LEN; j++) {
				TDM_PRINT1("\t%d",_flxport->ovs_grp_pms[hp_num][hp_indx][j]);
			}
		}
		
		TDM_PRINT1("\nHP_NUM=%d skip_ovs_for_speed:",hp_num);
		for (grp_speed_indx=0; grp_speed_indx<6; grp_speed_indx++) {
			TDM_PRINT1("\t%d",_flxport->skip_ovs_for_speed[hp_num][grp_speed_indx]);
		}

	}

		TDM_PRINT0("\n");
		TDM_PRINT1("\nPIPE=%d Ports to be removed:",pipe_id);
		for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
			phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
			for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
				phy_port = phy_base_port+ln_num;
				if (_flxport->ports_to_be_removed[phy_port]) {
					TDM_PRINT1("%5d",phy_port);
				}
			}
		}
	
		TDM_PRINT1("\nPIPE=%d Ports to be added:",pipe_id);
		for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
			phy_base_port = TH2_NUM_PM_LNS*pm_num+1;
			for (ln_num=0; ln_num<_tdm->_chip_data.soc_pkg.pmap_num_lanes; ln_num++) {
				phy_port = phy_base_port+ln_num;
				if (_flxport->ports_to_be_added[phy_port]) {
					TDM_PRINT1("%5d",phy_port);
				}
			}
		}
	
	
		TDM_PRINT0("\n\n");

	return PASS;
}



/**
@name: tdm_th2_ovs_map_pm_num_to_pblk_flex_port
@param: tdm_mod_t *_tdm

Maps PM num (block_id) to OVS pblk id
 */
int
tdm_th2_ovs_map_pm_num_to_pblk_flex_port( tdm_mod_t *_tdm, enum flexport_pm_state_e *pm_flexport_state )
{
	int i, j, pms_per_pipe, pipe_id;
	int pm_num;
	int half_pipe_num;
	int pblk_indx_to_pm[2][8];
	int indx, pblk_indx;
	
	pipe_id = _tdm->_core_data.vars_pkg.cal_id;
	pms_per_pipe  = _tdm->_chip_data.soc_pkg.pm_num_phy_modules/TH2_NUM_QUAD;
	
	/* remove from mapping PMs that are going down */
	indx = 0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		switch(pm_flexport_state[indx]) {
			/* This PM goes down*/
			case FLEXPORT_PM_UP_DOWN   :
			case FLEXPORT_PM_UP_UP     :
				_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[pm_num] = -1;
				break;
			default : break;
		}
		indx++;
	}

	/* Create PBLK index to PM num mapping */
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 8; j++) {
			pblk_indx_to_pm[i][j] = -1;
		}
	}
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		pblk_indx = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[pm_num];
		if (pblk_indx != -1) {
			half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
			pblk_indx_to_pm[half_pipe_num][pblk_indx] = pm_num;
		}
	}

	/* Add to mapping PMs that are brought up */
	indx = 0;
	for (pm_num=pipe_id*pms_per_pipe; pm_num<(pipe_id+1)*pms_per_pipe; pm_num++) {
		switch(pm_flexport_state[indx]) {
			/* This PM comes up */
			case FLEXPORT_PM_DOWN_UP   :
			case FLEXPORT_PM_UP_UP     :
				half_pipe_num = _tdm->_chip_data.soc_pkg.soc_vars.th2.pm_ovs_halfpipe[pm_num];
				/* Assign an unallocated pblk_indx to this PM */
				for (j = 0; j < 8; j++) {
					if (pblk_indx_to_pm[half_pipe_num][j] == -1) {
						pblk_indx_to_pm[half_pipe_num][j] = pm_num;
						_tdm->_chip_data.soc_pkg.soc_vars.th2.pm_num_to_pblk[pm_num] = j;						
						break;
					}
				}
				break;
			default : break;
		}
		indx++;
	}
	
	return PASS;
}
