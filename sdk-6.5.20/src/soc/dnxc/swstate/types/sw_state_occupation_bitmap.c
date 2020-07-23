/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */



#include <soc/dnxc/swstate/sw_state_features.h>


#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_OCCBMDNX




#include <sal/core/libc.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/types/sw_state_occupation_bitmap.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>






#define OCC_TEST_IF_ERR_EXIT(expression) \
    do { \
        if (SHR_FAILURE(expression)) goto exit; \
    } while(0)

















const static uint8 sw_state_bitmap_val_to_first_one[SW_STATE_OCC_BM_ARRAY_MAP_LEN] = {
    SW_STATE_OCC_BM_NODE_IS_FULL,
    0,
    1, 0,
    2, 0, 1, 0,
    3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1,
    0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1,
    0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0,
    1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2,
    0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};


const static uint8 sw_state_bitmap_val_to_first_zero[SW_STATE_OCC_BM_ARRAY_MAP_LEN] = {
    0,
    1, 0,
    2, 0, 1, 0,
    3, 0, 1, 0, 2, 0, 1, 0,
    4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1,
    0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1,
    0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0,
    1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2,
    0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    SW_STATE_OCC_BM_NODE_IS_FULL
};


const static uint8 sw_state_bitmap_val_to_last_zero[SW_STATE_OCC_BM_ARRAY_MAP_LEN] = {
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    3, 3, 3, 3, 3, 3, 3, 3,
    2, 2, 2, 2,
    1, 1,
    0,
    SW_STATE_OCC_BM_NODE_IS_FULL
};





