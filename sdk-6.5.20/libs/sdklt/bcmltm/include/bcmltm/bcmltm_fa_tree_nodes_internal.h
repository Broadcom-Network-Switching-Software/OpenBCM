/*! \file bcmltm_fa_tree_nodes_internal.h
 *
 * Logical Table Manager Binary Trees.
 * Field Adaptation Nodes.
 *
 * A set of functions and definitions forming the nodes of
 * LTM FA binary trees.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_FA_TREE_NODES_INTERNAL_H
#define BCMLTM_FA_TREE_NODES_INTERNAL_H

#include <bcmltm/bcmltm_md_internal.h>

/*!
 * \brief Extract select map field lists from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_FS_MAPPING(_nc_)   \
    ((bcmltm_field_select_mapping_t *)(_nc_))

/*!
 * \brief Extract a single field map list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 * \param [in] \_map\_index\_ Field select map index to choose field list.
 */
#define BCMLTM_FA_SELECT_FIELD_MAP_LIST(_nc_, _map_index_)   \
    (((bcmltm_field_select_mapping_t *)(_nc_))->field_map_list[_map_index_])


/*!
 * \brief Extract select map field lists from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_LTA_SELECT_MAPPING(_nc_)   \
    ((bcmltm_lta_select_mapping_t *)(_nc_))

/*!
 * \brief Extract a single field map list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 * \param [in] \_map\_index\_ Field select map index to choose field list.
 */
#define BCMLTM_FA_LTA_SELECT_FIELD_MAP_LIST(_nc_, _map_index_)   \
    (((bcmltm_lta_select_mapping_t *)(_nc_))->lta_select_field_list[_map_index_])

/*!
 * \brief Extract select map field lists from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_FS_MAPPING(_nc_)   \
    ((bcmltm_field_select_mapping_t *)(_nc_))

/*!
 * \brief Extract a single field map list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 * \param [in] \_map\_index\_ Field select map index to choose field list.
 */
#define BCMLTM_FA_SELECT_FIELD_MAP_LIST(_nc_, _map_index_)   \
    (((bcmltm_field_select_mapping_t *)(_nc_))->field_map_list[_map_index_])


/*!
 * \brief Extract a select field list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_LTA_SELECT_FIELD_LIST(_nc_)   \
    ((bcmltm_lta_select_field_list_t *)(_nc_))


/*!
 * \brief Extract fixed value field list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_FIXED_FIELD_LIST(_nc_) \
    ((bcmltm_fixed_field_list_t *)(_nc_))


/*!
 * \brief Extract fixed value field mapping from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_FIXED_FIELD_MAPPING(_nc_) \
    ((bcmltm_fixed_field_mapping_t *)(_nc_))


/*!
 * \brief Extract a single fixed field map list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 * \param [in] \_map\_index\_ Field select map index to choose field list.
 */
#define BCMLTM_FA_FIXED_FIELD_MAP_LIST(_nc_, _map_index_)   \
    (((bcmltm_fixed_field_mapping_t *)(_nc_))->fixed_field_list[_map_index_])


/*!
 * \brief Extract read-only fields list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_RO_FIELD_LIST(_nc_) \
    ((bcmltm_ro_field_list_t *)(_nc_))


/*!
 * \brief Extract wide fields list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_WIDE_FIELD_LIST(_nc_) \
    ((bcmltm_wide_field_list_t *)(_nc_))


/*!
 * \brief Extract WB copy list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_WB_COPY_LIST(_nc_) \
    ((bcmltm_wb_copy_list_t *)(_nc_))


/*!
 * \brief Extract LTM internal offsets from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_LTM(_nc_) \
    ((bcmltm_fa_ltm_t *)(_nc_))


/*!
 * \brief Extract a single field list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_SELECT_FIELD_MAP(_nc_)   \
    ((bcmltm_field_select_maps_t *)(_nc_))


/*!
 * \brief Extract Tracking Index params from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_TRACK_INDEX(_nc_) \
    ((bcmltm_track_index_t *)(_nc_))


/*!
 * \brief Extract lta field map list from node cookie.
 *
 * \param [in] \_nc\_ Pointer to the context information (node cookie)
 *                    for this operation.
 */
