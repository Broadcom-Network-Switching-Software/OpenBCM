

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kaps_jr1_ix_mgr.h"
#include "kaps_jr1_device_internal.h"
#include "kaps_jr1_ad.h"
#include "kaps_jr1_ad_internal.h"
#include "kaps_jr1_algo_common.h"
#include "kaps_jr1_algo_hw.h"
#include "kaps_jr1_fib_lsnmc.h"
#include "kaps_jr1_fib_lsnmc_hw.h"
#include "kaps_jr1_errors.h"

int32_t
kaps_jr1_ix_chunk_get_num_prefixes(
    void *lsn,
    struct kaps_jr1_ix_chunk *cur_chunk,
    int32_t * has_reserved_solt)
{
    kaps_jr1_lsn_mc *self = (kaps_jr1_lsn_mc *) lsn;

    if (has_reserved_solt)
        *has_reserved_solt = 0;

    if (cur_chunk->user_type == IX_LMPSOFAR)
    {
        kaps_jr1_sassert(cur_chunk->size == 1);
        return 1;
    }

    if (self->m_pSettings->m_isPerLpuGran)
    {
        kaps_jr1_lpm_lpu_brick *ixBrick = NULL;

        kaps_jr1_assert(self->m_ixInfo == NULL, "Invalid Ix info in the LSN ");

        ixBrick = self->m_lpuList;
        while (ixBrick)
        {
            if (ixBrick->m_ixInfo == cur_chunk)
                break;
            ixBrick = ixBrick->m_next_p;
        }


        kaps_jr1_sassert(ixBrick);
        if (has_reserved_solt && ixBrick->m_hasReservedSlot)
            *has_reserved_solt = 1;
        return ixBrick->m_numPfx;
    }
    else
    {
        return self->m_nNumPrefixes;
    }
}



int32_t
kaps_jr1_ix_mgr_ix_to_ad_blk(
    struct kaps_jr1_ix_mgr * mgr,
    uint32_t ix)
{
    uint32_t i;

    for (i = 0; i < mgr->max_num_ad_blks; ++i)
    {
        if (mgr->ix_translate[i] == -1)
            continue;

        if (mgr->ix_translate[i] <= ix && ix <= (mgr->ix_translate[i] + mgr->num_entries_per_ad_blk - 1))
        {
            return i;
        }
    }

    return -1;
}

static uint32_t
kaps_jr1_optimized_ix_to_ad_blk(
    struct kaps_jr1_ix_mgr *mgr,
    uint32_t guess_ad_blk,
    uint32_t rqt_ix)
{
    uint32_t found;
    int32_t ad_blk_no;

    
    found = 0;
    ad_blk_no = guess_ad_blk;
    while (ad_blk_no < mgr->max_num_ad_blks)
    {
        if (mgr->ix_translate[ad_blk_no] == -1)
        {
            ++ad_blk_no;
            continue;
        }

        if (mgr->ix_translate[ad_blk_no] <= rqt_ix && rqt_ix < mgr->ix_translate[ad_blk_no] + mgr->num_entries_per_ad_blk)
        {
            found = 1;
            break;
        }

        ++ad_blk_no;
    }

    
    if (!found)
    {
        ad_blk_no = kaps_jr1_ix_mgr_ix_to_ad_blk(mgr, rqt_ix);
    }

    if (ad_blk_no == -1 || ad_blk_no >= mgr->max_num_ad_blks)
    {
        kaps_jr1_assert(0, "Unable to find the SB for IX");
        return 0;
    }

    return (uint32_t) ad_blk_no;
}

uint32_t
kaps_jr1_is_chunk_in_continuous_ad_blk(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_chunk * cur_chunk)
{
    uint32_t i;
    static uint32_t start_ix_ad_blk = 0;
    uint32_t end_ix_ad_blk;
    struct kaps_jr1_device *device = NULL;
    uint32_t is_continuous;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    
    
    start_ix_ad_blk = kaps_jr1_optimized_ix_to_ad_blk(mgr, start_ix_ad_blk, cur_chunk->start_ix);
    end_ix_ad_blk = kaps_jr1_optimized_ix_to_ad_blk(mgr, start_ix_ad_blk, cur_chunk->start_ix + cur_chunk->size - 1);

    is_continuous = 1;

    
    i = start_ix_ad_blk + 1;
    while (i <= end_ix_ad_blk)
    {
        if (mgr->ix_translate[i] == -1)
        {
            is_continuous = 0;
            break;
        }

        if (mgr->ix_translate[i - 1] + mgr->num_entries_per_ad_blk != mgr->ix_translate[i])
        {
            is_continuous = 0;
            break;
        }

        ++i;
    }

    start_ix_ad_blk = end_ix_ad_blk;

    return is_continuous;
}

static uint32_t
kaps_jr1_check_ad_grow(
    struct kaps_jr1_ix_mgr *mgr,
    uint32_t chunk_loc_1,
    uint32_t chunk_loc_2,
    uint32_t * ix_translate)
{
    uint32_t ad_blk_no_1 = 0, ad_blk_no_2 = 0;
    uint32_t ad_blk_start_loc, ad_blk_end_loc;
    int32_t i;
    struct kaps_jr1_device *device = NULL;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    for (i = 0; i < mgr->max_num_ad_blks; i++)
    {
        if (ix_translate[i] == -1)
            continue;

        ad_blk_start_loc = ix_translate[i];
        ad_blk_end_loc = ix_translate[i] + mgr->num_entries_per_ad_blk - 1;
        if (ad_blk_start_loc <= chunk_loc_1 && chunk_loc_1 <= ad_blk_end_loc)
        {
            ad_blk_no_1 = i;
            break;
        }

    }

    for (i = 0; i < mgr->max_num_ad_blks; i++)
    {
        if (ix_translate[i] == -1)
            continue;
        ad_blk_start_loc = ix_translate[i];
        ad_blk_end_loc = ix_translate[i] + mgr->num_entries_per_ad_blk - 1;
        if (ad_blk_start_loc <= chunk_loc_2 && chunk_loc_2 <= ad_blk_end_loc)
        {
            ad_blk_no_2 = i;
            break;
        }
    }

    if (ad_blk_no_1 == ad_blk_no_2)
    {
        return 1;
    }

    if (ad_blk_no_2 == (ad_blk_no_1 + 1))
    {
        return 1;
    }

    return 0;

}

static uint32_t
kaps_jr1_ad_get_ad_blk_for_chunk(
    struct kaps_jr1_ix_mgr *mgr,
    uint32_t start_loc,
    uint32_t size,
    uint32_t rqt_size,
    uint32_t * ix_translate)
{
    int32_t i;
    uint32_t ad_blk_end_loc;

    
    for (i = 0; i < mgr->max_num_ad_blks; i++)
    {
        if (ix_translate[i] == -1)
            continue;

        ad_blk_end_loc = ix_translate[i] + mgr->num_entries_per_ad_blk - 1;

        if (start_loc > ad_blk_end_loc)
            continue;

        if (start_loc >= ix_translate[i])
        {
            
            if ((start_loc + rqt_size - 1) <= ad_blk_end_loc)
                return i;
            
            if (((i + 1) < mgr->max_num_ad_blks) && (ix_translate[i + 1] == ad_blk_end_loc + 1))
            {
                return i;
            }

        }

    }
    return -1;
}


static uint32_t
kaps_jr1_ad_get_chunk(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ad_db *ad_db,
    uint32_t rqt_size,
    uint32_t level,
    struct kaps_jr1_ix_chunk **chunk)
{
    struct kaps_jr1_ix_chunk *temp = NULL;
    uint32_t ad_blk_no = -1;

    temp = mgr->free_list[level];

    
    while (temp)
    {
        kaps_jr1_sassert(temp->size >= rqt_size);
        ad_blk_no =
            kaps_jr1_ad_get_ad_blk_for_chunk(mgr, temp->start_ix, temp->size, rqt_size, mgr->ix_translate);
        if (ad_blk_no != -1)
        {
            *chunk = temp;
            return ad_blk_no;
        }
        temp = temp->next_free_chunk;
    }

    return -1;
}


static struct kaps_jr1_ix_chunk *
kaps_jr1_merge_lists(
    struct kaps_jr1_ix_chunk *ptr1,
    struct kaps_jr1_ix_chunk *ptr2)
{
    struct kaps_jr1_ix_chunk *head, *tmp;

    if (!ptr1)
        return ptr2;
    else if (!ptr2)
        return ptr1;

    if (ptr1->start_ix > ptr2->start_ix)
    {
        tmp = ptr1;
        ptr1 = ptr2;
        ptr2 = tmp;
    }
    head = ptr1;

    while (ptr1 && ptr2)
    {
        if (ptr1->next_neighbor)
        {
            if (ptr1->next_neighbor->start_ix > ptr2->start_ix)
            {
                tmp = ptr2->next_neighbor;
                ptr2->next_neighbor = ptr1->next_neighbor;
                ptr1->next_neighbor = ptr2;
                ptr2 = tmp;
                ptr1 = ptr1->next_neighbor;
            }
            else
            {
                ptr1 = ptr1->next_neighbor;
            }
        }
        else
        {
            ptr1->next_neighbor = ptr2;
            break;
        }
    }

    return head;
}

static struct kaps_jr1_ix_chunk *
kaps_jr1_divide_and_sort(
    struct kaps_jr1_ix_chunk *ptr,
    uint32_t num_elements)
{
    struct kaps_jr1_ix_chunk *first_list_head;
    struct kaps_jr1_ix_chunk *second_list_head;
    uint32_t first_list_count, second_list_count;
    uint32_t i;

    if (num_elements <= 1)
    {
        if (ptr)
        {
            ptr->next_neighbor = NULL;
        }
        return ptr;
    }

    first_list_head = ptr;
    first_list_count = num_elements / 2;

    for (i = 0; i < first_list_count; ++i)
    {
        ptr = ptr->next_neighbor;
    }

    second_list_head = ptr;
    second_list_count = num_elements - first_list_count;

    first_list_head = kaps_jr1_divide_and_sort(first_list_head, first_list_count);
    second_list_head = kaps_jr1_divide_and_sort(second_list_head, second_list_count);

    return kaps_jr1_merge_lists(first_list_head, second_list_head);

}

static struct kaps_jr1_ix_chunk *
kaps_jr1_merge_sort(
    struct kaps_jr1_ix_chunk *list)
{
    struct kaps_jr1_ix_chunk *cur_node = list;
    uint32_t count = 0;

    
    while (cur_node)
    {
        count++;
        cur_node = cur_node->next_neighbor;
    }

    
    list = kaps_jr1_divide_and_sort(list, count);
    cur_node = list;
    cur_node->prev_neighbor = NULL;

    while (cur_node->next_neighbor)
    {
        cur_node->next_neighbor->prev_neighbor = cur_node;
        cur_node = cur_node->next_neighbor;
    }

    while (cur_node->prev_neighbor)
    {
        cur_node = cur_node->prev_neighbor;
    }


    return list;
}

static void
kaps_jr1_handle_error(
    void)
{
    kaps_jr1_sassert(0);
}

static void
kaps_jr1_ix_mgr_remove_from_free_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;

    if (level >= mgr->max_num_ix_chunk_levels)
        level = mgr->max_num_ix_chunk_levels - 1;

    if (cur_chunk->prev_free_chunk)
    {
        cur_chunk->prev_free_chunk->next_free_chunk = cur_chunk->next_free_chunk;
    }
    else
    {
        mgr->free_list[level] = cur_chunk->next_free_chunk;
    }

    mgr->num_ix_in_free_list[level] -= cur_chunk->size;

    if (cur_chunk->next_free_chunk)
    {
        cur_chunk->next_free_chunk->prev_free_chunk = cur_chunk->prev_free_chunk;
    }
}

