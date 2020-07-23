/*! \file udf.c
 *
 * UDF Driver for XGS chips.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>

#include <bcmltd/chip/bcmltd_str.h>

#include <bcm/udf.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/xgs/udf.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/issu.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_UDF

#define MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS (0x4)

/* Variable for XGS UDF control information */
bcmi_xgs_udf_control_info_t *xgs_udf_ctrl_info[BCM_MAX_NUM_UNITS];

/******************************************************************************
 * Private functions
 */

static int
ltsw_udf_pkt_format_set(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmint_udf_pkt_format_info_t *format_info)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(format, SHR_E_PARAM);
    SHR_NULL_CHECK(format_info, SHR_E_PARAM);

    format_info->priority = format->prio;
    format_info->ethertype = format->ethertype;
    format_info->ethertype_mask = format->ethertype_mask;
    format_info->ip_protocol = format->ip_protocol;
    format_info->ip_protocol_mask = format->ip_protocol_mask;
    format_info->l2 = format->l2;
    format_info->vlan_tag = format->vlan_tag;
    format_info->outer_ip = format->outer_ip;
    format_info->inner_ip = format->inner_ip;
    format_info->tunnel = format->tunnel;
    format_info->mpls = format->mpls;
    format_info->inner_protocol = format->inner_protocol;
    format_info->inner_protocol_mask = format->inner_protocol_mask;
    format_info->l4_dst_port = format->l4_dst_port;
    format_info->l4_dst_port_mask = format->l4_dst_port_mask;
    format_info->opaque_tag_type = format->opaque_tag_type;
    format_info->opaque_tag_type_mask = format->opaque_tag_type_mask;
    format_info->src_port = format->src_port;
    format_info->src_port_mask = format->src_port_mask;
    format_info->first_2B_after_mpls_bos =
        format->first_2bytes_after_mpls_bos;
    format_info->first_2B_after_mpls_bos_mask =
        format->first_2bytes_after_mpls_bos_mask;

    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeHigig,
            format->higig, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeVntag,
            format->vntag, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeEtag,
            format->etag, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeIcnm,
            format->icnm, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeCntag,
            format->cntag, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeSubportTag,
            format->subport_tag, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeIntPkt,
            format->int_pkt, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeOuterIFA,
            format->outer_ifa, format_info);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_SET(bcmiXgsUdfPktFormatMiscTypeInnerIFA,
            format->inner_ifa, format_info);

    format_info->class_id = format->class_id;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_pkt_format_get(
    int unit,
    bcmint_udf_pkt_format_info_t *format_info,
    bcm_udf_pkt_format_info_t *format)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(format, SHR_E_PARAM);
    SHR_NULL_CHECK(format_info, SHR_E_PARAM);

    sal_memset(format, 0, sizeof (bcm_udf_pkt_format_info_t));

    format->prio = format_info->priority;
    format->ethertype = format_info->ethertype;
    format->ethertype_mask = format_info->ethertype_mask;
    format->ip_protocol = format_info->ip_protocol;
    format->ip_protocol_mask = format_info->ip_protocol_mask;
    format->l2 = format_info->l2;
    format->vlan_tag = format_info->vlan_tag;
    format->outer_ip = format_info->outer_ip;
    format->inner_ip = format_info->inner_ip;
    format->tunnel = format_info->tunnel;
    format->mpls = format_info->mpls;
    format->inner_protocol = format_info->inner_protocol;
    format->inner_protocol_mask = format_info->inner_protocol_mask;
    format->l4_dst_port = format_info->l4_dst_port;
    format->l4_dst_port_mask = format_info->l4_dst_port_mask;
    format->opaque_tag_type = format_info->opaque_tag_type;
    format->opaque_tag_type_mask = format_info->opaque_tag_type_mask;
    format->src_port = format_info->src_port;
    format->src_port_mask = format_info->src_port_mask;
    format->first_2bytes_after_mpls_bos =
        format_info->first_2B_after_mpls_bos;
    format->first_2bytes_after_mpls_bos_mask =
        format_info->first_2B_after_mpls_bos_mask;

    /* Boolean pkt format needs only 2 bits */
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeHigig,
            format_info, format->higig);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeVntag,
            format_info, format->vntag);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeEtag,
            format_info, format->etag);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeIcnm,
            format_info, format->icnm);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeCntag,
            format_info, format->cntag);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeSubportTag,
            format_info, format->int_pkt);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeIntPkt,
            format_info, format->int_pkt);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeOuterIFA,
            format_info, format->outer_ifa);
    BCMI_XGS_UDF_PKT_FMT_MISC_FIELDS_GET(bcmiXgsUdfPktFormatMiscTypeInnerIFA,
            format_info, format->inner_ifa);

    format->class_id = format_info->class_id;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_fields_init(int unit)
{
    uint32 flags = 0;

    SHR_FUNC_ENTER(unit);

    if (XGS_UDF_TBL(unit)->etag_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_ETAG;
    }
    if (XGS_UDF_TBL(unit)->cntag_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_CNTAG;
    }
    if (XGS_UDF_TBL(unit)->vntag_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_VNTAG;
    }
    if (XGS_UDF_TBL(unit)->higig_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_HIGIG;
    }
    if (XGS_UDF_TBL(unit)->inband_telemetry_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_INT_PKT;
    }
    if (XGS_UDF_TBL(unit)->port_id_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_SRC_PORT;
    }
    if (XGS_UDF_TBL(unit)->loopback_hdr_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_HDR;
    }
    if (XGS_UDF_TBL(unit)->opaque_tag_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_OPAQUE_TAG_TYPE;
    }
    if (XGS_UDF_TBL(unit)->l4dst_port_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_L4DST_PORT;
    }
    if (XGS_UDF_TBL(unit)->lb_pkt_type_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_TYPE;
    }
    if (XGS_UDF_TBL(unit)->outer_fcoe_hdr_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_FCOE_HDR;
    }
    if (XGS_UDF_TBL(unit)->outer_ifa_hdr_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_OUTER_IFA_HDR;
    }
    if (XGS_UDF_TBL(unit)->inner_ifa_hdr_fid) {
        flags |= BCMI_XGS_UDF_LT_PKT_FORMAT_INNER_IFA_HDR;
    }
    if (XGS_UDF_TBL(unit)->class_id_fid) {
        flags |= BCMI_XGS_UDF_LT_OUTPUT_CLASS_ID;
    }
    if (XGS_UDF_TBL(unit)->flex_hash_fid) {
        flags |= BCMI_XGS_UDF_LT_OUTPUT_FLEX_HASH;
    }
    if (XGS_UDF_TBL(unit)->skip_outer_ifa_md_fid) {
        flags |= BCMI_XGS_UDF_LT_OFFSET_SKIP_OUTER_IFA_MD;
    }

    XGS_UDF_CTRL(unit)->lt_flags = flags;

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_l2format_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    const char *data_sym = NULL;
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_EXIT
        (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                    tbls_info->l2_type_fid,
                                    format->l2, &data_sym));

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_def_get(unit, tbls_info->udf_sid,
                               tbls_info->l2_type_mask_fid,
                               &fdef));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                tbls_info->l2_type_fid, data_sym));

    if (format->l2 == BCM_PKT_FORMAT_L2_ANY) {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->l2_type_mask_fid, fdef.def));
    } else {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->l2_type_mask_fid, fdef.max));
    }

exit:

    if (SHR_FUNC_ERR()) {
        LOG_DEBUG(BSL_LS_BCM_UDF,
                (BSL_META_U(unit, "Error: Invalid parameter value"
                            " for pkt_format->l2.\n")));
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_vlanformat_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    const char *data_sym = NULL;
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_EXIT
        (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                    tbls_info->vlan_tag_fid,
                                    format->vlan_tag, &data_sym));

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_def_get(unit, tbls_info->udf_sid,
                               tbls_info->vlan_tag_mask_fid,
                               &fdef));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                tbls_info->vlan_tag_fid, data_sym));

    if (format->vlan_tag == BCM_PKT_FORMAT_VLAN_TAG_ANY) {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->vlan_tag_mask_fid, fdef.def));
    } else {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->vlan_tag_mask_fid, fdef.max));
    }

