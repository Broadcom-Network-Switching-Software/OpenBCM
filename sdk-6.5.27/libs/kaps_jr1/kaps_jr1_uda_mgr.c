

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kaps_jr1_uda_mgr.h"
#include "kaps_jr1_lpm_algo.h"
#include "kaps_jr1_fib_lsnmc.h"
#include "kaps_jr1_ad_internal.h"



static void
kaps_jr1_handle_error(
    void)
{
    kaps_jr1_sassert(0);
}

static uint32_t
kaps_jr1_uda_mgr_calc_region_size(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id)
{
    int32_t region_size = 0;

    switch (mgr->which_udm_half)
    {
        case UDM_NO_HALF:
            kaps_jr1_sassert(0);
            break;
        case UDM_LOWER_HALF:
            region_size = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row) / 2;
            break;
        case UDM_HIGHER_HALF:
            region_size = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row) / 2;
            break;
        case UDM_BOTH_HALF:
            region_size = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row);
            break;
    }

    return region_size;
}

static uint32_t
kaps_jr1_uda_mgr_calc_start_res_id(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id)
{
    int32_t start_res_id = 0;

    switch (mgr->which_udm_half)
    {
        case UDM_NO_HALF:
            kaps_jr1_sassert(0);
            break;
        case UDM_LOWER_HALF:
            start_res_id = 0;
            break;
        case UDM_HIGHER_HALF:
            start_res_id = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row) / 2;
            break;
        case UDM_BOTH_HALF:
            start_res_id = 0;
            break;
    }

    return start_res_id;
}

static uint32_t
kaps_jr1_uda_mgr_calc_end_res_id(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id)
{
    int32_t end_res_id = 0;

    switch (mgr->which_udm_half)
    {
        case UDM_NO_HALF:
            kaps_jr1_sassert(0);
            break;
        case UDM_LOWER_HALF:
            end_res_id = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row) / 2;
            break;
        case UDM_HIGHER_HALF:
            end_res_id = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row);
            break;
        case UDM_BOTH_HALF:
            end_res_id = (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row);
            break;
    }
    return end_res_id;
}

static uint32_t
kaps_jr1_uda_mgr_calc_res_id(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *uda_chunk)
{
    return uda_chunk->offset;
}



static void
kaps_jr1_uda_mgr_remove_from_free_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;

    if (level > (mgr->region_info[cur_chunk->region_id].num_lpu - 1))
        level = mgr->region_info[cur_chunk->region_id].num_lpu - 1;

    if (level == mgr->max_lpus_in_a_chunk - 1)
    {
        mgr->available_big_chunk_bricks -= cur_chunk->size;
    }

    if (cur_chunk->prev_free_chunk)
    {
        cur_chunk->prev_free_chunk->next_free_chunk = cur_chunk->next_free_chunk;
    }
    else
    {
        mgr->free_list[level] = cur_chunk->next_free_chunk;
    }

    if (cur_chunk->next_free_chunk)
    {
        cur_chunk->next_free_chunk->prev_free_chunk = cur_chunk->prev_free_chunk;
    }
    cur_chunk->next_free_chunk = NULL;
    cur_chunk->prev_free_chunk = NULL;
}

void
kaps_jr1_uda_mgr_add_to_free_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;

    if (level > (mgr->region_info[cur_chunk->region_id].num_lpu - 1))
        level = mgr->region_info[cur_chunk->region_id].num_lpu - 1;

    if (level == mgr->max_lpus_in_a_chunk - 1)
    {
        mgr->available_big_chunk_bricks += cur_chunk->size;
    }

    cur_chunk->prev_free_chunk = NULL;
    cur_chunk->next_free_chunk = mgr->free_list[level];

    mgr->free_list[level] = cur_chunk;

    if (cur_chunk->next_free_chunk)
        cur_chunk->next_free_chunk->prev_free_chunk = cur_chunk;
}

static void
kaps_jr1_uda_mgr_add_to_free_list_tail(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;

    if (level > (mgr->region_info[cur_chunk->region_id].num_lpu - 1))
        level = mgr->region_info[cur_chunk->region_id].num_lpu - 1;

    if (level == mgr->max_lpus_in_a_chunk - 1)
    {

        mgr->available_big_chunk_bricks += cur_chunk->size;
    }

    cur_chunk->next_free_chunk = NULL;

    if (mgr->free_list[level])
    {
        struct uda_mem_chunk *tmp_chunk;

        tmp_chunk = mgr->free_list[level];

        while (tmp_chunk->next_free_chunk)
            tmp_chunk = tmp_chunk->next_free_chunk;

        cur_chunk->prev_free_chunk = tmp_chunk;
        tmp_chunk->next_free_chunk = cur_chunk;
    }
    else
    {
        cur_chunk->prev_free_chunk = NULL;
        mgr->free_list[level] = cur_chunk;
    }
}

static void
kaps_jr1_uda_mgr_remove_from_neighbor_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk)
{
    if (cur_chunk->prev_neighbor)
    {
        cur_chunk->prev_neighbor->next_neighbor = cur_chunk->next_neighbor;
    }
    else
    {
        mgr->neighbor_list = cur_chunk->next_neighbor;
    }

    if (cur_chunk->next_neighbor)
    {
        cur_chunk->next_neighbor->prev_neighbor = cur_chunk->prev_neighbor;
    }
    cur_chunk->next_neighbor = NULL;
    cur_chunk->prev_neighbor = NULL;
}

void
kaps_jr1_uda_mgr_add_to_neighbor_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *prev_chunk,
    struct uda_mem_chunk *cur_chunk)
{
    cur_chunk->prev_neighbor = prev_chunk;

    if (!prev_chunk)
    {
        cur_chunk->next_neighbor = mgr->neighbor_list;
        mgr->neighbor_list = cur_chunk;
    }
    else
    {
        cur_chunk->next_neighbor = prev_chunk->next_neighbor;
        prev_chunk->next_neighbor = cur_chunk;
    }

    if (cur_chunk->next_neighbor)
        cur_chunk->next_neighbor->prev_neighbor = cur_chunk;
}

static void
kaps_jr1_uda_mgr_add_to_neighbor_list_tail(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk)
{
    cur_chunk->next_neighbor = NULL;

    if (mgr->neighbor_list)
    {
        struct uda_mem_chunk *tmp_chunk;

        tmp_chunk = mgr->neighbor_list;
        while (tmp_chunk->next_neighbor)
            tmp_chunk = tmp_chunk->next_neighbor;

        cur_chunk->prev_neighbor = tmp_chunk;
        tmp_chunk->next_neighbor = cur_chunk;
    }
    else
    {
        cur_chunk->prev_neighbor = NULL;
        mgr->neighbor_list = cur_chunk;
    }
}


kaps_jr1_status
kaps_jr1_uda_mgr_get_next_region_op(
    struct kaps_jr1_uda_mgr * mgr,
    uint8_t alloc_udm[][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS],
    int32_t is_xor,
    uint8_t udms[],
    uint8_t * pNumLpu)
{
    int32_t num_udm = 0;
    int32_t lpu_num = 0, row_num = 0;

    for (row_num = 0; row_num < KAPS_JR1_UDM_PER_UDC; row_num++)
    {
        for (lpu_num = 0; lpu_num < KAPS_JR1_ALLOC_UDM_MAX_COLS; lpu_num++)
        {
            if (alloc_udm[mgr->device->core_id][row_num][lpu_num])
            {
                alloc_udm[mgr->device->core_id][row_num][lpu_num] = 0;
                udms[num_udm] = lpu_num * KAPS_JR1_UDM_PER_UDC + row_num;
                num_udm++;
            }
        }

        if (num_udm)    
            break;

        if (is_xor)     
            break;
    }

    *pNumLpu = num_udm;
    return KAPS_JR1_OK;
}

static void
kaps_jr1_uda_mgr_add_to_cb_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk,
    struct uda_mem_chunk **cb_head_pp,
    struct uda_mem_chunk **cb_tail_pp)
{
    struct uda_mem_chunk *cb_head = *cb_head_pp, *cb_tail = *cb_tail_pp;

    cur_chunk->next_neighbor = NULL;

    if (cb_head)
    {
        cb_tail->next_neighbor = cur_chunk;
        *cb_tail_pp = cur_chunk;
    }
    else
    {
        *cb_head_pp = cur_chunk;
        *cb_tail_pp = cur_chunk;
    }
}

static void
kaps_jr1_uda_mgr_remove_from_cb_list(
    struct kaps_jr1_uda_mgr *mgr,
    struct uda_mem_chunk *cur_chunk,
    struct uda_mem_chunk **cb_head_pp,
    struct uda_mem_chunk **cb_tail_pp)
{
    struct uda_mem_chunk *cb_head = *cb_head_pp;

    kaps_jr1_sassert(cur_chunk == cb_head);

    *cb_head_pp = cur_chunk->next_neighbor;

    if (!(*cb_head_pp))
        *cb_tail_pp = NULL;

    cur_chunk->next_neighbor = NULL;
}

struct uda_mem_chunk *
kaps_jr1_uda_mgr_get_region_start(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id)
{

    struct uda_mem_chunk *temp = mgr->neighbor_list;
    struct uda_mem_chunk *start_chunk = NULL;

    while (temp)
    {
        if (temp->region_id == region_id)
        {
            start_chunk = temp;
            break;
        }
        temp = temp->next_neighbor;
    }

    kaps_jr1_sassert(start_chunk);

    return start_chunk;
}
uint32_t kaps_jr1_uda_mgr_region_is_diff_widths(struct kaps_jr1_uda_mgr *mgr, uint32_t region_id)
{
    uint32_t i = 0, udc_num, row_num;
    uint32_t num_joined = 0;
    while (i < mgr->region_info[region_id].num_lpu)
    {
        kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row_from_offset(mgr, region_id, i, &udc_num, &row_num);
        if (mgr->config.joined_udcs[udc_num])
        {
            num_joined++;
        }
        i++;
    }

    
    if (num_joined == 0)
        return 0;

    
    if (num_joined * 2 == mgr->region_info[region_id].num_lpu)
        return 0;

    return 1;
}

