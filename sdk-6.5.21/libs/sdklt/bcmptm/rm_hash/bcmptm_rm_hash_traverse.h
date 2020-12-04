/*! \file bcmptm_rm_hash_traverse.h
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

#ifndef BCMPTM_RM_HASH_TRAVERSE_H
#define BCMPTM_RM_HASH_TRAVERSE_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Process GET_FIRST/GET_NEXT opcode for LT traversal operation
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] cseq_id Current sequence id.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_op Operation code.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_travs_cmd_proc(int unit,
                              uint64_t req_flags,
                              uint32_t cseq_id,
                              bcmltd_sid_t req_ltid,
                              bcmbd_pt_dyn_info_t *pt_dyn_info,
                              bcmptm_op_type_t req_op,
                              bcmptm_rm_hash_req_t *req_info,
                              size_t rsp_entry_buf_wsize,
                              uint32_t *rsp_entry_words,
                              bcmptm_rm_hash_rsp_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags);
#endif /* BCMPTM_RM_HASH_TRAVERSE_H */
