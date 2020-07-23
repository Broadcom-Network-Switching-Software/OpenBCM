/*! \file bcmecmp_common_imm.h
 *
 * ECMP imm handlers common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMECMP_COMMON_IMM_H
#define BCMECMP_COMMON_IMM_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmecmp/bcmecmp_db_internal.h>
#include <bcmimm/bcmimm_int_comp.h>

/*!
 * \brief In-memory entry validate callback function.
 *
 * This function is a callback function that a component can register with
 * the in-memory component to participate in the validation phase for specific
 * logical table. The callback contains the field values of the entry that
 * needed validation along with the key value identify the entry.
 * If the entry is valid this function should return no error (SHR_E_NONE),
 * otherwise it should return error code.
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key_fields This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data_field This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmecmp_lt_fields_fill_cb)(int unit,
                                    const bcmltd_field_t *field,
                                    bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief Initalize bcmecmp_lt_entry_t structure.
 *
 * Initializes bcmecmp_lt_entry_t structure member variables to apt initial
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table database source identifier.
 * \param [in] lt_entry Pointer to bcmecmp_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Invalid input parameter.
 */
extern int
bcmecmp_lt_entry_t_init(int unit,
                        bcmltd_sid_t sid,
                        bcmecmp_lt_entry_t *lt_entry);
/*!
 * \brief Logical table adaptors out fields populate routine.
 *
 * Get ECMP logical table field values from lt_entry and populate them in
 * logical table database out fields array.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry ECMP logical table entry data.
 * \param [in] include_key Include LT key field in the out fields list.
 * \param [out] out Logical table adaptors out field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failed to convert \c lt_entry to \c out.
 */
extern int
bcmecmp_lt_fields_populate(int unit,
                           bcmecmp_lt_entry_t *lt_entry,
                           bool include_key,
                           bcmltd_fields_t *out);

/*!
 * \brief Logical table entry input parameters validate function.
 *
 * Validate logical table entry input parameter values based on logical table
 * operation type.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode Logical table entry operation code.
 * \param [in] sid Logical table source identifier.
 * \param [in] lt_entry ECMP logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
extern int
bcmecmp_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmecmp_lt_entry_t *lt_entry);
/*!
 * \brief flex Logical table entry input parameters validate function.
 *
 * Validate logical table entry input parameter values based on logical table
 * operation type.
 *
 * \param [in] unit Unit number.
 * \param [in] opcode Logical table entry operation code.
 * \param [in] sid Logical table source identifier.
 * \param [in] lt_entry ECMP logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
extern int
bcmecmp_flex_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmecmp_flex_lt_entry_t *lt_entry);


/*!
 * \brief Get all current imm ECMP LT fields.
 *
 * Get the current fields from imm ECMP LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry ECMP LT entry data buffer.
 * \param [in] cb LT fields filling call back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
extern int
bcmecmp_lt_fields_lookup(int unit,
                         bcmecmp_lt_entry_t *lt_entry,
                         fn_lt_fields_fill_t cb);

/*!
 * \brief Update value of a field from IMM fields list.
 *
 * \param [in] unit Unit number.
 * \param [in] list IMM fields list.
 * \param [in] fid Fid to be updated.
 * \param [in] data Data to be updated.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND Fid not found.
 */
extern int
bcmecmp_lt_field_update(int unit,
                        bcmltd_fields_t *list,
                        uint32_t fid,
                        uint64_t data);

/*!
 * \brief Get all current imm ECMP flex LT fields.
 *
 * Get the current fields from imm ECMP flex LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry ECMP flex LT entry data buffer.
 * \param [in] cb LT fields filling call back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
extern int
bcmecmp_flex_lt_fields_lookup(int unit,
                         bcmecmp_flex_lt_entry_t *lt_entry,
                         fn_lt_fields_fill_t cb);
/*!
 * \brief Update sorted ECMP member fields.
 *
 * update ECMP member info in IMM with sorted member array.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry ECMP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
extern int
bcmecmp_lt_member_update(int unit,
                         bcmecmp_lt_entry_t *lt_entry);

/*!
 * \brief reallocate ECMP resources after warm boot.
 *
 * \param [in] unit Unit number.

 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL failure in retrieving HW info.
 */
extern int
bcmecmp_wb_itbm_ecmp_group_add(int unit);

/*!
 * \brief Initalize bcmecmp_flex_lt_entry_t structure.
 *
 * Initializes bcmecmp_flex_lt_entry_t structure member variables to apt initial
 * values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table database source identifier.
 * \param [in] lt_entry Pointer to bcmecmp_flex_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Invalid input parameter.
 */
extern int
bcmecmp_flex_lt_entry_t_init(int unit,
                             bcmltd_sid_t sid,
                             bcmecmp_flex_lt_entry_t *lt_entry);

/*!
 * \brief Fill ECMP member fields into lt_mfield array from cur entry.
 *
 * \param [in] unit Unit number.
 * \param [in] cur_lt_entry Current lt entry.
 * \param [in] member_tbl_id member tbl 0 or 1.
 * \param [in] lt_entry pointer to lt entry.
 * \param [in] replaceInvalidWithZeroes If true, Unspecified elements \
                                    replaced with 0 for HW programming.
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL failure.
 */
extern int
bcmecmp_flex_member_fields_fill(int unit,
                                bcmecmp_flex_lt_entry_t *cur_lt_entry,
                                int member_tbl_id,
                                bcmecmp_flex_lt_entry_t *lt_entry,
                                bool replaceInvalidWithZeroes);
#endif /* BCMECMP_COMMON_IMM_H */
