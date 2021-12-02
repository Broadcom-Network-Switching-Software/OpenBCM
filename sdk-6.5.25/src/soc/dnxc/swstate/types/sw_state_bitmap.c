
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */


#include <shared/bitop.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/types/sw_state_bitmap.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>



#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

typedef enum sw_state_bitmap_range_op_e
{
   SW_STATE_BITMAP_RANGE_WRITE,
   SW_STATE_BITMAP_RANGE_AND,
   SW_STATE_BITMAP_RANGE_OR,
   SW_STATE_BITMAP_RANGE_XOR,
   SW_STATE_BITMAP_RANGE_REMOVE,
   SW_STATE_BITMAP_RANGE_NEGATE,
   SW_STATE_BITMAP_RANGE_CLEAR,
   SW_STATE_BITMAP_RANGE_SET,
   SW_STATE_BITMAP_RANGE_NULL,
   SW_STATE_BITMAP_RANGE_TEST,
   SW_STATE_BITMAP_RANGE_EQ,
   SW_STATE_BITMAP_RANGE_COUNT,
} sw_state_bitmap_range_op_t;

STATIC int dnxc_sw_state_bitmap_range_bit_result(
    int unit,
    uint32 module_id,
    SHR_BITDCL *dest,
    uint32 offset,
    SHR_BITDCL *src,
    uint32 range,
    uint8 *result,
    sw_state_bitmap_range_op_t op_type,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);


    
    switch(op_type) {
        case SW_STATE_BITMAP_RANGE_NULL:
        *(result) = (SHR_BITNULL_RANGE(dest, offset, range) > 0) ? 1: 0;
        break;
        case SW_STATE_BITMAP_RANGE_TEST:
        SHR_BITTEST_RANGE(dest, offset, range, *(result));
        break;
        case SW_STATE_BITMAP_RANGE_EQ:
        *(result) = (SHR_BITEQ_RANGE(src, dest, offset, range) > 0) ? 1: 0;
        break;
        case SW_STATE_BITMAP_RANGE_COUNT:
        shr_bitop_range_count(dest, offset, range, (int *)result);
        break;
        default:
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state bitmap ERROR: unknown bitmap operation detected\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int dnxc_sw_state_bitmap_range_change(
    int unit,
    uint32 module_id,
    SHR_BITDCL *dest,
    uint32 dest_offset,
    SHR_BITDCL *src,
    uint32 src_offset,
    uint32 range,
    sw_state_bitmap_range_op_t op_type,
    uint32 flags,
    char *dbg_string)
{
    uint32 byte_offset = 0;
    uint32 bit_offset = 0;
    uint32 calculated_range = 0;

    SHR_BITDCL tmp_storage[1] = {0};

    SHR_FUNC_INIT_VARS(unit);

    
    byte_offset = (dest_offset / SHR_BITWID) * sizeof(SHR_BITDCL);

    
    bit_offset  = (dest_offset % SHR_BITWID);

    do {
        
        DNX_SW_STATE_MEMREAD(unit, tmp_storage, dest, byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

        
        calculated_range = UTILEX_MIN(range, SHR_BITWID - bit_offset);

        
        switch(op_type) {
            case SW_STATE_BITMAP_RANGE_CLEAR:
            SHR_BITCLR_RANGE(tmp_storage, bit_offset, calculated_range);
            break;
            case SW_STATE_BITMAP_RANGE_SET:
            SHR_BITSET_RANGE(tmp_storage, bit_offset, calculated_range);
            break;
            case SW_STATE_BITMAP_RANGE_WRITE:
            SHR_BITCOPY_RANGE(tmp_storage, bit_offset, src, src_offset, calculated_range);
            break;
            case SW_STATE_BITMAP_RANGE_AND:
            SHR_BITAND_RANGE(src, tmp_storage, bit_offset, calculated_range, tmp_storage);
            break;
            case SW_STATE_BITMAP_RANGE_OR:
            SHR_BITOR_RANGE(src, tmp_storage, bit_offset, calculated_range, tmp_storage);
            break;
            case SW_STATE_BITMAP_RANGE_XOR:
            SHR_BITXOR_RANGE(src, tmp_storage, bit_offset, calculated_range, tmp_storage);
            break;
            case SW_STATE_BITMAP_RANGE_REMOVE:
            SHR_BITREMOVE_RANGE(src, tmp_storage, bit_offset, calculated_range, tmp_storage);
            break;
            case SW_STATE_BITMAP_RANGE_NEGATE:
            SHR_BITNEGATE_RANGE(tmp_storage, bit_offset, calculated_range, tmp_storage);
            break;
            default:
            SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state bitmap ERROR: unknown bitmap operation detected\n%s%s%s", EMPTY, EMPTY, EMPTY);
        }

        
        if (tmp_storage[0] != dest[byte_offset/sizeof(SHR_BITDCL)])
        {
            DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, tmp_storage, 0, dest, byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);
        }

        

        
        bit_offset = 0;

        
        byte_offset += sizeof(SHR_BITDCL);

        
        src_offset += calculated_range;

        
        range -= calculated_range;

        
        tmp_storage[0] = 0;

        
        assert(((int) range) >= 0);

    }
    while (range > 0);

exit:
    SHR_FUNC_EXIT;
}

STATIC int dnxc_sw_state_bitmap_change(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 idx_bit,
    uint8 value,
    uint32 flags,
    char *dbg_string)
{
    uint32 byte_offset = 0;
    uint32 bit_offset = 0;
    SHR_BITDCL tmp_storage[1] = {0};

    SHR_FUNC_INIT_VARS(unit);

    
    byte_offset = (idx_bit / SHR_BITWID) * sizeof(SHR_BITDCL);

    
    bit_offset  = (idx_bit % SHR_BITWID);

    
    DNX_SW_STATE_MEMREAD(unit, tmp_storage, location, byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

    
    if( 0 == value ) {
        SHR_BITCLR((tmp_storage), bit_offset);
    } else {
        SHR_BITSET((tmp_storage), bit_offset);
    }

    
    DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, (uint8 *)(tmp_storage), 0, (uint8 *)(location), byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_alloc(
    int unit,
    uint32 module_id,
    uint8 **location,
    uint32 nof_bits_to_alloc,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_SW_STATE_ALLOC_BASIC(unit, module_id, location, _SHR_BITDCLSIZE(nof_bits_to_alloc) + 1, sizeof(SHR_BITDCL), flags | DNXC_SW_STATE_ALLOC_BITMAP_FLAG, dbg_string);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_change(unit, module_id, location, idx_bit, 1, flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_change(unit, module_id, location, idx_bit, 0, flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_get(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 idx_bit,
    uint8 *bit_result,
    uint32 flags,
    char *dbg_string)
{
    int bit_offset = 0;
    int byte_offset = 0;
    SHR_BITDCL tmp_storage[1] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    
    byte_offset = (idx_bit / SHR_BITWID) * sizeof(SHR_BITDCL);
    
    
    bit_offset  = (idx_bit % SHR_BITWID);

    DNX_SW_STATE_MEMREAD(unit, tmp_storage, location, byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

    *(bit_result) = (SHR_BITGET(tmp_storage, bit_offset) > 0) ? 1: 0;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;}


int dnxc_sw_state_bitmap_range_read(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 source_first,
    SHR_BITDCL *result_location,
    uint32 result_first,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{

    SHR_FUNC_INIT_VARS(unit);


    
    SHR_BITCOPY_RANGE(result_location, result_first, location, source_first, range);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_write(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    SHR_BITDCL *source_location,
    uint32 source_first,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, source_first, range, SW_STATE_BITMAP_RANGE_WRITE, flags, dbg_string ));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



int dnxc_sw_state_bitmap_range_and(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    SHR_BITDCL *source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_AND, flags, dbg_string ));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_or(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    SHR_BITDCL *source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_OR, flags, dbg_string ));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_xor(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    SHR_BITDCL *source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_XOR, flags, dbg_string ));


    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_remove(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    SHR_BITDCL *source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_REMOVE, flags, dbg_string ));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_negate(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, NULL, 0, range, SW_STATE_BITMAP_RANGE_NEGATE, flags, dbg_string ));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, NULL, 0, range, SW_STATE_BITMAP_RANGE_CLEAR, flags, dbg_string ));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

int dnxc_sw_state_bitmap_range_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, location, dest_first, range, SW_STATE_BITMAP_RANGE_SET, flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_null(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, NULL, range, result, SW_STATE_BITMAP_RANGE_NULL, flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_test(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    uint32 range,
    uint8 *result,
    uint32 _flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, NULL, range, result, SW_STATE_BITMAP_RANGE_TEST, _flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}



int dnxc_sw_state_bitmap_range_eq(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    SHR_BITDCL *source_location,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, source_location, range, result, SW_STATE_BITMAP_RANGE_EQ, flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


int dnxc_sw_state_bitmap_range_count(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 dest_first,
    uint32 range,
    int *result,
    uint32 flags,
    char *dbg_string)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, NULL, range, (uint8 *)(result), SW_STATE_BITMAP_RANGE_COUNT, flags, dbg_string));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#undef _ERR_MSG_MODULE_NAME
