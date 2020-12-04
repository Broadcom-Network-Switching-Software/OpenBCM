/*! \file bcmltm_wb_types_internal.h
 *
 * Logical Table Manager Working Buffer Types Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_TYPES_INTERNAL_H
#define BCMLTM_WB_TYPES_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmltm/bcmltm_types_internal.h>

/*!
 * \brief Working Buffer Block identifier.
 */
typedef uint32_t bcmltm_wb_block_id_t;


/*!
 * \brief Invalid working buffer block ID.
 */
#define BCMLTM_WB_BLOCK_ID_INVALID    ((bcmltm_wb_block_id_t)-1)


/*!
 * \brief Invalid working buffer offset.
 */
#define BCMLTM_WB_OFFSET_INVALID    0


/*!
 * \brief Working Buffer Block Types.
 *
 * This specifies the working buffer block type.
 *
 * Each block type has a specific memory layout and offset placement
 * for the different pieces of information required by the type.
 */
typedef enum bcmltm_wb_block_type_e {
    /*! PTM Index Table block type (used for LT and Pass Thru). */
    BCMLTM_WB_BLOCK_TYPE_PTM_INDEX = 1,

    /*! PTM Keyed Pass Thru Table block type. */
    BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU = 2,

    /*! PTM Keyed LT Table block type. */
    BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT = 3,

    /*! LT Private block type. */
    BCMLTM_WB_BLOCK_TYPE_LT_PVT = 4,

    /*! LTA Custom Table Handler block type. */
    BCMLTM_WB_BLOCK_TYPE_LTA_CTH = 5,

    /*! LTA Field Transform block type. */
    BCMLTM_WB_BLOCK_TYPE_LTA_XFRM = 6,

    /*! LTA Field Validation block type. */
    BCMLTM_WB_BLOCK_TYPE_LTA_VAL = 7,

    /*! LTM Internal block type. */
    BCMLTM_WB_BLOCK_TYPE_LTM = 8,

    /*! API Cache block type. */
    BCMLTM_WB_BLOCK_TYPE_APIC = 9,

    /*! Field Selection block type. */
    BCMLTM_WB_BLOCK_TYPE_FS = 10,
} bcmltm_wb_block_type_t;


/*!
 * \brief Working Buffer Block for PTM types.
 *
 * This structure contains the working buffer block information for
 * the following PTM block types:
 *     BCMLTM_WB_BLOCK_TYPE_PTM_INDEX
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU
 *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT
 *
 * Although each specific type has a different information, they
 * all share the basic block information and contains 2 sections:
 *    Parameter section (PTM parameters, such as index, port, etc.)
 *    Entry section (PTM entry buffers, such as Key, Data).
 *
 * This PTM block type has sufficient information to derive
 * individual offsets to parameters and entries key/data.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_ptm_s {
    /*! Physical table ID. */
    bcmdrd_sid_t ptid;

    /*! The number of PTM operations to perform for this PT. */
    uint32_t num_ops;

    /*! Size of param section for each PTM op. */
    uint32_t param_wsize;

    /*! Size of entry section for each PTM op. */
    uint32_t entry_wsize;

    /*! Size for each PTM op. */
    uint32_t op_wsize;

    struct {
        /*! PTM parameter offset for first PTM op. */
        uint32_t param;

        /*! PTM entry offset for first PTM op. */
        uint32_t entry;
    } offsets; /*!< PTM offsets for first PTM op.*/

} bcmltm_wb_block_ptm_t;


/*!
 * \brief LT Private Internal Element Offsets.
 *
 * This structure contains the working buffer offsets for the
 * various internal elements of an LT.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_lt_pvt_offsets_s {
    /*!
     * Offset of LT track index.
     *
     * This is used for tracking in-use entries in an indexed LT
     * and may be:
     *   - a result of combining the PT index and additional
     *     memory parameters, such a per-port or per-pipe.
     *   - supplied by a transform.
     */
    uint32_t track_index;

    /*! Offset for the index absent status. */
    uint32_t index_absent;

    /*! Offset for the PT suppress status. */
    uint32_t pt_suppress;
} bcmltm_wb_lt_pvt_offsets_t;


