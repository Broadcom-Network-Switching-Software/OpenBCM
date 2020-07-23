/*! \file bcmltm_md_internal.h
 *
 * Logical Table Manager Metadata Internal Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_MD_INTERNAL_H
#define BCMLTM_MD_INTERNAL_H

#include <shr/shr_types.h>
#include <shr/shr_bitop.h>

#include <bcmbd/bcmbd.h>

#include <bcmdrd/bcmdrd_symbol_types.h>

#include <bcmltd/bcmltd_handler.h>

#include <bcmltm/bcmltm_types.h>
#include <bcmltm/bcmltm_types_internal.h>
#include <bcmltm/bcmltm_tree.h>
#include <bcmltm/bcmltm_state_internal.h>


/*!
 * \brief Generic node data - Forward declaration
 */
typedef struct bcmltm_op_node_s bcmltm_op_node_s;

/*!
 * \brief LTM metadata for one operation on a single LT.
 */
typedef struct bcmltm_lt_op_md_s {
    /*! Number of binary tree root nodes per type (FA or EE). */
    uint32_t num_roots;

    /*! Array of Field Adaptation binary tree root pointers. */
    bcmltm_node_t **fa_node_roots;

    /*! Array of Execution Engine binary tree root pointers. */
    bcmltm_node_t **ee_node_roots;

    /*! Number of node functions for this operation. */
    bcmltm_tree_size_t num_nodes;

    /*! Array of node structure pointers to perform this operation. */
    bcmltm_op_node_s **nodes_array;

    /*! Size of the Working Buffer (in bytes) for this operation on this LT. */
    uint32_t working_buffer_size;
} bcmltm_lt_op_md_t;

/*!
 *
 * BCMLTM Logical Table Flags.
 *
 * These flags indicate fundamental properties of the LT.  They will not
 * change during run time, but are fixed by the configuration selections
 * provided to the LRD during initialization.  The LTM treats these
 * properties as fixed for a given LT.
 */

/*! Modeled logical table, eligible for an Atomic Transaction. */
#define BCMLTM_LT_FLAGS_MODELED        0x01

/*! Overlay logical table, requires global in-use bitmap. */
#define BCMLTM_LT_FLAGS_OVERLAY        0x02

/*! This LTM interface is used for PT Pass Thru. */
#define BCMLTM_LT_FLAGS_PASSTHRU       0x04

/*! Logical index table with no key fields. */
#define BCMLTM_LT_FLAGS_NO_KEY         0x08

/*!
 * \brief LTM parameters for a single LT.
 */
typedef struct bcmltm_lt_params_s {
    /*! Logical Table ID. */
    uint32_t ltid;

    /*! Logical Table flags. */
    uint32_t lt_flags;

    /*!
     * For Index LTs, the number of API Key field elements (fid, idx).
     * When set to 0, Tracking Index and Key field boundaries are
     * not used by this LT.
     */
    uint32_t index_key_num;

    
    /*!
     * Absolute minimum table index for this device and configuration.
     * May be overridden to a greater value by custom application
     * configuration in LT state.
     */
    uint32_t index_min;

    /*!
     * Absolute maximum table index for this device and configuration.
     * May be overridden to a lesser value by custom application
     * configuration in LT state.
     */
    uint32_t index_max;

    /*!
     * Absolute minimum table memory parameter for this device
     * and configuration.
     * May be overridden to a greater value by custom application
     * configuration in LT state.
     */
    uint32_t mem_param_min;

    /*!
     * Absolute maximum table memory parameter for this device
     * and configuration.
     * May be overridden to a greater value by custom application
     * configuration in LT state.
     */
    uint32_t mem_param_max;
    

    /*!
     * For overlay tables that share device resource entries,
     * one table maintains the global in-use bitmap.  The LTID
     * of that table is recorded here.
     */
    uint32_t global_inuse_ltid;

    /*! HA Pointer to the structure tracking runtime LT values. */
    bcmltm_ha_ptr_t lt_state_hap;

    /*! HA Pointer to the structure tracking rollback LT values.
     *  Set to BCMLTM_HA_PTR_INVALID when not employed during
     *  an Atomic Transaction.
     */
    bcmltm_ha_ptr_t lt_rollback_state_hap;

    /* ... */
} bcmltm_lt_params_t;

/*!
 * \brief Table Commit list data structure.
 *
 * This structure contains the list of Table Commit handlers.
 *
 * A non-CTH LT may have internal software state that needs
 * to be committed or aborted.  These handlers permit
 * LTs to perform corresponding actions during a commit or abort
 * request.
 *
 * This is applicable to non-CTH LTs.
 *
 * NOTE: Commit/abort for CTH are handled by the corresponding
 * CTH interface handler.
 */
typedef struct bcmltm_table_commit_list_s {
    /*! Number of Table Commit handlers. */
    uint32_t num_handlers;

    /*! Array of Table Commit handlers. */
    const bcmltd_table_commit_handler_t **handlers;
} bcmltm_table_commit_list_t;

/*!
 * \brief LTM information to track for a single LT.
 *
 * This structure contains pointers to both the parameters (including
 * runtime state) describing a Logical Table and the metadata
 * which determine the implementation of each operation supported on
 * the LT with the L2P mapping of the API fields to/from the internal
 * PTM representation.
 */
typedef struct bcmltm_lt_md_s {
    /*! Pointer to the per-LT parameters and runtime settings. */
    bcmltm_lt_params_t *params;

    /*! Array of pointers to the per-opcode metadata describing this LT. */
    bcmltm_lt_op_md_t *op[BCMLT_PT_OPCODE_NUM];

    /*! Per-LT statistics.   */
    uint32_t lt_stats[BCMLRD_FIELD_STATS_NUM];

    /*!
     * List of Table Commit handlers.
     *
     * This is applicable to non-CTH LTs.
     */
    const bcmltm_table_commit_list_t *tc_list;

    /*!
     * LTA Table Entry Limit Handler structure.
     *
     * If NULL, use LTM internal implementation.
     *
     * If non-NULL, invoke LTA handler function to get the
     * table entry limit information.
     */
    const bcmltd_table_entry_limit_handler_t *table_entry_limit;

    /*!
     * LTA Table Entry Usage Handler structure.
     *
     * If NULL, use LTM internal implementation.
     *
     * If non-NULL, invoke LTA handler functions to get the
     * table entry inuse count information and set the table control
     * max entries allowed.  In addition, the handler is responsible
     * for checking that the entry inuse count does not exceed
     * the table max entries setting.
     */
    const bcmltd_table_entry_usage_handler_t *table_entry_usage;

    /*!
     * LTA Custom Table Handler structure.
     * If NULL, not a LTA CTH Logical Table implementation.
     * If non-NULL, invoke LTA CTH functions, such as abort/commit,
     * instead of the LTM internal logic.
     */
    const bcmltd_table_handler_t *cth;

} bcmltm_lt_md_t;

/*!
 * \brief Extract statistics array from LT metadata.
 *
 * \param [in] \_md\_ Metadata for Logical Table.
 */
#define BCMLTM_STATS_ARRAY(_md_) \
    ((_md_)->lt_stats)

/*!
 * \brief LTM metadata for a unit.
 *
 * This should eventually be optimized for rapid retrieval of a given
 * LT's metadata.
 */
