/** \file field_presel.c
 * 
 *
 * Field preselector procedures for DNX.
 *
 * Management for contex selection i.e program selection table and line
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/error.h>
#include <bcm/field.h>
#include <bcm/error.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_port.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_group_access.h>
#include <sal/core/boot.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

/*
 * }Include files
 */

 /*
  * Defines
  * {
  */

/**
 * The value to be written in the CTX_ID_VALID field to make the presel entry change the context ID to
 * what is written in CTX_ID field.
 */
#define DNX_FIELD_PRESEL_CTX_ID_VALID_SET (1)
 /**
  * The value to be written in the CTX_ID_VALID field to make the presel entry leave the context ID unchanged
  * (determined by a previous stage).
  * At this point only supported for external stage.
  */
#define DNX_FIELD_PRESEL_CTX_ID_VALID_RESET (0)

 /*
  * }
  */

/**
* \brief
*  Clear the dnx_field_presel_qual_data_t, set it to preferred init values
* \param [in] unit          - Device ID
* \param [in] qual_data_p   - Pointer to input structure of
*                             dnx_field_presel_qual_data_t that needs to be init
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_field_presel_qual_data_t_init(
    int unit,
    dnx_field_presel_qual_data_t * qual_data_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_data_p, _SHR_E_PARAM, "qual_data_p");

    qual_data_p->qual_type_dbal_field = DBAL_NOF_FIELDS;
    qual_data_p->qual_value = 0;
    qual_data_p->qual_mask = 0;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_presel.h
 */
shr_error_e
dnx_field_presel_entry_id_t_init(
    int unit,
    dnx_field_presel_entry_id_t * entry_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    entry_id_p->presel_id = DNX_FIELD_PRESEL_ID_INVALID;
    entry_id_p->stage = DNX_FIELD_STAGE_NOF;
exit:
    SHR_FUNC_EXIT;
}
/**
 * See field_presel.h
 */
