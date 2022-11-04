

#include "kaps_jr1_ab.h"
#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_key_internal.h"
#include "kaps_jr1_instruction_internal.h"
#include "kaps_jr1_dma_internal.h"
#include "kaps_jr1_ad_internal.h"

#define MIN_NUM_SMALL_BB_PER_ROW (4)
#define MAX_NUM_SMALL_BB_PER_ROW (12)



void kaps_jr1_print_detailed_stats_html(
    FILE * f,
    struct kaps_jr1_device *dev,
    struct kaps_jr1_db *db);





kaps_jr1_status
kaps_jr1_initialize_mem_map(
    struct kaps_jr1_device * device)
{
    int32_t i;
    struct memory_map *mem_map;

    device->hw_res->incr_in_bbs = 4;
    mem_map = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct memory_map));
    if (!mem_map)
        return KAPS_JR1_OUT_OF_MEMORY;

    mem_map->ab_memory = device->alloc->xcalloc(device->alloc->cookie, device->num_ab, sizeof(struct kaps_jr1_ab_info));
    if (!mem_map->ab_memory)
    {
        device->alloc->xfree(device->alloc->cookie, mem_map);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    for (i = 0; i < device->num_ab; i++)
    {
        mem_map->ab_memory[i].ab_num = i;
        mem_map->ab_memory[i].device = device;
    }

    mem_map->rpb_ab = device->alloc->xcalloc(device->alloc->cookie, KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS, 
                                sizeof(struct kaps_jr1_ab_info));
    
    if (!mem_map->rpb_ab)
    {
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    for (i = 0; i < KAPS_JR1_HW_MAX_NUM_RPB_BLOCKS; ++i)
    {
        mem_map->rpb_ab[i].ab_num = i;
        mem_map->rpb_ab[i].device = device;
    }

    mem_map->num_bb = device->hw_res->total_num_bb / device->hw_res->num_bb_in_one_bb_cascade;
    mem_map->bb_map = device->alloc->xcalloc(device->alloc->cookie,
                                         mem_map->num_bb * device->hw_res->num_bb_in_one_bb_cascade, sizeof(struct bb_info));
    if (!mem_map->bb_map)
    {
        device->alloc->xfree(device->alloc->cookie, mem_map->ab_memory);
        device->alloc->xfree(device->alloc->cookie, mem_map);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    for (i = 0; i < mem_map->num_bb * device->hw_res->num_bb_in_one_bb_cascade; i++)
    {
        mem_map->bb_map[i].width_1 = KAPS_JR1_BKT_WIDTH_1;
        mem_map->bb_map[i].row = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct bb_partition_info));

        if (!mem_map->bb_map)
        {
            device->alloc->xfree(device->alloc->cookie, mem_map->ab_memory);

            for (i--; i >= 0; i--)
                device->alloc->xfree(device->alloc->cookie, mem_map->bb_map[i].row);

            device->alloc->xfree(device->alloc->cookie, mem_map->bb_map);
            device->alloc->xfree(device->alloc->cookie, mem_map);
            return KAPS_JR1_OUT_OF_MEMORY;
        }
        mem_map->bb_map[i].row->start_row = 0;
        mem_map->bb_map[i].row->num_rows = device->hw_res->num_rows_in_bb;
        mem_map->bb_map[i].row->owner = NULL;
        mem_map->bb_map[i].row->next = NULL;

        mem_map->bb_map[i].bb_num_rows = device->hw_res->num_rows_in_bb;
        mem_map->bb_map[i].num_units_in_bb = 1;
    }

    device->map = mem_map;

    return KAPS_JR1_OK;
}



kaps_jr1_status
kaps_jr1_config_small_bb_for_two_level_db(
    struct kaps_jr1_db *db)
{
    struct kaps_jr1_device *device = db->device;
    uint32_t i;
    uint32_t num_lpu;

    
    db->hw_res.db_res->start_lpu = db->hw_res.db_res->start_small_bb_nr;
    db->hw_res.db_res->end_lpu = db->hw_res.db_res->end_small_bb_nr;

    num_lpu = db->hw_res.db_res->end_lpu - db->hw_res.db_res->start_lpu + 1;

    db->hw_res.db_res->lsn_info[0].max_lsn_size = num_lpu;

    db->common_info->uda_mb = ((float) num_lpu * 
            device->hw_res->num_rows_in_small_bb * KAPS_JR1_BKT_WIDTH_1) / (1024 * 1024);

    db->common_info->total_bb_size_in_bits = num_lpu * 
            device->hw_res->num_rows_in_small_bb * KAPS_JR1_BKT_WIDTH_1;
    
    kaps_jr1_memset(db->hw_res.db_res->lsn_info[0].alloc_udm, 0,
                sizeof(db->hw_res.db_res->lsn_info[0].alloc_udm));
    
    for (i = db->hw_res.db_res->start_lpu; i <= db->hw_res.db_res->end_lpu; ++i)
    {
        db->hw_res.db_res->lsn_info[0].alloc_udm[0][0][i] = 1;
    }

    return KAPS_JR1_OK;
}





