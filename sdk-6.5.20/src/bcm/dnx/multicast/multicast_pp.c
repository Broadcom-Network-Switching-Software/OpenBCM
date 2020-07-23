/** \file src/bcm/dnx/multicast/multicast_pp.c
 * 
 *
 * PP MULTICAST functionality for DNX.
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <bcm_int/dnx/multicast/multicast.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/algo/ppmc/algo_ppmc.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ppmc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include "multicast_imp.h"
#include "multicast_pp.h"
/*
 * }
 */

/*
 * Defines
 * {
 */
/** Define the list of supported flags */
#define MULTICAST_PP_SUPPORTED_FLAGS (BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID | BCM_MULTICAST_ENCAP_EXTENSION_REPLACE)
/*
 * }
 */

/**
 * \brief
 *  Verify the input to the bcm_dnx_multicast_encap_extension_create API.
 * \param [in] unit - The unit number.
 * \param [in] flags - BCM_MULTICAST_ENCAP_EXTENSION_* flags
 * \param [in] multicast_replication_index - an encapsulation identification that
 *             used as a key for the extension information.
 * \param [in] encap_extension_count - the number of encapsulation
 *             extension to place in the multicast_replication_index entry.
 * \param [in] encap_extension_array - the encapsulation ids
 *             array which have encap_extension_count members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * bcm_dnx_multicast_encap_extension_create
 */
