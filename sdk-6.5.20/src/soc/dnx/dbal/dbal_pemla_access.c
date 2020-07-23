/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
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

#define DBAL_PEM_REG_WRITE_LOG(reg_id, field_id, pemla_field_id, val)                                                      \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Pemla access write Reg: reg_id=%d, field_name=%s=pemla_field_id=%d val=%x\n",\
                    reg_id, dbal_field_to_string(unit, field_id), pemla_field_id, val);

#define DBAL_PEM_REG_READ_LOG(reg_id, field_id, pemla_field_id, val)                                                       \
        LOG_INFO_EX(BSL_LOG_MODULE, "\n Pemla access read Reg: reg_id=%d, field_name=%s pemla_field_id=%d val=%x\n",\
                    reg_id, dbal_field_to_string(unit, field_id), pemla_field_id, val);

pemladrv_mem_t pemla_mem_pool[BCM_MAX_NUM_UNITS][DBAL_SW_NOF_ENTRY_HANDLES];


shr_error_e
dbal_pemla_verify_tables(
    int unit)
{
    dbal_logical_table_t *table;
    dbal_table_field_info_t *field_info = NULL;
    pemladrv_reg_info *pemla_reg_info;
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

        if (table->access_method == DBAL_ACCESS_METHOD_PEMLA)
        {
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
                continue;
            }

            for (pem_reg_index = 0; pem_reg_index < pemla_reg_info_size; pem_reg_index++)
            {
                char *ucode_table_name;

                ucode_table_name = &pemla_reg_info[pem_reg_index].reg_name[2];

                if (sal_strcasecmp(ucode_table_name, pemla_table_name) == 0)
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

            
            for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
            {
                multi_res_info_t *multi_res_info = &table->multi_res_info[res_type_index];

                for (field_index = 0; field_index < multi_res_info->nof_result_fields; field_index++)
                {
                    CONST char *field_name;

                    field_info = &multi_res_info->results_info[field_index];

                    field_name = dbal_field_to_string(unit, field_info->field_id);

                    for (pem_field_index = 0; pem_field_index < pemla_reg_info[pem_reg_index].nof_fields;
                         pem_field_index++)
                    {
                        pem_field_info = &pemla_reg_info[pem_reg_index].ptr[pem_field_index];

                        if (sal_strcasecmp(field_name, pem_field_info->field_name) == 0)
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
    }

exit:
    SHR_IF_ERR_EXIT(dnx_pemladrv_reg_info_de_allocate(unit, pemla_reg_info));
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
                             "Illegal field access %s , Field is Mapped Invalid due to mismtach between ARCH & APPL DB\n",
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

    SHR_IF_ERR_EXIT(pemladrv_reg_write(unit, entry_handle->table->pemla_mapping.reg_mem_id, pemla_drv_mem));

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
                             "Illegal field access %s , Field is Mapped Invalid due to mismtach between ARCH & APPL DB\n",
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
            
            SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_entry_set(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error table type %d\n", entry_handle->table->table_type);
            break;
    }
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

    
    SHR_IF_ERR_EXIT(pemladrv_reg_read(unit, entry_handle->table->pemla_mapping.reg_mem_id, pemla_reg_result));

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
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "pemla field id[%d] not fount in table field mapping\n", pemla_field_id);
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
            SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_entry_get(unit, entry_handle));
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
                    (unit, entry_handle->table->pemla_mapping.reg_mem_id, entry_payload_size,
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
            SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_entry_clear(unit, entry_handle));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error table type %d\n", entry_handle->table->table_type);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_pemla_table_pemla_reg_table_clear(
    int unit,
    dbal_entry_handle_t * entry_handle)
{
    SHR_FUNC_INIT_VARS(unit);

    if (entry_handle->table->nof_key_fields == 0)
    {
        SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_entry_clear(unit, entry_handle));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "not supported for tables with key fields\n");
    }

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
            SHR_IF_ERR_EXIT(dbal_pemla_table_pemla_reg_table_clear(unit, entry_handle));
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
            if (!iterator_info->used_first_key)
            {
                entry_handle->get_all_fields = 1;
                SHR_IF_ERR_EXIT(dbal_pemla_table_entry_get(unit, entry_handle));
                iterator_info->used_first_key = 1;
                {
                    if (sal_memcmp
                        (zero_buffer_to_compare, entry_handle->phy_entry.payload, DBAL_PHYSICAL_RES_SIZE_IN_BYTES) == 0)
                    {
                        
                        iterator_info->is_end = 1;
                    }
                }
            }
            else
            {
                iterator_info->is_end = 1;
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
