/*! \file bcm56996_a0_fp_qualifier_enum_get.c
 *
 * Chip Specific Stage Qualifier enum get function
 * for (56996_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_chip_internal.h>
#include <bcmfp/chip/bcm56996_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56996_a0_fp_qualifier_enum_get(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_qualifier_t qual,
                                  bool is_presel,
                                  uint32_t data,
                                  uint32_t mask,
                                  int *value)
{

    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_ing_qual_enum_get(unit, qual, is_presel, data, mask, value));
            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_vlan_qual_enum_get(unit, qual, is_presel, data, mask, value));
            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_egr_qual_enum_get(unit, qual, is_presel, data, mask, value));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56996_a0_fp_em_qual_enum_get(unit, qual, is_presel, data, mask, value));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
