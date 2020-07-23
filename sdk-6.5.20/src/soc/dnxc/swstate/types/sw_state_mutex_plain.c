
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
#include <soc/dnxc/swstate/types/sw_state_mutex_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL


int dnxc_sw_state_mutex_create(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, char *desc, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_mtx) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif

    ptr_mtx->mtx = sal_mutex_create(desc);

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_add(unit, (uint8**)ptr_mtx, NULL, DNXC_SW_STATE_DLL_MUTEX));

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_create(unit, module_id, ptr_mtx));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_mutex_destroy(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free_by_ptr_location(unit, (uint8**)ptr_mtx));
    
    if(NULL == ptr_mtx) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_mutex_destroy(ptr_mtx->mtx);

    ptr_mtx->mtx = NULL;

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_destroy(unit, module_id, ptr_mtx));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_mutex_take(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, int usec, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_mutex_take(ptr_mtx->mtx, usec);

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_take(unit, module_id, ptr_mtx, usec));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_mutex_give(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sal_mutex_give(ptr_mtx->mtx);

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_give(unit, module_id, ptr_mtx));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