/*!
 * \brief Working Buffer Block for LT Private type.
 *
 * This structure contains the working buffer block information for
 * the following LT block type:
 *     BCMLTM_WB_BLOCK_TYPE_LT_PVT
 *
 * This block is used to hold information for the internal elements
 * of an LT.  This block is per-LT.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_lt_pvt_s {

    /*! Offsets for the LT internal elements. */
    bcmltm_wb_lt_pvt_offsets_t offsets;

} bcmltm_wb_block_lt_pvt_t;


/*!
 * \brief LTA Field Offsets.
 *
 * This structure contains the working buffer offsets for the
 * different information in a LTA field structure bcmltd_fields_t.
 *
 * The LTA field type bcmltd_fields_t is used as argument
 * to pass set of fields into the LTA interfaces such as
 * custom table handlers, field transforms, and field validations.
 *
 * There are 3 sections in the working buffer for a LTA fields struct:
 *     Fields container:              bcmltd_fields_t
 *     Array of pointers to field:   (bcmltd_field_t *) x num_fields
 *     Array of field structs:       (bcmltd_field_t)   x num_fields
 *
 * The number 'num_fields' is the count of the array large
 * enough to accommodate all field elements (fid, idx).
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_lta_fields_offsets_s {
    /*!
     * Offset for the LTA fields structure (bcmltd_fields_t)
     * which contains the set of fields.
     */
    uint32_t param;

    /*!
     * Offset for the array of pointers to fields (bcmltd_field_t),
     * to be inserted into the fields structure above.
     */
    uint32_t ptrs;

    /*!
     * Offset for the array of fields bcmltd_field_t (bcmlt_field_list_t),
     * to be referenced by the array of pointers above.
     */
    uint32_t field;
} bcmltm_wb_lta_fields_offsets_t;


/*!
 * \brief Working Buffer Block for LTA Input/Output block types.
 *
 * This structure contains the working buffer block information for
 * the block types which require input and output fields:
 *     BCMLTM_WB_BLOCK_TYPE_LTA_CTH
 *     BCMLTM_WB_BLOCK_TYPE_LTA_XFRM
 *     BCMLTM_WB_BLOCK_TYPE_LTA_VAL
 *
 * The LTA block contains 3 sections:
 *     Input Fields
 *     Input Key Fields
 *     Output Fields
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_lta_s {
    /*! The number of Input fields. */
    uint32_t in_num_fields;

    /*! The number of Input fields. */
    uint32_t in_key_num_fields;

    /*! The number of Output fields. */
    uint32_t out_num_fields;

    struct {
        /*! Offset for the Input fields. */
        bcmltm_wb_lta_fields_offsets_t in;

        /*! Offset for the Input Key fields. */
        bcmltm_wb_lta_fields_offsets_t in_key;

        /*! Offset for the Output fields. */
        bcmltm_wb_lta_fields_offsets_t out;
    } offsets; /*!< Offsets for LTA sections: Input and Output. */

} bcmltm_wb_block_lta_t;


/*!
 * \brief LTM key fields Offsets.
 *
 * This structure contains the working buffer offsets for the
 * key fields in the LTM block.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ltm_offsets_s {
    /*!
     * Offsets for the LTM internal tables' key field values.
     */
    uint32_t key_value_offsets[BCMLTM_TABLE_KEYS_COUNT];
} bcmltm_wb_ltm_offsets_t;


/*!
 * \brief Working Buffer Block for LTM internal type.
 *
 * This structure contains the working buffer block information for
 * the following LT block type:
 *     BCMLTM_WB_BLOCK_TYPE_LTM
 *
 * This block is used LTs of type LTM internal.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_ltm_s {
    /*!
     * Offsets for the LTM internal tables' key field values.
     */
    bcmltm_wb_ltm_offsets_t offsets;

} bcmltm_wb_block_ltm_t;


