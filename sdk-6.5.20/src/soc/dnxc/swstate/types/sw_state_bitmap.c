
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <shared/bitop.h>
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
    uint32 dest_offset,
    SHR_BITDCL *src,
    uint32 src_offset,
    uint32 range,
    uint8 *result,
    sw_state_bitmap_range_op_t op_type,
    uint32 flags,
    char *dbg_string)
{
    uint32 byte_offset = 0;
    uint32 bit_offset = 0;

    uint32 tmp_storage_size = 0;
    SHR_BITDCL *tmp_storage = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    tmp_storage_size = (_SHR_BITDCLSIZE(range) + 1) * sizeof(SHR_BITDCL);

    tmp_storage = (SHR_BITDCL *)sal_alloc(tmp_storage_size, "bitmap range bit result");

    if(NULL == tmp_storage) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: failed to allocate memory\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    sal_memset(tmp_storage, 0x0, tmp_storage_size);

    
    byte_offset = (dest_offset / SHR_BITWID) * sizeof(SHR_BITDCL);

    
    bit_offset  = (dest_offset % SHR_BITWID);

    
    DNX_SW_STATE_MEMREAD(unit, tmp_storage, dest, byte_offset, tmp_storage_size, flags, dbg_string);

    
    switch(op_type) {
        case SW_STATE_BITMAP_RANGE_NULL:
        *(result) = (SHR_BITNULL_RANGE(tmp_storage, bit_offset, range) > 0) ? 1: 0;
        break;
        case SW_STATE_BITMAP_RANGE_TEST:
        SHR_BITTEST_RANGE(tmp_storage, bit_offset, range, *(result));
        break;
        case SW_STATE_BITMAP_RANGE_EQ:
        *(result) = (SHR_BITEQ_RANGE(src, tmp_storage, bit_offset, range) > 0) ? 1: 0;
        break;
        case SW_STATE_BITMAP_RANGE_COUNT:
        shr_bitop_range_count(tmp_storage, bit_offset, range, (int *)result);
        break;
        default:
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state bitmap ERROR: unknown bitmap operation detected\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    sal_free(tmp_storage);
    tmp_storage = NULL;

exit:
    if(NULL != tmp_storage)
    {
        sal_free(tmp_storage);
    }
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

    uint32 tmp_storage_size = 0;
    SHR_BITDCL *tmp_storage = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    tmp_storage_size = (_SHR_BITDCLSIZE(range) + 1) * sizeof(SHR_BITDCL);

    tmp_storage = (SHR_BITDCL *)sal_alloc(tmp_storage_size, "bitmap range change");

    if(NULL == tmp_storage) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: failed to allocate memory\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    byte_offset = (dest_offset / SHR_BITWID) * sizeof(SHR_BITDCL);

    
    bit_offset  = (dest_offset % SHR_BITWID);

    
    DNX_SW_STATE_MEMREAD(unit, tmp_storage, dest, byte_offset, tmp_storage_size, flags, dbg_string);

    
    switch(op_type) {
        case SW_STATE_BITMAP_RANGE_CLEAR:
        SHR_BITCLR_RANGE(tmp_storage, bit_offset, range);
        break;
        case SW_STATE_BITMAP_RANGE_SET:
        SHR_BITSET_RANGE(tmp_storage, bit_offset, range);
        break;
        case SW_STATE_BITMAP_RANGE_WRITE:
        SHR_BITCOPY_RANGE(tmp_storage, bit_offset, src, src_offset, range);
        break;
        case SW_STATE_BITMAP_RANGE_AND:
        SHR_BITAND_RANGE(src, tmp_storage, bit_offset, range, tmp_storage);
        break;
        case SW_STATE_BITMAP_RANGE_OR:
        SHR_BITOR_RANGE(src, tmp_storage, bit_offset, range, tmp_storage);
        break;
        case SW_STATE_BITMAP_RANGE_XOR:
        SHR_BITXOR_RANGE(src, tmp_storage, bit_offset, range, tmp_storage);
        break;
        case SW_STATE_BITMAP_RANGE_REMOVE:
        SHR_BITREMOVE_RANGE(src, tmp_storage, bit_offset, range, tmp_storage);
        break;
        case SW_STATE_BITMAP_RANGE_NEGATE:
        SHR_BITNEGATE_RANGE(tmp_storage, bit_offset, range, tmp_storage);
        break;
        default:
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state bitmap ERROR: unknown bitmap operation detected\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, tmp_storage, 0, dest, byte_offset, tmp_storage_size, flags, dbg_string);

    
    sal_free(tmp_storage);
    tmp_storage = NULL;

exit:
    if(NULL != tmp_storage)
    {
        sal_free(tmp_storage);
    }
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
    SHR_BITDCL tmp_storage = 0; 

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    byte_offset = (idx_bit / SHR_BITWID) * sizeof(SHR_BITDCL);

    
    bit_offset  = (idx_bit % SHR_BITWID);
    
    
    DNX_SW_STATE_MEMREAD(unit, &tmp_storage, location, byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

    
    if( 0 == value ) {
        SHR_BITCLR((&tmp_storage), bit_offset);
    } else {
        SHR_BITSET((&tmp_storage), bit_offset);
    }

    
    DNX_SW_STATE_MEMWRITE_BASIC(unit, module_id, (uint8 *)(&tmp_storage), 0, (uint8 *)(location), byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_bitmap_alloc(
    int unit,
    uint32 module_id,
    uint8 **location,
    uint32 nof_bits_to_alloc,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNX_SW_STATE_ALLOC_BASIC(unit, module_id, location, _SHR_BITDCLSIZE(nof_bits_to_alloc) + 1, sizeof(SHR_BITDCL), flags | DNXC_SW_STATE_ALLOC_BITMAP_FLAG, dbg_string);

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_bitmap_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_change(unit, module_id, location, idx_bit, 1, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}


int dnxc_sw_state_bitmap_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL *location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_change(unit, module_id, location, idx_bit, 0, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
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
    SHR_BITDCL tmp_storage = 0;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    
    byte_offset = (idx_bit / SHR_BITWID) * sizeof(SHR_BITDCL);
    
    
    bit_offset  = (idx_bit % SHR_BITWID);

    DNX_SW_STATE_MEMREAD(unit, &tmp_storage, location, byte_offset, sizeof(SHR_BITDCL), flags, dbg_string);

    *(bit_result) = (SHR_BITGET(&tmp_storage, bit_offset) > 0) ? 1: 0;

    DNXC_SW_STATE_FUNC_RETURN;
}


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

    uint32 offset = 0;
    SHR_BITDCL *tmp_storage = NULL;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    tmp_storage = (SHR_BITDCL *)sal_alloc(SHR_BITALLOCSIZE(range + SHR_BITWID), "bitmap range read");

    if(NULL == tmp_storage) {
        SHR_IF_ERR_EXIT_WITH_LOG(SOC_E_INTERNAL, "sw state ERROR: failed to allocate memory\n%s%s%s", EMPTY, EMPTY, EMPTY);
    }

    
    offset = (source_first / SHR_BITWID) * sizeof(SHR_BITDCL);
    DNX_SW_STATE_MEMREAD(unit, tmp_storage, location, offset, SHR_BITALLOCSIZE(range + SHR_BITWID), flags, dbg_string);

    
    SHR_BITCOPY_RANGE(result_location, result_first, tmp_storage, source_first % SHR_BITWID, range);

    sal_free(tmp_storage);

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, source_first, range, SW_STATE_BITMAP_RANGE_WRITE, flags, dbg_string ));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_AND, flags, dbg_string ));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_OR, flags, dbg_string ));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_XOR, flags, dbg_string ));


    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, source_location, 0, range, SW_STATE_BITMAP_RANGE_REMOVE, flags, dbg_string ));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, NULL, 0, range, SW_STATE_BITMAP_RANGE_NEGATE, flags, dbg_string ));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, NULL, 0, range, SW_STATE_BITMAP_RANGE_CLEAR, flags, dbg_string ));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_change(unit, module_id, location, dest_first, location, dest_first, range, SW_STATE_BITMAP_RANGE_SET, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, NULL, 0, range, result, SW_STATE_BITMAP_RANGE_NULL, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, NULL, 0, range, result, SW_STATE_BITMAP_RANGE_TEST, _flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, source_location, 0, range, result, SW_STATE_BITMAP_RANGE_EQ, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
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
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_bit_result(unit, module_id, location, dest_first, NULL, 0, range, (uint8 *)(result), SW_STATE_BITMAP_RANGE_COUNT, flags, dbg_string));

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