kaps_jr1_status
kaps_jr1_append_to_rpb_ab(struct kaps_jr1_db *cur_db, uint32_t i)
{
    struct kaps_jr1_device *device = cur_db->device;
    struct kaps_jr1_c_list_iter rpb_ab_it;
    struct kaps_jr1_list_node *rpb_ab_el;
    struct kaps_jr1_ab_info *ab;

    
    kaps_jr1_c_list_iter_init(&cur_db->hw_res.db_res->rpb_ab_list, &rpb_ab_it);
    while ((rpb_ab_el = kaps_jr1_c_list_iter_next(&rpb_ab_it)) != NULL)
    {
        ab = KAPS_JR1_ABLIST_TO_ABINFO(rpb_ab_el);

        while (ab->dup_ab)
            ab = ab->dup_ab;

        ab->dup_ab = &device->map->rpb_ab[i];
        device->map->rpb_ab[i].dup_ab = NULL;
        
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_assign_rpb_abs(struct kaps_jr1_db *cur_db, uint32_t i)
{
    struct kaps_jr1_device *device;
    

    if (cur_db ) 
    {
        device = cur_db->device;
            
        device->map->rpb_ab[i].db = cur_db;

        if (cur_db->hw_res.db_res->rpb_ab_list.count == 0)
        {
            
            kaps_jr1_c_list_add_tail(&cur_db->hw_res.db_res->rpb_ab_list, 
                                &device->map->rpb_ab[i].ab_node);
        }
        else
        {
            
            kaps_jr1_append_to_rpb_ab(cur_db, i);
        }
        
        cur_db->common_info->num_ab = 1;
    }

    return KAPS_JR1_OK;
}





kaps_jr1_status
kaps_jr1_resource_configure_bbs(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db)
{
    struct kaps_jr1_c_list_iter it2;
    struct kaps_jr1_list_node *el2;
    struct kaps_jr1_ab_info *ab_info;
    uint8_t register_data[4] = { 0 };
    struct kaps_jr1_ab_info *cur_dup_ab;
    int32_t bit_pos, bb_index;
    uint32_t num_bits_per_ab;
    uint32_t i, value;
    int8_t pair[2];

    num_bits_per_ab = 2;
    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID || device->id == KAPS_JR1_QUMRAN_DEVICE_ID
        || device->id == KAPS_JR1_QUX_DEVICE_ID)
        num_bits_per_ab = 1;

    kaps_jr1_c_list_iter_init(&db->hw_res.db_res->ab_list, &it2);
    while ((el2 = kaps_jr1_c_list_iter_next(&it2)) != NULL)
    {
        ab_info = KAPS_JR1_ABLIST_TO_ABINFO(el2);
        if (ab_info->db->is_clone || ab_info->db->parent)
            continue;

        cur_dup_ab = ab_info;
        bit_pos = 0;
        value = 0;
        pair[0] = -1;
        pair[1] = -1;
        for (i = 0; i < 2; ++i)
        {
            if (cur_dup_ab)
            {
                value = cur_dup_ab->ab_num;
                pair[i] = value;
                if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
                    value = cur_dup_ab->ab_num / 2;
            }

            KapsJr1WriteBitsInArray(register_data, sizeof(register_data), bit_pos + num_bits_per_ab - 1, bit_pos, value);
            if (device->id == KAPS_JR1_QUMRAN_DEVICE_ID || device->id == KAPS_JR1_QUX_DEVICE_ID)
                break;

            if (cur_dup_ab)
                cur_dup_ab = cur_dup_ab->dup_ab;

            bit_pos += num_bits_per_ab;
        }

        if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
        {
            if (pair[0] != -1 && pair[1] != -1)
            {
                
                if (pair[0] > pair[1])
                {
                    int8_t temp = pair[0];

                    pair[0] = pair[1];
                    pair[1] = temp;
                }

                
                if (pair[0] == 0 && pair[1] == 1)
                {
                    KapsJr1WriteBitsInArray(register_data, sizeof(register_data), 1, 0, 0);
                }
                else if (pair[0] == 2 && pair[1] == 3)
                {
                    KapsJr1WriteBitsInArray(register_data, sizeof(register_data), 1, 0, 3);
                }
                else if (pair[0] == 0 && pair[1] == 3)
                {
                    KapsJr1WriteBitsInArray(register_data, sizeof(register_data), 1, 0, 2);
                }
                else if (pair[0] == 1 && pair[1] == 2)
                {
                    KapsJr1WriteBitsInArray(register_data, sizeof(register_data), 1, 0, 1);
                }
                else
                {
                    return KAPS_JR1_INVALID_BB_CONFIG;
                }
            }
        }
    }

    for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
    {
        int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_bb_cascade;
        int32_t i = 0;

        for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
        {
            KAPS_JR1_STRY(kaps_jr1_dm_alg_reg_write(device, bb_blk_id, 0x00000021, sizeof(register_data), register_data));
            bb_blk_id++;
        }
    }
    return KAPS_JR1_OK;
}


static kaps_jr1_status
kaps_jr1_fit_bbs(
    struct kaps_jr1_device *device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    uint32_t num_bb, bb_index;
    float bb_size;
    uint32_t i, occupied_bb;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            bb_size =
                (((float) device->hw_res->num_rows_in_bb * KAPS_JR1_BKT_WIDTH_1 * device->hw_res->num_bb_in_one_bb_cascade) /
                 (1024 * 1024));
            db->common_info->uda_mb = db->hw_res.db_res->lsn_info[0].max_lsn_size * bb_size;

            if (!db->common_info->uda_mb)
                db->common_info->uda_mb = 1;
        }
    }

    if (!device->nv_ptr && device->issu_in_progress)
        return KAPS_JR1_OK;

    bb_index = 0;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        uint32_t start_found = 0;

        if (db->type == KAPS_JR1_DB_LPM)
        {

            if (db->common_info->capacity == 0)
            {
                db->hw_res.db_res->lsn_info[0].max_lsn_size = db->device->hw_res->incr_in_bbs;
                continue;
            }

            kaps_jr1_memset(db->hw_res.db_res->lsn_info[0].alloc_udm, 0, sizeof(db->hw_res.db_res->lsn_info[0].alloc_udm));
            num_bb = db->hw_res.db_res->lsn_info[0].max_lsn_size;
            while (num_bb)
            {
                if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_bb_cascade))
                    return KAPS_JR1_RESOURCE_FIT_FAIL;

                occupied_bb = 0;
                for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
                {
                    if (device->map->bb_map[bb_index].row->owner)
                    {
                        if (start_found)
                            return KAPS_JR1_RESOURCE_FIT_FAIL;
                        occupied_bb = 1;
                        bb_index++;
                    }
                }

                if (occupied_bb)
                    continue;

                if (!start_found)
                {
                    db->hw_res.db_res->start_lpu = bb_index / device->hw_res->num_bb_in_one_bb_cascade;
                    start_found = 1;
                }

                db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index / device->hw_res->num_bb_in_one_bb_cascade] =
                    1;

                for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
                {
                    device->map->bb_map[bb_index].row->owner = db;
                    bb_index++;
                }
                num_bb--;
            }
            db->hw_res.db_res->end_lpu = (bb_index / device->hw_res->num_bb_in_one_bb_cascade) - 1;
        }
    }

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            KAPS_JR1_STRY(kaps_jr1_resource_configure_bbs(device, db));
        }
    }

    return KAPS_JR1_OK;
}


static void
kaps_jr1_standalone_get_free_bbs_on_device(
    struct kaps_jr1_device *device,
    uint32_t * num_bb)
{
    uint32_t num_rows = 0;
    int32_t i;
    struct bb_partition_info *row;
    uint32_t num_bb_per_cascade;

    if (device->hw_res->reduced_algo_levels)
    {
        *num_bb = device->hw_res->total_small_bb;
        return;
    }

    num_bb_per_cascade = device->map->num_bb;
    for (i = 0; i < num_bb_per_cascade * device->hw_res->num_bb_in_one_bb_cascade; i += device->hw_res->num_bb_in_one_bb_cascade)
    {
        row = device->map->bb_map[i].row;
        while (row)
        {
            if (row->owner == NULL)
                num_rows += row->num_rows;
            row = row->next;
        }
    }

    *num_bb = num_rows / device->hw_res->num_rows_in_bb;
}


static void
kaps_jr1_allocate_static_lpm_resource(
    struct kaps_jr1_device *device,
    uint32_t capacity,
    uint8_t is_v6,
    uint32_t * num_bb)
{
    
    uint32_t max_cap = 300000;
    uint32_t nbb = 0;

    if (is_v6)
        max_cap /= 2;

    if (device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
        max_cap /= 2;

    if (device->id == KAPS_JR1_QUX_DEVICE_ID)
        max_cap /= 8;

    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
    {
        if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4
            || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM0)
        {
            max_cap *= 4;
        }
    }

    nbb = (capacity * device->map->num_bb + max_cap - 1) / max_cap;

    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
    {
        if (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4
            || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4)
        {
            
            nbb = ((nbb + 3) / 4) * 4;
        }
    }

    *num_bb = nbb;
}


