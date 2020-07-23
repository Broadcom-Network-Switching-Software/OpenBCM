/*! \file bcmltm_wb_ptm_internal.h
 *
 * Logical Table Manager Working Buffer Definitions for PTM interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_WB_PTM_INTERNAL_H
#define BCMLTM_WB_PTM_INTERNAL_H

#include <shr/shr_types.h>

#include <bcmptm/bcmptm_types.h>
#include <bcmltm/bcmltm_internal.h>
#include <bcmltm/bcmltm_wb_types_internal.h>

/*!
 * \brief PTM Parameter Offsets.
 *
 * This structure contains the working buffer offsets for the
 * different memory parameters used on PTM operations.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_param_offsets_s {
    /*! Offset for the INDEX parameter. */
    uint32_t index;

    /*! Offset for the SUBENTRY parameter. */
    uint32_t subentry;

    /*! Offset for the PORT parameter. */
    uint32_t port;

    /*! Offset for the TABLE_INST parameter. */
    uint32_t table_inst;

    /*! Offset for the PRIORITY parameter. */
    uint32_t priority;

    /*! Offset for the BANK parameter. */
    uint32_t bank;

    /*! Offset for the ACCTYPE parameter. */
    uint32_t acctype;

    /*! Offset for the PT SID override parameter. */
    uint32_t pt_sid_override;

    /*! Offset for the PT SID selector parameter. */
    uint32_t pt_sid_selector;

    /*! Offset for the PT entry Changed status. */
    uint32_t pt_changed;

    /*! Logical data type for PDD. */
    uint32_t pdd_ldtype[BCMLTM_PT_ENTRY_LIST_MAX];
} bcmltm_wb_ptm_param_offsets_t;


/*!
 * \brief PTM Entry Offsets for Index PT.
 *
 * This structure contains the working buffer offsets for the
 * entry buffer used for PTM operations on Index physical tables.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_entry_index_offsets_s {
    /*! Offset for the entry data for Indexed physical tables. */
    uint32_t data;
} bcmltm_wb_ptm_entry_index_offsets_t;


/*!
 * \brief PTM Entry Offsets for Keyed PT Pass Thru.
 *
 * This structure contains the working buffer offsets for the
 * entry buffers used for PTM operations on Keyed PT Pass Thru (Hash/TCAM).
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_entry_keyed_pthru_offsets_s {
    /*! Offsets for the KEY entry for Keyed tables (TCAM, Hash). */
    uint32_t key;

    /*! Offsets for the DATA entry for Keyed tables (TCAM, Hash). */
    uint32_t data;
} bcmltm_wb_ptm_entry_keyed_pthru_offsets_t;


/*!
 * Presently, the total number of entry buffers required for a
 * LT keyed table operation is 3 sets and are used as follows
 * for the different PTM operations:
 *
 *                 LOOKUP/TRAVERSE    INSERT/DELETE
 *    Input Key          K0                 K0
 *    Input Data         D0                 D1
 *    Output Key         K1                 K2
 *    Output Data        D1                 D2
 *
 * The PTM operations for TRAVERSE are GET_FIRST and GET_NEXT.
 *
 * Note that there is not an explicit PTM UPDATE operation,
 * The LT UPDATE translates to a PTM LOOKUP and INSERT.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM 3

/*!
 * The first set of PTM entries are for the LOOKUP/TRAVERSE input (search key).
 * For INSERT and DELETE ops, this key set is used.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_0 0

/*!
 * The second set of PTM entries are for the LOOKUP/TRAVERSE output.
 * For INSERT operations, this data set is used.
 * For DELETE, there may be reference counts to adjust.
 * Since an LT UPDATE results in a PTM INSERT operation, this data set
 * will be modified.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_1 1

/*!
 * The third set of PTM entries are for the INSERT/DELETE opcode output.
 * While this output is not expected to be relevant, since the
 * lookup already determined the previous entry to be replaced,
 * some future use case may require this output for some analysis.
 * Space is reserved to avoid overwriting the other two entry buffers.
 */
#define BCMLTM_WB_PTM_KEYED_ENTRY_SET_2 2


