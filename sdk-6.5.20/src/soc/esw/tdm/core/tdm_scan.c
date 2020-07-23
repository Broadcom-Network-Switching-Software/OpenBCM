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
@name: tdm_find_pm
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
 */
int
tdm_find_pm( tdm_mod_t *_tdm )
{
	int i, j, which=_tdm->_chip_data.soc_pkg.num_ext_ports;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_modules; i++) {
		for (j=0; j<_tdm->_chip_data.soc_pkg.pmap_num_lanes; j++) {
			if (_tdm->_chip_data.soc_pkg.pmap[i][j]==_tdm->_core_data.vars_pkg.port) {
				which=i;
			}
		}
		if (which!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			break;
		}
	}
	
	return which;
	
}


/**
@name: tdm_type_chk
@param:

Bubble sorts port module mapping slice and returns # of transitions
 */
int
tdm_type_chk( tdm_mod_t *_tdm )
{
	int i, j, id=_tdm->_chip_data.soc_pkg.num_ext_ports, cnt=1,
		tsc_arr[PM_SORT_STACK_SIZE],
		tsc_inst=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);

    if (tsc_inst < _tdm->_chip_data.soc_pkg.pm_num_phy_modules &&
        _tdm->_chip_data.soc_pkg.pmap_num_lanes<=PM_SORT_STACK_SIZE) {
        TDM_COPY(tsc_arr,_tdm->_chip_data.soc_pkg.pmap[tsc_inst],sizeof(int)*_tdm->_chip_data.soc_pkg.pmap_num_lanes);
        
        for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
            for (j=0; j<_tdm->_chip_data.soc_pkg.pmap_num_lanes-i; j++) {
                if ((j+1)<_tdm->_chip_data.soc_pkg.pmap_num_lanes &&
                    (j+1)<PM_SORT_STACK_SIZE ) {
                    if (tsc_arr[j] > tsc_arr[j+1]) {
                        id=tsc_arr[j];
                        tsc_arr[j]=tsc_arr[j+1];
                        tsc_arr[j+1]=id;
                    }
                }
            }
        }
        for (i=1; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes && i<PM_SORT_STACK_SIZE; i++) {
            if (tsc_arr[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports && tsc_arr[i]!=tsc_arr[i-1]) {
                cnt++;
            }
        }
    }
	
	return cnt;

}


/**
@name: tdm_find_fastest_port
@param:

Returns the fastest port number from within a tsc
 */
int
tdm_find_fastest_port( tdm_mod_t *_tdm )
{
	int i, port=_tdm->_chip_data.soc_pkg.num_ext_ports, tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	unsigned int spd=SPEED_0;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {	
			if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]>spd) {
				port=_tdm->_chip_data.soc_pkg.pmap[tsc_id][i];
				spd=_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]];
			}
		}
	}
	
	return port;
	
}


/**
@name: tdm_find_fastest_spd
@param:

Returns the speed of the fastest lane from within a tsc
 */
int
tdm_find_fastest_spd( tdm_mod_t *_tdm )
{
	int i, tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	unsigned int spd=SPEED_0;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]>spd) {
				spd=_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]];
			}
		}
	}
	
	return spd;	
}


/**
@name: tdm_find_fastest_triport
@param:

Returns whether the current port is the fastest port from within a triport
 */
int
tdm_find_fastest_triport( tdm_mod_t *_tdm )
{
	int i, tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_core_data.vars_pkg.port) {
			if (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.port]<_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]) {
				return BOOL_FALSE;
			}
		}
	}
	
	return BOOL_TRUE;
	
}


/**
@name: tdm_pick_vec
@param:

Select vector index on x axis to rotate based on priority of TSC pipeline
 */
int
tdm_pick_vec( tdm_mod_t *_tdm )
{
	int i, vec_sel=1, port=_tdm->_core_data.vars_pkg.port;
	
	for (i=_tdm->_core_data.vars_pkg.m_tdm_pick_vec.prev_vec; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
		if ( (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)==_tdm->_core_data.vars_pkg.m_tdm_pick_vec.tsc_dq) ) {
			if (_tdm->_core_data.vars_pkg.m_tdm_pick_vec.triport_priority) {
				if (tdm_find_fastest_triport(_tdm)) {
					vec_sel=i;
					_tdm->_core_data.vars_pkg.m_tdm_pick_vec.triport_priority=BOOL_FALSE;
					break;
				}
				else {
					continue;
				}
			}
			else {
				vec_sel=i;
				break;
			}
		}
	}
	
	_tdm->_core_data.vars_pkg.port=port;
	return vec_sel;
	
}


