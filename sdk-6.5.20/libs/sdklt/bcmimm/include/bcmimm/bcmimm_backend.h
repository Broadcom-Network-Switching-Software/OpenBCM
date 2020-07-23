/*! \file bcmimm_backend.h
 *
 *  In-memory back-end header file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMIMM_BACKEND_H
#define BCMIMM_BACKEND_H

#include <sal/sal_types.h>
#include <shr/shr_ha.h>

/*!
 * \brief In-Memory backend field ID size in bytes
 */
#define BCMIMM_BE_FID_SIZE  sizeof(uint32_t)

/*!
 * \brief Invalid BE table handle
 */
#define BCMIMM_BE_INVALID_TBL_HDL  0xFFFFFFFF
/*!
 * \brief In-Memory backend table handle type.
 */
typedef uint32_t bcmimm_be_tbl_hdl_t;

/*!
 * \brief Entry fields set.
 */
typedef struct bcmimm_be_fields_s {
    size_t count;   /*!< Number of fields */
    uint32_t *fid;  /*!< Array of size count for field IDs */
    /*!
     * Array of size count of field data. Each array element is of size given
     * to \ref bcmimm_be_table_create()
     */
    void *fdata;
} bcmimm_be_fields_t;

/*!
 * \brief Entry fields update set.
 */
typedef struct bcmimm_be_fields_upd_s {
    /*! Number of fields */
    size_t count;
    /*! Array of size count for field IDs */
    uint32_t *fid;
    /*! Array of size count of field operations */
    bool *del;
    /*!
     * Array of size count of field data. Each array element is of size given
     * to \ref bcmimm_be_table_create()
     */
    void *fdata;
} bcmimm_be_fields_upd_t;

/*!
 * \brief direct table - array field structure.
 *
 * Entries for direct tables are not compressed all the bits are used. This
 * structure passes to the insert, lookup, traverse delete and update functions
 * as an array. This structure is being used for direct tables that has one or
 * more fields as arrays.
 */
typedef struct {
    uint32_t id;  /*!< Field ID */
    uint32_t idx;  /*!< Index of the field in the array */
    uint64_t data; /*!< Field data */
} bcmimm_be_direct_tbl_field_array_t;

/*!
 * \brief direct table - basic field structure
 *
 * Entries for direct tables are not compressed all the bits are used. This
 * structure passes to the insert, lookup, traverse delete and update functions
 * as an array. This structure is being used for direct tables that doesn't have
 * any array fields. Therefore this structure is more compact.
 */
typedef struct {
    uint32_t id;  /*!< Field ID */
    uint64_t data; /*!< Field data */
} bcmimm_be_direct_tbl_field_basic_t;


/*!
 * \brief Initializes the back-end unit independent (generic) portion.
 *
 * This function initializes the back end generic module. It is called before
 * the per unit initialization occurs.
 *
 * \return SHR_E_NONE - successful.
 * \return SHR_E_MEMORY - failed to allocate the memory.
 */
extern int bcmimm_be_generic_init(void);

/*!
 * \brief Shutdown the back-end unit independent (generic) portion.
 *
 * This function freed up the resources allocated by the
 * \ref bcmimm_be_generic_init() function. This function should be called after
 * the per unit shutdown.
 */
extern void bcmimm_be_generic_shutdown(void);

/*!
 * \brief Initializes the back-end unit.
 *
 * This function initializes the back end module for a specific unit.
 *
 * \param [in] unit To initialize.
 *
 * \return SHR_E_NONE - successful.
 * \return SHR_E_MEMORY - failed to allocate the memory.
 */
extern int bcmimm_be_unit_init(int unit);

/*!
 * \brief Shutdown the back-end unit.
 *
 * This function freed up the resources allocated by the
 * \ref bcmimm_be_unit_init() function.
 */
extern void bcmimm_be_unit_shutdown(int unit);

/*!
 * \brief Initializes single back-end unit.
 *
 * This function Initializes the back-end tables associated with the unit.
 * If the warm parameter is true, it recovers the tables state by allocating
 * HA memory blocks.
 *
 * \param [in] unit This is the unit to initialize the back-end.
 * \param [in] comp_id This is the component ID to use (for HA).
 * \param [in] warm Indicates if the initialization is warm (where states
 * should be recovered from HA memory) or cold.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_be_init(int unit, shr_ha_mod_id comp_id, bool warm);

/*!
 * \brief Closes back-end tables for the given unit.
 *
 * This function is the inverse of \ref bcmimm_be_init(). It frees all
 * previously allocated resources associated with this unit.
 *
 * \param [in] unit This is the unit to close the back-end.
 *
 * \return SHR_E_NONE success, error code otherwise.
 */
extern int bcmimm_be_cleanup(int unit);

