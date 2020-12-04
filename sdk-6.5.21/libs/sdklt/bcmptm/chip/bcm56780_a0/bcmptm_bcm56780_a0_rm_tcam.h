/*! \file bcmptm_bcm56780_a0_rm_tcam.h
 *
 * Chip specific mem write utils
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_BCM56780_A0_RM_TCAM_H
#define BCMPTM_BCM56780_A0_RM_TCAM_H

/*******************************************************************************
 * Includes
 */
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_internal.h>
#include <sal/sal_types.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>


/*******************************************************************************
 * Defines
 */


/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */


/*******************************************************************************
 * Private Functions
 */


/*******************************************************************************
 * Function declarations (prototypes)
 */
extern int
bcm56780_a0_rm_tcam_memreg_oper(int unit,
                              bcmltd_sid_t ltid,
                              bcmptm_rm_tcam_sid_type_t sid_type,
                              bcmptm_rm_tcam_sid_oper_t sid_oper,
                              bcmptm_rm_tcam_sid_info_t *sid_info);


extern int
bcm56780_a0_rm_tcam_prio_only_hw_key_info_init(int unit,
                            bcmltd_sid_t ltid,
                            bcmptm_rm_tcam_lt_info_t *ltid_info,
                            uint8_t num_entries_per_index,
                            void *hw_field_list);

extern int
bcm56780_a0_rm_tcam_prio_only_shared_sid_compare(int unit,
                                     bcmdrd_sid_t sid,
                                     bcmdrd_sid_t shr_sid,
                                     uint32_t *neg_shr_flags,
                                     bool *compare);

extern int
bcm56780_a0_rm_tcam_ser_corr_sid_info_get(int unit,
                                 bcmdrd_sid_t sid,
                                 bcmdrd_sid_t *tcam_only_sid,
                                 bcmdrd_sid_t *data_only_sid,
                                 bcmdrd_sid_t *tcam_data_sid,
                                 bcmdrd_sid_t *paired_sid,
                                 uint8_t *sid_count);
#endif /* BCMPTM_BCM56780_A0_RM_TCAM_H */
