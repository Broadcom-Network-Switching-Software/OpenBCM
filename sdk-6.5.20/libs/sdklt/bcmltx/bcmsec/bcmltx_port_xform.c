/*! \file bcmltx_port_blk_idx.c
 *
 * SEC LT key to PT fields transform handler.
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
#include <bcmltx/bcmsec/bcmltx_port_blk_idx.h>
#include <bcmltx/bcmsec/bcmltx_sec_port_oper_state.h>
#include <bcmltx/bcmsec/bcmltx_sec_port_pt_suppress.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMSEC_LTX

#define PORT_INFO_VALID 0
#define PORT_INFO_UNAVAIL 1


/*******************************************************************************
 * Global definitions
 */
int
bcmltx_port_blk_idx_transform(int unit,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_transform_arg_t *arg)
{
    int lport, rv;
    sec_pt_port_info_t info = {0};

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* PORT_ID */
    lport = in->field[0]->data;

    /* Get the SEC port mapping info if available. */
    rv = bcmsec_port_info_get(unit, lport, &info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);

    /* __INDEX */
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;
    out->field[0]->data = info.sec_local_port;

    /* __PORT */
    out->field[1]->id = arg->rfield[1];
    out->field[1]->idx = 0;
    out->field[1]->data = info.sec_blk;
exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_sec_port_oper_state_rev_transform(int unit,
                                     const bcmltd_fields_t *in_key,
                                     const bcmltd_fields_t *in,
                                     bcmltd_fields_t *out,
                                     const bcmltd_transform_arg_t *arg)
{
    int lport, rv, oper;
    sec_pt_port_info_t info;

    SHR_FUNC_ENTER(unit);

    /* Logical port */
    lport = in_key->field[0]->data;

    /* Get the SEC port mapping info if available. */
    rv = bcmsec_port_info_get(unit, lport, &info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    if (rv == SHR_E_NOT_FOUND) {
        oper = PORT_INFO_UNAVAIL;
    } else {
        oper = info.oper;
    }

    /* updating operational status */
    out->field[0]->data = oper;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_sec_port_pt_suppress_transform(int unit,
                                  const bcmltd_fields_t *in,
                                  bcmltd_fields_t *out,
                                  const bcmltd_transform_arg_t *arg)
{
    int lport, rv;
    sec_pt_port_info_t info;
    uint32_t pt_suppress = 0;

    SHR_FUNC_ENTER(unit);

    /* Logical port */
    lport = in->field[0]->data;

    /* Get the SEC port mapping info if available. */
    rv = bcmsec_port_info_get(unit, lport, &info);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    if ((rv == SHR_E_NOT_FOUND) ||
        (info.oper != PORT_INFO_VALID)) {
        pt_suppress = 1;
    }

    out->field[0]->data = pt_suppress;
    out->field[0]->id = arg->rfield[0];
    out->field[0]->idx = 0;

exit:
    SHR_FUNC_EXIT();
}
