
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_REGRESSION_H_

#define _DNX_DATA_INTERNAL_REGRESSION_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_regression.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_regression_submodule_dvapi_topology,
    dnx_data_regression_submodule_ser_parameters,
    dnx_data_regression_submodule_source_info,
    dnx_data_regression_submodule_access,

    
    _dnx_data_regression_submodule_nof
} dnx_data_regression_submodule_e;








int dnx_data_regression_dvapi_topology_feature_get(
    int unit,
    dnx_data_regression_dvapi_topology_feature_e feature);



typedef enum
{
    dnx_data_regression_dvapi_topology_define_nof_pm_ext_loops,
    dnx_data_regression_dvapi_topology_define_nof_pm_p2p_links,
    dnx_data_regression_dvapi_topology_define_nof_pm_force_er,

    
    _dnx_data_regression_dvapi_topology_define_nof
} dnx_data_regression_dvapi_topology_define_e;



uint32 dnx_data_regression_dvapi_topology_nof_pm_ext_loops_get(
    int unit);


uint32 dnx_data_regression_dvapi_topology_nof_pm_p2p_links_get(
    int unit);


uint32 dnx_data_regression_dvapi_topology_nof_pm_force_er_get(
    int unit);



typedef enum
{
    dnx_data_regression_dvapi_topology_table_pm_ext_loops,
    dnx_data_regression_dvapi_topology_table_pm_p2p_links,
    dnx_data_regression_dvapi_topology_table_pm_force_er,
    dnx_data_regression_dvapi_topology_table_pm_test_limitations,

    
    _dnx_data_regression_dvapi_topology_table_nof
} dnx_data_regression_dvapi_topology_table_e;



const dnx_data_regression_dvapi_topology_pm_ext_loops_t * dnx_data_regression_dvapi_topology_pm_ext_loops_get(
    int unit,
    int index);


const dnx_data_regression_dvapi_topology_pm_p2p_links_t * dnx_data_regression_dvapi_topology_pm_p2p_links_get(
    int unit,
    int index);


const dnx_data_regression_dvapi_topology_pm_force_er_t * dnx_data_regression_dvapi_topology_pm_force_er_get(
    int unit,
    int index);


const dnx_data_regression_dvapi_topology_pm_test_limitations_t * dnx_data_regression_dvapi_topology_pm_test_limitations_get(
    int unit,
    int pm_index);



shr_error_e dnx_data_regression_dvapi_topology_pm_ext_loops_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_dvapi_topology_pm_p2p_links_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_dvapi_topology_pm_force_er_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_dvapi_topology_pm_test_limitations_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_regression_dvapi_topology_pm_ext_loops_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_dvapi_topology_pm_p2p_links_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_dvapi_topology_pm_force_er_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_dvapi_topology_pm_test_limitations_info_get(
    int unit);






int dnx_data_regression_ser_parameters_feature_get(
    int unit,
    dnx_data_regression_ser_parameters_feature_e feature);



typedef enum
{
    dnx_data_regression_ser_parameters_define_cgm_voq_profiles,
    dnx_data_regression_ser_parameters_define_ecgm_qqst_table,
    dnx_data_regression_ser_parameters_define_ecgm_qdct_table,
    dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table,
    dnx_data_regression_ser_parameters_define_erpp_per_pp_port_table_2,
    dnx_data_regression_ser_parameters_define_mdb_ivsi,
    dnx_data_regression_ser_parameters_define_mdb_evsi,
    dnx_data_regression_ser_parameters_define_mdb_glem,
    dnx_data_regression_ser_parameters_define_mdb_exem_1,
    dnx_data_regression_ser_parameters_define_mdb_fec_1,

    
    _dnx_data_regression_ser_parameters_define_nof
} dnx_data_regression_ser_parameters_define_e;



uint32 dnx_data_regression_ser_parameters_cgm_voq_profiles_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_ecgm_qqst_table_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_ecgm_qdct_table_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_erpp_per_pp_port_table_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_erpp_per_pp_port_table_2_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_mdb_ivsi_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_mdb_evsi_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_mdb_glem_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_mdb_exem_1_get(
    int unit);


uint32 dnx_data_regression_ser_parameters_mdb_fec_1_get(
    int unit);



typedef enum
{
    dnx_data_regression_ser_parameters_table_cgm_ipp_map,
    dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms,
    dnx_data_regression_ser_parameters_table_cgm_voq_dram_bound_prms_2,
    dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms,
    dnx_data_regression_ser_parameters_table_cgm_voq_dram_recovery_prms_2,

    
    _dnx_data_regression_ser_parameters_table_nof
} dnx_data_regression_ser_parameters_table_e;



const dnx_data_regression_ser_parameters_cgm_ipp_map_t * dnx_data_regression_ser_parameters_cgm_ipp_map_get(
    int unit,
    int index);


const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t * dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_get(
    int unit,
    int index);


const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t * dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_get(
    int unit,
    int index);


const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t * dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_get(
    int unit,
    int index);


const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t * dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_get(
    int unit,
    int index);



shr_error_e dnx_data_regression_ser_parameters_cgm_ipp_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_regression_ser_parameters_cgm_ipp_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_info_get(
    int unit);






int dnx_data_regression_source_info_feature_get(
    int unit,
    dnx_data_regression_source_info_feature_e feature);



typedef enum
{

    
    _dnx_data_regression_source_info_define_nof
} dnx_data_regression_source_info_define_e;




typedef enum
{
    dnx_data_regression_source_info_table_tlist,

    
    _dnx_data_regression_source_info_table_nof
} dnx_data_regression_source_info_table_e;



const dnx_data_regression_source_info_tlist_t * dnx_data_regression_source_info_tlist_get(
    int unit,
    int index);



shr_error_e dnx_data_regression_source_info_tlist_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_regression_source_info_tlist_info_get(
    int unit);






int dnx_data_regression_access_feature_get(
    int unit,
    dnx_data_regression_access_feature_e feature);



typedef enum
{
    dnx_data_regression_access_define_access_mode,
    dnx_data_regression_access_define_severity_miss_check,

    
    _dnx_data_regression_access_define_nof
} dnx_data_regression_access_define_e;



uint32 dnx_data_regression_access_access_mode_get(
    int unit);


uint32 dnx_data_regression_access_severity_miss_check_get(
    int unit);



typedef enum
{

    
    _dnx_data_regression_access_table_nof
} dnx_data_regression_access_table_e;






shr_error_e dnx_data_regression_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

