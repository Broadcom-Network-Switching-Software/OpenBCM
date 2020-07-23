/*! \file bcmimm_basic.h
 *
 * In-memory basic table handler. This file contains the declerations of
 * all the in-memory basic custom handler functions. In general, the API
 * and its behavior is being documented by the BCMLTA design document.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMIMM_BASIC_H
#define  BCMIMM_BASIC_H

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
extern int bcmimm_basic_validate(int unit,
                                 bcmlt_opcode_t opcode,
                                 const bcmltd_fields_t *in,
                                 const bcmltd_generic_arg_t *arg);

/*!
 * \brief Inserts an entry to a specific logical table.
 *
 * This function inserts an entry into a logical table. Note that this
 * operation has already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Contains the entry information.
 * \param [out] out Output fields. Not used in this function.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_insert(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Inserts an entry to a specific logical table and allocate the key.
 *
 * This function inserts an entry into a logical table while allocating the
 * key. Note that this operation has already been validated
 * by \ref bcmimm_basic_validate()
 * The allocated index is returned in the \c out parameter.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Contains the entry information.
 * \param [out] out Output fields. Not used in this function.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_insert_alloc(int unit,
                                     const bcmltd_op_arg_t *op_arg,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_generic_arg_t *arg);


/*!
 * \brief Lookup for an entry in a specific logical table.
 *
 * This function lookup for an entry in a logical table. Note that this
 * operation has already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Contains the entry information.
 * \param [out] out The output information from the lookup will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_lookup(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Deletes an entry from a specific logical table.
 *
 * This function deletes an entry from a logical table. Note that this
 * operation has already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Contains the entry information.
 * \param [out] out Output fields. Not used in this function.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_delete(int unit,
                               const bcmltd_op_arg_t *op_arg,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out,
                               const bcmltd_generic_arg_t *arg);

/*!
 * \brief Updates an entry from a specific logical table.
 *
 * This function deletes an entry from a logical table. Note that this
 * operation has already been validated by \ref bcmimm_basic_validate()
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Contains the entry information.
 * \param [out] out Output fields. Not used in this function.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_update(int unit,
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
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_commit(int unit,
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
 * committed (see \ref bcmimm_basic_commit())
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_abort(int unit,
                              uint32_t trans_id,
                              const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table enumeration get first.
 *
 * This function starts the enumeration operation by providing the first
 * entry in the table.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Not applicable for this function.
 * \param [out] out The content of the first entry will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_first(int unit,
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
 * \param [in] op_arg Is the operational arguments.
 * \param [in] in Is the entry content that was returned in previous call
 * to \ref bcmimm_basic_first or \ref bcmimm_basic_next.
 * \param [out] out The content of the next entry will be placed with
 * this parameter.
 * \param [in] arg Contains extra information and parameters. This parameter
 * is not being used in this implementation.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_next(int unit,
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
extern int bcmimm_basic_table_entry_inuse_get(
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
extern int bcmimm_basic_table_max_entries_set(
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
extern int bcmimm_basic_table_entry_limit_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_limit_arg_t *table_arg,
        bcmltd_table_entry_limit_t *table_data,
        const bcmltd_generic_arg_t *arg);

/*!
 * \brief Initialize the custom handler and its associated logical tables.
 *
 * This function finds the list of all the in-memory LT in the \c arg
 * parameter. For each of these LT, this function prepares the associated
 * field information and buffers required to perform the desired operations.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] arg Contains logical table information and parameters. The
 * init function prepare data structures for each of the LT listed in this
 * parameter.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_init(int unit,
                             const bcmltd_handler_lifecycle_arg_t *arg);

/*!
 * \brief Clean-up the custom handler and its resources.
 *
 * This function cleans up all resources associated with this custom handler.
 *
 * \param [in] unit Is the unit associated with the logical table.
 * \param [in] arg Contains logical table information and parameters.
 *
 * \return SHR_E_NONE of the operation is viable and error code otherwise.
 */
extern int bcmimm_basic_cleanup(int unit,
                                const bcmltd_handler_lifecycle_arg_t *arg);

#endif
