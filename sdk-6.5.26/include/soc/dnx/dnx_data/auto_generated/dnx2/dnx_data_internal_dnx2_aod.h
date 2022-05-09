
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DNX2_AOD_H_

#define _DNX_DATA_INTERNAL_DNX2_AOD_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_dnx2_aod.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_dnx2_aod_submodule_tables,

    
    _dnx_data_dnx2_aod_submodule_nof
} dnx_data_dnx2_aod_submodule_e;








int dnx_data_dnx2_aod_tables_feature_get(
    int unit,
    dnx_data_dnx2_aod_tables_feature_e feature);



typedef enum
{

    
    _dnx_data_dnx2_aod_tables_define_nof
} dnx_data_dnx2_aod_tables_define_e;




typedef enum
{
    dnx_data_dnx2_aod_tables_table_ERPP_FWD_CONTEXT_FILTER___image,
    dnx_data_dnx2_aod_tables_table_ERPP_FWD_CONTEXT_FILTER,
    dnx_data_dnx2_aod_tables_table_EGRESS_PER_FORWARD_CODE___image,
    dnx_data_dnx2_aod_tables_table_EGRESS_PER_FORWARD_CODE,
    dnx_data_dnx2_aod_tables_table_ERPP_FORWARD_CODE_SELECTION___image,
    dnx_data_dnx2_aod_tables_table_ERPP_FORWARD_CODE_SELECTION,
    dnx_data_dnx2_aod_tables_table_ETPP_FORWARD_CODE_SELECTION___image,
    dnx_data_dnx2_aod_tables_table_ETPP_FORWARD_CODE_SELECTION,
    dnx_data_dnx2_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT___image,
    dnx_data_dnx2_aod_tables_table_EGRESS_PARSING_START_TYPE_CONTEXT,
    dnx_data_dnx2_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image,
    dnx_data_dnx2_aod_tables_table_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE,
    dnx_data_dnx2_aod_tables_table_FER_FILTERS_PER_FWD_CONTEXT___image,
    dnx_data_dnx2_aod_tables_table_FER_FILTERS_PER_FWD_CONTEXT,
    dnx_data_dnx2_aod_tables_table_INGRESS_PRT_INFO___image,
    dnx_data_dnx2_aod_tables_table_INGRESS_PRT_INFO,
    dnx_data_dnx2_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_MAP___image,
    dnx_data_dnx2_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_MAP,
    dnx_data_dnx2_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image,
    dnx_data_dnx2_aod_tables_table_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES,

    
    _dnx_data_dnx2_aod_tables_table_nof
} dnx_data_dnx2_aod_tables_table_e;



const dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_t * dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER_t * dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER_get(
    int unit,
    int index);


const dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE___image_t * dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE_t * dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE_get(
    int unit,
    int index,
    int soc_value);


const dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_t * dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION_t * dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION_get(
    int unit,
    int index,
    int soc_value);


const dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_t * dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION_t * dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION_get(
    int unit,
    int index,
    int soc_value);


const dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_get(
    int unit,
    int index,
    int soc_value);


const dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_get(
    int unit,
    int index,
    int soc_value);


const dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT___image_t * dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT_t * dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT_get(
    int unit,
    int index);


const dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO___image_t * dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO_t * dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO_get(
    int unit,
    int index);


const dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP___image_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP_get(
    int unit,
    int index);


const dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_get(
    int unit);


const dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_get(
    int unit,
    int index);



shr_error_e dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_ERPP_FWD_CONTEXT_FILTER_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_EGRESS_PER_FORWARD_CODE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_ERPP_FORWARD_CODE_SELECTION_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_ETPP_FORWARD_CODE_SELECTION_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_START_TYPE_CONTEXT_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_EGRESS_PARSING_CONTEXT_MAP_JR1_MODE_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_FER_FILTERS_PER_FWD_CONTEXT_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_INGRESS_PRT_INFO_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_MAP_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES___image_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_dnx2_aod_tables_MPLS_SPECIAL_LABEL_PROFILE_ATTRIBUTES_info_get(
    int unit);



shr_error_e dnx_data_dnx2_aod_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

