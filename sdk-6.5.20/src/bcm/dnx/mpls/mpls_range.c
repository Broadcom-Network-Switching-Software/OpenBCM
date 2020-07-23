/** \file mpls_range.c IML-Range DB management for MPLS based EVPN.
 * This file contains the implementation and verification of the IML-Range DB
 * management APIs for MPLS based EVPN multicast encapsulated traffic.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <bcm/types.h>
#include <bcm/mpls.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mpls.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_mpls_access.h>

/*
 * Internal helpers for API Implementation
 * {
 */
/*
 * Utilities
 * {
 */
/**
 * See header in mpls_range.h
 */
shr_error_e
dnx_mpls_label_iml_range_check(
    int unit,
    int num_labels,
    bcm_mpls_label_t * in_labels_p,
    int *is_iml_p,
    bcm_mpls_label_t * found_labels_low_p,
    bcm_mpls_label_t * found_labels_high_p)
{
    uint32 entry_handle_id;
    int is_end;
    int num_label;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Validity and initializations.
     */
    SHR_NULL_CHECK(is_iml_p, _SHR_E_INTERNAL, "is_iml_p");
    SHR_NULL_CHECK(in_labels_p, _SHR_E_INTERNAL, "in_labels_p");
    SHR_NULL_CHECK(found_labels_low_p, _SHR_E_INTERNAL, "found_labels_low_p");
    SHR_NULL_CHECK(found_labels_high_p, _SHR_E_INTERNAL, "found_labels_high_p");
    *is_iml_p = FALSE;
    /*
     * Not required but cleaner.
     */
    *found_labels_low_p = *found_labels_high_p = 0;
    /*
     * Check 'sw state' image of loaded HW.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        uint32 low, high;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_LOW, INST_SINGLE, &low));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_HIGH, INST_SINGLE, &high));
        for (num_label = 0; num_label < num_labels; num_label++)
        {
            if ((in_labels_p[num_label] >= (bcm_mpls_label_t) low)
                && (in_labels_p[num_label] <= (bcm_mpls_label_t) high))
            {
                *is_iml_p = TRUE;
                *found_labels_low_p = low;
                *found_labels_high_p = high;
                SHR_EXIT();
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Check whether there is an mpls range with specified labels.
 *   a. If FALSE, indicate on '*found'
 *   b. If TRUE, indicate on '*found_p', specify its profile id in '*range_profile_p'
 *      and indicate whether it is IML in '*is_iml_p'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] in_label_low -
 *   Lowest MPLS label on range.
 * \param [in] in_label_high -
 *   Highest MPLS label on range.
 * \param [out] found_p -
 *   Pointer to int. 
 *   This procedure loads pointed memory by TRUE if specified range has been
 *   found. Otherwise, it is loaded by FALSE.
 * \param [out] is_iml_p -
 *   Pointer to int. 
 *   This procedure loads pointed memory by TRUE if specified range has been
 *   found and its RANGE_PROFILE_QUALIFIER is IML.
 * \param [out] range_profile_p -
 *   Pointer to int. 
 *   This procedure loads pointed memory by the matching range profile ('key')
 *   if specified range has been found.
 * \return
 *   Negative in case of an error.
 *   Zero in case of NO ERROR
 * \see
 *   * dnx_mpls_label_iml_range_check
 * \remark
 *   * None
 */
static shr_error_e
dnx_mpls_label_range_exists(
    int unit,
    bcm_mpls_label_t in_label_low,
    bcm_mpls_label_t in_label_high,
    int *found_p,
    int *is_iml_p,
    uint32 *range_profile_p)
{
    uint32 entry_handle_id;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Validity and initializations.
     */
    SHR_NULL_CHECK(is_iml_p, _SHR_E_INTERNAL, "is_iml_p");
    SHR_NULL_CHECK(found_p, _SHR_E_INTERNAL, "found_p");
    SHR_NULL_CHECK(range_profile_p, _SHR_E_INTERNAL, "range_profile_p");
    *found_p = FALSE;
    /*
     * Not required but cleaner.
     */
    *is_iml_p = FALSE;
    *range_profile_p = -1;
    /*
     * Check 'sw state' image of loaded HW.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        uint32 low, high;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_LOW, INST_SINGLE, &low));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_HIGH, INST_SINGLE, &high));
        {
            if ((in_label_low == (bcm_mpls_label_t) low) && (in_label_high == (bcm_mpls_label_t) high))
            {
                *found_p = TRUE;
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_RANGE_PROFILE, range_profile_p));
                *is_iml_p = TRUE;
                SHR_EXIT();
            }
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
static shr_error_e
dnx_mpls_range_action_evpn_iml_add(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high)
{
    uint32 entry_handle_id;
    int range_profile = 0;
    int feature_mpls_term_1_or_2_labels;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate entry
     */
    SHR_IF_ERR_EXIT(algo_mpls_db.label_range_profile.allocate_single(unit, 0, NULL, &range_profile));
    /*
     * Set in SW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_PROFILE, range_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_LOW, INST_SINGLE, label_low);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_HIGH, INST_SINGLE, label_high);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Set in HW
     */
    feature_mpls_term_1_or_2_labels =
        dnx_data_mpls.general.feature_get(unit, dnx_data_mpls_general_mpls_term_1_or_2_labels);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_PROFILE, range_profile);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_LOW, INST_SINGLE, label_low);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_LABEL_HIGH, INST_SINGLE, label_high);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_QUALIFIER_LABEL_RANGE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_MPLS_QUALIFIER_LABEL_RANGE_IML_LABEL_RANGE);
    if (feature_mpls_term_1_or_2_labels)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS_MASK, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS_VALUE, INST_SINGLE, FALSE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BOS_MASK, INST_SINGLE, TRUE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Given its key (range profile), remove an IML entry from HW and
 *   from sw-state.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] range_profile -
 *   Range profile ('key') of entry to remove
 * \return
 *   Negative in case of an error.
 *   Zero in case of NO ERROR
 * \see
 *   * dnx_mpls_label_iml_range_check
 * \remark
 *   * Caller is assumed to have verified that input key is valid and
 *     relates to an IML entry.
 */
