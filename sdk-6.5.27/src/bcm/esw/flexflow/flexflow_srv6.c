/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        flexflow_srv6.c
 * Purpose:     Flex flow Tunnel SRv6 related APIs.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#ifdef INCLUDE_L3

#include <assert.h>
#include <bcm/error.h>
#include <soc/error.h>
#include <bcm/types.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/flow.h>
#include <soc/esw/flow_db.h>
#include <bcm/flow.h>
#include <bcm_int/esw/vxlan.h>
#include <bcm_int/esw/l2gre.h>
#include <bcm_int/esw/mpls.h>
#include <soc/esw/cancun.h>

/* local defines */
#define TNL_SRV6_SID_MAX     3
#define IP6_U32_ARR_LEN      4
#define SRV6_ACTION_DELETE   (1 << 31)
#define SRV6_ACTION_REPLACE  (1 << 0)
#define SRV6_ACTION_WITH_ID  (1 << 1)

typedef struct srh_ctrl_map_s {
        char *name;   /* cancun ceh defined edit_ctrl_id names */
        int  flag;    /* SRv6 API flags */
        int  num_seg; /* number of SIDs */
} srh_ctrl_map_t;

/* local variables */

/* derive edit_ctrl_id for vc_and_swap table */
static srh_ctrl_map_t vc_ctrl_map[] = {
    {"B6_INSERT_FLOW_WITH_ONE_SRH_SEGMENT",
     BCM_FLOW_TUNNEL_SRV6_SRH_INSERT, 1},

    {"B6_INSERT_FLOW_WITH_ONE_SRH_SEGMENT_AND_REDUCED_MODE",
     BCM_FLOW_TUNNEL_SRV6_SRH_INSERT |
     BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE, 1},

    {"B6_INSERT_FLOW_WITH_THREE_SRH_SEGMENT",
     BCM_FLOW_TUNNEL_SRV6_SRH_INSERT, 3},

    {"B6_INSERT_FLOW_WITH_TWO_SRH_SEGMENT",
     BCM_FLOW_TUNNEL_SRV6_SRH_INSERT, 2},

    {"B6_INSERT_FLOW_WITH_TWO_SRH_SEGMENT_AND_REDUCED_MODE",
     BCM_FLOW_TUNNEL_SRV6_SRH_INSERT |
     BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE, 2},

    {"ONE_SEGMENT_WITHOUT_SRH", 0, 0},

    {"ONE_SEGMENT_WITH_SRH", 0, 1},

    {"ONE_SEGMENT_WITH_SRH_AND_REDUCED_MODE",
      BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE, 1},

    {"TWO_SEGMENT_WITH_SRH", 0, 2},

    {"TWO_SEGMENT_WITH_SRH_AND_REDUCED_MODE",
      BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE, 2},

    {"THREE_SEGMENT_WITH_SRH", 0, 3}
};

/* derive edit_ctrl_id for egr_ip_tunnel_ipv6 table */
static srh_ctrl_map_t tnl_ctrl_map[] = {
    {"SRV6_GSID_PREFIX1", BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID, 0},
    {"SRV6_GSID_PREFIX2", BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID |
                          BCM_FLOW_TUNNEL_SRV6_SRH_GSID_PREFIX2, 0},
    {"SRV6_STANDARD_SID_AND_USID", BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID, 0}
};

/* local function prototypes */

STATIC int _bcm_esw_flow_srh_edit_ctrl_id_get(int unit,
    int flags,
    int num_segments,
    int *vc_edit_ctrl_id,
    int *tnl_edit_ctrl_id);
STATIC int _tnl_srv6_nh_hash_calc(int unit, void *buf, uint16 *hash);

/*
 * Function:
 *      _bcm_flow_dvp_vc_swap_entry_index_alloc
 * Purpose:
 *      Allocated a joined dvp and vc_and_swap entry. Egress next_hop can use
 *  next_ptr to point to the both entries in TD3.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_X_XXX
 * NOTE:
 */

int
_bcm_flow_dvp_vc_swap_entry_index_alloc(int unit,
      egr_mpls_vc_and_swap_label_table_entry_t *vc_entry,
      egr_dvp_attribute_entry_t *dvp_entry,
      int cmp,
      int *index)
{
    int rv;
#if defined(BCM_MPLS_SUPPORT)
    rv = bcmi_td3_dvp_vc_swap_entry_index_alloc(unit, vc_entry,
                      dvp_entry, cmp, index);
#else
    rv = BCM_E_UNAVAIL;
#endif
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

int
_bcm_flow_dvp_vc_swap_entry_index_free(int unit,
      egr_dvp_attribute_entry_t *dvp_entry,
      int cmp,
      int index)
{
    int rv;
#if defined(BCM_MPLS_SUPPORT)
    rv = bcmi_td3_dvp_vc_swap_entry_index_free(unit, dvp_entry,
               cmp, index);
#else
    rv = BCM_E_UNAVAIL;
#endif
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_srh_sid_type_get
 * Purpose:
 *      Return API's SID type flags based on EGR_IP_TUNNEL_IPV6m's EDIT_CTRL_ID
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      type_value - (IN) hardware entry's edit_ctrl_id value.
 *      flags - (OUT) API's SID type flags.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */
STATIC int
_bcm_esw_flow_srh_sid_type_get(int unit,
    int is_nonc,  /* is non-canonical tunnel? */
    int type_value,
    uint32 *flags)
{
    int ix;
    soc_cancun_ceh_field_info_t ceh_info;
    int rv;

    *flags = 0;

    /* if 0, default to STD_SID */
    if ((!type_value) && is_nonc) {
        *flags = BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID;
        return BCM_E_NONE;
    }
    for (ix = 0; ix < COUNTOF(tnl_ctrl_map); ix++) {
        rv = soc_cancun_ceh_obj_field_get(unit,
                 "EGR_IP_TUNNEL_IPV6_FLEX_EDIT_CTRL_ID",
                 tnl_ctrl_map[ix].name, &ceh_info);
        BCM_IF_ERROR_RETURN(rv);
        if (type_value == ceh_info.value) {
            *flags = tnl_ctrl_map[ix].flag;
            break;
        }
    }
    if (ix == COUNTOF(tnl_ctrl_map)) {
        if (type_value) {
            /* not found */
            return BCM_E_NOT_FOUND;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_tunnel_idx_oif_set
 * Purpose:
 *      Set the tunnel index to the given L3 egress interface ID.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      oif_idx - (IN) egress L3 interface ID.
 *      tnl_idx - (IN) EGR_IP_TUNNEL entry index.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 *    For EGR_IP_TUNNEL_IP6m index, should multiply by 2.
 */

int
_bcm_flow_tunnel_idx_oif_set(int unit, int oif_idx, int tnl_idx)
{
    uint32 oif_entry[SOC_MAX_MEM_WORDS];
    soc_mem_t oif_mem_view;
    soc_mem_t oif_mem;
    int data_type;
    int rv;
    uint32 action_set;

    oif_mem = EGR_L3_INTFm;
    soc_mem_lock(unit, oif_mem);
    rv = soc_mem_read(unit, oif_mem, MEM_BLOCK_ANY,
                                  oif_idx,oif_entry);
    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, oif_mem);
        return rv;
    }
    data_type = soc_mem_field32_get(unit, oif_mem,
                               oif_entry, DATA_TYPEf);

    /* find the memory view based on the memory and data type */
    rv = soc_flow_db_mem_to_view_id_get (unit, oif_mem,
                    SOC_FLOW_DB_KEY_TYPE_INVALID, data_type,
                    0, NULL, (uint32 *)&oif_mem_view);
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        soc_mem_unlock(unit, oif_mem);
        return rv;
    }
    if (SOC_SUCCESS(rv)) {
        if (SOC_MEM_FIELD_VALID(unit,oif_mem_view,
                                 TUNNEL_PTR_ACTION_SETf)) {
            action_set = 0;
            soc_format_field32_set(unit,TUNNEL_PTR_ACTION_SETfmt,
                                       &action_set,TUNNEL_INDEXf, tnl_idx);
            soc_mem_field32_set(unit, oif_mem_view, oif_entry,
                                      TUNNEL_PTR_ACTION_SETf, action_set);
        }
    } else {
        if (SOC_MEM_FIELD_VALID(unit, oif_mem, TUNNEL_INDEXf)) {
            soc_mem_field32_set(unit, oif_mem, oif_entry,
                                      TUNNEL_INDEXf, tnl_idx);
        }
    }
    rv = soc_mem_write(unit, oif_mem, MEM_BLOCK_ANY,
                                  oif_idx, oif_entry);
    soc_mem_unlock(unit, oif_mem);
    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
_bcm_flow_tunnel_srh_reinit(int unit, int soft_tnl_idx)
{
    int nh_idx;
    soc_mem_t mem;
    int rv;
    int data_type;
    uint32 action_set;
    uint32 nh_view_id;
    int next_ptr;
    int next_ptr_type;
    int dvp_vc_swap;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */

    nh_idx = soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
    /* increment the refcnt */
    BCM_XGS3_L3_ENT_REF_CNT_INC(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                            nh_idx, _BCM_SINGLE_WIDE);

    mem = EGR_L3_NEXT_HOPm;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, &entry);
    BCM_IF_ERROR_RETURN(rv);

    data_type = soc_mem_field32_get(unit, mem, &entry, DATA_TYPEf);
    rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &nh_view_id);
    BCM_IF_ERROR_RETURN(rv);

    action_set = soc_mem_field32_get(unit, nh_view_id, &entry,
                        NEXT_PTR_ACTION_SETf);

    next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
    next_ptr = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
    if (next_ptr_type == BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
        /* the L3 module should already recover the reference count once,
         * but not the hash
         */
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, next_ptr, &entry);
        BCM_IF_ERROR_RETURN(rv);
        if (!BCM_XGS3_L3_ENT_HASH(BCM_XGS3_L3_TBL_PTR(unit, next_hop), next_ptr)) {
            rv = _tnl_srv6_nh_hash_calc(unit, (void *)(&entry),
               &BCM_XGS3_L3_ENT_HASH(BCM_XGS3_L3_TBL_PTR(unit, next_hop),next_ptr));
            BCM_IF_ERROR_RETURN(rv);
        } else {
            BCM_XGS3_L3_ENT_REF_CNT_INC(BCM_XGS3_L3_TBL_PTR(unit, next_hop),
                            next_ptr, _BCM_SINGLE_WIDE);
        }

        data_type = soc_mem_field32_get(unit, mem, &entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &nh_view_id);
        BCM_IF_ERROR_RETURN(rv);
        action_set = soc_mem_field32_get(unit, nh_view_id, &entry,
                               NEXT_PTR_ACTION_SETf);
        next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
        if (next_ptr_type !=
                        BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP) {
            return BCM_E_NOT_FOUND;
        }
        dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                   &action_set, NEXT_PTRf);
        /* recover the vc_swap refcnt */
        rv = bcmi_td3_srv6_dvp_vc_swap_index_recover(unit, dvp_vc_swap);
        BCM_IF_ERROR_RETURN(rv);

    } else if (next_ptr_type ==
                          BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP) {
        /* non-canonical */
        dvp_vc_swap = next_ptr;
        rv = bcmi_td3_srv6_dvp_vc_swap_index_recover(unit, dvp_vc_swap);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}
#endif /* #ifdef BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_flow_hw_srv6_srh_entry_get
 * Purpose:
 *      Retrive SRv6 SRH header and SID from egr_ip_tunnel_ipv6 hardware
 *  table. The table may not contain complete SRv6 information.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      entry    - (IN) the egr_ip_tunnel_ipv6 entry.
 *      view_id  - (IN) the memory view of the entry.
 *      srh      - (OUT) SRH information retrieved from the entry.
 *      sid_list - (OUT) SID list retrieved from the entry.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */

