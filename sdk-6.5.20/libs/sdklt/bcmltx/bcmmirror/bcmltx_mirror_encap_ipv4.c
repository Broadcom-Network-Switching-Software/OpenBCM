/*! \file bcmltx_mirror_encap_ipv4.c
 *
 * Mirror Encap IPV4 Handler.
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
#include <bcmltx/bcmmirror/bcmltx_mirror_encap_ipv4.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_mirror_encap_ipv4_rev_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_ipv4_transform \n")));

    if ((in->count < 3) || (arg->rfields < 6)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    out->count=0;
    /*! Source IPv4. */
    out->field[0]->data = (uint32_t)(in->field[0]->data >> 32);
    out->field[0]->id   = arg->rfield[out->count];
    out->field[0]->idx  = 0;
    out->count++;

    /*! Destination IPv4. */
    out->field[1]->data = (uint32_t)in->field[0]->data;
    out->field[1]->id   = arg->rfield[out->count];
    out->field[1]->idx  = 0;
    out->count++;

    /*! Type of service. */
    out->field[2]->data = (in->field[2]->data >> 16)& 0xFF;
    out->field[2]->id   = arg->rfield[out->count];
    out->field[2]->idx  = 0;
    out->count++;

    /*! Time to live. */
    out->field[3]->data = (in->field[1]->data >> 24)& 0xFF;
    out->field[3]->id   = arg->rfield[out->count];
    out->field[3]->idx  = 0;
    out->count++;

    /*! Proto. */
    out->field[4]->data = (in->field[1]->data >> 16)& 0xFF;
    out->field[4]->id   = arg->rfield[out->count];
    out->field[4]->idx  = 0;
    out->count++;

    /*! Do not fragment. */
    out->field[5]->data = (in->field[1]->data >> 46)& 0x1;
    out->field[5]->id   = arg->rfield[out->count];
    out->field[5]->idx  = 0;
    out->count++;

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mirror_encap_ipv4_transform(int unit,
                                const bcmltd_fields_t *in,
                                bcmltd_fields_t *out,
                                const bcmltd_transform_arg_t *arg)
{
    uint32_t  src_ipv4, dst_ipv4;
    uint8_t   tos, ttl, dnf, proto;

    SHR_FUNC_ENTER(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META(
                "bcmltx_mirror_encap_ipv4_transform \n")));

    if ((in->count < 6) || (arg->rfields < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     *                     IPv4 HEADER FORMAT
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * | | | | | | | | | | |1|1|1|1|1|1|1|1|1|1|2|2|2|2|2|2|2|2|2|2|3|3|
     * |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|
     * +-------+-------+---------------+-------------------------------+
     * | Vers  |  Len  |      ToS      |        Total Length           |
     * +-------+-------+---------------+-----+-------------------------+
     * |       Identification          |Flags|    Fragment Offset      |
     * +---------------+---------------+-----+-------------------------+
     * |      TTL      |   Protocol    |        Checksum               |
     * +---------------+---------------+-----+-------------------------+
     * |                  Source IP Address (32 bits)                  |
     * +---------------------------------------------------------------+
     * |               Destination IP Address (32 bits)                |
     * +---------------------------------------------------------------+
     */

    src_ipv4   = in->field[0]->data;
    dst_ipv4   = in->field[1]->data;
    tos        = in->field[2]->data;
    ttl        = in->field[3]->data;
    proto      = in->field[4]->data;
    dnf        = in->field[5]->data;

    /*
     * ERSPAN::ERSPAN_HEADER_V4 [191:32] bits
     * [ 95:32]  mapped to out->field[0] [63:0]
     * [159:96]  mapped to out->field[1] [63:0]
     * [191:160] mapped to out->field[2] [31:0]
     */
    out->count=0;
    out->field[0]->id   = arg->rfield[0];
    out->field[0]->idx  = out->count;
    out->field[0]->data = ((uint64_t)src_ipv4 << 32) | dst_ipv4;
    out->count++;

    out->field[1]->id   = arg->rfield[0];
    out->field[1]->idx  = out->count;
    out->field[1]->data = ((uint64_t) ttl << 24) |
                          ((uint64_t) proto << 16) | ((uint64_t)dnf << 46);
    out->count++;

    out->field[2]->id   = arg->rfield[0];
    out->field[2]->idx  = out->count;
    out->field[2]->data = ((uint64_t)0x45 << 24) | ((uint64_t)tos << 16);
    out->count++;

    SHR_EXIT();

 exit:
    SHR_FUNC_EXIT();
}

