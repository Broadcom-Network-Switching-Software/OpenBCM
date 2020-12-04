
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <soc/dnxc/swstate/sw_state_features.h>



#include <assert.h>
#include <shared/bsl.h>
#include <shared/mem_measure_tool.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnxc_sw_state_plain.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <soc/dnxc/swstate/auto_generated/types/dnxc_module_ids_types.h>
#include <soc/dnxc/swstate/auto_generated/types/multithread_analyzer_types.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_common.h>
#include <soc/dnxc/dnxc_verify.h>
#include <sal/core/libc.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL



typedef struct {
    dnxc_sw_state_allocation_dll_t dll;
    uint32 allocated_size;
    uint32 measurement_start_point;
} dnxc_sw_state_plain_db_t;

dnxc_sw_state_plain_db_t *sw_state_plain_db[SOC_MAX_NUM_DEVICES];

#define DNXC_SW_STATE_MEMORY_MEASUREMENT_INCREASE(_size) \
        memory_measurement_dnx_sw_state_sample((_size), 1)

#ifdef DNX_SW_STATE_VERIFICATIONS

#define DNXC_SW_STATE_VALIDATE_PTR_START_SANTINEL(_ptr) \
    assert(((dnxc_sw_state_allocation_data_prefix_t *)(((uint8 *)_ptr) - sizeof(dnxc_sw_state_allocation_data_prefix_t)))->start_santinel == DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL)


#define DNXC_SW_STATE_VALIDATE_PTR_END_SANTINEL(_ptr, _size) \
    assert(((dnxc_sw_state_allocation_data_suffix_t *)(((uint8 *)_ptr) + _size))->end_santinel == DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL)


#define DNXC_SW_STATE_VALIDATE_PTR(_unit, _ptr,_size)           \
do {                                                            \
    if(DNXC_VERIFY_ALLOWED_GET(_unit))                          \
    {                                                           \
        DNXC_SW_STATE_VALIDATE_PTR_START_SANTINEL(_ptr);        \
        DNXC_SW_STATE_VALIDATE_PTR_END_SANTINEL(_ptr, _size);   \
    }                                                           \
} while(0)


#define DNXC_SW_STATE_VALIDATE_DLL_ENTRY(_unit, _dll_entry)                                     \
do {                                                                                            \
    if(DNXC_VERIFY_ALLOWED_GET(_unit))                                                          \
    {                                                                                           \
        assert(_dll_entry->start_santinel == DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL);   \
        assert(_dll_entry->end_santinel == DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL);       \
    }                                                                                           \
} while(0)

#else 

#define DNXC_SW_STATE_VALIDATE_PTR_START_SANTINEL(_ptr)
#define DNXC_SW_STATE_VALIDATE_PTR_END_SANTINEL(_ptr, _size)
#define DNXC_SW_STATE_VALIDATE_PTR(_unit, _ptr,_size)
#define DNXC_SW_STATE_VALIDATE_DLL_ENTRY(_unit, _dll_entry)

#endif 




STATIC int dnxc_sw_state_alloc_plain_internal(int unit, uint8 **ptr, uint32 size, char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    *ptr = sal_alloc(size, dbg_string);
    if(NULL == (*ptr)) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "sw state ERROR: failed to allocate data\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    sal_memset((void *)(*ptr), 0x0, size);
    DNXC_SW_STATE_MEMORY_MEASUREMENT_INCREASE(size);
    sal_set_alloc_counters_offset(size, 0);
    sal_set_alloc_counters_offset_main_thr(size, 0);

    DNXC_SW_STATE_FUNC_RETURN;
}