static void
swap_val(
    uint32_t *x,
    uint32_t *y)
{
    uint32_t temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

static kaps_jr1_status
kaps_jr1_calculate_bb_proportion(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db **db_arr,
    uint32_t num_dbs,
    uint32_t bb_arr[],
    uint32_t total_bb_needed,
    uint32_t available_bb,
    uint32_t proportional_bb_arr[],
    uint32_t num_lpm_db)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    int32_t i, j;
    struct kaps_jr1_db *temp_db;


    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        uint32_t avg_num_bb_per_lpm_db = available_bb / num_lpm_db;
        uint32_t num_assigned_bb, num_bbs_left, num_4bb_units_left;
        uint32_t num_bbs_still_needed_arr[4];

        
        if (avg_num_bb_per_lpm_db < 4)
            return KAPS_JR1_RESOURCE_FIT_FAIL;

        if (num_lpm_db > 4)
            return KAPS_JR1_RESOURCE_FIT_FAIL;


        
        for (i = 0; i < num_lpm_db - 1; i++)
        {
           for (j = i+1; j < num_lpm_db; ++j)
           {
                if (db_arr[i]->common_info->capacity < 
                    db_arr[j]->common_info->capacity)
                {
                    temp_db = db_arr[i];
                    db_arr[i] = db_arr[j];
                    db_arr[j] = temp_db;
                    
                    swap_val(&bb_arr[i], &bb_arr[j]);
                    swap_val(&proportional_bb_arr[i], &proportional_bb_arr[j]);
                }
           }
        }
        

        
        num_assigned_bb = 0;
        for (j = 0; j < num_lpm_db; ++j)
        {
            proportional_bb_arr[j] = 4;
            num_assigned_bb += 4;

            if (bb_arr[j] > 4)
            {
                num_bbs_still_needed_arr[j] = bb_arr[j] - 4;
            }
            else
            {
                num_bbs_still_needed_arr[j] = 0;
            }
        }

        num_bbs_left = available_bb - num_assigned_bb;

        num_4bb_units_left = num_bbs_left / 4;


        
        while (num_4bb_units_left)
        {
           for (j = 0; j < num_lpm_db; ++j)
           {
                if (num_bbs_still_needed_arr[j])
                {
                    proportional_bb_arr[j] += 4;

                    if (num_bbs_still_needed_arr[j] > 4)
                        num_bbs_still_needed_arr[j] -= 4;
                    else
                        num_bbs_still_needed_arr[j] = 0;

                    num_4bb_units_left--;
                }

                if (num_4bb_units_left == 0)
                    break;
           }
            
        }

        
        return KAPS_JR1_OK;

    }
   

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            for (j = 0; j < num_dbs; j++)
            {
                if (db_arr[j] == db)
                {

                    proportional_bb_arr[j] = (bb_arr[j] * available_bb + total_bb_needed - 1) / total_bb_needed;

                    db->hw_res.db_res->lsn_info[0].max_lsn_size = 0;
                    break;
                }
            }
        }
    }

    return KAPS_JR1_OK;
}

static kaps_jr1_status kaps_jr1_allocate_dma_bbs(struct kaps_jr1_device *device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_fast_bitmap dma_tag_free_fbmp;
    struct memory_map *mem_map = resource_kaps_jr1_get_memory_map(device);
    struct kaps_jr1_db *last_dma_db = NULL;
    uint32_t total_num_bb_assigned_to_dma_dbs;

    KAPS_JR1_STRY(kaps_jr1_init_bmp(&dma_tag_free_fbmp, device->alloc, KAPS_JR1_DMA_TAG_MAX + 1, 1));

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        struct kaps_jr1_dma_db *dma_db;

        if (db->type == KAPS_JR1_DB_DMA) {
            dma_db = (struct kaps_jr1_dma_db *) db;
            if (dma_db->dma_tag != -1) {
                if (!kaps_jr1_get_bit(&dma_tag_free_fbmp, dma_db->dma_tag))
                    return KAPS_JR1_INVALID_RESOURCE_UDA;

                kaps_jr1_reset_bit(&dma_tag_free_fbmp, dma_db->dma_tag);
            }
        }
    }

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        struct kaps_jr1_dma_db *dma_db;

        if (db->type == KAPS_JR1_DB_DMA) {
            dma_db = (struct kaps_jr1_dma_db *) db;
            if (dma_db->dma_tag == -1) {
                dma_db->dma_tag = kaps_jr1_find_first_bit_set(&dma_tag_free_fbmp, 0, KAPS_JR1_DMA_TAG_MAX);
                if (dma_db->dma_tag == -1)
                    return KAPS_JR1_RESOURCE_FIT_FAIL;

                kaps_jr1_reset_bit(&dma_tag_free_fbmp, dma_db->dma_tag);
                
            }
        }
    }

    total_num_bb_assigned_to_dma_dbs = 0;

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        struct kaps_jr1_dma_db *dma_db;

        if (db->type == KAPS_JR1_DB_DMA) {
            dma_db = (struct kaps_jr1_dma_db *) db;
            if (db->common_info->user_specified) {
                if (mem_map->bb_map[db->hw_res.db_res->start_lpu].row->owner != NULL)
                    return KAPS_JR1_INVALID_RESOURCE_UDA;

                mem_map->bb_map[db->hw_res.db_res->start_lpu].row->owner = db;

                kaps_jr1_set_bit(dma_db->alloc_fbmap, db->hw_res.db_res->start_lpu);

                last_dma_db = db;
                total_num_bb_assigned_to_dma_dbs++;
            }
        }
    }

    kaps_jr1_free_bmp(&dma_tag_free_fbmp, device->alloc);

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        struct kaps_jr1_dma_db *dma_db;
        int32_t bb_index;

        if (db->type == KAPS_JR1_DB_DMA) {
            dma_db = (struct kaps_jr1_dma_db *) db;
            if (!db->common_info->user_specified) {
                for (bb_index = 0; bb_index < mem_map->num_bb * device->hw_res->num_bb_in_one_bb_cascade; bb_index++) {
                    if (mem_map->bb_map[bb_index].row->owner)
                        continue;
                    
                    db->hw_res.db_res->start_lpu = bb_index;
                    db->hw_res.db_res->end_lpu = bb_index;
                    mem_map->bb_map[bb_index].row->owner = db;
                    
                    kaps_jr1_set_bit(dma_db->alloc_fbmap, bb_index);
                    total_num_bb_assigned_to_dma_dbs++;
                    break;
                }
                if (bb_index == (mem_map->num_bb * device->hw_res->num_bb_in_one_bb_cascade))
                    return KAPS_JR1_RESOURCE_FIT_FAIL;
            }
            
            last_dma_db = db;
        }
    }

    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        uint32_t num_units_assigned_to_dma_dbs;
        uint32_t num_4bb_units_assigned_to_dma ;
        uint32_t num_extra_dma_bb_units_to_assign ;
        int32_t bb_index, num_extra_dma_bbs_to_mark_as_used;

        
        num_units_assigned_to_dma_dbs = (total_num_bb_assigned_to_dma_dbs + device->hw_res->num_bb_in_one_bb_cascade - 1) / device->hw_res->num_bb_in_one_bb_cascade;

        num_4bb_units_assigned_to_dma = ((num_units_assigned_to_dma_dbs + 3) / 4 ) ;

        
        num_extra_dma_bb_units_to_assign = (num_4bb_units_assigned_to_dma * 4) - num_units_assigned_to_dma_dbs;

        
        num_extra_dma_bbs_to_mark_as_used = num_extra_dma_bb_units_to_assign * device->hw_res->num_bb_in_one_bb_cascade; 

        
        if (num_extra_dma_bbs_to_mark_as_used)
        {
       
            for (bb_index = 0; bb_index < mem_map->num_bb * device->hw_res->num_bb_in_one_bb_cascade; bb_index++) 
            {
                if (mem_map->bb_map[bb_index].row->owner)
                    continue;

                mem_map->bb_map[bb_index].row->owner = last_dma_db;
                num_extra_dma_bbs_to_mark_as_used--;

                if (num_extra_dma_bbs_to_mark_as_used == 0)
                    break;
                
            }
        }
    }

    
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL) {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);
        struct kaps_jr1_dma_db *dma_db;
        int32_t bb_index;
        uint32_t register_data;
        uint32_t dma_addr_start_bit = 4;

        if (db->type == KAPS_JR1_DB_DMA) {
            dma_db = (struct kaps_jr1_dma_db *) db;
            register_data = 0;
            bb_index = db->hw_res.db_res->start_lpu;

            if (device->id == KAPS_JR1_QUX_DEVICE_ID || device->id == KAPS_JR1_QUMRAN_DEVICE_ID)
                dma_addr_start_bit = 1;
            else if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID)
                dma_addr_start_bit = 2;

            if (device->issu_in_progress == 1)
                continue;

            KapsJr1WriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), dma_addr_start_bit + 8, dma_addr_start_bit + 8, 1);
            KapsJr1WriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), dma_addr_start_bit + 7, dma_addr_start_bit, dma_db->dma_tag);
            KAPS_JR1_STRY(kaps_jr1_dm_alg_reg_write(device, bb_index, 0x00000021, sizeof(register_data), (uint8_t *) &register_data));

            if (device->id == KAPS_JR1_DEFAULT_DEVICE_ID) {
                
                KAPS_JR1_STRY(kaps_jr1_dm_alg_reg_read(device, bb_index, 0x00000023, sizeof(register_data), (uint8_t *) &register_data));
                KapsJr1WriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), 16, 16, 1);
                KAPS_JR1_STRY(kaps_jr1_dm_alg_reg_write(device, bb_index, 0x00000023, sizeof(register_data), (uint8_t *) &register_data));
                KapsJr1WriteBitsInArray((uint8_t *) &register_data, sizeof(register_data), 16, 16, 0);
                KAPS_JR1_STRY(kaps_jr1_dm_alg_reg_write(device, bb_index, 0x00000023, sizeof(register_data), (uint8_t *) &register_data));
            }
        }
    }

    return KAPS_JR1_OK;
}


