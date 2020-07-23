/*! \file bcmptm_rm_hash_robust_hash.h
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

#ifndef BCMPTM_RM_HASH_ROBUST_HASH_H
#define BCMPTM_RM_HASH_ROBUST_HASH_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Typedefs
 */
typedef struct rm_hash_rhash_ctrl_s *rm_hash_rhash_ctrl;

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get if robust hash is enabled or not.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Robust hash control pointer.
 * \param [out] enable Buffer to contain the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_rbust_enable_get(int unit,
                                rm_hash_rhash_ctrl rhash_ctrl,
                                bool *enable);

/*!
 * \brief Set robust hash enable state.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Robust hash control pointer.
 * \param [in] enable Buffer to contain the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_rbust_enable_set(int unit,
                                rm_hash_rhash_ctrl rhash_ctrl,
                                bool enabled);

/*!
 * \brief Get if robust hash is alignment or not.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Robust hash control pointer.
 * \param [out] enable Buffer to contain the result.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_rbust_alignment_get(int unit,
                                   rm_hash_rhash_ctrl rhash_ctrl,
                                   bool *alignment);

/*!
 * \brief Get robust hash info.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Robust hash control pointer.
 * \param [in] rh_idx Robust hash module index.
 * \param [in] key Buffer to contain the key.
 * \param [in] n_bits Key size in bit.
 * \param [in] remap_table_index Remap table index.
 * \param [in] action_table_index Action table index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_robust_hash_get(int unit,
                               rm_hash_rhash_ctrl rhash_ctrl,
                               int rh_idx,
                               uint8_t *key,
                               int n_bits,
                               int remap_table_index,
                               int action_table_index);

/*!
 * \brief Get robust hash remap table and action index.
 *
 * \param [in] unit Unit number.
 * \param [in] rhash_ctrl Robust hash control pointer.
 * \param [in] rh_idx Robust hash module index.
 * \param [in] key Buffer to contain the key.
 * \param [in] n_bits Key size in bit.
 * \param [out] remap_idx Remap table index.
 * \param [out] act_idx Action table index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_robust_hash_table_index_get(int unit,
                                           rm_hash_rhash_ctrl rhash_ctrl,
                                           uint8_t *key,
                                           int n_bits,
                                           int *remap_idx,
                                           int *act_idx);

/*!
 * \brief Initialize robust hash module for a key format.
 *
 * \param [in] unit Unit number.
 * \param [in] key_format Pointer to bcmptm_rm_hash_key_format_t structure.
 * \param [out] rhash_ctrl Pointer to rm_hash_rhash_cfg_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_rbust_info_init(int unit,
                               const bcmptm_rm_hash_key_format_t *key_format,
                               rm_hash_rhash_ctrl *rhash_ctrl);

/*!
 * \brief Free the resource allocated for robust hash module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_rbust_info_cleanup(int unit);

/*!
 * \brief Get the robust hash control object for an hash table SID.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Hash table SID.
 * \param [out] rhash_ctrl Pointer to rm_hash_rhash_ctrl structure.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_rm_hash_robust_hash_ctrl_get(int unit,
                                    bcmdrd_sid_t sid,
                                    rm_hash_rhash_ctrl *rhash_ctrl);

/*!
 * \brief Configure robust hash module for a robust hash module.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if this is warmboot.
 * \param [in] update Flag indicating if this is in update context.
 * \param [in] rhash_ctrl Pointer to rm_hash_rhash_ctrl structure.
 * \param [in] write_cb Pointer to bcmptm_rm_hash_robust_hash_write structure.
 * \param [in] ltid Hash LT id.
 * \param [in] trans_id Transaction id.
 * \param [in] seed Random seed for robust hash configuration.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_rm_hash_rbust_info_config(int unit,
                                 bool warm,
                                 bool init,
                                 rm_hash_rhash_ctrl rhash_ctrl,
                                 bcmptm_rm_hash_robust_hash_write write_cb,
                                 bcmltd_sid_t ltid,
                                 uint32_t trans_id,
                                 int seed);

#endif /* BCMPTM_RM_HASH_ROBUST_HASH_H */
