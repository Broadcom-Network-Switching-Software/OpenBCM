

#include "kaps_jr1_ab.h"
#include "kaps_jr1_device_wb.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_instruction_internal.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_db_internal.h"
#include "kaps_jr1_hw_limits.h"
#include "kaps_jr1_resource.h"
#include "kaps_jr1_ad_internal.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_utility.h"
#include "kaps_jr1_handle.h"
#include "kaps_jr1_math.h"
#include "kaps_jr1_cr_pool_mgr.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_dma_internal.h"



kaps_jr1_status
kaps_jr1_wb_refresh_db_handle_in_map(
    struct kaps_jr1_device *main_device)
{
    struct memory_map *map;
    uint32_t i, j;
    struct kaps_jr1_db *fresh_db_ptr = NULL;
    struct kaps_jr1_device *cascade;

    cascade = main_device;

    while (cascade)
    {

        map = resource_kaps_jr1_get_memory_map(cascade);

        for (i = 0; i < cascade->num_dba_sb; i++)
        {
            uint32_t start = i * cascade->num_ab_per_sb;

            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                struct kaps_jr1_ab_info *ab = &map->ab_memory[start + j];

                if (ab->db)
                {
                    KAPS_JR1_STRY(kaps_jr1_db_refresh_handle(main_device, ab->db, &fresh_db_ptr));
                    ab->db = fresh_db_ptr;
                }
            }
        }

        cascade = cascade->next_dev;
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_device_nv_memory_manager_init_internal(
    struct kaps_jr1_device *device,
    uint32_t db_count)
{
    device->nv_mem_mgr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_nv_memory_manager));
    if (!device->nv_mem_mgr)
        return KAPS_JR1_OUT_OF_MEMORY;

    if (device->smt)
        device->smt->nv_mem_mgr = device->nv_mem_mgr;

    device->nv_mem_mgr->num_db = db_count;
    device->nv_mem_mgr->offset_device_info_start = 0;
    device->nv_mem_mgr->offset_device_pending_list = 0;
    device->nv_mem_mgr->offset_device_lpm_info = 0;
    device->nv_mem_mgr->offset_device_lpm_shadow_info = 0;
    device->nv_mem_mgr->offset_device_hb_info = 0;

    device->nv_mem_mgr->offset_db_trie_has_been_built =
        device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_trie_has_been_built)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    device->nv_mem_mgr->offset_db_trie_sb_bitmap =
        device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_trie_sb_bitmap)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_has_been_built);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    device->nv_mem_mgr->offset_db_info_start =
        device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_info_start)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_has_been_built);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_sb_bitmap);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    device->nv_mem_mgr->offset_db_info_end = device->alloc->xcalloc(device->alloc->cookie, db_count, sizeof(uint32_t));
    if (!device->nv_mem_mgr->offset_db_info_end)
    {
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_has_been_built);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_trie_sb_bitmap);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr->offset_db_info_start);
        device->alloc->xfree(device->alloc->cookie, device->nv_mem_mgr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_nv_memory_manager_init(
    struct kaps_jr1_device * device)
{
    uint32_t db_count = 0;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type != KAPS_JR1_DB_AD && db->type != KAPS_JR1_DB_HB)
            db_count++;
    }
    return kaps_jr1_device_nv_memory_manager_init_internal(device, db_count);
}

kaps_jr1_status
kaps_jr1_device_nv_memory_manager_cr_init(
    struct kaps_jr1_device * device,
    uint32_t db_count)
{
    return kaps_jr1_device_nv_memory_manager_init_internal(device, db_count);
}

struct kaps_jr1_device_res_wb_info
{
    uint16_t num_db;
    uint16_t num_instructions;
    uint16_t num_dba_sb;
    uint16_t num_ab;

    struct
    {
        struct kaps_jr1_ab_wb_info ab_info[8];
    } sb_info[2 * KAPS_JR1_HW_MAX_DBA_SB];
};


kaps_jr1_status 
kaps_jr1_device_wb_save_dma(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;
    uint32_t count = 0;
    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

        if (db->type != KAPS_JR1_DB_DMA)
            continue;
        count++;
    }

    if (wb_fun->write_fn) {
        if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) &count,
                                  sizeof(count), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    } else {
        *((uint32_t *) wb_fun->nv_ptr) = count;
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(count);
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += sizeof(count);

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

        if (db->type != KAPS_JR1_DB_DMA)
            continue;
        KAPS_JR1_STRY(kaps_jr1_dma_db_wb_save_info(db, wb_fun));
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status 
kaps_jr1_device_wb_restore_dma(
    struct kaps_jr1_device *device,
    struct kaps_jr1_wb_cb_functions *wb_fun)
{
    uint32_t num_db, db_no;
    
    if (wb_fun->read_fn) {
        if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t*) &num_db,
                                 sizeof(num_db), *wb_fun->nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
    } else {
        num_db = *((uint32_t *) wb_fun->nv_ptr);
    }

    *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(num_db);
    if (wb_fun->nv_ptr)
        wb_fun->nv_ptr += sizeof(num_db);

    for (db_no = 0; db_no < num_db; db_no++) {
        KAPS_JR1_STRY(kaps_jr1_dma_db_wb_restore_info(device, wb_fun));
    }

    return KAPS_JR1_OK;
}


