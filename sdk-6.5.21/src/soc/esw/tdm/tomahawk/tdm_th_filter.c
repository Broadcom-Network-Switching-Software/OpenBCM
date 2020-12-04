/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip based calendar postprocessing filters
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_th_filter_check_pipe_sister_min
@param:
 */
int
tdm_th_filter_check_pipe_sister_min( tdm_mod_t *_tdm )
{
	
 	int i, j, idx0, tsc_i, tsc0, result=PASS,
	    *tdm_pipe_main, tdm_pipe_main_len;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
    tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit +
                        _tdm->_chip_data.soc_pkg.tvec_size;
	
	for (i=0; i<tdm_pipe_main_len; i++) {
		TH_TOKEN_CHECK(tdm_pipe_main[i]) {
			tsc_i = tdm_th_scan_which_tsc(tdm_pipe_main[i],_tdm);
			for (j=1; j<VBS_MIN_SPACING; j++){
				idx0 = ((i+j)<tdm_pipe_main_len)? (i+j): (i+j-tdm_pipe_main_len);
				TH_TOKEN_CHECK(tdm_pipe_main[idx0]) {
					tsc0 = tdm_th_scan_which_tsc(tdm_pipe_main[idx0],_tdm);
					if ( tsc_i == tsc0){
						result=FAIL;
						break;
					}
				}
			}
		}
		if (result==FAIL) {break;}
	}
	
	return result;
}


/**
@name: tdm_th_filter_check_port_sister_min
@param:
 */
int
tdm_th_filter_check_port_sister_min( tdm_mod_t *_tdm, int port )
{
	
 	int i, j, idx0, tsc_i, tsc0,result=PASS,
	    *tdm_pipe_main, tdm_pipe_main_len;
	/* enum port_speed_e *tdm_port_speed; */
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_core_data.vars_pkg.cap;
	/* tdm_port_speed    = _tdm->_chip_data.soc_pkg.speed; */
	
	TH_TOKEN_CHECK(port){
		tsc_i = tdm_th_scan_which_tsc(port,_tdm);
		for (i=0; i<tdm_pipe_main_len; i++) {
			if (port==tdm_pipe_main[i]){
				for (j=1; j<VBS_MIN_SPACING; j++){
					idx0 = ((i+j)<tdm_pipe_main_len)? (i+j): (i+j-tdm_pipe_main_len);
					TH_TOKEN_CHECK(tdm_pipe_main[idx0]) {
						tsc0 = tdm_th_scan_which_tsc(tdm_pipe_main[idx0],_tdm);
						if ( tsc_i == tsc0){
							result=FAIL;
							break;
						}
					}
				}
			}
			if (result==FAIL) {break;}
		}
	}
	
	return result;
}

/**
@name: tdm_th_filter_check_migrate_lr_slot
@param:

Migrate Linerate slot from src to dst in an array.
 */
int
tdm_th_filter_check_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc)
{
	int i=idx_src, j, filter_result=FAIL, check_pass=BOOL_TRUE,
	    idx0, idx1, tsc0, tsc1, tsc_i;
	
	if ( !(idx_src>=0 && idx_src<tdm_tbl_len) ||  !(idx_dst>=0 && idx_dst<tdm_tbl_len) ){
		check_pass = BOOL_FALSE;
	}
	if (check_pass==BOOL_TRUE){
		TH_TOKEN_CHECK(tdm_tbl[idx_src]){
			TH_TOKEN_CHECK(tdm_tbl[idx_dst]){
				tsc0 = tdm_th_legacy_which_tsc(tdm_tbl[idx_src],tsc);
				tsc1 = tdm_th_legacy_which_tsc(tdm_tbl[idx_dst],tsc);
				if ( tsc0 == tsc1 ){
					check_pass = BOOL_FALSE;
					TDM_PRINT6(" -------- sister port spacing violation, slots [#%d | #%d],  ports [%d | %d], TSC [%d | %d]\n", idx_src, idx_dst, tdm_tbl[idx_src], tdm_tbl[idx_dst], tsc0, tsc1);
				}
			}
		}
		else {
			check_pass = BOOL_FALSE;
		}
	}
	
	/* Check sister port spacing */
	if (check_pass==BOOL_TRUE){
		i     = idx_dst;
		tsc_i = tdm_th_legacy_which_tsc(tdm_tbl[i],tsc);
		
		for (j=1; j<VBS_MIN_SPACING; j++){
			idx0 = ((i+j)<tdm_tbl_len)? (i+j): (i+j-tdm_tbl_len);
			idx1 = ((i-j)>=0)? (i-j): (i-j+tdm_tbl_len);
			TH_TOKEN_CHECK(tdm_tbl[idx0]){
				tsc0 = tdm_th_legacy_which_tsc(tdm_tbl[idx0],tsc);
				if(tsc0==tsc_i){
					check_pass = BOOL_FALSE;
					TDM_PRINT6(" -------- sister port spacing violation, slots [#%d | #%d],  ports [%d | %d], TSC [%d | %d]\n", i, idx0, tdm_tbl[i], tdm_tbl[idx0], tsc_i, tsc0);
					break;
				}
			}
			TH_TOKEN_CHECK(tdm_tbl[idx1]){
				if (tdm_tbl[idx1]==tdm_tbl[i]){continue;}
				tsc1 = tdm_th_legacy_which_tsc(tdm_tbl[idx1],tsc);
				if(tsc1==tsc_i){
					check_pass = BOOL_FALSE;
					TDM_PRINT6(" -------- sister port spacing violation, slots [#%d | #%d],  ports [%d | %d], TSC [%d | %d]\n", i, idx1, tdm_tbl[i], tdm_tbl[idx1], tsc_i, tsc1);
					break;
				}
			}
		}
	}
	
	filter_result = (check_pass == BOOL_FALSE)? (FAIL): (PASS);
	
	return filter_result;
}


/**
@name: tdm_th_filter_chk_slot_shift_cond
@param:
 */
int
tdm_th_filter_chk_slot_shift_cond(int slot, int dir, int cal_len, int *cal_main, tdm_mod_t *_tdm)
{
    int k, src, dst, port, port_pm, idx, port_k, idx_k, result=PASS;
    int param_phy_lo, param_phy_hi,
        param_space_sister;
    
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    
    if (slot < cal_len) {
        /* determine src and dst index */
        src = slot;
        if (dir == TDM_DIR_UP) {
            dst = (src + cal_len - 1) % cal_len;
        } else {
            dst = (src + 1) % cal_len;
        }

        /* check src port spacing */
        idx     = src;
        port    = cal_main[idx];
        port_pm = tdm_th_filter_get_port_pm(port, _tdm);
        if (port >= param_phy_lo && port <= param_phy_hi) {
            /* sister port spacing */
            for (k=1; k<=param_space_sister; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + cal_len - k) % cal_len;
                } else {
                    idx_k = (idx + k) % cal_len;
                }
                port_k = cal_main[idx_k];
                if (port_k >= param_phy_lo && port_k <= param_phy_hi) {
                    if (tdm_th_filter_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }

        /* check dst port spacing */
        idx     = dst;
        port    = cal_main[idx];
        port_pm = tdm_th_filter_get_port_pm(port, _tdm);
        if (port >= param_phy_lo && port <= param_phy_hi) {
            /* sister port spacing */
            for (k=1; k<=param_space_sister; k++) {
                if (dir == TDM_DIR_UP) {
                    idx_k = (idx + k) % cal_len;
                } else {
                    idx_k = (idx + cal_len - k) % cal_len;
                }
                port_k = cal_main[idx_k];
                if (port_k >= param_phy_lo && port_k <= param_phy_hi) {
                    if (tdm_th_filter_get_port_pm(port_k, _tdm) == port_pm) {
                        result = FAIL;
                        break;
                    }
                }
            }
        }
    }

    return (result);
}


/**
@name: tdm_th_filter_calc_jitter
@param:
 */
int
tdm_th_filter_calc_jitter(int speed, int cal_len, int *space_min, int *space_max)
{
    int slot_req, space_frac, jitter_range, jitter_min=0, jitter_max=0;
    
    if (speed<SPEED_10G) {
        slot_req = (speed/100)/25;
    } else {
        slot_req = ((speed/10000)*100)/25;
    }
    if (slot_req>0) {
        space_frac  = ((cal_len*10)/slot_req)%10;
        jitter_range= (2*cal_len)/(slot_req*5);
        jitter_range= ((((2*cal_len*10)/(slot_req*5))%10)<5)?
                      (jitter_range):
                      (jitter_range+1);
        
        if (space_frac<5){
            jitter_min = cal_len/slot_req - jitter_range/2;
            jitter_max = cal_len/slot_req + jitter_range/2;
            jitter_max = ((jitter_range%2)==0)?
                        (jitter_max):
                        (jitter_max+1);
        } else {
            jitter_min = cal_len/slot_req - jitter_range/2;
            jitter_min = ((cal_len%slot_req)==0)?
                        (jitter_min):
                        (jitter_min+1);
            jitter_min = ((jitter_range%2)==0)?
                        (jitter_min):
                        (jitter_min-1);
            jitter_max = cal_len/slot_req + jitter_range/2;
            jitter_max = ((cal_len%slot_req)==0)?
                        (jitter_max):
                        (jitter_max+1);
        }
        jitter_min = (jitter_min<1)? (1): (jitter_min);
        jitter_max = (jitter_max<1)? (1): (jitter_max);
    }
    *space_min = jitter_min;
    *space_max = jitter_max;
    
    return (PASS);
}


/**
@name: tdm_th_filter_get_same_port_dist
@param:
 */
int
tdm_th_filter_get_same_port_dist(int slot, int dir, int *cal_main, int cal_len)
{
    int n, k, dist = 0;

    if (slot < cal_len && cal_len > 0) {
        if (dir == TDM_DIR_UP) { /* UP direction */
            for (n = 1; n < cal_len; n++) {
                k = (slot + cal_len - n) % cal_len;
                if (cal_main[k] == cal_main[slot]) {
                    dist  = n;
                    break;
                }
            }
        } else { /* DOWN direction */
            for (n = 1; n < cal_len; n++) {
                k = (slot + n) % cal_len;
                if (cal_main[k] == cal_main[slot]) {
                    dist  = n;
                    break;
                }
            }
        }
    }

    return (dist);
}


/**
@name: tdm_th_filter_get_port_pm
@param:

Return Port Macro number of the given port
 */
int
tdm_th_filter_get_port_pm(int port_token, tdm_mod_t *_tdm)
{
    int port_pm;

    if (_tdm->_chip_data.soc_pkg.pmap_num_lanes > 0 &&
        port_token >=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <=_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        port_pm = (port_token - 1) / _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    } else {
        port_pm = _tdm->_chip_data.soc_pkg.pm_num_phy_modules;
    }

    return (port_pm);
/*     
    _tdm->_core_data.vars_pkg.port = port_token;

    return (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)); */
}


/**
@name: tdm_th_filter_get_port_speed
@param:
 */
int
tdm_th_filter_get_port_speed(int port_token, tdm_mod_t *_tdm)
{
    if (port_token >= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo &&
        port_token <= _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi) {
        return (_tdm->_chip_data.soc_pkg.speed[port_token]);
    }
    
    return (SPEED_0);
}


/**
@name: tdm_th_filter_migrate_lr_slot
@param:

Migrate Linerate slot from src to dst in an array.
 */
int
tdm_th_filter_migrate_lr_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc)
{
	int j, port=TH_NUM_EXT_PORTS, filter_result=FAIL, check_pass=BOOL_TRUE;
	
	if (PASS==tdm_th_filter_check_migrate_lr_slot(idx_src, idx_dst, tdm_tbl, tdm_tbl_len, tsc)){
		if (check_pass==BOOL_TRUE){
			TH_TOKEN_CHECK(tdm_tbl[idx_dst]){
				port = tdm_tbl[idx_src];
				if (idx_src<idx_dst){
					for (j=idx_src; j<idx_dst; j++){
						tdm_tbl[j] = tdm_tbl[j+1];
					}
					tdm_tbl[idx_dst] = port;
				}
				else if (idx_src>idx_dst){
					for (j=idx_src; j>idx_dst; j--){
						tdm_tbl[j] = tdm_tbl[j-1];
					}
					tdm_tbl[idx_dst] = port;
				}
				TDM_PRINT3("Filter applied: Linerate Slot Migration, port %3d from index #%03d to index #%03d \n", port, idx_src, idx_dst);
			}
			else{
				port = tdm_tbl[idx_src];
				tdm_tbl[idx_src] = tdm_tbl[idx_dst];
				tdm_tbl[idx_dst] = port;
				TDM_PRINT4("Filter applied: Linerate Slot Migration, port %3d index #%03d, swap with, port %3d index #%03d \n", port, idx_src, tdm_tbl[idx_src], idx_dst);
			}
			filter_result = PASS;
		}
	}
	
	return filter_result;
}


