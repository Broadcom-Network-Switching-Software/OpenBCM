/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
	#include <tdm_vec.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/tdm/core/tdm_vec.h>
#endif


/**
@name: tdm_core_prealloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to presort ports by speed
 */
void
tdm_core_prealloc(unsigned short stack[TDM_AUX_SIZE], int buffer[TDM_AUX_SIZE], short *x, char *boolstr, int j)
{
	int i;

	*boolstr = BOOL_TRUE;
	(*x)++;
	stack[*x] = buffer[j];
	for (i=0; i<(*x); i++) {
		if (stack[i] == buffer[j]) {
			(*x)--;
		}
	}
}


/**
@name: tdm_core_postalloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to load time vectors into TDM vector map
 */
int
tdm_core_postalloc(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports)
{
	int v, load_status;
	
	TDM_BIG_BAR
	TDM_PRINT1("%sG scheduling pass\n",speed);
	TDM_SML_BAR
	TDM_PRINT0("Stack contains: [ ");
	for (v=*y; v>0; v--) {
		if (lr_stack[v]!=num_ext_ports) {
			TDM_PRINT1(" %0d ",lr_stack[v]);
		}
	}
	TDM_PRINT0(" ]\n");
	TDM_SML_BAR
	while (*y > 0) {
		load_status = tdm_core_vec_load(vector_map, freq, spd, *yy, lr_idx_limit, num_ext_ports);
		if (load_status==FAIL) {
			TDM_ERROR2("Failed to load %sG vector for port %0d\n",speed,lr_stack[*y]);
			return FAIL;
		}
		else {
			for (v=0; v<VEC_MAP_LEN; v++) {
				if (vector_map[*yy][v]==token) {
					vector_map[*yy][v]=lr_stack[*y];
				}
			}
			TDM_PRINT4("Loaded %sG port %d vector from stack pointer %d into map at index %0d\n",speed, lr_stack[*y], *y, *yy);
		}
		(*y)--; (*yy)++;
		if ((*y)>0) {
			TDM_SML_BAR
			TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", *yy, *y);
			TDM_SML_BAR
		}
		else {
			TDM_SML_BAR
			TDM_PRINT2("Done loading %sG vectors, map pointer floating at: %0d\n",speed, *yy);
			TDM_SML_BAR
		}
	}
	TDM_BIG_BAR
	
	return PASS;
	
}


/**
@name: tdm_core_postalloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to load time vectors into TDM vector map
 */
int
tdm_core_postalloc_vmap(unsigned short **vector_map, int freq, unsigned short spd, short *yy, short *y, int lr_idx_limit, unsigned short lr_stack[TDM_AUX_SIZE], int token, const char* speed, int num_ext_ports, int vmap_wid, int vmap_len)
{
	int v, load_status;
	
	TDM_BIG_BAR
	TDM_PRINT1("%sG scheduling pass\n",speed);
	TDM_SML_BAR
	TDM_PRINT0("Stack contains: [ ");
	for (v=*y; v>0; v--) {
		if (lr_stack[v]!=num_ext_ports) {
			TDM_PRINT1(" %0d ",lr_stack[v]);
		}
	}
	TDM_PRINT0(" ]\n");
	TDM_SML_BAR
	while ( ((*y)>0) && ((*yy)<vmap_wid) ){
		load_status = tdm_core_vec_load(vector_map, freq, spd, *yy, lr_idx_limit, num_ext_ports);
		if (load_status==FAIL) {
			TDM_ERROR2("Failed to load %sG vector for port %0d\n",speed,lr_stack[*y]);
			return FAIL;
			return FAIL;
		}
		else {
			for (v=0; v<vmap_len; v++) {
				if (vector_map[*yy][v]==token) {
					vector_map[*yy][v]=lr_stack[*y];
				}
			}
			TDM_PRINT4("Loaded %sG port %d vector from stack pointer %d into map at index %0d\n",speed, lr_stack[*y], *y, *yy);
		}
		(*y)--; (*yy)++;
		if ((*y)>0) {
			TDM_SML_BAR
			TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", *yy, *y);
			TDM_SML_BAR
		}
		else {
			TDM_SML_BAR
			TDM_PRINT2("Done loading %sG vectors, map pointer floating at: %0d\n",speed, *yy);
			TDM_SML_BAR
		}
	}
	TDM_BIG_BAR
	
	return PASS;
	
}


/**
@name: tdm_slots
@param:

Calculates number of slots required for a specific port
 */
int
tdm_slots(int port_speed)
{
	return ( (int)( (port_speed*10)/BW_QUANTA ) );
}


/**
@name: tdm_arr_exists
@param:

Returns boolean on whether an element exists in a fixed length array
 */
int
tdm_arr_exists(int element, int len, int arr[ARR_FIXED_LEN])
{
	int i, result=FAIL;
	
	for (i=0; i<len && i<ARR_FIXED_LEN; i++) {
		if (arr[i]==element) {
			result=PASS;
			break;
		}
	}
	
	return result;
}


/**
@name: tdm_arr_append
@param:

Appends to array without needing to track an index
Array must be initialized to NUM_EXT_PORTS
 */
/* int tdm_arr_append(int element, int len, int arr[len]) { */
/* C90 not supported */
int
tdm_arr_append(int element, int len, int arr[ARR_FIXED_LEN], int num_ext_ports)
{
	int i, done=BOOL_FALSE, idx=0;
	
	for (i=0; i<len && i<ARR_FIXED_LEN; i++) {
		if (arr[i]==num_ext_ports) {
			idx=i;
			done=BOOL_TRUE;
			break;
		}
	}
	if (done) {
		arr[idx]=element;
	}
	
	return done;
}


/**
@name: tdm_core_acc_alloc
@param:
 */
int
tdm_core_acc_alloc( tdm_mod_t *_tdm )
{
	int g, j, l,
		*tdm_pipe_main;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	for (j=1; j<=_tdm->_chip_data.soc_pkg.tvec_size; j++) {
		g=tdm_PQ((((10*_tdm->_core_data.vars_pkg.cap)/_tdm->_chip_data.soc_pkg.tvec_size)*j))+(j-1);
		for (l=(_tdm->_core_data.vmap_max_len-1); l>g; l--) {
			tdm_pipe_main[l]=tdm_pipe_main[l-1];
		}
		tdm_pipe_main[g]=_tdm->_chip_data.soc_pkg.soc_vars.ancl_token;
	}
	
	return PASS;
}


/**
@name: tdm_core_vbs_scheduler
@param:

Allocate slots for linerate ports within the linerate capacity limit
 */
