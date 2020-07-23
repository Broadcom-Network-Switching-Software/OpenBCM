/** \file mpls_tunnel_term.h
 *      General MPLS functionality for DNX tunnel termination. 
 *      Used by mpls.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _MPLS_TUNNEL_TERM_H_INCLUDED__
/*
 * {
 */
#define _MPLS_TUNNEL_TERM_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <bcm/mpls.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <bcm_int/dnx/mpls/mpls.h>
/*
 * TYPEDEFs
 * {
 */
/** actions supported by dnx_mpls_termination_lookup() */
typedef enum
{
    TERM_LOOKUP_SET,
    TERM_LOOKUP_GET,
    TERM_LOOKUP_DELETE,
    TERM_LOOKUP_FORCE_SET
} dnx_mpls_termination_lookup_action_e;

typedef enum
{
    TERM_MATCH_CRITERIA_LABEL,
    TERM_MATCH_CRITERIA_LABEL_2ND_PASS_DB,
    TERM_MATCH_CRITERIA_LABEL_BOS,
    TERM_MATCH_CRITERIA_TWO_LABELS,
    TERM_MATCH_CRITERIA_TWO_LABELS_2ND_PASS_DB,
    TERM_MATCH_CRITERIA_INTF_LABEL,
    TERM_MATCH_CRITERIA_INTF_LABEL_2ND_PASS_DB,
    TERM_MATCH_CRITERIA_VLAN_DOMAIN_LABEL,

    NOF_TERM_MATCH_CRITERIA
} dnx_mpls_termination_match_criteria_e;

/**
 * \brief Carry MPLS label match information for dnx_mpls_termination_lookup
 */
typedef struct dnx_mpls_term_match_s
{
    dnx_mpls_termination_match_criteria_e match_criteria;
    /**
     * \brief Always used
     */
    bcm_mpls_label_t label;
    /**
     * \brief Used when criteria == TERM_MATCH_CRITERIA_TWO_LABELS
     */
    bcm_mpls_label_t context_label;
    /**
     * \brief Used when criteria == TERM_MATCH_CRITERIA_INTF_LABEL
     */
    bcm_if_t context_intf;
    /**
     * \brief Used when criteria == TERM_MATCH_CRITERIA_LABEL_BOS
     */
    uint8 bos_val;
    /**
     * \brief Always used
     */
    uint32 service_tagged;
    /**
     * \brief Vlan domain
     */
    uint32 vlan_domain;
} dnx_mpls_term_match_t;

/*
 * Structure used for both MPLS module internal implementation and template MNGR MPLS algo
 */
typedef struct
{
    dbal_enum_value_field_labels_to_terminate_e labels_to_terminate;
    dbal_enum_value_field_ttl_exp_label_index_e ttl_exp_label_index;
    int reject_ttl_0;
    int reject_ttl_1;
    int has_cw;
    int expect_bos;
    int check_bos;
} dnx_mpls_termination_profile_t;

/*
 * }
 */
#if defined(INCLUDE_L3)
/* { */
/*
 * TYPEDEFs related to special labels.
 * {
 */
/*
 * }
 */
/*
 * DEFINEs related to special labels.
 * {
 */
/**
 * \brief
 *   Verify whether '_special_label_type' (of type the bcm_mpls_special_label_type_t) is valid for specified unit
 *   We assume that 'bcmMplsSpecialLabelTypeNone' is the first legal type
 *   We assume that 'bcmMplsSpecialLabelTypeFrr' is the last legal type
 */
#define DNX_MPLS_SPECIAL_LABEL_VERIFY(_unit,_special_label_type)                               \
    if (((int) (_special_label_type) > (int) (bcmMplsSpecialLabelTypeFrr)) ||                  \
                    ((int) (_special_label_type) < (int) (bcmMplsSpecialLabelTypeNone)))       \
    {                                                                                          \
        SHR_ERR_EXIT(_SHR_E_PARAM,                                                             \
                     "special_label_type (%d) is out of range (Should be between %d and %d)\r\n",   \
                     _special_label_type, bcmMplsSpecialLabelTypeNone, bcmMplsSpecialLabelTypeFrr); \
    }
/*
 * }
 */
/*
 * Procedures related to special labels.
 * {
 */
/**
 * \brief
 *   Initialize structure carrying info related to special label
 *
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [out] bcm_mpls_special_label_p -
 *   Pointer to a structure of type 'bcm_mpls_special_label_t' to
 *   be initialized.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_mpls_special_label_identifier_traverse
 */
