/*! \file bcmltx_mirror_encap_ipv6.c
 *
 * Mirror Encap IPV6 Handler.
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
#include <bcmltx/bcmmirror/bcmltx_mirror_encap_ipv6.h>
#include <bcmdrd/bcmdrd_field.h>

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_MIRROR

int
bcmltx_mirror_encap_ipv6_rev_transform(int unit,
                                       const bcmltd_fields_t *in,
                                       bcmltd_fields_t *out,
                                       const bcmltd_transform_arg_t *arg)
{
    uint32_t cnt;
    uint64_t src_ipv6_upper, src_ipv6_lower, dst_ipv6_upper, dst_ipv6_lower;
    uint32_t fl;
    uint8_t hop, nh, tc;
    uint64_t data[5] = {0, 0, 0, 0, 0};

    SHR_FUNC_ENTER(unit);

    if ((in->count < 3) || (arg->rfields < 6)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    for (cnt = 0; cnt < in->count; cnt++) {
        if (in->field[cnt]->id == arg->field[0] &&
            in->field[cnt]->idx < COUNTOF(data)) {
            data[in->field[cnt]->idx] = in->field[cnt]->data;
        }
    }

    dst_ipv6_lower  = data[0];
    dst_ipv6_upper  = data[1];
    src_ipv6_lower  = data[2];
    src_ipv6_upper  = data[3];
    hop             = (data[4])       & 0xFF;
    nh              = (data[4] >> 8)  & 0xFF;
    fl              = (data[4] >> 32) & 0xFFFFF;
    tc              = (data[4] >> 52) & 0xFF;

    out->count = 0;

    /*!  Destination IPv6. */
    out->field[out->count]->data = dst_ipv6_lower;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    out->field[out->count]->data = dst_ipv6_upper;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    /*! Source IPv6. */
    out->field[out->count]->data = src_ipv6_lower;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    out->field[out->count]->data = src_ipv6_upper;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    /* HOP_LIMIT */
    out->field[out->count]->data = hop;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    /* NEXT_HEADER */
    out->field[out->count]->data = nh;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    /* FLOW_LABLE */
    out->field[out->count]->data = fl;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

    /* TRAFFIC_CLASS */
    out->field[out->count]->data = tc;
    out->field[out->count]->id   = arg->rfield[out->count];
    out->field[out->count]->idx  = 0;
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}

int
bcmltx_mirror_encap_ipv6_transform(int unit,
                                   const bcmltd_fields_t *in,
                                   bcmltd_fields_t *out,
                                   const bcmltd_transform_arg_t *arg)
{
    uint64_t src_ipv6_upper, src_ipv6_lower, dst_ipv6_upper, dst_ipv6_lower;
    uint32_t fl;
    uint8_t hop, nh, tc;
    #define IPV6_HEADER_VERSION 0x6

    SHR_FUNC_ENTER(unit);

    if ((in->count < 8) || (arg->rfields < 1)) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /*
     *                    IPv6 HEADER FORMAT
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * | | | | | | | | | | |1|1|1|1|1|1|1|1|1|1|2|2|2|2|2|2|2|2|2|2|3|3|
     * |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|
     * +-------+---------------+---------------------------------------+
     * | Vers  | Traffic Class |          Flow Label                   |
     * +-------+---------------+-------+---------------+---------------+
     * |       Payload Length          |  Next Header  |   Hop Limit   |
     * +-------------------------------+---------------+---------------+
     * |                  Source IP Address (128 bits)                 |
     * +---------------------------------------------------------------+
     * |               Destination IP Address (128 bits)               |
     * +---------------------------------------------------------------+
     *
     * Next Header field in IPv6 is same as the Protocol field in IPv4
     * Traffic Class is same as Type Of Service field in IPv4
     */

    dst_ipv6_lower = in->field[0]->data;
    dst_ipv6_upper = in->field[1]->data;
    src_ipv6_lower = in->field[2]->data;
    src_ipv6_upper = in->field[3]->data;
    hop            = in->field[4]->data & 0xFF;
    nh             = in->field[5]->data & 0xFF;
    fl             = in->field[6]->data & 0xFFFFF;
    tc             = in->field[7]->data & 0xFF;

    /* 0 to 127 */
    out->count = 0;
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->idx  = out->count;
    out->field[out->count]->data = dst_ipv6_lower;
    out->count++;

    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->idx  = out->count;
    out->field[out->count]->data = dst_ipv6_upper;
    out->count++;

    /* 128 to 255 */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->idx  = out->count;
    out->field[out->count]->data = src_ipv6_lower;
    out->count++;

    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->idx  = out->count;
    out->field[out->count]->data = src_ipv6_upper;
    out->count++;

    /* 256 to 319 */
    out->field[out->count]->id   = arg->rfield[0];
    out->field[out->count]->idx  = out->count;
    out->field[out->count]->data = (((uint64_t)IPV6_HEADER_VERSION << 60) |
                                    ((uint64_t)tc << 52) |
                                    ((uint64_t)fl << 32) |
                                    ((uint64_t)nh << 8) |
                                    ((uint64_t)hop));
    out->count++;

 exit:
    SHR_FUNC_EXIT();
}