typedef struct bcmltm_md_s {
    /*! Current number of created LTs. */
    uint32_t lt_num;

    /*! Array size of lt_md.  */
    uint32_t lt_max;

    /*! Maximum Working Buffer size for any modeled LT sid */
    uint32_t wb_max_modeled;

    /*! Maximum Working Buffer size for any interactive LT sid */
    uint32_t wb_max_interactive;

    /*! Open-ended array of per-LT metadata (C99 convention). */
    bcmltm_lt_md_t *lt_md[];
} bcmltm_md_t;

/*!
 * \brief This is the traverse cookie for LTM stage operations.
 */
typedef struct bcmltm_tree_traverse_info_s {
    /*! Per-LT metadata. */
    bcmltm_lt_md_t *lt_md;

    /*! Per-LT runtime state. */
    bcmltm_lt_state_t *lt_state;

    /*! Pointer to Working Buffer space for this LT operation. */
    uint32_t *ltm_buffer;

    /*! The Logical Table entry structure passed on from the TRM. */
    bcmltm_entry_t *lt_entry;
} bcmltm_tree_traverse_info_t;


/*
 * Sample node cookie for API data to PT entry buffer.
 *
 * This node cookie is designed to store the information required to traverse
 * the set of API fields within this LT and convert the field data
 * representation into a HW field within a PT format entry within the
 * LTM working buffer.
 */


/*!
 * \brief Field bit position invalid value
 */
#define BCMLTM_FIELD_BIT_POSITION_INVALID 0xffff

/*!
 * \brief The location of a value within the Working Buffer.
 */
typedef struct bcmltm_wb_coordinate_s {
    /*!
     * Offset into the working buffer from which to measure
     * the bit positions.
     */
    uint32_t buffer_offset;

    /*! The maximum bit position of this field. */
    uint16_t maxbit;

    /*! The minimum bit position of this field. */
    uint16_t minbit;
} bcmltm_wb_coordinate_t;


/*!
 * \brief Field index invalid value
 */
#define BCMLTM_FIELD_INDEX_UNDEFINED 0xffffffff

/*!
 * \brief Field specifier (field, index).
 */
typedef struct bcmltm_field_spec_s {
    /*! API-facing field identifier. */
    uint32_t field_id;

    /*!
     * The index of the field in an array.
     */
    uint32_t field_idx;
} bcmltm_field_spec_t;

/*!
 * Working Buffer flags
 *
 * These values appear in specified WB locations to pass information
 * between function tree nodes.
 */

/*!
 * \brief PT entry changed flag
 */
#define BCMLTM_PT_ENTRY_CHANGED_VALUE 0x1

/*!
 * \brief Index Absent flag
 */
#define BCMLTM_INDEX_ABSENT           0x2

/*!
 * Field property flags
 *
 * Boolean flags within the field map data structures to indicate
 * various field properties.
 */

/*!
 * \brief Field flag - Key field
 */
#define BCMLTM_FIELD_KEY           0x1

/*!
 * \brief Field flag - Read only field
 */
#define BCMLTM_FIELD_READ_ONLY     0x2

/*!
 * Data structure to map a single field value specified by (id, idx)
 * to a coordinate within the Working Buffer (WBC) plus an API cache
 * field position.
 * For API fields, this position is the updatable copy of the field
 * values, used for UPDATE, INSERT with allocation, and TRAVERSE.
 */
typedef struct bcmltm_field_map_s {
    /*! Field identifier. */
    uint32_t field_id;

    /*!
     * The index of the field in an array.
     */
    uint32_t field_idx;

    /*!
     * Working Buffer offset to the bcmlt_field_list_t location
     * supplied to hold this field when a cached API is required.
     * Offset is invalid if this is a PT-side field.
     */
    uint32_t apic_offset;

    /*!
     * Working Buffer coordinate, the location within the WB to
     * place or retrieve this field value within a PTM entry.
     */
    bcmltm_wb_coordinate_t wbc;

    

    /*!
     * Working Buffer offset to the PT entry memory parameter
     * location indicating that the PT entry is changed.
     * If 0, then not used.
     */
    uint32_t pt_changed_offset;

    /*!
     * The default value for this field.
     * Logical Tables use the SW default value, which is determined
     * by table configuration during initialization.
     */
    uint64_t default_value;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;

    /*! Boolean field properties. */
    uint32_t flags;

    /*!
     * An extension to the field map, for
     * 1) N to M mappings
     * 2) Nonsymmetric read mappings
     */
    struct bcmltm_field_map_s *next_map;
} bcmltm_field_map_t;

/*!
 * \brief Field mapping data structure.
 */
typedef struct bcmltm_field_map_list_s {
    /*!
     * Number of API fields for this logical table corresponding
     * to this physical memory view.
     */
    uint32_t num_fields;

    /*! Individual field conversion data structures. */
    bcmltm_field_map_t *field_maps;

    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical index
     * is provided via API key fields.
     *
     * When this flag is set, the index value must be allocated
     * and returned to the application during INSERT operations.
     *
     * Only Index w/Allocation LTs support this use case.
     */
    uint32_t index_absent_offset;
} bcmltm_field_map_list_t;

/*!
 * General field mapping structure.
 *
 * Lists of mapping elements to connect the fields of a LT among the
 * API (fid, fidx), LTA-capable cache in WB, and the PT entry Working
 * Buffer coordinate.
 * This data structure is the node cookie for any FA nodes which
 * copy fields to/from the Working Buffer.
 */
typedef struct bcmltm_field_select_mapping_s {
    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical operation
     * may be optionally suppressed by LTA Transform.
     *
     * When this flag is set, the status of the PT suppress
     * location must be checked to determine
     * if a LTA transform has requested the PT operation
     * be omitted during this LT op.  If so, any API values returned
     * must be sourced from the field default.
     */
    
    uint32_t pt_suppress_offset;

    /*!
     * The offset into the working buffer of this field select
     * group's map index.
     * If offset is invalid, sole field list is unconditional.
     * Invalid if map index is 0.
     */
    uint32_t map_index_offset;

    /*! Number of select maps. Must be one if unconditional. */
    uint32_t num_maps;

    /*!
     * Lists of API field mappings to PTM entry format.
     * Since map index == 0 is invalid, the index of this array
     * is [map index - 1], or a single unconditional list at index 0
     * if map_index_offset is invalid.
     */
    bcmltm_field_map_list_t *field_map_list[];
} bcmltm_field_select_mapping_t;

/*
 * Fixed field values
 */

/*!
 * \brief Data structure for a physical field unmapped by any API field.
 */
typedef struct bcmltm_fixed_field_s {
    /*! Working Buffer coordinate, the location within the WB to
     *  place this field value */
    bcmltm_wb_coordinate_t wbc;

    /*!
     * Working Buffer offset to the PT entry memory parameter
     * location indicating that the PT entry is changed.
     * If 0, then not used.
     */
    uint32_t pt_changed_offset;

    /*!
     * Numeric fixed value to place in this working buffer position
     * during non-DELETE operations.
     */
    uint32_t field_value;

    /*!
     * Numeric fixed value to place in this working buffer position
     * during DELETE operations.
     */
    uint32_t delete_value;
} bcmltm_fixed_field_t;