/**
@name: tdm_th_filter_migrate_lr_slot_up
@param:

Migrate Linerate slot from src to dst in an array.
 */
int
tdm_th_filter_migrate_lr_slot_up(tdm_mod_t *_tdm)
{
 	int i, j, k, idx0, tsc_i, tsc0, idx_dst, mig_cnt=0,
	    *tdm_pipe_main, tdm_pipe_main_len, **tdm_port_pmap;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	tdm_port_pmap     = _tdm->_chip_data.soc_pkg.pmap;
	
	for (i=0; i<tdm_pipe_main_len; i++){
		tsc_i = tdm_th_scan_which_tsc(tdm_pipe_main[i],_tdm);
		TH_TOKEN_CHECK(tdm_pipe_main[i]) {
			for (j=1; j<VBS_MIN_SPACING; j++){
				idx0 = ((i+j)<tdm_pipe_main_len)? (i+j): (i+j-tdm_pipe_main_len);
				TH_TOKEN_CHECK(tdm_pipe_main[idx0]) {
					tsc0 = tdm_th_scan_which_tsc(tdm_pipe_main[idx0],_tdm);
					if (tsc_i == tsc0){
						idx_dst = ((i-(VBS_MIN_SPACING-j))>=0)? (i-(VBS_MIN_SPACING-j)): (i-(VBS_MIN_SPACING-j)+tdm_pipe_main_len);
						for (k=0; k<VBS_MIN_SPACING; k++){
							idx_dst = ((idx_dst-k)>=0)? (idx_dst-k): (idx_dst-k+tdm_pipe_main_len);
							if (PASS==tdm_th_filter_check_migrate_lr_slot(i, idx_dst, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap)){
								tdm_th_filter_migrate_lr_slot(i, idx_dst, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap);
								mig_cnt++;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	return mig_cnt;
}


/**
@name: tdm_th_filter_migrate_lr_slot_dn
@param:

Migrate Linerate slot from src to dst in an array.
 */
int
tdm_th_filter_migrate_lr_slot_dn(tdm_mod_t *_tdm)
{
 	int i, j, k, idx0, tsc_i, tsc0, idx_dst, mig_cnt=0,
	    *tdm_pipe_main, tdm_pipe_main_len, **tdm_port_pmap;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	tdm_port_pmap     = _tdm->_chip_data.soc_pkg.pmap;
	
	for (i=0; i<tdm_pipe_main_len; i++){
		tsc_i = tdm_th_scan_which_tsc(tdm_pipe_main[i],_tdm);
		TH_TOKEN_CHECK(tdm_pipe_main[i]) {
			for (j=1; j<VBS_MIN_SPACING; j++){
				idx0 = ((i+j)<tdm_pipe_main_len)? (i+j): (i+j-tdm_pipe_main_len);
				TH_TOKEN_CHECK(tdm_pipe_main[idx0]) {
					tsc0 = tdm_th_scan_which_tsc(tdm_pipe_main[idx0],_tdm);
					if (tsc_i == tsc0){
						idx_dst = ((idx0+VBS_MIN_SPACING-j)<tdm_pipe_main_len)? (idx0+VBS_MIN_SPACING-j): (idx0+VBS_MIN_SPACING-j-tdm_pipe_main_len);
						for (k=0; k<VBS_MIN_SPACING; k++){
							idx_dst = ((idx_dst+k)<tdm_pipe_main_len)? (idx_dst+k): (idx_dst+k-tdm_pipe_main_len);
							if (PASS==tdm_th_filter_check_migrate_lr_slot(idx0, idx_dst, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap)){
								tdm_th_filter_migrate_lr_slot(idx0, idx_dst, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap);
								mig_cnt++;
								break;
							}
						}
					}
				}
			}
		}
	}
	
	return mig_cnt;
}


/**
@name: tdm_th_filter_sister_min
@param:
 */
int
tdm_th_filter_sister_min( tdm_mod_t *_tdm )
{
	
 	int min_space_check, mig_cnt=0, timeout, result=FAIL;
	
	/* DOWN */
	timeout = 20;
	do{
		min_space_check = tdm_th_filter_check_pipe_sister_min(_tdm);
		if (min_space_check==FAIL){
			mig_cnt = tdm_th_filter_migrate_lr_slot_dn(_tdm);
		}
	}
	while ( (min_space_check==FAIL) && (mig_cnt>0) && (timeout--)>0 );
	
	/* UP */
/* 	timeout = 10;
	do{
		min_space_check = tdm_th_filter_check_pipe_sister_min(_tdm);
		if (min_space_check==FAIL){
			mig_cnt = tdm_th_filter_migrate_lr_slot_up(_tdm);
		}
	}
	while ( (min_space_check==FAIL) && (mig_cnt>0) && (timeout--)>0 ); */
	
	if (tdm_th_filter_check_pipe_sister_min(_tdm)==PASS) {result = PASS;}
	
	return result;
}


/**
@name: tdm_th_filter_dither
@param:
 */
int
tdm_th_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed)
{
	int g, i, j, k=1, l=TH_NUM_EXT_PORTS, slice_idx, shift_cnt=0;
	unsigned short dither_shift_done=BOOL_FALSE, dither_done=BOOL_FALSE, dither_slice_counter=0;
	
	/* Get index of the OVSB slot with the largest clump size */
	dither_shift_done=BOOL_FALSE;
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==TH_OVSB_TOKEN && tdm_th_scan_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories), &slice_idx)==tdm_th_slice_size(TH_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))) {
			g=i; while( (tdm_tbl[g]==TH_OVSB_TOKEN) && (g<(lr_idx_limit+accessories)) ) {g++;}
			if ( tdm_th_slice_prox_up(g,tdm_tbl,tsc,speed) &&
				 tdm_th_slice_prox_dn((tdm_th_slice_idx(TH_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))-1),tdm_tbl,(lr_idx_limit+accessories),tsc,speed) ) {
				l=tdm_tbl[i];
				for (j=i; j<255; j++) {
					tdm_tbl[j]=tdm_tbl[j+1];
				}
				k=i; dither_shift_done=BOOL_TRUE;
				break;
			}
		}
	}
	/* Get index of the LINERATE slot with the largest clump size, then insert OVSB slot. */
	dither_done=BOOL_FALSE;
	if (dither_shift_done) {
		for (i=1; i<(lr_idx_limit+accessories); i++) {
			dither_slice_counter=0;
			while (tdm_tbl[i]!=TH_OVSB_TOKEN && tdm_th_scan_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories),&slice_idx)==tdm_th_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && i<(lr_idx_limit+accessories) ) {
				if (++dither_slice_counter>=threshold && tdm_tbl[i-1]==TH_OVSB_TOKEN) {
					for (j=255; j>i; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[i+1]=TH_OVSB_TOKEN;
					dither_done=BOOL_TRUE;
					break;
				}
				i++; if (tdm_tbl[i]==TH_OVSB_TOKEN) {i++;}
			}
			if (dither_done) {
				break;
			}
		}
		if (!dither_done) {
			for (j=255; j>k; j--) {
				tdm_tbl[j]=tdm_tbl[j-1];
			}
			tdm_tbl[k]=l;
		}
		else {
			shift_cnt++;
			TDM_PRINT0("Filter applied: Dither (quantization correction)\n");
		}
	}
	
	return shift_cnt;
}


/**
@name: tdm_th_filter_fine_dither
@param:
 */
