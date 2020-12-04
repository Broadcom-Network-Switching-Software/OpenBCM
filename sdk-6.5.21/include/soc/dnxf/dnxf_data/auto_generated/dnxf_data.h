

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNXF_DATA_H_

#define _DNXF_DATA_H_

#include <soc/property.h>
#include <sal/limits.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_dev_init.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_intr.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_intr.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_linkscan.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_linkscan.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_module_testing.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_module_testing.h>



#ifndef BCM_DNXF_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnxf_data_init(
    int unit);




shr_error_e dnxf_data_deinit(
    int unit);



shr_error_e dnxf_data_if_init(
    int unit);


shr_error_e dnxf_data_if_deinit(
    int unit);




#endif 

