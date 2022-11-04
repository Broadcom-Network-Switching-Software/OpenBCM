/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALACCESSDNX
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include "dbal_internal.h"
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_bitstream.h>
#include "include/soc/dnx/pemladrv/auto_generated/pemladrv.h"

#include <soc/cmic.h>
#define DBAL_PEM_REG_TABLE_PREFIX "vr"
#define DBAL_PEM_MEM_TABLE_PREFIX "cDb"
#define DBAL_PEM_TABLE_PREFIX_MAX_SIZE 3

#define DBAL_PEM_REG_WRITE_LOG(reg_id, field_id, pemla_field_id, val)                                                      \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Pemla access write Reg: reg_id=%d, field_name=%s=pemla_field_id=%d val=%x\n",\
                    reg_id, dbal_field_to_string(unit, field_id), pemla_field_id, val);

#define DBAL_PEM_REG_READ_LOG(reg_id, field_id, pemla_field_id, val)                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Pemla access read Reg: reg_id=%d, field_name=%s pemla_field_id=%d val=%x\n",\
                    reg_id, dbal_field_to_string(unit, field_id), pemla_field_id, val);

pemladrv_mem_t pemla_mem_pool[BCM_MAX_NUM_UNITS][DBAL_SW_NOF_ENTRY_HANDLES];

#define DBAL_PEMLA_WRITE_CORE_ID ((entry_handle->phy_entry.core_id == DBAL_CORE_ALL) ? (-1) : entry_handle->phy_entry.core_id)
#define DBAL_PEMLA_READ_CORE_ID  ((entry_handle->phy_entry.core_id == DBAL_CORE_ALL) ? DBAL_CORE_DEFAULT : entry_handle->phy_entry.core_id)

