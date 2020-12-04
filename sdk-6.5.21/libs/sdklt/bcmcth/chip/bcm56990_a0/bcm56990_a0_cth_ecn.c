/*! \file bcm56990_a0_cth_ecn.c
 *
 * Chip stub for BCMCTH ECN.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>

#include <bcmdrd/chip/bcm56990_a0_enum.h>

#include <bcmcth/bcmcth_ecn_drv.h>
#include <bcmcth/bcmcth_ecn_protocol.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_ECN

/*******************************************************************************
* Private functions
 */

static int
bcm56990_a0_cth_ecn_proto_info_get(int unit, bcmcth_ecn_proto_info_t *proto_info)
{
    bcmcth_ecn_proto_info_t bcm56990_a0_proto_info = {
        .max_proto = 256,
        .num_proto_per_entry = 64,
        .protocol_match_sid = RESPONSIVE_PROTOCOL_MATCHm,
        .responsive_fid = RESPONSIVEf
    };

    if (proto_info) {
        sal_memcpy(proto_info, &bcm56990_a0_proto_info, sizeof(*proto_info));
    }
    return SHR_E_NONE;
}

static bcmcth_ecn_drv_t bcm56990_a0_cth_ecn_drv = {
    .proto_info_get = bcm56990_a0_cth_ecn_proto_info_get
};

/*******************************************************************************
* Public functions
 */

bcmcth_ecn_drv_t *
bcm56990_a0_cth_ecn_drv_get(int unit)
{
    return &bcm56990_a0_cth_ecn_drv;
}