shr_error_e
dnx_field_presel_entry_data_t_init(
    int unit,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    entry_data_p->entry_valid = FALSE;
    entry_data_p->nof_qualifiers = 0;
    entry_data_p->context_id = DNX_FIELD_CONTEXT_ID_INVALID;

    for (qual_idx = 0; qual_idx < DNX_FIELD_CS_QUAL_NOF_MAX; qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_presel_qual_data_t_init(unit, &(entry_data_p->qual_data[qual_idx])));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Write the Context Selection key to the hardware
 * \param [in] unit                 - Device ID
 * \param [in] field_stage          - The stage for which we check the context selection.
 * \param [in] qual_type_dbal_field - The DBAL field of the qualifier.
 * \param [out] is_supported_p      - Whether the DBAL field is supported for context selection on stage and unit.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_presel_cs_qual_supported(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_fields_e qual_type_dbal_field,
    uint8 *is_supported_p)
{
    dbal_tables_e cs_dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_supported_p, _SHR_E_PARAM, "is_supported_p");

    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, field_stage, &cs_dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, cs_dbal_table_id, qual_type_dbal_field, is_supported_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief Write the Context Selection key to the hardware
* \param [in] unit       - Device ID
* \param [in] flags      - Preselection flags
* \param [in] entry_id_p   - Identifier of the preselector (User-provided presel_id and stage)
* \param [in] enable_context - Indicates whether the presel entry should change the context.
* \param [in] entry_data_p - Preselection entry data that will be written into HW (PRESEL-QUALS, PROGRAM_ID, ENTRY_VALID)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_hw_set(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    uint8 enable_context,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    int qual_idx;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Setting Context Selection data to preselector ID %d...\n"), entry_id_p->presel_id));

    /*
     * Get a proper DBAL table according to the given stage
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, entry_id_p->stage, &dbal_table_id));
    /*
     * Allocate DBAL handle to the returned dbal_table_id
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /*
     * Setting the entry access ID this indicates the line of the entry in the HW,
     * in our case it is corresponding to entry_id_p->presel_id.
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_id_p->presel_id));

    /*
     * entry_data_p->entry_valid determines if we create/update an entry or delete it.
     */
    if (entry_data_p->entry_valid)
    {
        /*
         * Setting key fields
         */
        for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
        {
            dbal_entry_key_field_arr32_masked_set(unit,
                                                  entry_handle_id,
                                                  entry_data_p->qual_data[qual_idx].qual_type_dbal_field,
                                                  &(entry_data_p->qual_data[qual_idx].qual_value),
                                                  &(entry_data_p->qual_data[qual_idx].qual_mask));
        }

        /*
         * Setting value fields
         */
        if (entry_id_p->stage != DNX_FIELD_STAGE_EPMF)
        {
            dbal_entry_value_field8_set
                (unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID_VALID, INST_SINGLE, enable_context);
        }
        else
        {
            if (enable_context != DNX_FIELD_PRESEL_CTX_ID_VALID_SET)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "enable_context for ePMF stage is %d. ePMF does not support leaving context "
                             "unchanged in presel.\r\n", enable_context);
            }
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, INST_SINGLE,
                                     entry_data_p->context_id);
        /*
         * Write the presel.
         * Note we use DBAL_COMMIT_FORCE instead of DBAL_COMMIT because we can also overrun an existing presel.
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE | DBAL_COMMIT_OVERRIDE_DEFAULT));
    }
    else
    {
        /*
         * Delete the entry, but first check if it exists.
         */
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_IF_ERR_EXIT(rv);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief Read the Context Selection key from the hardware and
*        take the data for the used presel qualifiers only
* \param [in] unit        - Device ID
* \param [in] flags       - Preselection flags
* \param [in] entry_id_p  - Identifier of the preselector
* \param [out] enable_context_p - Indicates whether the presel entry should change the context.
* \param [out] entry_data_p - Preselection entry data for used presel qualifiers
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_hw_get(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    uint8 *enable_context_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_name;
    /*
     * Used to store the whole qual_data, which is collected from the HW.
     */
    dnx_field_presel_entry_data_t entry_data;
    int qual_idx;
    int qual_idx_all_units;
    dbal_fields_e dbal_field_cs_qual;
    /*
     * Pointer to the array of dnx cs quals sets
     */
    const dbal_fields_e *dnx_cs_qual_info;
    /*
     * Number of presel qualifiers per stage
     */
    int dnx_cs_qual_nof;
    uint8 qual_is_supported;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "Getting Context Selection data for preselector ID %d...\n"), entry_id_p->presel_id));
    /*
     * Get a proper DBAL table according to the given stage
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, entry_id_p->stage, &dbal_table_name));
    /*
     * Allocate DBAL handle to the returned dbal_table_id
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    /*
     * Setting the entry access ID this indicates the line of the entry in the HW
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_id_p->presel_id));

    /*
     * Perform the DBAL read
     * if the entry is found it is valid.
     */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv == _SHR_E_NOT_FOUND)
    {
        entry_data_p->entry_valid = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
        entry_data_p->entry_valid = TRUE;

        /*
         * Iterate all supported DNX qualifiers for stage preselection
         */
        SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data));
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_get(unit, entry_id_p->stage, &dnx_cs_qual_info, &dnx_cs_qual_nof));
        entry_data_p->nof_qualifiers = 0;
        qual_idx = 0;
        for (qual_idx_all_units = 0; qual_idx_all_units < dnx_cs_qual_nof; qual_idx_all_units++)
        {
            dbal_field_cs_qual = dnx_cs_qual_info[qual_idx_all_units];
            /*
             * Not all mapped qualifiers are available for all units.
             */
            SHR_IF_ERR_EXIT(dnx_field_presel_cs_qual_supported
                            (unit, entry_id_p->stage, dbal_field_cs_qual, &qual_is_supported));
            if (qual_is_supported == FALSE)
            {
                continue;
            }

            /*
             * Get a key fields
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit,
                                                                         entry_handle_id,
                                                                         dbal_field_cs_qual,
                                                                         &(entry_data.qual_data[qual_idx].qual_value),
                                                                         &(entry_data.qual_data[qual_idx].qual_mask)));

            /*
             * A non-zero mask indicates that the qualifier is used.
             * Only then we count it and fill the output structure with its data.
             */
            if (entry_data.qual_data[qual_idx].qual_mask)
            {
                entry_data_p->qual_data[entry_data_p->nof_qualifiers].qual_type_dbal_field = dbal_field_cs_qual;
                entry_data_p->qual_data[entry_data_p->nof_qualifiers].qual_value =
                    entry_data.qual_data[qual_idx].qual_value;
                entry_data_p->qual_data[entry_data_p->nof_qualifiers].qual_mask =
                    entry_data.qual_data[qual_idx].qual_mask;
                entry_data_p->nof_qualifiers++;
            }
            qual_idx++;
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, INST_SINGLE, &(entry_data_p->context_id)));
        if (entry_id_p->stage != DNX_FIELD_STAGE_EPMF)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID_VALID, INST_SINGLE, enable_context_p));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verifies input for dnx_field_presel_set function.
