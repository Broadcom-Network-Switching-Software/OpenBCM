
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_REGRESSION_H_

#define _DNX_DATA_REGRESSION_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_regression.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_regression_init(
    int unit);






typedef struct
{
    uint32 pm_id;
} dnx_data_regression_dvapi_topology_pm_ext_loops_t;


typedef struct
{
    uint32 pm_id;
    uint32 peer_pm_id;
} dnx_data_regression_dvapi_topology_pm_p2p_links_t;


typedef struct
{
    uint32 pm_id;
} dnx_data_regression_dvapi_topology_pm_force_er_t;


typedef struct
{
    int max_speed;
} dnx_data_regression_dvapi_topology_pm_test_limitations_t;



typedef enum
{

    
    _dnx_data_regression_dvapi_topology_feature_nof
} dnx_data_regression_dvapi_topology_feature_e;



typedef int(
    *dnx_data_regression_dvapi_topology_feature_get_f) (
    int unit,
    dnx_data_regression_dvapi_topology_feature_e feature);


typedef uint32(
    *dnx_data_regression_dvapi_topology_nof_pm_ext_loops_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_dvapi_topology_nof_pm_p2p_links_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_dvapi_topology_nof_pm_force_er_get_f) (
    int unit);


typedef const dnx_data_regression_dvapi_topology_pm_ext_loops_t *(
    *dnx_data_regression_dvapi_topology_pm_ext_loops_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_dvapi_topology_pm_p2p_links_t *(
    *dnx_data_regression_dvapi_topology_pm_p2p_links_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_dvapi_topology_pm_force_er_t *(
    *dnx_data_regression_dvapi_topology_pm_force_er_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_dvapi_topology_pm_test_limitations_t *(
    *dnx_data_regression_dvapi_topology_pm_test_limitations_get_f) (
    int unit,
    int pm_index);



typedef struct
{
    
    dnx_data_regression_dvapi_topology_feature_get_f feature_get;
    
    dnx_data_regression_dvapi_topology_nof_pm_ext_loops_get_f nof_pm_ext_loops_get;
    
    dnx_data_regression_dvapi_topology_nof_pm_p2p_links_get_f nof_pm_p2p_links_get;
    
    dnx_data_regression_dvapi_topology_nof_pm_force_er_get_f nof_pm_force_er_get;
    
    dnx_data_regression_dvapi_topology_pm_ext_loops_get_f pm_ext_loops_get;
    
    dnxc_data_table_info_get_f pm_ext_loops_info_get;
    
    dnx_data_regression_dvapi_topology_pm_p2p_links_get_f pm_p2p_links_get;
    
    dnxc_data_table_info_get_f pm_p2p_links_info_get;
    
    dnx_data_regression_dvapi_topology_pm_force_er_get_f pm_force_er_get;
    
    dnxc_data_table_info_get_f pm_force_er_info_get;
    
    dnx_data_regression_dvapi_topology_pm_test_limitations_get_f pm_test_limitations_get;
    
    dnxc_data_table_info_get_f pm_test_limitations_info_get;
} dnx_data_if_regression_dvapi_topology_t;






typedef struct
{
    int entry_index;
} dnx_data_regression_ser_parameters_cgm_ipp_map_t;


typedef struct
{
    int entry_index;
} dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t;


typedef struct
{
    int entry_index;
} dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t;


typedef struct
{
    int entry_index;
} dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t;


typedef struct
{
    int entry_index;
} dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t;



typedef enum
{

    
    _dnx_data_regression_ser_parameters_feature_nof
} dnx_data_regression_ser_parameters_feature_e;



typedef int(
    *dnx_data_regression_ser_parameters_feature_get_f) (
    int unit,
    dnx_data_regression_ser_parameters_feature_e feature);


typedef uint32(
    *dnx_data_regression_ser_parameters_cgm_voq_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_ecgm_qqst_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_ecgm_qdct_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_erpp_per_pp_port_table_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_erpp_per_pp_port_table_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_mdb_ivsi_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_mdb_evsi_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_mdb_glem_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_mdb_exem_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_ser_parameters_mdb_fec_1_get_f) (
    int unit);


typedef const dnx_data_regression_ser_parameters_cgm_ipp_map_t *(
    *dnx_data_regression_ser_parameters_cgm_ipp_map_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_t *(
    *dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_t *(
    *dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_t *(
    *dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_t *(
    *dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_regression_ser_parameters_feature_get_f feature_get;
    
    dnx_data_regression_ser_parameters_cgm_voq_profiles_get_f cgm_voq_profiles_get;
    
    dnx_data_regression_ser_parameters_ecgm_qqst_table_get_f ecgm_qqst_table_get;
    
    dnx_data_regression_ser_parameters_ecgm_qdct_table_get_f ecgm_qdct_table_get;
    
    dnx_data_regression_ser_parameters_erpp_per_pp_port_table_get_f erpp_per_pp_port_table_get;
    
    dnx_data_regression_ser_parameters_erpp_per_pp_port_table_2_get_f erpp_per_pp_port_table_2_get;
    
    dnx_data_regression_ser_parameters_mdb_ivsi_get_f mdb_ivsi_get;
    
    dnx_data_regression_ser_parameters_mdb_evsi_get_f mdb_evsi_get;
    
    dnx_data_regression_ser_parameters_mdb_glem_get_f mdb_glem_get;
    
    dnx_data_regression_ser_parameters_mdb_exem_1_get_f mdb_exem_1_get;
    
    dnx_data_regression_ser_parameters_mdb_fec_1_get_f mdb_fec_1_get;
    
    dnx_data_regression_ser_parameters_cgm_ipp_map_get_f cgm_ipp_map_get;
    
    dnxc_data_table_info_get_f cgm_ipp_map_info_get;
    
    dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_get_f cgm_voq_dram_bound_prms_get;
    
    dnxc_data_table_info_get_f cgm_voq_dram_bound_prms_info_get;
    
    dnx_data_regression_ser_parameters_cgm_voq_dram_bound_prms_2_get_f cgm_voq_dram_bound_prms_2_get;
    
    dnxc_data_table_info_get_f cgm_voq_dram_bound_prms_2_info_get;
    
    dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_get_f cgm_voq_dram_recovery_prms_get;
    
    dnxc_data_table_info_get_f cgm_voq_dram_recovery_prms_info_get;
    
    dnx_data_regression_ser_parameters_cgm_voq_dram_recovery_prms_2_get_f cgm_voq_dram_recovery_prms_2_get;
    
    dnxc_data_table_info_get_f cgm_voq_dram_recovery_prms_2_info_get;
} dnx_data_if_regression_ser_parameters_t;






typedef struct
{
    char *tlist;
} dnx_data_regression_source_info_tlist_t;



typedef enum
{

    
    _dnx_data_regression_source_info_feature_nof
} dnx_data_regression_source_info_feature_e;



typedef int(
    *dnx_data_regression_source_info_feature_get_f) (
    int unit,
    dnx_data_regression_source_info_feature_e feature);


typedef const dnx_data_regression_source_info_tlist_t *(
    *dnx_data_regression_source_info_tlist_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_regression_source_info_feature_get_f feature_get;
    
    dnx_data_regression_source_info_tlist_get_f tlist_get;
    
    dnxc_data_table_info_get_f tlist_info_get;
} dnx_data_if_regression_source_info_t;







typedef enum
{

    
    _dnx_data_regression_access_feature_nof
} dnx_data_regression_access_feature_e;



typedef int(
    *dnx_data_regression_access_feature_get_f) (
    int unit,
    dnx_data_regression_access_feature_e feature);


typedef uint32(
    *dnx_data_regression_access_access_mode_get_f) (
    int unit);


typedef uint32(
    *dnx_data_regression_access_severity_miss_check_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_regression_access_feature_get_f feature_get;
    
    dnx_data_regression_access_access_mode_get_f access_mode_get;
    
    dnx_data_regression_access_severity_miss_check_get_f severity_miss_check_get;
} dnx_data_if_regression_access_t;





typedef struct
{
    
    dnx_data_if_regression_dvapi_topology_t dvapi_topology;
    
    dnx_data_if_regression_ser_parameters_t ser_parameters;
    
    dnx_data_if_regression_source_info_t source_info;
    
    dnx_data_if_regression_access_t access;
} dnx_data_if_regression_t;




extern dnx_data_if_regression_t dnx_data_regression;


#endif 

