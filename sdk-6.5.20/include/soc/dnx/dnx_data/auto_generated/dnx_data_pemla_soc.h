

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PEMLA_SOC_H_

#define _DNX_DATA_PEMLA_SOC_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pemla_soc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_pemla_soc_init(
    int unit);







typedef enum
{

    
    _dnx_data_pemla_soc_soc_feature_nof
} dnx_data_pemla_soc_soc_feature_e;



typedef int(
    *dnx_data_pemla_soc_soc_feature_get_f) (
    int unit,
    dnx_data_pemla_soc_soc_feature_e feature);


typedef uint32(
    *dnx_data_pemla_soc_soc_JR1_MODE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pemla_soc_soc_JR2_MODE_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pemla_soc_soc_system_headers_mode_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_pemla_soc_soc_feature_get_f feature_get;
    
    dnx_data_pemla_soc_soc_JR1_MODE_get_f JR1_MODE_get;
    
    dnx_data_pemla_soc_soc_JR2_MODE_get_f JR2_MODE_get;
    
    dnx_data_pemla_soc_soc_system_headers_mode_get_f system_headers_mode_get;
} dnx_data_if_pemla_soc_soc_t;





typedef struct
{
    
    dnx_data_if_pemla_soc_soc_t soc;
} dnx_data_if_pemla_soc_t;




extern dnx_data_if_pemla_soc_t dnx_data_pemla_soc;


#endif 