*
* \param [in] unit         - Device ID
* \param [in] flags        - preselection flags
* \param [in] entry_id_p   - Identifier of the preselector:
*                          User-provided preselector ID and stage
* \param [in] entry_data_p - Data to be written in hardware:
*                          PMF Context ID and valid, qualifiers
*                          data
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
*/
static shr_error_e
dnx_field_presel_set_verify(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    int qual_idx_2;
    unsigned int start_bit, nof_bits;
    unsigned int max_quals;
    dnx_field_presel_entry_data_t entry_data_internal;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    if (entry_data_p->entry_valid)
    {
        if (entry_id_p->stage != DNX_FIELD_STAGE_EXTERNAL)
        {
            DNX_FIELD_CONTEXT_IS_ALLOCATED(entry_id_p->stage, entry_data_p->context_id, 0);
        }

        /** Check if requested presel_id is already allocated on this stage. */
        SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data_internal));
        SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, 0, entry_id_p, &entry_data_internal));
        if (entry_data_internal.entry_valid)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE, "Presel %d was overwritten! Old context %d, new context %d. %s\r\n",
                         entry_id_p->presel_id, entry_data_internal.context_id, entry_data_p->context_id, EMPTY);
        }

        /** Initialization to appease the compiler. */
        max_quals = 0;

        /*
         * Find the maximum number of qualifiers per stage.
         * Because in iPMF2 the user cannot use program selection profile, the maximum number for it is actually
         * one less then then the number we assign. however, we only test it later so that if we have
         * program selection profile, it will give an error for that and not number of qualifiers.
         */
        max_quals = DNX_FIELD_CS_QUAL_NOF(entry_id_p->stage);
        if (max_quals <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal stage for preselectors: \"%s\"(%d).\r\n",
                         dnx_field_stage_text(unit, entry_id_p->stage), entry_id_p->stage);
        }

        if (entry_data_p->nof_qualifiers > max_quals)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of qualifiers cannot exceed %d for stage \"%s\". "
                         "Received %d qualifiers for context %d.\r\n",
                         max_quals, dnx_field_stage_text(unit, entry_id_p->stage),
                         entry_data_p->nof_qualifiers, entry_data_p->context_id);
        }

        /** Sanity check, can be removed.*/
        if (entry_data_p->nof_qualifiers > DNX_FIELD_CS_QUAL_NOF_MAX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of qualifiers cannot exceed array size of %d. "
                         "Received %d qualifiers for context %d.\r\n",
                         DNX_FIELD_CS_QUAL_NOF_MAX, entry_data_p->nof_qualifiers, entry_data_p->context_id);
        }

        for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
        {
            if (entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Program Selection Profile preselector is for internal use only, qualifier index %d, "
                             "context ID %d.\r\n", qual_idx, entry_data_p->context_id);
            }
            /** Verify that InLIF profile doesn't exceed maximum profile ID according to configuration. */
            if ((entry_id_p->stage == DNX_FIELD_STAGE_IPMF1) &&
                ((entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_IN_LIF_PROFILE_MAP_0) ||
                 (entry_data_p->qual_data[qual_idx].qual_type_dbal_field == DBAL_FIELD_IN_LIF_PROFILE_MAP_1)))
            {
                SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                                (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, &start_bit, &nof_bits));
                if (entry_data_p->qual_data[qual_idx].qual_value >= (1 << nof_bits))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Inlif profile can't exceed the maximum profile %d.\r\n", ((1 << nof_bits) - 1));
                }
            }
            for (qual_idx_2 = 0; qual_idx_2 < qual_idx; qual_idx_2++)
            {
                if (entry_data_p->qual_data[qual_idx].qual_type_dbal_field ==
                    entry_data_p->qual_data[qual_idx_2].qual_type_dbal_field)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Qual type %d appears more than once in the preselector, in %d and %d, "
                                 "for context ID %d.\r\n",
                                 entry_data_p->qual_data[qual_idx].qual_type_dbal_field, qual_idx, qual_idx_2,
                                 entry_data_p->context_id);
                }
            }
        }

        /*
         * Checking for number of CS qualifiers given that program selection profile isn't an eligible qualifier
         * for iPMF2 and context ID isn't eligible for external stage.
         * Note we can remove this verification and fail on the illegal qualifier.
         */
        if (((entry_id_p->stage == DNX_FIELD_STAGE_IPMF2) || (entry_id_p->stage == DNX_FIELD_STAGE_EXTERNAL)) &&
            (entry_data_p->nof_qualifiers > max_quals - 1))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Number of qualifiers for stage %s cannot exceed %d. "
                         "Received %d qaulifiers for context %d.\r\n",
                         dnx_field_stage_text(unit, entry_id_p->stage),
                         max_quals - 1, entry_data_p->nof_qualifiers, entry_data_p->context_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function takes as argument a presel qualifier DBAL type and returns the corresponding FG type and key ID of the FG this qualifier cascades from
