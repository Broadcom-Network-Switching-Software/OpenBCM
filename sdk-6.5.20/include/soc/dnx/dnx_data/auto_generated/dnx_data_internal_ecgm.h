

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_ECGM_H_

#define _DNX_DATA_INTERNAL_ECGM_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ecgm.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_ecgm_submodule_core_resources,
    dnx_data_ecgm_submodule_port_resources,
    dnx_data_ecgm_submodule_delete_fifo,
    dnx_data_ecgm_submodule_info,

    
    _dnx_data_ecgm_submodule_nof
} dnx_data_ecgm_submodule_e;








int dnx_data_ecgm_core_resources_feature_get(
    int unit,
    dnx_data_ecgm_core_resources_feature_e feature);



typedef enum
{
    dnx_data_ecgm_core_resources_define_nof_sp,
    dnx_data_ecgm_core_resources_define_nof_interface_profiles,
    dnx_data_ecgm_core_resources_define_nof_port_profiles,
    dnx_data_ecgm_core_resources_define_total_pds,
    dnx_data_ecgm_core_resources_define_total_pds_nof_bits,
    dnx_data_ecgm_core_resources_define_total_dbs,
    dnx_data_ecgm_core_resources_define_total_dbs_nof_bits,
    dnx_data_ecgm_core_resources_define_max_alpha_value,
    dnx_data_ecgm_core_resources_define_min_alpha_value,
    dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps,

    
    _dnx_data_ecgm_core_resources_define_nof
} dnx_data_ecgm_core_resources_define_e;



uint32 dnx_data_ecgm_core_resources_nof_sp_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_nof_interface_profiles_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_nof_port_profiles_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_total_pds_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_total_pds_nof_bits_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_total_dbs_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_total_dbs_nof_bits_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_max_alpha_value_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_min_alpha_value_get(
    int unit);


uint32 dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_get(
    int unit);



typedef enum
{

    
    _dnx_data_ecgm_core_resources_table_nof
} dnx_data_ecgm_core_resources_table_e;









int dnx_data_ecgm_port_resources_feature_get(
    int unit,
    dnx_data_ecgm_port_resources_feature_e feature);



typedef enum
{
    dnx_data_ecgm_port_resources_define_max_nof_ports,

    
    _dnx_data_ecgm_port_resources_define_nof
} dnx_data_ecgm_port_resources_define_e;



uint32 dnx_data_ecgm_port_resources_max_nof_ports_get(
    int unit);



typedef enum
{

    
    _dnx_data_ecgm_port_resources_table_nof
} dnx_data_ecgm_port_resources_table_e;









int dnx_data_ecgm_delete_fifo_feature_get(
    int unit,
    dnx_data_ecgm_delete_fifo_feature_e feature);



typedef enum
{

    
    _dnx_data_ecgm_delete_fifo_define_nof
} dnx_data_ecgm_delete_fifo_define_e;




typedef enum
{
    dnx_data_ecgm_delete_fifo_table_thresholds,

    
    _dnx_data_ecgm_delete_fifo_table_nof
} dnx_data_ecgm_delete_fifo_table_e;



const dnx_data_ecgm_delete_fifo_thresholds_t * dnx_data_ecgm_delete_fifo_thresholds_get(
    int unit,
    int fifo_type);



shr_error_e dnx_data_ecgm_delete_fifo_thresholds_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ecgm_delete_fifo_thresholds_info_get(
    int unit);






int dnx_data_ecgm_info_feature_get(
    int unit,
    dnx_data_ecgm_info_feature_e feature);



typedef enum
{
    dnx_data_ecgm_info_define_nof_dropped_reasons_rqp,
    dnx_data_ecgm_info_define_nof_dropped_reasons_pqp,
    dnx_data_ecgm_info_define_nof_bits_in_pd_count,

    
    _dnx_data_ecgm_info_define_nof
} dnx_data_ecgm_info_define_e;



uint32 dnx_data_ecgm_info_nof_dropped_reasons_rqp_get(
    int unit);


uint32 dnx_data_ecgm_info_nof_dropped_reasons_pqp_get(
    int unit);


uint32 dnx_data_ecgm_info_nof_bits_in_pd_count_get(
    int unit);



typedef enum
{
    dnx_data_ecgm_info_table_dropped_reason_rqp,
    dnx_data_ecgm_info_table_dropped_reason_pqp,

    
    _dnx_data_ecgm_info_table_nof
} dnx_data_ecgm_info_table_e;



const dnx_data_ecgm_info_dropped_reason_rqp_t * dnx_data_ecgm_info_dropped_reason_rqp_get(
    int unit,
    int index);


const dnx_data_ecgm_info_dropped_reason_pqp_t * dnx_data_ecgm_info_dropped_reason_pqp_get(
    int unit,
    int index);



shr_error_e dnx_data_ecgm_info_dropped_reason_rqp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_ecgm_info_dropped_reason_pqp_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ecgm_info_dropped_reason_rqp_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_ecgm_info_dropped_reason_pqp_info_get(
    int unit);



shr_error_e dnx_data_ecgm_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

