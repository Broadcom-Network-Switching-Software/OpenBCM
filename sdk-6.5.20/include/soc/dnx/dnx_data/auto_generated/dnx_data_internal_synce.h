

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SYNCE_H_

#define _DNX_DATA_INTERNAL_SYNCE_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_synce.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_synce_submodule_general,

    
    _dnx_data_synce_submodule_nof
} dnx_data_synce_submodule_e;








int dnx_data_synce_general_feature_get(
    int unit,
    dnx_data_synce_general_feature_e feature);



typedef enum
{
    dnx_data_synce_general_define_nof_plls,
    dnx_data_synce_general_define_nif_div_min,
    dnx_data_synce_general_define_nif_div_max,
    dnx_data_synce_general_define_fabric_div_min,
    dnx_data_synce_general_define_fabric_div_max,
    dnx_data_synce_general_define_nof_clk_blocks,
    dnx_data_synce_general_define_nof_fabric_clk_blocks,
    dnx_data_synce_general_define_output_clk_mode,

    
    _dnx_data_synce_general_define_nof
} dnx_data_synce_general_define_e;



uint32 dnx_data_synce_general_nof_plls_get(
    int unit);


uint32 dnx_data_synce_general_nif_div_min_get(
    int unit);


uint32 dnx_data_synce_general_nif_div_max_get(
    int unit);


uint32 dnx_data_synce_general_fabric_div_min_get(
    int unit);


uint32 dnx_data_synce_general_fabric_div_max_get(
    int unit);


uint32 dnx_data_synce_general_nof_clk_blocks_get(
    int unit);


uint32 dnx_data_synce_general_nof_fabric_clk_blocks_get(
    int unit);


uint32 dnx_data_synce_general_output_clk_mode_get(
    int unit);



typedef enum
{
    dnx_data_synce_general_table_cfg,
    dnx_data_synce_general_table_nmg_chain_map,

    
    _dnx_data_synce_general_table_nof
} dnx_data_synce_general_table_e;



const dnx_data_synce_general_cfg_t * dnx_data_synce_general_cfg_get(
    int unit,
    int synce_index);


const dnx_data_synce_general_nmg_chain_map_t * dnx_data_synce_general_nmg_chain_map_get(
    int unit,
    int ethu_index);



shr_error_e dnx_data_synce_general_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_synce_general_nmg_chain_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_synce_general_cfg_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_synce_general_nmg_chain_map_info_get(
    int unit);



shr_error_e dnx_data_synce_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

