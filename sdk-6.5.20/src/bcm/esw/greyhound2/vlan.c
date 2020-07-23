/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    vlan.c
 * Purpose: Handle Greyhound2 specific vlan features:
 *          Manages VFI VLAN membership tables.
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_GREYHOUND2_SUPPORT) && defined(INCLUDE_L3)
#include <bcm_int/common/multicast.h>

#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/greyhound2.h>

/*
 * Function:
 *    bcmi_gh2_vfi_control_params_check
 * Purpose:
 *    Verifies if input parameters are valid for the operation on the device
 * Parameters:
 *    unit         - (IN) Unit number
 *    vid          - (IN) Vlan id
 *    valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK
 *    control      - (IN) Configuration structure
 * Returns:
 *    BCM_E_PARAM for errorneous inputs
 *    BCM_E_NONE  if all memebers in the structure are legal
 */
STATIC int
bcmi_gh2_vfi_control_params_check(
    int unit,
    bcm_vlan_t vid,
    uint32 valid_fields,
    bcm_vlan_control_vlan_t *control)
{
    uint32 supported_fields = 0;
    uint32 supported_flags = 0;

    /*
     * VFI table in GH2-B0 supports only the following fields:
     *
     *  - L3_IIF           : Layer3 Incoming Interface to be assigned to the packet
     *  - UUC_UMC_BC_INDEX : Default unknown unicast/multicast/broadcast group
     *                       for this VFI
     */
    supported_fields = (BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK |
                        BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK |
                        BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK);

    /*
     *  - IPMCV4_L2_ENABLE  : Enables L2-only forwarding of IPMCv4 packets
     *  - IPMCV6_L2_ENABLE  : Enables L2-only forwarding of IPMCv6 packets
     *  - IPMCV4_ENABLE     : IPMC Enable for IPV4 packets on the VFI
     *  - IPMCV6_ENABLE     : IPMC Enable for IPV6 packets on the VFI
     *  - IPV4_ENABLE       : Enables IPV4 Routing in this VFI
     *  - IPV6_ENABLE       : Enables IPV6 Routing in this VFI
     *  - L2_NON_UCAST_DROP : Drop all broadcast and multicast packets
     *  - L2_NON_UCAST_TOCPU: Copy all broadcast and multicast packets to CPU
     *  - L2_DA_MISS_DROP   : Drop packet thas miss L2 DA unicast, Do not flood
     *  - L2_DA_MISS_TOCPU  : Copy packet thas miss L2 DA unicast to the CPU
     *  - L2_SA_MISS_TOCPU  : Copy packet thas miss L2 SA unicast to the CPU
     *    Note: L2_SA_MISS_TOCPU shares the same flags with L2_DA_MISS_TOCPU
     */
    supported_flags = (BCM_VLAN_IP4_MCAST_L2_DISABLE |
                       BCM_VLAN_IP6_MCAST_L2_DISABLE |
                       BCM_VLAN_IP4_MCAST_DISABLE |
                       BCM_VLAN_IP6_MCAST_DISABLE |
                       BCM_VLAN_IP4_DISABLE |
                       BCM_VLAN_IP6_DISABLE |
                       BCM_VLAN_NON_UCAST_DROP |
                       BCM_VLAN_NON_UCAST_TOCPU |
                       BCM_VLAN_UNKNOWN_UCAST_DROP |
                       BCM_VLAN_UNKNOWN_UCAST_TOCPU);

    if (valid_fields != BCM_VLAN_CONTROL_VLAN_ALL_MASK) {
        if ((~supported_fields) & valid_fields) {
            return BCM_E_PARAM;
        }
    }

    if ((~supported_flags) & control->flags) {
        return BCM_E_PARAM;
    }

    VLAN_CHK_ID(unit, control->egress_vlan);
    if (!soc_feature(unit, soc_feature_vp_sharing)) {
        if (control->egress_vlan != BCM_VLAN_NONE) {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    bcmi_gh2_vlan_control_vpn_set
 * Purpose:
 *    Configures various controls on the VFI
 * Parameters:
 *    unit         - (IN) Unit number
 *    vid          - (IN) Vlan id
 *    valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK
 *    control      - (IN) Configuration structure
 * Return:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vlan_control_vpn_set(
    int unit,
    bcm_vlan_t vid,
    uint32 valid_fields,
    bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    int vfi;
    int value;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;

    if (!soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check */
    if (NULL == control) {
        return BCM_E_PARAM;
    }

    rv = bcmi_gh2_vfi_control_params_check(unit, vid, valid_fields, control);
    BCM_IF_ERROR_RETURN(rv);

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    /* Read the VFI memory */
    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry));

    /* Update VFI table per vlan_control INPUT */

    /*
     * VFI table in GH2-B0 supports only the following fields:
     *
     *  - L3_IIF           : Layer3 Incoming Interface to be assigned to the packet
     *  - UUC_UMC_BC_INDEX : Default unknown unicast/multicast/broadcast group
     *                       for this VFI
     */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        if ((control->ingress_if < 0) ||
            (control->ingress_if >= BCM_XGS3_L3_ING_IF_TBL_SIZE(unit))) {
            return BCM_E_PARAM;
        } else {
            value = control->ingress_if;
            soc_mem_field32_set(unit, mem, &vfi_entry, L3_IIFf, value);
        }
    }

    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK ||
        valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK ||
        valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
        /* Unknown unicast/multicast/broadcast group share the same field */
        if (!(valid_fields &
            BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) ||
            !(valid_fields &
            BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) ||
            !(valid_fields &
            BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK)) {
            return BCM_E_PARAM;
        }

        if ((control->unknown_unicast_group !=
            control->unknown_multicast_group) ||
            (control->unknown_multicast_group !=
            control->broadcast_group)) {
            return BCM_E_PARAM;
        }

        if (_BCM_MULTICAST_IS_VXLAN(control->unknown_unicast_group)) {
            value = _BCM_MULTICAST_ID_GET(control->unknown_unicast_group);
            soc_mem_field32_set(unit, mem, &vfi_entry,
                                UUC_UMC_BC_INDEXf, value);
        } else {
            return BCM_E_PARAM;
        }
    }

    /*
     *  - IPMCV4_L2_ENABLE  : Enables L2-only forwarding of IPMCv4 packets
     *  - IPMCV6_L2_ENABLE  : Enables L2-only forwarding of IPMCv6 packets
     *  - IPMCV4_ENABLE     : IPMC Enable for IPV4 packets on the VFI
     *  - IPMCV6_ENABLE     : IPMC Enable for IPV6 packets on the VFI
     *  - IPV4_ENABLE       : Enables IPV4 Routing in this VFI
     *  - IPV6_ENABLE       : Enables IPV6 Routing in this VFI
     *  - L2_NON_UCAST_DROP : Drop all broadcast and multicast packets
     *  - L2_NON_UCAST_TOCPU: Copy all broadcast and multicast packets to CPU
     *  - L2_DA_MISS_DROP   : Drop packet thas miss L2 DA unicast, Do not flood
     *  - L2_DA_MISS_TOCPU  : Copy packet thas miss L2 DA unicast to the CPU
     *  - L2_SA_MISS_TOCPU  : Copy packet thas miss L2 SA unicast to the CPU
     *    Note: L2_SA_MISS_TOCPU shares the same flags with L2_DA_MISS_TOCPU
     */

    /* Enable L2-only forwarding of IPMCv4/IPMCv6 packets on this VLAN */
    value = (control->flags & BCM_VLAN_IP4_MCAST_L2_DISABLE) ? 0 : 1;
    soc_mem_field32_set(unit, mem, &vfi_entry, IPMCV4_L2_ENABLEf, value);
    value = (control->flags & BCM_VLAN_IP6_MCAST_L2_DISABLE) ? 0 : 1;
    soc_mem_field32_set(unit, mem, &vfi_entry, IPMCV6_L2_ENABLEf, value);


    /* IPMC Enable IPv4/IPv6 packets on this VLAN */
    value = (control->flags & BCM_VLAN_IP4_MCAST_DISABLE) ? 0 : 1;
    soc_mem_field32_set(unit, mem, &vfi_entry, IPMCV4_ENABLEf, value);
    value = (control->flags & BCM_VLAN_IP6_MCAST_DISABLE) ? 0 : 1;
    soc_mem_field32_set(unit, mem, &vfi_entry, IPMCV6_ENABLEf, value);


    /* Enables IPV4/IPV6 Routing in this VFI */
    value = (control->flags & BCM_VLAN_IP4_DISABLE) ? 0 : 1;
    soc_mem_field32_set(unit, mem, &vfi_entry, IPV4_ENABLEf, value);
    value = (control->flags & BCM_VLAN_IP6_DISABLE) ? 0 : 1;
    soc_mem_field32_set(unit, mem, &vfi_entry, IPV6_ENABLEf, value);


    /* Drop non-unicast, broadcast or multicast packets that miss L2 lookup */
    value = (control->flags & BCM_VLAN_NON_UCAST_DROP) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &vfi_entry, L2_NON_UCAST_DROPf, value);

    /*
     * Copy to cpu non-unicast, broadcast or multicast packets
     * that miss L2 lookup
     */
    value = (control->flags & BCM_VLAN_NON_UCAST_TOCPU) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &vfi_entry, L2_NON_UCAST_TOCPUf, value);

    /* Drop packets that miss L2 lookup */
    value = (control->flags & BCM_VLAN_UNKNOWN_UCAST_DROP) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &vfi_entry, L2_DA_MISS_DROPf, value);

    /* Copy to cpu packets that miss L2 lookup */
    value = (control->flags & BCM_VLAN_UNKNOWN_UCAST_TOCPU) ? 1 : 0;
    soc_mem_field32_set(unit, mem, &vfi_entry, L2_DA_MISS_TOCPUf, value);
    soc_mem_field32_set(unit, mem, &vfi_entry, L2_SA_MISS_TOCPUf, value);

    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, vfi, &vfi_entry);

    return rv;
}