#define BCMLTM_FA_LTA_FIELD_LIST(_nc_) \
    ((bcmltm_lta_field_list_t *)(_nc_))


/*!
 * \brief Retrieve pointer to LTA transform parameters structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_LTA_TRANSFORM_PARAMS(_nc_) \
    ((bcmltm_lta_transform_params_t *)(_nc_))


/*!
 * \brief Retrieve pointer to LTA validate parameters structure.
 *
 * \param [in] \_nc\_ The void * pointer of the node cookie.
 */
#define BCMLTM_LTA_VALIDATE_PARAMS(_nc_) \
    ((bcmltm_lta_validate_params_t *)(_nc_))

/*!
 * \brief Existence requirements for input fields.
 *
 * Types of API field validation for copying values to the Working Buffer
 * or LTA.
 */
typedef enum bcmltm_fields_required_e {
    BCMLTM_FIELDS_REQUIRED_NONE,
    BCMLTM_FIELDS_REQUIRED_UNSET,
    BCMLTM_FIELDS_REQUIRED_DEFAULT,
    BCMLTM_FIELDS_REQUIRED_ALL,
    BCMLTM_FIELDS_REQUIRED_ALL_OR_NONE,
} bcmltm_fields_required_t;


/*!
 * \brief An FA tree node to copy API key fields into the Working Buffer.
 *
 * An FA tree node which copies the API key fields of an entry into
 * locations within the Working Buffer.
 * This function will return an error if any of the mapped fields are
 * missing from the API field list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_key_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);
/*!
 * \brief An FA tree node to copy API alloc key fields into the WB.
 *
 * An FA tree node which copies the API alloc key fields of an entry into
 * locations within the Working Buffer.
 * If no key fields are present, the keys absent flag is set in the WB.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_alloc_key_fields_to_wb(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to copy API value fields into the Working Buffer.
 *
 * An FA tree node which copies the API value fields of an entry into
 * locations within the Working Buffer.
 * This function will not return an error if any mapped fields are
 * missing from the API field list.  They are supplied by the default
 * value FA node later.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_value_fields_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to copy Working Buffer values to API fields.
 *
 * An FA tree node which copies values within the Working Buffer to
 * an API field list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wb_to_api_fields(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy Working Buffer keys
 *        to API fields.
 *
 * An FA tree node which conditionally copies index keys within the
 * Working Buffer to an API field list.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wb_to_api_alloc_fields(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to copy Working Buffer values to API key fields.
 *
 * An FA tree node which copies values within the Working Buffer to
 * an API key field list.
 * This node is used during TRAVERSE ops.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wb_to_api_key_fields(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);

/*!
 * \brief An FA tree node to place fixed field values into the WB.
 *
 * An FA tree node which writes fixed field values into the Working
 * Buffer.  These are most likely fixed physical field values --
 * such as key type, data type, or mode selection --
 * that are fixed for the specific LT.  But they may be arguments to
 * other logic that need to be placed into the WB, say inputs to the
 * flex counter specifying the type of resource.
 * The maximum value size of one element in the list is 32-bit, but
 * several entries may be added to produce a larger value.  This
 * is expected to be a very rare case, so it is not optimized.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_fixed_fields_to_wb(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to clear fixed field values in the WB.
 *
 * An FA tree node which writes fixed field values into the Working
 * Buffer.  These are most likely fixed physical field values --
 * such as key type, data type, or mode selection --
 * that are fixed for the specific LT.  But they may be arguments to
 * other logic that need to be placed into the WB, say inputs to the
 * flex counter specifying the type of resource.
 * The maximum value size of one element in the list is 32-bit, but
 * several entries may be added to produce a larger value.  This
 * is expected to be a very rare case, so it is not optimized.
 *
 * This function copies the default values for DELETE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_delete_fixed_fields_to_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to insert default field values into the WB
 *
 * An FA tree node which writes the default field values into the Working
 * Buffer.  The default values are supplied during metadata initialization
 * from the software defaults of the LT fields.
 *
 * If this becomes runtime adjustable state, the storage in metadata will
 * need adjustment.  For now, this function uses a node cookie
 * of bcmltm_field_select_mapping_t.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_default_fields_to_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);

/*!
 * \brief An FA tree node to revert to default field values into the WB
 *
 * An FA tree node which writes default field values into the Working
 * Buffer for selected fields during UPDATE ops.  If the
 * SHR_FMM_FIELD_DEL field flag is set for an input value field,
 * replace the current entry value(s) with the field default.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_unset_to_wb(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to copy a value in a specific Working Buffer
 * location to a different specific Working Buffer location.
 *
 * An FA tree node which extracts a value from a specific Working
 * Buffer location, then copies the value to a different
 * specific Working Buffer location.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_wbc(int unit,
                          bcmltm_lt_md_t *lt_md,
                          bcmltm_lt_state_t *lt_state,
                          bcmltm_entry_t *lt_entry,
                          uint32_t *ltm_buffer,
                          void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy a list of values in
 * specific Working Buffer locations to different specific
 * Working Buffer locations.
 *
 * An FA tree node which conditionally copies WB values based upon
 * the Index Absent flag used for Index with Allocation LT INSERT.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_wbc_to_wbc(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to reject any supplied read-only field values.
 *
 * An FA tree node which scans the LT entry input field list for
 * read-only value fields.  If such a value is found, return SHR_E_ACCESS.
 *
 * This node is applicable to INSERT and UPDATE ops.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_read_only_input_check(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to test the coherence of wide field values.
 *
 * An FA tree node which scans the LT entry input field list for
 * wide value fields (> 64 bits).  If such a value is found,
 * verify that either all or none of the field elements for that field
 * are present.  If not, return SHR_E_PARAM.
 *
 * This node is applicable to INSERT and UPDATE ops.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wide_field_check(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to determine the select field map index.
 *
 * An FA tree node which examines the value of a selector field
 * then places the corresponding map index into the WB location
 * reserved for that selection.
 * If the WB offset for the parent map index is INVALID, then
 * this selector field is top level.  It is active unconditionally.
 * Otherwise, the parent select map index must match the value
 * provided in the bcmltm_field_select_map_t structure for this
 * select map to be used.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_select_field_map(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to determine the select field from a map index.
 *
 * An FA tree node which examines the value of a select field map index
 * then places the corresponding select field value into the WB location
 * of that field.
 *
 * This node is explicitly for the case of IwA INSERT and TRAVERSE when
 * the Track Index includes a component for PT subentry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_select_field_unmap(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to insert default fields to LTA field arrays.
 *
 * An FA tree node which inserts default fields to an array of
 * Logical Table Adapter fields.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_default_fields_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to convert API key fields to LTA field arrays.
 *
 * An FA tree node which converts the API key fields to an array of
 * Logical Table Adapter fields.
 * This node is used for key fields only, and requires all API
 * fields in the metadata list are present.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_key_fields_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to convert API alloc key fields to
 *        LTA field arrays.
 *
 * An FA tree node which converts the API alloc key fields to an array of
 * Logical Table Adapter fields.
 * If no key fields are present, the keys absent flag is set in the WB.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_alloc_key_fields_to_lta(int unit,
                                           bcmltm_lt_md_t *lt_md,
                                           bcmltm_lt_state_t *lt_state,
                                           bcmltm_entry_t *lt_entry,
                                           uint32_t *ltm_buffer,
                                           void *node_cookie);

/*!
 * \brief An FA tree node to convert API value fields to LTA field arrays.
 *
 * An FA tree node which converts the API value fields to an array of
 * Logical Table Adapter fields.
 * This node is used for value fields only, so not all API
 * fields in the metadata list must be present.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_value_fields_to_lta(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie);

/*!
 * \brief An FA tree node to convert API fields or default values
 * to LTA field arrays.
 *
 * An FA tree node which converts the API fields to an array of
 * Logical Table Adapter fields.  If an API field is not present
 * in the input list, the default value is added to the LTA list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_default_to_lta(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to update API fields values within an
 * existing LTA field array.
 *
 * An FA tree node which transfers the API fields provided to an
 * existing array of Logical Table Adapter fields.  The API field ids
 * must be present already in the LTA field list.
 * This operation is required to perform the read-modify-write step
 * of an UPDATE opcode.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_update_to_lta(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie);

/*!
 * \brief An FA tree node to copy CTH API fields values within an
 * existing LTA field array.
 *
 * An FA tree node which transfers the API fields provided to an
 * existing array of Logical Table Adapter fields.  The API field ids
 * might be present already in the LTA field list.
 * This function is used for INSERT and UPDATE CTH operations.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_cth_to_lta(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to place CTH API fields in an
 * existing LTA field array.
 *
 * An FA tree node which fills out the field pointers of an
 * existing array of Logical Table Adapter fields.  The API field ids
 * must not be present already in the LTA field list.  If the API field
 * is unmodified, the API field structure is used directly.
 * If the UNSET flag is set for an UPDATE, the API field is copied
 * into the Working Buffer so the data value may be overwritten.
 * This function is used for INSERT and UPDATE CTH operations.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_fields_cth_to_lta_expedited(int unit,
                                               bcmltm_lt_md_t *lt_md,
                                               bcmltm_lt_state_t *lt_state,
                                               bcmltm_entry_t *lt_entry,
                                               uint32_t *ltm_buffer,
                                               void *node_cookie);

/*!
 * \brief An FA tree node to convert an LTA field array to API fields.
 *
 * An FA tree node which converts an array of Logical Table Adapter
 * fields into API fields of an entry.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_to_api_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to conditionally convert an LTA field array
 *        to API fields.
 *
 * An FA tree node which conditionally converts an array of
 * Logical Table Adapter fields into API fields of an entry.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_to_api_alloc_fields(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie);

/*!
 * \brief An FA tree node to prepare an input LTA field list.
 *
 * An FA tree node which connects the Working Buffer locations
 * reserved for the input field list of an LTA function.
 * The parameter array structure is initialized, and the count of
 * fields is reset.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_input_init(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to prepare an output LTA field list.
 *
 * An FA tree node which connects the Working Buffer locations
 * reserved for the output field list of an LTA function.
 * The parameter structure is configured with a number of elements
 * for the maximum number of output values, and the field value pointer
 * array is filled in.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_output_init(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie);

/*!
 * \brief An FA tree node to copy an LTA field array element to
 * another LTA field structure.
 *
 * An FA tree node which searches for field array elements in
 * a Logical Table Adapter fields list and appends them to a different
 * LTA fields list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to copy an LTA field array element to
 * a specific Working Buffer location.
 *
 * An FA tree node which searches for field array elements in
 * a Logical Table Adapter fields list, then copies the field value
 * to a specific Working Buffer location.  The location is generally
 * a PTM entry or memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_fields_to_wbc(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to copy an a specific Working Buffer location
 * to a LTA field array element.
 *
 * An FA tree node which transcribes a specific Working Buffer location
 * to a LTA field structure within the WB, then appends the structure
 * to a Logical Table Adapter fields list.  The WB location is generally
 * a PTM entry or memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_lta_fields(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Transform function.
 *
 * An FA tree node which passes the LTA input and output parameters
 * constructed by other FA nodes to a Logical Table Adapter Transform
 * function.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_transform(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Extended Transform function.
 *
 * An FA tree node which passes the LTA input, input_key and output parameters
 * constructed by other FA nodes to a Logical Table Adapter Transform
 * function.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_ext_transform(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Transform function during
 *        TRAVERSE operations.
 *
 * An FA tree node which passes the LTA input and output parameters
 * constructed by other FA nodes to a Logical Table Adapter Transform
 * function.  This node is specifically for forward key transforms
 * during TRAVERSE.  It checks if the operation is TRAVERSE_START, and
 * skips the LTA Transform if so due to the absence of any key fields
 * on the first call of a table TRAVERSE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_traverse_transform(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Transform function on the
 *        second pass of IwA LTs.
 *
 * An FA tree node which passes the LTA input and output parameters
 * constructed by other FA nodes to a Logical Table Adapter Transform
 * function.
 * This function is used for the second forward key transform of
 * Index with Allocation INSERT.  The standard transform node, which
 * invokes the transform conditionally based on the transform
 * suppress flag not set, is sufficient for the first pass of IwA INSERT.
 * The second pass must invert the sense of the suppress flag, so this
 * node indicates this situation.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_alloc_second_transform(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to convert the PT select of a LTA Transform.
 *
 * An FA tree node which retrieves the PT select index produced by
 * a LTA Transform operation where the the PT is determined by the
 * LT key fields, converts it to the PT SID required by the PTM,
 * and places it in the Working Buffer location for that PT op.
 *
 * If the PT SID is needed by multiple PT ops, then the WB to WB copy
 * node should be used to propagate the PT SID value.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_pt_sid_map(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to invoke a LTA Validate function.
 *
 * An FA tree node which passes the LTA input parameters
 * constructed by other FA nodes to a Logical Table Adapter
 * Validate function.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_validate(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);

/*!
 * \brief An FA tree node to determine the Tracking Index.
 *
 * An FA tree node which calculates the Tracking Index from values
 * already present in the Working Buffer.
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
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_track(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);

/*!
 * \brief An FA tree node to decompose the Tracking Index.
 *
 * An FA tree node which separates the Tracking Index into values
 * then placed in the Working Buffer.  This reverses the process
 * in bcmltm_fa_node_wbc_to_track which assembles different
 * physical parameters into a single Tracking Index.
 *
 * This function is used for TRAVERSE where the key fields are returned
 * via the LT entry output.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_track_to_wbc(int unit,
                            bcmltm_lt_md_t *lt_md,
                            bcmltm_lt_state_t *lt_state,
                            bcmltm_entry_t *lt_entry,
                            uint32_t *ltm_buffer,
                            void *node_cookie);

/*!
 * \brief An FA tree node to conditionally decompose the Tracking Index.
 *
 * An FA tree node which conditionally separates the Tracking Index
 * into values for the allocated components and places them in the
 * Working Buffer.
 *
 * This function is used for INSERT into Index with Allocation LTs
 * where the allocatable key fields are not provided in the LT entry input.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_track_to_wbc(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to determine the next Tracking Index.
 *
 * An FA tree node which finds the next in-use LT Tracking Index from the
 * provided Tracking Index of an existing traverse operation, or the first
 * Tracking Index when starting a traverse.
 *
 * If an in-use Tracking Index is found, bcmltm_fa_node_track_to_wbc
 * should be employed to copy the device parameters into the proper
 * locations in the Working Buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No further in-use index.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_traverse_track(int unit,
                              bcmltm_lt_md_t *lt_md,
                              bcmltm_lt_state_t *lt_state,
                              bcmltm_entry_t *lt_entry,
                              uint32_t *ltm_buffer,
                              void *node_cookie);

/*!
 * \brief An FA tree node to determine Tracking index for w/Allocation LTs.
 *
 * An FA tree node which calculates the Tracking index, from the input API
 * fields provided.  If allocatable key fields are not present, a free
 * Tracking Index is determined and returned at completion of the INSERT
 * operation.  This function is only applicable for Index w/Allocation
 * Logical Tables.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_track(int unit,
                           bcmltm_lt_md_t *lt_md,
                           bcmltm_lt_state_t *lt_state,
                           bcmltm_entry_t *lt_entry,
                           uint32_t *ltm_buffer,
                           void *node_cookie);

/*!
 * \brief An FA tree node to determine the Tracking Index for
 *        w/Allocation LTs using shared resources.
 *
 * An FA tree node which calculates the Tracking Index, from the input API
 * fields provided.  If allocatable key fields are not present, a free
 * Tracking Index is determined and returned at completion of the INSERT
 * operation.
 * This function is only applicable for Index w/Allocation
 * Logical Tables.
 * This function is only applicable to shared resource IwA LTs.  It
 * employs the global inuse bitmap of the master LT for this
 * shared resource.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_global_alloc_track(int unit,
                                  bcmltm_lt_md_t *lt_md,
                                  bcmltm_lt_state_t *lt_state,
                                  bcmltm_entry_t *lt_entry,
                                  uint32_t *ltm_buffer,
                                  void *node_cookie);

/*!
 * \brief An FA tree node to determine the next LT ID.
 *
 * An FA tree node which finds the next LT ID from the
 * provided LT ID of an existing traverse operation on one of the
 * TABLE_* LTs which use the LT ID itself as the key.  Will determine
 * the first LT ID when starting a traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No further LT ID.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_traverse_ltid(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

/*!
 * \brief An FA tree node to clear the Keyed data buffer.
 *
 * An FA tree node that erases the lookup data buffer of a Keyed LT.
 * This permits the same data buffer to be reused within the WB,
 * so per-field Working Buffer coordinates are unchanged between the
 * WB to API lookup and the API to WB write during UPDATE.
 *
 * Note that the expected node cookie is that for the PT insert
 * portion of UPDATE, in Pass 1 EE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_NOT_FOUND No further LT ID.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_keyed_data_clear(int unit,
                                bcmltm_lt_md_t *lt_md,
                                bcmltm_lt_state_t *lt_state,
                                bcmltm_entry_t *lt_entry,
                                uint32_t *ltm_buffer,
                                void *node_cookie);

/*!
 * \brief An FA tree node to copy key fields into the Working Buffer.
 *
 * An FA tree node which copies the LTM entry key fields into
 * locations within the Working Buffer API field cache.
 * This function will return an error if any of the mapped fields are
 * missing from the API field list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_in_key_fields_to_api_wb(int unit,
                                       bcmltm_lt_md_t *lt_md,
                                       bcmltm_lt_state_t *lt_state,
                                       bcmltm_entry_t *lt_entry,
                                       uint32_t *ltm_buffer,
                                       void *node_cookie);

/*!
 * \brief An FA tree node to copy alloc key fields into the Working Buffer.
 *
 * An FA tree node which copies the LTM entry alloc key fields into
 * locations within the Working Buffer API field cache.
 * If no key fields are present, the keys absent flag is set in the WB.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_in_alloc_key_fields_to_api_wb(int unit,
                                             bcmltm_lt_md_t *lt_md,
                                             bcmltm_lt_state_t *lt_state,
                                             bcmltm_entry_t *lt_entry,
                                             uint32_t *ltm_buffer,
                                             void *node_cookie);

/*!
 * \brief An FA tree node to copy value fields into the Working Buffer.
 *
 * An FA tree node which copies the LTM entry value fields into
 * locations within the Working Buffer API field cache.
 * This function will not return an error if any mapped fields are
 * missing from the API field list.  They are supplied by the default
 * value FA node later.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_in_value_fields_to_api_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to revert to default fields into the Working Buffer.
 *
 * An FA tree node which copies the LTM entry value fields into
 * locations within the Working Buffer API field cache during UPDATE ops.
 * If the SHR_FMM_FIELD_DEL field flag is set for an input value field,
 * supply the field default to the API field cache.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_in_fields_unset_to_api_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to copy API cache values to LTM entry output.
 *
 * An FA tree node which copies values within the Working Buffer
 * API field cache to the LTM entry out fields list.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_wb_to_out_fields(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);

/*!
 * \brief An FA tree node to initialize API cache values to defaults.
 *
 * An FA tree node which creates a set of field structures within
 * Working Buffer's API cache with all field values set to default.
 * This function is intended the beginning of value field processing
 * during UPDATE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_wb_defaults(int unit,
                               bcmltm_lt_md_t *lt_md,
                               bcmltm_lt_state_t *lt_state,
                               bcmltm_entry_t *lt_entry,
                               uint32_t *ltm_buffer,
                               void *node_cookie);

/*!
 * \brief An FA tree node to reset PT entry fields to default.
 *
 * An FA tree node which copies the DELETE values for PT entries
 * into the Working Buffer.
 * Used for DELETE to reset the PT entries, and for UPDATE to clear
 * the PT entries before the modified API fields are copied.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_pt_clear(int unit,
                        bcmltm_lt_md_t *lt_md,
                        bcmltm_lt_state_t *lt_state,
                        bcmltm_entry_t *lt_entry,
                        uint32_t *ltm_buffer,
                        void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy API cache keys
 *        to LTM entry output.
 *
 * An FA tree node which conditionally copies index keys within the
 * Working Buffer API field cache to the LTM entry out fields list.
 * This function is for Index w/Allocation INSERT only.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_wb_to_out_alloc_fields(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to copy API cache keys to LTM entry output.
 *
 * An FA tree node which copies index keys within the
 * Working Buffer API field cache to the LTM entry out fields list.
 * This node is used during TRAVERSE ops.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_wb_to_out_key_fields(int unit,
                                        bcmltm_lt_md_t *lt_md,
                                        bcmltm_lt_state_t *lt_state,
                                        bcmltm_entry_t *lt_entry,
                                        uint32_t *ltm_buffer,
                                        void *node_cookie);

/*!
 * \brief An FA tree node to copy cached API values within the Working Buffer.
 *
 * An FA tree node which copies Working Buffer API field cache values
 * within the Working Buffer to some other location - PT entry, IMM entry,
 * memory parameter, other WB internal location.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_api_wb_to_wbc(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

/*!
 * \brief An FA tree node to conditionaly copy cached API values.
 *
 * An FA tree node which copies Working Buffer API field cache values
 * within the Working Buffer to some other location - PT entry, IMM entry,
 * memory parameter, other WB internal location.
 *
 * This node is provided for the first index with allocation copy,
 * before the Track Index is calculated.  It will skip copying the
 * fields if the Index Absent offset is set.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_first_api_wb_to_wbc(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to conditionaly copy cached API values.
 *
 * An FA tree node which copies Working Buffer API field cache values
 * within the Working Buffer to some other location - PT entry, IMM entry,
 * memory parameter, other WB internal location.
 *
 * This node is provided for the second index with allocation copy,
 * after the allocated Track Index is calculated, decomposed to PT params,
 * and those params converted back to the API cache.  It will copy
 * the fields only if the Index Absent offset is set.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_alloc_second_api_wb_to_wbc(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to conditionaly copy cached API values.
 *
 * An FA tree node which copies Working Buffer API field cache values
 * within the Working Buffer to some other location - PT entry, IMM entry,
 * memory parameter, other WB internal location.
 *
 * This node is provided for TRAVERSE operations, which omit any
 * API keys for the first op in a sequence.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_key_traverse_api_wb_to_wbc(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to copy Working Buffer values into API field cache.
 *
 * An FA tree node which copies various elements within the Working Buffer
 * to the Working Buffer API field cache.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_wbc_to_api_wb(int unit,
                             bcmltm_lt_md_t *lt_md,
                             bcmltm_lt_state_t *lt_state,
                             bcmltm_entry_t *lt_entry,
                             uint32_t *ltm_buffer,
                             void *node_cookie);

/*!
 * \brief An FA tree node to place selected fixed field values into the WB.
 *
 * An FA tree node which writes fixed field values chosen by a
 * field select map into the Working Buffer.
 * These are most likely fixed physical field values --
 * such as key type, data type, or mode selection --
 * that are fixed for the specific LT.  But they may be arguments to
 * other logic that need to be placed into the WB, say inputs to the
 * flex counter specifying the type of resource.
 * The maximum value size of one element in the list is 32-bit, but
 * several entries may be added to produce a larger value.  This
 * is expected to be a very rare case, so it is not optimized.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_select_fixed_fields_to_wb(int unit,
                                         bcmltm_lt_md_t *lt_md,
                                         bcmltm_lt_state_t *lt_state,
                                         bcmltm_entry_t *lt_entry,
                                         uint32_t *ltm_buffer,
                                         void *node_cookie);

/*!
 * \brief An FA tree node to clear selected fixed field values in the WB.
 *
 * An FA tree node which writes fixed field values chosen by a
 * field select map into the Working Buffer.
 * These are most likely fixed physical field values --
 * such as key type, data type, or mode selection --
 * that are fixed for the specific LT.  But they may be arguments to
 * other logic that need to be placed into the WB, say inputs to the
 * flex counter specifying the type of resource.
 * The maximum value size of one element in the list is 32-bit, but
 * several entries may be added to produce a larger value.  This
 * is expected to be a very rare case, so it is not optimized.
 *
 * This function copies the default values for DELETE.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_delete_select_fixed_fields_to_wb(int unit,
                                                bcmltm_lt_md_t *lt_md,
                                                bcmltm_lt_state_t *lt_state,
                                                bcmltm_entry_t *lt_entry,
                                                uint32_t *ltm_buffer,
                                                void *node_cookie);

/*!
 * \brief An FA tree node to prepare an input LTA field list.
 *
 * An FA tree node which connects the Working Buffer locations
 * reserved for the input field list of an LTA function.
 * The parameter array structure is initialized, and the count of
 * fields is reset.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_select_input_init(int unit,
                                     bcmltm_lt_md_t *lt_md,
                                     bcmltm_lt_state_t *lt_state,
                                     bcmltm_entry_t *lt_entry,
                                     uint32_t *ltm_buffer,
                                     void *node_cookie);

/*!
 * \brief An FA tree node to prepare an output LTA field list.
 *
 * An FA tree node which connects the Working Buffer locations
 * reserved for the output field list of an LTA function.
 * The parameter structure is configured with a number of elements
 * for the maximum number of output values, and the field value pointer
 * array is filled in.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_lta_select_output_init(int unit,
                                      bcmltm_lt_md_t *lt_md,
                                      bcmltm_lt_state_t *lt_state,
                                      bcmltm_entry_t *lt_entry,
                                      uint32_t *ltm_buffer,
                                      void *node_cookie);

/*!
 * \brief An FA tree node to append field structures from API cache
 *        to LTA field array pointers.
 *
 * An FA tree node which fills the LTA field pointer array with the
 * elements of the API cache employed for field select LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_fa_node_select_api_wb_to_lta(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);

/*!
 * \brief An FA tree node to copy field structures from LTA field
 *        array pointers to the API cache.
 *
 * An FA tree node which scans an LTA field pointer array for fields
 * which should be copied to the API cache in field select LTs.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_fa_node_select_lta_to_api_wb(int unit,
                                    bcmltm_lt_md_t *lt_md,
                                    bcmltm_lt_state_t *lt_state,
                                    bcmltm_entry_t *lt_entry,
                                    uint32_t *ltm_buffer,
                                    void *node_cookie);

/*!
 * \brief An FA tree node to copy allocated key field structures
 *        from LTA field array pointers to the API cache.
 *
 * An FA tree node which scans an LTA field pointer array for fields
 * which should be copied to the API cache in field select LTs.
 * This function is for copying allocated key fields from a reverse
 * key transform during INSERT operations to Logical Tables with
 * field selection.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmltm_fa_node_select_alloc_lta_to_api_wb(int unit,
                                          bcmltm_lt_md_t *lt_md,
                                          bcmltm_lt_state_t *lt_state,
                                          bcmltm_entry_t *lt_entry,
                                          uint32_t *ltm_buffer,
                                          void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy an LTA field array
 * element to a specific Working Buffer location.
 *
 * An FA tree node which first verifies a given field select list
 * is active, searches for field array elements in
 * a Logical Table Adapter fields list, and then copies the field value
 * to a specific Working Buffer location.  The location is generally
 * a PTM entry or memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_select_lta_to_wbc(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

/*!
 * \brief An FA tree node to conditionally copy an a specific Working
 * Buffer location to a LTA field array element.
 *
 * An FA tree node which first verifies a given field select list
 * is active, transcribes a specific Working Buffer location
 * to a LTA field structure within the WB, and then appends the structure
 * to a Logical Table Adapter fields list.  The WB location is generally
 * a PTM entry or memory parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_md Pointer to the metadata for this LT.
 * \param [in] lt_state Pointer to the state for this LT.
 * \param [in] lt_entry Pointer to the API entry for this operation.
 * \param [in] ltm_buffer Pointer to the working buffer for this entry
 * \param [in] node_cookie Pointer to the context information for this
 *             operation.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmltm_fa_node_select_wbc_to_lta(int unit,
                                 bcmltm_lt_md_t *lt_md,
                                 bcmltm_lt_state_t *lt_state,
                                 bcmltm_entry_t *lt_entry,
                                 uint32_t *ltm_buffer,
                                 void *node_cookie);

#endif /* BCMLTM_FA_TREE_NODES_INTERNAL_H */
