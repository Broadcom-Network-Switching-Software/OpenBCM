/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INFO: This module is a generic rollback journal,
 * saving the old values that were overridden
 * by access calls since the beginning of the last transaction.
 * The ADT is double-ended queue, implemented as double-linked list.
 */

#include <assert.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <soc/drv.h>
#include <soc/dnx/recovery/rollback_journal.h>
#include <soc/dnx/recovery/rollback_journal_diag.h>
#include <soc/dnx/recovery/rollback_journal_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_ERRORRECOVERY

#define DNX_ROLLBACK_JOURNAL_OPT_CODE 1


static dnx_rollback_journal_pool_t dnx_rollback_journal_pool[SOC_MAX_NUM_DEVICES] = { {0} };

#ifdef DNX_ERR_RECOVERY_VALIDATION

#define DNX_ROLLBACK_JOURNAL_VALIDATE_HANDLE(_handle)\
do {\
    if((DNX_ROLLBACK_JOURNAL_INVALID_HANDLE == (_handle))\
     ||((_handle) >= DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM)\
     ||(NULL == dnx_rollback_journal_pool[unit].entries[(_handle)])) {\
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: invalid journal handle.\n%s%s%s", EMPTY, EMPTY, EMPTY);\
    }\
} while(0)
#else
#define DNX_ROLLBACK_JOURNAL_VALIDATE_HANDLE(_handle)
#endif 


#define DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal_ptr, _handle)   \
do {                                                                  \
    DNX_ROLLBACK_JOURNAL_VALIDATE_HANDLE((_handle));                 \
    journal_ptr = dnx_rollback_journal_pool[unit].entries[(_handle)];\
} while(0)

#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE

#define DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(size) ((size+3)&~3)
#else
#define DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(size) (size)
#endif 


#define DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(sizeof(dnx_rollback_journal_mem_chunk_t))


#define DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(sizeof(dnx_rollback_journal_entry_t))


#define DNX_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata_ptr, entry_ptr)   \
do {                                                                           \
    metadata_ptr = ((uint8 *)entry_ptr) + DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE;\
} while(0)


#define DNX_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload_ptr, entry_ptr)  \
do {                                                                        \
    payload_ptr = ((uint8 *)entry_ptr)                                      \
    + DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry_ptr->metadata_size);    \
} while(0)

#define DNX_ROLLBACK_JOURNAL_ENTRY_GET_CHUNK_PTR(chunk_ptr, entry_ptr)   \
do {                                                                           \
    chunk_ptr = entry_ptr->mem_chunk; \
} while(0)


#define DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_DATA_PTR(_type, _data_ptr, _mem_chunk)   \
do {                                                                           \
    _data_ptr = (_type)((uint8 *)_mem_chunk \
    + DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE); \
} while(0)


#define DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(_type, _data_ptr, _mem_chunk)   \
do {                                                                           \
    _data_ptr = (_type)((uint8 *)_mem_chunk \
    + DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE \
    + _mem_chunk->data_size \
    - _mem_chunk->available_space_at_end); \
} while(0)


#define DNX_ROLLBACK_JOURNAL_ENTRY_GET_NEXT_CHUNK_DATA_PTR(_type, _data_ptr, _entry_ptr)   \
do {                                                     \
    DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_DATA_PTR(   \
        _type, _data_ptr, _entry_ptr->mem_chunk->next); \
} while(0)


#define DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(_state) \
    ((DNX_ROLLBACK_JOURNAL_ENTRY_STATE_VALID == (_state)) \
     || (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID == (_state)))

#ifdef DNX_ROLLBACK_JOURNAL_DIAGNOSTICS
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)\
    SHR_IF_ERR_EXIT(dnx_rollback_journal_diag_alloc(_unit, _jrnl_type, _jrnl_subtype, _size))
#else
#define DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size)
#endif

#define DNX_ROLLBACK_JOURNAL_ENTRY_ALLOC(_unit, _ptr, _type, _size, _metadata_size, _str, _jrnl, _jrnl_type, _jrnl_subtype)\
do {                                                                                           \
    SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_alloc(_unit, _jrnl, _size, _metadata_size, _ptr));         \
} while(0)

#define DNX_ROLLBACK_JOURNAL_ALLOC(_unit, _ptr, _type, _size, _str, _jrnl_type, _jrnl_subtype)\
do {                                                                                           \
    (_ptr) = (_type)sal_alloc((_size), (_str));                                                \
    DNX_ROLLBACK_JOURNAL_DIAGNOSTICS_ALLOC(_unit, _jrnl_type, _jrnl_subtype, _size);          \
} while(0)