int
_bcm_flow_hw_srv6_srh_entry_get(int unit, uint32 *entry,
                      soc_mem_t view_id,
                      bcm_flow_tunnel_srv6_srh_t *srh,
                      bcm_ip6_t *sid_list)
{
    int data_type;
    uint32 ip6[IP6_U32_ARR_LEN];
    bcm_flow_logical_field_t lfield;
    int rv;
    uint32 action_set = 0;
    _bcm_flow_bookkeeping_t *flow_info;
    int tnl_edit_ctrl_id;

    flow_info = FLOW_INFO(unit);
    data_type = soc_mem_field32_get(unit, view_id, entry, DATA_TYPEf);
    ip6[0] = 0;
    ip6[1] = 0;
    ip6[2] = 0;
    ip6[3] = 0;
    SAL_IP6_ADDR_FROM_UINT32(sid_list[0],ip6);
    SAL_IP6_ADDR_FROM_UINT32(sid_list[1],ip6);
    SAL_IP6_ADDR_FROM_UINT32(sid_list[2],ip6);
    sal_memset(srh, 0, sizeof(bcm_flow_tunnel_srv6_srh_t));

    tnl_edit_ctrl_id = soc_mem_field32_get(unit, view_id, entry,
                                           FLEX_EDIT_CTRL_IDf);
    if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L2) {
        rv = _bcm_esw_flow_srh_sid_type_get(unit, FALSE, tnl_edit_ctrl_id,
                                            &srh->flags);
        BCM_IF_ERROR_RETURN(rv);
        /* GSID_ENCAP_SID0_63_32 */
        ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                   DIRECT_ASSIGNMENT_0_ACTION_SETf);

        /* GSID_ENCAP_SID0_95_64 */
        ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_23_ACTION_SETf);

        /* GSID_ENCAP_SID0_111_96 */
        rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                     "GSID_ENCAP_SID0_111_96", &lfield.id);
        BCM_IF_ERROR_RETURN(rv);
        lfield.value = soc_mem_field32_get(unit,view_id, entry,
                          lfield.id);

        ip6[3] = lfield.value & 0xffff;
        /* GSID_ENCAP_SID0_127_112 */
        rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                     "GSID_ENCAP_SID0_127_112", &lfield.id);
        BCM_IF_ERROR_RETURN(rv);
        lfield.value = soc_mem_field32_get(unit, view_id, entry,
                       lfield.id);
        ip6[3] |= lfield.value << 16;

        SAL_IP6_ADDR_FROM_UINT32(sid_list[0],ip6);

        /* SID1_31_0 */
        ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_20_ACTION_SETf);
        /* SID1_63_32 */
        ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_21_ACTION_SETf);
        /* SID1_95_64 */
        ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_22_ACTION_SETf);
        /* SID1_127_96 */
        ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                   DIRECT_ASSIGNMENT_25_ACTION_SETf);
        SAL_IP6_ADDR_FROM_UINT32(sid_list[1], ip6);

        /* SID2_31_0 */
        ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_1_ACTION_SETf);
        /* SID2_63_32 */
        ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_2_ACTION_SETf);
        /* SID2_95_64 */
        ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_3_ACTION_SETf);
        /* SID2_127_96 */
        ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_24_ACTION_SETf);
        SAL_IP6_ADDR_FROM_UINT32(sid_list[2], ip6);

        /* SRH base header */
        action_set = soc_mem_field32_get(unit, view_id, entry,
                                    DIRECT_ASSIGNMENT_9_ACTION_SETf);
        srh->tag = action_set & 0xffff;
        srh->next_hdr = (action_set >> 24) & 0xff;

    } else if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L3) {
        rv = _bcm_esw_flow_srh_sid_type_get(unit, FALSE, tnl_edit_ctrl_id,
                                            &srh->flags);
        BCM_IF_ERROR_RETURN(rv);
        /* SID2_31_0 */
        ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_1_ACTION_SETf);
        /* SID2_63_32 */
        ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_2_ACTION_SETf);
        /* SID2_95_64 */
        ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_3_ACTION_SETf);
        /* SID2_127_96 */
        ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_24_ACTION_SETf);
        SAL_IP6_ADDR_FROM_UINT32(sid_list[2], ip6);

        /* GSID */
        /* GSID_ENCAP_SID0_95_64 */
        ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_23_ACTION_SETf);

        /* GSID_ENCAP_SID0_63_32 */
        ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_0_ACTION_SETf);

        /* GSID_ENCAP_SID0_111_96 */
        rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                     "GSID_ENCAP_SID0_111_96", &lfield.id);
        BCM_IF_ERROR_RETURN(rv);
        lfield.value = soc_mem_field32_get(unit,view_id, entry,
                              lfield.id);
        ip6[3] = lfield.value;

        /* GSID_ENCAP_SID0_127_112 */
        rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                          "GSID_ENCAP_SID0_127_112", &lfield.id);
        BCM_IF_ERROR_RETURN(rv);
        lfield.value = soc_mem_field32_get(unit,view_id, entry,
                              lfield.id);
        ip6[3] |= lfield.value << 16;
        ip6[0] = 0;
        SAL_IP6_ADDR_FROM_UINT32(sid_list[0], ip6);

    } else if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_NON_CANONICAL) {
        rv = _bcm_esw_flow_srh_sid_type_get(unit, TRUE, tnl_edit_ctrl_id,
                                            &srh->flags);
        BCM_IF_ERROR_RETURN(rv);
        if (srh->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID) {
            /* GSID_ENCAP_SID0_95_64 */
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_25_ACTION_SETf);
            /* GSID_ENCAP_SID0_63_32 */
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_0_ACTION_SETf);
            /* GSID_ENCAP_SID0_111_96 */
            rv = bcm_esw_flow_logical_field_id_get(unit,
                             flow_info->flow_srv6_non_canonical,
                             "GSID_ENCAP_SID0_111_96", &lfield.id);
            BCM_IF_ERROR_RETURN(rv);
            lfield.value = ip6[3] & 0xffff;
            ip6[3] = soc_mem_field32_get(unit, view_id, entry, lfield.id);

            /* GSID_ENCAP_SID0_127_112 */
            rv = bcm_esw_flow_logical_field_id_get(unit,
                          flow_info->flow_srv6_non_canonical,
                          "GSID_ENCAP_SID0_127_112", &lfield.id);
            BCM_IF_ERROR_RETURN(rv);
            lfield.value = soc_mem_field32_get(unit, view_id, entry,
                                lfield.id);
            ip6[3] |= lfield.value << 16;
        } else {
            /* SID1_31_0 */
            ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_15_ACTION_SETf);
            /* SID1_63_32 */
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_16_ACTION_SETf);
            /* SID1_95_64 */
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_20_ACTION_SETf);
            /* SID1_127_96 */
            ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_21_ACTION_SETf);
        }
        SAL_IP6_ADDR_FROM_UINT32(sid_list[1], ip6);
    } else if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_NON_CANONICAL_WO_DIP) {
        rv = _bcm_esw_flow_srh_sid_type_get(unit, TRUE, tnl_edit_ctrl_id,
                                            &srh->flags);
        BCM_IF_ERROR_RETURN(rv);

        if (srh->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID) {
            /* GSID_ENCAP_SID0_95_64 */
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_25_ACTION_SETf);
            /* GSID_ENCAP_SID0_63_32 */
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_0_ACTION_SETf);
            /* GSID_ENCAP_SID0_111_96 */
            rv = bcm_esw_flow_logical_field_id_get(unit,
                             flow_info->flow_srv6_non_canonical,
                             "GSID_ENCAP_SID0_111_96", &lfield.id);
            BCM_IF_ERROR_RETURN(rv);
            lfield.value = ip6[3] & 0xffff;
            ip6[3] = soc_mem_field32_get(unit, view_id, entry, lfield.id);

            /* GSID_ENCAP_SID0_127_112 */
            rv = bcm_esw_flow_logical_field_id_get(unit,
                          flow_info->flow_srv6_non_canonical,
                          "GSID_ENCAP_SID0_127_112", &lfield.id);
            BCM_IF_ERROR_RETURN(rv);
            lfield.value = soc_mem_field32_get(unit, view_id, entry,
                                         lfield.id);
            ip6[3] |= lfield.value << 16;
            SAL_IP6_ADDR_FROM_UINT32(sid_list[1], ip6);

            /* SID1_31_0 */
            ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                                  DIRECT_ASSIGNMENT_15_ACTION_SETf);
            /*SID1_63_32*/
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                                  DIRECT_ASSIGNMENT_16_ACTION_SETf);
            /*SID1_95_64*/
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_20_ACTION_SETf);
            /*SID1_127_96*/
            ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                          DIRECT_ASSIGNMENT_21_ACTION_SETf);
            SAL_IP6_ADDR_FROM_UINT32(sid_list[2], ip6);
        } else {
            /* SID1_31_0 */
            ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_15_ACTION_SETf);
            /* SID1_63_32 */
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_16_ACTION_SETf);
            /* SID1_95_64 */
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_20_ACTION_SETf);
            /* SID1_127_96 */
            ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_21_ACTION_SETf);
            SAL_IP6_ADDR_FROM_UINT32(sid_list[1], ip6);

            /* SID2_31_0 */
            ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                               DIRECT_ASSIGNMENT_1_ACTION_SETf);
            /* SID2_63_32 */
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                               DIRECT_ASSIGNMENT_2_ACTION_SETf);
            /* SID2_95_64 */
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                               DIRECT_ASSIGNMENT_3_ACTION_SETf);
            /* SID2_127_96 */
            ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                               DIRECT_ASSIGNMENT_24_ACTION_SETf);
            SAL_IP6_ADDR_FROM_UINT32(sid_list[2], ip6);
        }
    } else {
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _tnl_init_ip6_del
 * Purpose:
 *      Delete the ipv6 tunnel entry for the given index.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      flags - (IN) control XGS3_L3_HWCALL delete operation, such as
 *                  _BCM_L3_SHR_WRITE_DISABLE.
 *      index - (IN) point to the delete entry.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */

STATIC int
_tnl_init_ip6_del(int unit, int flags, int index)
{
    _bcm_l3_tbl_op_t data;               /* Delete operation info. */
    uint32 tnl_entry[SOC_MAX_MEM_FIELD_WORDS];
    int rv;

    /* Initialization */
    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));

    data.width = _BCM_DOUBLE_WIDE;

    /* Check hw call is defined. */
    if (!BCM_XGS3_L3_HWCALL_CHECK(unit, tnl_init_del)) {
        return (BCM_E_UNAVAIL);
    }
    data.tbl_ptr =  BCM_XGS3_L3_TBL_PTR(unit, tnl_init);
    data.entry_index = index;
    data.oper_flags  = flags;
    data.delete_func = BCM_XGS3_L3_HWCALL(unit, tnl_init_del);

    /* Delete tunnel initiator table entry. */
    rv = _bcm_xgs3_tbl_del(unit, &data);
    BCM_IF_ERROR_RETURN(rv);

    /* don't clear if the entry is still been referenced by other objs */
    if (!BCM_XGS3_L3_ENT_REF_CNT(data.tbl_ptr, index)) {
        sal_memset(tnl_entry, 0, sizeof(tnl_entry));
        rv = soc_mem_write(unit, EGR_IP_TUNNEL_IPV6m, MEM_BLOCK_ALL,
                             index >> 1, tnl_entry);
        BCM_IF_ERROR_RETURN(rv);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _tnl_srv6_nh_hash_calc
 * Purpose:
 *      Calculate the hash value of the next hop entry using crc16.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      buf   - (IN) point to a egr_l3_next_hop entry.
 *      hash  - (OUT) Calculated hash value.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */
STATIC int
_tnl_srv6_nh_hash_calc(int unit, void *buf, uint16 *hash)
{
    egr_l3_next_hop_entry_t *nh_entry;        /* Next hop entry from api. */

    /* Input parameters check. */
    if ((NULL == buf) || (NULL == hash)) {
        return (BCM_E_PARAM);
    }
    nh_entry = (egr_l3_next_hop_entry_t *) buf;

    *hash = _shr_crc16(0, (uint8 *)nh_entry, sizeof(egr_l3_next_hop_entry_t));

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _tnl_srv6_nh_entry_cmp
 * Purpose:
 *      Compare the given next hop entry with the hardware entry pointed by
 *   the index and provided the equal or non-equal result.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      buf   - (IN) point to the given egr_l3_next_hop entry.
 *      index - (IN) index to a hardware entry.
 *      cmp_result - (OUT) comparison result.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */
STATIC int
_tnl_srv6_nh_entry_cmp(int unit, void *buf, int index, int *cmp_result)
{
    egr_l3_next_hop_entry_t *nh_entry;        /* Next hop entry from api. */
    egr_l3_next_hop_entry_t hw_entry;        /* Next hop entry from hardware. */

    if ((NULL == buf) || (NULL == cmp_result))
        return (BCM_E_PARAM);

    /* Get api entry info. */
    nh_entry = (egr_l3_next_hop_entry_t *) buf;
    sal_memset(&hw_entry, 0, sizeof(hw_entry));

    /* Get next hop entry from hw. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, EGR_L3_NEXT_HOPm,
            MEM_BLOCK_ANY, index, &hw_entry));

    if (sal_memcmp(nh_entry, &hw_entry, sizeof(egr_l3_next_hop_entry_t))) {
        *cmp_result = BCM_L3_CMP_NOT_EQUAL;
    } else {
        *cmp_result = BCM_L3_CMP_EQUAL;
    }
    return BCM_E_NONE;
}

int
_bcm_flow_srv6_nh_add(int unit, uint32 flags,
                     egr_l3_next_hop_entry_t *nh_entry,
                     int *index)
{
    _bcm_l3_tbl_op_t data;        /* Operation data. */

    /* Input parameters check. */
    if (NULL == nh_entry) {
        return (BCM_E_PARAM);
    }

    /* Initialization */
    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));
    data.tbl_ptr =  BCM_XGS3_L3_TBL_PTR(unit, next_hop);
    data.width = _BCM_SINGLE_WIDE;
    data.oper_flags = flags;
    data.entry_buffer = (void *)nh_entry;
    data.hash_func = _tnl_srv6_nh_hash_calc;
    data.cmp_func  = _tnl_srv6_nh_entry_cmp;
    data.add_func  = BCM_XGS3_L3_HWCALL(unit, nh_add);

    /* Add next hop table entry. */
    BCM_IF_ERROR_RETURN(_bcm_xgs3_tbl_add(unit, &data));
    *index = data.entry_index;
    return BCM_E_NONE;
}

