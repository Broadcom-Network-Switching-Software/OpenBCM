/*! \file udf.c
 *
 * BCM56990_A0 UDF Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>

#include <bcm_int/ltsw/mbcm/udf.h>
#include <bcm_int/ltsw/xgs/udf.h>
#include <bcm_int/ltsw/udf_int.h>
#include <bcm_int/ltsw/lt_intf.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_UDF

bcmi_xgs_udf_lt_tbls_info_t bcm56990_a0_udf_tbls_info = {
    .udf_config_sid                 = "UDF_CONFIG",
    .udf_opermode_pipeuniue_fid     = "UDF_OPERMODE_PIPEUNIQUE",
    .udf_sid                        = "UDF",
    .port_id_fid                    = "PORT_ID",
    .port_id_mask_fid               = "PORT_ID_MASK",
    .loopback_hdr_fid               = "LOOPBACK_HDR",
    .loopback_hdr_mask_fid          = "LOOPBACK_HDR_MASK",
    .opaque_tag_fid                 = "OPAQUE_TAG_TYPE",
    .opaque_tag_mask_fid            = "OPAQUE_TAG_TYPE_MASK",
    .l4dst_port_fid                 = "L4DST_PORT",
    .l4dst_port_mask_fid            = "L4DST_PORT_MASK",
    .inband_telemetry_fid           = "INBAND_TELEMETRY",
    .inband_telemetry_mask_fid      = "INBAND_TELEMETRY_MASK",
    .l2_type_fid                    = "L2_TYPE",
    .l2_type_mask_fid               = "L2_TYPE_MASK",
    .ethertype_fid                  = "ETHERTYPE",
    .ethertype_mask_fid             = "ETHERTYPE_MASK",
    .vlan_tag_fid                   = "VLAN_TAG",
    .vlan_tag_mask_fid              = "VLAN_TAG_MASK",
    .l3_fields_fid                  = "L3_FIELDS",
    .l3_fields_mask_fid             = "L3_FIELDS_MASK",
    .outer_l3_hdr_fid               = "OUTER_L3_HDR",
    .outer_l3_hdr_mask_fid          = "OUTER_L3_HDR_MASK",
    .inner_l3_hdr_fid               = "INNER_L3_HDR",
    .inner_l3_hdr_mask_fid          = "INNER_L3_HDR_MASK",
    .outer_ifa_hdr_fid              = "OUTER_IFA_HDR",
    .outer_ifa_hdr_mask_fid         = "OUTER_IFA_HDR_MASK",
    .inner_ifa_hdr_fid              = "INNER_IFA_HDR",
    .inner_ifa_hdr_mask_fid         = "INNER_IFA_HDR_MASK",
    .class_id_fid                   = "CLASS_ID",
    .flex_hash_fid                  = "FLEX_HASH",
    .skip_outer_ifa_md_fid          = "SKIP_OUTER_IFA_METADATA",
    .layer_fid                      = "LAYER",
    .offset_fid                     = "OFFSET",
};

static int
ltsw_udf_l2format_enum_sym_get(int unit,
    char *sid, char *fid, int enum_value,
    const char **symbol)
{
    bcmlt_field_def_t fdef;

    SHR_FUNC_ENTER(unit);

    switch(enum_value) {
        case BCM_PKT_FORMAT_L2_ETH_II:
            *symbol = "ETHER_2";
            break;
        case BCM_PKT_FORMAT_L2_SNAP:
            *symbol = "SNAP";
            break;
        case BCM_PKT_FORMAT_L2_LLC:
            *symbol = "LLC";
            break;
        case BCM_PKT_FORMAT_L2_ANY:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, sid, fid, &fdef));
            *symbol = fdef.sym_def;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_vlanformat_enum_sym_get(int unit,
    char *sid, char *fid, int enum_value,
    const char **symbol)
{
    bcmlt_field_def_t fdef;

    SHR_FUNC_ENTER(unit);

    switch(enum_value) {
        case BCM_PKT_FORMAT_VLAN_TAG_NONE:
            *symbol = "UNTAGGED";
            break;
        case BCM_PKT_FORMAT_VLAN_TAG_SINGLE:
            *symbol = "SINGLE_TAGGED";
            break;
        case BCM_PKT_FORMAT_VLAN_TAG_ANY:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, sid, fid, &fdef));
            *symbol = fdef.sym_def;
            break;
        default:
             SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_l3hdr_enum_sym_get(int unit,
    char *sid, char *fid, int enum_value,
    const char **symbol)
{
    bcmlt_field_def_t fdef;

    SHR_FUNC_ENTER(unit);

    switch(enum_value) {
        case BCM_PKT_FORMAT_IP4_WITH_OPTIONS:
            *symbol = "IPV4_WITH_OPTIONS";
            break;
        case BCM_PKT_FORMAT_IP6_WITH_OPTIONS:
            *symbol = "IPV6_WITH_EXTN";
            break;
        case BCM_PKT_FORMAT_IP_NONE:
            *symbol = "NON_IP";
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_field_def_get(unit, sid, fid, &fdef));
            *symbol = fdef.sym_def;
            break;
    }

exit:

    SHR_FUNC_EXIT();
}

static int
ltsw_udf_layer_enum_sym_get(int unit,
    char *sid, char *fid, int enum_value,
    const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    switch(enum_value) {
        case bcmUdfLayerL2Header:
            *symbol = "L2_HDR";
            break;
        case bcmUdfLayerL3OuterHeader:
        case bcmUdfLayerTunnelHeader:
            *symbol = "OUTER_L3_HDR";
            break;
        case bcmUdfLayerL3InnerHeader:
        case bcmUdfLayerL4OuterHeader:
            *symbol = "INNER_L3_HDR";
            break;
        case bcmUdfLayerL4InnerHeader:
        case bcmUdfLayerTunnelPayload:
            *symbol = "L4_HDR";
            break;
        case bcmUdfLayerHigigHeader:
        case bcmUdfLayerHigig2Header:
            *symbol = "MODULE_HDR";
            break;
        default:
            /* Unknown layer type: Return Error */
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_udf_enum_sym_get(int unit,
    char *sid, char *fid, int enum_value,
    const char **symbol)
{
    SHR_FUNC_ENTER(unit);

    if (!strcmp(fid, "L2_TYPE")) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_l2format_enum_sym_get(unit, sid, fid,
                                         enum_value, symbol));
    } else if (!strcmp(fid, "VLAN_TAG")) {

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_vlanformat_enum_sym_get(unit, sid, fid,
                                         enum_value, symbol));
    } else if (!strcmp(fid, "OUTER_L3_HDR") ||
            !(strcmp(fid, "INNER_L3_HDR"))) {

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_l3hdr_enum_sym_get(unit, sid, fid,
                                         enum_value, symbol));
    } else if (!strcmp(fid, "LAYER")) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_udf_layer_enum_sym_get(unit, sid, fid,
                                         enum_value, symbol));
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_udf_init(int unit)
{
    bcmi_xgs_udf_appl_chunks_info_t *appl_chunks_info;
    uint8 flexhash_chunks_list[] = {12, 13, 14, 15};
    uint8 policer_chunks_list[] = {0, 1};
    uint8 udfhash_chunks_list[] = {6, 7};
    uint8 rangecheck_chunks_list[] = {5};
    uint8 def_appl_chunks_list[] = {
        2, 3, 6, 7, 0, 1, 4, 5,
        8, 9, 10, 11, 12, 13, 14, 15
    };

    SHR_FUNC_ENTER(unit);

    /* Allocate xgs control */
    SHR_IF_ERR_VERBOSE_EXIT(xgs_ltsw_udf_control_alloc(unit));

    /* Supported udf aplications */
    UDF_CONTROL(unit)->info.flags |= BCMINT_UDF_RANGE_CHECK_SUPPORTED;
    UDF_CONTROL(unit)->info.flags |= BCMINT_UDF_FLEXHASH_SUPPORTED;
    UDF_CONTROL(unit)->info.flags |= BCMINT_UDF_POLICER_GROUP_SUPPORTED;
    UDF_CONTROL(unit)->info.flags |= BCMINT_UDF_UDFHASH_SUPPORTED;

    /* Device specific params */
    UDF_CONTROL(unit)->info.max_chunks   = 16;
    XGS_UDF_CTRL(unit)->gran             = 2;
    XGS_UDF_CTRL(unit)->max_parse_bytes  = 128;

    /* Software parameters */
    UDF_CONTROL(unit)->info.min_obj_id   = 0x1;
    UDF_CONTROL(unit)->info.max_obj_id   = 0xFFE;

    /* Table info */
    XGS_UDF_CTRL(unit)->tbls_info = &bcm56990_a0_udf_tbls_info;
    XGS_UDF_CTRL(unit)->enum_sym_get = bcm56990_a0_ltsw_udf_enum_sym_get;

    /* UDF hash chunks information */
    XGS_UDF_CTRL(unit)->hash_chunks_info.chunks = 2;
    XGS_UDF_CTRL(unit)->hash_chunks_info.chunk_ids[0] = 6;
    XGS_UDF_CTRL(unit)->hash_chunks_info.chunk_ids[1] = 7;
    XGS_UDF_CTRL(unit)->hash_chunks_info.field_str[0] = "UDF6_DATA_MASK";
    XGS_UDF_CTRL(unit)->hash_chunks_info.field_str[1] = "UDF7_DATA_MASK";

    /* UDF hash bytes ordering */
    XGS_UDF_CTRL(unit)->hash_chunks_info.bytes = 4;
    XGS_UDF_CTRL(unit)->hash_chunks_info.byte_order[0] = 13;
    XGS_UDF_CTRL(unit)->hash_chunks_info.byte_order[1] = 12;
    XGS_UDF_CTRL(unit)->hash_chunks_info.byte_order[2] = 15;
    XGS_UDF_CTRL(unit)->hash_chunks_info.byte_order[3] = 14;

    /* udf application chunks info */
    appl_chunks_info = &(XGS_UDF_CTRL(unit)->appl_chunks_info);

    appl_chunks_info->flexhash_chunks = COUNTOF(flexhash_chunks_list);
    sal_memcpy(appl_chunks_info->flexhash, flexhash_chunks_list,
            (sizeof (uint8) * COUNTOF(flexhash_chunks_list)));

    appl_chunks_info->policer_chunks = COUNTOF(policer_chunks_list);
    sal_memcpy(appl_chunks_info->policer, policer_chunks_list,
            (sizeof (uint8) * COUNTOF(policer_chunks_list)));

    appl_chunks_info->udfhash_chunks = COUNTOF(udfhash_chunks_list);
    sal_memcpy(appl_chunks_info->udfhash, udfhash_chunks_list,
            (sizeof (uint8) * COUNTOF(udfhash_chunks_list)));

    appl_chunks_info->rangecheck_chunks = COUNTOF(rangecheck_chunks_list);
    sal_memcpy(appl_chunks_info->rangecheck, rangecheck_chunks_list,
            (sizeof (uint8) * COUNTOF(rangecheck_chunks_list)));

    appl_chunks_info->def_appl_chunks = COUNTOF(def_appl_chunks_list);
    sal_memcpy(appl_chunks_info->def_appl, def_appl_chunks_list,
            (sizeof (uint8) * COUNTOF(def_appl_chunks_list)));

    SHR_IF_ERR_VERBOSE_EXIT(xgs_ltsw_udf_init(unit));

exit:

    SHR_FUNC_EXIT();
}

