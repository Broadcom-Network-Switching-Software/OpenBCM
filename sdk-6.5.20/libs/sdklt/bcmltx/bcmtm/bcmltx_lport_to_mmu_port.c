/*! \file bcmltx_lport_to_mmu_port.c
 *
 * TM table's index Transform Handler
 *
 * This file contains field transform information for TM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bsl/bsl.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcmtm/bcmltx_lport_to_mmu_port.h>
#include <bcmtm/bcmtm_utils.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_TM

int
bcmltx_lport_to_mmu_port_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    int lport, mmu_port;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_lport_to_mmu_port_transform \n")));

    if (arg->rfields != 1 || in->count != 1 || out->count != 1) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    lport = in->field[0]->data;
    SHR_IF_ERR_EXIT(bcmtm_lport_mport_get(unit, lport, &mmu_port));
    out->field[0]->id = arg->rfield[0];
    out->field[0]->data = mmu_port;

exit:
    SHR_FUNC_EXIT();
}
