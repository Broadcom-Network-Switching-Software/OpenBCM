/*! \file bcmltd_handler.h
 *
 * Data types for transform and custom table handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTD_HANDLER_H
#define BCMLTD_HANDLER_H

#include <bcmltd/bcmltd_types.h>

/* BCMLTA types */

/*!
 * \brief Field descriptor.
 *
 * In the BCMLTD context, each instance of a bcmlt_field_list_t is treated
 * as an array element, and the 'next' element should be ignored, as the
 * array may be the underlying list with elements reordered.
 */

typedef bcmlt_field_list_t bcmltd_field_t;

/*!
 * \brief Field set descriptor.
 */

typedef struct bcmltd_fields_s {
    /*! Number of fields. */
    size_t                    count;

    /*! Array of pointer to fields. */
    bcmltd_field_t            **field;

} bcmltd_fields_t;

/*!
 * \brief Generic mapping function pointer argument.
 */

typedef struct bcmltd_generic_arg_s {
    /*! Source table. */
    bcmltd_sid_t sid;

    /*! Number of Generic function arguments. */
    uint32_t values;

    /*! Array of Generic function argument values. */
    const uint32_t *value;

    /*! Component user data. */
    void *user_data;
} bcmltd_generic_arg_t;

/*!
 * \brief Operation function arguments.
 */
typedef struct bcmltd_op_arg_s {
    /*! Transaction identifier. */
    uint32_t trans_id;

    /*! LT entry attributes. */
    uint32_t attrib;
} bcmltd_op_arg_t;

/*!
 * \brief Field descriptor
 */
typedef struct bcmltd_field_desc_s {
    /*! Field ID. */
    uint32_t field_id;

    /*! Field index. */
    uint32_t field_idx;

    /*! Lowest bit position. */
    uint16_t minbit;

    /*! Highest bit position. */
    uint16_t maxbit;

    /*! Entry index for extended physical field */
    uint16_t entry_idx;

    /*! Reserved */
    const char* sym;

    /*! Reserved */
    uint8_t reserved;
} bcmltd_field_desc_t;

/*!
 * \brief Field list for LTA Transforms.
 *
 * Known use case:
 * This is supplied to the field demux/mux (1:M & N:1) Transforms
 * as the bcmltd_generic_arg_t.user_data in the transform arguments list.
 */
typedef struct bcmltd_field_list_s {
    /*! Number of subfields in list. */
    uint32_t field_num;

    /*! List of pointers to bcmcfg_field_desc_t specifiers. */
    const bcmltd_field_desc_t *field_array;
} bcmltd_field_list_t;

/*!
 * \brief Generic mapping function pointer argument.
 */

typedef struct bcmltd_transform_arg_s {
    /*! Number of transform function arguments. */
    uint32_t values;

    /*! Array of transform function argument values. */
    const uint32_t *value;

    /*! Number of transform tables. */
    uint32_t tables;

    /*! Array of transform tables. */
    const uint32_t *table;

    /*! Number of transform fields. */
    uint32_t fields;

    /*! Array of transform fields. */
    const uint32_t *field;

    /*! Array of fields descriptor. */
    const bcmltd_field_list_t *field_list;

    /*! Number of transform src key fields. */
    uint32_t kfields;

    /*! Array of transform src key logical fields. */
    const uint32_t *kfield;

    /*! Array of transform src key logical fields descriptor. */
    const bcmltd_field_list_t *kfield_list;

    /*! Number of transform return fields. */
    uint32_t rfields;

    /*! Array of transform return fields. */
    const uint32_t *rfield;

    /*! Array of return fields descriptor. */
    const bcmltd_field_list_t *rfield_list;

    /*! Component user data. */
    const bcmltd_generic_arg_t *comp_data;
} bcmltd_transform_arg_t;

/*!
 * \brief Reverse subfield transform information magic number.
 */
#define BCMLTD_REV_SUBFIELD_TRANSFORM_INFO_MAGIC 0x20178159


/*!
 * \brief Reverse subfield transform information.
 */

typedef struct bcmltd_rev_subfield_transform_info_s {
    /*! Magic number for this structure. */
    uint32_t magic;

    /*! Number of coupled reverse subfield transforms. */
    uint32_t num;

    /*! Pointer to state count. */
    uint32_t * const count;

    /*! Length of value array. */
    uint32_t values;

    /*! Array of argument values being consolidated. */
    uint64_t * const value;
} bcmltd_rev_subfield_transform_info_t;

