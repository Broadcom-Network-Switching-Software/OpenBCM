

#include <unistd.h>

#include "kaps_jr1_handle.h"
#include "kaps_jr1_ab.h"
#include "kaps_jr1_device_wb.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_cr_pool_mgr.h"
#include "kaps_jr1_pfx_hash_table.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_errors.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_fib_hw.h"
#include "kaps_jr1_xpt.h"
#include "kaps_jr1_default_allocator.h"
#include <inttypes.h> 


static kaps_jr1_status
initialize_nlm_allocator(
    struct kaps_jr1_device *device)
{
    kaps_jr1_nlm_allocator *alloc_p;

    alloc_p = kaps_jr1_nlm_allocator_ctor(&device->nlm_alloc);
    alloc_p->m_vtbl.m_malloc = (kaps_jr1_nlm_allocator_alloc_t) device->alloc->xmalloc;
    alloc_p->m_vtbl.m_calloc = (kaps_jr1_nlm_allocator_calloc_t) device->alloc->xcalloc;
    alloc_p->m_vtbl.m_free = (kaps_jr1_nlm_allocator_free_t) device->alloc->xfree;
    alloc_p->m_clientData_p = device->alloc->cookie;

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_device_init_internal(
    enum kaps_jr1_device_pair_type pair_type,
    uint32_t flags,
    void *xpt,
    struct kaps_jr1_device * device,
    int32_t core_id)
{
    kaps_jr1_status status;

    device->num_cores = 1;
    device->core_id = core_id;
    device->type = KAPS_JR1_DEVICE;
    device->pair_type = pair_type;

    KAPS_JR1_STRY(kaps_jr1_resource_init_module(device));

    device->xpt = xpt;
    device->flags = flags;
    device->prop.lpt_mode = 1;

    
    initialize_nlm_allocator(device);
    
    device->max_num_pfx_log_records = 0;
    
    if (device->max_num_pfx_log_records && !device->pfx_log_records)
    {
        
        device->pfx_log_records = device->alloc->xcalloc(device->alloc->cookie, 
                                        device->max_num_pfx_log_records,
                                        sizeof(kaps_jr1_prefix_log_record));
                                        
        if (!device->pfx_log_records)
        {
            device->alloc->xfree(device->alloc->cookie, device);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }


    KAPS_JR1_STRY(kaps_jr1_dm_init(device));


    device->num_ab = 4;
    device->num_dba_sb = 4;
    device->max_num_searches = 4;
    device->max_num_clone_parallel_lookups = 2;
    device->max_search_key_width_1 = 640;
    device->num_small_hb_blocks = 0;
    device->num_rows_in_small_hb_block = 0;
    device->tcam_format = KAPS_JR1_TCAM_FORMAT_1;
    device->combine_tcam_xy_write = 0;
    device->xpt_version = KAPS_JR1_XPT_VERSION_1;

    if (device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
    {
        device->num_ab = 2;
        device->num_dba_sb = 2;
        device->max_num_searches = 2;

        device->num_hb_blocks = 16;
        device->num_rows_in_hb_block = 1024;
    }

    if (device->id == KAPS_JR1_QUX_DEVICE_ID)
    {
        device->num_ab = 2;
        device->num_dba_sb = 2;
        device->max_num_searches = 2;

        device->num_hb_blocks = 16;
        device->num_rows_in_hb_block = 256;
    }

    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
    {
        device->num_hb_blocks = 32;

        device->num_rows_in_hb_block = 4096;

        if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM0)
        {
            device->num_rows_in_hb_block = 2048;
        }
        else if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
        {
            device->num_rows_in_hb_block = 2048 + 256;
        }
    }


    device->num_ab_per_sb = 1;

    device->max_ad_width_1 = 20;
    device->dba_offset = 1;

    status = kaps_jr1_device_advanced_init(device);
    if (status != KAPS_JR1_OK)
    {
        device->alloc->xfree(device->alloc->cookie, device);
        return status;
    }

    status = kaps_jr1_resource_init_mem_map(device);
    if (status != KAPS_JR1_OK)
    {
        device->alloc->xfree(device->alloc->cookie, device);
        return status;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_init(
    enum kaps_jr1_device_pair_type pair_type,
    uint32_t flags,
    void *xpt,
    struct kaps_jr1_device ** device_pp)
{
    struct kaps_jr1_device *device = NULL;
    struct kaps_jr1_allocator * alloc = NULL;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%u %u %p %p\n", pair_type, flags, xpt, device_pp);

    if (!device_pp)
        status = KAPS_JR1_INVALID_ARGUMENT;
    else if (pair_type >= KAPS_JR1_DEVICE_PAIR_TYPE_MAX)
        status = KAPS_JR1_INVALID_ARGUMENT;

    if (status == KAPS_JR1_OK)
    {
        
        KAPS_JR1_STRY(kaps_jr1_default_allocator_create(&alloc));

        device = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_jr1_device));
        if (!device) {
            KAPS_JR1_STRY(kaps_jr1_default_allocator_destroy(alloc));
            status = KAPS_JR1_OUT_OF_MEMORY;
        }
        else
        {
            device->alloc = alloc;
        }
    }

    if (status == KAPS_JR1_OK)
    {
        flags |= KAPS_JR1_DEVICE_ISSU;
        status = kaps_jr1_device_init_internal(pair_type, flags, xpt, device, 0);
    }

    if (status == KAPS_JR1_OK)
    {
        *device_pp = device;

    }

    KAPS_JR1_TRACE_OUT("%p\n", *device_pp);
    return status;
}





kaps_jr1_status
kaps_jr1_device_construct_mask(
    struct kaps_jr1_db *in_db,
    uint8_t * g_mask)
{
    struct kaps_jr1_key_field *tmp = in_db->key->first_field;

    while (tmp)
    {
        if (tmp->type == KAPS_JR1_KEY_FIELD_HOLE && !tmp->do_not_bmr)
        {
            uint32_t offset = tmp->offset_1 / 8;
            uint32_t width = tmp->width_1 / 8;

            kaps_jr1_memset(&g_mask[offset], 0xff, width);
        }
        tmp = tmp->next;
    }
    return KAPS_JR1_OK;
}

int32_t
kaps_jr1_check_if_inst_needed(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_DMA)
            return 0;
    }