static shr_error_e
dnx_mpls_range_action_evpn_iml_remove(
    int unit,
    uint32 range_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Free entry
     */
    SHR_IF_ERR_EXIT(algo_mpls_db.label_range_profile.free_single(unit, range_profile));
    /*
     * Remove from SW state
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_INFO_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_PROFILE, range_profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Set in HW
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MPLS_LABEL_RANGE_PROPERTIES, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RANGE_PROFILE, range_profile);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * }
 * Utilities
 */
/*
 * Verifiers
 * {
 */

/**
 * \brief
 *   Verify suitability of input mpls range to form a range.
 *   Just check that each is a ligitimate MPLS label and that
 *   the first is smaller than the second.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_low -
 *   First (lowest) MPLS label to verify.
 * \param [in] label_high -
 *   Second (highest) MPLS label to verify.
 * \return
 *   Negative in case of an error.
 *   Zero in case of NO ERROR
 * \see
 *   * dnx_mpls_range_action_remove_verify
 * \remark
 *   * None
 */
static shr_error_e
dnx_mpls_range_labels_verify(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((label_low <= (bcm_mpls_label_t) MPLS_MAX_RESERVED_LABEL) || (!DNX_MPLS_LABEL_IN_RANGE(unit, label_low)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "label_low (%d) is out of range for MPLS label within 'range'. Allowed range is: %d - %d, incl. edges.\r\n",
                     label_low, (MPLS_MAX_RESERVED_LABEL + 1), DNX_MPLS_LABEL_MAX(unit));
    }
    if ((label_high <= (bcm_mpls_label_t) MPLS_MAX_RESERVED_LABEL) || (!DNX_MPLS_LABEL_IN_RANGE(unit, label_high)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "label_high (%d) is out of range for MPLS label within 'range'. Allowed range is: %d - %d, incl. edges.\r\n",
                     label_high, (MPLS_MAX_RESERVED_LABEL + 1), DNX_MPLS_LABEL_MAX(unit));
    }
    if (label_low > label_high)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "label_high (%d) cannot be smaller than label_low (%d)\r\n", label_high, label_low);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief Verify parameters for range action add
 */
static shr_error_e
dnx_mpls_range_action_add_verify(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t * action_p)
{
    bcm_mpls_label_t *in_labels_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(action_p, _SHR_E_PARAM, "action_p");
    SHR_IF_ERR_EXIT(dnx_mpls_range_labels_verify(unit, label_low, label_high));
    {
        /*
         * Verify the range do not exist
         */
        int is_found = 0;
        int is_iml = 0;
        uint32 range_profile = 0;
        SHR_IF_ERR_EXIT(dnx_mpls_label_range_exists(unit, label_low, label_high, &is_found, &is_iml, &range_profile));
        if (is_found)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "\r\nMPLS Range (%d, %d) already exists.\r\n", label_low, label_high);
        }
    }
    {
        /*
         * Verify new range does not overlap a range that is already in the tables.
         */
        int num_labels = 2;
        bcm_mpls_label_t found_labels_low, found_labels_high;
        int is_iml;
        SHR_ALLOC(in_labels_p, sizeof(*in_labels_p) * num_labels, "in_labels_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        /*
         * The following two lines are not required but are added to silence compiler's warning.
         */
        is_iml = FALSE;
        found_labels_low = found_labels_high = 0;
        in_labels_p[0] = label_low;
        in_labels_p[1] = label_high;
        SHR_IF_ERR_EXIT(dnx_mpls_label_iml_range_check
                        (unit, num_labels, in_labels_p, &is_iml, &found_labels_low, &found_labels_high));
        if (is_iml != FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "One or more of specified labels (%d, %d) are within an IML range which is already in HW (%d,%d). Quit.\r\n",
                         label_low, label_high, found_labels_low, found_labels_high);
        }
    }
    {
        /*
         * Verify action
         */
        uint32 allowed_flags;
        /*
         * Here set bit map of all allowed flags.
         */
        allowed_flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
        if ((action_p->flags & allowed_flags) == 0)
        {
            /*
             * None of allowed flags is specified.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "None of allowed flags (0x%08X) is specified. Input flags: 0x%08X\r\n",
                         allowed_flags, action_p->flags);
        }
    }
    {
        /*
         * Verify the table has free entries
         */
        int nof_free = 0;
        SHR_IF_ERR_EXIT(algo_mpls_db.label_range_profile.nof_free_elements_get(unit, &nof_free));
        if (nof_free < 1)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "\r\nMPLS range table is full.\r\n");
        }
    }
