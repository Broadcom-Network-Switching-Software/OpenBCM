/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef SOC_DPP_DPP_DEFS_H
#define SOC_DPP_DPP_DEFS_H


typedef enum soc_dpp_stat_path_drop_stage_e {
    soc_dpp_stat_path_drop_stage_none = 0,
    soc_dpp_stat_path_drop_stage_ingress_no_packet = 1,
    soc_dpp_stat_path_drop_stage_ingress_tm = 2,
    soc_dpp_stat_path_drop_stage_egress_tm = 3,

    
    soc_dpp_stat_path_drop_stage_nof = 4
} soc_dpp_stat_path_drop_stage_t;


typedef struct soc_dpp_stat_path_info_s {
    int ingress_core;
    int egress_core;
    soc_dpp_stat_path_drop_stage_t drop;
} soc_dpp_stat_path_info_t;


typedef struct soc_dpp_pll_s {
    int p_div; 
    int n_div; 
    int m0_div; 
    int m1_div; 
    int m4_div; 
    int m5_div; 
    int locked; 
#ifdef COMPILER_HAS_DOUBLE
    double vco; 
    double ch0; 
    double ch1; 
    double ch4; 
    double ch5; 
    double ref_clk; 
#else
    int vco; 
    int ch0; 
    int ch1; 
    int ch4; 
    int ch5; 
    int ref_clk; 
#endif
} soc_dpp_pll_t;


typedef struct soc_dpp_pll_info_s {
    soc_dpp_pll_t core_pll;
    soc_dpp_pll_t uc_pll;
    soc_dpp_pll_t ts_pll;
    soc_dpp_pll_t bs_pll[2];
    soc_dpp_pll_t pmh_pll;
    soc_dpp_pll_t pml_pll[2];
    soc_dpp_pll_t fab_pll[2];
    soc_dpp_pll_t srd_pll[2];
    soc_dpp_pll_t ddr_pll[4];
} soc_dpp_pll_info_t;


#endif 

