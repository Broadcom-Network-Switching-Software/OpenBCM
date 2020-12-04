/*! \file bcmltx_sectag_icv_xform.c
 *
 * TM table's LT key and value pair to PT fields Transform Handler.
 *
 * Some PTs are designed to represent 2D table in 1D. Therefore, one of the
 * dimensions is encoded in the fields name such as PGx_SPID. However, the
 * corresponding LTs are usually 2D (2 key fields). Therefore, one of the key
 * fields is used as PT field selection. The transform functions in this files
 * is generic so that they can be applied to all PTs with such fields.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmltx/bcmsec/bcmltx_sectag_icv_xform.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMSEC_LTX


/*******************************************************************************
 * Global definitions
 */
int
bcmltx_sectag_icv_xform_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    uint32_t mode = 0;

    SHR_FUNC_ENTER(unit);
    mode = in->field[0]->data;

    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->field[0]->data = arg->value[mode];
    SHR_FUNC_EXIT();
}

int
bcmltx_sectag_icv_xform_rev_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    uint32_t index = 0, mode;

    SHR_FUNC_ENTER(unit);

    mode = in->field[0]->data;
    out->count = 1;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->idx = 0;
    for (index = 0; index < arg->values; index++) {
        if (arg->value[index] == mode) {
            out->field[0]->data = index;
            break;
        }
    }

    SHR_FUNC_EXIT();
}
