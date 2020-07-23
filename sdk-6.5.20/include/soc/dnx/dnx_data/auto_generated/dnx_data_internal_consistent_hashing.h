

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_CONSISTENT_HASHING_H_

#define _DNX_DATA_INTERNAL_CONSISTENT_HASHING_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_consistent_hashing.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_consistent_hashing_submodule_calendar,

    
    _dnx_data_consistent_hashing_submodule_nof
} dnx_data_consistent_hashing_submodule_e;








int dnx_data_consistent_hashing_calendar_feature_get(
    int unit,
    dnx_data_consistent_hashing_calendar_feature_e feature);



typedef enum
{
    dnx_data_consistent_hashing_calendar_define_max_nof_entries_in_calendar,

    
    _dnx_data_consistent_hashing_calendar_define_nof
} dnx_data_consistent_hashing_calendar_define_e;



uint32 dnx_data_consistent_hashing_calendar_max_nof_entries_in_calendar_get(
    int unit);



typedef enum
{

    
    _dnx_data_consistent_hashing_calendar_table_nof
} dnx_data_consistent_hashing_calendar_table_e;






shr_error_e dnx_data_consistent_hashing_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