static void
kaps_jr1_ix_mgr_add_to_free_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_chunk)
{
    uint32_t level = cur_chunk->size - 1;
    struct kaps_jr1_ix_chunk *next_free_chunk;
    struct kaps_jr1_ix_chunk **prev_free_chunk_p;

    if (level >= mgr->max_num_ix_chunk_levels)
        level = mgr->max_num_ix_chunk_levels - 1;

    cur_chunk->prev_free_chunk = NULL;
    cur_chunk->next_free_chunk = mgr->free_list[level];

    mgr->free_list[level] = cur_chunk;
    mgr->num_ix_in_free_list[level] += cur_chunk->size;

    next_free_chunk = cur_chunk->next_free_chunk;
    if (next_free_chunk == NULL)
    {
        return;
    }
    prev_free_chunk_p = &(next_free_chunk->prev_free_chunk);
    *prev_free_chunk_p = cur_chunk;
}

static void
kaps_jr1_ix_mgr_remove_from_neighbor_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_chunk)
{
    if (cur_chunk->prev_neighbor)
    {
        cur_chunk->prev_neighbor->next_neighbor = cur_chunk->next_neighbor;
    }
    else
    {
        mgr->neighbor_list = cur_chunk->next_neighbor;
    }

    if (cur_chunk == mgr->neighbor_list_end)
    {
        mgr->neighbor_list_end = cur_chunk->prev_neighbor;
    }

    if (cur_chunk->next_neighbor)
    {
        cur_chunk->next_neighbor->prev_neighbor = cur_chunk->prev_neighbor;
    }
}

static void
kaps_jr1_ix_mgr_add_to_neighbor_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *prev_chunk,
    struct kaps_jr1_ix_chunk *cur_chunk)
{
    struct kaps_jr1_ix_chunk *next_neighbor;
    struct kaps_jr1_ix_chunk **prev_neighbor_p;

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

    if (prev_chunk == mgr->neighbor_list_end)
    {
        mgr->neighbor_list_end = cur_chunk;
    }
    next_neighbor = cur_chunk->next_neighbor;
    if (next_neighbor)
    {
        prev_neighbor_p = &(next_neighbor->prev_neighbor);
        *prev_neighbor_p = cur_chunk;
    }
}

static void
kaps_jr1_ix_mgr_add_to_cb_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_chunk,
    struct kaps_jr1_ix_chunk **cb_head_pp,
    struct kaps_jr1_ix_chunk **cb_tail_pp)
{
    struct kaps_jr1_ix_chunk *cb_head = *cb_head_pp, *cb_tail = *cb_tail_pp;

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
kaps_jr1_ix_mgr_remove_from_cb_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_chunk,
    struct kaps_jr1_ix_chunk **cb_head_pp,
    struct kaps_jr1_ix_chunk **cb_tail_pp)
{
    struct kaps_jr1_ix_chunk *cb_head = *cb_head_pp;

    kaps_jr1_sassert(cur_chunk == cb_head);

    *cb_head_pp = cur_chunk->next_neighbor;

    if (!(*cb_head_pp))
        *cb_tail_pp = NULL;

    cur_chunk->next_neighbor = NULL;
}

static void
kaps_jr1_ix_mgr_add_to_playback_list(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_compaction_chunk *playback_chunk,
    struct kaps_jr1_ix_compaction_chunk **playback_head_pp,
    struct kaps_jr1_ix_compaction_chunk **playback_tail_pp)
{
    struct kaps_jr1_ix_compaction_chunk *playback_head = *playback_head_pp, *playback_tail = *playback_tail_pp;

    playback_chunk->next = NULL;

    if (playback_head)
    {
        playback_tail->next = playback_chunk;
        *playback_tail_pp = playback_chunk;
    }
    else
    {
        *playback_head_pp = playback_chunk;
        *playback_tail_pp = playback_chunk;
    }
}


void
kaps_jr1_find_longest_continuous_chunk(
    struct kaps_jr1_ix_mgr *mgr,
    uint32_t cur_chunk_start_ix,
    uint32_t final_end_ix,
    uint32_t * cur_chunk_end_ix_p)
{
    struct kaps_jr1_device *device = NULL;
    uint32_t i;
    static uint32_t cur_ad_blk = 0;

    if (mgr->main_ad_db)
    {
        device = mgr->main_ad_db->db_info.device;
        if (0 && device->main_bc_device)
            device = device->main_bc_device;
    }

    *cur_chunk_end_ix_p = 0;

    cur_ad_blk = kaps_jr1_optimized_ix_to_ad_blk(mgr, cur_ad_blk, cur_chunk_start_ix);

    
    if (final_end_ix < mgr->ix_translate[cur_ad_blk] + mgr->num_entries_per_ad_blk)
    {
        *cur_chunk_end_ix_p = final_end_ix;
        return;
    }

    
    *cur_chunk_end_ix_p = mgr->ix_translate[cur_ad_blk] + mgr->num_entries_per_ad_blk - 1;

    i = cur_ad_blk + 1;
    while (i < mgr->max_num_ad_blks)
    {
        if (mgr->ix_translate[i] == -1)
        {
            break;
        }

        if (mgr->ix_translate[i - 1] + mgr->num_entries_per_ad_blk != mgr->ix_translate[i])
        {
            break;
        }

        
        if (final_end_ix < mgr->ix_translate[i] + mgr->num_entries_per_ad_blk)
        {
            *cur_chunk_end_ix_p = final_end_ix;
            break;
        }

        
        *cur_chunk_end_ix_p = mgr->ix_translate[i] + mgr->num_entries_per_ad_blk - 1;

        ++i;
    }
}


kaps_jr1_status
kaps_jr1_ix_mgr_split_free_chunk(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *old_free_chunk)
{
    struct kaps_jr1_ix_chunk *prev_free_chunk, *next_free_chunk;
    uint32_t num_chunks;
    uint32_t is_old_chunk_at_end = 0;
    uint32_t old_chunk_start_ix, old_chunk_end_ix;
    uint32_t cur_chunk_start_ix, cur_chunk_end_ix;

    prev_free_chunk = old_free_chunk->prev_neighbor;
    if (old_free_chunk->next_neighbor == NULL)
    {
        is_old_chunk_at_end = 1;        
    }

    old_chunk_start_ix = old_free_chunk->start_ix;
    old_chunk_end_ix = old_free_chunk->start_ix + old_free_chunk->size - 1;

    
    kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, old_free_chunk);
    kaps_jr1_ix_mgr_remove_from_free_list(mgr, old_free_chunk);

    num_chunks = 0;
    cur_chunk_start_ix = old_chunk_start_ix;

    while (cur_chunk_start_ix <= old_chunk_end_ix)
    {

        
        cur_chunk_end_ix = 0;
        kaps_jr1_find_longest_continuous_chunk(mgr, cur_chunk_start_ix, old_chunk_end_ix, &cur_chunk_end_ix);

        
        POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, next_free_chunk);
        if (!next_free_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;

        kaps_jr1_memset(next_free_chunk, 0, sizeof(*next_free_chunk));

        next_free_chunk->start_ix = cur_chunk_start_ix;
        next_free_chunk->size = cur_chunk_end_ix - cur_chunk_start_ix + 1;

        next_free_chunk->type = IX_FREE_CHUNK;

        kaps_jr1_ix_mgr_add_to_free_list(mgr, next_free_chunk);
        kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_free_chunk, next_free_chunk);

        if (is_old_chunk_at_end)
            mgr->neighbor_list_end = next_free_chunk;

        ++num_chunks;

        prev_free_chunk = next_free_chunk;

        cur_chunk_start_ix = cur_chunk_end_ix + 1;

    }

    
    POOL_FREE(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, old_free_chunk);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_normalize_free_chunks(
    struct kaps_jr1_ix_mgr * mgr)
{
    struct kaps_jr1_ix_chunk *cur_chunk, *next_chunk;

