

#include "kaps_jr1_handle.h"
#include "kaps_jr1_cr_pool_mgr.h"
#include "kaps_jr1_key_internal.h"

kaps_jr1_status
kaps_jr1_db_init_internal(
    struct kaps_jr1_device *device,
    enum kaps_jr1_db_type type,
    uint32_t id,
    uint32_t capacity,
    struct kaps_jr1_db **dbp,
    int32_t is_broadcast)
{
    struct kaps_jr1_device *main_device;
    kaps_jr1_status status = KAPS_JR1_OK;

    if (!dbp)
        status = KAPS_JR1_INVALID_ARGUMENT;

    else if (!device)
        status = KAPS_JR1_INVALID_DEVICE_PTR;

    else if (device->is_config_locked)
        status = KAPS_JR1_DEVICE_ALREADY_LOCKED;

    else if ((device->flags & KAPS_JR1_DEVICE_ISSU) && (device->issu_status != KAPS_JR1_ISSU_INIT))
        status = KAPS_JR1_ISSU_IN_PROGRESS;


    if (status == KAPS_JR1_OK)
    {
        if ((device->type == KAPS_JR1_DEVICE) && (!is_broadcast))
        {
            if (device->main_dev)
            {
                device = device->main_dev;
            }
        }

        switch (type)
        {
            case KAPS_JR1_DB_LPM:
                status = kaps_jr1_lpm_db_init(device, id, capacity, dbp);
                break;
            default:
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (device->type == KAPS_JR1_DEVICE && capacity > 256 * 1024)
            capacity = 256 * 1024;

        (*dbp)->common_info->enable_dynamic_allocation = 1;


        main_device = device;
        if (main_device->main_dev)
            main_device = main_device->main_dev;

        if (main_device->next_dev || (main_device->type == KAPS_JR1_DEVICE))
            (*dbp)->common_info->enable_dynamic_allocation = 0;

        if (!is_broadcast)
        {
            KAPS_JR1_WB_HANDLE_TABLE_INIT((*dbp), capacity);
            KAPS_JR1_WB_HANDLE_TABLE_FREELIST_INIT(*dbp);
        }

        (*dbp)->bc_bitmap = 1 << device->bc_id;
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_init(
    struct kaps_jr1_device * device,
    enum kaps_jr1_db_type type,
    uint32_t id,
    uint32_t capacity,
    struct kaps_jr1_db ** dbp)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    KAPS_JR1_TRACE_IN("%p %u %u %u %p\n", device, type, id, capacity, dbp);
    status = kaps_jr1_db_init_internal(device, type, id, capacity, dbp, 0);
    KAPS_JR1_TRACE_OUT("%p\n", (dbp ? *dbp : 0));
    return status;
}

struct kaps_jr1_db *
kaps_jr1_db_get_bc_db_on_device(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *inp_db)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e;
    struct kaps_jr1_db *ret_db = NULL;


    if (device == inp_db->device || inp_db->common_info->is_cascaded)
    {
        ret_db = inp_db;
    }
    else
    {

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((e = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e);

            if (db->type != KAPS_JR1_DB_EM && db->type != KAPS_JR1_DB_LPM)
                continue;

            if ((inp_db->tid == db->tid) &&
                (inp_db->type == db->type) &&
                (!db->parent) && (inp_db->common_info->mapped_to_acl == db->common_info->mapped_to_acl))
            {
                ret_db = db;
                break;
            }
        }
    }


    return ret_db;
}


kaps_jr1_status
kaps_jr1_db_destroy_single_device(
    struct kaps_jr1_db * db)
{
    kaps_jr1_status status = KAPS_JR1_OK;



    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        if (!db->fn_table || !db->fn_table->db_destroy)
            status = KAPS_JR1_INTERNAL_ERROR;
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device && db->device->is_wb_continue)
        {
            struct kaps_jr1_device *tmp_device = db->device;

            db->device->issu_status = KAPS_JR1_ISSU_INIT;
            tmp_device = db->device;
            while (tmp_device)
            {
                tmp_device->issu_in_progress = NlmTrue;
                if (tmp_device->smt)
                    tmp_device->smt->issu_in_progress = NlmTrue;
                tmp_device = tmp_device->next_dev;
            }
        }

        if (db->description && db->device)
        {
            db->device->alloc->xfree(db->device->alloc->cookie, db->description);
            db->description = NULL;
        }

        status = db->fn_table->db_destroy(db);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_destroy(
    struct kaps_jr1_db * db)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p\n", db);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
        status = kaps_jr1_db_destroy_single_device(db);

    return status;
}

