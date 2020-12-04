/*! \file bcm56990_b0_fp_ing_meter.c
 *
 * APIs to update meter related info
 * in entry policy words for
 * TH4-G device(56990_B0).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmfp/chip/bcm56990_b0_fp.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_DEV

int
bcmfp_bcm56990_b0_ingress_meter_actions_set(int unit,
                                bcmfp_group_id_t group_id,
                                bcmfp_pdd_id_t pdd_id,
                                bcmfp_group_oper_info_t *opinfo,
                                bcmcth_meter_fp_policy_fields_t *meter_entry,
                                uint32_t **edw)
{
    uint32_t meter_set = 0;
    uint32_t value = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(opinfo, SHR_E_PARAM);
    SHR_NULL_CHECK(meter_entry, SHR_E_PARAM);
    SHR_NULL_CHECK(edw, SHR_E_PARAM);

    /* METER_PAIR_MODE_MODIFIER */
    value = meter_entry->mode_modifier;
    bcmdrd_field_set(&meter_set, 11, 11, &(value));

    /* METER_PAIR_MODE */
    value = meter_entry->meter_pair_mode;
    bcmdrd_field_set(&meter_set, 8, 10, &value);

    /* METER_PAIR_INDEX */
    value = meter_entry->meter_pair_index;
    bcmdrd_field_set(&meter_set, 0, 7, &value);

    /* METER_UPDATE_EVEN */
    value = meter_entry->meter_update_even;
    bcmdrd_field_set(&meter_set, 14, 14, &value);

    /* METER_TEST_EVEN */
    value = meter_entry->meter_test_even;
    bcmdrd_field_set(&meter_set, 12, 12, &value);

    /* METER_UPDATE_ODD */
    value = meter_entry->meter_update_odd;
    bcmdrd_field_set(&meter_set, 15, 15, &value);

    /* METER_TEST_ODD */
    value = meter_entry->meter_test_odd;
    bcmdrd_field_set(&meter_set, 13, 13, &value);

    /* Set Meter set in edw words */
    bcmdrd_field_set(edw[0], 216, 231, &meter_set);

exit:
    SHR_FUNC_EXIT();
}
