
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#if defined(BCM_WARM_BOOT_SUPPORT)

#include <soc/dnxc/swstate/sw_state_features.h>



#include <assert.h>
#include <shared/bsl.h>
#include <soc/drv.h>
#include <shared/mem_measure_tool.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>
#include <soc/dnxc/dnxc_verify.h>
#include <shared/utilex/utilex_seq.h>



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL



#define DNXC_SW_STATE_SCACHE_HANDLE_SET(_wb_handle, unit, _id)\
    SOC_SCACHE_HANDLE_SET(_wb_handle, unit, SOC_SCACHE_DNXC_SW_STATE, _id)
    
#define BLOCK_PTR_DIFF ((uint8*)data_block_header->data_ptr - (uint8*)restored_data_ptr)

#define DNXC_SW_STATE_MEMORY_MEASUREMENT_INCREASE(_size) \
        memory_measurement_dnx_sw_state_sample((_size), 1)
#define DNXC_SW_STATE_THREADS_NUM 4



 
 

typedef enum dnxc_sw_state_scache_oper_e
{
    socDnxcSwStateScacheRetreive,
    socDnxcSwStateScacheCreate,
    socDnxcSwStateScacheRealloc
} dnxc_sw_state_scache_oper_t;

typedef struct dnxc_sw_state_allocation_data_prefix_wb_d
{
    uint32 start_santinel;
    uint32 nof_elements;
    uint32 element_size;
} dnxc_sw_state_allocation_data_prefix_wb_t;

typedef struct dnxc_sw_state_allocation_data_suffix_d
{
    uint32 end_santinel;
} dnxc_sw_state_allocation_data_suffix_wb_t;

typedef struct
{
    uint8 *dest;
    uint32 data_size;
} memset_info_t;

 




dnxc_sw_state_data_block_header_t           *dnxc_sw_state_data_block_header[SOC_MAX_NUM_DEVICES];

uint32 sw_state_wb_size_start_measurement_point[SOC_MAX_NUM_DEVICES];

static int dnxc_sw_state_data_block_header_init(
    int unit,
    uint32 size);

static int dnxc_sw_state_restore_pointers(
    int unit,
    dnxc_sw_state_data_block_header_t * data_block_header);
    
static int
dnxc_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, dnxc_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists);

static void
dnxc_sw_state_parallel_memset(void* input);

static void
dnxc_sw_state_parallel_data_block_zero_init(uint32 data_size, uint8* data_ptr);

uint8 dnxc_sw_state_data_block_header_is_init(int unit) {
    if (dnxc_sw_state_data_block_header[unit] != NULL) {
        return dnxc_sw_state_data_block_header[unit]->is_init;
    }
    return FALSE;
}

uint8 dnxc_sw_state_data_block_is_in_range(int unit, uint8* ptr) {
     if ((ptr < (uint8 *)dnxc_sw_state_data_block_header[unit]->data_ptr)
        || (ptr > (uint8 *)dnxc_sw_state_data_block_header[unit]->next_free_data_slot)) {
            return FALSE;
     }
     return TRUE;
}

uint8 dnxc_sw_state_data_block_is_enough_space(int unit, uint32 size) {
    if (dnxc_sw_state_data_block_header[unit]->size_left < size + sizeof(dnxc_sw_state_alloc_element_t)) {
        return FALSE;
    }
    return TRUE;
}

int
dnxc_sw_state_wb_sizes_get(int unit, uint32* in_use, uint32* left) {
    if (!dnxc_sw_state_data_block_header[unit] || dnxc_sw_state_data_block_header[unit]->is_init == FALSE) {
        return _SHR_E_UNAVAIL;
    }
    *left = dnxc_sw_state_data_block_header[unit]->size_left;
    *in_use = dnxc_sw_state_data_block_header[unit]->data_size - *left;
    return _SHR_E_NONE;
}