STATIC int dnxc_sw_state_free_plain_internal (int unit, uint8 **ptr, uint32 size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_FREE(*ptr);
    sal_set_alloc_counters_offset(0, size);
    sal_set_alloc_counters_offset_main_thr(0, size);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_dll_entry_add(int unit, uint8 **ptr_location, dnxc_sw_state_allocation_data_prefix_t *ptr_prefix, dnxc_sw_state_dll_entry_type_e type)
{
    dnxc_sw_state_allocation_dll_entry_t *entry = NULL;
    dnxc_sw_state_allocation_dll_entry_t *prev = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain_internal(unit, (uint8 **) &entry, sizeof(dnxc_sw_state_allocation_dll_entry_t), "sw state dll entry"));

    
    sw_state_plain_db[unit]->allocated_size += sizeof(dnxc_sw_state_allocation_dll_entry_t);

    entry->prev = NULL;
    entry->next = NULL;
    entry->ptr = *ptr_location;
    entry->ptr_location = ptr_location;
    entry->type = type;
    entry->start_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL;
    entry->end_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL;

    
    if(NULL == sw_state_plain_db[unit]->dll.head) {
        sw_state_plain_db[unit]->dll.head = entry;
        sw_state_plain_db[unit]->dll.tail = entry;
    } else {
        prev = sw_state_plain_db[unit]->dll.tail;
        entry->prev = prev;
        prev->next = entry;
        sw_state_plain_db[unit]->dll.tail = entry;
    }

    
    if (ptr_prefix != NULL) {
        ptr_prefix->dll_entry = entry;
    }

    sw_state_plain_db[unit]->dll.entry_counter++;

    DNXC_SW_STATE_FUNC_RETURN;
}


STATIC int dnxc_sw_state_dll_entry_free(int unit, dnxc_sw_state_allocation_dll_entry_t *dll_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(dll_entry, SOC_E_INTERNAL, "sw state ERROR: cannot free NULL dll_entry"));

    
    DNXC_SW_STATE_VALIDATE_DLL_ENTRY(unit, dll_entry);

    
    if((NULL == dll_entry->next) && (NULL == dll_entry->prev )) {
        
        sw_state_plain_db[unit]->dll.head = NULL;
        sw_state_plain_db[unit]->dll.tail = NULL;
    } else {
        
        if(NULL == dll_entry->prev){
            sw_state_plain_db[unit]->dll.head = dll_entry->next;
            sw_state_plain_db[unit]->dll.head->prev = NULL;
        } else {
            dll_entry->prev->next = dll_entry->next;
        }

        
        if(NULL == dll_entry->next){
            sw_state_plain_db[unit]->dll.tail = dll_entry->prev;
            sw_state_plain_db[unit]->dll.tail->next = NULL;
        } else {
            dll_entry->next->prev = dll_entry->prev;
        }
    }

    
    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain_internal(unit, (uint8 **) &dll_entry, sizeof(dnxc_sw_state_allocation_dll_entry_t)));
    sw_state_plain_db[unit]->dll.entry_counter--;

    DNXC_SW_STATE_FUNC_RETURN;
}



int dnxc_sw_state_dll_entry_free_by_ptr_location(int unit, uint8 **ptr_location)
{

    dnxc_sw_state_allocation_dll_entry_t *entry = sw_state_plain_db[unit]->dll.head;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(ptr_location, SOC_E_INTERNAL, "sw state ERROR: invalid pointer location"));
    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(*ptr_location, SOC_E_INTERNAL, "sw state ERROR: invalid pointer location"));

    while(entry != NULL)
    {
        if(entry->ptr_location == ptr_location)
        {
            break;
        }
        entry = entry->next;
    }

    DNXC_SW_STATE_VERIFICATIONS_ONLY(SHR_NULL_CHECK(entry, SOC_E_INTERNAL, "sw state ERROR: associated dll entry not found"));
    
    SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free(unit, entry));

    DNXC_SW_STATE_FUNC_RETURN;
}


