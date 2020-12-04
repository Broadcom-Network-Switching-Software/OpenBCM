/*! \file bcmptm_rm_hash_req_handler.h
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

#ifndef BCMPTM_RM_HASH_REQ_HANDLER_H
#define BCMPTM_RM_HASH_REQ_HANDLER_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get physical hit bit table for a hash LT entry.
 * On some devices, the functional hash table has separated physical hit bit
 * table. In this case, when the hit bit to be retrieved, the corresponding hit
 * table should be read.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] dyn_info Pointer to dyn_info structure.
 * \param [in] hit_info Pointer to bcmptm_rm_hash_hit_info_t structure.
 * \param [out] ent_words Pointer to entry buffer.
 *
 * \retval NONE
 */
extern int
bcmptm_rm_hash_hit_bit_get(int unit,
                           rm_hash_req_context_t *req_ctxt,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           const bcmptm_rm_hash_hit_info_t *hit_info,
                           uint32_t *ent_words);

/*!
 * \brief Hash lookup request handler
 *
 * \param [in] unit Unit number.
 * \param [in] req_flags Requested flags passed from PTM.
 * \param [in] req_ltid Requested LT ID.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] req_info Pointer to bcmptm_rm_hash_req_t structure.
 * \param [in] rsp_entry_buf_wsize Size of response entry buffer.
 * \param [out] rsp_entry_words Pointer to a buffer for recording response entry.
 * \param [out] rsp_info Pointer to bcmptm_rm_hash_rsp_t structure.
 * \param [out] rsp_ltid Pointer to bcmltd_sid_t object.
 * \param [out] rsp_flags Pointer to response flags.
 * \param [out] rsp_map_idx Pointer to buffer recording the LT to PT map index.
 * \param [out] nme_info Pointer to buffer recording entry narrow mode info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lookup(int unit,
                      uint64_t req_flags,
                      bcmltd_sid_t req_ltid,
                      bcmbd_pt_dyn_info_t *pt_dyn_info,
                      bcmptm_rm_hash_req_t *req_info,
                      size_t rsp_entry_buf_wsize,
                      uint32_t *rsp_entry_words,
                      bcmptm_rm_hash_rsp_t *rsp_info,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags,
                      uint8_t *rsp_map_idx,
                      rm_hash_nme_info_t *nme_info);

#endif /* BCMPTM_RM_HASH_REQ_HANDLER_H */