    return 1;
}

kaps_jr1_status
kaps_jr1_device_check_alg_db_single_device(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_c_list_iter db_it;
    struct kaps_jr1_list_node *db_el;
    uint32_t is_alg_db_present;
    struct kaps_jr1_device *cascade;

    is_alg_db_present = 0;
    kaps_jr1_c_list_iter_init(&device->db_list, &db_it);
    while ((db_el = kaps_jr1_c_list_iter_next(&db_it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(db_el);

        if (db->type != KAPS_JR1_DB_LPM && db->type != KAPS_JR1_DB_EM)
            continue;

        if (kaps_jr1_db_get_algorithmic(db))
            is_alg_db_present = 1;
    }

    if (is_alg_db_present)
    {
        cascade = device;
        while (cascade)
        {
            cascade->is_alg_db_present = 1;
            cascade = cascade->next_dev;
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_check_alg_db(
    struct kaps_jr1_device * device)
{
    while (device)
    {
        KAPS_JR1_STRY(kaps_jr1_device_check_alg_db_single_device(device));
        device = device->next_bc_device;
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_db_assign_seq_num(
    struct kaps_jr1_device * device)
{
    struct kaps_jr1_device *bc_device;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_seq_num_to_ptr *seq_num_to_ptr;
    uint32_t db_seq_num = 1, ad_seq_num = 1, hb_seq_num = 1;

    if (device->main_bc_device)
        device = device->main_bc_device;

    if (device->is_config_locked)
        return KAPS_JR1_OK;

    if (device->seq_num_to_ptr && device->seq_num_to_ptr->is_memory_allocated)
        return KAPS_JR1_OK;

    bc_device = device;

    seq_num_to_ptr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_seq_num_to_ptr));

    if (seq_num_to_ptr == NULL)
        return KAPS_JR1_OUT_OF_MEMORY;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        kaps_jr1_c_list_iter_init(&bc_device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
            struct kaps_jr1_db *tab = db;

            while (tab)
            {
                switch (tab->type)
                {
                    case KAPS_JR1_DB_LPM:
                    case KAPS_JR1_DB_EM:
                        tab->seq_num = db_seq_num++;
                        break;
                    case KAPS_JR1_DB_AD:
                        tab->seq_num = ad_seq_num++;
                        break;
                    case KAPS_JR1_DB_DMA:
                        break;
                    case KAPS_JR1_DB_HB:
                        tab->seq_num = hb_seq_num++;
                        break;
                    default:
                        kaps_jr1_sassert(0);
                        break;
                }
                tab = tab->next_tab;
            }
        }
    }

    if (db_seq_num > 255)
        return KAPS_JR1_EXCEEDED_NUM_DB;

    if (ad_seq_num > 255)
        return KAPS_JR1_EXCEEDED_NUM_AD_DB;

    if (hb_seq_num > 255)
        return KAPS_JR1_EXCEEDED_NUM_HB_DB;

    if (db_seq_num)
    {
        seq_num_to_ptr->db_ptr = device->alloc->xcalloc(device->alloc->cookie, db_seq_num, sizeof(struct kaps_jr1_db *));
        if (seq_num_to_ptr->db_ptr == NULL)
        {
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    if (ad_seq_num)
    {
        seq_num_to_ptr->ad_db_ptr = device->alloc->xcalloc(device->alloc->cookie, ad_seq_num, sizeof(struct kaps_jr1_db *));
        if (seq_num_to_ptr->ad_db_ptr == NULL)
        {
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr->db_ptr);
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    if (hb_seq_num)
    {
        seq_num_to_ptr->hb_db_ptr = device->alloc->xcalloc(device->alloc->cookie, hb_seq_num, sizeof(struct kaps_jr1_db *));
        if (seq_num_to_ptr->hb_db_ptr == NULL)
        {
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr->ad_db_ptr);
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr->db_ptr);
            device->alloc->xfree(device->alloc->cookie, seq_num_to_ptr);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
    }

    if (device->main_bc_device)
        device = device->main_bc_device;

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        kaps_jr1_c_list_iter_init(&bc_device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
            struct kaps_jr1_db *tab = db;

            while (tab)
            {
                switch (tab->type)
                {
                    case KAPS_JR1_DB_LPM:
                    case KAPS_JR1_DB_EM:
                        seq_num_to_ptr->db_ptr[tab->seq_num] = tab;
                        break;
                    case KAPS_JR1_DB_AD:
                        seq_num_to_ptr->ad_db_ptr[tab->seq_num] = (struct kaps_jr1_ad_db *) tab;
                        break;
                    case KAPS_JR1_DB_DMA:
                        break;
                    case KAPS_JR1_DB_HB:
                        seq_num_to_ptr->hb_db_ptr[tab->seq_num] = (struct kaps_jr1_hb_db *) tab;
                        break;
                    default:
                        kaps_jr1_sassert(0);
                        break;
                }
                tab = tab->next_tab;
            }
        }
    }

    seq_num_to_ptr->num_db = db_seq_num - 1;
    seq_num_to_ptr->num_ad_db = ad_seq_num - 1;
    seq_num_to_ptr->num_hb_db = hb_seq_num - 1;
    seq_num_to_ptr->is_memory_allocated = 1;
    if (device->main_bc_device)
        device = device->main_bc_device;

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_device *dev;

        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        for (dev = bc_device; dev; dev = dev->next_dev)
        {
            dev->seq_num_to_ptr = seq_num_to_ptr;
            if (dev->smt)
            {
                dev->smt->seq_num_to_ptr = seq_num_to_ptr;
            }
        }
    }
    return KAPS_JR1_OK;
}

static kaps_jr1_status
find_if_db_is_public(
    struct kaps_jr1_db *db)
{
    uint32_t em_len_at_beginning_1 = 0;
    struct kaps_jr1_key_field *f;
    uint8_t is_prefix_reached = 0;

    if (db->type != KAPS_JR1_DB_LPM)
        return KAPS_JR1_OK;

    for (f = db->key->first_field; f; f = f->next)
    {
        if (f->type == KAPS_JR1_KEY_FIELD_PREFIX)
            is_prefix_reached = 1;

        if (f->type == KAPS_JR1_KEY_FIELD_TABLE_ID || f->type == KAPS_JR1_KEY_FIELD_EM)
        {
            if (!is_prefix_reached)
            {
                em_len_at_beginning_1 += f->width_1;
            }
        }
    }

    
    if (em_len_at_beginning_1 <= 8)
    {
        db->is_public = 1;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_cr_update_nv(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * cb_fun)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    uint32_t db_iter;
    uint32_t nv_offset = *(cb_fun->nv_offset);
    uint8_t *tmp_ptr;

    db_iter = 0;

    cb_fun->nv_offset = &nv_offset;
    tmp_ptr = device->nv_ptr;

    
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

    device->nv_mem_mgr->offset_device_info_start = nv_offset;
    
    KAPS_JR1_STRY(kaps_jr1_wb_save_device_info(device, cb_fun));

    device->nv_mem_mgr->offset_device_lpm_info = nv_offset;

    if (device->lpm_mgr)
        KAPS_JR1_STRY(kaps_jr1_lpm_cr_save_state(device, cb_fun));

    device->nv_mem_mgr->offset_device_hb_info = nv_offset;
    if (device->aging_table && device->type == KAPS_JR1_DEVICE)
        KAPS_JR1_STRY(kaps_jr1_hb_cr_init(device, cb_fun));

    
    db_iter = 0;
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            device->nv_mem_mgr->offset_db_trie_has_been_built[db_iter] = nv_offset;
            nv_offset += (sizeof(uint32_t) * 2);
            if (nv_offset > device->nv_size)
                return KAPS_JR1_EXHAUSTED_NV_MEMORY;
            if (cb_fun->nv_ptr)
            {
                cb_fun->nv_ptr += (sizeof(uint32_t) * 2);
            }
            db_iter++;
        }
    }

    
    db_iter = 0;
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            device->nv_mem_mgr->offset_db_trie_sb_bitmap[db_iter] = nv_offset;
            nv_offset += (sizeof(uint32_t) * 2);
            if (nv_offset > device->nv_size)
                return KAPS_JR1_EXHAUSTED_NV_MEMORY;
            if (cb_fun->nv_ptr)
            {
                cb_fun->nv_ptr += (sizeof(uint32_t) * 2);
            }
            db_iter++;
        }
    }

    
    db_iter = 0;
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            device->nv_mem_mgr->offset_db_info_start[db_iter] = nv_offset;
            db->fn_table->save_cr_state(db, cb_fun);
            device->nv_mem_mgr->offset_db_info_end[db_iter] = nv_offset;
            db_iter++;
        }
    }

    
    device->nv_mem_mgr->offset_device_pending_list = nv_offset;
    nv_offset += sizeof(uint32_t);
    if (nv_offset > device->nv_size)
        return KAPS_JR1_EXHAUSTED_NV_MEMORY;
    if (cb_fun->nv_ptr)
        cb_fun->nv_ptr += sizeof(uint32_t);

    if (tmp_ptr)
    {
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->num_db;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_lpm_shadow_info;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_info_start;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_pending_list;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_lpm_info;
        tmp_ptr += sizeof(uint32_t);
        *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_device_hb_info;
        tmp_ptr += sizeof(uint32_t);

        for (db_iter = 0; db_iter < device->nv_mem_mgr->num_db; db_iter++)
        {
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_trie_has_been_built[db_iter];
            tmp_ptr += sizeof(uint32_t);
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_trie_sb_bitmap[db_iter];
            tmp_ptr += sizeof(uint32_t);
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_info_start[db_iter];
            tmp_ptr += sizeof(uint32_t);
            *(uint32_t *) tmp_ptr = device->nv_mem_mgr->offset_db_info_end[db_iter];
            tmp_ptr += sizeof(uint32_t);
        }
    }
    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_device_create_cr_pool_mgr(
    struct kaps_jr1_device * device,
    struct kaps_jr1_wb_cb_functions * cb_fun)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    uint8_t lpm_present = 0;
    uint8_t *end_nv_ptr = device->nv_ptr;

    cb_fun->read_fn = device->nv_read_fn;
    cb_fun->write_fn = device->nv_write_fn;
    cb_fun->handle = device->nv_handle;
    cb_fun->nv_ptr = device->nv_ptr;

    end_nv_ptr = end_nv_ptr + device->nv_size - 1;
    
    KAPS_JR1_STRY(kaps_jr1_device_nv_memory_manager_init(device));
    KAPS_JR1_STRY(kaps_jr1_cr_pool_mgr_init(device, end_nv_ptr, &device->nv_mem_mgr->kaps_jr1_cr_pool_mgr));

    
    *(cb_fun->nv_offset) += sizeof(uint32_t);   
    *(cb_fun->nv_offset) += sizeof(uint32_t);   
    *(cb_fun->nv_offset) += sizeof(uint32_t);   
    *(cb_fun->nv_offset) += sizeof(uint32_t);   
    *(cb_fun->nv_offset) += sizeof(uint32_t);   
    *(cb_fun->nv_offset) += sizeof(uint32_t);   

    *(cb_fun->nv_offset) += (device->nv_mem_mgr->num_db * 6 * sizeof(uint32_t));        

    if (cb_fun->nv_ptr)
    {
        cb_fun->nv_ptr += sizeof(uint32_t);     
        cb_fun->nv_ptr += sizeof(uint32_t);     
        cb_fun->nv_ptr += sizeof(uint32_t);     
        cb_fun->nv_ptr += sizeof(uint32_t);     
        cb_fun->nv_ptr += sizeof(uint32_t);     
        cb_fun->nv_ptr += sizeof(uint32_t);     
        cb_fun->nv_ptr += (device->nv_mem_mgr->num_db * 6 * sizeof(uint32_t));
    }

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
            lpm_present = 1;
    }

    device->nv_mem_mgr->offset_device_lpm_shadow_info = *(cb_fun->nv_offset);

    if (lpm_present)
    {
        kaps_jr1_lpm_cr_get_adv_ptr(device, cb_fun);
    }

    if (*(cb_fun->nv_offset) > device->nv_size)
        return KAPS_JR1_EXHAUSTED_NV_MEMORY;

    return KAPS_JR1_OK;
}

