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
	#include <tdm_th_vec.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/tdm/tomahawk/tdm_th_vec.h>
#endif


/**
@name: tdm_th_proc_cal_tdm5
@param:
 */
int
tdm_th_proc_cal_tdm5( tdm_mod_t *_tdm )
{
	int j, v, w;
	unsigned short lr1[TDM_AUX_SIZE], lr10[TDM_AUX_SIZE], lr20[TDM_AUX_SIZE], lr25[TDM_AUX_SIZE], lr40[TDM_AUX_SIZE], lr50[TDM_AUX_SIZE], lr100[TDM_AUX_SIZE];
	int param_freq, param_lr_limit, param_vmap_wid, param_vmap_len;
    unsigned short **param_vmap;
    
    param_freq     = _tdm->_chip_data.soc_pkg.clk_freq;
    param_lr_limit = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_vmap_wid = _tdm->_core_data.vmap_max_wid;
    param_vmap_len = _tdm->_core_data.vmap_max_len;
    param_vmap     = _tdm->_core_data.vmap;
    
	for (j=0; j<(TDM_AUX_SIZE); j++) {
        lr1[j]   = TH_NUM_EXT_PORTS;
        lr10[j]  = TH_NUM_EXT_PORTS;
        lr20[j]  = TH_NUM_EXT_PORTS;
        lr25[j]  = TH_NUM_EXT_PORTS;
        lr40[j]  = TH_NUM_EXT_PORTS;
        lr50[j]  = TH_NUM_EXT_PORTS;
        lr100[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[j]  = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[j]= TH_NUM_EXT_PORTS;
	}	
	
	for (j=0; j<TDM_AUX_SIZE; j++) {
		if ((_tdm->_core_data.vars_pkg.lr_buffer[j] != TH_NUM_EXT_PORTS) && ( (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE)||(_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE_HG) )) {
			switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]]) {
				case SPEED_1G:
				case SPEED_2p5G:
					tdm_core_prealloc(lr1, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), &(_tdm->_core_data.vars_pkg.lr_1), j);
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
				case SPEED_11G:
					tdm_core_prealloc(lr10, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), &(_tdm->_core_data.vars_pkg.lr_10), j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_core_prealloc(lr20, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), &(_tdm->_core_data.vars_pkg.lr_20), j);
					break;
				case SPEED_25G:
				case SPEED_27G:
					tdm_core_prealloc(lr25, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), &(_tdm->_core_data.vars_pkg.lr_25), j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_core_prealloc(lr40, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), &(_tdm->_core_data.vars_pkg.lr_40), j);
					break;
				case SPEED_50G:
				case SPEED_53G:
					tdm_core_prealloc(lr50, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), &(_tdm->_core_data.vars_pkg.lr_50), j);
					break;
				case SPEED_100G:
				case SPEED_106G:
					tdm_core_prealloc(lr100, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), &(_tdm->_core_data.vars_pkg.lr_100), j);
					break;
				default:
					TDM_ERROR2("Speed %0d port %0d was skipped in linerate preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]],_tdm->_core_data.vars_pkg.lr_buffer[j]);
			}
		}
		else if (_tdm->_core_data.vars_pkg.lr_buffer[j] != TH_NUM_EXT_PORTS && _tdm->_core_data.vars_pkg.lr_buffer[j] != TH_OVSB_TOKEN) {
			TDM_ERROR2("Failed to presort linerate port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.lr_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1]);
		}
		if ((_tdm->_core_data.vars_pkg.os_buffer[j] != TH_NUM_EXT_PORTS) && ( (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB)||(_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB_HG) )) {
			switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]]) {
				case SPEED_1G:
				case SPEED_2p5G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8), &(_tdm->_core_data.vars_pkg.os_1), j);
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
				case SPEED_11G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1), &(_tdm->_core_data.vars_pkg.os_10), j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2), &(_tdm->_core_data.vars_pkg.os_20), j);
					break;
				case SPEED_25G:
				case SPEED_27G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6), &(_tdm->_core_data.vars_pkg.os_25), j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3), &(_tdm->_core_data.vars_pkg.os_40), j);
					break;
				case SPEED_50G:
				case SPEED_53G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5), &(_tdm->_core_data.vars_pkg.os_50), j);
					break;
				case SPEED_100G:
				case SPEED_106G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4), &(_tdm->_core_data.vars_pkg.os_100), j);
					break;
				default:
					TDM_ERROR2("Speed %0d port %0d was skipped in oversub preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]],_tdm->_core_data.vars_pkg.os_buffer[j]);
			}
		}
		else if (_tdm->_core_data.vars_pkg.os_buffer[j] != TH_NUM_EXT_PORTS) {
			TDM_ERROR2("Failed to presort oversub port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.os_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1]);
		}
	}   
	_tdm->_core_data.vars_pkg.os_enable = (_tdm->_core_data.vars_pkg.os_1||_tdm->_core_data.vars_pkg.os_10||_tdm->_core_data.vars_pkg.os_20||_tdm->_core_data.vars_pkg.os_25||_tdm->_core_data.vars_pkg.os_40||_tdm->_core_data.vars_pkg.os_50||_tdm->_core_data.vars_pkg.os_100||_tdm->_core_data.vars_pkg.os_120);
	_tdm->_core_data.vars_pkg.lr_enable = (_tdm->_core_data.vars_pkg.lr_1||_tdm->_core_data.vars_pkg.lr_10||_tdm->_core_data.vars_pkg.lr_20||_tdm->_core_data.vars_pkg.lr_25||_tdm->_core_data.vars_pkg.lr_40||_tdm->_core_data.vars_pkg.lr_50||_tdm->_core_data.vars_pkg.lr_100||_tdm->_core_data.vars_pkg.lr_120);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4;
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3;
	/* 4x106HG+3x40G uses a unique TDM in Tomahawk */
	if (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g==3 && _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g==4 && _tdm->_chip_data.soc_pkg.clk_freq==TH_CLK_850MHZ) {
		_tdm->_chip_data.soc_pkg.tvec_size=9;
		_tdm->_chip_data.soc_pkg.lr_idx_limit=201;
		_tdm->_core_data.vars_pkg.HG4X106G_3X40G=BOOL_TRUE;
	}
    else {
        _tdm->_core_data.vars_pkg.HG4X106G_3X40G=BOOL_FALSE;
    }
	/* To guarantee oversub smoothness in nonscaling 100G+40G case, algorithm must sort with a 120G port vector (bw will be reclaimed) */
	if ( ( (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g==1) && (_tdm->_core_data.vars_pkg.lr_40) && (!_tdm->_core_data.vars_pkg.lr_120) && (_tdm->_core_data.vars_pkg.os_enable) ) ) {
		_tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB=BOOL_TRUE;
	}
	/* To sort 120G and 100G vectors together, load a special 100G vector */
	if ( (_tdm->_core_data.vars_pkg.lr_100 && _tdm->_core_data.vars_pkg.lr_120) ) {
		_tdm->_core_data.vars_pkg.HGXx120G_Xx100G=BOOL_TRUE;
	}
	TDM_PRINT9("(1G - %0d) (10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) (120G - %0d) (Number of Line Rate Types - %0d)\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7, (_tdm->_core_data.vars_pkg.lr_1+_tdm->_core_data.vars_pkg.lr_10+_tdm->_core_data.vars_pkg.lr_20+_tdm->_core_data.vars_pkg.lr_25+_tdm->_core_data.vars_pkg.lr_40+_tdm->_core_data.vars_pkg.lr_50+_tdm->_core_data.vars_pkg.lr_100+_tdm->_core_data.vars_pkg.lr_120));
	TDM_BIG_BAR
	/* 100G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 > 0){
		if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 107, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), param_lr_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 100G time vector preallocation\n");
				return 0;
			}
		}
		else if (_tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 108, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), param_lr_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 100G time vector preallocation\n");
				return 0;
			}
		}
		else if (_tdm->_core_data.vars_pkg.HGXx120G_Xx100G) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 109, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), param_lr_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 100G time vector preallocation\n");
				return 0;
			}
		}
		else{
			if (tdm_th_check_pipe_ethernet(lr100[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 100, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), param_lr_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 100G time vector preallocation\n");
					return 0;
				}
			}
			else {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 106, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), param_lr_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 100G time vector preallocation\n");
					return 0;
				}
			}
		}
	}
	/* 50G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 > 0){
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			if (tdm_th_legacy_which_tsc(lr50[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_th_legacy_which_tsc(lr50[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr50[v];
				for (w=v; w>0; w--) {
					lr50[w]=lr50[w-1];
				}
				lr50[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap);
			if ((_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ||
			    (_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ){
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50=BOOL_TRUE;
				TDM_PRINT0("50G triport detected\n");
				break;
			}
		}
		if (tdm_th_check_pipe_ethernet(lr50[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 50, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), param_lr_limit, lr50, TOKEN_50G, "50", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 50G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 53, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), param_lr_limit, lr50, TOKEN_50G, "50", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 50G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 40G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			if (tdm_th_legacy_which_tsc(lr40[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_th_legacy_which_tsc(lr40[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr40[v];
				for (w=v; w>0; w--) {
					lr40[w]=lr40[w-1];
				}
				lr40[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap);
			if ((_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ||
			    (_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ){
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_TRUE;
				TDM_PRINT0("40G triport detected\n");
				break;
			}
		}
		if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G==BOOL_TRUE) {
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==3) {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 43, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), param_lr_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
			else if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==2) {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 44, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), param_lr_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
			else if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==1) {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 45, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), param_lr_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
		}
		else {
			if (tdm_th_check_pipe_ethernet(lr40[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 40, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), param_lr_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
			else {
				if (tdm_core_postalloc_vmap(param_vmap, param_freq, 42, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), param_lr_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
		}
	}
	/* 25G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 > 0) {
		if (tdm_th_check_pipe_ethernet(lr25[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 25, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), param_lr_limit, lr25, TOKEN_25G, "25", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 25G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 27, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), param_lr_limit, lr25, TOKEN_25G, "25", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 25G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 20G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr20[v],_tdm->_chip_data.soc_pkg.pmap);
			if ((_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ||
			    (_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ){
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20=BOOL_TRUE;
				TDM_PRINT0("20G triport detected\n");
				break;
			}
		}
		if (tdm_th_check_pipe_ethernet(lr20[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 20, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), param_lr_limit, lr20, TOKEN_20G, "20", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 20G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 21, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), param_lr_limit, lr20, TOKEN_20G, "20", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 20G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 10G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 > 0) {
		if (tdm_th_check_pipe_ethernet(lr10[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 10, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), param_lr_limit, lr10, TOKEN_10G, "10", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 10G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc_vmap(param_vmap, param_freq, 11, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), param_lr_limit, lr10, TOKEN_10G, "10", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
				TDM_ERROR0("Critical error in 10G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 1G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8 > 0) {
		if (tdm_core_postalloc_vmap(param_vmap, param_freq, 1, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), param_lr_limit, lr1, TOKEN_1G, "1", TH_NUM_EXT_PORTS, param_vmap_wid, param_vmap_len)!=PASS) {
			TDM_ERROR0("Critical error in 1G time vector preallocation\n");
			return 0;
		}
	}
	
	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) );
}


/**
@name: tdm_th_proc_cal_ancl
@param:

Allocate ancillary slots with matched speed rates accordingly
 */
int
tdm_th_proc_cal_ancl(tdm_mod_t *_tdm)
{
    int i, j, k, k_prev, k_idx, idx_up, idx_dn, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty, param_token_ancl,
        param_lr_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    
    /* allocate ANCL slots */
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_ancl_num>0){
        TDM_PRINT1("TDM: Allocate %d ANCL slots\n\n", param_ancl_num);
        if (param_lr_en==BOOL_TRUE){
            k = 0;
            for (j=0; j<param_ancl_num; j++){
                idx_up = 0;
                idx_dn = 0;
                k_idx  = 0;
                k_prev = k;
                if (j==(param_ancl_num-1)){
                    k = param_cal_len - 1;
                }
                else {
                    k = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) -1;
                }
                for (i=k; i>0 && i<param_cal_len; i--){
                    if (param_cal_main[i]==param_token_empty){
                        idx_up = i;
                        break;
                    }
                }
                for (i=k; i<param_cal_len; i++){
                    if (param_cal_main[i]==param_token_empty){
                        idx_dn = i;
                        break;
                    }
                }
                if (idx_up>0 || idx_dn>0){
                    if (idx_up>0 && idx_dn>0){
                        k_idx = ((k-idx_up)<(idx_dn-k))?(idx_up):(idx_dn);
                    }
                    else if (idx_up>0){
                        k_idx = idx_up;
                    }
                    else if (idx_dn>0){
                        k_idx = idx_dn;
                    }
                }
                if (k_idx==0){
                    for (i=(param_cal_len-1); i>0; i--){
                        if (param_cal_main[i]==param_token_empty){
                            k_idx = i;
                            break;
                        }
                    }
                }
                if (k_idx>0 && k_idx<param_cal_len){
                    param_cal_main[k_idx] = param_token_ancl;
                    TDM_PRINT2("TDM: Allocate %d-th ANCL slot at index #%03d\n",
                                j+1, k_idx);
                }
                else {
                    TDM_WARN1("TDM: Failed in %d-th ANCL slot allocation\n",j);
                    result = FAIL;
                }
            }
        }
        else {
            k_prev = 0;
            for (j=0; j<param_ancl_num; j++){
                k_idx = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) - 1;
                param_cal_main[k_idx] = param_token_ancl;
                k_prev= k_idx+1;
                TDM_PRINT1("TDM: Allocate ANCL slot at index #%03d\n",
                            k_idx);
            }
        }
        TDM_SML_BAR
    }
    
    return result;
}