int
tdm_core_vbs_scheduler( tdm_mod_t *_tdm )
{
	unsigned short dim_reduce;
	int i=0, j, k, singular_cnt=0,
		timeout=TIMEOUT, lr_slot_cnt=0, pspd, pspd1, fpnode,
		tsc_pipeline[ARR_FIXED_LEN], xx=0, x0=0, triport_op=BOOL_FALSE, vec_id, ns_min_cnt, 
		rotation_step=DEFAULT_STEP_SIZE, main_pass_done, main_pass_timeout,
		cadence_cnt=0, cadence_step_size, cadence_ref_idx, redis_100g_cnt, redis_100g_port,
		cadence_step, cadence_step_limit, cadence_blocks,
		cap_tsc=_tdm->_chip_data.soc_pkg.num_ext_ports, cap_lpbk=0, cap_lkbk=0,
		*tdm_pipe_main;
	struct node tdm_ll;
	int tsc_i, tsc_j_prev, tsc_j_curr, tsc_j_next, check_transpose, exist_transpose, src_idx, dst_idx, 
	    param_spd_i, param_spd_j, param_spd_ii, same_spd_num;
    char vmap_bmp[VEC_MAP_LEN];
    int cal_len, idx_up, idx_dn;
    
	tdm_ll.port=0;
	tdm_ll.next=NULL;
	_tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
	_tdm->_core_data.vars_pkg.cap = 0;
	for (i=0; i<VEC_MAP_LEN; i++){vmap_bmp[i]=1;}
    
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	TDM_PRINT1("Length of TDM round standardized to slot quanta set to %d\n",_tdm->_chip_data.soc_pkg.lr_idx_limit);
	for (j=0; j<ARR_FIXED_LEN; j++) {
		tsc_pipeline[j]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt=0;
	if (_tdm->_core_data.vars_pkg.lr_enable) {
		for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
			if (_tdm->_core_data.vmap[i][0]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt++;
			}
		}
	}
	for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
		if (tdm_type_chk(_tdm)==3) {
			triport_op=BOOL_TRUE;
			TDM_PRINT0("Triport mode TRUE\n");
			break;
		}
	}
	for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		lr_slot_cnt+=tdm_slots(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]/1000);
	}
	for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
		if ( !(tdm_arr_exists(_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm),ARR_FIXED_LEN,tsc_pipeline)) ) {
			tsc_pipeline[xx++]=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
		}
	}
	
	if (_tdm->_core_data.vars_pkg.lr_enable) {
		if (!triport_op) {
			tdm_vector_dump(_tdm);
			for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
				if ( (tdm_type_chk(_tdm)>1) && (tdm_count_param_spd(_tdm)>=10) ) {
					timeout=TIMEOUT;
					do {
						x0=(x0>=xx)?(0):(x0);
						_tdm->_core_data.vars_pkg.m_tdm_pick_vec.tsc_dq=tsc_pipeline[x0];
						_tdm->_core_data.vars_pkg.m_tdm_pick_vec.prev_vec=i;
						vec_id=_tdm->_core_exec[TDM_CORE_EXEC__PICK_VEC](_tdm);
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[vec_id][0];
						if ( (tdm_find_fastest_spd(_tdm)<=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]-SPEED_DELIMITER)) ||
							 (tdm_find_fastest_spd(_tdm)>=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]+SPEED_DELIMITER)) ) {
							x0++;
						}
					} while ( /*(vec_id<=i) && */
							  ( (tdm_find_fastest_spd(_tdm)<=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]-SPEED_DELIMITER)) ||
								(tdm_find_fastest_spd(_tdm)>=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]+SPEED_DELIMITER)) ) &&
								((--timeout)>0) );
					for (j=i; j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
						if (_tdm->_core_data.vmap[j][0]==tdm_find_fastest_port(_tdm)) {
							vec_id=j;
							break;
						}
					}
					if (vec_id!=i) {
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=i;
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=(_tdm->_core_data.vmap_max_wid-1);
						tdm_mem_transpose(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=vec_id;
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=i;
						tdm_mem_transpose(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=(_tdm->_core_data.vmap_max_wid-1);
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=vec_id;
						tdm_mem_transpose(_tdm);
					}
				}
				if ((x0++)>=xx) {x0=0;}
			}
		}
		/* MODIFIED: new added */
		else{
			param_spd_ii = 0;
			_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
			for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				same_spd_num = 0;
				_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
				param_spd_i = tdm_count_param_spd(_tdm);
				if (param_spd_i<10) {break;}
				if (param_spd_i!=param_spd_ii){
					param_spd_ii = param_spd_i;
					same_spd_num = 1;
					for (j=(i+1); j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[j][0];
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
						param_spd_j = tdm_count_param_spd(_tdm);
						if (param_spd_i==param_spd_j){same_spd_num++;}
						else {break;}
					}
				}
				if (same_spd_num>2){
					_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
					tsc_i = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					for (k=0; k<same_spd_num; k++) {
						exist_transpose = BOOL_FALSE;
						tsc_j_prev = _tdm->_chip_data.soc_pkg.pmap_num_modules;
						tsc_j_curr = tsc_i;
						tsc_j_next = _tdm->_chip_data.soc_pkg.pmap_num_modules;
						for (j=(i+1); ( (j<(i+same_spd_num)) && (j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt) ); j++) {
							check_transpose = FAIL;
							_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[j][0];
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
							tsc_j_next = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
							if (tsc_j_curr==tsc_j_prev && tsc_j_curr!=tsc_j_next && tsc_j_next!=_tdm->_chip_data.soc_pkg.pmap_num_modules){
								src_idx = j;
								dst_idx = j-1;
								check_transpose = PASS;
								exist_transpose = BOOL_TRUE;
							}
							else if (tsc_j_curr!=tsc_j_prev && tsc_j_curr==tsc_j_next && tsc_j_prev!=_tdm->_chip_data.soc_pkg.pmap_num_modules){
								src_idx = j-1;
								dst_idx = j-2;
								if (dst_idx <0) {break;}
								check_transpose = PASS;
								exist_transpose = BOOL_TRUE;
							}
							if (check_transpose == PASS){
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=dst_idx;
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=(_tdm->_core_data.vmap_max_wid-1);
								tdm_mem_transpose(_tdm);
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=src_idx;
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=dst_idx;
								tdm_mem_transpose(_tdm);
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=(_tdm->_core_data.vmap_max_wid-1);
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=src_idx;
								tdm_mem_transpose(_tdm);
							}
							tsc_j_prev = tsc_j_curr;
							tsc_j_curr = tsc_j_next;
						}
						if (exist_transpose==BOOL_FALSE) {break;}
					}
				}
				
			}
		}
		tdm_vector_dump(_tdm);
		if (_tdm->_core_data.vars_pkg.os_enable) {
			for (i=1; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
				pspd=tdm_count_param_spd(_tdm);
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=(i-1);
				if ( ((pspd!=tdm_count_param_spd(_tdm))) ) {
					_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_TRUE;
					_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=(i-1);
					pspd=tdm_count_param_spd(_tdm);
					_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
					pspd1 = tdm_count_param_spd(_tdm);
					if (pspd1>0 && pspd%pspd1==0) {
						cadence_blocks=1;
						cadence_ref_idx=i-1;
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=cadence_ref_idx;
						pspd=tdm_count_param_spd(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=(cadence_ref_idx-1);
						while(cadence_ref_idx>0 && (tdm_count_param_spd(_tdm)==tdm_count_param_spd(_tdm)) ) {
							cadence_ref_idx--;
						}
						for (j=0; j<_tdm->_chip_data.soc_pkg.lr_idx_limit && j<_tdm->_core_data.vmap_max_len; j++) {
							if (_tdm->_core_data.vmap[cadence_ref_idx][j]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[i][j]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
								cadence_blocks++;
							}
							if (j>0 && _tdm->_core_data.vmap[cadence_ref_idx][j]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[i][j]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
								break;
							}
						}
						for (j=i; j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
							pspd=tdm_count_param_spd(_tdm);
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
							if ( pspd==tdm_count_param_spd(_tdm) ) {
								cadence_cnt++;
							}
						}
						cadence_step_size=(cadence_cnt/cadence_blocks);
						if (cadence_step_size<1) {cadence_step_size=1;}
						for (j=(i+cadence_step_size); j<(i+cadence_cnt); j+=cadence_step_size) {
							for (k=0; k<cadence_step_size; k++) {
								cadence_step=0; cadence_step_limit=(_tdm->_chip_data.soc_pkg.lr_idx_limit/4);
								do {
									_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=(j+k);
									_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=_tdm->_chip_data.soc_pkg.lr_idx_limit;
									_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=1;
									tdm_vector_rotate(_tdm);
									_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle=0;
									_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx=(j+k);
									if (_tdm->_core_data.vmap[cadence_ref_idx][tdm_map_find_y_indx(_tdm)]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[j+k][tdm_map_find_y_indx(_tdm)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
										cadence_step++;
									}
									if ( cadence_step==((j-i)/cadence_step_size) ) {
										break;
									}
								} while ( (--cadence_step_limit)>0 );
							}
						}
						tdm_vector_dump(_tdm);
					}
				}
			}
		}
		main_pass_timeout=0;
		do {
			for (i=1; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				dim_reduce=FAIL;
				for (k=0; k<_tdm->_chip_data.soc_pkg.lr_idx_limit; k++) {
					_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=i;
					_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=_tdm->_chip_data.soc_pkg.lr_idx_limit;
					_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=rotation_step;
					tdm_vector_rotate(_tdm);
					tdm_vector_dump(_tdm);				
					_tdm->_core_data.vars_pkg.m_tdm_fit_singular_col.node_x=i;
					_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=i;
					_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
					if ( (tdm_fit_singular_col(_tdm)==PASS) && 
						 (tdm_fit_prox(_tdm)==PASS) ) {
						dim_reduce=PASS;
						break;
					}
				}
				if (!dim_reduce) {
					_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx=i;
					ns_min_cnt=tdm_count_nonsingular(_tdm);
					for (k=0; k<_tdm->_chip_data.soc_pkg.lr_idx_limit; k++) {
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=i;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=_tdm->_chip_data.soc_pkg.lr_idx_limit;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=rotation_step;
						tdm_vector_rotate(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx=i;
						_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=i;
						_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
						if ( (tdm_count_nonsingular(_tdm)<ns_min_cnt) &&
							 (tdm_fit_prox(_tdm)==PASS) ) {
							ns_min_cnt=tdm_count_nonsingular(_tdm);
						}
					}
					_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx=i;
					while ( ns_min_cnt!=tdm_count_nonsingular(_tdm) ) {
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=i;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=_tdm->_chip_data.soc_pkg.lr_idx_limit;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=rotation_step;
						tdm_vector_rotate(_tdm);
						tdm_vector_dump(_tdm);
					}
				}
			}
			for (i=1; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt;
				_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
				if (tdm_fit_prox(_tdm)!=PASS) {
					for (j=0; j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
						pspd=tdm_count_param_spd(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
						if (pspd==tdm_count_param_spd(_tdm)) {
							_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src=i;
							_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst=j;
							tdm_sticky_transpose(_tdm);
							_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt;
							_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
							fpnode=tdm_fit_prox(_tdm);
							_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=j;
							if ( (fpnode==PASS) &&
								 (tdm_fit_prox(_tdm)==PASS) ) {
								tdm_vector_dump(_tdm);
								break;
							}
							else {
								_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src=i;
								_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst=j;
								tdm_sticky_transpose(_tdm);
							}
						}
					}
				}
			}
			main_pass_done=BOOL_TRUE;
			if (!(_tdm->_core_data.vars_pkg.lr_1)) {
				for (i=0; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
					_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx=i;
					_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx=i;
					if ( (!tdm_nsin_row(_tdm)) || (!tdm_fit_row_min(_tdm)) ) {
						main_pass_done=BOOL_FALSE;
						break;
					}
				}
			}
		} while (!main_pass_done && (++main_pass_timeout<MAIN_SCHEDULER_PASSES));
	}
	if (_tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB==BOOL_TRUE) {
		redis_100g_cnt=0;
		redis_100g_port=_tdm->_core_data.vmap[0][0];
		_tdm->_core_data.vars_pkg.m_tdm_vector_clear.yy=0;
		tdm_vector_clear(_tdm);
		for (i=0; (i+18)<_tdm->_chip_data.soc_pkg.lr_idx_limit && (i+18)<_tdm->_core_data.vmap_max_len; i+=24) {
			_tdm->_core_data.vmap[0][i] = redis_100g_port;
			_tdm->_core_data.vmap[0][i+4] = redis_100g_port;
			_tdm->_core_data.vmap[0][i+8] = redis_100g_port;
			_tdm->_core_data.vmap[0][i+12] = redis_100g_port;
			_tdm->_core_data.vmap[0][i+18] = redis_100g_port;
		}
		for (j=0; j<_tdm->_chip_data.soc_pkg.lr_idx_limit; j++) {
			_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx=j;
			if (!tdm_nsin_row(_tdm)) {
				_tdm->_core_data.vars_pkg.m_tdm_vector_clear.yy=0;
				tdm_vector_clear(_tdm);
				for (i=0; ( (i<(_tdm->_chip_data.soc_pkg.lr_idx_limit-4)) && ((++redis_100g_cnt)<=40) && i<(_tdm->_core_data.vmap_max_len-4) ); i+=5) {
					if (tdm_empty_row(_tdm->_core_data.vmap,i,_tdm->_chip_data.soc_pkg.num_ext_ports,_tdm->_core_data.vmap_max_wid)) {
						_tdm->_core_data.vmap[0][i]=redis_100g_port;
					}
					else if (i>0 && (tdm_empty_row(_tdm->_core_data.vmap,(i-1),_tdm->_chip_data.soc_pkg.num_ext_ports,_tdm->_core_data.vmap_max_wid)) && (tdm_slice_size_2d(_tdm->_core_data.vmap,(i-1),_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.num_ext_ports,_tdm->_core_data.vmap_max_wid)<=tdm_slice_size_2d(_tdm->_core_data.vmap,(i+1),_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_chip_data.soc_pkg.num_ext_ports,_tdm->_core_data.vmap_max_wid)) ) {
						_tdm->_core_data.vmap[0][i-1]=redis_100g_port;
					}
					else if (tdm_empty_row(_tdm->_core_data.vmap,(i+1),_tdm->_chip_data.soc_pkg.num_ext_ports,_tdm->_core_data.vmap_max_wid)) {
						_tdm->_core_data.vmap[0][i+1]=redis_100g_port;
					}
				}
				break;
			}
		}
	}
	TDM_PRINT0("\n\n");
	TDM_SML_BAR
	TDM_PRINT1("\nSolution vmap pipe %0d:\n\n           ",((_tdm->_core_data.vmap[0][0])/64));
	tdm_print_vmap_vector(_tdm);
	TDM_PRINT0("\n\n\n");
	TDM_SML_BAR
	
	TDM_PRINT0("\n\tConstructing TDM linked list...\n");
	TDM_PRINT5("\t\t| Span=%0d | Length=%0d | Pivot=%0d | Slice Size=%0d | Retrace=%0d |\n",_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt,_tdm->_chip_data.soc_pkg.lr_idx_limit,_tdm->_core_data.vmap[0][0],tdm_map_cadence_count(_tdm->_core_data.vmap[0],0,_tdm->_core_data.vmap_max_len),tdm_map_retrace_count(_tdm->_core_data.vmap,0,0,_tdm->_core_data.vmap_max_len,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports));
	
    cal_len = _tdm->_chip_data.soc_pkg.lr_idx_limit;
	for (i=0; i<cal_len && i<VEC_MAP_LEN && i<_tdm->_core_data.vmap_max_len; i++) {
		singular_cnt = tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
		if (singular_cnt>1) { 
			for (j=0; j<(singular_cnt-1); j++){
				idx_up = -1; idx_dn = -1;
				for (k=(i-1); k>0; k--){
					if (vmap_bmp[k]==1 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
						idx_up = k; break;
					}
				}
				for (k=(i+1); k<cal_len && k<VEC_MAP_LEN && k<_tdm->_core_data.vmap_max_len; k++){
					if (vmap_bmp[k]==1 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
						idx_dn = k; break;
					}
				}
				if ( idx_up!=(-1) || idx_dn!=(-1) ){
					if (idx_up==(-1) || idx_dn==(-1)){
						if (idx_up!=(-1)) {vmap_bmp[idx_up]=0;}
						else              {vmap_bmp[idx_dn]=0;}
					}
					else {
						if (idx_up<=idx_dn){vmap_bmp[idx_up]=0;}
						else               {vmap_bmp[idx_dn]=0;}
					}
				}
			}
		}
	}
	for (i=0; i<cal_len && i<VEC_MAP_LEN && i<_tdm->_core_data.vmap_max_len; i++) {
		if (vmap_bmp[i]){
			singular_cnt = tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
			if (singular_cnt==0){
				tdm_ll_append(&tdm_ll,_tdm->_chip_data.soc_pkg.num_ext_ports);
			}
			else {
				for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
					TOKEN_CHECK(_tdm->_core_data.vmap[j][i]) {
						tdm_ll_append(&tdm_ll,_tdm->_core_data.vmap[j][i]);
					}
				}
			}
		}
	}
    
	for (i=0; i<cal_len && i<VEC_MAP_LEN && i<_tdm->_core_data.vmap_max_len; i++) {
		singular_cnt = tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
		if (singular_cnt==0){
			tdm_ll_append(&tdm_ll,_tdm->_chip_data.soc_pkg.num_ext_ports);
		}
		else if (singular_cnt>0){
			for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
				TOKEN_CHECK(_tdm->_core_data.vmap[j][i]) {
					tdm_ll_append(&tdm_ll,_tdm->_core_data.vmap[j][i]);
					break;
				}
			}
		}
	}
	for (i=0; i<60 && i<_tdm->_core_data.vmap_max_len; i++) {
		for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
			if (tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)>0) {
				TOKEN_CHECK(_tdm->_core_data.vmap[j][i]) {
					tdm_ll_append(&tdm_ll,_tdm->_core_data.vmap[j][i]);
				}
			}
			else {
				tdm_ll_append(&tdm_ll,_tdm->_chip_data.soc_pkg.num_ext_ports);
				break;
			}
		}
	}
	tdm_ll_print(&tdm_ll);
	tdm_ll_deref(&tdm_ll,tdm_pipe_main,_tdm->_chip_data.soc_pkg.lr_idx_limit);
	for (j=_tdm->_chip_data.soc_pkg.lr_idx_limit; j<_tdm->_core_data.vmap_max_len; j++) {
		TOKEN_CHECK(tdm_pipe_main[j]) {
			for (i=1; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
				if (tdm_pipe_main[_tdm->_chip_data.soc_pkg.lr_idx_limit-i]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
					tdm_pipe_main[_tdm->_chip_data.soc_pkg.lr_idx_limit-i]=tdm_pipe_main[j];
					tdm_pipe_main[j]=_tdm->_chip_data.soc_pkg.num_ext_ports;
					break;
				}
			}
		}
	}
	for (j=254; j>0; j--) {
		TOKEN_CHECK(tdm_pipe_main[j]) {
			_tdm->_core_data.vars_pkg.cap=(j+1); 
			break;
		}
	}
	TDM_PRINT1("Preprocessed TDM table terminates at index %0d\n", _tdm->_core_data.vars_pkg.cap);
	if ( _tdm->_core_data.vars_pkg.cap>0 && (_tdm->_chip_data.soc_pkg.speed[tdm_pipe_main[_tdm->_core_data.vars_pkg.cap-1]]>=SPEED_10G) ) {
		cap_tsc=_tdm->_chip_data.soc_pkg.num_ext_ports;
		for (j=0; j<_tdm->_chip_data.soc_pkg.pm_num_phy_modules; j++) {
			for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
				if (_tdm->_chip_data.soc_pkg.pmap[j][i]==tdm_pipe_main[_tdm->_core_data.vars_pkg.cap-1]) {
					cap_tsc=j;
					break;
				}
			}
			if (cap_tsc!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				break;
			}
		}
        if (cap_tsc<_tdm->_chip_data.soc_pkg.pm_num_phy_modules){
            TDM_PRINT1("TDM lookback port module is %0d\n", cap_tsc);
            cap_lkbk=1;
            for (j=(i-2); j>=0; j--) {
                if (tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][0]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][1]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][2]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][3]) {
                    cap_lkbk++;
                }
                else {
                    break;
                }
            }
            TDM_PRINT1("End of TDM table, lookback spacing is %0d\n", cap_lkbk);
            cap_lpbk=1;
            for (j=0; j<i; j++) {
                if (tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][0]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][1]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][2]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][3]) {
                    cap_lpbk++;
                }
                else {
                    break;
                }
            }
            TDM_PRINT1("End of TDM table, loopback spacing is %0d\n", cap_lpbk);
    /* 		if (cap_lpbk <= cap_lkbk) {
                _tdm->_core_data.vars_pkg.cap+=(cap_lkbk-cap_lpbk);
            } */
        }
        else {
            TDM_WARN0("TDM: Loopback port bandwidth may be insufficient\n");
        }
	}
	TDM_PRINT1("Postprocessed TDM table terminates at index %0d\n", _tdm->_core_data.vars_pkg.cap);
	if ((_tdm->_core_data.vars_pkg.cap)>(_tdm->_chip_data.soc_pkg.lr_idx_limit)) {
		TDM_ERROR1("Could not find solution TDM within max length at %0d MHz\n",_tdm->_chip_data.soc_pkg.clk_freq);
		return (TDM_EXEC_CORE_SIZE+1);
	}
	else {
		_tdm->_core_data.vars_pkg.cap = _tdm->_chip_data.soc_pkg.lr_idx_limit;
	}
	TDM_PRINT1("TDM extended to slot %0d\n",_tdm->_core_data.vars_pkg.cap);
	if (_tdm->_core_data.vars_pkg.os_enable && !_tdm->_core_data.vars_pkg.lr_enable) {
		for (j=0; j<_tdm->_core_data.vars_pkg.cap; j++) {
			if (tdm_pipe_main[j]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
				tdm_pipe_main[j] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
			}
		}
	}
	else {
		for (j=0; j<_tdm->_core_data.vars_pkg.cap; j++) {
			if (tdm_pipe_main[j]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
				if (_tdm->_core_data.vars_pkg.os_enable) {
					tdm_pipe_main[j] = _tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
				}
				else {
					if (j%2==0) {
						tdm_pipe_main[j] = _tdm->_chip_data.soc_pkg.soc_vars.idl1_token;
					}
					else {
						tdm_pipe_main[j] = _tdm->_chip_data.soc_pkg.soc_vars.idl2_token;
					}		
				}
			}
		}
	}
	if ( _tdm->_core_data.vars_pkg.lr_enable && _tdm->_core_data.vars_pkg.os_enable && ((((_tdm->_chip_data.soc_pkg.lr_idx_limit-lr_slot_cnt)*BW_QUANTA)/10)<100) && ((_tdm->_core_data.vars_pkg.lr_10+_tdm->_core_data.vars_pkg.lr_20+_tdm->_core_data.vars_pkg.lr_40+_tdm->_core_data.vars_pkg.lr_50+_tdm->_core_data.vars_pkg.lr_100+_tdm->_core_data.vars_pkg.lr_25)==1) ) {
		if (tdm_core_filter_refactor(_tdm)) {
			_tdm->_core_data.vars_pkg.refactor_done=BOOL_TRUE;
		}
	}
	
	tdm_ll_free(&tdm_ll);
	
	if (tdm_core_acc_alloc(_tdm)==FAIL) {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	if (_tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER_OVS](_tdm)==FAIL) {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__FILTER]( _tdm ) );
}