static kaps_jr1_status
kaps_jr1_device_update_db_handle_free_lists(
    struct kaps_jr1_device * device)
{
    uint32_t nv_offset = 0;
    struct kaps_jr1_wb_cb_functions cb_fun;
    struct kaps_jr1_device *cascade;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    for (cascade = device; cascade; cascade = cascade->next_dev)
    {
        cascade->issu_status = KAPS_JR1_ISSU_INIT;
        cascade->is_config_locked = 1;
        if (cascade->smt)
        {
            cascade->smt->issu_status = KAPS_JR1_ISSU_INIT;
            cascade->smt->is_config_locked = 1;
        }
    }

    if (device->nv_ptr)
    {
        uint32_t offset = 0;
        uint8_t *pending_list = NULL;

        
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_LPM)
                KAPS_JR1_STRY(db->fn_table->cr_reconcile(db));
        }
        offset = device->nv_mem_mgr->offset_device_pending_list;
        pending_list = (uint8_t *) device->nv_ptr + offset;
        *(uint32_t *) pending_list = 0;
    }

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_DMA)
            continue;

        if (db->is_bc_required && !db->is_main_bc_db)
            continue;

        if (KAPS_JR1_WB_HANDLE_TABLE_IS_FREELIST_INIT_DONE(db) == 0)
            KAPS_JR1_WB_HANDLE_TABLE_FREELIST_INIT(db);
    }

    if (device->nv_ptr)
    {
        nv_offset = 0;

        cb_fun.read_fn = device->nv_read_fn;
        cb_fun.write_fn = device->nv_write_fn;
        cb_fun.handle = device->nv_handle;
        cb_fun.nv_offset = &nv_offset;
        cb_fun.nv_ptr = device->nv_ptr;

        nv_offset = device->nv_mem_mgr->offset_device_info_start;
        cb_fun.nv_ptr += nv_offset;
        KAPS_JR1_STRY(kaps_jr1_wb_save_device_info(device, &cb_fun));
    }
    return KAPS_JR1_OK;
}

