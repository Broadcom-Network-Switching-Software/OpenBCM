/** \file oam_oamp_v1.h
 * OAMP generation one support - contains internal functions and definitions
 * for support of the OAM/BFD accelerator BFD functionality
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef OAM_OAMP_V1_H_INCLUDED
/*
 * {
 */
#define OAM_OAMP_V1_H_INCLUDED

#ifndef BCM_DNX2_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - Disable opcode statistics counting for per opcode
 *          for TX/RX.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_opcode_for_count_table_init(
    int unit);

/**
 * \brief - This function sets the OAMP MEP DB entries
 *          for a MEG with a 48B MAID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] group_info - structure containing the MEG params
 * \param [in] calculated_crc - 16-bit CRC used for verifying
 *        incoming CCM packets for endpoints in this MEG
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_db_48b_maid_set(
    int unit,
    bcm_oam_group_info_t * group_info,
    uint16 calculated_crc);

/**
 * \brief - This function configures value at LMM MAC DA OUI(MSB) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA MSB Profile/Index(maximum 8 profiles)
 * \param [in] msb - MSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mac_da_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t msb);

/**
 * \brief - This function gets the value configured at LMM MAC DA OUI(MSB) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA MSB Profile/Index(maximum 8 profiles)
 * \param [out] msb - MSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mac_da_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t msb);

/**
 * \brief - This function configures value at LMM MAC DA NIC(lsb) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA lsb Profile/Index(maximum 8 profiles)
 * \param [in] lsb - LSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mac_da_lsb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t lsb);

/**
 * \brief - This function gets the value configured at LMM MAC DA NIC(lsb) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA lsb Profile/Index(maximum 8 profiles)
 * \param [out] lsb - LSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mac_da_lsb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t lsb);

/**
 * \brief - This function adds OAMP MEP profile entry that is used in
 *          OAM PDU TX from OAMP
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile - MEP profile index
 * \param [in] is_rfc_6374 - MEP type is RFC6374 entry
 * \param [in] mep_profile_info - Pointer to MEP profile information structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_profile_set(
    int unit,
    uint8 mep_profile,
    uint8 is_rfc_6374,
    const dnx_oam_mep_profile_t * mep_profile_info);

/**
 * \brief - This function gets OAMP MEP profile entry that is used in
 *          OAM PDU TX from OAMP
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile - MEP profile index
 * \param [out] mep_profile_info - Pointer to MEP profile information structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_profile_get(
    int unit,
    uint8 mep_profile,
    dnx_oam_mep_profile_t * mep_profile_info);

/**
 * \brief - This function clears OAMP MEP profile entry
 *          from OAMP
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile - MEP profile index
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_profile_clear(
    int unit,
    uint8 mep_profile);

/**
 * \brief - This function add mpls/pwe push profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MPLS/PWE push profile(maximum 16 profiles
 * \param [in] ttl - time-to-live bits
 * \param [in] exp - experimental bits
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_set(
    int unit,
    uint8 profile,
    uint8 ttl,
    uint8 exp);

/**
 * \brief - This function provide exp and ttl bits according mpls/pwe push profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MPLS/PWE push profile(maximum 16 profiles
 * \param [out] ttl - time-to-live bits
 * \param [out] exp - experimental bits
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_get(
    int unit,
    uint8 profile,
    uint8 *exp,
    uint8 *ttl);

/**
 * \brief - This function clears push profile entry
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MPLS/PWE push profile(maximum 16 profiles)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_get
 *   dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_set
 */
shr_error_e dnx_oam_oamp_v1_mpls_pwe_exp_ttl_profile_clear(
    int unit,
    uint8 profile);

/**
* \brief
*  Function release endpoint profiles(sw) related to mep_db
* \param [in] unit  - Relevant unit.
* \param [in] mep_db_entry - mep_db entry according endpoint_info
* \param [out] mep_hw_profiles_write_data - return profiles that should be removed(data cleared) in hw
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*/
shr_error_e dnx_oam_oamp_v1_ccm_endpoint_mep_db_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data);

