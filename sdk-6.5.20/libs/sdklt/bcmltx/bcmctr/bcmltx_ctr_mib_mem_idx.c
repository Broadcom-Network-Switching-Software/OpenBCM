/*! \file bcmltx_ctr_mib_mem_idx.c
 *
 * MIB_MEM.__INDEX/__INSTANCE Transform Handler
 *
 * This file contains field transform information for
 * MIB_MEM.__INDEX/__INSTANCE.
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
#include <bcmltx/bcmctr/bcmltx_ctr_mib_mem_idx.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmpc/bcmpc_lport.h>
#include <bcmtm/bcmtm_utils.h>
#include <bcmptm/bcmptm_cci.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_CTR


int
bcmltx_ctr_mib_mem_idx_transform(int unit,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out,
                                 const bcmltd_transform_arg_t *arg)
{
    int pport = 0, lport;
    bcmdrd_sid_t sid;
    bcmdrd_port_num_domain_t domain;
    int index_min;
    int index_max;
    int txfm;
    int entry;


    SHR_FUNC_ENTER(unit);

    if ((in->count != 1)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    sid = arg->value[0];
    txfm = arg->value[1];
    entry = arg->value[2];
    lport = in->field[0]->data;

    /*
     * Need to change to physical port if domain is BCMDRD_PND_PHYS e.g. MIB
     * No Change in case domain is BCMDRD_PND_LOGIC e.g. IPIPE/EPIPE blocks
     */

    domain = bcmdrd_pt_port_num_domain(unit, sid);
    if (domain == BCMDRD_PND_PHYS) {
        pport = bcmpc_lport_to_pport(unit, lport);
        if (pport == BCMPC_INVALID_PPORT) {
            pport = lport;
        }
    } else {
        pport = lport;
    }

    if (txfm == 0) {
        SHR_IF_ERR_EXIT
            (bcmptm_pt_cci_frmt_index_from_port(unit, sid, pport,
                                  &index_min, &index_max));
        out->count = 0;
        /* ouput __INDEX */
        out->field[out->count]->id   = arg->rfield[0];
        out->field[out->count]->data = index_min + entry;
        out->count++;
    } else {
        out->count = 0;
        out->field[out->count]->id   = arg->rfield[0];
        out->field[out->count]->data = lport;
        out->count++;
    }
exit:
    SHR_FUNC_EXIT();
}
