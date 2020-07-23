/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip data structure scanning functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_th2_which_tsc
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
 */
int
tdm_th2_which_tsc( tdm_mod_t *_tdm_s )
{
	TH2_TOKEN_CHECK(_tdm_s->_core_data.vars_pkg.port) {
		return tdm_find_pm( _tdm_s );
	}

	return TH2_NUM_EXT_PORTS;
}


/**
@name: tdm_th2_check_ethernet
@param:

Returns BOOL_TRUE or BOOL_FALSE depending on if pipe of the given port has traffic entirely Ethernet
 */
int
tdm_th2_check_ethernet( tdm_mod_t *_tdm )
{
	int type=BOOL_TRUE;
	
	if (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.port-1]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.port-1]==PORT_STATE__OVERSUB_HG) {
		return BOOL_FALSE;
	}
	
	return type;
}


/**
@name: tdm_td2p_legacy_which_tsc
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
 */
int
tdm_th2_legacy_which_tsc(unsigned short port, int **tsc)
{
	int i, j, which=TH2_NUM_EXT_PORTS;
	
	TH2_TOKEN_CHECK(port) {
		for (i=0; i<TH2_NUM_PHY_PM; i++) {
			for (j=0; j<TH2_NUM_PM_LNS; j++) {
				if (tsc[i][j]==port) {
					which=i;
				}
			}
			if (which!=TH2_NUM_EXT_PORTS) {
				break;
			}
		}
	}
	
	return which;
	
}


/**
@name: tdm_th2_check_same_port_dist_dn
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
 */
int
tdm_th2_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (++slot==lim) {slot=0;}
		if (tdm_tbl[slot]==tdm_tbl[idx]) {
			break;
		}
		dist++;
	}

	return dist;

}


/**
@name: tdm_th2_check_same_port_dist_up
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
 */
int
tdm_th2_check_same_port_dist_up(int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (--slot<=0) {slot=(lim-1);}
		if (tdm_tbl[slot]==tdm_tbl[idx]) {
			break;
		}
		dist++;
	}

	return dist;

}


/**
@name: tdm_th2_check_same_port_dist_dn_port
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
 */
int
tdm_th2_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (++slot==lim) {slot=0;}
		if (tdm_tbl[slot]==port) {
			break;
		}
		dist++;
	}

	return dist;
}


/**
@name: tdm_th2_check_same_port_dist_up_port
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
 */
int
tdm_th2_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (--slot<=0) {slot=(lim-1);}
		if (tdm_tbl[slot]==port) {
			break;
		}
		dist++;
	}

	return dist;
}


/**
@name: tdm_th2_slice_size_local
@param:

Given index, returns size of largest contiguous slice
 */
int
tdm_th2_slice_size_local(unsigned short idx, int *tdm, int lim)
{
	int i, slice_size=(-1);
	
	if (tdm[idx]!=TH2_OVSB_TOKEN && tdm[idx]!=TH2_NUM_EXT_PORTS) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]!=TH2_OVSB_TOKEN && tdm[i]!=TH2_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]!=TH2_OVSB_TOKEN && tdm[i]!=TH2_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
	}
	else if (tdm[idx]==TH2_OVSB_TOKEN) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]==TH2_OVSB_TOKEN) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]==TH2_OVSB_TOKEN) {
				slice_size++;
			}
			else {
				break;
			}
		}
	}

	return slice_size;
}


/**
@name: tdm_th2_slice_size
@param:

Given port number, returns size of largest slice
 */
int
tdm_th2_slice_size(unsigned short port, int *tdm, int lim)
{
	int i, j, k=0, slice_size=0;
	
	if (port<129 && port>0) {
		for (i=0; i<lim; i++) {
			TH2_TOKEN_CHECK(tdm[i]) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					TH2_TOKEN_CHECK(tdm[j]) {k++;}
					else {break;}
				}
			}
			slice_size = (k>slice_size)?(k):(slice_size);
		}
	}
	else {
		for (i=2; i<lim; i++) {
			if (tdm[i]==port) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					if (tdm[j]==port) {k++;}
					else {break;}
				}
			}
			slice_size = (k>slice_size)?(k):(slice_size);
		}
	}

	return slice_size;
}


/**
@name: tdm_th2_slice_idx
@param:

Given port number, returns index of largest slice
 */
int
tdm_th2_slice_idx(unsigned short port, int *tdm, int lim)
{
	int i, j, k=0, slice_size=0, slice_idx=0;
	
	if (port<129 && port>0) {
		for (i=0; i<lim; i++) {
			TH2_TOKEN_CHECK(tdm[i]) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					TH2_TOKEN_CHECK(tdm[j]) {k++;}
					else {break;}
				}
			}
			if (k>slice_size) {
				slice_idx=i;
				slice_size=k;
			}
		}
	}
	else {
		for (i=2; i<lim; i++) {
			if (tdm[i]==port) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					if (tdm[j]==port) {k++;}
					else {break;}
				}
			}
			if (k>slice_size) {
				slice_idx=i;
				slice_size=k;
			}
		}
	}
	
	return slice_idx;
}


/**
@name: tdm_th2_slice_prox_dn
@param:

Given port number, checks min spacing in a slice in down direction
 */