/**
@name: tdm_core_vbs_scheduler_lr
@param:

Allocate slots for linerate ports within the overall TDM calendar capacity limit 
 */
int
tdm_core_vbs_scheduler_lr( tdm_mod_t *_tdm )
{
	unsigned short dim_reduce;
	int i=0, j, k, singular_cnt=0, cal_len,
		timeout=TIMEOUT, lr_slot_cnt=0, pspd, pspd1, fpnode, idx_up, idx_dn,
		tsc_pipeline[ARR_FIXED_LEN], xx=0, x0=0, triport_op=BOOL_FALSE, vec_id, ns_min_cnt, 
		rotation_step=DEFAULT_STEP_SIZE, main_pass_done, main_pass_timeout,
		cadence_cnt=0, cadence_step_size, cadence_ref_idx,
		cadence_step, cadence_step_limit, cadence_blocks,
		cap_tsc=_tdm->_chip_data.soc_pkg.num_ext_ports, cap_lpbk=0, cap_lkbk=0,
		*tdm_pipe_main;
	struct node tdm_ll;
	int tsc_i, tsc_j_prev, tsc_j_curr, tsc_j_next, check_transpose, exist_transpose, src_idx, dst_idx, 
		param_spd_i, param_spd_j, param_spd_ii, same_spd_num;
	int n, v, w, x, x_up, x_dn, migrate_flag, col_idx, row_idx, col_last,
		port_token, check_dist, cnt_tmp; 
	char vmap_bmp[VEC_MAP_LEN]; /* 1->enable; 0->disable; 2->UP; 3->DOWN */
    
	/* tdm_print_vmap_vector(_tdm); */
	tdm_ll.port=0;
	tdm_ll.next=NULL;
	_tdm->_core_data.vars_pkg.refactor_done = BOOL_FALSE;
	_tdm->_core_data.vars_pkg.cap = 0;
	for (i=0; i<VEC_MAP_LEN; i++){vmap_bmp[i]=1;}
	cal_len = _tdm->_chip_data.soc_pkg.lr_idx_limit+_tdm->_chip_data.soc_pkg.tvec_size;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_pipe_main);
	TDM_PRINT1("Length of TDM round standardized to slot quanta set to %d\n",cal_len);
	for (j=0; j<ARR_FIXED_LEN; j++) {
		tsc_pipeline[j]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	}
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt=0;
	if (_tdm->_core_data.vars_pkg.lr_enable) {
		for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
			if (_tdm->_core_data.vmap[i][0]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt++;
			}
		}
	}
	for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
		if (tdm_type_chk(_tdm)==3) {
			triport_op=BOOL_TRUE;
			TDM_PRINT0("Triport mode TRUE\n");
			break;
		}
	}
	for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		lr_slot_cnt+=tdm_slots(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]/1000);
	}
	for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
		if ( !(tdm_arr_exists(_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm),ARR_FIXED_LEN,tsc_pipeline)) ) {
			tsc_pipeline[xx++]=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
		}
	}
	
	if (_tdm->_core_data.vars_pkg.lr_enable) {
		if (!triport_op) {
			tdm_vector_dump(_tdm);
			for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
				if ( (tdm_type_chk(_tdm)>1) && (tdm_count_param_spd(_tdm)>=10) ) {
					timeout=TIMEOUT;
					do {
						x0=(x0>=xx)?(0):(x0);
						_tdm->_core_data.vars_pkg.m_tdm_pick_vec.tsc_dq=tsc_pipeline[x0];
						_tdm->_core_data.vars_pkg.m_tdm_pick_vec.prev_vec=i;
						vec_id=_tdm->_core_exec[TDM_CORE_EXEC__PICK_VEC](_tdm);
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[vec_id][0];
						if ( (tdm_find_fastest_spd(_tdm)<=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]-SPEED_DELIMITER)) ||
							 (tdm_find_fastest_spd(_tdm)>=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]+SPEED_DELIMITER)) ) {
							x0++;
						}
					} while (
							  ( (tdm_find_fastest_spd(_tdm)<=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]-SPEED_DELIMITER)) ||
								(tdm_find_fastest_spd(_tdm)>=(_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vmap[i][0]]+SPEED_DELIMITER)) ) &&
								((--timeout)>0) );
					for (j=i; j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
						if (_tdm->_core_data.vmap[j][0]==tdm_find_fastest_port(_tdm)) {
							vec_id=j;
							break;
						}
					}
					if (vec_id!=i) {
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=i;
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=(_tdm->_core_data.vmap_max_wid-1);
						tdm_mem_transpose(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=vec_id;
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=i;
						tdm_mem_transpose(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=(_tdm->_core_data.vmap_max_wid-1);
						_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=vec_id;
						tdm_mem_transpose(_tdm);
					}
				}
				if ((x0++)>=xx) {x0=0;}
			}
		}
		else{
			param_spd_ii = 0;
			_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
			for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				same_spd_num = 0;
				_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
				param_spd_i = tdm_count_param_spd(_tdm);
				if (param_spd_i<10) {break;}
				if (param_spd_i!=param_spd_ii){
					param_spd_ii = param_spd_i;
					same_spd_num = 1;
					for (j=(i+1); j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[j][0];
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
						param_spd_j = tdm_count_param_spd(_tdm);
						if (param_spd_i==param_spd_j){same_spd_num++;}
						else {break;}
					}
				}
				if (same_spd_num>2){
					_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
					tsc_i = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					for (k=0; k<same_spd_num; k++) {
						exist_transpose = BOOL_FALSE;
						tsc_j_prev = _tdm->_chip_data.soc_pkg.pmap_num_modules;
						tsc_j_curr = tsc_i;
						tsc_j_next = _tdm->_chip_data.soc_pkg.pmap_num_modules;
						for (j=(i+1); ( (j<(i+same_spd_num)) && (j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt) ); j++) {
							check_transpose = FAIL;
							_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[j][0];
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
							tsc_j_next = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
							if (tsc_j_curr==tsc_j_prev && tsc_j_curr!=tsc_j_next && tsc_j_next!=_tdm->_chip_data.soc_pkg.pmap_num_modules){
								src_idx = j;
								dst_idx = j-1;
								check_transpose = PASS;
								exist_transpose = BOOL_TRUE;
							}
							else if (tsc_j_curr!=tsc_j_prev && tsc_j_curr==tsc_j_next && tsc_j_prev!=_tdm->_chip_data.soc_pkg.pmap_num_modules){
								src_idx = j-1;
								dst_idx = j-2;
								if (dst_idx <0) {break;}
								check_transpose = PASS;
								exist_transpose = BOOL_TRUE;
							}
							if (check_transpose == PASS){
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=dst_idx;
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=(_tdm->_core_data.vmap_max_wid-1);
								tdm_mem_transpose(_tdm);
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=src_idx;
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=dst_idx;
								tdm_mem_transpose(_tdm);
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src=(_tdm->_core_data.vmap_max_wid-1);
								_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst=src_idx;
								tdm_mem_transpose(_tdm);
							}
							tsc_j_prev = tsc_j_curr;
							tsc_j_curr = tsc_j_next;
						}
						if (exist_transpose==BOOL_FALSE) {break;}
					}
				}
				
			}
		}
		tdm_vector_dump(_tdm);
		if (_tdm->_core_data.vars_pkg.os_enable) {
			for (i=1; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
				pspd=tdm_count_param_spd(_tdm);
				_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=(i-1);
				if ( ((pspd!=tdm_count_param_spd(_tdm))) ) {
					_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_TRUE;
					_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=(i-1);
					pspd=tdm_count_param_spd(_tdm);
					_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
					pspd1 = tdm_count_param_spd(_tdm);
					if (pspd1>0 && pspd%pspd1==0) {
						cadence_blocks=1;
						cadence_ref_idx=i-1;
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=cadence_ref_idx;
						pspd=tdm_count_param_spd(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=(cadence_ref_idx-1);
						while(cadence_ref_idx>0 && (tdm_count_param_spd(_tdm)==tdm_count_param_spd(_tdm)) ) {
							cadence_ref_idx--;
						}
						for (j=0; j<cal_len && j<_tdm->_core_data.vmap_max_len; j++) {
							if (_tdm->_core_data.vmap[cadence_ref_idx][j]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[i][j]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
								cadence_blocks++;
							}
							if (j>0 && _tdm->_core_data.vmap[cadence_ref_idx][j]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[i][j]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
								break;
							}
						}
						for (j=i; j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
							pspd=tdm_count_param_spd(_tdm);
							_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
							if ( pspd==tdm_count_param_spd(_tdm) ) {
								cadence_cnt++;
							}
						}
						cadence_step_size=(cadence_cnt/cadence_blocks);
						if (cadence_step_size<1) {cadence_step_size=1;}
						for (j=(i+cadence_step_size); j<(i+cadence_cnt); j+=cadence_step_size) {
							for (k=0; k<cadence_step_size; k++) {
								cadence_step=0; 
								cadence_step_limit=(cal_len/4);
								do {
									_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=(j+k);
									_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=cal_len;
									_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=1;
									tdm_vector_rotate(_tdm);
									_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle=0;
									_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx=(j+k);
									if (_tdm->_core_data.vmap[cadence_ref_idx][tdm_map_find_y_indx(_tdm)]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[j+k][tdm_map_find_y_indx(_tdm)]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
										cadence_step++;
									}
									if ( cadence_step==((j-i)/cadence_step_size) ) {
										break;
									}
								} while ( (--cadence_step_limit)>0 );
							}
						}
						tdm_vector_dump(_tdm);
					}
				}
			}
		}
		main_pass_timeout=0;
		do {
			for (i=1; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				dim_reduce=FAIL;
				for (k=0; k<cal_len; k++) {
					_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=i;
					_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=cal_len;
					_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=rotation_step;
					tdm_vector_rotate(_tdm);
					tdm_vector_dump(_tdm);				
					_tdm->_core_data.vars_pkg.m_tdm_fit_singular_col.node_x=i;
					_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=i;
					_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
					if ( (tdm_fit_singular_col(_tdm)==PASS) && 
						 (tdm_fit_prox(_tdm)==PASS) ) {
						dim_reduce=PASS;
						break;
					}
				}
				if (!dim_reduce) {
					_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx=i;
					ns_min_cnt=tdm_count_nonsingular(_tdm);
					for (k=0; k<cal_len; k++) {
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=i;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=cal_len;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=rotation_step;
						tdm_vector_rotate(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx=i;
						_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=i;
						_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
						if ( (tdm_count_nonsingular(_tdm)<ns_min_cnt) &&
							 (tdm_fit_prox(_tdm)==PASS) ) {
							ns_min_cnt=tdm_count_nonsingular(_tdm);
						}
					}
					_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx=i;
					while ( ns_min_cnt!=tdm_count_nonsingular(_tdm) ) {
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector=i;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size=cal_len;
						_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step=rotation_step;
						tdm_vector_rotate(_tdm);
						tdm_vector_dump(_tdm);
					}
				}
			}
			for (i=1; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
				_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt;
				_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
				if (tdm_fit_prox(_tdm)!=PASS) {
					for (j=0; j<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; j++) {
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round=BOOL_FALSE;
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=j;
						pspd=tdm_count_param_spd(_tdm);
						_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos=i;
						if (pspd==tdm_count_param_spd(_tdm)) {
							_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src=i;
							_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst=j;
							tdm_sticky_transpose(_tdm);
							_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt;
							_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=i;
							fpnode=tdm_fit_prox(_tdm);
							_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x=j;
							if ( (fpnode==PASS) &&
								 (tdm_fit_prox(_tdm)==PASS) ) {
								tdm_vector_dump(_tdm);
								break;
							}
							else {
								_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src=i;
								_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst=j;
								tdm_sticky_transpose(_tdm);
							}
						}
					}
				}
			}
			main_pass_done=BOOL_TRUE;
			if (!(_tdm->_core_data.vars_pkg.lr_1)) {
				for (i=0; i<cal_len; i++) {
					_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx=i;
					_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx=i;
					if ( (!tdm_nsin_row(_tdm)) || (!tdm_fit_row_min(_tdm)) ) {
						main_pass_done=BOOL_FALSE;
						break;
					}
				}
			}
		} while (!main_pass_done && (++main_pass_timeout<MAIN_SCHEDULER_PASSES));
	}
	TDM_PRINT0("\n\n");
	TDM_SML_BAR
	TDM_PRINT0("\n Original vmap \n\n");
	tdm_print_vmap_vector(_tdm);
	TDM_PRINT0("\n\n\n");
	TDM_SML_BAR
    /* singularity */
	for (i=0; i<cal_len && i<_tdm->_core_data.vmap_max_len && i<VEC_MAP_LEN; i++) {
		singular_cnt = tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
		if (singular_cnt>1) {
            /* get last column */
            col_last = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt;
            for (j=(col_last-1); j>0; j--){
                TOKEN_CHECK(_tdm->_core_data.vmap[j][i]){
                    col_last = j+1;
                    break;
                }
            }
            /* deal with each non-singular slot */
			for (j=1; j<singular_cnt; j++){
				row_idx= i;
                idx_up = i;
                idx_dn = i;
                x_up = i;
                x_dn = i;
                migrate_flag = BOOL_FALSE;
				/* case 1: local insertion */
                for (k=(i-1); k>0 && i>0; k--){
					if (vmap_bmp[k]>0 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
                        check_dist = PASS;
                        for (w=0; w<col_last; w++){
                            x_up = k;
                            x_dn = k;
                            n    = 0;
                            /* boundary */
                            for (x=(k-1); x>0 && k>0; x--){
                                TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                    x_up = x;
                                    break;
                                }
                            }
                            if (x_up==k){
                                for (x=(cal_len-1); x>=0 && x<_tdm->_core_data.vmap_max_len; x--){
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                        x_up = x;
                                        break;
                                    }
                                }
                            }
                            for (x=(k+1); x<cal_len && x<_tdm->_core_data.vmap_max_len; x++){
                                TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                    x_dn = x;
                                    break;
                                }
                            }
                            if (x_dn==k){
                                for (x=0; x<cal_len && x<_tdm->_core_data.vmap_max_len; x++){
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                        x_dn = x;
                                        break;
                                    }
                                }
                            }
                            /* distance */
                            for (v=k; v>=0; v--){
                                if (vmap_bmp[v]>0){
                                    cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                    n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                        if (cnt_tmp>1 && vmap_bmp[v]==2){
                                            n -= (cnt_tmp-1);
                                        }
                                        break;
                                    }
                                }
                            }
                            if (x_up>k){
                                for (v=(cal_len-1); v>x_up && v<_tdm->_core_data.vmap_max_len; v--){
                                    if (vmap_bmp[v]>0){
                                        cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                        n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                        TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                            if (cnt_tmp>1 && vmap_bmp[v]==2){
                                                n -= (cnt_tmp-1);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                            for (v=(k+1); v<cal_len && v<_tdm->_core_data.vmap_max_len && v<VEC_MAP_LEN; v++){
                                if (vmap_bmp[v]>0){
                                    cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                    n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                        if (cnt_tmp>1 && vmap_bmp[v]==1){
                                            n -= (cnt_tmp-1);
                                        }
                                        break;
                                    }
                                }
                            }
                            if (x_dn<k){
                                for (v=0; v<x_dn && v<cal_len && v<_tdm->_core_data.vmap_max_len; v++){
                                    if (vmap_bmp[v]>0){
                                        cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                        n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                        TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                            if (cnt_tmp>1 && vmap_bmp[v]==1){
                                                n -= (cnt_tmp-1);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                            /* check */
                            if ( !(n>(9+1)) ){
                                check_dist = FAIL;
                                break;
                            }
                        }
                        if (check_dist==PASS){
                            idx_up  = k;
                            break;
                        }
					}
				}
				for (k=(i+1); k<cal_len && k<_tdm->_core_data.vmap_max_len && k<VEC_MAP_LEN; k++){
					if (vmap_bmp[k]>0 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
                        check_dist = PASS;
                        for (w=0; w<col_last; w++){
                            x_up = k;
                            x_dn = k;
                            n    = 0;
                            /* boundary */
                            for (x=(k-1); x>0 && k>0; x--){
                                TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                    x_up = x;
                                    break;
                                }
                            }
                            if (x_up==k){
                                for (x=(cal_len-1); x>=0 && x<_tdm->_core_data.vmap_max_len; x--){
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                        x_up = x;
                                        break;
                                    }
                                }
                            }
                            for (x=(k+1); x<cal_len && x<_tdm->_core_data.vmap_max_len; x++){
                                TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                    x_dn = x;
                                    break;
                                }
                            }
                            if (x_dn==k){
                                for (x=0; x<cal_len && x<_tdm->_core_data.vmap_max_len; x++){
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][x]){
                                        x_dn = x;
                                        break;
                                    }
                                }
                            }
                            /* distance */
                            for (v=k; v>=0; v--){
                                if (vmap_bmp[v]>0){
                                    cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                    n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                        if (cnt_tmp>1 && vmap_bmp[v]==2){
                                            n -= (cnt_tmp-1);
                                        }
                                        break;
                                    }
                                }
                            }
                            if (x_up>k){
                                for (v=(cal_len-1); v>x_up && v<_tdm->_core_data.vmap_max_len; v--){
                                    if (vmap_bmp[v]>0){
                                        cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                        n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                        TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                            if (cnt_tmp>1 && vmap_bmp[v]==2){
                                                n -= (cnt_tmp-1);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                            for (v=(k+1); v<cal_len && v<_tdm->_core_data.vmap_max_len && v<VEC_MAP_LEN; v++){
                                if (vmap_bmp[v]>0){
                                    cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                    n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                    TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                        if (cnt_tmp>1 && vmap_bmp[v]==1){
                                            n -= (cnt_tmp-1);
                                        }
                                        break;
                                    }
                                }
                            }
                            if (x_dn<k){
                                for (v=0; v<x_dn && v<cal_len && v<_tdm->_core_data.vmap_max_len; v++){
                                    if (vmap_bmp[v]>0){
                                        cnt_tmp = tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
                                        n = (cnt_tmp>0)? (n+cnt_tmp): (n+1);
                                        TOKEN_CHECK(_tdm->_core_data.vmap[w][v]){
                                            if (cnt_tmp>1 && vmap_bmp[v]==1){
                                                n -= (cnt_tmp-1);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                            /* check */
                            if ( !(n>(9+1)) ){
                                check_dist = FAIL;
                                break;
                            }
                        }
                        if (check_dist==PASS){
                            idx_dn  = k;
                            break;
                        }
					}
				}
                if ( idx_up!=i || idx_dn!=i ){
                    if (idx_up!=i && idx_dn!=i){
                        if(idx_up<i && idx_dn>i){
                            row_idx = ((i-idx_up)<(idx_dn-i))?
                                      (idx_up):
                                      (idx_dn);
                        }
                        else if (idx_up>i && idx_dn>i){
                            row_idx = ((cal_len-idx_up+i)<(idx_dn-i))?
                                      (idx_up):
                                      (idx_dn);
                        }
                        else if (idx_up<i && idx_dn<i){
                            row_idx = ((i-idx_up)<(idx_dn+cal_len-1-i))?
                                      (idx_up):
                                      (idx_dn);
                        }
                        if (row_idx==idx_up && row_idx!=i){
                            vmap_bmp[i] = 2;
                        }
                    }
                    else if (idx_up!=i && idx_dn==i){
                        row_idx = idx_up;
                    }
                    else if (idx_up==i && idx_dn!=i){
                        row_idx = idx_dn;
                    }
                }
                /* case 2: remote migration */
                if (row_idx==i) {
                    idx_up = i;
                    idx_dn = i;
                    migrate_flag = BOOL_TRUE;
                    for (k=(i-1); k>0 && i>0; k--){
                        if (vmap_bmp[k]>0 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
                            idx_up = k;
                            break;
                        }
                    }
                    if (idx_up==i){
                        for (k=(cal_len-1); k>=0 && k<_tdm->_core_data.vmap_max_len && k<VEC_MAP_LEN; k--){
                            if (vmap_bmp[k]>0 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
                                idx_up = k;
                                break;
                            }
                        }
                    }
                    for (k=(i+1); k<cal_len && k<_tdm->_core_data.vmap_max_len && k<VEC_MAP_LEN; k++){
                        if (vmap_bmp[k]>0 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
                            idx_dn = k;
                            break;
                        }
                    }
                    if (idx_dn==i){
                        for (k=0; k<cal_len && k<_tdm->_core_data.vmap_max_len && k<VEC_MAP_LEN; k++){
                            if (vmap_bmp[k]>0 && 0==tdm_fit_singular_cnt(_tdm->_core_data.vmap,k,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)){
                                idx_dn = k;
                                break;
                            }
                        }
                    }
                    if ( idx_up!=i || idx_dn!=i ){
                        if (idx_up!=i && idx_dn!=i){
                            if(idx_up<i && idx_dn>i){
                                row_idx = ((i-idx_up)<(idx_dn-i))?
                                          (idx_up):
                                          (idx_dn);
                            }
                            else if (idx_up>i && idx_dn>i){
                                row_idx = ((cal_len-idx_up+i)<(idx_dn-i))?
                                          (idx_up):
                                          (idx_dn);
                            }
                            else if (idx_up<i && idx_dn<i){
                                row_idx = ((i-idx_up)<(idx_dn+cal_len-i))?
                                          (idx_up):
                                          (idx_dn);
                            }
                        }
                        else if (idx_up!=i && idx_dn==i){
                            row_idx = idx_up;
                        }
                        else if (idx_up==i && idx_dn!=i){
                            row_idx = idx_dn;
                        }
                    }
                    
                }
                /* insert or migrate non-singular slot */
                if (row_idx!=i && row_idx>0 && row_idx<VEC_MAP_LEN){
                    if (migrate_flag==BOOL_FALSE){
                        vmap_bmp[row_idx]=0;
                        TDM_PRINT2("Extra slot at row %3d, remove empty row %3d\n",
                                  i, row_idx);
                    }
                    else {
                        n = 0;
                        col_idx = _tdm->_core_data.vmap_max_wid;
                        for (k=0; k<_tdm->_core_data.vmap_max_wid; k++){
                            TOKEN_CHECK(_tdm->_core_data.vmap[k][i]) {
                                if ((++n)==2){
                                    col_idx = k;
                                    break;
                                }
                            }
                        }
                        if (col_idx>0 && col_idx<_tdm->_core_data.vmap_max_wid){
                            port_token = _tdm->_core_data.vmap[col_idx][i];
                            _tdm->_core_data.vmap[col_idx][i] = _tdm->_core_data.vmap[col_idx][row_idx];
                            _tdm->_core_data.vmap[col_idx][row_idx] = port_token;
                            TDM_PRINT4("Migrate slot at column %2d, port %3d, from row %3d to %3d\n",
                                      col_idx, port_token, i, row_idx);
                        }
                    }
                }
			}
		}
	}
	TDM_PRINT0("\n\n");
	TDM_SML_BAR
	TDM_PRINT1("\n Vmap solution (Pipe %0d)\n\n",((_tdm->_core_data.vmap[0][0])/64));
	tdm_print_vmap_vector(_tdm);
	TDM_PRINT0("\n\n\n");
	TDM_SML_BAR
	
	TDM_PRINT0("\n\tConstructing TDM linked list...\n");
	TDM_PRINT5("\t\t| Span=%0d | Length=%0d | Pivot=%0d | Slice Size=%0d | Retrace=%0d |\n",_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt,cal_len,_tdm->_core_data.vmap[0][0],tdm_map_cadence_count(_tdm->_core_data.vmap[0],0,_tdm->_core_data.vmap_max_len),tdm_map_retrace_count(_tdm->_core_data.vmap,0,0,_tdm->_core_data.vmap_max_len,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports));
	
	for (i=0; i<cal_len && i<_tdm->_core_data.vmap_max_len && i<VEC_MAP_LEN; i++) {
		if (vmap_bmp[i]>0){
			singular_cnt = tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports);
			if (singular_cnt==0){
				tdm_ll_append(&tdm_ll,_tdm->_chip_data.soc_pkg.num_ext_ports);
			}
			else {
                if (vmap_bmp[i]==1){
                    for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
                        TOKEN_CHECK(_tdm->_core_data.vmap[j][i]) {
                            tdm_ll_append(&tdm_ll,_tdm->_core_data.vmap[j][i]);
                        }
                    }
                }
                else {
                    for (j=(_tdm->_core_data.vmap_max_wid-1); j>=0; j--) {
                        TOKEN_CHECK(_tdm->_core_data.vmap[j][i]) {
                            tdm_ll_append(&tdm_ll,_tdm->_core_data.vmap[j][i]);
                        }
                    }
                }
			}
		}
	}
	for (i=0; i<60; i++) {
		for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
			if (tdm_fit_singular_cnt(_tdm->_core_data.vmap,i,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)>0) {
				TOKEN_CHECK(_tdm->_core_data.vmap[j][i]) {
					tdm_ll_append(&tdm_ll,_tdm->_core_data.vmap[j][i]);
				}
			}
			else {
				tdm_ll_append(&tdm_ll,_tdm->_chip_data.soc_pkg.num_ext_ports);
				break;
			}
		}
	}
	tdm_ll_print(&tdm_ll);
	tdm_ll_deref(&tdm_ll,tdm_pipe_main,cal_len);
	for (j=cal_len; j<_tdm->_core_data.vmap_max_len; j++) {
		TOKEN_CHECK(tdm_pipe_main[j]) {
			for (i=1; i<cal_len; i++) {
				if (tdm_pipe_main[cal_len-i]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
					tdm_pipe_main[cal_len-i]=tdm_pipe_main[j];
					tdm_pipe_main[j]=_tdm->_chip_data.soc_pkg.num_ext_ports;
					break;
				}
			}
		}
	}
	for (j=254; j>0; j--) {
		TOKEN_CHECK(tdm_pipe_main[j]) {
			_tdm->_core_data.vars_pkg.cap=(j+1); 
			break;
		}
	}
	TDM_PRINT1("Preprocessed TDM table terminates at index %0d\n", _tdm->_core_data.vars_pkg.cap);
	if ( _tdm->_core_data.vars_pkg.cap>0 && (_tdm->_chip_data.soc_pkg.speed[tdm_pipe_main[_tdm->_core_data.vars_pkg.cap-1]]>=SPEED_10G) ) {
		cap_tsc=_tdm->_chip_data.soc_pkg.num_ext_ports;
		for (j=0; j<_tdm->_chip_data.soc_pkg.pm_num_phy_modules; j++) {
			for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
				if (_tdm->_chip_data.soc_pkg.pmap[j][i]==tdm_pipe_main[_tdm->_core_data.vars_pkg.cap-1]) {
					cap_tsc=j;
					break;
				}
			}
			if (cap_tsc!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
				break;
			}
		}
		TDM_PRINT1("TDM lookback port module is %0d\n", cap_tsc);
		cap_lkbk=1;
		for (j=(i-2); j>=0; j--) {
			if (tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][0]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][1]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][2]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][3]) {
				cap_lkbk++;
			}
			else {
				break;
			}
		}
		TDM_PRINT1("End of TDM table, lookback spacing is %0d\n", cap_lkbk);
		cap_lpbk=1;
		for (j=0; j<i; j++) {
			if (tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][0]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][1]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][2]&&tdm_pipe_main[j]!=_tdm->_chip_data.soc_pkg.pmap[cap_tsc][3]) {
				cap_lpbk++;
			}
			else {
				break;
			}
		}
		TDM_PRINT1("End of TDM table, loopback spacing is %0d\n", cap_lpbk);