shr_error_e dnx_bcm_mpls_special_label_t_init(
    int unit,
    bcm_mpls_special_label_t * bcm_mpls_special_label_p);
/*
 * }
 */
/*
 * Procedures related to FRR feature.
 * {
 */
/**
 * \brief
 *   Add one entry (on FRR label on TCAM, MPLS_FRR_TCAM_DB)
 *
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_info_p -
 *   Pointer to a structure of type 'bcm_mpls_special_label_t'. This
 *   procedure only uses the 'label_value' which is an identifier of
 *   the MPLS label to add to the table.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_mpls_frr_label_identifier_traverse
 */
shr_error_e dnx_mpls_frr_label_identifier_add(
    int unit,
    bcm_mpls_special_label_t * label_info_p);
/**
 * \brief
 *   Get indication on whether an entry with indicated MPLS label exists (on
 *   FRR label on TCAM, MPLS_FRR_TCAM_DB)
 *
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_info_p -
 *   Pointer to a structure of type 'bcm_mpls_special_label_t'. This
 *   procedure only uses the 'label_value' which is an identifier of
 *   the MPLS label to search for on the table.
 *   If search is successful, this procedure returns without an error.
 *   else (in case of error)
 *     If specified 'label_value' is not found, no error log is ejected but
 *     return value is '_SHR_E_NOT_FOUND'
 *     Otherwise error log is ejected and returned error is as encountered.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e. If indicated label is not
 *     found, the error '_SHR_E_NOT_FOUND' is returned. No error log will be
 *     ejected in this case.
 * \remark
 *   None.
 * \see
 *   * bcm_dnx_mpls_special_label_identifier_get
 */
shr_error_e dnx_mpls_frr_label_identifier_get(
    int unit,
    bcm_mpls_special_label_t * label_info_p);
/**
 * \brief
 *   Delete an entry with indicated MPLS label (on FRR label on TCAM,
 *   MPLS_FRR_TCAM_DB)
 *
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] label_info_p -
 *   Pointer to a structure of type 'bcm_mpls_special_label_t'. This
 *   procedure only uses the 'label_value' which is an identifier of
 *   the MPLS label to search for on the table.
 *   If entry is not found (or some other error is encountered) error log
 *   is ejected and returned error is as encountered.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e.
 * \remark
 *   None.
 * \see
 *   * bcm_dnx_mpls_special_label_identifier_get
 */
shr_error_e dnx_mpls_frr_label_identifier_delete(
    int unit,
    bcm_mpls_special_label_t * label_info_p);

shr_error_e dnx_mpls_frr_label_identifier_delete_all(
    int unit);

/**
 * \brief
 *   Traverse over all entries (MPLS labels) on FRR label on TCAM,
 *   MPLS_FRR_TCAM_DB, and invoke call-back procedure per each.
 *
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] cb -
 *   Pointer to call-back procedure. This procedure goes through all
 *   entries of specified table and invokes this procedure per each.
 * \param [in] user_data_p -
 *   Pointer to general data. This procedure passes this pointer to
 *   the call-back procedure, 'cb'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e.
 * \remark
 *   None.
 * \see
 *   * bcm_dnx_mpls_special_label_identifier_traverse
 */
shr_error_e dnx_mpls_frr_label_identifier_traverse(
    int unit,
    bcm_mpls_special_label_identifier_traverse_cb cb,
    void *user_data_p);
/*
 * }
 */
/* } */
#endif /* INCLUDE_L3 */

/**
 * \brief Added verification for MPLS tunnel termination.
 * See dnx_mpls_tunnel_switch_term_add for more details.
 */
shr_error_e dnx_mpls_tunnel_switch_term_add_verify(
    int unit,
    bcm_mpls_tunnel_switch_t * info);


shr_error_e dnx_mpls_tunnel_switch_term_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info);

/**
* \brief
*  Get the created MPLS tunnels for mpls termination.
* \par DIRECT INPUT:
*    \param [in] unit -Relevant unit.
*    \param [in] local_inlif - pointer to the in-LIF entry in the inlif table.
*    \param [out] info -A pointer to a struct to be filled according the termination information.
*   info.label - A label according to which the lookup will be done.
*   info.tunnel_id - global_if in case of termination
*   info.port - destination port
* \par INDIRECT INPUT:
*   * \b *info \n
*     See 'info' in DIRECT INPUT above
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \par INDIRECT OUTPUT:
*   * See \b 'info' in DIRECT INPUT
*/
shr_error_e dnx_mpls_tunnel_switch_term_get(
    int unit,
    int local_inlif,
    bcm_mpls_tunnel_switch_t * info);

