/*! \file bcmptm_rm_hash_entry_search.h
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

#ifndef BCMPTM_RM_HASH_ENTRY_SEARCH_H
#define BCMPTM_RM_HASH_ENTRY_SEARCH_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Iterate all the mapping groups and try to search an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Context information for the requested operation.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [out] glb_srch_info Pointer to rm_hash_glb_srch_info_t structure.
 * \param [out] map_srch_result Pointer to rm_hash_map_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_search(int unit,
                          rm_hash_req_context_t *req_ctxt,
                          rm_hash_lt_ctrl_t *lt_ctrl,
                          rm_hash_glb_srch_info_t *glb_srch_info,
                          rm_hash_map_srch_result_t *map_srch_result);

#endif /* BCMPTM_RM_HASH_ENTRY_SEARCH_H */
