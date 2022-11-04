

#include "kaps_jr1_ad.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_ab.h"

kaps_jr1_status
kaps_jr1_ad_db_init(
    struct kaps_jr1_device *device,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    struct kaps_jr1_ad_db **db)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    KAPS_JR1_TRACE_IN("%p %u %u %u %p\n", device, id, capacity, width_1, db);

    status = kaps_jr1_ad_db_init_internal(device, id, capacity, width_1, db, 0);
    KAPS_JR1_TRACE_OUT("%p\n", (db ? (*db) : NULL));
    return status;
}

kaps_jr1_status
kaps_jr1_ad_db_init_internal(
    struct kaps_jr1_device * device,
    uint32_t id,
    uint32_t capacity,
    uint32_t width_1,
    struct kaps_jr1_ad_db ** db,
    uint32_t is_internal)
{
    struct kaps_jr1_ad_db *ad_db;
    kaps_jr1_status status;

    if (device == NULL)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    
    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_JR1_ISSU_INIT && device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    
    if (width_1 != KAPS_JR1_DEFAULT_AD_WIDTH_1)
        return KAPS_JR1_UNSUPPORTED_AD_SIZE;

    ad_db = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_ad_db));
    if (!ad_db)
    {
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    ad_db->db_info.is_internal = is_internal;
    status = kaps_jr1_resource_add_database(device, &ad_db->db_info, id, capacity, width_1, KAPS_JR1_DB_AD);
    if (status != KAPS_JR1_OK)
    {
        device->alloc->xfree(device->alloc->cookie, ad_db);
        return status;
    }

    ad_db->alloc = device->alloc;
    ad_db->user_width_1 = width_1;
    ad_db->user_bytes = (width_1 + KAPS_JR1_BITS_IN_BYTE - 1) / KAPS_JR1_BITS_IN_BYTE;

    ad_db->byte_offset = 0;
    ad_db->device_bytes = (ad_db->db_info.width.ad_width_1 + 7) / KAPS_JR1_BITS_IN_BYTE;
    ad_db->ix_max_num_holes_allowed = 4;

    POOL_INIT(kaps_jr1_ad_chunk, &ad_db->ad_chunk_pool, device->alloc);

    *db = ad_db;

    KAPS_JR1_WB_HANDLE_TABLE_INIT((&ad_db->db_info), capacity);

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status == KAPS_JR1_ISSU_RESTORE_START)
            return KAPS_JR1_OK;
    }

    KAPS_JR1_WB_HANDLE_TABLE_FREELIST_INIT(&ad_db->db_info);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_destroy(
    struct kaps_jr1_ad_db * ad_db)
{
    KAPS_JR1_TRACE_IN("%p\n", ad_db);

