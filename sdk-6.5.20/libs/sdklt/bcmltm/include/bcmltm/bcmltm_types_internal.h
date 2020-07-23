/*! \file bcmltm_types_internal.h
 *
 * Logical Table Manager Internal Type Definitions.
 *
 * These basic types are used to capture table and field information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_TYPES_INTERNAL_H
#define BCMLTM_TYPES_INTERNAL_H

#include <shr/shr_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>

/*! Reserved value for invalid LTID (SID) */
#define BCMLTM_LTID_INVALID ((uint32_t)-1)

/*! The maximum permitted number of entries in a keyed LT */
#define BCMLTM_KEYED_LT_ENTRY_MAX ((uint32_t)-1)

/*
 * The following macros BCMLTM_TABLE_TYPE_IS_xxx()
 * expect the argument (_t) to be of type bcmltm_table_type_t.
 */

/*! Check for Table Type: Simple Index. */
#define BCMLTM_TABLE_TYPE_IS_SIMPLE_INDEX(_t)   \
    ((_t) == BCMLTM_TABLE_TYPE_SIMPLE_INDEX)

/*! Check for Table Type: Index with Allocation. */
#define BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(_t)    \
    ((_t) == BCMLTM_TABLE_TYPE_ALLOC_INDEX)

/*! Check for Table Type: Configuration. */
#define BCMLTM_TABLE_TYPE_IS_CONFIG(_t)   \
    ((_t) == BCMLTM_TABLE_TYPE_CONFIG)

/*!
 * Check for Table Type: Index (Simple/IwA/Config).
 *
 * Note that a config table is considered a type of index table.
 * It is a special index table with no keys.
 */
#define BCMLTM_TABLE_TYPE_IS_INDEX(_t)        \
    (BCMLTM_TABLE_TYPE_IS_SIMPLE_INDEX(_t) || \
     BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX(_t) ||  \
     BCMLTM_TABLE_TYPE_IS_CONFIG(_t))

/*! Check for Table Type: Hash. */
#define BCMLTM_TABLE_TYPE_IS_HASH(_t)           \
    ((_t) == BCMLTM_TABLE_TYPE_HASH)

/*! Check for Table Type: TCAM. */
#define BCMLTM_TABLE_TYPE_IS_TCAM(_t)           \
    ((_t) == BCMLTM_TABLE_TYPE_TCAM)

/*! Check for Table Type: Keyed (Hash/TCAM). */
#define BCMLTM_TABLE_TYPE_IS_KEYED(_t)          \
    (BCMLTM_TABLE_TYPE_IS_HASH(_t) ||           \
     BCMLTM_TABLE_TYPE_IS_TCAM(_t))

/*
 * The following macros BCMLTM_TABLE_MAP_IS_xxx()
 * expect the argument (_t) to be of type bcmltm_table_map_type_t.
 */

/*! Check for Table Map: Direct Mapped PT. */
#define BCMLTM_TABLE_MAP_IS_DM(_t)              \
    ((_t) == BCMLTM_TABLE_MAP_DIRECT)

/*! Check for Table Map: Custom Table Handler. */
#define BCMLTM_TABLE_MAP_IS_CTH(_t)             \
    ((_t) == BCMLTM_TABLE_MAP_CTH)

/*! Check for Table Map: LTM based. */
#define BCMLTM_TABLE_MAP_IS_LTM(_t)             \
    ((_t) == BCMLTM_TABLE_MAP_LTM)


/*
 * The following macros BCMLTM_TABLE_MODE_IS_xxx()
 * expect the argument (_mode) to be of type bcmltm_table_mode_t.
 */
/*! Check if table operating mode is Modeled */
#define BCMLTM_TABLE_MODE_IS_MODELED(_mode)        \
    ((_mode) == BCMLTM_TABLE_MODE_MODELED)

/*! Check if table operating mode is Interactive */
#define BCMLTM_TABLE_MODE_IS_INTERACTIVE(_mode)    \
    ((_mode) == BCMLTM_TABLE_MODE_INTERACTIVE)


