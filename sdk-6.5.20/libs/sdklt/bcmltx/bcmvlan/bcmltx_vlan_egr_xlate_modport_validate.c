/*! \file bcmltx_vlan_egr_xlate_modport_validate.c
 *
 * Vlan Egress Xlate Custom Handler.
 *
 * This file contains table handler for Vlan Egress Xlate.
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
#include <bcmltx/bcmvlan/bcmltx_vlan_egr_xlate_modport_validate.h>
#include <bcmcfg/bcmcfg_lt.h>

/* Define Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTX_VLAN

int
bcmltx_vlan_egr_xlate_modport_validate(int unit,
                                       bcmlt_opcode_t opcode,
                                       const bcmltd_fields_t *in,
                                       const bcmltd_field_val_arg_t *arg)
{
    uint64_t port_grp_enable = 0;
    int retVal = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);
    COMPILER_REFERENCE(opcode);
    COMPILER_REFERENCE(in);
    COMPILER_REFERENCE(arg);

    retVal = bcmcfg_field_get(unit, VLAN_XLATE_CONFIGt,
                              VLAN_XLATE_CONFIGt_EGR_XLATE_PORT_GRPf, &port_grp_enable);
    if (SHR_FAILURE(retVal)) {
        return retVal;
    }

    if (port_grp_enable) {
        return SHR_E_CONFIG;
    }

    SHR_FUNC_EXIT();
}
