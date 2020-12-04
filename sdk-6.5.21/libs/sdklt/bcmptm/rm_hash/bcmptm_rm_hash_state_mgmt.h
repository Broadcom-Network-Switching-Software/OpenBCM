/*! \file bcmptm_rm_hash_state_mgmt.h
 *
 * Utils, defines internal to RM Hash state
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_STATE_MGMT_H
#define BCMPTM_RM_HASH_STATE_MGMT_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get the bitmap of used base entries of an entry in the SLB.
 *
 * This function will get the bitmap of used base entries of an entry
 * from the software logical bucket point of view.
 *
 * \param [in] unit Unit number.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [out] slb_be_bmp Pointer to a buffer to contain the calculated bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_slb_view_be_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_be_bmp);

/*!
 * \brief Get the bitmap of logical entry occupied by an entry in a SLB.
 *
 * This function will get the bitmap of logical entry for an entry
 * from the software logical bucket point of view.
 *
 * \param [in] unit Unit number.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [out] slb_le_bmp Pointer to a buffer to contain the calculated bitmap.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_slb_view_le_bmp_get(int unit,
                                       rm_hash_ent_loc_t *e_loc,
                                       uint16_t *slb_le_bmp);

/*!
 * \brief Allocate software state for a bank.
 *
 * This function will allocate software state for a bank.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] tbl_inst Table instance.
 *
 * \retval NONE.
 */
extern int
bcmptm_rm_hash_ha_state_alloc(int unit,
                              rm_hash_pt_info_t *pt_info,
                              uint8_t rbank,
                              int tbl_inst);

/*!
 * \brief Get the bucket state for an software logical bucket.
 *
 * This function will get the bucket state for an software logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] slb Software logical bucket.
 * \param [out] b_state Pointer to a buffer to contain the result.
 *
 * \retval NONE.
 */
extern int
bcmptm_rm_hash_slb_state_get(int unit,
                             rm_hash_pt_info_t *pt_info,
                             uint8_t rbank,
                             uint8_t pipe,
                             uint32_t slb,
                             rm_hash_slb_state_t **b_state);

/*!
 * \brief Allocate an entry node
 *
 * The entry node for each hash bucket is allocated in HA memory space during
 * system initialization. This routine finds the free entry node from the entry
 * node set belonging to the given hash bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] slb Software logical bucket number.
 * \param [in] e_node_idx Pointer to list of entry node.
 * \param [out] pe_node Pointer to a buffer to contain the address of node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_node_allocate(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t rbank,
                                 uint8_t pipe,
                                 uint32_t slb,
                                 uint8_t *e_node_idx,
                                 rm_hash_ent_node_t **pe_node);

/*!
 * \brief Insert an entry node
 *
 * Insert an entry node into the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] slb Software logical bucket number.
 * \param [in] nme_info Narrow mode entry info.
 * \param [in] e_node Pointer to a buffer to contain the address of node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_node_insert(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               uint8_t pipe,
                               uint32_t slb,
                               rm_hash_nme_info_t *nme_info,
                               rm_hash_ent_node_t *e_node);

/*!
 * \brief Search an entry node
 *
 * Search an entry node from the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] b_state Pointer to rm_hash_slb_state_t structure.
 * \param [in] bb_bmp base bucket bitmap.
 * \param [in] be_bmp base entry bitmap.
 * \param [in] nme_info Pointer to rm_hash_nme_info_t structure.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_node_search(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               uint8_t pipe,
                               rm_hash_slb_state_t *b_state,
                               uint8_t bb_bmp,
                               uint8_t be_bmp,
                               rm_hash_nme_info_t *nme_info,
                               rm_hash_ent_node_t **pe_node);

/*!
 * \brief Get the free entry node index in a bucket.
 *
 * Get an free entry index from the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] Pipe Pipe instance.
 * \param [in] slb SLB in which the entry will be installed.
 * \param [out] e_node_idx Buffer to hold the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_free_ent_node_idx_get(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     uint8_t rbank,
                                     uint8_t pipe,
                                     uint32_t slb,
                                     uint8_t *e_node_idx);

/*!
 * \brief Get the paired entry node of an entry node.
 *
 * Get an paired entry node for a given entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] pair_rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] paired_slb_seq Sequence number of paired SLB in a group.
 * \param [in] pair_slb Paired SLB value.
 * \param [out] pe_node Buffer to hold the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_paired_e_node_get(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t pair_rbank,
                                 uint8_t pipe,
                                 rm_hash_ent_node_t *e_node,
                                 uint8_t paired_slb_loc,
                                 uint32_t pair_slb,
                                 rm_hash_ent_node_t **pe_node);

/*!
 * \brief Get the paired SLB of an entry node.
 *
 * Get an paired SLB for a given entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] slb SLB value for an entry.
 * \param [out] exist Paired SLB exist.
 * \param [out] paired_slb_info Paired SLB value.
 * \param [out] pair_loc Buffer to hold the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_paired_slb_get(int unit,
                              rm_hash_pt_info_t *pt_info,
                              uint8_t rbank,
                              uint8_t pipe,
                              rm_hash_ent_node_t *e_node,
                              uint32_t slb,
                              bool *exist,
                              rm_hash_ent_slb_info_t *paired_slb_info,
                              uint8_t *paired_slb_loc);

/*!
 * \brief Delete an entry node
 *
 * Delete an entry node from the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] slb Software logical bucket.
 * \param [in] bb_bmp base bucket bitmap.
 * \param [in] be_bmp base entry bitmap.
 * \param [in] nme_info Pointer to rm_hash_nme_info_t structure.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_node_delete(int unit,
                               rm_hash_pt_info_t *pt_info,
                               uint8_t rbank,
                               uint8_t pipe,
                               uint32_t slb,
                               uint8_t bb_bmp,
                               uint8_t be_bmp,
                               rm_hash_nme_info_t *nme_info,
                               rm_hash_ent_node_t **pe_node);

/*!
 * \brief Get entry size based on entry bitmap.
 *
 * Get an entry size based on entry bitmap.
 *
 * \param [in] ent_bmp Base entry bitmap.
 *
 * \retval Entry size.
 */
