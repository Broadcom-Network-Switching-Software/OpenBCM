/*! \file udf.c
 *
 * BCM56780_A0 UDF Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/mbcm/udf.h>
#include <bcm_int/ltsw/xfs/udf.h>
#include <bcm_int/ltsw/chip/bcm56780_a0/udf.h>

#include <bcm_int/ltsw/lt_intf.h>
#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmlt/bcmlt.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_UDF

static mbcm_ltsw_udf_drv_t bcm56780_a0_ltsw_udf_drv = {
    .udf_init               = bcm56780_a0_ltsw_udf_init,           /* UDF init */
    .udf_detach             = xfs_ltsw_udf_detach,                 /* UDF detach */
    .udf_multi_chunk_create = xfs_ltsw_udf_multi_chunk_create,     /* UDF create */
    .udf_multi_chunk_info_get = xfs_ltsw_udf_multi_chunk_info_get, /* UDF chunk info get */
    .udf_destroy            = xfs_ltsw_udf_destroy,                /* UDF destroy */
    .udf_is_object_exist    = xfs_ltsw_udf_is_object_exist,        /* UDF object check */
    .udf_multi_pkt_fmt_info_get = xfs_ltsw_udf_multi_pkt_fmt_info_get, /* UDF Pkt Format Info. */
    .udf_abstr_pkt_fmt_object_list_get =
                              xfs_ltsw_udf_abstr_pkt_fmt_object_list_get, /* UDF objects list for a
                                                                             given abstract pkt format. */
    .udf_abstr_pkt_fmt_list_get = xfs_ltsw_udf_abstr_pkt_fmt_list_get,
                                        /* UDF supported abstract pkt fmt list */
};


/* UDF table info */
bcmint_udf_lt_tbls_info_t bcm56780_a0_udf_tbls_info = {
    .udf_sid             = "UDF_POLICY",
    .udf_key_fid         = "UDF_POLICY_ID",
    .udf_anchor_fid      = "PACKET_HEADER",
    .udf_offset_fid      = "OFFSET",
    .udf_cont_1_byte_fid = "CONTAINER_1_BYTE",
    .udf_cont_2_byte_fid = "CONTAINER_2_BYTE",
    .udf_cont_4_byte_fid = "CONTAINER_4_BYTE",
    .udf_info_sid        = "UDF_POLICY_INFO",
    .udf_info_key_fid    = "UDF_POLICY_INFO_ID",
    .udf_num_cont_1b_fid = "NUM_CONTAINER_1_BYTE",
    .udf_num_cont_2b_fid = "NUM_CONTAINER_2_BYTE",
    .udf_num_cont_4b_fid = "NUM_CONTAINER_4_BYTE",
    .udf_max_cont_fid    = "MAX_CONTAINER"
};

/*
 * UDF Abstract Packet Formats to LT PACKET_HEADER mapping.
 */