static shr_error_e
dnx_rollback_journal_mempool_insert(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_mem_chunk_t * new_chunk,
    uint32 insert_at_head)
{
    dnx_rollback_journal_mem_chunk_t *pool_tail = journal->mem_pool.tail;

    SHR_FUNC_INIT_VARS(unit);

    if (!pool_tail)
    {
        journal->mem_pool.head = new_chunk;
        journal->mem_pool.tail = new_chunk;
    }
    else
    {
        if (insert_at_head)
        {
            journal->mem_pool.head->prev = new_chunk;
            new_chunk->next = journal->mem_pool.head;
            journal->mem_pool.head = new_chunk;
        }
        else
        {
            journal->mem_pool.tail->next = new_chunk;
            new_chunk->prev = journal->mem_pool.tail;
            journal->mem_pool.tail = new_chunk;
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_rollback_journal_mem_chunk_alloc(
    int unit,
    dnx_rollback_journal_t * journal,
    uint32 required_chunk_size,
    dnx_rollback_journal_mem_chunk_t ** new_chunk)
{
    SHR_FUNC_INIT_VARS(unit);
    if ((0 != journal->max_size) && (journal->max_size < journal->mem_pool.total_bytes_allocated + required_chunk_size))
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: journal exceeds to maximum size allowed .\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    DNX_ROLLBACK_JOURNAL_ALLOC(unit, *new_chunk, dnx_rollback_journal_mem_chunk_t *,
                               required_chunk_size, "new rollback journal mem chunk", journal->type, journal->subtype);
    if (NULL == *new_chunk)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "rollback journal ERROR: failed to alloc chunk memory.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    (*new_chunk)->next = NULL;
    (*new_chunk)->prev = NULL;
    (*new_chunk)->data_size = required_chunk_size - DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
    (*new_chunk)->num_allocated_bytes = 0;
    (*new_chunk)->available_space_at_end = required_chunk_size - DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
    journal->mem_pool.total_bytes_allocated += required_chunk_size;
    journal->mem_pool.num_chunks++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_rollback_journal_mem_chunk_free(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_mem_chunk_t * mem_chunk)
{
    SHR_FUNC_INIT_VARS(unit);

    

    if (journal->mem_pool.head == mem_chunk)
        journal->mem_pool.head = mem_chunk->next;

    if (journal->mem_pool.tail == mem_chunk)
        journal->mem_pool.tail = mem_chunk->prev;

    if (mem_chunk->prev)
        mem_chunk->prev->next = mem_chunk->next;
    if (mem_chunk->next)
        mem_chunk->next->prev = mem_chunk->prev;

    journal->mem_pool.total_bytes_allocated -= mem_chunk->data_size + DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
    journal->mem_pool.num_chunks--;

    sal_free(mem_chunk);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_entry_alloc(
    int unit,
    dnx_rollback_journal_t * journal,
    uint32 entry_total_size,
    uint32 entry_metadata_size,
    dnx_rollback_journal_entry_t ** entry)
{
    dnx_rollback_journal_mem_chunk_t *pool_tail = NULL;
    dnx_rollback_journal_mem_chunk_t *new_chunk = NULL;
    uint32 required_chunk_size = DNX_ROLLBACK_JOURNAL_CHUNK_SIZE;
    uint32 use_a_whole_new_chunk = 0;

    SHR_FUNC_INIT_VARS(unit);

    
    *entry = NULL;

    pool_tail = journal->mem_pool.tail;

    if (pool_tail && pool_tail->available_space_at_end >= entry_total_size)
    {
        DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnx_rollback_journal_entry_t *, *entry, pool_tail);
        (*entry)->mem_chunk = pool_tail;
        (*entry)->split_offset = 0;
        pool_tail->num_allocated_bytes += entry_total_size;
        pool_tail->available_space_at_end -= entry_total_size;
    }
    else
    {
        if (entry_total_size >= DNX_ROLLBACK_JOURNAL_CHUNK_SIZE - DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE)
        {
            required_chunk_size = entry_total_size + DNX_ROLLBACK_JOURNAL_MEM_CHUNK_METADATA_SIZE;
            use_a_whole_new_chunk = 1;
        }

        SHR_IF_ERR_EXIT(dnx_rollback_journal_mem_chunk_alloc(unit, journal, required_chunk_size, &new_chunk));

        if (use_a_whole_new_chunk)
        {
            DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnx_rollback_journal_entry_t *, *entry, new_chunk);
            (*entry)->mem_chunk = new_chunk;
            (*entry)->split_offset = 0;
            new_chunk->num_allocated_bytes += entry_total_size;
            new_chunk->available_space_at_end -= entry_total_size;
            
            SHR_IF_ERR_EXIT(dnx_rollback_journal_mempool_insert(unit, journal, new_chunk, TRUE));
        }
        else
        {
            
            if (pool_tail && (DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE <= pool_tail->available_space_at_end))
            {
                DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnx_rollback_journal_entry_t *, *entry, pool_tail);
                (*entry)->mem_chunk = pool_tail;
                (*entry)->split_offset = pool_tail->available_space_at_end;
                pool_tail->num_allocated_bytes += pool_tail->available_space_at_end;
                pool_tail->available_space_at_end = 0;
                new_chunk->num_allocated_bytes += entry_total_size - (*entry)->split_offset;
                new_chunk->available_space_at_end -= entry_total_size - (*entry)->split_offset;
            }
            else
            {
                DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(dnx_rollback_journal_entry_t *, *entry, new_chunk);
                (*entry)->mem_chunk = new_chunk;
                (*entry)->split_offset = 0;
                new_chunk->num_allocated_bytes += entry_total_size;
                new_chunk->available_space_at_end -= entry_total_size;
            }

            SHR_IF_ERR_EXIT(dnx_rollback_journal_mempool_insert(unit, journal, new_chunk, FALSE));
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_stamp_alloc(
    int unit,
    dnx_rollback_journal_t * journal,
    char *stamp_str,
    uint32 stamp_size)
{
    dnx_rollback_journal_mem_chunk_t *pool_tail = NULL;
    dnx_rollback_journal_mem_chunk_t *new_chunk = NULL;
    uint32 required_chunk_size = DNX_ROLLBACK_JOURNAL_CHUNK_SIZE;
    dnx_rollback_journal_entry_t *entry;
    uint32 stamp_size_plus_one_aligned = DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(stamp_size + 1);
    char *data_ptr;

    SHR_FUNC_INIT_VARS(unit);

    pool_tail = journal->mem_pool.tail;
    if (!(pool_tail && stamp_size_plus_one_aligned < pool_tail->available_space_at_end))
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_mem_chunk_alloc(unit, journal, required_chunk_size, &new_chunk));

        SHR_IF_ERR_EXIT(dnx_rollback_journal_mempool_insert(unit, journal, new_chunk, FALSE));
        pool_tail = journal->mem_pool.tail;
    }

    DNX_ROLLBACK_JOURNAL_GET_MEM_CHUNK_CUR_ALLOC_PTR(char *,
                                                     data_ptr,
                                                     pool_tail);

    sal_memcpy(data_ptr, stamp_str, stamp_size);
    data_ptr[stamp_size] = 0;
    pool_tail->available_space_at_end -= stamp_size_plus_one_aligned;
    pool_tail->num_allocated_bytes += stamp_size_plus_one_aligned;

    entry = journal->tail;
    while (entry != NULL && entry->stamp == NULL)
    {
        entry->stamp = data_ptr;
        entry->stamp_mem_chunk = pool_tail;
        entry->stamp_size = 0;
        entry = entry->prev;
    }
    journal->tail->stamp_size = stamp_size_plus_one_aligned;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_rollback_journal_deque_entry_insert(
    int unit,
    uint8 is_tail,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_entry_t * entry)
{
    SHR_FUNC_INIT_VARS(unit);

    if (0 == journal->entry_counter)
    {
#ifdef DNX_ERR_RECOVERY_VALIDATION
        if ((NULL != journal->tail) || (NULL != journal->head))
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "rollback journal ERROR: journal state is inconsistent on entry push.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif 

        entry->prev = NULL;
        entry->next = NULL;

        journal->tail = entry;
        journal->head = entry;
    }
    else
    {
        
        if (is_tail)
        {
            entry->prev = journal->tail;
            journal->tail->next = entry;
            journal->tail = entry;
        }
        else
        {
            entry->next = journal->head;
            journal->head->prev = entry;
            journal->head = entry;
        }
    }

    journal->entry_counter++;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_stamp_str_free(
    int unit,
    dnx_rollback_journal_t * journal,
    char *stamp,
    dnx_rollback_journal_mem_chunk_t * mem_chunk,
    uint32 stamp_size)
{
    SHR_FUNC_INIT_VARS(unit);

    mem_chunk->num_allocated_bytes -= stamp_size;

    if (mem_chunk->num_allocated_bytes < 0)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: mem_chunk allocation size became negetive.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }

    if (mem_chunk->num_allocated_bytes == 0)
    {
        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_mem_chunk_free(unit, journal, mem_chunk));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_entry_free(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_entry_t * entry)
{
    uint32 total_entry_size;
    dnx_rollback_journal_mem_chunk_t *mem_chunk = NULL;
    dnx_rollback_journal_mem_chunk_t *mem_chunk_2nd = NULL;

    SHR_FUNC_INIT_VARS(unit);

    total_entry_size = DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE
        + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->metadata_size)
        + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->payload_size);

    DNX_ROLLBACK_JOURNAL_ENTRY_GET_CHUNK_PTR(mem_chunk, entry);
    if (!entry->split_offset)
    {
        mem_chunk->num_allocated_bytes -= total_entry_size;
    }
    else
    {
        mem_chunk_2nd = mem_chunk->next;
        mem_chunk->num_allocated_bytes -= entry->split_offset;
        mem_chunk_2nd->num_allocated_bytes -= total_entry_size - entry->split_offset;

        if (mem_chunk_2nd->num_allocated_bytes < 0)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "rollback journal ERROR: mem_chunk_2nd allocation size became negetive.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }

        if (mem_chunk_2nd->num_allocated_bytes == 0)
        {
            
            SHR_IF_ERR_EXIT(dnx_rollback_journal_mem_chunk_free(unit, journal, mem_chunk_2nd));
        }
    }
    if (mem_chunk->num_allocated_bytes < 0)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: mem_chunk allocation size became negetive.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
    if (mem_chunk->num_allocated_bytes == 0)
    {
        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_mem_chunk_free(unit, journal, mem_chunk));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_rollback_journal_entry_remove(
    int unit,
    uint8 is_tail,
    dnx_rollback_journal_t * journal)
{
    dnx_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if ((NULL == journal->tail) || (NULL == journal->head))
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: no entries found in journal to pop.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }
#endif

    
    if (is_tail)
    {
        entry = journal->tail;

        journal->size -= (DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE
                          + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->metadata_size)
                          + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->payload_size));

        journal->tail = journal->tail->prev;
        if (NULL != journal->tail)
        {
            journal->tail->next = NULL;
        }
    }
    else
    {
        entry = journal->head;

        journal->size -= (DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE
                          + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->metadata_size)
                          + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->payload_size));

        journal->head = journal->head->next;
        if (NULL != journal->head)
        {
            journal->head->prev = NULL;
        }
    }

    
    if (NULL != entry->stamp)
    {
#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE
        if (entry->stamp_size)
        {
            SHR_IF_ERR_EXIT(dnx_rollback_journal_stamp_str_free(unit,
                                                                journal, entry->stamp, entry->stamp_mem_chunk,
                                                                entry->stamp_size));
        }
#else
        sal_free(entry->stamp);
#endif
        entry->stamp = NULL;
    }

    
