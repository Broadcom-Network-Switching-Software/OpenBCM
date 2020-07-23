/*! \file bcmcth_mirror_imm.c
 *
 * BCMCTH Mirror IMM handler.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <bcmdrd_config.h>
#include <bcmcth/bcmcth_mirror_drv.h>
#include <bcmcth/bcmcth_mirror_util.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmissu/issu_api.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmpkt/flexhdr/bcmpkt_generic_loopback_t.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR

/* The maximum number of encap LT fields. */
#define ENCAP_FIELDS_NUM_MAX (BCMCTH_MIRROR_FIELD_ID_MAX)

/* MIRROR_VXLAN_T.TABLE_VXLAN */
#define TABLE_VXLAN (1)

/*******************************************************************************
 * Private data
 */
static shr_famm_hdl_t encap_fld_array_hdl[BCMDRD_CONFIG_MAX_UNITS];

/* Mirror encap state HA structure. */
static bcmcth_mirror_encap_state_t *mirror_encap_state[BCMDRD_CONFIG_MAX_UNITS];

/* Mirror flexhdr state HA structure. */
static bcmcth_mirror_flexhdr_state_t *mirror_flexhdr_state[BCMDRD_CONFIG_MAX_UNITS];

/* sid list of all mirror encap formats. */
static const bcmdrd_sid_t encap_sid[] = {
    MIRROR_ENCAP_BASICt,
    MIRROR_ENCAP_RSPANt,
    MIRROR_ENCAP_ERSPANt,
    MIRROR_ENCAP_ERSPAN_IPV6t,
    MIRROR_ENCAP_PSAMPt,
    MIRROR_ENCAP_PSAMP_IPV6t,
    MIRROR_ENCAP_PSAMP_METADATAt,
    MIRROR_ENCAP_PSAMP_METADATA_IPV6t,
    MIRROR_ENCAP_SFLOWt,
    MIRROR_ENCAP_SFLOW_IPV6t,
    MIRROR_ENCAP_SFLOW_SEQt,
    MIRROR_ENCAP_SFLOW_SEQ_IPV6t,
    MIRROR_ENCAP_MIRROR_ON_DROPt,
    MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t,
    MIRROR_ENCAP_VXLANt,
    MIRROR_ENCAP_VXLAN_IPV6t,
    MIRROR_ENCAP_IFA_1_PROBEt
};