extern uint8_t bcmptm_rm_hash_ent_size_get(uint8_t ent_bmp);

/*!
 * \brief Get an entry node.
 *
 * Get an entry node from the list of entry node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node_offset Offset of the requested entry node.
 * \param [out] pe_node Pointer to an entry node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_base_ent_node_get(int unit,
                                 rm_hash_pt_info_t *pt_info,
                                 uint8_t rbank,
                                 uint8_t pipe,
                                 uint32_t e_node_offset,
                                 rm_hash_ent_node_t **pe_node);

/*!
 * \brief Get an SLB node.
 *
 * Get an SLB node from the list of SLB node managed by a software
 * logical bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] e_slb_info_offset Offset of the requested SLB node.
 * \param [out] pe_slb_info Pointer to an rm_hash_ent_slb_info_t node.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_ent_slb_info_get(int unit,
                                rm_hash_pt_info_t *pt_info,
                                uint8_t rbank,
                                uint8_t pipe,
                                uint32_t e_slb_info_offset,
                                rm_hash_ent_slb_info_t **pe_slb_info);

/*!
 * \brief Determine if an entry node is a possible valid entry.
 *
 * Determine if an entry is a possible valid node and should be examined its content.
 *
 * \param [in] unit Unit number.
 * \param [in] e_node Pointer to rm_hash_ent_node_t structure.
 * \param [in] enm Narrow entry mode.
 * \param [out] match Pointer to the buffer recording the result.
 * \param [out] loc Pointer to the buffer recording the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_nm_ent_node_match(int unit,
                                 rm_hash_ent_node_t *e_node,
                                 rm_hash_entry_narrow_mode_t enm,
                                 bool *match,
                                 uint8_t *loc);

/*!
 * \brief Get the narrow mode entry bitmap.
 *
 * Get the narrow mode entry bitmap for a base entry in a bank.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe number.
 * \param [in] be_offset Base entry offset.
 * \param [out] nme_bmp Pointer to the buffer recording the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_nme_bmp_get(int unit,
                           rm_hash_pt_info_t *pt_info,
                           uint8_t rbank,
                           uint8_t pipe,
                           uint32_t be_offset,
                           rm_hash_nme_bmp_t **nme_bmp);

/*!
 * \brief Examine if there is enough space in a given bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] slb_e_bmp Pointer to rm_hash_slb_ent_bmp_t structure.
 * \param [in] e_info Pointer to rm_hash_req_ent_info_t structure.
 * \param [out] accom Pointer to bool variable.
 * \param [out] e_loc Pointer to rm_hash_ent_loc_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_bkt_free_ent_space_get(int unit,
                                      rm_hash_pt_info_t *pt_info,
                                      rm_hash_slb_ent_bmp_t *slb_e_bmp,
                                      rm_hash_req_ent_info_t *e_info,
                                      bool *accom,
                                      rm_hash_ent_loc_t *e_loc);

/*!
 * \brief Examine if there is enough space in a given bucket.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] rbank_list List of banks on which the slots will be searched.
 * \param [in] tbl_inst Table instance.
 * \param [in] slb_info_list Pointer to rm_hash_ent_slb_info_t list.
 * \param [in] num_slb_info Number of rm_hash_ent_slb_info_t in the list.
 * \param [in] e_attr Pointer to rm_hash_req_ent_attr_t structure.
 * \param [out] result Pointer to rm_hash_free_slot_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_free_ent_space_search(int unit,
                                     rm_hash_pt_info_t *pt_info,
                                     uint8_t *rbank_list,
                                     int tbl_inst,
                                     rm_hash_ent_slb_info_t *slb_info_list,
                                     uint8_t num_slb_info,
                                     rm_hash_req_ent_attr_t *e_attr,
                                     rm_hash_free_slot_info_t *result);

/*!
 * \brief Construct a new entry node based on various information.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_info Pointer to rm_hash_pt_info_t structure.
 * \param [in] e_loc Pointer to rm_hash_ent_loc_t structure.
 * \param [in] slb_info Pointer to rm_hash_ent_slb_info_t structure.
 * \param [in] num_slb_info Number of SLB.
 * \param [in] rbank Bank number.
 * \param [in] pipe Pipe instance.
 * \param [in] e_node_idx_list Entry node list.
 * \param [in] num_insts Number of instances.
 * \param [in] sid Physical table SID.
 * \param [in] e_sig Signature of the entry.
 * \param [in] bm Bucket mode of the entry.
 * \param [out] e_node Entry node of the entry.
 */
void
bcmptm_rm_hash_ent_node_set(int unit,
                            rm_hash_pt_info_t *pt_info,
                            rm_hash_ent_loc_t *e_loc,
                            rm_hash_ent_slb_info_t *slb_info,
                            uint8_t num_slb_info,
                            uint8_t rbank,
                            uint8_t pipe,
                            uint8_t *e_node_idx_list,
                            uint8_t num_insts,
                            bcmdrd_sid_t sid,
                            uint32_t e_sig,
                            rm_hash_bm_t bm,
                            rm_hash_ent_node_t *e_node);

/*!
 * \brief Resource cleanup.
 *
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_blk_ctrl_cleanup(int unit);

#endif /* BCMPTM_RM_HASH_STATE_MGMT_H */
