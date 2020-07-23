/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module is the entry poit for Crash Recovery feature
 */

#ifndef _SOC_DCMN_CRASH_RECOVERY_H
#define _SOC_DCMN_CRASH_RECOVERY_H

#include <soc/types.h>
#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/error.h>
#include <soc/drv.h>
#endif

#include <soc/dcmn/dcmn_crash_recovery_utils.h>
#include <shared/swstate/sw_state.h>
#ifdef CRASH_RECOVERY_SUPPORT
#if defined(__KERNEL__) || !defined(__linux__)
#error "cmpl flags err: CRASH_RECOVERY_SUPPORT cannot be used w/o LINUX or with __KERNEL__"
#endif
#ifndef BCM_WARM_BOOT_SUPPORT
#error "cmpl flags err: CRASH_RECOVERY_SUPPORT cannot be used w/o BCM_WARM_BOOT_SUPPORT"
#endif
#endif

typedef enum
{
    DCMN_CR_JOURNALING_MODE_DISABLED=0,
    DCMN_CR_JOURNALING_MODE_AFTER_EACH_API,
    DCMN_CR_JOURNALING_MODE_ON_DEMAND
} DCMN_CR_JOURNALING_MODE;

#ifdef CRASH_RECOVERY_SUPPORT

#ifdef BCM_WARM_BOOT_API_TEST
#define SOC_CR_DOCUMENT_API_NAME(unit)\
    sal_strcpy(warmboot_api_function_name[unit],__FUNCTION__);
#else
#define SOC_CR_DOCUMENT_API_NAME(unit)
#endif

#define SOC_CR_DISP_ERR_CHECK(disp_rv)\
    if (disp_rv != SOC_E_NONE) {return SOC_E_INTERNAL;}


typedef void* (*dcmn_ha_mem_get)(unsigned int size, char *str, int flags);


typedef void* (*dcmn_ha_mem_release)(unsigned int size, char *str);

int soc_dcmn_cr_transaction_start(int unit);
int soc_dcmn_cr_init(int unit);
int soc_dcmn_cr_commit(int unit);
int soc_dcmn_cr_dispatcher_commit(int unit);
int soc_dcmn_cr_abort(int unit);
int soc_dcmn_cr_recover(int unit);
int soc_dcmn_cr_suppress(int unit, dcmn_cr_no_support reason);
int soc_dcmn_cr_unsuppress(int unit);
int soc_dcmn_cr_journaling_mode_get(int unit);
int soc_dcmn_cr_is_journaling_per_api(int unit);
int soc_dcmn_cr_is_journaling_on_demand(int unit);
int soc_dcmn_cr_journaling_mode_set(int unit, DCMN_CR_JOURNALING_MODE mode);
int soc_dcmn_cr_dispatcher_transaction_start(int unit);
int soc_dcmn_cr_dispatcher_transaction_end(int unit);

extern int soc_dcmn_cr_ondemand_sanity_check(int unit);
extern int soc_dcmn_cr_ondemand_em_traverse_hit(int unit);
#endif 
#endif  