int
_bcm_flow_srv6_nh_del(int unit, uint32 flags, int nh_index)
{
    _bcm_l3_tbl_op_t data;        /* Operation data. */
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv = BCM_E_NONE;

    /* Initialization */
    sal_memset(&data, 0, sizeof(_bcm_l3_tbl_op_t));
    data.tbl_ptr =  BCM_XGS3_L3_TBL_PTR(unit, next_hop);
    data.entry_index = nh_index;
    data.width = _BCM_SINGLE_WIDE;
    data.oper_flags = flags;
    data.delete_func = BCM_XGS3_L3_HWCALL(unit, nh_del);

    /* Delete next hop table entry. */
    rv = _bcm_xgs3_tbl_del(unit, &data);
    BCM_IF_ERROR_RETURN(rv);

    /* clear the entry if no other object is using */
    if (!BCM_XGS3_L3_ENT_REF_CNT(data.tbl_ptr, nh_index)) {
        sal_memset(entry, 0, sizeof(entry));
        rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, nh_index, entry);
        BCM_IF_ERROR_RETURN(rv);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_flow_tunnel_srv6_dvp_vc_swap_init
 * Purpose:
 *      Populate egr_dvp_attribute entry and vc_swap entry with SRv6 SIDs and
 *   and edit_ctrl_id based on SRv6 tunnel type: L2, L3 or non-canonical.
 * Parameters:
 *      unit  - (IN) SOC unit number.
 *      tnl_type - (IN) SRv6 tunnel type: L2, L3 and non-canonical.
 *      sid_list - (IN) given SID list.
 *      vc_edit_ctrl_id - (IN) given edit_ctrl_id.
 *      vc_entry  - (OUT) vc_and_swap entry to populate.
 *      dvp_entry - (OUT) dvp entry to populate
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 *   for L2, DVP entry should be already initialized.
 */
int
_bcm_flow_tunnel_srv6_dvp_vc_swap_init(int unit,
                      int tnl_type,
                      bcm_ip6_t *sid_list,
                      int vc_edit_ctrl_id,
                      egr_mpls_vc_and_swap_label_table_entry_t  *vc_entry,
                      egr_dvp_attribute_entry_t *dvp_entry)
{
    uint32 dvp_option = 0;
    uint32 vc_swap_option;
    uint32 ip6[IP6_U32_ARR_LEN];
    soc_mem_t mem_view;
    int rv;
    _bcm_flow_bookkeeping_t *flow_info;
    uint32 flow_handle;
    int data_type;

    flow_info = FLOW_INFO(unit);

    /* L3 tunnel */
    if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_L3) {
        flow_handle = flow_info->flow_srv6;
        rv = bcm_esw_flow_option_id_get(unit, flow_info->flow_srv6,
                "L3_ASSIGN_SEGMENT_LIST_0", &dvp_option);
        BCM_IF_ERROR_RETURN(rv);
    } else if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_L2) {
        flow_handle = flow_info->flow_srv6;
        /* for L2, DVP entry should be initialized in encap_set */
    } else if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_NON_CANONICAL) {
        flow_handle = flow_info->flow_srv6_non_canonical;
        rv = bcm_esw_flow_option_id_get(unit,
                                   flow_info->flow_srv6_non_canonical,
                                   "ASSIGN_SEGMENT_LIST_0", &dvp_option);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        return BCM_E_PARAM;
    }

    /* initialize the dvp entry with all control fields */
    if (tnl_type != _BCM_FLOW_SRV6_SRH_TYPE_L2 && (dvp_entry != NULL)) {
        /* init dvp entry */
        rv = soc_flow_db_ffo_to_mem_view_id_get(unit, flow_handle,
                   dvp_option,
                   SOC_FLOW_DB_FUNC_ENCAP_SET_ID,
                    (uint32 *)&mem_view);
        BCM_IF_ERROR_RETURN(rv);

        sal_memset(dvp_entry, 0, sizeof(egr_dvp_attribute_entry_t));
        soc_flow_db_mem_view_entry_init(unit, mem_view, (uint32 *)dvp_entry);
        if (sid_list != NULL) {
            SAL_IP6_ADDR_TO_UINT32(sid_list[0], ip6);

            if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_NON_CANONICAL) {
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_11_ACTION_SETf, ip6[0]);
                /* SID0_63_32 */
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_12_ACTION_SETf, ip6[1]);
                /* SID0_95_64 */
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_13_ACTION_SETf, ip6[2]);
                /* SID0_127_96 */
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_14_ACTION_SETf, ip6[3]);
            } else {
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_10_ACTION_SETf, ip6[0]);
                /* SID0_63_32 */
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_12_ACTION_SETf, ip6[1]);
                /* SID0_95_64 */
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_14_ACTION_SETf, ip6[2]);
                /* SID0_127_96 */
                soc_mem_field32_set(unit, mem_view, dvp_entry,
                        DIRECT_ASSIGNMENT_16_ACTION_SETf, ip6[3]);
            }
        }
    }

    if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_L2) {
        rv = bcm_esw_flow_option_id_get(unit, flow_handle,
             "L2_OUTER_DIP_MSB_BITS_AND_PAYLOAD_OTAG_ACTIONS", &vc_swap_option);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        rv = bcm_esw_flow_option_id_get(unit, flow_handle,
             "L3_ASSIGN_SEGMENT_LIST_1", &vc_swap_option);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* initialize the vc_and_swap entry with all control fields */
    rv = soc_flow_db_ffo_to_mem_view_id_get(unit,flow_handle,
               vc_swap_option,
               SOC_FLOW_DB_FUNC_EGRESS_LABEL_ID,
               (uint32 *)&mem_view);
    BCM_IF_ERROR_RETURN(rv);

    if (vc_entry != NULL) {
        data_type = soc_mem_field32_get(unit, mem_view, vc_entry, DATA_TYPEf);

        /* check if vc_entry already initialized for L2 type */
        if ((tnl_type != _BCM_FLOW_SRV6_SRH_TYPE_L2) ||
            (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_L2 && data_type == 0)) {
             sal_memset(vc_entry, 0, sizeof(egr_mpls_vc_and_swap_label_table_entry_t));
             soc_flow_db_mem_view_entry_init(unit, mem_view, (uint32 *)vc_entry);
        }
        if (sid_list != NULL) {
            if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_L2) {
                /* dip6_127_96 in vc_swap entry */
                SAL_IP6_ADDR_TO_UINT32(sid_list[0], ip6);
                soc_mem_field32_set(unit, mem_view,  vc_entry,
                           DIRECT_ASSIGNMENT_16_ACTION_SETf, ip6[3]);
            } else {
                SAL_IP6_ADDR_TO_UINT32(sid_list[1], ip6);
                soc_mem_field32_set(unit, mem_view,  vc_entry,
                           DIRECT_ASSIGNMENT_25_ACTION_SETf, ip6[3]);
            }
        }
        /* indicate number of SIDs */
        soc_mem_field32_set(unit, mem_view, vc_entry, EDIT_CTRL_IDf, vc_edit_ctrl_id);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _tnl_srv6_id_validate
 * Purpose:
 *      Validate the given tunnel_id is for SRv6 tunnel
 * Parameters:
 *      unit      - (IN) SOC unit number.
 *      tunnel_id - (IN) the given tunnel ID .
 *      flag      - (OUT) return flag to indicate canonical or non-canonical
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */
STATIC int
_tnl_srv6_id_validate( int unit, bcm_gport_t tunnel_id, uint32 *flag)
{
    soc_mem_t mem;
    int data_type;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int oif_idx;
    int tnl_idx;
    int rv = BCM_E_NONE;
    int ipv6m_tnl_idx;

    mem = EGR_IP_TUNNEL_IPV6m;
    sal_memset(entry, 0, sizeof(entry));

    rv = _bcm_flow_tunnel_and_oif_index_get(unit, tunnel_id,
                   &oif_idx, &tnl_idx);
    BCM_IF_ERROR_RETURN(rv);

    ipv6m_tnl_idx = tnl_idx >> 1;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx, entry);
    BCM_IF_ERROR_RETURN(rv);

    data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
    if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L3 ||
        data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_L2) {
        if (flag != NULL) {
            *flag = BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL;
        }
    } else if (data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_NON_CANONICAL ||
           data_type == BCMI_TD3_TNL_DATA_TYPE_SRV6_NON_CANONICAL_WO_DIP) {
        /* non-canonical. No flag needs to set */
    } else {
        /* incorrect SRV6 tunnel */
        rv = BCM_E_NOT_FOUND;
    }
    return rv;
}