    if (!ad_db)
        return KAPS_JR1_INVALID_ARGUMENT;

    
    if (ad_db->db_info.common_info->ad_info.db)
        return KAPS_JR1_DB_ACTIVE;

    
    {
        struct kaps_jr1_device *device;
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *el;

        device = ad_db->db_info.device;
        if (device->main_dev)
            device = device->main_dev;

        KAPS_JR1_TX_ERROR_CHECK(device, 0);
        KAPS_JR1_ASSERT_OR_ERROR((device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (tmp == &ad_db->db_info)
            {
                kaps_jr1_c_list_remove_node(&device->db_list, el, &it);
                break;
            }
        }
    }

    if (ad_db->device_bytes <= 4)
    {
        POOL_FINI(kaps_jr1_ad_32b, &ad_db->pool.ad_entry_pool_32);
    }
    else if (ad_db->device_bytes == 8)
    {
        POOL_FINI(kaps_jr1_ad_64b, &ad_db->pool.ad_entry_pool_64);
    }
    else if (ad_db->device_bytes == 16)
    {
        POOL_FINI(kaps_jr1_ad_128b, &ad_db->pool.ad_entry_pool_128);
    }
    else
    {
        POOL_FINI(kaps_jr1_ad_256b, &ad_db->pool.ad_entry_pool_256);
    }

    POOL_FINI(kaps_jr1_ad_chunk, &ad_db->ad_chunk_pool);

    if (!ad_db->next)
        KAPS_JR1_WB_HANDLE_TABLE_DESTROY((&ad_db->db_info));

    kaps_jr1_resource_free_database(ad_db->db_info.device, &ad_db->db_info);

    ad_db->alloc->xfree(ad_db->alloc->cookie, ad_db);
    return KAPS_JR1_OK;
}

static void
copy_user_ad_data(
    struct kaps_jr1_ad_db *ad_db,
    uint8_t * value,
    struct kaps_jr1_ad *ad)
{
    kaps_jr1_memcpy(&ad->value[ad_db->byte_offset], value, ad_db->user_bytes);
    if (ad_db->user_width_1 & 0x7)
    {
        uint8_t spare_bits;
        uint8_t mask;

        spare_bits = ad_db->user_width_1 & 0x7;
        mask = 0xFF << (KAPS_JR1_BITS_IN_BYTE - spare_bits);
        ad->value[ad_db->byte_offset + ad_db->user_bytes - 1] &= mask;
    }
}

kaps_jr1_status
kaps_jr1_ad_db_add_entry_internal(
    struct kaps_jr1_ad_db *ad_db,
    uint8_t * value,
    struct kaps_jr1_ad **ad,
    intptr_t api_user_handle)
{
    struct kaps_jr1_ad *ad_handle;
    struct kaps_jr1_ad *act_ad_p;

    if (!ad_db || !ad)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!value && ad_db->user_width_1)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!ad_db->db_info.device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (!ad_db->pool_init_done)
    {
        if (ad_db->device_bytes <= 4)
        {
            POOL_INIT(kaps_jr1_ad_32b, &ad_db->pool.ad_entry_pool_32, ad_db->alloc);
        }
        else if (ad_db->device_bytes == 8)
        {
            POOL_INIT(kaps_jr1_ad_64b, &ad_db->pool.ad_entry_pool_64, ad_db->alloc);
        }
        else if (ad_db->device_bytes == 16)
        {
            POOL_INIT(kaps_jr1_ad_128b, &ad_db->pool.ad_entry_pool_128, ad_db->alloc);
        }
        else
        {
            POOL_INIT(kaps_jr1_ad_256b, &ad_db->pool.ad_entry_pool_256, ad_db->alloc);
        }
        ad_db->pool_init_done = 1;
    }