void
kaps_jr1_device_scan_databases(
    struct kaps_jr1_device *device)
{
    struct kaps_jr1_device *bc_device;
    uint32_t inplace_ad_present = 1;

    if (device->main_bc_device)
        device = device->main_bc_device;

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_c_list_iter it;
        struct kaps_jr1_list_node *el;

        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        kaps_jr1_c_list_iter_init(&bc_device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_LPM)
            {
                struct kaps_jr1_ad_db *ad_db = NULL;

                ad_db = (struct kaps_jr1_ad_db *) db->common_info->ad_info.ad;

                while (ad_db)
                {
                    if (kaps_jr1_resource_get_ad_type(&ad_db->db_info) == KAPS_JR1_AD_TYPE_INPLACE)
                        inplace_ad_present = 1;
                    ad_db = ad_db->next;
                }
            }
        }
    }

    bc_device = device;

    for (; bc_device; bc_device = bc_device->next_bc_device)
    {
        struct kaps_jr1_device *dev;

        if (bc_device->main_dev)
            bc_device = bc_device->main_dev;

        for (dev = bc_device; dev; dev = dev->next_dev)
        {
            dev->inplace_ad_present = inplace_ad_present;

            if (dev->smt)
            {
                dev->smt->inplace_ad_present = inplace_ad_present;
            }
        }
    }
}

