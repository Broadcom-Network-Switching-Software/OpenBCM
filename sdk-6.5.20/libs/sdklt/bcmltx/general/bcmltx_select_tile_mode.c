/*! \file bcmltx_select_tile_mode.c
 *
 * This transform simply copies fields from input to the output.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltx/general/bcmltx_select_tile_mode.h>
#include <bcmptm/bcmptm_uft.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

/*******************************************************************************
 * Public functions
 */
int
bcmltx_select_tile_mode_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    uint32_t tile_mode;

    SHR_FUNC_ENTER(unit);

    if (in->count != 0 || out->count != 2 || arg->values != 2) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Return tile mode corresponding to given tile. */
    SHR_IF_ERR_EXIT
        (bcmptm_uft_tile_cfg_get(unit, arg->value[0], &tile_mode, NULL, NULL));

    /* __SELECTOR: Tile to Tile Mode */
    out->field[0]->data = tile_mode;
    out->field[0]->idx = 0;
    out->field[0]->id = arg->rfield[0];

    /* __INDEX */
    out->field[1]->data = arg->value[1];
    out->field[1]->idx = 0;
    out->field[1]->id = arg->rfield[1];

exit:
    SHR_FUNC_EXIT();

    return 0;
}

int
bcmltx_select_tile_mode_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    return 0;
}


