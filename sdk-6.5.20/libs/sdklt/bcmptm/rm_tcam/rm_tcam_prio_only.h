/*! \file rm_tcam_prio_only.h
 *
 * Utils, defines internal to RM-TCAM-PRIO-ONLY TCAM
 * This file contains utils, defines which are internal to
 * TCAM resource manager(i.e. these are not visible outside RM-TCAM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_PRIO_ONLY_H
#define RM_TCAM_PRIO_ONLY_H

#include <bcmptm/generated/bcmptm_rm_tcam_prio_only_ha.h>

/*******************************************************************************
 * Typedefs
 */
#define BCMPTM_RM_TCAM_SKIP_RSVD_IDX_CHECK 0x1

#define BCMPTM_RM_TCAM_SKIP_RESERVED_ERROR_CHECK 0x4

#define BCMPTM_RM_TCAM_KEY0 (1 << 0)

#define BCMPTM_RM_TCAM_SHR_KEY0 (1 << 1)

#define BCMPTM_RM_TCAM_BOTH_KEY_KEY0 \
        (BCMPTM_RM_TCAM_KEY0 | BCMPTM_RM_TCAM_SHR_KEY0)

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Calculate the total memory required for SW state of
   \n all TCAM LTIDs together.
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] user_data User Data
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL initialization issues
 */
extern int
bcmptm_rm_tcam_prio_only_calc_mem_required(int unit,
                                           bcmltd_sid_t ltid,
                                           bcmptm_rm_tcam_lt_info_t *ltid_info,
                                           void *user_data);

/*!
 * \brief Update the SW metadata required for all LTIDs in
 * \n memory created in pass one.
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] start_ptr Starting position in metadata.
 * \param [in] offset Final offset for the data.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL initialization issues
 */
extern int
bcmptm_rm_tcam_prio_only_init_metadata(int unit,
                                       bool warm,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       void *offset);

/*!
 * \brief Insert the new TCAM entry info or Update the existing TCAM entry in
 * \n priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FULL No Resources
 * \retval SHR_E_EXISTS Requested entry_id already exist
 */
