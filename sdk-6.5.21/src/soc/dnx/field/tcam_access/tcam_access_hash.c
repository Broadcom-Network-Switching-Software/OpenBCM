
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_bank_types.h>
#include "tcam_access_defines.h"
#include "tcam_access_shadow.h"
#include "tcam_access_hash.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

typedef enum
{
    DNX_FIELD_TCAM_ACCESS_HASH_ARR_INVALID = -1,
    DNX_FIELD_TCAM_ACCESS_HASH_ARR_HEAD,
    DNX_FIELD_TCAM_ACCESS_HASH_ARR_NEXT,
} dnx_field_tcam_access_hash_arr_e;

typedef struct
{
    dnx_field_tcam_access_hash_arr_e arr;
    uint32 index;
} dnx_field_tcam_access_hash_ptr;

static shr_error_e
dnx_field_tcam_access_hash_ptr_set(
    int unit,
    int core,
    uint32 bank_id,
    dnx_field_tcam_access_hash_ptr * iter,
    uint32 value)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (iter->arr)
    {
        case DNX_FIELD_TCAM_ACCESS_HASH_ARR_HEAD:
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.key_2_location_hash.
                            head.set(unit, core, bank_id, iter->index, value));
            break;
        case DNX_FIELD_TCAM_ACCESS_HASH_ARR_NEXT:
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.key_2_location_hash.
                            next.set(unit, core, bank_id, iter->index, value));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_FAIL, "dnx_field_tcam_access_hash_ptr_set: Unkown or invalid array ptr type %d\n",
                         iter->arr);
    }

exit:
    SHR_FUNC_EXIT;
}

static uint32
crc32b(
    char *key,
    uint32 size)
{
    int i, j;
    uint32 byte, crc, mask;

    crc = 0xFFFFFFFF;
    for (i = 0; i < size; i++)
    {
        byte = key[i];
        crc = crc ^ byte;
        for (j = 7; j >= 0; j--)
        {
            mask = -(crc & 1);
            crc = (crc >> 1) ^ (0xEDB88320 & mask);
        }
    }
    return ~crc;
}