    cur_chunk = mgr->neighbor_list;

    

    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (!kaps_jr1_is_chunk_in_continuous_ad_blk(mgr, cur_chunk))
            {
                KAPS_JR1_STRY(kaps_jr1_ix_mgr_split_free_chunk(mgr, cur_chunk));
            }
        }

        cur_chunk = next_chunk;
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_init(
    struct kaps_jr1_device * device,
    uint32_t max_alloc_chunk_size,
    struct kaps_jr1_ad_db * ad_db,
    uint32_t daisy_chain_id,
    uint8_t is_warmboot)
{
    struct kaps_jr1_ix_mgr *mgr = NULL;
    struct kaps_jr1_ix_chunk *cur_free_chunk = NULL , *prev_free_chunk;
    uint32_t max_num_ix_chunk_levels = 0;
    int32_t i;
    struct memory_map *mem_map = device->map;
    uint32_t base_address;


    (void) cur_free_chunk;
    (void) prev_free_chunk;
    (void) mem_map;
    (void) base_address;
    (void) i;

    kaps_jr1_assert(ad_db, "ad_db should not be NULL \n");

    
    max_num_ix_chunk_levels = 1;
    while (max_num_ix_chunk_levels < max_alloc_chunk_size)
    {
        max_num_ix_chunk_levels = max_num_ix_chunk_levels << 1;
    }

    if (is_warmboot)
        max_num_ix_chunk_levels = max_alloc_chunk_size;

    mgr = device->alloc->xcalloc(device->alloc->cookie, 1, sizeof(struct kaps_jr1_ix_mgr));
    if (!mgr)
        return KAPS_JR1_OUT_OF_MEMORY;

    mgr->free_list =
        device->alloc->xcalloc(device->alloc->cookie, max_num_ix_chunk_levels, sizeof(struct kaps_jr1_ix_chunk *));
    if (!mgr->free_list)
    {
        kaps_jr1_ix_mgr_destroy(mgr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    mgr->num_ix_in_free_list = device->alloc->xcalloc(device->alloc->cookie, max_num_ix_chunk_levels, sizeof(uint32_t));
    if (!mgr->num_ix_in_free_list)
    {
        kaps_jr1_ix_mgr_destroy(mgr);
        return KAPS_JR1_OUT_OF_MEMORY;
    }

    mgr->num_allocated_ix = 0;
    mgr->max_alloc_chunk_size = max_alloc_chunk_size;
    mgr->max_num_ix_chunk_levels = max_num_ix_chunk_levels;
    mgr->device = device;
    mgr->ref_count = 1;
    mgr->daisy_chain_id = daisy_chain_id;
    mgr->ad_width_1 = ad_db->db_info.width.ad_width_1;
    mgr->ad_type = ad_db->db_info.hw_res.ad_res->ad_type;
    mgr->max_ix_per_daisy_chain = 16 * 1024 * 1024;
    mgr->ix_max_holes_allowed = ad_db->ix_max_num_holes_allowed;

    POOL_INIT(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, device->alloc);

    kaps_jr1_sassert(ad_db->mgr[device->core_id][daisy_chain_id] == NULL);
    ad_db->mgr[device->core_id][daisy_chain_id] = mgr;

    mgr->main_ad_db = ad_db;

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_init_for_all_ad_dbs(
    struct kaps_jr1_device * device,
    uint32_t max_alloc_chunk_size,
    struct kaps_jr1_db * db,
    uint8_t is_warmboot)
{
    uint32_t daisy_chain_id;
    struct kaps_jr1_ad_db *cur_ad_db, *prev_ad_db;
    uint32_t cur_ad_db_iter, prev_ad_db_iter;
    uint32_t found = 0, num_ad = 0;

    cur_ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;
    while (cur_ad_db)
    {
        num_ad++;
        cur_ad_db = cur_ad_db->next;
    }

    cur_ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;
    cur_ad_db_iter = 0;
    while (cur_ad_db)
    {
        found = 0;
        prev_ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;
        prev_ad_db_iter = 0;

        if (num_ad > 1)
        {
            cur_ad_db->ix_max_num_holes_allowed = 2;
        }

        
        while (prev_ad_db_iter < cur_ad_db_iter)
        {
            if (prev_ad_db->user_width_1 == cur_ad_db->user_width_1
                 && prev_ad_db->db_info.hw_res.ad_res->ad_type == KAPS_JR1_AD_TYPE_INDEX
                 && cur_ad_db->db_info.hw_res.ad_res->ad_type == KAPS_JR1_AD_TYPE_INDEX)
            {
                found = 1;
                break;
            }

            prev_ad_db_iter++;
            prev_ad_db = prev_ad_db->next;
        }

        if (found)
        {
            
            for (daisy_chain_id = 0; daisy_chain_id < device->hw_res->num_daisy_chains; ++daisy_chain_id)
            {
                cur_ad_db->mgr[device->core_id][daisy_chain_id] = prev_ad_db->mgr[device->core_id][daisy_chain_id];
            }
        }
        else
        {
            for (daisy_chain_id = 0; daisy_chain_id < device->hw_res->num_daisy_chains; ++daisy_chain_id)
            {
                KAPS_JR1_STRY(kaps_jr1_ix_mgr_init(device, max_alloc_chunk_size,
                                cur_ad_db, daisy_chain_id, is_warmboot));
            }
        }

        cur_ad_db_iter++;
        cur_ad_db = cur_ad_db->next;
    }

    return KAPS_JR1_OK;
}




kaps_jr1_status
kaps_jr1_ix_mgr_alloc_internal(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ad_db * ad_db,
    uint32_t rqt_size,
    void *lsn_ptr,
    enum kaps_jr1_ix_user_type user_type,
    struct kaps_jr1_ix_chunk ** alloc_chunk_pp)
{
    struct kaps_jr1_ix_chunk *chosen_chunk, *remainder_chunk = NULL, *remainder_chunk_2 = NULL;
    uint32_t level;
    uint32_t ad_blk_no = -1;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;
    struct kaps_jr1_ad_db *cur_ad_db = ad_db;

    *alloc_chunk_pp = NULL;

    level = rqt_size - 1;
    if (rqt_size == 0 || level > mgr->max_num_ix_chunk_levels - 1)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->ad_type == KAPS_JR1_AD_TYPE_INPLACE)
        cur_ad_db = NULL;


    
    chosen_chunk = NULL;
    if (cur_mgr->free_list[level])
    {
        if (!cur_ad_db)
            chosen_chunk = cur_mgr->free_list[level];
        else
            ad_blk_no = kaps_jr1_ad_get_chunk(cur_mgr, ad_db, rqt_size, level, &chosen_chunk);
    }

    if (!chosen_chunk)
    {
        while (level < cur_mgr->max_num_ix_chunk_levels)
        {
            if (cur_mgr->free_list[level] && level >= rqt_size)
            {
                if (cur_ad_db)
                {
                    ad_blk_no = kaps_jr1_ad_get_chunk(cur_mgr, ad_db, rqt_size, level, &chosen_chunk);
                    if (ad_blk_no != -1)
                        break;
                }
                else
                {
                    chosen_chunk = cur_mgr->free_list[level];
                    break;
                }
            }
            ++level;
        }
    }

    if (!chosen_chunk)
        return KAPS_JR1_OUT_OF_INDEX;

    if (chosen_chunk->size > rqt_size)
    {
        POOL_ALLOC(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, remainder_chunk);
        if (!remainder_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;
        kaps_jr1_memset(remainder_chunk, 0, sizeof(*remainder_chunk));
        if (cur_ad_db && cur_mgr->ix_translate[ad_blk_no] > chosen_chunk->start_ix)
        {
            POOL_ALLOC(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, remainder_chunk_2);
            if (!remainder_chunk_2)
                return KAPS_JR1_OUT_OF_MEMORY;
            kaps_jr1_memset(remainder_chunk_2, 0, sizeof(*remainder_chunk_2));
        }
    }

    
    kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, chosen_chunk);

    if (remainder_chunk_2)
    {

        remainder_chunk_2->size = cur_mgr->ix_translate[ad_blk_no] - chosen_chunk->start_ix;
        remainder_chunk_2->start_ix = chosen_chunk->start_ix;
        remainder_chunk_2->type = IX_FREE_CHUNK;

        
        chosen_chunk->start_ix = cur_mgr->ix_translate[ad_blk_no];

        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, remainder_chunk_2);
        kaps_jr1_ix_mgr_add_to_neighbor_list(cur_mgr, chosen_chunk->prev_neighbor, remainder_chunk_2);

        remainder_chunk->size = chosen_chunk->size - remainder_chunk_2->size - rqt_size;
        if (remainder_chunk->size)
        {
            remainder_chunk->start_ix = cur_mgr->ix_translate[ad_blk_no] + rqt_size;
            remainder_chunk->type = IX_FREE_CHUNK;

            kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, remainder_chunk);
            kaps_jr1_ix_mgr_add_to_neighbor_list(cur_mgr, chosen_chunk, remainder_chunk);
        }
        else
        {
            POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, remainder_chunk);
        }

    }
    else if (remainder_chunk)
    {   
        remainder_chunk->size = chosen_chunk->size - rqt_size;
        remainder_chunk->start_ix = chosen_chunk->start_ix + rqt_size;
        remainder_chunk->type = IX_FREE_CHUNK;

        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, remainder_chunk);
        kaps_jr1_ix_mgr_add_to_neighbor_list(cur_mgr, chosen_chunk, remainder_chunk);
    }

    chosen_chunk->type = IX_ALLOCATED_CHUNK;
    chosen_chunk->next_free_chunk = NULL;
    chosen_chunk->prev_free_chunk = NULL;
    chosen_chunk->size = rqt_size;

    if (ad_db)
    {
        chosen_chunk->ad_info =
            ad_db->db_info.device->alloc->xcalloc(ad_db->db_info.device->alloc->cookie, 1,
                                                  sizeof(struct kaps_jr1_ad_chunk));
        if (!chosen_chunk->ad_info)
            return KAPS_JR1_OUT_OF_MEMORY;
        chosen_chunk->ad_info->device = ad_db->db_info.device;
        chosen_chunk->ad_info->ad_db = ad_db;
    }

    if (cur_ad_db)
    {

        chosen_chunk->ad_info->ad_blk_no = ad_blk_no;
        chosen_chunk->ad_info->offset_in_ad_blk = chosen_chunk->start_ix - cur_mgr->ix_translate[ad_blk_no];
        cur_mgr->ad_blk_info[ad_blk_no].allocated += chosen_chunk->size;

    }

    cur_mgr->num_allocated_ix += rqt_size;
    kaps_jr1_sassert(cur_mgr->num_allocated_ix <= cur_mgr->size);
    chosen_chunk->lsn_ptr = lsn_ptr;
    chosen_chunk->user_type = user_type;
    *alloc_chunk_pp = chosen_chunk;

    return KAPS_JR1_OK;

}

static void
kaps_jr1_ad_get_ad_blk_nr(
    struct kaps_jr1_ix_mgr *mgr,
    int32_t start_loc,
    int32_t size,
    int32_t * ad_blk_no,
    int32_t * incr_start_loc)
{
    (void) size;

    *incr_start_loc = 0;

    *ad_blk_no = start_loc / mgr->ix_boundary_per_ad_blk;

}

int
kaps_jr1_qsort_of_free_space(
    const void *a,
    const void *b)
{
    const struct kaps_jr1_ad_blk_info *rec1 = a;
    const struct kaps_jr1_ad_blk_info *rec2 = b;
    int rec1_alloc = (rec1->size - rec1->allocated), rec2_alloc = (rec2->size - rec2->allocated);


    if (rec1_alloc == rec2_alloc)
    {
        return 0;
    }
    else if (rec1_alloc > rec2_alloc)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

struct kaps_jr1_ix_chunk *
kaps_jr1_ix_mgr_get_start_chunk_for_ad_blk(struct kaps_jr1_ix_mgr *mgr, int32_t ad_blk)
{
    int32_t start_ix = (mgr->max_ix_per_daisy_chain * mgr->daisy_chain_id)
                                    + (ad_blk * mgr->ix_boundary_per_ad_blk);
    struct kaps_jr1_ix_chunk *cur_chunk = mgr->neighbor_list;


    if (ad_blk == mgr->cb_blk)
        start_ix += (2 * mgr->max_alloc_chunk_size);

    while (cur_chunk)
    {
        if (cur_chunk->start_ix == start_ix)
            return cur_chunk;

        cur_chunk = cur_chunk->next_neighbor;
    }
    return NULL;
}

struct kaps_jr1_ix_chunk *
kaps_jr1_ix_mgr_get_end_chunk_for_ad_blk(struct kaps_jr1_ix_mgr *mgr, int32_t ad_blk)
{
    int32_t start_ix = (mgr->max_ix_per_daisy_chain * mgr->daisy_chain_id)
                                    + (ad_blk * mgr->ix_boundary_per_ad_blk);
    struct kaps_jr1_ix_chunk *next_chunk, *cur_chunk = mgr->neighbor_list;


    if (ad_blk == mgr->cb_blk)
        start_ix += (2 * mgr->max_alloc_chunk_size);

    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        if (next_chunk)
        {
            if (cur_chunk->start_ix + cur_chunk->size != next_chunk->start_ix)
            {
                return cur_chunk;
            }
        }
        cur_chunk = cur_chunk->next_neighbor;
    }
    return NULL;
}

kaps_jr1_status kaps_jr1_ix_mgr_move_chunk(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ix_chunk *cur_chunk,
    int32_t *dest_start_ix,
    int32_t *free_slots_in_brick)
{
    int32_t num_pfx = kaps_jr1_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, NULL);
    int32_t num_empty_slots_in_brick = 0, num_holes_allowed = mgr->ix_max_holes_allowed / 2;
    struct kaps_jr1_ix_compaction_chunk playback_chunk;

    if (num_holes_allowed == 0)
        num_holes_allowed = 1;

    kaps_jr1_memset(&playback_chunk, 0, sizeof(struct kaps_jr1_ix_compaction_chunk));

    if (cur_chunk->size > num_pfx + num_holes_allowed)
    {
        num_empty_slots_in_brick = cur_chunk->size - num_pfx - num_holes_allowed;
    }

    playback_chunk.kaps_jr1_ix_chunk = cur_chunk;
    playback_chunk.from_start_ix = cur_chunk->start_ix;
    playback_chunk.from_size = cur_chunk->size;
    playback_chunk.from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
    playback_chunk.to_start_ix = (*dest_start_ix);
    playback_chunk.to_size = cur_chunk->size - num_empty_slots_in_brick;
    playback_chunk.to_end_ix = (*dest_start_ix) + playback_chunk.to_size - 1;

    playback_chunk.to_ad_blk_no = kaps_jr1_ix_mgr_ix_to_ad_blk(mgr, playback_chunk.to_start_ix);
    kaps_jr1_sassert (playback_chunk.to_ad_blk_no != -1);
    playback_chunk.to_offset_in_ad_blk = playback_chunk.to_start_ix - mgr->ix_translate[playback_chunk.to_ad_blk_no];

    (*dest_start_ix) += cur_chunk->size - num_empty_slots_in_brick;
    mgr->num_allocated_ix -= num_empty_slots_in_brick;
    *free_slots_in_brick = num_empty_slots_in_brick;

    KAPS_JR1_STRY(kaps_jr1_lsn_mc_ad_chunk_move(mgr, &playback_chunk));

    cur_chunk->start_ix = playback_chunk.to_start_ix;
    cur_chunk->size = playback_chunk.to_size;

    return KAPS_JR1_OK;
}

