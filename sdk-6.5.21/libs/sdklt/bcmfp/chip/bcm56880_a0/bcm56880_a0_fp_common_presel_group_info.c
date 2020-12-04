/*! \file bcm56880_a0_fp_common_presel_group_info.c
 *
 * IFP/VFP/EFP presel group key controls get function for
 * Trident4(56880_A0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/chip/bcm56880_a0_fp.h>
#include <bcmfp/bcmfp_keygen_api.h>
#include <bcmdrd/bcmdrd_field.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcm56880_a0_fp_common_presel_group_info_get(int unit,
                        bcmfp_stage_id_t stage_id,
                        bcmfp_group_id_t group_id,
                        uint32_t **presel_group_info)
{
    uint8_t idx = 0;
    uint32_t value = 0;
    size_t size = 0;
    bcmfp_ext_codes_t *ext_codes = NULL;

    bcmfp_group_oper_info_t *group_oper_info = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(presel_group_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmfp_presel_group_oper_info_get(unit,
                                          stage_id,
                                          group_id,
                                          &group_oper_info));

    if (!(group_oper_info->valid)) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    ext_codes = &(group_oper_info->ext_codes[0]);

    for (idx = 0; idx < 8; idx++) {
        if (ext_codes->l1_e4_sel[idx] != -1) {
            value = ext_codes->l1_e4_sel[idx];
            bcmdrd_field_set(presel_group_info[0],
                             (idx * 7),
                             ((idx * 7) + 7),
                             &value);
        }
    }

    for (idx = 0; idx < 6; idx++) {
        if (ext_codes->l1_e8_sel[idx] != -1) {
            value = ext_codes->l1_e8_sel[idx];
            bcmdrd_field_set(presel_group_info[0],
                             (56 + (idx * 6)),
                             (56 + ((idx * 6) + 6)),
                             &value);
        }
    }

    /* Size of IFTA100_T4T_00_LTS_PRE_SEL is 12 bytes. */
    size = 12;
    sal_memcpy(presel_group_info[1], presel_group_info[0], size);
    sal_memcpy(presel_group_info[2], presel_group_info[0], size);
exit:
    SHR_FUNC_EXIT();
}
