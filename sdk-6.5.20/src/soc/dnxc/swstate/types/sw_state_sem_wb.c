
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

STATIC int dnxc_sw_state_sem_create_wb_internal_verify(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, char *desc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    assert(ptr_sem != NULL);

    
    if (dnxc_sw_state_data_block_header_is_init(unit) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INIT,"unit:%d trying to create sw state semaphore w/o initializing the SW state\n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (ptr_sem->sem != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"unit:%d sw state semaphore can only be created once \n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (dnxc_sw_state_data_block_is_in_range(unit, (uint8*)ptr_sem) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to create a semaphore and save it outside of the sw_state scope\n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (dnxc_sw_state_data_block_is_enough_space(unit, 0) == FALSE) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(dnxc_sw_state_data_block_header[unit]->size_left) ;
        total_buffer_size = (unsigned long)(dnxc_sw_state_data_block_header[unit]->total_buffer_size) ;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY,"dnxc_sw_state_sem_create_wb: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n",
                                                    total_buffer_size, size_left, (unsigned long)(sizeof(dnxc_sw_state_alloc_element_t)));
    }

    if (sal_strnlen(desc, SW_STATE_SEM_DESC_LENGTH) > (SW_STATE_SEM_DESC_LENGTH -1)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to create a semaphore with description string larger than SW_STATE_SEM_DESC_LENGTH chars\n%s%s", unit, EMPTY, EMPTY);
    }

    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}
#endif 
#endif 




int dnxc_sw_state_sem_create_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int is_binary, int initial_count, char *desc, uint32 flags) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    

#ifdef DNX_SW_STATE_VERIFICATIONS

    DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxc_sw_state_sem_create_wb_internal_verify(unit, module_id, ptr_sem, desc)));

#endif 

    
    ptr_sem->sem = sal_sem_create(desc, is_binary, initial_count);
    ptr_sem->is_binary = is_binary;
    ptr_sem->initial_count = initial_count;
    sal_strncpy(ptr_sem->description, desc, SW_STATE_SEM_DESC_LENGTH-1);

    
    dnxc_sw_state_data_block_pointers_stack_push(unit, (uint8**)ptr_sem, SW_STATE_ALLOC_ELEMENT_SEM);

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_create(unit, module_id, ptr_sem));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_sem_destroy_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
{
#ifdef BCM_DNX_SUPPORT
    int is_binary = ptr_sem->is_binary;
    int initial_count = ptr_sem->initial_count;
#endif 
    DNXC_SW_STATE_INIT_FUNC_DEFS;

#ifdef DNX_SW_STATE_VERIFICATIONS
    if(DNXC_VERIFY_ALLOWED_GET(unit))
    {
        if(NULL == ptr_sem->sem) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "sw state semaphore ERROR: semaphore ptr cannot be NULL \n%s%s%s", EMPTY, EMPTY, EMPTY);
        }
    }
#endif 

    
    SHR_IF_ERR_EXIT_WITH_LOG(dnxc_sw_state_free_wb(unit, module_id, (uint8**)ptr_sem, flags,"sw_state_sem_destroy"),
        "unit:%d sw_state semaphore destroy failed\n%s%s", unit, EMPTY, EMPTY);

#ifdef BCM_DNX_SUPPORT
    if(!sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_sem_destroy(unit, module_id, ptr_sem, is_binary, initial_count));
    }
#endif 
    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_sem_take_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, int usec, uint32 flags)
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


int dnxc_sw_state_sem_give_wb(int unit, uint32 module_id, sw_state_sem_t *ptr_sem, uint32 flags)
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

#else

typedef int make_iso_compilers_happy;

#endif 