/**
@name: tdm_map_find_y_indx
@param:

Scans vector map Y axis for Y index of any non-principle node
 */
int
tdm_map_find_y_indx( tdm_mod_t *_tdm )
{
	int i, id=_tdm->_chip_data.soc_pkg.num_ext_ports;
	
	if (_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx<_tdm->_core_data.vmap_max_wid) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
			if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx][i]!=_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: tdm_nsin_row
@param:

Checks if current row is singular
 */
int
tdm_nsin_row( tdm_mod_t *_tdm )
{
	int i, found_port=_tdm->_chip_data.soc_pkg.num_ext_ports, check=PASS;
	
	for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx]) {
			if (found_port==_tdm->_chip_data.soc_pkg.num_ext_ports) {
				found_port=_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx];
			}
			else {
				check=FAIL;
				break;
			}
		}
	}
	
	return check;
	
}


/**
@name: tdm_check_blank
@param:

Checks if current row is blank
 */
int
tdm_empty_row(unsigned short **map, unsigned short y_idx, int num_ext_ports, int vec_map_wid)
{
	int i, check=PASS;
	
	for (i=0; i<vec_map_wid; i++) {
		if (map[i][y_idx]!=num_ext_ports) {
			check=FAIL;
			break;
		}
	}
	
	return check;
	
}


/**
@name: tdm_slice_size_2d
@param:

Returns size of proximate 2-D slice in vector map or 0 if the passed row was blank
 */
int
tdm_slice_size_2d(unsigned short **map, unsigned short y_idx, int lim, int num_ext_ports, int vec_map_wid)
{
	int i, slice_size=1;
	
	if (tdm_empty_row(map,y_idx,num_ext_ports,vec_map_wid)) {
		return 0;
	}
	else {
		for (i=1; (y_idx-i)>=0; i++) {
			if (tdm_empty_row(map,i,num_ext_ports,vec_map_wid)) {
				break;
			}
			slice_size++;
		}
		for (i=1; (y_idx+i)<lim; i++) {
			if (tdm_empty_row(map,i,num_ext_ports,vec_map_wid)) {
				break;
			}
			slice_size++;
		}
	}
	
	return slice_size;	
}


/**
@name: tdm_fit_singular_cnt
@param:

Given y index, count number of nodes
 */
int
tdm_fit_singular_cnt(unsigned short **map, int node_y, int vec_map_wid, int num_ext_ports)
{
	int v, cnt=0;
	
	for (v=0; v<vec_map_wid; v++) {
		if (map[v][node_y]!=num_ext_ports) {
			++cnt;
		}
	}
	
	return cnt;
}


/**
@name: tdm_map_cadence_count
@param:

Returns size of port sequence at index
 */
int
tdm_map_cadence_count(unsigned short *vector, int idx, int vec_map_len)
{
	int i=idx, cnt=0;
	unsigned short port=vector[idx];
	
	while (vector[++i]!=port && i<vec_map_len) {
		cnt++;
	}
	
	return cnt;	
}


/**
@name: tdm_map_retrace_count
@param:

Returns number of retraceable slots within cadence at index
 */
int
tdm_map_retrace_count(unsigned short **map, int x_idx, int y_idx, int vec_map_len, int vec_map_wid, int num_ext_ports)
{
	int i=y_idx, cnt=0;
	unsigned short port=map[x_idx][y_idx];
	
	while (map[x_idx][++i]!=port && i<vec_map_len) {
		if ( tdm_fit_singular_cnt(map,i,vec_map_wid,num_ext_ports)==0 ) {
			cnt++;
		}
	}
	
	return cnt;
}


/**
@name: tdm_fit_singular_col
@param:

Given x index, determines fit based on if current column is reducible
 */
int
tdm_fit_singular_col( tdm_mod_t *_tdm )
{
	int v, result=PASS;

	for (v=0; v<_tdm->_chip_data.soc_pkg.lr_idx_limit; v++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_fit_singular_col.node_x][v]) {
			if ( tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)>1 ) {
				result=FAIL;
				break;
			}
		}
	}
	
	return result;
}


/**
@name: tdm_count_nonsingular
@param:

Counts number of unreduced rows at current rotation
 */
int
tdm_count_nonsingular( tdm_mod_t *_tdm )
{
	int i, j, ns_cnt=0;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx][i]) {
			for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
				if ( (j!=_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx) && (_tdm->_core_data.vmap[j][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) ) {
					ns_cnt++;
				}
			}
		}
	}
	
	return ns_cnt;
}