int dnxc_sw_state_data_block_pointers_stack_push(int unit, uint8** ptr_location, dnxc_sw_state_alloc_element_type_e type) { 

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    
    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp->ptr_offset = ((uint8*)ptr_location - (uint8*)dnxc_sw_state_data_block_header[unit]->data_ptr);

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp->ptr_value = *ptr_location;

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp->type = type;

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp - 1;

    dnxc_sw_state_data_block_header[unit]->size_left = dnxc_sw_state_data_block_header[unit]->size_left - sizeof(dnxc_sw_state_alloc_element_t);

    DNXC_SW_STATE_FUNC_RETURN;
}
                    

int dnxc_sw_state_init_wb(
    int unit,
    uint32 flags,
    uint32 sw_state_max_size)
{
    int                                rc = SOC_E_NONE, i;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags = 0;
    uint32                             restored_size = 0;
    int                                already_exists = 0;
    uint8                              *scache_ptr = NULL;
    dnxc_sw_state_data_block_header_t  *retrieved_data_block_header;
    dnxc_sw_state_init_mode_t          init_mode;
    
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    DNXC_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, DNXC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);


    if (SOC_WARM_BOOT(unit)) {
        init_mode = socDnxcSwStateDataBlockRestoreAndOveride;
    } else {
        init_mode = socDnxcSwStateDataBlockRegularInit;
    }
    
    switch (init_mode) {
        case socDnxcSwStateDataBlockRegularInit:

            if (sw_state_max_size == 0) {
                SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,"Unit:%d sw_state_max_size cannot be 0 \n%s%s", unit, EMPTY, EMPTY);
            }
            SHR_IF_ERR_EXIT(dnxc_sw_state_data_block_header_init(unit, sw_state_max_size));

            break;

        case socDnxcSwStateDataBlockRestoreAndOveride:

            
            
            rc = dnxc_sw_state_scache_ptr_get(unit, wb_handle_orig, socDnxcSwStateScacheRetreive, scache_flags,
                                    &restored_size, &scache_ptr, &already_exists);
            SHR_IF_ERR_EXIT(rc);
            retrieved_data_block_header = (dnxc_sw_state_data_block_header_t *) scache_ptr;

            SHR_IF_ERR_EXIT(dnxc_sw_state_restore_pointers(unit, retrieved_data_block_header)); 

            for (i = 0; i < NOF_MODULE_ID; i++) {
                if (retrieved_data_block_header->roots_array[i] != NULL) {
                    dnxc_sw_state_dispatcher_module_init(unit, i, DNXC_SW_STATE_IMPLEMENTATION_WB);
                } else {
                    dnxc_sw_state_dispatcher_module_init(unit, i, DNXC_SW_STATE_IMPLEMENTATION_PLAIN);
                }
            }
            dnxc_sw_state_data_block_header[unit] = retrieved_data_block_header;

            break;
    }

    sw_state_wb_size_start_measurement_point[unit] = 0;

    sw_state_roots_array[unit] = dnxc_sw_state_data_block_header[unit]->roots_array;
    dnxc_sw_state_data_block_header[unit]->is_init = 1;

    SHR_IF_ERR_EXIT(rc);



    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_data_block_header_init(int unit, uint32 size) {

    int                                rc = SOC_E_NONE;
    soc_scache_handle_t                wb_handle_orig = 0;
    int                                scache_flags=0;
    int                                already_exists = 0;
    uint8                             *scache_ptr = NULL;
    uint32                             total_buffer_size = 0;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNXC_SW_STATE_SCACHE_HANDLE_SET(wb_handle_orig, unit, DNXC_SW_STATE_SCACHE_HANDLE_DATA_BLOCK);

    total_buffer_size = size; 

    
    rc = dnxc_sw_state_scache_ptr_get(unit, 
                                     wb_handle_orig, 
                                     socDnxcSwStateScacheCreate, 
                                     scache_flags,
                                     &total_buffer_size, &scache_ptr, &already_exists);
    SHR_IF_ERR_EXIT(rc);

    
    scache_ptr = (uint8*)(((unsigned long)scache_ptr + 0x3) & (~0x3));

    dnxc_sw_state_data_block_header[unit] = (dnxc_sw_state_data_block_header_t *) scache_ptr;

    dnxc_sw_state_data_block_header[unit]->total_buffer_size = total_buffer_size;
    dnxc_sw_state_data_block_header[unit]->data_size = size - sizeof(dnxc_sw_state_data_block_header_t);
    
    dnxc_sw_state_data_block_header[unit]->size_left = dnxc_sw_state_data_block_header[unit]->data_size;

    
    dnxc_sw_state_data_block_header[unit]->roots_array = (void**)(dnxc_sw_state_data_block_header[unit] + 1);
    
    
    dnxc_sw_state_data_block_header[unit]->data_ptr = (uint8*)dnxc_sw_state_data_block_header[unit]->roots_array;
    
    
    dnxc_sw_state_data_block_header[unit]->next_free_data_slot = (uint8*)(dnxc_sw_state_data_block_header[unit]->roots_array) + sizeof(void*) * NOF_MODULE_ID;
    
    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp = 
        (dnxc_sw_state_alloc_element_t *) ((uint8*)dnxc_sw_state_data_block_header[unit] + size - sizeof(dnxc_sw_state_alloc_element_t));

    dnxc_sw_state_data_block_header[unit]->ptr_offsets_stack_base = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp;

    dnxc_sw_state_data_block_header[unit]->alloc_mtx = sal_mutex_create("sw_state_alloc_mtx");
    SHR_NULL_CHECK(dnxc_sw_state_data_block_header[unit]->alloc_mtx, SOC_E_INIT, "could not create sw state alloc mutex");

    dnxc_sw_state_parallel_data_block_zero_init(dnxc_sw_state_data_block_header[unit]->data_size, dnxc_sw_state_data_block_header[unit]->data_ptr);

    dnxc_sw_state_data_block_header[unit]->is_init = 1;


    DNXC_SW_STATE_FUNC_RETURN;
}



