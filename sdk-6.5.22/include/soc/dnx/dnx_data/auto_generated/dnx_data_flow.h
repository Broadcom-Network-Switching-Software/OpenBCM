
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FLOW_H_

#define _DNX_DATA_FLOW_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_flow.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_flow_init(
    int unit);






typedef struct
{
    
    char *field_name;
} dnx_data_flow_special_fields_name_mapping_t;



typedef enum
{

    
    _dnx_data_flow_special_fields_feature_nof
} dnx_data_flow_special_fields_feature_e;



typedef int(
    *dnx_data_flow_special_fields_feature_get_f) (
    int unit,
    dnx_data_flow_special_fields_feature_e feature);


typedef const dnx_data_flow_special_fields_name_mapping_t *(
    *dnx_data_flow_special_fields_name_mapping_get_f) (
    int unit,
    int field_id);



typedef struct
{
    
    dnx_data_flow_special_fields_feature_get_f feature_get;
    
    dnx_data_flow_special_fields_name_mapping_get_f name_mapping_get;
    
    dnxc_data_table_info_get_f name_mapping_info_get;
} dnx_data_if_flow_special_fields_t;







typedef enum
{

    
    _dnx_data_flow_ipv4_feature_nof
} dnx_data_flow_ipv4_feature_e;



typedef int(
    *dnx_data_flow_ipv4_feature_get_f) (
    int unit,
    dnx_data_flow_ipv4_feature_e feature);


typedef uint32(
    *dnx_data_flow_ipv4_use_flow_lif_init_get_f) (
    int unit);


typedef uint32(
    *dnx_data_flow_ipv4_use_flow_lif_term_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_flow_ipv4_feature_get_f feature_get;
    
    dnx_data_flow_ipv4_use_flow_lif_init_get_f use_flow_lif_init_get;
    
    dnx_data_flow_ipv4_use_flow_lif_term_get_f use_flow_lif_term_get;
} dnx_data_if_flow_ipv4_t;





typedef struct
{
    
    dnx_data_if_flow_special_fields_t special_fields;
    
    dnx_data_if_flow_ipv4_t ipv4;
} dnx_data_if_flow_t;




extern dnx_data_if_flow_t dnx_data_flow;


#endif 

