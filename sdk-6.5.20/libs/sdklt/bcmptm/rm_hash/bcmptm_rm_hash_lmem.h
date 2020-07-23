/*! \file bcmptm_rm_hash_lmem.h
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

#ifndef BCMPTM_RM_HASH_LMEM_H
#define BCMPTM_RM_HASH_LMEM_H

/*******************************************************************************
 * Includes
 */

#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate memory for rm_hash_entry_buf_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern rm_hash_entry_buf_t *bcmptm_rm_hash_entry_buf_alloc(void);

/*!
 * \brief Free memory for rm_hash_entry_buf_t structure.
 *
 * \param [in] ent_buf Pointer to rm_hash_entry_buf_t structure.
 */
extern void
bcmptm_rm_hash_entry_buf_free(rm_hash_entry_buf_t *ent_buf);

/*!
 * \brief Allocate memory for rm_hash_srch_info_t structure.
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern rm_hash_glb_srch_info_t *bcmptm_rm_hash_glb_srch_info_alloc(void);

/*!
 * \brief Free memory for rm_hash_srch_info_t structure.
 *
 * \param [in] srch_info Pointer to rm_hash_srch_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_glb_srch_info_free(rm_hash_glb_srch_info_t *glb_srch_info);

/*!
 * \brief Allocate memory for rm_hash_srch_result_t structure.
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern rm_hash_map_srch_result_t *bcmptm_rm_hash_map_srch_result_alloc(void);

/*!
 * \brief Free memory for rm_hash_srch_result_t structure.
 *
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_map_srch_result_free(rm_hash_map_srch_result_t *map_srch_result);

/*!
 * \brief Cleanup memory space for rm lmem.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern void
bcmptm_rm_hash_lmem_cleanup(int unit);

/*!
 * \brief Lmem init for rm hash.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lmem_init(int unit);

#endif /* BCMPTM_RM_HASH_LMEM_H */