int dnxc_sw_state_restore_pointers(int unit, dnxc_sw_state_data_block_header_t *data_block_header){
    
    uint8                            **tmp_ptr;
    uint8                             *restored_data_ptr;
    dnxc_sw_state_alloc_element_t      *p;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    restored_data_ptr = data_block_header->data_ptr;

    data_block_header->data_ptr = (uint8*)(data_block_header + 1);
    
    data_block_header->roots_array = (void**)(data_block_header->data_ptr);

    data_block_header->next_free_data_slot =
        ((uint8*)(data_block_header->next_free_data_slot) + BLOCK_PTR_DIFF);

    data_block_header->ptr_offsets_sp = (dnxc_sw_state_alloc_element_t*)
        ((uint8*)(data_block_header->ptr_offsets_sp) + BLOCK_PTR_DIFF);
    
    data_block_header->ptr_offsets_stack_base = (dnxc_sw_state_alloc_element_t*)
        ((uint8*)(data_block_header->ptr_offsets_stack_base) + BLOCK_PTR_DIFF);

    data_block_header->alloc_mtx = sal_mutex_create("sw_state_alloc_mtx");
        SHR_NULL_CHECK(data_block_header->alloc_mtx, SOC_E_INIT, "could not create sw state alloc mutex");

    
    p = data_block_header->ptr_offsets_sp;
    ++p;

    while (p <= data_block_header->ptr_offsets_stack_base) {
        tmp_ptr = (uint8 **) ((uint8*)(data_block_header->data_ptr) + p->ptr_offset);
        switch (p->type) {
            case SW_STATE_ALLOC_ELEMENT_POINTER:
                *tmp_ptr += BLOCK_PTR_DIFF;
                p->ptr_value = *tmp_ptr;
                break;
            case SW_STATE_ALLOC_ELEMENT_MUTEX:
                
                ((sw_state_mutex_t *)tmp_ptr)->mtx = sal_mutex_create(((sw_state_mutex_t *)tmp_ptr)->description);
                break;
            case SW_STATE_ALLOC_ELEMENT_SEM:
                
                ((sw_state_sem_t *)tmp_ptr)->sem = sal_sem_create(((sw_state_sem_t *)tmp_ptr)->description, ((sw_state_sem_t *)tmp_ptr)->is_binary, ((sw_state_sem_t *)tmp_ptr)->initial_count);
                break;
        }
        p++;
    }

    SOC_EXIT;

    DNXC_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_deinit_wb(int unit, uint32 flags) {

    dnxc_sw_state_alloc_element_t *p;
    sw_state_mutex_t mutex;
    sw_state_sem_t sem;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if(dnxc_sw_state_data_block_header_is_init(unit) == FALSE) {
        goto exit;
    }

    
    p = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp;
    ++p;

    while (p <= dnxc_sw_state_data_block_header[unit]->ptr_offsets_stack_base) {
        if (p->type == SW_STATE_ALLOC_ELEMENT_MUTEX) {
            mutex = *((sw_state_mutex_t*)((uint8*)(dnxc_sw_state_data_block_header[unit]->data_ptr) + p->ptr_offset));
            sal_mutex_destroy(mutex.mtx);
            mutex.mtx = NULL;
        }
        else if (p->type == SW_STATE_ALLOC_ELEMENT_SEM) {
            sem = *((sw_state_sem_t*)((uint8*)(dnxc_sw_state_data_block_header[unit]->data_ptr) + p->ptr_offset));
            sal_sem_destroy(sem.sem);
            sem.sem = NULL;
        }

        p++;
    }
    
    sal_mutex_destroy(dnxc_sw_state_data_block_header[unit]->alloc_mtx);

    
    dnxc_sw_state_data_block_header[unit] = NULL;

    DNXC_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_module_init_wb(
    int unit,
    uint32 module_id,
    uint32 size,
    uint32 flags,
    char *dbg_string) {

    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_wb(unit, module_id, (uint8**)&(dnxc_sw_state_data_block_header[unit]->roots_array[module_id]), 1, size, flags, dbg_string));
    sw_state_roots_array[unit][module_id] = dnxc_sw_state_data_block_header[unit]->roots_array[module_id];
    
    SOC_EXIT;

    DNXC_SW_STATE_FUNC_RETURN; 
}

int dnxc_sw_state_module_deinit_wb(
    int unit,
    uint32 module_id,
    uint32 flags,
    char *dbg_string) {
        
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_free_wb(unit, module_id, (uint8**)&(dnxc_sw_state_data_block_header[unit]->roots_array[module_id]), flags, dbg_string));

    sw_state_roots_array[unit][module_id] = NULL;
    dnxc_sw_state_data_block_header[unit]->roots_array[module_id] = NULL;
 
    SOC_EXIT;

    DNXC_SW_STATE_FUNC_RETURN;

}


