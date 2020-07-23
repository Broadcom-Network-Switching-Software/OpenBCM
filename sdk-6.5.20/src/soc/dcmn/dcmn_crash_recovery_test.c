/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this module is the entry poit for Crash Recovery feature
 *
 * some design details:
 * - transaction starts at the beginning of an API and ends at the end of an API
 */

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>

#if defined(CRASH_RECOVERY_SUPPORT)
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <soc/dcmn/dcmn_crash_recovery_test.h>
#endif

#if defined(CRASH_RECOVERY_SUPPORT) && defined(BCM_WARM_BOOT_API_TEST)
#include <soc/dcmn/dcmn_wb.h>
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE

extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];

int iteration_occured[SOC_MAX_NUM_DEVICES][2];
int cr_test_done[SOC_MAX_NUM_DEVICES] = { 1 }; 
int cr_test_ttl_counter[SOC_MAX_NUM_DEVICES];

extern char warmboot_api_function_name[SOC_MAX_NUM_DEVICES][100];

int dcmn_cr_test_prepare(int unit) {
    

    if (!dcmn_wb_cr_all_reset_test_utils_preconditions(unit))
    {
        return 0;
    }

    if (!soc_dcmn_cr_is_journaling_per_api(unit)) {
        return 0;
    }

    
    assert(cr_test_done[unit]);

    cr_test_done[unit] = 0;
    cr_test_ttl_counter[unit] = 0;
    iteration_occured[unit][0] = 0;
    iteration_occured[unit][1] = 0;

    return 0;
}

int dcmn_cr_test_keep_looping(int unit) {

    int test_mode;

    if (!dcmn_wb_cr_all_reset_test_utils_preconditions(unit))
    {
        return 0;
    }

    if (!soc_dcmn_cr_is_journaling_per_api(unit)) {
        return 0;
    }

    
    
    cr_test_ttl_counter[unit]++;
    assert(cr_test_ttl_counter[unit] < 3);

    
    soc_dcmn_wb_test_mode_get(unit, &test_mode);
    if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY != test_mode)
    {
        return 0;
    }

    
    return !(iteration_occured[unit][0] && iteration_occured[unit][1]);
}

int dcmn_cr_test_clean(int unit) {

    if (!dcmn_wb_cr_all_reset_test_utils_preconditions(unit))
    {
        return 0;
    }

    

    cr_test_done[unit] = 1;
    return 0;
}


int dcmn_bcm_crash_recovery_api_test_reset(int unit, int iteration, int *was_reset) {

    int test_counter;
    int warmboot_test_mode_enable;
    int nof_skip = 0;

    
    if (was_reset) {
        *was_reset = 0;
    }

    if (!dcmn_wb_cr_all_reset_test_utils_preconditions(unit)) {
        return 0;
    }

    
    soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode_enable);
    if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_CRASH_RECOVERY != warmboot_test_mode_enable)
    {
        return 0;
    }

    
    if (iteration_occured[unit][iteration])
    {
        return 0;
    }
    else
    {
        
        iteration_occured[unit][iteration] = TRUE;
    }

    
    if (!dcmn_wb_cr_common_api_preconditions(unit)) {
        if (iteration == 1) {
            
            dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(unit);
        }
        return 0;
    }

    
    assert(SOC_CR_ENABALED(unit));

    
    if (soc_dcmn_cr_is_journaling_per_api(unit) && !dcmn_cr_info[unit]->is_recovarable) {
        return 0;
    }

    
    
    if (iteration == 0) {
        soc_dcmn_wb_test_counter_plus_plus(unit);
    }

    
    soc_dcmn_wb_test_counter_get(unit, &test_counter);
    if (!dcmn_wb_exp_backoff(test_counter, &nof_skip)) {
        if(nof_skip) {
        LOG_WARN(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                             "\n  --> *** CR BCM API %s *** skipping %d crash tests for faster regression\n"),
                  warmboot_api_function_name[unit] , nof_skip));
        }
        return 0;
    }

    LOG_WARN(BSL_LS_BCM_COMMON,
             (BSL_META_U(unit,
              "\n**** Crash Recovery BCM API %s **** (test counter: %d, iteration: [%d]%s) ****\n"),
              warmboot_api_function_name[unit], test_counter, iteration,
              (iteration==0)? "roll-back" : (iteration==1)? "commit" : "value not expected (ERROR)!!!"));

    LOG_DEBUG(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
        "Unit:%d Starting warm reboot test\n"), unit));

    
    if (was_reset) {
        *was_reset = 1;
    }
    if (sh_process_command(unit, TR_141_COMMAND(TRUE)) != 0) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "Unit:%d Warm reboot test failed\n"), unit));
        return -1;
    } else {
        LOG_WARN(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "Unit:%d Warm reboot test finish successfully\n"), unit));
    }

    return 0;
}

#endif
