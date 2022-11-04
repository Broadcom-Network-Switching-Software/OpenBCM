

#include <stdio.h>

#include "kaps_jr1_simple_dba.h"
#include "kaps_jr1_algo_common.h"
#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_dma.h"

struct kaps_jr1_db_bb_info
{
    int8_t start_lpu;
    int8_t end_lpu;
    int8_t lsn_size;
};

static kaps_jr1_status
kaps_jr1_fill_lpm_granularities(
    struct kaps_jr1_device *device)
{
    
    uint16_t lsn_grans_kaps_jr1[] = { 8, 16, 24, 32, 40, 48, 56, 72, 96, 136, 168 };

    
    device->hw_res->lpm_num_gran = 11;
    kaps_jr1_memcpy(device->hw_res->lpm_gran_array, lsn_grans_kaps_jr1, sizeof(lsn_grans_kaps_jr1));

    device->hw_res->lpm_middle_level_num_gran = 11;
    kaps_jr1_memcpy(device->hw_res->lpm_middle_level_gran_array, lsn_grans_kaps_jr1, sizeof(lsn_grans_kaps_jr1));


    return NLMERR_OK;
}

kaps_jr1_status
kaps_jr1_device_check_lpm_constraints_single_device(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;
    struct kaps_jr1_db *parent;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        parent = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

        if (parent->type != KAPS_JR1_DB_LPM)
            continue;

        
        if (device->hw_res->no_overflow_lpm)
        {
            parent->hw_res.db_res->num_dba_dt = 0;
        }

    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_check_lpm_constraints(
    struct kaps_jr1_device * device)
{
    while (device)
    {
        KAPS_JR1_STRY(kaps_jr1_device_check_lpm_constraints_single_device(device));
        device = device->next_bc_device;
    }
    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_destroy_shadow(
    struct kaps_jr1_device *device)
{
    uint32_t i, j, k;
    struct kaps_jr1_shadow_device *shadow = device->kaps_jr1_shadow;
    uint32_t ads_depth;

    (void) j;
    (void) k;

    if (device->nv_ptr)
    {
        device->alloc->xfree(device->alloc->cookie, shadow);
        return KAPS_JR1_OK;
    }

    if (shadow)
    {
        if (shadow->rpb_blks)
        {
            for (i = 0; i < device->hw_res->num_rpb_blks; ++i)
            {
                device->alloc->xfree(device->alloc->cookie, shadow->rpb_blks[i].rpb_rows);
            }

            device->alloc->xfree(device->alloc->cookie, shadow->rpb_blks);
        }

        if (shadow->ads_blks)
        {
            for (i = 0; i < device->hw_res->num_rpb_blks; ++i)
            {
                device->alloc->xfree(device->alloc->cookie, shadow->ads_blks[i].ads_rows);
            }

            device->alloc->xfree(device->alloc->cookie, shadow->ads_blks);
        }

        if (shadow->bkt_blks)
        {
            for (i = 0; i < device->hw_res->total_num_bb; ++i)
            {
                if (shadow->bkt_blks[i].bkt_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks[i].bkt_rows);
                }
            }
            device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks);
        }

        if (shadow->small_bbs)
        {
            for (i = 0; i < device->hw_res->total_small_bb; ++i)
            {
                if (shadow->small_bbs[i].bkt_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->small_bbs[i].bkt_rows);
                }
            }
            device->alloc->xfree(device->alloc->cookie, shadow->small_bbs);
        }

        if (shadow->ads2_blks)
        {
            for (i = 0; i < device->hw_res->num_ads2_blks; ++i)
            {
                if (shadow->ads2_blks[i].ads_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->ads2_blks[i].ads_rows);
                }
            }

            device->alloc->xfree(device->alloc->cookie, shadow->ads2_blks);
        }


        if (shadow->ads2_overlay)
        {
            for (i = 0; i < device->hw_res->num_ads2_blks; ++i)
            {
                ads_depth = device->hw_res->ads2_depth[i];

                if (ads_depth)
                {
                    if (shadow->ads2_overlay[i].x_table)
                    {
                        device->alloc->xfree(device->alloc->cookie, shadow->ads2_overlay[i].x_table);
                    }

                    kaps_jr1_free_bmp(&shadow->ads2_overlay[i].it_fbmp, device->alloc);
                }
            }

            device->alloc->xfree(device->alloc->cookie, shadow->ads2_overlay);
        }

        if (shadow->ab_to_small_bb)
        {
            device->alloc->xfree(device->alloc->cookie, shadow->ab_to_small_bb);
        }

        device->alloc->xfree(device->alloc->cookie, shadow);
    }
    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_initialize_shadow(
    struct kaps_jr1_device *device)
{
    uint32_t i, j, k;