/*!
 * \brief Create back-end table.
 *
 * This function creates a back-end table. The table is associated with the
 * specified unit and component. The back end will allocate HA blocks using
 * the component ID and the sub component range from \c sub_comp_min to
 * \c sub_comp_max.
 *
 * \param [in] unit This is the unit to associate the table with.
 * \param [in] comp_id This is the component ID to use when allocating HA
 * block.
 * \param [in] sub_id This is the sub component id to use when
 * allocating HA block.
 * \param [in] key_size This is the size in bytes of the key portion. This
 * size is the total key size (meaning that the key is of a fixed size).
 * \param [in] data_size This is the size in bytes of a single data element
 * (field). Note that the size of the data element is fixed for all fields
 * in this table. The 'data' in this context means the following:
 *  - field data: (mandatory) that is the actual data attribute of the field
 *  - field idx: (optional) for tables with field array this indicates the
 *    index of the field in the array.
 * \param [in] rows This is the expected number of rows in the table.
 * \param [in] warm Indicate if it is cold or warm start.
 * \param [in] tbl_name The LT name.
 * \param [in] big_endian Indicates if the system data is big or small endian.
 * \param [out] hdl This is the table handle to use for any other reference
 * to this table.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_create(int unit,
                                  shr_ha_mod_id comp_id,
                                  shr_ha_sub_id sub_id,
                                  size_t key_size,
                                  size_t data_size,
                                  size_t rows,
                                  bool warm,
                                  const char *tbl_name,
                                  bool big_endian,
                                  bcmimm_be_tbl_hdl_t *hdl);

/*!
 * \brief Create back-end direct table.
 *
 * This function creates a back-end direct table. The table is associated with
 * the specified unit and component. The back end will allocate HA blocks using
 * the component ID and the sub component range from \c sub_comp_min to
 * \c sub_comp_max. This table is direct table which have two types of encoding:
 * a. Encoding of tables with one or more array fields.
 * b. Encoding of tables without any array fields. This table doesn't require
 * the index portion of the field.
 *
 * \param [in] unit This is the unit to associate the table with.
 * \param [in] comp_id This is the component ID to use when allocating HA
 * block.
 * \param [in] sub_id This is the sub component id to use when
 * allocating HA block.
 * \param [in] data_fields The maximal number of data fields in an entry
 * \param [in] rows The maximal number of rows in the table.
 * \param [in] start_idx The minimal allowed index.
 * \param [in] array Indicate if the table has array fields or not.
 * \param [in] warm Indicate if it is cold or warm start.
 * \param [in] tbl_name The LT name.
 * \param [out] hdl This is the table handle to use for any other reference
 * to this table.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_direct_table_create(int unit,
                                         shr_ha_mod_id comp_id,
                                         shr_ha_sub_id sub_id,
                                         size_t data_fields,
                                         size_t rows,
                                         uint32_t start_idx,
                                         bool array,
                                         bool warm,
                                         const char *tbl_name,
                                         bcmimm_be_tbl_hdl_t *hdl);

/*!
 * \brief Destroy previously allocated back-end table.
 *
 * This function closes a back-end table and free all its resources.
 * The table must be open first with a call to \ref bcmimm_be_table_create().
 *
 * \param [in] hdl This is the table handle.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_release(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Free HA memory associated with back-end table.
 *
 * This function free the HA block that was previously allocated for the
 * back-end table. This function is being called during warm boot before
 * the back-end table was created.
 *
 * \param [in] hdl This is the table handle.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_MEMORY Failed to associate HA block with the inpyt parameters.
 */
