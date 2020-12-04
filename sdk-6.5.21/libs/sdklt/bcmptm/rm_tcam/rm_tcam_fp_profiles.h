/*! \file rm_tcam_fp_profiles.h
 *
 *  FP profile table management APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_FP_PROFILES_H
#define RM_TCAM_FP_PROFILES_H

/*!
 * \brief Reserve indexes for new profiles in PDD ,SBR and PSG
 *  (presel group) profile tables.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 * \param [in] slice_id Physical slice ID.
 * \param [in] num_slices Number of slices from the slice_id.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_profiles_reserve(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd,
                                   uint8_t slice_id,
                                   uint8_t num_slices);
/*!
 * \brief Unreserve indexes of profiles in PDD, SBR and PSG
 *  profile tables.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_profiles_unreserve(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Copy PDD, SBR and PSG profiles from primary slice of
 *  a group to a newly expanded slice.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 * \param [in] new_slice_id Newly expanded physical slice ID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_profiles_copy(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                uint8_t new_slice_id);
/*!
 * \brief Clear the PDD, SBR and PSG profiles from a slice when
 *  the slice is released(happens when group is compressed)
 *  from the group.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 * \param [in] slice_id Physical slice ID that is freed.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_profiles_clear(int unit,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd,
                                 uint8_t slice_id);
#endif /* RM_TCAM_FP_PROFILES_H */