kaps_jr1_status kaps_jr1_ix_mgr_move_ad_blk(
    struct kaps_jr1_ix_mgr *mgr,
    int32_t src_ad_blk_num,
    int32_t dest_ad_blk_num,
    struct kaps_jr1_ix_chunk *ix_free_chunk)
{
    struct kaps_jr1_ix_chunk *cur_chunk,*next_chunk,*prev_chunk, *new_free_chunk, *blk_prev_chunk;
    int32_t cur_blk_hole = 0, end_of_blk = 0;
    int32_t dest_start_ix, dest_hole_size;

    if (ix_free_chunk == NULL)
        return KAPS_JR1_OK;

    dest_start_ix = ix_free_chunk->start_ix;
    dest_hole_size = ix_free_chunk->size;
    cur_chunk = kaps_jr1_ix_mgr_get_start_chunk_for_ad_blk(mgr, src_ad_blk_num);
    prev_chunk = ix_free_chunk;
    blk_prev_chunk = cur_chunk->prev_neighbor;


    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        if (end_of_blk)
            break;

        if (next_chunk)
        {
            if (cur_chunk->start_ix + cur_chunk->size != next_chunk->start_ix)
            {
                end_of_blk = 1;
            }
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            cur_blk_hole += cur_chunk->size;

            kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_jr1_ix_mgr_remove_from_free_list(mgr, cur_chunk);
            POOL_FREE(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, cur_chunk);

        } else {
            int32_t free_slots_in_brick = 0;

            if (dest_hole_size < cur_chunk->size)
                break;

            cur_blk_hole += cur_chunk->size;

            mgr->ad_blk_info[src_ad_blk_num].allocated -= cur_chunk->size;

            KAPS_JR1_STRY(kaps_jr1_ix_mgr_move_chunk(mgr, cur_chunk, &dest_start_ix, &free_slots_in_brick));

            kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);

            kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

            dest_hole_size -= cur_chunk->size;
            mgr->ad_blk_info[dest_ad_blk_num].allocated += cur_chunk->size;

            prev_chunk = cur_chunk;
        }

        cur_chunk = next_chunk;
    }

    if (dest_hole_size)
    {
        POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, new_free_chunk);
        if (!new_free_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;

        kaps_jr1_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

        new_free_chunk->type = IX_FREE_CHUNK;
        new_free_chunk->size = dest_hole_size;
        new_free_chunk->start_ix = prev_chunk->start_ix + prev_chunk->size;

        kaps_jr1_ix_mgr_add_to_free_list(mgr, new_free_chunk);
        kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);

        {
            int32_t start_ix = (mgr->max_ix_per_daisy_chain * mgr->daisy_chain_id)
                                                + (dest_ad_blk_num * mgr->ix_boundary_per_ad_blk);
            int32_t num_entries = new_free_chunk->start_ix + dest_hole_size - start_ix;
            if (mgr->num_entries_per_ad_blk != num_entries)
                kaps_jr1_sassert (0);
        }

        if (src_ad_blk_num  == dest_ad_blk_num + 1)
            blk_prev_chunk = new_free_chunk;
    }

    if (cur_blk_hole == mgr->num_entries_per_ad_blk)
        kaps_jr1_sassert(0);

    POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, new_free_chunk);
    if (!new_free_chunk)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

    new_free_chunk->type = IX_FREE_CHUNK;
    new_free_chunk->size = cur_blk_hole;
    new_free_chunk->start_ix = (mgr->max_ix_per_daisy_chain * mgr->daisy_chain_id)
    + (src_ad_blk_num * mgr->ix_boundary_per_ad_blk);

    if (src_ad_blk_num == mgr->cb_blk)
    {
        new_free_chunk->start_ix += mgr->cb_size;
    }

    kaps_jr1_ix_mgr_add_to_free_list(mgr, new_free_chunk);
    kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, blk_prev_chunk, new_free_chunk);

    kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, ix_free_chunk);
    kaps_jr1_ix_mgr_remove_from_free_list(mgr, ix_free_chunk);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_compact_ad_blk(
    struct kaps_jr1_ix_mgr *mgr,
    int32_t ad_blk_num,
    struct kaps_jr1_ix_chunk **free_chunk)
{
    struct kaps_jr1_ix_chunk *cur_chunk, *next_chunk, *prev_chunk;
    struct kaps_jr1_ix_chunk *new_free_chunk, *compaction_start_chunk;
    int32_t free_space;
    int32_t num_ix_in_window, num_empty_slots_in_lsn = 0;
    int32_t cb_start_ix, cb_size, end_of_blk;
    int32_t hole_start_ix, hole_size;
    struct kaps_jr1_ix_chunk *cb_head, *cb_tail;

    kaps_jr1_sassert(mgr);

    if (!mgr->cb_size)
    {
        kaps_jr1_sassert (0);
    }

    compaction_start_chunk = kaps_jr1_ix_mgr_get_start_chunk_for_ad_blk(mgr, ad_blk_num);

    kaps_jr1_sassert(compaction_start_chunk);

    
    cur_chunk = compaction_start_chunk;
    free_space = 0;
    num_ix_in_window = 0;
    cb_start_ix = mgr->cb_start_ix;
    cb_size = mgr->cb_size;
    hole_start_ix = -1;
    hole_size = 0;
    cb_head = NULL;
    cb_tail = NULL;
    end_of_blk = 0;
    prev_chunk = compaction_start_chunk->prev_neighbor;
    while (cur_chunk)
    {

        next_chunk = cur_chunk->next_neighbor;

        if (end_of_blk)
            break;

        if (next_chunk)
        {
            if (cur_chunk->start_ix + cur_chunk->size != cur_chunk->next_neighbor->start_ix)
            {
                end_of_blk = 1;
            }
        }

        num_ix_in_window += cur_chunk->size;

        
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {
            break;
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (hole_start_ix == -1)
            {
                hole_start_ix = cur_chunk->start_ix;
                hole_size = cur_chunk->size;
            }
            else
            {
                hole_size += cur_chunk->size;
            }

            free_space += cur_chunk->size;

            kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_jr1_ix_mgr_remove_from_free_list(mgr, cur_chunk);

            POOL_FREE(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, cur_chunk);

        }
        else
        {
            if (hole_size < cur_chunk->size)
            {
                uint32_t from_start_ix, from_hole_size;

                from_start_ix = cur_chunk->start_ix;
                from_hole_size = cur_chunk->size;

                KAPS_JR1_STRY(kaps_jr1_ix_mgr_move_chunk(mgr, cur_chunk, &cb_start_ix, &num_empty_slots_in_lsn));

                if (hole_start_ix == -1)
                    hole_start_ix = from_start_ix;

                hole_size += from_hole_size;
                free_space += num_empty_slots_in_lsn;
                mgr->ad_blk_info[ad_blk_num].allocated -= num_empty_slots_in_lsn;

                cb_size -= cur_chunk->size;
                kaps_jr1_sassert(cb_size >= 0);

                kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
                kaps_jr1_ix_mgr_add_to_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);

            }
            else
            {
                KAPS_JR1_STRY(kaps_jr1_ix_mgr_move_chunk(mgr, cur_chunk, &hole_start_ix, &num_empty_slots_in_lsn));
                hole_size += num_empty_slots_in_lsn;
                free_space += num_empty_slots_in_lsn;
                mgr->ad_blk_info[ad_blk_num].allocated -= num_empty_slots_in_lsn;

                prev_chunk = cur_chunk;

            }
        }
        cur_chunk = next_chunk;
    }

    
    cur_chunk = cb_head;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        KAPS_JR1_STRY(kaps_jr1_ix_mgr_move_chunk(mgr, cur_chunk, &hole_start_ix, &num_empty_slots_in_lsn));

        hole_size -= cur_chunk->size;

        kaps_jr1_ix_mgr_remove_from_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);
        kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

        prev_chunk = cur_chunk;

        cur_chunk = next_chunk;
    }


    if (hole_size == 0)
        return KAPS_JR1_OK;
    
    POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, new_free_chunk);
    if (!new_free_chunk)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

    new_free_chunk->type = IX_FREE_CHUNK;
    new_free_chunk->size = hole_size;
    new_free_chunk->start_ix = hole_start_ix;

    kaps_jr1_ix_mgr_add_to_free_list(mgr, new_free_chunk);
    kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);


    *free_chunk = new_free_chunk;

    {
        int32_t start_ix = (mgr->max_ix_per_daisy_chain * mgr->daisy_chain_id)
                                            + (ad_blk_num * mgr->ix_boundary_per_ad_blk);
        int32_t num_entries = hole_start_ix + hole_size - start_ix;
        if (mgr->num_entries_per_ad_blk != num_entries)
            kaps_jr1_sassert (0);
    }
    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_compact(struct kaps_jr1_ix_mgr *mgr, int32_t enable_move, int *was_compaction_done)
{
    struct kaps_jr1_ad_blk_info *temp_blk_info = NULL;
    struct kaps_jr1_ix_chunk *ix_free_chunk = NULL;
    int32_t i, iter, num_blks_to_compact, enable_prints = 0;

    if (mgr->device->debug_level == 3)
        enable_prints = 1;

    num_blks_to_compact = 2;

    temp_blk_info = mgr->device->alloc->xcalloc(mgr->device->alloc->cookie,
            mgr->max_num_ad_blks, sizeof(struct kaps_jr1_ad_blk_info));

    if (!temp_blk_info)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_memcpy(temp_blk_info, mgr->ad_blk_info, mgr->max_num_ad_blks * sizeof(struct kaps_jr1_ad_blk_info));

    qsort(temp_blk_info, mgr->cur_num_ad_blks,
                                        sizeof(struct kaps_jr1_ad_blk_info), kaps_jr1_qsort_of_free_space);

    if (enable_prints)
    {
        kaps_jr1_printf("Before \n");
    }

    for (i = 0; i < num_blks_to_compact; i++)
    {
        uint32_t blk_num = temp_blk_info[i].ad_blk_num;
        if (temp_blk_info[i].size == 0)
            continue;

        if (enable_prints)
        {
            kaps_jr1_printf("ad_blk_num :%d, Size :%d, allocated :%d, free:%d\n",
                    blk_num,
                    mgr->ad_blk_info[blk_num].size, mgr->ad_blk_info[blk_num].allocated,
                    mgr->ad_blk_info[blk_num].size - mgr->ad_blk_info[blk_num].allocated);
        }

    }

    if (mgr->ad_blk_info[temp_blk_info[0].ad_blk_num].size)
    {
        KAPS_JR1_STRY(kaps_jr1_ix_mgr_compact_ad_blk(mgr, temp_blk_info[0].ad_blk_num, &ix_free_chunk));

        kaps_jr1_ix_mgr_verify(mgr, 0);
    }

    mgr->ad_blk_info[temp_blk_info[0].ad_blk_num].is_compacted = 1;

    for (iter = 1; iter < num_blks_to_compact; iter++)
    {
        if (enable_move)
        {
            KAPS_JR1_STRY(kaps_jr1_ix_mgr_move_ad_blk(mgr, temp_blk_info[iter].ad_blk_num, temp_blk_info[iter - 1].ad_blk_num,
                    ix_free_chunk));

            kaps_jr1_ix_mgr_verify(mgr, 0);
        }

        if (mgr->ad_blk_info[temp_blk_info[iter].ad_blk_num].size)
        {
            KAPS_JR1_STRY(kaps_jr1_ix_mgr_compact_ad_blk(mgr, temp_blk_info[iter].ad_blk_num,
                    &ix_free_chunk));
            kaps_jr1_ix_mgr_verify(mgr, 0);

            mgr->ad_blk_info[temp_blk_info[iter].ad_blk_num].is_compacted = 1;
            if (enable_prints)
            {
                kaps_jr1_printf("iter:%d, blk_num; %d hole_size : %d\n", iter, temp_blk_info[iter].ad_blk_num,
                        mgr->ad_blk_info[temp_blk_info[iter].ad_blk_num].size -  mgr->ad_blk_info[temp_blk_info[iter].ad_blk_num].allocated);
            }
        }
    }

    if (enable_prints)
    {
        kaps_jr1_printf("After \n");
        for (i = 0; i < num_blks_to_compact; i++)
        {
            uint32_t blk_num = temp_blk_info[i].ad_blk_num;
            if (temp_blk_info[i].size == 0)
                continue;
            kaps_jr1_printf("ad_blk_num :%d, Size :%d, allocated :%d, free:%d\n",
                    blk_num,
                    mgr->ad_blk_info[blk_num].size, mgr->ad_blk_info[blk_num].allocated,
                    mgr->ad_blk_info[blk_num].size - mgr->ad_blk_info[blk_num].allocated);
        }
    }

    mgr->device->alloc->xfree(mgr->device->alloc->cookie, temp_blk_info);

    return KAPS_JR1_OK;
}




