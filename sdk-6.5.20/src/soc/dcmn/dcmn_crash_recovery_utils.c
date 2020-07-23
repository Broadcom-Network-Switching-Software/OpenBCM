/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: this is a utility module that is shared between the other Crash Recovery modules.
 */
#include <sal/core/thread.h>
#include <soc/error.h>
#include <shared/bsl.h>


#include <soc/drv.h>
#include <soc/dcmn/dcmn_crash_recovery_utils.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#include <shared/swstate/access/sw_state_access.h>

#ifdef CRASH_RECOVERY_SUPPORT

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SHARED_SWSTATE


#ifdef BCM_WARM_BOOT_API_TEST
#define DCMN_CR_CTR_MIN 0
#else
#define DCMN_CR_CTR_MIN 1
#endif


extern soc_dcmn_cr_t *dcmn_cr_info[SOC_MAX_NUM_DEVICES];

uint8 soc_dcmn_cr_utils_is_logging(int unit)
{
    return (dcmn_cr_utils[unit].is_logging);
}

int soc_dcmn_cr_utils_logging_start(int unit)
{
    SOC_INIT_FUNC_DEFS;

    
    if(dcmn_cr_utils[unit].is_logging) {
        return SOC_E_EXISTS;
    }

    dcmn_cr_utils[unit].is_logging = TRUE;
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_utils_logging_stop(int unit)
{
    SOC_INIT_FUNC_DEFS;

    dcmn_cr_utils[unit].is_logging = FALSE;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

uint8 soc_dcmn_cr_utils_is_journaling_thread(int unit)
{
    return (sal_thread_self() == dcmn_cr_utils[unit].tid);
}

int soc_dcmn_cr_utils_journaling_thread_set(int unit)
{
    SOC_INIT_FUNC_DEFS;

    dcmn_cr_utils[unit].tid = sal_thread_self();
    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_api_counter_increase(int unit)
{
    int counter;
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   && 
          (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }

    
    if (sal_thread_self() != sal_thread_main_get()) {
        SOC_EXIT;
    }

    _SOC_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.config.nested_api_counter.get(unit, &counter));
    counter++;
    _SOC_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.config.nested_api_counter.set(unit, counter));

    assert(counter >= 0);
    assert(counter < 4);

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

int soc_dcmn_cr_api_counter_decrease(int unit)
{

    int counter;
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   && 
          (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }

    
    if (sal_thread_self() != sal_thread_main_get()) {
        SOC_EXIT;
    }

    _SOC_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.config.nested_api_counter.get(unit, &counter));

    
    if(counter < DCMN_CR_CTR_MIN) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                       (BSL_META_U(unit, "CR API-in-API counter has an invalid value: %d\n"), counter));
        return SOC_E_FAIL;
    }
    counter--;

    
    DCMN_CR_TEST(if (counter == -1) counter = 0);

    _SOC_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.config.nested_api_counter.set(unit, counter));

    assert(counter >= 0);
    assert(counter < 4);

    SOC_EXIT;
exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_api_counter_count_get(int unit)
{
    int counter = 0;
    int rv;

    if (!(SOC_UNIT_VALID(unit) &&
         SOC_IS_DONE_INIT(unit)   &&
         (!SOC_WARM_BOOT(unit))   && 
         (!SOC_IS_DETACHING(unit))))
    {
        return 1;
    }

    if (!SOC_CR_ENABALED(unit)) return 1;

    if (sal_thread_self() != sal_thread_main_get()) {
        return 1;
    }

    rv = sw_state_access[unit].dpp.soc.config.nested_api_counter.get(unit, &counter);

    assert(rv==SOC_E_NONE);
    assert(counter >= 0);
    assert(counter < 4);

    return counter;
}

int soc_dcmn_cr_api_counter_reset(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!(SOC_UNIT_VALID(unit) &&
         SOC_IS_DONE_INIT(unit)   &&
         (!SOC_WARM_BOOT(unit))   && 
         (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    
    if (sal_thread_self() != sal_thread_main_get()) {
        SOC_EXIT;
    }

    _SOC_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.config.nested_api_counter.set(unit, 0));

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;

}

int soc_dcmn_cr_transaction_api_count_get(int unit)
{
    int count = 0;

    if (SOC_CR_ENABALED(unit)    &&
        SOC_UNIT_VALID(unit) &&
        SOC_IS_DONE_INIT(unit)   &&
        (!SOC_WARM_BOOT(unit))   &&
        (!SOC_IS_DETACHING(unit))) {
        count = dcmn_cr_info[unit]->api_cnt;
    }

    return count;
}

int soc_dcmn_cr_transaction_api_count_reset(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   &&
          (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }

    dcmn_cr_info[unit]->api_cnt = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

int soc_dcmn_cr_transaction_api_count_increase(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit) || !soc_dcmn_cr_utils_is_logging(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit)   &&
          SOC_IS_DONE_INIT(unit)     &&
          (!SOC_WARM_BOOT(unit))     &&
          (!SOC_IS_DETACHING(unit))) ||
        
        !soc_dcmn_cr_api_is_top_level(unit))
    {
        SOC_EXIT;
    }


    dcmn_cr_info[unit]->api_cnt++;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

uint8 soc_dcmn_cr_traverse_flag_get(int unit)
{
    uint8 flag = 0;

    if (SOC_CR_ENABALED(unit)    &&
        SOC_UNIT_VALID(unit) &&
        SOC_IS_DONE_INIT(unit)   &&
        (!SOC_WARM_BOOT(unit))   &&
        (!SOC_IS_DETACHING(unit)))
    {
        flag = dcmn_cr_info[unit]->em_traverse;
    }

    return flag; 
}


int soc_dcmn_cr_traverse_flag_set(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   &&
          (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }

    dcmn_cr_info[unit]->em_traverse = 1;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}


int soc_dcmn_cr_traverse_flag_reset(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   &&
          (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }

    dcmn_cr_info[unit]->em_traverse = 0;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}

uint8 soc_dcmn_cr_api_is_top_level(int unit)
{
    
    assert(soc_dcmn_cr_api_counter_count_get(unit));

    
    if (!SOC_CR_ENABALED(unit)) {
        return (soc_dcmn_cr_api_counter_count_get(unit) == 1);
    }
    else {
        return ((!soc_dcmn_cr_is_journaling_per_api(unit)) || 
                (soc_dcmn_cr_api_counter_count_get(unit) == 1));
    }
}
int soc_dcmn_cr_remove_suppression_in_next_commit_set(int unit)
{
    SOC_INIT_FUNC_DEFS;

    if (!SOC_CR_ENABALED(unit)) SOC_EXIT;

    if (!(SOC_UNIT_VALID(unit) &&
          SOC_IS_DONE_INIT(unit)   &&
          (!SOC_WARM_BOOT(unit))   &&
          (!SOC_IS_DETACHING(unit))))
    {
        SOC_EXIT;
    }
    dcmn_cr_info[unit]->remove_suppression_in_next_commit = TRUE;

    SOC_EXIT;

exit:
    SOC_FUNC_RETURN;
}


#endif 
