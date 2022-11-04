

#ifndef __KAPS_JR1_SIMPLE_DBA_H
#define __KAPS_JR1_SIMPLE_DBA_H

#include "kaps_jr1_bitmap.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_fibmgr.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_jr1_db;

#define MAX_NUM_RPT_ENTRIES_IN_POOL 256

    struct kaps_jr1_simple_dba_range_for_pref_len
    {
        int32_t first_row;
        int32_t last_row;
        int32_t prefix_len;
        struct kaps_jr1_simple_dba_range_for_pref_len *next;
        struct kaps_jr1_simple_dba_range_for_pref_len *prev;
    };

    struct kaps_jr1_simple_dba_node
    {
        struct kaps_jr1_simple_dba_range_for_pref_len *chunks;
        struct kaps_jr1_fast_bitmap used;
        uint8_t we_are_in_pre_insert_mode;
        uint8_t pre_inserted_entries_not_placed_yet;
        uint8_t alloced;
        void (
    *entry_move_callback) (
    void *,
    void *,
    uint32_t,
    uint32_t);
    };

    struct kaps_jr1_simple_dba
    {
        struct kaps_jr1_device *device;
        struct kaps_jr1_allocator *alloc;
        struct kaps_jr1_fast_bitmap free;
        struct kaps_jr1_simple_dba_node nodes[MAX_NUM_RPT_ENTRIES_IN_POOL];
        void **pfx_bundles;

        uint32_t pool_start_loc;
        uint32_t pool_size;
        uint32_t free_entries;
        uint32_t dba_no;
        uint32_t is_clone;
    };

    enum kaps_jr1_dba_trigger_compression_mode
    {
        DBA_NO_TRIGGER_COMPRESSION = 0,
        DBA_TRIGGER_COMPRESSION_1 = 1, 
        DBA_TRIGGER_COMPRESSION_2 = 2, 
    };

    struct kaps_jr1_fib_dba
    {
        struct kaps_jr1_simple_dba kaps_jr1_simple_dba;

        kaps_jr1_fib_tbl_mgr *m_fib_tbl_mgr;

        struct kaps_jr1_ab_info *m_ab_info;

        uint32_t pool_index;

        void *defaultRptEntry_p;

        enum kaps_jr1_dba_trigger_compression_mode mode;

        uint8_t m_start_offset_1;

        uint8_t m_num_bytes;

        uint8_t m_is_delete_op;

        uint8_t m_is_cur_active;

        uint8_t m_curDbId;

        uint8_t m_is_extra_byte_added;

        uint32_t cb_result;

        uint32_t cb_reason;
    };

    kaps_jr1_status kaps_jr1_simple_dba_find_place_for_entry(
    struct kaps_jr1_simple_dba *m,
    void *handle,
    int32_t group_id,
    int32_t len,
    int32_t * result);

    void kaps_jr1_simple_dba_place_entry_at_loc(
    struct kaps_jr1_simple_dba *m,
    void *handle,
    int32_t group_id,
    int32_t len,
    uint32_t pos);


    void kaps_jr1_simple_dba_pre_insert_entry(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id,
    int32_t len);


    void kaps_jr1_simple_dba_convert_ranges_from_pre_insert(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id);


    int32_t kaps_jr1_simple_dba_find_place_for_pre_inserted_entry(
    struct kaps_jr1_simple_dba *m,
    void *handle,
    int32_t group_id,
    int32_t len);

    void kaps_jr1_simple_dba_free_entry(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id,
    int32_t len,
    uint32_t position);

    void kaps_jr1_simple_dba_replace_entry(
    struct kaps_jr1_simple_dba *m,
    void *handle,
    int32_t group_id,
    uint32_t pos);

    void *kaps_jr1_simple_dba_get_entry(
    struct kaps_jr1_simple_dba *m,
    uint32_t pos);

    struct kaps_jr1_simple_dba *kaps_jr1_simple_dba_init(
    struct kaps_jr1_device *device,
    uint32_t pool_start,
    uint32_t pool_size,
    uint32_t dba_no,
    uint32_t is_clone);

    void kaps_jr1_simple_dba_destroy(
    struct kaps_jr1_simple_dba *m);

    kaps_jr1_status kaps_jr1_simple_dba_init_node(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id,
    void (*entry_move_callback) (void *,
                                 void *,
                                 uint32_t,
                                 uint32_t));

    void kaps_jr1_simple_dba_free_node(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id);

    uint32_t kaps_jr1_simple_dba_verify(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id,
    int32_t(*get_prio_length) (void *));

    void *kaps_jr1_simple_dba_search(
    struct kaps_jr1_simple_dba *m,
    int32_t group_id,
    struct kaps_jr1_db *db,
    uint8_t * key,
    int32_t(*compare_fn) (struct kaps_jr1_db * db,
                          uint8_t * key,
                          void *user_data));

    void kaps_jr1_simple_dba_repair_entry(
    struct kaps_jr1_simple_dba *m,
    uint32_t row_nr,
    uint32_t * found);

#ifdef __cplusplus
}
#endif

#endif