kaps_jr1_status
kaps_jr1_ix_mgr_compact_internal(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ad_db *ad_db,
    int32_t * was_compaction_done,
    struct kaps_jr1_ix_compaction_chunk **playback_head_pp)
{
    struct kaps_jr1_ix_chunk *cur_chunk, *next_chunk, *prev_chunk;
    struct kaps_jr1_ix_chunk *new_free_chunk, *compaction_start_chunk;
    int32_t free_space, num_prexies = 0, num_pfx = 0;
    int32_t num_ix_in_window, num_empty_slots_in_lsn = 0;
    int32_t cb_start_ix, cb_size, incr_start_ix = 0;
    int32_t hole_start_ix, hole_size, count;
    struct kaps_jr1_ix_chunk *cb_head, *cb_tail, *tmp_chunk;
    struct kaps_jr1_ix_compaction_chunk *playback_chunk, *playback_head, *playback_tail;
    int32_t has_reserved_slot = 0;      

    kaps_jr1_sassert(ad_db);
    kaps_jr1_sassert(mgr);

    if (!mgr->cb_size || mgr->size <= KAPS_JR1_MAX_IX_INVOLVED_IN_COMPACTION)
    {
        *was_compaction_done = 0;
        return KAPS_JR1_OK;
    }

    

    cur_chunk = mgr->neighbor_list->next_neighbor;
    free_space = 0;
    num_ix_in_window = 0;
    compaction_start_chunk = cur_chunk;

    while (cur_chunk)
    {

        if (cur_chunk->next_neighbor)
        {
            int32_t ad_blk_no1, ad_blk_no2, incr;
            kaps_jr1_ad_get_ad_blk_nr(mgr, cur_chunk->start_ix,
                                           cur_chunk->size, &ad_blk_no1, &incr);

            kaps_jr1_ad_get_ad_blk_nr(mgr, cur_chunk->next_neighbor->start_ix,
                       cur_chunk->next_neighbor->size, &ad_blk_no2, &incr);

            
            if (cur_chunk->start_ix + cur_chunk->size != cur_chunk->next_neighbor->start_ix)
            {
                if (free_space >= 2 * mgr->max_alloc_chunk_size)
                {
                    
                    break;
                }
                else
                {
                    
                    cur_chunk = cur_chunk->next_neighbor;
                    free_space = 0;
                    num_ix_in_window = 0;
                    compaction_start_chunk = cur_chunk;
                    continue;
                }
            }

            
            if ((ad_blk_no2 > ad_blk_no1 + 1) || (ad_blk_no2 < ad_blk_no1))
            {
                if (free_space >= 2 * mgr->max_alloc_chunk_size)
                {
                    
                    break;
                }
                else
                {
                    
                    cur_chunk = cur_chunk->next_neighbor;
                    free_space = 0;
                    num_ix_in_window = 0;
                    compaction_start_chunk = cur_chunk;
                    continue;
                }
            }
        }

        
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {

            if (free_space >= 2 * mgr->max_alloc_chunk_size)
            {
                break;
            }
            else
            {
                cur_chunk = cur_chunk->next_neighbor;
                free_space = 0;
                num_ix_in_window = 0;
                compaction_start_chunk = cur_chunk;
                continue;
            }
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            free_space += cur_chunk->size;
        }
        else if (cur_chunk->type == IX_ALLOCATED_CHUNK)
        {
            
            num_pfx = kaps_jr1_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);

            if (!has_reserved_slot)
            {
                if (cur_chunk->size > num_pfx + mgr->ix_max_holes_allowed)
                    free_space += cur_chunk->size - num_pfx - mgr->ix_max_holes_allowed;
            }
        }

        num_ix_in_window += cur_chunk->size;    

        cur_chunk = cur_chunk->next_neighbor;

        if (num_ix_in_window >= KAPS_JR1_MAX_IX_INVOLVED_IN_COMPACTION)
        {

            if (free_space >= 2 * mgr->max_alloc_chunk_size)
            {
                break;
            }
            else
            {
                num_ix_in_window = 0;
                free_space = 0;
                compaction_start_chunk = cur_chunk;
            }
        }

    }

    if (free_space < 2 * mgr->max_alloc_chunk_size)
    {
        *was_compaction_done = 0;
        return KAPS_JR1_OK;
    }


    tmp_chunk = compaction_start_chunk;
    if (tmp_chunk->prev_neighbor)
    {
        int32_t ad_blk_no1, incr;
        kaps_jr1_ad_get_ad_blk_nr(mgr, tmp_chunk->start_ix,
                      tmp_chunk->size, &ad_blk_no1, &incr);

        if (incr)
        {
            compaction_start_chunk = tmp_chunk->next_neighbor;
        }
    }

    
    cur_chunk = compaction_start_chunk;
    free_space = 0;
    num_ix_in_window = 0;
    cb_start_ix = mgr->cb_start_ix;
    cb_size = mgr->cb_size;
    hole_start_ix = -1;
    hole_size = 0;
    cb_head = NULL;
    cb_tail = NULL;
    playback_head = NULL;
    playback_tail = NULL;
    count = 0;
    prev_chunk = compaction_start_chunk->prev_neighbor;
    while (cur_chunk && num_ix_in_window < KAPS_JR1_MAX_IX_INVOLVED_IN_COMPACTION)
    {

        next_chunk = cur_chunk->next_neighbor;

        if (next_chunk)
        {
            int32_t ad_blk_no1, ad_blk_no2, incr;
            kaps_jr1_ad_get_ad_blk_nr(mgr, cur_chunk->start_ix,
                              cur_chunk->size, &ad_blk_no1, &incr);

            kaps_jr1_ad_get_ad_blk_nr(mgr, next_chunk->start_ix,
                              next_chunk->size, &ad_blk_no2, &incr);

            if (cur_chunk->start_ix + cur_chunk->size != cur_chunk->next_neighbor->start_ix)
            {
                kaps_jr1_sassert(free_space >= 2 * mgr->max_alloc_chunk_size);
                break;
            }

            if ((ad_blk_no2 > ad_blk_no1 + 1) || (ad_blk_no2 < ad_blk_no1))
            {
                kaps_jr1_sassert(free_space >= 2 * mgr->max_alloc_chunk_size);
                break;
            }

        }

        num_ix_in_window += cur_chunk->size;

        
        if (cur_chunk->user_type == IX_USER_OVERFLOW)
        {
            break;
        }

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (hole_start_ix == -1)
            {
                hole_start_ix = cur_chunk->start_ix;
                hole_size = cur_chunk->size;
            }
            else
            {
                hole_size += cur_chunk->size;
            }

            free_space += cur_chunk->size;

            kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
            kaps_jr1_ix_mgr_remove_from_free_list(mgr, cur_chunk);

            POOL_FREE(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, cur_chunk);

        }
        else
        {
            num_prexies += cur_chunk->size;
            if (hole_size < cur_chunk->size)
            {
                
                playback_chunk =
                    mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_jr1_ix_compaction_chunk));

                if (!playback_chunk)
                    return KAPS_JR1_OUT_OF_MEMORY;

                num_pfx = kaps_jr1_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);

                num_empty_slots_in_lsn = 0;

                if (!has_reserved_slot)
                {
                    if (cur_chunk->size > num_pfx + mgr->ix_max_holes_allowed)
                    {
                        num_empty_slots_in_lsn = cur_chunk->size - num_pfx - mgr->ix_max_holes_allowed;
                    }
                }

                playback_chunk->kaps_jr1_ix_chunk = cur_chunk;
                playback_chunk->from_start_ix = cur_chunk->start_ix;
                playback_chunk->from_size = cur_chunk->size;
                playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
                playback_chunk->to_start_ix = cb_start_ix;
                playback_chunk->to_size = cur_chunk->size - num_empty_slots_in_lsn;
                kaps_jr1_ad_get_ad_blk_nr(mgr, playback_chunk->to_start_ix,
                                      playback_chunk->to_size,
                                      &playback_chunk->to_ad_blk_no, &incr_start_ix);
                if (incr_start_ix)
                {
                    kaps_jr1_sassert(0);
                }
                playback_chunk->to_end_ix = cb_start_ix + playback_chunk->to_size - 1;

                kaps_jr1_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

                mgr->num_allocated_ix -= num_empty_slots_in_lsn;

                if (hole_start_ix == -1)
                    hole_start_ix = playback_chunk->from_start_ix;

                hole_size += cur_chunk->size;
                free_space += num_empty_slots_in_lsn;

                cb_start_ix += playback_chunk->to_size;
                cb_size -= playback_chunk->to_size;
                kaps_jr1_sassert(cb_size >= 0);

                cur_chunk->start_ix = playback_chunk->to_start_ix;
                cur_chunk->size = playback_chunk->to_size;

                kaps_jr1_ix_mgr_remove_from_neighbor_list(mgr, cur_chunk);
                kaps_jr1_ix_mgr_add_to_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);

            }
            else
            {
                
                playback_chunk =
                    mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_jr1_ix_compaction_chunk));
                if (!playback_chunk)
                    return KAPS_JR1_OUT_OF_MEMORY;

                num_pfx = kaps_jr1_ix_chunk_get_num_prefixes(cur_chunk->lsn_ptr, cur_chunk, &has_reserved_slot);

                num_empty_slots_in_lsn = 0;

                if (!has_reserved_slot)
                {
                    if (cur_chunk->size > num_pfx + mgr->ix_max_holes_allowed)
                    {
                        num_empty_slots_in_lsn = cur_chunk->size - num_pfx - mgr->ix_max_holes_allowed;
                    }
                }

                playback_chunk->kaps_jr1_ix_chunk = cur_chunk;
                playback_chunk->from_start_ix = cur_chunk->start_ix;
                playback_chunk->from_size = cur_chunk->size;
                playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
                playback_chunk->to_start_ix = hole_start_ix;
                playback_chunk->to_size = cur_chunk->size - num_empty_slots_in_lsn;
                kaps_jr1_ad_get_ad_blk_nr(mgr, playback_chunk->to_start_ix,
                                              playback_chunk->to_size,
                                              &playback_chunk->to_ad_blk_no, &incr_start_ix);

                if (incr_start_ix)
                {
                    kaps_jr1_sassert(0);
                }
                playback_chunk->to_end_ix = hole_start_ix + playback_chunk->to_size - 1;

                kaps_jr1_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

                mgr->num_allocated_ix -= num_empty_slots_in_lsn;

                hole_start_ix += cur_chunk->size - num_empty_slots_in_lsn;
                hole_size += num_empty_slots_in_lsn;
                free_space += num_empty_slots_in_lsn;

                cur_chunk->start_ix = playback_chunk->to_start_ix;
                cur_chunk->size = playback_chunk->to_size;

                prev_chunk = cur_chunk;

            }
        }
        cur_chunk = next_chunk;
        count++;
    }

    
    cur_chunk = cb_head;
    while (cur_chunk)
    {
        next_chunk = cur_chunk->next_neighbor;

        num_prexies += cur_chunk->size;
        playback_chunk =
            mgr->device->alloc->xcalloc(mgr->device->alloc->cookie, 1, sizeof(struct kaps_jr1_ix_compaction_chunk));
        if (!playback_chunk)
            return KAPS_JR1_OUT_OF_MEMORY;

        playback_chunk->kaps_jr1_ix_chunk = cur_chunk;
        playback_chunk->from_start_ix = cur_chunk->start_ix;
        playback_chunk->from_size = cur_chunk->size;
        playback_chunk->from_end_ix = cur_chunk->start_ix + cur_chunk->size - 1;
        playback_chunk->to_start_ix = hole_start_ix;
        playback_chunk->to_size = cur_chunk->size;
        kaps_jr1_ad_get_ad_blk_nr(mgr, playback_chunk->to_start_ix,
                                      playback_chunk->to_size,
                                      &playback_chunk->to_ad_blk_no, &incr_start_ix);
        if (incr_start_ix)
        {
            kaps_jr1_sassert(0);
            playback_chunk->to_start_ix += incr_start_ix;
            hole_start_ix += incr_start_ix;
        }

        playback_chunk->to_end_ix = hole_start_ix + cur_chunk->size - 1;

        kaps_jr1_ix_mgr_add_to_playback_list(mgr, playback_chunk, &playback_head, &playback_tail);

        hole_start_ix += cur_chunk->size;
        hole_size -= cur_chunk->size;

        cur_chunk->start_ix = playback_chunk->to_start_ix;

        kaps_jr1_ix_mgr_remove_from_cb_list(mgr, cur_chunk, &cb_head, &cb_tail);
        kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, cur_chunk);

        prev_chunk = cur_chunk;

        cur_chunk = next_chunk;
        count++;
    }

    
    POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, new_free_chunk);
    if (!new_free_chunk)
        return KAPS_JR1_OUT_OF_MEMORY;

    kaps_jr1_memset(new_free_chunk, 0, sizeof(*new_free_chunk));

    new_free_chunk->type = IX_FREE_CHUNK;
    new_free_chunk->size = hole_size;
    new_free_chunk->start_ix = hole_start_ix;

    kaps_jr1_ix_mgr_add_to_free_list(mgr, new_free_chunk);
    kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, new_free_chunk);

    *playback_head_pp = playback_head;
    *was_compaction_done = 1;

    return KAPS_JR1_OK;
}


