/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mpls.c
 * Purpose: Manages Tomahawk3 MPLS functions
 */
#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TOMAHAWK3_SUPPORT) && defined(BCM_MPLS_SUPPORT) && \
    defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/hash.h>
#include <soc/util.h>
#include <soc/triumph.h>

#include <bcm/error.h>
#include <bcm/mpls.h>


#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/port.h>
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/ecn.h>
#endif
#include <bcm_int/esw/vlan.h>
#ifdef BCM_TRIUMPH2_SUPPORT
#include <bcm_int/esw/triumph2.h>
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT*/
#ifdef BCM_KATANA2_SUPPORT
#include <bcm_int/esw/katana2.h>
#include <soc/katana2.h>
#endif
#include <bcm_int/esw/xgs5.h>
#if defined(BCM_SABER2_SUPPORT)
#include <soc/saber2.h>
#endif /* BCM_SABER2_SUPPORT */

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/esw/failover.h>

extern uint32 nh_entry_type_field;
#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#endif
#define ECN_MAP_ID_INVALID 0xffffffff

/*
 * Function:
 *        bcm_th3_mpls_l3_nh_info_get
 * Purpose:
 *        Get Tunnel_switch info for a specific EGR_NH Index
 * Parameters:
 *           IN :  Unit
 *           OUT : Tunnel switch object pointer
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_th3_mpls_l3_nh_info_get(int unit, bcm_mpls_tunnel_switch_t *info, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    int ix;
    int ecn_map_index;
    int ecn_map_pri;
    info->egress_if = nh_index + BCM_XGS3_EGRESS_IDX_MIN(unit);

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    if (soc_feature(unit, soc_feature_mpls_nh_ttl_control)) {
        if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MPLS__DISABLE_TTL_DECREMENTf)) {
            info->flags |= BCM_MPLS_SWITCH_KEEP_TTL;
        }
    }

    if (_BCM_MPLS_ACTION_PRESERVED == soc_mem_field32_get(unit,
        EGR_L3_NEXT_HOPm, &egr_nh, MPLS__MPLS_LABEL_ACTIONf)) {
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRESERVE;
        info->egress_label.label = BCM_MPLS_LABEL_INVALID;
    } else {
        info->egress_label.label = 
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__MPLS_LABELf);
    }

    info->egress_label.ttl = 
        soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, MPLS__MPLS_TTLf);
    if (info->egress_label.ttl) {
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    } else {
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    }
    if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                            &egr_nh, MPLS__MPLS_EXP_SELECTf) == 0x0) {
        /* Use the specified EXP, PRI and CFI */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
        info->egress_label.exp = 
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__MPLS_EXPf);
        info->egress_label.pkt_pri = 
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__NEW_PRIf);
        info->egress_label.pkt_cfi = 
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__NEW_CFIf);
    } else if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, MPLS__MPLS_EXP_SELECTf) == 0x1) {
        /* Use EXP-map for EXP, PRI and CFI */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        ix = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                 &egr_nh, MPLS__MPLS_EXP_MAPPING_PTRf);
                BCM_IF_ERROR_RETURN(
                    _egr_qos_hw_idx2id(unit, ix, &info->egress_label.qos_map_id));
    } else if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                   &egr_nh, MPLS__MPLS_EXP_SELECTf) == 0x3) {
        /* Use EXP from incoming MPLS label. If no incoming label, use the specified
         * EXP value. Use EXP-map for PRI/CFI.
         */
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        info->egress_label.flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
        info->egress_label.exp = 
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__MPLS_EXPf);
        ix = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                 &egr_nh, MPLS__MPLS_EXP_MAPPING_PTRf);
        BCM_IF_ERROR_RETURN(
                    _egr_qos_hw_idx2id(unit, ix, &info->egress_label.qos_map_id));
    }

    if (soc_feature(unit, soc_feature_mpls_ecn) &&
        soc_feature(unit, soc_feature_ecn_wred)) {
        ecn_map_index = soc_mem_field32_get(unit, 
                                            EGR_L3_NEXT_HOPm,
                                            &egr_nh, 
                                            MPLS__INT_CN_TO_EXP_MAPPING_PTRf);
        if (bcmi_xgs5_ecn_map_used_get(unit, ecn_map_index, 
                                       _bcmEcnmapTypeIntcn2Exp)) {
            info->egress_label.int_cn_map_id = 
                _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP | ecn_map_index;
            ecn_map_pri = soc_mem_field32_get(unit, 
                                              EGR_L3_NEXT_HOPm,
                                              &egr_nh, 
                                              MPLS__INT_CN_TO_EXP_PRIORITYf);
            if (ecn_map_pri) {
                info->egress_label.flags |= 
                    BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST;
            }
            info->egress_label.flags |= 
                BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_mpls_egr_nh_label_set 
 * Purpose:
 *       Set  EGR_NH entry
 * Parameters:
 *           IN :  Unit
 *           IN : egr_obj
 * Returns:
 *            BCM_E_XXX
 */
int
_bcm_th3_mpls_egr_nh_label_set (int unit, 
                                       bcm_mpls_tunnel_switch_t *info,
                                       bcm_l3_egress_t *egr_obj,
                                       int label_action,
                                       int hw_map_idx,
                                       egr_l3_next_hop_entry_t *pegr_nh)
{
    bcm_mpls_label_t mpls_label = BCM_MPLS_LABEL_INVALID;
    uint8 mpls_ttl = 0;
    uint32 mpls_flags = 0;
    uint8 mpls_exp = 0; 
    uint8 mpls_pkt_pri = 0; 
    uint8 mpls_pkt_cfi = 0; 
    int   mpls_ecn_map_id = ECN_MAP_ID_INVALID;
    int   mpls_int_cn_map_id = ECN_MAP_ID_INVALID;

    if (info != NULL) {
         mpls_label = info->egress_label.label;
         mpls_ttl = info->egress_label.ttl;
         mpls_flags = info->egress_label.flags;
         mpls_pkt_pri = info->egress_label.pkt_pri;
         mpls_pkt_cfi = info->egress_label.pkt_cfi;
         mpls_exp = info->egress_label.exp;
         if (soc_feature(unit, soc_feature_mpls_ecn) && 
             (mpls_flags & BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP)) {
             mpls_int_cn_map_id = info->egress_label.int_cn_map_id;
         }
    } else if (egr_obj != NULL) {
         mpls_label = egr_obj->mpls_label;
         mpls_ttl = egr_obj->mpls_ttl;
         mpls_flags = egr_obj->mpls_flags;
         mpls_pkt_pri = egr_obj->mpls_pkt_pri;
         mpls_pkt_cfi = egr_obj->mpls_pkt_cfi;
         mpls_exp = egr_obj->mpls_exp;
         if (soc_feature(unit, soc_feature_mpls_ecn)) {
            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP) {
                mpls_int_cn_map_id = egr_obj->mpls_ecn_map_id;
            }
            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP) {
                mpls_ecn_map_id = egr_obj->mpls_ecn_map_id;
            }
         }
    }

    if ((mpls_exp > 7) || (mpls_pkt_pri > 7) ||
        (mpls_pkt_cfi > 1)) {
        return BCM_E_PARAM;
    }

    if (_BCM_MPLS_EGRESS_LABEL_PRESERVE(unit, mpls_flags))  {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_LABEL_ACTIONf, 
                            _BCM_MPLS_ACTION_PRESERVED);
    } else {
        if (BCM_XGS3_L3_MPLS_LBL_VALID(mpls_label)) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                pegr_nh, MPLS__MPLS_LABELf,
                                mpls_label);
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                    pegr_nh, MPLS__MPLS_LABEL_ACTIONf,
                    label_action);

        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                pegr_nh, MPLS__MPLS_LABEL_ACTIONf,
                                _BCM_MPLS_ACTION_NOOP);
        }
    }
    if (mpls_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET) {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, 
                            pegr_nh, MPLS__MPLS_TTLf,
                            mpls_ttl);
    } else {
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, 
                            pegr_nh, MPLS__MPLS_TTLf, 0x0);
    }
    if ((mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
        (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {

        if ((mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) ||
            (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ||
            (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_REMARK)) {
            return  BCM_E_PARAM;
        }
        /* Use the specified EXP, PRI and CFI */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_SELECTf,
                            0x0); /* USE_FIXED */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXPf,
                            mpls_exp);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__NEW_PRIf,
                            mpls_pkt_pri);
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__NEW_CFIf,
                            mpls_pkt_cfi);
    } else if (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
        if (mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET) {
            return BCM_E_PARAM;
        }
        /* Use EXP-map for EXP, PRI and CFI */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_SELECTf,
                            0x1); /* USE_MAPPING */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_MAPPING_PTRf,
                            hw_map_idx);
    } else if ((label_action != _BCM_MPLS_ACTION_SWAP) && 
                    (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ) { 
        /* Use EXP from incoming label. If there is no incoming label,
         * use the specified EXP value. Use EXP-map for PRI/CFI.
         */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_SELECTf,
                            0x2); /* USE_INNER */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, 
                            pegr_nh, MPLS__MPLS_EXPf,
                            mpls_exp);

        /* Use EXP-PRI-map for PRI/CFI */
        soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_MAPPING_PTRf,
                            hw_map_idx);
    } else if ((label_action == _BCM_MPLS_ACTION_SWAP) && 
                    (mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_COPY) ) {
          soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_SELECTf,
                            0x3); /* USE_SWAP */
          soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXPf, mpls_exp);
          /* Use EXP-PRI-map for PRI/CFI */
          soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                            pegr_nh, MPLS__MPLS_EXP_MAPPING_PTRf,
                            hw_map_idx);
    }
    if (soc_feature(unit, soc_feature_mpls_ecn)) {
        if ((mpls_flags & BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP) && 
            (mpls_int_cn_map_id != ECN_MAP_ID_INVALID)) {
            int ecn_map_index;
            int ecn_map_type;
            int ecn_map_num;
            int ecn_map_hw_idx;
            ecn_map_type = mpls_int_cn_map_id & 
                            _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK;
            ecn_map_index = mpls_int_cn_map_id & 
                            _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK;
            ecn_map_num = 
                soc_mem_index_count(unit, EGR_INT_CN_TO_EXP_MAPPING_TABLEm) / 
                _BCM_ECN_MAX_ENTRIES_PER_MAP;
            if (ecn_map_type != _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP) {
                return BCM_E_PARAM;
            }
            if (ecn_map_index >= ecn_map_num) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                bcmi_ecn_map_id2hw_idx(unit, mpls_int_cn_map_id, &ecn_map_hw_idx));
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                pegr_nh, MPLS__INT_CN_TO_EXP_MAPPING_PTRf,
                                ecn_map_hw_idx);
            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    pegr_nh, MPLS__INT_CN_TO_EXP_PRIORITYf, 1);

            } else {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    pegr_nh, MPLS__INT_CN_TO_EXP_PRIORITYf, 0);
            }
        }
        if ((mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_TO_EXP_MAP) && 
            (mpls_ecn_map_id != ECN_MAP_ID_INVALID)) {
            int ecn_map_index;
            int ecn_map_type;
            int ecn_map_num;
            int ecn_map_hw_idx;
            ecn_map_type = mpls_ecn_map_id & 
                            _BCM_XGS5_MPLS_ECN_MAP_TYPE_MASK;
            ecn_map_index = mpls_ecn_map_id & 
                            _BCM_XGS5_MPLS_ECN_MAP_NUM_MASK;
            ecn_map_num = 
                soc_mem_index_count(unit, EGR_IP_ECN_TO_EXP_MAPPING_TABLEm) / 
                _BCM_ECN_MAX_ENTRIES_PER_MAP;
            if (ecn_map_type != _BCM_XGS5_MPLS_ECN_MAP_TYPE_ECN2EXP) {
                return BCM_E_PARAM;
            }
            if (ecn_map_index >= ecn_map_num) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN(
                bcmi_ecn_map_id2hw_idx(unit, mpls_ecn_map_id, &ecn_map_hw_idx));
            
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                pegr_nh, MPLS__IP_ECN_TO_EXP_MAPPING_PTRf,
                                ecn_map_hw_idx);
            if (mpls_flags & BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST) {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    pegr_nh, MPLS__IP_ECN_TO_EXP_PRIORITYf, 1);

            } else {
                soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm,
                                    pegr_nh, MPLS__IP_ECN_TO_EXP_PRIORITYf, 0);

            }

        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *       bcm_th3_mpls_l3_label_add
 * Purpose:
 *       Updates  Inner_LABEL for a specific EGR_NH Index
 * Parameters:
 *       IN :  Unit
 *       IN :  mpls_label
 *       IN :  nh_index
 * Returns:
 *       BCM_E_XXX
 */

