/*! \file bcmimm_direct.h
 *
 * In-memory direct table handler. This file contains the declerations of
 * all the in-memory direct table custom handler functions. In general, the API
 * and its behavior is being documented by the BCMLTA design document.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMIMM_DIRECT_H
#define BCMIMM_DIRECT_H

#include <bcmltd/bcmltd_handler.h>

/*!
 * \brief Validates entry operation on specific logical table.
 *
 * This function validates if a given operation (given as \c opcode) is
 * viable with the specific entry and logical table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] opcode Is the operation to validate.
 * \param [in] in Contains the entry information.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_validate(int unit,
                                  bcmlt_opcode_t opcode,
                                  const bcmltd_fields_t *in,
                                  const bcmltd_generic_arg_t *arg);


/*!
 * \brief Inserts an entry to a specific logical direct table.
 *
 * This function inserts an entry into a logical table. Note that this
 * operation has already been validated by \ref bcmimm_direct_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Contains the entry information.
 * \param [out] out Not applicable for this operation.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_insert(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Inserts an entry to a specific logical direct table and allocate it.
 *
 * This function inserts an entry into a logical table after allocating its
 * index. Note that this operation has already been validated
 * by \ref bcmimm_direct_validate().
 * The allocated index is returned in the \c out parameter.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Contains the entry information.
 * \param [out] out Not applicable for this operation.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_insert_allocate(int unit,
                                         const bcmltd_op_arg_t *op_arg,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief Lookup for an entry in a specific logical direct table.
 *
 * This function lookup for an entry in a logical table. Note that this
 * operation has already been validated by \ref bcmimm_direct_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Contains the entry information.
 * \param [out] out The output information from the lookup will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_lookup(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Deletes an entry from a specific logical direct table.
 *
 * This function deletes an entry from a logical table. Note that this
 * operation has already been validated by \ref bcmimm_direct_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Contains the entry information.
 * \param [out] out Not applicable for this operation.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_delete(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Updates an entry from a specific logical direct table.
 *
 * This function deletes an entry from a logical table. Note that this
 * operation has already been validated by \ref bcmimm_direct_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Contains the entry information.
 * \param [out] out Not applicable for this operation.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_update(int unit,
                                const bcmltd_op_arg_t *op_arg,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_generic_arg_t *arg);

/*!
 * \brief Commits a transaction.
 *
 * This function commits a transaction that was previously defined by
 * the various insert/delete/update and lookup operations. A transaction
 * may contains multiple operations that were staged before.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_direct_commit())
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_commit(int unit,
                               uint32_t trans_id,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Aborts a transaction.
 *
 * This function aborts a transaction that was previously staged via
 * the various insert/delete/update and lookup operations. As a result, all
 * the staging will be cleaned up and not actual change will take place.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * The actual operation being execute only once the transaction is being
 * committed (see \ref bcmimm_direct_commit())
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_abort(int unit,
                              uint32_t trans_id,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table enumeration get first.
 *
 * This function starts the enumeration operation by providing the first
 * entry in the table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Not applicable for this operation.
 * \param [out] out The content of the first entry will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_first(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table enumeration get next.
 *
 * This function continues the enumeration operation by providing the next
 * entry in the table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Operation argument contains information regarding the
 * operation such as transaction ID and operation attributes.
 * \param [in] in Is the entry content that was returned in previous call
 * to \ref bcmimm_direct_first or \ref bcmimm_direct_next.
 * \param [out] out The content of the next entry will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_next(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Current entries in use.
 *
 * This function provides the current number of entries that are in use.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * \param [in] sid logical table ID.
 * \param [in] table_arg Table arguments.
 * \param [out] table_data The number of in use entries.
 * \param [in] arg Handler arguments.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_table_entry_inuse_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_inuse_arg_t *table_arg,
        bcmltd_table_entry_inuse_t *table_data,
        const bcmltd_generic_arg_t *arg);

/*!
 * \brief Set the table max-entries attribute.
 *
 * This function sets the max-entries attribute of a table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * \param [in] sid logical table ID.
 * \param [in] table_arg Table arguments.
 * \param [in] table_data The number of in use entries.
 * \param [in] arg Handler arguments.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_table_max_entries_set(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_max_entries_arg_t *table_arg,
        const bcmltd_table_max_entries_t *table_data,
        const bcmltd_generic_arg_t *arg);

/*!
 * \brief Get the table actual entry limit.
 *
 * This function gets the actual entry limit. The entry limit might be
 * different from max entries in case that there are less resources allocated
 * to the component.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] trans_id Is the transaction ID associated with this operation.
 * \param [in] sid logical table ID.
 * \param [in] table_arg Table arguments.
 * \param [out] table_data The actual entry limit.
 * \param [in] arg Handler arguments.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_direct_table_entry_limit_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_limit_arg_t *table_arg,
        bcmltd_table_entry_limit_t *table_data,
        const bcmltd_generic_arg_t *arg);

#endif