STATIC int
_bcm_esw_flow_tnl_srv6_srh_validate(int unit,
       uint32 options,
       bcm_flow_tunnel_srv6_srh_t *info,
       int nofs,
       bcm_ip6_t *sid_list)
{
    int soft_tnl_idx;
    _bcm_flow_bookkeeping_t *flow_info;

    if (!soc_feature(unit, soc_feature_srv6_enable)) {
        return BCM_E_UNAVAIL;
    }

    flow_info = FLOW_INFO(unit);

    /* parameter check */
    if (!(options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_REPLACE)) {
        /* first time, creation */
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL) {
            if ((!(options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID)) ||
                (!_SHR_GPORT_IS_TUNNEL(info->tunnel_id)) ) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                  "srv6 : Missing either with_id option or invalid tunnel_id\n")));
                return BCM_E_PARAM;

            }
            soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
            _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);
            if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: tunnel_id has not been created\n")));
                return BCM_E_PARAM;
            }
        } else { /* non-canonical tunnel */
            if (options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                      "srv6 : invalid with_id option for non-canonical tunnel\n")));
                return BCM_E_PARAM;
            }
            if ((!(info->valid_elements &
                           BCM_FLOW_TUNNEL_SRV6_SRH_EGR_OBJ_VALID)) ||
                    (!BCM_XGS3_L3_EGRESS_IDX_VALID(unit, info->egr_obj))) {
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                      "srv6 : Missing valid flag or invalid egr_obj\n")));
                return BCM_E_PARAM;
            }
        }
    } else { /* replace */
        if (!(options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                  "srv6 : with_id option is not set for replace action\n")));
            return BCM_E_PARAM;
        }
        if (!_SHR_GPORT_IS_TUNNEL(info->tunnel_id)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                  "srv6 : invalid tunnel_id\n")));
            return BCM_E_PARAM;
        }
        if (info->valid_elements & BCM_FLOW_TUNNEL_SRV6_SRH_EGR_OBJ_VALID) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                      "srv6 : cannot replace egr_obj\n")));
            return BCM_E_PARAM;
        }
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
        _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: tunnel_id has not been created\n")));
            return BCM_E_PARAM;
        }
    }
    if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_USID) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
              "srv6: Use TYPE_STD_SID for uSID operation\n")));
        return BCM_E_UNAVAIL;
    }

    if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE) {
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
              "srv6: GSID is not supported in reduced mode\n")));
            return BCM_E_UNAVAIL;
        }
        if (nofs > (TNL_SRV6_SID_MAX - 1)) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
              "srv6: Max number of sid supported in reduced mode is %d\n"),
               TNL_SRV6_SID_MAX - 1));
            return BCM_E_UNAVAIL;
        }
    }

    if (nofs > TNL_SRV6_SID_MAX || nofs < 0 || (sid_list == NULL && nofs != 0)) {
        /* only support maximum 3 SIDs */
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: Support maximum 3  SIDs\n")));
        return BCM_E_PARAM;
    }

    /* Non-canonical */
    if (!(options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID)) {
        if (!(info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL) &&
            (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID) &&
            nofs == 3) {
            LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: Non-canonical with GSID doesn't support 3 SIDs\n")));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_flow_tunnel_srv6_srh_set
 * Purpose:
 *     Common function to handle add/delete/replace actions. For all entries,
 *  will use match/allocate method to make sure the same value will use a
 *  single entry.
 *
 *  case 1 L3 canonical:
 *    For add action, a dvp_vc_swap entry and ipv6_tunnel is allocated in tunnel
 *    function. Here needs to allocate a next_hop_2 entry and populate the
 *    next_hop_1, dvp_vc_swap and ipv6_tunnel entries. For later two entries,
 *    add new entry after population and delete the old one to ensure same value
 *    entry is not allocated twice. The final table linkage:
 *       first next hop --> second next hop --> dvp_vc_swap
 *            |
 *            |--------> egr_l3_intf ----> egr_ip_tunnel_ipv6
 *
 *  case 2 L2 canonical:
 *    For add action, no new object is allocated. Only populate the dvp_vc_swap
 *    and tunnel_ipv6 entries. Again population is done with add/delete, not
 *    modification directly. Final table linkage:
 *       dvp_vc_swap
 *       l3_intf_id ----> egr_ip_tunnel_ipv6
 *
 *  case 3 Non-canonical:
 *    For add action, allocate dvp_vc_swap and tunnel_ipv6. Final table linkage:
 *       egr_obj(next hop) --> dvp_vc_swap
 *             |
 *             |-------> egr_intf_id ---> egr_ip_tunnel_ipv6
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      actions - (IN) operation action: add/replace/delete
 *      info    - (IN/OUT) SRH parameter descriptor
 *      nofs    - (IN) number of SIDs
 *      sid_list - (IN) SID list. Size indicated by above nofs parameter
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_esw_flow_tunnel_srv6_srh_set(int unit,
       uint32 actions,
       bcm_flow_tunnel_srv6_srh_t *info,
       int nofs,
       bcm_ip6_t *sid_arr)
{
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    egr_l3_next_hop_entry_t nh_entry;
    egr_mpls_vc_and_swap_label_table_entry_t vc_entry;
    egr_dvp_attribute_entry_t dvp_entry;
    uint32 ip6[IP6_U32_ARR_LEN];
    int tnl_type;
    bcm_flow_tunnel_initiator_t tnl_info;
    int vc_edit_ctrl_id  = 0;
    int tnl_edit_ctrl_id = 0;
    int soft_tnl_idx = 0;
    int tnl_idx;
    int oif_idx;
    bcm_flow_logical_field_t lfield;
    uint32 view_id;
    uint32 nh_view_id;
    _bcm_flow_bookkeeping_t *flow_info;
    bcm_ip6_t tnl_sid_list[TNL_SRV6_SID_MAX];  /* sid list in hardware tunnel table */
    bcm_ip6_t sid_list[TNL_SRV6_SID_MAX];  /* sid list mapping */
    bcm_flow_tunnel_srv6_srh_t tnl_srh; /* srh in hardware tunnel table */
    uint32 action_set;
    int data_type;
    int next_ptr_type;
    int i;
    int rv, rv1;
    uint32 flags;
    int dvp_vc_swap = 0;
    int nh2_idx = 0;  /* second next hop pointed to by first next hop */
    int nh_idx = 0; /* first next hop */
    int tnl_idx_new = 0;
    int dvp_vc_swap_new = 0;
    int nh2_idx_new = 0; /* the newly allocated second next hop */
    uint32 *nh2_entry;  /* point to next_hop_2 entry */
    uint32 *tnl_entry;  /* point to egr_ip_tunnel_ipv6 entry */
    int ipv6m_tnl_idx;  /* egr_ip_tunnel_ip6 memory index */
    int l3_oif_cnt = 0;

    if (actions & SRV6_ACTION_WITH_ID) {
        soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    } else {
        soft_tnl_idx = info->egr_obj - BCM_XGS3_EGRESS_IDX_MIN(unit) +
                       _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
    }

    flow_info = FLOW_INFO(unit);

    rv = _bcm_flow_tunnel_and_oif_index_get(unit, info->tunnel_id,
                               &oif_idx, &tnl_idx);
    BCM_IF_ERROR_RETURN(rv);

    for (i = 0; i < TNL_SRV6_SID_MAX; i++) {
        sal_memset(tnl_sid_list[i], 0, sizeof(bcm_ip6_t));
        sal_memset(sid_list[i], 0, sizeof(bcm_ip6_t));
    }

    /* map user sid_list to internal programming list */
    if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE &&
        info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL) {
        for (i = 0; (i < nofs) && (i < (TNL_SRV6_SID_MAX - 1)); i++) {
            sal_memcpy(tnl_sid_list[i + 1], sid_arr[i], sizeof(bcm_ip6_t));
            sal_memcpy(sid_list[i + 1],     sid_arr[i], sizeof(bcm_ip6_t));
        }
    } else {
        for (i = 0; i < nofs; i++) {
            sal_memcpy(tnl_sid_list[i], sid_arr[i], sizeof(bcm_ip6_t));
            sal_memcpy(sid_list[i],     sid_arr[i], sizeof(bcm_ip6_t));
        }
    }

    sal_memset(&tnl_srh, 0, sizeof(tnl_srh));
    sal_memset(entry, 0, sizeof(entry));
    bcm_flow_tunnel_initiator_t_init(&tnl_info);

    if (actions & SRV6_ACTION_DELETE) {
        vc_edit_ctrl_id  = 0;
        tnl_edit_ctrl_id = 0;
    } else {
        rv = _bcm_esw_flow_srh_edit_ctrl_id_get(unit, info->flags, nofs,
                   &vc_edit_ctrl_id, &tnl_edit_ctrl_id);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL) {
        if (soft_tnl_idx > _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
            tnl_type = _BCM_FLOW_SRV6_SRH_TYPE_L3;
        } else {
            tnl_type = _BCM_FLOW_SRV6_SRH_TYPE_L2;
        }
    } else {
        tnl_type = _BCM_FLOW_SRV6_SRH_TYPE_NON_CANONICAL;
    }

    if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_L3) {
        /* soft_tnl_idx should be egress object index when tunnel is created */
        nh_idx = soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        mem = EGR_L3_NEXT_HOPm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, &nh_entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, &nh_entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &nh_view_id);
        BCM_IF_ERROR_RETURN(rv);

        action_set = soc_mem_field32_get(unit, nh_view_id, &nh_entry,
                        NEXT_PTR_ACTION_SETf);

        next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
        nh2_entry = entry;
        if (actions & SRV6_ACTION_REPLACE || actions & SRV6_ACTION_DELETE) {
            if (!next_ptr_type) {
                return BCM_E_PARAM;
            }
            nh2_idx = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh2_idx, nh2_entry);
            BCM_IF_ERROR_RETURN(rv);
            action_set = soc_mem_field32_get(unit, flow_info->nh2_mem_view,
                                     nh2_entry, NEXT_PTR_ACTION_SETf);
            dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
        } else {
            if (next_ptr_type) {  /* already attached to a srh */
                LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                    "srv6: a SRH entry is already attached to this tunnel_id\n")));
                return BCM_E_EXISTS;
            }
            dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);

            /* init next hop 2 entry which holds SRV6 segment list1 */
            soc_flow_db_mem_view_entry_init(unit, flow_info->nh2_mem_view,
                        nh2_entry);
        }

        SAL_IP6_ADDR_TO_UINT32(sid_list[1], ip6);
        /* SID1_31_0 */
        soc_mem_field32_set(unit, flow_info->nh2_mem_view, nh2_entry,
                DIRECT_ASSIGNMENT_20_ACTION_SETf, ip6[0]);
        /* SID1_63_32 */
        soc_mem_field32_set(unit, flow_info->nh2_mem_view, nh2_entry,
                DIRECT_ASSIGNMENT_21_ACTION_SETf, ip6[1]);
        /* SID1_95_64 */
        soc_mem_field32_set(unit, flow_info->nh2_mem_view, nh2_entry,
                DIRECT_ASSIGNMENT_22_ACTION_SETf, ip6[2]);

        sal_memset(&dvp_entry, 0, sizeof(dvp_entry));
        sal_memset(&vc_entry, 0, sizeof(vc_entry));
        rv = _bcm_flow_tunnel_srv6_dvp_vc_swap_init(unit,
                      _BCM_FLOW_SRV6_SRH_TYPE_L3,
                       sid_list, vc_edit_ctrl_id, &vc_entry, &dvp_entry);
        BCM_IF_ERROR_RETURN(rv);

        /* dvp entry is set up in tunnel_create, overwrite it */
        rv = soc_mem_read(unit, EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ANY,
                                  dvp_vc_swap, &dvp_entry);
        BCM_IF_ERROR_RETURN(rv);

        /* check if the same dvp_vc_swap entry exist */
        rv = _bcm_flow_dvp_vc_swap_entry_index_alloc(unit, &vc_entry,
                   &dvp_entry, TRUE, &dvp_vc_swap_new);
        BCM_IF_ERROR_RETURN(rv);

        /* write to the memory */
        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap_new,
                               &vc_entry);
        _BCM_FLOW_CLEANUP(rv);

        mem = EGR_DVP_ATTRIBUTEm;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap_new,
                               &dvp_entry);
        _BCM_FLOW_CLEANUP(rv);

        flags = _BCM_L3_SHR_WRITE_DISABLE;
        if (actions & SRV6_ACTION_DELETE) {
            rv = _bcm_flow_srv6_nh_del(unit, 0, nh2_idx);
            _BCM_FLOW_CLEANUP(rv);
        } else {
            /* set the given egress object to point to dvp/vc_and_swap memory */
            action_set = 0;
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                            &action_set, NEXT_PTR_TYPEf,
                            BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP);
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                                   &action_set, NEXT_PTRf, dvp_vc_swap_new);
            soc_mem_field32_set(unit, flow_info->nh2_mem_view, nh2_entry,
                                NEXT_PTR_ACTION_SETf, action_set);

            /* allocate a new second next hop and point to dvp and
             * vc_and_swap entry
             */
            rv = _bcm_flow_srv6_nh_add(unit, flags,
                         (egr_l3_next_hop_entry_t *)nh2_entry, &nh2_idx_new);
            _BCM_FLOW_CLEANUP(rv);

            rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                          nh2_idx_new, nh2_entry);
            _BCM_FLOW_CLEANUP(rv);

            if (actions & SRV6_ACTION_REPLACE) {
                if (nh2_idx) {
                    rv = _bcm_flow_srv6_nh_del(unit, 0, nh2_idx);
                    _BCM_FLOW_CLEANUP(rv);
                }
            }
        }

        mem = EGR_IP_TUNNEL_IPV6m;
        sal_memset(entry, 0, sizeof(entry));
        tnl_entry = entry;

        /* tunnel index should be divided by 2 for EGR_IP_TUNNEL_IPV6m */
        ipv6m_tnl_idx = tnl_idx >> 1;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx, tnl_entry);
        _BCM_FLOW_CLEANUP(rv);

        SAL_IP6_ADDR_TO_UINT32(sid_list[2], ip6);

        /* SID2_31_0 */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
              DIRECT_ASSIGNMENT_1_ACTION_SETf, ip6[0]);
        /* SID2_63_32 */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
              DIRECT_ASSIGNMENT_2_ACTION_SETf, ip6[1]);
        /* SID2_95_64 */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
              DIRECT_ASSIGNMENT_3_ACTION_SETf, ip6[2]);
        /* SID2_127_96 */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
              DIRECT_ASSIGNMENT_24_ACTION_SETf, ip6[3]);

        /* GSID */
        SAL_IP6_ADDR_TO_UINT32(sid_list[0], ip6);
        /* GSID_ENCAP_SID0_95_64 */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
              DIRECT_ASSIGNMENT_23_ACTION_SETf, ip6[2]);

        /* GSID_ENCAP_SID0_63_32 */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
              DIRECT_ASSIGNMENT_0_ACTION_SETf, ip6[1]);

        /* GSID_ENCAP_SID0_111_96 */
        rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                     "GSID_ENCAP_SID0_111_96", &lfield.id);
        _BCM_FLOW_CLEANUP(rv);

        lfield.value = ip6[3] & 0xffff;
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
                              lfield.id, lfield.value);

        /* GSID_ENCAP_SID0_127_112 */
        rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                     "GSID_ENCAP_SID0_127_112", &lfield.id);
        _BCM_FLOW_CLEANUP(rv);

        lfield.value = ip6[3] >> 16;
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
                              lfield.id, lfield.value);

        /* indicate standard sid, gsid or usid */
        soc_mem_field32_set(unit, flow_info->tnl_l3_mem_view, tnl_entry,
                 FLEX_EDIT_CTRL_IDf, tnl_edit_ctrl_id);

        tnl_info.type = bcmTunnelTypeSR6;
        tnl_info.valid_elements = BCM_FLOW_TUNNEL_INIT_SIP6_VALID;
        rv = _bcm_flow_hw_tunnel_entry_get(unit, tnl_idx, &tnl_info, NULL,
                           0, NULL);
        _BCM_FLOW_CLEANUP(rv);

        rv = bcm_esw_flow_option_id_get(unit, flow_info->flow_srv6,
                 "L3_ASSIGN_SIP_AND_SEGMENT_LIST_2", &tnl_info.flow_option);
        _BCM_FLOW_CLEANUP(rv);

        tnl_info.flow_handle = flow_info->flow_srv6;

        flags =  _BCM_L3_SHR_WRITE_DISABLE | _BCM_L3_SHR_SKIP_INDEX_ZERO;

        /* only sid0(gsid) or sid2 is in egr_ip_tunnel_ipv6 */
        sal_memset(tnl_sid_list[1], 0, sizeof(bcm_ip6_t));
        tnl_srh.flags = info->flags &
                            (BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID |
                             BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID |
                             BCM_FLOW_TUNNEL_SRV6_SRH_GSID_PREFIX2);
        rv = _bcm_flow_tnl_init_add(unit, flags, &tnl_info, NULL,
                                 0, &tnl_srh, tnl_sid_list, &tnl_idx_new);
        _BCM_FLOW_CLEANUP(rv);

        if (tnl_idx != tnl_idx_new) {
            /* need a new EGR_IP_TUNNEL entry */
            ipv6m_tnl_idx = tnl_idx_new >> 1;
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx,
                               tnl_entry);
            _BCM_FLOW_CLEANUP(rv);

            rv = _bcm_flow_tunnel_idx_oif_set(unit, oif_idx, tnl_idx_new);
            _BCM_FLOW_CLEANUP(rv);

            flow_info->init_tunnel[soft_tnl_idx].idx = tnl_idx_new;
        } else {
            /* a same entry exists. Do nothing */
        }

        /* connect the 2nd next hop object to the 1st next hop */
        mem = EGR_L3_NEXT_HOPm;
        action_set = 0;
        if (actions & SRV6_ACTION_DELETE) {
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf,
                               0);
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf, dvp_vc_swap_new);
            soc_mem_field32_set(unit, nh_view_id, &nh_entry, NEXT_PTR_ACTION_SETf,
                                action_set);
        } else {
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf,
                               BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH);
            soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf, nh2_idx_new);
            soc_mem_field32_set(unit, nh_view_id, &nh_entry, NEXT_PTR_ACTION_SETf,
                                action_set);
        }

        /* SRH base header */
        action_set = info->tag | (info->flags >> 16);
        soc_mem_field32_set(unit, nh_view_id, &nh_entry,
                  DIRECT_ASSIGNMENT_9_ACTION_SETf, action_set);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, nh_idx, &nh_entry);
        _BCM_FLOW_CLEANUP(rv);

        /* at last clean up replaced resource */
        /* if x_new == x, the _index_free just descreases the reference count */
        if (dvp_vc_swap) {
            rv = _bcm_flow_dvp_vc_swap_entry_index_free(unit, &dvp_entry,
                       TRUE, dvp_vc_swap);
        }

        rv1 = BCM_E_NONE;
        if (tnl_idx) {
            rv1 = _tnl_init_ip6_del(unit, _BCM_L3_SHR_WRITE_DISABLE, tnl_idx);
        }
        /* if error, simply return, the new objects already linked */
        BCM_IF_ERROR_RETURN(rv);
        BCM_IF_ERROR_RETURN(rv1);

    } else if (tnl_type == _BCM_FLOW_SRV6_SRH_TYPE_NON_CANONICAL) {
        /* non-canonical */
        nh_idx = soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        mem = EGR_L3_NEXT_HOPm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, &nh_entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, &nh_entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &nh_view_id);
        BCM_IF_ERROR_RETURN(rv);

        action_set = soc_mem_field32_get(unit, nh_view_id, &nh_entry,
                        NEXT_PTR_ACTION_SETf);
        next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                   &action_set, NEXT_PTR_TYPEf);
        dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                   &action_set, NEXT_PTRf);
        action_set = info->tag | (info->flags >> 16);
        soc_mem_field32_set(unit, nh_view_id, &nh_entry,
                      DIRECT_ASSIGNMENT_9_ACTION_SETf, action_set);

        if (actions & SRV6_ACTION_DELETE) {
            rv = soc_mem_read(unit, EGR_DVP_ATTRIBUTEm, MEM_BLOCK_ANY,
                                   dvp_vc_swap, &dvp_entry);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_flow_dvp_vc_swap_entry_index_free(unit, &dvp_entry,
                          TRUE, dvp_vc_swap);
            BCM_IF_ERROR_RETURN(rv);
            rv = _tnl_init_ip6_del(unit, _BCM_L3_SHR_WRITE_DISABLE, tnl_idx);
            BCM_IF_ERROR_RETURN(rv);
            soc_mem_field32_set(unit, nh_view_id, &nh_entry,
                                NEXT_PTR_ACTION_SETf, 0);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, nh_idx, &nh_entry);
            BCM_IF_ERROR_RETURN(rv);
            rv = _bcm_flow_tunnel_egr_intf_used_count_get(unit, oif_idx,
                                &l3_oif_cnt);
            BCM_IF_ERROR_RETURN(rv);

            /* detach tunnel index from egr_l3_intf */
            if (l3_oif_cnt <= 1) {
                rv = _bcm_flow_tunnel_idx_oif_set(unit, oif_idx, 0);
                BCM_IF_ERROR_RETURN(rv);
            }

            flow_info->init_tunnel[soft_tnl_idx].idx = 0;
            return BCM_E_NONE;
        }

        /* set up the vc_entry and dvp_entry */
        if (!(info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_DIP_AS_LAST_SID)) {
            vc_edit_ctrl_id |= 0x8; /* Bit 3 should be set for indicating a END.B6.Insert case */
        }
        rv = _bcm_flow_tunnel_srv6_dvp_vc_swap_init(unit,
                           _BCM_FLOW_SRV6_SRH_TYPE_NON_CANONICAL,
                           sid_list, vc_edit_ctrl_id, &vc_entry, &dvp_entry);
        BCM_IF_ERROR_RETURN(rv);

        /* allocate joined dvp and vc_swap entry */
        rv = _bcm_flow_dvp_vc_swap_entry_index_alloc(unit, &vc_entry,
                        &dvp_entry, TRUE, &dvp_vc_swap_new);
        BCM_IF_ERROR_RETURN(rv);

        /* a new index is allocated. Write to the memory */
        mem = EGR_DVP_ATTRIBUTEm;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap_new,
                               &dvp_entry);
        _BCM_FLOW_CLEANUP(rv);

        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap_new,
                               &vc_entry);
        _BCM_FLOW_CLEANUP(rv);

        if (actions & SRV6_ACTION_REPLACE) {
            if (dvp_vc_swap) {
                rv = _bcm_flow_dvp_vc_swap_entry_index_free(unit,
                              &dvp_entry, TRUE, dvp_vc_swap);
                _BCM_FLOW_CLEANUP(rv);
            }
        }

        mem = EGR_IP_TUNNEL_IPV6m;
        sal_memset(entry, 0, sizeof(entry));
        /* Allocate either existing or new tunnel initiator entry */
        bcm_flow_tunnel_initiator_t_init(&tnl_info);
        tnl_info.flow_handle = flow_info->flow_srv6_non_canonical;
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_DIP_AS_LAST_SID) {
            BCM_IF_ERROR_RETURN(bcm_esw_flow_option_id_get(unit,
                                 flow_info->flow_srv6_non_canonical,
                                 "ASSIGN_SEGMENT_LIST_1",
                                 &tnl_info.flow_option));
        } else {
            BCM_IF_ERROR_RETURN(bcm_esw_flow_option_id_get(unit,
                                 flow_info->flow_srv6_non_canonical,
                                 "ASSIGN_SEGMENT_LIST_2_AND_1_WITHOUT_DIP",
                                 &tnl_info.flow_option));
        }

        rv = soc_flow_db_ffo_to_mem_view_id_get(unit, tnl_info.flow_handle,
                    tnl_info.flow_option,
                   SOC_FLOW_DB_FUNC_TUNNEL_INITIATOR_ID,
                    (uint32 *)&view_id);
        _BCM_FLOW_CLEANUP(rv);

        soc_flow_db_mem_view_entry_init(unit, view_id, entry);

        SAL_IP6_ADDR_TO_UINT32(sid_list[1], ip6);
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID) {
            /* GSID_ENCAP_SID0_95_64 */
            soc_mem_field32_set(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_25_ACTION_SETf, ip6[2]);
            /* GSID_ENCAP_SID0_63_32 */
            soc_mem_field32_set(unit, view_id, entry,
                         DIRECT_ASSIGNMENT_0_ACTION_SETf, ip6[1]);
            /* GSID_ENCAP_SID0_111_96 */
            rv = bcm_esw_flow_logical_field_id_get(unit, tnl_info.flow_handle,
                         "GSID_ENCAP_SID0_111_96", &lfield.id);
            _BCM_FLOW_CLEANUP(rv);
            lfield.value = ip6[3] & 0xffff;
            soc_mem_field32_set(unit, view_id, entry, lfield.id, lfield.value);

            /* GSID_ENCAP_SID0_127_112 */
            rv = bcm_esw_flow_logical_field_id_get(unit, tnl_info.flow_handle,
                         "GSID_ENCAP_SID0_127_112", &lfield.id);
            _BCM_FLOW_CLEANUP(rv);
            lfield.value = ip6[3] >> 16;
            soc_mem_field32_set(unit, view_id, entry, lfield.id, lfield.value);

            /* indicate GSID */
            soc_mem_field32_set(unit, view_id, entry, FLEX_EDIT_CTRL_IDf,
                          tnl_edit_ctrl_id);

            if (!(info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_DIP_AS_LAST_SID)) {
                SAL_IP6_ADDR_TO_UINT32(sid_list[2], ip6);
                /* SID1_31_0 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_15_ACTION_SETf, ip6[0]);
                /* SID1_63_32 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_16_ACTION_SETf, ip6[1]);
                /* SID1_95_64 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_20_ACTION_SETf, ip6[2]);
                /* SID1_127_96 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_21_ACTION_SETf, ip6[3]);
            }
        } else {

            /* SID1_31_0 */
            soc_mem_field32_set(unit, view_id, entry,
                    DIRECT_ASSIGNMENT_15_ACTION_SETf, ip6[0]);
            /* SID1_63_32 */
            soc_mem_field32_set(unit, view_id, entry,
                    DIRECT_ASSIGNMENT_16_ACTION_SETf, ip6[1]);
            /* SID1_95_64 */
            soc_mem_field32_set(unit, view_id, entry,
                    DIRECT_ASSIGNMENT_20_ACTION_SETf, ip6[2]);
            /* SID1_127_96 */
            soc_mem_field32_set(unit, view_id, entry,
                    DIRECT_ASSIGNMENT_21_ACTION_SETf, ip6[3]);

            if (!(info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_DIP_AS_LAST_SID)) {
                SAL_IP6_ADDR_TO_UINT32(sid_list[2], ip6);
                /* SID2_31_0 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_1_ACTION_SETf, ip6[0]);
                /* SID2_63_32 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_2_ACTION_SETf, ip6[1]);
                /* SID2_95_64 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_3_ACTION_SETf, ip6[2]);
                /* SID2_127_96 */
                soc_mem_field32_set(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_24_ACTION_SETf, ip6[3]);
            }
        }

        flags =  _BCM_L3_SHR_WRITE_DISABLE | _BCM_L3_SHR_SKIP_INDEX_ZERO;
        sal_memcpy(tnl_info.dip6, sid_list[1], sizeof(bcm_ip6_t));
        sal_memset(tnl_sid_list[0], 0, sizeof(bcm_ip6_t));
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_DIP_AS_LAST_SID) {
            sal_memset(tnl_sid_list[2], 0, sizeof(bcm_ip6_t));
        }
        tnl_srh.flags = info->flags &
                            (BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID |
                             BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID |
                             BCM_FLOW_TUNNEL_SRV6_SRH_GSID_PREFIX2);
        rv = _bcm_flow_tnl_init_add(unit, flags, &tnl_info, NULL,
                                 0, &tnl_srh, tnl_sid_list, &tnl_idx_new);
        _BCM_FLOW_CLEANUP(rv);

        ipv6m_tnl_idx = tnl_idx_new >> 1;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx, entry);
        _BCM_FLOW_CLEANUP(rv);

        rv = _bcm_flow_tunnel_idx_oif_set(unit, oif_idx, tnl_idx_new);
        _BCM_FLOW_CLEANUP(rv);

        if (actions & SRV6_ACTION_REPLACE) {
            if (tnl_idx) {
                rv = _tnl_init_ip6_del(unit, _BCM_L3_SHR_WRITE_DISABLE, tnl_idx);
                _BCM_FLOW_CLEANUP(rv);
            }
        }

        action_set = 0;
        soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt, &action_set,
                       NEXT_PTR_TYPEf,
                       BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP);
        soc_format_field32_set(unit, NEXT_PTR_ACTION_SETfmt, &action_set,
                       NEXT_PTRf, dvp_vc_swap_new);
        soc_mem_field32_set(unit, nh_view_id, &nh_entry,
                       NEXT_PTR_ACTION_SETf, action_set);

        mem = EGR_L3_NEXT_HOPm;
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, nh_idx, &nh_entry);
        _BCM_FLOW_CLEANUP(rv);

        /* soft_tnl_idx should be egress object passed info */
        soft_tnl_idx = _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit) +
                       info->egr_obj - BCM_XGS3_EGRESS_IDX_MIN(unit);
        flow_info->init_tunnel[soft_tnl_idx].idx = tnl_idx_new;
        BCM_GPORT_TUNNEL_ID_SET(info->tunnel_id,soft_tnl_idx);

    } else {

        /* canonicalL2, must have tunnel_id, get tunnel type and dvp if L2 */
        /*
         * EGR_DVP_ATTRIBUTE and EGR_MPLS_VC_AND_SWAP_LABEL_TABLE share the
         * same index. vc_and_swap can be viewed as extension of the dvp.
         * Allocate the same index of the vc_and_swap entry when flow port
         * is created
         */

        /* L2 tunnel */
        dvp_vc_swap = soft_tnl_idx;

        /* dvp points to both vc_and_swap and egr_dvp_attribute tables */
        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
        sal_memset(entry, 0, sizeof(entry));

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap, entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id);
        BCM_IF_ERROR_RETURN(rv);

        /* indicate number of SIDs */
        soc_mem_field32_set(unit, view_id, entry, EDIT_CTRL_IDf, vc_edit_ctrl_id);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap, entry);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_flow_tunnel_and_oif_index_get(unit, info->tunnel_id,
                   &oif_idx, &tnl_idx);
        BCM_IF_ERROR_RETURN(rv);

        mem = EGR_IP_TUNNEL_IPV6m;
        sal_memset(entry, 0, sizeof(entry));

        ipv6m_tnl_idx = tnl_idx >> 1;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx, entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &view_id);
        BCM_IF_ERROR_RETURN(rv);

        SAL_IP6_ADDR_TO_UINT32(sid_list[0], ip6);
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID) {
            /* GSID_ENCAP_SID0_63_32 */
            soc_mem_field32_set(unit, view_id, entry,
                           DIRECT_ASSIGNMENT_0_ACTION_SETf, ip6[1]);

            /* GSID_ENCAP_SID0_95_64 */
            soc_mem_field32_set(unit, view_id, entry,
                           DIRECT_ASSIGNMENT_23_ACTION_SETf, ip6[2]);

            /* XXX could use OPAQUE_1(2)_ACTION_SET in cfh */
            /* GSID_ENCAP_SID0_111_96 */
            rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                         "GSID_ENCAP_SID0_111_96", &lfield.id);
            BCM_IF_ERROR_RETURN(rv);
            lfield.value = ip6[3] & 0xffff;
            soc_mem_field32_set(unit,view_id, entry,
                                  lfield.id, lfield.value);

            /* GSID_ENCAP_SID0_127_112 */
            rv = bcm_esw_flow_logical_field_id_get(unit, flow_info->flow_srv6,
                         "GSID_ENCAP_SID0_127_112", &lfield.id);
            BCM_IF_ERROR_RETURN(rv);
            lfield.value = ip6[3] >> 16;
            soc_mem_field32_set(unit,view_id, entry,
                                  lfield.id, lfield.value);
        }

        SAL_IP6_ADDR_TO_UINT32(sid_list[1], ip6);

        /* SID1_31_0 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_20_ACTION_SETf, ip6[0]);
        /* SID1_63_32 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_21_ACTION_SETf, ip6[1]);
        /* SID1_95_64 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_22_ACTION_SETf, ip6[2]);
        /* SID1_127_96 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_25_ACTION_SETf, ip6[3]);

        SAL_IP6_ADDR_TO_UINT32(sid_list[2], ip6);
        /* SID2_31_0 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_1_ACTION_SETf, ip6[0]);
        /* SID2_63_32 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_2_ACTION_SETf, ip6[1]);
        /* SID2_95_64 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_3_ACTION_SETf, ip6[2]);
        /* SID2_127_96 */
        soc_mem_field32_set(unit, view_id, entry,
                       DIRECT_ASSIGNMENT_24_ACTION_SETf, ip6[3]);

        /* SRH base header */
        action_set = info->tag | ((info->next_hdr & 0xff) << 24);
        soc_mem_field32_set(unit, view_id, entry,
                  DIRECT_ASSIGNMENT_9_ACTION_SETf, action_set);

        /* indicate standard sid, gsid or usid */
        soc_mem_field32_set(unit, view_id, entry,
                     FLEX_EDIT_CTRL_IDf, tnl_edit_ctrl_id);

        tnl_info.type = bcmTunnelTypeEthSR6;
        rv = _bcm_flow_hw_tunnel_entry_get(unit, tnl_idx, &tnl_info, NULL,
                                   0, NULL);
        BCM_IF_ERROR_RETURN(rv);
        rv = bcm_esw_flow_option_id_get(unit, flow_info->flow_srv6,
                     "L2_ASSIGN_SIP_SRH_BASE_SEGMENT_LIST_2_AND_1",
                     &tnl_info.flow_option);
        BCM_IF_ERROR_RETURN(rv);
        tnl_info.flow_handle = flow_info->flow_srv6;

        tnl_srh.tag = info->tag;
        tnl_srh.next_hdr = info->next_hdr;

        tnl_info.valid_elements |= BCM_FLOW_TUNNEL_INIT_SIP6_VALID;
        flags =  _BCM_L3_SHR_WRITE_DISABLE | _BCM_L3_SHR_SKIP_INDEX_ZERO;
        tnl_srh.flags = info->flags &
                            (BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID |
                             BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID |
                             BCM_FLOW_TUNNEL_SRV6_SRH_GSID_PREFIX2);
        rv = _bcm_flow_tnl_init_add(unit, flags, &tnl_info, NULL,
                                 0, &tnl_srh, tnl_sid_list, &tnl_idx_new);
        BCM_IF_ERROR_RETURN(rv);

        if (tnl_idx != tnl_idx_new) {
            /* need a new EGR_IP_TUNNEL entry */
            ipv6m_tnl_idx = tnl_idx_new >> 1;
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx,
                                   entry);
            _BCM_FLOW_CLEANUP(rv);

            /* delete the old tunnel  */
            rv = _tnl_init_ip6_del(unit, _BCM_L3_SHR_WRITE_DISABLE,
                                       tnl_idx);
            _BCM_FLOW_CLEANUP(rv);

            rv = _bcm_flow_tunnel_idx_oif_set(unit, oif_idx, tnl_idx_new);
            _BCM_FLOW_CLEANUP(rv);
        } else {
            /* decrease the reference count  */
            rv = _tnl_init_ip6_del(unit, _BCM_L3_SHR_WRITE_DISABLE,
                                       tnl_idx);
            _BCM_FLOW_CLEANUP(rv);
        }
    }
    return BCM_E_NONE;

