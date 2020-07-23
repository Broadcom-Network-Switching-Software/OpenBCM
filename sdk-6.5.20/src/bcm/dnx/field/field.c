/** \file field.c
 * 
 *
 * Field procedures for DNX.
 *
 * This file contains all procedures to be called by user application (Host API's)
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */
#include <sal/core/libc.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_flush.h>
#include <bcm/error.h>
#include "field_utils.h"
#include <bcm_int/dnx/field/field_range.h>
#include <bcm_int/dnx_dispatch.h>
/*
 * }Include files
 */

/**
* \brief
*  This function verifies the given Field group info to bcm_field_group_add().
* \param [in] unit        - Device Id
* \param [in] flags       - BCM_FIELD_FLAG_MSB_RESULT_ALIGN - align the result by default to MSB.
*                           Mandatory for some field groups, ignored for others.
*                           BCM_FIELD_FLAG_WITH_ID - to add a field group with ID.
*                           In case this flag is set 'fg_id' considered as input otherwise as output
* \param [in] fg_info  - Pointer to input structure of bcm_field_group_info_t that needs to be initialized
* \param [in] fg_id  - Pointer to handler-Id for the created field group.
* \return
*  int - Error Type, in sense of bcm_error_t, same as
*  shr_error_t
* \remark
*  None
* \see
*  Reference to bcm_field_group_info_t_init
*/
static int
dnx_field_group_add_verify(
    int unit,
    uint32 flags,
    bcm_field_group_info_t * fg_info,
    bcm_field_group_t * fg_id)
{
    int qual_iter;
    int qual_has_conversion;
    uint32 unsupported_flags;
    SHR_FUNC_INIT_VARS(unit);

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_MSB_RESULT_ALIGN | BCM_FIELD_FLAG_WITH_ID |
                                        BCM_FIELD_FLAG_EXTERNAL_COUNTERS | BCM_FIELD_FLAG_QUAL_PACKING))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    /** Number of qualifiers cannot exceed maximum number of qualifiers allow per group*/
    if (fg_info->nof_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of qualifiers cannot exceed maximum number of qualifiers allowed per group\n");
    }
    /** Number of actions cannot exceed maximum number of actions allow per group*/
    if (fg_info->nof_actions > BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions cannot exceed maximum number of actions allowed per group\n");
    }
    /** Number of TCAM banks requested cannot exceed maximum number of banks in BCM*/
    if (fg_info->tcam_info.nof_tcam_banks > BCM_FIELD_NUMBER_OF_TCAM_BANKS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of TCAM banks requested cannot exceed maximum number of banks.\n");
    }
    if (fg_info->tcam_info.nof_tcam_banks > 0
        && fg_info->fg_type != bcmFieldGroupTypeTcam && fg_info->fg_type != bcmFieldGroupTypeDirectTcam)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "TCAM banks reservation(nof_tcam_banks) can only be requested for field groups"
                     "of type TCAM\n");
    }
    if ((!(flags & BCM_FIELD_FLAG_MSB_RESULT_ALIGN)) &&
        ((fg_info->fg_type == bcmFieldGroupTypeTcam) ||
         (fg_info->fg_type == bcmFieldGroupTypeDirectTcam) ||
         (fg_info->fg_type == bcmFieldGroupTypeDirectMdb) || (fg_info->fg_type == bcmFieldGroupTypeExactMatch)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Flag BCM_FIELD_FLAG_MSB_RESULT_ALIGN must be set to align the result to the MSB for the field group type.\n");
    }

    /** Constant Field Groups have no key, hence number of qualifiers must be zero */
    if ((fg_info->fg_type == bcmFieldGroupTypeConst) && (fg_info->nof_quals != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number of qualifiers must be zero in constant Field Group\n");
    }

    /*
     * Make sure that no ranged qualifier uses a qualifier with value conversion.
     */
    for (qual_iter = 0; qual_iter < fg_info->nof_quals; qual_iter++)
    {
        if (fg_info->qual_is_ranged[qual_iter])
        {
            SHR_IF_ERR_EXIT(dnx_field_map_qual_has_conversion(unit, fg_info->stage, fg_info->qual_types[qual_iter],
                                                              &qual_has_conversion));
            if (qual_has_conversion)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Qualifier number %d (\"%s\") is a ranged qualifier, but also has "
                             "value convertion. This qualifier does not support ranged qualifiers.\n",
                             qual_iter, dnx_field_bcm_qual_text(unit, fg_info->qual_types[qual_iter]));
            }
        }
    }

    /*
     * Verify advanced featured for KBP field groups.
     */
    if ((fg_info->fg_type != bcmFieldGroupTypeExternalTcam) &&
        ((fg_info->external_info.capacity_valid) || (fg_info->external_info.algorithmic_mode_valid)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only external TCAM field groups can use external_info.\n");
    }
    if ((fg_info->fg_type != bcmFieldGroupTypeExternalTcam) && ((flags & BCM_FIELD_FLAG_EXTERNAL_COUNTERS) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only external TCAM field groups can use the external counters flag.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function creates a new Field group of any type on a desired stage
* \param [in] unit        - Device Id
* \param [in] flags       - BCM_FIELD_FLAG_MSB_RESULT_ALIGN - align the result by default to MSB.
*                           Mandatory for some field groups, ignored for others.
*                           BCM_FIELD_FLAG_WITH_ID - to add a field group with ID.
*                           In case this flag is set 'fg_id' considered as input otherwise as output
* \param [in] fg_info  - Pointer to input structure of bcm_field_group_info_t that needs to be initialized
* \param [in,out] fg_id  - Pointer to handler-Id for the created field group.
* \return
*  int - Error Type, in sense of bcm_error_t, same as
*  shr_error_t
* \remark
*  None
* \see
*  Reference to bcm_field_group_info_t_init
*/
int
bcm_dnx_field_group_add(
    int unit,
    uint32 flags,
    bcm_field_group_info_t * fg_info,
    bcm_field_group_t * fg_id)
{
    int qual_iter, action_iter, bank_iter;
    dnx_field_group_add_flags_e dnx_flags = 0;
    dnx_field_group_info_t dnx_field_group_info;
    dnx_field_action_t dnx_action;
    dnx_field_group_t dnx_fg_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_NULL_CHECK(fg_info, _SHR_E_PARAM, "fg_info");
    SHR_NULL_CHECK(fg_id, _SHR_E_PARAM, "fg_id");

    /** Validate input parameters. */
    SHR_INVOKE_VERIFY_DNX(dnx_field_group_add_verify(unit, flags, fg_info, fg_id));

    /**Fill in DNX level structs*/
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &dnx_field_group_info));
    if (flags & BCM_FIELD_FLAG_WITH_ID)
    {
        dnx_flags |= DNX_FIELD_GROUP_ADD_FLAG_WITH_ID;
        dnx_fg_id = *(fg_id);
    }
    if (flags & BCM_FIELD_FLAG_EXTERNAL_COUNTERS)
    {
        dnx_flags |= DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS;
    }
    if (flags & BCM_FIELD_FLAG_QUAL_PACKING)
    {
        dnx_flags |= DNX_FIELD_GROUP_ADD_FLAG_PACK_QUALIFIERS;
    }

    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, fg_info->stage, &(dnx_field_group_info.field_stage)));
    SHR_IF_ERR_EXIT(dnx_field_map_group_type_bcm_to_dnx(unit, fg_info->fg_type, &(dnx_field_group_info.fg_type)));

    /** Iterate over all input qualifiers and map them one by one to DNX qualifiers*/
    for (qual_iter = 0; qual_iter < fg_info->nof_quals; qual_iter++)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_qual_bcm_to_dnx
                                 (unit, dnx_field_group_info.field_stage, fg_info->qual_types[qual_iter],
                                  &(dnx_field_group_info.dnx_quals[qual_iter])),
                                 "Error for BCM Qualifier at index %d.%s%s\n", qual_iter, EMPTY, EMPTY);

        dnx_field_group_info.qual_is_ranged[qual_iter] = (fg_info->qual_is_ranged[qual_iter]) ? (TRUE) : (FALSE);
    }

    /** For each action array  map each BCM action to DNX action(s)*/
    for (action_iter = 0; action_iter < fg_info->nof_actions; action_iter++)
    {
        if (dnx_field_group_info.field_stage == DNX_FIELD_STAGE_IPMF1 &&
            fg_info->action_types[action_iter] == bcmFieldActionSmallExemLearn)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Small Exem Learn action is available only for IPMF2 stage.\n");
        }
        else if (dnx_field_group_info.field_stage == DNX_FIELD_STAGE_IPMF2 &&
                 fg_info->action_types[action_iter] == bcmFieldActionLargeExemLearn)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Large Exem Learn action is available only for IPMF1 stage.\n");
        }

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_action_bcm_to_dnx(unit, dnx_field_group_info.field_stage,
                                                                 fg_info->action_types[action_iter], &dnx_action),
                                 "Error for BCM Action at index %d.%s%s\n", action_iter, EMPTY, EMPTY);

        dnx_field_group_info.dnx_actions[action_iter] = dnx_action;
        dnx_field_group_info.use_valid_bit[action_iter] =
            (fg_info->action_with_valid_bit[action_iter]) ? (TRUE) : (FALSE);
    }

    sal_strncpy_s(dnx_field_group_info.name, (char *) (fg_info->name), sizeof(dnx_field_group_info.name));

    /*
     * Copy tcam_bank_ids array to DNX group info
     */
    for (bank_iter = 0; bank_iter < fg_info->tcam_info.nof_tcam_banks; bank_iter++)
    {
        dnx_field_group_info.tcam_info.tcam_bank_ids[bank_iter] = fg_info->tcam_info.tcam_bank_ids[bank_iter];
    }

    dnx_field_group_info.tcam_info.nof_tcam_banks = fg_info->tcam_info.nof_tcam_banks;
    SHR_IF_ERR_EXIT(dnx_field_map_bank_allocation_mode_bcm_to_dnx
                    (unit, fg_info->tcam_info.bank_allocation_mode,
                     &dnx_field_group_info.tcam_info.bank_allocation_mode));

    /*
     * Copy the external info.
     */
    if (fg_info->external_info.capacity_valid)
    {
        dnx_field_group_info.kbp_info.min_capacity_valid = TRUE;
        dnx_field_group_info.kbp_info.min_capacity = fg_info->external_info.capacity;
    }
    if (fg_info->external_info.algorithmic_mode_valid)
    {
        dnx_field_group_info.kbp_info.algorithmic_mode_valid = TRUE;
        dnx_field_group_info.kbp_info.algorithmic_mode = fg_info->external_info.algorithmic_mode;
    }

    /**Call DNX-level API*/
    SHR_IF_ERR_EXIT(dnx_field_group_add(unit, dnx_flags, &dnx_field_group_info, &dnx_fg_id));

    /**In case we add field group without ID get group ID from DNX layer) */
    if (!(flags & BCM_FIELD_FLAG_WITH_ID))
    {
        *fg_id = (bcm_field_group_t) dnx_fg_id;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will get all info of a specific Field Group.
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *  Field group ID for which we retrieve all relevant information.
 * \param [out] fg_info -
 *  Pointer to struct of Field info (see bcm_field_group_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_group_info_t_init
 *  - bcm_field_group_add
 */
int
bcm_dnx_field_group_info_get(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t * fg_info)
{
    int qual_iter;
    int action_iter;
    int bank_iter;
    dnx_field_group_info_t dnx_fg_basic_info;
    dnx_field_group_t dnx_fg_id;
    dnx_field_stage_e dnx_field_stage;
    uint8 is_allocated = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(fg_info, _SHR_E_PARAM, "fg_info");

    /** Initialize the output structure.*/
    bcm_field_group_info_t_init(fg_info);

    /** Fill in DNX level structs */
    SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &dnx_fg_basic_info));
    /*
     * Type cast the fg_id to be aligned to DNX type.
     */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_group_basic_info_get(unit, dnx_fg_id, &dnx_fg_basic_info));

    /** Convert DNX to BCM Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, dnx_fg_basic_info.field_stage, &(fg_info->stage)));

    /** Convert DNX to BCM Field Group Type */
    SHR_IF_ERR_EXIT(dnx_field_map_group_type_dnx_to_bcm(unit, dnx_fg_basic_info.fg_type, &(fg_info->fg_type)));

    /** Iterate over all input qualifiers, map them one by one to BCM qualifiers and their related info*/
    for (qual_iter = 0; qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; qual_iter++)
    {
        /*
         *  Last element in DNX qualifier array must be DNX_FIELD_QUAL_TYPE_INVALID.
         *  We are checking, if we reach this value it is the last qualifier in the array.
         */
        if (dnx_fg_basic_info.dnx_quals[qual_iter] == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_fg_basic_info.field_stage, dnx_fg_basic_info.dnx_quals[qual_iter],
                         &(fg_info->qual_types[qual_iter])));
        fg_info->qual_is_ranged[qual_iter] = dnx_fg_basic_info.qual_is_ranged[qual_iter];
    }

    /** Fill the number of qualifiers in bcm_field_group_info_t */
    fg_info->nof_quals = qual_iter;

    /*
     * Iterate over all actions, map them one by one to BCM qualifiers and their related info.
     * Last element in DNX actions array must be DNX_FIELD_ACTION_INVALID (unless the entire array is full.
     */
    for (action_iter = 0; action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_fg_basic_info.dnx_actions[action_iter] != DNX_FIELD_ACTION_INVALID; action_iter++)
    {
        /** For each action array map each DNX action to BCM action. */
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, dnx_field_stage,
                                                        dnx_fg_basic_info.dnx_actions[action_iter],
                                                        &(fg_info->action_types[action_iter])));

        /*
         * Fill the whether the action uses a valid bit.
         */
        fg_info->action_with_valid_bit[action_iter] = !(dnx_fg_basic_info.use_valid_bit[action_iter]);
    }

    /** Fill the number of actions in bcm_field_group_info_t. */
    fg_info->nof_actions = action_iter;

    sal_strncpy_s((char *) (fg_info->name), dnx_fg_basic_info.name, sizeof(fg_info->name));

    /** Number of TCAM banks requested cannot exceed maximum number of banks in BCM*/
    if (dnx_fg_basic_info.tcam_info.nof_tcam_banks > BCM_FIELD_NUMBER_OF_TCAM_BANKS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of TCAM banks retrieved %d exceeds maximum number of banks %d.\n",
                     dnx_fg_basic_info.tcam_info.nof_tcam_banks, BCM_FIELD_NUMBER_OF_TCAM_BANKS);
    }

    /** Copy tcam_bank_ids array to BCM group info. */
    for (bank_iter = 0; bank_iter < dnx_fg_basic_info.tcam_info.nof_tcam_banks; bank_iter++)
    {
        fg_info->tcam_info.tcam_bank_ids[bank_iter] = dnx_fg_basic_info.tcam_info.tcam_bank_ids[bank_iter];
    }
    fg_info->tcam_info.nof_tcam_banks = dnx_fg_basic_info.tcam_info.nof_tcam_banks;
    SHR_IF_ERR_EXIT(dnx_field_map_bank_allocation_mode_dnx_to_bcm(unit,
                                                                  dnx_fg_basic_info.tcam_info.bank_allocation_mode,
                                                                  &(fg_info->tcam_info.bank_allocation_mode)));

    if (dnx_fg_basic_info.fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        dnx_field_group_add_flags_e dnx_flags;
        SHR_IF_ERR_EXIT(dnx_field_group_flags_get(unit, fg_id, &dnx_flags));
        if (dnx_flags & DNX_FIELD_GROUP_ADD_FLAG_EXTERNAL_COUNTERS)
        {
#if defined(INCLUDE_KBP)
            /** get handle from dbal*/
            dbal_tables_e dbal_table_id;
            kbp_db_handles_t kbp_db_handles;

            SHR_IF_ERR_EXIT(dnx_field_group_dbal_table_id_get(unit, fg_id, &dbal_table_id));
            SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                            (unit, dbal_table_id, DBAL_ACCESS_METHOD_KBP, (void *) &kbp_db_handles));
            (fg_info->ext_acl_db) = (int *) (kbp_db_handles.db_p);
#else
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field group %d is external TCAM type, but KBP is not compiled.\r\n", fg_id);
#endif
        }
        if (dnx_fg_basic_info.kbp_info.min_capacity_valid)
        {
            fg_info->external_info.capacity_valid = TRUE;
            fg_info->external_info.capacity = dnx_fg_basic_info.kbp_info.min_capacity;
        }
        if (dnx_fg_basic_info.kbp_info.algorithmic_mode_valid)
        {
            fg_info->external_info.algorithmic_mode_valid = TRUE;
            fg_info->external_info.algorithmic_mode = dnx_fg_basic_info.kbp_info.algorithmic_mode;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete Field Group, deallocate all relevant resources
* \param [in] unit   - Device Id
* \param [in] fg_id  - Field Group Id to delete
* \return
*  int - Error Type, in sense of bcm_error_t, same as
*  shr_error_t
* \remark
*  None
* \see
*  Reference to bcm_field_group_add
*/
int
bcm_dnx_field_group_delete(
    int unit,
    bcm_field_group_t fg_id)
{
    dnx_field_group_t dnx_fg_id;
    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_START(unit);

    /**convert BCM to DNX FG ID types*/
    dnx_fg_id = (dnx_field_group_t) (fg_id);

    /**Call DNX-level API*/
    SHR_IF_ERR_EXIT(dnx_field_group_delete(unit, dnx_fg_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will verify input parameters to bcm_field_context_create().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  BCM_FIELD_FLAG_WITH_ID - to add a context with ID.
 *  In case this flag is set 'context_id' considered as input otherwise as output.
 * \param [in] stage -
 *  Field stage on which the context id should be allocated.
 * \param [in] context_info -
 *  Pointer to struct of Context info (see bcm_field_context_info_t in field.h)
 * \param [in] context_id -
 *  Pointer to handle-id for the created context.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to bcm_field_context_info_t_init/get
 */
static int
dnx_field_context_create_verify(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_info_t * context_info,
    bcm_field_context_t * context_id)
{
    uint32 unsupported_flags;
    SHR_FUNC_INIT_VARS(unit);

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_WITH_ID))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    if (stage != bcmFieldStageIngressPMF1)
    {
        if (context_info->hashing_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "hasing_enabled is only allowed for stage \"%s\". Stage is \"%s\".\n",
                         dnx_field_bcm_stage_text(bcmFieldStageIngressPMF1), dnx_field_bcm_stage_text(stage));
        }
        if (context_info->context_compare_modes.compare_1_mode != bcmFieldContextCompareTypeNone)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "hasing_enabled is only allowed for stage \"%s\". Stage is \"%s\".\n",
                         dnx_field_bcm_stage_text(bcmFieldStageIngressPMF1), dnx_field_bcm_stage_text(stage));
        }
        if (context_info->context_compare_modes.compare_2_mode != bcmFieldContextCompareTypeNone)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "hasing_enabled is only allowed for stage \"%s\". Stage is \"%s\".\n",
                         dnx_field_bcm_stage_text(bcmFieldStageIngressPMF1), dnx_field_bcm_stage_text(stage));
        }
    }
    if (stage != bcmFieldStageIngressPMF2 && stage != bcmFieldStageExternal)
    {
        if (context_info->cascaded_from != BCM_FIELD_CONTEXT_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "cascaded_from is only allowed for stages \"%s\" and \"%s\". "
                         "Stage is \"%s\", cascaded_from is %d.\n",
                         dnx_field_bcm_stage_text(bcmFieldStageIngressPMF2),
                         dnx_field_bcm_stage_text(bcmFieldStageExternal),
                         dnx_field_bcm_stage_text(stage), context_info->cascaded_from);
        }
    }
    if (stage != bcmFieldStageIngressPMF1 && stage != bcmFieldStageIngressPMF2)
    {
        if (context_info->state_table_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "state_table_enabled is only allowed for stages \"%s\" and \"%s\". Stage is \"%s\".\n",
                         dnx_field_bcm_stage_text(bcmFieldStageIngressPMF1),
                         dnx_field_bcm_stage_text(bcmFieldStageIngressPMF2), dnx_field_bcm_stage_text(stage));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will create a new Field context on a desired stage.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  BCM_FIELD_FLAG_WITH_ID - to add a context with ID.
 *  In case this flag is set 'context_id' considered as input otherwise as output.
 * \param [in] stage -
 *  Field stage on which the context id should be allocated.
 * \param [in] context_info -
 *  Pointer to struct of Context info (see bcm_field_context_info_t in field.h)
 * \param [in,out] context_id -
 *  Pointer to handle-id for the created context.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to bcm_field_context_info_t_init/get
 */
int
bcm_dnx_field_context_create(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_info_t * context_info,
    bcm_field_context_t * context_id)
{
    dnx_field_context_flags_e dnx_context_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_NULL_CHECK(context_info, _SHR_E_PARAM, "context_info");
    SHR_NULL_CHECK(context_id, _SHR_E_PARAM, "context_id");

    /** Validate input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_field_context_create_verify(unit, flags, stage, context_info, context_id));

    dnx_context_flags = 0;

    /*
     * According to the stage, we either create an Apptype or a context.
     */
    if (stage == bcmFieldStageExternal)
    {
        dnx_field_context_apptype_info_t apptype_info;
        bcm_field_AppType_t apptype;
        /*
         * Create an Apptype.
         */

        /** Fill in DNX level struct */
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_t_init(unit, &apptype_info));
        apptype_info.base_static_apptype = (dnx_field_context_t) (context_info->cascaded_from);
        sal_strncpy_s(apptype_info.name, (char *) (context_info->name), sizeof(apptype_info.name));
        /** Type cast the context_id to be aligned to Apptype type. */
        apptype = *context_id;
        /** Sanity check: verify that no information was lost in the conversion.*/
        if (apptype != *context_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Value changed when converting from context type to Apptype type (%d->%d).\n",
                         *context_id, apptype);
        }

        /** Call DNX-level API */
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_create(unit, dnx_context_flags, &apptype_info, &apptype));

        /** In case we create Field context without ID get Apptype from DNX layer */
        if (!(flags & BCM_FIELD_FLAG_WITH_ID))
        {
            /** Type cast the dnx_context_id to be aligned to BCM type. */
            *context_id = apptype;
            /** Sanity check: verify that no information was lost in the conversion.*/
            if (apptype != *context_id)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Value changed when converting from Apptype type to context type (%d->%d).\n",
                             apptype, *context_id);
            }
        }
    }
    else
    {
        dnx_field_stage_e dnx_stage;
        dnx_field_context_mode_t dnx_context_mode;
        dnx_field_context_t dnx_context_id;
        /*
         * Create a context.
         */

        /** Fill in DNX level struct */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_context_mode));
        if (flags & BCM_FIELD_FLAG_WITH_ID)
        {
            dnx_context_flags |= DNX_FIELD_CONTEXT_FLAG_WITH_ID;
            /** Type cast the context_id to be aligned to DNX type. */
            dnx_context_id = (*context_id);
            /** Sanity check: verify that no information was lost in the conversion.*/
            if (dnx_context_id != *context_id)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Value changed when converting from BCM type to DNX type (%d->%d).\n",
                             *context_id, dnx_context_id);
            }
        }
        if (stage == bcmFieldStageIngressPMF1)
        {
            dnx_context_mode.context_ipmf1_mode.hash_mode = context_info->hashing_enabled;
            SHR_IF_ERR_EXIT(dnx_field_map_context_compare_type_bcm_to_dnx
                            (unit, context_info->context_compare_modes.compare_1_mode,
                             &(dnx_context_mode.context_ipmf1_mode.compare_mode_1)));
            SHR_IF_ERR_EXIT(dnx_field_map_context_compare_type_bcm_to_dnx
                            (unit, context_info->context_compare_modes.compare_2_mode,
                             &(dnx_context_mode.context_ipmf1_mode.compare_mode_2)));
        }
        else if (stage == bcmFieldStageIngressPMF2)
        {
            dnx_context_mode.context_ipmf2_mode.cascaded_from = (dnx_field_context_t) (context_info->cascaded_from);
        }

        if (stage == bcmFieldStageIngressPMF1 || stage == bcmFieldStageIngressPMF2)
        {
            dnx_context_mode.state_table_mode = context_info->state_table_enabled;
        }

        sal_strncpy_s(dnx_context_mode.name, (char *) (context_info->name), sizeof(dnx_context_mode.name));

        /** Convert BCM to DNX Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_stage));

        /** Call DNX-level API */
        SHR_IF_ERR_EXIT(dnx_field_context_create
                        (unit, dnx_context_flags, dnx_stage, &dnx_context_mode, &dnx_context_id));

        /** In case we create Field context without ID get Context ID from DNX layer */
        if (!(flags & BCM_FIELD_FLAG_WITH_ID))
        {
            /** Type cast the dnx_context_id to be aligned to BCM type. */
            *context_id = dnx_context_id;
            /** Sanity check: verify that no information was lost in the conversion.*/
            if (dnx_context_id != *context_id)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Value changed when converting from DNX type to BCM type (%d->%d).\n",
                             dnx_context_id, *context_id);
            }
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will get all context info of a Field context.
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage from which the context info should be taken.
 * \param [in] context_id -
 *  Pointer to handle-id for the created context.
 * \param [out] context_info -
 *  Pointer to struct of Context info (see bcm_field_context_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to bcm_field_context_info_t_init/create
 */
int
bcm_dnx_field_context_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_context_info_t * context_info)
{
    dnx_field_stage_e dnx_stage;
    dnx_field_context_mode_t dnx_context_mode;
    dnx_field_context_t dnx_context_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_context_mode));

    /** Validate input parameters */
    SHR_NULL_CHECK(context_info, _SHR_E_PARAM, "context_info");

    /*
     * According to the stage, we either create an Apptype or a context.
     */
    if (stage == bcmFieldStageExternal)
    {
        dnx_field_context_apptype_info_t apptype_info;
        bcm_field_AppType_t apptype;

        /** Type cast the context_id to be aligned to Apptype type. */
        apptype = context_id;
        /** Sanity check: verify that no information was lost in the conversion.*/
        if (apptype != context_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Value changed when converting from context type to Apptype type (%d->%d).\n",
                         context_id, apptype);
        }

        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_t_init(unit, &apptype_info));
        SHR_IF_ERR_EXIT(dnx_field_context_apptype_info_get(unit, apptype, &apptype_info));

        context_info->cascaded_from = apptype_info.base_static_apptype;
        sal_strncpy_s((char *) context_info->name, apptype_info.name, sizeof(context_info->name));
    }
    else
    {
        /** Type cast the context_id to be aligned to DNX type. */
        dnx_context_id = (dnx_field_context_t) context_id;

        /** Convert BCM to DNX Field Stage */
        SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_stage));

        /** Call DNX-level API */
        /** Get all information about the given Context ID from DNX layer */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_get(unit, dnx_stage, dnx_context_id, &dnx_context_mode));

        /** Fill in BCM level struct */
        SHR_IF_ERR_EXIT(dnx_field_map_context_compare_type_dnx_to_bcm
                        (unit, dnx_context_mode.context_ipmf1_mode.compare_mode_1,
                         &(context_info->context_compare_modes.compare_1_mode)));
        SHR_IF_ERR_EXIT(dnx_field_map_context_compare_type_dnx_to_bcm
                        (unit, dnx_context_mode.context_ipmf1_mode.compare_mode_2,
                         &(context_info->context_compare_modes.compare_2_mode)));
        context_info->hashing_enabled = dnx_context_mode.context_ipmf1_mode.hash_mode;
        context_info->cascaded_from = (bcm_field_context_t) (dnx_context_mode.context_ipmf2_mode.cascaded_from);
        sal_strncpy_s((char *) (context_info->name), dnx_context_mode.name, sizeof(context_info->name));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies input parameters to bcm_field_context_destroy().
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id should be verified.
 * \param [in] context_id -
 *  Context id.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to bcm_field_context_create/get
 */