exit:

    if (SHR_FUNC_ERR()) {
        LOG_DEBUG(BSL_LS_BCM_UDF,
                (BSL_META_U(unit, "Error: Invalid parameter value"
                            " for pkt_format->vlan_tag.\n")));
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_l3fields_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    uint32 l3_fields_val = 0, l3_fields_mask = 0;
    bcm_ethertype_t l2_ethertype_val = 0, l2_ethertype_mask = 0;
    const char *outer_ip_type_str = NULL, *inner_ip_type_str = NULL;
    uint8_t outer_chn_mask = 0, inner_chn_mask = 0;
    const char *outer_chn_str = NULL, *inner_chn_str = NULL;
    uint8 outer_ip_type_mask = 0, inner_ip_type_mask = 0;
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                    tbls_info->outer_l3_hdr_fid,
                                    format->outer_ip,
                                    &outer_ip_type_str));

    SHR_IF_ERR_VERBOSE_EXIT
        (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                    tbls_info->inner_l3_hdr_fid,
                                    format->inner_ip,
                                    &inner_ip_type_str));

    if (format->tunnel == BCM_PKT_FORMAT_TUNNEL_NONE) {

        switch(format->outer_ip) {
            case BCM_PKT_FORMAT_IP4_WITH_OPTIONS:
                l2_ethertype_val    = SHR_L2_ETYPE_IPV4;
                l2_ethertype_mask   = 0xffff;
                outer_ip_type_mask  = 0x7;
                break;
            case BCM_PKT_FORMAT_IP6_WITH_OPTIONS:
                l2_ethertype_val    = SHR_L2_ETYPE_IPV6;
                l2_ethertype_mask   = 0xffff;
                outer_ip_type_mask  = 0x7;
                break;
            case BCM_PKT_FORMAT_IP4:
                l2_ethertype_val    = SHR_L2_ETYPE_IPV4;
                l2_ethertype_mask   = 0xffff;
                break;
            case BCM_PKT_FORMAT_IP6:
                l2_ethertype_val    = SHR_L2_ETYPE_IPV6;
                l2_ethertype_mask   = 0xffff;
                break;
            case BCM_PKT_FORMAT_IP_NONE:
                outer_ip_type_mask  = 0x7;
                break;
            default:
                outer_ip_type_mask  = 0x0;
        }
    } else if (format->tunnel == BCM_PKT_FORMAT_TUNNEL_IP_IN_IP) {
        l2_ethertype_mask   = 0xffff;
        l3_fields_mask      = 0xff0000;

        if (format->inner_ip == BCM_PKT_FORMAT_IP4) {
            /* Inner ip protocol v4. */
            l3_fields_val       = 0x40000;
        } else if (format->inner_ip == BCM_PKT_FORMAT_IP6) {
            /* Inner ip protocol v6. */
            l3_fields_val       = 0x290000;
        } else {
            LOG_DEBUG(BSL_LS_BCM_UDF,
                    (BSL_META_U(unit, "Error: Packet format combination of"
                                " tunnel and inner_ip is not supported"
                                " by the device.\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        if (format->outer_ip == BCM_PKT_FORMAT_IP4) {
            /* L2 ether type 0x800 */
            l2_ethertype_val    = SHR_L2_ETYPE_IPV4;
        } else if (format->outer_ip == BCM_PKT_FORMAT_IP6) {
            /* L2 ether type 0x86dd */
            l2_ethertype_val    = SHR_L2_ETYPE_IPV6;
        } else {
            LOG_DEBUG(BSL_LS_BCM_UDF,
                    (BSL_META_U(unit, "Error: Packet format combination of"
                                " tunnel and outer_ip is not supported"
                                " by the device.\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else if (format->tunnel == BCM_PKT_FORMAT_TUNNEL_GRE) {
        l2_ethertype_mask   = 0xffff;
        l3_fields_mask      = 0xffffff;

        if (format->inner_ip == BCM_PKT_FORMAT_IP4) {
            /* Inner ip protocol gre, gre ethertype 0x800. */
            l3_fields_val       = 0x2f0800;
        } else if (format->inner_ip == BCM_PKT_FORMAT_IP6) {
            /* Inner ip protocol gre, gre ethertype 0x86dd. */
            l3_fields_val       = 0x2f86dd;
        } else if (format->inner_ip == BCM_PKT_FORMAT_IP_NONE) {
            /*
             * when set inner_ip to IP_NONE, inner_protocal should not be
             * IPV4 and IPV6
             */
            if ((format->inner_protocol == SHR_L2_ETYPE_IPV4) ||
                    (format->inner_protocol == SHR_L2_ETYPE_IPV6)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
            l3_fields_val       = (format->inner_protocol & 0xffff);
            l3_fields_mask      = (format->inner_protocol_mask & 0xffff);
        } else {
            LOG_DEBUG(BSL_LS_BCM_UDF,
                    (BSL_META_U(unit, "Error: Packet format combination of"
                                " tunnel and inner_ip is not supported"
                                " by the device.\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        if (format->outer_ip == BCM_PKT_FORMAT_IP4) {
            l2_ethertype_val     = SHR_L2_ETYPE_IPV4;
        } else if (format->outer_ip == BCM_PKT_FORMAT_IP6) {
            l2_ethertype_val     = SHR_L2_ETYPE_IPV6;
        } else {
            LOG_DEBUG(BSL_LS_BCM_UDF,
                    (BSL_META_U(unit, "Error: Packet format combination of"
                                " tunnel and outer_ip is not supported"
                                " by the device.\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else if (format->tunnel == BCM_PKT_FORMAT_TUNNEL_MPLS) {
        l3_fields_mask      = 0x0f;

        switch (format->mpls) {
            case BCM_PKT_FORMAT_MPLS_ONE_LABEL:
                l3_fields_val       = 0x01;
                break;
            case BCM_PKT_FORMAT_MPLS_TWO_LABELS:
                l3_fields_val       = 0x02;
                break;
            case BCM_PKT_FORMAT_MPLS_THREE_LABELS:
                l3_fields_val       = 0x03;
                break;
            case BCM_PKT_FORMAT_MPLS_FOUR_LABELS:
                l3_fields_val       = 0x04;
                break;
            case BCM_PKT_FORMAT_MPLS_FIVE_LABELS:
                l3_fields_val       = 0x05;
                break;
            case BCM_PKT_FORMAT_MPLS_ANY:
                l3_fields_val       = 0x0;
                l3_fields_mask      = 0x0;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        l2_ethertype_val = 0x8847;
        l2_ethertype_mask = 0xffff;

        if (format->first_2bytes_after_mpls_bos_mask != 0) {
            l3_fields_val = (((format->first_2bytes_after_mpls_bos & 0xffff)
                        << MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS) |
                    (l3_fields_val & 0xf));
            l3_fields_mask = (((format->first_2bytes_after_mpls_bos_mask
                            & 0xffff) << MPLS_LABEL_COUNT_WIDTH_FOR_UDF_L3_FIELDS) |
                    (l3_fields_mask & 0xf));
        }
    } else if ((format->tunnel == BCM_PKT_FORMAT_TUNNEL_FCOE) ||
            (format->tunnel == BCM_PKT_FORMAT_TUNNEL_FCOE_INIT)) {
        if (!(XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_FCOE_HDR)) {
            LOG_DEBUG(BSL_LS_BCM_UDF,
                    (BSL_META_U(unit, "Error: pkt_format->tunnel is not"
                                " supported by the device.\n")));
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        l2_ethertype_mask = 0xffff;
        l2_ethertype_val    = 0x8914;
        if (format->tunnel == BCM_PKT_FORMAT_TUNNEL_FCOE) {
            l2_ethertype_val    = 0x8906;
        }

        SHR_IF_ERR_EXIT
            (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                        tbls_info->outer_fcoe_hdr_fid,
                                        format->fibre_chan_outer,
                                        &outer_chn_str));

        SHR_IF_ERR_EXIT
            (bcmi_lt_field_def_get(unit, tbls_info->udf_sid,
                                   tbls_info->outer_fcoe_hdr_mask_fid,
                                   &fdef));
        outer_chn_mask = fdef.max;
        if (format->fibre_chan_outer == BCM_PKT_FORMAT_FIBRE_CHAN_ANY) {
            outer_chn_mask = fdef.def;
        }

        SHR_IF_ERR_EXIT
            (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                        tbls_info->inner_fcoe_hdr_fid,
                                        format->fibre_chan_inner,
                                        &inner_chn_str));

        SHR_IF_ERR_EXIT
            (bcmi_lt_field_def_get(unit, tbls_info->udf_sid,
                                   tbls_info->inner_fcoe_hdr_mask_fid,
                                   &fdef));

        inner_chn_mask = fdef.max;
        if (format->fibre_chan_outer == BCM_PKT_FORMAT_FIBRE_CHAN_ANY) {
            inner_chn_mask = fdef.def;
        }

    } else if ((BCM_PKT_FORMAT_TUNNEL_ANY == format->tunnel)
            && (BCM_PKT_FORMAT_IP_ANY == format->outer_ip)
            && (BCM_PKT_FORMAT_IP_ANY == format->inner_ip)) {
        l2_ethertype_mask = 0;
        l3_fields_mask = 0;
        outer_ip_type_mask = 0;
        inner_ip_type_mask = 0;
        inner_chn_mask =0;
        outer_chn_mask =0;
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Configure ip_protocol/mask fields */
    if ((format->tunnel != BCM_PKT_FORMAT_TUNNEL_MPLS)) {
        l3_fields_val       |= (format->ip_protocol << 16);
        l3_fields_mask      |= (format->ip_protocol_mask << 16);
    }

    /* ALL the keys are configured. Now set them in entry */
    if (l2_ethertype_mask == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_field_def_get(unit,
                    tbls_info->udf_sid,
                    tbls_info->ethertype_mask_fid, &fdef));
        l2_ethertype_mask = fdef.def;
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->ethertype_fid, l2_ethertype_val));
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->ethertype_mask_fid, l2_ethertype_mask));

    if (l3_fields_mask == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_field_def_get(unit,
                    tbls_info->udf_sid,
                    tbls_info->l3_fields_mask_fid, &fdef));
        l3_fields_mask = fdef.def;
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->l3_fields_fid, l3_fields_val));
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->l3_fields_mask_fid, l3_fields_mask));

    if (outer_ip_type_mask == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_field_def_get(unit,
                    tbls_info->udf_sid,
                    tbls_info->outer_l3_hdr_fid, &fdef));
        outer_ip_type_str = fdef.sym_def;
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                tbls_info->outer_l3_hdr_fid, outer_ip_type_str));
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->outer_l3_hdr_mask_fid, outer_ip_type_mask));

    if (inner_ip_type_mask == 0) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_field_def_get(unit,
                    tbls_info->udf_sid,
                    tbls_info->inner_l3_hdr_fid, &fdef));
        inner_ip_type_str = fdef.sym_def;
    }
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                tbls_info->inner_l3_hdr_fid, inner_ip_type_str));
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->inner_l3_hdr_mask_fid, inner_ip_type_mask));

    if (XGS_UDF_CTRL(unit)->lt_flags &
            BCMI_XGS_UDF_LT_PKT_FORMAT_FCOE_HDR) {
        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                    tbls_info->outer_fcoe_hdr_fid, outer_chn_str));
        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->outer_fcoe_hdr_mask_fid, outer_chn_mask));

        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                    tbls_info->inner_fcoe_hdr_fid, inner_chn_str));
        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->inner_fcoe_hdr_mask_fid, inner_chn_mask));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_misc_info_get(
    int unit,
    bcmi_xgs_udf_pkt_format_misc_type_t type,
    bcmi_xgs_udf_pkt_format_misc_type_info_t *info)
{
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    switch(type) {
        case bcmiXgsUdfPktFormatMiscTypeHigig:
            info->field     = tbls_info->higig_fid;
            info->mask      = tbls_info->higig_mask_fid;
            info->present   = BCM_PKT_FORMAT_HIGIG_PRESENT;
            info->none      = BCM_PKT_FORMAT_HIGIG_NONE;
            info->any       = BCM_PKT_FORMAT_HIGIG_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeVntag:
            info->field     = tbls_info->vntag_fid;
            info->mask      = tbls_info->vntag_mask_fid;
            info->present   = BCM_PKT_FORMAT_VNTAG_PRESENT;
            info->none      = BCM_PKT_FORMAT_VNTAG_NONE;
            info->any       = BCM_PKT_FORMAT_VNTAG_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeEtag:
            info->field     = tbls_info->etag_fid;
            info->mask      = tbls_info->etag_mask_fid;
            info->present   = BCM_PKT_FORMAT_ETAG_PRESENT;
            info->none      = BCM_PKT_FORMAT_ETAG_NONE;
            info->any       = BCM_PKT_FORMAT_ETAG_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeIcnm:
            info->field     = NULL;
            info->mask      = NULL;
            info->present   = BCM_PKT_FORMAT_ICNM_PRESENT;
            info->none      = BCM_PKT_FORMAT_ICNM_NONE;
            info->any       = BCM_PKT_FORMAT_ICNM_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeCntag:
            info->field     = tbls_info->cntag_fid;
            info->mask      = tbls_info->cntag_mask_fid;
            info->present   = BCM_PKT_FORMAT_CNTAG_PRESENT;
            info->none      = BCM_PKT_FORMAT_CNTAG_NONE;
            info->any       = BCM_PKT_FORMAT_CNTAG_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeSubportTag:
            info->field     = NULL;
            info->mask      = NULL;
            info->present   = BCM_PKT_FORMAT_SUBPORT_TAG_PRESENT;
            info->none      = BCM_PKT_FORMAT_SUBPORT_TAG_NONE;
            info->any       = BCM_PKT_FORMAT_SUBPORT_TAG_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeIntPkt:
            info->field     = tbls_info->inband_telemetry_fid;
            info->mask      = tbls_info->inband_telemetry_mask_fid;
            info->present   = BCM_PKT_FORMAT_INT_PKT_PRESENT;
            info->none      = BCM_PKT_FORMAT_INT_PKT_NONE;
            info->any       = BCM_PKT_FORMAT_INT_PKT_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeOuterIFA:
            info->field     = tbls_info->outer_ifa_hdr_fid;
            info->mask      = tbls_info->outer_ifa_hdr_mask_fid;
            info->present   = BCM_PKT_FORMAT_IFA_PRESENT;
            info->none      = BCM_PKT_FORMAT_IFA_NONE;
            info->any       = BCM_PKT_FORMAT_IFA_ANY;
            break;
        case bcmiXgsUdfPktFormatMiscTypeInnerIFA:
            info->field     = tbls_info->inner_ifa_hdr_fid;
            info->mask      = tbls_info->inner_ifa_hdr_mask_fid;
            info->present   = BCM_PKT_FORMAT_IFA_PRESENT;
            info->none      = BCM_PKT_FORMAT_IFA_NONE;
            info->any       = BCM_PKT_FORMAT_IFA_ANY;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_misc_add(
    int unit,
    bcmi_xgs_udf_pkt_format_misc_type_t type,
    uint16 flags,
    bcmlt_entry_handle_t *hdl)
{
    uint64_t data, mask;
    bcmi_xgs_udf_pkt_format_misc_type_info_t info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_misc_info_get(unit, type, &info));

    if ((flags & info.present) && (flags & info.none)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((flags & info.present) || (flags & info.none)) {
        data = (flags & info.present);
        mask = 1;
    } else {
        data = mask = 0;
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl, info.field, data));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl, info.mask, mask));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_l4dstport_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->l4dst_port_fid, format->l4_dst_port));
    SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                tbls_info->l4dst_port_mask_fid, format->l4_dst_port_mask));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_opaque_tag_type_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl,
                               tbls_info->opaque_tag_fid,
                               format->opaque_tag_type));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl,
                               tbls_info->opaque_tag_mask_fid,
                               format->opaque_tag_type_mask));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_src_port_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl,
                               tbls_info->port_id_fid,
                               format->src_port));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl,
                               tbls_info->port_id_mask_fid,
                               format->src_port_mask));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_pkt_loopback_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    const char *data_sym = NULL;
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_EXIT
        (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                    tbls_info->lb_pkt_type_fid,
                                    format->lb_pkt_type, &data_sym));

    SHR_IF_ERR_EXIT
        (bcmi_lt_field_def_get(unit, tbls_info->udf_sid,
                               tbls_info->lb_pkt_type_mask_fid,
                               &fdef));

    SHR_IF_ERR_EXIT(bcmlt_entry_field_symbol_add(*hdl,
                tbls_info->lb_pkt_type_fid, data_sym));

    if (format->lb_pkt_type == BCM_PKT_FORMAT_LB_TYPE_ANY) {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->lb_pkt_type_mask_fid, fdef.def));
    } else {
        SHR_IF_ERR_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->lb_pkt_type_mask_fid, fdef.max));
    }

