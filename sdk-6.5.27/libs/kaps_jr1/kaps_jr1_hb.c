

#include "kaps_jr1_portable.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_hb.h"









static kaps_jr1_status
kaps_jr1_hb_init(
    struct kaps_jr1_device *device,
    struct kaps_jr1_hb_db *hb_db,
    uint32_t user_capacity)
{
    
    uint32_t capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    if (!device->hb_buffer)
    {
        device->hb_buffer = device->alloc->xmalloc(device->alloc->cookie, capacity / KAPS_JR1_BITS_IN_BYTE);
        if (!device->hb_buffer)
        {
            device->alloc->xfree(device->alloc->cookie, hb_db);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    if (!device->aging_table)
    {
        
        device->aging_table = device->alloc->xcalloc(device->alloc->cookie, capacity, sizeof(struct kaps_jr1_aging_entry));

        device->aging_table_size = capacity;
    }

    if (!device->aging_table)
    {
        device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
        device->alloc->xfree(device->alloc->cookie, hb_db);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    if (device->num_small_hb_blocks && !device->small_aging_table)
    {
        capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_small_hb_blocks * device->num_rows_in_small_hb_block; 

        device->small_aging_table = device->alloc->xcalloc(device->alloc->cookie, capacity, sizeof(struct kaps_jr1_aging_entry));

        device->small_aging_table_size = capacity;

        if (!device->small_aging_table)
        {
            device->alloc->xfree(device->alloc->cookie, device->aging_table);
            device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
            device->alloc->xfree(device->alloc->cookie, hb_db);
            return KAPS_JR1_OUT_OF_MEMORY;
        }

    }
  

    POOL_INIT(kaps_jr1_hb, &hb_db->kaps_jr1_hb_pool, device->alloc);

    KAPS_JR1_WB_HANDLE_TABLE_INIT((&hb_db->db_info), user_capacity);
    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_JR1_ISSU_RESTORE_START)
            return KAPS_JR1_OK;
    }

    kaps_jr1_db_user_handle_update_freelist(&hb_db->db_info);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_cr_init(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * cb_fun)
{
    uint32_t capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
    device->alloc->xfree(device->alloc->cookie, device->aging_table);

    if (cb_fun->nv_ptr)
    {
        device->hb_buffer = (uint8_t *) cb_fun->nv_ptr;
        cb_fun->nv_ptr = (void *) ((uint8_t *) cb_fun->nv_ptr + capacity / KAPS_JR1_BITS_IN_BYTE);
        *cb_fun->nv_offset += capacity / KAPS_JR1_BITS_IN_BYTE;
    }

    if (cb_fun->nv_ptr)
    {
        device->aging_table = (struct kaps_jr1_aging_entry *) cb_fun->nv_ptr;
        cb_fun->nv_ptr = (void *) ((uint8_t *) cb_fun->nv_ptr + (capacity * sizeof(struct kaps_jr1_aging_entry)));
        *cb_fun->nv_offset += (capacity * sizeof(struct kaps_jr1_aging_entry));
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_init(
    struct kaps_jr1_device * device,
    uint32_t id,
    uint32_t capacity,
    struct kaps_jr1_hb_db ** hb_dbp)
{
    struct kaps_jr1_hb_db *hb_db = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t is_supported;

    KAPS_JR1_TRACE_IN("%p %u %u %p\n", device, id, capacity, hb_dbp);
    if (!hb_dbp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    
    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_JR1_ISSU_INIT && device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    is_supported = 0;
    if (device->type == KAPS_JR1_DEVICE && device->num_hb_blocks > 0)
    {
        is_supported = 1;
    }

    if (!is_supported)
        return KAPS_JR1_UNSUPPORTED;

    if (capacity == 0)
        return KAPS_JR1_DYNAMIC_UNSUPPORTED;

    hb_db = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_hb_db));
    if (!hb_db)
    {
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    hb_db->capacity = capacity;
    hb_db->alloc = device->alloc;

    status = kaps_jr1_resource_add_database(device, &hb_db->db_info, id, capacity, 0, KAPS_JR1_DB_HB);

    if (status != KAPS_JR1_OK)
    {
        device->alloc->xfree(device->alloc->cookie, hb_db);
        return status;
    }

    if (device->type == KAPS_JR1_DEVICE)
    {
        KAPS_JR1_STRY(kaps_jr1_hb_init(device, hb_db, capacity));
    }

    *hb_dbp = hb_db;
    KAPS_JR1_TRACE_OUT("%p\n", *hb_dbp);

    return status;
}

kaps_jr1_status
kaps_jr1_hb_db_destroy(
    struct kaps_jr1_hb_db * hb_db)
{
    KAPS_JR1_TRACE_IN("%p\n", hb_db);
    if (!hb_db)
        return KAPS_JR1_INVALID_ARGUMENT;

    
    {
        struct kaps_jr1_device *main_device;
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *el;

        main_device = hb_db->db_info.device;
        if (main_device->main_dev)
            main_device = main_device->main_dev;
        kaps_jr1_c_list_iter_init(&main_device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (tmp == &hb_db->db_info)
            {
                kaps_jr1_c_list_remove_node(&main_device->db_list, el, &it);
                break;
            }
        }
    }

    KAPS_JR1_STRY(kaps_jr1_hb_db_delete_all_entries(hb_db));
    POOL_FINI(kaps_jr1_hb, &hb_db->kaps_jr1_hb_pool);

    KAPS_JR1_WB_HANDLE_TABLE_DESTROY((&hb_db->db_info));
    kaps_jr1_resource_free_database(hb_db->db_info.device, &hb_db->db_info);

    hb_db->alloc->xfree(hb_db->alloc->cookie, hb_db);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_add_entry(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_hb ** hb)
{
    struct kaps_jr1_hb *hb_p;

    KAPS_JR1_TRACE_IN("%p %p\n", hb_db, hb);
    if (!hb_db || !hb)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (hb_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    if (!hb_db->db_info.device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    POOL_ALLOC(kaps_jr1_hb, &hb_db->kaps_jr1_hb_pool, hb_p);
    if (!hb_p)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_memset(hb_p, 0, sizeof(*hb_p));

    kaps_jr1_c_list_add_tail(&hb_db->hb_list, &hb_p->hb_node);
    hb_p->hb_db_seq_num = hb_db->db_info.seq_num;
    *hb = hb_p;

    KAPS_JR1_WB_HANDLE_GET_FREE_AND_WRITE((&(hb_db->db_info)), (*hb));
    KAPS_JR1_TRACE_OUT("%p\n", *hb);

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_hb_db_delete_entry_internal(
    struct kaps_jr1_hb_db *hb_db,
    struct kaps_jr1_hb *hb,
    struct kaps_jr1_c_list_iter *it)
{
    struct kaps_jr1_hb *hb_p = NULL;

    if (!hb_db || !hb)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb_p), (uintptr_t) hb);

    if (!hb_p)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (hb_p->entry)
        return KAPS_JR1_INVALID_ARGUMENT;   

    if (!hb_db->db_info.device->is_config_locked)
    {
        if (hb_db->db_info.device->issu_status != KAPS_JR1_ISSU_RECONCILE_END)
            return KAPS_JR1_DEVICE_UNLOCKED;
    }

    kaps_jr1_c_list_remove_node(&hb_db->hb_list, &hb_p->hb_node, it);

    POOL_FREE(kaps_jr1_hb, &hb_db->kaps_jr1_hb_pool, hb_p);

    KAPS_JR1_WB_HANDLE_DELETE_LOC((&hb_db->db_info), (uintptr_t) hb);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_delete_entry(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_hb * hb)
{
    KAPS_JR1_TRACE_IN("%p %p\n", hb_db, hb);
    return kaps_jr1_hb_db_delete_entry_internal(hb_db, hb, NULL);
}

kaps_jr1_status
kaps_jr1_hb_db_delete_all_entries(
    struct kaps_jr1_hb_db * hb_db)
{
    KAPS_JR1_TRACE_IN("%p\n", hb_db);
    if (!hb_db)
        return KAPS_JR1_INVALID_ARGUMENT;

    do
    {
        struct kaps_jr1_list_node *el;
        struct kaps_jr1_hb *e;

        el = kaps_jr1_c_list_head(&hb_db->hb_list);
        if (el == NULL)
            break;
        e = KAPS_JR1_DBLIST_TO_HB_ENTRY(el);

        if (hb_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            KAPS_JR1_STRY(kaps_jr1_hb_db_delete_entry(hb_db, (struct kaps_jr1_hb *) (uintptr_t) e->user_handle));
        }
        else
        {
            KAPS_JR1_STRY(kaps_jr1_hb_db_delete_entry(hb_db, e));
        }
    }
    while (1);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_delete_unused_entries(
    struct kaps_jr1_hb_db * hb_db)
{
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_hb *e;
    struct kaps_jr1_c_list_iter it;

    KAPS_JR1_TRACE_IN("%p\n", hb_db);
    if (!hb_db)
        return KAPS_JR1_INVALID_ARGUMENT;

    kaps_jr1_c_list_iter_init(&hb_db->hb_list, &it);

    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        e = KAPS_JR1_DBLIST_TO_HB_ENTRY(el);

        if (!e->entry)
        {
            if (hb_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
            {
                KAPS_JR1_STRY(kaps_jr1_hb_db_delete_entry_internal(hb_db, (struct kaps_jr1_hb *) (uintptr_t) e->user_handle, &it));
            }
            else
            {
                KAPS_JR1_STRY(kaps_jr1_hb_db_delete_entry_internal(hb_db, e, &it));
            }
        }
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_hb_db_timer(
    struct kaps_jr1_hb_db * hb_db)
{
    struct kaps_jr1_db *db = hb_db->db_info.common_info->hb_info.db;
    struct kaps_jr1_device *device = hb_db->db_info.device;
    uint32_t start_hb_blk, end_hb_blk, num_hb_blks;
    uint32_t cur_blk, j, cur_row, offset, cur_hb_nr;
    uint16_t hb_word;
    uint32_t final_level_offset = kaps_jr1_device_get_final_level_offset(device, db);
    struct kaps_jr1_aging_entry *active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);
    uint32_t num_active_hb_blocks = kaps_jr1_device_get_active_num_hb_blocks(device, db);
    uint32_t num_active_rows_in_hb_block = kaps_jr1_device_get_active_num_hb_rows(device, db);

    KAPS_JR1_TRACE_IN("%p\n", hb_db);

    {
        start_hb_blk = db->common_info->start_hb_blk;
        num_hb_blks = db->common_info->num_hb_blks;
        end_hb_blk = start_hb_blk + num_hb_blks - 1;


        if (device->id != KAPS_JR1_QUMRAN_DEVICE_ID)
        {
            
            KAPS_JR1_STRY(kaps_jr1_dm_hb_dump(device, db, final_level_offset + start_hb_blk, 0,
                                           final_level_offset + end_hb_blk, num_active_rows_in_hb_block - 1, 1,
                                           device->hb_buffer));
        }

        offset = 0;

        for (cur_blk = start_hb_blk; cur_blk <= end_hb_blk; ++cur_blk)
        {

            for (cur_row = 0; cur_row < num_active_rows_in_hb_block; ++cur_row)
            {

                if (device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
                {
                    KAPS_JR1_STRY(kaps_jr1_dm_hb_read
                              (device, db, final_level_offset + cur_blk, cur_row, &device->hb_buffer[offset]));
                }

                hb_word =
                    KapsJr1ReadBitsInArrray(&device->hb_buffer[offset], KAPS_JR1_HB_ROW_WIDTH_8, KAPS_JR1_HB_ROW_WIDTH_1 - 1, 0);

                cur_hb_nr =
                    (cur_row * num_active_hb_blocks * KAPS_JR1_HB_ROW_WIDTH_1) + (cur_blk * KAPS_JR1_HB_ROW_WIDTH_1);


                for (j = 0; j < KAPS_JR1_HB_ROW_WIDTH_1; ++j)
                {
                    if (hb_word & (1u << j))
                        active_aging_table[cur_hb_nr].num_idles = 0;
                    else
                        active_aging_table[cur_hb_nr].num_idles++;

                    cur_hb_nr++;
                }

                
                offset += KAPS_JR1_HB_ROW_WIDTH_8;
            }
        }


            
    }

    if (db->fn_table->db_process_hit_bits)
        db->fn_table->db_process_hit_bits(device, db);

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_get_aged_entries(
    struct kaps_jr1_hb_db *hb_db,
    uint32_t buf_size,
    uint32_t * num_entries,
    struct kaps_jr1_entry **entries)
{
    struct kaps_jr1_db *db = hb_db->db_info.common_info->hb_info.db;
    struct kaps_jr1_device *device = hb_db->db_info.device;
    uint32_t start_hb_blk, end_hb_blk, cur_hb_blk, cur_row;
    uint32_t i, j, cur_hb_nr;
    struct kaps_jr1_aging_entry *active_aging_table = kaps_jr1_device_get_active_aging_table(device, db);
    uint32_t num_active_hb_blocks = kaps_jr1_device_get_active_num_hb_blocks(device, db);
    uint32_t num_active_rows_in_hb_block = kaps_jr1_device_get_active_num_hb_rows(device, db);

    {

        start_hb_blk = db->common_info->start_hb_blk;
        end_hb_blk = start_hb_blk + db->common_info->num_hb_blks - 1;

        i = 0;
        for (cur_row = 0; cur_row < num_active_rows_in_hb_block; ++cur_row)
        {
            cur_hb_nr = (cur_row * num_active_hb_blocks * KAPS_JR1_HB_ROW_WIDTH_1) + (start_hb_blk * KAPS_JR1_HB_ROW_WIDTH_1);

            for (cur_hb_blk = start_hb_blk; cur_hb_blk <= end_hb_blk; ++cur_hb_blk)
            {
                for (j = 0; j < KAPS_JR1_HB_ROW_WIDTH_1; ++j)
                {
                    if (active_aging_table[cur_hb_nr].entry
                        && active_aging_table[cur_hb_nr].num_idles >= hb_db->age_count)
                    {
                        if (i < buf_size)
                        {
                            entries[i] = active_aging_table[cur_hb_nr].entry;
                            ++i;
                        }
                    }
                    ++cur_hb_nr;
                }
            }

            if (i >= buf_size)
                break;
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_get_aged_entries(
    struct kaps_jr1_hb_db * hb_db,
    uint32_t buf_size,
    uint32_t * num_entries,
    struct kaps_jr1_entry ** entries)
{
    KAPS_JR1_TRACE_IN("%p %u %p %p\n", hb_db, buf_size, num_entries, entries);
    if ((hb_db == NULL) || (buf_size == 0) || (num_entries == NULL) || (entries == NULL))
        return KAPS_JR1_INVALID_ARGUMENT;

    if (hb_db->db_info.device->type == KAPS_JR1_DEVICE)
    {
        KAPS_JR1_STRY(kaps_jr1_get_aged_entries(hb_db, buf_size, num_entries, entries));
    }

    KAPS_JR1_TRACE_OUT("%u %p\n", *num_entries, *entries);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_aged_entry_iter_init(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_aged_entry_iter ** iter)
{
    struct kaps_jr1_c_list_iter *it;
    struct kaps_jr1_device *device;
    struct kaps_jr1_db *parent_db;

    KAPS_JR1_TRACE_IN("%p %p\n", hb_db, iter);
    if (!hb_db || !iter)
        return KAPS_JR1_INVALID_ARGUMENT;

    parent_db = KAPS_JR1_GET_DB_PARENT(hb_db->db_info.common_info->hb_info.db);
    device = hb_db->db_info.device;

    if (!device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
        {
            return KAPS_JR1_UNSUPPORTED;
        }
    }

    it = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(*it));
    if (!it)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_c_list_iter_init(&parent_db->db_list, it);
    *iter = (struct kaps_jr1_aged_entry_iter *) it;

    KAPS_JR1_TRACE_OUT("%p\n", *iter);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_aged_entry_iter_next(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_aged_entry_iter * iter,
    struct kaps_jr1_entry ** entry)
{
    struct kaps_jr1_device *device;
    struct kaps_jr1_db *parent_db;
    struct kaps_jr1_c_list_iter *it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_entry *actual_entry;
    struct kaps_jr1_hb *hb, *hb_entry;
    struct kaps_jr1_aging_entry *active_aging_table; 

    KAPS_JR1_TRACE_IN("%p %p %p\n", hb_db, iter, entry);
    if (!hb_db || !iter || !entry)
        return KAPS_JR1_INVALID_ARGUMENT;

    parent_db = KAPS_JR1_GET_DB_PARENT(hb_db->db_info.common_info->hb_info.db);
    device = hb_db->db_info.device;

    active_aging_table = kaps_jr1_device_get_active_aging_table(device, parent_db);

    if (!device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
        {
            return KAPS_JR1_UNSUPPORTED;
        }
    }

    it = (struct kaps_jr1_c_list_iter *) iter;

    if (it)
    {
        while ((el = kaps_jr1_c_list_iter_next(it)) != NULL)
        {
            actual_entry = KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(el);

            parent_db->fn_table->get_hb(parent_db, actual_entry, &hb);

            if (hb)
            {
                KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), &hb_entry, (uintptr_t) hb);
                if (hb_entry == NULL)
                {
                    return KAPS_JR1_INTERNAL_ERROR;
                }
                if (active_aging_table[hb_entry->bit_no].num_idles >= hb_db->age_count)
                {
                    *entry = active_aging_table[hb_entry->bit_no].entry;
                    KAPS_JR1_TRACE_OUT("%p\n", *entry);
                    return KAPS_JR1_OK;
                }
            }
        }
    }

    *entry = NULL;
    KAPS_JR1_TRACE_OUT("%p\n", *entry);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_aged_entry_iter_destroy(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_aged_entry_iter * iter)
{
    struct kaps_jr1_device *device;

    KAPS_JR1_TRACE_IN("%p %p\n", hb_db, iter);
    if (!hb_db || !iter)
        return KAPS_JR1_INVALID_ARGUMENT;

    device = hb_db->db_info.device;

    device->alloc->xfree(device->alloc->cookie, iter);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_entry_get_idle_count(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_hb * hb,
    uint32_t * idle_count)
{
    struct kaps_jr1_hb *actual_hb;
    struct kaps_jr1_db *db; 
    struct kaps_jr1_aging_entry *active_aging_table;

    KAPS_JR1_TRACE_IN("%p %p %p\n", hb_db, hb, idle_count);
    if (!hb_db || !hb || !idle_count)
        return KAPS_JR1_INVALID_ARGUMENT;

    db = hb_db->db_info.common_info->hb_info.db;
    active_aging_table = kaps_jr1_device_get_active_aging_table(db->device, db);

    *idle_count = 0;

    KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), (&actual_hb), (uintptr_t) hb);
    if (actual_hb == NULL)
    {
        return KAPS_JR1_INTERNAL_ERROR;
    }
    *idle_count = active_aging_table[actual_hb->bit_no].num_idles;
    KAPS_JR1_TRACE_OUT("%u\n", *idle_count);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_entry_set_idle_count(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_hb * hb,
    uint32_t idle_count)
{
    struct kaps_jr1_hb *actual_hb;
    struct kaps_jr1_db *db; 
    struct kaps_jr1_aging_entry *active_aging_table;

    KAPS_JR1_TRACE_IN("%p %p %u\n", hb_db, hb, idle_count);
    if (!hb_db || !hb)
        return KAPS_JR1_INVALID_ARGUMENT;

    db = hb_db->db_info.common_info->hb_info.db;
    active_aging_table = kaps_jr1_device_get_active_aging_table(db->device, db);

    KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), (&actual_hb), (uintptr_t) hb);
    if (actual_hb == NULL)
    {
        return KAPS_JR1_INTERNAL_ERROR;
    }
    active_aging_table[actual_hb->bit_no].num_idles = idle_count;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_entry_get_bit_value(
    struct kaps_jr1_hb_db * hb_db,
    struct kaps_jr1_hb * hb_handle,
    uint32_t * bit_value,
    uint8_t clear_on_read)
{
    struct kaps_jr1_hb *hb = NULL;
    struct kaps_jr1_device *device = NULL;
    struct kaps_jr1_db *db = NULL;
    uint32_t final_level_offset;
    uint32_t num_active_hb_blocks;

    KAPS_JR1_TRACE_IN("%p %p %p %u\n", hb_db, hb_handle, bit_value, clear_on_read);
    if (!hb_db || !hb_handle || !bit_value)
        return KAPS_JR1_INVALID_ARGUMENT;

    device = hb_db->db_info.device;
    db = hb_db->db_info.common_info->hb_info.db;

    final_level_offset = kaps_jr1_device_get_final_level_offset(device, db);
    num_active_hb_blocks = kaps_jr1_device_get_active_num_hb_blocks(device, db);

    KAPS_JR1_WB_HANDLE_READ_LOC((&hb_db->db_info), (&hb), (uintptr_t) hb_handle);
    if (hb == NULL)
    {
        return KAPS_JR1_INTERNAL_ERROR;
    }

    
    {
        uint8_t kaps_jr1_hb_data[2 * KAPS_JR1_HB_ROW_WIDTH_8];
        uint32_t block_num, row_num, bit_pos;
        uint32_t buf_len_8;
       

        kaps_jr1_memset(kaps_jr1_hb_data, 0, 2 * KAPS_JR1_HB_ROW_WIDTH_8);

        block_num = hb->bit_no / KAPS_JR1_HB_ROW_WIDTH_1;
        block_num = block_num % num_active_hb_blocks;

        row_num = hb->bit_no / (KAPS_JR1_HB_ROW_WIDTH_1 * num_active_hb_blocks);

        KAPS_JR1_STRY(kaps_jr1_dm_hb_read(device, db, final_level_offset + block_num, row_num, kaps_jr1_hb_data));

        buf_len_8 = KAPS_JR1_HB_ROW_WIDTH_8;
        bit_pos = hb->bit_no % KAPS_JR1_HB_ROW_WIDTH_1;

        *bit_value = KapsJr1ReadBitsInArrray(kaps_jr1_hb_data, buf_len_8, bit_pos, bit_pos);


        if (clear_on_read)
        {
            if (*bit_value)
            {
                KapsJr1WriteBitsInArray(kaps_jr1_hb_data, buf_len_8, bit_pos, bit_pos, 0);

                kaps_jr1_dm_hb_write(device, db, final_level_offset + block_num, row_num, kaps_jr1_hb_data);
            }
        }

        
        if (*bit_value == 0 || clear_on_read)
        {
            if (db->fn_table->db_get_algo_hit_bit_value && hb->entry)
                db->fn_table->db_get_algo_hit_bit_value(device, db, hb->entry, clear_on_read, bit_value);
        }
    }

    KAPS_JR1_TRACE_OUT("%u\n", *bit_value);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_set_property(
    struct kaps_jr1_hb_db * hb_db,
    enum kaps_jr1_db_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;

    if (hb_db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (hb_db->db_info.device->is_config_locked)
    {
        if (property != KAPS_JR1_PROP_AGE_COUNT)
            return KAPS_JR1_DB_ACTIVE;
    }

    if (hb_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    va_start(vl, property);
    switch (property)
    {
        case KAPS_JR1_PROP_DESCRIPTION:
        {
            char *desc = va_arg(vl, char *);
            KAPS_JR1_TRACE_IN("%p %d %s\n", hb_db, property, desc);
            if (!desc)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            hb_db->db_info.description
                = hb_db->db_info.device->alloc->xcalloc(hb_db->db_info.device->alloc->cookie, 1, (strlen(desc) + 1));
            if (!hb_db->db_info.description)
            {
                status = KAPS_JR1_OUT_OF_MEMORY;
                break;
            }
            strcpy(hb_db->db_info.description, desc);
            break;
        }
        case KAPS_JR1_PROP_AGE_COUNT:
        {
            uint32_t age_count = va_arg(vl, uint32_t);

            KAPS_JR1_TRACE_IN("%p %d %u\n", hb_db, property, age_count);
            if (age_count == 0)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            hb_db->age_count = age_count;
            break;
        }
        default:
            KAPS_JR1_TRACE_IN("%p %d\n", hb_db, property);
            status = KAPS_JR1_INVALID_ARGUMENT;
            break;
    }
    va_end(vl);
    return status;
}

kaps_jr1_status
kaps_jr1_hb_db_get_property(
    struct kaps_jr1_hb_db * hb_db,
    enum kaps_jr1_db_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %d\n", hb_db, property);
    if (hb_db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (hb_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    va_start(vl, property);
    switch (property)
    {
        case KAPS_JR1_PROP_DESCRIPTION:
        {
            char **desc = va_arg(vl, char **);
            if (!desc)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            *desc = hb_db->db_info.description;
            break;
        }
        case KAPS_JR1_PROP_AGE_COUNT:
        {
            uint32_t *age_count = va_arg(vl, uint32_t *);
            if (!age_count)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            *age_count = hb_db->age_count;
            break;
        }
        default:
            status = KAPS_JR1_INVALID_ARGUMENT;
            break;
    }
    va_end(vl);
    return status;
}

kaps_jr1_status
kaps_jr1_hb_wb_save_state(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    uint32_t capacity;

    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!wb_fun)
        return KAPS_JR1_INVALID_ARGUMENT;

    capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    if (!device->hb_buffer || !device->aging_table)
        return KAPS_JR1_OK;

    if (0 !=
        wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->hb_buffer, capacity / KAPS_JR1_BITS_IN_BYTE,
                         *wb_fun->nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *wb_fun->nv_offset += capacity / KAPS_JR1_BITS_IN_BYTE;

    if (0 !=
        wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->aging_table, (capacity * sizeof(struct kaps_jr1_aging_entry)),
                         *wb_fun->nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *wb_fun->nv_offset += (capacity * sizeof(struct kaps_jr1_aging_entry));


    if (device->num_small_hb_blocks)
    {
        capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_small_hb_blocks * device->num_rows_in_small_hb_block;
        if (0 !=
        wb_fun->write_fn(wb_fun->handle, (uint8_t *) device->small_aging_table, (capacity * sizeof(struct kaps_jr1_aging_entry)),
                         *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
        
        *wb_fun->nv_offset += (capacity * sizeof(struct kaps_jr1_aging_entry));
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_wb_restore_state(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    uint32_t capacity;

    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!wb_fun)
        return KAPS_JR1_INVALID_ARGUMENT;

    capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_hb_blocks * device->num_rows_in_hb_block;

    if (!device->hb_buffer || !device->aging_table)
        return KAPS_JR1_OK;

    if (0 !=
        wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->hb_buffer, capacity / KAPS_JR1_BITS_IN_BYTE,
                        *wb_fun->nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *wb_fun->nv_offset += capacity / KAPS_JR1_BITS_IN_BYTE;

    if (0 !=
        wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->aging_table, (capacity * sizeof(struct kaps_jr1_aging_entry)),
                        *wb_fun->nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    
    *wb_fun->nv_offset += (capacity * sizeof(struct kaps_jr1_aging_entry));

    if (device->num_small_hb_blocks)
    {
        capacity = KAPS_JR1_HB_ROW_WIDTH_1 * device->num_small_hb_blocks * device->num_rows_in_small_hb_block;

        if (0 !=
            wb_fun->read_fn(wb_fun->handle, (uint8_t *) device->small_aging_table, (capacity * sizeof(struct kaps_jr1_aging_entry)),
                            *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
        
        *wb_fun->nv_offset += (capacity * sizeof(struct kaps_jr1_aging_entry));    
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_wb_add_entry(
    struct kaps_jr1_hb_db * hb_db,
    uint32_t bit_no,
    uintptr_t user_handle)
{
    struct kaps_jr1_hb *hb_p;

    if (!hb_db)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (hb_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (hb_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT &&
            hb_db->db_info.device->issu_status != KAPS_JR1_ISSU_RESTORE_END)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    if (!hb_db->db_info.device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    POOL_ALLOC(kaps_jr1_hb, &hb_db->kaps_jr1_hb_pool, hb_p);
    if (!hb_p)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_memset(hb_p, 0, sizeof(*hb_p));

    kaps_jr1_c_list_add_tail(&hb_db->hb_list, &hb_p->hb_node);
    hb_p->hb_db_seq_num = hb_db->db_info.seq_num;
    hb_p->bit_no = bit_no;

    KAPS_JR1_WB_HANDLE_WRITE_LOC(((struct kaps_jr1_db *) hb_db), hb_p, user_handle);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_hb_db_refresh_handle(
    struct kaps_jr1_device * device,
    struct kaps_jr1_hb_db * stale_ptr,
    struct kaps_jr1_hb_db ** dbp)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;
    struct kaps_jr1_hb_db *db;

    KAPS_JR1_TRACE_IN("%p %p %p\n", device, stale_ptr, dbp);
    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (device->main_dev)
        device = device->main_dev;

    if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
        return KAPS_JR1_INVALID_FLAGS;

    if (device->issu_status == KAPS_JR1_ISSU_INIT)
        return KAPS_JR1_INVALID_ARGUMENT;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

        if (tmp->type == KAPS_JR1_DB_HB)
        {
            db = (struct kaps_jr1_hb_db *) tmp;
            if (db->db_info.stale_ptr == (uintptr_t) stale_ptr)
            {
                *dbp = db;
                KAPS_JR1_TRACE_OUT("%p\n", *dbp);
                return KAPS_JR1_OK;
            }
        }
    }

    if (device->other_core)
    {
        kaps_jr1_c_list_iter_init(&device->other_core->db_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

            if (tmp->type == KAPS_JR1_DB_HB)
            {
                db = (struct kaps_jr1_hb_db *) tmp;
                if (db->db_info.stale_ptr == (uintptr_t) stale_ptr)
                {
                    *dbp = db;
                    KAPS_JR1_TRACE_OUT("%p\n", *dbp);
                    return KAPS_JR1_OK;
                }
            }
        }
    }

    KAPS_JR1_TRACE_OUT("%p\n", 0);
    return KAPS_JR1_INVALID_ARGUMENT;
}
