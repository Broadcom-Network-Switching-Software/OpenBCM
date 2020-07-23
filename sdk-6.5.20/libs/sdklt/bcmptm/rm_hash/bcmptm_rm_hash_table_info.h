/*! \file bcmptm_rm_hash_table_info.h
 *
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_TABLE_INFO_H
#define BCMPTM_RM_HASH_TABLE_INFO_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get the actual table size for a hash LT.
 *
 * \param [in] unit Unit number.
 * \param [in] req_ltid Logical table ID.
 * \param [out] a_table_size Pointer to buffer to hold the table size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_table_info_get(int unit,
                              bcmltd_sid_t req_ltid,
                              uint32_t *a_table_size);
#endif /* BCMPTM_RM_HASH_TABLE_INFO_H */