exit:
    SHR_FREE(in_labels_p);
    SHR_FUNC_EXIT;
}
/**
 * \brief Verify parameters for range action remove
 */
static shr_error_e
dnx_mpls_range_action_remove_verify(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mpls_range_labels_verify(unit, label_low, label_high));
exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 * Verifiers
 */
/*
 * }
 */
/*
 * API Implementation
 * {
 */

/**
 * \brief
 *   Add an entry to a table related to 'MPLS range' depending on
 *   specified 'action'. See 'remark' below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_low -
 *   Lowest MPLS label for this range. This label, itself, is included in the range.
 * \param [in] label_high -
 *   Highest MPLS label for this range. This label, itself, is included in the range.
 * \param [in] action -
 *   Pointer to a structure of type 'bcm_mpls_range_action_t'. 
 *   Only element 'flags' is used. Element 'compressed_label' is ignored.
 *   Element 'flags' is a bit map. Currently, the list of relevent flags:
 *     BCM_MPLS_RANGE_ACTION_EVPN_IML
 * \return
 *   Negative in case of an error. See \ref shr_error_e, for example: 'MPLS range
 *   overlaps an existing range' or 'no space on tcam'.
 *   Zero in case of NO ERROR
 * \see
 *   * dnx_mpls_range_action_evpn_iml_add()
 *   * DBAL 'sw state' table: MPLS_LABEL_RANGE_INFO_SW
 *       (tools/autocoder/dbal/dnx/tables/tables_sw_state/PP/mpls/sw_pp_mpls_esi.xml)
 *   * DBAL 'hw' table: MPLS_LABEL_RANGE_PROPERTIES
 *       (./tools/autocoder/dbal/dnx/tables/tables_hard_logic/PP/mpls/hl_pp_mpls.xml)
 *   * HW tables:
 *   * * IPPA_VTT_MPLS_LABEL_RANGE_TABLE
 *   * * IPPF_VTT_MPLS_LABEL_RANGE_TABLE
 *   * * IPPA_VTT_MPLS_LABEL_RANGE_PROFILE
 *   * * IPPF_VTT_MPLS_LABEL_RANGE_PROFILE
 * \remark
 *   * Currently supported actions (as per 'action->flags'):
 *   * * BCM_MPLS_RANGE_ACTION_EVPN_IML
 */