kaps_jr1_status
kaps_jr1_uda_mgr_chunk_move(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    struct uda_mem_chunk * cb_head,
    int32_t to_region_id,
    int32_t to_offset,
    int32_t *size)
{
    int32_t old_region_id;

    if (mgr->has_index_based_ad == 0 && mgr->region_info[uda_chunk->region_id].is_diff_bb_widths == 0)
    {
        KAPS_JR1_STRY(mgr->uda_mgr_sbc(uda_chunk, to_region_id, to_offset));

        uda_chunk->offset = to_offset;

        old_region_id = uda_chunk->region_id;
        uda_chunk->region_id = to_region_id;
        kaps_jr1_sassert(mgr->uda_mgr_update_iit);
        KAPS_JR1_STRY(mgr->uda_mgr_update_iit(uda_chunk));
        uda_chunk->region_id = old_region_id;
        *size = uda_chunk->size;
    }
    else
    {
        int32_t errnum;

        old_region_id = uda_chunk->region_id;
        uda_chunk->region_id = to_region_id;

        errnum = kaps_jr1_lsn_mc_chunk_move_per_lpu_gran(uda_chunk, to_region_id, to_offset, size);
        if (errnum != 0)
        {
            kaps_jr1_sassert (0);
        }
        uda_chunk->region_id = old_region_id;
    }
    return KAPS_JR1_OK;
}


struct uda_mem_chunk *
kaps_jr1_uda_mgr_get_region_start_and_num_chunk(
    struct kaps_jr1_uda_mgr *mgr,
    int32_t region_id,
    int32_t * num_of_chunks)
{

    struct uda_mem_chunk *temp = mgr->neighbor_list;
    struct uda_mem_chunk *start_chunk = NULL;
    *num_of_chunks = 0;

    while (temp)
    {
        if (temp->region_id == region_id)
        {
            start_chunk = temp;
            break;
        }
        temp = temp->next_neighbor;
    }

    kaps_jr1_sassert(start_chunk);

    if (start_chunk->prev_neighbor)
        kaps_jr1_sassert(start_chunk->prev_neighbor->region_id != start_chunk->region_id);

    temp = start_chunk;

    while (temp)
    {
        (*num_of_chunks)++;
        if (temp->region_id != region_id)
            break;
        temp = temp->next_neighbor;
    }
    (*num_of_chunks) += (mgr->region_info[region_id].num_lpu * 2);
    return start_chunk;
}

kaps_jr1_status
kaps_jr1_uda_mgr_compact(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t new_region_id)
{
    struct uda_mem_chunk *compaction_start_chunk = kaps_jr1_uda_mgr_get_region_start(mgr, region_id);
    struct uda_mem_chunk *cur_chunk, *prev_chunk;
    int32_t cb_start_offset = -1;
    int32_t cb_size = mgr->region_info[region_id].num_lpu * 2;
    int32_t new_region_filled_offset = 0, hole_size = 0, res_id = 0;
    struct uda_mem_chunk *cb_head, *cb_tail, *next_chunk;
    struct uda_mem_chunk *new_free_chunk;
    int32_t old_region_size, new_region_size, new_region_end_resource_id, cur_new_chunk_size;

    if (!mgr->enable_compaction)
        return KAPS_JR1_OUT_OF_UDA;

    new_region_filled_offset = kaps_jr1_uda_mgr_calc_start_res_id(mgr, new_region_id);
    if (mgr->enable_compaction)
        new_region_filled_offset += mgr->region_info[new_region_id].num_lpu * 2;

    res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, compaction_start_chunk);
    res_id -= (mgr->region_info[region_id].num_lpu * 2);
    cb_start_offset = res_id;

    cb_head = NULL;
    cb_tail = NULL;

    prev_chunk = compaction_start_chunk->prev_neighbor;
    cur_chunk = compaction_start_chunk;

    while (cur_chunk)
    {

        next_chunk = cur_chunk->next_neighbor;

        if (cur_chunk->region_id != region_id)
            break;


        if (cur_chunk->type == FREE_CHUNK)
        {

            hole_size += cur_chunk->size;

            kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_jr1_uda_mgr_remove_from_free_list(mgr, cur_chunk);

            POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, cur_chunk);

        }
        else
        {
            uint32_t old_size = cur_chunk->size;

            if (hole_size < cur_chunk->size)
            {
                KAPS_JR1_STRY(kaps_jr1_uda_mgr_chunk_move(mgr, cur_chunk, cb_head, region_id, cb_start_offset, &cur_new_chunk_size));

                cur_chunk->size = cur_new_chunk_size;

                cb_start_offset += cur_chunk->size;
                hole_size += cur_chunk->size;
                cb_size -= cur_chunk->size;
                kaps_jr1_sassert(cb_size >= 0);

                kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, cur_chunk);
                kaps_jr1_uda_mgr_add_to_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);

            }
            else
            {
                KAPS_JR1_STRY(kaps_jr1_uda_mgr_chunk_move(mgr, cur_chunk, cb_head, new_region_id, new_region_filled_offset, &cur_new_chunk_size));
                cur_chunk->size = cur_new_chunk_size;

                new_region_filled_offset += cur_chunk->size;

                prev_chunk = cur_chunk;
            }

            if (cur_chunk->size < old_size)
                mgr->num_allocated_lpu_bricks -= (old_size - cur_chunk->size);
            else
                mgr->num_allocated_lpu_bricks += (cur_chunk->size - old_size);
        }
        cur_chunk = next_chunk;
    }

    cur_chunk = cb_head;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_chunk_move(mgr, cur_chunk, cb_head, new_region_id, new_region_filled_offset, &cur_new_chunk_size));
        cur_chunk->size = cur_new_chunk_size;
        new_region_filled_offset += cur_chunk->size;

        kaps_jr1_uda_mgr_remove_from_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);
        kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

        prev_chunk = cur_chunk;
        cur_chunk = next_chunk;
    }

    old_region_size = kaps_jr1_uda_mgr_calc_region_size(mgr, region_id);
    new_region_size = kaps_jr1_uda_mgr_calc_region_size(mgr, new_region_id);
    new_region_end_resource_id = kaps_jr1_uda_mgr_calc_end_res_id(mgr, new_region_id);

    if (new_region_id != region_id)
    {
        if (mgr->enable_compaction)
        {
            
            mgr->total_lpu_bricks += mgr->region_info[region_id].num_lpu * 2;
            mgr->total_lpu_bricks -= old_region_size;
            mgr->total_lpu_bricks += new_region_size;
            mgr->total_lpu_bricks -= mgr->region_info[new_region_id].num_lpu * 2;
        }
        else
        {
            mgr->total_lpu_bricks += new_region_size - old_region_size;
        }
    }

    if (region_id != new_region_id)
    {
        kaps_jr1_memcpy(&mgr->region_info[region_id], &mgr->region_info[new_region_id], sizeof(mgr->region_info[0]));
    }

    if (new_region_end_resource_id > new_region_filled_offset)
    {
        
        POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, new_free_chunk);
        if (!new_free_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;
        kaps_jr1_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

        new_free_chunk->offset = new_region_filled_offset;
        new_free_chunk->region_id = region_id;
        
        new_free_chunk->size = new_region_end_resource_id - new_region_filled_offset;

        kaps_jr1_uda_mgr_add_to_free_list(mgr, new_free_chunk);
        kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);
    }

    KAPS_JR1_STRY(kaps_jr1_uda_mgr_verify(mgr));

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_compact_all_regions(
    struct kaps_jr1_uda_mgr * mgr)
{
    int region_no, num_region, new_region_no;
    kaps_jr1_status status = KAPS_JR1_OUT_OF_UDA;

    if (!mgr->enable_compaction)
        return KAPS_JR1_OUT_OF_UDA;

    num_region = mgr->no_of_regions;
    new_region_no = mgr->no_of_regions;

    for (region_no = 0; region_no < num_region; region_no++)
    {
        int32_t region_size, max_lpus_on_region;

        max_lpus_on_region = mgr->region_info[region_no].num_lpu;
        if (max_lpus_on_region > mgr->max_lpus_in_a_chunk)
            max_lpus_on_region = mgr->max_lpus_in_a_chunk;

        if (mgr->free_list[max_lpus_on_region - 1] != NULL)
        {
            
            continue;
        }

        region_size = kaps_jr1_uda_mgr_calc_region_size(mgr, region_no);
        if (mgr->num_allocated_lpu_bricks_per_region[region_no] <= (region_size * 97 / 100))
        {
            kaps_jr1_memcpy(&mgr->region_info[new_region_no], &mgr->region_info[region_no], sizeof(mgr->region_info[0]));
            KAPS_JR1_STRY(kaps_jr1_uda_mgr_compact(mgr, region_no, region_no));
            status = KAPS_JR1_OK;
        }
    }

    return status;
}

kaps_jr1_status
kaps_jr1_uda_mgr_wb_restore_regions(
    struct kaps_jr1_uda_mgr * mgr,
    struct kaps_jr1_allocator * alloc,
    int32_t no_of_regions,
    struct region region_info[])
{
    int32_t region_id;
    int32_t row_num, lpu_num;

