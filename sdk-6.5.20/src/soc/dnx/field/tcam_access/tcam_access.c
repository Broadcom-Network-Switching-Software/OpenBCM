
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <bcm_int/dnx/field/tcam/tcam_location_algo.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/field_tcam.h>

#include <soc/dnx/field/tcam_access/tcam_access.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_mapper_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_cache_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>

#include <shared/utilex/utilex_bitstream.h>
#include <soc/register.h>
#include <soc/feature.h>
#include <soc/drv.h> 

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX


#define KEY_MODE_BIT_SIZE \
    dnx_data_field.tcam.key_mode_size_get(unit)


#define SEARCH_KEY_IN_TCAM_HW FALSE


#define ACCESS_HW TRUE


#define ACCESS_SW_MOCK TRUE


#define HALF_KEY_BIT_SIZE \
    dnx_data_field.tcam.key_size_half_get(unit)
#define SINGLE_KEY_BIT_SIZE \
    dnx_data_field.tcam.key_size_single_get(unit)
#define DOUBLE_KEY_BIT_SIZE \
    dnx_data_field.tcam.key_size_double_get(unit)
#define HALF_ACTION_BIT_SIZE \
    dnx_data_field.tcam.action_size_half_get(unit)
#define SINGLE_ACTION_BIT_SIZE \
    dnx_data_field.tcam.action_size_single_get(unit)
#define DOUBLE_ACTION_BIT_SIZE \
    dnx_data_field.tcam.action_size_double_get(unit)
#define HALF_KEY_WORD_SIZE \
    (BITS2WORDS(HALF_KEY_BIT_SIZE))
#define SINGLE_KEY_WORD_SIZE \
    (BITS2WORDS(SINGLE_KEY_BIT_SIZE))
#define DOUBLE_KEY_WORD_SIZE \
    (BITS2WORDS(DOUBLE_KEY_BIT_SIZE))
#define HALF_ACTION_WORD_SIZE \
    (BITS2WORDS(HALF_ACTION_BIT_SIZE))
#define SINGLE_ACTION_WORD_SIZE \
    (BITS2WORDS(SINGLE_ACTION_BIT_SIZE))
#define DOUBLE_ACTION_WORD_SIZE \
    (BITS2WORDS(DOUBLE_ACTION_BIT_SIZE))

#define MAX_HALF_KEY_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF
#define MAX_SINGLE_KEY_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE
#define MAX_DOUBLE_KEY_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_DOUBLE
#define MAX_HALF_ACTION_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_HALF
#define MAX_SINGLE_ACTION_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_SINGLE
#define MAX_DOUBLE_ACTION_BIT_SIZE \
    DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_DOUBLE
#define MAX_HALF_KEY_WORD_SIZE \
    (BITS2WORDS(MAX_HALF_KEY_BIT_SIZE))
#define MAX_SINGLE_KEY_WORD_SIZE \
    (BITS2WORDS(MAX_SINGLE_KEY_BIT_SIZE))
#define MAX_DOUBLE_KEY_WORD_SIZE \
    (BITS2WORDS(MAX_DOUBLE_KEY_BIT_SIZE))
#define MAX_HALF_ACTION_WORD_SIZE \
    (BITS2WORDS(MAX_HALF_ACTION_BIT_SIZE))
#define MAX_SINGLE_ACTION_WORD_SIZE \
    (BITS2WORDS(MAX_SINGLE_ACTION_BIT_SIZE))
#define MAX_DOUBLE_ACTION_WORD_SIZE \
    (BITS2WORDS(MAX_DOUBLE_ACTION_BIT_SIZE))

#define DNX_FIELD_TCAM_IS_STAGE_PMF(stage) \
    (((stage) == DNX_FIELD_TCAM_STAGE_IPMF1) || \
    ((stage) == DNX_FIELD_TCAM_STAGE_IPMF2) || \
    ((stage) == DNX_FIELD_TCAM_STAGE_IPMF3) || \
    ((stage) == DNX_FIELD_TCAM_STAGE_EPMF))


#define HALF_KEY_BIT_SIZE_HW \
    (dnx_data_field.tcam.entry_size_single_key_hw_get(unit) / 2)


typedef enum
{
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_INVALID = 2,
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF = 0,
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_SINGLE = 1,
    DNX_FIELD_TCAM_ACCESS_KEY_MODE_DOUBLE = 3,
} dnx_field_tcam_access_key_mode_e;


typedef enum
{
    DNX_FIELD_TCAM_ACCESS_PART_INVALID = 0,
    DNX_FIELD_TCAM_ACCESS_PART_LSB = 1,
    DNX_FIELD_TCAM_ACCESS_PART_MSB = 2,
    DNX_FIELD_TCAM_ACCESS_PART_ALL = 3,
} dnx_field_tcam_access_part_e;


typedef struct
{
        
    uint8 direct_table;
    
    uint32 key_size;
    
    uint32 payload_size;
    
    uint32 prefix_value;
    
    uint32 prefix_size;
    
    int dt_bank_id;
    
    dnx_field_tcam_stage_e stage;

} dnx_field_tcam_access_db_info;


