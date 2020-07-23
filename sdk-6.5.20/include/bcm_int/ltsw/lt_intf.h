/*! \file lt_intf.h
 *
 * LT Wrapper interface header file.
 * This file contains the lightweight wrapper of SDKLT LT APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LT_INTF_H
#define BCMI_LT_INTF_H

#include <bcmlt/bcmlt.h>

/******************************************************************************/
/*           Encapsulation for Complete SDKLT LT API Sequence                 */
/******************************************************************************/

/*! Recommended Maximum number of fields in LT entry information. */
#define BCMI_LT_FIELD_MAX   32

/*! Field value can be represented by a symbol string. */
#define BCMI_LT_FIELD_F_SYMBOL     (1 << 0)
/*! Field contents are represented by an array. */
#define BCMI_LT_FIELD_F_ARRAY      (1 << 1)
/*! Field to set. */
#define BCMI_LT_FIELD_F_SET        (1 << 2)
/*! Field to get. */
#define BCMI_LT_FIELD_F_GET        (1 << 3)
/*! Indicate if an element in scalar or symbol array is valid. */
#define BCMI_LT_FIELD_F_ELE_VALID  (1 << 4)

/*!
 * \brief LT field information.
 *
 * This data structure is used to identify the field, its type and value.
 */
typedef struct bcmi_lt_field_s {
    /*! Field name. */
    const char      *fld_name;
    /*! Field attribute (ref LT_FIELD_F_xxx). */
    uint16_t        flags;
    /*! Indicating the index of the element in an array. */
    uint16_t        idx;
    /*! The field data portion is a union of possible types. */
    union {
        /*! scalar data. */
        uint64_t    val;
        /*! symbol data. */
        const char  *sym_val;
    } u;
} bcmi_lt_field_t;

/*!
 * \brief LT entry information.
 *
 * This data structure is used to identify the entry fields and attributes.
 */
typedef struct bcmi_lt_entry_s {
    /*! Fields information array. */
    bcmi_lt_field_t   *fields;

    /*!
     * The number of fields array, it's not suggested to exceed
     * BCMI_LT_FIELD_MAX. Note that each element in an array will be counted.
     */
    int               nfields;

    /*! Entry attributes of the entry table(ref BCMLT_ENT_ATTR_F_xxx). */
    uint32_t          attr;
} bcmi_lt_entry_t;

/*!
 * \brief Callback function to handle an LT entry.
 *
 * This callback function is used to handle an LT entry.
 */
typedef int (*bcmi_lt_entry_handle_cb) (
    /*! Device Number. */
    int                  unit,
    /*! LT Name. */
    const char           *lt_name,
    /*! Handle to the entry. */
    bcmlt_entry_handle_t entry_hdl,
    /*! Cookie for LT entry information. */
    void                 *cookie
);

/*!
 * \brief LT field information.
 *
 * This data structure is used to provide the immutable propertires of a field.
 */
typedef struct bcmi_lt_field_info_s {
    /*! Enabled if this is a key field. */
    bool key;

    /*!
     * Enabled to indicate this field is specified via symbols,
     * not numeric values.
     */
    bool symbol;

    /*!
     * The least permitted value for this field in this LT,
     * as restricted by device resource and table configuration.
     * Applicable if SYMBOL is disabled.
     */
    uint64_t min_limit;

    /*!
     * The greatest permitted value for this field in this LT,
     * as restricted by device resource and table configuration.
     * Applicable if SYMBOL is disabled.
     */
    uint64_t max_limit;

    /*!
     * Number of array elements for this field.
     * If the field is scalar, this value is 0.
     */
    uint32_t array_depth;

    /*!
     * Number of elements for this field.
     * For a scalar, this will be (FIELD_WIDTH + 63 / 64).
     * For an array, this is equal to ARRAY_DEPTH.
     * The valid field index values for this field range
     * [ELEMENTS - 1, 0].
     */
    uint32_t elements;

    /*! Bit width of this field. */
    uint32_t field_width;

} bcmi_lt_field_info_t;

/*!
 * \brief Initialize LT entry struct.
 *
 * \param [in]  entry         LT entry information.
 * \param [in]  fields        Pointer to fields information array.
 *
 * \retval None.
 */
extern void
bcmi_lt_entry_init(bcmi_lt_entry_t *entry, bcmi_lt_field_t *fields);

/*!
 * \brief Set entry attributes.
 *
 * \param [in]  entry         LT entry information.
 * \param [in]  attr          Entry attributes of the entry table(ref
 * BCMLT_ENT_ATTR_F_xxx).
 *
 * \retval None.
 */
extern void
bcmi_lt_entry_attrib_set(bcmi_lt_entry_t *entry, uint32_t attr);

