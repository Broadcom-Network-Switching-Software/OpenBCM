/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 STACK H
 */
 
#ifndef _SOC_FE3200_STACK_H_
#define _SOC_FE3200_STACK_H_

#include <soc/error.h>
#include <soc/dfe/cmn/dfe_stack.h>
#include <soc/defs.h>

#define SOC_FE3200_STK_MAX_MODULE (2047)
#define SOC_FE3200_STK_FAP_GROUP_SIZE (32)
#define SOC_FE3200_STK_MAX_IGNORED_FAP_IDS (16)

soc_error_t soc_fe3200_stk_modid_set(int unit, uint32 fe_id);
soc_error_t soc_fe3200_stk_modid_get(int unit, uint32* fe_id);
soc_error_t soc_fe3200_stk_module_max_all_reachable_verify(int unit, bcm_module_t module);
soc_error_t soc_fe3200_stk_module_max_all_reachable_set(int unit, bcm_module_t module);
soc_error_t soc_fe3200_stk_module_max_all_reachable_get(int unit, bcm_module_t *module);
soc_error_t soc_fe3200_stk_module_max_fap_verify(int unit, bcm_module_t module);
soc_error_t soc_fe3200_stk_module_max_fap_set(int unit, bcm_module_t module);
soc_error_t soc_fe3200_stk_module_max_fap_get(int unit, bcm_module_t *module);
soc_error_t soc_fe3200_stk_module_all_reachable_ignore_id_set(int unit, bcm_module_t module, int arg);
soc_error_t soc_fe3200_stk_module_all_reachable_ignore_id_get(int unit, bcm_module_t module, int *arg);
soc_error_t soc_fe3200_stk_valid_module_id_verify(int unit, bcm_module_t module, int verify_divisble_by_32);

#endif