/*!
 * \brief PTM Entry Offsets for Keyed LT.
 *
 * This structure contains the working buffer offsets for the
 * entry buffers used for PTM operations on Keyed LT (Hash/TCAM).
 *
 * Offsets are returned based on the three copy keyed table model
 * described for the BCMLTM_WB_PTM_KEYED_ENTRY_SET_* flags.
 * Each set contains 4 PT entries.
 *
 * All offsets are the offsets from the start of the Working Buffer and
 * are in words.
 */
typedef struct bcmltm_wb_ptm_entry_keyed_lt_offsets_s {
    /*! Offsets for the KEY entries for Keyed tables (TCAM, Hash). */
    uint32_t key[BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM][BCMLTM_PT_ENTRY_LIST_MAX];

    /*! Offsets for the DATA entries for Keyed tables (TCAM, Hash). */
    uint32_t data[BCMLTM_WB_PTM_KEYED_ENTRY_SET_NUM][BCMLTM_PT_ENTRY_LIST_MAX];
} bcmltm_wb_ptm_entry_keyed_lt_offsets_t;


/*!
 * \brief Get PTM operation flags BCMLTM_PT_OP_FLAGS_ for PT on PT Pass Thru.
 *
 * This routine returns the PTM operation flags BCMLTM_PT_OP_FLAGS_
 * corresponding to given PT for PT Pass Thru.  These flags indicate the
 * parameters and entries required for PTM operations.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol (Physical Table) ID.
 *
 * \retval Working Buffer flags BCMLTM_PT_OP_FLAGS_
 */
extern uint32_t
bcmltm_wb_ptm_op_flags_pthru_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get PTM operation flags BCMLTM_PT_OP_FLAGS_ for PT on LT case.
 *
 * This routine returns the PTM operation flags BCMLTM_PT_OP_FLAGS_
 * corresponding to given PT for LT case.  These flags indicate the
 * parameters and entries required for PTM operations.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol (Physical Table) ID.
 *
 * \retval Working Buffer flags BCMLTM_PT_OP_FLAGS_
 */
extern uint32_t
bcmltm_wb_ptm_op_flags_lt_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Add a PTM working buffer block for a PT Pass Thru table.
 *
 * This function adds a working buffer block required for PTM operation
 * for a PT Pass Thru table.  The corresponding PTM block (index or
 * keyed) is added based on the PT flags provided.
 *
 * \param [in] unit Unit number.
 * \param [in] ptid Physical table ID.
 * \param [in] flags PT operation BCMLTM_PT_OP_FLAGS_ for table.
 * \param [in] num_ops Number of PTM operations required for this table.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_ptm_pthru_add(int unit,
                              bcmdrd_sid_t ptid,
                              uint32_t flags, uint32_t num_ops,
                              bcmltm_wb_handle_t *handle,
                              bcmltm_wb_block_id_t *block_id,
                              bcmltm_wb_block_t **block_ptr);

/*!
 * \brief Add a PTM working buffer block for a Logical Table.
 *
 * This function adds a working buffer block required for PTM operation
 * for a LT table.  The corresponding PTM block (index or
 * keyed) is added based on the LT type provided.
 *
 * \param [in] unit Unit number.
 * \param [in] ptid Physical table ID.
 * \param [in] table_type Table type.
 * \param [in] num_ops Number of PTM operations required for this table.
 * \param [in,out] handle Working buffer handle.
 * \param [out] block_id Returning ID for new working buffer block.
 * \param [out] block_ptr Optional return of pointer to working buffer block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_block_ptm_lt_add(int unit,
                           bcmdrd_sid_t ptid,
                           bcmltm_table_type_t table_type,
                           uint32_t num_ops,
                           bcmltm_wb_handle_t *handle,
                           bcmltm_wb_block_id_t *block_id,
                           bcmltm_wb_block_t **block_ptr);

/*!
 * \brief Get maximum working buffer size in words for a PT Pass Thru table.
 *
 * This function returns the maximum working buffer size, in words,
 * for a PT Pass Thru table with the specified number of PTM operations.
 *
 * \param [in] unit Unit number.
 * \param [in] num_ops Number of PTM operations.
 *
 * \retval Working buffer size in words.
 */
