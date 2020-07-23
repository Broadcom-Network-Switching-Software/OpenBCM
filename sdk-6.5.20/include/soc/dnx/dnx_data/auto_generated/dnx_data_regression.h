

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_REGRESSION_H_

#define _DNX_DATA_REGRESSION_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnxc/dnxc_port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_regression.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
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


typedef const dnx_data_regression_dvapi_topology_pm_ext_loops_t *(
    *dnx_data_regression_dvapi_topology_pm_ext_loops_get_f) (
    int unit,
    int index);


typedef const dnx_data_regression_dvapi_topology_pm_p2p_links_t *(
    *dnx_data_regression_dvapi_topology_pm_p2p_links_get_f) (
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
    
    dnx_data_regression_dvapi_topology_pm_ext_loops_get_f pm_ext_loops_get;
    
    dnxc_data_table_info_get_f pm_ext_loops_info_get;
    
    dnx_data_regression_dvapi_topology_pm_p2p_links_get_f pm_p2p_links_get;
    
    dnxc_data_table_info_get_f pm_p2p_links_info_get;
    
    dnx_data_regression_dvapi_topology_pm_test_limitations_get_f pm_test_limitations_get;
    
    dnxc_data_table_info_get_f pm_test_limitations_info_get;
} dnx_data_if_regression_dvapi_topology_t;





typedef struct
{
    
    dnx_data_if_regression_dvapi_topology_t dvapi_topology;
} dnx_data_if_regression_t;




extern dnx_data_if_regression_t dnx_data_regression;


#endif 

