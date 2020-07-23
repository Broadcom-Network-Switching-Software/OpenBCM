/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SW_STATE_OCCUPATION_BITMAP_H_INCLUDED

#define SW_STATE_OCCUPATION_BITMAP_H_INCLUDED

#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>

#define  SW_STATE_OCC_BM_ARRAY_MAP_LEN SAL_BIT(UTILEX_NOF_BITS_IN_CHAR)

#define  SW_STATE_OCC_BM_NODE_IS_FULL  UTILEX_NOF_BITS_IN_CHAR

#define SW_STATE_OCC_BM_CREATE(module_id, occ_bm, init_info, occ_size, alloc_flags) \
    SHR_IF_ERR_EXIT(sw_state_occ_bm_create(unit, module_id, init_info, &occ_bm, occ_size, alloc_flags));

#define SW_STATE_OCC_BM_DESTROY(module_id, occ_bm) \
    sw_state_occ_bm_destroy(unit, module_id, &occ_bm)

#define SW_STATE_OCC_BM_INIT_INFO_CLEAR(init_info) \
    sw_state_occ_bm_init_info_clear(init_info)

#define SW_STATE_OCC_BM_GET_NEXT_IN_RANGE(module_id, occ_bm, start, end, forward, val, place, found) \
    sw_state_occ_bm_get_next_in_range(unit, module_id, occ_bm, start, end, forward, val, place, found)

#define SW_STATE_OCC_BM_GET_NEXT(module_id, occ_bm, place, found) \
    sw_state_occ_bm_get_next(unit, module_id, occ_bm, place, found)

#define SW_STATE_OCC_BM_STATUS_SET(module_id, occ_bm, place, occupied) \
    sw_state_occ_bm_occup_status_set(unit, module_id, occ_bm, place, occupied)

#define SW_STATE_OCC_BM_IS_OCCUPIED(module_id, occ_bm, place, occupied) \
    sw_state_occ_bm_is_occupied(unit, module_id, occ_bm, place, occupied)

#define SW_STATE_OCC_BM_ALLOC_NEXT(module_id, occ_bm, place, found) \
    sw_state_occ_bm_alloc_next(unit, module_id, occ_bm, place, found)

#define SW_STATE_OCC_BM_CLEAR(module_id, occ_bm) \
    sw_state_occ_bm_clear(unit, module_id, occ_bm)

#define SW_STATE_OCC_BM_PRINT(module_id, occ_bm) \
    sw_state_occ_bm_print(unit, occ_bm)

#define SW_STATE_OCC_BM_DEFAULT_PRINT(unit, occ_bm) \
    sw_state_occ_bm_print(unit, *(occ_bm))

#define SW_STATE_OCC_BM_ACTIVE_INST_GET(_bit_map, _level_indx, _char_indx, _cur_val) \
  { \
    uint8 cache_enabled ; \
    uint32 buffer_offset ; \
    cache_enabled = _bit_map->cache_enabled; \
    if (cache_enabled) \
    { \
       \
      buffer_offset = _bit_map->levels_cache[_level_indx]; \
      _cur_val = _bit_map->levels_cache_buffer[buffer_offset + _char_indx]; \
    } \
    else \
    { \
       \
      buffer_offset = _bit_map->levels[_level_indx]; \
      _cur_val = _bit_map->levels_buffer[buffer_offset + _char_indx]; \
    } \
  }

#define SW_STATE_OCC_BM_SIZE_GET(init_info) \
    sw_state_occ_bm_size_get(init_info);

#define SW_STATE_OCC_BM_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
     \
    SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL) ; \
    SHR_EXIT() ; \
  }

typedef struct
{

    uint32 *levels;

    uint32 *levels_size;

    uint32 nof_levels;

    uint32 size;

    uint8 init_val;

    uint8 support_cache;

    uint8 cache_enabled;

    uint32 *levels_cache;

    uint8 *levels_buffer;
    uint8 *levels_cache_buffer;

    uint32 buffer_size;
}  *sw_state_occ_bm_t;

typedef struct
{

    uint32 size;

    uint8 init_val;

    uint8 support_cache;

} sw_state_occ_bitmap_init_info_t;

shr_error_e sw_state_occ_bm_init_info_clear(
    sw_state_occ_bitmap_init_info_t * info);

shr_error_e sw_state_occ_bm_create(
    int unit,
    uint32 module_id,
    sw_state_occ_bitmap_init_info_t * init_info,
    sw_state_occ_bm_t * bit_map_ptr,
    uint32 occ_size,
    uint32 alloc_flags);

shr_error_e sw_state_occ_bm_destroy(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t * bit_map_ptr);

shr_error_e sw_state_occ_bm_clear(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map);

shr_error_e sw_state_occ_bm_get_next(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 *place,
    uint8 *found);

shr_error_e sw_state_occ_bm_get_next_in_range(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 start,
    uint32 end,
    uint8 forward,
    uint8 val,
    uint32 *place,
    uint8 *found);

shr_error_e sw_state_occ_bm_alloc_next(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 *place,
    uint8 *found);

shr_error_e sw_state_occ_bm_occup_status_set(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 place,
    uint8 occupied);

shr_error_e sw_state_occ_bm_is_occupied(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 place,
    uint8 *occupied);

shr_error_e sw_state_occ_bm_cache_set(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint8 cached);

shr_error_e sw_state_occ_bm_cache_commit(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 flags);

shr_error_e sw_state_occ_bm_cache_rollback(
    int unit,
    uint32 module_id,
    sw_state_occ_bm_t bit_map,
    uint32 flags);

shr_error_e sw_state_occ_bm_get_buffer_size(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint32 *buffer_size_p);

shr_error_e sw_state_occ_bm_get_support_cache(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 *support_cache_p);

uint32 sw_state_occ_bm_get_illegal_bitmap_handle(
    void);

shr_error_e sw_state_occ_is_bitmap_active(
    int unit,
    sw_state_occ_bm_t bit_map,
    uint8 *in_use);

shr_error_e sw_state_occ_bm_print(
    int unit,
    sw_state_occ_bm_t bit_map);

int sw_state_occ_bm_size_get(
    sw_state_occ_bitmap_init_info_t * init_info);

#endif