/*
 * The following macros BCMLTM_TABLE_ATTR_xxx()
 * expect the argument (_attr) to be of type bcmltm_table_attr_t.
 */

/*! Check if table type Index with Allocation */
#define BCMLTM_TABLE_ATTR_TYPE_IS_IWA(_attr)            \
    (BCMLTM_TABLE_TYPE_IS_ALLOC_INDEX((_attr).type))

/*! Check if table is Shared (Overlay) */
#define BCMLTM_TABLE_ATTR_IS_SHARED(_attr)              \
    ((_attr).flag & BCMLTM_TABLE_FLAG_OVERLAY)

/*! Check if table operating mode is Modeled */
#define BCMLTM_TABLE_ATTR_MODE_IS_MODELED(_attr)        \
    (BCMLTM_TABLE_MODE_IS_MODELED((_attr).mode))

/*! Check if table operating mode is Interactive */
#define BCMLTM_TABLE_ATTR_MODE_IS_INTERACTIVE(_attr)    \
    (BCMLTM_TABLE_MODE_IS_INTERACTIVE((_attr).mode))


/*!
 * \brief Table Type.
 *
 * This specifies the key organization type of a table.
 */
typedef enum bcmltm_table_type_e {
    /*! None. */
    BCMLTM_TABLE_TYPE_NONE = 0,

    /*! Simple Index. */
    BCMLTM_TABLE_TYPE_SIMPLE_INDEX = 1,

    /*! Index with Allocation. */
    BCMLTM_TABLE_TYPE_ALLOC_INDEX = 2,

    /*! Hash. */
    BCMLTM_TABLE_TYPE_HASH = 3,

    /*! TCAM. */
    BCMLTM_TABLE_TYPE_TCAM = 4,

    /*! Configuration. */
    BCMLTM_TABLE_TYPE_CONFIG = 5
} bcmltm_table_type_t;

/*!
 * \brief Table Map Type.
 *
 * This specifies the map type implementation of a table.
 */
typedef enum bcmltm_table_map_type_e {
    /*! None. */
    BCMLTM_TABLE_MAP_NONE = 0,

    /*! Direct Mapped PT. */
    BCMLTM_TABLE_MAP_DIRECT = 1,

    /*! Custom Table Handler. */
    BCMLTM_TABLE_MAP_CTH = 2,

    /*! LTM based. */
    BCMLTM_TABLE_MAP_LTM = 3
} bcmltm_table_map_type_t;

/*!
 * \name Table Opcode Flags.
 * \anchor BCMLTM_OPCODE_FLAG_xxx
 */
/*! \{ */
/*! INSERT opcode. */
#define BCMLTM_OPCODE_FLAG_INSERT      0x0001
/*! LOOKUP opcode. */
#define BCMLTM_OPCODE_FLAG_LOOKUP      0x0002
/*! UPDATE opcode. */
#define BCMLTM_OPCODE_FLAG_UPDATE      0x0004
/*! DELETE opcode. */
#define BCMLTM_OPCODE_FLAG_DELETE      0x0008
/*! TRAVERSE opcode. */
#define BCMLTM_OPCODE_FLAG_TRAVERSE    0x0010
/*! \} */

/*!
 * \brief LT Opcodes.
 *
 * This specifies the opcodes supported in a table
 * \ref BCMLTM_OPCODE_FLAG_xxx.
 */
typedef uint32_t bcmltm_opcodes_t;

/*!
 * \brief Table Operating Modes.
 *
 * A Logical table has 2 modes of operations which is specified
 * in the LT map file:
 *    Modeled
 *    Interactive
 *
 * This mode indicates the corresponding PTM interface, modeled or
 * interactive, to be used during the PTM operation.
 *
 * Note that PT Pass Thru always operates on interactive mode.
 */
typedef enum bcmltm_table_mode_e {
    /*! Modeled mode. */
    BCMLTM_TABLE_MODE_MODELED = 0,

    /*! Interactive mode. */
    BCMLTM_TABLE_MODE_INTERACTIVE = 1,
} bcmltm_table_mode_t;