cleanup:
    if (tnl_idx_new) {
        (void)_tnl_init_ip6_del(unit, _BCM_L3_SHR_WRITE_DISABLE, tnl_idx_new);
    }
    if (dvp_vc_swap_new) {
        (void)_bcm_flow_dvp_vc_swap_entry_index_free(unit, &dvp_entry,
                          TRUE, dvp_vc_swap_new);
    }
    if (nh2_idx_new) {
        (void)_bcm_flow_srv6_nh_del(unit, 0, nh2_idx_new);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_srv6_srh_add
 * Purpose:
 *     Add SRH frame to the given L2/L3 packet. For the canonical encapsulation,
 * A L2 or L3 tunnel must be created first with bcm_flow_tunnel_initiator_create
 * API and the tunnel_id is passed to this API. For non-canonical encapsulation,
 * no L2 or L3 tunnel is created. A SRH frame is added to the packet after the
 * IPv6 header. In this case, the egress object is used to identify the packet.
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      options - (IN) operation options
 *      info    - (IN/OUT) SRH parameter descriptor
 *      nofs    - (IN) number of SIDs
 *      sid_list - (IN) SID list. Size indicated by above nofs parameter
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_tunnel_srv6_srh_add(int unit,
       uint32 options,
       bcm_flow_tunnel_srv6_srh_t *info,
       int nofs,
       bcm_ip6_t *sid_list)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_srv6_srh_t linfo;
    int soft_tnl_idx;
    uint32 actions;
    uint32 mpath_flag=0;
    int nh_idx = -1;

    rv = bcmi_esw_flow_lock(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* validate the parameters */
    rv = _bcm_esw_flow_tnl_srv6_srh_validate(unit, options, info, nofs,
                  sid_list);
    _BCM_FLOW_CLEANUP(rv);

    linfo = *info;
    actions = 0;
    if (options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID) {
        rv = _tnl_srv6_id_validate(unit, info->tunnel_id, NULL);
        _BCM_FLOW_CLEANUP(rv);
        actions = SRV6_ACTION_WITH_ID;
    }
    if (options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_REPLACE) {
        actions |= SRV6_ACTION_REPLACE;
    }

    /* for non-canonical */
    if (!(options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID)) {
        soft_tnl_idx = info->egr_obj - BCM_XGS3_EGRESS_IDX_MIN(unit) +
                       _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        BCM_GPORT_TUNNEL_ID_SET(linfo.tunnel_id, soft_tnl_idx);
    }
    BCM_XGS3_L3_MODULE_LOCK(unit);
    rv = _bcm_esw_flow_tunnel_srv6_srh_set(unit, actions, &linfo, nofs,
                 sid_list);
    BCM_XGS3_L3_MODULE_UNLOCK(unit);

    _BCM_FLOW_CLEANUP(rv);

    /* for non-canonical tunnel creation, return tunnel_id */
    if (!(options & BCM_FLOW_TUNNEL_SRV6_SRH_OPTION_WITH_ID)) {
        info->tunnel_id = linfo.tunnel_id;

        /* increment the egr_obj reference count */
        BCM_XGS3_L3_MODULE_LOCK(unit);
        rv = bcm_xgs3_get_nh_from_egress_object(unit, info->egr_obj,
                             &mpath_flag, 1, &nh_idx);
        BCM_XGS3_L3_MODULE_UNLOCK(unit);
        _BCM_FLOW_CLEANUP(rv);
    }

cleanup:
    bcmi_esw_flow_unlock (unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_srh_attr_get
 * Purpose:
 *      Retrieve API's SRH operation attributes: reduced mode, B6 insert mode
 *   and number of SID segments based on vc_and_swap entry's edit_ctrl_id.
 * Parameters:
 *      unit       - (IN) SOC unit number.
 *      attr_value - (IN) vc_and_swap entry's edit_ctrl_id.
 *      flags      - (OUT) SRH attribute flags: reduced mode and insert mode.
 *      num_segments - (OUT) number of SID segments.
 * Returns:
 *      BCM_E_XXX
 * NOTE:
 */

STATIC int
_bcm_esw_flow_srh_attr_get(int unit,
    int attr_value,
    int *flags,
    int *num_segments)
{
    int ix;
    soc_cancun_ceh_field_info_t ceh_info;
    int rv;
    for (ix = 0; ix < COUNTOF(vc_ctrl_map); ix++) {
        rv = soc_cancun_ceh_obj_field_get(unit,
                 "EGR_MPLS_VC_AND_SWAP_EDIT_CTRL_ID",
                 vc_ctrl_map[ix].name, &ceh_info);
        BCM_IF_ERROR_RETURN(rv);
        if (attr_value == ceh_info.value) {
            *num_segments = vc_ctrl_map[ix].num_seg;
            *flags = vc_ctrl_map[ix].flag;
            break;
        }
    }
    if (ix == COUNTOF(vc_ctrl_map)) {
        /* not found */
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_esw_flow_srh_edit_ctrl_id_get(int unit,
    int flags,
    int num_segments,
    int *vc_edit_ctrl_id,
    int *tnl_edit_ctrl_id)
{
    int ix;
    soc_cancun_ceh_field_info_t ceh_info;
    int rv;
    uint32 flag_mask;

    flag_mask = BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE |
                BCM_FLOW_TUNNEL_SRV6_SRH_INSERT;

    for (ix = 0; ix < COUNTOF(vc_ctrl_map); ix++) {
        rv = soc_cancun_ceh_obj_field_get(unit,
                 "EGR_MPLS_VC_AND_SWAP_EDIT_CTRL_ID",
                 vc_ctrl_map[ix].name, &ceh_info);
        BCM_IF_ERROR_RETURN(rv);
        if (num_segments == vc_ctrl_map[ix].num_seg &&
            ((flags & flag_mask) == vc_ctrl_map[ix].flag)) {
            *vc_edit_ctrl_id = ceh_info.value;
            break;
        }
    }
    if (ix == COUNTOF(vc_ctrl_map)) {
        /* not found */
        if (!num_segments) {
            *vc_edit_ctrl_id = 0;
        } else {
            return BCM_E_NOT_FOUND;
        }
    }

    flag_mask = BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_GSID |
                BCM_FLOW_TUNNEL_SRV6_SRH_GSID_PREFIX2 |
                BCM_FLOW_TUNNEL_SRV6_SRH_TYPE_STD_SID;

    for (ix = 0; ix < COUNTOF(tnl_ctrl_map); ix++) {
        rv = soc_cancun_ceh_obj_field_get(unit,
                 "EGR_IP_TUNNEL_IPV6_FLEX_EDIT_CTRL_ID",
                 tnl_ctrl_map[ix].name, &ceh_info);
        BCM_IF_ERROR_RETURN(rv);
        if ((flags & flag_mask) == tnl_ctrl_map[ix].flag) {
            *tnl_edit_ctrl_id = ceh_info.value;
            break;
        }
    }
    if (ix == COUNTOF(tnl_ctrl_map)) {
        /* not found */
        return BCM_E_NOT_FOUND;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_esw_flow_tunnel_srv6_srh_get
 * Purpose:
 *     Implement SRH get function.
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) SRH parameter descriptor
 *      max_nofs - (IN) size of the sid_list buffer.
 *      sid_list - (OUT) SID list buffer for retrieved SIDs.
 *      act_nofs - (OUT) actual number of SIDs.
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_esw_flow_tunnel_srv6_srh_get(int unit,
      bcm_flow_tunnel_srv6_srh_t *info,
      int max_nofs,
      bcm_ip6_t *sid_list,
      int *act_nofs)
{
    int soft_tnl_idx;
    uint32 entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    uint32 nh_entry[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry */
    soc_mem_t mem;                        /* Tunnel initiator table memory */
    uint32 view_id;
    uint32 ip6[IP6_U32_ARR_LEN];
    int oif_idx;
    int tnl_idx;
    int flags;
    int num_segments;
    int l2_tunnel;
    int edit_ctrl_id;
    bcm_flow_tunnel_srv6_srh_t tnl_srh;
    bcm_ip6_t tnl_sid_list[TNL_SRV6_SID_MAX];
    int rv;
    int dvp_vc_swap;
    int next_ptr;
    int next_ptr_type;
    uint32 action_set;
    uint32 nh_view_id;
    int nh_idx;
    int i;
    int data_type;
    int tnl_data_type;
    _bcm_flow_bookkeeping_t *flow_info;
    int ipv6m_tnl_idx;

    flow_info = FLOW_INFO(unit);
    soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(info->tunnel_id);
    _BCM_FLOW_TUNNEL_ID_IF_INVALID_RETURN(unit, soft_tnl_idx);

    if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: tunnel_id has not been created\n")));
        return BCM_E_PARAM;
    }

    if (max_nofs < 0 || max_nofs > TNL_SRV6_SID_MAX) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: Maximum SID is 3\n")));
        return BCM_E_PARAM;
    }

    rv = _tnl_srv6_id_validate(unit, info->tunnel_id, NULL);
    BCM_IF_ERROR_RETURN(rv);

    l2_tunnel = 0;
    if (soft_tnl_idx < _BCM_FLOW_TUNNEL_L2_MAX(unit)) {
        l2_tunnel = TRUE;
    }

    for (i = 0; i < IP6_U32_ARR_LEN; i++) {
        ip6[i] = 0;
    }
    rv = _bcm_flow_tunnel_and_oif_index_get(unit, info->tunnel_id,
               &oif_idx, &tnl_idx);
    BCM_IF_ERROR_RETURN(rv);

    info->flags = 0;
    for (i = 0; i < max_nofs; i++) {
        sal_memset(sid_list[i], 0, sizeof(bcm_ip6_t));
    }

    if (l2_tunnel) {
        info->flags = BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL;
        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, soft_tnl_idx, entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id);
        BCM_IF_ERROR_RETURN(rv);
        edit_ctrl_id = soc_mem_field32_get(unit, view_id, entry,
                           EDIT_CTRL_IDf);

        BCM_IF_ERROR_RETURN(_bcm_esw_flow_srh_attr_get(unit, edit_ctrl_id,
                 &flags, &num_segments));
        info->flags |= flags;

        /* mem = EGR_IP_TUNNEL_IPV6m; */
        mem = BCM_XGS3_L3_MEM(unit, tnl_init_v6);
        ipv6m_tnl_idx = tnl_idx >> 1;
        rv = BCM_XGS3_MEM_READ(unit, mem, ipv6m_tnl_idx, entry);
        SOC_IF_ERROR_RETURN(rv);

        tnl_data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);

        /* find the memory view based on the memory and key type */
        rv = soc_flow_db_mem_to_view_id_get (unit, mem,
                    SOC_FLOW_DB_KEY_TYPE_INVALID, tnl_data_type,
                    0, NULL, (uint32 *)&view_id);
        BCM_IF_ERROR_RETURN(rv);

        rv = _bcm_flow_hw_srv6_srh_entry_get(unit, entry,
                          view_id, &tnl_srh, tnl_sid_list);
        BCM_IF_ERROR_RETURN(rv);
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE) {
            for (i = 0; i < max_nofs && i < (TNL_SRV6_SID_MAX - 1); i++) {
                sal_memcpy(sid_list[i], tnl_sid_list[i + 1], sizeof(bcm_ip6_t));
            }
        } else {
            for (i = 0; i < max_nofs; i++) {
                sal_memcpy(sid_list[i], tnl_sid_list[i], sizeof(bcm_ip6_t));
            }
        }
        info->tag = tnl_srh.tag;
        info->next_hdr = tnl_srh.next_hdr;
        info->flags |= tnl_srh.flags;
    } else {
        /* L3 tunnel */
        nh_idx = soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit);
        mem = EGR_L3_NEXT_HOPm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, nh_idx, &nh_entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, &nh_entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &nh_view_id);
        BCM_IF_ERROR_RETURN(rv);

        action_set = soc_mem_field32_get(unit, nh_view_id, &nh_entry,
                        DIRECT_ASSIGNMENT_9_ACTION_SETf);
        info->tag = action_set & 0xffff;
        info->flags |= (action_set >> 16) & 0xff;

        action_set = soc_mem_field32_get(unit, nh_view_id, &nh_entry,
                        NEXT_PTR_ACTION_SETf);

        next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
        next_ptr = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTRf);
        if (next_ptr_type == BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_NH) {
            /* L3 and canonical */
            info->flags = BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL;
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, next_ptr, entry);
            BCM_IF_ERROR_RETURN(rv);

            data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
            rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &view_id);
            BCM_IF_ERROR_RETURN(rv);
            if (max_nofs >= 2) {
                /* SID1_31_0 */
                ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_20_ACTION_SETf);
                /* SID1_63_32 */
                ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_21_ACTION_SETf);
                /* SID1_95_64 */
                ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                        DIRECT_ASSIGNMENT_22_ACTION_SETf);
            }
            action_set = soc_mem_field32_get(unit, view_id, entry,
                               NEXT_PTR_ACTION_SETf);
            next_ptr_type = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                               &action_set, NEXT_PTR_TYPEf);
            if (next_ptr_type !=
                        BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP) {
                return BCM_E_INTERNAL;
            }
            dvp_vc_swap = soc_format_field32_get(unit, NEXT_PTR_ACTION_SETfmt,
                                   &action_set, NEXT_PTRf);


        } else if (next_ptr_type ==
                          BCMI_TD3_L3_NH_EGR_NEXT_PTR_TYPE_DVP_AND_VC_SWAP) {
            /* non-canonical */
            dvp_vc_swap = next_ptr;
        } else {
            return BCM_E_INTERNAL;
        }
        mem = EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap, entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &view_id);
        BCM_IF_ERROR_RETURN(rv);
        edit_ctrl_id = soc_mem_field32_get(unit, view_id, entry,
                               EDIT_CTRL_IDf);
        BCM_IF_ERROR_RETURN(_bcm_esw_flow_srh_attr_get(unit, edit_ctrl_id,
                 &flags, &num_segments));
        info->flags |= flags;
        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL) {
            if (max_nofs >= 2) {
                /* SID1_127_96 */
                ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                             DIRECT_ASSIGNMENT_25_ACTION_SETf);
            }

        } else {
            mem = EGR_DVP_ATTRIBUTEm;
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, dvp_vc_swap, entry);
            BCM_IF_ERROR_RETURN(rv);

            data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
            rv = soc_flow_db_mem_to_view_id_get(unit,
                                 mem,
                                 SOC_FLOW_DB_KEY_TYPE_INVALID,
                                 data_type,
                                 0,
                                 NULL,
                                 &view_id);
            BCM_IF_ERROR_RETURN(rv);
            /* SID0_31_0 */
            ip6[0] = soc_mem_field32_get(unit, view_id, entry,
                   DIRECT_ASSIGNMENT_11_ACTION_SETf);
            /* SID0_63_32 */
            ip6[1] = soc_mem_field32_get(unit, view_id, entry,
                   DIRECT_ASSIGNMENT_12_ACTION_SETf);
            /* SID0_95_64 */
            ip6[2] = soc_mem_field32_get(unit, view_id, entry,
                   DIRECT_ASSIGNMENT_13_ACTION_SETf);
            /* SID0_127_96 */
            ip6[3] = soc_mem_field32_get(unit, view_id, entry,
                   DIRECT_ASSIGNMENT_14_ACTION_SETf);
        }

        mem = EGR_IP_TUNNEL_IPV6m;
        sal_memset(entry, 0, sizeof(entry));

        ipv6m_tnl_idx = tnl_idx >> 1;
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, ipv6m_tnl_idx, entry);
        BCM_IF_ERROR_RETURN(rv);

        data_type = soc_mem_field32_get(unit, mem, entry, DATA_TYPEf);
        rv = soc_flow_db_mem_to_view_id_get(unit,
                             mem,
                             SOC_FLOW_DB_KEY_TYPE_INVALID,
                             data_type,
                             0,
                             NULL,
                             &view_id);
        BCM_IF_ERROR_RETURN(rv);

        if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL) {
            rv = _bcm_flow_hw_srv6_srh_entry_get(unit, entry,
                          view_id, &tnl_srh, tnl_sid_list);
            BCM_IF_ERROR_RETURN(rv);
            info->flags |= tnl_srh.flags;
            SAL_IP6_ADDR_FROM_UINT32(tnl_sid_list[1],ip6);
            if (info->flags & BCM_FLOW_TUNNEL_SRV6_SRH_REDUCED_MODE) {
                for (i = 0; i < max_nofs && i < (TNL_SRV6_SID_MAX - 1); i++) {
                    sal_memcpy(sid_list[i], tnl_sid_list[i + 1],
                               sizeof(bcm_ip6_t));
                }
            } else {
                for (i = 0; i < max_nofs; i++) {
                    sal_memcpy(sid_list[i], tnl_sid_list[i], sizeof(bcm_ip6_t));
                }
            }
        } else {   /* non-canonical */
            /* indicate standard sid, gsid or usid */
            rv = _bcm_flow_hw_srv6_srh_entry_get(unit, entry,
                          view_id, &tnl_srh, tnl_sid_list);
            BCM_IF_ERROR_RETURN(rv);
            info->flags |= tnl_srh.flags;
            SAL_IP6_ADDR_FROM_UINT32(tnl_sid_list[0], ip6);
            for (i = 0; i < max_nofs; i++) {
                sal_memcpy(sid_list[i], tnl_sid_list[i], sizeof(bcm_ip6_t));
            }
        }
    }
    *act_nofs = num_segments;
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_srv6_srh_get
 * Purpose:
 *     Retrive the SRH configuration from the hardware.
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      info    - (IN/OUT) SRH parameter descriptor
 *      max_nofs - (IN) size of the sid_list buffer.
 *      sid_list - (OUT) SID list buffer for retrieved SIDs.
 *      act_nofs - (OUT) actual number of SIDs.
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_tunnel_srv6_srh_get(int unit,
      bcm_flow_tunnel_srv6_srh_t *info,
      int max_nofs,
      bcm_ip6_t *sid_list,
      int *act_nofs)
{
    int rv = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_srv6_enable)) {
        return BCM_E_UNAVAIL;
    }
    /* parameter check */
    if (info == NULL || (sid_list == NULL && max_nofs != 0) ||
        act_nofs == NULL) {
        return BCM_E_PARAM;
    }

    rv = bcmi_esw_flow_lock(unit);
    BCM_IF_ERROR_RETURN(rv);
    rv = _bcm_esw_flow_tunnel_srv6_srh_get(unit, info, max_nofs, sid_list, act_nofs);
    bcmi_esw_flow_unlock (unit);
    BCM_IF_ERROR_RETURN(rv);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_srv6_srh_delete
 * Purpose:
 *     Delete a SRH entry for the given tunnel_id.
 *
 * Parameters:
 *      unit      - (IN) Device Number
 *      tunnel_id - (IN) tunnel_id associated with the SRH entry.
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_tunnel_srv6_srh_delete(
    int unit,
    bcm_gport_t tunnel_id)
{
    int soft_tnl_idx;
    int rv;
    bcm_ip6_t sid_list[TNL_SRV6_SID_MAX];
    bcm_flow_tunnel_srv6_srh_t srh_info;
    uint32 actions = 0;
    int i;
    _bcm_flow_bookkeeping_t *flow_info;
    int ref_count;

    if (!soc_feature(unit, soc_feature_srv6_enable)) {
        return BCM_E_UNAVAIL;
    }

    flow_info = FLOW_INFO(unit);

    rv = bcmi_esw_flow_lock(unit);
    BCM_IF_ERROR_RETURN(rv);

    /* validate parameter */
    soft_tnl_idx = BCM_GPORT_TUNNEL_ID_GET(tunnel_id);
    if (soft_tnl_idx < 0 ||
        soft_tnl_idx >= _BCM_FLOW_TUNNEL_ID_MAX(unit)) {
        rv = BCM_E_BADID;
        goto cleanup;
    }
    if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
        LOG_ERROR(BSL_LS_BCM_FLOW, (BSL_META_U(unit,
                   "srv6: tunnel_id has not been created\n")));
        rv = BCM_E_NOT_FOUND;
        goto cleanup;
    }

    sal_memset(&srh_info, 0, sizeof(srh_info));
    rv = _tnl_srv6_id_validate(unit, tunnel_id, &srh_info.flags);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for (i = 0; i < TNL_SRV6_SID_MAX; i++) {
        sal_memset(sid_list[i], 0, sizeof(bcm_ip6_t));
    }

    actions = SRV6_ACTION_DELETE | SRV6_ACTION_WITH_ID;
    srh_info.tunnel_id = tunnel_id;

    BCM_XGS3_L3_MODULE_LOCK(unit);
    rv = _bcm_esw_flow_tunnel_srv6_srh_set(unit, actions, &srh_info,
                      TNL_SRV6_SID_MAX,
                      sid_list);
    if (BCM_SUCCESS(rv)) {
        /* decrement the egr_obj refcnt */
        if (!(srh_info.flags & BCM_FLOW_TUNNEL_SRV6_SRH_CANONICAL)) {
            rv = bcm_xgs3_get_ref_count_from_nhi(unit, 0, &ref_count,
                  soft_tnl_idx - _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit));
        }
    }
    BCM_XGS3_L3_MODULE_UNLOCK(unit);