int
bcm_dnx_mpls_range_action_add(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t * action)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_mpls_range_action_add_verify(unit, label_low, label_high, action));
    /*
     * Treat the various values of 'flags' as an enumration.
     * We assume only one can be set, at a time.
     */
    switch (action->flags)
    {
        case BCM_MPLS_RANGE_ACTION_EVPN_IML:
        {
            SHR_IF_ERR_EXIT(dnx_mpls_range_action_evpn_iml_add(unit, label_low, label_high));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "\r\n"
                         "IML bit (0x%08X) is not specified on input flags (0x%08X).\r\n"
                         "==> Currently, only IML is supported.\r\n", BCM_MPLS_RANGE_ACTION_EVPN_IML, action->flags);
        }
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Get indication on whether an entry exists, with specified MPLS-labels range,
 *   and whether its RANGE_PROFILE_QUALIFIER is of type EVPN_IML . See 'remark' below.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_low -
 *   Lowest MPLS label for this range. This label, itself, is included in the range.
 * \param [in] label_high -
 *   Highest MPLS label for this range. This label, itself, is included in the range.
 * \param [out] action_p -
 *   Pointer to a structure of type 'bcm_mpls_range_action_t'. 
 *   Only element 'flags' is used. All other elements are ignored.
 *   Element 'action->flags' is a bit map.
 *   If specified range is found, and its RANGE_PROFILE_QUALIFIER is of type EVPN_IML,
 *     then the bit BCM_MPLS_RANGE_ACTION_EVPN_IML is set on 'action->flags'.
 *   Otherwise, 'action->flags' is not touched.
 * \return
 *   Negative in case of an error. See \ref shr_error_e. If MPLS range is not found,
 *   then '_SHR_E_NOT_FOUND' is returned. See 'remark' below.
 *   Zero in case of NO ERROR. In that case, specified range has been found.
 * \see
 *   * dnx_mpls_range_action_evpn_iml_add()
 *   * DBAL 'sw state' table: MPLS_LABEL_RANGE_INFO_SW
 *       (tools/autocoder/dbal/dnx/tables/tables_sw_state/PP/mpls/sw_pp_mpls_esi.xml)
 *   * DBAL 'hw' table: MPLS_LABEL_RANGE_PROPERTIES
 *       (./tools/autocoder/dbal/dnx/tables/tables_hard_logic/PP/mpls/hl_pp_mpls.xml)
 *   * HW tables:
 *   * * IPPA_VTT_MPLS_LABEL_RANGE_TABLE
 *   * * IPPF_VTT_MPLS_LABEL_RANGE_TABLE
 *   * * IPPA_VTT_MPLS_LABEL_RANGE_PROFILE
 *   * * IPPF_VTT_MPLS_LABEL_RANGE_PROFILE
 * \remark
 *   * If specified range is not found then '_SHR_E_NOT_FOUND' is returned but NO
 *     error log is ejected. This is up to the caller.
 */