#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE
    SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_free(unit, journal, entry));
#else
    sal_free(entry);
#endif

    entry = NULL;

    journal->entry_counter--;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    if (((NULL == journal->head) || (NULL == journal->tail)) && (0 != journal->entry_counter))
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "sw state journal ERROR: invalid entry found in journal.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    
    if (0 == journal->entry_counter)
    {
        journal->head = NULL;
        journal->tail = NULL;

        if (journal->mem_pool.total_bytes_allocated != 0 ||
            journal->mem_pool.num_chunks != 0 || journal->mem_pool.head || journal->mem_pool.tail)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "rollback journal ERROR: all journal entries freed but the mem_pool is not free yet.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static void
dnx_rollback_journal_entry_examine(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_entry_t ** entry,
    uint8 is_tail)
{
    if (is_tail)
    {
        *entry = (NULL == journal->tail) ? NULL : journal->tail;
    }
    else
    {
        *entry = (NULL == journal->head) ? NULL : journal->head;
    }
}

static shr_error_e
dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_entry_t * entry,
    uint8 **metadata_ptr,
    uint8 **payload_ptr,
    uint32 *should_release_metadata,
    uint32 *should_release_payload)
{
    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint8 *metadata_new = NULL;
    uint8 *payload_new = NULL;
    uint8 *next_chunk_data_ptr = NULL;
    uint32 part1_size = 0;

    SHR_FUNC_INIT_VARS(unit);

    *should_release_metadata = 0;
    *should_release_payload = 0;

    if (!entry->split_offset)
    {
        
        DNX_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
        DNX_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);
    }
    else
    {
        
        DNX_ROLLBACK_JOURNAL_ENTRY_GET_NEXT_CHUNK_DATA_PTR(uint8 *,
                                                           next_chunk_data_ptr,
                                                           entry);
        if (entry->split_offset < DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE + entry->metadata_size)
        {
            

            if (entry->split_offset == DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE)
            {
                
                metadata = next_chunk_data_ptr;
                payload = next_chunk_data_ptr + entry->metadata_size;
            }
            else
            {
                
                part1_size = entry->split_offset - DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE;
                DNX_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
                DNX_ROLLBACK_JOURNAL_ALLOC(unit, metadata_new, uint8 *,
                                           entry->metadata_size,
                                           "space to merge metadata",
                                           journal->type,
                                           journal->subtype);
                if (NULL == metadata_new)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                             "rollback journal ERROR: cannot allocate memory to merge meatadata.\n%s%s%s",
                                             EMPTY, EMPTY, EMPTY);
                }
                sal_memcpy(metadata_new, metadata, part1_size);
                sal_memcpy(metadata_new + part1_size, next_chunk_data_ptr, entry->metadata_size - part1_size);
            }
            if (entry->payload_size > 0)
            {
                payload =
                    next_chunk_data_ptr + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->metadata_size) - part1_size;
            }
        }
        else
        {
            
            DNX_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(metadata, entry);
            if (entry->split_offset <= (DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE
                                        + DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->metadata_size)))
            {
                
                payload = next_chunk_data_ptr;
            }
            else
            {
                
                part1_size = entry->split_offset
                    - DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE
                    - DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(entry->metadata_size);
                DNX_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(payload, entry);
                DNX_ROLLBACK_JOURNAL_ALLOC(unit, payload_new, uint8 *,
                                           entry->payload_size,
                                           "space to merge payload",
                                           journal->type,
                                           journal->subtype);
                if (NULL == payload_new)
                {
                    SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                             "rollback journal ERROR: cannot allocate memory to merge payload.\n%s%s%s",
                                             EMPTY, EMPTY, EMPTY);
                }
                sal_memcpy(payload_new, payload, part1_size);
                sal_memcpy(payload_new + part1_size, next_chunk_data_ptr, entry->payload_size - part1_size);
            }
        }
    }

    if (metadata_new)
    {
        *metadata_ptr = metadata_new;
        *should_release_metadata = 1;
    }
    else
    {
        *metadata_ptr = metadata;
        *should_release_metadata = 0;
    }

    if (payload_new)
    {
        *payload_ptr = payload_new;
        *should_release_payload = 1;
    }
    else
    {
        *payload_ptr = payload;
        *should_release_payload = 0;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_rollback_journal_entry_rollback_traverse(
    int unit,
    dnx_rollback_journal_t * journal,
    dnx_rollback_journal_entry_t * top)
{
    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint8 *metadata_traversed = NULL;
    uint8 *payload_traversed = NULL;

    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;
    uint32 should_release_metadata_traversed = 0;
    uint32 should_release_payload_traversed = 0;

    dnx_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    

    SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, top,
                                                                               &metadata, &payload,
                                                                               &should_release_metadata,
                                                                               &should_release_payload));

    
    entry = top->next;

    
    while (NULL != entry)
    {
        
        if (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                       &metadata_traversed,
                                                                                       &payload_traversed,
                                                                                       &should_release_metadata_traversed,
                                                                                       &should_release_payload_traversed));

            SHR_IF_ERR_EXIT(top->rollback_traverse_handler
                            (unit, metadata, payload, metadata_traversed, payload_traversed));
            if (should_release_metadata_traversed)
                sal_free(metadata_traversed);
            if (should_release_payload_traversed)
                sal_free(payload_traversed);
        }

        entry = entry->next;
    }

    if (should_release_metadata)
        sal_free(metadata);
    if (should_release_payload)
        sal_free(payload);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


