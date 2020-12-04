/*! \file rm_tcam_fp_psg.h
 *
 *  Presel group APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_FP_PSG_H
#define RM_TCAM_FP_PSG_H

/*!
 * \brief Get the number of presel groups across all pipes of a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe XGS/XFC pipeline instance.
 * \param [out] presel_group_count Number of presel grousp across all pipes.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_group_count_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint8_t pipe,
                              uint8_t *presel_group_count);
/*!
 * \brief Initialize reference count of all presel groups in all pipes
 *  of a stage.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_ptr HA memory pointer to a given stage.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_group_ref_count_init(int unit,
                        bcmptm_rm_tcam_fp_stage_t *stage_ptr);
/*!
 * \brief Get the start pointer to presel groups reference count
 *  segment of a slice.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id XGS/XFS pipe instance.
 * \param [in] stage id One of the FP_ING/FP_EGR_FP_VLAN
 * \param [in] slice_id Physical slice ID.
 * \param [out] ref_count_ptr Pointer to start presel groups
 *              reference count of the slice.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_group_ref_count_get(int unit,
                  bcmltd_sid_t ltid,
                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                  int pipe_id,
                  bcmptm_rm_tcam_fp_stage_t *stage_fc,
                  uint8_t slice_id,
                  uint32_t **ref_count_ptr);
/*!
 * \brief Reserve indexes for new profiles in presel group table.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 * \param [in] slice_id Physical slice ID.
 * \param [in] num_slices Number of slices from the slice_id.
 * \param [out] hw_indexes Reserved indexes in presel group table.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_group_reserve(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       uint8_t slice_id,
                                       uint8_t num_slices,
                                       uint8_t *hw_indexes);
/*!
 * \brief Unreserve indexes of profiles in presel group table.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_group_unreserve(int unit,
                                         bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Copy presel groups from primary slice of a group to a
 *  newly expanded slice.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 * \param [in] new_slice_id Newly expanded physical slice ID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_groups_copy(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t new_slice_id);
/*!
 * \brief Clear the presel groups from a slice when the slice is
 *  released(happens when group is compressed) from the group.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input and Output meta data
 * \param [in] slice_id Physical slice ID that is freed.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_fp_presel_groups_clear(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd,
                                      uint8_t slice_id);
#endif /* RM_TCAM_FP_PSG_H */