cleanup:
    bcmi_esw_flow_unlock (unit);
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_srv6_srh_traverse
 * Purpose:
 *     Traverse the SRH entries and invoke the user specified callback
 *  function on each entry.
 *
 * Parameters:
 *      unit    - (IN) Device Number
 *      cb      - (IN) User specified callback function
 *      user_data - (IN) User provided parameter. Passed into callback.
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_tunnel_srv6_srh_traverse(int unit,
    bcm_flow_tunnel_srv6_srh_traverse_cb cb,
    void *user_data)
{
    bcm_flow_tunnel_srv6_srh_t info;
    bcm_ip6_t sid_list[TNL_SRV6_SID_MAX];
    _bcm_flow_bookkeeping_t *flow_info;
    int soft_tnl_idx;
    int rv = BCM_E_NONE;
    int cnt = 0;

    if (!soc_feature(unit, soc_feature_srv6_enable)) {
        return BCM_E_UNAVAIL;
    }

    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    flow_info = FLOW_INFO(unit);

    for (soft_tnl_idx = 0; soft_tnl_idx < _BCM_FLOW_TUNNEL_ID_MAX(unit);
                           soft_tnl_idx++) {
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            continue;
        }
        /* check if SRv6 tunnel */
        if (soft_tnl_idx >= _BCM_FLOW_TUNNEL_L2_MAX(unit) &&
            soft_tnl_idx < _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
            continue;
        }
        bcm_flow_tunnel_srv6_srh_t_init(&info);
        BCM_GPORT_TUNNEL_ID_SET(info.tunnel_id, soft_tnl_idx);

        rv = bcmi_esw_flow_tunnel_srv6_srh_get(unit, &info, 3,
                  sid_list, &cnt);
        if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */
            continue;
        } else if (BCM_FAILURE(rv)) {
            break;
        }
        rv = cb(unit, &info, cnt, sid_list, user_data);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_tunnel_srv6_srh_delete_all
 * Purpose:
 *     Delete all SRH entries
 *
 * Parameters:
 *      unit    - (IN) Device Number
 * Returns:
 *      BCM_E_XXX
 */

