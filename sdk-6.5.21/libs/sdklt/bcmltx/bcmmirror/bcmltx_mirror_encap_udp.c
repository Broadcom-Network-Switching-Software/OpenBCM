/*! \file bcmltx_mirror_encap_udp.c
 *
 * Mirror Encap VLAN UDP Handler.
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
#include <bcmltx/bcmmirror/bcmltx_mirror_encap_udp.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_mirror_encap_udp_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint16_t   udp_src_port, udp_dst_port;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_udp_transform \n")));

    if ((in->count < 1) || (arg->rfields < 2)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    udp_src_port = (in->field[0]->data >> 48);
    udp_dst_port = (in->field[0]->data >> 32) & 0xFFFF;

    out->count=0;

    out->field[0]->data = udp_src_port;
    out->field[0]->id   = arg->rfield[out->count];
    out->field[0]->idx  = 0;
    out->count++;

    out->field[1]->data = udp_dst_port;
    out->field[1]->id   = arg->rfield[out->count];
    out->field[1]->idx  = 0;
    out->count++;

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mirror_encap_udp_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint16_t   udp_src_port, udp_dst_port;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_udp_transform \n")));

    if ((in->count < 2) || (arg->rfields < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    udp_src_port = in->field[0]->data;
    udp_dst_port = in->field[1]->data;

    out->count = 0;
    out->field[0]->data = (((uint64_t)udp_src_port << 48) |
                           ((uint64_t)udp_dst_port << 32));
    out->field[0]->id   = arg->rfield[out->count];
    out->field[0]->idx  = 0;
    out->count++;

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}