/* Table Flags */
/*! Read only Table flag. */
#define BCMLTM_TABLE_FLAG_READ_ONLY    0x00000001

/*! Overlay Table flag (table share device resource entries). */
#define BCMLTM_TABLE_FLAG_OVERLAY      0x00000002

/*!
 * \brief Table Attributes.
 *
 * This structure contains the table attributes.
 * For LTs, the table attributes are derived from the LT map file.
 */
typedef struct bcmltm_table_attr_s {
    /*!
     * Table type.
     * This specifies the key organization type of a table.
     */
    bcmltm_table_type_t type;

    /*!
     * Table map type.
     * This specifies the map type implementation of a table.
     */
    bcmltm_table_map_type_t map_type;

    /*!
     * Operation mode: Modeled or Interactive.
     */
    bcmltm_table_mode_t mode;

    /*!
     * Table flag.
     */
    uint32_t flag;

    /*!
     * Minimum table index allowed.
     * This value normally corresponds to the PT index.
     */
    uint32_t index_min;

    /*!
     * Maximum table index allowed.
     * This value normally corresponds to the PT index.
     */
    uint32_t index_max;

    /*!
     * Size of table index allowed.
     * This value normally corresponds to the PT index.
     * If zero, table has no corresponding PT index.
     */
    uint32_t index_size;

    /*!
     * Minimum memory parameter value allowed.
     * This value normally corresponds to the PT memory port or instance.
     */
    uint32_t mem_param_min;

    /*!
     * Maximum memory parameter value allowed.
     * This value normally corresponds to the PT memory port or instance.
     */
    uint32_t mem_param_max;

    /*!
     * Size of table memory parameter allowed.
     * This value normally corresponds to the PT memory port or instance.
     * If zero, table has no corresponding memory parameter.
     */
    uint32_t mem_param_size;

    /*!
     * Minimum PT SID selector value allowed.
     * This value corresponds to table transforms.
     */
    uint32_t sid_sel_min;

    /*!
     * Maximum PT SID selector value allowed.
     * This value corresponds to table transforms.
     */
    uint32_t sid_sel_max;

    /*!
     * Size of PT SID selector allowed.
     * This value corresponds to table transforms.
     * If zero, table has no corresponding PT SID selectors.
     */
    uint32_t sid_sel_size;

    /*!
     * Global LT ID.
     * Overlay tables share device resource entries and requires
     * a table to maintain the global in-use bitmap.  This is the ID
     * of such global table.
     */
    bcmltd_sid_t global_ltid;

    /*! Supported opcodes. */
    bcmltm_opcodes_t opcodes;
} bcmltm_table_attr_t;


/*!
 * \brief TABLE_* Logical Table Keys types.
 *
 * TABLE_* LTs use various key elements, including the LTID of
 * the Logical Table to be analyzed.  This list contains all of the
 * key fields which should be stored in the Working Buffer for
 * used by the internal table read/write function nodes.
 */
typedef enum bcmltm_table_keys_e {
    BCMLTM_TABLE_KEYS_LTID = 0,          /*!< Logical Table ID. */
    BCMLTM_TABLE_KEYS_LT_FIELD_ID = 1,   /*!< LT Field ID. */
    BCMLTM_TABLE_KEYS_SELECT_GROUP = 2,  /*!< LT Field Select Group. */
    BCMLTM_TABLE_KEYS_RESOURCE_INFO = 3, /*!< LT Resource Info Index. */
    BCMLTM_TABLE_KEYS_COUNT
} bcmltm_table_keys_t;


/*!
 * \brief Field ID and Index count.
 */
typedef struct bcmltm_fid_idx_s {
    /*! Field ID. */
    uint32_t fid;

    /*! Field Index Count. */
    uint32_t idx_count;
} bcmltm_fid_idx_t;


/*!
 * \brief Field ID and Index count.
 *
 * This structure contains a field ID with its corresponding index count.
  */
typedef bcmltm_fid_idx_t bcmltm_field_idx_count_t;