static int
dnx_field_context_destroy_verify(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (stage == bcmFieldStageExternal)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context create and destroy for stage external refer to Apptypes. "
                     "Apptype destroy not supported (nor context destroy in stage external).\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroy Context ID, deallocate all relevant resources.
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id should be destroyed.
 * \param [in] context_id -
 *  Pointer to handle-id for the context,
 *  which has to be destroyed.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to bcm_field_context_create/get
 */
int
bcm_dnx_field_context_destroy(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    dnx_field_stage_e dnx_stage;
    dnx_field_context_t dnx_context_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_context_destroy_verify(unit, stage, context_id));

    /** Convert BCM to DNX Field Context ID */
    dnx_context_id = (dnx_field_context_t) (context_id);
    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_stage));

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_destroy(unit, dnx_stage, dnx_context_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This Function verifies input params of bcm_field_group_context_attach().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  FG id to attach context to, after attach API context will
 *  also perform the lookup configured in the FG.
 * \param [in] context_id -
 *  Pointer to handle-id for the created context.
 * \param [in] attach_info -
 *  Pointer to struct of Group attach info (see bcm_field_group_attach_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_group_attach_info_t_init
 *    - bcm_dnx_field_group_context_info_get
 */
static int
dnx_field_group_context_attach_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_group_attach_info_t * attach_info)
{
    dnx_field_group_t dnx_fg_id;
    dnx_field_group_type_e fg_type;
    uint32 unsupported_flags;

    SHR_FUNC_INIT_VARS(unit);

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    /** Get the field group type. */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));

    /** Validate input parameters */
    SHR_NULL_CHECK(attach_info, _SHR_E_PARAM, "attach_info");

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_32_RESULT_MSB_ALIGN))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    /** Number of qualifiers cann't exceed maximum number of qualifiers allow per group*/
    if (attach_info->key_info.nof_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of qualifiers cann't exceed maximum number %d of qualifiers allowed per group\n",
                     BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
    }
    /** Number of actions cann't exceed maximum number of actions allow per group*/
    if (attach_info->payload_info.nof_actions > BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of actions cann't exceed maximum number %d of actions allowed per group\n",
                     BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP);
    }

    /** Compare ID is only relevant for TCAM FGs */
    if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM && attach_info->compare_id != BCM_FIELD_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "compare ID relevant only for TCAM FG type \n");
    }
    /** For TCAM FG need to check that compare ID is in range */
    if (attach_info->compare_id != BCM_FIELD_INVALID &&
        attach_info->compare_id >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_PAIRS_IN_COMPARE_MODE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "compare ID %d exceeds maximum number of %d\n",
                     attach_info->compare_id, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_COMPARE_PAIRS_IN_COMPARE_MODE);
    }

    /** Verify the number of invalidated FEMs.*/
    if (attach_info->payload_info.nof_invalidate_fems < 0 ||
        attach_info->payload_info.nof_invalidate_fems > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_invalidate_fems %d, must be within the range of 0 and %d.\n",
                     attach_info->payload_info.nof_invalidate_fems, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID);
    }
    if (attach_info->payload_info.nof_invalidate_fems != 0 && attach_info->payload_info.invalidate_fems == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_invalidate_fems %d, but no invalidate_fems array to read from.\n",
                     attach_info->payload_info.nof_invalidate_fems);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This Function Attach Field Group to specific context ID,
 *  and defines the key template and action priorities
 *  for this lookup for this specific context.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  FG id to attach context to, after attach API context will
 *  also perform the lookup configured in the FG.
 * \param [in] context_id -
 *  Pointer to handle-id for the created context.
 * \param [in] attach_info -
 *  Pointer to struct of Group attach info (see bcm_field_group_attach_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_group_attach_info_t_init
 *    - bcm_dnx_field_group_context_info_get
 */
int
bcm_dnx_field_group_context_attach(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_group_attach_info_t * attach_info)
{
    uint32 dnx_context_flags;
    dnx_field_group_t dnx_fg_id;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_group_type_e fg_type;
    dnx_field_group_attach_info_t dnx_attach_info;
    dnx_field_action_t dnx_action;
    dnx_field_context_t dnx_context_id;
    int unencoded_offset;
    int qual_iter;
    int action_iter;
    int fem_invalidate_iter;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Type cast the context_id to be aligned to DNX type. */
    dnx_context_id = (dnx_field_context_t) context_id;
    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    /** Get the field group stage. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));

    SHR_INVOKE_VERIFY_DNX(dnx_field_group_context_attach_verify(unit, flags, fg_id, context_id, attach_info));

    /** Fill in DNX level struct */
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &dnx_attach_info));

    dnx_context_flags = flags;
    if (flags & BCM_FIELD_FLAG_32_RESULT_MSB_ALIGN)
    {
        dnx_context_flags = DNX_FIELD_GROUP_CONTEXT_ATTACH_FLAG_32_RESULT_MSB_ALIGN;
    }

    /** Iterate over all input qualifiers, map them one by one to DNX qualifiers and their all info related */
    for (qual_iter = 0; qual_iter < attach_info->key_info.nof_quals; qual_iter++)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_qual_bcm_to_dnx
                                 (unit, dnx_field_stage, attach_info->key_info.qual_types[qual_iter],
                                  &(dnx_attach_info.dnx_quals[qual_iter])),
                                 "Error for BCM Qualifier at index %d. Field group is %d, context is %d.\n",
                                 qual_iter, fg_id, context_id);

        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_bcm_to_dnx
                        (unit, attach_info->key_info.qual_info[qual_iter].input_type,
                         &(dnx_attach_info.qual_info[qual_iter].input_type)));
        /** If we have input type METADATA, use the offset encoding to determine if it is METADATA or METADADA 2.*/
        unencoded_offset = attach_info->key_info.qual_info[qual_iter].offset;
        /** Handle non native Meta Data qualifier for user defined qualifier.*/
        if ((attach_info->key_info.qual_info[qual_iter].input_type == bcmFieldInputTypeMetaData) &&
            (attach_info->key_info.qual_info[qual_iter].offset & SAL_BIT(QUAL_OFFSET_META2_BIT)))

        {
            if (dnx_field_stage == DNX_FIELD_STAGE_IPMF2)
            {
                if (dnx_attach_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
                {
                    if ((DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[qual_iter])) == DNX_FIELD_QUAL_CLASS_USER)
                    {
                        dnx_attach_info.qual_info[qual_iter].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
                        unencoded_offset = unencoded_offset & SAL_RBIT(QUAL_OFFSET_META2_BIT);
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "Offset of qual %d (\"%s\") is encoded as metadata 2 in stage \"%s\", but the "
                                     "qualifier is not user defined.\n",
                                     qual_iter, dnx_field_bcm_qual_text(unit,
                                                                        attach_info->key_info.qual_types[qual_iter]),
                                     dnx_field_stage_text(unit, dnx_field_stage));
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Offset of qual %d (\"%s\") is encoded as metadata 2 in stage \"%s\", but the "
                                 "input type is %d, and not metadata (%d).\n",
                                 qual_iter, dnx_field_bcm_qual_text(unit, attach_info->key_info.qual_types[qual_iter]),
                                 dnx_field_stage_text(unit, dnx_field_stage),
                                 dnx_attach_info.qual_info[qual_iter].input_type, DNX_FIELD_INPUT_TYPE_META_DATA);
                }
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Offset of qual %d (\"%s\") is encoded as metadata 2, but the stage is \"%s\", "
                             "and only iPMF2 supports metadata 2.\n",
                             qual_iter, dnx_field_bcm_qual_text(unit, attach_info->key_info.qual_types[qual_iter]),
                             dnx_field_stage_text(unit, dnx_field_stage));
            }
        }
        /** Handle non native Meta Data qualifier for predefined qualifier.*/
        if ((DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[qual_iter])) == DNX_FIELD_QUAL_CLASS_META2)
        {
            if (dnx_attach_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_META_DATA)
            {
                dnx_attach_info.qual_info[qual_iter].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Qual %d (\"%s\") is a predefined native metadata qualifier in stage \"%s\", but the "
                             "input type is %d, and not metadata (%d).\n",
                             qual_iter, dnx_field_bcm_qual_text(unit, attach_info->key_info.qual_types[qual_iter]),
                             dnx_field_stage_text(unit, dnx_field_stage),
                             dnx_attach_info.qual_info[qual_iter].input_type, DNX_FIELD_INPUT_TYPE_META_DATA);
            }
        }
        dnx_attach_info.qual_info[qual_iter].input_arg = attach_info->key_info.qual_info[qual_iter].input_arg;
        dnx_attach_info.qual_info[qual_iter].offset = unencoded_offset;
    }

    /** For each action array map each BCM action to DNX action(s)*/
    for (action_iter = 0; action_iter < attach_info->payload_info.nof_actions; action_iter++)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_action_bcm_to_dnx
                                 (unit, dnx_field_stage, attach_info->payload_info.action_types[action_iter],
                                  &dnx_action),
                                 "Error for BCM Action at index %d. Field group is %d, context is %d.\n",
                                 action_iter, fg_id, context_id);

        dnx_attach_info.dnx_actions[action_iter] = dnx_action;
        dnx_attach_info.action_info[action_iter].priority = attach_info->payload_info.action_info[action_iter].priority;

        if (attach_info->payload_info.action_info[action_iter].valid_bit_polarity !=
            DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE
            && attach_info->payload_info.action_info[action_iter].valid_bit_polarity !=
            DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Given value for valid_bit_polarity %d is invalid. Valid bit polarity must be 0 or 1!\n",
                         attach_info->payload_info.action_info[action_iter].valid_bit_polarity);
        }

        dnx_attach_info.action_info[action_iter].valid_bit_polarity =
            attach_info->payload_info.action_info[action_iter].valid_bit_polarity;
    }
    /** For each FEM to be invalidated, map to FEM ID.*/
    for (fem_invalidate_iter = 0; fem_invalidate_iter < attach_info->payload_info.nof_invalidate_fems;
         fem_invalidate_iter++)
    {
        DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(dnx_attach_info.fems_invalidate[fem_invalidate_iter], unit,
                                                      attach_info->payload_info.invalidate_fems[fem_invalidate_iter]);
    }
    /** Fill the payload id in DNX level struct */
    dnx_attach_info.payload_id = attach_info->payload_info.payload_id;
    /** Fill the payload offset in DNX level struct */
    dnx_attach_info.payload_offset = attach_info->payload_info.payload_offset;

    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_compare_id_bcm_to_dnx
                        (unit, attach_info->compare_id, &dnx_attach_info.compare_id));
    }

    if (fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        /*
         * Convert apptype to opcode and pass it on as context.
         */
        dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, context_id, &opcode_id));
        dnx_context_id = opcode_id;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_group_context_attach
                    (unit, dnx_context_flags, dnx_fg_id, dnx_context_id, &dnx_attach_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will get all Group attach info of
 *  a specific Field context and Field group.
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *  Field group id, where context is attached.
 * \param [in] context_id -
 *  ID of the created context.
 * \param [in,out] attach_info -
 *  Pointer to struct of Group attach info (see bcm_field_group_attach_info_t in field.h)
 *  attach_info.payload_info.invalidate_fems is a pointer to a memory provided by the caller.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_group_attach_info_t_init
 *    - bcm_dnx_field_group_context_attach
 */
int
bcm_dnx_field_group_context_info_get(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_group_attach_info_t * attach_info)
{
    dnx_field_group_attach_info_t dnx_attach_info;
    dnx_field_context_t dnx_context_id;
    dnx_field_group_t dnx_fg_id;
    dnx_field_stage_e dnx_field_stage;
    int qual_iter;
    int bcm_cur_action_iter, bcm_action_iter, dnx_action_iter, action_type_exist;
    bcm_field_action_t bcm_action;
    dnx_field_stage_e field_stage;
    int fem_id_iter;
    dnx_field_fem_id_t invalidated_fem_id;
    bcm_field_action_priority_t invalidated_fem_priority;
    bcm_field_action_priority_t *invalidated_fem_priority_tmp_pointer;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(attach_info, _SHR_E_PARAM, "attach_info");

    /** Fill in DNX level struct */
    SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &dnx_attach_info));

    /** Initialize the output array. But preserve the user's FEM invalidate array.*/
    invalidated_fem_priority_tmp_pointer = attach_info->payload_info.invalidate_fems;
    bcm_field_group_attach_info_t_init(attach_info);
    attach_info->payload_info.invalidate_fems = invalidated_fem_priority_tmp_pointer;

    /** Type cast the context_id to be aligned to DNX type. */
    dnx_context_id = (dnx_field_context_t) context_id;
    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &field_stage));

    if (field_stage == DNX_FIELD_STAGE_EXTERNAL)
    {
        /*
         * Convert apptype to opcode and pass it on as context.
         */
        dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;
        SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, context_id, &opcode_id));
        dnx_context_id = opcode_id;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_group_context_basic_info_get(unit, fg_id, dnx_context_id, &dnx_attach_info, NULL));

    /** Get the field group stage. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    /** Iterate over all input qualifiers, map them one by one to DNX qualifiers and their all info related */
    for (qual_iter = 0; qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; qual_iter++)
    {
        /*
         *  Last element in DNX qualifier array must be DNX_FIELD_QUAL_TYPE_INVALID.
         *  We are checking, if we reach this value it is the last qualifier in the array.
         */
        if (dnx_attach_info.dnx_quals[qual_iter] == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_attach_info.dnx_quals[qual_iter],
                         &(attach_info->key_info.qual_types[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm
                        (unit, dnx_attach_info.qual_info[qual_iter].input_type,
                         &(attach_info->key_info.qual_info[qual_iter].input_type)));
        attach_info->key_info.qual_info[qual_iter].input_arg = dnx_attach_info.qual_info[qual_iter].input_arg;
        attach_info->key_info.qual_info[qual_iter].offset = dnx_attach_info.qual_info[qual_iter].offset;
        /** encode the offset for Metadata2*/
        if (((DNX_QUAL_CLASS(dnx_attach_info.dnx_quals[qual_iter])) == DNX_FIELD_QUAL_CLASS_USER) &&
            (dnx_attach_info.qual_info[qual_iter].input_type == DNX_FIELD_INPUT_TYPE_META_DATA2))
        {
            attach_info->key_info.qual_info[qual_iter].offset |= SAL_BIT(QUAL_OFFSET_META2_BIT);
        }

    }

    /** Fill the number of qualifiers in bcm_field_group_attach_info_t */
    attach_info->key_info.nof_quals = qual_iter;

    /** Iterate on output action array and fill its info */
    bcm_action_iter = 0;

    /**
     * Each BCM action can be constructed them more then one DNX action, fill in the DNX info struct accordingly.
     * Last element in DNX action array must be DNX_FIELD_ACTION_INVALID.
     * We are checking, if we reach this value it is the last action in the array.
     */
    for (dnx_action_iter = 0; dnx_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_attach_info.dnx_actions[dnx_action_iter] != DNX_FIELD_ACTION_INVALID; dnx_action_iter++)
    {
        /** For each action array map each DNX action to BCM action. */
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, dnx_field_stage,
                                                        dnx_attach_info.dnx_actions[dnx_action_iter], &(bcm_action)));

        for (bcm_cur_action_iter = 0; bcm_cur_action_iter < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP;
             bcm_cur_action_iter++)
        {
            /**
             * Check if "bcm_action" is already written to the action array,
             * indicate that action_type_exist is TRUE and leave the loop.
             * If "bcm_action" not exist set action_type_exist to FALSE.
             */
            if (attach_info->payload_info.action_types[bcm_cur_action_iter] == bcm_action)
            {
                action_type_exist = TRUE;
                break;
            }
            action_type_exist = FALSE;
        }
        /**
         * Check if the action_type_exist is FALSE, then fill the output
         * action array info.
         */
        if (action_type_exist == FALSE)
        {
            
            attach_info->payload_info.action_types[bcm_action_iter] = bcm_action;
            attach_info->payload_info.action_info[bcm_action_iter].priority =
                dnx_attach_info.action_info[dnx_action_iter].priority;
            attach_info->payload_info.action_info[bcm_action_iter].valid_bit_polarity =
                dnx_attach_info.action_info[dnx_action_iter].valid_bit_polarity;
            bcm_action_iter++;
        }
    }

    /** Fill the number of actions in bcm_field_group_attach_info_t */
    attach_info->payload_info.nof_actions = bcm_action_iter;

    /** Fill the payload id in BCM level struct. */
    attach_info->payload_info.payload_id = dnx_attach_info.payload_id;
    /** Fill the payload offset in BCM level struct. */
    attach_info->payload_info.payload_offset = dnx_attach_info.payload_offset;

    /** Fill the invalidated FEMs.*/
    for (fem_id_iter = 0;
         fem_id_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FEM_ID
         && dnx_attach_info.fems_invalidate[fem_id_iter] != DNX_FIELD_FEM_ID_INVALID; fem_id_iter++)
    {
        if (attach_info->payload_info.invalidate_fems != NULL)
        {
            invalidated_fem_id = dnx_attach_info.fems_invalidate[fem_id_iter];
            DNX_FIELD_ACTION_FEM_ID_ENCODED_POSITION_GET(invalidated_fem_priority, unit, invalidated_fem_id);
            attach_info->payload_info.invalidate_fems[fem_id_iter] = invalidated_fem_priority;
        }
    }
    attach_info->payload_info.nof_invalidate_fems = fem_id_iter;
    if ((attach_info->payload_info.invalidate_fems == NULL) && (fem_id_iter != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There are %d invalidated FEMs, \"invalidate_fems\" is NULL.\n", fem_id_iter);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Reset/deallocate all resources that were allocated
 *  for the context ID at input to the FG at input
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *  Field group id, where context is attached.
 * \param [in] context_id -
 *  ID of the created context, which
 *  has to be deallocated.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_group_context_attach
 *    - bcm_dnx_field_group_context_info_get
 */
int
bcm_dnx_field_group_context_detach(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id)
{
    dnx_field_context_t dnx_context_id;
    dnx_field_group_t dnx_fg_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Type cast the context_id to be aligned to DNX type. */
    dnx_context_id = (dnx_field_context_t) context_id;
    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_group_context_detach(unit, dnx_fg_id, dnx_context_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will create a new Field action on a desired stage.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  BCM_FIELD_FLAG_WITH_ID - to add a user action with ID.
 *  In case this flag is set 'action_id' considered as input otherwise as output.
 * \param [in] action_info -
 *  Pointer to struct of Action info (see bcm_field_action_info_t in field.h)
 * \param [in,out] action_id -
 *  Pointer to handle-id for the created user action.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_action_info_t_init
 *  - bcm_field_action_info_get
 */
int
bcm_dnx_field_action_create(
    int unit,
    uint32 flags,
    bcm_field_action_info_t * action_info,
    bcm_field_action_t * action_id)
{
    dnx_field_action_flags_e dnx_flags;
    dnx_field_action_in_info_t dnx_action_info_in;
    uint32 unsupported_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    dnx_flags = 0;

    /** Validate input parameters */
    SHR_NULL_CHECK(action_info, _SHR_E_PARAM, "action_info");
    SHR_NULL_CHECK(action_id, _SHR_E_PARAM, "action_id");

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_WITH_ID))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    /** Fill in DNX level struct */
    SHR_IF_ERR_EXIT(dnx_field_action_in_info_t_init(unit, &dnx_action_info_in));
    if (flags & BCM_FIELD_FLAG_WITH_ID)
    {
        dnx_flags |= DNX_FIELD_ACTION_FLAG_WITH_ID;
    }
    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, action_info->stage, &(dnx_action_info_in.stage)));
    sal_strncpy_s(dnx_action_info_in.name, (char *) (action_info->name), sizeof(dnx_action_info_in.name));
    dnx_action_info_in.size = action_info->size;
    dnx_action_info_in.bcm_action = action_info->action_type;
    dnx_action_info_in.prefix = action_info->prefix_value;
    dnx_action_info_in.prefix_size = action_info->prefix_size;

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_action_create(unit, dnx_flags, &dnx_action_info_in, action_id, NULL));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Maps the bcm value of the given action in the given stage to its corresponding HW value.
 *  This function is used to retrieve HW values of non-trivial user defined actions to be set
 *  in all APIs that involve receiving values for those actions
 * \param [in] unit -
 *  Device ID
 * \param [in] bcm_stage -
 *  Stage of the action for which the value is to be mapped
 * \param [in] bcm_action -
 *  The action for which the value is to be mapped
 * \param [in] bcm_value -
 *  BCM value to be mapped
 * \param [out] hw_value -
 *  Mapped HW value
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_action_create
 */
int
bcm_dnx_field_action_value_map(
    int unit,
    bcm_field_stage_t bcm_stage,
    bcm_field_action_t bcm_action,
    uint32 bcm_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS],
    uint32 hw_value[BCM_FIELD_ACTION_WIDTH_IN_WORDS])
{
    dnx_field_stage_e dnx_stage;
    bcm_field_entry_action_t bcm_action_info;
    dnx_field_entry_action_t dnx_action_info;
    int i_data;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(bcm_value, _SHR_E_PARAM, "bcm_value");
    SHR_NULL_CHECK(hw_value, _SHR_E_PARAM, "hw_value");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    bcm_action_info.type = bcm_action;
    for (i_data = 0; i_data < BCM_FIELD_ACTION_WIDTH_IN_WORDS; i_data++)
    {
        bcm_action_info.value[i_data] = bcm_value[i_data];
    }

    SHR_IF_ERR_EXIT(dnx_field_map_action_info_bcm_to_dnx(unit, 0, dnx_stage, &bcm_action_info, &dnx_action_info));

    for (i_data = 0; i_data < BCM_FIELD_ACTION_WIDTH_IN_WORDS; i_data++)
    {
        hw_value[i_data] = dnx_action_info.action_value[i_data];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets all information of a user defined action.
 * \param [in] unit -
 *  Device ID
 * \param [in] action_id -
 *  User created action ID.
 * \param [in] stage -
 *  Field stage for which to get the action info. Only relevant for predefined actions.
 * \param [out] action_info -
 *  Pointer to struct of Action info (see bcm_field_action_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_action_info_t_init
 *  - bcm_field_action_create
 */
int
bcm_dnx_field_action_info_get(
    int unit,
    bcm_field_action_t action_id,
    bcm_field_stage_t stage,
    bcm_field_action_info_t * action_info)
{
    bcm_field_stage_t bcm_stage_used;
    dnx_field_stage_e dnx_stage;
    dnx_field_action_t dnx_action;
    unsigned int action_size;
    uint32 prefix_value;
    unsigned int prefix_size;
    dnx_field_action_in_info_t udq_action_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(action_info, _SHR_E_PARAM, "action_info");

    /*
     * For user defined actions, get the stage from the action.
     */
    if (DNX_FIELD_BCM_ACTION_IS_USER(unit, action_id))
    {
        SHR_IF_ERR_EXIT(dnx_field_action_get(unit, action_id, &udq_action_info));
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, udq_action_info.stage, &bcm_stage_used));
    }
    else
    {
        bcm_stage_used = stage;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage_used, &dnx_stage));
    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, dnx_stage, action_id, &dnx_action));

    action_info->stage = bcm_stage_used;
    sal_strncpy_s((char *) (action_info->name), dnx_field_dnx_action_text(unit, dnx_action), sizeof(action_info->name));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_size(unit, dnx_stage, dnx_action, &action_size));
    action_info->size = action_size;
    SHR_IF_ERR_EXIT(dnx_field_map_bcm_action_base_action(unit, action_id, &(action_info->action_type)));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_prefix(unit, dnx_stage, dnx_action, &prefix_value, &prefix_size));
    action_info->prefix_value = prefix_value;
    action_info->prefix_size = prefix_size;

    /*
     * Sanity check
     */
    if (DNX_FIELD_BCM_ACTION_IS_USER(unit, action_id))
    {
        dnx_field_action_t base_dnx_action;
        bcm_field_action_t base_bcm_action;
        bcm_field_stage_t udq_bcm_stage;

        SHR_IF_ERR_EXIT(dnx_field_action_get(unit, action_id, &udq_action_info));
        if (action_info->size != udq_action_info.size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action Id %d: Size of action %d does not matched saved size %d.\n",
                         action_id, action_info->size, udq_action_info.size);
        }
        if (action_info->prefix_value != udq_action_info.prefix)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action Id %d: Prefix of action %d does not matched saved size %d.\n",
                         action_id, action_info->prefix_value, udq_action_info.prefix);
        }
        if (action_info->prefix_size != udq_action_info.prefix_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action Id %d: Prefix size of action %d does not matched saved size %d.\n",
                         action_id, action_info->prefix_size, udq_action_info.prefix_size);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_action_base_action(unit, dnx_stage, dnx_action, &base_dnx_action));
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, dnx_stage, base_dnx_action, &base_bcm_action));
        if (action_info->action_type != base_bcm_action)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action Id %d: Base action ID %d does not match the one "
                         "mapped back from DNX %d.\n", action_id, action_info->action_type, base_bcm_action);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, udq_action_info.stage, &udq_bcm_stage));
        if (action_info->stage != udq_bcm_stage)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Action Id %d: Stage %d does not matched action stage %d. "
                         "This should have been verified prior to this point.\n",
                         action_id, action_info->stage, udq_bcm_stage);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroys an existing user action based.
 * \param [in] unit -
 *  Device ID
 * \param [in] action_id -
 *  User created action ID to be destroyed.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to:
 *   - bcm_field_action_create
 *   - bcm_field_action_info_get
 */
