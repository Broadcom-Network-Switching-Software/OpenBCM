
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_cache_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <shared/shrextend/shrextend_debug.h>
#include "tcam_access_defines.h"
#include "tcam_access_shadow.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_field_tcam_access_entry_key_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    uint32 *data,
    uint8 *valid_bits)
{
    uint8 shadow_data[BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)];
    uint32 shadow_data_32[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)];
    uint32 entry_half_key_size;

    uint32 valid_bits_arr[1];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));
    entry_half_key_size = HALF_KEY_BIT_SIZE_HW;

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.entry_key.
                    key_data.range_read(unit, core, bank_id, tcam_bank_offset,
                                        BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW), shadow_data));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32
                    (shadow_data, BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW), shadow_data_32));

    valid_bits_arr[0] = 0;
    SHR_BITCOPY_RANGE(valid_bits_arr, 0, shadow_data_32, 2 * entry_half_key_size, 2);
    (*valid_bits) = valid_bits_arr[0];

    SHR_BITCOPY_RANGE(data, 0, shadow_data_32, 0, 2 * entry_half_key_size);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_key_write_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    int valid,
    dnx_field_tcam_access_part_e part,
    uint32 *data)
{
    uint8 shadow_data[BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)];
    uint32 shadow_data_32[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)];
    uint8 shadow_read_data[BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)];
    uint32 entry_half_key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));
    sal_memset(shadow_data_32, 0, sizeof(shadow_data_32));
    sal_memset(shadow_read_data, 0, sizeof(shadow_read_data));
    entry_half_key_size = HALF_KEY_BIT_SIZE_HW;

    if (part == DNX_FIELD_TCAM_ACCESS_PART_LSB || part == DNX_FIELD_TCAM_ACCESS_PART_MSB)
    {

        SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.entry_key.
                        key_data.range_read(unit, core, bank_id, tcam_bank_offset,
                                            BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW),
                                            shadow_read_data));
        SHR_IF_ERR_EXIT(utilex_U8_to_U32
                        (shadow_read_data, BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW),
                         shadow_data_32));
    }

    switch (part)
    {
        case DNX_FIELD_TCAM_ACCESS_PART_LSB:
        {
            SHR_BITCOPY_RANGE(shadow_data_32, 0, data, 0, entry_half_key_size);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size, valid & DNX_FIELD_TCAM_ACCESS_PART_LSB);
            break;
        }
        case DNX_FIELD_TCAM_ACCESS_PART_MSB:
        {
            SHR_BITCOPY_RANGE(shadow_data_32, entry_half_key_size, data, entry_half_key_size, entry_half_key_size);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size + 1, (valid & DNX_FIELD_TCAM_ACCESS_PART_MSB) >> 1);
            break;
        }
        case DNX_FIELD_TCAM_ACCESS_PART_ALL:
        {
            SHR_BITCOPY_RANGE(shadow_data_32, 0, data, 0, 2 * entry_half_key_size);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size, valid & DNX_FIELD_TCAM_ACCESS_PART_LSB);
            SHR_BITWRITE(shadow_data_32, 2 * entry_half_key_size + 1, (valid & DNX_FIELD_TCAM_ACCESS_PART_MSB) >> 1);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown entry part: %d.\n", part);
            break;
        }
    }

    SHR_IF_ERR_EXIT(utilex_U32_to_U8
                    (shadow_data_32, BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW), shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.entry_key.
                    key_data.range_write(unit, core, bank_id, tcam_bank_offset,
                                         BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW),
                                         shadow_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_payload_read_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload)
{
    uint8 shadow_data[BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF)];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.entry_payload.
                    payload_data.range_read(unit, core, action_bank_id, action_bank_offset,
                                            BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF), shadow_data));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_data, BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF), payload));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_payload_write_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload)
{
    uint8 shadow_data[BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF)];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(payload, BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF), shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.entry_payload.
                    payload_data.range_write(unit, core, action_bank_id, action_bank_offset,
                                             BITS2BYTES(DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF), shadow_data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_decoded_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask)
{
    uint8 valid_bits_dummy;

    SHR_FUNC_INIT_VARS(unit);

    if (bank_offset % 2)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Entry should start at even offset, offset %d invalid\n", bank_offset);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                    (unit, core, bank_id, bank_offset, key, &valid_bits_dummy));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                    (unit, core, bank_id, bank_offset + 1, key_mask, &valid_bits_dummy));

    SHR_BITCOPY_RANGE(key, 0, key, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, 0, key_mask, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key, HALF_KEY_BIT_SIZE, key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, HALF_KEY_BIT_SIZE, key_mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      HALF_KEY_BIT_SIZE);

    SHR_BITOR_RANGE(key_mask, key, 0, SINGLE_KEY_BIT_SIZE, key_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_equal_shadow(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 bank_id,
    uint32 bank_offset,
    uint8 skip_entry_in_use,
    uint8 *equal)
{
    uint8 entry_in_use;
    uint32 position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
    uint8 odd_comparison = FALSE;
    uint32 shadow_key_32[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)] = { 0 };
    uint32 shadow_mask_32[BITS2WORDS(DNX_DATA_MAX_FIELD_TCAM_ENTRY_SIZE_SINGLE_KEY_SHADOW)] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    *equal = FALSE;

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_get(unit, core, position, &entry_in_use));
    if (!skip_entry_in_use && !entry_in_use)
    {
        SHR_EXIT();
    }

    if (key_size == DOUBLE_KEY_BIT_SIZE)
    {

        if (bank_id % 2 == 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Double-key size need to start on even bank ID\n");
        }
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Double-key comparision, comparing second half in odd bank ID%s%s%s%s\n", EMPTY,
                     EMPTY, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_equal_shadow
                        (unit, core, key + SINGLE_KEY_WORD_SIZE, key_mask + SINGLE_KEY_WORD_SIZE, key_size / 2,
                         bank_id + 1, bank_offset, TRUE, equal));
        if (!*equal)
        {
            SHR_EXIT();
        }

        key_size /= 2;
    }

    if (bank_offset % 2)
    {
        if (key_size != HALF_KEY_BIT_SIZE)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Non half-key sizes are for sure not equal to an entry located in an odd offset (%d)%s%s%s\n",
                         bank_offset, EMPTY, EMPTY, EMPTY);
            *equal = FALSE;
            SHR_EXIT();
        }

        bank_offset--;
        odd_comparison = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_decoded_read_shadow
                    (unit, core, bank_id, bank_offset, shadow_key_32, shadow_mask_32));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Key(160b):        0x%08x%08x%08x%08x\n", key[4], key[3], key[2], key[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Key(160b):          %08x%s%s%s\n", key[0], EMPTY, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Mask(160b):       0x%08x%08x%08x%08x\n", key_mask[4], key_mask[3],
                 key_mask[2], key_mask[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Mask(160b):         %08x%s%s%s\n", key_mask[0], EMPTY, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "-------------------With-------------- >>> key size: %d odd_comparison: %d bank_id: %d bank_offset: %d <<< \n",
                 key_size, odd_comparison, bank_id, bank_offset);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Shadow_Key(160b): 0x%08x%08x%08x%08x\n", shadow_key_32[4],
                 shadow_key_32[3], shadow_key_32[2], shadow_key_32[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Shadow_Key(160b):   %08x%s%s%s\n", shadow_key_32[0], EMPTY, EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Shadow_Mask(160b): 0x%08x%08x%08x%08x\n", shadow_mask_32[4],
                 shadow_mask_32[3], shadow_mask_32[2], shadow_mask_32[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Comparing: Shadow_Mask(160b):   %08x%s%s%s\n", shadow_mask_32[0], EMPTY, EMPTY,
                 EMPTY);

    if (odd_comparison)
    {

        SHR_BITCOPY_RANGE(shadow_key_32, 0, shadow_key_32, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
        SHR_BITCOPY_RANGE(shadow_mask_32, 0, shadow_mask_32, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
    }

    if (key_size == HALF_KEY_BIT_SIZE)
    {

        SHR_BITCLR_RANGE(shadow_key_32, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
        SHR_BITCLR_RANGE(shadow_mask_32, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
        SHR_BITCLR_RANGE(key, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
        SHR_BITCLR_RANGE(key_mask, HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
        key_size = SINGLE_KEY_BIT_SIZE;
    }

    if (!sal_memcmp(key, shadow_key_32, BITS2BYTES(key_size))
        && !sal_memcmp(key_mask, shadow_mask_32, BITS2BYTES(key_size)))
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, ">>> Key,mask pair are equal to the entry <<<%s%s%s%s\n\n", EMPTY, EMPTY, EMPTY,
                     EMPTY);
        *equal = TRUE;
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, ">>> Key,mask pair NOT equal to the entry <<<%s%s%s%s\n\n", EMPTY, EMPTY, EMPTY,
                     EMPTY);
        *equal = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}