/*!
 * \brief LTM Field Types.
 *
 * LTM has the following field map types:
 *    Keys
 *    Values
 *
 * LTM constructs different metadata field lists depending on the field
 * map type in order to process the field map at the proper time and
 * for the appropriate opcode.
 *
 * For instance, Key field maps are processed earlier.  In addition,
 * only Key field maps are used for certain operations, such as
 * LOOKUP and DELETE.
 *
 * Note this type reflects the type of the field map, not how the field
 * is defined. For example, an LT field is defined as a Key, but it
 * maps to a PT Key and a PT Value, then there will be 2 field maps,
 * one of type Key and another of type Value:
 *     LT:  LTF1: PT1.__INDEX  --> Key
 *          LTF1: PT1.PTF5     --> Value
 */
typedef enum bcmltm_field_type_e {
    /*! Field maps to a Key. */
    BCMLTM_FIELD_TYPE_KEY = 0,

    /*! Field maps to a Value. */
    BCMLTM_FIELD_TYPE_VALUE = 1,

    BCMLTM_FIELD_TYPE_COUNT
} bcmltm_field_type_t;


/*!
 * \brief LTM Field Direction.
 *
 * Indicates if the field direction is INPUT or OUTPUT.
 *
 * This information is needed for cases where the working buffer offset
 * of the field differs depending on the direction of a field operation.
 * For instance, Keyed LTs use different buffers for the PTM
 * input and output entries.
 */
typedef enum bcmltm_field_dir_e {
    /*! Input field. */
    BCMLTM_FIELD_DIR_IN = 0,

    /*! Output field. */
    BCMLTM_FIELD_DIR_OUT = 1,
} bcmltm_field_dir_t;

/*!
 * \brief Field Key Variants.
 *
 * This information is used for selecting the desired set of key fields
 * for Index with Allocation tables.
 */
typedef enum bcmltm_field_key_type_e {
    /*! No allocation involved, select all key fields. */
    BCMLTM_FIELD_KEY_TYPE_ALL = 0,

    /*!
     * Select only keys fields that are required during an INSERT
     * operation on Index with Allocation LTs.
     */
    BCMLTM_FIELD_KEY_TYPE_IWA_REQ = 1,

    /*!
     * Select only keys fields that are allocatable (optional)
     * during an INSERT operation on Index with Allocation LTs.
     * If keys are not present, they are allocated during the operation.
     */
    BCMLTM_FIELD_KEY_TYPE_IWA_ALLOC = 2,
} bcmltm_field_key_type_t;

/*!
 * \brief LTM Fixed Field Types.
 *
 * LTM has the following fixed field map types:
 *    Keys
 *    Values
 *
 * LTM constructs different metadata fixed field lists, key and value,
 * depending on the field map type.  These lists are processed
 * in different stages of an opcode.
 */
typedef enum bcmltm_fixed_field_type_e {
    /*! Fixed key field. */
    BCMLTM_FIXED_FIELD_TYPE_KEY = 0,

    /*! Fixed value field. */
    BCMLTM_FIXED_FIELD_TYPE_VALUE = 1,
} bcmltm_fixed_field_type_t;

/*!
 * \brief Field Selection ID.
 *
 * This ID uniquely identifies a field selection in a logical table.
 *
 * A field selection defines a field selector, within a select hierarchy,
 * along with its associated selector values and corresponding
 * maps.
 *
 * The index to the LRD field selector data array is used as the
 * field selection ID.
 *
 * Note that the field selection ID is not the same as
 * the selector field ID.  A field can be a selector more than
 * once (in nested selects) and each instance of this requires
 * a unique selection ID.
 *
 * Example:
 * (Map syntax is for illustration only, i.e. not official)
 *   map T {
 *       select S1 {
 *           10: S2: PT.F1,
 *               A:  PT.F2,
 *               select S2 {
 *                   1: B: PT.F3;
 *                   2: C: PT.F4;
 *           };
 *           20: S2: PT.F5,
 *               A:  PT.F6,
 *               select S2 {
 *                   1: B: PT.F7;
 *                   2: C: PT.F8;
 *           };
 *       }
 *   }
 *
 * There are 3 field selections, each with a unique ID:
 *   - ID_1:  S1
 *   - ID_2:  S2 (S1:10 parent)
 *   - ID_3:  S2 (S1:20 parent)
 */
