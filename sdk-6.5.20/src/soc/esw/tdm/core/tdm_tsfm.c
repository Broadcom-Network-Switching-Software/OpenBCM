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
@name: tdm_mem_transpose
@param:

Transpose vector in vector map using memcpy standard function
 */
void
tdm_mem_transpose( tdm_mod_t *_tdm )
{
	int i;
	
	/* memcpy( &(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst]), &(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src]), sizeof(_tdm->_core_data.vmap[0]) ); // Requires perfectly contiguous memory */
	for (i=0; i<_tdm->_core_data.vmap_max_len; i++) {
		memcpy( &(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.dst][i]), &(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src][i]), sizeof(unsigned short) );
	}
	/* Clear source vector */
	for (i=0; i<_tdm->_core_data.vmap_max_len; i++) {
		_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_mem_transpose.src][i]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	}
}


/**
@name: tdm_sticky_transpose
@param:

Transpose vector in vector map with shape persistance
 */
void
tdm_sticky_transpose( tdm_mod_t *_tdm )
{
	int transpose_tmp, k, cal_len, _principle, _idx;
    
    _principle= _tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle,
    _idx      = _tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx;
    cal_len   = _tdm->_chip_data.soc_pkg.lr_idx_limit + 
                _tdm->_chip_data.soc_pkg.tvec_size;
	_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle=0;
	_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx=_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst;	
	transpose_tmp=_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst][tdm_map_find_y_indx(_tdm)];
	
	for (k=0; k<cal_len && k<VEC_MAP_LEN; k++) {
		_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx=_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src;
		_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst][k]=(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst][k]==_tdm->_chip_data.soc_pkg.num_ext_ports)?(_tdm->_chip_data.soc_pkg.num_ext_ports):(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src][tdm_map_find_y_indx(_tdm)]);
	}
	for (k=0; k<cal_len && k<VEC_MAP_LEN; k++) {
		_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src][k]=(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src][k]==_tdm->_chip_data.soc_pkg.num_ext_ports)?(_tdm->_chip_data.soc_pkg.num_ext_ports):(transpose_tmp);
	}
	
	_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle=_principle;
	_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx=_idx;
}


/**
@name: tdm_sticky_transpose
@param:

Transpose vector in vector map with shape persistance
 */
void
tdm_col_transpose( tdm_mod_t *_tdm )
{
	int k, cal_len, token_src, token_dst;
	
	cal_len = _tdm->_chip_data.soc_pkg.lr_idx_limit + _tdm->_chip_data.soc_pkg.tvec_size;
	
	for (k=0; k<cal_len && k<VEC_MAP_LEN; k++) {
		token_src = _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src][k];
		token_dst = _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst][k];
		_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.src][k]=token_dst;
		_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_sticky_transpose.dst][k]=token_src;
	}
}


/**
@name: tdm_vector_rotate_step
@param: int[], int, int

Performs rotation transform on input vector array of nodes in integer steps, can wrap around feature space
 */
void
tdm_vector_rotate_step(unsigned short vector[], int size, int step)
{
	int i,j, last;
	
	if (step > 0) {
		for (j=0; j<step; j++) {
			last=vector[size-1];
			for (i=(size-1); i>0; i--) {
				vector[i]=vector[i-1];
			}
			vector[0]=last;
		}
	}
	else if (step < 0) {
		for (j=(tdm_abs(step)); j>0; j--) {
			last=vector[0];
			for (i=0; i<size; i++) {
				vector[i]=vector[i+1];
			}
			vector[size-1]=last;
		}
	}
}


/**
@name: tdm_vector_rotate
@param:

Wraps rotation transform contingent upon the skew of the vector
 */
void
tdm_vector_rotate( tdm_mod_t *_tdm )
{
	int i;
	
	if (_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step > 0) {
		for (i=0; i<_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step; i++) {
			if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector][_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size-1]!=_tdm->_chip_data.soc_pkg.num_ext_ports && i<_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step) {
				while (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector][0]==_tdm->_chip_data.soc_pkg.num_ext_ports) {
					tdm_vector_rotate_step(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector], _tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size, -1);
				}
			}
			else {
				tdm_vector_rotate_step(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector], _tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size, 1);
			}
		}
	}
	else {
		tdm_vector_rotate_step(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_vector_rotate.vector], _tdm->_core_data.vars_pkg.m_tdm_vector_rotate.size, _tdm->_core_data.vars_pkg.m_tdm_vector_rotate.step);
	}
}


/**
@name: tdm_vector_clear
@param:

Remove vector at index in TDM vector map
 */
void
tdm_vector_clear( tdm_mod_t *_tdm )
{
	int i;
	
	for (i=0; i<_tdm->_core_data.vmap_max_len; i++) {
		_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_vector_clear.yy][i]=_tdm->_chip_data.soc_pkg.num_ext_ports;
	}
}