int
tdm_th_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc)
{
	int i, j, k, min_prox=11,slice_idx;
	unsigned short fine_dither_done;

	i=port; fine_dither_done=BOOL_FALSE;
	for (j=2; j<(lr_idx_limit+accessories-4); j++) {
		if ( tdm_tbl[j]!=TH_OVSB_TOKEN && tdm_tbl[j-2]!=TH_OVSB_TOKEN && tdm_tbl[j+2]!=TH_OVSB_TOKEN && tdm_tbl[j+4]!=TH_OVSB_TOKEN && 
			 tdm_th_scan_slice_size_local((j-2), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_th_scan_slice_size_local( j,    tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_th_scan_slice_size_local((j+2), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_th_scan_slice_size_local((j+4), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 &&
			 tdm_th_slice_prox_local( j,    tdm_tbl, (lr_idx_limit+accessories), tsc)>min_prox )
		{
			fine_dither_done=BOOL_TRUE;
			break;
		}
	}
	if (fine_dither_done) {
		TDM_PRINT1("Filter applied: Fine dithering (normal), index %0d\n",port);

		for (k=(j+1); k<(lr_idx_limit+accessories); k++) {
			tdm_tbl[k]=tdm_tbl[k+1];
		}
		for (k=255; k>i; k--) {
			tdm_tbl[k]=tdm_tbl[k-1];
		}
		tdm_tbl[i]=TH_OVSB_TOKEN;
		
		return PASS;
	}
	else {
		return FAIL;
	}
}


/**
@name: tdm_th_filter_shift_lr_port
@param:

Shifts all slots of the given linerate port up or down,
and returns the total number of shifting occurence.
 */
int
tdm_th_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir)
{
	int i, shift_cnt=0, port_swap;
	
	TH_TOKEN_CHECK(port) {
		if (dir==DN) {
			for (i=0; i<(tdm_tbl_len-1); i++) {
				if (tdm_tbl[i]==port) {
					port_swap=tdm_tbl[i+1];
					tdm_tbl[i+1]=port;
					tdm_tbl[i]=port_swap;
					shift_cnt++;
					i++;
				}
			}
		}
		else if (dir==UP) {
			for (i=1; i<tdm_tbl_len; i++) {
				if (tdm_tbl[i]==port) {
					port_swap=tdm_tbl[i-1];
					tdm_tbl[i-1]=port;
					tdm_tbl[i]=port_swap;
					shift_cnt++;
				}
			}
		}
	}
	
	return shift_cnt;
}


/**
@name: tdm_th_filter_migrate_os_slot
@param:

Migrate OVSB slot from src to dst in an array.
 */
int
tdm_th_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i=idx_src, j, token_tmp, filter_result=FAIL, check_pass=BOOL_TRUE,
	    idx0, idx1, tsc0, tsc1;
	
	if ( !(idx_src>=0 && idx_src<tdm_tbl_len) ||  !(idx_dst>=0 && idx_dst<tdm_tbl_len) ){
		check_pass = BOOL_FALSE;
	}
	if ( tdm_tbl[idx_src]!=TH_OVSB_TOKEN && tdm_tbl[idx_src]!=TH_ANCL_TOKEN) {
		check_pass = BOOL_FALSE;
	}
	/* Check sister port spacing */
	if (check_pass==BOOL_TRUE){
		i = idx_src;
		for (j=1; j<VBS_MIN_SPACING; j++){
			idx0 = ((i+j)<tdm_tbl_len)? (i+j): (i+j-tdm_tbl_len);
			idx1 = ((idx0-VBS_MIN_SPACING)>=0) ? (idx0-VBS_MIN_SPACING) : (idx0-VBS_MIN_SPACING+tdm_tbl_len);
			TH_TOKEN_CHECK(tdm_tbl[idx0]){
				TH_TOKEN_CHECK(tdm_tbl[idx1]){
					tsc0 = tdm_th_legacy_which_tsc(tdm_tbl[idx0],tsc);
					tsc1 = tdm_th_legacy_which_tsc(tdm_tbl[idx1],tsc);
					if(tsc0==tsc1){
						check_pass = BOOL_FALSE;
						break;
					}
				}
			}
		}
	}
	/* Check same port spacing */
	if (check_pass==BOOL_TRUE){
		for (j=1; j<LLS_MIN_SPACING; j++){
			idx0 = ((i+j)<tdm_tbl_len)? (i+j): (i+j-tdm_tbl_len);
			idx1 = ((idx0-LLS_MIN_SPACING)>=0)? (idx0-LLS_MIN_SPACING): (idx0-LLS_MIN_SPACING+tdm_tbl_len);
			TH_TOKEN_CHECK(tdm_tbl[idx0]){
				TH_TOKEN_CHECK(tdm_tbl[idx1]){
					if ( speed[tdm_tbl[idx0]]<=SPEED_42G_HG2 && tdm_tbl[idx0]==tdm_tbl[idx1]){
						check_pass = BOOL_FALSE;
						break;
					}
				}
			}
		}
	}
	/* Migrate OVSB/ANCL slot from src to dst */
	if (check_pass==BOOL_TRUE){
		token_tmp = tdm_tbl[idx_src];
		idx_dst   = (idx_src>idx_dst)? (idx_dst): (idx_dst-1);
		for (j=idx_src; j<tdm_tbl_len; j++){
			tdm_tbl[j] = tdm_tbl[j+1];
		}
		for (j=(tdm_tbl_len-1); j>idx_dst; j--){
			tdm_tbl[j] = tdm_tbl[j-1];
		}
		tdm_tbl[idx_dst] = token_tmp;
		filter_result = PASS;
		
		TDM_PRINT2("Filter applied: OVSB Slot Migration, from index #%03d to index #%03d \n", idx_src, idx_dst);
	}
	
	return filter_result;
}


/**
@name: tdm_th_filter_smooth_idle_slice
@param:

Smooth MMU TDM calendar by migrating IDLE slots
 */
int
tdm_th_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, k, idle1_token, idle2_token, pos_cnt, pos_step, pos,
	    idle_slot_cnt=0, idle_slice_max, idle_slice_max_idx, lr_slice_max, lr_slice_max_idx, 
		idx_x, idx_y, idx1, idx2, tsc_x, tsc_y, tsc1, tsc2, slot_token, slot_dst_idx,
		filter_done, filter_cnt=0;
	
	idle1_token = TH_IDL1_TOKEN;
	idle2_token = TH_IDL2_TOKEN;
	
	for (i=0; i<tdm_tbl_len; i++){
		if ( tdm_tbl[i]==idle1_token || 
			 tdm_tbl[i]==idle2_token ){
			idle_slot_cnt++;
		}
	}
	if(idle_slot_cnt>0){
		pos_cnt = 0;
		pos_step= tdm_tbl_len/4; 
		for (k=0; k<idle_slot_cnt; k++){
			filter_done= BOOL_TRUE;
			pos    = pos_step*pos_cnt;
			pos_cnt= (pos_cnt>=3)?(0):(pos_cnt+1);
			idle_slice_max = tdm_th_scan_slice_max(idle1_token, tdm_tbl,tdm_tbl_len, &idle_slice_max_idx,0);
			lr_slice_max   = tdm_th_scan_mix_slice_max(1,tdm_tbl,tdm_tbl_len, &lr_slice_max_idx,pos);
			
			/* Find idle clump with minimum size 2: ..._y_x_IDLE_IDLE_... */
			if ( (idle_slice_max>1 && lr_slice_max>0) &&
			     (idle_slice_max_idx<tdm_tbl_len && idle_slice_max_idx>1) &&
			     ((lr_slice_max_idx<tdm_tbl_len) && ((lr_slice_max_idx>0) || (lr_slice_max_idx==0 && lr_slice_max>1))) ){
				/* Check sister port spacing */
				idx_x = idle_slice_max_idx-1;
				idx_y = idle_slice_max_idx-2;
				idx1  = ((idle_slice_max_idx+2)<tdm_tbl_len) ? (idle_slice_max_idx+2) : ((idle_slice_max_idx+2)-tdm_tbl_len);
				idx2  = ((idle_slice_max_idx+3)<tdm_tbl_len) ? (idle_slice_max_idx+3) : ((idle_slice_max_idx+3)-tdm_tbl_len);
				tsc_x = tdm_th_legacy_which_tsc(tdm_tbl[idx_x],tsc);
				tsc_y = tdm_th_legacy_which_tsc(tdm_tbl[idx_y],tsc);
				tsc1  = tdm_th_legacy_which_tsc(tdm_tbl[idx1],tsc);
				tsc2  = tdm_th_legacy_which_tsc(tdm_tbl[idx2],tsc);
				if(tsc_x!=tsc1 && tsc_x!=tsc2 && tsc_y!=tsc1){
					slot_token  = tdm_tbl[idle_slice_max_idx];
					slot_dst_idx= lr_slice_max_idx+(lr_slice_max/2);
					for (i=idle_slice_max_idx; i<tdm_tbl_len; i++){
						tdm_tbl[i] = tdm_tbl[i+1];
					}
					for (i=(tdm_tbl_len-1); i>slot_dst_idx; i--){
						tdm_tbl[i] = tdm_tbl[i-1];
					}
					tdm_tbl[slot_dst_idx] = slot_token;
					filter_done = BOOL_FALSE;
					filter_cnt++;
					
					TDM_PRINT2("Filter applied: IDLE slot moving from index #%03d to index #%03d\n", idle_slice_max_idx, slot_dst_idx);
				}
			}
			if(filter_done == BOOL_TRUE){
				break;
			}
		}
	}
	
	return filter_cnt;
}



/**
@name: tdm_th_filter_smooth_os_slice
@param:

Smooth MMU TDM calendar by shifting linerate ports Downward/Upward
		Downward: x_ovsb___x_ovsb___x_ovsb -> ovsb_x___ovsb_x___ovsb_x
		Upward  : ovsb_x___ovsb_x___ovsb_x -> x_ovsb___x_ovsb___x_ovsb
 */
int
tdm_th_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir)
{
	int i, slice_idx, shift_dir, shift_cnt=0, lr_clump_max, lr_clump_min, filter_port=0;
	const char *str_dir;
	
	str_dir      = (dir==UP) ? "UPWARD": "DOWNWARD";
	shift_dir    = (dir==UP) ? (UP) : (DN);
	
	for (i=1; i<tdm_tbl_len; i++) {
		filter_port=tdm_tbl[i];
		if (filter_port==tdm_tbl[0]) {continue;}
		
		lr_clump_max = tdm_th_scan_mix_slice_max(1,tdm_tbl,tdm_tbl_len, &slice_idx, 0);
		lr_clump_min = tdm_th_scan_mix_slice_min(1,tdm_tbl,tdm_tbl_len, &slice_idx, 0);
		if ( (lr_clump_max<=1) || 
		     (lr_clump_max==2 && lr_clump_min==1)){
			break;
		}
		if ( FAIL==tdm_th_check_shift_cond_pattern(filter_port, tdm_tbl, tdm_tbl_len, tsc, dir) ) {
			continue;
		}
		else if ( FAIL==tdm_th_check_shift_cond_local_slice(filter_port, tdm_tbl, tdm_tbl_len, tsc, dir) ) {
			continue;
		}
		else {
			tdm_th_filter_shift_lr_port(filter_port, tdm_tbl, tdm_tbl_len, shift_dir);
			TDM_PRINT3("Filter applied: Vector shift %8s, port %3d, beginning index #%03d \n", str_dir, filter_port, i);
			shift_cnt++;
		}
	}
	
	return shift_cnt;
}


/**
@name: tdm_th_filter_smooth_os_slice_fine
@param:

Smooth MMU TDM calendar by migrating OVSB slot
 */
