/*! \file bcmltm_md_pthru_op.h
 *
 * Logical Table Manager - Pass Thru Opcode.
 *
 * This file contains routines to create the opcode metadata
 * for Pass Thru tables.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_PTHRU_OP_H
#define BCMLTM_MD_PTHRU_OP_H

/*!
 * \brief Initialize PT Pass Thru opcode metadata.
 *
 * This routine initializes the opcode metadata for PT Pass Thru tables and
 * creates internal data structures to maintain the information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid_max_count Maximum number of PT Pass Thru tables on unit.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_op_init(int unit,
                        uint32_t sid_max_count);

/*!
 * \brief Cleanup PT Pass Thru opcode metadata.
 *
 * This routine frees any resources allocated by the PT Pass Thru
 * opcode metadata module.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmltm_md_pthru_op_cleanup(int unit);

/*!
 * \brief Create table opcode metadata.
 *
 * This routine creates the opcode metadata for the given
 * table.
 *
 * The PT Pass Thru opcode module stores the information of only 1 table
 * at any given time.  The function will only proceed to create
 * new data if there is no table metadata stored at the time.
 * If the data contains a valid table, the caller first
 * needs to destroy the existing table information.
 *
 * - If opcode metadata contains a valid cached table and provided table ID
 *   is new, an error is returned.
 * - If provided table ID matches existing cached table ID,
 *   there is no action.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 */
extern int
bcmltm_md_pthru_op_table_create(int unit,
                                bcmdrd_sid_t sid);

/*!
 * \brief Destroy table opcode metadata.
 *
 * This routine destroys the opcode metadata for the given
 * table.
 *
 * The table ID BCMDRD_INVALID_ID has special meaning.  It indicates
 * to destroy the opcode metadata for the current cached table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_op_table_destroy(int unit,
                                 bcmdrd_sid_t sid);

/*!
 * \brief Get the opcode metadata.
 *
 * This routine gets the PT opcode list metadata for given table
 * and PT opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Table ID.
 * \param [in] opcode PT opcode to get metadata for.
 * \param [out] op_md_ptr Returning pointer to PT opcode metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_pthru_op_get(int unit,
                       bcmdrd_sid_t sid,
                       bcmlt_pt_opcode_t opcode,
                       bcmltm_lt_op_md_t **op_md_ptr);

#endif /* BCMLTM_MD_PTHRU_OP_H */