static shr_error_e
dnx_multicast_encap_extension_create_verify(
    int unit,
    uint32 flags,
    bcm_if_t multicast_replication_index,
    int encap_extension_count,
    bcm_if_t * encap_extension_array)
{
    int lif_idx;
    uint8 is_alloc = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_extension_array, _SHR_E_PARAM, "encap_extension_array");

    /** Verify that only supported flags have been provided to the API */
    if (_SHR_IS_FLAG_SET(flags, ~MULTICAST_PP_SUPPORTED_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "One or more of the used flags 0x%x aren't supported (supported flags are 0x%x).",
                     flags, MULTICAST_PP_SUPPORTED_FLAGS);
    }

    /** Verify that REPLACE flag is not used without WITH_ID flag. */
    if (_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_REPLACE)
        && !_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "If REPLACE flag is provided WITH_ID must be set as well.\n");
    }
    /*
     * Verify that the multicast_replication_index is not an ACE pointer -
     * it should be a PPMC MC replication index.
     */
    if (!DNX_ALGO_PPMC_IS_MC_REPLICATION_ID(unit, multicast_replication_index)
        && _SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation ID %d must be between %d and %d to be a valid MC replication ID.\n",
                     multicast_replication_index, dnx_data_ppmc.param.min_mc_replication_id_get(unit),
                     dnx_data_ppmc.param.max_mc_replication_id_get(unit));
    }

    /** If the user provided an ID for the entry, additional verification for existence needs to be carried out. */
    if (_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID))
    {
        SHR_IF_ERR_EXIT(dnx_algo_ppmc_mc_replication_id_is_allocated(unit, multicast_replication_index, &is_alloc));

        /*
         * If we are updating the entry, verify that it is already allocated.
         * If we are creating the entry, verify that it isn't allocated, yet.
         */
        if (_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_REPLACE) && is_alloc == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "PPMC entry with key %d is not found in allocation manager.",
                         multicast_replication_index);
        }
        else if (!_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_REPLACE) && is_alloc == TRUE)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "PPMC entry with key %d already exists in allocation manager.",
                         multicast_replication_index);
        }
    }

    /*
     * Verify that the number of extensions in the array is valid. Only 2 and 3 are supported.
     * Note that 1 actually is supported, we simply block it because it is regarded as an uninteresting scenario.
     */
    if (encap_extension_count < 2 || encap_extension_count > DNX_MULTICAST_PPMC_MAX_OUTLIFS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Incorrect number of result outLIFs %d specified. Only 2 to %d are supported.\n",
                     encap_extension_count, DNX_MULTICAST_PPMC_MAX_OUTLIFS);
    }
    /*
     * Iterate over all encapsulation extensions in the array and make sure that their values are valid.
     */
    for (lif_idx = 0; lif_idx < encap_extension_count; lif_idx++)
    {
        /** Verify that member of the array is not 0 */
        if (encap_extension_array[lif_idx] == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation outlif cannot be 0, this is an invalid value.\n");
        }
        /** Verify that the member of the array is a LIF or a RIF (in case lif_idx = 0)*/
        if (lif_idx == 0)
        {
            /*
             * For lif 0 both LIF and RIF types are allowed.
             */
            if (!BCM_L3_ITF_TYPE_IS_LIF(encap_extension_array[lif_idx])
                && !BCM_L3_ITF_TYPE_IS_RIF(encap_extension_array[lif_idx]))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Encapsulation outlif value is not a LIF or a RIF - incorrect encoding of encap_extension_array[%d] value\n",
                             lif_idx);
            }
            else if (BCM_L3_ITF_TYPE_IS_RIF(encap_extension_array[lif_idx]))
            {
                if (encap_extension_array[lif_idx] >= dnx_data_l3.rif.nof_rifs_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "First encapsulation element value %d is a RIF but is not in range - should be below %d\n",
                                 encap_extension_array[lif_idx], dnx_data_l3.rif.nof_rifs_get(unit));
                }
            }
        }
        else if (!BCM_L3_ITF_TYPE_IS_LIF(encap_extension_array[lif_idx]))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Encapsulation outlif value is not a LIF - incorrect encoding of encap_extension_array[%d] value\n",
                         lif_idx);
        }
        /** Verify that the member of the array is a valid out-LIF*/
        if (BCM_L3_ITF_VAL_GET(encap_extension_array[lif_idx]) >= dnx_data_lif.global_lif.nof_global_out_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation outlif value %d must be smaller than %d.\n",
                         BCM_L3_ITF_VAL_GET(encap_extension_array[lif_idx]),
                         dnx_data_lif.global_lif.nof_global_out_lifs_get(unit));
        }

        /** Verify that the outlif is not 0. */
        if (BCM_L3_ITF_VAL_GET(encap_extension_array[lif_idx]) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation outlif value cannot be equal to 0.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Allows the MC replication to use more than a single encapsulation.
 *  Adds an entry to the PPMC table that would map the members of the
 *  encap_extension_array to the packet CUD (multicast_replication_index).
 * \param [in] unit - The unit number.
 * \param [in] flags - BCM_MULTICAST_ENCAP_EXTENSION_* flags:
 *      * BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID
 *      * BCM_MULTICAST_ENCAP_EXTENSION_REPLACE
 * \param [in,out] multicast_replication_index - an encapsulation identification that
 *             used as a key for the extension information.
 * \param [in] encap_extension_count - the number of encapsulation
 *             extension to place in the multicast_replication_index entry.
 * \param [in] encap_extension_array - the encapsulation ids
 *             array which have encap_extension_count members.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION
 */
shr_error_e
bcm_dnx_multicast_encap_extension_create(
    int unit,
    uint32 flags,
    bcm_if_t * multicast_replication_index,
    int encap_extension_count,
    bcm_if_t * encap_extension_array)
{
    
    dnx_field_action_t dnx_actions[DNX_MULTICAST_PPMC_MAX_OUTLIFS] =
        { DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID),
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_ID),
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_3RD_OUTLIF_ACTION_ID)
    };
    uint8 idx;
    uint32 ace_flags = 0;
    dnx_field_ace_key_t ace_key;
    dnx_field_ace_id_t ace_id[DNX_MULTICAST_PPMC_MAX_OUTLIFS] = {
        DNX_MULTICAST_PPMC_SINGLE_OUTLIF_ACE_ID,
        DNX_MULTICAST_PPMC_DOUBLE_OUTLIF_ACE_ID,
        DNX_MULTICAST_PPMC_TRIPLE_OUTLIF_ACE_ID
    };
    dnx_field_entry_payload_t payload;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_encap_extension_create_verify
                          (unit, flags, *multicast_replication_index, encap_extension_count, encap_extension_array));
    SHR_IF_ERR_EXIT(dnx_field_entry_payload_t_init(unit, &payload));

    /** If REPLACE flag is not provided, then the entry doesn't exist yet and it needs to be allocated. */
    if (!_SHR_IS_FLAG_SET(flags, BCM_MULTICAST_ENCAP_EXTENSION_REPLACE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_ppmc_mc_replication_id_alloc(unit, flags, multicast_replication_index));
    }
    else
    {
        ace_flags |= DNX_FIELD_ENTRY_ADD_FLAG_UPDATE;
    }
    for (idx = 0; idx < encap_extension_count; idx++)
    {
        payload.action_info[idx].dnx_action = dnx_actions[idx];
        payload.action_info[idx].action_value[0] = BCM_L3_ITF_VAL_GET(encap_extension_array[idx]);
    }
    payload.action_info[idx].dnx_action = DNX_FIELD_ACTION_INVALID;

    /*
     * dnx_field_ace_entry_add() for PPMC always uses the WITH_ID since the allocation is performed 
     * by dnx_algo_ppmc_mc_replication_id_alloc.
     */
    ace_flags |= DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID;

    /**Add the entry*/
    ace_key = (*multicast_replication_index);
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_add(unit, ace_flags, ace_id[encap_extension_count - 1], &payload, &ace_key));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Receive the list of outLIFs (MC replications) that are mapped to the packet
 *  CUD (multicast_replication_index) value.
 * \param [in] unit - The unit number.
 * \param [in] flags - BCM_MULTICAST_ENCAP_EXTENSION_* flags
 * \param [in] multicast_replication_index - an encapsulation identification that
 *             used as a key for the extension information.
 * \param [in] encap_max - the size of the provided encap_extension_array
 * \param [out] encap_extension_array - an array of the found
 *              encapsulations IDs
 * \param [out] encap_extension_count - the number of encap IDs that were found
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION
 */
