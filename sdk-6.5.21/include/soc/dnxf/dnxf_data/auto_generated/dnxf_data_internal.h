

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_INTERNAL_H_

#define _DNXF_DATA_INTERNAL_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_dyn_loader.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnxf_data_module_port,
    dnxf_data_module_dev_init,
    dnxf_data_module_device,
    dnxf_data_module_fabric,
    dnxf_data_module_intr,
    dnxf_data_module_linkscan,
    dnxf_data_module_module_testing,

    
    _dnxf_data_module_nof
} dnxf_data_module_e;




#endif 

