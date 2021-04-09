
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FLOW_H_

#define _DNX_DATA_INTERNAL_FLOW_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_flow_submodule_special_fields,
    dnx_data_flow_submodule_ipv4,

    
    _dnx_data_flow_submodule_nof
} dnx_data_flow_submodule_e;








int dnx_data_flow_special_fields_feature_get(
    int unit,
    dnx_data_flow_special_fields_feature_e feature);



typedef enum
{

    
    _dnx_data_flow_special_fields_define_nof
} dnx_data_flow_special_fields_define_e;




typedef enum
{
    dnx_data_flow_special_fields_table_name_mapping,

    
    _dnx_data_flow_special_fields_table_nof
} dnx_data_flow_special_fields_table_e;



const dnx_data_flow_special_fields_name_mapping_t * dnx_data_flow_special_fields_name_mapping_get(
    int unit,
    int field_id);



shr_error_e dnx_data_flow_special_fields_name_mapping_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_flow_special_fields_name_mapping_info_get(
    int unit);






int dnx_data_flow_ipv4_feature_get(
    int unit,
    dnx_data_flow_ipv4_feature_e feature);



typedef enum
{
    dnx_data_flow_ipv4_define_use_flow_lif_init,
    dnx_data_flow_ipv4_define_use_flow_lif_term,

    
    _dnx_data_flow_ipv4_define_nof
} dnx_data_flow_ipv4_define_e;



uint32 dnx_data_flow_ipv4_use_flow_lif_init_get(
    int unit);


uint32 dnx_data_flow_ipv4_use_flow_lif_term_get(
    int unit);



typedef enum
{

    
    _dnx_data_flow_ipv4_table_nof
} dnx_data_flow_ipv4_table_e;






shr_error_e dnx_data_flow_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

