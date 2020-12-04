

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DBAL_H_

#define _DNX_DATA_INTERNAL_DBAL_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_dbal_submodule_diag,
    dnx_data_dbal_submodule_db_init,
    dnx_data_dbal_submodule_table,
    dnx_data_dbal_submodule_hw_ent,

    
    _dnx_data_dbal_submodule_nof
} dnx_data_dbal_submodule_e;








int dnx_data_dbal_diag_feature_get(
    int unit,
    dnx_data_dbal_diag_feature_e feature);



typedef enum
{

    
    _dnx_data_dbal_diag_define_nof
} dnx_data_dbal_diag_define_e;




typedef enum
{
    dnx_data_dbal_diag_table_loggerInfo,

    
    _dnx_data_dbal_diag_table_nof
} dnx_data_dbal_diag_table_e;



const dnx_data_dbal_diag_loggerInfo_t * dnx_data_dbal_diag_loggerInfo_get(
    int unit);



shr_error_e dnx_data_dbal_diag_loggerInfo_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dbal_diag_loggerInfo_info_get(
    int unit);






int dnx_data_dbal_db_init_feature_get(
    int unit,
    dnx_data_dbal_db_init_feature_e feature);



typedef enum
{
    dnx_data_dbal_db_init_define_dbal_device_state,
    dnx_data_dbal_db_init_define_run_ltt_after_wm_test,
    dnx_data_dbal_db_init_define_dbal_collection_pool_size,

    
    _dnx_data_dbal_db_init_define_nof
} dnx_data_dbal_db_init_define_e;



uint32 dnx_data_dbal_db_init_dbal_device_state_get(
    int unit);


uint32 dnx_data_dbal_db_init_run_ltt_after_wm_test_get(
    int unit);


uint32 dnx_data_dbal_db_init_dbal_collection_pool_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_dbal_db_init_table_nof
} dnx_data_dbal_db_init_table_e;









int dnx_data_dbal_table_feature_get(
    int unit,
    dnx_data_dbal_table_feature_e feature);



typedef enum
{
    dnx_data_dbal_table_define_nof_dynamic_tables,
    dnx_data_dbal_table_define_nof_dynamic_xml_tables,
    dnx_data_dbal_table_define_nof_dynamic_tables_labels,
    dnx_data_dbal_table_define_nof_dynamic_tables_key_fields,
    dnx_data_dbal_table_define_nof_dynamic_tables_result_fields,
    dnx_data_dbal_table_define_nof_dynamic_tables_multi_result_types,
    dnx_data_dbal_table_define_nof_dynamic_tables_ppmc_multi_result_types,
    dnx_data_dbal_table_define_nof_dynamic_and_static_tables,

    
    _dnx_data_dbal_table_define_nof
} dnx_data_dbal_table_define_e;



uint32 dnx_data_dbal_table_nof_dynamic_tables_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_xml_tables_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_tables_labels_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_tables_key_fields_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_tables_result_fields_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_tables_multi_result_types_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_tables_ppmc_multi_result_types_get(
    int unit);


uint32 dnx_data_dbal_table_nof_dynamic_and_static_tables_get(
    int unit);



typedef enum
{

    
    _dnx_data_dbal_table_table_nof
} dnx_data_dbal_table_table_e;









int dnx_data_dbal_hw_ent_feature_get(
    int unit,
    dnx_data_dbal_hw_ent_feature_e feature);



typedef enum
{
    dnx_data_dbal_hw_ent_define_nof_direct_maps,
    dnx_data_dbal_hw_ent_define_nof_groups_maps,

    
    _dnx_data_dbal_hw_ent_define_nof
} dnx_data_dbal_hw_ent_define_e;



uint32 dnx_data_dbal_hw_ent_nof_direct_maps_get(
    int unit);


uint32 dnx_data_dbal_hw_ent_nof_groups_maps_get(
    int unit);



typedef enum
{

    
    _dnx_data_dbal_hw_ent_table_nof
} dnx_data_dbal_hw_ent_table_e;






shr_error_e dnx_data_dbal_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

