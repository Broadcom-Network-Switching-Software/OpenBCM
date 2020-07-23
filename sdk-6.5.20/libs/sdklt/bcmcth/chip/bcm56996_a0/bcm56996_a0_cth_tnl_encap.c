/*! \file bcm56996_a0_cth_tnl_encap.c
 *
 * This file defines the detach, attach routines of
 * TNL encap driver for bcm56996_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_field.h>
#include <bcmdrd/chip/bcm56996_a0_enum.h>
#include <shr/shr_debug.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>

#include <shr/shr_error.h>
#include <bcmcth/bcmcth_tnl_drv.h>
#include <bcmcth/bcmcth_imm_util.h>
#include <bcmcth/bcmcth_util.h>

/*******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_TNL

#define BCM56996_A0_MAX_LABELS          (8)
#define BCM56996_A0_NUM_IPV4_TNL        (8192)
#define BCM56996_A0_NUM_IPV6_TNL        (4096)
#define BCM56996_A0_NUM_MPLS_TNL        (32768)
#define BCM56996_A0_IPV4_TNL_START_IDX  (1)
#define BCM56996_A0_IPV6_TNL_START_IDX  (2)
#define BCM56996_A0_MPLS_TNL_START_IDX  (2)
#define BCM56996_A0_TNL_NUM_KEY_FLDS    (1)
#define BCM56996_A0_TNL_NUM_DATA_FLDS   (100)
#define BCM56996_A0_IPV4_TNL_WIDTH      (1)
#define BCM56996_A0_IPV6_TNL_WIDTH      (2)
#define BCM56996_A0_MPLS_TNL_WIDTH      (2)

#define BCM56996_A0_EXP_MODE_FIXED      (0)
#define BCM56996_A0_EXP_MODE_MAP        (1)
#define BCM56996_A0_EXP_MODE_INNER      (2)

#define BCM56996_A0_DSCP_ACTION_FIXED   (0)
#define BCM56996_A0_DSCP_ACTION_PACKET  (1)
#define BCM56996_A0_DSCP_ACTION_MAP     (2)

uint32_t bcm56996_a0_tnl_mpls_entry_fid_arr[] = {
    MPLS_ENTRY_0f,
    MPLS_ENTRY_1f,
    MPLS_ENTRY_2f,
    MPLS_ENTRY_3f,
    MPLS_ENTRY_4f,
    MPLS_ENTRY_5f,
    MPLS_ENTRY_6f,
    MPLS_ENTRY_7f,
};

uint32_t bcm56996_a0_tnl_mpls_label_fid_arr[] = {
    MPLS_LABEL_0f,
    MPLS_LABEL_1f,
    MPLS_LABEL_2f,
    MPLS_LABEL_3f,
    MPLS_LABEL_4f,
    MPLS_LABEL_5f,
    MPLS_LABEL_6f,
    MPLS_LABEL_7f,
};
uint32_t bcm56996_a0_tnl_mpls_ttl_fid_arr[] = {
    MPLS_TTL_0f,
    MPLS_TTL_1f,
    MPLS_TTL_2f,
    MPLS_TTL_3f,
    MPLS_TTL_4f,
    MPLS_TTL_5f,
    MPLS_TTL_6f,
    MPLS_TTL_7f,
};
uint32_t bcm56996_a0_tnl_mpls_exp_sel_fid_arr[] = {
    MPLS_EXP_SELECT_0f,
    MPLS_EXP_SELECT_1f,
    MPLS_EXP_SELECT_2f,
    MPLS_EXP_SELECT_3f,
    MPLS_EXP_SELECT_4f,
    MPLS_EXP_SELECT_5f,
    MPLS_EXP_SELECT_6f,
    MPLS_EXP_SELECT_7f,
};
uint32_t bcm56996_a0_tnl_mpls_exp_ptr_fid_arr[] = {
    MPLS_EXP_MAPPING_PTR_0f,
    MPLS_EXP_MAPPING_PTR_1f,
    MPLS_EXP_MAPPING_PTR_2f,
    MPLS_EXP_MAPPING_PTR_3f,
    MPLS_EXP_MAPPING_PTR_4f,
    MPLS_EXP_MAPPING_PTR_5f,
    MPLS_EXP_MAPPING_PTR_6f,
    MPLS_EXP_MAPPING_PTR_7f,
};
uint32_t bcm56996_a0_tnl_mpls_exp_fid_arr[] = {
    MPLS_EXP_0f,
    MPLS_EXP_1f,
    MPLS_EXP_2f,
    MPLS_EXP_3f,
    MPLS_EXP_4f,
    MPLS_EXP_5f,
    MPLS_EXP_6f,
    MPLS_EXP_7f,
};
uint32_t bcm56996_a0_tnl_mpls_pri_fid_arr[] = {
    NEW_PRI_0f,
    NEW_PRI_1f,
    NEW_PRI_2f,
    NEW_PRI_3f,
    NEW_PRI_4f,
    NEW_PRI_5f,
    NEW_PRI_6f,
    NEW_PRI_7f,
};
uint32_t bcm56996_a0_tnl_mpls_cfi_fid_arr[] = {
    NEW_CFI_0f,
    NEW_CFI_1f,
    NEW_CFI_2f,
    NEW_CFI_3f,
    NEW_CFI_4f,
    NEW_CFI_5f,
    NEW_CFI_6f,
    NEW_CFI_7f,
};
uint32_t bcm56996_a0_tnl_encap_mpls_push_fid_arr[] = {
    MPLS_PUSH_ACTION_0f,
    MPLS_PUSH_ACTION_1f,
    MPLS_PUSH_ACTION_2f,
    MPLS_PUSH_ACTION_3f,
    MPLS_PUSH_ACTION_4f,
    MPLS_PUSH_ACTION_5f,
    MPLS_PUSH_ACTION_6f,
    MPLS_PUSH_ACTION_7f,
};
uint32_t bcm56996_a0_tnl_encap_mpls_int_cn_ptr_fid_arr[] = {
    INT_CN_TO_EXP_MAPPING_PTR_0f,
    INT_CN_TO_EXP_MAPPING_PTR_1f,
    INT_CN_TO_EXP_MAPPING_PTR_2f,
    INT_CN_TO_EXP_MAPPING_PTR_3f,
    INT_CN_TO_EXP_MAPPING_PTR_4f,
    INT_CN_TO_EXP_MAPPING_PTR_5f,
    INT_CN_TO_EXP_MAPPING_PTR_6f,
    INT_CN_TO_EXP_MAPPING_PTR_7f,
};
uint32_t bcm56996_a0_tnl_encap_mpls_int_cn_pri_fid_arr[] = {
    INT_CN_TO_EXP_PRIORITY_0f,
    INT_CN_TO_EXP_PRIORITY_1f,
    INT_CN_TO_EXP_PRIORITY_2f,
    INT_CN_TO_EXP_PRIORITY_3f,
    INT_CN_TO_EXP_PRIORITY_4f,
    INT_CN_TO_EXP_PRIORITY_5f,
    INT_CN_TO_EXP_PRIORITY_6f,
    INT_CN_TO_EXP_PRIORITY_7f,
};
uint32_t bcm56996_a0_tnl_encap_mpls_ip_ecn_ptr_fid_arr[] = {
    IP_ECN_TO_EXP_MAPPING_PTR_0f,
    IP_ECN_TO_EXP_MAPPING_PTR_1f,
    IP_ECN_TO_EXP_MAPPING_PTR_2f,
    IP_ECN_TO_EXP_MAPPING_PTR_3f,
    IP_ECN_TO_EXP_MAPPING_PTR_4f,
    IP_ECN_TO_EXP_MAPPING_PTR_5f,
    IP_ECN_TO_EXP_MAPPING_PTR_6f,
    IP_ECN_TO_EXP_MAPPING_PTR_7f,
};
uint32_t bcm56996_a0_tnl_encap_mpls_ip_ecn_pri_fid_arr[] = {
    IP_ECN_TO_EXP_PRIORITY_0f,
    IP_ECN_TO_EXP_PRIORITY_1f,
    IP_ECN_TO_EXP_PRIORITY_2f,
    IP_ECN_TO_EXP_PRIORITY_3f,
    IP_ECN_TO_EXP_PRIORITY_4f,
    IP_ECN_TO_EXP_PRIORITY_5f,
    IP_ECN_TO_EXP_PRIORITY_6f,
    IP_ECN_TO_EXP_PRIORITY_7f,
};

/*******************************************************************************
 * Private functions
 */

