

#include "kaps_jr1_handle.h"
#include "kaps_jr1_cr_pool_mgr.h"
#include "kaps_jr1_key_internal.h"

kaps_jr1_status
kaps_jr1_db_refresh_handle(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *stale_ptr,
    struct kaps_jr1_db **dbp)
{
    struct kaps_jr1_device *main_bc_device;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;

    KAPS_JR1_TRACE_IN("%p %p %p\n", device, stale_ptr, dbp);
    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    main_bc_device = kaps_jr1_get_main_bc_device(device);

    if (main_bc_device->main_dev)
        main_bc_device = main_bc_device->main_dev;

    for (device = main_bc_device; device; device = device->next_bc_device)
    {

        if (!stale_ptr || !dbp)
            return KAPS_JR1_INVALID_ARGUMENT;

        if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
            return KAPS_JR1_INVALID_FLAGS;

        if (device->issu_status == KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_INVALID_ARGUMENT;

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *tab = NULL;
            struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

            if (tmp->stale_ptr == (uintptr_t) stale_ptr)
            {
                *dbp = tmp;
                return KAPS_JR1_OK;
            }
            if (tmp->has_tables || tmp->has_clones)
            {
                for (tab = tmp->next_tab; tab; tab = tab->next_tab)
                {
                    if (tab->stale_ptr == (uintptr_t) stale_ptr)
                    {
                        *dbp = tab;
                        KAPS_JR1_TRACE_OUT("%p\n", *dbp);
                        return KAPS_JR1_OK;
                    }
                }
            }
        }
    }

    KAPS_JR1_TRACE_OUT("%p\n", 0);
    return KAPS_JR1_INVALID_ARGUMENT;
}

