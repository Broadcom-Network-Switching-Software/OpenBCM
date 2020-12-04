/*! \file bcmcth_dlb_util.h
 *
 * BCMCTH Dynamic Load Balance (DLB) driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_DLB_UTIL_H
#define BCMCTH_DLB_UTIL_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Structure for DLB related physical table operation.
 *
 * This structure is used to supply PTM API attributes used during Port related logical
 * table entry Read/Write operations to hardware/physical tables.
 */
typedef struct bcmcth_dlb_pt_op_info_s {

    /*! Device Resource Database Symbol ID. */
    bcmdrd_sid_t drd_sid;

    /*! Request LT symbol ID information. */
    bcmltd_sid_t req_lt_sid;

    /*! Response LT symbol ID information. */
    bcmltd_sid_t rsp_lt_sid;

    /*! LT operation transaction ID. */
    uint32_t trans_id;

    /*! Request PTM API option flags. */
    uint64_t req_flags;

    /*! Response PTM API option flags. */
    uint32_t rsp_flags;

    /*! PTM operation type. */
    bcmptm_op_type_t op;

    /*! Physical Table Entry Size in number of words. */
    size_t wsize;

    /*!
     * Dynamic address information of the physical table.
     * Physical Table Entry Hardware Index and Pipe Instances to Install Info.
     */
    bcmbd_pt_dyn_info_t dyn_info;

    /*! Information to be passed with LT request. */
    bcmptm_misc_info_t  misc_info;

} bcmcth_dlb_pt_op_info_t;

/*!
 * \brief DMA writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_dlb_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_dlb_pt_dma_write(int unit,
                        bcmcth_dlb_pt_op_info_t *op_info,
                        uint32_t *buf);

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_dlb_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_dlb_pt_write(int unit,
                    bcmcth_dlb_pt_op_info_t *op_info,
                    uint32_t *buf);

/*!
 * \brief Read an entry from a hardware table index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_dlb_pt_op_info_t structure.
 * \param [out] buf Pointer to hardware table entry read data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware table read operation unsuccessful.
 */
extern int
bcmcth_dlb_pt_read(int unit,
                   bcmcth_dlb_pt_op_info_t *op_info,
                   uint32_t *buf);

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] op_info Pointer to bcmcth_dlb_pt_op_info_t structure.
 * \param [in] buf Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_dlb_irq_pt_write(int unit,
                        bcmcth_dlb_pt_op_info_t *op_info,
                        uint32_t *buf);


/*!
 * \brief Reads the field value from field list given
 *        field ID.
 *
 * \param [in] unit Unit number.
 * \param [in] in Field list.
 * \param [in] fid Field ID.
 * \param [out] fval Field value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Field not found.
 */
extern int
bcmcth_dlb_imm_field_get(int unit,
                         const bcmltd_field_t *in,
                         uint32_t fid,
                         uint64_t *fval);

/*!
 * \brief Writes an entry to a hardware table at a specified index location.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] pt_id Physical table ID.
 * \param [in] index Index to write to.
 * \param [in] entry_data Pointer to hardware table entry write data.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_FAIL Hardware write operation unsuccessful.
 */
extern int
bcmcth_dlb_ireq_write(int unit,
                  bcmltd_sid_t lt_id,
                  bcmdrd_sid_t pt_id,
                  int index,
                  void *entry_data);

#endif /* BCMCTH_DLB_UTIL_H */
