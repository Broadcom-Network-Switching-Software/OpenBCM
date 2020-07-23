/*! \file lb_hash.c
 *
 * XGS LB hash management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm/switch.h>
#include <bcm_int/ltsw/xgs/lb_hash.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/l3.h>
#include <bcm_int/ltsw/lb_hash.h>
#include <bcm_int/ltsw/lb_hash_int.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/trunk.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_bitop.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_lb_hash_constants.h>
/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_HASH

typedef struct lb_hash_fields_info_s {
    uint32_t flags;
    const char *field_name;
} lb_hash_fields_info_t;

uint32_t ipv4_fields[] = {
    BCM_HASH_FIELD_DSTMOD,
    BCM_HASH_FIELD_DSTPORT,
    BCM_HASH_FIELD_SRCMOD,
    BCM_HASH_FIELD_SRCPORT,
    BCM_HASH_FIELD_PROTOCOL,
    BCM_HASH_FIELD_DSTL4,
    BCM_HASH_FIELD_SRCL4,
    BCM_HASH_FIELD_VLAN,
    BCM_HASH_FIELD_IP4DST_LO,
    BCM_HASH_FIELD_IP4DST_HI,
    BCM_HASH_FIELD_IP4SRC_LO,
    BCM_HASH_FIELD_IP4SRC_HI,
    BCM_HASH_FIELD_CNTAG_FLOW_ID
};

static const char *ipv4_hash0_fields_str[] = {
    HASH0_DST_MODIDs,
    HASH0_DST_PORTs,
    HASH0_SRC_MODIDs,
    HASH0_SRC_PORTs,
    HASH0_PROTOCOLs,
    HASH0_L4_DSTs,
    HASH0_L4_SRCs,
    HASH0_VIDs,
    HASH0_DST_IP_LOWERs,
    HASH0_DST_IP_UPPERs,
    HASH0_SRC_IP_LOWERs,
    HASH0_SRC_IP_UPPERs,
    HASH0_CNTAG_RPIDs
};

static const char *ipv4_hash1_fields_str[] = {
    HASH1_DST_MODIDs,
    HASH1_DST_PORTs,
    HASH1_SRC_MODIDs,
    HASH1_SRC_PORTs,
    HASH1_PROTOCOLs,
    HASH1_L4_DSTs,
    HASH1_L4_SRCs,
    HASH1_VIDs,
    HASH1_DST_IP_LOWERs,
    HASH1_DST_IP_UPPERs,
    HASH1_SRC_IP_LOWERs,
    HASH1_SRC_IP_UPPERs,
    HASH1_CNTAG_RPIDs
};


uint32_t ipv6_fields[] = {
    BCM_HASH_FIELD_FLOWLABEL_LO,
    BCM_HASH_FIELD_FLOWLABEL_HI,
    BCM_HASH_FIELD_SRCMOD,
    BCM_HASH_FIELD_SRCPORT,
    BCM_HASH_FIELD_NXT_HDR,
    BCM_HASH_FIELD_DSTL4,
    BCM_HASH_FIELD_SRCL4,
    BCM_HASH_FIELD_VLAN,
    BCM_HASH_FIELD_IP6DST_LO,
    BCM_HASH_FIELD_IP6DST_HI,
    BCM_HASH_FIELD_IP6SRC_LO,
    BCM_HASH_FIELD_IP6SRC_HI,
    BCM_HASH_FIELD_CNTAG_FLOW_ID
};

static const char *ipv6_hash0_fields_str[] = {
    HASH0_FLOW_ID_LOs,
    HASH0_FLOW_ID_HIs,
    HASH0_SRC_MODIDs,
    HASH0_SRC_PORTs,
    HASH0_PROTOCOLs,
    HASH0_L4_DSTs,
    HASH0_L4_SRCs,
    HASH0_VIDs,
    HASH0_DST_IP_LOWERs,
    HASH0_DST_IP_UPPERs,
    HASH0_SRC_IP_LOWERs,
    HASH0_SRC_IP_UPPERs,
    HASH0_CNTAG_RPIDs
};

static const char *ipv6_hash1_fields_str[] = {
    HASH1_FLOW_ID_LOs,
    HASH1_FLOW_ID_HIs,
    HASH1_SRC_MODIDs,
    HASH1_SRC_PORTs,
    HASH1_PROTOCOLs,
    HASH1_L4_DSTs,
    HASH1_L4_SRCs,
    HASH1_VIDs,
    HASH1_DST_IP_LOWERs,
    HASH1_DST_IP_UPPERs,
    HASH1_SRC_IP_LOWERs,
    HASH1_SRC_IP_UPPERs,
    HASH1_CNTAG_RPIDs
};

uint32_t l2_fields[] = {
    BCM_HASH_FIELD_DSTMOD,
    BCM_HASH_FIELD_DSTPORT,
    BCM_HASH_FIELD_SRCMOD,
    BCM_HASH_FIELD_SRCPORT,
    BCM_HASH_FIELD_VLAN,
    BCM_HASH_FIELD_ETHER_TYPE,
    BCM_HASH_FIELD_MACDA_LO,
    BCM_HASH_FIELD_MACDA_MI,
    BCM_HASH_FIELD_MACDA_HI,
    BCM_HASH_FIELD_MACSA_LO,
    BCM_HASH_FIELD_MACSA_MI,
    BCM_HASH_FIELD_MACSA_HI,
};

static const char *l2_hash0_fields_str[] = {
    HASH0_DST_MODIDs,
    HASH0_DST_PORTs,
    HASH0_SRC_MODIDs,
    HASH0_SRC_PORTs,
    HASH0_VIDs,
    HASH0_ETH_TYPEs,
    HASH0_MAC_DA_LOs,
    HASH0_MAC_DA_MEDs,
    HASH0_MAC_DA_HIs,
    HASH0_MAC_SA_LOs,
    HASH0_MAC_SA_MEDs,
    HASH0_MAC_SA_HIs
};

static const char *l2_hash1_fields_str[] = {
    HASH1_DST_MODIDs,
    HASH1_DST_PORTs,
    HASH1_SRC_MODIDs,
    HASH1_SRC_PORTs,
    HASH1_VIDs,
    HASH1_ETH_TYPEs,
    HASH1_MAC_DA_LOs,
    HASH1_MAC_DA_MEDs,
    HASH1_MAC_DA_HIs,
    HASH1_MAC_SA_LOs,
    HASH1_MAC_SA_MEDs,
    HASH1_MAC_SA_HIs
};

uint32_t l3_mpls_fields[] = {
    BCM_HASH_FIELD_FLOWLABEL_LO,
    BCM_HASH_FIELD_FLOWLABEL_HI,
    BCM_HASH_FIELD_SRCMOD,
    BCM_HASH_FIELD_SRCPORT,
    BCM_HASH_FIELD_NXT_HDR,
    BCM_HASH_FIELD_DSTL4,
    BCM_HASH_FIELD_SRCL4,
    BCM_HASH_FIELD_VLAN,
    BCM_HASH_MPLS_FIELD_IP4DST_LO,
    BCM_HASH_MPLS_FIELD_IP4DST_HI,
    BCM_HASH_MPLS_FIELD_IP4SRC_LO,
    BCM_HASH_MPLS_FIELD_IP4SRC_HI
};

static const char *l3_mpls_hash0_fields_str[] = {
    HASH0_FLOW_ID_LOs,
    HASH0_FLOW_ID_HIs,
    HASH0_SRC_MODIDs,
    HASH0_SRC_PORTs,
    HASH0_PROTOCOLs,
    HASH0_L4_DSTs,
    HASH0_L4_SRCs,
    HASH0_TUNNEL_VIDs,
    HASH0_DST_IP_LOWERs,
    HASH0_DST_IP_UPPERs,
    HASH0_SRC_IP_LOWERs,
    HASH0_SRC_IP_UPPERs
};

static const char *l3_mpls_hash1_fields_str[] = {
    HASH1_FLOW_ID_LOs,
    HASH1_FLOW_ID_HIs,
    HASH1_SRC_MODIDs,
    HASH1_SRC_PORTs,
    HASH1_PROTOCOLs,
    HASH1_L4_DSTs,
    HASH1_L4_SRCs,
    HASH1_TUNNEL_VIDs,
    HASH1_DST_IP_LOWERs,
    HASH1_DST_IP_UPPERs,
    HASH1_SRC_IP_LOWERs,
    HASH1_SRC_IP_UPPERs
};


uint32_t mpls_transit_fields[] = {
    BCM_HASH_FIELD_DSTMOD,
    BCM_HASH_FIELD_DSTPORT,
    BCM_HASH_FIELD_SRCMOD,
    BCM_HASH_FIELD_SRCPORT,
    BCM_HASH_MPLS_FIELD_3RD_LABEL,
    BCM_HASH_MPLS_FIELD_IP4SRC_LO,
    BCM_HASH_MPLS_FIELD_IP4SRC_HI,
    BCM_HASH_MPLS_FIELD_IP4DST_LO,
    BCM_HASH_MPLS_FIELD_IP4DST_HI,
    BCM_HASH_MPLS_FIELD_LABELS_4MSB,
    BCM_HASH_MPLS_FIELD_2ND_LABEL,
    BCM_HASH_MPLS_FIELD_TOP_LABEL
};

static const char *mpls_transit_hash0_fields_str[] = {
    HASH0_DST_MODIDs,
    HASH0_DST_PORTs,
    HASH0_SRC_MODIDs,
    HASH0_SRC_PORTs,
    HASH0_MPLS_3RD_LABELs,
    HASH0_SRC_IP_LOWERs,
    HASH0_SRC_IP_UPPERs,
    HASH0_DST_IP_LOWERs,
    HASH0_DST_IP_UPPERs,
    HASH0_MPLS_LABELS_4MSBs,
    HASH0_MPLS_2ND_LABELs,
    HASH0_MPLS_TOP_LABELs
};

static const char *mpls_transit_hash1_fields_str[] = {
    HASH1_DST_MODIDs,
    HASH1_DST_PORTs,
    HASH1_SRC_MODIDs,
    HASH1_SRC_PORTs,
    HASH1_MPLS_3RD_LABELs,
    HASH1_SRC_IP_LOWERs,
    HASH1_SRC_IP_UPPERs,
    HASH1_DST_IP_LOWERs,
    HASH1_DST_IP_UPPERs,
    HASH1_MPLS_LABELS_4MSBs,
    HASH1_MPLS_2ND_LABELs,
    HASH1_MPLS_TOP_LABELs
};

uint32_t mpls_transit_use_statck_fields[] = {
    BCM_HASH_FIELD_DSTMOD,
    BCM_HASH_FIELD_DSTPORT,
    BCM_HASH_FIELD_SRCMOD,
    BCM_HASH_FIELD_SRCPORT,
    BCM_HASH_MPLS_FIELD_3RD_LABEL_4MSB_5TH_LABEL,
    BCM_HASH_MPLS_FIELD_IP4SRC_IP6SRC_LO_4MSB_4TH_LABEL,
    BCM_HASH_MPLS_FIELD_IP4SRC_IP6SRC_HI_4MSB,
    BCM_HASH_MPLS_FIELD_IP4DST_IP6DST_LO_5TH_LABEL,
    BCM_HASH_MPLS_FIELD_IP4DST_IP6DST_HI_4TH_LABEL,
    BCM_HASH_MPLS_FIELD_LABELS_4MSB_3RD_LABEL,
    BCM_HASH_MPLS_FIELD_2ND_LABEL,
    BCM_HASH_MPLS_FIELD_TOP_LABEL
};

static const char *mpls_transit_use_statck_hash0_fields_str[] = {
    USE_MPLS_STACK_HASH0_DST_MODIDs,
    USE_MPLS_STACK_HASH0_DST_PORTs,
    USE_MPLS_STACK_HASH0_SRC_MODIDs,
    USE_MPLS_STACK_HASH0_SRC_PORTs,
    USE_MPLS_STACK_HASH0_MPLS_3RD_LABELs,
    USE_MPLS_STACK_HASH0_MPLS_4MSB_5TH_LABELs,
    USE_MPLS_STACK_HASH0_MPLS_4MSB_4TH_LABELs,
    USE_MPLS_STACK_HASH0_MPLS_5TH_LABELs,
    USE_MPLS_STACK_HASH0_MPLS_4TH_LABELs,
    USE_MPLS_STACK_HASH0_MPLS_LABELS_4MSBs,
    USE_MPLS_STACK_HASH0_MPLS_2ND_LABELs,
    USE_MPLS_STACK_HASH0_MPLS_TOP_LABELs
};

static const char *mpls_transit_use_statck_hash1_fields_str[] = {
    USE_MPLS_STACK_HASH1_DST_MODIDs,
    USE_MPLS_STACK_HASH1_DST_PORTs,
    USE_MPLS_STACK_HASH1_DST_MODIDs,
    USE_MPLS_STACK_HASH1_SRC_PORTs,
    USE_MPLS_STACK_HASH1_MPLS_3RD_LABELs,
    USE_MPLS_STACK_HASH1_MPLS_4MSB_5TH_LABELs,
    USE_MPLS_STACK_HASH1_MPLS_4MSB_4TH_LABELs,
    USE_MPLS_STACK_HASH1_MPLS_5TH_LABELs,
    USE_MPLS_STACK_HASH1_MPLS_4TH_LABELs,
    USE_MPLS_STACK_HASH1_MPLS_LABELS_4MSBs,
    USE_MPLS_STACK_HASH1_MPLS_2ND_LABELs,
    USE_MPLS_STACK_HASH1_MPLS_TOP_LABELs
};


static const char *field_selection_lt_str[bcmiFieldSelectTypeCount] = {
    LB_HASH_IPV4_FIELDS_SELECTIONs,
    LB_HASH_IPV4_TCP_UDP_FIELDS_SELECTIONs,
    LB_HASH_IPV4_TCP_UDP_PORTS_EQUAL_FIELDS_SELECTIONs,
    LB_HASH_IPV6_FIELDS_SELECTIONs,
    LB_HASH_IPV6_TCP_UDP_FIELDS_SELECTIONs,
    LB_HASH_IPV6_TCP_UDP_PORTS_EQUAL_FIELDS_SELECTIONs,
    LB_HASH_L2_FIELDS_SELECTIONs,
    LB_HASH_L3MPLS_PAYLOAD_FIELDS_SELECTIONs,
    LB_HASH_TNL_MPLS_TRANSIT_FIELDS_SELECTIONs,
    LB_HASH_IPV4_FIELDS_SELECTIONs,
    LB_HASH_IPV4_TCP_UDP_FIELDS_SELECTIONs,
    LB_HASH_IPV4_TCP_UDP_PORTS_EQUAL_FIELDS_SELECTIONs,
    LB_HASH_IPV6_FIELDS_SELECTIONs,
    LB_HASH_IPV6_TCP_UDP_FIELDS_SELECTIONs,
    LB_HASH_IPV6_TCP_UDP_PORTS_EQUAL_FIELDS_SELECTIONs,
    LB_HASH_L2_FIELDS_SELECTIONs,
    LB_HASH_L3MPLS_PAYLOAD_FIELDS_SELECTIONs,
    LB_HASH_TNL_MPLS_TRANSIT_FIELDS_SELECTIONs,
};

static const char *subset_sel_str[] = {
    USE_0_0s,
    USE_1_0s,
    USE_PORT_LBNs,
    USE_DEST_PORTs,
    USE_0_1s,
    USE_1_1s,
};

#define MAX_SUBSET 6
/******************************************************************************
 * Private functions
 */
