/*! \file bcm5699x_l3.c
 *
 * BCM5699x L3 subordinate driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/xgs/l3.h>
#include <bcm_int/ltsw/l3_int.h>


#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

static const bcmint_ltsw_l3_switch_control_tbl_t
l3_switch_control_tbls[bcmiL3ControlCount] =
{
    [bcmiL3ControlNonIpL3ErrToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            NON_IP_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlUnknownL3DestToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            IPV4_UC_DST_MISS_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlV6L3ErrToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            IPV6_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlV6L3DstMissToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            IPV6_UC_MISS_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlV6L3LocalLinkDrop] = {
        L3_UC_CONTROLs,
        1,
        {
            DROP_SRC_IPV6_LINK_LOCALs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlV4L3ErrToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            IPV4_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlV4L3DstMissToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            IPV4_UC_DST_MISS_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlMartianAddrToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            MARTIAN_ADDRESS_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlL3UcTtlErrToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            TTL_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlL3SlowpathToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            L3_SLOW_PATH_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlL3MtuFailToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            L3_MTU_CHECK_FAIL_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlL3UcastTtl1ToCpu] = {
        L3_UC_CONTROLs,
        1,
        {
            TTL_1_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlL3SrcHitEnable] = {
        L3_UC_CONTROLs,
        1,
        {
            L3_SRC_HITs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlFlexIP6ExtHdr] = {
        L3_PARSER_CONTROLs,
        1,
        {
            ING_IPV6_EXT_HDR_PROTOs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlFlexIP6ExtHdrEgress] = {
        L3_PARSER_CONTROLs,
        1,
        {
            EGR_IPV6_EXT_HDR_PROTOs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlV6L3SrcDstLocalLinkDropCancel] = {
        L3_UC_CONTROLs,
        1,
        {
            DO_NOT_DROP_SRC_IPV6_AND_DIP_LINK_LOCALs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlMcastUnknownErrToCpu] = {
        L3_MC_CONTROLs,
        1,
        {
            L3_MC_INDEX_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlIpmcPortMissToCpu] = {
        L3_MC_CONTROLs,
        1,
        {
            L3_MC_PORT_MISS_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlIpmcErrorToCpu] = {
        L3_MC_CONTROLs,
        1,
        {
            L3_MC_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlIpmcTtlErrToCpu] = {
        L3_MC_CONTROLs,
        1,
        {
            TTL_ERROR_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlIgmpReservedMcastEnable] = {
        L3_MC_CONTROLs,
        1,
        {
            IGMP_RESERVED_MCs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlMldReservedMcastEnable] = {
        L3_MC_CONTROLs,
        1,
        {
            MLD_RESERVED_MCs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlMldDirectAttachedOnly] = {
        L3_MC_CONTROLs,
        1,
        {
            STRICT_MLD_CHECKs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlUnknownIpmcAsMcast] = {
        L3_MC_CONTROLs,
        1,
        {
            L3_MC_MISS_TO_L2s,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlTunnelUnknownIpmcDrop] = {
        L3_MC_CONTROLs,
        1,
        {
            L3_MC_TNL_DROPs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlIpmcTtl1ToCpu] = {
        L3_MC_CONTROLs,
        1,
        {
            TTL_1_TO_CPUs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlIpmcSameVlanPruning] = {
        L3_MC_CONTROLs,
        1,
        {
            L2_L3_MC_COMBINED_MODEs,
            INVALIDs,
            INVALIDs
        },
        false
    },
    [bcmiL3ControlEcmpHashMultipath] = {
        ECMP_CONTROLs,
        1,
        {
            HASH_ALGs,
            INVALIDs,
            INVALIDs
        },
        true
    },
    [bcmiL3ControlEcmpHashControl] = {
        ECMP_CONTROLs,
        3,
        {
            USER_DEFINED_VALUEs,
            USE_DIP_IN_HASH_CALCs,
            LB_HASHs,
        },
        false
    },
    [bcmiL3ControlEcmpHashUseFlowSelEcmp] = {
        ECMP_CONTROLs,
        1,
        {
            LB_HASH_FLOW_BASEDs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlEcmpHashBitCountSelect] = {
        ECMP_CONTROLs,
        1,
        {
            HASH_MASKs,
            INVALIDs,
            INVALIDs,
        },
        true
    },
    [bcmiL3ControlIgmpUcastEnable] = {
        L3_MC_CONTROLs,
        1,
        {
            IGNORE_UC_IGMP_PAYLOADs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlMldUcastEnable] = {
        L3_MC_CONTROLs,
        1,
        {
            IGNORE_UC_MLD_PAYLOADs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlIp4McastL2DestCheck] = {
        L3_MC_CONTROLs,
        1,
        {
            IPV4_MC_DST_MAC_CHECKs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlIp6McastL2DestCheck] = {
        L3_MC_CONTROLs,
        1,
        {
            IPV6_MC_DST_MAC_CHECKs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlIpmcSameVlanL3Route] = {
        L3_MC_CONTROLs,
        1,
        {
            IPMC_ROUTE_SAME_VLANs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelWeightedEcmp] = {
        ECMP_CONTROLs,
        1,
        {
            LB_HASH_FLOW_BASED_MEMBER_WEIGHTs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelOverlayEcmp] = {
        ECMP_CONTROLs,
        1,
        {
            OVERLAY_LB_HASH_FLOW_BASEDs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelL2Ecmp] = {
        ECMP_CONTROLs,
        1,
        {
            LB_HASH_FLOW_BASED_L2s,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelOverlayL2Ecmp] = {
        ECMP_CONTROLs,
        1,
        {
            OVERLAY_LB_HASH_FLOW_BASED_L2s,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelRhEcmp] = {
        ECMP_CONTROLs,
        1,
        {
            LB_HASH_FLOW_BASED_RHs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelOverlayRhEcmp] = {
        ECMP_CONTROLs,
        1,
        {
            OVERLAY_LB_HASH_FLOW_BASED_RHs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelOverlayWeightedEcmp] = {
        ECMP_CONTROLs,
        1,
        {
            OVERLAY_LB_HASH_FLOW_BASED_MEMBER_WEIGHTs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlWESPProtocolEnable] = {
        L3_PARSER_CONTROLs,
        2,
        {
            ING_WESPs,
            EGR_WESPs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlWESPProtocol] = {
        L3_PARSER_CONTROLs,
        2,
        {
            ING_WESP_IP_PROTOs,
            EGR_WESP_IP_PROTOs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlECMPLevel1RandomSeed] = {
        ECMP_CONTROLs,
        1,
        {
            SINGLE_OVERLAY_RANDOM_SEEDs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlECMPLevel2RandomSeed] = {
        ECMP_CONTROLs,
        1,
        {
            UNDERLAY_RANDOM_SEEDs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
    [bcmiL3ControlFlowSelEcmpDynamic] = {
        DLB_CONTROLs,
        1,
        {
            LB_HASH_USE_FLOW_DLB_ECMPs,
            INVALIDs,
            INVALIDs,
        },
        false
    },
};

int
bcm5699x_ltsw_l3_switch_control_tbl_get(int unit,
                                        const bcmint_ltsw_l3_switch_control_tbl_t **tbl_db)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(tbl_db, SHR_E_PARAM);

    *tbl_db = l3_switch_control_tbls;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief L3 sub driver functions for bcm5699x.
 */
static mbcm_ltsw_l3_drv_t bcm5699x_l3_sub_drv = {
    .l3_switch_control_tbl_get = bcm5699x_ltsw_l3_switch_control_tbl_get,
};

/******************************************************************************
 * Public functions
 */

int
bcm5699x_l3_sub_drv_attach(int unit, void *hdl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_drv_set(unit, &bcm5699x_l3_sub_drv));

exit:
    SHR_FUNC_EXIT();
}