int dnxc_sw_state_alloc_wb(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string) {

      

    uint8                             is_skip_alloc_mode = 0;
    unsigned int                      alloc_size = 0;
    uint32                            *p = NULL;
    uint32                            size = nof_elements * element_size;
    uint32                            _nof_elements = nof_elements;
    
    dnxc_sw_state_allocation_data_prefix_wb_t *ptr_prefix = NULL;

    
    dnxc_sw_state_allocation_data_suffix_wb_t *ptr_suffix = NULL;
    
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (utilex_seq_multithread_active_get(unit))
    {
        sal_mutex_take(dnxc_sw_state_data_block_header[unit]->alloc_mtx, sal_mutex_FOREVER);
    }

    SHR_IF_ERR_EXIT(dnxc_sw_state_skip_alloc_mode_get(unit, &is_skip_alloc_mode));
    if (is_skip_alloc_mode) {
        SHR_EXIT();
    }

    
    if (_nof_elements < 1)
    {
        _nof_elements = 1;
        size = element_size;
    }

    
    DNXC_VERIFY_INVOKE(DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_alloc_verify_common(unit, module_id, ptr_location, _nof_elements, element_size, flags, dbg_string)));

    
    if (dnxc_sw_state_data_block_header_is_init(unit) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INIT,"unit:%d trying to allocate a SW state var w/o initializing the SW state\n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (dnxc_sw_state_data_block_is_in_range(unit, (uint8 *)ptr_location) == FALSE) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to allocate a pointer and save it outside of the sw_state scope\n%s%s", unit, EMPTY, EMPTY);
    }

    
    DNXC_SW_STATE_ALIGN_SIZE(size);

     
    
    alloc_size = sizeof(dnxc_sw_state_allocation_data_prefix_wb_t) + size + sizeof(dnxc_sw_state_allocation_data_suffix_wb_t);
    DNXC_SW_STATE_MEMORY_MEASUREMENT_INCREASE(alloc_size);

    
    if (dnxc_sw_state_data_block_is_enough_space(unit, alloc_size) == FALSE) {
        unsigned long size_left ;
        unsigned long total_buffer_size ;

        size_left = (unsigned long)(dnxc_sw_state_data_block_header[unit]->size_left) ;
        total_buffer_size = (unsigned long)(dnxc_sw_state_data_block_header[unit]->total_buffer_size) ;
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY,"dnxc_sw_state_alloc_wb: not enough memory for allocation. total %lu size_left %lu alloc_size %lu\n",
                                                    total_buffer_size,size_left,(unsigned long)alloc_size + sizeof(dnxc_sw_state_alloc_element_t));
    }

    p = (uint32 *) dnxc_sw_state_data_block_header[unit]->next_free_data_slot;
    
    dnxc_sw_state_data_block_header[unit]->next_free_data_slot = (uint8*)dnxc_sw_state_data_block_header[unit]->next_free_data_slot + alloc_size;
    
    dnxc_sw_state_data_block_header[unit]->size_left = dnxc_sw_state_data_block_header[unit]->size_left - alloc_size;

    assert(UINTPTR_TO_PTR(PTR_TO_UINTPTR(p)) == p);


    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_wb_t*)p;

    ptr_prefix->start_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL;
    ptr_prefix->nof_elements = _nof_elements;
    ptr_prefix->element_size = element_size;
    
    ptr_suffix = (dnxc_sw_state_allocation_data_suffix_wb_t *)(((uint8 *)ptr_prefix) + sizeof(dnxc_sw_state_allocation_data_prefix_wb_t) + size);
    ptr_suffix->end_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL;

    *ptr_location = ((uint8*)p) + sizeof(dnxc_sw_state_allocation_data_prefix_wb_t);

    dnxc_sw_state_data_block_pointers_stack_push(unit, ptr_location, SW_STATE_ALLOC_ELEMENT_POINTER);

