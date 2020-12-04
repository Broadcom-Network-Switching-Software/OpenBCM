/*! \file bcmptm_rm_hash_imm.h
 *
 * IMM table header file
 *
 * This file defines data structures and function for TABLE_HASH_STORE_INFO LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_IMM_H
#define BCMPTM_RM_HASH_IMM_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Insert a TABLE_HASH_STORE_INFO entry.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID of the entry.
 * \param [in] hash_store Hash store value of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_store_info_update(int unit,
                                 bcmltd_sid_t ltid,
                                 uint32_t hash_store);

#endif /* BCMPTM_RM_HASH_IMM_H */