shr_error_e
bcm_dnx_multicast_encap_extension_get(
    int unit,
    uint32 flags,
    bcm_if_t multicast_replication_index,
    int encap_max,
    bcm_if_t * encap_extension_array,
    int *encap_extension_count)
{
    dnx_field_entry_payload_t payload;
    int idx;
    int count = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verify that the multicast_replication_index is in the correct range of IDs.
     */
    if (!DNX_ALGO_PPMC_IS_MC_REPLICATION_ID(unit, multicast_replication_index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Encapsulation ID %d must be between %d and %d to be a valid MC replication ID.\n",
                     multicast_replication_index, dnx_data_ppmc.param.min_mc_replication_id_get(unit),
                     dnx_data_ppmc.param.max_mc_replication_id_get(unit));
    }

    *encap_extension_count = 0;
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_get(unit, multicast_replication_index, &payload, NULL));

    /*
     * Count the number of outlifs used.
     */
    for (idx = 0; idx < DNX_MULTICAST_PPMC_MAX_OUTLIFS; idx++)
    {
        if (payload.action_info[idx].dnx_action == DNX_FIELD_ACTION_INVALID)
        {
            break;
        }
        if (encap_max == idx && payload.action_info[idx].action_value[0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Found more outlifs than encap_max %d allows to return\n", encap_max);
            break;
        }
        count++;
    }
    *encap_extension_count = count;

    /*
     * Read the outlifs from MSB to LSB of the payload.
     */
    for (idx = count - 1; idx >= 0; idx--)
    {
        BCM_L3_ITF_SET(encap_extension_array[count - idx - 1], BCM_L3_ITF_TYPE_LIF,
                       payload.action_info[idx].action_value[0]);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Remove the mapping of packet CUD (multicast_replication_index) to a list of outLIFs (MC replications).
 * \param [in] unit - The unit number.
 * \param [in] flags - BCM_MULTICAST_ENCAP_EXTENSION_* flags
 * \param [in] multicast_replication_index - an encapsulation identification that
 *             used as a key for the extension information.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * ETM_PP_DESCRIPTOR_EXPANSION
 */
shr_error_e
bcm_dnx_multicast_encap_extension_destroy(
    int unit,
    uint32 flags,
    bcm_if_t multicast_replication_index)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Verify that the multicast_replication_index is in the correct range of IDs.
     */
    if (!DNX_ALGO_PPMC_IS_MC_REPLICATION_ID(unit, multicast_replication_index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Encapsulation ID %d must be between %d and %d to be a valid MC replication ID.\n",
                     multicast_replication_index, dnx_data_ppmc.param.min_mc_replication_id_get(unit),
                     dnx_data_ppmc.param.max_mc_replication_id_get(unit));
    }

    SHR_IF_ERR_EXIT(dnx_field_ace_entry_delete(unit, multicast_replication_index));
    SHR_IF_ERR_EXIT(dnx_algo_ppmc_mc_replication_id_free(unit, multicast_replication_index));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Remove all mappings of packet CUD (multicast_replication_index) to lists of outLIFs (MC replications).
 * \param [in] unit - The unit number.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * ETM_PP_DESCRIPTOR_EXPANSION
 */
int
bcm_dnx_multicast_encap_extension_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** Clear DBAL table */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION));
    /** Clear SW state */
    SHR_IF_ERR_EXIT(dnx_algo_ppmc_clear_all(unit));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Traverse over all entries in the PPMC table.
 *  Execute a callback function for each one.
 * \param [in] unit - The unit number.
 * \param [in] trav_fn - the name of the function to be executed for each entry.
 * \param [in] user_data - user data passed to the CB function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * ETM_PP_DESCRIPTOR_EXPANSION
 */
