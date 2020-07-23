/**
* \file        tcam_bank_algo.h
*
* DESCRIPTION :
*       This file represents the TCAM bank algo component. It exposes
*       all the bank algorithmic functionality the TCAM Bank Manager needs.
*
*
* PUBLIC FUNCTIONS (dnx_field_tcam omitted):
*       shr_error_e    bank_algo_find_id( unit, db, bank_prealloc )
*/
/**
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef TCAM_BANK_ALGO_H_INCLUDED
/** { */
#define TCAM_BANK_ALGO_H_INCLUDED

#include <bcm_int/dnx/field/tcam/tcam_entities.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_bank_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_access.h>

/**
 * \brief
 *  Returns whether or not there is a same-context collision for the given handler_id on the given bank_id.
 *  Same-context collision is a HW limitation in which no FGs that share the same context can share
 *  the same TCAM bank. This check is performed before adding the given handler on the given TCAM bank.
 * \param [in] unit       - Device ID
 * \param [in] bank_id    - Bank ID to check same-context collision on
 * \param [in] handler_id - The handler ID to check same-context collision for on the given bank ID
 * \param [out] collide   - TRUE if there is same-context collision for the given handler ID on the given bank ID
 */
shr_error_e dnx_field_tcam_bank_algo_check_context_collision(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *collide);

/**
 * \brief
 * shr_error_e bank_algo_get_owner_stage( unit, bank_id, *bank_owner_stage )
 * Returns the owner stage for the bank of the given bank_id.
 * \param [in] unit - unit
 * \param [in] bank_id - The id of the bank to return the owner stage for
 * \param [out] bank_owner_stage - The owner stage of the bank
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *       SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 */
shr_error_e dnx_field_tcam_bank_algo_get_owner_stage(
    int unit,
    int bank_id,
    dnx_field_tcam_stage_e * bank_owner_stage);

/**
 * \brief
 * bank_algo_get_handler_node_in_bank( unit, bank_id, handler_id, *handler_node )
 * Sets handler_node to the given handler's node in the active_handlers_id list of
 * the given bank. If node doesn't exist, it sets handler_node to NULL.
 *
 * \param [in] unit - unit
 * \param [in] bank_id - The bank to search in
 * \param [in] handler_id - The handler to search its node
 * \param [out] handler_node - The handler's node if it exists in the bank list,
 *                             NULL otherwise
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
shr_error_e dnx_field_tcam_bank_algo_get_handler_node_in_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    sw_state_ll_node_t * handler_node);

/**
 * \brief
 * shr_error_e bank_algo_handler_in_bank( unit, bank_id, handler_id, *handler_in_bank )
 * Checks whether the given handler_id is already inside the bank of the given bank_id
 * or not.
 * \param [in] unit - unit
 * \param [in] bank_id - The id of the bank to check for the handler_id in
 * \param [in] handler_id - The handler_id to check if it's inside the bank
 * \param [out] handler_in_bank - TRUE if given handler_id is inside the bank with
 *                                the given bank_id, FALSE otherwise.
 * \return
 *  \retval _SHR_E_NONE - success
 * \remark
 * SPECIAL INPUT:
 *       SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 */
shr_error_e dnx_field_tcam_bank_algo_handler_in_bank(
    int unit,
    int bank_id,
    uint32 handler_id,
    uint8 *handler_in_bank);

/**
 * \brief
 * bank_algo_find_id(unit, *bank_id) - Searches for a suitable bank
 * for the given handler to use.
 * \param [in] unit - unit
 * \param [in] handler_id - the handler id to search for.
 * \param [out] bank_id - The bank id that is suitable for the given handler to use
 * \return
 *  \retval _SHR_E_NONE - success
 * SPECIAL INPUT:
 *        SWSTATE:
 *           dnx_field_tcam_bank_sw - Holds information for all banks in device
 */
shr_error_e dnx_field_tcam_bank_algo_find_id(
    int unit,
    uint32 handler_id,
    int *bank_id);

/** } */
#endif