kaps_jr1_status
kaps_jr1_device_lock(
    struct kaps_jr1_device *bc_device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_wb_cb_functions cb_fun;
    uint32_t nv_offset = 0;
    struct kaps_jr1_device *device;

    KAPS_JR1_TRACE_IN("%p\n", bc_device);

    if (!bc_device)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    for (device = bc_device; device; device = device->next_bc_device)
    {
        if (device->issu_in_progress)
            KAPS_JR1_STRY(kaps_jr1_wb_refresh_db_handle_in_map(device));
    }

    for (device = bc_device; device; device = device->next_bc_device)
    {
        device->execute_the_next_loop = 0;

        if (device->is_generic_bc_device)
        {
            continue;
        }

        if (device->main_dev)
            device = device->main_dev;

        if (device->is_config_locked)
            return KAPS_JR1_DEVICE_ALREADY_LOCKED;

        device->dynamic_alloc_fp = bc_device->dynamic_alloc_fp;

        
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            find_if_db_is_public(db);
        }
        
        {
            uint32_t db_count = 0;

            if (kaps_jr1_check_if_inst_needed(device) && kaps_jr1_c_list_count(&device->inst_list) == 0)
                return KAPS_JR1_DEVICE_NO_INSTRUCTIONS;

            kaps_jr1_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
                
                if (db->type != KAPS_JR1_DB_AD
                    && db->type != KAPS_JR1_DB_DMA && db->type != KAPS_JR1_DB_HB)
                {
                    struct kaps_jr1_db *next_tab = db;

                    for (; next_tab; next_tab = next_tab->next_tab)
                    {
                        if (next_tab->key == NULL)
                            return KAPS_JR1_MISSING_KEY;
                    }
                }
                db_count++;
            }

            if (db_count == 0)
            {
                return KAPS_JR1_DEVICE_NO_DB;
            }
        }

        if (device->issu_in_progress)
        {
            if (device->issu_status != KAPS_JR1_ISSU_RESTORE_START)
            {
                device->issu_in_progress = 0;
                if (device->smt)
                    device->smt->issu_in_progress = 0;
            }
        }

        
        if (device == bc_device)
        {
            KAPS_JR1_STRY(kaps_jr1_device_db_assign_seq_num(bc_device));
            kaps_jr1_device_scan_databases(bc_device);
        }

        
        if (device->flags & KAPS_JR1_DEVICE_ISSU)
        {
            if (device->issu_status == KAPS_JR1_ISSU_RESTORE_END || device->issu_status == KAPS_JR1_ISSU_RECONCILE_END)
            {
                KAPS_JR1_STRY(kaps_jr1_device_update_db_handle_free_lists(device));
                continue;
            }
        }

        if (device->bc_id == 0)
        {
            KAPS_JR1_STRY(kaps_jr1_device_check_alg_db(device));
        }

        if (device->bc_id == 0)
        {
            KAPS_JR1_STRY(kaps_jr1_device_check_lpm_constraints(device));
        }

        device->execute_the_next_loop = 1;
    }

    for (device = bc_device; device; device = device->next_bc_device)
    {
        if (!device->execute_the_next_loop)
            continue;
        if (device->main_dev)
            device = device->main_dev;

        KAPS_JR1_STRY(kaps_jr1_resource_finalize(device));
    }

    for (device = bc_device; device; device = device->next_bc_device)
    {

        if (!device->execute_the_next_loop)
            continue;

        if (device->main_dev)
            device = device->main_dev;

        if (device->nv_ptr && device->issu_status == KAPS_JR1_ISSU_INIT)
        {
            cb_fun.nv_offset = &nv_offset;
            KAPS_JR1_STRY(kaps_jr1_device_create_cr_pool_mgr(device, &cb_fun));
        }

        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            uint32_t org_bmp;
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_AD
                || db->type == KAPS_JR1_DB_DMA || db->type == KAPS_JR1_DB_HB)
                continue;
            org_bmp = db->device->db_bc_bitmap;
            db->device->db_bc_bitmap = db->bc_bitmap;
            KAPS_JR1_STRY(db->fn_table->lock_db(db));
            db->device->db_bc_bitmap = org_bmp;
        }

        if (device->type == KAPS_JR1_DEVICE)
        {
            device->is_config_locked = 1;
        }

        if (device->nv_ptr && device->issu_status == KAPS_JR1_ISSU_INIT)
        {
            KAPS_JR1_STRY(kaps_jr1_device_cr_update_nv(device, &cb_fun));
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_destroy(
    struct kaps_jr1_device * cascade_device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_device *device = cascade_device;
    struct kaps_jr1_allocator *alloc = NULL;
    uint32_t print_heap_usage;

    KAPS_JR1_TRACE_IN("%p\n", cascade_device);

    if (!device)
        return KAPS_JR1_INVALID_DEVICE_PTR;


    print_heap_usage = device->prop.print_heap_usage;

    if (device->pfx_log_records)
    {
        device->alloc->xfree(device->alloc->cookie, device->pfx_log_records);
    }

    
    device->fatal_transport_error = 0;

    if (device->is_wb_continue)
    {
        struct kaps_jr1_device *bc_device = device;

        device->issu_status = KAPS_JR1_ISSU_INIT;

        for (; bc_device; bc_device = bc_device->next_bc_device)
        {
            struct kaps_jr1_device *device = bc_device;
            struct kaps_jr1_device *tmp_device = device;

            tmp_device = device;
            while (tmp_device)
            {
                tmp_device->issu_in_progress = NlmTrue;
                if (tmp_device->smt)
                    tmp_device->smt->issu_in_progress = NlmTrue;
                tmp_device = tmp_device->next_dev;
            }
        }
    }

    while (cascade_device)
    {
        struct kaps_jr1_device *next_cascade_dev = cascade_device->next_dev;
        struct kaps_jr1_device *bc_device;

        for (bc_device = cascade_device; bc_device; bc_device = bc_device->next_bc_device)
        {
            struct kaps_jr1_db *db_list[64];
            int32_t db_count = 0, db_index;

            device = bc_device;
            if (device->is_generic_bc_device)
            {
                continue;
            }
            

            kaps_jr1_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

                if (db->type == KAPS_JR1_DB_AD
                    || db->type == KAPS_JR1_DB_HB || db->type == KAPS_JR1_DB_DMA)
                    continue;   

                db_list[db_count++] = db;
            }
            for (db_index = db_count - 1; db_index >= 0; db_index--)
            {
                struct kaps_jr1_db *db = db_list[db_index];
                struct kaps_jr1_db *ad_db, *ad_db_next;

                
                if (!db->is_destroyed)
                {
                    uint32_t org_bmp;

                    ad_db = db->common_info->ad_info.ad;

                    org_bmp = device->db_bc_bitmap;
                    device->db_bc_bitmap = db->bc_bitmap;
                    db->fn_table->db_destroy(db);
                    device->db_bc_bitmap = org_bmp;

                    while (ad_db)
                    {
                        ad_db_next = ad_db->next_tab;
                        ad_db->common_info->ad_info.db = NULL;
                        kaps_jr1_c_list_remove_node(&device->db_list, &ad_db->node, NULL);
                        KAPS_JR1_STRY(kaps_jr1_ad_db_destroy((struct kaps_jr1_ad_db *) ad_db));

                        ad_db = ad_db_next;
                    }
                }
            }

            

            kaps_jr1_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

                if (db->type != KAPS_JR1_DB_AD)
                    continue;
                
                db->common_info->ad_info.db = NULL;
                kaps_jr1_c_list_remove_node(&device->db_list, el, &it);
                KAPS_JR1_STRY(kaps_jr1_ad_db_destroy((struct kaps_jr1_ad_db *) db));
            }

            

            kaps_jr1_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

                if (db->type != KAPS_JR1_DB_HB)
                    continue;

                db->common_info->hb_info.db = NULL;
                kaps_jr1_c_list_remove_node(&device->db_list, el, &it);
                KAPS_JR1_STRY(kaps_jr1_hb_db_destroy((struct kaps_jr1_hb_db *) db));
            }

            

            if (device->hb_buffer && !device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, device->hb_buffer);
            device->hb_buffer = 0;

            if (device->hb_vbuffer && !device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, device->hb_vbuffer);
            device->hb_vbuffer = 0;

            if (device->aging_valid_memory)
            {
                device->alloc->xfree(device->alloc->cookie, device->aging_valid_memory);
                device->aging_valid_memory = NULL;
            }

            if (device->aging_table && !device->nv_ptr)
                device->alloc->xfree(device->alloc->cookie, device->aging_table);
            
            device->aging_table = NULL;

            if (device->small_aging_table)
                device->alloc->xfree(device->alloc->cookie, device->small_aging_table);

            device->small_aging_table = NULL;
        }

        for (bc_device = cascade_device; bc_device; bc_device = bc_device->next_bc_device)
        {
            device = bc_device;
            if (device->is_generic_bc_device)
            {
                continue;
            }
            

            kaps_jr1_c_list_iter_init(&device->inst_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_instruction *ins = KAPS_JR1_INSTLIST_TO_ENTRY(el);

                kaps_jr1_c_list_remove_node(&device->inst_list, el, &it);
                KAPS_JR1_STRY(kaps_jr1_instruction_destroy(ins));
            }
        }

        

        for (bc_device = cascade_device; bc_device; bc_device = bc_device->next_bc_device)
        {
            device = bc_device;
            kaps_jr1_lpm_destroy(device);
        }

        bc_device = cascade_device;
        while (bc_device)
        {
            struct kaps_jr1_device *device = bc_device;

            bc_device = bc_device->next_bc_device;

            kaps_jr1_resource_fini_module(device);
            kaps_jr1_device_advanced_destroy(device);

            if (!next_cascade_dev && (device->bc_id == 0) && device->seq_num_to_ptr)
            {
                if (device->seq_num_to_ptr->db_ptr)
                    device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr->db_ptr);
                if (device->seq_num_to_ptr->ad_db_ptr)
                    device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr->ad_db_ptr);
                if (device->seq_num_to_ptr->hb_db_ptr)
                    device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr->hb_db_ptr);

                device->alloc->xfree(device->alloc->cookie, device->seq_num_to_ptr);
            }

            if (device->smt)
                device->alloc->xfree(device->alloc->cookie, device->smt);
            if (device->description)
                device->alloc->xfree(device->alloc->cookie, device->description);

            alloc = device->alloc;
            device->alloc->xfree(device->alloc->cookie, device);

            if (print_heap_usage)
            {
                struct kaps_jr1_default_allocator_stats stats;

                kaps_jr1_default_allocator_get_stats(alloc, &stats);


                
                kaps_jr1_printf("HEAP     nallocs:%"PRIu64", nfrees:%"PRIu64", peak bytes:%"PRIu64", peak:%0.2fMB, cumulative:%0.2fMB\n",
                           stats.nallocs, stats.nfrees, stats.peak_bytes,
                           ((float) stats.peak_bytes) / (1024.0 * 1024.0),
                           ((float) stats.cumulative_bytes) / (1024.0 * 1024.0));

            }
            
            (void) kaps_jr1_default_allocator_destroy(alloc);
        }

        cascade_device = next_cascade_dev;
    }


    return KAPS_JR1_OK;
}

