/*! \file bcmcth_imm_util.h
 *
 * BCMCTH Load Balance Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_IMM_UTIL_H
#define BCMCTH_IMM_UTIL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_table.h>

/*!
 * \brief Read IMM data from hardware.
 *
 * This function is a convenience function for doing PTM reads from
 * the IMM callbacks. The function parameterizes table instance and
 * index, and it hides rarely used PTM parameters.
 *
 * The function assumes that the provided output buffer is big enough
 * to hold a data entry corresponding to the physical table ID
 * provided.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] pt_id Physica table ID.
 * \param [in] tbl_inst Table instance.
 * \param [in] tbl_idx Table index.
 * \param [out] entry_data Buffer for read data.
 *
 * \retval 0 No errors
 */
extern int
bcmcth_imm_cb_read(int unit, bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                   int tbl_inst, int tbl_idx, void *entry_data);

/*!
 * \brief Write IMM data to hardware.
 *
 * This function is a convenience function for doing PTM writes from
 * the IMM callbacks. The function parameterizes table instance and
 * index, and it hides rarely used PTM parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] pt_id Physica table ID.
 * \param [in] tbl_inst Table instance.
 * \param [in] tbl_idx Table index.
 * \param [in] entry_data Buffer for write data.
 *
 * \retval 0 No errors
 */
extern int
bcmcth_imm_cb_write(int unit, bcmltd_sid_t lt_id, bcmdrd_sid_t pt_id,
                    int tbl_inst, int tbl_idx, void *entry_data);

/*!
 * \brief Interactive hw mem/reg table read for BCMCTH IMM.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_imm_pipe_ireq_read(
    int unit,
    bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id,
    int pipe,
    int index,
    void * entry_data);

/*!
 * \brief Interactive hw mem/reg table write for BCMCTH IMM.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table identifier.
 * \param [in] pt_id Memory table identifier.
 * \param [in] pipe Pipe of memory entry to write.
 * \param [in] index Index of memory entry to write.
 * \param [in] entry_data New data value of entry to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_imm_pipe_ireq_write(
    int unit,
    bcmltd_sid_t lt_id,
    bcmdrd_sid_t pt_id,
    int pipe,
    int index,
    void * entry_data);

/*!
 * \brief Free each field in the list.
 *
 * \param [in] unit    Unit number.
 * \param [in] in      Pointer to fields to be freed.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmcth_imm_fields_free(int unit, bcmltd_fields_t* in);

/*!
 * \brief Allocate BCMLT fields.
 *
 * \param [in] unit    Unit number.
 * \param [in] in      Pointer to fields to be allocated.
 * \param [in] num_fields      Number of fields to be allocated.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmcth_imm_fields_alloc(int unit, bcmltd_fields_t* in, size_t num_fields);

/*!
 * \brief Update value of a field from IMM fields list.
 *
 * \param [in] unit    Unit number.
 * \param [in] in      Pointer to fields to be searched.
 * \param [in] fid     Id of the field to be updated.
 * \param [in] data    Value of the field to be updated.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND Fail.
 */
int
bcmcth_imm_field_update(int unit,
                        bcmltd_fields_t *in,
                        uint32_t fid,
                        uint64_t data);

/*!
 * \brief Get value of a field from IMM fields list.
 *
 * \param [in] unit    Unit number.
 * \param [in] in      Pointer to fields to be searched.
 * \param [in] fid     Id of the field to be updated.
 * \param [in] data    Point of the field to be got.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND Fail.
 */
extern int
bcmcth_imm_fields_get(int unit,
                      const bcmltd_fields_t *in,
                      uint32_t fid,
                      uint64_t *data);

#endif /* BCMCTH_IMM_UTIL_H */
