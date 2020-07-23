/*! \file bcmptm_rm_hash_key_extract.h
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

#ifndef BCMPTM_RM_HASH_KEY_EXTRACT_H
#define BCMPTM_RM_HASH_KEY_EXTRACT_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include "bcmptm_rm_hash_robust_hash.h"

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Extract key from entry buffer
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry buffer.
 * \param [in] key_flds Pointer to bcmlrd_hw_field_list_t structure.
 * \param [in] max_key_len Maximum key length.
 * \param [in] rhash_ctrl Pointer to bcmptm_rm_hash_rhash_ctrl_t structure.
 * \param [out] key Buffer to record the key.
 * \param [out] raw_key_length Raw key length for robust hash transform.
 * \param [out] key_size Buffer to record the key size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_key_extract(int unit,
                           uint32_t *entry,
                           const bcmlrd_hw_field_list_t *key_flds,
                           uint16_t max_key_len,
                           rm_hash_rhash_ctrl rhash_ctrl,
                           uint8_t *key,
                           int *raw_key_length,
                           int *key_size);
#endif /* BCMPTM_RM_HASH_KEY_EXTRACT_H */