    mgr->total_lpu_bricks = 0;
    for (region_id = 0; region_id < no_of_regions; region_id++)
    {
        kaps_jr1_sassert(region_id < MAX_UDA_REGIONS);
        mgr->chunk_map[region_id] = alloc->xcalloc(alloc->cookie, mgr->region_info[region_id].num_row,
                                                   sizeof(struct chunk_info *));

        if (mgr->chunk_map[region_id] == NULL)
            return KAPS_JR1_OUT_OF_MEMORY;

        if (mgr->which_udm_half == UDM_BOTH_HALF)
        {
            mgr->total_lpu_bricks += mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row;
        }
        else
        {
            mgr->total_lpu_bricks += mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row / 2;
        }

        if (mgr->enable_compaction)
        {
            mgr->total_lpu_bricks -= mgr->region_info[region_id].num_lpu * 2;
        }

        for (row_num = 0; row_num < mgr->region_info[region_id].num_row; row_num++)
        {

            mgr->chunk_map[region_id][row_num] = alloc->xcalloc(alloc->cookie,
                                                                mgr->region_info[region_id].num_lpu,
                                                                sizeof(struct chunk_info));

            if (mgr->chunk_map[region_id][row_num] == NULL)
                return KAPS_JR1_OUT_OF_MEMORY;

            for (lpu_num = 0; lpu_num < mgr->region_info[region_id].num_lpu; lpu_num++)
            {
                mgr->chunk_map[region_id][row_num][lpu_num].lsn_id = 0xffffffff;
                mgr->chunk_map[region_id][row_num][lpu_num].chunk_p = NULL;
            }
        }
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_add_new_region(
    struct kaps_jr1_uda_mgr * mgr,
    uint32_t num_lpu,
    uint32_t num_row)
{
    struct uda_mem_chunk *free_chunk = NULL;
    uint32_t region_id;

    POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, free_chunk);
    if (!free_chunk)
        return KAPS_JR1_OUT_OF_MEMORY;

    region_id = mgr->no_of_regions;
    kaps_jr1_sassert(region_id < MAX_UDA_REGIONS);
    kaps_jr1_memset(free_chunk, 0, sizeof(*free_chunk));

    mgr->region_info[region_id].num_lpu = num_lpu;
    mgr->region_info[region_id].num_row = num_row * mgr->max_rows_in_udm;

    if (mgr->which_udm_half == UDM_BOTH_HALF)
    {
        free_chunk->size = num_lpu * num_row * mgr->max_rows_in_udm;
        
    }
    else
    {
        free_chunk->size = num_lpu * mgr->max_rows_in_udm / 2;
        mgr->region_info[region_id].num_row = mgr->max_rows_in_udm;

        if (mgr->which_udm_half == UDM_LOWER_HALF)
        {
            
        }
        else if (mgr->which_udm_half == UDM_HIGHER_HALF)
        {
            free_chunk->offset = num_lpu * mgr->max_rows_in_udm / 2;
        }
    }

    free_chunk->region_id = region_id;
    

    if (mgr->enable_compaction)
    {
        free_chunk->offset += (mgr->region_info[region_id].num_lpu * 2);
        free_chunk->size -= mgr->region_info[region_id].num_lpu * 2;
    }

    mgr->total_lpu_bricks += free_chunk->size;

    kaps_jr1_uda_mgr_add_to_free_list_tail(mgr, free_chunk);
    kaps_jr1_uda_mgr_add_to_neighbor_list_tail(mgr, free_chunk);

    mgr->region_info[region_id].region_powered_up = 0;
    mgr->no_of_regions++;

    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_uda_mgr_compute_num_rows_in_udm(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db,
    uint8_t alloc_udm[][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS])
{
    uint32_t rows_in_udm;

    rows_in_udm = device->hw_res->num_rows_in_bb;

    if (kaps_jr1_db_is_reduced_algo_levels(db))
    {
        rows_in_udm = device->hw_res->num_rows_in_small_bb;
    }


    return rows_in_udm;
}

kaps_jr1_status
kaps_jr1_uda_mgr_init(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db,
    int8_t dt_index,
    struct kaps_jr1_allocator * alloc,
    uint8_t alloc_udm[][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS],
    struct kaps_jr1_uda_mgr ** mgr_pp,
    uint8_t max_lpus_in_a_chunk,
    uint8_t is_warmboot,
    uint8_t which_udm_half,
    UdaMgr__UdaSBC f_uda_mgr_sbc,
    UdaMgr__UpdateIIT f_uda_mgr_update_iit,
    UdaMgr__update_lsn_size f_uda_mgr_update_lsn_size,
    int8_t unit_test_mode)
{
    struct kaps_jr1_uda_mgr *mgr = NULL;
    struct kaps_jr1_ad_db *ad_db = NULL;
    uint8_t r_num_lpu;
    uint8_t tmpUdaBmp[KAPS_JR1_MAX_NUM_CORES][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS];

    if (mgr_pp)
        *mgr_pp = NULL;

    mgr = alloc->xcalloc(alloc->cookie, 1, sizeof(struct kaps_jr1_uda_mgr));

    if (!mgr)
        return KAPS_JR1_OUT_OF_MEMORY;

    if (mgr_pp)
        *mgr_pp = mgr;

    mgr->alloc = alloc;

    mgr->enable_compaction = device->is_bb_compaction_enabled;

    mgr->max_num_uda_chunk_levels = device->hw_res->max_num_uda_chunk_levels;

    POOL_INIT(uda_mem_chunk, &mgr->uda_chunk_pool, alloc);

    mgr->db = db;
    mgr->device = device;
    if (mgr->device->main_dev)
    {
        mgr->device = mgr->device->main_dev;
    }

    ad_db = (struct kaps_jr1_ad_db *)db->common_info->ad_info.ad;
    while (ad_db)
    {
        if (ad_db->db_info.hw_res.ad_res->ad_type == KAPS_JR1_AD_TYPE_INDEX)
            mgr->has_index_based_ad = 1;

        ad_db = ad_db->next;
    }

    mgr->max_rows_in_udm = kaps_jr1_uda_mgr_compute_num_rows_in_udm(device, db, db->hw_res.db_res->lsn_info[0].alloc_udm);

    if (db->type == KAPS_JR1_DB_LPM && (max_lpus_in_a_chunk < MAX_LSN_WIDTH_FOR_GROW))
    {
        max_lpus_in_a_chunk = MAX_LSN_WIDTH_FOR_GROW;
    }
    mgr->which_udm_half = which_udm_half;
    mgr->unit_test_mode = unit_test_mode;
    mgr->dt_index = dt_index;
    kaps_jr1_memcpy(mgr->alloc_udm, alloc_udm, sizeof(mgr->alloc_udm));
    kaps_jr1_memcpy(tmpUdaBmp, alloc_udm, sizeof(mgr->alloc_udm));
    mgr->max_lpus_in_a_chunk = max_lpus_in_a_chunk;
    mgr->uda_mgr_sbc = f_uda_mgr_sbc;
    mgr->uda_mgr_update_iit = f_uda_mgr_update_iit;
    mgr->uda_mgr_update_lsn_size = f_uda_mgr_update_lsn_size;

    if (db->common_info->enable_dynamic_allocation)
    {
        mgr->enable_dynamic_allocation = 1;
    }

    if (!is_warmboot)
    {
        mgr->no_of_regions = 0;
        while (1)
        {

            r_num_lpu = 0;

            kaps_jr1_uda_mgr_get_next_region_op(mgr, tmpUdaBmp, db->has_dup_db, mgr->region_info[mgr->no_of_regions].udms,
                                            &r_num_lpu);

            if (r_num_lpu == 0)
            {
                break;
            }

            KAPS_JR1_STRY(kaps_jr1_uda_mgr_add_new_region(mgr, r_num_lpu, 1));

            if (mgr->db->has_dup_db)
            {
                int32_t i;

                for (i = 0; i < r_num_lpu; i++)
                {
                    mgr->region_info[mgr->no_of_regions].udms[i] = mgr->region_info[mgr->no_of_regions - 1].udms[i] + 1;
                    mgr->region_info[mgr->no_of_regions + 1].udms[i] =
                        mgr->region_info[mgr->no_of_regions - 1].udms[i] + 2;
                }
                KAPS_JR1_STRY(kaps_jr1_uda_mgr_add_new_region(mgr, r_num_lpu, 1));
                KAPS_JR1_STRY(kaps_jr1_uda_mgr_add_new_region(mgr, r_num_lpu, 1));
            }

        }
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_uda_mgr_expand_all_allocated_udms(
    struct kaps_jr1_uda_mgr * mgr)
{
    struct uda_mem_chunk *chunk;
    struct uda_mem_chunk *t_chunk;
    struct uda_mem_chunk *t_prev_chunk;
    struct uda_mem_chunk *free_chunk = NULL;
    int32_t num_rows_to_expand;
    int32_t region_id;

    if (!mgr)
        return KAPS_JR1_INVALID_ARGUMENT;

    kaps_jr1_sassert((mgr->which_udm_half == UDM_LOWER_HALF) || (mgr->which_udm_half == UDM_HIGHER_HALF));

    num_rows_to_expand = mgr->max_rows_in_udm / 2;

    for (region_id = 0; region_id < mgr->no_of_regions; region_id++)
    {
        t_chunk = mgr->neighbor_list;
        t_prev_chunk = NULL;
        POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, chunk);
        if (!chunk)
            return KAPS_JR1_OUT_OF_MEMORY;
        kaps_jr1_memset(chunk, 0, sizeof(*chunk));

        
        chunk->region_id = region_id;
        chunk->size = mgr->region_info[region_id].num_lpu * num_rows_to_expand;

        if (mgr->which_udm_half == UDM_HIGHER_HALF)
        {

            

            while (t_chunk != NULL)
            {
                if (t_chunk->region_id == region_id)
                    break;
                t_prev_chunk = t_chunk;
                t_chunk = t_chunk->next_neighbor;
            }

            if (mgr->enable_compaction)
            {
                POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, free_chunk);
                if (!free_chunk)
                    return KAPS_JR1_OUT_OF_MEMORY;
                kaps_jr1_memset(free_chunk, 0, sizeof(*free_chunk));

                chunk->offset += (mgr->region_info[region_id].num_lpu * 2);
                chunk->size -= (mgr->region_info[region_id].num_lpu * 2);

                
                free_chunk->region_id = region_id;
                free_chunk->size = mgr->region_info[region_id].num_lpu * 2;
                free_chunk->offset = num_rows_to_expand * mgr->region_info[region_id].num_lpu;

            }
            kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, t_prev_chunk, chunk);

            if (mgr->enable_compaction)
            {
                kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, chunk, free_chunk);
                kaps_jr1_uda_mgr_add_to_free_list(mgr, free_chunk);
                mgr->total_lpu_bricks += free_chunk->size;
            }

        }
        else
        {

            chunk->offset = num_rows_to_expand * mgr->region_info[region_id].num_lpu;

            while (t_chunk != NULL)
            {
                if (t_chunk->region_id == region_id)
                    t_prev_chunk = t_chunk;
                t_chunk = t_chunk->next_neighbor;
            }
            kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, t_prev_chunk, chunk);
        }