STATIC int dnxc_sw_state_free_sw_state_pointer(int unit, uint32 module_id, uint8 *ptr)
{
    uint32 size = 0;
    uint32 nof_elements = 0;
    uint32 element_size = 0;
    uint8 *real_ptr = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_size_plain(unit, module_id, ptr, &nof_elements, &element_size));
    size = nof_elements * element_size;
    DNXC_SW_STATE_ALIGN_SIZE(size);
    size += (sizeof(dnxc_sw_state_allocation_data_prefix_t) + sizeof(dnxc_sw_state_allocation_data_suffix_t));

    
    real_ptr = (ptr - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain_internal(unit, &real_ptr, size));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_init_plain(
    int unit,
    uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    sw_state_plain_db[unit] = sal_alloc(sizeof(dnxc_sw_state_plain_db_t), "sw_state_plain_db");
    SHR_NULL_CHECK(sw_state_plain_db[unit], SOC_E_MEMORY, "could not allocate memory for sw_state_plain_db");

    sw_state_plain_db[unit]->dll.entry_counter = 0;
    sw_state_plain_db[unit]->dll.head = NULL;
    sw_state_plain_db[unit]->dll.tail = NULL;
    sw_state_plain_db[unit]->allocated_size = 0;
    sw_state_plain_db[unit]->measurement_start_point = 0;

#if defined(BCM_WARM_BOOT_SUPPORT)
    
    if(dnxc_sw_state_is_warmboot_supported_get(unit) == FALSE) {
        sw_state_roots_array[unit] = sal_alloc(sizeof(void*) * NOF_MODULE_ID, "alloc sw state roots array (plain implementation)");
        sal_memset(sw_state_roots_array[unit], 0, sizeof(void*) * NOF_MODULE_ID);        
    }
#else
    sw_state_roots_array[unit] = sal_alloc(sizeof(void*) * NOF_MODULE_ID, "alloc sw state roots array (plain implementation)");
    sal_memset(sw_state_roots_array[unit], 0, sizeof(void*) * NOF_MODULE_ID);     
#endif

    SHR_EXIT();
    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_deinit_plain(
    int unit,
    uint32 flags)
{
    
    uint32 module_id = 0;

    dnxc_sw_state_allocation_dll_entry_t *entry = NULL;
    dnxc_sw_state_allocation_dll_entry_t *prev = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if (sw_state_plain_db[unit] != NULL)
    {
        entry = sw_state_plain_db[unit]->dll.tail;
    }

    
    while(entry != NULL) {
        
        DNXC_SW_STATE_VALIDATE_DLL_ENTRY(unit, entry);

        
        if (entry->type == DNXC_SW_STATE_DLL_POINTER)
        {
            SHR_IF_ERR_EXIT(dnxc_sw_state_free_sw_state_pointer(unit, module_id, entry->ptr));

            
            if(entry->ptr == *(entry->ptr_location))
            {
                *(entry->ptr_location) = NULL;
            }
        }
        else if(entry->type == DNXC_SW_STATE_DLL_MUTEX)
        { 
            sal_mutex_destroy(((sw_state_mutex_t*)(entry->ptr_location))->mtx);
            ((sw_state_mutex_t*)(entry->ptr_location))->mtx = NULL;
        }
        else if(entry->type == DNXC_SW_STATE_DLL_SEM)
        { 
            sal_sem_destroy(((sw_state_sem_t*)(entry->ptr_location))->sem);
            ((sw_state_sem_t*)(entry->ptr_location))->sem = NULL;
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: unrecognized entry type\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        
        prev = entry->prev;
        SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free(unit, entry));

        entry = prev;
    }

    if (sw_state_plain_db[unit] != NULL)
    {
        sw_state_plain_db[unit]->dll.entry_counter = 0;
        sw_state_plain_db[unit]->dll.head = NULL;
        sw_state_plain_db[unit]->dll.tail = NULL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if(dnxc_sw_state_is_warmboot_supported_get(unit) == FALSE) {
        SHR_FREE(sw_state_roots_array[unit]);
    }
#else
        SHR_FREE(sw_state_roots_array[unit]);
#endif

    SHR_FREE(sw_state_plain_db[unit]);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_module_init_plain(int unit, uint32 module_id, uint32 size, uint32 flags, char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain(unit, module_id, (uint8**)&sw_state_roots_array[unit][module_id], 1, size, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_module_deinit_plain(int unit, uint32 module_id, uint32 flags, char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain(unit, module_id, (uint8**)&sw_state_roots_array[unit][module_id], flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_alloc_plain(int unit, uint32 module_id, uint8 **ptr_location, uint32 nof_elements, uint32 element_size, uint32 flags, char* dbg_string)
{



    uint32 size = 0;
    uint32 full_size = 0;
    uint8  is_skip_alloc_mode = 0;
    uint32 _nof_elements = nof_elements;

    
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;
    dnxc_sw_state_allocation_data_suffix_t *ptr_suffix = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_skip_alloc_mode_get(unit, &is_skip_alloc_mode));
    if (is_skip_alloc_mode) {
        SHR_EXIT();
    }

    
    if (_nof_elements < 1)
    {
        _nof_elements = 1;
    }

    
    DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_alloc_verify_common(unit, module_id, ptr_location, _nof_elements, element_size, flags, dbg_string));

    
    size = _nof_elements * element_size;
    DNXC_SW_STATE_ALIGN_SIZE(size);

    
    full_size = sizeof(dnxc_sw_state_allocation_data_prefix_t) + size + sizeof(dnxc_sw_state_allocation_data_suffix_t);

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_plain_internal(unit, (uint8 **) ptr_location, full_size, dbg_string));

    
    sw_state_plain_db[unit]->allocated_size += full_size;

    
    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t *)(*ptr_location);
    ptr_prefix->start_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_START_SANTINEL;
    ptr_prefix->nof_elements = _nof_elements;
    ptr_prefix->element_size = element_size;

    
    *ptr_location += sizeof(dnxc_sw_state_allocation_data_prefix_t);

    
    ptr_suffix = (dnxc_sw_state_allocation_data_suffix_t *)(*ptr_location + size);
    ptr_suffix->end_santinel = DNXC_SW_STATE_MEMORY_ALLOCATION_END_SANTINEL;

    
    if (module_id != MULTITHREAD_ANALYZER_MODULE_ID) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_add(unit, ptr_location, ptr_prefix, DNXC_SW_STATE_DLL_POINTER));
    }

#ifdef BCM_DNX_SUPPORT
    
    if (dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags,  DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        SHR_IF_ERR_EXIT(dnxc_sw_state_journal_log_alloc(unit, module_id, ptr_location));
    }
#endif 

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_alloc_size_plain(int unit, uint32 module, uint8 *location, uint32 *nof_elements, uint32 *element_size)
{
    uint32 size = 0;
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_alloc_size_verify_common(unit, module, location, nof_elements, element_size));

    
    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t *)(location - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    *nof_elements = ptr_prefix->nof_elements;
    *element_size = ptr_prefix->element_size;

    size = ptr_prefix->nof_elements * ptr_prefix->element_size;
    DNXC_SW_STATE_ALIGN_SIZE(size);

    DNXC_SW_STATE_VALIDATE_PTR(unit, location, size);

    DNXC_SW_STATE_FUNC_RETURN;
}

#ifdef DNX_SW_STATE_VERIFICATIONS
#ifdef DNXC_VERIFICATION

STATIC int dnxc_sw_state_free_plain_internal_verify(int unit, uint32 module_id, uint8 **ptr_location, uint32 flags, char *dbg_string, dnxc_sw_state_allocation_dll_entry_t *dll_entry)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    if(((dll_entry->ptr_location != ptr_location) && !sw_state_is_flag_on(flags, DNXC_SW_STATE_LINKED_LIST_FREE)) || (dll_entry->ptr != *ptr_location))
    {

        if(dll_entry->ptr != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->ptr: %p\n"), (void *)dll_entry->ptr));
        }

        if(dll_entry->ptr_location != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->ptr_location: %p\n"), (void *)dll_entry->ptr_location));
        }

        if(dll_entry->next != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->next: %p\n"), (void *)(dll_entry->next)));
        }

        if(dll_entry->prev != NULL)
        {
            LOG_CLI((BSL_META("dll_entry->prev: %p\n"), (void *)(dll_entry->prev)));
        }

        if(dll_entry->type == DNXC_SW_STATE_DLL_POINTER)
        {
            LOG_CLI((BSL_META("swstate ptr entry\n")));
        }
        else
        {
            LOG_CLI((BSL_META("swstate mutex entry\n")));
        }

        if(*ptr_location != NULL)
        {
            LOG_CLI((BSL_META("*ptr_location: %p\n"), (void *)(*ptr_location)));
        }

        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: associated linked list entry stores a different sw state pointer value \n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}
#endif 
#endif 


int dnxc_sw_state_free_plain(int unit, uint32 module_id, uint8 **ptr_location, uint32 flags, char *dbg_string)
{
    int result = SOC_E_NONE;
    uint32 nof_elements = 0;
    uint32 element_size = 0;
    
    dnxc_sw_state_allocation_data_prefix_t *ptr_prefix = NULL;
    
    dnxc_sw_state_allocation_dll_entry_t *dll_entry = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_sw_state_free_verify_common(unit, module_id, ptr_location, flags, dbg_string));

    if (NULL == (*ptr_location)) {
        return SOC_E_NONE;
    }

    dnxc_sw_state_alloc_size_plain(unit, module_id, *ptr_location, &nof_elements, &element_size);

#ifdef BCM_DNX_SUPPORT
    if (dnxc_sw_state_journal_is_done_init(unit) && !sw_state_is_flag_on(flags, DNXC_SW_STATE_JOURNAL_ROLLING_BACK)) {
        result = dnxc_sw_state_journal_log_free(unit, module_id, nof_elements, element_size, ptr_location);
    }
#endif 

    
    ptr_prefix = (dnxc_sw_state_allocation_data_prefix_t *)((uint8 *)(*ptr_location) - sizeof(dnxc_sw_state_allocation_data_prefix_t));

    
    if (module_id != MULTITHREAD_ANALYZER_MODULE_ID) {

        
        dll_entry = ptr_prefix->dll_entry;
        if(dll_entry->type != DNXC_SW_STATE_DLL_POINTER)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: associated linked list entry type is not for a swstate ptr\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        
        DNXC_SW_STATE_VALIDATE_DLL_ENTRY(unit, dll_entry);

#ifdef DNX_SW_STATE_VERIFICATIONS
        
        DNXC_VERIFY_INVOKE(SHR_IF_ERR_EXIT(dnxc_sw_state_free_plain_internal_verify(unit, module_id, ptr_location, flags, dbg_string, dll_entry)));

#endif 

    }

    
    SHR_IF_ERR_EXIT(dnxc_sw_state_free_sw_state_pointer(unit, module_id, *ptr_location));
    *ptr_location = NULL;

    
    if (module_id != MULTITHREAD_ANALYZER_MODULE_ID) {
        
        SHR_IF_ERR_EXIT(dnxc_sw_state_dll_entry_free(unit, dll_entry));
    }

    SHR_IF_ERR_EXIT(result);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_plain_start_size_measurement_point(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    sw_state_plain_db[unit]->measurement_start_point = sw_state_plain_db[unit]->allocated_size;
    SHR_EXIT();

    DNXC_SW_STATE_FUNC_RETURN;
}


uint32 dnxc_sw_state_plain_size_measurement_get(int unit)
{
    uint32 size;
    size = sw_state_plain_db[unit]->allocated_size - sw_state_plain_db[unit]->measurement_start_point;
    return size;
}


int dnxc_sw_state_total_size_get_plain(int unit, uint32 *size)
{
    uint32 nof_elements;
    uint32 element_size;

    dnxc_sw_state_allocation_dll_entry_t *entry = sw_state_plain_db[unit]->dll.head;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    *size = 0;

    while(entry != NULL)
    {
        
        SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_size_plain(unit, -1, *(entry->ptr_location), &nof_elements, &element_size));
        *size = *size + (nof_elements * element_size);
    }

    DNXC_SW_STATE_FUNC_RETURN;
}


uint8* dnxc_sw_state_plain_calc_pointer(int unit, sal_vaddr_t offset)
{
    return INT_TO_PTR(offset);
}

sal_vaddr_t dnxc_sw_state_plain_calc_offset(int unit, uint8* ptr)
{
    return PTR_TO_UINTPTR(ptr);
}

uint8** dnxc_sw_state_plain_calc_double_pointer(int unit, sal_vaddr_t offset)
{
    return INT_TO_PTR(offset);
}

sal_vaddr_t dnxc_sw_state_plain_calc_offset_from_dptr(int unit, uint8** ptr)
{
    return PTR_TO_UINTPTR(ptr);
}

#undef _ERR_MSG_MODULE_NAME