static kaps_jr1_status
kaps_jr1_standalone_allocate_bbs(
    struct kaps_jr1_device *device)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    struct kaps_jr1_db **db_arr = NULL;
    uint32_t num_bb, available_bb, total_bb_needed = 0;
    uint32_t max_width = 0;
    uint32_t *bb_arr, *proportional_bb_arr, i = 0, j = 0;
    uint32_t num_dbs = kaps_jr1_c_list_count(&device->db_list);
    uint32_t num_lpm_db;

    KAPS_JR1_STRY(kaps_jr1_allocate_dma_bbs(device));


    if (!device->nv_ptr && device->issu_in_progress)
    {
        return KAPS_JR1_OK;
    }

    db_arr = device->alloc->xcalloc(device->alloc->cookie, num_dbs, sizeof(struct kaps_jr1_db *));
    if (!db_arr)
        return KAPS_JR1_OUT_OF_MEMORY;

    proportional_bb_arr = device->alloc->xcalloc(device->alloc->cookie, num_dbs, sizeof(uint32_t));
    if (!proportional_bb_arr)
    {
        device->alloc->xfree(device->alloc->cookie, db_arr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    bb_arr = device->alloc->xcalloc(device->alloc->cookie, num_dbs, sizeof(uint32_t));
    if (!bb_arr)
    {
        device->alloc->xfree(device->alloc->cookie, db_arr);
        device->alloc->xfree(device->alloc->cookie, proportional_bb_arr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    num_lpm_db = 0;
    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *tab, *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            for (tab = db; tab; tab = tab->next_tab)
            {
                if (tab->key->width_1 > max_width)
                    max_width = tab->key->width_1;
            }

            if (db->hw_res.db_res->num_bbs)
            {
                num_bb = db->hw_res.db_res->num_bbs;
            }
            else
            {
                kaps_jr1_allocate_static_lpm_resource(device, db->common_info->capacity, (max_width > 128), &num_bb);
            }

            if (num_bb == 0)
                num_bb = 1;

            db_arr[i] = db;
            bb_arr[i] = num_bb;
            total_bb_needed += bb_arr[i];
            i++;
            num_lpm_db++;
        }
    }

    kaps_jr1_standalone_get_free_bbs_on_device(device, &available_bb);

    if (device->id == KAPS_JR1_JERICHO_PLUS_DEVICE_ID &&
        (device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_FM4
         || device->silicon_sub_type == KAPS_JR1_JERICHO_PLUS_SUB_TYPE_JER_MODE_FM4))
    {
        if (available_bb % 4 != 0)
            available_bb = (available_bb / 4) * 4;
    }

    
    if (total_bb_needed > available_bb)
    {
        uint32_t num_allocated_bb = 0;
        uint32_t done = 0;

        KAPS_JR1_STRY(kaps_jr1_calculate_bb_proportion(device, db_arr, num_dbs, bb_arr, total_bb_needed, available_bb,
                                proportional_bb_arr, num_lpm_db));

        while (!done)
        {
            
            kaps_jr1_c_list_iter_init(&device->db_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

                if (db->type == KAPS_JR1_DB_LPM)
                {
                    for (j = 0; j < i; j++)
                    {
                        if (db_arr[j] == db)
                        {
                            if (proportional_bb_arr[j])
                            {
                                if (num_allocated_bb < available_bb)
                                {
                                    db->hw_res.db_res->lsn_info[0].max_lsn_size += 1;
                                    num_allocated_bb++;
                                    proportional_bb_arr[j]--;
                                }
                                else
                                {
                                    
                                    if (db->hw_res.db_res->lsn_info[0].max_lsn_size == 0)
                                    {
                                        device->alloc->xfree(device->alloc->cookie, db_arr);
                                        device->alloc->xfree(device->alloc->cookie, proportional_bb_arr);
                                        device->alloc->xfree(device->alloc->cookie, bb_arr);
                                        return KAPS_JR1_RESOURCE_FIT_FAIL;
                                    }
                                }
                            }

                            break;
                        }
                    }
                }
            }

            if (num_allocated_bb >= available_bb)
            {
                done = 1;
                break;
            }
        }
    }
    else
    {
        kaps_jr1_c_list_iter_init(&device->db_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

            if (db->type == KAPS_JR1_DB_LPM)
            {
                for (j = 0; j < i; j++)
                {
                    if (db_arr[j] == db)
                    {
                        db->hw_res.db_res->lsn_info[0].max_lsn_size = bb_arr[j];
                        db_arr[j] = NULL;
                        break;
                    }
                }
            }
        }
    }

    device->alloc->xfree(device->alloc->cookie, db_arr);
    device->alloc->xfree(device->alloc->cookie, proportional_bb_arr);
    device->alloc->xfree(device->alloc->cookie, bb_arr);

    return KAPS_JR1_OK;
}



kaps_jr1_status
kaps_jr1_rxc_process_jr1(
    struct kaps_jr1_device *device)
{
    
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;
    uint32_t nlpm = 0;
    uint32_t db_id = 0;
    struct kaps_jr1_db *tmp;
    uint8_t *data = NULL;

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            nlpm++;
        }
    }

    if (nlpm > device->num_ab)
        return KAPS_JR1_RESOURCE_FIT_FAIL;

    if (device->id == KAPS_JR1_DEFAULT_DEVICE_ID)
    {
        
        KAPS_JR1_STRY(kaps_jr1_dm_reset_blks(device, 0, 0, 0, data));
        
        KAPS_JR1_STRY(kaps_jr1_dm_enumerate_blks(device, 0, 0, 0, data));
    }


    KAPS_JR1_STRY(kaps_jr1_standalone_resource_assign_rpbs(device));


    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        
        if (db->type == KAPS_JR1_DB_LPM)
        {
            for (tmp = db; tmp; tmp = tmp->next_tab)
                tmp->ltr_db_id = db_id++;
        }
    }

    KAPS_JR1_STRY(kaps_jr1_standalone_allocate_bbs(device));

    KAPS_JR1_STRY(kaps_jr1_fit_bbs(device));

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_LPM)
        {
            db->common_info->finalized_resources = 1;

            if (db->type == KAPS_JR1_DB_LPM && device->num_hb_blocks)
            {
                db->common_info->start_hb_blk = db->hw_res.db_res->start_lpu * device->hw_res->num_bb_in_one_bb_cascade;
                db->common_info->num_hb_blks =
                    db->hw_res.db_res->lsn_info[0].max_lsn_size * device->hw_res->num_bb_in_one_bb_cascade;

            }
        }
    }

    return KAPS_JR1_OK;
}