#ifdef BCM_DNX_SUPPORT
    if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_alloc(unit, module_id, ptr_location));
    }
#endif 

    SHR_EXIT();
exit:
    if (utilex_seq_multithread_active_get(unit))
    {
        sal_mutex_give(dnxc_sw_state_data_block_header[unit]->alloc_mtx);
    }
    SHR_FUNC_EXIT;
}

int dnxc_sw_state_free_wb(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string) {

    uint8                             is_allocated = 0;
    uint8                             **freed_location = NULL;
    dnxc_sw_state_alloc_element_t     *p = NULL;
    dnxc_sw_state_alloc_element_t     *stack_base = NULL;
    uint32                            size = 0;
    DNXC_SW_STATE_INIT_FUNC_DEFS;
    
    stack_base = dnxc_sw_state_data_block_header[unit]->ptr_offsets_stack_base;
    
    p = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp;
    ++p;

    
    DNXC_VERIFY_INVOKE(DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_free_verify_common(unit, module_id, ptr_location, flags, dbg_string)));

    while (p <= stack_base) {
        if (p->ptr_value == *ptr_location) {
            
            uint32 nof_elements;
            uint32 element_size;

            is_allocated = 1;
            
            if (p->type == SW_STATE_ALLOC_ELEMENT_POINTER) {
                freed_location = (uint8 **) 
                    ((uint8*)dnxc_sw_state_data_block_header[unit]->data_ptr + p->ptr_offset);
                
                assert (*freed_location == *ptr_location);
                assert ((*ptr_location >= (uint8*)dnxc_sw_state_data_block_header[unit]->data_ptr)
                      && (*ptr_location < (uint8*)dnxc_sw_state_data_block_header[unit]->next_free_data_slot));

                
                
                SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_size_wb(unit, module_id, *ptr_location, &nof_elements, &element_size));

#ifdef BCM_DNX_SUPPORT
                if(dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags, DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
                    SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_free(unit, module_id, nof_elements, element_size, ptr_location));
                }
#endif 

                size = nof_elements * element_size;
                size = size + sizeof(dnxc_sw_state_allocation_data_prefix_wb_t) + sizeof(dnxc_sw_state_allocation_data_suffix_wb_t);
                
                DNXC_SW_STATE_ALIGN_SIZE(size);
                
                sal_memset((uint8*)(*ptr_location) - sizeof(dnxc_sw_state_allocation_data_prefix_wb_t), 0, size);                

                
                *ptr_location = NULL;

            } else if(p->type == SW_STATE_ALLOC_ELEMENT_MUTEX) {
                sal_mutex_destroy(((sw_state_mutex_t*)ptr_location)->mtx);
                ((sw_state_mutex_t*)ptr_location)->mtx = NULL;
            } else if(p->type == SW_STATE_ALLOC_ELEMENT_SEM) {
                sal_sem_destroy(((sw_state_sem_t*)ptr_location)->sem);
                ((sw_state_sem_t*)ptr_location)->sem = NULL;
            }


            break;
        }
        p++;
    }
    

    if (is_allocated == 0) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"unit:%d trying to free an invalid pointer\n%s%s", unit, EMPTY, EMPTY);
    }

    
    if (p != dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1) {
        p->ptr_value = (dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->ptr_value;
        p->ptr_offset = (dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->ptr_offset;
        p->type = (dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1)->type;
    }

    
    sal_memset(dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp, 0, sizeof(dnxc_sw_state_alloc_element_t));
    dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp = dnxc_sw_state_data_block_header[unit]->ptr_offsets_sp + 1;

    SOC_EXIT;

    DNXC_SW_STATE_FUNC_RETURN;
}

int dnxc_sw_state_alloc_size_wb(
    int unit,
    uint32 module,
    uint8 *location,
    uint32 *nof_elements,
    uint32 *element_size) {

    dnxc_sw_state_allocation_data_prefix_wb_t* prefix;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    
    DNXC_VERIFY_INVOKE(DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_alloc_size_verify_common(unit, module, location, nof_elements, element_size)));

    prefix = (dnxc_sw_state_allocation_data_prefix_wb_t*)(location - sizeof(dnxc_sw_state_allocation_data_prefix_wb_t));

    *element_size = prefix->element_size;
    *nof_elements = prefix->nof_elements;

    DNXC_SW_STATE_FUNC_RETURN;
}