/*!
 * \brief Get value for offset, sub_sel and concat for port based lb hash.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Input value.
 * \param [out] offset_val Offset value.
 * \param [out] sub_sel_val Sub_sel value.
 * \param [out] concat_val Concat enable.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lb_hash_port_hash_fields_caculate(
    int unit,
    int arg,
    int *offset_val,
    int *sub_sel_val,
    int *concat_val)
{
    int sub_field_width[8];
    int total_width, i;
    int offset = -1, concat = -1, sub_sel = -1;

    SHR_FUNC_ENTER(unit);

    if (arg < 0) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sub_field_width[0] = 16;  /* HASH_A0 (16bit) */
    sub_field_width[1] = 16;  /* HASH_B0 (16bit) */
    sub_field_width[2] = 6;   /* LBN (6bit)      */
    sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit)           */
    sub_field_width[4] = 16;  /* HASH_A1 (16 bit) */
    sub_field_width[5] = 16;  /* HASH_B1 (16 bit) */

    /* Get hash bits width */
    total_width = 0;
    for (i = 0; i < MAX_SUBSET; i++) {
        total_width += sub_field_width[i];
    }

    /* Concatenate if offset value exceeds total hash width */
    if (arg >= total_width) {
        concat = 1;
    }

    /* Concatenate if enforced or if offset value exceeds total hash width */
    if (concat == 1) {
        /* Concatenation hash computation order */
        sub_field_width[0] = 64;  /* Concat B1,B0,A1,A0(64bit)      */
        sub_field_width[1] = 0;   /* 0                              */
        sub_field_width[2] = 6;   /* LBN (6bit)                     */
        sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit)       */
        sub_field_width[4] = 0;   /* 0                              */
        sub_field_width[5] = 0;   /* 0                              */

        /* Get concatenated hash bits width */
        arg = arg - total_width;
        total_width = 0;
        for (i = 0; i < MAX_SUBSET; i++) {
            total_width += sub_field_width[i];
        }
    }

    /* Select hash sub select and hash bit offset */
    offset = arg % total_width;
    for (i = 0; i < MAX_SUBSET; i++) {
         offset -= sub_field_width[i];
         if (offset < 0) {
             offset += sub_field_width[i];
             sub_sel = i;
             break;
         }
    }

    if (offset > 0) {
        *offset_val = offset;
    }

    if (concat > 0) {
        *concat_val = concat;
    }

    if (sub_sel > 0) {
        *sub_sel_val = sub_sel;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Combine value from offset, sub_sel and concat for port based lb hash.
 *
 * \param [in] unit Unit number.
 * \param [in] offset_val Offset value.
 * \param [in] sub_sel_val Sub_sel value.
 * \param [in] concat_val Concat enable.
 * \param [out] arg Input value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lb_hash_port_hash_fields_combine(
    int unit,
    int offset_val,
    int sub_sel_val,
    int concat_val,
    int *arg)
{
    int idx = 0;
    int total_width = 0;
    int concat = 0;
    int sub_field_width[8];
    int i;

    SHR_FUNC_ENTER(unit);

    if (arg == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    idx = sub_sel_val;
    *arg = offset_val;
    concat = concat_val;

    sub_field_width[0] = 16;  /* HASH_A0 (16bit) */
    sub_field_width[1] = 16;  /* HASH_B0 (16bit) */
    sub_field_width[2] = 6;   /* LBN (6bit)      */
    sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit)           */
    sub_field_width[4] = 16;  /* HASH_A1 (16 bit) */
    sub_field_width[5] = 16;  /* HASH_B1 (16 bit) */

    if (concat) {
        /* Get hash bits width */
        for (i = 0; i < MAX_SUBSET; i++) {
            total_width += sub_field_width[i];
        }
        /* Concatenation hash computation order */
        sub_field_width[0] = 64;  /* Concat B1,B0,A1,A0(64bit)      */
        sub_field_width[1] = 0;   /* 0                              */
        sub_field_width[2] = 6;   /* LBN (6bit)                     */
        sub_field_width[3] = 16;  /* MH.DPORT/ HASH_A0(16bit)       */
        sub_field_width[4] = 0;   /* 0                              */
        sub_field_width[5] = 0;   /* 0                              */

        *arg += total_width;
    }

    for (i = 0; i < idx; i++) {
         *arg += sub_field_width[i];
    }

exit:
    SHR_FUNC_EXIT();

}

STATIC int
lb_hash_flow_hash_fields_get(int unit, bcmi_ltsw_lb_hash_os_t type,
                             int **sub_field_base, int *min_offset,
                             int *max_offset, int *stride_offset,
                             int *concat_enable)
{
    /* sub field width: 16, 16, 6, 16, 16, 16 */
    static int base[] = { 0, 16, 32, 38, 54, 70, 86};
    const char *lt_name = NULL;
    const char *key_name = NULL;
    int min, max, stride;
    int index, offset;
    uint32 fval = 0, sub_sel = 0;
    int concat_value = 0;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *offset_name = NULL;
    const char *sub_sel_name = NULL;
    const char *concat_name = NULL;
    int index_max;
    int rv;
    const char *data;
    int i;
    uint64_t value;
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_lb_hash_os_tbl_get(unit,  type, &tbl_info));

    if (!tbl_info->valid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt_name = tbl_info->name;
    offset_name = tbl_info->offset_name;
    sub_sel_name = tbl_info->sub_sel_name;
    concat_name = tbl_info->concat_name;
    key_name = tbl_info->key_name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name,
                                       key_name, NULL,
                                       &value));
    index_max = (int)value;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key_name, 0));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, sub_sel_name, &data));
        for (i = 0; i < MAX_SUBSET; i++) {
            if (!sal_strcmp(subset_sel_str[i], data)) {
                break;
            }
        }
        if (i >= MAX_SUBSET) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
        }
        sub_sel = i;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, offset_name, &value));
        fval = (uint32)value;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, concat_name, &value));
        concat_value = (uint32)value;
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    if (concat_value) {
        /* sub field width: 64, 0, 6, 64, 0, 0 */
        base[0] = 0;
        base[1] = 64;  /* base[1] = base[0] + width of base[0] */
        base[2] = 64;  /* base[2] = base[1] + width of base[1] */
        base[3] = 70;  /* base[3] = base[2] + width of base[2] */
        base[4] = 134; /* base[4] = base[3] + width of base[3] */
        base[5] = 134; /* base[5] = base[4] + width of base[4] */
        base[6] = 134; /* base[6] = base[5] + width of base[5] */
    } else {
        /* sub field width: 16, 16, 6, 16, 16, 16 */
        base[0] = 0;
        base[1] = 16;  /* base[1] = base[0] + width of base[0] */
        base[2] = 32;  /* base[2] = base[1] + width of base[1] */
        base[3] = 38;  /* base[3] = base[2] + width of base[2] */
        base[4] = 54;  /* base[4] = base[3] + width of base[3] */
        base[5] = 70;  /* base[5] = base[4] + width of base[4] */
        base[6] = 86;  /* base[6] = base[5] + width of base[5] */
    }

    min = base[sub_sel] + fval;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key_name, 1));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    sub_sel = 0;
    fval = 0;
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, sub_sel_name, &data));

        for (i = 0; i < MAX_SUBSET; i++) {
            if (!sal_strcmp(subset_sel_str[i], data)) {
                break;
            }
        }
        sub_sel = i;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, offset_name, &value));
        fval = (uint32)value;
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }


    max = base[sub_sel] + fval;
    stride = max - min;

    if (stride != 0) {
        for (index = 2; index <= index_max; index++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, key_name, index));

            rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL);
            sub_sel = 0;
            fval = 0;
            if (SHR_SUCCESS(rv)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(eh, sub_sel_name, &data));
                for (i = 0; i < MAX_SUBSET; i++) {
                    if (!sal_strcmp(subset_sel_str[i], data)) {
                        break;
                    }
                }
                sub_sel = i;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, offset_name, &value));
                fval = (uint32)value;
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            }

            offset = base[sub_sel] + fval;
            if (offset != max + stride) {
                break;
            }
            max = offset;
        }
    }

    if (sub_field_base != NULL) {
        *sub_field_base = base;
    }
    if (min_offset != NULL) {
        *min_offset = min;
    }
    if (max_offset != NULL) {
        *max_offset = max;
    }
    if (stride_offset != NULL) {
        *stride_offset = stride;
    }
    if (concat_enable != NULL) {
        *concat_enable = concat_value;
    }
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