uint8
dnx_rollback_journal_is_done_init(
    int unit)
{
    return (SOC_IS_DONE_INIT(unit) || (dnx_rollback_journal_pool[unit].is_init_restrict_bypassed_counter > 0));
}


shr_error_e
dnx_rollback_journal_is_init_check_bypass(
    int unit,
    uint8 is_on)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_on)
    {
        dnx_rollback_journal_pool[unit].is_init_restrict_bypassed_counter++;
    }
    else
    {
        if (0 == dnx_rollback_journal_pool[unit].is_init_restrict_bypassed_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: Init check bypass counter cannot be less than 0.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }

        dnx_rollback_journal_pool[unit].is_init_restrict_bypassed_counter--;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


uint8
dnx_rollback_journal_is_error_recovery_bypassed(
    int unit)
{
    if (dnx_rollback_journal_pool[unit].is_init_restrict_bypassed_counter != 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


uint8
dnx_rollback_journal_is_on(
    int unit,
    dnx_rollback_journal_handle_t handle)
{
    
    if ((handle >= DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM)
        || (NULL == dnx_rollback_journal_pool[unit].entries[handle])
        || (NULL == dnx_rollback_journal_pool[unit].entries[handle]->cbs.is_on))
    {
        return FALSE;
    }

    
    if (dnx_rollback_journal_pool[unit].entries[handle]->is_journaled_disabled_counter > 0)
    {
        return FALSE;
    }

    return dnx_rollback_journal_pool[unit].entries[handle]->cbs.is_on(unit);
}


shr_error_e
dnx_rollback_journal_entry_new(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint8 *metadata,
    uint32 metadata_size,
    uint8 *payload,
    uint32 payload_size,
    dnx_rollback_journal_entry_state default_state,
    dnx_rollback_journal_rollback_handler rollback_handler,
    dnx_rollback_journal_rollback_traverse_handler rollback_traverse_handler,
    dnx_rollback_journal_print_handler print_handler)
{
    uint32 total_size = 0;
    uint32 metadata_size_aligned = 0;
    uint32 payload_size_aligned = 0;

    dnx_rollback_journal_t *journal = NULL;
    dnx_rollback_journal_entry_t *entry = NULL;
    uint8 *entry_metadata = NULL;
    uint8 *entry_payload = NULL;
    uint8 *next_chunk_data_ptr = NULL;
    uint32 part1_size = 0;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    if ((NULL == metadata))
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: invalid parameter.\n%s%s%s", EMPTY, EMPTY,
                                                    EMPTY);
    }

    if (NULL == rollback_handler)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: rollback handler cannot be null.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    
    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    metadata_size_aligned = DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(metadata_size);
    payload_size_aligned = DNX_ROLLBACK_JOURNAL_GET_ALIGNED_SIZE(payload_size);
    total_size = DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE + metadata_size_aligned + payload_size_aligned;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    if ((0 != journal->max_size) && (journal->max_size < (journal->size + total_size)))
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: journal exceeds to maximum size allowed .\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    

#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE
    DNX_ROLLBACK_JOURNAL_ENTRY_ALLOC(unit, &entry, dnx_rollback_journal_entry_t *, total_size, metadata_size_aligned,
                                     "new rollback journal entry", journal, journal->type, journal->subtype);
#else
    DNX_ROLLBACK_JOURNAL_ALLOC(unit, entry, dnx_rollback_journal_entry_t *, total_size, "new rollback journal entry",
                               journal->type, journal->subtype);
#endif

    if (NULL == entry)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_MEMORY, "rollback journal ERROR: failed to alloc entry's data.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    
    entry->next = NULL;
    entry->prev = NULL;
    entry->state = default_state;
    entry->metadata_size = metadata_size;
    entry->payload_size = payload_size;
    entry->rollback_handler = rollback_handler;
    entry->rollback_traverse_handler = rollback_traverse_handler;
    entry->print_handler = print_handler;
    entry->seq_id = (NULL == journal->cbs.seq_id_get) ? 0 : journal->cbs.seq_id_get(unit);
    entry->stamp = NULL;
    entry->stamp_size = 0;
#ifndef DNX_ROLLBACK_JOURNAL_OPT_CODE
    
    entry->split_offset = 0;
#endif

    DNX_ROLLBACK_JOURNAL_ENTRY_GET_METADATA_PTR(entry_metadata, entry);
    DNX_ROLLBACK_JOURNAL_ENTRY_GET_PAYLOAD_PTR(entry_payload, entry);

    if (!entry->split_offset)
    {
        sal_memcpy(entry_metadata, metadata, metadata_size);
        if (payload_size > 0)
        {
            sal_memcpy(entry_payload, payload, payload_size);
        }
    }
    else
    {
        DNX_ROLLBACK_JOURNAL_ENTRY_GET_NEXT_CHUNK_DATA_PTR(uint8 *,
                                                           next_chunk_data_ptr,
                                                           entry);
        if (entry->split_offset < DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE + metadata_size)
        {
            
            if (entry->split_offset > DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE)
            {
                
                part1_size = entry->split_offset - DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE;
                sal_memcpy(entry_metadata, metadata, part1_size);
            }
            
            sal_memcpy(next_chunk_data_ptr, metadata + part1_size, metadata_size - part1_size);
            if (payload_size > 0)
            {
                sal_memcpy(next_chunk_data_ptr + metadata_size_aligned - part1_size, payload, payload_size);
            }
        }
        else
        {
            
            sal_memcpy(entry_metadata, metadata, metadata_size);

            if (entry->split_offset > (DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE + metadata_size_aligned))
            {
                
                part1_size = entry->split_offset - DNX_ROLLBACK_JOURNAL_ENTRY_STRUCT_SIZE - metadata_size_aligned;
                sal_memcpy(entry_payload, payload, part1_size);
            }
            
            sal_memcpy(next_chunk_data_ptr, payload + part1_size, payload_size - part1_size);
        }
    }

    
    SHR_IF_ERR_EXIT(dnx_rollback_journal_deque_entry_insert(unit, journal->is_inverse_journaling, journal, entry));

    
    journal->size += total_size;

    
    journal->is_distinct = FALSE;

    
    if (journal->is_logging && print_handler != NULL)
    {
        sal_printf("################################# Journal INSERT Entry ID: %d #################################\n",
                   entry->seq_id);
        SHR_IF_ERR_EXIT(print_handler(unit, metadata, payload));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_new(
    int unit,
    uint8 is_inverse_journaling,
    dnx_rollback_journal_type_e type,
    dnx_rollback_journal_subtype_e subtype,
    dnx_rollback_journal_cbs_t cbs,
    dnx_rollback_journal_handle_t * handle)
{
    dnx_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (NULL == handle)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: invalid parameter.\n%s%s%s", EMPTY, EMPTY,
                                 EMPTY);
    }

    
    *handle = DNX_ROLLBACK_JOURNAL_INVALID_HANDLE;

    if (dnx_rollback_journal_pool[unit].count >= DNX_ROLLBACK_JOURNAL_JOURNAL_MAX_NUM)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: cannot create new journal, maximum number of journals allowed reached.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    
    DNX_ROLLBACK_JOURNAL_ALLOC(unit, dnx_rollback_journal_pool[unit].entries[dnx_rollback_journal_pool[unit].count],
                               dnx_rollback_journal_t *, sizeof(dnx_rollback_journal_t), "rollback journal alloc",
                               type, subtype);

    
    journal = dnx_rollback_journal_pool[unit].entries[dnx_rollback_journal_pool[unit].count];

    if (NULL == journal)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "rollback journal ERROR: cannot create new journal.\n%s%s%s", EMPTY,
                                 EMPTY, EMPTY);
    }

    sal_memset(journal, 0x0, sizeof(dnx_rollback_journal_t));

    
    journal->head = NULL;
    journal->tail = NULL;
    journal->is_distinct = FALSE;
    journal->is_logging = FALSE;
    journal->entry_counter = 0;
    journal->size = 0;
    journal->is_journaled_disabled_counter = 0;
    journal->max_size = DNX_ROLLBACK_JOURNAL_MAX_SIZE;
    journal->cbs = cbs;
    journal->is_inverse_journaling = is_inverse_journaling;
    journal->type = type;
    journal->subtype = subtype;

    
    *handle = dnx_rollback_journal_pool[unit].count++;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_destroy(
    int unit,
    dnx_rollback_journal_handle_t * handle)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_VALIDATE_HANDLE(*handle);

    
    SHR_IF_ERR_EXIT(dnx_rollback_journal_clear(unit, FALSE, *handle));
    sal_free(dnx_rollback_journal_pool[unit].entries[*handle]);
    dnx_rollback_journal_pool[unit].entries[*handle] = NULL;

    dnx_rollback_journal_pool[unit].count--;

    *handle = DNX_ROLLBACK_JOURNAL_INVALID_HANDLE;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_disabled_counter_change(
    int unit,
    uint8 is_disable,
    dnx_rollback_journal_handle_t handle,
    uint8 is_unsafe)
{
    dnx_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if (NULL == dnx_rollback_journal_pool[unit].entries[handle])
    {
        SHR_EXIT();
    }

    if (!is_unsafe
        && (NULL == dnx_rollback_journal_pool[unit].entries[handle]->cbs.is_on
            || !dnx_rollback_journal_pool[unit].entries[handle]->cbs.is_on(unit)))
    {
        SHR_EXIT();
    }

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (is_disable)
    {
        journal->is_journaled_disabled_counter++;
    }
    else
    {
        if (0 == journal->is_journaled_disabled_counter)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: disabled counter is already zero.\n%s%s%s",
                                     EMPTY, EMPTY, EMPTY);
        }
        journal->is_journaled_disabled_counter--;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_disabled_counter_get(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint32 *counter)
{
    dnx_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    
    if (NULL == dnx_rollback_journal_pool[unit].entries[handle])
    {
        SHR_EXIT();
    }

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    *counter = journal->is_journaled_disabled_counter;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_rollback(
    int unit,
    uint8 only_head_rollback,
    dnx_rollback_journal_handle_t handle)
{
    shr_error_e rv_rollback = _SHR_E_NONE;
    dnx_rollback_journal_t *journal = NULL;
    dnx_rollback_journal_entry_t *entry = NULL;
    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (0 != journal->is_journaled_disabled_counter)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                 "rollback journal ERROR: journal cannot be rolled back when suppression is active.\n%s%s%s",
                                 EMPTY, EMPTY, EMPTY);
    }

    
    dnx_rollback_journal_pool[unit].is_any_journal_rolling_back = TRUE;

    while (journal->entry_counter > 0)
    {
        rv_rollback = _SHR_E_NONE;

        
        dnx_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (NULL == entry)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif 

        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                   &metadata, &payload,
                                                                                   &should_release_metadata,
                                                                                   &should_release_payload));