int
dnxc_sw_state_scache_ptr_get(int unit, soc_scache_handle_t handle, dnxc_sw_state_scache_oper_t oper,
                             int flags, uint32 *size, uint8 **scache_ptr, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    int        alloc_size;
    int        incr_size;


    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (scache_ptr == NULL) {
     SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d scache_ptr is null.\n%s%s", unit, EMPTY, EMPTY);

          
    }

    if (oper == socDnxcSwStateScacheCreate) {
        if (size == NULL) {
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d size is null.\n%s%s", unit, EMPTY, EMPTY);
           
        }
        if (already_exists == NULL) {
         SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d already_exist is null.\n%s%s", unit, EMPTY, EMPTY);
          
        }

        DNXC_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size);

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            return(rc);
        }

        if (rc == SOC_E_NONE) { 
            (*already_exists) = TRUE;
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM,"Unit:%d scache already_exist.\n%s%s", unit, EMPTY, EMPTY);
            
        }
        else { 
            (*already_exists) = FALSE;
            rc = soc_scache_alloc(unit, handle, alloc_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }

            rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
            if (rc != SOC_E_NONE) {
                return(rc);
            }
            if ((*scache_ptr) == NULL) {
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Memory failure.\n%s%s", unit, EMPTY, EMPTY);
             
            }
        }

        if (alloc_size != allocated_size) {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "Unit:%d Allocation size is not enough.\n%s%s", unit, EMPTY, EMPTY);
             
        }
    }

    else if (oper == socDnxcSwStateScacheRetreive) {
        if (size == NULL) {
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Size is null.\n%s%s", unit, EMPTY, EMPTY);
            
        }

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }
        (*size) = allocated_size;
        
        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }

    else if (oper == socDnxcSwStateScacheRealloc) {
        if (size == NULL) {
             SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Size is null.\n%s%s", unit, EMPTY, EMPTY);
            
        }


        
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        
        DNXC_SW_STATE_ALIGN_SIZE(*size);
        alloc_size = (*size);
        incr_size = alloc_size - allocated_size;

        rc = soc_scache_realloc(unit, handle, incr_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            return(rc);
        }

        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }
    else {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_PARAM, "Unit:%d Invalid parameter.\n%s%s", unit, EMPTY, EMPTY);
       
    }

    *size = allocated_size; 

    DNXC_SW_STATE_FUNC_RETURN;

}


