/**
 * \file bcm_int/dnx/field/field_kbp.h
 *
 * Field Field KBP configurations for DNX
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_KBP_H_INCLUDED
/* { */
#define FIELD_FIELD_KBP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif

#if defined(INCLUDE_KBP)

/*
 * Include files
 * {
 */
#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_map.h>
#include <include/bcm_int/dnx/field/field_group.h>
#include <include/bcm_int/dnx/field/field_context.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_kbp_types.h>

/*
 * }
 */
/*
 * Defines
 * {
 */

/*
 * }
 */
/*
 * Typedefs
 * {
 */
/*
 * }
 */

/*
 * Prototypes
 * {
 */

/**
 * \brief
 *  Verify function for external TCAM input type attach info
 * \param [in] unit             - Device Id
 * \param [in] field_stage      - For which Field stage to Configure HW
 * \param [in] qual_type        - Qual Type - includes Encoded qualifier
 * \param [in] qual_attach_info_p - info from attach info.
 * \return
 *   shr_error_e               - Error Type
 * \remark
 *   * None.
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_qual_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_attach_info_p);

/**
 * \brief
 *  This function compares two qualifier arrays for KBP to check if they can be shared.
 *
 * \param [in] unit                  - Device Id
 * \param [in] nof_quals_1           - Number of qualifiers in first array.
 * \param [in] dnx_qual_1            - First qualifier array
 * \param [in] qual_attach_info_1    - First qualifier array's attach info
 * \param [in] nof_quals_2           - Number of qualifiers in second array.
 * \param [in] dnx_qual_2            - Second qualifier array
 * \param [in] qual_attach_info_2    - Second qualifier array's attach info
 * \param [out] fully_shareable_p          - Whether or not the qualifiers can be shared.
 * \param [out] partially_shareable_p      - Whether or not dnx_qual_2 can share part of dnx_qual_1.
 * \param [out] partial_sharing_lsb_offset_p - If we can do partial sharing, the offset from the lsb
 *                                             of dnx_qual_1 for sharing nof_quals_2.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_acl_compare_qualifiers_for_sharing(
    int unit,
    int nof_quals_1,
    dnx_field_qual_t dnx_qual_1[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_1[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    int nof_quals_2,
    dnx_field_qual_t dnx_qual_2[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_2[DNX_FIELD_KBP_NOF_QUALS_PER_SEGMENT],
    int *fully_shareable_p,
    int *partially_shareable_p,
    int *partial_sharing_lsb_offset_p);

/**
 * \brief
 *  This function provides the keys and the LSB psoition of the last key of a part of the master key,
 *  given offset from MSB on the amster key and size in bytes.
 *
 * \param [in] unit             - Device Id
 * \param [in] opcode_id        - The opcode id
 * \param [in] offset_on_master_from_msb - The offset from MSB on the master key.
 * \param [in] size_on_key      - The bits of the chunk we refer to.
 * \param [out] key_kbp_id_last_p - The last key (where the lsb sits on).
 * \param [out] keys_bmp_p      - The bitmap of the keys the chunk sits on.
 * \param [out] offset_on_last_key_p - The offset from lsb on the last key.
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_master_offset_to_key(
    int unit,
    uint32 opcode_id,
    int offset_on_master_from_msb,
    int size_on_key,
    dbal_enum_value_field_kbp_kbr_idx_e * key_kbp_id_last_p,
    uint32 *keys_bmp_p,
    int *offset_on_last_key_p);

 /**
 * \brief
 *  Configure the KBP opcode on the required contexts.
 * \param [in] unit        - Device Id
 * \param [in] opcode_id   - Opcode ID (AppType).
 * \param [in] fwd_context - FWD context ID.
 * \param [in] acl_context_id - ACL Context ID.
 * \param [in] kbp_opcode_info_p  - structure that holds KBP info, including the array of segments to configure.
 *                           The information given per segment is the fg_idx and pointers(indexes) into the fg_info.
 *                           We need those pointers in order to extract the attach information about a given qualifier.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_kbp_set(
    int unit,
    uint32 opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_kbp_opcode_info_t * kbp_opcode_info_p);

/**
 * \brief
 *  This function converts the FWD2 (external stage) KBR form values 16-17 to
 *  KBP KBR values 2-3.
 *
 * \param [in] unit          - Device Id
 * \param [in] key_id        - Key Id
 * \param [out] kbp_kbr_id_p - Converted kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * dnx_field_kbp_key_acl_to_fwd2_key_id_convert
 * \see
 *   * dnx_field_key_fwd2_to_acl_key_id_convert
 */
shr_error_e dnx_field_kbp_key_fwd2_to_acl_key_id_convert(
    int unit,
    dbal_enum_value_field_field_key_e key_id,
    dbal_enum_value_field_kbp_kbr_idx_e * kbp_kbr_id_p);

