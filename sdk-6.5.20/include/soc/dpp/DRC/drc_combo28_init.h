/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains DPP DRC Init structure and routine declarations for the Dram operation using PHY Combo28.
 */
#ifndef _SOC_DPP_DRC_COMBO28_INIT_H
#define _SOC_DPP_DRC_COMBO28_INIT_H

 
#include <soc/dpp/DRC/drc_combo28.h>


int soc_dpp_drc_combo28_info_prepare(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_mr_register_info_prepare(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_info_struct_convert(int unit, soc_dpp_drc_combo28_info_t *drc_info, combo28_shmoo_dram_info_t *shmoo_info);
int soc_dpp_drc_combo28_init_dprc_init(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_init_drc_init(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_init_tune_ddr(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_init_gddr5_cdr_mechanism(int unit, int drc_ndx, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_sw_state_init(int unit);


int soc_dpp_drc_combo28_dprc_out_of_reset(int unit, int drc_ndx);
int soc_dpp_drc_combo28_drc_clam_shell_cfg(int drc_ndx, int init);
int soc_dpp_drc_combo28_drc_soft_init(int unit, int drc_ndx, int init);


#endif 