int
tdm_th_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int j, k, ovsb_token, pos, pos_step, pos_cnt, pos_cnt_max,
	    slice_max, slice_min, slice_max_idx, slice_min_idx, pp_prox,
		idx0, idx1, tsc0, tsc1, slot_tmp, idx_dst, idx_src, filter_cnt=0;
	
	ovsb_token  = TH_OVSB_TOKEN;
	pos         = 0;
	pos_cnt     = 0;
	pos_cnt_max = 16;
	pos_step    = tdm_tbl_len/pos_cnt_max + 1;
	
	for (k=0; k<tdm_tbl_len; k++){
		pos      = pos_cnt*pos_step;
		pos_cnt  = ((pos_cnt+1)<pos_cnt_max)? (pos_cnt+1): (0);
		slice_max= tdm_th_scan_slice_max(ovsb_token,tdm_tbl,tdm_tbl_len, &slice_max_idx,k);
		slice_min= tdm_th_scan_slice_min(ovsb_token,tdm_tbl,tdm_tbl_len, &slice_min_idx,pos);
		
		if (slice_max_idx < k) {break;}
		if ( (slice_max-slice_min)<2 || 
		    !(slice_max_idx>0 && slice_max_idx<tdm_tbl_len) || 
			!(slice_min_idx>0 && slice_min_idx<tdm_tbl_len) ){
			break;	
		}
		pp_prox = PASS;
		idx_src = slice_max_idx;
		idx_dst = slice_min_idx;
		/* Check sister port spacing */
		idx0 = idx_src-1;
		idx1 = ((idx0+VBS_MIN_SPACING)<tdm_tbl_len) ? (idx0+VBS_MIN_SPACING) : (idx0+VBS_MIN_SPACING-tdm_tbl_len);
		TH_TOKEN_CHECK(tdm_tbl[idx0]){
			TH_TOKEN_CHECK(tdm_tbl[idx1]){
				tsc0 = tdm_th_legacy_which_tsc(tdm_tbl[idx0],tsc);
				tsc1 = tdm_th_legacy_which_tsc(tdm_tbl[idx1],tsc);
				if ( tsc0==tsc1 ){
					pp_prox=FAIL;
				}
			}
		}
		/* Check same port spacing */
		for (j=1; j<LLS_MIN_SPACING; j++){
			idx0 = ((idx_src-j)>=0)? (idx_src-j): (idx_src-j+tdm_tbl_len);
			idx1 = ((idx0+LLS_MIN_SPACING)<tdm_tbl_len)? (idx0+LLS_MIN_SPACING): (idx0+LLS_MIN_SPACING-tdm_tbl_len);
			TH_TOKEN_CHECK(tdm_tbl[idx0]){
				TH_TOKEN_CHECK(tdm_tbl[idx1]){
					if ( speed[tdm_tbl[idx0]]<=SPEED_42G_HG2 && tdm_tbl[idx0]==tdm_tbl[idx1]){
						pp_prox=FAIL;
						break;
					}
				}
			}
		}
		/* Migrate ovsb slot to from src to dst */
		if (pp_prox==PASS) {
			slot_tmp = tdm_tbl[idx_src];
			idx_dst  = (idx_src>idx_dst)? (idx_dst): (idx_dst-1);
			for (j=idx_src; j<tdm_tbl_len; j++){
				tdm_tbl[j] = tdm_tbl[j+1];
			}
			for (j=(tdm_tbl_len-1); j>idx_dst; j--){
				tdm_tbl[j] = tdm_tbl[j-1];
			}
			tdm_tbl[idx_dst] = slot_tmp;
			
			filter_cnt++;
			TDM_PRINT2("Filter applied: OVSB slot migrating from index #%03d to index #%03d\n", idx_src, idx_dst);
		}
		else{
			k = slice_max_idx+slice_max;
		}
	}
	
	return filter_cnt;
}


/**
@name: tdm_th_filter_smooth_os_os_up
@param:

Smooth MMU TDM calendar by transforming:
        --- _x_y_ovsb_ovsb_ -> _x_ovsb_y_ovsb_
 */
int tdm_th_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, slot_tmp, filter_cnt=0;
	
	for (i=2; i<(tdm_tbl_len-1); i++) {
		if (tdm_tbl[i  ]==TH_OVSB_TOKEN &&
			tdm_tbl[i+1]==TH_OVSB_TOKEN ){
			TH_TOKEN_CHECK(tdm_tbl[i-1]){
				TH_TOKEN_CHECK(tdm_tbl[i-2]){
					if ( PASS==tdm_th_check_slot_swap_cond((i-1), tdm_tbl, tdm_tbl_len, tsc, speed) ){
						slot_tmp     = tdm_tbl[i-1];
						tdm_tbl[i-1] = tdm_tbl[i];
						tdm_tbl[i  ] = slot_tmp;
						filter_cnt++;
						i += 3;
						TDM_PRINT1("Filter applied: Local OVSB slot UP, index #%03d\n", i);
					}
				}
			}
		}
	}
	
	return filter_cnt;
}


/**
@name: tdm_th_filter_smooth_os_os_dn
@param:

Smooth MMU TDM calendar by transforming:
		--- _ovsb_ovsb_x_y_ -> _ovsb_x_ovsb_y_
 */
int tdm_th_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, slot_tmp, filter_cnt=0;
	
	for (i=1; i<(tdm_tbl_len-3); i++) {
		if (tdm_tbl[i  ]==TH_OVSB_TOKEN &&
			tdm_tbl[i+1]==TH_OVSB_TOKEN ){
			TH_TOKEN_CHECK(tdm_tbl[i+2]){
				TH_TOKEN_CHECK(tdm_tbl[i+3]){
					if ( PASS==tdm_th_check_slot_swap_cond((i+1), tdm_tbl, tdm_tbl_len, tsc, speed) ){
						slot_tmp     = tdm_tbl[i+1];
						tdm_tbl[i+1] = tdm_tbl[i+2];
						tdm_tbl[i+2] = slot_tmp;
						filter_cnt++;
						i += 3;
						TDM_PRINT1("Filter applied: Local OVSB slot UP, index #%03d\n", i);
					}
				}
			}
		}
	}
	
	return filter_cnt;
}


/**
@name: tdm_th_filter_smooth_ancl
@param:

Smooth MMU TDM calendar with ANCILLARY slots
 */