char *
kaps_jr1_device_db_name(
    struct kaps_jr1_db *db)
{
    switch (db->type)
    {
        case KAPS_JR1_DB_INVALID:
            return "ERROR";
        case KAPS_JR1_DB_LPM:
            return "LPM";
        case KAPS_JR1_DB_EM:
            return "EM";
        case KAPS_JR1_DB_AD:
            return "AD";
        case KAPS_JR1_DB_DMA:
            return "DMA";
        default:
            kaps_jr1_sassert(0);
    }
    return NULL;
}

kaps_jr1_status
kaps_jr1_device_set_property(
    struct kaps_jr1_device * device,
    enum kaps_jr1_device_properties property,
    ...)
{
    va_list a_list;
    char *desc;
    int32_t val;
    kaps_jr1_status status = KAPS_JR1_OK;
    struct kaps_jr1_device *dev;
    uint8_t *tmp_ptr;
    uint32_t mem_consumed = 0;

    KAPS_JR1_TRACE_IN("%p %u ", device, property);

    if (device == NULL)
    {
        KAPS_JR1_TRACE_PRINT("%s\n", " ");
        return KAPS_JR1_INVALID_DEVICE_PTR;
    }

    if (device->main_dev)
        dev = device->main_dev;
    else
        dev = device;

    va_start(a_list, property);

    switch (property)
    {
        case KAPS_JR1_DEVICE_PROP_RETURN_ERROR_ON_ASSERTS:
            val = va_arg(a_list, uint32_t);
            KAPS_JR1_TRACE_PRINT("%d", val);
            if (val > 1)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->prop.return_error_on_asserts = val;
                if (dev->smt)
                    device->smt->prop.return_error_on_asserts = val;
                if (dev->other_core)
                    device->other_core->prop.return_error_on_asserts = val;
            }
            break;

        case KAPS_JR1_DEVICE_PROP_DEBUG_LEVEL:
            
            if (device->is_config_locked)
            {
                status = KAPS_JR1_DEVICE_ALREADY_LOCKED;
                break;
            }

            val = va_arg(a_list, int32_t);
            KAPS_JR1_TRACE_PRINT("%d", val);

            
            if (val < 0)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            for (; dev; dev = dev->next_dev)
            {
                dev->debug_level = val;
                if (dev->smt)
                    dev->smt->debug_level = val;
            }
            break;

        case KAPS_JR1_DEVICE_PROP_DESCRIPTION:
            if (device->is_config_locked)
            {
                status = KAPS_JR1_DEVICE_ALREADY_LOCKED;
                break;
            }
            desc = va_arg(a_list, char *);
            KAPS_JR1_TRACE_PRINT("\"%s\"", desc);

            if (!desc)
            {
                status = KAPS_JR1_INVALID_DEVICE_DESC_PTR;
                break;
            }
            device->description = device->alloc->xcalloc(device->alloc->cookie, 1, (strlen(desc) + 1));
            if (!device->description)
            {
                status = KAPS_JR1_OUT_OF_MEMORY;
                break;
            }

            strcpy(device->description, desc);
            break;


        case KAPS_JR1_DEVICE_PROP_CRASH_RECOVERY:
        {
            if (device->is_config_locked)
            {
                status = KAPS_JR1_DEVICE_ALREADY_LOCKED;
                break;
            }

            kaps_jr1_resource_fini_module(device);

            device->is_fresh_nv = va_arg(a_list, uint32_t);
            KAPS_JR1_TRACE_PRINT("%d", device->is_fresh_nv);
            device->nv_ptr = va_arg(a_list, void *);
            KAPS_JR1_TRACE_PRINT("%p", device->nv_ptr);

            if (!device->nv_ptr)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }

            tmp_ptr = (uint8_t *) device->nv_ptr;
            device->nv_size = va_arg(a_list, uint32_t);
            KAPS_JR1_TRACE_PRINT("%d", device->nv_size);

            if (!device->nv_size)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }

            if (device->is_fresh_nv)
                kaps_jr1_memset(device->nv_ptr, 0, device->nv_size);

            status = kaps_jr1_resource_init_mem_map(device);
            if (status != KAPS_JR1_OK)
            {
                break;
            }

            if (device->type == KAPS_JR1_DEVICE)
            {
                status = kaps_jr1_initialize_advanced_shadow(device);
                if (status != KAPS_JR1_OK)
                {
                    break;
                }
            }

            dev = device;
            while (dev)
            {
                device->cr_status = (enum kaps_jr1_restore_status *) device->nv_ptr;
                device->nv_ptr = (void *) ((uint8_t *) device->nv_ptr + sizeof(enum kaps_jr1_restore_status));
                dev = dev->next_dev;
            }
            mem_consumed = (uint8_t *) device->nv_ptr - tmp_ptr;
            device->nv_size -= mem_consumed;

            dev = device;
            while (dev)
            {
                if (dev->smt)
                {
                    dev->smt->nv_ptr = device->nv_ptr;
                    dev->smt->nv_size = device->nv_size;
                }
                dev = dev->next_dev;
            }
            break;
        }


        case KAPS_JR1_DEVICE_PROP_DUMP_ON_ASSERT:
        {
            if (device->is_config_locked)
            {
                status = KAPS_JR1_DEVICE_ALREADY_LOCKED;
                break;
            }

            desc = va_arg(a_list, char *);
            KAPS_JR1_TRACE_PRINT("\"%s\"", desc);
            if (desc == NULL)
            {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }

            device->dump_on_assert = 1;
            strcpy(device->fname_dump, desc);

            break;
        }


        case KAPS_JR1_DEVICE_PROP_PRINT_HEAP_INFO:
        {
            device->prop.print_heap_usage = va_arg(a_list, uint32_t);
            break;
        }


        default:
            if (device->is_config_locked)
            {
                if ((int)property != 750)
                {
                    status = KAPS_JR1_DEVICE_ALREADY_LOCKED;
                    break;
                }
            }
            status = kaps_jr1_device_advanced_set_property(device, property, a_list);
            break;
    }

    va_end(a_list);

    KAPS_JR1_TRACE_PRINT("%s\n", " ");
    return status;
}

