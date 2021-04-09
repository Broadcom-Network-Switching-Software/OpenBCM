/**
 * \file bcm_int/dnx/field/field_kbp.h
 *
 *
 * Field Field KBP configurations for DNX
 */
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */

#ifndef FIELD_FIELD_KBP_H_INCLUDED
/* { */
#define FIELD_FIELD_KBP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#if defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP)

/*
 * Include files
 * {
 */
#include <include/bcm_int/dnx/field/field.h>
#include <include/bcm_int/dnx/field/field_map.h>
#include <include/bcm_int/dnx/field/field_group.h>
#include <include/bcm_int/dnx/field/field_context.h>
#include <include/soc/dnx/swstate/auto_generated/types/dnx_field_key_types.h>

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
 *  Verify function for KBP input type attach info
 * \param [in] unit             - Device Id
 * \param [in] field_stage      - For which Field stage to Configure HW
 * \param [in] qual_type        - Qual Type - includes Encoded qualifier
 * \param [in] qual_info_p      - info from attach info.
 * \return
 *   shr_error_e               - Error Type
 * \remark
 *   * None.
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_qual_info_get_kbp_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p);

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
    dnx_field_qual_t dnx_qual_1[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_1[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    int nof_quals_2,
    dnx_field_qual_t dnx_qual_2[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
    dnx_field_qual_attach_info_t qual_attach_info_2[DNX_FIELD_GROUP_EXTERNAL_NOF_QUALS_PER_SEGMENT],
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
    dnx_field_group_kbp_info_t * kbp_opcode_info_p);

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
 *  Configure the KBP ffc info get.
 * \param [in] unit        - Device Id
 * \param [in] fg_id   - Field group for which FFC info should be returned.
 * \param [in] opcode_id   - Opcode ID (AppType).
 * \param [in] fwd_context - FWD context ID.
 * \param [in] acl_context_id - ACL Context ID.
 * \param [out] group_ffc_info_p  - Structure that holds KBP FFC info.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 */
shr_error_e dnx_field_kbp_key_kbp_ffc_info_get(
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
 *  This function manages the available for ACL context ffc by
 *  masking out the ffc's that are used by FWD context.
 *  The function goes over all ifwd2 contexts and checks which ffc's were marked as used by
 *  FWD context. Those ffc's are set as 'used' in the ffc resource manager, thus will never
 *  be allocated for the ACL context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_ifwd2_acl_ffc_init(
    int unit);

/**
 * \brief
 *  This function verifies that ACL Key resources were not used by other applications (such as Pemla).
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_field_kbp_key_ifwd2_acl_key_verify(
    int unit);

/**
 * \brief
 * Initialize the master key info in SW state, specifically the number of segments used by FWD.
 * \param[in] unit       - Device ID
 * \return
 *   shr_error_e - Error Type
 */
shr_error_e dnx_field_kbp_group_master_key_init(
    int unit);

/**
* \brief
*  Attach context(opcode) to FG type of KBP
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
*  Allocates Context Id in the allowed range per Stage
* \param [in] unit                - Device ID
* \param [in] flags               - Flags for context creation (WITH_ID)
* \param [in] apptype_info_p      - The structure containing the name of the new apptype and the static apptype
*                                   it cascades from.
* \param [in,out] apptype_p       - Pointer to Apptype.
*                                          As in - Apptype to allocate in case WITH ID is set
*                                          As out - Allocated Apptype by algo
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_field_kbp_context_apptype_create(
    int unit,
    dnx_field_context_flags_e flags,
    dnx_field_context_apptype_info_t * apptype_info_p,
    bcm_field_AppType_t * apptype_p);

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

/* } */
#endif /** defined(BCM_DNX2_SUPPORT) && defined(INCLUDE_KBP) */
/* } */

#endif