/*!
 * \brief Fixed field value data structure.
 *
 * This structure is the node cookie for an FA node that copies
 * specific values into specified locations within the Working Buffer.
 * The purpose of these values is to supply fixed numbers required
 * for various physical entries, such as key/entry type or encoding
 * format.  It can be used to provide any fixed value for the WB
 * that is determined at metadata initialization during table
 * creation.
 */
typedef struct bcmltm_fixed_field_list_s {
    /*!
     * Number of fixed field values necessary to fill out the
     * working buffer for this logical table .
     */
    uint32_t num_fixed_fields;

    /*! Individual fixed field value data structures. */
    bcmltm_fixed_field_t *fixed_fields;
} bcmltm_fixed_field_list_t;

/*!
 * \brief Fixed field mapping data structure.
 *
 * This structure is the node cookie for an FA node that copies
 * specific values into specified locations within the Working Buffer
 * based on a selector field.
 */
typedef struct bcmltm_fixed_field_mapping_s {
    /*!
     * The offset into the working buffer of this field select
     * group's map index.
     * If offset is invalid, sole field list is unconditional.
     * Invalid if map index is 0.
     */
    uint32_t map_index_offset;

    /*! Number of select maps. Must be one if unconditional. */
    uint32_t num_maps;

    /*!
     * Lists of fixed fields to set in PTM entry format.
     * Since map index == 0 is invalid, the index of this array
     * is [map index - 1], or a single unconditional list at index 0
     * if map_index_offset is invalid.
     */
    bcmltm_fixed_field_list_t *fixed_field_list[];
} bcmltm_fixed_field_mapping_t;

/*!
 * \brief Read-only field list structure.
 *
 * This structure is the node cookie for an FA node that scans
 * the API input field list for any read-only values.
 */
typedef struct bcmltm_ro_field_list_s {
    /*!
     * Number of field specifiers in this list.
     */
    uint32_t num_field_specs;

    /*! Individual field specifier structures. */
    bcmltm_field_spec_t field_specs[];
} bcmltm_ro_field_list_t;

/*!
 * \brief Wide field descriptor (field, num_index).
 */
typedef struct bcmltm_wide_field_s {
    /*! API-facing field identifier. */
    uint32_t field_id;

    /*!
     * The number of indexes for the wide field array.
     */
    uint32_t num_field_idx;
} bcmltm_wide_field_t;

/*!
 * \brief Wide field structure.
 *
 * This structure is the node cookie for an FA node that scans
 * the API input field list for any wide field values to check
 * for field element coherence.
 */
typedef struct bcmltm_wide_field_list_s {
    /*!
     * Number of wide fields in this list.
     */
    uint32_t num_wide_fields;

    /*! Individual wide field structures. */
    bcmltm_wide_field_t wide_field[];
} bcmltm_wide_field_list_t;

/*!
 * \brief Data structure describing two locations in the Working Buffer,
 *        which may be used to copy a value of up to 32 bits.
 *        Both locations must be of the same size.
 */
typedef struct bcmltm_wb_copy_s {
    /*!
     * Working Buffer coordinate, the location within the WB from
     * which to retrieve this value.
     */
    bcmltm_wb_coordinate_t wbc_source;

    /*!
     * Working Buffer coordinate, the location within the WB to
     * place this value.
     */
    bcmltm_wb_coordinate_t wbc_destination;
} bcmltm_wb_copy_t;

/*!
 * \brief Working Buffer copy values data structure.
 *
 * This structure is the node cookie for an FA node that copies
 * values between specified locations within the Working Buffer.
 * The purpose of this list is to duplicate some Working Buffer
 * values that may not correspond directly to either API field values
 * or PT field values.
 */
typedef struct bcmltm_wb_copy_list_s {
    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical index
     * is provided via API key fields.
     *
     * When this flag is set, the index value must be allocated
     * and returned to the application during INSERT operations.
     *
     * Only Index w/Allocation LTs support this use case.
     */
    uint32_t index_absent_offset;

    /*!
     * Number of WB copy descriptions necessary to prepare the
     * working buffer for this logical table operation.
     */
    uint32_t num_wb_copy;

    /*! WB copy value data structures. */
    bcmltm_wb_copy_t *wb_copy;
} bcmltm_wb_copy_list_t;

/*
 * Overlay field selection
 */

/*! Select group ID of 0 is reserved to match WB init. */
#define BCMLTM_SELECT_GROUP_INVALID        0

/*!
 * \brief Field select map.
 *
 * For a single selector field value, specify the map index
 * to indicate the field list for other field operations.
 */
typedef struct bcmltm_select_map_s {
    /*! Selector field value corresponding to this map index. */
    uint64_t selector_field_value;

    /*!
     * Selector map index.
     * Invalid if map index is 0.
     */
    uint32_t map_index;
} bcmltm_select_map_t;

/*!
 * \brief  Overlay field selection structure.
 *
 * This structure is required for LTs with field select groups
 * to conditionally map field sets based upon other selector fields.
 *
 * The input to assembling this structure is the LRD select groups
 * with the information re-diagonalized into maps of the format:
 *
 * map Y {
 *     select SEL {
 *         1: A;
 *         2: B;
 *         3: A,B;
 *         4: C;
 *         5: A,C;
 *         6: B,C;
 *         7: A,B,C;
 *     }
 * }
 *
 * Once the map is determined by comparing the selector field value,
 * the map index is stored at the indicated Working Buffer
 * coordinate.  The index is used in subsequent field value operations.
 */
typedef struct bcmltm_field_select_maps_s {
    /*!
     * Working Buffer offset to the bcmlt_field_list_t location
     * supplied to hold this select API field.
     */
    uint32_t select_field_offset;

    /*!
     * Working Buffer offset to PT subentry memory parameter.
     * Exactly one of select_field_offset and pt_subentry_offset
     * is valid.
     */
    uint32_t pt_subentry_offset;

    /*!
     * The offset into the working buffer of the local field select
     * map index for later field value operations.
     * Invalid if map index is 0.
     */
    uint32_t map_index_offset;

    /*!
     * The offset into the working buffer of the parent selector
     * map index for this selector.  Permits hierarchy of selectors.
     * Invalid offset indicates top-level selector.
     */
    uint32_t parent_select_offset;

    /*!
     * The required value of the parent selector map index to
     * enable this selector.
     * Invalid if map index is 0.
     */
    uint32_t parent_select_map_index;

    /*! Number of select maps. */
    uint32_t num_maps;

    /*!
     * Individual select maps connecting the selector value
     * to the select map index.
     */
    bcmltm_select_map_t select_maps[];
} bcmltm_field_select_maps_t;

/*
 * LTA field values
 */

/*!
 * LTA field structure methodology
 *
 * The Working Buffer allocates sufficient space for each LTA field list
 * to hold the complete set of field values which it understands.  These
 * lists may be for input or output, and for custom table handler,
 * field transformation, or field validation (input only).  A given
 * LT may use multiple field sets for LTA.
 *
 * Each list is a fixed map of the API LT field values into the reserved
 * WB space for that list, each of which is a bcmltd_field_t structure:
 *
 * | spot 0 | spot 1 | ... | spot (N - 1) |
 *
 * A given API field ID maps to a particular spot, which is specified by
 * a a Working Buffer Coordinate.  (The FA node will fill out the other
 * elements of the bcmltd_field_t.) Not all of the possible N fields
 * in the LTA metadata field list may be supplied by the API.  The
 * FA node will record the fields that are actually present in the
 * lta_fields_ptrs array, which must be sized for enough pointers for the
 * maximum number of LTA fields for this function.
 *
 * | &(spot 0) | &(spot 5) | ... | &(spot (N - 2)) |
 */