int
bcm_dnx_mpls_range_action_get(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high,
    bcm_mpls_range_action_t * action_p)
{
    int found;
    int is_iml;
    uint32 range_profile;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(action_p, _SHR_E_PARAM, "action_p");
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_range_labels_verify(unit, label_low, label_high));
    SHR_IF_ERR_EXIT(dnx_mpls_label_range_exists(unit, label_low, label_high, &found, &is_iml, &range_profile));
    if (found == FALSE)
    {
        /*
         * If specified range has not been found then return with this error but
         * do not eject an error log. This is up to the caller.
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    if (is_iml == TRUE)
    {
        /*
         * If specified range has been found  and it os IML then set
         * the BCM_MPLS_RANGE_ACTION_EVPN_IML bit in 'action->flags'.
         * Note that we do not reset it in the other case, we just
         * leave it as is.
         */
        action_p->flags |= BCM_MPLS_RANGE_ACTION_EVPN_IML;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Delete MPLS-range entry:
 *   a. Look for an entry with specified MPLS-labels range.
 *   b. If found, delete it from HW and from SW image.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_low -
 *   Lowest MPLS label for this range. This label, itself, is included in the range.
 * \param [in] label_high -
 *   Highest MPLS label for this range. This label, itself, is included in the range.
 * \return
 *   Negative in case of an error. See \ref shr_error_e. If MPLS range is not found,
 *   then '_SHR_E_NOT_FOUND' is returned.
 *   Zero in case of NO ERROR
 * \see
 *   * dnx_mpls_range_action_evpn_iml_add()
 *   * DBAL 'sw state' table: MPLS_LABEL_RANGE_INFO_SW
 *       (tools/autocoder/dbal/dnx/tables/tables_sw_state/PP/mpls/sw_pp_mpls_esi.xml)
 *   * DBAL 'hw' table: MPLS_LABEL_RANGE_PROPERTIES
 *       (./tools/autocoder/dbal/dnx/tables/tables_hard_logic/PP/mpls/hl_pp_mpls.xml)
 *   * HW tables:
 *   * * IPPA_VTT_MPLS_LABEL_RANGE_TABLE
 *   * * IPPF_VTT_MPLS_LABEL_RANGE_TABLE
 *   * * IPPA_VTT_MPLS_LABEL_RANGE_PROFILE
 *   * * IPPF_VTT_MPLS_LABEL_RANGE_PROFILE
 * \remark
 *   * In all error cases, an error log is ejected in addition to the returned error.
 */
int
bcm_dnx_mpls_range_action_remove(
    int unit,
    bcm_mpls_label_t label_low,
    bcm_mpls_label_t label_high)
{
    int found;
    int is_iml;
    uint32 range_profile[1];

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    SHR_INVOKE_VERIFY_DNX(dnx_mpls_range_action_remove_verify(unit, label_low, label_high));
    SHR_IF_ERR_EXIT(dnx_mpls_label_range_exists(unit, label_low, label_high, &found, &is_iml, range_profile));
    if (found == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND,
                     "\r\n" "Specified range (%d,%d) is not on MPLS-ranges table. Quit.\r\n", label_low, label_high);
    }
    if (is_iml == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "\r\n"
                     "Specified RANGE_PROFILE_QUALIFIER is not IML. Currently, only IML is supported. Quit.\r\n");
    }
    /*
     * At this point, entry has been found and it is IML. Key is 'range_profile'
     */
    SHR_IF_ERR_EXIT(dnx_mpls_range_action_evpn_iml_remove(unit, range_profile[0]));
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * }
 * API Implementation
 */
