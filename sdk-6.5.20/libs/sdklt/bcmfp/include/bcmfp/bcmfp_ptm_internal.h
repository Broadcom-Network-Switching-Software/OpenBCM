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
#endif /* BCMFP_PTM_INTERNAL_H */