/*!
 * \brief Data structure to map a single LTA field value to a coordinate
 * within the Working Buffer.  This position is recorded in the LTA
 * fields parameter structure.
 */
typedef struct bcmltm_lta_field_map_s {
    /*! API-facing field identifier. */
    uint32_t api_field_id;

    /*!
     * The index of the field in an array.
     */
    uint32_t field_idx;

    /*! Working Buffer coordinate, the location within the WB to
     *  place this field value in a PTM entry or from which to
     * retrieve it */
    bcmltm_wb_coordinate_t wbc;

    /*!
     * Working Buffer offset to the PT entry memory parameter
     * location indicating that the PT entry is changed.
     * If 0, then not used.
     */
    uint32_t pt_changed_offset;

    /*!
     * Working Buffer offset to the bcmlt_field_list_t location
     * supplied to hold this field when a cached API is required.
     * Offset is invalid if this is a PT-side field.
     * Forward transforms use this for the input values.
     * All transforms use this for the key field input list.
     */
    uint32_t apic_offset;

    /*!
     * Working Buffer offset to the bcmlt_field_list_t location
     * supplied to hold this field for the LTA interface.
     * The output of a reverse transform requires both this
     * location for the output, and the API cache offset above
     * to which the field structure will be copied.
     */
    uint32_t field_offset;

    /*!
     * The default value for this field.
     * Logical Tables use the SW default value, which is determined
     * by table configuration during initialization.
     */
    uint64_t default_value;

    /*! The minimum value for this field. */
    uint64_t minimum_value;

    /*! The maximum value for this field. */
    uint64_t maximum_value;

    /*! Boolean field properties. */
    uint32_t flags;
} bcmltm_lta_field_map_t;

/*!
 * \brief LTA field value data structure.
 *
 * This structure is the node cookie for an FA node that copies
 * specific values into specified locations within the Working Buffer.
 * The purpose of these values is to adapt API values into LTA field
 * lists for use as one of the LTA interfaces:
 * - custom table handler
 * - field transformation
 * - field validation
 *
 * The same format is used to pass field values to each of these
 * interfaces.  The information can be used to describe either
 * input or output LTA field lists.
 */
typedef struct bcmltm_lta_field_list_s {
    /*!
     * LTA field list structure WB offset (bcmltd_fields_t)
     */
    uint32_t fields_parameter_offset;

    /*!
     * LTA field list structure WB offset (bcmltd_fields_t) for API LTA input.
     * Offset INVALID if not applicable.
     */
    uint32_t api_fields_parameter_offset;

    /*!
     * Destination LTA field list structure WB offset (bcmltd_fields_t)
     * when fields from one LTA list are referenced by a second LTA list.
     */
    uint32_t target_fields_parameter_offset;

    /*!
     * WB offset to space for an array of LTA field structure pointers,
     * to be inserted into parameter structure above.
     * (bcmltd_field_t pointer array)
     */
    uint32_t fields_ptrs_offset;

    /*!
     * Maximum number of LTA field values this interface can accept or return.
     */
    uint32_t max_fields;

    /*!
     * Number of field maps in the field_maps list.
     */
    uint32_t num_maps;

    /*! Array of API-facing field identifiers. */
    bcmltm_lta_field_map_t *field_maps;

    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical index
     * is provided via API key fields.
     *
     * When this flag is set, the index value must be allocated
     * and returned to the application during INSERT operations.
     *
     * Only Index w/Allocation LTs support this use case.
     */
    uint32_t index_absent_offset;

    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical operation
     * may be optionally suppressed by LTA Transform.
     *
     * When this flag is set, the status of the PT suppress
     * location must be checked to determine
     * if a LTA transform has requested the PT operation
     * be omitted during this LT op.  If so, any values fed into
     * the LTA reverse transform must be sourced from the
     * PT field default.
     */
    uint32_t pt_suppress_offset;
} bcmltm_lta_field_list_t;

/*!
 * \brief Data structure to map a single LTA field value to a coordinate
 * within the Working Buffer and/or a location within the API cache.
 * This structure is used for Logical Tables with field selection.
 */
typedef struct bcmltm_lta_select_field_map_s {
    /*! Field identifier. */
    uint32_t field_id;

    /*!
     * The index of the field in an array.
     */
    uint32_t field_idx;

    /*! Working Buffer coordinate, the location within the WB to
     *  place this field value in a PTM entry or from which to
     * retrieve it */
    bcmltm_wb_coordinate_t wbc;

    /*!
     * Working Buffer offset to the bcmlt_field_list_t location
     * supplied to hold this field when a cached API is required.
     * Offset is invalid if this is a PT-side field.
     * Forward transforms use this for the input values.
     * All transforms use this for the key field input list.
     */
    uint32_t apic_offset;

    /*!
     * Working Buffer offset to the bcmlt_field_list_t location
     * supplied to hold this field for the LTA interface.
     * The output of a reverse transform requires both this
     * location for the output, and the API cache offset above
     * to which the field structure will be copied.
     */
    uint32_t field_offset;

    /*!
     * Working Buffer offset to the PT entry memory parameter
     * location indicating that the PT entry is changed.
     * If 0, then not used.
     */
    uint32_t pt_changed_offset;

    /*! Boolean field properties. */
    uint32_t flags;
} bcmltm_lta_select_field_map_t;

/*!
 * \brief LTA field value data structure.
 *
 * This structure is the node cookie for an FA node that copies
 * specific values into specified locations within the Working Buffer.
 * The purpose of these values is to adapt API values into LTA field
 * lists for use as one of the LTA interfaces:
 * - custom table handler
 * - field transformation
 * - field validation
 *
 * The same format is used to pass field values to each of these
 * interfaces.  The information can be used to describe either
 * input or output LTA field lists.
 */
typedef struct bcmltm_lta_select_field_list_s {
    /*!
     * LTA field list structure WB offset (bcmltd_fields_t)
     */
    uint32_t fields_parameter_offset;

    /*!
     * LTA field list structure WB offset (bcmltd_fields_t) for API LTA input.
     * Offset INVALID if not applicable.
     */
    uint32_t api_fields_parameter_offset;

    /*!
     * WB offset to space for an array of LTA field structure pointers,
     * to be inserted into parameter structure above.
     * (bcmltd_field_t pointer array)
     */
    uint32_t fields_ptrs_offset;

    /*!
     * Maximum number of LTA field values this interface can accept or return.
     */
    uint32_t max_fields;

    /*!
     * Number of field maps in the field_maps list.
     */
    uint32_t num_maps;

    /*! Array of field identifiers. */
    bcmltm_lta_select_field_map_t *lta_select_field_maps;

    /*!
     * The offset into the working buffer of
     * a flag which indicates whether the LTA operation may
     * be conditionally skipped.
     *
     * When applicable, the offset maps to one of these:
     * - PT suppress offset, for value transforms.
     * - Index absent offset, for forward key transforms.
     *   -- Index absent offset may be used to indicate that the LTA
     *      operation _is_ executed for future Track Index support.
     */
    uint32_t lta_suppress_offset;

    /*!
     * The offset into the working buffer of the field select
     * map index corresponding to matching LTA op.
     * If offset is invalid, the op is at the top of the field select
     * hierarchy.
     */
    uint32_t select_map_index_offset;

    /*!
     * Selector map index required to execute matching LTA op.
     * If the value at map_index_offset does not match the value of
     * map_index, the op is not invoked.
     */
    uint32_t select_map_index;
} bcmltm_lta_select_field_list_t;