STATIC int
lb_hash_flow_hash_fields_set(int unit, bcmi_ltsw_lb_hash_os_t type,
                             bcmi_ltsw_lb_hash_flow_field_t field,
                             int arg)
{
    int *sub_field_base;
    int min_offset, max_offset, stride_offset, concat_enable;
    int index, offset;
    uint32 sub_sel;
    const char *lt_name = NULL;
    const char *key_name = NULL;
    const char *offset_name = NULL;
    const char *sub_sel_name = NULL;
    const char *concat_name = NULL;
    int index_max;
    int dunit;
    uint64_t field_value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info = NULL;
    bool concat_en_changed = false;
    static int base[] = { 0, 16, 32, 38, 54, 70, 86};
    static int base_concat[] = { 0, 64, 64, 70, 134, 134, 134};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_lb_hash_os_tbl_get(unit,  type, &tbl_info));

    if (!tbl_info->valid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt_name = tbl_info->name;
    offset_name = tbl_info->offset_name;
    sub_sel_name = tbl_info->sub_sel_name;
    concat_name = tbl_info->concat_name;
    key_name = tbl_info->key_name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name,
                                       key_name, NULL,
                                       &field_value));
    index_max = (int)field_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_flow_hash_fields_get(unit, type,
                                     &sub_field_base, &min_offset,
                                     &max_offset, &stride_offset,
                                     &concat_enable));

    if (arg < -1 || arg >= sub_field_base[MAX_SUBSET]) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (field) {
        case bcmiLbHosFlowScMin:
            if (arg == min_offset) {
                break;
            } else if ((max_offset > 0) && (arg > max_offset)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            min_offset = arg == -1 ? 0 : arg;
            break;
        case bcmiLbHosFlowScMax:
            if (arg == max_offset) {
                break;
            } else if ((arg < min_offset) && (arg != -1)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            max_offset = arg == -1 ? sub_field_base[MAX_SUBSET] - 1 : arg;
            break;
        case bcmiLbHosFlowScStride:
            if (arg == stride_offset) {
                break;
            } else if (arg < 0) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            stride_offset = arg;
            break;
        case bcmiLbHosFlowScConcat:
            if (arg == concat_enable) {
                break;
            } else if (!((arg == 0) || (arg == 1))) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
            concat_en_changed = true;
            concat_enable = arg;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    if (stride_offset == 0 && min_offset != max_offset) {
        stride_offset = 1;
    }

    if ((field == bcmiLbHosFlowScStride) &&
        (stride_offset == 1) &&
        (min_offset == max_offset)) {
        max_offset += stride_offset;
    }

    if (concat_en_changed) {
        sub_field_base = concat_enable ? base_concat : base;
        if (max_offset >= sub_field_base[MAX_SUBSET]) {
            max_offset = sub_field_base[MAX_SUBSET] - 1;
        }
        if (min_offset >= sub_field_base[MAX_SUBSET]) {
            min_offset = sub_field_base[MAX_SUBSET] - 1;
        }
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    offset = min_offset;
    for (index = 0; index <= index_max; index++) {
        for (sub_sel = 0; sub_sel <= MAX_SUBSET; sub_sel++) {
            if (offset < sub_field_base[sub_sel + 1]) {
                break;
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, key_name, index));

        field_value = concat_enable;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, concat_name, field_value));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, sub_sel_name, subset_sel_str[sub_sel]));

        field_value = offset - sub_field_base[sub_sel];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, offset_name, field_value));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

        offset += stride_offset;
        if (offset > max_offset) {
            offset = min_offset;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Clear the tables in LB hash module.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
lb_hash_tables_clear(int unit)
{
    int i, num;
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info;
    const char *ltname[] = {
        LB_HASH_BINS_ASSIGNMENTs,
        LB_HASH_ALGORITHMs,
        LB_HASH_PKT_HDR_SELECTIONs,
        LB_HASH_IPV4_FIELDS_SELECTIONs,
        LB_HASH_IPV4_TCP_UDP_FIELDS_SELECTIONs,
        LB_HASH_IPV4_TCP_UDP_PORTS_EQUAL_FIELDS_SELECTIONs,
        LB_HASH_IPV6_FIELDS_SELECTIONs,
        LB_HASH_IPV6_TCP_UDP_FIELDS_SELECTIONs,
        LB_HASH_IPV6_TCP_UDP_PORTS_EQUAL_FIELDS_SELECTIONs,
        LB_HASH_L2_FIELDS_SELECTIONs,
        LB_HASH_IPV6_COLLAPSE_SELECTIONs,
        LB_HASH_FLOW_SYMMETRYs,
        LB_HASH_FLOW_ID_SELECTIONs,
        LB_HASH_CONTROLs,
        LB_HASH_FLEX_FIELDS_SELECTIONs,
        LB_HASH_PORT_L3_ECMP_OUTPUT_SELECTIONs,
        LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs
    };

    SHR_FUNC_ENTER(unit);

    if (bcmi_warmboot_get(unit)) {
        SHR_EXIT();
    }

    num = COUNTOF(ltname);

    for (i = 0; i < num; i++) {
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, ltname[i]));
    }

    for (i = 0; i < bcmiLbHosTypeCount; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_lb_hash_os_tbl_get(unit,  i, &tbl_info));
        if (!tbl_info->valid) {
            continue;
        }
        SHR_IF_ERR_CONT
            (bcmi_lt_clear(unit, tbl_info->name));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