int
bcm_th3_mpls_l3_label_add (int unit, bcm_l3_egress_t *egr, int nh_index, uint32 flags)
{
    int  rv=BCM_E_NONE;
    egr_l3_next_hop_entry_t egr_nh;
    uint32    entry_type;
    int       hw_map_idx, num_mpls_map;

    /* Retrieve EGR L3 NHOP Entry */
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    BCM_IF_ERROR_RETURN(bcm_tr_mpls_get_entry_type(unit, nh_index, &entry_type));
    if ((entry_type == 1) && (egr->mpls_label != BCM_MPLS_LABEL_INVALID) && (flags & BCM_L3_REPLACE)) {
        /* Be sure that the existing entry is already setup to
         * egress into an MPLS tunnel. If not, return BCM_E_PARAM.
         */
        entry_type = 
            soc_EGR_L3_NEXT_HOPm_field32_get(unit, &egr_nh, nh_entry_type_field);

        if (entry_type != 0x1) { /* != MPLS_MACDA_PROFILE */
            return BCM_E_PARAM;
        }

    }else if ((entry_type == 1) && (egr->mpls_label == BCM_MPLS_LABEL_INVALID) && (flags & BCM_L3_REPLACE)) {
        rv = bcm_tr_mpls_l3_label_delete (unit, nh_index);
        return rv;
    }

    /* Retrieve hardware index for mapping pointer */
    if ((egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
        (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {
        /* Mapping pointer not used */
        hw_map_idx = -1;
    } else {
        num_mpls_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
        rv = _egr_qos_id2hw_idx(unit,egr->mpls_qos_map_id,&hw_map_idx);
        if ((rv != BCM_E_NONE) || hw_map_idx < 0 || hw_map_idx >= num_mpls_map) {
            if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
                return rv = BCM_E_PARAM;
            } else {
                hw_map_idx = 0; /* use default */
            }
        }
    }

    /* Program the EGR_L3_NEXT_HOP table entry */
    rv = _bcm_th3_mpls_egr_nh_label_set(unit, NULL, egr,
                                             _BCM_MPLS_ACTION_PUSH, hw_map_idx,
                                             &egr_nh);
    if (rv < 0){
       return rv;
    }
    soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, 
                        &egr_nh, nh_entry_type_field, 
                        entry_type);

    if (soc_feature(unit, soc_feature_mpls_nh_ttl_control)) {
        if (egr->flags & BCM_L3_KEEP_TTL) {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MPLS__DISABLE_TTL_DECREMENTf, 1);
        } else {
            soc_mem_field32_set(unit, EGR_L3_NEXT_HOPm, &egr_nh,
                                MPLS__DISABLE_TTL_DECREMENTf, 0);
        }
    }
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);

    if (rv < 0) {
       return rv;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->scache_dirty = 1;
    SOC_CONTROL_UNLOCK(unit);
#endif

    return rv;

}