*  if the qualifier is a cascading qualifier, otherwise a FG type invalid is returned.
*
* \param [in] unit - Device ID
* \param [in] qual_dbal_type - The presel qualifier DBAL type
* \param [out] fg_type_p     - The FG type that the qualifier DBAL type cascades from
*                              (INVALID if qualifier DBAL type is not cascading)
* \param [out] fg_key_p      - The key ID of the FG that is cascaded by the qualifier DBAL type
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_cascading_fg_type_key_get(
    int unit,
    dbal_fields_e qual_dbal_type,
    dnx_field_group_type_e * fg_type_p,
    dnx_field_key_id_t *fg_key_p)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (qual_dbal_type)
    {
        case DBAL_FIELD_STATE_TABLE_DATA_HW:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_J;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_0_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_A;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_1_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_B;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_2_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_C;
            break;
        case DBAL_FIELD_PMF1_TCAM_ACTION_3_MSB:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            fg_key_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_D;
            break;
        default:
            *fg_type_p = DNX_FIELD_GROUP_TYPE_INVALID;
            fg_key_p->id[0] = DBAL_NOF_ENUM_FIELD_KEY_VALUES;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Increases the cascading reference of each FG referenced by the given cascading presel qualifiers on the given 'cascacded_from_context_id' context.
*  This function loops over all the given presel qualifiers, and for each one that is cascaded it
*  increases by one the cascading reference of the FG referenced by the qualifier.
*
* \param [in] unit                     - Device ID
* \param [in] cascaded_from_context_id - The cascaded from context ID
* \param [in] entry_data_p             - The presel qualifiers data
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_cascading_refs_inc(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    dnx_field_group_t fg_id;
    dnx_field_presel_qual_data_t qual_data;
    dnx_field_group_type_e fg_type;
    dnx_field_key_id_t fg_key;
    uint16 nof_cascading_refs;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
    {
        qual_data = entry_data_p->qual_data[qual_idx];
        SHR_IF_ERR_EXIT(dnx_field_presel_cascading_fg_type_key_get
                        (unit, qual_data.qual_type_dbal_field, &fg_type, &fg_key));
        if (fg_type == DNX_FIELD_GROUP_TYPE_INVALID)
        {
            /*
             * Not a cascading qualifiers, Skip it
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_convert_context_key_to_fg_id
                        (unit, cascaded_from_context_id, fg_type, fg_key, &fg_id));
        
        if (fg_id == DNX_FIELD_GROUP_INVALID)
        {
            /*
             * User using a cascading presel qualifier with no equivalent FG, so we skip ref count
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_id, cascaded_from_context_id, &nof_cascading_refs));

        nof_cascading_refs++;

        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.set(unit, fg_id, cascaded_from_context_id, nof_cascading_refs));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Decreases the cascading reference of each FG referenced by the given cascading presel qualifiers on the given 'cascacded_from_context_id' context.
*  This function loops over all the given presel qualifiers, and for each one that is cascaded it
*  decreases by one the cascading reference of the FG referenced by the qualifier.
*
* \param [in] unit                     - Device ID
* \param [in] cascaded_from_context_id - The cascaded from context ID
* \param [in] entry_data_p             - The presel qualifiers data
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_cascading_refs_dec(
    int unit,
    dnx_field_context_t cascaded_from_context_id,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    int qual_idx;
    dnx_field_group_t fg_id;
    dnx_field_presel_qual_data_t qual_data;
    dnx_field_group_type_e fg_type;
    dnx_field_key_id_t fg_key;
    uint16 nof_cascading_refs;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
    {
        qual_data = entry_data_p->qual_data[qual_idx];
        SHR_IF_ERR_EXIT(dnx_field_presel_cascading_fg_type_key_get
                        (unit, qual_data.qual_type_dbal_field, &fg_type, &fg_key));
        if (fg_type == DNX_FIELD_GROUP_TYPE_INVALID)
        {
            /*
             * Not a cascading qualifiers, Skip it
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_convert_context_key_to_fg_id
                        (unit, cascaded_from_context_id, fg_type, fg_key, &fg_id));
        
        if (fg_id == DNX_FIELD_GROUP_INVALID)
        {
            /*
             * User using a cascading presel qualifier with no equivalent FG, so we skip ref count
             */
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.get(unit, fg_id, cascaded_from_context_id, &nof_cascading_refs));

        if (nof_cascading_refs <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Error decreasing cascading reference count for FG %d on context %d: Reference value is 0 or less\r\n",
                         fg_id, cascaded_from_context_id);
        }
        nof_cascading_refs--;

        SHR_IF_ERR_EXIT(dnx_field_group_sw.fg_info.context_info.
                        nof_cascading_refs.set(unit, fg_id, cascaded_from_context_id, nof_cascading_refs));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Removes all qualifiers with zero masks from the entry data, as a qualifier with zero mask is identical to not
*  having the qualifier in the entry.
* \param [in] unit                 - Device ID
* \param [in] entry_data_p         - The presel qualifiers data
* \param [out] entry_data_pruned_p - The presel qualifiers data without qualifiers with zero masks.
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_presel_prune_zero_masks(
    int unit,
    dnx_field_presel_entry_data_t * entry_data_p,
    dnx_field_presel_entry_data_t * entry_data_pruned_p)
{
    unsigned int qual_idx_src;
    unsigned int qual_idx_dst;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * To make sure no fields are missed, we first copy the entire structure.
     */
    sal_memcpy(entry_data_pruned_p, entry_data_p, sizeof(*entry_data_pruned_p));

    qual_idx_dst = 0;
    for (qual_idx_src = 0; qual_idx_src < entry_data_p->nof_qualifiers; qual_idx_src++)
    {
        if (entry_data_p->qual_data[qual_idx_src].qual_mask != 0)
        {
            sal_memcpy(&(entry_data_pruned_p->qual_data[qual_idx_dst]), &(entry_data_p->qual_data[qual_idx_src]),
                       sizeof(entry_data_pruned_p->qual_data[0]));
            qual_idx_dst++;
        }
    }

    entry_data_pruned_p->nof_qualifiers = qual_idx_dst;

    /*
     * Initialize the rest of the array. Not necessary.
     */
    for (; qual_idx_dst < DNX_FIELD_CS_QUAL_NOF_MAX; qual_idx_dst++)
    {
        SHR_IF_ERR_EXIT(dnx_field_presel_qual_data_t_init(unit, &(entry_data_pruned_p->qual_data[qual_idx_dst])));
    }

exit:
    SHR_FUNC_EXIT;
}

/* See field_presel.h */
shr_error_e
dnx_field_presel_set(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    dnx_field_presel_entry_data_t entry_data_intern_pruned;
    int cascaded_idx;
    dnx_field_presel_entry_data_t prev_entry_data;
    dnx_field_context_t prev_cascaded_from;
    dnx_field_context_t cascaded_from;
    uint8 enable_context;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_presel_set_verify(unit, flags, entry_id_p, entry_data_p));

    /*
     * Note we change entry_data_p to add the cascaded from context for the iPNF2 scenario.
     * We also prune it from the qualifiers that have zero masks.
     * For those two reasons, we use an internal variable.
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_prune_zero_masks(unit, entry_data_p, &entry_data_intern_pruned));

    enable_context = DNX_FIELD_PRESEL_CTX_ID_VALID_SET;

    /*
     * For External stage, the context is actually the apptype, and we need to convert it to ACL context.
     */
    if (entry_id_p->stage == DNX_FIELD_STAGE_EXTERNAL && entry_data_intern_pruned.entry_valid)
    {
        int nof_fwd_contexts;
        dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        bcm_field_AppType_t apptype;
        bcm_field_AppType_t qualifying_static_apptype;
        uint32 base_apptype_profile;
        int fwd_ctx_idx;

        apptype = entry_data_intern_pruned.context_id;

        if (apptype < bcmFieldAppTypeCount)
        {
            /*
             * For predefined apptypes we leave the acl_context to be the same as acl_context.
             * The Apptype to qualify upon is the Apptype itself.
             */
            enable_context = DNX_FIELD_PRESEL_CTX_ID_VALID_RESET;
            qualifying_static_apptype = apptype;
        }
        else
        {
            /*
             * For user defined apptypes we leave the acl_context to be the same as acl_context.
             * The Apptype to qualify upon is the predefined apptype we cascade from.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_fwd_contexts
                            (unit, apptype, &nof_fwd_contexts, fwd_contexts, acl_contexts));
            for (fwd_ctx_idx = 0; fwd_ctx_idx < nof_fwd_contexts; fwd_ctx_idx++)
            {
                if (acl_contexts[0] != acl_contexts[fwd_ctx_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "For user defined Apptype %d "
                                 "there is more than one acl_context (%d and %d at indices %d and %d). "
                                 "Only one acl_context per user defined Apptype is supported.\n",
                                 apptype, acl_contexts[0], acl_contexts[fwd_ctx_idx], 0, fwd_ctx_idx);
                }
            }

            /** Set the context to be the ACL context, not Apptype.*/
            entry_data_intern_pruned.context_id = acl_contexts[0];

            SHR_IF_ERR_EXIT(dnx_field_context_apptype_cascaded_get(unit, apptype, &qualifying_static_apptype));
        }

        /*
         * Add a qualifier for the profile of the predefined Apptype
         * (the cascaded from apptype in the case of user defined Apptypes).
         */
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_profile_get(unit, qualifying_static_apptype, &base_apptype_profile));
        cascaded_idx = entry_data_intern_pruned.nof_qualifiers;
        entry_data_intern_pruned.qual_data[cascaded_idx].qual_type_dbal_field = DBAL_FIELD_CURRENT_CONTEXT_PROFILE;
        entry_data_intern_pruned.qual_data[cascaded_idx].qual_value = base_apptype_profile;
        entry_data_intern_pruned.qual_data[cascaded_idx].qual_mask = 0x3F;
        entry_data_intern_pruned.nof_qualifiers++;

    }

    if (entry_id_p->stage == DNX_FIELD_STAGE_IPMF2)
    {

        SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, flags, entry_id_p, &prev_entry_data));
        if (prev_entry_data.entry_valid)
        {
            /*
             * Decrease the cascading reference count of each FG cascaded by the cascading presel qualifiers
             * that are now going to be removed from the CS TCAM.
             */
            SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                            (unit, entry_id_p->stage, prev_entry_data.context_id, &prev_cascaded_from));
            SHR_IF_ERR_EXIT(dnx_field_presel_cascading_refs_dec(unit, prev_cascaded_from, &prev_entry_data));
        }
        if (entry_data_intern_pruned.entry_valid)
        {
            int profile_num;

            SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                            (unit, entry_id_p->stage, entry_data_intern_pruned.context_id, &cascaded_from));

            /*
             * Increase the cascading reference count of each FG cascaded by the cascading presel qualifiers
             * that are now going to be added to the CS TCAM.
             */
            SHR_IF_ERR_EXIT(dnx_field_presel_cascading_refs_inc(unit, cascaded_from, &entry_data_intern_pruned));
            /*
             * Add the profile number of the cascaded_from context as a presel qual.
             * (This will make the preselection occur only when coming from the cascaded_from context)
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_link_profile_get(unit, cascaded_from, &profile_num));
            /*
             * Sanity check
             */
            if (profile_num == DNX_ALGO_FIELD_CONTEXT_LINK_PROFILE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Link profile was not allocated for context %d", cascaded_from);
            }
            cascaded_idx = entry_data_intern_pruned.nof_qualifiers;
            entry_data_intern_pruned.qual_data[cascaded_idx].qual_type_dbal_field =
                DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE;
            entry_data_intern_pruned.qual_data[cascaded_idx].qual_value = profile_num;
            entry_data_intern_pruned.qual_data[cascaded_idx].qual_mask = 0x1F;
            entry_data_intern_pruned.nof_qualifiers++;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_presel_hw_set(unit, flags, entry_id_p, enable_context, &entry_data_intern_pruned));

