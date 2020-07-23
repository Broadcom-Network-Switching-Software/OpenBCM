/*! \file bcmsec_utils_internal.h
 *
 * Internal utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_UTILS_INTERNAL_H
#define BCMSEC_UTILS_INTERNAL_H

#include <bcmlrd/bcmlrd_table.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd_intr.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_event.h>
#include <bcmsec/generated/bcmsec_ha.h>



/*! Default number of SA per SC in encrypt direction.*/
#define SEC_DEFAULT_NUM_SA_ENCRYPT      2

/*! Default number of SA per SC in decrypt direction.*/
#define SEC_DEFAULT_NUM_SA_DECRYPT      4

/*! Populate bcmltd_field_t field. */
#define BCMSEC_POPULATE_OUT_FIELD(fid, count, fval)\
    out->field[count]->id = fid; \
    out->field[count]->data = fval;

/*! SEC interrupt enable operations. */
typedef enum
bcmsec_intr_enable_e {
    /*! Disable interrupt. */
    INTR_DISABLE = 0,

    /*! Enable interrupt. */
    INTR_ENABLE = 1,

    /*! Clear interrupt. */
    INTR_CLEAR = 2
} bcmsec_intr_enable_t;

/*!
 * \brief Get the field value from field array for Logical table field ID.
 *
 * \param [in] unit  Unit number.
 * \param [in] fid   Logical table field ID.
 * \param [in] idx   Logical table array field index.
 * \param [in] field_array Field array having field id and field_value.
 * \param [out] fval field value for the logical table field ID.
 *
 * \return SHR_E_NONE No error.
 * \return SHR_E_NOT_FOUND field ID not found in field array.
 */
extern int
bcmsec_fval_get_from_field_array(int unit,
        bcmlrd_fid_t fid,
        uint32_t idx,
        const bcmltd_field_t *field_array,
        uint64_t *fval);

/*!
 * \brief Set the value of a field in the field list.
 *
 * Not applicable to array fields.
 *
 * \param [in] unit  Unit number.
 * \param [in] flist Allocated field list.
 * \param [in] fid   The fid of the field to be set.
 * \param [in] idx   The idx for array based fields.
 * \param [in] val   Value to set.
 *
 * \retval SHR_E_FULL No place to add new element to the field list.
 */
extern int
bcmsec_field_list_set(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t val);

/*!
 * \brief Set the value of a field in the field list.
 *
 * Not applicable to array fields.
 *
 * \param [in] unit  Unit number.
 * \param [in] flist Allocated field list.
 * \param [in] fid   The fid of the field to be set.
 * \param [in] idx   The idx for array based fields.
 * \param [in] val   Value to set.
 *
 * \retval SHR_E_FULL No place to add new element to the field list.
 */
extern int
bcmsec_field_list_get(int unit,
                     bcmltd_fields_t *flist,
                     bcmlrd_fid_t fid,
                     uint32_t idx,
                     uint64_t *val);
/*!
 * \brief allocate memory to make the field list
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in] unit         Unit number.
 * \param [in] num_fields   number of fields.
 * \param [out] flist       allocated memory for field list
 */
extern int
bcmsec_field_list_alloc(int unit,
                       size_t num_fields,
                       bcmltd_fields_t *flist);

/*!
 * \brief frees the allocated field list
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in]          flist allocated field list .
 */
void
bcmsec_field_list_free(bcmltd_fields_t *flist);

/*!
 * \brief Update the existing field list with the new list
 *        for a logical table. Also links the fields.
 *
 * verifies if the operation requested is valid for the input list or not.
 *
 * \param [in] unit     Unit
 * \param [in] new_fld  New field_list
 * \param [in] ext_fld  Existing field list
 */
extern
int sec_fields_update_and_link(int unit, bcmltd_field_t *new_fld,
                               bcmltd_fields_t * ext_fld);
/*!
 * \brief Enable/Disable SEC interrupts
 *
 * \param [in] unit     Unit
 * \param [in] intr_num Interrupt number
 * \param [in] intr_func Callback function for the interrupt.
 * \param [in] enable   Enable/Disable
 */
extern int
bcmsec_interrupt_enable(int unit,
                        int intr_num,
                        bcmbd_intr_f intr_func,
                        bcmsec_intr_enable_t enable);

/*!
 * \brief Set event info
 *
 * \param [in] unit     Unit
 * \param [in] info Event info
 */
extern void
bcmsec_event_info_set(int unit,
                      bcmsec_event_info_t *info);
/*!
 * \brief Get event info
 *
 * \param [in] unit     Unit
 * \param [in] info Event info
 */
extern void
bcmsec_event_info_get(int unit,
                      bcmsec_event_info_t **info);

/*!
 * \brief Get the device specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [out] dev_info Device information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_dev_info_get(int unit, bcmsec_dev_info_t **dev_info);

/*!
 * \brief Get the physical port driver info.
 *
 * \param [in] unit Unit number.
 * \param [out] port_info Port information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_pport_info_get(int unit, bcmsec_pport_info_t **port_info);

/*!
 * \brief Get the logical port specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [out] port_info Port information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_lport_info_get(int unit, bcmsec_lport_info_t **port_info);

#endif /* BCMSEC_UTILS_INTERNAL_H */