/*
 * Node Data
 *
 * Two types of nodes are employed in the LTM
 * 1) Field Adaptation
 * 2) Execution Engine
 */

/*!
 * \brief Generic node callback.
 *
 * This callback is invoked within the generic tree traversal sequence
 * to implement a single function node.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md LTM information for this LT
 * \param [in,out] lt_state Runtime state for this LT.
 * \param [in,out] lt_entry Specification of this LT operation
 * \param [in,out] ltm_buffer Pointer to Working Buffer for this op
 * \param [in] node_cookie Context data for the node function
 */
typedef int (*bcmltm_op_node_f)(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief Generic node data
 *
 * This structure contains a pointer to the implementation function
 * and context information necessary to do a single step in a
 * LTM operation.  The binary tree of such nodes comprises a stage in
 * the LTM sequence for this LT.
 */
typedef struct bcmltm_op_node_s {
    /*! The implementation of a single FA/EE step in a LTM operation. */
    bcmltm_op_node_f node_function;

    /*!
     * Context information that customizes how the node_function
     * performs the step for this LT.
     */
    void *node_cookie;
} bcmltm_op_node_t;

/*!
 * \brief Field Adaptation node.
 *
 * The Field Adaptation nodes are designed to convert the API field data
 * encoded values into a format usable by the internal workings of the LTM
 * and other server elements.
 * A node is designed to process API field
 * values into a particular format within the working buffer.
 * It may also perform various transformations and validation steps
 * on the resultant field values.
 */

/*!
 * \brief Field Adaptation node callback.
 *
 * This callback is invoked within the generic tree traversal sequence
 * to implement a single FA stage node.
 */
typedef bcmltm_op_node_f bcmltm_fa_node_f;

/*!
 * \brief Field adaptation node data
 *
 * This structure contains a pointer to the implementation function
 * and context information necessary to do a single step in a
 * LTM operation.  The binary tree of such nodes comprises a stage in
 * the LTM sequence for this LT.
 */
typedef bcmltm_op_node_t bcmltm_fa_node_t;

/*
 * Sample node cookie for PT passthru EE
 *
 * This node cookie is designed to store the information required to traverse
 * the working buffer and dispatch the appropriate set of PTM ops for
 * PT passthru, as well as some simple cases of LT to PT mapping.
 */

/*!
 *
 * BCMLTM Physical Table Opcode Flags.
 *
 * These flags are of two types.
 *
 * 1) Working buffer memory parameter declaration
 * These flags indicate the entry specific parameters required to
 * uniquely identify the device resource for this particular
 * PT element.  Almost all cases will include the entry index,
 * but other options are the port for port-based registers or PortMacro
 * tables, the table instance, the AccType for pipe-specific cases,
 * and other elements as needed.
 *
 * 2) Working buffer memory entry declaration
 * These flags indicate how many entry lines are needed for this PT op
 * These might be the basic entry, entry + field mask, or key + data +
 * field mask.
 */

/* PTM dynamic info flags */
/*! Table index present in param list. */
#define BCMLTM_PT_OP_FLAGS_INDEX        0x01
/*! Index of subentry chunk present in param list. */
#define BCMLTM_PT_OP_FLAGS_SUBENTRY     0x02
/*! Port number present in param list. */
#define BCMLTM_PT_OP_FLAGS_PORT         0x04
/*! Table instance present in param list. */
#define BCMLTM_PT_OP_FLAGS_TABLE_INST   0x08

/* Additional PTM preparation flags */
/*! The priority for a TCAM table entry */
#define BCMLTM_PT_OP_FLAGS_PRIORITY     0x0100
/*! The bank for a PT hash table entry */
#define BCMLTM_PT_OP_FLAGS_BANK         0x0200
/*! AccType present in param list. */
#define BCMLTM_PT_OP_FLAGS_ACCTYPE      0x0400
/*! PDD logical data type present in param list. */
#define BCMLTM_PT_OP_FLAGS_PDD_LDTYPE   0x0800
/*! PT SID override present in param list. */
#define BCMLTM_PT_OP_FLAGS_PT_SID       0x1000
/*! PT SID LTA selector present in param list. */
#define BCMLTM_PT_OP_FLAGS_PT_SELECT    0x2000
/*! PT entry changed flag present in param list. */
#define BCMLTM_PT_OP_FLAGS_PT_CHANGED   0x4000

/* PTM entry format flags */
/*! Key-only entry for PTM in entries buffer. */
#define BCMLTM_PT_OP_FLAGS_KEY          0x010000
/*! Data-only entry (separate from key fields) for PTM-format hash or
 *  TCAM operations in entries buffer. */
#define BCMLTM_PT_OP_FLAGS_DATA         0x020000


/*! Maximum number of PT key or data entries for a single PT op. */
#define BCMLTM_PT_ENTRY_LIST_MAX 4


/*!
 * \brief LTM memory operations list.
 *
 * Device resource parameters to denote physical operations to perform
 * on a specific PT.
 */
typedef struct bcmltm_pt_op_list_s {
    /*!
     * Bitflags which denote which memory parameters are
     * necessary for this PT.  The values are in the param list of
     * the WB. The first param for a specific op is indicated by
     * param_offsets.
     * From the set of BCMLTM_PT_OP_FLAGS_*
     */
    uint32_t flags;

    /*! The number of separated PT operations which will be issued to
     *  the PTM for this PT. */
    uint32_t num_pt_ops;

    /*! uint32_t array size needed to contain the PTM entry for this PT. */
    uint8_t word_size;

    /*! Number of separate PT key entries of word_size to be passed to
     *  the PTM */
    uint8_t key_entries;

    /*! Number of separate PT data entries of word_size to be passed to
     *  the PTM */
    uint8_t data_entries;

    /*!
     * The offsets into the working buffer of the first table parameter
     * value required for each specific entry operation.
     * Subsequent parameters values of an operation follow in
     * successive uint32_t positions of an array in the order
     * of BCMLTM_PT_OP_FLAGS_* indicated by the set state of each flag.
     * This member is an array of Working Buffer offsets.
     */
    uint32_t *param_offsets;

    /*!
     * The offsets for each PTM operation into the working buffer for
     * the given PT entry.
     * This member is an array of Working Buffer offsets.
     */
    uint32_t *buffer_offsets;
} bcmltm_pt_op_list_t;


/*!
 * \brief LTM cache of PT access values for a single LT.
 */
typedef struct bcmltm_pt_mem_args_s {
    /*! Memory flags used by PTM. */
    uint64_t flags;

    /*! Physical Table id. */
    bcmdrd_sid_t pt;

    /*! Default PT dynamic values, such as index and table instance. */
    bcmbd_pt_dyn_info_t *pt_dyn_info;

    /*! Default Hash PT dynamic values, such as table instance and bank. */
    bcmbd_pt_dyn_hash_info_t *pt_dyn_hash_info;

    /*! Default PT static values, which are fixed except for PassThru. */
    void *pt_static_info;
} bcmltm_pt_mem_args_t;

/*!
 * \brief LTM mem list.
 *
 * The list of physical tables which must be processed, with the
 * working buffer offsets to their entries.
 *
 * This is the node cookie for PT PassThru and index LTs.
 */
typedef struct bcmltm_pt_list_s {
    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical operation
     * may be optionally suppressed by LTA Transform.
     *
     * This flag is per-LT.
     */
    uint32_t pt_suppress_offset;

    /*!
     * The number of different physical tables required to process
     * the entries described in the working buffer.
     */
    uint32_t num_pt_view;

    /*!
     * The PT parameters required by the PTM interface to determine
     * the exact memory (or register) instance and the access parameters.
     */
    bcmltm_pt_mem_args_t **mem_args;

    /*!
     * A sequence of schan operations on a single PT described
     * by a mem_args set.
     */
    bcmltm_pt_op_list_t **memop_lists;
} bcmltm_pt_list_t;


/*!
 * \brief LTM keyed memory operations information.
 *
 * Device resource parameters to denote physical operations to perform
 * on a specific keyed PT, when the key and data entries are separate.
 * Further, the input and output buffers are separate.
 *
 * The PTM keyed memory ops interface allows multiple physical entries
 * per key or data.  There is no restriction in how these entries are
 * placed within the Working Buffer.
 *
 * The Output Data entry of a PT lookup may be the Input Data entry
 * of the PT insert when a LT INSERT operation is executed.  This
 * flexibility of the offsets is required for the implementation.
 *
 * The metadata initialization will allocate sufficient Working Buffer
 * entries for this particular keyed PT op, and connect the correct
 * WB location to the offset values here.
 *
 * NOTE:  Due to the reuse of some of these entry buffers during a
 * single keyed LT opcode, the different opcodes of a keyed LT must
 * have separate metadata descriptions, for the PT op entry offsets.
 *
 * Unlike the indexed table PT op list, this structure encodes the
 * information for one keyed PT op only.
 *
 * This is the EE node cookie for keyed PT ops.
 */
typedef struct bcmltm_pt_keyed_op_info_s {
    /*!
     * The offset into the working buffer of
     * the flag which indicates whether the physical operation
     * may be optionally suppressed by LTA Transform.
     *
     * This flag is per-LT.
     */
    uint32_t pt_suppress_offset;

    /*!
     * Bitflags which denote which memory parameters are
     * necessary for this PT.  The values are in the param list of
     * the WB. The first param for a specific op is indicated by
     * param_offsets.
     * From the set of BCMLTM_PT_OP_FLAGS_*
     */
    uint32_t flags;

    /*! uint32_t array size needed to contain the PTM key entry for this PT. */
    uint8_t key_word_size;

    /*! uint32_t array size needed to contain the PTM data entry for this PT. */
    uint8_t data_word_size;

    /*!
     * The PT parameters required by the PTM interface to determine
     * the exact memory (or register) instance and the access parameters.
     */
    bcmltm_pt_mem_args_t *mem_args;

    /*!
     * The offset into the working buffer of the first table parameter
     * value required for each specific entry operation.
     */
    uint32_t param_offset;

    /*!
     * The offset into the working buffer for this PT op's first
     * input key entry.
     */
    uint32_t input_key_buffer_offset[BCMLTM_PT_ENTRY_LIST_MAX];

    /*!
     * The offset into the working buffer for this PT op's first
     * input data entry.
     */
    uint32_t input_data_buffer_offset[BCMLTM_PT_ENTRY_LIST_MAX];

    /*!
     * The offset into the working buffer for this PT op's first
     * output key entry.
     */
    uint32_t output_key_buffer_offset[BCMLTM_PT_ENTRY_LIST_MAX];

    /*!
     * The offset into the working buffer for this PT op's first
     * output data entry.
     */
    uint32_t output_data_buffer_offset[BCMLTM_PT_ENTRY_LIST_MAX];
} bcmltm_pt_keyed_op_info_t;

/*!
 * \brief LTM Tracking index component
 *
 * This structure describes the mapping between the Tracking Index
 * of a Logical Table and a physical parameter of the device
 * resources to which it corresponds.
 * For operations where the API key fields are provided, this
 * structure is used to assemble the Tracking Index from the
 * physical parameters placed in the Working Buffer.
 * For operations where the API key fields are returned (TRAVERSE
 * or Index with Allocation INSERT), this structure is used to
 * determine the physical parameters from the Tracking Index value.
 */
typedef struct bcmltm_track_index_field_s {
    /*!
     * The offset into the working buffer of
     * the device parameter field.
     */
    uint32_t field_offset;

    /*!
     * The mask applied to the tracking index when retrieving
     * this device parameter field.
     */
    uint32_t field_mask;

    /*!
     * The shift applied to this device parameter field when
     * assembling the tracking index.
     */
    uint32_t field_shift;

    /*!
     * If TRUE, this device parameter may be allocated during
     * an INSERT operation to an Index with Allocation LT.
     */
    bool allocatable;
} bcmltm_track_index_field_t;

/*!
 * \brief LTM FA LT Tracking Index calculation
 *
 * This structure is the node_cookie for LT Tracking Index calculation.
 * The Tracking Index is a combination of the device parameters
 * required to identify a unique device resource corresponding to
 * a unique set of LT API Key fields.
 *
 * The Key fields are copied or Transformed into the device physical
 * parameters.  These may include memory index, register array index,
 * register port, pipeline number, PT select, and overlay select.
 * However many there may be, the resulting Tracking Index should
 * fit within a single 32-bit value.
 *
 * The Tracking Index is the offset into the in-use,
 * global in-use, and valid entry bitmaps.
 */
typedef struct bcmltm_track_index_s {
    /*!
     * The offset into the working buffer of
     * the internal index of this logical table for the purpose
     * of tracking the in-use and valid entries.
     */
    uint32_t track_index_offset;

    /*!
     * The maximum value of the Tracking Index.
     * This will correspond to the size of the in-use and
     * valid entries bitmaps.
     */
    uint32_t track_index_max;

    /*!
     * A mask of the LSB bits of the Tracking Index which
     * may be allocated on Index with Allocation LTs.
     *
     * The mask consists of the physical parameters that are mapped
     * from the allocatable Key fields.
     */
    uint32_t track_alloc_mask;

    /*!
     * The offset into the working buffer of
     * the flag which indicates whether all physical parameters
     * are provided via API key fields.
     *
     * When this flag is set, the Tracking Index value must be allocated
     * and returned to the application during INSERT operations.
     *
     * Only Index w/Allocation LTs support this use case.
     */
    uint32_t index_absent_offset;

    /*!
     * The number of physical parameters which compose the Tracking Index.
     */
    uint32_t track_index_field_num;

    /*!
     * The list of per-field information for managing this LT's
     * Tracking Index.
     */
    bcmltm_track_index_field_t *track_index_fields;
} bcmltm_track_index_t;

/*!
 * \brief LTM FA LTM parameters
 *
 * This structure is the node_cookie for LTM internal table traverse.
 * It contains the offsets of the key fields stored in the Working
 * Buffer for LTM internal implementations.
 */
typedef struct bcmltm_fa_ltm_s {
    /*! Working Buffer offset to Table Keys values. */
    uint32_t key_offset[BCMLTM_TABLE_KEYS_COUNT];
} bcmltm_fa_ltm_t;

/*!
 * \brief LTA custom table parameters
 *
 * When the Logical Table Adaptor logic invokes a custom table
 * implementation, the field list for input and output fields are
 * in a different format from the format received by the LTM.
 *
 * During the FA stage of an LTA custom table processing sequence, the
 * field lists are copied to or from the LTM field lists into LTA
 * format field lists within the Working Buffer.  When the
 * operations are invoked in the EE stage, the location of these
 * parameter lists must be available within the EE node functions
 * for LTA.  This metadata structure contains the lists.
 */
typedef struct bcmltm_lta_table_params_s {
    /*!
     * LTA handler list for each supported opcode of an LTA custom
     * table implementation.
     */
    const bcmltd_table_handler_t *handler;

    /*!
     * LTA input field list structure WB offset
     * This WB location is the start of a bcmltd_fields_t structure.
     */
    uint32_t fields_input_offset;

    /*!
     * LTA output field list structure WB offset
     * This WB location is the start of a bcmltd_fields_t structure.
     */
    uint32_t fields_output_offset;

    /*!
      * LTA input field list information for expedited CTH
      * validate error statistics.
      */
    bcmltm_lta_field_list_t *lta_key_list;

    /*!
      * LTA input field list information for expedited CTH
      * validate error statistics.
      */
    bcmltm_lta_field_list_t *lta_field_list;
} bcmltm_lta_table_params_t;

/*!
 * \brief LTA transform parameters
 *
 * When the Logical Table Adaptor logic invokes a transform
 * implementation, the field list for input and output fields are
 * in a different format from the format received by the LTM.
 *
 * During the FA stage of an LTA transform processing sequence, the
 * field lists are copied among the LTM field lists or PTM format entries
 * within the Working Buffer into LTA format field lists within
 * the Working Buffer.  When the operations are invoked in a subsequent
 * FA node, the location of these parameter lists must be available
 * within the EE node functions for LTA.  This metadata structure
 * contains the lists.
 */
typedef struct bcmltm_lta_transform_params_s {
    /*!
     * Function pointer to a LTA Transform implementation.
     */
    bcmltd_handler_transform_op_f lta_transform;

    /*!
     * Function pointer to a LTA Extended transform implementation.
     */
    bcmltd_handler_ext_transform_op_f lta_ext_transform;

    /*!
     * Context cookie argument for a LTA Transform implementation.
     */
    bcmltd_transform_arg_t *lta_args;

    /*!
     * LTA input field list structure WB offset
     * This WB location is the start of a bcmltd_fields_t structure.
     */
    uint32_t fields_input_offset;

    /*!
     * LTA input key field list structure WB offset
     * This WB location is the start of a bcmltd_fields_t structure.
     */
    uint32_t fields_input_key_offset;

    /*!
     * LTA output field list structure WB offset
     * This WB location is the start of a bcmltd_fields_t structure.
     */
    uint32_t fields_output_offset;

    /*!
     * LTA Transform PT SID selector WB offset.
     * If the Transform operation supplies an output SID selector,
     * this is the WB offset at which it should be placed.
     * It should correspond to the PTM op memory args list for
     * the PT operation.
     * If this value is 0, then the output SID selector is not copied.
     */
    uint32_t pt_sid_selector_offset;

    /*!
     * LTA Transform PT SID override WB offset.
     * The WB offset at which an override PT SID value should be placed.
     * The override value may be directly passed from the transform, or
     * indicated by a PT SID selector from the transform output.
     * It should correspond to the PTM op memory args list for the
     * PT operation.
     * If this value is 0, then the output SID is not copied.
     */
    uint32_t pt_sid_override_offset;

    /*!
     * The offset into the working buffer of
     * the status which indicates whether the transform
     * is to be skipped.
     *
     * When applicable, the offset maps to one of these:
     * - PT suppress offset, for value transforms.
     * - Index absent offset, for forward key transforms.
     *
     * When this status is non-zero, the transform is skipped.
     */
    uint32_t transform_suppress_offset;

    /*!
     * The offset into the working buffer of the field select
     * map index corresponding to this transform.
     * If offset is invalid, transform is a the top of the field select
     * hierarchy.
     */
    uint32_t select_map_index_offset;

    /*!
     * Selector map index required to execute this transform.
     * If the value at map_index_offset does not match the value of
     * map_index, this transform is not invoked.
     */
    uint32_t select_map_index;
} bcmltm_lta_transform_params_t;

/*!
 * \brief LTA validate parameters
 *
 * When the Logical Table Adaptor logic invokes a validate
 * implementation, the field list for input fields is
 * in a different format from the format received by the LTM.
 *
 * During the FA stage of an LTA validate processing sequence, the
 * field lists are copied among the LTM field lists or PTM format entries
 * within the Working Buffer into LTA format field lists within
 * the Working Buffer.  This metadata structure contains the list.
 */
typedef struct bcmltm_lta_validate_params_s {
    /*!
     * Function pointer to a LTA Transform implementation.
     */
    bcmltd_handler_field_val_op_f lta_validate;

    /*!
     * Context cookie argument for a LTA Transform implementation.
     */
    bcmltd_field_val_arg_t *lta_args;

    /*!
     * LTA input field list structure WB offset
     * This WB location is the start of a bcmltd_fields_t structure.
     */
    uint32_t fields_input_offset;

    /*!
     * The offset into the working buffer of the field select
     * map index corresponding to this transform.
     * If offset is invalid, transform is a the top of the field select
     * hierarchy.
     */
    uint32_t select_map_index_offset;

    /*!
     * Selector map index required to execute this transform.
     * If the value at map_index_offset does not match the value of
     * map_index, this transform is not invoked.
     */
    uint32_t select_map_index;
} bcmltm_lta_validate_params_t;

/*!
 * \brief Field specifier structure.
 *
 * This structure is the node cookie for EE nodes that process
 * internal values of the Logical Table Manger.  The TABLE_STATS and
 * TABLE_CONTROL tables are examples.  These tables contain custom
 * code to handle each field, but the supported list of fields
 * is contained in this data structure.
 */
typedef struct bcmltm_field_spec_list_s {
    /*! Working Buffer offset to Table Keys values. */
    uint32_t key_offset[BCMLTM_TABLE_KEYS_COUNT];

    /*!
     * Number of field specifiers in this list.
     */
    uint32_t num_field_specs;

    /*! Individual field specifier structures. */
    bcmltm_field_spec_t *field_specs;
} bcmltm_field_spec_list_t;


/*!
 * \brief Execution Engine node callback.
 *
 * This callback is invoked within the generic tree traversal sequence
 * to implement a single EE stage node.
 */
typedef bcmltm_op_node_f bcmltm_ee_node_f;

/*!
 * \brief Execution Engine node data.
 *
 * The Execution Engine nodes are designed to process the data buffer
 * generated by the FA tree to determine the PTM ops corresponding
 * to the LT entry operation.
 * The EE nodes are matched to the FA buffer format, so the trees
 * work in agreement on the buffer data.
 * The default buffer format for many common cases is a set of
 * one or more HW SBUS resource entries (whether register or memory).
 */
typedef bcmltm_op_node_t bcmltm_ee_node_t;

/*
 * Working buffer layout for PT passthru
 */

/*
 * | uint32_t param (index) |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 */

/*
 * Working buffer layout for multiple table entries
 */

/*
 * | uint32_t param (index) |\
 * ... Total of pt_data_count indexes ...
 * | uint32_t param (index) |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 * ... Total of pt_data_count entry data buffers ...
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] |
 */

/*
 * Generic Working Buffer layout
 */

/*
 * | uint32_t param (index)  [param_offsets] |            \
 * | uint32_t param (port)                   |            \
 * | uint32_t param (table inst)             |            \
 * | uint32_t param (AccType)                |            \
 * | uint32_t param (PDD bits)               |            \
 * | uint32_t param (index)  [param_offsets] |            \
 * | uint32_t param (index)  [param_offsets] |            \
 * | uint32_t param (port)                   |            \
 * | uint32_t param (index)  [param_offsets] |            \
 * ... Other parameters ...
 * | uint32_t param (index)  [param_offsets] |            \
 * | uint32_t param (PDD bits)               |            \
 *
 * | uint32_t param (index) |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] (key) |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] (normalized data) |
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] (key-data) |
 * ... Other entries ...
 * | uint32_t entry_data[${SBUS_DATA_MAX_WORDS}] (key-data) |
 */

/*
 * The first sequence of uint32_t values in the Generic Working Buffer
 * are the specific additional parameters necessary to specify the
 * exact physical resource accessed via PTM.
 *
 * Normally, this will be the entry index of the physical table.
 * The complete list of possible parameters are
 *  index - the memory or register index value
 *  port - for PortMacro ops, plus tables and registers per-port
 *  table instance - where that is not otherwise derivable, or PassThru
 *  AccType - for PassThru
 *  PDD bits - for variable action tables in FlexPP devices,
 *              or legacy multi-width entry tables.
 *
 *
 * The second sequence of entry_data elements are the arrays sent to
 * the PTM per-PT.
 * A given PT will normally have a single (key-data) entry element.
 * The complete list of options are
 *  (key-data) - a simply formatted entry, suitable for converting to
 *       an SBUS operation with minimal adjustment.
 *  (key) - The key (in an API sense, might be TCAM key+mask) elements
 *       required by the PTM to uniquely specify an entry for
 *       insert/lookup/delete operations
 *  (normalized data) - the associate data component of a (key) entry,
 *       passed to the PTM in a maximal format capable of containing
 *       all known action fields for this LTM.  The PTM then converts
 *       the action data into the format appropriate for the exact
 *       device table into which it will be placed.
 */

/*!
 * \brief LTM EE track index in-use structure
 *
 * This structure is the node_cookie for LT index tracking routines.
 *
 * This structure is also used for the LTID value of TABLE_* LTs.
 */
typedef struct bcmltm_ee_index_info_s {
    /*!
     * The offset into the working buffer of
     * the track index of this logical table for the purpose
     * of recording the in-use entries.
     *
     * The track index encapsulates a unique physical resource for in-use
     * and allocation bitmaps.
     */
    uint32_t index_offset;
} bcmltm_ee_index_info_t;


/*! Maximum number of units supported. */
#ifndef BCMLTM_MAX_UNITS
#define BCMLTM_MAX_UNITS    BCMDRD_CONFIG_MAX_UNITS
#endif


/*!
 * \brief Check that given unit is valid.
 *
 * \param [in] _u Unit number.
 *
 * \retval TRUE unit valid.
 * \retval FALSE unit is invalid.
 */
#define BCMLTM_UNIT_VALID(_u)                   \
    (((_u) >= 0) && ((_u) < BCMLTM_MAX_UNITS))


/*!
 * \brief Get the LTM Table ID for given Symbol ID.
 *
 * Resolve Symbol ID (LRD or DRD) to LTM Table ID.
 * The LRD and DRD symbol IDs have the same properties.
 * They are in a compact numbering space [0..(sid_max_count-1)].
 *
 * The Symbol ID is used as an index into the LTM metadata table array.
 *
 * \param [in] _sid Symbol ID.
 *
 * \retval LTM Table ID.
 */
#define BCMLTM_SID_TO_LTID(_sid)    ((uint32_t)(_sid))


/*!
 * \brief Get the Symbol ID for given LTM Table ID.
 *
 * Resolve LTM Table ID to Symbol ID (LRD or DRD).
 *
 * The Symbol ID is used as an index into the LTM metadata table array.
 *
 * \param [in] _ltid LTM Table ID.
 *
 * \retval Symbol ID.
 */
#define BCMLTM_LTID_TO_SID(_ltid)    (_ltid)


/*!
 * \brief LTM Table Categories.
 *
 * The LTM support the following main table categories:
 *
 * 1) Physical Table Pass Through (PT Pass Thru)
 * Each of these tables is an 1-1 mapping to physical tables.
 *
 * 2) Logical Table
 * In this category, the logical tables are defined through various
 * mechanisms to produce a mapping from the API fields to
 * either the physical resources on the device or an internal
 * database.
 */
typedef enum bcmltm_table_catg_e {
    BCMLTM_TABLE_CATG_PTHRU = 0,      /*!< PT Pass Thru. */
    BCMLTM_TABLE_CATG_LOGICAL = 1,    /*!< Logical Table. */
    BCMLTM_TABLE_CATG_COUNT           /*!< Total table category count. */
} bcmltm_table_catg_t;

/*!
 * \brief Initialize the LTM metadata for all tables on a given unit.
 *
 * Initialize the LTM metadata for all the table categories on
 * the specified unit.  This includes PT Pass Thru and Logical Tables.
 *
 * This includes allocating and creating all necessary metadata
 * data structures for the unit.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates if this is cold or warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_init(int unit, bool warm);


/*!
 * \brief Cleanup the LTM metadata for all tables on a given unit.
 *
 * Cleanup the LTM metadata for all table categories on the specified unit.
 * This includes PT Pass Thru and Logical Table categories.
 *
 * Memory previously allocated during the initialization process
 * would be freed.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_cleanup(int unit);


/*!
 * \brief Retrieve the metadata for a given table.
 *
 * Get the pointer to the table metadata for a given table id
 * on the specified table category and unit.
 *
 * \param [in] unit Unit number.
 * \param [in] catg Table category (BCMLTM_TABLE_CATG_...).
 * \param [in] ltid Table ID.
 * \param [out] ltm_md_ptr Returns pointer to a table metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_lt_retrieve(int unit, bcmltm_table_catg_t catg,
                      uint32_t ltid,
                      bcmltm_lt_md_t **ltm_md_ptr);

/*!
 * \brief Retrieve the logical metadata for a given unit.
 *
 * Get the pointer to the unit metadata for a all logical tables on a unit.
 *
 * \param [in] unit Unit number.
 * \param [out] ltm_md_ptr Returns pointer to a logical metadata.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_md_logical_retrieve(int unit,
                           bcmltm_md_t **ltm_md_ptr);


#endif /* BCMLTM_MD_INTERNAL_H */
