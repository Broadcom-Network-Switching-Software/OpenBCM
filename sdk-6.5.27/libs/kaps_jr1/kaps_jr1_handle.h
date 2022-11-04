

#ifndef __KAPS_JR1_HANDLE_H
#define __KAPS_JR1_HANDLE_H

#include <stdint.h>
#include "kaps_jr1_errors.h"
#include "kaps_jr1_legacy.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_db_internal.h"
#include "kaps_jr1_instruction_internal.h"
#include "kaps_jr1_ad_internal.h"
#ifdef __cplusplus
extern "C"
{
#endif




    kaps_jr1_status kaps_jr1_db_user_handle_table_realloc(
    struct kaps_jr1_db *db);


    kaps_jr1_status kaps_jr1_db_user_handle_table_init(
    struct kaps_jr1_allocator *alloc,
    struct kaps_jr1_db *db,
    uint32_t capacity);


    kaps_jr1_status kaps_jr1_db_get_user_handle(
    uint32_t * user_handle,
    struct kaps_jr1_db *db);


    kaps_jr1_status kaps_jr1_db_user_handle_update_freelist(
    struct kaps_jr1_db *db);

#define FREE_PTR(db) ((db)->common_info->user_hdl_to_entry[0])
#define FREE_MASK (0xF)
#define HANDLE_MAX_TABLE_SIZE (0x7FFFFFF)

#define KAPS_JR1_WB_HANDLE_TABLE_IS_LOCATION_FREE(db, user_handle) \
    (((((uintptr_t)db->common_info->user_hdl_to_entry[user_handle]) & FREE_MASK) == 0x1) ? 1 : 0) \

#define KAPS_JR1_WB_HANDLE_WRITE_LOC(db, wb_handle, usr_hdl)                               \
    do {                                                                              \
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU) {                                    \
            while (usr_hdl > (*db->common_info->user_hdl_table_size)) {               \
                KAPS_JR1_WB_HANDLE_TABLE_GROW(db);                                         \
            }                                                                         \
            if ((FREE_PTR(db) == (void *)0xFFFFFFFF))                                  \
                kaps_jr1_sassert(db->common_info->user_hdl_to_entry[usr_hdl] == NULL);     \
            else if ((((uintptr_t)db->common_info->user_hdl_to_entry[usr_hdl]) & FREE_MASK) != 0x1)  \
                return KAPS_JR1_DB_WB_LOCATION_BUSY;                                       \
            db->common_info->user_hdl_to_entry[usr_hdl] = wb_handle;                  \
            wb_handle->user_handle = usr_hdl;                                         \
        }                                                                                 \
    } while (0)

#define KAPS_JR1_WB_HANDLE_UPDATE_LOC(db, wb_handle, user_handle)                              \
    do {                                                                                  \
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU) {                                        \
            kaps_jr1_sassert((((uintptr_t)db->common_info->user_hdl_to_entry[user_handle]) & FREE_MASK) != 0x1);  \
            db->common_info->user_hdl_to_entry[user_handle] = wb_handle;                  \
            wb_handle->user_handle = user_handle;                                         \
        }                                                                                 \
    } while (0)

#define KAPS_JR1_WB_HANDLE_READ_LOC(db, wb_handle, user_handle)                                \
    do {                                                                                  \
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU){                                         \
            *wb_handle = db->common_info->user_hdl_to_entry[user_handle];                 \
            if (((uintptr_t)(*wb_handle)) & 0x1)                                                         \
                *wb_handle = NULL;                                                        \
        }                                                                                 \
        else                                                                              \
            *wb_handle = (void *)user_handle;                                             \
    } while (0)

#define KAPS_JR1_WB_HANDLE_DELETE_LOC(db, user_handle)                                         \
    do {                                                                                  \
        if ((db)->device->flags & KAPS_JR1_DEVICE_ISSU) {                                      \
            kaps_jr1_sassert((((uintptr_t)(db)->common_info->user_hdl_to_entry[(uint32_t)user_handle]) & FREE_MASK) != 0x1); \
            if ((FREE_PTR(db) == (void *)0xFFFFFFFF)) {                                    \
                (db)->common_info->user_hdl_to_entry[(uint32_t)user_handle] = NULL;       \
            } else {                                                                      \
                (db)->common_info->user_hdl_to_entry[(uint32_t)user_handle] =(void *) ((uintptr_t)FREE_PTR(db) << 4 | 0x1); \
                FREE_PTR(db) = (void *)user_handle;                                       \
                kaps_jr1_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
            }                                                                             \
        }                                                                                 \
    } while (0)

#define KAPS_JR1_WB_HANDLE_TABLE_DESTROY(db)                                                   \
    do {                                                                                  \
        if ((db)->device->flags & KAPS_JR1_DEVICE_ISSU) {                                      \
            (db)->device->alloc->xfree((db)->device->alloc->cookie, (db)->common_info->user_hdl_table_size);   \
            (db)->device->alloc->xfree((db)->device->alloc->cookie, (db)->common_info->user_hdl_to_entry);   \
        }                                                                                 \
    } while (0)                                                                            \

#define KAPS_JR1_WB_HANDLE_TABLE_GROW(db)                                                      \
    do {                                                                                  \
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)                                          \
            KAPS_JR1_STRY(kaps_jr1_db_user_handle_table_realloc(db));\
    } while (0)                                                                             \

#define KAPS_JR1_WB_HANDLE_TABLE_INIT(db, capacity)                                             \
    do {                                                                                  \
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU)                                          \
            KAPS_JR1_STRY(kaps_jr1_db_user_handle_table_init(db->device->alloc, db, capacity));     \
    } while (0)                                                                             \

#define KAPS_JR1_WB_HANDLE_TABLE_FREELIST_INIT(db)                                             \
    do {                                                                                  \
        if ((db)->device->flags & KAPS_JR1_DEVICE_ISSU) {                                      \
            KAPS_JR1_STRY(kaps_jr1_db_user_handle_update_freelist(db));                             \
            kaps_jr1_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
        }                                                                                 \
    } while (0)

#define KAPS_JR1_WB_HANDLE_GET_FREE_AND_WRITE(db, entry)                          \
    do {                                                                                  \
        if (db->device->flags & KAPS_JR1_DEVICE_ISSU) {                                        \
            uint32_t user_handle = 0;                                                     \
            KAPS_JR1_STRY(kaps_jr1_db_get_user_handle((&user_handle), db));                          \
            entry->user_handle = user_handle;                                              \
            kaps_jr1_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
            db->common_info->user_hdl_to_entry[user_handle] = (void *)(entry);            \
            entry = (void *) ((uintptr_t)user_handle);                                    \
        }                                                                                 \
    } while (0)

#define KAPS_JR1_WB_HANDLE_WRITE(db, entry, user_handle)                          \
            do {                                                                                  \
                if (db->device->flags & KAPS_JR1_DEVICE_ISSU) {                                        \
                    entry->user_handle = user_handle;                                              \
                    kaps_jr1_sassert(((uintptr_t)FREE_PTR(db)) <= (uintptr_t)(*((db)->common_info->user_hdl_table_size))); \
                    db->common_info->user_hdl_to_entry[user_handle] = (void *)(entry);            \
                }                                                                                 \
            } while (0)

#define KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(type, wb_handle) \
            ((struct type *)((uintptr_t)(wb_handle)))

#define KAPS_JR1_WB_HANDLE_TABLE_IS_FREELIST_INIT_DONE(db) \
        ((db->common_info->user_hdl_to_entry[0] == (void *)0xFFFFFFFF) ? 0 : 1)

#ifdef __cplusplus
}
#endif

#endif                          