    if (ad_db->device_bytes <= 4)
    {
        struct kaps_jr1_ad_32b *ad_hdl_32;

        POOL_ALLOC(kaps_jr1_ad_32b, &ad_db->pool.ad_entry_pool_32, ad_hdl_32);
        if (!ad_hdl_32)
        {
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        kaps_jr1_memset(ad_hdl_32, 0, sizeof(*ad_hdl_32));
        ad_handle = (struct kaps_jr1_ad *) ad_hdl_32;
    }
    else if (ad_db->device_bytes == 8)
    {
        struct kaps_jr1_ad_64b *ad_hdl_64;

        POOL_ALLOC(kaps_jr1_ad_64b, &ad_db->pool.ad_entry_pool_64, ad_hdl_64);
        if (!ad_hdl_64)
        {
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        kaps_jr1_memset(ad_hdl_64, 0, sizeof(*ad_hdl_64));
        ad_handle = (struct kaps_jr1_ad *) ad_hdl_64;
    }
    else if (ad_db->device_bytes == 16)
    {
        struct kaps_jr1_ad_128b *ad_hdl_128;

        POOL_ALLOC(kaps_jr1_ad_128b, &ad_db->pool.ad_entry_pool_128, ad_hdl_128);
        if (!ad_hdl_128)
        {
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        kaps_jr1_memset(ad_hdl_128, 0, sizeof(*ad_hdl_128));
        ad_handle = (struct kaps_jr1_ad *) ad_hdl_128;
    }
    else
    {
        struct kaps_jr1_ad_256b *ad_hdl_256;

        POOL_ALLOC(kaps_jr1_ad_256b, &ad_db->pool.ad_entry_pool_256, ad_hdl_256);
        if (!ad_hdl_256)
        {
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        kaps_jr1_memset(ad_hdl_256, 0, sizeof(*ad_hdl_256));
        ad_handle = (struct kaps_jr1_ad *) ad_hdl_256;
    }

    
    if (value)
        copy_user_ad_data(ad_db, value, ad_handle);

    kaps_jr1_c_list_add_tail(&ad_db->ad_list, &ad_handle->ad_node);
    ad_handle->ad_db_seq_num_0 = ad_db->db_info.seq_num & 0x1F;
    ad_handle->ad_db_seq_num_1 = (ad_db->db_info.seq_num >> 5) & 0x7;

    *ad = ad_handle;

    if (api_user_handle != -1)
    {
        act_ad_p = (struct kaps_jr1_ad *) ad_handle;
        KAPS_JR1_WB_HANDLE_WRITE_LOC((&ad_db->db_info), act_ad_p, api_user_handle);
    }
    else
    {
        KAPS_JR1_WB_HANDLE_GET_FREE_AND_WRITE((&ad_db->db_info), (*ad));

    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_add_entry(
    struct kaps_jr1_ad_db * ad_db,
    uint8_t * value,
    struct kaps_jr1_ad ** ad)
{
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %p\n", ad_db, value, ad);

    if (!ad_db || !value || !ad)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        if (ad_db->user_width_1 == 0 && ad_db->ad_list.count > 1)
        {
            return KAPS_JR1_AD_DB_ONLY_ONE_AD_HANDLE;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, 0);

        KAPS_JR1_ASSERT_OR_ERROR((ad_db->db_info.device->fatal_transport_error == 0),
                             "Transport Error ignored. Asserting\n",
                             ad_db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

        if (ad_db->db_info.common_info->ad_info.db)
        {
            ad_db->db_info.device->db_bc_bitmap = ad_db->db_info.common_info->ad_info.db->bc_bitmap;
        }

        if (ad_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (ad_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
                status = KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        ad_db->db_info.common_info->cur_capacity++;
        status = kaps_jr1_ad_db_add_entry_internal(ad_db, value, ad, -1);

        KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, status);
    }

    KAPS_JR1_TRACE_OUT("%p\n", (ad ? (*ad) : NULL));
    return status;
}

static kaps_jr1_status
kaps_jr1_ad_db_delete_entry_internal(
    struct kaps_jr1_ad_db *ad_db,
    struct kaps_jr1_ad *user_ad_p,
    struct kaps_jr1_c_list_iter *it)
{
    struct kaps_jr1_ad *ad = NULL;
    struct kaps_jr1_device *device;
    struct kaps_jr1_ad_db *parent_ad = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;

    if (!ad_db || !user_ad_p)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((ad_db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         ad_db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    KAPS_JR1_WB_HANDLE_READ_LOC((&ad_db->db_info), (&ad), (uintptr_t) user_ad_p);

    if (!ad_db || !ad)
        return KAPS_JR1_INVALID_ARGUMENT;

    device = ad_db->db_info.device;

    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, ad, ad_db);

    if (!ad_db->db_info.device->is_config_locked)
    {
        if (ad_db->db_info.device->issu_status != KAPS_JR1_ISSU_RECONCILE_END)
            return KAPS_JR1_DEVICE_UNLOCKED;
    }

    if (ad_db->user_width_1 == 0)
    {
        if (ad_db->num_0b_entries)
            return KAPS_JR1_AD_ENTRY_ACTIVE;
    }
    else
    {
        if (ad->ref_count)
            return KAPS_JR1_AD_ENTRY_ACTIVE;
    }

    ad_db->db_info.common_info->cur_capacity--;

    kaps_jr1_sassert(ad->entries == NULL);

    kaps_jr1_c_list_remove_node(&ad_db->ad_list, &ad->ad_node, it);

    parent_ad = ad_db;

    if (ad_db->db_info.is_internal)
    {
        kaps_jr1_sassert(ad_db->parent_ad);
        parent_ad = ad_db->parent_ad;
    }

    KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, status);

    if (parent_ad->device_bytes <= 4)
    {
        struct kaps_jr1_ad_32b *ad_hdl_32 = (struct kaps_jr1_ad_32b *) ad;

        POOL_FREE(kaps_jr1_ad_32b, &parent_ad->pool.ad_entry_pool_32, ad_hdl_32);
    }
    else if (parent_ad->device_bytes == 8)
    {
        struct kaps_jr1_ad_64b *ad_hdl_64 = (struct kaps_jr1_ad_64b *) ad;

        POOL_FREE(kaps_jr1_ad_64b, &parent_ad->pool.ad_entry_pool_64, ad_hdl_64);
    }
    else if (parent_ad->device_bytes == 16)
    {
        struct kaps_jr1_ad_128b *ad_hdl_128 = (struct kaps_jr1_ad_128b *) ad;

        POOL_FREE(kaps_jr1_ad_128b, &parent_ad->pool.ad_entry_pool_128, ad_hdl_128);
    }
    else
    {
        struct kaps_jr1_ad_256b *ad_hdl_256 = (struct kaps_jr1_ad_256b *) ad;

        POOL_FREE(kaps_jr1_ad_256b, &parent_ad->pool.ad_entry_pool_256, ad_hdl_256);
    }

    KAPS_JR1_WB_HANDLE_DELETE_LOC((&parent_ad->db_info), (uintptr_t) user_ad_p);

    return status;
}

kaps_jr1_status
kaps_jr1_ad_db_delete_entry(
    struct kaps_jr1_ad_db * ad_db,
    struct kaps_jr1_ad * user_ad_p)
{
    KAPS_JR1_TRACE_IN("%p %p\n", ad_db, user_ad_p);
    return kaps_jr1_ad_db_delete_entry_internal(ad_db, user_ad_p, NULL);
}

kaps_jr1_status
kaps_jr1_ad_db_update_entry(
    struct kaps_jr1_ad_db * ad_db,
    struct kaps_jr1_ad * usr_ad_handle,
    uint8_t * value)
{
    struct kaps_jr1_ad *ad_handle = NULL;
    uintptr_t user_handle = 0;
    struct kaps_jr1_db *db = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %p\n", ad_db, usr_ad_handle, value);

    if (!ad_db || !usr_ad_handle || !value)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((ad_db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         ad_db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    if (!ad_db->db_info.device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (ad_db->user_width_1 == 0)
        return KAPS_JR1_UNSUPPORTED_FOR_0B_AD;

    if (ad_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (ad_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    db = ad_db->db_info.common_info->ad_info.db;
    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_WB_HANDLE_READ_LOC((&ad_db->db_info), (&ad_handle), (uintptr_t) usr_ad_handle);
    if (ad_handle == NULL)
    {
        return KAPS_JR1_INTERNAL_ERROR;
    }
    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(db->device, ad_handle, ad_db);

    if (ad_db->db_info.common_info->ad_info.db)
    {
        ad_db->db_info.device->db_bc_bitmap = ad_db->db_info.common_info->ad_info.db->bc_bitmap;
    }

    
    ad_db->db_info.device->num_blk_ops = 0;
    ad_db->db_info.device->num_of_piowrs = 0;
    ad_db->db_info.device->num_of_piords = 0;

    
    copy_user_ad_data(ad_db, value, ad_handle);

    if (ad_handle->ref_count == 1)
    {
        struct kaps_jr1_db *in_db;
        struct kaps_jr1_entry *e = (struct kaps_jr1_entry *) ad_handle->entries;

        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(db->device, e, in_db);

        in_db->device->db_bc_bitmap = in_db->bc_bitmap;

        if (in_db->fn_table == NULL || in_db->fn_table->update_ad == NULL)
            return KAPS_JR1_INTERNAL_ERROR;

        status = in_db->fn_table->update_ad(in_db, ad_handle, e, ad_db->device_bytes);
        KAPS_JR1_TX_ERROR_CHECK(db->device, status);
        if (status != KAPS_JR1_OK)
            return status;

    }
    else if (ad_handle->ref_count > 1)
    {
        int32_t i;

        for (i = 0; i < ad_handle->ref_count; i++)
        {
            struct kaps_jr1_entry *e;
            struct kaps_jr1_db *in_db;

            e = ad_handle->entries[i];

            kaps_jr1_sassert(e != NULL);
            KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(db->device, e, in_db);

            in_db->device->db_bc_bitmap = in_db->bc_bitmap;

            if (in_db->fn_table == NULL || in_db->fn_table->update_ad == NULL)
                return KAPS_JR1_INTERNAL_ERROR;

            status = in_db->fn_table->update_ad(in_db, ad_handle, e, ad_db->device_bytes);
            KAPS_JR1_TX_ERROR_CHECK(db->device, status);
            if (status != KAPS_JR1_OK)
                return status;
        }
    }

    db->common_info->pio_stats.num_blk_ops += ad_db->db_info.device->num_blk_ops;
    db->common_info->pio_stats.num_of_piowrs += ad_db->db_info.device->num_of_piowrs;
    db->common_info->pio_stats.num_of_piords += ad_db->db_info.device->num_of_piords;

    if (ad_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        user_handle = (uintptr_t) usr_ad_handle;
        KAPS_JR1_WB_HANDLE_UPDATE_LOC((&ad_db->db_info), ad_handle, user_handle);
    }

    if (db->device->nv_ptr)
    {
        if (ad_handle->ref_count == 1)
        {
            struct kaps_jr1_db *in_db;
            struct kaps_jr1_entry *e = (struct kaps_jr1_entry *) ad_handle->entries;

            KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(db->device, e, in_db);
            status = in_db->fn_table->cr_entry_update_ad(in_db->device, e, ad_handle, value);
            KAPS_JR1_TX_ERROR_CHECK(db->device, status);
            if (status != KAPS_JR1_OK)
                return status;
        }
        else if (ad_handle->ref_count > 1)
        {
            int32_t i;

            for (i = 0; i < ad_handle->ref_count; i++)
            {
                struct kaps_jr1_entry *e;
                struct kaps_jr1_db *in_db;

                e = ad_handle->entries[i];
                KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(db->device, e, in_db);
                status = in_db->fn_table->cr_entry_update_ad(in_db->device, e, ad_handle, value);
                KAPS_JR1_TX_ERROR_CHECK(db->device, status);
                if (status != KAPS_JR1_OK)
                    return status;
            }
        }
    }

    KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, status);

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ad_db_get(
    struct kaps_jr1_ad_db * ad_db,
    struct kaps_jr1_ad * usr_ad_handle,
    uint8_t * value)
{
    struct kaps_jr1_ad *ad_handle = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %p %p\n", ad_db, usr_ad_handle, value);
    if (!ad_db || !usr_ad_handle || !value)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(ad_db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((ad_db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         ad_db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    if (!ad_db->db_info.device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (ad_db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (ad_db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    if (ad_db->db_info.device->prop.read_type == 0)
    {
        KAPS_JR1_WB_HANDLE_READ_LOC((&ad_db->db_info), (&ad_handle), (uintptr_t) usr_ad_handle);

        
        kaps_jr1_memcpy(value, &ad_handle->value[ad_db->byte_offset], ad_db->user_bytes);
    }

    return status;
}

kaps_jr1_status
kaps_jr1_ad_db_set_resource(
    struct kaps_jr1_ad_db * db,
    enum kaps_jr1_hw_resource resource,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %d ", db, resource);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    else if (db->db_info.device->is_config_locked)
        status = KAPS_JR1_DB_ACTIVE;

    else if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }
    else
    {
    }

    if (status == KAPS_JR1_OK)
    {
        va_start(vl, resource);
        switch (resource)
        {
            case KAPS_JR1_HW_RESOURCE_UDA:
            {
                uint32_t value = va_arg(vl, uint32_t);

                KAPS_JR1_TRACE_PRINT("%u", value);

                db->db_info.common_info->uda_mb = value;
                db->db_info.common_info->user_specified = 1;

                break;
            }
            default:
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
        }
        va_end(vl);
    }
    KAPS_JR1_TRACE_PRINT("%s\n", " ");
    return status;
}

kaps_jr1_status
kaps_jr1_ad_db_get_resource(
    struct kaps_jr1_ad_db * db,
    enum kaps_jr1_hw_resource resource,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %u\n", db, resource);

    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    va_start(vl, resource);
    switch (resource)
    {
        case KAPS_JR1_HW_RESOURCE_UDA:
        {
            uint32_t *value = va_arg(vl, uint32_t *);

            if (!value)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }

            *value = db->db_info.common_info->uda_mb;

            KAPS_JR1_TRACE_PRINT("%u", value);
            break;
        }
        default:
            status = KAPS_JR1_INVALID_ARGUMENT;
            break;
    }
    va_end(vl);

    KAPS_JR1_TRACE_PRINT("%s\n", " ");
    KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, status);

    return status;
}

kaps_jr1_status
kaps_jr1_ad_db_set_property(
    struct kaps_jr1_ad_db * db,
    enum kaps_jr1_db_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %u", db, property);

    if (db == NULL)
        status = KAPS_JR1_INVALID_ARGUMENT;

    else if (db->db_info.device->is_config_locked)
        status = KAPS_JR1_DB_ACTIVE;

    else if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            status = KAPS_JR1_ISSU_IN_PROGRESS;
    }
    else
    {

    }

    if (status == KAPS_JR1_OK)
    {
        va_start(vl, property);
        switch (property)
        {
            case KAPS_JR1_PROP_DESCRIPTION:
            {
                char *desc = va_arg(vl, char *);
                KAPS_JR1_TRACE_PRINT("%s", desc);

                if (!desc)
                {
                    status = KAPS_JR1_INVALID_ARGUMENT;
                    break;
                }
                db->db_info.description
                    = db->db_info.device->alloc->xcalloc(db->db_info.device->alloc->cookie, 1, strlen(desc) + 1);
                if (!db->db_info.description)
                {
                    status = KAPS_JR1_OUT_OF_MEMORY;
                    break;
                }
                strcpy(db->db_info.description, desc);
                break;
            }


            default:
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
        }
        va_end(vl);
    }

    KAPS_JR1_TRACE_PRINT("%s \n", " ");
    return status;
}

kaps_jr1_status
kaps_jr1_ad_db_get_property(
    struct kaps_jr1_ad_db * db,
    enum kaps_jr1_db_properties property,
    ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %u\n", db, property);
    if (db == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
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
            *desc = db->db_info.description;
            break;
        }

        default:
            status = KAPS_JR1_INVALID_ARGUMENT;
            break;
    }
    va_end(vl);

    KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, status);

    return status;
}

kaps_jr1_status
kaps_jr1_ad_entry_print(
    struct kaps_jr1_ad_db * db,
    struct kaps_jr1_ad * ad_m,
    FILE * fp)
{
    int32_t i;
    struct kaps_jr1_ad *ad = NULL;

    KAPS_JR1_TRACE_IN("%p %p %p\n", db, ad_m, fp);

    if (!db || !ad_m || !fp)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (db->db_info.device->issu_status != KAPS_JR1_ISSU_INIT)
            return KAPS_JR1_ISSU_IN_PROGRESS;
    }

    KAPS_JR1_WB_HANDLE_READ_LOC((&db->db_info), (&ad), (uintptr_t) ad_m);
    if (!ad)
        return KAPS_JR1_INVALID_ARGUMENT;

    kaps_jr1_fprintf(fp, " ");
    for (i = db->byte_offset; i < db->device_bytes; i++)
    {
        if (i == db->device_bytes - 1)
            kaps_jr1_fprintf(fp, "%02x", ad->value[i]);
        else
            kaps_jr1_fprintf(fp, "%02x.", ad->value[i]);
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_delete_all_entries(
    struct kaps_jr1_ad_db * db)
{
    KAPS_JR1_TRACE_IN("%p\n", db);
    if (!db)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_ASSERT_OR_ERROR((db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    do
    {
        struct kaps_jr1_list_node *el;
        struct kaps_jr1_ad *e;

        el = kaps_jr1_c_list_head(&db->ad_list);
        if (el == NULL)
            break;
        e = DBLIST_TO_AD_ENTRY(el);

        if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            KAPS_JR1_STRY(kaps_jr1_ad_db_delete_entry(db, KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_ad, e->user_handle)));
        }
        else
        {
            KAPS_JR1_STRY(kaps_jr1_ad_db_delete_entry(db, e));
        }
    }
    while (1);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_refresh_handle(
    struct kaps_jr1_device * device,
    struct kaps_jr1_ad_db * stale_ptr,
    struct kaps_jr1_ad_db ** dbp)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;
    struct kaps_jr1_ad_db *db;

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

        if (tmp->type == KAPS_JR1_DB_AD)
        {
            db = (struct kaps_jr1_ad_db *) tmp;
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

            if (tmp->type == KAPS_JR1_DB_AD)
            {
                db = (struct kaps_jr1_ad_db *) tmp;
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

kaps_jr1_status
kaps_jr1_ad_db_associate_entry_and_ad(
    struct kaps_jr1_device * device,
    struct kaps_jr1_ad * ad,
    struct kaps_jr1_entry * entry)
{
    struct kaps_jr1_ad_db *ad_db;
    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, ad, ad_db);

    if (ad_db->user_width_1 == 0)
    {
        ad_db->num_0b_entries++;
        return KAPS_JR1_OK;
    }

    
    if (ad->ref_count >= KAPS_JR1_AD_MAX_INDIRECTION)       
        return KAPS_JR1_EXCEEDED_INDIRECTION;

    if (ad->ref_count == 0)
    {
        ad->entries = (struct kaps_jr1_entry **) entry;
    }
    else if (ad->ref_count == 1)
    {
        struct kaps_jr1_db *db;
        struct kaps_jr1_entry *old_e = (struct kaps_jr1_entry *) ad->entries;
        uint16_t ratio = 0;

        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, entry, db)

        if (db->common_info->ad_info.ad->common_info->capacity)
            ratio = (db->common_info->capacity) / (db->common_info->ad_info.ad->common_info->capacity);

        if (ratio < 2)
            ratio = 2;
        ad->entries = db->device->alloc->xcalloc(db->device->alloc->cookie, ratio, sizeof(struct kaps_jr1_entry *));
        if (!ad->entries)
        {
            ad->entries = (struct kaps_jr1_entry **) old_e;
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        ad->entries[0] = old_e;
        ad->entries[ad->ref_count] = entry;
        ad->arr_size = ratio;
    }
    else if (ad->ref_count < ad->arr_size)
    {
        ad->entries[ad->ref_count] = entry;
    }
    else
    {
        struct kaps_jr1_db *db;
        uint32_t i, num_entries = ad->arr_size;
        uint32_t new_size = (uint32_t) ad->arr_size * 1.5;
        struct kaps_jr1_entry **tmp;

        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, entry, db);

        if (new_size >= KAPS_JR1_AD_MAX_INDIRECTION)
            new_size = KAPS_JR1_AD_MAX_INDIRECTION;

        tmp = db->device->alloc->xcalloc(db->device->alloc->cookie, new_size, sizeof(struct kaps_jr1_entry *));
        if (!tmp)
            return KAPS_JR1_OUT_OF_MEMORY;
        for (i = 0; i < num_entries; i++)
            tmp[i] = ad->entries[i];
        db->device->alloc->xfree(db->device->alloc->cookie, ad->entries);
        ad->entries = tmp;
        ad->entries[ad->ref_count] = entry;
        ad->arr_size = new_size;
    }

    ad->ref_count++;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_deassociate_entry_and_ad(
    struct kaps_jr1_device * device,
    struct kaps_jr1_ad * ad,
    struct kaps_jr1_entry * entry)
{
    struct kaps_jr1_ad_db *ad_db;
    KAPS_JR1_CONVERT_AD_DB_SEQ_NUM_TO_PTR(device, ad, ad_db);

    if (ad_db->user_width_1 == 0)
    {
        ad_db->num_0b_entries--;
        return KAPS_JR1_OK;
    }

    kaps_jr1_sassert(ad->ref_count > 0);
    if (ad->ref_count == 1)
    {
        ad->entries = NULL;
    }
    else if (ad->ref_count == 2)
    {
        struct kaps_jr1_entry *other_e;
        struct kaps_jr1_db *db;

        KAPS_JR1_CONVERT_DB_SEQ_NUM_TO_PTR(device, entry, db) if (ad->entries[0] == entry)
            other_e = ad->entries[1];
        else
            other_e = ad->entries[0];
        db->device->alloc->xfree(db->device->alloc->cookie, ad->entries);
        ad->entries = (struct kaps_jr1_entry **) other_e;
    }
    else
    {
        int32_t i;

        for (i = 0; i < ad->ref_count - 1; i++)
        {
            if (ad->entries[i] == entry)
            {
                ad->entries[i] = ad->entries[ad->ref_count - 1];
                break;
            }
        }
        ad->entries[ad->ref_count - 1] = NULL;
    }

    ad->ref_count--;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_delete_unused_entries(
    struct kaps_jr1_ad_db * db)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_ad *e;
    struct kaps_jr1_c_list_iter it;

    KAPS_JR1_TRACE_IN("%p\n", db);
    if (!db)
        return KAPS_JR1_INVALID_ARGUMENT;

    KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, 0);

    KAPS_JR1_ASSERT_OR_ERROR((db->db_info.device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         db->db_info.device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    kaps_jr1_c_list_iter_init(&db->ad_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        e = DBLIST_TO_AD_ENTRY(el);
        if (!e->ref_count)
        {
            if (db->db_info.device->flags & KAPS_JR1_DEVICE_ISSU)
            {
                status =
                    kaps_jr1_ad_db_delete_entry_internal(db, KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_ad, e->user_handle), &it);
                KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, status);
                if (status != KAPS_JR1_OK)
                    return status;
            }
            else
            {
                status = kaps_jr1_ad_db_delete_entry_internal(db, e, &it);
                KAPS_JR1_TX_ERROR_CHECK(db->db_info.device, status);
                if (status != KAPS_JR1_OK)
                    return status;
            }
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ad_db_update_width(
    struct kaps_jr1_db * db,
    uint32_t new_width)
{
    struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) db;

    kaps_jr1_sassert(db->device->is_config_locked == 0);
    ad_db->db_info.width.ad_width_1 = new_width;
    ad_db->device_bytes = (new_width + 7) / KAPS_JR1_BITS_IN_BYTE;
    ad_db->byte_offset = ad_db->device_bytes - ad_db->user_bytes;
    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_ad_db_get_seq_num(
    struct kaps_jr1_db * db,
    struct kaps_jr1_ad_db * ad_db_p)
{
    struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;
    uint32_t seq_num = 0;

    while (ad_db)
    {
        if (ad_db == ad_db_p)
            break;
        seq_num++;
        ad_db = ad_db->next;
    }
    kaps_jr1_sassert(ad_db);
    return seq_num;
}

struct kaps_jr1_ad_db *
kaps_jr1_ad_get_ad_db_ptr(
    struct kaps_jr1_db *db,
    uint32_t expected_seq_num)
{
    struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;
    uint32_t seq_num = 0;

    while (ad_db)
    {
        if (seq_num == expected_seq_num)
            break;
        seq_num++;
        ad_db = ad_db->next;
    }
    kaps_jr1_sassert(ad_db);
    return ad_db;
}