    struct kaps_jr1_shadow_device *shadow = device->alloc->xcalloc(device->alloc->cookie, 1,
                                                               sizeof(struct kaps_jr1_shadow_device));
    (void) j;
    (void) k;

    if (!shadow)
        return KAPS_JR1_OUT_OF_MEMORY;

    shadow->rpb_blks
        = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->num_rpb_blks, sizeof(struct kaps_jr1_shadow_rpb));

    if (!shadow->rpb_blks)
    {
    	kaps_jr1_destroy_shadow(device);
    	return KAPS_JR1_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->hw_res->num_rpb_blks; ++i)
    {
        shadow->rpb_blks[i].rpb_rows = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->num_rows_in_rpb[i],
                                        sizeof(struct kaps_jr1_rpb_entry));

        if (!shadow->rpb_blks[i].rpb_rows)
        {
            kaps_jr1_destroy_shadow(device);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    shadow->ads_blks
        =  device->alloc->xcalloc(device->alloc->cookie, device->hw_res->num_rpb_blks, sizeof(struct kaps_jr1_shadow_ads));

    if (!shadow->ads_blks)
    {
        kaps_jr1_destroy_shadow(device);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->hw_res->num_rpb_blks; ++i)
    {
        shadow->ads_blks[i].ads_rows
            = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->num_rows_in_rpb[i],
                                sizeof(struct kaps_jr1_ads));

        if (!shadow->ads_blks[i].ads_rows)
        {
            kaps_jr1_destroy_shadow(device);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    if (device->hw_res->total_num_bb)
    {
        shadow->bkt_blks
            = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->total_num_bb, sizeof(struct kaps_jr1_shadow_bkt));

        if (!shadow->bkt_blks)
        {
            kaps_jr1_destroy_shadow(device);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    for (i = 0; i < device->hw_res->total_num_bb; ++i)
    {
        if (device->hw_res->num_rows_in_bb)
        {
            shadow->bkt_blks[i].bkt_rows = device->alloc->xcalloc(device->alloc->cookie,
                                                                  device->hw_res->num_rows_in_bb,
                                                                  sizeof(struct kaps_jr1_shadow_bkt_row));

            if (!shadow->bkt_blks[i].bkt_rows)
            {
                kaps_jr1_destroy_shadow(device);
                return KAPS_JR1_OUT_OF_MEMORY;
            }
        }
    }

    if (device->hw_res->total_small_bb)
    {
        shadow->small_bbs = device->alloc->xcalloc(device->alloc->cookie, device->hw_res->total_small_bb,
                                                   sizeof(struct kaps_jr1_shadow_bkt));
        if (!shadow->small_bbs)
        {
            kaps_jr1_destroy_shadow(device);
            return KAPS_JR1_OUT_OF_MEMORY;
        }

        for (i = 0; i < device->hw_res->total_small_bb; ++i)
        {
            shadow->small_bbs[i].bkt_rows = device->alloc->xcalloc(device->alloc->cookie,
                                                                   device->hw_res->num_rows_in_small_bb,
                                                                   sizeof(struct kaps_jr1_shadow_bkt_row));
            if (!shadow->small_bbs[i].bkt_rows)
            {
                kaps_jr1_destroy_shadow(device);
                return KAPS_JR1_OUT_OF_MEMORY;
            }
        }
    }


    device->kaps_jr1_shadow = shadow;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_initialize_advanced_shadow(
    struct kaps_jr1_device * device)
{
    uint32_t i;
    struct kaps_jr1_shadow_device *shadow = device->kaps_jr1_shadow;

    if (device->nv_ptr)
    {
        shadow->rpb_blks = (struct kaps_jr1_shadow_rpb *) device->nv_ptr;
        device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + (device->num_ab * sizeof(struct kaps_jr1_shadow_rpb)));

        for (i = 0; i < device->num_ab; ++i)
        {
            shadow->rpb_blks[i].rpb_rows = (struct kaps_jr1_rpb_entry *) device->nv_ptr;
            device->nv_ptr =
                (void *) ((uint8_t *) device->nv_ptr + (device->hw_res->num_rows_in_rpb[i] * sizeof(struct kaps_jr1_rpb_entry)));
        }

        shadow->ads_blks = (struct kaps_jr1_shadow_ads *) device->nv_ptr;
        device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + (device->num_ab * sizeof(struct kaps_jr1_shadow_ads)));

        for (i = 0; i < device->num_ab; ++i)
        {
            shadow->ads_blks[i].ads_rows = (struct kaps_jr1_ads *) device->nv_ptr;
            device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + (device->hw_res->num_rows_in_rpb[i] * sizeof(struct kaps_jr1_ads)));
        }

        if (shadow->bkt_blks)
        {
            for (i = 0; i < device->hw_res->total_num_bb; ++i)
            {
                if (shadow->bkt_blks[i].bkt_rows)
                {
                    device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks[i].bkt_rows);
                }
            }
            device->alloc->xfree(device->alloc->cookie, shadow->bkt_blks);
        }

        shadow->bkt_blks = (struct kaps_jr1_shadow_bkt *) device->nv_ptr;
        device->nv_ptr =
            (void *) ((uint8_t *) device->nv_ptr + (device->hw_res->total_num_bb * sizeof(struct kaps_jr1_shadow_bkt)));

        for (i = 0; i < device->hw_res->total_num_bb; ++i)
        {
            shadow->bkt_blks[i].bkt_rows = (struct kaps_jr1_shadow_bkt_row *) device->nv_ptr;
            device->nv_ptr =
                (void *) ((uint8_t *) device->nv_ptr +
                          (device->hw_res->num_rows_in_bb * sizeof(struct kaps_jr1_shadow_bkt_row)));
        }

    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_save_shadow(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_shadow_device *shadow = device->kaps_jr1_shadow;
    int32_t i;
    int32_t size_of_bb;

    kaps_jr1_assert(shadow->bkt_blks, "Shadow NULL!!\n");

    if (wb_fun->write_fn)
    {
        size_of_bb = device->hw_res->num_rows_in_bb * sizeof(struct kaps_jr1_shadow_bkt_row);

        for (i = 0; i < device->hw_res->total_num_bb; ++i)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) shadow->bkt_blks[i].bkt_rows,
                                      size_of_bb, *wb_fun->nv_offset))
            {
                return KAPS_JR1_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + size_of_bb;
        }

    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_restore_shadow(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_shadow_device *shadow = device->kaps_jr1_shadow;
    int32_t i;
    int32_t size_of_bb;

    kaps_jr1_assert(shadow->bkt_blks, "Shadow NULL!!\n");

    if (wb_fun->read_fn)
    {
        size_of_bb = device->hw_res->num_rows_in_bb * sizeof(struct kaps_jr1_shadow_bkt_row);

        for (i = 0; i < device->hw_res->total_num_bb; ++i)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) shadow->bkt_blks[i].bkt_rows,
                                     size_of_bb, *wb_fun->nv_offset))
            {
                return KAPS_JR1_NV_READ_WRITE_FAILED;
            }
            *wb_fun->nv_offset = *wb_fun->nv_offset + size_of_bb;
        }

    }

    return KAPS_JR1_OK;
}



