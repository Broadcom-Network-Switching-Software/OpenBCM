
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_H_

#define _DNXF_DATA_INTERNAL_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_dyn_loader.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNXF family only!"
#endif



typedef enum
{
    dnxf_data_module_dev_init,
    dnxf_data_module_device,
    dnxf_data_module_fabric,
    dnxf_data_module_intr,
    dnxf_data_module_linkscan,
    dnxf_data_module_module_testing,
    dnxf_data_module_port,
    dnxf_data_module_trap,

    
    _dnxf_data_module_nof
} dnxf_data_module_e;




#endif 

