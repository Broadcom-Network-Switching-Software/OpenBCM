/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SW_STATE_DEFRAGMENTED_CHUNK_H_INCLUDED

#define SW_STATE_DEFRAGMENTED_CHUNK_H_INCLUDED

#ifdef BCM_DNX_SUPPORT

#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>

#include <soc/dnxc/swstate/types/sw_state_cb.h>

#define SW_STATE_DEFRAGMENTED_CHUNK_CREATE(module_id, defragmented_chunk, init_info, chunk_size, alloc_flags)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_create(unit, module_id, &defragmented_chunk, init_info, chunk_size, alloc_flags))

#define SW_STATE_DEFRAGMENTED_CHUNK_DESTROY(module_id, defragmented_chunk)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_destroy(unit, module_id, &(defragmented_chunk)))

#define SW_STATE_DEFRAGMENTED_CHUNK_PIECE_ALLOC(module_id, defragmented_chunk, slots_in_piece, move_cb_additional_info, piece_offset)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_piece_alloc(unit, module_id, defragmented_chunk, slots_in_piece, move_cb_additional_info, piece_offset))

#define SW_STATE_DEFRAGMENTED_CHUNK_PIECE_FREE(module_id, defragmented_chunk, piece_offset)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_piece_free(unit, module_id, defragmented_chunk, piece_offset))

#define SW_STATE_DEFRAGMENTED_CHUNK_PRINT(module_id, defragmented_chunk)\
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_print(unit, module_id, defragmented_chunk))

#define SW_STATE_DEFRAGMENTED_CHUNK_DEFAULT_PRINT(unit, module_id, defragmented_chunk)\
    sw_state_defragmented_chunk_print(unit, module_id, *(defragmented_chunk))

#define SW_STATE_DEFRAGMENTED_CHUNK_SIZE_GET(init_info)\
    sw_state_defragmented_chunk_size_get(init_info)

typedef struct
{

    uint32 chunk_size;

    char move_function[SW_STATE_CB_DB_NAME_STR_SIZE];
} sw_state_defragmented_chunk_init_info_t;

typedef struct
{

    uint8 *slots_array;

    uint32 free_slots_available;

    sw_state_defragmented_chunk_init_info_t init_info;

    sw_state_cb_t move_cb_db;
}  *sw_state_defragmented_chunk_t;

shr_error_e sw_state_defragmented_chunk_create(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk,
    sw_state_defragmented_chunk_init_info_t * init_info,
    uint32 chunk_size,
    uint32 alloc_flags);

shr_error_e sw_state_defragmented_chunk_destroy(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk);

shr_error_e sw_state_defragmented_chunk_piece_alloc(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 slots_in_piece,
    void *move_cb_additional_info,
    uint32 *piece_offset);

shr_error_e sw_state_defragmented_chunk_piece_free(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 piece_offset);

shr_error_e sw_state_defragmented_chunk_print(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk);

int sw_state_defragmented_chunk_size_get(
    sw_state_defragmented_chunk_init_info_t * init_info);

shr_error_e sw_state_defragmented_chunk_move_test(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    uint32 nof_reaources_profile_use,
    void *move_cb_additional_info);

#else

shr_error_e sw_state_defragmented_chunk_dummy_empty_function_to_make_compiler_happy(
    int unit);

#endif

#endif
