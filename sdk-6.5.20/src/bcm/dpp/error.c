/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Error translation
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>

#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/TMC/tmc_api_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/debug.h>

int
translate_sand_success_failure(const SOC_SAND_SUCCESS_FAILURE result)
{
    switch(result) {
        case SOC_SAND_SUCCESS:
            return BCM_E_NONE;
            
        case SOC_SAND_FAILURE_OUT_OF_RESOURCES:
        case SOC_SAND_FAILURE_OUT_OF_RESOURCES_2:
        case SOC_SAND_FAILURE_OUT_OF_RESOURCES_3:
            return BCM_E_FULL;
            
        case SOC_SAND_FAILURE_REMOVE_ENTRY_FIRST:
            return BCM_E_EXISTS;
            
        case SOC_SAND_FAILURE_INTERNAL_ERR:
            return BCM_E_INTERNAL;

        case SOC_SAND_FAILURE_UNKNOWN_ERR:
        default:
             break;
    }
    return BCM_E_PARAM; 
}

int
handle_sand_result(uint32 soc_sand_result)
{
#if BROADCOM_DEBUG
    uint32 proc_id, err_id;
    char *err_name, *err_text, *soc_sand_proc_name, *soc_sand_module_name;
#endif /* BROADCOM_DEBUG */
    uint16 error_code;

    error_code = soc_sand_get_error_code_from_error_word(soc_sand_result);
  
    if (error_code != SOC_SAND_OK) {
#if BROADCOM_DEBUG
        err_id = soc_sand_error_code_to_string(error_code, &err_name,&err_text) ;

        if (soc_sand_get_error_code_from_error_word(err_id) != SOC_SAND_OK) {
            err_text = "No error code description (or procedure id) found" ;
        }

        proc_id = soc_sand_get_proc_id_from_error_word(soc_sand_result) ;
        soc_sand_proc_id_to_string((unsigned long)proc_id,&soc_sand_module_name,&soc_sand_proc_name)  ;

        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META("SOC_SAND Error Code 0x%X (fail):\r\n"
                            "Text : %s\n\r" /*Error name*/
                            "%s\r\n"        /*Error description*/
                            "Procedure id: 0x%04X (Mod: %s, Proc: %s)\n\r"),
                   err_id,
                   err_name,
                   err_text,
                   proc_id,
                   soc_sand_module_name,
                   soc_sand_proc_name));
#endif /* BROADCOM_DEBUG */        
        
        /* map soc error to BCM error */        
        switch (error_code) {
        case (uint16) SOC_PPD_ERR_OUT_OF_RESOURCES:
            return BCM_E_RESOURCE;
        case (uint16) SOC_PPD_ERR_NOT_EXIST:
            return BCM_E_NOT_FOUND;
        case (uint16) SOC_TMC_INPUT_OUT_OF_RANGE:
            return BCM_E_PARAM;
        case (uint16) SOC_TMC_CONFIG_ERR:
            return BCM_E_CONFIG;
        case (uint16) ARAD_CELL_NO_RECEIVED_CELL_ERR:
            return BCM_E_EMPTY;
        case (uint16) ARAD_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR:
            return BCM_E_PARAM;
        default:
            return BCM_E_INTERNAL;        
        }
    }
    
    return BCM_E_NONE;
}
