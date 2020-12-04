/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN IPROC
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif 

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef BCM_IPROC_SUPPORT
    #include <soc/iproc.h>
#endif 

#include <soc/drv.h>
#include <soc/dcmn/dcmn_iproc.h>
#include <soc/dcmn/error.h>

#ifdef BCM_IPROC_SUPPORT


int soc_dcmn_iproc_config_paxb(int unit) 
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_FUNC_RETURN;
}

#endif 