/*!
 * \brief Field validation arguments.
 */

typedef struct bcmltd_field_val_arg_s {
    /*! Number of validate arguments. */
    uint32_t values;

    /*! Array of validate argument values. */
    const uint32_t *value;

    /*! Number of validate fields. */
    uint32_t fields;

    /*! Array of validate fields. */
    const uint32_t *field;

    /*! Field validation data. */
    const bcmltd_generic_arg_t *comp_data;
} bcmltd_field_val_arg_t;

/*
 * The following bcmltd_table_ structures are defined and used
 * in the TABLE LTA functions in order to allow for
 * future expansions, such as adding more information being
 * handled by the function, and minimize the impact in
 * every usage of these interfaces.
 */

/*!
 * \brief Table entry limit function arguments.
 */
typedef struct bcmltd_table_entry_limit_arg_s {
    /*! Value for TABLE_INFO.ENTRY_MAXIMUM. */
    uint64_t entry_maximum;
} bcmltd_table_entry_limit_arg_t;

/*!
 * \brief Table entry limit.
 */
typedef struct bcmltd_table_entry_limit_s {
    /*! Value for TABLE_INFO.ENTRY_LIMIT. */
    uint64_t entry_limit;
} bcmltd_table_entry_limit_t;

/*!
 * \brief Table entry in-use count function arguments.
 */
typedef struct bcmltd_table_entry_inuse_arg_s {
    /*! Needed for current empty struct.  Do not use. */
    uint32_t reserved;
} bcmltd_table_entry_inuse_arg_t;

/*!
 * \brief Table entry in-use count.
 */
typedef struct bcmltd_table_entry_inuse_s {
    /*! Value for TABLE_INFO.ENTRY_INUSE_CNT. */
    uint32_t entry_inuse;
} bcmltd_table_entry_inuse_t;

/*!
 * \brief Table max entries function arguments.
 */
typedef struct  bcmltd_table_max_entries_arg_s {
    /*! Needed for current empty struct.  Do not use. */
    uint32_t reserved;
} bcmltd_table_max_entries_arg_t;

/*!
 * \brief Table max entries.
 */
typedef struct bcmltd_table_max_entries_s {
    /*! Value for TABLE_CONTROL.MAX_ENTRIES. */
    uint32_t max_entries;
} bcmltd_table_max_entries_t;

/*!
 * \brief Component validation operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are valid with respect to the given opcode and handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_val_op_f)(int unit,
                                       bcmlt_opcode_t opcode,
                                       const bcmltd_fields_t *in,
                                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component handler operation function pointer type.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  op_arg          Operation arguments.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation according to the
 * handler.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_op_f)(int unit,
                                   const bcmltd_op_arg_t *op_arg,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component input operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation according to the
 * handler (used for insert, delete, update).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_in_op_f)(int unit,
                                      uint32_t trans_id,
                                      const bcmltd_fields_t *in,
                                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component output operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation  according to the
 * handler (used for traverse).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_out_op_f)(int unit,
                                       uint32_t trans_id,
                                       bcmltd_fields_t *out,
                                       const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component input/output operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are accepted as input for the relevant operation  according to the
 * handler (used for lookup).
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_inout_op_f)(int unit,
                                         uint32_t trans_id,
                                         const bcmltd_fields_t *in,
                                         bcmltd_fields_t *out,
                                         const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component High-Availability operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_ha_op_f)(int unit,
                                      uint32_t trans_id,
                                      const bcmltd_generic_arg_t *arg);

/*!
 * \brief Component transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in              LTD input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to/from physical.
 *
 * Handler should return SHR_E_NONE if the fields in the field set are
 * accepted as input for the transform operation. Transformed (output
 * physical) fields should be set in 'out', along with an updated
 * destination table in 'out_sid' if any.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_transform_op_f)(int unit,
                                             const bcmltd_fields_t *in,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief Component extended transform operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  in_key          LTD key input logical field set.
 * \param [in]  in_value        LTD value input field set.
 * \param [out] out             LTD output field set.
 * \param [in]  arg             Handler arguments.
 *
 * Transform direction is logical to/from physical.
 *
 * Handler should return SHR_E_NONE if the fields in the field set are
 * accepted as input for the transform operation. Transformed (output
 * physical) fields should be set in 'out', along with an updated
 * destination table in 'out_sid' if any.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_ext_transform_op_f)(int unit,
                                             const bcmltd_fields_t *in_key,
                                             const bcmltd_fields_t *in_value,
                                             bcmltd_fields_t *out,
                                             const bcmltd_transform_arg_t *arg);

/*!
 * \brief Component field validation operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  opcode          Table opcode.
 * \param [in]  in              LTD input field set.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if the fields in the field set
 * are valid with respect to the given opcode and handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_field_val_op_f)(int unit,
                                             bcmlt_opcode_t opcode,
                                             const bcmltd_fields_t *in,
                                             const bcmltd_field_val_arg_t *arg);

/*!
 * \brief Table entry limit get function pointer type.
 *
 * This function returns the entry limit for the given table.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  trans_id        Transaction identifier.
 * \param [in]  sid             Table ID.
 * \param [in]  table_arg       Table arguments.
 * \param [out] table_data      Returning table entry limit.
 * \param [in]  arg             Handler arguments.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_table_entry_limit_get_f)(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_entry_limit_arg_t *table_arg,
                     bcmltd_table_entry_limit_t *table_data,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief Table entry in-use count get function pointer type.
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
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_table_entry_inuse_get_f)(int unit,
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
typedef int (*bcmltd_handler_table_max_entries_set_f)(int unit,
                     uint32_t trans_id,
                     bcmltd_sid_t sid,
                     const bcmltd_table_max_entries_arg_t *table_arg,
                     const bcmltd_table_max_entries_t *table_data,
                     const bcmltd_generic_arg_t *arg);

/*!
 * \brief Transform handler interface type
 */
