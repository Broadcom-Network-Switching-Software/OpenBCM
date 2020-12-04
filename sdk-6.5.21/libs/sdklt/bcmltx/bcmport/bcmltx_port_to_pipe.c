/*! \file bcmltx_port_to_pipe.c
 *
 * Port table's port ID to port pipe field Transform Handler
 *
 * This file contains field transform information for Port.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_bitop.h>
#include <bsl/bsl.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmport/bcmltx_port_to_pipe.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_PORT



int
bcmltx_port_to_pipe_rev_transform(int unit,
                                  const bcmltd_fields_t *in_key,
                                  const bcmltd_fields_t *in_value,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    int pipe, lport;
    int blktype;
    const bcmdrd_chip_info_t *cinfo;

    SHR_FUNC_ENTER(unit);

    if ((in_key->count != 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lport = in_key->field[0]->data;
    cinfo = bcmdrd_dev_chip_info_get(unit);
    blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "ipipe");
    if (blktype < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pipe = bcmdrd_dev_blk_port_pipe(unit, blktype, lport);
    if (pipe < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count = 0;
    /* ouput PIPE */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = pipe;
    out->count++;

exit:
    SHR_FUNC_EXIT();

}