/*!
 * \brief Working Buffer Block for API Cache block type.
 *
 * This structure contains the working buffer block information for
 * the following API Cache block type:
 *     BCMLTM_WB_BLOCK_TYPE_APIC
 *
 * This block is used to hold information for the API fields of an LT.
 * This block is per-LT.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_apic_s {
    /*! Offset for the first field element. */
    uint32_t field_base_offset;

    /*! Total number of field elements (fid, idx) allocated in block. */
    uint32_t num_field_elems;

    /*! Size of one field element in working buffer. */
    uint32_t field_elem_wsize;

    /*! The number of fields in list. */
    uint32_t num_fields;

    /*!
     * List of field ID and its corresponding field index count.
     * This information will help identify the offset for a given
     * field element (fid, idx).
     */
    bcmltm_field_idx_count_t *fields;
} bcmltm_wb_block_apic_t;

/*!
 * \brief Working Buffer Block for Field Selection block type.
 *
 * This structure contains the working buffer block information for
 * the following Field Selection block type:
 *     BCMLTM_WB_BLOCK_TYPE_FS
 *
 * This block is per-LT.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_fs_s {
    /*! Offset to the first field selection map index. */
    uint32_t map_index_base_offset;

    /*! The number of field selections. */
    uint32_t num_sels;

    /*!
     * List of field selection IDs.
     * This information will help identify offsets for a given
     * field selection ID.
     */
    bcmltm_field_selection_id_t *sel_id_list;
} bcmltm_wb_block_fs_t;

/*!
 * \brief Working Buffer Block.
 *
 * This structure contains the working buffer information for
 * a given block.
 *
 * All size and offset information are in words.
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_block_s {
    /*! Working buffer block ID. */
    bcmltm_wb_block_id_t id;

    /*! Working buffer size of this block in words. */
    uint32_t wsize;

    /*! Working buffer base offset for this block. */
    uint32_t base_offset;

    /*! Block type. */
    bcmltm_wb_block_type_t type;

    union {
        /*!
         * PTM block types:
         *     BCMLTM_WB_BLOCK_TYPE_PTM_INDEX
         *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_PTHRU
         *     BCMLTM_WB_BLOCK_TYPE_PTM_KEYED_LT
         */
        bcmltm_wb_block_ptm_t ptm;

        /*!
         * LT private block type:
         *     BCMLTM_WB_BLOCK_TYPE_LT_PVT
         */
        bcmltm_wb_block_lt_pvt_t lt_pvt;

        /*!
         * LTA block types:
         *     BCMLTM_WB_BLOCK_TYPE_LTA_CTH
         *     BCMLTM_WB_BLOCK_TYPE_LTA_XFRM
         *     BCMLTM_WB_BLOCK_TYPE_LTA_VAL
         */
        bcmltm_wb_block_lta_t lta;

        /*!
         * LTM block type:
         *     BCMLTM_WB_BLOCK_TYPE_LTM
         */
        bcmltm_wb_block_ltm_t ltm;

        /*!
         * API Cache block type:
         *     BCMLTM_WB_BLOCK_TYPE_APIC
         */
        bcmltm_wb_block_apic_t apic;

        /*!
         * Field Selection block type:
         *     BCMLTM_WB_BLOCK_TYPE_FS
         */
        bcmltm_wb_block_fs_t fs;
    } u; /*!< Union of block type structures. */


    /*! Next working buffer block. */
    struct bcmltm_wb_block_s *next;
} bcmltm_wb_block_t;


/*!
 * \brief Working Buffer Handle.
 *
 * This structure contains the working buffer information for
 * a given table.
 *
 * This handle maintains the list of all the working
 * buffer blocks that pertains to the table.
 *
 * All size and offset information are in words.
 * LTM working buffer offset calculations are all based on uint32_t.
 *
 * An offset of zero is invalid.
 */
typedef struct bcmltm_wb_handle_s {
    /*! Total working buffer size in words. */
    uint32_t wsize;

    /*! Table ID. */
    uint32_t sid;

    /*! Indicates if table is logical or physical. */
    bool logical;

    /*! Number of blocks. */
    uint32_t num_blocks;

    /*! List of working buffer blocks. */
    bcmltm_wb_block_t *blocks;
} bcmltm_wb_handle_t;


#endif /* BCMLTM_WB_TYPES_INTERNAL_H */
