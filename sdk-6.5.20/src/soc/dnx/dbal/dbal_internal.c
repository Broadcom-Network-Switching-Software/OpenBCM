/** \file dbal_internal.c
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * System wide Logical Table Manager internal functions.
 *
 * To be used for:
 *   Access of physical table
 *   Access of pure software tables
 *   Activation of access procedutes (dispatcher) which is
 *     equivalent to 'MBCM and arad_pp_dbal' on SDK6 for JR1.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

#include <soc/drv.h> 
#include "dbal_internal.h"
#include <soc/dnx/mdb.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <sys/stat.h>
#include <dirent.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>


const uint32 dbal_field_legal_sizes[DBAL_NOF_POINTER_TYPES] =
    { 0, SAL_UINT8_NOF_BITS, SAL_UINT16_NOF_BITS, SAL_UINT32_NOF_BITS, 0, 0, SAL_UINT64_NOF_BITS };


#define DBAL_VALIDATE_HANDLE_STATUS_BRFORE_ACTION                                                               \
    if (entry_handle->handle_status < DBAL_HANDLE_STATUS_IN_USE)                                                \
    {                                                                                                           \
        LOG_ERROR(BSL_LOG_MODULE,(BSL_META_U(unit,"invalid status for Handle %d.\n"), entry_handle->handle_id));\
        return;                                                                                                 \
    }


#define DBAL_VALIDATE_VALID_INSTANCE_ID                                                                         \
        DBAL_FIELD_ERR_HANDLE((in_inst_id == INST_ALL),                                                         \
        "Illegal Usage of instance ALL. INST_ALL cannot be used in get APIs\n");


#define DBAL_VALIDATE_POINTER_SIZE(pointer_type, nof_bits)                                                      \
        if ((dbal_field_legal_sizes[pointer_type]) && (nof_bits > dbal_field_legal_sizes[pointer_type]))        \
        {                                                                                                       \
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field size too big for requested pointer\n");                        \
        }

#define DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_VOID()                      \
do                                                                       \
{                                                                        \
    int rv1;                                                             \
          \
    rv1 = bsl_fast_check_two(BSL_SOURCE_DBAL_DNX_CHK,BSL_SOURCE_DBALACCESS_DNX_CHK); \
    if (!rv1)                                                            \
    {                                                                    \
        return;                                                          \
    }                                                                    \
} while (0)

#define DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_ERR()                        \
do                                                                        \
{                                                                         \
    int rv1;                                                             \
          \
    rv1 = bsl_fast_check_two(BSL_SOURCE_DBAL_DNX_CHK,BSL_SOURCE_DBALACCESS_DNX_CHK); \
    if (!rv1)                                                            \
    {                                                                    \
        return 0;                                                        \
    }                                                                    \
} while (0)


static dbal_mngr_info_t *dbal_mngr[BCM_MAX_NUM_UNITS] = { 0 };

uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };

const uint32 zero_buffer_to_compare[DBAL_MAX_SIZE_FOR_GLOBAL_ZERO_BUFFER] = { 0 };

extern dbal_field_types_defs_e dbal_field_id_to_type[DBAL_NOF_FIELDS];
extern shr_error_e dbal_entry_get_handle_update_value_field(
    int unit,
    dbal_entry_handle_t * handle);
extern shr_error_e dbal_hl_non_standard_default_entry_build(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *default_entry);


uint8
dbal_is_j2c_a0(
    int unit)
{
    return (SOC_IS_J2C(unit));
}

uint8
dbal_is_jr2_a0(
    int unit)
{
    return (SOC_IS_JERICHO2_A0(unit));
}

uint8
dbal_is_jr2_b0(
    int unit)
{
    return (SOC_IS_JERICHO2_B(unit));
}

uint8
dbal_is_q2a_a0(
    int unit)
{
    return (SOC_IS_Q2A_A0(unit));
}

uint8
dbal_is_q2a_b0(
    int unit)
{
    return (SOC_IS_Q2A_B(unit));
}

uint8
dbal_is_j2p_a0(
    int unit)
{
    return (SOC_IS_J2P(unit));
}

uint8
dbal_is_dnx2(
    int unit)
{
    return SOC_IS_DNX2(unit);
}

uint8
dbal_is_initiated(
    int unit)
{
    int ii;
    if (dbal_mngr[unit] && dbal_mngr[unit]->status > DBAL_STATUS_NOT_INITIALIZED)
    {
        return TRUE;
    }

    
    for (ii = 0; ii < DBAL_PHYSICAL_RES_SIZE_IN_WORDS; ii++)
    {
        G_dbal_field_full_mask[ii] = 0xffffffff;
    }

    return FALSE;
}

static shr_error_e
dbal_mngr_alloc(
    int unit)
{
    int handles_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(dbal_mngr[unit], sizeof(dbal_mngr_info_t), "dbal_mngr_info_t",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    handles_size = sizeof(dbal_entry_handle_t) * (DBAL_SW_NOF_ENTRY_HANDLES + DBAL_SW_NOF_ENTRY_HANDLES_FOR_COLLECTION);

    
    SHR_ALLOC_SET_ZERO(dbal_mngr[unit]->entry_handles_pool, handles_size,
                       "handles pool", "%d%s%s\r\n", handles_size, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_mngr_dealloc(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_mngr[unit] != NULL)
    {
        
        SHR_FREE(dbal_mngr[unit]->entry_handles_pool);
        SHR_FREE(dbal_mngr[unit]);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_collection_worker_thread_start(
    int unit,
    int pool_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_COLLECTION_PRINTS((BSL_META("signaling to worker thread to start working on pool_id %d \n"), pool_id));

    sal_sem_give(dbal_mngr[unit]->collection_semaphore_array[pool_id]);

    SHR_FUNC_EXIT;
}


STATIC void
dbal_collection_worker_thread(
    void *cookie)
{
    dbal_collection_worker_thread_params_t *worker_thread_params = (dbal_collection_worker_thread_params_t *) cookie;
    int unit = worker_thread_params->unit;
    int my_id = worker_thread_params->mutex_id;
    int rv = SOC_E_NONE;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;
    int collection_pool_size = dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit);
    int first_entry_in_pool = DBAL_SW_NOF_ENTRY_HANDLES + (my_id * collection_pool_size);

    DBAL_COLLECTION_PRINTS((BSL_META("DBAL worker thread %d created!\n"), my_id));

    while (1)
    {
        int entries_counter = 0;
        int curr_handle;
        sal_sem_take(dbal_mngr[unit]->collection_semaphore_array[my_id], sal_sem_FOREVER);
        
        sal_mutex_take(dbal_mngr[unit]->collection_pool_mutex_array[my_id], sal_mutex_FOREVER);
        sal_mutex_take(dbal_mngr[unit]->collection_worker_thread_sync, sal_mutex_FOREVER);

        
        if (dbal_mngr[unit]->collection_signals[my_id] == DBAL_COLLECTION_SIGNAL_DESTROY)
        {
            DBAL_COLLECTION_PRINTS((BSL_META("Destroy signal received %d \n"), my_id));
            
            sal_mutex_destroy(dbal_mngr[unit]->collection_pool_mutex_array[my_id]);
            sal_sem_destroy(dbal_mngr[unit]->collection_semaphore_array[my_id]);

            
            sal_thread_exit(0);
        }

        DBAL_COLLECTION_PRINTS((BSL_META("starting to process pool_id %d first entry %d\n"), my_id,
                                first_entry_in_pool));
        for (curr_handle = first_entry_in_pool; curr_handle < first_entry_in_pool + collection_pool_size; curr_handle++)
        {
            dbal_entry_handle_t *entry_handle = &(dbal_mngr[unit]->entry_handles_pool[curr_handle]);
            if (entry_handle->handle_status != DBAL_HANDLE_STATUS_ACTION_COLLECTED)
            {
                
                
                
            }
            else
            {
                rv = dbal_entry_commit_normal(unit, entry_handle);
                if (rv)
                {
                    
                    DBAL_COLLECTION_PRINTS((BSL_META("\n ERROR! commit failed ID %d pool ID %d \n"), curr_handle,
                                            my_id));
                    break;
                }
                entries_counter++;
            }

            dbal_mngr[unit]->entry_handles_pool[curr_handle].handle_status = DBAL_HANDLE_STATUS_AVAILABLE;
        }
        DBAL_COLLECTION_PRINTS((BSL_META("pool %d - processing completed %d entries added\n"), my_id, entries_counter));

        
        sal_mutex_give(dbal_mngr[unit]->collection_worker_thread_sync);

        
        sal_mutex_give(dbal_mngr[unit]->collection_pool_mutex_array[my_id]);
    }

    

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof(thread_name));
    LOG_ERROR(BSL_LS_SOC_DMA, (BSL_META_U(unit, "AbnormalThreadExit:%s, unit %d\n"), thread_name, unit));

    sal_thread_exit(0);
}

shr_error_e
dbal_mutexes_init(
    int unit)
{
    int mutex_index;
    SHR_FUNC_INIT_VARS(unit);

    dbal_mngr[unit]->handle_mutex = sal_mutex_create("dbal handle mutex");

    
    for (mutex_index = 0; mutex_index < DBAL_NOF_SHARED_TABLE_MUTEXES; mutex_index++)
    {
        dbal_mngr[unit]->protect_info[mutex_index].pre_table_mutex = sal_mutex_create("per table mutex");
        dbal_mngr[unit]->protect_info[mutex_index].table_id = DBAL_TABLE_EMPTY;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_mutexes_deinit(
    int unit)
{
    int mutex_index;
    SHR_FUNC_INIT_VARS(unit);

    for (mutex_index = 0; mutex_index < DBAL_NOF_SHARED_TABLE_MUTEXES; mutex_index++)
    {
        sal_mutex_destroy(dbal_mngr[unit]->protect_info[mutex_index].pre_table_mutex);
    }

    sal_mutex_destroy(dbal_mngr[unit]->handle_mutex);

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_internal_init(
    int unit)
{
    uint8 is_std_1_u8 = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mngr_alloc(unit));
    SHR_IF_ERR_EXIT(dbal_collection_init(unit));
    SHR_IF_ERR_EXIT(dbal_logger_init(unit));
    SHR_IF_ERR_EXIT(dbal_mutexes_init(unit));

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1_u8));
    dbal_mngr[unit]->is_std_1 = is_std_1_u8;
    SHR_IF_ERR_EXIT(dbal_field_string_to_indexes_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_internal_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_mutexes_deinit(unit));
    SHR_IF_ERR_EXIT(dbal_logger_deinit(unit));
    SHR_IF_ERR_EXIT(dbal_mngr_dealloc(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_collection_init(
    int unit)
{
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    int worker_thread_id;
    dbal_collection_worker_thread_params_t worker_thread_params;

    SHR_FUNC_INIT_VARS(unit);

    
    dbal_mngr[unit]->collection_counter = 0;

    if (dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit))
    {
        worker_thread_params.unit = unit;
        dbal_mngr[unit]->collection_worker_thread_sync = sal_mutex_create("worker sync mutex");
        if (dbal_mngr[unit]->collection_worker_thread_sync == NULL)
        {
            SHR_ERR_EXIT(SOC_E_MEMORY, "worker sync mutex allocation failure.");
        }
        for (worker_thread_id = 0; worker_thread_id < DBAL_NOF_COLLECTION_POOLS; worker_thread_id++)
        {
            dbal_mngr[unit]->collection_signals[worker_thread_id] = DBAL_COLLECTION_SIGNAL_PROCESS_POOL;
            worker_thread_params.mutex_id = worker_thread_id;

            dbal_mngr[unit]->collection_pool_mutex_array[worker_thread_id] = sal_mutex_create("collection pool");
            if (dbal_mngr[unit]->collection_pool_mutex_array[worker_thread_id] == NULL)
            {
                SHR_ERR_EXIT(SOC_E_MEMORY, "collection pool mutex allocation failure.");
            }
            dbal_mngr[unit]->collection_semaphore_array[worker_thread_id] =
                sal_sem_create("collection sem", sal_sem_COUNTING, 0);
            if (dbal_mngr[unit]->collection_semaphore_array[worker_thread_id] == NULL)
            {
                SHR_ERR_EXIT(SOC_E_MEMORY, "collection sem allocation failure.");
            }

            sal_snprintf(thread_name, SAL_THREAD_NAME_MAX_LEN, "DBAL_COLLECTION_WORKER_%d", worker_thread_id);
            sal_thread_create(thread_name, SAL_THREAD_STKSZ, 50  , dbal_collection_worker_thread,
                              (void *) (&worker_thread_params));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

uint8
dbal_image_name_is_std_1(
    int unit)
{
    return dbal_mngr[unit]->is_std_1;
}

shr_error_e
dbal_per_table_mutex_reserve(
    int unit,
    dbal_tables_e table_id,
    uint8 *mutex_id)
{
    int mutex_index;

    SHR_FUNC_INIT_VARS(unit);

    for (mutex_index = 0; mutex_index < DBAL_NOF_SHARED_TABLE_MUTEXES; mutex_index++)
    {
        if (dbal_mngr[unit]->protect_info[mutex_index].table_id == DBAL_TABLE_EMPTY)
        {
            dbal_mngr[unit]->protect_info[mutex_index].table_id = table_id;
            (*mutex_id) = mutex_index;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_RESOURCE, " no available mutexes to reserve for table %s \n",
                 dbal_logical_table_to_string(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_per_table_mutex_info_get(
    int unit,
    uint8 mutex_id,
    CONST dbal_specific_table_protection_info_t ** protect_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mutex_id > DBAL_NOF_SHARED_TABLE_MUTEXES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Mutex ID (%d) out of range\n", mutex_id);
    }

    (*protect_info) = &(dbal_mngr[unit]->protect_info[mutex_id]);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_per_table_mutex_release(
    int unit,
    dbal_tables_e table_id,
    uint8 mutex_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mutex_id >= DBAL_NOF_SHARED_TABLE_MUTEXES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " Mutex ID (%d) out of range, table %s \n", mutex_id,
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (dbal_mngr[unit]->protect_info[mutex_id].table_id != table_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error releasing mutex, table %s is not related to mutex %d \n",
                     dbal_logical_table_to_string(unit, table_id), mutex_id);
    }

    dbal_mngr[unit]->protect_info[mutex_id].table_id = DBAL_TABLE_EMPTY;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_handles_init(
    int unit)
{
    int handle_index;

    SHR_FUNC_INIT_VARS(unit);

    for (handle_index = 0; handle_index < DBAL_SW_NOF_ENTRY_HANDLES; handle_index++)
    {
        dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status = DBAL_HANDLE_STATUS_AVAILABLE;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_handles_deinit(
    int unit)
{
    int handle_index, worker_thread_id;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_mngr[unit] == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_INIT, "dbal_mngr not initialized for unit %d\n", unit);
    }

    for (handle_index = 0; handle_index < DBAL_SW_NOF_ENTRY_HANDLES; handle_index++)
    {
        if (dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status != DBAL_HANDLE_STATUS_AVAILABLE)
        {
            LOG_CLI((BSL_META("handle %d was not release table %s\n"), handle_index,
                     dbal_mngr[unit]->entry_handles_pool[handle_index].table->table_name));

            dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status = DBAL_HANDLE_STATUS_AVAILABLE;
        }
    }

    
    if (dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit))
    {
        for (worker_thread_id = 0; worker_thread_id < DBAL_NOF_COLLECTION_POOLS; worker_thread_id++)
        {
            
            sal_mutex_take(dbal_mngr[unit]->collection_pool_mutex_array[worker_thread_id], sal_mutex_FOREVER);
            dbal_mngr[unit]->collection_signals[worker_thread_id] = DBAL_COLLECTION_SIGNAL_DESTROY;
            sal_mutex_give(dbal_mngr[unit]->collection_pool_mutex_array[worker_thread_id]);

            
            sal_sem_give(dbal_mngr[unit]->collection_semaphore_array[worker_thread_id]);
        }
        sal_mutex_destroy(dbal_mngr[unit]->collection_worker_thread_sync);
    }

exit:
    SHR_FUNC_EXIT;
}

void
dbal_status_set(
    int unit,
    dbal_status_e status)
{
    dbal_mngr[unit]->status = status;

    if (status == DBAL_STATUS_DEVICE_INIT_DONE)
    {
        DBAL_DUMP("\nDEVICE INIT DONE\n", 0, dbal_mngr[unit]->logger_info.dbal_file);
    }
}

shr_error_e
dbal_status_get(
    int unit,
    dbal_status_e * status)
{
    (*status) = dbal_mngr[unit]->status;

    return 0;
}

void
dbal_pemla_status_set(
    int unit,
    dbal_status_e status)
{
    dbal_mngr[unit]->pemla_status = status;
}

shr_error_e
dbal_pemla_status_get(
    int unit,
    dbal_status_e * status)
{
    (*status) = dbal_mngr[unit]->pemla_status;

    return 0;
}

shr_error_e
dbal_collection_trigger_set_internal(
    int unit,
    dbal_collection_trigger_type_e trigger_type)
{
    int pool_id = 0;
    int first_entry_in_pool, nof_entries_in_pool;
    int is_task_completed = 0;
    int collection_pool_size = dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    pool_id = DBAL_COLLECTION_POOL_ID_GET(dbal_mngr[unit]->collection_counter);

    first_entry_in_pool = DBAL_SW_NOF_ENTRY_HANDLES + (pool_id * collection_pool_size);
    nof_entries_in_pool = DBAL_COLLECTION_NOF_ENTRIES_IN_POOL(dbal_mngr[unit]->collection_counter);
    DBAL_COLLECTION_PRINTS((BSL_META
                            ("trigger receive to complete pool processing pool %d, first entry = %d nof_entries = %d \n"),
                            pool_id, first_entry_in_pool, nof_entries_in_pool));
    SHR_IF_ERR_EXIT(dbal_collection_worker_thread_start(unit, pool_id));

    
    if (trigger_type == DBAL_COLLECTION_TRIGGER_PROCESS_AND_WAIT)
    {
        while (!is_task_completed)
        {
            int handle_index;

            sal_mutex_take(dbal_mngr[unit]->collection_pool_mutex_array[pool_id], sal_mutex_FOREVER);

            handle_index = collection_pool_size ? (DBAL_SW_NOF_ENTRY_HANDLES + pool_id * DBAL_COLLECTION_POOL_SIZE) : 0;
            if (dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status != DBAL_HANDLE_STATUS_ACTION_COLLECTED)
            {
                is_task_completed = TRUE;
            }
            sal_mutex_give(dbal_mngr[unit]->collection_pool_mutex_array[(pool_id)]);
        }
        DBAL_COLLECTION_PRINTS((BSL_META("pool processing completed %d\n"), pool_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_info_get(
    int unit,
    int entry_handle_id,
    dbal_iterator_info_t ** iterator_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle_id > DBAL_SW_NOF_ENTRY_HANDLES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " handle id (%d) our of limit max %d \n", entry_handle_id,
                     DBAL_SW_NOF_ENTRY_HANDLES);
    }

    (*iterator_info) = &(dbal_mngr[unit]->iterators_pool[entry_handle_id]);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_handle_take_update_init_key_value(
    int unit,
    int entry_handle_id)
{
    int ii;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    table = dbal_mngr[unit]->entry_handles_pool[entry_handle_id].table;

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        if (SHR_BITGET(table->keys_info[ii].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
        {
            dbal_entry_key_field_set(unit, entry_handle_id, table->keys_info[ii].field_id,
                                     &table->keys_info[ii].const_value,
                                     G_dbal_field_full_mask, DBAL_POINTER_TYPE_UINT32);

            if (dbal_mngr[unit]->entry_handles_pool[entry_handle_id].error_info.error_exists)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error when setting init/const value for key field %s. table %s\n",
                             dbal_field_to_string(unit, table->keys_info[ii].field_id), table->table_name);
            }
        }
        else
        {
            continue;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_handle_take_update_init_result_value(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_RES_INFO.nof_result_fields; ii++)
    {
        if (SHR_BITGET(DBAL_RES_INFO.results_info[ii].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
        {
            uint32 const_value[2] = { DBAL_RES_INFO.results_info[ii].const_value, 0 };
            dbal_entry_value_field_set(unit, entry_handle->handle_id, DBAL_RES_INFO.results_info[ii].field_id,
                                       0, DBAL_POINTER_TYPE_ARR_UINT32, const_value, G_dbal_field_full_mask);
        }
        else
        {
            continue;
        }

        if (entry_handle->error_info.error_exists)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error when setting init/const value for result field %s. table %s\n",
                         dbal_field_to_string(unit, DBAL_RES_INFO.results_info[ii].field_id),
                         entry_handle->table->table_name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_collection_get_next_available_pool(
    int unit,
    int *pool_id)
{
    int is_found = 0;
    int pool_id_to_check;

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_mngr[unit]->collection_counter + 1 == DBAL_SW_NOF_ENTRY_HANDLES_FOR_COLLECTION)
    {
        DBAL_COLLECTION_PRINTS((BSL_META("collector counter wrap around \n")));
        pool_id_to_check = 0;
    }
    else
    {
        
        pool_id_to_check = DBAL_COLLECTION_POOL_ID_GET(dbal_mngr[unit]->collection_counter + 1);
    }

    while (!is_found)
    {
        int handle_index;

        
        sal_mutex_take(dbal_mngr[unit]->collection_pool_mutex_array[(pool_id_to_check)], sal_mutex_FOREVER);

        handle_index =
            dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit) ? DBAL_SW_NOF_ENTRY_HANDLES +
            pool_id_to_check * DBAL_COLLECTION_POOL_SIZE : 0;
        if (dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status != DBAL_HANDLE_STATUS_ACTION_COLLECTED)
        {
            is_found = TRUE;
            (*pool_id) = pool_id_to_check;
        }
        sal_mutex_give(dbal_mngr[unit]->collection_pool_mutex_array[(pool_id_to_check)]);
        DBAL_COLLECTION_PRINTS((BSL_META("NOTE! first entry in pool is occupied.. pool %d \n"), pool_id_to_check));
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_take_internal(
    int unit,
    dbal_tables_e table_id,
    uint32 *entry_handle_id,
    uint8 operation)
{
    int handle_index;
    dbal_logical_table_t *table;
    dbal_entry_handle_t *entry_handles_pool_ptr;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED))
    {
        
        if ((operation == DBAL_HANDLE_TAKE_ALLOC)
            || (dbal_mngr[unit]->entry_handles_pool[*entry_handle_id].table_id != table_id))
        {
            
            SHR_IF_ERR_EXIT(sal_mutex_take
                            (dbal_mngr[unit]->protect_info[table->mutex_id].pre_table_mutex, sal_mutex_FOREVER));
        }
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(dbal_mngr[unit]->handle_mutex, sal_mutex_FOREVER));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
    if (table_status == DBAL_TABLE_NOT_INITIALIZED)
    {
        *entry_handle_id = -1;
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s is not initiated cannot take handle\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (table->maturity_level <= DBAL_MATURITY_PARTIALLY_FUNCTIONAL)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\nWARNING! maturity level of table %s is low - "
                                                "operations on this table are not validated (if table was validated update XML)\n"),
                                     table->table_name));
    }

    if (table_status == DBAL_TABLE_HW_ERROR)
    {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                "\nWARNING! DBAL table %s has HW elements are invalid operation on table will not be executed\n"),
                                     table->table_name));
    }

    if (operation == DBAL_HANDLE_TAKE_ALLOC)
    {
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_COLLECT_MODE))
        {
            
            if (DBAL_COLLECTION_IS_POOL_FULL(dbal_mngr[unit]->collection_counter))
            {
                int pool_id, old_pool_id;

                old_pool_id = DBAL_COLLECTION_POOL_ID_GET(dbal_mngr[unit]->collection_counter);

                SHR_IF_ERR_EXIT(dbal_collection_get_next_available_pool(unit, &pool_id));
                dbal_mngr[unit]->collection_counter =
                    pool_id * dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit);
                DBAL_COLLECTION_PRINTS((BSL_META
                                        ("POOL %d is full, starts collecting from next pool %d, handle_id = %d\n"),
                                        old_pool_id, pool_id, dbal_mngr[unit]->collection_counter));
            }
            else
            {
                
                dbal_mngr[unit]->collection_counter++;
            }

            handle_index = dbal_mngr[unit]->collection_counter + DBAL_SW_NOF_ENTRY_HANDLES;
            *entry_handle_id = handle_index;

            if (dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status == DBAL_HANDLE_STATUS_ACTION_COLLECTED)
            {
                
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "handle %d is not available (status ACTION_COLLECTED) error processing this entry \n",
                             *entry_handle_id);
            }
        }
        else
        {
            for (handle_index = 0; handle_index < DBAL_SW_NOF_ENTRY_HANDLES; handle_index++)
            {
                if (dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status == DBAL_HANDLE_STATUS_AVAILABLE)
                {
                    break;
                }
            }
            
            if (handle_index == DBAL_SW_NOF_ENTRY_HANDLES)
            {
                int ii;

                for (ii = 0; ii < DBAL_SW_NOF_ENTRY_HANDLES; ii++)
                {
                    LOG_CLI((BSL_META("handle %d %s\n"), ii,
                             dbal_mngr[unit]->entry_handles_pool[ii].table->table_name));
                }
                *entry_handle_id = -1;
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_entry_handle_take no handles available for table %s\n",
                             dbal_logical_table_to_string(unit, table_id));
            }
            else
            {
                *entry_handle_id = handle_index;
            }
        }
    }
    else
    {

        handle_index = *entry_handle_id;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_COLLECT_MODE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "table with collect mode enabled is not expected to call HANDLE_CLEAR table%s handle_index %d\n",
                         dbal_logical_table_to_string(unit, table_id), handle_index);
        }
        if (*entry_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal entry handle = %d table = %s\n", *entry_handle_id,
                         table->table_name);
        }
        else if (dbal_mngr[unit]->entry_handles_pool[*entry_handle_id].handle_status == DBAL_HANDLE_STATUS_AVAILABLE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "calling handle clear with unused handle %d table %s\n", *entry_handle_id,
                         table->table_name);
        }
        
        if (dbal_mngr[unit]->iterators_pool[*entry_handle_id].is_init)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_destroy_macro(unit, *entry_handle_id));
        }
        
        DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_FREE(dbal_mngr[unit]->entry_handles_pool[*entry_handle_id]);
    }

    entry_handles_pool_ptr = &(dbal_mngr[unit]->entry_handles_pool[*entry_handle_id]);
    
    sal_memset(entry_handles_pool_ptr, 0x0, sizeof(dbal_entry_handle_t));
    

    
    entry_handles_pool_ptr->handle_status = DBAL_HANDLE_STATUS_IN_USE;
    entry_handles_pool_ptr->handle_id = *entry_handle_id;
    entry_handles_pool_ptr->table_id = table_id;
    entry_handles_pool_ptr->table = table;

    
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        entry_handles_pool_ptr->cur_res_type = DBAL_RESULT_TYPE_NOT_INITIALIZED;
    }

    if (table->core_mode == DBAL_CORE_MODE_SBC)
    {
        entry_handles_pool_ptr->core_id = DBAL_CORE_ALL;
    }
    else
    {
        entry_handles_pool_ptr->core_id = DBAL_CORE_NOT_INTIATED;
    }

    SHR_IF_ERR_EXIT(dbal_handle_take_update_init_key_value(unit, handle_index));

    entry_handles_pool_ptr->er_flags = 0x0;

    if (dnx_dbal_journal_is_done_init(unit) && dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_ROLLBACK))
    {
        entry_handles_pool_ptr->er_flags |= DNX_DBAL_JOURNAL_ROLLBACK_FLAG;
    }
    if (dnx_dbal_journal_is_done_init(unit) && dnx_dbal_journal_is_on(unit, DNX_DBAL_JOURNAL_COMPARISON))
    {
        entry_handles_pool_ptr->er_flags |= DNX_DBAL_JOURNAL_COMPARE_FLAG;
    }

exit:
    SHR_IF_ERR_EXIT(sal_mutex_give(dbal_mngr[unit]->handle_mutex));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_copy_internal(
    int unit,
    uint32 src_handle_id,
    uint32 *dst_handle_id)
{
    int handle_index;
    dbal_entry_handle_t *src_handles_pool_ptr;
    dbal_entry_handle_t *dst_handles_pool_ptr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(sal_mutex_take(dbal_mngr[unit]->handle_mutex, sal_mutex_FOREVER));

    
    if (src_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal source handle = %d\n", src_handle_id);
    }
    else if (dbal_mngr[unit]->entry_handles_pool[src_handle_id].handle_status == DBAL_HANDLE_STATUS_AVAILABLE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "calling handle copy with unused source handle %d\n", src_handle_id);
    }

    
    for (handle_index = 0; handle_index < DBAL_SW_NOF_ENTRY_HANDLES; handle_index++)
    {
        if (dbal_mngr[unit]->entry_handles_pool[handle_index].handle_status == DBAL_HANDLE_STATUS_AVAILABLE)
        {
            break;
        }
    }
    
    if (handle_index == DBAL_SW_NOF_ENTRY_HANDLES)
    {
        int ii;

        for (ii = 0; ii < DBAL_SW_NOF_ENTRY_HANDLES; ii++)
        {
            LOG_CLI((BSL_META("handle %d %s\n"), ii, dbal_mngr[unit]->entry_handles_pool[ii].table->table_name));
        }
        *dst_handle_id = -1;
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_entry_handle_copy no handles available\n");
    }

    *dst_handle_id = handle_index;

    src_handles_pool_ptr = &(dbal_mngr[unit]->entry_handles_pool[src_handle_id]);
    dst_handles_pool_ptr = &(dbal_mngr[unit]->entry_handles_pool[*dst_handle_id]);
    
    sal_memcpy(dst_handles_pool_ptr, src_handles_pool_ptr, sizeof(dbal_entry_handle_t));
    
    DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_COPY((*src_handles_pool_ptr), (*dst_handles_pool_ptr));
    dst_handles_pool_ptr->handle_id = *dst_handle_id;

exit:
    SHR_IF_ERR_EXIT(sal_mutex_give(dbal_mngr[unit]->handle_mutex));
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_release_internal(
    int unit,
    uint32 entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES)
    {
        if (dnx_data_dbal.db_init.dbal_collection_pool_size_get(unit))
        {
            dbal_logical_table_t *table = dbal_mngr[unit]->entry_handles_pool[entry_handle_id].table;
            
            if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_COLLECT_MODE))
            {
                SHR_EXIT();
            }
        }
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal entry handle = %d \n", entry_handle_id);
    }

    SHR_IF_ERR_EXIT(sal_mutex_take(dbal_mngr[unit]->handle_mutex, sal_mutex_FOREVER));

    {
        dbal_logical_table_t *table = dbal_mngr[unit]->entry_handles_pool[entry_handle_id].table;
        
        if (dbal_mngr[unit]->iterators_pool[entry_handle_id].is_init)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_destroy_macro(unit, entry_handle_id));
        }
        
        DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_FREE(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED))
        {
            SHR_IF_ERR_EXIT(sal_mutex_give(dbal_mngr[unit]->protect_info[table->mutex_id].pre_table_mutex));
        }
        dbal_mngr[unit]->entry_handles_pool[entry_handle_id].handle_status = DBAL_HANDLE_STATUS_AVAILABLE;
    }

exit:
    if (entry_handle_id < DBAL_SW_NOF_ENTRY_HANDLES)
    {
        SHR_IF_ERR_EXIT(sal_mutex_give(dbal_mngr[unit]->handle_mutex));
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_get_internal(
    int unit,
    uint32 entry_handle_id,
    dbal_entry_handle_t ** entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle_id >= DBAL_SW_NOF_ENTRY_HANDLES + DBAL_SW_NOF_ENTRY_HANDLES_FOR_COLLECTION)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal entry handle = %d \n", entry_handle_id);
    }

    (*entry_handle) = &(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);

exit:
    SHR_FUNC_EXIT;
}

void
dbal_entry_handle_update_result(
    int unit,
    uint32 orig_entry_handle_id,
    uint32 update_entry_handle_id)
{
    dbal_entry_handle_t *orig_entry_handle;
    dbal_entry_handle_t *update_entry_handle;

    orig_entry_handle = &(dbal_mngr[unit]->entry_handles_pool[orig_entry_handle_id]);
    update_entry_handle = &(dbal_mngr[unit]->entry_handles_pool[update_entry_handle_id]);

    sal_memcpy(&update_entry_handle->phy_entry.payload, &orig_entry_handle->phy_entry.payload,
               DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
    sal_memcpy(&update_entry_handle->phy_entry.p_mask, &orig_entry_handle->phy_entry.p_mask,
               DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
    sal_memcpy(&update_entry_handle->value_field_ids, &orig_entry_handle->value_field_ids,
               DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE * sizeof(dbal_fields_e));

    update_entry_handle->cur_res_type = orig_entry_handle->cur_res_type;
    update_entry_handle->nof_result_fields = orig_entry_handle->nof_result_fields;
}




static shr_error_e
dbal_entry_key_field_core_id_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 field_val)
{
    dbal_table_field_info_t *table_field = NULL;
    int curr_field_index, is_field_updated = 0;
    dbal_logical_table_t *table = entry_handle->table;

    SHR_FUNC_INIT_VARS(unit);

    if (field_val == DBAL_CORE_ALL)
    {
        for (curr_field_index = 0; curr_field_index < table->nof_key_fields; curr_field_index++)
        {
            if (table->keys_info[curr_field_index].field_id == DBAL_FIELD_CORE_ID)
            {
                table_field = &(table->keys_info[curr_field_index]);
                if (entry_handle->key_field_ids[curr_field_index] != DBAL_FIELD_EMPTY)
                {
                    is_field_updated = 1;
                }
                break;
            }

        }
        if (table_field != NULL)
        {
            entry_handle->key_field_ids[curr_field_index] = DBAL_FIELD_CORE_ID;
            if (!is_field_updated)
            {
                entry_handle->nof_key_fields++;
            }
        }
        SHR_EXIT();
    }
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "value %d expected %d\n", field_val, DBAL_CORE_ALL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_entry_key_field_mask_set(
    int unit,
    dbal_fields_e field_id,
    dbal_table_field_info_t * table_field,
    dbal_entry_handle_t * entry_handle,
    int is_sub_field,
    dbal_fields_e parent_field_id,
    uint32 *field_mask)
{
    int offset = table_field->bits_offset_in_buffer;
    int nof_bits = table_field->field_nof_bits;
    dbal_value_field_encode_types_e encode_type = DBAL_VALUE_FIELD_ENCODE_NONE;
    uint32 field_max_size = 0;
    uint32 encoded_field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_max_size));

    if (is_sub_field)
    {
        dbal_sub_field_info_t sub_field_info;

        SHR_IF_ERR_EXIT(dbal_fields_sub_field_info_get_internal(unit, field_id, parent_field_id, &sub_field_info));
        encode_type = sub_field_info.encode_info.encode_mode;
    }

    
    if (encode_type == DBAL_VALUE_FIELD_ENCODE_PREFIX)
    {
        uint32 prefix_length;
        uint32 mask_value;
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &prefix_length));

        mask_value = (1 << (nof_bits - prefix_length)) - 1;

        SHR_IF_ERR_EXIT(dbal_fields_encode_prefix
                        (unit, field_mask, mask_value, prefix_length, nof_bits, encoded_field_mask));

        
        if (entry_handle->table_id == DBAL_TABLE_ECOL_IDENTIFICATION_CAM)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(encoded_field_mask, 0, nof_bits - 1));
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(encoded_field_mask, offset, nof_bits,
                                                       &(entry_handle->phy_entry.k_mask[0])));
    }
    else if (encode_type == DBAL_VALUE_FIELD_ENCODE_SUFFIX)
    {
        uint32 mask_value = (1 << (nof_bits - field_max_size)) - 1;

        SHR_IF_ERR_EXIT(dbal_fields_encode_suffix
                        (unit, field_mask, mask_value, field_max_size, nof_bits, encoded_field_mask));

        
        if (entry_handle->table_id == DBAL_TABLE_ECOL_IDENTIFICATION_CAM)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(encoded_field_mask, 0, nof_bits - 1));
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(encoded_field_mask, offset, nof_bits,
                                                       &(entry_handle->phy_entry.k_mask[0])));
    }
    else
    {
        
        if ((entry_handle->table_id == DBAL_TABLE_ECOL_IDENTIFICATION_CAM)
            || (entry_handle->table_id == DBAL_TABLE_ECOL_HEADER_SIZE_CAM))
        {
            uint32 field_mask_copy[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
            sal_memset(field_mask_copy, 0x0, sizeof(field_mask_copy));
            sal_memcpy(field_mask_copy, field_mask, WORDS2BYTES(BITS2WORDS(nof_bits)));
            SHR_IF_ERR_EXIT(utilex_bitstream_bit_flip_range(field_mask_copy, 0, nof_bits - 1));
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_mask_copy, offset, nof_bits,
                                                           &(entry_handle->phy_entry.k_mask[0])));
        }
        else
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                            (field_mask, offset, nof_bits, &(entry_handle->phy_entry.k_mask[0])));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void
dbal_entry_key_field_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type)
{
    dbal_entry_handle_t *entry_handle = &(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);
    int curr_field_index, is_field_updated = 0;
    dbal_logical_table_t *table = entry_handle->table;
    int is_sub_field = 0;
    uint32 copied_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 encoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 *curr_field_value_ptr;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    shr_error_e ret_val;
    dbal_table_field_info_t *table_field = NULL;

    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);

    DBAL_VALIDATE_HANDLE_STATUS_BRFORE_ACTION;

    if (!DBAL_HANDLE_IS_COLLECTION_MODE(entry_handle))
    {
        DBAL_FIELD_ERR_HANDLE(dbal_mngr[unit]->iterators_pool[entry_handle_id].is_init,
                              "Key fields cannot be changed on handle with valid iterator");
    }

    if (table->core_mode == DBAL_CORE_MODE_SBC && field_id == DBAL_FIELD_CORE_ID)
    {
        DBAL_FIELD_ERR_HANDLE(dbal_entry_key_field_core_id_set(unit, entry_handle, field_val[0]),
                              "error core_id field");
        DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);
        return;
    }

    for (curr_field_index = 0; curr_field_index < table->nof_key_fields; curr_field_index++)
    {
        uint8 is_sub_field_found = FALSE;

        if (table->keys_info[curr_field_index].field_id == field_id)
        {
            table_field = &(table->keys_info[curr_field_index]);
            if (entry_handle->key_field_ids[curr_field_index] != DBAL_FIELD_EMPTY)
            {
                is_field_updated = 1;
            }
            break;
        }

        if (SHR_BITGET(table->keys_info[curr_field_index].field_indication_bm, DBAL_FIELD_IND_IS_PARENT_FIELD))
        {
            DBAL_FIELD_ERR_HANDLE(dbal_fields_sub_field_match(unit, table->keys_info[curr_field_index].field_id,
                                                              field_id, &is_sub_field_found), "error sub field");
            if (is_sub_field_found)
            {
                is_sub_field = 1;
                parent_field_id = table->keys_info[curr_field_index].field_id;
                table_field = &(table->keys_info[curr_field_index]);
                if (entry_handle->key_field_ids[curr_field_index] != DBAL_FIELD_EMPTY)
                {
                    is_field_updated = 1;
                }
                break;
            }
        }
    }

    DBAL_FIELD_ERR_HANDLE((table_field == NULL), "table field is NUL, KEY field not found");

    
    DBAL_FIELD_ERR_HANDLE(((dbal_field_legal_sizes[pointer_type]) &&
                           (table_field->field_nof_bits > dbal_field_legal_sizes[pointer_type])),
                          "field size too big for pointer type");

        
    if (field_id == DBAL_FIELD_CORE_ID)
    {
        if ((field_val[0] == DBAL_CORE_ALL) || (field_val[0] < DBAL_MAX_NUM_OF_CORES))
        {
            entry_handle->core_id = field_val[0];
            entry_handle->key_field_ids[curr_field_index] = field_id;
        }
        else
        {
            DBAL_FIELD_ERR_HANDLE(TRUE, "Invalid value set for key field CORE_ID");
        }

        if (!is_field_updated)
        {
            entry_handle->nof_key_fields++;
        }
        DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);
        return;
    }

    
    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS)
        && DBAL_RUNTIME_VERIFICATIONS_ENABLED(unit) && (table_field->field_nof_bits <= 32)
        && !SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ADDRESS))
    {
        uint8 is_illegal = FALSE;
        uint32 max_value = table_field->max_value;

        if (is_sub_field)
        {
            DBAL_FIELD_ERR_HANDLE(dbal_fields_max_value_get(unit, field_id, &max_value),
                                  "Cannot get the field max value");
        }

        
        if (field_val[0] > max_value)
        {
            DBAL_FIELD_ERR_HANDLE_NO_RETURN(_SHR_E_INTERNAL, "Illegal value for field, too big");
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "field value = %d max value %d\n"), field_val[0], max_value));
            DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);
            return;
        }

        
        if (field_val[0] < table_field->min_value)
        {
            DBAL_FIELD_ERR_HANDLE_NO_RETURN(_SHR_E_INTERNAL, "Illegal value for field, too low");
            LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "field value = %d min value %d\n"), field_val[0],
                                       table_field->min_value));
            DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);
            return;
        }

        
        if (!(SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
        {
            ret_val = dbal_fields_is_illegal_value(unit, field_id, field_val[0], &is_illegal);
            DBAL_FIELD_ERR_HANDLE((is_illegal || ret_val), "Illegal value for field, in illegals list");
        }
    }

    if (!(SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
    {
        
        ret_val = utilex_bitstream_get_any_field(field_val, table_field->offset_in_logical_field,
                                                 table_field->field_nof_bits, copied_field_value);

        DBAL_FIELD_ERR_HANDLE(ret_val, "Copy from offset error for field");
    }
    else
    {
        
        copied_field_value[0] = field_val[0];
    }
    curr_field_value_ptr = copied_field_value;

    if ((SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED)) || is_sub_field)
    {
        ret_val = dbal_fields_field_encode(unit, field_id, parent_field_id, table_field->field_nof_bits,
                                           copied_field_value, encoded_field_value);
        if (ret_val != _SHR_E_NONE)
        {
            ret_val = dbal_field_encode_handle_rv(unit, entry_handle, 1, field_id, ret_val);
            if (!dbal_image_name_is_std_1(unit) && dbal_mngr[unit]->status != DBAL_STATUS_DEVICE_INIT_DONE)
            {
                
                entry_handle->error_info.field_id = field_id;
                entry_handle->error_info.error_exists = _SHR_E_INTERNAL;
            }
            else
            {
                DBAL_FIELD_ERR_HANDLE(ret_val, "encoding error for field");
            }
        }
        curr_field_value_ptr = encoded_field_value;
    }

        
    ret_val = utilex_bitstream_set_any_field(curr_field_value_ptr, table_field->bits_offset_in_buffer,
                                             table_field->field_nof_bits, &entry_handle->phy_entry.key[0]);

    DBAL_FIELD_ERR_HANDLE(ret_val, "utilex_bitstream_set_any_field from offset error for field");

    if (field_mask && DBAL_TABLE_IS_KEY_MASK_REQUIRED(table))
    {
        ret_val =
            dbal_entry_key_field_mask_set(unit, field_id, table_field, entry_handle, is_sub_field, parent_field_id,
                                          field_mask);
        DBAL_FIELD_ERR_HANDLE(ret_val, "dbal_entry_key_field_mask_set issue");
    }

    entry_handle->key_field_ids[curr_field_index] = field_id;

    if (!is_field_updated)
    {
        entry_handle->phy_entry.key_size += table_field->field_nof_bits;
        entry_handle->nof_key_fields++;
    }
    else
    {
        if (entry_handle->error_info.error_exists == _SHR_E_UNAVAIL)
        {
            if ((entry_handle->error_info.field_id == field_id) && !dbal_image_name_is_std_1(unit))
            {
                
                entry_handle->error_info.error_exists = _SHR_E_NONE;
            }
        }
    }

    if (entry_handle->key_field_ranges[curr_field_index] != 0)
    {
        
        entry_handle->key_field_ranges[curr_field_index] = 0;
        entry_handle->nof_ranged_fields--;
    }

    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);
}

void
dbal_entry_key_field_range_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint32 min_val,
    uint32 max_val)
{
    dbal_entry_handle_t *entry_handle = &(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);
    uint32 field_val_u32[1];
    uint32 *field_mask_u32;
    dbal_table_field_info_t *field_info = NULL;
    int ii, is_core_field = 0, key_field_pos = 0;

    field_val_u32[0] = min_val;
    field_mask_u32 = G_dbal_field_full_mask;

    DBAL_VALIDATE_HANDLE_STATUS_BRFORE_ACTION;

    DBAL_FIELD_ERR_HANDLE(((entry_handle->table->access_method != DBAL_ACCESS_METHOD_KBP) &&
                           (DBAL_TABLE_IS_TCAM(entry_handle->table))),
                          "Range actions for TCAM are supported only for KBP");

    DBAL_FIELD_ERR_HANDLE(((min_val != DBAL_RANGE_ALL) && (min_val > max_val)), "Min range is bigger than max range");

    for (ii = 0; ii < entry_handle->table->nof_key_fields; ii++)
    {
        if (field_id == entry_handle->table->keys_info[ii].field_id)
        {
            key_field_pos = ii;
            field_info = &(entry_handle->table->keys_info[ii]);
            break;
        }
    }
    DBAL_FIELD_ERR_HANDLE((ii == entry_handle->table->nof_key_fields), "field not found for range");
    DBAL_FIELD_ERR_HANDLE(((entry_handle->table->access_method == DBAL_ACCESS_METHOD_KBP) &&
                           (!SHR_BITGET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_RANGED))),
                          "The field is not ranged");

    if (min_val == DBAL_RANGE_ALL)
    {
        min_val = field_info->min_value;
        field_val_u32[0] = min_val;
        DBAL_FIELD_ERR_HANDLE(((entry_handle->table->access_method == DBAL_ACCESS_METHOD_KBP) &&
                               (min_val >= (1 << DNX_KBP_MAX_RANGE_SIZE_IN_BITS))),
                              "Min value for the field is bigger than the max supported value for KBP ranges");
    }

    if (!dbal_mngr[unit]->is_std_1)
    {
        
        int jj;
        dbal_field_types_basic_info_t *field_type_info;

        DBAL_FIELD_ERR_HANDLE(dbal_fields_field_types_info_get(unit, field_id, &field_type_info),
                              "field type not found");

        if (field_type_info->encode_info.encode_mode == DBAL_VALUE_FIELD_ENCODE_ENUM)
        {
            if ((min_val < field_type_info->nof_enum_values) && field_type_info->enum_val_info[min_val].is_invalid)
            {
                
                for (jj = min_val; jj < field_type_info->nof_enum_values; jj++)
                {
                    if (field_type_info->enum_val_info[jj].is_invalid != TRUE)
                    {
                        min_val = jj;
                        field_val_u32[0] = min_val;
                        break;
                    }
                }
                entry_handle->error_info.error_exists = _SHR_E_UNAVAIL;
                return;
            }
        }
    }

    if ((field_id == DBAL_FIELD_CORE_ID) && (max_val == DBAL_RANGE_ALL))
    {
        field_val_u32[0] = DBAL_CORE_ALL;
        is_core_field = 1;
    }
    
    dbal_entry_key_field_set(unit, entry_handle_id, field_id, field_val_u32, field_mask_u32, DBAL_POINTER_TYPE_UINT32);

    if (!is_core_field)
    {
        if (!entry_handle->error_info.error_exists)
        {
            if (max_val == DBAL_RANGE_ALL)
            {
                max_val = field_info->max_value;
                DBAL_FIELD_ERR_HANDLE(((entry_handle->table->access_method == DBAL_ACCESS_METHOD_KBP) &&
                                       (max_val >= (1 << DNX_KBP_MAX_RANGE_SIZE_IN_BITS))),
                                      "Max value for the field is bigger than the max supported value for KBP ranges");
            }

            
            entry_handle->key_field_ranges[key_field_pos] = max_val - min_val + 1;
            entry_handle->nof_ranged_fields++;
        }
    }
}


void
dbal_entry_mul_result_handle(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    int res_type)
{
    uint32 result_type_hw_value[1] = { 0 }, field_index_in_table = 0;
    int iter, ret_val = 0;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t *table_field;
    dbal_iterator_info_t *iterator_info = NULL;

    DBAL_FIELD_ERR_HANDLE((field_id != DBAL_FIELD_RESULT_TYPE), "for table with multiple result types need"
                          " to configure entry type before setting any value field");

    if (!DBAL_HANDLE_IS_COLLECTION_MODE(entry_handle))
    {
        DBAL_FIELD_ERR_HANDLE(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info), "iter get error");
        DBAL_FIELD_ERR_HANDLE(iterator_info->is_init,
                              "RESULT_TYPE field cannot be changed on handle with valid iterator");
    }

    DBAL_FIELD_ERR_HANDLE(((res_type > table->nof_result_types)
                           && (res_type != DBAL_SUPERSET_RESULT_TYPE)), "illegal result type value");

    entry_handle->cur_res_type = res_type;

    if (res_type != DBAL_SUPERSET_RESULT_TYPE)
    {
        DBAL_FIELD_ERR_HANDLE(((table->multi_res_info[res_type].is_disabled)), "result type disabled cannot be set");

        result_type_hw_value[0] = table->multi_res_info[res_type].result_type_hw_value[0];

        for (iter = 0; iter < table->multi_res_info[res_type].nof_result_fields; iter++)
        {
            if (table->multi_res_info[res_type].results_info[iter].field_id == DBAL_FIELD_RESULT_TYPE)
            {
                field_index_in_table = iter;
                break;
            }
        }

        table_field = &(table->multi_res_info[res_type].results_info[field_index_in_table]);

        entry_handle->value_field_ids[field_index_in_table] = field_id;
        entry_handle->nof_result_fields++;

        ret_val = utilex_bitstream_set_any_field(result_type_hw_value, table_field->bits_offset_in_buffer,
                                                 table_field->field_nof_bits, &(entry_handle->phy_entry.payload[0]));

        DBAL_FIELD_ERR_HANDLE(ret_val, "mul res utilex_bitstream_set_any_field from offset error for field");

        ret_val = utilex_bitstream_set_any_field(G_dbal_field_full_mask, table_field->bits_offset_in_buffer,
                                                 table_field->field_nof_bits, &(entry_handle->phy_entry.p_mask[0]));

        DBAL_FIELD_ERR_HANDLE(ret_val, "mul res utilex_bitstream_set_any_field from offset error for field");
        dbal_handle_take_update_init_result_value(unit, entry_handle);
    }
    else
    {
        if (entry_handle->table_id == DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
        {
            DBAL_FIELD_ERR_HANDLE(ret_val, "superset res type is not supported for table ETM_PP_DESCRIPTOR_EXPANSION");
        }
            
        entry_handle->cur_res_type = entry_handle->table->nof_result_types;
    }
}


static shr_error_e
dbal_entry_field_instances_set(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_table_field_info_t * table_field,
    int field_index,
    uint32 *field_val)
{
    int offset_in_buffer = table_field->bits_offset_in_buffer;
    int nof_instances = table_field->nof_instances;
    int inst_iter;
    dbal_fields_e field_id = table_field->field_id;

    SHR_FUNC_INIT_VARS(unit);

    for (inst_iter = 1; inst_iter < nof_instances; inst_iter++)
    {
        offset_in_buffer += table_field->field_nof_bits;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val, offset_in_buffer,
                                                       table_field->field_nof_bits,
                                                       &(entry_handle->phy_entry.payload[0])));

        
        if (entry_handle->value_field_ids[field_index + inst_iter] == DBAL_FIELD_EMPTY)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(G_dbal_field_full_mask, offset_in_buffer,
                                                           table_field->field_nof_bits,
                                                           &(entry_handle->phy_entry.p_mask[0])));

            entry_handle->value_field_ids[field_index + inst_iter] = field_id;
            entry_handle->nof_result_fields++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_entry_field_instances_unset(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_table_field_info_t * table_field,
    int field_index)
{
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    int offset_in_buffer = table_field->bits_offset_in_buffer;
    int nof_instances = table_field->nof_instances;
    int inst_iter;

    SHR_FUNC_INIT_VARS(unit);

    for (inst_iter = 1; inst_iter < nof_instances; inst_iter++)
    {
        offset_in_buffer += table_field->field_nof_bits;

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_val, offset_in_buffer,
                                                       table_field->field_nof_bits,
                                                       &(entry_handle->phy_entry.payload[0])));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(G_dbal_field_full_mask, offset_in_buffer,
                                                       table_field->field_nof_bits,
                                                       &(entry_handle->phy_entry.p_mask[0])));

        entry_handle->value_field_ids[field_index + inst_iter] = DBAL_FIELD_EMPTY;
        entry_handle->nof_result_fields--;

    }

exit:
    SHR_FUNC_EXIT;
}

void
dbal_entry_value_field_set(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id,
    dbal_pointer_type_e pointer_type,
    uint32 *field_val,
    uint32 *field_mask)
{
    dbal_entry_handle_t *entry_handle = &(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);
    int is_field_updated = 0, field_index_in_table = 0;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    uint32 *curr_field_value_ptr;
    uint32 copied_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 encoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_value_after_valid_ind[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    int is_sub_field = 0;
    uint32 field_max_size = 0;
    shr_error_e ret_val;
    int instance_id = in_inst_id;
    dbal_table_field_info_t *table_field = NULL;

    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);

    DBAL_VALIDATE_HANDLE_STATUS_BRFORE_ACTION;

    if ((in_inst_id == INST_SINGLE) || (in_inst_id == INST_ALL))
    {
        instance_id = 0;
    }

    
    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        if ((entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED))
        {
            DBAL_FIELD_ERR_HANDLE((instance_id != 0), "result type cannot be set with not default instance idx");
            dbal_entry_mul_result_handle(unit, entry_handle, field_id, (int) field_val[0]);
            return;
        }
        else
        {
            
            DBAL_FIELD_ERR_HANDLE((field_id == DBAL_FIELD_RESULT_TYPE),
                                  "for table with multiple result types cannot re-set the result type");
        }
    }

    
    DBAL_FIELD_ERR_HANDLE(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, field_id, 0,
                                                              entry_handle->cur_res_type, instance_id,
                                                              &table_field, &field_index_in_table, &parent_field_id),
                          "VALUE field not found in table");

    DBAL_FIELD_ERR_HANDLE((table_field->permission == DBAL_PERMISSION_READONLY), "Cannot set value to read only field");

    
    if (!SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
    {
        
        DBAL_FIELD_ERR_HANDLE(((dbal_field_legal_sizes[pointer_type]) &&
                               (table_field->field_nof_bits > dbal_field_legal_sizes[pointer_type])),
                              "field size too big for pointer type");
    }

    if (entry_handle->value_field_ids[field_index_in_table] != DBAL_FIELD_EMPTY)
    {
        is_field_updated = 1;
    }

    if (parent_field_id != DBAL_FIELD_EMPTY)
    {
        is_sub_field = 1;
    }
    else
    {
        field_id += instance_id;
    }

    DBAL_FIELD_ERR_HANDLE(dbal_fields_max_size_get(unit, field_id, &field_max_size), "Cannot get the field max size");

    
    if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS)
        && DBAL_RUNTIME_VERIFICATIONS_ENABLED(unit) && (table_field->field_nof_bits <= 32)
        && !SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ADDRESS))
    {
        uint8 is_illegal = FALSE;
        uint32 max_value = table_field->max_value;

        if (is_sub_field)
        {
            DBAL_FIELD_ERR_HANDLE(dbal_fields_max_value_get(unit, field_id, &max_value),
                                  "Cannot get the field max value");
        }

        if (table_field->arr_prefix_size != 0
            && !(SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
        {
            
            ret_val = dbal_field_arr_prefix_decode(unit, table_field, field_val[0], field_val, TRUE);
            DBAL_FIELD_ERR_HANDLE(ret_val, "dbal_field_arr_prefix_encode");
        }
        else
        {
            
            if (field_val[0] > max_value)
            {
                DBAL_FIELD_ERR_HANDLE_NO_RETURN(_SHR_E_INTERNAL, "Illegal value for field, too big");
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "field value = %d max value %d\n"), field_val[0], max_value));
                return;
            }

            
            if (field_val[0] < table_field->min_value)
            {
                DBAL_FIELD_ERR_HANDLE_NO_RETURN(_SHR_E_INTERNAL, "Illegal value for field, too low");
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "field value = %d min value %d\n"), field_val[0],
                                           table_field->min_value));
                return;
            }
        }

        
        if (!(SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
        {
            ret_val = dbal_fields_is_illegal_value(unit, field_id, field_val[0], &is_illegal);
            DBAL_FIELD_ERR_HANDLE((is_illegal || ret_val), "Illegal value for field, in illegals list");
        }
    }

    if (!(SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
    {
        int size_to_copy = table_field->field_nof_bits;
        if (SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
        {
            
            size_to_copy = size_to_copy - 1;
        }
        
        ret_val = utilex_bitstream_get_any_field(field_val, table_field->offset_in_logical_field, size_to_copy,
                                                 copied_field_value);

        DBAL_FIELD_ERR_HANDLE(ret_val, "Copy from offset error for field");
        curr_field_value_ptr = copied_field_value;
    }
    else
    {
        
        curr_field_value_ptr = field_val;
    }

    if ((SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED)) || is_sub_field)
    {
        ret_val = dbal_fields_field_encode(unit, field_id, parent_field_id, table_field->field_nof_bits,
                                           curr_field_value_ptr, encoded_field_value);
        if (ret_val != _SHR_E_NONE)
        {
            ret_val = dbal_field_encode_handle_rv(unit, entry_handle, 0, field_id, ret_val);
            DBAL_FIELD_ERR_HANDLE(ret_val, "encoding error for field");
            return;
        }

        curr_field_value_ptr = encoded_field_value;

        if (SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM))
        {
            uint32 enum_max_value = (1 << table_field->field_nof_bits) - 1;
            if (table_field->arr_prefix_size != 0)
            {
                ret_val =
                    dbal_field_arr_prefix_decode(unit, table_field, curr_field_value_ptr[0], curr_field_value_ptr,
                                                 TRUE);
                DBAL_FIELD_ERR_HANDLE(ret_val, "dbal_field_arr_prefix_decode");
            }
            if ((enum_max_value > 0) && (curr_field_value_ptr[0] > enum_max_value))
            {
                LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "enum field value = %d max value %d\n"),
                                           curr_field_value_ptr[0], enum_max_value));
                DBAL_FIELD_ERR_HANDLE(TRUE, "enum max value error");
            }
        }
    }

    if (SHR_BITGET(table_field->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
    {
        ret_val = dbal_fields_encode_valid_ind(unit, curr_field_value_ptr, table_field->field_nof_bits,
                                               field_value_after_valid_ind);
        DBAL_FIELD_ERR_HANDLE(ret_val, "encoding error for field");
        curr_field_value_ptr = field_value_after_valid_ind;
    }

    
    ret_val = utilex_bitstream_set_any_field(curr_field_value_ptr, table_field->bits_offset_in_buffer,
                                             table_field->field_nof_bits, &(entry_handle->phy_entry.payload[0]));

    DBAL_FIELD_ERR_HANDLE(ret_val, "bitstream issue");
    entry_handle->value_field_ids[field_index_in_table] = field_id;

    
    ret_val =
        utilex_bitstream_set_any_field(field_mask, table_field->bits_offset_in_buffer, table_field->field_nof_bits,
                                       &(entry_handle->phy_entry.p_mask[0]));

    DBAL_FIELD_ERR_HANDLE(ret_val, "bitstream issue mask");

    
    if (!is_field_updated)
    {
        entry_handle->phy_entry.payload_size += table_field->field_nof_bits;
        entry_handle->nof_result_fields++;
    }

    
    if (in_inst_id == INST_ALL)
    {
        dbal_entry_field_instances_set(unit, entry_handle, table_field, field_index_in_table, curr_field_value_ptr);
    }

    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_SET);
}

void
dbal_entry_value_field_unset_field(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id)
{
    dbal_entry_handle_t *entry_handle = &(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);
    int field_index_in_table = 0, ret_val = 0;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;

    uint32 unset_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    int instance_id = in_inst_id;
    dbal_table_field_info_t *table_field = NULL;

    DBAL_VALIDATE_HANDLE_STATUS_BRFORE_ACTION;

    if ((in_inst_id == INST_SINGLE) || (in_inst_id == INST_ALL))
    {
        instance_id = 0;
    }

    DBAL_FIELD_ERR_HANDLE(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, field_id, 0,
                                                              entry_handle->cur_res_type, instance_id,
                                                              &table_field, &field_index_in_table, &parent_field_id),
                          "VALUE field not found in table");

    DBAL_FIELD_ERR_HANDLE((table_field->permission == DBAL_PERMISSION_READONLY),
                          "Cannot unset value to read only field");

    DBAL_FIELD_ERR_HANDLE((entry_handle->value_field_ids[field_index_in_table] == DBAL_FIELD_EMPTY),
                          "Cannot unset an unset field");

    field_id += instance_id;

    
    ret_val = utilex_bitstream_set_any_field(unset_field_value, table_field->bits_offset_in_buffer,
                                             table_field->field_nof_bits, &(entry_handle->phy_entry.payload[0]));

    DBAL_FIELD_ERR_HANDLE(ret_val, "bitstream issue");

    entry_handle->value_field_ids[field_index_in_table] = DBAL_FIELD_EMPTY;

    
    ret_val =
        utilex_bitstream_set_any_field(unset_field_value, table_field->bits_offset_in_buffer,
                                       table_field->field_nof_bits, &(entry_handle->phy_entry.p_mask[0]));

    DBAL_FIELD_ERR_HANDLE(ret_val, "bitstream issue mask");

    entry_handle->phy_entry.payload_size -= table_field->field_nof_bits;
    entry_handle->nof_result_fields--;

    
    if (in_inst_id == INST_ALL)
    {
        dbal_entry_field_instances_unset(unit, entry_handle, table_field, field_index_in_table);
    }
}

void
dbal_entry_value_field_request(
    int unit,
    dbal_pointer_type_e pointer_type,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id,
    void *field_val,
    void *field_mask)
{
    dbal_entry_handle_t *entry_handle = &(dbal_mngr[unit]->entry_handles_pool[entry_handle_id]);
    dbal_logical_table_t *table = entry_handle->table;
    int field_index_in_table = 0, is_field_updated = 0;
    int instance_id = in_inst_id;
    dbal_table_field_info_t *table_field;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;

    DBAL_VALIDATE_HANDLE_STATUS_BRFORE_ACTION;

    DBAL_FIELD_ERR_HANDLE((entry_handle->handle_status == DBAL_HANDLE_STATUS_ACTION_PREFORMED),
                          "Get all fields was performed, should use dbal_entry_handle_value_field_get\n");

    DBAL_VALIDATE_VALID_INSTANCE_ID;

    if (in_inst_id == INST_SINGLE)
    {
        instance_id = 0;
    }

    DBAL_FIELD_ERR_HANDLE(((SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
                           && (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)),
                          "cannot request field to multiple result types table, use handle_value_field or set the result type");

    DBAL_FIELD_ERR_HANDLE(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, field_id, 0,
                                                              entry_handle->cur_res_type, instance_id,
                                                              &table_field, &field_index_in_table, &parent_field_id),
                          "VALUE field not found in table");

    DBAL_FIELD_ERR_HANDLE((table_field->permission == DBAL_PERMISSION_WRITEONLY),
                          "Cannot get value from write only field");

    if (entry_handle->value_field_ids[field_index_in_table] != DBAL_FIELD_EMPTY)
    {
        is_field_updated = 1;
    }

    
    DBAL_FIELD_ERR_HANDLE(((dbal_field_legal_sizes[pointer_type]) &&
                           (table_field->field_nof_bits > dbal_field_legal_sizes[pointer_type])),
                          "field size too big for pointer type");

    
    if (field_id + in_inst_id == table_field->field_id)
    {
        entry_handle->value_field_ids[field_index_in_table] = field_id + in_inst_id;
    }
    else
    {
        entry_handle->value_field_ids[field_index_in_table] = field_id;
    }

    
    DBAL_ENTRY_HANDLE_USER_OUTPUT_INFO_ALLOC(*entry_handle);
    entry_handle->user_output_info[field_index_in_table].value_returned_pointer = field_val;
    entry_handle->user_output_info[field_index_in_table].pointer_type = pointer_type;

    if (field_mask)
    {
        entry_handle->user_output_info[field_index_in_table].mask_returned_pointer = field_mask;
    }

    if (!is_field_updated)
    {
        entry_handle->nof_result_fields++;
    }
}

static shr_error_e
dbal_entry_key_field_core_id_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 *field_val)
{

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->core_id != DBAL_CORE_NOT_INTIATED)
    {
        (*field_val) = entry_handle->core_id;
    }
    else
    {
        
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Requested field core ID , but core is not been set\n");
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_hw_field_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    uint32 *buffer,
    uint32 *returned_field_value)
{
    int field_offset_in_buffer = table_field_info->bits_offset_in_buffer;
    int field_nof_bits = table_field_info->field_nof_bits;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (buffer, field_offset_in_buffer, field_nof_bits, returned_field_value));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_enum_hw_value_or_key_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type)
{

    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id, field_id, 1, entry_handle->cur_res_type,
                                               0, &table_field_info));
    DBAL_VALIDATE_POINTER_SIZE(pointer_type, table_field_info.field_nof_bits);

    
    if (SHR_BITGET(table_field_info.field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM))
    {
        SHR_IF_ERR_EXIT(dbal_hw_field_from_buffer_get(unit, &table_field_info, entry_handle->phy_entry.key, field_val));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_key_field_get(unit, entry_handle, field_id, field_val, field_mask, pointer_type));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_key_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type)
{
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, entry_handle->table_id, field_id, 1, entry_handle->cur_res_type,
                                               0, &table_field_info));

    DBAL_VALIDATE_POINTER_SIZE(pointer_type, table_field_info.field_nof_bits);

    if (field_id == DBAL_FIELD_CORE_ID)
    {
        SHR_IF_ERR_EXIT(dbal_entry_key_field_core_id_get(unit, entry_handle, field_val));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, &table_field_info, field_id, entry_handle->phy_entry.key,
                                                   field_val));
    }

    if (field_mask && DBAL_TABLE_IS_KEY_MASK_REQUIRED(entry_handle->table))
    {
        SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get(unit, &table_field_info, field_id,
                                                        entry_handle->phy_entry.k_mask, field_mask));
    }

exit:
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET);
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_entry_handle_is_field_set(
    int unit,
    int entry_handle_id,
    dbal_fields_e field_id,
    int in_inst_id)
{
    dbal_table_field_info_t *table_field_info;
    int instance_id = in_inst_id, field_index_in_table;
    shr_error_e rv;
    dbal_fields_e parent_field_id;
    dbal_entry_handle_t *entry_handle = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (in_inst_id == INST_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal Usage of instance ALL. INST_ALL cannot be used in get APIs\n");
    }

    if (in_inst_id == INST_SINGLE)
    {
        instance_id = 0;
    }

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "cannot ask if field RESULT_TYPE was set.\n");
    }
    else
    {
        rv = dbal_tables_field_info_get_internal
            (unit, entry_handle->table_id, field_id, 0, entry_handle->cur_res_type, instance_id,
             (&table_field_info), &field_index_in_table, &parent_field_id);

        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            
            SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_PARAM);
        }
    }

    
    if (table_field_info->permission == DBAL_PERMISSION_WRITEONLY)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot get value from write only field %s\n",
                     dbal_field_to_string(unit, field_id));
    }

    
    if (entry_handle->value_field_ids[field_index_in_table] == DBAL_FIELD_EMPTY)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_value_field_get(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    int in_inst_id,
    uint32 *field_val,
    uint32 *field_mask,
    dbal_pointer_type_e pointer_type)
{
    dbal_table_field_info_t *table_field_info;
    int instance_id = in_inst_id, field_index_in_table;
    shr_error_e rv;
    dbal_fields_e parent_field_id;
    dbal_actions_e action = DBAL_NOF_ACTIONS;

    SHR_FUNC_INIT_VARS(unit);
    DNX_INIT_TIME_ANALYZER_DBAL_START(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET);

    SHR_IF_ERR_EXIT(dbal_action_skip_check(unit, entry_handle, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        SHR_EXIT();
    }

    if ((entry_handle->get_all_fields == 0) && (entry_handle->cur_res_type != entry_handle->table->nof_result_types))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "no available fields, need to perform get to recieve fields res type %s\n",
                     entry_handle->table->multi_res_info[entry_handle->cur_res_type].result_type_name);
    }

    if ((entry_handle->handle_status != DBAL_HANDLE_STATUS_ACTION_PREFORMED)
        && (entry_handle->cur_res_type != entry_handle->table->nof_result_types))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_entry_get() not performed on the entry - buffer empty\n");
    }

    if (in_inst_id == INST_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal Usage of instance ALL. INST_ALL cannot be used in get APIs\n");
    }

    if (in_inst_id == INST_SINGLE)
    {
        instance_id = 0;
    }

    
    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        (*field_val) = entry_handle->cur_res_type;
    }
    else
    {
        dbal_fields_e field_to_get;
        rv = dbal_tables_field_info_get_internal
            (unit, entry_handle->table_id, field_id, 0, entry_handle->cur_res_type, instance_id,
             (&table_field_info), &field_index_in_table, &parent_field_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT_NO_MSG(rv);

        DBAL_VALIDATE_POINTER_SIZE(pointer_type, table_field_info->field_nof_bits);

        
        if (field_id + instance_id == table_field_info->field_id)
        {
            field_to_get = field_id + instance_id;
        }
        else
        {
            field_to_get = field_id;
        }

        
        if (table_field_info->permission == DBAL_PERMISSION_WRITEONLY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot get value from write only field %s\n",
                         dbal_field_to_string(unit, field_id));
        }

        
        if ((entry_handle->cur_res_type == entry_handle->table->nof_result_types)
            && (entry_handle->value_field_ids[field_index_in_table] == DBAL_FIELD_EMPTY))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }

        rv = dbal_field_from_buffer_get(unit, table_field_info, field_to_get, entry_handle->phy_entry.payload,
                                        field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        SHR_IF_ERR_EXIT_NO_MSG(rv);

        if (field_mask)
        {
            SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get(unit, table_field_info, field_to_get,
                                                            entry_handle->phy_entry.p_mask, field_mask));
        }

        if ((table_field_info->arr_prefix_size)
            && !(SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
        {
            
            int total_size = table_field_info->arr_prefix_size + table_field_info->field_nof_bits;
            uint32 tmp_field_value = field_val[0];

            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field_info->arr_prefix,
                                                      table_field_info->field_nof_bits, total_size, field_val));
        }

    }

exit:
    DNX_INIT_TIME_ANALYZER_DBAL_STOP(unit, DNX_INIT_TIME_ANALYZER_DBAL_FIELD_GET);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_default_values_add(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int field_pos;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->nof_result_fields != DBAL_RES_INFO.nof_result_fields)
    {
        
        for (field_pos = 0; field_pos < DBAL_RES_INFO.nof_result_fields; field_pos++)
        {
            if (entry_handle->value_field_ids[field_pos] == DBAL_FIELD_EMPTY)
            {
                uint32 default_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                dbal_fields_e field_id = DBAL_RES_INFO.results_info[field_pos].field_id;

                if (entry_handle->table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC &&
                    (entry_handle->table->hl_mapping_multi_res[entry_handle->cur_res_type].is_default_non_standard))
                {
                    
                    
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "HL TCAM none standard default is not supported for table %s\n",
                                 entry_handle->table->table_name);
                }
                
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(default_val,
                                                               DBAL_RES_INFO.
                                                               results_info[field_pos].bits_offset_in_buffer,
                                                               DBAL_RES_INFO.results_info[field_pos].field_nof_bits,
                                                               &(entry_handle->phy_entry.payload[0])));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(G_dbal_field_full_mask,
                                                               DBAL_RES_INFO.
                                                               results_info[field_pos].bits_offset_in_buffer,
                                                               DBAL_RES_INFO.results_info[field_pos].field_nof_bits,
                                                               &(entry_handle->phy_entry.p_mask[0])));

                entry_handle->value_field_ids[field_pos] = field_id;
                entry_handle->nof_result_fields++;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_print(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 format)
{
    int iter;
    dbal_logical_table_t *table = entry_handle->table;
    uint32 *mask = NULL;
    int nof_fields_printed = 0;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE] = { 0 };
    int log_enabled;

    SHR_FUNC_INIT_VARS(unit);

    if (DBAL_TABLE_IS_KEY_MASK_REQUIRED(table))
    {
        if ((table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX] == DBAL_PHYSICAL_TABLE_TCAM) ||
            (table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX] == DBAL_PHYSICAL_TABLE_KAPS_1) ||
            (table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX] == DBAL_PHYSICAL_TABLE_KAPS_2))
        {
            mask = entry_handle->phy_entry.k_mask;
        }
    }

    log_enabled = ((bsl_check(bslLayerSocdnx, bslSourceDbaldnx, bslSeverityInfo, unit)) || format);

    if ((log_enabled) || (dbal_mngr[unit]->logger_info.dbal_file != NULL))
    {
        int nof_key_fields = (DBAL_TABLE_IS_TCAM(table)) ? table->nof_key_fields : entry_handle->nof_key_fields;
        DBAL_DUMP("KEY: ", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
        for (iter = 0; iter < table->nof_key_fields; iter++)
        {
            dbal_fields_e key_field = entry_handle->key_field_ids[iter];
            if (key_field == DBAL_FIELD_EMPTY)
            {
                if (DBAL_TABLE_IS_TCAM(table))
                {
                    key_field = table->keys_info[iter].field_id;
                }
                else
                {
                    continue;
                }
            }

            DBAL_DUMP(dbal_field_to_string(unit, key_field), log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
            DBAL_DUMP(" ", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
            if (key_field == DBAL_FIELD_CORE_ID)
            {
                
                if (entry_handle->core_id == DBAL_CORE_ALL)
                {
                    sal_snprintf(buffer, 5, "ALL");
                }
                else
                {
                    sal_snprintf(buffer, DBAL_MAX_STRING_LENGTH, " %d ", entry_handle->core_id);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, key_field,
                                                                         entry_handle->table_id,
                                                                         entry_handle->phy_entry.key, mask, 0, TRUE,
                                                                         TRUE, buffer));
            }
            DBAL_DUMP(buffer, log_enabled, dbal_mngr[unit]->logger_info.dbal_file);

            if (nof_fields_printed < (nof_key_fields - 1))
            {
                DBAL_DUMP(", ", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
            }
            nof_fields_printed++;
        }

        nof_fields_printed = 0;
        if ((entry_handle->nof_result_fields) || (entry_handle->get_all_fields))
        {
            int nof_result_fields = entry_handle->get_all_fields ?
                table->multi_res_info[entry_handle->cur_res_type].nof_result_fields : entry_handle->nof_result_fields;

            DBAL_DUMP("\nRESULT: ", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
            for (iter = 0; iter < table->multi_res_info[entry_handle->cur_res_type].nof_result_fields; iter++)
            {
                dbal_fields_e value_field = entry_handle->value_field_ids[iter];
                if ((entry_handle->value_field_ids[iter] == DBAL_FIELD_EMPTY))
                {
                    if (entry_handle->get_all_fields)
                    {
                        value_field = table->multi_res_info[entry_handle->cur_res_type].results_info[iter].field_id;
                    }
                    else
                    {
                        continue;
                    }
                }

                SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, value_field,
                                                                         entry_handle->table_id,
                                                                         entry_handle->phy_entry.payload, NULL,
                                                                         entry_handle->cur_res_type, FALSE, TRUE,
                                                                         buffer));
                DBAL_DUMP(dbal_field_to_string(unit, value_field), log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
                DBAL_DUMP(" ", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
                DBAL_DUMP(buffer, log_enabled, dbal_mngr[unit]->logger_info.dbal_file);

                if (nof_fields_printed < (nof_result_fields - 1))
                {
                    DBAL_DUMP(", ", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
                }
                nof_fields_printed++;
            }
        }
        DBAL_DUMP("\n", log_enabled, dbal_mngr[unit]->logger_info.dbal_file);
    }

exit:
    SHR_FUNC_EXIT;
}

void
dbal_entry_handle_attribute_parse(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int attr_index;

    for (attr_index = 0; attr_index < DBAL_NOF_ENTRY_ATTR_RES_TYPE; attr_index++)
    {
        if (entry_handle->attribute_info[attr_index] != NULL)
        {
            uint32 *attr_val_ptr;
            attr_val_ptr = entry_handle->attribute_info[attr_index];
            switch (attr_index)
            {
                case DBAL_ENTRY_ATTR_RES_PRIORITY:
                    (*attr_val_ptr) = entry_handle->phy_entry.prefix_length;
                    break;
                case DBAL_ENTRY_ATTR_RES_AGE:
                    (*attr_val_ptr) = entry_handle->phy_entry.age;
                    break;
                case DBAL_ENTRY_ATTR_RES_HIT:
                    (*attr_val_ptr) =
                        entry_handle->phy_entry.hitbit & DBAL_PHYSICAL_KEY_PRIMARY_AND_SECONDARY_HITBIT_ACCESSED;
                    break;
            }
        }
    }
}

shr_error_e
dbal_entry_handle_phy_entry_align(
    int unit,
    uint32 payload_offset,
    uint8 shift_left,
    dbal_entry_handle_t * entry_handle)
{
    uint32 payload_copy[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    uint32 payload_mask_copy[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    dbal_physical_entry_t *phy_entry = &(entry_handle->phy_entry);
    int size = entry_handle->table->max_payload_size;

    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type for entry table %s\n", entry_handle->table->table_name);
    }

    if (shift_left)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(phy_entry->payload, payload_offset, size, payload_copy));
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(payload_copy, 0, size + payload_offset, phy_entry->payload));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(phy_entry->p_mask, payload_offset, size, payload_mask_copy));
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(payload_mask_copy, 0, size + payload_offset, phy_entry->p_mask));
    }
    else
    {
        
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(phy_entry->payload, 0, size, payload_copy));

        
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(phy_entry->payload, 0, payload_offset));

        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (payload_copy, payload_offset, size - payload_offset, phy_entry->payload));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(phy_entry->p_mask, 0, size, payload_mask_copy));

        
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(phy_entry->p_mask, 0, payload_offset));

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (payload_mask_copy, payload_offset, size - payload_offset, phy_entry->p_mask));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_handle_value_fields_parse(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int iter;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_table_field_info_t *table_field_info;

    SHR_FUNC_INIT_VARS(unit);

    if (!entry_handle->user_output_info)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "no pointers for results field to update need to call field request prior to "
                     "field get or call get all fields\n");
    }

    for (iter = 0; iter < table->multi_res_info[entry_handle->cur_res_type].nof_result_fields; iter++)
    {
        if (entry_handle->value_field_ids[iter] == DBAL_FIELD_EMPTY)
        {
            continue;
        }

        if (entry_handle->value_field_ids[iter] == DBAL_FIELD_RESULT_TYPE)
        {
            
            if ((uint32 *) entry_handle->user_output_info[iter].value_returned_pointer)
            {
                uint32 *field_val = (uint32 *) entry_handle->user_output_info[iter].value_returned_pointer;
                field_val[0] = entry_handle->cur_res_type;
            }
            if ((uint32 *) entry_handle->user_output_info[iter].mask_returned_pointer)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "REUSLT_TYPE field should not be used with masked APIs\n");
            }
            continue;
        }

        table_field_info = &table->multi_res_info[entry_handle->cur_res_type].results_info[iter];
        if ((SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID)) &&
            (entry_handle->user_output_info[iter].pointer_type == 0) &&
            (entry_handle->user_output_info[iter].value_returned_pointer == NULL))
        {
            
            entry_handle->value_field_ids[iter] = DBAL_FIELD_EMPTY;
            continue;
        }

        DBAL_VALIDATE_POINTER_SIZE(entry_handle->user_output_info[iter].pointer_type, table_field_info->field_nof_bits);

        switch (entry_handle->user_output_info[iter].pointer_type)
        {
            case DBAL_POINTER_TYPE_UINT8:
            {
                uint8 *ptr_u8_val;
                uint32 field_u32_val[1];

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, table_field_info, entry_handle->value_field_ids[iter],
                                                           entry_handle->phy_entry.payload, field_u32_val));

                ptr_u8_val = (uint8 *) entry_handle->user_output_info[iter].value_returned_pointer;
                *ptr_u8_val = field_u32_val[0] & 0xFF;

                if (entry_handle->user_output_info[iter].mask_returned_pointer)
                {
                    uint8 *ptr_u8_mask;
                    uint32 field_u32_mask[1];
                    SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                                    (unit, table_field_info, entry_handle->value_field_ids[iter],
                                     entry_handle->phy_entry.p_mask, field_u32_mask));

                    ptr_u8_mask = (uint8 *) entry_handle->user_output_info[iter].mask_returned_pointer;
                    *ptr_u8_mask = field_u32_mask[0] & 0xFF;
                }
                break;
            }

            case DBAL_POINTER_TYPE_UINT16:
            {
                uint16 *ptr_u16_val;
                uint32 field_u32_val[1] = { 0 };

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, table_field_info, entry_handle->value_field_ids[iter],
                                                           entry_handle->phy_entry.payload, field_u32_val));

                ptr_u16_val = (uint16 *) entry_handle->user_output_info[iter].value_returned_pointer;
                *ptr_u16_val = field_u32_val[0] & 0xFFFF;

                if (entry_handle->user_output_info[iter].mask_returned_pointer)
                {
                    uint16 *ptr_u16_mask;
                    uint32 field_u32_mask[1];
                    SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                                    (unit, table_field_info, entry_handle->value_field_ids[iter],
                                     entry_handle->phy_entry.p_mask, field_u32_mask));

                    ptr_u16_mask = (uint16 *) entry_handle->user_output_info[iter].mask_returned_pointer;
                    *ptr_u16_mask = field_u32_mask[0] & 0xFFFF;
                }
                break;
            }
            case DBAL_POINTER_TYPE_UINT32:

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, table_field_info, entry_handle->value_field_ids[iter],
                                                           entry_handle->phy_entry.payload,
                                                           (uint32 *) entry_handle->
                                                           user_output_info[iter].value_returned_pointer));

                if (entry_handle->user_output_info[iter].mask_returned_pointer)
                {
                    SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                                    (unit, table_field_info, entry_handle->value_field_ids[iter],
                                     entry_handle->phy_entry.p_mask,
                                     (uint32 *) entry_handle->user_output_info[iter].mask_returned_pointer));
                }

                break;

            case DBAL_POINTER_TYPE_ARR_UINT8:
            {
                uint32 field_array32_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                uint32 field_array32_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, table_field_info, entry_handle->value_field_ids[iter],
                                                           entry_handle->phy_entry.payload, field_array32_val));

                SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                                (unit, table_field_info, entry_handle->value_field_ids[iter],
                                 entry_handle->phy_entry.p_mask, field_array32_mask));

                SHR_IF_ERR_EXIT(dbal_fields_transform_arr32_to_arr8
                                (unit,
                                 entry_handle->table_id, entry_handle->value_field_ids[iter], FALSE,
                                 entry_handle->cur_res_type, 0, field_array32_val,
                                 ((uint8 *) entry_handle->user_output_info[iter].value_returned_pointer),
                                 field_array32_mask,
                                 ((uint8 *) entry_handle->user_output_info[iter].mask_returned_pointer)));
                break;
            }

            case DBAL_POINTER_TYPE_ARR_UINT32:
                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, table_field_info, entry_handle->value_field_ids[iter],
                                                           entry_handle->phy_entry.payload,
                                                           (uint32 *) entry_handle->
                                                           user_output_info[iter].value_returned_pointer));

                if (entry_handle->user_output_info[iter].mask_returned_pointer)
                {
                    SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                                    (unit, table_field_info, entry_handle->value_field_ids[iter],
                                     entry_handle->phy_entry.p_mask,
                                     (uint32 *) entry_handle->user_output_info[iter].mask_returned_pointer));
                }
                break;

            case DBAL_POINTER_TYPE_UINT64:
            {
                uint64 *ptr_u64_val;
                uint32 field_array32_val[2] = { 0 };

                SHR_IF_ERR_EXIT(dbal_field_from_buffer_get(unit, table_field_info, entry_handle->value_field_ids[iter],
                                                           entry_handle->phy_entry.payload, field_array32_val));
                ptr_u64_val = (uint64 *) entry_handle->user_output_info[iter].value_returned_pointer;
                COMPILER_64_SET(*ptr_u64_val, field_array32_val[1], field_array32_val[0]);

                if (entry_handle->user_output_info[iter].mask_returned_pointer)
                {

                    uint64 *ptr_u64_mask;
                    uint32 field_array32_mask[2] = { 0 };

                    SHR_IF_ERR_EXIT(dbal_field_mask_from_buffer_get
                                    (unit, table_field_info, entry_handle->value_field_ids[iter],
                                     entry_handle->phy_entry.p_mask, field_array32_mask));

                    ptr_u64_mask = (uint64 *) entry_handle->user_output_info[iter].mask_returned_pointer;
                    COMPILER_64_SET(*ptr_u64_mask, field_array32_mask[1], field_array32_mask[0]);
                }
                break;
            }

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal type %d\n", entry_handle->user_output_info[iter].pointer_type);
                break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_entry_attribute_request_internal(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint32 attr_type,
    uint32 *attr_val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (attr_type == DBAL_ENTRY_ATTR_PRIORITY)
    {
        if (!SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "priority get supported only for KBP TCAM table\n");
        }
        
        entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_PRIORITY] = attr_val;
    }
    else if (attr_type == DBAL_ENTRY_ATTR_AGE)
    {
        uint32 age_state_not_updated = dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_state_not_updated);
        if (age_state_not_updated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "DBAL_ENTRY_ATTR_AGE is not supported");
        }
        if (!dbal_logical_table_is_mact(unit, entry_handle->table_id))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal use for AGE API in table %s", entry_handle->table->table_name);
        }
        entry_handle->phy_entry.age = 0;
        entry_handle->phy_entry.age = DBAL_PHYSICAL_KEY_AGE_GET;
        entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_AGE] = attr_val;
    }
    else
    {
        uint32 hitbit_en;
        entry_handle->phy_entry.hitbit = 0;

        SHR_IF_ERR_EXIT(dbal_tables_indication_get
                        (unit, entry_handle->table_id, DBAL_TABLE_IND_IS_HITBIT_EN, &hitbit_en));
        if (hitbit_en)
        {
            if (attr_type & DBAL_ENTRY_ATTR_HIT_GET)
            {
                entry_handle->phy_entry.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_GET;
            }
            if (attr_type & DBAL_ENTRY_ATTR_HIT_CLEAR)
            {
                entry_handle->phy_entry.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_CLEAR;
            }
            if (attr_type & DBAL_ENTRY_ATTR_HIT_PRIMARY)
            {
                entry_handle->phy_entry.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_PRIMARY;
            }
            if (attr_type & DBAL_ENTRY_ATTR_HIT_SECONDARY)
            {
                entry_handle->phy_entry.hitbit |= DBAL_PHYSICAL_KEY_HITBIT_SECONDARY;
            }

            
            entry_handle->attribute_info[DBAL_ENTRY_ATTR_RES_HIT] = attr_val;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value)
{
    int field_nof_bits;
    int is_sub_field = 0;
    dbal_fields_e parent_field_id = DBAL_FIELD_EMPTY;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 *curr_field_value_ptr;
    uint32 field_val_after_decoding[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_val_after_valid_ind[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    (*returned_field_value) = 0;

    if (field_id != table_field_info->field_id)
    {
        parent_field_id = table_field_info->field_id;
        is_sub_field = 1;
    }

    field_nof_bits = table_field_info->field_nof_bits;

    SHR_IF_ERR_EXIT(dbal_hw_field_from_buffer_get(unit, table_field_info, buffer, field_value));

    curr_field_value_ptr = field_value;

    if ((table_field_info->arr_prefix_size)
        && (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
    {
        int total_size = table_field_info->arr_prefix_size + table_field_info->field_nof_bits;
        uint32 tmp_field_value = curr_field_value_ptr[0];

        SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field_info->arr_prefix,
                                                  table_field_info->field_nof_bits, total_size, curr_field_value_ptr));

        SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, field_id, DBAL_FIELD_EMPTY, field_nof_bits, curr_field_value_ptr,
                                                 field_val_after_decoding));
        returned_field_value[0] = field_val_after_decoding[0];
    }
    else
    {
        if (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION))
        {
            
            uint32 is_valid = 0;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(curr_field_value_ptr, 0, 1, &is_valid));
            if (!is_valid)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
            SHR_IF_ERR_EXIT(dbal_fields_decode_valid_ind(unit, curr_field_value_ptr, field_nof_bits,
                                                         field_val_after_valid_ind));
            curr_field_value_ptr = field_val_after_valid_ind;
        }
        if ((SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED)) || (is_sub_field))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_decode
                            (unit, field_id, parent_field_id, field_nof_bits, curr_field_value_ptr,
                             field_val_after_decoding));
            curr_field_value_ptr = field_val_after_decoding;

            if (is_sub_field)
            {
                uint32 verify_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
                int ret_val;

                ret_val = dbal_fields_field_encode(unit, field_id, parent_field_id, field_nof_bits,
                                                   field_val_after_decoding, verify_field_val);
                ret_val = dbal_field_encode_handle_rv(unit, NULL, 0, parent_field_id, ret_val);
                SHR_IF_ERR_EXIT(ret_val);

                if (sal_memcmp(verify_field_val, field_value, BITS2BYTES(field_nof_bits)) != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 " Subfield encoding issue requested subfield %s from field %s but value %d didnot match\n",
                                 dbal_field_to_string(unit, field_id),
                                 dbal_field_to_string(unit, table_field_info->field_id), field_value[0]);
                }
            }
        }

        if (!SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM))
        {
            int field_logical_offset = table_field_info->offset_in_logical_field;

            if (field_logical_offset != 0)
            {
                uint32 field_value_after_offset[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (curr_field_value_ptr, field_logical_offset, field_nof_bits, field_value_after_offset));

                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (field_value_after_offset, 0, field_nof_bits + field_logical_offset,
                                 returned_field_value));
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (curr_field_value_ptr, 0, field_nof_bits + field_logical_offset, returned_field_value));
            }
        }
        else
        {
            returned_field_value[0] = curr_field_value_ptr[0];
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_physical_value_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value)
{
    int field_offset_in_buffer, field_nof_bits;

    SHR_FUNC_INIT_VARS(unit);

    (*returned_field_value) = 0;

    field_offset_in_buffer = table_field_info->bits_offset_in_buffer;
    field_nof_bits = table_field_info->field_nof_bits;

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (buffer, field_offset_in_buffer, field_nof_bits, returned_field_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_mask_from_buffer_get(
    int unit,
    dbal_table_field_info_t * table_field_info,
    dbal_fields_e field_id,
    uint32 *buffer,
    uint32 *returned_field_value)
{
    int field_offset_in_buffer, field_nof_bits, field_logical_offset;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_value_after_offset[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    (*returned_field_value) = 0;

    field_offset_in_buffer = table_field_info->bits_offset_in_buffer;
    field_nof_bits = table_field_info->field_nof_bits;
    field_logical_offset = table_field_info->offset_in_logical_field;

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(buffer, field_offset_in_buffer, field_nof_bits, field_value));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (field_value, field_logical_offset, field_nof_bits, field_value_after_offset));
    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                    (field_value_after_offset, 0, field_nof_bits + field_logical_offset, returned_field_value));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_field_struct_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    dbal_field_types_basic_info_t * field_type_info,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int size,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE])
{
    int ii, iter;
    int nof_struct_fields = field_type_info->nof_struct_fields;
    uint32 sub_struct_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    int char_count = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < BITS2WORDS(size); iter++)
    {
        char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x ", field_val[iter]);
        buffer_to_print += char_count;
    }

    char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "(");
    buffer_to_print += char_count;

    for (ii = nof_struct_fields - 1; ii >= 0; ii--)
    {
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode_masked
                        (unit, field_id, field_type_info->struct_field_info[ii].struct_field_id,
                         sub_struct_val, NULL, field_val, NULL, 0));
        

        if (field_type_info->struct_field_info[ii].struct_field_id != DBAL_FIELD_RESERVED)
        {
            dbal_fields_e next_level_field = field_type_info->struct_field_info[ii].struct_field_id;
            dbal_fields_e sub_field_id = DBAL_FIELD_EMPTY;
            uint32 sub_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
            dbal_field_types_basic_info_t *inner_field_info;

            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, next_level_field, &inner_field_info));
            
            if (inner_field_info->nof_child_fields != 0)
            {
                SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                                (unit, next_level_field, sub_struct_val, &sub_field_id, sub_field_val));
            }
            
            if (sub_field_id != DBAL_FIELD_EMPTY)
            {
                next_level_field = sub_field_id;
                sal_memcpy(sub_struct_val, sub_field_val, WORDS2BYTES(DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS));
            }

            char_count =
                sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s ",
                             dbal_field_to_string(unit, next_level_field));
            buffer_to_print += char_count;

            SHR_IF_ERR_EXIT(dbal_field_printable_string_get
                            (unit, next_level_field, sub_struct_val, NULL, 0, DBAL_FIELD_EMPTY,
                             field_type_info->struct_field_info[ii].length, buffer_to_print));
            buffer_to_print += sal_strlen(buffer_to_print);

            if (ii != 0)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "; ");
                buffer_to_print += char_count;
            }
        }
    }
    char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, ")");
    buffer_to_print += char_count;
    sal_snprintf(buffer_to_print, 2, "%s", "\0");

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dbal_field_child_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE])
{
    dbal_fields_e sub_field_id;
    uint32 sub_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    int char_count = 0;
    dbal_field_types_basic_info_t *sub_field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, field_id, field_val, &sub_field_id, sub_field_val));
    if (sub_field_id != DBAL_FIELD_EMPTY)
    {
        dbal_value_field_encode_types_e encode_type;
        
        char_count =
            sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s ", dbal_field_to_string(unit, sub_field_id));
        buffer_to_print += char_count;

        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, sub_field_id, &sub_field_info));

        SHR_IF_ERR_EXIT(dbal_fields_encode_type_get(unit, sub_field_id, &encode_type));
        if (dbal_fields_is_field_encoded(unit, sub_field_id) || (encode_type == DBAL_VALUE_FIELD_ENCODE_STRUCTURE))
        {
            
            return dbal_field_printable_string_get(unit, sub_field_id, sub_field_val, NULL, 1, field_id,
                                                   sub_field_info->max_size, buffer_to_print);
        }
        else
        {
            sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%d (%d)%s", sub_field_val[0], field_val[0], "\0");
        }
    }
    else
    {
        sal_snprintf(buffer_to_print, sizeof("No child field found!"), "%s", "No child field found!");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_field_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    multi_res_info_t * result_info,
    uint8 is_full_buffer,
    dbal_fields_e parent_field,
    int size,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE])
{
    int iter, char_count = 0, is_field_encoded = 0, ii;
    uint32 decoded_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_type_info));

    char_count = 0;

    if (field_id == DBAL_FIELD_RESULT_TYPE)
    {
        if (result_info == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " input field_id is DBAL_FIELD_RESULT_TYPE, but result_type is NULL\n");
        }
        char_count =
            sal_snprintf(buffer_to_print, sizeof(result_info->result_type_name), "%s ", result_info->result_type_name);
        buffer_to_print += char_count;
        sal_snprintf(buffer_to_print, 2, "%s ", "\0");

        SHR_EXIT();
    }

    if (is_full_buffer && ((dbal_fields_is_field_encoded(unit, field_id)) || (parent_field != DBAL_FIELD_EMPTY)))
    {
        
        SHR_IF_ERR_EXIT(dbal_fields_field_decode(unit, field_id, parent_field, size, field_val, decoded_field_val));
        is_field_encoded = 1;
    }

    
    for (ii = 0; ii < field_type_info->nof_defines_values; ii++)
    {
        if (field_type_info->defines_info[ii].value == field_val[0])
        {
            char_count =
                sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s ", field_type_info->defines_info[ii].name);
            buffer_to_print += char_count;
            sal_snprintf(buffer_to_print, 2, "%s", "\0");
            SHR_EXIT();
        }
    }

    
    if (field_type_info->nof_child_fields != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_child_printable_string_get(unit, field_id, field_val, buffer_to_print));
        SHR_EXIT();
    }

    
    if (field_type_info->nof_struct_fields != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_struct_printable_string_get
                        (unit, field_id, field_type_info, field_val, size, buffer_to_print));
        SHR_EXIT();
    }

    switch (field_type_info->print_type)
    {
        case DBAL_FIELD_PRINT_TYPE_STR:
        {
            uint8 buf[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];
            SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_val, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, buf));

            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%s ", buf);
            buffer_to_print += char_count;
        }
            break;
        case DBAL_FIELD_PRINT_TYPE_HEX:
            if (is_field_encoded)
            {
                char_count =
                    sal_snprintf(buffer_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%x (0x%x)", decoded_field_val[0],
                                 field_val[0]);
                buffer_to_print += char_count;
            }
            else
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "0x%x ", field_val[0]);
                buffer_to_print += char_count;
            }

            break;
        case DBAL_FIELD_PRINT_TYPE_UINT32:

            if (is_field_encoded)
            {
                char_count =
                    sal_snprintf(buffer_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%d (%d)", decoded_field_val[0],
                                 field_val[0]);
                buffer_to_print += char_count;

            }
            else
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%d", field_val[0]);
                buffer_to_print += char_count;
            }
            break;

        case DBAL_FIELD_PRINT_TYPE_IPV4:
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%d.%d.%d.%d (0x%x) ",
                                      UTILEX_GET_BYTE_0(field_val[0]), UTILEX_GET_BYTE_1(field_val[0]),
                                      UTILEX_GET_BYTE_2(field_val[0]), UTILEX_GET_BYTE_3(field_val[0]), field_val[0]);
            buffer_to_print += char_count;
            break;

        case DBAL_FIELD_PRINT_TYPE_IPV6:
        {
            uint8 u8_val[UTILEX_PP_IPV6_ADDRESS_NOF_U8] = { 0 };
            int start_byte_to_print = UTILEX_PP_IPV6_ADDRESS_NOF_U8 - BITS2BYTES(size);
            int size_to_print = UTILEX_PP_IPV6_ADDRESS_NOF_U8;
            dbal_field_types_defs_e field_type;

            start_byte_to_print -= start_byte_to_print & 0x1;   

            SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));
            if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) || (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))
            {
                SHR_IF_ERR_EXIT(utilex_pp_ipv6_address_long_to_struct(field_val, u8_val));
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_val, BITS2BYTES(size), u8_val));
                start_byte_to_print = 0;
                size_to_print = BITS2BYTES(size);
            }

            if ((start_byte_to_print < 0) || (start_byte_to_print > (UTILEX_PP_IPV6_ADDRESS_NOF_U8 - 2)))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal start byte to print, something fault with calculations\n");
            }

            for (iter = start_byte_to_print; iter < size_to_print - 2;)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%02x", u8_val[iter++]);
                buffer_to_print += char_count;
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%02x:", u8_val[iter++]);
                buffer_to_print += char_count;
            }
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%02x", u8_val[iter++]);
            buffer_to_print += char_count;
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%02x", u8_val[iter++]);
            buffer_to_print += char_count;
            break;
        }
        case DBAL_FIELD_PRINT_TYPE_MAC:
        {
            uint8 u8_val[UTILEX_PP_MAC_ADDRESS_NOF_U8] = { 0 };
            int size_to_print = UTILEX_PP_MAC_ADDRESS_NOF_U8;
            int start_byte_to_print = UTILEX_PP_MAC_ADDRESS_NOF_U8 - BITS2BYTES(size);
            dbal_field_types_defs_e field_type;

            SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

            if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)
            {
                SHR_IF_ERR_EXIT(utilex_pp_mac_address_long_to_struct(field_val, u8_val));
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_val, BITS2BYTES(size), u8_val));
                start_byte_to_print = 0;
                size_to_print = BITS2BYTES(size);
            }

            for (iter = start_byte_to_print; iter < size_to_print - 1; iter++)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%02x:", u8_val[iter]);
                buffer_to_print += char_count;
            }
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%02x", u8_val[iter]);
            buffer_to_print += char_count;
            break;
        }
        case DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE:
            if (field_val[0] == DBAL_CORE_ALL)
            {
                char_count = sal_snprintf(buffer_to_print, 5, "ALL ");
            }
            else
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%d ", field_val[0]);
            }
            buffer_to_print += char_count;
            break;

        case DBAL_FIELD_PRINT_TYPE_BITMAP:
        {
            for (iter = 0; iter < BITS2WORDS(size) - 1; iter++)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x, ", field_val[iter]);
                buffer_to_print += char_count;
            }
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x ", field_val[iter]);
            buffer_to_print += char_count;

            break;
        }

        case DBAL_FIELD_PRINT_TYPE_BOOL:
            char_count =
                sal_snprintf(buffer_to_print, DBAL_MAX_PRINTABLE_BUFFER_SIZE, "%s ", field_val[0] ? "True" : "False");
            buffer_to_print += char_count;
            break;

        case DBAL_FIELD_PRINT_TYPE_DBAL_TABLE:
            char_count =
                sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s ",
                             dbal_logical_table_to_string(unit, field_val[0]));
            buffer_to_print += char_count;
            break;

        case DBAL_FIELD_PRINT_TYPE_ARRAY8:
        {
            uint8 u8_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
            uint8 u8_decoded_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };

            if (is_field_encoded)
            {
                SHR_IF_ERR_EXIT(utilex_U32_to_U8(decoded_field_val, BITS2BYTES(size), u8_decoded_val));
                SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_val, BITS2BYTES(size), u8_val));
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_val, BITS2BYTES(size), u8_decoded_val));
            }

            for (iter = 0; iter < BITS2BYTES(size) - 1; iter++)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x, ", u8_decoded_val[iter]);
                buffer_to_print += char_count;
            }
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x ", u8_decoded_val[iter]);
            buffer_to_print += char_count;

            if (is_field_encoded)
            {
                char_count = sal_snprintf(buffer_to_print, 2, "(");
                buffer_to_print += char_count;

                for (iter = 0; iter < BITS2BYTES(size) - 1; iter++)
                {
                    char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x, ", u8_val[iter]);
                    buffer_to_print += char_count;
                }
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x)", u8_val[iter]);
                buffer_to_print += char_count;
            }
            break;
        }

        case DBAL_FIELD_PRINT_TYPE_ARRAY32:

            for (iter = 0; iter < BITS2WORDS(size) - 1; iter++)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x, ", field_val[iter]);
                buffer_to_print += char_count;
            }
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x ", field_val[iter]);
            buffer_to_print += char_count;

            if (is_field_encoded)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "(");
                buffer_to_print += char_count;

                for (iter = 0; iter < BITS2WORDS(size) - 1; iter++)
                {
                    char_count =
                        sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x, ", decoded_field_val[iter]);
                    buffer_to_print += char_count;
                }
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "0x%x)", decoded_field_val[iter]);
                buffer_to_print += char_count;
            }

            break;

        case DBAL_FIELD_PRINT_TYPE_ENUM:
        {
            uint32 enum_val = field_val[0];

            if (is_field_encoded)
            {
                enum_val = decoded_field_val[0];
            }
            if (enum_val == UTILEX_U32_MAX)
            {
                char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s ", "Unknown Enum Val");
            }
            else
            {
                char_count =
                    sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s (0x%x) ",
                                 field_type_info->enum_val_info[enum_val].name,
                                 field_type_info->enum_val_info[enum_val].value);
            }
            buffer_to_print += char_count;
            break;
        }

        case DBAL_FIELD_PRINT_TYPE_STRUCTURE:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " struct fields should be handled before the swtich \n");
        }
            break;

        default:
            break;
    }

    
    sal_snprintf(buffer_to_print, 2, "%s", "\0");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_in_table_printable_string_get(
    int unit,
    dbal_fields_e field_id,
    dbal_tables_e table_id,
    uint32 buffer_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 buffer_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    uint8 is_full_buffer,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE])
{
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    int offset = 0, size = 0, char_count = 0, iter = 0;
    dbal_fields_e parent_field = DBAL_FIELD_EMPTY;
    dbal_table_field_info_t table_field_info = { 0 };
    CONST dbal_logical_table_t *table;
    multi_res_info_t *result_info = NULL;
    dbal_field_print_type_e field_print_type = DBAL_FIELD_PRINT_TYPE_NONE;

    SHR_FUNC_INIT_VARS(unit);

    dbal_fields_print_type_get(unit, field_id, &field_print_type);

    if ((is_full_buffer == 0) && (field_print_type == DBAL_FIELD_PRINT_TYPE_ENUM))
    {
        
        field_val[0] = buffer_val[0];
        size = SAL_UINT32_NOF_BITS; 
        if (buffer_mask)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(buffer_mask, offset, size, field_mask));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, table_id, field_id, is_key, result_type_idx, 0, &table_field_info));
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        size = table_field_info.field_nof_bits;

        
        if (is_full_buffer)
        {
            offset = table_field_info.bits_offset_in_buffer;
            if (field_id == DBAL_FIELD_CORE_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field CORE_ID can't be printed from buffer, "
                             "should be handled outside the API\n");
            }
        }
        else
        {
            offset = table_field_info.offset_in_logical_field;
        }

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(buffer_val, offset, size, field_val));
        if (buffer_mask)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(buffer_mask, offset, size, field_mask));
        }

        if (table_field_info.field_id != field_id)
        {
            parent_field = table_field_info.field_id;
        }

        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            result_info = &(table->multi_res_info[result_type_idx]);
        }

        if (table_field_info.arr_prefix_size)
        {
            int total_size = table_field_info.arr_prefix_size + table_field_info.field_nof_bits;
            uint32 tmp_field_value = field_val[0];

            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field_info.arr_prefix,
                                                      table_field_info.field_nof_bits, total_size, field_val));
        }
    }
    SHR_IF_ERR_EXIT(dbal_field_printable_string_get(unit, field_id, field_val, result_info,
                                                    is_full_buffer, parent_field, size, buffer_to_print));

    char_count = sal_strlen(buffer_to_print);
    buffer_to_print += char_count;

    
    if (buffer_mask)
    {
        char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%s: 0x", " mask");
        buffer_to_print += char_count;

        char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%x", field_mask[BITS2WORDS(size) - 1]);
        buffer_to_print += char_count;

        for (iter = BITS2WORDS(size) - 2; iter >= 0; iter--)
        {
            char_count = sal_snprintf(buffer_to_print, DBAL_MAX_STRING_LENGTH, "%08x", field_mask[iter]);
            buffer_to_print += char_count;
        }
    }

    char_count = sal_snprintf(buffer_to_print, 2, "%s", "\0");
    buffer_to_print += char_count;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_transform_arr32_to_arr8(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type_index,
    int inst_id,
    uint32 *field_val_u32,
    uint8 *field_val,
    uint32 *field_mask_u32,
    uint8 *field_mask)
{
    int instance_idx;
    dbal_table_field_info_t field_info;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    if ((inst_id == INST_ALL) || (inst_id == INST_SINGLE))
    {
        instance_idx = 0;
    }
    else
    {
        instance_idx = inst_id;
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, table_id, field_id, is_key, res_type_index, instance_idx, &field_info));

    GET_FIELD_TYPE(field_id, &field_type);
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)
    {
        utilex_pp_mac_address_long_to_struct(field_val_u32, field_val);
        if ((field_mask_u32) && (field_mask))
        {
            utilex_pp_mac_address_long_to_struct(field_mask_u32, field_mask);
        }
    }
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) || (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))
    {
        utilex_pp_ipv6_address_long_to_struct(field_val_u32, field_val);
        if ((field_mask_u32) && (field_mask))
        {
            utilex_pp_ipv6_address_long_to_struct(field_mask_u32, field_mask);
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_val_u32, BITS2BYTES(field_info.field_nof_bits), field_val));
        if (field_mask_u32)
        {
            SHR_IF_ERR_EXIT(utilex_U32_to_U8(field_mask_u32, BITS2BYTES(field_info.field_nof_bits), field_mask));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_transform_arr8_to_arr32(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type_index,
    int inst_id,
    uint8 *field_val,
    uint32 *field_val_u32,
    uint8 *field_mask,
    uint32 *field_mask_u32)
{
    int instance_idx;
    dbal_table_field_info_t field_info;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    if ((inst_id == INST_ALL) || (inst_id == INST_SINGLE))
    {
        instance_idx = 0;
    }
    else
    {
        instance_idx = inst_id;
    }

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, table_id, field_id, is_key, res_type_index, instance_idx, &field_info));

    GET_FIELD_TYPE(field_id, &field_type);
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)
    {
        utilex_pp_mac_address_struct_to_long(field_val, field_val_u32);
        if ((field_mask_u32) && (field_mask))
        {
            utilex_pp_mac_address_struct_to_long(field_mask, field_mask_u32);
        }
    }
    else if (field_type == DBAL_FIELD_TYPE_DEF_IPV6)
    {
        utilex_pp_ipv6_address_struct_to_long(field_val, field_val_u32);
        if ((field_mask_u32) && (field_mask))
        {
            utilex_pp_ipv6_address_struct_to_long(field_mask, field_mask_u32);
        }
    }
    else if (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP)
    {
        utilex_pp_ipv6_group_struct_to_long(field_val, field_val_u32);
        if ((field_mask_u32) && (field_mask))
        {
            utilex_pp_ipv6_group_struct_to_long(field_mask, field_mask_u32);
        }
    }
    else if (field_type == DBAL_FIELD_TYPE_DEF_STRING)
    {
        int size = sal_strlen((char *) (field_val));
        SHR_IF_ERR_EXIT(utilex_U8_to_U32(field_val, size, field_val_u32));
        if ((field_mask_u32) && (field_mask))
        {
            SHR_IF_ERR_EXIT(utilex_U8_to_U32(field_mask, size, field_mask_u32));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_U8_to_U32(field_val, BITS2BYTES(field_info.field_nof_bits), field_val_u32));
        if ((field_mask_u32) && (field_mask))
        {
            SHR_IF_ERR_EXIT(utilex_U8_to_U32(field_mask, BITS2BYTES(field_info.field_nof_bits), field_mask_u32));
        }
    }

exit:
    SHR_FUNC_EXIT;

}


shr_error_e
dbal_lpm_prefix_length_to_mask(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_logical_table_t *table = entry_handle->table;
    uint32 key_size, prefix_length;

    SHR_FUNC_INIT_VARS(unit);

    key_size = table->key_size;
    prefix_length = entry_handle->phy_entry.prefix_length;
    sal_memset(entry_handle->phy_entry.k_mask, 0x0, DBAL_PHYSICAL_KEY_SIZE_IN_BYTES);

    
    if (((prefix_length == 0) && (table->access_method != DBAL_ACCESS_METHOD_KBP)) || (prefix_length > key_size))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Iterator found an LPM entry with illegal prefix length of %d. table %s, key size %d\n",
                     prefix_length, table->table_name, key_size);
    }
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range
                    (entry_handle->phy_entry.k_mask, key_size - prefix_length, key_size - 1));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_lpm_mask_to_prefix_length(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    uint32 nof_on_bits = 0;
    uint8 reached_on_end = FALSE;
    int bit_index;
    dbal_logical_table_t *table = entry_handle->table;
    int max_bit_index = entry_handle->table->key_size - 1;
    int min_bit_index = table->keys_info[table->nof_key_fields - 1].bits_offset_in_buffer;

    SHR_FUNC_INIT_VARS(unit);

    
    if (entry_handle->table->app_id_size > 0)
    {
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry_handle->phy_entry.k_mask,
                                                       max_bit_index - entry_handle->table->app_id_size + 1,
                                                       max_bit_index));
        
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field(entry_handle->phy_entry.key,
                                                   max_bit_index - entry_handle->table->app_id_size + 1,
                                                   entry_handle->table->app_id_size, entry_handle->table->app_id));
        nof_on_bits = entry_handle->table->app_id_size;
    }

    
    for (bit_index = max_bit_index - entry_handle->table->app_id_size; bit_index >= min_bit_index; --bit_index)
    {
        int is_bit_on = utilex_bitstream_test_bit(entry_handle->phy_entry.k_mask, bit_index);

        if (reached_on_end == FALSE)
        {
            if (is_bit_on)
            {
                nof_on_bits++;
            }
            else
            {
                reached_on_end = TRUE;
                if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_NO_VALIDATIONS))
                {
                    break;
                }
            }
        }
        else
        {
            if (is_bit_on)
            {
                int key_field_index;
                uint8 is_zero_pad_field = FALSE;

                
                for (key_field_index = 0; key_field_index < table->nof_key_fields; key_field_index++)
                {
                    if (table->keys_info[key_field_index].field_type == DBAL_FIELD_TYPE_DEF_ZERO_PADDING)
                    {
                        if ((table->keys_info[key_field_index].bits_offset_in_buffer +
                             table->keys_info[key_field_index].field_nof_bits - 1) == bit_index)
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(entry_handle->phy_entry.k_mask,
                                                                             table->keys_info
                                                                             [key_field_index].bits_offset_in_buffer,
                                                                             bit_index));
                            is_zero_pad_field = TRUE;
                        }
                    }
                }
                if (!is_zero_pad_field)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "For LPM tables, a valid k_mask's format is: 1....10....0. Bit %d in k_mask is not valid (Expected to be 0)\n",
                                 bit_index);
                }
            }
        }
    }

    entry_handle->phy_entry.prefix_length = nof_on_bits;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_key_or_value_condition_check(
    int unit,
    dbal_entry_handle_t * entry_handle,
    uint8 is_key,
    dbal_access_condition_info_t * access_condition,
    int nof_conditions,
    uint8 *is_passed)
{
    int ii;
    int cond_indx;
    int bits_to_compare, words_to_compare;
    uint32 compared_value[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };
    dbal_access_condition_info_t condition;
    uint8 is_condition_true;

    SHR_FUNC_INIT_VARS(unit);

    for (cond_indx = 0; cond_indx < nof_conditions; cond_indx++)
    {
        dbal_table_field_info_t field_info = { 0 };
        is_condition_true = FALSE;
        condition = access_condition[cond_indx];

        if (condition.field_id != DBAL_FIELD_EMPTY)
        {
            if (is_key)
            {
                int rv;
                field_info = (entry_handle->table->keys_info[condition.field_pos_in_table]);
                if (condition.field_id == DBAL_FIELD_CORE_ID)
                {
                    rv = dbal_entry_key_field_core_id_get(unit, entry_handle, compared_value);
                }
                else
                {
                    rv = dbal_field_from_buffer_get
                        (unit, &field_info, condition.field_id, entry_handle->phy_entry.key, compared_value);
                }
                SHR_IF_ERR_EXIT_WITH_LOG(rv, "issue with field %s table %s %s\n",
                                         dbal_field_to_string(unit, condition.field_id),
                                         entry_handle->table->table_name, EMPTY);

                bits_to_compare = field_info.field_nof_bits;
            }
            else
            {
                dbal_table_field_info_t field_info;
                int res;
                *is_passed = 0;

                res = dbal_tables_field_info_get_no_err(unit, entry_handle->table_id, condition.field_id,
                                                        FALSE, entry_handle->cur_res_type, 0, &field_info);
                
                if ((res == _SHR_E_NOT_FOUND) && (entry_handle->table->nof_result_types > 1))
                {
                    SHR_EXIT();
                }

                SHR_IF_ERR_EXIT_WITH_LOG(res, "field %s is not part of %s table %s\n",
                                         dbal_field_to_string(unit, condition.field_id),
                                         entry_handle->table->table_name, EMPTY);

                SHR_IF_ERR_EXIT(dbal_entry_value_field_get
                                (unit, entry_handle, condition.field_id, 0, compared_value, NULL,
                                 DBAL_POINTER_TYPE_ARR_UINT32));

                bits_to_compare = field_info.field_nof_bits;
            }
        }
        else
        {
            if (is_key)
            {
                bits_to_compare = entry_handle->table->key_size;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (entry_handle->phy_entry.key, 0, entry_handle->table->key_size, compared_value));
            }
            else
            {
                bits_to_compare = DBAL_RES_INFO.entry_payload_size;
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                (entry_handle->phy_entry.payload, 0, DBAL_RES_INFO.entry_payload_size, compared_value));
            }
        }
        words_to_compare = BITS2WORDS(bits_to_compare);
        switch (condition.type)
        {
            case DBAL_CONDITION_BIGGER_THAN:
                is_condition_true = FALSE;
                for (ii = words_to_compare - 1; ii >= 0; ii--)
                {
                    if (compared_value[ii] > condition.value[ii])
                    {
                        is_condition_true = TRUE;
                        break;
                    }
                    else if (compared_value[ii] < condition.value[ii])
                    {
                        is_condition_true = FALSE;
                        break;
                    }
                    else if (ii == 0)
                    {
                        
                        is_condition_true = FALSE;
                        break;
                    }
                }
                break;
            case DBAL_CONDITION_LOWER_THAN:
                is_condition_true = FALSE;
                for (ii = words_to_compare - 1; ii >= 0; ii--)
                {
                    if (compared_value[ii] < condition.value[ii])
                    {
                        is_condition_true = TRUE;
                        break;
                    }
                    else if (compared_value[ii] > condition.value[ii])
                    {
                        is_condition_true = FALSE;
                        break;
                    }
                    else if (ii == 0)
                    {
                        
                        is_condition_true = FALSE;
                        break;
                    }
                }
                break;
            case DBAL_CONDITION_EQUAL_TO:
                is_condition_true = TRUE;
                for (ii = words_to_compare - 1; ii >= 0; ii--)
                {
                    uint32 condition_value;
                    compared_value[ii] &= condition.mask[ii];
                    condition_value = condition.value[ii] & condition.mask[ii];

                    if (compared_value[ii] != condition_value)
                    {
                        is_condition_true = FALSE;
                        break;
                    }
                }
                break;
            case DBAL_CONDITION_NOT_EQUAL_TO:
                is_condition_true = FALSE;
                for (ii = words_to_compare - 1; ii >= 0; ii--)
                {
                    uint32 condition_value;
                    compared_value[ii] &= condition.mask[ii];
                    condition_value = condition.value[ii] & condition.mask[ii];

                    if (compared_value[ii] != condition_value)
                    {
                        is_condition_true = TRUE;
                        break;
                    }
                }
                break;
            case DBAL_CONDITION_NONE:
                is_condition_true = TRUE;
                break;
            case DBAL_CONDITION_IS_EVEN:
                if (compared_value[0] % 2 == 0)
                {
                    is_condition_true = TRUE;
                }
                else
                {
                    is_condition_true = FALSE;
                }
                break;
            case DBAL_CONDITION_IS_ODD:
                if (compared_value[0] % 2 == 1)
                {
                    is_condition_true = TRUE;
                }
                else
                {
                    is_condition_true = FALSE;
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal condition value %d\n", condition.type);
                break;
        }

        if (is_condition_true == FALSE)
        {
            *is_passed = 0;
            SHR_EXIT();
        }
    }

    if (dbal_mngr[unit]->iterators_pool[entry_handle->handle_id].is_init)
    {
        dbal_iterator_attribute_info_t *iterator_attrib_info =
            &(dbal_mngr[unit]->iterators_pool[entry_handle->handle_id].attrib_info);

        
        if (iterator_attrib_info->hit_bit_rule_valid == TRUE)
        {
            uint32 attr_val, is_hit;

            SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle->handle_id,
                                                     iterator_attrib_info->hit_bit_rule_type, &attr_val));
            is_hit = attr_val != 0;
            if (is_hit != iterator_attrib_info->hit_bit_rule_is_hit)
            {
                *is_passed = 0;
                SHR_EXIT();
            }
        }

        
        if (iterator_attrib_info->age_rule_valid == TRUE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle->handle_id,
                                                     DBAL_PHYSICAL_KEY_AGE_GET,
                                                     &iterator_attrib_info->age_rule_entry_value));

            if (iterator_attrib_info->age_rule_entry_value != iterator_attrib_info->age_rule_compare_value)
            {
                *is_passed = 0;
                SHR_EXIT();
            }
        }
    }
    *is_passed = 1;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_key_buffer_validate(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_key_value_validity_e * is_valid)
{
    int ii, jj;
    dbal_logical_table_t *table = entry_handle->table;
    dbal_field_types_basic_info_t *field_info;
    dbal_iterator_info_t *iterator_info = NULL;
    dbal_field_data_t key_info;

    SHR_FUNC_INIT_VARS(unit);

    *is_valid = DBAL_KEY_IS_VALID;
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        sal_memset(&key_info, 0, sizeof(dbal_field_data_t));
        
        key_info.field_id = table->keys_info[ii].field_id;
        if (table->keys_info[ii].field_id == DBAL_FIELD_CORE_ID)
        {
            if ((entry_handle->core_id == DBAL_CORE_ALL) ||
                ((entry_handle->core_id < DBAL_MAX_NUM_OF_CORES) && (entry_handle->core_id >= DBAL_CORE_DEFAULT)))
            {
                key_info.field_val[0] = entry_handle->core_id;
            }
            else
            {
                *is_valid = DBAL_KEY_IS_INVALID;
                SHR_EXIT();
            }
        }
        else
        {
            uint32 temp_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(entry_handle->phy_entry.key,
                                                           table->keys_info[ii].bits_offset_in_buffer,
                                                           table->keys_info[ii].field_nof_bits, temp_field_val));

            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(temp_field_val,
                                                           table->keys_info[ii].offset_in_logical_field,
                                                           table->keys_info[ii].field_nof_bits, key_info.field_val));
        }

        
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, table->keys_info[ii].field_id, &field_info));
        if (field_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
        {
            int max_sw_value = 0;
            int min_sw_value = 0;

            max_sw_value = table->keys_info[ii].max_value;
            min_sw_value = table->keys_info[ii].min_value;

            for (jj = min_sw_value; jj <= max_sw_value; jj++)
            {
                if (key_info.field_val[0] == field_info->enum_val_info[jj].value)
                {
                    key_info.field_val[0] = jj;
                    break;
                }
            }
            if (jj > max_sw_value)
            {
                *is_valid = DBAL_KEY_IS_INVALID;
                SHR_EXIT();
            }
            else
            {
                *is_valid = DBAL_KEY_IS_VALID;
            }
        }
        else if (((key_info.field_val[0] > table->keys_info[ii].max_value) ||
                  (key_info.field_val[0] < table->keys_info[ii].min_value)) &&
                 (table->keys_info[ii].field_nof_bits <= 31))
        {
            
            if (entry_handle->table_id == DBAL_TABLE_GLOBAL_LIF_EM)
            {
                if (key_info.field_val[0] < table->keys_info[ii].min_value)
                {
                    *is_valid = DBAL_KEY_IS_OUT_OF_ITERATOR_RULE;
                }
                else
                {
                    *is_valid = DBAL_KEY_IS_INVALID;
                }
                SHR_EXIT();
            }
            else if (entry_handle->table_id == DBAL_TABLE_GLOBAL_RIF_EM)
            {
                if (key_info.field_val[0] > table->keys_info[ii].max_value)
                {
                    *is_valid = DBAL_KEY_IS_OUT_OF_ITERATOR_RULE;
                }
                else
                {
                    *is_valid = DBAL_KEY_IS_INVALID;
                }
                SHR_EXIT();
            }
            else
            {
                *is_valid = DBAL_KEY_IS_INVALID;
                SHR_EXIT();
            }
        }
        else
        {
            uint8 is_illegal = FALSE;
            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, table->keys_info[ii].field_id,
                                                         key_info.field_val[0], &is_illegal));
            if (is_illegal)
            {
                *is_valid = DBAL_KEY_IS_INVALID;
                SHR_EXIT();
            }
        }
    }

    if (iterator_info->nof_key_rules > 0)
    {
        uint8 condition_pass = TRUE;
        SHR_IF_ERR_EXIT(dbal_key_or_value_condition_check(unit, entry_handle, TRUE, iterator_info->key_rules_info,
                                                          iterator_info->nof_key_rules, &condition_pass));
        if (condition_pass == FALSE)
        {
            *is_valid = DBAL_KEY_IS_OUT_OF_ITERATOR_RULE;
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dbal_logger_is_enable(
    int unit,
    dbal_tables_e table_id)
{

    DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_ERR();

    
    if (!dbal_mngr[unit]->logger_info.disable_logger)
    {
        if ((dbal_mngr[unit]->logger_info.user_log_locked_table == DBAL_TABLE_EMPTY) ||
            (table_id == dbal_mngr[unit]->logger_info.user_log_locked_table))
        {
            if ((dbal_mngr[unit]->logger_info.internal_log_locked_table == DBAL_TABLE_EMPTY) ||
                (table_id == dbal_mngr[unit]->logger_info.internal_log_locked_table))
            {
                if (sal_thread_self() == sal_thread_main_get())
                {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

dbal_status_e
dbal_mngr_status_get(
    int unit)
{
    return dbal_mngr[unit]->status;
}

shr_error_e
dbal_logger_init(
    int unit)
{
    CONST dnx_data_dbal_diag_loggerInfo_t *logger_info;

    SHR_FUNC_INIT_VARS(unit);

    
    dbal_mngr[unit]->logger_info.disable_logger = 0;
    dbal_mngr[unit]->logger_info.user_log_locked_table = DBAL_TABLE_EMPTY;
    dbal_mngr[unit]->logger_info.internal_log_locked_table = DBAL_TABLE_EMPTY;
    dbal_mngr[unit]->logger_info.logger_mode = DBAL_LOGGER_MODE_REGULAR;

    logger_info = dnx_data_dbal.diag.loggerInfo_get(unit);

    if (logger_info->file_name)
    {
        dbal_mngr[unit]->logger_info.dbal_file = sal_fopen(logger_info->file_name, "a");
        DBAL_DUMP("INIT PERFORMED\n\n", 0, dbal_mngr[unit]->logger_info.dbal_file);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_mngr[unit]->logger_info.dbal_file != NULL)
    {
        DBAL_DUMP("DEVICE DE-INIT\n\n", 0, dbal_mngr[unit]->logger_info.dbal_file);
        sal_fclose(dbal_mngr[unit]->logger_info.dbal_file);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_file_get(
    int unit,
    FILE ** dbal_file)
{
    SHR_FUNC_INIT_VARS(unit);

    (*dbal_file) = dbal_mngr[unit]->logger_info.dbal_file;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_file_open(
    int unit,
    char *file_name)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_mngr[unit]->logger_info.dbal_file == NULL)
    {
        dbal_mngr[unit]->logger_info.dbal_file = sal_fopen(file_name, "a");
        DBAL_DUMP("FILE OPENED\n\n", 0, dbal_mngr[unit]->logger_info.dbal_file);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "File already open!\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_file_close(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_mngr[unit]->logger_info.dbal_file != NULL)
    {
        DBAL_DUMP("FILE CLOSED\n\n", 0, dbal_mngr[unit]->logger_info.dbal_file);
        sal_fclose(dbal_mngr[unit]->logger_info.dbal_file);
        dbal_mngr[unit]->logger_info.dbal_file = NULL;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_table_user_lock(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    dbal_mngr[unit]->logger_info.user_log_locked_table = table_id;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_table_mode_set_internal(
    int unit,
    dbal_logger_mode_e mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mode >= DBAL_NOF_LOGGER_MODES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mode %d is out of range\n", mode);
    }

    dbal_mngr[unit]->logger_info.logger_mode = mode;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_user_locked_table_get(
    int unit,
    dbal_tables_e * table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    *table_id = dbal_mngr[unit]->logger_info.user_log_locked_table;

    SHR_FUNC_EXIT;
}

void
dbal_logger_internal_disable_set(
    int unit)
{
    DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_VOID();

    if (sal_thread_self() == sal_thread_main_get())
    {
        dbal_mngr[unit]->logger_info.disable_logger = TRUE;
    }
}

void
dbal_logger_internal_disable_clear(
    int unit)
{
    DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_VOID();

    if (sal_thread_self() == sal_thread_main_get())
    {
        dbal_mngr[unit]->logger_info.disable_logger = FALSE;
    }
}

shr_error_e
dbal_logger_table_internal_lock(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_ERR();

    if (sal_thread_self() == sal_thread_main_get())
    {
        if (dbal_mngr[unit]->logger_info.internal_log_locked_table == DBAL_TABLE_EMPTY)
        {
            dbal_mngr[unit]->logger_info.internal_log_locked_table = table_id;
        }
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logger_table_internal_unlock(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_CHECK_LOGGER_SEVERITY_WITH_INFO_ERR();

    if (sal_thread_self() == sal_thread_main_get())
    {
        if (dbal_mngr[unit]->logger_info.internal_log_locked_table == table_id)
        {
            dbal_mngr[unit]->logger_info.internal_log_locked_table = DBAL_TABLE_EMPTY;
        }
    }

    SHR_FUNC_EXIT;
}

void
dbal_logger_info_get_internal(
    int unit,
    CONST dbal_logger_info_t ** logger_info)
{
    *logger_info = &dbal_mngr[unit]->logger_info;
}

shr_error_e
dbal_iterator_increment_by_allocator(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_key_value_validity_e * is_valid_entry)
{
    dbal_logical_table_t *table;
    uint8 is_allocator;
    dbal_table_field_info_t table_field_info = { 0 };
    uint32 allocator_val[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    (*is_valid_entry) = DBAL_KEY_IS_INVALID;

    table = entry_handle->table;

    SHR_IF_ERR_EXIT(dbal_fields_is_allocator_get(unit, table->allocator_field_id, &is_allocator));

    if (is_allocator)
    {
        int index;

        SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                        (unit, entry_handle->table_id, table->allocator_field_id, 1, 0, 0, &table_field_info));
        SHR_IF_ERR_EXIT(dbal_field_from_buffer_get
                        (unit, &table_field_info, table->allocator_field_id, entry_handle->phy_entry.key,
                         allocator_val));
        index = allocator_val[0];

        if (index > table_field_info.max_value)
        {
            
            index = DNX_ALGO_RES_DBAL_ILLEGAL_ELEMENT;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_get_next
                            (unit, entry_handle->core_id, 0, entry_handle->table_id, table->allocator_field_id,
                             &entry_handle->cur_res_type, &index));
        }
        if ((index != DNX_ALGO_RES_DBAL_ILLEGAL_ELEMENT) && (index <= table_field_info.max_value))
        {
            (*is_valid_entry) = DBAL_KEY_IS_VALID;
            allocator_val[0] = index;
            
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(allocator_val, table_field_info.bits_offset_in_buffer,
                                                           table_field_info.field_nof_bits,
                                                           &entry_handle->phy_entry.key[0]));
        }
        else
        {
            
            allocator_val[0] = table_field_info.max_value;
            (*is_valid_entry) = DBAL_KEY_IS_INVALID;
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(allocator_val, table_field_info.bits_offset_in_buffer,
                                                           table_field_info.field_nof_bits,
                                                           &entry_handle->phy_entry.key[0]));
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s with allocator indication doesn't have allocator field %s\n",
                     table->table_name, dbal_field_to_string(unit, table->allocator_field_id));
    }

exit:
    SHR_FUNC_EXIT;

}


shr_error_e
dbal_iterator_init_handle_info(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int ii;
    uint32 key_size = 0;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        
        entry_handle->key_field_ids[ii] = table->keys_info[ii].field_id;

        if (table->keys_info[ii].field_id == DBAL_FIELD_CORE_ID)
        {
            if ((table->core_mode == DBAL_CORE_MODE_DPC))
            {
                
                entry_handle->core_id = DBAL_MAX_NUM_OF_CORES - 1;
            }
            else
            {
                
                entry_handle->core_id = 0;
            }
        }
        else
        {
            key_size += table->keys_info[ii].field_nof_bits;
        }
    }
    entry_handle->nof_key_fields = table->nof_key_fields;
    entry_handle->phy_entry.key_size = key_size;

    if (key_size != 0 && DBAL_TABLE_IS_KEY_MASK_REQUIRED(table))
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(entry_handle->phy_entry.k_mask, 0, key_size - 1));
    }

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        int nof_result_fields = table->multi_res_info[0].nof_result_fields;
        entry_handle->cur_res_type = 0;
        entry_handle->phy_entry.payload_size = table->max_payload_size;
        entry_handle->nof_result_fields = nof_result_fields;
    }
    else
    {
        entry_handle->phy_entry.payload_size = table->max_payload_size;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_key_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 *field_value,
    uint32 *field_mask)
{
    dbal_iterator_info_t *iterator = NULL;
    dbal_entry_handle_t *entry_handle = NULL;
    uint32 field_max_size;
    dbal_field_print_type_e field_print_type;
    int action_skip = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle_id, &iterator));

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    if (field_mask != NULL)
    {
        if ((condition != DBAL_CONDITION_EQUAL_TO) && (condition != DBAL_CONDITION_NOT_EQUAL_TO))
        {
            SHR_ERR_EXIT(_SHR_E_EMPTY, "Rule with mask can be set only for EQUAL and not EQUAL rules\n");
        }
    }

    if (iterator->nof_key_rules == 0)
    {
        
        SHR_ALLOC_SET_ZERO(iterator->key_rules_info, 1 * sizeof(dbal_access_condition_info_t),
                           "First iterator Key field allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY,
                           EMPTY);
    }
    else
    {
        dbal_access_condition_info_t *temp_cond_rule_ptr = NULL;

        
        SHR_ALLOC_SET_ZERO(temp_cond_rule_ptr,
                           (iterator->nof_key_rules + 1) * sizeof(dbal_access_condition_info_t),
                           "Iterator Key field allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY,
                           EMPTY);

        sal_memcpy(temp_cond_rule_ptr, iterator->key_rules_info,
                   iterator->nof_key_rules * sizeof(dbal_access_condition_info_t));

        SHR_FREE(iterator->key_rules_info);
        iterator->key_rules_info = temp_cond_rule_ptr;
    }

    iterator->key_rules_info[iterator->nof_key_rules].field_id = field_id;
    iterator->key_rules_info[iterator->nof_key_rules].type = condition;
    if (field_id == DBAL_FIELD_EMPTY)
    {
        field_max_size = entry_handle->table->key_size;
    }
    else
    {
        int field_index_in_table;
        dbal_fields_e parent_field_id;
        dbal_table_field_info_t *field_info_p;

        SHR_IF_ERR_EXIT(dbal_tables_field_info_get_internal(unit, entry_handle->table_id, field_id, TRUE, 0, 0,
                                                            &field_info_p, &field_index_in_table, &parent_field_id));
        SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
        if (field_print_type == DBAL_FIELD_PRINT_TYPE_STR)
        {
            SHR_ERR_EXIT(_SHR_E_EMPTY, "Rule not supported for field print type string.\n");
        }
        field_max_size = field_info_p->field_nof_bits;
        iterator->key_rules_info[iterator->nof_key_rules].field_pos_in_table = field_index_in_table;
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value, 0, field_max_size,
                                                   iterator->key_rules_info[iterator->nof_key_rules].value));
    if (field_mask != NULL)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_mask, 0, field_max_size,
                                                       iterator->key_rules_info[iterator->nof_key_rules].mask));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(G_dbal_field_full_mask, 0, field_max_size,
                                                       iterator->key_rules_info[iterator->nof_key_rules].mask));

    }
    iterator->nof_key_rules++;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_common_validation(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int *action_skip)
{
    dbal_actions_e action = DBAL_NOF_ACTIONS;

    SHR_FUNC_INIT_VARS(unit);

    (*action_skip) = 0;

    SHR_IF_ERR_EXIT(dbal_action_skip_check(unit, entry_handle, &action));
    if (action == DBAL_ACTION_SKIP)
    {
        (*action_skip) = 1;
        SHR_EXIT();
    }

    if (!dbal_mngr[unit]->iterators_pool[entry_handle->handle_id].is_init)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "Trying to set a rule to an uninitialized iterator\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_value_rule_add(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    dbal_condition_types_e condition,
    uint32 *field_value,
    uint32 *field_mask)
{
    dbal_iterator_info_t *iterator_info = NULL;
    dbal_entry_handle_t *entry_handle = NULL;
    uint32 field_max_size;
    dbal_field_print_type_e field_print_type;
    int action_skip = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &(entry_handle)));

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    if (field_mask != NULL)
    {
        if ((condition != DBAL_CONDITION_EQUAL_TO) && (condition != DBAL_CONDITION_NOT_EQUAL_TO))
        {
            SHR_ERR_EXIT(_SHR_E_EMPTY, "Rule with mask can be set only for EQUAL and not EQUAL rules. Cond= %d\n",
                         condition);
        }
    }

    if (iterator_info->nof_val_rules == 0)
    {
        
        SHR_ALLOC_SET_ZERO(iterator_info->val_rules_info, 1 * sizeof(dbal_access_condition_info_t),
                           "First iterator Value field allocation", "%s%s%s\r\n", entry_handle->table->table_name,
                           EMPTY, EMPTY);
    }
    else
    {
        dbal_access_condition_info_t *temp_cond_rule_ptr = NULL;

        
        SHR_ALLOC_SET_ZERO(temp_cond_rule_ptr,
                           (iterator_info->nof_val_rules + 1) * sizeof(dbal_access_condition_info_t),
                           "Iterator Value field allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY,
                           EMPTY);

        sal_memcpy(temp_cond_rule_ptr, iterator_info->val_rules_info,
                   iterator_info->nof_val_rules * sizeof(dbal_access_condition_info_t));

        SHR_FREE(iterator_info->val_rules_info);
        iterator_info->val_rules_info = temp_cond_rule_ptr;
    }

    iterator_info->val_rules_info[iterator_info->nof_val_rules].field_id = field_id;
    iterator_info->val_rules_info[iterator_info->nof_val_rules].type = condition;
    if (field_id == DBAL_FIELD_EMPTY)
    {
        field_max_size = entry_handle->table->max_payload_size;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_max_size));
        SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
        if (field_print_type == DBAL_FIELD_PRINT_TYPE_STR)
        {
            SHR_ERR_EXIT(_SHR_E_EMPTY, "Rule not supported for field print type string.\n");
        }
    }

    SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value, 0, field_max_size,
                                                   iterator_info->val_rules_info[iterator_info->nof_val_rules].value));
    if (field_mask != NULL)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_mask, 0, field_max_size,
                                                       iterator_info->val_rules_info[iterator_info->
                                                                                     nof_val_rules].mask));
    }
    else
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(G_dbal_field_full_mask, 0, field_max_size,
                                                       iterator_info->val_rules_info[iterator_info->
                                                                                     nof_val_rules].mask));
    }

    iterator_info->nof_val_rules++;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_iterator_action_add(
    int unit,
    dbal_entry_handle_t * entry_handle,
    dbal_fields_e field_id,
    dbal_iterator_action_types_e action_type,
    uint32 *field_value,
    uint32 *field_mask)
{
    dbal_iterator_info_t *iterator_info = NULL;
    uint32 field_max_size;
    int action_skip = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_common_validation(unit, entry_handle, &action_skip));
    if (action_skip)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    if (iterator_info->nof_actions == 0)
    {
        
        SHR_ALLOC_SET_ZERO(iterator_info->actions_info, 1 * sizeof(dbal_iterator_actions_info_t),
                           "First iterator action field allocation", "%s%s%s\r\n", entry_handle->table->table_name,
                           EMPTY, EMPTY);
    }
    else
    {
        dbal_iterator_actions_info_t *temp_action_ptr = NULL;

        
        SHR_ALLOC_SET_ZERO(temp_action_ptr,
                           (iterator_info->nof_actions + 1) * sizeof(dbal_iterator_actions_info_t),
                           "Iterator Value field allocation", "%s%s%s\r\n", entry_handle->table->table_name, EMPTY,
                           EMPTY);

        sal_memcpy(temp_action_ptr, iterator_info->actions_info,
                   iterator_info->nof_actions * sizeof(dbal_iterator_actions_info_t));

        SHR_FREE(iterator_info->actions_info);
        iterator_info->actions_info = temp_action_ptr;
    }
    iterator_info->actions_info[iterator_info->nof_actions].field_id = field_id;
    iterator_info->actions_info[iterator_info->nof_actions].action_type = action_type;
    if (action_type == DBAL_ITER_ACTION_UPDATE)
    {
        if ((field_id == DBAL_FIELD_EMPTY) || (field_id == DBAL_FIELD_RESULT_TYPE))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Action update cannot be set with field EMPTY or REUSLT TYPE\n");
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_max_size));

            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_value, 0, field_max_size,
                                                           iterator_info->actions_info[iterator_info->
                                                                                       nof_actions].value));
            if (field_mask != NULL)
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(field_mask, 0, field_max_size,
                                                               iterator_info->actions_info[iterator_info->
                                                                                           nof_actions].mask));
            }
            else
            {
                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(G_dbal_field_full_mask, 0, field_max_size,
                                                               iterator_info->actions_info[iterator_info->
                                                                                           nof_actions].mask));
            }
        }
    }
    iterator_info->nof_actions++;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_entry_handle_update_field_ids(
    int unit,
    uint32 entry_handle_id)
{
    dbal_entry_handle_t *entry_handle = NULL;
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    
    SHR_IF_ERR_EXIT(dbal_entry_get_handle_update_value_field(unit, entry_handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_dynamic_updates_dispatcher(
    int unit,
    char *parent_dir,
    dbal_dynamic_xml_add_f dbal_dynamic_xml_add,
    char *top_node_name)
{
    xml_node top_node, cur_node;
    SAL_DIR *dir;
    struct sal_dirent *dirp;
    char *path;
    struct stat st;
    char str[DBAL_MAX_STRING_LENGTH];
    int is_dynamic;

    SHR_FUNC_INIT_VARS(unit);

    if ((dir = sal_opendir(parent_dir)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Dir %s not found\n", parent_dir);
    }

    while ((dirp = sal_readdir(dir)) != NULL)
    {
        uint8 is_current_image = FALSE;

        
        if ((sal_strcmp(dirp->d_name, ".") == 0) || ((sal_strcmp(dirp->d_name, "..") == 0)))
        {
            continue;
        }

        path = sal_alloc(sal_strlen(parent_dir) + sal_strlen("/") + sal_strlen(dirp->d_name) + 100, "dir path");
        sal_snprintf(path, 200, "%s/%s", parent_dir, dirp->d_name);

        if (stat(path, &st) < 0)
        {
            sal_free(path);
            SHR_ERR_EXIT(_SHR_E_RESOURCE, "Could not stat file '%s'\n", path);
        }

        
        if (S_ISDIR(st.st_mode))
        {
            SHR_IF_ERR_EXIT(dbal_dynamic_updates_dispatcher(unit, path, dbal_dynamic_xml_add, top_node_name));
            continue;
        }

        
        if (sal_strcmp(path + (sal_strlen(path) - 4), ".xml"))
        {
            continue;
        }

        top_node = dbx_xml_top_get(path, top_node_name, 0);
        if (top_node == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Cannot open xml file: %s\n", path);
        }

        
        cur_node = dbx_xml_child_get_first(top_node, "GeneralInfo");
        if (cur_node == NULL)
        {
            continue;
        }

        is_dynamic = 0;
        
        SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_int(cur_node, "dynamic_loading", &is_dynamic), _SHR_E_NOT_FOUND);
        if (!is_dynamic)
        {
            continue;
        }

        sal_memset(str, '\0', DBAL_MAX_STRING_LENGTH);
        SHR_IF_ERR_EXIT_EXCEPT_IF(dbx_xml_property_get_str(cur_node, "image", str, DBAL_MAX_STRING_LENGTH),
                                  _SHR_E_NOT_FOUND);

        
        SHR_IF_ERR_EXIT(dnx_pp_prgm_current_image_check(unit, str, &is_current_image));
        if (!is_current_image)
        {
            continue;
        }

        
        SHR_IF_ERR_EXIT(dbal_dynamic_xml_add(unit, top_node));
    }

exit:
    if (dir)
    {
        sal_closedir(dir);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_value_field32_validate(
    int unit,
    uint32 *field_val,
    dbal_table_field_info_t * table_field_info,
    uint8 *is_valid_value)
{
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { *field_val };
    uint32 encoded_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 arr_stripped_field_value_ptr[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    *is_valid_value = TRUE;

    
    if ((table_field_info->field_nof_bits > SAL_UINT32_NOF_BITS) ||
        (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ADDRESS)))
    {
        return _SHR_E_NONE;
    }

    
    if ((table_field_info->arr_prefix_size == 0) ||
        (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM)))
    {
        
        if ((field_value[0] > table_field_info->max_value) || (field_value[0] < table_field_info->min_value))
        {
            *is_valid_value = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                    "result_type_selection out-of-range for field %s(%d), value %d, min_val %d, max_val %d !\n"),
                                         dbal_field_to_string(unit, table_field_info->field_id),
                                         table_field_info->field_id, field_value[0], table_field_info->min_value,
                                         table_field_info->max_value));
            return _SHR_E_NONE;
        }
    }
    
    else
    {
        arr_stripped_field_value_ptr[0] = field_value[0];
        rv = dbal_field_arr_prefix_decode(unit, table_field_info, arr_stripped_field_value_ptr[0],
                                          arr_stripped_field_value_ptr, FALSE);
        if (SHR_FAILURE(rv))
        {
            
            *is_valid_value = FALSE;
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                    "result_type_selection ARR-Prefix out-of-range for field_id %s(%d), value %d !\n"),
                                         dbal_field_to_string(unit, table_field_info->field_id),
                                         table_field_info->field_id, arr_stripped_field_value_ptr[0]));
            return _SHR_E_NONE;
        }
    }

    
    if (SHR_BITGET(table_field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM))
    {
        
        SHR_IF_ERR_EXIT(dbal_fields_field_encode(unit, table_field_info->field_id, 0, table_field_info->field_nof_bits,
                                                 field_value, encoded_field_value));

        
        if ((table_field_info->arr_prefix_size != 0))
        {
            arr_stripped_field_value_ptr[0] = encoded_field_value[0];
            rv = dbal_field_arr_prefix_decode(unit, table_field_info, arr_stripped_field_value_ptr[0],
                                              arr_stripped_field_value_ptr, FALSE);

            if (SHR_FAILURE(rv))
            {
                
                *is_valid_value = FALSE;
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                        "result_type_selection ENUM ARR-Prefix out-of-range for field %s(%d), value %d !\n"),
                                             dbal_field_to_string(unit, table_field_info->field_id),
                                             table_field_info->field_id, arr_stripped_field_value_ptr[0]));
                return _SHR_E_NONE;
            }
        }
        
        else
        {
            uint32 enum_max_encoded_val = (table_field_info->field_nof_bits == SAL_UINT32_NOF_BITS) ?
                SAL_UINT32_MAX : SAL_UPTO_BIT(table_field_info->field_nof_bits);
            if (encoded_field_value[0] > enum_max_encoded_val)
            {
                *is_valid_value = FALSE;
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                                                        "result_type_selection ENUM out-of-range for field %s(%d), value %d,  max_val %d !\n"),
                                             dbal_field_to_string(unit, table_field_info->field_id),
                                             table_field_info->field_id, encoded_field_value[0], enum_max_encoded_val));
                return _SHR_E_NONE;
            }
        }
    }

    return _SHR_E_NONE;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