/**
@name: tdm_th_proc_cal_acc
@param:

Allocate accessory slots with matched speed rates accordingly
 */
int
tdm_th_proc_cal_acc(tdm_mod_t *_tdm)
{
    int i, j, v, k, k_prev, k_idx, clump_size, clump_size_max, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty, param_token_ancl,
        param_lr_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    
    /* allocate ANCL slots */
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_ancl_num>0 && param_ancl_num<=param_cal_len){
        TDM_PRINT1("TDM: Allocate %d ANCL slots\n\n", param_ancl_num);
        if (param_lr_en==BOOL_TRUE){
            k = 0;
            for (j=0; j<param_ancl_num; j++){
                k_prev = k;
                if (j==(param_ancl_num-1)){
                    k = param_cal_len - 1;
                }
                else {
                    k = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) -1;
                }
                k_idx= 0;
                clump_size     = 0;
                clump_size_max = 0;
                for (i=k; i>=k_prev && i<param_cal_len; i--){
                    if (param_cal_main[i]==param_token_empty){
                        clump_size++;
                        if (i==k){
                            for (v=k+1; v<param_cal_len; v++){
                                if (param_cal_main[v]==param_token_empty){
                                    clump_size++;
                                }
                                else{
                                    break;
                                }
                            }
                        }
                    }
                    else{
                        if (clump_size>clump_size_max){
                            clump_size_max = clump_size;
                            k_idx = i + (clump_size_max/2) + 1;
                        }
                        clump_size = 0;
                    }
                }
                if (k_idx==0){
                    for (i=(param_cal_len-1); i>0; i--){
                        if (param_cal_main[i]==param_token_empty){
                            k_idx = i;
                            break;
                        }
                    }
                }
                if (k_idx>0 && k_idx<param_cal_len){
                    param_cal_main[k_idx] = param_token_ancl;
                    TDM_PRINT1("TDM: Allocate ANCL slot at index #%03d\n",
                                k_idx);
                }
                else {
                    TDM_WARN1("TDM: Failed in %d-th ANCL slot allocation\n",j);
                    result = FAIL;
                }
            }
        }
        else {
            k_prev = 0;
            for (j=0; j<param_ancl_num; j++){
                k_idx = k_prev + ((param_cal_len-k_prev)/(param_ancl_num-j)) - 1;
                param_cal_main[k_idx] = param_token_ancl;
                k_prev= k_idx+1;
                TDM_PRINT1("TDM: Allocate ANCL slot at index #%03d\n",
                            k_idx);
            }
        }
        TDM_SML_BAR
    }
    
    return result;
}