        mgr->num_allocated_lpu_bricks += chunk->size;   
        mgr->num_allocated_lpu_bricks_per_region[chunk->region_id] += chunk->size;
        mgr->total_lpu_bricks += chunk->size;
        kaps_jr1_uda_mgr_free(mgr, chunk);
    }
    mgr->which_udm_half = UDM_BOTH_HALF;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_wb_alloc(
    struct kaps_jr1_uda_mgr * mgr,
    void *handle,
    uint32_t rqt_num_lpus,
    uint32_t region_id,
    uint32_t offset,
    uint32_t lsn_id,
    struct uda_mem_chunk ** alloc_chunk_pp)
{
    struct uda_mem_chunk *chosen_chunk = NULL;
    uint32_t res_id = 0, end_res_id = 0;

    *alloc_chunk_pp = NULL;

    kaps_jr1_sassert(region_id != -1);

    if (rqt_num_lpus == 0 || rqt_num_lpus > mgr->region_info[region_id].num_lpu)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, chosen_chunk);
    if (!chosen_chunk)
        return KAPS_JR1_OUT_OF_MEMORY;
    kaps_jr1_memset(chosen_chunk, 0, sizeof(*chosen_chunk));

    chosen_chunk->region_id = region_id;
    chosen_chunk->type = ALLOCATED_CHUNK;
    




    chosen_chunk->size = rqt_num_lpus;
    chosen_chunk->offset = offset;
    chosen_chunk->handle = handle;

    mgr->num_allocated_lpu_bricks += rqt_num_lpus;
    mgr->num_allocated_lpu_bricks_per_region[region_id] += rqt_num_lpus;

    res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, chosen_chunk);
    end_res_id = res_id + rqt_num_lpus;

    for (; res_id < end_res_id; res_id++)
    {

        if (mgr->chunk_map[region_id][res_id / mgr->region_info[region_id].num_lpu][res_id %
                                                                                    mgr->region_info[region_id].
                                                                                    num_lpu].chunk_p)
        {
            kaps_jr1_sassert(0);
            return KAPS_JR1_INVALID_ARGUMENT;
        }
        else
            mgr->chunk_map[region_id][res_id / mgr->region_info[region_id].num_lpu][res_id %
                                                                                    mgr->
                                                                                    region_info[region_id].num_lpu].
                chunk_p = chosen_chunk;
        mgr->chunk_map[region_id][res_id / mgr->region_info[region_id].num_lpu][res_id %
                                                                                mgr->region_info[region_id].
                                                                                num_lpu].lsn_id = lsn_id;
    }

    *alloc_chunk_pp = chosen_chunk;

    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_uda_mgr_count_free(
    struct chunk_info ** chunk_map,
    uint32_t res_id,
    uint32_t end_res_id,
    uint32_t num_lpus_per_row)
{
    uint32_t size = 0;

    while ((res_id < end_res_id) && chunk_map[res_id / num_lpus_per_row][res_id % num_lpus_per_row].chunk_p == NULL)
    {
        size++;
        res_id++;
    }

    return size;
}

kaps_jr1_status
kaps_jr1_uda_mgr_wb_finalize(
    struct kaps_jr1_allocator * alloc,
    struct kaps_jr1_uda_mgr * mgr)
{
    uint32_t res_id, end_res_id;
    uint32_t size;
    uint32_t row_num;
    int32_t region_id;
    struct uda_mem_chunk *last = NULL, *current, *tmp, *next;

    if (mgr->is_finalized)
        return KAPS_JR1_OK;

    if (!mgr->chunk_map[0])
    {
        return KAPS_JR1_OK;
    }