shr_error_e
sw_state_occ_bm_init_info_clear(
    sw_state_occ_bitmap_init_info_t * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_INTERNAL, "info");

    sal_memset(info, 0x0, sizeof(sw_state_occ_bitmap_init_info_t));
    info->size = 0;
    info->init_val = FALSE;
    info->support_cache = FALSE;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_create(
    int unit,
    uint32 module_id,
    sw_state_occ_bitmap_init_info_t * init_info,
    sw_state_occ_bm_t * bit_map_ptr,
    uint32 occ_size,
    uint32 alloc_flags)
{
    uint32 cur_size, level_size, all_level_size, nof_levels, size;
    uint8 init_val;
    uint32 indx, buffer_offset;
    uint8 _true = TRUE;
    uint8 _false = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map_ptr, _SHR_E_INTERNAL, "bit_map_ptr");
    SHR_NULL_CHECK(init_info, _SHR_E_INTERNAL, "init_info");

    if (*bit_map_ptr != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state occ bmp create ERROR: occ bmp already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }
    
    DNX_SW_STATE_ALLOC(unit, module_id, *bit_map_ptr, **bit_map_ptr,  1, DNXC_SW_STATE_NO_FLAGS, "sw_state occ bitmap");
    
    
    if (occ_size != 0)
    {
        size = occ_size;
    }
    else
    {
        size = init_info->size;
    }

    if (init_info->init_val == FALSE)
    {
        init_val = 0X0;
    }
    else
    {
        init_val = 0XFF;
    }

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id, &((*bit_map_ptr)->init_val),
            &init_info->init_val,
            sizeof(init_info->init_val),
            DNXC_SW_STATE_NO_FLAGS,
            "init_val");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &((*bit_map_ptr)->cache_enabled),
            &_false,
            sizeof(_false),
            DNXC_SW_STATE_NO_FLAGS,
            "cache_enabled");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &((*bit_map_ptr)->support_cache),
            &init_info->support_cache,
            sizeof(init_info->support_cache),
            DNXC_SW_STATE_NO_FLAGS,
            "support_cache");

    cur_size = UTILEX_NOF_BITS_IN_CHAR; 
    nof_levels = 1;
    level_size = UTILEX_NOF_BITS_IN_CHAR;       
    
    while (cur_size < size)
    {
        cur_size *= UTILEX_NOF_BITS_IN_CHAR;
        ++nof_levels;
    }
    
    DNX_SW_STATE_ALLOC(unit, module_id, (*bit_map_ptr)->levels, uint32, nof_levels, alloc_flags, "occ_bitmap levels array");

    if (init_info->support_cache)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &((*bit_map_ptr)->support_cache),
                &_true,
                sizeof(_true),
                DNXC_SW_STATE_NO_FLAGS,
                "support_cache");

        DNX_SW_STATE_ALLOC(unit, module_id, (*bit_map_ptr)->levels_cache, uint32, nof_levels, alloc_flags, "occ_bitmap levels_cache array");
    }

    DNX_SW_STATE_ALLOC(unit, module_id, (*bit_map_ptr)->levels_size, uint32, nof_levels, alloc_flags, "occ_bitmap levels_size array");

    
    all_level_size = 0;
    level_size = size;
    for (indx = nof_levels; indx > 0; --indx)
    {
        level_size = (level_size + (UTILEX_NOF_BITS_IN_CHAR - 1)) / UTILEX_NOF_BITS_IN_CHAR;    

        all_level_size += level_size;
    }
    if (init_info->support_cache)
    {
         DNX_SW_STATE_ALLOC(unit, module_id, (*bit_map_ptr)->levels_cache_buffer, uint8, all_level_size, alloc_flags, "occ_bitmap levels_cache_buffer");
    }
    DNX_SW_STATE_ALLOC(unit, module_id, (*bit_map_ptr)->levels_buffer, uint8, all_level_size, alloc_flags, "occ_bitmap levels_buffer");

    
    if (init_val)
    {
        DNX_SW_STATE_MEMSET(unit, module_id, (*bit_map_ptr)->levels_buffer, 0, init_val, all_level_size, 0, "levels_buffer memset");
    }

    
    level_size = size;
    buffer_offset = 0;
    for (indx = nof_levels; indx > 0; --indx)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &((*bit_map_ptr)->levels_size[indx - 1]),
                &level_size,
                sizeof(level_size),
                DNXC_SW_STATE_NO_FLAGS,
                "levels_size");

        level_size = (level_size + (UTILEX_NOF_BITS_IN_CHAR - 1)) / UTILEX_NOF_BITS_IN_CHAR;    
        if (init_info->support_cache)
        {
            
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &((*bit_map_ptr)->levels_cache[indx - 1]),
                    &buffer_offset,
                    sizeof(buffer_offset),
                    DNXC_SW_STATE_NO_FLAGS,
                    "levels_cache");
        }
        
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &((*bit_map_ptr)->levels[indx - 1]),
                &buffer_offset,
                sizeof(buffer_offset),
                DNXC_SW_STATE_NO_FLAGS,
                "levels");
        
        buffer_offset += level_size;

    }
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &((*bit_map_ptr)->nof_levels),
            &nof_levels,
            sizeof(nof_levels),
            DNXC_SW_STATE_NO_FLAGS,
            "nof_levels");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &((*bit_map_ptr)->size),
            &size, sizeof(size),
            DNXC_SW_STATE_NO_FLAGS,
            "size");

    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &((*bit_map_ptr)->buffer_size),
            &all_level_size,
            sizeof(all_level_size),
            DNXC_SW_STATE_NO_FLAGS,
            "buffer_size");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_destroy(
   int unit,
   uint32 module_id,
   sw_state_occ_bm_t *bit_map_ptr)
{
    uint8 support_cache;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(*bit_map_ptr, _SHR_E_INTERNAL, "ptr bit_map");

    DNX_SW_STATE_FREE(unit, module_id, (*bit_map_ptr)->levels, "bitmap levels");
    DNX_SW_STATE_FREE(unit, module_id, (*bit_map_ptr)->levels_buffer, "bitmap level buffers");

    support_cache = (*bit_map_ptr)->support_cache;

    if (support_cache)
    {
        DNX_SW_STATE_FREE(unit, module_id, (*bit_map_ptr)->levels_cache, "bitmap level cache");
        DNX_SW_STATE_FREE(unit, module_id, (*bit_map_ptr)->levels_cache_buffer, "bitmap levels cache buffer");
    }

    DNX_SW_STATE_FREE(unit, module_id, (*bit_map_ptr)->levels_size, "bitmap levels size");
    DNX_SW_STATE_FREE(unit, module_id, (*bit_map_ptr), "swstate occ bitmap"); 
    
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_occ_bm_clear(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map)
{
    uint32 level_indx, level_size, ii, buffer_offset;
    uint8 init_val;
    uint32 nof_levels;
    uint8 support_cache, cache_enabled;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    init_val = bit_map->init_val;
    if (init_val == FALSE)
    {
        init_val = 0X0;
    }
    else
    {
        init_val = 0XFF;
    }
    support_cache = bit_map->support_cache;
    cache_enabled = bit_map->cache_enabled;
    {
        nof_levels = bit_map->nof_levels;
        for (level_indx = 0; level_indx < nof_levels; ++level_indx)
        {
            level_size = bit_map->levels_size[level_indx];
            
            level_size = (level_size + UTILEX_NOF_BITS_IN_CHAR - 1) / UTILEX_NOF_BITS_IN_CHAR;
            if (support_cache && cache_enabled)
            {
                
                buffer_offset = bit_map->
                                levels_cache[level_indx];
                for (ii = 0; ii < level_size; ii++)
                {
                    DNX_SW_STATE_MEMCPY_BASIC(unit,
                            module_id,
                            &(bit_map->levels_cache_buffer[buffer_offset + ii]),
                            &init_val,
                            sizeof(init_val),
                            DNXC_SW_STATE_NO_FLAGS,
                            "levels_cache_buffer");
                }
            }
            else
            {
                
                buffer_offset = bit_map->levels[level_indx];
                for (ii = 0; ii < level_size; ii++)
                {
                    DNX_SW_STATE_MEMCPY_BASIC(unit,
                            module_id,
                            &(bit_map->levels_buffer[buffer_offset + ii]),
                            &init_val,
                            sizeof(init_val),
                            DNXC_SW_STATE_NO_FLAGS,
                            "levels_buffer");
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
sw_state_occ_bm_get_next_helper(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 val,
    uint32 level_indx,
    uint32 char_indx,
    uint32 *place,
    uint8 *found)
{
    uint32 cur_val, cur_bit, level_size;
    uint32 nof_levels;
    uint8 cur_val_uint8;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    SW_STATE_OCC_BM_ACTIVE_INST_GET(bit_map, level_indx, char_indx, cur_val_uint8);

    cur_val = cur_val_uint8;
    level_size = bit_map->levels_size[level_indx];
    
    if (val)
    {
        cur_bit = sw_state_bitmap_val_to_first_one[cur_val];
    }
    else
    {
        cur_bit = sw_state_bitmap_val_to_first_zero[cur_val];
    }
    
    if (cur_bit == SW_STATE_OCC_BM_NODE_IS_FULL || cur_bit + UTILEX_NOF_BITS_IN_CHAR * char_indx >= level_size)
    {
        *place = 0;
        *found = FALSE;
        goto exit;
    }
    
    nof_levels = bit_map->nof_levels;
    if (level_indx == nof_levels - 1)
    {
        *place = cur_bit + UTILEX_NOF_BITS_IN_CHAR * char_indx;
        *found = TRUE;
        goto exit;
    }
    
    SHR_IF_ERR_EXIT(sw_state_occ_bm_get_next_helper(unit,
                                                  bit_map,
                                                  val,
                                                  level_indx + 1,
                                                  cur_bit + UTILEX_NOF_BITS_IN_CHAR * char_indx, place, found));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_occ_bm_calc_mask(
    uint32 nof_levels,
    uint32 level_indx,
    uint32 char_indx,
    uint32 down_limit,
    uint32 up_limit,
    uint8 forward)
{
    uint32 range_start, range_end, btmp_start, btmp_end, start, end, mask_val, indx;

    range_start = down_limit;
    range_end = up_limit;

    for (indx = nof_levels - 1; indx > level_indx; --indx)
    {
        range_start = (range_start) / UTILEX_NOF_BITS_IN_CHAR;
        range_end = (range_end) / UTILEX_NOF_BITS_IN_CHAR;
    }

    btmp_start = char_indx * UTILEX_NOF_BITS_IN_CHAR;
    btmp_end = (char_indx + 1) * UTILEX_NOF_BITS_IN_CHAR - 1;

    if (range_start < btmp_start)
    {
        start = 0;
    }
    else if (range_start > btmp_end)
    {
        return UTILEX_U32_MAX;
    }
    else
    {
        start = range_start - btmp_start;
    }

    if (range_end > btmp_end)
    {
        end = UTILEX_NOF_BITS_IN_UINT32 - 1;
    }
    else if (range_end < btmp_start)
    {
        return UTILEX_U32_MAX;
    }
    else
    {
        end = range_end - btmp_start;
    }

    mask_val = 0;
    mask_val = UTILEX_ZERO_BITS_MASK(end, start);
    mask_val &= UTILEX_ZERO_BITS_MASK(UTILEX_NOF_BITS_IN_UINT32 - 1, UTILEX_NOF_BITS_IN_CHAR);

    return mask_val;
}


static shr_error_e
sw_state_occ_bm_get_next_range_helper(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 val,
    uint32 level_indx,
    uint32 char_indx,
    uint32 down_limit,
    uint32 up_limit,
    uint8 forward,
    uint32 *place,
    uint8 *found)
{
    uint32 cur_val, cur_bit, char_iter, level_size, mask_val, nof_levels;
    uint8 cur_val_uint8;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    level_size = bit_map->levels_size[level_indx];
    nof_levels = bit_map->nof_levels;

    SW_STATE_OCC_BM_ACTIVE_INST_GET(bit_map, level_indx, char_indx, cur_val_uint8);
    cur_val = cur_val_uint8;
    
    mask_val = sw_state_occ_bm_calc_mask(nof_levels, level_indx, char_indx, down_limit, up_limit, forward);
    if (val)
    {
        
        mask_val = ~mask_val;
        
        mask_val &= UTILEX_ZERO_BITS_MASK(UTILEX_NOF_BITS_IN_UINT32 - 1, UTILEX_NOF_BITS_IN_CHAR);
        if (level_indx == nof_levels - 1)
        {
            
            cur_val &= mask_val;
        }
        else
        {
            
            cur_val = mask_val;
        }
    }
    else
    {
        
        cur_val |= mask_val;
    }
    
    for (char_iter = 0; char_iter < UTILEX_NOF_BITS_IN_CHAR; ++char_iter)
    {
        if (forward)
        {
            
            if (val)
            {
                cur_bit = sw_state_bitmap_val_to_first_one[cur_val];
            }
            else
            {
                cur_bit = sw_state_bitmap_val_to_first_zero[cur_val];
            }
        }
        else
        {
            if (val)
            {
                cur_bit = sw_state_bitmap_val_to_last_zero[255 - cur_val];
            }
            else
            {
                cur_bit = sw_state_bitmap_val_to_last_zero[cur_val];
            }
        }
        
        if (cur_bit == SW_STATE_OCC_BM_NODE_IS_FULL || cur_bit + UTILEX_NOF_BITS_IN_CHAR * char_indx >= level_size)
        {
            *place = 0;
            *found = FALSE;
            goto exit;
        }
        
        if (level_indx == nof_levels - 1)
        {
            *place = cur_bit + UTILEX_NOF_BITS_IN_CHAR * char_indx;
            *found = TRUE;
            goto exit;
        }
        
        if (val)
        {
            cur_val &= UTILEX_RBIT(cur_bit);
        }
        else
        {
            cur_val |= UTILEX_BIT(cur_bit);
        }
        
        SHR_IF_ERR_EXIT(sw_state_occ_bm_get_next_range_helper(unit,
                                                            bit_map,
                                                            val,
                                                            level_indx + 1,
                                                            cur_bit + UTILEX_NOF_BITS_IN_CHAR * char_indx,
                                                            down_limit, up_limit, forward, place, found));
        if (*found)
        {
            goto exit;
        }
    }
    *place = 0;
    *found = FALSE;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_occ_bm_get_next(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 *place,
    uint8 *found)
{
    uint32 size;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    SHR_IF_ERR_EXIT(sw_state_occ_bm_get_next_helper(unit, bit_map, 0, 0, 0, place, found));
    size = bit_map->size;
    if (*place >= size)
    {
        *place = 0;
        *found = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_get_next_in_range(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 start,
    uint32 end,
    uint8 forward,
    uint8 val,
    uint32 *place,
    uint8 *found)
{
    uint32 size;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    SHR_IF_ERR_EXIT(sw_state_occ_bm_get_next_range_helper(unit, bit_map, val, 0, 0, start, end, forward, place, found));
    size = bit_map->size;
    if (*place >= size)
    {
        *place = 0;
        *found = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_occ_bm_alloc_next(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 *place,
    uint8 *found)
{
    uint32 size;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    SHR_IF_ERR_EXIT(sw_state_occ_bm_get_next(unit, module_id, bit_map, place, found));
    size = bit_map->size;

    if (*place >= size)
    {
        *place = 0;
        *found = FALSE;
        goto exit;
    }

    SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set(unit, module_id, bit_map, *place, 1));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_occ_bm_occup_status_set_helper(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 level_indx,
    uint32 char_indx,
    uint32 bit_indx,
    uint8 occupied)
{
    uint32 old_val, new_val;
    uint8 new_val_uint8;
    uint8 old_val_uint8;
    uint32 buffer_offset;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    
    SW_STATE_OCC_BM_ACTIVE_INST_GET(bit_map, level_indx, char_indx, old_val_uint8);
    old_val = old_val_uint8;
    new_val = old_val;

    SHR_IF_ERR_EXIT(utilex_set_field(&new_val, bit_indx, bit_indx, occupied));

    {
        
        new_val_uint8 = new_val;

        if(bit_map->cache_enabled)
        {
           
            buffer_offset = bit_map->levels_cache[level_indx];
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &(bit_map->levels_cache_buffer[buffer_offset + char_indx]),
                    &new_val_uint8,
                    sizeof(new_val_uint8),
                    DNXC_SW_STATE_NO_FLAGS,
                    "levels_cache_buffer");
        }
        else
        {
           
            buffer_offset = bit_map->levels[level_indx];
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &(bit_map->levels_buffer[buffer_offset + char_indx]),
                    &new_val_uint8,
                    sizeof(new_val_uint8),
                    DNXC_SW_STATE_NO_FLAGS,
                    "levels_buffer");
        }

    }
    
    if (level_indx == 0)
    {
        goto exit;
    }

    if ((occupied && old_val != 0xFF && new_val == 0xFF) || (!occupied && old_val == 0xFF && new_val != 0xFF))
    {
        SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set_helper(unit,
                                                              module_id,
                                                              bit_map,
                                                              level_indx - 1,
                                                              char_indx / UTILEX_NOF_BITS_IN_CHAR,
                                                              char_indx % UTILEX_NOF_BITS_IN_CHAR, occupied));
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_occ_bm_occup_status_set(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 place,
    uint8 occupied)
{
    uint32 nof_levels;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    nof_levels = bit_map->nof_levels;

    SHR_IF_ERR_EXIT(sw_state_occ_bm_occup_status_set_helper(unit,
                                                          module_id,
                                                          bit_map,
                                                          nof_levels - 1,
                                                          place / UTILEX_NOF_BITS_IN_CHAR,
                                                          place % UTILEX_NOF_BITS_IN_CHAR, occupied));
exit:
    SHR_FUNC_EXIT;

}


shr_error_e
sw_state_occ_bm_is_occupied(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 place,
    uint8 *occupied)
{
    uint32 char_indx, val, bit_indx;
    uint32 nof_levels;
    uint8 val_uint8;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");
    SHR_NULL_CHECK(occupied, _SHR_E_INTERNAL, "occupied");
    nof_levels = bit_map->nof_levels;
    char_indx = place / UTILEX_NOF_BITS_IN_CHAR;
    SW_STATE_OCC_BM_ACTIVE_INST_GET(bit_map, nof_levels - 1, char_indx, val_uint8);
    val = val_uint8;
    bit_indx = place % UTILEX_NOF_BITS_IN_CHAR;

    val = UTILEX_GET_BITS_RANGE(val, bit_indx, bit_indx);

    *occupied = UTILEX_NUM2BOOL(val);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_cache_set(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint8 cached)
{
    uint32 level_indx, level_size, ii;
    uint8 tmp_uint8, support_cache, cache_enabled;
    uint32 nof_levels;
    uint32 levels_cache_offset, levels_offset;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    support_cache = bit_map->support_cache;
    if (!support_cache && cached)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }
    cache_enabled = bit_map->cache_enabled;
    
    if (cache_enabled == cached)
    {
        goto exit;
    }
    {
        cache_enabled = cached;
        DNX_SW_STATE_MEMCPY_BASIC(unit,
                module_id,
                &(bit_map->cache_enabled),
                &cache_enabled,
                sizeof(cache_enabled),
                DNXC_SW_STATE_NO_FLAGS,
                "cache_enabled");
    }
    
    nof_levels = bit_map->nof_levels;
    if (cached)
    {
        for (level_indx = 0; level_indx < nof_levels; ++level_indx)
        {
            level_size = bit_map->levels_size[level_indx];
            level_size = (level_size + UTILEX_NOF_BITS_IN_CHAR - 1) / UTILEX_NOF_BITS_IN_CHAR;
            levels_cache_offset = bit_map->levels_cache[level_indx];
            levels_offset = bit_map->levels[level_indx];
            for (ii = 0; ii < level_size; ii++)
            {
                tmp_uint8 = bit_map->levels_buffer[levels_offset + ii];
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                        module_id,
                        &(bit_map->levels_cache_buffer[levels_cache_offset + ii]),
                        &tmp_uint8,
                        sizeof(tmp_uint8),
                        DNXC_SW_STATE_NO_FLAGS,
                        "levels_cache_buffer");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_cache_commit(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 flags)
{
    uint32 level_indx, level_size, ii;
    uint32 levels_cache_offset, levels_offset;
    uint8 support_cache;
    uint8 tmp_uint8, cache_enabled;
    uint32 nof_levels;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    support_cache = bit_map->support_cache;
    if (!support_cache)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }
    
    cache_enabled = bit_map->cache_enabled;
    if (!cache_enabled)
    {
        goto exit;
    }
    
    nof_levels = bit_map->nof_levels;
    {
        for (level_indx = 0; level_indx < nof_levels; ++level_indx)
        {
            level_size = bit_map->levels_size[level_indx];

            level_size = (level_size + UTILEX_NOF_BITS_IN_CHAR - 1) / UTILEX_NOF_BITS_IN_CHAR;
            levels_cache_offset = bit_map->levels_cache[level_indx];
            levels_offset = bit_map->levels[level_indx];
            for (ii = 0; ii < level_size; ii++)
            {
                tmp_uint8 = bit_map->levels_cache_buffer[levels_cache_offset + ii];
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                        module_id,
                        &(bit_map->levels_buffer[levels_offset + ii]),
                        &tmp_uint8,
                        sizeof(tmp_uint8),
                        DNXC_SW_STATE_NO_FLAGS,
                        "levels_buffer");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_cache_rollback(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 flags)
{
    uint32 level_indx, level_size;
    uint8 support_cache;
    uint8 tmp_uint8, cache_enabled;
    uint32 nof_levels;
    uint32 ii, levels_cache_offset, levels_offset;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    support_cache = bit_map->support_cache;
    if (!support_cache)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }
    
    cache_enabled = bit_map->cache_enabled;
    if (!cache_enabled)
    {
        goto exit;
    }
    
    nof_levels = bit_map->nof_levels;
    {
        for (level_indx = 0; level_indx < nof_levels; ++level_indx)
        {
            level_size = bit_map->levels_size[level_indx];
            level_size = (level_size + UTILEX_NOF_BITS_IN_CHAR - 1) / UTILEX_NOF_BITS_IN_CHAR;
            levels_cache_offset = bit_map->levels_cache[level_indx];
            levels_offset = bit_map->levels[level_indx];
            for (ii = 0; ii < level_size; ii++)
            {
                tmp_uint8 = bit_map->levels_buffer[levels_offset + ii];
                DNX_SW_STATE_MEMCPY_BASIC(unit,
                        module_id,
                        &(bit_map->levels_cache_buffer[levels_cache_offset + ii]),
                        &tmp_uint8,
                        sizeof(tmp_uint8),
                        DNXC_SW_STATE_NO_FLAGS,
                        "levels_cache_buffer");
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_get_buffer_size(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint32 *buffer_size_p)
{
    uint32 buffer_size;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    buffer_size = bit_map->buffer_size;
    *buffer_size_p = buffer_size;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_get_support_cache(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 *support_cache_p)
{
    uint8 support_cache;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    support_cache = bit_map->support_cache;
    *support_cache_p = support_cache;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_occ_bm_print(
    int unit,
    sw_state_occ_bm_t bit_map)
{
    uint32
        char_indx, level_indx, level_size, levels_cache_offset, levels_offset, nof_levels, size, buffer_size;
    uint8 level_bits;
    uint8 init_val, support_cache, cache_enabled;

    SHR_FUNC_INIT_VARS(unit);

    SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(unit);
    SHR_NULL_CHECK(bit_map, _SHR_E_INTERNAL, "bit_map");

    size = bit_map->size;
    DNX_SW_STATE_PRINT(unit, "size %u.\n", size);
    init_val = bit_map->init_val;
    DNX_SW_STATE_PRINT(unit, "init_val %u.\n", init_val);
    support_cache = bit_map->support_cache;
    DNX_SW_STATE_PRINT(unit, "support_cache %u.\n", support_cache);
    cache_enabled = bit_map->cache_enabled;
    DNX_SW_STATE_PRINT(unit, "cache_enabled %u.\n", cache_enabled);
    buffer_size = bit_map->buffer_size;
    DNX_SW_STATE_PRINT(unit, "buffer_size %u.\n", buffer_size);
    nof_levels = bit_map->nof_levels;
    DNX_SW_STATE_PRINT(unit, "nof_levels %u.\n", nof_levels);
    DNX_SW_STATE_PRINT(unit, "Bare levels: \n");

    for (level_indx = 0; level_indx < nof_levels; ++level_indx)
    {
        level_size = bit_map->levels_size[level_indx];
        level_size = (level_size + UTILEX_NOF_BITS_IN_CHAR - 1) / UTILEX_NOF_BITS_IN_CHAR;
        DNX_SW_STATE_PRINT(unit, "level %u. level size %u:", level_indx, level_size);
        levels_offset = bit_map->levels[level_indx];
        for (char_indx = 0; char_indx < level_size; ++char_indx)
        {
            if(char_indx%20 == 0)
            {
                DNX_SW_STATE_PRINT(unit, "\n");
                DNX_SW_STATE_PRINT(unit, "%d:", char_indx);
            }
            level_bits = bit_map->levels_buffer[levels_offset + char_indx];
            DNX_SW_STATE_PRINT(unit, "%02x  ", level_bits);
        }
        DNX_SW_STATE_PRINT(unit, "\n");
    }
    if (cache_enabled)
    {
        DNX_SW_STATE_PRINT(unit, "Cache levels: \n");
        for (level_indx = 0; level_indx < nof_levels; ++level_indx)
        {
            level_size = bit_map->levels_size[level_indx];
            level_size = (level_size + UTILEX_NOF_BITS_IN_CHAR - 1) / UTILEX_NOF_BITS_IN_CHAR;
            levels_cache_offset = bit_map->levels_cache[level_indx];
            for (char_indx = 0; char_indx < level_size; char_indx++)
            {
                if(char_indx%20 == 0)
                {
                    DNX_SW_STATE_PRINT(unit, "\n");
                    DNX_SW_STATE_PRINT(unit, "%d:", char_indx);
                }
                level_bits = bit_map->levels_cache_buffer[levels_cache_offset + char_indx];
                DNX_SW_STATE_PRINT(unit, "%02x  ", level_bits);
            }
            DNX_SW_STATE_PRINT(unit, "\n");
        }
    }

    DNX_SW_STATE_PRINT(unit, "\n\n\n");

exit:
    SHR_FUNC_EXIT;

}

int sw_state_occ_bm_size_get(
    sw_state_occ_bitmap_init_info_t * init_info)
{
    int size = 0;
    uint32 cur_size, level_size = 0, all_level_size = 0, nof_levels = 1;
    uint32 indx;

    
    size += (sizeof(sw_state_occ_bm_t));

    cur_size = UTILEX_NOF_BITS_IN_CHAR;
    while (cur_size < init_info->size)
    {
        cur_size *= UTILEX_NOF_BITS_IN_CHAR;
        ++nof_levels;
    }

    
    size += (sizeof(uint32) * nof_levels);

    if (init_info->support_cache)
    {
        
        size += (sizeof(uint32) * nof_levels);
    }

    
    size += (sizeof(uint32) * nof_levels);

    all_level_size = 0;
    level_size = init_info->size;
    for (indx = nof_levels; indx > 0; --indx)
    {
        level_size = (level_size + (UTILEX_NOF_BITS_IN_CHAR - 1)) / UTILEX_NOF_BITS_IN_CHAR;    

        all_level_size += level_size;
    }
    if (init_info->support_cache)
    {
        
        size += (sizeof(uint8) * all_level_size);
    }

    
    size += (sizeof(uint8) * all_level_size);

    return size;
}

#undef _ERR_MSG_MODULE_NAME