struct kaps_jr1_ix_mgr*
kaps_jr1_ix_mgr_get_mgr_for_lmpsofar_pfx(
    struct kaps_jr1_ad_db *ad_db)
{
    struct kaps_jr1_db *db = ad_db->db_info.common_info->ad_info.db;
    struct kaps_jr1_device *device = db->device;
    struct kaps_jr1_ix_mgr *ix_mgr;
    uint32_t daisy_chain_id = 0;

    ix_mgr = ad_db->mgr[device->core_id][daisy_chain_id];

    return ix_mgr;
}



kaps_jr1_status
kaps_jr1_ix_mgr_alloc(
    struct kaps_jr1_ix_mgr *mgr,
    struct kaps_jr1_ad_db *ad_db,
    uint32_t rqt_size,
    void *lsn_ptr,
    enum kaps_jr1_ix_user_type user_type,
    struct kaps_jr1_ix_chunk **alloc_chunk_pp)
{
    kaps_jr1_status status;

    status = kaps_jr1_ix_mgr_alloc_internal(mgr, ad_db, rqt_size, lsn_ptr, user_type, alloc_chunk_pp);

    return status;
}

kaps_jr1_status
kaps_jr1_ix_mgr_wb_alloc(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ad_db * ad_db,
    uint32_t rqt_size,
    uint32_t start_ix,
    void *lsn_ptr,
    enum kaps_jr1_ix_user_type user_type,
    struct kaps_jr1_ix_chunk ** alloc_chunk_pp)
{
    struct kaps_jr1_ix_chunk *chosen_chunk;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;
    struct kaps_jr1_ad_db *cur_ad_db = ad_db;
    int32_t level;
    uint32_t ad_blk_no = -1;

    *alloc_chunk_pp = NULL;

    level = rqt_size - 1;
    if (rqt_size == 0 || level > mgr->max_num_ix_chunk_levels - 1)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INVALID_ARGUMENT;
    }

    if (ad_db && ad_db->db_info.hw_res.ad_res->ad_type == KAPS_JR1_AD_TYPE_INPLACE)
        cur_ad_db = NULL;


    

    POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, chosen_chunk);
    if (!chosen_chunk)
        return KAPS_JR1_OUT_OF_MEMORY;
    kaps_jr1_memset(chosen_chunk, 0, sizeof(*chosen_chunk));

    chosen_chunk->type = IX_ALLOCATED_CHUNK;
    chosen_chunk->next_free_chunk = NULL;
    chosen_chunk->prev_free_chunk = NULL;
    chosen_chunk->size = rqt_size;
    chosen_chunk->start_ix = start_ix;
    chosen_chunk->next_neighbor = NULL;
    chosen_chunk->prev_neighbor = NULL;
    chosen_chunk->lsn_ptr = lsn_ptr;
    chosen_chunk->user_type = user_type;

    cur_mgr->num_allocated_ix += rqt_size;

    kaps_jr1_ix_mgr_add_to_neighbor_list(cur_mgr, NULL, chosen_chunk);

    if (ad_db)
    {
        chosen_chunk->ad_info =
            ad_db->db_info.device->alloc->xcalloc(ad_db->db_info.device->alloc->cookie, 1,
                                                  sizeof(struct kaps_jr1_ad_chunk));
        if (!chosen_chunk->ad_info)
            return KAPS_JR1_OUT_OF_MEMORY;
        chosen_chunk->ad_info->device = ad_db->db_info.device;
        chosen_chunk->ad_info->ad_db = ad_db;
    }
    if (cur_ad_db)
    {
        ad_blk_no =
            kaps_jr1_ad_get_ad_blk_for_chunk(cur_mgr, chosen_chunk->start_ix, chosen_chunk->size, rqt_size,
                                     cur_mgr->ix_translate);
        kaps_jr1_sassert(ad_blk_no != -1);
        chosen_chunk->ad_info->ad_blk_no = ad_blk_no;
        chosen_chunk->ad_info->offset_in_ad_blk = chosen_chunk->start_ix - cur_mgr->ix_translate[ad_blk_no];
    }

    *alloc_chunk_pp = chosen_chunk;

    return KAPS_JR1_OK;

}

kaps_jr1_status
kaps_jr1_ix_mgr_wb_finalize(
    struct kaps_jr1_ix_mgr * mgr)
{
    struct kaps_jr1_ix_chunk *iter_chunk, *alloc_chunk, *prev_chunk;
    uint32_t num_pfx = 0;
    uint32_t ix_seg_no = 0;
    uint32_t temp_start_ix, temp_end_ix;
    int32_t has_reserved_slot = 0;

    if (!mgr->neighbor_list)
    {

        struct kaps_jr1_ix_chunk *first_free_chunk = NULL;

        if (mgr->is_finalized)
            return KAPS_JR1_OK;

        if (mgr->size > 0 && mgr->end_ix - mgr->start_ix + 1)
        {
            POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, first_free_chunk);
            if (!first_free_chunk)
                return KAPS_JR1_OUT_OF_MEMORY;
            kaps_jr1_memset(first_free_chunk, 0, sizeof(*first_free_chunk));
            first_free_chunk->start_ix = mgr->start_ix;
            first_free_chunk->size = mgr->end_ix - mgr->start_ix + 1;
            first_free_chunk->type = IX_FREE_CHUNK;
            kaps_jr1_ix_mgr_add_to_free_list(mgr, first_free_chunk);
            mgr->neighbor_list = first_free_chunk;
            mgr->neighbor_list_end = first_free_chunk;
            mgr->is_finalized = 1;

        }

        return KAPS_JR1_OK;
    }

    if (mgr->is_finalized)
        return KAPS_JR1_OK;

    mgr->neighbor_list = kaps_jr1_merge_sort(mgr->neighbor_list);

    iter_chunk = mgr->neighbor_list;
    temp_start_ix = mgr->start_ix;
    temp_end_ix = mgr->end_ix;
    prev_chunk = NULL;

    
    while (mgr->ix_segments_start[ix_seg_no] != -1)
    {
        mgr->start_ix = mgr->ix_segments_start[ix_seg_no];
        mgr->end_ix = mgr->ix_segments_end[ix_seg_no];
        if (ix_seg_no == 0)
            mgr->start_ix += mgr->cb_size;

        if (!iter_chunk || iter_chunk->start_ix > mgr->end_ix)
        {
            POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
            if (!alloc_chunk)
                return KAPS_JR1_OUT_OF_MEMORY;
            kaps_jr1_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

            alloc_chunk->type = IX_FREE_CHUNK;
            alloc_chunk->next_free_chunk = NULL;
            alloc_chunk->prev_free_chunk = NULL;
            alloc_chunk->start_ix = mgr->start_ix;
            alloc_chunk->size = mgr->end_ix - mgr->start_ix + 1;
            alloc_chunk->next_neighbor = NULL;
            alloc_chunk->prev_neighbor = NULL;

            kaps_jr1_ix_mgr_add_to_free_list(mgr, alloc_chunk);
            kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, alloc_chunk);
            prev_chunk = alloc_chunk;
            ix_seg_no++;
            continue;
        }

        if (iter_chunk->start_ix != mgr->start_ix)
        {
            POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
            if (!alloc_chunk)
                return KAPS_JR1_OUT_OF_MEMORY;
            kaps_jr1_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

            alloc_chunk->type = IX_FREE_CHUNK;
            alloc_chunk->next_free_chunk = NULL;
            alloc_chunk->prev_free_chunk = NULL;
            alloc_chunk->start_ix = mgr->start_ix;
            alloc_chunk->size = iter_chunk->start_ix - mgr->start_ix;
            alloc_chunk->next_neighbor = NULL;
            alloc_chunk->prev_neighbor = NULL;

            kaps_jr1_ix_mgr_add_to_free_list(mgr, alloc_chunk);
            kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, prev_chunk, alloc_chunk);
            prev_chunk = alloc_chunk;
        }

        while (iter_chunk && (iter_chunk->start_ix <= mgr->end_ix))
        {
            if (iter_chunk->next_neighbor && (iter_chunk->next_neighbor->start_ix <= mgr->end_ix))
            {
                if ((iter_chunk->start_ix + iter_chunk->size) < iter_chunk->next_neighbor->start_ix)
                {

                    POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
                    if (!alloc_chunk)
                        return KAPS_JR1_OUT_OF_MEMORY;
                    kaps_jr1_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

                    alloc_chunk->type = IX_FREE_CHUNK;
                    alloc_chunk->next_free_chunk = NULL;
                    alloc_chunk->prev_free_chunk = NULL;
                    alloc_chunk->start_ix = iter_chunk->start_ix + iter_chunk->size;
                    alloc_chunk->size = iter_chunk->next_neighbor->start_ix - alloc_chunk->start_ix;
                    alloc_chunk->next_neighbor = NULL;
                    alloc_chunk->prev_neighbor = NULL;

                    kaps_jr1_ix_mgr_add_to_free_list(mgr, alloc_chunk);
                    kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, iter_chunk, alloc_chunk);

                }
                else if ((iter_chunk->start_ix + iter_chunk->size) > iter_chunk->next_neighbor->start_ix)
                {
                    num_pfx = kaps_jr1_ix_chunk_get_num_prefixes(iter_chunk->lsn_ptr, iter_chunk, &has_reserved_slot);
                    if ((iter_chunk->start_ix + num_pfx) > iter_chunk->next_neighbor->start_ix)
                    {
                        kaps_jr1_printf("\n Error: Overlapping Index Chunks \n");
                        return KAPS_JR1_INTERNAL_ERROR;
                    }
                    mgr->num_allocated_ix -= iter_chunk->size;
                    iter_chunk->size = iter_chunk->next_neighbor->start_ix - iter_chunk->start_ix;
                    mgr->num_allocated_ix += iter_chunk->size;
                }
            }
            else if (iter_chunk->start_ix + iter_chunk->size != (mgr->end_ix + 1))
            {

                POOL_ALLOC(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, alloc_chunk);
                if (!alloc_chunk)
                    return KAPS_JR1_OUT_OF_MEMORY;
                kaps_jr1_memset(alloc_chunk, 0, sizeof(*alloc_chunk));

                alloc_chunk->type = IX_FREE_CHUNK;
                alloc_chunk->next_free_chunk = NULL;
                alloc_chunk->prev_free_chunk = NULL;
                alloc_chunk->start_ix = iter_chunk->start_ix + iter_chunk->size;
                alloc_chunk->size = mgr->end_ix - alloc_chunk->start_ix + 1;
                alloc_chunk->next_neighbor = NULL;
                alloc_chunk->prev_neighbor = NULL;

                kaps_jr1_ix_mgr_add_to_free_list(mgr, alloc_chunk);
                kaps_jr1_ix_mgr_add_to_neighbor_list(mgr, iter_chunk, alloc_chunk);

            }
            prev_chunk = iter_chunk;
            iter_chunk = iter_chunk->next_neighbor;
        }
        ix_seg_no++;
    }
    mgr->start_ix = temp_start_ix;
    mgr->end_ix = temp_end_ix;

    if (mgr->next)
        KAPS_JR1_STRY(kaps_jr1_ix_mgr_wb_finalize(mgr->next));

    mgr->is_finalized = 1;

    
    KAPS_JR1_TRY(kaps_jr1_ix_mgr_verify(mgr, 1));

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_wb_finalize_for_all_ad_dbs(
    struct kaps_jr1_device *device,
    struct kaps_jr1_ad_db *ad_db)
{
    uint32_t i;

