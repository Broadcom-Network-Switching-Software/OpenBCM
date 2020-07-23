/*! \file bcmltx_ctr_egr_perq_xmt_idx.c
 *
 * EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE Transform Handler
 *
 * This file contains field transform information for
 * EGR_PERQ_XMT_COUNTERS.__INDEX/__INSTANCE.
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
#include <bcmltx/bcmctr/bcmltx_ctr_egr_perq_xmt_idx.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmtm/bcmtm_utils.h>

#define BSL_LOG_MODULE  BSL_LS_BCMLTX_CTR

#define Q_TYPE_UC  (0)
#define Q_TYPE_MC  (1)
#define Q_TYPE_CPU (2)

int
bcmltx_ctr_egr_perq_xmt_idx_transform(int unit,
                                      const bcmltd_fields_t *in,
                                      bcmltd_fields_t *out,
                                      const bcmltd_transform_arg_t *arg)
{
    int pipe, index, q_type, lport;
    int cpu_cosq_num, gp_cosq_num;
    int q, uc_cosq_num, mc_cosq_num;
    int pport_num;
    int blktype;
    const bcmdrd_chip_info_t *cinfo;

    SHR_FUNC_ENTER(unit);

    if ((in->count != 2)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (arg->values != 3) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    lport = in->field[0]->data;
    cinfo = bcmdrd_dev_chip_info_get(unit);
    blktype = bcmdrd_chip_blktype_get_by_name(cinfo, "epipe");
    if (blktype < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    pipe = bcmdrd_dev_blk_port_pipe(unit, blktype, lport);
    if (pipe < 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    q                   = in->field[1]->data;
    q_type              = arg->value[0];
    cpu_cosq_num        = arg->value[1];
    gp_cosq_num         = arg->value[2];

    if (bcmtm_lport_num_ucq_get(unit, lport, &uc_cosq_num)) {
        /* PT suppressed */
        return 0;
    }
    mc_cosq_num = gp_cosq_num - uc_cosq_num;

    if (bcmtm_lport_is_cpu(unit, lport)) {
        if (q_type == Q_TYPE_UC) {
            /* PT suppressed */
            return 0;
        }
        q_type = Q_TYPE_CPU;
    } else if (q_type == Q_TYPE_UC) {
        if (q >= uc_cosq_num) {
            /* PT suppressed */
            return 0;
        }
    } else if (q_type == Q_TYPE_MC) {
        if (q >= mc_cosq_num) {
            /* PT suppressed */
            return 0;
        }
    }

    switch(q_type) {
        case Q_TYPE_CPU:
            break;
        case Q_TYPE_MC:
            q += uc_cosq_num;
            /* fall-through */
        case Q_TYPE_UC:
            q += cpu_cosq_num;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_INTERNAL);
            break;
    }

    /* port numbers per blktype pipe */
    pport_num = bcmdrd_dev_blk_pipe_num_ports(unit, blktype, pipe);
    if (pport_num <= 0) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    index = (lport % pport_num) * gp_cosq_num + q;

    out->count = 0;
    /* ouput __INDEX */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->data = index;
    out->count++;
    /* ouput __INSTANCE */
    out->field[out->count]->id   = arg->rfield[1];
    out->field[out->count]->data = pipe;
    out->count++;
    LOG_DEBUG(BSL_LS_BCMLTX_CTR,
              (BSL_META_U(unit,
                          "Table index(%d) = (lport(%d) %% pport_num(%d)) "
                          "* gp_cosq_num(%d) + q(%d)\n"),
               index, lport, pport_num, gp_cosq_num, q));

exit:
    SHR_FUNC_EXIT();
}