/*
 * Function:
 *    bcmi_gh2_vlan_control_vpn_get
 * Purpose:
 *    Reads VFI table and fills vlan_control
 * Parameters:
 *    unit         - (IN) Unit number
 *    vid          - (IN) Vlan id
 *    valid_fields - (IN) Valid fields, BCM_VLAN_CONTROL_VLAN_XXX_MASK
 *    control      - (OUT) Configuration structure
 * Return:
 *    BCM_E_XXX
 */
int
bcmi_gh2_vlan_control_vpn_get(
    int unit,
    bcm_vlan_t vid,
    uint32 valid_fields,
    bcm_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    int vfi;
    int value;
    vfi_entry_t vfi_entry;
    soc_mem_t mem = VFIm;

    if (!soc_feature(unit, soc_feature_vxlan_lite_riot)) {
        return BCM_E_UNAVAIL;
    }

    /* Input parameters check */
    if (NULL == control) {
        return BCM_E_PARAM;
    }

    _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, vid);

    /* Read the VFI memory */
    SOC_IF_ERROR_RETURN(
        soc_mem_read(unit, mem, MEM_BLOCK_ANY, (int)vfi, &vfi_entry));

    /* Get L3 Ingress Interface ID */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_INGRESS_IF_MASK) {
        value = soc_mem_field32_get(unit, mem, &vfi_entry, L3_IIFf);
        control->ingress_if = value;
    }

    /* Get Unknown unicast/multicast/broadcast group ID */
    if (valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK ||
        valid_fields & BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK ||
        valid_fields & BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK) {
        /* Unknown unicast/multicast/broadcast group share the same field */
        if (!(valid_fields &
            BCM_VLAN_CONTROL_VLAN_UNKNOWN_UNICAST_GROUP_MASK) ||
            !(valid_fields &
            BCM_VLAN_CONTROL_VLAN_UNKNOWN_MULTICAST_GROUP_MASK) ||
            !(valid_fields &
            BCM_VLAN_CONTROL_VLAN_BROADCAST_GROUP_MASK)) {
            return BCM_E_PARAM;
        }

        value = soc_mem_field32_get(unit, mem, &vfi_entry, UUC_UMC_BC_INDEXf);
        _BCM_MULTICAST_GROUP_SET(control->unknown_unicast_group,
                                 _BCM_MULTICAST_TYPE_VXLAN, value);
        _BCM_MULTICAST_GROUP_SET(control->unknown_multicast_group,
                                 _BCM_MULTICAST_TYPE_VXLAN, value);
        _BCM_MULTICAST_GROUP_SET(control->broadcast_group,
                                 _BCM_MULTICAST_TYPE_VXLAN, value);
    }

    /* Enable L2-only forwarding of IPMCv4/IPMCv6 packets on this VLAN */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, IPMCV4_L2_ENABLEf);
    if (value == 0) {
        control->flags |= BCM_VLAN_IP4_MCAST_L2_DISABLE;
    }

    value = soc_mem_field32_get(unit, mem, &vfi_entry, IPMCV6_L2_ENABLEf);
    if (value == 0) {
        control->flags |= BCM_VLAN_IP6_MCAST_L2_DISABLE;
    }


    /* IPMC Enable IPv4/IPv6 packets on this VLAN */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, IPMCV4_ENABLEf);
    if (value == 0) {
        control->flags |= BCM_VLAN_IP4_MCAST_DISABLE;
    }

    value = soc_mem_field32_get(unit, mem, &vfi_entry, IPMCV6_ENABLEf);
    if (value == 0) {
        control->flags |= BCM_VLAN_IP6_MCAST_DISABLE;
    }


    /* Enables IPV4/IPV6 Routing in this VFI */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, IPV4_ENABLEf);
    if (value == 0) {
        control->flags |= BCM_VLAN_IP4_DISABLE;
    }

    value = soc_mem_field32_get(unit, mem, &vfi_entry, IPV6_ENABLEf);
    if (value == 0) {
        control->flags |= BCM_VLAN_IP6_DISABLE;
    }


    /* Drop non-unicast, broadcast or multicast packets that miss L2 lookup */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, L2_NON_UCAST_DROPf);
    if (value) {
        control->flags |= BCM_VLAN_NON_UCAST_DROP;
    }

    /*
     * Copy to cpu non-unicast, broadcast or multicast packets
     * that miss L2 lookup
     */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, L2_NON_UCAST_TOCPUf);
    if (value) {
        control->flags |= BCM_VLAN_NON_UCAST_TOCPU;
    }

    /* Drop packets that miss L2 lookup */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, L2_DA_MISS_DROPf);
    if (value) {
        control->flags |= BCM_VLAN_UNKNOWN_UCAST_DROP;
    }

    /* Copy to cpu packets that miss L2 lookup */
    value = soc_mem_field32_get(unit, mem, &vfi_entry, L2_DA_MISS_TOCPUf);
    if (value) {
        control->flags |= BCM_VLAN_UNKNOWN_UCAST_TOCPU;
    }
    value = soc_mem_field32_get(unit, mem, &vfi_entry, L2_SA_MISS_TOCPUf);
    if (value) {
        control->flags |= BCM_VLAN_UNKNOWN_UCAST_TOCPU;
    }

    return rv;
}

#endif /* BCM_GREYHOUND2_SUPPORT && INCLUDE_L3 */