kaps_jr1_status
kaps_jr1_db_set_resource(
    struct kaps_jr1_db * db,
    enum kaps_jr1_hw_resource resource,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t value;

    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_JR1_DB_ACTIVE;
    }


    
    if (status == KAPS_JR1_OK)
    {
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT && db->device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        va_start(vl, resource);
        value = va_arg(vl, uint32_t);
        KAPS_JR1_TRACE_IN("%p %u %u\n", db, resource, value);
        switch (resource)
        {

            case KAPS_JR1_HW_RESOURCE_UDA:
 
                db->common_info->user_uda_mb = value;
                db->common_info->uda_mb = value;
                db->common_info->user_specified = 1;
                break;


            default:
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
        }

        va_end(vl);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_get_resource(
    struct kaps_jr1_db * db,
    enum kaps_jr1_hw_resource resource,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t *value = NULL;

    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
            status = KAPS_JR1_ISSU_IN_PROGRESS;
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        va_start(vl, resource);
        value = va_arg(vl, uint32_t *);
        KAPS_JR1_TRACE_IN("%p %u %p\n", db, resource, value);
        if (!value)
        {
            va_end(vl);
            status = KAPS_JR1_INVALID_ARGUMENT;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        switch (resource)
        {
            case KAPS_JR1_HW_RESOURCE_DBA:
            {
                uint32_t num_ab = 0;
                struct kaps_jr1_device *cascade;
                struct kaps_jr1_db *res_db;

                res_db = kaps_jr1_resource_get_res_db(db);

                cascade = db->device;
                if (cascade->main_dev)
                    cascade = cascade->main_dev;

                while (cascade)
                {
                    num_ab += kaps_jr1_resource_get_ab_usage(cascade, res_db);
                    cascade = cascade->next_dev;
                }

                *value = num_ab;
                break;
            }

            case KAPS_JR1_HW_RESOURCE_UDA:
                *value = db->common_info->uda_mb;
                break;

            default:
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
        }

        va_end(vl);
    }

    KAPS_JR1_TX_ERROR_CHECK(db->device, status);

    return status;
}

kaps_jr1_status
kaps_jr1_db_set_key(
    struct kaps_jr1_db * db,
    struct kaps_jr1_key * key)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, key);
    if (db == NULL || key == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    else if (db->device->is_config_locked)
        status = KAPS_JR1_DB_ACTIVE;

    else if (db->key)
        status = KAPS_JR1_DUPLICATE_KEY;

    else
    {
        if (db->device->type == KAPS_JR1_DEVICE)
        {
            if (key->width_1 > 160)
                status = KAPS_JR1_INVALID_KEY_WIDTH;
        }
        else
        {
            if (key->width_1 > KAPS_JR1_HW_MAX_DBA_WIDTH_1)
                status = KAPS_JR1_INVALID_KEY_WIDTH;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (key->first_field == NULL)
            status = KAPS_JR1_MISSING_KEY_FIELD;

        else if (key->has_dummy_fill_field)
            status = KAPS_JR1_DUMMY_FILL_FIELD_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->db_set_key)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {       
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (key->ref_count != 0)
        {
            
            struct kaps_jr1_key *new_key;
            status = kaps_jr1_key_clone(db->device, key, &new_key);
            if (status == KAPS_JR1_OK)
                key = new_key;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        key->ref_count++;

        status = kaps_jr1_key_validate_internal(db, key);
    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->db_set_key(db, key);
    }

    return status;
}


kaps_jr1_status
kaps_jr1_db_set_property(
    struct kaps_jr1_db * db,
    enum kaps_jr1_db_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;
    int32_t value;

    va_start(vl, property);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU && (property != KAPS_JR1_PROP_INDEX_CALLBACK))
        {

            if (db->device->issu_status != KAPS_JR1_ISSU_INIT && db->device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
            {
                status = KAPS_JR1_ISSU_IN_PROGRESS;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        switch (property)
        {

            case KAPS_JR1_PROP_DESCRIPTION:
            {
                if (db->device->is_config_locked)
                    status = KAPS_JR1_DB_ACTIVE;
                else
                {
                    char *desc = va_arg(vl, char *);
                    KAPS_JR1_TRACE_IN("%p %u %s\n", db, property, desc);
                    if (!desc)
                    {
                        status = KAPS_JR1_INVALID_DEVICE_DESC_PTR;
                        break;
                    }

                    if (!db->description) 
                    {
                        db->description = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, (strlen(desc) + 1));
                        if (!db->description)
                        {
                            status = KAPS_JR1_OUT_OF_MEMORY;
                            break;
                        }
                        strcpy(db->description, desc);
                    }
                }
                break;
            }

            case KAPS_JR1_PROP_INDEX_CALLBACK:
            {
                if (db->device->is_config_locked)
                {
                    status = KAPS_JR1_DB_ACTIVE;
                    break;
                }

                db->common_info->callback_fn = va_arg(vl, kaps_jr1_db_index_callback);
                db->common_info->callback_handle = va_arg(vl, void *);
                KAPS_JR1_TRACE_IN("%p %u %p %p\n", db, property, db->common_info->callback_fn,
                              db->common_info->callback_handle);
                if (!db->common_info->callback_fn)
                    status = KAPS_JR1_INVALID_ARGUMENT;

                break;
            }

            case KAPS_JR1_PROP_ALGORITHMIC:
            {
                if (db->device->is_config_locked)
                    status = KAPS_JR1_DB_ACTIVE;
                else
                {
                    value = va_arg(vl, int32_t);
                    KAPS_JR1_TRACE_IN("%p %u %d\n", db, property, value);
                    if (db->device->is_config_locked || db->ninstructions)
                    {
                        status = KAPS_JR1_INVALID_ARGUMENT;
                        break;
                    }
                    status = kaps_jr1_resource_set_algorithmic(db->device, db, value);
                    db->common_info->is_algo = 0;
                    if (!db->device->issu_in_progress)
                        kaps_jr1_resource_set_user_specified_algo_mode(db);
                    if (!db->device->issu_in_progress && status == KAPS_JR1_OK && value)
                    {
                        db->common_info->is_algo = 1;
                    }
                }
                break;
            }

            case KAPS_JR1_PROP_DEFER_DELETES:
            {
                if (db->device->is_config_locked)
                    status = KAPS_JR1_DB_ACTIVE;
                else
                {
                    value = va_arg(vl, uint32_t);
                    KAPS_JR1_TRACE_IN("%p %u %u\n", db, property, value);
                    if (value < 0 || value > 1)
                        status = KAPS_JR1_INVALID_ARGUMENT;
                    else
                    {
                        struct kaps_jr1_allocator *alloc = db->device->alloc;

                        if (db->common_info->defer_deletes_to_install && value)
                            break;

                        db->common_info->defer_deletes_to_install = value;
                        if (value)
                        {
                            if (db->common_info->capacity)
                            {
                                db->common_info->max_pending_del_entries = db->common_info->capacity;
                            }
                            else
                            {
                                db->common_info->max_pending_del_entries = 4 * 1024;
                            }

                            if (db->common_info->del_entry_list == NULL)
                            {
                                db->common_info->del_entry_list = alloc->xcalloc(alloc->cookie,
                                                                                 db->
                                                                                 common_info->max_pending_del_entries,
                                                                                 sizeof(struct kaps_jr1_entry *));
                                if (!db->common_info->del_entry_list)
                                {
                                    status = KAPS_JR1_OUT_OF_MEMORY;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            if (db->common_info->del_entry_list)
                            {
                                alloc->xfree(alloc->cookie, db->common_info->del_entry_list);
                                db->common_info->del_entry_list = NULL;
                            }
                        }
                        db->common_info->num_pending_del_entries = 0;
                    }
                }
                break;
            }

            case KAPS_JR1_PROP_LOG:
            {
                FILE *fp = va_arg(vl, FILE *);
                KAPS_JR1_TRACE_IN("%p %u %p\n", db, property, fp);

                
                db->common_info->fp = fp;
                break;
            }

            case KAPS_JR1_PROP_ENABLE_DYNAMIC_ALLOCATION:
            {
                value = va_arg(vl, int32_t);
                KAPS_JR1_TRACE_IN("%p %u %d\n", db, property, value);
                if (db->device->is_config_locked || (value != 0 && value != 1))
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }
                else
                    db->common_info->enable_dynamic_allocation = value;

                break;
            }

            default:
                
                KAPS_JR1_TRACE_IN("%p %u\n", db, property);
                if (!db->fn_table)
                    status = KAPS_JR1_INTERNAL_ERROR;
                if (status == KAPS_JR1_OK && !db->fn_table->db_set_property)
                    status = KAPS_JR1_INVALID_ARGUMENT;
                if (status == KAPS_JR1_OK)
                    status = db->fn_table->db_set_property(db, property, vl);
                break;
        }
        
    }

    va_end(vl);

    KAPS_JR1_TRACE_PRINT("%s\n", " ");
    return status;
}

kaps_jr1_status
kaps_jr1_db_get_property(
    struct kaps_jr1_db * db,
    enum kaps_jr1_db_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;
    int32_t *out;
    char **desc;

    KAPS_JR1_TRACE_IN("%p %u\n", db, property);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        va_start(vl, property);

        switch (property)
        {

            case KAPS_JR1_PROP_ALGORITHMIC:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }

                *out = kaps_jr1_db_get_algorithmic(db);

                break;



            case KAPS_JR1_PROP_DESCRIPTION:
                desc = va_arg(vl, char **);
                if (!desc)
                {
                    status = KAPS_JR1_INVALID_DEVICE_DESC_PTR;
                    break;
                }
                *desc = db->description;
                break;

            case KAPS_JR1_PROP_INDEX_CALLBACK:
            {
                kaps_jr1_db_index_callback *callback;
                void **handle;

                callback = va_arg(vl, kaps_jr1_db_index_callback *);
                handle = va_arg(vl, void **);
                if (!callback || !handle)
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }
                *callback = db->common_info->callback_fn;
                *handle = db->common_info->callback_handle;
                break;
            }

            case KAPS_JR1_PROP_DEFER_DELETES:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }

                *out = db->common_info->defer_deletes_to_install;
                break;


            case KAPS_JR1_PROP_ENTRY_META_PRIORITY:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }
                *out = db->common_info->meta_priority;
                break;

             case KAPS_JR1_PROP_NUM_LEVELS_IN_DB:
                out = va_arg(vl, int32_t *);
                if (!out)
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }
                *out = db->num_algo_levels_in_db;
                break;

            default:
                

                if (!db->fn_table)
                    status = KAPS_JR1_INTERNAL_ERROR;
                if (status == KAPS_JR1_OK && !db->fn_table->db_get_property)
                    status = KAPS_JR1_INVALID_ARGUMENT;
                if (status == KAPS_JR1_OK)
                    status = db->fn_table->db_get_property(db, property, vl);
                break;
        }

        va_end(vl);

        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_print(
    struct kaps_jr1_db * db,
    FILE * fp)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_db *next_tab;
    struct kaps_jr1_db_stats stats;

    KAPS_JR1_TRACE_IN("%p %p\n", db, fp);

    if (!db || !fp)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->parent)
            db = db->parent;

        kaps_jr1_fprintf(fp, "----------------------------------------------------------\n");

        kaps_jr1_fprintf(fp, "DB ID:%d TYPE:%s Capacity:%d BMR NO: ",
                     db->tid, kaps_jr1_device_db_name(db), db->common_info->capacity);

        if (db->key)
        {
            status = kaps_jr1_key_print(db->key, fp);
            KAPS_JR1_TX_ERROR_CHECK(db->device, status);
        }
    }

    if (status == KAPS_JR1_OK)
    {
        for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
        {
            struct kaps_jr1_db *tmp;

            if (next_tab->is_clone)
                continue;

            kaps_jr1_fprintf(fp, "|  \n|-- Table ID:%d TYPE:%s BMR NO: ", next_tab->tid, kaps_jr1_device_db_name(next_tab));

            if (next_tab->key)
            {
                kaps_jr1_fprintf(fp, "|   ");
                status = kaps_jr1_key_print(next_tab->key, fp);
                KAPS_JR1_TX_ERROR_CHECK(db->device, status);
                if (status != KAPS_JR1_OK)
                    break;
            }

            for (tmp = db->next_tab; tmp; tmp = tmp->next_tab)
            {
                if (tmp->is_clone && tmp->clone_of == next_tab)
                {
                    kaps_jr1_fprintf(fp, "|  |   \n|  |-- Clone ID:%d TYPE:%s BMR NO: ", tmp->tid,
                                 kaps_jr1_device_db_name(tmp));

                    if (tmp->key)
                    {
                        kaps_jr1_fprintf(fp, "|  |   ");
                        status = kaps_jr1_key_print(tmp->key, fp);
                        if (status != KAPS_JR1_OK)
                            break;
                    }
                }
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        for (next_tab = db->next_tab; next_tab; next_tab = next_tab->next_tab)
        {
            if (next_tab->is_clone && next_tab->clone_of == db)
            {
                kaps_jr1_fprintf(fp, "|  \n|-- Clone ID:%d TYPE:%s BMR NO: ", next_tab->tid, kaps_jr1_device_db_name(next_tab));

                if (next_tab->key)
                {
                    kaps_jr1_fprintf(fp, "|   ");
                    status = kaps_jr1_key_print(next_tab->key, fp);
                    KAPS_JR1_TX_ERROR_CHECK(db->device, status);
                    if (status != KAPS_JR1_OK)
                        break;
                }
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        
        if (db->common_info->ad_info.ad)
        {
            struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) (db->common_info->ad_info.ad);

            kaps_jr1_fprintf(fp, "\n---- AD Information ----\n");
            while (ad_db)
            {
                kaps_jr1_fprintf(fp, "AD DB ID:%d Width:%d Capacity:%d\n", ad_db->db_info.tid, ad_db->user_width_1,
                             ad_db->db_info.common_info->capacity);
                ad_db = ad_db->next;
            }
        }

        
        kaps_jr1_fprintf(fp, "\n");
        for (next_tab = db; next_tab; next_tab = next_tab->next_tab)
        {
            uint32_t i = 0;

            if (!next_tab->ninstructions)
                continue;

            if (next_tab == db)
            {
                kaps_jr1_fprintf(fp, "\n---- Instructions for %s ID:%d ninstructions:%d ----\n",
                             kaps_jr1_device_db_name(next_tab), next_tab->tid, next_tab->ninstructions);
            }
            else
            {
                kaps_jr1_fprintf(fp, "\n---- Instructions for %s ID:%d ninstructions:%d ----\n",
                             next_tab->is_clone ? "Clone" : "Table", next_tab->tid, next_tab->ninstructions);
            }

            for (i = 0; i < next_tab->ninstructions; i++)
            {
                kaps_jr1_instruction_print(next_tab->instructions[i], fp);
            }
            kaps_jr1_fprintf(fp, "\n");
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
        {
            kaps_jr1_fprintf(fp, "----------------------------------------------------------\n");
        }
        else
        {

            
            kaps_jr1_fprintf(fp, "---- STATS ----\n");
            status = kaps_jr1_db_stats(db, &stats);
            KAPS_JR1_TX_ERROR_CHECK(db->device, status);
            if (status != KAPS_JR1_OK)
                return status;

            kaps_jr1_fprintf(fp, "capacity estimate: %d nentries: %d \n",
                         stats.capacity_estimate, stats.num_entries);

            kaps_jr1_fprintf(fp, "----------------------------------------------------------\n");
        }
    }
    return status;
}

kaps_jr1_status
kaps_jr1_db_set_ad(
    struct kaps_jr1_db * db,
    struct kaps_jr1_ad_db * ad_db)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, ad_db);

    if (!db || !ad_db)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_JR1_DB_ACTIVE;

        
        else if (db->ninstructions)
            status = KAPS_JR1_DB_ALREADY_ADDED_TO_INSTRUCTION;

        
        else if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT && db->device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
        else
        {
            
        }
    }

    if (status == KAPS_JR1_OK)
        status = kaps_jr1_resource_db_add_ad(db->device, db, &ad_db->db_info);

    return status;
}

