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
@name: tdm_th2_filter_vector_shift
@param:

Translates an entire vector within flat unlinked table for purpose of smoothing oversub
Can only be used when all destination nodes are sparse
In this filter, only need to check prox port min spacing, as same port min spacing is guaranteed by the vector itself
 */
int
tdm_th2_filter_vector_shift(int *tdm_tbl, int port, int dir)
{
	int i, done=PASS, abort=BOOL_FALSE;
	
	TH2_TOKEN_CHECK(port) {
		if (dir==DN) {
			for (i=0; i<256; i++) {
				if (tdm_tbl[i]==port && tdm_tbl[i+1]!=TH2_OVSB_TOKEN) {
					abort=BOOL_TRUE;
					break;
				}
			}
			if (!abort) {
				for (i=0; i<256; i++) {
					if (tdm_tbl[i]==port) {
						tdm_tbl[i+1]=port;
						tdm_tbl[i]=TH2_OVSB_TOKEN;
						i++;
					}
				}
			}
			else {
				done=FAIL;
			}
		}
		else if (dir==UP) {
			for (i=1; i<256; i++) {
				if (tdm_tbl[i]==port && tdm_tbl[i-1]!=TH2_OVSB_TOKEN) {
					abort=BOOL_TRUE;
					break;
				}
			}
			if (!abort) {
				for (i=1; i<256; i++) {
					if (tdm_tbl[i]==port) {
						tdm_tbl[i-1]=port;
						tdm_tbl[i]=TH2_OVSB_TOKEN;
					}
				}
			}
			else {
				done=FAIL;
			}
		}
	}
	else {
		done=FAIL;
	}
	
	return done;
}


/**
@name: tdm_th2_filter_local_slice_dn
@param:
 */
void
tdm_th2_filter_local_slice_dn(int port, int *tdm_tbl, int **tsc, enum port_speed_e *speed)
{
	int i=port, j, pp_prox=PASS;

	if (tdm_tbl[i]<125 && tdm_tbl[i]>0) {
		for (j=1; j<4; j++) {
			if (tdm_th2_legacy_which_tsc(tdm_tbl[i+1+j],tsc)==tdm_th2_legacy_which_tsc(tdm_tbl[i],tsc)) {
				pp_prox=FAIL;
				break;
			}
		}
	}
/* #ifdef _LLS_SCHEDULER */
	if (speed[tdm_tbl[i]]<=SPEED_42G_HG2) {
		if (i<(TH2_VMAP_MAX_LEN-11)) {
			for (j=1; j<11; j++) {
				if (tdm_tbl[i+j]==tdm_tbl[i]) {
					pp_prox=FAIL;
					break;
				}
			}
		}
	}
/* #endif */
	if (pp_prox) {
		TDM_PRINT1("Filter applied: (1) Local Slice, Down, index %0d\n",port);
		tdm_tbl[i+1]=tdm_tbl[i];
		tdm_tbl[i]=TH2_OVSB_TOKEN;
	}

}


/**
@name: tdm_th2_filter_local_slice_up
@param:
 */
void
tdm_th2_filter_local_slice_up(int port, int *tdm_tbl, int **tsc, enum port_speed_e *speed)
{
	int i, j, pp_prox;

	i=port; pp_prox=PASS;
	if (tdm_tbl[i]<129 && tdm_tbl[i]>0) {
		for (j=1; j<4; j++) {
			if (tdm_th2_legacy_which_tsc(tdm_tbl[i-1-j],tsc)==tdm_th2_legacy_which_tsc(tdm_tbl[i],tsc)) {
				pp_prox=FAIL;
				break;
			}
		}
	}
/* #ifdef _LLS_SCHEDULER */
	if (speed[tdm_tbl[i]]<=SPEED_42G_HG2) {
		if (i>=11) {
			for (j=1; j<11; j++) {
				if (tdm_tbl[i-j]==tdm_tbl[i]) {
					pp_prox=FAIL;
					break;
				}
			}
		}
	}
/* #endif */
	if (pp_prox) {
		TDM_PRINT1("Filter applied: (1) Local Slice, Up, index %0d\n",port);
		tdm_tbl[i-1]=tdm_tbl[i];
		tdm_tbl[i]=TH2_OVSB_TOKEN;
	}

}


