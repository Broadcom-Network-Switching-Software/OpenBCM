/*! \file bcmptm_rm_hash_vec_compute.h
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

#ifndef BCMPTM_RM_HASH_VEC_COMPUTE_H
#define BCMPTM_RM_HASH_VEC_COMPUTE_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Compute hash vector for a given hash table entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to entry.
 * \param [in] key_a Pointer to entry key a.
 * \param [in] key_b Pointer to entry key b.
 * \param [in] key_size Key size.
 * \param [in] vec_attr Pointer to bcmptm_rm_hash_vec_attr_t structure.
 * \param [in] e_bm Bucket mode.
 * \param [out] inst_srch_info Pointer to rm_hash_inst_srch_info_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_vector_compute(int unit,
                              uint32_t *entry,
                              uint8_t *key_a,
                              uint8_t *key_b,
                              int key_size,
                              const bcmptm_rm_hash_vec_attr_t *vec_attr,
                              rm_hash_bm_t e_bm,
                              rm_hash_inst_srch_info_t *inst_srch_info);
#endif /* BCMPTM_RM_HASH_VEC_COMPUTE_H */