static shr_error_e
dnx_field_tcam_access_hash_key_find_internal(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    dnx_field_tcam_access_hash_ptr * last_iter,
    dnx_field_tcam_access_hash_ptr * next_iter,
    uint32 *index,
    uint8 *found)
{
    uint32 key_with_mask[2 * MAX_DOUBLE_KEY_WORD_SIZE];
    uint32 hash;
    uint32 cur_index;
    uint32 next_index;
    uint8 equal;

    SHR_FUNC_INIT_VARS(unit);

    *found = FALSE;

    SHR_BITAND_RANGE(key, key_mask, 0, key_size, key);

    SHR_BITCOPY_RANGE(key_with_mask, 0, key, 0, key_size);
    SHR_BITCOPY_RANGE(key_with_mask, key_size, key_mask, 0, key_size);

    hash = crc32b((char *) key_with_mask, 2 * BITS2BYTES(key_size)) % FIELD_TCAM_BANK_NOF_ENTRIES(bank_id);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.key_2_location_hash.
                    head.get(unit, core, bank_id, hash, &cur_index));

    if (last_iter != NULL)
    {
        last_iter->arr = DNX_FIELD_TCAM_ACCESS_HASH_ARR_HEAD;
        last_iter->index = hash;
    }
    if (next_iter != NULL)
    {
        next_iter->arr = DNX_FIELD_TCAM_ACCESS_HASH_ARR_INVALID;
    }
    if (cur_index == DNX_FIELD_TCAM_ACCESS_HASH_NULL)
    {

        SHR_EXIT();
    }
    if (next_iter != NULL)
    {
        next_iter->arr = DNX_FIELD_TCAM_ACCESS_HASH_ARR_NEXT;
        next_iter->index = cur_index;
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_equal_shadow
                    (unit, core, key, key_mask, key_size, bank_id, cur_index, FALSE, &equal));
    if (equal)
    {

        *found = TRUE;
        if (index != NULL)
        {
            *index = cur_index;
        }
        SHR_EXIT();
    }

    do
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.key_2_location_hash.
                        next.get(unit, core, bank_id, cur_index, &next_index));
        if (next_index == DNX_FIELD_TCAM_ACCESS_HASH_NULL)
        {
            if (next_iter != NULL)
            {
                next_iter->arr = DNX_FIELD_TCAM_ACCESS_HASH_ARR_INVALID;
            }

            break;
        }
        if (next_iter != NULL)
        {
            next_iter->arr = DNX_FIELD_TCAM_ACCESS_HASH_ARR_NEXT;
            next_iter->index = next_index;
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_equal_shadow
                        (unit, core, key, key_mask, key_size, bank_id, next_index, FALSE, &equal));
        if (equal)
        {
            *found = TRUE;
            if (index != NULL)
            {
                *index = next_index;
            }
            break;
        }

        cur_index = next_index;
    }
    while (TRUE);

    if (last_iter != NULL)
    {
        last_iter->arr = DNX_FIELD_TCAM_ACCESS_HASH_ARR_NEXT;
        last_iter->index = cur_index;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_hash_key_insert_at_index(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 index)
{
    uint32 found_index;
    uint8 found;
    dnx_field_tcam_access_hash_ptr last_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_hash_key_find_internal
                    (unit, core, bank_id, key, key_mask, key_size, &last_iter, NULL, &found_index, &found));

    if (found)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS,
                     "Error: Failed adding given entry to hash, entry already exists at bank: %d offset: %d\n", bank_id,
                     found_index);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_hash_ptr_set(unit, core, bank_id, &last_iter, index));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_hash_key_find(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 *index,
    uint8 *found)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_hash_key_find_internal
                    (unit, core, bank_id, key, key_mask, key_size, NULL, NULL, index, found));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_hash_delete_with_key(
    int unit,
    int core,
    uint32 bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size)
{
    dnx_field_tcam_access_hash_ptr last_iter;
    dnx_field_tcam_access_hash_ptr next_iter;
    uint8 found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_hash_key_find_internal
                    (unit, core, bank_id, key, key_mask, key_size, &last_iter, &next_iter, NULL, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error updating/deleting TCAM entry: entry not found in hash\n");
    }

    if (next_iter.arr != DNX_FIELD_TCAM_ACCESS_HASH_ARR_INVALID)
    {
        uint32 position;

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.key_2_location_hash.
                        next.get(unit, core, bank_id, next_iter.index, &position));
        dnx_field_tcam_access_hash_ptr_set(unit, core, bank_id, &last_iter, position);

        dnx_field_tcam_access_hash_ptr_set(unit, core, bank_id, &next_iter, DNX_FIELD_TCAM_ACCESS_HASH_NULL);
    }
    else
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_access_hash_ptr_set
                        (unit, core, bank_id, &last_iter, DNX_FIELD_TCAM_ACCESS_HASH_NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_hash_delete(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 key_size)
{
    uint32 key_32[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW) * 2] = { 0 };
    uint32 mask_32[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW) * 2] = { 0 };
    uint8 odd = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    if (bank_offset % 2)
    {

        if (key_size != HALF_KEY_BIT_SIZE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Odd bank offsets work only with half key sizes\n");
        }
        odd = TRUE;
        bank_offset--;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_decoded_read_shadow(unit, core, bank_id, bank_offset, key_32, mask_32));
    if (key_size == DOUBLE_KEY_BIT_SIZE)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_decoded_read_shadow
                        (unit, core, bank_id + 1, bank_offset, key_32 + SINGLE_KEY_WORD_SIZE,
                         mask_32 + SINGLE_KEY_WORD_SIZE));
    }
    if (odd)
    {

        SHR_BITCOPY_RANGE(key_32, 0, key_32, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
        SHR_BITCOPY_RANGE(mask_32, 0, mask_32, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_hash_delete_with_key(unit, core, bank_id, key_32, mask_32, key_size));

exit:
    SHR_FUNC_EXIT;
}