static shr_error_e
dnx_field_tcam_access_info_get(
    int unit,
    int fg_id,
    dnx_field_tcam_access_db_info * db_info)
{
    dnx_field_tcam_access_fg_params_t fg_params;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.get(unit, fg_id, &fg_params));
    db_info->key_size = fg_params.key_size;
    db_info->payload_size = fg_params.action_size;
    db_info->prefix_value = fg_params.prefix_value;
    db_info->prefix_size = fg_params.prefix_size;
    db_info->direct_table = fg_params.direct_table;
    db_info->dt_bank_id = fg_params.dt_bank_id;
    db_info->stage = fg_params.stage;

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_index_get(
    int unit,
    int core,
    uint32 access_id,
    uint32 *position)
{
    uint8 found;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_location_hash.find(unit, core, &access_id, position, &found));

    if (!found)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Entry with access_id: %d not found in entry->location Hash %s%s%s\n", access_id,
                     EMPTY, EMPTY, EMPTY);
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_key_read_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    uint32 *data,
    uint8 *valid_bits)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE];
    uint32 shadow_data_32[BYTES2WORDS(DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE)];
    uint32 entry_half_key_size;
    
    uint32 valid_bits_arr[1];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));
    entry_half_key_size = HALF_KEY_BIT_SIZE_HW;

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                    entry_key.range_read(unit, core, bank_id, tcam_bank_offset, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE,
                                         shadow_data));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_data, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE, shadow_data_32));

    valid_bits_arr[0] = 0;
    SHR_BITCOPY_RANGE(valid_bits_arr, 0, shadow_data_32, 2 * entry_half_key_size, 2);
    (*valid_bits) = valid_bits_arr[0];

    SHR_BITCOPY_RANGE(data, 0, shadow_data_32, 0, 2 * entry_half_key_size);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_key_write_shadow(
    int unit,
    int core,
    uint32 bank_id,
    uint32 tcam_bank_offset,
    int valid,
    dnx_field_tcam_access_part_e part,
    uint32 *data)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE];
    uint32 shadow_data_32[BYTES2WORDS(DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE)];
    uint8 shadow_read_data[DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE_MAX_ALL_DEVICE];
    uint32 entry_half_key_size;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));
    sal_memset(shadow_data_32, 0, sizeof(shadow_data_32));
    sal_memset(shadow_read_data, 0, sizeof(shadow_read_data));
    entry_half_key_size = HALF_KEY_BIT_SIZE_HW;

    if (part == DNX_FIELD_TCAM_ACCESS_PART_LSB || part == DNX_FIELD_TCAM_ACCESS_PART_MSB)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                        entry_key.range_read(unit, core, bank_id, tcam_bank_offset, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE,
                                             shadow_read_data));
        SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_read_data, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE, shadow_data_32));
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

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(shadow_data_32, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE, shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.tcam_bank.
                    entry_key.range_write(unit, core, bank_id, tcam_bank_offset, DNX_FIELD_TCAM_ENTRY_KEY_HW_SIZE,
                                          shadow_data));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_payload_read_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE_MAX_ALL_DEVICE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.
                    entry_payload.range_read(unit, core, action_bank_id, action_bank_offset,
                                             DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, shadow_data));

    SHR_IF_ERR_EXIT(utilex_U8_to_U32(shadow_data, DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, payload));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_payload_write_shadow(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 action_bank_offset,
    uint32 *payload)
{
    uint8 shadow_data[DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE_MAX_ALL_DEVICE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(shadow_data, 0, sizeof(shadow_data));

    SHR_IF_ERR_EXIT(utilex_U32_to_U8(payload, DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, shadow_data));

    SHR_IF_ERR_EXIT(dnx_field_tcam_cache_shadow.payload_table.
                    entry_payload.range_write(unit, core, action_bank_id, action_bank_offset,
                                              DNX_FIELD_TCAM_ENTRY_HALF_PAYLOAD_HW_SIZE, shadow_data));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_key_read(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    dnx_field_tcam_access_part_e part,
    uint32 *key,
    uint32 *key_mask,
    uint8 *valid_bits)
{
    
    uint32 data_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 data_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    soc_reg_above_64_val_t mem_val_read;
    soc_reg_above_64_val_t mem_val_write;
    uint32 tcam_bank_address;
    uint8 key_valid_bits;
    uint8 shadow_valid_bits;

    SHR_FUNC_INIT_VARS(unit);

    
    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    if (DNX_FIELD_TCAM_SHADOW_READ)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset, data_key, &key_valid_bits));

        
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset + 1, data_mask, &shadow_valid_bits));

    }
    else if (ACCESS_HW)
    {
        SOC_REG_ABOVE_64_CLEAR(mem_val_write);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_WRf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_RDf, 0x1);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_CMPf, 0x0);
        
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_write));

        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_read));

        key_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_key);

        
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm
                        (unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_write));

        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_read));

        shadow_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_mask);

    }

    if (key_valid_bits != shadow_valid_bits)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Valid bits of read key/mask are differnet, key valid bits: %d, mask valid bits: %d\n",
                     key_valid_bits, shadow_valid_bits);
    }
    *valid_bits = key_valid_bits;

    
    SHR_BITCOPY_RANGE(key, 0, data_key, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key, HALF_KEY_BIT_SIZE, data_key, 2 * KEY_MODE_BIT_SIZE + HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, 0, data_mask, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, HALF_KEY_BIT_SIZE, data_mask,
                      2 * KEY_MODE_BIT_SIZE + HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);

    
    
    SHR_BITOR_RANGE(key_mask, key, 0, SINGLE_KEY_BIT_SIZE, key_mask);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_key_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid)
{
    soc_reg_above_64_val_t mem_val;
    
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 data_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 neg_data_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 data_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 tcam_bank_address;

    SHR_FUNC_INIT_VARS(unit);

    
    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    
    if (part & DNX_FIELD_TCAM_ACCESS_PART_LSB)
    {
        
        SHR_BITCOPY_RANGE(data_key, 0, &key_mode, 0, KEY_MODE_BIT_SIZE);
        
        SHR_BITCOPY_RANGE(data_key, KEY_MODE_BIT_SIZE, key, 0, HALF_KEY_BIT_SIZE);

        
        SHR_BITCOPY_RANGE(data_mask, KEY_MODE_BIT_SIZE, key_mask, 0, HALF_KEY_BIT_SIZE);

        
        if (valid & DNX_FIELD_TCAM_ACCESS_PART_LSB)
        {
            
            SHR_BITSET_RANGE(data_mask, 0, KEY_MODE_BIT_SIZE);
        }
    }
    if (part & DNX_FIELD_TCAM_ACCESS_PART_MSB)
    {
        
        SHR_BITCOPY_RANGE(data_key, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, &key_mode, 0, KEY_MODE_BIT_SIZE);
        
        SHR_BITCOPY_RANGE(data_key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key, HALF_KEY_BIT_SIZE,
                          HALF_KEY_BIT_SIZE);
        
        SHR_BITCOPY_RANGE(data_mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key_mask, HALF_KEY_BIT_SIZE,
                          HALF_KEY_BIT_SIZE);

        
        if (valid & DNX_FIELD_TCAM_ACCESS_PART_MSB)
        {
            
            SHR_BITSET_RANGE(data_mask, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
        }
    }

    
    
    SHR_BITAND_RANGE(data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, encoded_key);

    
    SHR_BITNEGATE_RANGE(data_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, neg_data_key);
    SHR_BITAND_RANGE(neg_data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, encoded_mask);

    
    SHR_BITCOPY_RANGE(data, 0, encoded_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      encoded_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, part);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);

    
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, valid);

    if (ACCESS_HW)
    {
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));
    }

    if (DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_write_shadow
                        (unit, core, bank_id, bank_offset, valid, part, encoded_key));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_write_shadow
                        (unit, core, bank_id, bank_offset + 1, valid, part, encoded_mask));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_payload_read(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_FIELD_TCAM_SHADOW_READ)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read_shadow
                        (unit, core, action_bank_id, bank_offset, payload));
    }
    else if (ACCESS_HW)
    {
        if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
        {
            action_tbl = TCAM_TCAM_ACTIONm;
        }
        else
        {
            action_tbl = TCAM_TCAM_ACTION_SMALLm;
        }

        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));

        soc_mem_field_get(unit, action_tbl, data, ACTIONf, payload);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_payload_write(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (ACCESS_HW)
    {
        if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
        {
            action_tbl = TCAM_TCAM_ACTIONm;
        }
        else
        {
            action_tbl = TCAM_TCAM_ACTION_SMALLm;
        }

        soc_mem_field_set(unit, action_tbl, data, ACTIONf, payload);

        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));
    }
    if (DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write_shadow
                        (unit, core, action_bank_id, bank_offset, payload));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_dt_read_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 payload[],
    uint32 ent_payload_size)
{
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 payload_bank_id;
    uint32 payload_bank_offset;
    dnx_field_tcam_access_db_info db_info;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    
    payload_bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position) * 2 + 1;
    payload_bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position) / 2;

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read
                    (unit, core, payload_bank_id, payload_bank_offset, payload_msb_aligned));

    
    SHR_BITCOPY_RANGE(payload, 0, payload_msb_aligned, db_info.payload_size - ent_payload_size, ent_payload_size);

    
    if (payload[0] == 0x0)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_dt_write_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 payload[],
    uint32 ent_payload_size,
    uint8 is_update)
{
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 payload_bank_id;
    uint32 payload_bank_offset;
    dnx_field_tcam_access_db_info db_info;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    
    SHR_BITCOPY_RANGE(payload_msb_aligned, db_info.payload_size - ent_payload_size, payload, 0, ent_payload_size);

    
    payload_bank_id = (DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position) * 2) + 1;
    payload_bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position) / 2;

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write
                    (unit, core, payload_bank_id, payload_bank_offset, payload_msb_aligned));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_size_to_mode(
    int unit,
    uint32 key_size,
    dnx_field_tcam_access_key_mode_e * key_mode)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_INVALID;
    if (key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF;
    }
    else if (key_size == dnx_data_field.tcam.key_size_single_get(unit))
    {
        *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_SINGLE;
    }
    else if (key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        *key_mode = DNX_FIELD_TCAM_ACCESS_KEY_MODE_DOUBLE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid key_size: %d", key_size);
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_single_entry_read(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 is_special,
    uint32 *key,
    uint32 *key_mask,
    uint32 *payload)
{
    uint32 action_bank_offset;
    uint32 action_lsb_bank_id;
    uint32 action_msb_bank_id;
    uint8 valid_bits;
    SHR_FUNC_INIT_VARS(unit);
    
    action_bank_offset = bank_offset / 2;

    
    if (!is_special && key_mode != DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF)
    {
        action_lsb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id + 1 : 2 * bank_id;
        action_msb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id : 2 * bank_id + 1;
    }
    else
    {
        action_lsb_bank_id = 2 * bank_id;
        action_msb_bank_id = 2 * bank_id + 1;
    }
    
    if (part & DNX_FIELD_TCAM_ACCESS_PART_LSB)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read
                        (unit, core, action_lsb_bank_id, action_bank_offset, payload));
    }
    
    if (!is_special && (part & DNX_FIELD_TCAM_ACCESS_PART_MSB))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_read
                        (unit, core, action_msb_bank_id, action_bank_offset, payload + HALF_ACTION_WORD_SIZE));
    }
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read
                    (unit, core, bank_id, bank_offset, part, key, key_mask, &valid_bits));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_get_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 ent_payload_size,
    uint32 key[],
    uint32 key_mask[],
    uint32 payload[])
{
    dnx_field_tcam_access_db_info db_info;
    uint32 bank_id;
    uint32 bank_offset;
    uint8 odd;
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 key_read[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 mask_read[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 payload_read[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    dnx_field_tcam_access_key_mode_e key_mode;
    dnx_field_tcam_access_part_e part;
    uint8 is_special = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    
    odd = (position % 2);
    if (odd && db_info.key_size != dnx_data_field.tcam.key_size_half_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Database %d with key_size %d can't start from position %d", app_id,
                     db_info.key_size, position);
    }

    
    if (db_info.key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        part = (odd) ? DNX_FIELD_TCAM_ACCESS_PART_MSB : DNX_FIELD_TCAM_ACCESS_PART_LSB;
    }
    else
    {
        part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
    }

    
    position = position - (position % 2);

    
    if (db_info.key_size == dnx_data_field.tcam.key_size_single_get(unit)
        && db_info.payload_size == dnx_data_field.tcam.action_size_half_get(unit))
    {
        is_special = TRUE;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_size_to_mode(unit, db_info.key_size, &key_mode));

    
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_read
                    (unit, core, bank_id, bank_offset, key_mode, part, is_special, key_read, mask_read, payload_read));

    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        
        uint32 *msb_key_part = key_read + SINGLE_KEY_WORD_SIZE;
        uint32 *msb_key_mask_part = mask_read + SINGLE_KEY_WORD_SIZE;
        uint32 *msb_payload_part = payload_read + SINGLE_ACTION_WORD_SIZE;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_read
                        (unit, core, bank_id + 1, bank_offset, key_mode, part, FALSE, msb_key_part,
                         msb_key_mask_part, msb_payload_part));
    }

    
    if (odd)
    {
        SHR_BITCOPY_RANGE(key, 0, key_read, db_info.key_size, db_info.key_size);
        SHR_BITCOPY_RANGE(key_mask, 0, mask_read, db_info.key_size, db_info.key_size);
        SHR_BITCOPY_RANGE(payload_msb_aligned, 0, payload_read, db_info.payload_size, db_info.payload_size);
    }
    else
    {
        SHR_BITCOPY_RANGE(key, 0, key_read, 0, db_info.key_size);
        SHR_BITCOPY_RANGE(key_mask, 0, mask_read, 0, db_info.key_size);
        SHR_BITCOPY_RANGE(payload_msb_aligned, 0, payload_read, 0, db_info.payload_size);
    }

    
    SHR_BITCOPY_RANGE(payload, 0, payload_msb_aligned, db_info.payload_size - ent_payload_size, ent_payload_size);

    
    if (db_info.prefix_size > 0)
    {
        SHR_BITCLR_RANGE(key, db_info.key_size - db_info.prefix_size, db_info.prefix_size);
        SHR_BITCLR_RANGE(key_mask, db_info.key_size - db_info.prefix_size, db_info.prefix_size);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Upper Key(128b) Read: 0x%08x%08x%08x%08x\n", key[4], key[3], key[2], key[1]);
        if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "Upper Key(128b) Read MSB: 0x%08x%08x%08x%08x\n", key[9], key[8], key[7], key[6]);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_tcam_hit_per_core_get(
    int unit,
    dnx_field_tcam_core_e core_id,
    uint32 entry_handle,
    uint8 *entry_hit_p)
{
    uint32 bank_id, bank_line_id, bank_sram_id, sram_line_entry_id;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint8 hit_indication_value;
    soc_mem_t mem = INVALIDm;
    uint32 position;
    uint32 mem_entry_data;
    uint32 mem_hit_field_value;

    SHR_FUNC_INIT_VARS(unit);

    
    max_nof_srams_per_tcam_bank = dnx_data_field.tcam.action_width_selector_size_get(unit);
    max_nof_entries_in_two_srams =
        (dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit) * max_nof_srams_per_tcam_bank);

    
    hit_indication_value = 0;
    *entry_hit_p = 0;

    
    SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_index_get(unit, core_id, entry_handle, &position));
    
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);

    
    if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;

        
        max_nof_big_bank_lines = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
        bank_line_id = ((position % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
    }
    else
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;

        
        max_nof_small_bank_lines = dnx_data_field.tcam.nof_small_bank_lines_get(unit);
        bank_line_id = ((position % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
    }

    bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (position % 2));
    sram_line_entry_id =
        (((position % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);

    mem_entry_data = 0;
    mem_hit_field_value = 0;
    SHR_IF_ERR_EXIT(soc_mem_array_read
                    (unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &mem_entry_data));
    soc_mem_field_get(unit, mem, &mem_entry_data, HITf, &mem_hit_field_value);

    
    if (dnx_data_field.features.hitbit_volatile_get(unit))
    {
        uint8 hit_bit_sw_state_value;
        uint32 hit_bit_sw_state_value_uint32;
        if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.get(unit, bank_sram_id, core_id,
                                                                                      bank_line_id,
                                                                                      &hit_bit_sw_state_value);
        }
        else
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.bank_line_id.get(unit, (bank_sram_id -
                                                                                                   dnx_data_field.
                                                                                                   tcam.nof_big_banks_srams_get
                                                                                                   (unit)), core_id,
                                                                                            bank_line_id,
                                                                                            &hit_bit_sw_state_value);
        }
        hit_bit_sw_state_value_uint32 = hit_bit_sw_state_value;
        SHR_IF_ERR_EXIT(utilex_bitstream_or(&hit_bit_sw_state_value_uint32, &mem_hit_field_value, 1));
        hit_bit_sw_state_value = hit_bit_sw_state_value_uint32;
        if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.set(unit, bank_sram_id, core_id,
                                                                                      bank_line_id,
                                                                                      hit_bit_sw_state_value);
        }
        else
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.bank_line_id.set(unit, (bank_sram_id -
                                                                                                   dnx_data_field.
                                                                                                   tcam.nof_big_banks_srams_get
                                                                                                   (unit)), core_id,
                                                                                            bank_line_id,
                                                                                            hit_bit_sw_state_value);
        }
        hit_indication_value = utilex_bitstream_test_bit(&hit_bit_sw_state_value_uint32, sram_line_entry_id);
    }
    else
    {
        hit_indication_value = utilex_bitstream_test_bit(&mem_hit_field_value, sram_line_entry_id);
    }

    
    if (hit_indication_value)
    {
        
        *entry_hit_p = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_tcam_hit_get(
    int unit,
    uint32 entry_handle,
    uint8 *entry_hit_p)
{
    uint8 entry_hit;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_handle);
    uint8 internal_entry_hit[1];

    SHR_FUNC_INIT_VARS(unit);

    *entry_hit_p = 0;
    internal_entry_hit[0] = 0;

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
        for (core = 0; core < nof_cores; core++)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_get(unit, core, entry_handle, &entry_hit));

            
            if (entry_hit == TRUE)
            {
                
                SHR_BITSET(&internal_entry_hit[0], core);
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_get(unit, core, entry_handle, &entry_hit));

        
        if (entry_hit == TRUE)
        {
            
            SHR_BITSET(&internal_entry_hit[0], core);
        }
    }

    *entry_hit_p = internal_entry_hit[0];

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_tcam_hit_per_core_flush(
    int unit,
    dnx_field_tcam_core_e core_id,
    uint32 entry_handle)
{
    soc_reg_above_64_val_t reg_val_zero;
    soc_mem_t mem = INVALIDm;
    uint32 bank_id, bank_sram_id, bank_line_id, sram_line_entry_id;
    uint32 max_nof_big_bank_lines, max_nof_small_bank_lines;
    uint32 tcam_indirect_mask;
    uint32 max_nof_entries_in_two_srams;
    uint32 max_nof_srams_per_tcam_bank;
    uint32 position;

#ifdef ADAPTER_SERVER_MODE
    uint32 mem_entry_data = 0;
    uint32 mem_hit_field_value = 0;
#else
    uint32 mem_addr;
    uint8 at;
    uint32 reg_val = 0;
#endif

    SHR_FUNC_INIT_VARS(unit);

    bank_sram_id = bank_id = bank_line_id = sram_line_entry_id = 0;
    max_nof_big_bank_lines = max_nof_small_bank_lines = 0;
    max_nof_entries_in_two_srams = max_nof_srams_per_tcam_bank = 0;

    SOC_REG_ABOVE_64_CLEAR(reg_val_zero);

    
    SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_index_get(unit, core_id, entry_handle, &position));

    
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);

    
    max_nof_srams_per_tcam_bank = dnx_data_field.tcam.action_width_selector_size_get(unit);
    max_nof_entries_in_two_srams =
        (dnx_data_field.tcam.tcam_entries_per_hit_indication_entry_get(unit) * max_nof_srams_per_tcam_bank);

    
    if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;

        
        max_nof_big_bank_lines = dnx_data_field.tcam.nof_big_bank_lines_get(unit);
        
        bank_line_id = ((position % max_nof_big_bank_lines) / max_nof_entries_in_two_srams);
    }
    else
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;

        
        max_nof_small_bank_lines = dnx_data_field.tcam.nof_small_bank_lines_get(unit);
        
        bank_line_id = ((position % max_nof_small_bank_lines) / max_nof_entries_in_two_srams);
    }

    
    bank_sram_id = ((bank_id * max_nof_srams_per_tcam_bank) + (position % 2));
    sram_line_entry_id =
        (((position % max_nof_entries_in_two_srams) - (bank_sram_id % 2)) / max_nof_srams_per_tcam_bank);
    
    tcam_indirect_mask = 0x1 << sram_line_entry_id;

    