/**
@name: tdm_th_proc_cal_idle
@param:

Allocate idle slots with matched speed rates accordingly
 */
int
tdm_th_proc_cal_idle( tdm_mod_t *_tdm )
{
    int i, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_empty, param_token_ovsb,
        param_token_idl1, param_token_idl2,
        param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_empty = _tdm->_chip_data.soc_pkg.num_ext_ports;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_idl1  = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;
    param_token_idl2  = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    if (param_os_en==BOOL_TRUE){
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                param_cal_main[i] = param_token_ovsb;
            }
        }
    }
    else {
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_empty){
                if ((i%2)==0){
                    param_cal_main[i] = param_token_idl1;
                }
                else {
                    param_cal_main[i] = param_token_idl2;
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_th_proc_cal_lr
@param:

Allocate linerate slots with matched speed rates accordingly
 */
int
tdm_th_proc_cal_lr( tdm_mod_t *_tdm )
{
    int *param_lr_buff=NULL, result=PASS;
    
    /* allocate Linerate slots (step-1): vmap pre-allocation */
    if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_PREALLOC](_tdm)!=PASS){
        result = FAIL;
    }
    /* allocate Linerate slots (step-2): vmap allocation */
    else{
        /* SPECIAL CASE: 4x106HG+3x40G uses a unique TDM in Tomahawk */
        param_lr_buff = _tdm->_core_data.vars_pkg.lr_buffer;
        _tdm->_core_data.vars_pkg.HG4X106G_3X40G = BOOL_FALSE;
        if (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g==3 &&
            _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g==4 &&
            _tdm->_chip_data.soc_pkg.clk_freq==TH_CLK_850MHZ) {
            if (tdm_th_check_pipe_ethernet(param_lr_buff[0],_tdm)!=BOOL_TRUE||
                _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_TRUE  ||
                _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_TRUE   ){
                _tdm->_chip_data.soc_pkg.tvec_size   = 9;
                _tdm->_chip_data.soc_pkg.lr_idx_limit= 201;
                _tdm->_core_data.vars_pkg.HG4X106G_3X40G = BOOL_TRUE;
                TDM_PRINT0("TDM: Special case 4x106HG_3x40HG\n");
            }
        }
        /* vmap allocation */
        if(_tdm->_core_exec[TDM_CORE_EXEC__VMAP_ALLOC](_tdm)!=PASS){
            result = FAIL;
        }
    }
    
    return result;
}


/**
@name: tdm_th_proc_cal
@param:

Allocate slots for linerate/oversub/ancl ports with matched speed rates accordingly
 */
int
tdm_th_proc_cal( tdm_mod_t *_tdm )
{
    /* allocate LINERATE slots */
    if(tdm_th_proc_cal_lr(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    /* allocate ANCILLARY slots */
    if(tdm_th_proc_cal_ancl(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    /* allocate IDLE/OVSB slots */
    if(tdm_th_proc_cal_idle(_tdm)!=PASS){
        return (TDM_EXEC_CORE_SIZE+1);
    }
    
    return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER](_tdm) );
}
