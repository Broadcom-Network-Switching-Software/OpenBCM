/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DCMN WB
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>


#if (defined(BCM_DFE_SUPPORT) || defined(BCM_DPP_SUPPORT)) && defined(BCM_WARM_BOOT_SUPPORT)

#include <soc/dcmn/dcmn_wb.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <shared/bsl.h>
#include <appl/diag/shell.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif 

#ifdef BCM_WARM_BOOT_API_TEST

#define ISSU_DUMP_DIRECTORY_MAX_LENGTH 256


int    _soc_dcmn_wb_warmboot_test_mode[SOC_MAX_NUM_DEVICES];

int    _soc_dcmn_wb_override_wb_test[SOC_MAX_NUM_DEVICES];

int    _soc_dcmn_wb_field_test_mode_stop[SOC_MAX_NUM_DEVICES][_dcmnWbFieldFlagNOF];

int    _soc_dcmn_wb_disable_once_wb_test[SOC_MAX_NUM_DEVICES];

int    _soc_dcmn_wb_warmboot_test_counter[SOC_MAX_NUM_DEVICES];

int    _soc_dcmn_wb_nested_api_counter[SOC_MAX_NUM_DEVICES];

char   _soc_dcmn_wb_default_dump_directory[ISSU_DUMP_DIRECTORY_MAX_LENGTH] = {'\0'};
char   _soc_dcmn_wb_issu_dump_directory[ISSU_DUMP_DIRECTORY_MAX_LENGTH] = {'\0'};

char warmboot_api_function_name[SOC_MAX_NUM_DEVICES][100];


sal_thread_t _soc_dcmn_wb_main_thread[SOC_MAX_NUM_DEVICES];


int dcmn_wb_cr_all_reset_test_utils_preconditions (int unit) {

    
    if (!SOC_UNIT_VALID(unit)) 
    {
        return 0;
    }
    if (!SOC_IS_DONE_INIT(unit)) 
    {
        return 0;
    }
    if (SOC_WARM_BOOT(unit)) 
    {
        return 0;
    }
    if (SOC_IS_DETACHING(unit)) 
    {
        return 0;
    }

    
    if (sal_thread_self() != _soc_dcmn_wb_main_thread[unit]) {
        return 0;
    }

    
    if (!dcmn_wb_api_counter_is_top_level(unit)) {
        return 0;
    }

    return 1;
}

int dcmn_wb_cr_common_api_preconditions (int unit) {
    int no_wb_test;
    int field_wb_stop;
    int disable_once;

    if (!dcmn_wb_cr_all_reset_test_utils_preconditions(unit)) {
        return 0;
    }

    soc_dcmn_wb_field_test_mode_get(unit, _dcmnWbFieldFlagDirectExtraction, &field_wb_stop);
    if (field_wb_stop) {
        return 0;
    }
    soc_dcmn_wb_field_test_mode_get(unit, _dcmnWbFieldFlagDataQual, &field_wb_stop);
    if (field_wb_stop) {
        return 0;
    }

    
    soc_dcmn_wb_no_wb_test_get(unit, &no_wb_test);
    if (no_wb_test) {
        return 0;
    }

    
    soc_dcmn_wb_disable_once_get(unit, &disable_once);
    if (disable_once) {
        return 0;
    }

    
    return 1;
}



void dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(int unit) {
    int disable_once;

    assert(dcmn_wb_cr_all_reset_test_utils_preconditions(unit));

    soc_dcmn_wb_disable_once_get(unit, &disable_once);
    if (1 == disable_once) {
        soc_dcmn_wb_disable_once_set(unit, 0);
    }
    return;
}


int dcmn_bcm_warm_boot_api_test_reset(int unit) {

    int test_counter;
    int warmboot_test_mode_enable;
    int nof_skip = 0;

    
    soc_dcmn_wb_test_mode_get(unit, &warmboot_test_mode_enable);
    if (_DCMN_BCM_WARM_BOOT_API_TEST_MODE_AFTER_EVERY_API != warmboot_test_mode_enable)
    {
        return 0;
    }

    if (!dcmn_wb_cr_all_reset_test_utils_preconditions(unit)) {
        return 0;
    }

    
    if (!dcmn_wb_cr_common_api_preconditions(unit)) {
        dcmn_bcm_warm_boot_api_test_enable_if_disabled_once(unit);
        return 0;
    }

    
    assert(!SOC_CR_ENABALED(unit));

    
    
    soc_dcmn_wb_test_counter_plus_plus(unit);

    
    
    soc_dcmn_wb_test_counter_get(unit, &test_counter);
    if (!SOC_IS_DFE(unit) && !dcmn_wb_exp_backoff(test_counter, &nof_skip)) {
        if(nof_skip) {
        LOG_WARN(BSL_LS_BCM_COMMON,
                 (BSL_META_U(unit,
                             "  --> *** WB BCM API %s *** skipping %d warm reboot tests for faster regression\n"),
                  warmboot_api_function_name[unit] , nof_skip));
        }
        return 0;
    }

    LOG_WARN(BSL_LS_BCM_COMMON,
             (BSL_META_U(unit,
              "**** WB BCM API %s **** (test counter: %d) ****\n"),
              warmboot_api_function_name[unit], test_counter));


    LOG_DEBUG(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
        "Unit:%d Starting warm reboot test\n"), unit));

    
    if (sh_process_command(unit, TR_141_COMMAND(SOC_CONTROL(unit)->autosync)) != 0) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "Unit:%d Warm reboot test failed\n"), unit));
        return -1;
    } else {
        LOG_WARN(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "Unit:%d Warm reboot test finish successfully\n"), unit));
    }

    return 0;
}



