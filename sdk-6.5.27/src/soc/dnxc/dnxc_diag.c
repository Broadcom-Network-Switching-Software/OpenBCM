/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT
#include <shared/bsl.h>

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_diag.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <sal/core/sync.h>
#include <soc/sand/shrextend/shrextend_debug.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif

#undef BSL_LOG_MODULE