kaps_jr1_status
kaps_jr1_resource_process(
    struct kaps_jr1_device * device)
{
    KAPS_JR1_STRY(kaps_jr1_rxc_process_jr1(device));

    return KAPS_JR1_OK;
        
}


kaps_jr1_status
kaps_jr1_resource_dynamic_uda_alloc(
    struct kaps_jr1_device * device,
    struct memory_map * mem_map,
    struct kaps_jr1_db * db,
    int8_t dt_index,
    int32_t * p_num_lpu,
    uint8_t udms[])
{
    int32_t bb_index, i, num_bb;

    if (db->common_info->enable_dynamic_allocation == 0)
        return KAPS_JR1_OUT_OF_UDA;

    if (mem_map->bb_map[0].row->owner == NULL)
    {
        bb_index = 0;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb)
        {
            if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_bb_cascade))
                return KAPS_JR1_OUT_OF_UDA;

            for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                if (device->map->bb_map[bb_index].row->owner)
                    return KAPS_JR1_OUT_OF_UDA;
                bb_index++;
            }
            num_bb--;
        }
        db->hw_res.db_res->start_lpu = 0;
        db->hw_res.db_res->end_lpu = (bb_index / device->hw_res->num_bb_in_one_bb_cascade) - 1;

        for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
        {
            int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_bb_cascade;
            int32_t i = 0;

            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                kaps_jr1_sassert(device->map->bb_map[bb_blk_id].row->owner == NULL);
                device->map->bb_map[bb_blk_id].row->owner = db;
                bb_blk_id++;
            }
        }

    }
    else
    {

        bb_index = (device->map->num_bb - device->hw_res->incr_in_bbs) * device->hw_res->num_bb_in_one_bb_cascade;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb)
        {
            if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_bb_cascade))
                return KAPS_JR1_OUT_OF_UDA;

            for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                if (device->map->bb_map[bb_index].row->owner)
                    return KAPS_JR1_OUT_OF_UDA;
                bb_index++;
            }
            num_bb--;
        }
        db->hw_res.db_res->start_lpu = device->map->num_bb - device->hw_res->incr_in_bbs;
        db->hw_res.db_res->end_lpu = device->map->num_bb - 1;

        for (bb_index = db->hw_res.db_res->start_lpu; bb_index <= db->hw_res.db_res->end_lpu; bb_index++)
        {
            int32_t bb_blk_id = bb_index * device->hw_res->num_bb_in_one_bb_cascade;
            int32_t i = 0;

            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                kaps_jr1_sassert(device->map->bb_map[bb_blk_id].row->owner == NULL);
                device->map->bb_map[bb_blk_id].row->owner = db;
                bb_blk_id++;
            }
        }
    }

    *p_num_lpu = device->hw_res->incr_in_bbs;
    for (i = 0; i < *p_num_lpu; i++)
    {
        udms[i] = (db->hw_res.db_res->start_lpu + i) * KAPS_JR1_UDM_PER_UDC;
    }

    KAPS_JR1_STRY(kaps_jr1_resource_configure_bbs(device, db));
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_resource_expand_uda_mgr_regions(
    struct kaps_jr1_uda_mgr * mgr)
{
    struct kaps_jr1_db *db = mgr->db;
    struct kaps_jr1_device *device = db->device;
    int32_t bb_index, i, num_bb;

    if (mgr->db->common_info->enable_dynamic_allocation == 0 || mgr->enable_compaction == 0)
        return KAPS_JR1_OUT_OF_UDA;

    
    bb_index = (db->hw_res.db_res->end_lpu + 1) * device->hw_res->num_bb_in_one_bb_cascade;
    num_bb = device->hw_res->incr_in_bbs;
    while (num_bb)
    {

        if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_bb_cascade))
            break;

        for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
        {
            if (device->map->bb_map[bb_index].row->owner)
                break;
            
            bb_index++;
        }

        if (i < device->hw_res->num_bb_in_one_bb_cascade)
            break;

        num_bb--;
    }

    if (num_bb == 0)
    {
        int32_t new_region_no, old_num_lpu;
        int32_t region_no = 0;
        int32_t i, width;

        new_region_no = mgr->no_of_regions;
        old_num_lpu = mgr->region_info[region_no].num_lpu;
        width = device->hw_res->incr_in_bbs;

        kaps_jr1_memcpy(&mgr->region_info[new_region_no], &mgr->region_info[region_no], sizeof(mgr->region_info[0]));

        for (i = 0; i < old_num_lpu; i++)
        {
            mgr->region_info[new_region_no].udms[i] = mgr->region_info[region_no].udms[i];
        }

        for (i = 0; i < width; i++)
        {
            mgr->region_info[new_region_no].udms[old_num_lpu + i] =
                (db->hw_res.db_res->end_lpu + 1 + i) * KAPS_JR1_UDM_PER_UDC;
        }

        mgr->region_info[new_region_no].num_lpu += width;

        KAPS_JR1_STRY(kaps_jr1_uda_mgr_compact(mgr, 0, new_region_no));
        mgr->uda_mgr_update_lsn_size(mgr->db);

        bb_index = (db->hw_res.db_res->end_lpu + 1) * device->hw_res->num_bb_in_one_bb_cascade;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb)
        {

            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index / device->hw_res->num_bb_in_one_bb_cascade] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                device->map->bb_map[bb_index].row->owner = db;
                bb_index++;
            }

            num_bb--;
        }

        db->hw_res.db_res->end_lpu += device->hw_res->incr_in_bbs;
        db->hw_res.db_res->lsn_info[0].max_lsn_size += device->hw_res->incr_in_bbs;
        mgr->max_lpus_in_a_chunk += device->hw_res->incr_in_bbs;
        KAPS_JR1_STRY(kaps_jr1_resource_configure_bbs(device, db));
        return KAPS_JR1_OK;
    }

    
    bb_index = (db->hw_res.db_res->start_lpu - device->hw_res->incr_in_bbs) * device->hw_res->num_bb_in_one_bb_cascade;
    num_bb = device->hw_res->incr_in_bbs;
    while (num_bb > 0)
    {

        if (bb_index < 0)
            break;

        if (bb_index >= (device->map->num_bb * device->hw_res->num_bb_in_one_bb_cascade))
            break;

        for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
        {
            if (device->map->bb_map[bb_index].row->owner)
                break;
            
            bb_index++;
        }

        if (i < device->hw_res->num_bb_in_one_bb_cascade)
            break;

        num_bb--;
    }

    if (num_bb == 0)
    {
        int32_t new_region_no, old_num_lpu;
        int32_t region_no = 0;
        int32_t i, width;

        new_region_no = mgr->no_of_regions;
        old_num_lpu = mgr->region_info[region_no].num_lpu;
        width = device->hw_res->incr_in_bbs;

        kaps_jr1_memcpy(&mgr->region_info[new_region_no], &mgr->region_info[region_no], sizeof(mgr->region_info[0]));

        for (i = 0; i < width; i++)
        {
            mgr->region_info[new_region_no].udms[i] = (db->hw_res.db_res->start_lpu - width + i) * KAPS_JR1_UDM_PER_UDC;
        }

        for (i = 0; i < old_num_lpu; i++)
        {
            kaps_jr1_sassert(width + i < mgr->max_num_uda_chunk_levels);
            mgr->region_info[new_region_no].udms[width + i] = mgr->region_info[region_no].udms[i];
        }

        mgr->region_info[new_region_no].num_lpu += width;

        KAPS_JR1_STRY(kaps_jr1_uda_mgr_compact(mgr, 0, new_region_no));
        mgr->uda_mgr_update_lsn_size(mgr->db);

        bb_index = (db->hw_res.db_res->start_lpu - device->hw_res->incr_in_bbs) * device->hw_res->num_bb_in_one_bb_cascade;
        num_bb = device->hw_res->incr_in_bbs;
        while (num_bb > 0)
        {
            db->hw_res.db_res->lsn_info[0].alloc_udm[device->core_id][0][bb_index / device->hw_res->num_bb_in_one_bb_cascade] = 1;
            for (i = 0; i < device->hw_res->num_bb_in_one_bb_cascade; i++)
            {
                device->map->bb_map[bb_index].row->owner = db;
                bb_index++;
            }
            num_bb--;
        }

        db->hw_res.db_res->start_lpu -= device->hw_res->incr_in_bbs;
        db->hw_res.db_res->lsn_info[0].max_lsn_size += device->hw_res->incr_in_bbs;
        mgr->max_lpus_in_a_chunk += device->hw_res->incr_in_bbs;
        KAPS_JR1_STRY(kaps_jr1_resource_configure_bbs(device, db));
        return KAPS_JR1_OK;
    }

    return KAPS_JR1_OUT_OF_UDA;
}