exit:
    SHR_FUNC_EXIT;
}

/* See field_presel.h */
shr_error_e
dnx_field_presel_get(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * entry_id_p,
    dnx_field_presel_entry_data_t * entry_data_p)
{
    uint8 enable_context;
    uint32 removed_qual_value = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id_p, _SHR_E_PARAM, "entry_id_p");
    SHR_NULL_CHECK(entry_data_p, _SHR_E_PARAM, "entry_data_p");

    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, entry_data_p));

    SHR_IF_ERR_EXIT(dnx_field_presel_hw_get(unit, flags, entry_id_p, &enable_context, entry_data_p));

    /** Sanity check.*/
    if (entry_data_p->entry_valid && (entry_id_p->stage != DNX_FIELD_STAGE_EPMF) &&
        (enable_context != DNX_FIELD_PRESEL_CTX_ID_VALID_SET) &&
        (enable_context != DNX_FIELD_PRESEL_CTX_ID_VALID_RESET))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Presel %d in stage %s has ctx_id_valid %d. Unknown value.\n",
                     entry_id_p->presel_id, dnx_field_stage_text(unit, entry_id_p->stage), enable_context);
    }

    /*
     * Sanity check.
     * PCID odes not support indirect tables, therefore we do not perform the sanity check for simulations.
     */