int
tdm_th_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min)
{
	int i, j, g, l, pool, ancl_num=0, ancl_dist_up, ancl_dist_dn, lr_slice_size, lr_slice_min, slice_idx, timeout;
	
	lr_slice_min = tdm_th_scan_slice_min(1, tdm_tbl, tdm_tbl_len, &slice_idx,0);
	/* Retrace ancillary slots */
	for (i=0; i<tdm_tbl_len; i++) {
		if (tdm_tbl[i]==ancl_token) {
			for (j=i; j<tdm_tbl_len; j++) {
				tdm_tbl[j]=tdm_tbl[j+1];
			}
			i--; ancl_num++;
		}
	}
	/* Smooth TDM table with ancillary slots */
	pool = ancl_num;
	timeout=tdm_tbl_len;
	while( (pool>0) && ((--timeout)>0) ){
		for (i=1; i<tdm_tbl_len; i++) {
			if (pool<1){break;}
			TH_TOKEN_CHECK(tdm_tbl[i]) {
				lr_slice_size = tdm_th_scan_slice_size_local(i,tdm_tbl,tdm_tbl_len,&slice_idx);
				ancl_dist_up  = tdm_th_check_same_port_dist_up_port(ancl_token,i,tdm_tbl,tdm_tbl_len);
				ancl_dist_dn  = tdm_th_check_same_port_dist_dn_port(ancl_token,i,tdm_tbl,tdm_tbl_len);
				
				if ( lr_slice_size<=lr_slice_min && 
				     ancl_dist_up>ancl_space_min && 
					 ancl_dist_dn>ancl_space_min ){
					for (j=tdm_tbl_len; j>i; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[i]=ancl_token;
					pool--;
					TDM_PRINT1("Filter applied: Ancillary filter smoothing at index %0d\n",i);
				}
			}
		}
		lr_slice_min++;
	}
	/* Roll back if filter failed */
	if(pool>0 && ancl_num>0 ){
		TDM_PRINT0("Filter failed: Ancillary Filter failed, MMU TDM table rolling back\n");
		for (i=0; i<tdm_tbl_len; i++) {
			if (tdm_tbl[i]==ancl_token) {
				for (j=i; j<tdm_tbl_len; j++) {
					tdm_tbl[j]=tdm_tbl[j+1];
				}
			}
		}
		for (j=1; j<=ancl_num; j++) {
			g=tdm_PQ((((10*(tdm_tbl_len-ancl_num))/ancl_num)*j))+(j-1);
			for (l=255; l>g; l--) {
				tdm_tbl[l]=tdm_tbl[l-1];
			}
			tdm_tbl[g]=ancl_token;
		}
	}
	
	return PASS;
}


/**
@name: tdm_th_filter_chain
@param:
 */
int
tdm_th_filter_tdm5( tdm_mod_t *_tdm )
{
	int i, timeout=TIMEOUT, filter_result, filter_cnt, slice_idx,
		*tdm_pipe_main, tdm_pipe_main_len, **tdm_port_pmap, ovsb_token;
    int k, max_size, min_size, max_size_k, min_size_k, 
        max_slice_idx, min_slice_idx;
	enum port_speed_e *tdm_port_speed;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	tdm_port_pmap = _tdm->_chip_data.soc_pkg.pmap;
	tdm_port_speed= _tdm->_chip_data.soc_pkg.speed;
	ovsb_token    = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    
	/* tdm_th_print_tbl(_tdm); */
	TDM_BIG_BAR
	TDM_PRINT0("Filters Applied to smooth MMU/IDB TDM calendar\n");
	TDM_SML_BAR
    
	/* Sister port space */
    if (_tdm->_core_data.vars_pkg.lr_enable==BOOL_TRUE){
        filter_result = tdm_th_filter_sister_min(_tdm );
        if (filter_result==PASS){
            TDM_PRINT0("\nFilter done: ---SISTER MIN SPACING FILTER, PASS\n");
        }
        else {
            TDM_WARN1("\nFilter done: ---SISTER MIN SPACING FILTER, FAIL, min space violation in Pipe %d\n", _tdm->_core_data.vars_pkg.cal_id);
        }
        TDM_SML_BAR	
    }
	
	/* Case 1: linerate only */
	if(_tdm->_core_data.vars_pkg.lr_enable && !(_tdm->_core_data.vars_pkg.os_enable) && !(_tdm->_core_data.vars_pkg.refactor_done)){
		filter_result = tdm_th_filter_smooth_idle_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---IDLE SLOT SMOOTH, filter applied <%d> times\n", filter_result);
		TDM_SML_BAR
	}
	/* Case 2: linerate and oversub mixed */
	if (_tdm->_core_data.vars_pkg.lr_enable && _tdm->_core_data.vars_pkg.os_enable && !(_tdm->_core_data.vars_pkg.refactor_done)) {
		/* Smooth the extremely unbalanced ovsb clump */
		filter_cnt= 0;
		max_size = tdm_th_scan_mix_slice_max(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &max_slice_idx, 0);
		min_size = tdm_th_scan_mix_slice_min(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &min_slice_idx, 0);
		if( (max_size-min_size)>=6 ){
			for (k=1; k<max_size-min_size; k++){
				filter_result = tdm_th_filter_migrate_os_slot(max_slice_idx, min_slice_idx, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed );
				if(filter_result==FAIL){break;}
				filter_cnt++;
				max_size_k = tdm_th_scan_mix_slice_max(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &max_slice_idx, 0);
				min_size_k = tdm_th_scan_mix_slice_min(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &min_slice_idx, 0);
				if ((max_size_k-min_size_k)<=1) {break;}
			}
		}
		TDM_PRINT1("\nFilter done: ---OVSB MAX/MIN SLICE BALANCE, filter applied <%d> times\n", filter_cnt);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_cnt = 0;
		timeout = _tdm->_chip_data.soc_pkg.lr_idx_limit;
		do{
			filter_result = tdm_th_filter_smooth_os_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed, DN);
			TDM_PRINT2("\nFilter done: ---OVSB MAX SLICE REDUCTION (1) Shift Down (%2d), filter applied <%d> times\n", ++filter_cnt, filter_result);
			TDM_SML_BAR
		} while ( filter_result>0 && (timeout--)>0 );
		filter_cnt = 0;
		timeout = _tdm->_chip_data.soc_pkg.lr_idx_limit;
		do{
			filter_result = tdm_th_filter_smooth_os_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed, UP);
			TDM_PRINT2("\nFilter done: ---OVSB MAX SLICE REDUCTION (2) Shift Up (%2d), filter applied <%d> times\n", ++filter_cnt, filter_result);
			TDM_SML_BAR
		} while ( filter_result>0 && (timeout--)>0 );

		/* Smooth oversub distribution */
		filter_result = tdm_th_filter_smooth_os_os_up(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB LOCAL SLICE REDUCTION (1) slot UP, X_Y_OVSB_OVSB, filter applied <%d> times \n", filter_result);
		TDM_SML_BAR
		filter_result = tdm_th_filter_smooth_os_os_dn(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB LOCAL SLICE REDUCTION (2) slot DOWN, OVSB_OVSB_X_Y, filter applied <%d> times \n", filter_result);
		TDM_SML_BAR

		/* Smooth oversub distribution */
		filter_result = tdm_th_filter_smooth_os_slice_fine(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB SLOT MIGRATION, filter applied <%d> times\n", filter_result);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_result = 0;
		timeout=DITHER_PASS;
		while ( tdm_th_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_th_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_th_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
		       ((--timeout)>0) ) {			
			filter_result += tdm_th_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,DITHER_THRESHOLD,tdm_port_speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_th_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_th_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_th_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
				((--timeout)>0) ) {			
			filter_result += tdm_th_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE),tdm_port_speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_th_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_th_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_th_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
				((--timeout)>0) ) {			
			filter_result += tdm_th_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE-DITHER_SUBPASS_STEP_SIZE),tdm_port_speed);
		}
		TDM_PRINT1("Filter done: ---DITHER (1) filter finished, applied times %d\n", filter_result);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_result = 0;
		if (tdm_th_slice_size(1, tdm_pipe_main, tdm_pipe_main_len)==2) {
			for (i=3; i<tdm_pipe_main_len; i++) {
				if (tdm_pipe_main[i-3]!=ovsb_token && 
				    tdm_pipe_main[i  ]!=ovsb_token && 
					tdm_pipe_main[i+3]!=ovsb_token && 
					tdm_pipe_main[i+6]!=ovsb_token && 
					tdm_th_scan_slice_size_local((i-3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_th_scan_slice_size_local( i,    tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_th_scan_slice_size_local((i+3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_th_scan_slice_size_local((i+6), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2) {
					if (tdm_th_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap)==FAIL) {
						break;
					}
					filter_result++;
				}
			}
			for (i=3; i<tdm_pipe_main_len; i++) {
				if (tdm_pipe_main[i-3]!=ovsb_token && 
				    tdm_pipe_main[i  ]!=ovsb_token && 
					tdm_pipe_main[i+3]!=ovsb_token && 
					tdm_th_scan_slice_size_local((i-3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_th_scan_slice_size_local( i,    tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_th_scan_slice_size_local((i+3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2) {
					if (tdm_th_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap)==FAIL) {
						break;
					}
					filter_result++;
				}
			}
		}
		TDM_PRINT1("\nFilter done: ---DITHER (2) filter finished, applied times %d\n", filter_result);
		TDM_SML_BAR
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__PARSE]( _tdm ) );
}


/**
@name: tdm_th_filter_ovsb_p1
@param:
 */
int
tdm_th_filter_ovsb_p1(tdm_mod_t *_tdm)
{
        int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_space_sister,
        param_token_ovsb, param_token_ancl,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth dual OVSB pattern: Z_Y_OVSB_X_OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            2*os_slot_cnt>lr_slot_cnt){
            for (i=0; i<(param_cal_len-6); i++){
                /* z_y_ovsb_x_ovsb_ovsb -> z_ovsb_y_ovsb_x_ovsb */
                if ((param_cal_main[i]  !=param_token_ovsb && param_cal_main[i]  !=param_token_ancl) &&
                    (param_cal_main[i+1]!=param_token_ovsb && param_cal_main[i+1]!=param_token_ancl) &&
                    (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                    (param_cal_main[i+3]!=param_token_ovsb && param_cal_main[i+3]!=param_token_ancl) &&
                    (param_cal_main[i+4]==param_token_ovsb || param_cal_main[i+4]==param_token_ancl) &&
                    (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) ){
                    sister_prox = PASS;
                    x = i+3;
                    y = (x+param_space_sister)%param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        x = i+1;
                        port_x = param_cal_main[x];
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        
                        filter_cnt++;
                        TDM_PRINT4("Shift OVSB slot UP (1)from %03d to %03d (2)from %03d to %03d\n",
                                  i+4, i+3, i+2, i+1);
                    }
                }
            }
        }
    }
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_5x
@param:
 */
int
tdm_th_filter_ovsb_5x(tdm_mod_t *_tdm)
{
    int i, j, k, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
    x, port_x,
        idx, idx_up, idx_dn, dist_up, dist_dn;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb, param_token_ancl,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth quadrant OVSB pattern: OVSB_OVSB_OVSB_OVSB_OVSB \n\n");
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
            /* smooth ovsb_ovsb_ovsb_ovsb_ovsb_ovsb by ANCL */
            for (i=0; (i+4)<param_cal_len; i++){
                if ((param_cal_main[i]  ==param_token_ovsb) &&
                    (param_cal_main[i+1]==param_token_ovsb) &&
                    (param_cal_main[i+2]==param_token_ovsb) &&
                    (param_cal_main[i+3]==param_token_ovsb) &&
                    (param_cal_main[i+4]==param_token_ovsb)){
                    idx_up = param_cal_len;
                    idx_dn = param_cal_len;
                    idx    = param_cal_len;
                    for (j=1; j<param_cal_len; j++){
                        k = (param_cal_len+i-j)%param_cal_len;
                        if (param_cal_main[k]==param_token_ancl){
                            idx_up = k;
                            break;
                        }
                    }
                    for (j=1; j<param_cal_len; j++){
                        k = (i+j)%param_cal_len;
                        if (param_cal_main[k]==param_token_ancl){
                            idx_dn = k;
                            break;
                        }
                    }
                    if (idx_up<param_cal_len || idx_dn<param_cal_len){
                        if (idx_up<param_cal_len && idx_dn<param_cal_len){
                            dist_up = (idx_up<i)?(i-idx_up):(i+param_cal_len-idx_up);
                            dist_dn = (idx_dn>i)?(idx_dn-i):(param_cal_len-i+idx_dn);
                            idx = (dist_up<dist_dn)?(idx_up):(idx_dn);
                        }
                        else if (idx_up<param_cal_len) {
                            idx = idx_up;
                        }
                        else if (idx_dn<param_cal_len) {
                            idx = idx_dn;
                        }
                    }
                    if (idx>=0 && idx<param_cal_len){
                        x      = i+2;
                        port_x = param_cal_main[x];
                        param_cal_main[x]  = param_cal_main[idx];
                        param_cal_main[idx]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("(1)Swap ANCL with OVSB from %03d to %03d\n",
                                  idx, x);
                    }
                }
                else if ((param_cal_main[i]  ==param_token_ovsb || param_cal_main[i]  ==param_token_ancl) &&
                         (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                         (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                         (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                         (param_cal_main[i+4]==param_token_ovsb || param_cal_main[i+4]==param_token_ancl)){
                    idx = param_cal_len;
                    for (j=0; j<=4; j++){
                        if(param_cal_main[i+j]==param_token_ancl){
                            idx = i+j;
                            break;
                        }
                    }
                    if (idx>=0 && idx<param_cal_len){
                        x      = i+2;
                        port_x = param_cal_main[x];
                        param_cal_main[x]  = param_cal_main[idx];
                        param_cal_main[idx]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("(2)Swap ANCL with OVSB from %03d to %03d\n",
                                  idx, x);
                    }
                }
            }
            
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_4x
@param:
 */
int
tdm_th_filter_ovsb_4x(tdm_mod_t *_tdm)
{
    int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb, param_token_ancl,
        param_space_sister,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth quadrant OVSB pattern: OVSB_OVSB_OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
            /* ovsb_ovsb_ovsb_ovsb_x */
            for (i=0; i<(param_cal_len-6); i++){
                /* ovsb_ovsb_ovsb_ovsb_x_ovsb_y -> ovsb_ovsb_ovsb_ovsb_x_ovsb_ovsb_y */
                if ((param_cal_main[i]  ==param_token_ovsb || param_cal_main[i]  ==param_token_ancl) &&
                    (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                    (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                    (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                    (param_cal_main[i+4]!=param_token_ovsb && param_cal_main[i+4]!=param_token_ancl) &&
                    (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) &&
                    (param_cal_main[i+6]!=param_token_ovsb && param_cal_main[i+6]!=param_token_ancl) ){
                    x = i+4;
                    port_x = param_cal_main[x];
                    param_cal_main[x]  = param_cal_main[x-1];
                    param_cal_main[x-1]= port_x;
                    filter_cnt++;
                    TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                              x-1, x);
                }
                /* ovsb_ovsb_ovsb_ovsb_x_y -> ovsb_ovsb_x_ovsb_ovsb_y */
                else if ((param_cal_main[i]  ==param_token_ovsb || param_cal_main[i]  ==param_token_ancl) &&
                         (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                         (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                         (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                         (param_cal_main[i+4]!=param_token_ovsb && param_cal_main[i+4]!=param_token_ancl) &&
                         (param_cal_main[i+5]!=param_token_ovsb && param_cal_main[i+5]!=param_token_ancl) ){
                    sister_prox = PASS;
                    x = i+4;
                    y = ((x-2)>=(param_space_sister-1))?
                        (x-2-param_space_sister+1):
                        (param_cal_len+x-2-param_space_sister+1);
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x-2];
                        param_cal_main[x-2]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-2, x);
                    }
                }
            }
            /* x_ovsb_ovsb_ovsb_ovsb */
            for (i=0; i<(param_cal_len-6); i++){
                /* y_ovsb_x_ovsb_ovsb_ovsb_ovsb -> y_ovsb_ovsb_x_ovsb_ovsb_ovsb */
                if ((param_cal_main[i]  !=param_token_ovsb && param_cal_main[i]  !=param_token_ancl) &&
                    (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                    (param_cal_main[i+2]!=param_token_ovsb && param_cal_main[i+2]!=param_token_ancl) &&
                    (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                    (param_cal_main[i+4]==param_token_ovsb || param_cal_main[i+4]==param_token_ancl) &&
                    (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) &&
                    (param_cal_main[i+6]==param_token_ovsb || param_cal_main[i+6]==param_token_ancl) ){
                    x = i+2;
                    port_x = param_cal_main[x];
                    param_cal_main[x]  = param_cal_main[x+1];
                    param_cal_main[x+1]= port_x;
                    filter_cnt++;
                    TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                              x, x+1);
                }
                /* y_x_ovsb_ovsb_ovsb_ovsb -> y_ovsb_ovsb_x_ovsb_ovsb */
                else if ((param_cal_main[i]  !=param_token_ovsb && param_cal_main[i]  !=param_token_ancl) &&
                         (param_cal_main[i+1]!=param_token_ovsb && param_cal_main[i+1]!=param_token_ancl) &&
                         (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                         (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                         (param_cal_main[i+4]==param_token_ovsb || param_cal_main[i+4]==param_token_ancl) &&
                         (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) ){
                    sister_prox = PASS;
                    x = i+1;
                    y = ((x+2)+(param_space_sister-1))%param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+2];
                        param_cal_main[x+2]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x, x+2);
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_3x
@param:
 */
int
tdm_th_filter_ovsb_3x(tdm_mod_t *_tdm)
{
    int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb,
        param_space_sister,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth triple OVSB pattern: OVSB_OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        /* ovsb_ovsb_ovsb_x */
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
            /* y_ovsb_ovsb_ovsb_x_ovsb_z -> y_ovsb_ovsb_x_ovsb_ovsb_z */
            for (i=0; i<(param_cal_len-6); i++){
                if (param_cal_main[i]  !=param_token_ovsb &&
                    param_cal_main[i+1]==param_token_ovsb &&
                    param_cal_main[i+2]==param_token_ovsb &&
                    param_cal_main[i+3]==param_token_ovsb &&
                    param_cal_main[i+4]!=param_token_ovsb &&
                    /* param_cal_main[i+5]==param_token_ovsb && */
                    !(param_cal_main[i+5]>=param_phy_lo && param_cal_main[i+5]<=param_phy_hi ) &&
                    param_cal_main[i+6]!=param_token_ovsb ){
                    sister_prox = PASS;
                    x = i+4;
                    y = i;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x-1];
                        param_cal_main[x-1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-1, x);
                    }
                }
            }
            /* y_ovsb_x_ovsb_ovsb_ovsb_z -> y_ovsb_ovsb_x_ovsb_ovsb_z */
            for (i=0; i<(param_cal_len-6); i++){
                if (param_cal_main[i]  !=param_token_ovsb &&
                    /* param_cal_main[i+1]==param_token_ovsb && */
                    !(param_cal_main[i+1]>=param_phy_lo && param_cal_main[i+1]<=param_phy_hi ) &&
                    param_cal_main[i+2]!=param_token_ovsb &&
                    param_cal_main[i+3]==param_token_ovsb &&
                    param_cal_main[i+4]==param_token_ovsb &&
                    param_cal_main[i+5]==param_token_ovsb &&
                    param_cal_main[i+6]!=param_token_ovsb ){
                    sister_prox = PASS;
                    x = i+2;
                    y = i+6;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x+1, x);
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_2x
@param:
 */
int
tdm_th_filter_ovsb_2x(tdm_mod_t *_tdm)
{
    int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_space_sister,
        param_token_ovsb,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth dual OVSB pattern: OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        /* ovsb_ovsb_x_z */
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            os_slot_cnt<lr_slot_cnt){
            /* ovsb_ovsb_x_z -> ovsb_x_ovsb_z */
            for (i=0; i<(param_cal_len-3); i++){
                if (param_cal_main[i]  ==param_token_ovsb &&
                    param_cal_main[i+1]==param_token_ovsb &&
                    param_cal_main[i+2]!=param_token_ovsb &&
                    param_cal_main[i+3]!=param_token_ovsb){
                    sister_prox = PASS;
                    x = i+2;
                    y = (x>=param_space_sister)?
                        (x-param_space_sister):
                        (param_cal_len+x-param_space_sister);
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x-1];
                        param_cal_main[x-1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-1, x);
                    }
                }
            }
            /* z_x_ovsb_ovsb -> z_ovsb_x_ovsb */
            for (i=0; i<(param_cal_len-3); i++){
                if (param_cal_main[i]  !=param_token_ovsb &&
                    param_cal_main[i+1]!=param_token_ovsb &&
                    param_cal_main[i+2]==param_token_ovsb &&
                    param_cal_main[i+3]==param_token_ovsb){
                    sister_prox = PASS;
                    x = i+1;
                    y = (x+param_space_sister<param_cal_len)?
                        (x+param_space_sister):
                        (x+param_space_sister-param_cal_len);
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (param_space_sister>0 && 
                        port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        _tdm->_core_data.vars_pkg.port = port_x;
                        tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        _tdm->_core_data.vars_pkg.port = port_y;
                        tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x+1, x);
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_1x
@param:
 */
int
tdm_th_filter_ovsb_1x(tdm_mod_t *_tdm)
{
    int i, j, k, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox,
        dist_up, dist_dn, dist_range;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_space_sister,
        param_token_ovsb,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth single OVSB pattern: 2X unbalanced neighbouring slots \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        /* 2X unbalanced neighbouring slots */
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            2*os_slot_cnt<=lr_slot_cnt){
            for (i=1; i<param_cal_len; i++){
                if (param_cal_main[i]==param_token_ovsb){
                    dist_up = 0;
                    dist_dn = 0;
                    for (j=i; j>0; j--){
                        if (param_cal_main[j-1]!=param_token_ovsb){
                            dist_up++;
                        }
                        else{
                            break;
                        }
                    }
                    for (j=i+1; j<i+1+param_cal_len; j++){
                        k = j%param_cal_len;
                        if (param_cal_main[k]!=param_token_ovsb){
                            dist_dn++;
                        }
                        else {
                            break;
                        }
                    }
                    if (dist_up>=2*dist_dn){
                        dist_range = (dist_up - dist_dn)/2;
                        for (j=i; j>(i-dist_range) && j>2; j--){
                            sister_prox = PASS;
                            x = j-1;
                            y = ((x+param_space_sister)<param_cal_len)?
                                (x+param_space_sister):
                                (x+param_space_sister-param_cal_len);
                            port_x = param_cal_main[x];
                            port_y = param_cal_main[y];
                            if (param_space_sister>0 && 
                                port_x>=param_phy_lo && port_x<=param_phy_hi &&
                                port_y>=param_phy_lo && port_y<=param_phy_hi){
                                _tdm->_core_data.vars_pkg.port = port_x;
                                tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                                _tdm->_core_data.vars_pkg.port = port_y;
                                tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                                if (tsc_x==tsc_y){
                                    sister_prox = FAIL;
                                }
                            }
                            if (sister_prox==PASS){
                                param_cal_main[x]  = param_cal_main[x+1];
                                param_cal_main[x+1]= port_x;
                                filter_cnt++;
                                TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                          x+1, x);
                            }
                            else {
                                break;
                            }
                        }
                    }
                    else if (dist_dn>=2*dist_up){
                        dist_range = (dist_dn - dist_up)/2;
                        for (j=i; j<(i+dist_range) && j<(param_cal_len-1); j++){
                            sister_prox = PASS;
                            x = j+1;
                            y = (x>=param_space_sister)?
                                (x-param_space_sister):
                                (param_cal_len+x-param_space_sister);
                            port_x = param_cal_main[x];
                            port_y = param_cal_main[y];
                            if (param_space_sister>0 && 
                                port_x>=param_phy_lo && port_x<=param_phy_hi &&
                                port_y>=param_phy_lo && port_y<=param_phy_hi){
                                _tdm->_core_data.vars_pkg.port = port_x;
                                tsc_x = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                                _tdm->_core_data.vars_pkg.port = port_y;
                                tsc_y = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
                                if (tsc_x==tsc_y){
                                    sister_prox = FAIL;
                                }
                            }
                            if (sister_prox==PASS){
                                param_cal_main[x]  = param_cal_main[x-1];
                                param_cal_main[x-1]= port_x;
                                filter_cnt++;
                                TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                          x-1, x);
                            }
                            else {
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb
@param:
 */
int
tdm_th_filter_ovsb(tdm_mod_t *_tdm)
{
    int filter_cnt;
    
    /* Z_Y_OVSB_X_OVSB_OVSB */
    tdm_th_filter_ovsb_p1(_tdm);
    /* OVSB_OVSB_OVSB_OVSB_OVSB */
    tdm_th_filter_ovsb_5x(_tdm);
    /* OVSB_OVSB_OVSB_OVSB */
    tdm_th_filter_ovsb_4x(_tdm);
    /* OVSB_OVSB_OVSB */
    tdm_th_filter_ovsb_3x(_tdm);
    /* OVSB_OVSB */
    tdm_th_filter_ovsb_2x(_tdm);
    /* OVSB */
    filter_cnt = tdm_th_filter_ovsb_1x(_tdm);
    if (filter_cnt>0){
        tdm_th_filter_ovsb_1x(_tdm);
    }
    
    return PASS;
}


/**
@name: tdm_th_filter
@param:
 */
int
tdm_th_filter( tdm_mod_t *_tdm )
{
    int param_lr_en, param_os_en, param_cal_id;
    
    param_lr_en  = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en  = _tdm->_core_data.vars_pkg.os_enable;
    param_cal_id = _tdm->_core_data.vars_pkg.cal_id;
    
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Filters Applied to smooth TDM calendar\n\n");
    TDM_SML_BAR
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        switch(param_cal_id){
            case 0:
            case 4:
                TDM_PRINT0("TDM: <IDB PIPE 0> \n");
                _tdm->_core_data.vars_pkg.cal_id = 0;
                tdm_th_filter_ovsb(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 0> \n");
                _tdm->_core_data.vars_pkg.cal_id = 4;
                tdm_th_filter_ovsb(_tdm);
                break;
            case 1:
            case 5:
                TDM_PRINT0("TDM: <IDB PIPE 1> \n");
                _tdm->_core_data.vars_pkg.cal_id = 1;
                tdm_th_filter_ovsb(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 1> \n");
                _tdm->_core_data.vars_pkg.cal_id = 5;
                tdm_th_filter_ovsb(_tdm);
                break;
            case 2:
            case 6:
                TDM_PRINT0("TDM: <IDB PIPE 2> \n");
                _tdm->_core_data.vars_pkg.cal_id = 2;
                tdm_th_filter_ovsb(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 2> \n");
                _tdm->_core_data.vars_pkg.cal_id = 6;
                tdm_th_filter_ovsb(_tdm);
                break;
            case 3:
            case 7:
                TDM_PRINT0("TDM: <IDB PIPE 3> \n");
                _tdm->_core_data.vars_pkg.cal_id = 3;
                tdm_th_filter_ovsb(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 3> \n");
                _tdm->_core_data.vars_pkg.cal_id = 7;
                tdm_th_filter_ovsb(_tdm);
                break;
            default:
                break;
        }
        _tdm->_core_data.vars_pkg.cal_id = param_cal_id;
    }
    
    return (_tdm->_chip_exec[TDM_CHIP_EXEC__PARSE](_tdm));
}


/**
@name: tdm_th_filter_lr_jitter
@param:
 */
int
tdm_th_filter_lr_jitter(tdm_mod_t *_tdm, int min_speed)
{
    int i, j, k, filter_cnt=0, x, y, chk_x, chk_y, spd_x, spd_y;
    int m, port, port_speed, idx_curr,
        space_min, space_max,
        dist_up, dist_dn, dist_mv;
    int port_bmp[TH_NUM_EXT_PORTS];
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_phy_lo, param_phy_hi,
        param_lr_en;
    int *param_cal_main;
    enum port_speed_e *param_speeds;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;

    for (i=0; i<TH_NUM_EXT_PORTS; i++) {
        port_bmp[i] = 0;
    } 
    
    TDM_PRINT0("Smooth LR port jitter\n\n");
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id, param_cal_main);
    if (param_lr_en==BOOL_TRUE && min_speed>=SPEED_10G) {
        for (i=0; i<param_cal_len; i++) {
            port = param_cal_main[i];
            if (port >= param_phy_lo && port <= param_phy_hi &&
                port < TH_NUM_EXT_PORTS) {
                port_speed = param_speeds[port];
                if (port_speed<min_speed || port_bmp[port]==1) {
                    continue;
                }
                port_bmp[port] = 1;
                tdm_th_filter_calc_jitter(port_speed, param_cal_len,
                                          &space_min, &space_max);
                for (j=0; j<param_cal_len; j++) {
                    k = (i+param_cal_len-j)%param_cal_len;
                    if (param_cal_main[k]==port) {
                        idx_curr = k;
                        dist_up = tdm_th_filter_get_same_port_dist(idx_curr,
                                    TDM_DIR_UP, param_cal_main, param_cal_len);
                        dist_dn = tdm_th_filter_get_same_port_dist(idx_curr,
                                    TDM_DIR_DN, param_cal_main, param_cal_len);
                        /* filter port if space violation is detected: up */
                        if (dist_up>space_max || dist_dn<space_min) {
                            TDM_PRINT7("%s, port %d, slot %d, dist_up %d > %d, dist_dn %d < %d\n",
                                      "TDM: [Linerate jitter 1]",
                                      port, idx_curr,
                                      dist_up, space_max, dist_dn, space_min);
                            dist_mv = (dist_up-dist_dn)/2;
                            for (m=1; m<=dist_mv; m++) {
                                /* check spacing for neighbor and target port */
                                x = (idx_curr + param_cal_len - m) % param_cal_len;
                                y = (x + 1) % param_cal_len;
                                chk_x = tdm_th_filter_chk_slot_shift_cond(x, TDM_DIR_DN,
                                            param_cal_len, param_cal_main, _tdm);
                                chk_y = tdm_th_filter_chk_slot_shift_cond(y, TDM_DIR_UP,
                                            param_cal_len, param_cal_main, _tdm);
                                spd_x = tdm_th_filter_get_port_speed(param_cal_main[x], _tdm);
                                spd_y = tdm_th_filter_get_port_speed(param_cal_main[y], _tdm);
                                /* shift port */
                                if (chk_x == PASS && chk_y == PASS &&
                                    spd_x < SPEED_100G && spd_y < SPEED_100G) {
                                    param_cal_main[y] = param_cal_main[x];
                                    param_cal_main[x] = port;
                                    filter_cnt++;
                                    TDM_PRINT3("\t Shift port %d UP from slot %d to slot %d\n",
                                              port, y, x);
                                } else {
                                    break;
                                }
                            }
                        }
                        /* filter port if space violation is detected: down */
                        else if (dist_up<space_min || dist_dn>space_max) {
                            TDM_PRINT7("%s port %d, slot %d, dist_up %d < %d, dist_dn %d > %d\n",
                                      "TDM: [Linerate jitter 2]",
                                      port, idx_curr,
                                      dist_up, space_min, dist_dn, space_max);
                            dist_mv = (dist_dn-dist_up)/2;
                            for (m=1; m<=dist_mv; m++) {
                                /* check spacing for neighbor and target port */
                                x = (idx_curr + m) % param_cal_len;
                                y = (x + param_cal_len - 1) % param_cal_len;
                                chk_x = tdm_th_filter_chk_slot_shift_cond(x, TDM_DIR_UP,
                                            param_cal_len, param_cal_main, _tdm);
                                chk_y = tdm_th_filter_chk_slot_shift_cond(y, TDM_DIR_DN,
                                            param_cal_len, param_cal_main, _tdm);
                                spd_x = tdm_th_filter_get_port_speed(param_cal_main[x], _tdm);
                                spd_y = tdm_th_filter_get_port_speed(param_cal_main[y], _tdm);
                                /* shift port */
                                if (chk_x == PASS && chk_y == PASS &&
                                    spd_x < SPEED_100G && spd_y < SPEED_100G) {
                                    param_cal_main[y] = param_cal_main[x];
                                    param_cal_main[x] = port;
                                    filter_cnt++;
                                    TDM_PRINT3("\t Shift port %d DOWN from slot %d to slot %d\n",
                                              port, y, x);
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    TDM_SML_BAR

    return (filter_cnt);
}


/**
@name: tdm_th_filter_lr
@param:
 */
int
tdm_th_filter_lr(tdm_mod_t *_tdm)
{
    if (tdm_th_filter_lr_jitter(_tdm, SPEED_10G) == 0){
        return PASS;
    }
    if (tdm_th_filter_lr_jitter(_tdm, SPEED_20G) == 0) {
        return PASS;
    }
    if (tdm_th_filter_lr_jitter(_tdm, SPEED_40G) == 0) {
        return PASS;
    }
    return PASS;
}


/**
@name: tdm_th_filter_ovsb_4x_mix
@param:
 */
int
tdm_th_filter_ovsb_4x_mix(tdm_mod_t *_tdm)
{
    int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, spd_x, spd_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb, param_token_ancl,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_token_ancl  = _tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth quadrant OVSB pattern: OVSB_OVSB_OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
            for (i=0; i<(param_cal_len-6); i++){
                /* ovsb_ovsb_ovsb_ovsb_x_ovsb_y -> ovsb_ovsb_ovsb_ovsb_x_ovsb_ovsb_y */
                if ((param_cal_main[i]  ==param_token_ovsb || param_cal_main[i]  ==param_token_ancl) &&
                    (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                    (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                    (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                    (param_cal_main[i+4]!=param_token_ovsb && param_cal_main[i+4]!=param_token_ancl) &&
                    (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) &&
                    (param_cal_main[i+6]!=param_token_ovsb && param_cal_main[i+6]!=param_token_ancl) ){
                    x = i + 4;
                    port_x = param_cal_main[x];
                    param_cal_main[x]  = param_cal_main[x-1];
                    param_cal_main[x-1]= port_x;
                    filter_cnt++;
                    TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                              x-1, x);
                }
                /* ovsb_ovsb_ovsb_ovsb_x_y -> ovsb_ovsb_x_ovsb_ovsb_y */
                else if ((param_cal_main[i]  ==param_token_ovsb || param_cal_main[i]  ==param_token_ancl) &&
                         (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                         (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                         (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                         (param_cal_main[i+4]!=param_token_ovsb && param_cal_main[i+4]!=param_token_ancl) &&
                         (param_cal_main[i+5]!=param_token_ovsb && param_cal_main[i+5]!=param_token_ancl) ){
                    sister_prox = PASS;
                    x      = i + 4;
                    y      = (i + param_cal_len - 1) % param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi) {
                        spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                        spd_y  = tdm_th_filter_get_port_speed(port_y, _tdm);
                        tsc_y  = tdm_th_filter_get_port_pm(port_y, _tdm);
                        tsc_x  = tdm_th_filter_get_port_pm(port_x, _tdm);
                        if (tsc_x == tsc_y ||
                            spd_x >= SPEED_100G || spd_y >= SPEED_100G) {
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x-2];
                        param_cal_main[x-2]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-2, x);
                    }
                }
                /* y_ovsb_x_ovsb_ovsb_ovsb_ovsb -> y_ovsb_ovsb_x_ovsb_ovsb_ovsb */
                else if ((param_cal_main[i]  !=param_token_ovsb && param_cal_main[i]  !=param_token_ancl) &&
                         (param_cal_main[i+1]==param_token_ovsb || param_cal_main[i+1]==param_token_ancl) &&
                         (param_cal_main[i+2]!=param_token_ovsb && param_cal_main[i+2]!=param_token_ancl) &&
                         (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                         (param_cal_main[i+4]==param_token_ovsb || param_cal_main[i+4]==param_token_ancl) &&
                         (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) &&
                         (param_cal_main[i+6]==param_token_ovsb || param_cal_main[i+6]==param_token_ancl) ){
                    x = i + 2;
                    port_x = param_cal_main[x];
                    param_cal_main[x]  = param_cal_main[x+1];
                    param_cal_main[x+1]= port_x;
                    filter_cnt++;
                    TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                              x, x+1);
                }
                /* y_x_ovsb_ovsb_ovsb_ovsb -> y_ovsb_ovsb_x_ovsb_ovsb */
                else if ((param_cal_main[i]  !=param_token_ovsb && param_cal_main[i]  !=param_token_ancl) &&
                         (param_cal_main[i+1]!=param_token_ovsb && param_cal_main[i+1]!=param_token_ancl) &&
                         (param_cal_main[i+2]==param_token_ovsb || param_cal_main[i+2]==param_token_ancl) &&
                         (param_cal_main[i+3]==param_token_ovsb || param_cal_main[i+3]==param_token_ancl) &&
                         (param_cal_main[i+4]==param_token_ovsb || param_cal_main[i+4]==param_token_ancl) &&
                         (param_cal_main[i+5]==param_token_ovsb || param_cal_main[i+5]==param_token_ancl) ){
                    sister_prox = PASS;
                    x      = i + 1;
                    y      = (i + 6) % param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi) {
                        spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                        spd_y  = tdm_th_filter_get_port_speed(port_y, _tdm);
                        tsc_y  = tdm_th_filter_get_port_pm(port_y, _tdm);
                        tsc_x  = tdm_th_filter_get_port_pm(port_x, _tdm);
                        if (tsc_x == tsc_y ||
                            spd_x >= SPEED_100G || spd_y >= SPEED_100G) {
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+2];
                        param_cal_main[x+2]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x, x+2);
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_3x_mix
@param:
 */
int
tdm_th_filter_ovsb_3x_mix(tdm_mod_t *_tdm)
{
    int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, spd_x, spd_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_token_ovsb,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth triple OVSB pattern: OVSB_OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i=0; i<param_cal_len; i++){
            if (param_cal_main[i]==param_token_ovsb){
                os_slot_cnt++;
            }
            else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt>param_ancl_num)?(lr_slot_cnt-param_ancl_num):(0);
        /* ovsb_ovsb_ovsb_x */
        if (os_slot_cnt>0 && lr_slot_cnt>0 &&
            os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
            for (i=0; i<(param_cal_len-6); i++){
                /* y_ovsb_ovsb_ovsb_x_ovsb_z -> y_ovsb_ovsb_x_ovsb_ovsb_z */
                if (param_cal_main[i]  !=param_token_ovsb &&
                    param_cal_main[i+1]==param_token_ovsb &&
                    param_cal_main[i+2]==param_token_ovsb &&
                    param_cal_main[i+3]==param_token_ovsb &&
                    param_cal_main[i+4]!=param_token_ovsb &&
                    !(param_cal_main[i+5]>=param_phy_lo && param_cal_main[i+5]<=param_phy_hi ) &&
                    param_cal_main[i+6]!=param_token_ovsb ){
                    sister_prox = PASS;
                    x = i+4;
                    y = i;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    spd_y  = tdm_th_filter_get_port_speed(port_y, _tdm);
                    if (spd_x >= SPEED_100G || spd_y >= SPEED_100G) {
                        continue;
                    }
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x-1];
                        param_cal_main[x-1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-1, x);
                    }
                }
                /* y_ovsb_x_ovsb_ovsb_ovsb_z -> y_ovsb_ovsb_x_ovsb_ovsb_z */
                else if (param_cal_main[i]  !=param_token_ovsb &&
                         /* param_cal_main[i+1]==param_token_ovsb && */
                         !(param_cal_main[i+1]>=param_phy_lo && param_cal_main[i+1]<=param_phy_hi ) &&
                         param_cal_main[i+2]!=param_token_ovsb &&
                         param_cal_main[i+3]==param_token_ovsb &&
                         param_cal_main[i+4]==param_token_ovsb &&
                         param_cal_main[i+5]==param_token_ovsb &&
                         param_cal_main[i+6]!=param_token_ovsb) {
                    sister_prox = PASS;
                    x = i+2;
                    y = i+6;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    spd_y  = tdm_th_filter_get_port_speed(port_y, _tdm);
                    if (spd_x >= SPEED_100G || spd_y >= SPEED_100G) {
                        continue;
                    }
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x+1, x);
                    }
                }
                /* y_x_ovsb_ovsb_ovsb_z -> y_ovsb_x_ovsb_ovsb_z */
                else if (param_cal_main[i]  !=param_token_ovsb &&
                         param_cal_main[i+1]!=param_token_ovsb &&
                         param_cal_main[i+2]==param_token_ovsb &&
                         param_cal_main[i+3]==param_token_ovsb &&
                         param_cal_main[i+4]==param_token_ovsb &&
                         param_cal_main[i+5]!=param_token_ovsb) {
                    sister_prox = PASS;
                    x = i+1;
                    y = i+5;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    spd_y  = tdm_th_filter_get_port_speed(port_y, _tdm);
                    if (spd_x >= SPEED_100G || spd_y >= SPEED_100G) {
                        continue;
                    }
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x+1, x);
                    }
                }
                /* y_ovsb_ovsb_ovsb_x_z -> y_ovsb_ovsb_x_ovsb_z */
                else if (param_cal_main[i]  !=param_token_ovsb &&
                         param_cal_main[i+1]==param_token_ovsb &&
                         param_cal_main[i+2]==param_token_ovsb &&
                         param_cal_main[i+3]==param_token_ovsb &&
                         param_cal_main[i+4]!=param_token_ovsb &&
                         param_cal_main[i+5]!=param_token_ovsb) {
                    sister_prox = PASS;
                    x = i+4;
                    y = i;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    spd_y  = tdm_th_filter_get_port_speed(port_y, _tdm);
                    if (spd_x >= SPEED_100G || spd_y >= SPEED_100G) {
                        continue;
                    }
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi){
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x==tsc_y){
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox==PASS){
                        param_cal_main[x]  = param_cal_main[x-1];
                        param_cal_main[x-1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-1, x);
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb_2x_mix
@param:
 */
int
tdm_th_filter_ovsb_2x_mix(tdm_mod_t *_tdm)
{
    int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, spd_x, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_space_sister,
        param_token_ovsb,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_token_ovsb  = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    
    TDM_PRINT0("Smooth dual OVSB pattern: OVSB_OVSB \n\n");
    
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    if (param_os_en == BOOL_TRUE && param_lr_en == BOOL_TRUE) {
        lr_slot_cnt = 0;
        os_slot_cnt = 0;
        for (i = 0; i < param_cal_len; i++){
            if (param_cal_main[i] == param_token_ovsb) {
                os_slot_cnt++;
            } else {
                lr_slot_cnt++;
            }
        }
        lr_slot_cnt = (lr_slot_cnt > param_ancl_num) ?
                      (lr_slot_cnt - param_ancl_num) : 0;
        /* x_ovsb_ovsb_y */
        if (os_slot_cnt > 0 && lr_slot_cnt > 0) {
            /* 4-slot pattern */
            for (i = 0; i < (param_cal_len - 3); i++) {
                /* ovsb_ovsb_x_z -> ovsb_x_ovsb_z */
                if (param_cal_main[i]   == param_token_ovsb &&
                    param_cal_main[i+1] == param_token_ovsb &&
                    param_cal_main[i+2] != param_token_ovsb &&
                    param_cal_main[i+3] != param_token_ovsb){
                    sister_prox = PASS;
                    x = i + 2;
                    y = (x + param_cal_len - param_space_sister) % param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    if (spd_x >= SPEED_100G) {
                        continue;
                    }
                    if (port_x >= param_phy_lo && port_x <= param_phy_hi &&
                        port_y >= param_phy_lo && port_y <= param_phy_hi){
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x == tsc_y) {
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox == PASS) {
                        param_cal_main[x]  = param_cal_main[x-1];
                        param_cal_main[x-1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-1, x);
                    }
                }
                /* z_x_ovsb_ovsb -> z_ovsb_x_ovsb */
                else if (param_cal_main[i]   != param_token_ovsb &&
                         param_cal_main[i+1] != param_token_ovsb &&
                         param_cal_main[i+2] == param_token_ovsb &&
                         param_cal_main[i+3] == param_token_ovsb){
                    sister_prox = PASS;
                    x = i + 1;
                    y = (x + param_space_sister) % param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    if (spd_x >= SPEED_100G) {
                        continue;
                    }
                    if (port_x >= param_phy_lo && port_x <= param_phy_hi &&
                        port_y >= param_phy_lo && port_y <= param_phy_hi) {
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x == tsc_y) {
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox == PASS) {
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        filter_cnt++;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x+1, x);
                    }
                }
            }
            /* 6-slot pattern */
            for (i = 0; i < (param_cal_len - 5); i++) {
                /* z_y_ovsb_x_ovsb_ovsb -> z_ovsb_y_ovsb_x_ovsb */
                if (param_cal_main[i]   != param_token_ovsb &&
                    param_cal_main[i+1] != param_token_ovsb &&
                    param_cal_main[i+2] == param_token_ovsb &&
                    param_cal_main[i+3] != param_token_ovsb &&
                    param_cal_main[i+4] == param_token_ovsb &&
                    param_cal_main[i+5] == param_token_ovsb) {
                    sister_prox = PASS;
                    x = i + 3;
                    y = (x + param_space_sister) % param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    if (spd_x >= SPEED_100G) {
                        continue;
                    }
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi) {
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x==tsc_y) {
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox == PASS) {
                        x = i + 3;
                        y = i + 1;
                        port_x = param_cal_main[x];
                        port_y = param_cal_main[y];
                        param_cal_main[x]  = param_cal_main[x+1];
                        param_cal_main[x+1]= port_x;
                        param_cal_main[y]  = param_cal_main[y+1];
                        param_cal_main[y+1]= port_y;
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  y+1, y);
                        TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                  x+1, x);
                    }
                }
                /* ovsb_ovsb_x_ovsb_y_z -> ovsb_x_ovsb_y_ovsb_z */
                else if (param_cal_main[i]   == param_token_ovsb &&
                         param_cal_main[i+1] == param_token_ovsb &&
                         param_cal_main[i+2] != param_token_ovsb &&
                         param_cal_main[i+3] == param_token_ovsb &&
                         param_cal_main[i+4] != param_token_ovsb &&
                         param_cal_main[i+5] != param_token_ovsb) {
                    sister_prox = PASS;
                    x = i + 2;
                    y = (x + param_cal_len - param_space_sister) % param_cal_len;
                    port_x = param_cal_main[x];
                    port_y = param_cal_main[y];
                    spd_x  = tdm_th_filter_get_port_speed(port_x, _tdm);
                    if (spd_x >= SPEED_100G) {
                        continue;
                    }
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi &&
                        port_y>=param_phy_lo && port_y<=param_phy_hi) {
                        tsc_x = tdm_th_filter_get_port_pm(port_x, _tdm);
                        tsc_y = tdm_th_filter_get_port_pm(port_y, _tdm);
                        if (tsc_x==tsc_y) {
                            sister_prox = FAIL;
                        }
                    }
                    if (sister_prox == PASS) {
                        x = i + 2;
                        y = i + 4;
                        port_x = param_cal_main[x];
                        port_y = param_cal_main[y];
                        param_cal_main[x]  = param_cal_main[x-1];
                        param_cal_main[x-1]= port_x;
                        param_cal_main[y]  = param_cal_main[y-1];
                        param_cal_main[y-1]= port_y;
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  x-1, x);
                        TDM_PRINT2("Shift OVSB slot DOWN from %03d to %03d\n",
                                  y-1, y);
                    }
                }
            }
        }
        if (filter_cnt>0){
            TDM_PRINT1("\nFilter done: --- filter applied <%d> times\n", filter_cnt);
        }
    }
    TDM_SML_BAR
    
    return filter_cnt;
}


/**
@name: tdm_th_filter_ovsb
@param:
 */
int
tdm_th_filter_ovsb_mix(tdm_mod_t *_tdm)
{
    tdm_th_filter_ovsb_4x_mix(_tdm);
    tdm_th_filter_ovsb_3x_mix(_tdm);
    tdm_th_filter_ovsb_2x_mix(_tdm);
    
    return PASS;
}


/**
@name: tdm_th_filter
@param:
 */
int
tdm_th_filter_mix( tdm_mod_t *_tdm )
{
    int param_lr_en, param_os_en, param_cal_id;
    
    param_lr_en  = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en  = _tdm->_core_data.vars_pkg.os_enable;
    param_cal_id = _tdm->_core_data.vars_pkg.cal_id;
    
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Filters Applied to smooth TDM calendar\n\n");
    TDM_SML_BAR
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        switch(param_cal_id){
            case 0:
            case 4:
                TDM_PRINT0("TDM: <IDB PIPE 0> \n");
                _tdm->_core_data.vars_pkg.cal_id = 0;
                tdm_th_filter_ovsb_mix(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 0> \n");
                _tdm->_core_data.vars_pkg.cal_id = 4;
                tdm_th_filter_ovsb_mix(_tdm);
                break;
            case 1:
            case 5:
                TDM_PRINT0("TDM: <IDB PIPE 1> \n");
                _tdm->_core_data.vars_pkg.cal_id = 1;
                tdm_th_filter_ovsb_mix(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 1> \n");
                _tdm->_core_data.vars_pkg.cal_id = 5;
                tdm_th_filter_ovsb_mix(_tdm);
                break;
            case 2:
            case 6:
                TDM_PRINT0("TDM: <IDB PIPE 2> \n");
                _tdm->_core_data.vars_pkg.cal_id = 2;
                tdm_th_filter_ovsb_mix(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 2> \n");
                _tdm->_core_data.vars_pkg.cal_id = 6;
                tdm_th_filter_ovsb_mix(_tdm);
                break;
            case 3:
            case 7:
                TDM_PRINT0("TDM: <IDB PIPE 3> \n");
                _tdm->_core_data.vars_pkg.cal_id = 3;
                tdm_th_filter_ovsb_mix(_tdm);
                TDM_PRINT0("TDM: <MMU PIPE 3> \n");
                _tdm->_core_data.vars_pkg.cal_id = 7;
                tdm_th_filter_ovsb_mix(_tdm);
                break;
            default:
                break;
        }
        _tdm->_core_data.vars_pkg.cal_id = param_cal_id;
    }
    if (param_lr_en==BOOL_TRUE) {
        tdm_th_filter_lr(_tdm);
    }
    
    return (_tdm->_chip_exec[TDM_CHIP_EXEC__PARSE](_tdm));
}