#ifdef ADAPTER_SERVER_MODE
    
    if (core_id == 0)
    {
        SHR_IF_ERR_EXIT(soc_mem_array_read
                        (unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &mem_entry_data));
        soc_mem_field_get(unit, mem, &mem_entry_data, HITf, &mem_hit_field_value);

        mem_entry_data = mem_entry_data & ~tcam_indirect_mask;
        SHR_IF_ERR_EXIT(soc_mem_array_write
                        (unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &mem_entry_data));
    }
#else
    
    mem_addr = soc_mem_addr_get(unit, mem, bank_sram_id, TCAM_BLOCK(unit, core_id), bank_line_id, &at);

    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMANDr(unit, core_id, 0xe00003));
    soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_ADDRf, mem_addr);
    soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val, INDIRECT_COMMAND_TYPEf, 0x0);
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_ADDRESSr(unit, core_id, reg_val));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_WR_MASKr(unit, core_id, tcam_indirect_mask));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_WR_DATAr(unit, core_id, reg_val_zero));
#endif

    
    if (dnx_data_field.features.hitbit_volatile_get(unit))
    {
        uint8 hit_bit_sw_state_value;
        uint32 hit_bit_sw_state_value_uint32;
        if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.get(unit, bank_sram_id, core_id,
                                                                                      bank_line_id,
                                                                                      &hit_bit_sw_state_value);
        }
        else
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.bank_line_id.get(unit, (bank_sram_id -
                                                                                                   dnx_data_field.
                                                                                                   tcam.nof_big_banks_srams_get
                                                                                                   (unit)), core_id,
                                                                                            bank_line_id,
                                                                                            &hit_bit_sw_state_value);
        }
        hit_bit_sw_state_value_uint32 = hit_bit_sw_state_value;
        hit_bit_sw_state_value_uint32 = hit_bit_sw_state_value_uint32 & ~tcam_indirect_mask;

        hit_bit_sw_state_value = hit_bit_sw_state_value_uint32;
        if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication.bank_line_id.set(unit, bank_sram_id, core_id,
                                                                                      bank_line_id,
                                                                                      hit_bit_sw_state_value);
        }
        else
        {
            dnx_field_tcam_hit_indication.tcam_action_hit_indication_small.bank_line_id.set(unit, (bank_sram_id -
                                                                                                   dnx_data_field.
                                                                                                   tcam.nof_big_banks_srams_get
                                                                                                   (unit)), core_id,
                                                                                            bank_line_id,
                                                                                            hit_bit_sw_state_value);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_tcam_hit_flush(
    int unit,
    uint32 entry_handle)
{
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry_handle);

    SHR_FUNC_INIT_VARS(unit);

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
        
        for (core = 0; core < nof_cores; core++)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_flush(unit, core, entry_handle));
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_per_core_flush(unit, core, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_get(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    uint32 position;
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry->entry_hw_id);
    uint32 unsupported_hitbit_flags;
    uint8 hitbit_flags;

    SHR_FUNC_INIT_VARS(unit);

    hitbit_flags = entry->hitbit;
    if ((unsupported_hitbit_flags =
         (hitbit_flags &
          ~(DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
            DBAL_PHYSICAL_KEY_HITBIT_CLEAR))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported!", unsupported_hitbit_flags);
    }

    if (core == DNX_FIELD_TCAM_CORE_ALL)
    {
        
        core = DNX_FIELD_TCAM_CORE_FIRST;
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));
    
    
    SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_index_get(unit, core, entry->entry_hw_id, &position));

    if (db_info.direct_table)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_entry_dt_read_aux
                               (unit, core, app_id, position, entry->payload, entry->payload_size));
    }
    else
    {
        uint8 entry_in_use;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_get(unit, core, position, &entry_in_use));
        if (!entry_in_use)
        {
            
            entry->hitbit = 0;
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            SHR_EXIT();
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_get_aux
                        (unit, core, app_id, position, entry->payload_size, entry->key, entry->k_mask, entry->payload));

        if (hitbit_flags &
            (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
             DBAL_PHYSICAL_KEY_HITBIT_CLEAR))
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_get(unit, entry->entry_hw_id, &entry->hitbit));
            
            if (hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_flush(unit, entry->entry_hw_id));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_sw_hash_key_add(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 position)
{
    dnx_field_tcam_access_mapper_hash_key hash_key = { {0}, {0} };
    uint8 success;
    int bank_id;
    int bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_BITAND_RANGE(key, key_mask, 0, key_size, key);

    SHR_BITCOPY_RANGE(hash_key.key, 0, key, 0, key_size);
    SHR_BITCOPY_RANGE(hash_key.key_mask, 0, key_mask, 0, key_size);

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                    key_2_entry_id_hash.insert_at_index(unit, core, bank_id, &hash_key, bank_offset, &success));
    if (!success)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS,
                     "Failed adding new entry to position %d in SW hash - Check if entry already exists%s%s%s\n",
                     position, EMPTY, EMPTY, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_sw_hash_key_remove(
    int unit,
    int core,
    uint32 position)
{
    int bank_id;
    int bank_offset;

    SHR_FUNC_INIT_VARS(unit);

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                    key_2_entry_id_hash.delete_by_index(unit, core, bank_id, bank_offset));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_single_entry_payload_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *payload,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 is_special)
{
    uint32 action_bank_offset;
    uint32 action_lsb_bank_id;
    uint32 action_msb_bank_id;
    SHR_FUNC_INIT_VARS(unit);

    
    action_bank_offset = bank_offset / 2;
    
    if (!is_special && key_mode != DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF)
    {
        action_lsb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id + 1 : 2 * bank_id;
        action_msb_bank_id = (dnx_data_field.features.tcam_result_flip_eco_get(unit)) ? 2 * bank_id : 2 * bank_id + 1;
    }
    else
    {
        action_lsb_bank_id = 2 * bank_id;
        action_msb_bank_id = 2 * bank_id + 1;
    }
    
    if (part & DNX_FIELD_TCAM_ACCESS_PART_LSB)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write
                        (unit, core, action_lsb_bank_id, action_bank_offset, payload));
    }
    if (!is_special && (part & DNX_FIELD_TCAM_ACCESS_PART_MSB))
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_payload_write
                        (unit, core, action_msb_bank_id, action_bank_offset, payload + HALF_ACTION_WORD_SIZE));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_single_entry_key_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid)
{
    SHR_FUNC_INIT_VARS(unit);

    
    if (dnx_data_field.features.ecc_enable_get(unit) && part != DNX_FIELD_TCAM_ACCESS_PART_ALL)
    {
        
        uint8 entry_in_use;
        uint32 position =
            DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset + (DNX_FIELD_TCAM_ACCESS_PART_MSB - part));
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_get(unit, core, position, &entry_in_use));
        if (entry_in_use)
        {
            
            uint32 key_read[MAX_SINGLE_KEY_WORD_SIZE + 1];
            uint32 key_read_m[MAX_SINGLE_KEY_WORD_SIZE + 1];
            int copy_from = 0;
            uint8 valid_bits;
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read
                            (unit, core, bank_id, bank_offset, DNX_FIELD_TCAM_ACCESS_PART_ALL - part, key_read,
                             key_read_m, &valid_bits));
            if (part == DNX_FIELD_TCAM_ACCESS_PART_LSB)
            {
                copy_from = SINGLE_KEY_BIT_SIZE / 2;
            }
            
            valid |= valid_bits & (DNX_FIELD_TCAM_ACCESS_PART_ALL - part);
            
            SHR_BITCOPY_RANGE(key, copy_from, key_read, copy_from, SINGLE_KEY_BIT_SIZE / 2);
            SHR_BITCOPY_RANGE(key_mask, copy_from, key_read_m, copy_from, SINGLE_KEY_BIT_SIZE / 2);
            
            part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_write
                    (unit, core, bank_id, bank_offset, key, key_mask, key_mode, part, valid));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_single_entry_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *key,
    uint32 *key_mask,
    uint32 *payload,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid,
    uint8 is_special)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_payload_write
                    (unit, core, bank_id, bank_offset, payload, key_mode, part, is_special));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_key_write
                    (unit, core, bank_id, bank_offset, key, key_mask, key_mode, part, valid));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_invalidate(
    int unit,
    int core,
    int position,
    dnx_field_tcam_access_part_e part)
{
    uint32 zero_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 zero_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 valid = 0;
    int bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    int bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_key_write(unit, core, bank_id, bank_offset,
                                                                 zero_key, zero_mask, 0, part, valid));
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_key_prefix_add(
    int unit,
    uint32 *key,
    uint32 *key_mask,
    uint32 key_size,
    uint32 prefix_value,
    uint32 prefix_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (prefix_size > 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Writing prefix %d (size %d)%s%s\n", prefix_value, prefix_size, EMPTY, EMPTY);
        SHR_BITCOPY_RANGE(key, key_size - prefix_size, &prefix_value, 0, prefix_size);
        if (key_mask != NULL)
        {
            SHR_BITSET_RANGE(key_mask, key_size - prefix_size, prefix_size);
        }
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_add_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position,
    uint32 key[],
    uint32 key_mask[],
    uint32 payload[],
    uint32 ent_payload_size,
    uint8 valid,
    uint8 is_update)
{
    dnx_field_tcam_access_part_e part;
    uint8 odd;
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_access_key_mode_e key_mode;
    uint32 bank_id;
    uint32 bank_offset;
    uint32 key_to_write[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 mask_to_write[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 payload_to_write[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint32 payload_msb_aligned[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    uint8 is_special = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    
    odd = (position % 2);
    if (odd && db_info.key_size != dnx_data_field.tcam.key_size_half_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Database %d with key_size %d can't start from position %d", app_id,
                     db_info.key_size, position);
    }

    
    SHR_BITCOPY_RANGE(payload_msb_aligned, db_info.payload_size - ent_payload_size, payload, 0, ent_payload_size);

    
    SHR_BITCOPY_RANGE(key_to_write, 0, key, 0, db_info.key_size);
    SHR_BITCOPY_RANGE(mask_to_write, 0, key_mask, 0, db_info.key_size);
    SHR_BITCOPY_RANGE(payload_to_write, 0, payload_msb_aligned, 0, db_info.payload_size);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_key_prefix_add
                    (unit, key_to_write, mask_to_write, db_info.key_size, db_info.prefix_value, db_info.prefix_size));

    
    if (ACCESS_SW_MOCK && !DNX_FIELD_TCAM_IS_STAGE_PMF(db_info.stage))
    {
        
        if (is_update)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_hash_key_remove(unit, core, position));
        }
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_hash_key_add
                        (unit, core, key_to_write, mask_to_write, db_info.key_size, position));
    }

    if (db_info.key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        
        part = (odd) ? DNX_FIELD_TCAM_ACCESS_PART_MSB : DNX_FIELD_TCAM_ACCESS_PART_LSB;

        
        if (odd)
        {
            SHR_BITCOPY_RANGE(key_to_write, HALF_KEY_BIT_SIZE, key_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCOPY_RANGE(mask_to_write, HALF_KEY_BIT_SIZE, mask_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCOPY_RANGE(payload_to_write, HALF_ACTION_BIT_SIZE, payload_msb_aligned, 0, HALF_ACTION_BIT_SIZE);

            SHR_BITCLR_RANGE(key_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCLR_RANGE(mask_to_write, 0, HALF_KEY_BIT_SIZE);
            SHR_BITCLR_RANGE(payload_to_write, 0, HALF_ACTION_BIT_SIZE);
        }
        
        position = position - (position % 2);
    }
    else
    {
        
        part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
    }
    
    if (valid != 0)
    {
        valid = part;
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key(160b) after prefix: 0x%08x%08x%08x%08x", key[4], key[3], key[2], key[1]);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "%08x%s%s%s\n", key[0], EMPTY, EMPTY, EMPTY);
    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "MSB Key(160b) after prefix: 0x%08x%08x%08x%08x", key[9], key[8], key[7], key[6]);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "%08x%s%s%s\n", key[5], EMPTY, EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_size_to_mode(unit, db_info.key_size, &key_mode));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Key size: %d, key mode: %d%s%s\n", db_info.key_size, key_mode, EMPTY, EMPTY);
    
    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);
    bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);

    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        uint32 *msb_key_part;
        uint32 *msb_key_mask_part;
        uint32 *msb_payload_part;

        if (is_update)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_invalidate(unit, core, position, part));
        }

        
        msb_key_part = key_to_write + SINGLE_KEY_WORD_SIZE;
        msb_key_mask_part = mask_to_write + SINGLE_KEY_WORD_SIZE;
        msb_payload_part = payload_to_write + SINGLE_ACTION_WORD_SIZE;
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_write(unit, core, bank_id + 1, bank_offset,
                                                                 msb_key_part, msb_key_mask_part, msb_payload_part,
                                                                 key_mode, part, valid, FALSE));
    }

    
    if (db_info.key_size == dnx_data_field.tcam.key_size_single_get(unit)
        && db_info.payload_size == dnx_data_field.tcam.action_size_half_get(unit))
    {
        is_special = TRUE;
    }
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_single_entry_write(unit, core, bank_id, bank_offset,
                                                             key_to_write, mask_to_write, payload_to_write,
                                                             key_mode, part, valid, is_special));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_add(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    uint32 position;
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry->entry_hw_id);
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_index_get(unit, core_iter, entry->entry_hw_id, &position));

            if (db_info.direct_table)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_dt_write_aux
                                (unit, core_iter, app_id, position, entry->payload, entry->payload_size,
                                 entry->indirect_commit_mode));
            }
            else
            {
                uint8 valid;
                
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_get(unit, core_iter, position, &valid));
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_add_aux
                                (unit, core_iter, app_id, position, entry->key, entry->k_mask, entry->payload,
                                 entry->payload_size, valid, entry->indirect_commit_mode));
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_set(unit, core_iter, position));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_compare(
    int unit,
    int core,
    int bank_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    int *bank_offset,
    uint8 *found)
{
    soc_reg_above_64_val_t mem_val;
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE] = { 0 };
    uint32 position;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    
    position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, 0);

    SHR_BITCOPY_RANGE(data, 0, key, 0, SINGLE_KEY_BIT_SIZE);

    
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x1);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);

    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), position, mem_val));

    
    SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), position, mem_val));

    *found = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_MATCHf);
    if (*found)
    {
        *bank_offset = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_AINDEXf);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_id_get(
    int unit,
    int core,
    uint32 absolute_location,
    uint32 *access_id)
{
    uint8 found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.
                    entry_location_hash.get_by_index(unit, core, absolute_location, access_id, &found));

    if (!found)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Entry at location: %d not found in location->entry Hash", absolute_location);
    }

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_field_tcam_access_key_to_access_id_hw(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 *access_id)
{
    int bank_id;
    int bank_offset = 0;
    uint8 found = FALSE;
    uint32 absolute_location;

    SHR_FUNC_INIT_VARS(unit);

    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_compare(unit, core, bank_id, key, &bank_offset, &found));
        if (found)
        {
            absolute_location = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_id_get(unit, core, absolute_location, access_id));
            break;
        }
    }

    if (!found)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_access_key_to_access_id_sw(
    int unit,
    int core,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size,
    uint32 *access_id)
{
    dnx_field_tcam_access_mapper_hash_key hash_key = { {0}, {0} };
    uint8 found;
    int bank_id;
    uint32 bank_offset;
    uint32 position;

    SHR_FUNC_INIT_VARS(unit);

    
    SHR_BITAND_RANGE(key, key_mask, 0, key_size, key);

    SHR_BITCOPY_RANGE(hash_key.key, 0, key, 0, key_size);
    SHR_BITCOPY_RANGE(hash_key.key_mask, 0, key_mask, 0, key_size);

    
    for (bank_id = 0; bank_id < dnx_data_field.tcam.nof_banks_get(unit); bank_id++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_mapper_sw.
                        key_2_entry_id_hash.find(unit, core, bank_id, &hash_key, &bank_offset, &found));
        if (found)
        {
            break;
        }
    }

    if (!found)
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    
    position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_id_get(unit, core, position, access_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_key_to_access_id(
    int unit,
    int core,
    uint32 app_id,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_mask[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 *access_id)
{
    dnx_field_tcam_access_db_info db_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_key_prefix_add
                    (unit, key, key_mask, db_info.key_size, db_info.prefix_value, db_info.prefix_size));

    if (SEARCH_KEY_IN_TCAM_HW)
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_key_to_access_id_hw(unit, core, key, access_id));
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(dnx_field_tcam_access_key_to_access_id_sw
                               (unit, core, key, key_mask, db_info.key_size, access_id));
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_tcam_access_entry_delete_aux(
    int unit,
    int core,
    uint32 app_id,
    uint32 position)
{
    dnx_field_tcam_access_db_info db_info;
    dnx_field_tcam_access_part_e part;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    if (ACCESS_SW_MOCK && !DNX_FIELD_TCAM_IS_STAGE_PMF(db_info.stage))
    {
        
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw_hash_key_remove(unit, core, position));
    }

    if (db_info.key_size == dnx_data_field.tcam.key_size_half_get(unit))
    {
        part = (position % 2) ? DNX_FIELD_TCAM_ACCESS_PART_MSB : DNX_FIELD_TCAM_ACCESS_PART_LSB;
        position -= (position % 2);
    }
    else
    {
        part = DNX_FIELD_TCAM_ACCESS_PART_ALL;
    }

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_invalidate(unit, core, position, part));
    if (db_info.key_size == dnx_data_field.tcam.key_size_double_get(unit))
    {
        
        int bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position) + 1;
        int bank_offset = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_OFFSET(position);
        int second_part_double_position = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_invalidate(unit, core, second_part_double_position, part));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_entry_delete(
    int unit,
    dbal_physical_tables_e physical_tables[DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE],
    uint32 app_id,
    dbal_physical_entry_t * entry,
    uint32 flags)
{
    dnx_field_tcam_access_db_info db_info;
    uint32 position;
    dnx_field_tcam_core_e core = DNX_FIELD_TCAM_ENTRY_CORE_ID(entry->entry_hw_id);
    dnx_field_tcam_core_e core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));
    for (core_iter = 0; core_iter < dnx_data_device.general.nof_cores_get(unit); core_iter++)
    {
        if (core == DNX_FIELD_TCAM_CORE_ALL || core == core_iter)
        {
            
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_index_get(unit, core_iter, entry->entry_hw_id, &position));

            if (db_info.direct_table)
            {
                uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS] = { 0 };

                
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_dt_write_aux
                                (unit, core_iter, app_id, position, payload, entry->payload_size, TRUE));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_delete_aux(unit, core_iter, app_id, position));
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_clear(unit, core_iter, position));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_move(
    int unit,
    int core,
    uint32 app_id,
    uint32 position_from,
    uint32 position_to)
{
    uint32 key[MAX_DOUBLE_KEY_WORD_SIZE + 1];
    uint32 key_mask[MAX_DOUBLE_KEY_WORD_SIZE + 1];
    uint32 payload[MAX_DOUBLE_ACTION_WORD_SIZE];
    dnx_field_tcam_access_db_info db_info;
    uint8 valid;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_get_aux
                    (unit, core, app_id, position_from, db_info.payload_size, key, key_mask, payload));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_delete_aux(unit, core, app_id, position_from));
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_clear(unit, core, position_from));

    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.valid_bmp.v_bit.bit_get(unit, core, position_to, &valid));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_add_aux(unit, core, app_id, position_to, key, key_mask, payload, db_info.payload_size, valid, FALSE      ));
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.entry_in_use_bmp.v_bit.bit_set(unit, core, position_to));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_iterator_init(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    
    
    physical_entry_iterator->mdb_entry_index = -1;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_iterator_get_next(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator,
    dbal_physical_entry_t * entry,
    uint8 *is_end)
{
    uint32 *location = &(physical_entry_iterator->mdb_entry_index);
    uint8 found;
    dnx_field_tcam_access_db_info db_info;
    int core = 0;
    uint32 unsupported_hitbit_flags;
    uint8 hitbit_flags = entry->hitbit;

    SHR_FUNC_INIT_VARS(unit);

    if ((unsupported_hitbit_flags =
         (hitbit_flags &
          ~(DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
            DBAL_PHYSICAL_KEY_HITBIT_CLEAR))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported!", unsupported_hitbit_flags);
    }

    
    
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_info_get(unit, app_id, &db_info));

    while (TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_location_algo_handler_next_get(unit, core, app_id, *location, location, &found));

        if (found)
        {
            uint32 access_id;
            
            SHR_SET_CURRENT_ERR(dnx_field_tcam_access_entry_get_aux
                                (unit, core, app_id, *location, entry->payload_size, entry->key, entry->k_mask,
                                 entry->payload));

            if (SHR_FUNC_VAL_IS(_SHR_E_NOT_FOUND))
            {
                
                SHR_SET_CURRENT_ERR(_SHR_E_NONE);
                continue;
            }
            else
            {
                
                SHR_IF_ERR_EXIT(SHR_GET_CURRENT_ERR());
            }

            
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_id_get(unit, core, *location, &access_id));
            entry->entry_hw_id = access_id;

            if (hitbit_flags &
                (DBAL_PHYSICAL_KEY_HITBIT_GET | DBAL_PHYSICAL_KEY_HITBIT_PRIMARY | DBAL_PHYSICAL_KEY_HITBIT_SECONDARY |
                 DBAL_PHYSICAL_KEY_HITBIT_CLEAR))
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_get(unit, entry->entry_hw_id, &entry->hitbit));
                
                if (hitbit_flags & DBAL_PHYSICAL_KEY_HITBIT_CLEAR)
                {
                    SHR_IF_ERR_EXIT(dnx_field_tcam_access_tcam_hit_flush(unit, entry->entry_hw_id));
                }
            }
        }
        
        break;
    }

    
    *is_end = !found;

    
    if (!found)
    {
        entry->hitbit = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_access_iterator_deinit(
    int unit,
    dbal_physical_tables_e dbal_physical_table_id,
    uint32 app_id,
    dbal_physical_entry_iterator_t * physical_entry_iterator)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

static void
dnx_ser_tcam_single_entry_print(
    int unit,
    uint32 entryId,
    uint32 valid_bits,
    uint32 *key,
    uint32 *mask,
    uint32 ecc_key,
    uint32 ecc_mask)
{
    uint32 key_mode0 = 0, key_mode1 = 0, key_mode;
    uint32 half_key0[MAX_HALF_KEY_WORD_SIZE] = { 0 };
    uint32 half_key1[MAX_HALF_KEY_WORD_SIZE] = { 0 };
    char *str_key_mode[] = { "Half", "Single", "Invalid", "Double" };
    char str_key0[(MAX_HALF_KEY_WORD_SIZE * 8) + 3];
    char str_key1[(MAX_HALF_KEY_WORD_SIZE * 8) + 3];

    SHR_BITCOPY_RANGE(&key_mode0, 0, key, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(&key_mode1, 0, key, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key0, 0, key, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key1, 0, key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);

    if (valid_bits == 0)
    {
        cli_out("Error: tcam entry is not valid!!\n");
        return;
    }
    else if (valid_bits == 3)
    {
        
        if (key_mode0 != key_mode1)
        {
            cli_out("Error: key_mode0 and key_mode1 are not matched!!(%d vs %d)\n", key_mode0, key_mode1);
            return;
        }
        key_mode = key_mode0;
    }
    else
    {
        
        if (valid_bits == 1)
        {
            key_mode = key_mode0;
        }
        else
        {
            key_mode = key_mode1;
        }
        if (key_mode != DNX_FIELD_TCAM_ACCESS_KEY_MODE_HALF)
        {
            cli_out("Error: key mode is error, valid_bits=%d, key_mode0=%d, key_mode1=%d\n",
                    valid_bits, key_mode0, key_mode1);
            return;
        }
    }
    _shr_format_long_integer(str_key0, half_key0, BITS2BYTES(HALF_KEY_BIT_SIZE));
    _shr_format_long_integer(str_key1, half_key1, BITS2BYTES(HALF_KEY_BIT_SIZE));
    cli_out("%-8d%-8d%-10s%-26s%-26s0x%-8X\n", entryId, valid_bits,
            str_key_mode[key_mode], str_key0, str_key1, ecc_key);

    
    SHR_BITCOPY_RANGE(&key_mode0, 0, mask, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(&key_mode1, 0, mask, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key0, 0, mask, KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(half_key1, 0, mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE);
    _shr_format_long_integer(str_key0, half_key0, BITS2BYTES(HALF_KEY_BIT_SIZE));
    _shr_format_long_integer(str_key1, half_key1, BITS2BYTES(HALF_KEY_BIT_SIZE));
    cli_out("%-16s%d/%d%-7s%-26s%-26s0x%-8X\n", "", key_mode0, key_mode1, "", str_key0, str_key1, ecc_mask);
}

static int
dnx_ser_tcam_entry_is_equal(
    int unit,
    uint32 valid_bit1,
    uint32 *key1,
    uint32 *mask1,
    uint32 valid_bit2,
    uint32 *key2,
    uint32 *mask2)
{
    int rc = TRUE;
    if (valid_bit1 != valid_bit2)
    {
        return FALSE;
    }

    if (valid_bit1 & 0x1)
    {
        
        if (!SHR_BITEQ_RANGE(key1, key2, 0, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
        if (!SHR_BITEQ_RANGE(mask1, mask2, 0, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
    }
    if (valid_bit1 & 0x2)
    {
        
        if (!SHR_BITEQ_RANGE(key1, key2, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
        if (!SHR_BITEQ_RANGE
            (mask1, mask2, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE))
        {
            rc = FALSE;
        }
    }

    return rc;
}


shr_error_e
dnx_ser_tcam_access_entry_key_dump(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 mode)
{
    uint32 ii, bank_offset, max_entry, tcam_bank_address;
    uint8 shadow_key_valid_bits, key_valid_bits;
    uint32 shadow_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 shadow_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 data_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 data_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    soc_reg_above_64_val_t mem_val_read;
    soc_reg_above_64_val_t mem_val_write;
    soc_mem_t ecc_mem;
    uint32 ecc_key, ecc_mask;
    int core = core_id;

    SHR_FUNC_INIT_VARS(unit);

    cli_out("dnx_ser_tcam_access_entry_key_dump: core_id=%d, bank_id=%d, mode=%d\n", core_id, bank_id, mode);

    if (bank_id < dnx_data_field.tcam.nof_big_banks_get(unit))
    {
        max_entry = dnx_data_field.tcam.nof_big_bank_lines_get(unit) / 2;
        ecc_mem = TCAM_TCAM_ENTRY_ECCm;
    }
    else if (bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) + dnx_data_field.tcam.nof_small_banks_get(unit))
    {
        max_entry = dnx_data_field.tcam.nof_small_bank_lines_get(unit) / 2;
        ecc_mem = TCAM_TCAM_ENTRY_ECC_SMALLm;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error. tcam bank_id (%d) was not supported.\n", bank_id);
    }

    cli_out("%-8s%-8s%-10s%-26s%-26s%-8s\n", "EntryID", "Valid", "KeyMode",
            "Half-Key&Mask-0", "Half-Key&Mask-1", "ECC");
    cli_out("-----------------------------------------------------------------------\n");
    for (ii = 0; ii < max_entry; ii++)
    {
        bank_offset = ii * 2;
        tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

        
         
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset + 1, shadow_mask, &shadow_key_valid_bits));

        
         
        SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                        (unit, core, bank_id, bank_offset, shadow_key, &shadow_key_valid_bits));

        
        SOC_REG_ABOVE_64_CLEAR(mem_val_write);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_WRf, 0x0);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_RDf, 0x1);
        soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_CMPf, 0x0);
        
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_write));
        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_read));
        key_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_key);

        
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm
                        (unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_write));
        SOC_REG_ABOVE_64_CLEAR(mem_val_read);
        SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_read));
        soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_mask);

        
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ecc_mem, bank_id, TCAM_BLOCK(unit, core), bank_offset, &ecc_key));
        SHR_IF_ERR_EXIT(soc_mem_array_read(unit, ecc_mem, bank_id, TCAM_BLOCK(unit, core), bank_offset + 1, &ecc_mask));

        if (mode == 0)
        {
            
            if (key_valid_bits)
            {
                dnx_ser_tcam_single_entry_print(unit, ii, key_valid_bits, data_key, data_mask, ecc_key, ecc_mask);
            }
        }
        else if (mode == 1)
        {
            
            if (shadow_key_valid_bits)
            {
                dnx_ser_tcam_single_entry_print(unit, ii, shadow_key_valid_bits, shadow_key, shadow_mask, ecc_key,
                                                ecc_mask);
            }
        }
        else if (mode == 2)
        {
            
            if (!dnx_ser_tcam_entry_is_equal(unit, key_valid_bits, data_key, data_mask,
                                             shadow_key_valid_bits, shadow_key, shadow_mask))
            {
                cli_out("%-d:    TCAM shadow value is not match HW\n", ii);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_ser_tcam_access_write_hw_entry(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 valid,
    uint32 key_mode,
    uint32 *key,
    uint32 *mask)
{
    soc_reg_above_64_val_t mem_val;
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 encoded_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 tcam_bank_address;
    int core = core_id;

    SHR_FUNC_INIT_VARS(unit);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    SHR_BITCOPY_RANGE(encoded_key, 0, &key_mode, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_key, KEY_MODE_BIT_SIZE, key, 0, HALF_KEY_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_key, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, &key_mode, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_key, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, key,
                      HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);
    SHR_BITSET_RANGE(encoded_mask, 0, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_mask, KEY_MODE_BIT_SIZE, mask, 0, HALF_KEY_BIT_SIZE);
    SHR_BITSET_RANGE(encoded_mask, HALF_KEY_BIT_SIZE + KEY_MODE_BIT_SIZE, KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(encoded_mask, HALF_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, mask,
                      HALF_KEY_BIT_SIZE, HALF_KEY_BIT_SIZE);

    SHR_BITCOPY_RANGE(data, 0, encoded_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      encoded_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, valid);
    if (ACCESS_HW)
    {
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));
    }

exit:
    cli_out("dnx_ser_tcam_access_write_hw_entry, core_id=%d, bank_id=%d, bank_offset=%d, valid=%d, key_mode=%d\n",
            core_id, bank_id, bank_offset, valid, key_mode);
    cli_out("\t%-16s%08X %08X %08X %08X %08X\n", "Key:", key[0], key[1], key[2], key[3], key[4]);
    cli_out("\t%-16s%08X %08X %08X %08X %08X\n", "Mask:", mask[0], mask[1], mask[2], mask[3], mask[4]);
    cli_out("\t%-16s%08X %08X %08X %08X %08X %08X\n", "encoded_key:",
            encoded_key[0], encoded_key[1], encoded_key[2], encoded_key[3], encoded_key[4], encoded_key[5]);
    cli_out("\t%-16s%08X %08X %08X %08X %08X %08X\n", "encoded_mask:",
            encoded_mask[0], encoded_mask[1], encoded_mask[2], encoded_mask[3], encoded_mask[4], encoded_mask[5]);
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_ser_tcam_access_rewrite_key_from_shadow(
    int unit,
    int core_id,
    uint32 bank_id,
    uint32 bank_offset)
{
    soc_reg_above_64_val_t mem_val;
    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 shadow_key[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint32 shadow_mask[(MAX_SINGLE_KEY_WORD_SIZE + 1)] = { 0 };
    uint8 shadow_key_valid, shadow_mask_valid;
    uint32 tcam_bank_address;
    int core = core_id;

    SHR_FUNC_INIT_VARS(unit);

    
    if (!DNX_FIELD_TCAM_SHADOW_KEEP)
    {
        SHR_IF_ERR_EXIT(_SHR_E_RESOURCE);
    }

    
    bank_offset = bank_offset - (bank_offset & 0x1);
    
     
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                    (unit, core, bank_id, bank_offset, shadow_key, &shadow_key_valid));
    
     
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_entry_key_read_shadow
                    (unit, core, bank_id, bank_offset + 1, shadow_mask, &shadow_mask_valid));

    
    SHR_BITCOPY_RANGE(data, 0, shadow_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      shadow_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    cli_out("dnx_ser_tcam_access_rewrite_key_from_shadow: core_id=%d, bank_id=%d, bank_offset=%d\n",
            core_id, bank_id, bank_offset);
    dnx_ser_tcam_single_entry_print(unit, bank_offset, shadow_key_valid, shadow_key, shadow_mask, 0, 0);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, shadow_key_valid);
    if (ACCESS_HW)
    {
        SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnx_ser_tcam_protect_machine_enable(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (enable)
    {
        if (SOC_IS_J2P(unit))
        {
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_BANK_BITMAPf, 0xFFFF));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_ENABLEf, 0x1));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_ECC_CORRUPTION_DETECTION_ENr,
                                                   REG_PORT_ANY, TCAM_ECC_CORRUPTION_DETECTION_ENf, 0x1));

        }
        else
        {
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_BANK_BITMAPf, 0xFFFF));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_ECC_FIX_ENr, REG_PORT_ANY, TCAM_ECC_FIX_ENf, 0x1));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_TRIGGERr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_TRIGGERf, 0x1));
        }
    }
    else
    {
        if (SOC_IS_J2P(unit))
        {
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_BANK_BITMAPf, 0));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_ENABLEf, 0));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_ECC_CORRUPTION_DETECTION_ENr,
                                                   REG_PORT_ANY, TCAM_ECC_CORRUPTION_DETECTION_ENf, 0));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_TRIGGERr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_TRIGGERf, 0));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_ECC_FIX_ENr, REG_PORT_ANY, TCAM_ECC_FIX_ENf, 0));
            SHR_IF_ERR_EXIT(soc_reg_field32_modify(unit, TCAM_TCAM_PROTECTION_MACHINE_CONFIGURATIONr,
                                                   REG_PORT_ANY, TCAM_PROTECTION_BANK_BITMAPf, 0));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
