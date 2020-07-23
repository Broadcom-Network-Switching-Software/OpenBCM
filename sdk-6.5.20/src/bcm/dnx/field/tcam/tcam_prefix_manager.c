/**
 * \file       tcam_prefix_manager.c
 *
 * This file implements all the TCAM prefix manager functions.
 */
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <include/bcm_int/dnx/field/tcam/tcam_prefix_manager.h>
#include <include/bcm_int/dnx/field/tcam/tcam_access_profile_manager.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_prefix_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_tcam_manager_types.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

/**
 * \brief
 *  Checks whether the given prefix_value that size is equal to the given prefix_size
 *  is free to be used on the given bank_id
 *
 * \param [in] unit         - Device ID
 * \param [in] bank_id      - TCAM Bank ID to check if the given prefix_value is free to be used on
 * \param [in] prefix_size  - The size of the given prefix_value
 * \param [in] prefix_value - The prefix value to check whether it's free to be used
 * \param [out] is_free     - TRUE if the given prefix_value is free on the given bank_id
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_prefix_free_check(
    int unit,
    int bank_id,
    uint32 prefix_size,
    uint32 prefix_value,
    uint8 *is_free)
{
    /*
     * Number of complementary bits 
     */
    int nof_comp_bits = dnx_data_field.tcam.max_prefix_size_get(unit) - prefix_size;
    /*
     * Number of combinations available for the given prefix_size 
     */
    int nof_combs = SAL_BIT(nof_comp_bits);
    int comb;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We first assume that the prefix_value is free to be used 
     */
    *is_free = TRUE;

    /*
     * Run on all combintations (up to max_prefix_size) for the given prefix_value per the given prefix_size
     * For instance, if the prefix_size is 3, and the prefix_value is 0b101, and max_prefix_size
     * is 5, then all the available combinations are all the number with 0b101XX strucutre
     * Therefore, for the giving example, we need to run on all the number:
     * *--------*----------*
     * | Comb#  |   Comb   |
     * +--------+----------+
     * |   0    | 0b101-00 |
     * |   1    | 0b101-01 |
     * |   2    | 0b101-10 |
     * |   3    | 0b101-11 |
     * *--------*----------*
     *
     * For each of these numbers we need to validate that it's not in use, because if we
     * want to declare the given value, for example 0b101, as a free prefix, we need to make
     * sure that all the combinations of that prefix are not in use, otherwise, we might have
     * contention and ambiguity in deciding for which group belongs certain entries.
 */
    for (comb = 0; comb < nof_combs; comb++)
    {
        /*
         * Check to see if the current combination is in use by other handler 
         */
        uint32 handler;
        int prefix_to_check = (prefix_value << nof_comp_bits) | comb;
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.prefix_handler_map.get(unit, bank_id, prefix_to_check,
                                                                                     &handler));
        if (handler != DNX_FIELD_TCAM_HANDLER_ID_INVALID)
        {
            *is_free = FALSE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allocates the given prefix_value that its size is equal to the given prefix_size for the
 *  given handler_id on the given bank_id.
 *
 * \param [in] unit - Device ID
 * \param [in] bank_id - TCAM Bank ID to allocate prefix on
 * \param [in] handler_id - TCAM Handler ID to allocate prefix for
 * \param [in] prefix_size - Size of prefix to allocate
 * \param [in] prefix_value - Prefix to allocate
 *
 * \return
 *  \retval _SHR_E_NONE - success
 *  \retval _SHR_E_EXISTS - Prefix or one of its combinations is already allocated.
 */
static shr_error_e
dnx_field_tcam_prefix_val_alloc(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint32 prefix_size,
    uint32 prefix_value)
{
    /*
     * Number of complementary bits 
     */
    int nof_comp_bits = dnx_data_field.tcam.max_prefix_size_get(unit) - prefix_size;
    /*
     * Number of combinations available for the given prefix_size 
     */
    int nof_combs = SAL_BIT(nof_comp_bits);
    int comb;
    int extended_prefix_value;
    uint8 is_free;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_id, prefix_size, prefix_value, &is_free));
    if (!is_free)
    {
        SHR_ERR_EXIT(_SHR_E_EXISTS,
                     "Error allocating prefix value %d for handler %d prefix is already occupied\r\n",
                     prefix_value, handler_id);
    }

    /*
     * Occupy all combinations of the given prefix_value.
     * (See dnx_field_tcam_prefix_free_check for further information about combinations)
     */
    for (comb = 0; comb < nof_combs; comb++)
    {
        extended_prefix_value = (prefix_value << nof_comp_bits) | comb;
        /*
         * Update handler->prefix map with the new handler/prefix
         */
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.
                        prefix_handler_map.set(unit, bank_id, extended_prefix_value, handler_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Frees the prefix for the given handler_id on the given bank_id.
 *
 * \param [in] unit - Device ID
 * \param [in] bank_id - TCAM Bank ID to free prefix on
 * \param [in] handler_id - TCAM Handler ID to free prefix for
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
dnx_field_tcam_prefix_val_free(
    int unit,
    int bank_id,
    uint32 handler_id)
{
    uint32 prefix;
    uint32 handler;
    uint8 prefix_freed = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Run on all prefixes and free the ones that belong to the given handler_id 
     */
    for (prefix = 0; prefix < SAL_BIT(dnx_data_field.tcam.max_prefix_size_get(unit)); prefix++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.prefix_handler_map.get(unit, bank_id, prefix, &handler));
        if (handler == handler_id)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_sw.banks_prefix.
                            prefix_handler_map.set(unit, bank_id, prefix, DNX_FIELD_TCAM_HANDLER_ID_INVALID));
            prefix_freed = TRUE;
        }
    }
    if (!prefix_freed)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No prefix for handler %d was found on bank %d", handler_id, bank_id);
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE, "Freed prefix for handler %d on bank %d%s%s\n", handler_id, bank_id, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Checks whether there are any free prefixes that their size is equal to the given prefix_size on the given bank_id
 * \param [in] unit        - Device ID
 * \param [in] bank_id     - TCAM bank ID to check whether any free prefixes exists on
 * \param [in] prefix_size - The size of the prefixes to check availability for
 * \param [out] is_free    - TRUE if there is any free prefix that their size is equal to the given prefix_size on the given bank
 */
