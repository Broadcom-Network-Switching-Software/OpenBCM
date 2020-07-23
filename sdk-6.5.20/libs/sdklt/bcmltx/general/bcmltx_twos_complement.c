/*! \file bcmltx_twos_complement.c
 *
 * This transform is used to handle array field.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_bitop.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_twos_complement.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_twos_complement_rev_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)

{
    uint32_t bytes, decrement = 0, mask, num_bits;

    SHR_FUNC_ENTER(unit);
    bytes = (uint32_t)(in->field[0]->data);
    num_bits = (arg->field_list->field_array[0].maxbit -
                arg->field_list->field_array[0].minbit) + 1;
    mask = (1 << num_bits) - 1;
    if (num_bits > 32) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* coverity[overrun-local] */
    if (SHR_BITGET(&bytes, (num_bits - 1))) {
        decrement = 1;
        bytes = (~bytes + 1) & mask;
    }

    /* Bytes. */
    out->field[0]->data = bytes;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    /* Decrement. */
    out->field[1]->data = decrement;
    out->field[1]->idx = 0;
    out->field[1]->id = arg->rfield[1];

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_twos_complement_transform(int unit,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_transform_arg_t *arg)
{
    uint32_t bytes, decrement, num_bits = 8, mask;

    SHR_FUNC_ENTER(unit);

    bytes = (uint32_t)(in->field[0]->data);
    decrement = (uint32_t)(in->field[1]->data);
    num_bits = (arg->rfield_list->field_array[0].maxbit -
                arg->rfield_list->field_array[0].minbit) + 1;
    mask = (1 << num_bits) - 1;
    if (num_bits > 32) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }


    if (decrement) {
        /* 2's complement. */
        bytes = (~bytes + 1) & mask;
    }

    /* Secured data packet enable. */
    out->field[0]->data = bytes;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

exit:
    SHR_FUNC_EXIT();
}