kaps_jr1_status
kaps_jr1_db_set_hb(
    struct kaps_jr1_db * db,
    struct kaps_jr1_hb_db * hb_db)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, hb_db);

    if (!db || !hb_db)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_JR1_DB_ACTIVE;

        
        else if (db->ninstructions)
            status = KAPS_JR1_DB_ALREADY_ADDED_TO_INSTRUCTION;

        
        else if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT && db->device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
        else
        {
            
        }
    }

    if (status == KAPS_JR1_OK)
        status = kaps_jr1_resource_db_add_hb(db->device, db, &hb_db->db_info);

    return status;
}

kaps_jr1_status
kaps_jr1_db_add_prefix(
    struct kaps_jr1_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_jr1_entry ** entry)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *tmp = NULL;
    struct kaps_jr1_db *tab = db;

    KAPS_JR1_TRACE_IN("%p %p %u %p\n", db, prefix, length, entry);

    if (db == NULL || prefix == NULL || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;
    else
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_JR1_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->is_clone)
            status = KAPS_JR1_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (db->key == NULL)
            status = KAPS_JR1_MISSING_KEY;

        else if (!db->fn_table)
            status = KAPS_JR1_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_prefix)
            status = KAPS_JR1_INVALID_ARGUMENT;
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->common_info->insertion_mode == KAPS_JR1_DB_INSERTION_MODE_NONE)
        {
            db->common_info->insertion_mode = KAPS_JR1_DB_INSERTION_MODE_NORMAL;
        }

        status = db->fn_table->db_add_prefix(db, prefix, length, entry);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        tmp = *entry;
        KAPS_JR1_WB_HANDLE_GET_FREE_AND_WRITE(db, (*entry));

    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->nv_ptr)
        {
            uint32_t entry_offset;
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, tmp);
            if (status == KAPS_JR1_OK)
            {
                entry_offset = kaps_jr1_db_entry_get_nv_offset(db, tmp->user_handle);

                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_jr1_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_JR1_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_JR1_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;

                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    KAPS_JR1_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_jr1_status
kaps_jr1_db_add_prefix_with_index(
    struct kaps_jr1_db * db,
    uint8_t * prefix,
    uint32_t length,
    uint32_t user_handle)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry = NULL;
    struct kaps_jr1_db *tab = db;

    KAPS_JR1_TRACE_IN("%p %p %u %u\n", db, prefix, length, user_handle);

    if (db == NULL || prefix == NULL || (user_handle >> 27) || !user_handle)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_JR1_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {

        if (db->is_clone)
            status = KAPS_JR1_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (db->key == NULL)
            status = KAPS_JR1_MISSING_KEY;

        else if (!db->fn_table)
            status = KAPS_JR1_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_prefix)
            status = KAPS_JR1_INVALID_ARGUMENT;
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        while (user_handle > (*db->common_info->user_hdl_table_size))
        {
            KAPS_JR1_WB_HANDLE_TABLE_GROW(db);
        }

        if (KAPS_JR1_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, user_handle) == 0)
            status = KAPS_JR1_DB_WB_LOCATION_BUSY;
    }

    if (status == KAPS_JR1_OK)
    {
        db->common_info->insertion_mode = KAPS_JR1_DB_INSERTION_MODE_ENTRY_WITH_INDEX;

        status = db->fn_table->db_add_prefix(db, prefix, length, &entry);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        KAPS_JR1_WB_HANDLE_WRITE_LOC(db, entry, user_handle);

    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->nv_ptr)
        {
            uint32_t entry_offset;
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, entry);

            if (status == KAPS_JR1_OK)
            {
                entry_offset = kaps_jr1_db_entry_get_nv_offset(db, entry->user_handle);

                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_jr1_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_JR1_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_JR1_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;

                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_add_em(
    struct kaps_jr1_db * db,
    uint8_t * data,
    struct kaps_jr1_entry ** entry)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *tmp = NULL;
    struct kaps_jr1_db *tab = db;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, data, entry);

    if (db == NULL || data == NULL || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->is_clone)
            status = KAPS_JR1_INVALID_ARGUMENT;

        else if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (!db->fn_table)
            status = KAPS_JR1_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_em)
            status = KAPS_JR1_INVALID_ARGUMENT;
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->common_info->insertion_mode == KAPS_JR1_DB_INSERTION_MODE_NONE)
            db->common_info->insertion_mode = KAPS_JR1_DB_INSERTION_MODE_NORMAL;

        status = db->fn_table->db_add_em(db, data, entry);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        tmp = *entry;
        KAPS_JR1_WB_HANDLE_GET_FREE_AND_WRITE(db, (*entry));
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->nv_ptr)
        {
            
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint32_t entry_offset;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, tmp);
            if (status == KAPS_JR1_OK)
            {
                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_jr1_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_JR1_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_JR1_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;
                entry_offset = kaps_jr1_db_entry_get_nv_offset(db, tmp->user_handle);
                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    KAPS_JR1_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_jr1_status
kaps_jr1_db_add_em_with_index(
    struct kaps_jr1_db * db,
    uint8_t * data,
    uint32_t user_handle)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry = NULL;
    struct kaps_jr1_db *tab = db;

    KAPS_JR1_TRACE_IN("%p %p %u\n", db, data, user_handle);

    if (db == NULL || data == NULL || (user_handle >> 27) || !user_handle)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->is_clone)
            status = KAPS_JR1_INVALID_ARGUMENT;

        else if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (!db->fn_table)
            status = KAPS_JR1_INTERNAL_ERROR;

        else if (!db->fn_table->db_add_em)
            status = KAPS_JR1_INVALID_ARGUMENT;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        while (user_handle > (*db->common_info->user_hdl_table_size))
        {
            KAPS_JR1_WB_HANDLE_TABLE_GROW(db);
        }

        if (KAPS_JR1_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, user_handle) == 0)
            status = KAPS_JR1_DB_WB_LOCATION_BUSY;
    }

    if (status == KAPS_JR1_OK)
    {
        db->common_info->insertion_mode = KAPS_JR1_DB_INSERTION_MODE_ENTRY_WITH_INDEX;

        status = db->fn_table->db_add_em(db, data, &entry);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        KAPS_JR1_WB_HANDLE_WRITE_LOC(db, entry, user_handle);

        if (db->device->nv_ptr)
        {
            
            uint32_t offset;
            uint32_t pending_count = 0;
            uint8_t *nv_ptr = tab->device->nv_ptr;
            uint32_t entry_offset;
            uint8_t *pending_ptr = NULL;

            status = db->fn_table->cr_store_entry(tab, entry);

            if (status == KAPS_JR1_OK)
            {
                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_ptr = nv_ptr;
                pending_count = *(uint32_t *) nv_ptr;

                if (kaps_jr1_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                    status = KAPS_JR1_EXHAUSTED_NV_MEMORY;
            }

            if (status == KAPS_JR1_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;
                entry_offset = kaps_jr1_db_entry_get_nv_offset(db, entry->user_handle);
                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_delete_entry(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;

    KAPS_JR1_TRACE_IN("%p %p\n", db, entry);

    if (db == NULL || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status != KAPS_JR1_ISSU_INIT))
            status = KAPS_JR1_UNSUPPORTED;

        else if (db->is_clone)
            status = KAPS_JR1_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->fn_table || !db->fn_table->db_delete_entry)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_JR1_INVALID_HANDLE;
    }

    if (status == KAPS_JR1_OK)
    {

        
        db->device->num_of_piowrs = 0;
        db->device->num_of_piords = 0;
        db->device->num_blk_ops = 0;

        if (db->device->nv_ptr && db->device->issu_status == KAPS_JR1_ISSU_INIT)
        {

            uint32_t entry_offset = kaps_jr1_db_entry_get_nv_offset(db, entry_p->user_handle);
            uint32_t offset;
            uint8_t *nv_ptr = db->device->nv_ptr;
            uint8_t *pending_ptr = NULL;
            uint32_t pending_count = 0;

            nv_ptr += entry_offset;

            
            *(uint8_t *) nv_ptr = 1;

            nv_ptr = db->device->nv_ptr;

            offset = db->device->nv_mem_mgr->offset_device_pending_list;
            nv_ptr += offset;
            pending_ptr = nv_ptr;
            pending_count = *(uint32_t *) nv_ptr;

            if (kaps_jr1_cr_pool_mgr_is_nv_exhausted(db->device, sizeof(uint32_t)))
                status = KAPS_JR1_EXHAUSTED_NV_MEMORY;
            if (status == KAPS_JR1_OK)
            {
                offset = sizeof(uint32_t) + (pending_count * sizeof(uint32_t));
                nv_ptr += offset;
                *(uint32_t *) nv_ptr = entry_offset;
                *(uint32_t *) pending_ptr = pending_count + 1;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        db->device->db_bc_bitmap = db->bc_bitmap;

        status = db->fn_table->db_delete_entry(db, entry_p);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        db->common_info->pio_stats.num_of_piords += db->device->num_of_piords;
        db->common_info->pio_stats.num_of_piowrs += db->device->num_of_piowrs;
        db->common_info->pio_stats.num_blk_ops += db->device->num_blk_ops;

        if (!db->common_info->defer_deletes_to_install)
        {
            KAPS_JR1_WB_HANDLE_DELETE_LOC(KAPS_JR1_GET_DB_PARENT(db), (uintptr_t) entry);

            if (db->device->nv_ptr && db->device->issu_status == KAPS_JR1_ISSU_INIT)
            {
                uint32_t entry_offset = kaps_jr1_db_entry_get_nv_offset(db, (uint32_t) ((uintptr_t) entry));
                uint32_t offset;
                uint8_t *nv_ptr = db->device->nv_ptr;
                uint32_t pending_count = 0;

                nv_ptr += entry_offset;
                *(uint8_t *) nv_ptr = 0;

                nv_ptr = db->device->nv_ptr;
                offset = db->device->nv_mem_mgr->offset_device_pending_list;
                nv_ptr += offset;
                pending_count = *(uint32_t *) nv_ptr;
                kaps_jr1_sassert(pending_count != 0);
                *(uint32_t *) nv_ptr = pending_count - 1;
            }
        }
    }


    return status;
}

kaps_jr1_status
kaps_jr1_entry_set_property(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    enum kaps_jr1_entry_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;

    KAPS_JR1_TRACE_IN("%p %p %u  TO_BE_FILLED\n", db, entry, property);

    if (entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;
    else
    {

        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);
        if (entry_p == NULL)
        {
            return KAPS_JR1_INTERNAL_ERROR;
        }
        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(db->device, entry_p, db);
        va_start(vl, property);

        if (!db->fn_table)
            status = KAPS_JR1_INTERNAL_ERROR;
        if (status == KAPS_JR1_OK && !db->fn_table->entry_set_property)
            status = KAPS_JR1_UNSUPPORTED;
        if (status == KAPS_JR1_OK)
            status = db->fn_table->entry_set_property(db, entry_p, property, vl);

        va_end(vl);
    }
    return status;
}


kaps_jr1_status
kaps_jr1_entry_add_ad(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_ad * ad)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;
    struct kaps_jr1_ad *ad_p = NULL;
    struct kaps_jr1_ad_db *e_ad_db = NULL;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, ad);

    if (db == NULL || ad == NULL || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;
    }

    
    if (status == KAPS_JR1_OK)
    {
        if (db->common_info->ad_info.ad == NULL)
            status = KAPS_JR1_AD_DB_ABSENT;

        else if (!db->fn_table || !db->fn_table->entry_add_ad)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT && db->device->issu_status != KAPS_JR1_ISSU_RESTORE_END)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), &entry_p, (uintptr_t) entry);
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db)->common_info->ad_info.ad, (&ad_p), (uintptr_t) ad);

        KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->device, ad_p, e_ad_db)

    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->entry_add_ad(db, entry_p, ad_p);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->nv_ptr && db->device->issu_status == KAPS_JR1_ISSU_INIT)
        {
            uint32_t user_width = e_ad_db->user_bytes;
            uint32_t byte_offset =
                (e_ad_db->db_info.width.ad_width_1 + KAPS_JR1_BITS_IN_BYTE - 1) / KAPS_JR1_BITS_IN_BYTE - user_width;

            if (kaps_jr1_resource_get_ad_type(db) == KAPS_JR1_AD_TYPE_INPLACE)
                byte_offset = 0;

            status =
                db->fn_table->cr_entry_update_ad(db->device, entry_p, entry_p->ad_handle,
                                                 &entry_p->ad_handle->value[byte_offset]);
            KAPS_JR1_TX_ERROR_CHECK(db->device, status);
        }
    }
    return status;
}