extern uint32_t
bcmltm_wb_ptm_pthru_wsize_max_get(int unit,
                                  uint32_t num_ops);

/*!
 * \brief Get size of one PT entry element allocated in the working buffer.
 *
 * This function gets the size allocated in the working buffer for
 * one PT entry element in the PTM Entry section.
 *
 * Note that depending on the table type, the PTM entry section
 * can contain several PT entries.  For instance, the PTM entry
 * section for Keyed LTs has 6 PT entries (3 sets for each Key and Data
 * buffers).
 *
 * \retval Size, in words, of a PT entry element.
 */
extern uint32_t
bcmltm_wb_ptm_pt_entry_wsize(void);


/*!
 * \brief Get working buffer PTM parameter base offset for given PT op.
 *
 * This function gets the PTM parameter section base offset in the
 * working buffer for the given PTM operation index.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] op_idx PTM operation index.
 * \param [out] offset Returning PTM parameter base offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_param_base_offset_get(const bcmltm_wb_block_t *wb_block,
                                    uint32_t op_idx,
                                    uint32_t *offset);

/*!
 * \brief Get working buffer PTM entry base offset for given PT op.
 *
 * This function gets the PTM entry section base offset in the
 * working buffer for the given PTM operation index.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] op_idx PTM operation index.
 * \param [out] offset Returning PTM entry base offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_entry_base_offset_get(const bcmltm_wb_block_t *wb_block,
                                    uint32_t op_idx,
                                    uint32_t *offset);

/*!
 * \brief Get working buffer PTM parameter offsets.
 *
 * This function gets the PTM parameters working buffer offsets
 * for the given base offset.
 *
 * \param [in] base_offset Base offset of parameter section.
 * \param [out] offsets Returning PTM parameter offsets.
 *
 * \retval Size, in words, of PTM parameter section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_param_offsets_get(uint32_t base_offset,
                                bcmltm_wb_ptm_param_offsets_t *offsets);

/*!
 * \brief Get working buffer PTM entry offsets for Index table.
 *
 * This function gets the PTM entry buffer offsets on an Index table
 * for the given base offset.
 *
 * \param [in] base_offset Base offset of entry section.
 * \param [out] offsets Returning PTM entry offsets.
 *
 * \retval Size, in words, of PTM entry section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_entry_index_offsets_get(uint32_t base_offset,
                                      bcmltm_wb_ptm_entry_index_offsets_t
                                      *offsets);

/*!
 * \brief Get working buffer PTM entry offsets for Keyed PT Pass Thru table.
 *
 * This function gets the PTM entry buffer offsets on a PT Pass Thru Keyed
 * table for the given base offset.
 *
 * \param [in] base_offset Base offset of entry section.
 * \param [out] offsets Returning PTM entry offsets.
 *
 * \retval Size, in words, of PTM entry section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_entry_keyed_pthru_offsets_get(uint32_t base_offset,
                          bcmltm_wb_ptm_entry_keyed_pthru_offsets_t *offsets);

/*!
 * \brief Get working buffer PTM entry offsets for Keyed LT.
 *
 * This function gets the PTM entry buffer offsets on a LT Keyed
 * table for the given base offset.
 *
 * \param [in] base_offset Base offset of entry section.
 * \param [out] offsets Returning PTM entry offsets.
 *
 * \retval Size, in words, of PTM entry section for 1 PTM operation.
 */
extern uint32_t
bcmltm_wb_ptm_entry_keyed_lt_offsets_get(uint32_t base_offset,
                          bcmltm_wb_ptm_entry_keyed_lt_offsets_t *offsets);

