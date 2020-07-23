/*! \file bcm5699x_tunnel.c
 *
 * BCM5699x tunnel subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/tunnel.h>
#include <bcm_int/ltsw/xgs/tunnel.h>
#include <bcm_int/ltsw/tunnel_int.h>
#include <bcm_int/ltsw/dev.h>

#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_TUNNEL

/******************************************************************************
 * Private functions
 */

static const bcmint_tunnel_sc_t tunnel_sc_info[bcmiTunnelControlCount] = {
    [bcmiTunnelControlTunnelIp4IdShared] = {
        TNL_CONTROLs,
        SHARE_FRAGMENT_IDs,
    },
    [bcmiTunnelControlTunnelErrToCpu] = {
        TNL_CONTROLs,
        TNL_ERR_TO_CPUs,
    },
    [bcmiTunnelControlMplsLabelMissToCpu] = {
        TNL_CONTROLs,
        MPLS_LABEL_MISS_TO_CPUs,
    },
    [bcmiTunnelControlMplsTtlErrToCpu] = {
        TNL_CONTROLs,
        MPLS_TTL_CHECK_FAILs,
    },
    [bcmiTunnelControlMplsInvalidL3PayloadToCpu] = {
        TNL_CONTROLs,
        MPLS_INVALID_PAYLOAD_TO_CPUs,
    },
    [bcmiTunnelControlMplsInvalidActionToCpu] = {
        TNL_CONTROLs,
        MPLS_INVALID_ACTION_TO_CPUs,
    },
    [bcmiTunnelControlMplsGalAlertLabelToCpu] = {
        TNL_CONTROLs,
        MPLS_GAL_LABEL_EXPOSED_TO_CPUs,
    },
    [bcmiTunnelControlMplsRalAlertLabelToCpu] = {
        TNL_CONTROLs,
        MPLS_RAL_LABEL_EXPOSED_TO_CPUs,
    },
    [bcmiTunnelControlMplsIllegalReservedLabelToCpu] = {
        TNL_CONTROLs,
        MPLS_ILLEGAL_RESERVED_LABEL_TO_CPUs,
    },
    [bcmiTunnelControlMplsUnknownAchTypeToCpu] = {
        TNL_CONTROLs,
        MPLS_UNKNOWN_ACH_TYPE_TO_CPUs,
    },
    [bcmiTunnelControlMplsUnknownAchVersionToCpu] = {
        TNL_CONTROLs,
        MPLS_UNKNOWN_ACH_VERSION_TO_CPUs,
    },
    [bcmiTunnelControlMplsPWControlWordToCpu] = {
        TNL_CONTROLs,
        MPLS_PW_ACH_TO_CPUs,
    },
    [bcmiTunnelControlHashUseFlowSelMplsEcmp] = {
        TNL_CONTROLs,
        MPLS_ECMP_LB_HASH_FLOW_BASEDs,
    },
    [bcmiTunnelControlHashUseFlowSelMplsEcmpOverlay] = {
        TNL_CONTROLs,
        MPLS_OVERLAY_ECMP_LB_HASH_FLOW_BASEDs,
    },
};

static int
bcm5699x_ltsw_tunnel_sc_info_get(
    int unit,
    const bcmint_tunnel_sc_t **sc_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(sc_info, SHR_E_MEMORY);

    *sc_info = tunnel_sc_info;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Tunnel sub driver functions for bcm5699x.
 */
static mbcm_ltsw_tunnel_drv_t bcm5699x_tunnel_sub_drv = {
    .tunnel_sc_info_get = bcm5699x_ltsw_tunnel_sc_info_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_tunnel_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_tunnel_drv_set(unit, &bcm5699x_tunnel_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