/**
@name: tdm_th2_filter_ancillary_smooth
@param:
 */
void
tdm_th2_filter_ancillary_smooth(int port, int *tdm_tbl, int lr_idx_limit, int accessories)
{
	int i, j, pool=0, smallest_slice_idx=0, slice_limit=1, timeout=32000;
	
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==TH2_ANCL_TOKEN) {
			for (j=i; j<(lr_idx_limit+accessories); j++) {
				tdm_tbl[j]=tdm_tbl[j+1];
			}
			i--;
			pool++;
		}
	}
	while ( (pool>0) && ((--timeout)>0) ) {
		for (i=0; i<lr_idx_limit; i++) {
			if (pool<1) {
				return;
			}
			TH2_TOKEN_CHECK(tdm_tbl[i]) {
				if ( (tdm_th2_slice_size_local(i,tdm_tbl,lr_idx_limit)<=slice_limit) && ( (tdm_th2_check_same_port_dist_dn_port(TH2_ANCL_TOKEN,i,tdm_tbl,lr_idx_limit)>11) ) && ( (tdm_th2_check_same_port_dist_up_port(TH2_ANCL_TOKEN,i,tdm_tbl,lr_idx_limit)>11) ) ) {
					smallest_slice_idx=i;
					for (j=(lr_idx_limit+accessories); j>smallest_slice_idx; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					TDM_PRINT1("Filter applied: Ancillary smoothing at index %0d\n",smallest_slice_idx);
					tdm_tbl[smallest_slice_idx]=TH2_ANCL_TOKEN;
					pool--;
				}
			}
		}
		slice_limit++;
	}	
}


/**
@name: tdm_th2_filter_slice_dn
@param:
 */
int
tdm_th2_filter_slice_dn(int port, int *tdm_tbl, int lr_idx_limit, int **tsc, enum port_speed_e *speed)
{
	int i, j, k, slice_translation_done;
	
	i=port; slice_translation_done=BOOL_FALSE;
	for (j=3; (i+j)<(lr_idx_limit-1); j++) {
		if (tdm_tbl[i+j]==TH2_OVSB_TOKEN && 
			tdm_tbl[i+1+j]==TH2_OVSB_TOKEN &&
			(tdm_tbl[i+j-1]==TH2_OVSB_TOKEN || (tdm_th2_legacy_which_tsc(tdm_tbl[i+j-1],tsc)!=tdm_th2_legacy_which_tsc(tdm_tbl[i+j+3],tsc))) &&
			(tdm_tbl[i+j-2]==TH2_OVSB_TOKEN || (tdm_th2_legacy_which_tsc(tdm_tbl[i+j-2],tsc)!=tdm_th2_legacy_which_tsc(tdm_tbl[i+j+2],tsc)))) {
			for (k=(i+j); k>i; k--) {
				tdm_tbl[k]=tdm_tbl[k-1];
				slice_translation_done=BOOL_TRUE;
			}
			tdm_tbl[i]=TH2_OVSB_TOKEN;
		}
		if (slice_translation_done) {
			TDM_PRINT1("Filter applied: Slice translation, Down, index %0d\n",port);
			i++;
			break;
		}
	}
	
	return i;
}


/**
@name: tdm_th2_filter_slice_up
@param:
 */
