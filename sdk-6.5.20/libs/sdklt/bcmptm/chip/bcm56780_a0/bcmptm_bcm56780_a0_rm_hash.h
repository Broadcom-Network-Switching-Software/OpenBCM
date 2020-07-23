/*! \file bcmptm_bcm56780_a0_rm_hash.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_BCM56780_A0_RM_HASH_H
#define BCMPTM_BCM56780_A0_RM_HASH_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_rm_hash_internal.h>


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Get hash table information for rm hash.
 *
 * \param [in] unit Device unit.
 * \param [out] tbl_info Pointer to bcmptm_rm_hash_table_info_t structure.
 * \param [out] num_tbl_info Pointer to the buffer recording the table number.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_bcm56780_a0_hash_table_info_get(int unit,
                                       const bcmptm_rm_hash_table_info_t **tbl_info,
                                       uint8_t *num_tbl_info);

#endif /* BCMPTM_BCM56780_A0_RM_HASH_H */