int
bcm_dnx_field_action_destroy(
    int unit,
    bcm_field_action_t action_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_action_destroy(unit, action_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will create a new Field qualifier on a desired stage.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  BCM_FIELD_FLAG_WITH_ID - to add a user qualifier with ID.
 *  In case this flag is set 'qualifier_id' considered as input otherwise as output.
 * \param [in] qual_info -
 *  Pointer to struct of Qualifier info (see bcm_field_qualifier_info_create_t in field.h)
 * \param [in,out] qualifier_id -
 *  Pointer to handle-id for the created user qualifier.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_qualifier_info_t_init
 *  - bcm_field_qualifier_info_get
 */
int
bcm_dnx_field_qualifier_create(
    int unit,
    uint32 flags,
    bcm_field_qualifier_info_create_t * qual_info,
    bcm_field_qualify_t * qualifier_id)
{
    dnx_field_action_flags_e dnx_flags;
    dnx_field_qualifier_in_info_t dnx_qualifier_info_in;
    uint32 unsupported_flags;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    dnx_flags = 0;

    /** Validate input parameters */
    SHR_NULL_CHECK(qual_info, _SHR_E_PARAM, "qual_info");
    SHR_NULL_CHECK(qualifier_id, _SHR_E_PARAM, "qualifier_id");

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_WITH_ID))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    /** Fill in DNX level struct */
    SHR_IF_ERR_EXIT(dnx_field_qual_in_info_init(unit, &dnx_qualifier_info_in));
    if (flags & BCM_FIELD_FLAG_WITH_ID)
    {
        dnx_flags |= DNX_FIELD_QUALIFIER_FLAG_WITH_ID;
    }
    sal_strncpy_s(dnx_qualifier_info_in.name, (char *) (qual_info->name), sizeof(dnx_qualifier_info_in.name));
    dnx_qualifier_info_in.size = qual_info->size;

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_qual_create(unit, dnx_flags, &dnx_qualifier_info_in, qualifier_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Maps the bcm value of the given qualifier in the given stage to its corresponding HW value.
 *  This function is used to retrieve HW values of non-trivial user defined qualifiers to be set
 *  in all APIs that involve receiving values for those qualifiers
 * \param [in] unit -
 *  Device ID
 * \param [in] bcm_stage -
 *  Stage of the qualifier for which the value is to be mapped
 * \param [in] bcm_qual -
 *  The qualifier for which the value is to be mapped
 * \param [in] bcm_value -
 *  BCM value to be mapped
 * \param [out] hw_value -
 *  Mapped HW value
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_qualifier_create
 */
int
bcm_dnx_field_qualifier_value_map(
    int unit,
    bcm_field_stage_t bcm_stage,
    bcm_field_qualify_t bcm_qual,
    uint32 bcm_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS],
    uint32 hw_value[BCM_FIELD_QUAL_WIDTH_IN_WORDS])
{
    dnx_field_stage_e dnx_stage;
    bcm_field_entry_qual_t bcm_qual_info;
    dnx_field_entry_qual_t dnx_qual_info;
    bcm_field_presel_entry_data_t bcm_cs_qual_info;
    dnx_field_presel_entry_data_t dnx_cs_qual_info;
    int i_data;
    int cs_exists = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(bcm_value, _SHR_E_PARAM, "bcm_value");
    SHR_NULL_CHECK(hw_value, _SHR_E_PARAM, "hw_value");

    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    /*
     * If CS - handle differently.
     * We assume that the mapping function will be that same for a qualifier which is
     * used for key and for the context selection presel.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_bcm_mapping_exists(unit, dnx_stage, bcm_qual, &cs_exists));
    if (cs_exists)
    {
        bcm_field_presel_entry_data_info_init(&bcm_cs_qual_info);
        bcm_cs_qual_info.context_id = 0;        /* Default Context */
        bcm_cs_qual_info.entry_valid = TRUE;
        bcm_cs_qual_info.nof_qualifiers = 1;
        for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
        {
            bcm_cs_qual_info.qual_data[i_data].qual_type = bcm_qual;
            bcm_cs_qual_info.qual_data[i_data].qual_value = bcm_value[i_data];
            /*
             * Mask is not used in the mapping fuction, so it is 0
             */
            bcm_cs_qual_info.qual_data[i_data].qual_mask = 0;
            /*
             * Arg has no implication on the qualifier value. In some cases arg has implication on the bcm_to_dnx
             * mappnig, and we will have different dnx qualifier according to arg (for dbal purposes); but even then
             * all those dnx quals will have the same conversion (mapping) function. We set it to 0.
             */
            bcm_cs_qual_info.qual_data[i_data].qual_arg = 0;
        }

        SHR_IF_ERR_EXIT(dnx_field_presel_data_bcm_to_dnx_convert
                        (unit, 0, dnx_stage, &bcm_cs_qual_info, &dnx_cs_qual_info));

        for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
        {
            hw_value[i_data] = dnx_cs_qual_info.qual_data[i_data].qual_value;
        }
    }
    else
    {
        bcm_qual_info.type = bcm_qual;
        for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
        {
            bcm_qual_info.value[i_data] = bcm_value[i_data];
            bcm_qual_info.mask[i_data] = 0;
            bcm_qual_info.max_value[i_data] = 0;
        }

        SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, 0, dnx_stage, &bcm_qual_info, &dnx_qual_info));
        for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
        {
            hw_value[i_data] = dnx_qual_info.qual_value[i_data];
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets all information of a qualifier.
 * \param [in] unit -
 *  Device ID
 * \param [in] qualifier_id -
 *  User created qualifier ID.
 * \param [out] stage -
 *  The field stage used by the qualifier, if it is predefined.
 *  If the qualifier is user defined, than the stage input parameter is ignored.
 *  User defined qualifiers have no stage, stage is updated with an illegal value.
 * \param [out] qual_info -
 *  Pointer to struct of Qualifier info (see bcm_field_qualifier_info_get_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_qualifier_info_t_init
 *  - bcm_field_qualifier_create
 */
int
bcm_dnx_field_qualifier_info_get(
    int unit,
    bcm_field_qualify_t qualifier_id,
    bcm_field_stage_t stage,
    bcm_field_qualifier_info_get_t * qual_info)
{
    bcm_field_stage_t bcm_stage_used;
    dnx_field_stage_e dnx_stage;
    dnx_field_qual_t dnx_qual;
    uint32 size;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(qual_info, _SHR_E_PARAM, "qual_info");

    /*
     * Check if Qualifier Id is within the range of user defined qualifiers. If so, use an arbitrary stage, as user
     * defined qualifiers don't have a stage.
     */
    if (DNX_FIELD_BCM_QUAL_IS_USER(unit, qualifier_id))
    {
        bcm_stage_used = bcmFieldStageIngressPMF1;
    }
    else
    {
        bcm_stage_used = stage;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage_used, &dnx_stage));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_stage, qualifier_id, &dnx_qual));

    sal_strncpy_s((char *) (qual_info->name), dnx_field_dnx_qual_text(unit, dnx_qual), sizeof(qual_info->name));

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, dnx_stage, dnx_qual, &size));
    qual_info->size = size;

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, dnx_stage, dnx_qual, &(qual_info->offset)));

    switch (DNX_QUAL_CLASS(dnx_qual))
    {
        case DNX_FIELD_QUAL_CLASS_META2:
        {
            /** Note META2 has the same BCM class as MET (could have been implemented by removing the break)*/
            qual_info->qual_class = bcmFieldQualifierClassMetaData;
            /** Add the MetaData 2 bit to the offset.*/
            qual_info->offset |= SAL_BIT(QUAL_OFFSET_META2_BIT);
            break;
        }
        case DNX_FIELD_QUAL_CLASS_META:
        {
            qual_info->qual_class = bcmFieldQualifierClassMetaData;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_LAYER_RECORD:
        {
            qual_info->qual_class = bcmFieldQualifierClassLayerRecord;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_HEADER:
        {
            qual_info->qual_class = bcmFieldQualifierClassHeader;
            break;
        }
        case DNX_FIELD_QUAL_CLASS_USER:
        {
            qual_info->qual_class = bcmFieldQualifierClassUserCreated;
            /** Sanity check */
            if (DNX_FIELD_BCM_QUAL_IS_USER(unit, qualifier_id) == FALSE)
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM Qual ID:%d is out of range of user defines qualifiers:%d-%d, "
                             "but is is mapped to a user defined qualifier 0X%X.\n",
                             qualifier_id, dnx_data_field.qual.user_1st_get(unit),
                             dnx_data_field.qual.user_1st_get(unit) + dnx_data_field.qual.user_nof_get(unit) - 1,
                             dnx_qual);
            break;
        }
        case DNX_FIELD_QUAL_CLASS_VW:
        {
            /*
             * Virtual wire qualifiers are treated as predefined meta data qualifiers.
             */
            qual_info->qual_class = bcmFieldQualifierClassMetaData;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported class type: %s (%d).\n",
                         dnx_field_qual_class_text(DNX_QUAL_CLASS(dnx_qual)), DNX_QUAL_CLASS(dnx_qual));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function returns the offset of the given qualifier in the field group that its ID is
 *  given. Returns an error in case the qualifier was not found inside the given field group.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  TBD
 * \param [in] fg_id -
 *  Field group id, that contains the action we want to get the offset for
 * \param [in] bcm_qual -
 *  The BCM qualifier to return the offset for
 * \param [out] offset_p -
 *  The offset for the given qualifier for the given fg_id
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_fem_action_info_t_init
 *  - bcm_field_fem_action_add
 */
int
bcm_dnx_field_group_qualifier_offset_get(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_qualify_t bcm_qual,
    int *offset_p)
{
    dnx_field_qual_t dnx_qual;
    unsigned int offset;
    unsigned int width;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(offset_p, _SHR_E_PARAM, "offset_p");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    /*
     * Get field group stage and map BCM action to DNX action
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &dnx_field_stage));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_field_stage, bcm_qual, &dnx_qual));

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_group_qualifier_info_get(unit, fg_id, dnx_qual, &offset, &width));
    *offset_p = offset;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroys an existing user qualifier based.
 * \param [in] unit -
 *  Device ID
 * \param [in] qual_id -
 *  User created qualifier ID to be destroyed.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_field_qualifier_create
 *   - bcm_field_qualifier_info_get
 */
int
bcm_dnx_field_qualifier_destroy(
    int unit,
    bcm_field_qualify_t qual_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_qual_destroy(unit, qual_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
* Verify the Field entry Add according to constraints
*
*   \param [in] unit         - Relevant unit.
*   \param [in] flags        - entry_add flags.
*   \param [in] fg_id        - FG Id to add the entry to.
*   \param [in] entry_info   - Pointer to struct of Entry info (see bcm_field_entry_info_t in field.h).
*   \param [in] entry_handle - Pointer to handle-id for the added entry.
*
* \return
*   \retval  Zero if no error was detected
*   \retval  Negative if error was detected. See \ref shr_error_e
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_entry_add_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_entry_info_t * entry_info,
    bcm_field_entry_t * entry_handle)
{
    uint8 is_allocated = FALSE;
    dnx_field_group_t dnx_fg_id;
    uint32 unsupported_flags;
    dnx_field_group_type_e fg_type;
    int i_data, qual_iter;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_info, _SHR_E_PARAM, "entry_info");

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));

    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM ||
        fg_type == DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM || fg_type == DNX_FIELD_GROUP_TYPE_KBP)
    {
        /*
         * TCAM, TCAM DT and KBP should have entry handle
         */
        SHR_NULL_CHECK(entry_handle, _SHR_E_PARAM, "entry_handle");
    }

    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM && fg_type != DNX_FIELD_GROUP_TYPE_TCAM
        && entry_info->core != DBAL_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Core ID selection is available only for TCAM or TCAM_DT FGs\n");
    }

    if (fg_type != DNX_FIELD_GROUP_TYPE_TCAM && fg_type != DNX_FIELD_GROUP_TYPE_KBP && entry_info->priority != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Priority selection is available only for TCAM FGs (External included)\n");
    }

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_UPDATE | BCM_FIELD_FLAG_WITH_ID))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    if (fg_type != DNX_FIELD_GROUP_TYPE_KBP)
    {
        for (qual_iter = 0; qual_iter < entry_info->nof_entry_quals; qual_iter++)
        {
            for (i_data = 0; i_data < BCM_FIELD_QUAL_WIDTH_IN_WORDS; i_data++)
            {
                if (entry_info->entry_qual[qual_iter].max_value[i_data] != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Max value selection is available only for external TCAM in the BCM\n");
                }
            }
        }
    }

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            if (flags & BCM_FIELD_FLAG_UPDATE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "UPDATE flag is not supported for direct table TCAM lookup entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not supported for direct table TCAM lookup entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            /*
             * UPDATE and WITH_ID flags are supported.
             */
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not supported for EXEM/Direct MDB lookup entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not supported for external TCAM lookup entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            if (flags & BCM_FIELD_FLAG_UPDATE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "UPDATE flag is not supported for state table entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not supported for state table entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            if (flags & BCM_FIELD_FLAG_UPDATE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "UPDATE flag is not supported for MDB DT entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "WITH_ID flag is not supported for MDB DT entries. "
                             "Flags 0x%X, fg_id %d.\n", flags, fg_id);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Illegal FG Type! The bcm_dnx_field_entry_add() not supported for FG type %d\n", fg_type);
            break;
        }
    }

    if ((flags & BCM_FIELD_FLAG_UPDATE) && (flags & BCM_FIELD_FLAG_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag UPDATE can't be used together with flag WITH_ID\n");
    }

    /** Number of qualifiers and actions must be valid numbers between 0 and max value. */
    if ((entry_info->nof_entry_quals == BCM_FIELD_INVALID) || (entry_info->nof_entry_actions == BCM_FIELD_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry must be set with valid number of actions and qualifiers\n");
    }
    if ((entry_info->nof_entry_quals < 0) || (entry_info->nof_entry_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry number of qualifiers must be within array range (0 to %d)\n",
                     BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
    }
    if ((entry_info->nof_entry_actions < 0) || (entry_info->nof_entry_actions > BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry number of actions must be within array range (0 to %d)\n",
                     BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, dnx_fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", dnx_fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates, sets and installs to HW
 *  a single entry (ACL rule) for a specific field group.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  Field group id, to which entry is added.
 * \param [in] entry_info -
 *  Pointer to struct of Entry info (see bcm_field_entry_info_t in field.h)
 * \param [out] entry_handle -
 *  Pointer to handle-id for the added entry.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_entry_info_t_init
 *  - bcm_field_entry_info_get
 */
int
bcm_dnx_field_entry_add(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_entry_info_t * entry_info,
    bcm_field_entry_t * entry_handle)
{
    dnx_field_group_type_e fg_type;
    dnx_field_group_t dnx_fg_id;
    dnx_field_entry_t dnx_entry_info;
    uint32 dnx_flags = 0;
    uint32 dnx_entry_handle;

    dnx_field_stage_e dnx_field_stage;
    int qual_iter, action_iter;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_entry_add_verify(unit, flags, fg_id, entry_info, entry_handle));

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &dnx_entry_info));

    /**
     * Prepare the entry payload DNX struct.
     * For each action array  map each BCM action  to  DNX action(s).
     */
    for (action_iter = 0; action_iter < entry_info->nof_entry_actions; action_iter++)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_action_info_bcm_to_dnx(unit, entry_info->core, dnx_field_stage,
                                                                      &(entry_info->entry_action[action_iter]),
                                                                      &(dnx_entry_info.
                                                                        payload_info.action_info[action_iter])),
                                 "Error for BCM Action at index %d. Field group is %d.%s\n", action_iter, fg_id, EMPTY);
    }

    /** Last element in each DNX action array must be DNX_FIELD_ACTION_INVALID */
    if (action_iter < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP)
    {
        dnx_entry_info.payload_info.action_info[action_iter].dnx_action = DNX_FIELD_ACTION_INVALID;
    }

    /** Iterate over all input qualifiers and map them one by one to DNX qualifiers*/
    for (qual_iter = 0; qual_iter < entry_info->nof_entry_quals; qual_iter++)
    {
        /*
         * info bcm to dnx fetches dnx qualifier as well as data & mask
         */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_qual_info_bcm_to_dnx(unit, entry_info->core, dnx_field_stage,
                                                                    &(entry_info->entry_qual[qual_iter]),
                                                                    &(dnx_entry_info.key_info.qual_info[qual_iter])),
                                 "Error for BCM Qualifier at index %d. Field group is %d.%s\n", qual_iter, fg_id,
                                 EMPTY);
    }

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            /*
             * Copy priority just in order to report an error when priority is set for DT 
             */
            dnx_entry_info.priority = entry_info->priority;
            dnx_entry_info.core = entry_info->core;
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_dt_add
                            (unit, dnx_flags, dnx_fg_id, &dnx_entry_info, &dnx_entry_handle));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            dnx_entry_info.priority = entry_info->priority;
            dnx_entry_info.core = entry_info->core;
            dnx_entry_info.valid_bit = entry_info->valid_bit;
            if (flags & BCM_FIELD_FLAG_UPDATE)
            {
                dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_UPDATE;
            }
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID;
            }
            /** Copy existing value for update/with_id cases, in all other cases, value will be overridden */
            dnx_entry_handle = *entry_handle;

            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, dnx_flags, dnx_fg_id, &dnx_entry_info, &dnx_entry_handle));

            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            if (flags & BCM_FIELD_FLAG_UPDATE)
            {
                dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_UPDATE;
            }
            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_exem_add(unit, dnx_flags, dnx_fg_id, &dnx_entry_info));

            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            dnx_entry_info.priority = entry_info->priority;

            if (flags & BCM_FIELD_FLAG_UPDATE)
            {
                dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_UPDATE;
            }
            if (flags & BCM_FIELD_FLAG_WITH_ID)
            {
                dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID;
            }
            /** Copy existing value for update/with_id cases, in all other cases, value will be overridden */
            dnx_entry_handle = *entry_handle;

            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_kbp_add(unit, dnx_flags, dnx_fg_id, &dnx_entry_info, &dnx_entry_handle));

            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_state_table_add(unit, dnx_field_stage, &(dnx_entry_info.key_info)));

            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_mdb_dt_add(unit, dnx_flags, dnx_fg_id, &dnx_entry_info));

            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Illegal FG Type! The bcm_dnx_field_entry_add() not supported for FG type %d\n", fg_type);
            break;
        }
    }

    if (entry_handle != NULL)
    {
        *entry_handle = (bcm_field_entry_t) dnx_entry_handle;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function will get all info for a specific
 *  entry (ACL rule) of a specific field group.
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *  Field group id, to which entry is added.
 * \param [in] entry_handle -
 *  Entry handle for which we retrieve all relevant information.
 * \param [in,out] entry_info -
 *  Pointer to struct of Entry info (see bcm_field_entry_info_t in field.h).
 *  Note: in case if we want to add an EXEM or state table entry, we should provide entry key
 *  information in bcm_field_entry_info_t: entry_info.entry_qual.
 *  For state table we also expect the core to be given in entry_info.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_entry_info_t_init
 *  - bcm_field_entry_info_add
 */
int
bcm_dnx_field_entry_info_get(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_entry_t entry_handle,
    bcm_field_entry_info_t * entry_info)
{
    dnx_field_group_type_e fg_type;
    dnx_field_group_t dnx_fg_id;
    dnx_field_entry_t dnx_entry_info;
    dnx_field_stage_e dnx_field_stage;
    uint32 dnx_entry_handle;
    int qual_iter = 0, ii;
    int bcm_cur_action_iter, bcm_action_iter, dnx_action_iter, action_type_exist;
    bcm_field_action_t bcm_action;
    uint8 is_allocated = FALSE;
    bcm_field_entry_qual_t input_entry_qual[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    int input_nof_entry_quals;
    bcm_core_t input_core;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(entry_info, _SHR_E_PARAM, "entry_info");

    /** Copy the input from entry_info.*/
    sal_memcpy(input_entry_qual, entry_info->entry_qual, sizeof(input_entry_qual));
    input_nof_entry_quals = entry_info->nof_entry_quals;

    /** Save input core for future use */
    input_core = entry_info->core;

    /** Initialize entry_info.*/
    bcm_field_entry_info_t_init(entry_info);

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    dnx_entry_handle = (uint32) entry_handle;
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, dnx_fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", dnx_fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));
    SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &dnx_entry_info));

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            /** Number of qualifiers must be valid numbers between 0 and max value. */
            if ((input_nof_entry_quals < 0) || (input_nof_entry_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP) ||
                (input_nof_entry_quals == BCM_FIELD_INVALID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Entry number of qualifiers (%d) must be within array range (0 to %d) for FG type %s\n",
                             input_nof_entry_quals, BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP,
                             dnx_field_group_type_e_get_name(fg_type));
            }
            /** Iterate over all input qualifiers and map them one by one to DNX qualifiers */
            for (qual_iter = 0; qual_iter < input_nof_entry_quals; qual_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, input_core, dnx_field_stage,
                                                                   &(input_entry_qual[qual_iter]),
                                                                   &(dnx_entry_info.key_info.qual_info[qual_iter])));
            }

            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_state_table_get(unit, dnx_field_stage, &(dnx_entry_info.key_info)));
        }
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            dnx_entry_info.core = entry_info->core;
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_get(unit, dnx_fg_id, dnx_entry_handle, &dnx_entry_info));

            /*
             * valid_bit is only relevant for TCAM, not DT
             */
            entry_info->valid_bit = dnx_entry_info.valid_bit;
        }
            break;
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_kbp_get(unit, dnx_fg_id, dnx_entry_handle, &dnx_entry_info));
        }
            break;
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            /** Number of qualifiers must be valid numbers between 0 and max value. */
            if ((input_nof_entry_quals < 0) || (input_nof_entry_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP) ||
                (input_nof_entry_quals == BCM_FIELD_INVALID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Entry number of qualifiers (%d) must be within array range (0 to %d) for FG type %s\n",
                             input_nof_entry_quals, BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP,
                             dnx_field_group_type_e_get_name(fg_type));
            }
            /** Iterate over all input qualifiers and map them one by one to DNX qualifiers */
            for (qual_iter = 0; qual_iter < input_nof_entry_quals; qual_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, BCM_CORE_ALL, dnx_field_stage,
                                                                   &(input_entry_qual[qual_iter]),
                                                                   &(dnx_entry_info.key_info.qual_info[qual_iter])));
            }

            /** Call DNX-level entry get API */
            SHR_IF_ERR_EXIT(dnx_field_entry_exem_get(unit, dnx_fg_id, &(dnx_entry_info.key_info), &dnx_entry_info));

        }
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            /** Number of qualifiers must be valid numbers between 0 and max value. */
            if ((input_nof_entry_quals < 0) || (input_nof_entry_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP) ||
                (input_nof_entry_quals == BCM_FIELD_INVALID))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Entry number of qualifiers (%d) must be within array range (0 to %d) for FG type %s\n",
                             input_nof_entry_quals, BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP,
                             dnx_field_group_type_e_get_name(fg_type));
            }
            /** Iterate over all input qualifiers and map them one by one to DNX qualifiers */
            for (qual_iter = 0; qual_iter < input_nof_entry_quals; qual_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, BCM_CORE_ALL, dnx_field_stage,
                                                                   &(input_entry_qual[qual_iter]),
                                                                   &(dnx_entry_info.key_info.qual_info[qual_iter])));
            }

            /** Call DNX-level entry get API */
            SHR_IF_ERR_EXIT(dnx_field_entry_mdb_dt_get(unit, dnx_fg_id, &(dnx_entry_info.key_info), &dnx_entry_info));

        }
            break;
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Illegal FG Type! The bcm_dnx_field_entry_get() not supported for FG type %d\n", fg_type);
        }
            break;
    }

    /** Return retrieved priority, only relevant for TCAM/KBP entries */
    entry_info->priority = dnx_entry_info.priority;

    /** Iterate over all qualifiers, map them one by one to DNX qualifiers and all their info */
    for (qual_iter = 0; qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; qual_iter++)
    {
        /*
         *  Last element in DNX qualifier array must be DNX_FIELD_QUAL_TYPE_INVALID.
         *  We are checking, if we reach this value it is the last qualifier in the array.
         */
        if (dnx_entry_info.key_info.qual_info[qual_iter].dnx_qual == DNX_FIELD_QUAL_TYPE_INVALID)
        {
            break;
        }
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_entry_info.key_info.qual_info[qual_iter].dnx_qual,
                         &(entry_info->entry_qual[qual_iter].type)));
        for (ii = 0; ii < BCM_FIELD_QUAL_WIDTH_IN_WORDS; ii++)
        {
            entry_info->entry_qual[qual_iter].value[ii] = dnx_entry_info.key_info.qual_info[qual_iter].qual_value[ii];
            entry_info->entry_qual[qual_iter].max_value[ii] =
                dnx_entry_info.key_info.qual_info[qual_iter].qual_max_value[ii];
            entry_info->entry_qual[qual_iter].mask[ii] = dnx_entry_info.key_info.qual_info[qual_iter].qual_mask[ii];
        }
    }

    /** Fill the number of qualifiers in bcm_field_entry_info_t */
    entry_info->nof_entry_quals = qual_iter;

    /** Iterate on output action array and fill its info */
    bcm_action_iter = 0;

    /**
     * Each BCM action can be constructed them more then one DNX action, fill in the DNX info struct accordingly.
     * Last element in DNX action array must be DNX_FIELD_ACTION_INVALID.
     * We are checking, if we reach this value it is the last action in the array.
     */
    for (dnx_action_iter = 0; dnx_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_entry_info.payload_info.action_info[dnx_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID;
         dnx_action_iter++)
    {
        /** For each action array map each DNX action to BCM action. */
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, dnx_field_stage,
                                                        dnx_entry_info.payload_info.
                                                        action_info[dnx_action_iter].dnx_action, &(bcm_action)));

        for (bcm_cur_action_iter = 0; bcm_cur_action_iter < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP;
             bcm_cur_action_iter++)
        {
            /**
             * Check if "bcm_action" is already written to the entry action array,
             * indicate that action_type_exist is TRUE and leave the loop.
             * If "bcm_action" not exist set action_type_exist to FALSE.
             */
            if (entry_info->entry_action[bcm_cur_action_iter].type == bcm_action)
            {
                action_type_exist = TRUE;
                break;
            }
            action_type_exist = FALSE;
        }
        /**
         * Check if the action_type_exist is FALSE, then fill the output
         * action array info.
         */
        if (action_type_exist == FALSE)
        {
            entry_info->entry_action[bcm_action_iter].type = bcm_action;
            for (ii = 0; ii < BCM_FIELD_ACTION_WIDTH_IN_WORDS; ii++)
            {
                entry_info->entry_action[bcm_action_iter].value[ii] =
                    dnx_entry_info.payload_info.action_info[dnx_action_iter].action_value[ii];
            }
            bcm_action_iter++;
        }
    }

    /** Fill the number of actions in bcm_field_entry_info_t */
    entry_info->nof_entry_actions = bcm_action_iter;
    entry_info->core = dnx_entry_info.core;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete Entry From HW and free all allocated resources.
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *  Field group id, to which entry is added.
 * \param [in] entry_qual_info -
 *  Pointer to an array of structs of Entry qual info (see bcm_field_entry_qual_t in field.h).
 *  Used only in case if we want to delete EXEM entry. In other cases it should be
 *  set to NULL.
 * \param [in] entry_handle -
 *  Handle to the created entry, which
 *  has to be deleted.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_entry_add
 *    - bcm_dnx_field_entry_info_get
 */