#ifdef PLISIM
    if (!SAL_BOOT_PLISIM)
#endif
    {
        if (entry_data_p->entry_valid && (entry_id_p->stage != DNX_FIELD_STAGE_EPMF) &&
            (entry_id_p->stage != DNX_FIELD_STAGE_EXTERNAL) && (enable_context != DNX_FIELD_PRESEL_CTX_ID_VALID_SET))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Presel %d in stage %s has ctx_id_valid %d. Cannot be invalid exept in external stage.\n",
                         entry_id_p->presel_id, dnx_field_stage_text(unit, entry_id_p->stage), enable_context);
        }
    }

    if (entry_data_p->entry_valid &&
        (entry_id_p->stage == DNX_FIELD_STAGE_IPMF2 || entry_id_p->stage == DNX_FIELD_STAGE_EXTERNAL))
    {
        int qual_idx, tmp_idx;
        uint8 found = FALSE;
        dbal_fields_e dbal_field_to_remove;
        /**
         * For valid entries of stage iPMF2 we remove the auto added PROGRAM_SELECTION_PROFILE qual
         * For valid entries of stage iPMF2 we remove the auto added DBAL_FIELD_CURRENT_CONTEXT_PROFILE qual
         */
        if (entry_id_p->stage == DNX_FIELD_STAGE_IPMF2)
        {
            dbal_field_to_remove = DBAL_FIELD_PMF1_PROGRAM_SELECTION_PROFILE;
        }
        else if (entry_id_p->stage == DNX_FIELD_STAGE_EXTERNAL)
        {
            dbal_field_to_remove = DBAL_FIELD_CURRENT_CONTEXT_PROFILE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Should not reach here for stages other than iPMF2 and iFWD2. Stage %s (%d).\n",
                         dnx_field_stage_text(unit, entry_id_p->stage), entry_id_p->stage);
        }
        for (qual_idx = 0; qual_idx < entry_data_p->nof_qualifiers; qual_idx++)
        {
            if (entry_data_p->qual_data[qual_idx].qual_type_dbal_field == dbal_field_to_remove)
            {
                /** Save the value for the KBP case, to indicate the FWD predefined Apptype.*/
                removed_qual_value = entry_data_p->qual_data[qual_idx].qual_value;
                /*
                 * Remove element from array by shifting array to element place
                 */
                for (tmp_idx = qual_idx; tmp_idx < entry_data_p->nof_qualifiers - 1; tmp_idx++)
                {
                    /*
                     * Struct assignment
                     */
                    entry_data_p->qual_data[tmp_idx] = entry_data_p->qual_data[tmp_idx + 1];
                }
                entry_data_p->nof_qualifiers--;
                found = TRUE;
                break;
            }
        }
        if (!found)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Auto added field \"%s\" not added for stage %s in presel ID %d, context %d.\n",
                         dbal_field_to_string(unit, dbal_field_to_remove),
                         dnx_field_stage_text(unit, entry_id_p->stage), entry_id_p->presel_id,
                         entry_data_p->context_id);
        }
    }

    if (entry_data_p->entry_valid && (entry_id_p->stage == DNX_FIELD_STAGE_EXTERNAL))
    {
        uint32 base_apptype_profile;
        bcm_field_AppType_t apptype;
        bcm_field_AppType_t base_apptype;
        int nof_fwd_contexts;
        dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];
        uint8 opcode_id;

        base_apptype_profile = removed_qual_value;

        /*
         * Convert the context we point to to Apptype.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_profile_to_apptype_get(unit, base_apptype_profile, &base_apptype));
        if (enable_context == DNX_FIELD_PRESEL_CTX_ID_VALID_RESET)
        {
            /*
             * Predefined Apptype
             */
            apptype = base_apptype;
        }
        else if (enable_context == DNX_FIELD_PRESEL_CTX_ID_VALID_SET)
        {
            /*
             * User defined Apptype.
             * we get one of the fwd_contexts of the base_apptype to find the opcode.
             * Alternatively, we could have added a function that loops over fwd_contexts to translate
             * ACL context to opcode.
             */
            SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_fwd_contexts
                            (unit, base_apptype, &nof_fwd_contexts, fwd_contexts, acl_contexts));
            /** Sanity check.*/
            if (nof_fwd_contexts <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "AppType %d has no fwd_contexts (fwd_nof_contexts %d).\n", base_apptype, nof_fwd_contexts);
            }
            SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_get
                            (unit, fwd_contexts[0], entry_data_p->context_id, &opcode_id, NULL));
            SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_apptype_dnx_to_bcm(unit, opcode_id, &apptype));

        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Presel %d in stage %s has ctx_id_valid %d. Invalid value.\n",
                         entry_id_p->presel_id, dnx_field_stage_text(unit, entry_id_p->stage), enable_context);
        }
        entry_data_p->context_id = apptype;
    }

exit:
    SHR_FUNC_EXIT;
}