typedef struct bcmltd_xfrm_handler_s {

    /*! Transform function. */
    const bcmltd_handler_transform_op_f transform;

    /*! Extended transform function. */
    const bcmltd_handler_ext_transform_op_f ext_transform;

    /*! Transform Handler arguments. */
    const bcmltd_transform_arg_t *arg;
} bcmltd_xfrm_handler_t;

/*!
 * \brief Field validation handler interface type
 */
typedef struct bcmltd_field_val_handler_s {
    /*! Field validation function. */
    const bcmltd_handler_field_val_op_f validate;
    /*! Field validation arguments. */
    const bcmltd_field_val_arg_t *arg;
} bcmltd_field_val_handler_t;

/*!
 * \brief Table commit handler interface type
 */
typedef struct bcmltd_table_commit_handler_s {
    /*! Prepare transaction function: initiate and record transaction ID. */
    const bcmltd_handler_ha_op_f prepare;

    /*! Commit function. */
    const bcmltd_handler_ha_op_f commit;

    /*! Abort function. */
    const bcmltd_handler_ha_op_f abort;

    /*! Handler arguments. */
    const bcmltd_generic_arg_t *arg;
} bcmltd_table_commit_handler_t;

/*!
 * \brief Table entry limit handler interface type.
 *
 * This handler provides the interface to retrieve the table
 * entry limit which may change outside of the standard
 * Logical Table Manager flow.
 *
 * One example of such tables are Logical Tables which are implemented
 * upon flexible banks.  Bank control table modification will change
 * the physical resources available to one or more other tables.
 * Such resizable tables must provide these functions.
 */
typedef struct bcmltd_table_entry_limit_handler_s {
    /*!
     * Table entry limit get function.
     *
     * This function returns the value for TABLE_INFO.ENTRY_LIMIT.
     */
    const bcmltd_handler_table_entry_limit_get_f entry_limit_get;

    /*! Handler arguments. */
    const bcmltd_generic_arg_t *arg;
} bcmltd_table_entry_limit_handler_t;

/*!
 * \brief Table entry usage handler interface type.
 *
 * This handler provides the interface to manage the current
 * logical entry usage state.  This handler is required for tables
 * which may have added or removed entries outside of
 * LTM operations.
 *
 * The handler contains interfaces to retrieve the current
 * entry in-use count and set the desired runtime maximum entries
 * permitted.
 *
 * If this interface is defined, the handler is responsible
 * for checking that the entry in-use count does not exceed
 * the table max entries setting.
 */