shr_error_e
dbal_pemla_reg_tables_init(
    int unit)
{
    dbal_logical_table_t *table;
    dbal_table_field_info_t *field_info = NULL;
    pemladrv_reg_info *pemla_reg_info = NULL;
    int pemla_reg_info_size;
    dbal_tables_e table_id;
    int pem_reg_index, pem_field_index;
    int field_index;
    int res_type_index;
    char **pemla_table_name_tokens;
    char *pemla_table_name;
    unsigned int num_of_tokens;
    dbal_table_status_e table_status;
    pemladrv_reg_field_info *pem_field_info = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_reg_info_allocate(unit, &pemla_reg_info, &pemla_reg_info_size));

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

        if ((table->access_method != DBAL_ACCESS_METHOD_PEMLA)
            || ((table->core_mode == DBAL_CORE_MODE_SBC) && (table->nof_key_fields > 0))
            || ((table->core_mode == DBAL_CORE_MODE_DPC) && (table->nof_key_fields > 1)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        if (sw_state_is_warm_boot(unit))
        {
            if (table_status == DBAL_TABLE_HW_ERROR)
            {
                table->maturity_level = DBAL_MATURITY_LOW;
                continue;
            }
        }
        if (table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        pemla_table_name_tokens = utilex_str_split(table->table_name, "_", 2, &num_of_tokens);
        if (pemla_table_name_tokens != NULL)
        {
            pemla_table_name = pemla_table_name_tokens[1];
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Table name %s is not valid", dbal_logical_table_to_string(unit, table_id));
        }

        for (pem_reg_index = 0; pem_reg_index < pemla_reg_info_size; pem_reg_index++)
        {
            char *ucode_table_name;
            int prefix_length = sal_strnlen(DBAL_PEM_REG_TABLE_PREFIX, DBAL_PEM_TABLE_PREFIX_MAX_SIZE);

            ucode_table_name = &pemla_reg_info[pem_reg_index].reg_name[prefix_length];

            if (sal_strncasecmp(ucode_table_name, pemla_table_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                break;
            }
        }
        utilex_str_split_free(pemla_table_name_tokens, 2);

        if (pem_reg_index == pemla_reg_info_size)
        {

            table->maturity_level = DBAL_MATURITY_LOW;
            SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
            continue;
        }

        table->pemla_mapping.reg_mem_id = pemla_reg_info[pem_reg_index].reg_id;
        table->pemla_mapping.access_type = DBAL_HL_ACCESS_REGISTER;

        for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
        {
            multi_res_info_t *multi_res_info = &table->multi_res_info[res_type_index];

            for (field_index = 0; field_index < multi_res_info->nof_result_fields; field_index++)
            {
                CONST char *field_name;

                field_info = &multi_res_info->results_info[field_index];

                field_name = dbal_field_to_string(unit, field_info->field_id);

                for (pem_field_index = 0; pem_field_index < pemla_reg_info[pem_reg_index].nof_fields; pem_field_index++)
                {
                    pem_field_info = &pemla_reg_info[pem_reg_index].ptr[pem_field_index];

                    if (sal_strncasecmp
                        (field_name, pem_field_info->field_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
                    {
                        break;
                    }
                }

                if (pem_field_index == pemla_reg_info[pem_reg_index].nof_fields)
                {

                    table->pemla_mapping.result_fields_mapping[field_index] = DBAL_PEMLA_FIELD_MAPPING_INVALID;
                    field_info->field_nof_bits = 0;

                    table->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
                    continue;
                }

                table->pemla_mapping.result_fields_mapping[field_index] = pem_field_info->field_id;
                if (field_info->field_nof_bits > pem_field_info->field_size_in_bits)
                {

                    field_info->field_nof_bits = pem_field_info->field_size_in_bits;

                    table->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
                    continue;
                }
                else if (field_info->field_nof_bits < pem_field_info->field_size_in_bits)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "table[%s] APPL field size[%d]<ARCH field size[%d]\n",
                                 dbal_logical_table_to_string(unit, table_id), field_info->field_nof_bits,
                                 pem_field_info->field_size_in_bits);
                }
            }
        }
    }

exit:
    dnx_pemladrv_reg_info_de_allocate(unit, pemla_reg_info);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_direct_tables_init(
    int unit)
{
    dbal_logical_table_t *table;
    pemladrv_db_info *pemla_mem_info = NULL;
    int pemla_mem_info_size;
    dbal_tables_e table_id;
    int pem_mem_index;
    dbal_table_status_e table_status;

#if 0
    int pem_field_index = 0;
    int field_index;
    int res_type_index;
    pemladrv_db_field_info *pem_field_info = NULL;
    dbal_table_field_info_t *field_info = NULL;
#endif

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_pemladrv_direct_info_allocate(unit, &pemla_mem_info, &pemla_mem_info_size));

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

        if ((table->access_method != DBAL_ACCESS_METHOD_PEMLA)
            || ((table->core_mode == DBAL_CORE_MODE_SBC) && (table->nof_key_fields == 0))
            || ((table->core_mode == DBAL_CORE_MODE_DPC) && (table->nof_key_fields == 1)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        if (sw_state_is_warm_boot(unit))
        {
            if (table_status == DBAL_TABLE_HW_ERROR)
            {
                table->maturity_level = DBAL_MATURITY_LOW;
                continue;
            }
        }
        if (table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        for (pem_mem_index = 0; pem_mem_index < pemla_mem_info_size; pem_mem_index++)
        {
            char *ucode_table_name;
            int prefix_length = sal_strnlen(DBAL_PEM_MEM_TABLE_PREFIX, DBAL_PEM_TABLE_PREFIX_MAX_SIZE);

            ucode_table_name = &pemla_mem_info[pem_mem_index].db_name[prefix_length];

            if (sal_strncasecmp(table->table_name, ucode_table_name, sizeof(table->table_name)) == 0)
            {
                break;
            }
        }

        if (pem_mem_index == pemla_mem_info_size)
        {

            table->maturity_level = DBAL_MATURITY_LOW;
            SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
            continue;
        }

        table->pemla_mapping.reg_mem_id = pemla_mem_info[pem_mem_index].db_id;
        table->pemla_mapping.access_type = DBAL_HL_ACCESS_MEMORY;

#if 0

        for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
        {
            multi_res_info_t *multi_res_info = &table->multi_res_info[res_type_index];

            for (field_index = 0; field_index < multi_res_info->nof_result_fields; field_index++)
            {
                CONST char *field_name;

                field_info = &multi_res_info->results_info[field_index];

                field_name = dbal_field_to_string(unit, field_info->field_id);

                for (pem_field_index = 0; pem_field_index < pemla_mem_info[pem_mem_index].nof_fields; pem_field_index++)
                {
                    pem_field_info = &pemla_mem_info[pem_mem_index].ptr[pem_field_index];

                    if ((sal_strncasecmp
                         (field_name, pem_field_info->field_name, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0))
                    {
                        pem_field_info = &pemla_mem_info[pem_mem_index].ptr[pem_field_index];
                        break;
                    }
                }

                if (pem_field_index == pemla_mem_info[pem_mem_index].nof_fields)
                {

                    table->pemla_mapping.result_fields_mapping[field_index] = DBAL_PEMLA_FIELD_MAPPING_INVALID;
                    field_info->field_nof_bits = 0;

                    table->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
                    continue;
                }

                table->pemla_mapping.result_fields_mapping[field_index] =
                    pemla_mem_info[pem_mem_index].ptr[pem_field_index].field_id;
                if (table->multi_res_info->entry_payload_size < pem_field_info->field_size_in_bits)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "table[%s] APPL payload sise [%d] < ARCH payload size[%d]\n",
                                 dbal_logical_table_to_string(unit, table_id),
                                 table->multi_res_info->entry_payload_size, pem_field_info->field_size_in_bits);
                }

                if (field_info->field_nof_bits > pem_field_info->field_size_in_bits)
                {

                    field_info->field_nof_bits = pem_field_info->field_size_in_bits;

                    table->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
                    continue;
                }
                else if (field_info->field_nof_bits < pem_field_info->field_size_in_bits)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "table[%s] APPL field size[%d]<ARCH field size[%d]\n",
                                 dbal_logical_table_to_string(unit, table_id), field_info->field_nof_bits,
                                 pem_field_info->field_size_in_bits);
                }
            }
        }
#endif

    }

exit:
    if (pemla_mem_info)
    {
        dnx_pemladrv_direct_info_de_allocate(unit, pemla_mem_info);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_tables_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_pemla_direct_tables_init(unit));
    SHR_IF_ERR_EXIT(dbal_pemla_reg_tables_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_init(
    int unit)
{
    uint32 *field_value = NULL;
    uint8 handle_index;
    uint32 field_index;

    pemladrv_mem_t *pemla_drv_mem;

    pemladrv_field_t *pemla_drv_fields_ptr = NULL;
    pemladrv_field_t **pemla_drv_fields_pptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    for (handle_index = 0; handle_index < DBAL_SW_NOF_ENTRY_HANDLES; handle_index++)
    {
        pemla_drv_mem = &pemla_mem_pool[unit][handle_index];
        pemla_drv_mem->nof_fields = 0;
        pemla_drv_mem->flags = 0;
        field_index = 0;

        SHR_ALLOC_SET_ZERO(pemla_drv_fields_pptr,
                           (sizeof(pemla_drv_fields_ptr) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE),
                           "pemla db fields array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        pemla_drv_mem->fields = pemla_drv_fields_pptr;

        SHR_ALLOC_SET_ZERO(pemla_drv_fields_ptr,
                           (sizeof(pemladrv_field_t) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE),
                           "pemla db fields array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        pemla_drv_mem->fields[field_index] = pemla_drv_fields_ptr;

        SHR_ALLOC_SET_ZERO(field_value,
                           (DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE * DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES),
                           "pemla db fields value array", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        for (field_index = 0; field_index < DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE; field_index++)
        {
            pemla_drv_mem->fields[field_index] = pemla_drv_fields_ptr;
            pemla_drv_fields_ptr++;
            pemla_drv_mem->fields[field_index]->field_id = 0;
            pemla_drv_mem->fields[field_index]->fldbuf = field_value;
            field_value += DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS;
            pemla_drv_mem->fields[field_index]->flags = 0;
        }

        field_value = NULL;
        pemla_drv_fields_ptr = NULL;
        pemla_drv_fields_pptr = NULL;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_deinit(
    int unit)
{
    uint8 handle_index;
    pemladrv_mem_t *pemla_drv_mem;

    SHR_FUNC_INIT_VARS(unit);

    for (handle_index = 0; handle_index < DBAL_SW_NOF_ENTRY_HANDLES; handle_index++)
    {
        pemla_drv_mem = &pemla_mem_pool[unit][handle_index];
        if (pemla_drv_mem->fields != NULL)
        {
            if (pemla_drv_mem->fields[0] != NULL)
            {
                SHR_FREE(pemla_drv_mem->fields[0]->fldbuf);
            }
            SHR_FREE(pemla_drv_mem->fields[0]);
            SHR_FREE(pemla_drv_mem->fields);
        }
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_reg_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int field_index;
    dbal_table_field_info_t *table_field_info;

    pemladrv_mem_t *pemla_drv_mem = &pemla_mem_pool[unit][entry_handle->handle_id];
    int nof_results_field;

    SHR_FUNC_INIT_VARS(unit);

    pemla_drv_mem->nof_fields = 0;

    nof_results_field = DBAL_RES_INFO.nof_result_fields;

    for (field_index = 0; field_index < nof_results_field; field_index++)
    {
        int field_index_in_pemla = pemla_drv_mem->nof_fields;
        table_field_info = &(DBAL_RES_INFO.results_info[field_index]);

        if (entry_handle->value_field_ids[field_index] == DBAL_FIELD_EMPTY)
        {

            continue;
        }

        if (entry_handle->table->pemla_mapping.result_fields_mapping[field_index] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
        {
            if (dbal_mngr_status_get(unit) != DBAL_STATUS_DEVICE_INIT_DONE)
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Operation skipped table %s due to HW issues\n\n"),
                             entry_handle->table->table_name));
                continue;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal field access %s , Virtual register doesn't have HW mapping, due to not being used in Ucode\n",
                             dbal_field_to_string(unit, entry_handle->value_field_ids[field_index]));
            }
        }

        sal_memset(pemla_drv_mem->fields[field_index_in_pemla]->fldbuf, 0, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_handle->phy_entry.payload, table_field_info->bits_offset_in_buffer,
                         table_field_info->field_nof_bits, pemla_drv_mem->fields[field_index_in_pemla]->fldbuf));

        pemla_drv_mem->fields[field_index_in_pemla]->field_id =
            entry_handle->table->pemla_mapping.result_fields_mapping[field_index];
        pemla_drv_mem->fields[field_index_in_pemla]->flags = 0;

        DBAL_PEM_REG_WRITE_LOG(entry_handle->table->pemla_mapping.reg_mem_id,
                               entry_handle->value_field_ids[field_index],
                               entry_handle->table->pemla_mapping.result_fields_mapping[field_index],
                               pemla_drv_mem->fields[field_index_in_pemla]->fldbuf[0]);

        pemla_drv_mem->nof_fields++;
    }

    SHR_IF_ERR_EXIT(pemladrv_reg_write
                    (unit, DBAL_PEMLA_WRITE_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id, pemla_drv_mem));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_mem_raw_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    pemladrv_mem_t *pemla_drv_mem = &pemla_mem_pool[unit][entry_handle->handle_id];

    SHR_FUNC_INIT_VARS(unit);

    pemla_drv_mem->nof_fields = 1;
    pemla_drv_mem->fields[0]->field_id = 0;
    pemla_drv_mem->fields[0]->flags = 0;

    sal_memset(pemla_drv_mem->fields[0]->fldbuf, 0, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES));
    sal_memcpy(pemla_drv_mem->fields[0]->fldbuf, entry_handle->phy_entry.payload,
               sizeof(pemla_drv_mem->fields[0]->fldbuf[0]));

    SHR_IF_ERR_EXIT(pemladrv_direct_write
                    (unit, DBAL_PEMLA_WRITE_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id,
                     entry_handle->phy_entry.key[0], pemla_drv_mem));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_mem_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int field_index;
    dbal_table_field_info_t *table_field_info;
    int field_index_in_pemla;

    pemladrv_mem_t *pemla_drv_mem = &pemla_mem_pool[unit][entry_handle->handle_id];
    int nof_results_field;

    SHR_FUNC_INIT_VARS(unit);

    pemla_drv_mem->nof_fields = 0;

    nof_results_field = DBAL_RES_INFO.nof_result_fields;

    for (field_index = 0; field_index < nof_results_field; field_index++)
    {
        field_index_in_pemla = pemla_drv_mem->nof_fields;

        table_field_info = &(DBAL_RES_INFO.results_info[field_index]);

        if (entry_handle->value_field_ids[field_index] == DBAL_FIELD_EMPTY)
        {

            continue;
        }

        if (entry_handle->table->pemla_mapping.result_fields_mapping[field_index] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
        {
            if (dbal_mngr_status_get(unit) != DBAL_STATUS_DEVICE_INIT_DONE)
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Operation skipped table %s due to HW issues\n\n"),
                             entry_handle->table->table_name));
                continue;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal field access %s , Field is Mapped Invalid due to mismatch between ARCH & APPL DB\n",
                             dbal_field_to_string(unit, entry_handle->value_field_ids[field_index]));
            }
        }

        sal_memset(pemla_drv_mem->fields[field_index_in_pemla]->fldbuf, 0, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES));

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (entry_handle->phy_entry.payload, table_field_info->bits_offset_in_buffer,
                         table_field_info->field_nof_bits, pemla_drv_mem->fields[field_index_in_pemla]->fldbuf));

        pemla_drv_mem->fields[field_index_in_pemla]->field_id =
            entry_handle->table->pemla_mapping.result_fields_mapping[field_index];
        pemla_drv_mem->fields[field_index_in_pemla]->flags = 0;

        DBAL_PEM_REG_WRITE_LOG(entry_handle->table->pemla_mapping.reg_mem_id,
                               entry_handle->value_field_ids[field_index],
                               entry_handle->table->pemla_mapping.result_fields_mapping[field_index],
                               pemla_drv_mem->fields[field_index_in_pemla]->fldbuf[0]);

        pemla_drv_mem->nof_fields++;
    }

    SHR_IF_ERR_EXIT(pemladrv_direct_write
                    (unit, DBAL_PEMLA_WRITE_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id,
                     entry_handle->phy_entry.key[0], pemla_drv_mem));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_pemla_table_pemla_set_default_entry(
    int unit,
    dbal_entry_handle_t * entry_handle,
    pemladrv_mem_t ** pemla_drv_mem_out)
{
    int field_index;

    pemladrv_mem_t *pemla_drv_mem = &pemla_mem_pool[unit][entry_handle->handle_id];
    int nof_results_field;

    SHR_FUNC_INIT_VARS(unit);

    *pemla_drv_mem_out = NULL;
    pemla_drv_mem->nof_fields = 0;

    nof_results_field = DBAL_RES_INFO.nof_result_fields;

    if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_MEMORY)
    {

        nof_results_field = 1;
    }

    for (field_index = 0; field_index < nof_results_field; field_index++)
    {
        int field_index_in_pemla = pemla_drv_mem->nof_fields;

        if (entry_handle->table->pemla_mapping.result_fields_mapping[field_index] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
        {
            continue;
        }

        sal_memset(pemla_drv_mem->fields[field_index_in_pemla]->fldbuf, 0, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES));

        pemla_drv_mem->fields[field_index_in_pemla]->field_id =
            entry_handle->table->pemla_mapping.result_fields_mapping[field_index];
        pemla_drv_mem->fields[field_index_in_pemla]->flags = 0;

        DBAL_PEM_REG_WRITE_LOG(entry_handle->table->pemla_mapping.reg_mem_id,
                               entry_handle->value_field_ids[field_index],
                               entry_handle->table->pemla_mapping.result_fields_mapping[field_index],
                               pemla_drv_mem->fields[field_index_in_pemla]->fldbuf[0]);

        pemla_drv_mem->nof_fields++;
    }

    *pemla_drv_mem_out = pemla_drv_mem;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_reg_clear_entry(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    pemladrv_mem_t *pemla_drv_mem;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_set_default_entry(unit, entry_handle, &pemla_drv_mem));
    SHR_IF_ERR_EXIT(pemladrv_reg_write
                    (unit, DBAL_PEMLA_WRITE_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id, pemla_drv_mem));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_mem_clear_entry(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    pemladrv_mem_t *pemla_drv_mem;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_set_default_entry(unit, entry_handle, &pemla_drv_mem));
    SHR_IF_ERR_EXIT(pemladrv_direct_write(unit, DBAL_PEMLA_WRITE_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id,
                                          entry_handle->phy_entry.key[0], pemla_drv_mem));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_entry_set(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table type not supported %s\n",
                         dbal_table_type_to_string(unit, entry_handle->table->table_type));
            break;

        case DBAL_TABLE_TYPE_DIRECT:
            if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_REGISTER)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_entry_set(unit, entry_handle));
            }
            else if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_mem_raw_entry_set(unit, entry_handle));

            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in pemla table type %d, should be register or memory only.\n",
                             entry_handle->table->pemla_mapping.access_type);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in table type %d\n", entry_handle->table->table_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_mem_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int field_index, pemla_field_index;
    dbal_table_field_info_t *table_field_info;
    pemladrv_mem_t *pemla_mem_result = &pemla_mem_pool[unit][entry_handle->handle_id];
    int nof_results_field;
    SHR_FUNC_INIT_VARS(unit);

    pemla_mem_result->nof_fields = 0;

    nof_results_field = DBAL_RES_INFO.nof_result_fields;

    for (field_index = 0; field_index < nof_results_field; field_index++)
    {
        table_field_info = &(DBAL_RES_INFO.results_info[field_index]);

        if (entry_handle->table->pemla_mapping.result_fields_mapping[field_index] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
        {

            continue;
        }

        pemla_mem_result->fields[pemla_mem_result->nof_fields]->field_id =
            entry_handle->table->pemla_mapping.result_fields_mapping[field_index];
        pemla_mem_result->fields[pemla_mem_result->nof_fields]->flags = 0;
        pemla_mem_result->nof_fields++;
    }

    SHR_IF_ERR_EXIT(pemladrv_direct_read
                    (unit, DBAL_PEMLA_READ_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id,
                     entry_handle->phy_entry.key[0], pemla_mem_result));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla does not support multiple result\n");
    }

    for (pemla_field_index = 0; pemla_field_index < pemla_mem_result->nof_fields; pemla_field_index++)
    {
        field_id_t pemla_field_id;

        pemla_field_id = pemla_mem_result->fields[pemla_field_index]->field_id;

        for (field_index = 0; field_index < nof_results_field; field_index++)
        {
            if (pemla_field_id == entry_handle->table->pemla_mapping.result_fields_mapping[field_index])
            {
                if (!entry_handle->get_all_fields)
                {
                    if (entry_handle->value_field_ids[field_index] == DBAL_FIELD_EMPTY)
                    {
                        continue;
                    }
                }
                break;
            }
        }

        table_field_info = &(DBAL_RES_INFO.results_info[field_index]);
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        ((uint32 *) (pemla_mem_result->fields[pemla_field_index]->fldbuf),
                         table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                         entry_handle->phy_entry.payload));

        DBAL_PEM_REG_READ_LOG(entry_handle->table->pemla_mapping.reg_mem_id, table_field_info->field_id,
                              pemla_field_id, pemla_mem_result->fields[pemla_field_index]->fldbuf[0]);

        entry_handle->phy_entry.payload_size += table_field_info->field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_mem_raw_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    pemladrv_mem_t *pemla_mem_result = &pemla_mem_pool[unit][entry_handle->handle_id];
    uint32 result;
    SHR_FUNC_INIT_VARS(unit);

    pemla_mem_result->fields[0]->field_id = 0;
    pemla_mem_result->fields[0]->flags = 0;
    pemla_mem_result->nof_fields = 1;

    SHR_IF_ERR_EXIT(pemladrv_direct_read
                    (unit, DBAL_PEMLA_READ_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id,
                     entry_handle->phy_entry.key[0], pemla_mem_result));

    result = ((*pemla_mem_result->fields[0]->fldbuf) & UTILEX_BITS_MASK((entry_handle->phy_entry.payload_size - 1), 0));
    sal_memcpy(entry_handle->phy_entry.payload, &result, sizeof(pemla_mem_result->fields[0]->fldbuf[0]));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_reg_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int field_index, pemla_field_index;
    dbal_table_field_info_t *table_field_info;

    pemladrv_mem_t *pemla_reg_result = &pemla_mem_pool[unit][entry_handle->handle_id];
    int nof_results_field;

    SHR_FUNC_INIT_VARS(unit);

    pemla_reg_result->nof_fields = 0;

    nof_results_field = DBAL_RES_INFO.nof_result_fields;

    for (field_index = 0; field_index < nof_results_field; field_index++)
    {
        table_field_info = &(DBAL_RES_INFO.results_info[field_index]);

        if (entry_handle->table->pemla_mapping.result_fields_mapping[field_index] == DBAL_PEMLA_FIELD_MAPPING_INVALID)
        {

            continue;
        }

        pemla_reg_result->fields[pemla_reg_result->nof_fields]->field_id =
            entry_handle->table->pemla_mapping.result_fields_mapping[field_index];
        pemla_reg_result->fields[pemla_reg_result->nof_fields]->flags = 0;
        pemla_reg_result->nof_fields++;
    }

    SHR_IF_ERR_EXIT(pemladrv_reg_read
                    (unit, DBAL_PEMLA_READ_CORE_ID, entry_handle->table->pemla_mapping.reg_mem_id, pemla_reg_result));

    if (SHR_IS_BITSET(entry_handle->table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {

        SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla does not support multiple result\n");
    }

    for (pemla_field_index = 0; pemla_field_index < pemla_reg_result->nof_fields; pemla_field_index++)
    {
        field_id_t pemla_field_id;

        pemla_field_id = pemla_reg_result->fields[pemla_field_index]->field_id;

        for (field_index = 0; field_index < nof_results_field; field_index++)
        {
            if (pemla_field_id == entry_handle->table->pemla_mapping.result_fields_mapping[field_index])
            {
                if (!entry_handle->get_all_fields)
                {
                    if (entry_handle->value_field_ids[field_index] == DBAL_FIELD_EMPTY)
                    {
                        continue;
                    }
                }
                break;
            }
        }

        if (field_index == nof_results_field)
        {
            if (entry_handle->get_all_fields)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla field id[%d] not found in table field mapping\n", pemla_field_id);
            }
            else
            {
                continue;
            }
        }

        table_field_info = &(DBAL_RES_INFO.results_info[field_index]);
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        ((uint32 *) (pemla_reg_result->fields[pemla_field_index]->fldbuf),
                         table_field_info->bits_offset_in_buffer, table_field_info->field_nof_bits,
                         entry_handle->phy_entry.payload));

        DBAL_PEM_REG_READ_LOG(entry_handle->table->pemla_mapping.reg_mem_id, table_field_info->field_id,
                              pemla_field_id, pemla_reg_result->fields[pemla_field_index]->fldbuf[0]);

        entry_handle->phy_entry.payload_size += table_field_info->field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_entry_get(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->cur_res_type == DBAL_RESULT_TYPE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type not initialized table %s\n", entry_handle->table->table_name);
    }

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table type not supported %s\n",
                         dbal_table_type_to_string(unit, entry_handle->table->table_type));
            break;

        case DBAL_TABLE_TYPE_DIRECT:
            if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_REGISTER)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_entry_get(unit, entry_handle));
            }
            else if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_mem_raw_entry_get(unit, entry_handle));

            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in pemla table type %d, should be register or memory only.\n",
                             entry_handle->table->pemla_mapping.access_type);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error table type %d\n", entry_handle->table->table_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_pemla_reg_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int entry_payload_size;
    unsigned int *entry_value = NULL;

    SHR_FUNC_INIT_VARS(unit);

    entry_payload_size = entry_handle->table->multi_res_info[0].entry_payload_size;

    SHR_ALLOC_SET_ZERO(entry_value, WORDS2BYTES(BITS2WORDS(entry_payload_size)), "pemla entry value array",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_pemladrv_full_reg_write
                    (unit, -1, entry_handle->table->pemla_mapping.reg_mem_id, entry_payload_size,
                     (const unsigned int *) entry_value));

