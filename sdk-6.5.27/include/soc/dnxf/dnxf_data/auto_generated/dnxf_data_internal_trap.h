
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_TRAP_H_

#define _DNXF_DATA_INTERNAL_TRAP_H_

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_internal.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_trap.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



typedef enum
{
    dnxf_data_trap_submodule_etpp,

    
    _dnxf_data_trap_submodule_nof
} dnxf_data_trap_submodule_e;








int dnxf_data_trap_etpp_feature_get(
    int unit,
    dnxf_data_trap_etpp_feature_e feature);



typedef enum
{

    
    _dnxf_data_trap_etpp_define_nof
} dnxf_data_trap_etpp_define_e;




typedef enum
{

    
    _dnxf_data_trap_etpp_table_nof
} dnxf_data_trap_etpp_table_e;






shr_error_e dnxf_data_trap_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 