typedef struct bcmltd_table_entry_usage_handler_s {
    /*!
     * Table entry in-use count get function.
     *
     * This function returns the value for TABLE_INFO.ENTRY_INUSE_CNT.
     */
    const bcmltd_handler_table_entry_inuse_get_f entry_inuse_get;

    /*!
     * Table max entries set function.
     *
     * This function passes the value being set for
     * TABLE_CONTROL.MAX_ENTRIES. This value is used to check
     * that the entry in-use count does not exceed this setting.
     */
    const bcmltd_handler_table_max_entries_set_f max_entries_set;

    /*! Handler arguments. */
    const bcmltd_generic_arg_t *arg;
} bcmltd_table_entry_usage_handler_t;

/*!
 * \brief Optional custom table handler HA operations
 */
typedef struct bcmltd_table_ha_handler_s {
    /*! Commit function. */
    const bcmltd_handler_ha_op_f      commit;
    /*! Abort function. */
    const bcmltd_handler_ha_op_f      abort;
} bcmltd_table_ha_handler_t;

/*!
 * \brief Custom table handler traverse operations
 */
typedef struct bcmltd_table_traverse_handler_s {
    /*! First function. */
    const bcmltd_handler_out_op_f     first;
    /*! Next function. */
    const bcmltd_handler_inout_op_f   next;
} bcmltd_table_traverse_handler_t;

/*!
 * \brief Custom table handler interface type
 */
typedef struct bcmltd_table_handler_s {
    /*! Validation function. */
    const bcmltd_handler_val_op_f     validate;
    /*! Insert function. */
    const bcmltd_handler_op_f         op_insert;
    /*! Insert with Allocate function. */
    const bcmltd_handler_op_f         op_insert_alloc;
    /*! Lookup function. */
    const bcmltd_handler_op_f         op_lookup;
    /*! Delete function. */
    const bcmltd_handler_op_f         op_delete;
    /*! Update function. */
    const bcmltd_handler_op_f         op_update;
    /*! Traverse First function. */
    const bcmltd_handler_op_f         op_first;
    /*! Traverse Next function. */
    const bcmltd_handler_op_f         op_next;
    /*! Insert function. */
    const bcmltd_handler_in_op_f      insert;
    /*! Insert with Allocate function. */
    const bcmltd_handler_inout_op_f   insert_alloc;
    /*! Lookup function. */
    const bcmltd_handler_inout_op_f   lookup;
    /*! Delete function. */
    const bcmltd_handler_in_op_f      delete;
    /*! Update function. */
    const bcmltd_handler_in_op_f      update;
    /*! HA handlers. */
    const bcmltd_table_ha_handler_t   *ha;
    /*! Traverse handlers. */
    const bcmltd_table_traverse_handler_t   *traverse;
    /*! Table entry limit handlers. */
    const bcmltd_table_entry_limit_handler_t *entry_limit;
    /*! Table entry usage handlers. */
    const bcmltd_table_entry_usage_handler_t *entry_usage;
    /*! Handler arguments. */
    const bcmltd_generic_arg_t  *arg;
} bcmltd_table_handler_t;

/*!
 * \brief Custom Table Handler initialization function argument.
 */

typedef struct bcmltd_handler_lifecycle_arg_s {
    /*! Number of generic_args. */
    uint32_t generic_args;

    /*! Component table handler pointers. */
    const bcmltd_generic_arg_t **generic_arg;
} bcmltd_handler_lifecycle_arg_t;

/*!
 * \brief Component life cycle operation function pointer type
 *
 * \param [in]  unit            Unit number.
 * \param [in]  arg             Handler arguments.
 *
 * Handler should return SHR_E_NONE if operation was performed.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
typedef int (*bcmltd_handler_lifecycle_op_f)(int unit,
                                      const bcmltd_handler_lifecycle_arg_t *arg);

/*!
 * \brief Optional custom table lifecycle handler
 */
typedef struct bcmltd_table_lifecycle_handler_s {
    /*! Initialization function. */
    const bcmltd_handler_lifecycle_op_f    init;
    /*! Cleanup function. */
    const bcmltd_handler_lifecycle_op_f    cleanup;
    /*! Handler arguments. */
    const bcmltd_handler_lifecycle_arg_t   *arg;
} bcmltd_table_lifecycle_handler_t;

/*!
 * \brief Transform interface array
 */
typedef struct bcmltd_xfrm_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_xfrm_handler_t            **handler;
} bcmltd_xfrm_intf_t;

/*!
 * \brief Field validation interface array
 */
typedef struct bcmltd_field_val_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_field_val_handler_t       **handler;
} bcmltd_field_val_intf_t;