static char *bcm56780_a0_abstr_fmt_ltmap[bcmUdfAbstractPktFormatLastCount] = {
    "OUTER_L2_HDR_SNAP_OR_LLC",           /* bcmUdfAbstractPktFormatLlc */
    "INNER_L2_HDR_SNAP_OR_LLC",           /* bcmUdfAbstractPktFormatInnerLlc */
    "OUTER_L3_L4_HDR_UNKNOWN_L3",         /* bcmUdfAbstractPktFormatUnknownL3 */
    "INNER_L3_L4_HDR_UNKNOWN_L3",         /* bcmUdfAbstractPktFormatInnerUnknownL3 */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownNonIp */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerKnownNonIp */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownL3Mim */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownL3FcoeStdEncap */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownL3FcoeVftIfr */
    "OUTER_L3_L4_HDR_MPLS_LABEL_0",       /* bcmUdfAbstractPktFormatKnownL3MplsOneLabel */
    "OUTER_L3_L4_HDR_MPLS_LABEL_1",       /* bcmUdfAbstractPktFormatKnownL3MplsTwoLabel */
    "OUTER_L3_L4_HDR_MPLS_LABEL_2",       /* bcmUdfAbstractPktFormatKnownL3MplsThreeLabel */
    "OUTER_L3_L4_HDR_MPLS_LABEL_3",       /* bcmUdfAbstractPktFormatKnownL3MplsFourLabel */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownL3MplsMoreThanFourLabel */
    "OUTER_L3_L4_HDR_UNKNOWN_L4",         /* bcmUdfAbstractPktFormatUnknownL4 */
    "INNER_L3_L4_HDR_UNKNOWN_L4",         /* bcmUdfAbstractPktFormatInnerUnknownL4 */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUdpUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpGpeGeneve */
    "OUTER_L3_L4_HDR_BFD",                /* bcmUdfAbstractPktFormatUdpBfd */
    "INNER_L3_L4_HDR_BFD",                /* bcmUdfAbstractPktFormatInnerUdpBfd */
    "OUTER_L3_L4_HDR_VXLAN",              /* bcmUdfAbstractPktFormatUdpVxlan */
    "OUTER_L3_L4_HDR_P_1588",             /* bcmUdfAbstractPktFormatUdp1588 */
    NULL,                                 /* bcmUdfAbstractPktFormatTcpUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerTcpUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatSctpUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerSctpUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatGreWithoutKey */
    "OUTER_L3_L4_HDR_GRE_KEY",            /* bcmUdfAbstractPktFormatGreWithKey */
    NULL,                                 /* bcmUdfAbstractPktFormatGreWithoutChecksumRouting */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreWithoutChecksumRouting */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreWithoutRouting */
    NULL,                                 /* bcmUdfAbstractPktFormatAchNonIp */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpINT */
    NULL,                                 /* bcmUdfAbstractPktFormatTcpUnknownL5WithIpExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatL3GreUnknownPayload */
    NULL,                                 /* bcmUdfAbstractPktFormatGreWithKeyRouting */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreWithKeyRouting */
    "INNER_L3_L4_HDR_P_1588",             /* bcmUdfAbstractPktFormatInnerUdp1588 */
    NULL,                                 /* bcmUdfAbstractPktFormatTcpINT */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownL3FcoeUnknownRctl */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd0 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd0EHType0 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd0EHType1 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd0EHType2 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd2 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd2EHType0 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd2EHType1 */
    NULL,                                 /* bcmUdfAbstractPktFormatHiGigPpd2EHType2 */
    NULL,                                 /* bcmUdfAbstractPktFormatGpeIOAM */
    NULL,                                 /* bcmUdfAbstractPktFormatKnownMplsMcast */
    NULL,                                 /* bcmUdfAbstractPktFormatLoopbackHdrType0 */
    NULL,                                 /* bcmUdfAbstractPktFormatLoopbackHdrType1 */
    NULL,                                 /* bcmUdfAbstractPktFormatL3Ipv4Fragmented */
    NULL,                                 /* bcmUdfAbstractPktFormatL3Ipv4FragmentedWithOptions */
    NULL,                                 /* bcmUdfAbstractPktFormatL3Ipv4WithoutOptions */
    NULL,                                 /* bcmUdfAbstractPktFormatL3Ipv4WithOptions */
    "OUTER_L3_L4_HDR_IPV6",               /* bcmUdfAbstractPktFormatL3Ipv6 */
    NULL,                                 /* bcmUdfAbstractPktFormatL2 */
    NULL,                                 /* bcmUdfAbstractPktFormatL2Untagged */
    "OUTER_L2_HDR_OTAG",                  /* bcmUdfAbstractPktFormatL2OuterVlanTag */
    "OUTER_L2_HDR_ITAG",                  /* bcmUdfAbstractPktFormatL2InnerVlanTag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2DoubleVlanTag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2OuterVlanWithGbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2InnerVlanWithGbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2DoubleVlanWithGbpTag */
    "OUTER_L2_HDR_VNTAG",                 /* bcmUdfAbstractPktFormatL2Vntag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2OuterVlanWithVntag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2InnerVlanWithVntag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2DoubleVlanWithVntag */
    "OUTER_L2_HDR_ETAG",                  /* bcmUdfAbstractPktFormatL2Etag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2OuterVlanEtag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2InnerVlanEtag */
    NULL,                                 /* bcmUdfAbstractPktFormatL2DoubleVlanEtag */
    "INNER_L3_L4_HDR_IPV4",               /* bcmUdfAbstractPktFormatInnerL3Ipv4 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL3Ipv4Fragmented */
    "INNER_L3_L4_HDR_IPV6",               /* bcmUdfAbstractPktFormatInnerL3Ipv6 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL2Untagged */
    "INNER_L2_HDR_OTAG",                  /* bcmUdfAbstractPktFormatInnerL2OuterVlanTag */
    "INNER_L2_HDR_ITAG",                  /* bcmUdfAbstractPktFormatInnerL2InnerVlanTag */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL2DoubleVlanTag */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL2GbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL2OuterVlanWithGbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL2InnerVlanWithGbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerL2DoubleVlanWithGbpTag */
    NULL,                                 /* bcmUdfAbstractPktFormatUnknownL3CustomEtherType1 */
    NULL,                                 /* bcmUdfAbstractPktFormatUnknownL3CustomEtherType2 */
    NULL,                                 /* bcmUdfAbstractPktFormatUnknownL3CustomIpProtocol1 */
    NULL,                                 /* bcmUdfAbstractPktFormatUnknownL3CustomIpProtocol2 */
    NULL,                                 /* bcmUdfAbstractPktFormatUnknownL3CustomUdpDstPort1 */
    NULL,                                 /* bcmUdfAbstractPktFormatUnknownL3CustomUdpDstPort2 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUnknownL3CustomEtherType1 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUnknownL3CustomEtherType2 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUnknownL3CustomIpProtocol1 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUnknownL3CustomIpProtocol2 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUnknownL3CustomUdpDstPort1 */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUnknownL3CustomUdpDstPort2 */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpProbeBasedINT */
    NULL,                                 /* bcmUdfAbstractPktFormatMacsecDecrypt */
    NULL,                                 /* bcmUdfAbstractPktFormatGpeIOAMEdgeToEdge */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpVxlanWithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatUdp1588WithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpBfdWithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpProbeBasedINTWithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpINTWithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerTcpUnknownL5WithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatUdpUnknownL5WithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUdpBfdWithoutExtnHdr */
    "OUTER_L2_HDR_CNTAG",                 /* bcmUdfAbstractPktFormatL2Cntag */
    "OUTER_L3_L4_HDR_ARP",                /* bcmUdfAbstractPktFormatArp */
    "OUTER_L3_L4_HDR_AUTH_EXT_1",         /* bcmUdfAbstractPktFormatIpv6HdrAuthExt1 */
    "OUTER_L3_L4_HDR_AUTH_EXT_2",         /* bcmUdfAbstractPktFormatIpv6HdrAuthExt2 */
    "OUTER_L3_L4_HDR_ETHERTYPE",          /* bcmUdfAbstractPktFormatL2EtherType */
    "OUTER_L3_L4_HDR_FRAG_EXT_1",         /* bcmUdfAbstractPktFormatL3Ipv6HdrFragExt1 */
    "OUTER_L3_L4_HDR_FRAG_EXT_2",         /* bcmUdfAbstractPktFormatL3Ipv6HdrFragExt2 */
    "OUTER_L3_L4_HDR_GPE",                /* bcmUdfAbstractPktFormatGpe */
    "OUTER_L3_L4_HDR_GRE",                /* bcmUdfAbstractPktFormatGre */
    "OUTER_L3_L4_HDR_GRE_CHKSUM",         /* bcmUdfAbstractPktFormatGreWithChecksum */
    "OUTER_L3_L4_HDR_GRE_SEQ",            /* bcmUdfAbstractPktFormatGreWithSequence */
    "OUTER_L3_L4_HDR_ICMP",               /* bcmUdfAbstractPktFormatIcmp */
    "OUTER_L3_L4_HDR_IGMP",               /* bcmUdfAbstractPktFormatIgmp */
    "OUTER_L3_L4_HDR_IPV4",               /* bcmUdfAbstractPktFormatL3Ipv4 */
    "OUTER_L3_L4_HDR_MPLS_ACH",           /* bcmUdfAbstractPktFormatMplsAch */
    "OUTER_L3_L4_HDR_MPLS_CW",            /* bcmUdfAbstractPktFormatMplsCw */
    "OUTER_L3_L4_HDR_MPLS_LABEL_4",       /* bcmUdfAbstractPktFormatKnownL3MplsFiveLabel */
    "OUTER_L3_L4_HDR_MPLS_LABEL_5",       /* bcmUdfAbstractPktFormatKnownL3MplsSixLabel */
    "OUTER_L3_L4_HDR_MPLS_LABEL_6",       /* bcmUdfAbstractPktFormatKnownL3MplsSevenLabel */
    "OUTER_L3_L4_HDR_RARP",               /* bcmUdfAbstractPktFormatRarp */
    "OUTER_L3_L4_HDR_ROUT_EXT_1",         /* bcmUdfAbstractPktFormatL3RoutExt1 */
    "OUTER_L3_L4_HDR_ROUT_EXT_2",         /* bcmUdfAbstractPktFormatL3RoutExt2 */
    "OUTER_L3_L4_HDR_TCP_FIRST_4BYTES",   /* bcmUdfAbstractPktFormatL4Tcp */
    "OUTER_L3_L4_HDR_UDP",                /* bcmUdfAbstractPktFormatL4Udp */
    "OUTER_L3_L4_HDR_UNKNOWN_L5",         /* bcmUdfAbstractPktFormatUnknownL5 */
    "INNER_L2_HDR_CNTAG",                 /* bcmUdfAbstractPktFormatInnerL2Cntag */
    "INNER_L2_HDR_ETAG",                  /* bcmUdfAbstractPktFormatInnerL2Etag */
    "INNER_L2_HDR_L2",                    /* bcmUdfAbstractPktFormatInnerL2 */
    "INNER_L2_HDR_VNTAG",                 /* bcmUdfAbstractPktFormatInnerL2Vntag */
    "INNER_L3_L4_HDR_ARP",                /* bcmUdfAbstractPktFormatInnerArp */
    "INNER_L3_L4_HDR_AUTH_EXT_1",         /* bcmUdfAbstractPktFormatInnerIpv6HdrAuthExt1 */
    "INNER_L3_L4_HDR_AUTH_EXT_2",         /* bcmUdfAbstractPktFormatInnerIpv6HdrAuthExt2 */
    "INNER_L3_L4_HDR_ETHERTYPE",          /* bcmUdfAbstractPktFormatInnerL2EtherType */
    "INNER_L3_L4_HDR_FRAG_EXT_1",         /* bcmUdfAbstractPktFormatInnerL3Ipv6HdrFragExt1 */
    "INNER_L3_L4_HDR_FRAG_EXT_2",         /* bcmUdfAbstractPktFormatInnerL3Ipv6HdrFragExt2 */
    "INNER_L3_L4_HDR_ICMP",               /* bcmUdfAbstractPktFormatInnerIcmp */
    "INNER_L3_L4_HDR_IGMP",               /* bcmUdfAbstractPktFormatInnerIgmp */
    "INNER_L3_L4_HDR_RARP",               /* bcmUdfAbstractPktFormatInnerRarp */
    "INNER_L3_L4_HDR_TCP_FIRST_4BYTES",   /* bcmUdfAbstractPktFormatInnerL4Tcp */
    "INNER_L3_L4_HDR_UDP",                /* bcmUdfAbstractPktFormatInnerL4Udp */
    "INNER_L3_L4_HDR_UNKNOWN_L5",         /* bcmUdfAbstractPktFormatInnerUnknownL5 */
    NULL,                                 /* bcmUdfAbstractPktFormatIfaUdp */
    NULL,                                 /* bcmUdfAbstractPktFormatIfaTcp */
    NULL,                                 /* bcmUdfAbstractPktFormatNshOverL2 */
    NULL,                                 /* bcmUdfAbstractPktFormatCesBackplane */
    NULL,                                 /* bcmUdfAbstractPktFormatLoopbackType1WithDropIndication */
    NULL,                                 /* bcmUdfAbstractPktFormatGreHdrOfFourBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatGreHdrOfEightBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatGreHdrOfTwelveBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatGreHdrOfSixteenBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatGreHdrOfTwentyBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreHdrOfFourBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreHdrOfEightBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreHdrOfTwelveBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreHdrOfSixteenBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatL2GreHdrOfTwentyBytes */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUdpUnknownL5WithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatInnerUdp1588WithoutExtnHdr */
    NULL,                                 /* bcmUdfAbstractPktFormatL2Svtag */
    "SYS_HDR_EP_NIH"                      /* bcmUdfAbstractPktFormatRecircleHdr */
};