exit:
    if (SHR_FUNC_ERR()) {
        LOG_DEBUG(BSL_LS_BCM_UDF,
                (BSL_META_U(unit, "Error: Invalid parameter value"
                            " for pkt_format->lb_pkt_type.\n")));
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_ethertype_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    int rv;
    uint64_t data;
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    if (format->ethertype_mask) {
        /* This fields might be configured as part of L3_FIELDS */
        rv = bcmlt_entry_field_get(*hdl, tbls_info->ethertype_fid, &data);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_remove(*hdl, tbls_info->ethertype_fid));
        }
        rv = bcmlt_entry_field_get(*hdl, tbls_info->ethertype_mask_fid, &data);
        if (rv == SHR_E_NONE) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_remove(*hdl, tbls_info->ethertype_mask_fid));
        }

        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(*hdl,
                    tbls_info->ethertype_fid, format->ethertype));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*hdl,
                                   tbls_info->ethertype_mask_fid,
                                   format->ethertype_mask));
    } else {
        rv = bcmlt_entry_field_get(*hdl, tbls_info->ethertype_fid, &data);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                                       tbls_info->udf_sid,
                                       tbls_info->ethertype_fid, &fdef));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(*hdl,
                                       tbls_info->ethertype_fid, fdef.def));
        }
        rv = bcmlt_entry_field_get(*hdl, tbls_info->ethertype_mask_fid, &data);
        if (rv == SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit,
                                       tbls_info->udf_sid,
                                       tbls_info->ethertype_mask_fid, &fdef));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(*hdl,
                                       tbls_info->ethertype_mask_fid, fdef.def));
        }
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_loopback_hdr_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
                               tbls_info->udf_sid,
                               tbls_info->loopback_hdr_fid,
                               &fdef));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl,
                               tbls_info->loopback_hdr_fid,
                               fdef.def));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
                               tbls_info->udf_sid,
                               tbls_info->loopback_hdr_mask_fid,
                               &fdef));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(*hdl,
                               tbls_info->loopback_hdr_mask_fid,
                               fdef.def));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_fields_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmlt_entry_handle_t *hdl)
{
    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_l2format_add(unit, format, hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_vlanformat_add(unit, format, hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_l3fields_add(unit, format, hdl));

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_HIGIG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeHigig,
                                      format->higig, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_VNTAG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeVntag,
                                      format->vntag, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_ETAG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeEtag,
                                      format->etag, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_CNTAG) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeCntag,
                                      format->cntag, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_INT_PKT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeIntPkt,
                                      format->int_pkt, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_OUTER_IFA_HDR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeOuterIFA,
                                      format->outer_ifa, hdl));
    }
    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_INNER_IFA_HDR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_misc_add(unit,
                                      bcmiXgsUdfPktFormatMiscTypeInnerIFA,
                                      format->inner_ifa, hdl));
    }
    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_L4DST_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_l4dstport_add(unit, format, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags &
            BCMI_XGS_UDF_LT_PKT_FORMAT_OPAQUE_TAG_TYPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_opaque_tag_type_add(unit, format, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_OPAQUE_TAG_TYPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_opaque_tag_type_add(unit, format, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_SRC_PORT) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_src_port_add(unit, format, hdl));
    }

    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_TYPE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_pkt_loopback_add(unit, format, hdl));
    }
    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_HDR) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_loopback_hdr_add(unit, format, hdl));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_ethertype_add(unit, format, hdl));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_to_lt_data_get(
    int unit,
    bcmint_udf_offset_info_t *offset_info,
    const char **base_str,
    int *offset)
{
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_EXIT
        (XGS_UDF_ENUM_SYM_GET(unit)(unit, tbls_info->udf_sid,
                                    tbls_info->layer_fid,
                                    offset_info->layer,
                                    base_str));

    *offset = ((offset_info->start % \
                XGS_UDF_CTRL(unit)->max_parse_bytes)
            / XGS_UDF_CHUNK_GRAN(unit));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_data_offset_add(
    int unit,
    bool set,
    bcmint_udf_offset_info_t *offset_info,
    bcmlt_entry_handle_t *hdl)
{
    int idx, offset;
    const char *base;
    uint8 gran, max_chunks;
    uint32 hw_bmap;
    uint64_t u64offset[1];
    bcmlt_field_def_t fdef;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(offset_info, SHR_E_PARAM);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_to_lt_data_get(unit, offset_info,
                                        &base, &offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit,
                               tbls_info->udf_sid,
                               tbls_info->layer_fid, &fdef));

    gran = XGS_UDF_CHUNK_GRAN(unit);
    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    for (idx = 0; idx < max_chunks; idx++) {

        hw_bmap = 0;
        XGS_UDF_CHUNK_BYTE_BMAP_GET(&offset_info->byte_bmap,
                idx, gran, &hw_bmap);
        if (hw_bmap != 0) {
            if (set == true) {
                u64offset[0] = offset;
            } else {
                u64offset[0] = 0;
                base = fdef.sym_def;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(*hdl,
                                             tbls_info->offset_fid,
                                             idx, u64offset, 1));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(*hdl,
                                                    tbls_info->layer_fid,
                                                    idx, &base, 1));

            offset++;
        }
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_entry_commit(
    int unit,
    int set,
    bcmlt_opcode_t opcode,
    bcmlt_priority_level_t priority,
    bcmint_udf_offset_info_t *offset_info,
    bcmint_udf_pkt_format_info_t *format_info)
{
    int dunit;
    bcmlt_entry_handle_t handle;
    bcm_udf_oper_mode_t mode;
    bcm_udf_pkt_format_info_t format;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbls_info->udf_sid, &handle));

    bcm_udf_pkt_format_info_t_init(&format);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_get(unit, format_info, &format));

    /* UDF LT Key fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_fields_add(unit, &format, &handle));

    /* UDF LT Data Offset fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_data_offset_add(unit, set, offset_info, &handle));

    /* Pipe */
    SHR_IF_ERR_VERBOSE_EXIT(xgs_ltsw_udf_oper_mode_get(unit, &mode));
    if (mode == bcmUdfOperModePipeLocal) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, "PIPE", offset_info->pipe));
    }

    /* Udf Class */
    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_OUTPUT_CLASS_ID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle,
                                   tbls_info->class_id_fid,
                                   format_info->class_id));
    }

    /* Udf Flex conditional */
    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_OUTPUT_FLEX_HASH) {
        set = (offset_info->flags & BCMI_UDF_OFFSET_INFO_FLEXHASH) ? 1 : 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(handle, FLEX_HASHs, set));
    }

    /* Skip Outer IFA metadata */
    if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_OFFSET_SKIP_OUTER_IFA_MD) {
        set = (format_info->flags &
                BCMI_UDF_PKT_FMT_INFO_SKIP_OUTER_IFA_MD) ? 1 : 0;
        SHR_IF_ERR_VERBOSE_EXIT(bcmlt_entry_field_add(handle,
                    tbls_info->skip_outer_ifa_md_fid, set));
    }

    /*  Entry Priority */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(handle, "ENTRY_PRIORITY", format_info->priority));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, handle, opcode, priority));

exit:

    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_data_hash_mask_add(
    int unit,
    bcmi_xgs_udf_lt_hash_config_t *lt_hash_config,
    bcmlt_entry_handle_t *hdl)
{
    int idx;
    const char *hash_field_str;
    bcmi_xgs_udf_hash_chunks_info_t *hash_chunks_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(hdl, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_hash_config, SHR_E_PARAM);

    hash_chunks_info = &(XGS_UDF_CTRL(unit)->hash_chunks_info);

    for (idx = 0; idx < lt_hash_config->chunks; idx++) {

        if (lt_hash_config->chunk_en[idx] == false) {
            continue;
        }

        hash_field_str = hash_chunks_info->field_str[idx];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(*hdl, hash_field_str,
                                   lt_hash_config->mask[idx]));
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_hash_config_add(
    int unit,
    bcmint_udf_pkt_format_info_t *format_info,
    bcmi_xgs_udf_lt_hash_config_t *lt_hash_config,
    bcmlt_entry_handle_t *hdl)
{
    bcm_udf_pkt_format_info_t format;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(format_info, SHR_E_PARAM);
    SHR_NULL_CHECK(lt_hash_config, SHR_E_PARAM);

    bcm_udf_pkt_format_info_t_init(&format);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_get(unit, format_info, &format));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_fields_add(unit, &format, hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_data_hash_mask_add(unit, lt_hash_config, hdl));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_hash_config_set(
    int unit,
    bcmi_xgs_udf_lt_hash_config_t *lt_hash_config,
    bcmint_udf_offset_info_t *offset_info)
{
    int dunit;
    int idx, j;
    bool trans_valid;
    uint16 max_pkt_formats;
    bcmlt_opcode_t opcode;
    bcmlt_entry_handle_t entry_hdl;
    bcmlt_transaction_hdl_t trans_hdl;
    bcmint_udf_pkt_format_info_t *temp = NULL;
    bcmint_udf_pkt_format_info_t *format_info_array;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    opcode = BCMLT_OPCODE_UPDATE;
    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trans_hdl));

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;

    for (idx = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }

        for (j = 0; j < UDF_MAX_ID_LIST; j++) {
            if (offset_info->udf_id != temp->udf_id_list[j]) {
                continue;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, tbls_info->udf_sid, &entry_hdl));

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_udf_lt_hash_config_add(unit, temp,
                                             lt_hash_config, &entry_hdl));

            /* Add entry handle to trasaction handle */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_transaction_entry_add(trans_hdl, opcode, entry_hdl));

            entry_hdl = BCMLT_INVALID_HDL;
            trans_valid = true;
            break;
        }
    }

    if (trans_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_transaction_commit(trans_hdl, BCMLT_PRIORITY_NORMAL));
    }

exit:

    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    if (trans_hdl != BCMLT_INVALID_HDL) {
        bcmlt_transaction_free(trans_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_hash_config_get(
    int unit,
    bcmint_udf_offset_info_t *offset_info,
    bcmi_xgs_udf_lt_hash_config_t *lt_hash_config)
{
    int idx, j;
    int dunit;
    bool found;
    uint16 max_pkt_formats;
    uint64_t u64mask;
    const char *hash_field_str;
    bcm_udf_pkt_format_info_t format;
    bcmlt_entry_handle_t entry_hdl;
    bcmint_udf_pkt_format_info_t *temp = NULL;
    bcmint_udf_pkt_format_info_t *format_info_array;
    bcmi_xgs_udf_hash_chunks_info_t *hash_chunks_info;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;

    SHR_FUNC_ENTER(unit);

    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;
    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;

    for (idx = 0, found = false;
        ((idx < max_pkt_formats) && (found == false)); idx++) {
        temp = &format_info_array[idx];

        if (!(temp->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }

        for (j = 0; j < UDF_MAX_ID_LIST; j++) {
            if (offset_info->udf_id == temp->udf_id_list[j]) {
                found = true;
                break;
            }
        }
    }

    if (found == false) {
        SHR_EXIT();
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, tbls_info->udf_sid, &entry_hdl));

    bcm_udf_pkt_format_info_t_init(&format);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_get(unit, temp, &format));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_key_fields_add(unit, &format, &entry_hdl));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(entry_hdl,
                            BCMLT_OPCODE_LOOKUP,
                            BCMLT_PRIORITY_NORMAL));

    hash_chunks_info = &(XGS_UDF_CTRL(unit)->hash_chunks_info);
    sal_memset(lt_hash_config, 0, sizeof (bcmi_xgs_udf_lt_hash_config_t));

    for (idx = 0; idx < hash_chunks_info->chunks; idx++) {
        hash_field_str = hash_chunks_info->field_str[idx];

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(entry_hdl, hash_field_str, &u64mask));

        lt_hash_config->mask[idx] = u64mask;
    }

    lt_hash_config->chunks = hash_chunks_info->chunks;

