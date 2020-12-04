/*! \file rm_tcam_fp_entry_mgmt.h
 *
 * Low Level Functions for FP based TCAMs
 * This file contains low level functions for FP based TCAMs
 * to do operations like TCAM entry Insert/Lookup/Delete/Move.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_FP_ENTRY_MGMT_H
#define RM_TCAM_FP_ENTRY_MGMT_H

/*******************************************************************************
 * Defines
 */
#define BCMPTM_FP_GRP_SEG_WORDS 4

/*******************************************************************************
 * Typedefs
 */
/*! Entry Segment bitmap structure */
typedef struct bcmptm_fp_grp_seg_bmp_s {
    SHR_BITDCL w[BCMPTM_FP_GRP_SEG_WORDS];
} bcmptm_fp_grp_seg_bmp_t;


/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Insert the new FP entry.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] partition Partition Id.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FULL No Resources
 */

extern
int bcmptm_rm_tcam_fp_entry_insert(int unit,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Delete the new FP entry.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] partition Partition Id.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FULL No Resources
 */

extern
int bcmptm_rm_tcam_fp_entry_delete(int unit,
                                 uint64_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Lookup the new FP entry.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_fp_entry_lookup(int unit,
                                 uint64_t req_flags,
                                 bcmltd_sid_t ltid,
                                 bcmptm_rm_tcam_lt_info_t *ltid_info,
                                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                                 bcmptm_rm_tcam_req_t *req_info,
                                 bcmptm_rm_tcam_rsp_t *rsp_info,
                                 bcmltd_sid_t *rsp_ltid,
                                 uint32_t *rsp_flags,
                                 uint32_t *index,
                                 bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Move the TCAM entry from one index to the other in priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] from_idx TCAM index from which entry has to be moved.
 * \param [in] to_idx TCAM index to which entry has to be moved.
 * \param [in] entry_attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_fp_entry_move(int unit,
                                   bcmltd_sid_t ltid,
                                   bcmptm_rm_tcam_lt_info_t *ltid_info,
                                   uint8_t partition,
                                   uint32_t prefix,
                                   uint32_t from_idx,
                                   uint32_t to_idx,
                                   void *attrs,
                                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                                   bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief To Fetch entry segments for a particular group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] stage_id Stage id
 * \param [in] pipe_id Pipe number
 * \param [out] segment_id Segment allocated
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */

extern int
bcmptm_rm_tcam_fp_group_entry_segment_get(int unit,
                                          bcmltd_sid_t ltid,
                                          bcmptm_rm_tcam_lt_info_t *ltid_info,
                                          bcmptm_rm_tcam_entry_attrs_t *entry_attr,
                                          uint8_t slice_count,
                                          uint16_t *segment_id);
/*!
 * \brief Compress entry segments for a particular group
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] group_id Group Id
 * \param [in] pipe_id Pipe number
 * \param [in] stage_id Stage id
 * \param [in] num_slices_freed count of slices freed
 * \param [in] group_delete Group is deleted or not
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_NOT_FOUND No such stage exists
 */

extern int
bcmptm_rm_tcam_fp_entry_segment_compress(int unit,
                     bcmltd_sid_t ltid,
                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                     uint32_t group_id,
                     int pipe_id,
                     bcmptm_rm_tcam_fp_stage_id_t stage_id,
                     uint8_t num_slices_freed,
                     bool group_delete);

/*!
 * \brief Get the table information from FP TCAM
 *
 * \param [in] unit Logical device id
 * \param [in] flags flags from upper layer
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info dynamic information from upper layer
 * \param [in] req_info Request information
 * \param [out] rsp_info Response information
 * \param [out] rsp_ltid Response LTID information.
 * \param [out] rsp_flags Response flags.Not used.
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Table info updated
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_fp_get_table_info(int unit,
                                uint64_t flags,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_req_t *req_info,
                                bcmptm_rm_tcam_rsp_t *rsp_info,
                                bcmltd_sid_t *rsp_ltid,
                                uint32_t *rsp_flags,
                                bcmptm_rm_tcam_entry_iomd_t *iomd);





#endif /* RM_TCAM_FP_ENTRY_MGMT_H */
