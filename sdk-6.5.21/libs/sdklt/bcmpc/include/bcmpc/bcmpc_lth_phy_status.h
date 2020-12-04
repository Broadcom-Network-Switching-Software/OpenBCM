/*! \file bcmpc_lth_phy_status.h
 *
 * Logical Table Custom Handlers for PC_PHY_STATUS.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the logical table custom handlers for PC_PHY_STATUS.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_LTH_PHY_STATUS_H
#define BCMPC_LTH_PHY_STATUS_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PC_PHY_STATUS table validation.
 *
 * \param [in] unit Unit Number.
 * \param [in] opcode LT opcode.
 * \param [out] in Input field values.
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_validate(int unit,
                              bcmlt_opcode_t opcode,
                              const bcmltd_fields_t *in,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS table insert.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_insert(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS table lookup.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_lookup(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS table delete.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_delete(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS table update.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_update(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS traverse get first.
 *
 * Get the first PC_PHY_STATUS LT entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values (not applicable).
 * \param [out] out Output field values.
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_first(int unit,
                           const bcmltd_op_arg_t *op_arg,
                           const bcmltd_fields_t *in,
                           bcmltd_fields_t *out,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS traverse get next.
 *
 * Get the next PC_PHY_STATUS LT entry.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_next(int unit,
                          const bcmltd_op_arg_t *op_arg,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PHY_STATUS entries in-use get.
 *
 * Get the number of entres in use PC_PHY_STATUS LT.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  sid             Table ID.
 * \param [in]  table_arg       Table arguments.
 * \param [out] table_data      Returning table entry in-use count.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmpc_lth_phy_status_table_entry_inuse_get(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_entry_inuse_arg_t *table_arg,
                     bcmltd_table_entry_inuse_t *table_data,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table max entries set function pointer type.
 *
 * This function sets the max entries for the given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  sid             Table ID.
 * \param [in]  table_arg       Table arguments.
 * \param [in]  table_data      Table max entries to set.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmpc_lth_phy_status_table_max_entries_set(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_max_entries_arg_t *table_arg,
                     const bcmltd_table_max_entries_t *table_data,
                     const bcmltd_generic_arg_t *arg);

#endif /* BCMPC_LTH_PHY_STATUS_H */