/**
 * \brief
 *  This function converts the KBP KBR values 2-3 to FWD2 (external stage)
 *  KBR form values 16-17.
 *
 * \param [in] unit          - Device Id
 * \param [in] kbp_kbr_id        - KBP KBR Id
 * \param [out] key_id_p - Converted FWD2 kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_kbp_key_fwd2_to_acl_key_id_convert
 */
shr_error_e dnx_field_kbp_key_acl_to_fwd2_key_id_convert(
    int unit,
    dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_id,
    dbal_enum_value_field_field_key_e * key_id_p);

/**
 * \brief
 *  Get the FFC info for an external TCAM field group (for a specific opcode).
 * \param [in] unit        - Device Id
 * \param [in] fg_id   - Field group for which FFC info should be returned.
 * \param [in] opcode_id   - Opcode ID (AppType).
 * \param [in] fwd_context - FWD context ID.
 * \param [in] acl_context_id - ACL Context ID.
 * \param [out] group_ffc_info_p  - Structure that holds the FFC info.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *
 */
shr_error_e dnx_field_kbp_key_ffc_info_get(
    int unit,
    dnx_field_group_t fg_id,
    int opcode_id,
    uint8 fwd_context,
    dnx_field_context_t acl_context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p);

/**
 * \brief
 *  This function manages the available FFCs for a newly created ACL context by
 *  masking out the ffc's that are used by original FWD context.
 *
 * \param [in] unit           - Device Id
 * \param [in] base_opcode_id - The predefined opcode on which the new context is based.
 * \param [in] fwd_context_id - The FWD context that comes before the ACL context.
 * \param [in] acl_context_id - The newly created ACL cntext
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_ifwd2_acl_ffc_init
 */
shr_error_e dnx_field_kbp_ifwd2_acl_ffc_update_for_new_context(
    int unit,
    uint32 base_opcode_id,
    uint8 fwd_context_id,
    dnx_field_context_t acl_context_id);