static kaps_jr1_status
kaps_jr1_db_wb_init(
    struct kaps_jr1_device *device,
    enum kaps_jr1_db_type type,
    uint32_t id,
    uint32_t capacity,
    uintptr_t stale_ptr,
    struct kaps_jr1_db **dbp,
    int32_t is_broadcast)
{
    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!stale_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    
    

    switch (type)
    {
        case KAPS_JR1_DB_LPM:
            KAPS_JR1_STRY(kaps_jr1_lpm_db_init(device, id, capacity, dbp));
            break;
        default:
            return KAPS_JR1_INVALID_DB_TYPE;
            break;
    }
    (*dbp)->stale_ptr = stale_ptr;


    if (!is_broadcast)
    {
        KAPS_JR1_WB_HANDLE_TABLE_INIT((*dbp), capacity);
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_db_wb_add_table(
    struct kaps_jr1_db *db,
    uint32_t id,
    uintptr_t stale_ptr,
    struct kaps_jr1_db **table)
{
    if (db == NULL || stale_ptr == 0)
        return KAPS_JR1_INVALID_ARGUMENT;

    

    if (db->parent)
        db = db->parent;

    if (db->device->is_config_locked)
        return KAPS_JR1_DB_ACTIVE;

    if (!db->fn_table || !db->fn_table->db_add_table)
        return KAPS_JR1_INTERNAL_ERROR;

    KAPS_JR1_STRY(db->fn_table->db_add_table(db, id, 0, table));
    (*table)->stale_ptr = stale_ptr;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_entry_set_used(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry)
{
    struct kaps_jr1_entry *entry_p;

    KAPS_JR1_TRACE_IN("%p %p\n", db, entry);
    if (!db || !entry)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (db->device->issu_status != KAPS_JR1_ISSU_RECONCILE_START)
        return KAPS_JR1_RECONCILE_NOT_STARTED;

    KAPS_JR1_WB_HANDLE_READ_LOC(db, (&entry_p), (uintptr_t) entry);

    return db->fn_table->set_used(entry_p);
}

static kaps_jr1_status
kaps_jr1_db_wb_clone(
    struct kaps_jr1_db *db,
    uint32_t id,
    uintptr_t stale_ptr,
    struct kaps_jr1_db **clone)
{
    if (!db || !stale_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (db->device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (!db->fn_table || !db->fn_table->db_add_table)
        return KAPS_JR1_INTERNAL_ERROR;

    KAPS_JR1_STRY(db->fn_table->db_add_table(db, id, 1, clone));
    (*clone)->stale_ptr = stale_ptr;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_wb_save_info(
    struct kaps_jr1_db * db,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_db_wb_info *wb_info_ptr, *tmp = NULL;
    struct kaps_jr1_db *tmp_db;
    struct kaps_jr1_ad_db *ad_db;
    int32_t total_size;
    uint32_t i;

    total_size = sizeof(*wb_info_ptr);
    if (db->description)
        total_size += strlen(db->description) + 1;

    if (wb_fun->nv_ptr == NULL)
    {
        wb_info_ptr = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, total_size);
        if (wb_info_ptr == NULL)
            return KAPS_JR1_OUT_OF_MEMORY;
    }
    else
    {
        tmp = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, total_size);
        if (tmp == NULL)
            return KAPS_JR1_OUT_OF_MEMORY;
        wb_info_ptr = tmp;
    }

    

    ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;
    while (ad_db)
    {
        if (!ad_db->db_info.is_internal)
            wb_info_ptr->num_ad_databases++;
        ad_db = ad_db->next;
    }
    if (db->common_info->hb_info.hb)
    {
        struct kaps_jr1_hb_db *hb_db = (struct kaps_jr1_hb_db *) db->common_info->hb_info.hb;

        wb_info_ptr->has_hb = 1;
        wb_info_ptr->hb_id = hb_db->db_info.tid;
        wb_info_ptr->hb_capacity = hb_db->capacity;
        wb_info_ptr->hb_age_count = hb_db->age_count;
        wb_info_ptr->hb_stale_ptr = (uintptr_t) hb_db;
    }

    

    wb_info_ptr->tid = db->tid;
    wb_info_ptr->capacity = db->common_info->capacity;
    wb_info_ptr->type = db->type;
    wb_info_ptr->smt_no = db->device->smt_no;
    wb_info_ptr->enable_dynamic_allocation = db->common_info->enable_dynamic_allocation;
    wb_info_ptr->calc_shuffle_stats = db->common_info->calc_shuffle_stats;
    wb_info_ptr->has_dup_ad = db->has_dup_ad;

    wb_info_ptr->is_main_bc_db = db->is_main_bc_db;
    wb_info_ptr->is_bc_required = db->is_bc_required;
    wb_info_ptr->bc_bitmap = db->bc_bitmap;
    wb_info_ptr->mapped_to_acl = db->common_info->mapped_to_acl;
    wb_info_ptr->is_em = db->common_info->is_em;

    wb_info_ptr->is_algo = db->common_info->is_algo;
    wb_info_ptr->has_dup_ad = db->has_dup_ad;
    wb_info_ptr->defer_deletes = db->common_info->defer_deletes_to_install;
    wb_info_ptr->meta_priority = db->common_info->meta_priority;

    wb_info_ptr->is_algorithmic = kaps_jr1_db_get_algorithmic(db);
    wb_info_ptr->is_user_specified_algo_type = kaps_jr1_resource_get_is_user_specified_algo_mode(db);

    wb_info_ptr->insertion_mode = db->common_info->insertion_mode;

    wb_info_ptr->is_cascaded = db->common_info->is_cascaded;

    wb_info_ptr->alg_type = 0;
    wb_info_ptr->num_algo_levels_in_db = db->num_algo_levels_in_db;

    wb_info_ptr->stale_ptr = (uintptr_t) db;

    wb_info_ptr->desc = 0;
    wb_info_ptr->is_clone = 0;
    wb_info_ptr->is_table = 0;
    wb_info_ptr->clone_of = 0;

    if (db->description)
    {
        wb_info_ptr->desc = (uint16_t)(strlen(db->description) + 1);
        
        kaps_jr1_memcpy(wb_info_ptr->description, db->description, wb_info_ptr->desc);
    }

    

    wb_info_ptr->has_tables = db->has_tables;
    wb_info_ptr->has_clones = db->has_clones;
    if (db->is_clone)
    {
        wb_info_ptr->clone_of = (uintptr_t) db->clone_of;
        wb_info_ptr->is_clone = db->is_clone;
    }

    if (!db->is_clone && db->parent)
        wb_info_ptr->is_table = 1;

    

    wb_info_ptr->user_specified = db->common_info->user_specified;

    wb_info_ptr->callback_data = db->common_info->callback_fn || db->common_info->callback_handle;

    if (wb_info_ptr->calc_shuffle_stats)
    {
        wb_info_ptr->pio_stats.num_blk_ops = db->common_info->pio_stats.num_blk_ops;
        wb_info_ptr->pio_stats.num_ix_cbs = db->common_info->pio_stats.num_ix_cbs;
        wb_info_ptr->pio_stats.num_of_piords = db->common_info->pio_stats.num_of_piords;
        wb_info_ptr->pio_stats.num_of_piowrs = db->common_info->pio_stats.num_of_piowrs;
        wb_info_ptr->pio_stats.num_of_worst_case_shuffles = db->common_info->pio_stats.num_of_worst_case_shuffles;
        wb_info_ptr->pio_stats.num_of_shuffles = db->common_info->pio_stats.num_of_shuffles;
        wb_info_ptr->pio_stats.num_ad_writes = db->common_info->pio_stats.num_ad_writes;
    }

    if (wb_info_ptr->user_specified)
    {
        wb_info_ptr->user_num_ab = db->common_info->user_num_ab;
        wb_info_ptr->user_uda_mb = db->common_info->user_uda_mb;
    }

    if (wb_info_ptr->callback_data)
    {
        wb_info_ptr->callback_fn = db->common_info->callback_fn;
        wb_info_ptr->callback_handle = db->common_info->callback_handle;
    }

    if (db->is_clone || db->parent)
    {
        if (db->next_tab)
            KAPS_JR1_STRY(kaps_jr1_db_wb_save_info(db->next_tab, wb_fun));
    }

    
    if (wb_fun->write_fn)
    {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }
    *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
    if (wb_fun->nv_ptr)
    {
        wb_info_ptr = (struct kaps_jr1_db_wb_info *) wb_fun->nv_ptr;
        kaps_jr1_memcpy(wb_info_ptr, tmp, total_size);
        wb_fun->nv_ptr += total_size;
    }

    if (!db->is_clone && !db->parent)
    {
        
        struct kaps_jr1_ad_db_wb_info ad_wb_info, *ad_wb_info_ptr;

        ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;
        while (ad_db)
        {
            if (ad_db->db_info.is_internal)
            {
                ad_db = ad_db->next;
                continue;
            }
            if (wb_fun->nv_ptr == NULL)
            {
                ad_wb_info_ptr = &ad_wb_info;
            }
            else
            {
                ad_wb_info_ptr = (struct kaps_jr1_ad_db_wb_info *) wb_fun->nv_ptr;
            }

            kaps_jr1_memset(&ad_wb_info, 0, sizeof(struct kaps_jr1_ad_db_wb_info));
            ad_wb_info_ptr->id = ad_db->db_info.tid;
            ad_wb_info_ptr->ad_width = ad_db->user_width_1;
            ad_wb_info_ptr->capacity = ad_db->db_info.common_info->capacity;
            ad_wb_info_ptr->stale_ptr = (uintptr_t) ad_db;
            ad_wb_info_ptr->hb_ad_stale_ptr = (uintptr_t) ad_db->hb_ad;
            ad_wb_info_ptr->user_specified = ad_db->db_info.common_info->user_specified;
            ad_wb_info_ptr->uda_mb = ad_db->db_info.common_info->uda_mb;

            for (i = 0; i < KAPS_JR1_MAX_NUM_CORES; ++i)
            {
                ad_wb_info_ptr->num_entries_in_ad_db[i] = ad_db->num_entries_in_ad_db[i];
            }

            
            if (wb_info_ptr->enable_dynamic_allocation)
            {
                kaps_jr1_memcpy(&ad_wb_info_ptr->ad_hw_res, ad_db->db_info.hw_res.ad_res,
                            sizeof(struct kaps_jr1_ad_db_hw_resource));

                if (wb_fun->nv_ptr)
                {
                    if (!(db->device->flags & KAPS_JR1_DEVICE_SKIP_INIT))
                    {
                        db->device->alloc->xfree(db->device->alloc->cookie, ad_db->db_info.hw_res.ad_res);
                    }
                    ad_db->db_info.hw_res.ad_res = (struct kaps_jr1_ad_db_hw_resource *) &ad_wb_info_ptr->ad_hw_res;
                }
            }

            if (wb_fun->write_fn)
            {
                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) ad_wb_info_ptr,
                                          sizeof(*ad_wb_info_ptr), *wb_fun->nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
            }

            *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*ad_wb_info_ptr);
            if (wb_fun->nv_ptr)
                wb_fun->nv_ptr += sizeof(struct kaps_jr1_ad_db_wb_info);

            ad_db = ad_db->next;
        }
    }

    

    KAPS_JR1_STRY(kaps_jr1_key_wb_save(db->key, wb_fun));

    if (db->is_clone || db->parent)
    {
        if (wb_fun->nv_ptr == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
        }
        return KAPS_JR1_OK;
    }
    

    if (wb_info_ptr->has_tables || wb_info_ptr->has_clones)
    {
        uint32_t num_tables = 0;

        for (tmp_db = db->next_tab; tmp_db; tmp_db = tmp_db->next_tab)
        {
            num_tables++;
        }
        if (wb_fun->write_fn)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & num_tables, sizeof(num_tables), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        else
        {
            *((uint32_t *) wb_fun->nv_ptr) = num_tables;
        }

        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_tables);
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += sizeof(num_tables);

        if (db->next_tab)
            KAPS_JR1_STRY(kaps_jr1_db_wb_save_info(db->next_tab, wb_fun));
    }

    if (wb_fun->nv_ptr == NULL)
    {
        db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
    }
    else
    {
        db->device->alloc->xfree(db->device->alloc->cookie, tmp);
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_wb_restore_info(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db_p,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_db_wb_info *wb_info_ptr;
    struct kaps_jr1_db *db, *tmp;
    struct kaps_jr1_ad_db *ad_db;
    struct kaps_jr1_key *key;
    uint32_t tbl_no, num_tables = 0;
    struct kaps_jr1_device *target_device;
    int32_t total_size;
    struct kaps_jr1_hb_bank_list *head_hb_bank;
    uint32_t i;

    if (wb_fun->nv_ptr == NULL)
    {
        struct kaps_jr1_db_wb_info wb_info;

        wb_info_ptr = &wb_info;
        
        kaps_jr1_sassert(wb_fun->read_fn);
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, sizeof(*wb_info_ptr), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
        total_size = sizeof(*wb_info_ptr) + wb_info_ptr->desc;
        wb_info_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, total_size);
        if (wb_info_ptr == NULL)
            return KAPS_JR1_OUT_OF_MEMORY;
    }
    else
    {
        wb_info_ptr = (struct kaps_jr1_db_wb_info *) wb_fun->nv_ptr;
        total_size = sizeof(*wb_info_ptr) + wb_info_ptr->desc;
    }

    if (wb_fun->read_fn != NULL)
    {
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) wb_info_ptr, total_size, *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + total_size;
    if (wb_fun->nv_ptr != NULL)
        wb_fun->nv_ptr += total_size;

    if (wb_info_ptr->smt_no == device->smt_no)
        target_device = device;
    else
        target_device = device->smt;

    if (wb_info_ptr->is_table)
        KAPS_JR1_STRY(kaps_jr1_db_wb_add_table(db_p, wb_info_ptr->tid, wb_info_ptr->stale_ptr, &db));
    else if (wb_info_ptr->is_clone)
    {
        KAPS_JR1_TRY(kaps_jr1_db_refresh_handle(device, (struct kaps_jr1_db *) wb_info_ptr->clone_of, &tmp));
        KAPS_JR1_STRY(kaps_jr1_db_wb_clone(tmp, wb_info_ptr->tid, wb_info_ptr->stale_ptr, &db));
    }
    else
        KAPS_JR1_STRY(kaps_jr1_db_wb_init(target_device, wb_info_ptr->type,
                                  wb_info_ptr->tid,
                                  wb_info_ptr->capacity,
                                  wb_info_ptr->stale_ptr, &db,
                                  wb_info_ptr->is_bc_required && (!wb_info_ptr->is_main_bc_db)));

    db->is_main_bc_db = wb_info_ptr->is_main_bc_db;
    db->is_bc_required = wb_info_ptr->is_bc_required;
    db->bc_bitmap = wb_info_ptr->bc_bitmap;
    db->common_info->mapped_to_acl = wb_info_ptr->mapped_to_acl;
    db->common_info->is_em = wb_info_ptr->is_em;
    db->common_info->is_cascaded = wb_info_ptr->is_cascaded;
    db->num_algo_levels_in_db = wb_info_ptr->num_algo_levels_in_db;

    if (wb_info_ptr->num_ad_databases && !wb_info_ptr->is_table && !wb_info_ptr->is_clone)
    {
        uint32_t ad_iter;
        struct kaps_jr1_ad_db **ad_db_array = NULL;
        struct kaps_jr1_hb_bank_list **hb_head_bank_array = NULL;

        
        ad_db_array = device->alloc->xcalloc(device->alloc->cookie,
                                             wb_info_ptr->num_ad_databases, sizeof(struct kaps_jr1_ad_db *));
        if (!ad_db_array)
            return KAPS_JR1_OUT_OF_MEMORY;

        
        hb_head_bank_array = device->alloc->xcalloc(device->alloc->cookie,
                                                    wb_info_ptr->num_ad_databases, sizeof(struct kaps_jr1_hb_bank_list *));

        if (!hb_head_bank_array)
            return KAPS_JR1_OUT_OF_MEMORY;

        ad_iter = 0;
        while (ad_iter < wb_info_ptr->num_ad_databases)
        {
            struct kaps_jr1_ad_db_wb_info ad_wb_info, *ad_wb_info_ptr;

            if (wb_fun->nv_ptr == NULL)
            {
                ad_wb_info_ptr = &ad_wb_info;
            }
            else
            {
                ad_wb_info_ptr = (struct kaps_jr1_ad_db_wb_info *) wb_fun->nv_ptr;
            }

            if (wb_fun->read_fn != NULL)
            {
                if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) ad_wb_info_ptr,
                                         sizeof(*ad_wb_info_ptr), *wb_fun->nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*ad_wb_info_ptr);
            if (wb_fun->nv_ptr != NULL)
                wb_fun->nv_ptr += sizeof(*ad_wb_info_ptr);

            KAPS_JR1_STRY(kaps_jr1_ad_db_init(target_device, ad_wb_info_ptr->id,
                                      ad_wb_info_ptr->capacity, ad_wb_info_ptr->ad_width, &ad_db));

            ad_db->db_info.stale_ptr = (uintptr_t) ad_wb_info_ptr->stale_ptr;
            ad_db->db_info.hb_ad_stale_ptr = (uintptr_t) ad_wb_info_ptr->hb_ad_stale_ptr;

            if (ad_wb_info_ptr->user_specified)
                KAPS_JR1_STRY(kaps_jr1_db_set_resource(&ad_db->db_info, KAPS_JR1_HW_RESOURCE_UDA, ad_wb_info_ptr->uda_mb));

            for (i = 0; i < KAPS_JR1_MAX_NUM_CORES; ++i)
            {
                ad_db->num_entries_in_ad_db[i] = ad_wb_info_ptr->num_entries_in_ad_db[i];
            }


            if (wb_info_ptr->enable_dynamic_allocation)
            {
                kaps_jr1_memcpy(ad_db->db_info.hw_res.ad_res, &ad_wb_info_ptr->ad_hw_res,
                            sizeof(struct kaps_jr1_ad_db_hw_resource));
            }
            KAPS_JR1_STRY(kaps_jr1_db_set_ad(db, ad_db));

            head_hb_bank = NULL;

            
            ad_db_array[ad_iter] = ad_db;
            hb_head_bank_array[ad_iter] = head_hb_bank;

            ad_iter++;
        }

        if (wb_info_ptr->has_hb)
        {
            struct kaps_jr1_hb_db *hb_db;

            KAPS_JR1_STRY(kaps_jr1_hb_db_init(target_device, wb_info_ptr->hb_id, wb_info_ptr->hb_capacity, &hb_db));
            hb_db->db_info.stale_ptr = (uintptr_t) wb_info_ptr->hb_stale_ptr;
            KAPS_JR1_STRY(kaps_jr1_db_set_hb(db, hb_db));
            hb_db->age_count = wb_info_ptr->hb_age_count;
        }

        if (ad_db_array)
            device->alloc->xfree(device->alloc->cookie, ad_db_array);

        if (hb_head_bank_array)
            device->alloc->xfree(device->alloc->cookie, hb_head_bank_array);
    }

    if (wb_info_ptr->desc)
    {
        KAPS_JR1_STRY(kaps_jr1_db_set_property(db, KAPS_JR1_PROP_DESCRIPTION, wb_info_ptr->description));
    }

    if (wb_info_ptr->defer_deletes)
    {
        KAPS_JR1_STRY(kaps_jr1_db_set_property(db, KAPS_JR1_PROP_DEFER_DELETES, wb_info_ptr->defer_deletes));
    }

    if (db->is_bc_required)
    {
        if (db->is_main_bc_db)
            db->main_bc_db = db;
        else
        {
            struct kaps_jr1_device *tmpdev;
            struct kaps_jr1_db *tmp_db;

            tmpdev = db->device->main_bc_device;
            while (tmpdev)
            {
                tmp_db = kaps_jr1_db_get_bc_db_on_device(tmpdev, db);
                tmpdev = tmpdev->next_bc_device;
                db->main_bc_db = tmp_db;
                if (tmp_db)
                    break;
            }
        }
    }

    KAPS_JR1_STRY(kaps_jr1_db_set_property(db, KAPS_JR1_PROP_ALGORITHMIC, wb_info_ptr->is_algorithmic));

    if (wb_info_ptr->is_user_specified_algo_type)
        kaps_jr1_resource_set_user_specified_algo_mode(db);


    KAPS_JR1_STRY(kaps_jr1_key_init(target_device, &key));
    KAPS_JR1_STRY(kaps_jr1_key_wb_restore(key, wb_fun));
    KAPS_JR1_STRY(kaps_jr1_db_set_key(db, key));

    if (wb_info_ptr->is_table || wb_info_ptr->is_clone)
    {
        if (wb_fun->nv_ptr == NULL)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
        }
        return KAPS_JR1_OK;
    }

    KAPS_JR1_STRY(kaps_jr1_db_set_property(db, KAPS_JR1_PROP_ALGORITHMIC, wb_info_ptr->is_algorithmic));


    if (wb_info_ptr->has_tables || wb_info_ptr->has_clones)
    {
        if (wb_fun->read_fn)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & num_tables, sizeof(num_tables), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        else
        {
            kaps_jr1_sassert(wb_fun->nv_ptr != NULL);
            num_tables = *((uint32_t *) wb_fun->nv_ptr);
        }

        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_tables);
        if (wb_fun->nv_ptr)
            wb_fun->nv_ptr += sizeof(num_tables);

        for (tbl_no = 0; tbl_no < num_tables; tbl_no++)
        {
            KAPS_JR1_STRY(kaps_jr1_db_wb_restore_info(target_device, db, wb_fun));
        }
    }

    if (wb_info_ptr->user_specified)
    {
        KAPS_JR1_TRY(kaps_jr1_db_set_resource(db, KAPS_JR1_HW_RESOURCE_DBA, wb_info_ptr->user_num_ab));
        KAPS_JR1_TRY(kaps_jr1_db_set_resource(db, KAPS_JR1_HW_RESOURCE_UDA, wb_info_ptr->user_uda_mb));
    }

    if (wb_info_ptr->callback_data)
    {
        KAPS_JR1_TRY(kaps_jr1_db_set_property
                 (db, KAPS_JR1_PROP_INDEX_CALLBACK, wb_info_ptr->callback_fn, wb_info_ptr->callback_handle));
    }

    if (wb_info_ptr->enable_dynamic_allocation)
    {
        db->common_info->enable_dynamic_allocation = 1;
    }

    db->common_info->is_algo = wb_info_ptr->is_algo;
    db->common_info->calc_shuffle_stats = wb_info_ptr->calc_shuffle_stats;
    db->common_info->meta_priority = wb_info_ptr->meta_priority;

    if (wb_info_ptr->calc_shuffle_stats)
    {
        db->common_info->pio_stats.num_blk_ops = wb_info_ptr->pio_stats.num_blk_ops;
        db->common_info->pio_stats.num_ix_cbs = wb_info_ptr->pio_stats.num_ix_cbs;
        db->common_info->pio_stats.num_of_piords = wb_info_ptr->pio_stats.num_of_piords;
        db->common_info->pio_stats.num_of_piowrs = wb_info_ptr->pio_stats.num_of_piowrs;
        db->common_info->pio_stats.num_of_worst_case_shuffles = wb_info_ptr->pio_stats.num_of_worst_case_shuffles;
        db->common_info->pio_stats.num_of_shuffles = wb_info_ptr->pio_stats.num_of_shuffles;
        db->common_info->pio_stats.num_ad_writes = wb_info_ptr->pio_stats.num_ad_writes;
    }

    db->has_dup_ad = wb_info_ptr->has_dup_ad;

    db->common_info->insertion_mode = wb_info_ptr->insertion_mode;

    if (wb_fun->nv_ptr == NULL)
    {
        db->device->alloc->xfree(db->device->alloc->cookie, wb_info_ptr);
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_reconcile_start(
    struct kaps_jr1_db * db)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;
    struct kaps_jr1_entry *entry;
    struct kaps_jr1_db *tmp;

    if (!db)
        return KAPS_JR1_INVALID_ARGUMENT;

    for (tmp = db; tmp; tmp = tmp->next_tab)
    {
        if (tmp->is_clone)
            continue;
        kaps_jr1_c_list_iter_init(&tmp->db_list, &it);

        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            entry = KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(e1);
            KAPS_JR1_STRY(db->fn_table->reset_used(entry));
        }
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_reconcile_end(
    struct kaps_jr1_db * db)
{
    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (db->is_clone)
        return KAPS_JR1_INVALID_ARGUMENT;

    return db->fn_table->reconcile_end(db);
}

uint32_t
kaps_jr1_db_entry_get_nv_offset(
    struct kaps_jr1_db * db,
    uint32_t user_handle)
{
    uint8_t found;
    uint8_t *entry_nv_ptr;

    kaps_jr1_cr_pool_mgr_get_entry_nv_ptr(db->device->nv_mem_mgr->kaps_jr1_cr_pool_mgr, db, user_handle, &found, &entry_nv_ptr);
    kaps_jr1_sassert(found);
    kaps_jr1_sassert(entry_nv_ptr);

    return (entry_nv_ptr - ((uint8_t *) db->device->nv_ptr));
}
