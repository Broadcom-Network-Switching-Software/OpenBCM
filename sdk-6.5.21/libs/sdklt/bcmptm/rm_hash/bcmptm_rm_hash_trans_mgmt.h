/*! \file bcmptm_rm_hash_trans_mgmt.h
 *
 * Utils, defines internal to RM Hash state
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_TRANS_MGMT_H
#define BCMPTM_RM_HASH_TRANS_MGMT_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get the status of whether rm hash undo list require update.
 *
 * \param [in] unit Unit number.
 *
 * \retval bool.
 */
extern bool
bcmptm_rm_hash_undo_list_maintained(int unit);

/*!
 * \brief Allocate memory for transaction state.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if it is warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_state_elems_alloc(int unit, bool warm);

/*!
 * \brief Configure transaction state elements.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if it is warmboot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_state_elems_config(int unit, bool warm);

/*!
 * \brief Insert an undo operation into the transaction undo list.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state_offset Bucket state offset.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] opcode Undo operation code.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_undo_node_insert(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      uint32_t b_state_offset,
                                      uint8_t pipe,
                                      rm_hash_ent_node_t *e_node,
                                      rm_hash_trans_undo_opcode_t opcode);

/*!
 * \brief Update an undo operation in the transaction undo list.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] b_state_offset Bucket state offset.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_undo_node_update(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      uint32_t b_state_offset,
                                      rm_hash_ent_node_t *e_node);

/*!
 * \brief Update transaction record when bcmptm_rm_hash_req is called
 *
 * \param [in] unit Unit number.
 * \param [in] cseq_id Current sequence id
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_ent_req_cmd(int unit,
                                 uint32_t cseq_id);

/*!
 * \brief Free memory space for various transaction state elements.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_state_elems_cleanup(int unit);
#endif /* BCMPTM_RM_HASH_TRANS_MGMT_H */