static const bcmcth_mirror_flexhdr_info_t *flexhdr_info[BCMDRD_CONFIG_MAX_UNITS];
static uint32_t flexhdr_num[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */
static int
encap_lt_fields_parse_flexhdr(
    int unit,
    bcmdrd_sid_t sid,
    const char *str,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    bcmlrd_variant_t variant;
    uint32_t i;
    int id;
    char *member_name = NULL;
    size_t fname_len, mname_len;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(str, SHR_E_PARAM);

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Select the flexhdr information. */
    for (i = 0; i < flexhdr_num[unit]; i++) {
        if (flexhdr_info[unit][i].sid == sid) {
            break;
        }
    }
    if (i == flexhdr_num[unit]) {
        /* flexhdr_info is not supported for this LT */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    fname_len = sal_strlen(flexhdr_info[unit][i].fname) + 1;
    mname_len = sal_strlen(str) - fname_len + 1;
    /*
     * The fields are structure member, like
     * "LOOPBACK_HDR_DESTINATION_TYPE" and
     * "LOOPBACK_HDR_SOURCE_SYSTEM_PORT"
     * and we need to skip "LOOPBACK_HDR" and get strings
     * "DESTINATION_TYPE" and "SOURCE_SYSTEM_PORT"
     */
    member_name = sal_alloc(mname_len, "bcmcthmirrorflexhdrmembername");
    sal_memset(member_name, 0, mname_len);
    sal_memcpy(member_name,
               str + fname_len,
               mname_len);
    /* Get field ID */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpkt_generic_loopback_t_field_id_get(member_name, &id));

    if (flexhdr_info[unit][i].set) {
        SHR_IF_ERR_EXIT
            (flexhdr_info[unit][i].set(variant, entry->flexhdr, id, fval));
    }
exit:
    sal_free(member_name);
    SHR_FUNC_EXIT();
}

static int
store_flexhdr(
    int unit,
    int encap_id,
    bcmcth_mirror_encap_t *entry)
{
    bool enable;
    size_t size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Only support generic_loopback_t now. */

    if (mirror_flexhdr_state[unit] == NULL) {
        SHR_EXIT();
    }

    /* Reset value. */
    mirror_flexhdr_state[unit]->flexhdr_encap[encap_id] = FALSE;
    mirror_flexhdr_state[unit]->flexhdr_size[encap_id] = 0;

    /* Check enable and size. */
    enable = entry->flexhdr_encap;
    size = entry->flexhdr_size;
    if (!enable ||
        (size > sizeof(entry->flexhdr)) ||
        (size > sizeof(mirror_flexhdr_state[unit]->flexhdr[encap_id].data))) {
        SHR_EXIT();
    }

    /* Copy size and buffer. */
    mirror_flexhdr_state[unit]->flexhdr_encap[encap_id] = TRUE;
    mirror_flexhdr_state[unit]->flexhdr_size[encap_id] = size;
    sal_memcpy(&mirror_flexhdr_state[unit]->flexhdr[encap_id].data,
               &entry->flexhdr,
               entry->flexhdr_size);

exit:
    SHR_FUNC_EXIT();
}

static int
restore_flexhdr(
    int unit,
    int encap_id,
    bcmcth_mirror_encap_t *entry)
{
    bool enable;
    size_t size;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Only support generic_loopback_t now. */

    /* Reset value. */
    entry->flexhdr_encap = FALSE;
    entry->flexhdr_size = 0;

    if (mirror_flexhdr_state[unit] == NULL) {
        SHR_EXIT();
    }

    /* Check enable and size. */
    enable = mirror_flexhdr_state[unit]->flexhdr_encap[encap_id];
    size = mirror_flexhdr_state[unit]->flexhdr_size[encap_id];
    if (!enable ||
        (size > sizeof(entry->flexhdr)) ||
        (size > sizeof(mirror_flexhdr_state[unit]->flexhdr[encap_id].data))) {
        SHR_EXIT();
    }

    /* Copy size and buffer. */
    entry->flexhdr_encap = TRUE;
    entry->flexhdr_size = size;
    sal_memcpy(&entry->flexhdr,
               &mirror_flexhdr_state[unit]->flexhdr[encap_id].data,
               entry->flexhdr_size);
exit:
    SHR_FUNC_EXIT();
}

static int
flexhdr_lt_fields_parse(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    const bcmltd_fields_t *data_list,
    bcmcth_mirror_encap_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid;
    uint64_t fval;
    const bcmltd_table_rep_t *tbl;
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Get the table information. */
    tbl = bcmltd_table_get(sid);
    if (tbl == NULL) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Select the flexhdr information. */
    for (i = 0; i < flexhdr_num[unit]; i++) {
        if (flexhdr_info[unit][i].sid == sid) {
            break;
        }
    }
    if (i == flexhdr_num[unit]) {
        /* flexhdr_info is not supported this LT */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (flexhdr_info[unit][i].size > BCMCTH_MIRROR_FLEXHDR_SIZE_BYTES) {
        /* flexhdr_info size exceed buffer size. */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Parse key field. */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        if (fid == flexhdr_info[unit][i].key_fid) {
            entry->encap_id = fval;
            SHR_BITSET(entry->fbmp, fid);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        /* Parse fields of flexhdr */
        if (fid == flexhdr_info[unit][i].enable_fid) {
            /* Enable field. */
            entry->flexhdr_encap = fval;
            /* flexhdr size. */
            entry->flexhdr_size = flexhdr_info[unit][i].size;
        } else {
            /* Other flexhdr fields. */
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_fields_parse_flexhdr
                    (unit, sid, tbl->field[fid].name, fval, entry));
        }
        gen_field = gen_field->next;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_basic(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_BASICt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (fid) {
        case MIRROR_ENCAP_BASICt_ENCAP_MODEf:
            entry->encap_mode = fval;
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_rspan(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_RSPANt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (fid) {
        case MIRROR_ENCAP_RSPANt_UNTAGf:
            entry->untag = fval;
            break;
        case MIRROR_ENCAP_RSPANt_PRIf:
            entry->pri = fval;
            break;
        case MIRROR_ENCAP_RSPANt_CFIf:
            entry->cfi = fval;
            break;
        case MIRROR_ENCAP_RSPANt_VLAN_IDf:
            entry->vlan_id = fval;
            break;
        case MIRROR_ENCAP_RSPANt_TPIDf:
            entry->tpid = fval;
            break;
        case MIRROR_ENCAP_RSPANt_VNTAGf:
            entry->vntag = fval;
            break;
        case MIRROR_ENCAP_RSPANt_SRC_NIV_VIFf:
            entry->niv_src_vif = fval;
            break;
        case MIRROR_ENCAP_RSPANt_DST_NIV_VIFf:
            entry->niv_dst_vif = fval;
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_erspan(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_ERSPANt &&
        sid != MIRROR_ENCAP_ERSPAN_IPV6t) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Set default p_ft_hwid_d_gra_o to 0x83f5 */
    if (entry->p_ft_hwid_d_gra_o == 0) {
        entry->p_ft_hwid_d_gra_o = 0x83f5;
    }

    if (sid == MIRROR_ENCAP_ERSPANt) {
        switch (fid) {
            case MIRROR_ENCAP_ERSPANt_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_SRC_IPV4f:
                entry->src_ipv4 = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_DST_IPV4f:
                entry->dst_ipv4 = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_TOSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_TTLf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_PROTOf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_DO_NOT_FRAGMENTf:
                entry->df = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_GRE_HEADERf:
                entry->gre_header = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_SESSION_IDf:
                entry->session_id = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_ERSPAN3_SUB_HDRu_PDU_FRAMEf:
                entry->p_ft_hwid_d_gra_o &= (0x7FFF);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x1) << 15);
                break;
            case MIRROR_ENCAP_ERSPANt_ERSPAN3_SUB_HDRu_FRAME_TYPEf:
                entry->p_ft_hwid_d_gra_o &= (0x83FF);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x1F) << 10);
                break;
            case MIRROR_ENCAP_ERSPANt_ERSPAN3_SUB_HDRu_HW_IDf:
                entry->p_ft_hwid_d_gra_o &= (0xFC0F);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x3F) << 4);
                break;
            case MIRROR_ENCAP_ERSPANt_ERSPAN3_SUB_HDRu_DIRECTIONf:
                entry->p_ft_hwid_d_gra_o &= (0xFFF7);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x1) << 3);
                break;
            case MIRROR_ENCAP_ERSPANt_ERSPAN3_SUB_HDRu_TS_GRAf:
                entry->p_ft_hwid_d_gra_o &= (0xFFF9);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x3) << 1);
                break;
            case MIRROR_ENCAP_ERSPANt_ERSPAN3_SUB_HDRu_OPT_SUB_HDRf:
                entry->p_ft_hwid_d_gra_o &= (0xFFFE);
                entry->p_ft_hwid_d_gra_o |= (fval & 0x1);
                break;
            case MIRROR_ENCAP_ERSPANt_SWITCH_IDf:
                entry->switch_id = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_SEQf:
                entry->sq = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_ERSPANt_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            default:
                break;
        }
    } else if (sid == MIRROR_ENCAP_ERSPAN_IPV6t) {
        switch (fid) {
            case MIRROR_ENCAP_ERSPAN_IPV6t_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_SRC_IPV6u_LOWERf:
                entry->src_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_SRC_IPV6u_UPPERf:
                entry->src_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_DST_IPV6u_LOWERf:
                entry->dst_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_DST_IPV6u_UPPERf:
                entry->dst_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_TRAFFIC_CLASSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_HOP_LIMITf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_NEXT_HEADERf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_FLOW_LABELf:
                entry->fl = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_GRE_HEADERf:
                entry->gre_header = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_SESSION_IDf:
                entry->session_id = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ERSPAN3_SUB_HDRu_PDU_FRAMEf:
                entry->p_ft_hwid_d_gra_o &= (0x7FFF);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x1) << 15);
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ERSPAN3_SUB_HDRu_FRAME_TYPEf:
                entry->p_ft_hwid_d_gra_o &= (0x83FF);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x1F) << 10);
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ERSPAN3_SUB_HDRu_HW_IDf:
                entry->p_ft_hwid_d_gra_o &= (0xFC0F);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x3F) << 4);
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ERSPAN3_SUB_HDRu_DIRECTIONf:
                entry->p_ft_hwid_d_gra_o &= (0xFFF7);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x1) << 3);
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ERSPAN3_SUB_HDRu_TS_GRAf:
                entry->p_ft_hwid_d_gra_o &= (0xFFF9);
                entry->p_ft_hwid_d_gra_o |= ((fval & 0x3) << 1);
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_ERSPAN3_SUB_HDRu_OPT_SUB_HDRf:
                entry->p_ft_hwid_d_gra_o &= (0xFFFE);
                entry->p_ft_hwid_d_gra_o |= (fval & 0x1);
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_SWITCH_IDf:
                entry->switch_id = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_SEQf:
                entry->sq = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_ERSPAN_IPV6t_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_sflow(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_SFLOWt &&
        sid != MIRROR_ENCAP_SFLOW_IPV6t &&
        sid != MIRROR_ENCAP_SFLOW_SEQt &&
        sid != MIRROR_ENCAP_SFLOW_SEQ_IPV6t) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (sid == MIRROR_ENCAP_SFLOWt ||
        sid == MIRROR_ENCAP_SFLOW_IPV6t) {
        /* IMM is not supported since no chip use those LTs */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (sid == MIRROR_ENCAP_SFLOW_SEQt) {
        switch (fid) {
            case MIRROR_ENCAP_SFLOW_SEQt_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_SRC_IPV4f:
                entry->src_ipv4 = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_DST_IPV4f:
                entry->dst_ipv4 = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_TOSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_TTLf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_PROTOf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_DO_NOT_FRAGMENTf:
                entry->df = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_METADATA_TYPEf:
                entry->meta_type = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_METADATAf:
                entry->meta_data = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQt_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            default:
                break;
        }
    } else if (sid == MIRROR_ENCAP_SFLOW_SEQ_IPV6t) {
        switch (fid) {
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_SRC_IPV6u_LOWERf:
                entry->src_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_SRC_IPV6u_UPPERf:
                entry->src_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_DST_IPV6u_LOWERf:
                entry->dst_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_DST_IPV6u_UPPERf:
                entry->dst_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_TRAFFIC_CLASSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_HOP_LIMITf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_NEXT_HEADERf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_FLOW_LABELf:
                entry->fl = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_METADATA_TYPEf:
                entry->meta_type = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_METADATAf:
                entry->meta_data = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_SFLOW_SEQ_IPV6t_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_psamp(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_PSAMPt &&
        sid != MIRROR_ENCAP_PSAMP_IPV6t &&
        sid != MIRROR_ENCAP_PSAMP_METADATAt &&
        sid != MIRROR_ENCAP_PSAMP_METADATA_IPV6t) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (sid == MIRROR_ENCAP_PSAMPt ||
        sid == MIRROR_ENCAP_PSAMP_IPV6t) {
        /* IMM is not supported since no chip use those LTs */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (sid == MIRROR_ENCAP_PSAMP_METADATAt) {
        switch (fid) {
            case MIRROR_ENCAP_PSAMP_METADATAt_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_SRC_IPV4f:
                entry->src_ipv4 = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_DST_IPV4f:
                entry->dst_ipv4 = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_TOSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_TTLf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_PROTOf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_DO_NOT_FRAGMENTf:
                entry->df = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_METADATA_TYPEf:
                entry->meta_type = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_METADATAf:
                entry->meta_data = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_OBSERVATION_DOMAINf:
                entry->od = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_TEMPLATE_IDENTIFIERf:
                entry->ti = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_IPFIX_VERSIONf:
                entry->ipfix_ver = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_PSAMP_EPOCHf:
                entry->ipfix_epoch = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_VARIABLE_FLAGf:
                entry->vf = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATAt_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            default:
                break;
        }
    } else if (sid == MIRROR_ENCAP_PSAMP_METADATA_IPV6t) {
        switch (fid) {
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_SRC_IPV6u_LOWERf:
                entry->src_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_SRC_IPV6u_UPPERf:
                entry->src_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_DST_IPV6u_LOWERf:
                entry->dst_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_DST_IPV6u_UPPERf:
                entry->dst_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_TRAFFIC_CLASSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_HOP_LIMITf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_NEXT_HEADERf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_FLOW_LABELf:
                entry->fl = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_METADATA_TYPEf:
                entry->meta_type = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_METADATAf:
                entry->meta_data = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_OBSERVATION_DOMAINf:
                entry->od = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_TEMPLATE_IDENTIFIERf:
                entry->ti = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_IPFIX_VERSIONf:
                entry->ipfix_ver = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_PSAMP_EPOCHf:
                entry->ipfix_epoch = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_VARIABLE_FLAGf:
                entry->vf = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_PSAMP_METADATA_IPV6t_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_mod(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_MIRROR_ON_DROPt &&
        sid != MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (sid == MIRROR_ENCAP_MIRROR_ON_DROPt) {
        switch (fid) {
            case MIRROR_ENCAP_MIRROR_ON_DROPt_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_SRC_IPV4f:
                entry->src_ipv4 = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_DST_IPV4f:
                entry->dst_ipv4 = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_TOSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_TTLf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_PROTOf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_DO_NOT_FRAGMENTf:
                entry->df = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_METADATA_TYPEf:
                entry->meta_type = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_METADATAf:
                entry->meta_data = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_OBSERVATION_DOMAINf:
                entry->od = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_TEMPLATE_IDENTIFIERf:
                entry->ti = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_IPFIX_VERSIONf:
                entry->ipfix_ver = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_VARIABLE_FLAGf:
                entry->vf = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_SWITCH_IDf:
                entry->switch_id = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROPt_MODEf:
                entry->mod_mode = fval;
                break;
            default:
                break;
        }
    } else if (sid == MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t) {
        switch (fid) {
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_SRC_IPV6u_LOWERf:
                entry->src_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_SRC_IPV6u_UPPERf:
                entry->src_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_DST_IPV6u_LOWERf:
                entry->dst_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_DST_IPV6u_UPPERf:
                entry->dst_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_TRAFFIC_CLASSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_HOP_LIMITf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_NEXT_HEADERf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_FLOW_LABELf:
                entry->fl = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_METADATA_TYPEf:
                entry->meta_type = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_METADATAf:
                entry->meta_data = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_OBSERVATION_DOMAINf:
                entry->od = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_TEMPLATE_IDENTIFIERf:
                entry->ti = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_IPFIX_VERSIONf:
                entry->ipfix_ver = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_VARIABLE_FLAGf:
                entry->vf = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_SWITCH_IDf:
                entry->switch_id = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t_MODEf:
                entry->mod_mode = fval;
                break;
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_vxlan(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_VXLANt && sid != MIRROR_ENCAP_VXLAN_IPV6t) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (sid == MIRROR_ENCAP_VXLANt) {
        switch (fid) {
            case MIRROR_ENCAP_VXLANt_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_VXLANt_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_VXLANt_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_VXLANt_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_VXLANt_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_VXLANt_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_VXLANt_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_VXLANt_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_VXLANt_SRC_IPV4f:
                entry->src_ipv4 = fval;
                break;
            case MIRROR_ENCAP_VXLANt_DST_IPV4f:
                entry->dst_ipv4 = fval;
                break;
            case MIRROR_ENCAP_VXLANt_TOSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_VXLANt_TTLf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_VXLANt_PROTOf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_VXLANt_DO_NOT_FRAGMENTf:
                entry->df = fval;
                break;
            case MIRROR_ENCAP_VXLANt_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_VXLANt_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_VXLANt_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_VXLANt_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_VXLANt_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_VXLANt_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            case MIRROR_ENCAP_VXLANt_VXLAN_TYPEf:
                entry->vni_valid = (fval == TABLE_VXLAN ? TRUE : FALSE);
                break;
            case MIRROR_ENCAP_VXLANt_VXLAN_VNIDf:
                entry->vni = fval;
                break;
            default:
                break;
        }
    } else if (sid == MIRROR_ENCAP_VXLAN_IPV6t) {
        switch (fid) {
            case MIRROR_ENCAP_VXLAN_IPV6t_UNTAGf:
                entry->untag = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_OUTER_VLANf:
                entry->outer_vlan = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_PRIf:
                entry->pri = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_CFIf:
                entry->cfi = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_VLAN_IDf:
                entry->vlan_id = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_TPIDf:
                entry->tpid = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_SRC_MACf:
                entry->src_mac = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_DST_MACf:
                entry->dst_mac = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_SRC_IPV6u_LOWERf:
                entry->src_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_SRC_IPV6u_UPPERf:
                entry->src_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_DST_IPV6u_LOWERf:
                entry->dst_ipv6[0] = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_DST_IPV6u_UPPERf:
                entry->dst_ipv6[1] = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_TRAFFIC_CLASSf:
                entry->tos_tc = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_HOP_LIMITf:
                entry->ttl_hl = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_NEXT_HEADERf:
                entry->proto_nh = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_FLOW_LABELf:
                entry->fl = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_ETHERTYPEf:
                entry->ethertype = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_L4_DST_PORTf:
                entry->udp_dst_port = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_L4_SRC_PORTf:
                entry->udp_src_port = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_HIGIG3f:
                entry->higig3 = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_HIGIG3_ETHERTYPEf:
                entry->higig3_ethertype = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_HIGIG3_BASE_HDRf:
                entry->higig3_basehdr = fval;
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_VXLAN_TYPEf:
                entry->vni_valid = (fval == TABLE_VXLAN ? TRUE : FALSE);
                break;
            case MIRROR_ENCAP_VXLAN_IPV6t_VXLAN_VNIDf:
                entry->vni = fval;
                break;
            default:
                break;
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_fields_parse_ifa_1_probe(
    int unit,
    const bcmltd_sid_t sid,
    const bcmltd_fid_t fid,
    const uint64_t fval,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    if (sid != MIRROR_ENCAP_IFA_1_PROBEt) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (fid) {
        case MIRROR_ENCAP_IFA_1_PROBEt_PROBE_MARKER_1f:
            entry->ifa_1_probe_marker1 = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_PROBE_MARKER_2f:
            entry->ifa_1_probe_marker2 = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_VERSIONf:
            entry->ifa_1_version = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_MSG_TYPEf:
            entry->ifa_1_msg_type = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_REQUEST_VECTORf:
            entry->ifa_1_request_vector = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_ACTION_VECTORf:
            entry->ifa_1_action_vector = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_HOP_LIMITf:
            entry->ifa_1_hop_limit = fval;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt_MAX_LENGTHf:
            entry->ifa_1_max_length = fval;
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

static int
encap_type_parse(
    int unit,
    bcmltd_sid_t sid,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    switch (sid) {
        case MIRROR_ENCAP_BASICt:
            entry->type = BCMCTH_MIRROR_ENCAP_BASIC;
            break;
        case MIRROR_ENCAP_RSPANt:
            entry->type = BCMCTH_MIRROR_ENCAP_RSPAN;
            break;
        case MIRROR_ENCAP_ERSPANt:
            entry->type = BCMCTH_MIRROR_ENCAP_ERSPAN;
            break;
        case MIRROR_ENCAP_ERSPAN_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6;
            break;
        case MIRROR_ENCAP_PSAMPt:
            entry->type = BCMCTH_MIRROR_ENCAP_PSAMP;
            break;
        case MIRROR_ENCAP_PSAMP_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_PSAMP_IPV6;
            break;
        case MIRROR_ENCAP_PSAMP_METADATAt:
            entry->type = BCMCTH_MIRROR_ENCAP_PSAMP_METADATA;
            break;
        case MIRROR_ENCAP_PSAMP_METADATA_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6;
            break;
        case MIRROR_ENCAP_SFLOWt:
            entry->type = BCMCTH_MIRROR_ENCAP_SFLOW;
            break;
        case MIRROR_ENCAP_SFLOW_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_SFLOW_IPV6;
            break;
        case MIRROR_ENCAP_SFLOW_SEQt:
            entry->type = BCMCTH_MIRROR_ENCAP_SFLOW_SEQ;
            break;
        case MIRROR_ENCAP_SFLOW_SEQ_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6;
            break;
        case MIRROR_ENCAP_MIRROR_ON_DROPt:
            entry->type = BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP;
            break;
        case MIRROR_ENCAP_MIRROR_ON_DROP_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6;
            break;
        case MIRROR_ENCAP_VXLANt:
            entry->type = BCMCTH_MIRROR_ENCAP_VXLAN;
            break;
        case MIRROR_ENCAP_VXLAN_IPV6t:
            entry->type = BCMCTH_MIRROR_ENCAP_VXLAN_IPV6;
            break;
        case MIRROR_ENCAP_IFA_1_PROBEt:
            entry->type = BCMCTH_MIRROR_ENCAP_IFA_1_PROBE;
            break;
        default:
            break;
    }
exit:
    SHR_FUNC_EXIT();
}

typedef int (*lt_flds_parse_f)(int unit,
                               const bcmltd_sid_t sid,
                               const bcmltd_fid_t fid,
                               const uint64_t fval,
                               bcmcth_mirror_encap_t *entry);

const static lt_flds_parse_f mirror_lt_flds_parsers[] = {
    NULL,                               /* BCMCTH_MIRROR_ENCAP_NONE */
    encap_lt_fields_parse_basic,        /* BCMCTH_MIRROR_ENCAP_BASIC */
    encap_lt_fields_parse_rspan,        /* BCMCTH_MIRROR_ENCAP_RSPAN */
    encap_lt_fields_parse_erspan,       /* BCMCTH_MIRROR_ENCAP_ERSPAN */
    encap_lt_fields_parse_erspan,       /* BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6 */
    encap_lt_fields_parse_psamp,        /* BCMCTH_MIRROR_ENCAP_PSAMP */
    encap_lt_fields_parse_psamp,        /* BCMCTH_MIRROR_ENCAP_PSAMP_IPV6 */
    encap_lt_fields_parse_psamp,        /* BCMCTH_MIRROR_ENCAP_PSAMP_METADATA */
    encap_lt_fields_parse_psamp,        /* BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6 */
    encap_lt_fields_parse_sflow,        /* BCMCTH_MIRROR_ENCAP_SFLOW */
    encap_lt_fields_parse_sflow,        /* BCMCTH_MIRROR_ENCAP_SFLOW_IPV6 */
    encap_lt_fields_parse_sflow,        /* BCMCTH_MIRROR_ENCAP_SFLOW_SEQ */
    encap_lt_fields_parse_sflow,        /* BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6 */
    encap_lt_fields_parse_mod,          /* BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP */
    encap_lt_fields_parse_mod,          /* BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6 */
    encap_lt_fields_parse_vxlan,        /* BCMCTH_MIRROR_ENCAP_VXLAN */
    encap_lt_fields_parse_vxlan,        /* BCMCTH_MIRROR_ENCAP_VXLAN_IPV6 */
    encap_lt_fields_parse_ifa_1_probe,  /* BCMCTH_MIRROR_ENCAP_IFA_1_PROBE */
};

static int
encap_lt_fields_parse(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    const bcmltd_fields_t *data_list,
    bcmcth_mirror_encap_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid, i = 0;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Parse encap type, entry->type. */
    SHR_IF_ERR_VERBOSE_EXIT
        (encap_type_parse(unit, sid, entry));

    /* Parse key field. */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            /* The value of key field ID is the same for all encap formats. */
            case MIRROR_ENCAP_RSPANt_MIRROR_ENCAP_IDf:
                entry->encap_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Should not be NULL. */
    if (data != NULL || data_list != NULL) {
        SHR_NULL_CHECK(mirror_lt_flds_parsers[entry->type], SHR_E_INTERNAL);
    }

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        SHR_IF_ERR_VERBOSE_EXIT
            (mirror_lt_flds_parsers[entry->type](unit, sid,
                                                 fid, fval, entry));
        gen_field = gen_field->next;
    }

    /* Parse data_list field. */
    if (data_list) {
        for (i = 0; i < data_list->count; i++) {
            fid = data_list->field[i]->id;
            fval = data_list->field[i]->data;
            SHR_IF_ERR_VERBOSE_EXIT
                (mirror_lt_flds_parsers[entry->type](unit, sid,
                                                     fid, fval, entry));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_update(int unit, bcmltd_sid_t sid, bcmcth_mirror_encap_t *entry)
{
    uint32_t encap_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    encap_id = entry->encap_id;

    /* Restore flexhdr header */
    if (flexhdr_num[unit] != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (restore_flexhdr(unit, encap_id, entry));
    }

    /* Check insert empty and update existing entry. */
    if (mirror_encap_state[unit]->type[encap_id] != BCMCTH_MIRROR_ENCAP_NONE &&
        mirror_encap_state[unit]->type[encap_id] != entry->type) {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mirror_encap_op(unit, BCMCTH_MIRROR_ENCAP_OP_SET, entry));

    /* Store encap type. */
    mirror_encap_state[unit]->type[encap_id] = entry->type;
exit:
    SHR_FUNC_EXIT();
}

static int
encap_lt_delete(int unit, bcmltd_sid_t sid, bcmcth_mirror_encap_t *entry)
{
    uint32_t encap_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    encap_id = entry->encap_id;

    /* Check delete existing entry. */
    if (mirror_encap_state[unit]->type[encap_id] != entry->type) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mirror_encap_op(unit, BCMCTH_MIRROR_ENCAP_OP_DEL, entry));

    /* Delete encap type. */
    mirror_encap_state[unit]->type[encap_id] = BCMCTH_MIRROR_ENCAP_NONE;
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * IMM event handler
 */
static int
bcmcth_flexhdr_encap_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mirror_encap_t entry, committed;
    shr_famm_hdl_t hdl = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};

    SHR_FUNC_ENTER(unit);

    if (flexhdr_num[unit] == 0) {
        /* flexhdr_info is not supported this LT */
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    sal_memset(&entry, 0, sizeof(bcmcth_mirror_encap_t));
    sal_memset(&committed, 0, sizeof(bcmcth_mirror_encap_t));

    entry.trans_id = trans_id;
    entry.lt_id = sid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (flexhdr_lt_fields_parse(unit, sid, key, NULL, NULL, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* Parse the input to retrieve data values. */
            SHR_IF_ERR_VERBOSE_EXIT
                (flexhdr_lt_fields_parse(unit, sid, NULL, data, NULL, &entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            /*
             * Do an IMM lookup to retrieve the
             * previous fields in this LT entry.
             */
            hdl = encap_fld_array_hdl[unit];
            in_flds.count = 1;
            in_flds.field = shr_famm_alloc(hdl, 1);
            SHR_NULL_CHECK(in_flds.field, SHR_E_MEMORY);

            out_flds.count = ENCAP_FIELDS_NUM_MAX;
            out_flds.field = shr_famm_alloc(hdl, ENCAP_FIELDS_NUM_MAX);
            SHR_NULL_CHECK(out_flds.field, SHR_E_MEMORY);

            /* The value of key field ID is the same for all encap formats. */
            in_flds.field[0]->id = MIRROR_ENCAP_RSPANt_MIRROR_ENCAP_IDf;
            in_flds.field[0]->data = entry.encap_id;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds));

            SHR_IF_ERR_VERBOSE_EXIT
                (flexhdr_lt_fields_parse(unit, sid,
                                            NULL, NULL ,&out_flds, &entry));
            sal_memcpy(&committed, &entry, sizeof(committed));

            /* Parse the input to retrieve data values. */
            SHR_IF_ERR_VERBOSE_EXIT
                (flexhdr_lt_fields_parse(unit, sid, NULL, data, NULL, &entry));

            if (sal_memcmp(&entry, &committed, sizeof(committed)) == 0) {
                /* Do nothing if the config is not changed. */
                SHR_EXIT();
            }
            break;
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Store flexhdr header information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (store_flexhdr(unit, entry.encap_id, &entry));

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 1);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, ENCAP_FIELDS_NUM_MAX);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief MIRROR_ENCAP_xxx In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MIRROR_ENCAP_xxx logical table entry commit stages.
 */
static bcmimm_lt_cb_t encap_flexhdr_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmcth_flexhdr_encap_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static int
mirror_encap_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mirror_encap_t entry, committed;
    shr_famm_hdl_t hdl = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmcth_mirror_encap_t));
    sal_memset(&committed, 0, sizeof(bcmcth_mirror_encap_t));

    entry.trans_id = trans_id;
    entry.lt_id = sid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (encap_lt_fields_parse(unit, sid, key, NULL, NULL, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* Parse the input to retrieve data values. */
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_fields_parse(unit, sid, NULL, data, NULL, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_update(unit, sid, &entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            /*
             * Do an IMM lookup to retrieve the
             * previous fields in this LT entry.
             */
            hdl = encap_fld_array_hdl[unit];
            in_flds.count = 1;
            in_flds.field = shr_famm_alloc(hdl, 1);
            SHR_NULL_CHECK(in_flds.field, SHR_E_MEMORY);

            out_flds.count = ENCAP_FIELDS_NUM_MAX;
            out_flds.field = shr_famm_alloc(hdl, ENCAP_FIELDS_NUM_MAX);
            SHR_NULL_CHECK(out_flds.field, SHR_E_MEMORY);

            /* The value of key field ID is the same for all encap formats. */
            in_flds.field[0]->id = MIRROR_ENCAP_RSPANt_MIRROR_ENCAP_IDf;
            in_flds.field[0]->data = entry.encap_id;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds));

            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_fields_parse(unit, sid,
                                       NULL, NULL ,&out_flds, &entry));
            sal_memcpy(&committed, &entry, sizeof(committed));

            /* Parse the input to retrieve data values. */
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_fields_parse(unit, sid, NULL, data, NULL, &entry));

            if (sal_memcmp(&entry, &committed, sizeof(committed)) == 0) {
                /* Do nothing if the config is not changed. */
                SHR_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_update(unit, sid, &entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_lt_delete(unit, sid, &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 1);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, ENCAP_FIELDS_NUM_MAX);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief MIRROR_ENCAP_xxx In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MIRROR_ENCAP_xxx logical table entry commit stages.
 */
static bcmimm_lt_cb_t encap_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mirror_encap_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static int
encap_ins_attr_lt_fields_parse(
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    const bcmltd_fields_t *data_list,
    bcmcth_mirror_encap_ins_attr_t *entry)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid, i= 0;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    /* Parse key field. */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
            case MIRROR_ENCAP_INSTANCE_ATTRt_MIRROR_ENCAP_IDf:
                entry->encap_id = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data field. */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;
        switch (fid) {
            case MIRROR_ENCAP_INSTANCE_ATTRt_TRUNCATEf:
                entry->truncate_en = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MIRROR_ENCAP_INSTANCE_ATTRt_MIRROR_TRUNCATE_LENGTH_IDf:
                entry->truncate_ptr = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            case MIRROR_ENCAP_INSTANCE_ATTRt_SYSTEM_PORT_TABLE_IDf:
                entry->system_port = fval;
                SHR_BITSET(entry->fbmp, fid);
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    /* Parse data_list field. */
    if (data_list) {
        for (i = 0; i < data_list->count; i++) {
            fid = data_list->field[i]->id;
            fval = data_list->field[i]->data;
            switch (fid) {
                case MIRROR_ENCAP_INSTANCE_ATTRt_TRUNCATEf:
                    entry->truncate_en = fval;
                    SHR_BITSET(entry->fbmp, fid);
                    break;
                case MIRROR_ENCAP_INSTANCE_ATTRt_MIRROR_TRUNCATE_LENGTH_IDf:
                    entry->truncate_ptr = fval;
                    SHR_BITSET(entry->fbmp, fid);
                    break;
                case MIRROR_ENCAP_INSTANCE_ATTRt_SYSTEM_PORT_TABLE_IDf:
                    entry->system_port = fval;
                    SHR_BITSET(entry->fbmp, fid);
                    break;
                default:
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
mirror_encap_ins_attr_stage(
    int unit,
    bcmltd_sid_t sid,
    uint32_t trans_id,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mirror_encap_ins_attr_t entry, committed;
    shr_famm_hdl_t hdl = NULL;
    bcmltd_fields_t in_flds = {0}, out_flds = {0};

    SHR_FUNC_ENTER(unit);

    sal_memset(&entry, 0, sizeof(bcmcth_mirror_encap_ins_attr_t));
    sal_memset(&committed, 0, sizeof(bcmcth_mirror_encap_ins_attr_t));

    entry.trans_id = trans_id;
    entry.lt_id = sid;

    /* Parse the input to retrieve key values. */
    SHR_IF_ERR_VERBOSE_EXIT
        (encap_ins_attr_lt_fields_parse(unit, sid, key, NULL,
                                             NULL, &entry));
    if (output_fields) {
        output_fields->count = 0;
    }
    switch (event) {
        case BCMIMM_ENTRY_INSERT:
            /* Parse the input to retrieve data values. */
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_ins_attr_lt_fields_parse(unit, sid, NULL,
                                                     data, NULL, &entry));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mirror_encap_ins_attr_op(unit,
                                                      BCMCTH_MIRROR_ENCAP_OP_SET,
                                                      &entry));
            break;
        case BCMIMM_ENTRY_UPDATE:
            /*
             * Do an IMM lookup to retrieve the
             * previous fields in this LT entry.
             */
            hdl = encap_fld_array_hdl[unit];
            in_flds.count = 1;
            in_flds.field = shr_famm_alloc(hdl, 1);
            SHR_NULL_CHECK(in_flds.field, SHR_E_MEMORY);

            out_flds.count = ENCAP_FIELDS_NUM_MAX;
            out_flds.field = shr_famm_alloc(hdl, ENCAP_FIELDS_NUM_MAX);
            SHR_NULL_CHECK(out_flds.field, SHR_E_MEMORY);

            /* The value of key field ID is the same for all encap formats. */
            in_flds.field[0]->id = MIRROR_ENCAP_INSTANCE_ATTRt_MIRROR_ENCAP_IDf;
            in_flds.field[0]->data = entry.encap_id;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds));

            SHR_IF_ERR_VERBOSE_EXIT
                (encap_ins_attr_lt_fields_parse(unit, sid,
                                       NULL, NULL, &out_flds, &entry));
            sal_memcpy(&committed, &entry, sizeof(committed));

            /* Parse the input to retrieve data values. */
            SHR_IF_ERR_VERBOSE_EXIT
                (encap_ins_attr_lt_fields_parse(unit, sid, NULL,
                                                     data, NULL, &entry));

            if (sal_memcmp(&entry, &committed, sizeof(committed)) == 0) {
                /* Do nothing if the config is not changed. */
                SHR_EXIT();
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mirror_encap_ins_attr_op(unit,
                                                      BCMCTH_MIRROR_ENCAP_OP_SET,
                                                      &entry));
            break;
        case BCMIMM_ENTRY_DELETE:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmcth_mirror_encap_ins_attr_op(unit,
                                                      BCMCTH_MIRROR_ENCAP_OP_DEL,
                                                      &entry));
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    if (in_flds.field) {
        shr_famm_free(hdl, in_flds.field, 1);
    }
    if (out_flds.field) {
        shr_famm_free(hdl, out_flds.field, ENCAP_FIELDS_NUM_MAX);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief MIRROR_ENCAP_INSTANCE_ATTR In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to MIRROR_ENCAP_INSTANCE_ATTR logical table entry commit stages.
 */
static bcmimm_lt_cb_t encap_ins_attr_event_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = mirror_encap_ins_attr_stage,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static bool
bcmcth_mirror_imm_mapped(int unit, const bcmdrd_sid_t sid)
{
    int rv;
    const bcmlrd_map_t *map = NULL;

    rv = bcmlrd_map_get(unit, sid, &map);
    if (SHR_SUCCESS(rv) &&
        map &&
        map->group &&
        map->group[0].dest.kind == BCMLRD_MAP_CUSTOM) {
        return TRUE;
    }
    return FALSE;
}

/*!
 * \brief Stage callback function of IMM event handler (bcmimm_lt_cb_t).
 *
 * \param [in] unit This is device unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] trans_id is the transaction ID associated with this operation.
 * \param [in] event This is the reason for the entry event.
 * \param [in] key_flds This is a linked list of the key fields identifying
 *                        the entry.
 * \param [in] data_flds This is a linked list of the data fields in the
 *                        modified entry.
 * \param [in] context Is a pointer that was given during registration.
 *                     The callback can use this pointer to retrieve some
 *                     context.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_mirror_ctr_flex_sflow_stage (
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    bcmimm_entry_event_t event,
    const bcmltd_field_t *key,
    const bcmltd_field_t *data,
    void *context,
    bcmltd_fields_t *output_fields)
{
    bcmcth_mirror_flex_sflow_stat_t ctrl_info = {0};

    SHR_FUNC_ENTER(unit);

    if (output_fields) {
        output_fields->count = 0;
    }

    switch (event) {
        case BCMIMM_ENTRY_INSERT:
        case BCMIMM_ENTRY_UPDATE:
        case BCMIMM_ENTRY_DELETE:
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    ctrl_info.pipe_idx = -1;
    while (key) {
        if (key->id ==
            CTR_MIRROR_ING_FLEX_SFLOWt_CTR_MIRROR_ING_FLEX_SFLOW_IDf) {
            ctrl_info.flex_sflow_id = key->data;
        }
        if (key->id ==
            CTR_MIRROR_ING_FLEX_SFLOWt_PIPEf) {
            ctrl_info.pipe_idx = key->data;
        }
        key = key->next;
    }

    if ((event == BCMIMM_ENTRY_UPDATE) || (event == BCMIMM_ENTRY_INSERT)) {
        /* Parse the data fields */
        while (data) {
            if (data->id ==
                CTR_MIRROR_ING_FLEX_SFLOWt_SAMPLE_POOL_PKTf) {
                ctrl_info.sampling_eligible_pkt = data->data;
                ctrl_info.upd_sampling_eligible_pkt = true;
            } else if (data->id ==
                       CTR_MIRROR_ING_FLEX_SFLOWt_SAMPLE_PKTf) {
                ctrl_info.sampled_pkt = data->data;
                ctrl_info.upd_sampled_pkt = true;
            }  else if (data->id ==
                        CTR_MIRROR_ING_FLEX_SFLOWt_SAMPLE_POOL_PKT_SNAPSHOTf) {
                ctrl_info.eligible_pkt_snapshot = data->data;
                ctrl_info.upd_eligible_pkt_snapshot = true;
            } else {
                /* Do nothing */
            }
            data = data->next;
        }
    }

    if ((event == BCMIMM_ENTRY_DELETE) || (event == BCMIMM_ENTRY_INSERT)) {
        ctrl_info.upd_eligible_pkt_snapshot = true;
        ctrl_info.upd_sampled_pkt = true;
        ctrl_info.upd_sampling_eligible_pkt = true;
    }

    ctrl_info.attrib = op_arg->attrib;
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_ctr_flex_sflow_write(unit, &ctrl_info));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup callback function of IMM event handler.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Logical table ID.
 * \param [in]  op_arg          The operation arguments.
 * \param [in]  context         Pointer to retrieve some context.
 * \param [in]  lkup_type       Lookup type.
 * \param [in]  In              Input key fields list.
 * \param [in/out] out          Output parameter list.
 *
 * \retval SHR_E_NONE  OK
 * \retval !SHR_E_NONE ERROR
 */
static int
bcmcth_mirror_ctr_flex_sflow_lookup (
    int unit,
    bcmltd_sid_t sid,
    const bcmltd_op_arg_t *op_arg,
    void *context,
    bcmimm_lookup_type_t lkup_type,
    const bcmltd_fields_t *in,
    bcmltd_fields_t *out)
{
    const bcmltd_fields_t *key;
    uint32_t i = 0;
    bcmcth_mirror_flex_sflow_stat_t ctrl_info = {0};

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(op_arg, SHR_E_PARAM);

    /* key fields are in the "out" parameter for traverse operations */
    key = (lkup_type == BCMIMM_TRAVERSE) ? out : in;
    ctrl_info.pipe_idx = -1;
    for (i = 0; i < key->count; i++) {
        if (key->field[i]->id ==
            CTR_MIRROR_ING_FLEX_SFLOWt_CTR_MIRROR_ING_FLEX_SFLOW_IDf) {
            ctrl_info.flex_sflow_id = key->field[i]->data;
        }
        if (key->field[i]->id ==
            CTR_MIRROR_ING_FLEX_SFLOWt_PIPEf) {
            ctrl_info.pipe_idx = key->field[i]->data;
        }
    }

    ctrl_info.attrib = op_arg->attrib;
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_ctr_flex_sflow_read(unit, &ctrl_info));

    for (i = 0; i < out->count; i++) {
        if (out->field[i]->id ==
            CTR_MIRROR_ING_FLEX_SFLOWt_SAMPLE_POOL_PKTf) {
            out->field[i]->data = ctrl_info.sampling_eligible_pkt;
        } else if (out->field[i]->id ==
                   CTR_MIRROR_ING_FLEX_SFLOWt_SAMPLE_PKTf) {
            out->field[i]->data = ctrl_info.sampled_pkt;
        } else if (out->field[i]->id ==
                   CTR_MIRROR_ING_FLEX_SFLOWt_SAMPLE_POOL_PKT_SNAPSHOTf) {
            out->field[i]->data = ctrl_info.eligible_pkt_snapshot;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_MIRROR_ING_FLEX_SFLOW In-memory event callback structure.
 *
 * This structure contains callback functions that will be corresponding
 * to CTR_MIRROR_ING_FLEX_SFLOW logical table entry commit stages.
 */
static bcmimm_lt_cb_t mirro_ctr_flex_sflow_imm_hdl = {

    /*! Validate function. */
    .validate = NULL,

    /*! Extended staging function. */
    .op_stage = bcmcth_mirror_ctr_flex_sflow_stage,

    /*! Extended lookup function. */
    .op_lookup = bcmcth_mirror_ctr_flex_sflow_lookup,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

static int
bcmcth_mirror_imm_register(int unit)
{
    uint32_t i;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for MIRROR LTs here.
     */
    for (i = 0; i < sizeof(encap_sid)/sizeof(bcmdrd_sid_t); i++) {
        if (bcmcth_mirror_imm_mapped(unit, encap_sid[i])) {
            SHR_IF_ERR_EXIT
                (bcmimm_lt_event_reg(unit, encap_sid[i],
                                     &encap_event_hdl, NULL));
        }
    }

    /*
     * To register callback for flexhdr LTs here.
     */
    if (flexhdr_info[unit] && flexhdr_num[unit] != 0) {
        for (i = 0; i < flexhdr_num[unit]; i++) {
            if (bcmcth_mirror_imm_mapped(unit, flexhdr_info[unit][i].sid)) {
                SHR_IF_ERR_EXIT
                    (bcmimm_lt_event_reg(unit, flexhdr_info[unit][i].sid,
                                         &encap_flexhdr_event_hdl, NULL));
            }
        }
    }

    /* Register CTR_MIRROR_ING_FLEX_SFLOW handler */
    if (bcmcth_mirror_imm_mapped(unit, CTR_MIRROR_ING_FLEX_SFLOWt)) {
        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmimm_lt_event_reg(unit,
                                CTR_MIRROR_ING_FLEX_SFLOWt,
                                &mirro_ctr_flex_sflow_imm_hdl,
                                NULL), SHR_E_PARAM);
    }

    /* Register MIRROR_ENCAP_INSTANCE_ATTR handler */
    if (bcmcth_mirror_imm_mapped(unit, MIRROR_ENCAP_INSTANCE_ATTRt)) {
        SHR_IF_ERR_EXIT_EXCEPT_IF(
            bcmimm_lt_event_reg(unit,
                                MIRROR_ENCAP_INSTANCE_ATTRt,
                                &encap_ins_attr_event_hdl,
                                NULL), SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mirror_imm_ha_init(int unit, bool warm)
{
    uint32_t ha_alloc_size = 0, ha_req_size = 0, i;

    SHR_FUNC_ENTER(unit);

    mirror_encap_state[unit] = NULL;
    mirror_flexhdr_state[unit] = NULL;

    for (i = 0; i < sizeof(encap_sid)/sizeof(bcmdrd_sid_t); i++) {
        if (bcmcth_mirror_imm_mapped(unit, encap_sid[i])) {
            /* Allocate mirror encap status. */
            ha_req_size = sizeof(bcmcth_mirror_encap_state_t);
            ha_alloc_size = ha_req_size;
            mirror_encap_state[unit] =
                shr_ha_mem_alloc(unit,
                                 BCMMGMT_MIRROR_COMP_ID,
                                 BCMMIRROR_ENCAP_SUB_COMP_ID,
                                 "bcmcthMirrorEncapState",
                                 &ha_alloc_size);

            SHR_NULL_CHECK(mirror_encap_state[unit], SHR_E_MEMORY);
            SHR_IF_ERR_VERBOSE_EXIT(
                (ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
            if (!warm) {
                sal_memset(mirror_encap_state[unit], 0, ha_alloc_size);
                bcmissu_struct_info_report(unit, BCMMGMT_MIRROR_COMP_ID,
                                           BCMMIRROR_ENCAP_SUB_COMP_ID, 0,
                                           sizeof(bcmcth_mirror_encap_state_t),
                                           1,
                                           BCMCTH_MIRROR_ENCAP_STATE_T_ID);
            }

            if (flexhdr_num[unit] != 0) {
                /* Allocate mirror encap flexhdr status. */
                ha_req_size = sizeof(bcmcth_mirror_flexhdr_state_t);
                ha_alloc_size = ha_req_size;
                mirror_flexhdr_state[unit] =
                    shr_ha_mem_alloc(unit,
                                     BCMMGMT_MIRROR_COMP_ID,
                                     BCMMIRROR_ENCAP_FLEX_HDR_SUB_COMP_ID,
                                     "bcmcthMirrorEncapFlexHdrState",
                                     &ha_alloc_size);

                SHR_NULL_CHECK(mirror_flexhdr_state[unit], SHR_E_MEMORY);
                SHR_IF_ERR_VERBOSE_EXIT(
                    (ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);
                if (!warm) {
                    sal_memset(mirror_flexhdr_state[unit], 0, ha_alloc_size);
                    bcmissu_struct_info_report
                        (unit, BCMMGMT_MIRROR_COMP_ID,
                         BCMMIRROR_ENCAP_FLEX_HDR_SUB_COMP_ID, 0,
                         sizeof(bcmcth_mirror_flexhdr_state_t),
                         1,
                         BCMCTH_MIRROR_FLEXHDR_STATE_T_ID);
                }
            }
            return SHR_E_NONE;
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        if (mirror_encap_state[unit] != NULL) {
            shr_ha_mem_free(unit, mirror_encap_state[unit]);
            mirror_encap_state[unit] = NULL;
        }
        if (mirror_flexhdr_state[unit] != NULL) {
            shr_ha_mem_free(unit, mirror_flexhdr_state[unit]);
            mirror_flexhdr_state[unit] = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

static int
bcmcth_mirror_variant_info_init(int unit)
{
    /* Only support generic_loopback_t now. */
    const bcmcth_mirror_data_t *data = NULL;
    int rv;

    flexhdr_info[unit] = NULL;
    flexhdr_num[unit] = 0;

    rv = bcmcth_mirror_data_get(unit, &data);

    if (SHR_SUCCESS(rv) && data) {
        flexhdr_num[unit] = data->flexhdr_num;
        flexhdr_info[unit] = data->flexhdr;
    }
    return SHR_E_NONE;
}

static int
bcmcth_mirror_variant_info_deinit(int unit)
{
    flexhdr_info[unit] = NULL;
    flexhdr_num[unit] = 0;
    return SHR_E_NONE;
}

static int
bcmcth_mirror_imm_ha_deinit(int unit)
{
    return SHR_E_NONE;
}

/*******************************************************************************
 * Public Functions
 */

int
bcmcth_mirror_imm_init(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    assert(BCMCTH_MIRROR_ENCAP_TYPE_COUNT ==
           (sizeof(mirror_lt_flds_parsers)/sizeof(mirror_lt_flds_parsers[0])));

    SHR_IF_ERR_EXIT
        (shr_famm_hdl_init(ENCAP_FIELDS_NUM_MAX,
                           &encap_fld_array_hdl[unit]));
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_variant_info_init(unit));

    SHR_IF_ERR_EXIT
        (bcmcth_mirror_imm_register(unit));

    SHR_IF_ERR_EXIT
        (bcmcth_mirror_imm_ha_init(unit, warm));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mirror_imm_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (encap_fld_array_hdl[unit]) {
        shr_famm_hdl_delete(encap_fld_array_hdl[unit]);
        encap_fld_array_hdl[unit] = 0;
    }
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_variant_info_deinit(unit));
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_imm_ha_deinit(unit));
exit:
    SHR_FUNC_EXIT();
}