/*!
 * \brief Get working buffer offset for given field for PT Pass Thru.
 *
 * This function gets the working buffer offset for the specified
 * PT field ID in given PTM working buffer block on PT Pass Thru.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] fid Physical field ID.
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_field_pthru_offset_get(const bcmltm_wb_block_t *wb_block,
                                     bcmdrd_fid_t fid,
                                     uint32_t *offset);

/*!
 * \brief Get working buffer offset for given field for Logical Table.
 *
 * This function gets the working buffer offset for the specified
 * PT field ID in given PTM working buffer block on logical tables.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] fid Physical field ID.
 * \param [in] field_type Indicates key or value field map type.
 *                        This is only relevant for the entry section of
 *                        a Keyed PTM block.
 * \param [in] field_dir Indicates input or output field direction,
 *                       This is only relevant for the entry section of
 *                       a Keyed PTM block on Key fields.
 * \param [in] pt_entry_idx PT entry index.
 *                          This index indicates the desired PT entry buffer
 *                          (note that this is not the HW PT entry INDEX).
 *                          An LT can map to multiple PT entries within
 *                          a given PT ID.  In this case, multiple
 *                          PT buffers are allocated for a PT ID.
 *                          - Index LT: indexes are compact, starting at 0.
 *                          - Keyed LT: indexes are always [0..3]
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_field_lt_offset_get(const bcmltm_wb_block_t *wb_block,
                                  uint32_t fid,
                                  bcmltm_field_type_t field_type,
                                  bcmltm_field_dir_t field_dir,
                                  uint16_t pt_entry_idx,
                                  uint32_t *offset);

/*!
 * \brief Get working buffer offset for PT SID override.
 *
 * This function gets the working buffer offset for the PT SID override
 * parameter in given PTM working buffer block.
 *
 * \param [in] wb_block Working buffer block.
 * \param [in] pt_entry_idx PT entry index.
 *                          This index indicates the desired PT entry buffer
 *                          (note that this is not the HW PT entry INDEX).
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_pt_sid_override_offset_get(const bcmltm_wb_block_t *wb_block,
                                         uint16_t pt_entry_idx,
                                         uint32_t *offset);

/*!
 * \brief Get working buffer offset for PT Entry Changed status.
 *
 * This function gets the working buffer offset for the PT Entry Changed
 * status parameter in given PTM working buffer block for
 * the given table type and field type.
 *
 * The PT changed status is not applicable for all table types or
 * field types (Key/Value).  If the parameter is not applicable,
 * the returning offset is set to INVALID.
 *
 * \param [in] table_type Table type.
 * \param [in] field_type Indicates key or value field type.
 * \param [in] wb_block Working buffer block.
 * \param [in] pt_entry_idx PT entry index.
 *                          This index indicates the desired PT entry buffer
 *                          (note that this is not the HW PT entry INDEX).
 * \param [out] offset Returning working buffer offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_pt_changed_offset_get(bcmltm_table_type_t table_type,
                                    bcmltm_field_type_t field_type,
                                    const bcmltm_wb_block_t *wb_block,
                                    uint16_t pt_entry_idx,
                                    uint32_t *offset);

/*!
 * \brief Indicate if PT Entry Changed flag is valid for the table type.
 *
 * This function checks if the PT Entry Changed opcode flag
 * BCMLTM_PT_OP_FLAGS_PT_CHANGED is applicable for the given table type.
 *
 * \param [in] table_type Table type.
 *
 * \retval TRUE if flag is applicable.
 * \retval FALSE if flag is not applicable.
 */
extern bool
bcmltm_wb_ptm_pt_changed_flag_valid(bcmltm_table_type_t table_type);

/*!
 * \brief Get the PTM entry key set index for given PTM operation and field.
 *
 * This function gets the index for the corresponding PTM Keyed entry
 * set assigned to the given PTM operation and field type,
 * Key/Data, Input/Output.
 *
 * \param [in] ptm_op PTM operation.
 * \param [in] field_type Indicates key or value field map type.
 * \param [in] field_dir Indicates input or output field direction,
 *                       (relevant only for Keyed PTM block on Key fields).
 * \param [out] set_idx Returning index for PTM keyed set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_wb_ptm_entry_keyed_idx_get(bcmptm_op_type_t ptm_op,
                                  bcmltm_field_type_t field_type,
                                  bcmltm_field_dir_t field_dir,
                                  uint32_t *set_idx);

#endif /* BCMLTM_WB_PTM_INTERNAL_H */