#ifdef DNX_ERR_RECOVERY_VALIDATION
        
        if (NULL == entry->rollback_handler)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "rollback journal ERROR: entry's rollback handler cannot be null.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif 

        if (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            rv_rollback = entry->rollback_handler(unit, metadata, payload, entry->stamp);
            if (journal->is_logging && entry->print_handler != NULL)
            {
                sal_printf
                    ("################################# Journal ROLLBACK Entry ID: %d #################################\n",
                     entry->seq_id);
                SHR_IF_ERR_EXIT(entry->print_handler(unit, metadata, payload));
            }
        }

        if (should_release_metadata)
            sal_free(metadata);
        if (should_release_payload)
            sal_free(payload);

        
        if ((NULL != entry->rollback_traverse_handler)
            && (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state)) && (_SHR_E_NONE == rv_rollback))
        {
            rv_rollback = dnx_rollback_journal_entry_rollback_traverse(unit, journal, entry);
        }

        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_remove(unit, FALSE, journal));

        
        SHR_IF_ERR_EXIT_NO_MSG(rv_rollback);

        if (only_head_rollback)
        {
            break;
        }
    }

    dnx_rollback_journal_pool[unit].is_any_journal_rolling_back = FALSE;

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    if ((0 == journal->entry_counter) && (0 != journal->size))
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: journal entry count zero but size is not zero.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