lb_hash_random_hash_seed_get(
    int unit,
    int concat,
    int seed,
    int *offset,
    int *subsel)
{
    int random;

    /* Generate random number for the seed as described in C standards */
    random = seed * 1103515245 + 12345;
    /* random_seed / 65536) % 32768 */
    random = (random >> 16) & 0x7FFF;

    if (concat) {
        *offset = random & 0x3f;
        /* Use HASH A0_A1_B0_B1 */
        *subsel = 0;
    } else {
        *offset = random & 0xf;
        /* User any of hash A0, A1, B0, B1 */
        *subsel = (random & 0x30) >> 4;
    }

    return BCM_E_NONE;
}

/******************************************************************************
 * Public functions
 */

int
xgs_ltsw_lb_hash_deinit(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_tables_clear(unit));

exit:
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_lb_hash_init(int unit)
{
    int index;
    int count;
    int offset;
    int sub_sel;
    int dunit;
    uint64_t field_value;
    const char *lt_name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTIONs;
    const char *field_name = LB_HASH_FLOW_ECMP_OUTPUT_SELECTION_IDs;
    const char *sub_sel_name = SUBSET_SELECTs;
    const char *offset_name = OFFSETs;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    static int rtag7_field_width[] = { 16, 16, 6, 16, 16, 16};
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info;
    bcmi_ltsw_lb_hash_os_t type;

    SHR_FUNC_ENTER(unit);

    for (type = bcmiLbHosPortTypeDlbEcmp; type < bcmiLbHosTypeCount; type++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmint_ltsw_lb_hash_os_tbl_get(unit,  type, &tbl_info));
        if (tbl_info->flow_based && tbl_info->valid) {
            lt_name = tbl_info->name;
            field_name = tbl_info->key_name;
            sub_sel_name = tbl_info->sub_sel_name;
            offset_name = tbl_info->offset_name;
        } else {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, lt_name,
                                           field_name, NULL,
                                           &field_value));
        count = (int)field_value;
        dunit = bcmi_ltsw_dev_dunit(unit);

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, lt_name, &eh));

        /* Populate and enable RTAG7 Macro flow offset table */
        for (index = 0; index < count;) {
            for (sub_sel = 0; sub_sel < MAX_SUBSET && index < count; sub_sel++) {
                for (offset = 0;
                     offset < rtag7_field_width[sub_sel] && index < count;
                     offset++) {
                    if (!sal_strcmp(subset_sel_str[sub_sel], "INVALID")) {
                        index++;
                        continue;
                    }
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(eh, field_name, index));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_symbol_add(eh, sub_sel_name,
                                                      subset_sel_str[sub_sel]));
                    field_value = offset;
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_add(eh, offset_name, field_value));
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
                    index++;
                }
            }
        }
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_os_port_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int value)
{
    const char *lt_name = NULL;
    const char *offset_name = NULL;
    const char *sub_sel_name = NULL;
    const char *concat_name = NULL;
    const char *key_name = NULL;
    int offset = 0, concat = 0, sub_sel = 0;
    int dunit;
    uint64_t field_value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info;


    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_lb_hash_os_tbl_get(unit,  type, &tbl_info));

    if (!tbl_info->valid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt_name = tbl_info->name;
    offset_name = tbl_info->offset_name;
    sub_sel_name = tbl_info->sub_sel_name;
    concat_name = tbl_info->concat_name;
    key_name = tbl_info->key_name;

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_port_hash_fields_caculate(unit, value, &offset, &sub_sel, &concat));

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key_name, port));

    field_value = concat;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, concat_name, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, sub_sel_name, subset_sel_str[sub_sel]));

    field_value = offset;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, offset_name, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh,
                                  BCMLT_PRIORITY_NORMAL));
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_os_port_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int *value)
{
    const char *lt_name = NULL;
    const char *offset_name = NULL;
    const char *sub_sel_name = NULL;
    const char *concat_name = NULL;
    const char *key_name = NULL;
    int offset = 0, concat = 0, sub_sel = 0;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const char *data = NULL;
    int i;
    int rv;
    uint64_t field_value;
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_lb_hash_os_tbl_get(unit,  type, &tbl_info));

    if (!tbl_info->valid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt_name = tbl_info->name;
    offset_name = tbl_info->offset_name;
    sub_sel_name = tbl_info->sub_sel_name;
    concat_name = tbl_info->concat_name;
    key_name = tbl_info->key_name;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key_name, port));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL);
    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, concat_name, &field_value));
    concat = (int)field_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, sub_sel_name, &data));
    for (i = 0; i < 8; i++) {
        if (!sal_strcmp(subset_sel_str[i], data)) {
            break;
        }
    }
    sub_sel = i;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, offset_name, &field_value));
    offset = (int)field_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_port_hash_fields_combine(unit, offset, sub_sel,
                                          concat, value));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();

}