/*!
 * \brief Table commit interface array
 */
typedef struct bcmltd_table_commit_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_table_commit_handler_t    **handler;
} bcmltd_table_commit_intf_t;

/*!
 * \brief Table Entry limit interface array
 */
typedef struct bcmltd_table_entry_limit_intf_s {
    /*! Number of handlers. */
    size_t                                   count;
    /*! Handler functions. */
    const bcmltd_table_entry_limit_handler_t **handler;
} bcmltd_table_entry_limit_intf_t;

/*!
 * \brief Custom table handler interface array
 */
typedef struct bcmltd_table_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_table_handler_t           **handler;
} bcmltd_table_intf_t;

/*!
 * \brief Custom table lifecycle interface array
 */
typedef struct bcmltd_table_lifecycle_intf_s {
    /*! Number of handlers. */
    size_t                                 count;
    /*! Handler functions. */
    const bcmltd_table_lifecycle_handler_t **handler;
} bcmltd_table_lifecycle_intf_t;

/*!
 * \brief LTM LTA interfaces
 */
typedef struct bcmltd_ltm_intf_s {
    /*! Transform interfaces. */
    const bcmltd_xfrm_intf_t                 *xfrm;
    /*! Field validation interfaces. */
    const bcmltd_field_val_intf_t            *val;
    /*! Table commit interfaces. */
    const bcmltd_table_commit_intf_t         *table_commit;
    /*! Table entry limit interfaces. */
    const bcmltd_table_entry_limit_intf_t    *table_entry_limit;
    /*! Custom table handler interfaces. */
    const bcmltd_table_intf_t                *table;
    /*! Custom table lifecycle interfaces. */
    const bcmltd_table_lifecycle_intf_t      *table_lc;
} bcmltd_ltm_intf_t;

/*!
 * \brief Convert field to uint32
 *
 * Convert a LTD field descriptor to uint32
 *
 * \param [in]  field           Pointer to field descriptor.
 * \param [out] value           Pointer to uint32_t value.
 *
 *
 * A scalar value in the field descriptor is converted to a uint32_t
 * value. An error is returned if the value is not a scalar or is not
 * directly convertible to a uint32_t value.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_field_to_uint32(const bcmltd_field_t *field, uint32_t *value);

/*!
 * \brief Convert uint32 to field
 *
 * Convert a uint32 to a BCMLTD field descriptor
 *
 * \param [out]  field           Pointer to field descriptor.
 * \param [in]   value           Pointer to uint32_t value.
 *
 *
 * A uint32_t value is converted to a BCMLTD field descriptor
 * scalar value. An error is returned if the value is not convertible
 * for some reason.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_uint32_to_field(bcmltd_field_t *field, const  uint32_t *value);

/*!
 * \brief Copy a field value
 *
 * Copy a BCMLTD field value
 *
 * \param [in]   in              Input field.
 * \param [out]  out             Output field.
 *
 *
 * Copy an input field value to an output field.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_field_copy(const bcmltd_field_t *in, bcmltd_field_t *out);

/*!
 * \brief Check for number of fields
 *
 * Return success if there are at least 'num' many fields provided
 * in field array.
 *
 * \param [in]   fields          Field array.
 * \param [in]   num             Number of fields required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_fields_required(const bcmltd_fields_t *fields, uint32_t num);

/*!
 * \brief Check for number of generic arguments
 *
 * Return success if there are at least 'count' many generic arguments
 * provided in the generic argument structure.
 *
 * \param [in]   arg             Generic argument structure.
 * \param [in]   num             Number of arguments required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_generic_args_required(const bcmltd_generic_arg_t *arg, uint32_t num);

/*!
 * \brief Check for number of transform arguments
 *
 * Return success if there are at least 'count' many transform arguments
 * provided in the transform argument structure.
 *
 * \param [in]   arg             Transform argument structure.
 * \param [in]   num             Number of arguments required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_transform_args_required(const bcmltd_transform_arg_t *arg, uint32_t num);

/*!
 * \brief Check for number of transform table arguments
 *
 * Return success if there are at least 'count' many transform table
 * arguments provided in the transform argument structure.
 *
 * \param [in]   arg             Transform argument structure.
 * \param [in]   num             Number of tables required.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
extern int
bcmltd_transform_tables_required(const bcmltd_transform_arg_t *arg,
                                 uint32_t num);


#endif /* BCMLTD_HANDLER_H */