    tmp = mgr->neighbor_list;
    while (tmp)
    {
        next = tmp->next_neighbor;
        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, tmp);
        tmp = next;
    }

    mgr->neighbor_list = 0;
    kaps_jr1_memset(mgr->free_list, 0, sizeof(struct uda_mem_chunk *) * mgr->max_num_uda_chunk_levels);

    for (region_id = 0; region_id < mgr->no_of_regions; region_id++)
    {
        res_id = kaps_jr1_uda_mgr_calc_start_res_id(mgr, region_id);

        if (mgr->enable_compaction)
            res_id += mgr->region_info[region_id].num_lpu * 2;

        end_res_id = kaps_jr1_uda_mgr_calc_end_res_id(mgr, region_id);

        for (; res_id < end_res_id; res_id++)
        {
            current =
                mgr->chunk_map[region_id][res_id / mgr->region_info[region_id].num_lpu][res_id %
                                                                                        mgr->
                                                                                        region_info[region_id].num_lpu].
                chunk_p;
            if (current != NULL)
            {
                kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, last, current);
                last = current;
                res_id += current->size - 1;
            }
            else
            {
                size =
                    kaps_jr1_uda_mgr_count_free(mgr->chunk_map[region_id], res_id, end_res_id,
                                            mgr->region_info[region_id].num_lpu);
                POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, current);
                if (!current)
                    return KAPS_JR1_OUT_OF_MEMORY;
                kaps_jr1_memset(current, 0, sizeof(*current));
                current->offset = res_id;
                
                current->size = size;
                current->region_id = region_id;
                kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, last, current);
                kaps_jr1_uda_mgr_add_to_free_list_tail(mgr, current);
                last = current;
                res_id += current->size - 1;
            }

        }
        for (row_num = 0; row_num < mgr->region_info[region_id].num_row; row_num++)
        {
            alloc->xfree(alloc->cookie, mgr->chunk_map[region_id][row_num]);
        }
        alloc->xfree(alloc->cookie, mgr->chunk_map[region_id]);
        mgr->chunk_map[region_id] = 0;
    }
    mgr->is_finalized = 1;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_power_up_the_region(
    struct kaps_jr1_device * device,
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id)
{
    mgr->region_info[region_id].region_powered_up = 1;

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_uda_mgr_alloc_internal(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t rqt_num_lpus,
    struct kaps_jr1_device * device,
    void *handle,
    struct uda_mem_chunk ** alloc_chunk_pp,
    struct kaps_jr1_db * db)
{
    struct uda_mem_chunk *chosen_chunk = NULL, *remainder_chunk = NULL;
    int32_t level;
    uint32_t res_id = 0;
    int32_t num_lpus_to_allocate;
    int32_t region_id, next_level = mgr->max_num_uda_chunk_levels - 1;

    *alloc_chunk_pp = NULL;

    if (rqt_num_lpus == 0 || rqt_num_lpus > mgr->max_lpus_in_a_chunk)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    num_lpus_to_allocate = rqt_num_lpus;

    
    if (mgr->free_list[num_lpus_to_allocate - 1])
    {
        chosen_chunk = mgr->free_list[num_lpus_to_allocate - 1];
    }
    else
    {
        
        for (region_id = 0; region_id < mgr->no_of_regions; region_id++)
        {
            if ((mgr->region_info[region_id].num_lpu >= num_lpus_to_allocate) &&
                (mgr->region_info[region_id].num_lpu <= next_level))
            {
                next_level = mgr->region_info[region_id].num_lpu - 1;
            }
        }

        if (mgr->free_list[next_level])
        {
            chosen_chunk = mgr->free_list[next_level];
        }
        else
        {
            for (level = mgr->max_num_uda_chunk_levels - 1; level >= num_lpus_to_allocate - 1; level--)
            {
                if (mgr->free_list[level])
                {
                    chosen_chunk = mgr->free_list[level];
                    break;
                }
            }
        }
    }

    
    if ((!chosen_chunk) && (num_lpus_to_allocate > rqt_num_lpus))
    {
        num_lpus_to_allocate = rqt_num_lpus;
        
        if (mgr->free_list[num_lpus_to_allocate - 1])
        {
            chosen_chunk = mgr->free_list[num_lpus_to_allocate - 1];
        }
        else
        {
            
            for (region_id = 0; region_id < mgr->no_of_regions; region_id++)
            {
                if ((mgr->region_info[region_id].num_lpu >= num_lpus_to_allocate) &&
                    (mgr->region_info[region_id].num_lpu <= next_level))
                {
                    next_level = mgr->region_info[region_id].num_lpu - 1;
                }
            }

            if (mgr->free_list[next_level])
            {
                chosen_chunk = mgr->free_list[next_level];
            }
            else
            {
                for (level = mgr->max_num_uda_chunk_levels - 1; level >= num_lpus_to_allocate - 1; level--)
                {
                    if (mgr->free_list[level])
                    {
                        chosen_chunk = mgr->free_list[level];
                        break;
                    }
                }
            }
        }
    }

    if (!chosen_chunk)
    {
        return KAPS_JR1_OUT_OF_UDA;
    }

    if (chosen_chunk->size > num_lpus_to_allocate)
    {
        int32_t udc_no = kaps_jr1_uda_mgr_compute_brick_udc(mgr, chosen_chunk, num_lpus_to_allocate);
        if (mgr->config.joined_udcs[udc_no])
        {
            num_lpus_to_allocate++;
            rqt_num_lpus = num_lpus_to_allocate;
        }
    }

    if (chosen_chunk->size > num_lpus_to_allocate)
    {
        POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, remainder_chunk);
        if (!remainder_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;
        kaps_jr1_memset(remainder_chunk, 0, sizeof(*remainder_chunk));
    }

    
    kaps_jr1_uda_mgr_remove_from_free_list(mgr, chosen_chunk);

    
    if (remainder_chunk)
    {
        remainder_chunk->size = chosen_chunk->size - num_lpus_to_allocate;

        res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, chosen_chunk);
        res_id += num_lpus_to_allocate;

        remainder_chunk->region_id = chosen_chunk->region_id;

        remainder_chunk->offset = res_id;

        remainder_chunk->type = FREE_CHUNK;

        chosen_chunk->type = ALLOCATED_CHUNK;
        chosen_chunk->size = rqt_num_lpus;

        kaps_jr1_uda_mgr_add_to_free_list(mgr, remainder_chunk);
        kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, chosen_chunk, remainder_chunk);

    }
    else
    {
        chosen_chunk->type = ALLOCATED_CHUNK;
        chosen_chunk->size = rqt_num_lpus;
    }

    mgr->num_allocated_lpu_bricks += rqt_num_lpus;
    mgr->num_allocated_lpu_bricks_per_region[chosen_chunk->region_id] += rqt_num_lpus;
    chosen_chunk->db = db;
    chosen_chunk->handle = handle;

    *alloc_chunk_pp = chosen_chunk;
    if (!mgr->region_info[(*alloc_chunk_pp)->region_id].region_powered_up)
    {
        KAPS_JR1_STRY(kaps_jr1_power_up_the_region(device, mgr, (*alloc_chunk_pp)->region_id));
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_dynamic_uda_alloc_n_lpu(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t num_udms_to_fit)
{
    kaps_jr1_status status = KAPS_JR1_OK;
    uint8_t alloc_udm[2][KAPS_JR1_UDM_PER_UDC][KAPS_JR1_ALLOC_UDM_MAX_COLS];
    int32_t num_lpu = 0;
    int i;

    if (mgr->enable_dynamic_allocation == 0)
    {
        return KAPS_JR1_OUT_OF_UDA;
    }

    kaps_jr1_memcpy(alloc_udm, mgr->db->hw_res.db_res->lsn_info[mgr->dt_index].alloc_udm, sizeof(alloc_udm));

    {
        if (!mgr->no_of_regions)
        {
            status = kaps_jr1_resource_dynamic_uda_alloc(mgr->device, mgr->device->map, mgr->db, mgr->dt_index,
                                                     &num_lpu, mgr->region_info[mgr->no_of_regions].udms);
        }
        else
        {
            status = KAPS_JR1_OUT_OF_UDA;
        }
    }

    if (status != KAPS_JR1_OK)
        return status;

    KAPS_JR1_STRY(kaps_jr1_uda_mgr_add_new_region(mgr, num_lpu, 1));
    if (mgr->db->has_dup_db)
    {
        for (i = 0; i < num_lpu; i++)
        {
            mgr->region_info[mgr->no_of_regions].udms[i] = mgr->region_info[mgr->no_of_regions - 1].udms[i] + 1;
            mgr->region_info[mgr->no_of_regions + 1].udms[i] = mgr->region_info[mgr->no_of_regions - 1].udms[i] + 2;
        }
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_add_new_region(mgr, num_lpu, 1));
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_add_new_region(mgr, num_lpu, 1));
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_get_more_uda(
    struct kaps_jr1_uda_mgr * mgr)
{
    kaps_jr1_status status = KAPS_JR1_OUT_OF_UDA;
    int32_t num_udms_to_fit;

    kaps_jr1_sassert(mgr->db->hw_res.db_res->lsn_info[mgr->dt_index].max_lsn_size);

    num_udms_to_fit = mgr->db->hw_res.db_res->lsn_info[mgr->dt_index].max_lsn_size;
    if (num_udms_to_fit < mgr->db->hw_res.db_res->lsn_info[mgr->dt_index].combined_lsn_size)
        num_udms_to_fit = mgr->db->hw_res.db_res->lsn_info[mgr->dt_index].combined_lsn_size;

    
    status = kaps_jr1_uda_mgr_dynamic_uda_alloc_n_lpu(mgr, num_udms_to_fit);
    if (status != KAPS_JR1_OUT_OF_UDA)
        return status;

    status = kaps_jr1_resource_expand_uda_mgr_regions(mgr);

    return status;
}

kaps_jr1_status
kaps_jr1_uda_mgr_alloc(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t rqt_num_lpus,
    struct kaps_jr1_device * device,
    void *handle,
    struct uda_mem_chunk ** alloc_chunk_pp,
    struct kaps_jr1_db * db)
{
    kaps_jr1_status status;

    status = kaps_jr1_uda_mgr_alloc_internal(mgr, rqt_num_lpus, device, handle, alloc_chunk_pp, db);
    if (status != KAPS_JR1_OUT_OF_UDA)
        return status;

    if (db->type == KAPS_JR1_DB_LPM)
        return KAPS_JR1_OUT_OF_UDA;

    if (mgr->enable_compaction)
    {
        status = kaps_jr1_uda_mgr_compact_all_regions(mgr);
        if (status == KAPS_JR1_OK)
            status = kaps_jr1_uda_mgr_alloc_internal(mgr, rqt_num_lpus, device, handle, alloc_chunk_pp, db);
    }

    if (status != KAPS_JR1_OUT_OF_UDA)
        return status;

    if (mgr->enable_dynamic_allocation == 0)
    {
        return KAPS_JR1_OUT_OF_UDA;
    }

    kaps_jr1_uda_mgr_get_more_uda(mgr);

    status = kaps_jr1_uda_mgr_alloc_internal(mgr, rqt_num_lpus, device, handle, alloc_chunk_pp, db);
    return status;
}


kaps_jr1_status
kaps_jr1_uda_mgr_check_grow_up(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * cur_uda_chunk,
    uint32_t * grow_up_size_p)
{
    struct uda_mem_chunk *prev_neighbor = cur_uda_chunk->prev_neighbor;
    uint32_t cur_res_id, prev_res_id;

    *grow_up_size_p = 0;

    if (prev_neighbor && prev_neighbor->type == FREE_CHUNK && (prev_neighbor->region_id == cur_uda_chunk->region_id))
    {
        cur_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, cur_uda_chunk);
        prev_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, prev_neighbor);

        
        if (prev_res_id + prev_neighbor->size == cur_res_id)
        {
            *grow_up_size_p = prev_neighbor->size;
        }
    }

    if (*grow_up_size_p + cur_uda_chunk->size > mgr->region_info[cur_uda_chunk->region_id].num_lpu)
        *grow_up_size_p = mgr->region_info[cur_uda_chunk->region_id].num_lpu - cur_uda_chunk->size;
    if (*grow_up_size_p + cur_uda_chunk->size > mgr->max_lpus_in_a_chunk)
        *grow_up_size_p = mgr->max_lpus_in_a_chunk - cur_uda_chunk->size;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_grow_up(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * cur_uda_chunk,
    uint32_t grow_up_size)
{
    struct uda_mem_chunk *prev_neighbor = cur_uda_chunk->prev_neighbor;
    uint32_t cur_res_id;

    if (!prev_neighbor || prev_neighbor->type != FREE_CHUNK || prev_neighbor->size < grow_up_size)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INTERNAL_ERROR;
    }

    cur_uda_chunk->size += grow_up_size;

    cur_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, cur_uda_chunk);
    cur_res_id -= grow_up_size;
    cur_uda_chunk->offset = cur_res_id;

    kaps_jr1_uda_mgr_remove_from_free_list(mgr, prev_neighbor);
    prev_neighbor->size -= grow_up_size;

    if (prev_neighbor->size > 0)
    {
        kaps_jr1_uda_mgr_add_to_free_list(mgr, prev_neighbor);
    }
    else
    {
        kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, prev_neighbor);
        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, prev_neighbor);
    }

    mgr->num_allocated_lpu_bricks += grow_up_size;
    mgr->num_allocated_lpu_bricks_per_region[cur_uda_chunk->region_id] += grow_up_size;

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_uda_mgr_check_grow_down(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * cur_uda_chunk,
    uint32_t * grow_down_size_p)
{
    struct uda_mem_chunk *next_neighbor = cur_uda_chunk->next_neighbor;
    uint32_t cur_res_id, next_res_id;

    *grow_down_size_p = 0;

    if (next_neighbor && next_neighbor->type == FREE_CHUNK && (next_neighbor->region_id == cur_uda_chunk->region_id))
    {
        cur_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, cur_uda_chunk);
        next_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, next_neighbor);

        
        if (cur_res_id + cur_uda_chunk->size == next_res_id)
        {
            *grow_down_size_p += next_neighbor->size;
        }
    }

    if (*grow_down_size_p + cur_uda_chunk->size > mgr->region_info[cur_uda_chunk->region_id].num_lpu)
        *grow_down_size_p = mgr->region_info[cur_uda_chunk->region_id].num_lpu - cur_uda_chunk->size;
    if (*grow_down_size_p + cur_uda_chunk->size > mgr->max_lpus_in_a_chunk)
        *grow_down_size_p = mgr->max_lpus_in_a_chunk - cur_uda_chunk->size;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_grow_down(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * cur_uda_chunk,
    uint32_t grow_down_size)
{
    struct uda_mem_chunk *next_neighbor = cur_uda_chunk->next_neighbor;
    uint32_t next_res_id;
    int32_t remaining_grow_down_size;

    remaining_grow_down_size = grow_down_size;


    if (!next_neighbor || next_neighbor->type != FREE_CHUNK
        || next_neighbor->size < remaining_grow_down_size)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INTERNAL_ERROR;
    }


    if (remaining_grow_down_size)
    {
        cur_uda_chunk->size += remaining_grow_down_size;

        next_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, next_neighbor);
        next_res_id += remaining_grow_down_size;

        kaps_jr1_uda_mgr_remove_from_free_list(mgr, next_neighbor);
        next_neighbor->size -= remaining_grow_down_size;

        if (next_neighbor->size > 0)
        {
            next_neighbor->offset = next_res_id;
            kaps_jr1_uda_mgr_add_to_free_list(mgr, next_neighbor);
        }
        else
        {
            kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, next_neighbor);
            POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, next_neighbor);
        }
    }

    mgr->num_allocated_lpu_bricks += grow_down_size;
    mgr->num_allocated_lpu_bricks_per_region[cur_uda_chunk->region_id] += grow_down_size;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_grow(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * cur_uda_chunk,
    uint32_t num_bricks_to_grow,
    uint32_t * num_bricks_grown_p,
    int8_t * flag_grown_up_p)
{
    uint32_t grow_up_size = 0;
    uint32_t grow_down_size;

    kaps_jr1_sassert(num_bricks_to_grow);

    *num_bricks_grown_p = 0;
    *flag_grown_up_p = 0;

    
    kaps_jr1_uda_mgr_check_grow_up(mgr, cur_uda_chunk, &grow_up_size);
    if (grow_up_size >= num_bricks_to_grow)
    {
        *flag_grown_up_p = 1;
        *num_bricks_grown_p = num_bricks_to_grow;
        return kaps_jr1_uda_mgr_grow_up(mgr, cur_uda_chunk, num_bricks_to_grow);
    }

    
    kaps_jr1_uda_mgr_check_grow_down(mgr, cur_uda_chunk, &grow_down_size);
    if (grow_down_size >= num_bricks_to_grow)
    {
        *flag_grown_up_p = 0;
        *num_bricks_grown_p = num_bricks_to_grow;
        return kaps_jr1_uda_mgr_grow_down(mgr, cur_uda_chunk, num_bricks_to_grow);
    }

    if (grow_up_size == 0 && grow_down_size == 0)
        return KAPS_JR1_OK;

    if (grow_up_size > grow_down_size)
    {
        *flag_grown_up_p = 1;
        *num_bricks_grown_p = grow_up_size;
        return kaps_jr1_uda_mgr_grow_up(mgr, cur_uda_chunk, grow_up_size);
    }
    else
    {
        *flag_grown_up_p = 0;
        *num_bricks_grown_p = grow_down_size;
        return kaps_jr1_uda_mgr_grow_down(mgr, cur_uda_chunk, grow_down_size);
    }
}