int
xgs_ltsw_lb_hash_os_flow_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int value)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_flow_hash_fields_set(unit, type, field, value));

exit:
    SHR_FUNC_EXIT();

}

int
xgs_ltsw_lb_hash_os_flow_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int *value)
{
    int min_offset;
    int max_offset;
    int stride_offset;
    int concat_enable;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_flow_hash_fields_get(unit, type, NULL,
                                      &min_offset,
                                      &max_offset,
                                      &stride_offset,
                                      &concat_enable));
    switch (field) {
        case bcmiLbHosFlowScMin:
            *value = min_offset;
            break;
        case bcmiLbHosFlowScMax:
            *value = max_offset;
            break;
        case bcmiLbHosFlowScStride:
            *value = stride_offset;
            break;
        case bcmiLbHosFlowScConcat:
            *value = concat_enable;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    SHR_FUNC_EXIT();

}

int
xgs_ltsw_lb_hash_fields_select_set(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int value)
{
    const char *lt_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int i;
    int size;
    const char **field_name = NULL;
    uint32_t *flags = NULL;
    int mpls_use_stack = 0;
    bool hash_fld_support = false;

    SHR_FUNC_ENTER(unit);

    lt_name = field_selection_lt_str[type];

    dunit = bcmi_ltsw_dev_dunit(unit);
    switch (type) {
        case bcmiHash0FieldSelectTypeIP:
        case bcmiHash0FieldSelectTypeTcpUdp:
        case bcmiHash0FieldSelectTypeL4Port:
            field_name = ipv4_hash0_fields_str;
            size = sizeof(ipv4_fields)/sizeof(uint32_t);
            flags = ipv4_fields;
            break;
        case bcmiHash1FieldSelectTypeIP:
        case bcmiHash1FieldSelectTypeTcpUdp:
        case bcmiHash1FieldSelectTypeL4Port:
            field_name = ipv4_hash1_fields_str;
            size = sizeof(ipv4_fields)/sizeof(uint32_t);
            flags = ipv4_fields;
            break;
        case bcmiHash0FieldSelectTypeIP6:
        case bcmiHash0FieldSelectTypeTcpUdpIp6:
        case bcmiHash0FieldSelectTypeL4PortIp6:
            field_name = ipv6_hash0_fields_str;
            size = sizeof(ipv6_fields)/sizeof(uint32_t);
            flags = ipv6_fields;
            break;
        case bcmiHash1FieldSelectTypeIP6:
        case bcmiHash1FieldSelectTypeTcpUdpIp6:
        case bcmiHash1FieldSelectTypeL4PortIp6:
            field_name = ipv6_hash1_fields_str;
            size = sizeof(ipv6_fields)/sizeof(uint32_t);
            flags = ipv6_fields;
            break;
        case bcmiHash0FieldSelectTypeL2:
            field_name = l2_hash0_fields_str;
            size = sizeof(l2_fields)/sizeof(uint32_t);
            flags = l2_fields;
            break;
        case bcmiHash1FieldSelectTypeL2:
            field_name = l2_hash1_fields_str;
            size = sizeof(l2_fields)/sizeof(uint32_t);
            flags = l2_fields;
            break;
        case bcmiHash0FieldSelectTypeL3Mpls:
            field_name = l3_mpls_hash0_fields_str;
            size = sizeof(l3_mpls_fields)/sizeof(uint32_t);
            flags = l3_mpls_fields;
            break;
        case bcmiHash1FieldSelectTypeL3Mpls:
            field_name = l3_mpls_hash1_fields_str;
            size = sizeof(l3_mpls_fields)/sizeof(uint32_t);
            flags = l3_mpls_fields;
            break;
        case bcmiHash0FieldSelectTypeMplsTransit:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                                   bcmiPktHdrSelMplsUseLabelStack,
                                                   &mpls_use_stack));
            field_name = mpls_use_stack ?
                mpls_transit_use_statck_hash0_fields_str :
                mpls_transit_hash0_fields_str;
            size = sizeof(mpls_transit_fields)/sizeof(uint32_t);
            flags = mpls_use_stack ?
                    mpls_transit_use_statck_fields : mpls_transit_fields;
            break;
        case bcmiHash1FieldSelectTypeMplsTransit:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                                   bcmiPktHdrSelMplsUseLabelStack,
                                                   &mpls_use_stack));
            field_name = mpls_use_stack ?
                mpls_transit_use_statck_hash1_fields_str :
                mpls_transit_hash1_fields_str;
            size = sizeof(mpls_transit_fields)/sizeof(uint32_t);
            flags = mpls_use_stack ?
                    mpls_transit_use_statck_fields : mpls_transit_fields;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    for (i = 0; i < size; i++) {
        if (flags[i] & value) {
            hash_fld_support = true;
        }
    }

    if (!hash_fld_support && value) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    for (i = 0; i < size; i++) {
        if (flags[i] & value) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name[i], 1));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name[i], 0));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_fields_select_get(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int *value)
{
    const char *lt_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int i;
    int size;
    const char **field_name = NULL;
    uint32_t *flags = NULL;
    int mpls_use_stack = 0;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    lt_name = field_selection_lt_str[type];

    dunit = bcmi_ltsw_dev_dunit(unit);
    switch (type) {
        case bcmiHash0FieldSelectTypeIP:
        case bcmiHash0FieldSelectTypeTcpUdp:
        case bcmiHash0FieldSelectTypeL4Port:
            field_name = ipv4_hash0_fields_str;
            size = sizeof(ipv4_fields)/sizeof(uint32_t);
            flags = ipv4_fields;
            break;
        case bcmiHash1FieldSelectTypeIP:
        case bcmiHash1FieldSelectTypeTcpUdp:
        case bcmiHash1FieldSelectTypeL4Port:
            field_name = ipv4_hash1_fields_str;
            size = sizeof(ipv4_fields)/sizeof(uint32_t);
            flags = ipv4_fields;
            break;
        case bcmiHash0FieldSelectTypeIP6:
        case bcmiHash0FieldSelectTypeTcpUdpIp6:
        case bcmiHash0FieldSelectTypeL4PortIp6:
            field_name = ipv6_hash0_fields_str;
            size = sizeof(ipv6_fields)/sizeof(uint32_t);
            flags = ipv6_fields;
            break;
        case bcmiHash1FieldSelectTypeIP6:
        case bcmiHash1FieldSelectTypeTcpUdpIp6:
        case bcmiHash1FieldSelectTypeL4PortIp6:
            field_name = ipv6_hash1_fields_str;
            size = sizeof(ipv6_fields)/sizeof(uint32_t);
            flags = ipv6_fields;
            break;
        case bcmiHash0FieldSelectTypeL2:
            field_name = l2_hash0_fields_str;
            size = sizeof(l2_fields)/sizeof(uint32_t);
            flags = l2_fields;
            break;
        case bcmiHash1FieldSelectTypeL2:
            field_name = l2_hash1_fields_str;
            size = sizeof(l2_fields)/sizeof(uint32_t);
            flags = l2_fields;
            break;
        case bcmiHash0FieldSelectTypeL3Mpls:
        case bcmiHash1FieldSelectTypeL3Mpls:
            field_name = l3_mpls_hash0_fields_str;
            size = sizeof(l3_mpls_fields)/sizeof(uint32_t);
            flags = l3_mpls_fields;
            break;
        case bcmiHash0FieldSelectTypeMplsTransit:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                                   bcmiPktHdrSelMplsUseLabelStack,
                                                   &mpls_use_stack));
            field_name = mpls_use_stack ?
                mpls_transit_use_statck_hash0_fields_str :
                mpls_transit_hash0_fields_str;
            size = sizeof(mpls_transit_fields)/sizeof(uint32_t);
            flags = mpls_use_stack ?
                    mpls_transit_use_statck_fields : mpls_transit_fields;
            break;
        case bcmiHash1FieldSelectTypeMplsTransit:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_pkt_hdr_sel_get(unit,
                                                   bcmiPktHdrSelMplsUseLabelStack,
                                                   &mpls_use_stack));
            field_name = mpls_use_stack ?
                mpls_transit_use_statck_hash1_fields_str :
                mpls_transit_hash1_fields_str;
            size = sizeof(mpls_transit_fields)/sizeof(uint32_t);
            flags = mpls_use_stack ?
                    mpls_transit_use_statck_fields : mpls_transit_fields;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    for (i = 0; i < size; i++) {
        uint64_t field_value = 0;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, field_name[i], &field_value));
        if (field_value) {
            *value |= flags[i];
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_bins_set(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_BINS_ASSIGNMENTs, &eh));

    switch (control) {
        case bcmiHashBinField0PreProcessEnable:
            field_value = value;
            field_name = HASH0_ALL_BINS_PRE_PROCESSING_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiHashBinField1PreProcessEnable:
            field_value = value;
            field_name = HASH1_ALL_BINS_PRE_PROCESSING_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiHashBinField0Ip6FlowLabel:
            field_value = value;
            field_name = HASH0_BINS0_1_IPV6_FLOW_LABEL_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiHashBinField1Ip6FlowLabel:
            field_value = value;
            field_name = HASH1_BINS0_1_IPV6_FLOW_LABEL_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiHashBinUdfHashEnable:
            field_value = (value & BCM_HASH_FIELD0_ENABLE_UDFHASH ) ? 1 : 0;
            field_name = HASH0_BIN2_UDF_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            field_name = HASH0_BIN3_UDF_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_ENABLE_UDFHASH ) ? 1 : 0;
            field_name = HASH1_BIN2_UDF_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            field_name = HASH1_BIN3_UDF_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiHashBinGtpTeidEnable0:
            field_value = value;
            field_name = HASH0_BINS5_6_GTP_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiHashBinGtpTeidEnable1:
            field_value = value;
            field_name = HASH1_BINS5_6_GTP_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_bins_get(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int *value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    int rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_BINS_ASSIGNMENTs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiHashBinField0PreProcessEnable:
            field_name = HASH0_ALL_BINS_PRE_PROCESSING_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiHashBinField1PreProcessEnable:
            field_name = HASH1_ALL_BINS_PRE_PROCESSING_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiHashBinField0Ip6FlowLabel:
            field_name = HASH0_BINS0_1_IPV6_FLOW_LABEL_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiHashBinField1Ip6FlowLabel:
            field_name = HASH1_BINS0_1_IPV6_FLOW_LABEL_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiHashBinUdfHashEnable:
            field_name = HASH0_BIN2_UDF_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_ENABLE_UDFHASH : 0;

            field_name = HASH1_BIN2_UDF_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_ENABLE_UDFHASH : 0;
            break;
        case bcmiHashBinGtpTeidEnable0:
            field_name = HASH0_BINS5_6_GTP_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiHashBinGtpTeidEnable1:
            field_name = HASH1_BINS5_6_GTP_ENs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_pkt_hdr_sel_set(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_PKT_HDR_SELECTIONs, &eh));

    switch (control) {
        case bcmiPktHdrSelControl:
            field_value = (value & BCM_HASH_FIELD0_DISABLE_IP4 ) ? 1 : 0;
            field_name = HASH0_IGNORE_IPV4s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_IP4) ? 1 : 0;
            field_name = HASH1_IGNORE_IPV4s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD0_DISABLE_IP6) ? 1 : 0;
            field_name = HASH0_IGNORE_IPV6s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_IP6) ? 1 : 0;
            field_name = HASH1_IGNORE_IPV6s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD0_DISABLE_MPLS) ? 1 : 0;
            field_name = HASH0_IGNORE_MPLSs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_MPLS) ? 1 : 0;
            field_name = HASH1_IGNORE_MPLSs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_4OVER4_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_4OVER4_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_6OVER4_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_4OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP6) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_4OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP6) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_4OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP6) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_6OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP6) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_6OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE_IP4 |
                   BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE)) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_4OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE_IP6 |
                   BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE)) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_4OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE_IP4 |
                   BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE)) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_4OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE_IP6 |
                   BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE)) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_4OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE_IP4 |
                   BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE)) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_6OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE_IP6 |
                   BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE)) ? 1 : 0;
            field_name = HASH0_IGNORE_INNER_6OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE_IP4 |
                   BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE)) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_6OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));

            field_value = (value & (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE_IP6 |
                   BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE)) ? 1 : 0;
            field_name = HASH1_IGNORE_INNER_6OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiPktHdrSelMplsUseLabelStack:
            field_value = value;
            field_name = HASH_USE_MPLS_STACKs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiPktHdrSelIp6ExtensionHeader:
            field_value = value ? 0 : 1;
            field_name = IGNORE_IP_EXTN_HDRs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        case bcmiPktHdrSelplsExcludeReservedLabel:
            field_value = value;
            field_name = IGNORE_MPLS_RESERVED_LABELSs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, field_name, field_value));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_lb_hash_pkt_hdr_sel_get(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int *value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    int rv;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_PKT_HDR_SELECTIONs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiPktHdrSelControl:
            field_name = HASH0_IGNORE_IPV4s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_IP4 : 0;

            field_name = HASH1_IGNORE_IPV4s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_IP4 : 0;

            field_name = HASH0_IGNORE_IPV6s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_IP4 : 0;

            field_name = HASH1_IGNORE_IPV6s;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_IP6 : 0;

            field_name = HASH0_IGNORE_MPLSs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_MPLS : 0;

            field_name = HASH1_IGNORE_MPLSs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_MPLS : 0;

            field_name = HASH0_IGNORE_INNER_4OVER4_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP : 0;

            field_name = HASH1_IGNORE_INNER_4OVER4_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP : 0;

            field_name = HASH0_IGNORE_INNER_6OVER4_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP : 0;

            field_name = HASH0_IGNORE_INNER_4OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP : 0;

            field_name = HASH0_IGNORE_INNER_4OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP6 : 0;

            field_name = HASH1_IGNORE_INNER_4OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP6 : 0;

            field_name = HASH0_IGNORE_INNER_6OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP6 : 0;

            field_name = HASH1_IGNORE_INNER_6OVER6_IP_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP6 : 0;

            field_name = HASH0_IGNORE_INNER_4OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE_IP4 |
                                      BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE) : 0;

            field_name = HASH0_IGNORE_INNER_4OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE_IP6 |
                                      BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE) : 0;

            field_name = HASH1_IGNORE_INNER_4OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE_IP4 |
                                      BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE) : 0;

            field_name = HASH1_IGNORE_INNER_4OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE_IP6 |
                                      BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE) : 0;

            field_name = HASH0_IGNORE_INNER_6OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE_IP4 |
                                      BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE) : 0;

            field_name = HASH0_IGNORE_INNER_6OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE_IP6 |
                                      BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE) : 0;

            field_name = HASH1_IGNORE_INNER_6OVER4_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE_IP4 |
                                      BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE) : 0;

            field_name = HASH1_IGNORE_INNER_6OVER6_GRE_TUNNELs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? (BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE_IP6 |
                                      BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE) : 0;
            break;
        case bcmiPktHdrSelMplsUseLabelStack:
            field_name = HASH_USE_MPLS_STACKs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiPktHdrSelIp6ExtensionHeader:
            field_name = IGNORE_IP_EXTN_HDRs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = field_value ? 0 : 1;
            break;
        case bcmiPktHdrSelplsExcludeReservedLabel:
            field_name = IGNORE_MPLS_RESERVED_LABELSs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_ip6_collapse_set(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int value)
{
    const char *field_name = NULL;
    const char *field_str = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (value & BCM_HASH_IP6_COLLAPSE_XOR) {
        field_str = FOLD_AND_XORs;
    } else if (value & BCM_HASH_IP6_COLLAPSE_LSB) {
        field_str = USE_LSBs;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_IPV6_COLLAPSE_SELECTIONs, &eh));

    switch (control) {
        case bcmiIp6CollapseField0:
            field_name = HASH0_SELECTIONs;
            break;
        case bcmiIp6CollapseField1:
            field_name = HASH1_SELECTIONs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, field_name, field_str));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_ip6_collapse_get(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int *value)
{
    const char *field_str = NULL;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_IPV6_COLLAPSE_SELECTIONs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiIp6CollapseField0:
            field_name = HASH0_SELECTIONs;
            break;
        case bcmiIp6CollapseField1:
            field_name = HASH1_SELECTIONs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_get(eh, field_name, &field_str));

    if (sal_strcasecmp(field_str, FOLD_AND_XORs) == 0) {
        *value = BCM_HASH_IP6_COLLAPSE_XOR;
    } else if (sal_strcasecmp(field_str, USE_LSBs) == 0) {
        *value = BCM_HASH_IP6_COLLAPSE_LSB;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_algorithm_set(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int value)
{
    const char *field_name = NULL;
    const char *field_str = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    if (value == BCM_HASH_FIELD_CONFIG_CRC16XOR8) {
        field_str = CRC16_BISYNC_AND_XOR8s;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC16XOR4) {
        field_str = CRC16_BISYNC_AND_XOR4s;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC16XOR2) {
        field_str = CRC16_BISYNC_AND_XOR2s;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC16XOR1) {
        field_str = CRC16_BISYNC_AND_XOR1s;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC16) {
        field_str = CRC16_BISYNCs;
    } else if (value == BCM_HASH_FIELD_CONFIG_XOR16) {
        field_str = XOR16s;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC16CCITT) {
        field_str = CRC16_CCITTs;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC32LO) {
        field_str = CRC32_LOs;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC32HI) {
        field_str = CRC32_HIs;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC32_ETH_LO) {
        field_str = CRC32_ETH_LOs;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC32_ETH_HI) {
        field_str = CRC32_ETH_HIs;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC32_KOOPMAN_LO) {
        field_str = CRC32_KOOPMAN_LOs;
    } else if (value == BCM_HASH_FIELD_CONFIG_CRC32_KOOPMAN_HI) {
        field_str = CRC32_KOOPMAN_HIs;
    } else if (value == BCM_HASH_FIELD_CONFIG_VERSATILE_HASH_LO) {
        field_str = VERSATILE_HASH_0s;
    } else if (value == BCM_HASH_FIELD_CONFIG_VERSATILE_HASH_HI) {
        field_str = VERSATILE_HASH_1s;
    } else if (value == 0) {
        field_str = RESERVEDs;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_ALGORITHMs, &eh));

    switch (control) {
        case bcmiAlgorithmField0Inst0:
            field_name = HASH0_INSTANCE0_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, field_name, field_str));
            break;
        case bcmiAlgorithmField0Inst1:
            field_name = HASH0_INSTANCE1_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, field_name, field_str));
            break;
        case bcmiAlgorithmField1Inst0:
            field_name = HASH1_INSTANCE0_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, field_name, field_str));
            break;
        case bcmiAlgorithmField1Inst1:
            field_name = HASH1_INSTANCE1_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, field_name, field_str));
            break;
        case bcmiAlgorithmMacroFlow:
            field_name = MACRO_FLOW_HASH_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, field_name, field_str));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_algorithm_get(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int *value)
{
    const char *field_str = NULL;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_ALGORITHMs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiAlgorithmField0Inst0:
            field_name = HASH0_INSTANCE0_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, field_name, &field_str));
            break;
        case bcmiAlgorithmField0Inst1:
            field_name = HASH0_INSTANCE1_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, field_name, &field_str));
            break;
        case bcmiAlgorithmField1Inst0:
            field_name = HASH1_INSTANCE0_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, field_name, &field_str));
            break;
        case bcmiAlgorithmField1Inst1:
            field_name = HASH1_INSTANCE1_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, field_name, &field_str));
            break;
        case bcmiAlgorithmMacroFlow:
            field_name = MACRO_FLOW_HASH_ALGs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, field_name, &field_str));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    if (sal_strcasecmp(field_str, CRC16_BISYNC_AND_XOR8s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC16XOR8;
    } else if (sal_strcasecmp(field_str, CRC16_BISYNC_AND_XOR4s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC16XOR4;
    } else if (sal_strcasecmp(field_str, CRC16_BISYNC_AND_XOR2s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC16XOR2;
    } else if (sal_strcasecmp(field_str, CRC16_BISYNC_AND_XOR1s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC16XOR1;
    } else if (sal_strcasecmp(field_str, CRC16_BISYNCs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC16;
    } else if (sal_strcasecmp(field_str, XOR16s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_XOR16;
    } else if (sal_strcasecmp(field_str, CRC16_CCITTs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC16CCITT;
    } else if (sal_strcasecmp(field_str, CRC32_LOs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC32LO;
    } else if (sal_strcasecmp(field_str, CRC32_HIs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC32HI;
    } else if (sal_strcasecmp(field_str, CRC32_ETH_LOs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC32_ETH_LO;
    } else if (sal_strcasecmp(field_str, CRC32_ETH_HIs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC32_ETH_HI;
    } else if (sal_strcasecmp(field_str, CRC32_KOOPMAN_LOs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC32_KOOPMAN_LO;
    } else if (sal_strcasecmp(field_str, CRC32_KOOPMAN_HIs) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_CRC32_KOOPMAN_HI;
    } else if (sal_strcasecmp(field_str, VERSATILE_HASH_0s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_VERSATILE_HASH_LO;
    } else if (sal_strcasecmp(field_str, VERSATILE_HASH_1s) == 0) {
        *value = BCM_HASH_FIELD_CONFIG_VERSATILE_HASH_HI;
    } else if (sal_strcasecmp(field_str, RESERVEDs) == 0) {
        *value = 0;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_symmetric_control_set(
    int unit,
    int value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_FLOW_SYMMETRYs, &eh));

    field_value = (value & BCM_SYMMETRIC_HASH_0_IP4_ENABLE) ? 1 : 0;
    field_name = HASH0_SYMMETRIC_IPV4s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    field_value = (value & BCM_SYMMETRIC_HASH_1_IP4_ENABLE) ? 1 : 0;
    field_name = HASH1_SYMMETRIC_IPV4s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    field_value = (value & BCM_SYMMETRIC_HASH_0_IP6_ENABLE) ? 1 : 0;
    field_name = HASH0_SYMMETRIC_IPV6s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    field_value = (value & BCM_SYMMETRIC_HASH_1_IP6_ENABLE) ? 1 : 0;
    field_name = HASH1_SYMMETRIC_IPV6s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    field_value = (value & BCM_SYMMETRIC_HASH_0_SUPPRESS_UNIDIR_FIELD_ENABLE) ? 1 : 0;
    field_name = HASH0_SYMMETRIC_SUPPRESS_UNIDIR_FIELDs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    field_value = (value & BCM_SYMMETRIC_HASH_1_SUPPRESS_UNIDIR_FIELD_ENABLE) ? 1 : 0;
    field_name = HASH1_SYMMETRIC_SUPPRESS_UNIDIR_FIELDs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_lb_hash_symmetric_control_get(
    int unit,
    int *value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_FLOW_SYMMETRYs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    field_name = HASH0_SYMMETRIC_IPV4s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value |= field_value ? BCM_SYMMETRIC_HASH_0_IP4_ENABLE : 0;

    field_name = HASH1_SYMMETRIC_IPV4s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value |= field_value ? BCM_SYMMETRIC_HASH_1_IP4_ENABLE : 0;

    field_name = HASH0_SYMMETRIC_IPV6s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value |= field_value ? BCM_SYMMETRIC_HASH_0_IP6_ENABLE : 0;

    field_name = HASH1_SYMMETRIC_IPV6s;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value |= field_value ? BCM_SYMMETRIC_HASH_1_IP6_ENABLE : 0;

    field_name = HASH0_SYMMETRIC_SUPPRESS_UNIDIR_FIELDs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value |= field_value ? BCM_SYMMETRIC_HASH_0_SUPPRESS_UNIDIR_FIELD_ENABLE : 0;

    field_name = HASH1_SYMMETRIC_SUPPRESS_UNIDIR_FIELDs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, field_name, &field_value));
    *value |= field_value ? BCM_SYMMETRIC_HASH_1_SUPPRESS_UNIDIR_FIELD_ENABLE : 0;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_control_set(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_CONTROLs, &eh));

    switch (control) {
        case bcmiHashControlEcmp:
            field_value = (value & BCM_HASH_CONTROL_MULTIPATH_L4PORTS) ? 1 : 0;
            field_name = USE_TCP_UDP_PORTs;
            break;
        case bcmiHashControlEntroyLabelFlowBased:
            field_value = value;
            field_name = ENTROPY_LABEL_FLOW_BASEDs;
            break;
        case bcmiHashControlFlexHashLookupStatus:
            field_value = value;
            field_name = FLEX_HASHs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_control_get(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int *value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_CONTROLs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiHashControlEcmp:
            field_name = USE_TCP_UDP_PORTs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value |= field_value ? BCM_HASH_CONTROL_MULTIPATH_L4PORTS : 0;
            break;
        case bcmiHashControlEntroyLabelFlowBased:
            field_name = ENTROPY_LABEL_FLOW_BASEDs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = field_value;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_bins_flex_set(
    int unit,
    bool enable)
{
    int dunit, idx;
    uint64_t field_value;
    uint64_t field_val_arr[LB_HASH_NUM_FLEX_FIELDS];
    bcmlt_field_def_t fdef;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_BINS_ASSIGNMENTs, &eh));

    field_value = enable ?  1: 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, HASH0_BIN2_FLEX_ENs, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, HASH1_BIN2_FLEX_ENs, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, HASH0_BIN3_FLEX_ENs, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, HASH1_BIN3_FLEX_ENs, field_value));

    for (idx = 0; idx < LB_HASH_NUM_FLEX_FIELDS; idx++) {
        field_val_arr[idx] = field_value;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_def_get(unit, LB_HASH_BINS_ASSIGNMENTs,
                               HASH0_BIN_FLEX_FIELD_SELECTs, &fdef));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, HASH0_BIN_FLEX_FIELD_SELECTs,
                                     0, field_val_arr, fdef.depth));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, HASH1_BIN_FLEX_FIELD_SELECTs,
                                     0, field_val_arr, fdef.depth));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_control_seed_set(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);
    field_value = (uint32_t)value;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_SEED_CONTROLs, &eh));

    switch (control) {
        case bcmiHashControlSeed0:
            field_name = HASH0_SEEDs;
            break;
        case bcmiHashControlSeed1:
            field_name = HASH1_SEEDs;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, field_name, field_value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}


int
xgs_ltsw_lb_hash_control_seed_get(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int *value)
{
    uint64_t field_value;
    const char *field_name = NULL;
    int dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int rv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, LB_HASH_SEED_CONTROLs, &eh));

    rv = bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL);

    if (rv == SHR_E_NOT_FOUND) {
        *value = 0;
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_NOT_FOUND);
    }

    switch (control) {
        case bcmiHashControlSeed0:
            field_name = HASH0_SEEDs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
            *value = (int)field_value;
            break;
        case bcmiHashControlSeed1:
            field_name = HASH1_SEEDs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, field_name, &field_value));
           *value = (int)field_value;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();
}