int
tdm_th2_filter_slice_up(int port, int *tdm_tbl, int lr_idx_limit, int **tsc, enum port_speed_e *speed)
{
	int i, j, k, slice_translation_done;
	
	i=port; slice_translation_done=BOOL_FALSE;
	for (j=3; j<(lr_idx_limit-1); j++) {
		if ((i-j-3)>=0) {
			if (tdm_tbl[i-j]==TH2_OVSB_TOKEN && 
				tdm_tbl[i-1-j]==TH2_OVSB_TOKEN &&
				(tdm_tbl[i-j+1]==TH2_OVSB_TOKEN || (tdm_th2_legacy_which_tsc(tdm_tbl[i-j+1],tsc)!=tdm_th2_legacy_which_tsc(tdm_tbl[i-j-3],tsc))) &&
				(tdm_tbl[i-j+2]==TH2_OVSB_TOKEN || (tdm_th2_legacy_which_tsc(tdm_tbl[i-j+2],tsc)!=tdm_th2_legacy_which_tsc(tdm_tbl[i-j-2],tsc))) &&
				(tdm_th2_check_lls_flat_up(i,tdm_tbl,speed))) {
				for (k=(i-j); k<i; k++) {
					tdm_tbl[k]=tdm_tbl[k+1];
					slice_translation_done=BOOL_TRUE;
				}
				tdm_tbl[i]=TH2_OVSB_TOKEN;
			}
			if (slice_translation_done) {
				TDM_PRINT1("Filter applied: Slice translation, Up, index %0d\n",port);
				i--;
				break;
			}
		}
	}
	
	return i;
}


/**
@name: tdm_th2_filter_dither
@param:
 */
void
tdm_th2_filter_dither(int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc, int threshold, enum port_speed_e *speed)
{
	int g, i, j, k=1, l=TH2_NUM_EXT_PORTS;
	unsigned short dither_shift_done=BOOL_FALSE, dither_done=BOOL_FALSE, dither_slice_counter=0;

	dither_shift_done=BOOL_FALSE;
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==TH2_OVSB_TOKEN && tdm_th2_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))==tdm_th2_slice_size(TH2_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))) {
			g=i; while( (tdm_tbl[g]==TH2_OVSB_TOKEN) && (g<(lr_idx_limit+accessories)) ) {g++;}
			if ( tdm_th2_slice_prox_up(g,tdm_tbl,tsc,speed) &&
				 tdm_th2_slice_prox_dn((tdm_th2_slice_idx(TH2_OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))-1),tdm_tbl,(lr_idx_limit+accessories),tsc,speed) ) {
				l=tdm_tbl[i];
				for (j=i; j<255; j++) {
					tdm_tbl[j]=tdm_tbl[j+1];
				}
				k=i; dither_shift_done=BOOL_TRUE;
				break;
			}
		}
	}
	dither_done=BOOL_FALSE;
	if (dither_shift_done) {
		for (i=1; i<(lr_idx_limit+accessories); i++) {
			dither_slice_counter=0;
			while (tdm_tbl[i]!=TH2_OVSB_TOKEN && tdm_th2_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))==tdm_th2_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && i<(lr_idx_limit+accessories) ) {
				if (++dither_slice_counter>=threshold && tdm_tbl[i-1]==TH2_OVSB_TOKEN) {
					for (j=255; j>i; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[i+1]=TH2_OVSB_TOKEN;
					dither_done=BOOL_TRUE;
					break;
				}
				i++; if (tdm_tbl[i]==TH2_OVSB_TOKEN) {i++;}
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
			TDM_PRINT0("Filter applied: Dither (quantization correction)\n");
		}
	}
}


/**
@name: tdm_th2_filter_fine_dither
@param:
 */
