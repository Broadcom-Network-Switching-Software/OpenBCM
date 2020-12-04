
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>


#include <shared/bsl.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_sem_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL


int dnxc_sw_state_sem_create(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int is_binary, int initial_count, char *desc, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_sem) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state sempahore ERROR: sempahore ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif

    ptr_sem->sem = sal_sem_create(desc, is_binary, initial_count);
    ptr_sem->is_binary = is_binary;
    ptr_sem->initial_count = initial_count;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_add(unit, (uint8**)ptr_sem, NULL, DNXC_SW_STATE_DLL_SEM));

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_create(unit, module_id, ptr_sem));
    }
#endif 


    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_sem_destroy(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
{
#ifdef BCM_DNX_SUPPORT
    int is_binary = 0;
    int initial_count = 0;
#endif 
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_NULL_CHECK(ptr_sem, _SHR_E_PARAM, "ptr_sem");

#ifdef BCM_DNX_SUPPORT
    is_binary = ptr_sem->is_binary;
    initial_count = ptr_sem->initial_count;
#endif 

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free_by_ptr_location(unit, (uint8**)ptr_sem));

    if(NULL == ptr_sem) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state sempahore ERROR: sempahore ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_sem_destroy(ptr_sem->sem);

    ptr_sem->sem = NULL;

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_destroy(unit, module_id, ptr_sem, is_binary, initial_count));
    }
#endif 
    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_sem_take(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int usec, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_sem_take(ptr_sem->sem, usec);

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_take(unit, module_id, ptr_sem, usec));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_sem_give(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_sem_give(ptr_sem->sem);

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_give(unit, module_id, ptr_sem));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
