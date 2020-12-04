/*! \file bcm56780_a0_tcam_fp_dynamic.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56780_A0_TCAM_FP_DYNAMIC_H
#define BCM56780_A0_TCAM_FP_DYNAMIC_H

/*******************************************************************************
 * Includes
 */
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmptm/bcmptm_rm_tcam_fp.h>
#include <bcmptm/bcmptm_rm_tcam_internal.h>

extern int
bcm56780_a0_rm_tcam_dyn_lt_mreq_info_init(int unit,
                                          uint32_t trans_id,
                                          bcmltd_sid_t ltid,
                                          bool warm);

extern int
bcm56780_a0_rm_tcam_dynamic_lt_validate(int unit, bcmltd_sid_t ltid);

#endif /* BCM56780_A0_TCAM_FP_DYNAMIC_H */