kaps_jr1_status
kaps_jr1_device_advanced_init(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_device *tmp;
    struct kaps_jr1_device_resource *hw_res = NULL;
    uint32_t i = 0;
    uint32_t num_rows_in_rpb;


    switch (device->type)
    {

        case KAPS_JR1_DEVICE:
            hw_res = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(*hw_res));
            if (hw_res == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;
            device->hw_res = hw_res;

            hw_res->num_rpb_blks = 4;

            num_rows_in_rpb = 2048;

            
            hw_res->total_lpus = 16;
            hw_res->max_lpu_per_db = 16;

            hw_res->total_num_bb = 32;
            hw_res->num_bb_in_one_bb_cascade = 2;

            hw_res->inplace_width_1 = 20;
            hw_res->num_rows_in_bb = 1024;
            
            hw_res->max_num_uda_chunk_levels = 28;

            hw_res->num_algo_levels = 2;
            hw_res->num_db_in_one_group = 2;
            hw_res->num_daisy_chains = 1;

            hw_res->ads_width_1 = 128;
            hw_res->ads_width_8 = 16;

            hw_res->num_ads2_blks = 0;
            hw_res->is_hw_mapped_ix = 1;

            device->uda_offset = 5;
            device->is_bb_compaction_enabled = 1;

            if (device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
            {
                hw_res->total_num_bb = 16;
                hw_res->num_bb_in_one_bb_cascade = 1;
            }

            if (device->id == KAPS_JR1_QUX_DEVICE_ID)
            {
                num_rows_in_rpb = 512;

                hw_res->total_num_bb = 16;
                hw_res->num_bb_in_one_bb_cascade = 1;
                hw_res->num_rows_in_bb = 256;
            }

            if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
            {
                
                num_rows_in_rpb = 2 * 4096;

                if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
                {
                    num_rows_in_rpb = 2048;
                }
                else if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
                {
                    num_rows_in_rpb = 2048 + 256;
                }

                
                hw_res->num_rows_in_bb = 4 * 1024;

                if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
                {
                    hw_res->num_rows_in_bb = 2 * 1024;
                }
                else if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
                {
                    hw_res->num_rows_in_bb = (2 * 1024) + 256;
                }

                hw_res->max_lpu_per_db = 16;
            }


            for (i = 0; i < hw_res->num_rpb_blks; ++i)
            {
                hw_res->num_rows_in_rpb[i] = num_rows_in_rpb;
            }


            hw_res->sram_dt_per_db = 1;
            hw_res->bb_width_1 = KAPS_JR1_BKT_WIDTH_1;
            hw_res->has_algorithmic_lpm = 1;
            kaps_jr1_initialize_shadow(device);
            break;

        default:
            return KAPS_JR1_INVALID_DEVICE_TYPE;
    }

    for (tmp = device; tmp; tmp = tmp->next_dev)
        kaps_jr1_fill_lpm_granularities(tmp);

    
    kaps_jr1_sassert((hw_res->sram_dt_per_db + hw_res->dba_dt_per_db) <= KAPS_JR1_HW_MAX_DT_PER_DB);
    kaps_jr1_sassert(hw_res->max_lpu_per_db <= KAPS_JR1_HW_MAX_LPUS_PER_DB);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_advanced_destroy(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_JR1_DB_DMA)
            continue;

        kaps_jr1_c_list_remove_node(&device->db_list, el, &it);
        KAPS_JR1_STRY(kaps_jr1_dma_db_destroy((struct kaps_jr1_dma_db *) db));
    }

    if (device->type == KAPS_JR1_DEVICE)
        kaps_jr1_destroy_shadow(device);

    if (device->hw_res)
        device->alloc->xfree(device->alloc->cookie, device->hw_res);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_advanced_set_property(
    struct kaps_jr1_device * device,
    uint32_t property,
    va_list a_list)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint32_t value;

    switch (property)
    {
        case KAPS_JR1_DEVICE_PROP_ADV_PRINT:
        {
            struct kaps_jr1_device *tmp_dev = NULL;

            value = va_arg(a_list, uint32_t);
            KAPS_JR1_TRACE_PRINT(" %d", value);

            tmp_dev = device;
            if (device->main_bc_device)
                tmp_dev = device->main_bc_device;

            while (tmp_dev)
            {
                tmp_dev->hw_res->device_print_advance = value;
                tmp_dev = tmp_dev->next_bc_device;
            }
            break;
        }

        case KAPS_JR1_DEVICE_PROP_ENABLE_COMPACTION:
        {
            value = va_arg(a_list, uint32_t);
            KAPS_JR1_TRACE_PRINT(" %d", value);
            device->is_bb_compaction_enabled = value;
            break;
        }

        default:
            status = KAPS_JR1_INVALID_ARGUMENT;
            break;
    }

    return status;
}