exit:

    if (entry_hdl != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(entry_hdl);
    }
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_port_pipe_get(int unit,
        bcm_pbmp_t pbmp, uint8_t *udf_pipe)
{
    uint8_t pipe = 0;
    bcm_pbmp_t pc_pbmp;
    int num_pipe = 0;
    bcm_udf_oper_mode_t mode;
    bcm_port_config_t port_config;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (xgs_ltsw_udf_oper_mode_get(unit, &mode));

    /* Retrieve port configuration */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    if (mode == bcmUdfOperModePipeLocal) {

        /* per-pipe pbm */
        num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);

        for (pipe = 0; pipe < num_pipe; pipe++) {
            BCM_PBMP_ASSIGN(pc_pbmp, port_config.per_pp_pipe[pipe]);
            if (BCM_PBMP_EQ(pbmp, pc_pbmp)) {
                break;
            }
        }

        if (pipe == num_pipe) {
            LOG_DEBUG(BSL_LS_BCM_FP,
               (BSL_META_U(unit, "Error: Unable to find pipe"
                   " for provided group port.\n")));
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }

        if (udf_pipe != NULL) {
            *udf_pipe = pipe;
        }
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_info_add(
    int unit,
    bcm_udf_t *udf_info,
    bcmint_udf_offset_info_t **offset_info)
{
    int idx;
    uint16 max_udfs;
    bcmint_udf_offset_info_t *temp = NULL;
    bcmint_udf_offset_info_t *offset_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(udf_info, SHR_E_PARAM);
    SHR_NULL_CHECK(offset_info, SHR_E_PARAM);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;
    SHR_NULL_CHECK(offset_info_array, SHR_E_INTERNAL);

    /* Search for free index */
    for (idx = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            break;
        }
    }

    if (idx >= max_udfs) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    SHR_NULL_CHECK(temp, SHR_E_INTERNAL);

    temp->layer = udf_info->layer;
    temp->start = BITS2BYTES(udf_info->start);
    temp->width = BITS2BYTES(udf_info->width);
    temp->flags |= BCMI_UDF_OFFSET_INFO_IN_USE;

    SHR_IF_ERR_EXIT(ltsw_udf_offset_port_pipe_get(unit,
                udf_info->ports, (uint8 *) &temp->pipe));

    *offset_info = temp;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcmint_udf_offset_info_t **offset_info)
{
    int idx;
    uint16 max_udfs;
    bcmint_udf_offset_info_t *temp = NULL;
    bcmint_udf_offset_info_t *offset_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(offset_info, SHR_E_PARAM);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;
    SHR_NULL_CHECK(offset_info_array, SHR_E_INTERNAL);

    for (idx = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            continue;
        }
        if (temp->udf_id == udf_id) {
            break;
        }
    }

    if (idx >= max_udfs) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_NULL_CHECK(temp, SHR_E_INTERNAL);

    *offset_info = temp;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_info_delete(
    int unit,
    bcm_udf_id_t udf_id)
{
    int idx;
    uint16 max_udfs;
    bcmint_udf_offset_info_t *temp = NULL;
    bcmint_udf_offset_info_t *offset_info_array;

    SHR_FUNC_ENTER(unit);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;
    SHR_NULL_CHECK(offset_info_array, SHR_E_INTERNAL);

    for (idx = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            continue;
        }
        if (temp->udf_id == udf_id) {
            break;
        }
    }

    if (idx >= max_udfs) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_NULL_CHECK(temp, SHR_E_INTERNAL);

    sal_memset(temp, 0, sizeof (bcmint_udf_offset_info_t));

exit:
    SHR_FUNC_EXIT();
}

static int
ltsw_udf_pkt_format_info_add(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmint_udf_pkt_format_info_t **format_info)
{
    int idx;
    uint16 max_pkt_formats;
    bcmint_udf_pkt_format_info_t *temp = NULL;
    bcmint_udf_pkt_format_info_t *format_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(format, SHR_E_PARAM);
    SHR_NULL_CHECK(format_info, SHR_E_PARAM);

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;
    SHR_NULL_CHECK(format_info_array, SHR_E_INTERNAL);

    for (idx = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp ->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            break;
        }
    }

    if (idx >= max_pkt_formats) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }
    SHR_NULL_CHECK(temp, SHR_E_INTERNAL);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_set(unit, format, temp));

    temp->flags |= BCMI_UDF_PKT_FMT_INFO_IN_USE;

    *format_info = temp;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_pkt_format_info_delete(
    int unit,
    bcm_udf_pkt_format_id_t pkt_fmt_id)
{
    int idx;
    uint16 max_pkt_formats;
    bcmint_udf_pkt_format_info_t *temp = NULL;
    bcmint_udf_pkt_format_info_t *format_info_array;

    SHR_FUNC_ENTER(unit);

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;
    SHR_NULL_CHECK(format_info_array, SHR_E_INTERNAL);

    for (idx = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp ->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }
        if (temp->pkt_fmt_id == pkt_fmt_id) {
            break;
        }
    }

    if (idx >= max_pkt_formats) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_NULL_CHECK(temp, SHR_E_INTERNAL);

    sal_memset(temp, 0, sizeof (bcmint_udf_pkt_format_info_t));

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_fmt_id,
    bcmint_udf_pkt_format_info_t **format_info)
{
    int idx;
    uint16 max_pkt_formats;
    bcmint_udf_pkt_format_info_t *temp = NULL;
    bcmint_udf_pkt_format_info_t *format_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(format_info, SHR_E_PARAM);

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;
    SHR_NULL_CHECK(format_info_array, SHR_E_INTERNAL);

    for (idx = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp ->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }
        if (temp->pkt_fmt_id == pkt_fmt_id) {
            break;
        }
    }

    if (idx >= max_pkt_formats) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }
    SHR_NULL_CHECK(temp, SHR_E_INTERNAL);

    *format_info = temp;

exit:

    SHR_FUNC_EXIT();
}

static bool
ltsw_pkt_format_info_compare(
    bcmint_udf_pkt_format_info_t *a,
    bcmint_udf_pkt_format_info_t *b)
{
    if ((a->ethertype & a->ethertype_mask) !=
            (b->ethertype & b->ethertype_mask)) {
        return false;
    }
    if ((a->ip_protocol & a->ip_protocol_mask) !=
            (b->ip_protocol & b->ip_protocol_mask)) {
        return false;
    }
    if ((a->l2 != b->l2) ||
            (a->vlan_tag != b->vlan_tag) ||
            (a->outer_ip != b->outer_ip) ||
            (a->inner_ip != b->inner_ip) ||
            (a->tunnel != b->tunnel) ||
            (a->mpls != b->mpls) ||
            (a->misc_en_fields != b->misc_en_fields)) {
        return false;
    }
    if ((a->inner_protocol & a->inner_protocol_mask) !=
            (b->inner_protocol & b->inner_protocol_mask)) {
        return false;
    }
    if ((a->l4_dst_port & a->l4_dst_port_mask) !=
            (b->l4_dst_port & b->l4_dst_port_mask)) {
        return false;
    }
    if ((a->src_port & a->src_port_mask) !=
            (b->src_port & b->src_port_mask)) {
        return false;
    }
    if ((a->first_2B_after_mpls_bos & a->first_2B_after_mpls_bos_mask) !=
            (b->first_2B_after_mpls_bos & b->first_2B_after_mpls_bos_mask)) {
        return false;
    }

    return true;
}