/*
 * Function:
 *      bcm_th3_mpls_swap_nh_info_add
 * Purpose:
 *       Add  SWAP_LABEL for a specific EGR_NH Index
 * Parameters:
 *           IN :  Unit
 *           IN : Egress object Pointer
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_th3_mpls_swap_nh_info_add (int unit, bcm_l3_egress_t *egr, int nh_index, uint32 flags)
{
    int  rv=BCM_E_NONE;
    int hw_map_idx, num_mpls_map;
    ing_l3_next_hop_entry_t ing_nh;
    egr_l3_next_hop_entry_t egr_nh;

    /* TH3 doesnt support EGR_MPLS_VC_AND_SWAP_LABEL_TABLEm*/

    sal_memset(&egr_nh, 0, sizeof(egr_l3_next_hop_entry_t));

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &ing_nh));
    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    /* Check for egr->flags & BCM_L3_REPLACE */
    if (flags & BCM_L3_REPLACE) {
        if (!(egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_TTL_SET)) {
            egr->mpls_ttl = soc_mem_field32_get(unit,
                    EGR_L3_NEXT_HOPm,
                    &egr_nh, MPLS__MPLS_TTLf);
            if (egr->mpls_ttl) {
                egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
            }
        }
    }
    /* Retrieve hardware index for mapping pointer */
    if ((egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_SET) ||
        (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_PRI_SET)) {
        /* Mapping pointer not used */
        hw_map_idx = -1;
    } else {
        num_mpls_map = soc_mem_index_count(unit, EGR_MPLS_EXP_MAPPING_1m) / 64;
        rv = _egr_qos_id2hw_idx(unit,egr->mpls_qos_map_id,&hw_map_idx);
        if ((rv != BCM_E_NONE) || hw_map_idx < 0 || hw_map_idx >= num_mpls_map) {
            if (egr->mpls_flags & BCM_MPLS_EGRESS_LABEL_EXP_REMARK) {
                return rv = BCM_E_PARAM;
            } else {
                hw_map_idx = 0; /* use default */
            }
        }
    }
    rv = _bcm_th3_mpls_egr_nh_label_set(unit, NULL,
              egr, _BCM_MPLS_EGRESS_LABEL_PRESERVE(unit, egr->mpls_flags) ?
              _BCM_MPLS_ACTION_PRESERVED : _BCM_MPLS_ACTION_SWAP,
              hw_map_idx, &egr_nh);

    BCM_IF_ERROR_RETURN(rv);

    /* Write EGR_L3_NEXT_HOP entry */
    rv = soc_mem_write(unit, EGR_L3_NEXT_HOPm,
                       MEM_BLOCK_ALL, nh_index, &egr_nh);
    BCM_IF_ERROR_RETURN(rv);

    /* Write ING_L3_NEXT_HOP entry */
    
    soc_mem_field32_set(unit, ING_L3_NEXT_HOPm,
                        &ing_nh, ENTRY_TYPEf, 0x1); /* MPLS L3_OIF */
    rv = soc_mem_write (unit, ING_L3_NEXT_HOPm,
                        MEM_BLOCK_ALL, nh_index, &ing_nh);
    return rv;
}