/**
@name: tdm_fit_row_min
@param:

Checks if current row in vmap has a min spacing violation
 */
int
tdm_fit_row_min( tdm_mod_t *_tdm )
{
	int i, j, port=_tdm->_chip_data.soc_pkg.num_ext_ports,
		pm_0, pm_1;

	for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx]) {
			port=_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx];
			break;
		}
	}
	TOKEN_CHECK(port) {
		for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
			for (j=0; j<VBS_MIN_SPACING; j++) {
				TOKEN_CHECK(_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx+j]) {
					_tdm->_core_data.vars_pkg.port = _tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx+j];
					pm_0 = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					_tdm->_core_data.vars_pkg.port = port;
					pm_1 = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					if (_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx+j]!=port && pm_0==pm_1) {
						return FAIL;
					}
				}
			}
		}
	}

	return PASS;
	
}


/**
@name: tdm_fit_prox
@param:

Given x index, determines fit for current vector based on sister port (VBS) spacing rule
 */
int
tdm_fit_prox( tdm_mod_t *_tdm )
{
	int i, j, v, check=PASS, wc=_tdm->_chip_data.soc_pkg.num_ext_ports, _port=_tdm->_core_data.vars_pkg.port;
		
	for (i=(_tdm->_core_data.rule__prox_port_min-1); i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x][i]) {
			_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x][i];
			wc=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
			for (v=0; v<_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid; v++) {
				if (v!=_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x) {	
					for (j=0; j<_tdm->_core_data.rule__prox_port_min; j++) {
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[v][i+j];
						if (wc==_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)) {
							check=FAIL;
							break;
						}
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[v][i-j];
						if (wc==_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)) {
							check=FAIL;
							break;
						}
					}
				}
			}			
		}
		if (!check) {
			break;
		}
	}
	
	_tdm->_core_data.vars_pkg.port=_port;
	return check;
}


/**
@name: tdm_count_param_spd
@param:

Returns parameterized speed of a vector on the vector map by counting the number of nodes
 */
int
tdm_count_param_spd( tdm_mod_t *_tdm )
{
	int k=0, v, param_spd, y_pos=0, port, cal_len;
	param_spd=_tdm->_chip_data.soc_pkg.num_ext_ports;
	port=_tdm->_core_data.vars_pkg.port;
	cal_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	
	for (v=0; v<cal_len; v++) {
		if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos][v]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			y_pos = (y_pos==0) ? (v):(y_pos);
			k++;
		}
	}
	_tdm->_core_data.vars_pkg.port = _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos][y_pos];
	switch(k) {
		case 1: param_spd = 1; break;
		case 4: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 10;} else {param_spd = 11;} break;
		case 8: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 20;} else {param_spd = 21;} break;
		case 10: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 25;} else {param_spd = 27;} break;
		case 16: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 40;} else {param_spd = 42;} break;
		case 20: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 50;} else {param_spd = 53;} break;
		case 39: param_spd = 107; break;
		case 40: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 100;} else {param_spd = 106;} break;
		case 48: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 120;} else {param_spd = 124;} break;
		case 0 : param_spd = 0; break;
		default: TDM_PRINT2("Unrecognized spd_slot_num %d of port %d\n", k, _tdm->_core_data.vars_pkg.port); break;
	}
	_tdm->_core_data.vars_pkg.port = port;

	if (param_spd>=10) {
		while (_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round && param_spd%5!=0) {
			param_spd--;
		}
	}
	
	return param_spd;

}


/**
@name: tdm_cmn_chk_port_is_fp
@param:
@desc: Return BOOL_TRUE if given port is a front panel port,
       otherwise return BOOL_FALSE.
 */
int
tdm_cmn_chk_port_is_fp(tdm_mod_t *_tdm, int port_token)
{
    return ((port_token >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
             port_token <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) ?
            BOOL_TRUE : BOOL_FALSE);
}


/**
@name: tdm_cmn_chk_pipe_os_spd_types
@param:
 */
