

#include "kaps_jr1_cr_pool_mgr.h"
#include "kaps_jr1_list.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_utility.h"

static uint32_t
kaps_jr1_cr_get_released_pool(
    struct kaps_jr1_cr_pool_mgr *mgr,
    uint32_t num_bytes_needed,
    uint32_t * released_pool_start_nv_offset,
    uint32_t * released_pool_end_nv_offset)
{
    uint32_t db_num, iter;
    uint32_t proper_fit_found;
    uint32_t size_in_bytes;
    uint32_t chosen_db_num = 0, chosen_pool_id = 0;
    uint32_t wasted_space_so_far = 0xFFFFFFFF;

    proper_fit_found = 0;

    for (db_num = 0; db_num < mgr->num_dbs; ++db_num)
    {

        for (iter = 0; iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; iter++)
        {

            if (mgr->pools_for_db[db_num].node_info[iter].in_use == 1 &&
                mgr->pools_for_db[db_num].node_info[iter].num_valid_entries == 0)
            {

                size_in_bytes = mgr->pools_for_db[db_num].node_info[iter].end_nv_offset -
                    mgr->pools_for_db[db_num].node_info[iter].start_nv_offset + 1;

                
                if (size_in_bytes >= num_bytes_needed)
                {

                    if (wasted_space_so_far == 0xFFFFFFFF || size_in_bytes - num_bytes_needed < wasted_space_so_far)
                    {
                        proper_fit_found = 1;
                        chosen_db_num = db_num;
                        chosen_pool_id = iter;
                        wasted_space_so_far = size_in_bytes - num_bytes_needed;
                    }
                }
            }

        }
    }

    if (proper_fit_found)
    {

        *released_pool_start_nv_offset = mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].start_nv_offset;
        *released_pool_end_nv_offset = mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].end_nv_offset;

        mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].in_use = 0;
        mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].end_nv_offset = 0;
        mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].start_nv_offset = 0;
        mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].start_user_handle = 0;
        mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].end_user_handle = 0;
        mgr->pools_for_db[chosen_db_num].node_info[chosen_pool_id].num_valid_entries = 0;

        return 1;
    }

    return 0;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_init(
    struct kaps_jr1_device * device,
    uint8_t * end_nv_ptr,
    struct kaps_jr1_cr_pool_mgr ** mgr_pp)
{
    struct kaps_jr1_cr_pool_mgr *mgr;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_db *db;
    uint32_t db_num, cur_size, iter;
    uint8_t *cur_nv_ptr = end_nv_ptr;

    
    cur_nv_ptr -= sizeof(struct kaps_jr1_cr_pool_mgr);
    mgr = (struct kaps_jr1_cr_pool_mgr *) (cur_nv_ptr + 1);

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);

    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            ++(mgr->num_dbs);
        }
    }

    
    cur_size = mgr->num_dbs * sizeof(struct kaps_jr1_cr_pools_for_db);
    cur_nv_ptr -= cur_size;
    mgr->pools_for_db = (struct kaps_jr1_cr_pools_for_db *) (cur_nv_ptr + 1);

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    db_num = 0;
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {

            mgr->pools_for_db[db_num].db = db;

            if (db->type == KAPS_JR1_DB_LPM)
            {
                mgr->pools_for_db[db_num].num_entries_per_pool = KAPS_JR1_CR_LPM_MAX_NUM_ENTRIES_PER_POOL;
            }

            mgr->pools_for_db[db_num].entry_size_8 = db->fn_table->cr_calc_entry_mem(db);

            for (iter = 0; iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; iter++)
            {
                mgr->pools_for_db[db_num].node_info[iter].pool_id = iter;
                mgr->pools_for_db[db_num].node_info[iter].in_use = 0;
                mgr->pools_for_db[db_num].node_info[iter].end_nv_offset = 0;
                mgr->pools_for_db[db_num].node_info[iter].start_nv_offset = 0;
                mgr->pools_for_db[db_num].node_info[iter].start_user_handle = 0;
                mgr->pools_for_db[db_num].node_info[iter].end_user_handle = 0;
                mgr->pools_for_db[db_num].node_info[iter].num_valid_entries = 0;
            }

            db_num++;

        }

    }

    mgr->free_end_nv_offset = (cur_nv_ptr - ((uint8_t *) device->nv_ptr));

    *mgr_pp = mgr;
    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_refresh_db_handles(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_cr_pool_mgr *cr_mgr = device->nv_mem_mgr->kaps_jr1_cr_pool_mgr;
    struct kaps_jr1_cr_pools_for_db *pools_for_db = cr_mgr->pools_for_db;
    int iter;

    for (iter = 0; iter < cr_mgr->num_dbs; iter++)
    {
        KAPS_JR1_STRY(kaps_jr1_db_refresh_handle(device, pools_for_db[iter].db, &pools_for_db[iter].db));
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_associate_user_handle(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    uint32_t user_handle,
    uint32_t * pool_id)
{
    uint32_t i, pool_iter;
    uint32_t found = 0, num_bytes_needed;
    struct kaps_jr1_cr_pool_node *cur_node;
    uint32_t released_pool_start_nv_offset = 0, released_pool_end_nv_offset = 0;
    uint32_t found_released_pool;

    *pool_id = KAPS_JR1_CR_INVALID_POOL_ID;

    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr");
        return KAPS_JR1_OK;
    }

    
    cur_node = &mgr->pools_for_db[i].node_info[0];
    found = 0;

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (cur_node->in_use == 0)
            continue;

        if (cur_node->start_user_handle <= user_handle && user_handle <= cur_node->end_user_handle)
        {
            found = 1;
            cur_node->num_valid_entries++;
            break;
        }

    }

    if (found)
    {
        *pool_id = cur_node->pool_id;
        return KAPS_JR1_OK;
    }

    mgr->pools_for_db[i].entry_size_8 = db->fn_table->cr_calc_entry_mem(db);
    num_bytes_needed = mgr->pools_for_db[i].entry_size_8 * mgr->pools_for_db[i].num_entries_per_pool;

    found_released_pool = 0;

    if (kaps_jr1_cr_pool_mgr_is_nv_exhausted(db->device, num_bytes_needed))
    {
        
        found_released_pool =
            kaps_jr1_cr_get_released_pool(mgr, num_bytes_needed, &released_pool_start_nv_offset,
                                      &released_pool_end_nv_offset);

        if (!found_released_pool)
        {
            *pool_id = KAPS_JR1_CR_INVALID_POOL_ID;
            return KAPS_JR1_OK;
        }
    }

    
    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (cur_node->in_use == 1)
            continue;

        if (found_released_pool)
        {
            cur_node->start_nv_offset = released_pool_start_nv_offset;
            cur_node->end_nv_offset = released_pool_end_nv_offset;
        }
        else
        {

            
            cur_node->start_nv_offset = mgr->free_end_nv_offset - num_bytes_needed + 1;
            cur_node->end_nv_offset = mgr->free_end_nv_offset;

            mgr->free_end_nv_offset -= num_bytes_needed;
        }

        cur_node->num_valid_entries = 1;

        cur_node->start_user_handle = (user_handle / mgr->pools_for_db[i].num_entries_per_pool)
            * mgr->pools_for_db[i].num_entries_per_pool;

        cur_node->end_user_handle = cur_node->start_user_handle + mgr->pools_for_db[i].num_entries_per_pool - 1;

        cur_node->in_use = 1;

        found = 1;

        *pool_id = pool_iter;

        break;
    }

    
    if (found == 0)
    {
        *pool_id = KAPS_JR1_CR_INVALID_POOL_ID;
        return KAPS_JR1_OK;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_disassociate_user_handle(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    uint32_t user_handle)
{
    uint32_t i, pool_iter;
    uint32_t found = 0;
    struct kaps_jr1_cr_pool_node *cur_node;

    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr\n");
        return KAPS_JR1_OK;
    }

    found = 0;

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (cur_node->in_use == 0)
            continue;

        if (cur_node->start_user_handle <= user_handle && user_handle <= cur_node->end_user_handle)
        {
            found = 1;
            (cur_node->num_valid_entries)--;
            break;
        }

    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the user handle in the CR pool Mgr\n");
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_get_num_slots(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    uint32_t * num_active_slots,
    uint32_t * num_total_slots)
{
    uint32_t i, pool_iter;
    uint32_t found = 0;
    struct kaps_jr1_cr_pool_node *cur_node;

    if (num_active_slots)
        *num_active_slots = 0;
    if (num_total_slots)
        *num_total_slots = 0;

    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr\n");
        return KAPS_JR1_OK;
    }

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (cur_node->in_use == 0)
            continue;
        if (num_active_slots)
            *num_active_slots += cur_node->num_valid_entries;
        if (num_total_slots)
            *num_total_slots += mgr->pools_for_db[i].num_entries_per_pool;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_get_entry_nv_ptr(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    uint32_t user_handle,
    uint8_t * found_p,
    uint8_t ** entry_nv_ptr)
{
    uint32_t i;
    uint32_t found = 0, pool_iter;
    struct kaps_jr1_cr_pool_node *cur_node;
    uint32_t offset;

    *found_p = 0;
    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr");
        return KAPS_JR1_OK;
    }

    cur_node = &mgr->pools_for_db[i].node_info[0];
    found = 0;

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (cur_node->in_use == 0)
            continue;

        if (cur_node->start_user_handle <= user_handle && user_handle <= cur_node->end_user_handle)
        {
            found = 1;
            break;
        }

    }

    if (found)
    {
        *found_p = 1;
        offset = (user_handle - cur_node->start_user_handle) * mgr->pools_for_db[i].entry_size_8;
        offset += cur_node->start_nv_offset;
        *entry_nv_ptr = ((uint8_t *) db->device->nv_ptr) + offset;
        return KAPS_JR1_OK;
    }

    *found_p = 0;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_get_user_handle(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    uint8_t * entry_nv_ptr,
    uint8_t * found_p,
    uint32_t * user_handle)
{
#if 0
    uint32_t i;
    uint32_t found = 0, pool_iter;
    struct kaps_jr1_cr_pool_node *cur_node;
    uint32_t offset;

    *found_p = 0;
    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr");
        return KAPS_JR1_OK;
    }

    cur_node = &mgr->pools_for_db[i].node_info[0];
    found = 0;

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (cur_node->in_use == 0)
            continue;

        if (cur_node->start_nv_ptr <= entry_nv_ptr && entry_nv_ptr <= cur_node->end_nv_ptr)
        {
            found = 1;
            break;
        }

    }

    if (found)
    {
        *found_p = 1;
        offset = entry_nv_ptr - cur_node->start_nv_ptr;
        *user_handle = cur_node->start_user_handle + (offset / mgr->pools_for_db[i].entry_size_8);
        return KAPS_JR1_OK;
    }