static void
kaps_jr1_print_db_key(
    struct kaps_jr1_key *key,
    FILE * fp)
{
    struct kaps_jr1_key_field *field = key->first_field;
    uint32_t fcount = 0, count = 0, next_offset = 0;

    for (field = key->first_field; field; field = field->next)
    {
        if (field->type == KAPS_JR1_KEY_FIELD_HOLE && !field->is_usr_bmr)
            continue;
        fcount++;
    }
    do
    {
        for (field = key->first_field; field; field = field->next)
        {
            if (next_offset != field->orig_offset_1)
                continue;
            count++;
            next_offset = next_offset + field->width_1;
            kaps_jr1_print_to_file(key->device, fp, "%s(%d) ", field->name, field->width_1);
            break;
        }
    }
    while (count < fcount);
}


static void
kaps_jr1_print_capacity(
    struct kaps_jr1_device *device,
    uint32_t capacity,
    FILE * f)
{
    if (capacity >= 1024 && capacity % 1024 == 0)
    {
        capacity /= 1024;
        if (capacity >= 1024 && capacity % 1024 == 0)
        {
            capacity /= 1024;
            kaps_jr1_print_to_file(device, f, "%dM", capacity);
        }
        else
        {
            kaps_jr1_print_to_file(device, f, "%dK", capacity);
        }
    }
    else
    {
        kaps_jr1_print_to_file(device, f, "%d", capacity);
    }
}

static void
kaps_jr1_print_db_name(
    struct kaps_jr1_db *db,
    FILE * f)
{
    if (db->parent)
    {
        if (db->is_clone)
        {
            if (!db->clone_of->parent)
                kaps_jr1_print_to_file(db->device, f, "%s%d Clone%d", kaps_jr1_device_db_name(db->parent), db->clone_of->tid,
                                   db->tid);
            else
                kaps_jr1_print_to_file(db->device, f, "%s%d Table%d Clone%d",
                                   kaps_jr1_device_db_name(db->clone_of->parent),
                                   db->clone_of->parent->tid, db->clone_of->tid, db->tid);
        }
        else
        {
            kaps_jr1_print_to_file(db->device, f, "%s%d Table%d", kaps_jr1_device_db_name(db->parent), db->parent->tid,
                               db->tid);
        }
    }
    else
    {
        kaps_jr1_print_to_file(db->device, f, "%s%d", kaps_jr1_device_db_name(db), db->tid);
    }
}


static void
kaps_jr1_print_dba(
    struct kaps_jr1_device *device,
    FILE * f,
    struct memory_map *mem_map)
{
    float ystart;
    int32_t i, j, k, count = 0, ab_no = 0;
    float xstart = 6.5;

    kaps_jr1_print_to_file(device, f, "<text x=\"12.7cm\" y=\"1.2cm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n");
    kaps_jr1_print_to_file(device, f, "RPBs\n");
    kaps_jr1_print_to_file(device, f, "</text>\n");

    kaps_jr1_print_to_file(device, f, "<g id=\"RPB\">\n");
    ystart = 1.5;

    for (i = 0; i < device->num_dba_sb; i++)
    {
        kaps_jr1_print_to_file(device, f, "    <g id=\"RPB%d\">\n", count * 8 + i);
        for (k = 0; k < device->num_ab_per_sb; k++)
        {
            struct kaps_jr1_db *db = device->map->ab_memory[ab_no].db;

            kaps_jr1_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"2.50cm\" "
                               "height=\"2.50cm\" fill=\"%s\" stroke=\"black\" stroke-width=\".02cm\"/>\n",
                               xstart, ystart, db ? db->hw_res.db_res->color : "none");
            if (db && db->is_clone)
            {
                for (j = 0; j < 16; j++)
                {
                    kaps_jr1_print_to_file(device, f, "    <line x1=\"%.02fcm\" y1=\"%.02fcm\" x2=\"%.02fcm\" "
                                       "y2=\"%.02fcm\" stroke=\"black\"/>\n",
                                       xstart, ystart + j * 0.15, xstart + 2.50, ystart + j * 0.15 + 0.15);
                }
            }
            ab_no++;
        }
        xstart += 3.25;
        kaps_jr1_print_to_file(device, f, "    </g>\n");
    }

    kaps_jr1_print_to_file(device, f, "</g>\n");
}




static void
kaps_jr1_print_bbs(
    struct kaps_jr1_device *device,
    struct memory_map *mem_map,
    FILE * f)
{
    int32_t i, j;
    float xstart, ystart;

    kaps_jr1_print_to_file(device, f, "<text x=\"12.0cm\" y=\"4.8cm\" "
                       "font-family=\"Verdana\" font-size=\"14\" font-weight=\"bold\" fill=\"black\" >\n");
    kaps_jr1_print_to_file(device, f, "Bucket Blocks\n");
    kaps_jr1_print_to_file(device, f, "</text>\n");

    kaps_jr1_print_to_file(device, f, "<g id=\"UDA\">\n");
    kaps_jr1_print_to_file(device, f, "    <g id=\"LPU\"/>\n");
    kaps_jr1_print_to_file(device, f, "        <rect x=\"5.0cm\" y=\"5.0cm\" width=\"15.7cm\" "
                       "height=\"0.3cm\" fill=\"black\"/>\n");
    kaps_jr1_print_to_file(device, f, "    </g>\n");
    xstart = 5.0;
    ystart = 5.5;
    for (i = 0; i < mem_map->num_bb; i++)
    {
        for (j = 0; j < device->hw_res->num_bb_in_one_bb_cascade; j++)
        {
            struct kaps_jr1_db *db = device->map->bb_map[j + (i * device->hw_res->num_bb_in_one_bb_cascade)].row->owner;

            kaps_jr1_print_to_file(device, f, "    <g id=\"BB%d\">\n", i);

            kaps_jr1_print_to_file(device, f, "        <rect x=\"%.02fcm\" y=\"%.02fcm\" width=\"0.7cm\" "
                               "height=\"%.02fcm\" fill=\"%s\" stroke=\"black\" stroke-width=\".05cm\"/>\n",
                               xstart, ystart + (j * 1.0), 0.7, db ? db->hw_res.db_res->color : "none");
            kaps_jr1_print_to_file(device, f, "    </g>\n");
        }
        xstart += 1.0;
    }
    kaps_jr1_print_to_file(device, f, "</g>\n");
}

