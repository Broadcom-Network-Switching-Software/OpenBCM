/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _SOC_DPP_JER_INIT_H
#define _SOC_DPP_JER_INIT_H




#define FDT_IPT_MESH_MC_TABLE_MAX_INX       2048
#define IQM_PACKING_MODE_TABLE_MAX_INX      128



int soc_jer_tbls_init(int unit);


int soc_jer_tbls_deinit(int unit);


int soc_jer_init_sequence_phase1(int unit);


int soc_jer_init_brdc_blk_id_set(int unit);


soc_error_t soc_jer_pp_mgmt_functional_init(int unit);


int jer_pll_init(int unit);


int jer_synce_init(int unit);


int soc_jer_init_blocks_conf(int unit);


int soc_jer_write_masks_set (int unit);


uint32
soc_jer_ipt_contexts_init(
  SOC_SAND_IN int     unit
);


int soc_bist_all_jer(const int unit, const int skip_errors);
int soc_bist_all_qax(const int unit, const int skip_errors);
int qax_mbist_fix_arm_core(const int unit, const int skip_errors);

int soc_jer_core_clock_verification(int unit);

#endif 

