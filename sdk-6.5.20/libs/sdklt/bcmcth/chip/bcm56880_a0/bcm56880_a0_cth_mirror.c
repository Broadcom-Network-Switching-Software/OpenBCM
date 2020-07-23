/*! \file bcm56880_a0_cth_mirror.c
 *
 * Chip stub for BCMCTH MIRROR.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_ha.h>
#include <shr/shr_util.h>
#include <bcmcth/bcmcth_mirror_drv.h>
#include <bcmcth/bcmcth_mirror_util.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56880_a0_defs.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/chip/bcm56880_a0/bcm56880_a0_lrd_enum_ctype.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR

/* Variant invalid ID */
#define VARIANT_INVALID_ID                 (-1)

/* MIRROR_METADATA_T */
#define MIRROR_METADATA_T_TABLE_METADATA   (1)

/* egr_sequence.usage_mode_create.'1' */
#define EGR_SEQ_USER_MODE_MIRROR           (1)

#define INSERT_ENGINE_ID_NULL              (0)
#define INSERT_ENGINE_ID_MIRROR_ENCAP_0    (13)

#define EGR_MIRROR_FLEX_EDITOR_WIDTH       (40)

/* Encap. format content */
#define PSAMP_TRUNCATE_LEN_ADJUST          (60)
#define PSAMP_TRUNCATE_UDP_LEN_ADJUST      (40)

#define PKT_ENCAP_VXLAN                    (0x08)

/*
 * GRE Protocol Type:
 * Type I is obsolete.
 * 0x88BE (Type II)
 * 0x22EB (Type III)
 */
#define ERSPAN_GRE_PROTOCOL_TYPE_2         (0x88BE)
#define ERSPAN_GRE_PROTOCOL_TYPE_3         (0x22EB)

#define GRE_PROTOCOL                       (0x2F)
#define UDP_PROTOCOL                       (0x11)

#define PKT_VLAN_MASK            (0xFFF)
#define PKT_PRIO_SHIFT           (13)
#define PKT_PRIO_MASK            (7)
#define PKT_PRIO(p)              (((p) & PKT_PRIO_MASK) << PKT_PRIO_SHIFT)
#define PKT_CFI_MASK             (1)
#define PKT_CFI_SHIFT            (12)
#define PKT_CFI(c)               (((c) & PKT_CFI_MASK) << PKT_CFI_SHIFT)

#define IPV4_HEADER_LENGTH_WORDS (5)
#define IPV4_VERSION             (4)
#define IPV4_HDR_CHECKSUM_BYTES  (2)
#define IPV4_IP_ADDR_BYTES       (8)
#define IPV4_ETHER_TYPE          (0x0800)
#define IPV4_NIV_ETHER_TYPE      (0x220E)

#define IPV6_VERSION             (6)
#define IPV6_TRAFFIC_CLASS       (2)
#define IPV6_HOP_LIMIT           (0x3F)
#define IPV6_ETHER_TYPE          (0x86DD)

#define LOOPBACK_HEADER_LEN      (BCMPKT_LBHDR_SIZE_BYTES)
#define OTAG_LEN                 (4)
#define IPV6_HEADER_LEN          (40)
#define IPV4_HEADER_LEN          (20)
#define UDP_HEADER_LEN           (8)
#define SFLOW_HEADER_LEN         (16)
#define GRE_HEADER_LEN           (4)
#define VXLAN_HEADER_LEN         (8)
#define HIGIG3_BASE_HDR_LEN      (8)
#define VARIABLE_FLAG            (0xFF)

#define TWOS_COMPLEMENT(x)       (0x100 - (x))
#define NIBBLE_SHIFT_OFFSET      (4)

/* IP don't fragment flag position */
#define DF_FLAG_SHIFTER          (6)

/*******************************************************************************
 * Private variables
 */
/* Variant data structure. */
static const bcmcth_mirror_data_t *variant_data[BCMDRD_CONFIG_MAX_UNITS];

/* Check if mirror encapsulation is supported. */
#define BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(_u_)             \
    do {                                                   \
        if (!variant_data[_u_] ||                          \
            variant_data[unit]->hdr_id_mirror_transport == \
            VARIANT_INVALID_ID) {                          \
            SHR_ERR_EXIT(SHR_E_UNAVAIL);                   \
        }                                                  \
    } while (0)

/*******************************************************************************
 * Local functions
 */

static int
bcm56880_a0_cth_mirror_encap_type_update(
    int unit,
    bcmcth_mirror_encap_t *entry,
    bool enable)
{
    int index = 0;
    EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm_t buf;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t *buf_ptr = NULL;
    uint32_t fval;
    uint32_t trans_id;
    bcmltd_sid_t lt_id;
    bool vxlan_flex = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    /* Write to memory */
    buf_ptr = (uint32_t *)(&buf);
    sal_memset(buf_ptr, 0,  sizeof(buf));

    pt_id = EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm;
    pt_fid = MIRROR_EDIT_CTRL_IDf;

    /*
     * MIRROR_EDIT_CTRL_ID
     * enum mec_id_enum {
     *   MIRROR_NO_ENCAP            = 1,
     *   MIRROR_INSERT_RSPAN        = 0x43,
     *   MIRROR_ENCAP_ERSPAN_1      = 0x81,
     *   MIRROR_ENCAP_PSAMP         = 0x82,
     *   MIRROR_ENCAP_SFLOW         = 0x84,
     *   MIRROR_ENCAP_ERSPAN_3      = 0x85,
     *   MIRROR_ENCAP_MIRROR_ON_DROP= 0x86,
     *   MIRROR_ENCAP_VXLAN         = 0x87,
     *   MIRROR_ENCAP_IFA_L3_TNL    = 0x8a,
     *   MIRROR_NO_ENCAP_LPBK_PORT  = 0x8b,
     *   IFA_1_PROBE_INSERT         = 0xc1,
     *   MIRROR_ENCAP_IFA_LOOPBACK  = 0xc2
     *}
     */
    if (enable) {
        switch (entry->type) {
        case BCMCTH_MIRROR_ENCAP_BASIC:
            if (entry->encap_mode ==
                BCM56880_A0_LRD_MIRROR_ENCAP_MODE_T_T_NO_ENCAP_LOOPBACK) {
                /* MIRROR_NO_ENCAP_LPBK_PORT for no-encap mirror. */
                fval = 0x8b;
            } else if (entry->encap_mode ==
                       BCM56880_A0_LRD_MIRROR_ENCAP_MODE_T_T_ENCAP_IFA_LOOPBACK) {
                /* MIRROR_ENCAP_IFA_LOOPBACK for 1st PASS packet of IFA 2.0. */
                fval = 0xc2;
            } else {
                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            break;
        case BCMCTH_MIRROR_ENCAP_RSPAN:
            fval = 0x43;
            break;
        case BCMCTH_MIRROR_ENCAP_ERSPAN:
        case BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6:
            if (entry->gre_header == ERSPAN_GRE_PROTOCOL_TYPE_3) {
                /* erspan3 */
                fval = 0x85;
            } else {
                /* erspan_1 */
                fval = 0x81;
            }
            break;
        /* case BCMCTH_MIRROR_ENCAP_PSAMP: */
        /* case BCMCTH_MIRROR_ENCAP_PSAMP_IPV6: */
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA:
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6:
            fval = 0x82;
            break;
        /* case BCMCTH_MIRROR_ENCAP_SFLOW: */
        /* case BCMCTH_MIRROR_ENCAP_SFLOW_IPV6: */
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6:
            fval = 0x84;
            break;
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP:
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6:
            fval = 0x86;
            break;
        case BCMCTH_MIRROR_ENCAP_VXLAN:
        case BCMCTH_MIRROR_ENCAP_VXLAN_IPV6:
            vxlan_flex = entry->vni_valid ? FALSE : TRUE;
            /* VXLAN encapsulation using flex engine */
            if (vxlan_flex) {
                /*
                 * VXLAN header will be added by flex engine and
                 * ERSPAN portion is added by mirror profile configs
                 */
                fval = 0x87;
            } else {
                fval = 0x81;
            }
            break;
        default:
            fval = 0x0;
            break;
        }
        LOG_DEBUG(BSL_LOG_MODULE,
                  (BSL_META_U(unit,
                              "MIRROR_EDIT_CTRL_ID=0x%x\n"),
                   fval));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mirror_pt_read(unit, trans_id, lt_id, pt_id,
                                   index, buf_ptr));

        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit,
                                pt_id,
                                buf_ptr,
                                pt_fid,
                                (uint32_t *) (&fval)));
    }

    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id, pt_id, index, buf_ptr));

exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_z2_shim_del(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    EGR_MIRROR_ENCAP_TABLEm_t encap_data_buf;
    uint32_t *p_encap_data_buf;
    bcmdrd_sid_t pt_id;
    uint32_t trans_id;
    bcmltd_sid_t lt_id;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    /* Zone-2 Shim reset: Mirror encap data. */
    p_encap_data_buf = (uint32_t *) &encap_data_buf;
    sal_memset(p_encap_data_buf, 0,  sizeof(encap_data_buf));

    /* Zone-2 Shim write: Mirror encap data. */
    pt_id = EGR_MIRROR_ENCAP_TABLEm;
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id, index, p_encap_data_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_z2_shim_set(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    bool rspan = FALSE, sflow = FALSE, erspan3 = FALSE;
    bool psamp = FALSE, mirr_on_drop = FALSE, metadata= FALSE;
    EGR_MIRROR_ENCAP_TABLEm_t encap_data_buf;
    uint32_t encap_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t *p_encap_data_buf;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t fval;
    uint32_t trans_id;
    bcmltd_sid_t lt_id;
    int index = 0;
    uint32_t i;
    const bcmdrd_fid_t encap_data_pt_sid[8] = {
        MIRROR_ENCAP_DATA_0f,
        MIRROR_ENCAP_DATA_1f,
        MIRROR_ENCAP_DATA_2f,
        MIRROR_ENCAP_DATA_3f,
        MIRROR_ENCAP_DATA_4f,
        MIRROR_ENCAP_DATA_5f,
        MIRROR_ENCAP_DATA_6f,
        MIRROR_ENCAP_DATA_7f
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    switch (entry->type) {
        case BCMCTH_MIRROR_ENCAP_RSPAN:
            rspan = TRUE;
            break;
        case BCMCTH_MIRROR_ENCAP_ERSPAN:
        case BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6:
            if (entry->gre_header == ERSPAN_GRE_PROTOCOL_TYPE_3) {
                erspan3 = TRUE;
            }
            break;
        case BCMCTH_MIRROR_ENCAP_SFLOW:
        case BCMCTH_MIRROR_ENCAP_SFLOW_IPV6:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6:
            sflow = TRUE;
            break;
        case BCMCTH_MIRROR_ENCAP_PSAMP:
        case BCMCTH_MIRROR_ENCAP_PSAMP_IPV6:
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA:
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6:
            psamp = TRUE;
            break;
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP:
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6:
            mirr_on_drop = TRUE;
            break;
        default:
        /*
         * VXLAN : no needs to update Mirror encap data
         * Hence, we don't have to handle VXLAN case.
         *  - BCMCTH_MIRROR_ENCAP_VXLAN
         *  - BCMCTH_MIRROR_ENCAP_VXLAN_IPV6
         */
            break;
    }

    if (entry->meta_type == MIRROR_METADATA_T_TABLE_METADATA) {
        metadata = TRUE;
    }

    /* Zone-2 Shim reset: Mirror encap data. */
    p_encap_data_buf = (uint32_t *) &encap_data_buf;
    sal_memset(encap_data, 0,  sizeof(encap_data_buf));

    /* Zone-2 Shim update: Mirror encap data. */
    if (rspan) {
        encap_data[0] = entry->tpid;
        encap_data[1] = (entry->vlan_id & PKT_VLAN_MASK) |
                        (PKT_CFI(entry->cfi)) |
                        (PKT_PRIO(entry->pri));
    } else if (erspan3) {
        encap_data[0] = entry->session_id;
        encap_data[1] = entry->p_ft_hwid_d_gra_o;
        encap_data[2] = entry->switch_id;
    } else if (sflow) {
        encap_data[0] = (metadata) ? (entry->meta_data) : 0;
    } else if (psamp) {
        encap_data[0] = entry->ipfix_ver;
        encap_data[1] = (entry->od) & 0xFFFF;
        encap_data[2] = (entry->od >> 16) & 0xFFFF;
        encap_data[3] = entry->ti;
        encap_data[4] = entry->ipfix_epoch;
        encap_data[5] = (metadata) ? (entry->meta_data) : 0;
        /* Varible flag as constant 0xFF. */
        encap_data[6] = (entry->vf) ? (entry->vf) : VARIABLE_FLAG;
    } else if (mirr_on_drop) {
        encap_data[0] = entry->ipfix_ver;
        encap_data[1] = (entry->od) & 0xFFFF;
        encap_data[2] = (entry->od >> 16) & 0xFFFF;
        encap_data[3] = entry->ti;
        encap_data[4] = (entry->switch_id) & 0xFFFF;
        encap_data[5] = (entry->switch_id >> 16) & 0xFFFF;
        encap_data[6] = (metadata) ? (entry->meta_data) : 0;
        /* Varible flag as constant 0xFF. */
        encap_data[7] = (entry->vf) ? (entry->vf) : VARIABLE_FLAG;
    }

    /* Zone-2 Shim write: Mirror encap data. */
    pt_id = EGR_MIRROR_ENCAP_TABLEm;

    /*
     * mapping of between the indexes of EGR_MIRROR_ENCAP_TABLE[i] and
     * FLEX_EDITOR_ZONE_0_EDIT_ID_INS_1_TABLE[EDIT_ID_INS_0 + i].
     */
    pt_fid = MIRROR_ENGINE_EDIT_ID_INS_OFFSETf;
    fval = 0;
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_encap_data_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /*
     * 1-1 mapping of between the indexes of EGR_MIRROR_ENCAP_TABLE and
     * FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1/2/3_TABLE.
     * index of FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1/2/3 =
     * ENGINE_ID_INS_0_PROFILE_INDEX (base from EDIT_CTRL_TCAM_x) +
     * MIRROR_ENGINE_PROFILE_OFFSET (offset)
     */
    pt_fid = MIRROR_ENGINE_PROFILE_OFFSETf;
    fval = 0;
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_encap_data_buf,
                            pt_fid, (uint32_t *) (&fval)));

    for (i = 0; i < sizeof(encap_data_pt_sid)/sizeof(bcmdrd_fid_t); i++) {
        pt_fid = encap_data_pt_sid[i];
        fval = encap_data[i];
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id, p_encap_data_buf,
                                pt_fid, (uint32_t *) (&fval)));
    }
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id, index, p_encap_data_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_z0_edit_ins_del(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    FLEX_EDITOR_ZONE_0_EDIT_ID_INS_1_TABLEm_t z0_edit_ins_buf;
    uint32_t *p_z0_edit_ins_buf;
    bcmdrd_sid_t pt_id;
    uint32_t trans_id;
    bcmltd_sid_t lt_id;
    int index = 0, edit_id = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    /* Zone-0 insert: reset. */
    p_z0_edit_ins_buf = (uint32_t *) &z0_edit_ins_buf;
    sal_memset(p_z0_edit_ins_buf, 0, sizeof(z0_edit_ins_buf));

    /*
     * Zone-0 insert: Write.
     * EDIT_ID = 4 + n for r_mirror_encap_profile_n
     * (FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1_TABLEm[n]).
     */
    edit_id = variant_data[unit]->edit_id_base + index;
    pt_id = FLEX_EDITOR_ZONE_0_EDIT_ID_INS_1_TABLEm;

    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id, edit_id, p_z0_edit_ins_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_z0_edit_ins_set(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    FLEX_EDITOR_ZONE_0_EDIT_ID_INS_1_TABLEm_t z0_edit_ins_buf;
    uint32_t *p_z0_edit_ins_buf;
    uint32_t ins_valid[2] = {1, 0};
    uint32_t ins_enhanced_ins_enable[2] = {0, 0};
    uint32_t ins_ins_mode[2] = {0, 0};
    uint32_t ins_egr_start_hdr_id[2] = {0, 0};
    uint32_t ins_egr_rel_offset[2] = {0, 0};
    uint32_t ins_num_ins_hdr_id[2] = {1, 0};
    uint32_t ins_bitmap_select[2] = {0, 0};
    uint32_t ins_bitmap_mode_valid[2] = {0, 0};
    uint32_t hdr_id_list_hdr_id[2] = {0, 0};
    uint32_t hdr_id_list_hdr_size[2] = {0, 0};
    uint32_t hdr_id_list_ins_cmd_num[2] = {0, 0};
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t fval;
    uint32_t trans_id;
    bcmltd_sid_t lt_id;
    int index = 0, edit_id = 0;
    bool otag;
    bool erspan3 = FALSE, erspan_seq = FALSE, vxlan_flex = TRUE;
    bool higig3 = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);
    SHR_NULL_CHECK(variant_data[unit], SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    if (entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN ||
        entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6) {
        if (entry->gre_header == ERSPAN_GRE_PROTOCOL_TYPE_3) {
            erspan3 = TRUE;
        }
        if (entry->sq) {
            erspan_seq = TRUE;
        }
    }

    if (entry->type == BCMCTH_MIRROR_ENCAP_VXLAN ||
        entry->type == BCMCTH_MIRROR_ENCAP_VXLAN_IPV6) {
            vxlan_flex = entry->vni_valid ? FALSE : TRUE;
    }

    higig3 = entry->higig3;
    otag = entry->outer_vlan;

    switch (entry->type) {
        case BCMCTH_MIRROR_ENCAP_BASIC:
            hdr_id_list_hdr_size[0] = 4;
            break;
        /* IPv4 */
        case BCMCTH_MIRROR_ENCAP_RSPAN:
            hdr_id_list_hdr_size[0] = 4;
            break;
        case BCMCTH_MIRROR_ENCAP_ERSPAN:
            if (erspan3 || erspan_seq) {
                hdr_id_list_hdr_size[0] = 46;
            } else {
                hdr_id_list_hdr_size[0] = 42;
            }
            hdr_id_list_hdr_size[0] += (higig3 ? HIGIG3_BASE_HDR_LEN : 0);
            break;
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA:
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ:
            hdr_id_list_hdr_size[0] = 46;
            hdr_id_list_hdr_size[0] += (higig3 ? HIGIG3_BASE_HDR_LEN : 0);
            break;
        case BCMCTH_MIRROR_ENCAP_VXLAN:
            if (vxlan_flex) {
                hdr_id_list_hdr_size[0] = 46;
            } else {
                hdr_id_list_hdr_size[0] = 54;
            }
            hdr_id_list_hdr_size[0] += (higig3 ? HIGIG3_BASE_HDR_LEN : 0);
            break;
        /* IPv6 */
        case BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6:
            if (erspan3 || erspan_seq) {
                ins_num_ins_hdr_id[1] = 1;
                hdr_id_list_ins_cmd_num[1] = 1;
                hdr_id_list_hdr_size[0] = 64;
                hdr_id_list_hdr_size[1] = 2;
                ins_valid[1] = 1;
                hdr_id_list_hdr_size[1] += (higig3 ? HIGIG3_BASE_HDR_LEN : 0);
            } else {
                if (higig3) {
                    /* 62 + HIGIG3_BASE_HDR_LEN = 70 (64 + 6) */
                    ins_num_ins_hdr_id[1] = 1;
                    hdr_id_list_ins_cmd_num[1] = 1;
                    hdr_id_list_hdr_size[0] = 64;
                    hdr_id_list_hdr_size[1] = 6;
                    ins_valid[1] = 1;
                } else {
                    hdr_id_list_hdr_size[0] = 62;
                }
            }
            break;
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6:
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6:
            ins_num_ins_hdr_id[1] = 1;
            hdr_id_list_ins_cmd_num[1] = 1;
            hdr_id_list_hdr_size[0] = 64;
            hdr_id_list_hdr_size[1] = 2;
            ins_valid[1] = 1;
            hdr_id_list_hdr_size[1] += (higig3 ? HIGIG3_BASE_HDR_LEN : 0);
            break;
        case BCMCTH_MIRROR_ENCAP_VXLAN_IPV6:
            ins_num_ins_hdr_id[1] = 1;
            hdr_id_list_ins_cmd_num[1] = 1;
            hdr_id_list_hdr_size[0] = 64;
            ins_valid[1] = 1;
            if (vxlan_flex) {
                hdr_id_list_hdr_size[1] = 2;
                hdr_id_list_hdr_size[1] += (higig3 ? HIGIG3_BASE_HDR_LEN : 0);
            } else {
                if (higig3) {
                    /*
                     * Calculate hdr_size[0] + hdr_size[1].
                     * 64 + 10 + HIGIG3_BASE_HDR_LEN = 82 > Max 80
                     * Hence, not support.
                     */
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "Encap header excess 80 Bytes.\n")));
                    LOG_WARN(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "VXLAN_TYPE == TABLE_VXLAN doesn't "
                                         "support VXLAN IPv6 with HiGig3 "
                                         "header.\n")));
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
                hdr_id_list_hdr_size[1] = 10;
            }
            break;
        default:
            break;
    }

    /* Decrease size of OTAG. */
    hdr_id_list_hdr_size[0] -= (otag ? 0 : OTAG_LEN);
    hdr_id_list_hdr_size[0] += (entry->flexhdr_encap ? entry->flexhdr_size : 0);

    /* When hdr_size[0] is exceed 64 */
    if (hdr_id_list_hdr_size[0] + hdr_id_list_hdr_size[1] > 80) {
        /* Mirror encap supports maximum 80 Bytes. */
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "hdr_id_list_hdr_size[] = %d %d\n"
                             "hdr_id_list_hdr_id[] =   %d %d\n"
                             "ins_valid[]          =   %d %d\n"),
                  hdr_id_list_hdr_size[0], hdr_id_list_hdr_size[1],
                  hdr_id_list_hdr_id[0], hdr_id_list_hdr_id[1],
                  ins_valid[0], ins_valid[1]));
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Encap header excess 80 Bytes.\n")));
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    } else if (hdr_id_list_hdr_size[0] > 64) {
        hdr_id_list_hdr_size[1] += hdr_id_list_hdr_size[0] - 64;
        hdr_id_list_hdr_size[0] = 64;
        ins_num_ins_hdr_id[1] = 1;
        hdr_id_list_ins_cmd_num[1] = 1;
        ins_valid[1] = 1;
    }
    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "hdr_id_list_hdr_id[] = %d %d\n"
                          "ins_valid[]          = %d %d\n"),
               hdr_id_list_hdr_id[0], hdr_id_list_hdr_id[1],
               ins_valid[0], ins_valid[1]));

    /* hdr_id (egress_pkt.sys_hdr.mirror_transport) in map.yml */
    if (ins_valid[0]) {
        hdr_id_list_hdr_id[0] = variant_data[unit]->hdr_id_mirror_transport;
    }
    if (ins_valid[1]) {
        hdr_id_list_hdr_id[1] = variant_data[unit]->hdr_id_mirror_transport;
    }

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    /* Zone-0 insert: reset. */
    p_z0_edit_ins_buf = (uint32_t *) &z0_edit_ins_buf;
    sal_memset(p_z0_edit_ins_buf, 0, sizeof(z0_edit_ins_buf));

    /* Zone-0 insert: update. */
    pt_id = FLEX_EDITOR_ZONE_0_EDIT_ID_INS_1_TABLEm;

    /* Indicates valid insert action. */
    pt_fid = INSERT_0_VALIDf;
    fval = ins_valid[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    pt_fid = INSERT_1_VALIDf;
    fval = ins_valid[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /* Indicates valid insert action. */
    pt_fid = INSERT_0_ENHANCED_INSERT_ENABLEf;
    fval = ins_enhanced_ins_enable[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = INSERT_1_ENHANCED_INSERT_ENABLEf;
    fval = ins_enhanced_ins_enable[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /*
     * Indicates insert mode.
     * 0:Insert header in order;
     * 1:Insert header after EGR_START_HDR_ID.
     */
    pt_fid = INSERT_0_INSERT_MODEf;
    fval = ins_ins_mode[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = INSERT_1_INSERT_MODEf;
    fval = ins_ins_mode[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /*
     * The HDR_ID to which the start of the insert action is anchored in the
     * zone.
     * 0:Value 0 means start at the start of the zone.
     * Note:It is ignored if INSERT_MODE is 0.
     */
    pt_fid = INSERT_0_EGR_START_HDR_IDf;
    fval = ins_egr_start_hdr_id[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = INSERT_1_EGR_START_HDR_IDf;
    fval = ins_egr_start_hdr_id[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /*
     * Adjust offset (in byte granularity, but only even number allowed)
     * relative to the MSB of the header identified by EGR_START_HDR_ID.
     * Note:It is ignored if INSERT_MODE is 0.
     */
    pt_fid = INSERT_0_EGR_REL_OFFSETf;
    fval = ins_egr_rel_offset[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    pt_fid = INSERT_1_EGR_REL_OFFSETf;
    fval = ins_egr_rel_offset[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /* Number of headers being inserted as part of this insert action. */
    pt_fid = INSERT_0_NUM_INS_HDR_IDf;
    fval = ins_num_ins_hdr_id[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    pt_fid = INSERT_1_NUM_INS_HDR_IDf;
    fval = ins_num_ins_hdr_id[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /* Select which bitmap to use for the bitmap mode. */
    pt_fid = INSERT_0_BITMAP_SELECTf;
    fval = ins_bitmap_select[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = INSERT_1_BITMAP_SELECTf;
    fval = ins_bitmap_select[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /*
     * Use bitmap vector to determine the size of the inserted header.
     * NUM_IUNS_HDR_ID should be 1 only
     */
    pt_fid = INSERT_0_BITMAP_MODE_VALIDf;
    fval = ins_bitmap_mode_valid[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = INSERT_1_BITMAP_MODE_VALIDf;
    fval = ins_bitmap_mode_valid[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /* The header ID in this zone that needs to be inserted. */
    pt_fid = HDR_ID_LIST_0_HDR_IDf;
    fval = hdr_id_list_hdr_id[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = HDR_ID_LIST_1_HDR_IDf;
    fval = hdr_id_list_hdr_id[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /* The size (in byte granularity) of the HDR_ID associated with it. */
    pt_fid = HDR_ID_LIST_0_HDR_SIZEf;
    fval = hdr_id_list_hdr_size[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = HDR_ID_LIST_1_HDR_SIZEf;
    fval = hdr_id_list_hdr_size[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));


    /* The insert command the header ID associates with. */
    pt_fid = HDR_ID_LIST_0_INS_COMMAND_NUMf;
    fval = hdr_id_list_ins_cmd_num[0];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = HDR_ID_LIST_1_INS_COMMAND_NUMf;
    fval = hdr_id_list_ins_cmd_num[1];
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /* Identifies the engine to perform the insert. */
    pt_fid = INSERT_0_ENGINE_IDf;
    fval = INSERT_ENGINE_ID_MIRROR_ENCAP_0;
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));
    pt_fid = INSERT_1_ENGINE_IDf;
    fval = INSERT_ENGINE_ID_NULL;
    SHR_IF_ERR_EXIT(
        bcmdrd_pt_field_set(unit, pt_id, p_z0_edit_ins_buf,
                            pt_fid, (uint32_t *) (&fval)));

    /*
     * Zone-0 insert: Write.
     * EDIT_ID = 4 + n for r_mirror_encap_profile_n
     * (FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1_TABLEm[n]).
     */
    edit_id = variant_data[unit]->edit_id_base + index;
    pt_id = FLEX_EDITOR_ZONE_0_EDIT_ID_INS_1_TABLEm;

    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id, edit_id, p_z0_edit_ins_buf));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_z0_encap_prof_del(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1_TABLEm_t profile_1;
    FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_2_TABLEm_t profile_2;
    FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_3_TABLEm_t profile_3;
    FLEX_EDITOR_MIRROR_ENCAP_FIELD_0_PROFILE_TABLEm_t field_0;
    uint32_t *p_profile_1_hdr, *p_profile_2_hdr, *p_profile_3_hdr;
    uint32_t *p_field_0;
    bcmdrd_sid_t pt_id_1, pt_id_2, pt_id_3, pt_id_field_0;
    uint32_t trans_id;
    bcmltd_sid_t lt_id;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    /* Zone-0 reset: Flex editor encap profile. */
    pt_id_1 = FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1_TABLEm;
    pt_id_2 = FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_2_TABLEm;
    pt_id_3 = FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_3_TABLEm;

    p_profile_1_hdr = (uint32_t *) &profile_1;
    p_profile_2_hdr = (uint32_t *) &profile_2;
    p_profile_3_hdr = (uint32_t *) &profile_3;

    sal_memset(p_profile_1_hdr, 0, sizeof(profile_1));
    sal_memset(p_profile_2_hdr, 0, sizeof(profile_2));
    sal_memset(p_profile_3_hdr, 0, sizeof(profile_3));

    /* Zone-0 reset: Sequence number. */
    pt_id_field_0 = FLEX_EDITOR_MIRROR_ENCAP_FIELD_0_PROFILE_TABLEm;
    p_field_0 = (uint32_t *) &field_0;
    sal_memset(p_field_0, 0, sizeof(field_0));

    /* Zone-0 write: Flex editor encap profile. */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_1, index, p_profile_1_hdr));
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_2, index, p_profile_2_hdr));
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_3, index, p_profile_3_hdr));

    /* Zone-0 write: Sequence number. */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_field_0, index, p_field_0));
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_bitp_set(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    bool seq = FALSE;
    bool psamp = FALSE, erspan_seq = FALSE, mirr_on_drop = FALSE;
    bcmdrd_sid_t pt_id;
    bcmdrd_fid_t pt_fid;
    uint32_t *p_seq = NULL;
    bcmltd_sid_t lt_id;
    uint32_t fval, trans_id, entry_sz = 0;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    if (entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN ||
        entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6) {
        if (entry->sq) {
            erspan_seq = TRUE;
        }
    }

    if (entry->type == BCMCTH_MIRROR_ENCAP_PSAMP ||
        entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_METADATA ||
        entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6) {
        psamp = TRUE;
    }

    if (entry->type == BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP ||
        entry->type == BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6 ) {
        mirr_on_drop = TRUE;
    }

    if (psamp || erspan_seq || mirr_on_drop ||
        entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_SEQ ||
        entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6) {
        seq = TRUE;
    }

    /* Enable seq number on bitp. */
    if (seq) {
        trans_id = entry->trans_id;
        lt_id = entry->lt_id;
        pt_id = EGR_SEQUENCE_BITP_PROFILEm;
        index = EGR_SEQ_USER_MODE_MIRROR;

        entry_sz = bcmdrd_pt_entry_wsize(unit, pt_id) * 4;
        SHR_ALLOC(p_seq, entry_sz, "bcmcthEgrSeqBitp");
        SHR_NULL_CHECK(p_seq, SHR_E_MEMORY);
        sal_memset(p_seq, 0, entry_sz);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mirror_pt_read(unit, trans_id, lt_id,
                                   pt_id, index,
                                   p_seq));
        pt_fid = MIRROR_LKUP_ENf;
        fval = 1;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id, p_seq,
                                pt_fid, (uint32_t *) (&fval)));
        SHR_IF_ERR_EXIT
            (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                    pt_id, index,
                                    p_seq));
    }
exit:
    SHR_FREE(p_seq);
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_z0_encap_prof_set(
    int unit,
    bcmcth_mirror_encap_t *entry)
{
    bool ipv6 = FALSE, tagged = FALSE, truncate = FALSE;
    bool rspan = FALSE, erspan = FALSE, erspan3 = FALSE, erspan_seq = FALSE;
    bool vxlan = FALSE, vxlan_flex = TRUE;
    bool vntag = FALSE, otag, higig3;
    bool psamp = FALSE, sflow = FALSE, mirr_on_drop = FALSE, basic = FALSE;
    uint16_t flow_label = 0, traffic_class = 0;
    uint16_t gre_header;
    uint8_t p_profile_hdr[2 * EGR_MIRROR_FLEX_EDITOR_WIDTH];
    uint8_t *p_prof_hdr, *p_prof_hdr_s;
    uint32_t mask[3], def_proto_nh = 0, vlan;
    int len_offset = 0, len_adjust = 0;
    int udp_len_offset = 0, udp_len_adjust = 0;
    uint8_t msb_max_cont_num, lsb_max_cont_num;
    FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1_TABLEm_t profile_1;
    FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_2_TABLEm_t profile_2;
    FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_3_TABLEm_t profile_3;
    FLEX_EDITOR_MIRROR_ENCAP_FIELD_0_PROFILE_TABLEm_t field_0;
    EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm_t attr;
    uint32_t *p_profile_1_hdr, *p_profile_2_hdr, *p_profile_3_hdr;
    uint32_t *p_field_0;
    bcmdrd_sid_t pt_id_1, pt_id_2, pt_id_3, pt_id_field_0;
    bcmdrd_fid_t pt_fid;
    uint32_t fval, trans_id, i;
    bcmltd_sid_t lt_id;
    int index = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    trans_id = entry->trans_id;
    lt_id = entry->lt_id;
    index = entry->encap_id;

    /* Zone-0 reset: Flex editor encap profile. */
    pt_id_1 = FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_1_TABLEm;
    pt_id_2 = FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_2_TABLEm;
    pt_id_3 = FLEX_EDITOR_MIRROR_ENCAP_0_PROFILE_3_TABLEm;

    p_prof_hdr_s = p_profile_hdr;
    p_prof_hdr = p_profile_hdr;

    p_profile_1_hdr = (uint32_t *) &profile_1;
    p_profile_2_hdr = (uint32_t *) &profile_2;
    p_profile_3_hdr = (uint32_t *) &profile_3;

    sal_memset(p_profile_hdr, 0, sizeof(p_profile_hdr));
    sal_memset(p_profile_1_hdr, 0, sizeof(profile_1));
    sal_memset(p_profile_2_hdr, 0, sizeof(profile_2));
    sal_memset(p_profile_3_hdr, 0, sizeof(profile_3));

    /* Mirror Bitp Configuration. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56880_a0_cth_mirror_bitp_set(unit, entry));

    /* Zone-0 reset: Sequence number. */
    pt_id_field_0 = FLEX_EDITOR_MIRROR_ENCAP_FIELD_0_PROFILE_TABLEm;
    p_field_0 = (uint32_t *) &field_0;
    sal_memset(p_field_0, 0, sizeof(field_0));

    /* Get truncation status. */
    EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm_CLR(attr);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mirror_pt_read(unit, trans_id, lt_id,
                               EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm,
                               index, &attr));
    fval = EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLEm_TRUNCATE_ENf_GET(attr);
    if (fval == 1) {
        truncate = TRUE;
    }

    /* Read MIRROR_ENCAP_TRUNCATE_MODE and MIRROR_ENCAP_TRUNCATE_PTR. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mirror_pt_read(unit, trans_id, lt_id,
                               pt_id_3, index, p_profile_3_hdr));

    /* IPv6 protocol */
    if (entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6 ||
        entry->type == BCMCTH_MIRROR_ENCAP_VXLAN_IPV6) {
        ipv6 = TRUE;
    }

    /* Protocol and encap. format. */
    switch (entry->type) {
        case BCMCTH_MIRROR_ENCAP_BASIC:
            basic = TRUE;
            break;
        case BCMCTH_MIRROR_ENCAP_RSPAN:
            rspan = TRUE;
            if (entry->vntag) {
                vntag = TRUE;
            }
            break;
        case BCMCTH_MIRROR_ENCAP_ERSPAN:
        case BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6:
            erspan = TRUE;
            if (entry->gre_header == ERSPAN_GRE_PROTOCOL_TYPE_3) {
                erspan3 = TRUE;
            }
            if (entry->sq) {
                erspan_seq = TRUE;
            }
            break;
        case BCMCTH_MIRROR_ENCAP_SFLOW:
        case BCMCTH_MIRROR_ENCAP_SFLOW_IPV6:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ:
        case BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6:
            sflow = TRUE;
            if (entry->vntag) {
                vntag = TRUE;
            }
            break;
        case BCMCTH_MIRROR_ENCAP_PSAMP:
        case BCMCTH_MIRROR_ENCAP_PSAMP_IPV6:
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA:
        case BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6:
            psamp = TRUE;
            break;
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP:
        case BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6:
            mirr_on_drop = TRUE;
            break;
        case BCMCTH_MIRROR_ENCAP_VXLAN:
        case BCMCTH_MIRROR_ENCAP_VXLAN_IPV6:
            vxlan = TRUE;
            vxlan_flex = entry->vni_valid ? FALSE : TRUE;
            break;
        default:
            break;
    }

    /* Add an outer VLAN tag into the encapsulation header. */
    otag = entry->outer_vlan;

    /* Add HiGig3 Base Header into the encapsulation header. */
    higig3 = entry->higig3;

    if (entry->flexhdr_encap) {
        /*
         *  e.g. generic_loopback_t
         *
         *  struct generic_loopback_t {
         *      fields {
         *          bit[8]        start_byte;
         *          bit[4]        header_type;
         *          bit[4]        flags;
         *          bit[4]        input_priority;
         *          bit[4]        reserved_1;
         *          bit[4]        interface_ctrl;
         *          bit[4]        processing_ctrl_0;
         *          bit[4]        processing_ctrl_1;
         *          bit[4]        destination_type;
         *          bit[8]        qos_obj;
         *          bit[16]       destination_obj;
         *          bit[16]       source_system_port;
         *          bit[16]       interface_obj;
         *          bit[16]       entropy_obj;
         *          bit[16]       reserved_2;
         *      }
         *  }
         */
        for (i = 0;
             i < SHR_BYTES2WORDS(entry->flexhdr_size);
             i++) {
            *(uint32_t *)p_prof_hdr = shr_htonl(entry->flexhdr[i]);
            p_prof_hdr += sizeof(uint32_t);
        }
        /*
         * Copy every 4 bytes and need to minus the extra bytes copied
         * flexhdr_size = 16, copy 4 words (16 Bytes) and minus 0 byte.
         * flexhdr_size = 17, copy 5 words (20 Bytes) and minus 3 bytes.
         * flexhdr_size = 18, copy 5 words (20 Bytes) and minus 2 bytes.
         * flexhdr_size = 19, copy 5 words (20 Bytes) and minus 1 byte.
 */
        if ((entry->flexhdr_size % 4) != 0) {
            p_prof_hdr -=
                sizeof(uint8_t) * (4 - (entry->flexhdr_size % 4));
        }
    }


    /* Zone-0 udate: Flex editor encap profile. */
    if (erspan || sflow || vxlan || psamp || mirr_on_drop) {

        /*
         * ERSPAN:
         *  LB(16)/L2(12)/HiGig3(8)/Dot1Q(4)/EtherType(2)/IP/GRE
         *
         * VXLAN:
         *  LB(16)/L2(12)/HiGig3(8)/Dot1Q(4)/EtherType(2)/IP/UDP(8)/VXLAN(8)
         *
         * SFLOW:
         *  LB(16)/L2(12)/HiGig3(8)/Dot1Q(4)/EtherType(2)/IP/UDP(8)/SFLOW
         *
         * PSAMP:
         *  LB(16)/L2(12)/HiGig3(8)/Dot1Q(4)/EtherType(2)/IP/UDP(8)/PSAMP
         *
         * Mirror-on-Drop:
         *  LB(16)/L2(12)/HiGig3(8)/Dot1Q(4)/EtherType(2)/IP/UDP(8)/MOD
         */

        /* Construct MAC and VLAN information L2 Stuff. */
        *(uint32_t *)p_prof_hdr =
            shr_htonl((entry->dst_mac >> 16) & 0xFFFFFFFF);
        p_prof_hdr += sizeof(uint32_t);
        *(uint32_t *)p_prof_hdr =
            shr_htonl(((entry->dst_mac << 16) & 0xFFFF0000) |
                            ((entry->src_mac >> 32) & 0xFFFF));
        p_prof_hdr += sizeof(uint32_t);
        *(uint32_t *)p_prof_hdr =
            shr_htonl((entry->src_mac >> 0) & 0xFFFFFFFF);
        p_prof_hdr += sizeof(uint32_t);
        /* For VNTAG. */
        if (vntag) {
            *(uint16_t *)p_prof_hdr = shr_htons(IPV4_NIV_ETHER_TYPE);
            p_prof_hdr += sizeof(uint16_t); /* VNTAG ether type 2 bytes. */

            *(uint16_t *)p_prof_hdr = shr_htons(entry->niv_dst_vif);
            p_prof_hdr += sizeof(uint16_t);

            *(uint16_t *)p_prof_hdr = shr_htons(entry->niv_src_vif);
            p_prof_hdr += sizeof(uint16_t);
        }

        /* Add HiGig3 Base Header. */
        if (higig3) {
            *(uint16_t *)p_prof_hdr = shr_htons(entry->higig3_ethertype);
            p_prof_hdr += sizeof(uint16_t);
            *(uint16_t *)p_prof_hdr =
                shr_htons((entry->higig3_basehdr >> 32) & 0xFFFF);
            p_prof_hdr += sizeof(uint16_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl(entry->higig3_basehdr & 0xFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
        }

        /* For OTAG. */
        if (otag) {
            tagged = TRUE;
            *(uint16_t *)p_prof_hdr = shr_htons(entry->tpid);
            p_prof_hdr += sizeof(uint16_t);
            vlan = (entry->vlan_id & PKT_VLAN_MASK) |
                   (PKT_CFI(entry->cfi)) |
                   (PKT_PRIO(entry->pri));
            *(uint16_t *)p_prof_hdr = shr_htons(vlan);
            p_prof_hdr += sizeof(uint16_t);
        }

        /* Set default IPv4 Proto and IPv6 next header */
        def_proto_nh = erspan ? GRE_PROTOCOL : UDP_PROTOCOL;

        if (ipv6) {
            /* Add Ether Type. */
            *(uint16_t *)p_prof_hdr = (entry->ethertype) ?
                                      shr_htons(entry->ethertype) :
                                      shr_htons(IPV6_ETHER_TYPE);
            p_prof_hdr += sizeof(uint16_t);

            /* Construct IPV6 Information. */
            len_adjust = TWOS_COMPLEMENT(p_prof_hdr - p_prof_hdr_s);

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
             * Traffic Class is same as Type Of Service field in IPv4.
             */

            /* Traffic class. */
            traffic_class = (entry->tos_tc) ?
                            (uint16_t)(entry->tos_tc) : (IPV6_TRAFFIC_CLASS);

            /* Version and traffic class upper nibble. */
            *p_prof_hdr = ((IPV6_VERSION << NIBBLE_SHIFT_OFFSET) |
                           (traffic_class >> NIBBLE_SHIFT_OFFSET));
            p_prof_hdr += sizeof(uint8_t);

            /* Traffic class lower nibble. */
            flow_label = (uint16_t)(entry->fl >> 16);
            *p_prof_hdr = (((traffic_class & 0xF) << NIBBLE_SHIFT_OFFSET) |
                            (flow_label & 0xF));
            p_prof_hdr += sizeof(uint8_t);

            /* Flow label. */
            *(uint16_t *)p_prof_hdr = shr_htons((uint16_t)entry->fl);
            p_prof_hdr += sizeof(uint16_t);

            len_offset = p_prof_hdr - p_prof_hdr_s;

            /* Payload length. */
            *(uint16_t *)p_prof_hdr = 0;
            p_prof_hdr += sizeof(uint16_t);

            /* Next Header. */
            *(p_prof_hdr) = (entry->proto_nh) ?
                             entry->proto_nh :
                             def_proto_nh;
            p_prof_hdr += sizeof(uint8_t);

            /* HOP Limit. */
            *p_prof_hdr = (entry->ttl_hl) ?
                          (entry->ttl_hl) : (IPV6_HOP_LIMIT);
            p_prof_hdr += sizeof(uint8_t);

            /* Source IPv6. */
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->src_ipv6[1] >> 32) &
                               0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->src_ipv6[1]) & 0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->src_ipv6[0] >> 32) &
                               0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->src_ipv6[0]) & 0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);

            /* Dest IPv6. */
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->dst_ipv6[1] >> 32) &
                               0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->dst_ipv6[1]) & 0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->dst_ipv6[0] >> 32) &
                               0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
            *(uint32_t *)p_prof_hdr =
                shr_htonl((entry->dst_ipv6[0]) & 0xFFFFFFFFFFFFFFFF);
            p_prof_hdr += sizeof(uint32_t);
        } else {
            /* Add Ether Type. */
            *(uint16_t *)p_prof_hdr = (entry->ethertype) ?
                                      shr_htons(entry->ethertype) :
                                      shr_htons(IPV4_ETHER_TYPE);
            p_prof_hdr += sizeof(uint16_t);

            /* Construct IPV4 Information. */
            len_adjust = TWOS_COMPLEMENT(p_prof_hdr - p_prof_hdr_s);

            /*
             *                     IPv4 HEADER FORMAT
             * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
             * | | | | | | | | | | |1|1|1|1|1|1|1|1|1|1|2|2|2|2|2|2|2|2|2|2|3|3|
             * |0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|2|3|4|5|6|7|8|9|0|1|
             * +-------+-------+---------------+-------------------------------+
             * | Vers  |  Len  |      ToS      |        Total Length           |
             * +-------+-------+---------------+-----+-------------------------+
             * |       Identification          |Flags|    Fragment Offset      |
             * +---------------+---------------+-----+-------------------------+
             * |      TTL      |   Protocol    |        Checksum               |
             * +---------------+---------------+-----+-------------------------+
             * |                  Source IP Address (32 bits)                  |
             * +---------------------------------------------------------------+
             * |               Destination IP Address (32 bits)                |
             * +---------------------------------------------------------------+
             */

            *p_prof_hdr = ((IPV4_VERSION << NIBBLE_SHIFT_OFFSET) |
                           (IPV4_HEADER_LENGTH_WORDS));
            p_prof_hdr += sizeof(uint8_t);

            /* Tos. */
            *p_prof_hdr = entry->tos_tc;
            p_prof_hdr += sizeof(uint8_t);

            len_offset =  p_prof_hdr - p_prof_hdr_s;

            /* From length move to flags field.*/
            p_prof_hdr += sizeof(uint32_t);

            /* Set DF flag. */
            *p_prof_hdr = entry->df ? (1 << DF_FLAG_SHIFTER) : 0;
            p_prof_hdr += sizeof(uint8_t);

            /* Fragment offset. */
            p_prof_hdr += sizeof(uint8_t);

            /* Time to Live. */
            *p_prof_hdr = entry->ttl_hl;
            p_prof_hdr += sizeof(uint8_t);

            /* Proto. */
            *(p_prof_hdr) = (entry->proto_nh) ?
                             entry->proto_nh :
                             def_proto_nh;
            p_prof_hdr += sizeof(uint8_t);

            /* Checksum. */
            p_prof_hdr += sizeof(uint16_t);

            /* Source IPv4. */
            *(uint32_t *)p_prof_hdr = shr_htonl(entry->src_ipv4);
            p_prof_hdr += sizeof(uint32_t);

            /* Dest IPv4. */
            *(uint32_t *)p_prof_hdr = shr_htonl(entry->dst_ipv4);
            p_prof_hdr += sizeof(uint32_t);
        }

        if (erspan) {
            /* Construct GRE Protocol. */
            if (erspan3 || erspan_seq) {
                /* Only sequence number present bit is set in erspan3. */
                *(uint16_t *)p_prof_hdr = shr_htons(0x1000);
            }
            p_prof_hdr += sizeof(uint16_t);

            gre_header = (entry->gre_header) ?
                         (entry->gre_header) : (ERSPAN_GRE_PROTOCOL_TYPE_2);
            *(uint16_t *)p_prof_hdr = shr_htons(gre_header);
            p_prof_hdr += sizeof(uint16_t);
        } else {
            /* Construct UDP Information */
            udp_len_adjust = TWOS_COMPLEMENT(p_prof_hdr - p_prof_hdr_s);

            *(uint16_t *)p_prof_hdr = shr_htons(entry->udp_src_port);
            p_prof_hdr += sizeof(uint16_t);

            *(uint16_t *)p_prof_hdr = shr_htons(entry->udp_dst_port);
            p_prof_hdr += sizeof(uint16_t);

            udp_len_offset = p_prof_hdr - p_prof_hdr_s;

            /* UDP Length and check sum */
            p_prof_hdr += sizeof(uint32_t);

            if (vxlan) {
                if (vxlan_flex) {
                    /*
                     * VXLAN encapsulation using flex engine
                     * (VXLAN header will be added by flex engine and
                     * ERSPAN portion is added by mirror profile configs)
                     */
                } else {
                    /* 8 bits encap, 24 bits reserved */
                    *(uint32_t *)p_prof_hdr =
                        shr_htonl(PKT_ENCAP_VXLAN << 24);
                    p_prof_hdr += sizeof(uint32_t);

                    /* 24 bits VNI, 8 bits reserved */
                    *(uint32_t *)p_prof_hdr = shr_htonl(entry->vni << 8);
                    p_prof_hdr += sizeof(uint32_t);
                }
            }
        }
    } else if (rspan) {
        /* For VNTAG. */
        if (vntag) {
            *(uint16_t *)p_prof_hdr = shr_htons(IPV4_NIV_ETHER_TYPE);
            p_prof_hdr += sizeof(uint16_t); /* VNTAG ether type 2 bytes. */

            *(uint16_t *)p_prof_hdr = shr_htons(entry->niv_dst_vif);
            p_prof_hdr += sizeof(uint16_t);

            *(uint16_t *)p_prof_hdr = shr_htons(entry->niv_src_vif);
            p_prof_hdr += sizeof(uint16_t);
        }

        /* RSPAN construct */
        tagged = TRUE;
        *(uint16_t *)p_prof_hdr = shr_htons(entry->tpid);
        p_prof_hdr += sizeof(uint16_t);
        vlan = (entry->vlan_id & PKT_VLAN_MASK) |
               (PKT_CFI(entry->cfi)) |
               (PKT_PRIO(entry->pri));
        *(uint16_t *)p_prof_hdr = shr_htons(vlan);
        p_prof_hdr += sizeof(uint16_t);
    } else if (basic) {
        /* For no-encap mirror. */
    } else {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    /* Reverse Copy -- Could be endian thingee -- for bcmsim purposes. */
    {
        int mcnt;
        uint32_t tmp, idx;

        for (mcnt = 0; mcnt < EGR_MIRROR_FLEX_EDITOR_WIDTH / 4; mcnt++) {
            /* ENCAP_DATA_MSB */
            idx = EGR_MIRROR_FLEX_EDITOR_WIDTH - 4 * (mcnt + 1);
            tmp = *(uint32_t *)(p_profile_hdr + idx);
            p_profile_1_hdr[mcnt] = shr_htonl(tmp);
            /* ENCAP_DATA_LSB */
            idx = EGR_MIRROR_FLEX_EDITOR_WIDTH * 2  - 4 * (mcnt + 1);
            tmp = *(uint32_t *)(p_profile_hdr + idx);
            p_profile_2_hdr[mcnt] = shr_htonl(tmp);
        }
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "len_offset=%d udp_len_offset=%d\n"),
               len_offset, udp_len_offset));
    /* Update PROFILE_3_TABLE */
    if (len_offset) {
        int hdr_chksum_offset = len_offset + 8;

        if (!ipv6) {
            /* Valid 1 */
            pt_fid = MIRROR_ENCAP_CHECKSUM_0_VALIDf;
            fval = 1;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                    pt_fid, (uint32_t *) (&fval)));

            /* Starting offset of the checksum in the header relative to MSB. */
            pt_fid = MIRROR_ENCAP_CHECKSUM_0_FIELD_REL_OFFSETf;
            /* mirror_encap_checksum_0_field_rel_offset */
            fval = hdr_chksum_offset;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                    pt_fid, (uint32_t *) (&fval)));
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "hdr_chksum_offset=%d\n"),
                       hdr_chksum_offset));

            /* no vlan: subtract 4 bytes of tpid and vlan. */
            if (higig3) {
                /* mirror_encap_checksum_0_mask = 0x3ffffc00000000 */
                mask[0] = 0x00000000;
                mask[1] = 0x003ffffc;
                mask[2] = 0x00000000;
            } else if (tagged == FALSE) {
                mask[0] = 0x00000000;
                mask[1] = 0xffffc000;
                mask[2] = 0x00000003;
            } else {
                /* mirror_encap_checksum_0_mask = 0x3ffffc0000000000 */
                mask[0] = 0x00000000;
                mask[1] = 0x3ffffc00;
                mask[2] = 0x00000000;
            }

            pt_fid = MIRROR_ENCAP_CHECKSUM_0_MASKf;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit,
                                    pt_id_3,
                                    p_profile_3_hdr,
                                    pt_fid,
                                    (uint32_t *) (&mask)));
        }

        /* Update the packet length. */
        pt_fid = MIRROR_ENCAP_PKT_LEN_0_VALIDf;
        fval = 1;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                pt_fid, (uint32_t *) (&fval)));

        /* Starting offset of the packet length in the header relative to MSB. */
        pt_fid = MIRROR_ENCAP_PKT_LEN_0_FIELD_REL_OFFSETf;
        fval = len_offset;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                pt_fid, (uint32_t *) (&fval)));
        if (truncate) {
            if (psamp) {
                /* 2s complement adjust to encapsulated payload length. */
                pt_fid = MIRROR_ENCAP_PKT_LEN_0_ADJUSTf;
                fval = PSAMP_TRUNCATE_LEN_ADJUST;
                SHR_IF_ERR_EXIT(
                    bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                        pt_fid, (uint32_t *) (&fval)));
                /* 2:Packet length after truncation. */
                pt_fid = MIRROR_ENCAP_PKT_LEN_0_SRCf;
                fval = 2;
                SHR_IF_ERR_EXIT(
                    bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                        pt_fid, (uint32_t *) (&fval)));
                if (udp_len_offset) {
                    /* 2s complement adjust to encapsulated payload length. */
                    pt_fid = MIRROR_ENCAP_PKT_LEN_1_ADJUSTf;
                    fval = PSAMP_TRUNCATE_UDP_LEN_ADJUST;
                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                            pt_fid, (uint32_t *) (&fval)));
                    /* 2:Packet length after truncation. */
                    pt_fid = MIRROR_ENCAP_PKT_LEN_1_ADJUSTf;
                    fval = 2;
                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                            pt_fid, (uint32_t *) (&fval)));
                }
            } else {
                /*
                 * when MIRROR_ENCAP_PKT_LEN_0_SRC = 2, the length is
                 * the original truncated packet length, need to add
                 * the encap header length.
                 */
                int truncate_ip_len_adjust = 0;
                int truncate_udp_len_adjust = 0;
                if (sflow || vxlan || erspan) {
                    /* psamp is already done above. */

                    if (sflow) {
                        truncate_ip_len_adjust = SFLOW_HEADER_LEN;
                        truncate_ip_len_adjust += UDP_HEADER_LEN;
                        truncate_udp_len_adjust = truncate_ip_len_adjust;
                    } else if (vxlan) {
                        truncate_ip_len_adjust = VXLAN_HEADER_LEN;
                        truncate_ip_len_adjust += UDP_HEADER_LEN;
                        truncate_udp_len_adjust = truncate_ip_len_adjust;
                    } else if (erspan) {
                        truncate_ip_len_adjust = GRE_HEADER_LEN;
                    }
                    if (ipv6) {
                        truncate_ip_len_adjust += IPV6_HEADER_LEN;
                    } else {
                        truncate_ip_len_adjust += IPV4_HEADER_LEN;
                    }
                }
                if (truncate_ip_len_adjust) {
                    /* 2s complement adjust to encapsulated payload length. */
                    pt_fid = MIRROR_ENCAP_PKT_LEN_0_ADJUSTf;
                    fval = truncate_ip_len_adjust;
                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                            pt_fid, (uint32_t *) (&fval)));
                    /* 2:Packet length after truncation. */
                    pt_fid = MIRROR_ENCAP_PKT_LEN_0_SRCf;
                    fval = 2;
                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                            pt_fid, (uint32_t *) (&fval)));
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "MIRROR_ENCAP_PKT_LEN_0_ADJUST=%d"
                                          "MIRROR_ENCAP_PKT_LEN_0_SRC=%d\n"),
                               truncate_ip_len_adjust, fval));
                }
                if (truncate_udp_len_adjust) {
                    /* 2s complement adjust to encapsulated payload length. */
                    pt_fid = MIRROR_ENCAP_PKT_LEN_1_ADJUSTf;
                    fval = truncate_udp_len_adjust;
                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                            pt_fid, (uint32_t *) (&fval)));
                    /* 2:Packet length after truncation. */
                    pt_fid = MIRROR_ENCAP_PKT_LEN_1_SRCf;
                    fval = 2;
                    SHR_IF_ERR_EXIT(
                        bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                            pt_fid, (uint32_t *) (&fval)));
                    LOG_DEBUG(BSL_LOG_MODULE,
                              (BSL_META_U(unit,
                                          "MIRROR_ENCAP_PKT_LEN_1_ADJUST=%d"
                                          "MIRROR_ENCAP_PKT_LEN_1_SRC=%d\n"),
                               truncate_ip_len_adjust, fval));
                }
            }
        } else {
            /* 2s complement adjust to encapsulated payload length. */
            pt_fid = MIRROR_ENCAP_PKT_LEN_0_ADJUSTf;
            fval = (ipv6) ? udp_len_adjust : len_adjust;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                    pt_fid, (uint32_t *) (&fval)));
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "MIRROR_ENCAP_PKT_LEN_0_ADJUST=%d"),
                       fval));
            /* 1:Packet length after edited packet. */
            pt_fid = MIRROR_ENCAP_PKT_LEN_0_SRCf;
            fval = 1;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                    pt_fid, (uint32_t *) (&fval)));
            if (udp_len_offset) {
                /* 2s complement adjust to encapsulated payload length. */
                pt_fid = MIRROR_ENCAP_PKT_LEN_1_ADJUSTf;
                fval = udp_len_adjust;
                SHR_IF_ERR_EXIT(
                    bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                        pt_fid, (uint32_t *) (&fval)));
                LOG_DEBUG(BSL_LOG_MODULE,
                          (BSL_META_U(unit,
                                      "MIRROR_ENCAP_PKT_LEN_1_ADJUST=%d"),
                           fval));
                /* 1:Packet length after edited packet. */
                pt_fid = MIRROR_ENCAP_PKT_LEN_1_SRCf;
                fval = 1;
                SHR_IF_ERR_EXIT(
                    bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                        pt_fid, (uint32_t *) (&fval)));
            }
        }

        if (udp_len_offset) {
            /* Update the packet length. */
            pt_fid = MIRROR_ENCAP_PKT_LEN_1_VALIDf;
            fval = 1;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                    pt_fid, (uint32_t *) (&fval)));
            /*
             * Starting offset of the packet length in the header relative to
             * MSB.
             */
            pt_fid = MIRROR_ENCAP_PKT_LEN_1_FIELD_REL_OFFSETf;
            fval = udp_len_offset;
            SHR_IF_ERR_EXIT(
                bcmdrd_pt_field_set(unit, pt_id_3, p_profile_3_hdr,
                                    pt_fid, (uint32_t *) (&fval)));
            LOG_DEBUG(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "udp_len_offset=%d\n"),
                       udp_len_offset));
        }
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit,
                          "seq_num_0_src_sel=%d\n"
                          "seq_num_1_src_sel=%d\n"
                          "seq_num_1_cont_num=%d\n"
                          "seq_num_1_cont_num=%d\n"),
               variant_data[unit]->seq_num_0_src_sel,
               variant_data[unit]->seq_num_1_src_sel,
               variant_data[unit]->seq_num_0_cont_num,
               variant_data[unit]->seq_num_1_cont_num));

    if (erspan_seq) {
        /* Sequence number is 32bit, hence select two 16-bit from object bus. */

        /* Indicate the field is valid. */
        pt_fid = MIRROR_FIELD_16BIT_0_VALIDf;
        fval = 1;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));
        pt_fid = MIRROR_FIELD_16BIT_1_VALIDf;
        fval = 1;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));
        /*
         * Select the bus (field bus, computed bus, etc) from Box 1 for extraction.
         * 0 : Select user fields
         * 1 : Select object bus 0
         * 2 : Select object bus 1 and 2
         * 3 : Select field bus 0
         * 4 : Select field bus 1
         * 5 : Select scratch bus from timestamp sfc
         * 6 : RSVD_0
         * 6 : RSVD_1
         */

        /* LSB 16-bit of Sequence number. */
        pt_fid = MIRROR_FIELD_16BIT_0_SRC_SELf;
        if (variant_data[unit]->seq_num_0_src_sel == 0) {
            /* LSB 16-bit is present in BUS_EGR_OBJ0, select object bus 0. */
            fval = 1;
            lsb_max_cont_num = 32;
        } else if (variant_data[unit]->seq_num_0_src_sel == 1) {
            /* LSB 16-bit is present in BUS_EGR_OBJ1, select object bus 1. */
            fval = 2;
            lsb_max_cont_num = 16;
        } else {
            /* LSB 16-bit isn't present in BUS_EGR_OBJ0 or BUS_EGR_OBJ1. */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "seq_num_0_src_sel = %d \n"),
                      variant_data[unit]->seq_num_0_src_sel));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));

        /* MSB 16-bit of Sequence number. */
        pt_fid = MIRROR_FIELD_16BIT_1_SRC_SELf;
        if (variant_data[unit]->seq_num_1_src_sel == 0) {
            /* MSB 16-bit is present in BUS_EGR_OBJ0, select object bus 0. */
            fval = 1;
            msb_max_cont_num = 32;
        } else if (variant_data[unit]->seq_num_1_src_sel == 1) {
            /* MSB 16-bit is present in BUS_EGR_OBJ1, select object bus 1. */
            fval = 2;
            msb_max_cont_num = 16;
        } else {
            /* MSB 16-bit isn't present in BUS_EGR_OBJ0 or BUS_EGR_OBJ1. */
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "seq_num_1_src_sel = %d \n"),
                      variant_data[unit]->seq_num_1_src_sel));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));
        /* Selects the field from the selected Bus by BUS_SEL. */
        pt_fid = MIRROR_FIELD_16BIT_0_INDEXf;

        /*
         * index_0_16bit is where LSB portion of sequence number is present in
         * BUS_EGR_OBJ0/BUS_EGR_OBJ1.
         *  (number of COUTAINERs in BUS_EGR_OBJ0/1) - (egr_obj0/1_cont_Y) - 1
         */
        fval = lsb_max_cont_num - variant_data[unit]->seq_num_0_cont_num - 1;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));

        pt_fid = MIRROR_FIELD_16BIT_1_INDEXf;
        /*
         * index_1_16bit is where MSB portion of sequence number is present in
         * BUS_EGR_OBJ0/BUS_EGR_OBJ1.
         *  (number of COUTAINERs in BUS_EGR_OBJ0/1) - (egr_obj0/1_cont_Y) - 1
         */
        fval = msb_max_cont_num - variant_data[unit]->seq_num_1_cont_num - 1;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));

        /* SHIFT_OFFSET_0 specify from where sequence number MSB should start. */
        pt_fid = MIRROR_FIELD_16BIT_0_SHIFT_OFFSETf;
        /* seq_num_0_shift */
        fval = ipv6 ? 32 : 22 ;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));

        /* SHIFT_OFFSET_1 specify from where sequence number LSB should start. */
        pt_fid = MIRROR_FIELD_16BIT_1_SHIFT_OFFSETf;
        /* seq_num_1_shift */
        fval = ipv6 ? 31 : 21 ;
        SHR_IF_ERR_EXIT(
            bcmdrd_pt_field_set(unit, pt_id_field_0, p_field_0,
                                pt_fid, (uint32_t *) (&fval)));
    }

    /* Zone-0 write: Flex editor encap profile. */
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_1, index, p_profile_1_hdr));
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_2, index, p_profile_2_hdr));
    SHR_IF_ERR_EXIT
        (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                pt_id_3, index, p_profile_3_hdr));
    if (erspan_seq) {
        /* Zone-0 write: Sequence number. */
        SHR_IF_ERR_EXIT
            (bcmcth_mirror_pt_write(unit, trans_id, lt_id,
                                    pt_id_field_0, index, p_field_0));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_flex_editor_update(
    int unit,
    bcmcth_mirror_encap_t *entry,
    bool enable)
{
    bool sflow = FALSE, erspan = FALSE, psamp = FALSE;
    bool vxlan = FALSE, mirr_on_drop = FALSE, basic = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    if (enable) {
        if (entry->type == BCMCTH_MIRROR_ENCAP_BASIC) {
            basic = TRUE;
        }

        if (entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN ||
            entry->type == BCMCTH_MIRROR_ENCAP_ERSPAN_IPV6) {
            erspan = TRUE;
        }

        if (entry->type == BCMCTH_MIRROR_ENCAP_SFLOW ||
            entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_IPV6 ||
            entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_SEQ ||
            entry->type == BCMCTH_MIRROR_ENCAP_SFLOW_SEQ_IPV6) {
            sflow = TRUE;
        }

        if (entry->type == BCMCTH_MIRROR_ENCAP_VXLAN ||
            entry->type == BCMCTH_MIRROR_ENCAP_VXLAN_IPV6 ) {
            vxlan = TRUE;
        }

        if (entry->type == BCMCTH_MIRROR_ENCAP_PSAMP ||
            entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_IPV6 ||
            entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_METADATA ||
            entry->type == BCMCTH_MIRROR_ENCAP_PSAMP_METADATA_IPV6) {
            psamp = TRUE;
        }

        if (entry->type == BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP ||
            entry->type == BCMCTH_MIRROR_ENCAP_MIRROR_ON_DROP_IPV6 ) {
            mirr_on_drop = TRUE;
        }

        /* Zone-0 insert. */
        if (basic || erspan ||sflow || vxlan || psamp || mirr_on_drop) {
            SHR_IF_ERR_EXIT
                (bcm56880_a0_cth_mirror_encap_z0_edit_ins_set(unit, entry));
            SHR_IF_ERR_EXIT
                (bcm56880_a0_cth_mirror_encap_z0_encap_prof_set(unit, entry));
        }

        /* Zone-2 Shim. */
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_z2_shim_set(unit, entry));
    } else {
        /* Zone-0 insert. */
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_z0_edit_ins_del(unit, entry));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_z0_encap_prof_del(unit, entry));

        /* Zone-2 Shim. */
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_z2_shim_del(unit, entry));
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcm56880_a0_cth_mirror_encap_op(
    int unit,
    bcmcth_mirror_encap_op_t op,
    bcmcth_mirror_encap_t *entry)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(entry, SHR_E_PARAM);

    BCMCTH_MIRROR_ENCAP_FEATURE_CHECK(unit);

    switch (op) {
    case BCMCTH_MIRROR_ENCAP_OP_SET:
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_type_update(
                unit, entry, TRUE));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_flex_editor_update(
                unit, entry, TRUE));
        break;
    case BCMCTH_MIRROR_ENCAP_OP_DEL:
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_type_update(
                unit, entry, FALSE));
        SHR_IF_ERR_EXIT
            (bcm56880_a0_cth_mirror_encap_flex_editor_update(
                unit, entry, FALSE));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

exit:
    SHR_FUNC_EXIT();

}

static int
bcm56880_a0_cth_mirror_deinit(int unit)
{
    variant_data[unit] = NULL;
    return SHR_E_NONE;
}

static int
bcm56880_a0_cth_mirror_init(
    int unit, int warm, const bcmcth_mirror_data_t *data)
{
    variant_data[unit] = data;
    return SHR_E_NONE;
}

static bcmcth_mirror_drv_t bcm56880_a0_cth_mirror_drv = {
    .init = bcm56880_a0_cth_mirror_init,
    .deinit = bcm56880_a0_cth_mirror_deinit,
    .encap_op = &bcm56880_a0_cth_mirror_encap_op,
};

/*******************************************************************************
 * Public functions
 */

bcmcth_mirror_drv_t *
bcm56880_a0_cth_mirror_drv_get(int unit)
{
    return &bcm56880_a0_cth_mirror_drv;
}