int
tdm_th2_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed)
{
	int i, cnt=0, wc, idx=(slot+1), slice_prox=PASS;
	
	wc=(tdm[slot]==TH2_ANCL_TOKEN)?(tdm[slot]):(tdm_th2_legacy_which_tsc(tdm[slot],tsc));
	if (slot<=(lim-5)) {
		if ( wc==tdm_th2_legacy_which_tsc(tdm[slot+1],tsc) ||
		     wc==tdm_th2_legacy_which_tsc(tdm[slot+2],tsc) ||
		     wc==tdm_th2_legacy_which_tsc(tdm[slot+3],tsc) ||
			 wc==tdm_th2_legacy_which_tsc(tdm[slot+4],tsc) ) {
			slice_prox=FAIL;
		}
	}
	else {
		while (idx<lim) {
			if (wc==tdm_th2_legacy_which_tsc(tdm[idx],tsc)) {
				slice_prox=FAIL;
				break;
			}
			idx++; cnt++;
		}
		for (i=(lim-slot-cnt-1); i>=0; i--) {
			if (wc==tdm_th2_legacy_which_tsc(tdm[i],tsc)) {
				slice_prox=FAIL;
				break;
			}
		}
	}
/* #ifdef _LLS_SCHEDULER */
	{
		int i=slot, j;
		if (speed[tdm[i]]<=SPEED_42G_HG2) {
			if (i<(TH2_VMAP_MAX_LEN-1)) {
				for (j=1; j<11; j++) {
					if (tdm[i+j]==tdm[i]) {
						slice_prox=FAIL;
						break;
					}
				}
			}
		}
	}
/* #endif */

	return slice_prox;
}


/**
@name: tdm_th2_slice_prox_up
@param:

Given port number, checks min spacing in a slice in up direction
 */
int
tdm_th2_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed)
{
	int wc, slice_prox=PASS;
	
	wc=(tdm[slot]==TH2_ANCL_TOKEN)?(tdm[slot]):(tdm_th2_legacy_which_tsc(tdm[slot],tsc));
	if (slot>=4) {
		if ( wc==tdm_th2_legacy_which_tsc(tdm[slot-1],tsc) ||
		     wc==tdm_th2_legacy_which_tsc(tdm[slot-2],tsc) ||
		     wc==tdm_th2_legacy_which_tsc(tdm[slot-3],tsc) ||
			 wc==tdm_th2_legacy_which_tsc(tdm[slot-4],tsc) ) {
			slice_prox=FAIL;
		}
	}
/* #ifdef _LLS_SCHEDULER */
	{
		int i=slot, j;
		if (speed[tdm[i]]<=SPEED_42G_HG2) {
			if (i>=1) {
				for (j=1; j<11; j++) {
					if (tdm[i-j]==tdm[i]) {
						slice_prox=FAIL;
						break;
					}
				}
			}
		}
	}
/* #endif */

	return slice_prox;
}


/**
@name: tdm_th2_check_fit_smooth
@param:

Inside of table array, returns number of nodes inside a port vector that clump with other nodes of the same type
 */
int
tdm_th2_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh)
{
	int i, cnt=0;

	for (i=0; i<lr_idx_limit; i++) {
		if ( (tdm_tbl[i]==port) && (tdm_th2_slice_size_local(i,tdm_tbl,lr_idx_limit)>=clump_thresh) ) {
			cnt++;
		}
	}

	return cnt;

}


/**
@name: tdm_th2_check_lls_flat_up
@param:

Checks LLS scheduler min spacing in tdm array, up direction only, returns dist
 */
int
tdm_th2_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed)
{
	int lls_prox=TH2_VMAP_MAX_LEN;

/* #ifdef _LLS_SCHEDULER */
	{
		int i=idx, j;
		lls_prox=1;
		if (i>=11 && tdm_tbl[idx]<=SPEED_42G_HG2) {
			for (j=1; j<11; j++) {
				if (tdm_tbl[i-j]==tdm_tbl[i]) {
					break;
				}
				lls_prox++;
			}
		}
	}
/* #endif */

	return lls_prox;

}


/**
@name: tdm_th2_slice_prox_local
@param:

Given index, checks min spacing of two nearest non-token ports
 */
int
tdm_th2_slice_prox_local(unsigned short idx, int *tdm, int lim, int **tsc)
{
	int i, prox_len=0, wc=TH2_NUM_EXT_PORTS;
	
	/* Nearest non-token port */
	TH2_TOKEN_CHECK(tdm[idx]) {
		wc=tdm_th2_legacy_which_tsc(tdm[idx],tsc);
	}
	else {
		for (i=1; (idx-i)>=0; i++) {
			TH2_TOKEN_CHECK(tdm[i]) {
				wc=tdm_th2_legacy_which_tsc(tdm[idx-i],tsc);
				break;
			}
		}
	}
	for (i=1; (idx+i)<lim; i++) {
		if (tdm_th2_legacy_which_tsc(tdm[idx+i],tsc)!=wc) {
			prox_len++;
		}
		else {
			break;
		}
	}

	return prox_len;
}


/**
@name: tdm_th2_num_lr_slots
@param:
 */
int
tdm_th2_num_lr_slots(int *tdm_tbl)
{
	int i, cnt=0;
	
	for (i=0; i<TH2_VMAP_MAX_LEN; i++) {
		TH2_TOKEN_CHECK(tdm_tbl[i]) {
			cnt++;
		}
	}
	
	return cnt;
}


/**
@name: tdm_th2_pick_vec
@param:

Select vector index on x axis to rotate based on priority of TSC pipeline
 */
int
tdm_th2_pick_vec( tdm_mod_t *_tdm )
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