kaps_jr1_status
kaps_jr1_entry_add_hb(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_hb * hb)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;
    struct kaps_jr1_hb *hb_p = NULL;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, hb);

    if (db == NULL || hb == NULL || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;
    }

    if (status == KAPS_JR1_OK)
    {
        
        if (db->common_info->hb_info.hb == NULL)
            status = KAPS_JR1_HB_DB_ABSENT;

        else if (!db->fn_table)
            status = KAPS_JR1_INTERNAL_ERROR;

        else if (!db->fn_table->entry_add_hb)
            status = KAPS_JR1_DB_HAS_NO_HB;

        else if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT && db->device->issu_status != KAPS_JR1_ISSU_RESTORE_END)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), &entry_p, (uintptr_t) entry);
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db)->common_info->hb_info.hb, (&hb_p), (uintptr_t) hb);

        kaps_jr1_sassert(entry_p->ad_handle);

        status = db->fn_table->entry_add_hb(db, entry_p, hb_p);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_entry_print(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    FILE * fp)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, fp);

    if (db == NULL || fp == NULL || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->entry_print)
            status = KAPS_JR1_INTERNAL_ERROR;

        else if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);
        status = db->fn_table->entry_print(db, entry_p, fp);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

#define KAPS_JR1_STRY_NEW(A)                                                 \
    {                                                                   \
        kaps_jr1_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_JR1_OK)                                     \
        {                                                               \
            return __tmp_status;                                        \
        }                                                               \
    }