void
soc_dcmn_wb_test_mode_set(int unit, int enable)
{
    
    soc_dcmn_wb_main_thread_set(unit);

    _soc_dcmn_wb_warmboot_test_mode[unit] = enable;
}

void
soc_dcmn_wb_test_mode_get(int unit, int *enable)
{
    *enable =  _soc_dcmn_wb_warmboot_test_mode[unit];
}

void
soc_dcmn_wb_issu_dump_directory_set(int unit, char* directory)
{
    sal_strcpy(_soc_dcmn_wb_issu_dump_directory, directory);
}

void
soc_dcmn_wb_issu_dump_directory_get(int unit, char** directory)
{
    *directory = _soc_dcmn_wb_issu_dump_directory;
}

void
soc_dcmn_wb_default_dump_directory_set(int unit, char* directory)
{
    sal_strcpy(_soc_dcmn_wb_default_dump_directory, directory);
}

void
soc_dcmn_wb_default_dump_directory_get(int unit, char** directory)
{
    *directory = _soc_dcmn_wb_default_dump_directory;
}


void
soc_dcmn_wb_test_counter_set(int unit, int counter)
{
    assert(dcmn_wb_cr_all_reset_test_utils_preconditions(unit));
    _soc_dcmn_wb_warmboot_test_counter[unit] = counter;
}

void
soc_dcmn_wb_test_counter_get(int unit, int *counter)
{
    assert(dcmn_wb_cr_all_reset_test_utils_preconditions(unit));
    *counter =  _soc_dcmn_wb_warmboot_test_counter[unit];
}

void
soc_dcmn_wb_test_counter_plus_plus(int unit)
{
    assert(dcmn_wb_cr_all_reset_test_utils_preconditions(unit));
    _soc_dcmn_wb_warmboot_test_counter[unit]++;
}

void
soc_dcmn_wb_test_counter_reset(int unit)
{
    _soc_dcmn_wb_warmboot_test_counter[unit] = 0;
}


 
void
soc_dcmn_wb_no_wb_test_set(int unit, int wb_flag)
{
    if (wb_flag == 0)
    {
        _soc_dcmn_wb_override_wb_test[unit]--;
    }
    else
    {
        _soc_dcmn_wb_override_wb_test[unit]++;
    }
}

void
soc_dcmn_wb_no_wb_test_get(int unit, int *wb_flag)
{
    *wb_flag =  _soc_dcmn_wb_override_wb_test[unit];
}


void soc_dcmn_wb_field_test_mode_set(int unit, _dcmn_wb_field_falgs_e type, int wb_flag)
{
    
    soc_dcmn_wb_main_thread_set(unit);

    _soc_dcmn_wb_field_test_mode_stop[unit][type] = wb_flag;
}  
void soc_dcmn_wb_field_test_mode_get(int unit, _dcmn_wb_field_falgs_e type, int *wb_flag)
{
    
    assert(sal_thread_self() == _soc_dcmn_wb_main_thread[unit]);

    *wb_flag =  _soc_dcmn_wb_field_test_mode_stop[unit][type];
} 

 
void
soc_dcmn_wb_disable_once_set(int unit, int wb_flag)
{
    assert(dcmn_wb_cr_all_reset_test_utils_preconditions(unit));
    _soc_dcmn_wb_disable_once_wb_test[unit] = wb_flag;
}

void
soc_dcmn_wb_disable_once_get(int unit, int *wb_flag)
{
    assert(dcmn_wb_cr_all_reset_test_utils_preconditions(unit));
    *wb_flag = _soc_dcmn_wb_disable_once_wb_test[unit];
}

int dcmn_bcm_warm_boot_api_test_mode_skip_wb_sequence(int unit) {
    int rv = BCM_E_NONE;
    if (dcmn_wb_cr_all_reset_test_utils_preconditions(unit)) {
        soc_dcmn_wb_disable_once_set(unit, 1);
    }
    return rv;
}



int dcmn_wb_exp_backoff(int test_counter, int *nof_skip) {
    int i = DCMN_WARM_EXP_PARAM;
    int skip = 0;

    while (i < test_counter) {
        i *= DCMN_WARM_EXP_PARAM;
    }
    if (test_counter % (i/DCMN_WARM_EXP_PARAM) == 0 || test_counter == 0) {
      return 1;
    }
    skip = i / DCMN_WARM_EXP_PARAM - test_counter % (i/DCMN_WARM_EXP_PARAM);
    if (nof_skip != NULL && skip + 1 == i/DCMN_WARM_EXP_PARAM) *nof_skip = skip;
    else if (nof_skip != NULL) *nof_skip = 0;
    return 0;
}

