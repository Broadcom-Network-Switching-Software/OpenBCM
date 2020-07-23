/*! \file bcmptm_rm_hash_hit_mgmt.h
 *
 * Resource Manager for Hash Table
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_HIT_MGMT_H
#define BCMPTM_RM_HASH_HIT_MGMT_H

/*******************************************************************************
 * Includes
 */

#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Defines
 */
#define RM_HASH_HIT_MAX_CNT         4

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Remove the hit bit value in the physical entry before issue the entry.
 * On some devices, the functional hash table has separated physical hit bit
 * table. In this case, some reserved fileds in physical entry are used for
 * passing LT hit value to rm hash. These reserved fileds need to be cleared
 * before installing the entry into physical tables.
 *
 * \param [in] unit Unit number.
 * \param [in] ent_words Pointer to entry buffer.
 * \param [in] hit_info Pointer to bcmptm_rm_hash_hit_info_t structure.
 * \param [out] hit_list Buffer to hold the hit bit value.
 * \param [out] hit_cnt Buffer to hold the hit bit number.
 *
 * \retval NONE
 */
extern void
bcmptm_rm_hash_entry_hit_bit_retrieve(int unit,
                                      uint32_t *ent_words,
                                      const bcmptm_rm_hash_hit_info_t *hit_info,
                                      uint32_t *hit_list,
                                      uint8_t *hit_cnt);

/*!
 * \brief Update physical hit bit table for a hash LT entry.
 * On some devices, the functional hash table has separated physical hit bit
 * table. In this case, when the functional hash table is programmed, its
 * corresponding physical hit table needs to be programmed as well.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] dyn_info Pointer to dyn_info structure.
 * \param [in] hit_info Pointer to bcmptm_rm_hash_hit_info_t structure.
 * \param [in] hit_list Pointer to buffer containing the hit bit value.
 * \param [in] hit_cnt The hit bit count.
 *
 * \retval NONE
 */
extern int
bcmptm_rm_hash_hit_table_update(int unit,
                                rm_hash_req_context_t *req_ctxt,
                                bcmbd_pt_dyn_info_t *dyn_info,
                                const bcmptm_rm_hash_hit_info_t *hit_info,
                                uint32_t *hit_list,
                                uint8_t hit_cnt);

/*!
 * \brief Get physical hit bit table for a hash LT entry.
 * On some devices, the functional hash table has separated physical hit bit
 * table. In this case, when the hit bit to be retrieved, the corresponding hit
 * table should be read.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ctxt Pointer to rm_hash_req_context_t structure.
 * \param [in] dyn_info Pointer to dyn_info structure.
 * \param [in] hit_info Pointer to bcmptm_rm_hash_hit_info_t structure.
 * \param [out] ent_words Pointer to entry buffer.
 *
 * \retval NONE
 */
extern int
bcmptm_rm_hash_hit_bit_get(int unit,
                           rm_hash_req_context_t *req_ctxt,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           const bcmptm_rm_hash_hit_info_t *hit_info,
                           uint32_t *ent_words);
#endif /* BCMPTM_RM_HASH_HIT_MGMT_H */
