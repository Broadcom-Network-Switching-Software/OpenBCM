/*! \file bcmltm.h
 *
 * BCMLTM Module Interface.
 *
 * Logical Table Manager.
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the Logical Table Manager module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_H
#define BCMLTM_H

#include <bcmltm/bcmltm_types.h>
#include <bcmptm/bcmptm_types.h>

/*!
 *
 * \brief Initialize the metadata for managing the LTs on a specific unit.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_init(int unit, bool warm);

/*!
 * \brief Cleanup LTM metadata on a specific unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_cleanup(int unit);

/*!
 * \brief Entry operation functions.
 *
 * All the entry operation functions receive a pointer to bcmltm entry.
 * The pointer is obtained by the caller to the operation functions and
 * should remain valid until one of the following occur:
 * 1. A call to bcmltm_abort() with match transaction ID occur. Once
 * the bcmltm_abort() returns the entry pointer no longer valid.
 * 2. Once the transaction callback function (notif_fn) has been called.
 * The entry is considered invalid once the callback function has returned.
 *
 * The pointer to the entry will be freed by the caller to the operation
 * functions.
 */

/*!
 *
 * \brief Process one LT entry insert operation.
 *
 * The LTM implementation of a single LT entry insert operation, which
 * creates a new element with the specified key and value fields.
 * Unspecified fields are assigned the software default.
 * Index tables track whether the entry is in-use and reject a second
 * insert operation.
 * Index with Allocation tables track whether the entry is reserved
 * and reserve the index if not.
 * Keyed table entries may be rejected by the PTM if insufficient
 * resources exist to place the entry.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_insert(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry lookup operation.
 *
 * The LTM implementation of a single LT entry lookup operation.
 * The entry with matching key must have been previously supplied
 * with the INSERT command.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_lookup(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry delete operation.
 *
 * The LTM implementation of a single LT entry delete operation.
 *
 * Simple index tables will have the entry matching the supplied key
 * returned to software default field values.  The in-use status is
 * cleared.
 *
 * Keyed table entries will be removed from the device resource.
 *
 * Index will Allocation tables may supply a range argument in
 * addition to the index key value.  If the range value is not
 * supplied, it is presumed to mean a range of 1.
 * The index key value is be interpreted as a base index.
 * For each index in the sequence [base_index, base_index + range - 1]
 * these steps are taken:
 *  1) Write corresponding PT entries to the device to set all
 *     fields to software defaults.
 *        -- More complex tables may require profile reference count
 *           management, aggregation list remove, etc.
 *  2) Clear the in-use status.
 *  3) Mark the entry index as unreserved.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_delete(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry update operation.
 *
 * The LTM implementation of a single LT entry update operation, which
 * will change only the specified values fields for the entry which
 * matches the given key field.
 * This operation also permits the removal of individual fields, which
 * are returned to software default and/or removed from (P-)PDD.
 * The LT entry with this key must have been previously supplied by the
 * INSERT comment.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_update(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry traverse start operation.
 *
 * The LTM implementation of a single LT entry traverse start operation,
 * which will return the fields of the first entry in a LT.
 * For index LTs, the first entry is the first index within the in-use
 * bitmap.
 * For Keyed LTs, the PTRM determines an ordering for the entries which
 * it tracks.  This tracking begins for a LTID with this traverse start.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_traverse_first(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry traverse next operation.
 *
 * The LTM implementation of a single LT entry traverse next operation,
 * which will return the fields of the next entry in a LT.  The key
 * fields of the previous entry are supplied to this operation.
 * For index LTs, the next entry is the subsequent index of the in-use
 * bitmap.
 * For Keyed LTs, the PTRM determines an ordering for the entries which
 * it tracks.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_traverse_next(bcmltm_entry_t *entry);

/*
 * PassThru exclusive functions
 */

/*!
 *
 * \brief Process one LT entry PT set operation.
 *
 * The LTM implementation of a single LT entry PT set operation.
 * Constructs a PT entry for the indicated physical resource with
 * each fields set to either a provided API field value or
 * the hardware default field value.
 * This entry is written to the device resource indicated by the
 * supplied index field value.  Some device resources may accept
 * additional memory parameters to specify the exact entry location.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_set(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT modify operation.
 *
 * The LTM implementation of a single LT entry PT modify operation.
 * The PT entry indicated by the index field value plus other memory
 * parameters is retrieved from the PTM.  The supplied API field values
 * overwrite the current physical field value.  Then the entry is
 * written back to the device resource.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_modify(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT get operation.
 *
 * The LTM implementation of a single LT entry PT get operation.
 * The PT entry indicated by the index field value plus other memory
 * parameters is retrieved from the PTM. The physical field values
 * are copied to the API output field list.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_get(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT clear operation.
 *
 * The LTM implementation of a single LT entry PT clear operation.
 * The PT entry indicated by the index field value plus other memory
 * parameters is written with an entry containing all physical fields
 * set to the hardware default field value.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_clear(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT insert operation.
 *
 * The LTM implementation of a single LT entry PT insert operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_insert(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT delete operation.
 *
 * The LTM implementation of a single LT entry PT delete operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_delete(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT lookup operation.
 *
 * The LTM implementation of a single LT entry PT lookup operation.
 *
 * \param [in] entry pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_lookup(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT FIFO pop operation.
 *
 * The LTM implementation of a single LT entry PT FIFO pop operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_fifo_pop(bcmltm_entry_t *entry);

/*!
 *
 * \brief Process one LT entry PT FIFO push operation.
 *
 * The LTM implementation of a single LT entry PT FIFO push operation.
 *
 * \param [in] entry Pointer to LTM entry specification.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_entry_pt_fifo_push(bcmltm_entry_t *entry);

/*!
 *
 * \brief Commit all entries with transaction ID.
 *
 * All entries with the specified ID should be committed to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID to commit. All entries belonging
 *              to this transaction will be committed.
 * \param [in] notif_fn Notification callback to application.
 * \param [in] context Notification callback cookie.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_commit(int unit,
              uint32_t trans_id,
              bcmptm_trans_cb_f notif_fn,
              void *context);

/*!
 *
 * \brief Abort all entries with transaction ID.
 *
 * The operation for all entries with the specified ID should
 * be aborted and the state should be restored to the state
 * before the entry(ies) with this ID were installed.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID to abort. All entries belonging
 *              to this transaction will be aborted.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmltm_abort(int unit,
             uint32_t trans_id);


/*!
 * \brief Increment LT_STATS field by LTID.
 *
 * Wrapper for auto-incrementing various LT_STATS values given the LTID
 * of the table to update.  It must verify LT in transaction and
 * retrieve the latest LT state.  This function is intended for use
 * by modules outside of the LTM implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical Table ID.
 * \param [in] stat_field FID indicating which stat is to increment.
 */
extern void
bcmltm_stats_increment_lt(int unit,
                          uint32_t ltid,
                          uint32_t stat_field);

/*!
 * \brief Retrieve number of LT in-use entries.
 *
 * This function is intended to be registered with PTM to permit
 * access to the number of entries currently in-use for a logical
 * table by other modules.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction identifier.
 * \param [in] ltid Logical Table ID.
 * \param [out] inuse_count Return pointer for number of in-use entries.
 */
extern int
bcmltm_inuse_count_get(int unit,
                       uint32_t trans_id,
                       uint32_t ltid,
                       uint32_t *inuse_count);


#endif /* BCMLTM_H */
