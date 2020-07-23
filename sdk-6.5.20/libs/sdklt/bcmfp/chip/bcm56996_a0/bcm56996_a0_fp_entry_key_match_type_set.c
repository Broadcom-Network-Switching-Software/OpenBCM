/*! \file bcm56996_a0_fp_entry_key_match_type_set.c
 *
 * Chip Specific Stage entry key match type set function
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
bcm56996_a0_fp_entry_key_match_type_set(int unit,
                                        bcmfp_stage_t *stage,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        bcmfp_group_slice_mode_t slice_mode,
                                        bcmfp_group_type_t port_pkt_type,
                                        uint32_t **ekw)
{

    bcmfp_stage_id_t stage_id;

    SHR_FUNC_ENTER(unit);

    stage_id = stage->stage_id;

    switch (stage_id) {
        case BCMFP_STAGE_ID_EGRESS:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmfp_bcm56996_a0_egress_entry_key_match_type_set(unit,
                                                                   stage,
                                                                   group_id,
                                                                   opinfo,
                                                                   slice_mode,
                                                                   port_pkt_type,
                                                                   ekw));

            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();
}