int
tdm_th2_filter_fine_dither(int port, int *tdm_tbl, int lr_idx_limit, int accessories, int **tsc)
{
	int i, j, k, min_prox=11;
	unsigned short fine_dither_done;

	i=port; fine_dither_done=BOOL_FALSE;
	for (j=2; j<(lr_idx_limit+accessories-4); j++) {
		if ( tdm_tbl[j]!=TH2_OVSB_TOKEN && tdm_tbl[j-2]!=TH2_OVSB_TOKEN && tdm_tbl[j+2]!=TH2_OVSB_TOKEN && tdm_tbl[j+4]!=TH2_OVSB_TOKEN && 
			 tdm_th2_slice_size_local((j-2), tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 tdm_th2_slice_size_local(j, tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 tdm_th2_slice_size_local((j+2), tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 tdm_th2_slice_size_local((j+4), tdm_tbl, (lr_idx_limit+accessories))==1 &&
			 tdm_th2_slice_prox_local(j, tdm_tbl, (lr_idx_limit+accessories), tsc)>min_prox )
		{
			for (k=(j+1); k<(lr_idx_limit+accessories); k++) {
				tdm_tbl[k]=tdm_tbl[k+1];
			}
			fine_dither_done=BOOL_TRUE;
			break;
		}
	}
	if (fine_dither_done) {
		TDM_PRINT1("Filter applied: Fine dithering (normal), index %0d\n",port);
		for (j=255; j>i; j--) {
			tdm_tbl[j]=tdm_tbl[j-1];
		}
		tdm_tbl[i]=TH2_OVSB_TOKEN;
		
		return PASS;
	}
	else {
		return FAIL;
	}
}


int
tdm_th2_filter_chain( tdm_mod_t *_tdm )
{
	int g, i, j, k, l,
		timeout=TIMEOUT,
		vec_trans_port_list[ARR_FIXED_LEN],
		last_smooth, vec_filter_result, vec_filter_abort, vec_filter_port, vec_filter_wc,
		filter_slice_size=0, filter_slice_start, filter_slice_end=0, filter_slice_wc_0, filter_slice_wc_1, filter_slice_found, filter_slice_dist,
		local_clump_wc, wpm0, wpm1, wpm2,
		*tdm_pipe_main;
		
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	for (j=0; j<ARR_FIXED_LEN; j++) {
		vec_trans_port_list[j]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	}
	if (_tdm->_core_data.vars_pkg.lr_enable && _tdm->_core_data.vars_pkg.os_enable && !(_tdm->_core_data.vars_pkg.refactor_done)) {
		for (i=1; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
			TH2_TOKEN_CHECK(tdm_pipe_main[i]) {
				if (tdm_arr_exists(tdm_pipe_main[i],ARR_FIXED_LEN,vec_trans_port_list)) {
					continue;
				}
				else {
					vec_filter_port=tdm_pipe_main[i];
					TDM_PUSH(vec_filter_port,TDM_CORE_EXEC__PM_SCAN,vec_filter_wc);
					if (tdm_arr_append(vec_filter_port,ARR_FIXED_LEN,vec_trans_port_list,_tdm->_chip_data.soc_pkg.num_ext_ports)==BOOL_TRUE) {
						last_smooth = tdm_th2_check_fit_smooth(tdm_pipe_main,vec_filter_port,_tdm->_chip_data.soc_pkg.lr_idx_limit,2);
						if (tdm_th2_filter_vector_shift(tdm_pipe_main,vec_filter_port,DN)) {
							vec_filter_abort=BOOL_FALSE;
							for (j=0; j<_tdm->_chip_data.soc_pkg.lr_idx_limit; j++) {
								if (tdm_pipe_main[j]==vec_filter_port) {	
									TDM_PUSH(tdm_pipe_main[j+1],TDM_CORE_EXEC__PM_SCAN,wpm0);
									TDM_PUSH(tdm_pipe_main[j+2],TDM_CORE_EXEC__PM_SCAN,wpm1);
									TDM_PUSH(tdm_pipe_main[j+3],TDM_CORE_EXEC__PM_SCAN,wpm2);
									if ( (wpm0==vec_filter_wc) || (wpm1==vec_filter_wc) || (wpm2==vec_filter_wc) ) {
										vec_filter_abort=BOOL_TRUE;
										break;
									}
								}
							}
							if (tdm_th2_check_fit_smooth(tdm_pipe_main,vec_filter_port,_tdm->_chip_data.soc_pkg.lr_idx_limit,2)>=last_smooth) {
								vec_filter_abort=BOOL_TRUE;
							}
							if (vec_filter_abort) {
								vec_filter_result=tdm_th2_filter_vector_shift(tdm_pipe_main,vec_filter_port,UP);
								TDM_PRINT3("Filter failed: Vector re-shift up index %0d, port %0d, result %0d\n",i,vec_filter_port,vec_filter_result);
							}
							else {
								TDM_PRINT2("Filter applied: Vector translation down index %0d, port %0d\n",i,vec_filter_port);
								i++;
							}
						}
					}
				}
			}
		}
		for (i=0; i<ARR_FIXED_LEN; i++) {vec_trans_port_list[i]=_tdm->_chip_data.soc_pkg.num_ext_ports;}
		for (i=4; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
			TH2_TOKEN_CHECK(tdm_pipe_main[i]) {
				if (tdm_arr_exists(tdm_pipe_main[i],ARR_FIXED_LEN,vec_trans_port_list)) {
					continue;
				}
				else {
					vec_filter_port=tdm_pipe_main[i];
					TDM_PUSH(vec_filter_port,TDM_CORE_EXEC__PM_SCAN,vec_filter_wc);
					if (tdm_arr_append(vec_filter_port,ARR_FIXED_LEN,vec_trans_port_list,_tdm->_chip_data.soc_pkg.num_ext_ports)==BOOL_TRUE) {
						last_smooth = tdm_th2_check_fit_smooth(tdm_pipe_main,vec_filter_port,_tdm->_chip_data.soc_pkg.lr_idx_limit,2);
						if (tdm_th2_filter_vector_shift(tdm_pipe_main,vec_filter_port,UP)) {
							vec_filter_abort=BOOL_FALSE;
							for (j=4; j<_tdm->_chip_data.soc_pkg.lr_idx_limit; j++) {
								if (tdm_pipe_main[j]==vec_filter_port) {
									TDM_PUSH(tdm_pipe_main[j-1],TDM_CORE_EXEC__PM_SCAN,wpm0);
									TDM_PUSH(tdm_pipe_main[j-2],TDM_CORE_EXEC__PM_SCAN,wpm1);
									TDM_PUSH(tdm_pipe_main[j-3],TDM_CORE_EXEC__PM_SCAN,wpm2);
									if ( (wpm0==vec_filter_wc) || (wpm1==vec_filter_wc) || (wpm2==vec_filter_wc) ) {
										vec_filter_abort=BOOL_TRUE;
										break;
									}
								}
							}
							if (tdm_th2_check_fit_smooth(tdm_pipe_main,vec_filter_port,_tdm->_chip_data.soc_pkg.lr_idx_limit,2)>=last_smooth) {
								vec_filter_abort=BOOL_TRUE;
							}
							if (vec_filter_abort) {
								vec_filter_result=tdm_th2_filter_vector_shift(tdm_pipe_main,vec_filter_port,DN);
								TDM_PRINT3("Filter failed: Vector re-shift down index %0d, port %0d, result %0d\n",i,vec_filter_port,vec_filter_result);
							}
							else {
								TDM_PRINT2("Filter applied: Vector translation up index %0d, port %0d\n",i,vec_filter_port);
							}
						}
					}
				}					
			}
		}
		for (i=1; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit-4); i++) {
			if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+2]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
				tdm_th2_filter_local_slice_dn(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.speed);
			}
		}
		for (i=4; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit-1); i++) {
			if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+1]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-2]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
				tdm_th2_filter_local_slice_up(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.speed);
			}
		}
		if (((_tdm->_core_data.vars_pkg.cap+_tdm->_chip_data.soc_pkg.tvec_size)/tdm_th2_num_lr_slots(tdm_pipe_main))<3) {
			tdm_th2_filter_ancillary_smooth(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size);
			for (i=0; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); i++) {
				TH2_TOKEN_CHECK(tdm_pipe_main[i]) {
					if ( (_tdm->_chip_data.soc_pkg.speed[tdm_pipe_main[i]]<=(SPEED_40G+SPEED_DELIMITER)) && (_tdm->_chip_data.soc_pkg.speed[tdm_pipe_main[i]]>=(SPEED_40G-SPEED_DELIMITER)) ) {
						if (tdm_th2_check_same_port_dist_dn(i,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))<_tdm->_core_data.rule__same_port_min) {
							TDM_PRINT1("Filter failed: Ancillary smoothing due to index %0d\n",i);
							for (j=0; j<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); j++) {
								if (tdm_pipe_main[j]==_tdm->_chip_data.soc_pkg.soc_vars.ancl_token) {
									for (k=j; k<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); k++) {
										tdm_pipe_main[k]=tdm_pipe_main[k+1];
									}
								}
							}
							for (j=1; j<=_tdm->_chip_data.soc_pkg.tvec_size; j++) {
								g=tdm_PQ((((10*_tdm->_core_data.vars_pkg.cap)/_tdm->_chip_data.soc_pkg.tvec_size)*j))+(j-1);
								for (l=255; l>g; l--) {
									tdm_pipe_main[l]=tdm_pipe_main[l-1];
								}
								tdm_pipe_main[g]=_tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
							}
							break;
						}
					}
				}
			}
		}
		i=0;
		while (i<_tdm->_chip_data.soc_pkg.lr_idx_limit) {
			while (i<_tdm->_chip_data.soc_pkg.lr_idx_limit) {i++; if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {break;}}
			filter_slice_size=tdm_th2_slice_size_local(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit);
			if (filter_slice_size>3) {
				filter_slice_start=i;
				filter_slice_found=FAIL;
				for (j=4; j<_tdm->_chip_data.soc_pkg.lr_idx_limit; j++) {
					if (tdm_pipe_main[i+j]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+j+1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
						filter_slice_end=(i+j-1);
						filter_slice_found=PASS;
						TDM_PUSH(tdm_pipe_main[filter_slice_end],TDM_CORE_EXEC__PM_SCAN,filter_slice_wc_0);
						TDM_PUSH(tdm_pipe_main[filter_slice_end-1],TDM_CORE_EXEC__PM_SCAN,filter_slice_wc_1);
						TDM_PUSH(tdm_pipe_main[filter_slice_end+3],TDM_CORE_EXEC__PM_SCAN,wpm0);
						TDM_PUSH(tdm_pipe_main[filter_slice_end+4],TDM_CORE_EXEC__PM_SCAN,wpm1);
						if ( (wpm0==filter_slice_wc_0) || (wpm1==filter_slice_wc_0) || (wpm0==filter_slice_wc_1) ) {
							filter_slice_found=FAIL;
						}
						for (k=filter_slice_end; k>=i; k--) {
							TH2_TOKEN_CHECK(tdm_pipe_main[k]) {
								if (tdm_pipe_main[k]<=SPEED_42G_HG2) {
									filter_slice_dist=tdm_th2_check_same_port_dist_dn(k,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size));
									if ( (filter_slice_dist<=_tdm->_core_data.rule__same_port_min) && ((k+filter_slice_dist)>filter_slice_end) ) {
										filter_slice_found=FAIL;
										break;
									}
								}
							}
						}
						if ((i+j)>=(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size)) {
							filter_slice_found=FAIL;
							break;
						}
						if (filter_slice_found) {
							break;
						}
					}
				}
				if (filter_slice_found) {
					TDM_PRINT3("Filter applied: Port slice translation - Size: %0d | Begin: %0d | End: %0d\n",filter_slice_size,filter_slice_start,filter_slice_end);
					for (j=(filter_slice_end+1); j>=(filter_slice_start+(filter_slice_size/2)); j--) {
						tdm_pipe_main[j]=tdm_pipe_main[j-1];
					}
					tdm_pipe_main[filter_slice_start+(filter_slice_size/2)]=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
				}
			}
		}
		for (i=1; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); i++) {
			if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+2]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
				if ((tdm_pipe_main[i]>SPEED_42G_HG2) || (tdm_th2_check_same_port_dist_dn(i,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>_tdm->_core_data.rule__same_port_min)) {
					TDM_PUSH(tdm_pipe_main[i],TDM_CORE_EXEC__PM_SCAN,local_clump_wc);
					TDM_PUSH(tdm_pipe_main[i+3],TDM_CORE_EXEC__PM_SCAN,wpm0);
					TDM_PUSH(tdm_pipe_main[i+4],TDM_CORE_EXEC__PM_SCAN,wpm1);
					if ((wpm0!=local_clump_wc) && (wpm1!=local_clump_wc)) {
						tdm_pipe_main[i+1]=tdm_pipe_main[i];
						tdm_pipe_main[i]=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
						TDM_PRINT1("Filter applied: (2) Local slice oversub index %0d\n",i);
					}
				}
			}
		}
		for (i=(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size-1); i>=4; i--) {
			if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+1]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-2]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
				if ((tdm_pipe_main[i]>SPEED_42G_HG2) || (tdm_th2_check_same_port_dist_up(i,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>_tdm->_core_data.rule__same_port_min)) {
					_tdm->_core_data.vars_pkg.port=tdm_pipe_main[i];
					local_clump_wc=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					TDM_PUSH(tdm_pipe_main[i-3],TDM_CORE_EXEC__PM_SCAN,wpm0);
					TDM_PUSH(tdm_pipe_main[i-4],TDM_CORE_EXEC__PM_SCAN,wpm1);
					if ((wpm0!=local_clump_wc) && (wpm1!=local_clump_wc)) {
						tdm_pipe_main[i-1]=tdm_pipe_main[i];
						tdm_pipe_main[i]=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
						TDM_PRINT1("Filter applied: (2) Local slice oversub index %0d\n",i);
					}
				}
			}
		}