/**
 * \brief - Set endpoint info of mpls oam according mep_db
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_db_entry - mep_db
 * \param [out] endpoint_info - return filled endpoint info for accelerated endpoints according mep_db
 *  Updated fields are:
 *               type,
 *               egress_label.label, egress_label.ttl, egress_label.exp
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_ccm_endpoint_mpls_oam_get(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info);

/**
 * \brief - This function clears the OAMP MEP DB entries
 *          for a MEG with a 48B MAID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] header_entry_index - index of the
 *        EXTRA_DATA_HEADER type entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_db_48b_maid_clear(
    int unit,
    uint32 header_entry_index);

/**
 * \brief - This function add entry to RMEP_INDEX_DB.
 *          This DB map OAM-ID and MEP-ID to Remote MEP index(RMEP_INDEX).
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 * \param [in] mep_id - Index of Maintenance End Point
 * \param [in] rmep_id - Remote MEP DB(RMEP_INDEX) id.
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_EXIST - entry exist
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_rmep_index_db_add(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 rmep_id);

/**
 * \brief - This function remove the entry from RMEP_INDEX_DB table.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 * \param [in] mep_id - Index of Maintenance End Point
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_NOT_FOUND - entry not exist
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_rmep_index_db_remove(
    int unit,
    uint16 oam_id,
    uint16 mep_id);

/**
 * \brief - Add an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_loss_add(
    int unit,
    bcm_oam_loss_t * loss_ptr);

/**
 * \brief - Get an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_loss_get(
    int unit,
    bcm_oam_loss_t * loss_ptr);

/**
 * \brief - Delete an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_loss_delete(
    int unit,
    bcm_oam_loss_t * loss_ptr);

/**
 * \brief - Add an OAM Delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_delay_add(
    int unit,
    bcm_oam_delay_t * delay_ptr);

/**
 * \brief - Get an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_delay_get(
    int unit,
    bcm_oam_delay_t * delay_ptr);

/**
 * \brief - Delete an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_delay_delete(
    int unit,
    bcm_oam_delay_t * delay_ptr);

/**
 * \brief - Function fill mep_db struct. Assumes ID is allocated
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested MEP
 *                                 return mep_id if with id flag not set.
 * \param [in] sw_endpoint_info - software info structure of the requested MEP
 * \param [in] group_info - structure containing MEG parameters
 * \param [out] mep_db_entry - return filled mep_db entry according endpoint_info
 * \param [out] mep_hw_profiles_write_data - return new profiles that should be created in hw
 * \param [out] mep_hw_profiles_delete_data - when modifying, return existing  profiles that
 *        should be deleted from hw
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * dnx_oam_local_mep_create
 */
shr_error_e dnx_oam_oamp_v1_ccm_endpoint_mep_db_create(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data);

/**
 * \brief - This function reads the data relevant to a single
 *        accelerated OAM CCM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.  For full
 *        entry this must be a multiple or 4.
 * \param [in] is_down_mep_egress_injection - indication if endpoint is down mep egress injection
 * \param [out] entry_values - Structure into which all the
 *        endpoint parameters are written.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_db_ccm_endpoint_get(
    int unit,
    uint16 oam_id,
    uint8 is_down_mep_egress_injection,
    dnx_oam_oamp_ccm_endpoint_t * entry_values);

/**
 * \brief - Helper function for deleting associated loss entries
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_endpoint_delete_associated_loss_entries(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info);

/**
 * \brief - Helper function for getting oam id
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] lif_resolution - lifs assigned to gport/mpls_out_gport
 * \param [in] group_info - Information about the MEP's group
 * \param [in] is_bfd_alloc - BFD exists indication
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_endpoint_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const oam_lif_resolution_t * lif_resolution,
    const bcm_oam_group_info_t * group_info,
    uint8 *is_bfd_alloc);

/**
 * \brief - Helper function for local MEP create.
 *          This function gets lif resolution,
 *          analyzes and verifies that MEP create
 *          complies with hlm by mdl rules.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] mep_db_entry - return filled mep_db entry according endpoint_info
 * \param [in] mep_db_extra_data_length - size of extra data on mep_db
 * \param [in] is_mpls_lm_dm_without_bfd_endpoint
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_endpoint_remove_from_mep_db(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 mep_db_extra_data_length,
    const uint8 is_mpls_lm_dm_without_bfd_endpoint);

/**
 * \brief - Helper function for local MEP create.
 *          This function gets lif resolution,
 *          analyzes and verifies that MEP create
 *          complies with hlm by mdl rules.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] is_mpls_lm_dm_without_bfd_endpoint
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_accelerated_endpoint_destroy_mep_id_dealloc_v1(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 is_mpls_lm_dm_without_bfd_endpoint);

/**
 * Offsets are calcualted in the following way:
 * ETH: DA, SA addresss (12B) + num-tags (taken from mep db) * 4B
 *        + Ethertype (2B) + offset in OAM PDU (4B).
 *
 * For piggy backed CCMs an extra 54 bytes are added
 *          4B (sequence #) + 2B (MEP-ID) + 48B (MEG-ID)
 *
 * MPLS: GAL + GACH + MPLS/PWE label (4B each) + offset in OAM PDU (4B).
 *
 * PWE: GACH, PWE label, offset in OAM PDU (4 byte each)
 */