static int
ltsw_udf_pkt_format_info_search(
    int unit,
    bcm_udf_pkt_format_info_t *format,
    bcmint_udf_pkt_format_info_t **matched_format_info)
{
    int idx;
    bool matched;
    uint16 max_pkt_formats;
    bcmint_udf_pkt_format_info_t *temp = NULL;
    bcmint_udf_pkt_format_info_t format_info;
    bcmint_udf_pkt_format_info_t *format_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(format, SHR_E_PARAM);
    SHR_NULL_CHECK(matched_format_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_set(unit, format, &format_info));

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;
    SHR_NULL_CHECK(format_info_array, SHR_E_INTERNAL);

    matched = false;
    for (idx = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }

        matched = ltsw_pkt_format_info_compare(&format_info, temp);
        if (matched == true) {
            break;
        }
    }

    if (idx >= max_pkt_formats) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    *matched_format_info = temp;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_entry_reserve(
    int unit,
    int pipe,
    int max_chunks,
    uint32 *offset_array)
{
    int idx;
    bool first;
    uint8 gran;
    SHR_BITDCL *byte_bmap;
    SHR_BITDCL *chunk_bmap;
    bcmi_xgs_udf_offset_entry_t *offset_entry;
    bcmi_xgs_udf_offset_entry_t *offset_entry_arr;

    SHR_FUNC_ENTER(unit);

    chunk_bmap = &(XGS_UDF_CTRL(unit)->hw_bmap[pipe]);
    byte_bmap = &(XGS_UDF_CTRL(unit)->byte_bmap[pipe]);
    offset_entry_arr = XGS_UDF_CTRL(unit)->offset_entry_arr[pipe];

    first = true;
    gran = XGS_UDF_CHUNK_GRAN(unit);

    for (idx = 0; idx < max_chunks; idx++) {
        if (offset_array[idx] == 0) {
            continue;
        }

        offset_entry = &(offset_entry_arr[idx]);

        /* Incr ref_cnt on the offset entry */
        offset_entry->num_udfs += 1;

        if (offset_entry->num_udfs == 1) {

            /* Mark the entries as used */
            SHR_BITSET(chunk_bmap, idx);
            *byte_bmap |= (offset_array[idx] << (idx * gran));
        }

        if (first == true) {
            offset_entry->flags |= BCMI_XGS_UDF_OFFSET_ENTRY_GROUP;
            offset_entry->grp_id = idx;
            first = false;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_entry_unreserve(
    int unit,
    int pipe,
    int max_chunks,
    uint32 *offset_array)
{
    int idx;
    uint8 gran;
    SHR_BITDCL *byte_bmap;
    SHR_BITDCL *chunk_bmap;
    bcmi_xgs_udf_offset_entry_t *offset_entry;
    bcmi_xgs_udf_offset_entry_t *offset_entry_arr;

    SHR_FUNC_ENTER(unit);

    chunk_bmap = &(XGS_UDF_CTRL(unit)->hw_bmap[pipe]);
    byte_bmap = &(XGS_UDF_CTRL(unit)->byte_bmap[pipe]);
    offset_entry_arr = XGS_UDF_CTRL(unit)->offset_entry_arr[pipe];

    gran = XGS_UDF_CHUNK_GRAN(unit);

    for (idx = 0; idx < max_chunks; idx++) {
        if (offset_array[idx] == 0) {
            continue;
        }

        offset_entry = &(offset_entry_arr[idx]);

        /* Decr ref_cnt on the offset entry */
        offset_entry->num_udfs -= 1;

        if (offset_entry->num_udfs == 0) {

            /* Mark the entries as free */
            SHR_BITCLR(chunk_bmap, idx);
            *byte_bmap &= ~(offset_array[idx] << (idx * gran));

            /* Clear the UDF Offset entry */
            offset_entry->flags = 0x0;
            offset_entry->grp_id = 0x0;
        }
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_control_info_recover(int unit)
{
    int idx, cidx, pipe;
    bool first;
    uint8 gran;
    uint16 max_udfs, max_chunks;
    uint32 cbmap, *used_byte_bmap;
    bcmint_udf_offset_info_t *temp = NULL;
    bcmint_udf_offset_info_t *offset_info_array;
    bcmi_xgs_udf_offset_entry_t *offset_entry = NULL;
    bcmi_xgs_udf_offset_entry_t *offset_entry_array;

    SHR_FUNC_ENTER(unit);

    gran = XGS_UDF_CHUNK_GRAN(unit);
    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;
    SHR_NULL_CHECK(offset_info_array, SHR_E_INTERNAL);

    for (idx = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            continue;
        }

        pipe = temp->pipe;
        used_byte_bmap = &(XGS_UDF_CTRL(unit)->byte_bmap[pipe]);
        offset_entry_array = XGS_UDF_CTRL(unit)->offset_entry_arr[pipe];

        offset_entry->grp_id = temp->grp_id;

        first = true;
        for (cidx = 0; cidx < max_chunks; cidx++) {
            cbmap = 0;
            XGS_UDF_CHUNK_BYTE_BMAP_GET(&(temp->byte_bmap),
                    cidx, gran, &cbmap);

            if (cbmap != 0) {
                offset_entry = &offset_entry_array[cidx];
                offset_entry->grp_id = temp->grp_id;
                offset_entry->num_udfs++;

                if (first == true) {
                    offset_entry->flags |= BCMI_XGS_UDF_OFFSET_ENTRY_GROUP;
                    if (offset_entry->num_udfs > 0) {
                        offset_entry->flags |= BCMI_XGS_UDF_OFFSET_ENTRY_SHARED;
                    }
                    first = false;
                } else {
                    offset_entry->flags |= BCMI_XGS_UDF_OFFSET_ENTRY_PART;
                }
            }
        }

        *used_byte_bmap |= (temp->byte_bmap);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_lt_key_fields_check(
    int unit,
    bcm_udf_pkt_format_info_t *format)
{
    uint32 lt_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(format, SHR_E_PARAM);

    lt_flags = XGS_UDF_CTRL(unit)->lt_flags;

    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_HIGIG) &&
            (format->higig > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_VNTAG) &&
            (format->vntag > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_ETAG) &&
            (format->etag > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_ICNM) &&
            (format->icnm > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_CNTAG) &&
            (format->cntag > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_SUBPORT_TAG) &&
            (format->subport_tag > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_INT_PKT) &&
            (format->int_pkt > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_OPAQUE_TAG_TYPE) &&
            (format->opaque_tag_type_mask & format->opaque_tag_type)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_L4DST_PORT) &&
            (format->l4_dst_port_mask & format->l4_dst_port)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_SRC_PORT) &&
            (format->src_port_mask & format->src_port)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    if (!(lt_flags & BCMI_XGS_UDF_LT_PKT_FORMAT_LOOPBACK_TYPE) &&
            (format->lb_pkt_type > 0)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_obj_input_sanitize(
    int unit,
    bcm_udf_t *udf_info,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_pkt_format_info_t *pkt_format)
{
    int dunit;
    uint64_t value_min, value_max;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;
    bcmlt_entry_handle_t entry_hdl = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    if (udf_info != NULL) {

        /* start and width should be at byte boundary */
        if (((udf_info->start % 8) != 0) || ((udf_info->width % 8) != 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* start and width should be less than max byte selection */
        if (BITS2BYTES(udf_info->start + udf_info->width) >
                XGS_UDF_CTRL(unit)->max_parse_bytes) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* flags in udf_info is currently not used; must be 0 */
        if (udf_info->flags != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* check if layer is in known range */
        if ((udf_info->layer < bcmUdfLayerL2Header) ||
                (udf_info->layer >= bcmUdfLayerCount)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (udf_info->layer == bcmUdfLayerUserPayload) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_EXIT(
                ltsw_udf_offset_port_pipe_get(unit, udf_info->ports, NULL));
    }

    if (hints != NULL) {
        /* check if chip support flex hash */
        if ((hints->flags & BCM_UDF_CREATE_O_FLEXHASH) &&
                (!(UDF_CONTROL(unit)->info.flags &
                   BCMINT_UDF_FLEXHASH_SUPPORTED))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* check if chip support policer group */
        if ((hints->flags & BCM_UDF_CREATE_O_POLICER_GROUP) &&
                (!(UDF_CONTROL(unit)->info.flags &
                   BCMINT_UDF_POLICER_GROUP_SUPPORTED))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* check if chip support udf hash */
        if ((hints->flags & BCM_UDF_CREATE_O_UDFHASH) &&
                (!(UDF_CONTROL(unit)->info.flags &
                   BCMINT_UDF_UDFHASH_SUPPORTED))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* check create udf if withid is used together with replace*/
        if (((hints->flags & BCM_UDF_CREATE_O_WITHID) &&
                    !(hints->flags & BCM_UDF_CREATE_O_REPLACE)) ||
                (!(hints->flags & BCM_UDF_CREATE_O_WITHID) &&
                 (hints->flags & BCM_UDF_CREATE_O_REPLACE))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* shared udf id should be valid */
        if (hints->flags & BCM_UDF_CREATE_O_SHARED_HWID) {
            BCMINT_UDF_ID_VALIDATE(unit, hints->shared_udf);
        }
        /* check if chip supports Range Check */
        if ((hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) &&
                (!(UDF_CONTROL(unit)->info.flags &
                   BCMINT_UDF_RANGE_CHECK_SUPPORTED))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        /* check if chip support flex counter */
        if ((hints->flags & BCM_UDF_CREATE_O_FLEX_COUNTER) &&
                (!(UDF_CONTROL(unit)->info.flags &
                   BCMINT_UDF_FLEXCTR_SUPPORTED))) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (pkt_format != NULL) {

        tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, tbls_info->udf_sid,
                                           "ENTRY_PRIORITY",
                                           &value_min, &value_max));
        if ((pkt_format->prio < value_min) ||
                (pkt_format->prio > value_max)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_fields_check(unit, pkt_format));

        if (XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_OUTPUT_CLASS_ID) {

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_value_range_get(unit, tbls_info->udf_sid,
                                               tbls_info->class_id_fid,
                                               &value_min, &value_max));

            if ((pkt_format->class_id < value_min) ||
                    (pkt_format->class_id > value_max)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        dunit = bcmi_ltsw_dev_dunit(unit);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, tbls_info->udf_sid, &entry_hdl));

        /* UDF LT Key fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_fields_add(unit, pkt_format, &entry_hdl));
    }

exit:

    if (entry_hdl != BCMLT_INVALID_HDL) {
        (void ) bcmlt_entry_free(entry_hdl);
    }

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_appl_offset_info_get(
    int unit,
    int req_offsets,
    uint8 pipe,
    bcm_udf_alloc_hints_t *hints,
    uint8 *offset_order,
    int *actual)
{
    int idx, num_chunks;
    bool use_udf1, use_udf2;
    uint8 udf1_free_chunks;
    uint8 udf2_free_chunks;
    uint8 *appl_offset_order;
    uint32 hw_bmap[1];
    bcmi_xgs_udf_appl_chunks_info_t *appl_chunks_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);
    SHR_NULL_CHECK(offset_order, SHR_E_PARAM);

    appl_chunks_info = &(XGS_UDF_CTRL(unit)->appl_chunks_info);

    *actual = 0;
    if ((hints != NULL) &&
            (hints->flags & BCM_UDF_CREATE_O_FLEXHASH)) {

        *actual = appl_chunks_info->flexhash_chunks;
        appl_offset_order = appl_chunks_info->flexhash;
        sal_memcpy(offset_order, appl_offset_order, *actual);

    } else if ((hints != NULL) &&
            (hints->flags & BCM_UDF_CREATE_O_POLICER_GROUP)) {

        *actual = appl_chunks_info->policer_chunks;
        appl_offset_order = appl_chunks_info->policer;
        sal_memcpy(offset_order, appl_offset_order, *actual);

    } else if ((hints != NULL) &&
            (hints->flags & BCM_UDF_CREATE_O_UDFHASH)) {

        *actual = appl_chunks_info->udfhash_chunks;
        appl_offset_order = appl_chunks_info->udfhash;
        sal_memcpy(offset_order, appl_offset_order, *actual);

    } else if ((hints != NULL) &&
            (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK)) {

        *actual = appl_chunks_info->rangecheck_chunks;
        appl_offset_order = appl_chunks_info->rangecheck;
        sal_memcpy(offset_order, appl_offset_order, *actual);

    } else if ((hints != NULL) &&
            ((hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) ||
             (hints->flags & BCM_UDF_CREATE_O_FIELD_LOOKUP))) {

        hw_bmap[0] = XGS_UDF_CTRL(unit)->hw_bmap[pipe];
        num_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

        udf1_free_chunks = udf2_free_chunks = 0;
        for (idx = 0; idx < num_chunks; idx++) {
            if (!SHR_BITGET(&hw_bmap[0], idx)) {
                if (idx < (num_chunks)/2) {
                    udf1_free_chunks++;
                } else {
                    udf2_free_chunks++;
                }
            }
        }

        /* verify if resources are available in order of udf1 alone,
           udf2 alone, followed by udf1 + udf2 together */
        if ((udf1_free_chunks + udf2_free_chunks) < req_offsets) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }

        use_udf1 = use_udf2 = false;
        if (udf1_free_chunks >= req_offsets) {
            use_udf1 = true;
        } else if (udf2_free_chunks >= req_offsets) {
            use_udf2 = true;
        } else {
            use_udf1 = use_udf2 = true;
        }

        *actual = idx = 0;
        while((idx < num_chunks) && (*actual < req_offsets)) {
            if ((use_udf1 == false) && (idx < (num_chunks/2))) {
                idx++;
                continue;
            }

            if (SHR_BITGET(&hw_bmap[0], idx) == 0) {
                offset_order[*actual] = idx;
                (*actual)++;
            }
            idx++;
        }
        if (*actual != req_offsets) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
    } else {

        *actual = appl_chunks_info->def_appl_chunks;
        appl_offset_order = appl_chunks_info->def_appl;
        sal_memcpy(offset_order, appl_offset_order, *actual);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_offset_chunk_alloc(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcmint_udf_offset_info_t *offset_info)
{
    int i, j, alloc_cnt;
    int byte_offset, req_offsets;
    int num_offsets, widthcovergbl, widthcoverint;
    uint8 gran, max_chunks, grp_id;
    uint32 hw_bmap[1];
    uint32 alloc_bmap, byte_bmap;
    uint8 offset_order[UDF_MAX_CHUNKS];
    uint32 offset_array[UDF_MAX_CHUNKS];

    SHR_FUNC_ENTER(unit);

    gran = XGS_UDF_CHUNK_GRAN(unit);
    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    byte_offset = offset_info->start % gran;
    req_offsets = (offset_info->width + byte_offset + gran - 1) / gran;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_appl_offset_info_get(unit, req_offsets, offset_info->pipe,
                                       hints, offset_order, &num_offsets));

    hw_bmap[0] = XGS_UDF_CTRL(unit)->hw_bmap[offset_info->pipe];

    /* All chunks are reserved */
    if ((hw_bmap[0] & 0xFFFF) == 0xFFFF) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    grp_id = -1;
    alloc_cnt = alloc_bmap = byte_bmap = 0;
    widthcovergbl = widthcoverint = 0;
    sal_memset(offset_array, 0, UDF_MAX_CHUNKS * sizeof(uint32));
    for (j = 0; j < num_offsets; j++) {
        if ((SHR_BITGET(&(hw_bmap[0]), offset_order[j]))) {
            offset_array[offset_order[j]] = 0;
            continue;
        }

        /* Mark the first offset entry as the grp_id */
        if (grp_id == -1) {
            grp_id = offset_order[j];
        } else if (grp_id > offset_order[j]) {
            grp_id = offset_order[j];
        }

        /* offset_order will have order of chunks to be allocated.
         * offset_array should be filled with 0x1, 0x2 or 0x3 if
         * first half, or second half or full chunk is alloc'ed respy.
         */
        alloc_bmap |= (1 << offset_order[j]);
        if (alloc_cnt == 0) {
            /* First Chunk to be allocated */
            for(i = 0;i< (gran - byte_offset); i++) {
                if (widthcovergbl < offset_info->width) {
                    byte_bmap |= (0x1 << (((offset_order[j]+1) * gran) - byte_offset -i -1));
                    offset_array[offset_order[j]] |= (0x1 << (gran - byte_offset -i -1));
                    widthcovergbl++;
                }
            }
        } else {
            /* Rest all chunk to be allocated */
            widthcoverint = 0;
            for(i = 0;(i <(offset_info->width - widthcovergbl) && (i < gran)); i++){
                byte_bmap |= (0x1 << (((offset_order[j]+1) * gran) -i -1));
                offset_array[offset_order[j]] |= (0x1 << (gran -i -1));
                widthcoverint++;
            }
            widthcovergbl += widthcoverint;
        }

        if (++alloc_cnt == req_offsets) {
            break;
        }
    }

    if (alloc_cnt < req_offsets) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_entry_reserve(unit, offset_info->pipe,
                                       max_chunks, offset_array));
    offset_info->grp_id = grp_id;
    offset_info->byte_bmap |= byte_bmap;

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_hash_config_set(
    int unit,
    bcm_udf_hash_config_t *config,
    bcmint_udf_offset_info_t *offset_info)
{
    int i, j, chunk_id;
    uint8 gran, *byte_order;
    uint32 hash_mask, chunk_bmap;
    uint16 chunk_mask[BCMI_XGS_UDF_HASH_CHUNKS_MAX];
    bcmi_xgs_udf_lt_hash_config_t lt_hash_config;
    bcmi_xgs_udf_hash_chunks_info_t *hash_chunks_info;

    SHR_FUNC_ENTER(unit);

    gran = XGS_UDF_CHUNK_GRAN(unit);
    hash_chunks_info = &(XGS_UDF_CTRL(unit)->hash_chunks_info);

    if ((!(offset_info->flags & BCMI_UDF_OFFSET_INFO_UDFHASH)) ||
        (offset_info->width != config->mask_length)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    for (i = 0; i < hash_chunks_info->chunks; i++) {
        chunk_id = hash_chunks_info->chunk_ids[i];

        chunk_bmap = 0;
        XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                chunk_id, gran, &chunk_bmap);
        if (chunk_bmap != 0) {
            break;
        }
    }
    if (i >= hash_chunks_info->chunks) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    hash_mask = 0;
    byte_order = &hash_chunks_info->byte_order[0];

    /* mask[0]~[3] reflect to the order in byte_map */
    for (i = 0, j = 0; i < hash_chunks_info->bytes; i++) {
        if (offset_info->byte_bmap & (1 << byte_order[i])) {
            hash_mask |= (config->hash_mask[j++] << (8 * (3 - i)));
        }
    }

    chunk_mask[1] = hash_mask & 0x0000FFFF;
    chunk_mask[0] = (hash_mask & 0xFFFF0000) >> 16;

    for (i = 0; i < hash_chunks_info->chunks; i++) {
        chunk_id = hash_chunks_info->chunk_ids[i];

        chunk_bmap = 0;
        XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                chunk_id, gran, &chunk_bmap);
        if (chunk_bmap != 0) {
            lt_hash_config.mask[i] = chunk_mask[i];
            lt_hash_config.chunk_en[i] = true;
        } else {
            lt_hash_config.mask[i] = 0;
            lt_hash_config.chunk_en[i] = false;
        }
    }
    lt_hash_config.chunks = hash_chunks_info->chunks;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_hash_config_set(unit, &lt_hash_config, offset_info));

exit:

    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public functions
 */
int
xgs_ltsw_udf_control_alloc(int unit)
{
    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_MEMALLOC(XGS_UDF_CTRL(unit),
            sizeof(bcmi_xgs_udf_control_info_t),
            "xgsUdfCtrl");

    if (!XGS_UDF_CTRL(unit)) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_control_free(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (XGS_UDF_CTRL(unit)) {
        BCMINT_UDF_MEMFREE(XGS_UDF_CTRL(unit));
        XGS_UDF_CTRL(unit) = NULL;
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_init(int unit)
{
    int warm, pipe, num_pipes;
    uint16 noffsets, max_udfs;
    uint16 max_pkt_formats;
    uint32 nentries, ha_instance_size;
    uint32 ha_alloc_size, ha_req_size;
    bcm_udf_oper_mode_t mode;

    SHR_FUNC_ENTER(unit);

    warm = bcmi_warmboot_get(unit);

    SHR_IF_ERR_VERBOSE_EXIT(bcmi_lt_capacity_get(unit, UDFs, &nentries));

    XGS_UDF_CTRL(unit)->max_entries = nentries;
    XGS_UDF_CTRL(unit)->max_udfs = nentries;
    XGS_UDF_CTRL(unit)->max_pkt_formats = nentries;

    noffsets = UDF_CONTROL(unit)->info.max_chunks;
    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;

    /* Get udf oper mode */
    SHR_IF_ERR_VERBOSE_EXIT(xgs_ltsw_udf_oper_mode_get(unit, &mode));

    num_pipes = bcmi_ltsw_dev_max_pipe_num(unit);

    for (pipe = 0; pipe < num_pipes; pipe++) {
        /* Allocate offset entry array */
        BCMINT_UDF_MEMALLOC(XGS_UDF_CTRL(unit)->offset_entry_arr[pipe],
                sizeof (bcmi_xgs_udf_offset_entry_t) * noffsets,
                "xgsUdfOffsetEntryArr");
    }

    SHR_IF_ERR_VERBOSE_EXIT(ltsw_udf_lt_fields_init(unit));

    /* Allocate HA for UDF object */
    ha_instance_size = sizeof (bcmint_udf_offset_info_t);
    ha_req_size = ha_alloc_size = ha_instance_size * max_udfs;

    XGS_UDF_CTRL(unit)->offset_info_array =
        (bcmint_udf_offset_info_t *) bcmi_ltsw_ha_mem_alloc(
                unit,
                BCMI_HA_COMP_ID_UDF,
                BCMINT_UDF_OBJ_SUB_COMP_ID,
                "bcmUdfObj",
                &ha_alloc_size);
    SHR_NULL_CHECK(XGS_UDF_CTRL(unit)->offset_info_array, SHR_E_MEMORY);
    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_issu_struct_info_report(unit,
                               BCMI_HA_COMP_ID_UDF,
                               BCMINT_UDF_OBJ_SUB_COMP_ID,
                               0, ha_instance_size, max_udfs,
                               BCMINT_UDF_OFFSET_INFO_T_ID),
         SHR_E_EXISTS);


    /* Allocate HA for UDF pkt format object */
    ha_instance_size = sizeof (bcmint_udf_pkt_format_info_t);
    ha_req_size = ha_alloc_size = ha_instance_size * max_pkt_formats;

    XGS_UDF_CTRL(unit)->format_info_array =
        (bcmint_udf_pkt_format_info_t *) bcmi_ltsw_ha_mem_alloc(
                unit,
                BCMI_HA_COMP_ID_UDF,
                BCMINT_UDF_PKT_FMT_OBJ_SUB_COMP_ID,
                "bcmUdfPktFmt",
                &ha_alloc_size);
    SHR_NULL_CHECK(XGS_UDF_CTRL(unit)->format_info_array, SHR_E_MEMORY);
    if (ha_alloc_size < ha_req_size) {
        SHR_ERR_EXIT(SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_issu_struct_info_report(unit,
                               BCMI_HA_COMP_ID_UDF,
                               BCMINT_UDF_PKT_FMT_OBJ_SUB_COMP_ID,
                               0, ha_instance_size, max_pkt_formats,
                               BCMINT_UDF_PKT_FORMAT_INFO_T_ID),
         SHR_E_EXISTS);


    /* Recover udf control info that can be derived */
    if (warm == true) {
        SHR_IF_ERR_VERBOSE_EXIT(ltsw_udf_control_info_recover(unit));
    } else {
        /* Reset HA memories */
        sal_memset(XGS_UDF_CTRL(unit)->offset_info_array, 0,
                sizeof (bcmint_udf_offset_info_t) * max_udfs);
        sal_memset(XGS_UDF_CTRL(unit)->format_info_array, 0,
                sizeof (bcmint_udf_pkt_format_info_t) * max_pkt_formats);
    }

exit:

    if (SHR_FUNC_ERR()) {

        if (XGS_UDF_CTRL(unit)) {
            if (XGS_UDF_CTRL(unit)->offset_info_array != NULL) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                        XGS_UDF_CTRL(unit)->offset_info_array);
                XGS_UDF_CTRL(unit)->offset_info_array = NULL;
            }
            if (XGS_UDF_CTRL(unit)->format_info_array) {
                (void)bcmi_ltsw_ha_mem_free(unit,
                        XGS_UDF_CTRL(unit)->format_info_array);
                XGS_UDF_CTRL(unit)->format_info_array = NULL;
            }
        }
        (void) xgs_ltsw_udf_detach(unit);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_detach(int unit)
{
    int pipe;
    bcmi_xgs_udf_offset_entry_t *offset_entry_arr;

    SHR_FUNC_ENTER(unit);

    XGS_UDF_CTRL(unit)->offset_info_array = NULL;
    XGS_UDF_CTRL(unit)->format_info_array = NULL;

    for (pipe = 0; pipe < UDF_MAX_PIPES; pipe++) {
        offset_entry_arr = XGS_UDF_CTRL(unit)->offset_entry_arr[pipe];
        BCMINT_UDF_MEMFREE(offset_entry_arr);
        XGS_UDF_CTRL(unit)->offset_entry_arr[pipe] = NULL;
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_create(
    int unit,
    bcm_udf_alloc_hints_t *hints,
    bcm_udf_t *udf_info,
    bcm_udf_id_t *udf_id)
{
    int idx;
    bool id_running_alloc = false;
    bool offset_info_alloc = false;
    uint8 gran, max_chunks;
    uint32 offset[16];
    bcmint_udf_offset_info_t *offset_info;
    bcmint_udf_offset_info_t *offset_info2;
    bcmi_xgs_udf_offset_entry_t *offset_entry_arr;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(udf_id, SHR_E_PARAM);
    SHR_NULL_CHECK(udf_info, SHR_E_PARAM);

    gran = XGS_UDF_CHUNK_GRAN(unit);
    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_obj_input_sanitize(unit, udf_info, hints, NULL));

    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_WITHID)) {

        BCMINT_UDF_ID_VALIDATE(unit, *udf_id);

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_offset_info_get(unit, *udf_id, &offset_info));

        if (hints->flags & BCM_UDF_CREATE_O_REPLACE) {
            /* UDF object already in use */
            if (offset_info->num_pkt_formats >= 1) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            for (idx = 0; idx < max_chunks; idx++) {
                offset[idx] = 0;
                XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                        idx, gran, &offset[idx]);
            }
            /* unreserve udf chunks */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_udf_offset_entry_unreserve(unit, offset_info->pipe,
                                                 max_chunks, offset));

            /* Delete UDF offset object node */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_udf_offset_info_delete(unit, *udf_id));
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        /* New Udf object */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_obj_running_id_alloc(unit, udf_id));
        id_running_alloc = true;
    }

    /* Add UDF offset object */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_add(unit, udf_info, &offset_info));
    offset_info_alloc = true;

    offset_info->udf_id = *udf_id;

    if ((XGS_UDF_CTRL(unit)->num_udfs + 1) ==
            XGS_UDF_CTRL(unit)->max_udfs) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if ((hints != NULL) && (hints->flags & BCM_UDF_CREATE_O_SHARED_HWID)) {

        /* Check if given shared UDF Object Id same as current UDF */
        if (*udf_id == hints->shared_udf) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_offset_info_get(unit, hints->shared_udf,
                                      &offset_info2));

        /* Validate shared udf offset info */
        if (offset_info->width != offset_info2->width) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }

        /* Shared offset should 'start' at the same byte boundary
         * Otherwise the chunks allocated may not suffice the requirement
         */
        if ((offset_info->start % gran) != (offset_info2->start % gran)) {
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }

        /* Copy fields from shared offset info */
        offset_info->grp_id = offset_info2->grp_id;
        offset_info->byte_bmap = offset_info2->byte_bmap;
        offset_info->flags |= BCMI_UDF_OFFSET_INFO_SHARED;
        offset_info2->flags |= BCMI_UDF_OFFSET_INFO_SHARED;

        /* Update udf chunks */
        offset_entry_arr = XGS_UDF_CTRL(unit)->offset_entry_arr[offset_info->pipe];
        for (idx = 0; idx < max_chunks; idx++) {
            offset[idx] = 0;
            XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                        idx, gran, &offset[idx]);
            if (offset[idx] != 0) {
                offset_entry_arr[idx].grp_id = offset_info->grp_id;
                offset_entry_arr[idx].num_udfs += 1;
                offset_entry_arr[idx].flags |= BCMI_XGS_UDF_OFFSET_ENTRY_SHARED;
            }
        }
    } else {
        /* Allocate udf chunks */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_offset_chunk_alloc(unit, hints, offset_info));
    }

    if (hints != NULL) {
        if (hints->flags & BCM_UDF_CREATE_O_FLEXHASH) {
            offset_info->flags |= BCMI_UDF_OFFSET_INFO_FLEXHASH;
        }
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_INGRESS) {
            offset_info->flags |= BCMI_UDF_OFFSET_INFO_ING_FP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_FIELD_LOOKUP) {
            offset_info->flags |= BCMI_UDF_OFFSET_INFO_VLAN_FP;
        }
        if (hints->flags & BCM_UDF_CREATE_O_POLICER_GROUP) {
            offset_info->flags |= BCMI_UDF_OFFSET_INFO_POLICER;
        }
        if (hints->flags & BCM_UDF_CREATE_O_UDFHASH) {
            offset_info->flags |= BCMI_UDF_OFFSET_INFO_UDFHASH;
        }
        if (hints->flags & BCM_UDF_CREATE_O_RANGE_CHECK) {
            offset_info->flags |= BCMI_UDF_OFFSET_INFO_RANGE_CHECK;
        }
    }

    /* Increment num udfs */
    if ((hints != NULL) && !(hints->flags & BCM_UDF_CREATE_O_REPLACE)) {
        XGS_UDF_CTRL(unit)->num_udfs += 1;
    }

exit:

    if (SHR_FUNC_ERR()) {

        if (offset_info_alloc == true) {
            (void) ltsw_udf_offset_info_delete(unit, *udf_id);
        }
        if ((id_running_alloc == 1) &&
                (UDF_CONTROL(unit)->info.udf_id_running !=
                 BCMINT_UDF_ID_MAX(unit))) {
            UDF_CONTROL(unit)->info.udf_id_running -= 1;
        }
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_destroy(
    int unit,
    bcm_udf_id_t udf_id)
{
    int idx;
    uint8 gran, max_chunks;
    uint32 offset[UDF_MAX_CHUNKS];
    bcmint_udf_offset_info_t *offset_info;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_ID_VALIDATE(unit, udf_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));

    if (offset_info->num_pkt_formats) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    gran = XGS_UDF_CHUNK_GRAN(unit);
    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    for (idx = 0; idx < max_chunks; idx++) {
        offset[idx] = 0;
        XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                idx, gran, &offset[idx]);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_entry_unreserve(unit, offset_info->pipe,
                                         max_chunks, offset));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_delete(unit, udf_id));

    /* Decrement counter */
    XGS_UDF_CTRL(unit)->num_udfs -= 1;

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_t *udf_info)
{
    bcm_port_config_t config;
    bcm_udf_oper_mode_t mode;
    bcmint_udf_offset_info_t *offset_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(udf_info, SHR_E_PARAM);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_ID_VALIDATE(unit, udf_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));

    udf_info->flags = 0;
    udf_info->start = BYTES2BITS(offset_info->start);
    udf_info->width = BYTES2BITS(offset_info->width);
    udf_info->layer = offset_info->layer;

    SHR_IF_ERR_VERBOSE_EXIT(xgs_ltsw_udf_oper_mode_get(unit, &mode));
    if (mode == bcmUdfOperModePipeLocal) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_config_get(unit, &config));
        BCM_PBMP_ASSIGN(udf_info->ports, config.per_pipe[offset_info->pipe]);
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_get_all(
    int unit,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    int idx;
    uint16 max_udfs;
    bcmint_udf_offset_info_t *temp;
    bcmint_udf_offset_info_t *offset_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    BCMINT_UDF_INIT_CHECK(unit);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;

    for (idx = 0, *actual = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            continue;
        }

        if ((udf_id_list != NULL) && (*actual < max)) {
            udf_id_list[(*actual)] = temp->udf_id;
        }
        (*actual) += 1;
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    int idx;
    uint16 num_udfs = 0;
    bool id_running_alloc = false;
    bcmint_udf_pkt_format_info_t *format_info = NULL;
    bcmint_udf_pkt_format_info_t *format_info2 = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pkt_format, SHR_E_PARAM);
    SHR_NULL_CHECK(pkt_format_id, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_obj_input_sanitize(unit, NULL, NULL, pkt_format));

    if (options & BCM_UDF_PKT_FORMAT_CREATE_O_WITHID) {
        BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, *pkt_format_id);

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_pkt_format_info_get(unit, *pkt_format_id, &format_info));

        if (options & BCM_UDF_PKT_FORMAT_CREATE_O_REPLACE) {
            for (idx = 0; idx < UDF_MAX_PIPES; idx++) {
                num_udfs += format_info->num_udfs[idx];
            }
            if (num_udfs >= 1) {
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_udf_pkt_format_info_delete(unit, *pkt_format_id));
        }
    } else {
        /* New packet format object */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_udf_pkt_format_obj_running_id_alloc(unit, pkt_format_id));
        id_running_alloc = true;
    }

    /*
     * The packet format matching hardware is a tcam and hence
     * there is no point in allowing to create more than one entry
     * of similar configuration. Search for a matching entry in
     * list of already installed objects and return error appropriately.
     */
    if (SHR_SUCCESS(ltsw_udf_pkt_format_info_search(unit,
                    pkt_format, &format_info2))) {
        if (format_info2->priority == pkt_format->prio) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }

    /* Allocate UDF pkt format object node */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_add(unit, pkt_format, &format_info));

    /* Increment num udfs */
    XGS_UDF_CTRL(unit)->num_pkt_formats += 1;

    /* update pkt_format_id in the pkt_format_info */
    format_info->pkt_fmt_id = *pkt_format_id;