/*!
 * \brief Insert an entry into LT table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_insert(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb);

/*!
 * \brief Modify the specified fields of an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_update(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb);

/*!
 * \brief Delete an entry from LT table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_delete(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb);

/*!
 * \brief Lookup if an entry exists or not.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [in]  cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_lookup(int unit, const char *lt_name, void *cookie,
                     bcmi_lt_entry_handle_cb hdl_cb);

/*!
 * \brief Retrieve the specified fields of an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [out] cookie        LT entry information.
 * \param [in]  hdl_set_cb    User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 * \param [in]  hdl_get_cb    User callback to get LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_get(int unit, const char *lt_name, void *cookie,
                  bcmi_lt_entry_handle_cb hdl_set_cb,
                  bcmi_lt_entry_handle_cb hdl_get_cb);

/*!
 * \brief Insert or update an entry.
 *
 * If the LT entry exists, it will be updated directly. If the LT entry doesn't
 * exist, a new LT entry will be inserted.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [out] cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields. If it's not
 * specified(NULL), cookie should be organized per \c bcmi_lt_entry_t
 * definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_set(int unit, const char *lt_name,
                  void *cookie, bcmi_lt_entry_handle_cb hdl_cb);

/*!
 * \brief Traverse all the entries in an LT.
 *
 * This function goes through LT table entries and runs the user callback
 * function at each LT entry. Don't support the bcmi_lt_entry_t format so far.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lt_name       LT name.
 * \param [out] cookie        LT entry information.
 * \param [in]  hdl_cb        User callback to set LT fields/traverse entries.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_traverse(int unit, const char *lt_name,
                       void *cookie, bcmi_lt_entry_handle_cb hdl_cb);

/*!
 * \brief Add a field to an LT entry.
 *
 * This function is used to add a field to an entry. The field can be up to
 * 64-bits in size. Use field array for larger fields (see
 * \ref bcmi_lt_entry_field_array_add)
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  data          64-bits value of the field to set.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
extern void
bcmi_lt_entry_field_add(int unit, bcmi_lt_entry_t *entry,
                        const char *fld_name, uint64_t data, uint16_t flags);

/*!
 * \brief Get a field from an LT entry.
 *
 * This function is used to get a field value from an entry. The field
 * can be up to 64-bits in size. Use field array for larger fields (see
 * \ref bcmi_lt_entry_field_array_get)
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [out] data          64-bits value of the field to get.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_field_get(int unit, bcmi_lt_entry_t *entry,
                        const char *fld_name, uint64_t *data);

/*!
 * \brief Add a symbol field to an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  data          Symbol of the field to set.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
extern void
bcmi_lt_entry_field_symbol_add(int unit, bcmi_lt_entry_t *entry,
                               const char *fld_name, const char *data,
                               uint16_t flags);
/*!
 * \brief Get a symbol field to an entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [out] data          Symbol of the field to get.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_field_symbol_get(int unit, bcmi_lt_entry_t *entry,
                               const char *fld_name, const char **data);

/*!
 * \brief Add a field array to an entry.
 *
 * This function is used to add a field array to an entry. Each element of the
 * field array can be up to 64-bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to insert two
 * elements at index 3 & 4 the application will set \c start_idx = 3 and
 * num_of_elem = 2. Note that the array index starts from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [in]  array         Pointer to array of 64-bits values.
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
extern void
bcmi_lt_entry_field_array_add(int unit, bcmi_lt_entry_t *entry,
                              const char *fld_name, uint16_t start_idx,
                              uint64_t *array, uint16_t size, uint16_t flags);

/*!
 * \brief Get a field array from an entry.
 *
 * This function is used to get a field array from an entry. Each element of the
 * field array must be up to 64 bits in size. This function can be called
 * multiple times with different \c start_idx values to construct a complete
 * array of elements. For example, if the application wants to get the values of
 * two elements in index 3 & 4 the application will set \c start_idx = 3 and
 * num_of_elem = 2. Note that the array index starts from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [in]  array         Pointer to array of 64-bits values.
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  r_size        Actual number of array elements that are written
 * into the data array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_field_array_get(int unit, bcmi_lt_entry_t *entry,
                              const char *fld_name, uint16_t start_idx,
                              uint64_t *array, uint16_t size, uint16_t *r_size);

/*!
 * \brief Add a symbol array to an entry.
 *
 * This function is used to add a symbol array to an entry. This function can
 * be called multiple times with different start_idx values to construct a
 * complete array of elements. For example, if the application wants to insert
 * two elements at index 3 & 4 the application will set \c start_idx = 3 and
 * num_of_elem = 2. Note that the array index starts from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [in]  array         Pointer to array of symbols (strings).
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  flags         Field flags(\c BCMI_LT_FIELD_F_SET
 * or \c BCMI_LT_FIELD_F_GET).
 *
 * \retval None.
 */
extern void
bcmi_lt_entry_field_array_symbol_add(int unit, bcmi_lt_entry_t *entry,
                                     const char *fld_name, uint16_t start_idx,
                                     const char **array, uint16_t size,
                                     uint16_t flags);