/*
 * Function:
 *        bcm_th3_mpls_swap_nh_info_get
 * Purpose:
 *        Get  Label_Swap info within Egress Object for a specific EGR_NH Index
 * Parameters:
 *           IN :  Unit
 *           OUT : Egress object Pointer
 *           IN :  nh_index
 * Returns:
 *           BCM_E_XXX
 */

int
bcm_th3_mpls_swap_nh_info_get(int unit, bcm_l3_egress_t *egr, int nh_index)
{
    egr_l3_next_hop_entry_t egr_nh;
    int ix;
    uint32	 label_action = 0;
    uint32 label_select = 0;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, EGR_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                                      nh_index, &egr_nh));

    label_action =  soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                &egr_nh, MPLS__MPLS_LABEL_ACTIONf);

    if (label_action == _BCM_MPLS_ACTION_SWAP ||
        (soc_feature(unit, soc_feature_mpls_swap_label_preserve) &&
        ((label_action == _BCM_MPLS_ACTION_PRESERVED) || label_select))) { /* SWAP */

        if (soc_feature(unit, soc_feature_mpls_swap_label_preserve) &&
            ((label_action == _BCM_MPLS_ACTION_PRESERVED) || label_select)) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRESERVE;
            egr->mpls_label = BCM_MPLS_LABEL_INVALID;
        } else {
            egr->mpls_label =
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__MPLS_LABELf);
        }
        egr->mpls_ttl =
            soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh,MPLS__MPLS_TTLf);
        if (egr->mpls_ttl) {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
        } else {
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        }
        if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                &egr_nh, MPLS__MPLS_EXP_SELECTf) == 0x0) {
            /* Use the specified EXP, PRI and CFI */
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_SET;
            egr->mpls_exp = 
                soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, MPLS__MPLS_EXPf);
            egr->mpls_pkt_pri = 
                soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, MPLS__NEW_PRIf);
            egr->mpls_pkt_cfi = 
                soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, MPLS__NEW_CFIf);
        } else if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                       &egr_nh, MPLS__MPLS_EXP_SELECTf) == 0x1) {
            /* Use EXP-map for EXP, PRI and CFI */
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_REMARK;
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
            ix = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                     &egr_nh, MPLS__MPLS_EXP_MAPPING_PTRf);
                BCM_IF_ERROR_RETURN(
                    _egr_qos_hw_idx2id(unit, ix, &egr->mpls_qos_map_id));
        } else if (soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                       &egr_nh, MPLS__MPLS_EXP_SELECTf) == 0x3) {
            /* Use EXP from incoming MPLS label. If no incoming label, use the specified
             * EXP value. Use EXP-map for PRI/CFI.
             */
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
            egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_PRI_REMARK;
            egr->mpls_exp = 
                soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                    &egr_nh, MPLS__MPLS_EXPf);
            ix = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                     &egr_nh, MPLS__MPLS_EXP_MAPPING_PTRf);
                BCM_IF_ERROR_RETURN(
                    _egr_qos_hw_idx2id(unit, ix, &egr->mpls_qos_map_id));
        }
        if (soc_feature(unit, soc_feature_mpls_ecn)) {
            int ecn_map_id;
            int ecn_map_pri;
            int ecn_map_hw_idx;
            int rv;
            ecn_map_hw_idx = soc_mem_field32_get(unit, 
                                EGR_L3_NEXT_HOPm, &egr_nh, 
                                MPLS__INT_CN_TO_EXP_MAPPING_PTRf);
            ecn_map_pri = soc_mem_field32_get(unit, 
                              EGR_L3_NEXT_HOPm, &egr_nh, 
                              MPLS__INT_CN_TO_EXP_PRIORITYf);
            rv = bcmi_ecn_map_hw_idx2id(unit, ecn_map_hw_idx, 
                                        _BCM_XGS5_MPLS_ECN_MAP_TYPE_INTCN2EXP, 
                                        &ecn_map_id); 
            if (BCM_SUCCESS(rv)) {
                egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_INT_CN_TO_EXP_MAP;
                egr->mpls_ecn_map_id = ecn_map_id;
                if (ecn_map_pri) {
                    egr->mpls_flags |= BCM_MPLS_EGRESS_LABEL_ECN_EXP_MAP_TRUST;
                }
            }
        }
    } else {
        egr->mpls_label = BCM_MPLS_LABEL_INVALID;
    }
    return BCM_E_NONE;
}


#endif