int dcmn_wb_api_counter_increase(int unit)
{
    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   && 
          (!SOC_IS_DETACHING(unit))))
    {
        return 0;
    }

    
    if (sal_thread_self() != _soc_dcmn_wb_main_thread[unit]) {
        return 0;
    }

    _soc_dcmn_wb_nested_api_counter[unit]++;

    assert(_soc_dcmn_wb_nested_api_counter[unit] >= 0);
    assert(_soc_dcmn_wb_nested_api_counter[unit] < 4);

    return 0;
}

int dcmn_wb_api_counter_decrease(int unit)
{
    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   && 
          (!SOC_IS_DETACHING(unit))))
    {
        return 0;
    }

    
    if (sal_thread_self() != _soc_dcmn_wb_main_thread[unit]) {
        return 0;
    }

    
    assert(_soc_dcmn_wb_nested_api_counter[unit]>0);

    _soc_dcmn_wb_nested_api_counter[unit]--;

    assert(_soc_dcmn_wb_nested_api_counter[unit] >= 0);
    assert(_soc_dcmn_wb_nested_api_counter[unit] < 4);

    return 0;
}

int dcmn_wb_api_counter_get(int unit)
{
    if (!(SOC_UNIT_VALID(unit) &&
         SOC_IS_DONE_INIT(unit)   &&
         (!SOC_WARM_BOOT(unit))   && 
         (!SOC_IS_DETACHING(unit))))
    {
        return 1;
    }

    if (sal_thread_self() != _soc_dcmn_wb_main_thread[unit]) {
        return 1;
    }

    assert(_soc_dcmn_wb_nested_api_counter[unit] >= 0);
    assert(_soc_dcmn_wb_nested_api_counter[unit] < 4);

    return _soc_dcmn_wb_nested_api_counter[unit];
}

int dcmn_wb_api_counter_reset(int unit)
{
    if (!(SOC_UNIT_VALID(unit) &&
         SOC_IS_DONE_INIT(unit)   &&
         (!SOC_WARM_BOOT(unit))   && 
         (!SOC_IS_DETACHING(unit))))
    {
        return 0;
    }

    
    if (sal_thread_self() != _soc_dcmn_wb_main_thread[unit]) {
        return 0;
    }

    _soc_dcmn_wb_nested_api_counter[unit] = 0;

    return 0;
}

int dcmn_wb_api_counter_is_top_level(int unit)
{
#if defined(CRASH_RECOVERY_SUPPORT)
    
    if(!SOC_CR_ENABALED(unit) || soc_dcmn_cr_is_journaling_per_api(unit)) {
        assert(dcmn_wb_api_counter_get(unit));
    }
    
    if (!SOC_CR_ENABALED(unit)) {
        return (dcmn_wb_api_counter_get(unit) == 1);
    }
    else {
        return ((!soc_dcmn_cr_is_journaling_per_api(unit)) || 
                (dcmn_wb_api_counter_get(unit) == 1));
    }
#else
    return dcmn_wb_api_counter_get(unit) == 1;
#endif
}

void
soc_dcmn_wb_main_thread_set(
    int unit)
{
    _soc_dcmn_wb_main_thread[unit] = sal_thread_main_get();
    return;
}

#endif 

void soc_dcmn_wb_field_test_mode_set_or_suppress(int unit, _dcmn_wb_field_falgs_e type, int wb_flag)
{
    if (wb_flag) {
        if (type == _dcmnWbFieldFlagDirectExtraction) {
#ifdef BCM_WARM_BOOT_API_TEST
                soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDirectExtraction, 1);
#endif 
#ifdef CRASH_RECOVERY_SUPPORT
                soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_external_directory);
#endif 
        }
        if (type == _dcmnWbFieldFlagDataQual) {
#ifdef BCM_WARM_BOOT_API_TEST
                soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDataQual, 1);
#endif 
#ifdef CRASH_RECOVERY_SUPPORT
                soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_qualifier);
#endif 
        }
    }
    else {
        if (type == _dcmnWbFieldFlagDirectExtraction) {
#ifdef BCM_WARM_BOOT_API_TEST
                soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDirectExtraction, 0);
#endif 
#ifdef CRASH_RECOVERY_SUPPORT
                soc_dcmn_cr_remove_suppression_in_next_commit_set(unit);
#endif 
        }
        if (type == _dcmnWbFieldFlagDataQual) {
#ifdef BCM_WARM_BOOT_API_TEST
                soc_dcmn_wb_field_test_mode_set(unit, _dcmnWbFieldFlagDataQual, 0);
#endif 
#ifdef CRASH_RECOVERY_SUPPORT
                soc_dcmn_cr_remove_suppression_in_next_commit_set(unit);
#endif 
        }

    }
}

#endif 