int
bcm_dnx_field_entry_delete(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP],
    bcm_field_entry_t entry_handle)
{
    dnx_field_group_type_e fg_type;
    dnx_field_group_t dnx_fg_id;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_entry_key_t dnx_entry_key_info;
    int qual_iter = 0;
    uint8 is_allocated = FALSE;
    uint32 dnx_entry_handle;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    dnx_entry_handle = (uint32) entry_handle;
    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, dnx_fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", dnx_fg_id);
    }
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));
    /*
     * Init dnx_entry_key_info
     */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_iter++)
    {
        dnx_entry_key_info.qual_info[qual_iter].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    /** Call DNX-level API */
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_tcam_delete(unit, dnx_fg_id, entry_handle, &dnx_entry_key_info));
        }
            break;
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {

            SHR_NULL_CHECK(entry_qual_info, _SHR_E_PARAM, "entry_qual_info");

            /** Iterate over all input qualifiers and map them one by one to DNX qualifiers*/
            for (qual_iter = 0; qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP &&
                 entry_qual_info[qual_iter].type != bcmFieldQualifyCount; qual_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, BCM_CORE_ALL, dnx_field_stage,
                                                                   &(entry_qual_info[qual_iter]),
                                                                   &(dnx_entry_key_info.qual_info[qual_iter])));
            }

            SHR_IF_ERR_EXIT(dnx_field_entry_exem_delete(unit, dnx_fg_id, &dnx_entry_key_info));
        }
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {

            SHR_NULL_CHECK(entry_qual_info, _SHR_E_PARAM, "entry_qual_info");

            /** Iterate over all input qualifiers and map them one by one to DNX qualifiers*/
            for (qual_iter = 0; qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP &&
                 entry_qual_info[qual_iter].type != bcmFieldQualifyCount; qual_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, BCM_CORE_ALL, dnx_field_stage,
                                                                   &(entry_qual_info[qual_iter]),
                                                                   &(dnx_entry_key_info.qual_info[qual_iter])));
            }

            SHR_IF_ERR_EXIT(dnx_field_entry_mdb_dt_delete(unit, dnx_fg_id, &dnx_entry_key_info));
        }
            break;
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_kbp_delete(unit, dnx_fg_id, dnx_entry_handle));
        }
            break;
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            SHR_NULL_CHECK(entry_qual_info, _SHR_E_PARAM, "entry_qual_info");

            /** Iterate over all input qualifiers and map them one by one to DNX qualifiers*/
            for (qual_iter = 0; qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP &&
                 entry_qual_info[qual_iter].type != bcmFieldQualifyCount; qual_iter++)
            {
                SHR_IF_ERR_EXIT(dnx_field_map_qual_info_bcm_to_dnx(unit, BCM_CORE_ALL, dnx_field_stage,
                                                                   &(entry_qual_info[qual_iter]),
                                                                   &(dnx_entry_key_info.qual_info[qual_iter])));
            }

            /** Call DNX-level entry add API */
            SHR_IF_ERR_EXIT(dnx_field_entry_state_table_delete(unit, dnx_field_stage, &dnx_entry_key_info));

        }
            break;
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Illegal FG Type! The bcm_dnx_field_entry_delete() not supported for FG type %d\n", fg_type);
        }
            break;
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete all entries in all field groups.
 * \param [in] unit -
 *  Device ID
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_entry_delete
 */
int
bcm_dnx_field_entry_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

/**
 * \brief
 *  This function is used to verify input params of bcm_field_context_hash_create().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the HASH should be created.
 * \param [in] hash_info -
 *  Pointer to struct of HASH info (see bcm_field_context_hash_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_hash_info_t_init
 */
static int
dnx_field_context_hash_create_verify(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_context_hash_info_t * hash_info)
{
    uint32 unsupported_flags;
    int hash_config_iter;
    SHR_FUNC_INIT_VARS(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(hash_info, _SHR_E_PARAM, "hash_info");

    if ((unsupported_flags = (flags & ~BCM_FIELD_FLAG_UPDATE)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    /** The HASH feature is available only on IPMF1 stage! */
    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Hash' can only be configured on bcmFieldStageIngressPMF1.\r\n");
    }

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id (%d) can't exceed maximum number %d of Context IDs allow per field stage\n",
                     context_id, dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

    /** Number of qualifiers cann't exceed maximum number of qualifiers allow per group. */
    if (hash_info->key_info.nof_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of qualifiers (%d) can't exceed maximum number %d of qualifiers in qual_types array\n",
                     hash_info->key_info.nof_quals, BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
    }
    if (hash_info->key_info.nof_quals > DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of qualifiers (%d) can't exceed maximum number %d of qualifiers allowed per group\n",
                     hash_info->key_info.nof_quals, DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);
    }

    /** Number of hash config can't exceed maximum number of action keys per context. */
    if (hash_info->nof_additional_hash_config > BCM_FIELD_NUMBER_OF_HASH_KEYS
        || hash_info->nof_additional_hash_config > DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS - 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of hash_config (%d) can't exceed maximum number of action_keys allowed per context\n",
                     hash_info->nof_additional_hash_config);
    }

    /** Verify hash function is defined */
    if (hash_info->hash_function == bcmFieldContextHashFunctionCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hash config: must define hash_function\n");
    }

    /** Verify there is at least one action key defined */
    if (hash_info->hash_config.action_key == bcmFieldContextHashActionKeyCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hash config: must define hash_config action_key\n");
    }

    /** Verify CRC select is specified if and only if selecting an augment function */
    if (hash_info->hash_config.crc_select == bcmFieldCrcSelectInvalid &&
        (hash_info->hash_config.function_select == bcmFieldContextHashActionValueAugmentCrc ||
         hash_info->hash_config.function_select == bcmFieldContextHashActionValueAugmentKey))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Hash config: CRC Select must be specified when selecting an augment function\n");
    }

    if (hash_info->hash_config.crc_select != bcmFieldCrcSelectInvalid &&
        (hash_info->hash_config.function_select == bcmFieldContextHashActionValueNone ||
         hash_info->hash_config.function_select == bcmFieldContextHashActionValueReplaceCrc))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Hash config: CRC Select must not be specified when not selecting an augment function\n");
    }

    for (hash_config_iter = 0; hash_config_iter < hash_info->nof_additional_hash_config; hash_config_iter++)
    {
        if (hash_info->additional_hash_config[hash_config_iter].crc_select == bcmFieldCrcSelectInvalid &&
            (hash_info->additional_hash_config[hash_config_iter].function_select ==
             bcmFieldContextHashActionValueAugmentCrc
             || hash_info->additional_hash_config[hash_config_iter].function_select ==
             bcmFieldContextHashActionValueAugmentKey))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Hash config: CRC Select must be specified when selecting an augment function at index %d\n",
                         hash_config_iter);
        }

        if (hash_info->additional_hash_config[hash_config_iter].crc_select != bcmFieldCrcSelectInvalid &&
            (hash_info->additional_hash_config[hash_config_iter].function_select == bcmFieldContextHashActionValueNone
             || hash_info->additional_hash_config[hash_config_iter].function_select ==
             bcmFieldContextHashActionValueReplaceCrc))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Hash config: CRC Select must not be specified when not selecting an augment function at index %d\n",
                         hash_config_iter);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create, allocate all resources and configure
 *  HASH key which is used to create CRC.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the HASH should be created.
 * \param [in] hash_info -
 *  Pointer to struct of HASH info (see bcm_field_context_hash_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_hash_info_t_init
 */
int
bcm_dnx_field_context_hash_create(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_context_hash_info_t * hash_info)
{
    dnx_field_context_hash_flags_e dnx_hash_flags;
    dnx_field_context_hash_info_t dnx_hash_info;
    dnx_field_stage_e dnx_field_stage;
    int qual_iter, config_iter;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validate input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_field_context_hash_create_verify(unit, flags, stage, context_id, hash_info));

    /** Fill in DNX level struct */
    SHR_IF_ERR_EXIT(dnx_field_context_hash_info_t_init(unit, &dnx_hash_info));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    dnx_hash_flags = 0;
    if (flags & BCM_FIELD_FLAG_UPDATE)
    {
        dnx_hash_flags |= DNX_FIELD_CONTEXT_HASH_FLAGS_UPDATE;
    }

    /** Iterate over all input qualifiers, map them one by one to DNX qualifiers and their all info related */
    for (qual_iter = 0; qual_iter < hash_info->key_info.nof_quals; qual_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_field_stage, hash_info->key_info.qual_types[qual_iter],
                                                      &(dnx_hash_info.key_info.dnx_quals[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_bcm_to_dnx
                        (unit, hash_info->key_info.qual_info[qual_iter].input_type,
                         &(dnx_hash_info.key_info.qual_info[qual_iter].input_type)));
        dnx_hash_info.key_info.qual_info[qual_iter].input_arg = hash_info->key_info.qual_info[qual_iter].input_arg;
        dnx_hash_info.key_info.qual_info[qual_iter].offset = hash_info->key_info.qual_info[qual_iter].offset;
    }

    /** Compression and Order are TRUE/FALSE parameters, which enable or disable these functions */
    dnx_hash_info.compression = hash_info->compression;
    dnx_hash_info.order = hash_info->order;

    /** Map the BCM Hashing Function to DNX(DBAL) level for the DNX API */
    SHR_IF_ERR_EXIT(dnx_field_map_hash_func_bcm_to_dbal(unit,
                                                        hash_info->hash_function, &(dnx_hash_info.hash_function)));

    /** Map the BCM Hashing LB key to DNX(DBAL) level for the DNX API */
    SHR_IF_ERR_EXIT(dnx_field_map_hash_lb_key_bcm_to_dbal(unit,
                                                          hash_info->hash_config.action_key,
                                                          &(dnx_hash_info.hash_config[0].action_key)));

    /** Map the BCM Hash Action to DNX(DBAL) level for the DNX API */
    SHR_IF_ERR_EXIT(dnx_field_map_hash_action_bcm_to_dbal(unit,
                                                          hash_info->hash_config.function_select,
                                                          &(dnx_hash_info.hash_config[0].hash_action)));

    /** Map the BCM CRC select to DNX(DBAL) level for the DNX API */
    if (hash_info->hash_config.crc_select != bcmFieldCrcSelectInvalid)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_crc_select_bcm_to_dbal(unit,
                                                             hash_info->hash_config.crc_select,
                                                             &(dnx_hash_info.hash_config[0].crc_select)));
    }

    /** Map the BCM Additional Hash config to DNX(DBAL) level for the DNX API */
    for (config_iter = 0; config_iter < hash_info->nof_additional_hash_config; config_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_hash_lb_key_bcm_to_dbal
                        (unit, hash_info->additional_hash_config[config_iter].action_key,
                         &(dnx_hash_info.hash_config[config_iter + 1].action_key)));

        if (hash_info->additional_hash_config[config_iter].crc_select != bcmFieldCrcSelectInvalid)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_crc_select_bcm_to_dbal
                            (unit, hash_info->additional_hash_config[config_iter].crc_select,
                             &(dnx_hash_info.hash_config[config_iter + 1].crc_select)));
        }
        SHR_IF_ERR_EXIT(dnx_field_map_hash_action_bcm_to_dbal
                        (unit, hash_info->additional_hash_config[config_iter].function_select,
                         &(dnx_hash_info.hash_config[config_iter + 1].hash_action)));
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_hash_create(unit, dnx_hash_flags, dnx_field_stage, context_id, &dnx_hash_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to verify input params of bcm_field_context_hash_info_get().
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id from which the HASH should be retrieved.
 * \param [in] hash_info -
 *  Pointer to struct of HASH info (see bcm_field_context_hash_info_t in field.h)
 *  In case you want to retrieve information for one of the various lb-keys, which are iPMF1 output,
 *  the hash_info->hash_config.action_key, must be specified. Otherwise no info will be returned.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_hash_info_t_init
 */
static int
dnx_field_context_hash_info_get_verify(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_context_hash_info_t * hash_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(hash_info, _SHR_E_PARAM, "hash_info");

    /** The HASH feature is available only on IPMF1 stage! */
    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Hash' can only be configured on bcmFieldStageIngressPMF1.\r\n");
    }

    if (hash_info->hash_config.action_key < bcmFieldContextHashActionKeyFirst
        || hash_info->hash_config.action_key > bcmFieldContextHashActionKeyCount)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Selected action key (%d) is not supported.\r\n", hash_info->hash_config.action_key);
    }

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id can't exceed maximum number %d of Context IDs allow per field stage\n",
                     dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to retrieve, all configured resources about
 *  HASH key which is used to create CRC.
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id from which the HASH should be retrieved.
 * \param [in,out] hash_info -
 *  Pointer to struct of HASH info (see bcm_field_context_hash_info_t in field.h)
 *  In case you want to retrieve information for one of the various lb-keys, which are iPMF1 output,
 *  the hash_info->hash_config.action_key, must be specified, otherwise the first key will be stored.
 *  The information of the rest lb-keys will be stored in additional_hash_config.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_hash_info_t_init
 */
int
bcm_dnx_field_context_hash_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    bcm_field_context_hash_info_t * hash_info)
{
    dnx_field_context_hash_info_t dnx_hash_info;
    dnx_field_stage_e dnx_field_stage;
    bcm_field_context_hash_action_key_t bcm_action_key, requested_action_key;
    int qual_iter;
    int config_iter = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validate input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_field_context_hash_info_get_verify(unit, stage, context_id, hash_info));

    /** Fill in DNX level struct */
    SHR_IF_ERR_EXIT(dnx_field_context_hash_info_t_init(unit, &dnx_hash_info));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_hash_info_get(unit, dnx_field_stage, context_id, &dnx_hash_info));

    requested_action_key = hash_info->hash_config.action_key;
    bcm_field_context_hash_info_t_init(hash_info);

    /** Iterate over all input qualifiers, map them one by one to DNX qualifiers and their all info related */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         dnx_hash_info.key_info.dnx_quals[qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        if (qual_iter >= BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Returned DNX qualifiers are exceeding maximum number %d of BCM qualifiers!\n",
                         BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_hash_info.key_info.dnx_quals[qual_iter],
                         &(hash_info->key_info.qual_types[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm
                        (unit, dnx_hash_info.key_info.qual_info[qual_iter].input_type,
                         &(hash_info->key_info.qual_info[qual_iter].input_type)));
        hash_info->key_info.qual_info[qual_iter].input_arg = dnx_hash_info.key_info.qual_info[qual_iter].input_arg;
        hash_info->key_info.qual_info[qual_iter].offset = dnx_hash_info.key_info.qual_info[qual_iter].offset;
    }

    hash_info->key_info.nof_quals = qual_iter;

    /** Compression and Order are TRUE/FALSE parameters, which enable or disable these functions */
    hash_info->compression = dnx_hash_info.compression;
    hash_info->order = dnx_hash_info.order;

    hash_info->nof_additional_hash_config = 0;

    /**
     * for backward compatibility, if the user specified the action key, the relevant data will be updated in hash config,
     * otherwise, hash_config will get the first action key and the rest will be stored in additional_hash_config
     */
    if (requested_action_key == bcmFieldContextHashActionKeyCount)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_hash_lb_key_dbal_to_bcm(unit,
                                                              dnx_hash_info.hash_config[config_iter].action_key,
                                                              &(hash_info->hash_config.action_key)));

        SHR_IF_ERR_EXIT(dnx_field_map_crc_select_dbal_to_bcm(unit,
                                                             dnx_hash_info.hash_config[config_iter].crc_select,
                                                             &(hash_info->hash_config.crc_select)));

        SHR_IF_ERR_EXIT(dnx_field_map_hash_action_dbal_to_bcm(unit,
                                                              dnx_hash_info.hash_config[config_iter].hash_action,
                                                              &(hash_info->hash_config.function_select)));
        config_iter++;
    }
    for (; config_iter < DNX_DATA_MAX_FIELD_CONTEXT_NOF_HASH_KEYS &&
         dnx_hash_info.hash_config[config_iter].action_key != DBAL_NOF_ENUM_FIELD_HASH_LB_KEY_VALUES; config_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_hash_lb_key_dbal_to_bcm(unit,
                                                              dnx_hash_info.hash_config[config_iter].action_key,
                                                              &bcm_action_key));

        /**
         * if the user specified the action key, the relevant data will be updated in hash config.
         */
        if (requested_action_key == bcm_action_key)
        {
            hash_info->hash_config.action_key = requested_action_key;
            SHR_IF_ERR_EXIT(dnx_field_map_crc_select_dbal_to_bcm(unit,
                                                                 dnx_hash_info.hash_config[config_iter].crc_select,
                                                                 &(hash_info->hash_config.crc_select)));

            SHR_IF_ERR_EXIT(dnx_field_map_hash_action_dbal_to_bcm(unit,
                                                                  dnx_hash_info.hash_config[config_iter].hash_action,
                                                                  &(hash_info->hash_config.function_select)));
        }
        else
        {
            hash_info->additional_hash_config[hash_info->nof_additional_hash_config].action_key = bcm_action_key;

            SHR_IF_ERR_EXIT(dnx_field_map_crc_select_dbal_to_bcm
                            (unit, dnx_hash_info.hash_config[config_iter].crc_select,
                             &(hash_info->additional_hash_config[hash_info->nof_additional_hash_config].crc_select)));

            SHR_IF_ERR_EXIT(dnx_field_map_hash_action_dbal_to_bcm
                            (unit, dnx_hash_info.hash_config[config_iter].hash_action,
                             &(hash_info->
                               additional_hash_config[hash_info->nof_additional_hash_config].function_select)));

            hash_info->nof_additional_hash_config++;
        }
    }
    if (hash_info->hash_config.function_select == bcmFieldContextHashActionValueNone)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Selected action key (%d) is not configured for context %d.\r\n",
                     hash_info->hash_config.action_key, context_id);
    }

    /** Map the DNX(DBAL) Hashing Function to BCM level. */
    SHR_IF_ERR_EXIT(dnx_field_map_hash_func_dbal_to_bcm(unit,
                                                        dnx_hash_info.hash_function, &(hash_info->hash_function)));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies input parameters of bcm_field_context_hash_destroy().
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the HASH was created.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_dnx_field_context_hash_create
 */
static int
dnx_field_context_hash_destroy_verify(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** The HASH feature is available only on IPMF1 stage! */
    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Hash' can only be configured on bcmFieldStageIngressPMF1.\r\n");
    }

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id can't exceed maximum number %d of Context IDs allow per field stage\n",
                     dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroy HASH key which is used to create CRC, deallocate all resources
 *  allocated by bcm_dnx_field_context_hash_create.
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the HASH was created.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_dnx_field_context_hash_create
 */