typedef uint32_t bcmltm_field_selection_id_t;

/*!
 * \brief Field Selector Type.
 *
 * Indicates if the field selector for a given selection group
 * is a key or value field.
 *
 * The unconditional selection group is assigned NONE.
 */
typedef enum bcmltm_field_selector_type_e {
    /*! Unconditional selection group (no selector). */
    BCMLTM_FIELD_SELECTOR_TYPE_NONE,

    /*! Key field selector. */
    BCMLTM_FIELD_SELECTOR_TYPE_KEY,

    /*! Value field selector. */
    BCMLTM_FIELD_SELECTOR_TYPE_VALUE,
} bcmltm_field_selector_type_t;

/*!
 * \brief Field selection ID for unconditional selection group (no selector).
 */
#define BCMLTM_FIELD_SELECTION_ID_UNCOND    BCMLRD_INVALID_SELECTOR_INDEX

/*!
 * \brief Field selection level for unconditional selection group - Level 0.
 */
#define BCMLTM_FIELD_SELECTION_LEVEL_UNCOND       0

/*!
 * \brief Field selection map index for unconditional selection group.
 */
#define BCMLTM_FIELD_SELECTION_MAP_INDEX_UNCOND   0

/*!
 * \brief Field selection ID for key fields.
 *
 * Key fields cannot be subject to any field selector.  As such,
 * the database for key fields uses the selection ID for
 * unconditional selection group.
 */
#define BCMLTM_FIELD_SELECTION_ID_KEY     BCMLTM_FIELD_SELECTION_ID_UNCOND

/*!
 * \brief Field selection level for a key field selector type - Level 1.
 *
 * This is case where the field selector is a key field.
 * The level is always 1 because a key field selector cannot be nested.
 */
#define BCMLTM_FIELD_SELECTION_LEVEL_KEY_SELECTOR          1

/*!
 * \brief Field Transform Types.
 *
 * This information is used for classifying and selecting
 * the corresponding field transform type during metadata construction.
 */
typedef enum bcmltm_field_xfrm_type_e {
    /*! Forward key transform. */
    BCMLTM_FIELD_XFRM_TYPE_FWD_KEY,

    /*! Forward value transform. */
    BCMLTM_FIELD_XFRM_TYPE_FWD_VALUE,

    /*! Reverse key transform. */
    BCMLTM_FIELD_XFRM_TYPE_REV_KEY,

    /*! Reverse value transform. */
    BCMLTM_FIELD_XFRM_TYPE_REV_VALUE,
} bcmltm_field_xfrm_type_t;

/*!
 * \brief Field Validation Types.
 *
 * This information is used for classifying and selecting
 * the corresponding field validation type during metadata construction.
 */
typedef enum bcmltm_field_validate_type_e {
    /*! Key field validation. */
    BCMLTM_FIELD_VALIDATE_TYPE_KEY = 0,

    /*! Value field validation. */
    BCMLTM_FIELD_VALIDATE_TYPE_VALUE = 1,
} bcmltm_field_validate_type_t;

/*!
 * \brief CTH Interface Types.
 *
 * This information is used to indicate the custom table
 * handler interface flow.
 */
typedef enum bcmltm_cth_handler_intf_type_e {
    /*!
     * This is the default mode and indicates LTM to use
     * its standard flow, which involves steps such as
     * validations of limits in value fields.
     */
    BCMLTM_CTH_HANDLER_INTF_TYPE_STANDARD = 0,

    /*!
     * This is used to expedite the LTM processing of
     * custom table handler functions.  As a result,
     * LTM may skip steps that are then required to be
     * managed by the handler.
     */
    BCMLTM_CTH_HANDLER_INTF_TYPE_EXPEDITED = 1,
} bcmltm_cth_handler_intf_type_t;

#endif /* BCMLTM_TYPES_INTERNAL_H */