/******************************************************************************
 * Private functions
 */

int
bcm56780_a0_ltsw_udf_drv_attach(int unit)
{
   return mbcm_ltsw_udf_drv_set(unit, &bcm56780_a0_ltsw_udf_drv);
}

int
bcm56780_a0_ltsw_udf_init(int unit)
{
    uint32_t len;
    bcm_udf_abstract_pkt_format_t fmt;
    bcmint_udf_abstr_pkt_fmt_ltinfo_t *lt_info;
    SHR_FUNC_ENTER(unit);

    for (fmt = 0; fmt < bcmUdfAbstractPktFormatLastCount; fmt++) {
        if (bcm56780_a0_abstr_fmt_ltmap[fmt] != NULL) {
            lt_info = NULL;
            BCMINT_UDF_MEMALLOC(lt_info,
                                sizeof(bcmint_udf_abstr_pkt_fmt_ltinfo_t),
                                "UDF Pkt Fmt LTInfo Alloc.");
            if (lt_info == NULL) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_MEMORY);
            }
            len = sal_strlen(bcm56780_a0_abstr_fmt_ltmap[fmt]) + 1;
            BCMINT_UDF_MEMALLOC(lt_info->pkt_header, len,
                                "UDF anchor name alloc.");
            sal_strcpy(lt_info->pkt_header,
                       bcm56780_a0_abstr_fmt_ltmap[fmt]);

            /* update Anchor type in the flags */
            if (sal_strncmp(lt_info->pkt_header, "INNER", 5) == 0) {
                lt_info->flags = BCMINT_UDF_LT_ANCHOR_TYPE_INNER;
            } else {
                lt_info->flags = BCMINT_UDF_LT_ANCHOR_TYPE_OUTER;
            }
            ltsw_udf_abstr_pkt_fmt_ltinfo[unit][fmt] = lt_info;
        }
    }

    udf_tbls_info[unit] = &bcm56780_a0_udf_tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (xfs_ltsw_udf_init(unit));

exit:
    if (SHR_FAILURE(_func_rv)) {
        for (fmt = 0; fmt < bcmUdfAbstractPktFormatLastCount; fmt++) {
             if (ltsw_udf_abstr_pkt_fmt_ltinfo[unit][fmt]) {
                 BCMINT_UDF_MEMFREE(ltsw_udf_abstr_pkt_fmt_ltinfo[unit][fmt]);
                 ltsw_udf_abstr_pkt_fmt_ltinfo[unit][fmt] = NULL;
             }
        }
    }
    SHR_FUNC_EXIT();
}