extern int
bcmptm_rm_tcam_prio_only_entry_insert(int unit,
                                      bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Delete the existing TCAM entry in priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details

 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_prio_only_entry_delete(int unit,
                                     uint64_t req_flags,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                                     bcmptm_rm_tcam_req_t *req_info,
                                     bcmptm_rm_tcam_rsp_t *rsp_info,
                                     bcmltd_sid_t *rsp_ltid,
                                     uint32_t *rsp_flags,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Read TCAM entry key words(key + mask) and data words for
 * \n the given TCAM entry ID for the priority based TCAMs.
 * \n By default data is read from PTCache for cacheable PTs..
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior
               (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details

 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [out] index TCAM index of the entry_id in req_info if entry_id exists
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_prio_only_entry_lookup(int unit,
                                     uint64_t req_flags,
                                     bcmltd_sid_t ltid,
                                     bcmptm_rm_tcam_lt_info_t *ltid_info,
                                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                                     bcmptm_rm_tcam_req_t *req_info,
                                     bcmptm_rm_tcam_rsp_t *rsp_info,
                                     bcmltd_sid_t *rsp_ltid,
                                     uint32_t *rsp_flags,
                                     uint32_t *index,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Fetch the address to TCAM entry information of the given LTID.
 * \n This address points to address of zeroth TCAM index entry information.
 * \n To get the address to TCAM entry information of any specific TCAM index,
 * \n increment this pointer by those many TCAM indexes as memory created to
 * \n hold TCAM entry information of all TCAM indexes is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [out] entry_info double pointer to hold pointer to
 * \n           start address of TCAM entry information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_only_entry_info_get(int unit,
                         bcmltd_sid_t ltid,
                         bcmptm_rm_tcam_lt_info_t *ltid_info,
                         int pipe_id,
                         bcmptm_rm_tcam_prio_only_entry_info_t **entry_info);


/*!
 * \brief Move the TCAM entry from one index to the other in priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] from_idx TCAM index from which entry has to be moved.
 * \param [in] to_idx TCAM index to which entry has to be moved.
 * \param [in] entry_attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_prio_only_entry_move(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    uint8_t partition,
                                    uint32_t prefix,
                                    uint32_t from_idx,
                                    uint32_t to_idx,
                                    void *attrs,
                                    bcmbd_pt_dyn_info_t *pt_dyn_info,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Write the entry words to a particulat TCAM index.
 *  Updates both HW and SW state
 *
 * \param [in] unit    Logical device id
 * \param [in] iomd    IN/OUT metadata to/from RM-TCAM
 * \param [in] hw_only Write entry to hardware without touching
 *                     the software state.
 * \param [in] index   TCAM index at which entry has to be written
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_only_entry_write(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t hw_only,
                                     int index);
/*!
 * \brief Read the entry words from a particular TCAM index.
 *
 * \param [in] unit  Logical device id
 * \param [in] iomd  IN/OUT metadata to/from RM-TCAM
 * \param [in] index TCAM index to read from
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_only_entry_read(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd,
                                    int index);


/*!
 * \brief Clear(writing all zeros) the TCAM entry at a given index.
 *
 * \param [in] unit               Logical device id
 * \param [in] iomd               IN/OUT metadata to/from RM-TCAM
 * \param [in] entry_update_flags To indicate that the change is needed
 *                                only in HW or SW state.
 * \param [in] index              TCAM index at which entry has to be written
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_only_entry_clear(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t entry_update_flags,
                                     uint32_t index);


/*!
 * \brief Find the index for the new TCAM entry info priority based TCAMs.
 *
 * \param [in] unit          Logical device id
 * \param [in] iomd          IN/OUT metadata to/from RM-TCAM
 * \param [in] prio_change   To identify if its a priority change
 *                           of existing entry
 * \param [out] target_index Index where the TCAM insert is calculated.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FULL No Resources
 */
extern int
bcmptm_rm_tcam_prio_only_entry_index_allocate(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd,
                                     uint8_t prio_change,
                                     uint32_t *target_index,
                                     uint8_t *reserve_used,
                                     bool *shared_reorder);

/*!
 * \brief Reserve on entry for a particular LTID priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id pipe no
 * \param [in] tcam_info TCAM information from HA area for this LTID
 * \param [in] index Index to be reserved.
 * \param [in] entry type.
 * \param [in] Lookup ID.
 * \param [in] attrs Entry attributes pointer from req info.
 * \param [in] pt_dyn_info PT Dynamic information from req info.
 * \param [in] clear Indicates whether it reserve or unreserve
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_reserve_entries_for_ltid(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int index,
                                        int8_t entry_type,
                                        uint8_t lookup_id,
                                        bool clear);

/*!
 * \brief Calculate the hash value for the entry key and data
 *  and the given Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid..
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry_key_words Entry Key
 * \param [in] entry_data_words Entry Data
 * \param [out] hash_value Calculated hash.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_prio_only_entry_hash_value_get(int unit,
                                    bcmltd_sid_t ltid,
                                    bcmptm_rm_tcam_lt_info_t *ltid_info,
                                    uint32_t **entry_key_words,
                                    uint32_t **entry_data_words,
                                    uint32_t *hash_val);

/*!
 * \brief Fetch the address to TCAM entry hash of the given Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid..
 * \n This address points to the address of hash zero. To get the address to
 * \n any other valid hash, increment this start pointer by hash times as memory
 * \n created to hold the hash information of all valid hash values is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] lookup_id lookup_id for the current LTID.
 * \param [out] entry_hash Double pointer to hold pointer to start address
 * \n           of TCAM hash information of an LTID.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_only_entry_hash_get(int unit,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       int pipe_id,
                                       uint8_t lookup_id,
                                       int **entry_hash);
/*!
 * \brief Move the range of TCAM entries from one index to the other
 *    in priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] from_idx TCAM index from which entry has to be moved.
 * \param [in] to_idx TCAM index to which entry has to be moved.
 * \param [in] entry_type of entry
 * \param [in] entry_attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_prio_only_entry_move_range(int unit,
                                          bcmptm_rm_tcam_entry_iomd_t *iomd,
                                          uint32_t idx_start,
                                          uint32_t idx_end,
                                          uint8_t entry_type);
/*!
 * \brief Compress entries in order to free entries for 2x,3x and 4x type
 * \n entries.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd IN/OUT metadata to/from RM-TCAM
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_only_compress(int unit,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Fetch the address to tcam information of a given LTID.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [out] tcam_info memory address to tcam information.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_only_tcam_info_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_prio_only_info_t **tcam_info);

/*!
 * \brief Compare's key in request info and index provided.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] index Key at this index will be compared with key req_info.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 *
 * \retval SHR_E_NONE kes are same.
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_only_entry_compare(int unit,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd,
                                       uint32_t index);

/*!
 * \brief Compare's key in request info and index provided in traverse snapshot
 *        of a given prio only LTID.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] index Index in snapshot
 * \param [in] req_info Information passed with TCAM table request
 * \param [in] key_db Address to KEY database in snapshot.
 * \param [in] key_size Key size in words for the LT.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_traverse_prio_only_entry_compare(int unit,
                                                bcmltd_sid_t ltid,
                                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                                uint32_t index,
                                                bcmptm_rm_tcam_req_t *req_info,
                                                uint32_t **key_db,
                                                uint16_t key_size);

/*!
 * \brief Get the LTID TCAM info overlapped with other LTID TCAM info.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 * \param [in] shared_info Overlap TCAM info.
 * \param [in] shared_info_count size of shared_info array.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_only_shared_info_get(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              int pipe_id,
                              bcmptm_rm_tcam_prio_only_shr_info_t **shared_info,
                              uint16_t *shared_info_count);

/*!
 * \brief Check if loops are formed in the prio only TCAM LT entry
 * \n hash linked list. This is a debug function which can be used to
 * \n when system is in indefinite loop once bcmptm request dispatches
 * \n to rm tcam prio only driver.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 *
 * \retval SHR_E_NONE No loops found
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL loop found
 */
extern int
bcmptm_rm_tcam_prio_only_find_loops(int unit,
                             bcmltd_sid_t ltid,
                             bcmptm_rm_tcam_lt_info_t *ltid_info,
                             int pipe_id);
/*!
 * \brief Validate entry info for an LTID
 * \n This is a debug function which can be used to
 * \n find incosistencies in entry info
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pipe_id Pipe Number
 *
 * \retval SHR_E_NONE Entry info is consistent
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL Bad entry info found
 */

extern int
bcmptm_rm_tcam_prio_only_validate_entry_info(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int flags);


/*!
 * \brief Update the shared information in the case of prio only TCAM
 * \n Shared information is kept for shared TCAM. The entries of the
 * \n shared TCAM should be marked as used since the unlerlying resource
 * \n is shared
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] entry_info entry information of the entry
 * \param [in] pipe_id Pipe Number
 * \param [in] clear flag indicating whether to clear/add
 * \param [in] index index in the shared TCAM
 *
 * \retval SHR_E_NONE Information updated
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_prio_only_shared_info_update(int unit,
                                bcmptm_rm_tcam_entry_iomd_t *iomd,
                                bool clear,
                                uint32_t index);

/*!
 * \brief Update the shared information in the case of prio only TCAM
 * \n Shared information is kept for shared TCAM. The entries of the
 * \n shared TCAM should be marked as used since the unlerlying resource
 * \n is shared
 *
 * \param [in] unit Logical device id
 * \param [in] flags flags from upper layer
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info dynamic information from upper layer
 * \param [in] req_info Request information
 * \param [out] rsp_info Response information
 * \param [out] rsp_ltid Response LTID information.
 * \param [out] rsp_flags Response flags.Not used.
 * \param [in] iomd Input(from PTM), Output(to PTM) and Meta(In RM-TCAM)
 *                  data corresponding to the LT entry.
 *
 * \retval SHR_E_NONE Table info updated
 * \retval SHR_E_PARAM Invalid parameters
 */

extern int
bcmptm_rm_tcam_get_table_info(int unit,
                              uint64_t flags,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              bcmbd_pt_dyn_info_t *pt_dyn_info,
                              bcmptm_rm_tcam_req_t *req_info,
                              bcmptm_rm_tcam_rsp_t *rsp_info,
                              bcmltd_sid_t *rsp_ltid,
                              uint32_t *rsp_flags,
                              bcmptm_rm_tcam_entry_iomd_t *iomd);
extern int
bcmptm_rm_tcam_prio_only_copy_metadata(int unit,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                void *old_ptr,
                                void *new_ptr);
extern int
bcmptm_rm_tcam_prio_only_metadata_cb_process(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_lt_info_t *ltid_info,
                              uint8_t *start_ptr,
                              uint8_t sub_component_id);
extern int
bcmptm_rm_tcam_prio_only_iomd_init(int unit,
                                  uint32_t req_flags,
                                  uint32_t cseq_id,
                                  bcmltd_sid_t ltid,
                                  bcmptm_rm_tcam_lt_info_t *ltid_info,
                                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                                  bcmptm_op_type_t req_op,
                                  bcmptm_rm_tcam_req_t *req_info,
                                  bcmptm_rm_tcam_rsp_t *rsp_info,
                                  bcmltd_sid_t *rsp_ltid,
                                  uint32_t *rsp_flags,
                                  bcmptm_rm_tcam_entry_iomd_t *iomd);

extern int
bcmptm_rm_tcam_prio_only_iomd_update(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd);
#endif /* RM_TCAM_PRIO_ONLY_H */