/* #ifdef _LLS_SCHEDULER */
		if ( (_tdm->_chip_data.soc_pkg.clk_freq<760) || (_tdm->_chip_data.soc_pkg.clk_freq>=760 && _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g==0) ) {
/* #endif */
			for (i=1; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit-4); i++) {
				if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+2]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
					i=tdm_th2_filter_slice_dn(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.speed);
				}
			}
			for (i=4; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit-1); i++) {
				if (tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+1]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-1]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i-2]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
					i=tdm_th2_filter_slice_up(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.speed);
				}
			}
/* #ifdef _LLS_SCHEDULER */
		}
/* #endif */
		timeout=DITHER_PASS;
		while ( tdm_th2_slice_size(_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>1 && tdm_th2_slice_size(_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>=tdm_th2_slice_size(1,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size)) && ((--timeout)>0) ) {
			tdm_th2_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,_tdm->_chip_data.soc_pkg.pmap,DITHER_THRESHOLD,_tdm->_chip_data.soc_pkg.speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_th2_slice_size(_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>1 && tdm_th2_slice_size(_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>=tdm_th2_slice_size(1,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size)) && ((--timeout)>0) ) {
			tdm_th2_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,_tdm->_chip_data.soc_pkg.pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE),_tdm->_chip_data.soc_pkg.speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_th2_slice_size(_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>1 && tdm_th2_slice_size(_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))>=tdm_th2_slice_size(1,tdm_pipe_main,(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size)) && ((--timeout)>0) ) {
			tdm_th2_filter_dither(tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,_tdm->_chip_data.soc_pkg.pmap,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE-DITHER_SUBPASS_STEP_SIZE),_tdm->_chip_data.soc_pkg.speed);
		}
		if (tdm_th2_slice_size(1, tdm_pipe_main, (_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size))==2) {
			for (i=3; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); i++) {
				if (tdm_pipe_main[i-3]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+3]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+6]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_th2_slice_size_local((i-3), tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2 && tdm_th2_slice_size_local(i, tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2 && tdm_th2_slice_size_local((i+3), tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2 && tdm_th2_slice_size_local((i+6), tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2) {
					if (tdm_th2_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,_tdm->_chip_data.soc_pkg.pmap)==FAIL) {
						break;
					}
				}
			}
			for (i=3; i<(_tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size); i++) {
				if (tdm_pipe_main[i-3]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_pipe_main[i+3]!=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token && tdm_th2_slice_size_local((i-3), tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2 && tdm_th2_slice_size_local(i, tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2 && tdm_th2_slice_size_local((i+3), tdm_pipe_main, _tdm->_chip_data.soc_pkg.lr_idx_limit)==2) {
					if (tdm_th2_filter_fine_dither(i,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.tvec_size,_tdm->_chip_data.soc_pkg.pmap)==FAIL) {
						break;
					}
				}
			}
		}
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__PARSE]( _tdm ) );
}