int
tdm_cmn_chk_pipe_os_spd_types(tdm_mod_t *_tdm)
{
    int i, spd_type_cnt = 0, grp_num_req = 0;
    m_tdm_pipe_info_t *pipe_info;
    tdm_calendar_t *cal;

    pipe_info = &(_tdm->_core_data.vars_pkg.pipe_info);
    cal = tdm_cmn_get_pipe_cal(_tdm);
    if (pipe_info == NULL || cal == NULL) return PASS;

    if (pipe_info->os_en && cal->grp_len > 0) {
        for (i = 0; i < MAX_SPEED_TYPES; i++) {
            if (pipe_info->os_spd_en[i]) {
                spd_type_cnt++;
                grp_num_req += (pipe_info->os_prt_cnt[i]-1)/cal->grp_len + 1;
            }
        }
    }

    TDM_PRINT1("   1G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_1G]   );
    TDM_PRINT1("1.25G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_1P25G]);
    TDM_PRINT1(" 2.5G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_2P5G] );
    TDM_PRINT1("  10G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_10G]  );
    TDM_PRINT1("12.5G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_12P5G]);
    TDM_PRINT1("  20G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_20G]  );
    TDM_PRINT1("  25G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_25G]  );
    TDM_PRINT1("  40G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_40G]  );
    TDM_PRINT1("  50G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_50G]  );
    TDM_PRINT1(" 100G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_100G] );
    TDM_PRINT1(" 120G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_120G] );
    TDM_PRINT1(" 200G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_200G] );
    TDM_PRINT1(" 400G ports - %0d\n", pipe_info->os_prt_cnt[TDM_SPEED_IDX_400G] );
    TDM_PRINT1("\nNumber of OVSB speed types - %3d\n", spd_type_cnt);
    TDM_SML_BAR

    if (spd_type_cnt > cal->grp_num) {
        TDM_ERROR2("OVSB speed types exceeded, spd_types %0d, limit %0d\n",
                    spd_type_cnt, cal->grp_num);
        return FAIL;
    }
    if (grp_num_req > cal->grp_num) {
        TDM_ERROR2("OVSB speed groups overflow, grp_req %0d, limit %0d\n",
                    grp_num_req, cal->grp_num);
        return FAIL;
    }

    return PASS;
}


/**
@name: tdm_cmn_get_port_pm
@param:
 */
int
tdm_cmn_get_port_pm(tdm_mod_t *_tdm, int port_token)
{
    _tdm->_core_data.vars_pkg.port = port_token;
    return (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm));
}


/**
@name: tdm_cmn_get_port_speed
@param:
 */
int
tdm_cmn_get_port_speed(tdm_mod_t *_tdm, int port_token)
{
    if (tdm_cmn_chk_port_is_fp(_tdm, port_token) == BOOL_TRUE) {
        return (_tdm->_chip_data.soc_pkg.speed[port_token]);
    } else {
        return 0;
    }
}


/**
@name: tdm_cmn_get_port_speed_eth
@param:

Return port speed in ethernet mode. This converts HiGig speed to ethernet.
 */
int
tdm_cmn_get_port_speed_eth(tdm_mod_t *_tdm, int port_token)
{
    int port_speed, port_speed_eth;

    port_speed = tdm_cmn_get_port_speed(_tdm, port_token);
    switch(port_speed) {
        case SPEED_1G:
        case SPEED_1p25G:
        case SPEED_2G:
        case SPEED_2p5G:
        case SPEED_5G:
        case SPEED_6G:
            port_speed_eth = port_speed;
            break;
        case SPEED_10G:
        case SPEED_10G_DUAL:
        case SPEED_10G_XAUI:
        case SPEED_11G:
            port_speed_eth = SPEED_10G;
            break;
        case SPEED_12p5G:
            port_speed_eth = SPEED_12p5G;
            break;
        case SPEED_20G:
        case SPEED_21G:
        case SPEED_21G_DUAL:
            port_speed_eth = SPEED_20G;
            break;
        case SPEED_25G:
        case SPEED_27G:
            port_speed_eth = SPEED_25G;
            break;
        case SPEED_40G:
        case SPEED_42G:
        case SPEED_42G_HG2:
            port_speed_eth = SPEED_40G;
            break;
        case SPEED_50G:
        case SPEED_53G:
            port_speed_eth = SPEED_50G;
            break;
        case SPEED_100G:
        case SPEED_106G:
            port_speed_eth = SPEED_100G;
            break;
        case SPEED_120G:
        case SPEED_127G:
            port_speed_eth = SPEED_120G;
            break;
        case SPEED_200G:
            port_speed_eth = SPEED_200G;
            break;
        case SPEED_400G:
            port_speed_eth = SPEED_400G;
            break;
        default:
            port_speed_eth = 0;
            break;
    }

    return port_speed_eth;
}


/**
@name: tdm_cmn_get_port_speed_idx
@param:

Return speed class of the given port according.
 */
int
tdm_cmn_get_port_speed_idx(tdm_mod_t *_tdm, int port_token)
{
    int port_speed, speed_idx = 0;

    port_speed = tdm_cmn_get_port_speed_eth(_tdm, port_token);
    switch(port_speed) {
        case SPEED_1G   : speed_idx = TDM_SPEED_IDX_1G   ; break;
        case SPEED_1p25G: speed_idx = TDM_SPEED_IDX_1P25G; break;
        case SPEED_2p5G : speed_idx = TDM_SPEED_IDX_2P5G ; break;
        case SPEED_5G   : speed_idx = TDM_SPEED_IDX_5G   ; break;
        case SPEED_10G  : speed_idx = TDM_SPEED_IDX_10G  ; break;
        case SPEED_12p5G: speed_idx = TDM_SPEED_IDX_12P5G; break;
        case SPEED_20G  : speed_idx = TDM_SPEED_IDX_20G  ; break;
        case SPEED_25G  : speed_idx = TDM_SPEED_IDX_25G  ; break;
        case SPEED_40G  : speed_idx = TDM_SPEED_IDX_40G  ; break;
        case SPEED_50G  : speed_idx = TDM_SPEED_IDX_50G  ; break;
        case SPEED_100G : speed_idx = TDM_SPEED_IDX_100G ; break;
        case SPEED_120G : speed_idx = TDM_SPEED_IDX_120G ; break;
        case SPEED_200G : speed_idx = TDM_SPEED_IDX_200G ; break;
        case SPEED_400G : speed_idx = TDM_SPEED_IDX_400G ; break;
        default:          speed_idx = TDM_SPEED_IDX_0    ; break;
    }

    return speed_idx;
}


/**
@name: tdm_cmn_get_slot_unit
@param:

Return the granularity (bandwidth per slot) of main calendar.
Note that the slot_unit should have the same precision of port SPEED.
 */
int
tdm_cmn_get_slot_unit(tdm_mod_t *_tdm)
{
    int slot_unit;

    slot_unit = _tdm->_core_data.vars_pkg.pipe_info.slot_unit;
    /* NOTE: add new slot_unit below for backward consistency */
    if (slot_unit != TDM_SLOT_UNIT_1G    &&
        slot_unit != TDM_SLOT_UNIT_1P25G &&
        slot_unit != TDM_SLOT_UNIT_2P5G  &&
        slot_unit != TDM_SLOT_UNIT_5G    &&
        slot_unit != TDM_SLOT_UNIT_10G   &&
        slot_unit != TDM_SLOT_UNIT_25G   &&
        slot_unit != TDM_SLOT_UNIT_50G) {
        slot_unit = TDM_SLOT_UNIT_2P5G;
    }

    return slot_unit;
}


/**
@name: tdm_cmn_get_speed_slots
@param:
 */
int
tdm_cmn_get_speed_slots(tdm_mod_t *_tdm, int port_speed)
{
    int speed_slots = 0;
    int slot_unit;

    slot_unit = tdm_cmn_get_slot_unit(_tdm);
    if (slot_unit > 0 && port_speed > 0) {
        speed_slots = port_speed / slot_unit;
        /* if (port_speed % slot_unit > 0) speed_slots += 1; */
        if (speed_slots == 0) speed_slots = 1;
    }

    return speed_slots;
}


/**
@name: tdm_cmn_get_port_slots
@param:
 */
int
tdm_cmn_get_port_slots(tdm_mod_t *_tdm, int port_token)
{
    int port_slots, port_speed;

    port_speed = tdm_cmn_get_port_speed_eth(_tdm, port_token);
    port_slots = tdm_cmn_get_speed_slots(_tdm, port_speed);

    return port_slots;
}


/**
@name: tdm_cmn_get_pipe_cal
@param:

Return calendar pointer of the current pipe.
 */
tdm_calendar_t *
tdm_cmn_get_pipe_cal(tdm_mod_t *_tdm)
{
    tdm_calendar_t * cal = NULL;

    if (_tdm != NULL) {
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
                TDM_PRINT1("Invalid calendar ID - %0d\n",
                           _tdm->_core_data.vars_pkg.cal_id);
                cal = NULL;
                break;
        }
    }

    return cal;
}


/**
@name: tdm_cmn_get_pipe_cal
@param:

Return previous calendar pointer of the current pipe.
 */
tdm_calendar_t *
tdm_cmn_get_pipe_cal_prev(tdm_mod_t *_tdm)
{
    tdm_calendar_t * cal = NULL;

    if (_tdm != NULL) {
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
                TDM_PRINT1("Invalid calendar ID - %0d\n",
                           _tdm->_core_data.vars_pkg.cal_id);
                cal = NULL;
                break;
        }
    }

    return cal;
}