static kaps_jr1_status
kaps_jr1_device_recover_state(
    struct kaps_jr1_device *device)
{
    uint32_t nv_offset = 0;
    uint32_t num_db;
    uint32_t db_iter;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_wb_cb_functions cb_fun;
    uint32_t pending_count;
    struct kaps_jr1_device *tmp_device;
    uint8_t *tmp_ptr = NULL;

    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    if (!device->nv_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    cb_fun.read_fn = device->nv_read_fn;
    cb_fun.write_fn = device->nv_write_fn;
    cb_fun.handle = device->nv_handle;
    cb_fun.nv_offset = &nv_offset;
    cb_fun.nv_ptr = device->nv_ptr;

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
    {
        tmp_device->issu_status = KAPS_JR1_ISSU_RESTORE_START;
        if (tmp_device->smt)
        {
            tmp_device->smt->issu_status = KAPS_JR1_ISSU_RESTORE_START;
        }
    }

    tmp_ptr = cb_fun.nv_ptr;

    num_db = *(uint32_t *) cb_fun.nv_ptr;
    tmp_ptr += sizeof(uint32_t);

    KAPS_JR1_STRY(kaps_jr1_device_nv_memory_manager_cr_init(device, num_db));
    KAPS_JR1_CR_GET_POOL_MGR(device, device->nv_mem_mgr->kaps_jr1_cr_pool_mgr);

    device->nv_mem_mgr->num_db = num_db;

    device->nv_mem_mgr->offset_device_lpm_shadow_info = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    device->nv_mem_mgr->offset_device_info_start = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    device->nv_mem_mgr->offset_device_pending_list = *(uint32_t *) tmp_ptr;
    pending_count = *(uint32_t *) ((uint8_t *) device->nv_ptr + device->nv_mem_mgr->offset_device_pending_list);
    tmp_ptr += sizeof(uint32_t);

    if (pending_count)
    {
        *device->cr_status = KAPS_JR1_RESTORE_CHANGES_ABORTED;
    }
    else if (*device->cr_status == KAPS_JR1_RESTORE_CHANGES_ABORTED)
    {
        *device->cr_status = KAPS_JR1_RESTORE_CHANGES_COMMITTED;
    }
    else
    {
        *device->cr_status = KAPS_JR1_RESTORE_NO_CHANGE;
    }

    device->nv_mem_mgr->offset_device_lpm_info = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    device->nv_mem_mgr->offset_device_hb_info = *(uint32_t *) tmp_ptr;
    tmp_ptr += sizeof(uint32_t);

    for (db_iter = 0; db_iter < device->nv_mem_mgr->num_db; db_iter++)
    {
        device->nv_mem_mgr->offset_db_trie_has_been_built[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
        device->nv_mem_mgr->offset_db_trie_sb_bitmap[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
        device->nv_mem_mgr->offset_db_info_start[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
        device->nv_mem_mgr->offset_db_info_end[db_iter] = *(uint32_t *) tmp_ptr;
        tmp_ptr += sizeof(uint32_t);
    }

    cb_fun.nv_ptr = cb_fun.nv_ptr + device->nv_mem_mgr->offset_device_info_start;

    
    KAPS_JR1_STRY(kaps_jr1_wb_restore_device_info(device, &cb_fun));

    KAPS_JR1_STRY(kaps_jr1_device_lock(device));

    if (device->lpm_mgr)
    {
        KAPS_JR1_STRY(kaps_jr1_lpm_cr_restore_state(device, &cb_fun));
        cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + device->nv_mem_mgr->offset_device_lpm_shadow_info;
        *cb_fun.nv_offset = device->nv_mem_mgr->offset_device_lpm_shadow_info;
        kaps_jr1_lpm_cr_get_adv_ptr(device, &cb_fun);
    }
    if (device->aging_table && device->type == KAPS_JR1_DEVICE)
    {
        cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + device->nv_mem_mgr->offset_device_hb_info;
        *cb_fun.nv_offset = device->nv_mem_mgr->offset_device_hb_info;
        KAPS_JR1_STRY(kaps_jr1_hb_cr_init(device, &cb_fun));
    }

    KAPS_JR1_STRY(kaps_jr1_cr_pool_mgr_refresh_db_handles(device));

    db_iter = 0;
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            db->common_info->entry_size = db->fn_table->cr_calc_entry_mem(db);
            db->common_info->nv_db_iter = db_iter;
            db_iter++;
        }
    }

    for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
    {
        tmp_device->issu_status = KAPS_JR1_ISSU_RESTORE_END;
        if (tmp_device->smt)
        {
            tmp_device->smt->issu_status = KAPS_JR1_ISSU_RESTORE_END;
        }
    }

    db_iter = 0;

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            nv_offset = device->nv_mem_mgr->offset_db_info_start[db_iter];
            cb_fun.nv_ptr = (uint8_t *) device->nv_ptr + nv_offset;
            KAPS_JR1_STRY(db->fn_table->restore_cr_state(db, &cb_fun));
            db_iter++;
        }
    }