void
kaps_jr1_print_database_key_structure(
    struct kaps_jr1_device *device,
    FILE * f)
{
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    kaps_jr1_print_to_file(device, f, "<br>\n");
    kaps_jr1_print_to_file(device, f, "<h3>Database Key Structure</h3>\n");

    kaps_jr1_print_to_file(device, f, "<table>\n");
    kaps_jr1_print_to_file(device, f, "<tbody>\n");
    kaps_jr1_print_to_file(device, f, "<tr class =\"broadcom2\">\n");

    kaps_jr1_print_to_file(device, f, "<th>Color</th>\n");

    kaps_jr1_print_to_file(device, f, "<th>Name</th>\n");
    kaps_jr1_print_to_file(device, f, "<th>Key Width</th>\n");
    kaps_jr1_print_to_file(device, f, "<th>Achieved Capacity</th>\n");

    kaps_jr1_print_to_file(device, f, "<th>Key Structure</th>\n");
    kaps_jr1_print_to_file(device, f, "</tr>\n");

    kaps_jr1_c_list_iter_init(&device->db_list, &it);
    while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
    {
        struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

        if (db->type == KAPS_JR1_DB_AD || db->type == KAPS_JR1_DB_HB)
            continue;

        for (; db; db = db->next_tab)
        {
            kaps_jr1_print_to_file(device, f, "<tr>\n");

            kaps_jr1_print_to_file(device, f, "  <td bgcolor=%s></td>\n", db->hw_res.db_res->color);

            kaps_jr1_fprintf(f, "<td>");
            kaps_jr1_print_db_name(db, f);
            kaps_jr1_print_to_file(device, f, "</td>");
            kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", db->width.key_width_1);

            if (db->is_clone)
                kaps_jr1_print_to_file(device, f, "  <td> - </td>\n");
            else
                kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", kaps_jr1_c_list_count(&db->db_list));

            kaps_jr1_print_to_file(device, f, "<td>");
            if (db->key)
                kaps_jr1_print_db_key(db->key, f);
            kaps_jr1_print_to_file(device, f, "</td>");
            kaps_jr1_print_to_file(device, f, "</tr>\n");
        }
    }
    kaps_jr1_print_to_file(device, f, "</tbody>\n");
    kaps_jr1_print_to_file(device, f, "</table>\n");
    kaps_jr1_print_to_file(device, f, "<br>\n");
}