exit:
    sal_free(entry_value);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_entry_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table type not supported %s\n",
                         dbal_table_type_to_string(unit, entry_handle->table->table_type));
            break;

        case DBAL_TABLE_TYPE_DIRECT:
            if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_REGISTER)
            {

                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_clear_entry(unit, entry_handle));
            }
            else if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_mem_clear_entry(unit, entry_handle));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in pemla table type %d, should be register or memory only.\n",
                             entry_handle->table->pemla_mapping.access_type);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error table type %d\n", entry_handle->table->table_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_pemla_table_mem_iterator_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    int rv;
    dbal_key_value_validity_e is_valid_entry = DBAL_KEY_IS_INVALID;
    uint8 entry_found = FALSE;
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    while ((!entry_found) && (!iterator_info->is_end))
    {
        if (!iterator_info->used_first_key)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {

                entry_handle->phy_entry.core_id = DBAL_MAX_NUM_OF_CORES - 1;
            }
            else
            {

                entry_handle->phy_entry.core_id = 0;
            }
            iterator_info->used_first_key = TRUE;
        }
        else
        {
            if (entry_handle->phy_entry.key[0] == iterator_info->max_num_of_iterations)
            {
                entry_handle->phy_entry.core_id--;
                if (entry_handle->phy_entry.core_id < 0)
                {
                    break;
                }
                else
                {
                    entry_handle->phy_entry.key[0] = 0;
                }
            }
            else
            {
                entry_handle->phy_entry.key[0]++;
            }
        }

        SHR_IF_ERR_EXIT(dbal_key_buffer_validate(unit, entry_handle, &is_valid_entry));

        if (is_valid_entry == DBAL_KEY_IS_VALID)
        {
            sal_memset(entry_handle->phy_entry.payload, 0, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
            entry_handle->get_all_fields = TRUE;

            rv = dbal_pemla_table_entry_get(unit, entry_handle);
            if (rv == _SHR_E_NONE)
            {

                if (iterator_info->mode == DBAL_ITER_MODE_ALL)
                {
                    entry_found = TRUE;
                }
                else if ((iterator_info->mode == DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT) ||
                         (iterator_info->mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE))
                {
                    if (sal_memcmp
                        (zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) == 0)
                    {

                        continue;
                    }
                    else
                    {
                        entry_found = TRUE;
                    }
                }
            }
            else if (rv != _SHR_E_NOT_FOUND)
            {

                if (((entry_handle->phy_entry.key[0] + 1) == iterator_info->max_num_of_iterations) &&
                    (entry_handle->phy_entry.core_id == 0))
                {
                    iterator_info->is_end = TRUE;
                }
                SHR_ERR_EXIT(rv, "PEMLA entry get");
            }
        }
    }

    if (entry_found == FALSE)
    {
        iterator_info->is_end = TRUE;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_pemla_table_pemla_mem_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    SHR_IF_ERR_EXIT(dbal_entry_handle_clear_macro(unit, entry_handle->table_id, entry_handle->handle_id));

    iterator_info->mode = DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT;
    iterator_info->is_end = FALSE;
    iterator_info->used_first_key = FALSE;
    entry_handle->get_all_fields = 1;
    iterator_info->is_init = 1;

    SHR_IF_ERR_EXIT(dbal_iterator_init_handle_info(unit, entry_handle));
    SHR_IF_ERR_EXIT(dbal_pemla_table_iterator_init(unit, entry_handle));

    SHR_IF_ERR_EXIT(dbal_pemla_table_mem_iterator_get_next(unit, entry_handle));

    while (!iterator_info->is_end)
    {
        SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_mem_clear_entry(unit, entry_handle));
        SHR_IF_ERR_EXIT(dbal_pemla_table_mem_iterator_get_next(unit, entry_handle));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_pemla_table_pemla_reg_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_clear_entry(unit, entry_handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table type not supported %s\n",
                         dbal_table_type_to_string(unit, entry_handle->table->table_type));
            break;

        case DBAL_TABLE_TYPE_DIRECT:
            if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_REGISTER)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_table_clear(unit, entry_handle));
            }
            else if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_mem_table_clear(unit, entry_handle));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in pemla table type %d, should be register or memory only.\n",
                             entry_handle->table->pemla_mapping.access_type);
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error table type %d\n", entry_handle->table->table_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_iterator_init(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_DIRECT:
            SHR_IF_ERR_EXIT(dbal_tables_max_key_value_get
                            (unit, entry_handle->table_id, &iterator_info->max_num_of_iterations));
            break;
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "iterator operation not supported for pemla\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_iterator_deinit(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_DIRECT:
            break;
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "iterator operation not supported for pemla\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_pemla_table_reg_iterator_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;
    int entry_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    while (!entry_found && !iterator_info->is_end)
    {
        if (!iterator_info->used_first_key)
        {
            if (entry_handle->table->core_mode == DBAL_CORE_MODE_DPC)
            {

                entry_handle->phy_entry.core_id = DBAL_MAX_NUM_OF_CORES - 1;
            }
            else
            {

                entry_handle->phy_entry.core_id = 0;
            }
            iterator_info->used_first_key = 1;
        }
        else
        {
            entry_handle->phy_entry.core_id--;
            if (entry_handle->phy_entry.core_id < 0)
            {
                iterator_info->is_end = TRUE;
                break;
            }
        }

        entry_handle->get_all_fields = 1;
        SHR_IF_ERR_EXIT(dbal_pemla_table_entry_get(unit, entry_handle));

        if (iterator_info->mode == DBAL_ITER_MODE_ALL)
        {
            entry_found = TRUE;
        }
        else if ((iterator_info->mode == DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT) ||
                 (iterator_info->mode == DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE))
        {
            if (sal_memcmp
                (zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) == 0)
            {

                continue;
            }
            else
            {
                entry_found = TRUE;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_pemla_table_iterator_get_next(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    dbal_iterator_info_t *iterator_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle->handle_id, &iterator_info));

    switch (entry_handle->table->table_type)
    {
        case DBAL_TABLE_TYPE_DIRECT:
            if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_REGISTER)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_reg_iterator_get_next(unit, entry_handle));
            }
            else if (entry_handle->table->pemla_mapping.access_type == DBAL_HL_ACCESS_MEMORY)
            {
                SHR_IF_ERR_EXIT(dbal_pemla_table_mem_iterator_get_next(unit, entry_handle));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error in pemla table type %d, should be register or memory only.\n",
                             entry_handle->table->pemla_mapping.access_type);
            }
            break;
        case DBAL_TABLE_TYPE_EM:
        case DBAL_TABLE_TYPE_TCAM:
        case DBAL_TABLE_TYPE_TCAM_DIRECT:
        case DBAL_TABLE_TYPE_LPM:
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "iterator operation not supported for pemla\n");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
