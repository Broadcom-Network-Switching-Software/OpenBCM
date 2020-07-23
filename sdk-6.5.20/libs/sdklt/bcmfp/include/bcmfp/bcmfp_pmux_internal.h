/*! \file bcmfp_pmux_internal.h
 *
 * Defines, Enums and Structures to represent FP post extractors
 * related information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_PMUX_INTERNAL_H
#define BCMFP_PMUX_INTERNAL_H

#include <shr/shr_bitop.h>
#include <bcmfp/bcmfp_qual_internal.h>

typedef enum bcmfp_pmux_type_e {

    BCMFP_PMUX_TYPE_IPBM  = 0,

    BCMFP_PMUX_TYPE_DROP,

    BCMFP_PMUX_TYPE_NAT_NEEDED,

    BCMFP_PMUX_TYPE_NAT_DST_REALM_ID,

    BCMFP_PMUX_TYPE_SRC_DST_CONT_0,

    BCMFP_PMUX_TYPE_SRC_DST_CONT_1,

    BCMFP_PMUX_TYPE_SRC_DST_CONT_0_SLICE_B,

    BCMFP_PMUX_TYPE_SRC_DST_CONT_1_SLICE_B,

    BCMFP_PMUX_TYPE_SRC_DST_CONT_0_SLICE_C,

    BCMFP_PMUX_TYPE_SRC_DST_CONT_1_SLICE_C,

    BCMFP_PMUX_TYPE_EM_FIRST_LOOKUP_HIT,

    BCMFP_PMUX_TYPE_EM_SECOND_LOOKUP_HIT,

    BCMFP_PMUX_TYPE_EM_FIRST_LOOKUP_CLASS_ID,

    BCMFP_PMUX_TYPE_EM_SECOND_LOOKUP_CLASS_ID,

    BCMFP_PMUX_TYPE_COUNT,

} bcmfp_pmux_type_t;

typedef struct bcmfp_pmux_type_bmp_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_PMUX_TYPE_COUNT)];
} bcmfp_pmux_type_bmp_t;

typedef struct bcmfp_pmux_info_s {

    bcmfp_qualifier_t pmux_qual;

    uint8_t pmux_part;

    bcmfp_pmux_type_t pmux_type;

    int8_t pmux_pri_ctrl_sel_val;
} bcmfp_pmux_info_t;

#endif /* BCMFP_PMUX_INTERNAL_H */
