/*! \file bcmltx_dlb_port_status.c
 *
 * DLB_TRUNK_PORT_STATUS transform
 * utilities for link status.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmltx/bcmdlb/bcmltx_dlb_port_status.h>
#include <bcmltd/bcmltd_bitop.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_GENERAL

#define MAX_PORTS BCMDRD_CONFIG_MAX_PORTS
#define PBMP_SIZE  (MAX_PORTS/sizeof(uint64_t))
int
bcmltx_dlb_port_status_rev_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in_val,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    uint16_t port = 0;
    uint16_t array_idx = 0;
    int rv = SHR_E_NONE;
    BCMLTD_BITDCL in_pbmp[PBMP_SIZE] = {0};

    SHR_FUNC_ENTER(unit);

    if (in_key->count != 1) {
        rv = SHR_E_INTERNAL;
        SHR_ERR_EXIT(rv);
    }

    port = in_key->field[0]->data;

    out->count = 0;


    for (array_idx = 0; array_idx < in_val->count; array_idx++) {
        in_pbmp[array_idx] = in_val->field[array_idx]->data;
    }


    out->field[out->count]->data = (bool)BCMLTD_BIT_GET(in_pbmp, port);
    out->field[out->count]->idx  = 0;
    out->field[out->count++]->id = arg->rfield[0];

exit:
    SHR_FUNC_EXIT();
}