extern int bcmimm_be_table_free(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief The number of entries in the table
 *
 * This function returns the number of entries currently available inside
 * the table specified via \c hdl.
 *
 * \param [in] hdl This is the table handle.
 *
 * \return the number of elements in the table. Return of 0 indicates either
 * table is empty or that the handle is invalid.
 */
extern size_t bcmimm_be_table_size_get(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Create back-end table snapshot.
 *
 * This function allocates memory and records the key field(s) of every entry
 * within the newly allocated memory. By doing so the system creates a snapshot
 * of all the entries within the specific LT.
 * If a snapshot already exist it will be freed and a new snapshot will be
 * created instead.
 *
 * \param [in] hdl This is the table handle.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_NOT_FOUND The table is empty.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
extern int bcmimm_be_table_snapshot_create(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Free back-end table snapshot.
 *
 * This function frees the memory that was used for the LT snapshot. A snapshot
 * can be created by calling \ref bcmimm_be_table_snapshot_create().
 *
 * \param [in] hdl This is the table handle.
 *
 * \retval None.
 */
extern void bcmimm_be_table_snapshot_free(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Get the first entry of the table.
 *
 * This function enumerates the entries of the table specified via \c hdl and
 * returns the first entry of that table. The search may start from a
 * pre-existing snapshot created by \ref bcmimm_be_table_snapshot_create()
 * or can be started from the first entry with the lower internal index.
 * Note that multiple threads may traverse this table simultaneously
 * without impacting each other.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] use_snapshot Indicate if the search should use the snapshot
 * search or should simply start a raw search on the table. To use snapshot
 * the caller must first call \ref bcmimm_be_table_snapshot_create().
 * \param [out] key This is a pointer to store the key value. The size of
 * the key memory must be at least as it was provided during
 * \ref bcmimm_be_table_create().
 * \param [in,out] out This is the structure that contains the content of the
 * first entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_get_first(bcmimm_be_tbl_hdl_t hdl,
                                     bool use_snapshot,
                                     void *key,
                                     bcmimm_be_fields_t *out);

/*!
 * \brief Get the next entry of the table based on context.
 *
 * This function returns the next entry of the table specified via \c hdl. The
 * next entry is the entry followed by the entry that matches the input key.
 * Using the input key as context allows multiple threads to simultaneously
 * traverse the same table.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] use_snapshot Indicate if the search should use the snapshot
 * search or should search on the raw table. To use snapshot
 * the caller must first call \ref bcmimm_be_table_snapshot_create().
 * \param [in] in_key This is the key of the previous entry that was traversed.
 * \param [out] out_key This is a pointer to store the key value. The size of
 * the key memory must be at least as it was provided during
 * \ref bcmimm_be_table_create().
 * \param [in,out] out This is the structure that contains the content of the
 * next entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_get_next(bcmimm_be_tbl_hdl_t hdl,
                                    bool use_snapshot,
                                    void *in_key,
                                    void *out_key,
                                    bcmimm_be_fields_t *out);

/*!
 * \brief Clear the content of the table.
 *
 * This function clears the content of the table by deleting all the entries
 * associated with this table. The table state identical to the state of the
 * table after \ref bcmimm_be_table_create().
 *
 * \param [in] hdl This is the table handle.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_table_clear(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Retrieve the entry count of a table.
 *
 * This function retrieves the entry count of a table.
 *
 * \param [in] hdl This is the table handle.
 *
 * \return The entry count.
 */
extern uint32_t bcmimm_be_table_cnt_get(bcmimm_be_tbl_hdl_t hdl);

/*!
 * \brief Insert an entry to the table.
 *
 * This function inserts an entry into the table specified via \c hdl. The
 * entry composed of key and data portions, where the data has
 * \c data_elements elements. If the entry already exists, it will be
 * override by the new values.
 * If a field was present before and it is no longer valid, it will be
 * deleted.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [in] in This is the input to the entry. It contains two arrays, both
 * with \c in->count elements. The first array contains the field IDs and the
 * second contains the field data. See \ref bcmimm_be_fields_t for more
 * information.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_insert(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  const bcmimm_be_fields_t *in);

/*!
 * \brief Delete an entry from the table.
 *
 * This function deletes an entry from the table specified via \c hdl. The
 * deletion is done based on the key value.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_delete(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key);

/*!
 * \brief Lookup an entry in the table.
 *
 * This function looks up for an entry in the table specified via \c hdl. The
 * lookup is done based on the key value.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [out] out This is the structure that contains the content of the
 * entry. The count field of \c out contains initially (input)the arrays
 * size. It will be updated to the actual number of fields that were copied
 * into the \c out parameter.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_lookup(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  bcmimm_be_fields_t *out);

/*!
 * \brief Update an entry in the table.
 *
 * This function updates the content of an entry in the table specified via
 * \c hdl. The entry identified based on the key value. Once found, the fields
 * will be updated according to their specified operation (in->oper). Existing
 * fields that are not specified by the \c in parameter will remain the same.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [in] in This is the structure that contains the content of the
 * modified fields. The count field of \c in contains the arrays size.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_update(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  const bcmimm_be_fields_upd_t *in);

/*!
 * \brief Lock an entry of the table.
 *
 * This function locks an entry from the table specified via \c hdl. Locked
 * entry can not be modified (or deleted). This functionality is useful
 * if the entry being referred by another field in a different logical table.
 * By default every entry is unlocked unless explicitly locked.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_lock(bcmimm_be_tbl_hdl_t hdl,
                                const void *key);

/*!
 * \brief Unlock an entry of the table.
 *
 * This function unlocks an entry that was previously locked. Once unlocked,
 * the entry can be modified or deleted.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_unlock(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key);

/*!
 * \brief Obtain entry lock reference count.
 *
 * This function retrieves the lock reference count of an entry.
 * On success, the value will be placed into \c lock_cnt.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key This is a pointer to the key value. The size of
 * the key was provided during \ref bcmimm_be_table_create().
 * \param [out] lock_cnt The value of the lock reference count will be placed
 * into this output variable.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_get_lock_cnt(bcmimm_be_tbl_hdl_t hdl,
                                  const void *key,
                                  uint32_t *lock_cnt);

/*!
 * \brief Resize (up) the data size
 *
 * This function typically will call after s/w upgrade where the LT
 * changes causes the data to be larger (possibly by adding array field).
 * The function will replace all the fields into larger fields.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] d_size This is the desired (new) data size.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_data_resize(bcmimm_be_tbl_hdl_t hdl, size_t d_size);

/*!
 * \brief Overwrite entry field ID and index.
 *
 * This function being used only for ISSU purposes where the field IDs of the
 * saved entries require update to the current ID values.
 *
 * \param [in] hdl This is the table handle.
 * \param [in] key The entry key.
 * \param [in] in The entry new content.
 *
 * \return SHR_E_NONE on success, error code otherwise.
 */
extern int bcmimm_be_entry_overwrite(bcmimm_be_tbl_hdl_t hdl,
                                     const void *key,
                                     bcmimm_be_fields_t *in);

#endif /* BCMIMM_BACKEND_H */