/* 		if (cap_lpbk <= cap_lkbk) {
			_tdm->_core_data.vars_pkg.cap+=(cap_lkbk-cap_lpbk);
		} */
	}
	TDM_PRINT1("Postprocessed TDM table terminates at index %0d\n", _tdm->_core_data.vars_pkg.cap);
	if ((_tdm->_core_data.vars_pkg.cap)>(cal_len)) {
		TDM_ERROR1("Could not find solution TDM within max length at %0d MHz\n",_tdm->_chip_data.soc_pkg.clk_freq);
		return (TDM_EXEC_CORE_SIZE+1);
	}
	else {
		_tdm->_core_data.vars_pkg.cap = cal_len;
	}
	TDM_PRINT1("TDM extended to slot %0d\n",_tdm->_core_data.vars_pkg.cap);
	
	tdm_ll_free(&tdm_ll);
	
	return PASS;
}


/**
@name: tdm_core_vbs_scheduler
@param:

Allocate slots for linerate ports within the linerate capacity limit
 */
int
tdm_core_vbs_scheduler_wrapper( tdm_mod_t *_tdm )
{   
    if (_tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER_OVS](_tdm)!=PASS) {
        return (TDM_EXEC_CORE_SIZE+1);
    }
    return ( _tdm->_chip_exec[TDM_CHIP_EXEC__FILTER]( _tdm ) );
}