exit:
    if (dnx_rollback_journal_pool[unit].is_any_journal_rolling_back)
    {
        dnx_rollback_journal_pool[unit].is_any_journal_rolling_back = FALSE;
    }
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_clear(
    int unit,
    uint8 only_head_clear,
    dnx_rollback_journal_handle_t handle)
{
    dnx_rollback_journal_t *journal = NULL;
#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE
    dnx_rollback_journal_mem_chunk_t *mem_chunk = NULL;
    dnx_rollback_journal_mem_chunk_t *next_mem_chunk = NULL;
#endif
    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (0 != journal->is_journaled_disabled_counter)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: journal cannot be cleared when suppression is active.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE
    if (only_head_clear)
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_remove(unit, FALSE, journal));
    }
    else
    {
        

        
        mem_chunk = journal->mem_pool.head;
        while (mem_chunk)
        {
            next_mem_chunk = mem_chunk->next;
            sal_free(mem_chunk);
            mem_chunk = next_mem_chunk;
        }
    }
#else
    
    while (journal->entry_counter > 0)
    {
        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_remove(unit, FALSE, journal));

        if (only_head_clear)
        {
            break;
        }
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    if (!only_head_clear && ((0 != journal->size) || (0 != journal->entry_counter)))
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: journal cleared but entries or size are non zero.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 
#endif 
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_get_head_seq_id(
    int unit,
    uint32 *seq_id,
    dnx_rollback_journal_handle_t handle)
{
    dnx_rollback_journal_t *journal = NULL;
    dnx_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == seq_id)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_PARAM,
                                                    "rollback journal ERROR: sequence id parameter cannot be null.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    
    *seq_id = 0;

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (journal->entry_counter > 0)
    {
        
        dnx_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
        if (NULL == entry)
        {
            DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                        "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                                        EMPTY, EMPTY, EMPTY);
        }
#endif 

        *seq_id = entry->seq_id;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_invalidate_last_inserted_entry(
    int unit,
    dnx_rollback_journal_handle_t handle)
{
    dnx_rollback_journal_t *journal = NULL;
    dnx_rollback_journal_entry_t *entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (journal->entry_counter == 0)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: Cannot invalidate last inserted entry. No entries found in journal.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    
    dnx_rollback_journal_entry_examine(unit, journal, &entry, FALSE);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == entry)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: NULL entry retrieved from journal, but one or more entries present in it.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    
    if (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID != entry->state)
    {
        entry->state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_distinct(
    int unit,
    dnx_rollback_journal_handle_t handle,
    dnx_rollback_journal_is_distinct_cb is_distinct_cb)
{
    uint8 are_distinct = FALSE;

    dnx_rollback_journal_t *journal = NULL;

    dnx_rollback_journal_entry_t *entry = NULL;
    dnx_rollback_journal_entry_t *trav_entry = NULL;

    uint8 *metadata = NULL;
    uint8 *payload = NULL;

    uint8 *trav_metadata = NULL;
    uint8 *trav_payload = NULL;

    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;
    uint32 should_release_trav_metadata = 0;
    uint32 should_release_trav_payload = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (journal->is_distinct)
    {
        SHR_EXIT();
    }

    
    entry = journal->head;
    while (entry != NULL)
    {
        
        if (!DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            entry = entry->next;
            continue;
        }

        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                   &metadata, &payload,
                                                                                   &should_release_metadata,
                                                                                   &should_release_payload));

        
        trav_entry = entry->next;
        while (trav_entry != NULL)
        {
            if (!DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(trav_entry->state))
            {
                trav_entry = trav_entry->next;
                continue;
            }

            
            SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                       &trav_metadata, &trav_payload,
                                                                                       &should_release_trav_metadata,
                                                                                       &should_release_trav_payload));
            SHR_IF_ERR_EXIT(is_distinct_cb(unit, metadata, payload, trav_metadata, trav_payload, &are_distinct));
            if (should_release_trav_metadata)
                sal_free(trav_metadata);
            if (should_release_trav_payload)
                sal_free(trav_payload);

            if (!are_distinct && (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_ALWAYS_VALID != trav_entry->state))
            {
                trav_entry->state = DNX_ROLLBACK_JOURNAL_ENTRY_STATE_INVALID;
            }

            trav_entry = trav_entry->next;
        }

        if (should_release_metadata)
            sal_free(metadata);
        if (should_release_payload)
            sal_free(payload);

        entry = entry->next;
    }

    journal->is_distinct = TRUE;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_entries_stamp(
    int unit,
    dnx_rollback_journal_handle_t handle,
    char *str)
{
    dnx_rollback_journal_t *journal = NULL;
    uint32 size = 0;
#ifndef DNX_ROLLBACK_JOURNAL_OPT_CODE
    dnx_rollback_journal_entry_t *entry = NULL;
#endif

    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    if (!journal->tail || journal->tail->stamp)
    {
        SHR_EXIT();
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (NULL == str)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: stamp string cannot be null.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    size = sal_strlen(str);

#ifdef DNX_ERR_RECOVERY_VALIDATION
    if (size > DNX_ROLLBACK_JOURNAL_MAX_STAMP_SIZE)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: stamp size exceeds the maximum allowed.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

#ifdef DNX_ROLLBACK_JOURNAL_OPT_CODE
    SHR_IF_ERR_EXIT(dnx_rollback_journal_stamp_alloc(unit, journal, str, size));
#else
    entry = journal->tail;
    while (entry != NULL && entry->stamp == NULL)
    {
        
        DNX_ROLLBACK_JOURNAL_ALLOC(unit, entry->stamp, char *,
                                   size + 1,
                                   "snapshot journal stamp array",
                                   journal->type,
                                   journal->subtype);
        if (NULL == entry->stamp)
        {
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL,
                                     "rollback journal ERROR: cannot allocate memory for stamp.\n%s%s%s", EMPTY, EMPTY,
                                     EMPTY);
        }

        sal_memset(entry->stamp, 0x0, size + 1);
        
        sal_strncpy(entry->stamp, str, size);

        entry = entry->prev;
    }
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}


