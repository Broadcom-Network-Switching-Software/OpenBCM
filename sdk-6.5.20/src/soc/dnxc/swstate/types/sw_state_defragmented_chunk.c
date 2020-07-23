/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX



#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/types/sw_state_defragmented_chunk.h>
#include <soc/dnxc/swstate/types/sw_state_cb.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/auto_generated/access/dnx_sw_state_defragmented_chunk_access.h>





#define SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT 0xff
#define SW_STATE_DEFRAGMENTED_CHUNK_SAVED_FOR_FUTURE_USE_EXPONENT 0xfe










 


#ifdef BCM_DNX_SUPPORT





shr_error_e
sw_state_defragmented_chunk_create(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk,
    sw_state_defragmented_chunk_init_info_t * init_info,
    uint32 chunk_size,
    uint32 alloc_flags)
{
    uint8 tmp_exponent;
    uint32 _chunk_size;
    SHR_FUNC_INIT_VARS(unit);

    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT;

    if (*defragmented_chunk != NULL) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_EXISTS, "sw state defragmented_chunk create ERROR: defragmented_chunk already exist.\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    SHR_NULL_CHECK(init_info, _SHR_E_PARAM, "init_info");

    
    if (init_info->chunk_size == 0 && chunk_size == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid chunk_size\n");
    }

    if (chunk_size != 0)
    {
        _chunk_size = chunk_size;
    }
    else
    {
        _chunk_size = init_info->chunk_size;
    }
    DNX_SW_STATE_ALLOC(unit, module_id, *defragmented_chunk, **defragmented_chunk,  1,
                       DNXC_SW_STATE_NO_FLAGS, "sw_state defragmented_chunk");

    
    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &((*defragmented_chunk)->init_info), init_info,
                              sizeof(sw_state_defragmented_chunk_init_info_t), 0,
                              "sw_state defragmented_chunk init_info");

    
    DNX_SW_STATE_ALLOC(unit, module_id, (*defragmented_chunk)->slots_array, uint8,
            _chunk_size, alloc_flags, "defragmented_chunk init_info");

    DNX_SW_STATE_RANGE_FILL(unit, module_id, (*defragmented_chunk)->slots_array, uint8,
                            0,
                            _chunk_size,
                            tmp_exponent,
                            0,
                            "slots_array");

    
    DNX_SW_STATE_SET(unit, module_id, (*defragmented_chunk)->free_slots_available, _chunk_size, uint32,
                     0,
                     "free_slots_available");

    SW_STATE_CB_DB_REGISTER_CB(module_id,
            (*defragmented_chunk)->move_cb_db,
            init_info->move_function,
            sw_state_defragmented_cunk_move_cb_get_cb);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_defragmented_chunk_destroy(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t * defragmented_chunk)
{
    SHR_FUNC_INIT_VARS(unit);

    
    DNX_SW_STATE_FREE(unit, module_id, (*defragmented_chunk)->slots_array, "sw state defragmented_chunk slots_array");

    
    DNX_SW_STATE_FREE(unit, module_id, (*defragmented_chunk), "sw state defragmented_chunk");

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_defragmented_chunk_empty_piece_search(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    uint32 *piece_offset)
{
    uint32 slot_in_piece;
    uint32 chunk_size;
    uint32 current_offset;
    uint8 exponent;

    SHR_FUNC_INIT_VARS(unit);

    
    chunk_size = defragmented_chunk->init_info.chunk_size;
    for (current_offset = 0; current_offset <= (chunk_size - wanted_size); current_offset += wanted_size)
    {
        exponent = defragmented_chunk->slots_array[current_offset];
        if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
        {
            
            *piece_offset = current_offset;
            for (slot_in_piece = 0; slot_in_piece < wanted_size; ++slot_in_piece)
            {
                exponent = defragmented_chunk->slots_array[current_offset + slot_in_piece];
                if (exponent != SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
                {
                    break;
                }
            }

            if (slot_in_piece == wanted_size)
            {
                
                SHR_EXIT();
            }
        }
    }

    
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_defragmented_chunk_emptier_piece_search(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    uint32 *piece_offset)
{
    uint8 exponent;
    uint32 chunk_size;
    uint32 offset;
    uint32 slot;
    uint32 nof_clear_slots_in_piece;
    uint32 max_nof_clear_slots_in_piece = 0;
    SHR_FUNC_INIT_VARS(unit);

    chunk_size = defragmented_chunk->init_info.chunk_size;
    
    for (offset = 0; offset <= chunk_size - wanted_size; offset += wanted_size)
    {
        
        for (nof_clear_slots_in_piece = 0, slot = 0; slot < wanted_size; ++slot)
        {
            
            exponent = defragmented_chunk->slots_array[offset + slot];
            if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
            {
                ++nof_clear_slots_in_piece;
            }
        }
        
        if (max_nof_clear_slots_in_piece < nof_clear_slots_in_piece)
        {
            max_nof_clear_slots_in_piece = nof_clear_slots_in_piece;
            *piece_offset = offset;
        }
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_defragmented_chunk_empty_slots_in_piece_as_saved_mark(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 size,
    uint32 piece_offset)
{
    uint8 exponent, tmp_exponent;
    uint32 offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(defragmented_chunk, _SHR_E_PARAM, "defragmented_chunk");

    
    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_SAVED_FOR_FUTURE_USE_EXPONENT;

    
    for (offset = piece_offset; offset < piece_offset + size; ++offset)
    {
        
        exponent = defragmented_chunk->slots_array[offset];
        if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
        {
            DNX_SW_STATE_MEMCPY_BASIC(unit,
                    module_id,
                    &defragmented_chunk->slots_array[offset],
                    &tmp_exponent,
                    sizeof(tmp_exponent),
                    DNXC_SW_STATE_NO_FLAGS,
                    "defragmented_chunk->slots_array");
        }
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
sw_state_defragmented_chunk_defrag(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 wanted_size,
    void *move_cb_additional_info,
    uint32 *piece_offset)
{
    int rv;
    uint32 iterated_size;
    uint32 offset;
    uint32 iteration_exponent;
    uint32 new_location_offset;
    uint8 exponent, tmp_exponent;
    sw_state_defragmented_cunk_move_cb move_cb;

    SHR_FUNC_INIT_VARS(unit);

    
    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_SAVED_FOR_FUTURE_USE_EXPONENT;

    
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_emptier_piece_search
                    (unit, module_id, defragmented_chunk, wanted_size, piece_offset));
    
    SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_empty_slots_in_piece_as_saved_mark
                    (unit, module_id, defragmented_chunk, wanted_size, *piece_offset));
    
    iterated_size = wanted_size;
    do
    {
        iterated_size /= 2;
        iteration_exponent = utilex_log2_round_down(iterated_size);
        
        for (offset = *piece_offset; offset < (*piece_offset + wanted_size); offset += iterated_size)
        {
            exponent = defragmented_chunk->slots_array[offset];
            if (exponent != iteration_exponent)
            {
                continue;
            }
            
            rv = sw_state_defragmented_chunk_empty_piece_search(unit, module_id, defragmented_chunk, iterated_size,
                                                                &new_location_offset);
            if (rv == _SHR_E_NOT_FOUND)
            {
                
                SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_defrag
                                (unit, module_id, defragmented_chunk, iterated_size, move_cb_additional_info,
                                 &new_location_offset));
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }
            
            DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                                    new_location_offset,
                                    iterated_size,
                                    exponent,
                                    0,
                                    "slots_array");
            
            rv = sw_state_defragmented_cunk_move_cb_get_cb(&defragmented_chunk->move_cb_db, 1 , NULL);
            SW_STATE_CB_DB_GET_CB(module_id,
                    defragmented_chunk->move_cb_db,
                    &move_cb,
                    sw_state_defragmented_cunk_move_cb_get_cb);

            if (rv != _SHR_E_NOT_FOUND )
            {
                SHR_IF_ERR_EXIT((*move_cb) (unit, offset, new_location_offset, iterated_size, move_cb_additional_info));
            }
            
            DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                                    offset,
                                    iterated_size,
                                    tmp_exponent,
                                    0,
                                    "slots_array");
        }
    }
    while (iterated_size > 1);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_defragmented_chunk_piece_alloc(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 slots_in_piece,
    void *move_cb_additional_info,
    uint32 *piece_offset)
{
    int rv;
    uint8 exponent;
    uint32 chunk_size;
    uint32 free_slots_available;
    uint32 tmp_free_slots_available;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!utilex_is_power_of_2(slots_in_piece))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid slots_in_piece size, has to be a power of 2\n");
    }

    chunk_size = defragmented_chunk->init_info.chunk_size;
    if (slots_in_piece > chunk_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid slots_in_piece size, bigger the total chunk size\n");
    }

    
    free_slots_available = defragmented_chunk->free_slots_available;
    if (slots_in_piece > free_slots_available)
    {
        SHR_ERR_EXIT(_SHR_E_FULL, "Chunk is full, no place for piece of size %d\n", slots_in_piece);
    }

    
    rv = sw_state_defragmented_chunk_empty_piece_search(unit, module_id, defragmented_chunk, slots_in_piece,
                                                        piece_offset);
    if (rv == _SHR_E_NOT_FOUND)
    {
        
        SHR_IF_ERR_EXIT(sw_state_defragmented_chunk_defrag
                        (unit, module_id, defragmented_chunk, slots_in_piece, move_cb_additional_info, piece_offset));
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    
    tmp_free_slots_available = free_slots_available - slots_in_piece;
    DNX_SW_STATE_MEMCPY_BASIC(unit,
            module_id,
            &defragmented_chunk->free_slots_available,
            &tmp_free_slots_available,
            sizeof(tmp_free_slots_available),
            DNXC_SW_STATE_NO_FLAGS,
            "defragmented_chunk->free_slots_available");

    exponent = utilex_log2_round_down(slots_in_piece);
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            *piece_offset,
                            slots_in_piece,
                            exponent,
                            0,
                            "slots_array");

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
sw_state_defragmented_chunk_piece_free(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk,
    uint32 piece_offset)
{
    uint32 chunk_size;
    uint32 nof_free_slots;
    uint8 exponent, tmp_exponent;
    uint32 piece_size;

    SHR_FUNC_INIT_VARS(unit);

    
    tmp_exponent = SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT;

    
    chunk_size = defragmented_chunk->init_info.chunk_size;
    if (piece_offset >= chunk_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid piece_offset value - " "provided offset is too big for provided chunk\n");
    }

    

    exponent = defragmented_chunk->slots_array[piece_offset];
    if (exponent == SW_STATE_DEFRAGMENTED_CHUNK_INVALID_EXPONENT)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid piece offset value - " "provided offset was not allocated\n");
    }
    piece_size = 1 << exponent;
    if (piece_offset % piece_size != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid piece_offset value - "
                     "provided offset is not the offset of the first slot in piece\n");
    }

    
    DNX_SW_STATE_RANGE_FILL(unit, module_id, defragmented_chunk->slots_array, uint8,
                            piece_offset,
                            piece_size,
                            tmp_exponent,
                            0,
                            "slots_array");

    
    nof_free_slots = defragmented_chunk->free_slots_available + piece_size;

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, &defragmented_chunk->free_slots_available, &nof_free_slots, sizeof(nof_free_slots), DNXC_SW_STATE_NO_FLAGS, "defragmented_chunk->free_slots_available");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_defragmented_chunk_print(
    int unit,
    uint32 module_id,
    sw_state_defragmented_chunk_t defragmented_chunk)
{
    uint32 i;
    uint32 chunk_size;
    uint8 exponent;

    SHR_FUNC_INIT_VARS(unit);

    if(defragmented_chunk == NULL) {
        DNX_SW_STATE_PRINT(unit, "NULL\n");
        goto exit;
    }

    chunk_size = defragmented_chunk->init_info.chunk_size;
    for (i = 0; i < chunk_size; ++i)
    {
        DNX_SW_STATE_DUMP_UPDATE_CURRENT_IDX(unit, i);
        exponent = defragmented_chunk->slots_array[i];
        DNX_SW_STATE_PRINT_WITH_STRIDE_UPDATE(unit, exponent, "offset %03d: %d\n", i, exponent);
    }
    DNX_SW_STAET_DUMP_END_OF_STRIDE(unit);

exit:
    SHR_FUNC_EXIT;
}

int sw_state_defragmented_chunk_size_get(
    sw_state_defragmented_chunk_init_info_t * init_info)
{
    int size = 0;

    size += (sizeof(sw_state_defragmented_chunk_t));

    size += (sizeof(uint8) * init_info->chunk_size);

    return size;
}

shr_error_e
sw_state_defragmented_chunk_move_test(
    int unit,
    uint32 old_offset,
    uint32 new_offset,
    uint32 nof_reaources_profile_use,
    void *move_cb_additional_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



#else


shr_error_e
sw_state_defragmented_chunk_dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

#endif
