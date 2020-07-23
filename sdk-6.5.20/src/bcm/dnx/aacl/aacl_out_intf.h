/*! \file src/bcm/dnx/aacl/aacl_out_intf.h
 *
 * Internal DNX AACL OUTPUT Interface API
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _AACL_OUT_INTF_INCLUDED__
/*
 * {
 */
#define _AACL_OUT_INTF_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * Include files
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/aacl/aacl_in_intf.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * GLOBALs
 * {
 */

/*
 * }
 */

/*
 * FUNCTIONs
 * {
 */

/**
* \brief
*    API to write LPM entry.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] entry_type - Type of the Entry we want to write.
*   \param [in] prefix - prefix to add.
*   \param [in] prefix_length - Length of the prefix.
*   \param [in] payload - payload associated to the entry.
*   \param [in] payload_size - size of the payload.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_lpm_entry_add(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    int payload_size);

/**
* \brief
*    API to delete LPM entry.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] entry_type - Type of the Entry we want to write.
*   \param [in] prefix - prefix to delete.
*   \param [in] prefix_length - Length of the prefix.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_lpm_entry_delete(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length);

/**
* \brief
*    API to update LPM entry payload.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] entry_type - Type of the Entry we want to write.
*   \param [in] prefix - prefix.
*   \param [in] prefix_length - Length of the prefix.
*   \param [in] payload - payload associated to the entry.
*   \param [in] payload_size - size of the payload.
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_lpm_entry_payload_update(
    int unit,
    int group_id,
    aacl_lpm_entry_type entry_type,
    uint32 *prefix,
    int prefix_length,
    uint32 *payload,
    uint32 payload_size);

/**
* \brief
*    API to write TCAM entry.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] entry_type - Type of the Entry we want to write.
*   \param [in] key - Entry key.
*   \param [in] mask - Entry mask.
*   \param [in] priority - Priority of the tcam entry.
*   \param [in] payload - payload associated to the entry.
*   \param [in] payload_size - size of the payload.
*   \param [out] entry_id - entry_id returned value
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/

shr_error_e dnx_aacl_tcam_entry_add(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    int priority,
    uint32 *payload,
    int payload_size,
    uint32 *entry_id);

/**
* \brief
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] entry_type - Type of the Entry we want to write.
*   \param [in] key - Entry key.
*   \param [in] mask - Entry mask.
*   \param [in] entry_id - entry_id returned during add
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_tcam_entry_delete(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 entry_id);

/**
* \brief
*    API to update payload of TCAM entry.
*   \param [in] unit - Relevant unit.
*   \param [in] group_id - AACL group id.
*   \param [in] entry_type - Type of the Entry we want to write.
*   \param [in] key - Entry key.
*   \param [in] mask - Entry mask.
*   \param [in] payload - payload associated to the entry.
*   \param [in] payload_size - size of the payload.
*   \param [in] entry_id - entry_id returned during add
*  \return
*    \retval errors if unexpected behavior. See \ref shr_error_e
*  \remark
*    None
*  \see
*    shr_error_e
*/
shr_error_e dnx_aacl_tcam_entry_payload_update(
    int unit,
    int group_id,
    aacl_tcam_entry_type entry_type,
    uint32 *key,
    uint32 *mask,
    uint32 *payload,
    int payload_size,
    uint32 entry_id);

/**
 */

#endif /* AACL_OUT_INTF_INCLUDED */
