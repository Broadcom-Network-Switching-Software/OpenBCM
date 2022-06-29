
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_KLEAP_STAGE_INFO_H_

#define _DNX_DATA_KLEAP_STAGE_INFO_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <src/appl/diag/dnx/pp/kleap/diag_dnx_ikleap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_kleap_stage_info.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_kleap_stage_info_init(
    int unit);






typedef struct
{
    char *stage_name;
    int type;
    int nof_sub_stages;
    int nof_kbrs;
    int nof_ffc;
    int nof_ffc_g;
    int nof_pd;
    int dbal_table_resource_mapping;
    int dbal_table_kbr_info;
    int dbal_table_ffc_instruction;
    int dbal_table_ffc_quad_is_acl;
    int dbal_table_pd_info;
    int dbal_kbr_idx_field_per_stage;
    int dbal_context_profile_field_per_stage;
    int dbal_ffc_instruction_field_per_stage;
    int dbal_litrally_instruction_field_per_stage;
    int dbal_relative_header_instruction_field_per_stage;
    int dbal_header_instruction_field_per_stage;
    int dbal_record_instruction_field_per_stage;
} dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t;


typedef struct
{
    int nof_kbrs;
    int kbr2physical[19];
    int kbr2key_sig[19];
} dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t;


typedef struct
{
    int pd_mapping_type;
    char *pd_mapping_name;
} dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t;


typedef struct
{
    char *kleap_sub_stage_name;
    int stage_index_in_pipe;
    int dbal_context_enum;
    int dbal_stage_context_id;
    int dbal_stage_context_properties;
    char *literally_signal_structure_name;
} dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t;



typedef enum
{

    
    _dnx_data_kleap_stage_info_kleap_stage_info_feature_nof
} dnx_data_kleap_stage_info_kleap_stage_info_feature_e;



typedef int(
    *dnx_data_kleap_stage_info_kleap_stage_info_feature_get_f) (
    int unit,
    dnx_data_kleap_stage_info_kleap_stage_info_feature_e feature);


typedef uint32(
    *dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_get_f) (
    int unit);


typedef const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *(
    *dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_get_f) (
    int unit,
    int stage_index);


typedef const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *(
    *dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_get_f) (
    int unit,
    int stage_index);


typedef const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *(
    *dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_get_f) (
    int unit,
    int stage_index,
    int pd_index);


typedef const dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *(
    *dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_get_f) (
    int unit,
    int stage_index,
    int sub_stage_index);



typedef struct
{
    
    dnx_data_kleap_stage_info_kleap_stage_info_feature_get_f feature_get;
    
    dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_get_f nof_kleap_stages_get;
    
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_get_f info_per_stage_get;
    
    dnxc_data_table_info_get_f info_per_stage_info_get;
    
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_get_f info_per_stage_per_kbr_get;
    
    dnxc_data_table_info_get_f info_per_stage_per_kbr_info_get;
    
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_get_f info_per_stage_per_pd_get;
    
    dnxc_data_table_info_get_f info_per_stage_per_pd_info_get;
    
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_get_f info_per_stage_per_sub_stage_get;
    
    dnxc_data_table_info_get_f info_per_stage_per_sub_stage_info_get;
} dnx_data_if_kleap_stage_info_kleap_stage_info_t;





typedef struct
{
    
    dnx_data_if_kleap_stage_info_kleap_stage_info_t kleap_stage_info;
} dnx_data_if_kleap_stage_info_t;




extern dnx_data_if_kleap_stage_info_t dnx_data_kleap_stage_info;


#endif 