exit:

    if (SHR_FUNC_ERR()) {
        if ((id_running_alloc) &&
                (UDF_CONTROL(unit)->info.udf_pkt_format_id_running !=
                 BCMINT_UDF_ID_MAX(unit))) {
            /* Decrement the running pkt format id */
            UDF_CONTROL(unit)->info.udf_pkt_format_id_running -= 1;
        }
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_info_t *pkt_format)
{
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pkt_format, SHR_E_PARAM);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_get(unit, format_info, pkt_format));

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_destroy(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    int pipe;
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    for (pipe = 0; pipe < UDF_MAX_PIPES; pipe++) {
        if (format_info->num_udfs[pipe]) {
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_delete(unit, pkt_format_id));

    XGS_UDF_CTRL(unit)->num_pkt_formats -= 1;

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_is_object_exist(
    int unit,
    bcm_udf_id_t udf_id)
{
    bcmint_udf_offset_info_t *offset_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id,
                                  &offset_info));

exit:

    SHR_FUNC_EXIT();
}
int
xgs_ltsw_udf_is_pkt_format_exist(
    int unit,
    bcm_udf_pkt_format_id_t pkt_fmt_id)
{
    bcmint_udf_pkt_format_info_t *format_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_fmt_id,
                                      &format_info));

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_add(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    int idx, num_udfs;
    bool set;
    bcmlt_opcode_t opcode;
    bcmint_udf_offset_info_t *offset_info;
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_ID_VALIDATE(unit, udf_id);
    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    for (idx = 0; idx < UDF_MAX_ID_LIST; idx++) {
        if (format_info->udf_id_list[idx] == udf_id) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    }

    if ((offset_info->num_pkt_formats + 1) >= XGS_UDF_MAX_ENTRIES(unit)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    num_udfs = format_info->num_udfs[offset_info->pipe];
    if ((num_udfs + 1) > BCMINT_UDF_MAX_CHUNKS(unit)) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    if (num_udfs == 0) {
        opcode = BCMLT_OPCODE_INSERT;
    } else {
        opcode = BCMLT_OPCODE_UPDATE;
    }
    set = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_entry_commit(unit, set, opcode,
                                  BCMLT_PRIORITY_NORMAL,
                                  offset_info, format_info));

    offset_info->num_pkt_formats++;
    format_info->num_udfs[offset_info->pipe]++;

    for (idx = 0; idx < UDF_MAX_ID_LIST; idx++) {
        if (format_info->udf_id_list[idx] == 0) {
            format_info->udf_id_list[idx] = udf_id;
            break;
        }
    }

    format_info->flags |= BCMI_UDF_PKT_FMT_INFO_INSTALLED;

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    int max,
    bcm_udf_id_t *udf_id_list,
    int *actual)
{
    int idx, num_udfs;
    bcm_udf_id_t udf_id;
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    num_udfs = 0;
    for (idx = 0; idx < UDF_MAX_PIPES; idx++) {
        num_udfs += format_info->num_udfs[idx];
    }

    if ((udf_id_list == NULL) || (max == 0)) {
        *actual = num_udfs;
        SHR_EXIT();
    }

    for (idx = 0, *actual = 0; idx < UDF_MAX_ID_LIST; idx++) {
        udf_id = format_info->udf_id_list[idx];
        if (udf_id > 0) {
            if (*actual < max) {
                udf_id_list[*actual] = udf_id;
            }
            (*actual) += 1;
        }
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_delete(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_pkt_format_id_t pkt_format_id)
{
    int idx, num_udfs;
    bool set;
    bcmlt_opcode_t opcode;
    bcmint_udf_offset_info_t *offset_info;
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_ID_VALIDATE(unit, udf_id);
    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    if ((offset_info->num_pkt_formats) == 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    for (idx = 0; idx < UDF_MAX_ID_LIST; idx++) {
        if (format_info->udf_id_list[idx] == udf_id) {
            break;
        }
    }

    if (idx >= UDF_MAX_ID_LIST) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    num_udfs = format_info->num_udfs[offset_info->pipe];
    if (num_udfs == 0) {
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    if (num_udfs == 1) {
        opcode = BCMLT_OPCODE_DELETE;
    } else {
        opcode = BCMLT_OPCODE_UPDATE;
    }
    set = false;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_entry_commit(unit, set, opcode,
                                  BCMLT_PRIORITY_NORMAL,
                                  offset_info, format_info));

    offset_info->num_pkt_formats--;
    format_info->num_udfs[offset_info->pipe]--;
    format_info->udf_id_list[idx] = 0;
    format_info->flags &= (~BCMI_UDF_PKT_FMT_INFO_INSTALLED);

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_get_all(
    int unit,
    bcm_udf_id_t udf_id,
    int max,
    bcm_udf_pkt_format_id_t *pkt_format_id_list,
    int *actual)
{
    int idx, j;
    uint16 max_pkt_formats;
    bcmint_udf_offset_info_t *offset_info;
    bcmint_udf_pkt_format_info_t *temp;
    bcmint_udf_pkt_format_info_t *format_info_array;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_ID_VALIDATE(unit, udf_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));

    if ((max == 0) || (pkt_format_id_list == NULL)) {
        *actual = offset_info->num_pkt_formats;
        SHR_EXIT();
    }

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;

    for (idx = 0, *actual = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }
        for (j = 0; j < UDF_MAX_ID_LIST; j++) {
            if (temp->udf_id_list[j] == udf_id) {
                if (*actual < max) {
                    pkt_format_id_list[*actual] = temp->pkt_fmt_id;
                }
                (*actual) += 1;
                break;
            }
        }
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_delete_all(
    int unit,
    bcm_udf_id_t udf_id)
{
    int idx, j;
    uint16 max_pkt_formats;
    bcmint_udf_pkt_format_info_t *temp;
    bcmint_udf_pkt_format_info_t *format_info_array;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);
    BCMINT_UDF_ID_VALIDATE(unit, udf_id);

    max_pkt_formats = XGS_UDF_CTRL(unit)->max_pkt_formats;
    format_info_array = XGS_UDF_CTRL(unit)->format_info_array;

    for (idx = 0; idx < max_pkt_formats; idx++) {
        temp = &format_info_array[idx];

        if (!(temp->flags & BCMI_UDF_PKT_FMT_INFO_IN_USE)) {
            continue;
        }
        for (j = 0; j < UDF_MAX_ID_LIST; j++) {
            if (temp->udf_id_list[j] == udf_id) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (xgs_ltsw_udf_pkt_format_delete(unit, udf_id,
                                                    temp->pkt_fmt_id));
                break;
            }
        }
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_chunk_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcm_udf_multi_chunk_info_t *info)
{
    int idx;
    uint8 gran, max_chunks;
    uint32 chunk_bmap;
    bcmint_udf_offset_info_t *offset_info;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));

    info->offset = offset_info->start;
    info->width = offset_info->width;

    BCM_UDF_CBMP_CHUNK_INIT(info->one_byte_chunk_bmap);
    BCM_UDF_CBMP_CHUNK_INIT(info->two_byte_chunk_bmap);
    BCM_UDF_CBMP_CHUNK_INIT(info->four_byte_chunk_bmap);

    gran = XGS_UDF_CHUNK_GRAN(unit);
    max_chunks = BCMINT_UDF_MAX_CHUNKS(unit);

    for (idx = 0; idx < max_chunks; idx++) {
        chunk_bmap = 0;
        XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                idx, gran, &chunk_bmap);
        if ((chunk_bmap != 0) && (gran == 2)) {
            BCM_UDF_CBMP_CHUNK_ADD(info->two_byte_chunk_bmap, idx);
        }
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_oper_mode_get(
    int unit,
    bcm_udf_oper_mode_t *mode)
{
    int dunit;
    uint64_t oper_mode = 0;
    bcmi_xgs_udf_lt_tbls_info_t *tbls_info = NULL;
    bcmlt_entry_handle_t oper_mode_template = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(mode, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);
    tbls_info = XGS_UDF_CTRL(unit)->tbls_info;

    /* Entry handle allocate for Operational mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit,
                              tbls_info->udf_config_sid,
                              &oper_mode_template));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, oper_mode_template,
                              BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(oper_mode_template,
                               tbls_info->udf_opermode_pipeuniue_fid,
                               &oper_mode));
    if (oper_mode) {
        *mode = bcmUdfOperModePipeLocal;
    } else {
        *mode = bcmUdfOperModeGlobal;
    }

exit:

    if (BCMLT_INVALID_HDL != oper_mode_template) {
        (void) bcmlt_entry_free(oper_mode_template);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_hash_config_add(
    int unit,
    uint32 options,
    bcm_udf_hash_config_t *config)
{
    bcmint_udf_offset_info_t *offset_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, config->udf_id, &offset_info));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_hash_config_set(unit, config, offset_info));

    /* Set flag to indicate the object is in the list that
       participates in hash calculation */
    offset_info->flags |= BCMI_UDF_OFFSET_INFO_UDFHASH_CONFIG;

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_hash_config_delete(
    int unit,
    bcm_udf_hash_config_t *config)
{
    bcmint_udf_offset_info_t *offset_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, config->udf_id, &offset_info));

    sal_memset(config->hash_mask, 0, sizeof(config->hash_mask));
    config->mask_length = offset_info->width;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_hash_config_set(unit, config, offset_info));

    /* clear flags */
    offset_info->flags &= ~BCMI_UDF_OFFSET_INFO_UDFHASH_CONFIG;

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_hash_config_delete_all(int unit)
{
    int idx;
    uint16 max_udfs;
    bcmint_udf_offset_info_t *temp;
    bcmint_udf_offset_info_t *offset_info_array;
    bcm_udf_hash_config_t hash_config;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_INIT_CHECK(unit);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;

    for (idx = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            continue;
        }
        /* the udf id is in the list, delete its config */
        if (temp->flags & BCMI_UDF_OFFSET_INFO_UDFHASH_CONFIG) {
            sal_memset(&hash_config, 0, sizeof (hash_config));
            (&hash_config)->udf_id = temp->udf_id;
            SHR_IF_ERR_VERBOSE_EXIT
                (xgs_ltsw_udf_hash_config_delete(unit, &hash_config));
        }
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_hash_config_get(
    int unit,
    bcm_udf_hash_config_t *config)
{
    int i, j, chunk_id;
    uint8 gran, *byte_order;
    uint16 chunk_mask[BCMI_XGS_UDF_HASH_CHUNKS_MAX];
    uint32 hash_mask, chunk_bmap;
    bcmint_udf_offset_info_t *offset_info;
    bcmi_xgs_udf_lt_hash_config_t lt_hash_config;
    bcmi_xgs_udf_hash_chunks_info_t *hash_chunks_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(config, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, config->udf_id, &offset_info));

    if ((!(offset_info->flags & BCMI_UDF_OFFSET_INFO_UDFHASH)) ||
            (!(offset_info->flags & BCMI_UDF_OFFSET_INFO_UDFHASH_CONFIG))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    gran = XGS_UDF_CHUNK_GRAN(unit);
    hash_chunks_info = &(XGS_UDF_CTRL(unit)->hash_chunks_info);

    for (i = 0; i < hash_chunks_info->chunks; i++) {
        chunk_id = hash_chunks_info->chunk_ids[i];

        chunk_bmap = 0;
        XGS_UDF_CHUNK_BYTE_BMAP_GET(&(offset_info->byte_bmap),
                chunk_id, gran, &chunk_bmap);
        if (chunk_bmap != 0) {
            break;
        }
    }
    if (i >= hash_chunks_info->chunks) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_lt_hash_config_get(unit, offset_info, &lt_hash_config));

    for (i = 0; i < lt_hash_config.chunks; i++) {
        chunk_mask[i] = lt_hash_config.mask[i];
    }

    hash_chunks_info = &(XGS_UDF_CTRL(unit)->hash_chunks_info);
    byte_order = &(hash_chunks_info->byte_order)[0];

    hash_mask = (chunk_mask[0] << 16) | (chunk_mask[1] & 0xFFFF);
    /* mask[0]~[3] reflect to the order in byte_bmap */
    for (i = 0, j = 0; i < hash_chunks_info->bytes; i++) {
        if (offset_info->byte_bmap & (1 << byte_order[i])) {
            config->hash_mask[j++] = (hash_mask >> (8 * (3 - i))) & 0xFF;
        }
    }

    config->mask_length = offset_info->width;
    config->flags = 0;

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_hash_config_get_all(
    int unit,
    int max,
    bcm_udf_hash_config_t *config_list,
    int *actual)
{
    int idx;
    uint16 max_udfs;
    bcmint_udf_offset_info_t *temp;
    bcmint_udf_offset_info_t *offset_info_array;
    bcm_udf_hash_config_t hash_config;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(actual, SHR_E_PARAM);

    BCMINT_UDF_INIT_CHECK(unit);

    max_udfs = XGS_UDF_CTRL(unit)->max_udfs;
    offset_info_array = XGS_UDF_CTRL(unit)->offset_info_array;

    *actual = 0;
    for (idx = 0; idx < max_udfs; idx++) {
        temp = &offset_info_array[idx];

        if (!(temp->flags & BCMI_UDF_OFFSET_INFO_IN_USE)) {
            continue;
        }
        if (temp->flags & BCMI_UDF_OFFSET_INFO_UDFHASH_CONFIG) {
            if ((*actual < max) && (config_list != NULL)) {
                sal_memset(&hash_config, 0, sizeof (hash_config));
                (&hash_config)->udf_id = temp->udf_id;
                SHR_IF_ERR_VERBOSE_EXIT
                    (xgs_ltsw_udf_hash_config_get(unit, &hash_config));
                sal_memcpy((config_list + (*actual)), &hash_config,
                        sizeof(bcm_udf_hash_config_t));
            }
            (*actual) += 1;
        }
    }

exit:

    SHR_FUNC_EXIT();
}

int
bcmint_udf_obj_offset_info_get(
    int unit,
    bcm_udf_id_t udf_id,
    bcmint_udf_offset_info_t **offset_info)
{
    return ltsw_udf_offset_info_get(unit, udf_id, offset_info);
}

int
xgs_ltsw_udf_pkt_format_action_set(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int arg)
{
    int dunit;
    bool installed = false;
    uint64_t data;
    bcmlt_entry_handle_t handle = BCMLT_INVALID_HDL;
    bcm_udf_pkt_format_info_t format;
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);

    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    installed = format_info->flags & BCMI_UDF_PKT_FMT_INFO_INSTALLED;

    switch(action) {
        case bcmUdfPktFormatActionClassId:
            if (!(XGS_UDF_CTRL(unit)->lt_flags & BCMI_XGS_UDF_LT_OUTPUT_CLASS_ID)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            if (installed == true) {
                data = arg;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit, UDFs, &handle));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(handle, CLASS_IDs, data));
            }
            break;
        case bcmUdfPktFormatActionSkipOuterL4IfaMetadata:
            if (!(XGS_UDF_CTRL(unit)->lt_flags &
                        BCMI_XGS_UDF_LT_OFFSET_SKIP_OUTER_IFA_MD)) {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            if (installed == true) {
                data = (arg > 0) ? 1: 0;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_allocate(dunit, UDFs, &handle));

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(handle, SKIP_OUTER_IFA_METADATAs, data));
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (installed == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_pkt_format_get(unit, format_info, &format));

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_lt_key_fields_add(unit, &format, &handle));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, handle, BCMLT_OPCODE_UPDATE,
                                  BCMLT_PRIORITY_NORMAL));
    }

    switch(action) {
        case bcmUdfPktFormatActionClassId:
            format_info->class_id = arg;
            break;
        case bcmUdfPktFormatActionSkipOuterL4IfaMetadata:
            if (arg > 0) {
                format_info->flags |=
                    BCMI_UDF_PKT_FMT_INFO_SKIP_OUTER_IFA_MD;
            } else {
                format_info->flags &=
                    (~BCMI_UDF_PKT_FMT_INFO_SKIP_OUTER_IFA_MD);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    if (handle != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(handle);
    }

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_pkt_format_action_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_udf_pkt_format_action_t action,
    int *arg)
{
    bcmint_udf_pkt_format_info_t *format_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    BCMINT_UDF_PKT_FORMAT_ID_VALIDATE(unit, pkt_format_id);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_pkt_format_info_get(unit, pkt_format_id, &format_info));

    switch(action) {
        case bcmUdfPktFormatActionClassId:
            *arg = format_info->class_id;
            break;
        case bcmUdfPktFormatActionSkipOuterL4IfaMetadata:
            *arg = 0;
            if (format_info->flags &
                    BCMI_UDF_PKT_FMT_INFO_SKIP_OUTER_IFA_MD) {
                *arg = 1;
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

int
xgs_ltsw_udf_obj_is_range_check(
        int unit,
        bcm_udf_id_t udf_id,
        bool *res)
{
    bcmint_udf_offset_info_t *offset_info = NULL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(res, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_udf_offset_info_get(unit, udf_id, &offset_info));
    *res = (offset_info->flags & BCMI_UDF_OFFSET_INFO_RANGE_CHECK) ? true : false;

exit:
    SHR_FUNC_EXIT();
}
