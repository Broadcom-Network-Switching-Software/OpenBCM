
/*! \file bcmcth_ts_bs.c
 *
 * BCMCTH TS BS LT APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_ts_drv.h>
#include <bcmcth/bcmcth_ts_bs.h>
#include <bcmcth/bcmcth_ts_bs_uc.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TIMESYNC

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ts_bs_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_uc_alloc(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_ts_bs_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_uc_free(unit));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_imm_register(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    /* Register the various components. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_control_imm_register(unit, warm));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_ts_bs_interface_imm_register(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_imm_cb_register(int unit, bool warm, int num_sids,
                               bcmcth_ts_bs_imm_reg_t *imm_reg)
{
    int i;

    SHR_FUNC_ENTER(unit);

    /* Callback registration for BS LTs. */
    for (i = 0; i < num_sids; i++) {
        if (bcmcth_ts_imm_mapped(unit, imm_reg[i].sid)) {

            if ((warm == false) && imm_reg[i].prepopulate != NULL) {
                /* Prepopulate update_only tables. */
                imm_reg[i].prepopulate(unit, imm_reg[i].sid);
            }

            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit, imm_reg[i].sid,
                                     imm_reg[i].cb, NULL));
        }
    }

 exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_fields_free(int unit, size_t num_fields, bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    if (fields->field != NULL) {
        for (idx = 0; idx < num_fields; idx++) {
            if (fields->field[idx] != NULL) {
                shr_fmm_free(fields->field[idx]);
            }
        }
        SHR_FREE(fields->field);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_ts_bs_fields_alloc(int unit, size_t num_fields,
                            bcmltd_fields_t *fields)
{
    size_t idx;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(fields, SHR_E_PARAM);

    /* Allocate fields buffers */
    SHR_ALLOC(fields->field,
              sizeof(bcmltd_field_t *) * num_fields,
              "bcmcthTsBsFields");

    SHR_NULL_CHECK(fields->field, SHR_E_MEMORY);

    sal_memset(fields->field, 0, sizeof(bcmltd_field_t *) * num_fields);

    fields->count = num_fields;

    for (idx = 0; idx < num_fields; idx++) {
        fields->field[idx] = shr_fmm_alloc();
        SHR_NULL_CHECK(fields->field[idx], SHR_E_MEMORY);
        sal_memset(fields->field[idx], 0, sizeof(bcmltd_field_t));
    }

exit:
    if (SHR_FUNC_ERR()) {
        bcmcth_ts_bs_fields_free(unit, num_fields, fields);
    }
    SHR_FUNC_EXIT();
}
