/*! \file bcm56990_a0_fp_qualifier_enum_set.c
 *
 * Chip Specific Stage Qualifier enum set function
 * for (56990_A0).
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
#include <bcmfp/chip/bcm56990_a0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

int
bcm56990_a0_fp_qualifier_enum_set(int unit,
                                  bcmfp_stage_id_t stage_id,
                                  bcmfp_qualifier_t qual,
                                  bool is_presel,
                                  int value,
                                  uint32_t *data,
                                  uint32_t *mask)
{

    SHR_FUNC_ENTER(unit);

    switch (stage_id) {
        case BCMFP_STAGE_ID_INGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_fp_ing_qual_enum_set(unit, qual, is_presel, value, data, mask));
            break;
        case BCMFP_STAGE_ID_LOOKUP:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_fp_vlan_qual_enum_set(unit, qual, is_presel, value, data, mask));
            break;
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_fp_egr_qual_enum_set(unit, qual, is_presel, value, data, mask));
            break;
        case BCMFP_STAGE_ID_EXACT_MATCH:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm56990_a0_fp_em_qual_enum_set(unit, qual, is_presel, value, data, mask));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