    while (ad_db)
    {
        for (i = 0; i < device->hw_res->num_daisy_chains; ++i)
        {
            KAPS_JR1_STRY(kaps_jr1_ix_mgr_wb_finalize(ad_db->mgr[device->core_id][i]));
        }

        ad_db = ad_db->next;
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_check_grow_up(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_chunk * cur_ix_chunk,
    uint32_t * grow_up_size_p)
{
    struct kaps_jr1_ix_chunk *prev_neighbor = cur_ix_chunk->prev_neighbor;
    uint32_t ad_grow = 1;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;

    *grow_up_size_p = 0;


    if (prev_neighbor && prev_neighbor->type == IX_FREE_CHUNK)
    {
        
        if (cur_ix_chunk->ad_info )
        {
            ad_grow = kaps_jr1_check_ad_grow(cur_mgr, prev_neighbor->start_ix, cur_ix_chunk->start_ix,
                                         cur_mgr->ix_translate);

        }
        if (prev_neighbor->start_ix + prev_neighbor->size == cur_ix_chunk->start_ix && ad_grow)
        {
            *grow_up_size_p = prev_neighbor->size;
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_grow_up(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_chunk * cur_ix_chunk,
    uint32_t grow_up_size)
{
    struct kaps_jr1_ix_chunk *prev_neighbor = cur_ix_chunk->prev_neighbor;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;


    if (!prev_neighbor || prev_neighbor->type != IX_FREE_CHUNK || prev_neighbor->size < grow_up_size)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INTERNAL_ERROR;
    }

    cur_ix_chunk->size += grow_up_size;

    cur_ix_chunk->start_ix -= grow_up_size;

    kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, prev_neighbor);
    prev_neighbor->size -= grow_up_size;

    if (prev_neighbor->size > 0)
    {
        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, prev_neighbor);
    }
    else
    {
        kaps_jr1_ix_mgr_remove_from_neighbor_list(cur_mgr, prev_neighbor);
        POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, prev_neighbor);
    }

    cur_mgr->num_allocated_ix += grow_up_size;
    kaps_jr1_sassert(cur_mgr->num_allocated_ix <= cur_mgr->size);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_check_grow_down(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_chunk * cur_ix_chunk,
    uint32_t * grow_down_size_p)
{
    struct kaps_jr1_ix_chunk *next_neighbor = cur_ix_chunk->next_neighbor;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;
    int32_t ad_grow = 1;

    *grow_down_size_p = 0;


    if (next_neighbor && next_neighbor->type == IX_FREE_CHUNK)
    {
        
        if (cur_ix_chunk->ad_info)
        {
            ad_grow = kaps_jr1_check_ad_grow(cur_mgr, cur_ix_chunk->start_ix, next_neighbor->start_ix,
                                         cur_mgr->ix_translate);
        }
        if (cur_ix_chunk->start_ix + cur_ix_chunk->size == next_neighbor->start_ix && ad_grow )
        {
            *grow_down_size_p = next_neighbor->size;
        }
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_grow_down(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_chunk * cur_ix_chunk,
    uint32_t grow_down_size)
{
    struct kaps_jr1_ix_chunk *next_neighbor = cur_ix_chunk->next_neighbor;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;


    if (!next_neighbor || next_neighbor->type != IX_FREE_CHUNK || next_neighbor->size < grow_down_size)
    {
        kaps_jr1_sassert(0);
        return KAPS_JR1_INTERNAL_ERROR;
    }

    cur_ix_chunk->size += grow_down_size;

    kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, next_neighbor);
    next_neighbor->size -= grow_down_size;

    if (next_neighbor->size > 0)
    {
        next_neighbor->start_ix += grow_down_size;
        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, next_neighbor);
    }
    else
    {
        kaps_jr1_ix_mgr_remove_from_neighbor_list(cur_mgr, next_neighbor);
        POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, next_neighbor);
    }

    cur_mgr->num_allocated_ix += grow_down_size;
    kaps_jr1_sassert(cur_mgr->num_allocated_ix <= cur_mgr->size);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_free(
    struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_chunk * released_chunk)
{
    struct kaps_jr1_ix_chunk *prev_neighbor = NULL, *next_neighbor = NULL;
    int32_t can_merge_prev_neighbor = 0, can_merge_next_neighbor = 0, ad_blk_num = -1;
    struct kaps_jr1_ix_mgr *cur_mgr = mgr;
    enum kaps_jr1_ad_type ad_type = KAPS_JR1_AD_TYPE_INPLACE;

    if (!released_chunk)
        return KAPS_JR1_INVALID_ARGUMENT;

    if (released_chunk->ad_info)
    {
        ad_type = released_chunk->ad_info->ad_db->db_info.hw_res.ad_res->ad_type;
    }

    if (ad_type == KAPS_JR1_AD_TYPE_INDEX && released_chunk->ad_info)
    {
        ad_blk_num = released_chunk->ad_info->ad_blk_no;
        mgr->ad_blk_info[ad_blk_num].allocated -= released_chunk->size;
    }

    released_chunk->lsn_ptr = NULL;

    prev_neighbor = released_chunk->prev_neighbor;
    next_neighbor = released_chunk->next_neighbor;

    if (prev_neighbor && prev_neighbor->type == IX_FREE_CHUNK)
    {
        if (prev_neighbor->start_ix + prev_neighbor->size == released_chunk->start_ix)
            can_merge_prev_neighbor = 1;

        if (can_merge_prev_neighbor && released_chunk->ad_info && released_chunk->ad_info->ad_db->user_width_1 != 0)
        {
            can_merge_prev_neighbor = kaps_jr1_check_ad_grow(cur_mgr, prev_neighbor->start_ix, released_chunk->start_ix,
                                                         cur_mgr->ix_translate);
        }
    }

    if (next_neighbor && next_neighbor->type == IX_FREE_CHUNK)
    {
        if (next_neighbor->start_ix == released_chunk->start_ix + released_chunk->size)
            can_merge_next_neighbor = 1;

        if (can_merge_next_neighbor && released_chunk->ad_info && released_chunk->ad_info->ad_db->user_width_1 != 0)
        {
            can_merge_next_neighbor = kaps_jr1_check_ad_grow(cur_mgr, released_chunk->start_ix, next_neighbor->start_ix,
                                                         cur_mgr->ix_translate);
        }
    }

    if (released_chunk->ad_info)
        released_chunk->ad_info->ad_db->db_info.device->alloc->xfree(released_chunk->ad_info->ad_db->db_info.
                                                                     device->alloc->cookie, released_chunk->ad_info);

    cur_mgr->num_allocated_ix -= released_chunk->size;

    if (can_merge_prev_neighbor && can_merge_next_neighbor)
    {
        kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, prev_neighbor);
        kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, next_neighbor);

        prev_neighbor->size += released_chunk->size + next_neighbor->size;

        kaps_jr1_ix_mgr_remove_from_neighbor_list(cur_mgr, next_neighbor);
        kaps_jr1_ix_mgr_remove_from_neighbor_list(cur_mgr, released_chunk);

        POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, next_neighbor);
        POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, released_chunk);

        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, prev_neighbor);

    }
    else if (can_merge_prev_neighbor)
    {
        kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, prev_neighbor);

        prev_neighbor->size += released_chunk->size;

        kaps_jr1_ix_mgr_remove_from_neighbor_list(cur_mgr, released_chunk);
        POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, released_chunk);

        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, prev_neighbor);

    }
    else if (can_merge_next_neighbor)
    {
        kaps_jr1_ix_mgr_remove_from_free_list(cur_mgr, next_neighbor);

        released_chunk->size += next_neighbor->size;
        released_chunk->type = IX_FREE_CHUNK;

        kaps_jr1_ix_mgr_remove_from_neighbor_list(cur_mgr, next_neighbor);
        POOL_FREE(kaps_jr1_ix_chunk, &cur_mgr->ix_chunk_pool, next_neighbor);

        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, released_chunk);

    }
    else
    {
        released_chunk->type = IX_FREE_CHUNK;
        kaps_jr1_ix_mgr_add_to_free_list(cur_mgr, released_chunk);
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_destroy(
    struct kaps_jr1_ix_mgr * mgr)
{
    struct kaps_jr1_ix_chunk *tmp, *next;
    struct kaps_jr1_allocator *alloc = NULL;

    if (!mgr)
        return KAPS_JR1_INVALID_ARGUMENT;

    alloc = mgr->ix_chunk_pool.allocator;

    tmp = mgr->neighbor_list;
    while (tmp)
    {
        next = tmp->next_neighbor;
        POOL_FREE(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool, tmp);
        tmp = next;
    }
    POOL_FINI(kaps_jr1_ix_chunk, &mgr->ix_chunk_pool);

    if (mgr->free_list)
        alloc->xfree(alloc->cookie, mgr->free_list);

    if (mgr->num_ix_in_free_list)
        alloc->xfree(alloc->cookie, mgr->num_ix_in_free_list);

    if (mgr->ix_translate)
        alloc->xfree(alloc->cookie, mgr->ix_translate);

    if (mgr->ix_segments_start)
        alloc->xfree(alloc->cookie, mgr->ix_segments_start);

    if (mgr->ix_segments_end)
        alloc->xfree(alloc->cookie, mgr->ix_segments_end);

    if (mgr->ad_blk_info)
        alloc->xfree(alloc->cookie, mgr->ad_blk_info);

    alloc->xfree(alloc->cookie, mgr);
    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_destroy_for_all_ad_dbs(
    struct kaps_jr1_device *device,
    struct kaps_jr1_db *db)
{
    uint32_t i;
    struct kaps_jr1_ad_db *cur_ad_db, *next_ad_db;
    struct kaps_jr1_ix_mgr *cur_ix_mgr;

    cur_ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;

    while (cur_ad_db)
    {
        for (i = 0; i < device->hw_res->num_daisy_chains; ++i)
        {
            cur_ix_mgr = cur_ad_db->mgr[device->core_id][i];

            if (cur_ix_mgr)
            {
                KAPS_JR1_STRY(kaps_jr1_ix_mgr_destroy(cur_ix_mgr));

                
                next_ad_db = cur_ad_db->next;

                while (next_ad_db)
                {
                    if (next_ad_db->mgr[device->core_id][i] == cur_ix_mgr)
                    {
                        next_ad_db->mgr[device->core_id][i] = NULL;
                    }

                    next_ad_db = next_ad_db->next;
                }

                cur_ad_db->mgr[device->core_id][i] = NULL;
            }
        }

        cur_ad_db = cur_ad_db->next;
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_calc_stats(
    const struct kaps_jr1_ix_mgr * mgr,
    struct kaps_jr1_ix_mgr_stats * ix_stats)
{
    const struct kaps_jr1_ix_mgr *tmp = mgr;
    struct kaps_jr1_ad_db *ad_db = tmp->main_ad_db;

    (void) ad_db;

    ix_stats->total_num_allocated_ix = 0;
    ix_stats->total_num_free_ix = 0;
    for (; tmp; tmp = tmp->next)
    {
        ix_stats->total_num_allocated_ix += tmp->num_allocated_ix;
        ix_stats->total_num_free_ix += (tmp->size - tmp->num_allocated_ix);
    }

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_print_stats(
    struct kaps_jr1_ix_mgr_stats * ix_stats)
{
    uint32_t total_ix = ix_stats->total_num_allocated_ix + ix_stats->total_num_free_ix;
    float percent_allocated_ix = ix_stats->total_num_allocated_ix * 100.0F / total_ix;
    float percent_free_ix = ix_stats->total_num_free_ix * 100.0F / total_ix;

    kaps_jr1_printf("\n\nTotal number of allocated indexes = %d  (%.2f%%) \n",
                ix_stats->total_num_allocated_ix, percent_allocated_ix);

    kaps_jr1_printf("Total number of free indexes = %d (%.2f%%) \n\n", ix_stats->total_num_free_ix, percent_free_ix);

    return KAPS_JR1_OK;
}



kaps_jr1_status
kaps_jr1_ix_mgr_verify(
    struct kaps_jr1_ix_mgr * mgr,
    uint32_t recalc_neighbor_list_end)
{
    uint32_t level = 0;
    struct kaps_jr1_ix_chunk *cur_chunk = mgr->neighbor_list, *prev_chunk = NULL, *next_chunk = NULL;
    uint32_t total_free_ix_1, total_free_ix_2, total_alloc_ix;
    int32_t cur_level_num_free_ix, cur_blk_num = -1, cur_blk_allocated = 0;
    uint32_t check_if_chunk_in_non_neighbor_ad_blk = 1;     


    (void) check_if_chunk_in_non_neighbor_ad_blk;
    (void) cur_blk_allocated;
    (void) cur_blk_num;

    total_free_ix_1 = 0;
    total_alloc_ix = 0;
    while (cur_chunk)
    {

        if (cur_chunk->type == IX_FREE_CHUNK)
        {
            if (cur_chunk->next_free_chunk == NULL && cur_chunk->prev_free_chunk == NULL)
            {

                level = cur_chunk->size - 1;
                if (level >= mgr->max_num_ix_chunk_levels)
                    level = mgr->max_num_ix_chunk_levels - 1;

                if (mgr->free_list[level] != cur_chunk)
                {
                    kaps_jr1_printf("Incorrect arrangement in sorted neighbor list \n");
                    kaps_jr1_handle_error();
                    return KAPS_JR1_INTERNAL_ERROR;
                }
            }
            total_free_ix_1 += cur_chunk->size;
        }

        if (cur_chunk->type == IX_ALLOCATED_CHUNK)
        {
            if (cur_chunk->next_free_chunk || cur_chunk->prev_free_chunk)
            {
                kaps_jr1_printf("Incorrect arrangement in sorted neighbor list \n");
                kaps_jr1_handle_error();
                return KAPS_JR1_INTERNAL_ERROR;
            }
            total_alloc_ix += cur_chunk->size;
        }

        
        next_chunk = cur_chunk->next_neighbor;
        if (next_chunk)
        {
            if (cur_chunk->start_ix + cur_chunk->size == next_chunk->start_ix)
            {
                
            }
            else
            {
                uint32_t ad_blk1, ad_blk2;

                if (mgr->num_entries_per_ad_blk == mgr->ix_boundary_per_ad_blk)
                {
                    kaps_jr1_printf("Neighboring IX chunks are not continuous \n");
                    kaps_jr1_handle_error();
                    return KAPS_JR1_INTERNAL_ERROR;
                }

                ad_blk1 = cur_chunk->start_ix / mgr->ix_boundary_per_ad_blk;
                ad_blk2 = next_chunk->start_ix / mgr->ix_boundary_per_ad_blk;

                if (ad_blk1 == ad_blk2)
                {
                    kaps_jr1_printf("Neighboring IX chunks in the same AD Blk are not continuous \n");
                    kaps_jr1_handle_error();
                    return KAPS_JR1_INTERNAL_ERROR;
                }
            }
        }

        prev_chunk = cur_chunk;
        cur_chunk = cur_chunk->next_neighbor;
    }

    if (recalc_neighbor_list_end)
    {
        mgr->neighbor_list_end = prev_chunk;
    }
    else
    {
        if (prev_chunk != mgr->neighbor_list_end)
        {
            kaps_jr1_handle_error();
            return KAPS_JR1_INTERNAL_ERROR;
        }
    }


    total_free_ix_2 = 0;
    for (level = 0; level < mgr->max_num_ix_chunk_levels; ++level)
    {
        cur_level_num_free_ix = 0;
        cur_chunk = mgr->free_list[level];

        while (cur_chunk)
        {
            if (level == mgr->max_num_ix_chunk_levels - 1)
            {
                if (cur_chunk->size - 1 < level)
                {
                    kaps_jr1_printf("Error in array of lists \n");
                    kaps_jr1_handle_error();
                    return KAPS_JR1_INTERNAL_ERROR;
                }
            }
            else
            {
                if (cur_chunk->size - 1 != level)
                {
                    kaps_jr1_printf("Error in array of lists \n");
                    kaps_jr1_handle_error();
                    return KAPS_JR1_INTERNAL_ERROR;
                }
            }

            cur_level_num_free_ix += cur_chunk->size;
            total_free_ix_2 += cur_chunk->size;
            cur_chunk = cur_chunk->next_free_chunk;
        }

        if (cur_level_num_free_ix != mgr->num_ix_in_free_list[level])
        {
            kaps_jr1_printf("Error in the number of IX in the free list\n");
            kaps_jr1_handle_error();
            return KAPS_JR1_INTERNAL_ERROR;
        }

    }

    if (total_free_ix_1 != total_free_ix_2)
    {
        kaps_jr1_printf("Error in the total free size between neighbor list and free list nodes\n");
        kaps_jr1_handle_error();
        return KAPS_JR1_INTERNAL_ERROR;
    }

    if (total_alloc_ix != mgr->num_allocated_ix)
    {
        kaps_jr1_printf("Error in the total alloc size\n");
        kaps_jr1_handle_error();
        return KAPS_JR1_INTERNAL_ERROR;
    }

    if (total_alloc_ix + total_free_ix_1 + mgr->cb_size != mgr->size)
    {
        kaps_jr1_printf("Some of the indexes in the IX Manager are missing\n");
        kaps_jr1_handle_error();
        return KAPS_JR1_INTERNAL_ERROR;
    }

    return KAPS_JR1_OK;

}


kaps_jr1_status
kaps_jr1_ix_mgr_verify_for_all_ad_dbs(
    struct kaps_jr1_device * device,
    struct kaps_jr1_db * db)
{
    uint32_t i;
    struct kaps_jr1_ad_db *ad_db;

    ad_db = (struct kaps_jr1_ad_db*) db->common_info->ad_info.ad;

    while (ad_db)
    {
        for (i = 0; i < device->hw_res->num_daisy_chains; ++i)
        {
            KAPS_JR1_STRY(kaps_jr1_ix_mgr_verify(ad_db->mgr[device->core_id][i], 0 ));
        }

        ad_db = ad_db->next;
    }

    return KAPS_JR1_OK;
}


kaps_jr1_status
kaps_jr1_ix_mgr_wb_save_ix_translate(
    struct kaps_jr1_ix_mgr * mgr,
    kaps_jr1_device_issu_write_fn write_fn,
    void *handle,
    uint32_t * nv_offset)
{
    if (0 != write_fn(handle, (uint8_t *) & mgr->ix_translate, sizeof(mgr->ix_translate), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_translate);


    if (0 != write_fn(handle, (uint8_t *) & mgr->cb_start_ix, sizeof(mgr->cb_start_ix), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_start_ix);
    if (0 != write_fn(handle, (uint8_t *) & mgr->cb_end_ix, sizeof(mgr->cb_end_ix), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_end_ix);
    if (0 != write_fn(handle, (uint8_t *) & mgr->cb_size, sizeof(mgr->cb_size), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_size);


    if (0 != write_fn(handle, (uint8_t *) & mgr->size, sizeof(mgr->size), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->size);

    if (mgr->next)
        return kaps_jr1_ix_mgr_wb_save_ix_translate(mgr->next, write_fn, handle, nv_offset);

    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_ix_mgr_wb_restore_ix_translate(
    struct kaps_jr1_ix_mgr * mgr,
    kaps_jr1_device_issu_write_fn read_fn,
    void *handle,
    uint32_t * nv_offset)
{

    if (0 != read_fn(handle, (uint8_t *) & mgr->ix_translate, sizeof(mgr->ix_translate), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->ix_translate);


    if (0 != read_fn(handle, (uint8_t *) & mgr->cb_start_ix, sizeof(mgr->cb_start_ix), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_start_ix);
    if (0 != read_fn(handle, (uint8_t *) & mgr->cb_end_ix, sizeof(mgr->cb_end_ix), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_end_ix);
    if (0 != read_fn(handle, (uint8_t *) & mgr->cb_size, sizeof(mgr->cb_size), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->cb_size);


    if (0 != read_fn(handle, (uint8_t *) & mgr->size, sizeof(mgr->size), *nv_offset))
        return KAPS_JR1_NV_READ_WRITE_FAILED;
    *nv_offset += sizeof(mgr->size);

    if (mgr->next)
        return kaps_jr1_ix_mgr_wb_restore_ix_translate(mgr->next, read_fn, handle, nv_offset);

    return KAPS_JR1_OK;
}