kaps_jr1_status
kaps_jr1_db_install(
    struct kaps_jr1_db * db)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p\n", db);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_JR1_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->db_install)
            status = KAPS_JR1_INTERNAL_ERROR;
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        
        db->device->num_of_piowrs = 0;
        db->device->num_of_piords = 0;
        db->device->num_blk_ops = 0;

        db->device->db_bc_bitmap = db->bc_bitmap;

        status = db->fn_table->db_install(db);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
        if (status != KAPS_JR1_OK)
        {

        }
        else
        {

            db->common_info->pio_stats.num_of_piords += db->device->num_of_piords;
            db->common_info->pio_stats.num_of_piowrs += db->device->num_of_piowrs;
            db->common_info->pio_stats.num_blk_ops += db->device->num_blk_ops;

            if (db->device->nv_ptr)
            {
                uint32_t offset;
                struct kaps_jr1_wb_cb_functions cb_fun;
                uint32_t nv_db_iter = db->common_info->nv_db_iter;

                cb_fun.handle = db->device->nv_handle;
                cb_fun.read_fn = db->device->nv_read_fn;
                cb_fun.write_fn = db->device->nv_write_fn;
                cb_fun.nv_offset = &offset;
                cb_fun.nv_ptr = db->device->nv_ptr;

                offset = db->device->nv_mem_mgr->offset_db_info_start[nv_db_iter];
                cb_fun.nv_ptr = (uint8_t *) db->device->nv_ptr + offset;

                status = db->fn_table->save_cr_state(db, &cb_fun);
            }
        }
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_stats(
    struct kaps_jr1_db * db,
    struct kaps_jr1_db_stats * stats)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, stats);

    if (db == NULL || stats == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_JR1_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->db_stats)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->db_stats(db, stats);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}



