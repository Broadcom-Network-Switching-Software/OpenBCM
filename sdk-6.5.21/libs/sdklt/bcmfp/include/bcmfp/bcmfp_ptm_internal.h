/*! \file bcmfp_ptm_internal.h
 *
 * APIs to interact with different resource
 * managers in BCMPTM component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_PTM_INTERNAL_H
#define BCMFP_PTM_INTERNAL_H

/*!
 * \brief API to dispatch table info get request from FP to
 *  PTM for ALPM LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] op_arg        LT operation arguments.
 * \param [in] req_ltid      ALPM LT ID
 * \param [in] table_arg     LTD arguments for entry limit attr.
 * \param [in] table_data    Maximium allowed entries in LT
 * \param [in] arg           LTD generic arguments.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_alpm_table_info_get(int unit,
                              uint32_t trans_id,
                              bcmltd_sid_t req_ltid,
                              const bcmltd_table_entry_limit_arg_t *table_arg,
                              bcmltd_table_entry_limit_t *table_data,
                              const bcmltd_generic_arg_t *arg);
/*!
 * \brief API to dispatch insert request from FP to
 *  PTM for ALPM LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] op_arg        LT operation arguments.
 * \param [in] req_ltid      ALPM LT ID
 * \param [in] entry_words   Entry words containes
 *                           both key and data..
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_alpm_insert(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t req_ltid,
                      uint32_t *entry_words);

/*!
 * \brief API to dispatch delete request from FP to
 *  PTM for ALPM LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] op_arg        LT operation arguments.
 * \param [in] req_ltid      ALPM LT ID
 * \param [in] entry_words   Entry words containes
 *                           both key and data..
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_alpm_delete(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t req_ltid,
                      uint32_t *entry_words);
/*!
 * \brief API to dispatch lookup request from FP to
 *  PTM for ALPM LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] op_arg        LT operation arguments.
 * \param [in] req_ltid      ALPM LT ID
 * \param [in] entry_words   Entry words containes
 *                           both key and data.
 * \param [in] rsp_ekw_ew    Response entry key words.
 * \param [in] rsp_edw_ew    Response entry data words.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_alpm_lookup(int unit,
                      const bcmltd_op_arg_t *op_arg,
                      bcmltd_sid_t req_ltid,
                      uint32_t *entry_words,
                      uint32_t *rsp_ekw_ew,
                      uint32_t *rsp_edw_ew);
/*!
 * \brief API to dispatch traverse(get/next) request from
 *  FP to PTM for ALPM LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] op_arg        LT operation arguments.
 * \param [in] req_ltid      ALPM LT ID
 * \param [in] get_first     GET_FIRST/NEXT indicator.
 * \param [in] entry_words   Entry words containes
 *                           both key and data.
 * \param [in] rsp_ekw_ew    Response entry key words.
 * \param [in] rsp_edw_ew    Response entry data words.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_alpm_traverse(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        bcmltd_sid_t req_ltid,
                        bool get_first,
                        uint32_t *entry_words,
                        uint32_t *rsp_ekw_ew,
                        uint32_t *rsp_edw_ew);
/*!
 * \brief API to dispatch read request from FP to
 *  PTM for indexed LT. Read data will be compared
 *  with comp_data and return SUCCESS if matched
 *  otherwise NOT_FOUND.
 *
 * \param [in] unit          Logical device id
 * \param [in] trans_id      Transaction ID
 * \param [in] tbl_inst      Pipe ID
 * \param [in] req_ltid      Indexed LT ID
 * \param [in] phy_sid       Physical Table to read
 * \param [in] comp_data     Data to be compared with
 *                           read data
 * \param [in] index         Index at which entry has
 *                           to be written
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_index_lookup(int unit,
                       uint32_t trans_id,
                       int tbl_inst,
                       bcmltd_sid_t req_ltid,
                       bcmdrd_sid_t profile_sid,
                       uint32_t *comp_data,
                       int index);
/*!
 * \brief API to dispatch write request from FP to
 *  PTM for indexed LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] trans_id      Transaction ID
 * \param [in] tbl_inst      Pipe ID
 * \param [in] req_ltid      Indexed LT ID
 * \param [in] phy_sid       Physical Table to read
 * \param [in] entry_words   Entry data to be read
 * \param [in] index         Index at which entry has
 *                           to be written
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_index_insert(int unit,
                       uint32_t trans_id,
                       int tbl_inst,
                       bcmltd_sid_t req_ltid,
                       bcmdrd_sid_t phy_sid,
                       uint32_t *entry_words,
                       int index);
/*!
 * \brief API to dispatch delete request from FP to
 *  PTM for indexed LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] trans_id      Transaction ID
 * \param [in] tbl_inst      Pipe ID
 * \param [in] req_ltid      Indexed LT ID
 * \param [in] phy_sid       Physical Table to read
 * \param [in] index         Index at which entry has
 *                           to be deleted
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_index_delete(int unit,
                       uint32_t trans_id,
                       int tbl_inst,
                       bcmltd_sid_t req_ltid,
                       bcmdrd_sid_t phy_sid,
                       int index);
/*!
 * \brief API to dispatch read request from FP to
 *  PTM for indexed LT.
 *
 * \param [in] unit          Logical device id
 * \param [in] trans_id      Transaction ID
 * \param [in] tbl_inst      Pipe ID
 * \param [in] req_ltid      Indexed LT ID
 * \param [in] phy_sid       Physical Table to read
 * \param [in] entry_words   Entry data to be read
 * \param [in] index         Index at which entry has
 *                           to be read
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_index_read(int unit,
                     uint32_t trans_id,
                     int tbl_inst,
                     bcmltd_sid_t req_ltid,
                     bcmdrd_sid_t phy_sid,
                     uint32_t *entry_words,
                     int index);
/*!
 *  \brief BCMFP wrapper for bcmptm_mreq_keyed_lt
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] req_ltid Logical Table enum that is accessing the table
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] req_op LOOKUP, DELETE, INSERT ops are valid for tables
 * \param [in] req_info See the definition for bcmptm_keyed_lt_mreq_info_t
 * \param [in] cseq_id Commit Sequence ID.
 * \param [out] rsp_info See the definition for bcmptm_keyed_lt_mrsp_info_t
 * \param [out] rsp_ltid Previous owner of the entry
 * \param [out] rsp_flags Response flags propogated back to BCMFP
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_ltm_mreq_keyed_lt(int unit,
                            uint64_t req_flags,
                            bcmltd_sid_t req_ltid,
                            void *pt_dyn_info,
                            void *pt_ovrr_info,
                            bcmptm_op_type_t req_op,
                            bcmptm_keyed_lt_mreq_info_t *req_info,
                            uint32_t trans_id,
                            bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                            bcmltd_sid_t *rsp_ltid,
                            uint32_t *rsp_flags);
/*!
 *  \brief Scopes the availability of physical resources requested.
 *
 * \param [in] unit                Logical device id
 * \param [in] flags               Control to alter default behavior
 *                                 (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] op_arg              LT operation arguments.
 * \param [in] req_ltid            Logical Table enum ID.
 * \param [in] req_buffers         Different buffers to hold requested
 *                                 information to PTM.
 * \param [in] rsp_buffers         Different buffers to hold response
 *                                 information from PTM.
 * \param [in] req_res             Requested resources to be scoped.
 * \param [in] req_res_count       Number of elements in req_res array.
 * \param [in] resources_available Flag to indicate resources availability
 *                                 in PTM.
 *
 * \retval SHR_E_NONE   Success
 * \retval SHR_E_PARAM  Invalid parameters.
 */
extern int
bcmfp_ptm_resource_scope(int unit,
                         uint32_t flags,
                         const bcmltd_op_arg_t *op_arg,
                         bcmfp_stage_id_t stage_id,
                         bcmltd_sid_t req_ltid,
                         bcmfp_buffers_t *req_buffers,
                         bcmfp_buffers_t *rsp_buffers,
                         bcmptm_rm_tcam_fp_resources_req_t *req_res,
                         uint16_t req_res_count,
                         bool *resources_available);
#endif /* BCMFP_PTM_INTERNAL_H */
