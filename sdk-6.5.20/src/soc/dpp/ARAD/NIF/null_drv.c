/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC NULL DRV
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/cm.h>
#include <soc/ll.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/common_drv.h>



STATIC soc_error_t 
soc_null_drv_init(int unit, soc_port_t port)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_FUNC_RETURN; 
}


STATIC int
soc_null_drv_enable_set(int unit, soc_port_t port, int enable)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}


STATIC int
soc_null_drv_speed_set(int unit, soc_port_t port, int speed)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}


STATIC int
soc_null_drv_loopback_set(int unit, soc_port_t port, int lb)
{
    SOCDNX_INIT_FUNC_DEFS

    SOCDNX_FUNC_RETURN
}


STATIC int
soc_null_drv_loopback_get(int unit, soc_port_t port, int *lb)
{
    SOCDNX_INIT_FUNC_DEFS

    *lb = 0;

    SOCDNX_FUNC_RETURN
}

CONST mac_driver_t soc_null_driver = {
    "NULL Driver",                  
    soc_null_drv_init,              
    soc_null_drv_enable_set,        
    NULL,                           
    NULL,                           
    NULL,                           
    soc_null_drv_speed_set,         
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    soc_null_drv_loopback_set,      
    soc_null_drv_loopback_get,      
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL,                           
    NULL                            
 };

#undef _ERR_MSG_MODULE_NAME


