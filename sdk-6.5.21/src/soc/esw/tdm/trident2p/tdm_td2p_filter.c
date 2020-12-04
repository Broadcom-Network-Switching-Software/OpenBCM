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
@name: tdm_td2p_filter_dither
@param:
 */
int
tdm_td2p_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed)
{
	int g, i, j, k=1, l=TD2P_NUM_EXT_PORTS, slice_idx, shift_cnt=0;
	unsigned short dither_shift_done=BOOL_FALSE, dither_done=BOOL_FALSE, dither_slice_counter=0;
	
	/* Get index of the OVSB slot with the largest clump size */
	dither_shift_done=BOOL_FALSE;
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==TD2P_OVSB_TOKEN && tdm_td2p_scan_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories), &slice_idx)==tdm_td2p_slice_size(TD2P_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))) {
			g=i; while( (tdm_tbl[g]==TD2P_OVSB_TOKEN) && (g<(lr_idx_limit+accessories)) ) {g++;}
			if ( tdm_td2p_slice_prox_up(g,tdm_tbl,tsc,speed) &&
				 tdm_td2p_slice_prox_dn((tdm_td2p_slice_idx(TD2P_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))-1),tdm_tbl,(lr_idx_limit+accessories),tsc,speed) ) {
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
			while (tdm_tbl[i]!=TD2P_OVSB_TOKEN && tdm_td2p_scan_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories),&slice_idx)==tdm_td2p_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && i<(lr_idx_limit+accessories) ) {
				if (++dither_slice_counter>=threshold && tdm_tbl[i-1]==TD2P_OVSB_TOKEN) {
					for (j=255; j>i; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[i+1]=TD2P_OVSB_TOKEN;
					dither_done=BOOL_TRUE;
					break;
				}
				i++; if (tdm_tbl[i]==TD2P_OVSB_TOKEN) {i++;}
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
@name: tdm_td2p_filter_fine_dither
@param:
 */
int
tdm_td2p_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc)
{
	int i, j, k, min_prox=11,slice_idx;
	unsigned short fine_dither_done;

	i=port; fine_dither_done=BOOL_FALSE;
	for (j=2; j<(lr_idx_limit+accessories-4); j++) {
		if ( tdm_tbl[j]!=TD2P_OVSB_TOKEN && tdm_tbl[j-2]!=TD2P_OVSB_TOKEN && tdm_tbl[j+2]!=TD2P_OVSB_TOKEN && tdm_tbl[j+4]!=TD2P_OVSB_TOKEN && 
			 tdm_td2p_scan_slice_size_local((j-2), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_td2p_scan_slice_size_local( j,    tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_td2p_scan_slice_size_local((j+2), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_td2p_scan_slice_size_local((j+4), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 &&
			 tdm_td2p_slice_prox_local( j,    tdm_tbl, (lr_idx_limit+accessories), tsc)>min_prox )
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
		tdm_tbl[i]=TD2P_OVSB_TOKEN;
		
		return PASS;
	}
	else {
		return FAIL;
	}
}


/**
@name: tdm_td2p_filter_shift_lr_port
@param:

Shifts all slots of the given linerate port up or down,
and returns the total number of shifting occurence.
 */
int
tdm_td2p_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir)
{
	int i, shift_cnt=0, port_swap;
	
	TD2P_TOKEN_CHECK(port) {
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
@name: tdm_td2p_filter_migrate_os_slot
@param:

Migrate OVSB slot from src to dst in an array.
 */
int
tdm_td2p_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i=idx_src, j, token_tmp, filter_result=FAIL, check_pass=BOOL_TRUE,
	    idx0, idx1, tsc0, tsc1;
	
	if ( !(idx_src>=0 && idx_src<tdm_tbl_len) ||  !(idx_dst>=0 && idx_dst<tdm_tbl_len) ){
		check_pass = BOOL_FALSE;
	}
	if ( tdm_tbl[idx_src]!=TD2P_OVSB_TOKEN && tdm_tbl[idx_src]!=TD2P_ANCL_TOKEN) {
		check_pass = BOOL_FALSE;
	}
	/* Check sister port spacing */
	if (check_pass==BOOL_TRUE){
		i = idx_src;
		for (j=1; j<VBS_MIN_SPACING; j++){
			idx0 = ((i+j)<tdm_tbl_len)? (i+j): (i+j-tdm_tbl_len);
			idx1 = ((idx0-VBS_MIN_SPACING)>=0) ? (idx0-VBS_MIN_SPACING) : (idx0-VBS_MIN_SPACING+tdm_tbl_len);
			TD2P_TOKEN_CHECK(tdm_tbl[idx0]){
				TD2P_TOKEN_CHECK(tdm_tbl[idx1]){
					tsc0 = tdm_td2p_legacy_which_tsc(tdm_tbl[idx0],tsc);
					tsc1 = tdm_td2p_legacy_which_tsc(tdm_tbl[idx1],tsc);
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
			TD2P_TOKEN_CHECK(tdm_tbl[idx0]){
				TD2P_TOKEN_CHECK(tdm_tbl[idx1]){
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
@name: tdm_td2p_filter_smooth_idle_slice
@param:

Smooth MMU TDM calendar by migrating IDLE slots
 */
int
tdm_td2p_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, k, idle1_token, idle2_token, pos_cnt, pos_step, pos,
	    idle_slot_cnt=0, idle_slice_max, idle_slice_max_idx, lr_slice_max, lr_slice_max_idx, 
		idx_x, idx_y, idx1, idx2, tsc_x, tsc_y, tsc1, tsc2, slot_token, slot_dst_idx,
		filter_done, filter_cnt=0;
	
	idle1_token = TD2P_IDL1_TOKEN;
	idle2_token = TD2P_IDL2_TOKEN;
	
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
			idle_slice_max = tdm_td2p_scan_slice_max(idle1_token, tdm_tbl,tdm_tbl_len, &idle_slice_max_idx,0);
			lr_slice_max   = tdm_td2p_scan_mix_slice_max(1,tdm_tbl,tdm_tbl_len, &lr_slice_max_idx,pos);
			
			/* Find idle clump with minimum size 2: ..._y_x_IDLE_IDLE_... */
			if ( (idle_slice_max>1 && lr_slice_max>0) &&
			     (idle_slice_max_idx<tdm_tbl_len && idle_slice_max_idx>1) &&
			     ((lr_slice_max_idx<tdm_tbl_len) && ((lr_slice_max_idx>0) || (lr_slice_max_idx==0 && lr_slice_max>1))) ){
				/* Check sister port spacing */
				idx_x = idle_slice_max_idx-1;
				idx_y = idle_slice_max_idx-2;
				idx1  = ((idle_slice_max_idx+2)<tdm_tbl_len) ? (idle_slice_max_idx+2) : ((idle_slice_max_idx+2)-tdm_tbl_len);
				idx2  = ((idle_slice_max_idx+3)<tdm_tbl_len) ? (idle_slice_max_idx+3) : ((idle_slice_max_idx+3)-tdm_tbl_len);
				tsc_x = tdm_td2p_legacy_which_tsc(tdm_tbl[idx_x],tsc);
				tsc_y = tdm_td2p_legacy_which_tsc(tdm_tbl[idx_y],tsc);
				tsc1  = tdm_td2p_legacy_which_tsc(tdm_tbl[idx1],tsc);
				tsc2  = tdm_td2p_legacy_which_tsc(tdm_tbl[idx2],tsc);
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
@name: tdm_td2p_filter_smooth_os_slice
@param:

Smooth MMU TDM calendar by shifting linerate ports Downward/Upward
		Downward: x_ovsb___x_ovsb___x_ovsb -> ovsb_x___ovsb_x___ovsb_x
		Upward  : ovsb_x___ovsb_x___ovsb_x -> x_ovsb___x_ovsb___x_ovsb
 */
int
tdm_td2p_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir)
{
	int i, slice_idx, shift_dir, shift_cnt=0, lr_clump_max, lr_clump_min, filter_port=0;
	const char *str_dir;
	
	str_dir      = (dir==UP) ? "UPWARD": "DOWNWARD";
	shift_dir    = (dir==UP) ? (UP) : (DN);
	
	for (i=1; i<tdm_tbl_len; i++) {
		filter_port=tdm_tbl[i];
		if (filter_port==tdm_tbl[0]) {continue;}
		
		lr_clump_max = tdm_td2p_scan_mix_slice_max(1,tdm_tbl,tdm_tbl_len, &slice_idx, 0);
		lr_clump_min = tdm_td2p_scan_mix_slice_min(1,tdm_tbl,tdm_tbl_len, &slice_idx, 0);
		if ( (lr_clump_max<=1) || 
		     (lr_clump_max==2 && lr_clump_min==1)){
			break;
		}
		if ( FAIL==tdm_td2p_check_shift_cond_pattern(filter_port, tdm_tbl, tdm_tbl_len, tsc, dir) ) {
			continue;
		}
		else if ( FAIL==tdm_td2p_check_shift_cond_local_slice(filter_port, tdm_tbl, tdm_tbl_len, tsc, dir) ) {
			continue;
		}
		else {
			tdm_td2p_filter_shift_lr_port(filter_port, tdm_tbl, tdm_tbl_len, shift_dir);
			TDM_PRINT3("Filter applied: Vector shift %8s, port %3d, beginning index #%03d \n", str_dir, filter_port, i);
			shift_cnt++;
		}
	}
	
	return shift_cnt;
}


/**
@name: tdm_td2p_filter_smooth_os_slice_fine
@param:

Smooth MMU TDM calendar by migrating OVSB slot
 */
int
tdm_td2p_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int j, k, ovsb_token, pos, pos_step, pos_cnt, pos_cnt_max,
	    slice_max, slice_min, slice_max_idx, slice_min_idx, pp_prox,
		idx0, idx1, tsc0, tsc1, slot_tmp, idx_dst, idx_src, filter_cnt=0;
	
	ovsb_token  = TD2P_OVSB_TOKEN;
	pos         = 0;
	pos_cnt     = 0;
	pos_cnt_max = 16;
	pos_step    = tdm_tbl_len/pos_cnt_max + 1;
	
	for (k=0; k<tdm_tbl_len; k++){
		pos      = pos_cnt*pos_step;
		pos_cnt  = ((pos_cnt+1)<pos_cnt_max)? (pos_cnt+1): (0);
		slice_max= tdm_td2p_scan_slice_max(ovsb_token,tdm_tbl,tdm_tbl_len, &slice_max_idx,k);
		slice_min= tdm_td2p_scan_slice_min(ovsb_token,tdm_tbl,tdm_tbl_len, &slice_min_idx,pos);
		
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
		TD2P_TOKEN_CHECK(tdm_tbl[idx0]){
			TD2P_TOKEN_CHECK(tdm_tbl[idx1]){
				tsc0 = tdm_td2p_legacy_which_tsc(tdm_tbl[idx0],tsc);
				tsc1 = tdm_td2p_legacy_which_tsc(tdm_tbl[idx1],tsc);
				if ( tsc0==tsc1 ){
					pp_prox=FAIL;
				}
			}
		}
		/* Check same port spacing */
		for (j=1; j<LLS_MIN_SPACING; j++){
			idx0 = ((idx_src-j)>=0)? (idx_src-j): (idx_src-j+tdm_tbl_len);
			idx1 = ((idx0+LLS_MIN_SPACING)<tdm_tbl_len)? (idx0+LLS_MIN_SPACING): (idx0+LLS_MIN_SPACING-tdm_tbl_len);
			TD2P_TOKEN_CHECK(tdm_tbl[idx0]){
				TD2P_TOKEN_CHECK(tdm_tbl[idx1]){
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
@name: tdm_td2p_filter_smooth_os_os_up
@param:

Smooth MMU TDM calendar by transforming:
        --- _x_y_ovsb_ovsb_ -> _x_ovsb_y_ovsb_
 */
int tdm_td2p_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, slot_tmp, filter_cnt=0;
	
	for (i=2; i<(tdm_tbl_len-1); i++) {
		if (tdm_tbl[i  ]==TD2P_OVSB_TOKEN &&
			tdm_tbl[i+1]==TD2P_OVSB_TOKEN ){
			TD2P_TOKEN_CHECK(tdm_tbl[i-1]){
				TD2P_TOKEN_CHECK(tdm_tbl[i-2]){
					if ( PASS==tdm_td2p_check_slot_swap_cond((i-1), tdm_tbl, tdm_tbl_len, tsc, speed) ){
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
@name: tdm_td2p_filter_smooth_os_os_dn
@param:

Smooth MMU TDM calendar by transforming:
		--- _ovsb_ovsb_x_y_ -> _ovsb_x_ovsb_y_
 */
int tdm_td2p_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, slot_tmp, filter_cnt=0;
	
	for (i=1; i<(tdm_tbl_len-3); i++) {
		if (tdm_tbl[i  ]==TD2P_OVSB_TOKEN &&
			tdm_tbl[i+1]==TD2P_OVSB_TOKEN ){
			TD2P_TOKEN_CHECK(tdm_tbl[i+2]){
				TD2P_TOKEN_CHECK(tdm_tbl[i+3]){
					if ( PASS==tdm_td2p_check_slot_swap_cond((i+1), tdm_tbl, tdm_tbl_len, tsc, speed) ){
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
@name: tdm_td2p_filter_tdm5
@param:

Smooth MMU main calendar in terms of linerate and/or oversub distribution
 */
int
tdm_td2p_filter_tdm5( tdm_mod_t *_tdm )
{
	int i, k, timeout=TIMEOUT, filter_result, filter_cnt, 
		max_size, min_size, max_size_k, min_size_k, max_slice_idx, min_slice_idx, slice_idx,
		*tdm_pipe_main, tdm_pipe_main_len, **tdm_port_pmap, ovsb_token;
	enum port_speed_e *tdm_port_speed;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	tdm_port_pmap = _tdm->_chip_data.soc_pkg.pmap;
	tdm_port_speed= _tdm->_chip_data.soc_pkg.speed;
	ovsb_token    = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
	
	TDM_BIG_BAR
	TDM_PRINT0("Filters Applied to smooth MMU TDM calendar\n");
	TDM_SML_BAR
	
	/* Case 1: linerate only */
	if(_tdm->_core_data.vars_pkg.lr_enable && !(_tdm->_core_data.vars_pkg.os_enable) && !(_tdm->_core_data.vars_pkg.refactor_done)){
		filter_result = tdm_td2p_filter_smooth_idle_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---IDLE SLOT SMOOTH, filter applied <%d> times\n", filter_result);
		TDM_SML_BAR
	}
	/* Case 2: linerate and oversub mixed */
	if (_tdm->_core_data.vars_pkg.lr_enable && _tdm->_core_data.vars_pkg.os_enable && !(_tdm->_core_data.vars_pkg.refactor_done)) {
		/* Smooth the extremely unbalanced ovsb clump */
		filter_cnt= 0;
		max_size = tdm_td2p_scan_mix_slice_max(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &max_slice_idx, 0);
		min_size = tdm_td2p_scan_mix_slice_min(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &min_slice_idx, 0);
		if( (max_size-min_size)>=6 ){
			for (k=1; k<max_size-min_size; k++){
				filter_result = tdm_td2p_filter_migrate_os_slot(max_slice_idx, min_slice_idx, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed );
				if(filter_result==FAIL){break;}
				filter_cnt++;
				max_size_k = tdm_td2p_scan_mix_slice_max(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &max_slice_idx, 0);
				min_size_k = tdm_td2p_scan_mix_slice_min(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &min_slice_idx, 0);
				if ((max_size_k-min_size_k)<=1) {break;}
			}
		}
		TDM_PRINT1("\nFilter done: ---OVSB MAX/MIN SLICE BALANCE, filter applied <%d> times\n", filter_cnt);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_cnt = 0;
		timeout = _tdm->_chip_data.soc_pkg.lr_idx_limit;
		do{
			filter_result = tdm_td2p_filter_smooth_os_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed, DN);
			TDM_PRINT2("\nFilter done: ---OVSB MAX SLICE REDUCTION (1) Shift Down (%2d), filter applied <%d> times\n", ++filter_cnt, filter_result);
			TDM_SML_BAR
		} while ( filter_result>0 && (timeout--)>0 );
		filter_cnt = 0;
		timeout = _tdm->_chip_data.soc_pkg.lr_idx_limit;
		do{
			filter_result = tdm_td2p_filter_smooth_os_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed, UP);
			TDM_PRINT2("\nFilter done: ---OVSB MAX SLICE REDUCTION (2) Shift Up (%2d), filter applied <%d> times\n", ++filter_cnt, filter_result);
			TDM_SML_BAR
		} while ( filter_result>0 && (timeout--)>0 );

		/* Smooth oversub distribution */
		filter_result = tdm_td2p_filter_smooth_os_os_up(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB LOCAL SLICE REDUCTION (1) slot UP, X_Y_OVSB_OVSB, filter applied <%d> times \n", filter_result);
		TDM_SML_BAR
		filter_result = tdm_td2p_filter_smooth_os_os_dn(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB LOCAL SLICE REDUCTION (2) slot DOWN, OVSB_OVSB_X_Y, filter applied <%d> times \n", filter_result);
		TDM_SML_BAR

		/* Smooth oversub distribution */
/* 		filter_result = tdm_td2p_filter_smooth_os_slice_fine(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB SLOT MIGRATION, filter applied <%d> times\n", filter_result);
		TDM_SML_BAR */
		
		/* Smooth oversub distribution */
		filter_result = 0;
		timeout=DITHER_PASS;
		while ( tdm_td2p_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_td2p_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_td2p_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
		       ((--timeout)>0) ) {			
			filter_result += tdm_td2p_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,DITHER_THRESHOLD,tdm_port_speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_td2p_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_td2p_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_td2p_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
				((--timeout)>0) ) {			
			filter_result += tdm_td2p_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE),tdm_port_speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_td2p_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_td2p_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_td2p_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
				((--timeout)>0) ) {			
			filter_result += tdm_td2p_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE-DITHER_SUBPASS_STEP_SIZE),tdm_port_speed);
		}
		TDM_PRINT1("Filter done: ---DITHER (1) filter finished, applied times %d\n", filter_result);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_result = 0;
		if (tdm_td2p_slice_size(1, tdm_pipe_main, tdm_pipe_main_len)==2) {
			for (i=3; i<tdm_pipe_main_len; i++) {
				if (tdm_pipe_main[i-3]!=ovsb_token && 
				    tdm_pipe_main[i  ]!=ovsb_token && 
					tdm_pipe_main[i+3]!=ovsb_token && 
					tdm_pipe_main[i+6]!=ovsb_token && 
					tdm_td2p_scan_slice_size_local((i-3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_td2p_scan_slice_size_local( i,    tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_td2p_scan_slice_size_local((i+3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_td2p_scan_slice_size_local((i+6), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2) {
					if (tdm_td2p_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap)==FAIL) {
						break;
					}
					filter_result++;
				}
			}
			for (i=3; i<tdm_pipe_main_len; i++) {
				if (tdm_pipe_main[i-3]!=ovsb_token && 
				    tdm_pipe_main[i  ]!=ovsb_token && 
					tdm_pipe_main[i+3]!=ovsb_token && 
					tdm_td2p_scan_slice_size_local((i-3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_td2p_scan_slice_size_local( i,    tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_td2p_scan_slice_size_local((i+3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2) {
					if (tdm_td2p_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap)==FAIL) {
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
@name: tdm_td2p_filter_chk_shift_slot
@param:
 */
int
tdm_td2p_filter_chk_shift_slot(tdm_mod_t *_tdm, int slot_idx, int dir)
{
    int i, j, port_i, port_j, tsc_i, tsc_j, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_space_sister,
        param_phy_lo, param_phy_hi;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    if (slot_idx>=0 && slot_idx<param_cal_len && param_space_sister>0){
        /* set index for shift-DOWN */
        if (dir==DN){
            i = slot_idx;
            j = (i+param_space_sister)%param_cal_len;
        }
        /* set index for shift-UP */
        else {
            i = slot_idx;
            j = (param_cal_len+i-param_space_sister)%param_cal_len;
        }
        /* check tsc */
        port_i = param_cal_main[i];
        port_j = param_cal_main[j];
        if(port_i>=param_phy_lo && port_i<=param_phy_hi &&
           port_j>=param_phy_lo && port_j<=param_phy_hi){
            _tdm->_core_data.vars_pkg.port = port_i;
            tsc_i  = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
            _tdm->_core_data.vars_pkg.port = port_j;
            tsc_j  = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
            if (tsc_i==tsc_j){
                result = FAIL;
            }
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_filter_shift_slot
@param:
 */
int
tdm_td2p_filter_shift_slot(tdm_mod_t *_tdm, int slot_idx, int dir)
{
    int port_idx, shift_dir, idx_src=0, idx_dst=0, port_src, port_dst,
        result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    port_idx  = slot_idx;
    shift_dir = dir;
    if(port_idx>=0 && port_idx<param_cal_len){
        /* check shift-up condition */
        if (shift_dir==UP){
            idx_src = port_idx;
            idx_dst = (param_cal_len+idx_src-1)%param_cal_len;
            if (tdm_td2p_filter_chk_shift_slot(_tdm, idx_src, UP)!=PASS ||
                tdm_td2p_filter_chk_shift_slot(_tdm, idx_dst, DN)!=PASS ){
                result = FAIL;
            }
        }
        /* check shift-down condition */
        else {
            idx_src = port_idx;
            idx_dst = (idx_src+1)%param_cal_len;
            if (tdm_td2p_filter_chk_shift_slot(_tdm, idx_src, DN)!=PASS ||
                tdm_td2p_filter_chk_shift_slot(_tdm, idx_dst, UP)!=PASS ){
                result = FAIL;
            }
        }
        /* shift */
        if (result==PASS){
            port_src= param_cal_main[idx_src];
            port_dst= param_cal_main[idx_dst];
            param_cal_main[idx_dst] = port_src;
            param_cal_main[idx_src] = port_dst;
            TDM_PRINT3("TDM: -- shift port %3d from index #%03d to #%03d\n",
                      port_src, idx_src, idx_dst);
        }
    }
    else {
        result = FAIL;
    }
    
    return result;
}


/**
@name: tdm_td2p_filter_shift_lr_port_e
@param:
 */
int
tdm_td2p_filter_shift_lr_port_e(tdm_mod_t *_tdm, int port_token, int port_tsc, int dir)
{
    int i, port_i, tsc_i, result=PASS;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_phy_lo, param_phy_hi,
        param_space_sister;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_phy_lo      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi      = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_space_sister= _tdm->_core_data.rule__prox_port_min;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    for (i=0; i<param_cal_len; i++){
        port_i = param_cal_main[i];
        if (port_i>=param_phy_lo && port_i<=param_phy_hi &&
            port_i!=port_token){
            _tdm->_core_data.vars_pkg.port = port_i;
            tsc_i = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
            if (tsc_i==port_tsc){
                /* shift down */
                if (dir==DN){
                    if(tdm_td2p_filter_shift_slot(_tdm, i, DN)!=PASS){
                        result = FAIL;
                    }
                    i += param_space_sister;
                }
                /* shift up */
                else {
                    if(tdm_td2p_filter_shift_slot(_tdm, i, UP)!=PASS){
                        result = FAIL;
                    }
                    i = (param_space_sister>0)?(i+param_space_sister-1):(i);
                }
            }
        }
    }
    
    return result;
}


/**
@name: tdm_td2p_filter_smooth_ancl
@param:
 */
int
tdm_td2p_filter_smooth_ancl(tdm_mod_t *_tdm)
{
    int dist_ratio, dist_max, dist_min, dist_step;
    int i, j, k, port_idx, port_token, dist_dn,
        slot_cnt=0, mgmt_slot_num=0;
    int param_lr_limit, param_ancl_num, param_cal_len,
        param_lr_en,
        param_mgmt_bw;
    int *param_cal_main;
    
    param_lr_limit    = _tdm->_chip_data.soc_pkg.lr_idx_limit;
    param_ancl_num    = _tdm->_chip_data.soc_pkg.tvec_size;
    param_cal_len     = param_lr_limit + param_ancl_num;
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_mgmt_bw     = _tdm->_chip_data.soc_pkg.soc_vars.td2p.mgmtbw;
    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,param_cal_main);
    
    if (param_mgmt_bw==1 || param_mgmt_bw==4){
        for (i=0; i<param_cal_len; i++){
            port_token = param_cal_main[i];
            if (port_token==TD2P_MGMT_PORT_0 || 
                port_token==TD2P_MGMT_PORT_1 || 
                port_token==TD2P_MGMT_PORT_2 || 
                port_token==TD2P_MGMT_PORT_3 ){
                mgmt_slot_num ++;
            }
        }
    }
    if (param_lr_en==BOOL_TRUE && param_ancl_num>0 && mgmt_slot_num>0){
        if (param_mgmt_bw==1){
            TDM_PRINT1("TDM: Smooth CPU/MGMT slot distribution (TOKEN [%d])\n\n",
                       TD2P_MGMT_PORT_0);
        }
        else{
            TDM_PRINT4("TDM: Smooth CPU/MGMT slot distribution (TOKEN [%d,%d,%d,%d])\n\n",
                       TD2P_MGMT_PORT_0,
                       TD2P_MGMT_PORT_1,
                       TD2P_MGMT_PORT_2, 
                       TD2P_MGMT_PORT_3);
        }
        slot_cnt  = 0;
        dist_ratio= 20;
        dist_max  = (param_cal_len*(100+dist_ratio))/(mgmt_slot_num*100);
        dist_min  = (param_cal_len*(100-dist_ratio))/(mgmt_slot_num*100);
        for (i=0; i<param_cal_len; i++){
            port_token = param_cal_main[i];
            if (port_token==TD2P_MGMT_PORT_0 ||
                port_token==TD2P_MGMT_PORT_1 ||
                port_token==TD2P_MGMT_PORT_2 ||
                port_token==TD2P_MGMT_PORT_3 ){
                slot_cnt++;
                dist_dn = 0;
                for (j=1; j<param_cal_len; j++){
                    k = (i+j)%param_cal_len;
                    port_token =  param_cal_main[k];
                    if (port_token==TD2P_MGMT_PORT_0 ||
                        port_token==TD2P_MGMT_PORT_1 ||
                        port_token==TD2P_MGMT_PORT_2 ||
                        port_token==TD2P_MGMT_PORT_3 ){
                        dist_dn = (param_cal_len+k-i)%param_cal_len;
                        break;
                    }
                }
                TDM_PRINT7("TDM: %s %d, index #%03d, %s = {%d | [%d, %d]}\n",
                           "CPU/MGMT slot",
                           slot_cnt,
                           i,
                           "{dist_dn | [min, max]}", 
                           dist_dn, 
                           dist_min, 
                           dist_max);
                /* shift up */
                if (dist_dn<dist_min){
                    dist_step = (dist_min+2-dist_dn);
                    for (j=0; j<dist_step; j++){
                        port_idx = (param_cal_len+i-j)%param_cal_len;
                        if (tdm_td2p_filter_shift_slot(_tdm, port_idx, UP)!=PASS){
                            break;
                        }
                    }
                }
                /* shift down */
                else if (dist_dn>dist_max){
                    dist_step = (dist_dn+2-dist_max);
                    for (j=0; j<dist_step; j++){
                        port_idx = (i+j)%param_cal_len;
                        if (tdm_td2p_filter_shift_slot(_tdm, port_idx, DN)!=PASS){
                            break;
                        }
                    }
                }
                i += (dist_dn-1);
            }
        }
        TDM_SML_BAR
    }
    
    return PASS;
}


/**
@name: tdm_td2p_filter_ovsb_p1
@param:
 */
int
tdm_td2p_filter_ovsb_p1(tdm_mod_t *_tdm)
{
        int i, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox;
    int param_lr_limit, param_ancl_num, param_cal_len, 
        param_space_sister,
        param_token_ovsb, param_token_ancl,
        param_phy_lo, param_phy_hi,
        param_lr_en, param_os_en;
    int *param_cal_main;
    enum port_speed_e *param_speeds;
    
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
    param_speeds      = _tdm->_chip_data.soc_pkg.speed;
    
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
                    port_x = param_cal_main[i+1];
                    port_y = param_cal_main[i+3];
                    if (port_x>=param_phy_lo && port_x<=param_phy_hi)
                        if (param_speeds[port_x] >= SPEED_100G) continue;
                    if (port_y>=param_phy_lo && port_y<=param_phy_hi)
                        if (param_speeds[port_y] >= SPEED_100G) continue;
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
@name: tdm_td2p_filter_ovsb_p2
@param:
 */
int
tdm_td2p_filter_ovsb_p2(tdm_mod_t *_tdm)
{
    int i, j, k, lr_slot_cnt, os_slot_cnt, filter_cnt=0,
        x, y, port_x, port_y, tsc_x, tsc_y, sister_prox,
        dist_up, dist_dn, dist_range, timeout, shift_exist=BOOL_TRUE;
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
    
    TDM_PRINT0("Smooth OVSB pattern: sparse OVSB distribution\n\n");
    
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
            4*os_slot_cnt<=lr_slot_cnt){
            timeout = param_cal_len;
            
            while((timeout--)>0 && shift_exist==BOOL_TRUE){
                shift_exist = BOOL_FALSE;
                for (i=1; i<param_cal_len; i++){
                    if (param_cal_main[i]==param_token_ovsb){
                        dist_up = 0;
                        dist_dn = 0;
                        for (j=i-1; j>=0; j--){
                            if (param_cal_main[j]!=param_token_ovsb){
                                dist_up++;
                            }
                            else{
                                break;
                            }
                            if (j==0 && param_cal_main[j]!=param_token_ovsb){
                                for (k=param_cal_len-1; k>i; k--){
                                    if (param_cal_main[k]!=param_token_ovsb){
                                        dist_up++;
                                    }
                                    else{
                                        break;
                                    }
                                }
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
                        if (dist_up>=(dist_dn+2)){
                            dist_range = (dist_up - dist_dn)/2;
                            for (j=i; j>(i-dist_range) && j>1; j--){
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
                                    shift_exist = BOOL_TRUE;
                                    TDM_PRINT2("Shift OVSB slot UP from %03d to %03d\n",
                                              x+1, x);
                                }
                                else {
                                    break;
                                }
                            }
                        }
                        else if (dist_dn>=(dist_up+2)){
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
                                    shift_exist = BOOL_TRUE;
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
        }
    }
    
    return filter_cnt;
}


/**
@name: tdm_td2p_filter_ovsb_5x
@param:
 */
int
tdm_td2p_filter_ovsb_5x(tdm_mod_t *_tdm)
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
        if (os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
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
@name: tdm_td2p_filter_ovsb_4x
@param:
 */
int
tdm_td2p_filter_ovsb_4x(tdm_mod_t *_tdm)
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
    
    TDM_PRINT0("Smooth quadrant OVSB pattern: OVSB_OVSB_OVSB_OVSB_X_OVSB_Y \n\n");
    
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
        if (os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
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
@name: tdm_td2p_filter_ovsb_3x
@param:
 */
int
tdm_td2p_filter_ovsb_3x(tdm_mod_t *_tdm)
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
        if (os_slot_cnt>lr_slot_cnt && os_slot_cnt<2*lr_slot_cnt){
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
@name: tdm_td2p_filter_ovsb_2x
@param:
 */
int
tdm_td2p_filter_ovsb_2x(tdm_mod_t *_tdm)
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
    
    TDM_PRINT0("Smooth dual OVSB pattern: OVSB_OVSB_X_Y \n\n");
    
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
        /* ovsb_ovsb_x_z */
        if (os_slot_cnt>0 && os_slot_cnt<lr_slot_cnt){
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
@name: tdm_td2p_filter_ovsb_1x
@param:
 */
int
tdm_td2p_filter_ovsb_1x(tdm_mod_t *_tdm)
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
        /* 2X unbalanced neighbouring slots */
        if (os_slot_cnt>0 && 2*os_slot_cnt<=lr_slot_cnt){
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
@name: tdm_td2p_filter_ovsb
@param:
 */
int
tdm_td2p_filter_ovsb(tdm_mod_t *_tdm)
{
    int filter_cnt;
    
    /* Z_Y_OVSB_X_OVSB_OVSB */
    tdm_td2p_filter_ovsb_p1(_tdm);
    /* OVSB_OVSB_OVSB_OVSB */
    tdm_td2p_filter_ovsb_5x(_tdm);
    /* OVSB_OVSB_OVSB_OVSB */
    tdm_td2p_filter_ovsb_4x(_tdm);
    /* OVSB_OVSB_OVSB */
    tdm_td2p_filter_ovsb_3x(_tdm);
    /* OVSB_OVSB */
    tdm_td2p_filter_ovsb_2x(_tdm);
    /* OVSB */
    filter_cnt = tdm_td2p_filter_ovsb_1x(_tdm);
    if (filter_cnt>0){
        tdm_td2p_filter_ovsb_1x(_tdm);
    }
    /* sparse OVSB */
    tdm_td2p_filter_ovsb_p2(_tdm);
    
    return PASS;
}

/**
@name: tdm_td2p_filter_lr_40g
@param:
 */
int
tdm_td2p_filter_lr_40g(tdm_mod_t *_tdm)
{
    int i, j, filter_cnt=0;
    int port, port_tmp, dist, dist_up, dist_dn, i_up, i_dn, dir, i_dst;
    int space_min_40g = 11;
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

    TDM_PRINT0("Filtering 40G linerate port ...\n\n");

    TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id, param_cal_main);
    if (param_lr_en != BOOL_TRUE) return filter_cnt;
    for (i=1; i<param_cal_len; i++) {
        port = param_cal_main[i];
        if (port >= param_phy_lo && port <= param_phy_hi) {
            if (param_speeds[port] == SPEED_40G ||
                param_speeds[port] == SPEED_42G) {
                /* check up and down distance of the same port */
                dist_up = 0;
                dist_dn = 0;
                for (j=1; j<param_cal_len; j++) {
                    i_up = (i + param_cal_len - j) % param_cal_len;
                    i_dn = (i + j) % param_cal_len;
                    if (param_cal_main[i_up]==port && dist_up==0) dist_up = j;
                    if (param_cal_main[i_dn]==port && dist_dn==0) dist_dn = j;
                    if (dist_up>0 && dist_dn>0) break;
                }
                if (dist_up>=space_min_40g && dist_dn>=space_min_40g) continue;
                /* filter current port to avoid min space violation */
                dir  = (dist_up<space_min_40g) ? (1) : (-1);
                if (dir == 1) {
                    dist = (dist_dn>space_min_40g) ? dist_dn-space_min_40g : 0;
                } else {
                    dist = (dist_up>space_min_40g) ? dist_up-space_min_40g : 0;
                }
                for (j=1; j<param_cal_len && j < dist; j++) {
                    i_dst = (i + param_cal_len + dir * j) % param_cal_len;
                    port_tmp = param_cal_main[i_dst];
                    if (!(port_tmp >= param_phy_lo && port_tmp <= param_phy_hi)) {
                        param_cal_main[i_dst] = port;
                        param_cal_main[i] = port_tmp;
                        filter_cnt++;
                        TDM_PRINT4("%s, migrate port %3d from slot %03d to %03d\n",
                                   "Filter 40G linerate port",
                                   port, i, i_dst);
                        break;
                    }
                }
            }
        }
    }
    TDM_SML_BAR

    return filter_cnt;
}

/**
@name: tdm_td2p_filter_lr
@param:
 */
int
tdm_td2p_filter_lr(tdm_mod_t *_tdm)
{
    tdm_td2p_filter_lr_40g(_tdm);

    return PASS;
}

int
tdm_td2p_filter( tdm_mod_t *_tdm )
{
    int param_lr_en, param_os_en, param_mgmt_bw;
    
    param_lr_en       = _tdm->_core_data.vars_pkg.lr_enable;
    param_os_en       = _tdm->_core_data.vars_pkg.os_enable;
    param_mgmt_bw     = _tdm->_chip_data.soc_pkg.soc_vars.td2p.mgmtbw;
    
    TDM_BIG_BAR
    TDM_PRINT0("TDM: Filters Applied to smooth TDM calendar\n");
    
    /* Smooth OVSB slots */
    if (param_os_en==BOOL_TRUE && param_lr_en==BOOL_TRUE){
        TDM_SML_BAR
        TDM_PRINT0("TDM: Filters Applied to smooth OVSB slot distribution\n");
        TDM_SML_BAR
        tdm_td2p_filter_ovsb(_tdm);
    }
    /* Smooth CPU/MGMT slot */
    if (param_mgmt_bw>0){
        TDM_SML_BAR
        TDM_PRINT0("TDM: Filters Applied to smooth CPU/Management slot distribution\n");
        TDM_SML_BAR
        tdm_td2p_filter_smooth_ancl(_tdm);
    }
    /* Smooth Linerate slots */
    tdm_td2p_filter_lr(_tdm);
    
    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__PARSE]( _tdm ) );
}