kaps_jr1_status
kaps_jr1_device_advanced_get_property(
    struct kaps_jr1_device * device,
    uint32_t property,
    va_list a_list)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    int32_t *val;

    switch (property)
    {
        case KAPS_JR1_DEVICE_PROP_ENABLE_COMPACTION:
        {
            val = va_arg(a_list, int32_t *);
            *val = device->is_bb_compaction_enabled;
            break;
        }
        default:
            status = KAPS_JR1_INVALID_ARGUMENT;
            break;
    }

    return status;
}

kaps_jr1_status
kaps_jr1_device_save_two_level_bb_info(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;

    if (wb_fun->nv_ptr == NULL)
    {
        kaps_jr1_sassert (wb_fun->write_fn);
        kaps_jr1_c_list_iter_init(&device->db_list, &it);

        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);
            struct kaps_jr1_db_bb_info db_bb_info;

            if (db->type == KAPS_JR1_DB_LPM)
            {
                db_bb_info.start_lpu = db->hw_res.db_res->start_lpu;
                db_bb_info.end_lpu = db->hw_res.db_res->end_lpu;
                db_bb_info.lsn_size = db->hw_res.db_res->lsn_info[0].max_lsn_size;

                if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) & db_bb_info,
                                          sizeof(db_bb_info), *wb_fun->nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;

                *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_jr1_db_bb_info);
            }
        }
    }


    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_restore_two_level_bb_info(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;


    if (wb_fun->nv_ptr == NULL)
    {
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);
            struct kaps_jr1_db_bb_info db_bb_info;
            int32_t bb_index;

            if (db->type == KAPS_JR1_DB_LPM && wb_fun->read_fn)
            {
                if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & db_bb_info,
                                         sizeof(db_bb_info), *wb_fun->nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;

                db->hw_res.db_res->start_lpu = db_bb_info.start_lpu;
                db->hw_res.db_res->end_lpu = db_bb_info.end_lpu;
                db->hw_res.db_res->lsn_info[0].max_lsn_size = db_bb_info.lsn_size;

                *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_jr1_db_bb_info);

                for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
                {
                    int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_bb_cascade;
                    int32_t i = 0;

                    db->hw_res.db_res->lsn_info[0].alloc_udm[db->device->core_id][0][bb_index] = 1;
                    for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
                    {
                        kaps_jr1_sassert(device->map->bb_map[bb_blk_id].row->owner == NULL);
                        device->map->bb_map[bb_blk_id].row->owner = db;
                        bb_blk_id++;
                    }
                }
            }
        }
    }


    return KAPS_JR1_OK;
}




