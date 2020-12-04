/*! \file bcmpc_lth_pm_prop.h
 *
 * Logical Table Custom Handlers for PC_PM_PROP.
 *
 * Declaration of the structures, enumerations, and functions to implement
 * the logical table custom handlers for PC_PM_PROP.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_LTH_PM_PROP_H
#define BCMPC_LTH_PM_PROP_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief PC_PM_PROP table validation.
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
bcmpc_lth_pm_prop_validate(int unit,
                           bcmlt_opcode_t opcode,
                           const bcmltd_fields_t *in,
                           const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PM_PROP table insert.
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
bcmpc_lth_pm_prop_insert(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PM_PROP table lookup.
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
bcmpc_lth_pm_prop_lookup(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PM_PROP table delete.
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
bcmpc_lth_pm_prop_delete(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PM_PROP table update.
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
bcmpc_lth_pm_prop_update(int unit,
                         const bcmltd_op_arg_t *op_arg,
                         const bcmltd_fields_t *in,
                         bcmltd_fields_t *out,
                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PM_PROP traverse get first.
 *
 * Get the first PC_PM_PROP LT entry.
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
bcmpc_lth_pm_prop_first(int unit,
                        const bcmltd_op_arg_t *op_arg,
                        const bcmltd_fields_t *in,
                        bcmltd_fields_t *out,
                        const bcmltd_generic_arg_t *arg);

/*!
 * \brief PC_PM_PROP traverse get next.
 *
 * Get the next PC_PM_PROP LT entry.
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
bcmpc_lth_pm_prop_next(int unit,
                       const bcmltd_op_arg_t *op_arg,
                       const bcmltd_fields_t *in,
                       bcmltd_fields_t *out,
                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief Logical table entries in-use get.
 *
 * This function returns the entry in-use count for the given table.
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
bcmpc_lth_pm_prop_table_entry_inuse_get(int unit,
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
bcmpc_lth_pm_prop_table_max_entries_set(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_max_entries_arg_t *table_arg,
                     const bcmltd_table_max_entries_t *table_data,
                     const bcmltd_generic_arg_t *arg);
#endif /* BCMPC_LTH_PM_PROP_H */