/*
 * \brief Map LT field to HW field
 *
 * Get the HW field corresponding to the LT field for
 * TNL_IPV4_ENCAP LT.
 *
 * \param [in]  lt_fid  LT field.
 * \param [out] hw_fid  HW table field
 */
static int
bcm56996_a0_tnl_ipv4_lt_fid_to_hw_fid(int unit,
                                      uint32_t lt_fid,
                                      uint32_t dscp_action,
                                      uint32_t *hw_fid)
{
    SHR_FUNC_ENTER(unit);

    switch (lt_fid) {
    case TNL_IPV4_ENCAPt_DST_MACf:
        *hw_fid = DEST_ADDRf;
        break;
    case TNL_IPV4_ENCAPt_SRC_IPV4f:
        *hw_fid = SIPf;
        break;
    case TNL_IPV4_ENCAPt_DST_IPV4f:
        *hw_fid = DIPf;
        break;
    case TNL_IPV4_ENCAPt_TNL_TYPEf:
        *hw_fid = TUNNEL_TYPEf;
        break;
    case TNL_IPV4_ENCAPt_TNL_TTLf:
        *hw_fid = TTLf;
        break;
    case TNL_IPV4_ENCAPt_IPV4_IN_IPV4_DF_MODEf:
        *hw_fid = IPV4_DF_SELf;
        break;
    case TNL_IPV4_ENCAPt_IPV6_IN_IPV4_DF_MODEf:
        *hw_fid = IPV6_DF_SELf;
        break;
    case TNL_IPV4_ENCAPt_PHB_EGR_DSCP_ACTIONf:
        *hw_fid = DSCP_SELf;
        break;
    case TNL_IPV4_ENCAPt_PHB_EGR_IP_INT_PRI_TO_DSCP_IDf:
        if (dscp_action != BCM56996_A0_DSCP_ACTION_MAP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = DSCP_MAPPING_PTRf;
        break;
    case TNL_IPV4_ENCAPt_DSCPf:
        if (dscp_action != BCM56996_A0_DSCP_ACTION_FIXED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = DSCPf;
        break;
    case TNL_IPV4_ENCAPt_ECN_TNL_ENCAP_IDf:
        *hw_fid = ECN_ENCAP_MAPPING_PTRf;
        break;
    case TNL_IPV4_ENCAPt_CTR_EGR_EFLEX_ACTIONf:
        *hw_fid = FLEX_CTR_ACTIONf;
        break;
    case TNL_IPV4_ENCAPt_SRC_L4_PORTf:
        *hw_fid = L4_SRC_PORTf;
        break;
    case TNL_IPV4_ENCAPt_DST_L4_PORTf:
        *hw_fid = L4_DEST_PORTf;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Map LT field to HW field
 *
 * Get the HW field corresponding to the LT field for
 * TNL_IPV6_ENCAP LT.
 *
 * \param [in]  lt_fid  LT field.
 * \param [out] partial Indicates if LT fid maps to a part og HW field
 * \param [out] hw_fid  HW field
 * \param [out] s_bit   Start bit of HW field if partial==true.
 * \param [out] e_bit   End bit of HW field if partial==true.
 */
static int
bcm56996_a0_tnl_ipv6_lt_fid_to_hw_fid(int unit,
                                      uint32_t lt_fid,
                                      uint32_t dscp_action,
                                      bool *partial,
                                      uint32_t *hw_fid,
                                      uint32_t *s_bit,
                                      uint32_t *e_bit)
{
    SHR_FUNC_ENTER(unit);

    *partial = false;
    switch (lt_fid) {
    case TNL_IPV6_ENCAPt_DST_MACf:
        *hw_fid = DEST_ADDRf;
        break;
    case TNL_IPV6_ENCAPt_SRC_IPV6u_LOWERf:
        *partial = true;
        *s_bit = 59;
        *e_bit = 122;
        break;
    case TNL_IPV6_ENCAPt_SRC_IPV6u_UPPERf:
        *partial = true;
        *s_bit = 123;
        *e_bit = 186;
        break;
    case TNL_IPV6_ENCAPt_DST_IPV6u_LOWERf:
        *partial = true;
        *s_bit = 253;
        *e_bit = 316;
        break;
    case TNL_IPV6_ENCAPt_DST_IPV6u_UPPERf:
        *partial = true;
        *s_bit = 317;
        *e_bit = 380;
        break;
    case TNL_IPV6_ENCAPt_TNL_TYPEf:
        *hw_fid = TUNNEL_TYPEf;
        break;
    case TNL_IPV6_ENCAPt_HOP_LIMITf:
        *hw_fid = TTLf;
        break;
    case TNL_IPV6_ENCAPt_FLOW_LABEL_SELECTf:
        *hw_fid = FLOW_LABEL_SELf;
        break;
    case TNL_IPV6_ENCAPt_FLOW_LABELf:
        *hw_fid = FLOW_LABELf;
        break;
    case TNL_IPV6_ENCAPt_PHB_EGR_DSCP_ACTIONf:
        *hw_fid = DSCP_SELf;
        break;
    case TNL_IPV6_ENCAPt_PHB_EGR_IP_INT_PRI_TO_DSCP_IDf:
        if (dscp_action != BCM56996_A0_DSCP_ACTION_MAP) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = DSCP_MAPPING_PTRf;
        break;
    case TNL_IPV6_ENCAPt_DSCPf:
        if (dscp_action != BCM56996_A0_DSCP_ACTION_FIXED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = DSCPf;
        break;
    case TNL_IPV6_ENCAPt_ECN_TNL_ENCAP_IDf:
        *hw_fid = ECN_ENCAP_MAPPING_PTRf;
        break;
    case TNL_IPV6_ENCAPt_CTR_EGR_EFLEX_ACTIONf:
        *hw_fid = FLEX_CTR_ACTIONf;
        break;
    case TNL_IPV6_ENCAPt_SRC_L4_PORTf:
        *hw_fid = L4_SRC_PORTf;
        break;
    case TNL_IPV6_ENCAPt_DST_L4_PORTf:
        *hw_fid = L4_DEST_PORTf;
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Map LT field to HW field
 *
 * Get the HW field corresponding to the LT field for
 * TNL_MPLS_ENCAP LT.
 *
 * \param [in]  lt_fid  LT field.
 * \param [in]  sub_id  Offset of the label within the MLPS base entry.
 * \param [in]  lbl_idx Index of the label in the label array.
 * \param [out] hw_fid  HW table field.
 */
static int
bcm56996_a0_tnl_mpls_lt_fid_to_hw_fid(int unit,
                                      uint32_t lt_fid,
                                      uint32_t sub_id,
                                      uint32_t lbl_idx,
                                      tnl_mpls_encap_entry_t *entry,
                                      uint32_t *hw_fid,
                                      uint64_t *data)
{
    uint32_t arr_idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Get the array index from offset and label index. */
    arr_idx = (sub_id + lbl_idx);

    switch (lt_fid) {
    case TNL_MPLS_ENCAPt_LABELf:
        *hw_fid = bcm56996_a0_tnl_mpls_label_fid_arr[arr_idx];
        *data = entry->label[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_LABEL_TTLf:
        *hw_fid = bcm56996_a0_tnl_mpls_ttl_fid_arr[arr_idx];
        *data = entry->ttl[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_MPLS_TNL_EXP_MODEf:
        *hw_fid = bcm56996_a0_tnl_mpls_exp_sel_fid_arr[arr_idx];
        *data = entry->exp_mode[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_PHB_EGR_MPLS_IDf:
        if (entry->exp_mode[lbl_idx] == BCM56996_A0_EXP_MODE_FIXED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = bcm56996_a0_tnl_mpls_exp_ptr_fid_arr[arr_idx];
        *data = entry->phb_id[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_LABEL_EXPf:
        *hw_fid = bcm56996_a0_tnl_mpls_exp_fid_arr[arr_idx];
        *data = entry->exp[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_PRIf:
        if (entry->exp_mode[lbl_idx] != BCM56996_A0_EXP_MODE_FIXED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = bcm56996_a0_tnl_mpls_pri_fid_arr[arr_idx];
        *data = entry->pri[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_CFIf:
        if (entry->exp_mode[lbl_idx] != BCM56996_A0_EXP_MODE_FIXED) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        *hw_fid = bcm56996_a0_tnl_mpls_cfi_fid_arr[arr_idx];
        *data = entry->cfi[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_ECN_CNG_TO_MPLS_EXP_IDf:
        *hw_fid = bcm56996_a0_tnl_encap_mpls_int_cn_ptr_fid_arr[arr_idx];
        *data = entry->cng_to_exp[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_ECN_CNG_TO_MPLS_EXP_PRIORITYf:
        *hw_fid = bcm56996_a0_tnl_encap_mpls_int_cn_pri_fid_arr[arr_idx];
        *data = entry->cng_to_exp_pri[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_ECN_IP_TO_MPLS_EXP_IDf:
        *hw_fid = bcm56996_a0_tnl_encap_mpls_ip_ecn_ptr_fid_arr[arr_idx];
        *data = entry->ecn_to_exp[lbl_idx];
        break;
    case TNL_MPLS_ENCAPt_ECN_IP_TO_MPLS_EXP_PRIORITYf:
        *hw_fid = bcm56996_a0_tnl_encap_mpls_ip_ecn_pri_fid_arr[arr_idx];
        *data = entry->ecn_to_exp_pri[lbl_idx];
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Prepare HW entry from LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_IPV4_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_ipv4_encap_hw_entry_prepare(int unit,
                                            uint32_t hw_sid,
                                            const bcmltd_field_t *in_data,
                                            uint8_t *entry_buf)
{
    int      rv = SHR_E_NONE;
    uint32_t fid = 0, hw_fid = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    const bcmltd_field_t *field;
    uint64_t dscp_action = BCM56996_A0_DSCP_ACTION_FIXED;

    SHR_FUNC_ENTER(unit);

    fid = TNL_IPV4_ENCAPt_PHB_EGR_DSCP_ACTIONf;
    bcmcth_imm_field_get(unit, in_data, fid, &dscp_action);

    /* Scan input LT field list. */
    field = in_data;
    while (field) {
        fid = field->id;
        data = field->data;

        /* Convert LT field into corresponding HW field. */
        rv = bcm56996_a0_tnl_ipv4_lt_fid_to_hw_fid(unit, fid, dscp_action, &hw_fid);
        if (rv != SHR_E_NONE) {
            /* Skip SW only fields. */
            field = field->next;
            continue;
        } else {
            BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
            bcmdrd_pt_field_set(unit,
                                hw_sid,
                                (uint32_t *)entry_buf,
                                hw_fid,
                                fbuf);
        }
        field = field->next;
    }

    /* Set entry type to IPV4. */
    data = TNL_TYPE_IPV4;

    BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit,
                        hw_sid,
                        (uint32_t *)entry_buf,
                        ENTRY_TYPEf,
                        fbuf);

    SHR_FUNC_EXIT();
}

/*
 * \brief Prepare HW entry from LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_IPV6_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_ipv6_encap_hw_entry_prepare(int unit,
                                            uint32_t hw_sid,
                                            const bcmltd_field_t *in_data,
                                            uint8_t *entry_buf)
{
    int      rv = SHR_E_NONE;
    bool     partial = false;
    uint32_t fid = 0, hw_fid = 0;
    uint32_t s_bit = 0, e_bit = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    const bcmltd_field_t *field;
    uint64_t dscp_action = BCM56996_A0_DSCP_ACTION_FIXED;

    SHR_FUNC_ENTER(unit);

    fid = TNL_IPV6_ENCAPt_PHB_EGR_DSCP_ACTIONf;
    bcmcth_imm_field_get(unit, in_data, fid, &dscp_action);

    /* Scan input LT field list. */
    field = in_data;
    while (field) {
        fid = field->id;
        data = field->data;

        /* Convert LT field into corresponding HW field. */
        rv = bcm56996_a0_tnl_ipv6_lt_fid_to_hw_fid(unit,
                                                   fid,
                                                   dscp_action,
                                                   &partial,
                                                   &hw_fid,
                                                   &s_bit,
                                                   &e_bit);
        if (rv != SHR_E_NONE) {
            /* Skip SW only fields. */
            field = field->next;
            continue;
        } else {
            BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
            if (partial == true) {
                bcmdrd_field_set((uint32_t *)entry_buf,
                                 s_bit,
                                 e_bit,
                                 fbuf);
            } else {
                bcmdrd_pt_field_set(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    fbuf);
            }
        }
        field = field->next;
    }

    /* Set entry type to IPv6. */
    data = TNL_TYPE_IPV6;
    BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit,
                        hw_sid,
                        (uint32_t *)entry_buf,
                        ENTRY_TYPEf,
                        fbuf);

    SHR_FUNC_EXIT();
}

/*
 * \brief Prepare HW entry from LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_MPLS_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] sub_id       Offset within the MPLS base entry.
 * \param [in] entry        MPLS tunnel sw entry.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_mpls_encap_hw_entry_prepare(int unit,
                                            uint32_t hw_sid,
                                            uint32_t sub_id,
                                            tnl_mpls_encap_entry_t *entry,
                                            uint8_t *entry_buf)
{
    int      rv = SHR_E_NONE;
    size_t   num_fid = 0, count = 0;
    uint32_t fid = 0, hw_fid = 0, arr_idx = 0;
    uint64_t data = 0;
    uint32_t fbuf[2] = {0};
    uint32_t i = 0, lbl_idx = 0, table_sz = 0, num_labels = 0;
    bcmlrd_fid_t   *fid_list = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_count_get(unit, TNL_MPLS_ENCAPt, &num_fid));

    table_sz = (num_fid * sizeof(bcmlrd_fid_t));
    SHR_ALLOC(fid_list, table_sz, "bcmcthTnlMplsEncapFldList");
    SHR_NULL_CHECK(fid_list, SHR_E_MEMORY);
    sal_memset(fid_list, 0, table_sz);

    SHR_IF_ERR_EXIT
        (bcmlrd_field_list_get(unit,
                               TNL_MPLS_ENCAPt,
                               num_fid,
                               fid_list,
                               &count));

    for (i = 0; i < count; i++) {
        fid = fid_list[i];
        for (lbl_idx = 0; lbl_idx < entry->max_labels; lbl_idx++) {
            /* Convert LT field into corresponding HW field. */
            rv = bcm56996_a0_tnl_mpls_lt_fid_to_hw_fid(unit,
                                                       fid,
                                                       sub_id,
                                                       lbl_idx,
                                                       entry,
                                                       &hw_fid,
                                                       &data);

            if (rv != SHR_E_NONE) {
                /* Skip SW only fields. */
                continue;
            } else {
                BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
                bcmdrd_pt_field_set(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    fbuf);
            }
        }
    }

    /* Program push action for each label in the entry. */
    lbl_idx = 0;
    num_labels = entry->num_labels;
    do {
        arr_idx = (sub_id + lbl_idx);
        hw_fid = bcm56996_a0_tnl_encap_mpls_push_fid_arr[arr_idx];

        if (num_labels == 0) {
            /* Do not push any labels. */
            data = BCMCTH_TNL_MPLS_PUSH_NONE;
        } else {
            /* Push this label. */
            data = BCMCTH_TNL_MPLS_PUSH_ONE;
        }

        /*
         * If this label is not the last label of this mpls entry,
         * mark action as PUSH_NEXT, push this label and next label.
         */
        if ((lbl_idx + 1) < num_labels) {
            data = BCMCTH_TNL_MPLS_PUSH_TWO;
        }

        BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            hw_fid,
                            fbuf);
        lbl_idx++;
    } while (lbl_idx < num_labels);

    /* Set entry type to MPLS. */
    data = TNL_TYPE_MPLS;
    BCMCTH_TNL_ENCAP_UINT64_TO_UINT32(data, fbuf);
    bcmdrd_pt_field_set(unit,
                        hw_sid,
                        (uint32_t *)entry_buf,
                        ENTRY_TYPEf,
                        fbuf);
exit:
    SHR_FREE(fid_list);
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear MPLS HW entry.
 *
 * Clear the HW entry corresponding to the TNL_MPLS_ENCAP LT
 * entry that is being deleted.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] sub_id       Offset within the MPLS base entry.
 * \param [in] entry        MPLS tunnel sw entry.
 * \param [in] entry_remove Flag indicating if entire MPLS base
 *                          tunnel entry has to be cleared.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_mpls_encap_hw_entry_clear(int unit,
                                          uint32_t hw_sid,
                                          uint32_t sub_id,
                                          int max_labels,
                                          bool entry_remove,
                                          uint8_t *entry_buf)
{
    int      lbl_idx = 0;
    uint32_t hw_fid = 0, arr_idx = 0;
    uint32_t data[2] = {0};

    SHR_FUNC_ENTER(unit);

    for (lbl_idx = 0; lbl_idx < max_labels; lbl_idx++) {
        arr_idx = (sub_id + lbl_idx);
        hw_fid = bcm56996_a0_tnl_mpls_entry_fid_arr[arr_idx];
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            hw_fid,
                            data);

        hw_fid = bcm56996_a0_tnl_encap_mpls_push_fid_arr[arr_idx];
        /* Clear push action of the tunnel. */
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            hw_fid,
                            data);
    }

    /*
     * Last MPLS entry being removed from this tunnel entry,
     * Clear tunnel entry type.
     */
    if (entry_remove == true) {
        bcmdrd_pt_field_set(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            ENTRY_TYPEf,
                            data);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Parse HW entry to LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_IPV4_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_ipv4_encap_hw_entry_parse(int unit,
                                          uint32_t hw_sid,
                                          uint8_t *entry_buf,
                                          bcmltd_fields_t *data)
{
    int      rv = SHR_E_NONE;
    uint32_t fid = 0, hw_fid = 0, i;
    uint32_t fbuf[2] = {0};
    uint64_t tmp_data = 0;
    const bcmltd_fields_t *fields = data;
    uint64_t dscp_action = BCM56996_A0_DSCP_ACTION_FIXED;

    SHR_FUNC_ENTER(unit);

    fid = TNL_IPV4_ENCAPt_PHB_EGR_DSCP_ACTIONf;
    rv = bcmcth_imm_fields_get(unit, fields, fid, &dscp_action);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    for (i = 0; i < data->count; i++) {
        fid = data->field[i]->id;

        /* Convert LT field into corresponding HW field. */
        rv = bcm56996_a0_tnl_ipv4_lt_fid_to_hw_fid(unit, fid, dscp_action, &hw_fid);
        if (rv != SHR_E_NONE) {
            /* Skip SW only fields. */
            continue;
        } else {
            fbuf[0] = 0;
            fbuf[1] = 0;
            bcmdrd_pt_field_get(unit,
                                hw_sid,
                                (uint32_t *)entry_buf,
                                hw_fid,
                                fbuf);
            BCMCTH_TNL_ENCAP_UINT32_TO_UINT64(fbuf, tmp_data);
            data->field[i]->data = tmp_data;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Parse HW entry to LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_IPV6_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_ipv6_encap_hw_entry_parse(int unit,
                                          uint32_t hw_sid,
                                          uint8_t *entry_buf,
                                          bcmltd_fields_t *data)
{
    int      rv = SHR_E_NONE;
    uint32_t fid = 0, hw_fid = 0, i;
    uint32_t fbuf[2] = {0};
    uint64_t tmp_data = 0;
    bool     partial = false;
    uint32_t s_bit = 0, e_bit = 0;
    const bcmltd_fields_t *fields = data;
    uint64_t dscp_action = BCM56996_A0_DSCP_ACTION_FIXED;

    SHR_FUNC_ENTER(unit);

    fid = TNL_IPV6_ENCAPt_PHB_EGR_DSCP_ACTIONf;
    rv = bcmcth_imm_fields_get(unit, fields, fid, &dscp_action);
    if (rv != SHR_E_NONE) {
        SHR_ERR_EXIT(rv);
    }

    for (i = 0; i < data->count; i++) {
        fid = data->field[i]->id;
        /* Convert LT field into corresponding HW field. */
        rv = bcm56996_a0_tnl_ipv6_lt_fid_to_hw_fid(unit,
                                                   fid,
                                                   dscp_action,
                                                   &partial,
                                                   &hw_fid,
                                                   &s_bit,
                                                   &e_bit);
        if (rv != SHR_E_NONE) {
            /* Skip SW only fields. */
            continue;
        } else {
            fbuf[0] = 0;
            fbuf[1] = 0;
            if (partial == true) {
                bcmdrd_field_get((uint32_t *)entry_buf,
                                 s_bit,
                                 e_bit,
                                 fbuf);
            } else {
                bcmdrd_pt_field_get(unit,
                                    hw_sid,
                                    (uint32_t *)entry_buf,
                                    hw_fid,
                                    fbuf);
            }
            BCMCTH_TNL_ENCAP_UINT32_TO_UINT64(fbuf, tmp_data);
            data->field[i]->data = tmp_data;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Parse HW entry to LT fields.
 *
 * Construct the HW table entry from the input LT fields for
 * TNL_MPLS_ENCAP LT.
 *
 * \param [in] unit         Unit number.
 * \param [in] hw_sid       HW table id.
 * \param [in] in           Input field list.
 * \param [out] entry_buf   HW entry data.
 */
static int
bcm56996_a0_tnl_mpls_encap_hw_entry_parse(int unit,
                                          uint32_t hw_sid,
                                          uint32_t sub_id,
                                          tnl_mpls_encap_entry_t *entry,
                                          uint8_t *entry_buf,
                                          bcmltd_fields_t *data)
{
    int      rv = SHR_E_NONE;
    uint32_t fid = 0, hw_fid = 0, arr_idx = 0;
    uint32_t i = 0, lbl_idx = 0;
    uint32_t fbuf[2] = {0};
    uint64_t tmp_data = 0;

    SHR_FUNC_ENTER(unit);

    lbl_idx = 0;
    do {
        arr_idx = (sub_id + lbl_idx);
        hw_fid = bcm56996_a0_tnl_encap_mpls_push_fid_arr[arr_idx];

        bcmdrd_pt_field_get(unit,
                            hw_sid,
                            (uint32_t *)entry_buf,
                            hw_fid,
                            fbuf);
        if (fbuf[0] == BCMCTH_TNL_MPLS_PUSH_NONE) {
            break;
        } else {
            /*BCMCTH_TNL_MPLS_PUSH_ONE or BCMCTH_TNL_MPLS_PUSH_TWO case*/
            lbl_idx++;
        }
    } while (fbuf[0] == BCMCTH_TNL_MPLS_PUSH_TWO);

    if (lbl_idx > BCM56996_A0_MAX_LABELS) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }
    if (lbl_idx != entry->num_labels) {
        entry->num_labels = lbl_idx;
        SHR_IF_ERR_EXIT(
            bcmcth_imm_field_update(unit,
                                    data,
                                    TNL_MPLS_ENCAPt_NUM_LABELSf,
                                    entry->num_labels));
        if (entry->num_labels > entry->max_labels) {
            entry->max_labels = entry->num_labels;
            SHR_IF_ERR_EXIT(
                bcmcth_imm_field_update(unit,
                                        data,
                                        TNL_MPLS_ENCAPt_MAX_LABELSf,
                                        entry->max_labels));
        }
    }

    for (i = 0; i < data->count; i++) {
        fid = data->field[i]->id;
        lbl_idx = data->field[i]->idx;

        if ((sub_id + lbl_idx) >= entry->max_labels) {
            continue;
        }
        /* Convert LT field into corresponding HW field. */
        rv = bcm56996_a0_tnl_mpls_lt_fid_to_hw_fid(unit,
                                                   fid,
                                                   sub_id,
                                                   lbl_idx,
                                                   entry,
                                                   &hw_fid,
                                                   &tmp_data);

        if (rv != SHR_E_NONE) {
            /* Skip SW only fields. */
            continue;
        } else {
            fbuf[0] = 0;
            fbuf[1] = 0;
            bcmdrd_pt_field_get(unit,
                                hw_sid,
                                (uint32_t *)entry_buf,
                                hw_fid,
                                fbuf);
            BCMCTH_TNL_ENCAP_UINT32_TO_UINT64(fbuf, tmp_data);
            data->field[i]->data = tmp_data;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/* Driver structure. */
bcmcth_tnl_encap_drv_t bcm56996_a0_tnl_encap_drv = {
    .num_ipv4_tnl = BCM56996_A0_NUM_IPV4_TNL,
    .num_ipv6_tnl = BCM56996_A0_NUM_IPV6_TNL,
    .num_mpls_tnl = BCM56996_A0_NUM_MPLS_TNL,
    .max_labels = BCM56996_A0_MAX_LABELS,
    .ipv4_tnl_width = BCM56996_A0_IPV4_TNL_WIDTH,
    .ipv6_tnl_width = BCM56996_A0_IPV6_TNL_WIDTH,
    .mpls_tnl_width = BCM56996_A0_MPLS_TNL_WIDTH,
    .ipv4_start_idx = BCM56996_A0_IPV4_TNL_START_IDX,
    .ipv6_start_idx = BCM56996_A0_IPV6_TNL_START_IDX,
    .mpls_start_idx = BCM56996_A0_MPLS_TNL_START_IDX,
    .num_key_flds = BCM56996_A0_TNL_NUM_KEY_FLDS,
    .num_data_flds = BCM56996_A0_TNL_NUM_DATA_FLDS,
    .ipv4_hw_sid = EGR_IP_TUNNELm,
    .ipv6_hw_sid = EGR_IP_TUNNEL_IPV6m,
    .mpls_hw_sid = EGR_IP_TUNNEL_MPLSm,
    .ipv4_lt_to_hw_map = bcm56996_a0_tnl_ipv4_encap_hw_entry_prepare,
    .ipv6_lt_to_hw_map = bcm56996_a0_tnl_ipv6_encap_hw_entry_prepare,
    .mpls_lt_to_hw_map = bcm56996_a0_tnl_mpls_encap_hw_entry_prepare,
    .mpls_hw_entry_clear = bcm56996_a0_tnl_mpls_encap_hw_entry_clear,
    .ipv4_hw_to_lt_map = bcm56996_a0_tnl_ipv4_encap_hw_entry_parse,
    .ipv6_hw_to_lt_map = bcm56996_a0_tnl_ipv6_encap_hw_entry_parse,
    .mpls_hw_to_lt_map = bcm56996_a0_tnl_mpls_encap_hw_entry_parse,
};

bcmcth_tnl_encap_drv_t *
bcm56996_a0_cth_tnl_encap_drv_get(int unit)
{
    return &bcm56996_a0_tnl_encap_drv;
}
