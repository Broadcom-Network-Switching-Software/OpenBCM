/** \file dnxcmn.c
 *  
 *  Common utils for dnx only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

/*
 * Include files.
 * {
 */

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm/types.h>

#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/util.h>
#include <soc/dnx/dbal/dbal.h>
#include <sal/core/boot.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

/*
 * }
 */

/*
 * DBAL utils
 * {
 */
/**
 * See egq_dbal.h
 */
shr_error_e
dnx_dbal_gen_get(
    uint32 unit,
    dbal_tables_e dbal_table_id,
    uint32 num_keys,
    uint32 num_fields,
    ...)
{
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 dbal_value, key_index, field_index;
    uint32 *dbal_value_p;
    uint64 *dbal_value64_p;
    int inst_id;
    dnx_gen_dbal_field_e field_type;
    va_list ap;

    SHR_FUNC_INIT_VARS(unit);
    va_start(ap, num_fields);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    for (key_index = 0; key_index < num_keys; key_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value = va_arg(ap, uint32);
                dbal_entry_key_field32_set(unit, entry_handle_id, dbal_field_id, dbal_value);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for keys. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    /*
     * This is a 'get' operation. Each field identifier is followed by a pointer.
     */
    for (field_index = 0; field_index < num_fields; field_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        inst_id = va_arg(ap, uint32);
        if ((inst_id != INST_SINGLE) && (inst_id != INST_ALL) && (inst_id < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Input INSTANCE (%d) is illegal. It is negative and neither INST_SINGLE (%d) nor INST_ALL (%d). Quit.\r\n",
                         inst_id, INST_SINGLE, INST_ALL);
        }
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value_p = va_arg(ap, uint32 *);
                /*
                 * Always initialize to zero.
                 */
                *dbal_value_p = 0;
                dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value_p);
                break;
            }
            case GEN_DBAL_FIELD64:
            {
                dbal_value64_p = va_arg(ap, uint64 *);
                /*
                 * Always initialize to zero.
                 */
                COMPILER_64_ZERO(*dbal_value64_p);
                dbal_value_field64_request(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value64_p);
                break;
            }
            case GEN_DBAL_ARR32:
            {
                dbal_value_p = va_arg(ap, uint32 *);
                dbal_value_field_arr32_request(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value_p);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for result-fields. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    /*
     * Last field MUST be GEN_DBAL_FIELD_LAST_MARK
     */
    field_type = va_arg(ap, uint32);
    if (field_type != GEN_DBAL_FIELD_LAST_MARK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Input last field (%d) is illegal. It must be GEN_DBAL_FIELD_LAST_MARK (%d). Quit.\r\n",
                     field_type, GEN_DBAL_FIELD_LAST_MARK);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    va_end(ap);
    SHR_FUNC_EXIT;
}
/**
 * See egq_dbal.h
 */
shr_error_e
dnx_dbal_gen_set(
    uint32 unit,
    dbal_tables_e dbal_table_id,
    uint32 num_keys,
    uint32 num_fields,
    ...)
{
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 dbal_value, key_index, field_index;
    uint32 dbal_range_from_value, dbal_range_to_value;
    int inst_id;
    uint32 *dbal_value_p;
    dnx_gen_dbal_field_e field_type;
    va_list ap;

    SHR_FUNC_INIT_VARS(unit);
    va_start(ap, num_fields);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    for (key_index = 0; key_index < num_keys; key_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value = va_arg(ap, uint32);
                dbal_entry_key_field32_set(unit, entry_handle_id, dbal_field_id, dbal_value);
                break;
            }
            case GEN_DBAL_RANGE32:
            {
                dbal_range_from_value = va_arg(ap, uint32);
                dbal_range_to_value = va_arg(ap, uint32);
                dbal_entry_key_field32_range_set(unit, entry_handle_id, dbal_field_id, dbal_range_from_value,
                                                 dbal_range_to_value);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for keys. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    for (field_index = 0; field_index < num_fields; field_index++)
    {
        field_type = va_arg(ap, uint32);
        dbal_field_id = va_arg(ap, uint32);
        inst_id = va_arg(ap, uint32);
        if ((inst_id != INST_SINGLE) && (inst_id != INST_ALL) && (inst_id < 0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "Input INSTANCE (%d) is illegal. It is negative and neither INST_SINGLE (%d) nor INST_ALL (%d). Quit.\r\n",
                         inst_id, INST_SINGLE, INST_ALL);
        }
        switch (field_type)
        {
            case GEN_DBAL_FIELD32:
            {
                dbal_value = va_arg(ap, uint32);
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value);
                break;
            }
            case GEN_DBAL_ARR32:
            {
                dbal_value_p = va_arg(ap, uint32 *);
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, dbal_field_id, inst_id, dbal_value_p);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Currently, this field type (%d) is not supported for result-fields. See dnx_gen_dbal_field_e.\r\n",
                             field_type);
            }
        }
    }
    /*
     * Last field MUST be GEN_DBAL_FIELD_LAST_MARK
     */
    field_type = va_arg(ap, uint32);
    if (field_type != GEN_DBAL_FIELD_LAST_MARK)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Input last field (%d) is illegal. It must be GEN_DBAL_FIELD_LAST_MARK (%d). Quit.\r\n",
                     field_type, GEN_DBAL_FIELD_LAST_MARK);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    va_end(ap);
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Time
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnxcmn_polling(
    int unit,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 entry_handle_id,
    dbal_fields_e field,
    uint32 expected_value)
{
    soc_timeout_t to;
    uint32 result = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get field value
     */

    soc_timeout_init(&to, time_out, min_polls);
    for (;;)
    {
        dbal_value_field32_request(unit, entry_handle_id, field, INST_SINGLE, &result);
        /*
         * Get entry
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            result = expected_value;
        }
#endif
        if (result == expected_value)
        {
            break;
        }
        if (soc_timeout_check(&to))
        {
            LOG_ERROR(BSL_LS_SOC_REG,
                      (BSL_META("TIMEOUT when polling field: %s \n"), dbal_field_to_string(unit, field)));
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "_SHR_E_TIMEOUT");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */

/*
 * Time
 * {
 */

/*
 * See .h file
 */
shr_error_e
dnxcmn_polling_range(
    int unit,
    sal_usecs_t time_out,
    int32 min_polls,
    uint32 entry_handle_id,
    dbal_fields_e field,
    uint32 nof_valid_ranges,
    uint32 *expected_min_value,
    uint32 *expected_max_value)
{
    soc_timeout_t to;
    uint32 result = 0;
    int is_in_range = 0;
    uint32 i;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get field value
     */

    soc_timeout_init(&to, time_out, min_polls);
    for (;;)
    {
        dbal_value_field32_request(unit, entry_handle_id, field, INST_SINGLE, &result);
        /*
         * Get entry
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

#ifdef PLISIM
        if (SAL_BOOT_PLISIM)
        {
            break;
        }
#endif
        for (i = 0; i < nof_valid_ranges; i++)
        {
            if (result >= expected_min_value[i] && result <= expected_max_value[i])
            {
                is_in_range = 1;
                break;
            }
        }
        if (is_in_range == 1)
        {
            break;
        }
        if (soc_timeout_check(&to))
        {
            LOG_ERROR(BSL_LS_SOC_REG,
                      (BSL_META("TIMEOUT when polling field: %s \n"), dbal_field_to_string(unit, field)));
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "_SHR_E_TIMEOUT");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