    KAPS_JR1_STRY(kaps_jr1_lpm_wb_post_processing(device));

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmFalse;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmFalse;
        tmp_device = tmp_device->next_dev;
    }

    
    device->is_config_locked = 0;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_wb_save_device_info(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;
    uint32_t i, j;
    struct kaps_jr1_device *cascade;

    
    cascade = device;
    while (cascade != NULL)
    {
        uint16_t count;
        struct kaps_jr1_device_res_wb_info *res_ptr = NULL;
        struct memory_map *map;

        if (wb_fun->write_fn)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) (&cascade->prop),
                                      sizeof(struct kaps_jr1_device_property), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        else
        {
            kaps_jr1_memcpy(wb_fun->nv_ptr, (uint8_t *) (&cascade->prop), sizeof(struct kaps_jr1_device_property));
            wb_fun->nv_ptr += sizeof(struct kaps_jr1_device_property);
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_jr1_device_property);

        if (wb_fun->nv_ptr == NULL)
        {
            res_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_device_res_wb_info));
            if (res_ptr == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;
        }
        else if (device->type == KAPS_JR1_DEVICE)
        {
            res_ptr = (struct kaps_jr1_device_res_wb_info *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(*res_ptr);
        }

        count = 0;
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

            if (db->type == KAPS_JR1_DB_AD 
                || db->type == KAPS_JR1_DB_DMA || db->type == KAPS_JR1_DB_HB)
                continue;

            if (db->type != KAPS_JR1_DB_LPM)
                return KAPS_JR1_UNSUPPORTED;

            
            if (db->is_clone || db->parent)
                continue;
            count++;
        }
        if (wb_fun->nv_ptr == NULL || device->type == KAPS_JR1_DEVICE)
        {
            res_ptr->num_db = count;
        }
        else
        {
            *(uint16_t *) wb_fun->nv_ptr = count;
            wb_fun->nv_ptr += sizeof(count);
            *wb_fun->nv_offset += sizeof(count);
        }

        count = 0;
        kaps_jr1_c_list_iter_init(&device->inst_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_instruction *instruction = KAPS_JR1_INSTLIST_TO_ENTRY(e1);

            
            if (instruction->is_cmp3_pair)
                continue;

            
            if (instruction->device->device_no != 0)
                continue;
            count++;
        }
        if (wb_fun->nv_ptr == NULL || device->type == KAPS_JR1_DEVICE)
        {
            res_ptr->num_instructions = count;
        }
        else
        {
            *(uint16_t *) wb_fun->nv_ptr = count;
            wb_fun->nv_ptr += sizeof(count);
            *wb_fun->nv_offset += sizeof(count);
            break;
        }

        KAPS_JR1_STRY(kaps_jr1_resource_wb_pre_process(cascade));
        map = resource_kaps_jr1_get_memory_map(cascade);

        res_ptr->num_dba_sb = cascade->num_dba_sb;

        res_ptr->num_ab = cascade->num_ab;
        
        for (i = 0; i < cascade->num_dba_sb; i++)
        {
            uint32_t start = i * cascade->num_ab_per_sb;

            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                struct kaps_jr1_ab_info *ab = &map->ab_memory[start + j];

                res_ptr->sb_info[i].ab_info[j].ab_num = ab->ab_num;
                res_ptr->sb_info[i].ab_info[j].num_slots = ab->num_slots;
                res_ptr->sb_info[i].ab_info[j].base_addr = ab->base_addr;
                res_ptr->sb_info[i].ab_info[j].base_idx = ab->base_idx;
                res_ptr->sb_info[i].ab_info[j].blk_cleared = ab->blk_cleared;
                res_ptr->sb_info[i].ab_info[j].conf = ab->conf;
                res_ptr->sb_info[i].ab_info[j].device_no = ab->device->device_no;
                res_ptr->sb_info[i].ab_info[j].translate = ab->translate;
                res_ptr->sb_info[i].ab_info[j].db = ab->db;
                res_ptr->sb_info[i].ab_info[j].is_dup_ab = ab->is_dup_ab;

                if (ab->dup_ab)
                    res_ptr->sb_info[i].ab_info[j].paired_ab_num = ab->dup_ab->ab_num;
                else
                    res_ptr->sb_info[i].ab_info[j].paired_ab_num = INVALID_PAIRED_AB_NUM;
            }
        }

        if (wb_fun->write_fn != NULL)
        {
            if (0 != wb_fun->write_fn(wb_fun->handle, (uint8_t *) res_ptr, sizeof(*res_ptr), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*res_ptr);

        if (wb_fun->nv_ptr == NULL)
            device->alloc->xfree(device->alloc->cookie, res_ptr);
        cascade = cascade->next_dev;
    }

    

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

        if (db->type == KAPS_JR1_DB_AD || db->type == KAPS_JR1_DB_DMA
            || db->type == KAPS_JR1_DB_HB)
            continue;

        if (db->type != KAPS_JR1_DB_LPM)
            return KAPS_JR1_UNSUPPORTED;

        
        if (db->is_clone || db->parent)
            continue;

        KAPS_JR1_STRY(kaps_jr1_db_wb_save_info(db, wb_fun));
    }

    KAPS_JR1_STRY(kaps_jr1_device_wb_save_dma(device, wb_fun));

    KAPS_JR1_STRY(kaps_jr1_device_save_two_level_bb_info(device, wb_fun));

    

    kaps_jr1_c_list_iter_init(&device->inst_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *instruction = KAPS_JR1_INSTLIST_TO_ENTRY(e1);

        
        if (instruction->is_cmp3_pair)
            continue;

        
        if (instruction->device->device_no != 0)
            continue;
        KAPS_JR1_STRY(kaps_jr1_instruction_wb_save(instruction, wb_fun));
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_wb_restore_device_info(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * wb_fun)
{
    uint32_t i, j, num_db = 0, num_instructions = 0;
    struct kaps_jr1_device *cascade;

    
    cascade = device;
    while (cascade)
    {
        struct kaps_jr1_device_res_wb_info *res_ptr;
        struct memory_map *map;

        if (wb_fun->read_fn)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) & cascade->prop,
                                     sizeof(struct kaps_jr1_device_property), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        else
        {
            kaps_jr1_memcpy((uint8_t *) (&cascade->prop), wb_fun->nv_ptr, sizeof(struct kaps_jr1_device_property));
            wb_fun->nv_ptr += sizeof(struct kaps_jr1_device_property);
            if (cascade->smt)
                kaps_jr1_memcpy((uint8_t *) (&cascade->smt->prop), (uint8_t *) (&cascade->prop),
                            sizeof(struct kaps_jr1_device_property));
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(struct kaps_jr1_device_property);

        if (wb_fun->nv_ptr == NULL)
        {
            res_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_device_res_wb_info));
            if (res_ptr == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;
        }
        else if (device->type == KAPS_JR1_DEVICE)
        {
            res_ptr = (struct kaps_jr1_device_res_wb_info *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(*res_ptr);
        }
        else
        {
            num_db = *(uint16_t *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(uint16_t);
            num_instructions = *(uint16_t *) wb_fun->nv_ptr;
            wb_fun->nv_ptr += sizeof(uint16_t);
            break;
        }
        if (wb_fun->read_fn != NULL)
        {
            if (0 != wb_fun->read_fn(wb_fun->handle, (uint8_t *) res_ptr, sizeof(*res_ptr), *wb_fun->nv_offset))
                return KAPS_JR1_NV_READ_WRITE_FAILED;
        }
        *wb_fun->nv_offset = *wb_fun->nv_offset + sizeof(*res_ptr);

        cascade->num_dba_sb = res_ptr->num_dba_sb;

        cascade->num_ab = res_ptr->num_ab;
        
        map = resource_kaps_jr1_get_memory_map(cascade);

        for (i = 0; i < cascade->num_dba_sb; i++)
        {
            for (j = 0; j < cascade->num_ab_per_sb; ++j)
            {
                uint32_t ab_num = (i * cascade->num_ab_per_sb) + j;
                struct kaps_jr1_ab_info *ab = &map->ab_memory[ab_num];

                kaps_jr1_sassert(res_ptr->sb_info[i].ab_info[j].ab_num == ab_num);
                kaps_jr1_sassert(res_ptr->sb_info[i].ab_info[j].device_no == cascade->device_no);

                ab->num_slots = res_ptr->sb_info[i].ab_info[j].num_slots;
                ab->conf = res_ptr->sb_info[i].ab_info[j].conf;
                ab->blk_cleared = res_ptr->sb_info[i].ab_info[j].blk_cleared;
                ab->translate = res_ptr->sb_info[i].ab_info[j].translate;

                if (res_ptr->sb_info[i].ab_info[j].paired_ab_num != INVALID_PAIRED_AB_NUM)
                    ab->dup_ab = &map->ab_memory[res_ptr->sb_info[i].ab_info[j].paired_ab_num];

                ab->base_addr = res_ptr->sb_info[i].ab_info[j].base_addr;
                ab->base_idx = res_ptr->sb_info[i].ab_info[j].base_idx;
                ab->db = res_ptr->sb_info[i].ab_info[j].db;
                ab->is_dup_ab = res_ptr->sb_info[i].ab_info[j].is_dup_ab;

            }
        }

        if (num_db == 0)
            num_db = res_ptr->num_db;
        if (num_instructions == 0)
            num_instructions = res_ptr->num_instructions;

        if (wb_fun->nv_ptr == NULL)
            device->alloc->xfree(device->alloc->cookie, res_ptr);

        cascade = cascade->next_dev;
    }

    
    for (i = 0; i < num_db; i++)
    {
        KAPS_JR1_STRY(kaps_jr1_db_wb_restore_info(device, NULL, wb_fun));
    }

    
    KAPS_JR1_STRY(kaps_jr1_device_wb_restore_dma(device, wb_fun)); 

    KAPS_JR1_STRY(kaps_jr1_device_restore_two_level_bb_info(device, wb_fun));


    
    for (i = 0; i < num_instructions; i++)
    {
        KAPS_JR1_STRY(kaps_jr1_instruction_wb_restore(device, wb_fun));
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_save_state(
    struct kaps_jr1_device * bc_device,
    kaps_jr1_device_issu_read_fn read_fn,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle)
{
    uint32_t nv_start_offset = 0;

    KAPS_JR1_TRACE_IN("%p %p %p %p\n", bc_device, read_fn, write_fn, handle);
    if (!bc_device || !write_fn || !read_fn)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (bc_device->main_bc_device)
        bc_device = bc_device->main_bc_device;

    KAPS_JR1_ASSERT_OR_ERROR((bc_device->fatal_transport_error == 0),
                         "Transport Error ignored. Asserting\n",
                         bc_device->prop.return_error_on_asserts, KAPS_JR1_FATAL_TRANSPORT_ERROR);

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_device *device = bc_device;
        uint32_t nv_offset = nv_start_offset;
        uint32_t nv_size = 0, nv_size_crc = 0, nv_footer = KAPS_JR1_NV_MAGIC_NUMBER;
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *el;
        struct kaps_jr1_wb_cb_functions cb_fun;
        struct kaps_jr1_device *tmp_device = device;

        if (device->is_generic_bc_device)
        {
            device->issu_in_progress = NlmTrue;
            if (device->smt)
                device->smt->issu_in_progress = NlmTrue;
            continue;
        }

        if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
            return KAPS_JR1_INVALID_FLAGS;

        if (device->type != KAPS_JR1_DEVICE)
            return KAPS_JR1_UNSUPPORTED;

        if (!device->is_config_locked)
            return KAPS_JR1_DEVICE_UNLOCKED;

        
        KAPS_JR1_STRY(kaps_jr1_lpm_wb_pre_processing(device));

        tmp_device = device;
        while (tmp_device)
        {
            tmp_device->issu_in_progress = NlmTrue;
            if (tmp_device->smt)
                tmp_device->smt->issu_in_progress = NlmTrue;
            tmp_device = tmp_device->next_dev;
        }

        nv_offset += sizeof(nv_size);
        nv_offset += sizeof(nv_size_crc);

        cb_fun.read_fn = read_fn;
        cb_fun.write_fn = write_fn;
        cb_fun.handle = handle;
        cb_fun.nv_offset = &nv_offset;
        cb_fun.nv_ptr = NULL;

        
        KAPS_JR1_STRY(kaps_jr1_wb_save_device_info(device, &cb_fun));

        
        if (device->lpm_mgr)
        {
            KAPS_JR1_STRY(kaps_jr1_lpm_wb_save_state(device, &cb_fun));
        }

        
        KAPS_JR1_STRY(kaps_jr1_hb_wb_save_state(device, &cb_fun));

        

        cb_fun.read_fn = read_fn;
        cb_fun.write_fn = write_fn;
        cb_fun.handle = handle;
        cb_fun.nv_offset = &nv_offset;

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_AD
                || db->type == KAPS_JR1_DB_DMA || db->type == KAPS_JR1_DB_HB)
                continue;
            KAPS_JR1_STRY(db->fn_table->save_state(db, &cb_fun));

            if (db->common_info->defer_deletes_to_install)
            {
                int32_t i;

                if (0 != cb_fun.write_fn(cb_fun.handle, (uint8_t *) & db->common_info->num_pending_del_entries,
                                         sizeof(db->common_info->num_pending_del_entries), *cb_fun.nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
                *cb_fun.nv_offset += sizeof(db->common_info->num_pending_del_entries);

                if (0 != cb_fun.write_fn(cb_fun.handle, (uint8_t *) & db->common_info->max_pending_del_entries,
                                         sizeof(db->common_info->max_pending_del_entries), *cb_fun.nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
                *cb_fun.nv_offset += sizeof(db->common_info->max_pending_del_entries);

                for (i = 0; i < db->common_info->num_pending_del_entries; i++)
                {
                    uint32_t user_handle;

                    user_handle = db->common_info->del_entry_list[i]->user_handle;
                    if (0 != cb_fun.write_fn(cb_fun.handle, (uint8_t *) & user_handle,
                                             sizeof(user_handle), *cb_fun.nv_offset))
                        return KAPS_JR1_NV_READ_WRITE_FAILED;
                    *cb_fun.nv_offset += sizeof(user_handle);
                }
            }
        }

        
        nv_size = nv_offset - nv_start_offset;
        nv_size_crc = kaps_jr1_crc32(nv_size_crc, (uint8_t *) & nv_size, sizeof(nv_size));
        if (0 != write_fn(handle, (uint8_t *) & nv_size, sizeof(nv_size), nv_start_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;

        if (0 != write_fn(handle, (uint8_t *) & nv_size_crc, sizeof(nv_size_crc), nv_start_offset + sizeof(nv_size)))
            return KAPS_JR1_NV_READ_WRITE_FAILED;

        if (0 != write_fn(handle, (uint8_t *) & nv_footer, sizeof(nv_footer), nv_offset))
            return KAPS_JR1_NV_READ_WRITE_FAILED;
        nv_offset += sizeof(nv_footer);

        device->issu_status = KAPS_JR1_ISSU_SAVE_COMPLETED;
        nv_start_offset = nv_offset;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_save_state_and_continue(
    struct kaps_jr1_device * bc_device,
    kaps_jr1_device_issu_read_fn read_fn,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle)
{
    KAPS_JR1_TRACE_IN("%p %p %p %p\n", bc_device, read_fn, write_fn, handle);
    KAPS_JR1_STRY(kaps_jr1_device_save_state(bc_device, read_fn, write_fn, handle));

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_device *device = bc_device;
        struct kaps_jr1_device *tmp_device = device;

        device->is_wb_continue = 1;
        device->issu_status = KAPS_JR1_ISSU_INIT;
        tmp_device = device;
        while (tmp_device)
        {
            tmp_device->issu_in_progress = NlmFalse;
            if (tmp_device->smt)
                tmp_device->smt->issu_in_progress = NlmFalse;
            tmp_device = tmp_device->next_dev;
        }
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_restore_state_partial(
    struct kaps_jr1_device * device,
    kaps_jr1_device_issu_read_fn read_fn,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle,
    struct kaps_jr1_wb_cb_functions * cb_fun,
    uint32_t * nv_start_offset)
{
    uint32_t nv_offset = *nv_start_offset;
    uint32_t nv_size = 0, nv_size_crc = 0, nv_footer = 0, check_crc = 0;
    struct kaps_jr1_device *tmp_device = device;

    if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
        return KAPS_JR1_INVALID_FLAGS;

    if (device->type != KAPS_JR1_DEVICE)
        return KAPS_JR1_UNSUPPORTED;

    if (device->is_config_locked)
        return KAPS_JR1_DEVICE_ALREADY_LOCKED;

    if (device->issu_status != KAPS_JR1_ISSU_INIT)
        return KAPS_JR1_ISSU_RESTORE_DONE;

    if (!(device->flags & KAPS_JR1_DEVICE_SKIP_INIT))
        return KAPS_JR1_DEVICE_NO_SKIP_INIT_FLAG;

    if (kaps_jr1_c_list_count(&device->inst_list) != 0)
        return KAPS_JR1_DEVICE_NOT_EMPTY;

    if (kaps_jr1_c_list_count(&device->db_list) != 0)
        return KAPS_JR1_DEVICE_NOT_EMPTY;

    for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
    {
        tmp_device->issu_status = KAPS_JR1_ISSU_RESTORE_START;
        if (tmp_device->smt)
        {
            tmp_device->smt->issu_status = KAPS_JR1_ISSU_RESTORE_START;
        }
    }

    tmp_device = device;
    while (tmp_device)
    {
        tmp_device->issu_in_progress = NlmTrue;
        if (tmp_device->smt)
            tmp_device->smt->issu_in_progress = NlmTrue;
        tmp_device = tmp_device->next_dev;
    }

    

    if (0 != read_fn(handle, (uint8_t *) & nv_size, sizeof(nv_size), nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    nv_offset += sizeof(nv_size);

    
    if (0 != read_fn(handle, (uint8_t *) & nv_size_crc, sizeof(nv_size_crc), nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    nv_offset += sizeof(nv_size_crc);

    check_crc = kaps_jr1_crc32(check_crc, (uint8_t *) & nv_size, sizeof(nv_size));
    if (check_crc != nv_size_crc)
        return KAPS_JR1_NV_DATA_CORRUPT;

    if (0 != read_fn(handle, (uint8_t *) & nv_footer, sizeof(nv_footer), *nv_start_offset + nv_size))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_start_offset = *nv_start_offset + nv_size + sizeof(nv_footer);

    if (KAPS_JR1_NV_MAGIC_NUMBER != nv_footer)
        return KAPS_JR1_NV_DATA_CORRUPT;

    cb_fun->read_fn = read_fn;
    cb_fun->write_fn = write_fn;
    cb_fun->handle = handle;
    cb_fun->nv_ptr = device->nv_ptr;
    *cb_fun->nv_offset = nv_offset;

    KAPS_JR1_STRY(kaps_jr1_wb_restore_device_info(device, cb_fun));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_restore_cascaded_inst_ptr(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *e1;

    kaps_jr1_c_list_iter_init(&device->inst_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *instr = KAPS_JR1_INSTLIST_TO_ENTRY(e1);

        if (instr->cascaded_inst_stale_ptr)
        {
            KAPS_JR1_STRY(kaps_jr1_instruction_refresh_handle
                      (device->other_core, (struct kaps_jr1_instruction *) instr->cascaded_inst_stale_ptr,
                       &instr->cascaded_inst));
        }
    }

    kaps_jr1_c_list_iter_init(&device->other_core->inst_list, &it);
    while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *instr = KAPS_JR1_INSTLIST_TO_ENTRY(e1);

        if (instr->cascaded_inst_stale_ptr)
        {
            KAPS_JR1_STRY(kaps_jr1_instruction_refresh_handle
                      (device, (struct kaps_jr1_instruction *) instr->cascaded_inst_stale_ptr, &instr->cascaded_inst));
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_restore_state(
    struct kaps_jr1_device * bc_device,
    kaps_jr1_device_issu_read_fn read_fn,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle)
{
    struct kaps_jr1_device *device;
    struct kaps_jr1_device *tmp_device;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_wb_cb_functions cb_fun[KAPS_JR1_MAX_BROADCAST_DEVICES];
    uint32_t nv_offset[KAPS_JR1_MAX_BROADCAST_DEVICES];
    uint32_t nv_start_offset = 0;

    KAPS_JR1_TRACE_IN("%p %p %p %p\n", bc_device, read_fn, write_fn, handle);

    if (!bc_device)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (bc_device->nv_ptr)
        return kaps_jr1_device_recover_state(bc_device);
    else if (!read_fn || !write_fn)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (bc_device->main_bc_device && bc_device != bc_device->main_bc_device)
        return KAPS_JR1_OK;

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    kaps_jr1_memset (cb_fun, 0, sizeof(cb_fun));

    for (; device; device = device->next_bc_device)
    {
        if (device->is_generic_bc_device)
        {
            continue;
        }
        cb_fun[device->bc_id].nv_offset = &nv_offset[device->bc_id];
        KAPS_JR1_STRY(kaps_jr1_device_restore_state_partial(device, read_fn, write_fn, handle,
                                                    &cb_fun[device->bc_id], &nv_start_offset));
    }

    KAPS_JR1_STRY(kaps_jr1_device_lock(bc_device));

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    for (; device; device = device->next_bc_device)
    {
        for (tmp_device = device; tmp_device; tmp_device = tmp_device->next_dev)
        {
            tmp_device->issu_status = KAPS_JR1_ISSU_RESTORE_END;
            if (tmp_device->smt)
            {
                tmp_device->smt->issu_status = KAPS_JR1_ISSU_RESTORE_END;
            }
        }
    }

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    for (; device; device = device->next_bc_device)
    {
        if (device->is_generic_bc_device)
        {
            device->issu_in_progress = NlmFalse;
            if (device->smt)
                device->smt->issu_in_progress = NlmFalse;
            continue;
        }
        if (device->lpm_mgr)
        {
            cb_fun[device->bc_id].read_fn = read_fn;
            cb_fun[device->bc_id].write_fn = write_fn;
            cb_fun[device->bc_id].handle = handle;
            cb_fun[device->bc_id].nv_offset = &nv_offset[device->bc_id];
            cb_fun[device->bc_id].nv_ptr = device->nv_ptr;
            KAPS_JR1_STRY(kaps_jr1_lpm_wb_restore_state(device, &cb_fun[device->bc_id]));
        }

        

        KAPS_JR1_STRY(kaps_jr1_hb_wb_restore_state(device, &cb_fun[device->bc_id]));

        
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_AD
                || db->type == KAPS_JR1_DB_DMA || db->type == KAPS_JR1_DB_HB)
                continue;
            db->device->db_bc_bitmap = db->bc_bitmap;
            KAPS_JR1_STRY(db->fn_table->restore_state(db, &cb_fun[device->bc_id]));
            if (db->common_info->defer_deletes_to_install)
            {
                int32_t i;

                if (0 !=
                    cb_fun[device->bc_id].read_fn(cb_fun[device->bc_id].handle,
                                                  (uint8_t *) & db->common_info->num_pending_del_entries,
                                                  sizeof(db->common_info->num_pending_del_entries),
                                                  *cb_fun[device->bc_id].nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
                *cb_fun[device->bc_id].nv_offset += sizeof(db->common_info->num_pending_del_entries);

                if (0 !=
                    cb_fun[device->bc_id].read_fn(cb_fun[device->bc_id].handle,
                                                  (uint8_t *) & db->common_info->max_pending_del_entries,
                                                  sizeof(db->common_info->max_pending_del_entries),
                                                  *cb_fun[device->bc_id].nv_offset))
                    return KAPS_JR1_NV_READ_WRITE_FAILED;
                *cb_fun[device->bc_id].nv_offset += sizeof(db->common_info->max_pending_del_entries);

                for (i = 0; i < db->common_info->num_pending_del_entries; i++)
                {
                    uint32_t user_handle;

                    if (0 != cb_fun[device->bc_id].read_fn(cb_fun[device->bc_id].handle, (uint8_t *) & user_handle,
                                                           sizeof(user_handle), *cb_fun[device->bc_id].nv_offset))
                        return KAPS_JR1_NV_READ_WRITE_FAILED;
                    *cb_fun[device->bc_id].nv_offset += sizeof(user_handle);
                    if (db->type == KAPS_JR1_DB_LPM)
                        db->common_info->del_entry_list[i] = KAPS_JR1_WB_CONVERT_TO_ENTRY_HANDLE(kaps_jr1_entry, user_handle);
                    else
                        KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), &db->common_info->del_entry_list[i],
                                                (uintptr_t) user_handle);
                }
            }
        }

        KAPS_JR1_STRY(kaps_jr1_lpm_wb_post_processing(device));

        
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type != KAPS_JR1_DB_LPM)
                continue;
            if (db->common_info->defer_deletes_to_install)
            {
                int32_t i;

                for (i = 0; i < db->common_info->num_pending_del_entries; i++)
                {
                    KAPS_JR1_WB_HANDLE_READ_LOC(KAPS_JR1_GET_DB_PARENT(db), &db->common_info->del_entry_list[i],
                                            (uintptr_t) db->common_info->del_entry_list[i]);
                }
            }
        }

        
        device->is_config_locked = 0;
    }

    device = bc_device;
    if (bc_device->main_bc_device)
        device = bc_device->main_bc_device;

    for (; device; device = device->next_bc_device)
    {
        struct kaps_jr1_device *tmp_device;

        tmp_device = device;
        while (tmp_device)
        {
            tmp_device->issu_in_progress = NlmFalse;
            if (tmp_device->smt)
                tmp_device->smt->issu_in_progress = NlmFalse;

            tmp_device = tmp_device->next_dev;
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_reconcile_start(
    struct kaps_jr1_device * bc_device)
{
    KAPS_JR1_TRACE_IN("%p\n", bc_device);
    if (!bc_device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_device *device = bc_device;
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *e1;
        struct kaps_jr1_device *cascade;

        if (device->is_generic_bc_device)
        {
            continue;
        }

        if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
            return KAPS_JR1_INVALID_FLAGS;

        if (device->issu_status != KAPS_JR1_ISSU_RESTORE_END)
            return KAPS_JR1_ISSU_RESTORE_REQUIRED;

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

            KAPS_JR1_TRY(kaps_jr1_db_reconcile_start(tmp));
        }
        for (cascade = device; cascade; cascade = cascade->next_dev)
        {
            cascade->issu_status = KAPS_JR1_ISSU_RECONCILE_START;
            if (cascade->smt)
                cascade->smt->issu_status = KAPS_JR1_ISSU_RECONCILE_START;
        }
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_reconcile_end(
    struct kaps_jr1_device * bc_device)
{
    KAPS_JR1_TRACE_IN("%p\n", bc_device);
    if (!bc_device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_device *device = bc_device;
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *e1;
        struct kaps_jr1_device *cascade;

        if (device->is_generic_bc_device)
        {
            continue;
        }

        if (!(device->flags & KAPS_JR1_DEVICE_ISSU))
            return KAPS_JR1_INVALID_FLAGS;

        if (device->issu_status != KAPS_JR1_ISSU_RECONCILE_START)
            return KAPS_JR1_ISSU_RECONCILE_REQUIRED;

        for (cascade = device; cascade; cascade = cascade->next_dev)
        {
            cascade->issu_status = KAPS_JR1_ISSU_RECONCILE_END;
            if (cascade->smt)
                cascade->smt->issu_status = KAPS_JR1_ISSU_RECONCILE_END;
        }

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((e1 = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *tmp = KAPS_JR1_SSDBLIST_TO_ENTRY(e1);

            if (tmp->type == KAPS_JR1_DB_AD
                || tmp->type == KAPS_JR1_DB_DMA || tmp->type == KAPS_JR1_DB_HB)
                continue;
            KAPS_JR1_TRY(kaps_jr1_db_reconcile_end(tmp));
        }
    }
    return KAPS_JR1_OK;
}

