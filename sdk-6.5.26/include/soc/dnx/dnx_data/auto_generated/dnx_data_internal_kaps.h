
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_KAPS_H_

#define _DNX_DATA_INTERNAL_KAPS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_kaps.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_kaps_submodule_flist,

    
    _dnx_data_kaps_submodule_nof
} dnx_data_kaps_submodule_e;








int dnx_data_kaps_flist_feature_get(
    int unit,
    dnx_data_kaps_flist_feature_e feature);



typedef enum
{

    
    _dnx_data_kaps_flist_define_nof
} dnx_data_kaps_flist_define_e;




typedef enum
{

    
    _dnx_data_kaps_flist_table_nof
} dnx_data_kaps_flist_table_e;






shr_error_e dnx_data_kaps_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

