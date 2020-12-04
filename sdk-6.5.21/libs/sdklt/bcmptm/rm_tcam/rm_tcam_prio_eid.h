/*! \file rm_tcam_prio_eid.h
 *
 * Utils, defines internal to RM-TCAM-PRIO-EID TCAM
 * This file contains utils, defines which are internal to
 * TCAM resource manager(i.e. these are not visible outside RM-TCAM)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef RM_TCAM_PRIO_EID_H
#define RM_TCAM_PRIO_EID_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>
#include <bcmptm/generated/bcmptm_rm_tcam_prio_eid_ha.h>

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Function declarations (prototypes)
 */
/*!
 * \brief Read TCAM entry key words(key + mask) and data words for
 * \n the given TCAM entry ID for the priority based TCAMs.
 * \n By default data is read from PTCache for cacheable PTs..
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior
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
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_lookup(int unit,
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
 * \brief Insert the new TCAM entry info or Update the existing TCAM entry in
 * \n priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FULL No Resources
 * \retval SHR_E_EXISTS Requested entry_id already exist
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_insert(int unit,
                                     bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Delete the existing TCAM entry in priority based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] req_flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] req_info Information passed with TCAM table request
 * \param [out] rsp_info Information returned for tcam table request
 * \param [out] rsp_ltid Logical table from which entry is read
 * \param [out] rsp_flags Flags used in serving the TCAM table request
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_NOT_FOUND Request entry_id not found
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_delete(int unit,
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
 * \brief Move the TCAM entry from one index to the other in priority
 * \n based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] from_idx TCAM index from which entry has to be moved.
 * \param [in] to_idx TCAM index to which entry has to be moved.
 * \param [in] entry_attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_move(int unit,
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
 * Clear a TCAM entry at specific index. Involves writing all zeros to a
 * particulat TCAM index. Updates both HW and SW state.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_clear(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Read the entry words from a particular TCAM index.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_read(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Write the entry words to a particulat TCAM index.
 * \n Updates both HW and SW state
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_write(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Reserve the index for the new entry in prio-eid based TCAMs.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 * \param [out] reserved_used Reserved entry index is used.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_FULL No Resources
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_index_allocate(int unit,
                               bcmptm_rm_tcam_entry_iomd_t *iomd,
                               uint8_t * reserved_used);
/*!
 * Fetch the address to TCAM entry hash of the given Logical Table
 * enum that is accessing the table. This address points to the
 * address of hash zero. To get the address to any other valid hash,
 * increment this start pointer by hash times as memory created to
 * hold the hash information of all valid hash values is consecutive.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_hash_table_get(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Fetch the number of valid entries in the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_num_entries_get(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Fetch the entry hash size of the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_hash_table_size_get(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Fetch the free entry count of the TCAM.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_free_count_get(int unit,
                        bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Table information retrieval API
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
bcmptm_rm_tcam_prio_eid_tcam_get_table_info(int unit,
                                       uint64_t flags,
                                       bcmltd_sid_t ltid,
                                       bcmptm_rm_tcam_lt_info_t *ltid_info,
                                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                                       bcmptm_rm_tcam_req_t *req_info,
                                       bcmptm_rm_tcam_rsp_t *rsp_info,
                                       bcmltd_sid_t *rsp_ltid,
                                       uint32_t *rsp_flags,
                                       bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Increment the entry count in the TCAM.
 *
 * \param [in]  unit  Logical device id
 * \param [out] iomd  Input(from PTM), Output(to PTM)
 *                    and Meta(In RM-TCAM) data
 *                    corresponding to the LT entry.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_free_count_incr(int unit,
                             bcmptm_rm_tcam_entry_iomd_t *iomd);


/*!
 * \brief Populate all the information in IOMD structure.
 *
 * \param [in]  unit        Logical device id
 * \param [in]  req_flags   Flags from PTM
 * \param [in]  ltid        Logical Table ID.
 * \param [in]  ltid_info   LRD information for the LTID.
 * \param [in]  pt_dyn_info Physical table index information.
 * \param [in]  req_info    PTM Request information
 * \param [in]  rsp_info    PTM Response information
 * \param [in]  rsp_ltid    Response LTID information.
 * \param [in]  rsp_flags   Response flags.Not used.
 * \param [out] iomd        Input(from PTM), Output(to PTM)
 *                          and Meta(In RM-TCAM) data
 *                          corresponding to the LT entry.
 *
 * \retval SHR_E_NONE No errors
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_iomd_init(int unit,
                                  uint64_t req_flags,
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
/*!
 * \brief Update the IOM data of the tcam Logical table.
 *
 * \param [in] unit Logical device id
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_iomd_update(int unit,
                                    bcmptm_rm_tcam_entry_iomd_t *iomd);

/*!
 * \brief Move the range of prio eid TCAM entries one index up or downward
 * \n direction. This will internally call bcmptm_rm_tcam_entry_move
 * \n function for each individual entry move.
 *
 * \param [in] unit Logical device id
 * \param [in] ltid Logical Table enum that is accessing the table.
 * \param [in] ltid_info LRD information for the ltid.
 * \param [in] partition
 * \param [in] idx_start Start TCAM index to be move in the range.
 * \param [in] idx_end Last TCAM index to be moved in the range.
 * \param [in] entry_type
 * \param [in] attrs Attributes of entry.
 * \param [in] pt_dyn_info Dynamic address information for physical table.
 *             Refer bcmptm_rm_tcam_internal.h for more details
 * \param [in] iomd structure.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmptm_rm_tcam_prio_eid_entry_move_range(int unit,
                                bcmltd_sid_t ltid,
                                bcmptm_rm_tcam_lt_info_t *ltid_info,
                                uint8_t partition,
                                uint32_t idx_start,
                                uint32_t idx_end,
                                int8_t entry_type,
                                void *attrs,
                                bcmbd_pt_dyn_info_t *pt_dyn_info,
                                bcmptm_rm_tcam_entry_iomd_t *iomd);
/*!
 * \brief Get the local index and pointer to entry info structure
 *  when global index is passed
 *
 * \param [in]  unit Logical device id
 * \param [in]  iomd iomd structure.
 * \param [in]  global_idx global TCAM index.
 * \param [out] einfo entry info structure
 * \param [out] local_idx TCAM index local to the entry info structure.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
 extern int
bcmptm_rm_tcam_entry_info_get_from_array(int unit,
                              bcmptm_rm_tcam_entry_iomd_t *iomd,
                              int global_idx,
                              bcmptm_rm_tcam_prio_eid_entry_info_t **einfo,
                              uint32_t *local_idx);

/*!
 * \brief Update reserve entry index in fp group
 *
 * \param [in]  unit Logical device id
 * \param [in]  iomd iomd structure.
 * \param [in]  old_rsvd_idx global TCAM index.
 * \param [in]  new_rsvd_index new global TCAM index
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
 extern int
bcmptm_rm_tcam_fp_update_reserve_entry_index(int unit,
                                        bcmptm_rm_tcam_entry_iomd_t *iomd,
                                        int old_rsvd_index,
                                        int new_rsvd_index);
/*!
 * Clear a TCAM entry at specific index in HW. Involves writing all zeros to a
 * particulat TCAM index. Updates HW state.
 *
 * \param [in] unit Logical device id
 * \param [in] target_index global TCAM index.
 * \param [in] iomd Input(from PTM), Output(to PTM) and
 *                  Meta(In RM-TCAM) data corresponding
 *                  to the LT entry.
 *
 * \retval SHR_E_NONE Success
 */

extern int
bcmptm_rm_tcam_prio_eid_entry_clear_hw(int unit,
                              uint32_t target_index,
                              bcmptm_rm_tcam_entry_iomd_t *iomd);
#endif /* RM_TCAM_PRIO_EID_H */