/* See field_presel.h */
shr_error_e
dnx_field_presel_free_id_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_presel_t * presel_id_p)
{
    dnx_field_presel_entry_id_t entry_id;
    dnx_field_presel_entry_data_t entry_data;
    uint32 max_nof_cs_lines;
    dnx_field_presel_t presel_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(presel_id_p, _SHR_E_PARAM, "presel_id_p");

    /** Get max number of context selection lines. */
    max_nof_cs_lines = dnx_data_field.common_max_val.nof_cs_lines_get(unit);

    *presel_id_p = DNX_FIELD_PRESEL_ID_INVALID;

    /**
     * Iterate over all preselectors from lowest priority one to the highest one (128 - 0)
     * and get information about preselector.
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, &entry_id));
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data));
    for (presel_index = (max_nof_cs_lines - 1); presel_index >= 0; presel_index--)
    {
        entry_id.presel_id = presel_index;
        entry_id.stage = field_stage;
        /** Retrieve information about the current presel_id. */
        SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, 0, &entry_id, &entry_data));
        /**
         * Check if the entry_valid is nor set for the current presel_id.
         * It means we found first free presel ID and should break and return it.
         */
        if (!entry_data.entry_valid)
        {
            *presel_id_p = presel_index;
            break;
        }
    }

    if (*presel_id_p == DNX_FIELD_PRESEL_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No Free Presel IDs were found for the given Field Stage %s!\n",
                     dnx_field_stage_text(unit, field_stage));
    }

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_id_bcm_to_dnx_convert(
    int unit,
    uint32 flags,
    bcm_field_presel_entry_id_t * bcm_entry_id_p,
    dnx_field_presel_entry_id_t * dnx_entry_id_p)
{
    uint32 cs_nof_lines;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_entry_id_p, _SHR_E_PARAM, "bcm_entry_id_p");
    SHR_NULL_CHECK(dnx_entry_id_p, _SHR_E_PARAM, "dnx_entry_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_entry_id_p->stage, &(dnx_entry_id_p->stage)));
    dnx_entry_id_p->presel_id = bcm_entry_id_p->presel_id;
    /*
     * Check the correct stage, and get its max presel ID
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_nof_lines(unit, dnx_entry_id_p->stage, &cs_nof_lines));
    if (dnx_entry_id_p->presel_id >= cs_nof_lines)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "ContextSelector ID %d is not valid for PMF stage:\"%s\"\n"
                     "Please fill entry_id.presel_id within range 0 to %d !\n",
                     dnx_entry_id_p->presel_id, dnx_field_stage_text(unit, dnx_entry_id_p->stage), cs_nof_lines - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_data_bcm_to_dnx_convert(
    int unit,
    uint32 flags,
    dnx_field_stage_e stage,
    bcm_field_presel_entry_data_t * bcm_entry_data_p,
    dnx_field_presel_entry_data_t * dnx_entry_data_p)
{
    int qual_idx;
    uint8 qual_is_supported;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_entry_data_p, _SHR_E_PARAM, "bcm_entry_data_p");
    SHR_NULL_CHECK(dnx_entry_data_p, _SHR_E_PARAM, "dnx_entry_data_p");

    dnx_entry_data_p->entry_valid = bcm_entry_data_p->entry_valid;
    dnx_entry_data_p->context_id = bcm_entry_data_p->context_id;
    dnx_entry_data_p->nof_qualifiers = bcm_entry_data_p->nof_qualifiers;

    /*
     * Iterate all the used qualifiers
     */
    for (qual_idx = 0; qual_idx < bcm_entry_data_p->nof_qualifiers; qual_idx++)
    {
        /**
         * Context selection qualifier bcmFieldQualifyForwardingType
         * in IPMF3 stage works only for FWD layer 0.
         */
        if (bcm_entry_data_p->qual_data[qual_idx].qual_type == bcmFieldQualifyForwardingType &&
            stage == DNX_FIELD_STAGE_IPMF3 && bcm_entry_data_p->qual_data[qual_idx].qual_arg != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Context selection qualifier bcmFieldQualifyForwardingType in IPMF3"
                         " stage works only for FWD layer 0 (qual_arg = 0)!\n");
        }
        /*
         * Map BCM qualifier to DNX qualifier (DBAL field).
         */
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_info_bcm_to_dnx(unit,
                                                              BCM_CORE_ALL,
                                                              stage,
                                                              dnx_entry_data_p->context_id,
                                                              &bcm_entry_data_p->qual_data[qual_idx],
                                                              &dnx_entry_data_p->qual_data[qual_idx]));
        /*
         * Verify that the DBAL field exists in the table for the unit.
         */
        SHR_IF_ERR_EXIT(dnx_field_presel_cs_qual_supported(unit, stage,
                                                           dnx_entry_data_p->qual_data[qual_idx].qual_type_dbal_field,
                                                           &qual_is_supported));
        if (qual_is_supported == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "BCM Qualifier %s (%d) with argument %d is not supported for stage:%s "
                         "on this device.",
                         dnx_field_bcm_qual_text(unit, bcm_entry_data_p->qual_data[qual_idx].qual_type),
                         bcm_entry_data_p->qual_data[qual_idx].qual_type,
                         bcm_entry_data_p->qual_data[qual_idx].qual_arg, dnx_field_stage_text(unit, stage));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_data_dnx_to_bcm_convert(
    int unit,
    uint32 flags,
    dnx_field_presel_entry_id_t * dnx_entry_id_p,
    dnx_field_presel_entry_data_t * dnx_entry_data_p,
    bcm_field_presel_entry_data_t * bcm_entry_data_p)
{
    int qual_idx;
    uint32 qual_data_uint32;
    uint8 is_std_1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_entry_id_p, _SHR_E_PARAM, "dnx_entry_id_p");
    SHR_NULL_CHECK(dnx_entry_data_p, _SHR_E_PARAM, "dnx_entry_data_p");
    SHR_NULL_CHECK(bcm_entry_data_p, _SHR_E_PARAM, "bcm_entry_data_p");

    bcm_field_presel_entry_data_info_init(bcm_entry_data_p);

    bcm_entry_data_p->entry_valid = dnx_entry_data_p->entry_valid;
    bcm_entry_data_p->context_id = dnx_entry_data_p->context_id;
    bcm_entry_data_p->nof_qualifiers = dnx_entry_data_p->nof_qualifiers;

    SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
    /*
     * Iterate all the used qualifiers
     */
    for (qual_idx = 0; qual_idx < dnx_entry_data_p->nof_qualifiers; qual_idx++)
    {
        shr_error_e rv = _SHR_E_NONE;
        uint32 bcm_qual_value;
        bcm_field_qualify_t bcm_qual_type;
        /*
         * Map DNX qualifier to BCM qualifier
         */
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_dnx_to_bcm(unit,
                                                         dnx_entry_id_p->stage, dnx_entry_data_p->context_id,
                                                         dnx_entry_data_p->qual_data[qual_idx].qual_type_dbal_field,
                                                         &(bcm_entry_data_p->qual_data[qual_idx].qual_type),
                                                         &qual_data_uint32));
        if (bcm_entry_data_p->qual_data[qual_idx].qual_type == bcmFieldQualifyForwardingTypeRaw && is_std_1)
        {
            rv = dnx_field_map_layer_type_dnx_to_bcm(unit, dnx_entry_data_p->qual_data[qual_idx].qual_value,
                                                     &bcm_qual_value);
            bcm_qual_type = bcmFieldQualifyForwardingType;
        }
        else if (bcm_entry_data_p->qual_data[qual_idx].qual_type == bcmFieldQualifyVlanFormatRaw && is_std_1)
        {
            rv = dnx_field_map_vlan_format_dnx_to_bcm(unit, dnx_entry_data_p->qual_data[qual_idx].qual_value,
                                                      &bcm_qual_value);
            bcm_qual_type = bcmFieldQualifyVlanFormat;
        }
        else
        {
            bcm_qual_type = bcm_entry_data_p->qual_data[qual_idx].qual_type;
            bcm_qual_value = dnx_entry_data_p->qual_data[qual_idx].qual_value;
        }

        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NOT_FOUND)
        {
            bcm_entry_data_p->qual_data[qual_idx].qual_value = dnx_entry_data_p->qual_data[qual_idx].qual_value;
        }
        else
        {
            bcm_entry_data_p->qual_data[qual_idx].qual_type = bcm_qual_type;
            bcm_entry_data_p->qual_data[qual_idx].qual_value = bcm_qual_value;
        }

        bcm_entry_data_p->qual_data[qual_idx].qual_arg = qual_data_uint32;
        bcm_entry_data_p->qual_data[qual_idx].qual_mask = dnx_entry_data_p->qual_data[qual_idx].qual_mask;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function zeros out all of the presel TCAM tables at init
 *  (only the key, not the payload as it is a direct table and can be initialized otherwise)
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_presel_init_zero_tables(
    int unit)
{
    dbal_tables_e dbal_table_id;
    dnx_field_stage_e field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    DNX_FIELD_STAGE_CS_QUAL_ITERATOR(field_stage)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_table_id(unit, field_stage, &dbal_table_id));
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Init a catch all entry for the default context for all stages.
 * \param [in] unit  - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_presel_default_context_default_cs_init(
    int unit)
{
    dnx_field_presel_entry_id_t entry_id;
    dnx_field_presel_entry_data_t entry_data;
    uint32 cs_flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, &entry_id));
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &entry_data));
    
    entry_data.entry_valid = TRUE;
    entry_data.context_id = DNX_FIELD_CONTEXT_ID_DEFAULT(unit);
    entry_data.nof_qualifiers = 0;
    cs_flags = 0;

    /** Configure context selection for iPMF1*/
    entry_id.stage = DNX_FIELD_STAGE_IPMF1;
    entry_id.presel_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_cs_lines - 1;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, cs_flags, &entry_id, &entry_data));
    /** Configure context selection for iPMF3*/
    entry_id.stage = DNX_FIELD_STAGE_IPMF3;
    entry_id.presel_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF3)->nof_cs_lines - 1;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, cs_flags, &entry_id, &entry_data));
    /** Configure context selection for ePMF*/
    entry_id.stage = DNX_FIELD_STAGE_EPMF;
    entry_id.presel_id = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EPMF)->nof_cs_lines - 1;
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, cs_flags, &entry_id, &entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_presel_init_zero_tables(unit));

    SHR_IF_ERR_EXIT(dnx_field_presel_default_context_default_cs_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**See field_presel.h file*/
shr_error_e
dnx_field_presel_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * The deinit function has nothing to deinit.
     */
    SHR_FUNC_EXIT;
}
