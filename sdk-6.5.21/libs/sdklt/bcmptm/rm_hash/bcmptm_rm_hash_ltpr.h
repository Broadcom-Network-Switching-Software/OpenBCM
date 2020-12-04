/*! \file bcmptm_rm_hash_ltpr.h
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

#ifndef BCMPTM_RM_HASH_LTPR_H
#define BCMPTM_RM_HASH_LTPR_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Configure LTPR table for an LT lookup.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Flag indicating if this is warmboot.
 * \param [in] init Flag indicating if this is in init context.
 * \param [in] ltpr_info Pointer to bcmptm_lt_lookup_ltpr_info_t structure.
 * \param [in] clear_hit Flag indicating if the LTPR entry should be cleared.
 * \param [in] write_cb Pointer to bcmptm_rm_hash_table_write structure.
 * \param [in] ltid Hash LT id.
 * \param [in] trans_id Transaction id.
 */
extern int
bcmptm_rm_hash_ltpr_info_config(int unit,
                                bool warm,
                                bool init,
                                bcmptm_lt_lookup_ltpr_info_t *ltpr_info,
                                bool clear_hit,
                                bcmptm_rm_hash_table_write write_cb,
                                bcmltd_sid_t ltid,
                                uint32_t trans_id);

#endif /* BCMPTM_RM_HASH_LTPR_H */
