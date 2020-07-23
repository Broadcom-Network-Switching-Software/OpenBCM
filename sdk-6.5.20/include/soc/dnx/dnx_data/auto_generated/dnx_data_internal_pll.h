

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PLL_H_

#define _DNX_DATA_INTERNAL_PLL_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_pll_submodule_general,
    dnx_data_pll_submodule_pll1,
    dnx_data_pll_submodule_pll3,
    dnx_data_pll_submodule_pll4,
    dnx_data_pll_submodule_synce_pll,

    
    _dnx_data_pll_submodule_nof
} dnx_data_pll_submodule_e;








int dnx_data_pll_general_feature_get(
    int unit,
    dnx_data_pll_general_feature_e feature);



typedef enum
{

    
    _dnx_data_pll_general_define_nof
} dnx_data_pll_general_define_e;




typedef enum
{
    dnx_data_pll_general_table_nif_pll,
    dnx_data_pll_general_table_fabric_pll,

    
    _dnx_data_pll_general_table_nof
} dnx_data_pll_general_table_e;



const dnx_data_pll_general_nif_pll_t * dnx_data_pll_general_nif_pll_get(
    int unit,
    int pll_index);


const dnx_data_pll_general_fabric_pll_t * dnx_data_pll_general_fabric_pll_get(
    int unit,
    int pll_index);



shr_error_e dnx_data_pll_general_nif_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_pll_general_fabric_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pll_general_nif_pll_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_pll_general_fabric_pll_info_get(
    int unit);






int dnx_data_pll_pll1_feature_get(
    int unit,
    dnx_data_pll_pll1_feature_e feature);



typedef enum
{
    dnx_data_pll_pll1_define_nof_pll1,
    dnx_data_pll_pll1_define_ts_phase_initial_lo,
    dnx_data_pll_pll1_define_ts_phase_initial_hi,
    dnx_data_pll_pll1_define_ts_nof_bits_hi,
    dnx_data_pll_pll1_define_ts_nof_bits_right_shift_ns,
    dnx_data_pll_pll1_define_ts_pll1_id,
    dnx_data_pll_pll1_define_bs_pll1_id,
    dnx_data_pll_pll1_define_fab_pll1_id,
    dnx_data_pll_pll1_define_nif_pll1_id,
    dnx_data_pll_pll1_define_flexe_pll1_id,

    
    _dnx_data_pll_pll1_define_nof
} dnx_data_pll_pll1_define_e;



uint32 dnx_data_pll_pll1_nof_pll1_get(
    int unit);


uint32 dnx_data_pll_pll1_ts_phase_initial_lo_get(
    int unit);


uint32 dnx_data_pll_pll1_ts_phase_initial_hi_get(
    int unit);


uint32 dnx_data_pll_pll1_ts_nof_bits_hi_get(
    int unit);


uint32 dnx_data_pll_pll1_ts_nof_bits_right_shift_ns_get(
    int unit);


uint32 dnx_data_pll_pll1_ts_pll1_id_get(
    int unit);


uint32 dnx_data_pll_pll1_bs_pll1_id_get(
    int unit);


uint32 dnx_data_pll_pll1_fab_pll1_id_get(
    int unit);


uint32 dnx_data_pll_pll1_nif_pll1_id_get(
    int unit);


uint32 dnx_data_pll_pll1_flexe_pll1_id_get(
    int unit);



typedef enum
{
    dnx_data_pll_pll1_table_config,

    
    _dnx_data_pll_pll1_table_nof
} dnx_data_pll_pll1_table_e;



const dnx_data_pll_pll1_config_t * dnx_data_pll_pll1_config_get(
    int unit,
    int pll1_type);



shr_error_e dnx_data_pll_pll1_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pll_pll1_config_info_get(
    int unit);






int dnx_data_pll_pll3_feature_get(
    int unit,
    dnx_data_pll_pll3_feature_e feature);



