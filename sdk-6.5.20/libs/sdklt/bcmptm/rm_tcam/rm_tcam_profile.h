/*! \file rm_tcam_profile.h
 *
 *  Profile Table resource management APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_PROFILE_H
#define RM_TCAM_PROFILE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Add profile entries in profile tables.
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id Transaction id of this operation.
 * \param [in] tbl_inst XGS/XFS pipe instance.
 * \param [in] req_ltid Logical Table enum that is accessing the table.
 * \param [in] num_profiles Number of profile SIDs
 * \param [in] profile_sids Array of physical table Id of profile tables.
 * \param [in] profile_data Array of profile entries data
 * \param [in] profile_ref_count Reference count of entries in profile SIDs.
 * \param [out] profile_index Physical indexe of profiles in profile SIDs.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_profile_add(int unit,
                  uint32_t trans_id,
                  int tbl_inst,
                  bcmltd_sid_t req_ltid,
                  uint8_t num_profiles,
                  bcmdrd_sid_t *profile_sids,
                  uint32_t *profile_data[],
                  uint32_t *profile_ref_count,
                  int *profile_index);

/*!
 * \brief Delete profile entries in profile tables.
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id Transaction id of this operation.
 * \param [in] tbl_inst XGS/XFS pipe instance.
 * \param [in] req_ltid Logical Table enum that is accessing the table.
 * \param [in] num_profiles Number of profile SIDs
 * \param [in] profile_sids Array of physical table Id of profile tables.
 * \param [in] profile_ref_count Reference count of entries in profile SIDs.
 * \param [in] profile_index Physical indexe of profiles in profile SIDs.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_profile_delete(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     uint8_t num_profiles,
                     bcmltd_sid_t *profile_sids,
                     uint32_t *profile_ref_count,
                     int profile_index);
/*!
 * \brief Lookup for profile entry in profile table.
 *
 * \param [in] unit Logical device id
 * \param [in] trans_id Transaction id of this operation.
 * \param [in] tbl_inst XGS/XFS pipe instance.
 * \param [in] req_ltid Logical Table enum that is accessing the table.
 * \param [in] profile_sid Physical table Id of profile table.
 * \param [in] profile_data Profile entry data
 * \param [out] index Physical indexe of profile in profile SID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_profile_entry_lookup(int unit,
                           uint32_t trans_id,
                           int tbl_inst,
                           bcmltd_sid_t req_ltid,
                           bcmdrd_sid_t profile_sid,
                           uint32_t *profile_data,
                           int index);
#endif /* RM_TCAM_PROFILE_H */