int dnx_oam_oamp_v1_lm_dm_set_offset(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    uint8 is_piggy_back,
    uint8 is_dm,
    uint8 is_reply,
    uint8 is_rfc_6374_lsp,
    uint8 is_egress_inj);

/**
 * \brief - This function initialize OAMP_INIT_Y1731_GENERAL_CFG table.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Registers and memories initialized in this function:
 *
 *   The following memory is initialized in this function:
 *       b. OAMP_INIT_Y1731_GENERAL_CFG table\n
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_init_y1731_cfg(
    int unit);

/**
 * \brief - flexible crc initialization
 *
 * \param [in] unit - unit number
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_flexible_crc_init(
    int unit);

/**
 * \brief - This function modifies the MEP DB and MDB entries of
 *        an offloaded LM/DM accelerated OAM endpoint for the
 *        purpose of activating or modifying the LM
 *        functionality.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_lm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    const dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params);

/**
 * \brief - This function creates or modifies an LM_DB type
 *        entry for a self-contained accelerated OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 * \param [in] update_ptr - indicates whether FLEX_LM_DM_PTR
 *        needs to be set in MEP DB CCM entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_self_contained_endpoint_lm_db_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *update_ptr);

/**
 * \brief - This function creates or modifies the dbal entries
 *        needed for an accelerated OAM CCM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.  For full
 *        entry this must be a multiple or 4.
 * \param [in] entry_values - Structure containing all the
 *        endpoint parameters
 * \param [in] is_last_entry - Indication if this is the last entry in flex pointer stack
 * \param [in] is_bfd_exist - Indication if a BFD entry already exists on the same LIF
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_rfc_6374_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values,
    uint8 is_last_entry,
    uint8 is_bfd_exist);

/**
 * \brief - This function add TPID profile used by MEP_DB
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - TPID Profile/Index(maximum 4 profiles)
 * \param [in] tpid - tpid
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_tpid_profile_set(
    int unit,
    uint8 profile,
    uint16 tpid);

/**
 * \brief - This function provide tpid according tpid profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - tpid profile profile(maximum 4 profiles)
 * \param [out] tpid - tpid
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_tpid_profile_get(
    int unit,
    uint8 profile,
    uint16 *tpid);

/**
 * \brief - This function map local port to system port
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] pp_port_profile - key to table entry
 * \param [in] system_port - system_port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_local_port_to_system_port_set(
    int unit,
    uint16 pp_port_profile,
    uint32 system_port);

/**
 * \brief - Release profiles(sw) that were used for ethernet oam and sign ethernet oam profiles that not in use anymore
 *
 * \param [in] unit  - Relevant unit.
 * \param [in] mep_db_entry - mep_db entry according endpoint_info
 * \param [out] mep_hw_profiles_write_data - sign profiles that should be removed(data cleared) in hw
 * \retval
 *   shr_error - Error indication
 * \remark
 *   None
 * \see
 *   None
 */
shr_error_e dnx_oam_oamp_v1_ccm_endpoint_eth_oam_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data);

