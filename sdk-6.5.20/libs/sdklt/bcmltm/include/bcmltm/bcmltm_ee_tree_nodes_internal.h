/*! \file bcmltm_ee_tree_nodes_internal.h
 *
 * Logical Table Manager Binary Trees.
 * Execution Engine Nodes.
 *
 * A set of functions and definitions forming the nodes of
 * LTM EE binary trees.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_EE_TREE_NODES_INTERNAL_H
#define BCMLTM_EE_TREE_NODES_INTERNAL_H

#include <bcmbd/bcmbd.h>
#include <bcmltm/bcmltm_md_internal.h>
#include <bcmltm/bcmltm_wb_ptm_internal.h>

/*!
 * Indexed and PT PassThru table Metadata from node cookie
 */

/*!
 * \brief Retrieve pointer to PT List structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_PT_LIST(_nc_) \
    ((bcmltm_pt_list_t *)(_nc_))

/*!
 * \brief Number of Physical Tables.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_PT_LIST_NUM(_nc_) \
    (((bcmltm_pt_list_t *)(_nc_))->num_pt_view)


/*!
 * \brief Retrieve pointer to Memory Arguments structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 * \param [in] \_view\_ Index number of PT, in range 0...(num_pt_view -1).
 */
#define BCMLTM_PT_LIST_MEM_ARGS(_nc_, _view_) \
    ((((bcmltm_pt_list_t *)(_nc_))->mem_args)[(_view_)])

/*!
 * \brief Retrieve pointer to Memory Operations structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 * \param [in] \_view\_ Index number of PT, in range 0...(num_pt_view -1).
 */
#define BCMLTM_PT_LIST_OPS_LIST(_nc_, _view_) \
    ((((bcmltm_pt_list_t *)(_nc_))->memop_lists)[(_view_)])


/*!
 * Keyed table Metadata from node cookie
 */

/*!
 * \brief Retrieve pointer to Memory Arguments structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_KEYED_OP_INFO(_nc_) ((bcmltm_pt_keyed_op_info_t *)(_nc_))


/*!
 * \brief Retrieve pointer to EE index information from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_EE_INDEX_INFO(_nc_) \
    ((bcmltm_ee_index_info_t *)(_nc_))


/*!
 * \brief Extract field spec list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_EE_FIELD_SPEC_LIST(_nc_) \
    ((bcmltm_field_spec_list_t *)(_nc_))


/*!
 * LTA special handler table Metadata from node cookie
 */

/*!
 * \brief Retrieve pointer to LTA table parameters structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_LTA_TABLE_PARAMS(_nc_) \
    ((bcmltm_lta_table_params_t *)(_nc_))

/*!
 * \brief The traverse cookie for EE stage operations.
 */
typedef struct bcmltm_ee_traverse_info_s {
    /*! Pointer to the metadata for this LT. */
    bcmltm_lt_md_t *lt_md;

    /*! Pointer to the runtime state for this LT. */
    bcmltm_lt_state_t *lt_state;

    /*! Pointer to the working buffer for this entry. */
    uint32_t *ltm_buffer;
} bcmltm_ee_traverse_info_t;


/**
 * EE node support functions
 */

/*!
 * \brief Convert parameter offset list and LTM buffer to PTM dynamic info
 *
 * This function extracts the PTM parameters from the Working Buffer
 * based on the parsed parameter offsets and returns the information
 * in the dynamic info structure to be used by the PTM operation.
 *
 * \param [in] pt_flags BCMLTM_PT_OP_FLAGS_* to parse parameters.
 * \param [in] offsets The processed set of LTM buffer offsets that
 *                    indicate the location of each memory parameter.
 * \param [in] ltm_buffer Pointer to the LTM Working Buffer for this op.
 * \param [out] dyn_info The PTM dynamic information structure to be
 *                     filled out with the provided memory parameters.
 *
 * \retval None.
 */
extern void
bcmltm_pt_dyn_info_get(uint32_t pt_flags,
                       bcmltm_wb_ptm_param_offsets_t *offsets,
                       uint32_t *ltm_buffer,
                       bcmbd_pt_dyn_info_t *dyn_info);


/*******************************************************************************
 * PT PassThru functions
 */

/*!
 * \brief An EE tree node to perform SCHAN write operations with
 *        memory parameter flags.
 *
 * An EE tree node which dispatches a set of SCHAN writes to the PTM
 * using the entries already assembled in the LTM working buffer.
 *
 * This version of schan_write can handle different PT parameters in the
 * Working Buffer list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_schan_write(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie);

/*!
 * \brief An EE tree node to perform SCHAN read operations with
 *        memory parameter flags.
 *
 * An EE tree node which dispatches a set of SCHAN reads to the PTM
 * using the indexes listed in the LTM working buffer.  The read data
 * is stored in the LTM working buffer.
 *
 * This version of schan_read can handle different PT parameters in the
 * Working Buffer list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in,out] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_schan_read(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*******************************************************************************
 * PT hash table functions
 */

/*!
 * \brief An EE tree node to insert a hash table entry for the
 * INSERT  commands.
 *
 * An EE tree node which presents the separated key and data PT entry
 * arrays to the PTM for a PT insert operation.  This PT op may be used
 * as part of the INSERT opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_insert(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node to delete a hash table entry for the
 * DELETE command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT delete operation.  This PT op is used as part of the
 * DELETE opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_delete(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node to retrieve a hash table entry for the
 * LOOKUP or UPDATE commands.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This PT op is used as part of the
 * LOOKUP opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_lookup(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node to search for an existing hash table entry
 * for the INSERT command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This node expects the PT lookup
 * to return a not found status.  If the lookup succeeds, the status
 * is converted to an exists error.
 * This PT op is used as part of the INSERT opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No matching entry exists.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_pt_hash_lookup_not_found(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie);

/*******************************************************************************
 * LT Modeled functions
 */

