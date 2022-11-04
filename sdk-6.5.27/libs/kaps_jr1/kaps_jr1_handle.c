
#include <stdlib.h>
#include "kaps_jr1_handle.h"

kaps_jr1_status
kaps_jr1_db_user_handle_table_realloc(
    struct kaps_jr1_db *db)
{
    void **tmp;
    uintptr_t iter;
    uintptr_t curr_size = *db->common_info->user_hdl_table_size + 1;
    uintptr_t size;
    uintptr_t *freeptr;

    kaps_jr1_sassert(curr_size < HANDLE_MAX_TABLE_SIZE);    
    size = curr_size * 1.5;
    if (size > HANDLE_MAX_TABLE_SIZE)
        size = HANDLE_MAX_TABLE_SIZE;
    if (size < 4096)
        size = 4096;

    tmp = db->device->alloc->xcalloc(db->device->alloc->cookie, size, sizeof(void *));

    if (!tmp)
        return KAPS_JR1_OUT_OF_MEMORY;

    freeptr = (uintptr_t *) & tmp[0];

    
    for (iter = 0; iter < curr_size; iter++)
    {
        tmp[iter] = db->common_info->user_hdl_to_entry[iter];
    }

    
    if (tmp[0] != (void*)0xFFFFFFFF)
    {
        for (iter = curr_size; iter < size - 1; iter++)
        {
            tmp[iter] = (void *) (((iter + 1) << 4) | 0x1);
        }
        tmp[size - 1] = (void *) (((*freeptr) << 4) | 0x1);
        *freeptr = curr_size;
    }

    *db->common_info->user_hdl_table_size = size - 1;
    db->device->alloc->xfree(db->device->alloc->cookie, db->common_info->user_hdl_to_entry);
    db->common_info->user_hdl_to_entry = tmp;

    
    if (db->type == KAPS_JR1_DB_AD)
    {
        struct kaps_jr1_db *main_db = db->common_info->ad_info.db;
        struct kaps_jr1_ad_db *ad_db = (struct kaps_jr1_ad_db *) main_db->common_info->ad_info.ad;

        while (ad_db)
        {
            ad_db->db_info.common_info->user_hdl_table_size = db->common_info->user_hdl_table_size;
            ad_db->db_info.common_info->user_hdl_to_entry = db->common_info->user_hdl_to_entry;
            ad_db = ad_db->next;
        }
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_user_handle_table_init(
    struct kaps_jr1_allocator * alloc,
    struct kaps_jr1_db * db,
    uint32_t capacity)
{
    uint32_t size;

    kaps_jr1_sassert(capacity < HANDLE_MAX_TABLE_SIZE);     

    size = capacity;

    if (size > HANDLE_MAX_TABLE_SIZE)
        size = HANDLE_MAX_TABLE_SIZE;

    if (size == 0)
        size = 4096;

    db->common_info->user_hdl_table_size = alloc->xcalloc(alloc->cookie, 1, sizeof(uint32_t));
    if (!db->common_info->user_hdl_table_size)
        return KAPS_JR1_OUT_OF_MEMORY;

    db->common_info->user_hdl_to_entry = alloc->xcalloc(alloc->cookie, size, sizeof(void *));
    if (!db->common_info->user_hdl_to_entry)
    {
        alloc->xfree(alloc->cookie, db->common_info->user_hdl_table_size);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    *db->common_info->user_hdl_table_size = size - 1;   
    db->common_info->user_hdl_to_entry[0] = (void *) 0xFFFFFFFF;        
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_user_handle_update_freelist(
    struct kaps_jr1_db * db)
{
    void **entry_pp = db->common_info->user_hdl_to_entry;
    uint32_t i = *db->common_info->user_hdl_table_size; 
    uintptr_t *freeptr = (uintptr_t *) & entry_pp[0];

    kaps_jr1_sassert(db->common_info->user_hdl_to_entry[0] == (void *) 0xFFFFFFFF);

    *freeptr = 0;
    while (i > 0)
    {
        if (entry_pp[i] == NULL)
        {
            entry_pp[i] = (void *) ((*freeptr) << 4 | 0x1);
            *freeptr = i;
        }
        i--;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_db_get_user_handle(
    uint32_t * user_handle,
    struct kaps_jr1_db * db)
{
    uintptr_t *freeptr = (uintptr_t *) & db->common_info->user_hdl_to_entry[0];

    if (db->common_info->insertion_mode == KAPS_JR1_DB_INSERTION_MODE_ENTRY_WITH_INDEX)
    {
        uintptr_t iter = *db->common_info->user_hdl_table_size;

        for (; iter > 0; iter--)
        {
            if (KAPS_JR1_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, iter))
                break;
        }

        if (iter != 0)
        {
            *user_handle = (uint32_t) iter;
            return KAPS_JR1_OK;
        }

        KAPS_JR1_STRY(kaps_jr1_db_user_handle_table_realloc(db));
        iter = *db->common_info->user_hdl_table_size;
        kaps_jr1_sassert(KAPS_JR1_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, iter));
        *user_handle = (uint32_t) iter;
        return KAPS_JR1_OK;
    }

    if (*freeptr == 0)
    {
        KAPS_JR1_STRY(kaps_jr1_db_user_handle_table_realloc(db));
        freeptr = (uintptr_t *) & db->common_info->user_hdl_to_entry[0];
        kaps_jr1_sassert(*freeptr != 0);
    }

    *user_handle = (uint32_t) * freeptr;
    *freeptr = ((uintptr_t) db->common_info->user_hdl_to_entry[*user_handle]) >> 4;

    return KAPS_JR1_OK;
}