int
bcm_dnx_field_context_hash_destroy(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    dnx_field_context_hash_flags_e dnx_hash_flags;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Assign it to "0" cause it is currently not in use */
    dnx_hash_flags = 0;

    SHR_INVOKE_VERIFY_DNX(dnx_field_context_hash_destroy_verify(unit, stage, context_id));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_hash_destroy(unit, dnx_hash_flags, dnx_field_stage, context_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to verify input parameters of bcm_field_context_compare_create().
 *  for a given context.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the comparison should be done.
 * \param [in] pair_id -
 *  Up to two pairs can be create and used for comparison(Valid values are 0 and 1).
 *  Select the pair of keys to be compared, either First Pair or Second Pair
 * \param [in] cmp_info -
 *  Pointer to struct of Compare info (see bcm_field_context_compare_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_compare_info_t_init
 */
static int
dnx_field_context_compare_create_verify(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 pair_id,
    bcm_field_context_compare_info_t * cmp_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    /** The Compare feature is available only on IPMF1 stage! */
    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Compare' can only be configured on bcmFieldStageIngressPMF1.\r\n");
    }

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id can't exceed maximum number %d of Context IDs allow per field stage\n",
                     dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

    /**
     * Up to two Compare Pairs can be created and used for comparison.
     * Their values must be "0" or "1".
     */
    DNX_FIELD_COMPARE_PAIR_VERIFY(unit, pair_id);

    /** Number of qualifiers cann't exceed maximum number of qualifiers allow per group. */
    if (cmp_info->first_key_info.nof_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of qualifiers for the First Key cann't exceed maximum number %d of qualifiers allowed per group\n",
                     BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
    }

    /** Number of qualifiers cann't exceed maximum number of qualifiers allow per group. */
    if (cmp_info->second_key_info.nof_quals > BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of qualifiers for the Second Key cann't exceed maximum number %d of qualifiers allowed per group\n",
                     BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to set up a pair of keys for comparison
 *  for a given context.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the comparison should be done.
 * \param [in] pair_id -
 *  Up to two pairs can be create and used for comparison(Valid values are 1 and 2).
 *  Select the pair of keys to be compared, either First Pair or Second Pair
 * \param [in] cmp_info -
 *  Pointer to struct of Compare info (see bcm_field_context_compare_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_compare_info_t_init
 */
int
bcm_dnx_field_context_compare_create(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 pair_id,
    bcm_field_context_compare_info_t * cmp_info)
{
    dnx_field_stage_e dnx_field_stage;
    dnx_field_key_compare_pair_info_t dnx_pair_info;
    int qual_iter;
    dnx_field_context_compare_pair_e dnx_pair_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_context_compare_create_verify(unit, flags, stage, context_id, pair_id, cmp_info));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    SHR_IF_ERR_EXIT(dnx_field_context_compare_pair_info_t_init(unit, &dnx_pair_info));

    /** Iterate over all input qualifiers, map them one by one to DNX qualifiers and their all info related. */
    /** The qualifier mapping is done for both keys. */
    for (qual_iter = 0; qual_iter < cmp_info->first_key_info.nof_quals; qual_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                        (unit, dnx_field_stage, cmp_info->first_key_info.qual_types[qual_iter],
                         &(dnx_pair_info.first_key_info.dnx_quals[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_bcm_to_dnx
                        (unit, cmp_info->first_key_info.qual_info[qual_iter].input_type,
                         &(dnx_pair_info.first_key_info.qual_info[qual_iter].input_type)));
        dnx_pair_info.first_key_info.qual_info[qual_iter].input_arg =
            cmp_info->first_key_info.qual_info[qual_iter].input_arg;
        dnx_pair_info.first_key_info.qual_info[qual_iter].offset = cmp_info->first_key_info.qual_info[qual_iter].offset;
    }

    for (qual_iter = 0; qual_iter < cmp_info->second_key_info.nof_quals; qual_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx
                        (unit, dnx_field_stage, cmp_info->second_key_info.qual_types[qual_iter],
                         &(dnx_pair_info.second_key_info.dnx_quals[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_bcm_to_dnx
                        (unit, cmp_info->second_key_info.qual_info[qual_iter].input_type,
                         &(dnx_pair_info.second_key_info.qual_info[qual_iter].input_type)));
        dnx_pair_info.second_key_info.qual_info[qual_iter].input_arg =
            cmp_info->second_key_info.qual_info[qual_iter].input_arg;
        dnx_pair_info.second_key_info.qual_info[qual_iter].offset =
            cmp_info->second_key_info.qual_info[qual_iter].offset;
    }

    if (pair_id == 0)
    {
        dnx_pair_id = DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR;
    }
    else
    {
        dnx_pair_id = DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_compare_create
                    (unit, flags, dnx_field_stage, context_id, dnx_pair_id, &dnx_pair_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to verify input parameters of bcm_field_context_compare_info_get();
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id for which the comparison info should be retrieved.
 * \param [in] pair_id -
 *  Info for up to two pairs can be retrieved (Valid values are 0 and 1).
 *  Select the pair of keys for which info will be returned, either First Pair or Second Pair
 * \param [in] cmp_info -
 *  Pointer to struct of Compare info (see bcm_field_context_compare_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_compare_info_t_init
 */
static int
dnx_field_context_compare_info_get_verify(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 pair_id,
    bcm_field_context_compare_info_t * cmp_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** The Compare feature is available only on IPMF1 stage! */
    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Compare' can only be configured on bcmFieldStageIngressPMF1.\r\n");
    }

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id can't exceed maximum number %d of Context IDs allow per field stage\n",
                     dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

    /**
     * Up to two Compare Pairs can be created and used for comparison.
     * Their values must be "0" or "1".
     */
    DNX_FIELD_COMPARE_PAIR_VERIFY(unit, pair_id);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to retrieve info about pair of keys for
 *  used for comparison for a given context.
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id for which the comparison info should be retrieved.
 * \param [in] pair_id -
 *  Info for up to two pairs can be retrieved (Valid values are 1 and 2).
 *  Select the pair of keys for which info will be returned, either First Pair or Second Pair
 * \param [out] cmp_info -
 *  Pointer to struct of Compare info (see bcm_field_context_compare_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_context_compare_info_t_init
 */
int
bcm_dnx_field_context_compare_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 pair_id,
    bcm_field_context_compare_info_t * cmp_info)
{
    dnx_field_stage_e dnx_field_stage;
    dnx_field_key_compare_pair_info_t dnx_pair_info;
    int qual_iter;
    dnx_field_context_compare_pair_e dnx_pair_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_context_compare_info_get_verify(unit, stage, context_id, pair_id, cmp_info));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    SHR_IF_ERR_EXIT(dnx_field_context_compare_pair_info_t_init(unit, &dnx_pair_info));

    if (pair_id == 0)
    {
        dnx_pair_id = DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR;
    }
    else
    {
        dnx_pair_id = DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_compare_info_get(unit, dnx_field_stage, context_id, dnx_pair_id, &dnx_pair_info));

    /** Iterate over all input qualifiers, map them one by one to DNX qualifiers and their all info related. */
    /** The qualifier mapping is done for both keys. */
    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         dnx_pair_info.first_key_info.dnx_quals[qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        if (qual_iter >= BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Returned DNX qualifiers are exceeding maximum number %d of BCM qualifiers!\n",
                         BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
        }

        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_pair_info.first_key_info.dnx_quals[qual_iter],
                         &(cmp_info->first_key_info.qual_types[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm
                        (unit, dnx_pair_info.first_key_info.qual_info[qual_iter].input_type,
                         &(cmp_info->first_key_info.qual_info[qual_iter].input_type)));
        cmp_info->first_key_info.qual_info[qual_iter].input_arg =
            dnx_pair_info.first_key_info.qual_info[qual_iter].input_arg;
        cmp_info->first_key_info.qual_info[qual_iter].offset = dnx_pair_info.first_key_info.qual_info[qual_iter].offset;
    }

    cmp_info->first_key_info.nof_quals = qual_iter;

    for (qual_iter = 0; qual_iter < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         dnx_pair_info.second_key_info.dnx_quals[qual_iter] != DNX_FIELD_QUAL_TYPE_INVALID; qual_iter++)
    {
        if (qual_iter >= BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Returned DNX qualifiers are exceeding maximum number %d of BCM qualifiers!\n",
                         BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP);
        }

        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_pair_info.second_key_info.dnx_quals[qual_iter],
                         &(cmp_info->second_key_info.qual_types[qual_iter])));
        SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm
                        (unit, dnx_pair_info.second_key_info.qual_info[qual_iter].input_type,
                         &(cmp_info->second_key_info.qual_info[qual_iter].input_type)));
        cmp_info->second_key_info.qual_info[qual_iter].input_arg =
            dnx_pair_info.second_key_info.qual_info[qual_iter].input_arg;
        cmp_info->second_key_info.qual_info[qual_iter].offset =
            dnx_pair_info.second_key_info.qual_info[qual_iter].offset;
    }

    cmp_info->second_key_info.nof_quals = qual_iter;

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies input parameters of bcm_field_context_compare_destroy().
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the comparison was done.
 * \param [in] pair_id -
 *  User created pair ID to be destroyed.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_dnx_field_context_compare_create
 */
static int
dnx_field_context_compare_destroy_verify(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 pair_id)
{
    SHR_FUNC_INIT_VARS(unit);
    /**
     * Up to two Compare Pairs can be created and used for comparison.
     * Their values must be "0" or "1".
     */
    DNX_FIELD_COMPARE_PAIR_VERIFY(unit, pair_id);

    /** The Compare feature is available only on IPMF1 stage! */
    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'Compare' can only be configured on bcmFieldStageIngressPMF1.\r\n");
    }

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id can't exceed maximum number %d of Context IDs allow per field stage\n",
                     dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Destroys an existing Context Compare Pair.
 * \param [in] unit -
 *  Device ID
 * \param [in] stage -
 *  Field stage on which the context id has bean allocated.
 * \param [in] context_id -
 *  Context id on which the comparison was done.
 * \param [in] pair_id -
 *  User created pair ID to be destroyed.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_dnx_field_context_compare_create
 */
int
bcm_dnx_field_context_compare_destroy(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 pair_id)
{
    uint32 dnx_cmp_flags;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_context_compare_pair_e dnx_pair_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Assign it to "0" cause it is currently not in use. */
    dnx_cmp_flags = 0;

    SHR_INVOKE_VERIFY_DNX(dnx_field_context_compare_destroy_verify(unit, stage, context_id, pair_id));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    if (pair_id == 0)
    {
        dnx_pair_id = DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR;
    }
    else
    {
        dnx_pair_id = DNX_FIELD_CONTEXT_COMPARE_SECOND_PAIR;
    }

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_context_compare_destroy(unit, dnx_cmp_flags, dnx_field_stage, context_id, dnx_pair_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Evacuates the given fg_id OR pp_app_id from the bank IDs that are given in
 * evac_info struct. The evacuation process involves moving the entries
 * of the given fg_id from the targeted banks to other banks allocated by
 * the fg_id, if not enough space is available for moving the entries,
 * this function fails.
 * If user sets both FG-ID and PP-APP-ID, function will fail.
 *
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  TBD
 * \param [in] evac_info -
 *  Evacuation information (includes FG-ID/PP-APP-ID and bank IDs to evacuate FG/PP-APP-ID from).
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_dnx_field_group_add
 */
int
bcm_dnx_field_tcam_bank_evacuate(
    int unit,
    uint32 flags,
    bcm_field_tcam_bank_info_t * evac_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(evac_info, _SHR_E_PARAM, "evac_info");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    if (evac_info->fg_id != BCM_FIELD_ID_INVALID && evac_info->pp_app != bcmFieldAppDbInvalid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only one of FG ID or PP-APP-DB can be set, but not both\n");
    }

    if (evac_info->fg_id == BCM_FIELD_ID_INVALID && evac_info->pp_app == bcmFieldAppDbInvalid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "No FG-ID nor PP-APP-DB are given, one of them need to be valid to evacuate from the given banks\n");
    }

    if (evac_info->fg_id != BCM_FIELD_ID_INVALID)
    {
        dnx_field_group_t dnx_fg_id = (dnx_field_group_t) evac_info->fg_id;

        SHR_IF_ERR_EXIT(dnx_field_group_tcam_bank_evacuate
                        (unit, dnx_fg_id, evac_info->nof_banks, evac_info->tcam_bank_ids));
    }
    else        /* PP-APP-DB */
    {
        dbal_tables_e dbal_table;
        uint32 tcam_handler_id;
        /*
         * Retrieve the PP APP's DBAL table ID, then retrieve the TCAM handler ID from the table, afterwards evacuate
         * the returned TCAM handler ID from the given banks.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_pp_app_dbal_table_get(unit, evac_info->pp_app, &dbal_table));
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table, &tcam_handler_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_evacuate
                        (unit, tcam_handler_id, evac_info->nof_banks, evac_info->tcam_bank_ids));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add the given bank IDs to the given FG-ID OR PP-APP-ID that are given in
 * add_info struct.
 * If user sets both FG-ID and PP-APP-ID, function will fail.
 *
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  TBD
 * \param [in] add_info -
 *  Add information (includes FG-ID/PP-APP-ID and bank IDs to add FG/PP-APP-ID to).
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *   Reference to :
 *   - bcm_dnx_field_group_add
 */
int
bcm_dnx_field_tcam_bank_add(
    int unit,
    uint32 flags,
    bcm_field_tcam_bank_info_t * add_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validate input parameters */
    SHR_NULL_CHECK(add_info, _SHR_E_PARAM, "add_info");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    if (add_info->fg_id != BCM_FIELD_ID_INVALID && add_info->pp_app != bcmFieldAppDbInvalid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only one of FG ID or PP-APP-DB can be set, but not both\n");
    }

    if (add_info->fg_id == BCM_FIELD_ID_INVALID && add_info->pp_app == bcmFieldAppDbInvalid)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "No FG-ID nor PP-APP-DB are given, one of them need to be valid to add to the banks\n");
    }

    if (add_info->fg_id != BCM_FIELD_ID_INVALID)
    {
        dnx_field_group_t dnx_fg_id = (dnx_field_group_t) add_info->fg_id;

        SHR_IF_ERR_EXIT(dnx_field_group_tcam_bank_add(unit, dnx_fg_id, add_info->nof_banks, add_info->tcam_bank_ids));
    }
    else        /* PP-APP-DB */
    {
        dbal_tables_e dbal_table;
        uint32 tcam_handler_id;
        /*
         * Retrieve the PP APP's DBAL table ID, then retrieve the TCAM handler ID from the table, afterwards add the
         * returned TCAM handler ID to the given banks.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_pp_app_dbal_table_get(unit, add_info->pp_app, &dbal_table));
        SHR_IF_ERR_EXIT(dbal_tables_tcam_handler_id_get(unit, dbal_table, &tcam_handler_id));
        SHR_IF_ERR_EXIT(dnx_field_tcam_handler_bank_add
                        (unit, tcam_handler_id, add_info->nof_banks, add_info->tcam_bank_ids));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies input parameters of bcm_field_ace_format_add().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags - BCM_FIELD_FLAG_WITH_ID - to add an ACE Format with ID.
 *  In case this flag is set 'ace_format_id' considered as input otherwise as output
 * \param [in] ace_format_info -
 *  Pointer to struct of ACE Format info (see bcm_field_ace_format_info_t in field.h)
 * \param [in] ace_format_id -
 *  Pointer to the created ACE Format.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_format_info_t_init
 *  - bcm_dnx_field_ace_format_add
 */
static int
dnx_field_ace_format_add_verify(
    int unit,
    uint32 flags,
    bcm_field_ace_format_info_t * ace_format_info,
    bcm_field_ace_format_t * ace_format_id)
{
    uint32 unsupported_flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ace_format_info, _SHR_E_PARAM, "ace_format_info");
    SHR_NULL_CHECK(ace_format_id, _SHR_E_PARAM, "ace_format_id");

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_UPDATE))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    /** Validate input parameters*/
    /** Number of actions can not exceed maximum number of actions allow per ace format*/
    if (ace_format_info->nof_actions > BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of actions cannot exceed maximum number of actions allowed per group\n");
    }

    if (flags & BCM_FIELD_FLAG_WITH_ID)
    {
        /** ACE Format Id can't exceed maximum number of ACE IDs. */
        if (*ace_format_id == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ACE Format Id '0' is reserved and can't be used! Please use ACE Format IDs between 1-%d! \n",
                         ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
        }
        else if (*ace_format_id >= dnx_data_field.ace.nof_ace_id_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ACE Format Id can't exceed maximum number %d of ACE IDs\n",
                         ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
        }
        /** ACE Format ID used for ACE (PMF extension) has limited allocated IDs.*/
        if (((flags & BCM_FIELD_FLAG_WITH_ID) || (flags & BCM_FIELD_FLAG_UPDATE)) &&
            (DNX_FIELD_ACE_FORMAT_ID_IS_ACE(unit, (*ace_format_id))) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "ACE Format Id used for ACE purposes must be between %d and %d.\n",
                         dnx_data_field.ace.ace_id_pmf_alloc_first_get(unit),
                         dnx_data_field.ace.ace_id_pmf_alloc_last_get(unit));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the information about a specific ACE format.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags - BCM_FIELD_FLAG_WITH_ID - to add an ACE Format with ID.
 *  In case this flag is set 'ace_format_id' considered as input otherwise as output
 * \param [in] ace_format_info -
 *  Pointer to struct of ACE Format info (see bcm_field_ace_format_info_t in field.h)
 * \param [in] ace_format_id -
 *  Pointer to the created ACE Format.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_format_info_t_init
 *  - bcm_dnx_field_ace_format_add
 */
int
bcm_dnx_field_ace_format_add(
    int unit,
    uint32 flags,
    bcm_field_ace_format_info_t * ace_format_info,
    bcm_field_ace_format_t * ace_format_id)
{
    int action_iter;
    dnx_field_ace_format_add_flags_e dnx_flags;
    dnx_field_ace_format_info_t dnx_ace_info;
    dnx_field_action_t dnx_action;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    dnx_flags = 0;

    SHR_NULL_CHECK(ace_format_info, _SHR_E_PARAM, "ace_format_info");
    SHR_NULL_CHECK(ace_format_id, _SHR_E_PARAM, "ace_format_id");

    /** ACE Format feature is available only on ACE Stage (ACL Extension over EPMF). */
    dnx_field_stage = DNX_FIELD_STAGE_ACE;

    /** Validate input parameters. */
    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_format_add_verify(unit, flags, ace_format_info, ace_format_id));

    /**Fill in DNX level structs. */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_t_init(unit, &dnx_ace_info));
    if (flags & BCM_FIELD_FLAG_WITH_ID)
    {
        dnx_flags |= DNX_FIELD_ACE_FORMAT_ADD_FLAG_WITH_ID;
    }

    /** For each action array  map each BCM action  to  DNX action(s)*/
    for (action_iter = 0; action_iter < ace_format_info->nof_actions; action_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                        (unit, dnx_field_stage, ace_format_info->action_types[action_iter], &dnx_action));
        /**
         * Note that unlike field group, we only have a one dimensional array of actions to fill.
         */
        dnx_ace_info.dnx_actions[action_iter] = dnx_action;
        dnx_ace_info.use_valid_bit[action_iter] =
            (ace_format_info->action_with_valid_bit[action_iter]) ? (TRUE) : (FALSE);
    }

    sal_strncpy_s((dnx_ace_info.name), (char *) ace_format_info->name, sizeof(dnx_ace_info.name));

    /**Call DNX-level API*/
    SHR_IF_ERR_EXIT(dnx_field_ace_format_add(unit, dnx_flags, &dnx_ace_info, ace_format_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verifies input parameters of bcm_field_ace_format_info_get().
 * \param [in] unit -
 *  Device ID
 * \param [in] ace_format_id -
 *  ACE Format ID for which we retrieve all relevant information.
 * \param [in] ace_format_info -
 *  Pointer to struct of ACE Format info (see bcm_field_ace_format_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_format_info_t_init
 *  - bcm_dnx_field_ace_format_add
 */
static int
dnx_field_ace_format_info_get_verify(
    int unit,
    bcm_field_ace_format_t ace_format_id,
    bcm_field_ace_format_info_t * ace_format_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ace_format_info, _SHR_E_PARAM, "ace_format_info");

    /** ACE Format Id can't exceed maximum number of ACE IDs. */
    if (ace_format_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ACE Format Id '0' is reserved and can't be used! Please use ACE Format IDs between 1-%d! \n",
                     ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
    }
    else if (ace_format_id >= dnx_data_field.ace.nof_ace_id_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ACE Format Id can't exceed maximum number %d of ACE IDs\n",
                     ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the information about a specific ACE format.
 * \param [in] unit -
 *  Device ID
 * \param [in] ace_format_id -
 *  ACE Format ID for which we retrieve all relevant information.
 * \param [out] ace_format_info -
 *  Pointer to struct of ACE Format info (see bcm_field_ace_format_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_format_info_t_init
 *  - bcm_dnx_field_ace_format_add
 */
int
bcm_dnx_field_ace_format_info_get(
    int unit,
    bcm_field_ace_format_t ace_format_id,
    bcm_field_ace_format_info_t * ace_format_info)
{
    dnx_field_ace_format_full_info_t dnx_ace_full_info;
    dnx_field_stage_e dnx_field_stage;
    int bcm_cur_action_iter, bcm_action_iter, dnx_action_iter, action_type_exist;
    bcm_field_action_t bcm_action;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_format_info_get_verify(unit, ace_format_id, ace_format_info));

    /** ACE Format feature is available only on ACE Stage (ACL Extension over EPMF). */
    dnx_field_stage = DNX_FIELD_STAGE_ACE;

    /** Initialize the DNX structure and perform get operation. */
    SHR_IF_ERR_EXIT(dnx_field_ace_format_info_t_init(unit, &(dnx_ace_full_info.format_basic_info)));
    SHR_IF_ERR_EXIT(dnx_field_ace_format_get(unit, ace_format_id, &dnx_ace_full_info));

    /** Iterate on output action array and fill its info */
    bcm_action_iter = 0;

    /**
     * Each BCM action can be constructed them more then one DNX action, fill in the DNX info struct accordingly.
     * Last element in DNX action array must be DNX_FIELD_ACTION_INVALID.
     * We are checking, if we reach this value it is the last action in the array.
     */
    for (dnx_action_iter = 0; dnx_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_ace_full_info.format_basic_info.dnx_actions[dnx_action_iter] != DNX_FIELD_ACTION_INVALID;
         dnx_action_iter++)
    {
        /** For each action array map each DNX action to BCM action. */
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, dnx_field_stage,
                                                        dnx_ace_full_info.
                                                        format_basic_info.dnx_actions[dnx_action_iter], &(bcm_action)));

        for (bcm_cur_action_iter = 0; bcm_cur_action_iter < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP;
             bcm_cur_action_iter++)
        {
            /**
             * Check if "bcm_action" is already written to the action array,
             * indicate that action_type_exist is TRUE and leave the loop.
             * If "bcm_action" not exist set action_type_exist to FALSE.
             */
            if (ace_format_info->action_types[bcm_cur_action_iter] == bcm_action)
            {
                action_type_exist = TRUE;
                break;
            }
            action_type_exist = FALSE;
        }
        /**
         * Check if the action_type_exist is FALSE, then fill the output
         * action array info.
         */
        if (action_type_exist == FALSE)
        {
            ace_format_info->action_types[bcm_action_iter] = bcm_action;
            bcm_action_iter++;
        }
    }

    sal_strncpy_s((char *) (ace_format_info->name), dnx_ace_full_info.format_basic_info.name,
                  sizeof(ace_format_info->name));

    /** Fill the number of actions in bcm_field_ace_format_info_t. */
    ace_format_info->nof_actions = bcm_action_iter;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function verifies input parameters of bcm_field_ace_format_delete().
* \param [in] unit   - Device Id
* \param [in] ace_format_id - ACE Format Id to delete
* \return
*  int - Error Type, in sense of bcm_error_t, same as
*  shr_error_t
* \remark
*  None
* \see
*  Reference to:
*  - bcm_dnx_field_ace_format_add
*  - bcm_dnx_field_ace_format_info_get
*/
static int
dnx_field_ace_format_delete_verify(
    int unit,
    bcm_field_ace_format_t ace_format_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /** ACE Format Id can't exceed maximum number of ACE IDs. */
    if (ace_format_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ACE Format Id '0' is reserved and can't be used! Please use ACE Format IDs between 1-%d! \n",
                     ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
    }
    else if (ace_format_id >= dnx_data_field.ace.nof_ace_id_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ACE Format Id can't exceed maximum number %d of ACE IDs\n",
                     ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function deletes an ACE format, deallocates all relevant resources.
* \param [in] unit   - Device Id
* \param [in] ace_format_id - ACE Format Id to delete
* \return
*  int - Error Type, in sense of bcm_error_t, same as
*  shr_error_t
* \remark
*  None
* \see
*  Reference to:
*  - bcm_dnx_field_ace_format_add
*  - bcm_dnx_field_ace_format_info_get
*/
int
bcm_dnx_field_ace_format_delete(
    int unit,
    bcm_field_ace_format_t ace_format_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_format_delete_verify(unit, ace_format_id));

    SHR_IF_ERR_EXIT(dnx_field_ace_format_delete(unit, ace_format_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies input parameters of bcm_field_ace_entry_add().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  BCM_FIELD_FLAG_WITH_ID - to add a context with ID.
 *  In case this flag is set 'entry_handle' considered as input otherwise as output.
 *  BCM_FIELD_FLAG_UPDATE - to updated in existing entry.
 * \param [in] ace_format_id -
 *  ACE Format id, to which entry is added.
 * \param [in] entry_info -
 *  Pointer to struct of ACE Entry info (see bcm_field_ace_entry_info_t in field.h)
 * \param [in] entry_handle -
 *  Pointer to handle-id for the added entry.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_entry_info_t_init
 *  - bcm_dnx_field_ace_entry_info_get
 */
static int
dnx_field_ace_entry_add_verify(
    int unit,
    uint32 flags,
    bcm_field_ace_format_t ace_format_id,
    bcm_field_ace_entry_info_t * entry_info,
    uint32 *entry_handle)
{
    uint32 unsupported_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(entry_info, _SHR_E_PARAM, "entry_info");
    SHR_NULL_CHECK(entry_handle, _SHR_E_PARAM, "entry_handle");

    if ((unsupported_flags = (flags & ~(BCM_FIELD_FLAG_WITH_ID | BCM_FIELD_FLAG_UPDATE))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    if ((flags & BCM_FIELD_FLAG_UPDATE) && (flags & BCM_FIELD_FLAG_WITH_ID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag UPDATE can't be used together with flag WITH_ID\n");
    }

    /** ACE Format Id can't exceed maximum number of ACE IDs. */
    if (ace_format_id == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ACE Format Id '0' is reserved and can't be used! Please use ACE Format IDs between 1-%d! \n",
                     ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
    }
    else if (ace_format_id >= dnx_data_field.ace.nof_ace_id_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "ACE Format Id can't exceed maximum number %d of ACE IDs\n",
                     ((dnx_data_field.ace.nof_ace_id_get(unit)) - 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds new entry in hardware to indicated ACE format.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  BCM_FIELD_FLAG_WITH_ID - to add a context with ID.
 *  In case this flag is set 'entry_handle' considered as input otherwise as output.
 *  BCM_FIELD_FLAG_UPDATE - to updated in existing entry.
 * \param [in] ace_format_id -
 *  ACE Format id, to which entry is added.
 * \param [in] entry_info -
 *  Pointer to struct of ACE Entry info (see bcm_field_ace_entry_info_t in field.h)
 * \param [in,out] entry_handle -
 *  Pointer to handle-id for the added entry.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_entry_info_t_init
 *  - bcm_dnx_field_ace_entry_info_get
 */
int
bcm_dnx_field_ace_entry_add(
    int unit,
    uint32 flags,
    bcm_field_ace_format_t ace_format_id,
    bcm_field_ace_entry_info_t * entry_info,
    uint32 *entry_handle)
{
    dnx_field_entry_payload_t dnx_entry_payload;
    uint32 dnx_flags;
    dnx_field_stage_e dnx_field_stage;
    int action_iter;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_ace_entry_add_verify(unit, flags, ace_format_id, entry_info, entry_handle));

    dnx_flags = 0;

    /** ACE Format feature is available only on ACE Stage (ACL Extension over EPMF). */
    dnx_field_stage = DNX_FIELD_STAGE_ACE;

    /**Fill in DNX level structs*/
    SHR_IF_ERR_EXIT(dnx_field_entry_payload_t_init(unit, &dnx_entry_payload));
    if (flags & BCM_FIELD_FLAG_WITH_ID)
    {
        dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_WITH_ID;
    }
    if (flags & BCM_FIELD_FLAG_UPDATE)
    {
        dnx_flags |= DNX_FIELD_ENTRY_ADD_FLAG_UPDATE;
    }

    /**
     * Prepare the entry payload DNX struct.
     * For each action array map each BCM action to DNX action(s).
     */
    for (action_iter = 0; action_iter < entry_info->nof_entry_actions; action_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_action_info_bcm_to_dnx(unit, BCM_CORE_ALL, dnx_field_stage,
                                                             &(entry_info->entry_action[action_iter]),
                                                             &(dnx_entry_payload.action_info[action_iter])));
    }

    /** Call DNX-level entry add API */
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_add(unit, dnx_flags, ace_format_id, &dnx_entry_payload, entry_handle));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function retrieves the entry information
 *  of a specific ACE Format ID.
 * \param [in] unit -
 *  Device ID
 * \param [in] entry_handle -
 *  Entry handle for which we retrieve all relevant information.
 * \param [out] entry_info -
 *  Pointer to struct of ACE Entry info (see bcm_field_ace_entry_info_t in field.h).
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_ace_entry_info_t_init
 *  - bcm_dnx_field_ace_entry_add
 */
int
bcm_dnx_field_ace_entry_info_get(
    int unit,
    uint32 entry_handle,
    bcm_field_ace_entry_info_t * entry_info)
{
    dnx_field_entry_payload_t dnx_entry_payload;
    dnx_field_stage_e dnx_field_stage;
    int bcm_cur_action_iter, bcm_action_iter, dnx_action_iter, action_type_exist, ii;
    bcm_field_action_t bcm_action;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(entry_info, _SHR_E_PARAM, "entry_info");

    /** ACE Format feature is available only on ACE Stage (ACL Extension over EPMF). */
    dnx_field_stage = DNX_FIELD_STAGE_ACE;

    SHR_IF_ERR_EXIT(dnx_field_entry_payload_t_init(unit, &dnx_entry_payload));

    /** Call DNX-level entry get API.*/
    SHR_IF_ERR_EXIT(dnx_field_ace_entry_get(unit, entry_handle, &dnx_entry_payload, NULL));

    /** Iterate on output action array and fill its info */
    bcm_action_iter = 0;

    /**
     * Each BCM action can be constructed them more then one DNX action, fill in the DNX info struct accordingly.
     * Last element in DNX action array must be DNX_FIELD_ACTION_INVALID.
     * We are checking, if we reach this value it is the last action in the array.
     */
    for (dnx_action_iter = 0; dnx_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_entry_payload.action_info[dnx_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID; dnx_action_iter++)
    {
        /** For each action array map each DNX action to BCM action. */
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm(unit, dnx_field_stage,
                                                        dnx_entry_payload.action_info[dnx_action_iter].dnx_action,
                                                        &(bcm_action)));

        for (bcm_cur_action_iter = 0; bcm_cur_action_iter < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP;
             bcm_cur_action_iter++)
        {
            /**
             * Check if "bcm_action" is already written to the entry action array,
             * indicate that action_type_exist is TRUE and leave the loop.
             * If "bcm_action" not exist set action_type_exist to FALSE.
             */
            if (entry_info->entry_action[bcm_cur_action_iter].type == bcm_action)
            {
                action_type_exist = TRUE;
                break;
            }
            action_type_exist = FALSE;
        }
        /**
         * Check if the action_type_exist is FALSE, then fill the output
         * action array info.
         */
        if (action_type_exist == FALSE)
        {
            entry_info->entry_action[bcm_action_iter].type = bcm_action;
            for (ii = 0; ii < BCM_FIELD_ACTION_WIDTH_IN_WORDS; ii++)
            {
                entry_info->entry_action[bcm_action_iter].value[ii] =
                    dnx_entry_payload.action_info[dnx_action_iter].action_value[ii];
            }

            bcm_action_iter++;
        }
    }

    /** Fill the number of actions in bcm_field_ace_entry_info_t */
    entry_info->nof_entry_actions = bcm_action_iter;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function deletes an ACE entry from hardware.
 * \param [in] unit -
 *  Device ID
 * \param [in] entry_handle -
 *  Handle to the created entry, which
 *  has to be deleted.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   None
 * \see
 *  Reference to:
 *    - bcm_dnx_field_ace_entry_add
 *    - bcm_dnx_field_ace_entry_info_get
 */
int
bcm_dnx_field_ace_entry_delete(
    int unit,
    uint32 entry_handle)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_field_ace_entry_delete(unit, entry_handle));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies input parameters of bcm_field_context_param_set().
 * \param [in] unit           - Device ID
 * \param [in] flags          - Currently not in use!
 * \param [in] bcm_stage      - Field stage on which the context id has bean allocated.
 * \param [in] context_id     - Context id of which the parameters should be change.
 * \param [in] context_params - Pointer to struct of Context Params info (see bcm_field_context_param_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 */
static int
dnx_field_context_param_set_verify(
    int unit,
    uint32 flags,
    bcm_field_stage_t bcm_stage,
    bcm_field_context_t context_id,
    bcm_field_context_param_info_t * context_params)
{
    dnx_field_stage_e dnx_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(context_params, _SHR_E_PARAM, "context_params");
    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flag %d is not supported.\n", flags);
    }

    DNX_FIELD_CONTEXT_IS_ALLOCATED(dnx_stage, context_id, 1);

    /** Context Id can't exceed maximum number of Context IDs allow per field stage. */
    if (context_id >= dnx_data_field.common_max_val.nof_contexts_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Context Id can't exceed maximum number %d of Context IDs allow per field stage\n",
                     dnx_data_field.common_max_val.nof_contexts_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function sets specific attribute value for a context.
 * \param [in] unit           - Device ID
 * \param [in] flags          - Currently not in use!
 * \param [in] bcm_stage      - Field stage on which the context id has bean allocated.
 * \param [in] context_id     - Context id of which the parameters should be change.
 * \param [in] context_params - Pointer to struct of Context Params info (see bcm_field_context_param_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 */
int
bcm_dnx_field_context_param_set(
    int unit,
    uint32 flags,
    bcm_field_stage_t bcm_stage,
    bcm_field_context_t context_id,
    bcm_field_context_param_info_t * context_params)
{
    dnx_field_dbal_entry_t field_dbal_entry;
    dnx_field_stage_e dnx_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_context_param_set_verify(unit, flags, bcm_stage, context_id, context_params));

    /** Convert BCM to DNX Field Stage */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    SHR_IF_ERR_EXIT(dnx_field_map_context_param_bcm_to_dnx(unit, bcm_stage, context_id, context_params,
                                                           &field_dbal_entry));

    SHR_IF_ERR_EXIT(dnx_field_dbal_entry_set(unit, &field_dbal_entry));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies input parameters of bcm_field_fem_action_add().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  Field group id, to which fem is going to be added.
 * \param [in] action_priority -
 *  Variable of type bcm_field_action_priority_t. Contains information ('array_id' and
 *  'fem_position_in_array' from which 'fem_id' can be constructed.
 * \param [in] fem_action_info -
 *  Pointer to struct of FEM action info (see bcm_field_fem_action_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  * bcm_field_fem_action_info_t_init
 *  * bcm_field_fem_action_info_get
 */
static int
dnx_field_fem_action_add_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_action_priority_t action_priority,
    bcm_field_fem_action_info_t * fem_action_info)
{
    dnx_field_group_t dnx_fg_id;
    dnx_field_fem_id_t dnx_fem_id;
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(fem_action_info, _SHR_E_PARAM, "fem_action_info");

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    if (DNX_FIELD_ACTION_PRIORITY_IS_POSITION(action_priority) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "action_priority (0x%x) must represent position, as encoded by BCM_FIELD_ACTION_POSITION().\r\n",
                     action_priority);
    }

    /*
     * Convert input (encoded) 'action_priority' to 'fem_id'.
     */
    DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(dnx_fem_id, unit, action_priority);
    /** Field Group Id can't exceed maximum number of Field Group IDs allow per unit. */
    DNX_FIELD_FG_ID_VERIFY(unit, dnx_fg_id);
    DNX_FIELD_FEM_ID_VERIFY(unit, dnx_fem_id);
    /*
     * Get the field group type.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));
    /*
     * Get the Stage on which field group was created.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));
    /*
     * The FEM tool is available only on IPMF1 and IPMF2 stages!
     */
    if (dnx_field_stage != DNX_FIELD_STAGE_IPMF1 && dnx_field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "'FEM tool' can only be configured on bcmFieldStageIngressPMF1 and bcmFieldStageIngressPMF2 stages.\r\n");
    }
    /*
     * Currently the FEM tool supports only TCAM and DIREXT field group types!
     */
    if ((fg_type != DNX_FIELD_GROUP_TYPE_TCAM) && (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        && (fg_type != DNX_FIELD_GROUP_TYPE_EXEM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "'FEM tool' supports only TCAM and DirectExtraction Field group types!\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds FEM machine as a resource to a specific field
 *  group.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  Field group id, to which fem is going to be added.
 * \param [in] action_priority -
 *  Variable of type bcm_field_action_priority_t. Contains information ('array_id' and
 *  'fem_position_in_array' from which 'fem_id' can be constructed.
 * \param [in] fem_action_info -
 *  Pointer to struct of FEM action info (see bcm_field_fem_action_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  * bcm_field_fem_action_info_t_init
 *  * bcm_field_fem_action_info_get
 */
int
bcm_dnx_field_fem_action_add(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_action_priority_t action_priority,
    bcm_field_fem_action_info_t * fem_action_info)
{
    uint8 input_size;
    dnx_field_group_t dnx_fg_id;
    dnx_field_fem_id_t dnx_fem_id;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_action_t dnx_action;
    dnx_field_group_t dnx_second_fg_id;
    dnx_field_fem_condition_entry_t *fem_condition_entry_p = NULL;
    dnx_field_action_t *fem_encoded_actions_p = NULL;
    dnx_field_fem_action_entry_t *fem_action_entry_p = NULL;

    int map_bits_per_fem_iter, extr_per_fem_iter, cond_per_fem_iter;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_fem_action_add_verify(unit, flags, fg_id, action_priority, fem_action_info));
    SHR_ALLOC(fem_condition_entry_p, sizeof(*fem_condition_entry_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                                          DNX_FIELD_STAGE_IPMF1)->nof_fem_condition,
              "fem_condition_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(fem_encoded_actions_p,
              sizeof(*fem_encoded_actions_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                   DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index,
              "fem_encoded_actions_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(fem_action_entry_p,
              sizeof(*fem_action_entry_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index,
              "fem_action_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    /** Convert input (encoded) 'action_priority' to 'fem_id'. */
    DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(dnx_fem_id, unit, action_priority);

    /** Get the Stage on which field group was created. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    /** Take the value for the input_size from the DNX_DATA. */
    input_size = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_bits_in_fem_key_select;

    /** Fill DNX FEM condition entry information. */
    for (cond_per_fem_iter = 0; cond_per_fem_iter < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; cond_per_fem_iter++)
    {
        /** Initialize DNX FEM condition entry arrays. */
        SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init(unit, &fem_condition_entry_p[cond_per_fem_iter]));

        fem_condition_entry_p[cond_per_fem_iter].fem_map_index =
            fem_action_info->fem_condition[cond_per_fem_iter].extraction_id;
        fem_condition_entry_p[cond_per_fem_iter].fem_action_valid =
            fem_action_info->fem_condition[cond_per_fem_iter].is_action_valid;
    }

    for (extr_per_fem_iter = 0; extr_per_fem_iter < BCM_FIELD_NUMBER_OF_EXTRACTIONS_PER_FEM; extr_per_fem_iter++)
    {
        /** Initialize DNX FEM action entry arrays. */
        SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry_p[extr_per_fem_iter]));
        fem_encoded_actions_p[extr_per_fem_iter] = DNX_FIELD_ACTION_INVALID;
    }

    /** Fill DNX FEM action entry information. */
    for (extr_per_fem_iter = 0; extr_per_fem_iter < BCM_FIELD_NUMBER_OF_EXTRACTIONS_PER_FEM &&
         fem_action_info->fem_extraction[extr_per_fem_iter].action_type != bcmFieldActionCount; extr_per_fem_iter++)
    {
        fem_action_entry_p[extr_per_fem_iter].fem_adder = fem_action_info->fem_extraction[extr_per_fem_iter].increment;

        /** For each action array map each BCM action to DNX action(s)*/
        SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                        (unit, dnx_field_stage, fem_action_info->fem_extraction[extr_per_fem_iter].action_type,
                         &dnx_action));

        fem_encoded_actions_p[extr_per_fem_iter] = dnx_action;
        fem_action_entry_p[extr_per_fem_iter].fem_action = dnx_action;

        for (map_bits_per_fem_iter = 0; map_bits_per_fem_iter < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM &&
             fem_action_info->fem_extraction[extr_per_fem_iter].output_bit[map_bits_per_fem_iter].source_type !=
             bcmFieldFemExtractionOutputSourceTypeCount; map_bits_per_fem_iter++)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_fem_ext_output_src_type_bcm_to_dnx(unit,
                                                                             fem_action_info->fem_extraction
                                                                             [extr_per_fem_iter].output_bit
                                                                             [map_bits_per_fem_iter].source_type,
                                                                             &(fem_action_entry_p
                                                                               [extr_per_fem_iter].fem_bit_info
                                                                               [map_bits_per_fem_iter].fem_bit_format)));

            switch (fem_action_info->fem_extraction[extr_per_fem_iter].output_bit[map_bits_per_fem_iter].source_type)
            {
                case bcmFieldFemExtractionOutputSourceTypeKey:
                {
                    /** Check if the source_type is KEY, we should take the relevant bits from the key. */
                    fem_action_entry_p[extr_per_fem_iter].fem_bit_info[map_bits_per_fem_iter].
                        fem_bit_value.bit_on_key_select =
                        fem_action_info->fem_extraction[extr_per_fem_iter].output_bit[map_bits_per_fem_iter].offset;
                    break;
                }
                case bcmFieldFemExtractionOutputSourceTypeForce:
                {
                    fem_action_entry_p[extr_per_fem_iter].fem_bit_info[map_bits_per_fem_iter].fem_bit_value.bit_value =
                        fem_action_info->fem_extraction[extr_per_fem_iter].
                        output_bit[map_bits_per_fem_iter].forced_value;
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal FEM extraction output source type!");
                    break;
                }
            }
        }
    }
    /** Fill the overriding_fg_id. */
    if (fem_action_info->fem_input.overriding_fg_id == BCM_FIELD_ID_INVALID)
    {
        dnx_second_fg_id = DNX_FIELD_GROUP_INVALID;
    }
    else
    {
        dnx_second_fg_id = fem_action_info->fem_input.overriding_fg_id;
    }

    /** Call DNX-level API. */
    SHR_IF_ERR_EXIT(dnx_field_fem_action_add
                    (unit, dnx_fem_id, dnx_fg_id, fem_action_info->fem_input.input_offset, input_size,
                     dnx_second_fg_id, fem_action_info->condition_msb, fem_condition_entry_p,
                     fem_encoded_actions_p, fem_action_entry_p));

exit:
    SHR_FREE(fem_condition_entry_p);
    SHR_FREE(fem_encoded_actions_p);
    SHR_FREE(fem_action_entry_p);
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function verifies input parameters of bcm_field_fem_action_info_get().
 * \param [in] unit -
 *   Device ID
 * \param [in] fg_id -
 *   Field group id, to which fem has been added.
 * \param [in] action_priority -
 *   Variable of type bcm_field_action_priority_t. Contains information ('array_id' and
 *   'fem_position_in_array') from which 'fem_id' can be constructed.
 *   This FEM has been added to specified 'fg_id' when bcm_dnx_field_fem_action_add()
 *   was invoked.
 * \param [in] fem_action_info -
 *   Pointer to struct of FEM action info (see bcm_field_fem_action_info_t in field.h).
 *   This procedure loads pointed memory by information collected from HW which corresponds
 *   to that loaded when bcm_dnx_field_fem_action_add() has been invoked.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   * Note that only 'extraction_id's that are specified on the conditions array
 *     (fem_action_info->fem_condition[fem_condition_index].extraction_id) have meaningful
 *     entries on fem_action_info->fem_extraction[]. The non-meaningful entries are
 *     marked as follows: action_type = bcmFieldActionCount,
 *     source_type = bcmFieldFemExtractionOutputSourceTypeCount
 *     (See bcm_field_fem_action_info_t_init()).
 * \see
 *  * bcm_field_fem_action_info_t_init
 *  * bcm_field_fem_action_add
 *  * bcm_dnx_field_fem_action_add
 *  * dnx_field_fem_action_add
 *  * dnx_field_actions_fem_set
 *  * dnx_field_actions_fem_get
 */
static int
dnx_field_fem_action_info_get_verify(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_action_priority_t action_priority,
    bcm_field_fem_action_info_t * fem_action_info)
{
    dnx_field_group_t dnx_fg_id;
    dnx_field_fem_id_t dnx_fem_id;
    dnx_field_group_type_e fg_type;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(fem_action_info, _SHR_E_PARAM, "fem_action_info");
    /*
     * Type cast the fg_id to be aligned to DNX type.
     */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    /*
     * Convert input (encoded) 'action_priority' to 'fem_id'.
     */
    DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(dnx_fem_id, unit, action_priority);
    /*
     * Field Group Id can't exceed maximum number of Field Group IDs allow per unit.
     */
    DNX_FIELD_FG_ID_VERIFY(unit, dnx_fg_id);
    DNX_FIELD_FEM_ID_VERIFY(unit, dnx_fem_id);
    /*
     * Get the field group type.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, dnx_fg_id, &fg_type));
    /*
     * Get the Stage on which field group was created.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));
    /*
     * The FEM tool is available only on IPMF1 and IPMF2 stages!
     */
    if (dnx_field_stage != DNX_FIELD_STAGE_IPMF1 && dnx_field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Illegal stage (%s) implied by input fg_id (%d).\r\n"
                     "==> Stage can only be bcmFieldStageIngressPMF1 or bcmFieldStageIngressPMF2.\r\n",
                     dnx_field_stage_e_get_name(dnx_field_stage), dnx_fg_id);
    }

    if ((fg_type != DNX_FIELD_GROUP_TYPE_TCAM) && (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION)
        && (fg_type != DNX_FIELD_GROUP_TYPE_EXEM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Illegal fg_type (%s) implied by input fg_id (%d).\r\n"
                     "==> Stage can only be bcmFieldStageIngressPMF1 or bcmFieldStageIngressPMF2.\r\n",
                     dnx_field_group_type_e_get_name(fg_type), dnx_fg_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function gets info related to FEM machine as was specified by the
 *   user that invoked the corresponding bcm_dnx_field_fem_action_add().
 * \param [in] unit -
 *   Device ID
 * \param [in] fg_id -
 *   Field group id, to which fem has been added.
 * \param [in] action_priority -
 *   Variable of type bcm_field_action_priority_t. Contains information ('array_id' and
 *   'fem_position_in_array') from which 'fem_id' can be constructed.
 *   This FEM has been added to specified 'fg_id' when bcm_dnx_field_fem_action_add()
 *   was invoked.
 * \param [in] fem_action_info -
 *   Pointer to struct of FEM action info (see bcm_field_fem_action_info_t in field.h).
 *   This procedure loads pointed memory by information collected from HW which corresponds
 *   to that loaded when bcm_dnx_field_fem_action_add() has been invoked.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *   * Note that only 'extraction_id's that are specified on the conditions array
 *     (fem_action_info->fem_condition[fem_condition_index].extraction_id) have meaningful
 *     entries on fem_action_info->fem_extraction[]. The non-meaningful entries are
 *     marked as follows: action_type = bcmFieldActionCount,
 *     source_type = bcmFieldFemExtractionOutputSourceTypeCount
 *     (See bcm_field_fem_action_info_t_init()).
 * \see
 *  * bcm_field_fem_action_info_t_init
 *  * bcm_field_fem_action_add
 *  * bcm_dnx_field_fem_action_add
 *  * dnx_field_fem_action_add
 *  * dnx_field_actions_fem_set
 *  * dnx_field_actions_fem_get
 */
int
bcm_dnx_field_fem_action_info_get(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_action_priority_t action_priority,
    bcm_field_fem_action_info_t * fem_action_info)
{
    dnx_field_group_t dnx_fg_id;
    dnx_field_fem_id_t dnx_fem_id;
    dnx_field_stage_e dnx_field_stage;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    dnx_field_fem_program_t fem_program;
    int found;
    dnx_field_group_t sw_fg_id;
    uint8 input_offset;
    dnx_field_fem_condition_entry_t *fem_condition_entry_p = NULL;
    dnx_field_fem_action_entry_t *fem_action_entry_p = NULL;
    dnx_field_action_t *fem_encoded_actions_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_fem_action_info_get_verify(unit, fg_id, action_priority, fem_action_info));
    SHR_ALLOC(fem_condition_entry_p, sizeof(*fem_condition_entry_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                                          DNX_FIELD_STAGE_IPMF1)->nof_fem_condition,
              "fem_condition_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(fem_action_entry_p,
              sizeof(*fem_action_entry_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index,
              "fem_action_entry_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    bcm_field_fem_action_info_t_init(fem_action_info);
    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;
    /** Convert input (encoded) 'action_priority' to 'fem_id'. */
    DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(dnx_fem_id, unit, action_priority);

    /** Get the Stage on which field group was created. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    /*
     * Find the 'FEM program' corresponding to the pair 'fem_id,fg_id'.
     * Note that, per 'fem_id' we have only one 'fg_id' so it is sufficient
     * to find the first matching 'fg_id' for that 'fem_id'.
     * If a matching 'fg_id' is not found then return to the caller with an error: _SHR_E_PARAM
     */
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;
    found = FALSE;
    /** Just set some initial value to silence compiler's warning. */
    fem_program = DNX_FIELD_FEM_PROGRAM_INVALID;
    for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
    {
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, dnx_fem_id, fem_program_index, &sw_fg_id));
        if (sw_fg_id == dnx_fg_id)
        {
            fem_program = fem_program_index;
            found = TRUE;
        }
    }
    if (found != TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Specified input fg_id (%d) could not be found on this fem_id (%d).\r\n"
                     "==> Wrong input.\r\n", dnx_fg_id, dnx_fem_id);
    }
    /** At this point, 'fem_program' corresponds to input 'fg_id'. */
    /** 1. Get 'input_offset' and 'overriding_fg_id'. Both on SWSTATE. */
    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.second_fg_id.get(unit, dnx_fem_id, fem_program, &sw_fg_id));
    if (sw_fg_id == DNX_FIELD_GROUP_INVALID)
    {
        fem_action_info->fem_input.overriding_fg_id = BCM_FIELD_ID_INVALID;
    }
    else
    {
        fem_action_info->fem_input.overriding_fg_id = (bcm_field_group_t) sw_fg_id;
    }

    SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.input_offset.get(unit, dnx_fem_id, fem_program, &input_offset));
    {
        /*
         * Half TCAM payload size FGs need to have extra boost in offset when TCAM result flip
         * ECO is implemented, since the result is now placed aligned to full TCAM payload size.
         * The extra boost was added internally in the FEM_ADD and we now extract the offset.
         */
        int template_shift;
        SHR_IF_ERR_EXIT(dnx_field_group_action_buffer_shift_get(unit, fg_id, &template_shift));
        input_offset -= template_shift;
    }
    fem_action_info->fem_input.input_offset = input_offset;
    {
        dnx_field_fem_condition_t fem_condition_index, fem_condition_max;
        dnx_field_fem_map_index_t fem_map_index_max, fem_map_index;
        uint8 ignore_actions;
        /** Get all remaining info from HW. */
        dnx_field_fem_condition_ms_bit_t condition_ms_bit;
        /*
         * 'fem_encoded_actions' is the encoded version of DNX 'action's as stored in HW. This is the value
         * that was specified by the caller of dnx_field_fem_action_add() (See bcm_dnx_field_fem_action_add())
         */
        unsigned int bit_in_fem_action;

        SHR_ALLOC(fem_encoded_actions_p, sizeof(*fem_encoded_actions_p) * dnx_data_field.stage.stage_info_get(unit,
                                                                                                              DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index,
                  "fem_encoded_actions_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        /*
         * 3. Get all 'condition' entries (the array in which the 4 bits specified in 'condition_msb' above are index).
         *    This is read directly from HW.
         */
        fem_condition_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_condition;
        if (BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM != fem_condition_max)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "Number of conditions per FEM on BCM (%d) is not equal to the same on DNX (%d).\r\n"
                         "==> Internal error.\r\n", BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM, fem_condition_max);
        }
        for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
        {
            /** Initialize DNX FEM condition entry arrays. */
            SHR_IF_ERR_EXIT(dnx_field_fem_condition_entry_t_init(unit, &fem_condition_entry_p[fem_condition_index]));
        }
        /*
         * Maximum number of FEM 'map indices' in iPMF1/2 == 4
         * This is the number of actions available per FEM
         */
        fem_map_index_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_map_index;
        if (BCM_FIELD_NUMBER_OF_EXTRACTIONS_PER_FEM != fem_map_index_max)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "Number of actions (extractions) per FEM on BCM (%d) is not equal to the same on DNX (%d).\r\n"
                         "==> Internal error.\r\n", BCM_FIELD_NUMBER_OF_EXTRACTIONS_PER_FEM, fem_map_index_max);
        }
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            /** Initialize DNX FEM action entry arrays. */
            SHR_IF_ERR_EXIT(dnx_field_fem_action_entry_t_init(unit, &fem_action_entry_p[fem_map_index]));
            fem_encoded_actions_p[fem_map_index] = DNX_FIELD_ACTION_INVALID;
        }

        SHR_IF_ERR_EXIT(dnx_field_actions_fem_get
                        (unit, dnx_fem_id, fem_program, &condition_ms_bit, fem_condition_entry_p, fem_action_entry_p));
        /*
         * 2. Get 'condition_msb' (the MSB of a 4 bit chunk that will choose 'condition' entry ID).
         *    This is read directly from HW.
         */
        fem_action_info->condition_msb = condition_ms_bit;
        for (fem_condition_index = 0; fem_condition_index < fem_condition_max; fem_condition_index++)
        {
            fem_action_info->fem_condition[fem_condition_index].extraction_id =
                fem_condition_entry_p[fem_condition_index].fem_map_index;
            fem_action_info->fem_condition[fem_condition_index].is_action_valid =
                fem_condition_entry_p[fem_condition_index].fem_action_valid;
        }
        /*
         * 4. Get all 'extractions' (action) entries (action, adder, and des criptor of each bit.
         *    This is read directly from HW. Load only entries which correspond to 'extractions'
         *    that are on the 'conditions' array.
         */
        if (BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM != DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "\r\n"
                         "Number of bit descriptors per actions (extraction) on BCM (%d) is not equal to the same on DNX (%d).\r\n"
                         "==> Internal error.\r\n", BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM,
                         DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION);
        }
        /*
         * 'ignore_actions' is a bit map: If bit(i) is set then action (extraction) 'i' is NOT meaningful.
         */
        SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.ignore_actions.get(unit, dnx_fem_id, fem_program, &ignore_actions));
        for (fem_map_index = 0; fem_map_index < fem_map_index_max; fem_map_index++)
        {
            if ((ignore_actions & SAL_BIT(fem_map_index)) == 0)
            {
                /*
                 * Handle the ADDER (increment).
                 */
                fem_action_info->fem_extraction[fem_map_index].increment =
                    (uint32) (fem_action_entry_p[fem_map_index].fem_adder);
                /*
                 * For each valid action on array, map DNX action to BCM action.
                 */
                SHR_IF_ERR_EXIT(FEM_INFO_FEM_ENCODED_ACTIONS.get(unit, dnx_fem_id, fem_program, fem_map_index,
                                                                 &(fem_encoded_actions_p[fem_map_index])));
                SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                                (unit, dnx_field_stage, fem_encoded_actions_p[fem_map_index],
                                 &(fem_action_info->fem_extraction[fem_map_index].action_type)));
                for (bit_in_fem_action = 0;
                     bit_in_fem_action < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FEM_ACTION; bit_in_fem_action++)
                {
                    if (fem_action_entry_p[fem_map_index].fem_bit_info[bit_in_fem_action].fem_bit_format !=
                        DNX_FIELD_FEM_BIT_FORMAT_INVALID)
                    {
                        /*
                         * Convert 'bit_format' on DNX to 'source_type' on BCM.
                         */
                        SHR_IF_ERR_EXIT(dnx_field_map_fem_ext_output_src_type_dnx_to_bcm(unit,
                                                                                         fem_action_entry_p
                                                                                         [fem_map_index].fem_bit_info
                                                                                         [bit_in_fem_action].fem_bit_format,
                                                                                         &
                                                                                         (fem_action_info->fem_extraction
                                                                                          [fem_map_index].output_bit
                                                                                          [bit_in_fem_action].source_type)));
                        switch (fem_action_info->fem_extraction[fem_map_index].
                                output_bit[bit_in_fem_action].source_type)
                        {
                            case bcmFieldFemExtractionOutputSourceTypeKey:
                            {
                                /*
                                 * If the source_type is KEY, we should take the relevant bits from the key.
                                 */
                                fem_action_info->fem_extraction[fem_map_index].output_bit[bit_in_fem_action].offset =
                                    fem_action_entry_p[fem_map_index].fem_bit_info[bit_in_fem_action].
                                    fem_bit_value.bit_on_key_select;
                                break;
                            }
                            case bcmFieldFemExtractionOutputSourceTypeForce:
                            {
                                fem_action_info->fem_extraction[fem_map_index].
                                    output_bit[bit_in_fem_action].forced_value =
                                    fem_action_entry_p[fem_map_index].fem_bit_info[bit_in_fem_action].
                                    fem_bit_value.bit_value;
                                break;
                            }
                            default:
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "Illegal BCM-level FEM extraction output source type (%d) when converting from DNX (HW)!",
                                             fem_action_info->
                                             fem_extraction[fem_map_index].output_bit[bit_in_fem_action].source_type);
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
exit:
    SHR_FREE(fem_condition_entry_p);
    SHR_FREE(fem_action_entry_p);
    SHR_FREE(fem_encoded_actions_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function returns the offset of the given action in the field group that its ID is
 *  given. Returns an error in case the action was not found inside the given field group.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  TBD
 * \param [in] fg_id -
 *  Field group id, that contains the action we want to get the offset for
 * \param [in] bcm_action -
 *  The BCM action to return the offset for
 * \param [out] offset_p -
 *  The offset for the given action for the given fg_id
 * \remark
 *  None
 * \see
 *  * bcm_field_fem_action_info_t_init
 *  * bcm_field_fem_action_add
 */
int
bcm_dnx_field_group_action_offset_get(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_action_t bcm_action,
    int *offset_p)
{
    dnx_field_action_t dnx_action;
    dnx_field_stage_e dnx_field_stage;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(offset_p, _SHR_E_PARAM, "offset_p");
    /** Get field group stage and map BCM action to DNX action. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &dnx_field_stage));

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx(unit, dnx_field_stage, bcm_action, &(dnx_action)));

    /** Call DNX-level API. */
    SHR_IF_ERR_EXIT(dnx_field_group_action_offset_get(unit, fg_id, dnx_action, offset_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function sets FEM machine's HW and SW values (SWSTATE) to initial
 *  values.
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *  Field group id, to which fem is going to be added.
 * \param [in] action_priority -
 *   Variable of type bcm_field_action_priority_t. Contains information ('array_id' and
 *   'fem_position_in_array' from which 'fem_id' can be constructed.
 *   This FEM is going to be deleted.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_fem_action_add
 *  - bcm_field_fem_action_info_get
 */
int
bcm_dnx_field_fem_action_delete(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_action_priority_t action_priority)
{
    dnx_field_group_t dnx_fg_id;
    dnx_field_fem_id_t dnx_fem_id;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** Convert input (encoded) 'action_priority' to 'fem_id'. */
    DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET(dnx_fem_id, unit, action_priority);
    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    /** Call DNX-level API. */
    SHR_IF_ERR_EXIT(dnx_field_fem_action_remove(unit, dnx_fem_id, dnx_fg_id));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify function for bcm_dnx_field_efes_action_add.
 *  group.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  Field group id, to which EFEs is going to be added for a specific context.
 * \param [in] context_id -
 *  Context ID, attached to fg_id, in which the EFES will be added.
 * \param [in] action_priority -
 *  Variable of type bcm_field_action_priority_t. Contains the position of the EFES.
 * \param [in] efes_action_info -
 *  Pointer to struct of EFES action info (see bcm_field_efes_action_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  * bcm_field_efes_action_info_t_init
 *  * bcm_field_efes_action_info_get
 */
static int
dnx_field_efes_action_add_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_action_priority_t * action_priority,
    bcm_field_efes_action_info_t * efes_action_info)
{
    dnx_field_group_t dnx_fg_id;
    int fes_2msb_comb;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_priority, _SHR_E_PARAM, "action_priority");
    SHR_NULL_CHECK(efes_action_info, _SHR_E_PARAM, "efes_action_info");

    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    /** Field Group Id can't exceed maximum number of Field Group IDs allow per unit. */
    DNX_FIELD_FG_ID_VERIFY(unit, dnx_fg_id);

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    for (fes_2msb_comb = 0; fes_2msb_comb < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES; fes_2msb_comb++)
    {
        if (efes_action_info->efes_condition_conf[fes_2msb_comb].is_action_valid)
        {
            if (fes_2msb_comb >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "efes_condition_conf element %d is valid, but there are only %d options.\n",
                             fes_2msb_comb, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES);
            }
            if (efes_action_info->efes_condition_conf[fes_2msb_comb].action_type == BCM_FIELD_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "'action_type' must be provided for all valid elements of "
                             "efes_condition_conf (not provided for %d).\n", fes_2msb_comb);
            }
            if (efes_action_info->efes_condition_conf[fes_2msb_comb].action_lsb == BCM_FIELD_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "'action_lsb' must be provided for all valid elements of "
                             "efes_condition_conf (not provided for %d).\n", fes_2msb_comb);
            }
            if (efes_action_info->efes_condition_conf[fes_2msb_comb].action_nof_bits == BCM_FIELD_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "'action_nof_bits' must be provided for all valid elements of "
                             "efes_condition_conf (not provided for %d).\n", fes_2msb_comb);
            }
            if (efes_action_info->efes_condition_conf[fes_2msb_comb].valid_bit_polarity !=
                DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ONE &&
                efes_action_info->efes_condition_conf[fes_2msb_comb].valid_bit_polarity !=
                DNX_FIELD_ACTION_FES_VALID_BIT_POLARITY_ZERO)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Given value for valid_bit_polarity %d is invalid (in configuration %d). "
                             "Valid bit polarity must be 0 or 1!\n",
                             efes_action_info->efes_condition_conf[fes_2msb_comb].valid_bit_polarity, fes_2msb_comb);
            }
        }
    }

    if (efes_action_info->is_condition_valid)
    {
        if (efes_action_info->condition_msb == BCM_FIELD_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "'is_condition_valid' is true, "
                         "but no position of condition bits provided in 'condition_msb'.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds an EFES to a specific field group and context pair.
 *  group.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] fg_id -
 *  Field group id, to which EFEs is going to be added for a specific context.
 * \param [in] context_id -
 *  Context ID, attached to fg_id, in which the EFES will be added.
 * \param [in] action_priority -
 *  Variable of type bcm_field_action_priority_t. Contains the position of the EFES.
 * \param [in] efes_action_info -
 *  Pointer to struct of EFES action info (see bcm_field_efes_action_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  * bcm_field_efes_action_info_t_init
 *  * bcm_field_efes_action_info_get
 */
int
bcm_dnx_field_efes_action_add(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_action_priority_t * action_priority,
    bcm_field_efes_action_info_t * efes_action_info)
{
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    dnx_field_group_t dnx_fg_id;
    dnx_field_context_t dnx_context_id;
    int use_condition;
    int fes_2msb_comb;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_efes_action_add_verify
                          (unit, flags, fg_id, context_id, action_priority, efes_action_info));

    /** Init efes_encoded_extraction*/
    dnx_field_actions_fes_single_add_conf_t_init(unit, efes_encoded_extraction);

    /** Type cast the context_id to be aligned to DNX type. */
    dnx_context_id = (dnx_field_context_t) context_id;
    /** Type cast the fg_id to be aligned to DNX type. */
    dnx_fg_id = (dnx_field_group_t) fg_id;

    /** Get the Stage on which field group was created. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, dnx_fg_id, &dnx_field_stage));

    /*
     * Fill the EFES configuration input.
     */
    for (fes_2msb_comb = 0; fes_2msb_comb < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_comb++)
    {
        if (efes_action_info->efes_condition_conf[fes_2msb_comb].is_action_valid)
        {
            SHR_IF_ERR_EXIT(dnx_field_map_action_bcm_to_dnx
                            (unit, dnx_field_stage, efes_action_info->efes_condition_conf[fes_2msb_comb].action_type,
                             &efes_encoded_extraction[fes_2msb_comb].dnx_action));
            efes_encoded_extraction[fes_2msb_comb].lsb =
                efes_action_info->efes_condition_conf[fes_2msb_comb].action_lsb;
            efes_encoded_extraction[fes_2msb_comb].mask =
                efes_action_info->efes_condition_conf[fes_2msb_comb].action_or_mask;
            efes_encoded_extraction[fes_2msb_comb].size =
                efes_action_info->efes_condition_conf[fes_2msb_comb].action_nof_bits;
            if (efes_action_info->efes_condition_conf[fes_2msb_comb].action_with_valid_bit)
            {
                efes_encoded_extraction[fes_2msb_comb].dont_use_valid_bit = FALSE;
            }
            else
            {
                efes_encoded_extraction[fes_2msb_comb].dont_use_valid_bit = TRUE;
            }
            efes_encoded_extraction[fes_2msb_comb].valid_bit_polarity =
                efes_action_info->efes_condition_conf[fes_2msb_comb].valid_bit_polarity;
        }
        else
        {
            efes_encoded_extraction[fes_2msb_comb].dnx_action = DNX_FIELD_ACTION_INVALID;
        }
    }

    if (efes_action_info->is_condition_valid)
    {
        use_condition = TRUE;
    }
    else
    {
        use_condition = FALSE;
    }

    /*
     * Call the DNX API.
     */
    SHR_IF_ERR_EXIT(dnx_field_efes_action_add
                    (unit, dnx_fg_id, dnx_context_id, *action_priority, use_condition, efes_action_info->condition_msb,
                     efes_encoded_extraction));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function gets info related to an EFES that was added by bcm_dnx_field_efes_action_add().
 * \param [in] unit -
 *   Device ID
 * \param [in] fg_id -
 *   Field group id, to which EFES has been added.
 * \param [in] context_id -
 *   Context ID, attached to fg_id, in which the EFES was added.
 * \param [in] action_priority -
 *   Variable of type bcm_field_action_priority_t. Contains the position of the EFES.
 * \param [out] efes_action_info -
 *   Pointer to struct of EFES action info (see bcm_field_efes_action_info_t in field.h).
 *   This procedure loads pointed memory by information collected from HW which corresponds
 *   to that loaded when bcm_dnx_field_efes_action_add() has been invoked.
 * \return
 *   int - Error Type, in sense of bcm_error_t, same as
 *   shr_error_t
 * \remark
 *  None
 * \see
 *  * bcm_field_efes_action_info_t_init
 *  * bcm_field_efes_action_add
 *  * bcm_dnx_field_efes_action_add
 *  * dnx_field_efes_action_add
 */
int
bcm_dnx_field_efes_action_info_get(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_action_priority_t action_priority,
    bcm_field_efes_action_info_t * efes_action_info)
{
    int use_condition;
    int efes_condition_msb;
    dnx_field_actions_fes_single_add_conf_t efes_encoded_extraction[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES];
    int fes_2msb_comb;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    bcm_field_efes_action_info_t_init(efes_action_info);

    /** Call the DNX get*/
    SHR_IF_ERR_EXIT(dnx_field_efes_action_info_get(unit, fg_id, context_id, action_priority,
                                                   &use_condition, &efes_condition_msb, efes_encoded_extraction));

    /** Get the Stage on which field group was created. */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &dnx_field_stage));

    /*
     * Fill efes_action_info
     */
    efes_action_info->is_condition_valid = use_condition;
    if (efes_action_info->is_condition_valid)
    {
        efes_action_info->condition_msb = efes_condition_msb;
    }
    /*
     * Get the different configurations on the EFES.
     * Note that we cannot have more than BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES different configurations.
     */
    for (fes_2msb_comb = 0; fes_2msb_comb <
          /* coverity[same_on_both_sides:FALSE]  */
         MIN(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES, BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES);
         fes_2msb_comb++)
    {
        if (efes_encoded_extraction[fes_2msb_comb].dnx_action == DNX_FIELD_ACTION_INVALID)
        {
            efes_action_info->efes_condition_conf[fes_2msb_comb].is_action_valid = FALSE;
        }
        else
        {
            efes_action_info->efes_condition_conf[fes_2msb_comb].is_action_valid = TRUE;
            SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                            (unit, dnx_field_stage, efes_encoded_extraction[fes_2msb_comb].dnx_action,
                             &efes_action_info->efes_condition_conf[fes_2msb_comb].action_type));
            efes_action_info->efes_condition_conf[fes_2msb_comb].action_lsb =
                efes_encoded_extraction[fes_2msb_comb].lsb;
            efes_action_info->efes_condition_conf[fes_2msb_comb].action_nof_bits =
                efes_encoded_extraction[fes_2msb_comb].size;
            efes_action_info->efes_condition_conf[fes_2msb_comb].action_or_mask =
                efes_encoded_extraction[fes_2msb_comb].mask;
            efes_action_info->efes_condition_conf[fes_2msb_comb].action_with_valid_bit =
                !(efes_encoded_extraction[fes_2msb_comb].dont_use_valid_bit);
        }
    }
    /*
     * Sanity check: Verify that we don't have more than BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES configurations.
     */
    for (; fes_2msb_comb < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES; fes_2msb_comb++)
    {
        /*
         * Coverity explanation: DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES and
         * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_PROG_PER_FES are currently the same value,
         * but we can't rely on that always being true.
         */
         /* coverity[dead_error_line:FALSE]  */
        if (efes_encoded_extraction[fes_2msb_comb].dnx_action != DNX_FIELD_ACTION_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "There are more than %d different configurations.\n",
                         BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function deletes an EFES that was added with bcm_dnx_field_efes_action_add().
 *  values.
 * \param [in] unit -
 *  Device ID
 * \param [in] fg_id -
 *   Field group id, to which EFES has been added.
 * \param [in] context_id -
 *  Context ID, attached to fg_id, in which the EFES was added.
 * \param [in] action_priority -
 *   Variable of type bcm_field_action_priority_t. Contains the position of the EFES.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_field_efes_action_add
 *  - bcm_field_efes_action_info_get
 */
int
bcm_dnx_field_efes_action_delete(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_context_t context_id,
    bcm_field_action_priority_t action_priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_field_efes_action_remove(unit, fg_id, context_id, action_priority));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verifies input parameters of bcm_field_presel_set().
* \param [in] unit       - Device ID
* \param [in] flags      - Preselection flags (currently not in
*        use)
* \param [in] entry_id   - Preselection entry identifier.
*      See bcm_field_presel_entry_id_t structure containing:
*           - presel_id  - Preselector ID
*                  values: range within 0-127
*           - stage      - Preselection PMF stage
*                  values: See dnx_field_map_stage_info for BCM to DNX stage mapping
* \param [in] entry_data - Preselection entry data.
*       See bcm_field_presel_entry_data_t structure
*          containing:
*           - entry_valid - bool for HW valid of whole
*             preselector
*           - program_id  - Context ID
*                  values: range within 0-63
*           - nof_qualifiers - Number of qualifiers, used for
*             this preselector
*           - qual_data[] - Array of data describing the
*             qualifiers, used for this preselector
* \return
*   int - Error Type, in sense of bcm_error_t, same as
*   shr_error_t
* \remark
*   * Exposed in BCM API bcm_field_presel_set
* \see
*   * bcm_error_e
*/
static int
dnx_field_presel_set_verify(
    int unit,
    uint32 flags,
    bcm_field_presel_entry_id_t * entry_id,
    bcm_field_presel_entry_data_t * entry_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_id, _SHR_E_PARAM, "entry_id");
    SHR_NULL_CHECK(entry_data, _SHR_E_PARAM, "entry_data");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    if (entry_data->nof_qualifiers > BCM_FIELD_MAX_NOF_CS_QUALIFIERS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "entry_data->nof_qualifiers is %d, cannot exceed BCM maximum %d",
                     entry_data->nof_qualifiers, BCM_FIELD_MAX_NOF_CS_QUALIFIERS);
    }
    if (entry_data->nof_qualifiers > DNX_FIELD_CS_QUAL_NOF_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "entry_data->nof_qualifiers is %d, cannot exceed %d",
                     entry_data->nof_qualifiers, DNX_FIELD_CS_QUAL_NOF_MAX);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief Set a preselector instance API
* \param [in] unit       - Device ID
* \param [in] flags      - Preselection flags (currently not in
*        use)
* \param [in] entry_id   - Preselection entry identifier.
*      See bcm_field_presel_entry_id_t structure containing:
*           - presel_id  - Preselector ID
*                  values: range within 0-127
*           - stage      - Preselection PMF stage
*                  values: See dnx_field_map_stage_info for BCM to DNX stage mapping
* \param [in] entry_data - Preselection entry data.
*       See bcm_field_presel_entry_data_t structure
*          containing:
*           - entry_valid - bool for HW valid of whole
*             preselector
*           - program_id  - Context ID
*                  values: range within 0-63
*           - nof_qualifiers - Number of qualifiers, used for
*             this preselector
*           - qual_data[] - Array of data describing the
*             qualifiers, used for this preselector
* \return
*   int - Error Type, in sense of bcm_error_t, same as
*   shr_error_t
* \remark
*   * Exposed in BCM API bcm_field_presel_set
* \see
*   * bcm_error_e
*/
int
bcm_dnx_field_presel_set(
    int unit,
    uint32 flags,
    bcm_field_presel_entry_id_t * entry_id,
    bcm_field_presel_entry_data_t * entry_data)
{
    dnx_field_presel_entry_id_t dnx_entry_id;
    dnx_field_presel_entry_data_t dnx_entry_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_presel_set_verify(unit, flags, entry_id, entry_data));

    SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, &dnx_entry_id));
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &dnx_entry_data));
    /*
     * Translate BCM level structures into DNX level structures to provide as parameters
     * Verification of translation are done inside the translation functions
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_id_bcm_to_dnx_convert(unit, flags, entry_id, &dnx_entry_id));
    SHR_IF_ERR_EXIT(dnx_field_presel_data_bcm_to_dnx_convert
                    (unit, flags, dnx_entry_id.stage, entry_data, &dnx_entry_data));

    /*
     * DNX API call
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, flags, &dnx_entry_id, &dnx_entry_data));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
* \brief Get a preselector instance API
* \param [in] unit         - Device ID
* \param [in] flags        - preselection flags (currently not
*        in use)
* \param [in] entry_id     - preselection entry identifier. See
*        bcm_field_presel_entry_id_t structure containing:
*           - presel_id  - Preselector ID
*                  values: range within 0-127
*           - stage      - Preselection PMF stage
*                  values: See dnx_field_map_stage_info for BCM to DNX stage mapping
* \param [out] entry_data  - preselection entry data
*         See bcm_field_presel_entry_data_t structure
*           containing:
*           - entry_valid - bool for HW valid of whole
*             preselector
*           - program_id  - Context ID
*                  values: range within 0-63
*           - nof_qualifiers - Number of qualifiers, used for
*             this preselector
*           - qual_data[] - Array of data describing the
*             qualifiers, used for this preselector
* \return
*   int - Error Type, in sense of bcm_error_t, same as
*   shr_error_t
* \remark
*   * Exposed in BCM API bcm_field_presel_get
* \see
*   * bcm_error_e
*/
int
bcm_dnx_field_presel_get(
    int unit,
    uint32 flags,
    bcm_field_presel_entry_id_t * entry_id,
    bcm_field_presel_entry_data_t * entry_data)
{
    dnx_field_presel_entry_id_t dnx_entry_id;
    dnx_field_presel_entry_data_t dnx_entry_data;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(entry_id, _SHR_E_PARAM, "entry_id");
    SHR_NULL_CHECK(entry_data, _SHR_E_PARAM, "entry_data");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_field_presel_entry_id_t_init(unit, &dnx_entry_id));
    SHR_IF_ERR_EXIT(dnx_field_presel_entry_data_t_init(unit, &dnx_entry_data));
    /*
     * Translate BCM level structure into DNX level structure to provide as parameter to DNX API
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_id_bcm_to_dnx_convert(unit, flags, entry_id, &dnx_entry_id));

    /*
     * DNX API call
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_get(unit, flags, &dnx_entry_id, &dnx_entry_data));

    /*
     * Translate DNX level result structure into BCM level structure to return as out parameter
     */
    SHR_IF_ERR_EXIT(dnx_field_presel_data_dnx_to_bcm_convert(unit, flags, &dnx_entry_id, &dnx_entry_data, entry_data));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  This function sets maximum and minimum value for a specific range.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] stage -
 *  Field stage on which the range id has bean allocated.
 * \param [in] range_id -
 *  Range id of which the parameters should be change.
 * \param [in] range_info -
 *  Pointer to structure of Range info (see bcm_field_range_info_t in field.h)
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 */
int
bcm_dnx_field_range_set(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_range_t range_id,
    bcm_field_range_info_t * range_info)
{
    dnx_field_stage_e dnx_field_stage;
    dnx_field_range_info_t dnx_range_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_NULL_CHECK(range_info, _SHR_E_PARAM, "range_info");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    /** Init all of the parameters in the structure dnx_field_range_info_t */
    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &dnx_range_info));

    /** Convert BCM to DNX Field Stage. */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));

    /** Convert BCM range type to relevant DNX range type. */
    SHR_IF_ERR_EXIT(dnx_field_map_range_type_bcm_to_dnx(unit, range_info->range_type, &dnx_range_info.range_type));

    /** Copy the min/max values, convert default values. */
    if (range_info->min_val == -1)
    {
        dnx_range_info.min_val = DNX_FIELD_RANGE_VALUE_INVALID;
    }
    else
    {
        dnx_range_info.min_val = range_info->min_val;
    }
    if (range_info->max_val == -1)
    {
        dnx_range_info.max_val = DNX_FIELD_RANGE_VALUE_INVALID;
    }
    else
    {
        dnx_range_info.max_val = range_info->max_val;
    }

    /** Call DNX-level API. */
    SHR_IF_ERR_EXIT(dnx_field_range_set(unit, flags, dnx_field_stage, range_id, &dnx_range_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
* \brief Get a range instance API
* \param [in] unit         - Device ID
* \param [in] stage        - The BCM level PMF stage
*                  values: See dnx_field_map_stage_info for BCM to DNX stage mapping
* \param [in] range_id  - Range ID
* \param [out] range_info_p  - Range info
*         See bcm_field_range_info_s structure
*           containing:
*           - range_type - Range_type
*           - min_val - The minimum value of the range
*           - max_val - The maximum valjue of the range
* \return
*   int - Error Type, in sense of bcm_error_t, same as
*   shr_error_t
* \remark
*   * Exposed in BCM API bcm_field_range_info_get
* \see
*   * bcm_error_e
*/
int
bcm_dnx_field_range_info_get(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_range_t * range_id,
    bcm_field_range_info_t * range_info_p)
{
    dnx_field_range_info_t dnx_range_info;
    dnx_field_stage_e dnx_field_stage;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(range_info_p, _SHR_E_PARAM, "range_info_p");
    /**
     * Init the value of the dnx_range_info
     */
    SHR_IF_ERR_EXIT(dnx_field_range_info_t_init(unit, &dnx_range_info));
    /**
     * Convert BCM to DNX Field Stage.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, stage, &dnx_field_stage));
    /**
     * Translate BCM level structure into DNX level structure to provide as parameter to DNX API
     */
    SHR_IF_ERR_EXIT(dnx_field_map_range_type_bcm_to_dnx(unit, range_info_p->range_type, &dnx_range_info.range_type));
    /**
     * DNX API call
     */
    SHR_IF_ERR_EXIT(dnx_field_range_get(unit, dnx_field_stage, range_id, &dnx_range_info));
    /**
     * Copy min and max values.
     */
    range_info_p->min_val = dnx_range_info.min_val;
    range_info_p->max_val = dnx_range_info.max_val;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to verify input params of bcm_field_range_type_config().
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] stage -
 *  Stage of the configurable range
 * \param [in] configurable_range_index -
 *  The index of the configurable range to set
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
static int
dnx_field_range_type_config_verify(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    uint32 configurable_range_index)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    if (stage != bcmFieldStageIngressPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "stage %s not supported, only stage bcmFieldStageIngressPMF1 is supported\n",
                     dnx_field_bcm_stage_text(stage));
    }

    /*
     * No need to validate index >= 0 since index is uint32 
     */
    if (configurable_range_index >= dnx_data_field.L4_Ops.nof_configurable_ranges_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Configurable range index %d out of range, configurable ranges are up to %d\n",
                     configurable_range_index, dnx_data_field.L4_Ops.nof_configurable_ranges_get(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to configure the FFCs of the L4 OPS Extended.
 * \param [in] unit                    - Device ID
 * \param [in] flags                    - TBD
 * \param [in] stage                    - Stage of the configurable range
 * \param [in] context_id               - Context ID to configure the configurable range for (not supported on J2P)
 * \param [in] configurable_range_index - The index of the configurable range to set
 * \param [in] range_qualifier_info     - Qualifier (type/info) to configure the range type with
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
int
bcm_dnx_field_range_type_config_set(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 configurable_range_index,
    bcm_field_range_qual_info_t * range_qualifier_info)
{
    uint32 dnx_flags;
    dnx_field_range_type_qual_info_t dnx_qual_info;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /** Validate input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_field_range_type_config_verify(unit, flags, stage, configurable_range_index));

    dnx_flags = 0;

    /*
     * Map BCM qual type/info to DNX counterparts
     */
    SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, DNX_FIELD_STAGE_L4_OPS, range_qualifier_info->qual_type,
                                                  &(dnx_qual_info.dnx_qual)));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_bcm_to_dnx
                    (unit, range_qualifier_info->qual_info.input_type, &(dnx_qual_info.qual_info.input_type)));
    dnx_qual_info.qual_info.input_arg = range_qualifier_info->qual_info.input_arg;
    dnx_qual_info.qual_info.offset = range_qualifier_info->qual_info.offset;

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_range_ffc_config_set(unit, dnx_flags, configurable_range_index, &dnx_qual_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to retrive the configuration of the FFCs of the L4 OPS Extended.
 * \param [in] unit                     - Device ID
 * \param [in] flags                    - TBD
 * \param [in] stage                    - Stage of the configurable range
 * \param [in] context_id               - Context ID to get the configurable range configuration from (not supported on J2P)
 * \param [in] configurable_range_index - The index of the configurable range to get
 * \param [out] range_qualifier_info    - The qualifier that the range type is configured with
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
int
bcm_dnx_field_range_type_config_get(
    int unit,
    uint32 flags,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id,
    uint32 configurable_range_index,
    bcm_field_range_qual_info_t * range_qualifier_info)
{
    uint32 dnx_flags;
    dnx_field_range_type_qual_info_t dnx_qual_info;

    SHR_FUNC_INIT_VARS(unit);

    /** Validate input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_field_range_type_config_verify(unit, flags, stage, configurable_range_index));

    dnx_flags = 0;

    /** Call DNX-level API */
    SHR_IF_ERR_EXIT(dnx_field_range_ffc_config_get(unit, dnx_flags, configurable_range_index, &dnx_qual_info));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                    (unit, DNX_FIELD_STAGE_L4_OPS, dnx_qual_info.dnx_qual, &(range_qualifier_info->qual_type)));

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_dnx_to_bcm
                    (unit, dnx_qual_info.qual_info.input_type, &(range_qualifier_info->qual_info.input_type)));

    range_qualifier_info->qual_info.input_arg = dnx_qual_info.qual_info.input_arg;
    range_qualifier_info->qual_info.offset = dnx_qual_info.qual_info.offset;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to give the offset of a specific compare operand inside the 6-bit
 * compare result qualifier.
 * \param [in] unit -
 *  Device ID
 * \param [in] pair_id -
 *  Up to two pairs can be create and used for comparison(Valid values are 0 and 1).
 *  Select the pair of keys to be compared, either First Pair or Second Pair
 * \param [in] compare_operand -
 *  The compare operand. It is showing which comparison we want to do.
 * (Can be equal, not equal, smaller, not smaller, bigger, not bigger)
 * \param [out] offset -
 *  The offset in the qualifier coresponding to the comapare_operand.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_compare_operand_offset_get
 */
int
bcm_dnx_field_compare_operand_offset_get(
    int unit,
    int pair_id,
    bcm_field_compare_operand_t compare_operand,
    int *offset)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(offset, _SHR_E_PARAM, "offset");
    /**
     * Up to two Compare Pairs can be created and used for comparison.
     * Their values must be "0" or "1".
     */
    DNX_FIELD_COMPARE_PAIR_VERIFY(unit, pair_id);
    /**
     * Switch between the compare operands and get the offset.
     */
    switch (compare_operand)
    {
        case bcmFieldCompareOperandEqual:
        {
            *offset = dnx_data_field.Compare_operand.equal_get(unit);
            break;
        }
        case bcmFieldCompareOperandNotEqual:
        {
            *offset = dnx_data_field.Compare_operand.not_equal_get(unit);
            break;
        }
        case bcmFieldCompareOperandFirstKeySmaller:
        {
            *offset = dnx_data_field.Compare_operand.smaller_get(unit);
            break;
        }
        case bcmFieldCompareOperandFirstKeyNotSmaller:
        {
            *offset = dnx_data_field.Compare_operand.not_smaller_get(unit);
            break;
        }
        case bcmFieldCompareOperandFirstKeyBigger:
        {
            *offset = dnx_data_field.Compare_operand.bigger_get(unit);
            break;
        }
        case bcmFieldCompareOperandFirstKeyNotBigger:
        {
            *offset = dnx_data_field.Compare_operand.not_bigger_get(unit);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal compare operand = %d\n", compare_operand);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function returns hit indication info for
 *  a specific TCAM entry per core.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags -
 *  Currently not in use!
 * \param [in] entry_handle -
 *  Entry handle for which hit indication info will be retrieved.
 * \param [out] entry_hit_core_bmp -
 *  2 bits bitmap, where every
*   bit indicates on which core the entry was hit:
*       - bit 0 = core 0
*       - bit 1 = core 1
*   There is possibility that entry was hit on both cores, then
*   both bits will be set.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_entry_hit_flush
 */
int
bcm_dnx_field_entry_hit_get(
    int unit,
    uint32 flags,
    bcm_field_entry_t entry_handle,
    uint8 *entry_hit_core_bmp)
{
    uint32 dnx_entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(entry_hit_core_bmp, _SHR_E_PARAM, "entry_hit_core_bmp");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    dnx_entry_handle = (uint32) entry_handle;

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_get(unit, flags, dnx_entry_handle, entry_hit_core_bmp));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function flushes hit information of an entry or all entries,
 *  from hardware.
 * \param [in] unit -
 *  Device ID
 * \param [in] flags - Flags to indicate what action should be performed
 *  by the API, currently in use is:
 *      - BCM_FIELD_ENTRY_HIT_FLUSH_ALL - used for flushing hit
 *        indication info for all entries. No need to specify an entry ID.
 *      - If no flag is specified, the API will clear information only for
 *        the given entry_handle.
 * \param [in] entry_handle -
 *  Entry handle for which hit indication info will be flushed.
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_entry_hit_get
 */
int
bcm_dnx_field_entry_hit_flush(
    int unit,
    uint32 flags,
    bcm_field_entry_t entry_handle)
{
    uint32 unsupported_flags;
    uint32 dnx_entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if ((unsupported_flags = (flags & ~(BCM_FIELD_ENTRY_HIT_FLUSH_ALL))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", unsupported_flags);
    }

    dnx_entry_handle = (uint32) entry_handle;

    SHR_IF_ERR_EXIT(dnx_field_entry_tcam_hit_flush(unit, flags, dnx_entry_handle));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function get's the ID of a field object (e.g. field group, qualifier, context) from its name.
 *  from hardware.
 * \param [in] unit            - Device ID
 * \param [in] flags           - Flags to indicate what action should be performed
 *                               by the API, currently no flag is used.
 * \param [in] name_to_id_info - All information about the object we find the ID for.
 * \param [out] nof_ids - The number of IDs returned for the name, number of elements in the array ids.
 * \param [out] ids              - The array of the IDs returned.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_entry_hit_get
 */
static int
dnx_field_name_to_id_verify(
    int unit,
    uint32 flags,
    bcm_field_name_to_id_info_t * name_to_id_info,
    int *nof_ids,
    uint32 ids[BCM_FIELD_NAME_TO_ID_MAX_IDS])
{
    int name_length;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(name_to_id_info, _SHR_E_PARAM, "name_to_id_info");
    SHR_NULL_CHECK(nof_ids, _SHR_E_PARAM, "nof_ids");
    SHR_NULL_CHECK(ids, _SHR_E_PARAM, "ids");

    if (flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Flags 0x%X are not supported.\n", flags);
    }

    if ((name_to_id_info->name_to_id_type < 0) || (name_to_id_info->name_to_id_type >= bcmFieldNameToIdCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "name_to_id_type (%d) is out of range (%d-%d).\n",
                     name_to_id_info->name_to_id_type, 0, bcmFieldNameToIdCount - 1);
    }
    if ((name_to_id_info->stage != bcmFieldStageCount) && (name_to_id_info->name_to_id_type != bcmFieldNameToIdContext))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only contexts require stage as input. Provided stage \"%s\" (%d).\n",
                     dnx_field_bcm_stage_text(name_to_id_info->stage), name_to_id_info->stage);
    }
    if ((name_to_id_info->name_to_id_type == bcmFieldNameToIdContext) && (name_to_id_info->stage == bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context requires stage as input.\n");
    }

    name_length = sal_strnlen((char *) name_to_id_info->name, BCM_FIELD_MAX_SHORT_NAME_LEN);
    if (name_length >= BCM_FIELD_MAX_SHORT_NAME_LEN)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name is not a NULL terminated string.\n");
    }
    if (name_length <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No name provided.\n");
    }
    if (name_length >= DBAL_MAX_STRING_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Name cannot be larger than %d.\n", DBAL_MAX_STRING_LENGTH);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function get's the ID of a field object (e.g. field group, qualifier, context) from its name.
 *  from hardware.
 * \param [in] unit            - Device ID
 * \param [in] flags           - Flags to indicate what action should be performed
 *                               by the API, currently no flag is used.
 * \param [in] name_to_id_info - All information about the object we find the ID for.
 * \param [out] nof_ids - The number of IDs returned for the name, number of elements in the array ids.
 * \param [out] ids              - The array of the IDs returned.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as
 *  shr_error_t
 * \remark
 *  None
 * \see
 *  Reference to:
 *  - bcm_dnx_field_entry_hit_get
 */
int
bcm_dnx_field_name_to_id(
    int unit,
    uint32 flags,
    bcm_field_name_to_id_info_t * name_to_id_info,
    int *nof_ids,
    uint32 ids[BCM_FIELD_NAME_TO_ID_MAX_IDS])
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Initialize nof_ids to indicate noting returned for the case of an error. Not mandatory.*/
    (*nof_ids) = 0;

    SHR_INVOKE_VERIFY_DNX(dnx_field_name_to_id_verify(unit, flags, name_to_id_info, nof_ids, ids));

    switch (name_to_id_info->name_to_id_type)
    {
        case bcmFieldNameToIdGroup:
        {
            dnx_field_group_t fg_id;
            SHR_IF_ERR_EXIT(dnx_field_group_name_to_id(unit, (char *) name_to_id_info->name, &fg_id));
            if (fg_id == DNX_FIELD_GROUP_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No field group with the name \"%.*s\" was found.\n",
                             BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
            }
            (*nof_ids) = 1;
            (*ids) = fg_id;
            break;
        }
        case bcmFieldNameToIdQualifier:
        {
            bcm_field_qualify_t bcm_qual;
            SHR_IF_ERR_EXIT(dnx_field_map_name_to_qual(unit, (char *) name_to_id_info->name, &bcm_qual));
            if (bcm_qual == bcmFieldQualifyCount)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No qualifier with the name \"%.*s\" was found.\n",
                             BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
            }
            (*nof_ids) = 1;
            (*ids) = bcm_qual;
            break;
        }
        case bcmFieldNameToIdAction:
        {
            bcm_field_action_t bcm_actions[DNX_FIELD_ACTION_MAX_ACTION_PER_VM];
            int nof_dnx_actions;
            int action_ndx;
            SHR_IF_ERR_EXIT(dnx_field_map_name_to_action
                            (unit, (char *) name_to_id_info->name, &nof_dnx_actions, bcm_actions));
            if (nof_dnx_actions < 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_dnx_actions is negative (%d) for the name \"%.*s\".\n",
                             nof_dnx_actions, BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
            }
            if ((nof_dnx_actions == 0) || (bcm_actions[0] == bcmFieldActionCount))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No action with the name \"%.*s\" was found.\n",
                             BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
            }
            if (nof_dnx_actions > DNX_FIELD_ACTION_MAX_ACTION_PER_VM)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_dnx_actions (%d) is larger then maximum %d for name \"%.*s\".\n",
                             nof_dnx_actions, DNX_FIELD_ACTION_MAX_ACTION_PER_VM,
                             BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
            }
            (*nof_ids) = nof_dnx_actions;
            for (action_ndx = 0; action_ndx < nof_dnx_actions; action_ndx++)
            {
                ids[action_ndx] = bcm_actions[action_ndx];
            }
            break;
        }
        case bcmFieldNameToIdContext:
        {
            if (name_to_id_info->stage == bcmFieldStageExternal)
            {
                bcm_field_AppType_t apptype;
                SHR_IF_ERR_EXIT(dnx_field_context_name_to_apptype(unit, (char *) name_to_id_info->name, &apptype));
                if (apptype == DNX_FIELD_APPTYPE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No Apptype with the name \"%.*s\" was found.\n",
                                 BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
                }
                (*nof_ids) = 1;
                (*ids) = apptype;
            }
            else
            {
                dnx_field_context_t context_id;
                dnx_field_stage_e dnx_stage;
                SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, name_to_id_info->stage, &dnx_stage));
                SHR_IF_ERR_EXIT(dnx_field_context_name_to_context
                                (unit, dnx_stage, (char *) name_to_id_info->name, &context_id));
                if (context_id == DNX_FIELD_CONTEXT_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No context with the name \"%.*s\" was found in stage \"%s\".\n",
                                 BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name,
                                 dnx_field_bcm_stage_text(name_to_id_info->stage));
                }
                (*nof_ids) = 1;
                (*ids) = context_id;
            }

            break;
        }
        case bcmFieldNameToIdAceFormat:
        {
            dnx_field_ace_id_t ace_id;
            SHR_IF_ERR_EXIT(dnx_field_ace_format_name_to_id(unit, (char *) name_to_id_info->name, &ace_id));
            if (ace_id == DNX_FIELD_ACE_FORMAT_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No ACE format with the name \"%.*s\" was found.\n",
                             BCM_FIELD_MAX_SHORT_NAME_LEN, name_to_id_info->name);
            }
            (*nof_ids) = 1;
            (*ids) = ace_id;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unknown type %d.\n", name_to_id_info->name_to_id_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify function for bcm_dnx_field_flush_entry_add().
 * \param [in] unit            - Device ID
 * \param [in] flags           - Currently no flag is used.
 * \param [in] fg_id           - The exact match field group to be affected by the flush entry.
 * \param [in] entry_id        - The ID of the flush entry in the TCAM. lower values will take precedence.
 * \param [in] flush_entry_info - The parameters of the flush entry to be added.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
static shr_error_e
dnx_field_flush_entry_add_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    int entry_id,
    bcm_field_flush_entry_info_t * flush_entry_info)
{
    dnx_field_group_type_e fg_type;
    int max_nof_qualifiers = MIN(BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP, DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);
    int max_nof_actions =
        MIN(BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP, DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP);
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_info, _SHR_E_PARAM, "flush_entry_info");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags are supported for flush entry add API. Flags provided are 0x%x.\n", flags);
    }

    SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id, &is_allocated));
    if (is_allocated == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Field Group %d does not exist. \n", fg_id);
    }

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    if (fg_type != DNX_FIELD_GROUP_TYPE_EXEM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Field group %d isn't an exact match field group.\n", fg_id);
    }

    if ((flush_entry_info->key.nof_entry_quals == BCM_FIELD_INVALID)
        || (flush_entry_info->key.nof_entry_actions == BCM_FIELD_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry key must be set with valid numbers of actions and qualifiers\n");
    }
    if (flush_entry_info->payload.nof_entry_actions == BCM_FIELD_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry payload must be set with a valid number of actions.\n");
    }
    if ((flush_entry_info->key.nof_entry_quals < 0) || (flush_entry_info->key.nof_entry_quals > max_nof_qualifiers))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry key number of qualifiers must be between 0 and %d \n", max_nof_qualifiers);
    }
    if ((flush_entry_info->key.nof_entry_actions < 0) || (flush_entry_info->key.nof_entry_actions > max_nof_actions))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry key number of actions must be between 0 and %d \n", max_nof_actions);
    }
    if ((flush_entry_info->payload.nof_entry_actions < 0)
        || (flush_entry_info->payload.nof_entry_actions > max_nof_actions))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Entry payload number of actions must be between 0 and %d \n", max_nof_actions);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds an entry to the flush machine used as part of the EXEM learn mechanism.
 * \param [in] unit            - Device ID
 * \param [in] flags           - Currently no flag is used.
 * \param [in] fg_id           - The exact match field group to be affected by the flush entry.
 * \param [in] entry_id        - The ID of the flush entry in the TCAM. lower values will take precedence.
 * \param [in] flush_entry_info - The parameters of the flush entry to be added.
 *                                Includes the EXEM entry to qualify upon, whether the entry was hit or not,
 *                                the command to be performed, and if the command is to transplant the entry
 *                                the part of the payload to change.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
int
bcm_dnx_field_flush_entry_add(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    int entry_id,
    bcm_field_flush_entry_info_t * flush_entry_info)
{
    dnx_field_stage_e dnx_field_stage;
    dnx_field_flush_entry_t dnx_flush_entry_info;
    uint32 dnx_flags;
    bcm_field_entry_action_t entry_add_bcm_action_info;
    dnx_field_entry_action_t entry_add_dnx_action_info;
    bcm_field_entry_qual_t entry_add_bcm_qual_info;
    dnx_field_entry_qual_t entry_add_dnx_qual_info;
    int qual_iter;
    int action_iter;
    int action_mask_iter;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_entry_add_verify(unit, flags, fg_id, entry_id, flush_entry_info));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &dnx_field_stage));

    SHR_IF_ERR_EXIT(dnx_field_flush_entry_t_init(unit, &dnx_flush_entry_info));

    /*
     * Prepare the entry key DNX struct, including qualifiers and actions of the EXEM entry to qualify upon.
     */
    sal_memset(&entry_add_bcm_action_info, 0x0, sizeof(entry_add_bcm_action_info));
    sal_memset(&entry_add_dnx_action_info, 0x0, sizeof(entry_add_dnx_action_info));
    sal_memset(&entry_add_bcm_qual_info, 0x0, sizeof(entry_add_bcm_qual_info));
    sal_memset(&entry_add_dnx_qual_info, 0x0, sizeof(entry_add_dnx_qual_info));
    for (action_iter = 0; action_iter < flush_entry_info->key.nof_entry_actions; action_iter++)
    {
        entry_add_bcm_action_info.type = flush_entry_info->key.entry_action[action_iter].type;
        sal_memcpy(entry_add_bcm_action_info.value, flush_entry_info->key.entry_action[action_iter].value,
                   sizeof(entry_add_bcm_action_info.value));
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_action_info_bcm_to_dnx
                                 (unit, BCM_CORE_ALL, dnx_field_stage, &entry_add_bcm_action_info,
                                  &entry_add_dnx_action_info),
                                 "Error for key BCM Action at index %d. Flush entry for field group %d.%s\n",
                                 action_iter, fg_id, EMPTY);
        dnx_flush_entry_info.key_info.action_info[action_iter].dnx_action = entry_add_dnx_action_info.dnx_action;
        sal_memcpy(dnx_flush_entry_info.key_info.action_info[action_iter].action_value,
                   entry_add_dnx_action_info.action_value,
                   sizeof(dnx_flush_entry_info.key_info.action_info[0].action_value));
        for (action_mask_iter = 0; action_mask_iter < BCM_FIELD_ACTION_WIDTH_IN_WORDS; action_mask_iter++)
        {
            dnx_flush_entry_info.key_info.action_info[action_iter].action_mask[action_mask_iter] =
                flush_entry_info->key.entry_action[action_iter].mask[action_mask_iter];
        }
        dnx_flush_entry_info.key_info.action_info[action_iter].valid_bit_value =
            (flush_entry_info->key.entry_action[action_iter].valid_bit_value) ? (0x1) : (0x0);
        dnx_flush_entry_info.key_info.action_info[action_iter].valid_bit_mask =
            (flush_entry_info->key.entry_action[action_iter].valid_bit_mask) ? (0x1) : (0x0);
    }
    for (qual_iter = 0; qual_iter < flush_entry_info->key.nof_entry_quals; qual_iter++)
    {
        entry_add_bcm_qual_info.type = flush_entry_info->key.entry_qual[qual_iter].type;
        sal_memcpy(entry_add_bcm_qual_info.value, flush_entry_info->key.entry_qual[qual_iter].value,
                   sizeof(entry_add_bcm_qual_info.value));
        sal_memcpy(entry_add_bcm_qual_info.mask, flush_entry_info->key.entry_qual[qual_iter].mask,
                   sizeof(entry_add_bcm_qual_info.mask));
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_qual_info_bcm_to_dnx
                                 (unit, BCM_CORE_ALL, dnx_field_stage, &entry_add_bcm_qual_info,
                                  &entry_add_dnx_qual_info),
                                 "Error for key BCM Qualifier at index %d. Flush entry for field group %d.%s\n",
                                 qual_iter, fg_id, EMPTY);
        dnx_flush_entry_info.key_info.qual_info[qual_iter].dnx_qual = entry_add_dnx_qual_info.dnx_qual;
        sal_memcpy(dnx_flush_entry_info.key_info.qual_info[qual_iter].qual_value, entry_add_dnx_qual_info.qual_value,
                   sizeof(dnx_flush_entry_info.key_info.qual_info[0].qual_value));
        sal_memcpy(dnx_flush_entry_info.key_info.qual_info[qual_iter].qual_mask, entry_add_dnx_qual_info.qual_mask,
                   sizeof(dnx_flush_entry_info.key_info.qual_info[0].qual_mask));
    }
    /*
     * Prepare the entry payload DNX struct, of the actions to change.
     */
    for (action_iter = 0; action_iter < flush_entry_info->payload.nof_entry_actions; action_iter++)
    {
        entry_add_bcm_action_info.type = flush_entry_info->payload.entry_action[action_iter].type;
        sal_memcpy(entry_add_bcm_action_info.value, flush_entry_info->payload.entry_action[action_iter].value,
                   sizeof(entry_add_bcm_action_info.value));
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_map_action_info_bcm_to_dnx
                                 (unit, BCM_CORE_ALL, dnx_field_stage, &entry_add_bcm_action_info,
                                  &entry_add_dnx_action_info),
                                 "Error for payload BCM Action at index %d. Flush entry for field group %d.%s\n",
                                 action_iter, fg_id, EMPTY);
        dnx_flush_entry_info.payload_info.action_info[action_iter].dnx_action = entry_add_dnx_action_info.dnx_action;
        sal_memcpy(dnx_flush_entry_info.payload_info.action_info[action_iter].action_value,
                   entry_add_dnx_action_info.action_value,
                   sizeof(dnx_flush_entry_info.payload_info.action_info[action_iter].action_value));
        for (action_mask_iter = 0; action_mask_iter < BCM_FIELD_ACTION_WIDTH_IN_WORDS; action_mask_iter++)
        {
            dnx_flush_entry_info.payload_info.action_info[action_iter].action_mask[action_mask_iter] =
                flush_entry_info->payload.entry_action[action_iter].mask[action_mask_iter];
        }
        dnx_flush_entry_info.payload_info.action_info[action_iter].valid_bit_value =
            (flush_entry_info->payload.entry_action[action_iter].valid_bit_value) ? (0x1) : (0x0);
        dnx_flush_entry_info.payload_info.action_info[action_iter].valid_bit_mask =
            (flush_entry_info->payload.entry_action[action_iter].valid_bit_mask) ? (0x1) : (0x0);
    }

    /*
     * Add hit bit to qualify upon.
     */
    dnx_flush_entry_info.key_info.hit_bit_value = (flush_entry_info->key.hit) ? (0x1) : (0x0);
    dnx_flush_entry_info.key_info.hit_bit_mask = (flush_entry_info->key.hit_mask) ? (0x1) : (0x0);

    /*
     * Add the commands.
     */
    dnx_flush_entry_info.payload_info.hit_bit_clear = (flush_entry_info->payload.hit_clear_cmd) ? (0x1) : (0x0);
    dnx_flush_entry_info.payload_info.delete = (flush_entry_info->payload.delete_cmd) ? (0x1) : (0x0);
    dnx_flush_entry_info.payload_info.transplant = (flush_entry_info->payload.transplant_cmd) ? (0x1) : (0x0);

    /*
     * Currently no flags are supported.
     */
    dnx_flags = 0;

    /*
     * Call DNX-level entry add API.
     */
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_entry_add(unit, dnx_flags, fg_id, entry_id, &dnx_flush_entry_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify function for bcm_dnx_field_flush_entry_get().
 * \param [in] unit            - Device ID
 * \param [in] flags           - Currently no flag is used.
 * \param [in] fg_id           - The exact match field group affected by the flush entry.
 * \param [in] entry_id        - The ID of the flush entry in the TCAM. lower values take precedence.
 * \param [in] flush_entry_info - The parameters of the flush entry.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
static shr_error_e
dnx_field_flush_entry_get_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    int entry_id,
    bcm_field_flush_entry_info_t * flush_entry_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(flush_entry_info, _SHR_E_PARAM, "flush_entry_info");

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags are supported for flush entry get API. Flags provided are 0x%x.\n", flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function for getting the parameter of a specific flush entry.
 * \param [in] unit            - Device ID
 * \param [in] flags           - Currently no flag is used.
 * \param [in] fg_id           - The exact match field group affected by the flush entry.
 * \param [in] entry_id        - The ID of the flush entry in the TCAM. lower values take precedence.
 * \param [out] flush_entry_info - The parameters of the flush entry.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
int
bcm_dnx_field_flush_entry_get(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    int entry_id,
    bcm_field_flush_entry_info_t * flush_entry_info)
{
    uint32 dnx_flags;
    dnx_field_flush_entry_t dnx_flush_entry_info;
    int key_qual_iter;
    int key_action_iter;
    int payload_action_iter;
    int value_mask_iter;
    dnx_field_stage_e dnx_field_stage;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Call verify function.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_entry_get_verify(unit, flags, fg_id, entry_id, flush_entry_info));

    /*
     * Init the output structure.
     */
    bcm_field_flush_entry_info_t_init(flush_entry_info);

    dnx_flags = 0;
    /*
     * Call DNX-level entry add API.
     */
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_entry_get(unit, dnx_flags, fg_id, entry_id, &dnx_flush_entry_info));

    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id, &dnx_field_stage));

    /*
     * Get the flush entry's key, both key and payload of the EXEM entry.
     */
    for (key_qual_iter = 0; key_qual_iter < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP &&
         dnx_flush_entry_info.key_info.qual_info[key_qual_iter].dnx_qual != DNX_FIELD_QUAL_TYPE_INVALID;
         key_qual_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_qual_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_flush_entry_info.key_info.qual_info[key_qual_iter].dnx_qual,
                         &(flush_entry_info->key.entry_qual[key_qual_iter].type)));
        for (value_mask_iter = 0; value_mask_iter < BCM_FIELD_QUAL_WIDTH_IN_WORDS; value_mask_iter++)
        {
            flush_entry_info->key.entry_qual[key_qual_iter].value[value_mask_iter] =
                dnx_flush_entry_info.key_info.qual_info[key_qual_iter].qual_value[value_mask_iter];
            flush_entry_info->key.entry_qual[key_qual_iter].mask[value_mask_iter] =
                dnx_flush_entry_info.key_info.qual_info[key_qual_iter].qual_mask[value_mask_iter];
        }
    }
    flush_entry_info->key.nof_entry_quals = key_qual_iter;
    for (key_action_iter = 0; key_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_flush_entry_info.key_info.action_info[key_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID;
         key_action_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                        (unit, dnx_field_stage, dnx_flush_entry_info.key_info.action_info[key_action_iter].dnx_action,
                         &(flush_entry_info->key.entry_action[key_action_iter].type)));
        for (value_mask_iter = 0; value_mask_iter < BCM_FIELD_ACTION_WIDTH_IN_WORDS; value_mask_iter++)
        {
            flush_entry_info->key.entry_action[key_action_iter].value[value_mask_iter] =
                dnx_flush_entry_info.key_info.action_info[key_action_iter].action_value[value_mask_iter];
            flush_entry_info->key.entry_action[key_action_iter].mask[value_mask_iter] =
                dnx_flush_entry_info.key_info.action_info[key_action_iter].action_mask[value_mask_iter];
        }
        flush_entry_info->key.entry_action[key_action_iter].valid_bit_value =
            dnx_flush_entry_info.key_info.action_info[key_action_iter].valid_bit_value;
        flush_entry_info->key.entry_action[key_action_iter].valid_bit_mask =
            dnx_flush_entry_info.key_info.action_info[key_action_iter].valid_bit_mask;
    }
    flush_entry_info->key.nof_entry_actions = key_action_iter;

    flush_entry_info->key.hit = dnx_flush_entry_info.key_info.hit_bit_value;
    flush_entry_info->key.hit_mask = dnx_flush_entry_info.key_info.hit_bit_mask;

    /*
     * Get the payload.
     */
    for (payload_action_iter = 0; payload_action_iter < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_ACTION_PER_GROUP &&
         dnx_flush_entry_info.payload_info.action_info[payload_action_iter].dnx_action != DNX_FIELD_ACTION_INVALID;
         payload_action_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_action_dnx_to_bcm
                        (unit, dnx_field_stage,
                         dnx_flush_entry_info.payload_info.action_info[payload_action_iter].dnx_action,
                         &(flush_entry_info->payload.entry_action[payload_action_iter].type)));
        for (value_mask_iter = 0; value_mask_iter < BCM_FIELD_ACTION_WIDTH_IN_WORDS; value_mask_iter++)
        {
            flush_entry_info->payload.entry_action[payload_action_iter].value[value_mask_iter] =
                dnx_flush_entry_info.payload_info.action_info[payload_action_iter].action_value[value_mask_iter];
            flush_entry_info->payload.entry_action[payload_action_iter].mask[value_mask_iter] =
                dnx_flush_entry_info.payload_info.action_info[payload_action_iter].action_mask[value_mask_iter];
        }
        flush_entry_info->payload.entry_action[payload_action_iter].valid_bit_value =
            dnx_flush_entry_info.payload_info.action_info[payload_action_iter].valid_bit_value;
        flush_entry_info->payload.entry_action[payload_action_iter].valid_bit_mask =
            dnx_flush_entry_info.payload_info.action_info[payload_action_iter].valid_bit_mask;
    }
    flush_entry_info->payload.nof_entry_actions = payload_action_iter;

    flush_entry_info->payload.hit_clear_cmd = dnx_flush_entry_info.payload_info.hit_bit_clear;
    flush_entry_info->payload.delete_cmd = dnx_flush_entry_info.payload_info.delete;
    flush_entry_info->payload.transplant_cmd = dnx_flush_entry_info.payload_info.transplant;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify function for bcm_dnx_field_flush_entry_delete().
 * \param [in] unit            - Device ID
 * \param [in] flags           - Currently no flag is used.
 * \param [in] fg_id           - The exact match field group affected by the flush entry.
 * \param [in] entry_id        - The ID of the flush entry in the TCAM. lower values take precedence. *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
static shr_error_e
dnx_field_flush_entry_delete_verify(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    int entry_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags are supported for flush entry delete API. Flags provided are 0x%x.\n",
                     flags);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function for getting the parameter of a specific flush entry.
 * \param [in] unit            - Device ID
 * \param [in] flags           - Currently no flag is used.
 * \param [in] fg_id           - The exact match field group affected by the flush entry.
 * \param [in] entry_id        - The ID of the flush entry in the TCAM. lower values take precedence.
 *
 * \return
 *  int - Error Type, in sense of bcm_error_t, same as shr_error_t
 * \remark
 *  None
 * \see
 *  None
 */
int
bcm_dnx_field_flush_entry_delete(
    int unit,
    uint32 flags,
    bcm_field_group_t fg_id,
    int entry_id)
{
    uint32 dnx_flags;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_flush_entry_delete_verify(unit, flags, fg_id, entry_id));

    /*
     * Currently no flags are supported.
     */
    dnx_flags = 0;

    /*
     * Call DNX-level entry add API.
     */
    SHR_IF_ERR_EXIT(dnx_field_flush_machine_entry_delete(unit, dnx_flags, fg_id, entry_id));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