/**
* \brief
*  Delete MPLS tunnels for mpls termination.
*   \param [in] unit -Relevant unit.
*   \param [in] local_inlif - pointer to the in-lif to be deleted.
*   \param [in] info  - Ingress MPLS entry to be deleted.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
shr_error_e dnx_mpls_tunnel_switch_term_delete(
    int unit,
    int local_inlif,
    bcm_mpls_tunnel_switch_t * info);

/**
 * \brief
 *    Delete all MPLS tunnels for mpls termination.
 *
 * \param [in] unit       -  device unit number.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref bcm_error_t
 * \remark
 *   None.
 * \see
 *   * None
 */
shr_error_e dnx_mpls_tunnel_switch_term_delete_all(
    int unit);

/**
 * \brief
 *   Extract the match information and criteria from a tunnel switch structure
 * \param [in] unit
 * \param [in] tunnel - input structure from an API
 * \param [out] match - result match criteria and information for dnx_mpls_termination_lookup
 */
shr_error_e dnx_mpls_term_match_criteria_from_tunnel_switch_get(
    int unit,
    bcm_mpls_tunnel_switch_t * tunnel,
    dnx_mpls_term_match_t * match);

/**
* \brief
* Add/get/delete MPLS termination label to either 
* DBAL_TABLE_MPLS_TERMINATION_SINGLE_LABEL_DB or 
* DBAL_TABLE_MPLS_TERMINATION_CONTEXT_SPECIFIC_LABEL_DB table. 
* This function configures ISEM lookup and is done for all cores. 
*   \param [in] unit    -  Relevant unit.
*   \param [in] action - perform SET/GET/DELETE on the table.
*   \param [in] match_info  -  Match struct holding the match criteria and the match information.
*   \param [in] local_in_lif   -  Local lif to be saved or looked up.
* \return
*   shr_error_e - Non-zero in case of an error.
*/
shr_error_e dnx_mpls_termination_lookup(
    int unit,
    dnx_mpls_termination_lookup_action_e action,
    dnx_mpls_term_match_t * match_info,
    int *local_in_lif);

/**
 * \brief 
 * Write to DBAL_TABLE_INGRESS_MPLS_TERMINATION_PROFILE table.
 * \param [in] unit - Relevant unit. 
 * \param [in] term_profile_info - A pointer to a struct containing relevant information for the Term Profile.
 * \param [in] term_profile - Termination Profile number, used as index to the table.
 * \return
 *   Error indication according to shr_error_e enum
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_termination_profile_hw_set(
    int unit,
    dnx_mpls_termination_profile_t * term_profile_info,
    int term_profile);

/**
 * \brief
 * Delete the termination profile entry form the HW
 *
 * \param [in] unit - Relevant unit. 
 * \param [in] term_profile - Termination profile number to be deleted
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_hw_termination_profile_delete(
    int unit,
    uint32 term_profile);

/**
 * \brief 
 * Check whether the termination profile was already allocated 
 * using the template allocation algorithm, if not allocate new 
 * profile. 
 *
 * \param [in] unit - Relevant unit. 
 * \param [in] term_profile_info - A struct containing relevant information for the Term Profile.
 * \param [out] term_profile                     -  Termination profile used as reference
 * \param [in] old_term_profile                     -  Termination profile used as reference - used in case exchange is needed
 * \param [out] is_first_term_profile_reference  -  Returns 1 if the profile is the first reference
 * \param [out] is_last_term_profile_reference  -  Returns 1 if the profile is the last reference - used in case exchange is needed
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_mpls_termination_profile_allocation(
    int unit,
    dnx_mpls_termination_profile_t term_profile_info,
    int *term_profile,
    int old_term_profile,
    uint8 *is_first_term_profile_reference,
    uint8 *is_last_term_profile_reference);

/**
 * \brief
 * Traverse all MPLS tunnels in the ingress for mpls termination and run a callback function
 * provided by the user for each one.
 * \param [in] unit - The unit number.
 * \param [in] cb   - A pointer to callback function,
 *          it receives the value of the user_data variable and
 *          all MPLS tunnel objects that were iterated
 * \param [in] user_data - user data that will be sent to the callback function
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \see
 */
shr_error_e dnx_mpls_tunnel_switch_term_traverse(
    int unit,
    bcm_mpls_tunnel_switch_traverse_cb cb,
    void *user_data);
/*
 * }
 */
#endif/*_MPLS_TUNNEL_TERM_H_INCLUDED__*/