typedef enum
{
    dnx_data_pll_pll3_define_nof_pll3,
    dnx_data_pll_pll3_define_vco_clock,
    dnx_data_pll_pll3_define_kp,
    dnx_data_pll_pll3_define_ki,
    dnx_data_pll_pll3_define_en_ctrl,
    dnx_data_pll_pll3_define_en_ctrl_byp,
    dnx_data_pll_pll3_define_route_ctr_byp,
    dnx_data_pll_pll3_define_ref_clock_125,
    dnx_data_pll_pll3_define_ref_clock_156_25,
    dnx_data_pll_pll3_define_ref_clock_312_5,
    dnx_data_pll_pll3_define_nif0_pll3_id,
    dnx_data_pll_pll3_define_nif1_pll3_id,
    dnx_data_pll_pll3_define_mas0_pll3_id,
    dnx_data_pll_pll3_define_mas1_pll3_id,

    
    _dnx_data_pll_pll3_define_nof
} dnx_data_pll_pll3_define_e;



uint32 dnx_data_pll_pll3_nof_pll3_get(
    int unit);


uint32 dnx_data_pll_pll3_vco_clock_get(
    int unit);


uint32 dnx_data_pll_pll3_kp_get(
    int unit);


uint32 dnx_data_pll_pll3_ki_get(
    int unit);


uint32 dnx_data_pll_pll3_en_ctrl_get(
    int unit);


uint32 dnx_data_pll_pll3_en_ctrl_byp_get(
    int unit);


uint32 dnx_data_pll_pll3_route_ctr_byp_get(
    int unit);


uint32 dnx_data_pll_pll3_ref_clock_125_get(
    int unit);


uint32 dnx_data_pll_pll3_ref_clock_156_25_get(
    int unit);


uint32 dnx_data_pll_pll3_ref_clock_312_5_get(
    int unit);


uint32 dnx_data_pll_pll3_nif0_pll3_id_get(
    int unit);


uint32 dnx_data_pll_pll3_nif1_pll3_id_get(
    int unit);


uint32 dnx_data_pll_pll3_mas0_pll3_id_get(
    int unit);


uint32 dnx_data_pll_pll3_mas1_pll3_id_get(
    int unit);



typedef enum
{
    dnx_data_pll_pll3_table_config,

    
    _dnx_data_pll_pll3_table_nof
} dnx_data_pll_pll3_table_e;



const dnx_data_pll_pll3_config_t * dnx_data_pll_pll3_config_get(
    int unit,
    int pll3_type);



shr_error_e dnx_data_pll_pll3_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pll_pll3_config_info_get(
    int unit);






int dnx_data_pll_pll4_feature_get(
    int unit,
    dnx_data_pll_pll4_feature_e feature);



typedef enum
{
    dnx_data_pll_pll4_define_nof_pll4,
    dnx_data_pll_pll4_define_ts_pll4_id,
    dnx_data_pll_pll4_define_bs_pll4_id,
    dnx_data_pll_pll4_define_nif_pll4_id,
    dnx_data_pll_pll4_define_fab_pll4_id,

    
    _dnx_data_pll_pll4_define_nof
} dnx_data_pll_pll4_define_e;



uint32 dnx_data_pll_pll4_nof_pll4_get(
    int unit);


uint32 dnx_data_pll_pll4_ts_pll4_id_get(
    int unit);


uint32 dnx_data_pll_pll4_bs_pll4_id_get(
    int unit);


uint32 dnx_data_pll_pll4_nif_pll4_id_get(
    int unit);


uint32 dnx_data_pll_pll4_fab_pll4_id_get(
    int unit);



typedef enum
{
    dnx_data_pll_pll4_table_config,

    
    _dnx_data_pll_pll4_table_nof
} dnx_data_pll_pll4_table_e;



const dnx_data_pll_pll4_config_t * dnx_data_pll_pll4_config_get(
    int unit,
    int pll4_type);



shr_error_e dnx_data_pll_pll4_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pll_pll4_config_info_get(
    int unit);






int dnx_data_pll_synce_pll_feature_get(
    int unit,
    dnx_data_pll_synce_pll_feature_e feature);



typedef enum
{

    
    _dnx_data_pll_synce_pll_define_nof
} dnx_data_pll_synce_pll_define_e;




typedef enum
{

    
    _dnx_data_pll_synce_pll_table_nof
} dnx_data_pll_synce_pll_table_e;






shr_error_e dnx_data_pll_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