void dnxc_sw_state_parallel_memset(void *input)
{
    memset_info_t *info = (memset_info_t *) input;
    sal_memset(info->dest, 0x0, info->data_size);
}

void dnxc_sw_state_parallel_data_block_zero_init(uint32 data_size, uint8* data_ptr)
{
    int thread_idx;
    memset_info_t                        mem_init[DNXC_SW_STATE_THREADS_NUM];
    sal_thread_t                       mem_init_threads[DNXC_SW_STATE_THREADS_NUM] = {0};
    sal_memset(mem_init, 0, DNXC_SW_STATE_THREADS_NUM*sizeof(memset_info_t));

    for (thread_idx = 0; thread_idx <DNXC_SW_STATE_THREADS_NUM; thread_idx++)
    {
        
        mem_init[thread_idx].data_size = data_size/DNXC_SW_STATE_THREADS_NUM;
        if(thread_idx == DNXC_SW_STATE_THREADS_NUM-1)
        {
            mem_init[thread_idx].data_size += data_size%DNXC_SW_STATE_THREADS_NUM;
        }
        mem_init[thread_idx].dest = (uint8*)((char*)data_ptr+((data_size/DNXC_SW_STATE_THREADS_NUM)*thread_idx));

        
        mem_init_threads[thread_idx] = sal_thread_create("mem_init_threads", SAL_THREAD_STKSZ, 1, dnxc_sw_state_parallel_memset, &(mem_init[thread_idx]));
    }

    for (thread_idx = 0; thread_idx <DNXC_SW_STATE_THREADS_NUM; thread_idx++)
    {
        
        sal_thread_join(mem_init_threads[thread_idx], NULL);
        mem_init_threads[thread_idx] = 0;
    }
}

int dnxc_sw_state_wb_start_size_measurement_point(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    if(dnxc_sw_state_data_block_header_is_init(unit))
    {
        sw_state_wb_size_start_measurement_point[unit] = dnxc_sw_state_data_block_header[unit]->size_left;
    }
    else
    {
        sw_state_wb_size_start_measurement_point[unit] = 0;
    }
    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}

uint32 dnxc_sw_state_wb_size_measurement_get(int unit)
{
    uint32 size;
    if(dnxc_sw_state_data_block_header_is_init(unit))
    {
        size = sw_state_wb_size_start_measurement_point[unit] - dnxc_sw_state_data_block_header[unit]->size_left;
    }
    else
    {
        size = 0;
    }
    return size;
}

uint8* dnxc_sw_state_wb_calc_pointer(int unit, sal_vaddr_t offset)
{
    return (uint8*)(dnxc_sw_state_data_block_header[unit]->roots_array) + offset;
}

sal_vaddr_t dnxc_sw_state_wb_calc_offset(int unit, uint8* ptr)
{
    return ptr - (uint8*)(dnxc_sw_state_data_block_header[unit]->roots_array);
}

uint8** dnxc_sw_state_wb_calc_double_pointer(int unit, sal_vaddr_t offset)
{
    return (uint8**)((uint8*)(dnxc_sw_state_data_block_header[unit]->roots_array) + offset);
}

sal_vaddr_t dnxc_sw_state_wb_calc_offset_from_dptr(int unit, uint8** ptr)
{
    return (uint8*)ptr - (uint8*)(dnxc_sw_state_data_block_header[unit]->roots_array);
}


#undef _ERR_MSG_MODULE_NAME

#else

typedef int make_iso_compilers_happy;

#endif 
