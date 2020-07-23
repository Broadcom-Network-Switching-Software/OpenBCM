/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    l3.c
 * Purpose: Greyhound2 L3 function implementations
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
#include <bcm/l3.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/greyhound2.h>

/*
 * Function:
 *    bcmi_gh2_l3_intf_qos_set
 * Purpose:
 *    Set L3 Interface QoS parameters
 * Parameters:
 *    unit          - (IN) Unit number
 *    l3_if_entry_p - (IN) Pointer to buffer
 *    intf_info     - (IN) Intf_info
 * Returns:
 *    BCM_E_XXX.
 */
int
bcmi_gh2_l3_intf_qos_set(
    int unit,
    uint32 *l3_if_entry_p,
    _bcm_l3_intf_cfg_t *intf_info)
{
    soc_mem_t mem;
    int hw_map_idx = 0;

    if (!soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check */
    if ((NULL == l3_if_entry_p) || (NULL == intf_info)) {
        return BCM_E_PARAM;
    }

    mem = EGR_L3_INTFm;

    /* GH2 doesn't support L3 Interface OVLAN/IVLAN QoS parameters */
    if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY ||
        intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET ||
        intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK ||
        intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_COPY ||
        intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_SET ||
        intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_REMARK) {
        return BCM_E_PARAM;
    }

    if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_COPY) {
        /* DSCP_SELf = 0x0: Do not modify DSCP */
        soc_mem_field32_set(unit, mem, l3_if_entry_p, DSCP_SELf, 0x0);
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_SET) {
        /* DSCP_SELf = 0x1: Use DSCP from this table */
        soc_mem_field32_set(unit, mem, l3_if_entry_p, DSCP_SELf, 0x1);
        soc_mem_field32_set(unit, mem, l3_if_entry_p,
                            DSCPf, intf_info->dscp_qos.dscp);
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_REMARK) {
        /* DSCP_SELf = 0x2: Use DSCP_MAPPING_PTR from this table */
        soc_mem_field32_set(unit, mem, l3_if_entry_p, DSCP_SELf, 0x2);

        BCM_IF_ERROR_RETURN(
            _bcm_tr2_qos_id2idx(unit, intf_info->dscp_qos.qos_map_id,
                                &hw_map_idx));
        soc_mem_field32_set(unit, mem, l3_if_entry_p,
                            DSCP_MAPPING_PTRf, hw_map_idx);
    } else {
        /* DSCP_SELf = 0x3: Pick up from packet */
        soc_mem_field32_set(unit, mem, l3_if_entry_p, DSCP_SELf, 0x3);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_l3_intf_qos_get
 * Purpose:
 *    Get L3 Interface QoS parameters
 * Parameters:
 *    unit          - (IN) Unit number
 *    l3_if_entry_p - (IN) Pointer to buffer
 *    intf_info     - (OUT) Intf_info
 * Returns:
 *     BCM_E_XXX.
 */
int
bcmi_gh2_l3_intf_qos_get(
    int unit,
    uint32 *l3_if_entry_p,
    _bcm_l3_intf_cfg_t *intf_info)
{
    soc_mem_t mem;
    uint32 dscp_sel = 0;
    int hw_map_idx = 0;

    if (!soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check */
    if ((NULL == l3_if_entry_p) || (NULL == intf_info)) {
        return BCM_E_PARAM;
    }

    mem = EGR_L3_INTFm;

    dscp_sel = soc_mem_field32_get(unit, mem, l3_if_entry_p, DSCP_SELf);

    if (dscp_sel == 0x0) {
        /* Do not modify DSCP */
        intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_COPY;
    } else if (dscp_sel == 0x1) {
        /* Use DSCP from this table */
        intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_SET;
        intf_info->dscp_qos.dscp =
            soc_mem_field32_get(unit, mem, l3_if_entry_p, DSCPf);
    } else if (dscp_sel == 0x2) {
        /* Use DSCP_MAPPING_PTR from this table */
        intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_REMARK;
        hw_map_idx =
            soc_mem_field32_get(unit, mem, l3_if_entry_p,
                                DSCP_MAPPING_PTRf);
        BCM_IF_ERROR_RETURN(
            _bcm_tr2_qos_idx2id(unit, hw_map_idx,
                                _BCM_QOS_MAP_TYPE_EGR_DSCP_TABLE,
                                &intf_info->dscp_qos.qos_map_id));
    }

    return BCM_E_NONE;
}

#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */
