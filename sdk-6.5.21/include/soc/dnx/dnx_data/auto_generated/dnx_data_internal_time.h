

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TIME_H_

#define _DNX_DATA_INTERNAL_TIME_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_time.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_time_submodule_timesync,

    
    _dnx_data_time_submodule_nof
} dnx_data_time_submodule_e;








int dnx_data_time_timesync_feature_get(
    int unit,
    dnx_data_time_timesync_feature_e feature);



typedef enum
{

    
    _dnx_data_time_timesync_define_nof
} dnx_data_time_timesync_define_e;




typedef enum
{

    
    _dnx_data_time_timesync_table_nof
} dnx_data_time_timesync_table_e;






shr_error_e dnx_data_time_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

