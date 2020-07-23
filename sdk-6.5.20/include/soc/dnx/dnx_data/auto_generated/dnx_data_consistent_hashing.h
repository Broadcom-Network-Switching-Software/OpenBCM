

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_CONSISTENT_HASHING_H_

#define _DNX_DATA_CONSISTENT_HASHING_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_consistent_hashing.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_consistent_hashing_init(
    int unit);







typedef enum
{

    
    _dnx_data_consistent_hashing_calendar_feature_nof
} dnx_data_consistent_hashing_calendar_feature_e;



typedef int(
    *dnx_data_consistent_hashing_calendar_feature_get_f) (
    int unit,
    dnx_data_consistent_hashing_calendar_feature_e feature);


typedef uint32(
    *dnx_data_consistent_hashing_calendar_max_nof_entries_in_calendar_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_consistent_hashing_calendar_feature_get_f feature_get;
    
    dnx_data_consistent_hashing_calendar_max_nof_entries_in_calendar_get_f max_nof_entries_in_calendar_get;
} dnx_data_if_consistent_hashing_calendar_t;





typedef struct
{
    
    dnx_data_if_consistent_hashing_calendar_t calendar;
} dnx_data_if_consistent_hashing_t;




extern dnx_data_if_consistent_hashing_t dnx_data_consistent_hashing;


#endif 