int
bcm_dnx_multicast_encap_extension_traverse(
    int unit,
    bcm_multicast_encap_extension_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 ppmc_rep_id;
    uint32 flags = 0;
    int is_end;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PP_CUD, &ppmc_rep_id));
        SHR_IF_ERR_EXIT((*trav_fn) (unit, ppmc_rep_id, flags, user_data));
        /** Receive next entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_multicast_pp_jr1_smae_interface_init(
    int unit)
{
    dnx_field_entry_payload_t payload;
    dnx_field_ace_key_t ace_key = DNX_MULTICAST_INVALID_CUD;
    SHR_FUNC_INIT_VARS(unit);

    payload.action_info[0].dnx_action =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID);
    payload.action_info[0].action_value[0] = 0;
    payload.action_info[1].dnx_action = DNX_FIELD_ACTION_INVALID;

    SHR_IF_ERR_EXIT(dnx_field_ace_entry_add
                    (unit, DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID, DNX_MULTICAST_PPMC_SINGLE_OUTLIF_ACE_ID, &payload,
                     &ace_key));

exit:
    SHR_FUNC_EXIT;
}

/** Refer to .h file */
shr_error_e
dnx_multicast_pp_init(
    int unit)
{
    dnx_field_action_flags_e action_flags;
    bcm_field_action_t bcm_action;
    dnx_field_ace_format_add_flags_e ace_format_flags;
    dnx_field_action_in_info_t action_info;
    dnx_field_ace_format_info_t ace_format_info;
    dnx_field_ace_id_t ace_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify the maximum number of outlif results.
     */
    if (DNX_MULTICAST_PPMC_MAX_OUTLIFS > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Number of result outlifs in PPMC (%d) cannot exceed %d.\r\n",
                     DNX_MULTICAST_PPMC_MAX_OUTLIFS, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    }

    /*
     * Create three user defined actions for OUTLIFs with a prefix to mark the outlif as valid (23rd bit).
     * The reason we use action prefix, is to reduce the amount of bits in the payload and allow two outlifs 
     * to be written within a 60 bit entry.
     * Note we initialize the name string to contain only NULLs after the string is NUL terminated.
     */
    SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &action_info));
    action_info.stage = DNX_FIELD_STAGE_ACE;
    action_info.size = 22; 
    action_info.prefix = 0x1;
    action_info.prefix_size = 1;
    action_flags = DNX_FIELD_ACTION_FLAG_WITH_ID;
    /*
     * 1st action
     */
    action_info.bcm_action = bcmFieldActionOutVport0Raw;
    sal_strncpy(action_info.name, DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_NAME, sizeof(action_info.name));
    bcm_action = BCM_FIELD_ACTION_ID_MAP(DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID);
    SHR_IF_ERR_EXIT(dnx_field_action_create(unit, action_flags, &action_info, &bcm_action, NULL));
    /*
     * 2nd action
     */
    action_info.bcm_action = bcmFieldActionOutVport1Raw;
    sal_strncpy(action_info.name, DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_NAME, sizeof(action_info.name));
    bcm_action = BCM_FIELD_ACTION_ID_MAP(DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_ID);
    SHR_IF_ERR_EXIT(dnx_field_action_create(unit, action_flags, &action_info, &bcm_action, NULL));
    /*
     * 3rd action
     */
    action_info.bcm_action = bcmFieldActionOutVport2Raw;
    sal_strncpy(action_info.name, DNX_MULTICAST_PPMC_3RD_OUTLIF_ACTION_NAME, sizeof(action_info.name));
    bcm_action = BCM_FIELD_ACTION_ID_MAP(DNX_MULTICAST_PPMC_3RD_OUTLIF_ACTION_ID);
    SHR_IF_ERR_EXIT(dnx_field_action_create(unit, action_flags, &action_info, &bcm_action, NULL));

    
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_t_init(unit, &ace_format_info));
    ace_format_flags = DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID;
    ace_format_info.use_valid_bit[0] = FALSE;
    ace_format_info.use_valid_bit[1] = FALSE;
    ace_format_info.use_valid_bit[2] = FALSE;
    /*
     * One outlif ACE format.
     */
    ace_format_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID);
    ace_format_info.dnx_actions[1] = DNX_FIELD_ACTION_INVALID;
    ace_id = DNX_MULTICAST_PPMC_SINGLE_OUTLIF_ACE_ID;
    sal_strncpy(ace_format_info.name, DNX_MULTICAST_PPMC_SINGLE_OUTLIF_ACE_FORMAT_NAME, sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, ace_format_flags, &ace_format_info, &ace_id));
    /*
     * Two outlifs ACE format.
     */
    ace_format_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_ID);
    ace_format_info.dnx_actions[1] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID);
    ace_format_info.dnx_actions[2] = DNX_FIELD_ACTION_INVALID;
    ace_id = DNX_MULTICAST_PPMC_DOUBLE_OUTLIF_ACE_ID;
    sal_strncpy(ace_format_info.name, DNX_MULTICAST_PPMC_DOUBLE_OUTLIF_ACE_FORMAT_NAME, sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, ace_format_flags, &ace_format_info, &ace_id));
    /*
     * Three outlifs ACE format.
     */
    ace_format_info.dnx_actions[0] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_3RD_OUTLIF_ACTION_ID);
    ace_format_info.dnx_actions[1] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_2ND_OUTLIF_ACTION_ID);
    ace_format_info.dnx_actions[2] =
        DNX_ACTION(DNX_FIELD_ACTION_CLASS_USER, DNX_FIELD_STAGE_ACE, DNX_MULTICAST_PPMC_1ST_OUTLIF_ACTION_ID);
    ace_format_info.dnx_actions[3] = DNX_FIELD_ACTION_INVALID;
    ace_id = DNX_MULTICAST_PPMC_TRIPLE_OUTLIF_ACE_ID;
    sal_strncpy(ace_format_info.name, DNX_MULTICAST_PPMC_TRIPLE_OUTLIF_ACE_FORMAT_NAME, sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, ace_format_flags, &ace_format_info, &ace_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Verify function for BCM-API: bcm_multicast_l3_encap_get().
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] intf -RIF L3 interface id
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * BCM_L3_ITF_TYPE_IS_RIF
 */
