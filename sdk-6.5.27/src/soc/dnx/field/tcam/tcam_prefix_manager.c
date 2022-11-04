
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#include <soc/dnx/field/tcam/tcam_prefix_manager.h>

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <soc/dnx/field/tcam/tcam_access_profile_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_prefix_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

static shr_error_e
dnx_field_tcam_prefix_free_check(
    int unit,
    int bank_id,
    bcm_core_t core_id,
    uint32 prefix_size,
    uint32 prefix_value,
    uint8 *is_free)
{

    int nof_comp_bits = dnx_data_field.tcam.max_prefix_size_get(unit) - prefix_size;

    int nof_combs = SAL_BIT(nof_comp_bits);
    int comb;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    *is_free = TRUE;

    for (comb = 0; comb < nof_combs; comb++)
    {

        uint32 handler;
        int prefix_to_check = (prefix_value << nof_comp_bits) | comb;
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.core_prefix_map.
                        prefix_handler_map.get(unit, bank_id, core_id, prefix_to_check, &handler));
        if (handler != DNX_FIELD_TCAM_HANDLER_ID_INVALID)
        {
            *is_free = FALSE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_prefix_val_alloc(
    int unit,
    int bank_id,
    bcm_core_t core_id,
    uint32 handler_id,
    uint32 prefix_size,
    uint32 prefix_value)
{

    int nof_comp_bits = dnx_data_field.tcam.max_prefix_size_get(unit) - prefix_size;

    int nof_combs = SAL_BIT(nof_comp_bits);
    int comb;
    int extended_prefix_value;
    uint8 is_free;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_id, core_id, prefix_size, prefix_value, &is_free));
    if (!is_free)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS,
                     "Error allocating prefix value %d for handler %d prefix is already occupied\r\n",
                     prefix_value, handler_id);
    }

    for (comb = 0; comb < nof_combs; comb++)
    {
        extended_prefix_value = (prefix_value << nof_comp_bits) | comb;

        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.core_prefix_map.
                        prefix_handler_map.set(unit, bank_id, core_id, extended_prefix_value, handler_id));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_prefix_val_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id)
{
    uint32 prefix;
    uint32 handler;
    uint8 prefix_freed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);

    for (prefix = 0; prefix < SAL_BIT(dnx_data_field.tcam.max_prefix_size_get(unit)); prefix++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.core_prefix_map.
                        prefix_handler_map.get(unit, bank_id, core_id, prefix, &handler));
        if (handler == handler_id)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.core_prefix_map.
                            prefix_handler_map.set(unit, bank_id, core_id, prefix, DNX_FIELD_TCAM_HANDLER_ID_INVALID));
            prefix_freed = TRUE;
        }
    }
    if (prefix_freed)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "Freed prefix for handler %d on bank %d on core %d%s\n", handler_id, bank_id,
                     core_id, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_tcam_prefix_any_free(
    int unit,
    int bank_id,
    bcm_core_t core_id,
    uint32 prefix_size,
    uint8 *is_free)
{
    int prefix;

    SHR_FUNC_INIT_VARS(unit);

    *is_free = FALSE;

    for (prefix = 0; prefix < SAL_BIT(prefix_size); prefix++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_id, core_id, prefix_size, prefix, is_free));
        if (*is_free)
        {

            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_prefix_set(
    int unit,
    uint32 handler_id,
    int banks_count,
    int bank_ids[],
    bcm_core_t core_ids[])
{
    int bank_i;
    uint32 prefix_size;
    uint32 prefix;
    uint8 is_free = TRUE;
    uint32 key_size;
    bcm_core_t core_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.get(unit, handler_id, &prefix_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    for (prefix = 0; prefix < SAL_BIT(prefix_size); prefix++)
    {
        for (bank_i = 0; bank_i < banks_count; bank_i++)
        {
            DNXCMN_CORES_ITER(unit, core_ids[bank_i], core_iter)
            {
                SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check
                                (unit, bank_ids[bank_i], core_iter, prefix_size, prefix, &is_free));
                if (is_free && key_size == dnx_data_field.tcam.key_size_double_get(unit))
                {

                    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check
                                    (unit, bank_ids[bank_i] + 1, core_iter, prefix_size, prefix, &is_free));
                }
                if (!is_free)
                {

                    break;
                }
            }
            if (!is_free)
            {

                break;
            }
        }
        if (is_free)
        {
            break;
        }
    }

    if (prefix == SAL_BIT(prefix_size))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No suitable prefix was found for handler %d on requested banks.\r\n",
                     handler_id);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Prefix %d (size: %d) was chosen for handler %d%s\n", prefix, prefix_size, handler_id, EMPTY);

    for (bank_i = 0; bank_i < banks_count; bank_i++)
    {
        DNXCMN_CORES_ITER(unit, core_ids[bank_i], core_iter)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_prefix_per_core.set(unit, handler_id, core_iter, prefix));

            SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_prefix_write
                            (unit, handler_id, core_iter,
                             prefix << (dnx_data_field.tcam.max_prefix_size_get(unit) - prefix_size)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_prefix_bank_alloc(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id)
{
    uint32 prefix_size;
    uint32 prefix_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_prefix_per_core.get(unit, handler_id, core_id, &prefix_value));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.get(unit, handler_id, &prefix_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_val_alloc(unit, bank_id, core_id, handler_id, prefix_size, prefix_value));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Allocated prefix %d (size: %d) for handler %d on bank %d\n", prefix_value, prefix_size, handler_id,
                 bank_id);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_prefix_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_val_free(unit, bank_id, handler_id, core_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_prefix_is_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    bcm_core_t core_id,
    uint8 *is_free)
{
    uint32 prefix_size;
    uint32 prefix_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.get(unit, handler_id, &prefix_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_prefix_per_core.get(unit, handler_id, core_id, &prefix_value));

    if (prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_any_free(unit, bank_id, core_id, prefix_size, is_free));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_id, core_id, prefix_size, prefix_value, is_free));
    }

exit:
    SHR_FUNC_EXIT;
}
