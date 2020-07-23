
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#if defined(BCM_WARM_BOOT_SUPPORT)

#include <soc/dnxc/swstate/sw_state_features.h>


#include <shared/bsl.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/dnxc_verify.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

extern dnxc_sw_state_data_block_header_t           *dnxc_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

#ifdef DNX_SW_STATE_VERIFICATIONS
#ifdef DNXC_VERIFICATION

STATIC int dnxc_sw_state_mutex_create_wb_internal_verify(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, char *desc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ptr_mtx != NULL);

    
    if (dnxc_sw_state_data_block_header_is_init(unit) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INIT,"unit:%d trying to create sw state mutex w/o initializing the SW state\n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (ptr_mtx->mtx != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"unit:%d sw state mutex can only be created once \n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (dnxc_sw_state_data_block_is_in_range(unit, (uint8*)ptr_mtx) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to create a mutex and save it outside of the sw_state scope\n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (dnxc_sw_state_data_block_is_enough_space(unit, 0) == FALSE) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(dnxc_sw_state_data_block_header[unit]->size_left) ;
        total_buffer_size = (unsigned long)(dnxc_sw_state_data_block_header[unit]->total_buffer_size) ;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY,"dnxc_sw_state_mutex_create_wb: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n",
                                                    total_buffer_size, size_left, (unsigned long)(sizeof(dnxc_sw_state_alloc_element_t)));
    }

    if (sal_strnlen(desc, SW_STATE_MUTEX_DESC_LENGTH) > (SW_STATE_MUTEX_DESC_LENGTH -1)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to create a mutex with description string larger than SW_STATE_MUTEX_DESC_LENGTH chars\n%s%s", unit, EMPTY, EMPTY);
    }

    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}
#endif 
#endif 



int dnxc_sw_state_mutex_create_wb(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, char *desc, uint32 flags) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    

#ifdef DNX_SW_STATE_VERIFICATIONS

    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxc_sw_state_mutex_create_wb_internal_verify(unit, module_id, ptr_mtx, desc)));

#endif 

    
    ptr_mtx->mtx = sal_mutex_create(desc);
    sal_strncpy(ptr_mtx->description, desc, SW_STATE_MUTEX_DESC_LENGTH-1);

    
    dnxc_sw_state_data_block_pointers_stack_push(unit, (uint8**)ptr_mtx, SW_STATE_ALLOC_ELEMENT_MUTEX);
#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_create(unit, module_id, ptr_mtx));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_mutex_destroy_wb(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_mtx->mtx) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state mutex ERROR: mutex ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif 

    
    SHR_IF_ERR_EXIT_WITH_LOG(dnxc_sw_state_free_wb(unit, module_id, (uint8**)ptr_mtx, flags,"sw_state_mutex_destroy"),
        "unit:%d sw_state mutex destroy failed\n%s%s", unit, EMPTY, EMPTY);
    
#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_mutex_destroy(unit, module_id, ptr_mtx));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_mutex_take_wb(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, int usec, uint32 flags)
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


int dnxc_sw_state_mutex_give_wb(int unit, uint32 module_id, sw_state_mutex_t *ptr_mtx, uint32 flags)
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

#else

typedef int make_iso_compilers_happy;

#endif 