void
kaps_jr1_resource_print_html(
    struct kaps_jr1_device *device,
    FILE * f)
{
    struct memory_map *mem_map = device->map;
    int32_t i, j, num_smt;
    struct kaps_jr1_c_list_iter it;
    struct kaps_jr1_list_node *el;

    if (device->map_print)
    {
        device->map_print_offset = 0;
        device->hw_res->device_print_advance = 2;
    }

    
    kaps_jr1_print_to_file(device, f, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\"\n");
    kaps_jr1_print_to_file(device, f, "\"http://www.w3.org/TR/html4/strict.dtd\">\n");
    kaps_jr1_print_to_file(device, f, "<HTML lang=\"en\">\n");
    kaps_jr1_print_to_file(device, f, "<HEAD>\n");
    kaps_jr1_print_to_file(device, f, "<STYLE type=\"text/css\">\n");
    kaps_jr1_print_to_file(device, f, "td,th {padding-bottom:4pt;padding-top:4pt;padding-left:4pt;padding-right:4pt}\n");
    kaps_jr1_print_to_file(device, f, "table {border-collapse:collapse}\n");
    kaps_jr1_print_to_file(device, f, "td {text-align:center;font-family:Courier New;font-weight:bold;font-size:100%%}\n");
    kaps_jr1_print_to_file(device, f, "table,td,th {border:2px solid #adafb2}\n");
    kaps_jr1_print_to_file(device, f, "tr.broadcom1 {background:#e31837;color:#ffffff}\n");
    kaps_jr1_print_to_file(device, f, "tr.broadcom2 {background:#005568;color:#ffffff}\n");
    kaps_jr1_print_to_file(device, f, "tr.broadcom3 {background:#ffd457;color:#000000}\n");
    kaps_jr1_print_to_file(device, f, "body {margin:20px 20px 20px 20px}\n");
    kaps_jr1_print_to_file(device, f, "</STYLE>\n");
    kaps_jr1_print_to_file(device, f, "</HEAD>\n");

    kaps_jr1_print_to_file(device, f, "<BODY>\n");
    if (device->description)
    {
        kaps_jr1_print_to_file(device, f, "<h2>%s</h2>\n", device->description);
    }
    else
    {
        kaps_jr1_print_to_file(device, f, "<h2>KBP Scenario Output</h2>\n");
    }

    {
        
        kaps_jr1_print_to_file(device, f, "<h3>Databases</h3>\n");
        kaps_jr1_print_to_file(device, f, "<table>\n");
        kaps_jr1_print_to_file(device, f, "<tbody>\n");
        kaps_jr1_print_to_file(device, f, "<tr class=\"broadcom2\"> \n");
        kaps_jr1_print_to_file(device, f, "  <th>Color</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>Name</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>Description</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>Key width</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>Capacity</th>\n");


        kaps_jr1_print_to_file(device, f, "  <th>AD<br>entries*width,Mb</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>Algorithmic</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>BB</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>DT(SRAM,DBA)</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>AB</th>\n");


        kaps_jr1_print_to_file(device, f, "  <th>LSN Size</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>LSN Mb</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>Failed to fit</th>\n");
        kaps_jr1_print_to_file(device, f, "</tr>\n");

        if (device->smt)
            num_smt = 2;
        else
            num_smt = 1;
        for (i = 0; i < num_smt; i++)
        {
            struct kaps_jr1_device *dev;

            if (i == 0)
            {
                dev = device;
            }
            else
            {
                dev = device->smt;
            }

            kaps_jr1_c_list_iter_init(&dev->db_list, &it);
            while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
            {
                struct kaps_jr1_db *db = KAPS_JR1_SSDBLIST_TO_ENTRY(el);

                if (db->type == KAPS_JR1_DB_AD || db->type == KAPS_JR1_DB_HB)
                    continue;

                kaps_jr1_print_to_file(device, f, "<tr>\n");
                kaps_jr1_print_to_file(device, f, "  <td bgcolor=%s></td>\n", db->hw_res.db_res->color);
                kaps_jr1_print_to_file(device, f, "  <td>%s%d", kaps_jr1_device_db_name(db), db->tid);

                if (device->smt)
                {
                    if (i == 0)
                        kaps_jr1_print_to_file(device, f, " (Thread 0)");
                    else
                        kaps_jr1_print_to_file(device, f, " (Thread 1)");
                }
                kaps_jr1_print_to_file(device, f, "</td>\n");

                kaps_jr1_print_to_file(device, f, "<td>%s</td>\n", db->description ? db->description : "-");
                kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n",
                                   (db->type == KAPS_JR1_DB_DMA ? KAPS_JR1_DMA_WIDTH_1 : db->key->width_1));
                kaps_jr1_print_to_file(device, f, "  <td>");
                kaps_jr1_print_capacity(device, db->common_info->capacity, f);
                kaps_jr1_print_to_file(device, f, "</td>\n");


                {

                    if (db->common_info->ad_info.ad)
                    {
                        kaps_jr1_print_to_file(device, f, "  <td>");
                        kaps_jr1_print_capacity(device, db->common_info->ad_info.ad->common_info->capacity, f);
                        kaps_jr1_print_to_file(device, f, "*%db,%dMb</td>\n",
                                           db->common_info->ad_info.ad->width.ad_width_1,
                                           db->common_info->ad_info.ad->common_info->uda_mb);
                    }
                    else
                    {
                        kaps_jr1_print_to_file(device, f, "  <td>-</td>\n");
                    }
                    if (kaps_jr1_db_get_algorithmic(db))
                        kaps_jr1_print_to_file(device, f, "  <td>yes</td>\n");
                    else
                        kaps_jr1_print_to_file(device, f, "  <td>no</td>\n");
                    kaps_jr1_print_to_file(device, f, "  <td>%d,%d</td>\n", db->hw_res.db_res->num_dt,
                                       db->hw_res.db_res->num_dba_dt);
                    kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", db->common_info->num_ab);
                }

                kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", db->hw_res.db_res->lsn_info[0].max_lsn_size);
                kaps_jr1_print_to_file(device, f, "  <td>%.02f</td>\n",
                                   ((db->common_info->total_bb_rows * KAPS_JR1_BKT_WIDTH_1) / (1024.0 * 1024.0)));

                if (db->common_info->fit_error == 0)
                {
                    kaps_jr1_print_to_file(device, f, "  <td> - </td>\n");
                }
                else
                {
                    kaps_jr1_print_to_file(device, f, "  <td> ");
                    if (db->common_info->fit_error & FAILED_FIT_DBA)
                        kaps_jr1_print_to_file(device, f, "DBA ");
                    if (db->common_info->fit_error & FAILED_FIT_LSN)
                        kaps_jr1_print_to_file(device, f, "LSN ");
                    if (db->common_info->fit_error & FAILED_FIT_AD)
                        kaps_jr1_print_to_file(device, f, "AD ");
                    if (db->common_info->fit_error & FAILED_FIT_IT)
                        kaps_jr1_print_to_file(device, f, "IT ");
                    kaps_jr1_print_to_file(device, f, "</td>\n");
                }
                kaps_jr1_print_to_file(device, f, "</tr>\n");
            }
        }

        kaps_jr1_print_to_file(device, f, "</tbody>\n");
        kaps_jr1_print_to_file(device, f, "</table>\n");
    }

    kaps_jr1_print_database_key_structure(device, f);

    kaps_jr1_print_to_file(device, f, "<h3>Instructions</h3>\n");
    kaps_jr1_print_to_file(device, f, "<table>\n");
    kaps_jr1_print_to_file(device, f, "<tbody>\n");
    kaps_jr1_print_to_file(device, f, "<tr class =\"broadcom2\">\n");
    kaps_jr1_print_to_file(device, f, "<th>ID</th>\n");
    kaps_jr1_print_to_file(device, f, "<th>Master key width</th>\n");

    for (i = 0; i < device->max_num_searches; i++)
    {
        kaps_jr1_print_to_file(device, f, "<th>%d</th>\n", i);
    }

    kaps_jr1_print_to_file(device, f, "</tr>\n");

    for (j = 0; j < num_smt; j++)
    {
        struct kaps_jr1_device *dev;

        if (j == 0)
        {
            dev = device;
        }
        else
        {
            dev = device->smt;
        }

        kaps_jr1_c_list_iter_init(&dev->inst_list, &it);
        while ((el = kaps_jr1_c_list_iter_next(&it)) != NULL)
        {
            struct kaps_jr1_instruction *instruction = KAPS_JR1_INSTLIST_TO_ENTRY(el);
            struct kaps_jr1_db *db[KAPS_JR1_HW_MAX_SEARCH_DB] = { 0 };

            if (instruction->is_installed == 0)
                continue;

            kaps_jr1_print_to_file(device, f, "<tr>\n");
            kaps_jr1_print_to_file(device, f, "<td>%d</td>\n", instruction->id);
            kaps_jr1_print_to_file(device, f, "<td>%d</td>\n", instruction->master_key->width_1);

            for (i = 0; i < device->max_num_searches; i++)
            {
                uint32_t s_interface;

                if (!instruction->desc[i].db)
                    continue;
                s_interface = instruction->desc[i].result_id;
                db[s_interface] = instruction->desc[i].db;
            }

            for (i = 0; i < device->max_num_searches; i++)
            {
                if (db[i])
                {
                    kaps_jr1_print_to_file(device, f, "<td>");
                    kaps_jr1_print_db_name(db[i], f);
                    kaps_jr1_print_to_file(device, f, "</td>\n");
                }
                else
                {
                    kaps_jr1_print_to_file(device, f, "<td>-</td>\n");
                }
            }

            kaps_jr1_print_to_file(device, f, "</tr>\n");
        }
    }

    kaps_jr1_print_to_file(device, f, "</tbody>\n");
    kaps_jr1_print_to_file(device, f, "</table>\n");
    kaps_jr1_print_to_file(device, f, "<br>\n");

    {
        
        kaps_jr1_print_to_file(device, f, "<h3>Device Configuration</h3>\n");
        kaps_jr1_print_to_file(device, f, "<table>\n");
        kaps_jr1_print_to_file(device, f, "<tbody>\n");
        kaps_jr1_print_to_file(device, f, "<tr class=\"broadcom3\"> \n");
        kaps_jr1_print_to_file(device, f, "  <th>Type</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>AB</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>BBs</th>\n");
        kaps_jr1_print_to_file(device, f, "  <th>BB Cascades</th>\n");
        kaps_jr1_print_to_file(device, f, "</tr>\n");

        kaps_jr1_print_to_file(device, f, "<tr>\n");
        kaps_jr1_print_to_file(device, f, "  <td>KAPS_JR1</td>\n");
        kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", device->num_ab);
        kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", device->hw_res->total_lpus);
        kaps_jr1_print_to_file(device, f, "  <td>%d</td>\n", device->hw_res->num_bb_in_one_bb_cascade);
        kaps_jr1_print_to_file(device, f, "</tr>\n");
        kaps_jr1_print_to_file(device, f, "</tbody>\n");
        kaps_jr1_print_to_file(device, f, "</table>\n");
        kaps_jr1_print_to_file(device, f, "<br>\n");
    }

    if (device->hw_res->device_print_advance == 2)
    {

        kaps_jr1_print_to_file(device, f, "<h3>Device Mapping</h3>\n");

        
        kaps_jr1_print_to_file(device, f,
                           "<svg preserveAspectRatio=\"xMinYMin meet\" width=\"26cm\" height=\"20cm\" xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n");
        kaps_jr1_print_to_file(device, f,
                           "<g id=\"Bounding box\"><rect x=\"0.2cm\" y=\".2cm\" width=\"25.6cm\" height=\"19.6cm\" "
                           "fill=\"none\" stroke=\"orange\" stroke-width=\".2cm\"/></g>\n");

        {
            
            kaps_jr1_print_dba(device, f, mem_map);

            
            kaps_jr1_print_bbs(device, mem_map, f);
        }

        kaps_jr1_print_to_file(device, f, "</g>\n");

        kaps_jr1_print_to_file(device, f, "</svg>\n");
    }

    kaps_jr1_print_to_file(device, f, "</BODY>\n");
    kaps_jr1_print_to_file(device, f, "</HTML>\n");
}