int
xgs_ltsw_lb_hash_macro_flow_seed_set(
    int unit,
    bcmi_ltsw_macro_flow_t type,
    int value)
{
    int concat_enable;
    int index;
    const char *lt_name = NULL;
    const char *key_name = NULL;
    const char *offset_name = NULL;
    const char *sub_sel_name = NULL;
    const char *concat_name = NULL;
    int index_max;
    int dunit;
    uint64_t field_value;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_lb_hash_os_tbl_t *tbl_info;
    bcmi_ltsw_lb_hash_os_t os_type;
    int offset = 0;
    int subsel = 0;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case bcmiMacroFlowEcmp:
            if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_tunnel_control_set(unit,
                                              bcmiTunnelControlHashUseFlowSelMplsEcmp,
                                              1));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_control_set(unit,
                                          bcmiL3ControlFlowSelRhEcmp,
                                          1));

            os_type = bcmiLbHosFlowTypeEcmp;
            break;
        case bcmiMacroFlowEcmpOverlay:
            if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_tunnel_control_set(unit,
                                              bcmiTunnelControlHashUseFlowSelMplsEcmpOverlay,
                                              1));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_control_set(unit,
                                          bcmiL3ControlFlowSelOverlayRhEcmp,
                                          1));
            os_type = bcmiLbHosFlowTypeOverlayECMP;
            break;
        case bcmiMacroFlowLoadBalance:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_lb_hash_control_set(unit,
                                               bcmiHashControlEcmp,
                                               bcmiHashControlEntroyLabelFlowBased));
            os_type = bcmiLbHosFlowTypeEntropy;
            break;
        case bcmiMacroFlowECMPUnderlay:
            if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_tunnel_control_set(unit,
                                              bcmiTunnelControlHashUseFlowSelMplsEcmp,
                                              1));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_l3_control_set(unit,
                                          bcmiL3ControlFlowSelRhEcmp,
                                          1));
            os_type = bcmiLbHosFlowTypeEcmp;
            break;
        case bcmiMacroFlowTrunk:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_trunk_sc_lb_control_nonunicast_set(unit, 1));
            os_type = bcmiLbHosFlowTypeTrunk;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INTERNAL);
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmint_ltsw_lb_hash_os_tbl_get(unit,  os_type, &tbl_info));

    if (!tbl_info->valid) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    lt_name = tbl_info->name;
    offset_name = tbl_info->offset_name;
    sub_sel_name = tbl_info->sub_sel_name;
    concat_name = tbl_info->concat_name;
    key_name = tbl_info->key_name;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, lt_name,
                                       key_name, NULL,
                                       &field_value));
    index_max = (int)field_value;

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, lt_name, &eh));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, key_name, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, concat_name, &field_value));
    concat_enable = (int)field_value;

    SHR_IF_ERR_VERBOSE_EXIT
        (lb_hash_random_hash_seed_get(unit, concat_enable, value, &offset, &subsel));

    for (index = 0; index <= index_max; index++) {

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, key_name, index));

        field_value = concat_enable;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, concat_name, concat_enable));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, sub_sel_name, subset_sel_str[subsel]));

        field_value = offset;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, offset_name, field_value));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }
    SHR_FUNC_EXIT();

}
