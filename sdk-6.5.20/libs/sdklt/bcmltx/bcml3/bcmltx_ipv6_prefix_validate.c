/*! \file bcmltx_ipv6_prefix_validate.c
 *
 * IPv6 prefix to IPv4 map Custom Handler.
 *
 * This file contains table handler for IPV6 prefix to IPv4 map.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltx/bcml3/bcmltx_ipv6_prefix_validate.h>

/* Define Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_L3

int
bcmltx_ipv6_prefix_validate(int unit,
                            bcmlt_opcode_t opcode,
                            const bcmltd_fields_t *in,
                            const bcmltd_field_val_arg_t *arg)
{
    uint64_t ipv6_lower = 0;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(arg);

    /* No input no output */
    if (in->count == 0) {
        SHR_EXIT();
    }

    if (in->count != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ipv6_lower = in->field[0]->data;
    /* The lower 32 bits of  IPv6 address can't be set. */
    if (ipv6_lower & 0xffffffff) {
        return SHR_E_PARAM;
    }

exit:
    SHR_FUNC_EXIT();
}