kaps_jr1_status
kaps_jr1_db_search(
    struct kaps_jr1_db * db,
    uint8_t * key,
    struct kaps_jr1_entry ** entry,
    int32_t * index,
    int32_t * prio_len)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %p %p %p\n", db, key, entry, index, prio_len);

    if (db == NULL || key == NULL || entry == NULL || index == NULL || prio_len == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED)
            {
                status = KAPS_JR1_UNSUPPORTED;
            }
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if (!db->fn_table || !db->fn_table->db_search)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->is_clone)
            db = db->clone_of;

        status = db->fn_table->db_search(db, key, entry, index, prio_len);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU && *entry)
            *entry = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, (*entry)->user_handle);
    }

    KAPS_JR1_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_jr1_status
kaps_jr1_db_add_table(
    struct kaps_jr1_db * db,
    uint32_t id,
    struct kaps_jr1_db ** table)
{
    struct kaps_jr1_db *tab;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %u %p\n", db, id, table);

    if (db == NULL || table == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    
    if (status == KAPS_JR1_OK)
    {
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
                return KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->parent)
            db = db->parent;

        if (db->device->is_config_locked)
            status = KAPS_JR1_DB_ACTIVE;

        else if (!db->fn_table || !db->fn_table->db_add_table)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        tab = db;
        while (tab)
        {
            if (tab->ninstructions)
                return KAPS_JR1_TABLES_CREATION_NOT_ALLOWED;
            tab = tab->next_tab;
        }

        status = db->fn_table->db_add_table(db, id, 0, table);
    }