static shr_error_e
dnx_field_tcam_prefix_any_free(
    int unit,
    int bank_id,
    uint32 prefix_size,
    uint8 *is_free)
{
    int prefix;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * First, we assume there are no free prefixes available 
     */
    *is_free = FALSE;
    /*
     * Run on all prefixes of the given size 
     */
    for (prefix = 0; prefix < SAL_BIT(prefix_size); prefix++)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_id, prefix_size, prefix, is_free));
        if (*is_free)
        {
            /*
             * Free prefix was found 
             */
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
    int bank_ids[])
{
    int bank_i;
    uint32 prefix_size;
    uint32 prefix;
    uint8 is_free = TRUE;
    uint32 key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.get(unit, handler_id, &prefix_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.key_size.get(unit, handler_id, &key_size));

    /*
     * Run on all prefixes of the given size, and find one that is free on all given banks.
     */
    for (prefix = 0; prefix < SAL_BIT(prefix_size); prefix++)
    {
        for (bank_i = 0; bank_i < banks_count; bank_i++)
        {
            SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_ids[bank_i], prefix_size, prefix, &is_free));
            if (is_free && key_size == dnx_data_field.tcam.key_size_double_get(unit))
            {
                /*
                 * For double key DBs, check the next bank in turn 
                 */
                SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check
                                (unit, bank_ids[bank_i] + 1, prefix_size, prefix, &is_free));
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

    /*
     * Set the prefix for the given handler 
     */
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_value.set(unit, handler_id, prefix));

    /*
     * Write final prefix value to the access profile entry of the given handler
     */
    prefix <<= (dnx_data_field.tcam.max_prefix_size_get(unit) - prefix_size);
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_profile_prefix_write(unit, handler_id, prefix));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_prefix_bank_alloc(
    int unit,
    int bank_id,
    uint32 handler_id)
{
    uint32 prefix_size;
    uint32 prefix_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_value.get(unit, handler_id, &prefix_value));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.get(unit, handler_id, &prefix_size));

    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_val_alloc(unit, bank_id, handler_id, prefix_size, prefix_value));

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
    uint32 handler_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_val_free(unit, bank_id, handler_id));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_tcam_prefix_is_free(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *is_free)
{
    uint32 prefix_size;
    uint32 prefix_value;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_size.get(unit, handler_id, &prefix_size));
    SHR_IF_ERR_EXIT(dnx_field_tcam_access_sw.fg_params.prefix_value.get(unit, handler_id, &prefix_value));

    /*
     * If prefix_value requested is Auto, then we check if any prefix that has the requested
     * size is free to be used on the bank.
     * If prefix_value requested is not auto, or prefix has already been allocated for the handler,
     * we check if the prefix we have is free to be used on the bank.
     */
    if (prefix_value == DNX_FIELD_TCAM_PREFIX_VAL_AUTO)
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_any_free(unit, bank_id, prefix_size, is_free));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_field_tcam_prefix_free_check(unit, bank_id, prefix_size, prefix_value, is_free));
    }

exit:
    SHR_FUNC_EXIT;
}
