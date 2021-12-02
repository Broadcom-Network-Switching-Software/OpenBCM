
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ADAPTER_H_

#define _DNX_DATA_INTERNAL_ADAPTER_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_adapter_submodule_tx,
    dnx_data_adapter_submodule_rx,
    dnx_data_adapter_submodule_general,
    dnx_data_adapter_submodule_reg_mem_access,
    dnx_data_adapter_submodule_mdb,

    
    _dnx_data_adapter_submodule_nof
} dnx_data_adapter_submodule_e;








int dnx_data_adapter_tx_feature_get(
    int unit,
    dnx_data_adapter_tx_feature_e feature);



typedef enum
{
    dnx_data_adapter_tx_define_loopback_enable,

    
    _dnx_data_adapter_tx_define_nof
} dnx_data_adapter_tx_define_e;



uint32 dnx_data_adapter_tx_loopback_enable_get(
    int unit);



typedef enum
{

    
    _dnx_data_adapter_tx_table_nof
} dnx_data_adapter_tx_table_e;









int dnx_data_adapter_rx_feature_get(
    int unit,
    dnx_data_adapter_rx_feature_e feature);



typedef enum
{
    dnx_data_adapter_rx_define_constant_header_size,

    
    _dnx_data_adapter_rx_define_nof
} dnx_data_adapter_rx_define_e;



uint32 dnx_data_adapter_rx_constant_header_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_adapter_rx_table_nof
} dnx_data_adapter_rx_table_e;









int dnx_data_adapter_general_feature_get(
    int unit,
    dnx_data_adapter_general_feature_e feature);



typedef enum
{
    dnx_data_adapter_general_define_nof_sub_units,
    dnx_data_adapter_general_define_lib_ver,
    dnx_data_adapter_general_define_oamp_ms_id,

    
    _dnx_data_adapter_general_define_nof
} dnx_data_adapter_general_define_e;



uint32 dnx_data_adapter_general_nof_sub_units_get(
    int unit);


uint32 dnx_data_adapter_general_lib_ver_get(
    int unit);


uint32 dnx_data_adapter_general_oamp_ms_id_get(
    int unit);



typedef enum
{
    dnx_data_adapter_general_table_Injection,

    
    _dnx_data_adapter_general_table_nof
} dnx_data_adapter_general_table_e;



const dnx_data_adapter_general_Injection_t * dnx_data_adapter_general_Injection_get(
    int unit);



shr_error_e dnx_data_adapter_general_Injection_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_adapter_general_Injection_info_get(
    int unit);






int dnx_data_adapter_reg_mem_access_feature_get(
    int unit,
    dnx_data_adapter_reg_mem_access_feature_e feature);



typedef enum
{
    dnx_data_adapter_reg_mem_access_define_do_collect_enable,
    dnx_data_adapter_reg_mem_access_define_cmic_block_index,
    dnx_data_adapter_reg_mem_access_define_iproc_block_index,
    dnx_data_adapter_reg_mem_access_define_swap_core_index_zero_with_core_index,

    
    _dnx_data_adapter_reg_mem_access_define_nof
} dnx_data_adapter_reg_mem_access_define_e;



uint32 dnx_data_adapter_reg_mem_access_do_collect_enable_get(
    int unit);


uint32 dnx_data_adapter_reg_mem_access_cmic_block_index_get(
    int unit);


uint32 dnx_data_adapter_reg_mem_access_iproc_block_index_get(
    int unit);


uint32 dnx_data_adapter_reg_mem_access_swap_core_index_zero_with_core_index_get(
    int unit);



typedef enum
{
    dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map,
    dnx_data_adapter_reg_mem_access_table_swap_core_ignore_map_by_global_block_ids_enum,
    dnx_data_adapter_reg_mem_access_table_clear_on_read_mems,
    dnx_data_adapter_reg_mem_access_table_clear_on_read_regs,

    
    _dnx_data_adapter_reg_mem_access_table_nof
} dnx_data_adapter_reg_mem_access_table_e;



const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_t * dnx_data_adapter_reg_mem_access_swap_core_ignore_map_get(
    int unit,
    int block_type);


const dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_t * dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_get(
    int unit,
    int block_type);


const dnx_data_adapter_reg_mem_access_clear_on_read_mems_t * dnx_data_adapter_reg_mem_access_clear_on_read_mems_get(
    int unit,
    int index);


const dnx_data_adapter_reg_mem_access_clear_on_read_regs_t * dnx_data_adapter_reg_mem_access_clear_on_read_regs_get(
    int unit,
    int index);



shr_error_e dnx_data_adapter_reg_mem_access_swap_core_ignore_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_adapter_reg_mem_access_clear_on_read_mems_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_adapter_reg_mem_access_clear_on_read_regs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_adapter_reg_mem_access_swap_core_ignore_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_adapter_reg_mem_access_swap_core_ignore_map_by_global_block_ids_enum_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_adapter_reg_mem_access_clear_on_read_mems_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_adapter_reg_mem_access_clear_on_read_regs_info_get(
    int unit);






int dnx_data_adapter_mdb_feature_get(
    int unit,
    dnx_data_adapter_mdb_feature_e feature);



typedef enum
{

    
    _dnx_data_adapter_mdb_define_nof
} dnx_data_adapter_mdb_define_e;




typedef enum
{
    dnx_data_adapter_mdb_table_lookup_caller_id_mapping,

    
    _dnx_data_adapter_mdb_table_nof
} dnx_data_adapter_mdb_table_e;



const dnx_data_adapter_mdb_lookup_caller_id_mapping_t * dnx_data_adapter_mdb_lookup_caller_id_mapping_get(
    int unit,
    int caller_id,
    int stage_mdb_key);



shr_error_e dnx_data_adapter_mdb_lookup_caller_id_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_adapter_mdb_lookup_caller_id_mapping_info_get(
    int unit);



shr_error_e dnx_data_adapter_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

