/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXC IPROC
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif 

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif 

#include <soc/drv.h>
#include <soc/dnxc/dnxc_iproc.h>
#include <soc/dnxc/error.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_IPROC_SUPPORT


int
soc_dnxc_iproc_config_paxb(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif 
