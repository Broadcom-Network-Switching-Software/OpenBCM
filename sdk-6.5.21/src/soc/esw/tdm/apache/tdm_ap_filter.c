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
@name: tdm_ap_filter_dither
@param:
 */
int
tdm_ap_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed)
{
	int g, i, j, k=1, l=AP_NUM_EXT_PORTS, slice_idx, shift_cnt=0;
	unsigned short dither_shift_done=BOOL_FALSE, dither_done=BOOL_FALSE, dither_slice_counter=0;
	
	/* Get index of the OVSB slot with the largest clump size */
	dither_shift_done=BOOL_FALSE;
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==AP_OVSB_TOKEN && tdm_ap_scan_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories), &slice_idx)==tdm_ap_slice_size(AP_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))) {
			g=i; while( (tdm_tbl[g]==AP_OVSB_TOKEN) && (g<(lr_idx_limit+accessories)) ) {g++;}
			if ( tdm_ap_slice_prox_up(g,tdm_tbl,tsc,speed) &&
				 tdm_ap_slice_prox_dn((tdm_ap_slice_idx(AP_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))-1),tdm_tbl,(lr_idx_limit+accessories),tsc,speed) ) {
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
			while (tdm_tbl[i]!=AP_OVSB_TOKEN && tdm_ap_scan_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories),&slice_idx)==tdm_ap_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && i<(lr_idx_limit+accessories) ) {
				if (++dither_slice_counter>=threshold && tdm_tbl[i-1]==AP_OVSB_TOKEN) {
					for (j=255; j>i; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[i+1]=AP_OVSB_TOKEN;
					dither_done=BOOL_TRUE;
					break;
				}
				i++; if (tdm_tbl[i]==AP_OVSB_TOKEN) {i++;}
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
@name: tdm_ap_filter_fine_dither
@param:
 */
int
tdm_ap_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc)
{
	int i, j, k, min_prox=11,slice_idx;
	unsigned short fine_dither_done;

	i=port; fine_dither_done=BOOL_FALSE;
	for (j=2; j<(lr_idx_limit+accessories-4); j++) {
		if ( tdm_tbl[j]!=AP_OVSB_TOKEN && tdm_tbl[j-2]!=AP_OVSB_TOKEN && tdm_tbl[j+2]!=AP_OVSB_TOKEN && tdm_tbl[j+4]!=AP_OVSB_TOKEN && 
			 tdm_ap_scan_slice_size_local((j-2), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_ap_scan_slice_size_local( j,    tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_ap_scan_slice_size_local((j+2), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 && 
			 tdm_ap_scan_slice_size_local((j+4), tdm_tbl, (lr_idx_limit+accessories), &slice_idx)==1 &&
			 tdm_ap_slice_prox_local( j,    tdm_tbl, (lr_idx_limit+accessories), tsc)>min_prox )
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
		tdm_tbl[i]=AP_OVSB_TOKEN;
		
		return PASS;
	}
	else {
		return FAIL;
	}
}


/**
@name: tdm_ap_filter_shift_lr_port
@param:

Shifts all slots of the given linerate port up or down,
and returns the total number of shifting occurence.
 */
int
tdm_ap_filter_shift_lr_port(unsigned short port, int *tdm_tbl, int tdm_tbl_len, int dir)
{
	int i, shift_cnt=0, port_swap;
	
	AP_TOKEN_CHECK(port) {
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
@name: tdm_ap_filter_migrate_os_slot
@param:

Migrate OVSB slot from src to dst in an array.
 */
int
tdm_ap_filter_migrate_os_slot(int idx_src, int idx_dst, int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i=idx_src, j, token_tmp, filter_result=FAIL, check_pass=BOOL_TRUE,
	    idx0, idx1, tsc0, tsc1;
	
	if ( !(idx_src>=0 && idx_src<tdm_tbl_len) ||  !(idx_dst>=0 && idx_dst<tdm_tbl_len) ){
		check_pass = BOOL_FALSE;
	}
	if ( tdm_tbl[idx_src]!=AP_OVSB_TOKEN && tdm_tbl[idx_src]!=AP_ANCL_TOKEN) {
		check_pass = BOOL_FALSE;
	}
	/* Check sister port spacing */
	if (check_pass==BOOL_TRUE){
		i = idx_src;
		for (j=1; j<VBS_MIN_SPACING; j++){
			idx0 = ((i+j)<tdm_tbl_len)? (i+j): (i+j-tdm_tbl_len);
			idx1 = ((idx0-VBS_MIN_SPACING)>=0) ? (idx0-VBS_MIN_SPACING) : (idx0-VBS_MIN_SPACING+tdm_tbl_len);
			AP_TOKEN_CHECK(tdm_tbl[idx0]){
				AP_TOKEN_CHECK(tdm_tbl[idx1]){
					tsc0 = tdm_ap_legacy_which_tsc(tdm_tbl[idx0],tsc);
					tsc1 = tdm_ap_legacy_which_tsc(tdm_tbl[idx1],tsc);
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
			AP_TOKEN_CHECK(tdm_tbl[idx0]){
				AP_TOKEN_CHECK(tdm_tbl[idx1]){
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
@name: tdm_ap_filter_smooth_idle_slice
@param:

Smooth MMU TDM calendar by migrating IDLE slots
 */
int
tdm_ap_filter_smooth_idle_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, k, idle1_token, idle2_token, pos_cnt, pos_step, pos,
	    idle_slot_cnt=0, idle_slice_max, idle_slice_max_idx, lr_slice_max, lr_slice_max_idx, 
		idx_x, idx_y, idx1, idx2, tsc_x, tsc_y, tsc1, tsc2, slot_token, slot_dst_idx,
		filter_done, filter_cnt=0;
	
	idle1_token = AP_IDL1_TOKEN;
	idle2_token = AP_IDL2_TOKEN;
	
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
			idle_slice_max = tdm_ap_scan_slice_max(idle1_token, tdm_tbl,tdm_tbl_len, &idle_slice_max_idx,0);
			lr_slice_max   = tdm_ap_scan_mix_slice_max(1,tdm_tbl,tdm_tbl_len, &lr_slice_max_idx,pos);
			
			/* Find idle clump with minimum size 2: ..._y_x_IDLE_IDLE_... */
			if ( (idle_slice_max>1 && lr_slice_max>0) &&
			     (idle_slice_max_idx<tdm_tbl_len && idle_slice_max_idx>1) &&
			     ((lr_slice_max_idx<tdm_tbl_len) && ((lr_slice_max_idx>0) || (lr_slice_max_idx==0 && lr_slice_max>1))) ){
				/* Check sister port spacing */
				idx_x = idle_slice_max_idx-1;
				idx_y = idle_slice_max_idx-2;
				idx1  = ((idle_slice_max_idx+2)<tdm_tbl_len) ? (idle_slice_max_idx+2) : ((idle_slice_max_idx+2)-tdm_tbl_len);
				idx2  = ((idle_slice_max_idx+3)<tdm_tbl_len) ? (idle_slice_max_idx+3) : ((idle_slice_max_idx+3)-tdm_tbl_len);
				tsc_x = tdm_ap_legacy_which_tsc(tdm_tbl[idx_x],tsc);
				tsc_y = tdm_ap_legacy_which_tsc(tdm_tbl[idx_y],tsc);
				tsc1  = tdm_ap_legacy_which_tsc(tdm_tbl[idx1],tsc);
				tsc2  = tdm_ap_legacy_which_tsc(tdm_tbl[idx2],tsc);
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
@name: tdm_ap_filter_smooth_os_slice
@param:

Smooth MMU TDM calendar by shifting linerate ports Downward/Upward
		Downward: x_ovsb___x_ovsb___x_ovsb -> ovsb_x___ovsb_x___ovsb_x
		Upward  : ovsb_x___ovsb_x___ovsb_x -> x_ovsb___x_ovsb___x_ovsb
 */
int
tdm_ap_filter_smooth_os_slice(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed, int dir)
{
	int i, slice_idx, shift_dir, shift_cnt=0, lr_clump_max, lr_clump_min, filter_port=0;
	const char *str_dir;
	
	str_dir      = (dir==UP) ? "UPWARD": "DOWNWARD";
	shift_dir    = (dir==UP) ? (UP) : (DN);
	
	for (i=1; i<tdm_tbl_len; i++) {
		filter_port=tdm_tbl[i];
		if (filter_port==tdm_tbl[0]) {continue;}
		
		lr_clump_max = tdm_ap_scan_mix_slice_max(1,tdm_tbl,tdm_tbl_len, &slice_idx, 0);
		lr_clump_min = tdm_ap_scan_mix_slice_min(1,tdm_tbl,tdm_tbl_len, &slice_idx, 0);
		if ( (lr_clump_max<=1) || 
		     (lr_clump_max==2 && lr_clump_min==1)){
			break;
		}
		if ( FAIL==tdm_ap_check_shift_cond_pattern(filter_port, tdm_tbl, tdm_tbl_len, tsc, dir) ) {
			continue;
		}
		else if ( FAIL==tdm_ap_check_shift_cond_local_slice(filter_port, tdm_tbl, tdm_tbl_len, tsc, dir) ) {
			continue;
		}
		else {
			tdm_ap_filter_shift_lr_port(filter_port, tdm_tbl, tdm_tbl_len, shift_dir);
			TDM_PRINT3("Filter applied: Vector shift %8s, port %3d, beginning index #%03d \n", str_dir, filter_port, i);
			shift_cnt++;
		}
	}
	
	return shift_cnt;
}


/**
@name: tdm_ap_filter_smooth_os_slice_fine
@param:

Smooth MMU TDM calendar by migrating OVSB slot
 */
int
tdm_ap_filter_smooth_os_slice_fine(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int j, k, ovsb_token, pos, pos_step, pos_cnt, pos_cnt_max,
	    slice_max, slice_min, slice_max_idx, slice_min_idx, pp_prox,
		idx0, idx1, tsc0, tsc1, slot_tmp, idx_dst, idx_src, filter_cnt=0;
	
	ovsb_token  = AP_OVSB_TOKEN;
	pos         = 0;
	pos_cnt     = 0;
	pos_cnt_max = 16;
	pos_step    = tdm_tbl_len/pos_cnt_max + 1;
	
	for (k=0; k<tdm_tbl_len; k++){
		pos      = pos_cnt*pos_step;
		pos_cnt  = ((pos_cnt+1)<pos_cnt_max)? (pos_cnt+1): (0);
		slice_max= tdm_ap_scan_slice_max(ovsb_token,tdm_tbl,tdm_tbl_len, &slice_max_idx,k);
		slice_min= tdm_ap_scan_slice_min(ovsb_token,tdm_tbl,tdm_tbl_len, &slice_min_idx,pos);
		
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
		AP_TOKEN_CHECK(tdm_tbl[idx0]){
			AP_TOKEN_CHECK(tdm_tbl[idx1]){
				tsc0 = tdm_ap_legacy_which_tsc(tdm_tbl[idx0],tsc);
				tsc1 = tdm_ap_legacy_which_tsc(tdm_tbl[idx1],tsc);
				if ( tsc0==tsc1 ){
					pp_prox=FAIL;
				}
			}
		}
		/* Check same port spacing */
		for (j=1; j<LLS_MIN_SPACING; j++){
			idx0 = ((idx_src-j)>=0)? (idx_src-j): (idx_src-j+tdm_tbl_len);
			idx1 = ((idx0+LLS_MIN_SPACING)<tdm_tbl_len)? (idx0+LLS_MIN_SPACING): (idx0+LLS_MIN_SPACING-tdm_tbl_len);
			AP_TOKEN_CHECK(tdm_tbl[idx0]){
				AP_TOKEN_CHECK(tdm_tbl[idx1]){
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
@name: tdm_ap_filter_smooth_os_os_up
@param:

Smooth MMU TDM calendar by transforming:
        --- _x_y_ovsb_ovsb_ -> _x_ovsb_y_ovsb_
 */
int tdm_ap_filter_smooth_os_os_up(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, slot_tmp, filter_cnt=0;
	
	for (i=2; i<(tdm_tbl_len-1); i++) {
		if (tdm_tbl[i  ]==AP_OVSB_TOKEN &&
			tdm_tbl[i+1]==AP_OVSB_TOKEN ){
			AP_TOKEN_CHECK(tdm_tbl[i-1]){
				AP_TOKEN_CHECK(tdm_tbl[i-2]){
					if ( PASS==tdm_ap_check_slot_swap_cond((i-1), tdm_tbl, tdm_tbl_len, tsc, speed) ){
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
@name: tdm_ap_filter_smooth_os_os_dn
@param:

Smooth MMU TDM calendar by transforming:
		--- _ovsb_ovsb_x_y_ -> _ovsb_x_ovsb_y_
 */
int tdm_ap_filter_smooth_os_os_dn(int *tdm_tbl, int tdm_tbl_len, int **tsc, enum port_speed_e *speed)
{
	int i, slot_tmp, filter_cnt=0;
	
	for (i=1; i<(tdm_tbl_len-3); i++) {
		if (tdm_tbl[i  ]==AP_OVSB_TOKEN &&
			tdm_tbl[i+1]==AP_OVSB_TOKEN ){
			AP_TOKEN_CHECK(tdm_tbl[i+2]){
				AP_TOKEN_CHECK(tdm_tbl[i+3]){
					if ( PASS==tdm_ap_check_slot_swap_cond((i+1), tdm_tbl, tdm_tbl_len, tsc, speed) ){
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
@name: tdm_ap_filter_smooth_ancl
@param:

Smooth MMU TDM calendar with ANCILLARY slots
 */
int
tdm_ap_filter_smooth_ancl(int ancl_token, int *tdm_tbl, int tdm_tbl_len, int ancl_space_min)
{
	int i, j, g, l, pool, ancl_num=0, ancl_dist_up, ancl_dist_dn, lr_slice_size, lr_slice_min, slice_idx, timeout;
	
	lr_slice_min = tdm_ap_scan_slice_min(1, tdm_tbl, tdm_tbl_len, &slice_idx,0);
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
			AP_TOKEN_CHECK(tdm_tbl[i]) {
				lr_slice_size = tdm_ap_scan_slice_size_local(i,tdm_tbl,tdm_tbl_len,&slice_idx);
				ancl_dist_up  = tdm_ap_check_same_port_dist_up_port(ancl_token,i,tdm_tbl,tdm_tbl_len);
				ancl_dist_dn  = tdm_ap_check_same_port_dist_dn_port(ancl_token,i,tdm_tbl,tdm_tbl_len);
				
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
@name: tdm_ap_filter_chain
@param:

Smooth MMU main calendar in terms of linerate and/or oversub distribution
 */
int
tdm_ap_filter_chain( tdm_mod_t *_tdm )
{
    int i=0, k=0, timeout=TIMEOUT, filter_result, filter_cnt, one_slot_cnt = 0, 
    max_size, min_size, max_size_k, min_size_k, max_slice_idx, min_slice_idx,
		*tdm_pipe_main, tdm_pipe_main_len, **tdm_port_pmap, ovsb_token, tdm_clk_freq;
	enum port_speed_e *tdm_port_speed;
  unsigned short mirror_tbl[256];
  int cal_len=0,size=0;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	tdm_clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
	tdm_pipe_main_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	tdm_port_pmap = _tdm->_chip_data.soc_pkg.pmap;
	tdm_port_speed= _tdm->_chip_data.soc_pkg.speed;
	ovsb_token    = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
	
	TDM_BIG_BAR
	TDM_PRINT0("Filters Applied to smooth MMU TDM calendar\n");
	TDM_SML_BAR
 /*check for FB5 config:unique case */
if (_tdm->_core_data.vars_pkg.lr_enable && _tdm->_core_data.vars_pkg.os_enable && 
   (tdm_clk_freq==793 || tdm_clk_freq ==794 || tdm_clk_freq == 795 || tdm_clk_freq == 796 ||tdm_clk_freq == 797 )
   ) {
		TDM_PRINT0("Filter applied: smoothing ovs slots in FB Configs\n");

	for (i=0; i<tdm_pipe_main_len; i++){
		 AP_TOKEN_CHECK(tdm_pipe_main[i]) { 
			 /* check only 1G/2.5G is in LR in FB5 */
		       if(tdm_port_speed[tdm_pipe_main[i]] == SPEED_1G || tdm_port_speed[tdm_pipe_main[i]] == SPEED_2p5G) {
			     one_slot_cnt++; 
		       } else if (tdm_port_speed[tdm_pipe_main[i]] > SPEED_10G) { /* FB5 config can have 10G speed for Mgig traffic - earlier assumption was that 1G/2.5G traffic will be part of Mgig traffic. SDK-115853*/
			     break;
		       }
		  }
	}   
}

	/* case 793: calendar length is 402*/
  if(tdm_clk_freq==793 || tdm_clk_freq ==794 || tdm_clk_freq == 795 || tdm_clk_freq == 796 ||tdm_clk_freq == 797 ) {
    int toggle=0;  /* Flag to spread the 1G slots evenly*/
    cal_len = tdm_pipe_main_len;
    tdm_pipe_main_len = 402;

    for (i=0; i<cal_len; i++) {
      mirror_tbl[i] = tdm_pipe_main[i]; 
 /* FIX for JIRA SDK-98690 for 1G ports */
      if ( tdm_port_speed[mirror_tbl[i]] == SPEED_1G){
        toggle = ~toggle;
	      if (toggle)
/* SDK- 122032 - In FB5/MV config if only LR ports are enabled, reuse the 1G slot allocation for IDLE instead of OVS  */
	        tdm_pipe_main[i+cal_len] = (_tdm->_core_data.vars_pkg.os_enable ==1) ? AP_OVSB_TOKEN : AP_IDL1_TOKEN;   
        else  {
	        tdm_pipe_main[i] = (_tdm->_core_data.vars_pkg.os_enable ==1) ? AP_OVSB_TOKEN : AP_IDL1_TOKEN;   
	        tdm_pipe_main[i+cal_len] = mirror_tbl[i];   
        }
      }
      else { 
        tdm_pipe_main[i+cal_len] = mirror_tbl[i];
      }
    }

    for(i=0; i<512; i++) {
    if(tdm_pipe_main[i] == AP_NUM_EXT_PORTS) break;
    }
    size = i;
   
    while(size < tdm_pipe_main_len) {
      tdm_pipe_main[size] = AP_ANCL_TOKEN;
      size++;
    } 
  }

	/* Case 1: linerate only */
	if(_tdm->_core_data.vars_pkg.lr_enable && !(_tdm->_core_data.vars_pkg.os_enable) && !(_tdm->_core_data.vars_pkg.refactor_done)){
		/*filter_result = tdm_ap_filter_smooth_idle_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---IDLE SLOT SMOOTH, filter applied <%d> times\n", filter_result);*/
		TDM_SML_BAR
	}
	/* Case 2: linerate and oversub mixed */
	if (_tdm->_core_data.vars_pkg.lr_enable && _tdm->_core_data.vars_pkg.os_enable && !(_tdm->_core_data.vars_pkg.refactor_done) && (_tdm->_core_data.vars_pkg.lr_100 == 0) ) {
		/* Smooth the extremely unbalanced ovsb clump */
		filter_cnt= 0;
		max_size = tdm_ap_scan_mix_slice_max(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &max_slice_idx, 0);
		min_size = tdm_ap_scan_mix_slice_min(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &min_slice_idx, 0);
		if( (max_size-min_size)>=6 ){
			for (k=1; k<(max_size-min_size); k++){
				filter_result = tdm_ap_filter_migrate_os_slot(max_slice_idx, min_slice_idx, tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed );
				if(filter_result==FAIL){break;}
				filter_cnt++;
				max_size_k = tdm_ap_scan_mix_slice_max(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &max_slice_idx, 0);
				min_size_k = tdm_ap_scan_mix_slice_min(ovsb_token,tdm_pipe_main,tdm_pipe_main_len, &min_slice_idx, 0);
				if ((max_size_k-min_size_k)<=6) {break;}
			}
		}
		TDM_PRINT1("\nFilter done: ---OVSB MAX/MIN SLICE BALANCE, filter applied <%d> times\n", filter_cnt);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_cnt = 0;
		timeout = _tdm->_chip_data.soc_pkg.lr_idx_limit;
		do{
			filter_result = tdm_ap_filter_smooth_os_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed, (i%2) ? UP : DN);
			TDM_SML_BAR
		} while ( filter_result>0 && (timeout--)>0 );
		filter_cnt = 0;
		timeout = _tdm->_chip_data.soc_pkg.lr_idx_limit;
		do{
			filter_result = tdm_ap_filter_smooth_os_slice(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed, UP);
			TDM_PRINT2("\nFilter done: ---OVSB MAX SLICE REDUCTION (2) Shift Up (%2d), filter applied <%d> times\n", ++filter_cnt, filter_result);
			TDM_SML_BAR
		} while ( filter_result>0 && (timeout--)>0 );

		/* Smooth oversub distribution */
		filter_result = tdm_ap_filter_smooth_os_os_up(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB LOCAL SLICE REDUCTION (1) slot UP, X_Y_OVSB_OVSB, filter applied <%d> times \n", filter_result);
		TDM_SML_BAR
		filter_result = tdm_ap_filter_smooth_os_os_dn(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB LOCAL SLICE REDUCTION (2) slot DOWN, OVSB_OVSB_X_Y, filter applied <%d> times \n", filter_result);
		TDM_SML_BAR

		/* Smooth oversub distribution */
    /*filter_result = tdm_ap_filter_smooth_os_slice_fine(tdm_pipe_main, tdm_pipe_main_len, tdm_port_pmap, tdm_port_speed);
		TDM_PRINT1("\nFilter done: ---OVSB SLOT MIGRATION, filter applied <%d> times\n", filter_result);*/
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_result = 0;
		timeout=DITHER_PASS;
		while ( tdm_ap_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_ap_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_ap_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
		       ((--timeout)>0) ) {			
			filter_result += tdm_ap_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,DITHER_THRESHOLD,tdm_port_speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_ap_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_ap_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_ap_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
				((--timeout)>0) ) {			
			filter_result += tdm_ap_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE),tdm_port_speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_ap_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>1 && 
		        tdm_ap_slice_size(ovsb_token,tdm_pipe_main,tdm_pipe_main_len)>=tdm_ap_slice_size(1,tdm_pipe_main,tdm_pipe_main_len) && 
				((--timeout)>0) ) {			
			filter_result += tdm_ap_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE-DITHER_SUBPASS_STEP_SIZE),tdm_port_speed);
		}
		TDM_PRINT1("Filter done: ---DITHER (1) filter finished, applied times %d\n", filter_result);
		TDM_SML_BAR
		
		/* Smooth oversub distribution */
		filter_result = 0;
	/*	if (tdm_ap_slice_size(1, tdm_pipe_main, tdm_pipe_main_len)==2) {
			for (i=3; i<tdm_pipe_main_len; i++) {
				if (tdm_pipe_main[i-3]!=ovsb_token && 
				    tdm_pipe_main[i  ]!=ovsb_token && 
					tdm_pipe_main[i+3]!=ovsb_token && 
					tdm_pipe_main[i+6]!=ovsb_token && 
					tdm_ap_scan_slice_size_local((i-3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_ap_scan_slice_size_local( i,    tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_ap_scan_slice_size_local((i+3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_ap_scan_slice_size_local((i+6), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2) {
					if (tdm_ap_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap)==FAIL) {
						break;
					}
					filter_result++;
				}
			}
			for (i=3; i<tdm_pipe_main_len; i++) {
				if (tdm_pipe_main[i-3]!=ovsb_token && 
				    tdm_pipe_main[i  ]!=ovsb_token && 
					tdm_pipe_main[i+3]!=ovsb_token && 
					tdm_ap_scan_slice_size_local((i-3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_ap_scan_slice_size_local( i,    tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2 && 
					tdm_ap_scan_slice_size_local((i+3), tdm_pipe_main, tdm_pipe_main_len, &slice_idx)==2) {
					if (tdm_ap_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,tdm_port_pmap)==FAIL) {
						break;
					}
					filter_result++;
				}
			}
		}*/
		TDM_PRINT2("\nFilter done: ---DITHER (2) filter finished, applied times %d tdm_clk_freq=%0d\n", filter_result, tdm_clk_freq);
		TDM_SML_BAR
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__PARSE]( _tdm ) );
}



