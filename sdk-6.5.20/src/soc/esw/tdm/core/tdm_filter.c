/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * TDM core filtering functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_core_filter_refactor
@param:
 */
int
tdm_core_filter_refactor( tdm_mod_t *_tdm )
{
	int i, j, os_re_pool=0, os_re_spacing, os_re_cnt=0, *tdm_cal_main;
	
	TDM_SEL_CAL(_tdm->_core_data.vars_pkg.cal_id,tdm_cal_main);
	TDM_PRINT0("Filter applied: Refactor list distribution\n");
	for (i=0; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
		if (tdm_cal_main[i]==_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token) {
			++os_re_pool;
			for (j=i; j<_tdm->_chip_data.soc_pkg.lr_idx_limit; j++) {
				tdm_cal_main[j]=tdm_cal_main[j+1];
			}
			i--;
		}
	}
	os_re_spacing=(os_re_pool>0)?(_tdm->_chip_data.soc_pkg.lr_idx_limit/os_re_pool):(_tdm->_chip_data.soc_pkg.lr_idx_limit);
	for (i=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; ( (i<_tdm->_chip_data.soc_pkg.lr_idx_limit) && ((++os_re_cnt)<=os_re_pool) ); i+=os_re_spacing) {
		for (j=_tdm->_chip_data.soc_pkg.lr_idx_limit; j>i; j--) {
			tdm_cal_main[j]=tdm_cal_main[j-1];
		}
		tdm_cal_main[i]=_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token;
	}
	
	return PASS;
	
}
