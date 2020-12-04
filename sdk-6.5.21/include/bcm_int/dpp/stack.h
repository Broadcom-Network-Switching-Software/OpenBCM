/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        stack.h
 * Purpose:     STACK internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_STACK_H_
#define   _BCM_INT_DPP_STACK_H_

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/PPD/ppd_api_lag.h>

/* Save internal sysport erp for each unit,device */
#define _DPP_STACK_MAX_DEVICES (SOC_TMC_NOF_FAPS_IN_SYSTEM)
#define _DPP_STACK_MAX_DEVICES_UINT32_BITMAP ((_DPP_STACK_MAX_DEVICES / 32) + 1)
#define _DPP_STACK_MAX_TM_DOMAIN (SOC_TMC_NOF_TM_DOMAIN_IN_SYSTEM)
#define _DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP ((_DPP_STACK_MAX_DEVICES / 32) + 1)

#define _DPP_STACK_INVALID_DOMAIN 0xffffffff

typedef struct bcm_dpp_wb_stack_config_s {

    int _sysport_erp[_DPP_STACK_MAX_DEVICES];
    SHR_BITDCL _modid_to_domain[_DPP_STACK_MAX_TM_DOMAIN][_DPP_STACK_MAX_DEVICES_UINT32_BITMAP];
    SHR_BITDCL _domain_to_stk_trunk[_DPP_STACK_MAX_TM_DOMAIN][_DPP_STACK_MAX_TM_DOMAIN_UINT32_BITMAP];

} bcm_dpp_stack_config_t; 

/* 
 *  Stacknig utility functions.
 */ 
int bcm_petra_stk_modid_to_domain_find(int unit, uint32 local_modid, uint32 sysport_modid, uint32 *is_same_domain);
int bcm_petra_stk_domain_modid_get(int unit, uint32 modid, int *domain);

extern int
_bcm_petra_stk_detach(int unit);

int 
_bcm_petra_stk_trunk_domian_get(
    int unit, 
    int domain,
    int stk_trunk_max,
    bcm_trunk_t *stk_trunk_array,
    int *stk_trunk_count);

int 
__bcm_petra_stk_trunk_domian_fec_map_update(
    int unit, 
    int domain);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_stk_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
      
#endif /* _BCM_INT_PETRA_STACK_H_ */