int bcmi_esw_flow_tunnel_srv6_srh_delete_all(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;
    int soft_tnl_idx;
    int rv = BCM_E_NONE;
    bcm_gport_t tunnel_id;

    if (!soc_feature(unit, soc_feature_srv6_enable)) {
        return BCM_E_UNAVAIL;
    }

    flow_info = FLOW_INFO(unit);
    for (soft_tnl_idx = 0; soft_tnl_idx < _BCM_FLOW_TUNNEL_ID_MAX(unit);
                           soft_tnl_idx++) {

        /* check if SRv6 tunnel */
        if (!flow_info->init_tunnel[soft_tnl_idx].idx) {
            continue;
        }
        if (soft_tnl_idx >= _BCM_FLOW_TUNNEL_L2_MAX(unit) &&
            soft_tnl_idx < _BCM_FLOW_TUNNEL_EGR_OBJ_TYPE_ID_OFFSET(unit)) {
            continue;
        }

        BCM_GPORT_TUNNEL_ID_SET(tunnel_id, soft_tnl_idx);
        rv = bcmi_esw_flow_tunnel_srv6_srh_delete(unit, tunnel_id);
        if (rv == BCM_E_NOT_FOUND) { /* for entries not exposed to user */
            rv = BCM_E_NONE;
            continue;
        } else if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return rv;
}

#endif  /* INCLUDE_L3 */