/*******************************************************************************
 * PT PassThru functions
 */

/*!
 * \brief An EE tree node to perform modeled index write operations.
 *
 * An EE tree node which dispatches a set of modeled index writes
 * to the PTM using the entries already assembled in the LTM
 * working buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_write(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An EE tree node to perform modeled index read operations.
 *
 * An EE tree node which dispatches a set of modeled index reads to the PTM
 * using the indexes listed in the LTM working buffer.  The read data
 * is stored in the LTM working buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in,out] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_read(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*******************************************************************************
 * LT simple index table functions
 */

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * simple index table on an INSERT operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is available for a new modeled LT entry to be placed into a simple
 * index managed table.  The insert is rejected if an entry already
 * exists at this index.  Update should be used instead.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_insert(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * simple index table on a DELETE operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is in use with an entry in a simple index managed table.
 * The delete fails if no entry already exists at this index.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_delete(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to validate the index usage of a
 * simple index table on an UPDATE or LOOKUP operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is in use with an entry in a simple index managed table.
 * Fails if no entry already exists at this index.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_exists(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to validate the index availability of a
 * simple index table on an INSERT operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is not in use with an entry in a simple index managed table.
 * Fails if an entry already exists at this index.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_index_free(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * shared resource simple index table on an INSERT operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is available for a new modeled LT entry to be placed into a shared
 * resource simple index managed table.  The insert is rejected if an
 * entry already exists for this index in any of the LTs sharing this
 * device resource.
 *
 * This node updates the global resource in-use bitmap maintained by
 * the master LT of the resource group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_global_index_insert(int unit,
                                   bcmltm_lt_md_t *lt_md,
                                   bcmltm_lt_state_t *lt_state,
                                   bcmltm_entry_t *lt_entry,
                                   uint32_t *ltm_buffer,
                                   void *node_cookie);

/*!
 * \brief An EE tree node to validate and update the index usage of a
 * shared resource simple index table on a DELETE operation.
 *
 * An EE tree node which verifies whether the index of a simple index table
 * is in use by an entry in a shared resource simple index managed table.
 * The delete fails if no entry already exists at this index.
 *
 * This node updates the global resource in-use bitmap maintained by
 * the master LT of the resource group.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_global_index_delete(int unit,
                                   bcmltm_lt_md_t *lt_md,
                                   bcmltm_lt_state_t *lt_state,
                                   bcmltm_entry_t *lt_entry,
                                   uint32_t *ltm_buffer,
                                   void *node_cookie);


/*******************************************************************************
 * LT keyed table functions
 */

/*!
 * \brief An EE tree node to insert a keyed table entry for the
 * INSERT or UPDATE commands.
 *
 * An EE tree node which presents the separated key and data PT entry
 * arrays to the PTM for a PT insert operation.  This PT op may be used
 * as part of the INSERT or UPDATE opcodes.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_insert(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to delete a keyed table entry for the
 * DELETE command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT delete operation.  This PT op is used as part of the
 * DELETE opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_delete(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to retrieve a keyed table entry for the
 * LOOKUP or UPDATE commands.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This PT op is used as part of the
 * LOOKUP or UPDATE opcodes.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_lookup(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An EE tree node to search for an existing hash table entry
 * for the INSERT command.
 *
 * An EE tree node which presents the key PT entry arrays to the PTM
 * for a PT lookup operation.  This node expects the PT lookup
 * to return a not found status.  If the lookup succeeds, the status
 * is converted to an exists error.
 * This PT op is used as part of the INSERT opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_lookup_not_found(int unit,
                                            bcmltm_lt_md_t *lt_md,
                                            bcmltm_lt_state_t *lt_state,
                                            bcmltm_entry_t *lt_entry,
                                            uint32_t *ltm_buffer,
                                            void *node_cookie);

/*!
 * \brief An EE tree node to fetch the next keyed table entry for the
 * TRAVERSE command.
 *
 * An EE tree node which presents either the key PT entry arrays
 * to the PTM or a no key entry for a PT GET_NEXT or GET_FIRST
 * operation.  This PT op is used as part of the TRAVERSE opcode.
 *
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_model_keyed_traverse(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);



/*******************************************************************************
 * LTA dispatch nodes
 */

/*!
 * \brief An EE tree node wrapper for the LTA custom table insert interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface insert operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_insert(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table alloc insert intf.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface insert with allocation operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_alloc_insert(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table lookup interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface lookup operation.
 * The returned fields are appended to the LTM entry output fields list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_lookup(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table delete interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface delete operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_delete(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table update interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface update operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_update(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LTA custom table traverse interface.
 *
 * An EE tree node which adapts the LT entry information into the
 * format and parameters required for the LTA custom table handler
 * interface traverse operation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lta_traverse(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);


/*******************************************************************************
 * LT statistics nodes
 */

/*!
 * \brief An EE tree node wrapper for the LT TABLE_* writes.
 *
 * An EE tree node that updates LT TABLE_* values and stores
 * the values in an HA-protected resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lt_table_write(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An EE tree node wrapper for the LT TABLE_* read.
 *
 * An EE tree node that retrieves LT TABLE_* values from
 * an HA-protected resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry supplied for this
 *             LT operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry.
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_EXISTS Unexpected matching entry found.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_ee_node_lt_table_read(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

#endif /* BCMLTM_EE_TREE_NODES_INTERNAL_H */
