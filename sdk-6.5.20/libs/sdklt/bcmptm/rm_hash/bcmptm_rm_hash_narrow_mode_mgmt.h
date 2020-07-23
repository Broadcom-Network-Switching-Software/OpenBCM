/*! \file bcmptm_rm_hash_narrow_mode_mgmt.h
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

#ifndef BCMPTM_RM_HASH_NARROW_MODE_MGMT_H
#define BCMPTM_RM_HASH_NARROW_MODE_MGMT_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Extract narrow mode entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] enm Entry narrow mode
 * \param [in] nme_loc Narrow mode entry location.
 * \param [in] src_ent_buf Pointer to source entry buffer.
 * \param [out] dst_ent_buf Pointer to destination entry buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_nm_entry_extract(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                rm_hash_entry_narrow_mode_t enm,
                                uint8_t nme_loc,
                                uint32_t *src_ent_buf,
                                uint32_t *dst_ent_buf);

/*!
 * \brief Get the matched ltid location.
 *
 * Get the narrow mode entry ltid locations.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table id.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] be_offset Base entry offset.
 * \param [out] ltid_loc_list Pointer to the buffer recording the ltid location.
 * \param [out] num_ltids Pointer to the buffer recording the ltid number.
 * \param [out] e_nm Entry narrow mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_matched_ltid_get(int unit,
                                bcmltd_sid_t ltid,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                uint8_t pipe,
                                uint32_t be_offset,
                                uint8_t *ltid_loc_list,
                                uint8_t *num_ltids,
                                rm_hash_entry_narrow_mode_t *e_nm);

/*!
 * \brief Get the ltid based on narrow mode entry location.
 *
 * Get the narrow mode entry ltid based on location.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] be_offset Base entry offset.
 * \param [in] nme_loc Narrow mode entry location.
 * \param [out] ltid Pointer to the buffer recording the ltid.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_nme_ltid_get(int unit,
                            rm_hash_pt_info_t *pt_info,
                            uint8_t rbank,
                            uint8_t pipe,
                            uint32_t be_offset,
                            uint8_t nme_loc,
                            bcmltd_sid_t *ltid);

#endif /* BCMPTM_RM_HASH_NARROW_MODE_MGMT_H */
