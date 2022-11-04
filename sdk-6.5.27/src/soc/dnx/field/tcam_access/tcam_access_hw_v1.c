#include <soc/drv.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_location_types.h>

#include "tcam_access_hw_v1.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

shr_error_e
dnx_tcam_access_hw_v1_payload_read(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
    {
        action_tbl = TCAM_TCAM_ACTIONm;
    }
    else
    {
        action_tbl = TCAM_TCAM_ACTION_SMALLm;
    }

    SHR_IF_ERR_EXIT(soc_mem_array_read(unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));

    soc_mem_field_get(unit, action_tbl, data, ACTIONf, payload);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_payload_write(
    int unit,
    int core,
    uint32 action_bank_id,
    uint32 bank_offset,
    uint32 *payload)
{
    uint32 data[MAX_DOUBLE_ACTION_WORD_SIZE] = { 0 };
    soc_mem_t action_tbl;

    SHR_FUNC_INIT_VARS(unit);

    if (action_bank_id < dnx_data_field.tcam.nof_big_banks_get(unit) * 2)
    {
        action_tbl = TCAM_TCAM_ACTIONm;
    }
    else
    {
        action_tbl = TCAM_TCAM_ACTION_SMALLm;
    }

    soc_mem_field_set(unit, action_tbl, data, ACTIONf, payload);

    SHR_IF_ERR_EXIT(soc_mem_array_write(unit, action_tbl, action_bank_id, TCAM_BLOCK(unit, core), bank_offset, data));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_entry_key_read(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    uint32 *data_key,
    uint32 *data_mask,
    uint8 *key_valid_bits)
{
    soc_reg_above_64_val_t mem_val_read;
    soc_reg_above_64_val_t mem_val_write;
    uint32 tcam_bank_address;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val_write);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_RDf, 0x1);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val_write, TCAM_CPU_CMD_CMPf, 0x0);

    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_write));

    SOC_REG_ABOVE_64_CLEAR(mem_val_read);
    SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val_read));

    *key_valid_bits = soc_mem_field32_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_VALIDf);
    soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_key);

    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_write));

    SOC_REG_ABOVE_64_CLEAR(mem_val_read);
    SHR_IF_ERR_EXIT(READ_TCAM_TCAM_BANK_REPLYm(unit, TCAM_BLOCK(unit, core), tcam_bank_address + 1, mem_val_read));

    soc_mem_field_get(unit, TCAM_TCAM_BANK_REPLYm, mem_val_read, TCAM_CPU_REP_DOUTf, data_mask);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_entry_key_write(
    int unit,
    int core,
    uint32 bank_id,
    uint32 bank_offset,
    dnx_field_tcam_access_key_mode_e key_mode,
    dnx_field_tcam_access_part_e part,
    uint8 valid,
    uint32 *key,
    uint32 *key_mask)
{
    soc_reg_above_64_val_t mem_val;

    uint32 data[MAX_DOUBLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 data_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 neg_data_key[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 data_mask[MAX_SINGLE_KEY_WORD_SIZE + 1] = { 0 };
    uint32 tcam_bank_address;

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(mem_val);

    tcam_bank_address = DNX_FIELD_TCAM_LOCATION_BANK_TO_ABSOLUTE(bank_id, bank_offset);

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

    SHR_BITCOPY_RANGE(key, 0, data_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(key_mask, 0, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    SHR_BITAND_RANGE(data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, data_key);

    SHR_BITNEGATE_RANGE(data_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, neg_data_key);
    SHR_BITAND_RANGE(neg_data_key, data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE, data_mask);

    SHR_BITCOPY_RANGE(data, 0, data_key, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);
    SHR_BITCOPY_RANGE(data, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE,
                      data_mask, 0, SINGLE_KEY_BIT_SIZE + 2 * KEY_MODE_BIT_SIZE);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data);

    soc_mem_field32_set(unit, TCAM_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, valid);

    SHR_IF_ERR_EXIT(WRITE_TCAM_TCAM_BANK_COMMANDm(unit, TCAM_BLOCK(unit, core), tcam_bank_address, mem_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_tcam_hit_per_core_get(
    int unit,
    int core_id_specific,
    uint32 position,
    uint32 hw_bank_id,
    uint32 line_chunk_id,
    uint32 *mem_hit_field_value)
{
    uint32 bank_id;
    soc_mem_t mem = INVALIDm;
    uint32 mem_entry_data;

    SHR_FUNC_INIT_VARS(unit);

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);

    if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;
    }
    else
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;
    }

    mem_entry_data = 0;
    *mem_hit_field_value = 0;
    SHR_IF_ERR_EXIT(soc_mem_array_read
                    (unit, mem, hw_bank_id, TCAM_BLOCK(unit, core_id_specific), line_chunk_id, &mem_entry_data));
    soc_mem_field_get(unit, mem, &mem_entry_data, HITf, mem_hit_field_value);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_tcam_access_hw_v1_tcam_hit_per_core_flush(
    int unit,
    int core_id_specific,
    uint32 position,
    uint32 hw_bank_id,
    uint32 line_chunk_id,
    uint32 tcam_indirect_mask)
{
    soc_reg_above_64_val_t reg_val_zero;
    uint32 bank_id;
    soc_mem_t mem = INVALIDm;

#ifdef ADAPTER_SERVER_MODE
    uint32 mem_entry_data = 0;
    uint32 mem_hit_field_value = 0;
#else
    uint32 mem_addr;
    uint8 at;
    uint32 reg_val_addres = 0;
    uint32 reg_val_command = 0;
    int tcam_indirect_command_version = dnx_data_field.tcam.tcam_indirect_command_version_get(unit);
#endif

    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_val_zero);

    bank_id = DNX_FIELD_TCAM_LOCATION_ABSOLUTE_TO_BANK_ID(position);

    if (FIELD_TCAM_BANK_IS_BIG_BANK(bank_id))
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATIONm;
    }
    else
    {
        mem = TCAM_TCAM_ACTION_HIT_INDICATION_SMALLm;
    }

#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(soc_mem_array_read
                    (unit, mem, hw_bank_id, TCAM_BLOCK(unit, core_id_specific), line_chunk_id, &mem_entry_data));
    soc_mem_field_get(unit, mem, &mem_entry_data, HITf, &mem_hit_field_value);

    mem_entry_data = mem_entry_data & ~tcam_indirect_mask;
    SHR_IF_ERR_EXIT(soc_mem_array_write
                    (unit, mem, hw_bank_id, TCAM_BLOCK(unit, core_id_specific), line_chunk_id, &mem_entry_data));
#else

    mem_addr = soc_mem_addr_get(unit, mem, hw_bank_id, TCAM_BLOCK(unit, core_id_specific), line_chunk_id, &at);

    if (tcam_indirect_command_version == 1)
    {
        reg_val_command = 0xE00001;
    }
    else if (tcam_indirect_command_version == 2)
    {
        reg_val_command = 0xE000001;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal version of indirect command %d.\n", tcam_indirect_command_version);
    }
    soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val_addres, INDIRECT_COMMAND_ADDRf, mem_addr);
    soc_reg_field_set(unit, TCAM_INDIRECT_COMMAND_ADDRESSr, &reg_val_addres, INDIRECT_COMMAND_TYPEf, 0x0);
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_ADDRESSr(unit, core_id_specific, reg_val_addres));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_WR_MASKr(unit, core_id_specific, tcam_indirect_mask));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMAND_WR_DATAr(unit, core_id_specific, reg_val_zero));
    SHR_IF_ERR_EXIT(WRITE_TCAM_INDIRECT_COMMANDr(unit, core_id_specific, reg_val_command));
#endif

exit:
    SHR_FUNC_EXIT;
}