uint8
dnx_rollback_journal_is_any_journal_rolling_back(
    int unit)
{
    return dnx_rollback_journal_pool[unit].is_any_journal_rolling_back;
}


shr_error_e
dnx_rollback_journal_print(
    int unit,
    dnx_rollback_journal_handle_t handle)
{

    uint32 entry_number = 0;
    dnx_rollback_journal_t *journal = NULL;
    dnx_rollback_journal_entry_t *entry = NULL;
#ifdef DNX_ERR_RECOVERY_VALIDATION
    dnx_rollback_journal_entry_t *last_entry = NULL;
#endif

    uint8 *metadata = NULL;
    uint8 *payload = NULL;
    uint32 should_release_metadata = 0;
    uint32 should_release_payload = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    
    switch (journal->type)
    {
        case DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK:
            LOG_CLI((BSL_META("Journal type: ROLLBACK\n")));
            break;
        case DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON:
            LOG_CLI((BSL_META("Journal type: COMPARISON\n")));
            break;
        default:
            LOG_CLI((BSL_META("Journal type: UNKNOWN\n")));
    }

    switch (journal->subtype)
    {
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE:
            LOG_CLI((BSL_META("Journal subtype: SWSTATE\n")));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL:
            LOG_CLI((BSL_META("Journal subtype: DBAL\n")));
            break;
        case DNX_ROLLBACK_JOURNAL_SUBTYPE_GENERIC:
            LOG_CLI((BSL_META("Journal subtype: GENERIC\n")));
            break;
        default:
            LOG_CLI((BSL_META("Journal subtype: UNKNOWN\n")));
    }

    if (dnx_rollback_journal_is_on(unit, handle))
    {
        LOG_CLI((BSL_META("State: ON\n")));
    }
    else
    {
        LOG_CLI((BSL_META("State: OFF\n")));
    }

    LOG_CLI((BSL_META("Entries count: %u\n"), journal->entry_counter));

    LOG_CLI((BSL_META("Size: %u\n"), journal->size));

    LOG_CLI((BSL_META("Max size: %u\n"), journal->max_size));

    if (!(journal->is_inverse_journaling))
    {
        LOG_CLI((BSL_META("Journaling: head to tail\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Journaling: tail to head\n")));
    }

    if (journal->is_distinct)
    {
        LOG_CLI((BSL_META("Is distinct: YES\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Is distinct: NO\n")));
    }

    LOG_CLI((BSL_META("Handle id: %u\n"), handle));

    LOG_CLI((BSL_META("\n")));

    
    entry = journal->head;

    while (NULL != entry)
    {
        
#ifdef DNX_ERR_RECOVERY_VALIDATION
        last_entry = entry;
#endif

        
        SHR_IF_ERR_EXIT(dnx_rollback_journal_entry_get_merged_metadata_payload_ptr(unit, journal, entry,
                                                                                   &metadata, &payload,
                                                                                   &should_release_metadata,
                                                                                   &should_release_payload));

        LOG_CLI((BSL_META("Entry number in journal: %u\n"), entry_number));

        LOG_CLI((BSL_META("Sequence id in transaction: %u\n"), entry->seq_id));

        if (DNX_ROLLBACK_JOURNAL_ENTRY_STATE_IS_VALID(entry->state))
        {
            LOG_CLI((BSL_META("Is entry valid: YES\n")));
        }
        else
        {
            LOG_CLI((BSL_META("Is entry valid: NO\n")));
        }

        if (NULL != entry->stamp)
        {
            LOG_CLI((BSL_META("Associated stamp is: %s\n"), entry->stamp));
        }
        else
        {
            LOG_CLI((BSL_META("Associated stamp is: NONE\n")));
        }

        if (NULL != entry->print_handler)
        {
            SHR_IF_ERR_EXIT(entry->print_handler(unit, metadata, payload));
        }

        if (should_release_metadata)
            sal_free(metadata);
        if (should_release_payload)
            sal_free(payload);

        entry = entry->next;
        entry_number++;
        LOG_CLI((BSL_META("\n")));
    }

#ifdef DNX_ERR_RECOVERY_VALIDATION
    
    if (last_entry != journal->tail)
    {
        DNX_ROLLBACK_JOURNAL_IF_ERR_ASSERT_WITH_LOG(SOC_E_INTERNAL,
                                                    "rollback journal ERROR: last non-null entry in journal is difference than the journal tail.\n%s%s%s",
                                                    EMPTY, EMPTY, EMPTY);
    }
#endif 

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}


shr_error_e
dnx_rollback_journal_logger_state_change_by_type(
    int unit,
    dnx_rollback_journal_handle_t handle)
{
    dnx_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);

    journal->is_logging = !(journal->is_logging);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_rollback_journal_is_logging(
    int unit,
    dnx_rollback_journal_handle_t handle,
    uint8 *is_logging)
{
    dnx_rollback_journal_t *journal = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_logging, _SHR_E_PARAM, "is_logging");

    DNX_ROLLBACK_JOURNAL_GET_JOURNAL_PTR(journal, handle);
    SHR_NULL_CHECK(journal, _SHR_E_PARAM, "journal");

    *is_logging = journal->is_logging;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