/*!
 * \brief Get a symbol array from an entry.
 *
 * This function is used to get a symbol field array from an entry. This
 * function can be called multiple times with different \c start_idx values
 * to construct a complete array of elements. For example, if the application
 * wants to get the values of two elements in index 3 & 4 the application will
 * set \c start_idx = 3 and num_of_elem = 2. Note that the array index starts
 * from index 0.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry         LT entry information.
 * \param [in]  fld_name      LT field name.
 * \param [in]  start_idx     Target array index where elements should start
 * being applied.
 * \param [out] array         Pointer to array of symbols (strings).
 * \param [in]  size          Indicates the number of array elements.
 * \param [in]  r_size        Actual number of array elements that are written
 * into the data array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_field_array_symbol_get(int unit, bcmi_lt_entry_t *entry,
                                     const char *fld_name, uint16_t start_idx,
                                     const char **array, uint16_t size,
                                     uint16_t *r_size);

/******************************************************************************/
/*              Encapsulation for Partial SDKLT LT API Sequence               */
/******************************************************************************/

/*!
 * \brief LT Synchronous entry commit.
 *
 * SDKLT LT commit APIs have been successful if the operation had been executed
 * regardless of the actual result of the operation, and it is required to
 * validate the entry status after the function returns. This function provides
 * the encapsulation with the combination of above SDKLT operations.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry_hdl     Handle to the entry.
 * \param [in]  opcode        LT operations code.
 * \param [in]  priority      Priority.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_commit(int unit, bcmlt_entry_handle_t entry_hdl,
                     bcmlt_opcode_t opcode, bcmlt_priority_level_t priority);

/*!
 * \brief Commit an entry with LT set operation.
 *
 * If the LT entry exists, it will be updated directly. If the LT entry doesn't
 * exist, a new LT entry will be inserted.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  entry_hdl     Handle to the entry.
 * \param [in]  priority      Priority.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_entry_set_commit(int unit, bcmlt_entry_handle_t entry_hdl,
                         bcmlt_priority_level_t priority);

/*!
 * \brief Transaction synchronous commit.
 *
 * SDKLT LT commit APIs have been successful if the operation had been executed
 * regardless of the actual result of the operation, and it is required to
 * validate the entry status after the function returns. This function provides
 * the encapsulation with the combination of above SDKLT operations in
 * transaction.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  trans_hdl     Handle to the transaction.
 * \param [in]  priority      Priority.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_transaction_commit(int unit,bcmlt_transaction_hdl_t trans_hdl,
                           bcmlt_priority_level_t priority);

/*!
 * \brief Retrieve the definition of a specified LT field.
 *
 * This function retrieves the definition associated with the specified
 * LT field on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] fld_def       LT field definition.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_field_def_get(int unit, const char *tbl_name,
                      const char *fld_name, bcmlt_field_def_t *fld_def);

/*!
 * \brief Retrieve the width of a specified LT field.
 *
 * This function retrieves the width(number of bits) of the specified
 * LT field on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] width         LT field width.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_field_width_get(int unit, const char *tbl_name,
                        const char *fld_name, uint32_t *width);

/*!
 * \brief Retrieve the range of a specified LT field value.
 *
 * This function retrieves the allowed value range of a specified LT field
 * on a particular device.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] value_min     Minimum field value allowed.
 * \param [out] value_max     Maximum field value allowed.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_field_value_range_get(int unit, const char *tbl_name,
                              const char *fld_name, uint64_t *value_min,
                              uint64_t *value_max);

/*!
 * \brief Get immutable properties of a field.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 * \param [out] fld_info      LT field info.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_field_info_get(int unit, const char *tbl_name, const char *fld_name,
                       bcmi_lt_field_info_t *fld_info);

/*!
 * \brief Check the specified field is valid.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [in]  fld_name      LT field name.
 *
 * \retval SHR_E_NONE         The field is valid.
 * \retval !SHR_E_NONE        The field is invalid.
 */
extern int
bcmi_lt_field_validate(int unit, const char *tbl_name, const char *fld_name);

/*!
 * \brief Clear table content.
 *
 * This API clears the content of a table.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_clear(int unit, const char *tbl_name);

/*!
 * \brief Retrieves the capacity of a specified LT.
 *
 * This function retrieves the capacity of a specified LT.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  tbl_name      LT name.
 * \param [out] entry_num     Maximum number of entries in a table.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
extern int
bcmi_lt_capacity_get(int unit, const char *tbl_name, uint32_t *entry_num);

/*!
 * \brief Retrieves the current number of entries inserted in a specified LT.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_name LT name.
 * \param [out] cnt The number of entries inserted in a table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_lt_entry_inuse_cnt_get(int unit, const char *tbl_name, uint32_t *cnt);

#endif /* BCMI_LT_INTF_H */
