/*! \file bcmcth_trunk_vp_common_imm.h
 *
 * TRUNK_VP imm handlers common functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TRUNK_VP_COMMON_IMM_H
#define BCMCTH_TRUNK_VP_COMMON_IMM_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmcth/bcmcth_trunk_vp_db_internal.h>
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
 * \param [in] field imm fields.
 * \param [in] lt_entry LT entry information.
 * The callback can use this pointer to retrieve some context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (bcmcth_trunk_vp_lt_fields_fill_cb)(int unit,
                                    const void *field,
                                    void *lt_entry);

/*!
 * \brief Initalize bcmcth_trunk_vp_lt_entry_t structure.
 *
 * Initializes bcmcth_trunk_vp_lt_entry_t structure member
 * variables to apt initial values.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table database source identifier.
 * \param [in] lt_entry Pointer to bcmcth_trunk_vp_lt_entry_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Invalid input parameter.
 */
extern int
bcmcth_trunk_vp_lt_entry_t_init(int unit,
                        bcmltd_sid_t sid,
                        bcmcth_trunk_vp_lt_entry_t *lt_entry);
/*!
 * \brief Logical table adaptors out fields populate routine.
 *
 * Get TRUNK_VP logical table field values from lt_entry and populate them in
 * logical table database out fields array.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_entry TRUNK_VP logical table entry data.
 * \param [in] include_key Include LT key field in the out fields list.
 * \param [out] out Logical table adaptors out field array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failed to convert \c lt_entry to \c out.
 */
extern int
bcmcth_trunk_vp_lt_fields_populate(int unit,
                           bcmcth_trunk_vp_lt_entry_t *lt_entry,
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
 * \param [in] lt_entry TRUNK_VP logical table entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Input parameter failed validation check.
 */
extern int
bcmcth_trunk_vp_lt_input_params_validate(int unit,
                                 bcmimm_entry_event_t opcode,
                                 bcmltd_sid_t sid,
                                 bcmcth_trunk_vp_lt_entry_t *lt_entry);

/*!
 * \brief Get all current imm TRUNK_VP LT fields.
 *
 * Get the current fields from imm TRUNK_VP LT
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry TRUNK_VP LT entry data buffer.
 * \param [in] cb LT fields filling call back.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
extern int
bcmcth_trunk_vp_lt_fields_lookup(int unit,
                                 bcmcth_trunk_vp_lt_entry_t *lt_entry,
                                 bcmcth_trunk_vp_lt_fields_fill_cb *cb);

/*!
 * \brief Update sorted TRUNK_VP member fields.
 *
 * update TRUNK_VP member info in IMM with sorted member array.
 *
 * \param [in] unit Unit number.
 * \param [out] lt_entry TRUNK_VP LT entry data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Faild to geet current LT entry fields.
 */
extern int
bcmcth_trunk_vp_lt_member_update(int unit,
                                 bcmcth_trunk_vp_lt_entry_t *lt_entry);
#endif /* BCMCTH_TRUNK_VP_COMMON_IMM_H */