static int
bcm56990_a0_ltsw_udf_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT(xgs_ltsw_udf_detach(unit));

    SHR_IF_ERR_EXIT(xgs_ltsw_udf_control_free(unit));

exit:

    SHR_FUNC_EXIT();
}

static mbcm_ltsw_udf_drv_t bcm56990_a0_ltsw_udf_drv = {
    /*! UDF init */
    .udf_init                           = bcm56990_a0_ltsw_udf_init,

    /*! UDF detach */
    .udf_detach                         = bcm56990_a0_ltsw_udf_detach,

    /*! UDF object exists */
    .udf_is_object_exist                = xgs_ltsw_udf_is_object_exist,

    /*! UDF multi-chunk create */
    .udf_multi_chunk_create             = NULL,

    /*! UDF multi-chunk info get */
    .udf_multi_chunk_info_get           = NULL,

    /*! UDF destroy */
    .udf_destroy                        = xgs_ltsw_udf_destroy,

    /*! UDF multi packet format info get */
    .udf_multi_pkt_fmt_info_get         = NULL,

    /*! UDF abstract packet format object list get */
    .udf_abstr_pkt_fmt_object_list_get  = NULL,

    /*! UDF object create */
    .udf_create                         = xgs_ltsw_udf_create,

    /*! UDF info get */
    .udf_get                            = xgs_ltsw_udf_get,

    /*! Get all UDF objects */
    .udf_get_all                        = xgs_ltsw_udf_get_all,

    /*! UDF pkt format create */
    .udf_pkt_format_create              = xgs_ltsw_udf_pkt_format_create,

    /*! UDF pkt format info */
    .udf_pkt_format_info_get            = xgs_ltsw_udf_pkt_format_info_get,

    /*! UDF pkt format destroy */
    .udf_pkt_format_destroy             = xgs_ltsw_udf_pkt_format_destroy,

    /*! Check is UDF pkt format object is created */
    .udf_is_pkt_format_exist            = xgs_ltsw_udf_is_pkt_format_exist,

    /*! UDF pkt format add to UDF object */
    .udf_pkt_format_add                 = xgs_ltsw_udf_pkt_format_add,

    /*! Get all UDF object associated with pkt format */
    .udf_pkt_format_get                 = xgs_ltsw_udf_pkt_format_get,

    /*! Delete packet format associated with UDF object */
    .udf_pkt_format_delete              = xgs_ltsw_udf_pkt_format_delete,

    /*! Get all pkt formats associated with UDF object */
    .udf_pkt_format_get_all             = xgs_ltsw_udf_pkt_format_get_all,

    /*! Delete all pkt format associated with UDF object */
    .udf_pkt_format_delete_all          = xgs_ltsw_udf_pkt_format_delete_all,

    /*! UDF operating mode */
    .udf_oper_mode_get                  = xgs_ltsw_udf_oper_mode_get,

    /*! Add UDF object to hashing list */
    .udf_hash_config_add                = xgs_ltsw_udf_hash_config_add,

    /*! Delete UDF object from hashing list */
    .udf_hash_config_delete             = xgs_ltsw_udf_hash_config_delete,

    /*! Delete all UDF object from hashing list */
    .udf_hash_config_delete_all         = xgs_ltsw_udf_hash_config_delete_all,

    /*! Get UDF hashing config */
    .udf_hash_config_get                = xgs_ltsw_udf_hash_config_get,

    /*! Get all UDF object from hashing list */
    .udf_hash_config_get_all            = xgs_ltsw_udf_hash_config_get_all,

    /*! Apply action for pkt format */
    .udf_pkt_format_action_set          = xgs_ltsw_udf_pkt_format_action_set,

    /*! Get action value for pkt format */
    .udf_pkt_format_action_get          = xgs_ltsw_udf_pkt_format_action_get,
};

/******************************************************************************
 * Private functions
 */

int
bcm56990_a0_ltsw_udf_drv_attach(int unit)
{
   return mbcm_ltsw_udf_drv_set(unit, &bcm56990_a0_ltsw_udf_drv);
}

/*!
 * \brief Attach functions for other members.
 */
#define BCM56990_A0_DRV_ATTACH_ENTRY(_dn,_vn,_pf,_pd,_r0) \
int _vn##_ltsw_udf_drv_attach(int unit) \
{ \
    SHR_FUNC_ENTER(unit); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (mbcm_ltsw_udf_drv_set(unit, &bcm56990_a0_ltsw_udf_drv)); \
exit: \
    SHR_FUNC_EXIT(); \
}
#include "sub_devlist.h"