kaps_jr1_status
kaps_jr1_uda_mgr_free(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * released_chunk)
{
    uint32_t prev_res_id = 0, next_res_id = 0, res_id = 0;
    struct uda_mem_chunk *prev_neighbor = NULL, *next_neighbor = NULL;
    uint32_t can_merge_prev_neighbor = 0, can_merge_next_neighbor = 0;
    int32_t total_bricks;
    uint32_t should_add_to_tail = 1;

    if (!released_chunk)
        return KAPS_JR1_INVALID_ARGUMENT;

    released_chunk->handle = NULL;

    if (mgr->device->type == KAPS_JR1_DEVICE)
        should_add_to_tail = 0;

    total_bricks = mgr->region_info[released_chunk->region_id].num_lpu *
        mgr->region_info[released_chunk->region_id].num_row;

    
    if (released_chunk->offset >= total_bricks)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    mgr->num_allocated_lpu_bricks -= released_chunk->size;
    mgr->num_allocated_lpu_bricks_per_region[released_chunk->region_id] -= released_chunk->size;

    prev_neighbor = released_chunk->prev_neighbor;
    next_neighbor = released_chunk->next_neighbor;

    res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, released_chunk);

    if (prev_neighbor && prev_neighbor->type == FREE_CHUNK)
    {
        prev_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, prev_neighbor);
        if ((prev_neighbor->region_id == released_chunk->region_id) && (prev_res_id + prev_neighbor->size == res_id))
            can_merge_prev_neighbor = 1;
    }

    if (next_neighbor && next_neighbor->type == FREE_CHUNK)
    {
        next_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, next_neighbor);
        if ((next_neighbor->region_id == released_chunk->region_id) && (next_res_id == res_id + released_chunk->size))
            can_merge_next_neighbor = 1;
    }

    if (can_merge_prev_neighbor && can_merge_next_neighbor)
    {
        kaps_jr1_uda_mgr_remove_from_free_list(mgr, prev_neighbor);
        kaps_jr1_uda_mgr_remove_from_free_list(mgr, next_neighbor);

        prev_neighbor->size += released_chunk->size + next_neighbor->size;

        kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, next_neighbor);
        kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, released_chunk);

        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, next_neighbor);
        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, released_chunk);

        if (should_add_to_tail)
        {
            kaps_jr1_uda_mgr_add_to_free_list_tail(mgr, prev_neighbor);
        }
        else
        {
            kaps_jr1_uda_mgr_add_to_free_list(mgr, prev_neighbor);
        }

    }
    else if (can_merge_prev_neighbor)
    {
        kaps_jr1_uda_mgr_remove_from_free_list(mgr, prev_neighbor);

        prev_neighbor->size += released_chunk->size;

        kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, released_chunk);
        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, released_chunk);

        if (should_add_to_tail)
        {
            kaps_jr1_uda_mgr_add_to_free_list_tail(mgr, prev_neighbor);
        }
        else
        {
            kaps_jr1_uda_mgr_add_to_free_list(mgr, prev_neighbor);
        }

    }
    else if (can_merge_next_neighbor)
    {
        kaps_jr1_uda_mgr_remove_from_free_list(mgr, next_neighbor);

        released_chunk->size += next_neighbor->size;
        released_chunk->type = FREE_CHUNK;

        kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, next_neighbor);
        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, next_neighbor);

        if (should_add_to_tail)
        {
            kaps_jr1_uda_mgr_add_to_free_list_tail(mgr, released_chunk);
        }
        else
        {
            kaps_jr1_uda_mgr_add_to_free_list(mgr, released_chunk);
        }

    }
    else
    {
        released_chunk->type = FREE_CHUNK;

        if (should_add_to_tail)
        {
            kaps_jr1_uda_mgr_add_to_free_list_tail(mgr, released_chunk);
        }
        else
        {
            kaps_jr1_uda_mgr_add_to_free_list(mgr, released_chunk);
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_destroy(
    struct kaps_jr1_uda_mgr * mgr)
{
    struct uda_mem_chunk *tmp, *next;
    struct kaps_jr1_allocator *alloc;

    if (!mgr)
        return KAPS_JR1_INVALID_ARGUMENT;

    alloc = mgr->uda_chunk_pool.allocator;

    tmp = mgr->neighbor_list;
    while (tmp)
    {
        next = tmp->next_neighbor;
        POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, tmp);
        tmp = next;
    }
    POOL_FINI(uda_mem_chunk, &mgr->uda_chunk_pool);
    alloc->xfree(alloc->cookie, mgr);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_print_detailed_stats(
    const struct kaps_jr1_uda_mgr * mgr)
{
    
#if 1
    int32_t free_count[MAX_UDA_CHUNK_LEVELS_ARRAY_SIZE];
    int32_t alloc_count[MAX_UDA_CHUNK_LEVELS_ARRAY_SIZE];
    struct uda_mem_chunk *neighbor_list;
    int32_t level;
    int32_t i, j;
    int32_t total, allocated = 0, free = 0, small = 0;

    kaps_jr1_memset(free_count, 0, sizeof(free_count));
    kaps_jr1_memset(alloc_count, 0, sizeof(alloc_count));
    neighbor_list = mgr->neighbor_list;

    while (neighbor_list)
    {
        if (neighbor_list->size < mgr->max_num_uda_chunk_levels)
        {
            level = neighbor_list->size - 1;
        }
        else
        {
            level = mgr->max_num_uda_chunk_levels - 1;
        }
        if (neighbor_list->type == FREE_CHUNK)
        {
            free_count[level]++;
            free += neighbor_list->size;
            if (neighbor_list->size <= mgr->max_lpus_in_a_chunk / 2)
                small += neighbor_list->size;
        }
        else
        {
            allocated += neighbor_list->size;
            alloc_count[level]++;
        }
        neighbor_list = neighbor_list->next_neighbor;
    }

    kaps_jr1_printf("\nUDA Mgr Statics, db id: %d, dt_index: %d, Half: %d, Num Region: %d\n", mgr->db->tid, mgr->dt_index,
                mgr->which_udm_half, mgr->no_of_regions);

    for (i = 0; i < mgr->no_of_regions; i++)
    {
        kaps_jr1_printf("Region Id: %d, Num Udm: %d, Udms:", i, mgr->region_info[i].num_lpu);
        for (j = 0; j < mgr->region_info[i].num_lpu; j++)
        {
            kaps_jr1_printf("%3d", mgr->region_info[i].udms[j]);
        }
        kaps_jr1_printf("\n");
    }

    kaps_jr1_printf("free chunks count     : ");
    for (i = 0; i < mgr->max_num_uda_chunk_levels; i++)
    {
        kaps_jr1_printf("%3d ", free_count[i]);
    }
    kaps_jr1_printf("\n");

    kaps_jr1_printf("allocated chunks count: ");
    for (i = 0; i < mgr->max_num_uda_chunk_levels; i++)
    {
        kaps_jr1_printf("%3d ", alloc_count[i]);
    }
    kaps_jr1_printf("\n");
    total = free + allocated;
    kaps_jr1_printf
        ("no of regions: %d, total bricks: %d, allocated  bricks: %d, free  bricks: %d, small  bricks: %d, percent small: %d\n",
         mgr->no_of_regions, total, allocated, free, small, total ? (small * 100) / total : 0);

#endif
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_calc_stats(
    const struct kaps_jr1_uda_mgr * mgr,
    struct uda_mgr_stats * uda_stats)
{
    uda_stats->total_num_allocated_lpu_bricks = mgr->num_allocated_lpu_bricks;
    uda_stats->total_num_free_lpu_bricks = mgr->total_lpu_bricks - mgr->num_allocated_lpu_bricks;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_print_stats(
    struct uda_mgr_stats * uda_stats)
{
    uint32_t total_num_bricks = uda_stats->total_num_allocated_lpu_bricks + uda_stats->total_num_free_lpu_bricks;

    float percent_allocated_bricks = uda_stats->total_num_allocated_lpu_bricks * 100.0F / total_num_bricks;

    float percent_free_bricks = uda_stats->total_num_free_lpu_bricks * 100.0F / total_num_bricks;

    kaps_jr1_printf("\n\nTotal number of allocated LPUs = %d  (%.2f%%) \n",
                uda_stats->total_num_allocated_lpu_bricks, percent_allocated_bricks);

    kaps_jr1_printf("Total number of free LPUs = %d (%.2f%%) \n\n", uda_stats->total_num_free_lpu_bricks,
                percent_free_bricks);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_usage(
    const struct kaps_jr1_uda_mgr * mgr_p,
    double *usage)
{
    struct uda_mgr_stats uda_stats;

    kaps_jr1_uda_mgr_calc_stats(mgr_p, &uda_stats);
    *usage = 1.0 * uda_stats.total_num_allocated_lpu_bricks /
        (uda_stats.total_num_allocated_lpu_bricks + uda_stats.total_num_free_lpu_bricks);
    
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_verify(
    struct kaps_jr1_uda_mgr * mgr)
{
    uint32_t prev_res_id, cur_res_id;
    uint32_t level, prev_size;
    struct uda_mem_chunk *cur_chunk = mgr->neighbor_list;
    int32_t region_id = 0;
    uint32_t total_free_size, total_alloc_size;
    struct uda_mem_chunk *uda_chunk_in_lsn;

    (void) uda_chunk_in_lsn;

    total_free_size = 0;
    total_alloc_size = 0;
    for (region_id = 0; region_id < mgr->no_of_regions; region_id++)
    {
        prev_res_id = 0;
        prev_size = 0;
        while (cur_chunk && cur_chunk->region_id == region_id)
        {
            cur_res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, cur_chunk);

            if (cur_chunk->prev_neighbor && (cur_chunk->prev_neighbor->region_id == cur_chunk->region_id))
            {
                if (prev_res_id + prev_size != cur_res_id)
                {
                    kaps_jr1_printf("Incorrect arrangement in sorted neighbor list ");
                    kaps_jr1_handle_error();
                    return KAPS_JR1_INTERNAL_ERROR;
                }
            }

            if (cur_chunk->type == FREE_CHUNK)
            {
                total_free_size += cur_chunk->size;
                if (cur_chunk->next_free_chunk == NULL && cur_chunk->prev_free_chunk == NULL)
                {

                    level = cur_chunk->size - 1;
                    if (level > (mgr->region_info[cur_chunk->region_id].num_lpu - 1))
                        level = mgr->region_info[cur_chunk->region_id].num_lpu - 1;

                    if (mgr->free_list[level] != cur_chunk)
                    {
                        kaps_jr1_printf("Incorrect arrangement in sorted neighbor list ");
                        kaps_jr1_handle_error();
                        return KAPS_JR1_INTERNAL_ERROR;
                    }
                }
            }

            if (cur_chunk->type == ALLOCATED_CHUNK)
            {
                total_alloc_size += cur_chunk->size;
            }

            prev_res_id = cur_res_id;
            prev_size = cur_chunk->size;

            cur_chunk = cur_chunk->next_neighbor;
        }

        kaps_jr1_sassert(mgr->which_udm_half != UDM_NO_HALF);

        if (mgr->which_udm_half == UDM_BOTH_HALF || mgr->which_udm_half == UDM_HIGHER_HALF)
        {
            if (prev_res_id + prev_size != (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row))
            {
                kaps_jr1_printf("Size in sorted neighbor list does not add up");
                kaps_jr1_handle_error();
                return KAPS_JR1_INTERNAL_ERROR;
            }
        }
        else
        {
            if (prev_res_id + prev_size !=
                (mgr->region_info[region_id].num_lpu * mgr->region_info[region_id].num_row / 2))
            {
                kaps_jr1_printf("Size in sorted neighbor list does not add up");
                kaps_jr1_handle_error();
                return KAPS_JR1_INTERNAL_ERROR;
            }
        }
    }

    if (total_alloc_size != mgr->num_allocated_lpu_bricks)
    {
        kaps_jr1_printf("The allocated size doesn't match");
        kaps_jr1_handle_error();
        return KAPS_JR1_INTERNAL_ERROR;
    }

    if (total_free_size != mgr->total_lpu_bricks - mgr->num_allocated_lpu_bricks)
    {
        kaps_jr1_printf("The free size doesn't match");
        kaps_jr1_handle_error();
        return KAPS_JR1_INTERNAL_ERROR;
    }

    for (level = 0; level < mgr->max_num_uda_chunk_levels; ++level)
    {
        cur_chunk = mgr->free_list[level];

        while (cur_chunk)
        {
            if (((cur_chunk->size - 1) != level) && ((mgr->region_info[cur_chunk->region_id].num_lpu) - 1 != level))
            {
                kaps_jr1_printf("Error in array of lists \n");
                kaps_jr1_handle_error();
                return KAPS_JR1_INTERNAL_ERROR;
            }
            cur_chunk = cur_chunk->next_free_chunk;
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_enable_dynamic_allocation(
    struct kaps_jr1_uda_mgr * mgr)
{
    mgr->enable_dynamic_allocation = 1;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_disble_dynamic_allocation(
    struct kaps_jr1_uda_mgr * mgr)
{
    mgr->enable_dynamic_allocation = 0;
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_release_region(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id)
{
    int32_t udm_index, udm_no, udc_no;
    struct kaps_jr1_device *device = mgr->device;
    int32_t region_size;

    if (device->main_dev)
        device = device->main_dev;

    region_size = kaps_jr1_uda_mgr_calc_region_size(mgr, region_id);
    if (mgr->enable_compaction)
    {
        region_size -= mgr->region_info[region_id].num_lpu * 2;
    }
    mgr->total_lpu_bricks -= region_size;

    for (udm_index = 0; udm_index < mgr->region_info[region_id].num_lpu; udm_index++)
    {
        udm_no = mgr->region_info[region_id].udms[udm_index];

        udc_no = udm_no / KAPS_JR1_UDM_PER_UDC;
        udm_no = udm_no % KAPS_JR1_UDM_PER_UDC;

        kaps_jr1_resource_release_udm(mgr->device, mgr->device->map, mgr->db, mgr->dt_index, udc_no, udm_no);
    }
    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_uda_mgr_release_all_regions(
    struct kaps_jr1_uda_mgr * mgr)
{
    int32_t region_id;

    for (region_id = 0; region_id < mgr->no_of_regions; region_id++)
    {
        kaps_jr1_uda_mgr_release_region(mgr, region_id);
    }
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_release_last_allocated_regions(
    struct kaps_jr1_uda_mgr * mgr)
{
    int32_t start_region_id, end_region_id;
    struct uda_mem_chunk *cur_chunk, *next_chunk;

    start_region_id = mgr->no_of_regions - 1;
    end_region_id = start_region_id;

    if (mgr->db->has_dup_db)
    {
        start_region_id -= 2;
    }

    cur_chunk = mgr->neighbor_list;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;
        if ((start_region_id <= cur_chunk->region_id) && cur_chunk->region_id <= end_region_id)
        {
            kaps_jr1_sassert(cur_chunk->type == FREE_CHUNK);
            kaps_jr1_uda_mgr_remove_from_free_list(mgr, cur_chunk);
            kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, cur_chunk);
        }
        cur_chunk = next_chunk;
    }


    kaps_jr1_uda_mgr_release_region(mgr, start_region_id);
    mgr->no_of_regions = mgr->no_of_regions - 1;

    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_uda_mgr_get_num_free_bricks(
    struct kaps_jr1_uda_mgr * mgr)
{
    return mgr->total_lpu_bricks - mgr->num_allocated_lpu_bricks;
}


uint32_t
kaps_jr1_uda_mgr_compute_brick_udc(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    int32_t brick_no)
{
    brick_no = (brick_no + uda_chunk->offset) % mgr->region_info[uda_chunk->region_id].num_lpu;
    return mgr->region_info[uda_chunk->region_id].udms[brick_no] / KAPS_JR1_UDM_PER_UDC;
}

uint32_t
kaps_jr1_uda_mgr_compute_brick_row(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    int32_t brick_no)
{
    int32_t udm_start_row = 0;
    int32_t lpu_index, row;

    lpu_index = (brick_no + uda_chunk->offset) % mgr->region_info[uda_chunk->region_id].num_lpu;
    row = (brick_no + uda_chunk->offset) / mgr->region_info[uda_chunk->region_id].num_lpu;

    udm_start_row =
        (mgr->region_info[uda_chunk->region_id].udms[lpu_index] % KAPS_JR1_UDM_PER_UDC) * mgr->device->hw_res->num_rows_in_bb;

    return (udm_start_row + row);
}

kaps_jr1_status
kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    int32_t brick_no,
    uint32_t * abs_udc,
    uint32_t * abs_row)
{
    int32_t udm_start_row = 0;
    int32_t lpu_index, row;

    kaps_jr1_sassert(brick_no < uda_chunk->size);

    lpu_index = (brick_no + uda_chunk->offset) % mgr->region_info[uda_chunk->region_id].num_lpu;
    row = (brick_no + uda_chunk->offset) / mgr->region_info[uda_chunk->region_id].num_lpu;

    udm_start_row =
        (mgr->region_info[uda_chunk->region_id].udms[lpu_index] % KAPS_JR1_UDM_PER_UDC) *
        mgr->region_info[uda_chunk->region_id].num_row;

    *abs_row = udm_start_row + row;
    *abs_udc = mgr->region_info[uda_chunk->region_id].udms[lpu_index] / KAPS_JR1_UDM_PER_UDC;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row_from_offset(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t offset,
    uint32_t * abs_udc,
    uint32_t * abs_row)
{
    int32_t udm_start_row = 0;
    int32_t lpu_index, row;


    lpu_index = (offset) % mgr->region_info[region_id].num_lpu;
    row = (offset) / mgr->region_info[region_id].num_lpu;

    udm_start_row =
        (mgr->region_info[region_id].udms[lpu_index] % KAPS_JR1_UDM_PER_UDC) *
        mgr->region_info[region_id].num_row;

    *abs_row = udm_start_row + row;
    *abs_udc = mgr->region_info[region_id].udms[lpu_index] / KAPS_JR1_UDM_PER_UDC;

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_print_abs_brick_udc_and_row(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    int32_t brick_no)
{
    uint32_t abs_udc = 100, abs_row = 0xEFFFFFFF;       

    kaps_jr1_uda_mgr_compute_abs_brick_udc_and_row(mgr, uda_chunk, brick_no, &abs_udc, &abs_row);
    kaps_jr1_printf("abs_udc: %d, abs_row: %d\n", abs_udc, abs_row);

    return KAPS_JR1_OK;
}

uint32_t
kaps_jr1_uda_mgr_compute_brick_row_relative(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    int32_t brick_no)
{
    return (brick_no + uda_chunk->offset) / mgr->region_info[uda_chunk->region_id].num_lpu;
}

uint32_t
kaps_jr1_uda_mgr_compute_brick_udm(
    struct kaps_jr1_uda_mgr * mgr,
    struct uda_mem_chunk * uda_chunk,
    int32_t brick_no)
{
    brick_no = (brick_no + uda_chunk->offset) % mgr->region_info[uda_chunk->region_id].num_lpu;
    return mgr->region_info[uda_chunk->region_id].udms[brick_no];
}

kaps_jr1_status
kaps_jr1_uda_mgr_get_no_of_contigous_bricks(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t offset)
{
    int brick_no, udm_no, prev_udm_no, count;

    brick_no = (offset) % mgr->region_info[region_id].num_lpu;
    prev_udm_no = mgr->region_info[region_id].udms[brick_no];
    count = 1;

    brick_no++;
    while (brick_no < mgr->region_info[region_id].num_lpu)
    {
        udm_no = mgr->region_info[region_id].udms[brick_no];
        if (udm_no != prev_udm_no + KAPS_JR1_UDM_PER_UDC)
        {
            break;
        }
        prev_udm_no = udm_no;
        count++;
        brick_no++;
    }
    return count;
}

uint32_t
kaps_jr1_uda_mgr_max_region_width(
    struct kaps_jr1_uda_mgr * mgr)
{
    int32_t i, width = 0;

    for (i = 0; i < mgr->no_of_regions; i++)
    {
        if (width < mgr->region_info[i].num_lpu)
        {
            width = mgr->region_info[i].num_lpu;
        }
    }

    return width;
}

uint32_t
kaps_jr1_uda_mgr_compute_brick_udc_from_region_id(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t offset,
    int32_t brick_no)
{
    brick_no = (brick_no + offset) % mgr->region_info[region_id].num_lpu;
    return mgr->region_info[region_id].udms[brick_no] / KAPS_JR1_UDM_PER_UDC;
}

kaps_jr1_status
kaps_jr1_uda_mgr_compute_brick_udc_and_row_from_region_id(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t chunk_offset,
    int32_t brick_no,
    uint32_t * p_abs_lpu,
    uint32_t * p_abs_row)
{
    int32_t total_offset;
    int32_t udm_row_start = 0;
    int32_t lpu_index;

    total_offset = chunk_offset + brick_no;

    lpu_index = total_offset % mgr->region_info[region_id].num_lpu;

    *p_abs_lpu = mgr->region_info[region_id].udms[lpu_index] / KAPS_JR1_UDM_PER_UDC;

    udm_row_start =
        (mgr->region_info[region_id].udms[lpu_index] % KAPS_JR1_UDM_PER_UDC) * mgr->region_info[region_id].num_row;

    *p_abs_row = udm_row_start + (total_offset / mgr->region_info[region_id].num_lpu);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_uda_mgr_get_relative_params_from_abs(
    struct kaps_jr1_uda_mgr * mgr,
    uint32_t lpu_no,
    uint32_t row_no,
    uint32_t * p_region_id,
    uint32_t * p_offset)
{
    uint32_t udm_no;
    uint32_t i, j;
    uint32_t relative_row;

    udm_no = lpu_no * KAPS_JR1_UDM_PER_UDC + (row_no / mgr->device->hw_res->num_rows_in_bb);
    relative_row = row_no % mgr->device->hw_res->num_rows_in_bb;

    for (i = 0; i < mgr->no_of_regions; i++)
    {
        for (j = 0; j < mgr->region_info[i].num_lpu; j++)
        {
            if (mgr->region_info[i].udms[j] == udm_no)
            {
                *p_region_id = i;
                *p_offset = relative_row * mgr->region_info[i].num_lpu + j;
                return KAPS_JR1_OK;
            }
        }
    }
    return KAPS_JR1_INTERNAL_ERROR;
}

kaps_jr1_status
kaps_jr1_uda_mgr_compact_new(
    struct kaps_jr1_uda_mgr * mgr,
    int32_t region_id,
    int32_t new_region_id)
{
    struct uda_mem_chunk *compaction_start_chunk = kaps_jr1_uda_mgr_get_region_start(mgr, region_id);
    struct uda_mem_chunk *cur_chunk, *prev_chunk;
    int32_t cb_start_offset = -1;
    int32_t cb_size = mgr->region_info[region_id].num_lpu * 2;
    int32_t new_region_filled_offset = 0, hole_size = 0, res_id = 0;
    struct uda_mem_chunk *cb_head, *cb_tail, *next_chunk;
    struct uda_mem_chunk *new_free_chunk;
    int32_t old_region_size, new_region_size, new_region_end_resource_id, cur_new_chunk_size;

    if (!mgr->enable_compaction)
        return KAPS_JR1_OUT_OF_UDA;

    new_region_filled_offset = kaps_jr1_uda_mgr_calc_start_res_id(mgr, new_region_id);
    if (mgr->enable_compaction)
        new_region_filled_offset += mgr->region_info[new_region_id].num_lpu * 2;

    res_id = kaps_jr1_uda_mgr_calc_res_id(mgr, compaction_start_chunk);
    res_id -= (mgr->region_info[region_id].num_lpu * 2);
    cb_start_offset = res_id;

    cb_head = NULL;
    cb_tail = NULL;

    prev_chunk = compaction_start_chunk->prev_neighbor;
    cur_chunk = compaction_start_chunk;

    while (cur_chunk)
    {

        next_chunk = cur_chunk->next_neighbor;

        if (cur_chunk->region_id != region_id)
            break;

        if (cur_chunk->type == FREE_CHUNK)
        {

            hole_size += cur_chunk->size;

            kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_jr1_uda_mgr_remove_from_free_list(mgr, cur_chunk);

            POOL_FREE(uda_mem_chunk, &mgr->uda_chunk_pool, cur_chunk);

        }
        else
        {

            if (hole_size < cur_chunk->size)
            {
                KAPS_JR1_STRY(kaps_jr1_uda_mgr_chunk_move(mgr, cur_chunk, cb_head, region_id, cb_start_offset, &cur_new_chunk_size));

                cur_chunk->size = cur_new_chunk_size;

                cb_start_offset += cur_chunk->size;
                hole_size += cur_chunk->size;
                cb_size -= cur_chunk->size;
                kaps_jr1_sassert(cb_size >= 0);

                kaps_jr1_uda_mgr_remove_from_neighbor_list(mgr, cur_chunk);
                kaps_jr1_uda_mgr_add_to_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);

            }
            else
            {
                KAPS_JR1_STRY(kaps_jr1_uda_mgr_chunk_move(mgr, cur_chunk, cb_head, new_region_id, new_region_filled_offset, &cur_new_chunk_size));
                cur_chunk->size = cur_new_chunk_size;

                new_region_filled_offset += cur_chunk->size;

                prev_chunk = cur_chunk;
            }
        }
        cur_chunk = next_chunk;
    }

    cur_chunk = cb_head;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;
        KAPS_JR1_STRY(kaps_jr1_uda_mgr_chunk_move(mgr, cur_chunk, cb_head, new_region_id, new_region_filled_offset, &cur_new_chunk_size));
        cur_chunk->size = cur_new_chunk_size;
        new_region_filled_offset += cur_chunk->size;

        kaps_jr1_uda_mgr_remove_from_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);
        kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

        prev_chunk = cur_chunk;
        cur_chunk = next_chunk;
    }

    old_region_size = kaps_jr1_uda_mgr_calc_region_size(mgr, region_id);
    new_region_size = kaps_jr1_uda_mgr_calc_region_size(mgr, new_region_id);
    new_region_end_resource_id = kaps_jr1_uda_mgr_calc_end_res_id(mgr, new_region_id);

    if (new_region_id != region_id)
    {
        if (mgr->enable_compaction)
        {
            
            mgr->total_lpu_bricks += mgr->region_info[region_id].num_lpu * 2;
            mgr->total_lpu_bricks -= old_region_size;
            mgr->total_lpu_bricks += new_region_size;
            mgr->total_lpu_bricks -= mgr->region_info[new_region_id].num_lpu * 2;
        }
        else
        {
            mgr->total_lpu_bricks += new_region_size - old_region_size;
        }
    }

    if (region_id != new_region_id)
    {
        kaps_jr1_memcpy(&mgr->region_info[region_id], &mgr->region_info[new_region_id], sizeof(mgr->region_info[0]));
    }

    if (new_region_end_resource_id > new_region_filled_offset)
    {
        
        POOL_ALLOC(uda_mem_chunk, &mgr->uda_chunk_pool, new_free_chunk);
        if (!new_free_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;
        kaps_jr1_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

        new_free_chunk->offset = new_region_filled_offset;
        new_free_chunk->region_id = region_id;
        
        new_free_chunk->size = new_region_end_resource_id - new_region_filled_offset;

        kaps_jr1_uda_mgr_add_to_free_list(mgr, new_free_chunk);
        kaps_jr1_uda_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);
    }

    KAPS_JR1_STRY(kaps_jr1_uda_mgr_verify(mgr));

    return KAPS_JR1_OK;
}