    KAPS_JR1_TRACE_OUT("%p\n", *table);
    return status;
}

kaps_jr1_status
kaps_jr1_db_entry_iter_init(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry_iter ** iter)
{
    struct kaps_jr1_c_list_iter *it = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, iter);

    if (!db || !iter)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;
    }

    if (status == KAPS_JR1_OK)
    {
        if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            return KAPS_JR1_UNSUPPORTED;
    }

    if (status == KAPS_JR1_OK)
    {
        it = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(*it));
        if (!it)
            status = KAPS_JR1_OUT_OF_MEMORY;
    }

    if (status == KAPS_JR1_OK)
    {
        kaps_jr1_c_list_iter_init(&db->db_list, it);
        *iter = (struct kaps_jr1_entry_iter *) it;
    }

    KAPS_JR1_TRACE_OUT("%p\n", *iter);
    return status;
}

kaps_jr1_status
kaps_jr1_db_entry_iter_next(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry_iter * iter,
    struct kaps_jr1_entry ** entry)
{
    struct kaps_jr1_entry *entry_p;
    struct kaps_jr1_c_list_iter *it;
    struct kaps_jr1_list_node *el;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, iter, entry);

    if (!db || !iter || !entry)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;
    }

    if (status == KAPS_JR1_OK)
    {
        if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            status = KAPS_JR1_UNSUPPORTED;
    }

    if (status == KAPS_JR1_OK)
    {
        it = (struct kaps_jr1_c_list_iter *) iter;
        el = kaps_jr1_c_list_iter_next(it);
        if (el == NULL)
        {
            *entry = NULL;

        }
        else
        {

            entry_p = KAPS_JR1_DBLIST_TO_KAPS_JR1_ENTRY(el);

            if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
            {
                *entry = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, entry_p->user_handle);
            }
            else
                *entry = entry_p;
        }
    }
    KAPS_JR1_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_jr1_status
kaps_jr1_db_entry_iter_destroy(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry_iter * iter)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, iter);

    if (!db || !iter)
        status = KAPS_JR1_INVALID_ARGUMENT;
    else
    {

        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        db->device->alloc->xfree(db->device->alloc->cookie, iter);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_delete_all_entries(
    struct kaps_jr1_db * db)
{
    uint32_t defer_deletes = 0;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p\n", db);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->is_clone)
            status = KAPS_JR1_ADD_DEL_ENTRY_TO_CLONE_DB;

        else if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            status = KAPS_JR1_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->db_delete_all_entries)
            status = KAPS_JR1_INTERNAL_ERROR;
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        
        db->device->num_of_piowrs = 0;
        db->device->num_of_piords = 0;
        db->device->num_blk_ops = 0;

        db->device->db_bc_bitmap = db->bc_bitmap;

        if (db->common_info->defer_deletes_to_install)
        {
            defer_deletes = 1;
            db->common_info->defer_deletes_to_install = 0;
        }

        status = db->fn_table->db_delete_all_entries(db);
    }

    if (status == KAPS_JR1_OK)
    {
        db->common_info->defer_deletes_to_install = defer_deletes;

        db->common_info->pio_stats.num_of_piords += db->device->num_of_piords;
        db->common_info->pio_stats.num_of_piowrs += db->device->num_of_piowrs;
        db->common_info->pio_stats.num_blk_ops += db->device->num_blk_ops;

    }
    return status;
}

kaps_jr1_status
kaps_jr1_db_delete_all_pending_entries(
    struct kaps_jr1_db * db)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p\n", db);

    KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    status = db->fn_table->db_delete_all_pending_entries(db);

    return status;
}

