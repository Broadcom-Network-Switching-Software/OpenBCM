

#include "kaps_jr1_resource_algo.h"
#include "kaps_jr1_it_mgr.h"
#include "kaps_jr1_simple_dba.h"
#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_ad_internal.h"

kaps_jr1_status
kaps_jr1_resource_init_mem_map(
    struct kaps_jr1_device *device)
{
    if (device->type == KAPS_JR1_DEVICE)
        KAPS_JR1_STRY(kaps_jr1_initialize_mem_map(device));
    else
        return KAPS_JR1_UNSUPPORTED;

    return KAPS_JR1_OK;
}

struct kaps_jr1_db *
kaps_jr1_resource_get_res_db(
    struct kaps_jr1_db *db)
{
    struct kaps_jr1_db *res_db;

    res_db = db;

    if (res_db->parent)
        res_db = res_db->parent;

    if (res_db->type == KAPS_JR1_DB_LPM)
    {
        res_db = kaps_jr1_lpm_get_res_db(res_db);
    }

    return res_db;
}

void
kaps_jr1_resource_fini_module(
    struct kaps_jr1_device *device)
{
    int32_t i, num_smt;
    int32_t pcm_dba_mgr_idx;
    uint32_t should_release_mem_map;

    should_release_mem_map = 0;

    
    if (device->map && device->type == KAPS_JR1_DEVICE)
    {
        should_release_mem_map = 1;
    }

    if (device->map && !device->nv_ptr)
    {
        should_release_mem_map = 1;
    }

    if (should_release_mem_map)
    {
        int32_t j, i, ndevices = 0;
        struct kaps_jr1_device *tmp;

        for (tmp = device; tmp; tmp = tmp->next_dev)
            ndevices++;

        for (j = 0; j < ndevices; j++)
        {
            struct memory_map *map = &device->map[j];

            for (i = 0; i < map->num_bb * device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                device->alloc->xfree(device->alloc->cookie, map->bb_map[i].row);
            }
            
            if (map->bb_map)
                device->alloc->xfree(device->alloc->cookie, map->bb_map);

            if (map->ab_memory)
                device->alloc->xfree(device->alloc->cookie, map->ab_memory);

            if (map->rpb_ab)
                device->alloc->xfree(device->alloc->cookie, map->rpb_ab);


        }

        device->alloc->xfree(device->alloc->cookie, device->map);
        device->map = NULL;
    }

    num_smt = 1;
    if (device->smt)
    {
        num_smt = 2;
    }

    for (i = 0; i < num_smt; ++i)
    {
        struct kaps_jr1_device *dev;

        if (i == 0)
            dev = device;
        else
            dev = device->smt;

        for (pcm_dba_mgr_idx = 0; pcm_dba_mgr_idx < HW_MAX_PCM_BLOCKS; pcm_dba_mgr_idx++)
        {
            if (dev->hw_res->pcm_dba_mgr[pcm_dba_mgr_idx])
            {
                kaps_jr1_simple_dba_destroy(dev->hw_res->pcm_dba_mgr[pcm_dba_mgr_idx]);
                dev->hw_res->pcm_dba_mgr[pcm_dba_mgr_idx] = NULL;
            }
        }
    }

    if (device->hw_res->it_mgr)
    {
        kaps_jr1_it_mgr_destroy((struct it_mgr *) device->hw_res->it_mgr);
    }
}

