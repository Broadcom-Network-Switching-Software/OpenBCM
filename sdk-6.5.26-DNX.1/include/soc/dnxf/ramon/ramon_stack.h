/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * RAMON STACK H
 */

#ifndef _SOC_RAMON_STACK_H_
#define _SOC_RAMON_STACK_H_

#ifndef BCM_DNXF1_SUPPORT
#error "This file is for use by DNXF1 (Ramon) family only!"
#endif

#include <soc/error.h>
#include <soc/dnxf/cmn/dnxf_stack.h>
#include <soc/defs.h>

shr_error_e soc_ramon_stk_modid_set(
    int unit,
    int fe_id);

shr_error_e soc_ramon_stk_modid_get(
    int unit,
    int *fe_id);

shr_error_e soc_ramon_stk_module_max_all_reachable_set(
    int unit,
    bcm_module_t module);

shr_error_e soc_ramon_stk_module_max_all_reachable_get(
    int unit,
    bcm_module_t * module);

shr_error_e soc_ramon_stk_module_max_fap_set(
    int unit,
    bcm_module_t module);

shr_error_e soc_ramon_stk_module_max_fap_get(
    int unit,
    bcm_module_t * module);

shr_error_e soc_ramon_stk_module_all_reachable_ignore_id_set(
    int unit,
    bcm_module_t module,
    int arg);

shr_error_e soc_ramon_stk_module_all_reachable_ignore_id_get(
    int unit,
    bcm_module_t module,
    int *arg);

#endif