/**
* \brief
*  Attach context(opcode) to FG type of external TCAM
* \param [in] unit            - Device ID
* \param [in] fg_id           - ACL Field group id (lookup_id) to attach to opcode
* \param [in] opcode_id       - the KBP OPCODE
* \param [in] attach_info_p   - attach info as was passed by user
* \param [out] context_info_p - holds the information about this context
*
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_group_opcode_attach(
    int unit,
    dnx_field_group_t fg_id,
    uint32 opcode_id,
    dnx_field_group_attach_info_t * attach_info_p,
    dnx_field_attach_context_info_t * context_info_p);

/**
* \brief
*  Returns the indices on the master key of the segments uses by each qualifier of a field group.
*
* \param [in] unit                - Device ID
* \param [in] fg_id               - field group
* \param [in] opcode_id           - Opcode
* \param [out] seg_index_on_master_by_qual - For each qualifier, the index of the corresponding segment on the
*                                            opcode's master key.
* \param [out] qual_index_on_segment    - For each qualifier, it's index within the segment.
*                                            Zero for non packed quals, as when ther are
*                                            multiple quals per segment they are packed.
* \param [out] qual_offset_on_segment   - For each qualifier, it's offset from the lsb of the segment in bits.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_group_segment_indices(
    int unit,
    dnx_field_group_t fg_id,
    uint32 opcode_id,
    uint8 seg_index_on_master_by_qual[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_index_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    uint8 qual_offset_on_segment[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG]);

/**
* \brief
*  Returns the number of segmentd used by FWD for a opcode.
*
* \param [in] unit                - Device ID
* \param [in] opcode_id           - Opcode
* \param [out] nof_fwd_segments_p - Number of FWD segments
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_group_opcode_nof_fwd_segments(
    int unit,
    uint32 opcode_id,
    uint8 *nof_fwd_segments_p);

/**
* \brief
*  Perform the preperations for device lock.
*
* \param [in] unit                - Device ID
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_group_set(
    int unit);

/**
 * \brief
 *  Get the FWD and ACL contexts used by an opcode
 * \param [in] unit                - Device ID
 * \param [in] opcode_id - The opcode id
 * \param [out] nof_contexts_p - The number of FWD context used by the opcode.
 * \param [out] fwd_contexts - The FWD contexts used by the opcode.
 *                             An array with DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE elements.
 * \param [out] acl_contexts - The ACL context corresponding to each FWD context.
 *                             An array with DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE elements.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_opcode_to_contexts(
    int unit,
    uint32 opcode_id,
    int *nof_contexts_p,
    dnx_field_context_t fwd_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE],
    dnx_field_context_t acl_contexts[DNX_DATA_MAX_FIELD_EXTERNAL_TCAM_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE]);

/**
* \brief
*  Allocates Context Id in the allowed range per Stage
* \param [in] unit                  - Device ID
* \param [in] base_static_opcode_id - The opcode cascaded from.
* \param [in] opcode_name           - The name of the new apptype.
* \param [in] user_defined_apptype  - The new apptype.
* \param [out] opcode_id_p          - Opcode id used.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_opcode_create(
    int unit,
    uint32 base_static_opcode_id,
    char opcode_name[DBAL_MAX_STRING_LENGTH],
    bcm_field_AppType_t user_defined_apptype,
    uint32 *opcode_id_p);

/**
 * \brief
 *  Returns if device lock has happened already.
 * \param [in] unit                - Device ID
 * \param [out] device_locked_p    - If TRUE device lock has happened
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_is_device_locked(
    int unit,
    int *device_locked_p);

/**
 * \brief
 *  Returns if KBP device is used.
 * \param [in] unit                - Device ID
 * \param [out] kbp_device_exist_p    - If TRUE KBP device is found
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_is_kbp_device_available(
    int unit,
    int *kbp_device_exist_p);

/**
 * \brief
 *  Returns opcode ID based on fwd context and acl context
 * \param [in] unit            - Device ID
 * \param [in] fwd_context     - fwd_context
 * \param [in] acl_context     - acl_context
 * \param [out] opcode_id_p    - Opcode ID.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_contexts_to_opcode_get(
    int unit,
    uint32 fwd_context,
    uint32 acl_context,
    uint32 *opcode_id_p);

/**
* \brief
*   This API retrieves the properties provided to a dynamic KBP table created by kbp_mngr_db_create.
*   \param [in]  unit          - Relevant unit.
*   \param [in]  fg_id - The field group
*   \param [out]  counters_enable_p - Enable counters for ACL db.
*   \param [out]  min_capacity_valid_p - True when min_capacity has a meaningful info, otherwise default values are set.
*   \param [out]  min_capacity_p - Set the minimum capacity reserved in EXTERNAL TCAM. Used if capacity_valid is true.
*   \param [out]  algorithmic_mode_valid_p - Whether to set the property ALGORITHMIC for algorithmic mode
*   \param [out]  algorithmic_mode_p - Set the algorithmic mode (algorithmic property) to this value. Used if algorithmic_mode_valid is true.
*   \param [out]  lpm_instead_of_tcam_p - Use LPM behaviour for ACL DB.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_field_kbp_db_properties_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *counters_enable_p,
    uint8 *min_capacity_valid_p,
    uint32 *min_capacity_p,
    uint8 *algorithmic_mode_valid_p,
    uint32 *algorithmic_mode_p,
    uint8 *lpm_instead_of_tcam_p);

/**
* \brief
*   This API retrieves the properties provided to a dynamic KBP table created by kbp_mngr_db_create.
*   \param [in]  unit          - Relevant unit.
*   \param [in]  fg_id - The field group
*   \param [out]  lpm_instead_of_tcam_p - ACL DP is actually an LPM DB and not a TCAM one.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_field_kbp_db_is_lpm_get(
    int unit,
    dnx_field_group_t fg_id,
    uint8 *lpm_instead_of_tcam_p);

/**
* \brief
*  Set TCAM field group cache mode for an external TCAM field group
* \param [in] unit         - Device id
* \param [in] flags        - Cache flags (currently not in use)
* \param [in] fg_id        - Field Group id to set mode to
* \param [in] mode         - Cache mode of the field group
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_group_cache(
    int unit,
    uint32 flags,
    dnx_field_group_t fg_id,
    dnx_field_group_cache_mode_e mode);

/**
* \brief
*  Verify that the payload size is legal for an external TCAM field group.
* \param [in] unit         - Device id
* \param [in] payload_size_bits - size of the field group's payload in bits
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_payload_size_verify(
    int unit,
    int payload_size_bits);

/**
 * \brief
 *  This function initiates the ACL resources.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This function is called from the dnx_general_pp_init() because we need
 *   the PEMLA to finish it's initializations for the key/ffc resource recognition.
 *   EMLA finishes it's initialization after Field module, thus we call the acl_init from
 *   dnx_general_pp_init().
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_context_ifwd2_kbp_acl_init(
    int unit);

/**
 * \brief
 *  Actions to be performed when adding a field group.
 * \param [in] unit         - Device id
 * \param [in] fg_id        - Field group ID.
 * \param [in] fg_info_p    - Field group information in group SW state.
 * \param [in] external_tcam_info_p - Field group information specific to external TCAM.
 * \param [in] actions_size_in_bits - Payload size
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_group_external_tcam_add(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_group_fg_info_for_sw_t * fg_info_p,
    dnx_field_group_external_tcam_info_t * external_tcam_info_p,
    unsigned int actions_size_in_bits);

/**
 * \brief
 *  This function performs initialization for external TCAM SW state
 * \param [in] unit                - Device ID
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_tcam_external_swstate_init(
    int unit);

/* } */
#endif
/* } */

#endif