kaps_jr1_status
kaps_jr1_device_get_property(
    struct kaps_jr1_device * device,
    enum kaps_jr1_device_properties property,
    ...)
{
    va_list a_list;
    char **desc;
    int32_t *val;
    kaps_jr1_status status = KAPS_JR1_OK;

    KAPS_JR1_TRACE_IN("%p %u\n", device, property);

    if (device == NULL)
        return KAPS_JR1_INVALID_DEVICE_PTR;

    va_start(a_list, property);
    switch (property)
    {
        case KAPS_JR1_DEVICE_PROP_DEBUG_LEVEL:
            val = va_arg(a_list, int32_t *);
            *val = device->debug_level;
            break;

        case KAPS_JR1_DEVICE_PROP_DESCRIPTION:
            desc = va_arg(a_list, char **);
            if (!desc)
            {
                status = KAPS_JR1_INVALID_DEVICE_DESC_PTR;
                break;
            }
            *desc = device->description;
            break;

        default:
            status = kaps_jr1_device_advanced_get_property(device, property, a_list);
            break;
    }

    va_end(a_list);
    return status;
}

kaps_jr1_status
kaps_jr1_device_print_html(
    struct kaps_jr1_device * device,
    FILE * fp)
{
    KAPS_JR1_TRACE_IN("%p %p\n", device, fp);
    if (!device || !fp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_JR1_ISSU_INIT)
        {
            return KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    kaps_jr1_resource_print_html(device, fp);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_print_sw_state(
    struct kaps_jr1_device * device,
    FILE * fp)
{
    KAPS_JR1_TRACE_IN("%p %p\n", device, fp);
    if (!device || !fp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_JR1_ISSU_INIT)
        {
            return KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    device->map_print = 1;
    kaps_jr1_resource_print_html(device, fp);
    device->map_print = 0;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_print(
    struct kaps_jr1_device * device,
    FILE * fp)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    KAPS_JR1_TRACE_IN("%p %p\n", device, fp);

    if (!device || !fp)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!device->is_config_locked)
        return KAPS_JR1_DEVICE_UNLOCKED;

    if (device->flags & KAPS_JR1_DEVICE_ISSU)
    {
        if (device->issu_status != KAPS_JR1_ISSU_INIT)
        {
            return KAPS_JR1_ISSU_IN_PROGRESS;
        }
    }

    if (device->description)
        kaps_jr1_fprintf(fp, "%s\n", device->description);

    kaps_jr1_c_list_iter_init(&device->inst_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_instruction *instruction = KAPS_JR1_INSTLIST_TO_ENTRY(el);

        kaps_jr1_instruction_print(instruction, fp);
    }

    return KAPS_JR1_OK;
}

const char *
kaps_jr1_device_get_sdk_version(
    void)
{
    KAPS_JR1_TRACE_PRINT("%s\n", " ");
    return KAPS_JR1_SDK_VERSION_FULL;
}

kaps_jr1_status
kaps_jr1_device_start_transaction(
    struct kaps_jr1_device * device)
{
    KAPS_JR1_TRACE_IN("%p\n", device);
    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    
    if (!device->nv_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    device->txn_in_progress = 1;
    *device->cr_status = KAPS_JR1_RESTORE_NO_CHANGE;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_end_transaction(
    struct kaps_jr1_device * device)
{
    uint32_t offset;
    uint8_t *pending_count;

    KAPS_JR1_TRACE_IN("%p\n", device);
    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    
    if (!device->nv_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    *device->cr_status = KAPS_JR1_RESTORE_CHANGES_ABORTED;
    offset = device->nv_mem_mgr->offset_device_pending_list;
    pending_count = (uint8_t *) device->nv_ptr + offset;
    *(uint32_t *) pending_count = 0;
    device->txn_in_progress = 0;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_query_restore_status(
    struct kaps_jr1_device * device,
    enum kaps_jr1_restore_status * status)
{
    KAPS_JR1_TRACE_IN("%p %p\n", device, status);
    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!device->nv_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    *status = *device->cr_status;
    KAPS_JR1_TRACE_OUT("%d\n", *status);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_clear_restore_status(
    struct kaps_jr1_device * device)
{
    KAPS_JR1_TRACE_IN("%p\n", device);
    if (!device)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (!device->nv_ptr)
        return KAPS_JR1_INVALID_ARGUMENT;

    *device->cr_status = KAPS_JR1_RESTORE_NO_CHANGE;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_device_log_error(
    struct kaps_jr1_device * device,
    kaps_jr1_status return_status,
    char *fmt,
    ...)
{
    va_list ap;
    va_start(ap, fmt);

    kaps_jr1_sassert(device);

    if (device->main_dev)
        device = device->main_dev;

    kaps_jr1_vsnprintf(device->verbose_error_string, sizeof(device->verbose_error_string), fmt, ap);
    va_end(ap);

    return return_status;
}

const char *
kaps_jr1_device_get_last_error(
    struct kaps_jr1_device *device)
{
    KAPS_JR1_TRACE_IN("%p\n", device);
    kaps_jr1_sassert(device);

    if (device->main_dev)
        device = device->main_dev;

    return device->verbose_error_string;
}

kaps_jr1_status
kaps_jr1_device_thread_init(
    struct kaps_jr1_device * device,
    uint32_t tid,
    struct kaps_jr1_device ** thr_device)
{
    return KAPS_JR1_OK;
}

struct kaps_jr1_device *
kaps_jr1_get_main_bc_device(
    struct kaps_jr1_device *device)
{
    kaps_jr1_sassert(device);

    return device;
}





uint32_t
kaps_jr1_device_get_final_level_offset(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    uint32_t final_level_offset;
    
    final_level_offset = device->uda_offset;

    return final_level_offset;
}


uint32_t
kaps_jr1_device_get_num_final_level_bbs(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    uint32_t num_bbs_in_final_level;
    
    num_bbs_in_final_level = device->hw_res->total_lpus;


    return num_bbs_in_final_level;
}


struct kaps_jr1_aging_entry*
kaps_jr1_device_get_active_aging_table(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    struct kaps_jr1_aging_entry *active_aging_table = NULL;

    active_aging_table = device->aging_table;

    return active_aging_table;
}



uint32_t 
kaps_jr1_device_get_active_num_hb_blocks(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    uint32_t num_active_hb_blocks = 0;


    num_active_hb_blocks = device->num_hb_blocks;


    return num_active_hb_blocks;
}



uint32_t kaps_jr1_device_get_active_num_hb_rows(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    uint32_t num_active_hb_rows = 0;
    
    num_active_hb_rows = device->num_rows_in_hb_block;

    return num_active_hb_rows;
    
}