kaps_jr1_status
kaps_jr1_resource_set_algorithmic_single_device(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db,
    int32_t value)
{
    struct kaps_jr1_db *tmp;

    if (db->type == KAPS_JR1_DB_AD || db->type == KAPS_JR1_DB_HB)
        return KAPS_JR1_OK;

    if (value == 0)
    {
        
        db->hw_res.db_res->num_dt = 0;
        db->hw_res.db_res->num_dba_dt = 0;
        for (tmp = db; tmp; tmp = tmp->next_tab)
        {
            tmp->hw_res.db_res->algorithmic = 0;
        }

    }
    else if (value == 1)
    {
        
        if (db->type == KAPS_JR1_DB_LPM)
            db->hw_res.db_res->num_dt = 1;
        else
            db->hw_res.db_res->num_dt = 0;

        db->hw_res.db_res->num_dba_dt = 1;

        for (tmp = db; tmp; tmp = tmp->next_tab)
        {
            tmp->hw_res.db_res->algorithmic = 1;
        }
    }
    else
    {
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_resource_set_algorithmic(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db,
    int32_t value)
{
    struct kaps_jr1_device *main_dev, *tmp_dev;
    struct kaps_jr1_db *tmp_db;
    uint32_t bc_bmp;

    if (!db->is_bc_required || db->device->issu_in_progress)
    {
        KAPS_JR1_STRY(kaps_jr1_resource_set_algorithmic_single_device(device, db, value));
        return KAPS_JR1_OK;
    }

    bc_bmp = db->bc_bitmap;
    main_dev = device;
    if (main_dev->main_bc_device)
        main_dev = main_dev->main_bc_device;

    for (tmp_dev = main_dev; tmp_dev; tmp_dev = tmp_dev->next_bc_device)
    {
        if (bc_bmp & (1 << tmp_dev->bc_id))
        {
            tmp_db = kaps_jr1_db_get_bc_db_on_device(tmp_dev, db);
            kaps_jr1_sassert(tmp_db);
            KAPS_JR1_STRY(kaps_jr1_resource_set_algorithmic_single_device(tmp_dev, tmp_db, value));
        }
    }

    return KAPS_JR1_OK;
}

void
kaps_jr1_resource_set_user_specified_algo_mode(
    struct kaps_jr1_db *db)
{
    db->hw_res.db_res->user_specified_algo_type = 1;
}

uint8_t
kaps_jr1_resource_get_is_user_specified_algo_mode(
    struct kaps_jr1_db *db)
{
    return db->hw_res.db_res->user_specified_algo_type;
}



kaps_jr1_status
kaps_jr1_resource_finalize(
    struct kaps_jr1_device *device)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_device *tmp;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    uint32_t has_cascaded_db = 0;

    kaps_jr1_sassert(device->main_dev == NULL);


    

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_AD || db->type == KAPS_JR1_DB_DMA
            || db->type == KAPS_JR1_DB_HB)
            continue;

        if (db->common_info->is_cascaded)
            has_cascaded_db = 1;

    }

    for (tmp = device; tmp; tmp = tmp->next_dev)
    {
        struct it_mgr *res;

        if (!tmp->hw_res->it_mgr)
        {
            KAPS_JR1_STRY(kaps_jr1_it_mgr_init(tmp, device->alloc, &res));
            tmp->hw_res->it_mgr = res;

            
            if (has_cascaded_db && !tmp->other_core->hw_res->it_mgr)
            {
                KAPS_JR1_STRY(kaps_jr1_it_mgr_init(tmp->other_core, device->alloc, &res));
                tmp->other_core->hw_res->it_mgr = res;
            }
        }
    }

    if (device->type == KAPS_JR1_DEVICE)
        status = kaps_jr1_resource_process(device);
    else
        status = KAPS_JR1_UNSUPPORTED;

    return status;
}

kaps_jr1_status
kaps_jr1_resource_dynamic_ad_alloc(
    struct kaps_jr1_db * ad,
    uint8_t * sb_bitmap,
    int32_t num_sb_needed)
{
    if (0 && !ad->common_info->ad_info.db->common_info->enable_dynamic_allocation)
    {
        return KAPS_JR1_OUT_OF_AD;
    }

    kaps_jr1_sassert(0);
    return KAPS_JR1_INTERNAL_ERROR;
}



uint32_t
kaps_jr1_resource_get_num_unused_uda_bricks(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db)
{
    return 0;
}



uint8_t
kaps_jr1_db_get_algorithmic(
    const struct kaps_jr1_db * db)
{
    return db->hw_res.db_res->algorithmic;
}

kaps_jr1_status
kaps_jr1_resource_wb_pre_process(
    struct kaps_jr1_device * device)
{
    return KAPS_JR1_OK;
}



kaps_jr1_status
kaps_jr1_resource_release_udm(
    struct kaps_jr1_device * device,
    struct memory_map * mem_map,
    struct kaps_jr1_db * db,
    int32_t dt_index,
    int32_t udc_no,
    int32_t udm_no)
{
    (void) mem_map;
    
    db->hw_res.db_res->lsn_info[dt_index].alloc_udm[device->core_id][udm_no][udc_no] = 0;

    return KAPS_JR1_OK;
}


