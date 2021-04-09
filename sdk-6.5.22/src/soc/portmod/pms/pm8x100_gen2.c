/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#include <soc/types.h>
#include <soc/error.h>
#include <soc/portmod/portmod_internal.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_dispatch.h>


        
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef PORTMOD_PM8X100_GEN2_SUPPORT

int pm8x100_gen2_port_attach(int unit, int port, pm_info_t pm_info, const portmod_port_add_info_t* add_info)
{        
    SOC_INIT_FUNC_DEFS;
    
    /* Place your code here */

    SOC_FUNC_RETURN; 
    
}


#endif /* PORTMOD_PM8X100_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
