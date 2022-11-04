
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_TRAP_H_

#define _DNXF_DATA_TRAP_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_trap.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



shr_error_e dnxf_data_if_trap_init(
    int unit);







typedef enum
{
    dnxf_data_trap_etpp_mtu_layer_to_compressed_layer_mapping_support,

    
    _dnxf_data_trap_etpp_feature_nof
} dnxf_data_trap_etpp_feature_e;



typedef int(
    *dnxf_data_trap_etpp_feature_get_f) (
    int unit,
    dnxf_data_trap_etpp_feature_e feature);



typedef struct
{
    
    dnxf_data_trap_etpp_feature_get_f feature_get;
} dnxf_data_if_trap_etpp_t;





typedef struct
{
    
    dnxf_data_if_trap_etpp_t etpp;
} dnxf_data_if_trap_t;




extern dnxf_data_if_trap_t dnxf_data_trap;


#endif 

