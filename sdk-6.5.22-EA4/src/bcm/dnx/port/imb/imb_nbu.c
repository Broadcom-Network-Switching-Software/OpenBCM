
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#include <bcm/port.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_internal.h>
#include <soc/portmod/portmod.h>
#include <bcm_int/dnx/port/imb/imb_nbu.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/util.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

int
imb_nbu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;

}

int
imb_nbu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;

}

int
imb_nbu_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Place your code here 
     */

    SHR_FUNC_EXIT;

}

int
imb_nbu_port_detach(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Place your code here 
     */

    SHR_FUNC_EXIT;

}