/**
 * \brief - This function checks that a short entry being allocated
 *          either has sub-index 0, or that the short entry with
 *          sub-index 0 has the same MEP type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - number of entry being added.
 * \param [in] endpoint_info - OAM entry parameters for
 *             intended entry.
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_short_entry_type_verify(
    int unit,
    uint16 oam_id,
    const bcm_oam_endpoint_info_t * endpoint_info);

/**
 * \brief - This function sets extra data entries
 *          attached to CCM endpoint
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] extra_data - structure containing extra data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_ccm_endpoint_extra_data_set(
    int unit,
    dnx_oamp_ccm_endpoint_extra_data_t * extra_data);

/**
 * \brief - This function gets extra data entries
 *          attached to CCM endpoint
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] extra_data - structure containing extra data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_ccm_endpoint_extra_data_get(
    int unit,
    dnx_oamp_ccm_endpoint_extra_data_t * extra_data);

/**
 * \brief - This function searches the LM/DM entires associated
 *        with a self contained OAM endpoint for a specific type
 *        of the possible four types.  If an entry is found,
 *        that entry number is returned.  If not, the number of
 *        the entry in the next bank after the current last
 *        entry is returned.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 * \param [in,out] search_params - a pointer to a structure
 *        containing input fields specifying the search and
 *        output fields where the results will be written.
 * \param [in] is_find_only - 0 for find&add, 1 for find only
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_find_or_add_lm_dm_entry(
    int unit,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    dnx_oam_oamp_lm_dm_search_t * search_params,
    int is_find_only);

/**
 * \brief - This function creates or modifies an LM_STAT type
 *        entry for a self-contained accelerated OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_self_contained_endpoint_lm_stat_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params);

/**
 * \brief - This function reads the LM data for an LM/DM
 *        offloaded OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For LM/DM
 *        offloaded entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] lm_exists - Whether LM is enabled or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_lm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists);

/**
 * \brief - This function finds the LM_DB entry associated with
 *        a self-contained OAM endpoint and reads its data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For a self
 *        contained entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] lm_exists - Whether LM is enabled or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_self_contained_endpoint_lm_db_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists);

/**
 * \brief - This function finds the LM_STAT entry associated
 *        with a self-contained OAM endpoint and reads its data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For a self
 *        contained entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] lm_stat_exists - Whether LM stat is enabled or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_self_contained_endpoint_lm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_stat_exists);

/**
 * \brief - This function modifies the MEP DB and MDB entries of
 *        an offloaded LM/DM accelerated OAM endpoint for the
 *        purpose of activating or modifying the DM
 *        functionality.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_dm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params);

/**
 * \brief - This function reads the DM data for an LM/DM
 *        offloaded OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For LM/DM
 *        offloaded entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] is_found - Whether dm entry exists or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_dm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found);

/**
 * \brief - This function finds the DM_STAT entry (one-way or two-way)
 *        associated with a self-contained OAM endpoint and reads its
 *        data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For a self
 *        contained entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] is_found - Whether dm entry exists or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_self_contained_endpoint_dm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found);

/**
 * \brief
 * This function adds extra data header entry and extra data payload entries to the oamp mep db
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id - index of OAM endpoint
 * \param [in] is_offloaded - indicates if this is an extention to an offloaded entry
 * \param [in] is_q_entry - indicates if this is an extention to 1/4 entry
 * \param [in] calculated_crc - crc to be put in the extra data header entry
 * \param [in] opcode - the opcode for which extra data is created
 * \param [in] nof_data_bits - number of data bits to add to the extra data header and extra data payload entries
 * \param [in] data - pointer the data to be copied to the extra data header and extra data payload entries
 * \param [in] is_update - indication if mep is new or updated
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_oamp_v1_mdb_extra_data_set(
    int unit,
    uint16 oam_id,
    uint8 is_offloaded,
    uint8 is_q_entry,
    uint16 calculated_crc,
    int opcode,
    uint32 nof_data_bits,
    uint8 *data,
    uint8 is_update);

/*
 * This function get extra data header entry from the oamp mdb
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id- index of OAM endpoint
 * \param [in] extra_data_length - length of extra data in bits
 * \param [out] data - pointer the data to get the extra data header and extra data payload entries
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mdb_extra_data_get(
    int unit,
    uint16 oam_id,
    uint32 extra_data_length,
    uint8 *data);

/**
 * \brief
 * This function adds extra data header entry and extra data payload entries to the oamp mep db
 * \param [in] unit - number of hardware unit used
 * \param [in] index- index of OAM endpoint
 * \param [in] nof_data_bits - number of data bits to add to the extra data header and extra data payload entries
 * \param [in] data - pointer the data to be copied to the extra data header and extra data payload entries
 * \param [in] opcode - the opcode for which extra data is created
 * \param [in] calculated_crc_1 - crc_1 to be put in the extra data header entry
 * \param [in] calculated_crc_2 - crc_2 to be put in the extra data header entry
 * \param [in] is_update - indication if mep is new or updated
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_extra_data_set(
    int unit,
    int index,
    uint32 nof_data_bits,
    uint8 *data,
    int opcode,
    uint16 calculated_crc_1,
    uint16 calculated_crc_2,
    uint8 is_update);

/**
 * \brief
 * This function get extra data header entry from the oamp mep db
 * \param [in] unit - number of hardware unit used
 * \param [in] index- index of OAM endpoint
 * \param [out] data - pointer the data to get the extra data header and extra data payload entries
 * \param [out] crc_1 - expected crc (CHECK_CRC_VALUE_1)
 * \param [out] crc_2 - expected crc (CHECK_CRC_VALUE_2)
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_extra_data_get(
    int unit,
    int index,
    uint8 *data,
    uint16 *crc_1,
    uint16 *crc_2);

/**
 * \brief - enable dynamic memory access to all tables under oamp block
 *
 * \param [in] unit - unit number
 * \param [in] enable - enable/disable 1/0
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_dbal_dynamic_memory_access_set(
    int unit,
    int enable);

/**
 * \brief
 * This function calculates the phase counts for given opcode in the opcode parameter
 * and updates it in the mep_profile_data. It also changes the other opcodes' phase counts
 * if necessary.
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - id of the endpoint. Useful only if opcode = CCM/BFD
 * \param [in] is_jumbo_tlv_dm  - indicate jumbo tlv is config.
 * \param [in] is_short_entry_type - If it is a quarter entry. Useful only for opcode = CCM/BFD
 * \param [in] ccm_rate - rate of CCM/BFD opcode,since it is not available in mep_profile_data
 * \param [in] ccm_rx_without_tx - Indicates if it is a RX-only MEP.
 * \param [in] opcode   - The opcode for which rate is newly configured.
 * \param [in,out] mep_profile_data - Contains the rates for opcodes. Will be filled with
 *                                    appropriate phase counts.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_scan_count_calc(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_jumbo_tlv_dm,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    uint8 ccm_rx_without_tx,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data);

/**
 * \brief
 * This function sets the phase counts for LM or DM entries associated with
 * an MPLS-LM-DM endpoint.
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id - Index of Maintenance End Point.
 * \param [in] tx_rate - transmission rate for the given opcode.
 * \param [in] opcode - opcode of type of transmitted packet
 * \param [in,out] mep_profile_data - Contains the rates for opcodes.  Will
 *                                    be filled with appropriate phase counts.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_scan_count_calc_rfc_6374_lm_dm(
    int unit,
    uint32 mep_id,
    uint32 tx_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data);

/**
 * \brief - This function adds or modifies a single entry in the
 *        OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - ID of entry to add or modify.
 *        For full entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_db_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function returns the arguments of a single
 *        entry in the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - ID of entry to read.  For full entry,
 *        this must be a multiple of 4, that is OAM_ID.
 * \param [in] entry_values - Pointer to a struct to which the
 *        entry's arguments will be written.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_db_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function deletes a single entry in the OAMP MEP
 *        DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - entry_id - ID of entry to delete.  For
 *        full entry, this must be a multiple of 4.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mep_db_delete(
    int unit,
    const uint32 entry_id);

/**
 * \brief - This function adds or modifies a single entry in the
 *        MDB OAMP MEP DB static data table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   While some
 *   endpoints require multiple MDB entries, this function
 *   only adds one, so it must be called repeatedly.
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mdb_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function reads a single entry in the
 *        MDB OAMP MEP DB static data table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] access_index - this parameter specifies which MDB
 *        entry should be retrieved (up to 6 supported)
 * \param [out] entry_values - struct containing the values to
 *        be written to the entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   While some endpoints require multiple MDB entries, this
 *   function reads adds one, so it must be called repeatedly.
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mdb_get(
    int unit,
    const uint32 oamp_entry_id,
    const uint32 access_index,
    dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function deletes a single entry in the MDB OAMP
 *        MEP DB static data table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] access_index - this parameter specifies which MDB
 *        entry should be deleted (up to 6 supported)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_v1_mdb_delete(
    int unit,
    const uint32 oamp_entry_id,
    const uint32 access_index);

/**
 * \brief - This function adds, modifies or enables entries
 *        needed to allow an accelerated endpoint to collect LM
 *        statistics
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [in] entry_values - Only the flags are relevant here.
 *        The rest are read-only
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_lm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values);

/**
 * \brief - This function reads the LM statistics for an
 *        accelerated OAM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [out] entry_values - a structure into which the
 *        statistics will be written
 * \param [out] lm_exists - whether lm is enabled or not.
 * \param [out] lm_stat_exists - whether lm stat entry is found or not.
 *                               This makes sense only for self contained endpoints.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_lm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    uint8 *lm_exists,
    uint8 *lm_stat_exists);

/**
 * \brief - This function is relevant for self-contained OAM
 *        endpoints only.  The LM entries for this endpoint will
 *        be deleted from dbal and freed.  If there are any DM
 *        entries succeeding them, they will also be deleted.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint that will have
 *        its LM/DM entries deleted.
 * \param [in] is_mpls_lm_dm_entry_exists - indication if MPLS-LM-DM additional entry exists
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_lm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_mpls_lm_dm_entry_exists);

/**
 * \brief - This function adds, modifies or enables entries
 *        needed to allow an accelerated endpoint to collect DM
 *        statistics
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [in] entry_values - Only the flags are relevant here.
 *        The rest are read-only
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_dm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values);

/**
 * \brief - This function reads the DM statistics for an
 *        accelerated OAM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [out] entry_values - a structure into which the
 *        statistics will be written
 * \param [out] is_found - Whether dm entry exists or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_dm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    uint8 *is_found);

/**
 * \brief - This function deletes the OAMP DM entry for an
 *        accelerated OAM endpoint.  This can apply to both
 *        self-contained mode and offloaded mode.  The entries
 *        are deleted from DBAL and freed in the hardware.  For
 *        self-contained mode, the "last entry" bit will be set
 *        for the relevant LM entry.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAMP endpoint entry that
 *        will have its DM entries deleted.
 * \param [in] is_mpls_lm_dm_entry_exists - indicates if RFC6374 entry exists.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_mep_db_dm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 is_mpls_lm_dm_entry_exists);

/**
 * \brief
 * This function gets oam_id of lm with priority session
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id - index of OAM endpoint
 * \param [in] priority - priority of the session
 * \param [out] session_oam_id - oam_id of the lm session with
 *        the given priority
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_lm_with_priority_session_map_get(
    int unit,
    int oam_id,
    uint8 priority,
    uint32 *session_oam_id);

/**
 * \brief
 * This function clears lm session entry for a given oam_id and
 * priority
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id - index of OAM endpoint
 * \param [in] priority - priority of the session
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_v1_lm_with_priority_session_map_clear(
    int unit,
    int oam_id,
    uint8 priority);

/**
 * \brief -
 * Validatation of setting extra data index for endpoint.
 * extra data can be used to store LM/DM flexible DMAC or
 * LM/DM flexible DMAC + 48 bytes MAID.
 *
 * \param [in] unit -     Relevant unit.
 * \param [in] endpoint_info -   info structure of the requested
 *        MEP
 * \param [in] sw_endpoint_info -  SW info structure of the requested
 *        MEP
 * \param [in] group_info - group name
 *
 * \retval Error indication.
 */
shr_error_e dnx_oam_oamp_v1_lmdm_flexible_da_verify(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const dnx_oam_endpoint_info_t * sw_endpoint_info,
    const bcm_oam_group_info_t * group_info);

/*
 * }
 */
#endif /* OAM_OAMP_V1_H_INCLUDED */
