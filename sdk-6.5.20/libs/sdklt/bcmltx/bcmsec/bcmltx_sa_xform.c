/*! \file bcmltx_sa_xform.c
 *
 * SEC SA table's LT key and value pair to PT fields Transform Handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmltx/bcmsec/bcmltx_num_sa_per_sc_map.h>
#include <bcmltx/bcmsec/bcmltx_sa_index.h>
#include <bcmltx/bcmsec/bcmltx_sa_oper_state.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMSEC_LTX


/*******************************************************************************
 * Global definitions
 */
int
bcmltx_num_sa_per_sc_map_transform(int unit,
                            const bcmltd_fields_t *in_key,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    uint32_t num_sa_val, blk_id, encrypt, num_sa;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (arg->rfields != 1 || in->count != 1 || out->count != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count = 1;
    blk_id = in_key->field[0]->data;
    num_sa_val = in->field[0]->data;
    encrypt = *arg->value;
    num_sa = num_sa_val ? 2 : 1;
    if (!encrypt) {
        num_sa = num_sa_val ? 4 : 2;
        num_sa_val = (~num_sa_val) & 0x1;
    }
    SHR_IF_ERR_EXIT
        (bcmsec_num_sa_per_sc_set(unit, blk_id, encrypt, num_sa));

    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->field[0]->data = num_sa_val;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_num_sa_per_sc_map_rev_transform(int unit,
                                  const bcmltd_fields_t *in_key,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t num_sa_val = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    if (arg->rfields != 1 || in->count != 1 || out->count != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    num_sa_val = in->field[0]->data;
    if (*arg->value == 0) {
        num_sa_val = (~num_sa_val) & 0x1;
    }

    out->count = 1;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->data = num_sa_val;
    out->field[0]->idx = 0;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_sa_index_transform(int unit,
                          const bcmltd_fields_t *in,
                          bcmltd_fields_t *out,
                          const bcmltd_transform_arg_t *arg)
{
    int sc_id, blk_id, an, num_sa = 0;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    out->count = 1;
    blk_id = in->field[0]->data;
    sc_id = in->field[1]->data;
    an = in->field[2]->data;

    if (*arg->value == 0) {
        SHR_IF_ERR_EXIT
            (bcmsec_num_sa_per_sc_get(unit, blk_id, 1, &num_sa));
    } else {
        SHR_IF_ERR_EXIT
            (bcmsec_num_sa_per_sc_get(unit, blk_id, 0, &num_sa));
    }

    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->field[0]->data = (sc_id * num_sa) + an;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_sa_oper_state_rev_transform(int unit,
                                   const bcmltd_fields_t *in_key,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
#define VALID 0
#define INVALID 1
    int blk_id, an, num_sa = 0, oper = INVALID;

    SHR_FUNC_ENTER(unit);

    /* Block ID */
    blk_id = in_key->field[0]->data;
    /* Association number*/
    an = in_key->field[1]->data;

    if (*arg->value == 0) {
        SHR_IF_ERR_EXIT
            (bcmsec_num_sa_per_sc_get(unit, blk_id, 1, &num_sa));
    } else {
        SHR_IF_ERR_EXIT
            (bcmsec_num_sa_per_sc_get(unit, blk_id, 0, &num_sa));
    }

    if (an < num_sa) {
        oper = VALID;
    }

    /* updating operational status */
    out->field[0]->data = oper;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

exit:
    SHR_FUNC_EXIT();
}