static shr_error_e
dnx_multicast_l3_encap_get_verify(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_if_t intf,
    bcm_if_t * encap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify L3 interface type 
     */
    if (!BCM_L3_ITF_TYPE_IS_RIF(intf))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_multicast_l3_encap_get is only used for RIF type L3 interface.\n");
    }

    /*
     * Verify RIF L3 interface range 
     */
    if (BCM_L3_ITF_VAL_GET(intf) >= dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "RIF type L3 interface ID %d must be smaller than or equal to the maximum RIF %d.\n",
                     BCM_L3_ITF_VAL_GET(intf), dnx_data_l3.rif.nof_rifs_get(unit));
    }

    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Get the encap information from RIF type L3 interface.
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] intf -RIF L3 interface id
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 *  * BCM_L3_ITF_TYPE_IS_RIF
 */
int
bcm_dnx_multicast_l3_encap_get(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_if_t intf,
    bcm_if_t * encap_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_l3_encap_get_verify(unit, group, port, intf, encap_id));

    /*
     * Encap id is simply RIF intf id
     */
    *encap_id = intf;

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Verify function for BCM-API: bcm_multicast_vlan_encap_get().
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] vlan_port_id -vlan port
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_multicast_vlan_encap_get_verify(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_gport_t vlan_port_id,
    bcm_if_t * encap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify vlan_port_id is vlan port 
     */
    if (!BCM_GPORT_IS_VLAN_PORT(vlan_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given gport (%d) is not vlan port.\n", vlan_port_id);
    }

    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Get the encap information from vlan port.
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] vlan_port_id -vlan port
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_multicast_vlan_encap_get(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_gport_t vlan_port_id,
    bcm_if_t * encap_id)
{
    uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_vlan_encap_get_verify(unit, group, port, vlan_port_id, encap_id));

    /*
     * Get global out lif 
     */
    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, vlan_port_id, lif_flags, &gport_hw_resources));

    /*
     * The same as mpls API, use global out lif without BCM_L3_ITF_SET 
     */
    *encap_id = gport_hw_resources.global_out_lif;

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Verify function for BCM-API: bcm_multicast_vpls_encap_get().
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] mpls_port_id - mpls port
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_multicast_vpls_encap_get_verify(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_gport_t mpls_port_id,
    bcm_if_t * encap_id)
{
    int gport_internal_id = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify mpls_port_id is mpls port 
     */
    if (!BCM_GPORT_IS_MPLS_PORT(mpls_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given gport (%d) is not mpls gport.\n", mpls_port_id);
    }

    /** MPLS PORT - store internal id, check SUB_TYPE later on */
    gport_internal_id = BCM_GPORT_MPLS_PORT_ID_GET(mpls_port_id);
    if (BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_internal_id) != BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given mpls gport (%d) is expected to use egress only MPLS gport in this API.\n",
                     mpls_port_id);
    }

    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Get the encap information from mpls port.
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] mpls_port_id - mpls port
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_multicast_vpls_encap_get(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_gport_t mpls_port_id,
    bcm_if_t * encap_id)
{
    uint32 lif_flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_vpls_encap_get_verify(unit, group, port, mpls_port_id, encap_id));

    /*
     * Get global out lif 
     */
    sal_memset(&gport_hw_resources, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, mpls_port_id, lif_flags, &gport_hw_resources));

    /*
     * The same as mpls API, use global out lif without BCM_L3_ITF_SET 
     */
    *encap_id = gport_hw_resources.global_out_lif;

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Verify function for BCM-API: bcm_multicast_vxlan_encap_get().
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] vxlan_port_id - vxlan port
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_multicast_vxlan_encap_get_verify(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_gport_t vxlan_port_id,
    bcm_if_t * encap_id)
{
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * verify vxlan_port_id is vxlan port 
     */
    if (!BCM_GPORT_IS_VXLAN_PORT(vxlan_port_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Given gport (%d) is not vxlan port.\n", vxlan_port_id);
    }

    /*
     * verify vxlan port. forwarding dest is outlif 
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, vxlan_port_id, BCM_FORWARD_ENCAP_ID_INVALID, &forward_info));
    if (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "result_type (%d) is not supported for get api \r\n",
                     forward_info.fwd_info_result_type);
    }

    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");

exit:
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Get the encap information from vxlan port.
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] port - N/A parameter
 * \param [in] vxlan_port_id - vxlan port
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_multicast_vxlan_encap_get(
    int unit,
    bcm_multicast_t group,
    bcm_gport_t port,
    bcm_gport_t vxlan_port_id,
    bcm_if_t * encap_id)
{
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_vxlan_encap_get_verify(unit, group, port, vxlan_port_id, encap_id));

    /*
     * Get global out lif ID based on vxlan port. 
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_and_encap_to_forward_information
                    (unit, vxlan_port_id, BCM_FORWARD_ENCAP_ID_INVALID, &forward_info));
    *encap_id = forward_info.outlif;

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Verify function for BCM-API: bcm_multicast_egress_object_encap_get().
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] intf - FEC or Tunnel egress object
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
static shr_error_e
dnx_multicast_egress_object_encap_get_verify(
    int unit,
    bcm_multicast_t group,
    bcm_if_t intf,
    bcm_if_t * encap_id)
{
    uint32 max_fec_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");

    /*
     * Verify L3 interface type 
     */
    if ((!BCM_L3_ITF_TYPE_IS_FEC(intf)) && (!BCM_L3_ITF_TYPE_IS_LIF(intf)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_multicast_egress_object_encap_get are used for FEC or Tunnel intf egress object.\n");
    }

    /*
     * Verify LIF type L3 interface range 
     */
    if (BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, BCM_L3_ITF_VAL_GET(intf));
    }

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
    /*
     * Verify FEC type L3 interface range 
     */
    if ((BCM_L3_ITF_TYPE_IS_FEC(intf)) && (BCM_L3_ITF_VAL_GET(intf) > max_fec_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "FEC type L3 interface ID %d must be smaller than or equal to the maximum FEC %d.\n",
                     BCM_L3_ITF_VAL_GET(intf), max_fec_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *\brief
 *  Get the encap information from egress object.
 * \param [in] unit - The unit number
 * \param [in] group - N/A parameter
 * \param [in] intf - FEC or Tunnel egress object
 * \param [out] encap_id - encap id
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
int
bcm_dnx_multicast_egress_object_encap_get(
    int unit,
    bcm_multicast_t group,
    bcm_if_t intf,
    bcm_if_t * encap_id)
{
    bcm_l3_egress_t egr;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_multicast_egress_object_encap_get_verify(unit, group, intf, encap_id));

    /*
     * Get encap id from FEC or Tunnel intf egress object. For RIF use bcm_multicast_l3_encap_get 
     */
    if (BCM_L3_ITF_TYPE_IS_LIF(intf))
    {
        *encap_id = intf;
    }
    else if (BCM_L3_ITF_TYPE_IS_FEC(intf))
    {
        bcm_l3_egress_t_init(&egr);
        SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, intf, &egr));
        *encap_id = egr.intf;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