kaps_jr1_status
kaps_jr1_entry_get_priority(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    uint32_t * prio_length)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, prio_length);

    if (!db || !entry || !prio_length)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            status = KAPS_JR1_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->entry_get_priority)
            status = KAPS_JR1_INTERNAL_ERROR;
        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_JR1_INVALID_ARGUMENT;
    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->entry_get_priority(db, entry_p, prio_length);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_entry_get_ad(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_ad ** ad)
{
    struct kaps_jr1_entry *entry_p = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, ad);

    if (!db || !entry || !ad)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_JR1_INVALID_HANDLE;

        else if (!db->common_info->ad_info.ad)
            status = KAPS_JR1_AD_DB_ABSENT;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        *ad = entry_p->ad_handle;
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            *ad = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_ad, entry_p->ad_handle->user_handle);
        }
    }

    KAPS_JR1_TRACE_OUT("%p\n", *ad);
    return status;
}

kaps_jr1_status
kaps_jr1_entry_get_hb(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_hb ** hb)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p = NULL;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, hb);

    if (!db || !entry || !hb)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (db->device->issu_status != KAPS_JR1_ISSU_INIT)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        if (!entry_p)
            status = KAPS_JR1_INVALID_HANDLE;

        else if (!db->common_info->hb_info.hb)
            status = KAPS_JR1_HB_DB_ABSENT;

        else if (!db->fn_table || !db->fn_table->get_hb)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {
        }
    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->get_hb(db, entry_p, hb);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    KAPS_JR1_TRACE_OUT("%p\n", *hb);
    return status;
}

kaps_jr1_status
kaps_jr1_entry_get_index(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    int32_t * nindices,
    int32_t ** indices)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p;

    KAPS_JR1_TRACE_IN("%p %p %p %p\n", db, entry, nindices, indices);

    if (!db || !entry || !nindices || !indices)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            status = KAPS_JR1_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->entry_get_index)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {

        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        status = db->fn_table->entry_get_index(db, entry_p, nindices, indices);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    KAPS_JR1_TRACE_OUT("%d\n", *nindices);
    return status;
}

kaps_jr1_status
kaps_jr1_entry_free_index_array(
    struct kaps_jr1_db * db,
    int32_t * indices)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p\n", db, indices);

    if (!db || !indices)
        status = KAPS_JR1_INVALID_ARGUMENT;
    else
    {

        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        db->device->alloc->xfree(db->device->alloc->cookie, indices);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_db_get_prefix_handle(
    struct kaps_jr1_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_jr1_entry ** entry)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %u %p\n", db, prefix, length, entry);

    if (!db || !prefix || entry == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->key == NULL)
            status = KAPS_JR1_PREFIX_NOT_FOUND;

        else if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            status = KAPS_JR1_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->get_prefix_handle)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {

        }
    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->get_prefix_handle(db, prefix, length, entry);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU && *entry)
        {
            *entry = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, (*entry)->user_handle);
        }
    }

    KAPS_JR1_TRACE_OUT("%p\n", *entry);
    return status;
}

kaps_jr1_status
kaps_jr1_db_clone(
    struct kaps_jr1_db * db,
    uint32_t id,
    struct kaps_jr1_db ** clone)
{
    struct kaps_jr1_db *tab;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %u %p\n", db, id, clone);

    if (!db || !clone)
        status = KAPS_JR1_INVALID_ARGUMENT;

    else if (db->device->is_config_locked)
        status = KAPS_JR1_DEVICE_ALREADY_LOCKED;

    else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status != KAPS_JR1_ISSU_INIT))
        status = KAPS_JR1_ISSU_IN_PROGRESS;

    else if (!db->fn_table || !db->fn_table->db_add_table)
        status = KAPS_JR1_INTERNAL_ERROR;

    else
    {

    }

    if (status == KAPS_JR1_OK)
    {
        tab = db;
        while (tab)
        {
            if (tab->ninstructions)
            {
                status = KAPS_JR1_TABLES_CREATION_NOT_ALLOWED;
                break;
            }
            tab = tab->next_tab;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        status = db->fn_table->db_add_table(db, id, 1, clone);
    }

    KAPS_JR1_TRACE_OUT("%p\n", *clone);
    return status;
}

kaps_jr1_status
kaps_jr1_entry_get_info(
    struct kaps_jr1_db * db,
    struct kaps_jr1_entry * entry,
    struct kaps_jr1_entry_info * info)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_entry *entry_p;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, entry, info);

    if (!db || !entry || !info)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(db->device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((db->device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             db->device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (!db->device->is_config_locked)
            status = KAPS_JR1_DEVICE_UNLOCKED;

        else if ((db->device->flags & KAPS_JR1_DEVICE_ISSU) && (db->device->issu_status == KAPS_JR1_ISSU_SAVE_COMPLETED))
            status = KAPS_JR1_UNSUPPORTED;

        else if (!db->fn_table || !db->fn_table->entry_get_info)
            status = KAPS_JR1_INTERNAL_ERROR;

        else
        {

        }
    }

    if (status == KAPS_JR1_OK)
    {
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if ((uintptr_t) entry > (*db->common_info->user_hdl_table_size))
                status = KAPS_JR1_INVALID_HANDLE;
            if (KAPS_JR1_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, ((uintptr_t) entry)))
                status = KAPS_JR1_INVALID_HANDLE;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), (&entry_p), (uintptr_t) entry);

        status = db->fn_table->entry_get_info(db, entry_p, info);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
    }

    return status;
}


struct kaps_jr1_db *
kaps_jr1_db_get_main_db(
    struct kaps_jr1_db *db)
{
    struct kaps_jr1_db *main_db = db;
    struct kaps_jr1_db *ret = NULL;

    KAPS_JR1_TRACE_IN("%p\n", db);

    if (main_db)
    {

        if (main_db->clone_of)
            main_db = main_db->clone_of;

        if (main_db->parent)
            main_db = main_db->parent;

        ret = main_db;
    }
    return ret;
}