#endif
    *found_p = 0;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_get_user_handle_from_nv_location(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    uint32_t nv_location,
    uint8_t * found_p,
    uint32_t * user_handle)
{
    uint32_t i, found = 0;
    uint32_t pool_iter;
    struct kaps_jr1_cr_pool_node *cur_node;
    uint32_t pool_start, pool_end;
    uint32_t offset;

    *user_handle = 0;
    *found_p = 0;
    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr");
        return KAPS_JR1_OK;
    }

    pool_start = 0;
    pool_end = mgr->pools_for_db[i].num_entries_per_pool - 1;

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (!cur_node->in_use)
            continue;

        if (pool_start <= nv_location && nv_location <= pool_end)
        {
            offset = nv_location - (pool_iter * mgr->pools_for_db[i].num_entries_per_pool);
            *user_handle = cur_node->start_user_handle + offset;
            *found_p = 1;
            return KAPS_JR1_OK;
        }

        pool_start += mgr->pools_for_db[i].num_entries_per_pool;
        pool_end += mgr->pools_for_db[i].num_entries_per_pool;
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_iter_init(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    struct kaps_jr1_cr_pool_entry_iter ** iter)
{
    struct kaps_jr1_cr_pool_entry_iter *it;
    if (db->parent)
        db = db->parent;

    it = db->device->alloc->xcalloc(db->device->alloc->cookie, 1, sizeof(*it));
    if (!it)
        return KAPS_JR1_OUT_OF_MEMORY;
    *iter = it;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_iter_next(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    struct kaps_jr1_cr_pool_entry_iter * iter,
    uint8_t ** entry_nv_ptr)
{
    uint32_t i, found = 0;
    uint32_t pool_iter;
    struct kaps_jr1_cr_pool_node *cur_node;
    uint32_t pool_start, pool_end;
    uint32_t offset;

    *entry_nv_ptr = NULL;
    if (db->parent)
        db = db->parent;

    
    for (i = 0; i < mgr->num_dbs; ++i)
    {
        if (mgr->pools_for_db[i].db == db)
        {
            found = 1;
            break;
        }
    }

    if (!found)
    {
        kaps_jr1_assert(0, "Unable to find the db in CR Pool Mgr");
        return KAPS_JR1_OK;
    }

    pool_start = 0;
    pool_end = mgr->pools_for_db[i].num_entries_per_pool - 1;

    for (pool_iter = 0; pool_iter < KAPS_JR1_MAX_NUM_CR_POOLS_PER_DB; pool_iter++)
    {
        cur_node = &mgr->pools_for_db[i].node_info[pool_iter];

        if (!cur_node->in_use)
            continue;

        if (pool_start <= iter->entry_nr && iter->entry_nr <= pool_end)
        {
            offset = (iter->entry_nr - pool_start) * mgr->pools_for_db[i].entry_size_8;
            offset += cur_node->start_nv_offset;
            *entry_nv_ptr = ((uint8_t *) db->device->nv_ptr) + offset;

            (iter->entry_nr)++;

            return KAPS_JR1_OK;
        }

        pool_start += mgr->pools_for_db[i].num_entries_per_pool;
        pool_end += mgr->pools_for_db[i].num_entries_per_pool;
    }

    *entry_nv_ptr = NULL;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_iter_destroy(
    struct kaps_jr1_cr_pool_mgr * mgr,
    struct kaps_jr1_db * db,
    struct kaps_jr1_cr_pool_entry_iter * iter)
{
    db->device->alloc->xfree(db->device->alloc->cookie, iter);
    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_cr_pool_mgr_is_nv_exhausted(
    struct kaps_jr1_device * device,
    uint32_t num_bytes_needed)
{
    uint8_t *pending_nv_ptr = device->nv_ptr;
    uint32_t pending_nv_offset = 0;
    uint32_t offset = device->nv_mem_mgr->offset_device_pending_list, pending_count;

    pending_nv_ptr += offset;
    pending_count = *(uint32_t *) pending_nv_ptr;

    pending_nv_ptr += (pending_count * sizeof(uint32_t));

    pending_nv_offset = pending_nv_ptr - ((uint8_t *) device->nv_ptr);
    if ((device->nv_mem_mgr->kaps_jr1_cr_pool_mgr->free_end_nv_offset - pending_nv_offset + 1) >= num_bytes_needed)
        return 0;

    return 1;

}

kaps_jr1_status
kaps_jr1_cr_pool_mgr_get_stats(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_cr_pool_mgr *cr_mgr = device->nv_mem_mgr->kaps_jr1_cr_pool_mgr;
    uint8_t *start_nv_ptr = device->nv_ptr;
    float device_size, pool_mgr_size;
    uint32_t offset = device->nv_mem_mgr->offset_device_pending_list, pending_count;
    uint8_t *pending_nv_ptr = device->nv_ptr;

    pending_nv_ptr += offset;
    pending_count = *(uint32_t *) pending_nv_ptr;
    pending_nv_ptr += (pending_count * sizeof(uint32_t));

    device_size = (float) (pending_nv_ptr - start_nv_ptr + 1) / 1024.0;
    pool_mgr_size = (float) (device->nv_size - cr_mgr->free_end_nv_offset) / 1024.0;

    kaps_jr1_printf("NV Stats : \n");
    kaps_jr1_printf("\tTotal NV Allocated           : %.2f KB\n", device->nv_size / 1024.0);
    kaps_jr1_printf("\tNV Allocated for Device Info : %.2f KB\n", device_size);
    kaps_jr1_printf("\tNV Allocated for CR_POOL_MGR : %.2f KB\n", pool_mgr_size);
    kaps_jr1_printf("\tTotal NV Used                : %.2f KB\n", device_size + pool_mgr_size);
    kaps_jr1_printf("\tTotal NV Remained            : %.2f KB\n",
                (device->nv_size / 1024.0) - (device_size + pool_mgr_size));

    return KAPS_JR1_OK;

}
